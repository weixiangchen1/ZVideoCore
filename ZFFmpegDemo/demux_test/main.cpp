#include <iostream>
#include "zdecode.h"
#include "zencode.h"
#include "zvideoview.h"
#include "utils.h"
extern "C" {
#include <libavformat/avformat.h>
}
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")


int main() {
    // 打开媒体文件
    const char* strURL = "v1080.mp4";
    // 解封装上下文
    AVFormatContext* pFmtCtx = nullptr;
    int iRet = avformat_open_input(&pFmtCtx, strURL,
        nullptr,
        nullptr);
    if (iRet != 0) {
        std::string strErrMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "avformat_open_input error: " << strErrMsg.c_str() << std::endl;
        return -1;
    }


    iRet = avformat_find_stream_info(pFmtCtx, nullptr);
    if (iRet != 0) {
        std::string strErrMsg = Utils::GetAVErrorMessage(iRet);
        std::cerr << "avformat_find_stream_info error: " << strErrMsg.c_str() << std::endl;
        return -1;
    }
    av_dump_format(pFmtCtx, 0, strURL, 0);


    AVStream* pAudioStream = nullptr;
    AVStream* pVideoStream = nullptr;
    for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
        if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            pAudioStream = pFmtCtx->streams[i];
            std::cout << "============audio============" << std::endl;
            std::cout << "sample_rate: " << pAudioStream->codecpar->sample_rate << std::endl;
        }
        else if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            pVideoStream = pFmtCtx->streams[i];
            std::cout << "============video============" << std::endl;
            std::cout << "width: " << pVideoStream->codecpar->width << " height: " << pVideoStream->codecpar->height << std::endl;
        }
    }


    AVCodecID videoCodeId = AV_CODEC_ID_H264;
    AVCodecParameters* videoParamter = nullptr;
    if (pVideoStream != nullptr) {
        videoCodeId = pVideoStream->codecpar->codec_id;
        videoParamter = pVideoStream->codecpar;
    }


    // 视频解码器初始化
    ZDecode zdecode;
    AVCodecContext* pCodecCtx = ZCodec::CreateCodecContext((int)videoCodeId, 1);
    avcodec_parameters_to_context(pCodecCtx, videoParamter);
    zdecode.SetCodecContext(pCodecCtx);
    if (!zdecode.OpenCodec()) {
        std::cerr << "open decodec failed" << std::endl;
        return -2;
    }

    // 视频渲染器初始化
    ZVideoView* pVideoView = ZVideoView::CreateVideoView();
    pVideoView->Init(videoParamter->width, videoParamter->height, ZVideoView::VideoFormat::YUV420P);

    AVFrame* pFrame = zdecode.CreateFrame();
    AVPacket* packet = av_packet_alloc();
    while (av_read_frame(pFmtCtx, packet) >= 0) {
        if (pVideoStream && packet->stream_index == pVideoStream->index) {
            std::cout << "video packet size: " << packet->size << std::endl;
            if (zdecode.SendPacket(packet)) {
                while (true) {
                    int ret = zdecode.RecvFrame(pFrame);
                    if (ret == 1) {
                        std::cout << "decoded frame: " << pFrame->width << "x" << pFrame->height << std::endl;
                        pVideoView->DrawFrame(pFrame);
                    } else if (ret == 0) {
                        // 需要更多数据，跳出取帧循环，继续读取下一个 packet
                        break;
                    } else {
                        // 错误或 EOF
                        std::cerr << "RecvFrame returned error or EOF" << std::endl;
                        break;
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    av_frame_free(&pFrame);
    avformat_close_input(&pFmtCtx);
    return 0;
}