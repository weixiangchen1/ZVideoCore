#include "ZVideoView.h"
#include <iostream>
#include "ZSDLView.h"
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avutil.lib")

ZVideoView::~ZVideoView() {
	if (m_pCacheBuf != nullptr) {
		delete m_pCacheBuf;
		m_pCacheBuf = nullptr;
	}
}

ZVideoView* ZVideoView::CreateVideoView(RenderType eType) {
	switch (eType) {
	case ZVideoView::RenderType::SDL:
		return new ZSDLView();
		break;
	default:
		break;
	}
    return nullptr;
}

bool ZVideoView::Init(AVCodecParameters* pAVParam) {
	if (pAVParam == nullptr) {
		return false;
	}
	VideoFormat eFormat = (VideoFormat)pAVParam->format;
	switch (pAVParam->format) {
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
		eFormat = VideoFormat::YUV420P;
		break;
	default:
		break;
	}
	return Init(pAVParam->width, pAVParam->height, eFormat);
}

bool ZVideoView::DrawFrame(AVFrame* pFrame) {
	if (pFrame == nullptr || pFrame->data[0] == nullptr) {
		return false;
	}
	m_iCount++;
	if (m_lBeginMs <= 0) {
		m_lBeginMs = clock();
	} else if (((clock() - m_lBeginMs) / (CLOCKS_PER_SEC / 1000)) >= 1000) {
		m_iFps = m_iCount;
		m_iCount = 0;
		m_lBeginMs = clock();
	}
	switch (pFrame->format) {
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
		return Draw(pFrame->data[0], pFrame->linesize[0],
					pFrame->data[1], pFrame->linesize[1], 
					pFrame->data[2], pFrame->linesize[2]);
	case AV_PIX_FMT_NV12: {
		// pFrame->data[0] + pFrame->data[1]
		if (m_pCacheBuf == nullptr) {
			m_pCacheBuf = new unsigned char[4096 * 2160 * 1.5];
		}
		int iLineSize = pFrame->width;
		if (pFrame->linesize[0] == pFrame->width) {
			memcpy(m_pCacheBuf, pFrame->data[0], pFrame->linesize[0] * pFrame->height);
			memcpy(m_pCacheBuf + pFrame->linesize[0] * pFrame->height,
				pFrame->data[1], pFrame->linesize[1] * pFrame->height / 2);
		} else {
			// 由于对齐问题 需要逐行进行复制 舍弃对齐的数据
			// 逐行复制Y数据
			for (int i = 0; i < pFrame->height; ++i) {
				memcpy(m_pCacheBuf + i * pFrame->width,
					pFrame->data[0] + i * pFrame->linesize[0],
					pFrame->width);
			}
			// 逐行复制UV数据
			unsigned char* pOffsetY = m_pCacheBuf + pFrame->height * pFrame->width;
			for (int i = 0; i < pFrame->height / 2; ++i) {
				memcpy(pOffsetY + i * pFrame->width,
					pFrame->data[1] + i * pFrame->linesize[1],
					pFrame->width);
			}
		}
		return Draw(m_pCacheBuf, iLineSize);
	}
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_RGBA:
	case AV_PIX_FMT_ARGB:
		return Draw(pFrame->data[0], pFrame->linesize[0]);
	case AV_PIX_FMT_RGB24:
		return Draw(pFrame->data[0], pFrame->linesize[0]);
	default:
		break;
	}
	return false;
}

int ZVideoView::GetRenderFps() {
	return m_iFps;
}

bool ZVideoView::OpenFile(std::string strFile) {
	if (m_file.is_open()) {
		m_file.close();
	}
	m_file.open(strFile, std::ios::binary);
	return m_file.is_open();
}

AVFrame* ZVideoView::ReadFrame() {
	if (m_iWidth <= 0 || m_iHeight <= 0 || !m_file.is_open()) {
		return nullptr;
	}
	// 如果AVFrame空间已经申请，如参数发生变化，需要释放空间
	if (m_pFrame != nullptr) {
		if (m_pFrame->width != m_iWidth || m_pFrame->height != m_iHeight ||
			m_pFrame->format != (int)m_eFormat) {
			// 释放AVFrame空间及buf引用计数减一
			av_frame_free(&m_pFrame);
		}
	}
	// 申请AVFrame空间并设置参数
	if (m_pFrame == nullptr) {
		m_pFrame = av_frame_alloc();
		m_pFrame->width = m_iWidth;
		m_pFrame->height = m_iHeight;
		m_pFrame->format = (int)m_eFormat;
		m_pFrame->linesize[0] = m_iWidth * 4;
		if (m_pFrame->format == AV_PIX_FMT_YUV420P) {
			m_pFrame->linesize[0] = m_iWidth;		// Y
			m_pFrame->linesize[1] = m_iWidth / 2;	// U
			m_pFrame->linesize[2] = m_iWidth / 2;	// V
		} else if (m_pFrame->format == AV_PIX_FMT_RGB24) {
			m_pFrame->linesize[0] = m_iWidth * 3;
		}
		int iRet = av_frame_get_buffer(m_pFrame, 0);
		if (iRet != 0) {
			char strError[1024] = { 0 };
			av_strerror(iRet, strError, sizeof(strError) - 1);
			std::cout << strError << std::endl;
			av_frame_free(&m_pFrame);
			return nullptr;
		}
	}

	if (m_pFrame == nullptr) {
		return nullptr;
	}

	// 读取数据
	if (m_pFrame->format == AV_PIX_FMT_YUV420P) {
		m_file.read((char*)m_pFrame->data[0], m_pFrame->linesize[0] * m_iHeight);		// Y
		m_file.read((char*)m_pFrame->data[1], m_pFrame->linesize[1] * m_iHeight / 2);	// U
		m_file.read((char*)m_pFrame->data[2], m_pFrame->linesize[2] * m_iHeight / 2);	// V
	} else {
		m_file.read((char*)m_pFrame->data[0], m_pFrame->linesize[0] * m_iHeight);
	}

	if (m_file.gcount() == 0) {
		return nullptr;
	}
	return m_pFrame;
}

void ZVideoView::SetWindow(void* pWindow) {
	m_pWindow = pWindow;
}

void ZVideoView::Scale(int iScaleWidth, int iScaleHeight) {
	m_iScaleWidth = iScaleWidth;
	m_iScaleHeight = iScaleHeight;
}
