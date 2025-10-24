#include <iostream>
#include "zdecode.h"
#include "zvideoview.h"
extern "C" {
#include <libavformat/avformat.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")


std::string GetAVErrorMessage(int iErrorId) {
    char strMsg[1024] = { 0 };
    av_strerror(iErrorId, strMsg, sizeof(strMsg) - 1);
    return std::string(strMsg);
}

#define AVLOG(ret) if (ret != 0) { std::cerr << GetAVErrorMessage(ret).c_str() << std::endl; return -1; }

int main() {
    /////////////////////////////////////////////////////////////////////
    // 解封装
    const char* strURL = "v1080.mp4";
    // 获取解封装上下文 打开输入文件 获取基本信息
    AVFormatContext* pFormatCtx = nullptr;
    int iRet = avformat_open_input(&pFormatCtx, strURL, nullptr, nullptr);
    AVLOG(iRet);

    // 解析流信息
    iRet = avformat_find_stream_info(pFormatCtx, nullptr);
    AVLOG(iRet);

    // 打印媒体文件详细信息
    av_dump_format(pFormatCtx, 0, strURL, 0);

    AVStream* pAudioStream = nullptr;
    AVStream* pVideoStream = nullptr;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            pAudioStream = pFormatCtx->streams[i];
        }
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            pVideoStream = pFormatCtx->streams[i];
        }
    }

    AVCodecID videoCodecId = AV_CODEC_ID_H264;
    AVCodecParameters* pVideoParam = nullptr;
    if (pVideoStream) {
        videoCodecId = pVideoStream->codecpar->codec_id;
        pVideoParam = pVideoStream->codecpar;
    }

    ZDecode zdecode;
    AVCodecContext* pCodecCtx = zdecode.CreateCodecContext((int)videoCodecId, 1);
    avcodec_parameters_to_context(pCodecCtx, pVideoParam);
    zdecode.SetCodecContext(pCodecCtx);
    if (!zdecode.OpenCodec()) {
        std::cerr << "open decodec failed" << std::endl;
        return -1;
    }

    /////////////////////////////////////////////////////////////////////
    // 封装
    AVFormatContext* pFormatECtx = nullptr;
    const char* strOutURL = "out.mp4";
    iRet = avformat_alloc_output_context2(&pFormatECtx, nullptr, nullptr, strOutURL);
    AVLOG(iRet);

    // 添加音频流和视频流
    AVStream* pMVideoStream = avformat_new_stream(pFormatECtx, nullptr);    // 视频流
    AVStream* pMAudioStream = avformat_new_stream(pFormatECtx, nullptr);    // 音频流

    // 打开输出IO
    iRet = avio_open(&pFormatECtx->pb, strOutURL, AVIO_FLAG_WRITE);
    AVLOG(iRet);

    // 设置编码音视频参数
    if (pVideoStream) {
        pMVideoStream->time_base = pVideoStream->time_base;                         // 时间基数与原视频一致
        avcodec_parameters_copy(pMVideoStream->codecpar, pVideoStream->codecpar);   // 从解封装复制参数
    }
    if (pAudioStream) {
        pMAudioStream->time_base = pAudioStream->time_base;
        avcodec_parameters_copy(pMAudioStream->codecpar, pAudioStream->codecpar);
    }

    // 写入文件头
    iRet = avformat_write_header(pFormatECtx, nullptr);
    AVLOG(iRet);

    // 打印媒体文件详细信息
    av_dump_format(pFormatECtx, 0, strURL, 1);

    // 截取10~20s的音视频流 以视频的时间轴为基准定义
    // pts：帧解码后显示的时间（观众看到的时间）
    // dts：帧解码的时间（解码器处理的时间）
    // time_base: 时间基 time_base={1，25} => 把1秒分为25等份
    // duration: 通常指的是媒体文件的总时长
    // 对于无 B 帧（双向预测帧）的流（如音频、部分视频），DTS 和 PTS 通常相同；
    // 对于有 B 帧的视频，因 B 帧依赖前后帧解码，DTS 和 PTS 可能不同（PTS 可能大于或小于 DTS）
    double beginTime = 10.0;
    double endTime = 20.0;
    long long beginVideoPts = 0;
    long long beginAudioPts = 0;
    long long endPts = 0;
    // 计算out.mp4的开始pts和结束pts
    if (pVideoStream && pVideoStream->time_base.num > 0) {
        // pts = beginTime / time_base (time_base = num / den)
        // pts = beginTime / (time_base.num / time_base.den)
        // pts = beginTime * time_base.den / time_base.num
        beginVideoPts = beginTime * pVideoStream->time_base.den / pVideoStream->time_base.num;
        endPts = endTime * pVideoStream->time_base.den / pVideoStream->time_base.num;
    }
    if (pAudioStream && pAudioStream->time_base.num > 0) {
        beginAudioPts = beginTime * pAudioStream->time_base.den / pAudioStream->time_base.num;
    }

    // 通过av_seek_frame 移动到10s的关键帧位置
    // AVSEEK_FLAG_BACKWARD：允许定位到小于等于目标时间戳的最近关键帧（默认行为）。
    // AVSEEK_FLAG_BYTE：基于文件字节位置而非时间戳 seek（较少用）。
    // AVSEEK_FLAG_ANY：允许定位到非关键帧（可能导致解码花屏，需配合后续重新同步）。
    // AVSEEK_FLAG_FRAME：按帧索引 seek（timestamp 表示帧索引，而非时间戳）。
    if (pVideoStream) {
        iRet = av_seek_frame(pFormatCtx, pVideoStream->index, beginVideoPts,
                    AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);      // 向后关键帧
        AVLOG(iRet);
    }

    AVPacket* pPacket = av_packet_alloc();
    AVFrame* pFrame = zdecode.CreateFrame();
    while (av_read_frame(pFormatCtx, pPacket) >= 0) {
        AVStream* pInStream = pFormatCtx->streams[pPacket->stream_index];
        AVStream* pOutStream = nullptr;
        long long lOffset = 0;      // 截取的音频或视频流的偏移pts
        if (pVideoStream && pPacket->stream_index == pVideoStream->index) {
            // 视频流
            lOffset = beginVideoPts;
            pOutStream = pFormatECtx->streams[0];
            // 超过20s退出循环
            // 音频流和视频流的时序是对齐的（通过 PTS/DTS 同步）
            // 视频流的endPts（20s 对应的视频时间戳）已经隐含了音频流的对应结束位置
            if (pPacket->dts > endPts) {
                av_packet_unref(pPacket);   // 未执行到av_interleaved_write_frame 需要释放资源
                break;
            }
        } else if (pAudioStream && pPacket->stream_index == pAudioStream->index) {
            // 音频流
            lOffset = beginAudioPts;
            pOutStream = pFormatECtx->streams[1];
        }

        std::cout << pPacket->pts << " : " << pPacket->dts << " :" << pPacket->size << std::endl;
        // 重新计算packet的pts dts duration
        // 输出时间戳 = (输入时间戳 - 偏移量) × (输入时间基 / 输出时间基)
        if (pOutStream) {
            pPacket->pts = av_rescale_q_rnd(pPacket->pts - lOffset, pInStream->time_base,
                pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pPacket->dts = av_rescale_q_rnd(pPacket->dts - lOffset, pInStream->time_base,
                pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pPacket->duration = av_rescale_q(pPacket->duration, pInStream->time_base, pOutStream->time_base);
        }
        pPacket->pos = -1;

        // 向文件中写入音视频流 执行完成会自动清理packet
        iRet = av_interleaved_write_frame(pFormatECtx, pPacket);
        if (iRet != 0) {
            std::cerr << GetAVErrorMessage(iRet).c_str() << std::endl;
        }
    }

    // 写入文件尾部数据
    iRet = av_write_trailer(pFormatECtx);
    if (iRet != 0) {
        std::cerr << GetAVErrorMessage(iRet).c_str() << std::endl;
    }

    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    avformat_close_input(&pFormatCtx);
    avio_close(pFormatECtx->pb);
    avformat_free_context(pFormatECtx);
    pFormatECtx = nullptr;
    return 0;
}