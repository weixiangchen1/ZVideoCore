#include <iostream>
#include <fstream>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

int main(int argc, char* argv[]) {
    std::string strFile = "400_300_25_preset";
    AVCodecID codecId = AV_CODEC_ID_H264;
    if (argc > 1) {
        std::string strCodec = argv[1];
        if (strCodec == "h265" || strCodec == "hevc") {
            codecId = AV_CODEC_ID_HEVC;
        }
    }
    if (codecId == AV_CODEC_ID_H264) {
        strFile += ".h264";
    } else if (codecId = AV_CODEC_ID_HEVC) {
        strFile += ".h265";
    }

    std::ofstream file;
    file.open(strFile, std::ios::binary);

    // 1.Ѱ�ұ�����  AV_CODEC_ID_HEVC(H265)
    AVCodec* pCodec = avcodec_find_encoder(codecId);
    if (pCodec == nullptr) {
        std::cerr << "avcodec_find_encoder failed..." << std::endl;
        return -1;
    }

    // 2.��ȡ����������
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == nullptr) {
        std::cerr << "avcodec_alloc_context3 failed..." << std::endl;
        return -2;
    }

    // 3.�趨�����Ĳ���
    pCodecCtx->width = 400;
    pCodecCtx->height = 300;
    pCodecCtx->time_base = { 1, 25 };           // ֡ʱ�����ʱ�䵥λ pts*time_base=��ǰ����ʱ��
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;    // ָ��Դ�������ظ�ʽ��������㷨���
    pCodecCtx->thread_count = 16;               // �����߳���
    pCodecCtx->max_b_frames = 0;                // b֡Ϊ0����ʱ���ͣ����������
    // Ԥ�����������
    av_opt_set(pCodecCtx->priv_data, "preset", "ultrafast", 0);         // ����ٶ�
    av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);         // 0��ʱ

    // 4.�򿪱���������
    int iRet = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (iRet < 0) {
        char strErr[1024] = { 0 };
        av_strerror(iRet, strErr, sizeof(strErr) - 1);
        std::cerr << strErr << std::endl;
        return -3;
    }
    std::cout << "avcodec_open2 success..." << std::endl;

    AVFrame* pFrame = av_frame_alloc();
    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;
    pFrame->format = pCodecCtx->pix_fmt;
    iRet = av_frame_get_buffer(pFrame, 0);
    if (iRet != 0) {
        char strErr[1024] = { 0 };
        av_strerror(iRet, strErr, sizeof(strErr) - 1);
        std::cerr << strErr << std::endl;
        return -4;
    }


    AVPacket* pPacket = av_packet_alloc();
    // ����10s��Ƶ 250֡
    for (int i = 0; i < 250; ++i) {
        // ����AVFrame����
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

        // ����δѹ��֡�������߳���
        iRet = avcodec_send_frame(pCodecCtx, pFrame);
        if (iRet != 0) {
            break;
        }

        // ���ܱ��������ݣ�һ��ǰ���ε��÷���null������δ��ɣ�
        // �������ڶ����߳��У�����ʱ�����ڻ���
        // �п��ܷ��ض�֡
        while (iRet >= 0) {
            iRet = avcodec_receive_packet(pCodecCtx, pPacket);
            if (iRet == AVERROR(EAGAIN) || iRet == AVERROR_EOF) {
                break;
            } 
            if (iRet < 0) {
                char strErr[1024] = { 0 };
                av_strerror(iRet, strErr, sizeof(strErr) - 1);
                std::cerr << strErr << std::endl;
                break;
            }
            std::cout << pPacket->size << " " << std::flush;
            file.write((char*)pPacket->data, pPacket->size);
            av_packet_unref(pPacket);
        }
    }

    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    avcodec_free_context(&pCodecCtx);
    file.close();

    return 0;
}