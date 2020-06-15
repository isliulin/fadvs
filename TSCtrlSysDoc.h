
// TSCtrlSysDoc.h : CTSCtrlSysDoc 类的接口
//


#pragma once


class CTSCtrlSysDoc : public CDocument
{
protected: // 仅从序列化创建
	CTSCtrlSysDoc();
	DECLARE_DYNCREATE(CTSCtrlSysDoc)

// 属性
public:

// 操作
public:
	CString GetModeFile(int iIndex);//0:Mark1, 1: Mark2
	CString GetPadModelFile(int nIndex,bool bModel = true);
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CTSCtrlSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
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


