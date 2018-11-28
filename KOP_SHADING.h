#pragma once
#if 1//2017.07.18
#include "CSV.h"
#endif
class CKOP_SHADING
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
#if 1//2015.09.10
	static
	BOOL IS_AVAILABLE_CORRECTION(void);
	static
	void INIT_CORRECTION(void);
	static
	void TERM_CORRECTION(void);
	static
	void DO_CORRECTION(LPBYTE pImgPxl);
#endif
	static
	COLORREF HSV2RGB(int h, int s, int v);

	static
	void DO_FILTER(CWnd *pWndForm, LPBYTE pImgPxl, int nFilType, int nFilPara);
	static
	void CALC_SHADING(LPBYTE pImgPxl);
	static
	void UPDATE_RESULT(CWnd *pWndForm);
	static
	void DRAW_DIAGO(CWnd *pWnd);
	static
	void DRAW_GRADIENT(CWnd *pWnd);
	static
	void DRAW_GRAPH(CWnd *pWnd);
#if 1//2017.04.01
	static
	void DRAW_GABPTS(CWnd *pWnd);
#endif
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

