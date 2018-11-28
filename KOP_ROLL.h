//**************************************************************************************
// ÉJÉÅÉâåXÇ´âÊñ 
//--------------------------------------------------------------------------------------
// 2016.04.01 êVãKí«â¡
//**************************************************************************************

#pragma once
#if 1//2017.07.18
#include "CSV.h"
#endif
class CKOP_ROLL
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
	BOOL IS_BIN_UPPER(int n);
	static
	void CALC_ROLL(LPBYTE pImgPxl);
	static
	void CALC_CENT(LPBYTE pImgPxl);
	static
	void DRAW_MARQUEE(CWnd *pWnd);
	static
	void DRAW_GRAPH(CWnd *pWnd);
	static
	void DRAW_CURS(CDC *pDC, int x1, int x2, int y1, int y2);
	static
	void DRAW_CURS(CWnd *pWnd);
	static
	void DRAW_MARKER(CWnd *pWnd);
	static
	void UPDATE_LABELS(CWnd *pWndForm, int mask);
	static
	void UPDATE_RESULT(CWnd *pWndForm, int idx=0);
	static
	void SAVE_RESULT(CWnd *pWndForm);
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

