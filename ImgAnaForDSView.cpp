/******************************************************************************************
20160506 Hirota comment
@@20160513 Hirota comment(画像表示のための関数)
20160513 Hirota comment(最初のダイアログの画像表示エリア)
@@20160516 Hirota comment(画像のスクロールバー表示テスト)
*******************************************************************************************/


// ImgAnaForDSView.cpp : CImgAnaForDSView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、サムネイル、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "ImgAnaForDS.h"
#endif

#include "ImgAnaForDSDoc.h"
#include "ImgAnaForDSView.h"
#include "MainFrm.h"

#include "DS.h"
#include "DglSelDevice.h"
#include "KOP.h"
#include "KOP_DEBUG.h"
#include "DlgSerialNo.h"
#include "PrpSheetEx.h"
#if 1//2018.05.01
#include "STG.h"
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SAFE_DELETE(x)  { if(x) delete[] x; x=NULL; }

// CImgAnaForDSView

IMPLEMENT_DYNCREATE(CImgAnaForDSView, CView)

BEGIN_MESSAGE_MAP(CImgAnaForDSView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_CHAR()

	//---
	ON_COMMAND(ID_MODE_ANALYSE, OnModeAnalyse)
	ON_UPDATE_COMMAND_UI(ID_MODE_ANALYSE, OnUpdateModeAnalyse)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON6, IDC_BUTTON6, OnButtons)
	ON_COMMAND(ID_STOP_BY_CODE, OnStopByCode)
	//---
	ON_MESSAGE(WM_CALLBACK, OnCallBack)
	ON_COMMAND_RANGE          (ID_ZOOM_DOWN , ID_SET_SYSTEM, OnCommands)
	//---
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM_DOWN , ID_SET_SYSTEM, OnCommandsU)
#if 1//2017.03.20
	ON_MESSAGE( WM_USER+0x1000, OnPluginCB)
#endif
#if 1//2018.05.01
	ON_WM_CTLCOLOR()
#endif
END_MESSAGE_MAP()



LPBYTE	m_pImgBufBak = NULL;
DWORD	m_nImgBufSize = 0;
DWORD	m_nImgBufBakSize = 0;
DWORD	m_tk1, m_tk2;
int		m_skip_cnt;
BOOL	m_bRUN = FALSE;
CString	m_buf;
int		m_iZoomLvl = 4;
int		m_nZoomTbl[] = {
	10, 25, 50, 75, 100, 125, 150, 200, 300, 400, 500
};
#if 1//2017.03.20
BITMAPINFO
		m_bmi;
#endif
#if 1//2017.07.18
static
int		m_fid;
#if 1//2018.05.01
double	m_fps = C_NAN();
int		m_fps_tk1;
int		m_fps_tk2;
int		m_fps_cnt;
#endif
#endif
#if 1//2018.05.01
typedef struct {
	LARGE_INTEGER
			frq;
	LARGE_INTEGER
			tk1, tk2;
	int		cnt;
	double	fps;

	double GET_MS(void) {
		return((1000.0*(tk2.QuadPart-tk1.QuadPart))/frq.QuadPart);
	}
	double GET_FPS(void) {
		return(1000.0*cnt / GET_MS());
	}
} T_FPS;
T_FPS	m_fps_r, m_fps_d, m_fps_s;
#endif
// CImgAnaForDSView コンストラクション/デストラクション

CImgAnaForDSView::CImgAnaForDSView()
{
	// TODO: 構築コードをここに追加します。
	m_pImage = NULL;
	m_bPAUSE = FALSE;
}

CImgAnaForDSView::~CImgAnaForDSView()
{
	SAFE_DELETE(m_pImage);
	if (m_pImgBufBak != NULL) {
		free(m_pImgBufBak);
	}
}

BOOL CImgAnaForDSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
	cs.style |= WS_HSCROLL;
	cs.style |= WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

// CImgAnaForDSView 描画

void CImgAnaForDSView::OnDraw(CDC* /*pDC*/)//@@20160513 Hirota comment(画像表示のための関数)
{
	CImgAnaForDSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
	if (TRUE /*0 != m_PreviewMode*/){
#if 1 
		HDC	hDC = ::GetDC(m_staImage.m_hWnd);
		DrawImage(hDC);
		::ReleaseDC(m_staImage.m_hWnd, hDC);
#else
		DrawImage(pDC->m_hDC);
#endif
	}
}


// CImgAnaForDSView 診断

#ifdef _DEBUG
void CImgAnaForDSView::AssertValid() const
{
	CView::AssertValid();
}

void CImgAnaForDSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImgAnaForDSDoc* CImgAnaForDSView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImgAnaForDSDoc)));
	return (CImgAnaForDSDoc*)m_pDocument;
}
#endif //_DEBUG


// CImgAnaForDSView メッセージ ハンドラー


int CImgAnaForDSView::OnCreate(LPCREATESTRUCT lpCreateStruct)//20160513 Hirota comment(最初のダイアログの画像表示エリア)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ここに特定な作成コードを追加してください。
	//20160513 Hirota comment(最初のダイアログの画像表示エリア)
	if (!m_staImage.Create("", WS_CHILD|WS_VISIBLE|SS_SIMPLE|SS_CENTERIMAGE|0, CRect(0,0,100,100), this, IDC_IMAGEAREA)) {
		return(-1);
	}

	return 0;
}


void CImgAnaForDSView::OnDestroy()
{
	if (CKOP::STAT_OF_ANALYSE != 0) {
		CKOP::MOVE_FORM(this, 0);
	}

	CView::OnDestroy();
#if 1//2017.03.20
	if (G_PD != NULL) {
		G_PD->PD_STOP();
		G_PD->PD_CLOSE();
		G_PD->PD_TERM();
		return;
	}
#endif
	// TODO: ここにメッセージ ハンドラー コードを追加します。
	CDS::STOP();
#if 1//2018.05.01
	Sleep(100);
#endif
	CDS::DISCONNECT_ALL();
	CDS::TERM();
}


void CImgAnaForDSView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
#if 1//2018.05.01
	if (CKOP::STAT_OF_ANALYSE == 0) {
		KillTimer(nIDEvent);
	}
	else {
		CKOP::TIMER_PROC(this);
	}
#endif
	CView::OnTimer(nIDEvent);
}


void CImgAnaForDSView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: ここにメッセージ ハンドラー コードを追加します。
#if 1
	if (CKOP::STAT_OF_ANALYSE != 0) {
	m_staImage.MoveWindow(10, 30, CKOP::IMG_WID, CKOP::IMG_HEI);
	}
	else {
	m_staImage.MoveWindow(0, 0, cx, cy);
	}
#endif

	if (FALSE/*0 == m_PreviewMode*/){
		CWnd* pChild = GetWindow(GW_CHILD);
		if(pChild){
			CRect rc;
			GetClientRect(&rc);
			pChild->SetWindowPos(&CWnd::wndTop, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
	}
#if 1//2015.09.10
	if (CKOP::STAT_OF_ANALYSE != 0) {
		return;
	}
#endif
	if (TRUE/*1 == m_PreviewMode || 3 == m_PreviewMode || 4 == m_PreviewMode*/){
		ChangeScrollbar();
	}
}


void CImgAnaForDSView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	SCROLLINFO sc;
	GetScrollInfo(SB_HORZ, &sc);

	switch(nSBCode){
	case SB_LEFT:			SetScrollPos(SB_HORZ, 0);									break;
	case SB_ENDSCROLL:																	break;
	case SB_LINELEFT:		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - sc.nPage/4);	break;
	case SB_LINERIGHT:		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + sc.nPage/4);	break;
	case SB_PAGELEFT:		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - sc.nPage);	break;
	case SB_PAGERIGHT:		SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + sc.nPage);	break;
	case SB_RIGHT:			SetScrollPos(SB_HORZ, GetScrollLimit(SB_HORZ));				break;
	case SB_THUMBPOSITION:	nPos &= 0x0000FFFF;	SetScrollPos(SB_HORZ, nPos);			break;
	case SB_THUMBTRACK:		nPos &= 0x0000FFFF;	SetScrollPos(SB_HORZ, nPos);			break;
	}

	DrawImage(NULL);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CImgAnaForDSView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	SCROLLINFO sc;
	GetScrollInfo(SB_VERT, &sc);

	switch(nSBCode){
	case SB_LEFT:			SetScrollPos(SB_VERT, 0);									break;
	case SB_ENDSCROLL:																	break;
	case SB_LINELEFT:		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - sc.nPage/4);	break;
	case SB_LINERIGHT:		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + sc.nPage/4);	break;
	case SB_PAGELEFT:		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - sc.nPage);	break;
	case SB_PAGERIGHT:		SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + sc.nPage);	break;
	case SB_RIGHT:			SetScrollPos(SB_VERT, GetScrollLimit(SB_VERT));				break;
	case SB_THUMBPOSITION:	nPos &= 0x0000FFFF;	SetScrollPos(SB_VERT, nPos);			break;
	case SB_THUMBTRACK:		nPos &= 0x0000FFFF;	SetScrollPos(SB_VERT, nPos);			break;
	}

	DrawImage(NULL);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CImgAnaForDSView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	// 画像上の相対的な位置（座標）をステータスバーに表示します

	CString strx, stry;
	if (CKOP::STAT_OF_ANALYSE == 0) {
	strx.Format(_T("%d"), point.x + GetScrollPos(SB_HORZ));
	stry.Format(_T("%d"), point.y + GetScrollPos(SB_VERT));
	}
	static_cast<CMainFrame*>(theApp.m_pMainWnd)->SetStatusText(STATUS_X, strx);
	static_cast<CMainFrame*>(theApp.m_pMainWnd)->SetStatusText(STATUS_Y, stry);

	CView::OnMouseMove(nFlags, point);
}


void CImgAnaForDSView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	CDglSelDevice	dlg;
#if 1//2018.05.01
	::QueryPerformanceFrequency(&m_fps_r.frq);
	m_fps_s.frq = m_fps_d.frq = m_fps_r.frq;
#endif
	if (!CDS::INIT(TRUE, /*8BIT=*/FALSE/*TRUE*/)) {
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return;
	}
	CDS::m_hWnd = this->m_hWnd;
	if (dlg.DoModal() == IDCANCEL) {
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return;
	}
	DispStsText(STATUS_SW);
	DispStsText(STATUS_FRAME);
	DoZoom100();
#if 0
	OnCommands(ID_CAP_START);
#else
#if 1//2017.03.20
	if (G_PD != NULL) {
#if 1//2018.05.01
		if (G_PD->PD_SET_TPF != NULL && G_SS.SKIP_TPF == 0) {
			G_PD->PD_SET_TPF(G_SS.FRM_MINTIME);
		}
#endif
		if (!G_PD->PD_RUN(this->m_hWnd)) {
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
		G_PD->PD_GET_BITMAPINFO(&m_bmi);
		memcpy(&G_BH, &m_bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		DispStsText(STATUS_SW);
	}
	else {
#endif
#if 1//2018.05.01
	if (G_SS.SKIP_TPF) {
		CDS::SET_TPF(G_SS.FRM_MINTIME);
	}
#endif
	if (!CDS::RUN(/*bWAIT=*/TRUE)) {
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return;
	}
#if 1//2017.03.20
	}
#endif
	SAFE_DELETE(m_pImage);
	if (m_pImage == NULL) {
		m_pImage = new BYTE[CDS::GetSize()];
		m_nImgBufSize = CDS::GetSize();
	}
	if (CDS::m_bOFFLINE) {
		SetOfflineImage();
	}
	ChangeScrollbar();
#if 1//2017.03.20
	if (G_PD == NULL) {
#endif
	CDS::SET_CALLBACK(TRUE);
#if 1//2017.03.20
	}
#endif
	m_bRUN = TRUE;
	m_bPAUSE = FALSE;
#endif
}


BOOL CImgAnaForDSView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	switch (pMsg->message) {
	case WM_PAINT:
	case WM_TIMER:
	case WM_MOUSEMOVE:
	case WM_MOUSELEAVE:
	break;
	case WM_COMMAND:
		CKOP::IMG_RECT.bottom = CKOP::IMG_RECT.bottom;
	break;
	default:
		if (::GetDlgCtrlID(pMsg->hwnd) == IDC_BUTTON1) {
			CKOP::IMG_RECT.bottom = CKOP::IMG_RECT.bottom;
		}
	break;
	}
	if (CKOP::STAT_OF_ANALYSE != 0) {
/*		switch (pMsg->message) {
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOR:
		{*/
			LONG ret;
			if ((ret = CKOP::MSG_PROC(this, pMsg)) != FALSE) {
				return(ret);
			}
/*		}
		break;
		}*/
	}
#if 1//2018.05.01
	else {
		if (pMsg->hwnd == this->m_hWnd) {
			switch (pMsg->message) {
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				DoZoomDown();
			break;
			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
				DoZoomUp();
			break;
			}
		}
	}
#endif
//skip:
	return CView::PreTranslateMessage(pMsg);
}

//20160517 Hirota comment (CImgAnaForDSViewクラスからCViewクラスへコマンドメッセージを投げて処理している)
BOOL CImgAnaForDSView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	if (CKOP::STAT_OF_ANALYSE != 0) {
		LONG ret;
		if ((ret = CKOP::CMD_MSG(this, nID, nCode, pExtra, pHandlerInfo)) != FALSE) {
			return(ret);
		}
#if 0
		if (nID == IDC_COMBO1) {
			if (nCode == CBN_SELCHANGE) {
				nID = nID;
				Invalidate();
			}
			nID = nID;
		}
#endif
	}
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

struct BITMAPINFO2 {
	BITMAPINFOHEADER   bmiHeader;
	RGBQUAD            bmiColors[256];
};

#if 1//_KOP_
LPBITMAPINFO GetBitMapInfo(int Bpp, int Width, int Height)
{
	static
	BITMAPINFO2	bm;

	bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bm.bmiHeader.biPlanes = 1;
	bm.bmiHeader.biCompression = BI_RGB;
	bm.bmiHeader.biSizeImage = 0;
	bm.bmiHeader.biXPelsPerMeter = 0;
	bm.bmiHeader.biYPelsPerMeter = 0;
	bm.bmiHeader.biClrUsed = 0;
	bm.bmiHeader.biClrImportant = 0;

	bm.bmiHeader.biWidth = Width;
	bm.bmiHeader.biHeight = Height;

	switch (Bpp) {
	case 1: bm.bmiHeader.biBitCount = 8; break;// モノクロ８ビット画像転送
	case 2:	bm.bmiHeader.biBitCount =16; break;// モノクロ１０ビット～１６ビット画像転送
	case 3:	bm.bmiHeader.biBitCount =24; break;// カラー２４ビット転送
	case 4:	bm.bmiHeader.biBitCount =32; break;// カラー３２ビット転送
	case 6:	bm.bmiHeader.biBitCount =48; break;// カラー４８転送
	case 8:	bm.bmiHeader.biBitCount =64; break;// ６４ビット転送
	}

	if (bm.bmiColors[255].rgbBlue == 0) {
		for(int i=0; i<256; i++){
			bm.bmiColors[i].rgbRed = i;
			bm.bmiColors[i].rgbGreen = i;
			bm.bmiColors[i].rgbBlue = i;
			bm.bmiColors[i].rgbReserved = 0;
		}
	}
	return((LPBITMAPINFO)&bm);
}
#endif

LRESULT CImgAnaForDSView::DrawImage(HDC hDC)//@@Hirota 最初に画像を描画
{
	if(NULL == m_pImage){
		return 0;
	}
	BOOL Flg = FALSE;
#if 1//_KOP_
	BITMAPINFO2 bm;
	LPBYTE	pImage;
#endif

	if(NULL == hDC){
		hDC = ::GetDC(
#if 1//_KOP_
			m_staImage.m_hWnd
#else
			m_hWnd
#endif
			);
		Flg = TRUE;
	}

	CPoint Pos;
#if 1//_KOP_
	if (CKOP::STAT_OF_ANALYSE != 0) {
		Pos.x = Pos.y = 0;
	}
	else {
#endif
	Pos.x = GetScrollPos(SB_HORZ);
	Pos.y = GetScrollPos(SB_VERT);
#if 1//_KOP_
	}
#endif
#if 1//_KOP_
	LPBYTE	ptmp = m_pImage;
	if (m_pImgBufBak == NULL || m_nImgBufSize != m_nImgBufBakSize) {
		if (m_pImgBufBak != NULL) {
			free(m_pImgBufBak);
		}
		m_pImgBufBak = (LPBYTE)malloc(m_nImgBufSize);
		m_nImgBufBakSize = m_nImgBufSize;
	}
	if (hDC == NULL) {
		return(0);
	}
	memcpy(m_pImgBufBak, m_pImage, m_nImgBufSize);
	pImage = m_pImgBufBak;
#endif

	if(hDC){
		LONG Width = CDS::GetWidth();
		LONG Height = CDS::GetHeight();
		LONG Bpp = CDS::GetBpp();
#if 0 //ndef _KOP_
		BITMAPINFO2 bm;
#endif
		bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bm.bmiHeader.biPlanes = 1;
		bm.bmiHeader.biCompression = BI_RGB;
		bm.bmiHeader.biSizeImage = 0;
		bm.bmiHeader.biXPelsPerMeter = 0;
		bm.bmiHeader.biYPelsPerMeter = 0;
		bm.bmiHeader.biClrUsed = 0;
		bm.bmiHeader.biClrImportant = 0;

		bm.bmiHeader.biWidth = Width;
		bm.bmiHeader.biHeight = Height;

#if 1//_KOP_
		CKOP::ON_DRAW_STA(this, pImage, GetBitMapInfo(Bpp, Width, Height));//@@Hirota 最初に画像を取得
#endif
		// モノクロ８ビット画像転送
		if(1 == Bpp){
			bm.bmiHeader.biBitCount = 8;

			for(int i=0; i<256; i++){
				bm.bmiColors[i].rgbRed = i;
				bm.bmiColors[i].rgbGreen = i;
				bm.bmiColors[i].rgbBlue = i;
				bm.bmiColors[i].rgbReserved = 0;
			}
#if 1//2017.04.01
			if ((CKOP::STAT_OF_ANALYSE == 2 || CKOP::STAT_OF_ANALYSE == 8) && CKOP::P_COLMAP == 0) {
			::SetDIBColorTable(hDC, 0, 256, CKOP::RGB_TBL2);
			memcpy(bm.bmiColors, CKOP::RGB_TBL2, sizeof(bm.bmiColors));
			}
			else
#endif
#if 1//_KOP_
			if ((CKOP::STAT_OF_ANALYSE == 2 || CKOP::STAT_OF_ANALYSE == 8) && CKOP::P_COLMAP == 1) {
			::SetDIBColorTable(hDC, 0, 256, CKOP::RGB_TBL1);
			memcpy(bm.bmiColors, CKOP::RGB_TBL1, sizeof(bm.bmiColors));
			}
			else if ((CKOP::STAT_OF_ANALYSE == 2 || CKOP::STAT_OF_ANALYSE == 8) && CKOP::P_COLMAP == 2) {
			::SetDIBColorTable(hDC, 0, 256, CKOP::RGB_TBL2);
			memcpy(bm.bmiColors, CKOP::RGB_TBL2, sizeof(bm.bmiColors));
			}
#if 1//2015.09.10
			else if (CKOP::STAT_OF_ANALYSE == 6 && CKOP::P_BIN_IMG) {
			bm.bmiColors[200].rgbRed   = 255;
			bm.bmiColors[200].rgbGreen = 0;
			bm.bmiColors[200].rgbBlue  = 0;
			bm.bmiColors[100].rgbRed   = 0;
			bm.bmiColors[100].rgbGreen = 0;
			bm.bmiColors[100].rgbBlue  = 255;

			::SetDIBColorTable(hDC, 0, 256, bm.bmiColors);
			}
#endif
			else {
#endif
			::SetDIBColorTable(hDC, 0, 256, bm.bmiColors);
			}
			if (CKOP::STAT_OF_ANALYSE != 0) {
#if 1//2015.09.10
//			SetStretchBltMode(hDC, BLACKONWHITE);//default
//			SetStretchBltMode(hDC, COLORONCOLOR);
			SetStretchBltMode(hDC, HALFTONE);
#endif
			StretchDIBits     (hDC, 0, 0, CKOP::IMG_WID, CKOP::IMG_HEI,
							0, 0, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight, pImage, (BITMAPINFO*)&bm, DIB_RGB_COLORS, SRCCOPY);
			}
			else {
			SetStretchBltMode(hDC, HALFTONE);

#if 1
			StretchDIBits     (hDC, -Pos.x, -Pos.y, GetZoomedWidth(), GetZoomedHeight(),
							0, 0, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight, pImage, (BITMAPINFO*)&bm, DIB_RGB_COLORS, SRCCOPY);
#else
			SetDIBitsToDevice(hDC, -Pos.x, -Pos.y, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight,
								0, 0, 0, bm.bmiHeader.biHeight,
								pImage,
								(BITMAPINFO*)&bm, DIB_RGB_COLORS);
#endif
			}
		}

		// モノクロ１０ビット～１６ビット画像転送
		else if(2 == Bpp){
		}
		// カラー２４ビット転送
		else if(3 == Bpp){
			bm.bmiHeader.biBitCount = 24;
#if 1//_KOP_
			if (CKOP::STAT_OF_ANALYSE != 0) {
#if 1//2015.09.10
				SetStretchBltMode(hDC, HALFTONE);
#endif
				StretchDIBits     (hDC, 0, 0, CKOP::IMG_WID, CKOP::IMG_HEI,
							0, 0, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight, pImage, (BITMAPINFO*)&bm, DIB_RGB_COLORS, SRCCOPY);
			}
			else {
#endif
				SetDIBitsToDevice(hDC, -Pos.x, -Pos.y, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight,
								0, 0, 0, bm.bmiHeader.biHeight,
#if 1//_KOP_
								pImage,
#else
								m_pImage,
#endif
								(BITMAPINFO*)&bm, DIB_RGB_COLORS);
#if 1//_KOP_
			}
#endif
		}

		// カラー３２ビット転送
		else if(4 == Bpp){
			bm.bmiHeader.biBitCount = 24;

			LPBYTE pTemp = new BYTE[bm.bmiHeader.biWidth * bm.bmiHeader.biHeight * 3];
			if(pTemp){
				for(int y=0; y<bm.bmiHeader.biHeight; y++){

					LPBYTE pDest = pTemp    + (y * bm.bmiHeader.biWidth * 3);
					LPBYTE pSrc  =
#if 1//_KOP_
								pImage
#else
								m_pImage
#endif
						+ (y * bm.bmiHeader.biWidth * 4);

					for(int x=0; x<bm.bmiHeader.biWidth; x++){
						pDest[0] = pSrc[0];
						pDest[1] = pSrc[1];
						pDest[2] = pSrc[2];

						pDest+=3;
						pSrc +=4;
					}
				}
#if 1//2017.04.01
				SetStretchBltMode(hDC, HALFTONE);
#endif
#if 1//_KOP_
				if (CKOP::STAT_OF_ANALYSE != 0) {
					StretchDIBits     (hDC, 0, 0, CKOP::IMG_WID, CKOP::IMG_HEI,
							0, 0, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight, pTemp, (BITMAPINFO*)&bm, DIB_RGB_COLORS, SRCCOPY);
				}
				else {
#endif
#if 1//2017.04.01
					StretchDIBits     (hDC, -Pos.x, -Pos.y, GetZoomedWidth(), GetZoomedHeight(),
							0, 0, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight, pTemp, (BITMAPINFO*)&bm, DIB_RGB_COLORS, SRCCOPY);
#else
				SetDIBitsToDevice(hDC, -Pos.x, -Pos.y, bm.bmiHeader.biWidth, bm.bmiHeader.biHeight,
									0, 0, 0, bm.bmiHeader.biHeight, pTemp, (BITMAPINFO*)&bm, DIB_RGB_COLORS);
#endif
#if 1//_KOP_
				}
#endif
				delete[] pTemp;
			}
		}

		// カラー４８、６４ビット転送
		else if(6 == Bpp || 8 == Bpp){
		}


		if(Flg){
#if 1//_KOP_
			::ReleaseDC(m_staImage.m_hWnd, hDC);
#else
			::ReleaseDC(m_hWnd, hDC);
#endif
		}
#if 1//_KOP_
//		CKOP::ON_DRAW_END(this, pImage, (LPBITMAPINFO)&bm);
		CKOP::ON_DRAW_END(this, pImage, GetBitMapInfo(Bpp, Width, Height));
#endif
		return 1;
	}
//araya	m_pImage = ptmp;
	return 0;
}


void CImgAnaForDSView::ChangeScrollbar(void)
{
#if 1//2015.09.10
	if (CKOP::STAT_OF_ANALYSE != 0) {
		return;
	}
#endif
	if (FALSE/*0 == m_PreviewMode*/){
		SetScrollPos(SB_HORZ, 0);
		SetScrollPos(SB_VERT, 0);
		ShowScrollBar(SB_HORZ, FALSE);
		ShowScrollBar(SB_VERT, FALSE);
		return;
	}

	CRect rc;
	GetClientRect(&rc);
	
	LONG lWidth  = GetZoomedWidth();
	LONG lHeight = GetZoomedHeight();

	LONG PosX = GetScrollPos(SB_HORZ);
	LONG PosY = GetScrollPos(SB_VERT);


	SCROLLINFO sc;
	sc.fMask	= SIF_RANGE | SIF_POS | SIF_PAGE;

	LONG OffsetH = 16;
	LONG OffsetV = 16;


	if(rc.right + OffsetH >= lWidth){
		ShowScrollBar(SB_HORZ, FALSE);
	} else {
		sc.nMin		= 0;
		sc.nMax		= lWidth;
		sc.nPage	= rc.right;
		sc.nPos		= PosX;
		SetScrollInfo(SB_HORZ, &sc);
#if 1//2015.09.10
		ShowScrollBar(SB_HORZ, TRUE);
#endif
	}

	if(rc.bottom + OffsetV >= lHeight){
		ShowScrollBar(SB_VERT, FALSE);
	} else {
		sc.nMin		= 0;
		sc.nMax		= lHeight;
		sc.nPage	= rc.bottom;
		sc.nPos		= PosY;
		SetScrollInfo(SB_VERT, &sc);
#if 1//2015.09.10
		ShowScrollBar(SB_VERT, TRUE);
#endif
	}

	if(lWidth  - rc.right  < PosX)	SetScrollPos(SB_HORZ, lWidth  - rc.right);
	if(lHeight - rc.bottom < PosY)	SetScrollPos(SB_VERT, lHeight - rc.bottom);
	if(0.0 > PosX)					SetScrollPos(SB_HORZ, 0);
	if(0.0 > PosY)					SetScrollPos(SB_VERT, 0);
}


#if 1//_KOP_
void CImgAnaForDSView::OnModeAnalyse()
{
	CRect	rt;
	GetClientRect(&rt);

	if (CKOP::STAT_OF_ANALYSE == 0) {
		LONG Bpp = CDS::GetBpp();
		switch (Bpp) {
		case 1: // モノクロ８ビット画像転送
		case 3:	// カラー２４ビット転送
		case 4:	// カラー３２ビット転送
		break;
		default:
			mlog("転送モードを8bit/24bit/32bitのいずれかにしてください.");
			return;
		break;
		}
//AA		if (m_PreviewMode != 1) {
			//OnShowCallback();
			//if (m_PreviewMode != 1) {
			//	return;
			//}
//AA		}
		if (m_nZoomTbl[m_iZoomLvl] != 100) {
			DoZoom100();
		}
	}

	if (CKOP::STAT_OF_ANALYSE == 0) {								// 20160506 Hirota comment
		CDlgSerialNo	dlg;
		dlg.m_text = GetProfileStr("APPLICATION", "SERIALNO", "");
		if (dlg.DoModal() != IDOK) {
			return;
		}
		lstrcpyn(CKOP::SERIALNO, dlg.m_text, sizeof(CKOP::SERIALNO));
		WriteProfileStr("APPLICATION", "SERIALNO", dlg.m_text);
		//mlog("シリアル番号入力画面を表示");
#if 0
		if (GetDocument()->IsGrabThreadRunning() == FALSE) {
			CSize bmpSize = m_logo.GetBitmapDimension();
			BITMAPINFOHEADER* pheader = &GetDocument()->m_bitmapInfo.bmiHeader;
   
			pheader->biWidth  = bmpSize.cx;
			pheader->biHeight =-bmpSize.cy;
		}
#endif
#if 1//2015.09.10
		//   総水平画素      総垂直画素
		//    3856            2764
		//   有効水平画素    垂直画素
		//    1088             864
		//   有効水平起点    有効垂直起点
		//    1460             960
		// 以下xxxのカスタムサイズの設定の有効水平,垂直画素値が返値として返る
		//  CDS::GetWidth() :1088
		//  CDS::GetHeight(): 864
		BOOL	ret;
		ret = 
		CKOP::PRESET_SIZE(GetBitMapInfo(CDS::GetBpp(), CDS::GetWidth(), CDS::GetHeight()));
		if (!ret) {
			return;
		}
#else
		CKOP::PRESET_SIZE(&GetDocument()->m_bitmapInfo);//img_wid, img_hei, bmp_wid, bmp_hei);
#endif
		m_staImage.MoveWindow(10, 30, CKOP::IMG_WID, CKOP::IMG_HEI);

#if 0
		//ZOOM状態を合わせる
		float	RATE = 100.0f * (float)CKOP::IMG_WID / (float)CKOP::BMP_WID;

		m_zoomLevelIndex = 6;//50%
        for (unsigned int i=0; i < m_zoomLevelTable.size() - 1; i++) {
			float	zoom = m_zoomLevelTable[i];
			if (fabs(zoom - RATE) <= 0.1) {
				m_zoomLevelIndex = i;
				break;//HIT
			}
		}
		m_zoomLevel = m_zoomLevelTable[m_zoomLevelIndex] / 100.0f;
//		AdjustViewSize(rt.Width(), rt.Height());
#endif
#if 1//2018.05.01
		if (G_SS.USE_SURUGA) {
			CString	path(GetDirDoc());
			path += "\\DS102.INI";
			STG::SET_INI_PATH(path);
			STG::LOAD_OFFLINE();
			STG::LOAD_DSTBL();
			if (STG::OPEN()) {
				STG::SET_PARAM(-1);
			}
		}
#endif
#if 1//2018.05.01
		SetTimer(1, 125/*ms*/, NULL);
#endif
//		CKOP::STAT_OF_ANALYSE = 1;
		CKOP::MOVE_FORM(this, 1);
//		CKOP::CREATE_FORM(this, 1);
#if 1//2015.09.10
		ShowScrollBar(SB_HORZ, FALSE);
		ShowScrollBar(SB_VERT, FALSE);
#endif
#if 1//2017.04.01
		CKOP::INIT_MITSUTOYO(this);
#endif
	}
	else {
#if 1//2017.04.01
		CKOP::TERM_MITSUTOYO();
#endif
#if 1//2018.05.01
		if (G_SS.USE_SURUGA) {
			STG::CLOSE();
		}
#endif
		CKOP::MOVE_FORM(this, 0);
		//CKOP::STAT_OF_ANALYSE = 0;
		//CKOP::DESTROY_FORM(this);
#if 1//2015.09.10
		ChangeScrollbar();
#endif
#if 1//2016.01.01
		Invalidate();
#endif
	}
//	AdjustViewSize(rt.Width(), rt.Height());
	if (CKOP::STAT_OF_ANALYSE != 0) {
		CKOP::PRESET(this);
	}
#if 1
	CWnd*	pWnd = AfxGetMainWnd();
	CMenu*	pMenu = pWnd->GetMenu();
	if (CKOP::STAT_OF_ANALYSE != 0) {
	}
	else {
	SetScrollPos(SB_HORZ, 0);
	SetScrollPos(SB_VERT, 0);
	CRect	rt;
	GetClientRect(&rt);
	m_staImage.MoveWindow(0, 0, rt.Width(), rt.Height());
	}
	// メニューバーの再描画
	pWnd->DrawMenuBar();
#endif
}
void CImgAnaForDSView::OnUpdateModeAnalyse(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CKOP::STAT_OF_ANALYSE == 0);//20160517 Hirota comment メニューを使用禁止にする(TRUEにすれば使用可能になる)
}
void CImgAnaForDSView::OnButtons(UINT nID)
{
	switch (CKOP::STAT_OF_ANALYSE) {
	case 1://メニュー画面
		switch (nID) {
		case IDC_BUTTON6://戻る
			OnModeAnalyse();
			//OnAnalyse();
		break;
		default:
			mlog("nID=%d", nID);
		break;
		}
	break;
	case 2://シェーデング解析
	case 3://面積＆重心
	case 4://倍率解析
	case 5://ＭＴＦ解析
	case 8://シェーデング解析拡張
	default:
		mlog("nID=%d", nID);
	break;
	}
}


void CImgAnaForDSView::OnStopByCode()
{
	// 動画の時のみ止める
	if (TRUE/*0 == m_PreviewMode || 1 == m_PreviewMode*/){
		if(!m_bPAUSE){
			OnCommands(ID_CAP_PAUSE);
		}
	}
}
void CImgAnaForDSView::OnCommands(UINT nID)
{
	switch (nID) {
	case ID_SET_FILTER:
#if 1//2017.03.20
		if (G_PD != NULL) {
			G_PD->PD_SHOW_PROP(m_hWnd, -1);
			break;
		}
#endif
		CDS::TEST02(NULL, AfxGetMainWnd()->m_hWnd);
	break;
	case ID_SET_FORMAT:
		if (m_bRUN) {
			OnCommands(ID_CAP_STOP);
			CDS::TEST09(NULL, AfxGetMainWnd()->m_hWnd);
			OnCommands(ID_CAP_START);
		}
		else {
			CDS::TEST09(NULL, AfxGetMainWnd()->m_hWnd);
		}
	break;
	case ID_CAP_START:
#if 1//2018.05.01
		//::QueryPerformanceCounter(&m_fps_r.tk1);
		m_fps_s.tk1.QuadPart = m_fps_r.tk1.QuadPart = m_fps_d.tk1.QuadPart = 0;
		m_fps_s.cnt = m_fps_r.cnt = m_fps_d.cnt = 0;
#endif
#if 1//2017.03.20
		m_fid = m_fps_cnt = 0;
		m_fps = C_NAN();
		if (G_PD != NULL) {
#if 1//2018.05.01
			if (G_PD->PD_SET_TPF != NULL && G_SS.SKIP_TPF == 0) {
				G_PD->PD_SET_TPF(G_SS.FRM_MINTIME);
			}
#endif
			if (!G_PD->PD_RUN(m_hWnd)) {
				return;
			}
		}
		else {
#endif
		if (!CDS::RENDER()) {
			return;
		}
		CDS::SET_CALLBACK(FALSE);
#if 1//2018.05.01
		if (G_SS.SKIP_TPF == 0) {
			CDS::SET_TPF(G_SS.FRM_MINTIME);
		}
#endif
		if (!CDS::RUN()) {
			return;
		}
#if 1//2017.03.20
		}
#endif
		// 映像用の領域を開放
		SAFE_DELETE(m_pImage);

		m_pImage = new BYTE[CDS::GetSize()];
#if 1//_KOP_
		m_nImgBufSize = CDS::GetSize();
#endif
		ChangeScrollbar();
		Invalidate();

#if 1//2017.03.20
		if (G_PD == NULL) {
#endif
		CDS::SET_CALLBACK(TRUE);
#if 1//2017.03.20
		}
#endif
		m_bRUN = TRUE;
		m_bPAUSE = FALSE;
		DispStsText(STATUS_SW);
		DispStsText(STATUS_FRAME);
	break;
	case ID_CAP_STOP:
#if 1//2017.03.20
		if (G_PD != NULL) {
			m_bRUN = FALSE;
TRACE("%d:STOP\n", GetTickCount());
			G_PD->PD_STOP();
		}
		else {
#endif
		CDS::SET_CALLBACK(FALSE);
		CDS::STOP();
#if 1//2017.03.20
		}
#endif
		m_bRUN = FALSE;
		m_bPAUSE = FALSE;
	break;
	case ID_CAP_PAUSE:
#if 1//2017.03.20
		if (G_PD != NULL) {
			if (m_bPAUSE){
				G_PD->PD_PAUSE();
				m_bPAUSE = FALSE;
			}
			else {
				G_PD->PD_PAUSE();
				m_bPAUSE = TRUE;
			}
		}
		else {
#endif
		if (m_bPAUSE){
#if 1//2018.05.01
			if (G_SS.SKIP_TPF == 0) {
				CDS::SET_TPF(G_SS.FRM_MINTIME);
			}
#endif
			CDS::RUN();
			m_bPAUSE = FALSE;
		}
		else {
			CDS::PAUSE();
			m_bPAUSE = TRUE;
		}
#if 1//2017.03.20
		}
#endif
	break;
	case ID_SET_SYSTEM:
	{
		CPrpSheetEx prp(this, 0, &G_SS);
		if (prp.DoModal() == IDOK) {
			((CImgAnaForDSApp*)AfxGetApp())->WriteProfile();
		}
	}
	break;
	case ID_ZOOM_UP:
		DoZoomUp();
	break;
	case ID_ZOOM_DOWN:
		DoZoomDown();
	break;
	case ID_ZOOM_100:
		DoZoom100();
	break;
	case ID_SAVE_IMAGE:
		SaveImage();
	break;
	}
}
void CImgAnaForDSView::OnCommandsU(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID) {
	case ID_SET_FORMAT:
#if 1//2017.03.20
		if (G_PD != NULL) {
			pCmdUI->Enable(FALSE);
			break;
		}
#endif
		pCmdUI->Enable(CKOP::STAT_OF_ANALYSE == 0);
	break;
	case ID_SET_FILTER:
		pCmdUI->Enable(CKOP::STAT_OF_ANALYSE == 0 && CDS::m_bOFFLINE == FALSE);
	break;
	case ID_CAP_START:
		pCmdUI->Enable(m_bRUN == FALSE && CDS::m_bOFFLINE == FALSE);
	break;
	case ID_CAP_STOP:
		pCmdUI->Enable(m_bRUN == TRUE && CDS::m_bOFFLINE == FALSE);
	break;
	case ID_CAP_PAUSE:
		pCmdUI->Enable(m_bRUN == TRUE && CDS::m_bOFFLINE == FALSE);
		pCmdUI->SetCheck(m_bPAUSE == TRUE);
	break;
	case ID_SET_SYSTEM:
		pCmdUI->Enable(TRUE);
	break;
	case ID_ZOOM_UP:
	case ID_ZOOM_DOWN:
	case ID_ZOOM_100:
		pCmdUI->Enable(CKOP::STAT_OF_ANALYSE == 0);
	break;
	case ID_SAVE_IMAGE:
//		pCmdUI->Enable(FALSE);
		pCmdUI->Enable(TRUE);
	break;
	}
}
/*
void CImgAnaForDSView::OnDebugImage(UINT nID)
{
	LONG Width = CDS::GetWidth();
	LONG Height = CDS::GetHeight();
	LONG Bpp = CDS::GetBpp();
	LPBYTE	p;
	int	q = 0;
	int	ofs;
#if 1//2015.09.10
	static
	int	chk = 0;
#endif

	if (m_pImage == NULL) {
	// 取り込み用領域確保
	m_pImage = new BYTE[CDS::GetSize()];
	m_nImgBufSize = CDS::GetSize();
	}
	if (m_pImage == NULL) {
		return;
	}
	if (m_nImgBufSize != ( Width * Height * Bpp)) {
		mlog("buffer size error!");
		free(m_pImage);
		m_pImage = NULL;
		return;
	}
	p = m_pImage;

	switch (nID) {
	case ID_DEBUG_IMAGE1:
	{
		memset(p, 0xff, Width * Height * Bpp);
		for (int y = 0; y < Height; y++) {
			for (int x = 0; x < Width; x++, q++) {
				if (
#if 1
					(y/50+(x/30)) % 5 == 0
#else
					(q/30) % 50
#endif
					) {
					ofs = x + y * Width;
					ofs*= Bpp;
					for (int j = 0; j < Bpp; j++) {
						*(p+ofs+j) = 0x00;
					}
				}
			}
		}
		DrawImage(NULL);
	}
	break;
	case ID_DEBUG_IMAGE2:
#if 1//2015.09.10
	case ID_DEBUG_IMAGE3:
	{
		int	DIV = 50;
		memset(p, 0xcc, Width * Height * Bpp);
		for (int y = 0; y < Height; y++) {
			if ((((y+chk)/DIV)%2) != 1) {
				continue;
			}
			for (int x = 0; x < Width; x++) {
				if ((((x+chk)/DIV)%2)==1) {
					ofs = x + y * Width;
					ofs*= Bpp;
					for (int j = 0; j < Bpp; j++) {
						*(p+ofs+j) = 0x00;
					}
				}
			}
		}
		if (nID == ID_DEBUG_IMAGE3) {
		for (int y = 230; y < 230+100; y++) {
			for (int x = 230; x <= 230+100; x++) {
				ofs = (x+chk*3) + (y+chk*3) * Width;
				ofs*= Bpp;
				for (int j = 0; j < Bpp; j++) {
					*(p+ofs+j) = 0x00;
				}
			}
		}
		}
		DrawImage(NULL);
		if ((chk += 7) >= 100) {
			chk = 0;
		}
	}
#endif
	break;
	}
}*/
#endif


#if 1
LRESULT CImgAnaForDSView::OnCallBack(WPARAM wParam, LPARAM lParam)
{
	static
	double	RAT = 0.75;
	m_fid++;

	::QueryPerformanceCounter(&m_fps_r.tk2);
	m_fps_s.tk2 = m_fps_d.tk2 = m_fps_r.tk2;
#if 1//2018.05.01
	if (++m_fps_r.cnt >= 10 || m_fps_r.GET_MS() >= 1000) {
		m_fps_r.fps = m_fps_r.GET_FPS();
		m_fps_r.tk1 = m_fps_r.tk2;
		m_fps_r.cnt = 0;
	}
#endif
	double	ms = m_fps_s.GET_MS();
	if (ms < G_SS.FRM_MINTIME*RAT) {
		m_skip_cnt++;
		return(TRUE);//このフレームをスキップする
	}
	else {
		m_fps_s.tk1 = m_fps_s.tk2;
	}
	if (++m_fps_d.cnt >= 10 || m_fps_d.GET_MS() >= 1000) {
		m_fps_d.fps = m_fps_d.GET_FPS();
		m_fps_d.tk1 = m_fps_d.tk2;
		m_fps_d.cnt = 0;
	}
	LPBYTE	pbuf = (LPBYTE)wParam;
	long	size = (int)lParam;
	if (m_pImage == NULL) {
		m_pImage = new BYTE[CDS::GetSize()];
		m_nImgBufSize = CDS::GetSize();
	}
	if (m_pImage != NULL && CDS::GetSize() == size) {
		memcpy(m_pImage, pbuf, size);
		DrawImage(NULL);
	}
	DispStsText(STATUS_FPS);
	return 1;
}
#else
LRESULT CImgAnaForDSView::OnCallBack(WPARAM wParam, LPARAM lParam)
{
#if 1//2017.07.18
	m_fid++;
	switch (++m_fps_cnt) {
	case  1:
		m_fps_tk1 = GetTickCount();
	break;
	case 11:
		m_fps_tk2 = GetTickCount();
		m_fps = 10.0/((m_fps_tk2-m_fps_tk1)/1000.0);
		m_fps_cnt = 0;
	break;
	}
#endif
#if 1//_KOP_
#if 0//2018.05.01
	const
#endif
	DWORD	MIN_TICK_TIME = G_SS.FRM_MINTIME;//300;
#if 1//2018.05.01
	MIN_TICK_TIME = (DWORD)(MIN_TICK_TIME*0.75);
#endif
	m_tk1 = GetTickCount();
	if ((m_tk1-m_tk2) < MIN_TICK_TIME) {
		m_skip_cnt++;
#if 0
		m_buf += I2S(m_skip_cnt);
		if (m_buf.GetLength() > 10) {
			m_buf.Delete(0, 1);
		}
AfxGetMainWnd()->SetWindowText(m_buf);
#endif
		return(TRUE);//このフレームをスキップする
	}
	if (m_skip_cnt > 0) {
		m_skip_cnt = 0;
#if 0
		m_buf += I2S(m_skip_cnt);
		if (m_buf.GetLength() > 10) {
			m_buf.Delete(0, 1);
		}
AfxGetMainWnd()->SetWindowText(m_buf);
#endif
	}
	else {
#if 0
		m_buf += "*";
		if (m_buf.GetLength() > 10) {
			m_buf.Delete(0, 1);
		}
AfxGetMainWnd()->SetWindowText(m_buf);
#endif
	}
#endif
#if 1//2018.05.01
	if (++m_fps_prc.cnt >= 10) {
		int	tic = GetTickCount();
		m_fps_prc.fps = 1000.0 * m_fps_prc.cnt / (tic-m_fps_prc.tic);
		m_fps_prc.tic = tic;
		m_fps_prc.cnt = 0;
	}
#endif
#if 1//2018.05.01
	LPBYTE	pbuf = (LPBYTE)wParam;
	long	size = (int)lParam;
	if (m_pImage == NULL) {
		m_pImage = new BYTE[CDS::GetSize()];
		m_nImgBufSize = CDS::GetSize();
	}
	if (m_pImage != NULL && CDS::GetSize() == size) {
		memcpy(m_pImage, pbuf, size);
		DrawImage(NULL);
	}
#endif
#if 1//_KOP_
	m_tk2 = GetTickCount();
#endif
#if 1//2017.07.18
	DispStsText(STATUS_FPS);
#endif
	return 1;
}
#endif
#if 1//2017.03.20
LRESULT CImgAnaForDSView::OnPluginCB(WPARAM wParam, LPARAM lParam)
{
TRACE("%d:OnPluginCB\n", GetTickCount());
	if (m_bRUN == FALSE) {
		return(TRUE);
	}
	return(OnCallBack(wParam, m_bmi.bmiHeader.biSizeImage));
}
#endif


BOOL CImgAnaForDSView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (nFlags & MK_CONTROL && CKOP::STAT_OF_ANALYSE == 0) {
		if (zDelta < 0) {
			DoZoomDown();
		}
		else {
			DoZoomUp();
		}
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CImgAnaForDSView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (CKOP::STAT_OF_ANALYSE == 0) {
		DoZoom100();
	}
	CView::OnMButtonDown(nFlags, point);
}
void CImgAnaForDSView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if (nChar == 0x1c) {
		CDS::TEST00();
	}

	CView::OnChar(nChar, nRepCnt, nFlags);
}
int CImgAnaForDSView::GetZoomedWidth(void)
{
	return(CDS::GetWidth() * m_nZoomTbl[m_iZoomLvl] / 100);
}
int CImgAnaForDSView::GetZoomedHeight(void)
{
	return(CDS::GetHeight() * m_nZoomTbl[m_iZoomLvl] / 100);
}
void CImgAnaForDSView::DispStsText(int i, int p1, int p2)
{
	CString	buf;
	switch (i) {
#if 1//2017.07.18
	case STATUS_FPS:
#if 1//2018.05.01
		buf.Format("FPS:%4.1lf(%4.1lf),IDX:%d", m_fps_d.fps, m_fps_r.fps, m_fid);
#else
		buf.Format("FPS:%4.1lf,IDX:%d", m_fps, m_fid);
#endif
	break;
#endif
	case STATUS_ZOOM:
		buf.Format("Zoom:%d%%", m_nZoomTbl[m_iZoomLvl]);
	break;
	case STATUS_SW:
		buf = CDS::GetSizeStr();
	break;
	case STATUS_FRAME:
		buf = CDS::GetName();
	break;
	}
	//static_cast<CMainFrame*>(theApp.m_pMainWnd)->SetStatusText(STATUS_SW   , CDS::GetSizeStr());
	//static_cast<CMainFrame*>(theApp.m_pMainWnd)->SetStatusText(STATUS_FRAME, CDS::GetName());
	static_cast<CMainFrame*>(theApp.m_pMainWnd)->SetStatusText(i, buf);
}
void CImgAnaForDSView::DoZoomUp(void)
{
	if (m_iZoomLvl < (ROWS(m_nZoomTbl)-1)) {
		m_iZoomLvl++;
		DispStsText(STATUS_ZOOM);
		ChangeScrollbar();
		Invalidate();
	}
}
void CImgAnaForDSView::DoZoomDown(void)
{
	if (m_iZoomLvl > 0) {
		m_iZoomLvl--;
		DispStsText(STATUS_ZOOM);
		ChangeScrollbar();
		Invalidate();
	}
}
void CImgAnaForDSView::DoZoom100(void)
{
	m_iZoomLvl = 4;
	DispStsText(STATUS_ZOOM);
	ChangeScrollbar();
	Invalidate();
}
#if 1//2017.03.20
BOOL GET_IMAGE_INFO(LPCTSTR pszFileName, BITMAP *pBITMAP)
{
	HBITMAP	hBMP = CKOP::LOAD_BITMAP(pszFileName);
	if (hBMP == NULL) {
#if 1
		HRESULT	hr;
		CImage	img;
		hr = img.Load(pszFileName);
		if (FAILED(hr)) {
		return(FALSE);
		}
		hBMP = (HBITMAP)img;
		CBitmap	bmp;

		bmp.Attach(hBMP);
		bmp.GetBitmap(pBITMAP);

		bmp.Detach();
#else
		return(FALSE);
#endif
	}
	CBitmap	bmp;

	bmp.Attach(hBMP);
	bmp.GetBitmap(pBITMAP);

	bmp.Detach();
	DeleteObject(hBMP);
	return(TRUE);
}
BOOL LOAD_IMAGE(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP)
{
	HBITMAP	hBMP = CKOP::LOAD_BITMAP(pszFileName);
	if (hBMP == NULL) {
		return(FALSE);
	}
	BOOL	rc = FALSE;
	CBitmap	bmp;
	BITMAP	bitmap;
	int		cnt = width * height;
	int		size;
	LPBYTE	p, po = NULL;

	bmp.Attach(hBMP);

	bmp.GetBitmap(&bitmap);
	if (bitmap.bmWidth != width || bitmap.bmHeight != height) {
		goto skip;
	}
	if (bitmap.bmBitsPixel != 8 && bitmap.bmBitsPixel != 24) {
		goto skip;
	}
	size = cnt*bitmap.bmBitsPixel/8;
	if ((po = (LPBYTE)malloc(size)) == NULL) {
		goto skip;
	}
	bmp.GetBitmapBits(size, po);

	if (BPP != 1 && BPP != 3 && BPP != 4) {
		goto skip;
	}
	if (BPP == 1) {
		if (bitmap.bmBitsPixel == 8) {
			memcpy(pBuf, po, size);
		}
		else {
			p = po;
			for (int i = 0; i < size; i+=3) {
				*pBuf++ = *p++;
						   p++;
						   p++;
			}
		}
	}
	else if (BPP == 4) {
		if (bitmap.bmBitsPixel == 32) {
			memcpy(pBuf, po, size);
		}
		else {
			p = po;
			for (int i = 0; i < size; i+=3, p+=3) {
				*pBuf++ = *p  ;
				*pBuf++ = *p  ;
				*pBuf++ = *p  ;
				*pBuf++ = 255 ;
			}
		}
	}
	else {
		if (bitmap.bmBitsPixel == 24) {
			memcpy(pBuf, po, size);
		}
		else {
			p = po;
			for (int i = 0; i < size; i++) {
				*pBuf++ = *p  ;
				*pBuf++ = *p  ;
				*pBuf++ = *p++;
			}
		}
	}
	rc = TRUE;
skip:
	if (po != NULL) {
		free(po);
	}
	bmp.Detach();
	DeleteObject(hBMP);
	return(rc);
}
void DEBUG_FILE_LOAD(LPBYTE DBG_BUF)
{
	WIN32_FIND_DATA
			fd;
	HANDLE	h;
	CString	path(GetDirDoc());
	CString	file;
	int		q,
			cnt = 0;
	BOOL	bFAILED = FALSE;
	static
	int		DBG_FILE_IDX;
	static
	int		DBG_IMG_BPP;
	static
	int		DBG_IMG_WIDTH;
	static
	int		DBG_IMG_HEIGHT;

	path += "\\OFFLINE";

	if (!::PathFileExists(GetDirPath(path))) {
		::CreateDirectory(GetDirPath(path), NULL);
	}

	path += "\\*.bmp";

	while (TRUE) {
		q = 0;
		h = ::FindFirstFile(path, &fd);
		if (h == INVALID_HANDLE_VALUE) {
			return;
		}
		while (TRUE) {
			cnt++;
			if (q == DBG_FILE_IDX) {
				//hit
				CString	file(GetDirDoc());
				BITMAP	bitmap;
				file += "\\OFFLINE\\";
				file += fd.cFileName;

				GET_IMAGE_INFO(file, &bitmap);
				DBG_IMG_BPP     = bitmap.bmBitsPixel /8;
				DBG_IMG_WIDTH   = bitmap.bmWidth;
				if (bitmap.bmHeight < 0) {
				DBG_IMG_HEIGHT  = -bitmap.bmHeight;
				}
				else {
				DBG_IMG_HEIGHT  =  bitmap.bmHeight;
				}
				//---
				if (DBG_IMG_WIDTH != G_SS.OFF_WIDTH || G_SS.OFF_HEIGHT != G_SS.OFF_HEIGHT) {
					//skip
					bFAILED = TRUE;
					DBG_FILE_IDX++;
				}
				else
				//---
				if (DBG_IMG_WIDTH > MAX_WIDTH || DBG_IMG_HEIGHT > MAX_HEIGHT || DBG_IMG_BPP > 4) {
					//skip
					bFAILED = TRUE;
					DBG_FILE_IDX++;
				}
				else {
					LOAD_IMAGE(file, DBG_BUF, DBG_IMG_WIDTH, DBG_IMG_HEIGHT, 1/*DBG_IMG_BPP*/);
					DBG_FILE_IDX++;
					FindClose(h);
					return;
				}
			}
			if (!FindNextFileA(h, &fd)) {
				FindClose(h);
				cnt = q+1;
				if (DBG_FILE_IDX >= cnt) {
					DBG_FILE_IDX %= cnt;
				}
				else if (bFAILED) {
				}
				break;
			}
			q++;
		}
	}
}
#endif
void CImgAnaForDSView::SetOfflineImage(void)
{
#if 1//2017.03.20
	DEBUG_FILE_LOAD(m_pImage);
#else
	CImage	img;
	CDC*	pDC;
	CRect	rt(0, 0, G_SS.OFF_WIDTH, G_SS.OFF_HEIGHT);
	RGBQUAD	rgb[256];

	for (int i = 0; i < ROWS(rgb); i++) {
		rgb[i].rgbBlue = rgb[i].rgbGreen = rgb[i].rgbRed = i;
		rgb[i].rgbReserved = 0;
	}
	img.Create(G_SS.OFF_WIDTH, G_SS.OFF_HEIGHT, 8, 0);

	img.SetColorTable(0, 256, rgb);

	pDC = CDC::FromHandle(img.GetDC());
	pDC->FillSolidRect(0, 0, G_SS.OFF_WIDTH, G_SS.OFF_HEIGHT, RGB(16,16,16));
	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(_T("OFFLINE"), &rt, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	img.ReleaseDC();

	int	width, height, bpp;
	LPVOID	pt, pb, pv;
	int	n1, n2;

	width  = img.GetWidth();
	height = img.GetHeight();
	bpp    = img.GetBPP();
	pt     = img.GetPixelAddress(0, 0);
	pb     = img.GetPixelAddress(0, height-1);     
	pv     = img.GetBits();
	n1     = img.GetPixel(0, 0);
	n2     = img.GetPixel(width-1, height-1);     


	if (img.GetPitch() >= 0) {
		pv = pt;
	}
	else {
		pv = pb;
	}
	memcpy(m_pImage, pv, m_nImgBufSize);
#endif
}
void CImgAnaForDSView::SaveImage(void)
{
	LPBYTE	p = (LPBYTE)malloc(m_nImgBufSize);
	if (!p) {
		return;
	}
	memcpy(p, m_pImage, m_nImgBufSize);

	CFileDialog	dlg(FALSE,		/* open:true, save:false */
					"BMP",		/* default extention */
					m_strPath,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_EXPLORER | OFN_ENABLESIZING
						,
					"BMP Files(*.bmp)|*.bmp|All Files (*.*)|*.*||",
					NULL);		/* owner */

	if (dlg.DoModal() != IDOK) {
		goto skip;
	}

	m_strPath = dlg.GetPathName();
	CKOP_DEBUG::SAVE_IMAGE(m_strPath, p, CDS::GetWidth(), CDS::GetHeight(), CDS::GetBpp());
skip:
	if (p != NULL) {
		free(p);
	}
	return;
}
#if 1//2018.05.01

BOOL CImgAnaForDSView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
/*	if (wParam == IDC_SLIDER1) {
		lParam = lParam;
		int	q = ((NMHDR*)lParam)->code;
		TRACE("%d::CODE:%d\n", GetTickCount(), q);
		if (q == TRBN_THUMBPOSCHANGING) {
		TRACE("HERE\n");
		}
	}*/
	if (CKOP::STAT_OF_ANALYSE != 0) {
		LONG ret;
		if (ret = CKOP::NOTIFY_MSG(this, (UINT)wParam, (LPNMHDR)lParam, pResult)) {
			return(ret);
		}
	}
	return CView::OnNotify(wParam, lParam, pResult);
}
/*
void CImgAnaForDSView::OnDebugImage(UINT nID)
{
	switch (nID) {
	case ID_DEBUG_VIDEOIN:
		CKOP_3D::DBG_FILEIN  = 0;
		Invalidate();
	break;
	case ID_DEBUG_FILEIN:
		if (CKOP_3D::DBG_FILEIN != 1) {
			CKOP_3D::DBG_FILE_IDX = 0;

			CKOP_3D::DEBUG_FILE_LOAD();
			CKOP_3D::DBG_FILEIN = 1;
			CKOP_3D::DBG_PAUSE = 0;
			G_bSKIP_FRAME = 1;//全フレームスキップ
			Invalidate();
		}
	break;
	case ID_DEBUG_PAUSE:
		if (CKOP_3D::DBG_PAUSE) {
			CKOP_3D::SET_SKIP_BY_FILTER(CKOP_3DMAIN::P_FILTER);
			CKOP_3D::DBG_PAUSE = 0;
		}
		else {
			G_bSKIP_FRAME = 1;//全フレームスキップ
			CKOP_3D::DBG_PAUSE = 1;
		}
	break;
	case ID_DEBUG_NEXT:
		if (CKOP_3D::DBG_FILEIN) {
			CKOP_3D::DEBUG_FILE_LOAD();
			Invalidate();
		}
		else {
			CKOP_3D::DBG_FRAME_IDX = G_FRAME_IDX_OF_GRAB;
			G_bSKIP_FRAME = 0;//全フレーム入力
			CKOP_3D::DBG_REQ_CODE = 2;
		}
	break;
	case ID_DEBUG_SAVE:
		CKOP_3D::DBG_REQ_CODE = 1;
		Invalidate();
	break;
	}
}
void CImgAnaForDSView::OnUpdateDebugImage(CCmdUI *pCmdUI)
{
	switch (pCmdUI->m_nID) {
	case ID_DEBUG_VIDEOIN:
		pCmdUI->SetCheck(CKOP_3D::DBG_FILEIN == 0);
	break;
	case ID_DEBUG_FILEIN:
		pCmdUI->SetCheck(CKOP_3D::DBG_FILEIN != 0);
	break;
	case ID_DEBUG_PAUSE:
		pCmdUI->SetCheck(CKOP_3D::DBG_PAUSE  != 0);
	break;
	case ID_DEBUG_NEXT:
		pCmdUI->Enable(CKOP_3D::DBG_PAUSE != 0 || CKOP_3D::DBG_FILEIN != 0);
	break;
	case ID_DEBUG_SAVE:
		pCmdUI->Enable(TRUE);
	break;
	}
}
*/


HBRUSH CImgAnaForDSView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CView::OnCtlColor(pDC, pWnd, nCtlColor);
	if (CKOP::STAT_OF_ANALYSE != 0) {
		HBRUSH ret;
		if ((ret = CKOP::CTL_COLOR(pDC, pWnd)) != NULL) {
			return(ret);
		}
	}
	// TODO:  既定値を使用したくない場合は別のブラシを返します。
	return hbr;
}

#endif