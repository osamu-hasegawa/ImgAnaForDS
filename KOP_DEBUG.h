#pragma once

#define CH_OF_VERT	(1)
#define CH_OF_HORZ	(0)
#define MAX_WIDTH	(2048)
#define MAX_HEIGHT	(1920)
#define MAX_THD_CNT	(1000)


typedef double (*F_CONV)(double f);

class CKOP_DEBUG
{
public:
	//CKOP_DEBUG(void);
	//~CKOP_DEBUG(void);
	static
	BYTE	IMG_BUF[MAX_WIDTH*MAX_HEIGHT*4];
	static
	BYTE	IMG_SUB[MAX_WIDTH*MAX_HEIGHT*4];

	//DEBUG—p
	static
	int		CKOP_DEBUG::DBG_REQ_CODE;
	static
	int		CKOP_DEBUG::DBG_FILEIN;
	static
	int		CKOP_DEBUG::DBG_PAUSE;
	static
	int		CKOP_DEBUG::DBG_NEXT;
	static
	int		CKOP_DEBUG::DBG_FRAME_IDX;
	static
	int		CKOP_DEBUG::DBG_FILE_IDX;
	static
	BYTE	CKOP_DEBUG::DBG_BUF[MAX_WIDTH*MAX_HEIGHT*4];
	static
	int		CKOP_DEBUG::DBG_IMG_BPP;
	static
	int		CKOP_DEBUG::DBG_IMG_WIDTH;
	static
	int		CKOP_DEBUG::DBG_IMG_HEIGHT;

	static
	int CKOP_DEBUG::DEBUG_PROC(LPBYTE pBUF);
	static
	int CKOP_DEBUG::DEBUG_GET_PIXEL(LPBYTE pBUF, int x, int y);
	static
	void CKOP_DEBUG::DEBUG_COPY_IMG(LPBYTE pBUF);
	static
	void DEBUG_FILE_LOAD(void);
	static
	BOOL CKOP_DEBUG::GET_EDIT_INT(CWnd *pWndForm, UINT nID, int *pNUM, int nMIN=0, int nMAX=0);
	static
	BOOL CKOP_DEBUG::GET_EDIT_DBL(CWnd* pWndForm, UINT nID, double *p, double fMIN=0, double fMAX=0);
	static
	LPCTSTR CKOP_DEBUG::TIME_STR(LPCTSTR header = NULL, LPCTSTR footer = NULL);
	static
	void CKOP_DEBUG::SAVE_WINDOW(LPCTSTR title);
	static
	void CKOP_DEBUG::SAVE_CSVDAT(LPCTSTR title, float buf[], int ycnt, int wid, LPCTSTR header=NULL, F_CONV = NULL);
	//-----------------
	//-----------------
	//-----------------
	static
	BOOL CKOP_DEBUG::GET_IMAGE_INFO(LPCTSTR pszFileName, BITMAP *pBITMAP);
	static
	BOOL LOAD_IMAGE(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP);
	static
	void SAVE_IMAGE(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP);
	//-----------------
	//-----------------
	//-----------------
	//-----------------
	//-----------------

};

