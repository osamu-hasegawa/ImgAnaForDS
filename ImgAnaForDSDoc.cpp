
// ImgAnaForDSDoc.cpp : CImgAnaForDSDoc �N���X�̎���
//

#include "stdafx.h"
// SHARED_HANDLERS �́A�v���r���[�A�T���l�C���A����ь����t�B���^�[ �n���h���[���������Ă��� ATL �v���W�F�N�g�Œ�`�ł��A
// ���̃v���W�F�N�g�Ƃ̃h�L�������g �R�[�h�̋��L���\�ɂ��܂��B
#ifndef SHARED_HANDLERS
#include "ImgAnaForDS.h"
#endif

#include "ImgAnaForDSDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImgAnaForDSDoc

IMPLEMENT_DYNCREATE(CImgAnaForDSDoc, CDocument)

BEGIN_MESSAGE_MAP(CImgAnaForDSDoc, CDocument)
END_MESSAGE_MAP()


// CImgAnaForDSDoc �R���X�g���N�V����/�f�X�g���N�V����

CImgAnaForDSDoc::CImgAnaForDSDoc()
{
	// TODO: ���̈ʒu�� 1 �x�����Ă΂��\�z�p�̃R�[�h��ǉ����Ă��������B

}

CImgAnaForDSDoc::~CImgAnaForDSDoc()
{
}

BOOL CImgAnaForDSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���̈ʒu�ɍď�����������ǉ����Ă��������B
	// (SDI �h�L�������g�͂��̃h�L�������g���ė��p���܂��B)

	return TRUE;
}




// CImgAnaForDSDoc �V���A����

void CImgAnaForDSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �i�[����R�[�h�������ɒǉ����Ă��������B
	}
	else
	{
		// TODO: �ǂݍ��ރR�[�h�������ɒǉ����Ă��������B
	}
}

#ifdef SHARED_HANDLERS

// �T���l�C���̃T�|�[�g
void CImgAnaForDSDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ���̃R�[�h��ύX���ăh�L�������g�̃f�[�^��`�悵�܂�
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// �����n���h���[�̃T�|�[�g
void CImgAnaForDSDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// �h�L�������g�̃f�[�^���猟���R���e���c��ݒ肵�܂��B 
	// �R���e���c�̊e������ ";" �ŋ�؂�K�v������܂�

	// ��:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImgAnaForDSDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImgAnaForDSDoc �f�f

#ifdef _DEBUG
void CImgAnaForDSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImgAnaForDSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImgAnaForDSDoc �R�}���h