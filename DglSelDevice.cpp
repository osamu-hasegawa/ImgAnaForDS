// DglSelDevice.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImgAnaForDS.h"
#include "DglSelDevice.h"
#include "afxdialogex.h"
#include "DS.h"

#if 1//2017.03.20
LONG	m_dvcnt;
LONG	m_pdcnt;
#endif
// CDglSelDevice ダイアログ

IMPLEMENT_DYNAMIC(CDglSelDevice, CDialogEx)

CDglSelDevice::CDglSelDevice(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDglSelDevice::IDD, pParent)
{
	m_strLastDev = GetProfileStr("APPLICATION", "LASTDEV", "");
}

CDglSelDevice::~CDglSelDevice()
{
	WriteProfileStr("APPLICATION", "LASTDEV", m_strLastDev);
}

void CDglSelDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDglSelDevice, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDglSelDevice::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDglSelDevice::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDglSelDevice::OnBnClickedButton3)
	ON_LBN_SELCHANGE(IDC_LIST1, &CDglSelDevice::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CDglSelDevice メッセージ ハンドラー


BOOL CDglSelDevice::OnInitDialog()
{
	CDialogEx::OnInitDialog();
#if 1//2017.03.20
	void TEST0(LPCTSTR dir);
	TCHAR	path[MAX_PATH];
	GetDirModule(path);
	TEST0(path);
//	TEST0("C:\\CURPOJ\\KOP-IMGANA-FOR-SONY\\CAM_SONYGIGE.20170323\\Debug");
#endif
	// TODO:  ここに初期化を追加してください
	OnBnClickedButton1();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CDglSelDevice::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	int	cur = ((CListBox*)GetDlgItem(IDC_LIST1))->GetCurSel();
#if 1//2017.03.20
	if (cur >= m_dvcnt) {
		if (!G_PD->PD_OPEN(cur-m_dvcnt)) {
			return;
		}
#if 1//2018.05.01
		((CListBox*)GetDlgItem(IDC_LIST1))->GetText(cur, m_strLastDev);
#endif
	}
	else {
#endif
	if (!CDS::ADD_CAP(CDS::GetDevName(cur))) {
		return;
	}
	if (!CDS::RENDER()) {
		return;
	}

	m_strLastDev = CDS::GetDevName(cur);
#if 1//2017.03.20
		if (G_PD != NULL) {
			FreeLibrary(G_PD->hMOD);
			delete G_PD;
			G_PD = NULL;
		}
	}
#endif

	CDialogEx::OnOK();
}


void CDglSelDevice::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	CDialogEx::OnCancel();
}


void CDglSelDevice::OnBnClickedButton1()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	((CListBox*)GetDlgItem(IDC_LIST1))->ResetContent();
	CDS::ENUM_DEVS();
	for (int i = 0; i < CDS::GetDevCnt(); i++) {
		CString	buf = CDS::GetDevName(i);

		if (!buf.CompareNoCase(_T("OFFLINE"))) {
			buf += "(カメラ無しで起動)";
		}
		((CListBox*)GetDlgItem(IDC_LIST1))->AddString(buf);
	}
#if 1//2017.03.20
	m_dvcnt = CDS::GetDevCnt();
	if (G_PD != NULL) {
		char	name[256];
		G_PD->PD_GET_DEV_CNT(&m_pdcnt);
		for (int i = 0; i < m_pdcnt; i++) {
			G_PD->PD_GET_DEV_NAME(i, name, sizeof(name));
			((CListBox*)GetDlgItem(IDC_LIST1))->AddString(name);
		}
	}
#endif
	if (
#if 1//2017.03.20
		(m_dvcnt+m_pdcnt)
#else
		CDS::GetDevCnt()
#endif
		> 0) {
		((CListBox*)GetDlgItem(IDC_LIST1))->SetCurSel(0);
		//---
	}
	else {
	}
	int	idx;

	idx = ((CListBox*)GetDlgItem(IDC_LIST1))->FindString(0, m_strLastDev);
	((CListBox*)GetDlgItem(IDC_LIST1))->SetCurSel(idx);

	OnLbnSelchangeList1();
}


void CDglSelDevice::OnBnClickedButton2()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int	cur = ((CListBox*)GetDlgItem(IDC_LIST1))->GetCurSel();
#if 1//2017.03.20
	if (cur >= m_dvcnt) {
		if (m_pdcnt > 0) {
			G_PD->PD_SHOW_PROP(this->m_hWnd, (cur-m_dvcnt));
		}
		return;
	}
#endif
	CDS::TEST02(CDS::GetDevName(cur), this->m_hWnd);
}


void CDglSelDevice::OnBnClickedButton3()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int	cur = ((CListBox*)GetDlgItem(IDC_LIST1))->GetCurSel();
	CDS::TEST09(CDS::GetDevName(cur), this->m_hWnd);
}


void CDglSelDevice::OnLbnSelchangeList1()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int	cur = ((CListBox*)GetDlgItem(IDC_LIST1))->GetCurSel();
#if 1//2017.03.20
	if (cur >= m_dvcnt) {
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
		GetDlgItem(IDOK       )->EnableWindow(TRUE);
	}
	else
#endif
	if (G_SS.USE_OFFLINE == FALSE || (cur < (CDS::GetDevCnt()-1))) {
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDOK       )->EnableWindow(TRUE);
	}
	else {
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDOK       )->EnableWindow(TRUE);
	}
}

#if 1//2017.03.20
CPluginDLL* TEST1(LPCTSTR path)
{
	HMODULE	h;
	if ((h = LoadLibrary(path)) == NULL) {
#ifdef _WIN64//2018.07.30
		DWORD err = GetLastError();
#endif
		return(NULL);
	}
	CPluginDLL*p = new CPluginDLL();
	p->PD_INIT           = (P_INIT          )GetProcAddress(h, "INIT");
	p->PD_TERM			 = (P_TERM			)GetProcAddress(h, "TERM");
	p->PD_GET_DEV_CNT	 = (P_GET_DEV_CNT	)GetProcAddress(h, "GET_DEV_CNT");
	p->PD_GET_DEV_NAME	 = (P_GET_DEV_NAME	)GetProcAddress(h, "GET_DEV_NAME");
	p->PD_SHOW_PROP		 = (P_SHOW_PROP		)GetProcAddress(h, "SHOW_PROP");
	p->PD_OPEN			 = (P_OPEN			)GetProcAddress(h, "OPEN");
	p->PD_CLOSE			 = (P_CLOSE			)GetProcAddress(h, "CLOSE");
	p->PD_GET_BITMAPINFO = (P_GET_BITMAPINFO)GetProcAddress(h, "GET_BITMAPINFO");
	p->PD_RUN			 = (P_RUN			)GetProcAddress(h, "RUN");
	p->PD_PAUSE			 = (P_PAUSE			)GetProcAddress(h, "PAUSE");
	p->PD_STOP			 = (P_STOP			)GetProcAddress(h, "STOP");
#if 1//2018.05.01
	p->PD_SET_TPF		 = (P_SET_TPF		)GetProcAddress(h, "SET_TPF");
#endif
	if (p->PD_INIT           == NULL
	 || p->PD_TERM			 == NULL
	 || p->PD_GET_DEV_CNT	 == NULL
	 || p->PD_GET_DEV_NAME	 == NULL
	 || p->PD_SHOW_PROP		 == NULL
	 || p->PD_OPEN			 == NULL
	 || p->PD_CLOSE			 == NULL
	 || p->PD_GET_BITMAPINFO == NULL
	 || p->PD_RUN			 == NULL
	 || p->PD_PAUSE			 == NULL
	 || p->PD_STOP			 == NULL) {
		 FreeLibrary(h);
		 return(NULL);
	}
	p->hMOD = h;
	return(p);
}
void TEST0(LPCTSTR dir)
{
	WIN32_FIND_DATA	fd;
	CString	path(dir);
	HANDLE	h;
#ifdef _WIN64//2018.07.30
	int tmp;
	tmp = sizeof(int);		//4
	tmp = sizeof(__int32);	//4
	tmp = sizeof(long);		//4
	tmp = sizeof(LONG);		//4
	tmp = sizeof(DWORD);	//4
	tmp = sizeof(__int64);	//8
	tmp = sizeof(LONGLONG);	//8
	tmp = sizeof(int*);		//8
	tmp = sizeof(char*);	//8
	tmp = sizeof(WPARAM);	//8
	tmp = sizeof(LPARAM);	//8
	path += _T("\\CAM_*64.DLL");
#else
	path += _T("\\CAM_*.DLL");
#endif
	h = FindFirstFile(path, &fd);
	if (h == INVALID_HANDLE_VALUE) {
		return;
	}
	while (TRUE) {
		path = dir;
		path+= "\\";
		path+= fd.cFileName;
		CPluginDLL* p = TEST1(path);
		if (p != NULL) {
			G_PD = p;
			G_PD->PD_INIT(NULL);
			break;
		}
		if (!FindNextFile(h, &fd)) {
			break;
		}    
	}
	FindClose(h);
}
#endif