
// ImgAnaForDS.h : ImgAnaForDS �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��


// CImgAnaForDSApp:
// ���̃N���X�̎����ɂ��ẮAImgAnaForDS.cpp ���Q�Ƃ��Ă��������B
//

class CImgAnaForDSApp : public CWinApp
{
public:
	CImgAnaForDSApp();

	HANDLE	m_hMutex;

	void CImgAnaForDSApp::GetProfile(void);
	void CImgAnaForDSApp::WriteProfile(void);

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CImgAnaForDSApp theApp;
