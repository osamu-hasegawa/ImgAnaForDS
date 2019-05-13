/*****************************************************************************************
20160427 Hirota add to draw blue line.(Graph)
20160509 Hirota add to draw blue ROI.(MTF)
******************************************************************************************/

#pragma once
#if 1//2018.05.01
#include "DlgProgress.h"
//extern
//int		m_fid;
#endif
#if 1//2017.07.18
#include "CSV.h"
#endif
typedef struct t_itbl {
	int		sid;
	int		nID;
	void*	p;
} ITBL, *LPITBL;

typedef struct t_ctbl {
	CWnd	*p;
	int		offs;
	int		nID;
	int		ifnt;
	LPCTSTR	cap;
	int		x;
	int		y;
	int		cx;
	int		cy;
	UINT	st;
} CTBL, *LPCTBL;

typedef struct t_ftbl {
	LPCTSTR	face;
	int		point;
	int		bold;
	CFont	fnt;
} FTBL, *LPFTBL;

typedef struct t_gdef {
	RECT	rt_cl;		//クライアント領域全体
	RECT	rt_gr;		//グラフエリア
	int		rt_wid;		//グラフエリア幅
	int		rt_hei;		//グラフエリア高
	//---
	int		xmin;
	int		xmax;
	int		xtic;
	int		ymin;
	int		ymax;
	int		ytic;
	//---
	int		bitLBL;		//1:X軸ラベル, 2:Y軸ラベル
	int		bGRID_X;
	int		bGRID_Y;
	int		bTIC_X;
	int		bTIC_Y;
	int		bNUM_X;
	int		bNUM_Y;
	//---
	int		xwid;
	int		yhei;
	//---
} GDEF, *LPGDEF;


class CKOP
{
public:
	static
	int		STAT_OF_ANALYSE;
	static
	POINT	m_pnt_g, m_pnt_r;
	static
	int		IMG_WID;
	static
	int		IMG_HEI;
	static
	int		BMP_WID;
	static
	int		BMP_HEI;
	static const
	int		BASE_WID;
	static const
	int		BASE_HEI;
	static
	double	RX;
	static
	double	RY;
	static
	TCHAR	SERIALNO[64];
	static
	int		CKOP::BMP_BIT_CNT;
	static
	int		CKOP::BMP_BYTE_CNT;
	static
	int		m_bBOTTOMUP;
#if 0//2016.01.01
	static
//	BYTE	IMG_DEF[640*480*4];
	BYTE	IMG_DEF[2448*2048*4];
#endif

	static
	CPen	m_penBlue;

	static
	CBitmap	BMP_DEF;
	static
	CPen	m_penRed;
	static
	CPen	m_penGreen;
	static
	CPen	m_penYellow;
	static
	CPen	m_penPink;
	static
	CPen	m_penWater;
	static
	CPen	m_penOrange;
	static
	CPen	m_penBrown;
	static
	CPen	m_penViolet;
	static
	CPen	m_penRedDot;
	static
	CPen	m_penGreenDot;

	static
	CPen	m_penExtra;
	static
	CBrush	m_brsBack;
	static
	CRect	IMG_RECT;
	static
	TCHAR	CKOP::SAVE_DIR[MAX_PATH];
#if 1//2015.07.24
	static
	int		P_COLMAP;
	static
	RGBQUAD	RGB_TBL1[256];
	static
	RGBQUAD	RGB_TBL2[256];
#endif
#if 1//2015.09.10
	static
	int		P_BIN_IMG;
#endif
#if 1//2017.04.01
	static
	FIXEDPARA	FP;
#endif

	static
	int		STAT_OF_DISPATCH;
#if 1//2018.05.01
	static CDlgProgress
			DLG_PRG;
#endif
	static
	CFont *CKOP::GET_FONT(int i);

	static
	void CREATE_FORM(CWnd *pWnd, LPCTBL pct, LPITBL pit=NULL);
	static
	void DESTROY_FORM(CWnd *pWnd);
	static
	void MOVE_FORM(CWnd* pWndForm, int iNextPage);
	static
	void ToClinet(HWND hWnd, HWND hWndClient, LPRECT p);
	static
	void ToClinet(HWND hWnd, HWND hWndClient, LPPOINT p);
#if 1
	static
	void IMGCD_TO_BMPCD(LPRECT ri, LPRECT rb);
	static
	void IMGCD_TO_BMPCD(LPPOINT ri, LPPOINT rb);
	static
	void BMPCD_TO_IMGCD(LPRECT rb, LPRECT ri);
	static
	LPRECT BMPCD_TO_IMGCD(LPRECT rb);
	static
	void BMPCD_TO_IMGCD(LPPOINT rb, LPPOINT ri);
	static
	LPPOINT BMPCD_TO_IMGCD(LPPOINT rb);
	static
	LPPOINT IMGCD_TO_BMPCD(LPPOINT ri);
#endif
#if 1//2018.05.01
	static
	void NORMALIZE_RECT(LPRECT ri);
	static
	void RECT_TO_CVRECT(LPRECT ri, void *ro);
#endif
	static
	void CKOP::DRAW_MARQUEE(CDC *pDC, LPRECT pr);
	static
	void CKOP::DRAW_MARQUEE(CDC *pDC, int x1, int y1, int x2, int y2);
	static
	void CKOP::DRAW_ZOOM_EX(CWnd *pWnd, LPPOINT pnt, LPBYTE pImgPxl, LPBITMAPINFO pBmpInfo, int zoom);
	static
	LPCTSTR CKOP::SERIAL_STR(void);
	static
#if 1//2015.09.10
	BOOL PRESET_SIZE(LPBITMAPINFO pBmpInfo);
#else
	void PRESET_SIZE(LPBITMAPINFO pBmpInfo);
#endif
	static
	void PRESET(CWnd* pWndForm);
	static
	int GET_PIXEL(int x, int y, LPBYTE pImgPxl);

	static
	void GDEF_INIT(LPGDEF pdef, int bitLBL, CWnd* pWnd, LPRECT poff=NULL);
	static
	void GDEF_PSET(LPGDEF pdef, int xmin, int xmax, int xtic, int ymin, int ymax, int ytic);
	static
	int GDEF_XPOS(LPGDEF pdef, double f);
	static
	int GDEF_YPOS(LPGDEF pdef, double f);
	static
	void GDEF_GRID(LPGDEF pdef,CDC *pDC);

	static
	HBITMAP LOAD_BITMAP(LPCTSTR pszFileName);
	static
	void SAVE_BITMAP(HBITMAP hbmp,  LPCTSTR pszFileName);
	static
	void SAVE_WINDOW(CWnd *pWnd, LPCTSTR pszFileName);
#if 1//2017.07.18
	static
	void SAVE_CSV(CWnd *pWnd, CCSV *p, LPCTSTR pszFileName);
#endif
#if 1//2018.05.01
	static
	void SAVEADD_CSV(CWnd *pWnd, CCSV *p, LPCTSTR pszFileName);
#endif
	static
	void ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);

	static
	void ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);

	static
	void UPDATE_TEXT(CWnd *pWndForm, int mask, LPBYTE pImgPxl);

	static
	BOOL CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	static
	BOOL MSG_PROC(CWnd* pWnd, MSG* pMsg);

	static
	BOOL MSG_PROC_MOUSE(CWnd* pWndForm, MSG* pMsg, int *pCaptured, LPPOINT pPnt);
#if 1//2015.07.24
	static
	void STOP(void);
#endif
#if 1//2017.04.01
	static
	void INIT_MITSUTOYO(CWnd* pWndForm);
	static
	void TERM_MITSUTOYO(void);
	static
	BOOL GET_MITSUTOYO(char *p, int n);
	static
	void GetProfileFixed(void);
	static
	BOOL IS_RECT_OK(LPRECT p);
#endif
#if 1//2018.05.01
	static
	BOOL NOTIFY_MSG(CWnd* pWndForm, UINT nID, LPNMHDR pNMHdr, LRESULT* pResult);
	static
	HBRUSH CTL_COLOR(CDC *pDC, CWnd* pWnd);
	static
	void TIMER_PROC(CWnd* pWndForm);
	static
	void UPD_STG_TEXT(CWnd *pWndForm, int nID, int ch);
	static
	void BEEP(int cnt);
#endif
};

