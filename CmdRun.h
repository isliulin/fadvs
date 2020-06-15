#pragma once
#include "paramdef.h"
#include "GTS400Ex.h"
#include <Afxtempl.h>
#include "RFIDPublic.h"
#include "ImgStatic.h"
#include "LogFile.h"
#include "SocketUDP.h"
enum K_RUN_STATUS
{
	K_RUN_STS_NONE,
	K_RUN_STS_RUN,
	K_RUN_STS_PAUSE,
	K_RUN_STS_STOP,
	K_RUN_STS_EMERGENCY,
	K_RUN_STS_ALARM
};

class CCmdRun
{
private:
	static UINT ThreadValveReset(LPVOID lparam);
	static UINT ThreadHome(LPVOID lparam);
	static UINT ThreadTestImage(LPVOID lparam);
	static UINT ThreadDetect(LPVOID lparam);
	static UINT ThreadClean(LPVOID lparam);
	static UINT ThreadLog(LPVOID lparam);
	bool WaitPasueEnd(void);
	bool KeepStillWhenPause(void);//�ֽ⶯����������ͣʱ����ԭ״
public:
	int  RunErrorID;//���д������
	CTime m_StarT;
	CTime m_EndT;
	int nTempCount;
	CCriticalSection m_csLock;
	bool m_bImageUse;
	bool m_bIsCleanValve;//2016-03-14
	int m_nCleanValveNum;//2016-03-14
	int m_nDetectImageIndex;
	int  nTempLackStatic;//ȱ������Ŀͳ��//2016-01-16
	int  nFind;
	int  nCleanValveID;
    int  padLackTotal;
	int  pcsDoneFromStart;
	bool RunDispFSelect[3];//����ʱ����ѡ��
	bool V9RestOK;
	bool AutoRunCp;//�Զ���ɵ�����
	BOOL V9DotRest;//�㽺RST
	bool m_bV9FirstDisp;//�㽺����λ���һ�ε㽺
	bool m_bFirstDisp;//�㽺��ͣ��㽺����λ���һ�ε㽺
	bool m_bRunLock;
	bool m_bTestRunStop;
	bool m_bValveSelect[3];
	bool m_bPlcRunError;
	bool m_bRepair;
	bool m_bHoming;
	bool m_bRefresh;
	bool m_bValveReset;
	bool m_bStartRun;
	bool m_bDispenseSuccess;
	bool m_bTestDispense;
	bool m_bCleanNeedle;
	bool m_bDischarge;
	bool m_bSearchMark;
	bool m_bSlotDetect;
	bool m_bMeasureHeight;
	bool m_bPadDetecting; 
    bool bIsExitClean;//2016-01-16 add
	bool m_bDoorOpen;
    bool m_bStepOne;
	bool m_bNeedleCalibrate; //��ͷ�궨�� 20180814
	bool m_bNeedleAutoAdjust;//�Զ������� 20180814
	bool m_bAutoNeedleErr; //�Զ��������
	tgProductParam ProductParam;
	tgV9Param V9Parm[3];//��̨V9����
	tgPos m_tOffset, m_tMarkCenter;
	double m_dRotateAngle;
	double m_dMarkAcceptance;
	GTS400Ex *m_pMv;
	CListBox *m_pListBox;
	CLogFile *m_pLog;
	K_RUN_STATUS m_tStatus;
	K_RUN_STATUS m_LastStatus;
	bool  bIsThreadRunning;
	bool m_bIsPause;
	bool m_bIsSupend;
	double dTempValue1,dTempValue2;
	double dTempDelay;

	tagDPostion *PDposting;
	tagDPostion * PDpostingBug;
	bool *m_pPadDetect;
	bool *m_pTransitionDetect;
	int m_nPadNumber;
	tagStautDis StautDisp;
	int DisCompCount;
	bool ManalRun;//�ֶ�
	CStringList *m_LogInfo;

    CTime m_TimeCleanRemindSt;
	
	CCmdRun(void);
	~CCmdRun(void);

	//////UDP���
	struct GLUE_MSG m_Glue_msg;
	//SocketUDP m_SocketUDP;
	bool bMesDataUpdate;
	bool bMaterialCodeNew;//ɨ�������...

	void Create(GTS400Ex *pMv, CListBox *pListBox, CLogFile *pLog);
	void AddMsg(CString strText);
	void PutLogIntoList(CString& str);

	// �����߳�
	void RunThreadLog();
	void RunThreadPlay();
	void RunThreadHome();
	void RunThreadTest();
	void RunThreadTestImage();
	void RunThreadValveReset();
	void RunThreadDetect();

	void Pause();
	void Stop();
	void Emergency();
	void Alarm(int nID);
	void AlarmOnly(int nID);
	void SetLamp();
	BOOL IsExitRun();
	void RunStop();
	// �����㽺
	bool SingleDot(int nDisNumber);
	//�����㽺Ϊ�ۺ�ģʽ
	bool SingleDotComplex(double x,double y,double za,double zb,double zc,bool bVertical,bool bV9Reset= true);
	// �����㽺
	bool SingleDot(double x,double y,double za,double zb,double zc,bool bV9Reset = true);
	// �����㽺�������ģʽ
	bool RunSingleDot(double x,double y,double za,double zb,double zc,bool RunDot_RestV9,bool bMultiMode);
	// �㽺ģʽ����������ģʽ
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dCircleRadius,double dCircleDistance,bool RunDot_RestV9);
	// �����㽺�����߶�ģʽ
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dLength,bool bVertical,int DisBufCout,bool RunDot_RestV9);
	// �����㽺--�߶�ģʽ��չ
	bool RunSingleDotComplex(double x,double y,double za,double zb,double zc,double dLength,bool bVertical,int DisBufCout,bool RunDot_RestV9);
	// �㽺ģʽ��������ģʽ
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dRectWidth,double dRectLength,int DisBufCout,bool RunDot_RestV9);
	// �㽺ģʽ�����Զ���ģʽ
	bool RunSingleDot(double x,double y,double za,double zb,double zc,CList<tgPos,tgPos&> &list,bool RunDot_RestV9);
	// �㽺ģʽ��������ģʽ
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dCircleRadius,bool RunDot_RestV9);
	bool RestDisP();
	void PosInit();
	BOOL ExtandTrack(double offsetX,double offsetY,double offsetZ,double dAngle);
	void Run();
	void TestRun();
	void DispenseOperation();
	bool SingleComplexDispense();
	bool SingleDispense();
	bool SecondDispense();
	bool ThirdDispense();
	void Home();
	void ValveReset();
	
	void OneValveRepaire(bool *bIsDispense);
	void TwoValveRepaire(bool *bIsDispense);
	void ThreeValveRepaire(bool *bIsDispense);

	//��ϴ
	bool CleanNeedle(bool Z_ON);
	void CleanValves();
	void CleanThreadPlay();
	void CleanSuspend();
	int DisDotCtrl();//���Ƶ㽺��
	// �㽺λ
	bool DispensingPos(bool Z_OFF);
	// ȡͼλ
	bool StartGrabImagePos();
	bool EndGrabImagePos();
	bool GrabImagePos(int nIndex,bool bIsSafety);
	// �Խ�λ
	bool TestDotPos(bool Z_ON);
	bool TestDispense(int nIndex,bool bDispense);
	bool TestDispense();
	// �Ž�λ
	bool DischargePosition(bool Z_ON,bool bDefoam=false);
	bool DischargePositionEX();
	// �������λ
	bool SlotDetectPosition();
	// �ƶ������λ��
	bool MoveToHeightPosition(int nIndex);
   tgPos PosAfterMarkComp(tgPos posIn);
	// ���
	HEIGHT_STATUS MeasureHeight(double& dOffsetZ);
	// ����Ƿ��ڵ�һ��λ��
	bool CheckMPdotFirst(void);
	double CapRestDistance(double dispul,long m_dispsetcount, long *discount,double *dispdistance,double dFirstCompVol=0.0);
	double CapRestDistance(double dispul,double compVol,long m_dispsetcount, long *discount,double *dispdistance,double dFirstCompVol=0.0);
	void ZEROClearn(void);
	void ClearPadDetect();
	void CopyPadDtect();
	void InitPadDetect();

	// ͼ��
	bool FindTwoCenter();
	bool FindTwoMark();
	bool SlotDetection();
	bool ScanInspect();
	void PadDetect();
	bool MoveToCenter();
	void TestImage();
	void TestImageTwo();
	bool FindImageOffset(double &dOffsetX,double &dOffsetY,double &dAngle,bool bTest);
	bool FindTwoImageOffset(tgPos tgStartPos,tgPos tgEndPos,double &dOffsetX,double &dOffsetY,double &dRotateAngle,bool bTest);
	bool FindOneImageOffset(tgPos tgImagePos,double &dOffsetX,double &dOffsetY,bool bTest);
	bool SpliceInspect(bool bSplice);
    bool SpliceInspectEx(bool bSplice);
	bool SpliceInspectUpGrade(bool bSplice);
	bool IsInRect(double x,double y,CRect rect);
	bool SplicePadDetection(bool *pResult,int nStartColumn,int &nEndColumn,CRect RectOriginal);

	//�޸�
	bool SelectDispense(double x,double y,double za,double zb,double zc);
	void SelectDispense(bool *bIsDispense);
	//֧��λ��
	double CalculateAngle(double dX,double dY);
	int	Getquforpoint(double dX,double dY);
	double Getagforpoint(double dX,double dY);
	void Rotation(double vx,double vy,double dAngle,double &dx,double &dy);
	tgPos GetPadDispensePos(int nFirstRow,int nFirstColumn,int nSecondRow,int nSecondColumn,int nThirdRow,int nThirdColumn);
	tgPos GetPadCameraPos(int nFirstRow,int nFirstColumn,int nSecondRow,int nSecondColumn,int nThirdRow,int nThirdColumn);
	tgPos GetPadDispensePos(int nIrregularRow,int nIrregularColumn);
	tgPos GetPadCameraPos(int nIrregularRow,int nIrregularColumn);
	tgPos GetFirstCameraPos();
	tgPos GetLastCameraPos();

	bool MoveZ(int nAxisNo,double z,bool bWaitStop);
	bool SynchronizeMoveZ(double z,bool bWaitStop);
	bool SynchronizeMoveZ(double za,double zb,double zc,bool bRunStatus, bool bWaitStop);
	bool WaitStopZ(int nAxisNo, unsigned long lTimeout);
	bool SynchronizeMoveXY(double x,double y,bool bWaitStop);
	bool WaitStopXY(unsigned long lTimeout);
	bool MoveZ(int nAxisNo,double z,double vel,double acc,bool bWaitStop);
	bool SynchronizeMoveZ(double z,double vel,double acc,bool bWaitStop);
	bool SynchronizeMoveZ(double za,double zb,double zc,double dVel,double dAcc,bool bRunStatus,bool bWaitStop);
	bool SynchronizeMoveXY(double x,double y,double vel,double acc,bool bWaitStop);
	bool IsXCommandAt(double dPosX);
	bool IsYCommandAt(double dPosY);
	bool IsZCommandAt(int nAxisNo, double dPosZ);
	bool IsXInPosition(double dPosX);
	bool IsYInPosition(double dPosY);
	bool IsZInPosition(int nAxisNo, double dPosZ);
	bool WaitInPlaceX(double dPosX,long lTimeout);
	bool WaitInPlaceY(double dPosY,long lTimeout);
	bool WaitInPlaceZ(int nAxisNo, double dPosZ,long lTimeout);
	// ������״̬���ٶ�
	bool SetAxisProfile(VelType type);
	// Z����������ȫλ
	bool MoveToZSafety();
	// ��ͷ�Ӵ����
	BOOL CheckHeightEx(int nAxisNo);
	// �������˶�
	bool RailMotor(bool bRun,bool bPositive = true);
	// �Խ���ϴ
	bool CleanAndTest(bool bFill);

	// ��ȡ������
	bool ReadContactSensorValue(double *pValue);
	bool ReadHeightSensorValue(double *pValue);
	bool IsRotatingLightOpen();
	bool IsRotatingLightClose();
	bool MoveToPadDetectPos();

	bool IsRunStop();
	bool IsRunAlarm();
	bool IsRunEmc();
	bool IsExisAlarm();
	bool RunZUp(double za,double zb,double zc);
	bool RunValveOperation();

	// ��ͷ���������ԭ��
	bool AdjustMotorHome();
	// ��ͷ�����������λ
	bool AdjustMotorZero();
	// ������ͷ
	bool AdjustNeedle();
	//��ȷ������ͷ
	bool AdjustNeedleAccurate();
	bool AdjustNeedle(double dOffsetA,double dOffsetB,double dOffsetC,double dOffsetD);
	// ��ͷ�����������ϴλ
	bool AdjustMotorToCleanPos();
	// ���õ������״̬���ٶ�
	bool SetAdjustMotorProfile(VelType type);
	// �Զ����롪���ƶ����㽺λ
	bool MoveToAdjustPos(int nIndex);
	// �Զ�����
	bool AutoAdjustNeedle(int nIndex);
	bool AutoAdjustNeedle();
	bool FindBlobCenter(double &dCenterX,double &dCenterY);
	bool FindNeedleModel(double &dCenterX,double &dCenterY,double &dScore);
	void DisableRegTable(bool bDisable);
	void ComputeNeedleGap();
	bool WaitAutoNeedleSys(bool bX,bool bY,bool bZA,bool bZB,bool bZC,bool bA,bool bB,bool bC,bool bD);
	bool TargetISZoneA(int Num);
	BOOL CheckHeightEx(int nAxisNo,double &zOffset);
	bool NeedleHeightCheck();
	bool GlueCheck();
	int ReadURLText(CString URL,CString &ServerIP,int &ServerPort,int &FCompanyCode);
	CString PickOutURLMsg(const CString key, const CString magbuf);
	CString ReadProdutMag(const CString key, const CString magbuf);
	bool FindProdutMag(const CString key, const CString magbuf);
};