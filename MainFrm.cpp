
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgPrompt.h"
#include "MainFrm.h"
#include "TSCtrlSysView.h"
#include "DlgLogin.h"
#include "DlgProudSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)	
	ON_COMMAND_RANGE(IDS_TOOL_LOGIN, IDS_TOOL_IO, &CMainFrame::OnToolCmd)

	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_CMD_REOPEN_FILE, &CMainFrame::OnCmdReopenFile)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_SYS_INFO, &CMainFrame::OnViewSysInfo)
	ON_COMMAND(ID_LOGON, &CMainFrame::OnLogon)
	ON_COMMAND(ID_LOGOFF, &CMainFrame::OnLogoff)

	ON_MESSAGE(K_MAIN_MSG, &CMainFrame::OnMessage)
	ON_MESSAGE(WM_HOTKEY,&CMainFrame::OnHotKey)

	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_LOGIN,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_CTRL,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_PARAM_DISPENSING,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_PARAM_VALVE,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_PARAM,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_CAMERA_LIVE,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_CAMERA_STOP,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_MARK,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_TEST,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_LACK,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_REPAIR,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_DEBUG,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_RUN,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_PAUSE,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_STOP,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_VALVE_RESET,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_IO,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_RUN,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_NEEDLE,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_AUTONEEDLE,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_HEIGHT,&CMainFrame::OnUpdateCmdHome)
	ON_UPDATE_COMMAND_UI(IDS_TOOL_BALANCE,&CMainFrame::OnUpdateCmdHome)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	IDS_APP_STATUS,
	IDS_COMPILE_NO,
	IDS_CMD_COUNT,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	m_bAdmin = FALSE;
	IsV9SetDlgVisible = false;
	bIsCtrlDlgVisible = false;
	bIsDebugNeedleVisible = false;
	bIsDebugHeightVisible = false;
	bIsAlignDlgVisible = false;
	bIsHeightDlgVisible = false;
	bIsNeedleDlgVisible = false;
	bIsRCDDlgVisible = false;
	bIsCustomVisible = false;
	bIsCalibVisible = false;
	bAutoDlgVisible = false;
	bImagePosVisible = false;
	bIsHeightCalibVisible = false;
	m_pLog= new CLogFile(CFunction::GetDirectory() + "system\\LogFile.log");
	CString str = "��ʼ�����!";
	m_pLog->log(str);
}

CMainFrame::~CMainFrame()
{
	if(m_pLog)
	{
		delete m_pLog;
	}
}

void CMainFrame::OnOutputWnd()
{
}

void CMainFrame::OnPropertiesWnd()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// ���ڳ־�ֵ�����Ӿ�����������ʽ
	OnApplicationLook(theApp.m_nAppLook);
	//if (!m_wndMenuBar.Create(this))
	//{
		//TRACE0("δ�ܴ����˵���\n");
		//return -1;      // δ�ܴ���
	//}

	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// ��ֹ�˵����ڼ���ʱ��ý���
	//CMFCPopupMenu::SetForceMenuFocus(FALSE);

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	//{
	//	TRACE0("δ�ܴ���������\n");
	//	return -1;      // δ�ܴ���
	//}
	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |  
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1),IDR_MAINFRAME_256);
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME_256, 0, 0, FALSE,0,0,IDB_BITMAP1);
	m_wndToolBar.EnableTextLabels();
	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// �����û�����Ĺ���������:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	//StatusBar 
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���7
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	//m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	//// �������񽫴�������࣬��˽���ʱ��������ͣ��:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// CMfcOutLookBar -NavigationBar:
	//ID_VIEW_NAVIGATION:defined in Resource.h;
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndPosInfo, 350))
	{
		TRACE0("δ�ܴ�����������\n");
		return -1;      //δ�ܴ���
	}

	// �Ѵ��� Outlook����Ӧ���������ͣ����
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// OutPut window
	if (!CreateDockingWindows())
	{
		TRACE0("δ�ܴ���ͣ������\n");
		return -1;
	}
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	//m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndProperties);
   

	// ���ù�������ͣ�����ڲ˵��滻
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ���ÿ���(��ס Alt �϶�)�������Զ���
	CMFCToolBar::EnableQuickCustomization();
	//if (CMFCToolBar::GetUserImages() == NULL)
	//{
	//	// �����û�����Ĺ�����ͼ��
	//	if (m_UserImages.Load(_T(".\\UserImages.bmp")))
	//	{
	//		m_UserImages.SetImageSize(CSize(16, 16), FALSE);
	//		CMFCToolBar::SetUserImages(&m_UserImages);
	//	}
	//}

	// ���ò˵����Ի�(���ʹ�õ�����)
	// TODO: �������Լ��Ļ������ȷ��ÿ�������˵�������һ���������
	CList<UINT, UINT> lstBasicCommands;
	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	//////////////////////////////////////////////////////////////////////////
	g_pFrm = this;

	m_CmdRun.Create(&theApp.m_Mv400, &m_wndOutput.m_wndOutputBuild,m_pLog);
	theApp.m_SysUser.LoadUserData();
	g_pFrm->m_CmdRun.DisableRegTable(true);//����ע��� 
	::RegisterHotKey(this->GetSafeHwnd(),1001,MOD_CONTROL,'R');

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.style = WS_OVERLAPPED | WS_CAPTION// | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
    cs.hMenu = NULL;
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// �����������
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI,AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
	{
		TRACE0("δ�ܴ����������\n");
		return FALSE; // δ�ܴ���
	}
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CDlgPosInfo& tree, int nInitialWidth)
{
	CWindowDC dc(NULL);

	bar.SetMode2003();
	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_RIGHT))
	{
		return FALSE; // δ�ܴ���
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();
	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(FALSE);
	static UINT uiPageID = 1;
	// �ɸ��������Զ����أ��ɵ�����С�������ܹر�
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE| AFX_CBRS_RESIZE;
	CRect rectDummy(0, 0, 0, 0);
	//tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	tree.Create(IDD_DLG_INFO, &bar);
	bNameValid = strTemp.LoadString(IDS_FOLDERS);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);
	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);
	bar.SetButtonsFont(&afxGlobalData.fontBold);
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* ɨ��˵�*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// ���ཫִ�������Ĺ���

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// Ϊ�����û������������Զ��尴ť
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::OnToolCmd(UINT ID)
{
	bool bSaveConfigure = false;
	switch(ID)
	{
	case IDS_TOOL_LOGIN://�û���¼
		OnLogon();
		break;
	case IDS_TOOL_CTRL://�˶�����
		if((m_CmdRun.m_tStatus != K_RUN_STS_PAUSE)&&(m_CmdRun.m_tStatus != K_RUN_STS_RUN))
		{
			m_dlgCtrl.DoModal();
		}
		break;
	case IDS_TOOL_PARAM_DISPENSING://�㽺����
		if(m_CmdRun.m_tStatus != K_RUN_STS_RUN)
		{
			m_dlgProd.DoModal();
		}
		break;
	case IDS_TOOL_PARAM_VALVE://�������
		if(m_CmdRun.m_tStatus != K_RUN_STS_RUN)
		{
			m_V9Param.DoModal();
		}
		break;
	case IDS_TOOL_PARAM:
		if(m_CmdRun.m_tStatus != K_RUN_STS_RUN)
		{
			ShowParamDialog();
		}
		break;
	case IDS_TOOL_CAMERA_LIVE:
		g_pView->m_ImgStatic.ImgLive();
		break;
	case IDS_TOOL_CAMERA_STOP:
		g_pView->m_ImgStatic.ImgStop();
		break;
	case IDS_TOOL_MARK:
		m_dlgSetMark.DoModal();
		//if(g_pFrm->m_CmdRun.ProductParam.visionMode==TWO_BLOB_MODE||g_pFrm->m_CmdRun.ProductParam.visionMode==ONE_BLOB_MODE)
		//{
		//	m_dlgBlobSet.DoModal();
		//}
		//else if(g_pFrm->m_CmdRun.ProductParam.visionMode==VISION_TWO_MARK||g_pFrm->m_CmdRun.ProductParam.visionMode==ONE_MARK_MODE)
		//{
		//	//m_dlgMarkSet.DoModal();
		//	m_dlgSetMark.DoModal();
		//}
		//else if(g_pFrm->m_CmdRun.ProductParam.visionMode==PATTERN_MODE)
		//{
		//	m_dlgPattern.DoModal();
		//}
		break;
	case IDS_TOOL_TEST:
		if(m_CmdRun.ManalRun)
		{
			m_CmdRun.RunThreadTest();
		}
		break;
	case IDS_TOOL_LACK:   //ago,this tool btn is used for paddetected;now is used for autoneedle;
		//m_dlgCfg.SetParam(&bSaveConfigure);
		//if(m_dlgCfg.DoModal()==IDOK)
		//{
		//	if(bSaveConfigure)
		//	{
		//		::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);
		//	}
		//}
		m_dlgPadDetect.DoModal();
		break;
	case IDS_TOOL_AUTONEEDLE:
		//m_dlgAutoNeedle.DoModal();
		if(AfxMessageBox("ȷ��Ҫ��ʼ�Զ�����ô��",MB_YESNO)==IDYES)
		{
		m_dlgNeedleAdjust.DoModal();	
		}		
		break;

	case IDS_TOOL_REPAIR:
		{
			CDlgPrompt dlg;
			dlg.SetShowText("����[�޸�]��ѡ��[ȷ��]������[ƽ�����]��ѡ��[ȡ��]...");
			if(IDOK==dlg.DoModal())
			{
				m_dlgRepair.RuleDeviceAdjust(false);
				m_dlgRepair.DoModal();
			}
			else
			{
				m_dlgRepair.RuleDeviceAdjust(true);
				m_dlgRepair.DoModal();
			}
		}		
		break;
	case IDS_TOOL_NEEDLE:
		//m_dlgNeedle.DoModal();
		if(theApp.m_SysUser.m_CurUD.level>GM_ADMIN)
		{
           m_dlgNeedleCamera.DoModal();
		}
		else
		{
           CDlgNeedleOne dlg;
		   dlg.DoModal();
		}
				
		{
         //CDlgNeedleOne dlg;
		// dlg.DoModal();
		}
		break;
	case IDS_TOOL_HEIGHT:
		m_dlgHeight.DoModal();
		break;
	case IDS_TOOL_BALANCE:
		m_dlgCfg.SetParam(&bSaveConfigure);
		if(m_dlgCfg.DoModal()==IDOK)
		{
			if(bSaveConfigure)
			{
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);
			}
		}
		break;
	case IDS_TOOL_DEBUG:
		//ShowDebugDialog();
		m_dlgRunInfo.DoModal();
		break;
	case IDS_TOOL_RUN://����
		if (m_CmdRun.m_tStatus == K_RUN_STS_PAUSE)
		{
			m_CmdRun.m_tStatus = K_RUN_STS_RUN;
		}
		else
		{
			if ((m_CmdRun.m_tStatus == K_RUN_STS_STOP)&&theApp.m_Mv400.GetInput(theApp.m_tSysParam.inEmergecy))
			{
				m_CmdRun.RunThreadPlay();			 			 
			}
		} 
		break;
	case IDS_TOOL_PAUSE://��ͣ
		if (m_CmdRun.m_tStatus == K_RUN_STS_RUN)
		{
			m_CmdRun.Pause();
		}
		break;
	case IDS_TOOL_STOP://ֹͣ
		m_CmdRun.ManalRun = true;
		m_CmdRun.RunStop();
		break;
	case IDS_TOOL_VALVE_RESET://���帴λ
		if (m_CmdRun.ManalRun)
		{
			m_CmdRun.RestDisP();
		}
		break;
	case IDS_TOOL_IO://IO�ź�
		ShowIODialog();
		break;
	default:
		break;
	}
}

void CMainFrame::ShowLoadCtrlDialog()
{
	if(m_V9Param.GetSafeHwnd() == NULL)
	{
		m_V9Param.Create(IDD_DlG_V9SET, this);
	}
	m_V9Param.ShowWindow( SW_SHOW);
}

void CMainFrame::ShowParamDialog()
{
	CPropertySheet sheet("��������");
	//CPageUld pageUld;
	CPagePLC pagePLC;
	CPageFactory pageFactory;
	//sheet.AddPage(&pageUld);
	sheet.AddPage(&pagePLC);
	if(theApp.m_SysUser.m_CurUD.level>GM_ADMIN)
	{
		sheet.AddPage(&pageFactory);
	}
	sheet.DoModal();
}
void CMainFrame::ShowUnLoadCtrlDialog()
{
}

LRESULT CMainFrame::OnMessage(WPARAM wParam, LPARAM lParam)
{
	char *p = (char *)lParam;
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(int(wParam)), CString(p) );	

	return 0;
}

void CMainFrame::SetPanelTextEx(UINT ID, CString strText)
{
	SendMessage(K_MAIN_MSG, WPARAM(ID), LPARAM(strText.GetBuffer()));
	strText.ReleaseBuffer();
}

//�޸����������
void CMainFrame::SetWindowTextEx()
{
	CString strTemp;
	//CString str=g_pDoc->GetPathName();
	//strTemp.Format("%s [%s]", theApp.m_tSysParam.szApp, g_pDoc->GetFileName());
	strTemp.Format("%s", theApp.m_tSysParam.szApp);
	if(g_pDoc != NULL)
	{
		if(g_pDoc->IsModified())
		{
			strTemp += " *";
		}
	}
	SetWindowText(strTemp);
}

void CMainFrame::ShowOffsetDialog()
{	
}

void CMainFrame::ShowPixelDialog()
{
}

void CMainFrame::ShowIODialog()
{
	if(m_dlgIO.GetSafeHwnd() == NULL)
	{
		m_dlgIO.Create(IDD_DlG_IO, this);
		m_dlgIO.ShowWindow(SW_SHOW);
	}
	else
	{
		m_dlgIO.ShowWindow(SW_SHOW);
		m_dlgIO.SetTimer(0,200,NULL);
	}
}

void CMainFrame::ShowProductDialog()
{
	if(m_dlgProd.GetSafeHwnd() == NULL)
	{
		m_dlgProd.Create(IDD_DLG_PROUD, this);
	}
	m_dlgProd.ShowWindow( SW_SHOW);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_DELETE:
		break;
	default:
		return 	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
	}
	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
		OnKeyDown(pMsg->wParam, 1, 0);
	else if(pMsg->message == WM_KEYUP)
		OnKeyUp(pMsg->wParam, 1, 0);
	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_DELETE:
	
		break;
	default:
		return 	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
	}
	CFrameWndEx::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnCmdReopenFile()
{
	// TODO: Add your command handler code here
	g_pDoc->SetModifiedFlag(FALSE);
}

void CMainFrame::OnClose()
{
	g_pFrm->m_CmdRun.DisableRegTable(true);//����ע��� 
	// TODO: Add your message handler code here and/or call default
	//g_pView->m_ImgStatic.ImgFree();
	if( IDOK != AfxMessageBox("��ȷ���Ƿ��˳�����", MB_OKCANCEL) )
	{
		return;
	}
	CString str="";
	if(PLC_KEYENCE == theApp.m_tSysParam.PlcType)
	{
		if(!theApp.m_Serial.DisconnectPlc(str))
		{
			AfxMessageBox("PLC�Ͽ�ʧ�ܣ�");
		}
	}
	m_CmdRun.Stop();
	theApp.m_Mv400.SetOutput(K_ALL_PORTS, FALSE);
	//F_CarmGard.CloseCarm();
	CFunction::DelayEx(0.05);
	gt_bClose = TRUE;
	//theApp.m_Mv400.FreeGE(); //add by wqj 2011.5.20
	CFrameWndEx::OnClose();
}

void CMainFrame::OnViewSysInfo()
{
	// TODO: Add your command handler code here
	m_wndNavigationBar.ShowTab(&m_wndPosInfo, TRUE, FALSE, FALSE);
}

void CMainFrame::OnLogon()
{
	// TODO: Add your command handler code here
	CDlgLogin log;
	if(log.DoModal() == IDOK)
	{
		m_bAdmin = TRUE;
	}
}

void CMainFrame::OnLogoff()
{
	// TODO: Add your command handler code here
	m_bAdmin = FALSE;
}

void CMainFrame::OnUpdateCmdHome(CCmdUI *pCmdUI)//
{
	// TODO: Add your command update UI handler code here 
	if(pCmdUI->m_nID==IDS_TOOL_CAMERA_LIVE)
	{
		pCmdUI->Enable(FALSE);
	}
	if(pCmdUI->m_nID==IDS_TOOL_CAMERA_STOP)
	{
		pCmdUI->Enable(FALSE);
	}
	if (pCmdUI->m_nID==ID_FILE_OPEN)//���ļ�
	{ 
		pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem);
	}
	if (pCmdUI->m_nID== ID_FILE_SAVE)//�����ļ�
	{ 
		pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem&&theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	}
	if (pCmdUI->m_nID==IDS_TOOL_CTRL)//�˶�����
	{
		pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem);
	}
	if (pCmdUI->m_nID==IDS_TOOL_PARAM_VALVE)//�������
	{   
		pCmdUI->Enable(m_CmdRun.m_tStatus != K_RUN_STS_RUN&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if (pCmdUI->m_nID==IDS_TOOL_PARAM_DISPENSING)//�㽺��������
	{
		pCmdUI->Enable(m_CmdRun.m_tStatus != K_RUN_STS_RUN&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if(pCmdUI->m_nID==IDS_TOOL_TEST)
	{
		pCmdUI->Enable(m_CmdRun.m_tStatus == K_RUN_STS_STOP&&!theApp.m_EncryptOp.m_bLockSystem&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if(pCmdUI->m_nID==IDS_TOOL_MARK)
	{
		pCmdUI->Enable(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if(pCmdUI->m_nID==IDS_TOOL_REPAIR)
	{
		pCmdUI->Enable(/*theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL&&*/!theApp.m_EncryptOp.m_bLockSystem);
	}
	if(pCmdUI->m_nID==IDS_TOOL_NEEDLE)
	{
		pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if(pCmdUI->m_nID==IDS_TOOL_AUTONEEDLE)
	{
		pCmdUI->Enable(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if(pCmdUI->m_nID==IDS_TOOL_LACK)
	{
		//pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem);               //δ����+ֹͣ״̬
		pCmdUI->Enable(m_CmdRun.m_tStatus== K_RUN_STS_STOP&&!theApp.m_EncryptOp.m_bLockSystem&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if(pCmdUI->m_nID==IDS_TOOL_HEIGHT)
	{
		pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if(pCmdUI->m_nID==IDS_TOOL_BALANCE)
	{
		pCmdUI->Enable(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if(pCmdUI->m_nID==IDS_TOOL_DEBUG)
	{
		pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem);
	}
	if (pCmdUI->m_nID==IDS_TOOL_RUN)//����
	{
		pCmdUI->Enable(((m_CmdRun.m_tStatus != K_RUN_STS_NONE&&m_CmdRun.ManalRun&&!theApp.m_EncryptOp.m_bLockSystem)||(!m_CmdRun.ManalRun&&m_CmdRun.m_tStatus == K_RUN_STS_PAUSE&&!theApp.m_EncryptOp.m_bLockSystem))&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if (pCmdUI->m_nID==IDS_TOOL_PAUSE)//��ͣ
	{
		pCmdUI->Enable(m_CmdRun.m_tStatus != K_RUN_STS_NONE&&m_CmdRun.ManalRun&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if (pCmdUI->m_nID==IDS_TOOL_STOP)//ֹͣ
	{
		pCmdUI->Enable(m_CmdRun.m_tStatus != K_RUN_STS_NONE&&m_CmdRun.ManalRun&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL));
	}
	if (pCmdUI->m_nID==IDS_TOOL_VALVE_RESET)//���帴λ
	{   
		pCmdUI->Enable(m_CmdRun.ManalRun&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if (pCmdUI->m_nID== IDS_TOOL_IO)//IO�ź�
	{
		pCmdUI->Enable(m_CmdRun.m_tStatus != K_RUN_STS_RUN&&!theApp.m_EncryptOp.m_bLockSystem);
	}
	if(pCmdUI->m_nID==IDS_TOOL_PARAM)
	{
       pCmdUI->Enable(!theApp.m_EncryptOp.m_bLockSystem/*&&theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL*/);
	}
}

void CMainFrame::OnUpdateCmdSetio(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CMainFrame::ShowDebugDialog()
{
	CDlgPrompt m_dlgPrompt;
	m_dlgPrompt.SetShowText("��ȷ�ϲ�Ʒ�����Ƿ���ȷ�������ȷ�����룻��ȡ���˳�");
	if(IDOK==m_dlgPrompt.DoModal())
	{
		m_dlgProd.DoModal();
	}
	else
	{
		return;
	}
	m_dlgPrompt.SetShowText("��ȷ����ͷ��ࡢ��ͷλ��");
	if(IDOK==m_dlgPrompt.DoModal())
	{
		m_dlgNeedle.DoModal();
	}
	else
	{
		return;
	}
	m_dlgPrompt.SetShowText("��ȷ�ϽӴ����λ��У����ͷ");
	if(IDOK==m_dlgPrompt.DoModal())
	{
		m_dlgHeight.DoModal();
	}
	else
	{
		return;
	}
	m_dlgPrompt.SetShowText("��ȷ�ϵ㽺λ�á���ϴλ�á��Խ�λ�á�ȡͼλ�����λ������λ");
	if(IDOK==m_dlgPrompt.DoModal())
	{
		m_dlgCtrl.DoModal();
	}
	else
	{
		return;
	}
	m_dlgPrompt.SetShowText("��ȷ��Mark��ģ��ͼ��");
	if(IDOK==m_dlgPrompt.DoModal())
	{
		//m_dlgMarkSet.DoModal();
		m_dlgSetMark.DoModal();
	}
}

// �ȼ���Ӧ����
LONG CMainFrame::OnHotKey(WPARAM wPARAM, LPARAM lPARAM)
{
	if (wPARAM == 1001)
	{
		CUserRegister regidter;
		regidter.DoModal();
	}
	return 0;
}