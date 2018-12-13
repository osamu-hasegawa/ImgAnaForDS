/**************************************************************************
20170703 Uchida change to Calcurate method.
***************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_SCALE.h"
#include "KOP_SHADING.h"
#include "KOP_SHADING_EX.h"
#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "EX.h"

#define SCALE_MAX_PIXEL 2000

static
struct {
	// PARAMTER
	int		P_AVG_CNT;
	POINT	P_PNT_LT;
	POINT	P_PNT_RB;
	int		P_WEIGHT;
	int		P_CHECK;
//	int		P_DIAGO;
	int		P_FILTER;
	int		P_PARAM;
	int		P_SHAD_CORR;
	// RESULT
	int		R_YOKO;		//1:YOKO, 0:TATE
	double	R_PITCH_SCALE;
	double	R_PITCH_PIXEL;
	double	R_PERIOD_VAL;
	double	R_ZOOM_RATE;
	double  R_AVERAGE;
} m_d;
//------------------------------------
//------------------------------------
static
int		m_nCaptured;
static
LPBYTE	m_pLineBuf;
static
double	*m_pLineVal,
		*m_pLineTmp;
static
int		m_nLineCnt;
static
CArray<double, double> m_af;
static
CArray<int, int> m_ai;
static
GDEF		m_gdef;
//------------------------------------

ITBL CKOP_SCALE::itbl[] = {
	{ 2, IDC_COMBO1, "なし"    },
	{ 2, IDC_COMBO1, "BLUR"    },
	{ 2, IDC_COMBO1, "GAUSSIAN"},
	{ 2, IDC_COMBO1, "MEDIAN"  },
	{ 2, IDC_COMBO2, "なし"    },
	{ 2, IDC_COMBO2, "3x3"     },
	{ 2, IDC_COMBO2, "5x5"     },
	{ 2, IDC_COMBO2, "9x9"     },
	{ 2, IDC_COMBO2, "11x11"   },
	{ 2, IDC_COMBO3, "5"       },
	{ 2, IDC_COMBO3, "7"       },
	{ 2, IDC_COMBO3, "9"       },
	{ 2, IDC_COMBO3, "11"      },
	{ 2, IDC_COMBO3, "13"      },
	{ 2, IDC_COMBO3, "15"      },
	{ 2, IDC_COMBO3, "17"      },
	{ 2, IDC_COMBO3, "19"      },
	{ 2, IDC_COMBO3, "21"      },
	{ 2, IDC_COMBO3, "23"      },
	{ 2, IDC_COMBO3, "25"      },
	{ 0,          0,          0}
};

//
// 倍率解析画面定義
//
const
int	GG=35;
int	GC=20;
CTBL CKOP_SCALE::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 5, "LOGO"              ,1110,  30, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 0  , IDC_STATIC10, 5, "倍率解析"          ,  10,   3, 322, 22,           SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  , 780,   3, 350, 22, WS_BORDER|SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC  , 5, "フィルター処理"    , 670,10+30, 200, 22, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 5, "ウェイト"          , 900,10+30, 200, 22, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_COMBO1  , 5, "Gauss"             , 670,10+60, 200, 30},
	{ 0, 0  , IDC_COMBO2  , 5, "3x3"               , 900,10+60, 200, 30},

	{ 0, 4  , IDC_STATIC14, 5, "横"                , 655,   8,  30, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC12, 5, "定規ピッチ"        , 670,70+  60, 200, 25, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC13, 5, "画素ピッチ"        , 670,70+  90, 200, 25, 0|SS_CENTERIMAGE},

	{ 0, 0  , IDC_EDIT1   , 5, "500"               , 900,70+  60, 100, 25, WS_BORDER|0|ES_CENTER},/*20160507 Hirota Comment 定規ピッチ*/
	{ 0, 0  , IDC_EDIT2   , 5, "5.3"               , 900,70+  90, 100, 25, WS_BORDER|0|ES_CENTER|0/*ES_READONLY*/},/*20160507 Hirota Comment 画素ピッチ*/

	{ 0, 0  , IDC_STATIC15, 5, "μm"               ,1030,70+  60,  60, 25, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC15, 5, "μm"               ,1030,70+  90,  60, 25, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC17, 5, "光学倍率"          , 670,30+ 200-GC, 200, 30, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC21, 5, "NN.NNN"               , 900,30+ 200-GC, 100, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 0  , IDC_STATIC15, 5, "倍"                ,1030,30+ 200-GC,  60, 30, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC23, 3, "重心方式に変更"
												   , 670, 300-GG, 200, 30, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC24, 3, "平均"	           , 700, 330-GG, 150, 30, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC25, 3, "周期"			   , 700, 360-GG, 150, 30, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC27, 3, "NNN"               , 920, 330-GG,  80, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC28, 3, "NNN.NN"            , 920, 360-GG,  80, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC30, 1, "最小カット幅"      , 670, 400-GG,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC31, 1, "ウェイト"          , 700, 420-GG,  80, 25, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_COMBO3  , 1, ""                  , 780, 420-GG, 100, 25, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_CHECK1  , 1, "グラフ表示(緑線)"  , 700, 445-GG, 180, 25, 0|0|0},
	{ 0, 0  , IDC_BUTTON10, 1, "データの保存"      , 700, 470-GG, 180, 25, WS_BORDER|0|0},
	{ 0, 0  , IDC_CHECK2  , 7, "シェーディング補正", 990, 440, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_BUTTON7 , 0, "保存"              , 990, 500, 140, 50},
	{ 0, 0  , IDC_BUTTON8 , 0, "戻る"              , 990, 560, 140, 50},
	{ 0, 8  , IDC_STATIC37, 0, "GRAPH"             ,  10, 518, 810, 90, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC  , 1, "保存ファイル名"    , 685, 610, 140, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_EDIT19  , 0, ""                  , 685, 630, 250, 30, WS_BORDER},
	{ 0, 0  ,            0, 0, NULL}
};


static
void CHECK_FIXED(CWnd *pWndForm)
{
	CArray<UINT, UINT> ar;

	if (CKOP::FP.SCALE_AVG_CNT >= 0) {
		m_d.P_AVG_CNT = CKOP::FP.SCALE_AVG_CNT;
	}
	if (CKOP::FP.SCALE_PITCH_SCALE >= 0) {
		m_d.R_PITCH_SCALE = CKOP::FP.SCALE_PITCH_SCALE;
		ar.Add(IDC_EDIT1);
	}
	if (CKOP::FP.SCALE_PITCH_PIXEL >= 0) {
		m_d.R_PITCH_PIXEL = CKOP::FP.SCALE_PITCH_PIXEL;
		ar.Add(IDC_EDIT2);
	}
	if (CKOP::FP.SCALE_WEIGHT >= 0) {
		m_d.P_WEIGHT = CKOP::FP.SCALE_WEIGHT;
		ar.Add(IDC_COMBO3);
	}
	if (CKOP::FP.SCALE_CHECK >= 0) {
		m_d.P_CHECK = CKOP::FP.SCALE_CHECK;
		ar.Add(IDC_CHECK1);
	}
	if (CKOP::IS_RECT_OK(&CKOP::FP.SCALE_RECT)) {
		m_d.P_PNT_LT.x = CKOP::FP.SCALE_RECT.left;
		m_d.P_PNT_LT.y = CKOP::FP.SCALE_RECT.top;
		m_d.P_PNT_RB.x = CKOP::FP.SCALE_RECT.right;
		m_d.P_PNT_RB.y = CKOP::FP.SCALE_RECT.bottom;
		//ar.Add(IDC_EDIT2);
	}
	if (CKOP::FP.SCALE_FILTER >= 0) {
		m_d.P_FILTER = CKOP::FP.SCALE_FILTER;
		ar.Add(IDC_COMBO1);
	}
	if (CKOP::FP.SCALE_PARAM >= 0) {
		m_d.P_PARAM = CKOP::FP.SCALE_PARAM;
		ar.Add(IDC_COMBO2);
	}
	if (CKOP::FP.SCALE_SHAD_CORR >= 0) {
		m_d.P_SHAD_CORR = CKOP::FP.SCALE_SHAD_CORR;
		ar.Add(IDC_CHECK2);
	}

	for (int i = 0; i < ar.GetCount(); i++) {
		pWndForm->GetDlgItem(ar[i])->EnableWindow(FALSE);
	}
}

void CKOP_SCALE::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	m_d.P_AVG_CNT   = GetProfileINT("SCALE", "AVG_CNT"    ,   3);
	m_d.R_PITCH_SCALE = GetProfileDbl("SCALE", "PITCH_SCALE", 500);
	m_d.R_PITCH_PIXEL = GetProfileDbl("SCALE", "PITCH_PIXEL", 5.3);
	m_d.P_WEIGHT    = GetProfileINT("SCALE", "WEIGHT"     ,  19);
	m_d.P_CHECK     = GetProfileINT("SCALE", "CHECK"      ,   1);
	m_d.P_PNT_LT.x  = GetProfileINT("SCALE", "LT.X"       ,   0);
	m_d.P_PNT_LT.y  = GetProfileINT("SCALE", "LT.Y"       ,   0);
	m_d.P_PNT_RB.x  = GetProfileINT("SCALE", "RB.X"       ,   0);
	m_d.P_PNT_RB.y  = GetProfileINT("SCALE", "RB.Y"       ,   0);

	m_d.P_FILTER    = GetProfileINT("SCALE", "FILTER", 1);
	m_d.P_PARAM     = GetProfileINT("SCALE", "PARAM" , 0);
	m_d.P_SHAD_CORR = GetProfileINT("SCALE", "SHAD_CORR" , 0);
	if (!CKOP_SHADING::IS_AVAILABLE_CORRECTION() || !CKOP_SHADING_EX::IS_AVAILABLE_CORRECTION()) {
		 m_d.P_SHAD_CORR = 0;
		 pWndForm->GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	}
	pWndForm->CheckDlgButton(IDC_CHECK2, m_d.P_SHAD_CORR!=0);


	if (G_SS.USE_FIXED) {
		CKOP::GetProfileFixed();
		CHECK_FIXED(pWndForm);
	}

	//-----
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->SetCurSel(m_d.P_FILTER);
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO2))->SetCurSel(m_d.P_PARAM);
	//-----
	pWndForm->SetDlgItemTextA(IDC_EDIT1, CEX::F2A(m_d.R_PITCH_SCALE));		/* 定規ピッチセット*/
	pWndForm->SetDlgItemTextA(IDC_EDIT2, CEX::F2A(m_d.R_PITCH_PIXEL));
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO3))->SelectString(0, I2S(m_d.P_WEIGHT));
	((CButton*  )pWndForm->GetDlgItem(IDC_CHECK1))->SetCheck(m_d.P_CHECK);

	m_pLineBuf = (LPBYTE) malloc(CKOP::BMP_WID);
	m_pLineVal = (double*)malloc(CKOP::BMP_WID*sizeof(double));
//	m_pLineTmp = (double*)malloc(CKOP::BMP_WID*sizeof(double));

	CKOP::GDEF_INIT(&m_gdef, /*bitLBL=*/1+2, pWndForm->GetDlgItem(IDC_STATIC37));
	CKOP::GDEF_PSET(&m_gdef, 0, CKOP::BMP_WID-1, CKOP::BMP_WID/8, 0, 255, 64);
	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}
}

void CKOP_SCALE::TERM_FORM(void)
{
	free(m_pLineBuf);
	m_pLineBuf = NULL;
	free(m_pLineVal);
	m_pLineVal = NULL;
	free(m_pLineTmp);
	m_pLineTmp = NULL;
	
	WriteProfileINT("SCALE", "AVG_CNT"    , m_d.P_AVG_CNT  );
	WriteProfileDbl("SCALE", "PITCH_SCALE", m_d.R_PITCH_SCALE);
	WriteProfileDbl("SCALE", "PITCH_PIXEL", m_d.R_PITCH_PIXEL);
	WriteProfileINT("SCALE", "WEIGHT"     , m_d.P_WEIGHT   );
	WriteProfileINT("SCALE", "CHECK"      , m_d.P_CHECK    );
	WriteProfileINT("SCALE", "LT.X"       , m_d.P_PNT_LT.x );
	WriteProfileINT("SCALE", "LT.Y"       , m_d.P_PNT_LT.y );
	WriteProfileINT("SCALE", "RB.X"       , m_d.P_PNT_RB.x );
	WriteProfileINT("SCALE", "RB.Y"       , m_d.P_PNT_RB.y );
	WriteProfileINT("SCALE", "FILTER", m_d.P_FILTER);
	WriteProfileINT("SCALE", "PARAM" , m_d.P_PARAM);
#if 1//2015.09.10
	WriteProfileINT("SCALE", "SHAD_CORR"  , m_d.P_SHAD_CORR);
#endif
}
static	double GetAve(BYTE buf[], int cnt)
{
	double	average = 0;
	for (int i = 0; i < cnt; i++) {
		average += (double)buf[i];
	}
	average =average/double(cnt+1);
	return(average);
}

void CKOP_SCALE::CALC_SCALE(LPBYTE pImgPxl)
//　スケールを計算する　アルゴリズム
{
	CRect	rt;
	int		y1, y2, x1, x2;
	int		n, q = 0;

	if (m_d.P_PNT_LT.x < m_d.P_PNT_RB.x) {
		rt.left  = m_d.P_PNT_LT.x;
		rt.right = m_d.P_PNT_RB.x;
	}
	else {
		rt.left  = m_d.P_PNT_RB.x;
		rt.right = m_d.P_PNT_LT.x;
	}
	if (m_d.P_PNT_LT.y < m_d.P_PNT_RB.y) {
		rt.top    = m_d.P_PNT_LT.y;
		rt.bottom = m_d.P_PNT_RB.y;
	}
	else {
		rt.top    = m_d.P_PNT_RB.y;
		rt.bottom = m_d.P_PNT_LT.y;
	}
	if (rt.Width() >= rt.Height()) {
		//横方向と判断した場合
		m_d.R_YOKO = TRUE;
		y1 = (rt.top + rt.bottom)/2;
		y2 = y1;
		x1 = rt.left;
		x2 = rt.right;
		y1 = (int)(y1 * CKOP::RY);
		y2 = (int)(y2 * CKOP::RY);
		x1 = (int)(x1 * CKOP::RX);
		x2 = (int)(x2 * CKOP::RX);

		y1 = y1 - m_d.P_AVG_CNT/2;
		y2 = y1 + m_d.P_AVG_CNT;

		for (int x = x1; x < x2; x++, q++) {
			n = 0;
			for (int y = y1; y < y2; y++) {
				n += CKOP::GET_PIXEL(x, y, pImgPxl);		//20160427 Hirota add(横方向にROIを設定した場合縦方向のピクセル値を加算して平均を出している。)
			}

			*(m_pLineBuf+q) = n/m_d.P_AVG_CNT;				//@@Hirota LPBYTEの使用方法
			*(m_pLineVal+q) = (double)n/(double)m_d.P_AVG_CNT;
		}
	}
	else {
		//縦方向と判断した場合
		m_d.R_YOKO = FALSE;
		x1 = (rt.left + rt.right)/2;
		x2 = x1;
		y1 = rt.top;
		y2 = rt.bottom;

		y1 = (int)(y1 * CKOP::RY);
		y2 = (int)(y2 * CKOP::RY);
		x1 = (int)(x1 * CKOP::RX);
		x2 = (int)(x2 * CKOP::RX);

		x1 = x1 - m_d.P_AVG_CNT/2;
		x2 = x1 + m_d.P_AVG_CNT;

		for (int y = y1; y < y2; y++, q++) {
			n = 0;
			for (int x = x1; x < x2; x++) {
				n += CKOP::GET_PIXEL(x, y, pImgPxl);	//20160427 Hirota add(縦方向にROIを設定した場合横方向のピクセル値を加算して平均を出している。)
			}

			*(m_pLineBuf+q) = n/m_d.P_AVG_CNT;
			*(m_pLineVal+q) = (double)n/(double)m_d.P_AVG_CNT;
		}
	}
	m_nLineCnt = q;

//　ここまでで、m_pLineVal に　マウスで囲んだ範囲のラインデータを入力

//	SmoothSG2(m_pLineVal, m_pLineTmp, m_nLineCnt, m_d.P_WEIGHT);	//　2次微分処理実施

	m_af.RemoveAll();
	m_ai.RemoveAll();

	double	yave = GetAve(m_pLineBuf, m_nLineCnt);		//	グラフの縦軸Yの平均値を算出　

	double	max = 0 , jcount = 0;
	int jst = 1;

	for (int i = 0; i < m_nLineCnt; i++) {
		double	val = *(m_pLineVal+i);
		if(val < yave) {
			if(jst != 1){
				max += (yave-val)*(double)i;
				jcount += (yave-val);
			}
		}else{
			jst=0;
			if(jcount != 0){
				m_af.Add(max);
				m_ai.Add(max/jcount);
			}
			max = 0;
			jcount = 0;
		}
	}

/*

	double	max = -1e99;
	double	bak = *m_pLineTmp;

	for (int i = 1; i < m_nLineCnt-1; i++) {
		double	val = *(m_pLineTmp+i);
		double	nxt = *(m_pLineTmp+i+1);
		
		if (val > bak && val > nxt) {
			m_af.Add(val);
			m_ai.Add(i);
			if (max < val) {
				max = val;
			}
		}
		bak = val;
	}
	for (int i = m_af.GetCount()-1; i >= 0 ; i--) {
		if (m_af.GetAt(i) < (max/3)) {
			m_af.RemoveAt(i);
			m_ai.RemoveAt(i);
		}
	}
*/

// ピーク間隔の平均を出す。

	double		xttl = 0;
	for (int i = 1; i < m_af.GetCount(); i++) {
		double	x1 = m_ai.GetAt(i-1);
		double	x2 = m_ai.GetAt(i);

		xttl += (x2-x1);
	}
	m_d.R_PERIOD_VAL = xttl / (double)(m_af.GetCount()-1);

// 倍率を計算　倍率　=　ピッチ計算値　*　1ピクセル辺りの寸法値　/　定規のスケール

	m_d.R_ZOOM_RATE  = (m_d.R_PERIOD_VAL * m_d.R_PITCH_PIXEL) / m_d.R_PITCH_SCALE;
	m_d.R_AVERAGE = (double)yave;

}
void CKOP_SCALE::DRAW_MARQUEE(CWnd *pWnd)
{
	CDC	*pDC = pWnd->GetDC();
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pDC->MoveTo(m_d.P_PNT_LT.x, m_d.P_PNT_LT.y);
	pDC->LineTo(m_d.P_PNT_RB.x, m_d.P_PNT_LT.y);
	pDC->LineTo(m_d.P_PNT_RB.x, m_d.P_PNT_RB.y);
	pDC->LineTo(m_d.P_PNT_LT.x, m_d.P_PNT_RB.y);
	pDC->LineTo(m_d.P_PNT_LT.x, m_d.P_PNT_LT.y);

	pDC->SelectObject(&CKOP::m_penGreen);

	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
static	int GetMin(BYTE buf[], int cnt)
{
	int	min = 999;
	for (int i = 0; i < cnt; i++) {
		if (min > buf[i]) {
			min = buf[i];
		}
	}
	return(min);
}
static	int GetMax(BYTE buf[], int cnt)
{
	int	max = -999;
	for (int i = 0; i < cnt; i++) {
		if (max < buf[i]) {
			max = buf[i];
		}
	}
	return(max);
}

void CKOP_SCALE::DRAW_GRAPH(CWnd *pWndForm,CWnd *pWnd)
//　グラフ描画　:　201707.03　大幅変更　BY　UCHIDA
{
	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
	CRect	rt;
	int	xmin, xmax, ymin, ymax;
	int		h = 0;
	LPBYTE	p;

	LPBYTE	p_ave;

	CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);

	int	x, y;
	xmin = 0;
	xmax = m_nLineCnt;							//　グラフの横軸Xの最大値
	ymin = GetMin(m_pLineBuf, m_nLineCnt);		//　グラフの縦軸Yの最小値を算出
	ymax = GetMax(m_pLineBuf, m_nLineCnt);		//	グラフの縦軸Yの最大値を算出
	double yave = GetAve(m_pLineBuf, m_nLineCnt);		//	グラフの縦軸Yの平均値を算出　

	CKOP::GDEF_PSET(&m_gdef, xmin, xmax, (xmax-xmin)/8, ymin, ymax, (ymax-ymin)/4);
	CKOP::GDEF_GRID(&m_gdef, pDC);

	p = m_pLineBuf;

//　グラフ上に画像の輝度値を黒色で描画
//	pDC->SelectObject(&CKOP::m_penBlack);

	for (int i = 0; i < m_nLineCnt; i++, p++) {

		x = CKOP::GDEF_XPOS(&m_gdef, i);
		y = CKOP::GDEF_YPOS(&m_gdef, (int)*p);

		if (!i) {
			pDC->MoveTo(x, y);
		}
		else {
			pDC->LineTo(x, y);
		}
	}

//　グラフ上に画像の輝度平均値を赤色で描画

	pDC->SelectObject(&CKOP::m_penRed);

	x = CKOP::GDEF_XPOS(&m_gdef, 0);
	y = CKOP::GDEF_YPOS(&m_gdef,(int)yave);
	pDC->MoveTo(x,y);
	x = CKOP::GDEF_XPOS(&m_gdef, m_nLineCnt);
	pDC->LineTo(x,y);

//　チャックがないと他のグラフ描画は飛ばす

	if (!m_d.P_CHECK) {
		goto skip;
	}

// グラフ上に算出したピーク値を出す

	pDC->SelectObject(&CKOP::m_penGreen);		


	for (int q = 0; q < m_af.GetCount(); q++) {
		int	x, y, i;
		i = m_ai[q];
		x = CKOP::GDEF_XPOS(&m_gdef, i);
		y = CKOP::GDEF_YPOS(&m_gdef, (int)yave);

		pDC->MoveTo(x, y-10);
		pDC->LineTo(x, y+10);
		pDC->MoveTo(x-3, y);
		pDC->LineTo(x+3, y);

	}
skip:
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);

}

void CKOP_SCALE::UPDATE_RESULT(CWnd *pWndForm)
{
	if (m_d.R_YOKO) {
		pWndForm->SetDlgItemText(IDC_STATIC14,  "横");
	}
	else {
		pWndForm->SetDlgItemText(IDC_STATIC14,  "縦");
	}
	pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(F4S(m_d.R_ZOOM_RATE));		/* 倍率を画面上に表示 */
	pWndForm->GetDlgItem(IDC_STATIC27)->SetWindowText(F2S(m_d.R_AVERAGE));			/* 平均値を画面上に表示 */
	pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText(F2S(m_d.R_PERIOD_VAL));		/* 周波数を画面上に表示 */
}

void CKOP_SCALE::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CString	buf;

	pWndForm->GetDlgItemText(IDC_EDIT1, buf);		/* 定規ピッチゲット*/
	m_d.R_PITCH_SCALE = atof(buf);
	//-----
	pWndForm->GetDlgItemText(IDC_EDIT2, buf);		/* カメラピッチゲット*/
	m_d.R_PITCH_PIXEL = atof(buf);
	//-----
	m_d.P_CHECK = pWndForm->IsDlgButtonChecked(IDC_CHECK1);

	//-----
	if (TRUE) {
		CComboBox	*p1 = (CComboBox*)(pWndForm->GetDlgItem(IDC_COMBO1));
		CComboBox	*p2 = (CComboBox*)(pWndForm->GetDlgItem(IDC_COMBO2));
		if (p1 != NULL) {
			m_d.P_FILTER = p1->GetCurSel();
		}
		if (p2 != NULL) {
			m_d.P_PARAM = p2->GetCurSel();
		}
	}

	m_d.P_SHAD_CORR = pWndForm->IsDlgButtonChecked(IDC_CHECK2);
	if (m_d.P_SHAD_CORR) {
		CKOP_SHADING::DO_CORRECTION(pImgPxl);
		CKOP_SHADING_EX::DO_CORRECTION(pImgPxl);
	}

	//-----
	CKOP_SHADING::DO_FILTER(pWndForm, pImgPxl, m_d.P_FILTER, m_d.P_PARAM);
	CKOP_SHADING_EX::DO_FILTER(pWndForm, pImgPxl, m_d.P_FILTER, m_d.P_PARAM);
	CALC_SCALE(pImgPxl);
}

void CKOP_SCALE::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);

	DRAW_MARQUEE(pWnd);

	DRAW_GRAPH(pWndForm,pWndForm->GetDlgItem(IDC_STATIC37));

	UPDATE_RESULT(pWndForm);
}

BOOL CKOP_SCALE::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON7://保存
#if 0
			CKOP::SAVE_WINDOW(AfxGetMainWnd(), "mag.png");
#else
			CKOP::SAVE_WINDOW(pWndForm, "mag.png");
#endif
#if 1//2017.07.18
			{
				CCSV	csv;
				SET_CSV(&csv);
				CKOP::SAVE_CSV(pWndForm, &csv, "mag.csv");
			}
#endif
			return(TRUE);
		break;
		case IDC_BUTTON8://戻る
			CKOP::MOVE_FORM(pWndForm, 1);
			return(TRUE);
		break;
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
			fprintf(fp, "%d	%.2lf	%.2lf\n", i, *(m_pLineVal+i), *(m_pLineTmp+i));
			}
			fclose(fp);
			::MessageBeep(MB_OK);
			return(TRUE);
		}
		break;
		}
	break;
	case CBN_SELCHANGE:
		if (nID == IDC_COMBO3) {
			TCHAR	buf[16];
			pWndForm->SendDlgItemMessageA(nID,  WM_GETTEXT, sizeof(buf)-1, (LPARAM)buf);
			m_d.P_WEIGHT = atoi(buf);
pWndForm->Invalidate(FALSE);
			return(TRUE);
		}
		if (nID == IDC_COMBO1 || nID == IDC_COMBO2) {
pWndForm->Invalidate(FALSE);
		}
	break;
	}
	return(FALSE);
}

BOOL CKOP_SCALE::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
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
		case WM_RBUTTONDOWN:
#if 1//2017.04.01
			if (G_SS.USE_FIXED && CKOP::IS_RECT_OK(&CKOP::FP.SCALE_RECT)) {
				break;
			}
#endif
			pnt.x = GET_X_LPARAM(pMsg->lParam);
			pnt.y = GET_Y_LPARAM(pMsg->lParam);
			if (CKOP::IMG_RECT.PtInRect(pnt)) {
				CKOP::ToClinet(pWndForm->m_hWnd, ::GetDlgItem(pWndForm->m_hWnd, IDC_IMAGEAREA), &pnt);
				if (TRUE) {
					m_d.P_PNT_LT = pnt;
					m_d.P_PNT_RB = pnt;
					m_nCaptured = 1;
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
				if (TRUE) {
					m_d.P_PNT_RB = pnt;
				}
pWndForm->Invalidate(FALSE);
			}
		break;
		case WM_RBUTTONUP:
			ReleaseCapture();
			m_nCaptured = 0;
		break;
		}
	break;

	}
	return(FALSE);
}
#if 1//2017.07.18
void CKOP_SCALE::SET_CSV(CCSV *pc)
{
	CString	buf;
	int		r = 0;
	pc->set( 0, r, "倍率解析");
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
	pc->set( 0, r, "方向");
	pc->set( 1, r, m_d.R_YOKO==0 ? "縦":"横");
	r++;
	//---
	pc->set( 0, r, "フィルター処理");
	switch (m_d.P_FILTER) {
	case 1: buf = "BLUR"; break;
	case 2: buf = "GAUSSIAN"; break;
	case 3: buf = "MEDIAN"; break;
	default:buf = "なし"; break;
	}
	switch (m_d.P_PARAM) {
	case 1: buf += ":3x3"; break;
	case 2: buf += ":5x5"; break;
	case 3: buf += ":9x9"; break;
	case 4: buf += ":11x11"; break;
	default:buf += ":なし"; break;
	}
	pc->set( 1, r, buf);
	r++;
	//---
	pc->set( 0, r, "定規ピッチ(um)");
	pc->set( 1, r, F2S(m_d.R_PITCH_SCALE));
	r++;
	//---
	pc->set( 0, r, "画素ピッチ(um)");
	pc->set( 1, r, F2S(m_d.R_PITCH_PIXEL));
	r++;
	//---
	pc->set( 0, r, "光学倍率(倍)");
	pc->set( 1, r, F4S(m_d.R_ZOOM_RATE));
	r++;
	//---
	pc->set( 0, r, "重心:平均");
	pc->set( 1, r, F2S(m_d.R_AVERAGE));
	r++;
	//---
	pc->set( 0, r, "重心:周期");
	pc->set( 1, r, F2S(m_d.R_PERIOD_VAL));
	r++;
	//---
	pc->set( 0, r, "最小カット幅(ウェイト)");
	pc->set( 1, r, I2S(m_d.P_WEIGHT));
	r++;
	//---
	return;
}
#endif