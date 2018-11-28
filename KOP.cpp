//**************************************************************************************
// 画像解析共通処理
//--------------------------------------------------------------------------------------
// 20170401 VER 2.0.0.0 機能追加対応
//**************************************************************************************

#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_MENU.h"
#include "KOP_SHADING.h"
#include "KOP_CIRCLE.h"
#include "KOP_SCALE.h"
#include "KOP_MTF.h"
#include "KOP_TVD.h"
#if 1//2017.04.01
#include "KOP_ROLL.h"
#endif
#include "EX.h"
#if 1//2018.05.01
#include "STG.h"
#endif

#include <math.h>
#include <atlimage.h>

#include <Gdiplusimaging.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#ifdef _DEBUG
//#pragma comment(lib, "FlyCapture2/lib/FlyCapture2d_v100.lib")
//#pragma comment(lib, "FlyCapture2/lib/FlyCapture2GUId_v100.lib")
#else
//#pragma comment(lib, "FlyCapture2/lib/FlyCapture2_v100.lib")
//#pragma comment(lib, "FlyCapture2/lib/FlyCapture2GUI_v100.lib")
#endif

#include "opencv/cv.h"
#include "opencv/highgui.h"
#ifdef _DEBUG
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#else
#pragma comment(lib, "opencv_core2411.lib")
#pragma comment(lib, "opencv_imgproc2411.lib")
#endif
#if 1//2018.05.01
CDlgProgress
			CKOP::DLG_PRG;
BOOL		G_bCANCEL;
#endif
BOOL		G_bDEBUG=TRUE;
//BOOL		G_bDEBUG = FALSE;
BOOL		G_bTRACE;
BOOL		G_bSTEP;
BOOL		G_bPROG;
HWND		G_hPROG;

int		CKOP::STAT_OF_ANALYSE = 0;
int		CKOP::IMG_WID;
int		CKOP::IMG_HEI;
int		CKOP::BMP_WID;
int		CKOP::BMP_HEI;
double	CKOP::RX;
double	CKOP::RY;

int		CKOP::BMP_BIT_CNT;
int		CKOP::BMP_BYTE_CNT;
BOOL	CKOP::m_bBOTTOMUP;
const
int		CKOP::BASE_WID = 644;
const
int		CKOP::BASE_HEI = 482;
#if 0//2016.01.01
//BYTE	CKOP::IMG_DEF[640*480*4];
BYTE	CKOP::IMG_DEF[2448*2048*4];
#endif


CPen	CKOP::m_penBlue;

CBitmap	CKOP::BMP_DEF;
CBrush	CKOP::m_brsBack;
CPen	CKOP::m_penRed;
CPen	CKOP::m_penGreen;
CPen	CKOP::m_penRedDot;
CPen	CKOP::m_penGreenDot;

CPen	CKOP::m_penExtra;

CRect	CKOP::IMG_RECT;
TCHAR	CKOP::SAVE_DIR[MAX_PATH];
TCHAR	CKOP::SERIALNO[64];
//------------------------------------
#if 1//2015.07.24
int		CKOP::P_COLMAP;
RGBQUAD	CKOP::RGB_TBL1[256];	//COLMAP1用(固定)
RGBQUAD	CKOP::RGB_TBL2[256];	//COLMAP2用(MAX,MINにより毎回編集)
#endif
#if 1//2015.09.10
int		CKOP::P_BIN_IMG;
#endif
#if 1//2017.04.01
FIXEDPARA	CKOP::FP;
#endif
//------------------------------------
int		CKOP::STAT_OF_DISPATCH = 0;
//------------------------------------
//------------------------------------

FTBL fnts[] = {
	{/*0*/""               ,   0, 0        },
	{/*1*/"ＭＳ Ｐゴシック", 100, FW_NORMAL},
	{/*2*/"ＭＳ Ｐゴシック", 130, FW_NORMAL},
	{/*3*/"ＭＳ Ｐゴシック", 130, FW_BOLD  },
	{/*4*/"ＭＳ Ｐゴシック", 150, FW_NORMAL},
	{/*5*/"ＭＳ Ｐゴシック", 150, FW_BOLD  },
	{/*6*/"ＭＳ Ｐゴシック",  80, FW_NORMAL},
#if 1//2015.09.10
	{/*7*/"ＭＳ Ｐゴシック", 120, FW_NORMAL},
#endif
	{/*8*/"ＭＳ Ｐゴシック", 180, FW_BOLD  },
#if 1//2017.04.01
	{/*9*/"ＭＳ Ｐゴシック", 110, FW_BOLD  },
#endif
	{/*Z*/NULL             ,   0, 0}
};

LPCTBL	pTBL;

//
// OpenCVテスト用画面定義
//
CTBL tbl9[] = {
	{ 0, 0  , IDC_RADIO1 , 0, "生画像"            , 990,  10, 140, 70, BS_AUTORADIOBUTTON|BS_PUSHLIKE|WS_GROUP},
	{ 0, 0  , IDC_RADIO2 , 0, "cvThreshold\r(Th=64)"      , 990,  90, 140, 70, BS_AUTORADIOBUTTON|BS_PUSHLIKE         },
	{ 0, 0  , IDC_RADIO3 , 0, "cvSmooth\r(Gauss:9x9)"         , 990, 170, 140, 70, BS_AUTORADIOBUTTON|BS_PUSHLIKE         },
	{ 0, 0  , IDC_BUTTON7, 0, "保存"              , 990, 460, 140, 70},
	{ 0, 0  , IDC_BUTTON8, 0, "戻る"              , 990, 540, 140, 70},
	{ 0, 0  , IDC_STATIC1, 0, "シェーディング解析",  10,   3, 644, 22,                     SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 0  , IDC_STATIC1, 0, "Ser.No:201506012"  , 680,   3, 300, 22, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 0  ,           0, 0, NULL}
};

void /*CKOP::*/INIT(void)
{
	LOGFONT		lf;
	CClientDC	dc(AfxGetMainWnd());
#if 1//2017.04.01
	if (fnts[0].fnt.GetSafeHandle() != NULL) {
		return;
	}
#endif
	memset(&lf, 0, sizeof(lf));

	for (LPFTBL p = fnts; p->face != NULL; p++) {
		if (p->fnt.GetSafeHandle() != NULL) {
			p->fnt.DeleteObject();
		}
		lstrcpyn(lf.lfFaceName, p->face, sizeof(lf.lfFaceName));
		lf.lfHeight = p->point;
		lf.lfWeight = p->bold;
		p->fnt.CreatePointFontIndirect(&lf, &dc);
	}
}
void ITEM_SET(CWnd *pWnd, LPITBL pit, int nID)
{
	for (LPITBL p = pit; p->nID != 0; p++) {
		if (p->nID == nID) {
			((CComboBox*)pWnd)->AddString((LPCTSTR)p->p);
		}
	}
}
CFont *CKOP::GET_FONT(int i)
{
	return(&fnts[i].fnt);
}
/*
                  (460)
670 --------------------------------1130
       (230)       900     (230)
670 ---(???)--------|---------------1130
*/
void CKOP::CREATE_FORM(CWnd *pWnd, LPCTBL pct, LPITBL pit)
{
	CRect	rt;
	int		cx, cy;
	int		gap_x = 0;
	int		gap_y = 0;
	int		gap_cx = 0;
	int		gap_cy = 0;
	int		q = 0;
#if 1
	int		off_x = 0,
			off_y = 0;
#endif
	pTBL = pct;

	INIT();
	for (LPCTBL pt = pTBL; pt->nID != 0; pt++, q++) {
		UINT	nID = pt->nID;
#if 0//2016.01.01
		if (pt->nID < 0) {
			off_x = pt->x;
			off_y = pt->y;
			continue;
		}
#endif
		if (nID < 0) {
			continue;//2016.01.01(UINTだからここにはこない...)
		}
		rt.left  = pt->x + gap_x;
		rt.top   = pt->y + gap_y;
#if 1
		rt.left += off_x;
		rt.top  += off_y;
#endif
		cx = pt->cx;
		cy = pt->cy;

		if (pt->offs & 1) {
			cx = CKOP::IMG_WID;
		}
		if (pt->offs & 2) {
			cy = CKOP::IMG_HEI;
		}
		if (pt->offs & 4) {
			rt.left += (CKOP::IMG_WID-BASE_WID);
		}
		if (pt->offs & 8) {
			rt.top  += (CKOP::IMG_HEI-BASE_HEI);
		}
		rt.right = cx + rt.left + gap_cx;
		rt.bottom= cy + rt.top  + gap_cy;
		if (FALSE) {
		}
		else if (nID >= IDC_BUTTON1 && nID <= IDC_BUTTON40) {
			pt->p = new CButton();
//				pt->p = new CMFCButton();
//				((CButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_MULTILINE, rt, pWnd, nID);
//			((CMFCButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_MULTILINE, rt, pWnd, nID);
			((CButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_MULTILINE, rt, pWnd, nID);
#if 0
			((CMFCButton*)pt->p)->SetFaceColor(RGB(164,0,0),true);
			((CMFCButton*)pt->p)->SetTextColor(RGB(255,255,255));
#endif
		}
		else if (nID >= IDC_RADIO1 && nID <= IDC_RADIO10) {
#if 1
			pt->p = new CMFCButton();
			((CMFCButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_MULTILINE|pt->st, rt, pWnd, nID);
#else
			pt->p = new CButton();
			((CButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_MULTILINE|pt->st, rt, pWnd, nID);
#endif
		}
		else if (nID >= IDC_CHECK1 && nID <= IDC_CHECK10) {
			pt->p = new CButton();
			((CButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|pt->st, rt, pWnd, nID);
		}
		else if ((nID >= IDC_STATIC1 && nID <=
#if 1//2017.04.01
			IDC_STATIC60
#else
			IDC_STATIC40
#endif
			) || nID == IDC_STATIC) {
			pt->p = new CStatic();
			((CStatic*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|pt->st, rt, pWnd, nID);
		}
		else if (nID >= IDC_COMBO1 && nID <= IDC_COMBO10) {
			pt->p = new CComboBox();
			rt.bottom += 300;

			((CComboBox*)pt->p)->Create(WS_CHILD|WS_VISIBLE|0|CBS_DROPDOWNLIST|pt->st, rt, pWnd, nID);

			if (pit != NULL) {
			ITEM_SET(pt->p, pit, nID);
			}
		}
		else if (nID >= IDC_EDIT1 && nID <= IDC_EDIT20) {
			pt->p = new CEdit();
			((CEdit*)pt->p)->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|pt->st, rt, pWnd, nID);
			if (pt->cap != NULL) {
			((CEdit*)pt->p)->SetWindowText(pt->cap);
			}
		}
		else if (nID >= IDC_SPIN1 && nID <= IDC_SPIN10) {
			pt->p = new CSpinButtonCtrl();
			((CSpinButtonCtrl*)pt->p)->Create(WS_CHILD|WS_VISIBLE|pt->st, rt, pWnd, nID);
			((CSpinButtonCtrl*)pt->p)->SetBuddy((pt-1)->p);
			int max = HIWORD((LONG)pt->cap);
			int	min = LOWORD((LONG)pt->cap);
#if 1
			if (max == 0 && min == 0) {
			min = pt->x;
			max = pt->y;
			}
#endif
			((CSpinButtonCtrl*)pt->p)->SetRange(min, max);
		}
#if 1//2015.09.25
		else if (nID >= IDC_GROUP1 && nID <= IDC_GROUP10) {
			pt->p = new CButton();
//			pt->p = new CGourpBoxEx();
			((CButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_GROUPBOX|pt->st, rt, pWnd, nID);
			((CButton*)pt->p)->ModifyStyleEx(0, WS_EX_TRANSPARENT);
		}
		else if (nID >= IDC_MFCBTN1 && nID <= IDC_MFCBTN5) {
			pt->p = new CMFCButton();
			((CMFCButton*)pt->p)->Create(pt->cap, WS_CHILD|WS_VISIBLE|BS_MULTILINE, rt, pWnd, nID);
#if 0
			((CMFCButton*)pt->p)->SetFaceColor(RGB(164,0,0),true);
			((CMFCButton*)pt->p)->SetTextColor(RGB(255,255,255));
#endif
		}
#endif
		else if (nID >= IDC_SLIDER1 && nID <= IDC_SLIDER1) {
			pt->p = new CSliderCtrl();
			((CSliderCtrl*)pt->p)->Create(WS_CHILD|WS_VISIBLE|pt->st, rt, pWnd, nID);
			//((CSliderCtrl*)pt->p)->SetBuddy((pt-1)->p);
			int max = HIWORD((LONG)pt->cap);
			int	min = LOWORD((LONG)pt->cap);
			((CSliderCtrl*)pt->p)->SetRange(min, max);
		}
		else {
			mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
			continue;
		}
		if (pt->ifnt > 0) {
			CFont*	pfnt;
			LOGFONT	lf;
			memset(&lf, 0, sizeof(lf));
			pfnt = pt->p->GetFont();
			if (pfnt->GetSafeHandle() != NULL) {
				pfnt->GetLogFont(&lf);
			}
			pt->p->SetFont(&fnts[pt->ifnt].fnt);
		}
	}
	q++;
	pTBL = (LPCTBL)malloc(sizeof(CTBL)*q);
	memcpy(pTBL, pct, sizeof(CTBL)*q);
	//if (n == 1) {
	//	CKOP::m_pnt_r.x = GetProfileINT("SAVED", "RBAR:X", BASE_WID/2+10);
	//	CKOP::m_pnt_r.y = GetProfileINT("SAVED", "RBAR:Y", BASE_HEI/2+10);
	//	CKOP::m_pnt_g.x = GetProfileINT("SAVED", "GBAR:X", BASE_WID/2-10);
	//	CKOP::m_pnt_g.y = GetProfileINT("SAVED", "GBAR:Y", BASE_HEI/2-10);
	//}
	//if (n == 2) {
	//	int	n1 = GetProfileINT("SAVED2", "FILTER", 1);
	//	int	n2 = GetProfileINT("SAVED2", "PARAM" , 0);
	//	((CComboBox*)pWnd->GetDlgItem(IDC_COMBO1))->SetCurSel(n1);
	//	((CComboBox*)pWnd->GetDlgItem(IDC_COMBO2))->SetCurSel(n2);
	//}
/*	if (!m_graph.Create("", WS_CHILD|WS_VISIBLE|SS_NOTIFY|SS_WHITEFRAME, rt, this, IDC_LIST2)) {
		//return;
	}*/
//	WriteProfileStr("APPLICATION", "NANIKA", "TEST");
}

void CKOP::DESTROY_FORM(CWnd *pWnd)
{
	if (pTBL == NULL) {
		return;
	}
	for (LPCTBL pt = pTBL; pt->nID != 0; pt++) {
		if (pt->p != NULL) {
			pt->p->DestroyWindow();
			delete pt->p;
			pt->p = NULL;
		}
#if 1
		else if (pt->nID < 0) {
			continue;
		}
#endif
		else {
			mlog("TEST");
			CWnd	*p = pWnd->GetDlgItem(pt->nID);
			p->DestroyWindow();
			delete p;
		}
		//p->p->DestroyWindow();
		//delete p->p;
	}
//	mlog("OpenCVライブラリのてきとうな呼び出し");
//	if (pTBL == tbl1) {
//		WriteProfileINT("SAVED", "RBAR:X", CKOP::m_pnt_r.x);
//		WriteProfileINT("SAVED", "RBAR:Y", CKOP::m_pnt_r.y);
//		WriteProfileINT("SAVED", "GBAR:X", CKOP::m_pnt_g.x);
//		WriteProfileINT("SAVED", "GBAR:Y", CKOP::m_pnt_g.y);
//	}
//	if (pTBL == tbl2) {
////		WriteProfileINT("SAVED2", "FILTER", m_nCurFil);
////		WriteProfileINT("SAVED2", "PARAM" , m_nCurPar);
//	}
	free(pTBL);
	pTBL = NULL;
}
void CKOP::MOVE_FORM(CWnd* pWndForm, int iNextPage)	//@@20160517 Hirota ここで機能を選択 
{
	LPCTBL pct;
	LPITBL pit=NULL;
	//CTBL TVD_TBL[] = {
	CTBL MENU_TBL[] = {
		{ 0, 0  ,            0, 0, NULL}
	};
	//{ 0, 0  ,            0, 0, NULL}
	//};
	switch (CKOP::STAT_OF_ANALYSE) {
	case 1: CKOP_MENU   ::TERM_FORM(); break;
	case 2: CKOP_SHADING::TERM_FORM(); break;
	case 3: CKOP_CIRCLE ::TERM_FORM(); break;
	case 4: CKOP_SCALE  ::TERM_FORM(); break;
	case 5: CKOP_MTF    ::TERM_FORM(); break;
	case 6: CKOP_TVD	::TERM_FORM(); break;
#if 1//2017.04.01
	case 7: CKOP_ROLL   ::TERM_FORM(); break;
#endif
	}
	if (CKOP::STAT_OF_ANALYSE > 0) {
	CKOP::DESTROY_FORM(pWndForm);
	}
	//----------
	switch (CKOP::STAT_OF_ANALYSE = iNextPage) {
	case 1:
#if 1//2017.04.01
		pct = CKOP_MENU::ctbl;
#else
		pct = MENU_TBL;//CKOP_MENU::ctbl;
#endif
		pit = CKOP_MENU::itbl;
	break;
	case 2:
		pct = CKOP_SHADING::ctbl;
		pit = CKOP_SHADING::itbl;
	break;
	case 3:
		pct = CKOP_CIRCLE::ctbl;
		pit = CKOP_CIRCLE::itbl;
	break;
	case 4:
		pct = CKOP_SCALE::ctbl;
		pit = CKOP_SCALE::itbl;
	break;
	case 5:
		pct = CKOP_MTF::ctbl;
		pit = CKOP_MTF::itbl;
	break;
	case 6:
		pct = CKOP_TVD::ctbl;
//		pct = TVD_TBL;
		pit = CKOP_TVD::itbl;
	break;
#if 1//2017.04.01
	case 7:
		pct = CKOP_ROLL::ctbl;
		pit = CKOP_ROLL::itbl;
	break;
#endif
	}
	//----------
	if (CKOP::STAT_OF_ANALYSE <= 0) {
#if 1//2015.09.10
		CKOP_SHADING::TERM_CORRECTION();
#endif
		return;
	}
	pWndForm->EnableWindow(FALSE);
	CKOP::CREATE_FORM(pWndForm, pct, pit);
#if 0//2015.07.28
	pWndForm->ValidateRect(NULL);
#else
	pWndForm->Invalidate();
#endif
	pWndForm->EnableWindow(TRUE);
	//----------
	switch (CKOP::STAT_OF_ANALYSE) {
	case 1:
		CKOP_MENU::INIT_FORM(pWndForm);
	break;
	case 2:
		CKOP_SHADING::INIT_FORM(pWndForm);
	break;
	case 3:
		CKOP_CIRCLE::INIT_FORM(pWndForm);
	break;
	case 4:
		CKOP_SCALE::INIT_FORM(pWndForm);
	break;
	case 5:
		CKOP_MTF::INIT_FORM(pWndForm);
	break;
	case 6:
		CKOP_TVD::INIT_FORM(pWndForm);
	break;
#if 1//2017.04.01
	case 7:
		CKOP_ROLL::INIT_FORM(pWndForm);
	break;
#endif
	}
}

int CKOP::GET_PIXEL(int x, int y, LPBYTE pImgPxl)
{
	int		unit = CKOP::BMP_BIT_CNT/8;
	LPBYTE	bp = pImgPxl;
	int		ret;

	if (unit <= 0) {
		unit = 1;//8bit以下はないと思うが...
	}
	if (m_bBOTTOMUP) {
		//the bitmap is a bottom-up DIB and its origin is the lower left corner. 
		bp += (CKOP::BMP_HEI - y - 1) * unit * CKOP::BMP_WID;
	}
	else {
		//the bitmap is a top-down DIB and its origin is the upper left corner. 
		bp += y * unit * CKOP::BMP_WID;
	}
	bp += x * unit;
	switch (unit) {
	case 3:
	case 4:
		ret = *bp;
	break;
	case 1:
	default:
		ret = *bp;
	break;
	}
	if (ret > 255) {
		ret = ret;
	}
	return(ret);
}

void CKOP::ToClinet(HWND hWnd, HWND hWndClient, LPRECT p)
{
//	::ClientToScreen(hWnd, p);
//	::ScreenToClient(hWndClient, p);
	MapWindowPoints(
			hWnd,     // 変換元ウィンドウのハンドル
			hWndClient,       // 変換先ウィンドウのハンドル
			(LPPOINT)p,  // 変換対象の点からなる配列
			2			// 配列内の点の数
	);
}

void CKOP::ToClinet(HWND hWnd, HWND hWndClient, LPPOINT p)
{
	::ClientToScreen(hWnd, p);
	::ScreenToClient(hWndClient, p);
}
/********************************************************************/

//
// IMAGE座標系からBMP座標系に変換
//
void CKOP::IMGCD_TO_BMPCD(LPRECT ri, LPRECT rb)
{
	rb->left   = (int)(ri->left   * CKOP::RX);
	rb->right  = (int)(ri->right  * CKOP::RX);
	rb->top    = (int)(ri->top    * CKOP::RY);
	rb->bottom = (int)(ri->bottom * CKOP::RY);
}
void CKOP::IMGCD_TO_BMPCD(LPPOINT ri, LPPOINT rb)
{
	rb->x      = (int)(ri->x      * CKOP::RX);
	rb->y      = (int)(ri->y      * CKOP::RY);
}
//
// BMP座標系からIMAGE座標系に変換
//
void CKOP::BMPCD_TO_IMGCD(LPRECT rb, LPRECT ri)
{
	ri->left   = (int)(rb->left   / CKOP::RX);
	ri->right  = (int)(rb->right  / CKOP::RX);
	ri->top    = (int)(rb->top    / CKOP::RY);
	ri->bottom = (int)(rb->bottom / CKOP::RY);
}
LPRECT CKOP::BMPCD_TO_IMGCD(LPRECT rb)
{
	static
	RECT	ri;
	BMPCD_TO_IMGCD(rb, &ri);
	return(&ri);
}
//
// BMP座標系からIMAGE座標系に変換
//
void CKOP::BMPCD_TO_IMGCD(LPPOINT rb, LPPOINT ri)
{
	ri->x   = (int)(rb->x   / CKOP::RX);
	ri->y   = (int)(rb->y   / CKOP::RY);
}
LPPOINT CKOP::BMPCD_TO_IMGCD(LPPOINT rb)
{
	static
	POINT	ri;
	BMPCD_TO_IMGCD(rb, &ri);
	return(&ri);
}
LPPOINT CKOP::IMGCD_TO_BMPCD(LPPOINT ri)
{
	static
	POINT	rb;
	IMGCD_TO_BMPCD(ri, &rb);
	return(&rb);
}
#if 1//2018.05.01
void CKOP::NORMALIZE_RECT(LPRECT ri)
{
	RECT	rb;

	if (ri->left <= ri->right) {
		rb.left  = ri->left ;
		rb.right = ri->right;
	}
	else {
		rb.left  = ri->right;
		rb.right = ri->left ;
	}
	if (ri->top <= ri->bottom) {
		rb.top    = ri->top;
		rb.bottom = ri->bottom;
	}
	else {
		rb.top    = ri->bottom;
		rb.bottom = ri->top;
	}
	memcpy(ri, &rb, sizeof(rb));
}
void CKOP::RECT_TO_CVRECT(LPRECT ri, void *rt)
{
	CvRect *ro = (CvRect*)rt;
	ro->x      = ri->left;
	ro->width  = ri->right  - ri->left;
	ro->y      = ri->top;
	ro->height = ri->bottom - ri->top;
}
#endif



void CKOP::DRAW_MARQUEE(CDC *pDC, LPRECT pr)
{
	DRAW_MARQUEE(pDC, pr->left, pr->top, pr->right-1, pr->bottom-1);
}
void CKOP::DRAW_MARQUEE(CDC *pDC, int x1, int y1, int x2, int y2)
{
	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y1);
	pDC->LineTo(x2, y2);
	pDC->LineTo(x1, y2);
	pDC->LineTo(x1, y1);
}
// ppnt: ZOOM描画の中心座標
// zoom: 倍率(1,2,3,5)
void CKOP::DRAW_ZOOM_EX(CWnd *pWnd, LPPOINT ppnt, LPBYTE pImgPxl, LPBITMAPINFO pBmpInfo, int zoom)
{
	CDC *pDC = pWnd->GetDC();
	CRect	rt;
	int		wid, hei;
	int		wid_i = CKOP::BMP_WID;
    int		hei_i = CKOP::BMP_HEI;
	POINT	pnt = *ppnt;

	pWnd->GetWindowRect(&rt);
//pnt.y = 0;
	wid = rt.Width();
	hei = rt.Height();

	int		sw = wid/zoom;
	int		sh = hei/zoom;
	int		ox = pnt.x;// * CKOP::BMP_WID/CKOP::IMG_WID;
	int		oy = pnt.y;// * CKOP::BMP_HEI/CKOP::IMG_HEI;
	int		sx = ox - sw/2;
	int		sy = oy - sh/2;
	if (FALSE) {
	ox = pnt.x * CKOP::RX;
	oy = pnt.y * CKOP::RY;
	sx = ox - sw/2;
	sy = oy - sh/2;
	}
	sy = CKOP::BMP_HEI-(sy+sh)-1;
//	sh = -sh;
#if 0
	sy = BMP_HEI-180+90;
	sh = 180;
#endif
//	pDC->FillSolidRect(0,0,wid,hei, RGB(255,0,255));

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
LPCTSTR CKOP::SERIAL_STR(void)
{
	static
	TCHAR	buf[96];
	sprintf_s(buf, _countof(buf), "Ser.No:%s", CKOP::SERIALNO);
	return(buf);
}
#if 1//2015.09.10

#define MAX_IMG_SIZE	(645)
#define MIN_IMG_SIZE	(500)
#define MAX_DIV_NUM		(10)

static
BOOL CHK_SIZE(int w, int h)
{
	if (w >= MIN_IMG_SIZE && w <= MAX_IMG_SIZE) {
		return(TRUE);
	}
	return(FALSE);
}
BOOL CKOP::PRESET_SIZE(LPBITMAPINFO pBmpInfo)
#else
void CKOP::PRESET_SIZE(LPBITMAPINFO pBmpInfo)
#endif
{
    int		bmp_wid = pBmpInfo->bmiHeader.biWidth;
    int		bmp_hei = pBmpInfo->bmiHeader.biHeight;
	int		img_wid = bmp_wid;
	int		img_hei = bmp_hei;

#if 1//2016.01.01(とりあえず640x480固定にする)
	img_wid = G_SS.ANA_WIDTH;
	img_hei = G_SS.ANA_HEIGHT;
#else
#if 1//2015.09.10
	int		i;

	for (i = 1; i <= MAX_DIV_NUM; i++) {
		if (!(img_wid % i) && !(img_hei % i)) {
			if (CHK_SIZE(img_wid/i, img_hei/i)) {
				img_wid/=i;
				img_hei/=i;
				break;
			}
		}
	}
	if (i > MAX_DIV_NUM) {
		mlog("有効画素サイズを整数(1～10のいずれか)で割った時に、"
			 "水平画素サイズが%d-%dの範囲に入るように設定してください。"
			 "また整数での割り算時には余りがないようにサイズを指定してください。",
			 MIN_IMG_SIZE, MAX_IMG_SIZE);
		return(FALSE);
	}
#else
	while (img_wid > 700) {
		img_wid/=2;
		img_hei/=2;
	}
	if (img_wid < 600) {
		img_wid = 640;
		img_hei = (int)((double)bmp_hei / ((double)bmp_wid/(double)img_wid));
	}
#endif
#endif
	if (bmp_hei < 0) {
		//上下反対せずにメモリに格納(先頭データがイメージの左上ピクセル)
		bmp_hei = -bmp_hei;
		img_hei = -img_hei;
		m_bBOTTOMUP = FALSE;
	}
	else {
		//上下反対にメモリに格納(先頭データがイメージの左下ピクセル)
		m_bBOTTOMUP = TRUE;
	}
	CKOP::BMP_WID = bmp_wid;
	CKOP::BMP_HEI = bmp_hei;
	CKOP::BMP_BIT_CNT  = pBmpInfo->bmiHeader.biBitCount;
	CKOP::BMP_BYTE_CNT = CKOP::BMP_BIT_CNT/8;
	CKOP::IMG_WID = img_wid;
	CKOP::IMG_HEI = img_hei;
#if 1//2016.01.01
	CKOP::RX = (double)CKOP::BMP_WID/(double)CKOP::IMG_WID;
	CKOP::RY = (double)CKOP::BMP_HEI/(double)CKOP::IMG_HEI;
#else
	CKOP::RX = CKOP::BMP_WID/CKOP::IMG_WID;
	CKOP::RY = CKOP::BMP_HEI/CKOP::IMG_HEI;
#endif
	//CKOP::m_pnt_r.x = GetProfileInt("SAVED", "RBAR:X", BASE_WID/2+10);
	//CKOP::m_pnt_r.y = GetProfileInt("SAVED", "RBAR:Y", BASE_HEI/2+10);
	//CKOP::m_pnt_g.x = GetProfileInt("SAVED", "GBAR:X", BASE_WID/2-10);
	//CKOP::m_pnt_g.y = GetProfileInt("SAVED", "GBAR:Y", BASE_HEI/2-10);
#if 1//2015.09.10
	CKOP_SHADING::INIT_CORRECTION();
	return(TRUE);
#endif
}
void CKOP::PRESET(CWnd* pWndForm)
{
	CWnd*	pWndImg = pWndForm->GetDlgItem(IDC_IMAGEAREA);//20160512 Hirota comment (ここで最初のダイアログの画像情報を取得)

	pWndImg->GetWindowRect(&IMG_RECT);
	pWndForm->ScreenToClient(&IMG_RECT);

	if (m_brsBack.GetSafeHandle()) {
		m_brsBack.DeleteObject();
	}
	m_brsBack.CreateSolidBrush(RGB(128,0,0));

	if (m_penRed.GetSafeHandle()) {
		m_penRed.DeleteObject();
	}
	if (m_penGreen.GetSafeHandle()) {
		m_penGreen.DeleteObject();
	}
	if (m_penRedDot.GetSafeHandle()) {
		m_penRedDot.DeleteObject();
	}
	if (m_penGreenDot.GetSafeHandle()) {
		m_penGreenDot.DeleteObject();
	}
#if 1//2017.04.01
	if (m_penBlue.GetSafeHandle()) {
		m_penBlue.DeleteObject();
	}
#endif
	//20160509 Hirota add to draw blue ROI.(MTF) Start
	/*
	if (m_penBlueDot.GetSafeHandle()) {
		m_penBlueDot.DeleteObject();
	}
	*/
	//20160509 Hirota add to draw blue ROI.(MTF) End

	if (m_penExtra.GetSafeHandle()) {
		m_penExtra.DeleteObject();
	}

	//20160427 Hirota add to draw blue line.(Graph) Start
	m_penBlue     .CreatePen(PS_SOLID, 1 , RGB(0,0,255));
	//20160427 Hirota add to draw blue line.(Graph) End

	m_penRed     .CreatePen(PS_SOLID, 1 , RGB(255,0,0));
	m_penGreen   .CreatePen(PS_SOLID, 1 , RGB(0,255,0));
	m_penRedDot  .CreatePen(PS_DOT  , 1 , RGB(64,255,255));
	m_penGreenDot.CreatePen(PS_DOT  , 1 , RGB(0,0,255));

	//20160509 Hirota add to draw blue ROI.(MTF) Start
	/*
	m_penBlueDot .CreatePen(PS_DOT  , 1 , RGB(0,0,255));
	*/
	//20160509 Hirota add to draw blue ROI.(MTF) End

	m_penExtra   .CreatePen(PS_DOT  , 1 , RGB(0,0,255));
	lstrcpyn(
		CKOP::SAVE_DIR, GetProfileStr("APPLICATION", "SAVE_DIR", "#1#2#3#"), sizeof(CKOP::SAVE_DIR));
	if (!lstrcmpi(SAVE_DIR, "#1#2#3#")) {
		lstrcpyn(CKOP::SAVE_DIR, GetDirDoc(), sizeof(CKOP::SAVE_DIR));
		WriteProfileStr("APPLICATION", "SAVE_DIR", CKOP::SAVE_DIR);
	}
//SAVE_DIR=C:\Users\Public\Documents\KOP\FlyCapture2
}
void CKOP::GDEF_INIT(LPGDEF pdef, int bitLBL, CWnd* pWnd, LPRECT poff)
{
	CRect	rt;

	pWnd->GetWindowRect(&rt);
	rt.OffsetRect(-rt.left, -rt.top);//(0,0)基準にする
	pdef->rt_cl = rt;
	//---
	rt.top    += 5;
	rt.bottom -= 7;
	rt.left   += 5;
	rt.right  -= 5;	
	if (bitLBL & 1) {
		//X軸にラベル表示
		rt.bottom -= 8;
		rt.right  -= 7;
	}
	if (bitLBL & 2) {
		//Y軸にラベル表示
		rt.left += 16;//+7:符号があるとき
		rt.top  += 1;
	}
	if (poff != NULL) {
		rt.top    += poff->top;
		rt.bottom += poff->bottom;
		rt.left   += poff->left;
		rt.right  += poff->right;
	}
	//---
	pdef->rt_gr = rt;
	//---
	pdef->rt_wid = rt.Width();
	pdef->rt_hei = rt.Height();
	//---
	pdef->bitLBL = bitLBL;
}
void CKOP::GDEF_PSET(LPGDEF pdef, int xmin, int xmax, int xtic, int ymin, int ymax, int ytic)
{
	pdef->xmin = xmin;
	pdef->xmax = xmax;
	pdef->ymin = ymin;
	pdef->ymax = ymax;
	pdef->xtic = xtic;
	pdef->ytic = ytic;
	//---
	pdef->xwid = xmax-xmin;
	pdef->yhei = ymax-ymin;
}
int CKOP::GDEF_XPOS(LPGDEF pdef, double f)
{
	int	x;
	x = (int)(pdef->rt_gr.left + pdef->rt_wid * (f-pdef->xmin)/(pdef->xwid));
	return(x);
}
int CKOP::GDEF_YPOS(LPGDEF pdef, double f)
{
	int	y;
	y = (int)(pdef->rt_gr.bottom - pdef->rt_hei * (f-pdef->ymin)/(pdef->yhei));
	return(y);
}
void CKOP::GDEF_GRID(LPGDEF pdef,CDC *pDC)
{
	CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CFont*	old_fnt = (CFont*)pDC->SelectObject(CKOP::GET_FONT(6));
	CRect	rt;
	TCHAR	fmt[16], buf[32];
	BOOL	bZERO;
	BOOL	bMAXMIN;
//	double	ep=0.1;
	int		dot, tmp;
	const
	int		SLEN=3;	
#if 1//2015.07.24
	pDC->FillSolidRect(0, 0, pdef->rt_cl.right-2, pdef->rt_cl.bottom-2, RGB(224,224,224));
#else
	pDC->FillSolidRect(&pdef->rt_cl, RGB(224,224,224));
#endif
	pDC->FillSolidRect(&pdef->rt_gr, RGB(255,255,255));
	Draw4Edge(pDC, &pdef->rt_gr);
	//--------------------------------
	rt = pdef->rt_gr;
	rt.right = rt.left-SLEN+1;
	rt.left  = rt.right-100;
	//Y軸のめもり描画
	CEX::EnumGridFirst(pdef->ymax, pdef->ymin, pdef->ytic, pdef->rt_hei, fmt, _countof(fmt), /*bALIGN=*/FALSE);
	while (CEX::EnumGridNext(&dot, buf, _countof(buf), &bZERO, 0.1, &bMAXMIN)) {
		tmp = pdef->rt_gr.bottom - dot;
		rt.top    = tmp - 300;
		rt.bottom = tmp + 300;

//		pDC->SelectObject(&rpen);//太
		pDC->MoveTo(pdef->rt_gr.left     , tmp);
		pDC->LineTo(pdef->rt_gr.left-SLEN, tmp);
		if (pdef->bitLBL & 2) {
		pDC->DrawText(buf, -1, &rt, DT_SINGLELINE|DT_RIGHT|DT_VCENTER);
		}
		if (bMAXMIN) {
		continue;
		}
		if (FALSE) {
		pDC->MoveTo(pdef->rt_gr.left , tmp);
		pDC->LineTo(pdef->rt_gr.right, tmp);
		}
	}
	//Ｘ軸のめもり描画
	//--------------------------------
	rt = pdef->rt_gr;
	rt.top    = rt.bottom+SLEN;
	rt.bottom = rt.top + 100;
	//--------------------------------
	if (TRUE) {
//		int		d;
		BOOL	bZERO, bMAXMIN;

		CEX::EnumGridFirst(pdef->xmax, pdef->xmin, pdef->xtic, pdef->rt_wid, fmt, _countof(fmt));
		while (CEX::EnumGridNext(&dot, buf, _countof(buf), &bZERO, 0.1, &bMAXMIN)) {
			tmp = pdef->rt_gr.left + dot;
			rt.left  = tmp - 500;
			rt.right = tmp + 500;

//			pDC->SelectObject(&rpen);//太
			pDC->MoveTo(tmp, pdef->rt_gr.bottom     );
			pDC->LineTo(tmp, pdef->rt_gr.bottom+SLEN);
			if (pdef->bitLBL & 1) {
			pDC->DrawText(buf, -1, &rt, DT_SINGLELINE|DT_CENTER|DT_TOP);
			}
			if (bMAXMIN) {
			continue;
			}
			//if (!bZERO) {
			//pDC->SelectObject(&gpen);//細
			//}
			if (FALSE) {
			pDC->MoveTo(tmp, pdef->rt_gr.top   );
			pDC->LineTo(tmp, pdef->rt_gr.bottom);
			}
		}
	}

	pDC->SelectObject(old_fnt);
	pDC->SelectObject(old_pen);
}

HBITMAP CKOP::LOAD_BITMAP(LPCTSTR pszFileName)
{
	HANDLE	h;
	
	h = LoadImage(
			NULL,   // インスタンスのハンドル
			pszFileName,  // イメージの名前または識別子
			IMAGE_BITMAP,        // イメージのタイプ
			0,     // 希望する幅
			0,     // 希望する高さ
			0
			|LR_LOADFROMFILE		// ロードのオプション
			|LR_CREATEDIBSECTION
		);

	return((HBITMAP)h);
}
void CKOP::SAVE_BITMAP(HBITMAP hbmp,  LPCTSTR pszFileName)
{
	CImage	img;
	TCHAR	buf[4];
	int		l;

	if (!::PathFileExists(GetDirPath(pszFileName))) {
		::CreateDirectory(GetDirPath(pszFileName), NULL);
	}
	img.Attach(hbmp);

	if ((l = lstrlen(pszFileName)) >= 4) {
		lstrcpy(buf, &pszFileName[l-3]);
	}
	else {
		buf[0] = '\0';
	}
	if (!lstrcmpi(buf, "png")) {
		img.Save(pszFileName, Gdiplus::ImageFormatPNG);
	}
	else if (!lstrcmpi(buf, "jpg")) {
		img.Save(pszFileName, Gdiplus::ImageFormatJPEG);
	}
	else {
		img.Save(pszFileName, Gdiplus::ImageFormatBMP);
	}
#if 1//2015.09.10
	if (TRUE) {
		DWORD err = GetLastError();
		if (err != 0) {
TRACE("GetLastError():%d\n", err);
		}
	}
#endif
	img.Detach();
}
void CKOP::SAVE_WINDOW(CWnd *pWnd, LPCTSTR pszFileName)
{
	if (pWnd == NULL) {
		return;
	}
//	CDC		*pDC = pWnd->GetDC();//クライアント領域のみ
	CDC		*pDC = pWnd->GetWindowDC();//ウィンドウ全体
	CDC		mdc;
	CRect	rt;
	int		w, h;
	CBitmap	bmp, *old_bmp;
#if 1//2016.01.01
	CString	path(G_SS.SAVE_DIR);
#else
	CString	path(CKOP::SAVE_DIR);
#endif
	if (path.Right(1) != "\\") {
		path += "\\";
	}
	path += CKOP::SERIALNO;
	path += "_";
	path += pszFileName;

	pWnd->GetWindowRect(&rt);
	w = rt.Width();
	h = rt.Height();
	// メモリDCにpWndをコピー
	bmp.CreateCompatibleBitmap(pDC, w, h);
	mdc.CreateCompatibleDC(pDC);
	old_bmp = mdc.SelectObject(&bmp);
//	mdc.FillSolidRect(0, 0, w, h, RGB(255,0,0));
#if 1
	PrintWindow(pWnd->m_hWnd, mdc.m_hDC, /*nFlags*/0);
#else
	//タイトルバーがうまくコピーされない(Aeroの影響)
	mdc.BitBlt(10, 10, w, h, pDC, 0, 0, SRCCOPY);
#endif
	mdc.SelectObject(old_bmp);

	SAVE_BITMAP((HBITMAP)bmp.m_hObject, path);

//	bmp.DeleteObject();
//	mdc.DeleteDC();

	pWnd->ReleaseDC(pDC);
	::MessageBeep(MB_OK);
}
#if 1//2017.07.18
void CKOP::SAVE_CSV(CCSV *p, LPCTSTR pszFileName)
{
	CString	path(G_SS.SAVE_DIR);

	if (path.Right(1) != "\\") {
		path += "\\";
	}
	path += CKOP::SERIALNO;
	path += "_";
	path += pszFileName;

	p->save(path);
}
#endif
#if 1//2018.05.01
void CKOP::SAVEADD_CSV(CCSV *p, LPCTSTR pszFileName)
{
	CString	path(G_SS.SAVE_DIR);

	if (path.Right(1) != "\\") {
		path += "\\";
	}
	path += CKOP::SERIALNO;
	path += "_";
	path += pszFileName;

	p->saveadd(path);
}
#endif

void CKOP::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	if (STAT_OF_DISPATCH) {
		TRACE("CKOP::ON_DRAW_STA:%d\n", STAT_OF_DISPATCH);
		return;
	}
	if (!CKOP::STAT_OF_ANALYSE) {
		return;
	}
//ZZ	STAT_OF_DISPATCH = 10;

	switch (CKOP::STAT_OF_ANALYSE) {
	case 1://メニュー画面
		CKOP_MENU::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);	//20160513 Hirota comment (最初のダイアログに画像表示等)
	break;
	case 2://シェーディング画面
		CKOP_SHADING::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 3://円の重心と面積
		CKOP_CIRCLE::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 4://倍率解析
		CKOP_SCALE::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 5:
		CKOP_MTF::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 6:
		CKOP_TVD::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);
	break;
#if 1//2017.04.01
	case 7://カメラ傾き
		CKOP_ROLL::ON_DRAW_STA(pWndForm,  pImgPxl, pbmpinfo);
	break;
#endif
	default://OpenCVテスト画面
	break;
	}
	STAT_OF_DISPATCH = 0;
}

void CKOP::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);

	if (STAT_OF_DISPATCH) {
		TRACE("CKOP::ON_DRAW_END:%d\n", STAT_OF_DISPATCH);
		return;
	}
	if (!CKOP::STAT_OF_ANALYSE) {
		return;
	}
//ZZ	STAT_OF_DISPATCH = 20;

	switch (CKOP::STAT_OF_ANALYSE) {
	case 1://メニュー画面
		CKOP_MENU::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 2://シェーディング画面
		CKOP_SHADING::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 3://円の重心と面積
		CKOP_CIRCLE::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 4://倍率解析
		CKOP_SCALE::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 5:
		CKOP_MTF::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
	case 6:
		CKOP_TVD::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
#if 1//2017.04.01
	case 7://カメラ傾き
		CKOP_ROLL::ON_DRAW_END(pWndForm,  pImgPxl, pbmpinfo);
	break;
#endif
	}
	STAT_OF_DISPATCH = 0;
}

//20160517 Hirota comment (ここから各機能とのコマンドメッセージやりとりをしている。)
BOOL CKOP::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	BOOL	ret = FALSE;

	if (STAT_OF_DISPATCH) {
//		TRACE("CKOP::CMD_MSG:%d\n", STAT_OF_DISPATCH);
		return(FALSE);
	}
//ZZ	STAT_OF_DISPATCH = 30;

	switch (CKOP::STAT_OF_ANALYSE) {
	case 1:
		ret = CKOP_MENU   ::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
	case 2:
		ret = CKOP_SHADING::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
	case 3://円の重心と面積
		ret = CKOP_CIRCLE::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
	case 4:
		ret = CKOP_SCALE::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
	case 5:
		ret = CKOP_MTF::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
	case 6:
		ret = CKOP_TVD::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
#if 1//2017.04.01
	case 7://カメラ傾き
		ret = CKOP_ROLL::CMD_MSG(pWndForm, nID, nCode, pExtra, pHandlerInfo);
	break;
#endif
	}
	STAT_OF_DISPATCH = 0;
	return(ret);
}

BOOL CKOP::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
{
	BOOL	ret = FALSE;

	if (STAT_OF_DISPATCH) {
		TRACE("CKOP::MSG_PROC:%d\n", STAT_OF_DISPATCH);
		return(FALSE);
	}
//ZZ	STAT_OF_DISPATCH = 40;
	switch (CKOP::STAT_OF_ANALYSE) {
	case 1:
		ret = CKOP_MENU   ::MSG_PROC(pWndForm, pMsg);
	break;
	case 2:
		ret = CKOP_SHADING::MSG_PROC(pWndForm, pMsg);
	break;
	case 3://円の重心と面積
		ret = CKOP_CIRCLE::MSG_PROC(pWndForm, pMsg);
	break;
	case 4:
		ret = CKOP_SCALE::MSG_PROC(pWndForm, pMsg);
	break;
	case 5:
		ret = CKOP_MTF::MSG_PROC(pWndForm, pMsg);
	break;
	case 6:
		ret = CKOP_TVD::MSG_PROC(pWndForm, pMsg);
	break;
#if 1//2017.04.01
	case 7://カメラ傾き
		ret = CKOP_ROLL::MSG_PROC(pWndForm, pMsg);
	break;
#endif
	}
	STAT_OF_DISPATCH = 0;
	return(ret);
}
#if 1//2018.05.01
BOOL CKOP::NOTIFY_MSG(CWnd* pWndForm, UINT nID, LPNMHDR pNMHdr, LRESULT* pResult)
{
	BOOL	ret = FALSE;

	switch (CKOP::STAT_OF_ANALYSE) {
	case 10:
//		ret = CKOP_3DMAIN::NOTIFY_MSG(pWndForm, nID, pNMHdr, pResult);
	break;
	case 11:
//		ret = CKOP_3DSETT::NOTIFY_MSG(pWndForm, nID, pNMHdr, pResult);
	break;
	case 12:
//		ret = CKOP_3DDISP::NOTIFY_MSG(pWndForm, nID, pNMHdr, pResult);
	break;
	}
	return(ret);
}
HBRUSH CKOP::CTL_COLOR(CDC *pDC, CWnd* pWnd)
{
	HBRUSH	ret = NULL;

	switch (CKOP::STAT_OF_ANALYSE) {
	case 10:
//		ret = CKOP_3DMAIN::CTL_COLOR(pDC, pWnd);
	break;
	case 11:
//		ret = CKOP_3DSETT::CTL_COLOR(pDC, pWnd);
	break;
	case 12:
//		ret = CKOP_3DDISP::CTL_COLOR(pDC, pWnd);
	break;
	}
	return(ret);
}
void CKOP::TIMER_PROC(CWnd* pWndForm)
{
	//HBRUSH	ret = NULL;
	
	if (STAT_OF_DISPATCH) {
		TRACE("CKOP::TIMER_PROC:%d\n", STAT_OF_DISPATCH);
		return;
	}
	STAT_OF_DISPATCH = 50;
	switch (CKOP::STAT_OF_ANALYSE) {
	case 1:
		CKOP_MENU  ::TIMER_PROC(pWndForm);		
	break;
	case 3://円の重心と面積
		CKOP_CIRCLE::TIMER_PROC(pWndForm);
	break;
	case 5:
		CKOP_MTF   ::TIMER_PROC(pWndForm);
	break;
	case 10:
//		CKOP_3DMAIN::TIMER_PROC(pWndForm);
	break;
	case 11:
//		CKOP_3DSETT::TIMER_PROC(pWndForm);
	break;
	case 12:
//		CKOP_3DDISP::TIMER_PROC(pWndForm);
	break;
	}
	STAT_OF_DISPATCH = 0;
}
#endif

BOOL CKOP::MSG_PROC_MOUSE(CWnd* pWndForm, MSG* pMsg, int *pCaptured, LPPOINT pPnt)
{
#if 0
	return(TRUE);
#else
//	POINT	pnt;
	CRect	RT;

	::GetWindowRect(pMsg->hwnd, &RT);
	CKOP::ToClinet(HWND_DESKTOP, pWndForm->m_hWnd, &RT);

	switch (pMsg->message) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		pPnt->x = GET_X_LPARAM(pMsg->lParam);
		pPnt->y = GET_Y_LPARAM(pMsg->lParam);
		if (/*RT.PtInRect(pnt)*/TRUE) {
		//	CKOP::ToClinet(pWndForm->m_hWnd, pMsg->hwnd, &pnt);
			if (pMsg->message == WM_LBUTTONDOWN) {
				*pCaptured = 1;
			}
			else {
				*pCaptured = 2;
			}
pWndForm->Invalidate(FALSE);
			::SetCapture(pMsg->hwnd);
			return(TRUE);
		}
		else {
			::MessageBeep(MB_ICONHAND);
		}
	break;
	case WM_MOUSEMOVE:
		if (*pCaptured) {
			pPnt->x = GET_X_LPARAM(pMsg->lParam);
			pPnt->y = GET_Y_LPARAM(pMsg->lParam);
		//	CKOP::ToClinet(pWndForm->m_hWnd, pMsg->hwnd, &pnt);
			if (pPnt->x < 0) {
				pPnt->x = 0;
			}
			else if (pPnt->x >= RT.Width()) {
				pPnt->x = RT.Width()-1;
			}
			if (pPnt->y < 0) {
				pPnt->y = 0;
			}
			else if (pPnt->y >= RT.Height()) {
				pPnt->y = RT.Height()-1;
			}
pWndForm->Invalidate(FALSE);
			return(TRUE);
		}
	break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		*pCaptured = 0;
		return(TRUE);
	break;
	}
	return(FALSE);
#endif
}
#if 1//2015.07.24
void CKOP::STOP(void)
{
	AfxGetMainWnd()->SendMessageToDescendants(WM_COMMAND, ID_STOP_BY_CODE);
}
#endif

#if 1//2017.04.01
HANDLE	m_hDevMitsutoyo;
HWND	m_hWndRawInp;
WNDPROC	m_WndProcBak;
static
char	m_buff[256];
static
char	m_kcnt;
CStatic	m_staLbl;
CStatic	m_staVal;

LRESULT APIENTRY  WinProcRawInp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != WM_INPUT) {
		return(0);
	}

	UINT dwSize;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
				sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[dwSize];

    if (lpb == NULL) {
        goto skip;
    } 

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, 
         sizeof(RAWINPUTHEADER)) != dwSize ) {
         OutputDebugString (TEXT("GetRawInputData does not return correct size !\n")); 
	}

    RAWINPUT* raw = (RAWINPUT*)lpb;

    if (raw->header.dwType == RIM_TYPEKEYBOARD) 
    {
		CString	tmp;
		tmp.Format(TEXT(" Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n"), 
            raw->data.keyboard.MakeCode, 
            raw->data.keyboard.Flags, 
            raw->data.keyboard.Reserved, 
            raw->data.keyboard.ExtraInformation, 
            raw->data.keyboard.Message, 
            raw->data.keyboard.VKey);

		if (raw->header.hDevice == m_hDevMitsutoyo) {
			if (raw->data.keyboard.Message == WM_KEYDOWN) {
				WORD	w = MapVirtualKey(raw->data.keyboard.VKey, 2);
				m_buff[m_kcnt] = (char)w;

				if (m_buff[m_kcnt] == '\n' || m_buff[m_kcnt] == '\r') {
					m_buff[m_kcnt] = '\0';

					TRACE("%d::%s\n", GetTickCount(), m_buff);
//					lstrcat(m_buff, " ㎜");
					lstrcat(m_buff, " mm");
					m_staVal.SetWindowText(m_buff);
					m_kcnt = 0;
/*					::MessageBeep(0xFFFFFFFF);
					::MessageBeep(MB_ICONASTERISK);
					::MessageBeep(MB_ICONINFORMATION);
					::MessageBeep(MB_ICONEXCLAMATION);
					::MessageBeep(MB_ICONWARNING);
					::MessageBeep(MB_ICONERROR);
					::MessageBeep(MB_ICONHAND);
					::MessageBeep(MB_ICONERROR);
					::MessageBeep(MB_ICONHAND);
					::MessageBeep(MB_ICONINFORMATION);
					::MessageBeep(MB_ICONQUESTION);
					::MessageBeep(MB_ICONSTOP);
					::MessageBeep(MB_OK);*/
					::MessageBeep(MB_ICONWARNING);
				}
				else if (++m_kcnt >= (sizeof(m_buff)-1)) {
					m_kcnt = 0;
				}
			}
		}
		else {
			HWND h = GetFocus();
			if (h != NULL) {
				switch (raw->data.keyboard.Message) {
				case WM_KEYUP:
					h = h;
//					buf[1] = '\0';
//				::PostMessage(h, raw->data.keyboard.Message, raw->data.keyboard.VKey, 0);
//TRACE("%d:WM_KEYUP  :%X\n", GetTickCount(), raw->data.keyboard.VKey);
				break;
				case WM_KEYDOWN:
					::PostMessage(h, raw->data.keyboard.Message, raw->data.keyboard.VKey, 0);
//					::PostMessage(h, WM_KEYUP                  , raw->data.keyboard.VKey, 0);
TRACE("%d:WM_KEYDOWN:%X\n", GetTickCount(), raw->data.keyboard.VKey);
				break;
				default:
//					::PostMessage(h, raw->data.keyboard.Message, raw->data.keyboard.VKey, 0);
				break;
/*
::SendMessage(Memo1->Handle,WM_KEYDOWN,VK_DOWN,0);
::SendMessage(Memo1->Handle,WM_KEYUP  ,VK_DOWN,0);*/
				}
			}
		}
	}
	delete[] lpb; 
skip:
//	LRESULT	rst;
//	rst = DefRawInputProc((PRAWINPUT*)lParam, 1, sizeof(RAWINPUTHEADER ));
	return(1);
}
void CKOP::INIT_MITSUTOYO(CWnd* pWndForm)
{
	UINT	ndev;
	PRAWINPUTDEVICELIST
			pridl;
	CString	str;
	CString	tmp;
	if (m_hDevMitsutoyo != NULL) {
		mlog(_T("Internal error %s %d"), __FILE__, __LINE__);
		return;
	}
	if (!G_SS.USE_MITSUTOYO) {
		return;//測長器は使用しない
	}

	if (GetRawInputDeviceList(NULL, &ndev, sizeof(RAWINPUTDEVICELIST)) != 0) {
		goto skip;
	}
	if ((pridl = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * ndev)) == NULL) {
		goto skip;
	}
	if (GetRawInputDeviceList(pridl, &ndev, sizeof(RAWINPUTDEVICELIST)) == -1) {
		goto skip;
	}

	for (int i = 0; i < ndev; i++) {
		HANDLE	hDev = pridl[i].hDevice;
		TCHAR	buff[256];
		UINT	size = sizeof(buff);
		// デバイス名の長さを取得
		//if (GetRawInputDeviceInfo(hDev, RIDI_DEVICENAME, NULL, &size) != 0) {
		//	break;
		//}
		if (GetRawInputDeviceInfo(hDev, RIDI_DEVICENAME, buff, &size) == -1) {
			break;
		}
#if 0
		// デバイスの情報を表示
		static const TCHAR* deviceTypeNames[] = {
 			_T("MOU"),
 			_T("KBD"),
 			_T("HID"),
		};
		tmp.Format("[%02d]::%s::%s",
					i,
					deviceTypeNames[pridl[i].dwType],
					buff);
		str += tmp; 	 
#endif
		if (pridl[i].dwType == RIM_TYPEKEYBOARD) {
 			RID_DEVICE_INFO devinfo = { sizeof devinfo, };
 			UINT sz = sizeof devinfo;
 			if (GetRawInputDeviceInfo(hDev, RIDI_DEVICEINFO, &devinfo, &sz) == -1) {
 				break;
 			}
 			ASSERT(devinfo.dwType == RIM_TYPEKEYBOARD);
 			tmp.Format(" ==> [0x%02x, 0x%02x]:VID=%04X,PID=%04X", devinfo.keyboard.dwType, devinfo.keyboard.dwSubType,
				devinfo.hid.dwVendorId, devinfo.hid.dwProductId);
			str += tmp;
/*
VID_0FE7&PID_4001
idVendor:           0x0FE7
idProduct:          0x4001
*/
		}
#if 0
		str += "\r\n";
#endif
		strupr(buff);
		if (strstr(buff, "VID_0FE7") != NULL && strstr(buff, "PID_4001") != NULL) {
			m_hDevMitsutoyo = pridl[i].hDevice;
			break;
		}
 	}
	// after the job, free the RAWINPUTDEVICELIST
	free(pridl);
	if (m_hDevMitsutoyo == NULL) {
		mlog("ミツトヨ製測長器の接続を確認できませんでした.");
		return;
	}
	if (TRUE) {
		RAWINPUTDEVICE rid[1];
		//---  10,   3, 644, 22,           SS_CENTERIMAGE},
		m_hWndRawInp = CreateWindow("STATIC", "", WS_CHILD, 0,0,0,0, pWndForm->m_hWnd,NULL, NULL, NULL);
#if 1//2018.05.01
		m_WndProcBak = (WNDPROC)SetWindowLongPtr(m_hWndRawInp, GWLP_WNDPROC, (LONG_PTR)WinProcRawInp); 
#else
		m_WndProcBak = (WNDPROC)SetWindowLong(m_hWndRawInp, GWL_WNDPROC, (LONG)WinProcRawInp); 
#endif
		//---
		m_staLbl.Create("高さ情報:", WS_CHILD|WS_VISIBLE|0                                 , CRect(470,   3, 470+80, 3+22), pWndForm, IDC_STATIC);
		m_staVal.Create(""         , WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_CENTERIMAGE, CRect(555,   3, 555+95, 3+22), pWndForm, IDC_STATIC);
		m_staLbl.SetFont(GET_FONT(3));
		m_staVal.SetFont(GET_FONT(3));
		//---
		rid[0].usUsagePage = 0x01;	// keyboard/keypad
		rid[0].usUsage     = 0x06;	// keyboard
		rid[0].dwFlags     = RIDEV_INPUTSINK;   // adds HID keyboard and also ignores legacy keyboard messages
		rid[0].dwFlags    |= RIDEV_NOLEGACY;
		rid[0].hwndTarget  = m_hWndRawInp;
		if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE) {
			//registration failed. Call GetLastError for the cause of the error
		}
	}
skip:
	return;
}
void CKOP::TERM_MITSUTOYO(void)
{
	if (TRUE) {
		RAWINPUTDEVICE rid[1];
		//---
		rid[0].usUsagePage = 0x01;	// keyboard/keypad
		rid[0].usUsage     = 0x06;	// keyboard
		rid[0].dwFlags     = RIDEV_REMOVE;
		rid[0].hwndTarget  = NULL;//m_hWndRawInp;
		RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	}
	if (m_WndProcBak != NULL) {
#if 1//2018.05.01
		SetWindowLongPtr(m_hWndRawInp, GWLP_WNDPROC, (LONG_PTR)m_WndProcBak); 
#else
		SetWindowLong(m_hWndRawInp, GWL_WNDPROC, (LONG)m_WndProcBak);
#endif
		m_WndProcBak = NULL;
	}
	if (m_hWndRawInp != NULL) {
		::DestroyWindow(m_hWndRawInp);
		m_hWndRawInp = NULL;
	}
	if (m_hDevMitsutoyo != NULL) {
		m_hDevMitsutoyo = NULL;
	}
	if (m_staLbl.GetSafeHwnd() != NULL) {
		m_staLbl.DestroyWindow();
	}
	if (m_staVal.GetSafeHwnd() != NULL) {
		m_staVal.DestroyWindow();
	}
}
BOOL CKOP::GET_MITSUTOYO(char *p, int n)
{
	if (m_staVal.GetSafeHwnd() != NULL) {
		m_staVal.GetWindowText(p, n);
		return(TRUE);
	}
	return(FALSE);
}
#endif
#if 1//2017.04.01
void CKOP::GetProfileFixed(void)
{
	/***********************************/
	CKOP::FP.SHADING_FILTER    = GetProfileINT("FIXED", "SHADING_FILTER"   , -1);
	CKOP::FP.SHADING_PARAM     = GetProfileINT("FIXED", "SHADING_PARAM"    , -1);
	CKOP::FP.SHADING_COLMAP    = GetProfileINT("FIXED", "SHADING_COLMAP"   , -1);
	CKOP::FP.SHADING_DIAGO     = GetProfileINT("FIXED", "SHADING_DIAGO"    , -1);
	CKOP::FP.SHADING_SKIPCNT   = GetProfileINT("FIXED", "SHADING_SKIPCNT"  , -1);
	CKOP::FP.SHADING_GABVAL    = GetProfileINT("FIXED", "SHADING_GABVAL"   , -1);
	CKOP::FP.SHADING_SHAD_CORR = GetProfileINT("FIXED", "SHADING_SHAD_CORR", -1);
	/***********************************/
	CKOP::FP.SCALE_AVG_CNT     = GetProfileINT("FIXED", "SCALE_AVG_CNT"    , -1);
	CKOP::FP.SCALE_PITCH_SCALE = GetProfileINT("FIXED", "SCALE_PITCH_SCALE", -1);
	CKOP::FP.SCALE_PITCH_PIXEL = GetProfileINT("FIXED", "SCALE_PITCH_PIXEL", -1);
	CKOP::FP.SCALE_WEIGHT      = GetProfileINT("FIXED", "SCALE_WEIGHT"     , -1);
	CKOP::FP.SCALE_CHECK       = GetProfileINT("FIXED", "SCALE_CHECK"      , -1);
	CKOP::FP.SCALE_RECT        =*GetProfileRec("FIXED", "SCALE_RECT"       , -1);
	CKOP::FP.SCALE_FILTER      = GetProfileINT("FIXED", "SCALE_FILTER"     , -1);
	CKOP::FP.SCALE_PARAM       = GetProfileINT("FIXED", "SCALE_PARAM"      , -1);
	CKOP::FP.SCALE_SHAD_CORR   = GetProfileINT("FIXED", "SCALE_SHAD_CORR"  , -1);
	/***********************************/
	CKOP::FP.MTF_ZOOM          = GetProfileINT("FIXED", "MTF_ZOOM"         , -1);
	CKOP::FP.MTF_W             =*GetProfileRec("FIXED", "MTF_W"            , -1);
	CKOP::FP.MTF_B             =*GetProfileRec("FIXED", "MTF_B"            , -1);
	CKOP::FP.MTF_RECT_T1       =*GetProfileRec("FIXED", "MTF_RECT_T1"      , -1);
	CKOP::FP.MTF_RECT_Y1       =*GetProfileRec("FIXED", "MTF_RECT_Y1"      , -1);
	CKOP::FP.MTF_RECT_T2       =*GetProfileRec("FIXED", "MTF_RECT_T2"      , -1);
	CKOP::FP.MTF_RECT_Y2       =*GetProfileRec("FIXED", "MTF_RECT_Y2"      , -1);
	CKOP::FP.MTF_RECT_T3       =*GetProfileRec("FIXED", "MTF_RECT_T3"      , -1);
	CKOP::FP.MTF_RECT_Y3       =*GetProfileRec("FIXED", "MTF_RECT_Y3"      , -1);
	CKOP::FP.MTF_RECT_T4       =*GetProfileRec("FIXED", "MTF_RECT_T4"      , -1);
	CKOP::FP.MTF_RECT_Y4       =*GetProfileRec("FIXED", "MTF_RECT_Y4"      , -1);
	CKOP::FP.MTF_RECT_T5       =*GetProfileRec("FIXED", "MTF_RECT_T5"      , -1);
	CKOP::FP.MTF_RECT_Y5       =*GetProfileRec("FIXED", "MTF_RECT_Y5"      , -1);
	CKOP::FP.MTF_SHAD_CORR     = GetProfileINT("FIXED", "MTF_SHAD_CORR"    , -1);
	/***********************************/
	CKOP::FP.CIRCLE_SAVPOS     = GetProfileINT("FIXED", "CIRCLE_SAVPOS"    , -1);
	CKOP::FP.CIRCLE_BINVAL     = GetProfileINT("FIXED", "CIRCLE_BINVAL"    , -1);
	CKOP::FP.CIRCLE_BINIMG     = GetProfileINT("FIXED", "CIRCLE_BINIMG"    , -1);
	CKOP::FP.CIRCLE_BAKMOD     = GetProfileINT("FIXED", "CIRCLE_BAKMOD"    , -1);
#if 1//2018.05.01
	CKOP::FP.CIRCLE_AREA       =*GetProfileRec("FIXED", "CIRCLE_AREA"      , -1);
#endif
	CKOP::FP.CIRCLE_SHAD_CORR  = GetProfileINT("FIXED", "CIRCLE_SHAD_CORR" , -1);
	/***********************************/
	CKOP::FP.TVD_AVG_CNT       = GetProfileINT("FIXED", "TVD_AVG_CNT"      , -1);
	CKOP::FP.TVD_BIN_IMG       = GetProfileINT("FIXED", "TVD_BIN_IMG"      , -1);
	CKOP::FP.TVD_BIN_VAL       = GetProfileINT("FIXED", "TVD_BIN_VAL"      , -1);
	CKOP::FP.TVD_CHECK         = GetProfileINT("FIXED", "TVD_CHECK"        , -1); 
	CKOP::FP.TVD_SHAD_CORR     = GetProfileINT("FIXED", "TVD_SHAD_CORR"    , -1);
	/***********************************/
	CKOP::FP.ROLL_SAVPOS       = GetProfileINT("FIXED", "ROLL_SAVPOS"      , -1);
	CKOP::FP.ROLL_BINVAL       = GetProfileINT("FIXED", "ROLL_BINVAL"      , -1);
	CKOP::FP.ROLL_BAKMOD       = GetProfileINT("FIXED", "ROLL_BAKMOD"      , -1);
	CKOP::FP.ROLL_CSR_LT       = GetProfileINT("FIXED", "ROLL_CSR_LT"      , -1);
	CKOP::FP.ROLL_CSR_RT       = GetProfileINT("FIXED", "ROLL_CSR_RT"      , -1);
	CKOP::FP.ROLL_SHAD_CORR    = GetProfileINT("FIXED", "ROLL_SHAD_CORR"   , -1);
	/***********************************/
}
BOOL CKOP::IS_RECT_OK(LPRECT p)
{
	BOOL	ret;
	CRect	rt(p);
	if (rt.Width() > 0 && rt.Height() > 0) {
		ret = TRUE;
	}
	else {
		ret = FALSE;
	}
	return(ret);
}
#endif
#if 1//2018.05.01
void CKOP::UPD_STG_TEXT(CWnd *pWndForm, int nID, int ch)
{
	char	buf[32];

	STG::GET_POSITION(ch, buf, sizeof(buf));
	pWndForm->SetDlgItemText(nID, buf);
}
void CKOP::BEEP(int cnt)
{
	const
	int	FREQ = 1600,
		TIME = 250;

	Beep(FREQ, TIME);
	for (int i = 1; i < cnt; i++) {
		Sleep(TIME);
		Beep(FREQ, TIME);
	}
}
#endif