#pragma once
// ����Ƶ������
struct AVCodecParameters;
struct AVRational;
class ZAVParam {
public:
    AVCodecParameters* pParam = nullptr;         // ����Ƶ����
    AVRational* pTimebase = nullptr;             // ʱ�����
    static ZAVParam* CreateAVParam();
    ~ZAVParam();
private:
    ZAVParam();
};

