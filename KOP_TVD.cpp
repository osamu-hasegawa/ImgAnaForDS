#include "StdAfx.h"
#include "resource.h"
#include "KOP.h"
#include "KOP_TVD.h"
#if 1//2015.09.10
#include "KOP_SHADING.h"
#endif
#include <math.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "EX.h"
#include "LSQ.h"
#define MAX_AVG	(20)

static
struct {
	// PARAMTER
//	RECT	P_RT_L;
//	RECT	P_RT_R;

//	RECT	P_RT_LU;//IMAGE座標系⇔ZOOM座標系変換用の単位矩形
//	RECT	P_RT_RU;

	int		P_X_LINE;
	int		P_BIN_IMG;
	int		P_BIN_VAL;
	int		P_AVG_CNT;
#if 1//2015.09.10
	int		P_SHAD_CORR;
#endif
	//
	//
	// RESULT
	double	R_TVD;
	double	R_COEF_T[3];
	double	R_COEF_B[3];
//	double	R_MTF_T[5];//測定結果:MTF(縦)
//	double	R_MTF_Y[5];//測定結果:MTF(横)
	//---
	//---
//	int		B_BASE;//基準矩形の指定待ち
	int		B_CNT_R;//領域数(cvFindContoursによる)
	int		B_CNT_S;//領域数(最大領域の値の1/3未満をカットした後の数)
//	RECT	B_RT_L;
//	RECT	B_RT_R;
//	TCHAR	B_SERIAL[64];
	CArray<DOT,DOT>	B_AD;	//全領域情報
	CArray<DOT,DOT>	B_AT[MAX_AVG];	//上端部分の領域情報
	CArray<DOT,DOT>	B_AB[MAX_AVG];	//下端...
	//---
	CArray<DOT,DOT>	B_AT_AVG;//座標平均化後
	CArray<DOT,DOT>	B_AB_AVG;//座標平均化後
	//
	//  B_AT[B_IDX], B_AB[B_IDX]に書き込み
	//  B_CNT >= R_AVG_CNT以上なら
	//  B_IDX-1からB_IDX-R_AVG_CNT-1までのデータを平均化して
	//  B_AT_AVG, B_AB_AVGに格納する
	//
	int		B_IDX;	//平均化処理用:次のデータ保存位置
	int		B_CNT;	//平均化処理用:データ保存数
} m_d;
//------------------------------------
static
GDEF		m_gdef[2];

//------------------------------------
//------------------------------------
//CArray<DOT,DOT>	m_d.B_AD;
//CArray<DOT,DOT>	m_d.B_AT;
//CArray<DOT,DOT>	m_d.B_AB;

//------------------------------------

ITBL CKOP_TVD::itbl[] = {
	{ 0,          0,    0}
};

//
// 倍率解析画面定義
//
CTBL CKOP_TVD::ctbl[] = {
	{ 0, 0  , IDC_STATIC40, 5, "LOGO"              ,1140,  30, 159,108, WS_BORDER|0|SS_CENTERIMAGE|SS_BITMAP},
	{ 0, 0  , IDC_STATIC10, 5, "TVディストーション測定",  10,   3, 322, 22,           SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC11, 5, "Ser.No:201506012"  , 780,   3, 350, 22, WS_BORDER|SS_CENTERIMAGE},
#if 1//2015.09.10
	{ 0, 0  , IDC_CHECK1  , 7, "十字線"            , 990,  40, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
	{ 0, 0  , IDC_CHECK2  , 7, "シェーディング補正", 990, 190, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
#else
	{ 0, 0  , IDC_CHECK1  , 5, "十字線"            , 990,  40, 140, 50, WS_BORDER|0|BS_PUSHLIKE},
#endif
	{ 0, 0  , IDC_STATIC12, 3, "TVディストーション", 670, 130, 150, 25, WS_BORDER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC13, 5, "N.NNN%"            , 850, 130, 200, 25, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
#if 1
	{ 0, 0  , IDC_STATIC4, 0, "IMAGE1"             , 700, 200, 240,180, WS_BORDER},//WS_BORDER},//SS_WHITEFRAME},
#else
	{ 0, 0  , IDC_STATIC4, 0, "IMAGE1"             , 830, 200, 240,180, WS_BORDER},//WS_BORDER},//SS_WHITEFRAME},
#endif
#if 1
	{ 0, 0  , IDC_STATIC31, 5, "2値化画像"         , 990, 260, 140, 50, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_NOTIFY},
	{ 0, 0  , IDC_STATIC33, 5, "2値化閾値"         , 990, 330, 140, 35, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_EDIT2   , 5, "66"                , 990, 365, 140, 25, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|ES_NUMBER},
	{ 0, 0  , IDC_SPIN2   , 0, (LPCTSTR)(0|255<<16),   0,   0,   0,  0, UDS_SETBUDDYINT|UDS_ALIGNRIGHT},
#endif
	{ 0, 0  , IDC_STATIC4 , 5, "平均回数"          , 900, 450, 100, 25, 0|0|ES_CENTER},
	{ 0, 0  , IDC_EDIT1   , 5, "1"                 ,1000, 450, 100, 25, WS_BORDER|0|ES_CENTER|0/*ES_READONLY*/},
	{ 0, 0  , IDC_SPIN1   , 0, (LPCTSTR)(1|20<<16) ,   0,   0,   0,  0, UDS_SETBUDDYINT|UDS_ALIGNRIGHT},

	{ 0, 0  , IDC_STATIC14, 5, "DEBUG.INFO"        ,700, 400-5, 150, 25, 0|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC15, 5, "RR"                ,700, 430-5, 150, 25, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC16, 5, "RR"                ,700, 460-5, 150, 25, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},
	{ 0, 0  , IDC_STATIC17, 5, "RR"                ,700, 490-5, 150, 25, WS_BORDER|SS_CENTER|SS_CENTERIMAGE},

	{ 0, 0  , IDC_BUTTON7 , 0, "保存"              , 990, 500, 140, 50},
	{ 0, 0  , IDC_BUTTON8 , 0, "戻る"              , 990, 560, 140, 50},
	{ 0, 8  , IDC_STATIC36, 0, "COORDINATES"       ,  10, 518, 810, 35, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//SS_WHITEFRAME},
	{ 0, 8  , IDC_STATIC37, 0, "GRAPH"             ,  10, 553+1, 810, 60, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//SS_WHITEFRAME},
	{ 0, 8  , IDC_STATIC38, 0, "COORDINATES"       ,  10, 613+2, 810, 35, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//SS_WHITEFRAME},
	{ 0, 8  , IDC_STATIC39, 0, "GRAPH"             ,  10, 648+3, 810, 60, WS_BORDER|SS_CENTER|SS_CENTERIMAGE|SS_WHITEFRAME},//SS_WHITEFRAME},
	{ 0, 0  ,            0, 0, NULL}
};
#if 1//2017.04.01
static
void CHECK_FIXED(CWnd *pWndForm)
{
	CArray<UINT, UINT> ar;

	if (CKOP::FP.TVD_AVG_CNT >= 0) {
		m_d.P_AVG_CNT = CKOP::FP.TVD_AVG_CNT;
		ar.Add(IDC_EDIT1);
	}
	if (CKOP::FP.TVD_BIN_VAL >= 0) {
		m_d.P_BIN_VAL = CKOP::FP.TVD_BIN_VAL;
		ar.Add(IDC_EDIT2);
	}
	if (CKOP::FP.TVD_BIN_IMG >= 0) {
		m_d.P_BIN_IMG = CKOP::FP.TVD_BIN_IMG;
		ar.Add(IDC_STATIC31);
	}
	if (CKOP::FP.TVD_CHECK >= 0) {
		m_d.P_X_LINE = CKOP::FP.TVD_CHECK;
		ar.Add(IDC_CHECK1);
	}
	if (CKOP::FP.TVD_SHAD_CORR >= 0) {
		m_d.P_SHAD_CORR = CKOP::FP.TVD_SHAD_CORR;
		ar.Add(IDC_CHECK2);
	}

	for (int i = 0; i < ar.GetCount(); i++) {
		pWndForm->GetDlgItem(ar[i])->EnableWindow(FALSE);
	}
}
#endif
void CKOP_TVD::INIT_FORM(CWnd *pWndForm)
{
	pWndForm->SetDlgItemTextA(IDC_STATIC11, CKOP::SERIAL_STR());
	//-----
	m_d.P_AVG_CNT = GetProfileINT("TVD", "AVG_CNT"    ,   3);
	m_d.P_BIN_IMG = GetProfileINT("TVD", "BIN_IMG"    ,   0);
	m_d.P_BIN_VAL = GetProfileINT("TVD", "BIN_VAL"    , 128);
	m_d.P_X_LINE  = GetProfileINT("TVD", "CHECK"      ,   0);
#if 1//2015.09.10
	CKOP::P_BIN_IMG = m_d.P_BIN_IMG;

	m_d.P_SHAD_CORR = GetProfileINT("TVD", "SHAD_CORR" , 0);
	if (!CKOP_SHADING::IS_AVAILABLE_CORRECTION()) {
		 m_d.P_SHAD_CORR = 0;
		 pWndForm->GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	}
	pWndForm->CheckDlgButton(IDC_CHECK2, m_d.P_SHAD_CORR!=0);
#endif
#if 1//2017.04.01
	if (G_SS.USE_FIXED) {
		CKOP::GetProfileFixed();
		CHECK_FIXED(pWndForm);
	}
#endif
	//-----
	pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_AVG_CNT );
	pWndForm->SetDlgItemInt(IDC_EDIT2, m_d.P_BIN_VAL );
	pWndForm->CheckDlgButton(IDC_CHECK1, m_d.P_X_LINE!=0);

	UPDATE_LABELS(pWndForm);
	//-----
	CKOP::GDEF_INIT(&m_gdef[0], /*bitLBL=*/1+2, pWndForm->GetDlgItem(IDC_STATIC37));
	CKOP::GDEF_INIT(&m_gdef[1], /*bitLBL=*/1+2, pWndForm->GetDlgItem(IDC_STATIC39));
	//-----
	m_d.B_CNT = 0;
	m_d.B_IDX = 0;
	//-----
	pWndForm->GetDlgItem(IDC_STATIC14)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC15)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC16)->ShowWindow(SW_HIDE);
	pWndForm->GetDlgItem(IDC_STATIC17)->ShowWindow(SW_HIDE);
	if (TRUE) {
	HBITMAP	hBMP;
	hBMP = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP8), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|0);
	((CStatic*)pWndForm->GetDlgItem(IDC_STATIC40))->SetBitmap(hBMP);
	}
}

void CKOP_TVD::TERM_FORM(void)
{
	//-----
	WriteProfileINT("TVD", "AVG_CNT"    , m_d.P_AVG_CNT);
	WriteProfileINT("TVD", "BIN_IMG"    , m_d.P_BIN_IMG);
	WriteProfileINT("TVD", "BIN_VAL"    , m_d.P_BIN_VAL);
	WriteProfileINT("TVD", "CHECK"      , m_d.P_X_LINE );
#if 1//2015.09.10
	WriteProfileINT("TVD", "SHAD_CORR"  , m_d.P_SHAD_CORR);
#endif
}
/********************************************************************/


void CKOP_TVD::DO_FILTER(CWnd *pWndForm, LPBYTE pImgPxl)
{
}

int GetNearest(CArray<DOT,DOT> &ad, int x, int y)
{
	double	dx, dy, rr;
	double	fmin = 1e99;
	int		imin = -1;

	for (int i = 0; i < ad.GetCount(); i++) {
		dx = ad[i].cogx - x;
		dy = ad[i].cogy - y;
		rr = sqrt(dx*dx + dy*dy);
		if (fmin > rr) {
			fmin = rr;
			imin = i;
		}
	}
	return(imin);
}
// adのドット領域配列より
//　i1からi2へ連なる領域を抽出して、asにセットする
//
void ExtractPath(CArray<DOT,DOT> &ad, CArray<DOT,DOT> &as, int i1, int i2)
{
	double	x1 = ad[i1].cogx;
	double	y1 = ad[i1].cogy;
	double	x2 = ad[i2].cogx;
	double	y2 = ad[i2].cogy;
	double	dx = x2-x1,
			dy = y2-y1;
	double	th, xx, yy, tt, rr;
	double	dth = 45 * 3.141592/180;	//±45度の範囲で探索

	as.RemoveAll();
	as.Add(ad[i1]);

	th = atan2(dy, dx);

	while (TRUE) {
		double	fmin = 1e99;
		int		imin = -1;

		for (int i = 0; i < ad.GetCount(); i++) {
			if (i == i1) {
				continue;
			}
			xx = ad[i].cogx;
			yy = ad[i].cogy;
			dx = xx-x1;
			dy = yy-y1;
			tt = atan2(dy, dx);
			if (fabs(tt-th) > dth) {
				continue;//方向が違うためスキップ
			}
			rr = sqrt(dx*dx + dy*dy);
			if (fmin > rr) {
				fmin = rr;
				imin = i;
			}
		}

		if (imin < 0 || as.GetCount() >= ad.GetCount()) {
			break;//ここにはこない
		}
		as.Add(ad[imin]);
		if (imin == i2) {
			break;//終了
		}
		//-----
		xx = ad[imin].cogx;
		yy = ad[imin].cogy;
		dx = xx-x1;
		dy = yy-y1;
		tt = atan2(dy, dx);
		//-----
		x1 = xx;
		y1 = yy;
		th = tt;
		i1 = imin;
		//-----
	}
}

void CKOP_TVD::CALC_TVD(LPBYTE pImgPxl)
{
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
	if (TRUE) {
		cvThreshold(img_gry, img_dst, m_d.P_BIN_VAL, 255, CV_THRESH_BINARY_INV);	//白背景に黒丸の時は反転しておく
	}
	else {
	//	cvThreshold(img_gry, img_dst, m_d.P_BIN_VAL, 255, CV_THRESH_BINARY);		//黒背景に白丸
	}
	try {
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

		m_d.B_AD.RemoveAll();
		if (contours != NULL){
			//輪郭の描画
			if (m_d.P_BIN_IMG) {
			}
			else {
#if 0//2015.09.10
			cvDrawContours(img_src, contours, CV_RGB(255,0,0), CV_RGB(0,0,255), 2, 2);
#endif
			}

			double	fmax = -1e99;
			CvSeq*	pmax = NULL;
			CvSeq*	pos = contours;
			CvMoments	moments;

			while (pos != NULL) {
				double	f = cvContourArea(pos);
#if 1
				DOT	dot;

				cvMoments(pos, &moments);

				dot.cogx = moments.m10/moments.m00;//重心座標
				dot.cogy = moments.m01/moments.m00;
				dot.relx = dot.cogx - CKOP::BMP_WID/2;
				dot.rely =-dot.cogy + CKOP::BMP_HEI/2;
				dot.sqar = f;
				m_d.B_AD.Add(dot);
#endif
				if (fmax < f) {
					fmax = f;
					pmax = pos;
				}
				pos = pos->h_next;
			}
m_d.B_CNT_R = m_d.B_AD.GetCount();
			for (int i = m_d.B_AD.GetCount()-1; i >= 0; i--) {
				if (m_d.B_AD[i].sqar < (fmax/3)) {
					m_d.B_AD.RemoveAt(i);
				}
			}
m_d.B_CNT_S = m_d.B_AD.GetCount();
			int	tl, tr, bl, br;
			//左上に一番近い領域
			tl = GetNearest(m_d.B_AD,             0,             0);
			//右上に一番近い領域
			tr = GetNearest(m_d.B_AD, CKOP::BMP_WID,             0);
			//左下に一番近い領域
			bl = GetNearest(m_d.B_AD,             0, CKOP::BMP_HEI);
			//右下に一番近い領域
			br = GetNearest(m_d.B_AD, CKOP::BMP_WID, CKOP::BMP_HEI);
			if (tl < 0 || tr < 0) {
				m_d.B_AT[m_d.B_IDX].RemoveAll();
			}
			else {
				ExtractPath(m_d.B_AD, m_d.B_AT[m_d.B_IDX], tl, tr);
			}
			if (bl < 0 || br < 0) {
				m_d.B_AB[m_d.B_IDX].RemoveAll();
			}
			else {
				ExtractPath(m_d.B_AD, m_d.B_AB[m_d.B_IDX], bl, br);
			}
		}
		if (m_d.P_BIN_IMG) {
			//表示上は背景の反転はしないで表示するため
			cvThreshold(img_gry, img_dst, m_d.P_BIN_VAL, 255, CV_THRESH_BINARY);
#if 1//2015.07.24
			if (CKOP::BMP_BYTE_CNT == 1) {
#if 1//2015.09.10
			cvDrawContours(img_dst, contours, CV_RGB(200,200,200), CV_RGB(100,100,100), 2, 2);
			if (CKOP::m_bBOTTOMUP) {
				cvFlip(img_dst, NULL, 0); // 水平軸で反転（垂直反転）
			}
#else
			cvDrawContours(img_dst, contours, CV_RGB(255,0,0), CV_RGB(0,0,255), 2, 2);
#endif
			//8bitカラーのときはグレースケールで格納されていると想定する
			memcpy(pImgPxl, img_dst->imageData, img_dst->imageSize);
			}
			else {
#endif
			cvCvtColor(img_dst, img_src, CV_GRAY2BGR);
			cvDrawContours(img_src, contours, CV_RGB(255,0,0), CV_RGB(0,0,255), 2, 2);
#if 1//2015.09.10
			if (CKOP::m_bBOTTOMUP) {
				cvFlip(img_src, NULL, 0); // 水平軸で反転（垂直反転）
			}
#endif
			memcpy(pImgPxl, img_src->imageData, img_src->imageSize);
#if 1//2015.07.24
			}
#endif
		}
		//メモリストレージの解放
		cvReleaseMemStorage (&storage);
	}
	catch (cv::Exception ex) {
		mlog("%s", ex.msg);
	}
	
	cvReleaseImage(&img_dst);
	cvReleaseImage(&img_gry);
	cvReleaseImage(&img_src);
}
void CKOP_TVD::DRAW_CROSSHAIR(CWnd *pWnd)
{
	CDC	*pDC = pWnd->GetDC();
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	pDC->MoveTo(CKOP::IMG_WID/2,             0);
	pDC->LineTo(CKOP::IMG_WID/2, CKOP::IMG_HEI);

	pDC->MoveTo(            0, CKOP::IMG_HEI/2);
	pDC->LineTo(CKOP::IMG_WID, CKOP::IMG_HEI/2);

//	pDC->SelectObject(&CKOP::m_penGreen);

	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
void DRAW_MARKER(CDC *pDC, int x, int y)
{
	const
	int	gap = 4;
	pDC->MoveTo((int)(x/CKOP::RX-gap+0.5  ), (int)(y/CKOP::RY+0.5));
	pDC->LineTo((int)(x/CKOP::RX+gap+0.5+1), (int)(y/CKOP::RY+0.5));

	pDC->MoveTo((int)(x/CKOP::RX+0.5), (int)(y/CKOP::RY-gap+0.5  ));
	pDC->LineTo((int)(x/CKOP::RX+0.5), (int)(y/CKOP::RY+gap+0.5+1));
}
void CKOP_TVD::DRAW_MARKER(CWnd *pWnd)
{
//	return;
	CDC	*pDC = pWnd->GetDC();
	CPen* old_pen = pDC->SelectObject(&CKOP::m_penRed);

	for (int i = 0; i < m_d.B_AD.GetCount(); i++) {
		::DRAW_MARKER(pDC, (int)m_d.B_AD[i].cogx, (int)m_d.B_AD[i].cogy);
	}
	pDC->SelectObject(&CKOP::m_penGreen);
	for (int i = 0; i < m_d.B_AT_AVG.GetCount(); i++) {
		::DRAW_MARKER(pDC, (int)m_d.B_AT_AVG[i].cogx, (int)m_d.B_AT_AVG[i].cogy);
	}
	for (int i = 0; i < m_d.B_AB_AVG.GetCount(); i++) {
		::DRAW_MARKER(pDC, (int)m_d.B_AB_AVG[i].cogx, (int)m_d.B_AB_AVG[i].cogy);
	}

	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
void DRAW_COORD(CWnd *pWnd, CArray<DOT,DOT> &ad, LPGDEF pgdef)
{
	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
	CRect	rt;
	int		/*wid,*/ hei;
	int		cnt = ad.GetCount();
	CFont*	old_fnt = pDC->SelectObject(CKOP::GET_FONT(6));
	CRect	rg(pgdef->rt_gr);
	double	fwid = 0;
	double	fx = rg.left;

	pWnd->GetWindowRect(&rt);
	rt.OffsetRect(-rt.left, -rt.top);

#if 1//2015.07.24
	pDC->FillSolidRect(0, 0, rt.right-2, rt.bottom-2, RGB(255,255,255));
#else
	pDC->FillSolidRect(&rt, RGB(255,255,255));
#endif

//	wid = rt.Width();
	hei = rt.Height();
	pDC->SetBkMode(TRANSPARENT);
//	pDC->Rectangle(&rt);

	pDC->MoveTo(rt.left+2 , rt.top+hei/2);
	pDC->LineTo(rt.right-2, rt.top+hei/2);

	if (cnt > 1) {
		fwid = rg.Width() / (cnt-1);
	}

	for (int i =-1; i < cnt; i++) {
		CRect	rx;
		CString	buf1, buf2;
		UINT	nFmt;
	
		rx.top    = rt.top;
		rx.bottom = rt.top + hei/2;
		rx.left  = (int)(fx-fwid/2);
		rx.right = (int)(fx+fwid/2);
		if (cnt == 1) {
			rx.right = rt.right;
		}
		if (i < 0) {
			buf1 = " X";
			buf2 = " Y";
			nFmt = DT_LEFT;
			rx.left = 0;
		}
		else {
			buf1.Format("%d", (int)( ad[i].cogx - CKOP::BMP_WID/2));
			buf2.Format("%d", (int)(-ad[i].cogy + CKOP::BMP_HEI/2));
			buf1.Format("%d", (int)( ad[i].relx));
			buf2.Format("%d", (int)( ad[i].rely));

			nFmt = DT_CENTER;
		}

		if (i < 1) {
//			pDC->MoveTo(rx.right, rt.top+2);
//			pDC->LineTo(rx.right, rt.bottom-2);
		}
		else {
			pDC->MoveTo(rx.left, rt.top+2);
			pDC->LineTo(rx.left, rt.bottom-2);
		}
//		pDC->Rectangle(&rx);
		pDC->DrawText(buf1, &rx, DT_SINGLELINE|DT_VCENTER|nFmt);
		rx.OffsetRect(0, hei/2);
//		pDC->Rectangle(&rx);
		pDC->DrawText(buf2, &rx, DT_SINGLELINE|DT_VCENTER|nFmt);
		if (i >= 0) {
			fx += fwid;
		}
	}
	pDC->SelectObject(old_fnt);
	pWnd->ReleaseDC(pDC);
}
void GetXYMinMax(CArray<DOT,DOT> &ad, int *pxmin, int *pxmax, int *pymin, int *pymax)
{
	double	xmin = 9999,
			xmax =-9999,
			ymin = 9999,
			ymax =-9999;
	double	x, y;
	for (int i = 0; i < ad.GetCount(); i++) {
		x = ad[i].relx;
		y = ad[i].rely;
		if (xmin > x) {
			xmin = x;
		}
		if (xmax < x) {
			xmax = x;
		}
		if (ymin > y) {
			ymin = y;
		}
		if (ymax < y) {
			ymax = y;
		}
	}
	*pxmin = (int)floor(xmin);
	*pxmax = (int)ceil (xmax);
	*pymin = (int)floor(ymin);
	*pymax = (int)ceil (ymax);
}


#if 1
void CKOP_TVD::DRAW_GRAPH(CWnd *pWnd, CArray<DOT,DOT> &ad, double cof[], LPGDEF pgdef)
{
	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
	int		xmin, xmax, ymin, ymax;
	int		cnt = ad.GetCount();
	CFont*	old_fnt = pDC->SelectObject(CKOP::GET_FONT(6));
	int		h = 0;
//	LPBYTE	p;
	CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
#if 1//2015.09.10
//	void GDEF_CLIP(LPGDEF pdef, CDC *pDC)
	CRgn	rgn;
	rgn.CreateRectRgnIndirect(&pgdef->rt_cl);
	pDC->SelectClipRgn(&rgn);
#endif
//	pWnd->GetWindowRect(&rt);
//	rt.OffsetRect(-rt.left, -rt.top);

	GetXYMinMax(ad, &xmin, &xmax, &ymin, &ymax);

	CKOP::GDEF_PSET(pgdef, xmin, xmax, (xmax-xmin)/8, ymin, ymax, (ymax-ymin));
	CKOP::GDEF_GRID(pgdef, pDC);

//	pDC->SelectObject(&CKOP::m_penRed);
//	pDC->SelectObject(&CKOP::m_penGreen);
	for (int i = 0; i < cnt; i++) {
		int		x, y;
		CRect	rr;
		x = CKOP::GDEF_XPOS(pgdef, ad[i].relx);
		y = CKOP::GDEF_YPOS(pgdef, ad[i].rely);
//		x = (int)(rt.left   + wid * (ad[i].relx-xmin)/(xmax-xmin));
//		y = (int)(rt.bottom - hei * (ad[i].rely-ymin)/(ymax-ymin));

		rr.SetRect(x-2, y-2, x+3, y+3);
		pDC->FillSolidRect(&rr, RGB(255,0,0));
	}
	if (cnt >= 3) {
		pDC->SelectObject(&CKOP::m_penGreen);
		for (int i = 0; i < cnt; i++) {
			int		x, y;
			double	xx, yy;
			xx = ad[i].relx;
			yy = cof[0] + cof[1] * xx + cof[2] * xx * xx;

			x = CKOP::GDEF_XPOS(pgdef, xx);
			y = CKOP::GDEF_YPOS(pgdef, yy);
//			x = (int)(rt.left   + wid * (xx-xmin)/(xmax-xmin));
//			y = (int)(rt.bottom - hei * (yy-ymin)/(ymax-ymin));

			if (!i) {
				pDC->MoveTo(x, y);
			}
			else {
				pDC->LineTo(x, y);
			}
		}
		if (TRUE) {
			CRect	rt(pgdef->rt_gr);
			CString	buf;
			pDC->SetBkMode(TRANSPARENT);
			buf.Format("Y = %+.3lEX^2  %+.3lEX  %+.3lf", cof[2], cof[1], cof[0]);
			rt.OffsetRect(-2, 2);
			pDC->DrawText(buf, &rt, DT_RIGHT|DT_TOP);
		}

		CArray<double,double>	ay;
		pDC->SelectObject(&CKOP::m_penExtra);

		for (int i = 0; i < cnt; i++) {
			double	xx, yy;
			xx = ad[i].relx;
			yy = cof[0] /*+ cof[1] * xx*/ + cof[2] * xx * xx;
			ay.Add(yy);
		}
//		ymin = GetfMin(ay.GetData(), cnt);
//		ymax = GetfMax(ay.GetData(), cnt);
		for (int i = 0; i < cnt; i++) {
			int		x, y;
			double	xx, yy;
			xx = ad[i].relx;
			yy = ay[i];

			x = CKOP::GDEF_XPOS(pgdef, xx);
			y = CKOP::GDEF_YPOS(pgdef, yy);
//			x = (int)(rt.left   + wid * (xx-xmin)/(xmax-xmin));
//			y = (int)(rt.bottom - hei * (yy-ymin)/(ymax-ymin));

			if (!i) {
				pDC->MoveTo(x, y);
			}
			else {
				pDC->LineTo(x, y);
			}
		}
	}
//skip:
//	pDC->SetBkMode(TRANSPARENT);
//	rt.DeflateRect(3,3);
//	pDC->DrawText("現状 緑線のみ対応\rY軸は0-255固定", &rt, DT_RIGHT|DT_TOP);
#if 1//2015.09.10
	pDC->SelectClipRgn(NULL);
	rgn.DeleteObject();
#endif
	pDC->SelectObject(old_fnt);
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}

#else
void CKOP_TVD::DRAW_GRAPH(CWnd *pWnd, CArray<DOT,DOT> &ad, double cof[])
{
	CKOP::GDEF_INIT(&m_gdef, /*bitLBL=*/1+2, pWndForm->GetDlgItem(IDC_STATIC37));
	CKOP::GDEF_PSET(&m_gdef, 0, CKOP::BMP_WID-1, CKOP::BMP_WID/8, 0, 255, 64);

	if (pWnd == NULL) {
		return;
	}
	CDC	*pDC = pWnd->GetDC();
	CRect	rt;
	int		wid, hei, xmin, xmax, ymin, ymax;
	int		cnt = ad.GetCount();
	CFont*	old_fnt = pDC->SelectObject(CKOP::GET_FONT(6));
	int		h = 0;
//	LPBYTE	p;
	CPen*	old_pen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	pWnd->GetWindowRect(&rt);
	rt.OffsetRect(-rt.left, -rt.top);

	pDC->FillSolidRect(&rt, RGB(255,255,255));
	rt.top    += 5;
	rt.bottom -= 5;
#if 0
	rt.right = CKOP::BMP_WID;
#else
	rt.left   += 20;
	rt.right  -= 5;
#endif
	wid = rt.Width();
	hei = rt.Height();

	GetXYMinMax(ad, &xmin, &xmax, &ymin, &ymax);

	pDC->Rectangle(&rt);
#if 0
	for (int i = 0; i < cnt; i++) {
		int	x, y;
//		x = (int)(rt.left   + wid * (ad[i].cogx-xmin)/(xmax-xmin));
//		y = (int)(rt.bottom - hei * (ymax-ad[i].cogy)/(ymax-ymin));
		x = (int)(rt.left   + wid * (ad[i].relx-xmin)/(xmax-xmin));
		y = (int)(rt.bottom - hei * (ad[i].rely-ymin)/(ymax-ymin));
		if (!i) {
			pDC->MoveTo(x, y);
		}
		else {
			pDC->LineTo(x, y);
		}
	}
#endif
//	pDC->SelectObject(&CKOP::m_penRed);
//	pDC->SelectObject(&CKOP::m_penGreen);
	for (int i = 0; i < cnt; i++) {
		int		x, y;
		CRect	rr;
		x = (int)(rt.left   + wid * (ad[i].relx-xmin)/(xmax-xmin));
		y = (int)(rt.bottom - hei * (ad[i].rely-ymin)/(ymax-ymin));

		rr.SetRect(x-2, y-2, x+3, y+3);
		pDC->FillSolidRect(&rr, RGB(255,0,0));
	}
	if (cnt >= 3) {
		pDC->SelectObject(&CKOP::m_penGreen);
		for (int i = 0; i < cnt; i++) {
			int		x, y;
			double	xx, yy;
			xx = ad[i].relx;
			yy = cof[0] + cof[1] * xx + cof[2] * xx * xx;

			x = (int)(rt.left   + wid * (xx-xmin)/(xmax-xmin));
			y = (int)(rt.bottom - hei * (yy-ymin)/(ymax-ymin));

			if (!i) {
				pDC->MoveTo(x, y);
			}
			else {
				pDC->LineTo(x, y);
			}
		}
		if (TRUE) {
			CString	buf;
			pDC->SetBkMode(TRANSPARENT);
			buf.Format("Y = %+.3lEX^2  %+.3lEX  %+.3lf", cof[2], cof[1], cof[0]);
			rt.OffsetRect(-2, 2);
			pDC->DrawText(buf, &rt, DT_RIGHT|DT_TOP);
		}

		CArray<double,double>	ay;
		pDC->SelectObject(&CKOP::m_penExtra);

		for (int i = 0; i < cnt; i++) {
			double	xx, yy;
			xx = ad[i].relx;
			yy = cof[0] /*+ cof[1] * xx*/ + cof[2] * xx * xx;
			ay.Add(yy);
		}
//		ymin = GetfMin(ay.GetData(), cnt);
//		ymax = GetfMax(ay.GetData(), cnt);
		for (int i = 0; i < cnt; i++) {
			int		x, y;
			double	xx, yy;
			xx = ad[i].relx;
			yy = ay[i];

			x = (int)(rt.left   + wid * (xx-xmin)/(xmax-xmin));
			y = (int)(rt.bottom - hei * (yy-ymin)/(ymax-ymin));

			if (!i) {
				pDC->MoveTo(x, y);
			}
			else {
				pDC->LineTo(x, y);
			}
		}
	}
//skip:
//	pDC->SetBkMode(TRANSPARENT);
//	rt.DeflateRect(3,3);
//	pDC->DrawText("現状 緑線のみ対応\rY軸は0-255固定", &rt, DT_RIGHT|DT_TOP);
	pDC->SelectObject(old_fnt);
	pDC->SelectObject(old_pen);
	pWnd->ReleaseDC(pDC);
}
#endif

void CKOP_TVD::UPDATE_LABELS(CWnd *pWndForm, int mask)
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
	}
	if (mask & 4) {
	}
	if (mask & 8) {
	}
}

void CKOP_TVD::UPDATE_RESULT(CWnd *pWndForm)
{
	//----------
	pWndForm->SetDlgItemText(IDC_STATIC13, F3S(m_d.R_TVD, "%"));
}

void CKOP_TVD::ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	m_d.P_AVG_CNT = pWndForm->GetDlgItemInt(IDC_EDIT1);
	if (m_d.P_AVG_CNT <= 0) {
		pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_AVG_CNT = 1);
	}
	if (m_d.P_AVG_CNT > MAX_AVG) {
		pWndForm->SetDlgItemInt(IDC_EDIT1, m_d.P_AVG_CNT = MAX_AVG);
	}
	//-----
	m_d.P_BIN_VAL = pWndForm->GetDlgItemInt(IDC_EDIT2);
	//-----
	m_d.P_X_LINE = pWndForm->IsDlgButtonChecked(IDC_CHECK1);
#if 1//2015.09.10
	m_d.P_SHAD_CORR = pWndForm->IsDlgButtonChecked(IDC_CHECK2);
	if (m_d.P_SHAD_CORR) {
		CKOP_SHADING::DO_CORRECTION(pImgPxl);
	}
#endif
//	DO_FILTER(pWndForm, pImgPxl);
	CALC_TVD(pImgPxl);
}

void SET_LSQ()
{
	CArray<double,double>	ax;
	CArray<double,double>	ay;


	if (m_d.B_AT_AVG.GetCount() >= 3) {
		for (int i = 0; i < m_d.B_AT_AVG.GetCount(); i++) {
			ax.Add(m_d.B_AT_AVG[i].relx);
			ay.Add(m_d.B_AT_AVG[i].rely);
		}
		CLsq::Lsq(ax.GetData(), ay.GetData(), m_d.B_AT_AVG.GetCount()-1, 3-1, m_d.R_COEF_T);
	}

	ax.RemoveAll();
	ay.RemoveAll();

	if (m_d.B_AB_AVG.GetCount() >= 3) {
		for (int i = 0; i < m_d.B_AB_AVG.GetCount(); i++) {
			ax.Add(m_d.B_AB_AVG[i].relx);
			ay.Add(m_d.B_AB_AVG[i].rely);
		}
		CLsq::Lsq(ax.GetData(), ay.GetData(), m_d.B_AB_AVG.GetCount()-1, 3-1, m_d.R_COEF_B);
	}
	if (m_d.B_AT_AVG.GetCount() >= 3 && m_d.B_AB_AVG.GetCount() >= 3) {
		double	x0 = CKOP::BMP_WID/2;
		double	x2 = x0*x0;
		m_d.R_TVD = (m_d.R_COEF_T[2] * x2 - m_d.R_COEF_B[2] * x2) / (m_d.R_COEF_T[0] - m_d.R_COEF_B[0]);
	}
	else {
		m_d.R_TVD = C_NAN();
	}
}

BOOL SET_AVG(CArray<DOT,DOT> ary[], CArray<DOT,DOT> &avg)
{
	int	cnt = m_d.P_AVG_CNT;
	int	idx = m_d.B_IDX-1;

	avg.RemoveAll();

	while (cnt--) {
		if (idx < 0) {
			idx = MAX_AVG-1;
		}
		if (avg.GetCount() <= 0) {
			for (int i = 0; i < ary[idx].GetCount(); i++) {
				avg.Add(ary[idx][i]);
			}
		}
		else {
			if (ary[idx].GetCount() != avg.GetCount()) {
				//画像が安定してない...
				return(FALSE);
			}
			for (int i = 0; i < ary[idx].GetCount(); i++) {
				avg[i].cogx += ary[idx][i].cogx;
				avg[i].cogy += ary[idx][i].cogy;
				avg[i].relx += ary[idx][i].relx;
				avg[i].rely += ary[idx][i].rely;
				avg[i].sqar += ary[idx][i].sqar;
			}
		}
		idx--;
	}
	for (int i = 0; i < avg.GetCount(); i++) {
		avg[i].cogx /= m_d.P_AVG_CNT;
		avg[i].cogy /= m_d.P_AVG_CNT;
		avg[i].relx /= m_d.P_AVG_CNT;
		avg[i].rely /= m_d.P_AVG_CNT;
		avg[i].sqar /= m_d.P_AVG_CNT;
	}
	return(TRUE);
}

void CKOP_TVD::ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo)
{
	CWnd* pWnd = pWndForm->GetDlgItem(IDC_IMAGEAREA);
#if 1
	CString	buf;
	buf.Format("領域数:%d",m_d.B_CNT_R);	pWndForm->SetDlgItemText(IDC_STATIC15, buf);
	buf.Format("CUT後:%d" ,m_d.B_CNT_S);	pWndForm->SetDlgItemText(IDC_STATIC16, buf);
	buf.Format("上端数:%d",m_d.B_AT[m_d.B_IDX].GetCount());	pWndForm->SetDlgItemText(IDC_STATIC17, buf);
#endif
	m_d.B_CNT++;
	if (++m_d.B_IDX >= MAX_AVG) {
		m_d.B_IDX = 0;
	}

	if (m_d.P_X_LINE) {
	DRAW_CROSSHAIR(pWnd);
	}
	if (m_d.B_CNT >= m_d.P_AVG_CNT) {
		if (!SET_AVG(m_d.B_AT, m_d.B_AT_AVG)) {
			return;
		}
		if (!SET_AVG(m_d.B_AB, m_d.B_AB_AVG)) {
			return;
		}

		DRAW_MARKER(pWnd);
//		DRAW_MARQUEE(pWnd);
		SET_LSQ();
	CRect	rt(20, 0, -10, 0);
	CKOP::GDEF_INIT(&m_gdef[0], /*bitLBL=*/1+2, pWndForm->GetDlgItem(IDC_STATIC37), &rt);
	CKOP::GDEF_INIT(&m_gdef[1], /*bitLBL=*/1+2, pWndForm->GetDlgItem(IDC_STATIC39), &rt);

		DRAW_COORD(pWndForm->GetDlgItem(IDC_STATIC36), m_d.B_AT_AVG, &m_gdef[0]);
		DRAW_COORD(pWndForm->GetDlgItem(IDC_STATIC38), m_d.B_AB_AVG, &m_gdef[1]);
		DRAW_GRAPH(pWndForm->GetDlgItem(IDC_STATIC37), m_d.B_AT_AVG, m_d.R_COEF_T, &m_gdef[0]);
		DRAW_GRAPH(pWndForm->GetDlgItem(IDC_STATIC39), m_d.B_AB_AVG, m_d.R_COEF_B, &m_gdef[0]);

		UPDATE_RESULT(pWndForm);
	}
//	buf.Format("%.3lf%%", m_d.R_TVD);
//	pWndForm->SetDlgItemText(IDC_STATIC13, buf);
}

BOOL CKOP_TVD::CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	switch (nCode) {
	case BN_CLICKED:
		switch (nID) {
		case IDC_BUTTON7://保存
			CKOP::SAVE_WINDOW(AfxGetMainWnd(), "dist.png");
#if 1//2017.07.18
			{
				CCSV	csv;
				SET_CSV(&csv);
				CKOP::SAVE_CSV(&csv, "dist.csv");
			}
#endif
			return(TRUE);
		break;
		case IDC_BUTTON8://戻る
			CKOP::MOVE_FORM(pWndForm, 1);
			return(TRUE);
		break;
		}
	break;
	case EN_CHANGE:
		switch (nID) {
		case IDC_EDIT1://平均回数
		break;
		case IDC_EDIT2://値化閾値
			m_d.P_BIN_VAL = pWndForm->GetDlgItemInt(nID);
pWndForm->Invalidate();
		break;
		}
	break;
	}
	return(FALSE);
}

BOOL CKOP_TVD::MSG_PROC(CWnd* pWndForm, MSG* pMsg)
{
	UINT	nID;

	if (pWndForm->m_hWnd == pMsg->hwnd) {
		nID = IDC_IMAGEAREA;
	}
	else {
		nID = ::GetDlgCtrlID(pMsg->hwnd);
	}
	switch (nID) {
	case IDC_IMAGEAREA:
	break;
	case IDC_STATIC31:
		switch (pMsg->message) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
#if 1//2015.09.10
			CKOP::P_BIN_IMG =
#endif
			m_d.P_BIN_IMG = !m_d.P_BIN_IMG;
			UPDATE_LABELS(pWndForm, 1);
		break;
		}
	break;
	}
	return(FALSE);
}
#if 1//2017.07.18
void CKOP_TVD::SET_CSV(CCSV *pc)
{
	CString	buf;
	LPCTSTR	pHead0[] = {
		"", "Ave", "Max", "Min", "シェーディング(+)", "シェーディング(-)"
	};
	LPCTSTR	pHead1[] = {
		"解析結果", "緑線", "赤線", ""
	};
	int		r = 0;
	pc->set( 0, r, "TVディストーション測定");
	r++;
	//---
/*	pc->set( 0, r, "フィルター処理");
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
	r++;*/
	//---
	pc->set( 0, r, "TVディストーション");
	pc->set( 1, r, F3S(m_d.R_TVD));
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
	pc->set( 0, r, "上部");
	r++;
	//---
	pc->set( 0, r, "y=c2*x^2 + c1*x + c0");
	pc->set( 1, r, "c2");
	pc->set( 2, r, "c1");
	pc->set( 3, r, "c0");
	r++;
	//---
	pc->set( 0, r, "係数");
	for (int i = 0; i < 3; i++) {
		if (i < 2) {
			buf.Format("%.3lE", m_d.R_COEF_T[2-i]);
		}
		else {
			buf = F3S(m_d.R_COEF_T[2-i]);
		}
		pc->set(1+i, r, buf);
	}
	r++;
	//---
	pc->set( 0, r+0, "No.");
	pc->set( 0, r+1, "座標X");
	pc->set( 0, r+2, "座標Y");
	for (int i = 0; i < m_d.B_AT_AVG.GetCount(); i++) {
		int	c = i+1;
		pc->set(c, r+0, I2S(c));
		pc->set(c, r+1, I2S(m_d.B_AT_AVG[i].relx));
		pc->set(c, r+2, I2S(m_d.B_AT_AVG[i].rely));
	}
	r+=3;
	//---
	pc->set( 0, r, "下部");
	r++;
	//---
	pc->set( 0, r, "y=c2*x^2 + c1*x + c0");
	pc->set( 1, r, "c2");
	pc->set( 2, r, "c1");
	pc->set( 3, r, "c0");
	r++;
	//---
	pc->set( 0, r, "係数");
	for (int i = 0; i < 3; i++) {
		if (i < 2) {
			buf.Format("%.3lE", m_d.R_COEF_B[2-i]);
		}
		else {
			buf = F3S(m_d.R_COEF_B[2-i]);
		}
		pc->set(1+i, r, buf);
	}
	r++;
	//---
	pc->set( 0, r+0, "No.");
	pc->set( 0, r+1, "座標X");
	pc->set( 0, r+2, "座標Y");
	for (int i = 0; i < m_d.B_AB_AVG.GetCount(); i++) {
		int	c = i+1;
		pc->set(c, r+0, I2S(c));
		pc->set(c, r+1, I2S(m_d.B_AB_AVG[i].relx));
		pc->set(c, r+2, I2S(m_d.B_AB_AVG[i].rely));
	}
	r+=3;
	//---
	return;
}
#endif