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
    // ���װ
    const char* strURL = "v1080.mp4";
    // ��ȡ���װ������ �������ļ� ��ȡ������Ϣ
    AVFormatContext* pFormatCtx = nullptr;
    int iRet = avformat_open_input(&pFormatCtx, strURL, nullptr, nullptr);
    AVLOG(iRet);

    // ��������Ϣ
    iRet = avformat_find_stream_info(pFormatCtx, nullptr);
    AVLOG(iRet);

    // ��ӡý���ļ���ϸ��Ϣ
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
    // ��װ
    AVFormatContext* pFormatECtx = nullptr;
    const char* strOutURL = "out.mp4";
    iRet = avformat_alloc_output_context2(&pFormatECtx, nullptr, nullptr, strOutURL);
    AVLOG(iRet);

    // �����Ƶ������Ƶ��
    AVStream* pMVideoStream = avformat_new_stream(pFormatECtx, nullptr);    // ��Ƶ��
    AVStream* pMAudioStream = avformat_new_stream(pFormatECtx, nullptr);    // ��Ƶ��

    // �����IO
    iRet = avio_open(&pFormatECtx->pb, strOutURL, AVIO_FLAG_WRITE);
    AVLOG(iRet);

    // ���ñ�������Ƶ����
    if (pVideoStream) {
        pMVideoStream->time_base = pVideoStream->time_base;                         // ʱ�������ԭ��Ƶһ��
        avcodec_parameters_copy(pMVideoStream->codecpar, pVideoStream->codecpar);   // �ӽ��װ���Ʋ���
    }
    if (pAudioStream) {
        pMAudioStream->time_base = pAudioStream->time_base;
        avcodec_parameters_copy(pMAudioStream->codecpar, pAudioStream->codecpar);
    }

    // д���ļ�ͷ
    iRet = avformat_write_header(pFormatECtx, nullptr);
    AVLOG(iRet);

    // ��ӡý���ļ���ϸ��Ϣ
    av_dump_format(pFormatECtx, 0, strURL, 1);

    // ��ȡ10~20s������Ƶ�� ����Ƶ��ʱ����Ϊ��׼����
    // pts��֡�������ʾ��ʱ�䣨���ڿ�����ʱ�䣩
    // dts��֡�����ʱ�䣨�����������ʱ�䣩
    // time_base: ʱ��� time_base={1��25} => ��1���Ϊ25�ȷ�
    // duration: ͨ��ָ����ý���ļ�����ʱ��
    // ������ B ֡��˫��Ԥ��֡������������Ƶ��������Ƶ����DTS �� PTS ͨ����ͬ��
    // ������ B ֡����Ƶ���� B ֡����ǰ��֡���룬DTS �� PTS ���ܲ�ͬ��PTS ���ܴ��ڻ�С�� DTS��
    double beginTime = 10.0;
    double endTime = 20.0;
    long long beginVideoPts = 0;
    long long beginAudioPts = 0;
    long long endPts = 0;
    // ����out.mp4�Ŀ�ʼpts�ͽ���pts
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

    // ͨ��av_seek_frame �ƶ���10s�Ĺؼ�֡λ��
    // AVSEEK_FLAG_BACKWARD������λ��С�ڵ���Ŀ��ʱ���������ؼ�֡��Ĭ����Ϊ����
    // AVSEEK_FLAG_BYTE�������ļ��ֽ�λ�ö���ʱ��� seek�������ã���
    // AVSEEK_FLAG_ANY������λ���ǹؼ�֡�����ܵ��½��뻨��������Ϻ�������ͬ������
    // AVSEEK_FLAG_FRAME����֡���� seek��timestamp ��ʾ֡����������ʱ�������
    if (pVideoStream) {
        iRet = av_seek_frame(pFormatCtx, pVideoStream->index, beginVideoPts,
                    AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);      // ���ؼ�֡
        AVLOG(iRet);
    }

    AVPacket* pPacket = av_packet_alloc();
    AVFrame* pFrame = zdecode.CreateFrame();
    while (av_read_frame(pFormatCtx, pPacket) >= 0) {
        AVStream* pInStream = pFormatCtx->streams[pPacket->stream_index];
        AVStream* pOutStream = nullptr;
        long long lOffset = 0;      // ��ȡ����Ƶ����Ƶ����ƫ��pts
        if (pVideoStream && pPacket->stream_index == pVideoStream->index) {
            // ��Ƶ��
            lOffset = beginVideoPts;
            pOutStream = pFormatECtx->streams[0];
            // ����20s�˳�ѭ��
            // ��Ƶ������Ƶ����ʱ���Ƕ���ģ�ͨ�� PTS/DTS ͬ����
            // ��Ƶ����endPts��20s ��Ӧ����Ƶʱ������Ѿ���������Ƶ���Ķ�Ӧ����λ��
            if (pPacket->dts > endPts) {
                av_packet_unref(pPacket);   // δִ�е�av_interleaved_write_frame ��Ҫ�ͷ���Դ
                break;
            }
        } else if (pAudioStream && pPacket->stream_index == pAudioStream->index) {
            // ��Ƶ��
            lOffset = beginAudioPts;
            pOutStream = pFormatECtx->streams[1];
        }

        std::cout << pPacket->pts << " : " << pPacket->dts << " :" << pPacket->size << std::endl;
        // ���¼���packet��pts dts duration
        // ���ʱ��� = (����ʱ��� - ƫ����) �� (����ʱ��� / ���ʱ���)
        if (pOutStream) {
            pPacket->pts = av_rescale_q_rnd(pPacket->pts - lOffset, pInStream->time_base,
                pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pPacket->dts = av_rescale_q_rnd(pPacket->dts - lOffset, pInStream->time_base,
                pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pPacket->duration = av_rescale_q(pPacket->duration, pInStream->time_base, pOutStream->time_base);
        }
        pPacket->pos = -1;

        // ���ļ���д������Ƶ�� ִ����ɻ��Զ�����packet
        iRet = av_interleaved_write_frame(pFormatECtx, pPacket);
        if (iRet != 0) {
            std::cerr << GetAVErrorMessage(iRet).c_str() << std::endl;
        }
    }

    // д���ļ�β������
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