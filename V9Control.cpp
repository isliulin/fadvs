// V9Control.cpp : implementation file

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "V9Control.h"
#include <math.h>

// CV9Control

IMPLEMENT_DYNAMIC(CV9Control, CWnd)

CV9Control::CV9Control()
{
   V9_RunCount=0;
}

CV9Control::~CV9Control()
{
}

// CV9Control message handlers
BEGIN_MESSAGE_MAP(CV9Control, CWnd)
END_MESSAGE_MAP()

// 阀体回零
int CV9Control::V9C_Home(int axis)
{
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		int V9ID=axis-6;
		bool selectID=FALSE;
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);
				if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
				{
					return V9ID+1;//关阀失败
				}
				theApp.m_Mv400.MoveDot(axis,1,5,theApp.m_tSysParam.DispMaxAcc,TRUE,false);
				if(!theApp.m_Mv400.HomeAxis(axis,-1000,1,0,5,1,theApp.m_tSysParam.DispMaxAcc))
				{
					return -(V9ID+1);//回零失败
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)//运行时选阀
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)//手动时选阀
			{  
				selectID=true;
			}

			for (int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//前进1MM
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
					if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
					{
						return i+1;//关阀失败
					}
					theApp.m_Mv400.MoveDot(i+6,1,5,theApp.m_tSysParam.DispMaxAcc,false,false);
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//等停止
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.WaitStop(i+6);
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//后退
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.MoveDot(i+6,-1000,5,theApp.m_tSysParam.DispMaxAcc,false,false);
				}
			}	
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.WaitStop(i+6);
					if(!theApp.m_Mv400.HomeAxis(i+6,-1000,1,0,5,1,theApp.m_tSysParam.DispMaxAcc))
					{
						return -(i+1);//回零失败
					}
				}
			}
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		int V9ID=axis-6;
		bool selectID=FALSE;
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);
				if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
				{
					return V9ID+1;//关阀失败
				}
				theApp.m_Mv400.MoveDot(axis,-1000,5,theApp.m_tSysParam.DispMaxAcc,TRUE,false);
				if(!theApp.m_Mv400.HomeAxis(axis,-1000,1,0.2,5,1,theApp.m_tSysParam.DispMaxAcc))
				{
					return -(V9ID+1);//回零失败
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)//运行时选阀
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)//手动时选阀
			{  
				selectID=true;
			}

			for (int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//前进1MM
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
					if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
					{
						return i+1;//关阀失败
					}
					theApp.m_Mv400.MoveDot(i+6,-1000,5,theApp.m_tSysParam.DispMaxAcc,false,false);
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//等停止
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.WaitStop(i+6);
				}
			}	
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					if(!theApp.m_Mv400.HomeAxis(i+6,-1000,1,0.2,5,1,theApp.m_tSysParam.DispMaxAcc))
					{
						return -(i+1);//回零失败
					}
				}
			}
		}
	}
	return 0;
}
// 阀体复位
int CV9Control::V9C_Rest(int axis,bool UseParam, double m_Fullspeed,double m_RestD)
{	
	V9_RunCount=0;
	return V9C_Full( axis,true,UseParam, m_Fullspeed, m_RestD);
}
/******************************************清洗/填充**********************************************/
//由参数2,3决定实际流程:param2=param3=(true || false)
/************************************************************************************************/
int CV9Control::V9C_Full(int axis,bool bRestRun,bool UseParam, double m_Fullspeed,double m_RestD)
{
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		int V9ID=axis-6;
		double  Tempost;
		bool selectID=FALSE;
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],FALSE);//关气			  		    
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);//关阀
				if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
				{
					return -(V9ID+1);////关阀失败
				}
				Tempost=theApp.m_Mv400.GetPos(axis);
				if (Tempost>theApp.m_tSysParam.DispFreeDistance)
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],TRUE);
				}
				Sleep(500);
				////////////////////到8MM位置/////////////////////////////////////////////////////
				if (!UseParam)//设定值
				{
					theApp.m_Mv400.MoveDot(axis,theApp.m_tSysParam.DispFreeDistance,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,theApp.m_tSysParam.DispFreeDistance,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],FALSE);
				if (!bRestRun)//填充
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],TRUE);
					if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[V9ID]))
					{
						return V9ID+1;//开阀失败
					}
				}
				/////////////////////////到极限位////////////////////////////////////////////////////////////////////	
				if (!UseParam)//设定值
				{
					theApp.m_Mv400.MoveDot(axis,theApp.m_tSysParam.DisEndDistance,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,theApp.m_tSysParam.DisEndDistance,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
				if (!bRestRun)//填充
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);
					if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
					{
						return -(V9ID+1);////关阀失败
					}
				}
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],TRUE);
				Sleep(500);
				////////////////////////////到	RST位/////////////////////////////////////////////////////////////
				if (!UseParam)
				{
					if (m_RestD==0)
					{
						m_RestD=g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance-1;
					}
					theApp.m_Mv400.MoveDot(axis,m_RestD,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance-1,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],FALSE);
				Sleep(500);
				if (!UseParam)
				{
					theApp.m_Mv400.MoveDot(axis,m_RestD,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				Sleep(500);
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],TRUE);
				if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[V9ID]))
				{
					return V9ID+1;//开阀失败
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			//////////////////////////关阀关气////////////////////////////////////////////
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if (fabs(Tempost-g_pFrm->m_CmdRun.V9Parm[i].RestDistance)>0.01)
					{
						theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
						theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
					}
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if (fabs(Tempost-g_pFrm->m_CmdRun.V9Parm[i].RestDistance)>0.01)
					{
						if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
						{
							return -(i+1);////关阀失败
						}
						Tempost=theApp.m_Mv400.GetPos(i+6);
						if (Tempost>theApp.m_tSysParam.DispFreeDistance)
						{
							theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);
						}
					}
				}
			}
			Sleep(500);
			////////////////////////////////////////////////到8MM位置////////////////////////////////////////////////////////////////////////////////////////////
			if (!bRestRun)//填充开阀
			{
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//到8MM位置
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						if (!UseParam)
						{
							theApp.m_Mv400.MoveDot(6+i,theApp.m_tSysParam.DispFreeDistance,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
						else
						{
							theApp.m_Mv400.MoveDot(6+i,theApp.m_tSysParam.DispFreeDistance,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
					}
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.WaitStop(i+6);
					}
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//关气
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
					}
				}
				if (!bRestRun)//填充开阀
				{
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//开阀
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],TRUE);
						}
					}
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//开阀检测
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[i]))
							{
								return -(i+1);////开阀失败
							}
						}
					}
				}
			}
			/////////////////////////到极限位////////////////////////////////////////////////////////////////////		 
			if (!bRestRun)//填充开阀
			{	 
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//回终点位
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						if (!UseParam)
						{
							theApp.m_Mv400.MoveDot(6+i,theApp.m_tSysParam.DisEndDistance,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
						else
						{
							theApp.m_Mv400.MoveDot(6+i,theApp.m_tSysParam.DisEndDistance,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
					}
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.WaitStop(i+6);
					}
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
				if (!bRestRun)//填充关阀
				{
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//关阀
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
						}
					}
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//关阀检测
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
							{
								return -(i+1);//关阀失败
							}
						}
					}	
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);
					}
				}
				Sleep(500);
			}
			////////////////////////////到	RST位/////////////////////////////////////////////////////////////	 
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//抽胶
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if (fabs(Tempost-g_pFrm->m_CmdRun.V9Parm[i].RestDistance)>0.01)
					{
						theApp.m_Mv400.MoveDot(6+i,g_pFrm->m_CmdRun.V9Parm[i].RestDistance-1,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
					}
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.WaitStop(i+6);
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
				}
			}
			Sleep(500);
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if (fabs(Tempost-g_pFrm->m_CmdRun.V9Parm[i].RestDistance)>0.01)
					{
						theApp.m_Mv400.MoveDot(6+i,g_pFrm->m_CmdRun.V9Parm[i].RestDistance,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
					}
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				theApp.m_Mv400.WaitStop(i+6);
			}
			Sleep(500);
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],TRUE);
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[i]))
					{
						return i+1;//开阀失败
					}
				}
			}
			g_pFrm->m_CmdRun.m_bV9FirstDisp=true;
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		int V9ID=axis-6;
		double  Tempost;
		bool selectID=FALSE;
		if(axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],FALSE);			  		    
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);
				if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
				{
					return -(V9ID+1);
				}
				Tempost=theApp.m_Mv400.GetPos(axis);
				//if (Tempost<g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance)  //20180418
				//{                                                        //20180418
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],TRUE);
				//}                                                        //20180418
				Sleep(500);
				////////////////////到8MM位置/////////////////////////////////////////////////////
				if (!UseParam)//设定值
				{
					theApp.m_Mv400.MoveDot(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],FALSE);
				if (!bRestRun)//填充
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],TRUE);
					if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[V9ID]))
					{
						return V9ID+1;//开阀失败
					}
				}
				/////////////////////////到极限位/////////////////////////////////////////////////	
				if (!UseParam)//设定值
				{
					theApp.m_Mv400.MoveDot(axis,0,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,0,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				if (!bRestRun)
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);
					if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
					{
						return -(V9ID+1);
					}
				}
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],TRUE);
				Sleep(500);
				////////////////////////////到	RST位/////////////////////////////////////////////////////////////
				if (!UseParam)
				{
					if (fabs(m_RestD)<0.001)
					{
						m_RestD=g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance;
					}
					theApp.m_Mv400.MoveDot(axis,m_RestD+1,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance+1,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],FALSE);
				Sleep(500);
				if (!UseParam)
				{
					theApp.m_Mv400.MoveDot(axis,m_RestD,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				else
				{
					theApp.m_Mv400.MoveDot(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].RestDistance,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,TRUE);
				}
				Sleep(500);
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],TRUE);
				if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[V9ID]))
				{
					return V9ID+1;
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			//////////////////////////关气关阀///////////////////////////
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
				}
			}

			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
					{
						return -(i+1);
					}
					Tempost=theApp.m_Mv400.GetPos(i+6);
					//if (Tempost>theApp.m_tSysParam.DispFreeDistance)
					//{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);//20180418
					//}
				}
			}
			Sleep(500);

			////////////////////////////////////////////////到8MM位置///////////////////////////////////////////////
			if (!bRestRun)//When Clean:增加此环节...up and open 
			{
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						if (!UseParam)
						{
							theApp.m_Mv400.MoveDot(6+i,g_pFrm->m_CmdRun.V9Parm[i].RestDistance,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
						else
						{
							theApp.m_Mv400.MoveDot(6+i,g_pFrm->m_CmdRun.V9Parm[i].RestDistance,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
					}
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.WaitStop(i+6);
					}
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
					}
				}
				if (!bRestRun)
				{
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//开阀
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],TRUE);
						}
					}
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//开阀检测
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[i]))
							{
								return -(i+1);////开阀失败
							}
						}
					}
				}
			}
			/////////////////////////到极限位/////////////////////////////////////////////////	 
			if (!bRestRun)//When Clean:增加此环节...down and close...
			{	 
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						if (!UseParam)
						{
							theApp.m_Mv400.MoveDot(6+i,0,m_Fullspeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
						else
						{
							theApp.m_Mv400.MoveDot(6+i,0,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
						}
					}
				}
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
					{
						theApp.m_Mv400.WaitStop(i+6);
					}
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
				if (!bRestRun)//填充关阀
				{
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//关阀
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
						}
					}
					for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//关阀检测
					{
						if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
						{
							if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
							{
								return -(i+1);//关阀失败
							}
						}
					}	
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);
				}
			}
			Sleep(500);
			//for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)//开阀检测
			//{
			//	if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
			//	{
			//		if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[i]))
			//		{
			//			return -(i+1);////开阀失败
			//		}
			//	}
			//}
			/////////////////////////到极限位////////////////////////////////////////////////////////////////////		 

			////////////////////////////到	RST位/////////////////////////////////////////////////////////////	 
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)// Core action in Auto mode ...
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if (0==m_RestD)
					{
						theApp.m_Mv400.MoveDot(6+i,g_pFrm->m_CmdRun.V9Parm[i].RestDistance+1,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
					}
					else
					{
						theApp.m_Mv400.MoveDot(6+i,m_RestD,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
					}
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.WaitStop(i+6);
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],FALSE);
				}
			}
			Sleep(500);
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if (0==m_RestD)
					{
						theApp.m_Mv400.MoveDot(6+i,g_pFrm->m_CmdRun.V9Parm[i].RestDistance,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
					}
					else
					{
						theApp.m_Mv400.MoveDot(6+i,m_RestD,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,FALSE);
					}
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				theApp.m_Mv400.WaitStop(i+6);
			}
			Sleep(500);
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],TRUE);
				}
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.ProductParam.FSelectVal[i])
				{
					if (!CheckSensor(theApp.m_tSysParam.inValveOpenSen[i]))
					{
						return i+1;//开阀失败
					}
				}
			}
			g_pFrm->m_CmdRun.m_bV9FirstDisp=true;
		}
	}
	return 0;
}
// 点胶
int CV9Control::V9C_Dot(int axis,bool bCleanOrTest,bool UseParam,double disPos,double dotspeed,double Rest_distance)
{
	int V9ID=axis-6;
	bool selectID=false;
	double Tempost;
	double m_BackD[3];
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				Tempost=theApp.m_Mv400.GetPos(axis);
				theApp.m_Mv400.Move(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance+Tempost,true,true);
				Tempost=theApp.m_Mv400.GetPos(axis);
				m_BackD[V9ID]=g_pFrm->m_CmdRun.V9Parm[V9ID].BackVolume/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(axis,Tempost-m_BackD[V9ID],true,true);//回吸

				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					if (!UseParam)
					{
						V9C_Rest(axis,false,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,Rest_distance);
						V9C_SetProfile();
					}
					else
					{
						V9C_Rest(axis);
						V9C_SetProfile();
					}
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if(g_pFrm->m_CmdRun.m_bV9FirstDisp&&!bCleanOrTest)
					{
						double dFirstComp = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol/theApp.m_tSysParam.Dispratios;
						theApp.m_Mv400.Move(i+6,Tempost+g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp,false,true);
						if(g_pFrm!=NULL)
						{
							CString str;
							str.Format("首点补偿:valve%d=%0.3f",i,g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol);
							g_pFrm->m_CmdRun.AddMsg(str);
						}
					}
					else
					{
						theApp.m_Mv400.Move(i+6,Tempost+g_pFrm->m_CmdRun.V9Parm[i].DispDistance,false,true);
					}
				}
			}
			if(g_pFrm->m_CmdRun.m_bV9FirstDisp&&!bCleanOrTest)
			{
				g_pFrm->m_CmdRun.m_bV9FirstDisp = false;
			}
			V9_RunCount++;
			//theApp.m_Mv400.WaitValvesStop();
			V9C_WaitStop();
			//Sleep(5);
			if(g_pFrm->m_CmdRun.V9Parm[0].BackVolume>0)//回吸
			{
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
					{
						Tempost=theApp.m_Mv400.GetPos(i+6);
						m_BackD[i]=g_pFrm->m_CmdRun.V9Parm[i].BackVolume/theApp.m_tSysParam.Dispratios;
						theApp.m_Mv400.Move(i+6,Tempost-m_BackD[i],false,true);
					}
				}
				//theApp.m_Mv400.WaitValvesStop();
				V9C_WaitStop();
			}
			//////////////////////////////////////////////////////////////////////
			int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
				{
					nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
				}
			}
			if(V9_RunCount>=nCount)
			{
				if (selectID)//全部
				{
					if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN)
					{
						return 1;
					}
					else
					{
						V9C_Rest(K_ALL_AXIS);
						V9C_SetProfile();
					}
				}//选择
				else
					//V9C_Rest(K_SELECT_AXIS);
					return 1;
			}
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				Tempost=theApp.m_Mv400.GetPos(axis);
				theApp.m_Mv400.Move(axis,Tempost-g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance,true,true);
				Tempost=theApp.m_Mv400.GetPos(axis);
				m_BackD[V9ID]=g_pFrm->m_CmdRun.V9Parm[V9ID].BackVolume/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(axis,Tempost+m_BackD[V9ID],true,true);//回吸

				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					if (!UseParam)
					{
						V9C_Rest(axis,false,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,Rest_distance);
						V9C_SetProfile();
					}
					else
					{
						V9C_Rest(axis);
						V9C_SetProfile();
					}
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					if(g_pFrm->m_CmdRun.m_bV9FirstDisp&&!bCleanOrTest)
					{
						double dFirstComp = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol/theApp.m_tSysParam.Dispratios;
						theApp.m_Mv400.Move(i+6,Tempost-g_pFrm->m_CmdRun.V9Parm[i].DispDistance-dFirstComp,false,true);
						if(g_pFrm!=NULL)
						{
							CString str;
							str.Format("首点补偿:valve%d=%0.3f",i,g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol);
							g_pFrm->m_CmdRun.AddMsg(str);
						}
					}
					else
					{
						theApp.m_Mv400.Move(i+6,Tempost-g_pFrm->m_CmdRun.V9Parm[i].DispDistance,false,true);
					}
				}
			}
			if(g_pFrm->m_CmdRun.m_bV9FirstDisp&&!bCleanOrTest)
			{
				g_pFrm->m_CmdRun.m_bV9FirstDisp = false;
			}
			V9_RunCount++;
			//theApp.m_Mv400.WaitValvesStop();
			V9C_WaitStop();
			//Sleep(5);
			if(g_pFrm->m_CmdRun.V9Parm[0].BackVolume>0)//回吸
			{
				for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
				{
					if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
					{
						Tempost=theApp.m_Mv400.GetPos(i+6);
						m_BackD[i]=g_pFrm->m_CmdRun.V9Parm[i].BackVolume/theApp.m_tSysParam.Dispratios;
						theApp.m_Mv400.Move(i+6,Tempost+m_BackD[i],false,true);
					}
				}
				//theApp.m_Mv400.WaitValvesStop();
				V9C_WaitStop();
			}
			//////////////////////////////////////////////////////////////////////
			int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
				{
					nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
				}
			}
			if(V9_RunCount>=nCount)
			{
				if (selectID)//全部
				{
					if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN)
					{
						return 1;
					}
					else
					{
						V9C_Rest(K_ALL_AXIS);
						V9C_SetProfile();
					}
				}//选择
				else
					//V9C_Rest(K_SELECT_AXIS);
					return 1;
			}
		}
	}
	return 0;
}
int CV9Control::V9C_Test(int axis,bool UseParam/* =true */,double disPos/* =0 */,double dotspeed/* =0 */,double Rest_distance/* =0 */)
{
	int V9ID=axis-6;
	bool selectID=false;
	double Tempost;
	double m_TestDistance[3];
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				Tempost=theApp.m_Mv400.GetPos(axis);
				m_TestDistance[V9ID]=g_pFrm->m_CmdRun.V9Parm[V9ID].TestVol/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(axis,m_TestDistance[V9ID]+Tempost,true,true);
				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					if (!UseParam)
					{
						V9C_Rest(axis,false,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,Rest_distance);
						V9C_SetProfile();
					}
					else
					{
						V9C_Rest(axis);
						V9C_SetProfile();
					}
				}
			}

			return 0;
		}
		else if (axis==K_SELECT_AXIS)
		{
			selectID=false; 
		}
		else if (axis==K_ALL_AXIS)
		{  
			selectID=true;
		}
		for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
		{
			if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
			{
				Tempost=theApp.m_Mv400.GetPos(i+6);
				m_TestDistance[i]=g_pFrm->m_CmdRun.V9Parm[i].TestVol/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(i+6,Tempost+m_TestDistance[i],false,true);
			}
		}
		V9_RunCount++;
		//theApp.m_Mv400.WaitValvesStop();
		V9C_WaitStop();
		//////////////////////////////////////////////////////////////////////
		int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
		for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
		{
			if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
			{
				nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
			}
		}
		if(V9_RunCount>=nCount)
		{
			if (selectID)//全部
			{
				if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN)
				{
					return 1;
				}
				else
				{
					V9C_Rest(K_ALL_AXIS);
					V9C_SetProfile();
				}
			}//选择
			else
				//V9C_Rest(K_SELECT_AXIS);
				return 1;
		}
	}
	else
	{
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				Tempost=theApp.m_Mv400.GetPos(axis);
				m_TestDistance[V9ID]=g_pFrm->m_CmdRun.V9Parm[V9ID].TestVol/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(axis,Tempost-m_TestDistance[V9ID],true,true);
				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					if (!UseParam)
					{
						V9C_Rest(axis,false,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,Rest_distance);
						V9C_SetProfile();
					}
					else
					{
						V9C_Rest(axis);
						V9C_SetProfile();
					}
				}
			}

			return 0;
		}
		else if (axis==K_SELECT_AXIS)
		{
			selectID=false; 
		}
		else if (axis==K_ALL_AXIS)
		{  
			selectID=true;
		}
		for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
		{
			if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
			{
				Tempost=theApp.m_Mv400.GetPos(i+6);
				m_TestDistance[i]=g_pFrm->m_CmdRun.V9Parm[i].TestVol/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(i+6,Tempost-m_TestDistance[i],false,true);
			}
		}
		V9_RunCount++;
		//theApp.m_Mv400.WaitValvesStop();
		V9C_WaitStop();
		//////////////////////////////////////////////////////////////////////
		int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
		for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
		{
			if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
			{
				nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
			}
		}
		if(V9_RunCount>=nCount)
		{
			if (selectID)//全部
			{
				if(g_pFrm->m_CmdRun.m_tStatus==K_RUN_STS_RUN)
				{
					return 1;
				}
				else
				{
					V9C_Rest(K_ALL_AXIS);
					V9C_SetProfile();
				}
			}//选择
			else
				//V9C_Rest(K_SELECT_AXIS);
				return 1;
		}
	}

	return 0;
}
// 清洗阀体
int CV9Control::V9C_Clearn(int axis,int ClearnCout,double ClearnSpeed)
{
	for (int i=0;i<ClearnCout;i++)
	{
		if (axis>0)
		{
			V9C_Full(axis,false,FALSE,ClearnSpeed);
		}
		else if (axis==K_SELECT_AXIS)
		{
			V9C_Full(K_SELECT_AXIS,FALSE,FALSE,ClearnSpeed);
		}
		else if (axis==K_ALL_AXIS)
		{  
			V9C_Full(K_ALL_AXIS,FALSE,FALSE,ClearnSpeed);
		}
	}
	return 0;
}
// 检查点胶阀开关感应器
bool CV9Control::CheckSensor(int SensorID)
{
	DWORD StarTime,EndTime;
	StarTime= GetTickCount();
	while (!theApp.m_Mv400.GetInput(SensorID))
	{
		CFunction::DoEvents();
		EndTime= GetTickCount();
		if ((EndTime-StarTime)/1000>4)//4S
		{
			return false;
		}
		Sleep(350);
	}
	return TRUE;
}

int CV9Control::V9C_Dot_Line(int axis)
{
	int V9ID = axis - 6;
	bool selectID = false;
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		if(axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.AddBufGear(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance);
				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					return 1;
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
					{
						double dFirstComp = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol/theApp.m_tSysParam.Dispratios;
						theApp.m_Mv400.AddBufGear(i+6,g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp);
					}
					else
					{
						theApp.m_Mv400.AddBufGear(i+6,g_pFrm->m_CmdRun.V9Parm[i].DispDistance);
					}
				}
			}
			if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
			{
				g_pFrm->m_CmdRun.m_bV9FirstDisp = false;
			}
			V9_RunCount++;
			int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
				{
					nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
				}
			}
			if(V9_RunCount>=nCount)
			{
				return 1;
			}
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		if(axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.AddBufGear(axis,-g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance);
				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					return 1;
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
					{
						double dFirstComp = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol/theApp.m_tSysParam.Dispratios;
						theApp.m_Mv400.AddBufGear(i+6,-(g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp));
					}
					else
					{
						theApp.m_Mv400.AddBufGear(i+6,-g_pFrm->m_CmdRun.V9Parm[i].DispDistance);
					}
				}
			}
			if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
			{
				g_pFrm->m_CmdRun.m_bV9FirstDisp = false;
			}
			V9_RunCount++;
			int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
				{
					nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
				}
			}
			if(V9_RunCount>=nCount)
			{
				return 1;
			}
		}
	}
	return 0;
}

int CV9Control::V9C_Dot_Line(int axis,bool bForward)
{
	int V9ID = axis - 6;
	bool selectID = false;
	double dForwardDis[3];
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		if(axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				dForwardDis[V9ID] = g_pFrm->m_CmdRun.V9Parm[V9ID].ForwardVol/theApp.m_tSysParam.Dispratios;
				if(bForward)
				{
					theApp.m_Mv400.AddBufGear(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance-dForwardDis[V9ID]);
				}
				else
				{
					theApp.m_Mv400.AddBufGear(axis,g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance);
				}
				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					return 1;
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					dForwardDis[i] = g_pFrm->m_CmdRun.V9Parm[i].ForwardVol/theApp.m_tSysParam.Dispratios;
					if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
					{
						double dFirstComp = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol/theApp.m_tSysParam.Dispratios;
						if(bForward)
						{
							theApp.m_Mv400.AddBufGear(i+6,g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp-dForwardDis[i]);
						}
						else
						{
							theApp.m_Mv400.AddBufGear(i+6,g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp);
						}
					}
					else
					{
						if(bForward)
						{
							theApp.m_Mv400.AddBufGear(i+6,g_pFrm->m_CmdRun.V9Parm[i].DispDistance-dForwardDis[i]);
						}
						else
						{
							theApp.m_Mv400.AddBufGear(i+6,g_pFrm->m_CmdRun.V9Parm[i].DispDistance);
						}
					}
				}
			}
			if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
			{
				g_pFrm->m_CmdRun.m_bV9FirstDisp = false;
			}
			V9_RunCount++;
			int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
				{
					nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
				}
			}
			if(V9_RunCount>=nCount)
			{
				return 1;
			}
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		if(axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				dForwardDis[V9ID] = g_pFrm->m_CmdRun.V9Parm[V9ID].ForwardVol/theApp.m_tSysParam.Dispratios;
				if(bForward)
				{
					theApp.m_Mv400.AddBufGear(axis,-(g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance-dForwardDis[V9ID]));
				}
				else
				{
					theApp.m_Mv400.AddBufGear(axis,-g_pFrm->m_CmdRun.V9Parm[V9ID].DispDistance);
				}
				V9_RunCount++;
				if(V9_RunCount>=g_pFrm->m_CmdRun.V9Parm[V9ID].DispNum)
				{
					return 1;
				}
			}
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					dForwardDis[i] = g_pFrm->m_CmdRun.V9Parm[i].ForwardVol/theApp.m_tSysParam.Dispratios;
					if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
					{
						double dFirstComp = g_pFrm->m_CmdRun.V9Parm[i].FirstCompVol/theApp.m_tSysParam.Dispratios;
						if(bForward)
						{
							theApp.m_Mv400.AddBufGear(i+6,-(g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp-dForwardDis[i]));
						}
						else
						{
							theApp.m_Mv400.AddBufGear(i+6,-(g_pFrm->m_CmdRun.V9Parm[i].DispDistance+dFirstComp));
						}
					}
					else
					{
						if(bForward)
						{
							theApp.m_Mv400.AddBufGear(i+6,-(g_pFrm->m_CmdRun.V9Parm[i].DispDistance-dForwardDis[i]));
						}
						else
						{
							theApp.m_Mv400.AddBufGear(i+6,-g_pFrm->m_CmdRun.V9Parm[i].DispDistance);
						}
					}
				}
			}
			if(g_pFrm->m_CmdRun.m_bV9FirstDisp)
			{
				g_pFrm->m_CmdRun.m_bV9FirstDisp = false;
			}
			V9_RunCount++;
			int nCount = g_pFrm->m_CmdRun.V9Parm[0].DispNum;
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if(nCount>g_pFrm->m_CmdRun.V9Parm[i].DispNum)
				{
					nCount = g_pFrm->m_CmdRun.V9Parm[i].DispNum;
				}
			}
			if(V9_RunCount>=nCount)
			{
				return 1;
			}
		}
	}
	return 0;
}

void CV9Control::V9C_Dot(int axis,double dVolumn)
{
	double dTempPos,dDistance;
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		if (axis>0)
		{
			dTempPos=theApp.m_Mv400.GetPos(axis);
			dDistance=dVolumn/theApp.m_tSysParam.Dispratios;
			if(dDistance+dTempPos>theApp.m_tSysParam.DisEndDistance)
			{
				V9C_Rest(axis);
				V9C_SetProfile();
				dTempPos = theApp.m_Mv400.GetPos(axis);
			}
			theApp.m_Mv400.Move(axis,dDistance+dTempPos,true,true);
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		if (axis>0)
		{
			dTempPos=theApp.m_Mv400.GetPos(axis);
			dDistance=dVolumn/theApp.m_tSysParam.Dispratios;
			if(dTempPos-dDistance<0)
			{
				V9C_Rest(axis);
				V9C_SetProfile();
				dTempPos = theApp.m_Mv400.GetPos(axis);
			}
			theApp.m_Mv400.Move(axis,dTempPos-dDistance,true,true);
		}
	}
}

void CV9Control::V9C_Dot_Forward(int axis)
{
	int V9ID=axis-6;
	bool selectID=FALSE;
	double Tempost;
	double dForwardDis[3];
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				Tempost=theApp.m_Mv400.GetPos(axis);
				dForwardDis[V9ID]=g_pFrm->m_CmdRun.V9Parm[V9ID].ForwardVol/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(axis,dForwardDis[V9ID]+Tempost,true,true);
			}

		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=TRUE;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					dForwardDis[i] = g_pFrm->m_CmdRun.V9Parm[i].ForwardVol/theApp.m_tSysParam.Dispratios;
					theApp.m_Mv400.Move(i+6,Tempost+dForwardDis[i],false,true);
				}
			}
			//theApp.m_Mv400.WaitValvesStop();
			V9C_WaitStop();
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		if (axis>0)
		{
			if(V9ID<(int)theApp.m_tSysParam.valveNum)
			{
				Tempost=theApp.m_Mv400.GetPos(axis);
				dForwardDis[V9ID]=g_pFrm->m_CmdRun.V9Parm[V9ID].ForwardVol/theApp.m_tSysParam.Dispratios;
				theApp.m_Mv400.Move(axis,Tempost-dForwardDis[V9ID],true,true);
			}

		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=TRUE;
			}
			for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
			{
				if (g_pFrm->m_CmdRun.RunDispFSelect[i])
				{
					Tempost=theApp.m_Mv400.GetPos(i+6);
					dForwardDis[i] = g_pFrm->m_CmdRun.V9Parm[i].ForwardVol/theApp.m_tSysParam.Dispratios;
					theApp.m_Mv400.Move(i+6,Tempost-dForwardDis[i],false,true);//计算绝对位置
				}
			}
			//theApp.m_Mv400.WaitValvesStop();
			V9C_WaitStop();
		}
	}
}

bool CV9Control::V9C_SetProfile()
{
	MOTION_STATUS status;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		status = theApp.m_Mv400.SetProfile(K_AXIS_U+i,g_pFrm->m_CmdRun.V9Parm[i].dispspeed,
			theApp.m_tSysParam.DispMaxAcc,theApp.m_tSysParam.DispMaxAcc,0,g_pFrm->m_CmdRun.V9Parm[i].StartVel);
		if(MOT_STATUS_OK!=status)
		{
			return false;
		}
	}
	CString str;
	double dCurrentVel = 0.0;
	double dCurrentAcc = 0.0;
	double dCurrentDec = 0.0;
	double dStartVel = 0.0;
	int nSmoothTime = 0;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		status = theApp.m_Mv400.GetProfile(K_AXIS_U+i,&dCurrentVel,&dCurrentAcc,&dCurrentDec,&dStartVel,&nSmoothTime);
		if(MOT_STATUS_OK!=status)
		{
			str.Format("Valve %d:vel=%0.1f,acc=%0.1f,dec=%0.1f,startVel=%0.1f,smoothTime=%d",
				i+1,dCurrentVel,dCurrentAcc,dCurrentDec,dStartVel,nSmoothTime);
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			return false;
		}
	}
	return true;
}

bool CV9Control::V9C_WaitStop()
{
	MOTION_STATUS status;
	for(int i=0;i<(int)theApp.m_tSysParam.valveNum;i++)
	{
		status = theApp.m_Mv400.WaitStop(K_AXIS_U+i,10000);
		if(MOT_STATUS_OK != status)
		{
			return false;
		}
	}
	return true;
}

int CV9Control::V9C_Defoam(int axis)
{
	if((VALVE_M9 == theApp.m_tSysParam.ValveType)||(VALVE_L9 == theApp.m_tSysParam.ValveType))
	{
		int nRet = V9C_Home(axis);
		if(nRet!=0)
		{
			return nRet;
		}
		int V9ID=axis-6;
		bool selectID=false;
		if(axis>0)
		{
			if(V9ID<theApp.m_tSysParam.valveNum)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],TRUE);
			}
		}
		else
		{
			if(axis == K_SELECT_AXIS)
			{
				selectID = false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID = true;
			}
			for(int i=0;i<theApp.m_tSysParam.valveNum;i++)
			{
				if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);
				}
			}
		}
	}
	else if(VALVE_H9 == theApp.m_tSysParam.ValveType)
	{
		int V9ID=axis-6;
		bool selectID=false;
		if (axis>0)
		{
			if(V9ID<3)
			{
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[V9ID],FALSE);
				if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[V9ID]))
				{
					return V9ID+1;//关阀失败
				}
				theApp.m_Mv400.MoveDot(axis,theApp.m_tSysParam.DisEndDistance,g_pFrm->m_CmdRun.V9Parm[V9ID].FullSpeed,theApp.m_tSysParam.DispMaxAcc,true);
				//theApp.m_Mv400.Move(axis,theApp.m_tSysParam.DisEndDistance,true,true);
				theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[V9ID],TRUE);
			}
			return 0;
		}
		else
		{
			if (axis==K_SELECT_AXIS)
			{
				selectID=false; 
			}
			else if (axis==K_ALL_AXIS)
			{  
				selectID=true;
			}
			for(int i=0;i<theApp.m_tSysParam.valveNum;i++)
			{
				if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outValveOpen[i],FALSE);
					if (!CheckSensor(theApp.m_tSysParam.inValveCloseSen[i]))
					{
						return V9ID+1;//关阀失败
					}
					//theApp.m_Mv400.Move(i+4,theApp.m_tSysParam.DisEndDistance,true,true);
					theApp.m_Mv400.MoveDot(i+6,theApp.m_tSysParam.DisEndDistance,g_pFrm->m_CmdRun.V9Parm[i].FullSpeed,theApp.m_tSysParam.DispMaxAcc,true);
				}
			}
			for(int i=0;i<theApp.m_tSysParam.valveNum;i++)
			{
				if ((g_pFrm->m_CmdRun.RunDispFSelect[i]&&g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&!selectID)||(g_pFrm->m_CmdRun.ProductParam.FSelectVal[i]&&selectID))
				{
					theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outOpenPress[i],TRUE);
				}
			}
		}
	}
	return 0;
}