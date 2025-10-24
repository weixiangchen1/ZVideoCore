#include <iostream>
#include <fstream>
extern "C" {
#include <libswscale/swscale.h>
}
#pragma comment(lib, "swscale.lib")
#define YUV_FILE "400_300_25.yuv"
#define RGB_FILE "800_600_25.rgba"

// ����������һ��ͼ��ת��������(struct SwsContext)
// �������Ĵ洢��ת����������в������м�״̬�������ظ���ʼ�����������ܿ���
// struct SwsContext* sws_getCachedContext(struct SwsContext* context,
//                                        int srcW, int srcH, enum AVPixelFormat srcFormat,
//                                        int dstW, int dstH, enum AVPixelFormat dstFormat,
//                                        int flags, SwsFilter* srcFilter,
//                                        SwsFilter* dstFilter, const double* param);
// context���Ѵ��ڵ�ת�������ģ���Ϊ NULL����ʱ�������������ģ���
// srcW / srcH��Դͼ��Ŀ�Ⱥ͸߶ȡ�
// srcFormat��Դͼ������ظ�ʽ���� AV_PIX_FMT_YUV420P����
// dstW / dstH��Ŀ��ͼ��Ŀ�Ⱥ͸߶ȡ�
// dstFormat��Ŀ��ͼ������ظ�ʽ���� AV_PIX_FMT_RGB24����
// flags�������㷨��־������ֵ��SWS_BILINEAR ˫���Բ�ֵ��SWS_BICUBIC ˫���β�ֵ�ȣ���
// srcFilter / dstFilter��Դ / Ŀ��ͼ����˲�����ͨ��Ϊ NULL��ʹ��Ĭ�ϣ���
// param������Ĳ�����ͨ��Ϊ NULL����

// ʹ���� sws_getCachedContext �����������ģ���ͼ����и�ʽת��������
// int sws_scale(struct SwsContext *c,
//               const uint8_t* const srcSlice[], const int srcStride[],
//               int srcSliceY, int srcSliceH,
//               uint8_t* const dst[], const int dstStride[]);
// c���� sws_getCachedContext ������ת�������ġ�
// srcSlice��Դͼ�����ݵ�ָ�����飨���� planar ��ʽ���� YUV420P������Ԫ�طֱ�ָ�� Y��U��V ƽ�棩��
// srcStride��Դͼ��ÿ��ƽ����п�ȣ�һ�����ص��ֽ��������ܰ���������䣩��
// srcSliceY��Դͼ���п�ʼ�������ʼ�У�ͨ��Ϊ 0����
// srcSliceH����Ҫ�����������ͨ������Դͼ��߶� srcH����
// dst��Ŀ��ͼ�����ݵ�ָ�����飨ͬ srcSlice��ָ��Ŀ��ƽ�棩��
// dstStride��Ŀ��ͼ��ÿ��ƽ����п�ȣ�ͬ srcStride����


int main() {
    int iYUVWidth = 400, iYUVHeight = 300;
    int iRGBWidth = 800, iRGBHeight = 600;
    // ƽ��洢 yyyy uu vv
    unsigned char* pYUVData[3] = { 0 };
    int iYUVLineSize[3] = { iYUVWidth, iYUVWidth / 2, iYUVWidth / 2 };
    pYUVData[0] = new unsigned char[iYUVWidth * iYUVHeight];         // Y
    pYUVData[1] = new unsigned char[iYUVWidth * iYUVHeight / 4];     // U
    pYUVData[2] = new unsigned char[iYUVWidth * iYUVHeight / 4];     // V
    
    // ����洢 rgba rgba rgba
    unsigned char* pRGBData = new unsigned char[iRGBWidth * iRGBHeight * 4];
    int iRGBLineSize = iRGBWidth * 4;

    // 400*300 YUV ת 800*600 RGBA
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

        // ���� YUVתRGBA ��������SwsContext
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
        // ����ͼ�����ź͸�ʽת��
        int iRet = sws_scale(pYUV2RGB, pYUVData, iYUVLineSize, 0, iYUVHeight, data, linesize);
        std::cout << "rgba height: " << iRet << std::endl;
        os.write((char*)pRGBData, iRGBWidth * iRGBHeight * 4);
    }

    // 800*600 RGBA ת 400*300 YUV
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

        // ���� RGBAתYUV ��������SwsContext
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