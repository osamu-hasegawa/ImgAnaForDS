//**************************************************************************************
// メニュー画面
//--------------------------------------------------------------------------------------
// 2017.04.01 VER 2.0.0.0 カメラ傾き画面の追加
//**************************************************************************************
#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_MENU.h"
#include "math.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"
#if 1//2018.05.01
#include "STG.h"
#endif
//------------------------------------
POINT	m_pnt_g;
POINT	m_pnt_r;
CRect	m_rtImg;
int		m_nCaptured;
int		m_zoom=1;
#if 1//2018.05.01
static
HANDLE	m_hExtProg=NULL;
#endif
//------------------------------------
ITBL CKOP_MENU::itbl[] = {
	{ 0,          0,    0}
};

//
// メイン画面定義
//    基準WID = BASE_WID = 644
//    基準HEI = BASE_HEI = 482
//      IMG_WID,IMG_HEIは現在接続中カメラの画像表示エリアのサイズ(実サイズの1/2 or 1/4 or ...)
//      基準ＸＹサイズとのずれをoffsで制御して画面を整列させる
//        offs=1:CXをIMG_WIDで
//             2:CYをIMG_HEIで
//             4: Xを(IMG_WID-基準WID)分オフセットする
//             8: Yを(IMG_HEI-基準HEI)分オフセットする
CTBL CKOP_MENU::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 0, "LOGO"              ,1140,  10, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 0  , IDC_BUTTON1 , 0, "シェーディング解析", 990,  10, 140, 70},
	{ 0, 0  , IDC_BUTTON2 , 0, "倍率解析"          , 990,  90, 140, 70},
	{ 0, 0  , IDC_BUTTON3 , 0, "ＭＴＦ解析"        , 990, 170, 140, 70},
#if 1//2018.05.01
	{ 0, 0  , IDC_BUTTON8 , 0, "ステージ設定"      ,1140, 170, 140, 70},
#endif
	{ 0, 0  , IDC_BUTTON4 , 0, "面積＆重心"        , 990, 250, 140, 70},
	{ 0, 0  , IDC_BUTTON5 , 0, "ディストーション"  , 990, 330, 140, 70},
#if 1//2017.04.01
	{ 0, 0  , IDC_BUTTON7 , 0, "カメラ傾き"        , 990, 410, 140, 70},
	{ 0, 0  , IDC_BUTTON6 , 0, "戻る"              , 990, 540, 140, 70},
	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  ,  10,   3, 322, 22, WS_BORDER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 2+4, IDC_STATIC2 , 0, "\0HIST"            , 660,  30,  50,482, 0|SS_CENTER|SS_CENTERIMAGE|SS_USERITEM},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 1+8, IDC_STATIC3 , 0, "\0HIST"            ,  10, 518, 644, 50, 0|SS_CENTER|SS_CENTERIMAGE|SS_SIMPLE},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC4 , 0, "\0IMAGE1"          , 730,  30, 240,180, 0|SS_CENTER|SS_CENTERIMAGE|SS_SIMPLE},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC5 , 0, "\0IMAGE2"          , 730, 240, 240,180, 0|SS_CENTER|SS_CENTERIMAGE|SS_SIMPLE},//SS_WHITEFRAME},
#else
#if 1//2015.09.10
	{ 0, 0  , IDC_BUTTON7 , 0, "シェーディング補正", 990, 410, 140, 70},
#endif
	{ 0, 0  , IDC_BUTTON6 , 0, "戻る"              , 990, 540, 140, 70},
	{ 0, 1  , IDC_STATIC11, 0, "Ser.No:201506012"  ,  10,   3, 644, 22, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 2+4, IDC_STATIC2 , 0, "\0HIST1"           , 660,  30,  50,482, 0|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 1+8, IDC_STATIC3 , 0, "\0HIST2"           ,  10, 518, 644, 50, 0|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC4 , 0, "\0IMAGE1"          , 730,  30, 240,180, 0|SS_CENTER|SS_CENTERIMAGE|0},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC5 , 0, "\0IMAGE2"          , 730, 240, 240,180, 0|SS_CENTER|SS_CENTERIMAGE|SS_BLACKFRAME},//SS_WHITEFRAME},
#endif
	{ 0, 0  , IDC_STATIC6 , 0, "倍率 x1"           , 800, 440, 100, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},//|SS_NOTIFY
	{ 0, 4+8, IDC_STATIC7 , 0, "Center"            , 685, 540, 100, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 8  , IDC_STATIC8 , 0, "赤(625,246)=109"   ,  10, 580, 250, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC9 , 0, "緑( 13,243)=140"   , 330, 580, 250, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC10, 0, "相対位置(612, 3)"  , 685, 580, 250, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  ,            0, 0, NULL}
};


/*
                  (460)
670 --------------------------------1130
       (230)       900     (230)
670 ---(???)--------|---------------1130
*/
void CKOP_MENU::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	m_pnt_r.x = GetProfileINT("MENU", "RBAR:X", CKOP::BASE_WID/2+10);
	m_pnt_r.y = GetProfileINT("MENU", "RBAR:Y", CKOP::BASE_HEI/2+10);
	m_pnt_g.x = GetProfileINT("MENU", "GBAR:X", CKOP::BASE_WID/2-10);
	m_pnt_g.y = GetProfileINT("MENU", "GBAR:Y", CKOP::BASE_HEI/2-10);
	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}
#if 1//2015.09.10
	if (m_pnt_r.x >= CKOP::IMG_WID) {
		m_pnt_r.x = CKOP::IMG_WID-1;
	}
	if (m_pnt_r.y >= CKOP::IMG_HEI) {
		m_pnt_r.y = CKOP::IMG_HEI-1;
	}
	if (m_pnt_g.x >= CKOP::IMG_WID) {
		m_pnt_g.x = CKOP::IMG_WID-1;
	}
	if (m_pnt_g.y >= CKOP::IMG_HEI) {
		m_pnt_g.y = CKOP::IMG_HEI-1;
	}
#endif
#if 1//2018.05.01
	if (!G_SS.USE_SURUGA) {
		pWndForm->GetDlgItem(IDC_BUTTON8)->ShowWindow(SW_HIDE);
	}
	else if (!STG::IS_OPENED()) {
		pWndForm->GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
	}
#endif
}

void CKOP_MENU::TERM_FORM(void)
{
	WriteProfileINT("MENU", "RBAR:X", m_pnt_r.x);
	WriteProfileINT("MENU", "RBAR:Y", m_pnt_r.y);
	WriteProfileINT("MENU", "GBAR:X", m_pnt_g.x);
	WriteProfileINT("MENU", "GBAR:Y", m_pnt_g.y);
}

void CKOP_MENU::UPDATE_TEXT(CWnd *pWndForm, int mask, LPBYTE pImgPxl)
{
	CString	buf;
//	int		RX = CKOP::BMP_WID/CKOP::IMG_WID;
//	int		RY = CKOP::BMP_HEI/CKOP::IMG_HEI;
#if 1//2016.01.01
	double	RX = CKOP::RX;
	double	RY = CKOP::RY;

	if (mask & 1) {
		buf.Format("赤(%3d,%3d)=%3d", (int)(m_pnt_r.x*RX), (int)(m_pnt_r.y*RY), CKOP::GET_PIXEL(m_pnt_r.x*RX, m_pnt_r.y*RY, pImgPxl));
		pWndForm->GetDlgItem(IDC_STATIC8)->SetWindowText(buf);
	}
	if (mask & 2) {
		buf.Format("緑(%3d,%3d)=%3d", (int)(m_pnt_g.x*RX), (int)(m_pnt_g.y*RY), CKOP::GET_PIXEL(m_pnt_g.x*RX, m_pnt_g.y*RY, pImgPxl));
		pWndForm->GetDlgItem(IDC_STATIC9)->SetWindowText(buf);
	}
	if (mask & (1|2)) {
		buf.Format("相対位置(%3d,%3d)", (int)((m_pnt_r.x-m_pnt_g.x)*RX), (int)((m_pnt_r.y-m_pnt_g.y)*RY));
		pWndForm->GetDlgItem(IDC_STATIC10)->SetWindowText(buf);
	}
	if (mask & 8) {
		buf.Format("倍率 x%d", m_zoom);
		pWndForm->GetDlgItem(IDC_STATIC6)->SetWindowText(buf);
	}
#else
	int		RX = (int)(CKOP::RX);
	int		RY = (int)(CKOP::RY);

	if (mask & 1) {
		buf.Format("赤(%3d,%3d)=%3d", m_pnt_r.x*RX, m_pnt_r.y*RY, CKOP::GET_PIXEL(m_pnt_r.x*RX, m_pnt_r.y*RY, pImgPxl));
		pWndForm->GetDlgItem(IDC_STATIC8)->SetWindowText(buf);
	}
	if (mask & 2) {
		buf.Format("緑(%3d,%3d)=%3d", m_pnt_g.x*RX, m_pnt_g.y*RY, CKOP::GET_PIXEL(m_pnt_g.x*RX, m_pnt_g.y*RY, pImgPxl));
		pWndForm->GetDlgItem(IDC_STATIC9)->SetWindowText(buf);
	}
	if (mask & (1|2)) {
		buf.Format("相対位置(%3d,%3d)", (m_pnt_r.x-m_pnt_g.x)*RX, (m_pnt_r.y-m_pnt_g.y)*RY);
		pWndForm->GetDlgItem(IDC_STATIC10)->SetWindowText(buf);
	}
	if (mask & 8) {
		buf.Format("倍率 x%d", m_zoom);
		pWndForm->GetDlgItem(IDC_STATIC6)->SetWindowText(buf);
	}
#endif
}

void CKOP_MENU::DRAW_HIST(CWnd *pWnd, int n, LPBYTE pImgPxl, LPBITMAPINFO /*pBmpInfo*/)
{
	CDC		*pDC = pWnd->GetDC();
	CRect	rt;
	CPen*	old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pWnd->GetWindowRect(&rt);
	rt.right = rt.Width();
	rt.bottom = rt.Height();
	rt.top = 0;
	rt.left = 0;

//	rt.DeflateRect(4,4);
	pDC->FillSolidRect(0,0,rt.Width(),rt.Height(), RGB(240,240,240));
	if (!n) {

//		double	rx = CKOP::RX;//CKOP::BMP_WID/CKOP::IMG_WID;
//		double	ry = CKOP::RY;//CKOP::BMP_HEI/CKOP::IMG_HEI;
		int		ox;
//		int		oy;
		int		xbak = -1,
				ybak = -1;

		for (int q = 0; q < 2; q++) {
			if (!q) {
				pDC->SelectObject(&CKOP::m_penRed);
				ox = (int)(m_pnt_r.x * CKOP::RX);
			}
			else {
				pDC->SelectObject(&CKOP::m_penGreen);
				ox = (int)(m_pnt_g.x * CKOP::RX);
			}
			//垂直方向・輝度・断面グラフ
			for (int i = 0; i < CKOP::BMP_HEI; i++) {
				int n = CKOP::GET_PIXEL(ox, i, pImgPxl);
				int	x, y;
				x = (rt.Width()-1) * n / 255; 
				y = (int)(i/CKOP::RY);
				if (!i) {
					pDC->MoveTo(x, y);
				}
				else if (x == xbak && y == ybak) {
					//skip
				}
				else {
					pDC->LineTo(x, y);
					xbak = x;
					ybak = y;
				}
			}
		}
	}
	else {
//		int		rx = CKOP::BMP_WID/CKOP::IMG_WID;
//		int		ry = CKOP::BMP_HEI/CKOP::IMG_HEI;
//		int		ox;
		int		oy;
		int		xbak = -1,
				ybak = -1;

		for (int q = 0; q < 2; q++) {
			if (!q) {
				pDC->SelectObject(&CKOP::m_penRed);
				oy = (int)(m_pnt_r.y * CKOP::RY);
			}
			else {
				pDC->SelectObject(&CKOP::m_penGreen);
				oy = (int)(m_pnt_g.y * CKOP::RY);
			}
			//垂直方向・輝度・断面グラフ
			for (int i = 0; i < CKOP::BMP_WID; i++) {
				int n = CKOP::GET_PIXEL(i, oy, pImgPxl);
				int	x, y;
				x = (int)(i/CKOP::RX);
				y = (rt.bottom-1)-(rt.Height()-1) * n / 255; 
				if (!i) {
					pDC->MoveTo(x, y);
				}
				else if (x == xbak && y == ybak) {
					//skip
				}
				else {
					pDC->LineTo(x, y);
					xbak = x;
					ybak = y;
				}
			}
		}
	}
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
void CKOP_MENU::DRAW_ZOOM(CWnd *pWnd, int n, LPBYTE pImgPxl, LPBITMAPINFO pBmpInfo)
{
	CDC *pDC = pWnd->GetDC();
	CRect	rt;
	int		wid, hei;
	int		wid_i = CKOP::BMP_WID;
    int		hei_i = CKOP::BMP_HEI;
	POINT	pnt;
	if (!n) {
		pnt = m_pnt_r;
	}
	else {
		pnt = m_pnt_g;
	}
	pWnd->GetWindowRect(&rt);
//pnt.y = 0;
	wid = rt.Width();
	hei = rt.Height();

	int		sw = wid/m_zoom;
	int		sh = hei/m_zoom;
	int		ox = pnt.x * CKOP::BMP_WID/CKOP::IMG_WID;
	int		oy = pnt.y * CKOP::BMP_HEI/CKOP::IMG_HEI;
	int		sx = ox - sw/2;
	int		sy = oy - sh/2;

	sy = CKOP::BMP_HEI-(sy+sh)-1;
//	sh = -sh;
#if 0
	sy = BMP_HEI-180+90;
	sh = 180;
#endif
//	pDC->FillSolidRect(0,0,wid,hei, RGB(255,0,255));

#if 1//2015.07.24
	if (pBmpInfo->bmiHeader.biBitCount == 8) {
		::SetDIBColorTable(pDC->GetSafeHdc(), 0, 256, pBmpInfo->bmiColors);
	}
#endif

	StretchDIBits(
		pDC->GetSafeHdc(),
		//dest
		  0, 0, wid, hei,
		//from
		sx, sy, sw, sh,
		pImgPxl, 
		pBmpInfo,
        DIB_RGB_COLORS,
		SRCCOPY	);
	if (sx < 0) {
		int	xx = wid * (-sx) / sw;
		pDC->FillSolidRect(0,0, xx, hei, RGB(255,0,255));
	}
	if ((sx+sw) > CKOP::BMP_WID) {
		int	xx = wid * (CKOP::BMP_WID-(sx+sw)) / sw;
		pDC->FillSolidRect(wid+xx,0, -xx-1, hei, RGB(255,0,255));
	}
	if (sy < 0) {
		int	df = sy;
		int	yy = hei * (df) / sh;
		pDC->FillSolidRect(0, hei+yy, wid, -yy-1, RGB(255,0,255));
	}
	if ((sy+sh) > CKOP::BMP_HEI) {
		int	df =  CKOP::BMP_HEI-(sy+sh);
		int	yy = hei * (-df) / sh;
		pDC->FillSolidRect(0,0, wid, yy, RGB(255,0,255));
	}
	pDC->MoveTo(  0, hei/2);
	pDC->LineTo(wid, hei/2);
	pDC->MoveTo(wid/2,   0);
	pDC->LineTo(wid/2, hei);


	pWnd->ReleaseDC(pDC);
}
void CKOP_MENU::DRAW_CURS(CWnd *pWnd)
{
	CDC	*pDC = pWnd->GetDC();
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pDC->MoveTo(m_pnt_r.x    ,             0);
	pDC->LineTo(m_pnt_r.x    , CKOP::IMG_HEI);
	pDC->MoveTo(            0, m_pnt_r.y    );
	pDC->LineTo(CKOP::IMG_WID, m_pnt_r.y    );

	pDC->SelectObject(&CKOP::m_penGreen);

	pDC->MoveTo(m_pnt_g.x    ,             0);
	pDC->LineTo(m_pnt_g.x    , CKOP::IMG_HEI);
	pDC->MoveTo(            0, m_pnt_g.y    );
	pDC->LineTo(CKOP::IMG_WID, m_pnt_g.y    );

	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}

void CKOP_MENU::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
}

void CKOP_MENU::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);

	DRAW_CURS(pWnd);
	DRAW_HIST(pWndForm->GetDlgItem(IDC_STATIC2), 0, pImgPxl, NULL);
	DRAW_HIST(pWndForm->GetDlgItem(IDC_STATIC3), 1, pImgPxl, NULL);
	DRAW_ZOOM(pWndForm->GetDlgItem(IDC_STATIC4), 0, pImgPxl, pbmpinfo);
	DRAW_ZOOM(pWndForm->GetDlgItem(IDC_STATIC5), 1, pImgPxl, pbmpinfo);
	UPDATE_TEXT(pWndForm, -1, pImgPxl);
}

BOOL CKOP_MENU::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON1://シェーディング解析
			CKOP::MOVE_FORM(pWndForm, 2);
			return(TRUE);
		break;
		case IDC_BUTTON2://倍率解析
			CKOP::MOVE_FORM(pWndForm, 4);
			return(TRUE);
		break;
		case IDC_BUTTON3://ＭＴＦ解析
			CKOP::MOVE_FORM(pWndForm, 5);
			return(TRUE);
		break;
		case IDC_BUTTON4://面積＆重心
			CKOP::MOVE_FORM(pWndForm, 3);
			return(TRUE);
		break;
		case IDC_BUTTON5://ディストーション
			CKOP::MOVE_FORM(pWndForm, 6);
			return(TRUE);
		break;
#if 1//2015.09.10
		case IDC_BUTTON7://シェーディング補正
			CKOP::MOVE_FORM(pWndForm, 7);
			return(TRUE);
		break;
#endif
#if 1//2018.05.01
		case IDC_BUTTON8://ステージ設定
			STG::CLOSE();
			if (EXEC_EXT_PROG()) {
				AfxGetMainWnd()->EnableWindow(FALSE);
			}
			return(TRUE);
		break;
#endif
/*		case IDC_BUTTON6://戻る
			CKOP::MOVE_FORM(pWndForm, 1);
			return(TRUE);
		break;*/
		}
	break;
	case WM_CTLCOLOR:
		nID = nID;
	break;
	}
	return(FALSE);
}

BOOL CKOP_MENU::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
{
	UINT	nID;
	POINT	pnt;

	if (pWndForm->m_hWnd == pMsg->hwnd) {
		nID = IDC_IMAGEAREA;
	}
	else {
		nID = ::GetDlgCtrlID(pMsg->hwnd);
	}
	switch (nID) {
	case IDC_IMAGEAREA:
		switch (pMsg->message) {

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			pnt.x = GET_X_LPARAM(pMsg->lParam);
			pnt.y = GET_Y_LPARAM(pMsg->lParam);
			if (CKOP::IMG_RECT.PtInRect(pnt)) {
				CKOP::ToClinet(pWndForm->m_hWnd, ::GetDlgItem(pWndForm->m_hWnd, IDC_IMAGEAREA), &pnt);
				if (pMsg->message == WM_LBUTTONDOWN) {
					m_pnt_r = pnt;
					m_nCaptured = 1;
				}
				else {
					m_pnt_g = pnt;
					m_nCaptured = 2;
				}
pWndForm->Invalidate(FALSE);
//				CKOP::ON_DRAW_STA(pWndForm, IMG_WID, IMG_HEI);
//				UPDATE_TEXT(pWndForm, m_nCaptured);
				pWndForm->SetCapture();
			}
			else {
				::MessageBeep(MB_ICONHAND);
			}
		break;
		case WM_MOUSEMOVE:
			if (m_nCaptured) {
				pnt.x = GET_X_LPARAM(pMsg->lParam);
				pnt.y = GET_Y_LPARAM(pMsg->lParam);
				CKOP::ToClinet(pWndForm->m_hWnd, ::GetDlgItem(pWndForm->m_hWnd, IDC_IMAGEAREA), &pnt);
				if (pnt.x < 0) {
					pnt.x = 0;
				}
				else if (pnt.x >= CKOP::IMG_WID) {
					pnt.x = CKOP::IMG_WID-1;
				}
				if (pnt.y < 0) {
					pnt.y = 0;
				}
				else if (pnt.y >= CKOP::IMG_HEI) {
					pnt.y = CKOP::IMG_HEI-1;
				}
				if (m_nCaptured == 1) {
					m_pnt_r = pnt;
				}
				else {
					m_pnt_g = pnt;
				}
pWndForm->Invalidate(FALSE);
//				UPDATE_TEXT(pWndForm, m_nCaptured);
//				CKOP::ON_DRAW_STA(pWndForm, IMG_WID, IMG_HEI);
			}
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			ReleaseCapture();
			m_nCaptured = 0;
		break;
		}
	break;
	case IDC_STATIC6://倍率
		switch (pMsg->message) {
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			switch (m_zoom) {
			case  1:m_zoom = 2; break;
			case  2:m_zoom = 3; break;
			case  3:m_zoom = 5; break;
			default:m_zoom = 1; break;
			}
			pWndForm->Invalidate(FALSE);
			//UPDATE_TEXT(pWndForm, 8);
		break;
		}
	break;
	case IDC_STATIC7://Center
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			pnt.x = CKOP::IMG_WID/2;
			pnt.y = CKOP::IMG_HEI/2;
			if (pMsg->message == WM_LBUTTONDOWN) {
				m_pnt_r = pnt;
				//UPDATE_TEXT(pWndForm, 1);
			}
			else {
				m_pnt_g = pnt;
				//UPDATE_TEXT(pWndForm, 2);
			}
			pWndForm->Invalidate(FALSE);
		break;
		}
	break;
	case IDC_STATIC8:
	case IDC_STATIC9:
		switch (pMsg->message) {
		case WM_CTLCOLORSTATIC:
			::SetTextColor((HDC)(pMsg->wParam), RGB(255,0,0));
			return((BOOL)CKOP::m_brsBack.m_hObject);
		break;
		}
	break;
	}
	return(FALSE);
}
#if 1//2018.05.01
BOOL CKOP_MENU::EXEC_EXT_PROG(void)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	TCHAR	cmd[MAX_PATH];
	TCHAR	arg[MAX_PATH];
	BOOL	ret;

	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);

	strcpy_s(cmd, _countof(cmd), "\"");
	strcat_s(cmd, _countof(cmd), GetDirModule(NULL));
	strcat_s(cmd, _countof(cmd), "\\DS102.exe");
	strcat_s(cmd, _countof(cmd), "\"");
	//---
	strcat_s(cmd, _countof(cmd), " ");
	//---
	strcat_s(cmd, _countof(arg), "\"");
	strcat_s(cmd, _countof(arg), STG::GET_INI_PATH());
	strcat_s(cmd, _countof(arg), "\"");
	//---
	ret = CreateProcess(
				NULL,
				cmd,
				NULL,
				NULL,
				FALSE,NORMAL_PRIORITY_CLASS,
				NULL,
				NULL,
				&si,
				&pi
			);
	if (!ret) {
		ret = ::GetLastError();
		mlog("ステージ設定コマンド\rDS102.EXEの起動に失敗しました.(%d)", ret);
		return(FALSE);
	}
	CloseHandle(pi.hThread);

	m_hExtProg=pi.hProcess;
	return(TRUE);
}
void CKOP_MENU::TIMER_PROC(CWnd* pWndForm)
{
	if (m_hExtProg != NULL) {
		if (WaitForSingleObject(m_hExtProg, 0) == WAIT_OBJECT_0) {
			CloseHandle(m_hExtProg);
			m_hExtProg = NULL;
			AfxGetMainWnd()->EnableWindow(TRUE);
			AfxGetMainWnd()->SetForegroundWindow();
			//---
			STG::OPEN();
			STG::LOAD_DSTBL();
		}
	}
}
#endif