
#pragma once
#include "SocketUdp.h"
#include "StsStatic.h"
#include "afxwin.h"
#include "resource.h"
#include "EncryptOp.h"

const int BARCODE_FIELD_NUM=3;
const int BARCODE_MAX_FIELD_NUM=8;

// CDlgPosInfo dialog

class CDlgPosInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgPosInfo)

public:
	CDlgPosInfo(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	enum { IDD = IDD_DLG_INFO };
	virtual ~CDlgPosInfo();

	DECLARE_MESSAGE_MAP()
	CStsStatic m_Static;
protected:
public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnInfoReset();
	afx_msg void OnTimer(UINT id);
	CBrush m_brush;
	LRESULT OnMessageInputEvent(WPARAM wparam, LPARAM lparam);
	LRESULT OnMessageOutputEvent(WPARAM wparam, LPARAM lparam);
	LRESULT OnMessagePosEvent(WPARAM wparam, LPARAM lparam);
	LRESULT OnMessageGlueEvent(WPARAM wparam,LPARAM lparam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void DisplayWorkStat(void);
	void UpdateTimes();
	void DisplayGlueVol();

    bool UDPSendMsg();
	void AutoSetGlue(void);

	void SetAuthority();
    void UpdateArrangeTime();
	void DisplayRemainSpan();
    void DisplayValveRemainSpan();
	bool TemperatureCheck();  //温度监控
	void CanEnterStop();
	void InitManager();  //定时复位权限
	bool  TimeStarC;
	CFont * f;
	bool ReadEorrTxt(void);
	CString  *PlcError;
	int ErrorCount;
	long ErrorVal;
	long lRegisterStatus;
	long ErrorPlcData;
	double dRemainSpan; ///剩余时间统计
    double dValveRemainSpan;//阀体剩余时间
	bool PlcEorrTag;
	bool bRecordError;
	bool bLogGlueEnable;//控制胶水报警写日志：防止不间断写此日志于记录；
	afx_msg void OnBnClickedBtnInfoClearError();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//CBitmapButton m_Home;
	//CBitmapButton m_Rest;
	bool PlcRTag;
	bool bStartTime;  //记录是否已经开始计时
	DWORD StartTime; //记录权限改变时间
	long  lTimeInStop;
	bool  bStopPush;
	CSocketUdp m_uSocket;
	CRegisterCode MainRegMsg;
	afx_msg void OnBnClickedBtnInfoClearCapacity();
	afx_msg void OnBnClickedBtnInfoDebug();
	afx_msg void OnBnClickedBtnInfoMovetoCleanPos();
	afx_msg void OnBnClickedBtnInfoMovetoDischargePos();
	afx_msg void OnBnClickedBtnInfoClearSlot();
	afx_msg void OnBnClickedBtnInfoClearDetect();
	afx_msg void OnBnClickedBtnManualLoad();
	afx_msg void OnBnClickedBtnManualUnload();
	afx_msg void OnBnClickedBtnFirstInPos();
	afx_msg void OnBnClickedBtnFirstOutPos();
	afx_msg void OnEnChangeEditGlueSetting();
	afx_msg void OnBnClickedCheckInfoEnableArrange();
	afx_msg void OnBnClickedBtnInfoArrangeTime();
	afx_msg void OnBnClickedBtnValveUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonProductivityr();
	afx_msg void OnBnClickedInfoUdpSendmsg();
	bool OnReceive();
	bool ReadTemperature();
	BOOL CheckRegisterCode(CRegisterCode &tempRegMsg);
	
};
