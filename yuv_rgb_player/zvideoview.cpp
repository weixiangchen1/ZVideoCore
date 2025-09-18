#include "zvideoview.h"
#include "zsdlview.h"
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avutil.lib")

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
		return Draw(pFrame->data[0], pFrame->linesize[0],
					pFrame->data[1], pFrame->linesize[1], 
					pFrame->data[2], pFrame->linesize[2]);
	case AV_PIX_FMT_BGRA:
		return Draw(pFrame->data[0], pFrame->linesize[0]);
	default:
		break;
	}
	return false;
}

int ZVideoView::GetRenderFps() {
	return m_iFps;
}

void ZVideoView::Scale(int iScaleWidth, int iScaleHeight) {
	m_iScaleWidth = iScaleWidth;
	m_iScaleHeight = iScaleHeight;
}
