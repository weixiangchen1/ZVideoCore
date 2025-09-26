#include "zvideoviewtest.h"
#include <QtWidgets/QApplication>
#include <fstream>
#include "zencode.h"
#include "utils.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

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
    AVCodecContext* pCodecCtx = encode.CreateContext(id);
    pCodecCtx->width = 400;
    pCodecCtx->height = 300;
    encode.SetCodecContext(pCodecCtx);
    encode.SetAVOpt("crf", 18);
    encode.OpenEncoder();

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

    std::vector<AVPacket*> packets = encode.GetFlushPacket();
    for (auto packet : packets) {
        iCount++;
        file.write((char*)packet->data, packet->size);
        av_packet_free(&packet);
    }

    file.close();
    encode.SetCodecContext(nullptr);
    std::cout << "count: " << iCount << std::endl;
}

int main(int argc, char *argv[]) {
    //QApplication app(argc, argv);
    //ZVideoViewTest window;
    //window.show();
    //return app.exec();

    AVCodecID codecId = AV_CODEC_ID_H264;
    if (argc > 1) {
        std::string strCodec = argv[1];
        if (strCodec == "h265" || strCodec == "hevc") {
            codecId = AV_CODEC_ID_HEVC;
        }
    }
    TestZEncode(codecId);
    return 0;
}
