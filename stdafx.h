
// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �ꕔ�� CString �R���X�g���N�^�[�͖����I�ł��B

// ��ʓI�Ŗ������Ă����S�� MFC �̌x�����b�Z�[�W�̈ꕔ�̔�\�����������܂��B
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �̃R�A����ѕW���R���|�[�l���g
#include <afxext.h>         // MFC �̊g������


#include <afxdisp.h>        // MFC �I�[�g���[�V���� �N���X



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �R���� �R���g���[�� �T�|�[�g
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �R���� �R���g���[�� �T�|�[�g
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC �ɂ����郊�{���ƃR���g���[�� �o�[�̃T�|�[�g


//������������ǉ�����

/********************************************************************/
typedef struct {
	/***********************************/
	int		OFF_WIDTH;
	int		OFF_HEIGHT;
	/***********************************/
	int		ANA_WIDTH;
	int		ANA_HEIGHT;
	/***********************************/
	int		SHD_SKIPCNT;
	/***********************************/
	TCHAR	SAVE_DIR[MAX_PATH];
	/***********************************/
	int		USE_OFFLINE;
	int		AVG_ON_8BIT;
	int		FRM_MINTIME;
#if 1//2018.05.01
	int		SKIP_TPF;
#endif
	/***********************************/
#if 1//2017.04.01
	int		USE_MITSUTOYO;
	int		USE_FIXED;
	int		MTF_MODE;
	/***********************************/
#endif
#if 1//2018.05.01
	int		USE_SURUGA;//�x�̓X�e�[�W���g�p
	int		STG_WAIT_TIME;
#endif
} SYSSET, *LPSYSSET;
#if 1//2017.04.01
/********************************************************************/
typedef struct {
	/***********************************/
	int		SHADING_FILTER;
	int		SHADING_PARAM;
	int		SHADING_COLMAP;
	int		SHADING_DIAGO;
	int		SHADING_SKIPCNT;
	int		SHADING_GABVAL;
	int		SHADING_SHAD_CORR;
	/***********************************/
	int		SCALE_AVG_CNT;
	double	SCALE_PITCH_SCALE;
	double	SCALE_PITCH_PIXEL;
	int		SCALE_WEIGHT;
	int		SCALE_CHECK;
	RECT	SCALE_RECT;
	int		SCALE_FILTER;
	int		SCALE_PARAM;
	int		SCALE_SHAD_CORR;
	/***********************************/
	int		MTF_ZOOM;
	RECT	MTF_W;
	RECT	MTF_B;
	RECT	MTF_RECT_T1;
	RECT	MTF_RECT_Y1;
	RECT	MTF_RECT_T2;
	RECT	MTF_RECT_Y2;
	RECT	MTF_RECT_T3;
	RECT	MTF_RECT_Y3;
	RECT	MTF_RECT_T4;
	RECT	MTF_RECT_Y4;
	RECT	MTF_RECT_T5;
	RECT	MTF_RECT_Y5;

	int		MTF_SHAD_CORR;
	/***********************************/
	int		CIRCLE_SAVPOS;
	int		CIRCLE_BINVAL;
	int		CIRCLE_BINIMG;
	int		CIRCLE_BAKMOD;
#if 1//2018.05.01
	RECT	CIRCLE_AREA;
#endif
	int		CIRCLE_SHAD_CORR;
	/***********************************/
	int		TVD_AVG_CNT;
	int		TVD_BIN_IMG;
	int		TVD_BIN_VAL;
	int		TVD_CHECK;
	int		TVD_SHAD_CORR;
	/***********************************/
	int		ROLL_SAVPOS;
	int		ROLL_BINVAL;
	int		ROLL_BAKMOD;
	int		ROLL_CSR_LT;
	int		ROLL_CSR_RT;
	int		ROLL_SHAD_CORR;
	/***********************************/
} FIXEDPARA, *LPFIXEDPARA;
#endif
#include "tools.h"
#include <afxdlgs.h>
//AA#include <afxcontrolbars.h>
#define ROWS(x)	(sizeof(x)/sizeof(x[0]))
#define WM_CALLBACK	(WM_USER+0x500)
#pragma warning( disable : 4482 ) // �\���}��:C4482:��W���̊g���@�\���g�p����Ă��܂�: enum 'FlyCapture2::PixelFormat' �����薼�Ŏg�p����܂�
#pragma warning( disable : 4819 ) // �t�@�C���́A���݂̃R�[�h �y�[�W (932) �ŕ\���ł��Ȃ��������܂�ł��܂��B�f�[�^�̑�����h�����߂ɁA�t�@�C���� Unicode �`���ŕۑ����Ă��������B
#pragma warning( disable : 4996 ) //  This function or variable may be unsafe.
extern BOOL		G_bONLINE;
extern BOOL		G_bCANCEL;
extern BOOL		G_bDEBUG;
extern BOOL		G_bTRACE;
extern BOOL		G_bSTEP;
extern BOOL		G_bPROG;
extern HWND		G_hPROG;
extern SYSSET	G_SS;
#if 1//2017.03.20
#include "PluginDll.h"
extern CPluginDLL*
				G_PD;
extern BITMAPINFOHEADER
				G_BH;
#endif
//���������܂Œǉ�����

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


