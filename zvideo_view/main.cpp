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

    // ����10s��Ƶ 250֡
    AVFrame* pFrame = encode.CreateFrame();
    int iCount = 0;   // д���ļ���֡�� SPS PPS IDR ��ͬһ֡
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
    // ����h264 ����AVPacket
    std::string strFile = "test.h264";
    std::ifstream file(strFile, std::ios::binary);
    if (!file) {
        return;
    }
    unsigned char szbuf[4096] = { 0 };
    // �ָ�������
    AVCodecID codeId = AV_CODEC_ID_H264;
    ZDecode decode;
    AVCodecContext* pCodecCtx = decode.CreateCodecContext(codeId, 1);
    decode.SetCodecContext(pCodecCtx);

    // ��ʼ��Ӳ������������
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
            // ͨ��0001�ָ�NALU�������AVPacket
            // av_parser_parse2 ������Ҫ�����֡ ����ֵΪ��ǰ�����֡��С
            // �ɹ��ָ����޸�buf_sizeֵ��ʧ����Ϊ0
            int iRet = av_parser_parse2(pCodecPCtx, pCodecCtx,
                &pPacket->data, &pPacket->size,
                pData, iDataSize,
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            pData += iRet;
            iDataSize -= iRet;
            if (pPacket->size) {
                // std::cout << pPacket->size << " " << std::flush;
                // ����AVPacket�������߳�
                if (!decode.SendPacket(pPacket)) {
                    break;
                }

                // ��ȡ��֡��������
                while (decode.RecvFrame(pFrame)) {
                    // ��һ֡��ʼ������
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

    // ȡ�������е�AVFrame
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

void TestZFormat(std::string infile, std::string outfile, int beginTime, int endTime, int width, int height) {
    // ���װ
    ZDemux demux;
    AVFormatContext* pFormatCtx = demux.CreateDemuxContext(infile.c_str());
    demux.SetFormatContext(pFormatCtx);

    const char* strOutURL = "out1.mp4";
    // ��װ
    ZMux mux;
    AVFormatContext* pFormatECtx = mux.CreateMuxContext(outfile.c_str());
    mux.SetFormatContext(pFormatECtx);

    long long lVideoBeginPts = 0;
    long long lAudioBeginPts = 0;
    long long lVideoEndPts = 0;
    if (beginTime > 0) {
        if (demux.GetVideoIndex() >= 0 && demux.GetVideoTimeBase().num > 0) {
            lVideoBeginPts = beginTime * (double)demux.GetVideoTimeBase().den / (double)demux.GetVideoTimeBase().num;
            lVideoEndPts = endTime * (double)demux.GetVideoTimeBase().den / (double)demux.GetVideoTimeBase().num;
            demux.SeekFrame(lVideoBeginPts, demux.GetVideoIndex());
        }
        if (demux.GetAudioIndex() >= 0 && demux.GetAudioTimeBase().num > 0) {
            lAudioBeginPts = beginTime * (double)demux.GetAudioTimeBase().den / (double)demux.GetAudioTimeBase().num;
            demux.SeekFrame(lAudioBeginPts, demux.GetAudioIndex());
        }
    }

    // ��Ƶ��������ʼ��
    ZDecode decode;
    AVCodecContext* pDCodecCtx = decode.CreateCodecContext(demux.GetVideoCodecId(), 1);
    demux.CopyParam(demux.GetVideoIndex(), pDCodecCtx);
    decode.SetCodecContext(pDCodecCtx);
    decode.OpenCodec();

    // ��Ƶ��������ʼ��
    int iVideoWidth = width;
    int iVideoHeight = height;
    if (demux.GetVideoIndex() >= 0) {
        if (iVideoHeight <= 0 || iVideoHeight <= 0) {
            iVideoWidth = pFormatECtx->streams[demux.GetVideoIndex()]->codecpar->width;
            iVideoHeight = pFormatECtx->streams[demux.GetVideoIndex()]->codecpar->height;
        }
    }
    ZEncode encode;
    AVCodecContext* pECodecCtx = encode.CreateCodecContext((int)AV_CODEC_ID_H265, 0);
    pECodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    //pECodecCtx->width = iVideoWidth;
    //pECodecCtx->height = iVideoHeight;
    pECodecCtx->width = 1920;
    pECodecCtx->height = 1080;
    encode.SetCodecContext(pECodecCtx);
    encode.OpenCodec();

    AVStream* pVideoStream = pFormatECtx->streams[mux.GetVideoIndex()];
    AVStream* pAudioStream = pFormatECtx->streams[mux.GetAudioIndex()];

    if (demux.GetVideoIndex() >= 0) {
        pVideoStream->time_base.den = demux.GetVideoTimeBase().den;
        pVideoStream->time_base.num = demux.GetVideoTimeBase().num;

        // demux.CopyParam(demux.GetVideoIndex(), pVideoStream->codecpar);
        avcodec_parameters_from_context(pVideoStream->codecpar, pECodecCtx);
    }
    if (demux.GetAudioIndex() >= 0) {
        pAudioStream->time_base.den = demux.GetAudioTimeBase().den;
        pAudioStream->time_base.num = demux.GetAudioTimeBase().num;

        demux.CopyParam(demux.GetAudioIndex(), pAudioStream->codecpar);
    }

    mux.WriteHead();

    AVFrame* pFrame = decode.CreateFrame();
    AVPacket* pPacket = av_packet_alloc();
    while (demux.ReadFrame(pPacket)) {
        if (pPacket->stream_index == demux.GetVideoIndex()) {
            mux.RescaleTimeParam(pPacket, lVideoBeginPts, demux.GetVideoTimeBase());
            if (lVideoEndPts > 0 && pPacket->pts > lVideoEndPts) {
                av_packet_unref(pPacket);
                break;
            }
            if (decode.SendPacket(pPacket)) {
                while (true) {
                    int iRet = decode.RecvFrame(pFrame);
                    if (iRet == 1) {
                        std::cout << "." << std::flush;
                        AVPacket* pEPacket = encode.EncodeData(pFrame);
                        if (pEPacket) {
                            mux.WriteFrame(pEPacket);
                            av_packet_free(&pEPacket);
                        }
                    } else break;
                }
            }
        } else if (pPacket->stream_index == demux.GetAudioIndex()) {
            mux.RescaleTimeParam(pPacket, lAudioBeginPts, demux.GetAudioTimeBase());
            mux.WriteFrame(pPacket);
        } else {
            av_packet_unref(pPacket);
        }
    }

    mux.WriteTail();

    demux.SetFormatContext(nullptr);
    mux.SetFormatContext(nullptr);
    encode.SetCodecContext(nullptr);
    decode.SetCodecContext(nullptr);
    av_packet_free(&pPacket);
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

    std::string strUseage = "input file | output file | begin time | end time | width | height\n";
    strUseage += "eg: v1080.mp4 out.mp4 10 20 400 300 \n";
    std::cout << strUseage;

    if (argc < 3) {
        return -1;
    }
    std::string strInFile = argv[1];
    std::string strOutFile = argv[2];
    int iBeginTime = 0;
    int iEndTime = 0;
    int iWidth = 0;
    int iHeight = 0;
    if (argc > 3) {
        iBeginTime = atoi(argv[3]);
    }
    if (argc > 4) {
        iEndTime = atoi(argv[4]);
    }
    if (argc > 6) {
        iWidth = atoi(argv[5]);
        iHeight = atoi(argv[6]);
    }

    TestZFormat(strInFile, strOutFile, iBeginTime, iEndTime, iWidth, iHeight);
    return 0;
}
