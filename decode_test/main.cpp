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
    // ����h264 ����AVPacket
    std::string strFile = "test.h264";
    std::ifstream file(strFile, std::ios::binary);
    if (!file) {
        return -1;
    }
    unsigned char szbuf[4096] = { 0 };

    // �ָ�������
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
                iRet = avcodec_send_packet(pCodecCtx, pPacket);
                if (iRet < 0) {
                    break;
                }
                // ��ȡ��֡��������
                while (iRet >= 0) {
                    // avcodec_receive_frameÿ�λ����av_frame_unref�ͷſռ�
                    iRet = avcodec_receive_frame(pCodecCtx, pFrame);
                    if (iRet < 0) {
                        break;
                    }
                    std::cout << pFrame->format << " " << std::flush;
                    // ��һ֡��ʼ������
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

    // ȡ�������е�AVFrame
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