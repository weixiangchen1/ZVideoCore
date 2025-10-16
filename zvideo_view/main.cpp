#include "zvideoviewtest.h"
#include <QtWidgets/QApplication>
#include <fstream>
#include "zcodec.h"
#include "zdecode.h"
#include "zencode.h"
#include "zmux.h"
#include "zdemux.h"
#include "zvideoview.h"
#include "utils.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")

void TestZEncode(AVCodecID id) {
    std::string strFile = "400_300_25_1";
    if (id == AV_CODEC_ID_H264) {
        strFile += ".h264";
    } else if (id == AV_CODEC_ID_HEVC) {
        strFile += ".h265";
    }
    std::ofstream file;
    file.open(strFile, std::ios::binary);

    ZEncode encode;
    AVCodecContext* pCodecCtx = encode.CreateCodecContext(id, 0);
    pCodecCtx->width = 400;
    pCodecCtx->height = 300;
    encode.SetCodecContext(pCodecCtx);
    encode.SetAVOpt("crf", 18);
    encode.OpenCodec();

    // 发生10s视频 250帧
    AVFrame* pFrame = encode.CreateFrame();
    int iCount = 0;   // 写入文件的帧数 SPS PPS IDR 在同一帧
    for (int i = 0; i < 250; ++i) {
        // Y
        for (int y = 0; y < pCodecCtx->height; ++y) {
            for (int x = 0; x < pCodecCtx->width; ++x) {
                pFrame->data[0][y * pFrame->linesize[0] + x] = x + y + i * 3;
            }
        }
        // UV
        for (int y = 0; y < pCodecCtx->height / 2; ++y) {
            for (int x = 0; x < pCodecCtx->width / 2; ++x) {
                pFrame->data[1][y * pFrame->linesize[1] + x] = 128 + y + i * 2;
                pFrame->data[2][y * pFrame->linesize[2] + x] = 64 + y + i * 5;
            }
        }
        pFrame->pts = i;
        AVPacket* pPacket = encode.EncodeData(pFrame);
        if (pPacket) {
            iCount++;
            file.write((char*)pPacket->data, pPacket->size);
            av_packet_free(&pPacket);
        }
    }

    std::vector<AVPacket*> packets = encode.GetCachePackets();
    for (auto packet : packets) {
        iCount++;
        file.write((char*)packet->data, packet->size);
        av_packet_free(&packet);
    }

    file.close();
    encode.SetCodecContext(nullptr);
    std::cout << "count: " << iCount << std::endl;
}

void TestZDecode() {
    ZVideoView* pVideoView = ZVideoView::CreateVideoView();
    // 解析h264 存入AVPacket
    std::string strFile = "test.h264";
    std::ifstream file(strFile, std::ios::binary);
    if (!file) {
        return;
    }
    unsigned char szbuf[4096] = { 0 };
    // 分割上下文
    AVCodecID codeId = AV_CODEC_ID_H264;
    ZDecode decode;
    AVCodecContext* pCodecCtx = decode.CreateCodecContext(codeId, 1);
    decode.SetCodecContext(pCodecCtx);

    // 初始化硬件加速上下文
    decode.InitHWA((int)AV_HWDEVICE_TYPE_DXVA2);

    avcodec_open2(pCodecCtx, nullptr, nullptr);
    AVCodecParserContext* pCodecPCtx = av_parser_init(codeId);
    AVPacket* pPacket = av_packet_alloc();
    AVFrame* pFrame = av_frame_alloc();
    long long lBeginTime = Utils::GetCurrentTimestamp();
    int iCount = 0;
    bool isInit = false;

    while (!file.eof()) {
        file.read((char*)szbuf, sizeof(szbuf));
        int iDataSize = file.gcount();
        if (iDataSize <= 0) {
            break;
        }

        unsigned char* pData = szbuf;
        while (iDataSize > 0) {
            // 通过0001分割NALU流输出到AVPacket
            // av_parser_parse2 可能需要处理多帧 返回值为当前处理的帧大小
            // 成功分隔会修改buf_size值，失败置为0
            int iRet = av_parser_parse2(pCodecPCtx, pCodecCtx,
                &pPacket->data, &pPacket->size,
                pData, iDataSize,
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            pData += iRet;
            iDataSize -= iRet;
            if (pPacket->size) {
                // std::cout << pPacket->size << " " << std::flush;
                // 发送AVPacket到解码线程
                if (!decode.SendPacket(pPacket)) {
                    break;
                }

                // 获取多帧解码数据
                while (decode.RecvFrame(pFrame)) {
                    // 第一帧初始化窗口
                    if (!isInit) {
                        pVideoView->Init(pFrame->width, pFrame->height, (ZVideoView::VideoFormat)pFrame->format);
                        isInit = true;
                    }
                    pVideoView->DrawFrame(pFrame);
                    iCount++;
                    long long lEndTime = Utils::GetCurrentTimestamp();
                    if (lEndTime - lBeginTime >= 100) {
                        std::cout << "\nfps = " << iCount * 10 << std::endl;
                        iCount = 0;
                        lBeginTime = Utils::GetCurrentTimestamp();
                    }
                }
            }
        }
    }

    // 取出缓存中的AVFrame
    std::vector<AVFrame*> vecFrames = decode.GetCacheFrames();
    for (int i = 0; i < vecFrames.size(); ++i) {
        std::cout << "(" << vecFrames[i]->format << ")" << std::flush;
        pVideoView->DrawFrame(vecFrames[i]);
    }

    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    avcodec_free_context(&pCodecCtx);
    av_parser_close(pCodecPCtx);
}

void TestZFormat() {
    const char* strURL = "v1080.mp4";
    // 解封装
    ZDemux demux;
    AVFormatContext* pFormatCtx = demux.CreateDemuxContext(strURL);
    demux.SetFormatContext(pFormatCtx);

    const char* strOutURL = "out1.mp4";
    // 封装
    ZMux mux;
    AVFormatContext* pFormatECtx = mux.CreateMuxContext(strOutURL);
    mux.SetFormatContext(pFormatECtx);
    AVStream* pVideoStream = pFormatECtx->streams[mux.GetVideoIndex()];
    AVStream* pAudioStream = pFormatECtx->streams[mux.GetAudioIndex()];

    if (demux.GetVideoIndex() >= 0) {
        pVideoStream->time_base.den = demux.GetVideoTimeBase().den;
        pVideoStream->time_base.num = demux.GetVideoTimeBase().num;

        demux.CopyParam(demux.GetVideoIndex(), pVideoStream->codecpar);
    }
    if (demux.GetAudioIndex() >= 0) {
        pAudioStream->time_base.den = demux.GetAudioTimeBase().den;
        pAudioStream->time_base.num = demux.GetAudioTimeBase().num;

        demux.CopyParam(demux.GetAudioIndex(), pAudioStream->codecpar);
    }

    mux.WriteHead();

    AVPacket* pPacket = av_packet_alloc();
    while (demux.ReadFrame(pPacket)) {
        mux.WriteFrame(pPacket);
    }

    mux.WriteTail();

    demux.SetFormatContext(nullptr);
    mux.SetFormatContext(nullptr);
}

int main(int argc, char *argv[]) {
    //QApplication app(argc, argv);
    //ZVideoViewTest window;
    //window.show();
    //return app.exec();

    //AVCodecID codecId = AV_CODEC_ID_H264;
    //if (argc > 1) {
    //    std::string strCodec = argv[1];
    //    if (strCodec == "h265" || strCodec == "hevc") {
    //        codecId = AV_CODEC_ID_HEVC;
    //    }
    //}
    //TestZEncode(codecId);
    //TestZDecode();
    TestZFormat();
    return 0;
}
