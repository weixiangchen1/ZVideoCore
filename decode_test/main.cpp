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

    AVHWDeviceType hwType = AV_HWDEVICE_TYPE_DXVA2;
    // 支持的硬件加速方式
    for (int i = 0; ; i++) {
        const AVCodecHWConfig* pCodecHW = avcodec_get_hw_config(pCodec, i);
        if (pCodecHW == nullptr) {
            break;
        }
        if (pCodecHW->device_type) {
            std::cout << av_hwdevice_get_type_name(pCodecHW->device_type) << std::endl;
        }
    }
    // 初始化硬件加速上下文
    AVBufferRef* pHWCtx = nullptr;
    av_hwdevice_ctx_create(&pHWCtx, hwType, nullptr, nullptr, 0);
    pCodecCtx->hw_device_ctx = av_buffer_ref(pHWCtx);

    avcodec_open2(pCodecCtx, nullptr, nullptr);
    AVCodecParserContext* pCodecPCtx = av_parser_init(codeId);
    AVPacket* pPacket = av_packet_alloc();
    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pHWFrame = av_frame_alloc();       // 用于硬解码转换
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
                    AVFrame* pShowFrame = pFrame;
                    // 硬件加速返回的数据格式为AV_PIX_FMT_DXVA2_VLD
                    if (pCodecCtx->hw_device_ctx) {
                        // 硬解码转换 GPU->CPU 显存复制到内存
                        // av_hwframe_transfer_data处理后格式变为AV_PIX_FMT_NV12
                        av_hwframe_transfer_data(pHWFrame, pFrame, 0);
                        pShowFrame = pHWFrame;
                    }
                    std::cout << pFrame->format << " " << std::flush;
                    std::cout << pHWFrame->format << " " << std::flush;
                    // 第一帧初始化窗口
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