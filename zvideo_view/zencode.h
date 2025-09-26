#pragma once
#include <mutex>
#include <vector>
struct AVPacket;
struct AVFrame;
struct AVCodecContext;
class ZEncode {
public:
    /////////////////////////////////////////////////
    // ����������������
    // @para iCodeId ������id��ffmpeg��Ӧ
    // @return ������������ ʧ�ܷ���nullptr
    static AVCodecContext* CreateContext(int iCodeId);

    /////////////////////////////////////////////////
    // ���ñ����������� ��Դ�ɶ���ά�� (�̰߳�ȫ)
    // m_pCodecCtx��Ϊnull ����������Դ
    // @para pCodecCtx ������������
    void SetCodecContext(AVCodecContext* pCodecCtx);

    /////////////////////////////////////////////////
    // ���ñ��������� (�̰߳�ȫ)
    bool SetAVOpt(const char* pKey, const char* pValue);
    bool SetAVOpt(const char* pKey, const int iValue);

    /////////////////////////////////////////////////
    // �򿪱����� (�̰߳�ȫ)
    bool OpenEncoder();

    /////////////////////////////////////////////////
    // �������� (�̰߳�ȫ)
    // @para pFrame ��Դ���û�ά��
    // @return ʧ�ܷ���null ���ص�AVPacket���û�ͨ��av_packet_free������Դ
    AVPacket* EncodeData(const AVFrame* pFrame);

    /////////////////////////////////////////////////
    // ���ݱ��������Ĵ���AVFrame ��Դ���û�ά��
    AVFrame* CreateFrame();

    /////////////////////////////////////////////////
    // ��ȡ�����е�AVPacket
    std::vector<AVPacket*> GetFlushPacket();

private:
    AVCodecContext* m_pCodecCtx = nullptr;
    std::mutex m_mutex;
};

