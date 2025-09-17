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

void ZVideoView::Scale(int iScaleWidth, int iScaleHeight) {
	m_iScaleWidth = iScaleWidth;
	m_iScaleHeight = iScaleHeight;
}
