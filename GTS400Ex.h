#pragma once
#include "paramdef.h"
#include "gts.h"
#include "ExtMdl.h"
#include "LogFile.h"
#define K_AXIS_X	1
#define K_AXIS_Y	2
#define K_AXIS_ZA	3
#define K_AXIS_ZB	4
#define K_AXIS_ZC	5
#define K_AXIS_U	6//V9#1
#define K_AXIS_V	7//V9#2
#define K_AXIS_W	8//V9#

#define K_AXIS_A	9//V9#2 X�������
#define K_AXIS_B	10//V9#2 Y�������
#define K_AXIS_C	11//V9#3 X�������
#define K_AXIS_D	12//V9#3 Y�������
#define K_ALL_AXIS  -1//ȫ��V9
#define K_SELECT_AXIS -2// ѡ����
#define K_ALL_PORTS		-1
#define K_NONE_PORTS	-2
#define K_PI		3.1415926

#define WM_MSG_OUTPUT_EVENT	WM_USER + 0x100//0x0400
//wparam:input no
//lparam: -2:�½��أ� 0/1����̬�� 2��������

#define WM_MSG_INPUT_EVENT	WM_USER + 0x101//0x0400
//wparam:input no
//lparam: -2:�½��أ� 0/1����̬�� 2��������

#define WM_MSG_POS_EVENT	WM_USER + 0x102//0x0400
//wparam:axis pos double[4]
//lparam:no used
#define   TYPE_INT_NOLOOK   1
#define   TYPE_INT_LOOKAHEAD 2
#define   TYPE_TRAP 0
#define   AXIS_NUM 3
#define   CARD_NUM 2

const long g_GOOGOL_STATUS_RESERVED_00		= 1 << 0;
const long g_GOOGOL_STATUS_DRIVE_ALARM		= 1 << 1;
const long g_GOOGOL_STATUS_RESERVED_02		= 1 << 2;
const long g_GOOGOL_STATUS_RESERVED_03		= 1 << 3;
const long g_GOOGOL_STATUS_POSITION_ERROR	= 1 << 4;
const long g_GOOGOL_STATUS_POSITIVE_LIMIT	= 1 << 5;
const long g_GOOGOL_STATUS_NEGATIVE_LIMIT	= 1 << 6;
const long g_GOOGOL_STATUS_SMOOTH_STOP		= 1 << 7;
const long g_GOOGOL_STATUS_ABRUPT_STOP		= 1 << 8;
const long g_GOOGOL_STATUS_MOTOR_ACTIVATED	= 1 << 9;
const long g_GOOGOL_STATUS_PROFILE_RUNNING	= 1 << 10;
const long g_GOOGOL_STATUS_MOTOR_ARRIVED	= 1 << 11;
const long g_GOOGOL_STATUS_RESERVED_12		= 1 << 12;
const long g_GOOGOL_STATUS_RESERVED_13		= 1 << 13;

const long g_GOOGOL_MOTION_MODE_TCURVE	= 0;
const long g_GOOGOL_MOTION_MODE_JOG		= 1;
const long g_GOOGOL_MOTION_MODE_PT		= 2;
const long g_GOOGOL_MOTION_MODE_EGEAR	= 3;
const long g_GOOGOL_MOTION_MODE_FOLLOW	= 4;

extern BOOL gt_bClose;
class GTS400Ex
{
public:
	short rtn;
	BOOL m_bStopEvent;
	BOOL m_bInitOK;
	tgSysParam *m_pSysParam;
	CListBox *m_pListBox;
	CLogFile *m_pLog;
	unsigned short m_outputSts[CARD_NUM];
	unsigned short m_inputSts[CARD_NUM];
	BOOL m_bExit;//��ͣ���»�����˳�ʱ��true;
	HWND m_hWnd;
	int ctrlMode;
	TCrdPrm crdPrm;//����ϵ������
	GTS400Ex(void);
	~GTS400Ex(void);

	//////////////////////////////////////////////////////////////////////////
	//1.��ʼ��
	BOOL InitGE(CListBox *pListBox, tgSysParam *pSysParam, CLogFile *pLog);
	static void __stdcall CommandHandle(char *command,short error);
	void SetMessageWnd(HWND hWnd);
	void CheckGTStatus();

	//2.��λ
	BOOL Home(double dHighVel, double dLowVel,double dAcc);
	BOOL HomeAxis(unsigned short axis, double pos, double offset, double homeOffset, double vel_high, double vel_low,double dAcc);
	BOOL HomeServoAixs(unsigned short axis, double pos, double offset, double vel_high, double vel_low, double dAcc);
	bool HomeXY();
	//3.�ͷ�
	void FreeGE();
	void CloseGE();
	BOOL IsInitOK();

	//��λģʽ��
	void Move(int iAxisNO, double dPos, double dVel,double dAcc, BOOL bWaitStop, BOOL bCheckPos = TRUE);
	void MoveDot(int iAxisNO, double dPos, double dVel,double dAcc, BOOL bWaitStop, BOOL bCheckPos=TRUE);////X1Y1Z1U1
	void MoveXY(double dX, double dY, double dVel, double dAcc, BOOL bWaitStop, BOOL bCheckPos = TRUE);
	
	void StopMove();
	void WaitStop();
	void WaitStop(int iAxisNo);
	void WaitValvesStop();
	bool ListWaitStop();
	void Stop(int iAxisNo);
	void Stop();
	void EStop();
	
	//Jogģʽ
	bool ContinuousMove(int iAxisNo,double dVel,double dAcc,bool bPositiveDir);

	//////////////////////////////////////////////////////////////////////////
	//button move
	void MoveBtn(int iAxisNo, int iMvSpeed/*0,1,2*/, int iDir/*-1, +1*/);

	double m_dCurVelEditXY, m_dCurVelEditZ;

	//������
	//void ListMoveXYZ(double dX, double dY, double dZ,double dVel, double dAcc, BOOL bCheckPos=TRUE);//ǰհָ��

	//void ListAddLine(double x, double y, double z,double dVel, double dAcc);
	void ListAddLine(double x, double y, double dVel, double dAcc);
	void ListDelay(double dTime);//s
	void ListOpen();
	BOOL ListIsOpen();

	//r�����ӻ��� ���Ż��� iDirection:1��ʱ����ת�� -1˳ʱ����ת
	void ListAddArc(double dEx, double dEy, double r, short iDirection, double dVel, double dAcc);

	//dAngle:�Ƕ�-360~360
	//void ListAddArc(double dCx, double dCy, double z, double dAngle);
	void ListAddArc(double dEx,double dEy,double dCx, double dCy,short Dir,double dVel,double dAcc);
	void AddBufGear(int gearAxisNo,double dPos);
	bool AllListRun();

	//IO
	void SetOutput(int bitNO, BOOL bOn, BOOL bUpdate=FALSE);
	BOOL GetInput(int bitNO);
	BOOL GetOutput(int bitNO);

	//����
	BOOL IsMove();
	BOOL IsMove(int iAxisNo);
	BOOL IsMoveList();
	BOOL IsMoveAdjustMotor();
	double GetPos(int iAxisNO);
	double GetVel(int iAxisNo);
	bool CheckBufSpace();
	void Delay(double dTime);
	void DoEvents();
	double GetEncPos(int iAxisNo);

	void Trans2Pulse(double *pX, double *pY, double *pZ, double *pU, double *pVel, double *pAcc);
	void Trans2Pulse(int iAxisNo,double *pX,  double *pVel,double *pAcc);

	void Trans2MM(double *pX, double *pY, double *pZ, double *pU, double *pVel, double *pAcc);
	void Trans2MM(int iAxisNo,double *pX,  double *pVel,double *pAcc);

   
	void CheckPos(double *pX, double *pY, double *pZ, double *pU);
	void CheckPos(int iAxisNo,double *pX);


	void AddMsg(CString strText);

	//
	//////////////////////////////////////////////////////////////////////////
	void ListStartEx(double x, double y, double z, tgPos nextPos, double dSynVel, double dSynAcc, BOOL bClearList);
	void ListAddLineEx(double x, double y, double z);
	void ListAddArcEx(double dEx, double dEy, double z,double dCx, double dCy, double dCr,double dArcAngle/*����*/, double dLen, BOOL bAddDir);

	//////////////////////////////////////////////////////////////////////////
	void TransPosToGTEx(double &x, double &y, double &z, double &u);
	void TransPosToGTEx(double &x, double &y, double &z);
	void TransPosToGTEx(double &x, double &y);
	BOOL WarnCheckPos(double *pX, double *pY, double *pZ, double *pU);
	BOOL WarnCheckPos(double *pX, double *pY, double *pZa, double *pZb, double *pZc);

	MOTION_STATUS SetProfile(int nAxisNo,double dVel,double dAcc);
	MOTION_STATUS SetProfile(int nAxisNo,double dVel,double dAcc,double dDec,double dJerk,double dStartVel);
	MOTION_STATUS GetProfile(int nAxisNo,double *pVel,double *pAcc,double *pDec,double *pStartVel,int *pSmoothTime);
	void Trans2Pulse(int nAxisNo,double *pos,double *vel,double *acc,double *dec,double *jerk);
	MOTION_STATUS Move(int nAxisNo,double dPos,bool bWaitStop,bool bCheckPos);
	MOTION_STATUS WaitStop(int nAxisNo,unsigned long lTimeout);
	MOTION_STATUS WaitMotionComplete(int nAxisNo,bool *pCompleted,unsigned long lTimeout);
	MOTION_STATUS IsMoveCompleted(int nAxisNo,bool *pCompleted);
	bool IsDriverAlarm(int nAxisNO);
	bool IsPositiveLimitOn(int nAxisNo);
	bool IsNegativeLimitOn(int nAxisNo);
	short SetCrdPrm(BOOL lookAhead,short crd,short dimension,double T=5.0,double accMax=0.5);
	short SetAllAxisTrap();
	void CheckListRun(void);
	double GetAdVolVal(int chanalID);
	int GetAdValue(int chanalID);
	void AxisClrsts(unsigned short axis);
	MOTION_STATUS GetAxisSts(int iAxisNo,long *pSts);

	bool SetStopDec(int nAxisNo,double decSmoothStop,double decAbrupStop);
};
