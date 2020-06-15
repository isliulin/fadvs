#ifndef _RFIDPUBLIC_
#define _RFIDPUBLIC_

#define STX			0x02
#define ETX			0x03
#define ProLen		112

//byte 0 1,结构体
struct plateIDSTRUCT
{
	unsigned short chPlateIDType:8;	//机种别
	unsigned short chPlateIDNo:8;	//料盘编码
	
};
//byte 0 1,结构体&联合
union PLATEID
{
	unsigned short sPlateIDSingle;
	struct plateIDSTRUCT PlateID; 
};

//byte 2,结构体
struct XYSizeSTRUCT
{
	unsigned char chYSize:4;	//行数
	unsigned char chXSize:4;	//列数
};
//byte 2,结构体&联合
union XYSIZE
{
	unsigned char chXYSizeSingle;
	struct XYSizeSTRUCT XYSize; 
	
};

//byte 3,结构体
struct XYPitchSTRUCT
{
	unsigned char chYPitch:4;	//Y方向距离
	unsigned char chXPitch:4;	//X方向距离
};
//byte 3,结构体&联合
union XYPITCH
{
	unsigned char chXYPitchSingle;
	struct XYPitchSTRUCT XYPitch; 
	
};

//byte 12 13,结构体
struct cCOUNTSTRUCT
{
	unsigned short chCountHigh:8;	//生产计数高位
	unsigned short chCountLow:8;	//生产计数低位
	
};
//byte 12 13,结构体&联合
union CYCLECOUNT
{
	unsigned short sCOUNTSingle;
	struct cCOUNTSTRUCT cCOUNTSTRUCT; 
};



//byte 32-255,结构体
struct PRODATASTRUCT
{
	unsigned short chNGRAngle:8;	//NG 判定/回转角度,0xFF = NG
	unsigned short :8;			//保留
};
//byte 32-255,结构体&联合
union PRODATA
{
	unsigned short sProDataSingle;
	struct PRODATASTRUCT ProductData; 
	
};


typedef struct RF_T5P10
{
	union PLATEID uPlateID;		//料盘ID :				高8位:机种别
						   		//						低8位:ID
	union XYSIZE uXYSize;		//行列数:				高4位:列数
								//						低4位:行数
	union XYPITCH uXYPitch;		//XY距离(单位:mm):	高4位:X方向距离
								//						低4位:Y方向距离
	unsigned char dispTime;		//点胶量disp Quantity 	0.01s单位
	unsigned char dispRds;		//点胶半径disp Radius	0.1mm单位
	unsigned char dispAngle;	//点胶角度disp Angle	度单位
	unsigned char dispHeight;	//点胶高度disp Height   0.1mm单位
	
	unsigned char:8;			//保留区域1
	unsigned char:8;			//保留区域2
	unsigned char:8;			//保留区域3
	unsigned char:8;			//保留区域4

	union CYCLECOUNT cycleCount;//生产管理计数
								//注意:数据格式为大端CPU格式。

	unsigned char:8;
	unsigned char:8;

	unsigned char NoASC[16];	//保留的字符串
	
	union PRODATA sProductData[112];//料盘上每个产品的数据资料
							
}	RFID_TAG;

/*
typedef struct DISP_DATA
{
	;
}DISPDATA;
*/

extern RFID_TAG RF;

#endif
