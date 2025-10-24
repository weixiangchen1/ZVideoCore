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

    AVHWDeviceType hwType = AV_HWDEVICE_TYPE_DXVA2;
    // ֧�ֵ�Ӳ�����ٷ�ʽ
    for (int i = 0; ; i++) {
        const AVCodecHWConfig* pCodecHW = avcodec_get_hw_config(pCodec, i);
        if (pCodecHW == nullptr) {
            break;
        }
        if (pCodecHW->device_type) {
            std::cout << av_hwdevice_get_type_name(pCodecHW->device_type) << std::endl;
        }
    }
    // ��ʼ��Ӳ������������
    AVBufferRef* pHWCtx = nullptr;
    av_hwdevice_ctx_create(&pHWCtx, hwType, nullptr, nullptr, 0);
    pCodecCtx->hw_device_ctx = av_buffer_ref(pHWCtx);

    avcodec_open2(pCodecCtx, nullptr, nullptr);
    AVCodecParserContext* pCodecPCtx = av_parser_init(codeId);
    AVPacket* pPacket = av_packet_alloc();
    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pHWFrame = av_frame_alloc();       // ����Ӳ����ת��
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
                    AVFrame* pShowFrame = pFrame;
                    // Ӳ�����ٷ��ص����ݸ�ʽΪAV_PIX_FMT_DXVA2_VLD
                    if (pCodecCtx->hw_device_ctx) {
                        // Ӳ����ת�� GPU->CPU �Դ渴�Ƶ��ڴ�
                        // av_hwframe_transfer_data������ʽ��ΪAV_PIX_FMT_NV12
                        av_hwframe_transfer_data(pHWFrame, pFrame, 0);
                        pShowFrame = pHWFrame;
                    }
                    std::cout << pFrame->format << " " << std::flush;
                    std::cout << pHWFrame->format << " " << std::flush;
                    // ��һ֡��ʼ������
                    if (!isInit) {
                        pVideoView->Init(pShowFrame->width, pShowFrame->height, (ZVideoView::VideoFormat)pShowFrame->format);
                        isInit = true;
                    }
                    pVideoView->DrawFrame(pShowFrame);
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