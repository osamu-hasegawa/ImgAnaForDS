/***************************************************************************************
//20160514 Hirota add to calcrate m_d.R_ZOOM_RATE.
****************************************************************************************/

#pragma once
#if 1//2017.07.18
#include "CSV.h"
#endif
class CKOP_SCALE
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
	void CALC_SCALE(LPBYTE pImgPxl);
	static
	void UPDATE_RESULT(CWnd *pWndForm);
	static
	void DRAW_MARQUEE(CWnd *pWnd);
	static
	//20160514 Hirota add to calcrate m_d.R_ZOOM_RATE. Start
	//void CKOP_SCALE::DRAW_GRAPH(CWnd *pWnd);//source
	void CKOP_SCALE::DRAW_GRAPH(CWnd *pWndForm,CWnd *pWnd);
	//20160514 Hirota add to calcrate m_d.R_ZOOM_RATE. End

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

