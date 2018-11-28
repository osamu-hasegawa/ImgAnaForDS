#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include <Windows.h>

typedef BOOL (APIENTRY * P_INIT          )(HWND hWnd);
typedef VOID (APIENTRY * P_TERM          )(VOID);
typedef BOOL (APIENTRY * P_GET_DEV_CNT   )(LONG *pcnt);
typedef BOOL (APIENTRY * P_GET_DEV_NAME  )(LONG i, char* pName, DWORD size);
typedef BOOL (APIENTRY * P_SHOW_PROP     )(HWND hWnd, LONG i);
typedef BOOL (APIENTRY * P_OPEN          )(LONG i);
typedef VOID (APIENTRY * P_CLOSE         )(VOID);
typedef BOOL (APIENTRY * P_GET_BITMAPINFO)(BITMAPINFO *pbi);
typedef BOOL (APIENTRY * P_RUN           )(HWND hWnd);
typedef BOOL (APIENTRY * P_PAUSE         )(VOID);
typedef VOID (APIENTRY * P_STOP          )(VOID);
#if 1//2018.05.01
typedef BOOL (APIENTRY * P_SET_TPF       )(LONG);
#endif

class CPluginDLL
{
public:
	HMODULE				hMOD;
	P_INIT				PD_INIT;
	P_TERM				PD_TERM;
	P_GET_DEV_CNT		PD_GET_DEV_CNT;
	P_GET_DEV_NAME		PD_GET_DEV_NAME;
	P_SHOW_PROP			PD_SHOW_PROP;
	P_OPEN				PD_OPEN;
	P_CLOSE				PD_CLOSE;
	P_GET_BITMAPINFO	PD_GET_BITMAPINFO;
	P_RUN				PD_RUN;
	P_PAUSE				PD_PAUSE;
	P_STOP				PD_STOP;
#if 1//2018.05.01
	P_SET_TPF			PD_SET_TPF;
#endif

};