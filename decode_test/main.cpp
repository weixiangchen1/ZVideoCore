#include <iostream>
#include <fstream>
#include "zvideoview.h"
#include "utils.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

int main(int argc, char* argv[]) {
    ZVideoView* pVideoView = ZVideoView::CreateVideoView();
    // 解析h264 存入AVPacket
    std::string strFile = "test.h264";
    std::ifstream file(strFile, std::ios::binary);
    if (!file) {
        return -1;
    }
    unsigned char szbuf[4096] = { 0 };

    // 分割上下文
    AVCodecID codeId = AV_CODEC_ID_H264;
    AVCodec* pCodec = avcodec_find_decoder(codeId);
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
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
                iRet = avcodec_send_packet(pCodecCtx, pPacket);
                if (iRet < 0) {
                    break;
                }
                // 获取多帧解码数据
                while (iRet >= 0) {
                    // avcodec_receive_frame每次会调用av_frame_unref释放空间
                    iRet = avcodec_receive_frame(pCodecCtx, pFrame);
                    if (iRet < 0) {
                        break;
                    }
                    std::cout << pFrame->format << " " << std::flush;
                    // 第一帧初始化窗口
                    if (!isInit) {
                        pVideoView->Init(pFrame->width, pFrame->height, (ZVideoView::VideoFormat)pFrame->format);
                        isInit = true;
                    }
                    pVideoView->DrawFrame(pFrame);
                    iCount++;
                    long long lEndTime = Utils::GetCurrentTimestamp();
                    if (lEndTime - lBeginTime >= 100) {
                        std::cout << "\nfps = " << iCount*10 << std::endl;
                        iCount = 0;
                        lBeginTime = Utils::GetCurrentTimestamp();
                    }
                }
            }
        }
    }

    // 取出缓存中的AVFrame
    int iRet = avcodec_send_packet(pCodecCtx, nullptr);
    while (iRet >= 0) {
        iRet = avcodec_receive_frame(pCodecCtx, pFrame);
        if (iRet < 0) {
            break;
        }
        std::cout << "(" << pFrame->format << ")" << std::flush;
        pVideoView->DrawFrame(pFrame);
    }

    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    avcodec_free_context(&pCodecCtx);
    av_parser_close(pCodecPCtx);
    return 0;
}