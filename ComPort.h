#pragma once
#include "SerialPort.h"
#include "paramdef.h"

class CComPort/* :
			  public CSerialPort*/
{
public:
	CComPort(void);
	~CComPort(void);
	CSerialPort m_Port[5];//0��PLCͨѸ/�ĸ�V9
public:
	//PLCͨѸ
	long ReadPlcData(CString PLCDataID);				//��ȡ˫�ֽ�����DT1-DT2
	BOOL WritePlcYOut(CString OutID, bool on_off);
	BOOL WritePlcROut(CString OutID, bool on_off);
	BOOL WritePLCData(CString DTID, long DateVal);
	int ReadPlcRData(CString PLCRDataID);
	bool ConnectPlc(CString &str);
	bool DisconnectPlc(CString &str);

	bool ReadPlcData(long *data,CString strType,CString strNum,CString strFormat="");
	bool WritePlcData(long data,CString strType,CString strNum,CString strFormat="");
	// ��ߴ�����
	double	ReadData(char *string,int nPortID,DeviceType deviceType);
	bool SetBand(int nPortID);

	//��ƽͨѶ
	bool ReadBalance(double *pData,bool *bStable);
	bool ReadBalance(double *pData,int nPortID,bool *bStable);//COM3
	void BalanceZero(int nPortID);
	void BalanceTare(int nPortID);
	void BalanceCal(int nPortID);
	void BalanceResultImmediateBU(int nPortID);
	void BalanceResultStableBU(int nPortID);
	void BalanceResultImmediateCU(int nPortID);
	void BalanceResultStableCU(int nPortID);
	void BalanceStartTransmissionBU(int nPortID);
	void BalanceFinishTransmissionBU(int nPortID);
	void BalanceStartTransmissionCU(int nPortID);
	void BalanceFinishTransmissionCU(int nPortID);
	void ClearCommPort(int nPortID);
	bool SendGlueToMES(double dGlue);  //����ָ�MES
	bool SendHaltToMES();  //����ָ�MES
private:
	long ChangStrTOlong(CString ChangStr);
	CString AddZero(CString str, int StrCout);			//����
	BOOL CommSendPlc(CString StrPlc);
};

