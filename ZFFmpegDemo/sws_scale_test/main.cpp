#include <iostream>
#include <fstream>
extern "C" {
#include <libswscale/swscale.h>
}
#pragma comment(lib, "swscale.lib")
#define YUV_FILE "400_300_25.yuv"
#define RGB_FILE "800_600_25.rgba"

// 创建或重用一个图像转换上下文(struct SwsContext)
// 该上下文存储了转换所需的所有参数和中间状态，避免重复初始化带来的性能开销
// struct SwsContext* sws_getCachedContext(struct SwsContext* context,
//                                        int srcW, int srcH, enum AVPixelFormat srcFormat,
//                                        int dstW, int dstH, enum AVPixelFormat dstFormat,
//                                        int flags, SwsFilter* srcFilter,
//                                        SwsFilter* dstFilter, const double* param);
// context：已存在的转换上下文（可为 NULL，此时将创建新上下文）。
// srcW / srcH：源图像的宽度和高度。
// srcFormat：源图像的像素格式（如 AV_PIX_FMT_YUV420P）。
// dstW / dstH：目标图像的宽度和高度。
// dstFormat：目标图像的像素格式（如 AV_PIX_FMT_RGB24）。
// flags：缩放算法标志（常用值：SWS_BILINEAR 双线性插值、SWS_BICUBIC 双三次插值等）。
// srcFilter / dstFilter：源 / 目标图像的滤波器（通常为 NULL，使用默认）。
// param：额外的参数（通常为 NULL）。

// 使用由 sws_getCachedContext 创建的上下文，对图像进行格式转换和缩放
// int sws_scale(struct SwsContext *c,
//               const uint8_t* const srcSlice[], const int srcStride[],
//               int srcSliceY, int srcSliceH,
//               uint8_t* const dst[], const int dstStride[]);
// c：由 sws_getCachedContext 创建的转换上下文。
// srcSlice：源图像数据的指针数组（对于 planar 格式，如 YUV420P，数组元素分别指向 Y、U、V 平面）。
// srcStride：源图像每个平面的行跨度（一行像素的字节数，可能包含对齐填充）。
// srcSliceY：源图像中开始处理的起始行（通常为 0）。
// srcSliceH：需要处理的行数（通常等于源图像高度 srcH）。
// dst：目标图像数据的指针数组（同 srcSlice，指向目标平面）。
// dstStride：目标图像每个平面的行跨度（同 srcStride）。


int main() {
    int iYUVWidth = 400, iYUVHeight = 300;
    int iRGBWidth = 800, iRGBHeight = 600;
    // 平面存储 yyyy uu vv
    unsigned char* pYUVData[3] = { 0 };
    int iYUVLineSize[3] = { iYUVWidth, iYUVWidth / 2, iYUVWidth / 2 };
    pYUVData[0] = new unsigned char[iYUVWidth * iYUVHeight];         // Y
    pYUVData[1] = new unsigned char[iYUVWidth * iYUVHeight / 4];     // U
    pYUVData[2] = new unsigned char[iYUVWidth * iYUVHeight / 4];     // V
    
    // 交叉存储 rgba rgba rgba
    unsigned char* pRGBData = new unsigned char[iRGBWidth * iRGBHeight * 4];
    int iRGBLineSize = iRGBWidth * 4;

    // 400*300 YUV 转 800*600 RGBA
    std::ifstream is;
    is.open(YUV_FILE, std::ios::binary);
    if (!is) {
        return -1;
    }

    std::ofstream os;
    os.open(RGB_FILE, std::ios::binary);
    if (!os) {
        return -1;
    }

    SwsContext* pYUV2RGB = nullptr;
    for (;;) {
        is.read((char*)pYUVData[0], iYUVWidth * iYUVHeight);
        is.read((char*)pYUVData[1], iYUVWidth * iYUVHeight / 4);
        is.read((char*)pYUVData[2], iYUVWidth * iYUVHeight / 4);
        if (is.gcount() == 0) {
            break;
        }

        // 创建 YUV转RGBA 的上下文SwsContext
        pYUV2RGB = sws_getCachedContext(pYUV2RGB, iYUVWidth, iYUVHeight, AV_PIX_FMT_YUV420P,
                                        iRGBWidth, iRGBHeight, AV_PIX_FMT_RGBA,
                                        SWS_BILINEAR, 0, 0, 0);
        if (pYUV2RGB == nullptr) {
            return -2;
        }
        unsigned char* data[1];
        data[0] = pRGBData;
        int linesize[1];
        linesize[0] = iRGBLineSize;
        // 进行图像缩放和格式转换
        int iRet = sws_scale(pYUV2RGB, pYUVData, iYUVLineSize, 0, iYUVHeight, data, linesize);
        std::cout << "rgba height: " << iRet << std::endl;
        os.write((char*)pRGBData, iRGBWidth * iRGBHeight * 4);
    }

    // 800*600 RGBA 转 400*300 YUV
    is.close();
    os.close();
    is.open(RGB_FILE, std::ios::binary);
    if (!is) {
        return -3;
    }

    os.open(YUV_FILE, std::ios::binary);
    if (!is) {
        return -3;
    }

    SwsContext* pRGB2YUV = nullptr;
    for (;;) {
        is.read((char*)pRGBData, iRGBWidth * iRGBHeight * 4);
        if (is.gcount() == 0) {
            break;
        }

        // 创建 RGBA转YUV 的上下文SwsContext
        pRGB2YUV = sws_getCachedContext(pRGB2YUV, iRGBWidth, iRGBHeight, AV_PIX_FMT_RGBA,
                                        iYUVWidth, iYUVHeight, AV_PIX_FMT_YUV420P,
                                        SWS_BILINEAR, 0, 0, 0);
        if (pRGB2YUV == nullptr) {
            return -4;
        }
        unsigned char* data[1];
        data[0] = pRGBData;
        int linesize[1];
        linesize[0] = iRGBLineSize;
        int iRet = sws_scale(pRGB2YUV, data, linesize, 0, iRGBHeight, pYUVData, iYUVLineSize);
        std::cout << "yuv height: " << iRet << std::endl;
        os.write((char*)pYUVData[0], iYUVWidth * iYUVHeight);
        os.write((char*)pYUVData[1], iYUVWidth * iYUVHeight / 4);
        os.write((char*)pYUVData[2], iYUVWidth * iYUVHeight / 4);
    }

    delete pYUVData[0];
    delete pYUVData[1];
    delete pYUVData[2];
    delete pRGBData;
    is.close();
    os.close();

    return 0;
}