#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

int main() {
    std::cout << avcodec_configuration() << std::endl;
    // ����AVFrame����
    AVFrame* pFrame1 = av_frame_alloc();
    
    // ����AVFrameͼ�����
    pFrame1->width = 400;
    pFrame1->height = 300;
    pFrame1->format = AV_PIX_FMT_ARGB;

    // ����ؼ������ö�����
    int iRet = av_frame_get_buffer(pFrame1, 32);
    if (iRet != 0) {
        char buf[1024] = { 0 };
        av_strerror(iRet, buf, 1024);
        std::cout << buf << std::endl;
    }
    std::cout << "linesize: " << pFrame1->linesize[0] << std::endl;
    if (pFrame1->buf[0]) {
        std::cout << "Frame1 ref count: " << av_buffer_get_ref_count(pFrame1->buf[0]) << std::endl;
    }

    AVFrame* pFrame2 = av_frame_alloc();
    // ����һ�� AVFrame �����ã����ӻ����������ü��������ݹ���
    av_frame_ref(pFrame2, pFrame1);
    std::cout << "av_frame_ref(pFrame2, pFrame1); " << std::endl;
    if (pFrame1->buf[0]) {
        std::cout << "Frame1 ref count: " << av_buffer_get_ref_count(pFrame1->buf[0]) << std::endl;
    }
    if (pFrame2->buf[0]) {
        std::cout << "Frame2 ref count: " << av_buffer_get_ref_count(pFrame2->buf[0]) << std::endl;
    }

    // ����� AVFrame �����ã����ٻ����������ü�����
    // �����ü�����Ϊ 0 ʱ�����Զ��ͷ���ػ�����
    av_frame_unref(pFrame2);
    std::cout << "av_frame_unref(pFrame2);" << std::endl;
    if (pFrame1->buf[0]) {
        std::cout << "Frame1 ref count: " << av_buffer_get_ref_count(pFrame1->buf[0]) << std::endl;
    }
    // �˺������ȵ��� av_frame_unref () ������ã�Ȼ���ͷ� AVFrame �ṹ�屾������ָ����Ϊ NULL
    av_frame_free(&pFrame1);
    av_frame_free(&pFrame2);

    return 0;
}