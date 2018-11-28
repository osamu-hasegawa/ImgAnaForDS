#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_SHADING_EX.h"
#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

static
struct {
	// PARAMTER
	int		P_FILTER;
	int		P_PARAM;
	int		P_COLMAP;
	int		P_DIAGO;
	int		P_SKIPCNT;
#if 1//2015.09.10
	int		P_SHAD_CORR;
#endif
#if 1//2017.04.01
	int		P_GABVAL;//ゴミ除去
#endif
	// RESULT
	double	R_SHD_AVG[3];
	int		R_SHD_MAX[3];
	int		R_SHD_MIN[3];
	double	R_SHD_POS[3];
	double	R_SHD_NEG[3];

} m_d;
//------------------------------------
int a1x1;
int a1y1;
int a1x2;
int a1y2;
int a2x1;
int a2y1;
int a2x2;
int a2y2;
int a3x1;
int a3y1;
int a3x2;
int a3y2;
int subCount;

static
COLORREF	m_G2CTBL[256];
static
LPBYTE		m_pLineBuf[2];
static
GDEF		m_gdef;
#if 1//2015.09.10
static
int			m_nLineCnt[2];
static
int			m_scan_code;
static
LPBYTE		m_pShadBuf[2];
static
int			m_nShadSize[2];
static
int			m_nShadBPP[2];
#endif
#if 1//2017.04.01
//CArray<POINT,POINT>	m_ptg;
#endif

	double	*R_SHD_AVG1;//エリア1の細分化したブロックの平均値
	int		*R_SHD_MAX1;//エリア1の細分化したブロックの最大値
	int		*R_SHD_MIN1;//エリア1の細分化したブロックの最小値
	double	*R_SHD_POS1;//エリア1の細分化したブロックの値
	double	*R_SHD_NEG1;//エリア1の細分化したブロックの値

	double	*R_SHD_AVG2;//エリア2の細分化したブロックの平均値
	int		*R_SHD_MAX2;//エリア2の細分化したブロックの最大値
	int		*R_SHD_MIN2;//エリア2の細分化したブロックの最小値
	double	*R_SHD_POS2;//エリア2の細分化したブロックの値
	double	*R_SHD_NEG2;//エリア2の細分化したブロックの値

	double	*R_SHD_AVG3;//エリア3の細分化したブロックの平均値
	int		*R_SHD_MAX3;//エリア3の細分化したブロックの最大値
	int		*R_SHD_MIN3;//エリア3の細分化したブロックの最小値
	double	*R_SHD_POS3;//エリア3の細分化したブロックの値
	double	*R_SHD_NEG3;//エリア3の細分化したブロックの値

	double	Max_R_SHD_AVG1;//エリア1の細分化したブロック全体の平均値の最大
	double	Min_R_SHD_AVG1;//エリア1の細分化したブロック全体の平均値の最小
	double	Rst_R_SHD_AVG1;//((エリア1内平均値の最大 - エリア1内平均値の最小) / エリア1内平均値の最大) × 100%

	double	Max_R_SHD_AVG2;//エリア2の細分化したブロック全体の平均値の最大
	double	Min_R_SHD_AVG2;//エリア2の細分化したブロック全体の平均値の最小
	double	Rst_R_SHD_AVG2;//((エリア2内平均値の最大 - エリア2内平均値の最小) / エリア2内平均値の最大) × 100%

	double	Max_R_SHD_AVG3;//エリア3の細分化したブロック全体の平均値の最大
	double	Min_R_SHD_AVG3;//エリア3の細分化したブロック全体の平均値の最小
	double	Rst_R_SHD_AVG3;//((エリア3内平均値の最大 - エリア3内平均値の最小) / エリア3内平均値の最大) × 100%

//------------------------------------

ITBL CKOP_SHADING_EX::itbl[] = {
	{ 2, IDC_COMBO1, "なし"    },
	{ 2, IDC_COMBO1, "BLUR"    },
	{ 2, IDC_COMBO1, "GAUSSIAN"},
	{ 2, IDC_COMBO1, "MEDIAN"  },
	{ 2, IDC_COMBO2, "なし"    },
	{ 2, IDC_COMBO2, "3x3"     },
	{ 2, IDC_COMBO2, "5x5"     },
	{ 2, IDC_COMBO2, "9x9"     },
	{ 2, IDC_COMBO2, "11x11"   },
	{ 0,          0,    0}
};

//
// シェーデング解析画面定義
//
#if 1//2015.09.10
CTBL CKOP_SHADING_EX::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 5, "LOGO"              ,1110,  30, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
#if 1//2017.04.01
	{ 0, 0  , IDC_STATIC1 , 5, "シェーディング解析",  10,   3, 322, 22,           SS_CENTERIMAGE},
#else
	{ 0, 0  , IDC_STATIC1 , 5, "シェーディング解析",  10,   3, 644, 22,           SS_CENTERIMAGE},
#endif
	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  , 780,   3, 350, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC3 , 5, "フィルター処理"    , 670,  30, 200, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC4 , 5, "ウェイト"          , 900,  30, 200, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_COMBO1  , 5, "Gauss"             , 670,  60, 200, 30},
	{ 0, 0  , IDC_COMBO2  , 5, "3x3"               , 900,  60, 200, 30},
#if 1//2017.04.01
	{ 0, 0  , IDC_STATIC  , 5, "周辺無視範囲"      , 670, 100, 200, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 5, "ゴミ除去"          , 670, 130, 200, 22, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_EDIT1   , 5, "0"                 , 900, 100, 200, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_EDIT1   , 5, "999"                 , 500,   3, 100, 22, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_SPIN1   , 0, (LPCTSTR)(0|10<<16),   0,   0,   0,  0, UDS_SETBUDDYINT|UDS_ALIGNRIGHT},
	{ 0, 0  , IDC_EDIT2   , 5, "0"                 , 900, 130, 200, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_SPIN2   , 0, (LPCTSTR)(0|255<<16),   0,   0,   0,  0, UDS_SETBUDDYINT|UDS_ALIGNRIGHT},
//	{ 0, 0  , IDC_STATIC5 , 5, "解析結果"          , 670, 170, 100, 30, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC6 , 5, "Ave:"              , 780, 170,  80, 30, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC7 , 5, "Max:"              , 780, 210,  80, 30, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC8 , 5, "Min:"              , 780, 250,  80, 30, WS_BORDER|SS_CENTERIMAGE},
#else
	{ 0, 0  , IDC_STATIC5 , 5, "解析結果"          , 670, 130, 100, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC6 , 5, "Ave:"              , 700, 170, 160, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC7 , 5, "Max:"              , 700, 210, 160, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC8 , 5, "Min:"              , 700, 250, 160, 30, WS_BORDER|SS_CENTERIMAGE},
#endif
//	{ 0, 0  , IDC_STATIC9 , 5, "シェーディング:"   , 700, 290, 160, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC5 , 5, "エリア1"           , 690, 300, 100, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC6 , 5, "エリア2"           , 800, 300, 100, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC7 , 5, "エリア3"           , 910, 300, 100, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC8 , 5, "分割数"            ,1020, 300,  80, 30, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_EDIT7   , 0, ""                  , 690, 340,  50, 30, WS_BORDER},//エリア1 A x
	{ 0, 0  , IDC_EDIT8   , 0, ""                  , 740, 340,  50, 30, WS_BORDER},//エリア1 A y
	{ 0, 0  , IDC_EDIT9   , 0, ""                  , 690, 380,  50, 30, WS_BORDER},//エリア1 B x
	{ 0, 0  , IDC_EDIT10  , 0, ""                  , 740, 380,  50, 30, WS_BORDER},//エリア1 B y

	{ 0, 0  , IDC_EDIT11  , 0, ""                  , 800, 340,  50, 30, WS_BORDER},//エリア2 A x
	{ 0, 0  , IDC_EDIT12  , 0, ""                  , 850, 340,  50, 30, WS_BORDER},//エリア2 A y
	{ 0, 0  , IDC_EDIT13  , 0, ""                  , 800, 380,  50, 30, WS_BORDER},//エリア2 B x
	{ 0, 0  , IDC_EDIT14  , 0, ""                  , 850, 380,  50, 30, WS_BORDER},//エリア2 B y

	{ 0, 0  , IDC_EDIT15  , 0, ""                  , 910, 340,  50, 30, WS_BORDER},//エリア3 A x
	{ 0, 0  , IDC_EDIT16  , 0, ""                  , 960, 340,  50, 30, WS_BORDER},//エリア3 A y
	{ 0, 0  , IDC_EDIT17  , 0, ""                  , 910, 380,  50, 30, WS_BORDER},//エリア3 B x
	{ 0, 0  , IDC_EDIT18  , 0, ""                  , 960, 380,  50, 30, WS_BORDER},//エリア3 B y
	{ 0, 0  , IDC_EDIT20  , 0, ""                  ,1020, 340,  70, 30, WS_BORDER},//分割数

	{ 0, 0  , IDC_STATIC19, 5, "198"               , 770, 190, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア1の最大平均値
	{ 0, 0  , IDC_STATIC20, 5, "198"               , 880, 190, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア1の最小平均値
	{ 0, 0  , IDC_STATIC21, 5, "198"               , 990, 190, 140, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア1の計算値

	{ 0, 0  , IDC_STATIC22, 5, "198"               , 770, 220, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア2の最大平均値
	{ 0, 0  , IDC_STATIC23, 5, "198"               , 880, 220, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア2の最小平均値
	{ 0, 0  , IDC_STATIC27, 5, "198"               , 990, 220, 140, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア2の計算値

	{ 0, 0  , IDC_STATIC28, 5, "198"               , 770, 250, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア3の最大平均値
	{ 0, 0  , IDC_STATIC29, 5, "198"               , 880, 250, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア3の最小平均値
	{ 0, 0  , IDC_STATIC30, 5, "198"               , 990, 250, 140, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},//エリア3の計算値

	{ 0, 0  , IDC_STATIC31, 1, "エリア1"           , 670, 190,  60, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
	{ 0, 0  , IDC_STATIC32, 1, "エリア2"           , 670, 220,  60, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
	{ 0, 0  , IDC_STATIC33, 1, "エリア3"           , 670, 250,  60, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},

	{ 0, 0  , IDC_STATIC38, 1, "最大平均値"        , 770, 162, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
	{ 0, 0  , IDC_STATIC39, 1, "最小平均値"        , 880, 162, 100, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
	{ 0, 0  , IDC_STATIC40, 1, "(最大-最小)/最大"  , 990, 162, 140, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},

//	{ 0, 0  , IDC_STATIC19, 5, "198.34"            , 870, 170,  90, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
//	{ 0, 0  , IDC_STATIC20, 5, "220"               , 870, 210,  90, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
//	{ 0, 0  , IDC_STATIC21, 5, "188"               , 870, 250,  90, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
//	{ 0, 0  , IDC_STATIC22, 5, "+ 10.92%"          , 870, 290,  90, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},
//	{ 0, 0  , IDC_STATIC23, 5, "-  5.21%"          , 870, 330,  90, 30, WS_BORDER|SS_CENTERIMAGE|SS_CENTER},

//	{ 0, 0  , IDC_STATIC10, 1, "Ave:"              , 840, 390,  70, 20, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC2 , 1, "Max:"              , 840, 410,  70, 20, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC12, 1, "Min:"              , 840, 430,  70, 20, WS_BORDER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC13, 6, "シェーディング:"   , 840, 450,  70, 20, WS_BORDER|SS_CENTERIMAGE},

//	{ 0, 0  , IDC_STATIC27, 1, "緑線"              , 920, 370,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC28, 1, "198.34"            , 920, 390,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC29, 1, "220"               , 920, 410,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC30, 1, "188"               , 920, 430,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC14, 1, "XXX"               , 920, 450,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC15, 1, "YYY"               , 920, 470,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

//	{ 0, 0  , IDC_STATIC31, 1, "赤線"              ,1000, 370,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC32, 1, "195.05"            ,1000, 390,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC33, 1, "220"               ,1000, 410,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC34, 1, "188"               ,1000, 430,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC16, 1, "XXX"               ,1000, 450,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
//	{ 0, 0  , IDC_STATIC17, 1, "YYY"               ,1000, 470,  80, 20, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC24, 0, "CMAP1"             , 670, 297,  25,130, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 8  , IDC_STATIC25, 0, "CMAP2"             , 670, 297,  25,130, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 8  , IDC_STATIC35, 0, "生画像"            , 680, 430, 140, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 8  , IDC_STATIC36, 0, "対角"              , 680, 470, 140, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
#if 1//2017.04.01
	{ 0, 0  , IDC_CHECK2  , 7, "シェーディング\r補正", 1150, 170, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_BUTTON10, 0, "補正画像\rWHITE取込",1150, 230, 140, 50},
#else
	{ 0, 0  , IDC_CHECK2  , 7, "シェーディング補正", 990, 150, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_BUTTON10, 0, "補正画像\rWHITE取込",990, 210, 140, 50},
#endif
//	{ 0, 0  , IDC_BUTTON11, 0, "補正画像\rBLACK取込",990, 270, 140, 50},

	{ 0, 0  , IDC_BUTTON7 , 0, "保存"              , 990, 500, 140, 50},
	{ 0, 0  , IDC_BUTTON8 , 0, "戻る"              , 990, 560, 140, 50},
	{ 0, 8  , IDC_STATIC37, 0, "GRAPH"             ,  10, 518, 810, 90, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC  , 1, "保存ファイル名"    , 685, 610, 140, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_EDIT19  , 0, ""                  , 685, 630, 250, 30, WS_BORDER},
	{ 0, 0  ,            0, 0, NULL}
};
#else
#endif
#if 1//2017.04.01
static
void CHECK_FIXED(CWnd *pWndForm, int *pFILTER, int *pPARAM, int *pCOLMAP, int *pDIAGO)
{
	CArray<UINT, UINT> ar;

	if (CKOP::FP.SHADING_FILTER >= 0) {
		*pFILTER = CKOP::FP.SHADING_FILTER;
		ar.Add(IDC_COMBO1);
	}
	if (CKOP::FP.SHADING_PARAM >= 0) {
		*pPARAM = CKOP::FP.SHADING_PARAM;
		ar.Add(IDC_COMBO2);
	}
	if (CKOP::FP.SHADING_COLMAP >= 0) {
		*pCOLMAP = CKOP::FP.SHADING_COLMAP;
		ar.Add(IDC_STATIC35);
	}
	if (CKOP::FP.SHADING_DIAGO >= 0) {
		*pDIAGO = CKOP::FP.SHADING_DIAGO;
		ar.Add(IDC_STATIC36);
	}
	if (CKOP::FP.SHADING_SKIPCNT >= 0) {
		m_d.P_SKIPCNT = CKOP::FP.SHADING_SKIPCNT;
		ar.Add(IDC_EDIT1);
	}
	if (CKOP::FP.SHADING_GABVAL >= 0) {
		m_d.P_GABVAL = CKOP::FP.SHADING_GABVAL;
		ar.Add(IDC_EDIT2);
	}
	if (CKOP::FP.SHADING_SHAD_CORR >= 0) {
		m_d.P_SHAD_CORR = CKOP::FP.SHADING_SHAD_CORR;
		ar.Add(IDC_CHECK2);
	}
	for (int i = 0; i < ar.GetCount(); i++) {
		pWndForm->GetDlgItem(ar[i])->EnableWindow(FALSE);
	}
}
#endif
void CKOP_SHADING_EX::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	int	n1 = GetProfileINT("SHADING", "FILTER", 1);
	int	n2 = GetProfileINT("SHADING", "PARAM" , 0);
	int	n3 = GetProfileINT("SHADING", "COLMAP", 0);
	int	n4 = GetProfileINT("SHADING", "DIAGO" , 1);
#if 1//2016.01.01
	m_d.P_SKIPCNT = G_SS.SHD_SKIPCNT;
#else
	m_d.P_SKIPCNT = GetProfileINT("SHADING", "SKIPCNT" , 2);
#endif
#if 1//2017.04.01
	m_d.P_SKIPCNT = GetProfileINT("SHADING", "SKIPCNT" , 0);
	m_d.P_GABVAL  = GetProfileINT("SHADING", "GABVAL" , 0);
#endif
#if 1//2015.09.10
	m_d.P_SHAD_CORR = GetProfileINT("SHADING", "SHAD_CORR" , 0);
#if 1//2017.04.01
	if (G_SS.USE_FIXED) {
		CKOP::GetProfileFixed();
		CHECK_FIXED(pWndForm, &n1, &n2, &n3, &n4);
	}
#endif
	if (!IS_AVAILABLE_CORRECTION()) {
		 m_d.P_SHAD_CORR = 0;
		 pWndForm->GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	}
	pWndForm->CheckDlgButton(IDC_CHECK2, m_d.P_SHAD_CORR!=0);
#endif
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->SetCurSel(n1);
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO2))->SetCurSel(n2);
	switch (n3) {
	case  1:pWndForm->SetDlgItemText(IDC_STATIC35, "カラーMAP1");break;
	case  2:pWndForm->SetDlgItemText(IDC_STATIC35, "カラーMAP2");break;
#if 1//2017.04.01
	case  3:pWndForm->SetDlgItemText(IDC_STATIC35, "生画像+ゴミ除去");break;
#endif
	case  0:
	default:pWndForm->SetDlgItemText(IDC_STATIC35, "生画像");break;
	}
	switch (n4) {
	case  1:pWndForm->SetDlgItemText(IDC_STATIC36, "対角");break;
	case  0:
	default:pWndForm->SetDlgItemText(IDC_STATIC36, "ＸＹ");break;
	}
	CKOP::P_COLMAP = m_d.P_COLMAP = n3;
#if 1//2017.04.01
	pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_SKIPCNT);
	pWndForm->SetDlgItemInt(IDC_EDIT2, m_d.P_GABVAL);
#endif
	for (int i = 0; i < 256; i++) {
		int		h = (int)(359 - i/255.0 * 359);
		m_G2CTBL[i] = HSV2RGB(h, 255, 255);
#if 1//2015.07.24
		CKOP::RGB_TBL1[i].rgbBlue  = ((m_G2CTBL[i] >> 0) & 0xff);
		CKOP::RGB_TBL1[i].rgbGreen = ((m_G2CTBL[i] >> 8) & 0xff);
		CKOP::RGB_TBL1[i].rgbRed   = ((m_G2CTBL[i] >>16) & 0xff);
#endif
	}
#if 0
	CWnd *p = pWndForm->GetDlgItem(IDC_BUTTON40);
	if (p != NULL) {
		p->MoveWindow(840, 560, 140, 50);
	}
#endif
	m_pLineBuf[0] = (LPBYTE)malloc(CKOP::BMP_WID);
	m_pLineBuf[1] = (LPBYTE)malloc(CKOP::BMP_WID);

	CKOP::GDEF_INIT(&m_gdef, /*bitLBL=*/0+2, pWndForm->GetDlgItem(IDC_STATIC37));
	CKOP::GDEF_PSET(&m_gdef, 0, CKOP::BMP_WID-1, CKOP::BMP_WID/8, 0, 255, 64);

//サイズ確保 start
	double taikakusen = sqrt((int)(CKOP::BMP_WID * CKOP::BMP_WID) + (int)(CKOP::BMP_HEI * CKOP::BMP_HEI));
	int bufsize = (int)taikakusen + 100;
	if(R_SHD_AVG1 == NULL)
	{
		R_SHD_AVG1 = new double [bufsize];
		memset(R_SHD_AVG1, 0, bufsize * sizeof(double));
	}
	if(R_SHD_MAX1 == NULL)
	{
		R_SHD_MAX1 = new int [bufsize];
		memset(R_SHD_MAX1, 0, bufsize * sizeof(int));
	}
	if(R_SHD_MIN1 == NULL)
	{
		R_SHD_MIN1 = new int [bufsize];
		memset(R_SHD_MIN1, 0, bufsize * sizeof(int));
	}
	if(R_SHD_POS1 == NULL)
	{
		R_SHD_POS1 = new double [bufsize];
		memset(R_SHD_POS1, 0, bufsize * sizeof(double));
	}
	if(R_SHD_NEG1 == NULL)
	{
		R_SHD_NEG1 = new double [bufsize];
		memset(R_SHD_NEG1, 0, bufsize * sizeof(double));
	}

	if(R_SHD_AVG2 == NULL)
	{
		R_SHD_AVG2 = new double [bufsize];
		memset(R_SHD_AVG2, 0, bufsize * sizeof(double));
	}
	if(R_SHD_MAX2 == NULL)
	{
		R_SHD_MAX2 = new int [bufsize];
		memset(R_SHD_MAX2, 0, bufsize * sizeof(int));
	}
	if(R_SHD_MIN2 == NULL)
	{
		R_SHD_MIN2 = new int [bufsize];
		memset(R_SHD_MIN2, 0, bufsize * sizeof(int));
	}
	if(R_SHD_POS2 == NULL)
	{
		R_SHD_POS2 = new double [bufsize];
		memset(R_SHD_POS2, 0, bufsize * sizeof(double));
	}
	if(R_SHD_NEG2 == NULL)
	{
		R_SHD_NEG2 = new double [bufsize];
		memset(R_SHD_NEG2, 0, bufsize * sizeof(double));
	}

	if(R_SHD_AVG3 == NULL)
	{
		R_SHD_AVG3 = new double [bufsize];
		memset(R_SHD_AVG3, 0, bufsize * sizeof(double));
	}
	if(R_SHD_MAX3 == NULL)
	{
		R_SHD_MAX3 = new int [bufsize];
		memset(R_SHD_MAX3, 0, bufsize * sizeof(int));
	}
	if(R_SHD_MIN3 == NULL)
	{
		R_SHD_MIN3 = new int [bufsize];
		memset(R_SHD_MIN3, 0, bufsize * sizeof(int));
	}
	if(R_SHD_POS3 == NULL)
	{
		R_SHD_POS3 = new double [bufsize];
		memset(R_SHD_POS3, 0, bufsize * sizeof(double));
	}
	if(R_SHD_NEG3 == NULL)
	{
		R_SHD_NEG3 = new double [bufsize];
		memset(R_SHD_NEG3, 0, bufsize * sizeof(double));
	}
//サイズ確保 end

	HBITMAP	hBMP;
	
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_DEFAULTSIZE);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC24))->SetBitmap(hBMP);
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_DEFAULTSIZE);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC25))->SetBitmap(hBMP);

	switch (n3) {
	case  1:
		pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_SHOW);
		pWndForm->GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
	break;
	case  2:
		pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
		pWndForm->GetDlgItem(IDC_STATIC25)->ShowWindow(SW_SHOW);
	break;
	case  0:
	default:
		pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
		pWndForm->GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
	break;
	}
	
	pWndForm->Invalidate();
	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}

	a1x1 = GetProfileINT("SHADING", "AREA1_X1" , 0);
	a1y1 = GetProfileINT("SHADING", "AREA1_Y1" , 0);
	a1x2 = GetProfileINT("SHADING", "AREA1_X2" , 0);
	a1y2 = GetProfileINT("SHADING", "AREA1_Y2" , 0);
	a2x1 = GetProfileINT("SHADING", "AREA2_X1" , 0);
	a2y1 = GetProfileINT("SHADING", "AREA2_Y1" , 0);
	a2x2 = GetProfileINT("SHADING", "AREA2_X2" , 0);
	a2y2 = GetProfileINT("SHADING", "AREA2_Y2" , 0);
	a3x1 = GetProfileINT("SHADING", "AREA3_X1" , 0);
	a3y1 = GetProfileINT("SHADING", "AREA3_Y1" , 0);
	a3x2 = GetProfileINT("SHADING", "AREA3_X2" , 0);
	a3y2 = GetProfileINT("SHADING", "AREA3_Y2" , 0);

//初期、もしiniファイル上０の場合
	if(a1x1 == 0) a1x1 = 1;
	if(a1y1 == 0) a1y1 = 10;
	if(a1x2 == 0) a1x2 = CKOP::BMP_WID - 1;
	if(a1y2 == 0) a1y2 = 20;
	if(a2x1 == 0) a2x1 = 1;
	if(a2y1 == 0) a2y1 = CKOP::BMP_HEI / 2;
	if(a2x2 == 0) a2x2 = CKOP::BMP_WID - 1;
	if(a2y2 == 0) a2y2 = CKOP::BMP_HEI / 2 + 10;
	if(a3x1 == 0) a3x1 = 1;
	if(a3y1 == 0) a3y1 = CKOP::BMP_HEI - 20;
	if(a3x2 == 0) a3x2 = CKOP::BMP_WID - 1;
	if(a3y2 == 0) a3y2 = CKOP::BMP_HEI - 10;

	pWndForm->SetDlgItemInt(IDC_EDIT7,  a1x1);
	pWndForm->SetDlgItemInt(IDC_EDIT8,  a1y1);
	pWndForm->SetDlgItemInt(IDC_EDIT9,  a1x2);
	pWndForm->SetDlgItemInt(IDC_EDIT10, a1y2);
	pWndForm->SetDlgItemInt(IDC_EDIT11, a2x1);
	pWndForm->SetDlgItemInt(IDC_EDIT12, a2y1);
	pWndForm->SetDlgItemInt(IDC_EDIT13, a2x2);
	pWndForm->SetDlgItemInt(IDC_EDIT14, a2y2);
	pWndForm->SetDlgItemInt(IDC_EDIT15, a3x1);
	pWndForm->SetDlgItemInt(IDC_EDIT16, a3y1);
	pWndForm->SetDlgItemInt(IDC_EDIT17, a3x2);
	pWndForm->SetDlgItemInt(IDC_EDIT18, a3y2);

	//初期値(各エリア座標)
	if(n4 == 0)
	{
		m_d.P_DIAGO = 0;
		pWndForm->GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT9)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT10)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT11)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT12)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT13)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT14)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT15)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT16)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT17)->EnableWindow(TRUE);
		pWndForm->GetDlgItem(IDC_EDIT18)->EnableWindow(TRUE);

		pWndForm->GetDlgItem(IDC_STATIC31)->SetWindowText("エリア1");
		pWndForm->GetDlgItem(IDC_STATIC32)->SetWindowText("エリア2");
		pWndForm->GetDlgItem(IDC_STATIC33)->SetWindowText("エリア3");
	}
	else
	{
		m_d.P_DIAGO = 1;
		pWndForm->GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT9)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT10)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT11)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT12)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT13)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT14)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT15)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT17)->EnableWindow(FALSE);
		pWndForm->GetDlgItem(IDC_EDIT18)->EnableWindow(FALSE);

		pWndForm->GetDlgItem(IDC_STATIC31)->SetWindowText("緑線");
		pWndForm->GetDlgItem(IDC_STATIC32)->SetWindowText("赤線");
		pWndForm->GetDlgItem(IDC_STATIC33)->SetWindowText("");
	}

	//初期値(分割数)
	subCount = GetProfileINT("SHADING", "SUBDIVIDE_COUNT" , 20);
	if(subCount == 0) subCount = 20;
	pWndForm->SetDlgItemInt(IDC_EDIT20, subCount);

}

void CKOP_SHADING_EX::TERM_FORM(void)
{
	free(m_pLineBuf[0]);
	free(m_pLineBuf[1]);
	m_pLineBuf[0] = NULL;
	m_pLineBuf[1] = NULL;

	if(R_SHD_AVG1 != NULL)
	{
		delete [] R_SHD_AVG1;
		R_SHD_AVG1 = NULL;
	}
	if(R_SHD_MAX1 != NULL)
	{
		delete [] R_SHD_MAX1;
		R_SHD_MAX1 = NULL;
	}
	if(R_SHD_MIN1 != NULL)
	{
		delete [] R_SHD_MIN1;
		R_SHD_MIN1 = NULL;
	}
	if(R_SHD_POS1 != NULL)
	{
		delete [] R_SHD_POS1;
		R_SHD_POS1 = NULL;
	}
	if(R_SHD_NEG1 != NULL)
	{
		delete [] R_SHD_NEG1;
		R_SHD_NEG1 = NULL;
	}

	if(R_SHD_AVG2 != NULL)
	{
		delete [] R_SHD_AVG2;
		R_SHD_AVG2 = NULL;
	}
	if(R_SHD_MAX2 != NULL)
	{
		delete [] R_SHD_MAX2;
		R_SHD_MAX2 = NULL;
	}
	if(R_SHD_MIN2 != NULL)
	{
		delete [] R_SHD_MIN2;
		R_SHD_MIN2 = NULL;
	}
	if(R_SHD_POS2 != NULL)
	{
		delete [] R_SHD_POS2;
		R_SHD_POS2 = NULL;
	}
	if(R_SHD_NEG2 != NULL)
	{
		delete [] R_SHD_NEG2;
		R_SHD_NEG2 = NULL;
	}

	if(R_SHD_AVG3 != NULL)
	{
		delete [] R_SHD_AVG3;
		R_SHD_AVG3 = NULL;
	}
	if(R_SHD_MAX3 != NULL)
	{
		delete [] R_SHD_MAX3;
		R_SHD_MAX3 = NULL;
	}
	if(R_SHD_MIN3 != NULL)
	{
		delete [] R_SHD_MIN3;
		R_SHD_MIN3 = NULL;
	}
	if(R_SHD_POS3 != NULL)
	{
		delete [] R_SHD_POS3;
		R_SHD_POS3 = NULL;
	}
	if(R_SHD_NEG3 != NULL)
	{
		delete [] R_SHD_NEG3;
		R_SHD_NEG3 = NULL;
	}

	WriteProfileINT("SHADING", "FILTER", m_d.P_FILTER);
	WriteProfileINT("SHADING", "PARAM" , m_d.P_PARAM);
	WriteProfileINT("SHADING", "COLMAP", m_d.P_COLMAP);
	WriteProfileINT("SHADING", "DIAGO" , m_d.P_DIAGO);
#if 1//2015.09.10
	WriteProfileINT("SHADING", "SHAD_CORR" , m_d.P_SHAD_CORR);
#endif
#if 1//2017.04.01
	WriteProfileINT("SHADING", "SKIPCNT" , m_d.P_SKIPCNT);
	WriteProfileINT("SHADING", "GABVAL" , m_d.P_GABVAL);
#endif

	WriteProfileINT("SHADING", "AREA1_X1" , a1x1);
	WriteProfileINT("SHADING", "AREA1_Y1" , a1y1);
	WriteProfileINT("SHADING", "AREA1_X2" , a1x2);
	WriteProfileINT("SHADING", "AREA1_Y2" , a1y2);
	WriteProfileINT("SHADING", "AREA2_X1" , a2x1);
	WriteProfileINT("SHADING", "AREA2_Y1" , a2y1);
	WriteProfileINT("SHADING", "AREA2_X2" , a2x2);
	WriteProfileINT("SHADING", "AREA2_Y2" , a2y2);
	WriteProfileINT("SHADING", "AREA3_X1" , a3x1);
	WriteProfileINT("SHADING", "AREA3_Y1" , a3y1);
	WriteProfileINT("SHADING", "AREA3_X2" , a3x2);
	WriteProfileINT("SHADING", "AREA3_Y2" , a3y2);

	WriteProfileINT("SHADING", "SUBDIVIDE_COUNT" , subCount);
}
#if 1//2015.09.10
BOOL LOAD_CORR_IMAGE_EX(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP)
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

	if (BPP != 1 && BPP != 3) {
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
void SAVE_CORR_IMAGE_EX(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP)
{
	int		cnt = width*height;
	int		size = cnt*BPP;
#if 1
	BITMAPINFO bmpInfo;
    LPBYTE lpPixel;
    //DIBの情報を設定する
    bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth=width;
    bmpInfo.bmiHeader.biHeight=height;
    bmpInfo.bmiHeader.biPlanes=1;
    bmpInfo.bmiHeader.biBitCount=24;
    bmpInfo.bmiHeader.biCompression=BI_RGB;
    //DIBSection作成
    HBITMAP hBmp=CreateDIBSection(NULL,&bmpInfo,DIB_RGB_COLORS,(void**)&lpPixel,NULL,0);
	if (BPP == 1) {
		LPBYTE	p, po = (LPBYTE)lpPixel;
		if (po == NULL) {
			return;
		}
		p = po;
		for (int i = 0; i < cnt; i++) {
			*p++ = *pBuf;
			*p++ = *pBuf;
			*p++ = *pBuf++;
		}
	}
	else if (BPP == 3) {
		memcpy(lpPixel, pBuf, size);
	}
/*	HBITMAP	h = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_DEFAULTSIZE);
	CKOP::SAVE_BITMAP(h, "c:\\test00.bmp");
	CKOP::SAVE_BITMAP(h, "c:\\temp\\test00.bmp");
	CKOP::SAVE_BITMAP(hBmp, "c:\\temp\\test00.bmp");
*/
	CKOP::SAVE_BITMAP(hBmp, pszFileName);

    DeleteObject(hBmp);
#else
	CBitmap	bmp;

	bmp.CreateBitmap(width, height, 1, 24, NULL);
	if (BPP == 1) {
		LPBYTE	p, po = (LPBYTE)malloc(size);
		if (po == NULL) {
			return;
		}
		p = po;
		for (int i = 0; i < cnt; i++) {
			*p++ = *pBuf;
			*p++ = *pBuf;
			*p++ = *pBuf++;
		}
		bmp.SetBitmapBits(size, po);
		free(po);
	}
	else if (BPP == 3) {
		bmp.SetBitmapBits(size, pBuf);
	}
	else {
		return;
	}
	CKOP::SAVE_BITMAP((HBITMAP)bmp, pszFileName);
#endif
}

void IMPORT_CORR_IMAGE_EX(int code, LPBYTE pImgPxl, BOOL bSAVE_IMG)
{
	int		i = code-1;
	int		size;

	if (code != 1 && code != 2) {
		return;
	}

	size = CKOP::BMP_WID * CKOP::BMP_HEI * CKOP::BMP_BYTE_CNT;

	if (m_nShadSize[i] != size) {
		if (m_pShadBuf[i] != NULL) {
			free(m_pShadBuf[i]);
			m_pShadBuf[i] = NULL;
		}
	}
	if (m_pShadBuf[i] == NULL) {
		if ((m_pShadBuf[i] = (LPBYTE)malloc(size)) == NULL) {
			return;
		}
		m_nShadSize[i] = size;
	}
	memcpy(m_pShadBuf[i], pImgPxl, size);
	if (bSAVE_IMG) {
		if (code == 1) {
			TCHAR	path[MAX_PATH];
			lstrcpyn(path, GetDirDoc(), sizeof(path));

			sprintf_s(path, _countof(path), "%s\\%s",
				GetDirDoc(),
				"SHADING_CORRECTION_WHITE.bmp");

			SAVE_CORR_IMAGE_EX(path, pImgPxl, CKOP::BMP_WID, CKOP::BMP_HEI, CKOP::BMP_BYTE_CNT);
	//		SAVE_CORR_IMAGE("c:\\temp\\test00.png", pImgPxl, CKOP::BMP_WID, CKOP::BMP_HEI, CKOP::BMP_BYTE_CNT);
		}
	}
}

BOOL CKOP_SHADING_EX::IS_AVAILABLE_CORRECTION(void)
{
	if (m_pShadBuf[0] == NULL) {
		return(FALSE);
	}
	return(TRUE);
}

void CKOP_SHADING_EX::INIT_CORRECTION(void)
{
	int		cnt = CKOP::BMP_WID * CKOP::BMP_HEI;
	int		size = cnt*CKOP::BMP_BYTE_CNT;
	BOOL	ret;

	if (TRUE) {
		if ((m_pShadBuf[0] = (LPBYTE)malloc(size)) == NULL) {
			return;
		}
	}
	if (TRUE) {
		TCHAR	path[MAX_PATH];
		lstrcpyn(path, GetDirDoc(), sizeof(path));

		sprintf_s(path, _countof(path), "%s\\%s",
			GetDirDoc(),
			"SHADING_CORRECTION_WHITE.bmp");

		ret = LOAD_CORR_IMAGE_EX(path, m_pShadBuf[0], CKOP::BMP_WID, CKOP::BMP_HEI, CKOP::BMP_BYTE_CNT);
	}
	if (ret == FALSE) {
		TERM_CORRECTION();
	}
	m_nShadSize[0] = size;
	m_nShadBPP[0]  = CKOP::BMP_BYTE_CNT;
}
void CKOP_SHADING_EX::TERM_CORRECTION(void)
{
	if (m_pShadBuf[0] != NULL) {
		free(m_pShadBuf[0]);
		m_pShadBuf[0] = NULL;
	}
	if (m_pShadBuf[1] != NULL) {
		free(m_pShadBuf[1]);
		m_pShadBuf[1] = NULL;
	}
}
void CKOP_SHADING_EX::DO_CORRECTION(LPBYTE pImgPxl)
{
	LPBYTE	pw = m_pShadBuf[0];
	LPBYTE	pi = pImgPxl;
#if 0
	BYTE	bk = 0;
	LPBYTE	pb = &bk;
#endif
	int		n;
	int		size = m_nShadSize[0];
	int		d;

	for (int i = 0; i < size; i++) {
#if 1
		d = *pw;
#else
		d = (*pw - *pb);
#endif
		if (d <= 0) {
			n = 255;
		}
		else {
#if 1
			n = 255 * (*pi      ) / d;
#else
			n = 255 * (*pi - *pb) / d;
#endif
			//if (n < 0) {
			//	n = 0;
			//}
			if (n > 255) {
				n = 255;
			}
		}
		*pi++ = n;
		pw++;
	}
}
#endif
/********************************************************************/
COLORREF CKOP_SHADING_EX::HSV2RGB(int h, int s, int v)
{
	//PseudoColor
	int		r,g,b;
	double	phase = h/360.0;
	double	PI = 3.141592;
	double	shift=0;
	while (phase < 0) {
		phase += 1.0;
	}
	while (phase >= 1.0) {
		phase -= 1.0;
	}
	shift += PI+PI/4;     //青から赤に
	r = (int)(255*(sin(1.5*PI*phase + shift + PI ) + 1)/2.0 );
	g = (int)(255*(sin(1.5*PI*phase + shift + PI/2 ) + 1)/2.0 );
	b = (int)(255*(sin(1.5*PI*phase + shift  ) + 1)/2.0 );
//	return (RGB(r,g,b));
	return(r<<16|g<<8|b);
}


void CKOP_SHADING_EX::DO_FILTER(CWnd *pWndForm, LPBYTE pImgPxl, int nFilType, int nFilPara)
{

	int			n;

	switch (nFilPara) {
	case  1:n = 3; break;//3x3
	case  2:n = 5; break;//5x5
	case  3:n = 9; break;//9x9
	case  4:n =11; break;//11x11
	case  0:
	default:n = 0; break;//なし
	}

	if (nFilType >= 1 && nFilType <= 3 && n > 0) {
		IplImage *img_src = cvCreateImage(cvSize(CKOP::BMP_WID, CKOP::BMP_HEI), IPL_DEPTH_8U, CKOP::BMP_BYTE_CNT);
		IplImage *img_dst = cvCreateImage(cvSize(CKOP::BMP_WID, CKOP::BMP_HEI), IPL_DEPTH_8U, 1);
		try {
#if 1//2015.07.24
			if (CKOP::BMP_BYTE_CNT == 1) {
			//8bitカラーのときはグレースケールで格納されていると想定する
			memcpy(img_dst->imageData, pImgPxl, img_src->imageSize);
			}
			else {
#endif
			memcpy(img_src->imageData, pImgPxl, img_src->imageSize);
			cvCvtColor(img_src, img_dst, CV_BGR2GRAY);
#if 1//2015.07.24
			}
#endif
			switch (nFilType) {
			case 1://BLUR
				cvSmooth(img_dst, img_dst, CV_BLUR    , n, n, 0, 0);
			break;
			case 2://GAUSSIAN
				cvSmooth(img_dst, img_dst, CV_GAUSSIAN, n, n, 0, 0);
			break;
			case 3://MEDIAN
				cvSmooth(img_dst, img_dst, CV_MEDIAN  , n, 0, 0, 0);
			break;
			}			
#if 1//2015.07.24
			if (CKOP::BMP_BYTE_CNT == 1) {
			//8bitカラーのときはグレースケールで格納されていると想定する
			memcpy(pImgPxl, img_dst->imageData, img_dst->imageSize);
			}
			else {
#endif
			cvCvtColor(img_dst, img_src, CV_GRAY2BGR);
			memcpy(pImgPxl, img_src->imageData, img_src->imageSize);
#if 1//2015.07.24
			}
#endif
		}
		catch (cv::Exception ex) {
			CKOP::STOP();
			mlog("%s", ex.msg.c_str());
		}
		cvReleaseImage(&img_dst);
		cvReleaseImage(&img_src);
	}
}

void CALC_SHADING_CROSS(LPBYTE pImgPxl, CWnd *pWndForm)
{
	pWndForm->GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT9)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT10)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT11)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT12)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT13)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT14)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT15)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT17)->EnableWindow(FALSE);
	pWndForm->GetDlgItem(IDC_EDIT18)->EnableWindow(FALSE);

	pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText("");
	pWndForm->GetDlgItem(IDC_STATIC29)->SetWindowText("");
	pWndForm->GetDlgItem(IDC_STATIC30)->SetWindowText("");

	pWndForm->GetDlgItem(IDC_STATIC31)->SetWindowText("緑線");
	pWndForm->GetDlgItem(IDC_STATIC32)->SetWindowText("赤線");
	pWndForm->GetDlgItem(IDC_STATIC33)->SetWindowText("");

	int		ret;
	int		max, min;
	double	ttl, avg;
	int		cnt;
	int		y1, y2, x1, x2;

	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;

	y1 = m_d.P_SKIPCNT;
	y2 = CKOP::BMP_HEI - m_d.P_SKIPCNT;
	x1 = m_d.P_SKIPCNT;
	x2 = CKOP::BMP_WID - m_d.P_SKIPCNT;

	subCount = pWndForm->GetDlgItemInt(IDC_EDIT20);
	if(subCount == 0)
	{
		subCount = 1;//0除算回避
		pWndForm->SetDlgItemInt(IDC_EDIT20, subCount);
	}
	else if(subCount > CKOP::BMP_WID)
	{
		subCount = CKOP::BMP_WID;
		pWndForm->SetDlgItemInt(IDC_EDIT20, subCount);
	}

	int subx = x2 / subCount;
	int subNum = 0;

//対角:(0,0) -> (BMP_WID,BMP_HEI);　緑線
	while(subNum < subCount)
	{
		int upperlimit = 0;
		if(subNum != (subCount - 1))//最後以外
		{
			upperlimit = x1 + subx * (subNum + 1);
		}
		else
		{
			upperlimit = x2;
		}

		for(int x = (x1 + subx * subNum); x <= upperlimit; x++)
		{
			int	y = (int)AXpB(0, CKOP::BMP_WID-1, 0, CKOP::BMP_HEI-1, x);
			if (y < y1 || y >= y2) {
				continue;
			}
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);

			if (ret < m_d.P_GABVAL) {
				continue;//ゴミ判定(計算から除外)
			}

			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
			cnt++;
		}
		avg = ttl / cnt;
		R_SHD_MAX1[subNum] = max;
		R_SHD_MIN1[subNum] = min;
		R_SHD_AVG1[subNum] = avg;
		R_SHD_POS1[subNum] = (max-avg)/avg * 100;
		R_SHD_NEG1[subNum] = (min-avg)/avg * 100;

		subNum++;
	}

	//全ブロックの平均値から最大値と最小値を求める
	int avg_max = -1;
	int avg_min = 256;
	for(int i = 0; i < subNum; i++)
	{
		if(avg_max < R_SHD_AVG1[i])
		{
			avg_max = R_SHD_AVG1[i];
		}
		if(avg_min > R_SHD_AVG1[i])
		{
			avg_min = R_SHD_AVG1[i];
		}
	}
	Max_R_SHD_AVG1 = avg_max;
	Min_R_SHD_AVG1 = avg_min;
	double sabun = (double)(avg_max - avg_min);
	double sho = (double)(sabun / (double)avg_max);
	Rst_R_SHD_AVG1 = (double)(sho * 100);


//対角:(0,BMP_HEI) -> (BMP_WID,0);　赤線
	subNum = 0;
	cnt = 0;
	ttl = 0.0;
	while(subNum < subCount)
	{
		int upperlimit = 0;
		if(subNum != (subCount - 1))//最後以外
		{
			upperlimit = x1 + subx * (subNum + 1);
		}
		else
		{
			upperlimit = x2;
		}

		for(int x = (x1 + subx * subNum); x <= upperlimit; x++)
		{
			int	y = (int)AXpB(0, CKOP::BMP_WID-1, CKOP::BMP_HEI-1, 0, x);
			if (y < y1 || y >= y2) {
				continue;
			}
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);

			if (ret < m_d.P_GABVAL) {
				continue;//ゴミ判定(計算から除外)
			}

			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
			cnt++;
		}

		avg = ttl / cnt;
		R_SHD_MAX2[subNum] = max;
		R_SHD_MIN2[subNum] = min;
		R_SHD_AVG2[subNum] = avg;
		R_SHD_POS2[subNum] = (max-avg)/avg * 100;
		R_SHD_NEG2[subNum] = (min-avg)/avg * 100;

		subNum++;
	}
	//全ブロックの平均値から最大値と最小値を求める
	avg_max = -1;
	avg_min = 256;
	for(int i = 0; i < subNum; i++)
	{
		if(avg_max < R_SHD_AVG2[i])
		{
			avg_max = R_SHD_AVG2[i];
		}
		if(avg_min > R_SHD_AVG2[i])
		{
			avg_min = R_SHD_AVG2[i];
		}
	}
	Max_R_SHD_AVG2 = avg_max;
	Min_R_SHD_AVG2 = avg_min;
	sabun = (double)(avg_max - avg_min);
	sho = (double)(sabun / (double)avg_max);
	Rst_R_SHD_AVG2 = (double)(sho * 100);

}

void CALC_SHADING_HORIZON(LPBYTE pImgPxl, CWnd *pWndForm)
{
//エリア1のシェーディング計算
	pWndForm->GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT9)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT10)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT11)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT12)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT13)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT14)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT15)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT16)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT17)->EnableWindow(TRUE);
	pWndForm->GetDlgItem(IDC_EDIT18)->EnableWindow(TRUE);

	pWndForm->GetDlgItem(IDC_STATIC31)->SetWindowText("エリア1");
	pWndForm->GetDlgItem(IDC_STATIC32)->SetWindowText("エリア2");
	pWndForm->GetDlgItem(IDC_STATIC33)->SetWindowText("エリア3");

	a1x1 = pWndForm->GetDlgItemInt(IDC_EDIT7);
	a1y1 = pWndForm->GetDlgItemInt(IDC_EDIT8);
	a1x2 = pWndForm->GetDlgItemInt(IDC_EDIT9);
	a1y2 = pWndForm->GetDlgItemInt(IDC_EDIT10);

	if(a1x1 > CKOP::BMP_WID - 1)
	{
		a1x1 = CKOP::BMP_WID - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT7, a1x1);
	}
	if(a1y1 > CKOP::BMP_HEI - 1)
	{
		a1y1 = CKOP::BMP_HEI - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT8, a1y1);
	}
	if(a1x2 > CKOP::BMP_WID - 1)
	{	
		a1x2 = CKOP::BMP_WID - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT9, a1x2);
	}
	if(a1y2 > CKOP::BMP_HEI - 1)
	{
		a1y2 = CKOP::BMP_HEI - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT10, a1y2);
	}

	if(a1x1 < 1)
	{
		a1x1 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT7, a1x1);
	}
	if(a1y1 < 1)
	{
		a1y1 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT8, a1y1);
	}
	if(a1x2 < 1)
	{
		a1x2 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT9, a1x2);
	}
	if(a1y2 < 1)
	{
		a1y2 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT10, a1y2);
	}

	int		ret;
	int		max, min;
	double	ttl, avg;
	int		cnt;
	int		y1, y2, x1, x2;

	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;

	y1 = a1y1;
	y2 = a1y2 - a1y1;
	x1 = a1x1;
	x2 = a1x2 - a1x1;

	subCount = pWndForm->GetDlgItemInt(IDC_EDIT20);
	if(subCount == 0)
	{
		subCount = 1;//0除算回避
		pWndForm->SetDlgItemInt(IDC_EDIT20, subCount);
	}
	else if(subCount > CKOP::BMP_WID)
	{
		subCount = CKOP::BMP_WID;
		pWndForm->SetDlgItemInt(IDC_EDIT20, subCount);
	}

	int subx = x2 / subCount;
	int subNum = 0;
	while(subNum < subCount)
	{
		for(int y = y1; y <= (y1 + y2); y++)//細切れ単位で解析
		{
			int upperlimit = 0;
			if(subNum != (subCount - 1))//最後以外
			{
				upperlimit = x1 + subx * (subNum + 1);
			}
			else
			{
				upperlimit = a1x2;
			}

			for(int x = (x1 + subx * subNum); x <= upperlimit; x++)
			{
				ret = CKOP::GET_PIXEL(x, y, pImgPxl);
				if (ret < m_d.P_GABVAL) {
					continue;//ゴミ判定(計算から除外)
				}

				if (max < ret) {
					max = ret;
				}
				if (min > ret) {
					min = ret;
				}
				ttl += ret;
				cnt++;
			}
		}

		avg = ttl / cnt;
		R_SHD_MAX1[subNum] = max;
		R_SHD_MIN1[subNum] = min;
		R_SHD_AVG1[subNum] = avg;
		R_SHD_POS1[subNum] = (max-avg)/avg * 100;
		R_SHD_NEG1[subNum] = (min-avg)/avg * 100;

		subNum++;
	}

	//全ブロックの平均値から最大値と最小値を求める
	int avg_max = -1;
	int avg_min = 256;
	for(int i = 0; i < subNum; i++)
	{
		if(avg_max < R_SHD_AVG1[i])
		{
			avg_max = R_SHD_AVG1[i];
		}
		if(avg_min > R_SHD_AVG1[i])
		{
			avg_min = R_SHD_AVG1[i];
		}
	}
	Max_R_SHD_AVG1 = avg_max;
	Min_R_SHD_AVG1 = avg_min;
	double sabun = (double)(avg_max - avg_min);
	double sho = (double)(sabun / (double)avg_max);
	Rst_R_SHD_AVG1 = (double)(sho * 100);

//エリア2のシェーディング計算
	a2x1 = pWndForm->GetDlgItemInt(IDC_EDIT11);
	a2y1 = pWndForm->GetDlgItemInt(IDC_EDIT12);
	a2x2 = pWndForm->GetDlgItemInt(IDC_EDIT13);
	a2y2 = pWndForm->GetDlgItemInt(IDC_EDIT14);

	if(a2x1 > CKOP::BMP_WID - 1)
	{
		a2x1 = CKOP::BMP_WID - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT11, a2x1);
	}
	if(a2y1 > CKOP::BMP_HEI - 1)
	{
		a2y1 = CKOP::BMP_HEI - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT12, a2y1);
	}
	if(a2x2 > CKOP::BMP_WID - 1)
	{	
		a2x2 = CKOP::BMP_WID - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT13, a2x2);
	}
	if(a2y2 > CKOP::BMP_HEI - 1)
	{
		a2y2 = CKOP::BMP_HEI - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT14, a2y2);
	}

	if(a2x1 < 1)
	{
		a2x1 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT11, a2x1);
	}
	if(a2y1 < 1)
	{
		a2y1 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT12, a2y1);
	}
	if(a2x2 < 1)
	{
		a2x2 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT13, a2x2);
	}
	if(a2y2 < 1)
	{
		a2y2 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT14, a2y2);
	}

	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;

	y1 = a2y1;
	y2 = a2y2 - a2y1;
	x1 = a2x1;
	x2 = a2x2 - a2x1;

	subx = x2 / subCount;
	subNum = 0;
	while(subNum < subCount)
	{
		for(int y = y1; y <= (y1 + y2); y++)//細切れ単位で解析
		{
			int upperlimit = 0;
			if(subNum != (subCount - 1))//最後以外
			{
				upperlimit = x1 + subx * (subNum + 1);
			}
			else
			{
				upperlimit = a2x2;
			}

			for(int x = (x1 + subx * subNum); x <= upperlimit; x++)
			{
				ret = CKOP::GET_PIXEL(x, y, pImgPxl);
				if (ret < m_d.P_GABVAL) {
					continue;//ゴミ判定(計算から除外)
				}

				if (max < ret) {
					max = ret;
				}
				if (min > ret) {
					min = ret;
				}
				ttl += ret;
				cnt++;
			}
		}

		avg = ttl / cnt;
		R_SHD_MAX2[subNum] = max;
		R_SHD_MIN2[subNum] = min;
		R_SHD_AVG2[subNum] = avg;
		R_SHD_POS2[subNum] = (max-avg)/avg * 100;
		R_SHD_NEG2[subNum] = (min-avg)/avg * 100;

		subNum++;
	}

	//全ブロックの平均値から最大値と最小値を求める
	avg_max = -1;
	avg_min = 256;
	for(int i = 0; i < subNum; i++)
	{
		if(avg_max < R_SHD_AVG2[i])
		{
			avg_max = R_SHD_AVG2[i];
		}
		if(avg_min > R_SHD_AVG2[i])
		{
			avg_min = R_SHD_AVG2[i];
		}
	}
	Max_R_SHD_AVG2 = avg_max;
	Min_R_SHD_AVG2 = avg_min;
	sabun = (double)(avg_max - avg_min);
	sho = (double)(sabun / (double)avg_max);
	Rst_R_SHD_AVG2 = (double)(sho * 100);


//エリア3のシェーディング計算
	a3x1 = pWndForm->GetDlgItemInt(IDC_EDIT15);
	a3y1 = pWndForm->GetDlgItemInt(IDC_EDIT16);
	a3x2 = pWndForm->GetDlgItemInt(IDC_EDIT17);
	a3y2 = pWndForm->GetDlgItemInt(IDC_EDIT18);

	if(a3x1 > CKOP::BMP_WID - 1)
	{
		a3x1 = CKOP::BMP_WID - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT11, a3x1);
	}
	if(a3y1 > CKOP::BMP_HEI - 1)
	{
		a3y1 = CKOP::BMP_HEI - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT12, a3y1);
	}
	if(a3x2 > CKOP::BMP_WID - 1)
	{	
		a3x2 = CKOP::BMP_WID - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT13, a3x2);
	}
	if(a3y2 > CKOP::BMP_HEI - 1)
	{
		a3y2 = CKOP::BMP_HEI - 1;
		pWndForm->SetDlgItemInt(IDC_EDIT14, a3y2);
	}

	if(a3x1 < 1)
	{
		a3x1 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT11, a3x1);
	}
	if(a3y1 < 1)
	{
		a3y1 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT12, a3y1);
	}
	if(a3x2 < 1)
	{
		a3x2 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT13, a3x2);
	}
	if(a3y2 < 1)
	{
		a3y2 = 1;
		pWndForm->SetDlgItemInt(IDC_EDIT14, a3y2);
	}

	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;

	y1 = a3y1;
	y2 = a3y2 - a3y1;
	x1 = a3x1;
	x2 = a3x2 - a3x1;

	subx = x2 / subCount;
	subNum = 0;
	while(subNum < subCount)
	{
		for(int y = y1; y <= (y1 + y2); y++)//細切れ単位で解析
		{
			int upperlimit = 0;
			if(subNum != (subCount - 1))//最後以外
			{
				upperlimit = x1 + subx * (subNum + 1);
			}
			else
			{
				upperlimit = a3x2;
			}

			for(int x = (x1 + subx * subNum); x <= upperlimit; x++)
			{
				ret = CKOP::GET_PIXEL(x, y, pImgPxl);
				if (ret < m_d.P_GABVAL) {
					continue;//ゴミ判定(計算から除外)
				}

				if (max < ret) {
					max = ret;
				}
				if (min > ret) {
					min = ret;
				}
				ttl += ret;
				cnt++;
			}
		}

		avg = ttl / cnt;
		R_SHD_MAX3[subNum] = max;
		R_SHD_MIN3[subNum] = min;
		R_SHD_AVG3[subNum] = avg;
		R_SHD_POS3[subNum] = (max-avg)/avg * 100;
		R_SHD_NEG3[subNum] = (min-avg)/avg * 100;

		subNum++;
	}

	//全ブロックの平均値から最大値と最小値を求める
	avg_max = -1;
	avg_min = 256;
	for(int i = 0; i < subNum; i++)
	{
		if(avg_max < R_SHD_AVG3[i])
		{
			avg_max = R_SHD_AVG3[i];
		}
		if(avg_min > R_SHD_AVG3[i])
		{
			avg_min = R_SHD_AVG3[i];
		}
	}
	Max_R_SHD_AVG3 = avg_max;
	Min_R_SHD_AVG3 = avg_min;
	sabun = (double)(avg_max - avg_min);
	sho = (double)(sabun / (double)avg_max);
	Rst_R_SHD_AVG3 = (double)(sho * 100);

}

void CKOP_SHADING_EX::CALC_SHADING(LPBYTE pImgPxl)
{
	int		ret;
	int		max, min;
	double	ttl, avg;
	int		cnt;
	int		y1, y2, x1, x2;

	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;

	y1 = m_d.P_SKIPCNT;
	y2 = CKOP::BMP_HEI - m_d.P_SKIPCNT;
	x1 = m_d.P_SKIPCNT;
	x2 = CKOP::BMP_WID - m_d.P_SKIPCNT;
#if 1//2017.04.01
	int	lcnt = 0;
	//m_ptg.RemoveAll();
#endif
	//全体のシェーディング値を求める
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
#if 1//2017.04.01
			if (ret < m_d.P_GABVAL) {
				//POINT	p;
				//p.x = x, p.y = y;
				//m_ptg.Add(p);
				continue;//ゴミ判定(計算から除外)
			}
#endif
			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
			cnt++;
		}
	}
	avg = ttl / cnt;
	m_d.R_SHD_MAX[0] = max;
	m_d.R_SHD_MIN[0] = min;
	m_d.R_SHD_AVG[0] = avg;
	m_d.R_SHD_POS[0] = (max-avg)/avg * 100;
	m_d.R_SHD_NEG[0] = (min-avg)/avg * 100;

	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;
	// 緑線のシェーディング値を求める
	if (m_d.P_DIAGO) {
		//対角:(0,      0) -> (BMP_WID,BMP_HEI);
		for (int x = x1; x < x2; x++) {
			int	y = (int)AXpB(0, CKOP::BMP_WID-1, 0, CKOP::BMP_HEI-1, x);
			if (y < y1 || y >= y2) {
				continue;
			}
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
#if 1//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[0] + lcnt) = (BYTE)ret;
				lcnt++;
			}
			if (ret < m_d.P_GABVAL) {
				continue;//ゴミ判定(計算から除外)
			}
#endif
			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
#if 0//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[0] + cnt) = (BYTE)ret;
			}
#endif
			cnt++;
		}
	}
	else {
		//ＸＹ:(0, BMP_HEI/2) -> (BMP_WID,BMP_HEI/2);
		int	y = CKOP::BMP_HEI/2;
		for (int x = x1; x < x2; x++) {
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
#if 1//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[0] + lcnt) = (BYTE)ret;
				lcnt++;
			}
			if (ret < m_d.P_GABVAL) {
				continue;//ゴミ判定(計算から除外)
			}
#endif
			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
#if 0//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[0] + cnt) = (BYTE)ret;
			}
#endif
			cnt++;
		}
	}
	avg = ttl / cnt;
	m_d.R_SHD_MAX[1] = max;
	m_d.R_SHD_MIN[1] = min;
	m_d.R_SHD_AVG[1] = avg;
	m_d.R_SHD_POS[1] = (max-avg)/avg * 100;
	m_d.R_SHD_NEG[1] = (min-avg)/avg * 100;
#if 1//2017.04.01
	m_nLineCnt[0] = lcnt;
	lcnt = 0;
#else
#if 1//2015.09.10
	m_nLineCnt[0] = cnt;
#endif
#endif
	max = -1, min = 256;
	ttl = 0.0;
	cnt = 0;
	// 赤線のシェーディング値を求める
	if (m_d.P_DIAGO) {
		//対角:(0,BMP_HEI) -> (BMP_WID,      0);
		for (int x = x1; x < x2; x++) {
			int	y = (int)AXpB(0, CKOP::BMP_WID-1, CKOP::BMP_HEI-1, 0, x);
			if (y < y1 || y >= y2) {
				continue;
			}
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
#if 1//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[1] + lcnt) = (BYTE)ret;
				lcnt++;
			}
			if (ret < m_d.P_GABVAL) {
				continue;//ゴミ判定(計算から除外)
			}
#endif
			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
#if 0//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[1] + cnt) = (BYTE)ret;
			}
#endif
			cnt++;
		}
	}
	else {
		//ＸＹ:(BMP_WID/2, 0) -> (BMP_WID/2,BMP_HEI);
		int	x = CKOP::BMP_WID/2;
		for (int y = y1; y < y2; y++) {
			ret = CKOP::GET_PIXEL(x, y, pImgPxl);
#if 1//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[1] + lcnt) = (BYTE)ret;
				lcnt++;
			}
			if (ret < m_d.P_GABVAL) {
				continue;//ゴミ判定(計算から除外)
			}
#endif
			if (max < ret) {
				max = ret;
			}
			if (min > ret) {
				min = ret;
			}
			ttl += ret;
#if 0//2017.04.01
			if (TRUE) {
				*(m_pLineBuf[1] + cnt) = (BYTE)ret;
			}
#endif
			cnt++;
		}
	}
	avg = ttl / cnt;
	m_d.R_SHD_MAX[2] = max;
	m_d.R_SHD_MIN[2] = min;
	m_d.R_SHD_AVG[2] = avg;
	m_d.R_SHD_POS[2] = (max-avg)/avg * 100;
	m_d.R_SHD_NEG[2] = (min-avg)/avg * 100;
#if 1//2017.04.01
	m_nLineCnt[1] = lcnt;
#else
#if 1//2015.09.10
	m_nLineCnt[1] = cnt;
#endif
#endif
}
void CKOP_SHADING_EX::DRAW_DIAGO(CWnd *pWndForm)
{
//	return;
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);

	CDC	*pDC = pWnd->GetDC();
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penGreen);

	if (m_d.P_DIAGO) {
		//対角
		pDC->MoveTo(            0,             0);
		pDC->LineTo(CKOP::IMG_WID, CKOP::IMG_HEI);

		pDC->SelectObject(&CKOP::m_penRed);

		pDC->MoveTo(            0, CKOP::IMG_HEI);
		pDC->LineTo(CKOP::IMG_WID,             0);
	}
	else {
		//ＸＹ
		double wid_bairitsu = (double)(CKOP::BMP_WID) / (double)(CKOP::IMG_WID);
		double hei_bairitsu = (double)(CKOP::BMP_HEI) / (double)(CKOP::IMG_HEI);
//
		int a1x = pWndForm->GetDlgItemInt(IDC_EDIT7);
		int a1y = pWndForm->GetDlgItemInt(IDC_EDIT8);
		int b1x = pWndForm->GetDlgItemInt(IDC_EDIT9);
		int b1y = pWndForm->GetDlgItemInt(IDC_EDIT10);

		if(a1x < 1) a1x = 1;
		if(b1x < 1) b1x = 1;
		if(a1x > CKOP::BMP_WID - 1) a1x = CKOP::BMP_WID - 1;
		if(b1x > CKOP::BMP_WID - 1) b1x = CKOP::BMP_WID - 1;

		if(a1y < 1) a1y = 1;
		if(b1y < 1) b1y = 1;
		if(a1y > CKOP::BMP_HEI - 1) a1y = CKOP::BMP_HEI - 1;
		if(b1y > CKOP::BMP_HEI - 1) b1y = CKOP::BMP_HEI - 1;

		a1x /= wid_bairitsu;
		a1y /= hei_bairitsu;
		b1x /= wid_bairitsu;
		b1y /= hei_bairitsu;
//
		int a2x = pWndForm->GetDlgItemInt(IDC_EDIT11);
		int a2y = pWndForm->GetDlgItemInt(IDC_EDIT12);
		int b2x = pWndForm->GetDlgItemInt(IDC_EDIT13);
		int b2y = pWndForm->GetDlgItemInt(IDC_EDIT14);

		if(a2x < 1) a2x = 1;
		if(b2x < 1) b2x = 1;
		if(a2x > CKOP::BMP_WID - 1) a2x = CKOP::BMP_WID - 1;
		if(b2x > CKOP::BMP_WID - 1) b2x = CKOP::BMP_WID - 1;

		if(a2y < 1) a2y = 1;
		if(b2y < 1) b2y = 1;
		if(a2y > CKOP::BMP_HEI - 1) a2y = CKOP::BMP_HEI - 1;
		if(b2y > CKOP::BMP_HEI - 1) b2y = CKOP::BMP_HEI - 1;

		a2x /= wid_bairitsu;
		a2y /= hei_bairitsu;
		b2x /= wid_bairitsu;
		b2y /= hei_bairitsu;
//
		int a3x = pWndForm->GetDlgItemInt(IDC_EDIT15);
		int a3y = pWndForm->GetDlgItemInt(IDC_EDIT16);
		int b3x = pWndForm->GetDlgItemInt(IDC_EDIT17);
		int b3y = pWndForm->GetDlgItemInt(IDC_EDIT18);

		if(a3x < 1) a3x = 1;
		if(b3x < 1) b3x = 1;
		if(a3x > CKOP::BMP_WID - 1) a3x = CKOP::BMP_WID - 1;
		if(b3x > CKOP::BMP_WID - 1) b3x = CKOP::BMP_WID - 1;

		if(a3y < 1) a3y = 1;
		if(b3y < 1) b3y = 1;
		if(a3y > CKOP::BMP_HEI - 1) a3y = CKOP::BMP_HEI - 1;
		if(b3y > CKOP::BMP_HEI - 1) b3y = CKOP::BMP_HEI - 1;

		a3x /= wid_bairitsu;
		a3y /= hei_bairitsu;
		b3x /= wid_bairitsu;
		b3y /= hei_bairitsu;
//
		CKOP::DRAW_MARQUEE(pDC, a1x, a1y, b1x, b1y);

		pDC->SelectObject(&CKOP::m_penRed);
		CKOP::DRAW_MARQUEE(pDC, a2x, a2y, b2x, b2y);

		pDC->SelectObject(&CKOP::m_penBlue);
		CKOP::DRAW_MARQUEE(pDC, a3x, a3y, b3x, b3y);
	}
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
void CKOP_SHADING_EX::UPDATE_RESULT(CWnd *pWndForm)
{
	pWndForm->GetDlgItem(IDC_STATIC19)->SetWindowText(F0S(Max_R_SHD_AVG1));
	pWndForm->GetDlgItem(IDC_STATIC20)->SetWindowText(F0S(Min_R_SHD_AVG1));
	pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(F2S(Rst_R_SHD_AVG1, "%"));

	pWndForm->GetDlgItem(IDC_STATIC22)->SetWindowText(F0S(Max_R_SHD_AVG2));
	pWndForm->GetDlgItem(IDC_STATIC23)->SetWindowText(F0S(Min_R_SHD_AVG2));
	pWndForm->GetDlgItem(IDC_STATIC27)->SetWindowText(F2S(Rst_R_SHD_AVG2, "%"));

	if(m_d.P_DIAGO)
	{
	}
	else
	{
		pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText(F0S(Max_R_SHD_AVG3));
		pWndForm->GetDlgItem(IDC_STATIC29)->SetWindowText(F0S(Min_R_SHD_AVG3));
		pWndForm->GetDlgItem(IDC_STATIC30)->SetWindowText(F2S(Rst_R_SHD_AVG3, "%"));
	}
}

void CKOP_SHADING_EX::DRAW_GRADIENT(CWnd *pWnd)
{
	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
	CRect	rt;
	int		hei;
	int		h;
	pWnd->GetWindowRect(&rt);

	hei = rt.Height()-1;

	for (int y = 0; y < hei; y++) {
		h = 359-(int)((double)y/(double)hei*360);
		pDC->FillSolidRect(0, y, rt.Width()-1, 1, HSV2RGB(h, 255, 255));
//		if (!(x%90) && x <= 360) {
//		pDC->FillSolidRect(x, 0, 1, rt.Height(), RGB(0,0,0));
//		}
	}
	pWnd->ReleaseDC(pDC);
}

void CKOP_SHADING_EX::DRAW_GRAPH(CWnd *pWnd)
{
	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
//	CRect	rt(m_gdef.rt_gr);
	int		h = 0;
	LPBYTE	p;
	CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	int		ycnt, xcnt;

	ycnt = CKOP::BMP_HEI - m_d.P_SKIPCNT*2;
	xcnt = CKOP::BMP_WID - m_d.P_SKIPCNT*2;



	CKOP::GDEF_GRID(&m_gdef, pDC);

	p = m_pLineBuf[0];
	
	pDC->SelectObject(&CKOP::m_penGreen);

	for (int i = 0; i <
#if 1//2015.09.10
		m_nLineCnt[0];
#else
		xcnt;
#endif
		i++, p++) {
		int	x, y;
#if 1//2015.09.10
		x = CKOP::GDEF_XPOS(&m_gdef, i+m_d.P_SKIPCNT);
#else
		x = CKOP::GDEF_XPOS(&m_gdef, i);
#endif
		y = CKOP::GDEF_YPOS(&m_gdef, (int)*p);
		if (!i) {
			pDC->MoveTo(x, y);
		}
		else {
			pDC->LineTo(x, y);
		}
	}
	p = m_pLineBuf[1];
	pDC->SelectObject(&CKOP::m_penRed);
#if 1//2015.09.10
	int		cnt = m_nLineCnt[1];
#else
	int		cnt = m_d.P_DIAGO ? xcnt: ycnt;
#endif
	for (int i = 0; i < cnt; i++, p++) {
		int	x, y;
#if 1//2015.09.10
		x = CKOP::GDEF_XPOS(&m_gdef, i+m_d.P_SKIPCNT);
#else
		x = CKOP::GDEF_XPOS(&m_gdef, i);
#endif
		y = CKOP::GDEF_YPOS(&m_gdef, (int)*p);
		if (!i) {
			pDC->MoveTo(x, y);
		}
		else {
			pDC->LineTo(x, y);
		}
#if 1//2015.09.10
		if (i == (cnt-1)) {
			i = i;
		}
#endif
	}
//	pDC->SetBkMode(TRANSPARENT);
//	rt.DeflateRect(3,3);
//	pDC->DrawText("現状 緑線のみ対応\rY軸は0-255固定", &rt, DT_RIGHT|DT_TOP);
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
#if 1//2017.04.01
void DRAW_GABPTS_EX(LPBYTE pImgPxl)
{
	int		unit = CKOP::BMP_BIT_CNT/8;
	int		size = CKOP::BMP_WID * CKOP::BMP_HEI;

	if (m_d.P_COLMAP != 3) {//生画像+ゴミ除去
		return;
	}
	if (CKOP::BMP_BIT_CNT == 8) {
		for (int i = 0; i < 256; i++) {
			if (i < m_d.P_GABVAL) {
				CKOP::RGB_TBL2[i].rgbBlue  =  0;
				CKOP::RGB_TBL2[i].rgbGreen =  0;
				CKOP::RGB_TBL2[i].rgbRed   =255;
			}
			else {
				CKOP::RGB_TBL2[i].rgbBlue  =  i;
				CKOP::RGB_TBL2[i].rgbGreen =  i;
				CKOP::RGB_TBL2[i].rgbRed   =  i;
			}
		}
		return;
	}
	for (int i = 0; i < size; i++) {
		int	n = *pImgPxl;
		LPDWORD	p = (LPDWORD)pImgPxl;
		if (n < m_d.P_GABVAL) {
			switch (CKOP::BMP_BIT_CNT) {
			case 24:
			{
				LPBYTE	pb = (LPBYTE)p;
				*(pb+0) = 0xff;
				*(pb+1) = 0;
				*(pb+2) = 0;
			}
			break;
			case 32:
				*p = 0xffff0000;
			break;
			}
		}

		pImgPxl += unit;
	}
}
#endif
void COLOR_MAP_EX(LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	int		unit = CKOP::BMP_BIT_CNT/8;
	int		size = CKOP::BMP_WID * CKOP::BMP_HEI;
#if 0
	if (TRUE) {
		LPDWORD	p = (LPDWORD)pImgPxl;
		int		r = (*p & 0x0000FF);
		int		g = (*p & 0x00FF00)>>8;
		int		b = (*p & 0xFF0000)>>16;
		if (r == g && g == b) {
			// gray
		}
		else {
			//color
			return;//skip
		}
	}
#endif
	switch (m_d.P_COLMAP) {
	case 1:
#if 1//2015.07.24
		if (CKOP::BMP_BIT_CNT == 8) {
			break;
		}
#endif
		for (int i = 0; i < size; i++) {
			int	n = *pImgPxl;
			LPDWORD	p = (LPDWORD)pImgPxl;
			if (FALSE) {
			*p = 0xFF0000;	//R
			*p = 0x00FF00;	//G
			*p = 0x0000FF;	//B
			}
			else {
			//*p = m_G2CTBL[n];
			}
			switch (CKOP::BMP_BIT_CNT) {
			case 24:
			{
				int	rgb = m_G2CTBL[n];
				LPBYTE	pb = (LPBYTE)p;
				*(pb+0) = ((rgb >> 0) & 0xff);
				*(pb+1) = ((rgb >> 8) & 0xff);
				*(pb+2) = ((rgb >>16) & 0xff);
			}
			break;
			case 32:
				*p = m_G2CTBL[n];
			break;
			}

			pImgPxl += unit;
		}
	break;
	case 2://カラーMAP2
#if 1//2015.07.24
		if (CKOP::BMP_BIT_CNT == 8) {
			for (int i = 0; i < 256; i++) {
				if (i < m_d.R_SHD_MIN[0] || i > m_d.R_SHD_MAX[0]) {
					CKOP::RGB_TBL2[i].rgbBlue  = 0;
					CKOP::RGB_TBL2[i].rgbGreen = 0;
					CKOP::RGB_TBL2[i].rgbRed   = 0;
				}
				else {
					int	j;
					if ((m_d.R_SHD_MAX[0] - m_d.R_SHD_MIN[0]) == 0) {
						j = 255;
					}
					else {
						j = 255 * (i - m_d.R_SHD_MIN[0]) / (m_d.R_SHD_MAX[0] - m_d.R_SHD_MIN[0]);
						if (j < 0) {
							j = 0;
						}
						else if (j > 255) {
							j = 255;
						}
					}
					CKOP::RGB_TBL2[i] = CKOP::RGB_TBL1[j];
				}
			}
		}
#endif
		for (int i = 0; i < size; i++) {
			int	n = *pImgPxl;
			LPDWORD	p = (LPDWORD)pImgPxl;
//			n = 255 * (n - m_d.R_SHD_MIN[0]) / (m_d.R_SHD_MAX[0] - m_d.R_SHD_MIN[0]);
			if ((m_d.R_SHD_MAX[0] - m_d.R_SHD_MIN[0]) == 0) {
			n = 255;
			}
			else {
			n = 255 * (n - m_d.R_SHD_MIN[0]) / (m_d.R_SHD_MAX[0] - m_d.R_SHD_MIN[0]);
			}
			if (n < 0) {
				n = 0;
			}
			if (n > 255) {
				n = 255;
			}
			switch (CKOP::BMP_BIT_CNT) {
			case 24:
			{
				int	rgb = m_G2CTBL[n];
				LPBYTE	pb = (LPBYTE)p;
				*(pb+0) = ((rgb >> 0) & 0xff);
				*(pb+1) = ((rgb >> 8) & 0xff);
				*(pb+2) = ((rgb >>16) & 0xff);
			}
			break;
			case 32:
				*p = m_G2CTBL[n];
			break;
			}

			pImgPxl += unit;
		}
	break;
	case 0://生画像
#if 1
	if (CKOP::BMP_BIT_CNT == 8) {
		for (int i = 0; i < 256; i++) {
			if (i < m_d.P_GABVAL) {
				CKOP::RGB_TBL2[i].rgbBlue  =  0;
				CKOP::RGB_TBL2[i].rgbGreen =  0;
				CKOP::RGB_TBL2[i].rgbRed   =255;
			}
			else {
				CKOP::RGB_TBL2[i].rgbBlue  =  i;
				CKOP::RGB_TBL2[i].rgbGreen =  i;
				CKOP::RGB_TBL2[i].rgbRed   =  i;
			}
		}
	}
	break;
#endif
	default:
	break;
	}
}
void CKOP_SHADING_EX::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CComboBox	*p1 = (CComboBox*)(pWndForm->GetDlgItem(IDC_COMBO1));
	CComboBox	*p2 = (CComboBox*)(pWndForm->GetDlgItem(IDC_COMBO2));
#if 1//2015.09.10
	if (m_scan_code == 1) {
		//補正画像\rWHITE取込
		IMPORT_CORR_IMAGE_EX(1, pImgPxl, TRUE);
		m_scan_code = 0;
		if (IS_AVAILABLE_CORRECTION()) {
			 pWndForm->GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		}
		pWndForm->GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
	}
	if (m_scan_code == 2) {
		//補正画像\rBLACK取込
		m_scan_code = 0;
	}
	m_d.P_SHAD_CORR = pWndForm->IsDlgButtonChecked(IDC_CHECK2);
#endif
	if (p1 != NULL) {
		m_d.P_FILTER = p1->GetCurSel();
	}
	if (p2 != NULL) {
		m_d.P_PARAM = p2->GetCurSel();
	}
#if 1//2015.09.10
	if (m_d.P_SHAD_CORR && IS_AVAILABLE_CORRECTION()) {
		DO_CORRECTION(pImgPxl);
	}
#endif
#if 1//2017.04.01
	m_d.P_SKIPCNT = pWndForm->GetDlgItemInt(IDC_EDIT1);
	if (m_d.P_SKIPCNT < 0) {
		pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_SKIPCNT = 0);
	}
	else if (m_d.P_SKIPCNT > 10) {
		pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_SKIPCNT = 10);
	}
	m_d.P_GABVAL = pWndForm->GetDlgItemInt(IDC_EDIT2);
	if (m_d.P_GABVAL < 0) {
		pWndForm->SetDlgItemInt(IDC_EDIT2, m_d.P_GABVAL = 0);
	}
	else if (m_d.P_GABVAL > 255) {
		pWndForm->SetDlgItemInt(IDC_EDIT2, m_d.P_GABVAL = 255);
	}
#endif

	DO_FILTER(pWndForm, pImgPxl, m_d.P_FILTER, m_d.P_PARAM);
	CALC_SHADING(pImgPxl);
	if(m_d.P_DIAGO)//対角
	{
		CALC_SHADING_CROSS(pImgPxl, pWndForm);
	}
	else			//水平
	{
		CALC_SHADING_HORIZON(pImgPxl, pWndForm);
	}
	
	COLOR_MAP_EX(pImgPxl, pbmpinfo);
#if 1//2017.04.01
	if (m_d.P_COLMAP == 3) {
	::DRAW_GABPTS_EX(pImgPxl);
	}
#endif

}

void CKOP_SHADING_EX::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);
#if 0
	DRAW_DIAGO(pWnd);
#else
	DRAW_DIAGO(pWndForm);
#endif
//	DRAW_GRADIENT(pWndForm->GetDlgItem(IDC_STATIC18));
//	DRAW_GRAPH(pWndForm->GetDlgItem(IDC_STATIC37));
#if 1//2017.04.01
	if (m_d.P_COLMAP == 0) {
//	DRAW_GABPTS(pWnd);
//	::DRAW_GABPTS(pImgPxl);
	}
#endif
	UPDATE_RESULT(pWndForm);
}

BOOL CKOP_SHADING_EX::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON7://保存
#if 0
			CKOP::SAVE_WINDOW(AfxGetMainWnd(), "shading.png");
#else
			CKOP::SAVE_WINDOW(pWndForm, "shadingEx.png");
#endif
#if 1//2017.07.18
			{
				CCSV	csv;
				SET_CSV(&csv);
				CKOP::SAVE_CSV(&csv, "shadingEx.csv");
			}
#endif
			return(TRUE);
		break;
		case IDC_BUTTON8://戻る
			CKOP::MOVE_FORM(pWndForm, 1);
			return(TRUE);
		break;
#if 1//2015.09.10
		case IDC_BUTTON10://補正画像\rWHITE取込
			m_scan_code = 1;
			pWndForm->GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
		break;
		case IDC_BUTTON11://補正画像\rBLACK取込
			m_scan_code = 2;
		break;
#endif
		}
	break;
	case CBN_SELCHANGE:
		if (nID == IDC_COMBO1 || nID == IDC_COMBO2) {
pWndForm->Invalidate(FALSE);
		}
	break;
	}
	return(FALSE);
}

BOOL CKOP_SHADING_EX::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
{
	UINT	nID;

	if (pWndForm->m_hWnd == pMsg->hwnd) {
		nID = IDC_IMAGEAREA;
	}
	else {
		nID = ::GetDlgCtrlID(pMsg->hwnd);
	}

	switch (nID) {
	case IDC_COMBO1:
	case IDC_COMBO2:
		switch (pMsg->message) {
		case 512:
		case 513:
		case 514:
		break;
		default:
		nID = nID;
		break;
		}
	break;
	case IDC_STATIC35:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
#if 1//2017.04.01
			if (pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONDBLCLK) {
				if (--m_d.P_COLMAP<0) {
					m_d.P_COLMAP = 3;
				}
			}
			else if (++m_d.P_COLMAP>3) {
				m_d.P_COLMAP = 0;
			}
#else
			if (++m_d.P_COLMAP>2) {
				m_d.P_COLMAP = 0;
			}
#endif
			CKOP::P_COLMAP = m_d.P_COLMAP;
			switch (m_d.P_COLMAP) {
			case  1: ::SetWindowText(pMsg->hwnd, "カラーMAP1");break;
			case  2: ::SetWindowText(pMsg->hwnd, "カラーMAP2");break;
#if 1//2017.04.01
			case  3: ::SetWindowText(pMsg->hwnd, "生画像+ゴミ除去");break;
#endif
			case  0:
			default: ::SetWindowText(pMsg->hwnd, "生画像");break;
			}
			switch (m_d.P_COLMAP) {
			case  1:
				pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_SHOW);
				pWndForm->GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
			break;
			case  2:
				pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
				pWndForm->GetDlgItem(IDC_STATIC25)->ShowWindow(SW_SHOW);
			break;
			case  0:
#if 1//2017.04.01
			case  3:
#endif
			default:
				pWndForm->GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
				pWndForm->GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
			break;
			}
			pWndForm->Invalidate();
		break;
		}
	break;
	case IDC_STATIC36:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			if (++m_d.P_DIAGO > 1) {
				m_d.P_DIAGO = 0;
			}
			switch (m_d.P_DIAGO) {
			case  1: ::SetWindowText(pMsg->hwnd, "対角");break;
			case  0:
			default: ::SetWindowText(pMsg->hwnd, "ＸＹ");break;
			}
			pWndForm->Invalidate();
		break;
		}
	break;
	}
	return(FALSE);
}

#if 1//2017.07.18
void CKOP_SHADING_EX::SET_CSV(CCSV *pc)
{
	CString	buf;
	LPCTSTR	pHead0[] = {
		"", "Ave", "Max", "Min", "シェーディング:＋(%)", "シェーディング:－(%)"
	};
	LPCTSTR	pHead1[] = {
		"解析結果", "緑線", "赤線", ""
	};
	int		r = 0;
	pc->set( 0, r, "シェーディング解析");
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
	pc->set( 0, r, "周辺無視範囲");
	pc->set( 1, r, I2S(m_d.P_SKIPCNT));
	r++;
	//---
	pc->set( 0, r, "ゴミ除去");
	pc->set( 1, r, I2S(m_d.P_GABVAL));
	r++;
	//---
	pc->set( 0, r, "緑赤線");
	pc->set( 1, r, m_d.P_DIAGO ? "対角": "ＸＹ" );
	r++;
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
			case 1: buf = F2S(m_d.R_SHD_AVG[i]); break;
			case 2: buf = F0S(m_d.R_SHD_MAX[i]); break;
			case 3: buf = F0S(m_d.R_SHD_MIN[i]); break;
			case 4: buf = F2S(m_d.R_SHD_POS[i]); break;
			case 5: buf = F2S(m_d.R_SHD_NEG[i]); break;
			}
			pc->set(c, r+i, buf);
		}
	}
	return;
}
#endif