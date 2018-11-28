// DlgDevOffline.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ImgAnaForDS.h"
#include "DlgDevOffline.h"
#include "afxdialogex.h"


// CDlgDevOffline �_�C�A���O

IMPLEMENT_DYNAMIC(CDlgDevOffline, CDialogEx)

CDlgDevOffline::CDlgDevOffline(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDevOffline::IDD, pParent)
{
	m_width  = G_SS.OFF_WIDTH ;
	m_height = G_SS.OFF_HEIGHT;
}

CDlgDevOffline::~CDlgDevOffline()
{
}

void CDlgDevOffline::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_width);
	DDV_MinMaxInt(pDX, m_width, 100, 4000);
	DDX_Text(pDX, IDC_EDIT2, m_height);
	DDV_MinMaxInt(pDX, m_height, 100, 4000);
}


BEGIN_MESSAGE_MAP(CDlgDevOffline, CDialogEx)
END_MESSAGE_MAP()


// CDlgDevOffline ���b�Z�[�W �n���h���[


void CDlgDevOffline::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	if (!UpdateData(TRUE)) {
		return;
	}
	G_SS.OFF_WIDTH  = m_width;
	G_SS.OFF_HEIGHT = m_height;
	((CImgAnaForDSApp*)AfxGetApp())->WriteProfile();

	CDialogEx::OnOK();
}
