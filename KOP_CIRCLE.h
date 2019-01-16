#pragma once
#if 1//2017.07.18
#include "CSV.h"
#endif
class CKOP_CIRCLE
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
	void CALC_GRAV(CWnd *pWndForm, LPBYTE pImgPxl);

	static
	void UPDATE_LABELS(CWnd *pWndForm, int mask=-1);
	static
	void UPDATE_RESULT(CWnd *pWndForm, int idx=0);
	static
	void DRAW_MARKER(CDC *pDC, int xx , int yy, int gap);
	static
	void DRAW_MARKER(CWnd *pWnd);
	static
	void SAVE_RESULT(CWnd *pWnd);
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
#if 1//2018.05.01
	static
	void TIMER_PROC(CWnd* pWndForm);
	static
	void SAVE_STG_RESULT(CWnd* pWndForm);
	static
	void STORE_STG_RESULT(void);
#endif
};

