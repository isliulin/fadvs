#pragma once

#define GT_API extern "C" short __stdcall

#define MC_NONE                         -1

#define MC_LIMIT_POSITIVE               0
#define MC_LIMIT_NEGATIVE               1
#define MC_ALARM                        2
#define MC_HOME                         3
#define MC_GPI                          4

#define MC_ENABLE                       10
#define MC_CLEAR                        11
#define MC_GPO                          12

#define MC_DAC                          20
#define MC_STEP                         21
#define MC_PULSE                        22
#define MC_ENCODER                      23

#define MC_AXIS                         30
#define MC_PROFILE                      31
#define MC_CONTROL                      32

#define CAPTURE_HOME                    1
#define CAPTURE_INDEX                   2
#define CAPTURE_PROBE                   3

#define PT_MODE_STATIC                  0
#define PT_MODE_DYNAMIC                 1

#define PT_SEGMENT_NORMAL               0
#define PT_SEGMENT_EVEN                 1
#define PT_SEGMENT_STOP                 2

#define GEAR_MASTER_ENCODER             1
#define GEAR_MASTER_PROFILE             2
#define GEAR_MASTER_AXIS                3

#define FOLLOW_MASTER_ENCODER           1
#define FOLLOW_MASTER_PROFILE           2
#define FOLLOW_MASTER_AXIS              3

#define FOLLOW_EVENT_START              1
#define FOLLOW_EVENT_PASS               2

#define FOLLOW_SEGMENT_NORMAL           0
#define FOLLOW_SEGMENT_EVEN             1
#define FOLLOW_SEGMENT_STOP             2
#define FOLLOW_SEGMENT_CONTINUE         3

#define INTERPOLATION_AXIS_MAX          4
#define CRD_FIFO_MAX                    4096
#define CRD_MAX                         2
#define CRD_OPERATION_DATA_EXT_MAX      2

#define CRD_OPERATION_TYPE_NONE         0
#define CRD_OPERATION_TYPE_BUF_IO_DELAY 1
#define CRD_OPERATION_TYPE_LASER_ON     2
#define CRD_OPERATION_TYPE_LASER_OFF    3
#define CRD_OPERATION_TYPE_BUF_DA       4
#define CRD_OPERATION_TYPE_LASER_CMD    5
#define CRD_OPERATION_TYPE_LASER_FOLLOW 6
#define CRD_OPERATION_TYPE_LMTS_ON      7
#define CRD_OPERATION_TYPE_LMTS_OFF     8
#define CRD_OPERATION_TYPE_SET_STOP_IO  9
#define CRD_OPERATION_TYPE_BUF_MOVE     10
#define CRD_OPERATION_TYPE_BUF_GEAR     11
#define CRD_OPERATION_TYPE_SET_SEG_NUM  12

#define INTERPOLATION_MOTION_TYPE_LINE  0
#define INTERPOLATION_MOTION_TYPE_CIRCLE 1

#define INTERPOLATION_CIRCLE_PLAT_XY    0
#define INTERPOLATION_CIRCLE_PLAT_YZ    1
#define INTERPOLATION_CIRCLE_PLAT_ZX    2

#define INTERPOLATION_CIRCLE_DIR_CW     0
#define INTERPOLATION_CIRCLE_DIR_CCW    1

typedef struct TrapPrm
{
    double acc;
    double dec;
    double velStart;
    short  smoothTime;
} TTrapPrm;

typedef struct JogPrm
{
    double acc;
    double dec;
    double smooth;
} TJogPrm;

typedef struct Pid
{
    double kp;
    double ki;
    double kd;
    double kvff;
    double kaff;
    long   integralLimit;
    long   derivativeLimit;
    short  limit;
} TPid;

typedef struct ThreadSts
{
    short  run;
    short  error;
    double result;
    short  line;
} TThreadSts;

typedef struct VarInfo
{
    short id;
    short dataType;
    char  name[32];
} TVarInfo;

typedef struct CompileInfo
{
    char  *pFileName;
    short *pLineNo;
    char  *pMessage;
} TCompileInfo;

typedef struct CrdPrm
{
    short dimension;                              // 坐标系维数
    short profile[8];                             // 关联profile和坐标轴
    double synVelMax;                             // 最大合成速度
    double synAccMax;                             // 最大合成加速度
    short evenTime;                               // 最小匀速时间
    short setOriginFlag;                          // 设置原点坐标值标志,0:默认当前规划位置为原点位置;1:用户指定原点位置
    long originPos[8];                            // 用户指定的原点位置
}TCrdPrm;

typedef struct CrdBufOperation
{
    short flag;                                   // 标志该插补段是否含有IO和延时
    unsigned short delay;                         // 延时时间
    short doType;                                 // 缓存区IO的类型,0:不输出IO
    unsigned short doMask;                        // 缓存区IO的输出控制掩码
    unsigned short doValue;                       // 缓存区IO的输出值
    unsigned short dataExt[CRD_OPERATION_DATA_EXT_MAX];     // 辅助操作扩展数据
}TCrdBufOperation;

typedef struct CrdData
{
    short motionType;                             // 运动类型,0:直线插补,1:圆弧插补
    short circlePlat;                             // 圆弧插补的平面
    long pos[INTERPOLATION_AXIS_MAX];             // 当前段各轴终点位置
    double radius;                                  // 圆弧插补的半径
    short circleDir;                              // 圆弧旋转方向,0:顺时针;1:逆时针
    double center[2];                               // 圆弧插补的圆心坐标
    double vel;                                   // 当前段合成目标速度
    double acc;                                   // 当前段合成加速度
    short velEndZero;                             // 标志当前段的终点速度是否强制为0,0:不强制为0;1:强制为0
    TCrdBufOperation operation;                   // 缓存区延时和IO结构体

    double cos[INTERPOLATION_AXIS_MAX];           // 当前段各轴对应的余弦值
    double velEnd;                                // 当前段合成终点速度
    double velEndAdjust;                          // 调整终点速度时用到的变量(前瞻模块)
    double r;                                     // 当前段合成位移量
}TCrdData;

GT_API GT_SetCardNo(short index);
GT_API GT_GetCardNo(short *pIndex);

GT_API GT_GetVersion(short cardNum,char **pVersion);

GT_API GT_Open(short cardNum,short channel=0,short param=1);
GT_API GT_Close(short cardNum);

GT_API GT_LoadConfig(short cardNum,char *pFile);

GT_API GT_AlarmOff(short cardNum,short axis);
GT_API GT_AlarmOn(short cardNum,short axis);
GT_API GT_LmtsOn(short cardNum,short axis,short limitType=-1);
GT_API GT_LmtsOff(short cardNum,short axis,short limitType=-1);
GT_API GT_ProfileScale(short cardNum,short axis,short alpha,short beta);
GT_API GT_EncScale(short cardNum,short axis,short alpha,short beta);
GT_API GT_StepDir(short cardNum,short step);
GT_API GT_StepPulse(short cardNum,short step);
GT_API GT_SetMtrBias(short cardNum,short dac,short bias);
GT_API GT_GetMtrBias(short cardNum,short dac,short *pBias);
GT_API GT_SetMtrLmt(short cardNum,short dac,short limit);
GT_API GT_GetMtrLmt(short cardNum,short dac,short *pLimit);
GT_API GT_EncSns(short cardNum,unsigned short sense);
GT_API GT_EncOn(short cardNum,short encoder);
GT_API GT_EncOff(short cardNum,short encoder);
GT_API GT_SetPosErr(short cardNum,short control,long error);
GT_API GT_GetPosErr(short cardNum,short control,long *pError);
GT_API GT_SetStopDec(short cardNum,short profile,double decSmoothStop,double decAbruptStop);
GT_API GT_GetStopDec(short cardNum,short profile,double *pDecSmoothStop,double *pDecAbruptStop);
GT_API GT_LmtSns(short cardNum,unsigned short sense);
GT_API GT_CtrlMode(short cardNum,short axis,short mode);

GT_API GT_SetDo(short cardNum,short doType,long value);
GT_API GT_SetDoBit(short cardNum,short doType,short doIndex,short value);
GT_API GT_GetDo(short cardNum,short doType,long *pValue);

GT_API GT_GetDi(short cardNum,short diType,long *pValue);
GT_API GT_GetDiReverseCount(short cardNum,short diType,short diIndex,unsigned long *pReverseCount,short count=1);
GT_API GT_SetDiReverseCount(short cardNum,short diType,short diIndex,unsigned long *pReverseCount,short count=1);
GT_API GT_GetDiRaw(short cardNum,short diType,long *pValue);

GT_API GT_SetDac(short cardNum,short dac,short *pValue,short count=1);
GT_API GT_GetDac(short cardNum,short dac,short *pValue,short count=1,unsigned long *pClock=NULL);

GT_API GT_SetEncPos(short cardNum,short encoder,long encPos);
GT_API GT_GetEncPos(short cardNum,short encoder,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetEncVel(short cardNum,short encoder,double *pValue,short count=1,unsigned long *pClock=NULL);

GT_API GT_SetCaptureMode(short cardNum,short encoder,short mode);
GT_API GT_GetCaptureMode(short cardNum,short encoder,short *pMode,short count=1);
GT_API GT_GetCaptureStatus(short cardNum,short encoder,short *pStatus,long *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_SetCaptureSense(short cardNum,short encoder,short mode,short sense);

GT_API GT_Reset(short cardNum);
GT_API GT_GetClock(short cardNum,unsigned long *pClock,unsigned long *pLoop=NULL);
GT_API GT_GetClockHighPrecision(short cardNum,unsigned long *pClock);

GT_API GT_GetSts(short cardNum,short axis,long *pSts,short count=1,unsigned long *pClock=NULL);
GT_API GT_ClrSts(short cardNum,short axis,short count=1);
GT_API GT_AxisOn(short cardNum,short axis);
GT_API GT_AxisOff(short cardNum,short axis);
GT_API GT_Stop(short cardNum,long mask,long option);
GT_API GT_SetPrfPos(short cardNum,short profile,long prfPos);
GT_API GT_SynchAxisPos(short cardNum,long mask);
GT_API GT_ZeroPos(short cardNum,short axis,short count=1);

GT_API GT_SetSoftLimit(short cardNum,short axis,long positive,long negative);
GT_API GT_GetSoftLimit(short cardNum,short axis,long *pPositive,long *pNegative);
GT_API GT_SetAxisBand(short cardNum,short axis,long band,long time);
GT_API GT_GetAxisBand(short cardNum,short axis,long *pBand,long *pTime);
GT_API GT_SetBacklash(short cardNum,short axis,long compValue,double compChangeValue,long compDir);
GT_API GT_GetBacklash(short cardNum,short axis,long *pCompValue,double *pCompChangeValue,long *pCompDir);

GT_API GT_GetPrfPos(short cardNum,short profile,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetPrfVel(short cardNum,short profile,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetPrfAcc(short cardNum,short profile,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetPrfMode(short cardNum,short profile,long *pValue,short count=1,unsigned long *pClock=NULL);

GT_API GT_GetAxisPrfPos(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetAxisPrfVel(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetAxisPrfAcc(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetAxisEncPos(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetAxisEncVel(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetAxisEncAcc(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);
GT_API GT_GetAxisError(short cardNum,short axis,double *pValue,short count=1,unsigned long *pClock=NULL);

GT_API GT_SetControlFilter(short cardNum,short control,short index);
GT_API GT_GetControlFilter(short cardNum,short control,short *pIndex);

GT_API GT_SetPid(short cardNum,short control,short index,TPid *pPid);
GT_API GT_GetPid(short cardNum,short control,short index,TPid *pPid);

GT_API GT_Update(short cardNum,long mask);
GT_API GT_SetPos(short cardNum,short profile,long pos);
GT_API GT_GetPos(short cardNum,short profile,long *pPos);
GT_API GT_SetVel(short cardNum,short profile,double vel);
GT_API GT_GetVel(short cardNum,short profile,double *pVel);

GT_API GT_PrfTrap(short cardNum,short profile);
GT_API GT_SetTrapPrm(short cardNum,short profile,TTrapPrm *pPrm);
GT_API GT_GetTrapPrm(short cardNum,short profile,TTrapPrm *pPrm);

GT_API GT_PrfJog(short cardNum,short profile);
GT_API GT_SetJogPrm(short cardNum,short profile,TJogPrm *pPrm);
GT_API GT_GetJogPrm(short cardNum,short profile,TJogPrm *pPrm);

GT_API GT_PrfPt(short cardNum,short profile,short mode=PT_MODE_STATIC);
GT_API GT_SetPtLoop(short cardNum,short profile,long loop);
GT_API GT_GetPtLoop(short cardNum,short profile,long *pLoop);
GT_API GT_PtSpace(short cardNum,short profile,short *pSpace,short fifo=0);
GT_API GT_PtData(short cardNum,short profile,double pos,long time,short type=PT_SEGMENT_NORMAL,short fifo=0);
GT_API GT_PtClear(short cardNum,short profile,short fifo=0);
GT_API GT_PtStart(short cardNum,long mask,long option=0);
GT_API GT_SetPtMemory(short cardNum,short profile,short memory);
GT_API GT_GetPtMemory(short cardNum,short profile,short *pMemory);

GT_API GT_PrfGear(short cardNum,short profile,short dir=0);
GT_API GT_SetGearMaster(short cardNum,short profile,short masterIndex,short masterType=GEAR_MASTER_PROFILE,short masterItem=0);
GT_API GT_GetGearMaster(short cardNum,short profile,short *pMasterIndex,short *pMasterType=NULL,short *pMasterItem=NULL);
GT_API GT_SetGearRatio(short cardNum,short profile,long masterEven,long slaveEven,long masterSlope=0);
GT_API GT_GetGearRatio(short cardNum,short profile,long *pMasterEven,long *pSlaveEven,long *pMasterSlope=NULL);
GT_API GT_GearStart(short cardNum,long mask);

GT_API GT_PrfFollow(short cardNum,short profile,short dir=0);
GT_API GT_SetFollowMaster(short cardNum,short profile,short masterIndex,short masterType=FOLLOW_MASTER_PROFILE,short masterItem=0);
GT_API GT_GetFollowMaster(short cardNum,short profile,short *pMasterIndex,short *pMasterType=NULL,short *pMasterItem=NULL);
GT_API GT_SetFollowLoop(short cardNum,short profile,long loop);
GT_API GT_GetFollowLoop(short cardNum,short profile,long *pLoop);
GT_API GT_SetFollowEvent(short cardNum,short profile,short event,short masterDir,long pos=0);
GT_API GT_GetFollowEvent(short cardNum,short profile,short *pEvent,short *pMasterDir,long *pPos=NULL);
GT_API GT_FollowSpace(short cardNum,short profile,short *pSpace,short fifo=0);
GT_API GT_FollowData(short cardNum,short profile,long masterSegment,double slaveSegment,short type=FOLLOW_SEGMENT_NORMAL,short fifo=0);
GT_API GT_FollowClear(short cardNum,short profile,short fifo=0);
GT_API GT_FollowStart(short cardNum,long mask,long option=0);
GT_API GT_FollowSwitch(short cardNum,long mask);
GT_API GT_SetFollowMemory(short cardNum,short profile,short memory);
GT_API GT_GetFollowMemory(short cardNum,short profile,short *pMemory);

GT_API GT_Compile(short cardNum,char *pFileName,TCompileInfo *pWrongInfo);
GT_API GT_Download(short cardNum,char *pFileName);

GT_API GT_GetFunId(short cardNum,char *pFunName,short *pFunId);
GT_API GT_Bind(short cardNum,short thread,short funId,short page);

GT_API GT_RunThread(short cardNum,short thread);
GT_API GT_StopThread(short cardNum,short thread);
GT_API GT_PauseThread(short cardNum,short thread);

GT_API GT_GetThreadSts(short cardNum,short thread,TThreadSts *pThreadSts);

GT_API GT_GetVarId(short cardNum,char *pFunName,char *pVarName,TVarInfo *pVarInfo);
GT_API GT_SetVarValue(short cardNum,short page,TVarInfo *pVarInfo,double *pValue,short count=1);
GT_API GT_GetVarValue(short cardNum,short page,TVarInfo *pVarInfo,double *pValue,short count=1);

GT_API GT_SetCrdPrm(short cardNum,short crd,TCrdPrm *pCrdPrm);
GT_API GT_GetCrdPrm(short cardNum,short crd,TCrdPrm *pCrdPrm);
GT_API GT_CrdSpace(short cardNum,short crd,long *pSpace,short fifo=0);
GT_API GT_CrdData(short cardNum,short crd,TCrdData *pCrdData,short fifo=0);
GT_API GT_CrdDataCircle(short cardNum,short crd,TCrdData *pCrdData,short fifo=0);
GT_API GT_LnXY(short cardNum,short crd,long x,long y,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_LnXYZ(short cardNum,short crd,long x,long y,long z,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_LnXYZA(short cardNum,short crd,long x,long y,long z,long a,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_LnXYG0(short cardNum,short crd,long x,long y,double synVel,double synAcc,short fifo=0);
GT_API GT_LnXYZG0(short cardNum,short crd,long x,long y,long z,double synVel,double synAcc,short fifo=0);
GT_API GT_LnXYZAG0(short cardNum,short crd,long x,long y,long z,long a,double synVel,double synAcc,short fifo=0);
GT_API GT_ArcXYR(short cardNum,short crd,long x,long y,double radius,short circleDir,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_ArcXYC(short cardNum,short crd,long x,long y,double xCenter,double yCenter,short circleDir,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_ArcYZR(short cardNum,short crd,long y,long z,double radius,short circleDir,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_ArcYZC(short cardNum,short crd,long y,long z,double yCenter,double zCenter,short circleDir,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_ArcZXR(short cardNum,short crd,long z,long x,double radius,short circleDir,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_ArcZXC(short cardNum,short crd,long z,long x,double zCenter,double xCenter,short circleDir,double synVel,double synAcc,double velEnd=0,short fifo=0);
GT_API GT_BufIO(short cardNum,short crd,unsigned short doType,unsigned short doMask,unsigned short doValue,short fifo=0);
GT_API GT_BufDelay(short cardNum,short crd,unsigned short delayTime,short fifo=0);
GT_API GT_BufDA(short cardNum,short crd,short chn,short daValue,short fifo=0);
GT_API GT_BufLmtsOn(short cardNum,short crd,short axis,short limitType,short fifo=0);
GT_API GT_BufLmtsOff(short cardNum,short crd,short axis,short limitType,short fifo=0);
GT_API GT_BufSetStopIo(short cardNum,short crd,short axis,short stopType,short inputType,short inputIndex,short fifo=0);
GT_API GT_BufMove(short cardNum,short crd,short moveAxis,long pos,double vel,double acc,short modal,short fifo=0);
GT_API GT_BufGear(short cardNum,short crd,short gearAxis,long pos,short fifo=0);
GT_API GT_BufStopMotion(short cardNum,short crd,short fifo=0);
GT_API GT_CrdStart(short cardNum,short mask,short option);
GT_API GT_SetOverride(short cardNum,short crd,double synVelRatio);
GT_API GT_InitLookAhead(short cardNum,short crd,short fifo,double T,double accMax,short n,TCrdData *pLookAheadBuf);
GT_API GT_CrdClear(short cardNum,short crd,short fifo);
GT_API GT_CrdStatus(short cardNum,short crd,short *pRun,long *pSegment,short fifo=0);
GT_API GT_SetUserSegNum(short cardNum,short crd,long segNum,short fifo=0);
GT_API GT_GetUserSegNum(short cardNum,short crd,long *pSegment,short fifo=0);
GT_API GT_GetRemainderSegNum(short cardNum,short crd,long *pSegment,short fifo=0);
GT_API GT_SetCrdStopDec(short cardNum,short crd,double decSmoothStop,double decAbruptStop);
GT_API GT_GetCrdStopDec(short cardNum,short crd,double *pDecSmoothStop,double *pDecAbruptStop);
GT_API GT_GetCrdPos(short cardNum,short crd,double *pPos);
GT_API GT_GetCrdVel(short cardNum,short crd,double *pSynVel);


GT_API GT_PrfPvt(short cardNum,short profile);
GT_API GT_SetPvtLoop(short cardNum,short profile,long loop);
GT_API GT_GetPvtLoop(short cardNum,short profile,long *pLoopCount,long *pLoop);
GT_API GT_PvtStatus(short cardNum,short profile,short *pTableId,double *pTime,short count=1);
GT_API GT_PvtStart(short cardNum,long mask);
GT_API GT_PvtTableSelect(short cardNum,short profile,short tableId);

GT_API GT_PvtTable(short cardNum,short tableId,long count,double *pTime,double *pPos,double *pVel);
GT_API GT_PvtTableEx(short cardNum,short tableId,long count,double *pTime,double *pPos,double *pVelBegin,double *pVelEnd);
GT_API GT_PvtTableComplete(short cardNum,short tableId,long count,double *pTime,double *pPos,double *pA,double *pB,double *pC,double velBegin=0,double velEnd=0);
GT_API GT_PvtTablePercent(short cardNum,short tableId,long count,double *pTime,double *pPos,double *pPercent,double velBegin=0);
GT_API GT_PvtPercentCalculate(short cardNum,long n,double *pTime,double *pPos,double *pPercent,double velBegin,double *pVel);
GT_API GT_PvtTableContinuous(short cardNum,short tableId,long count,double *pPos,double *pVel,double *pPercent,double *pVelMax,double *pAcc,double *pDec,double timeBegin);
GT_API GT_PvtContinuousCalculate(short cardNum,long n,double *pPos,double *pVel,double *pPercent,double *pVelMax,double *pAcc,double *pDec,double *pTime);
