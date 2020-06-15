#ifndef _RFIDPUBLIC_
#define _RFIDPUBLIC_

#define STX			0x02
#define ETX			0x03
#define ProLen		112

//byte 0 1,�ṹ��
struct plateIDSTRUCT
{
	unsigned short chPlateIDType:8;	//���ֱ�
	unsigned short chPlateIDNo:8;	//���̱���
	
};
//byte 0 1,�ṹ��&����
union PLATEID
{
	unsigned short sPlateIDSingle;
	struct plateIDSTRUCT PlateID; 
};

//byte 2,�ṹ��
struct XYSizeSTRUCT
{
	unsigned char chYSize:4;	//����
	unsigned char chXSize:4;	//����
};
//byte 2,�ṹ��&����
union XYSIZE
{
	unsigned char chXYSizeSingle;
	struct XYSizeSTRUCT XYSize; 
	
};

//byte 3,�ṹ��
struct XYPitchSTRUCT
{
	unsigned char chYPitch:4;	//Y�������
	unsigned char chXPitch:4;	//X�������
};
//byte 3,�ṹ��&����
union XYPITCH
{
	unsigned char chXYPitchSingle;
	struct XYPitchSTRUCT XYPitch; 
	
};

//byte 12 13,�ṹ��
struct cCOUNTSTRUCT
{
	unsigned short chCountHigh:8;	//����������λ
	unsigned short chCountLow:8;	//����������λ
	
};
//byte 12 13,�ṹ��&����
union CYCLECOUNT
{
	unsigned short sCOUNTSingle;
	struct cCOUNTSTRUCT cCOUNTSTRUCT; 
};



//byte 32-255,�ṹ��
struct PRODATASTRUCT
{
	unsigned short chNGRAngle:8;	//NG �ж�/��ת�Ƕ�,0xFF = NG
	unsigned short :8;			//����
};
//byte 32-255,�ṹ��&����
union PRODATA
{
	unsigned short sProDataSingle;
	struct PRODATASTRUCT ProductData; 
	
};


typedef struct RF_T5P10
{
	union PLATEID uPlateID;		//����ID :				��8λ:���ֱ�
						   		//						��8λ:ID
	union XYSIZE uXYSize;		//������:				��4λ:����
								//						��4λ:����
	union XYPITCH uXYPitch;		//XY����(��λ:mm):	��4λ:X�������
								//						��4λ:Y�������
	unsigned char dispTime;		//�㽺��disp Quantity 	0.01s��λ
	unsigned char dispRds;		//�㽺�뾶disp Radius	0.1mm��λ
	unsigned char dispAngle;	//�㽺�Ƕ�disp Angle	�ȵ�λ
	unsigned char dispHeight;	//�㽺�߶�disp Height   0.1mm��λ
	
	unsigned char:8;			//��������1
	unsigned char:8;			//��������2
	unsigned char:8;			//��������3
	unsigned char:8;			//��������4

	union CYCLECOUNT cycleCount;//�����������
								//ע��:���ݸ�ʽΪ���CPU��ʽ��

	unsigned char:8;
	unsigned char:8;

	unsigned char NoASC[16];	//�������ַ���
	
	union PRODATA sProductData[112];//������ÿ����Ʒ����������
							
}	RFID_TAG;

/*
typedef struct DISP_DATA
{
	;
}DISPDATA;
*/

extern RFID_TAG RF;

#endif
