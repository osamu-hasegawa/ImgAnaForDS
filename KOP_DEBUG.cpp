#include "StdAfx.h"
#include "resource.h"

#include "KOP_DEBUG.h"
#include "KOP.h"
#include "EX.h"
#include "CSV.h"
#include <math.h>
#include "DS.h"

//------------------------------------
BYTE	CKOP_DEBUG::IMG_BUF[MAX_WIDTH*MAX_HEIGHT*4];
BYTE	CKOP_DEBUG::IMG_SUB[MAX_WIDTH*MAX_HEIGHT*4];
//------------------------------------
//DEBUG用
int		CKOP_DEBUG::DBG_REQ_CODE;
int		CKOP_DEBUG::DBG_FILEIN;
int		CKOP_DEBUG::DBG_PAUSE;
int		CKOP_DEBUG::DBG_NEXT;
int		CKOP_DEBUG::DBG_FRAME_IDX;
int		CKOP_DEBUG::DBG_FILE_IDX;
BYTE	CKOP_DEBUG::DBG_BUF[MAX_WIDTH*MAX_HEIGHT*4];
int		CKOP_DEBUG::DBG_IMG_BPP;
int		CKOP_DEBUG::DBG_IMG_WIDTH;
int		CKOP_DEBUG::DBG_IMG_HEIGHT;
#if 1//2016.01.01
static
int	G_FRAME_IDX_OF_DRAW;
static
int	G_bSKIP_FRAME;
#endif
int CKOP_DEBUG::DEBUG_PROC(LPBYTE pBUF)
{
	switch (CKOP_DEBUG::DBG_REQ_CODE) {
	case 1:
	{
		CString	file(CKOP_DEBUG::TIME_STR("IMAGE-", ".bmp"));
		TCHAR	path[MAX_PATH];
		sprintf_s(path, _countof(path), "%s\\IMAGES\\%s", GetDirDoc(), file);
		int	SKIP_FRAME_BAK = G_bSKIP_FRAME;
		G_bSKIP_FRAME = 1;//全てSKIP


		CKOP_DEBUG::SAVE_IMAGE(path, CKOP_DEBUG::IMG_BUF, CDS::GetWidth(),  CDS::GetHeight(), CKOP::BMP_BYTE_CNT);
		::MessageBeep(MB_OK);
		G_bSKIP_FRAME = SKIP_FRAME_BAK;
		CKOP_DEBUG::DBG_REQ_CODE = 0;
	}
	break;
	case 2:
		if (G_FRAME_IDX_OF_DRAW != CKOP_DEBUG::DBG_FRAME_IDX) {
			//次のフレームが着たらポーズに戻す
			G_bSKIP_FRAME = 1;//全フレームスキップ
			CKOP_DEBUG::DBG_REQ_CODE = 0;
		}
		else {
			CKOP_DEBUG::DBG_REQ_CODE = CKOP_DEBUG::DBG_REQ_CODE;
		}
	break;
	default:
		CKOP_DEBUG::DBG_REQ_CODE = 0;
	break;
	}
	return(0);
}

int CKOP_DEBUG::DEBUG_GET_PIXEL(LPBYTE pBUF, int x, int y)
{
	int	v;
	int	i;

	if (x > DBG_IMG_WIDTH) {
		x %= DBG_IMG_WIDTH;
	}
	if (y > DBG_IMG_HEIGHT) {
		y %= DBG_IMG_HEIGHT;
	}
	i = x + y * CKOP_DEBUG::DBG_IMG_WIDTH;
	i*= CKOP_DEBUG::DBG_IMG_BPP;

	return(pBUF[i]);
}
void CKOP_DEBUG::DEBUG_COPY_IMG(LPBYTE pBUF)
{
	int	xo = 0;
	int	yo = 0;
	int		x, y, xx, yy, h, i = 0;
	int		v;

	for (y = 0; y < CDS::GetHeight(); y++) {
		for (x = 0; x < CDS::GetWidth(); x++) {
			yy = (yo+y);
			if (CKOP::m_bBOTTOMUP) {
				yy = CKOP::BMP_HEI-yy-1;
			}
			h = xo + x + yy*CKOP::BMP_WID;

			v = DEBUG_GET_PIXEL(CKOP_DEBUG::DBG_BUF, x, yy);

			if (CKOP::BMP_BYTE_CNT == 1) {
				CKOP_DEBUG::IMG_BUF[i] = v;
				i++;
			}
			else {
				h *= 1;//CKOP::BMP_BYTE_CNT;
				for (int q = 0; q < CKOP::BMP_BYTE_CNT; i++, h++, q++) {
					if (q != 3) {
						CKOP_DEBUG::IMG_BUF[i] = v;
					}
				}
			}
		}
	}
}
void CKOP_DEBUG::DEBUG_FILE_LOAD(void)
{
	WIN32_FIND_DATA
			fd;
	HANDLE	h;
	CString	path(GetDirDoc());
	CString	file;
	int		q,
			cnt = 0;
	BOOL	bFAILED = FALSE;

	path += "\\IMAGES";

	if (!::PathFileExists(GetDirPath(path))) {
		::CreateDirectory(GetDirPath(path), NULL);
	}

	path += "\\*.bmp";

	while (TRUE) {
		q = 0;
		h = ::FindFirstFile(path, &fd);
		if (h == INVALID_HANDLE_VALUE) {
			return;
		}
		while (TRUE) {
			cnt++;
			if (q == CKOP_DEBUG::DBG_FILE_IDX) {
				//hit
				CString	file(GetDirDoc());
				BITMAP	bitmap;
				file += "\\IMAGES\\";
				file += fd.cFileName;

				CKOP_DEBUG::GET_IMAGE_INFO(file, &bitmap);
				CKOP_DEBUG::DBG_IMG_BPP     = bitmap.bmBitsPixel /8;
				CKOP_DEBUG::DBG_IMG_WIDTH   = bitmap.bmWidth;
				if (bitmap.bmHeight < 0) {
				CKOP_DEBUG::DBG_IMG_HEIGHT  = -bitmap.bmHeight;
				}
				else {
				CKOP_DEBUG::DBG_IMG_HEIGHT  =  bitmap.bmHeight;
				}
				if (CKOP_DEBUG::DBG_IMG_WIDTH > MAX_WIDTH || CKOP_DEBUG::DBG_IMG_HEIGHT > MAX_HEIGHT || CKOP_DEBUG::DBG_IMG_BPP > 4) {
					//skip
					bFAILED = TRUE;
					CKOP_DEBUG::DBG_FILE_IDX++;
				}
				else {
					CKOP_DEBUG::LOAD_IMAGE(file, CKOP_DEBUG::DBG_BUF, CKOP_DEBUG::DBG_IMG_WIDTH, CKOP_DEBUG::DBG_IMG_HEIGHT, CKOP_DEBUG::DBG_IMG_BPP);
					CKOP_DEBUG::DBG_FILE_IDX++;
					FindClose(h);
					return;
				}
			}
			if (!FindNextFileA(h, &fd)) {
				FindClose(h);
				cnt = q+1;
				if (CKOP_DEBUG::DBG_FILE_IDX >= cnt) {
					CKOP_DEBUG::DBG_FILE_IDX %= cnt;
				}
				else if (bFAILED) {
				}
				break;
			}
			q++;
		}
	}
}

LPCTSTR CKOP_DEBUG::TIME_STR(LPCTSTR header, LPCTSTR footer)
{
	static
	TCHAR	buf[MAX_PATH];
	CTime	tim = CTime::GetCurrentTime();
	if (header == NULL) {
		header = "";
	}
	if (footer == NULL) {
		footer = "";
	}
	sprintf_s(buf, _countof(buf), "%s%s%s", header, tim.Format("%Y%m%d-%Hh%Mm%Ss"), footer);
	return(buf);
}

void CKOP_DEBUG::SAVE_WINDOW(LPCTSTR title)
{
	CString	path(title);
	path += ".PNG";
	CKOP::SAVE_WINDOW(AfxGetMainWnd(), path);
}
void CKOP_DEBUG::SAVE_CSVDAT(LPCTSTR title, float buf[], int ycnt, int wid, LPCTSTR header, F_CONV conv)
{
	CCSV	csv;
	int		q = 0;
	CString	path(CKOP::SAVE_DIR);

	if (path.Right(1) != "\\") {
		path += "\\";
	}
	path += CKOP::SERIALNO;
	path += "_";
	path += title;
	path += ".csv";
	for (int h = 0; h < wid; h++) {
		csv.set(0, h, I2S(h));
	}
	if (conv != NULL) {
		for (int i = 0; i < ycnt; i++) {
			for (int h = 0; h < wid; h++, q++) {
				double	f = conv(buf[q]);
				csv.set(1+i, h, F3S(f));
			}
		}
	}
	else {
		for (int i = 0; i < ycnt; i++) {
			for (int h = 0; h < wid; h++, q++) {
				csv.set(1+i, h, F3S(buf[q]));
			}
		}
	}
	csv.save(path, header);
}
#if 0//2016.01.01
//------------------------------------
void CKOP_DEBUG::SET_SKIP_BY_FILTER(int FILTER)
{
	if (CKOP_DEBUG::DBG_FILEIN || CKOP_DEBUG::DBG_PAUSE) {
		G_bSKIP_FRAME = 1;//全フレームスキップ
		return;
	}
	switch (FILTER) {
	case 0://生
		G_bSKIP_FRAME = 0;	//全フレーム
	break;
	case 1://フィルター画像
		G_bSKIP_FRAME = 2;	//1/2
	break;
	case 2://台形補正後画像
		G_bSKIP_FRAME = 3;	//1/3
	break;
	case 3://シェーディング補正後画像
		G_bSKIP_FRAME = 4;	//1/4
	break;
	default:
		G_bSKIP_FRAME = 0;	//全フレーム
	break;
	}
}
#endif
BOOL CKOP_DEBUG::GET_IMAGE_INFO(LPCTSTR pszFileName, BITMAP *pBITMAP)
{
	HBITMAP	hBMP = CKOP::LOAD_BITMAP(pszFileName);
	if (hBMP == NULL) {
		return(FALSE);
	}
	CBitmap	bmp;

	bmp.Attach(hBMP);
	bmp.GetBitmap(pBITMAP);

	bmp.Detach();
	DeleteObject(hBMP);
	return(TRUE);
}

#if 1//2015.09.10
BOOL CKOP_DEBUG::LOAD_IMAGE(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP)
{
	HBITMAP	hBMP = CKOP::LOAD_BITMAP(pszFileName);
	if (hBMP == NULL) {
		return(FALSE);
	}
	BOOL	rc = FALSE;
	CBitmap	bmp;
	BITMAP	bitmap;
	int		cnt = width * height;
	int		size;
	LPBYTE	p, po = NULL;

	bmp.Attach(hBMP);

	bmp.GetBitmap(&bitmap);
	if (bitmap.bmWidth != width || bitmap.bmHeight != height) {
		goto skip;
	}
	if (bitmap.bmBitsPixel != 8 && bitmap.bmBitsPixel != 24) {
		goto skip;
	}
	size = cnt*bitmap.bmBitsPixel/8;
	if ((po = (LPBYTE)malloc(size)) == NULL) {
		goto skip;
	}
	bmp.GetBitmapBits(size, po);

	if (BPP != 1 && BPP != 3 && BPP != 4) {
		goto skip;
	}
	if (BPP == 1) {
		if (bitmap.bmBitsPixel == 8) {
			memcpy(pBuf, po, size);
		}
		else {
			p = po;
			for (int i = 0; i < size; i+=3) {
				*pBuf++ = *p++;
						   p++;
						   p++;
			}
		}
	}
	else if (BPP == 4) {
		if (bitmap.bmBitsPixel == 32) {
			memcpy(pBuf, po, size);
		}
		else {
			p = po;
			for (int i = 0; i < size; i+=3, p+=3) {
				*pBuf++ = *p  ;
				*pBuf++ = *p  ;
				*pBuf++ = *p  ;
				*pBuf++ = 255 ;
			}
		}
	}
	else {
		if (bitmap.bmBitsPixel == 24) {
			memcpy(pBuf, po, size);
		}
		else {
			p = po;
			for (int i = 0; i < size; i++) {
				*pBuf++ = *p  ;
				*pBuf++ = *p  ;
				*pBuf++ = *p++;
			}
		}
	}
	rc = TRUE;
skip:
	if (po != NULL) {
		free(po);
	}
	bmp.Detach();
	DeleteObject(hBMP);
	return(rc);
}
void CKOP_DEBUG::SAVE_IMAGE(LPCTSTR pszFileName, LPBYTE pBuf, int width, int height, int BPP)
{
	int		cnt = width*height;
	int		size = cnt*BPP;
	BITMAPINFO bmpInfo;
    LPBYTE lpPixel;
    //DIBの情報を設定する
    bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth=width;
	if (CKOP::m_bBOTTOMUP) {
	bmpInfo.bmiHeader.biHeight=+height;
	}
	else {
	bmpInfo.bmiHeader.biHeight=-height;
	}
    bmpInfo.bmiHeader.biPlanes=1;
    bmpInfo.bmiHeader.biBitCount=24;
    bmpInfo.bmiHeader.biCompression=BI_RGB;
    //DIBSection作成
    HBITMAP hBmp=CreateDIBSection(NULL,&bmpInfo,DIB_RGB_COLORS,(void**)&lpPixel,NULL,0);
	if (BPP == 1) {
		LPBYTE	p, po = (LPBYTE)lpPixel;
		if (po == NULL) {
			return;
		}
		p = po;
		for (int i = 0; i < cnt; i++) {
			*p++ = *pBuf;
			*p++ = *pBuf;
			*p++ = *pBuf++;
		}
	}
	else if (BPP == 3) {
		memcpy(lpPixel, pBuf, size);
	}
	else if (BPP == 4) {
		LPBYTE	p, po = (LPBYTE)lpPixel;
		if (po == NULL) {
			return;
		}
		p = po;
		for (int i = 0; i < cnt; i++) {
			*p++ = *pBuf++;
			*p++ = *pBuf++;
			*p++ = *pBuf++;
			pBuf++;
		}
	}
	CKOP::SAVE_BITMAP(hBmp, pszFileName);

    DeleteObject(hBmp);
}
#endif