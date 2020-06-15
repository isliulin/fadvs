#pragma once


// CV9Control

class CV9Control : public CWnd
{
	DECLARE_DYNAMIC(CV9Control)
public:
	CV9Control();
	virtual ~CV9Control();

protected:
	DECLARE_MESSAGE_MAP()
public:
	int V9C_Home(int axis);
	int V9C_Full(int axis,bool bRestRun=FALSE,bool UseParam=TRUE, double m_Fullspeed=0,double m_RestD=0);
	bool CheckSensor(int SensorID);
	int V9C_Dot(int axis,bool bCleanOrTest,bool UseParam=true,double disPos=0,double dotspeed=0,double Rest_distance=0);
	int V9C_Test(int axis,bool UseParam=true,double disPos=0,double dotspeed=0,double Rest_distance=0);
	int V9C_Clearn(int axis,int ClearnCout,double ClearnSpeed);
	int V9C_Rest(int axis,bool UseParam=TRUE, double m_Fullspeed=0,double m_RestD=0);
	int V9C_Dot_Line(int axis);
	int V9C_Dot_Line(int axis,bool bForward);
	void V9C_Dot(int axis,double dVolumn);
	void V9C_Dot_Forward(int axis);
	bool V9C_SetProfile();
	bool V9C_WaitStop();
	int V9C_Defoam(int axis);

	int V9_RunCount;
};


