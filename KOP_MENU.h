#pragma once


class CKOP_MENU
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
	void DRAW_HIST(CWnd *pWnd, int n, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);
	static
	void DRAW_ZOOM(CWnd *pWnd, int n, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);
	static
	void DRAW_CURS(CWnd *pWnd);
	static
	void UPDATE_TEXT(CWnd *pWndForm, int mask, LPBYTE pImgPxl);
#if 1//2018.05.01
	static
	BOOL EXEC_EXT_PROG(void);

	static
	void TIMER_PROC(CWnd* pWndForm);
#endif
	static
	void ON_DRAW_STA(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);
	static
	void ON_DRAW_END(CWnd *pWndForm, LPBYTE pImgPxl, LPBITMAPINFO pbmpinfo);
	static
	BOOL CMD_MSG(CWnd* pWndForm, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	static
	BOOL MSG_PROC(CWnd* pWnd, MSG* pMsg);
};

