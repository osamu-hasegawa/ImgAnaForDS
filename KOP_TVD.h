#pragma once
#if 1//2017.07.18
#include "CSV.h"
#endif
//------------------------------------
typedef struct t_dot {
	double	cogx;	//èdêSç¿ïW
	double	cogy;
	double	relx;
	double	rely;
	double	sqar;	//ñ êœ
} DOT;

class CKOP_TVD
{
public:
	static
	ITBL itbl[];
	static
	CTBL ctbl[];

	static
	void INIT_FORM(CWnd *pWndForm);
	static
	void TERM_FORM(void);
	static
	COLORREF HSV2RGB(int h, int s, int v);

	static
	void DO_FILTER(CWnd *pWndForm, LPBYTE pImgPxl);
	static
	void CALC_TVD(LPBYTE pImgPxl);
	static
	void UPDATE_LABELS(CWnd *pWndForm, int mask=-1);
	static
	void UPDATE_RESULT(CWnd *pWndForm);
	static
	void DRAW_CROSSHAIR(CWnd *pWnd);
	static
	void DRAW_MARKER(CWnd *pWnd);
	
	static
	void DRAW_GRAPH(CWnd *pWnd, CArray<DOT,DOT> &ad, double cof[], LPGDEF pgdef);

	static
	void ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);
	static
	void ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);
	static
	BOOL CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	static
	BOOL MSG_PROC(CWnd* pWnd, MSG* pMsg);
#if 1//2017.07.18
	static
	void SET_CSV(CCSV *pc);
#endif
};

