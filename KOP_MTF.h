#pragma once
#if 1//2017.07.18
#include "CSV.h"
#endif
class CKOP_MTF
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
#if 1//2015.07.24
	static
	void CKOP_MTF::CHECK_RECT(LPRECT pr);
#endif
	static
	void CKOP_MTF::ROI_MAX_MIN(LPBYTE pImgPxl, LPRECT pr, int *pmax, int *pmin);
	static
	void CKOP_MTF::ROI_STDV(LPBYTE pImgPxl, LPRECT pr, double *average, double *stdv);
	static
	double CKOP_MTF::ROI_AVG_SEL(LPBYTE pImgPxl, LPRECT pr, int hi, int lo);
	static
	double CKOP_MTF::ROI_AVG(LPBYTE pImgPxl, LPRECT pr);

	static
	void CALC_MTF(LPBYTE pImgPxl);
	static
	void UPDATE_LABELS(CWnd *pWndForm, int mask=-1);
	static
	void UPDATE_RESULT(CWnd *pWndForm, int idx=0);
	static
	void SAVE_RESULT(CWnd *pWndForm);
	static
	void DRAW_ZOOM(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pBmpInfo);
	static
	void CKOP_MTF::DRAW_GRAPH(CWnd *pWndForm, int mask=-1);

	static
	void DRAW_MARQUEE(CWnd *pWnd, int mask=-1);
	static
	void DRAW_MARQUEE_IN_ZOOM(CWnd *pWnd, CWnd* pWndZoom);

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

