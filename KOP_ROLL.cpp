//**************************************************************************************
// カメラ傾き画面
//--------------------------------------------------------------------------------------
// 2017.04.01 新規追加
//**************************************************************************************

#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_ROLL.h"
#include "KOP_SHADING.h"
#include "KOP_SHADING_EX.h"
#include "KOP_CIRCLE.h"
#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "LSQ.h"
#include "EX.h"

#define SCALE_MAX_PIXEL 2000

static
struct {
	// PARAMTER
	int		P_SAVEPOS;
	int		P_BIN_VAL;
	int		P_BAK_MOD;
	int		P_CSR_LT;
	int		P_CSR_RT;
	int		P_SHAD_CORR;
	int		P_CSR_FIXED;
	// RESULT
	double	R_CENTER_L[3];
	double	R_CENTER_R[3];
	double	R_DIFF[3];
	double	R_LENGTH[3];
	double	R_ROLL[3];
	double	R_CROSS_X[3];
	double	R_CROSS_Y[3];
	//---
	HBITMAP	R_HBMP[2];
	//---
	double	R_COEF_L[3];
	double	R_COEF_R[3];
	//---
	TCHAR	B_SERIAL[64];
} m_d;
//------------------------------------
//------------------------------------
static
int		m_nCaptured;
//static
//LPBYTE	m_pLineBuf;
static
double	*m_pLineValL,
		*m_pLineValR;
//static
//int		m_nLineCnt;
//static
//CArray<double, double> m_af;
//static
//CArray<int, int> m_ai;
static
GDEF		m_gdef;
//------------------------------------

ITBL CKOP_ROLL::itbl[] = {
	{ 0, IDC_COMBO1, " 1"},
	{ 0, IDC_COMBO1, " 2"},
	{ 0,          0,   0}
};

//
// 画面定義
//

static
const
int	GG=35;
static
int	GC=20;
CTBL CKOP_ROLL::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 5, "LOGO"              ,1110,  30, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 0  , IDC_STATIC  , 5, "カメラ傾き"        ,  10,   3, 322, 22,           SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  , 780,   3, 350, 22, WS_BORDER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 5, "測定結果　"        ,  10, 518, 150, 22, 0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 5, "左中心　　"        ,  10, 545, 150, 22, 0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 5, "右中心　　"        ,  10, 570, 150, 22, 0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 5, "差　　　　"        ,  10, 595, 150, 22, 0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 5, "間隔　　　"        , 350, 545, 150, 22, 0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 5, "カメラ角度"        , 350, 570, 150, 22, 0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 5, "十字中心　"        , 350, 595, 150, 22, 0|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC20, 5, "\0XXX.X"             ,  10+150, 545, 150, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC21, 5, "\0XXX.X"             ,  10+150, 570, 150, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC22, 5, "\0 XX.X"             ,  10+150, 595, 150, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC23, 5, "\0XXXpix"            , 350+150, 545, 150, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC24, 5, "\0X.Xdeg"            , 350+150, 570, 150, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC25, 5, "\0(X,Y)"             , 350+150, 595, 150, 22, WS_BORDER|SS_CENTERIMAGE},
//-----
	{ 0, 0  , IDC_STATIC  , 1, "左中心"            ,1000,  30,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "右中心"            ,1000,  65,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "差"                ,1000, 100,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "間隔"              ,1000, 135,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "カメラ角度"        ,1000, 170,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "十字中心"          ,1000, 205,  80, 15, 0|SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC26, 1, "\0 XXX.X "         ,1005,  45,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC27, 1, "\0 XXX.X "         ,1005,  80,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC28, 1, "\0  XX.X "         ,1005, 115,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC29, 1, "\0XXX pix"         ,1005, 150,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC30, 1, "\0X.X deg"         ,1005, 185,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC31, 1, "\0(X,Y)  "         ,1005, 220,  75, 20, WS_BORDER|SS_CENTERIMAGE},
//-----
	{ 0, 0  , IDC_STATIC  , 1, "左中心"            ,1000,  30+215,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "右中心"            ,1000,  65+215,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "差"                ,1000, 100+215,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "間隔"              ,1000, 135+215,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "カメラ角度"        ,1000, 170+215,  80, 15, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "十字中心"          ,1000, 205+215,  80, 15, 0|SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC32, 1, "\0 XXX.X "         ,1005,  45+215,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC33, 1, "\0 XXX.X "         ,1005,  80+215,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC34, 1, "\0  XX.X "         ,1005, 115+215,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC35, 1, "\0XXX pix"         ,1005, 150+215,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC36, 1, "\0X.X deg"         ,1005, 185+215,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC37, 1, "\0(X,Y)  "         ,1005, 220+215,  75, 20, WS_BORDER|SS_CENTERIMAGE},
//-----
	{ 0, 0  , IDC_STATIC  , 1, "1回目"             , 757,  30,  40, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC38, 0, "IMAGE"             , 757,  50, 240,180, /*WS_BORDER|*/0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},

	{ 0, 0  , IDC_STATIC  , 1, "2回目"             , 757, 245,  40, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC39, 0, "IMAGE"             , 757, 265, 240,180, /*WS_BORDER|*/0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},

	{ 0, 0  , IDC_STATIC  , 5, "2値化閾値"         , 670, 545, 140, 35, 0|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_EDIT1   , 5, "120"               , 670, 580, 140, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_SPIN1   , 0, (LPCTSTR)(0|255<<16),   0,   0,   0,  0, UDS_SETBUDDYINT|UDS_ALIGNRIGHT},

	{ 0, 0  , IDC_CHECK2  , 7, "シェーディング補正", 830, 460, 140, 30, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_STATIC  , 5, "測定回数"          , 830, 495, 140, 30},
	{ 0, 0  , IDC_COMBO1  , 5, "1"                 , 830, 525, 140, 35},
	{ 0, 0  , IDC_STATIC12, 5, "黒地に白"          , 830, 560, 140, 50, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},

	{ 0, 0  , IDC_BUTTON1 , 0, "測定クリア"        , 990, 460, 140, 30},
	{ 0, 0  , IDC_BUTTON7 , 0, "保存"              , 990, 500, 140, 50},
	{ 0, 0  , IDC_BUTTON8 , 0, "戻る"              , 990, 560, 140, 50},
	{ 0, 2+4, IDC_STATIC10, 0, "\0GRAPH"             , 660,  30,  50,482, 0|SS_CENTER|SS_CENTERIMAGE|SS_USERITEM},//WS_BORDER},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC  , 1, "保存ファイル名"    , 685, 610, 140, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_EDIT19  , 0, ""                  , 685, 630, 250, 30, WS_BORDER},
	{ 0, 0  ,            0, 0, NULL}
};
#if 1//2017.04.01
static
void CHECK_FIXED(CWnd *pWndForm)
{
	CArray<UINT, UINT> ar;

	if (CKOP::FP.ROLL_SAVPOS >= 0) {
		m_d.P_SAVEPOS = CKOP::FP.ROLL_SAVPOS;
		ar.Add(IDC_COMBO1);
	}
	if (CKOP::FP.ROLL_BINVAL >= 0) {
		m_d.P_BIN_VAL = CKOP::FP.ROLL_BINVAL;
		ar.Add(IDC_EDIT1);
	}
	if (CKOP::FP.ROLL_BAKMOD >= 0) {
		m_d.P_BAK_MOD = CKOP::FP.ROLL_BAKMOD;
		ar.Add(IDC_STATIC12);
	}
	if (CKOP::FP.ROLL_CSR_LT >= 0) {
		m_d.P_CSR_LT = CKOP::FP.ROLL_CSR_LT;
		m_d.P_CSR_FIXED |= 1;
	}
	if (CKOP::FP.ROLL_CSR_RT >= 0) {
		m_d.P_CSR_RT = CKOP::FP.ROLL_CSR_RT;
//		ar.Add(IDC_CHECK1);
		m_d.P_CSR_FIXED |= 2;
	}
	if (CKOP::FP.ROLL_SHAD_CORR >= 0) {
		m_d.P_SHAD_CORR = CKOP::FP.ROLL_SHAD_CORR;
		ar.Add(IDC_CHECK2);
	}

	for (int i = 0; i < ar.GetCount(); i++) {
		pWndForm->GetDlgItem(ar[i])->EnableWindow(FALSE);
	}
}
#endif
void CKOP_ROLL::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	m_d.P_SAVEPOS   = GetProfileINT("ROLL", "SAVPOS"   ,   0);
	m_d.P_BIN_VAL   = GetProfileINT("ROLL", "BINVAL"   , 128);
	m_d.P_BAK_MOD   = GetProfileINT("ROLL", "BAKMOD"   ,   0);
	m_d.P_CSR_LT    = GetProfileINT("ROLL", "CSR_LT"   , 100);
	m_d.P_CSR_RT    = GetProfileINT("ROLL", "CSR_RT"   ,   0);
	m_d.P_SHAD_CORR = GetProfileINT("ROLL", "SHAD_CORR",   0);
	if (!CKOP_SHADING::IS_AVAILABLE_CORRECTION() || !CKOP_SHADING_EX::IS_AVAILABLE_CORRECTION()) {
		 m_d.P_SHAD_CORR = 0;
		 pWndForm->GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	}
	pWndForm->CheckDlgButton(IDC_CHECK2, m_d.P_SHAD_CORR!=0);
	m_d.P_CSR_FIXED = 0;
	if (G_SS.USE_FIXED) {
		CKOP::GetProfileFixed();
		CHECK_FIXED(pWndForm);
	}
	//-----
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->SetCurSel(m_d.P_SAVEPOS);
	//-----
	pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_BIN_VAL);
	//-----
	UPDATE_LABELS(pWndForm, -1);
	//-----
	if (lstrcmp(CKOP::SERIALNO, m_d.B_SERIAL)) {
		lstrcpyn(m_d.B_SERIAL, CKOP::SERIALNO, sizeof(m_d.B_SERIAL));
		for (int i = 0; i < ROWS(m_d.R_HBMP); i++) {
			if (m_d.R_HBMP[i] != NULL) {
				DeleteObject(m_d.R_HBMP[i]);
				m_d.R_HBMP[i] = NULL;
			}
		}
	}
	else {
		for (int i = 0; i < ROWS(m_d.R_HBMP); i++) {
			UPDATE_RESULT(pWndForm, i);
		}
	}
	//-----
#if 0
#endif
	//m_pLineBuf = (LPBYTE) malloc(CKOP::BMP_WID);
	m_pLineValL = (double*)malloc(CKOP::BMP_WID*sizeof(double));
	m_pLineValR = (double*)malloc(CKOP::BMP_WID*sizeof(double));

	CKOP::GDEF_INIT(&m_gdef, /*bitLBL=*/0, pWndForm->GetDlgItem(IDC_STATIC10));
	CKOP::GDEF_PSET(&m_gdef, 0, 255, 64, 0, CKOP::BMP_HEI-1, CKOP::BMP_HEI/8);
	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}
}

void CKOP_ROLL::TERM_FORM(void)
{
	//free(m_pLineBuf);
	//m_pLineBuf = NULL;
	free(m_pLineValL);
	m_pLineValL = NULL;
	free(m_pLineValR);
	m_pLineValR = NULL;

	WriteProfileINT("ROLL", "SAVPOS"   , m_d.P_SAVEPOS);
	WriteProfileINT("ROLL", "BINVAL"   , m_d.P_BIN_VAL);
	WriteProfileINT("ROLL", "BAKMOD"   , m_d.P_BAK_MOD);
	WriteProfileINT("ROLL", "CSR_LT"   , m_d.P_CSR_LT );
	WriteProfileINT("ROLL", "CSR_RT"   , m_d.P_CSR_RT );
	WriteProfileINT("ROLL", "SHAD_CORR", m_d.P_SHAD_CORR);
}
/********************************************************************/
void CKOP_ROLL::CALC_ROLL(LPBYTE pImgPxl)
{
	CRect	rt;
	int		x1, x2;
	int		n, q1 = 0, q2 = 0;

	if (m_d.P_CSR_LT < m_d.P_CSR_RT) {
		rt.left  = m_d.P_CSR_LT;
		rt.right = m_d.P_CSR_RT;
	}
	else {
		rt.left  = m_d.P_CSR_RT;
		rt.right = m_d.P_CSR_LT;
	}
	CArray<double,double>	xr, yr, xl, yl;
	if (TRUE) {
		x1 = rt.left;
		x2 = rt.right;
		x1 = (int)(x1 * CKOP::RX);
		x2 = (int)(x2 * CKOP::RX);

		for (int y = 0; y < CKOP::BMP_HEI; y++) {
			n = CKOP::GET_PIXEL(x1, y, pImgPxl);
			*(m_pLineValL+y) = n;
			if (IS_BIN_UPPER(n)) {
				xl.Add(y);
				yl.Add(n);
			}
			//---
			n = CKOP::GET_PIXEL(x2, y, pImgPxl);
			*(m_pLineValR+y) = n;
			if (IS_BIN_UPPER(n)) {
				xr.Add(y);
				yr.Add(n);
			}
		}
	}
	CLsq::Lsq(xl.GetData(), yl.GetData(), xl.GetCount()-1, 3-1, m_d.R_COEF_L);
	CLsq::Lsq(xr.GetData(), yr.GetData(), xr.GetCount()-1, 3-1, m_d.R_COEF_R);

	m_d.R_CENTER_L[0] =-m_d.R_COEF_L[1]/(2*m_d.R_COEF_L[2]);
	m_d.R_CENTER_R[0] =-m_d.R_COEF_R[1]/(2*m_d.R_COEF_R[2]);
	m_d.R_DIFF[0]     = fabs(m_d.R_CENTER_L[0]-m_d.R_CENTER_R[0]);
	m_d.R_LENGTH[0]   = fabs((double)x1-(double)x2);
	m_d.R_ROLL[0]     = atan(m_d.R_DIFF[0] / m_d.R_LENGTH[0]) * 180 / 3.141592;
}
BOOL CKOP_ROLL::IS_BIN_UPPER(int n)
{
	BOOL	ret;
	if (!m_d.P_BAK_MOD) {
		ret = (n > m_d.P_BIN_VAL);//黒地に白
	}
	else {
		ret = (n < m_d.P_BIN_VAL);//白地に黒
	}
	return(ret);
}
/********************************************************************/
void CKOP_ROLL::CALC_CENT(LPBYTE pImgPxl)
{
	int		y1, y2, x1, x2, n;

	CArray<double,double>
			xr, yr, xl, yl,
			xt, yt, xb, yb;
	double	COEF_T[3];
	double	COEF_B[3];
	double	COEF_L[3];
	double	COEF_R[3];

	//---
	x1 = CKOP::BMP_WID/4;
	x2 = CKOP::BMP_WID*3/4;
	y1 = CKOP::BMP_HEI/4;
	y2 = CKOP::BMP_HEI*3/4;
	//---
	for (int x = x1; x < x2; x++) {
		n = CKOP::GET_PIXEL(x, y1, pImgPxl);
		if (IS_BIN_UPPER(n)) {
			xt.Add(x);
			yt.Add(n);
		}
		n = CKOP::GET_PIXEL(x, y2, pImgPxl);
		if (IS_BIN_UPPER(n)) {
			xb.Add(x);
			yb.Add(n);
		}
	}
	//---
	for (int y = y1; y < y2; y++) {
		n = CKOP::GET_PIXEL(x1, y, pImgPxl);
		if (IS_BIN_UPPER(n)) {
			xl.Add(y);
			yl.Add(n);
		}
		n = CKOP::GET_PIXEL(x2, y, pImgPxl);
		if (IS_BIN_UPPER(n)) {
			xr.Add(y);
			yr.Add(n);
		}
	}
	if (xt.GetCount() < 3 || xb.GetCount() < 3 || xl.GetCount() < 3 || xr.GetCount() < 3) {
		m_d.R_CROSS_X[0] = C_NAN();
		m_d.R_CROSS_Y[0] = C_NAN();
	}
	else {
		double	XT, XB, YL, YR, S1, S2;
		CLsq::Lsq(xt.GetData(), yt.GetData(), xt.GetCount()-1, 3-1, COEF_T);
		CLsq::Lsq(xb.GetData(), yb.GetData(), xb.GetCount()-1, 3-1, COEF_B);
		CLsq::Lsq(xl.GetData(), yl.GetData(), xl.GetCount()-1, 3-1, COEF_L);
		CLsq::Lsq(xr.GetData(), yr.GetData(), xr.GetCount()-1, 3-1, COEF_R);

		//=-m_d.R_COEF_L[1]/(2*m_d.R_COEF_L[2]);
		XT = -COEF_T[1]/(2*COEF_T[2]);
		XB = -COEF_B[1]/(2*COEF_B[2]);
		YL = -COEF_L[1]/(2*COEF_L[2]);
		YR = -COEF_R[1]/(2*COEF_R[2]);
		/*
			REF. http://imagingsolution.blog107.fc2.com/blog-entry-137.html

                                   P4(xt, y1)

           P1(x1, yl)                                      P3(x2, yr)

                                   P2(xb, y2)
           面積S1　= {(P4.X - P2.X) * (P1.Y - P2.Y) - (P4.Y - P2.Y) * (P1.X - P2.X)} / 2
           面積S2　= {(P4.X - P2.X) * (P2.Y - P3.Y) - (P4.Y - P2.Y) * (P2.X - P3.X)} / 2

           となり、交点の座標は

           C1.X　= P1.X + (P3.X - P1.X) * S1 / (S1 + S2)
           C1.Y　= P1.Y + (P3.Y - P1.Y) * S1 / (S1 + S2)
		*/
		S1 = ((XT - XB) * (YL - y2) - (y1 - y2) * (x1 - XB)) / 2;
        S2 = ((XT - XB) * (y2 - YR) - (y1 - y2) * (XB - x2)) / 2;

		m_d.R_CROSS_X[0] = x1 + (x2 - x1) * S1 / (S1 + S2);
		m_d.R_CROSS_Y[0] = YL + (YR - YL) * S1 / (S1 + S2);
		//---
		//m_d.R_CROSS_X[0]-= CKOP::BMP_WID/2;
		//m_d.R_CROSS_Y[0]-= CKOP::BMP_HEI/2;
	}
}

void CKOP_ROLL::DRAW_GRAPH(CWnd *pWnd)
{
	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
	double*	p;

	CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);

	CKOP::GDEF_GRID(&m_gdef, pDC);

	p = m_pLineValL;
	pDC->SelectObject(&CKOP::m_penRed);			//20160427 Hirota add (グラフ上に画像の平均輝度値を赤色で描画)

	for (int i = 0; i < CKOP::BMP_HEI; i++, p++) {
		int	x, y;
		x = CKOP::GDEF_XPOS(&m_gdef, (int)*p);
		y = CKOP::GDEF_YPOS(&m_gdef, CKOP::BMP_HEI-1-i);
		if (!i) {
			pDC->MoveTo(x, y);
		}
		else {
			pDC->LineTo(x, y);
		}
	}

	p = m_pLineValR;
	pDC->SelectObject(&CKOP::m_penGreen);

	for (int i = 0; i < CKOP::BMP_HEI; i++, p++) {
		int	x, y;
		x = CKOP::GDEF_XPOS(&m_gdef, (int)*p);
		y = CKOP::GDEF_YPOS(&m_gdef, CKOP::BMP_HEI-1-i);
		if (!i) {
			pDC->MoveTo(x, y);
		}
		else {
			pDC->LineTo(x, y);
		}
		if ((i%100)==0) {
			i = i;
		}
	}

	//if (!m_d.P_CHECK) {
	//	goto skip;
	//}

//skip:
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
void CKOP_ROLL::DRAW_CURS(CDC *pDC, int x1, int x2, int y1, int y2)
{
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x1, y2);

	pDC->SelectObject(&CKOP::m_penGreen);

	pDC->MoveTo(x2, y1);
	pDC->LineTo(x2, y2);

	pDC->SelectObject(old_pen);
}
void CKOP_ROLL::DRAW_CURS(CWnd *pWnd)
{
	CDC	*pDC = pWnd->GetDC();
#if 1
	DRAW_CURS(pDC, m_d.P_CSR_LT,  m_d.P_CSR_RT, 0, CKOP::IMG_HEI);
#else
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pDC->MoveTo(m_d.P_CSR_LT    ,             0);
	pDC->LineTo(m_d.P_CSR_LT    , CKOP::IMG_HEI);

	pDC->SelectObject(&CKOP::m_penGreen);

	pDC->MoveTo(m_d.P_CSR_RT    ,             0);
	pDC->LineTo(m_d.P_CSR_RT    , CKOP::IMG_HEI);

	pDC->SelectObject(old_pen);
#endif
	pWnd->ReleaseDC(pDC);
}

void CKOP_ROLL::DRAW_MARKER(CWnd *pWnd)
{
	CDC	*pDC = pWnd->GetDC();
	const
	int		gap = 5;
	int		xx = (int)(m_d.R_CROSS_X[0]/CKOP::RX);
	int		yy = (int)(m_d.R_CROSS_Y[0]/CKOP::RY);
	CRgn	rgn;
	CRect	rc;
	pDC->GetClipBox(&rc);
	rgn.CreateRectRgnIndirect(&rc);
	pDC->SelectClipRgn(&rgn);

	CKOP_CIRCLE::DRAW_MARKER(pDC, xx, yy, gap);

	pDC->SelectClipRgn(NULL);
	pWnd->ReleaseDC(pDC);
}

void CKOP_ROLL::UPDATE_LABELS(CWnd *pWndForm, int mask)
{
/*	if (mask & 1) {
		if (m_d.P_BIN_IMG != 0) {
			pWndForm->SetDlgItemText(IDC_STATIC31, "2値化画像");
		}
		else {
			pWndForm->SetDlgItemText(IDC_STATIC31, "リアル画像");
		}
	}*/
	if (mask & 2) {
		switch (m_d.P_BAK_MOD) {
		case  1: pWndForm->SetDlgItemText(IDC_STATIC12, "白地に黒");break;
		case  0:
		default: pWndForm->SetDlgItemText(IDC_STATIC12, "黒地に白");break;
		}
	}
	if (mask & 4) {
	}
	if (mask & 8) {
	}
}
#if 0
void CKOP_ROLL::UPDATE_RESULT(CWnd *pWndForm)
{
/*	if (m_d.R_YOKO) {
		pWndForm->SetDlgItemText(IDC_STATIC14,  "横");
	}
	else {
		pWndForm->SetDlgItemText(IDC_STATIC14,  "縦");
	}
	pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(F2S(m_d.R_ZOOM_RATE));
	pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText(F2S(m_d.R_PERIOD_VAL));
*/
/*	CString	s1, s2, s3, s4, s5, s6;
	pWndForm->GetDlgItem(IDC_STATIC20)->SetWindowText(s1);
	pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(s2);
	pWndForm->GetDlgItem(IDC_STATIC22)->SetWindowText(s3);
	pWndForm->GetDlgItem(IDC_STATIC23)->SetWindowText(s4);
	pWndForm->GetDlgItem(IDC_STATIC24)->SetWindowText(s5);
	pWndForm->GetDlgItem(IDC_STATIC25)->SetWindowText(s6);*/
	UPDATE_RESULT(pWndForm, 0);
}
#endif
void CKOP_ROLL::UPDATE_RESULT(CWnd *pWndForm, int idx)
{
	//----------
	CString	buf1, buf2, buf3, buf4, buf5, buf6;
	int		i=idx;
	if (i > 0 && m_d.R_HBMP[i] == NULL) {
	}
	/*else if (i > 0) {
	buf1.Format("%.1lf,%.1lf", m_d.R_COG_X[i], m_d.R_COG_Y[i]);
	buf2.Format("%.0lf pix", m_d.R_SQAR[i]);
	buf3.Format("%4.1lf,%4.1lf", m_d.R_DIF_X[i], m_d.R_DIF_Y[i]);
	buf4.Format("%.2lf pix", m_d.R_PHI[i]);
	}*/
	else {
	buf1.Format("%5.1lf", m_d.R_CENTER_L[i]);
	buf2.Format("%5.1lf", m_d.R_CENTER_R[i]);
	buf3.Format("%5.1lf", m_d.R_DIFF[i]    );
	buf4.Format("%5.0lf", m_d.R_LENGTH[i]  );
	buf5.Format("%.1lf" , m_d.R_ROLL[i]    );
	buf6.Format("%.0lf,%.0lf" , m_d.R_CROSS_X[i]-CKOP::BMP_WID/2, m_d.R_CROSS_Y[i]-CKOP::BMP_HEI/2);
	}
	switch (i) {
	case 0:
		pWndForm->GetDlgItem(IDC_STATIC20)->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC22)->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC23)->SetWindowText(buf4);
		pWndForm->GetDlgItem(IDC_STATIC24)->SetWindowText(buf5);
		pWndForm->GetDlgItem(IDC_STATIC25)->SetWindowText(buf6);
	break;
	case 1:
		pWndForm->GetDlgItem(IDC_STATIC26)->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC27)->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC29)->SetWindowText(buf4);
		pWndForm->GetDlgItem(IDC_STATIC30)->SetWindowText(buf5);
		pWndForm->GetDlgItem(IDC_STATIC31)->SetWindowText(buf6);
		((CStatic*)pWndForm->GetDlgItem(IDC_STATIC38))->SetBitmap(m_d.R_HBMP[i]);
	break;
	case 2:
		pWndForm->GetDlgItem(IDC_STATIC32)->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC33)->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC34)->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC35)->SetWindowText(buf4);
		pWndForm->GetDlgItem(IDC_STATIC36)->SetWindowText(buf5);
		pWndForm->GetDlgItem(IDC_STATIC37)->SetWindowText(buf6);
		((CStatic*)pWndForm->GetDlgItem(IDC_STATIC39))->SetBitmap(m_d.R_HBMP[i]);
	break;
	}
}
//
// ri矩形(0,0,w,h)からru矩形(ox,oy,ww,hh)へアスペクト比を維持してマッピング
//
void MAP_RECT(LPRECT ri, LPRECT ru, int width, int height)
{
	int	iw = ri->right  - ri->left;
	int	ih = ri->bottom - ri->top ;
	int	uw = width ;
	int	uh = height;

	int	NH = (ih * uw / iw), NW;

	//アスペクト比維持のためオフセット分の計算
	if (NH < uh) {
		ru->top    = (uh-NH)/2;
		ru->bottom = ru->top + NH;
		ru->left   = 0;
		ru->right  = uw;
	}
	else {
		NW = (iw * uh / ih);
		ru->top    = 0;
		ru->bottom = uh;
		ru->left   = (uw-NW)/2;
		ru->right  = ru->left + NW;
	}
}

//
// ri座標系からru座標系に変換
//
void MAP_CD_I2U(LPRECT ri, LPRECT ru, LPPOINT pi, LPPOINT pu)
{
	double	iw = ri->left   - ri->right;
	double	ih = ri->bottom - ri->top  ;
	double	uw = ru->left   - ru->right;
	double	uh = ru->bottom - ru->top  ;

	pu->x = (int)(ru->left + (pi->x - ri->left) * uw / iw);
	pu->y = (int)(ru->top  + (pi->y - ri->top ) * uw / iw);
}

void CKOP_ROLL::SAVE_RESULT(CWnd *pWndForm)
{
	m_d.P_SAVEPOS = ((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->GetCurSel();
	
	int		i = m_d.P_SAVEPOS+1;
	CWnd*	pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);
	CDC		*pDC = pWnd->GetWindowDC();
	CRect	rt;
	CDC		mdc;
	int		w, h;
	HBITMAP	bmp, old_bmp;
	int		WW = 240;//BITMAP.SIZE
	int		HH = 180;
	int		XX = 0, YY = 0;
	//-----
	m_d.R_CENTER_L[i] = m_d.R_CENTER_L[0];
	m_d.R_CENTER_R[i] = m_d.R_CENTER_R[0];
	m_d.R_DIFF    [i] = m_d.R_DIFF    [0];
	m_d.R_LENGTH  [i] = m_d.R_LENGTH  [0];
	m_d.R_ROLL    [i] = m_d.R_ROLL    [0];
	m_d.R_CROSS_X [i] = m_d.R_CROSS_X [0];
	m_d.R_CROSS_Y [i] = m_d.R_CROSS_Y [0];
	//-----
	pWnd->GetWindowRect(&rt);
	w = rt.Width();
	h = rt.Height();
	// メモリDCにpWndをコピー
	bmp = ::CreateCompatibleBitmap(pDC->m_hDC, WW, HH);
	mdc.CreateCompatibleDC(pDC);
	old_bmp = (HBITMAP)mdc.SelectObject(bmp);
	//int	NH = (h * WW / w),
	//	NW;
	//アスペクト比維持のためオフセット分の計算
	//if (NH < HH) {
	//	YY = (HH-NH)/2;
	//	HH = NH;
	//}
	//else {
	//	NW = (w * HH / h),
	//	XX = (WW-NW)/2;
	//	WW = NW;
	//}
//	mdc.StretchBlt(XX, YY, WW, HH, pDC, 0, 0, w, h, SRCCOPY);

	CRect	ru;
	rt.OffsetRect(-rt.left, -rt.top);
	MAP_RECT(&rt, &ru, WW, HH);
	SetStretchBltMode(mdc.m_hDC, HALFTONE);
	mdc.StretchBlt(ru.left, ru.top, ru.Width(), ru.Height(), pDC, 0, 0, w, h, SRCCOPY);

	if (TRUE) {
		CPoint	p1, p2, p3, p4, p5, p6;
		p1.SetPoint(m_d.P_CSR_LT, 0);
		p3.SetPoint(m_d.P_CSR_LT, CKOP::IMG_HEI);
		p5.SetPoint(m_d.P_CSR_RT, 0);
		MAP_CD_I2U(&rt, &ru, &p1, &p2);
		MAP_CD_I2U(&rt, &ru, &p3, &p4);
		MAP_CD_I2U(&rt, &ru, &p5, &p6);
		DRAW_CURS(&mdc, p2.x, p6.x, p2.y, p4.y);
	}
	if (TRUE) {
		int		xx = (int)(m_d.R_CROSS_X[0]/CKOP::RX);
		int		yy = (int)(m_d.R_CROSS_Y[0]/CKOP::RY);
		CPoint	p1(xx, yy), p2;
		MAP_CD_I2U(&rt, &ru, &p1, &p2);

		CKOP_CIRCLE::DRAW_MARKER(&mdc, p2.x, p2.y, 3);
	}

	mdc.SelectObject(old_bmp);
	pWnd->ReleaseDC(pDC);

	if (m_d.R_HBMP[i] != NULL) {
		UINT	nIDS[] = {
			0, IDC_STATIC38, IDC_STATIC39
		};
		CStatic* pSta = (CStatic*)pWndForm->GetDlgItem(nIDS[i]);
		
		if (pSta != NULL) {
			pSta->SetBitmap(NULL);
		}
		DeleteObject(m_d.R_HBMP[i]);
		m_d.R_HBMP[i] = NULL;
	}
	m_d.R_HBMP[i] = bmp;
	UPDATE_RESULT(pWndForm, i);
	::MessageBeep(MB_OK);
}

void CKOP_ROLL::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	m_d.P_SHAD_CORR = pWndForm->IsDlgButtonChecked(IDC_CHECK2);
	if (m_d.P_SHAD_CORR) {
		CKOP_SHADING::DO_CORRECTION(pImgPxl);
		CKOP_SHADING_EX::DO_CORRECTION(pImgPxl);
	}
	//-----
	m_d.P_BIN_VAL = pWndForm->GetDlgItemInt(IDC_EDIT1);
	CALC_ROLL(pImgPxl);
	CALC_CENT(pImgPxl);
}

void CKOP_ROLL::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);
	DRAW_CURS(pWnd);
//	CKOP::GDEF_GRID(&m_gdef, pWndForm->GetDlgItem(IDC_STATIC10)->GetDC());
	DRAW_MARKER(pWnd);
	DRAW_GRAPH(pWndForm->GetDlgItem(IDC_STATIC10));
	UPDATE_RESULT(pWndForm);
}

BOOL CKOP_ROLL::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON7://保存
#if 0
			CKOP::SAVE_WINDOW(AfxGetMainWnd(), "roll.png");
#else
			CKOP::SAVE_WINDOW(pWndForm, "roll.png");
#endif
#if 1//2017.07.18
			{
				CCSV	csv;
				SET_CSV(&csv);
				CKOP::SAVE_CSV(pWndForm, &csv, "roll.csv");
			}
#endif
			return(TRUE);
		break;
		case IDC_BUTTON8://戻る
			CKOP::MOVE_FORM(pWndForm, 1);
			return(TRUE);
		break;
		case IDC_BUTTON1://クリア
			if (mlog("#q測定結果をクリアします。よろしいですか？") == IDYES) {
				UINT	nIDS[] = {
					0, IDC_STATIC38, IDC_STATIC39
				};
				for (int i = 0; i < ROWS(nIDS); i++) {
					CStatic* pSta = (CStatic*)pWndForm->GetDlgItem(nIDS[i]);
					if (pSta != NULL) {
						pSta->SetBitmap(NULL);
						CDC*	pDC = pSta->GetWindowDC();
						CRect	rt;
						pDC->GetClipBox(&rt);
						//rt.DeflateRect(1,1);
						pDC->FillSolidRect(&rt, GetSysColor(COLOR_3DFACE));
						pSta->ReleaseDC(pDC);
					}
				}

				for (int i = 1; i < ROWS(m_d.R_CENTER_L); i++) {
					m_d.R_CENTER_L[i] = C_NAN();
					m_d.R_CENTER_R[i] = C_NAN();
					m_d.R_DIFF[i]     = C_NAN();
					m_d.R_LENGTH[i]   = C_NAN();
					m_d.R_ROLL[i]     = C_NAN();
					m_d.R_CROSS_X[i]    = C_NAN();
					m_d.R_CROSS_Y[i]    = C_NAN();
					//---
					if (m_d.R_HBMP[i] != NULL) {
						DeleteObject(m_d.R_HBMP[i]);
					}
					m_d.R_HBMP[i] = NULL;
					UPDATE_RESULT(pWndForm, i);
				}
			}
		break;

#if 0
		case IDC_BUTTON10://データ保存
		{
			CString	path;
			CTime	tim = CTime::GetCurrentTime();
			path.Format("%s\\SCALE_VAL-%s.TXT",
				GetDirDoc(),
				tim.Format("%Y%m%d-%Hh%Mm%Ss"));

			FILE *fp = fopen(path, "wt");
			fprintf(fp, "WEIGHT	%d\n", m_d.P_WEIGHT);
			fprintf(fp, "COUNT	%d\n", m_nLineCnt);
			fprintf(fp, "IDX	VAL	VAL''\n");
			for (int i = 0; i < m_nLineCnt; i++) {
			fprintf(fp, "%d	%.2lf	%.2lf\n", i, *(m_pLineValL+i), *(m_pLineValR+i));
			}
			fclose(fp);
			::MessageBeep(MB_OK);
			return(TRUE);
		}
		break;
#endif
		}
	break;
	case CBN_SELCHANGE:
		if (nID == IDC_COMBO3) {
		}
		if (nID == IDC_COMBO1 || nID == IDC_COMBO2) {
pWndForm->Invalidate(FALSE);
		}
	break;
	}
	return(FALSE);
}

BOOL CKOP_ROLL::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
{
	UINT	nID;
	POINT	pnt;

	if (pWndForm->m_hWnd == pMsg->hwnd) {
		nID = IDC_IMAGEAREA;
	}
	else {
		nID = ::GetDlgCtrlID(pMsg->hwnd);
	}
	switch (pMsg->message) {
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN) {
			SAVE_RESULT(pWndForm);
			::MessageBeep(MB_ICONASTERISK);
			return(TRUE);
		}
	break;
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
					if (!(m_d.P_CSR_FIXED & 1)) {
						m_d.P_CSR_LT = pnt.x;
						m_nCaptured = 1;
					}
				}
				else {
					if (!(m_d.P_CSR_FIXED & 2)) {
						m_d.P_CSR_RT = pnt.x;
						m_nCaptured = 2;
					}
				}
pWndForm->Invalidate(FALSE);
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
					m_d.P_CSR_LT = pnt.x;
				}
				else {
					m_d.P_CSR_RT = pnt.x;
				}
pWndForm->Invalidate(FALSE);
			}
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			ReleaseCapture();
			m_nCaptured = 0;
		break;
		}
	break;
	case IDC_STATIC12:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			if (++m_d.P_BAK_MOD >= 2) {
				m_d.P_BAK_MOD = 0;
			}
			UPDATE_LABELS(pWndForm, 2);
pWndForm->Invalidate();
		break;
		}
	break;
/*	case IDC_STATIC21:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			if ((m_d.P_WEIGHT-=2) < 0) {
				m_d.P_WEIGHT = 25;
			}
			pWndForm->SetDlgItemInt(IDC_STATIC21, m_d.P_WEIGHT);
			pWndForm->Invalidate(FALSE);
			//UPDATE_TEXT(pWndForm, 8);
		break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			if ((m_d.P_WEIGHT+=2) > 25) {
				m_d.P_WEIGHT = 5;
			}
			pWndForm->SetDlgItemInt(IDC_STATIC21, m_d.P_WEIGHT);
			pWndForm->Invalidate(FALSE);
			//UPDATE_TEXT(pWndForm, 8);
		break;
		}
	break;*/
	}
	return(FALSE);
}
#if 1//2017.07.18
void CKOP_ROLL::SET_CSV(CCSV *pc)
{
	CString	buf;
	LPCTSTR	pHead0[] = {
		"", "左中心", "右中心", "差", "間隔(pix)", "カメラ角度(deg)", "十字中心(x)", "十字中心(y)"
	};
	LPCTSTR	pHead1[] = {
		"測定結果", "1回目", "2回目", ""
	};
	int		r = 0;
	pc->set(0, r, "カメラ傾き");
	r++;
	//---
	if (CKOP::GET_MITSUTOYO(buf.GetBuffer(256), 256)) {
		buf.ReleaseBuffer();
		pc->set( 0, r, "高さ情報");
		pc->set( 1, r, buf);
		r++;
	}
	else {
		buf.ReleaseBuffer();
	}
	//---
	for (int c = 0; c < ROWS(pHead0); c++) {
		pc->set(c, r, pHead0[c]);
	}
	r++;
	//---
	for (int i = 0; i < 3; i++) {
		for (int c = 0; c < ROWS(pHead0); c++) {
			switch (c) {
			case 0: buf = pHead1[i]; break;
			case 1: buf = F1S(m_d.R_CENTER_L[i]); break;
			case 2: buf = F1S(m_d.R_CENTER_R[i]); break;
			case 3: buf = F1S(m_d.R_DIFF    [i]); break;
			case 4: buf = F0S(m_d.R_LENGTH  [i]); break;
			case 5: buf = F1S(m_d.R_ROLL    [i]); break;
			case 6: buf = F0S(m_d.R_CROSS_X [i]); break;
			case 7: buf = F0S(m_d.R_CROSS_Y [i]); break;
			}
			pc->set(c, r+i, buf);
		}
	}
	return;
}
#endif