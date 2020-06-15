
// TSCtrlSysDoc.h : CTSCtrlSysDoc ��Ľӿ�
//


#pragma once


class CTSCtrlSysDoc : public CDocument
{
protected: // �������л�����
	CTSCtrlSysDoc();
	DECLARE_DYNCREATE(CTSCtrlSysDoc)

// ����
public:

// ����
public:
	CString GetModeFile(int iIndex);//0:Mark1, 1: Mark2
	CString GetPadModelFile(int nIndex,bool bModel = true);
// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CTSCtrlSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString m_strFileName;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpenDxf();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	CString GetFileName();
	void SetFileName(CString str);
};


