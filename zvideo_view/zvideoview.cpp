#include "zvideoview.h"
#include "zsdlview.h"

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

void ZVideoView::Scale(int iScaleWidth, int iScaleHeight) {
	m_iScaleWidth = iScaleWidth;
	m_iScaleHeight = iScaleHeight;
}
