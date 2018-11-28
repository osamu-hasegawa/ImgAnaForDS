// DlgProgress.h : �w�b�_ �t�@�C��
// CG: ���̃t�@�C���́u�v���O���X �_�C�A���O�v�R���|�[�l���g�ɂ��ǉ�����Ă��܂��B

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress �_�C�A���O

#ifndef __DLGPROGRESS_H__
#define __DLGPROGRESS_H__

class CDlgProgress : public CDialog
{
// �\�z / �j��
public:
    CDlgProgress(UINT nCaptionID = 0);   // �W���R���X�g���N�^
    ~CDlgProgress();
#if 1//AA
	BOOL Create(LPCTSTR szCaption, CWnd *pParent, BOOL bCancelEnable, LPCTSTR szText);
#endif
	BOOL Create(LPCTSTR szCaption, CWnd *pParent, BOOL bCancelEnable=TRUE);
//	BOOL Create(CWnd *pParent=NULL);


    // �L�����Z�� �{�^���̃`�F�b�N
    BOOL CheckCancelButton();

    // �v���O���X �_�C�A���O�̏���

	void SetStatusStr(LPCTSTR lpszMessage);
	int  SetStatus(LPCTSTR fmt, ...);
// �_�C�A���O �f�[�^
    //{{AFX_DATA(CDlgProgress)
    enum { IDD = IDD_PROGRESS };
    //}}AFX_DATA

// �I�[�o�[���C�h
    // ClassWizard �́A���̈ʒu�ɉ��z�֐��𐶐����ăI�[�o�[���C�h���܂��B
    //{{AFX_VIRTUAL(CDlgProgress)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
    //}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
    BOOL m_bCancel;
	BOOL m_bSkipper;
	int		m_nRButtonCnt;
protected:
	UINT m_nCaptionID;

    BOOL m_bParentDisabled;
	BOOL m_bCentering;
	BOOL m_bCancelEnable;
	CFont
		m_fnt;
    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 

    void PumpMessages();

    // �������ꂽ���b�Z�[�W�}�b�v�֐�
    //{{AFX_MSG(CDlgProgress)
    virtual BOOL OnInitDialog();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __DLGPROGRESS_H__
