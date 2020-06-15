
// TSCtrlSys.h : TSCtrlSys Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "paramdef.h"
#include "MainFrm.h"
#include "TSCtrlSysDoc.h"
#include "TSCtrlSysView.h"
//#include "GE400Ex.h"
#include "GTS400Ex.h"
#include "ImgStatic.h"
#include "V9Control.h"
#include "UserManage.h"
#include "MilApp.h"
#include "EncryptOp.h"
#include "vld.h"
#include "SocketUDP.h"

#define WM_MSG_GLUE_EVENT	WM_USER + 0x200  //MES ϵͳ���������¼�
#define WM_MSG_ADJUSTFINISH_CAM WM_USER+0x300 //�Զ�����___������
#define WM_MSG_ADJUSTFINISH_NEEDLEONE WM_USER+0x301 //�Զ�����___��ͷ1���
#define WM_MSG_ADJUSTFINISH_NEEDLETWO  WM_USER+0x302 //�Զ�����___��ͷ2���
#define WM_MSG_ADJUSTFINISH_NEEDLETHREE WM_USER+0x303 //�Զ�����___��ͷ3���

// CTSCtrlSysApp:
// �йش����ʵ�֣������ TSCtrlSys.cpp
//
#include "ComPort.h"
class CTSCtrlSysApp : public CWinAppEx
{
public:
	CTSCtrlSysApp();
	~CTSCtrlSysApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()


public:
	//////////////////////////////////////////////////////////////////////////
	GTS400Ex m_Mv400;
	tgSysParam m_tSysParam;
    CComPort m_Serial;
	CV9Control m_V9Control;
	CUserManage m_SysUser;
	CEncryptOp m_EncryptOp;   // ����ģ��
	bool m_bInitOk;
	BOOL m_bComOpen;
	void SysParam(BOOL bRead);
	void ProductParam(BOOL bRead);
	void BmpParam(BOOL bRead);
	void V9Param(BOOL bRead);
	virtual int ExitInstance();
	void LoadParam(void);
	void HeatSystemOn();
	void PlcPreHeatOn();
    void PlcHeatOn();
	bool ServerInfoGet();
	double dGlueNewest;      //MES ���佺��
	bool   bMachineEnable;   //MES ����ʹ�� 
};

extern CTSCtrlSysApp theApp;
extern CMainFrame *g_pFrm;
extern CTSCtrlSysDoc *g_pDoc;

extern CTSCtrlSysView *g_pView;
//extern CImgStatic *g_imagStatic;
extern CMilApp milApp;
extern CRITICAL_SECTION  g_cs;

