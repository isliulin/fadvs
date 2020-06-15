#ifndef TK_PARAM_DEFINE_H_
#define TK_PARAM_DEFINE_H_
#include "TimeUtil.h"
#pragma   warning(disable:4996)

enum MatchMode
{
	GRAYSCALE = 0,
	GEOMETRIC
};
enum ContactSensorType
{
	ContactSensorClose = 0,
	ContactSensorOpen,
	ContactSensorHiPrecision
};
enum HeightSensorType
{
	HEIGHT_SENSOR_MLD,
	HEIGHT_SENSOR_PANASONIC,
	HEIGHT_SENSOR_KEYENCE,
	HEIGHT_SENSOR_DISABLE,
};
enum ValveNum
{
	ValveNum_0 = 0,
	ValveNum_1,
	ValveNum_2,
	ValveNum_3
};
enum PLC_TYPE
{
	PLC_KEYENCE = 0,
	PLC_PANASONIC
};
enum VALVE_TYPE
{
	VALVE_M9 = 0,
	VALVE_L9,
	VALVE_H9
};
enum DeviceType
{
	MLD_HS,
	PANASONIC_HL,
	KEYENCE_IL,
};

enum DetectCameraType
{
	DETECT_CAMERA_PG,
	DETECT_CAMERA_MV,
};

enum DispFillMode  //////////////////////
{
	DOT_DISP_MODE,
	LINE_VERTICAL_DISP_MODE,
	LINE_HORIZONTAL_DISP_MODE,
	RECTANGLE_DISP_MODE,
	HELIX_DISP_MODE,
	CUSTOM_DISP_MODE,
	CIRCLE_DISP_MODE,
	MULTI_DISP_MODE,
	COMPLEX_DISP_MODE
};

enum VisionMode
{
	VISION_DISABLE,
	VISION_ONE_MARK,
	VISION_TWO_MARK
};

enum InspectMode
{
	NO_INSPECT,
	TRANSITION_INSPECT,
	SCAN_INSPECT,
	SESSION_INDPECT
};

enum Orientation
{
	ROW_DISP,
	COLUMN_DISP
};

enum Sequence
{
	CLEAN_LATER_TEST,
	TEST_LATER_CLEAN
};

enum VelType
{
	LOW_VEL,
	MEDIUM_VEL,
	HIGH_VEL,
	LIFT_VEL
};

enum ValveSelect
{
	ONE_VALVE,
	TWO_VALVE,
	THREE_VALVE
};

typedef enum
{
	MOT_STATUS_OK = 0,
	MOT_STATUS_COMM_ERROR,
	MOT_STATUS_EXCEED_SW_LIMIT,
	MOT_STATUS_TIMEOUT,
	MOT_STATUS_UNKNOWN_ERROR,
	MOT_STATUS_MOTION_FAILURE
}MOTION_STATUS;

enum HEIGHT_STATUS
{
	HEIGHT_STATUS_OK = 0,
	HEIGHT_STATUS_MOTION_FAILURE,
	HEIGHT_STATUS_EXCEED_LIMIT,
	HEIGHT_STATUS_EXCEED_HEIGHTCOMP_LIMIT,//补偿值超限
	HEIGHT_STATUS_UNKNOWN_ERROR
};

enum NeedleSearchMode
{
	GREY_MODE,
	BLOB_MODE
};

struct AutoNeedleStep
{
	double dPitchX;
	double dPitchY;
	int nCountX;
	int nCountY;
	int nCurrentX;
	int nCurrentY;

	AutoNeedleStep()
	{
		dPitchX = 0.0;
		dPitchY = 0.0;
		nCountX = 0;
		nCountY = 0;
		nCurrentX = 0;
		nCurrentY = 0;
	}
};

struct TrackInfo
{
	int m_nFirstRow;
	int m_nFirstCol;
	int m_nSecondRow;
	int m_nSecondCol;
	int m_nThirdRow;
	int m_nThirdCol;
	int m_nIrregularRow;
	int m_nIrregularCol;
	TrackInfo()
	{
		TrackReset();
	}
	void TrackReset()
	{
		m_nFirstRow = 0;
		m_nFirstCol = 0;
		m_nSecondRow = 0;
		m_nSecondCol = 0;
		m_nThirdRow = 0;
		m_nThirdCol = 0;
		m_nIrregularRow = 0;
		m_nIrregularCol = 0;
	}
	bool IsTrackReset()
	{
		bool value = false;
		if((m_nFirstRow==0)&&(m_nFirstCol==0)&&(m_nSecondRow==0)&&(m_nSecondCol==0)&&
			(m_nThirdRow==0)&&(m_nThirdCol==0)&&(m_nIrregularRow==0)&&(m_nIrregularCol==0))
		{
			value = true;
		}
		return value;
	}
};
struct TrackInfoMulti
{
	int m_nMultiRow;
	int m_nMultiColumn;
	TrackInfoMulti()
	{
		TrackReset();
	}
	void TrackReset()
	{
		m_nMultiRow = 0;
		m_nMultiColumn = 0;
	}
	bool IsTrackReset()
	{
		bool value = false;
		if((m_nMultiRow==0)&&(m_nMultiColumn==0))
		{
			value = true;
		}
		return value;
	}
};

struct tgAxis
{
	tgAxis()
	{
		scale = 0.001;
		maxpos = 100;
		vel[0]=vel[1]=vel[2]=vel[3]=20;
		acc[0]=acc[1]=acc[2]=acc[3]=300;
		jerk[0]=jerk[1]=jerk[2]=jerk[3]=30000000;
		dec[0]=dec[1]=dec[2]=dec[3]=300;
		velStart[0]=velStart[1]=velStart[2]=velStart[3]=0;
		highHomeVel=50;
		lowHomeVel=20;
		homeAcc=300;
	}
	double scale;
	double minpos, maxpos;
	double acc[4];
	double vel[4];
	double jerk[4];
	double dec[4];
	double velStart[4];
	double lowHomeVel;
	double highHomeVel;
	double homeAcc;
	double maxVel;
};

struct tgPos
{
	tgPos(){x = y= za = zb = zc = 0;}
	tgPos(double dX, double dY, double dZa, double dZb, double dZc){x =dX; y= dY;za =dZa; zb = dZb; zc = dZc;}
	double x, y, za, zb, zc;
};

struct tgPixel
{
	int x;
	int y;
	tgPixel() {x=y=0;}
	tgPixel(int dx,int dy) {x=dx;y=dy;}
};

struct tgPadDetectResult
{
	double x;
	double y;
	int nRow;
	int nColumn;
};

struct tagDPostion//点胶位置
{
	double Dx;
	double Dy;
	double Dza;
	double Dzb;
	double Dzc;
	int nFirstRow;
	int nFirstColumn;
	int nSecondRow;
	int nSecondColumn;
	int nThirdRow;
	int nThirdColumn;
	bool bIsCompleted;
	bool bIsHasPad;
};

struct tagStautDis//显示状态
{
	CTimeSpan W_TotalTime;
	CTimeSpan W_StandbyTime;
	CTimeSpan W_RunTime;
	//long W_Count;
	double W_SingeTimer;
	double  W_UPH;
};

struct tgCameraCalibration
{
	double dCalibrationA;
	double dCalibrationB;
	double dCalibrationC;
	double dCalibrationD;

	tgCameraCalibration()
	{
		dCalibrationA = 1.0;
		dCalibrationB = 0;
		dCalibrationC = 0;
		dCalibrationD = 1.0;
	}
	void SetCalibrationParam(double dValueA,double dValueB,double dValueC,double dValueD)
	{
		dCalibrationA = dValueA;
		dCalibrationB = dValueB;
		dCalibrationC = dValueC;
		dCalibrationD = dValueD;
	}
	void ConvertPixelToTable(double dPixelX,double dPixelY,double &dTableX,double &dTableY)
	{
		dTableX = dPixelX*dCalibrationA+dPixelY*dCalibrationC;
		dTableY = dPixelX*dCalibrationB+dPixelY*dCalibrationD;
	}

	void ConvertPixelToMM(double dPixelX,double dPixelY,double &dTableX,double &dTableY)
	{
		dTableX=dPixelX/dCalibrationA;
        dTableY=dPixelY/dCalibrationB;
	}

	void StepConvertPixelToMM(double dPixelX,double dPixelY,double &dTableX,double &dTableY)
	{
		dTableX=dPixelX/dCalibrationA;
		dTableY=dPixelY/dCalibrationB;
	}

	void ConvertTableToPixel(double dTableX,double dTableY,double &dPixelX,double &dPixelY)
	{
		double dValue = dCalibrationA*dCalibrationD - dCalibrationB*dCalibrationC;
		if(dValue!=0)
		{
			dPixelX = (dTableX*dCalibrationD - dTableY*dCalibrationC)/dValue;
			dPixelY = (dTableY*dCalibrationA - dTableX*dCalibrationB)/dValue;
		}
		else
		{
			dPixelX = dTableX;
			dPixelY = dTableY;
		}
	}

	void ConvertMMToPixsel(double dTableX,double dTableY,double &dPixelX,double &dPixelY)
	{
		dPixelX=dCalibrationA*dTableX;
		dPixelY=dCalibrationB*dTableY;	
	}
		
};

struct tgSysParam
{
	tgSysParam()
	{
		int i=0;
		//strcpy(szApp, "TSCtrlSys LED荧光粉高速点胶系统");
		strcpy(szApp, "TSCtrlSys LED High-Speed Phosphor Dispensing System");
		for(i=0; i<3; i++)
		{
			dVelEditXY[i] = 30*i + 10;
			dVelEditZ[i] =  30*i + 10;
		}
		for(i=0;i<4;i++)
		{
			nBaud[i] = 9600;
			nBytes[i] = 8;
			nParity[i] = 0;
		}
		for(i = 0;i<5;i++)
		{
			CleanPosition[i] = 5.0;
			TestDotPostion[i] = 5.0;
			DischargeGluePostion[i] = 5.0;
		}
		strcpy(szOutputPortDisp,"AF6,BF7,G4,Y3,R2,T5,S9,");
		strcpy(szInputPortButton, "AF6,BF7,R3,P6,S5,E2,H10,");
		iXDir = 0;
		iYDir = 0;
		iZDir = 0;
		sensorType = ContactSensorClose;
		valveNum = ValveNum_0;
		PlcType = PLC_KEYENCE;
		ValveType = VALVE_M9;
		heightSensorType = HEIGHT_SENSOR_DISABLE;
		detectCameraType = DETECT_CAMERA_MV;
		dPixelScaleX = 0.005839;
		dPixelScaleY = 0.005839;
		nSubstrateType = 0;
		CameraGain = 25;
		CameraExposureTime = 15;

		NeedleCameraGain=25;
		NeedleCameraExposure=15;

		CameraDetectExposureTime = 5;
		CameraDetectGain = 10;

		BmpPadDetectAcceptance[0] =BmpPadDetectAcceptance[1]= 50.0;
		BmpPadDetectAngle[0]=BmpPadDetectAngle[1] = 10;

		BmpPadDetectLearnWin[0].top = BmpPadDetectLearnWin[0].left = 0;
		BmpPadDetectLearnWin[1].top = BmpPadDetectLearnWin[1].left =0;
		BmpPadDetectLearnWin[2].top = BmpPadDetectLearnWin[2].left =0;
		BmpPadDetectLearnWin[3].top = BmpPadDetectLearnWin[3].left =0;
		BmpPadDetectLearnWin[4].top = BmpPadDetectLearnWin[4].left =0;

		BmpPadDetectLearnWin[0].bottom = BmpPadDetectLearnWin[0].right = 100;		
		BmpPadDetectLearnWin[1].bottom = BmpPadDetectLearnWin[1].right = 100;
		BmpPadDetectLearnWin[2].bottom = BmpPadDetectLearnWin[2].right = 100;
		BmpPadDetectLearnWin[3].bottom = BmpPadDetectLearnWin[3].right = 100;
		BmpPadDetectLearnWin[4].bottom = BmpPadDetectLearnWin[4].right = 100;

		BmpPadDetectROI[0].top = BmpPadDetectROI[0].left = BmpPadDetectROI[0].bottom = BmpPadDetectROI[0].right = 0;

		BmpPadDetectROI[1].top = BmpPadDetectROI[1].left = BmpPadDetectROI[1].bottom = BmpPadDetectROI[1].right = 0;

		BmpMarkROI.left = BmpMarkROI.top = 0;
		BmpMarkROI.right = 640;
		BmpMarkROI.bottom = 512;

		BmpMarkRefCamROI.left=BmpMarkRefCamROI.top=0;
		BmpMarkRefCamROI.right=640;
		BmpMarkRefCamROI.bottom=512;

		BmpNeedleROI.left=BmpNeedleROI.top=0;
		BmpNeedleROI.right=640;
		BmpNeedleROI.bottom=512;

		for(int i=0;i<2;i++)
		{
			BmpMarkLearnWin[i].left = 320-100;
			BmpMarkLearnWin[i].top = 256-50;
			BmpMarkLearnWin[i].right = 320+100;
			BmpMarkLearnWin[i].bottom = 256+50;
		}

		BmpMarkRefCam.left=320-100;
		BmpMarkRefCam.top=256-50;
		BmpMarkRefCam.right=320+100;
		BmpMarkRefCam.bottom=256+50;

		BmpNeedleLearnWin.left=320-100;
		BmpNeedleLearnWin.top=256-50;
		BmpNeedleLearnWin.right=320+100;
		BmpNeedleLearnWin.bottom=256+50;

		EmbolisD = 5.0;
		DisEndDistance = 48.0;
		RestDistance = 48.0;
		DispFreeDistance = 8.0;
		DispMaxSpeed = 24.0;
		DispMaxAcc = 300;

		MotorMaxSpeed = 400;
		MotorMaxAcc = 15000;

		StatCapacity = 0;

		dAutoNeedleAcceptance = 50;
		dAutoNeedleAngle = 5;

		dFocusZPos=0;
		BmpPadDetectFirstPixselX=0;//首点像素坐标X
		BmpPadDetectFirstPixselY=0;//首点像素坐标Y

		BmpNeedleMarkAcceptance=70;
		BmpNeedleMarkAngle=6;
		iIsLog=0; 
		iTimeLogOut=10;
		MotorAInitPos=1;
		MotorCInitPos=3;
		nCamMarkNo=0;
		nCamNeedleNo=1;
		strcpy(szLastCleanTime, "2017-8-29  19:06:23");
		strcpy(RegCode, "Init");
	}
	// 系统标题
	char szApp[255];
	// 产品型号
	char szProductID[10];
	// 运动轴
	tgAxis tAxis[12];
	// 编辑轨迹速度
	double dVelEditXY[3], dVelEditZ[3];
	// 输出端口定义：点胶阀F,点胶阀F,..绿灯G,黄灯Y,红灯R,报警A
	char szOutputPortDisp[511];
	// 输入按钮端口定义:运行R，暂停P，停止S，紧急E,点胶阀F
	char szInputPortButton[511];
	int iXDir;
	int iYDir;
	int iZDir;
	int iIsLog; //是否写日志
	int iTimeLogOut;//自动注销时间
	// 接触测高传感器类型
	ContactSensorType sensorType;
	// 阀体数量
	ValveNum valveNum;
	//PLC类型
	PLC_TYPE PlcType;
	//阀体类型
	VALVE_TYPE ValveType;
	// 高度传感器类型
	HeightSensorType heightSensorType;
	// 检测相机类型
	DetectCameraType detectCameraType;
	//端口号——急停
	int inEmergecy;
	//端口号——暂停or启动---启动按钮
	int inPauseOrRun;
	//端口号——停止--停按钮止
	int inStop;
	//端口号——复位
	int inReset;
	//端口号——接触测高
	int inHeight;
	//端口号——PLC运行
	int inPlcRun;
	//端口号——PLC暂停
	int inPlcPause;
	//端口号——PLC准备好
	int inPlcReady;
	//端口号——PLC异常
	int inPlcAlarm;
	//端口号——轨道电机
	int inRailRun;
	//端口号——点胶阀气缸限位
	int inValveOpenSen[3];
	//端口号——点胶阀气缸原位
	int inValveCloseSen[3];
	//端口号——检测信号
	int inTransitionDetect;
	//端口号——门禁开关
	int inDoorSwitch;

	//输出信号——信号灯
	int outGreen, outYellow, outRed;
	//输出信号——真空清洗
	int outNeedleClean;
	//输出信号——打开阀体
	int outValveOpen[3];
	//输出信号——胶体加压
	int outOpenPress[3];

	//输出信号——检测光源 ：缺颗粒
	int outDetectLight;
	//输出信号——对针光源 ：对针环光
	int outNeedleLight;
	//输出信号——Mark点光源：Mark 点光
	int outMarkLight;
	//输出信号——日光灯
	int outDayLight;
	
	/* TO_PLC */
	//输出信号——准备好
	int outReady;
	//输出信号——阀体启动
	int outValve[2];
	//输出信号——点胶完成信号
	int outDispenseFinish;
	//输出信号——检测完成
	int outDetectFinish;
	//输出信号——旋转气缸
	int outRotatingLight;

	double dLook[5];
	int iLookNum;//200
	// 串口通信参数——波特率
	int nBaud[4];
	// 串口通信参数——数据长度
	int nBytes[4];
	// 串口通信参数——奇偶校验 0:无校验；1:奇校验（Odd）；2:偶校验（Even）
	int nParity[4];
	// 图像参数——BLOB——中心坐标X
	double BmpCenterX[2];
	// 图像参数——BLOB——中心坐标Y
	double BmpCenterY[2];
	// 图像参数——BLOB——标定的标准长度
	int StandardLen;
	// 图像参数——BLOB——距离和像素比值
	double BmpScale;
	// 图像参数——BLOB——查找区域
	CRect BmpROI[2];
	// 图像参数——BLOB——面积最小值
	double BmpAreaMin;
	// 图像参数——BLOB——面积最大值
	double BmpAreaMax;
	// 图像参数——BLOB——高度最大值
	double BmpHeighMax;
	// 图像参数——BLOB——高度最小值
	double BmpHeighMin;
	// 图像参数——BLOB——宽度最大值
	double BmpWidthMax;
	// 图像参数——BLOB——宽度最小值
	double BmpWidthMin;
	// 图像参数——MARK——距离和像素比值
	//double BmpMarkScale;
	// 图像参数——MARK——模板窗口
	CRect BmpMarkLearnWin[2];
	//图像参数----MARK---看参考相机模板
	CRect BmpMarkRefCam;
	
	// 图像参数——MARK——匹配分数//PadDetect
	double BmpMarkAcceptance;
	// 图像参数——MARK——匹配角度//PadDetect
	double BmpMarkAngle;

	//图像参数--NEEDLEMARK--针头匹配度
     double BmpNeedleMarkAcceptance;
	//图像参数--NEEDLEMARK--搜索角度
     double BmpNeedleMarkAngle;
	// 图像参数——MARK——查找窗口
	CRect BmpMarkROI;
	//图像参数--MARKREF--搜索窗口
	CRect BmpMarkRefCamROI;

	// 相机校正参数
	tgCameraCalibration BmpMarkCalibration;
	//相机校正参数：对准参考点相机时用
	tgCameraCalibration BmpMarkRefCamCalibration;

	// 图像参数——SlotDetect——模板窗口
	CRect BmpSlotLearnWin;
	// 图像参数——SlotDetect——匹配分数
	double BmpSlotAcceptance;
	// 图像参数——SlotDetect——匹配角度
	double BmpSlotAngle;
	// 图像参数——SlotDetect——查找窗口
	CRect BmpSlotROI;
	// 相机校正参数——SlotDetect
	tgCameraCalibration BmpSlotCalibration;
	// 图像参数——PATTERN——距离和像素比值
	double BmpPatternScale;
	// 图像参数——PATTERN——模板窗口
	CRect BmpPatternLearnWin;
	// 图像参数——PATTERN——查找窗口
	CRect BmpPatternROI;
	// 图像参数——PATTERN——匹配分数
	double BmpPatternAcceptance;
	// 图像参数——PATTERN——匹配角度
	double BmpPatternAngle;
	// 相机参数——增益
	long CameraGain;
	// 相机参数——曝光时间
	long CameraExposureTime;
    //对针头相机--增益
	long NeedleCameraGain;
	//对针头相机--曝光时间
	long NeedleCameraExposure;
	// 图像参数——Pad Detect——模板窗口//0 used find pad ;1 used find frame
	CRect BmpPadDetectLearnWin[5];
	//图像参数--Needle---模板窗口
	CRect BmpNeedleLearnWin;
	// 图像参数——Pad Detect——查找窗口//0 used find pad ;1 used find frame
	CRect BmpPadDetectROI[2];
	//图像参数--Needle--ROI
	CRect BmpNeedleROI;
	// 图像参数——Pad Detect——匹配分数
	double BmpPadDetectAcceptance[2];
	// 图像参数——Pad Detect——匹配角度
	double BmpPadDetectAngle[2];

	// 图像参数——Pad Detect——中心坐标X
	double BmpPadDetectCenterX;
	// 图像参数——Pad Detect——中心坐标Y
	double BmpPadDetectCenterY;
	//图像参数--Pad Detect--首点像素坐标X
	double BmpPadDetectFirstPixselX;
	//图像参数--Pad Detect--首点像素坐标Y
	double BmpPadDetectFirstPixselY;
	// 图像参数——Pad Detect——相机校正参数
	tgCameraCalibration BmpPadDetectCalibration;
	// 相机参数——检测相机——增益
	long CameraDetectGain;
	// 相机参数——检测相机——曝光时间
	long CameraDetectExposureTime;

	//图像参数--针头校正---相机校正
	tgCameraCalibration BmpNeedleCalibration;

	//图像参数--针头校正---相机校正//靠ABCD 调整的系数
	tgCameraCalibration BmpStepNeedleCalibration;


	// 针头自动调整参数
	tgCameraCalibration AutoNeedleCalib;
	CRect AutoNeedleROI;
	CRect AutoNeedleModelWin;
	double dAutoNeedleAcceptance;
	double dAutoNeedleAngle;
	tgPos tgNeedleOffset[3];

	int nSubstrateType;
	//针头清洗时间
	int  NeedleTime;
    //相机焦距Z位置；
	double dFocusZPos;
	//测试点胶位
	double TestDotPostion[5];
	//脱泡位
	double DischargeGluePostion[5];
	//清洗位
	double CleanPosition[5];

	// V9控制参数——控制器类型
	int ControlType;
	// V9控制参数——点胶系数
	double Dispratios;
	// V9控制参数——总行程
	double DisEndDistance;
	// V9控制参数——活栓直径
	double EmbolisD;
	// V9控制参数——点胶空距离
	//此值标识空闲不用部分：根据阀体类型不同使用方式不一样[H9:一段距离/M9：可以标识绝对位置]
	double DispFreeDistance;
	// V9控制参数——复位距离
	double   RestDistance;
	// V9控制参数——最大速度
	double  DispMaxSpeed;
	// V9控制参数——最大加速度
	double  DispMaxAcc;
	// 伺服电机参数——最大速度
	double MotorMaxSpeed;
	// 伺服电机参数——最大加速度
	double MotorMaxAcc;
	// V9回零是否成功
	bool V9HomeSu;
	// 三轴回零是否成功
	bool ThreeAxHomeSu;
	//调整马达回零是否成功
	bool AdjustMotorHomeSu;	
/////////////////////////////////////////////////////////////////////
	double dPixelScaleX,dPixelScaleY;
	// 产能
	long StatCapacity;
	double MotorAInitPos,MotorCInitPos;
	//上次清洗时间
	char szLastCleanTime[255];
	//mark 相机编号
	int nCamMarkNo;
	//needle 相机编号
	int nCamNeedleNo;
	char RegCode[32];
public:
	void GetPortNo()
	{
		inEmergecy = GetInputNo("EMC");
		inPauseOrRun = GetInputNo("PauseRun");
		inStop = GetInputNo("Stop");
		inTransitionDetect = GetInputNo("PadDetect");
		inReset = GetInputNo("Reset");
		inHeight = GetInputNo("Height");
		inPlcRun = GetInputNo("PlcRun");
		inPlcPause = GetInputNo("PlcPause");
		inPlcReady = GetInputNo("PlcReady");
		inPlcAlarm = GetInputNo("PlcAlarm");
		inRailRun = GetInputNo("RailRun");
		inDoorSwitch = GetInputNo("DoorSwitch");
		//三台M9
		inValveOpenSen[0] = GetInputNo("VOA");
		inValveCloseSen[0] = GetInputNo("VCA");
		inValveOpenSen[1] = GetInputNo("VOB");
		inValveCloseSen[1] = GetInputNo("VCB");
		inValveOpenSen[2] = GetInputNo("VOC");
		inValveCloseSen[2] = GetInputNo("VCC");

		outGreen = GetOutputNo("Green");
		outRed = GetOutputNo("Red");
		outYellow = GetOutputNo("Yellow");
		outNeedleClean = GetOutputNo("VacuumA");
		outValveOpen[0] = GetOutputNo("ValveA");
		outValveOpen[1] = GetOutputNo("ValveB");
		outValveOpen[2] = GetOutputNo("ValveC");
		outOpenPress[0] = GetOutputNo("PressA");
		outOpenPress[1] = GetOutputNo("PressB");
		outOpenPress[2] = GetOutputNo("PressC");
		outDetectLight = GetOutputNo("DetectLight");
		outNeedleLight = GetOutputNo("NeedleLight");
		outMarkLight = GetOutputNo("MarkLight");
		outDayLight = GetOutputNo("DayLight");
		outRotatingLight = GetOutputNo("RotatingLight");
		// PLC
		outValve[0] = GetOutputNo("RunVA");
		outValve[1] = GetOutputNo("RunVB");
		outReady = GetOutputNo("Ready");
		outDispenseFinish = GetOutputNo("DispFinish");
		outDetectFinish = GetOutputNo("DetectFinish");
	}
protected:
	int GetInputNo(CString strInput, CString szFlag)
	{
		CString strTemp = CString(strInput);
		int m = strTemp.Find(szFlag);
		if(m<0)
		{
			return -1;
		}
		strTemp.Find(",");
		return atoi(strTemp.Mid(m+szFlag.GetLength()));
	}

	int GetInputNo(CString szFlag)
	{
		return GetInputNo(szInputPortButton, szFlag);
	}

	int GetOutputNo(CString strOutput, CString szFlag)
	{
		CString strTemp = CString(strOutput);
		int m = strTemp.Find(szFlag);
		if(m<0)
		{
			return -1;
		}
		return atoi(strTemp.Mid(m+szFlag.GetLength()));
	}

	int GetOutputNo(CString szFlag)
	{
		return GetOutputNo(szOutputPortDisp, szFlag);
	}
};

struct tgProductParam
{
	tgProductParam()
	{
		reset();
	}
	void reset()
	{
		dPreTime =0;
		dDispTime = 0.5;
		dPostDisTime = 0;
		dZupHigh = 0;
		
		FirstDispRow=1;
		FirstDispColumn=1;
		FirstDispRowD=55;
		FirstDispColumnD=22;
		SecondDispRow=1;
		SecondDispColumn=1;
		SecondDispRowD=55;
		SecondDispColumnD=22;
		ThirdDispRow=1;
		ThirdDispColumn=1;
		ThirdDispRowD=55;
		ThirdDispColumnD=22;
		IrregularRow=5;
		IrregularColumn=5;
		IrregularSlotLength = 150;
		IrregularSlotWidth = 60;
		MultiDispRow = 1;
		MultiDispColumn = 1;
		MultiDispRowD = 0.0;
		MultiDispColumnD = 0.0;
		for(int i=0;i<20;i++)
		{
			IrregularRowD[i] = 0.0;
		}
		for(int i=0;i<50;i++)
		{
			IrregularColumnD[i] = 0.0;
		}
		dDispAccXY= 5000;
		dDispVelXY= 100;
		dDispVelZ = 100;
		dDispAccZ = 5000;
		DispMode=0;
		CleanNd=0;
		FirstTest=0;
		SlotDetect=0;
		FullTestDot=0;
		FullClean=0;
		IrregularMatrix=0;
		DoorSwitch = 0;
		DispFinishFill = 0;
		CleanDischarge = 0;
		PauseCleanTest = 0;
		MeasureHeightEnable = 0;
		AutoAdjust = 0;
		HeatWhenStart=0;
		TempJudgeBeforeStart=0;
		FreeVal=100;
		FreeAcc=2500;
		CircleVel=10;
		CircleAcc=2500;
		LineVel = 10;
		LineAcc = 2000;
		LineLength = 1.0;
		RectLength = 1.0;
		RectWidth = 1.0;
		tSlotDetectPos.x = tSlotDetectPos.y = tSlotDetectPos.za = tSlotDetectPos.zb = tSlotDetectPos.zc = 0.0;
		nImageNum = 2;
		for(int i=0;i<3;i++)
		{
			FSelectVal[i]=false;

			HeightSensorPos[i] = 5.0;
			ImageDispensePosition[i] = 5.0;
			HeightDispensePosition[i] = 5.0;

			tgImagePos[i].x = tgImagePos[i].y = 5.0;
		}
		for(int i=0;i<5;i++)
		{
			DispenseStartPos[i] = 5.0;
			NeedlePos[i] = 5.0;
			ContactSensorPos[i] = 5.0;
		}
		for(int i=0;i<2;i++)
		{
			AdjustNeedleAngle[0] = AdjustNeedleAngle[1] = 70.0;
		}
        //Init position data;
		for (int i=0;i<6;i++)
		{
			tgLabelPos[i].x=0;
			tgLabelPos[i].y=0;
			tgLabelPos[i].za=0;
			tgLabelPos[i].zb=0;
			tgLabelPos[i].zc=0;
		}

		for (int i=0;i<2;i++)
		{
			CameraOnContactor[i]=0;
		}
		offset2x=offset2y=offset3x=offset3y=0;//record the lable position;
		dComposedSecondY=dComposedThirdY=0;//compose the small offset;
		dComposeSecondX=dComposedThirdX=0;//compose the small offset;
		dPauseHeight=0.0;
		dPauseTime=0.0;
		dFirstDispDelay=0.0;
		dDispRadius=0.0;
		dDistance=0.0;
		dGrabImageDelay = 0.0;
		dCompAngle = 0.0;
		dCleanDelay=0.0;
		dTestDotDelay = 0.5;
		TestDotNumber = 1;
		nLackPadCmpValue=1000;
		HeightNumber = 3;
		dHeightError = 0.5;
		dHeightDelay = 1.0;
		dDispenseHeight = 0.0;
		dTempError=5.0;
		dHeatSetting=75.0;
		HeightGlueVol = 0.5;
		HeightStandardValue = 0.0;
		dHeightCompMax=6;
		ContactStandardValue = 0.0;
		ContactMeasureVel = 20.0;
		ContactMeasureAcc = 2000.0;
		NeedleVel = 20.0;
		NeedleAcc = 2000.0;
		NeedleGap = 0.0;
		CleanGap = 0.0;

		DispOffsetX = 0.0;
		DispOffsetY = 0.0;
		MaxOffsetAngle = 5.0;
		MaxOffsetX = 1.0;
		MaxOffsetY = 1.0;
;
		HeatOne = 0;
		HeatTwo = 0;
		ScanDelay = 0.5;

		fillMode = DOT_DISP_MODE;
		visionMode = VISION_DISABLE;
		inspectMode = NO_INSPECT;
		dispOrientation = ROW_DISP;
		dispSequence = CLEAN_LATER_TEST;
		valveSelect = THREE_VALVE;
		matchMode = GRAYSCALE;

		searchMode = GREY_MODE;
		nAutoAdjustCount = 5;
		nBlobAreaMin = 0;
		nBlobAreaMax = 10000;
		nBlobLengthMin = 0;
		nBlobLengthMax = 100;
		nBlobHeightMin = 0;
		nBlobHeightMax = 100;
		nBlobThreshold = 150;

		dAutoNeedleErrorX = 0.5;
		dAutoNeedleErrorY = 0.5;
		dAutoNeedleGlueVol = 0.5;

		dNeedleLaserHeight=5;
		//recode the position after adjusted;
		dAdjustMotorInRunA=0;
		dAdjustMotorInRunB=0;
		dAdjustMotorInRunC=0;
		dAdjustMotorInRunD=0;
        //胶水预约相关参数
		ArrangeEnable=0;
		ArrangeSpan=0.0;
		ArrangeTime=0;
		NeedleHeightCheck = 0;
		ValveMonitorEnable=0;
		ValveMonitorSpan=0.0;
		ValveMonitorTime=0;
		lGasketRingLife=1000;//k
		dZDispOffsetAll=-2;
		bCleanRemind = 0;
		CleanRemindTimeHour = 0;
		bMesOffline = 1;
		ContactFrequency = 1;
		ContactErrPermit = 0.05;
	    MesDataMin=0;
        MesDataMax=2;
		for(int i=0;i<10;i++)
			dLaserDataBase[i]=0;
		//UDP相关
		UDPOffline = false;
		UDPRestConnet = false;
		UDPSendMsgFlag = false;
		ServerPort = 8888;
		FCompanyCode = 0;
		strcpy(ServerIP,"127.0.0.1");
		//strcpy(UDPSendMsg,"110000");

		UDPSendMsg="110000";
		strcpy(ServerURL,"www.xpwin.jfsb");
		strcpy(MachineID,"ZJ888");
		Save_rev_CodeID = "";

		dPreHeatTemp=20;
		dDispHeatTemp=30;
		dRealPreHeatTemp = 20;
		dRealDispHeatTemp = 30;

		nCheckIsCompensate = 1;

	}
	//单点点胶参数:前延时间，点胶时间，点胶结束暂停时间
	double dPreTime, dDispTime, dPostDisTime;
	// 点胶速度——XY速度
	double dDispVelXY;
	// 点胶速度——XY加速度
	double dDispAccXY;
	// 点胶速度——Z轴速度
	double dDispVelZ;
	// 点胶加速度——Z轴加速度
	double dDispAccZ;
	// Z提升高度,
	double dZupHigh;
	// 空闲速度
	double FreeVal;
	// 空闲加速度
	double FreeAcc;
	// 画圆速度
	double CircleVel;
	// 画圆加速度
	double CircleAcc;
	// 画线速度
	double LineVel;
	// 画线加速度
	double LineAcc;
	// 线段长度
	double LineLength;
	// 矩形长度
	double RectLength;
	// 矩形宽度
	double RectWidth;
	// 开始点胶位
	double DispenseStartPos[5];
	// 三级行数
	int ThirdDispRow;
	// 三级列数
	int ThirdDispColumn;
	// 三级行距
	double ThirdDispRowD;
	// 三级列距
	double ThirdDispColumnD;
	// 二级行数
	int SecondDispRow;
	// 二级列数
	int SecondDispColumn;
	// 二级行距
	double SecondDispRowD;
	// 二级列距
	double SecondDispColumnD;
	// 一级行数
	int FirstDispRow;
	// 一级列数
	int FirstDispColumn;
	// 一级行距
	double FirstDispRowD;
	// 一级列距
	double FirstDispColumnD;
	// 不规则矩阵——行数
	int IrregularRow;
	// 不规则矩阵——列数
	int IrregularColumn;
	// 不规则矩阵——行距
	double IrregularRowD[20];
	// 不规则矩阵——列距
	double IrregularColumnD[50];
	// 不规则矩阵——支架长度
	double IrregularSlotLength;
	// 不规则矩阵——支架宽度
	double IrregularSlotWidth;
	// 多点行数
	int MultiDispRow;
	// 多点列数
	int MultiDispColumn;
	// 多点行距
	double MultiDispRowD;
	// 多点列距
	double MultiDispColumnD;
	// 点胶阀体数量
	int DispFCount;
	// 阀体选择
	bool FSelectVal[3];
	// 点胶模式
	int DispMode;
	// 清洗针头
	int CleanNd;
	// 首点试胶
	int FirstTest;
	// 正反面检测
	int SlotDetect;
	// 填充试胶
	int FullTestDot;
	// 不规则矩阵
	int IrregularMatrix;
	// 单阀点胶
	int SingleValve;
	// 填充清洗
	int FullClean;
	// 门禁开关
	int DoorSwitch;
	// 点胶完成后填充
	int DispFinishFill;
	// 清洗出胶
	int CleanDischarge;
	// 暂停清洗试胶
	int PauseCleanTest;
	// 测高
	int MeasureHeightEnable;
	// 自动对针
	int AutoAdjust;
	//开机自动加热
	int HeatWhenStart;
	//启动前温度判断
	int TempJudgeBeforeStart;
	//针头高度检测
	int NeedleHeightCheck;
	// 支架检测
	//int OFDetected;
	// 提升停顿高度
	double dPauseHeight;
	// 提升停顿时间
	double dPauseTime;
	// 首点点胶延时
	double dFirstDispDelay;
	double dDispRadius;
	double dDistance;
	double dGrabImageDelay;
	double dCompAngle;
	double dCleanDelay;
	//温度检测偏差
	double dTempError;
	//点胶区设定温度
	double dHeatSetting;
	DispFillMode fillMode;//////
	VisionMode visionMode;
	InspectMode inspectMode;
	Orientation dispOrientation;
	Sequence dispSequence;
	ValveSelect valveSelect;
	MatchMode matchMode;
	NeedleSearchMode searchMode;
	// 校正吐胶量
	double HeightGlueVol;
	// 测高标准值
	double ContactStandardValue;
	// 接触测高加速度
	double ContactMeasureAcc;
	// 接触测高速度
	double ContactMeasureVel;
	// 接触传感器位置
	double ContactSensorPos[5];
	// 高度传感器位置
	double HeightSensorPos[3];
	//相机看测高感应器位置//与激光标定用
	double CameraOnContactor[2];
	// 高度传感器标准值
	double HeightStandardValue;
	// 清洗间距
	double CleanGap;
	// 针头间距
	double NeedleGap;
	// 针头校正加速度
	double NeedleAcc;
	// 针头校正速度
	double NeedleVel;
	// 针头位置
	double NeedlePos[5];
	// 自定义模式下，定义点的个数
	int ListElementNum;
	// 试胶延时
	double dTestDotDelay;
	// 试胶点数
	int TestDotNumber;
	// 点胶偏移X
	double DispOffsetX;
	// 点胶偏移Y
	double DispOffsetY;
	// 最大偏差角度
	double MaxOffsetAngle;
	// 测高点数
	int HeightNumber;
	// 测高误差
	double dHeightError;
	// 测高延时
	double dHeightDelay;
	//缺颗粒判断比较值   //2016-01-16 add
	int nLackPadCmpValue;
	// 点胶高度
	double dDispenseHeight;
	// 最大调整X值
	double MaxOffsetX;
	// 最大调整Y值
	double MaxOffsetY;

	// 支架正反检测位置
	tgPos tSlotDetectPos;
	// 轨迹标定位置
	tgPos tgCustomLabelPos;
	// 试胶位置
	tgPos tgTestDispPos[10];
	// 扫描延时
	double ScanDelay;
	// A轨道加热标记
	int HeatOne;
	// B轨道加热标记
	int HeatTwo;

	// 开始位-相机位
	double ImageDispensePosition[3];
	// 开始位-测高位
	double HeightDispensePosition[3];
	// 取图数
	int nImageNum;
	// 取图位
	tgPos tgImagePos[3];
	// 轨迹信息
	TrackInfo tgTrackInfo;
	//多点轨迹
    TrackInfoMulti tgTrackInfoMulti;
	// 测高位置
	tgPos tgMeasureHeightPos[10];
	// 首片激光数据
	double dLaserDataBase[10];
	// 缺颗检测位置
	tgPos tPadDetectPos;
	// 标定坐标值
	tgPos tgLabelPos[6];
	//标定坐标数据补偿
	double offset2x;
	double offset2y;
	double offset3x;
	double offset3y;
	
	//针对产品偏移Y向补偿阀体
	double dComposedSecondY;
	double dComposedThirdY;
	//针对产品偏移X向补偿阀体
	double dComposeSecondX;
	double dComposedThirdX;

	// 针头自动调整
	double dAutoNeedleGlueVol;
	AutoNeedleStep tgAutoNeedleStep;
	double dAutoNeedleErrorX;
	double dAutoNeedleErrorY;
	int nAutoAdjustCount;
	int nBlobThreshold;
	int nBlobAreaMin;
	int nBlobAreaMax;
	int nBlobLengthMin;
	int nBlobLengthMax;
	int nBlobHeightMin;
	int nBlobHeightMax;

	// 点胶偏移补偿角度
	double AdjustNeedleAngle[2];
	//激光测高时初始ZA坐标；
	double dNeedleLaserHeight;
	//自动对针完毕马达ABCD位置;
	double dAdjustMotorInRunA;
	double dAdjustMotorInRunB;
	double dAdjustMotorInRunC;
	double dAdjustMotorInRunD;
	//胶水监控功能：新增
	//是否使能
	int ArrangeEnable;
	//约定时间
	double ArrangeSpan;
	//预约时间点
	long ArrangeTime;
	//密封圈寿命
	long lGasketRingLife;
	//Z向DISPPOS微调
	double dZDispOffsetAll;
	//阀体清洗监控
	int ValveMonitorEnable;
	//监控时长 
	double ValveMonitorSpan;
	//监控开始时间
	long ValveMonitorTime;
	// 阀体清洗提醒时间(h)
	int bCleanRemind;
	double CleanRemindTimeHour;
	int bMesOffline;
	//测高补偿极限值
	double dHeightCompMax;
	//接触测高频率
	long ContactFrequency;
	//接触测高容许偏差
	double ContactErrPermit;
	//mes 数据下限
	double MesDataMin;
	//mes 数据上限
	double MesDataMax;
	//UDP相关
	bool UDPOffline;
	bool UDPRestConnet;
	bool UDPSendMsgFlag;
	int ServerPort;
	int FCompanyCode;
	char MachineID[255];
	char ServerURL[255];
	char ServerIP[255];
	//char UDPSendMsg[255];
	CString UDPSendMsg;
	CString Save_rev_CodeID;

	double dPreHeatTemp;
	double dDispHeatTemp;
	double dRealPreHeatTemp;
	double dRealDispHeatTemp;
	int nCheckIsCompensate;
};

struct tgV9Param
{
	tgV9Param()
	{
		reset();
	}
	void reset()
	{
		DispVolume=0.5;
		DispTime=2.0;
		DispCount=10;
		CleanSpeed=5;
		FullSpeed=5;
		CleanCount=5;
		DispAcc=0.5;
		FirstCompVol=0.0;
		ForwardVol=0.0;
		StartVel = 0.2;
		TestVol = 0.1;
		DispNum=10;
		dHeightFullALL=35;
		CompVolume=0.0;
		
	}
	// 胶量
	double DispVolume;
	// 点胶时间
	double DispTime;
	// 点胶加减速时间
	double DispAcc;
	// 单程次数
	long DispCount;
	// 填充速度
	double FullSpeed;
	// 清洗速度
	double CleanSpeed;
	// 清洗数量
	long CleanCount;
	// 复位距离
	double RestDistance;
	// 单次点胶行程
	double DispDistance;
	// 点胶速度
	double dispspeed;
	// 回吸速度
	double BackSpeed;
	// 回吸胶量
	double BackVolume;
	// 第一点胶位补偿胶量
	double FirstCompVol;
	// 画线提前出胶量
	double ForwardVol;
	// 起跳速度
	double StartVel;
	// 试胶胶量
	double TestVol;
	// 点胶次数
	long DispNum;
	//手动联动复位高度
	double dHeightFullALL;
	// 补偿胶量
	double CompVolume;//20180503 add by zwg

	
};
struct GLUE_MSG{

	GLUE_MSG()
	{
		for (int i=0;i<3;i++)
		{
			rev_MESGlue[i] = 0;
		}
		rev_CodeID = "";
		Qrev_CodeID = "";
		rev_bIsDischarge = false;
		rev_Succeed = false;
	}
	double rev_MESGlue[3];
	CString rev_CodeID;
	CString Qrev_CodeID;
	bool rev_bIsDischarge;
	bool rev_Succeed;
};
//////////////////////////////////////////////////////////////////////////
//命令类型


#endif