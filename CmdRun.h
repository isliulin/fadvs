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
	bool KeepStillWhenPause(void);//分解动作中命中暂停时保持原状
public:
	int  RunErrorID;//运行错误代码
	CTime m_StarT;
	CTime m_EndT;
	int nTempCount;
	CCriticalSection m_csLock;
	bool m_bImageUse;
	bool m_bIsCleanValve;//2016-03-14
	int m_nCleanValveNum;//2016-03-14
	int m_nDetectImageIndex;
	int  nTempLackStatic;//缺颗粒数目统计//2016-01-16
	int  nFind;
	int  nCleanValveID;
    int  padLackTotal;
	int  pcsDoneFromStart;
	bool RunDispFSelect[3];//运行时阀体选择
	bool V9RestOK;
	bool AutoRunCp;//自动完成单流程
	BOOL V9DotRest;//点胶RST
	bool m_bV9FirstDisp;//点胶阀复位后第一次点胶
	bool m_bFirstDisp;//点胶暂停或点胶阀复位后第一次点胶
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
	bool m_bNeedleCalibrate; //针头标定中 20180814
	bool m_bNeedleAutoAdjust;//自动对针中 20180814
	bool m_bAutoNeedleErr; //自动对针出错
	tgProductParam ProductParam;
	tgV9Param V9Parm[3];//三台V9参数
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
	bool ManalRun;//手动
	CStringList *m_LogInfo;

    CTime m_TimeCleanRemindSt;
	
	CCmdRun(void);
	~CCmdRun(void);

	//////UDP相关
	struct GLUE_MSG m_Glue_msg;
	//SocketUDP m_SocketUDP;
	bool bMesDataUpdate;
	bool bMaterialCodeNew;//扫描过单号...

	void Create(GTS400Ex *pMv, CListBox *pListBox, CLogFile *pLog);
	void AddMsg(CString strText);
	void PutLogIntoList(CString& str);

	// 运行线程
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
	// 单个点胶
	bool SingleDot(int nDisNumber);
	//单个点胶为综合模式
	bool SingleDotComplex(double x,double y,double za,double zb,double zc,bool bVertical,bool bV9Reset= true);
	// 单个点胶
	bool SingleDot(double x,double y,double za,double zb,double zc,bool bV9Reset = true);
	// 单个点胶——打点模式
	bool RunSingleDot(double x,double y,double za,double zb,double zc,bool RunDot_RestV9,bool bMultiMode);
	// 点胶模式——螺旋线模式
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dCircleRadius,double dCircleDistance,bool RunDot_RestV9);
	// 单个点胶——线段模式
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dLength,bool bVertical,int DisBufCout,bool RunDot_RestV9);
	// 单个点胶--线段模式扩展
	bool RunSingleDotComplex(double x,double y,double za,double zb,double zc,double dLength,bool bVertical,int DisBufCout,bool RunDot_RestV9);
	// 点胶模式——矩形模式
	bool RunSingleDot(double x,double y,double za,double zb,double zc,double dRectWidth,double dRectLength,int DisBufCout,bool RunDot_RestV9);
	// 点胶模式——自定义模式
	bool RunSingleDot(double x,double y,double za,double zb,double zc,CList<tgPos,tgPos&> &list,bool RunDot_RestV9);
	// 点胶模式——弧形模式
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

	//清洗
	bool CleanNeedle(bool Z_ON);
	void CleanValves();
	void CleanThreadPlay();
	void CleanSuspend();
	int DisDotCtrl();//控制点胶阀
	// 点胶位
	bool DispensingPos(bool Z_OFF);
	// 取图位
	bool StartGrabImagePos();
	bool EndGrabImagePos();
	bool GrabImagePos(int nIndex,bool bIsSafety);
	// 试胶位
	bool TestDotPos(bool Z_ON);
	bool TestDispense(int nIndex,bool bDispense);
	bool TestDispense();
	// 排胶位
	bool DischargePosition(bool Z_ON,bool bDefoam=false);
	bool DischargePositionEX();
	// 正反检测位
	bool SlotDetectPosition();
	// 移动到测高位置
	bool MoveToHeightPosition(int nIndex);
   tgPos PosAfterMarkComp(tgPos posIn);
	// 测高
	HEIGHT_STATUS MeasureHeight(double& dOffsetZ);
	// 电机是否在第一点位置
	bool CheckMPdotFirst(void);
	double CapRestDistance(double dispul,long m_dispsetcount, long *discount,double *dispdistance,double dFirstCompVol=0.0);
	double CapRestDistance(double dispul,double compVol,long m_dispsetcount, long *discount,double *dispdistance,double dFirstCompVol=0.0);
	void ZEROClearn(void);
	void ClearPadDetect();
	void CopyPadDtect();
	void InitPadDetect();

	// 图像
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

	//修复
	bool SelectDispense(double x,double y,double za,double zb,double zc);
	void SelectDispense(bool *bIsDispense);
	//支架位置
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
	// 设置轴状态和速度
	bool SetAxisProfile(VelType type);
	// Z轴提升到安全位
	bool MoveToZSafety();
	// 针头接触测高
	BOOL CheckHeightEx(int nAxisNo);
	// 轨道电机运动
	bool RailMotor(bool bRun,bool bPositive = true);
	// 试胶清洗
	bool CleanAndTest(bool bFill);

	// 读取传感器
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

	// 针头调整电机回原点
	bool AdjustMotorHome();
	// 针头调整电机回零位
	bool AdjustMotorZero();
	// 调整针头
	bool AdjustNeedle();
	//精确调整针头
	bool AdjustNeedleAccurate();
	bool AdjustNeedle(double dOffsetA,double dOffsetB,double dOffsetC,double dOffsetD);
	// 针头调整电机到清洗位
	bool AdjustMotorToCleanPos();
	// 设置调整电机状态和速度
	bool SetAdjustMotorProfile(VelType type);
	// 自动对针——移动到点胶位
	bool MoveToAdjustPos(int nIndex);
	// 自动对针
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