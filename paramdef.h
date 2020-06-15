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
	HEIGHT_STATUS_EXCEED_HEIGHTCOMP_LIMIT,//����ֵ����
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

struct tagDPostion//�㽺λ��
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

struct tagStautDis//��ʾ״̬
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
		//strcpy(szApp, "TSCtrlSys LEDӫ��۸��ٵ㽺ϵͳ");
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
		BmpPadDetectFirstPixselX=0;//�׵���������X
		BmpPadDetectFirstPixselY=0;//�׵���������Y

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
	// ϵͳ����
	char szApp[255];
	// ��Ʒ�ͺ�
	char szProductID[10];
	// �˶���
	tgAxis tAxis[12];
	// �༭�켣�ٶ�
	double dVelEditXY[3], dVelEditZ[3];
	// ����˿ڶ��壺�㽺��F,�㽺��F,..�̵�G,�Ƶ�Y,���R,����A
	char szOutputPortDisp[511];
	// ���밴ť�˿ڶ���:����R����ͣP��ֹͣS������E,�㽺��F
	char szInputPortButton[511];
	int iXDir;
	int iYDir;
	int iZDir;
	int iIsLog; //�Ƿ�д��־
	int iTimeLogOut;//�Զ�ע��ʱ��
	// �Ӵ���ߴ���������
	ContactSensorType sensorType;
	// ��������
	ValveNum valveNum;
	//PLC����
	PLC_TYPE PlcType;
	//��������
	VALVE_TYPE ValveType;
	// �߶ȴ���������
	HeightSensorType heightSensorType;
	// ����������
	DetectCameraType detectCameraType;
	//�˿ںš�����ͣ
	int inEmergecy;
	//�˿ںš�����ͣor����---������ť
	int inPauseOrRun;
	//�˿ںš���ֹͣ--ͣ��ťֹ
	int inStop;
	//�˿ںš�����λ
	int inReset;
	//�˿ںš����Ӵ����
	int inHeight;
	//�˿ںš���PLC����
	int inPlcRun;
	//�˿ںš���PLC��ͣ
	int inPlcPause;
	//�˿ںš���PLC׼����
	int inPlcReady;
	//�˿ںš���PLC�쳣
	int inPlcAlarm;
	//�˿ںš���������
	int inRailRun;
	//�˿ںš����㽺��������λ
	int inValveOpenSen[3];
	//�˿ںš����㽺������ԭλ
	int inValveCloseSen[3];
	//�˿ںš�������ź�
	int inTransitionDetect;
	//�˿ںš����Ž�����
	int inDoorSwitch;

	//����źš����źŵ�
	int outGreen, outYellow, outRed;
	//����źš��������ϴ
	int outNeedleClean;
	//����źš����򿪷���
	int outValveOpen[3];
	//����źš��������ѹ
	int outOpenPress[3];

	//����źš�������Դ ��ȱ����
	int outDetectLight;
	//����źš��������Դ �����뻷��
	int outNeedleLight;
	//����źš���Mark���Դ��Mark ���
	int outMarkLight;
	//����źš����չ��
	int outDayLight;
	
	/* TO_PLC */
	//����źš���׼����
	int outReady;
	//����źš�����������
	int outValve[2];
	//����źš����㽺����ź�
	int outDispenseFinish;
	//����źš���������
	int outDetectFinish;
	//����źš�����ת����
	int outRotatingLight;

	double dLook[5];
	int iLookNum;//200
	// ����ͨ�Ų�������������
	int nBaud[4];
	// ����ͨ�Ų����������ݳ���
	int nBytes[4];
	// ����ͨ�Ų���������żУ�� 0:��У�飻1:��У�飨Odd����2:żУ�飨Even��
	int nParity[4];
	// ͼ���������BLOB������������X
	double BmpCenterX[2];
	// ͼ���������BLOB������������Y
	double BmpCenterY[2];
	// ͼ���������BLOB�����궨�ı�׼����
	int StandardLen;
	// ͼ���������BLOB������������ر�ֵ
	double BmpScale;
	// ͼ���������BLOB������������
	CRect BmpROI[2];
	// ͼ���������BLOB���������Сֵ
	double BmpAreaMin;
	// ͼ���������BLOB����������ֵ
	double BmpAreaMax;
	// ͼ���������BLOB�����߶����ֵ
	double BmpHeighMax;
	// ͼ���������BLOB�����߶���Сֵ
	double BmpHeighMin;
	// ͼ���������BLOB����������ֵ
	double BmpWidthMax;
	// ͼ���������BLOB���������Сֵ
	double BmpWidthMin;
	// ͼ���������MARK������������ر�ֵ
	//double BmpMarkScale;
	// ͼ���������MARK����ģ�崰��
	CRect BmpMarkLearnWin[2];
	//ͼ�����----MARK---���ο����ģ��
	CRect BmpMarkRefCam;
	
	// ͼ���������MARK����ƥ�����//PadDetect
	double BmpMarkAcceptance;
	// ͼ���������MARK����ƥ��Ƕ�//PadDetect
	double BmpMarkAngle;

	//ͼ�����--NEEDLEMARK--��ͷƥ���
     double BmpNeedleMarkAcceptance;
	//ͼ�����--NEEDLEMARK--�����Ƕ�
     double BmpNeedleMarkAngle;
	// ͼ���������MARK�������Ҵ���
	CRect BmpMarkROI;
	//ͼ�����--MARKREF--��������
	CRect BmpMarkRefCamROI;

	// ���У������
	tgCameraCalibration BmpMarkCalibration;
	//���У����������׼�ο������ʱ��
	tgCameraCalibration BmpMarkRefCamCalibration;

	// ͼ���������SlotDetect����ģ�崰��
	CRect BmpSlotLearnWin;
	// ͼ���������SlotDetect����ƥ�����
	double BmpSlotAcceptance;
	// ͼ���������SlotDetect����ƥ��Ƕ�
	double BmpSlotAngle;
	// ͼ���������SlotDetect�������Ҵ���
	CRect BmpSlotROI;
	// ���У����������SlotDetect
	tgCameraCalibration BmpSlotCalibration;
	// ͼ���������PATTERN������������ر�ֵ
	double BmpPatternScale;
	// ͼ���������PATTERN����ģ�崰��
	CRect BmpPatternLearnWin;
	// ͼ���������PATTERN�������Ҵ���
	CRect BmpPatternROI;
	// ͼ���������PATTERN����ƥ�����
	double BmpPatternAcceptance;
	// ͼ���������PATTERN����ƥ��Ƕ�
	double BmpPatternAngle;
	// ���������������
	long CameraGain;
	// ������������ع�ʱ��
	long CameraExposureTime;
    //����ͷ���--����
	long NeedleCameraGain;
	//����ͷ���--�ع�ʱ��
	long NeedleCameraExposure;
	// ͼ���������Pad Detect����ģ�崰��//0 used find pad ;1 used find frame
	CRect BmpPadDetectLearnWin[5];
	//ͼ�����--Needle---ģ�崰��
	CRect BmpNeedleLearnWin;
	// ͼ���������Pad Detect�������Ҵ���//0 used find pad ;1 used find frame
	CRect BmpPadDetectROI[2];
	//ͼ�����--Needle--ROI
	CRect BmpNeedleROI;
	// ͼ���������Pad Detect����ƥ�����
	double BmpPadDetectAcceptance[2];
	// ͼ���������Pad Detect����ƥ��Ƕ�
	double BmpPadDetectAngle[2];

	// ͼ���������Pad Detect������������X
	double BmpPadDetectCenterX;
	// ͼ���������Pad Detect������������Y
	double BmpPadDetectCenterY;
	//ͼ�����--Pad Detect--�׵���������X
	double BmpPadDetectFirstPixselX;
	//ͼ�����--Pad Detect--�׵���������Y
	double BmpPadDetectFirstPixselY;
	// ͼ���������Pad Detect�������У������
	tgCameraCalibration BmpPadDetectCalibration;
	// �������������������������
	long CameraDetectGain;
	// ����������������������ع�ʱ��
	long CameraDetectExposureTime;

	//ͼ�����--��ͷУ��---���У��
	tgCameraCalibration BmpNeedleCalibration;

	//ͼ�����--��ͷУ��---���У��//��ABCD ������ϵ��
	tgCameraCalibration BmpStepNeedleCalibration;


	// ��ͷ�Զ���������
	tgCameraCalibration AutoNeedleCalib;
	CRect AutoNeedleROI;
	CRect AutoNeedleModelWin;
	double dAutoNeedleAcceptance;
	double dAutoNeedleAngle;
	tgPos tgNeedleOffset[3];

	int nSubstrateType;
	//��ͷ��ϴʱ��
	int  NeedleTime;
    //�������Zλ�ã�
	double dFocusZPos;
	//���Ե㽺λ
	double TestDotPostion[5];
	//����λ
	double DischargeGluePostion[5];
	//��ϴλ
	double CleanPosition[5];

	// V9���Ʋ�����������������
	int ControlType;
	// V9���Ʋ��������㽺ϵ��
	double Dispratios;
	// V9���Ʋ����������г�
	double DisEndDistance;
	// V9���Ʋ���������˨ֱ��
	double EmbolisD;
	// V9���Ʋ��������㽺�վ���
	//��ֵ��ʶ���в��ò��֣����ݷ������Ͳ�ͬʹ�÷�ʽ��һ��[H9:һ�ξ���/M9�����Ա�ʶ����λ��]
	double DispFreeDistance;
	// V9���Ʋ���������λ����
	double   RestDistance;
	// V9���Ʋ�����������ٶ�
	double  DispMaxSpeed;
	// V9���Ʋ������������ٶ�
	double  DispMaxAcc;
	// �ŷ����������������ٶ�
	double MotorMaxSpeed;
	// �ŷ�����������������ٶ�
	double MotorMaxAcc;
	// V9�����Ƿ�ɹ�
	bool V9HomeSu;
	// ��������Ƿ�ɹ�
	bool ThreeAxHomeSu;
	//�����������Ƿ�ɹ�
	bool AdjustMotorHomeSu;	
/////////////////////////////////////////////////////////////////////
	double dPixelScaleX,dPixelScaleY;
	// ����
	long StatCapacity;
	double MotorAInitPos,MotorCInitPos;
	//�ϴ���ϴʱ��
	char szLastCleanTime[255];
	//mark ������
	int nCamMarkNo;
	//needle ������
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
		//��̨M9
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
        //��ˮԤԼ��ز���
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
		//UDP���
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
	//����㽺����:ǰ��ʱ�䣬�㽺ʱ�䣬�㽺������ͣʱ��
	double dPreTime, dDispTime, dPostDisTime;
	// �㽺�ٶȡ���XY�ٶ�
	double dDispVelXY;
	// �㽺�ٶȡ���XY���ٶ�
	double dDispAccXY;
	// �㽺�ٶȡ���Z���ٶ�
	double dDispVelZ;
	// �㽺���ٶȡ���Z����ٶ�
	double dDispAccZ;
	// Z�����߶�,
	double dZupHigh;
	// �����ٶ�
	double FreeVal;
	// ���м��ٶ�
	double FreeAcc;
	// ��Բ�ٶ�
	double CircleVel;
	// ��Բ���ٶ�
	double CircleAcc;
	// �����ٶ�
	double LineVel;
	// ���߼��ٶ�
	double LineAcc;
	// �߶γ���
	double LineLength;
	// ���γ���
	double RectLength;
	// ���ο��
	double RectWidth;
	// ��ʼ�㽺λ
	double DispenseStartPos[5];
	// ��������
	int ThirdDispRow;
	// ��������
	int ThirdDispColumn;
	// �����о�
	double ThirdDispRowD;
	// �����о�
	double ThirdDispColumnD;
	// ��������
	int SecondDispRow;
	// ��������
	int SecondDispColumn;
	// �����о�
	double SecondDispRowD;
	// �����о�
	double SecondDispColumnD;
	// һ������
	int FirstDispRow;
	// һ������
	int FirstDispColumn;
	// һ���о�
	double FirstDispRowD;
	// һ���о�
	double FirstDispColumnD;
	// ��������󡪡�����
	int IrregularRow;
	// ��������󡪡�����
	int IrregularColumn;
	// ��������󡪡��о�
	double IrregularRowD[20];
	// ��������󡪡��о�
	double IrregularColumnD[50];
	// ��������󡪡�֧�ܳ���
	double IrregularSlotLength;
	// ��������󡪡�֧�ܿ��
	double IrregularSlotWidth;
	// �������
	int MultiDispRow;
	// �������
	int MultiDispColumn;
	// ����о�
	double MultiDispRowD;
	// ����о�
	double MultiDispColumnD;
	// �㽺��������
	int DispFCount;
	// ����ѡ��
	bool FSelectVal[3];
	// �㽺ģʽ
	int DispMode;
	// ��ϴ��ͷ
	int CleanNd;
	// �׵��Խ�
	int FirstTest;
	// ��������
	int SlotDetect;
	// ����Խ�
	int FullTestDot;
	// ���������
	int IrregularMatrix;
	// �����㽺
	int SingleValve;
	// �����ϴ
	int FullClean;
	// �Ž�����
	int DoorSwitch;
	// �㽺��ɺ����
	int DispFinishFill;
	// ��ϴ����
	int CleanDischarge;
	// ��ͣ��ϴ�Խ�
	int PauseCleanTest;
	// ���
	int MeasureHeightEnable;
	// �Զ�����
	int AutoAdjust;
	//�����Զ�����
	int HeatWhenStart;
	//����ǰ�¶��ж�
	int TempJudgeBeforeStart;
	//��ͷ�߶ȼ��
	int NeedleHeightCheck;
	// ֧�ܼ��
	//int OFDetected;
	// ����ͣ�ٸ߶�
	double dPauseHeight;
	// ����ͣ��ʱ��
	double dPauseTime;
	// �׵�㽺��ʱ
	double dFirstDispDelay;
	double dDispRadius;
	double dDistance;
	double dGrabImageDelay;
	double dCompAngle;
	double dCleanDelay;
	//�¶ȼ��ƫ��
	double dTempError;
	//�㽺���趨�¶�
	double dHeatSetting;
	DispFillMode fillMode;//////
	VisionMode visionMode;
	InspectMode inspectMode;
	Orientation dispOrientation;
	Sequence dispSequence;
	ValveSelect valveSelect;
	MatchMode matchMode;
	NeedleSearchMode searchMode;
	// У���½���
	double HeightGlueVol;
	// ��߱�׼ֵ
	double ContactStandardValue;
	// �Ӵ���߼��ٶ�
	double ContactMeasureAcc;
	// �Ӵ�����ٶ�
	double ContactMeasureVel;
	// �Ӵ�������λ��
	double ContactSensorPos[5];
	// �߶ȴ�����λ��
	double HeightSensorPos[3];
	//�������߸�Ӧ��λ��//�뼤��궨��
	double CameraOnContactor[2];
	// �߶ȴ�������׼ֵ
	double HeightStandardValue;
	// ��ϴ���
	double CleanGap;
	// ��ͷ���
	double NeedleGap;
	// ��ͷУ�����ٶ�
	double NeedleAcc;
	// ��ͷУ���ٶ�
	double NeedleVel;
	// ��ͷλ��
	double NeedlePos[5];
	// �Զ���ģʽ�£������ĸ���
	int ListElementNum;
	// �Խ���ʱ
	double dTestDotDelay;
	// �Խ�����
	int TestDotNumber;
	// �㽺ƫ��X
	double DispOffsetX;
	// �㽺ƫ��Y
	double DispOffsetY;
	// ���ƫ��Ƕ�
	double MaxOffsetAngle;
	// ��ߵ���
	int HeightNumber;
	// ������
	double dHeightError;
	// �����ʱ
	double dHeightDelay;
	//ȱ�����жϱȽ�ֵ   //2016-01-16 add
	int nLackPadCmpValue;
	// �㽺�߶�
	double dDispenseHeight;
	// ������Xֵ
	double MaxOffsetX;
	// ������Yֵ
	double MaxOffsetY;

	// ֧���������λ��
	tgPos tSlotDetectPos;
	// �켣�궨λ��
	tgPos tgCustomLabelPos;
	// �Խ�λ��
	tgPos tgTestDispPos[10];
	// ɨ����ʱ
	double ScanDelay;
	// A������ȱ��
	int HeatOne;
	// B������ȱ��
	int HeatTwo;

	// ��ʼλ-���λ
	double ImageDispensePosition[3];
	// ��ʼλ-���λ
	double HeightDispensePosition[3];
	// ȡͼ��
	int nImageNum;
	// ȡͼλ
	tgPos tgImagePos[3];
	// �켣��Ϣ
	TrackInfo tgTrackInfo;
	//���켣
    TrackInfoMulti tgTrackInfoMulti;
	// ���λ��
	tgPos tgMeasureHeightPos[10];
	// ��Ƭ��������
	double dLaserDataBase[10];
	// ȱ�ż��λ��
	tgPos tPadDetectPos;
	// �궨����ֵ
	tgPos tgLabelPos[6];
	//�궨�������ݲ���
	double offset2x;
	double offset2y;
	double offset3x;
	double offset3y;
	
	//��Բ�Ʒƫ��Y�򲹳�����
	double dComposedSecondY;
	double dComposedThirdY;
	//��Բ�Ʒƫ��X�򲹳�����
	double dComposeSecondX;
	double dComposedThirdX;

	// ��ͷ�Զ�����
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

	// �㽺ƫ�Ʋ����Ƕ�
	double AdjustNeedleAngle[2];
	//������ʱ��ʼZA���ꣻ
	double dNeedleLaserHeight;
	//�Զ�����������ABCDλ��;
	double dAdjustMotorInRunA;
	double dAdjustMotorInRunB;
	double dAdjustMotorInRunC;
	double dAdjustMotorInRunD;
	//��ˮ��ع��ܣ�����
	//�Ƿ�ʹ��
	int ArrangeEnable;
	//Լ��ʱ��
	double ArrangeSpan;
	//ԤԼʱ���
	long ArrangeTime;
	//�ܷ�Ȧ����
	long lGasketRingLife;
	//Z��DISPPOS΢��
	double dZDispOffsetAll;
	//������ϴ���
	int ValveMonitorEnable;
	//���ʱ�� 
	double ValveMonitorSpan;
	//��ؿ�ʼʱ��
	long ValveMonitorTime;
	// ������ϴ����ʱ��(h)
	int bCleanRemind;
	double CleanRemindTimeHour;
	int bMesOffline;
	//��߲�������ֵ
	double dHeightCompMax;
	//�Ӵ����Ƶ��
	long ContactFrequency;
	//�Ӵ��������ƫ��
	double ContactErrPermit;
	//mes ��������
	double MesDataMin;
	//mes ��������
	double MesDataMax;
	//UDP���
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
	// ����
	double DispVolume;
	// �㽺ʱ��
	double DispTime;
	// �㽺�Ӽ���ʱ��
	double DispAcc;
	// ���̴���
	long DispCount;
	// ����ٶ�
	double FullSpeed;
	// ��ϴ�ٶ�
	double CleanSpeed;
	// ��ϴ����
	long CleanCount;
	// ��λ����
	double RestDistance;
	// ���ε㽺�г�
	double DispDistance;
	// �㽺�ٶ�
	double dispspeed;
	// �����ٶ�
	double BackSpeed;
	// ��������
	double BackVolume;
	// ��һ�㽺λ��������
	double FirstCompVol;
	// ������ǰ������
	double ForwardVol;
	// �����ٶ�
	double StartVel;
	// �Խ�����
	double TestVol;
	// �㽺����
	long DispNum;
	//�ֶ�������λ�߶�
	double dHeightFullALL;
	// ��������
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
//��������


#endif