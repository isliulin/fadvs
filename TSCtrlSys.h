
// TSCtrlSys.h : TSCtrlSys 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
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

#define WM_MSG_GLUE_EVENT	WM_USER + 0x200  //MES 系统胶量更改事件
#define WM_MSG_ADJUSTFINISH_CAM WM_USER+0x300 //自动对针___相机完成
#define WM_MSG_ADJUSTFINISH_NEEDLEONE WM_USER+0x301 //自动对针___针头1完成
#define WM_MSG_ADJUSTFINISH_NEEDLETWO  WM_USER+0x302 //自动对针___针头2完成
#define WM_MSG_ADJUSTFINISH_NEEDLETHREE WM_USER+0x303 //自动对针___针头3完成

// CTSCtrlSysApp:
// 有关此类的实现，请参阅 TSCtrlSys.cpp
//
#include "ComPort.h"
class CTSCtrlSysApp : public CWinAppEx
{
public:
	CTSCtrlSysApp();
	~CTSCtrlSysApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
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
	CEncryptOp m_EncryptOp;   // 加密模块
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
	double dGlueNewest;      //MES 传输胶量
	bool   bMachineEnable;   //MES 开机使能 
};

extern CTSCtrlSysApp theApp;
extern CMainFrame *g_pFrm;
extern CTSCtrlSysDoc *g_pDoc;

extern CTSCtrlSysView *g_pView;
//extern CImgStatic *g_imagStatic;
extern CMilApp milApp;
extern CRITICAL_SECTION  g_cs;

