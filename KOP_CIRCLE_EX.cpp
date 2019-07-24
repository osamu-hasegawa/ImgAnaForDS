#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_CIRCLE_EX.h"
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
	int		P_SAVEPOS;
	int		P_BIN_VAL;
	int		P_BIN_IMG;
	int		P_BAK_MOD;
#if 1//2015.09.10
	int		P_SHAD_CORR;
#endif
#if 1//2018.05.01
	RECT	P_AREA[9];//エリア
	int		P_CUR_AREA;//選択中のエリアNo
	int		isEnable[9];
#endif
	// 
	double	R_CEN_X;
	double	R_CEN_Y;
	// RESULT
	double	R_COG_X[4][9];
	double	R_COG_Y[4][9];
	double	R_DIF_X[4][9];
	double	R_DIF_Y[4][9];
	double	R_SQAR[4][9];
	double	R_PHI[4][9];
	//---
	HBITMAP	R_HBMP[4][9];
	//---
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
	int		R_MIN_IDX;
	double	R_MIN;
	double	R_POS  [256];
	double	R_COG_X[256][9];
	double	R_COG_Y[256][9];
	double	R_DIF_X[256][9];
	double	R_DIF_Y[256][9];
	double	R_SQAR [256][9];
	double	R_PHI  [256][9];
} m_s;
#endif
//------------------------------------
#if 1//2018.05.01
static
int		m_nCaptured;
#endif
//------------------------------------

ITBL CKOP_CIRCLE_EX::itbl[] = {
	{ 0, IDC_COMBO1, " 1"},
	{ 0, IDC_COMBO1, " 2"},
	{ 0, IDC_COMBO1, " 3"},
	{ 0,          0,   0}
};

//
// 円の面積＆重心画面定義
//    基準WID = BASE_WID = 644
//    基準HEI = BASE_HEI = 482
//      IMG_WID,IMG_HEIは現在接続中カメラの画像表示エリアのサイズ(実サイズの1/2 or 1/4 or ...)
//      基準ＸＹサイズとのずれをoffsで制御して画面を整列させる
//        offs=1:CXをIMG_WIDで
//             2:CYをIMG_HEIで
//             4: Xを(IMG_WID-基準WID)分オフセットする
//             8: Yを(IMG_HEI-基準HEI)分オフセットする
//
CTBL CKOP_CIRCLE_EX::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 5, "LOGO"              ,1140,  30, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
#if 1//2017.04.01
	{ 0, 0  , IDC_STATIC10, 5, "円の面積＆重心"    ,  10,   3, 322, 22,           SS_CENTERIMAGE},
#else
	{ 0, 0  , IDC_STATIC10, 5, "円の面積＆重心"    ,  10,   3, 644, 22,           SS_CENTERIMAGE},
#endif
	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  , 910,   3, 220, 22, WS_BORDER|SS_CENTERIMAGE},
#if 1//2015.09.10
	{ 0, 0  , IDC_STATIC12, 1, "1回目"             , 657,   5, 200, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC13, 0, "IMAGE"             , 657,  25, 240,180, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC14, 1, "2回目"             , 657, 205, 200, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC15, 0, "IMAGE"             , 657, 225, 240,180, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC16, 1, "3回目"             , 657, 405, 200, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC17, 0, "IMAGE"             , 657, 425, 240,180, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},
#else
	{ 0, 0  , IDC_STATIC12, 1, "1回目"             , 655,   5, 200, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC13, 0, "IMAGE"             , 655,  25, 240,180, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC14, 1, "2回目"             , 655, 205, 200, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC15, 0, "IMAGE"             , 655, 225, 240,180, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},
	{ 0, 0  , IDC_STATIC16, 1, "3回目"             , 655, 405, 200, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC17, 0, "IMAGE"             , 655, 425, 240,180, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},//SS_WHITEFRAME},
#endif
	{ 0, 8  , IDC_STATIC18, 3, "測定結果�@"        ,  10, 518, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC19, 3, "重心 (XXX.X,XXX.X)",  10, 545, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC20, 3, "面積 XXXXX pix"    , 350, 545, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC21, 3, "中心からのズレ(XX)",  10, 572, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC22, 3, "直径  120.05pix"   , 350, 572, 300, 22, WS_BORDER|SS_CENTERIMAGE},
#if 1//2018.05.01
	{ 0, 8  , IDC_STATIC23, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 518, 300, 22, WS_BORDER|SS_CENTERIMAGE},
#endif
//2番目領域
	{ 0, 8  , IDC_STATIC61, 3, "測定結果�A"        ,  10, 599, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC62, 3, "重心 (XXX.X,XXX.X)",  10, 626, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC63, 3, "面積 XXXXX pix"    , 350, 626, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC64, 3, "中心からのズレ(XX)",  10, 648, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC65, 3, "直径  120.05pix"   , 350, 648, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC66, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 599, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//3番目領域
	{ 0, 8  , IDC_STATIC67, 3, "測定結果�B"        ,  10, 675, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC68, 3, "重心 (XXX.X,XXX.X)",  10, 702, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC69, 3, "面積 XXXXX pix"    , 350, 702, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC70, 3, "中心からのズレ(XX)",  10, 724, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC71, 3, "直径  120.05pix"   , 350, 724, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC72, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 675, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//4番目領域
	{ 0, 8  , IDC_STATIC73, 3, "測定結果�C"        ,  10, 751, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC74, 3, "重心 (XXX.X,XXX.X)",  10, 778, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC75, 3, "面積 XXXXX pix"    , 350, 778, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC76, 3, "中心からのズレ(XX)",  10, 800, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC77, 3, "直径  120.05pix"   , 350, 800, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC78, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 751, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//5番目領域
	{ 0, 8  , IDC_STATIC79, 3, "測定結果�D"        ,  10, 827, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC80, 3, "重心 (XXX.X,XXX.X)",  10, 854, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC81, 3, "面積 XXXXX pix"    , 350, 854, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC82, 3, "中心からのズレ(XX)",  10, 876, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC83, 3, "直径  120.05pix"   , 350, 876, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC84, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 827, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//6番目領域
	{ 0, 8  , IDC_STATIC85, 3, "測定結果�E"        ,  10, 903, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC86, 3, "重心 (XXX.X,XXX.X)",  10, 930, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC87, 3, "面積 XXXXX pix"    , 350, 930, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC88, 3, "中心からのズレ(XX)",  10, 952, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC89, 3, "直径  120.05pix"   , 350, 952, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC90, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 903, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//7番目領域
	{ 0, 8  , IDC_STATIC91, 3, "測定結果�F"        ,  10, 979, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC92, 3, "重心 (XXX.X,XXX.X)",  10, 1006, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC93, 3, "面積 XXXXX pix"    , 350, 1006, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC94, 3, "中心からのズレ(XX)",  10, 1028, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC95, 3, "直径  120.05pix"   , 350, 1028, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC96, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 979, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//8番目領域
	{ 0, 8  , IDC_STATIC97, 3, "測定結果�G"        ,  10, 1055, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC98, 3, "重心 (XXX.X,XXX.X)",  10, 1082, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC99, 3, "面積 XXXXX pix"    , 350, 1082, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC100, 3, "中心からのズレ(XX)", 10, 1104, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC101, 3, "直径  120.05pix"   ,350, 1104, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC102, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													 350, 1055, 300, 22, WS_BORDER|SS_CENTERIMAGE},
//9番目領域
	{ 0, 8  , IDC_STATIC103, 3, "測定結果�H"        ,  10, 1131, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC104, 3, "重心 (XXX.X,XXX.X)",  10, 1158, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC105, 3, "面積 XXXXX pix"    , 350, 1158, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC106, 3, "中心からのズレ(XX)",  10, 1180, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC107, 3, "直径  120.05pix"   , 350, 1180, 300, 22, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 8  , IDC_STATIC108, 3, "エリア(XXX,XXX)-(XXX,XXX)",
													  350, 1131, 300, 22, WS_BORDER|SS_CENTERIMAGE},

//-----
	{ 0, 0  , IDC_STATIC  , 1, "重心"              , 900,  25,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "ズレ"              , 900,  65,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "面積"              , 900, 105,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "直径"              , 900, 145,  80, 20, 0|SS_CENTERIMAGE},
#if 1//2017.07.18
	{ 0, 0  , IDC_STATIC27, 1, "\0(640.3,472.8)"   , 905,  45,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC29, 1, "\0(0.3,-8.8)"      , 905,  85,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC28, 1, "\0 12345 pix"      , 905, 125,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC30, 1, "\0 120.05 pix"     , 905, 165,  75, 20, WS_BORDER|SS_CENTERIMAGE},
#else
	{ 0, 0  , IDC_STATIC27, 1, "(640.3,472.8)"     , 905,  45,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC29, 1, "(0.3,-8.8)"        , 905,  85,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC28, 1, "12345 pix"         , 905, 125,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC30, 1, "120.05 pix"        , 905, 165,  75, 20, WS_BORDER|SS_CENTERIMAGE},
#endif
//-----
	{ 0, 0  , IDC_STATIC  , 1, "重心"              , 900, 225,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "ズレ"              , 900, 265,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "面積"              , 900, 305,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "直径"              , 900, 345,  80, 20, 0|SS_CENTERIMAGE},
#if 1//2017.07.18
	{ 0, 0  , IDC_STATIC1 , 1, "\0(640.3,472.8)"   , 905, 245,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC3 , 1, "\0(0.3,-8.8)"      , 905, 285,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC2 , 1, "\0 12345 pix"      , 905, 325,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC4 , 1, "\0 120.05 pix"     , 905, 365,  75, 20, WS_BORDER|SS_CENTERIMAGE},
#else
	{ 0, 0  , IDC_STATIC1 , 1, "(640.3,472.8)"     , 905, 245,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC3 , 1, "(0.3,-8.8)"        , 905, 285,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC2 , 1, "12345 pix"         , 905, 325,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC4 , 1, "120.05 pix"        , 905, 365,  75, 20, WS_BORDER|SS_CENTERIMAGE},
#endif
//-----
	{ 0, 0  , IDC_STATIC  , 1, "重心"              , 900, 425,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "ズレ"              , 900, 465,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "面積"              , 900, 505,  80, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC  , 1, "直径"              , 900, 545,  80, 20, 0|SS_CENTERIMAGE},
#if 1//2017.07.18
	{ 0, 0  , IDC_STATIC5 , 1, "\0(640.3,472.8)"   , 905, 445,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC7 , 1, "\0(0.3,-8.8)"      , 905, 485,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC6 , 1, "\0 12345 pix"      , 905, 525,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC8 , 1, "\0 120.05 pix"     , 905, 565,  75, 20, WS_BORDER|SS_CENTERIMAGE},
#else
	{ 0, 0  , IDC_STATIC5 , 1, "(640.3,472.8)"     , 905, 445,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC7 , 1, "(0.3,-8.8)"        , 905, 485,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC6 , 1, "12345 pix"         , 905, 525,  75, 20, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC8 , 1, "120.05 pix"        , 905, 565,  75, 20, WS_BORDER|SS_CENTERIMAGE},
#endif
//-----
#if 1//2015.09.10
	{ 0, 0  , IDC_CHECK2  , 7, "シェーディング補正", 990,  30, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
#endif
	{ 0, 0  , IDC_STATIC31, 5, "2値化画像"         , 990,  90, 140, 70, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 0  , IDC_STATIC34, 5, "測定回数"          , 990, 170, 140, 35},
	{ 0, 0  , IDC_COMBO1  , 5, "3"                 , 990, 205, 140, 35},
	{ 0, 0  , IDC_STATIC32, 5, "白地に黒"          , 990, 250, 140, 70, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 0  , IDC_STATIC33, 5, "2値化閾値"         , 990, 330, 140, 35, 0|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_EDIT1   , 5, "120"               , 990, 365, 140, 35, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_SPIN1   , 0, (LPCTSTR)(0|255<<16),   0,   0,   0,  0, UDS_SETBUDDYINT|UDS_ALIGNRIGHT},

	{ 0, 0  , IDC_BUTTON1 , 0, "測定クリア"        , 990, 460, 140, 30},

	{ 0, 0  , IDC_BUTTON7 , 0, "保存"              , 990, 500, 140, 50},
	{ 0, 0  , IDC_BUTTON8 , 0, "戻る"              , 990, 560, 140, 50},
//	{ 0, 8  , IDC_STATIC26, 0, "GRAPH"             ,  10, 518, 810, 90, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},//SS_WHITEFRAME},
#if 1//2018.05.01
	{ 0, 0  , IDC_STATIC60, 9, "ステージ移動"      ,1100+40, 170, 140, 20},
	{ 0, 0  , IDC_EDIT2   , 5, "9999"              ,1100+40, 190, 140, 30, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER|ES_READONLY},
	{ 0, 0  , IDC_STATIC59, 9, "mm"                ,1240+40, 195,  30, 20, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_BUTTON9 , 0, "原点移動"          ,1280+40, 185, 100, 40},
	{ 0, 0  , IDC_BUTTON10, 0, "CLEAR"             ,1300+40, 230,  70, 25},
	{ 0, 0  , IDC_BUTTON11, 0, "<JOG+"             ,1100+40, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON12, 0, "<STEP+"            ,1170+40, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON13, 0, "STEP->"            ,1240+40, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON14, 0, "JOG->"             ,1310+40, 260,  65, 40},
	{ 0, 0  , IDC_BUTTON15, 0, "オート検出"        ,1120+40, 305, 240, 50},
	{ 0, 0  , IDC_BUTTON16, 0, "連続測定"          ,1120+40, 360, 240, 50},
#endif
	{ 0, 0  , IDC_BUTTON17, 0, "全エリア消去"      , 685, 700, 240, 50},
	{ 0, 0  , IDC_STATIC  , 1, "保存ファイル名"    , 685, 610, 140, 20, 0|SS_CENTERIMAGE},
	{ 0, 0  , IDC_EDIT19  , 0, ""                  , 685, 630, 250, 30, WS_BORDER},
	{ 0, 0  ,            0, 0, NULL}
};
#if 1//2017.04.01
static
void CHECK_FIXED(CWnd *pWndForm)
{
	CArray<UINT, UINT> ar;

	if (CKOP::FP.CIRCLE_SAVPOS >= 0) {
		m_d.P_SAVEPOS = CKOP::FP.CIRCLE_SAVPOS;
		ar.Add(IDC_COMBO1);
	}
	if (CKOP::FP.CIRCLE_BINVAL >= 0) {
		m_d.P_BIN_VAL = CKOP::FP.CIRCLE_BINVAL;
		ar.Add(IDC_EDIT1);
	}
	if (CKOP::FP.CIRCLE_BINIMG >= 0) {
		m_d.P_BIN_IMG = CKOP::FP.CIRCLE_BINIMG;
		ar.Add(IDC_STATIC31);
	}
	if (CKOP::FP.CIRCLE_BAKMOD >= 0) {
		m_d.P_BAK_MOD = CKOP::FP.CIRCLE_BAKMOD;
		ar.Add(IDC_STATIC32);
	}
	if (CKOP::FP.CIRCLE_SHAD_CORR >= 0) {
		m_d.P_SHAD_CORR = CKOP::FP.CIRCLE_SHAD_CORR;
		ar.Add(IDC_CHECK2);
	}
#if 1//2018.05.01
	if (CKOP::IS_RECT_OK(&CKOP::FP.CIRCLE_AREA)) {
		for(int i = 0; i < 9; i++)
		{
			m_d.P_AREA[i] = CKOP::FP.CIRCLE_AREA;
		}
	}
#endif
	for (int i = 0; i < ar.GetCount(); i++) {
		pWndForm->GetDlgItem(ar[i])->EnableWindow(FALSE);
	}
}
#endif
#if 1//2018.05.01
/*static
void RECT_TO_CVRECT(LPRECT ri, void *rt)
{
	CvRect *ro = (CvRect*)rt;
	ro->x      = ri->left;
	ro->width  = ri->right  - ri->left;
	ro->y      = ri->top;
	ro->height = ri->bottom - ri->top;
}*/
#endif
void CKOP_CIRCLE_EX::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	m_d.P_SAVEPOS = GetProfileINT("CIRCLE", "SAVPOS" ,   0);
	m_d.P_BIN_VAL = GetProfileINT("CIRCLE", "BINVAL" , 128);
	m_d.P_BIN_IMG = GetProfileINT("CIRCLE", "BINIMG" ,   1);
	m_d.P_BAK_MOD = GetProfileINT("CIRCLE", "BAKMOD" ,   0);
#if 1//2015.09.10
	m_d.P_SHAD_CORR = GetProfileINT("CIRCLE", "SHAD_CORR" , 0);
#if 1//2018.05.01
	for(int i = 0; i < 9; i++)
	{
		m_d.P_AREA[i]    =*GetProfileRec("CIRCLE", "AREA"   ,  -1);
		if (TRUE) {
			CRect	rt(m_d.P_AREA[i]);
			if (rt.Width() <= 0 || rt.Width() > CKOP::BMP_WID
			 || rt.Height() <= 0 || rt.Height() > CKOP::BMP_HEI) {
				//エリア指定が無効な範囲なときは最大エリアに再設定する
				m_d.P_AREA[i].left = 0;
				m_d.P_AREA[i].top = 0;
				m_d.P_AREA[i].right = CKOP::BMP_WID;
				m_d.P_AREA[i].bottom = CKOP::BMP_HEI;
			}
		}
	}
#endif
#if 1//2017.04.01
	if (G_SS.USE_FIXED) {
		CKOP::GetProfileFixed();
		CHECK_FIXED(pWndForm);
	}
#endif
	if (!CKOP_SHADING::IS_AVAILABLE_CORRECTION() || !CKOP_SHADING_EX::IS_AVAILABLE_CORRECTION()) {
		 m_d.P_SHAD_CORR = 0;
		 pWndForm->GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	}
	pWndForm->CheckDlgButton(IDC_CHECK2, m_d.P_SHAD_CORR!=0);
#endif
	//-----
	((CComboBox*)pWndForm->GetDlgItem(IDC_COMBO1))->SetCurSel(m_d.P_SAVEPOS);
	//-----
	pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_BIN_VAL);

	for(int i = 0; i < 9; i++)
	{
		m_d.isEnable[i] = 0;
	}

	//-----
	UPDATE_LABELS(pWndForm);
	//-----


	m_d.R_CEN_X = CKOP::BMP_WID/2;
	m_d.R_CEN_Y = CKOP::BMP_HEI/2;
	//-----
	if (lstrcmp(CKOP::SERIALNO, m_d.B_SERIAL)) {
		lstrcpyn(m_d.B_SERIAL, CKOP::SERIALNO, sizeof(m_d.B_SERIAL));
#if 0
		for (int i = 0; i < ROWS(m_d.R_HBMP); i++) {
			if (m_d.R_HBMP[i] != NULL) {
				DeleteObject(m_d.R_HBMP[i]);
				m_d.R_HBMP[i] = NULL;
			}
		}
#else
		for(int j = 0; j < 9; j++)
		{
			for (int i = 0; i < ROWS(m_d.R_HBMP); i++) {
				if (m_d.R_HBMP[i][j] != NULL) {
					DeleteObject(m_d.R_HBMP[i][j]);
					m_d.R_HBMP[i][j] = NULL;
				}
			}
		}
#endif
	}
	else {
		for (int i = 0; i < ROWS(m_d.R_HBMP); i++) {
			if (TRUE/*m_d.R_HBMP[i] != NULL*/) {
				UPDATE_RESULT(pWndForm, i);
			}
		}
	}
	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}
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
	//初期化
	m_d.P_CUR_AREA = 0;
}

void CKOP_CIRCLE_EX::TERM_FORM(void)
{
	WriteProfileINT("CIRCLE", "SAVPOS" , m_d.P_SAVEPOS);
	WriteProfileINT("CIRCLE", "BINVAL" , m_d.P_BIN_VAL);
	WriteProfileINT("CIRCLE", "BINIMG" , m_d.P_BIN_IMG);
	WriteProfileINT("CIRCLE", "BAKMOD" , m_d.P_BAK_MOD);
#if 1//2015.09.10
	WriteProfileINT("CIRCLE", "SHAD_CORR"  , m_d.P_SHAD_CORR);
#endif
#if 1//2018.05.01
	for(int i = 0; i < 9; i++)
	{
		WriteProfileRec("CIRCLE", "AREA"   , &m_d.P_AREA[i]  );
	}
#endif
}

void CKOP_CIRCLE_EX::CALC_GRAV(CWnd *pWndForm, LPBYTE pImgPxl, int areaPos)
{
#if 1//2018.05.01
	CvRect	roi;
#endif
	m_d.P_BIN_VAL = pWndForm->GetDlgItemInt(IDC_EDIT1);
    //# 画像取得・変換
    //im = cv2.imread("test.png")                     # 入力画像の取得
    //gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)     # 入力画像をグレースケール変換
    //th = cv2.threshold(gray,127,255,0)[1]           # グレースケール画像の2値化
    //# 輪郭抽出
    //cnts = cv2.findContours(th,1,2)[0]              # グレースケール画像から輪郭検出
    //areas = [cv2.contourArea(cnt) for cnt in cnts]
    //cnt_max = [cnts[areas.index(max(areas))]][0]    # 最大の輪郭を抽出
    //# 最大面積領域の重心を計算する
    //M = cv2.moments(cnt_max)
    //(cx, cy) = ( int(M["m10"]/M["m00"]),int(M["m01"]/M["m00"]) )
    //cv2.circle(im,(cx,cy),5, (0,0,255), -1)         # 重心を赤円で描く
    //# 結果表示
    //cv2.imshow("Show Image",im)                     # 画像の表示
    //cv2.waitKey(0)                                  # キー入力待機
    //cv2.destroyAllWindows()     
	double	avg;
	IplImage *img_src = cvCreateImage(cvSize(CKOP::BMP_WID, CKOP::BMP_HEI), IPL_DEPTH_8U, CKOP::BMP_BYTE_CNT);
	IplImage *img_gry = cvCreateImage(cvSize(CKOP::BMP_WID, CKOP::BMP_HEI), IPL_DEPTH_8U, 1);
	IplImage *img_dst = cvCreateImage(cvSize(CKOP::BMP_WID, CKOP::BMP_HEI), IPL_DEPTH_8U, 1);
#if 1//2015.07.24
	if (CKOP::BMP_BYTE_CNT == 1) {
	//8bitカラーのときはグレースケールで格納されていると想定する
	memcpy(img_gry->imageData, pImgPxl, img_src->imageSize);
	}
	else {
#endif
	memcpy(img_src->imageData, pImgPxl, img_src->imageSize);

	cvCvtColor(img_src, img_gry, CV_BGR2GRAY);
#if 1//2015.07.24
	}
#endif
#if 1//2015.09.10
	if (CKOP::m_bBOTTOMUP) {
		cvFlip(img_gry, NULL, 0); // 水平軸で反転（垂直反転）
	}
#endif
	int	nBakMod = m_d.P_BAK_MOD;
	if (nBakMod == 2) {
		//自動認識
		avg = cvAvg(img_gry).val[0];
TRACE("CALC_GRAV(AVG:%.0lf)\n", avg);
		if (avg <= m_d.P_BIN_VAL) {
			nBakMod = 1;//黒背景
		}
		else {
			nBakMod = 0;//白背景
		}
	}
	if (!nBakMod) {
		cvThreshold(img_gry, img_dst, m_d.P_BIN_VAL, 255, CV_THRESH_BINARY_INV);	//白背景に黒丸の時は反転しておく
	}
	else {
		cvThreshold(img_gry, img_dst, m_d.P_BIN_VAL, 255, CV_THRESH_BINARY);		//黒背景に白丸
	}
	try {
#if 1//2018.05.01
		RECT	rt = m_d.P_AREA[areaPos];
		CKOP::NORMALIZE_RECT(&rt);
		CKOP::RECT_TO_CVRECT(&rt, &roi);
		if (roi.width <= 0) {
			roi.width = 1;
		}
		if (roi.height <= 0) {
			roi.height = 1;
		}
		cvSetImageROI(img_dst, roi);
#endif
		CvMemStorage *storage = cvCreateMemStorage(0);
		CvSeq *contours = NULL;

		//抽出モード:
		//		CV_RETR_EXTERNAL:最も外側の輪郭のみ抽出
		//		CV_RETR_LIST	:全ての輪郭を抽出し，リストに追加
		//		CV_RETR_CCOMP	:全ての輪郭を抽出し，二つのレベルを持つ階層構造を構成する．
		//						:1番目のレベルは連結成分の外側の境界線，
		//						:2番目のレベルは穴（連結成分の内側に存在する）の境界線．
		//		CV_RETR_TREE	:全ての輪郭を抽出し，枝分かれした輪郭を完全に表現する階層構造を構成する．
		//----------------------------------
		//		CV_CHAIN_APPROX_SIMPLE	:輪郭の折れ線の端点を取得
		//		CV_CHAIN_APPROX_NONE	:輪郭の全ての点を取得,Teh-Chinチェーンの近似アルゴリズム中の一つを適用する 
		//		CV_CHAIN_APPROX_TC89_L1
		//		CV_CHAIN_APPROX_TC89_KCOS

		int find_contour_num = cvFindContours (
									img_dst,                     // 入力画像
									storage,                      // 抽出された輪郭を保存する領域
									&contours,                  // 一番外側の輪郭へのポインタへのポインタ
									sizeof (CvContour),      // シーケンスヘッダのサイズ
#if 1
									CV_RETR_EXTERNAL,
//									CV_CHAIN_APPROX_SIMPLE
									CV_CHAIN_APPROX_NONE
#else
									CV_RETR_TREE,
									CV_CHAIN_APPROX_NONE
#endif
			);


		if (contours != NULL){
			//輪郭の描画
			//cvDrawContours(img_src, contours, CV_RGB(255,0,0), CV_RGB(0,0,255), 2, 2);

			double	fmax = -1e99;
			CvSeq*	pmax = NULL;
			CvSeq*	pos = contours;
			while (pos != NULL) {
				double	f = cvContourArea(pos);
				if (fmax < f) {
					fmax = f;
					pmax = pos;
				}
				pos = pos->h_next;
			}
			//輪郭に隣接する矩形の取得
            CvRect		rt = cvBoundingRect(pmax);
			CvMoments	moments;

			cvMoments(pmax, &moments);
			m_d.R_COG_X[0][areaPos] = moments.m10/moments.m00;
			m_d.R_COG_Y[0][areaPos] = moments.m01/moments.m00;
#if 1//2018.05.01
			m_d.R_COG_X[0][areaPos]+= roi.x;
			m_d.R_COG_Y[0][areaPos]+= roi.y;
#endif
			m_d.R_DIF_X[0][areaPos] = m_d.R_COG_X[0][areaPos] - m_d.R_CEN_X;
			m_d.R_DIF_Y[0][areaPos] = m_d.R_COG_Y[0][areaPos] - m_d.R_CEN_Y;
			
			m_d.R_SQAR [0][areaPos] = fmax;



#if 0
			CEN_X = rt.x + rt.width/2;
			CEN_Y = rt.y + rt.height/2;
#endif
			m_d.R_PHI[0][areaPos] = 2 * sqrt(m_d.R_SQAR[0][areaPos]/3.141592);	//直径 L = 2r = 2 √(S/π)
		}
		//メモリストレージの解放
		cvReleaseMemStorage (&storage);
	}
	catch (cv::Exception ex) {
#if 1//2018.05.01
		mlog("%s", ex.msg);
#endif
	}
#if 1//2018.05.01
	cvResetImageROI(img_dst);
#endif

	if (m_d.P_BIN_IMG) {
#if 1//2015.09.10
		if (CKOP::m_bBOTTOMUP) {
			cvFlip(img_gry, NULL, 0); // 水平軸で反転（垂直反転）
		}
#endif
		//表示上は背景の反転はしないで表示するため
		cvThreshold(img_gry, img_dst, m_d.P_BIN_VAL, 255, CV_THRESH_BINARY);
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

	
	cvReleaseImage(&img_dst);
	cvReleaseImage(&img_gry);
	cvReleaseImage(&img_src);

}
void CKOP_CIRCLE_EX::DRAW_MARKER(CDC *pDC, int xx , int yy, int gap)
{

	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pDC->MoveTo(xx-gap  , yy    );
	pDC->LineTo(xx+gap+1, yy    );

	pDC->MoveTo(xx    , yy-gap  );
	pDC->LineTo(xx    , yy+gap+1);

	pDC->SelectObject(old_pen);
}

void CKOP_CIRCLE_EX::DRAW_MARKER(CDC *pDC)
{
	const
	int		gap = 5;
	for(int i = 0; i < 9; i++)
	{
		if(m_d.isEnable[i] == 0)
		{
			continue;
		}

		CPen* old_pen;
		if(i == 0)	   old_pen = pDC->SelectObject(&CKOP::m_penRed);
		else if(i == 1)old_pen = pDC->SelectObject(&CKOP::m_penGreen);
		else if(i == 2)old_pen = pDC->SelectObject(&CKOP::m_penYellow);
		else if(i == 3)old_pen = pDC->SelectObject(&CKOP::m_penPink);
		else if(i == 4)old_pen = pDC->SelectObject(&CKOP::m_penWater);
		else if(i == 5)old_pen = pDC->SelectObject(&CKOP::m_penBlue);
		else if(i == 6)old_pen = pDC->SelectObject(&CKOP::m_penOrange);
		else if(i == 7)old_pen = pDC->SelectObject(&CKOP::m_penBrown);
		else if(i == 8)old_pen = pDC->SelectObject(&CKOP::m_penViolet);

		int		xx = (int)(m_d.R_COG_X[0][i]/CKOP::RX);
		int		yy = (int)(m_d.R_COG_Y[0][i]/CKOP::RY);

		pDC->MoveTo(xx-gap  , yy    );
		pDC->LineTo(xx+gap+1, yy    );

		pDC->MoveTo(xx    , yy-gap  );
		pDC->LineTo(xx    , yy+gap+1);

		pDC->SelectObject(old_pen);
	}
}
void CKOP_CIRCLE_EX::DRAW_MARKER(CWnd *pWnd)
{
	CDC*	pDC = pWnd->GetDC();
#if 0
	const
	int		gap = 5;
	int		xx = (int)(m_d.R_COG_X[0]/CKOP::RX);
	int		yy = (int)(m_d.R_COG_Y[0]/CKOP::RY);

	DRAW_MARKER(pDC, xx, yy, gap);
#else
	DRAW_MARKER(pDC);
#endif

#if 1//2018.05.01
	if (TRUE) {
		for(int i = 0; i < 9; i++)
		{
			if(m_d.isEnable[i] == 0)
			{
				continue;
			}

			CPen* old_pen;
			if(i == 0)	   old_pen = pDC->SelectObject(&CKOP::m_penRed);
			else if(i == 1)old_pen = pDC->SelectObject(&CKOP::m_penGreen);
			else if(i == 2)old_pen = pDC->SelectObject(&CKOP::m_penYellow);
			else if(i == 3)old_pen = pDC->SelectObject(&CKOP::m_penPink);
			else if(i == 4)old_pen = pDC->SelectObject(&CKOP::m_penWater);
			else if(i == 5)old_pen = pDC->SelectObject(&CKOP::m_penBlue);
			else if(i == 6)old_pen = pDC->SelectObject(&CKOP::m_penOrange);
			else if(i == 7)old_pen = pDC->SelectObject(&CKOP::m_penBrown);
			else if(i == 8)old_pen = pDC->SelectObject(&CKOP::m_penViolet);

			CKOP::DRAW_MARQUEE(pDC, CKOP::BMPCD_TO_IMGCD(&m_d.P_AREA[i]));
			pDC->SelectObject(old_pen);
		}

	}
#endif
	pWnd->ReleaseDC(pDC);
}

void CKOP_CIRCLE_EX::UPDATE_LABELS(CWnd *pWndForm, int mask)
{
	if (mask & 1) {
		if (m_d.P_BIN_IMG != 0) {
			pWndForm->SetDlgItemText(IDC_STATIC31, "2値化画像");
		}
		else {
			pWndForm->SetDlgItemText(IDC_STATIC31, "リアル画像");
		}
	}
	if (mask & 2) {
		switch (m_d.P_BAK_MOD) {
		case  1: pWndForm->SetDlgItemText(IDC_STATIC32, "黒地に白");break;
		case  2: pWndForm->SetDlgItemText(IDC_STATIC32, "自動認識");break;
		case  0:
		default: pWndForm->SetDlgItemText(IDC_STATIC32, "白地に黒");break;
		}
	}
	if (mask & 4) {
#if 0
		CString buf;
		buf.Format("エリア(%d,%d)-(%d,%d)", m_d.P_AREA.left, m_d.P_AREA.top, m_d.P_AREA.right, m_d.P_AREA.bottom);
		pWndForm->SetDlgItemText(IDC_STATIC23, buf);
#else
		CString buf;
		for(int j = 0; j < 9; j++)
		{
			if(m_d.isEnable[j] == 0)
			{
				buf.Format("");
			}
			else
			{
				buf.Format("エリア(%d,%d)-(%d,%d)", m_d.P_AREA[j].left, m_d.P_AREA[j].top, m_d.P_AREA[j].right, m_d.P_AREA[j].bottom);
			}

			if(j == 0)		pWndForm->SetDlgItemText(IDC_STATIC23, buf);
			else if(j == 1)	pWndForm->SetDlgItemText(IDC_STATIC66, buf);
			else if(j == 2)	pWndForm->SetDlgItemText(IDC_STATIC72, buf);
			else if(j == 3)	pWndForm->SetDlgItemText(IDC_STATIC78, buf);
			else if(j == 4)	pWndForm->SetDlgItemText(IDC_STATIC84, buf);
			else if(j == 5)	pWndForm->SetDlgItemText(IDC_STATIC90, buf);
			else if(j == 6)	pWndForm->SetDlgItemText(IDC_STATIC96, buf);
			else if(j == 7)	pWndForm->SetDlgItemText(IDC_STATIC102, buf);
			else if(j == 8)	pWndForm->SetDlgItemText(IDC_STATIC108, buf);
		}
#endif
	}
	if (mask & 8) {
	}
}

void CKOP_CIRCLE_EX::UPDATE_RESULT(CWnd *pWndForm, int idx)
{
	//----------
	CString	buf1, buf2, buf3, buf4;
	CString	buf5, buf6, buf7, buf8;
	CString	buf9, buf10, buf11, buf12;
	CString	buf13, buf14, buf15, buf16;
	CString	buf17, buf18, buf19, buf20;
	CString	buf21, buf22, buf23, buf24;
	CString	buf25, buf26, buf27, buf28;
	CString	buf29, buf30, buf31, buf32;
	CString	buf33, buf34, buf35, buf36;
	int		i=idx;
	
	if (i > 0 && m_d.R_HBMP[i][m_d.P_CUR_AREA] == NULL) {
	}
	else if (i > 0) {
	buf1.Format("%.1lf,%.1lf", m_d.R_COG_X[i][m_d.P_CUR_AREA], m_d.R_COG_Y[i][m_d.P_CUR_AREA]);
	buf2.Format("%.0lf pix", m_d.R_SQAR[i][m_d.P_CUR_AREA]);
	buf3.Format("%4.1lf,%4.1lf", m_d.R_DIF_X[i][m_d.P_CUR_AREA], m_d.R_DIF_Y[i][m_d.P_CUR_AREA]);
	buf4.Format("%.2lf pix", m_d.R_PHI[i][m_d.P_CUR_AREA]);
	}
	else {// i == 0
#if 0
	buf1.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i], m_d.R_COG_Y[i]);
	buf2.Format("面積 %.0lf pix", m_d.R_SQAR[i]);
	buf3.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i], m_d.R_DIF_Y[i]);
	buf4.Format("直径  %.2lf pix", m_d.R_PHI[i]);
#else
		for(int j = 0; j < 9; j++)
		{
			if(m_d.isEnable[j] == 0)
			{
				continue;
			}

			if(j == 0)
			{
				buf1.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf2.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf3.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf4.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 1)
			{
				buf5.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf6.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf7.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf8.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 2)
			{
				buf9.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf10.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf11.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf12.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 3)
			{
				buf13.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf14.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf15.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf16.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 4)
			{
				buf17.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf18.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf19.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf20.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 5)
			{
				buf21.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf22.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf23.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf24.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 6)
			{
				buf25.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf26.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf27.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf28.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 7)
			{
				buf29.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf30.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf31.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf32.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
			else if(j == 8)
			{
				buf33.Format("重心 (%.1lf,%.1lf)", m_d.R_COG_X[i][j], m_d.R_COG_Y[i][j]);
				buf34.Format("面積 %.0lf pix", m_d.R_SQAR[i][j]);
				buf35.Format("中心からのズレ(%4.1lf,%4.1lf)", m_d.R_DIF_X[i][j], m_d.R_DIF_Y[i][j]);
				buf36.Format("直径  %.2lf pix", m_d.R_PHI[i][j]);
			}
		}
#endif
	}
	switch (i) {
	case 0:
#if 0
		pWndForm->GetDlgItem(IDC_STATIC19)->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC20)->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC22)->SetWindowText(buf4);
#else
		pWndForm->GetDlgItem(IDC_STATIC19)->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC20)->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC21)->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC22)->SetWindowText(buf4);

		pWndForm->GetDlgItem(IDC_STATIC62)->SetWindowText(buf5);
		pWndForm->GetDlgItem(IDC_STATIC63)->SetWindowText(buf6);
		pWndForm->GetDlgItem(IDC_STATIC64)->SetWindowText(buf7);
		pWndForm->GetDlgItem(IDC_STATIC65)->SetWindowText(buf8);

		pWndForm->GetDlgItem(IDC_STATIC68)->SetWindowText(buf9);
		pWndForm->GetDlgItem(IDC_STATIC69)->SetWindowText(buf10);
		pWndForm->GetDlgItem(IDC_STATIC70)->SetWindowText(buf11);
		pWndForm->GetDlgItem(IDC_STATIC71)->SetWindowText(buf12);

		pWndForm->GetDlgItem(IDC_STATIC74)->SetWindowText(buf13);
		pWndForm->GetDlgItem(IDC_STATIC75)->SetWindowText(buf14);
		pWndForm->GetDlgItem(IDC_STATIC76)->SetWindowText(buf15);
		pWndForm->GetDlgItem(IDC_STATIC77)->SetWindowText(buf16);

		pWndForm->GetDlgItem(IDC_STATIC80)->SetWindowText(buf17);
		pWndForm->GetDlgItem(IDC_STATIC81)->SetWindowText(buf18);
		pWndForm->GetDlgItem(IDC_STATIC82)->SetWindowText(buf19);
		pWndForm->GetDlgItem(IDC_STATIC83)->SetWindowText(buf20);

		pWndForm->GetDlgItem(IDC_STATIC86)->SetWindowText(buf21);
		pWndForm->GetDlgItem(IDC_STATIC87)->SetWindowText(buf22);
		pWndForm->GetDlgItem(IDC_STATIC88)->SetWindowText(buf23);
		pWndForm->GetDlgItem(IDC_STATIC89)->SetWindowText(buf24);

		pWndForm->GetDlgItem(IDC_STATIC92)->SetWindowText(buf25);
		pWndForm->GetDlgItem(IDC_STATIC93)->SetWindowText(buf26);
		pWndForm->GetDlgItem(IDC_STATIC94)->SetWindowText(buf27);
		pWndForm->GetDlgItem(IDC_STATIC95)->SetWindowText(buf28);

		pWndForm->GetDlgItem(IDC_STATIC98)->SetWindowText(buf29);
		pWndForm->GetDlgItem(IDC_STATIC99)->SetWindowText(buf30);
		pWndForm->GetDlgItem(IDC_STATIC100)->SetWindowText(buf31);
		pWndForm->GetDlgItem(IDC_STATIC101)->SetWindowText(buf32);

		pWndForm->GetDlgItem(IDC_STATIC104)->SetWindowText(buf33);
		pWndForm->GetDlgItem(IDC_STATIC105)->SetWindowText(buf34);
		pWndForm->GetDlgItem(IDC_STATIC106)->SetWindowText(buf35);
		pWndForm->GetDlgItem(IDC_STATIC107)->SetWindowText(buf36);
#endif
	break;
	case 1:
		pWndForm->GetDlgItem(IDC_STATIC27)->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC28)->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC29)->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC30)->SetWindowText(buf4);
		((CStatic*)pWndForm->GetDlgItem(IDC_STATIC13))->SetBitmap(m_d.R_HBMP[i][m_d.P_CUR_AREA]);
	break;
	case 2:
		pWndForm->GetDlgItem(IDC_STATIC1 )->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC2 )->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC3 )->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC4 )->SetWindowText(buf4);
		((CStatic*)pWndForm->GetDlgItem(IDC_STATIC15))->SetBitmap(m_d.R_HBMP[i][m_d.P_CUR_AREA]);
	break;
	case 3:
		pWndForm->GetDlgItem(IDC_STATIC5 )->SetWindowText(buf1);
		pWndForm->GetDlgItem(IDC_STATIC6 )->SetWindowText(buf2);
		pWndForm->GetDlgItem(IDC_STATIC7 )->SetWindowText(buf3);
		pWndForm->GetDlgItem(IDC_STATIC8 )->SetWindowText(buf4);
		((CStatic*)pWndForm->GetDlgItem(IDC_STATIC17))->SetBitmap(m_d.R_HBMP[i][m_d.P_CUR_AREA]);
	break;
	}
}

void CKOP_CIRCLE_EX::SAVE_RESULT(CWnd *pWndForm)
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
	m_d.R_COG_X[i][m_d.P_CUR_AREA] = m_d.R_COG_X[0][m_d.P_CUR_AREA];
	m_d.R_COG_Y[i][m_d.P_CUR_AREA] = m_d.R_COG_Y[0][m_d.P_CUR_AREA];
	m_d.R_DIF_X[i][m_d.P_CUR_AREA] = m_d.R_DIF_X[0][m_d.P_CUR_AREA];
	m_d.R_DIF_Y[i][m_d.P_CUR_AREA] = m_d.R_DIF_Y[0][m_d.P_CUR_AREA];
	m_d.R_SQAR [i][m_d.P_CUR_AREA] = m_d.R_SQAR [0][m_d.P_CUR_AREA];
	m_d.R_PHI  [i][m_d.P_CUR_AREA] = m_d.R_PHI  [0][m_d.P_CUR_AREA];
	//-----
	pWnd->GetWindowRect(&rt);
	w = rt.Width();
	h = rt.Height();
	// メモリDCにpWndをコピー
	bmp = ::CreateCompatibleBitmap(pDC->m_hDC, WW, HH);
	mdc.CreateCompatibleDC(pDC);
	old_bmp = (HBITMAP)mdc.SelectObject(bmp);
	int	NH = (h * WW / w),
		NW;

	if (NH < HH) {
		YY = (HH-NH)/2;
		HH = NH;
	}
	else {
		NW = (w * HH / h),
		XX = (WW-NW)/2;
		WW = NW;
	}
	mdc.StretchBlt(XX, YY, WW, HH, pDC, 0, 0, w, h, SRCCOPY);
	if (TRUE) {
		int		xx = (int)(m_d.R_COG_X[0][m_d.P_CUR_AREA]/CKOP::RX);
		int		yy = (int)(m_d.R_COG_Y[0][m_d.P_CUR_AREA]/CKOP::RY);

		xx = (int)(XX + xx * (double)WW / (double)w);
		yy = (int)(YY + yy * (double)HH / (double)h);
		DRAW_MARKER(&mdc, xx, yy, 3);
	}
	mdc.SelectObject(old_bmp);
	pWnd->ReleaseDC(pDC);

	if (m_d.R_HBMP[i][m_d.P_CUR_AREA] != NULL) {
		UINT	nIDS[] = {
			0, IDC_STATIC13, IDC_STATIC15, IDC_STATIC17
		};
		CStatic* pSta = (CStatic*)pWndForm->GetDlgItem(nIDS[i]);
		
		if (pSta != NULL) {
			pSta->SetBitmap(NULL);
		}
		DeleteObject(m_d.R_HBMP[i][m_d.P_CUR_AREA]);
		m_d.R_HBMP[i][m_d.P_CUR_AREA] = NULL;
	}
	m_d.R_HBMP[i][m_d.P_CUR_AREA] = bmp;
	UPDATE_RESULT(pWndForm, i);
	::MessageBeep(MB_OK);
}


void CKOP_CIRCLE_EX::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_COMBO1);
	if (pWnd == NULL) {
		return;
	}
	m_d.P_SAVEPOS = ((CComboBox*)pWnd)->GetCurSel();

#if 1//2015.09.10
	m_d.P_SHAD_CORR = pWndForm->IsDlgButtonChecked(IDC_CHECK2);
	if (m_d.P_SHAD_CORR) {
		CKOP_SHADING::DO_CORRECTION(pImgPxl);
		CKOP_SHADING_EX::DO_CORRECTION(pImgPxl);
	}
#endif
#if 0
	CALC_GRAV(pWndForm, pImgPxl);
#else
	for(int i = 0; i < 9; i++)
	{
		if(m_d.isEnable[i] == 0)
		{
			continue;
		}
		CALC_GRAV(pWndForm, pImgPxl, i);
	}
#endif
}

void CKOP_CIRCLE_EX::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);

	DRAW_MARKER(pWnd);
	UPDATE_RESULT(pWndForm);
#if 1//2018.05.01
	if (m_s.GET_TRIG) {
		STORE_STG_RESULT();
	}
#endif
}

BOOL CKOP_CIRCLE_EX::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON7://保存
#if 0
			CKOP::SAVE_WINDOW(AfxGetMainWnd(), "center.png");
#else
			CKOP::SAVE_WINDOW(pWndForm, "center.png");
#endif

#if 1//2017.07.18
			{
				CCSV	csv;
				SET_CSV(&csv);
				CKOP::SAVE_CSV(pWndForm, &csv, "center.csv");
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
					0, IDC_STATIC13, IDC_STATIC15, IDC_STATIC17
				};
				for (int i = 0; i < ROWS(nIDS); i++) {
					CStatic* pSta = (CStatic*)pWndForm->GetDlgItem(nIDS[i]);
					if (pSta != NULL) {
						pSta->SetBitmap(NULL);
						CDC*	pDC = pSta->GetWindowDC();
						CRect	rt;
						pDC->GetClipBox(&rt);
						rt.DeflateRect(1,1);
						pDC->FillSolidRect(&rt, GetSysColor(COLOR_3DFACE));
						pSta->ReleaseDC(pDC);
					}
				}

#if 0
				for (int i = 1; i < ROWS(m_d.R_COG_X); i++) {
					m_d.R_COG_X[i] = C_NAN();
					m_d.R_COG_Y[i] = C_NAN();
					m_d.R_DIF_X[i] = C_NAN();
					m_d.R_DIF_Y[i] = C_NAN();
					m_d.R_SQAR[i]  = C_NAN();
					m_d.R_PHI[i]   = C_NAN();
					//---
					if (m_d.R_HBMP[i] != NULL) {
						DeleteObject(m_d.R_HBMP[i]);
					}
					m_d.R_HBMP[i] = NULL;
					UPDATE_RESULT(pWndForm, i);
				}
#else
				for(int j = 0; j < 9; j++)
				{
					for (int i = 1; i < ROWS(m_d.R_COG_X); i++) {
						m_d.R_COG_X[i][j] = C_NAN();
						m_d.R_COG_Y[i][j] = C_NAN();
						m_d.R_DIF_X[i][j] = C_NAN();
						m_d.R_DIF_Y[i][j] = C_NAN();
						m_d.R_SQAR[i][j]  = C_NAN();
						m_d.R_PHI[i][j]   = C_NAN();
						//---
						if (m_d.R_HBMP[i][j] != NULL) {
							DeleteObject(m_d.R_HBMP[i][j]);
						}
						m_d.R_HBMP[i][j] = NULL;
						UPDATE_RESULT(pWndForm, i);
					}
				}
				pWndForm->Invalidate();
#endif
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
			m_s.R_MIN_IDX = 0;
			m_s.R_MIN = C_PINF();

			CKOP::DLG_PRG.Create("", AfxGetMainWnd(), TRUE, "オート検出実行中");
			STG::MOVE_ABS(1, -STG::DS_TBL[0].MESRANGE);

		break;
		case IDC_BUTTON16://連続測定
			GetLocalTime(&m_s.R_TIME);
			m_s.STG_STAT = 2;
			m_s.MES_STEP = (STG::DS_TBL[0].MESRANGE*2)/(STG::DATA_COUNT-1);
			m_s.GET_TRIG = 0;
			m_s.R_IDX = 0;
			m_s.R_MIN_IDX = 0;
			m_s.R_MIN = C_PINF();

			CKOP::DLG_PRG.Create("", AfxGetMainWnd(), TRUE, "連続測定実行中");
			STG::MOVE_ABS(1, -STG::DS_TBL[0].MESRANGE);
		break;
#endif

		case IDC_BUTTON17://全エリア消去
			for(int i = 0; i < 9; i++)
			{
				m_d.isEnable[i] = 0;
			}
			UPDATE_LABELS(pWndForm, 4);
		break;

		}
	break;
	}
	return(FALSE);
}

BOOL CKOP_CIRCLE_EX::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
{
	CWnd*	pWndImg;
	RECT targetRect;
	RECT afterRect;
	LPRECT pp;

	UINT	nID;
#if 1//2018.05.01
	POINT	pnt;
#endif
	if (pWndForm->m_hWnd == pMsg->hwnd) {
		nID = IDC_IMAGEAREA;
	}
	else {
		nID = ::GetDlgCtrlID(pMsg->hwnd);
	}
	switch (pMsg->message) {
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN) {
//			::MessageBeep(MB_ICONHAND);
//			::MessageBeep(MB_ICONQUESTION);
//			::MessageBeep(MB_ICONEXCLAMATION);
			SAVE_RESULT(pWndForm);
			::MessageBeep(MB_ICONASTERISK);
			return(TRUE);
		}
	break;
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
	case IDC_STATIC31:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			m_d.P_BIN_IMG = !m_d.P_BIN_IMG;
			UPDATE_LABELS(pWndForm, 1);
		break;
		}
	break;
	case IDC_STATIC32:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			if (++m_d.P_BAK_MOD > 2) {
				m_d.P_BAK_MOD = 0;
			}
			UPDATE_LABELS(pWndForm, 2);
pWndForm->Invalidate();
		break;
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
	case IDC_IMAGEAREA:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
			pnt.x = GET_X_LPARAM(pMsg->lParam);
			pnt.y = GET_Y_LPARAM(pMsg->lParam);

			pWndImg = pWndForm->GetDlgItem(IDC_IMAGEAREA);
			for(int i = 0; i < 9; i++)
			{
				if(m_d.isEnable[i] == 0)
				{
					continue;
				}

				targetRect = m_d.P_AREA[i];

				pp = CKOP::BMPCD_TO_IMGCD(&targetRect);

				afterRect.top = pp->top;
				afterRect.bottom = pp->bottom;
				afterRect.left = pp->left;
				afterRect.right = pp->right;
				
				if(pp->bottom < pp->top)//座標系の置換
				{
					afterRect.top = pp->bottom;
					afterRect.bottom = pp->top;
				}
				if(pp->right < pp->left)
				{
					afterRect.left = pp->right;
					afterRect.right = pp->left;
				}

				if(PtInRect(&afterRect, pnt))//範囲内にあるか判断
				{
					m_d.isEnable[i] = 0;
					UPDATE_LABELS(pWndForm, 4);
				}
			}

		break;
		case WM_RBUTTONDOWN:
			pnt.x = GET_X_LPARAM(pMsg->lParam);
			pnt.y = GET_Y_LPARAM(pMsg->lParam);
			if (G_SS.USE_FIXED && CKOP::IS_RECT_OK(&CKOP::FP.CIRCLE_AREA)) {
				//固定モード時はマウスによるエリア設定は行わない
			}
			else if (CKOP::IMG_RECT.PtInRect(pnt)) {
				CKOP::ToClinet(pWndForm->m_hWnd, ::GetDlgItem(pWndForm->m_hWnd, IDC_IMAGEAREA), &pnt);

				for(int i = 0; i < 9; i++)
				{
					if(m_d.isEnable[i] == 0)
					{
						m_d.P_CUR_AREA = i;
						break;
					}
				}
				m_d.isEnable[m_d.P_CUR_AREA] = 1;

				CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_AREA[m_d.P_CUR_AREA].left);
				CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_AREA[m_d.P_CUR_AREA].right);
				m_nCaptured = TRUE;
				UPDATE_LABELS(pWndForm, 4);
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
				CKOP::IMGCD_TO_BMPCD(&pnt, (LPPOINT)&m_d.P_AREA[m_d.P_CUR_AREA].right);
				UPDATE_LABELS(pWndForm, 4);
pWndForm->Invalidate(FALSE);
			}
		break;
		case WM_RBUTTONUP:
			ReleaseCapture();
			m_nCaptured = 0;
		break;
		}
	break;
#endif
	}
	return(FALSE);
}

HBRUSH CKOP_CIRCLE_EX::CTL_COLOR(CDC *pDC, CWnd* pWnd)
{
	UINT nID = pWnd->GetDlgCtrlID();

	switch (nID) {
	case IDC_STATIC18:	case IDC_STATIC19:	case IDC_STATIC20:	case IDC_STATIC21:	case IDC_STATIC22:	case IDC_STATIC23:
		pDC->SetTextColor(RGB(255, 0, 0));//Red
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC61:	case IDC_STATIC62:	case IDC_STATIC63:	case IDC_STATIC64:	case IDC_STATIC65:	case IDC_STATIC66:
		pDC->SetTextColor(RGB(0, 255, 0));//Green
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC67:	case IDC_STATIC68:	case IDC_STATIC69:	case IDC_STATIC70:	case IDC_STATIC71:	case IDC_STATIC72:
		pDC->SetTextColor(RGB(255, 255, 0));//Yellow
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC73:	case IDC_STATIC74:	case IDC_STATIC75:	case IDC_STATIC76:	case IDC_STATIC77:	case IDC_STATIC78:
		pDC->SetTextColor(RGB(255, 0, 255));//Pink
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC79:	case IDC_STATIC80:	case IDC_STATIC81:	case IDC_STATIC82:	case IDC_STATIC83:	case IDC_STATIC84:
		pDC->SetTextColor(RGB(0, 255, 255));//Water
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC85:	case IDC_STATIC86:	case IDC_STATIC87:	case IDC_STATIC88:	case IDC_STATIC89:	case IDC_STATIC90:
		pDC->SetTextColor(RGB(0, 0, 255));//Blue
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC91:	case IDC_STATIC92:	case IDC_STATIC93:	case IDC_STATIC94:	case IDC_STATIC95:	case IDC_STATIC96:
		pDC->SetTextColor(RGB(255, 127, 0));//Orange
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC97:	case IDC_STATIC98:	case IDC_STATIC99:	case IDC_STATIC100:	case IDC_STATIC101:	case IDC_STATIC102:
		pDC->SetTextColor(RGB(152, 38, 0));//Brown
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	case IDC_STATIC103:	case IDC_STATIC104:	case IDC_STATIC105:	case IDC_STATIC106:	case IDC_STATIC107:	case IDC_STATIC108:
		pDC->SetTextColor(RGB(127, 0, 255));//Violet
		pDC->SetBkColor(RGB(204, 204, 204));
	break;
	default:
	break;
	}

	return NULL;
}

#if 1//2017.07.18
void CKOP_CIRCLE_EX::SET_CSV(CCSV *pc)
{
	CString	buf;
	LPCTSTR pHead0[] = {
		"", "重心(x)", "重心(y)", "中心からのズレ(x)", "ズレ(y)",
		"面積(pix)", "直径(pix)"
	};
	LPCTSTR	pHead1[] = {
		"測定結果", "1回目", "2回目", "3回目"
	};
	int		r = 0;
	pc->set( 0, r, "円の面積＆重心");
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
#if 0
	for (int i = 0; i < ROWS(pHead1); i++) {
		for (int c = 0; c < ROWS(pHead0); c++) {
			switch (c) {
			case 0: buf = pHead1[i]; break;
			case 1: buf = F1S(m_d.R_COG_X[i]); break;
			case 2: buf = F1S(m_d.R_COG_Y[i]); break;
			case 3: buf = F1S(m_d.R_DIF_X[i]); break;
			case 4: buf = F1S(m_d.R_DIF_Y[i]); break;
			case 5: buf = F0S(m_d.R_SQAR [i]); break;
			case 6: buf = F2S(m_d.R_PHI  [i]); break;
			}
			pc->set(c, r+i, buf);
		}
	}
#else
	for (int j = 0; j < 9; j++) {
		for (int i = 0; i < ROWS(pHead1); i++) {
			for (int c = 0; c < ROWS(pHead0); c++) {
				CString	num;
				num.Format("エリア%d", j+1);

				switch (c) {
				case 0: buf = pHead1[i] + num; break;
				case 1: buf = F1S(m_d.R_COG_X[i][j]); break;
				case 2: buf = F1S(m_d.R_COG_Y[i][j]); break;
				case 3: buf = F1S(m_d.R_DIF_X[i][j]); break;
				case 4: buf = F1S(m_d.R_DIF_Y[i][j]); break;
				case 5: buf = F0S(m_d.R_SQAR [i][j]); break;
				case 6: buf = F2S(m_d.R_PHI  [i][j]); break;
				}
				pc->set(c, r+i, buf);
			}
		}
		r+=4;
	}
#endif
	return;
}
#endif
#if 1//2018.05.01
void CKOP_CIRCLE_EX::TIMER_PROC(CWnd* pWndForm)
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
			double pos = -STG::DS_TBL[0].MESRANGE + m_s.MES_STEP * m_s.R_MIN_IDX;
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
void CKOP_CIRCLE_EX::STORE_STG_RESULT(void)
{
	if ((GetTickCount()-m_s.CUR_TIC) >= G_SS.STG_WAIT_TIME) {
		int	j = m_s.R_IDX;
		if (j < ROWS(m_s.R_POS)) {
			m_s.R_POS  [j] = STG::POSITION[0];
#if 0
			m_s.R_COG_X[j] = m_d.R_COG_X[0][0];
			m_s.R_COG_Y[j] = m_d.R_COG_Y[0][0];
			m_s.R_SQAR [j] = m_d.R_SQAR[0][0];
			m_s.R_DIF_X[j] = m_d.R_DIF_X[0][0];
			m_s.R_DIF_Y[j] = m_d.R_DIF_Y[0][0];
			m_s.R_PHI  [j] = m_d.R_PHI  [0][0];
			if (m_s.R_MIN > m_d.R_SQAR[0][0]) {
				m_s.R_MIN = m_d.R_SQAR[0][0];
				m_s.R_MIN_IDX = m_s.R_IDX;
			}
#else
			for(int kk = 0; kk < 9; kk++)
			{
				if(m_d.isEnable[kk] == 0)
				{
					continue;
				}

				m_s.R_COG_X[j][kk] = m_d.R_COG_X[0][kk];
				m_s.R_COG_Y[j][kk] = m_d.R_COG_Y[0][kk];
				m_s.R_SQAR [j][kk] = m_d.R_SQAR[0][kk];
				m_s.R_DIF_X[j][kk] = m_d.R_DIF_X[0][kk];
				m_s.R_DIF_Y[j][kk] = m_d.R_DIF_Y[0][kk];
				m_s.R_PHI  [j][kk] = m_d.R_PHI  [0][kk];
				if (m_s.R_MIN > m_d.R_SQAR[0][kk]) {
					m_s.R_MIN = m_d.R_SQAR[0][kk];
					m_s.R_MIN_IDX = m_s.R_IDX;
				}
			}
#endif
		}
		m_s.GET_TRIG = 0;
	}
}
void CKOP_CIRCLE_EX::SAVE_STG_RESULT(CWnd* pWndForm)
{
	CCSV	csv;
	CCSV	*pc = &csv;
	CString	buf;
	CString	bufArea;
	LPCTSTR pHead0[] = {
		"", "重心(x)", "重心(y)", "中心からのズレ(x)", "ズレ(y)",
		"面積(pix)", "直径(pix)"
	};
	//LPCTSTR	pHead1[] = {
	//	"測定結果", "1回目", "2回目", "3回目"
	//};
	int		r = 0;
	int index = 0;
	for(int kk = 0; kk < 9; kk++)
	{
		if(m_d.isEnable[kk] == 0)
		{
			continue;
		}

		bufArea.Format("円の面積＆重心 エリア-%d", kk + 1);
		pc->set( 0, r, bufArea);
		r++;
		//---
		pc->set(0, r, "測定時刻");
		pc->set(1, r, DT2S(&m_s.R_TIME));
		r++;
		//---
		for (int c = 0; c < ROWS(pHead0); c++) {
			pc->set(c, r, pHead0[c]);
		}
		if (TRUE) {
			buf.Format("ステージ位置(%s)", STG::UNIT_STR(1));
			pc->set(0, r, buf);
		}
		r++;
		//---
		for (int i = 0; i < STG::DATA_COUNT; i++) {
			for (int c = 0; c < ROWS(pHead0); c++) {
				switch (c) {
				case 0: buf = FGS(m_s.R_POS  [i]); break;
				case 1: buf = F1S(m_s.R_COG_X[i][kk]); break;
				case 2: buf = F1S(m_s.R_COG_Y[i][kk]); break;
				case 3: buf = F1S(m_s.R_DIF_X[i][kk]); break;
				case 4: buf = F1S(m_s.R_DIF_Y[i][kk]); break;
				case 5: buf = F0S(m_s.R_SQAR [i][kk]); break;
				case 6: buf = F2S(m_s.R_PHI  [i][kk]); break;
				}
				pc->set(c, r+i, buf);
			}
			index = i;
		}
		r += index + 1;
	}
	//---
	CKOP::SAVEADD_CSV(pWndForm, &csv, "center_stg.csv");
}
#endif