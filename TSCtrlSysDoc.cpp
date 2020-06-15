
// TSCtrlSysDoc.cpp : CTSCtrlSysDoc 类的实现
//

#include "stdafx.h"
#include "TSCtrlSys.h"

#include "TSCtrlSysDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTSCtrlSysDoc

IMPLEMENT_DYNCREATE(CTSCtrlSysDoc, CDocument)

BEGIN_MESSAGE_MAP(CTSCtrlSysDoc, CDocument)
	ON_COMMAND(ID_FILE_NEW, &CTSCtrlSysDoc::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN_DXF, &CTSCtrlSysDoc::OnFileOpenDxf)
	ON_COMMAND(ID_FILE_SAVE, &CTSCtrlSysDoc::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, &CTSCtrlSysDoc::OnFileOpen)
END_MESSAGE_MAP()


// CTSCtrlSysDoc 构造/析构

CTSCtrlSysDoc::CTSCtrlSysDoc()
{
	// TODO: 在此添加一次性构造代码
	m_strFileName = "";
}

CTSCtrlSysDoc::~CTSCtrlSysDoc()
{
}

BOOL CTSCtrlSysDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	g_pDoc = this;
	return TRUE;
}




// CTSCtrlSysDoc 序列化

void CTSCtrlSysDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CTSCtrlSysDoc 诊断

#ifdef _DEBUG
void CTSCtrlSysDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTSCtrlSysDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTSCtrlSysDoc 命令

BOOL CTSCtrlSysDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
 	if (!CDocument::OnOpenDocument(lpszPathName))
 		return FALSE;
	// TODO:  Add your specialized creation code here
	if(g_pFrm != NULL)
	{
		SetPathName(lpszPathName);
	
		SetModifiedFlag(FALSE);
		g_pFrm->SetWindowTextEx();
	}
	return TRUE;
}

BOOL CTSCtrlSysDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	if(g_pFrm != NULL)
	{
	}
	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);
	g_pFrm->SetWindowTextEx();
	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}

void CTSCtrlSysDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::OnCloseDocument();
}

void CTSCtrlSysDoc::OnFileNew()
{
	// TODO: Add your command handler code here
	g_pView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_SAVE_AS,0),0);
}

//提供正常模板的文件路径
CString CTSCtrlSysDoc::GetModeFile(int iIndex)//0:Mark1, 1: Mark2
{
	CString strTemp;
	if(GRAYSCALE == g_pFrm->m_CmdRun.ProductParam.matchMode)
	{
		strTemp.Format("product\\GrayScale_%d.mod", iIndex);
	}
	else if(GEOMETRIC == g_pFrm->m_CmdRun.ProductParam.matchMode)
	{
		strTemp.Format("product\\Geometric_%d.MMF", iIndex);
	}
	return (CFunction::GetDirectory() + strTemp);
}

//提供缺颗粒模板的文件路径
CString CTSCtrlSysDoc::GetPadModelFile(int nIndex,bool bModel)
{
	CString strTemp;
	if(bModel)
	{
		if(GRAYSCALE == g_pFrm->m_CmdRun.ProductParam.matchMode)
		{
			strTemp.Format("product\\PadGrayScale_%d.mod", nIndex);
		}
		else if(GEOMETRIC == g_pFrm->m_CmdRun.ProductParam.matchMode)
		{
			strTemp.Format("product\\PadGeometric_%d.MMF", nIndex);
		}
	}
	else
	{
		strTemp.Format("product\\PadImage_%d.bmp", nIndex);
	}
	return (CFunction::GetDirectory() + strTemp);
}

void CTSCtrlSysDoc::OnFileOpenDxf()
{
}
// 保存文件
void CTSCtrlSysDoc::OnFileSave()
{
	theApp.ProductParam(FALSE);
	theApp.V9Param(FALSE);
	theApp.SysParam(FALSE);

	char szInitDir[500];
	GetCurrentDirectory(sizeof(szInitDir),szInitDir);
	CString szCurrentDirectory;
	szCurrentDirectory.Format("%s",szInitDir);
	CString szFilter	= _T("TSCtrl Files(*.ctrl)|*.ctrl||");
	CFileDialog dlg(FALSE,_T("ctrl"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
	if(IDOK!=dlg.DoModal())
	{
		return;
	}
	CString szFile = dlg.GetPathName();

	CStringList strList;
	strList.AddTail(_T("system"));
	strList.AddTail(_T("product"));

	CString command;
	command.Format("%s\\zip.exe -r -q %s",szCurrentDirectory,"param");
	for(int i=0;i<strList.GetCount();i++)
	{
		command.AppendFormat(_T(" \"%s\""),strList.GetAt(strList.FindIndex(i)));
	}

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION	processInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	CreateProcess(NULL, command.GetBuffer(command.GetLength()+1), NULL, NULL, FALSE, CREATE_NO_WINDOW,NULL, szCurrentDirectory, &startupInfo, &processInfo);
	WaitForSingleObject(processInfo.hProcess,INFINITE);
	CopyFile(szCurrentDirectory+_T("\\param.zip"),szFile,FALSE);
	DeleteFile(szCurrentDirectory+_T("\\param.zip"));
	////
	strcpy(theApp.m_tSysParam.szApp, szFile);
	theApp.SysParam(FALSE);
	g_pFrm->SetWindowTextEx();
}
// 打开文件
void CTSCtrlSysDoc::OnFileOpen()
{
	char szInitDir[500];
	GetCurrentDirectory(sizeof(szInitDir),szInitDir);
	CString szCurrentDirectory;
	szCurrentDirectory.Format("%s",szInitDir);
	CString szFilter	= _T("TSCtrl Files(*.ctrl)|*.ctrl||");
	CFileDialog dlg(TRUE,_T("ctrl"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
	if(IDOK!=dlg.DoModal())
	{
		return;
	}
	CString szFile = dlg.GetPathName();
	CFileStatus status;
	SetFileName(dlg.GetFileTitle());
	if(!CFile::GetStatus(szFile,status))
	{
		return;
	}
	CopyFile(szFile,szCurrentDirectory+_T("\\param.zip"),FALSE);
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	CString command;
	command.Format(_T("%s\\unzip.exe -q -o %s"),szCurrentDirectory,_T("param"));
	memset(&startupInfo,0,sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	CreateProcess(NULL,command.GetBuffer(command.GetLength()+1),NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,szCurrentDirectory,&startupInfo,&processInfo);
	WaitForSingleObject(processInfo.hProcess,INFINITE);
	DeleteFile(szCurrentDirectory+_T("\\param.zip"));
	////
	strcpy(theApp.m_tSysParam.szApp, szFile);
	theApp.SysParam(FALSE);
	theApp.LoadParam();
}

CString CTSCtrlSysDoc::GetFileName()
{
	return m_strFileName;
}

void CTSCtrlSysDoc::SetFileName(CString str)
{
	if(str!="")
	{
		m_strFileName = str;
	}
}