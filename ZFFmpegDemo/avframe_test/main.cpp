#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

int main() {
    std::cout << avcodec_configuration() << std::endl;
    // 创建AVFrame对象
    AVFrame* pFrame1 = av_frame_alloc();
    
    // 设置AVFrame图像参数
    pFrame1->width = 400;
    pFrame1->height = 300;
    pFrame1->format = AV_PIX_FMT_ARGB;

    // 分配控件，设置对齐数
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
    // 创建一个 AVFrame 的引用（增加缓冲区的引用计数）数据共享
    av_frame_ref(pFrame2, pFrame1);
    std::cout << "av_frame_ref(pFrame2, pFrame1); " << std::endl;
    if (pFrame1->buf[0]) {
        std::cout << "Frame1 ref count: " << av_buffer_get_ref_count(pFrame1->buf[0]) << std::endl;
    }
    if (pFrame2->buf[0]) {
        std::cout << "Frame2 ref count: " << av_buffer_get_ref_count(pFrame2->buf[0]) << std::endl;
    }

    // 解除对 AVFrame 的引用（减少缓冲区的引用计数）
    // 当引用计数减为 0 时，会自动释放相关缓冲区
    av_frame_unref(pFrame2);
    std::cout << "av_frame_unref(pFrame2);" << std::endl;
    if (pFrame1->buf[0]) {
        std::cout << "Frame1 ref count: " << av_buffer_get_ref_count(pFrame1->buf[0]) << std::endl;
    }
    // 此函数会先调用 av_frame_unref () 解除引用，然后释放 AVFrame 结构体本身，并将指针设为 NULL
    av_frame_free(&pFrame1);
    av_frame_free(&pFrame2);

    return 0;
}