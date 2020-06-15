
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "OutputWnd.h"
#include "PropertiesWnd.h"

#include "Resource.h"
#include "DlgCtrl.h"
#include "DlgPosInfo.h"
#include "DlgIOCtrl.h"
#include "DlgProudSet.h"
#include "DlgBlobSet.h"
#include "DlgMarkSet.h"
#include "DlgPattern.h"
#include "DlgRepair.h"
#include "DlgNeedle.h"
#include "DlgNeedleOne.h"////////
#include "DlgHeight.h"
#include "CmdRun.h"
#include "V9Param.h"
#include "PageFactory.h"
#include "PagePLC.h"
#include "DlgPrompt.h"
#include "DlgSetMark.h"
#include "UserRegister.h"
#include "DlgConfigure.h"
#include "DlgPadDetect.h"
#include "DlgAlignNeedle.h"
#include "DlgCustom.h"
#include "DlgDebugHeight.h"
#include "DlgDebugNeedle.h"
#include "DlgRCD.h"
#include "DlgAutoAlign.h"
#include "DlgNeedleCamera.h"
#include "DlgNeedleAdjust.h"
#include "DlgImagePosition.h"
#include "DlgHeightCalib.h"
#include "DlgLabel.h"
#include "DlgAutoNeedle.h"
#include "DlgRunInfo.h"
#define K_MAIN_MSG	WM_USER+0x101

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); }
};

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:
	BOOL m_bAdmin;
// 操作
public:

	LRESULT OnMessage(WPARAM wParam, LPARAM lParam);	
	void OnOutputWnd();
	void OnPropertiesWnd();

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	COutlookBar       m_wndNavigationBar;
	CDlgPosInfo		  m_wndPosInfo;
	CCmdRun m_CmdRun;
	CDlgCtrl m_dlgCtrl;
	CDlgIOCtrl m_dlgIO;
	CV9Param m_V9Param;
	CDlgBlobSet m_dlgBlobSet;
	CDlgMarkSet m_dlgMarkSet;
	CDlgPattern m_dlgPattern;
	CDlgRepair m_dlgRepair;
	CDlgNeedle m_dlgNeedle;
	
	CDlgHeight m_dlgHeight;
	CDlgAutoNeedle m_dlgAutoNeedle;
	CDlgNeedleCamera  m_dlgNeedleCamera;
	CDlgNeedleAdjust m_dlgNeedleAdjust;
	CDlgProudSet m_dlgProd;
	CLogFile *m_pLog;
	CDlgSetMark m_dlgSetMark;
	CDlgConfigure m_dlgCfg;
	CDlgPadDetect m_dlgPadDetect;

	CDlgAlignNeedle m_dlgAlignNeedle;
	CDlgDebugHeight m_dlgDebugHeight;
	CDlgDebugNeedle m_dlgDebugNeedle;
	CDlgCustom m_dlgCustom;
	CDlgRCD m_dlgRCD;
	CDlgAutoAlign m_dlgAutoAlign;
	CDlgImagePosition m_dlgImagePos;
	CDlgHeightCalib m_dlgHeightCalib;
	CDlgLabel m_dlgLabel;
	CDlgRunInfo m_dlgRunInfo;

	bool IsV9SetDlgVisible;//V9窗口是否打开
	bool bIsCtrlDlgVisible;
	bool bIsDebugNeedleVisible;
	bool bIsDebugHeightVisible;
	bool bIsAlignDlgVisible;
	bool bIsHeightDlgVisible;
	bool bIsNeedleDlgVisible;
	bool bIsRCDDlgVisible;
	bool bIsCustomVisible;
	bool bIsCalibVisible;
	bool bAutoDlgVisible;
	bool bImagePosVisible;
	bool bIsHeightCalibVisible;
	CList <tgPos, tgPos&> m_rawList;//命令链表
	//CList <double, double&> m_rawRowD;//行距链表
	//CList <double, double&> m_rawColumnD;//列距链表

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnToolCmd(UINT ID);

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CDlgPosInfo& tree, int nInitialWidth);

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;

public:
	void SetPanelTextEx(UINT ID, CString strText); 

	void ShowLoadCtrlDialog();
	void ShowUnLoadCtrlDialog();
	void ShowParamDialog();
	void ShowDebugDialog();

	void SetWindowTextEx();
	void ShowOffsetDialog();
	void ShowPixelDialog();
	void ShowIODialog();
	void ShowProductDialog();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCmdReopenFile();
	afx_msg void OnClose();
	afx_msg void OnViewSysInfo();
	afx_msg void OnLogon();
	afx_msg void OnLogoff();
	afx_msg void OnUpdateCmdHome(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCmdSetio(CCmdUI *pCmdUI);
	afx_msg LONG OnHotKey(WPARAM wPARAM, LPARAM lPARAM);
};


