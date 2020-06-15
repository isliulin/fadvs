
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
	bool TemperatureCheck();  //�¶ȼ��
	void CanEnterStop();
	void InitManager();  //��ʱ��λȨ��
	bool  TimeStarC;
	CFont * f;
	bool ReadEorrTxt(void);
	CString  *PlcError;
	int ErrorCount;
	long ErrorVal;
	long lRegisterStatus;
	long ErrorPlcData;
	double dRemainSpan; ///ʣ��ʱ��ͳ��
    double dValveRemainSpan;//����ʣ��ʱ��
	bool PlcEorrTag;
	bool bRecordError;
	bool bLogGlueEnable;//���ƽ�ˮ����д��־����ֹ�����д����־�ڼ�¼��
	afx_msg void OnBnClickedBtnInfoClearError();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//CBitmapButton m_Home;
	//CBitmapButton m_Rest;
	bool PlcRTag;
	bool bStartTime;  //��¼�Ƿ��Ѿ���ʼ��ʱ
	DWORD StartTime; //��¼Ȩ�޸ı�ʱ��
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
