/*********************************************************************************************
  MTF 計算処理　ルーチン　　　　　
20160704　方式3(田村さん方式）を追加。　by　uchida
*********************************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_MTF.h"
#if 1//2015.09.10
#include "KOP_SHADING.h"
#include "KOP_SHADING_EX.h"
#endif
#include "math.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"
#if 1//2018.05.01
#include "STG.h"
#endif
static
struct {
	// PARAMTER
	RECT	P_RT_L;
	RECT	P_RT_R;

	RECT	P_RT_LU;//IMAGE座標系⇔ZOOM座標系変換用の単位矩形
	RECT	P_RT_RU;

	RECT	P_RT_W;//方式2の基準値用矩形(白)
	RECT	P_RT_B;//方式2の基準値用矩形(黒)
	int		P_ZOOM;
	int		P_MODE2;
	int		P_SAVEPOS;
#if 1//2015.09.10
	int		P_SHAD_CORR;
#endif	//
#if 1//2017.04.01
	int		P_FIXED;
	RECT	P_FIXED_T[5];	//縦
	RECT	P_FIXED_Y[5];	//横
	double	B_MTF_T[5];		//測定測値:MTF(縦)
	double	B_MTF_Y[5];		//測定測値:MTF(横)
	RECT	R_RECT_T[5];	//エリア縦
	RECT	R_RECT_Y[5];	//エリア横
#endif
	//
	// RESULT
	double	R_MTF[2];//0:左(縦), 1:右(横)
	double	R_AVG_W[2];
	double	R_AVG_B[2];
	double	R_MTF_T[5];//測定結果:MTF(縦)
	double	R_MTF_Y[5];//測定結果:MTF(横)
	double	R_BAS_W;
	double	R_BAS_B;
	//---
	//---
	int		B_BASE;//基準矩形の指定待ち
	int		B_ZM_ID;
	int		B_ZM_CAP;
	RECT	B_RT_L;
	RECT	B_RT_R;
	TCHAR	B_SERIAL[64];
} m_d;
#if 1//2018.05.01
static
struct {
	int		STG_STAT;
	int		CUR_TIC;
	BOOL	GET_TRIG;
	double	MES_STEP;
	//---
	SYSTEMTIME
			R_TIME;
	int		R_IDX;
	int		R_MAX_IDX;
	double	R_MAX;
	double	R_POS  [256];
	double	R_MTF_T[5][256];//測定結果:MTF(縦)
	double	R_MTF_Y[5][256];//測定結果:MTF(横)
} m_s;
#endif
//------------------------------------
static
LPBYTE		m_pLineBuf[2];
static
GDEF		m_gdef[2];
//------------------------------------
static
int		m_nCaptured;
//------------------------------------

ITBL CKOP_MTF::itbl[] = {
	{ 0, IDC_COMBO1, " 1"},
	{ 0, IDC_COMBO1, " 2"},
	{ 0, IDC_COMBO1, " 3"},
	{ 0, IDC_COMBO1, " 4"},
	{ 0, IDC_COMBO1, " 5"},
	{ 0,          0,    0}
};

//
// MTF解析画面定義
//

CTBL CKOP_MTF::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 0, "LOGO"              ,1100,  30, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},

	{ 0, 0  , IDC_STATIC10, 5, "MTF解析(方式1)  (MAX-MIN)/(MAX+MIN)"    ,  10,   3, 400, 22,           SS_CENTERIMAGE},

	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  , 780,   3, 350, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC12, 5, "倍率 x1"           , 670,   3, 100, 22, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 4  , IDC_STATIC13, 0, "ZOOM IMAGE1"       , 670,  30, 200,200, WS_BORDER|SS_WHITEFRAME|SS_CENTERIMAGE|SS_NOTIFY},	//SS_WHITEFRAME},
	{ 0, 4  , IDC_STATIC14, 0, "ZOOM IMAGE2"       , 880,  30, 200,200, WS_BORDER|SS_WHITEFRAME|SS_CENTERIMAGE|SS_NOTIFY},	//SS_WHITEFRAME},
	{ 0, 4  , IDC_STATIC15, 0, "GRAPH1"            , 670, 240, 200, 70, WS_BORDER|SS_WHITEFRAME|SS_CENTERIMAGE|0},
	{ 0, 4  , IDC_STATIC16, 0, "GRAPH2"            , 880, 240, 200, 70, WS_BORDER|SS_WHITEFRAME|SS_CENTERIMAGE|0},

	{ 0, 4  , IDC_STATIC55  , 5, "輝度(白):"         , 680, 330,  90, 30, 0|SS_CENTERIMAGE},								//	方式2の時に平均値に変更
	{ 0, 4  , IDC_STATIC56  , 5, "輝度(黒):"         , 680, 360,  90, 30, 0|SS_CENTERIMAGE},								//	方式2の時に標準偏差に変更
	{ 0, 4  , IDC_STATIC  , 5, "MTF(縦):"          , 680, 390,  90, 30, 0|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC20, 5, "XXX.X"             , 780, 330,  90, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC21, 5, "XX.X"              , 780, 360,  90, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC22, 5, "XX.XX%"            , 780, 390,  90, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 4  , IDC_STATIC57  , 5, "輝度(白):"         , 890, 330,  90, 30, 0|SS_CENTERIMAGE},								//	方式2の時に平均値に変更
	{ 0, 4  , IDC_STATIC58  , 5, "輝度(黒):"         , 890, 360,  90, 30, 0|SS_CENTERIMAGE},								//	方式2の時に標準偏差に変更
	{ 0, 4  , IDC_STATIC  , 5, "MTF(横):"          , 890, 390,  90, 30, 0|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC26, 5, "XXX.X"             , 990, 330,  90, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC27, 5, "XX.X"              , 990, 360,  90, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC28, 5, "XX.XX%"            , 990, 390,  90, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC43, 6,"(xxx,xxx)-(xxx-xxx)", 765, 605, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC44, 6,"(xxx,xxx)-(xxx-xxx)", 875, 605, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC17, 0, "凡例(白)"          , 770, 618+0,  48, 25, 0|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 0  , IDC_STATIC18, 0, "凡例(黒)"          , 880, 618+0,  48, 25, 0|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 0  , IDC_CHECK1  , 0, "基準(白)"          , 770, 643+0, 100, 30, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_CHECK2  , 0, "基準(黒)"          , 880, 643+0, 100, 30, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_STATIC23, 1, ""                  , 820, 620+0,  50, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC24, 1, ""                  , 930, 620+0,  50, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 4  , IDC_STATIC  , 5, "縦エリア"          , 680, 435,  90, 30, 0|0|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC  , 5, "横エリア"          , 680, 465,  90, 30, 0|0|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC41, 5,"(xxx,xxx)-(xxx-xxx)", 775, 435, 210, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 4  , IDC_STATIC42, 5,"(xxx,xxx)-(xxx-xxx)", 775, 465, 210, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 0  , IDC_CHECK3  , 7, "シェーディング補正", 990, 430, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_BUTTON1 , 0, "測定クリア"        , 990, 480+5, 140, 30},
	{ 0, 0  , IDC_STATIC  , 3, "測定箇所"          , 990, 515+5,  80, 30, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_COMBO1  , 5, "3"                 ,1070, 515+5,  60, 35},

	{ 0, 0  , IDC_BUTTON7 , 0, "保存"              , 990, 500+60, 140, 50},
	{ 0, 0  , IDC_BUTTON8 , 0, "戻る"              , 990, 560+60, 140, 50},


	{ 0, 8  , IDC_STATIC  , 9, "測定1"             ,  10, 520,  40, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "縦エリア"          ,  50, 518,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC45, 6, "\0(xx,xx)-(xx-xxx)",  90, 518, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "横エリア"          ,  50, 532,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC46, 6, "\0(xx,xx)-(xx-xxx)",  90, 532, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 3, "MTF(縦):"          ,  20, 545,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 3, "MTF(横):"          ,  20, 570,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC30, 3, ""                  , 100, 545,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC31, 3, ""                  , 100, 570,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 9, "測定2"             , 200, 520,  40, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "縦エリア"          , 240, 518,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC47, 6, "\0(xx,xx)-(xx-xxx)", 280, 518, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "横エリア"          , 240, 532,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC48, 6, "\0(xx,xx)-(xx-xxx)", 280, 532, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 3, "MTF(縦):"          , 210, 545,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 3, "MTF(横):"          , 210, 570,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC32, 3, ""                  , 290, 545,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC33, 3, ""                  , 290, 570,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},	//20160509 Hirota comment(上の不具合修正)

	{ 0, 8  , IDC_STATIC  , 9, "測定3"             , 390, 520,  40, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "縦エリア"          , 430, 518,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC49, 6, "\0(xx,xx)-(xx-xxx)", 470, 518, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "横エリア"          , 430, 532,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC50, 6, "\0(xx,xx)-(xx-xxx)", 470, 532, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 3, "MTF(縦):"          , 400, 545,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 3, "MTF(横):"          , 400, 570,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC34, 3, ""                  , 480, 545,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC35, 3, ""                  , 480, 570,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 9, "測定4"             , 580, 520,  40, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "縦エリア"          , 620, 518,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC51, 6, "\0(xx,xx)-(xx-xxx)", 660, 518, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "横エリア"          , 620, 532,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC52, 6, "\0(xx,xx)-(xx-xxx)", 660, 532, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 3, "MTF(縦):"          , 590, 545,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 3, "MTF(横):"          , 590, 570,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC36, 3, ""                  , 670, 545,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC37, 3, ""                  , 670, 570,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 9, "測定5"             , 770, 520,  40, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "縦エリア"          , 810, 518,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC53, 6, "\0(xx,xx)-(xx-xxx)", 850, 518, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 6, "横エリア"          , 810, 532,  40, 11, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC54, 6, "\0(xx,xx)-(xx-xxx)", 850, 532, 110, 11, 0|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 8  , IDC_STATIC  , 3, "MTF(縦):"          , 780, 545,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC  , 3, "MTF(横):"          , 780, 570,  80, 20, 0|0|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC38, 3, ""                  , 860, 545,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC39, 3, ""                  , 860, 570,  70, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
#if 1//2018.05.01
	{ 0, 0  , IDC_STATIC60, 9, "ステージ移動"      ,1100, 170, 140, 20},
	{ 0, 0  , IDC_EDIT2   , 5, "9999"              ,1100, 190, 140, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER|ES_READONLY},
	{ 0, 0  , IDC_STATIC59, 9, "mm"                ,1240, 195,  30, 20, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_BUTTON9 , 0, "原点移動"          ,1280, 185, 100, 40},
	{ 0, 0  , IDC_BUTTON10, 0, "CLEAR"             ,1300, 230,  70, 25},
	{ 0, 0  , IDC_BUTTON11, 0, "<JOG+"             ,1100, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON12, 0, "<STEP+"            ,1170, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON13, 0, "STEP->"            ,1240, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON14, 0, "JOG->"             ,1310, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON15, 0, "オート検出"        ,1120, 305, 240, 50},
	{ 0, 0  , IDC_BUTTON16, 0, "連続測定"          ,1120, 360, 240, 50},
#endif
	{ 0, 0  , IDC_STATIC  , 1, "保存ファイル名"    , 685, 610, 140, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_EDIT19  , 0, ""                  , 685, 630, 250, 30, WS_BORDER},
	{ 0, 0  ,            0, 0, NULL}
};

#if 1//2017.04.01
static
void CHECK_FIXED(CWnd *pWndForm)
{
	CArray<UINT, UINT> ar;

	if (CKOP::FP.MTF_ZOOM >= 0) {
		m_d.P_ZOOM = CKOP::FP.MTF_ZOOM;
		ar.Add(IDC_STATIC12);
	}
	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_W)) {
		m_d.P_RT_W = CKOP::FP.MTF_W;
		ar.Add(IDC_CHECK1);
	}
	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_B)) {
		m_d.P_RT_B = CKOP::FP.MTF_B;
		ar.Add(IDC_CHECK2);
	}
	//---
	m_d.R_RECT_T[0] = m_d.P_FIXED_T[0] = CKOP::FP.MTF_RECT_T1;
	m_d.R_RECT_Y[0] = m_d.P_FIXED_Y[0] = CKOP::FP.MTF_RECT_Y1;
	m_d.R_RECT_T[1] = m_d.P_FIXED_T[1] = CKOP::FP.MTF_RECT_T2;
	m_d.R_RECT_Y[1] = m_d.P_FIXED_Y[1] = CKOP::FP.MTF_RECT_Y2;
	m_d.R_RECT_T[2] = m_d.P_FIXED_T[2] = CKOP::FP.MTF_RECT_T3;
	m_d.R_RECT_Y[2] = m_d.P_FIXED_Y[2] = CKOP::FP.MTF_RECT_Y3;
	m_d.R_RECT_T[3] = m_d.P_FIXED_T[3] = CKOP::FP.MTF_RECT_T4;
	m_d.R_RECT_Y[3] = m_d.P_FIXED_Y[3] = CKOP::FP.MTF_RECT_Y4;
	m_d.R_RECT_T[4] = m_d.P_FIXED_T[4] = CKOP::FP.MTF_RECT_T5;
	m_d.R_RECT_Y[4] = m_d.P_FIXED_Y[4] = CKOP::FP.MTF_RECT_Y5;
	//---
	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_T1) && CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_Y1)) {
		m_d.P_FIXED |= 1;
		//-----
		m_d.P_RT_L = m_d.P_FIXED_T[0];
		m_d.P_RT_R = m_d.P_FIXED_Y[0];
	}
	else {
		m_d.R_MTF_T[0] = C_NAN();
		m_d.R_MTF_Y[0] = C_NAN();
	}
	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_T2) && CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_Y2)) {
		m_d.P_FIXED |= 2;
	}
	else {
		m_d.R_MTF_T[1] = C_NAN();
		m_d.R_MTF_Y[1] = C_NAN();
	}
	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_T3) && CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_Y3)) {
		m_d.P_FIXED |= 4;
	}
	else {
		m_d.R_MTF_T[2] = C_NAN();
		m_d.R_MTF_Y[2] = C_NAN();
	}

	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_T4) && CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_Y4)) {
		m_d.P_FIXED |= 8;
	}
	else {
		m_d.R_MTF_T[3] = C_NAN();
		m_d.R_MTF_Y[3] = C_NAN();
	}

	if (CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_T5) && CKOP::IS_RECT_OK(&CKOP::FP.MTF_RECT_Y5)) {
		m_d.P_FIXED |= 16;
	}
	else {
		m_d.R_MTF_T[4] = C_NAN();
		m_d.R_MTF_Y[4] = C_NAN();
	}

	if (CKOP::FP.SCALE_SHAD_CORR >= 0) {
		m_d.P_SHAD_CORR = CKOP::FP.SCALE_SHAD_CORR;
		ar.Add(IDC_CHECK2);
	}

	for (int i = 0; i < ar.GetCount(); i++) {
		pWndForm->GetDlgItem(ar[i])->EnableWindow(FALSE);
	}
}
LPCTSTR RECT2STR(LPRECT p)
{
	static
	char	buf[64];
	sprintf_s(buf, _countof(buf), "(%d,%d)-(%d,%d)", p->left, p->top, p->right, p->bottom);
	return(buf);
}
#endif
#if 1//2015.07.24
void CKOP_MTF::CHECK_RECT(LPRECT pr)
{
	if (pr->top < 0) {
		pr->top = 0;
	}
	if (pr->top >= CKOP::BMP_HEI) {
		pr->top =  CKOP::BMP_HEI-1;
	}
	if (pr->bottom < 0) {
		pr->bottom = 0;
	}
	if (pr->bottom >= CKOP::BMP_HEI) {
		pr->bottom =  CKOP::BMP_HEI-1;
	}
	//---
	if (pr->left < 0) {
		pr->left = 0;
	}
	if (pr->left >= CKOP::BMP_WID) {
		pr->left =  CKOP::BMP_WID-1;
	}
	if (pr->right < 0) {
		pr->right = 0;
	}
	if (pr->right >= CKOP::BMP_WID) {
		pr->right =  CKOP::BMP_WID-1;
	}
}
#endif
void CKOP_MTF::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	m_d.P_ZOOM        = GetProfileINT("MTF", "ZOOM"   , 1);
	m_d.P_RT_L.left   = GetProfileINT("MTF", "L.X1"   , 0);
	m_d.P_RT_L.top    = GetProfileINT("MTF", "L.Y1"   , 0);
	m_d.P_RT_L.right  = GetProfileINT("MTF", "L.X2"   , 1);
	m_d.P_RT_L.bottom = GetProfileINT("MTF", "L.Y2"   , 1);

	m_d.P_RT_R.left   = GetProfileINT("MTF", "R.X1"   , 0);
	m_d.P_RT_R.top    = GetProfileINT("MTF", "R.Y1"   , 0);
	m_d.P_RT_R.right  = GetProfileINT("MTF", "R.X2"   , 1);
	m_d.P_RT_R.bottom = GetProfileINT("MTF", "R.Y2"   , 1);

	m_d.P_RT_W.left   = GetProfileINT("MTF", "W.X1"   , 0);
	m_d.P_RT_W.top    = GetProfileINT("MTF", "W.Y1"   , 0);
	m_d.P_RT_W.right  = GetProfileINT("MTF", "W.X2"   , 1);
	m_d.P_RT_W.bottom = GetProfileINT("MTF", "W.Y2"   , 1);

	m_d.P_RT_B.left   = GetProfileINT("MTF", "B.X1"   , 0);
	m_d.P_RT_B.top    = GetProfileINT("MTF", "B.Y1"   , 0);
	m_d.P_RT_B.right  = GetProfileINT("MTF", "B.X2"   , 1);
	m_d.P_RT_B.bottom = GetProfileINT("MTF", "B.Y2"   , 1);

	m_d.P_SAVEPOS     = GetProfileINT("MTF", "SAVEPOS", 0);
	m_d.P_MODE2       = GetProfileINT("MTF", "MODE2"  , 0);

	m_d.R_MTF_T[0]    = GetProfileDbl("MTF", "MTF_T:0", C_NAN());
	m_d.R_MTF_Y[0]    = GetProfileDbl("MTF", "MTF_Y:0", C_NAN());
	m_d.R_MTF_T[1]    = GetProfileDbl("MTF", "MTF_T:1", C_NAN());
	m_d.R_MTF_Y[1]    = GetProfileDbl("MTF", "MTF_Y:1", C_NAN());
	m_d.R_MTF_T[2]    = GetProfileDbl("MTF", "MTF_T:2", C_NAN());
	m_d.R_MTF_Y[2]    = GetProfileDbl("MTF", "MTF_Y:2", C_NAN());
	m_d.R_MTF_T[3]    = GetProfileDbl("MTF", "MTF_T:3", C_NAN());
	m_d.R_MTF_Y[3]    = GetProfileDbl("MTF", "MTF_Y:3", C_NAN());
	m_d.R_MTF_T[4]    = GetProfileDbl("MTF", "MTF_T:4", C_NAN());
	m_d.R_MTF_Y[4]    = GetProfileDbl("MTF", "MTF_Y:4", C_NAN());
#if 1//2017.04.01
	m_d.R_RECT_T[0]   =*GetProfileRec("MTF", "RECT_T:0", -1);
	m_d.R_RECT_Y[0]   =*GetProfileRec("MTF", "RECT_Y:0", -1);
	m_d.R_RECT_T[1]   =*GetProfileRec("MTF", "RECT_T:1", -1);
	m_d.R_RECT_Y[1]   =*GetProfileRec("MTF", "RECT_Y:1", -1);
	m_d.R_RECT_T[2]   =*GetProfileRec("MTF", "RECT_T:2", -1);
	m_d.R_RECT_Y[2]   =*GetProfileRec("MTF", "RECT_Y:2", -1);
	m_d.R_RECT_T[3]   =*GetProfileRec("MTF", "RECT_T:3", -1);
	m_d.R_RECT_Y[3]   =*GetProfileRec("MTF", "RECT_Y:3", -1);
	m_d.R_RECT_T[4]   =*GetProfileRec("MTF", "RECT_T:4", -1);
	m_d.R_RECT_Y[4]   =*GetProfileRec("MTF", "RECT_Y:4", -1);
#endif
#if 1//2015.09.10
	m_d.P_SHAD_CORR = GetProfileINT("MTF", "SHAD_CORR" , 0);
	if (!CKOP_SHADING::IS_AVAILABLE_CORRECTION() || !CKOP_SHADING_EX::IS_AVAILABLE_CORRECTION()) {
		 m_d.P_SHAD_CORR = 0;
		 pWndForm->GetDlgItem(IDC_CHECK3)->EnableWindow(FALSE);
	}
	pWndForm->CheckDlgButton(IDC_CHECK3, m_d.P_SHAD_CORR!=0);
#endif
#if 1//2017.04.01
	m_d.P_MODE2       = G_SS.MTF_MODE;
	m_d.P_FIXED = 0;
	if (G_SS.USE_FIXED) {
		CKOP::GetProfileFixed();
		CHECK_FIXED(pWndForm);
		pWndForm->GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
	}
#endif
#if 1//2015.07.24
	CHECK_RECT(&m_d.P_RT_L);
	CHECK_RECT(&m_d.P_RT_R);
	CHECK_RECT(&m_d.P_RT_W);
	CHECK_RECT(&m_d.P_RT_B);

#endif
	switch (m_d.P_MODE2) {										// 2017.07.06 方式3　を追加　by　uchida
	case 0:
		pWndForm->GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
		pWndForm->GetDlgItem(IDC_CHECK2)->ShowWindow(SW_HIDE);
		break;
	case 1:
		pWndForm->SetDlgItemTextA(IDC_STATIC10, "MTF解析(方式2) MAX/MINをﾉｰﾓﾗｲｽﾞ");
		break;
	case 2:
		pWndForm->SetDlgItemTextA(IDC_STATIC10, "MTF解析(方式3) STDV()/AVE()");
		pWndForm->SetDlgItemTextA(IDC_STATIC55, "平均値");
		pWndForm->SetDlgItemTextA(IDC_STATIC56, "標準偏差");
		pWndForm->SetDlgItemTextA(IDC_STATIC57, "平均値");
		pWndForm->SetDlgItemTextA(IDC_STATIC58, "標準偏差");
		break;
	}

	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->SetCurSel(m_d.P_SAVEPOS);

	m_pLineBuf[0] = (LPBYTE)malloc(CKOP::BMP_WID);
	m_pLineBuf[1] = (LPBYTE)malloc(CKOP::BMP_WID);

	CKOP::GDEF_INIT(&m_gdef[0], /*bitLBL=*/0+0, pWndForm->GetDlgItem(IDC_STATIC15));
	CKOP::GDEF_PSET(&m_gdef[0], 0, CKOP::BMP_WID-1, CKOP::BMP_WID/8, 0, 255, 64);
	//-----
	CKOP::GDEF_INIT(&m_gdef[1], /*bitLBL=*/0+0, pWndForm->GetDlgItem(IDC_STATIC16));
	CKOP::GDEF_PSET(&m_gdef[1], 0, CKOP::BMP_WID-1, CKOP::BMP_WID/8, 0, 255, 64);

	UPDATE_LABELS(pWndForm);
	for (int i = 0; i < ROWS(m_d.R_MTF_T); i++) {
		UPDATE_RESULT(pWndForm, 1+i);							// 結果をUP-DATEする。
	}

	if (m_d.P_MODE2 == 1) {										// MTF　方式2の場合
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP6), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC17))->SetBitmap(hBMP);
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP7), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC18))->SetBitmap(hBMP);
	}															// MTF　方式1,3の場合
	else {
	pWndForm->GetDlgItem(IDC_STATIC17)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC18)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC23)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC43)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC44)->ShowWindow(SW_HIDE);

	}

	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}
#if 1//2017.04.01
	if (G_SS.USE_FIXED && !(m_d.P_FIXED &1)) {
		mlog("測定1のエリア設定が指定されていません.");
		pWndForm->PostMessage(WM_COMMAND, MAKELONG(IDC_BUTTON8,0));
	}
#endif
#if 1//2018.05.01
	if (!G_SS.USE_SURUGA || (G_SS.USE_SURUGA && STG::IS_OPENED() == FALSE)) {
		UINT nIDs[] = {
			IDC_STATIC60, IDC_EDIT2   ,	IDC_STATIC59, IDC_BUTTON9 ,
			IDC_BUTTON10, IDC_BUTTON11, IDC_BUTTON12, IDC_BUTTON13,
			IDC_BUTTON14, IDC_BUTTON15,	IDC_BUTTON16
		};
		for (int i = 0; i < ROWS(nIDs); i++) {
			if (!G_SS.USE_SURUGA) {
				pWndForm->GetDlgItem(nIDs[i])->ShowWindow(SW_HIDE);
			}
			else {
				pWndForm->GetDlgItem(nIDs[i])->EnableWindow(FALSE);
			}
		}
	}
	else {
		pWndForm->SetDlgItemText(IDC_STATIC59, STG::UNIT_STR(1));
		CKOP::UPD_STG_TEXT(pWndForm, IDC_EDIT2, 1);
	}
#endif

}

void CKOP_MTF::TERM_FORM(void)
{
	free(m_pLineBuf[0]);
	free(m_pLineBuf[1]);
	m_pLineBuf[0] = NULL;
	m_pLineBuf[1] = NULL;

	WriteProfileINT("MTF", "ZOOM"   , m_d.P_ZOOM       );

	WriteProfileINT("MTF", "L.X1"   , m_d.P_RT_L.left  );
	WriteProfileINT("MTF", "L.Y1"   , m_d.P_RT_L.top   );
	WriteProfileINT("MTF", "L.X2"   , m_d.P_RT_L.right );
	WriteProfileINT("MTF", "L.Y2"   , m_d.P_RT_L.bottom);

	WriteProfileINT("MTF", "R.X1"   , m_d.P_RT_R.left  );
	WriteProfileINT("MTF", "R.Y1"   , m_d.P_RT_R.top   );
	WriteProfileINT("MTF", "R.X2"   , m_d.P_RT_R.right );
	WriteProfileINT("MTF", "R.Y2"   , m_d.P_RT_R.bottom);

	WriteProfileINT("MTF", "W.X1"   , m_d.P_RT_W.left  );
	WriteProfileINT("MTF", "W.Y1"   , m_d.P_RT_W.top   );
	WriteProfileINT("MTF", "W.X2"   , m_d.P_RT_W.right );
	WriteProfileINT("MTF", "W.Y2"   , m_d.P_RT_W.bottom);

	WriteProfileINT("MTF", "B.X1"   , m_d.P_RT_B.left  );
	WriteProfileINT("MTF", "B.Y1"   , m_d.P_RT_B.top   );
	WriteProfileINT("MTF", "B.X2"   , m_d.P_RT_B.right );
	WriteProfileINT("MTF", "B.Y2"   , m_d.P_RT_B.bottom);

	WriteProfileINT("MTF", "SAVEPOS", m_d.P_SAVEPOS );
	WriteProfileINT("MTF", "MODE2"  , m_d.P_MODE2   );

	WriteProfileDbl("MTF", "MTF_T:0", m_d.R_MTF_T[0]);
	WriteProfileDbl("MTF", "MTF_Y:0", m_d.R_MTF_Y[0]);
	WriteProfileDbl("MTF", "MTF_T:1", m_d.R_MTF_T[1]);
	WriteProfileDbl("MTF", "MTF_Y:1", m_d.R_MTF_Y[1]);
	WriteProfileDbl("MTF", "MTF_T:2", m_d.R_MTF_T[2]);
	WriteProfileDbl("MTF", "MTF_Y:2", m_d.R_MTF_Y[2]);
	WriteProfileDbl("MTF", "MTF_T:3", m_d.R_MTF_T[3]);
	WriteProfileDbl("MTF", "MTF_Y:3", m_d.R_MTF_Y[3]);
	WriteProfileDbl("MTF", "MTF_T:4", m_d.R_MTF_T[4]);
	WriteProfileDbl("MTF", "MTF_Y:4", m_d.R_MTF_Y[4]);
#if 1//2017.04.01
	WriteProfileRec("MTF", "RECT_T:0", &m_d.R_RECT_T[0]);
	WriteProfileRec("MTF", "RECT_Y:0", &m_d.R_RECT_Y[0]);
	WriteProfileRec("MTF", "RECT_T:1", &m_d.R_RECT_T[1]);
	WriteProfileRec("MTF", "RECT_Y:1", &m_d.R_RECT_Y[1]);
	WriteProfileRec("MTF", "RECT_T:2", &m_d.R_RECT_T[2]);
	WriteProfileRec("MTF", "RECT_Y:2", &m_d.R_RECT_Y[2]);
	WriteProfileRec("MTF", "RECT_T:3", &m_d.R_RECT_T[3]);
	WriteProfileRec("MTF", "RECT_Y:3", &m_d.R_RECT_Y[3]);
	WriteProfileRec("MTF", "RECT_T:4", &m_d.R_RECT_T[4]);
	WriteProfileRec("MTF", "RECT_Y:4", &m_d.R_RECT_Y[4]);
#endif
#if 1//2015.09.10
	WriteProfileINT("MTF", "SHAD_CORR"  , m_d.P_SHAD_CORR);
#endif
}

//
// BMP座標系からZOOM座標系に変換(ZOOMエリアは(200x200)固定)
// 
//(ru.left, ru.top) - (ru.right, ru.bottom)
//     　　　　　　 ↓(写像)
//(      0,      0) - (     200,       200)
// この写像関係から、rbを写像を通した時の座標を返すことになる
//
// ru:BMP座標系
//
void BMPCD_TO_ZOMCD(LPRECT rb, LPRECT ru, LPRECT rz)
{
	int	wid = ru->right  - ru->left;
	int	hei = ru->bottom - ru->top;

	rz->left   = (rb->left   - ru->left) * 200 / wid;
	rz->right  = (rb->right  - ru->left) * 200 / wid;
	//-----
	rz->top    = (rb->top    - ru->top ) * 200 / hei;
	rz->bottom = (rb->bottom - ru->top ) * 200 / hei;
}
//
// ZOOM座標系からBMP座標系に変換
//
void ZOMCD_TO_BMPCD(LPRECT rz, LPRECT ru, LPRECT rb)
{
	int	wid = ru->right  - ru->left;
	int	hei = ru->bottom - ru->top;

	rb->left   =  rz->left   * wid / 200 + ru->left;
	rb->right  =  rz->right  * wid / 200 + ru->left;
	//-----
	rb->top    =  rz->top    * hei / 200 + ru->top ;
	rb->bottom =  rz->bottom * hei / 200 + ru->top ;
}
/********************************************************************/
void CKOP_MTF::ROI_MAX_MIN(LPBYTE pImgPxl, LPRECT pr, int *pmax, int *pmin)				// MAX MIN　を計算
{																						// 方式1用
	int		max, min, ret;

	max = -1, min = 256;

	for (int y = pr->top; y < pr->bottom; y++) {
		for (int x = pr->left; x < pr->right; x++) {
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
		}
	}
	*pmax = max;
	*pmin = min;
}
void CKOP_MTF::ROI_STDV(LPBYTE pImgPxl, LPRECT pr, double *paverage, double *pstdv)		// 平均値と標準偏差　を計算
{																						// 方式3用
	int		icnt,ret;
	double	average,stdv;

	icnt = 0;
	average = 0, stdv = 0;

	for (int y = pr->top; y < pr->bottom; y++) {
		for (int x = pr->left; x < pr->right; x++) {
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
			icnt ++;
			average += ret;
		}
	}
	average = average / double(icnt);
	for (int y = pr->top; y < pr->bottom; y++) {
		for (int x = pr->left; x < pr->right; x++) {
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
			stdv += (ret-average)*(ret-average);
		}
	}
	stdv = sqrt(stdv/double(icnt));

	*paverage = average;
	*pstdv = stdv;
}
double CKOP_MTF::ROI_AVG_SEL(LPBYTE pImgPxl, LPRECT pr, int hi, int lo)
{
	int		ttl=0;
	int		q = 0;

	for (int y = pr->top; y < pr->bottom; y++) {
		for (int x = pr->left; x < pr->right; x++) {
			int	ret = CKOP::GET_PIXEL(x, y, pImgPxl);
			if (ret >= lo && ret <= hi) {
				ttl += ret;
				q++;
			}
		}
	}
	if (q <= 0) {
		return(0);
	}
	return((double)ttl / q);
}
double CKOP_MTF::ROI_AVG(LPBYTE pImgPxl, LPRECT pr)
{
	int		ttl = 0;
	int		q = 0;

	for (int y = pr->top; y < pr->bottom; y++) {
		for (int x = pr->left; x < pr->right; x++) {
			int ret = CKOP::GET_PIXEL(x, y, pImgPxl);
			if (TRUE) {
				ttl += ret;
				q++;
			}
		}
	}
	if (q <= 0) {
		return(0);
	}
	return((double)ttl / q);
}

void CKOP_MTF::CALC_MTF(LPBYTE pImgPxl)					// MTF 計算のためのサブルーチン　
{
	int		max, min, wid;
	double	avg_w, avg_b,bas_w, bas_b, mtf,stdv,average;

	if (m_d.P_MODE2 == 1) {								// 方式2の場合
		m_d.R_BAS_W = bas_w = ROI_AVG(pImgPxl, &m_d.P_RT_W);
		m_d.R_BAS_B = bas_b = ROI_AVG(pImgPxl, &m_d.P_RT_B);
	}

#if 1//2017.04.01
	for (int u = 0; u < 5; u++) {
	if (!G_SS.USE_FIXED) {
		if (u >= 1) {
			break;					//非固定モード(従来通り)
		}
	}
	else {
		if (!(m_d.P_FIXED & (1<<u))) {
			continue;				//skip
		}
	}
#endif
	for (int q = 0; q < 2; q++) {	// q=0で　　横の計算　q=1で　縦の計算を実施
		CRect	rt;
		if (q == 0) {
			rt = m_d.P_RT_L;
		}
		else {
			rt = m_d.P_RT_R;		/******************************************************/
		}

		if (G_SS.USE_FIXED) {
			if (q == 0) {
				rt = m_d.P_FIXED_T[u];//縦(左)
			}
			else {
				rt = m_d.P_FIXED_Y[u];//横(右)
			}
		}

		if (rt.bottom < rt.top || rt.right < rt.left) {
			rt.NormalizeRect();
		}
		ROI_MAX_MIN(pImgPxl, &rt, &max, &min);
		ROI_STDV(pImgPxl, &rt, &average, &stdv);
		wid = max-min;
		avg_w = ROI_AVG_SEL(pImgPxl, &rt, (wid)*98/100 + min, (wid)*95/100 + min);
		avg_b = ROI_AVG_SEL(pImgPxl, &rt, (wid)* 5/100 + min, (wid)* 2/100 + min);
		switch (m_d.P_MODE2) {
			case 0:					//　方式1
				mtf = (avg_w-avg_b)/(avg_w+avg_b)*100.0;
				break;
			case 1:					//　方式2
				mtf = (avg_w-avg_b)/(bas_w-bas_b)*100.0;
				break;
			case 2:					//　 方式3
				mtf = (stdv / average)*100.0;
				break;
		}

		if (u == 0) {				// 測定エリア0　の数値を表示
			if(m_d.P_MODE2 != 2 ){
				m_d.R_AVG_W[q] = avg_w;
				m_d.R_AVG_B[q] = avg_b;
			}else{
				m_d.R_AVG_W[q] = average;
				m_d.R_AVG_B[q] = stdv;
			}
		}

		m_d.R_MTF  [q] = mtf;		//　MTF測定　1～5　を行列に代入

		if (G_SS.USE_FIXED && u > 0) {
			continue;
		}

		if (q == 0) {
			int	y = (rt.top + rt.bottom)/2;
			for (int x = 0; x < CKOP::BMP_WID; x++) {
				*(m_pLineBuf[0] + x) = (BYTE)CKOP::GET_PIXEL(x, y, pImgPxl);	//画像横方向に計測ライン設定時
			}
		}
		else {
			int	x = (rt.left + rt.right)/2;
			for (int y = 0; y < CKOP::BMP_HEI; y++) {
				*(m_pLineBuf[1] + y) = (BYTE)CKOP::GET_PIXEL(x, y, pImgPxl);	//画像縦方向に計測ライン設定時
			}
		}
	}

	m_d.B_MTF_T[u] = m_d.R_MTF[0];												//測定結果:MTF(縦)
	m_d.B_MTF_Y[u] = m_d.R_MTF[1];												//測定結果:MTF(横)
	}

}

void CKOP_MTF::DRAW_MARQUEE(CWnd *pWnd, int mask)								//MTF算出のための関心領域描画
{
	CDC	*pDC = pWnd->GetDC();
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	if (m_d.P_MODE2 == 1) {
		pDC->SetBkColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);
	}

	if (mask & 1) {
		CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_RT_L));
		if (m_d.P_MODE2 == 1) {
		pDC->SelectObject(&CKOP::m_penGreenDot);
		CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_RT_W));
		}
	}

	if (mask & 2) {
		pDC->SelectObject(&CKOP::m_penGreen);								//左画像上の右クリックでのROI描画色
		CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_RT_R));
		if (m_d.P_MODE2 == 1 ) {
		pDC->SelectObject(&CKOP::m_penRedDot);
		CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_RT_B));
		}
	}
#if 1//2017.04.01
	if (G_SS.USE_FIXED) {
		for (int u = 0; u < 5; u++) {
			if (!(m_d.P_FIXED & (1<<u))) {
				continue;//skip
			}
			if (mask & 1) {
				pDC->SelectObject(&CKOP::m_penRed);
				CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_FIXED_T[u]));
			}
			if (mask & 2) {
				pDC->SelectObject(&CKOP::m_penGreen);
				CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_FIXED_Y[u]));
			}
		}
		if (m_d.P_MODE2 == 1) {
			if (mask & 1) {
				pDC->SelectObject(&CKOP::m_penGreenDot);
				CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_RT_W));
			}
			if (mask & 2) {
				pDC->SelectObject(&CKOP::m_penRedDot);
				CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_RT_B));
			}
		}
	}
#endif

	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}

void CKOP_MTF::DRAW_MARQUEE_IN_ZOOM(CWnd *pWnd, CWnd* pWndZoom)
{
	CDC*	pDC = pWnd->GetDC();
	CDC*	pDCZoom = pWndZoom->GetDC();
	CPen*	old_pen;

	if (m_d.B_ZM_ID == IDC_STATIC13) {
		old_pen = pDCZoom->SelectObject(&CKOP::m_penRed);

		CKOP::DRAW_MARQUEE(pDCZoom, &m_d.B_RT_L);

		pDCZoom->SelectObject(old_pen);
	}
	else {
		old_pen = pDCZoom->SelectObject(&CKOP::m_penGreen);

		CKOP::DRAW_MARQUEE(pDCZoom, &m_d.B_RT_R);

		pDCZoom->SelectObject(old_pen);
	}
	pWndZoom->ReleaseDC(pDCZoom);
	pWnd->ReleaseDC(pDC);
}

void CKOP_MTF::DRAW_ZOOM(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pBmpInfo)
{
	POINT	pnt;
	CRect	rt;
	LPRECT	pr;
	CRect	rn;
	CDC*	pDC;
	CWnd*	pWnd;
	CPen*	old_pen;
	CRgn	rgn;
	static
	BOOL	chk = 0;

	rgn.CreateRectRgn(0,0,200,200);

	// ZOOM表示枠は200x200dot
	for (int i = 0; i < 2; i++) {
		UINT	nID;
		LPRECT	pLR;
		LPRECT	pLRU;
		CPen*	pen;

		if (!i) {
			nID = IDC_STATIC13;
			pLR = &m_d.P_RT_L;
			pLRU= &m_d.P_RT_LU;
			pen = &CKOP::m_penRed;					//右上の画像上の左クリックでの赤ROI
		}
		else {
			nID = IDC_STATIC14;
			pLR = &m_d.P_RT_R;/******************************************/
			pLRU= &m_d.P_RT_RU;
			pen = &CKOP::m_penGreen;				//右上の画像上の左クリックでの緑ROI
		}
		pnt.x = (pLR->left + pLR->right )/2;		//矩形の中心(BMP座標系)
		pnt.y = (pLR->top  + pLR->bottom)/2;
		pWnd = pWndForm->GetDlgItem(nID);

//		CKOP::DRAW_ZOOM_EX(pWnd, BMPCD_TO_IMGCD(&pnt), pImgPxl, pBmpInfo, m_d.P_ZOOM);
		if (chk) {
		CKOP::DRAW_ZOOM_EX(pWnd, CKOP::IMGCD_TO_BMPCD(CKOP::BMPCD_TO_IMGCD(&pnt)), pImgPxl, pBmpInfo, m_d.P_ZOOM);
		}
		else {
		CKOP::DRAW_ZOOM_EX(pWnd,                &pnt , pImgPxl, pBmpInfo, m_d.P_ZOOM);
		}

		pLRU->left   = pnt.x-100/m_d.P_ZOOM;
		pLRU->right  = pnt.x+100/m_d.P_ZOOM;
		pLRU->top    = pnt.y-100/m_d.P_ZOOM;
		pLRU->bottom = pnt.y+100/m_d.P_ZOOM;

		pr = pLR;

		BMPCD_TO_ZOMCD(pLR, pLRU, &rn);

		pDC = pWnd->GetDC();
		old_pen = pDC->SelectObject(pen);
		pDC->SelectClipRgn(&rgn);
		if ((i == 0 && m_d.B_ZM_ID == IDC_STATIC13) || (i == 1 && m_d.B_ZM_ID == IDC_STATIC14)) {
			//ZOOMエリアにてマウス操作中
		}
		else {
			CKOP::DRAW_MARQUEE(pDC, &rn);
		}
		pWnd->ReleaseDC(pDC);
	}
}

void CKOP_MTF::DRAW_GRAPH(CWnd* pWndForm, int mask)
{
	UINT	nIDS[] = {
		IDC_STATIC15, IDC_STATIC16
	};
	for (int i = 0; i < 2; i++) {
		if (!(mask & (1<<i))) {
			continue;
		}
		CWnd*	pWnd = pWndForm->GetDlgItem(nIDS[i]);
		CDC*	pDC = pWnd->GetDC();


		CRect	rt;
		int		h = 0;
		LPBYTE	p;
		CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
//		int		ycnt, xcnt;

		p = m_pLineBuf[i];
		if (!i) {
			rt = m_d.P_RT_LU;
		}
		else {
			rt = m_d.P_RT_RU;
		}

		int	q1, q2;
		if (!i) {
			q1 = rt.left;
			q2 = rt.right;
		}
		else {
			q1 = rt.top;
			q2 = rt.bottom;
		}

		CKOP::GDEF_PSET(&m_gdef[i], q1, q2, (q2-q1)/8, 0, 255, (256-0)/4);
		CKOP::GDEF_GRID(&m_gdef[i], pDC);
		
		p += q1;
#if 1//2015.09.10
		int	cnt = 0;
		int	lim = (i==0) ? CKOP::BMP_WID: CKOP::BMP_HEI;
#endif

		for (int q = q1; q <= q2; q++, p++) {
			int	x, y;
#if 1//2015.09.10
			if (q < 0 || q >= lim) {
				continue;
			}
#endif
			x = CKOP::GDEF_XPOS(&m_gdef[i], q);
			y = CKOP::GDEF_YPOS(&m_gdef[i], (int)*p);
//			y = 100;
			if (
#if 1
				cnt == 0
#else
				q <= q1
#endif
				) {
				pDC->MoveTo(x, y);
			}
			else {
				pDC->LineTo(x, y);
			}
#if 1//2015.09.10
			cnt++;
#endif
		}
		pDC->SelectObject(&CKOP::m_penExtra);
		if (TRUE) {
			int	yy = CKOP::GDEF_YPOS(&m_gdef[i], (int)m_d.R_AVG_B[i]);
			pDC->SelectObject(&CKOP::m_penExtra);
		
			pDC->MoveTo(m_gdef[i].rt_gr.left , yy);
			pDC->LineTo(m_gdef[i].rt_gr.right, yy);
		}
		if (TRUE) {
			int	yy = CKOP::GDEF_YPOS(&m_gdef[i], (int)m_d.R_AVG_W[i]);
		
			pDC->MoveTo(m_gdef[i].rt_gr.left , yy);
			pDC->LineTo(m_gdef[i].rt_gr.right, yy);
		}
		if (i == 0) {
			int	x1 = CKOP::GDEF_XPOS(&m_gdef[i], (int)m_d.P_RT_L.left);
			int	x2 = CKOP::GDEF_XPOS(&m_gdef[i], (int)m_d.P_RT_L.right);
		
			pDC->SelectObject(&CKOP::m_penRed);
			if (x1 >= m_gdef[i].rt_gr.left && x1 <= m_gdef[i].rt_gr.right) {
			pDC->MoveTo(x1, m_gdef[i].rt_gr.top   );
			pDC->LineTo(x1, m_gdef[i].rt_gr.bottom);
			}
			if (x2 >= m_gdef[i].rt_gr.left && x2 <= m_gdef[i].rt_gr.right) {
			pDC->MoveTo(x2, m_gdef[i].rt_gr.top   );
			pDC->LineTo(x2, m_gdef[i].rt_gr.bottom);
			}
		}
		else {
			int	x1 = CKOP::GDEF_XPOS(&m_gdef[i], (int)m_d.P_RT_R.top);
			int	x2 = CKOP::GDEF_XPOS(&m_gdef[i], (int)m_d.P_RT_R.bottom);
		
			pDC->SelectObject(&CKOP::m_penGreen);
			if (x1 >= m_gdef[i].rt_gr.left && x1 <= m_gdef[i].rt_gr.right) {
			pDC->MoveTo(x1, m_gdef[i].rt_gr.top   );
			pDC->LineTo(x1, m_gdef[i].rt_gr.bottom);
			}
			if (x2 >= m_gdef[i].rt_gr.left && x2 <= m_gdef[i].rt_gr.right) {
			pDC->MoveTo(x2, m_gdef[i].rt_gr.top   );
			pDC->LineTo(x2, m_gdef[i].rt_gr.bottom);
			}
		}

		pDC->SelectObject(old_pen);
		pWnd->ReleaseDC(pDC);
	}
}

void CKOP_MTF::UPDATE_LABELS(CWnd *pWndForm, int mask)
{
	if (mask & 1) {
		CString	buf;
		buf.Format("倍率 x%d", m_d.P_ZOOM);
		pWndForm->SetDlgItemText(IDC_STATIC12, buf);
	}
	if (mask & 2) {

	}
	if (mask & 4) {
#if 1//2017.04.01
		pWndForm->SetDlgItemText(IDC_STATIC41, RECT2STR(&m_d.P_RT_L));
		pWndForm->SetDlgItemText(IDC_STATIC42, RECT2STR(&m_d.P_RT_R));
		pWndForm->SetDlgItemText(IDC_STATIC43, RECT2STR(&m_d.P_RT_W));
		pWndForm->SetDlgItemText(IDC_STATIC44, RECT2STR(&m_d.P_RT_B));
#endif
	}
	if (mask & 8) {
#if 1//2017.04.01
		int	nID1=IDC_STATIC45;
		int	nID2=IDC_STATIC46;
		for (int i = 0; i < 5; i++, nID1+=2, nID2+=2) {
		if (!(m_d.P_FIXED & (1<<i))) {
			continue;//skip
		}
		else {
		pWndForm->SetDlgItemText(nID1, RECT2STR(&m_d.P_FIXED_T[i]));
		pWndForm->SetDlgItemText(nID2, RECT2STR(&m_d.P_FIXED_Y[i]));
		}
		}
#endif
	}
}


void CKOP_MTF::UPDATE_RESULT(CWnd *pWndForm, int idx)									// MTF の結果を画面表示
{
	if (idx == 0) {
		pWndForm->GetDlgItem(IDC_STATIC20)->SetWindowText(F1S(m_d.R_AVG_W[0]));
		pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(F1S(m_d.R_AVG_B[0]));
		pWndForm->GetDlgItem(IDC_STATIC22)->SetWindowText(F2S(m_d.R_MTF[0], "%"));		//MTF(縦)の列の表示

		pWndForm->GetDlgItem(IDC_STATIC26)->SetWindowText(F2S(m_d.R_AVG_W[1]));
		pWndForm->GetDlgItem(IDC_STATIC27)->SetWindowText(F2S(m_d.R_AVG_B[1]));
		pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText(F2S(m_d.R_MTF[1], "%"));		//MTF(横)の列の表示
	}
	else {
		UINT	nIDS_T[] = {
			IDC_STATIC30, IDC_STATIC32, IDC_STATIC34, IDC_STATIC36, IDC_STATIC38
		};
		UINT	nIDS_Y[] = {
			IDC_STATIC31, IDC_STATIC33, IDC_STATIC35, IDC_STATIC37, IDC_STATIC39
		};
		int	i = idx-1;
		CString	buf1, buf2;

		//測定1～5のテスト表示 Start

		if (!_isnan(m_d.R_MTF_T[i])) {
			buf1 = F2S(m_d.R_MTF_T[i], "%");										//測定1～測定5までのMTF（縦）欄に値を%付でbuf1に代入
		}
		if (!_isnan(m_d.R_MTF_Y[i])) {
			buf2 = F2S(m_d.R_MTF_Y[i], "%");										//測定1～測定5までのMTF（横）欄に値を%付でbuf2に代入
		}
		pWndForm->GetDlgItem(nIDS_T[i])->SetWindowText(buf1);						//測定1～測定5までのMTF（縦）欄に値を%付で表示欄にセット
		pWndForm->GetDlgItem(nIDS_Y[i])->SetWindowText(buf2);						//測定1～測定5までのMTF（横）欄に値を%付で表示欄にセット

		if (CKOP::IS_RECT_OK(&m_d.R_RECT_T[i]) && CKOP::IS_RECT_OK(&m_d.R_RECT_Y[i])) {			//測定エリアを表示//
		pWndForm->GetDlgItem(IDC_STATIC45+i*2)->SetWindowText(RECT2STR(&m_d.R_RECT_T[i]));
		pWndForm->GetDlgItem(IDC_STATIC46+i*2)->SetWindowText(RECT2STR(&m_d.R_RECT_Y[i]));
		}
		else {
		pWndForm->GetDlgItem(IDC_STATIC45+i*2)->SetWindowText("");
		pWndForm->GetDlgItem(IDC_STATIC46+i*2)->SetWindowText("");
		}

	}
	if (m_d.P_MODE2) {
		pWndForm->GetDlgItem(IDC_STATIC23)->SetWindowText(F1S(m_d.R_BAS_W));
		pWndForm->GetDlgItem(IDC_STATIC24)->SetWindowText(F1S(m_d.R_BAS_B));
	}
}


void CKOP_MTF::SAVE_RESULT(CWnd *pWndForm)										// MTF結果をSaveする。
{

	if (G_SS.USE_FIXED) {														// 固定モードの時に5つの結果を画面に表示
		for (int i = 0; i < 5; i++) {
			m_d.R_MTF_T[i] = m_d.B_MTF_T[i];
			m_d.R_MTF_Y[i] = m_d.B_MTF_Y[i];
			UPDATE_RESULT(pWndForm, 1+i);
		}
		::MessageBeep(MB_OK);
	}

	m_d.P_SAVEPOS = ((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->GetCurSel();
	
	int		i = m_d.P_SAVEPOS;
	//-----

	m_d.R_RECT_T[i] = m_d.P_RT_L;		//縦
	m_d.R_RECT_Y[i] = m_d.P_RT_R;		//横

	m_d.R_MTF_T[i] = m_d.R_MTF[0];
	m_d.R_MTF_Y[i] = m_d.R_MTF[1];

	UPDATE_RESULT(pWndForm, 1+i);
	::MessageBeep(MB_OK);
}



void CKOP_MTF::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
#if 1//2015.09.10
	m_d.P_SHAD_CORR = pWndForm->IsDlgButtonChecked(IDC_CHECK3);
	if (m_d.P_SHAD_CORR) {
		CKOP_SHADING::DO_CORRECTION(pImgPxl);
		CKOP_SHADING_EX::DO_CORRECTION(pImgPxl);
	}
#endif
	CALC_MTF(pImgPxl);
}

void CKOP_MTF::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);

	DRAW_ZOOM(pWndForm, pImgPxl, pbmpinfo);
	DRAW_GRAPH(pWndForm);

	if (m_d.B_ZM_ID == IDC_STATIC13) {
		DRAW_MARQUEE_IN_ZOOM(pWnd, pWndForm->GetDlgItem(IDC_STATIC13));
	}
	else {
		DRAW_MARQUEE(pWnd, 1);
	}
	if (m_d.B_ZM_ID == IDC_STATIC14) {
		DRAW_MARQUEE_IN_ZOOM(pWnd, pWndForm->GetDlgItem(IDC_STATIC14));
	}
	else {
		DRAW_MARQUEE(pWnd, 2);
	}

	UPDATE_RESULT(pWndForm);
#if 1//2018.05.01
	if (m_s.GET_TRIG) {
		STORE_STG_RESULT();
	}
#endif
}

BOOL CKOP_MTF::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON7://保存
#if 0
			CKOP::SAVE_WINDOW(AfxGetMainWnd(), "mtf.png");
#else
			CKOP::SAVE_WINDOW(pWndForm, "mtf.png");
#endif
#if 1//2017.07.18
			{
				CCSV	csv;
				SET_CSV(&csv);
				CKOP::SAVE_CSV(pWndForm, &csv, "mtf.csv");
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
				for (int i = 0; i < ROWS(m_d.R_MTF_T); i++) {
					m_d.R_MTF_T[i] = C_NAN();
					m_d.R_MTF_Y[i] = C_NAN();
#if 1//2017.04.01
					if (!G_SS.USE_FIXED) {
					m_d.R_RECT_T[i].left =  m_d.R_RECT_T[i].top    = -1;
					m_d.R_RECT_T[i].right = m_d.R_RECT_T[i].bottom = -1;
					}
#endif
					UPDATE_RESULT(pWndForm, 1+i);
				}
			}
		break;
		case IDC_CHECK1:
			if (pWndForm->IsDlgButtonChecked(nID)) {
				pWndForm->CheckDlgButton(IDC_CHECK2, 0);
				m_d.B_BASE = 1;
			}
			else {
				m_d.B_BASE = 0;
			}
		break;
		case IDC_CHECK2:
			if (pWndForm->IsDlgButtonChecked(nID)) {
				pWndForm->CheckDlgButton(IDC_CHECK1, 0);
				m_d.B_BASE = 2;
			}
			else {
				m_d.B_BASE = 0;
			}
		break;
#if 1//2018.05.01
		case IDC_BUTTON9:
			STG::MOVE_ABS(1, /*pos*/0);
		break;
		case IDC_BUTTON10:
			mlog("CLEAR");
		break;
		case IDC_BUTTON12://<STEP+
			STG::MOVE_REL(1, +STG::DS_TBL[0].RELSTEP);
		break;
		case IDC_BUTTON13://STEP->
			STG::MOVE_REL(1, -STG::DS_TBL[0].RELSTEP);
		break;
		case IDC_BUTTON15://オート検出
			GetLocalTime(&m_s.R_TIME);
			m_s.STG_STAT = 1;
			m_s.MES_STEP = (STG::DS_TBL[0].MESRANGE*2)/(STG::DATA_COUNT-1);
			m_s.GET_TRIG = 0;
			m_s.R_IDX = 0;
			m_s.R_MAX_IDX = 0;
			m_s.R_MAX = C_PINF();

			CKOP::DLG_PRG.Create("", AfxGetMainWnd(), TRUE, "オート検出実行中");
			STG::MOVE_ABS(1, -STG::DS_TBL[0].MESRANGE);

		break;
		case IDC_BUTTON16://連続測定
			GetLocalTime(&m_s.R_TIME);
			m_s.STG_STAT = 2;
			m_s.MES_STEP = (STG::DS_TBL[0].MESRANGE*2)/(STG::DATA_COUNT-1);
			m_s.GET_TRIG = 0;
			m_s.R_IDX = 0;
			m_s.R_MAX_IDX = 0;
			m_s.R_MAX = C_PINF();

			CKOP::DLG_PRG.Create("", AfxGetMainWnd(), TRUE, "連続測定実行中");
			STG::MOVE_ABS(1, -STG::DS_TBL[0].MESRANGE);
		break;
#endif
		}
	break;
	}
	return(FALSE);
}


BOOL CKOP_MTF::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
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
				if (m_d.B_BASE == 1) {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_W.left );
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_W.right);
					m_nCaptured = m_d.B_BASE;
				}
				else if (m_d.B_BASE == 2) {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_B.left );
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_B.right);
					m_nCaptured = m_d.B_BASE;
				}
#if 1//2017.04.01
				else if (G_SS.USE_FIXED) {
					//固定モード時はマウスによるエリア設定は行わない
				}
#endif

				else if (pMsg->message == WM_LBUTTONDOWN) {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_L.left );
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_L.right);
					m_nCaptured = 1;
				}
				else {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_R.left );
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_R.right);
					m_nCaptured = 2;
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
				if (m_d.B_BASE == 1) {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_W.right);
				}
				else if (m_d.B_BASE == 2) {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_B.right);
				}

				else if (m_nCaptured == 1) {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_L.right);
				}
				else {
					CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_RT_R.right);
				}
#if 1//2017.04.01
				UPDATE_LABELS(pWndForm, 4);
#endif
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
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			switch (m_d.P_ZOOM) {
			case  1:m_d.P_ZOOM = 2; break;
			case  2:m_d.P_ZOOM = 5; break;
			default:m_d.P_ZOOM = 1; break;
			}
			UPDATE_LABELS(pWndForm, 1);
pWndForm->Invalidate(FALSE);
		break;
		}
	break;
	case IDC_STATIC13:								//ZOOM IMAGE1
	case IDC_STATIC14:								//ZOOM IMAGE2

		if (G_SS.USE_FIXED) {
			break;									//固定モード時はマウスによるエリア設定は行わない
		}

		if (CKOP::MSG_PROC_MOUSE(pWndForm, pMsg, &m_d.B_ZM_CAP, &pnt)) {
			LPRECT	pr;
			if (nID == IDC_STATIC13) {
				pr = &m_d.B_RT_L;
			}
			else {
				pr = &m_d.B_RT_R;
			}
			switch (pMsg->message) {
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				m_d.B_ZM_ID = nID;
				pr->right  = pr->left = pnt.x;
				pr->bottom = pr->top  = pnt.y;
			break;
			case WM_MOUSEMOVE:
				pr->right  = pnt.x;
				pr->bottom = pnt.y;
#if 1//2017.04.01
				UPDATE_LABELS(pWndForm, 4);
#endif
			break;
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
				m_d.B_ZM_ID = 0;
				if (nID == IDC_STATIC13) {
				ZOMCD_TO_BMPCD(pr, &m_d.P_RT_LU, &m_d.P_RT_L);
				}
				else {
				ZOMCD_TO_BMPCD(pr, &m_d.P_RT_RU, &m_d.P_RT_R);
				}
				pWndForm->Invalidate();
#if 1//2017.04.01
				UPDATE_LABELS(pWndForm, 4);
#endif
			break;
			}
		}
	break;
#if 1//2018.05.01
	case IDC_BUTTON11://<JOG+
	case IDC_BUTTON14://JOG->
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			STG::MOVE_JOG(1, nID == IDC_BUTTON11/*<JOG+*/ ? +1: -1, STG::DS_TBL[0].JOG2SPD);
		break;
		case WM_LBUTTONUP:
			STG::MOVE_STP(1, /*bEMS*/0);
		break;
		}
	break;
#endif
	}
	return(FALSE);
}
#if 1//2017.07.18
void CKOP_MTF::SET_CSV(CCSV *pc)
{
	CString	buf;
	LPCTSTR	pHead0[] = {
		"",
		"縦/横",
		"輝度(白)", "輝度(黒)", "MTF(%)",  "エリアX1", "Y1", "X2", "Y2"
	};
	LPCTSTR	pHead1[] = {
		"測定結果", "",
		"測定1", "",
		"測定2", "",
		"測定3", "",
		"測定4", "",
		"測定5", ""
	};
	LPCTSTR	pHead2[] = {
		"",
		"白/黒", "輝度", "", "", "エリアX1", "Y1", "X2", "Y2"
	};
	LPCTSTR	pHead3[] = {
		"",
		"縦/横",
		"平均値", "標準偏差", "MTF(%)",  "エリアX1", "Y1", "X2", "Y2"
	};
	int		r = 0;
	pc->set( 0, r, "MTF解析");
	r++;
	//---
	switch (m_d.P_MODE2) {
	case  1: buf = "MTF解析(方式2) MAX/MINをﾉｰﾓﾗｲｽﾞ"; break;
	case  2: buf = "MTF解析(方式3) STDV()/AVE()"; break;
	default: buf = "MTF解析(方式1)  (MAX-MIN)/(MAX+MIN)"; break;
	}
	pc->set( 0, r, "方式");
	pc->set( 1, r, buf);
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
	if (m_d.P_MODE2 == 1) {
		//---
		r++;//1行空ける
		for (int c = 0; c < ROWS(pHead2); c++) {
			pc->set(c, r, pHead2[c]);
		}
		r++;
		//---
		//MTF解析(方式2)
		for (int i = 0; i < 2; i++) {
			for (int c = 0; c < ROWS(pHead2); c++) {
				buf.Empty();
				if (i == 0) {
					switch (c) {
					case 0: buf = "基準"; break;
					case 1: buf = "白"; break;
					case 2: buf = F1S(m_d.R_BAS_W); break;
					case 3: break;
					case 4: break;
					case 5: buf = I2S(m_d.P_RT_W.left  ); break;
					case 6: buf = I2S(m_d.P_RT_W.top   ); break;
					case 7: buf = I2S(m_d.P_RT_W.right ); break;
					case 8: buf = I2S(m_d.P_RT_W.bottom); break;
					}
				}
				else {
					switch (c) {
					case 0: break;
					case 1: buf = "黒"; break;
					case 2: buf = F1S(m_d.R_BAS_B); break;
					case 3: break;
					case 4: break;
					case 5: buf = I2S(m_d.P_RT_B.left  ); break;
					case 6: buf = I2S(m_d.P_RT_B.top   ); break;
					case 7: buf = I2S(m_d.P_RT_B.right ); break;
					case 8: buf = I2S(m_d.P_RT_B.bottom); break;
					}
				}
				pc->set(c, r, buf);
			}
			r++;
		}
	}
	//---
	r++;//1行空ける
	for (int c = 0; c < ROWS(pHead0); c++) {
		pc->set(c, r, (m_d.P_MODE2 != 2) ? pHead0[c]: pHead3[c]);
	}
	r++;
	//---
	for (int ii= 0; ii< ROWS(pHead1);ii++) {
		for (int c = 0; c < ROWS(pHead0); c++) {
			int i = ii/2;
			int	q = i-1;
			buf.Empty();
			if (c < 2) {
				switch (c) {
				case 0: buf = pHead1[ii]; break;
				case 1: buf = ((ii%2) == 0) ? "縦": "横"; break;
				}
			}
			else if (ii== 0) {
				switch (c) {
				case 2: buf = F2S(m_d.R_AVG_W[0]); break;
				case 3: buf = F2S(m_d.R_AVG_B[0]); break;
				case 4: buf = F2S(m_d.R_MTF[0]); break;
				case 5: buf = I2S(m_d.P_RT_L.left  ); break;
				case 6: buf = I2S(m_d.P_RT_L.top   ); break;
				case 7: buf = I2S(m_d.P_RT_L.right ); break;
				case 8: buf = I2S(m_d.P_RT_L.bottom); break;
				}
			}
			else if (ii== 1) {
				switch (c) {
				case 2: buf = F2S(m_d.R_AVG_W[1]); break;
				case 3: buf = F2S(m_d.R_AVG_B[1]); break;
				case 4: buf = F2S(m_d.R_MTF[1]); break;
				case 5: buf = I2S(m_d.P_RT_R.left  ); break;
				case 6: buf = I2S(m_d.P_RT_R.top   ); break;
				case 7: buf = I2S(m_d.P_RT_R.right ); break;
				case 8: buf = I2S(m_d.P_RT_R.bottom); break;
				}
			}
			else if ((ii%2) == 0) {
				switch (c) {
				case 2: break;
				case 3: break;
				case 4: buf = F2S(m_d.R_MTF_T[q]); break;
				case 5: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_T[q].left  ): ""; break;
				case 6: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_T[q].top   ): ""; break;
				case 7: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_T[q].right ): ""; break;
				case 8: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_T[q].bottom): ""; break;
				}
			}
			else {
				switch (c) {
				case 2: break;
				case 3: break;
				case 4: buf = F2S(m_d.R_MTF_Y[q]); break;
				case 5: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_Y[q].left  ): ""; break;
				case 6: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_Y[q].top   ): ""; break;
				case 7: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_Y[q].right ): ""; break;
				case 8: buf = CKOP::IS_RECT_OK(&m_d.R_RECT_T[q]) ? I2S(m_d.R_RECT_Y[q].bottom): ""; break;
				}
			}
			pc->set(c, r+ii, buf);
		}
	}
	return;
}
#endif
#if 1//2018.05.01
void CKOP_MTF::TIMER_PROC(CWnd* pWndForm)
{
	if (!STG::IS_OPENED()) {
		return;
	}
	if (CKOP::DLG_PRG.GetSafeHwnd() != NULL) {
		if (CKOP::DLG_PRG.CheckCancelButton()) {
			STG::MOVE_STP(1, /*bEMS*/0);
			m_s.STG_STAT = 0;
			CKOP::DLG_PRG.DestroyWindow();
		}
	}
	if (STG::IS_BUSY(1)) {
		STG::CHK_STS(NULL);
		CKOP::UPD_STG_TEXT(pWndForm, IDC_EDIT2, 1);
	}
	else if (m_s.STG_STAT == 0) {
		//nanimo shinai
	}
	else if (m_s.STG_STAT < 10) {
		m_s.CUR_TIC = GetTickCount();
		m_s.GET_TRIG = 1;
		m_s.STG_STAT += 10;
	}
	else if (m_s.STG_STAT <= 20) {
		if (!m_s.GET_TRIG) {
			if (++m_s.R_IDX < STG::DATA_COUNT) {
				CKOP::BEEP(1);
				STG::MOVE_REL(1, m_s.MES_STEP);
				m_s.STG_STAT -= 10;
			}
			else {
				CKOP::BEEP(3);
				m_s.STG_STAT += 10;
			}
		}
	}
	else if (m_s.STG_STAT <= 30) {
		if (m_s.STG_STAT == 21) {
			//オート検出
			double pos = -STG::DS_TBL[0].MESRANGE + m_s.MES_STEP * m_s.R_MAX_IDX;
			STG::MOVE_ABS(1, pos);
		}
		else {
			//連続測定
			STG::MOVE_ABS(1, 0);
			SAVE_STG_RESULT(pWndForm);
		}
		m_s.STG_STAT = 0;
		CKOP::DLG_PRG.DestroyWindow();
	}
}
void CKOP_MTF::STORE_STG_RESULT(void)
{
	if ((GetTickCount() - m_s.CUR_TIC) >= G_SS.STG_WAIT_TIME) {
		int	j = m_s.R_IDX;
		if (j < ROWS(m_s.R_POS)) {
			double avg = 0.0;
			int	cng = 0;
			m_s.R_POS  [j] = STG::POSITION[0];
			if (G_SS.USE_FIXED) {
				for (int i = 0; i < 5; i++) {
				m_s.R_MTF_T[i][j] = m_d.B_MTF_T[i];
				m_s.R_MTF_Y[i][j] = m_d.B_MTF_Y[i];
				avg += m_d.B_MTF_T[i];
				avg += m_d.B_MTF_Y[i];
				}
				avg /= 5;
			}
			else {
				m_s.R_MTF_T[0][j] = m_d.R_MTF[0];
				m_s.R_MTF_Y[0][j] = m_d.R_MTF[1];
				avg += m_d.R_MTF[0];
				avg += m_d.R_MTF[1];
				avg /= 2;
			}
			if (m_s.R_MAX < avg) {
				m_s.R_MAX = avg;
				m_s.R_MAX_IDX = m_s.R_IDX;
			}
		}
		m_s.GET_TRIG = 0;
	}
}
void CKOP_MTF::SAVE_STG_RESULT(CWnd* pWndForm)
{
	CCSV	csv;
	CCSV	*pc = &csv;
	CString	buf;
	LPCTSTR pHead0[] = {
		"",
		"MTF1縦(%)",  "MTF1横(%)",
		"MTF2縦(%)",  "MTF2横(%)",
		"MTF3縦(%)",  "MTF3横(%)",
		"MTF4縦(%)",  "MTF4横(%)",
		"MTF5縦(%)",  "MTF5横(%)"
	};
	LPCTSTR	pHead2[] = {
		"",
		"白/黒", "輝度", "", "", "エリアX1", "Y1", "X2", "Y2"
	};
	int		r = 0;
	pc->set( 0, r, "MTF解析");
	r++;
	//---
	pc->set(0, r, "測定時刻");
	pc->set(1, r, DT2S(&m_s.R_TIME));
	r++;
	//---
	//---
	switch (m_d.P_MODE2) {
	case  1: buf = "MTF解析(方式2) MAX/MINをﾉｰﾓﾗｲｽﾞ"; break;
	case  2: buf = "MTF解析(方式3) STDV()/AVE()"; break;
	default: buf = "MTF解析(方式1)  (MAX-MIN)/(MAX+MIN)"; break;
	}
	pc->set( 0, r, "方式");
	pc->set( 1, r, buf);
	r++;
	//---
	//---
	if (m_d.P_MODE2 == 1) {
		//---
		r++;//1行空ける
		for (int c = 0; c < ROWS(pHead2); c++) {
			pc->set(c, r, pHead2[c]);
		}
		r++;
		//---
		//MTF解析(方式2)
		for (int i = 0; i < 2; i++) {
			for (int c = 0; c < ROWS(pHead2); c++) {
				buf.Empty();
				if (i == 0) {
					switch (c) {
					case 0: buf = "基準"; break;
					case 1: buf = "白"; break;
					case 2: buf = F1S(m_d.R_BAS_W); break;
					case 3: break;
					case 4: break;
					case 5: buf = I2S(m_d.P_RT_W.left  ); break;
					case 6: buf = I2S(m_d.P_RT_W.top   ); break;
					case 7: buf = I2S(m_d.P_RT_W.right ); break;
					case 8: buf = I2S(m_d.P_RT_W.bottom); break;
					}
				}
				else {
					switch (c) {
					case 0: break;
					case 1: buf = "黒"; break;
					case 2: buf = F1S(m_d.R_BAS_B); break;
					case 3: break;
					case 4: break;
					case 5: buf = I2S(m_d.P_RT_B.left  ); break;
					case 6: buf = I2S(m_d.P_RT_B.top   ); break;
					case 7: buf = I2S(m_d.P_RT_B.right ); break;
					case 8: buf = I2S(m_d.P_RT_B.bottom); break;
					}
				}
				pc->set(c, r, buf);
			}
			r++;
		}
	}
	//---
	r++;//1行空ける
	//---
	if (TRUE) {
		LPCTSTR pHead9[] = {
			"エリア", "縦/横", "X1", "Y1", "X2", "Y2"
		};
		for (int c = 0; c < ROWS(pHead9); c++) {
			pc->set(c, r, pHead9[c]);
		}
		r++;
		//---
		for (int i = 0; i < 5*2; i++) {
			int q = i/2;
			CRect	rt;
			BOOL	eve = ((i%2) == 0);

			if (!G_SS.USE_FIXED) {
				if (i >= 2) {
					break;
				}
				if (eve) {
					rt = m_d.P_RT_L;
				}
				else {
					rt = m_d.P_RT_R;
				}
			}
			else {
				if (eve) {
					rt = m_d.R_RECT_T[q];
				}
				else {
					rt = m_d.R_RECT_Y[q];
				}
			}

			for (int c = 0; c < ROWS(pHead9); c++) {
				buf.Empty();
				if (c == 2 && !CKOP::IS_RECT_OK(&rt)) {
					continue;
				}
				switch (c) {
				case 0: buf = eve ? I2S(q+1): ""; break;
				case 1: buf = eve ? "縦": "横"; break;
				case 2: buf = I2S(rt.left  ); break;
				case 3: buf = I2S(rt.top   ); break;
				case 4: buf = I2S(rt.right ); break;
				case 5: buf = I2S(rt.bottom); break;
				}
				pc->set(c, r, buf);
			}
			r++;
		}
	}
	//---
	r++;//1行空ける
	//---
	for (int c = 0; c < ROWS(pHead0); c++) {
		pc->set(c, r, pHead0[c]);
		if (!G_SS.USE_FIXED && c >= 2) {
			break;
		}
	}
	if (TRUE) {
		buf.Format("ステージ位置(%s)", STG::UNIT_STR(1));
		pc->set(0, r, buf);
	}
	r++;
	//---
	for (int i= 0; i< STG::DATA_COUNT;i++) {
		int cmax = (G_SS.USE_FIXED) ? ROWS(pHead0): 3;
		for (int c = 0; c < cmax; c++) {
			buf.Empty();
			if (c == 0) {
				buf = FGS(m_s.R_POS  [i]);
			}
			else {
				int q = (c-1)/2;
				if (G_SS.USE_FIXED && !CKOP::IS_RECT_OK(&m_d.R_RECT_T[q])) {
					buf = "";
				}
				else if ((c%2) != 0) {
					buf = F2S(m_s.R_MTF_T[q][i]);
				}
				else {
					buf = F2S(m_s.R_MTF_Y[q][i]);
				}
			}
			pc->set(c, r, buf);
		}
		r++;
	}
	//---
	pc->set(0, r, "");//1行空ける
	//---
	CKOP::SAVEADD_CSV(pWndForm, &csv, "mtf_stg.csv");
}
#endif