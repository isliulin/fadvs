// DlgRepair.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgRepair.h"


// CDlgRepair dialog

IMPLEMENT_DYNAMIC(CDlgRepair, CDialog)

CDlgRepair::CDlgRepair(CWnd* pParent /*=NULL*/)
: CDialog(CDlgRepair::IDD, pParent)
{
	m_pBtn = NULL;
	m_bDrag = false;
	m_bIsFirstDraw = true;
	m_nSelIndex = -1;
}

CDlgRepair::~CDlgRepair()
{
	if(m_pBtn!=NULL)
	{
		delete[] m_pBtn;
		m_pBtn = NULL;
	}
}

void CDlgRepair::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRepair, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BTN_SELECT_DISP, &CDlgRepair::OnBnClickedBtnSelectDisp)
	ON_BN_CLICKED(IDC_BTN_ALL_DISP, &CDlgRepair::OnBnClickedBtnAllDisp)
	ON_BN_CLICKED(IDC_BTN_SELECT_REGION, &CDlgRepair::OnBnClickedBtnSelectRegion)
	ON_BN_CLICKED(IDOK, &CDlgRepair::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MOVE_TO, &CDlgRepair::OnBnClickedBtnMoveTo)
	ON_BN_CLICKED(IDCANCEL, &CDlgRepair::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SAFETY_POS, &CDlgRepair::OnBnClickedBtnSafetyPos)
	ON_BN_CLICKED(IDC_BTN_TARGETPOS, &CDlgRepair::OnBnClickedBtnTargetpos)
END_MESSAGE_MAP()


// CDlgRepair message handlers

BOOL CDlgRepair::OnInitDialog()
{
	CDialog::OnInitDialog();
	int nIndex = 0;
	int num = 0;
	CString strText;
	int left,right,top,buttom;
	double dRatio = 0.0;
	if(g_pFrm->m_CmdRun.ProductParam.IrregularMatrix)
	{
		nButtonNum = g_pFrm->m_CmdRun.ProductParam.IrregularRow*g_pFrm->m_CmdRun.ProductParam.IrregularColumn;
		CRect rect;
		CString strText;
		GetWindowRect(&rect);
		dRatio = (g_pFrm->m_CmdRun.ProductParam.IrregularColumn*(g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[0]+25))/(g_pFrm->m_CmdRun.ProductParam.IrregularColumn*25);
		long nWidth = long(g_pFrm->m_CmdRun.ProductParam.IrregularColumn*25*dRatio+40);
		//int nHeight = g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*20+180;
		rect.right = (LONG)nWidth;
		//rect.bottom = (LONG)nHeight;
		//SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE);
		MoveWindow(&rect);
		GetClientRect(&rect);
		double dScaleX = (rect.Width()-40)/(g_pFrm->m_CmdRun.ProductParam.IrregularColumn*(g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[0]+25));
		double dScaleY = (rect.Height()-120)/(g_pFrm->m_CmdRun.ProductParam.IrregularRow*(g_pFrm->m_CmdRun.ProductParam.IrregularRowD[0]+5));
		//int nRowDistance = (rect.Height()-120)/g_pFrm->m_CmdRun.ProductParam.SecondDispRow;
		//int nColumnDistance = (rect.Width()-40)/g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
		nButtonID = 10000;
		if(m_pBtn!=NULL)
		{
			delete[] m_pBtn;
			m_pBtn = NULL;
		}
		m_pBtn = new CXPButton[nButtonNum];
		for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.IrregularRow;i++)
		{
			for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.IrregularColumn;j++)
			{
				nIndex=j+i*g_pFrm->m_CmdRun.ProductParam.IrregularColumn;
				num = j+i*g_pFrm->m_CmdRun.ProductParam.IrregularColumn;
				left = int(rect.right-10-((g_pFrm->m_CmdRun.ProductParam.IrregularColumnD[0]+25)*j)*dScaleX);
				right = int(left+25*dScaleX);
				top = int(10+((g_pFrm->m_CmdRun.ProductParam.IrregularRowD[0]+5)*i)*dScaleY);
				buttom = int(top+3*dScaleY);
				strText.Format("%d",num);
				m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
				//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);

				m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
				m_pBtn[nIndex].Set_BackColor(RGB(61,133,184));
				m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
				m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
				m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));
			}
		}
	}
	else
	{
		nButtonNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*
			g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.FirstDispRow;
		CRect rect;
		GetWindowRect(&rect);
		dRatio = ((g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*25)+(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD))/
			(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*25);
		long nWidth = long(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*25*dRatio+40);
		//int nHeight = g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*20+180;
		rect.right = nWidth;
		//rect.bottom = (LONG)nHeight;
		//SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE);
		MoveWindow(&rect);
		GetClientRect(&rect);
		double dScaleX = (rect.Width()-40)/(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD+
			g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*25);
		double dScaleY = (rect.Height()-120)/(g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD+
			g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*5);
		//int nRowDistance = (rect.Height()-120)/g_pFrm->m_CmdRun.ProductParam.SecondDispRow;
		//int nColumnDistance = (rect.Width()-40)/g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
		nButtonID = 10000;
		if(m_pBtn!=NULL)
		{
			delete[] m_pBtn;
			m_pBtn = NULL;
		}
		m_pBtn = new CXPButton[nButtonNum];
		//for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;i++)
		//{
		//	for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.SecondDispRow;j++)
		//	{
		//		num = i*g_pFrm->m_CmdRun.ProductParam.SecondDispRow + j;
		//		strText.Format("%d",num);
		//		btn[num].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(20+nColumnDistance*i,20+nRowDistance*j,20+nColumnDistance*(i+1),20+nRowDistance*(j+1)),this,nButtonID+num);
		//	}
		//}
		int thirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
		int secondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
		for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.FirstDispRow;i++)
		{
			for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.FirstDispColumn;j++)
			{
				for(int m=0;m<g_pFrm->m_CmdRun.ProductParam.SecondDispRow;m++)
				{
					for(int n=0;n<g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;n++)
					{
						for(int k=0;k<g_pFrm->m_CmdRun.ProductParam.ThirdDispRow;k++)
						{
							for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)
							{
								nIndex=l+k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+n*thirdNum+m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*thirdNum+j*secondNum*thirdNum+i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*secondNum*thirdNum;
								num = l+n*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+
									k*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+
									m*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*thirdNum +
									i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*secondNum*thirdNum;
								left = int(rect.right-10-(g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD*j+g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD*n+g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD*l+
									(j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+n*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+l+1)*25)*dScaleX);
								right = int(left+25*dScaleX);
								top = int(10+(g_pFrm->m_CmdRun.ProductParam.FirstDispRowD*i+g_pFrm->m_CmdRun.ProductParam.SecondDispRowD*m+g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD*k+
									(i*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow+m*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow+k)*5)*dScaleY);
								buttom = int(top+5*dScaleY);
								///////////////////////////////////////////////
								if (THREE_VALVE==g_pFrm->m_CmdRun.ProductParam.valveSelect)
								{
									bool bInRectA,bInRectB,bInRectC;
									bInRectA=bInRectB=bInRectC=false;
									int nLastPartNum=0;

									if(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn>1)
									{
										int nNum=(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn+2)/3;
										nLastPartNum=g_pFrm->m_CmdRun.ProductParam.SecondDispColumn-2*nNum;

										if ((n>=0)&&(n<nNum))
										{
											bInRectA=true;
											bInRectB=false;
											bInRectC=false;
										}
										else if((n>=nNum)&&(n<2*nNum)) 
										{
											bInRectA=false;
											bInRectB=true;
											bInRectC=false;
										}
										else
										{
											bInRectA=false;
											bInRectB=false;
											bInRectC=true;
										}

									}
									else
									{
										int nNum=(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+2)/3;
										nLastPartNum=g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-2*nNum;

										if ((l>=0)&&(l<nNum))
										{
											bInRectA=true;
											bInRectB=false;
											bInRectC=false;
										}
										else if((l>=nNum)&&(l<2*nNum)) 
										{
											bInRectA=false;
											bInRectB=true;
											bInRectC=false;
										}
										else
										{
											bInRectA=false;
											bInRectB=false;
											bInRectC=true;
										}

									}
									if((bInRectA)&&(!bInRectB)&&(!bInRectC))
									{
										strText.Format("%d",num);
										m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
										//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
										m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
										m_pBtn[nIndex].Set_BackColor(RGB(128,0,0));
										m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
										m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
										m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));
										if(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn>1)
										{
											if((n>=nLastPartNum)&&m_bDeviceAdjust)
											{
												m_pBtn[nIndex].EnableWindow(false);
											}
										}
										else
										{
											if((l>=nLastPartNum)&&m_bDeviceAdjust)
											{
												m_pBtn[nIndex].EnableWindow(false);
											}
										}

									}
									else if ((!bInRectA)&&(bInRectB)&&(!bInRectC))
									{
										strText.Format("%d",num);
										m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
										//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
										m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
										m_pBtn[nIndex].Set_BackColor(RGB(0,128,0));
										m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
										m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
										m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));
										if(m_bDeviceAdjust)
										{
											m_pBtn[nIndex].EnableWindow(false);
										}
									}
									else if((!bInRectA)&&(!bInRectB)&&(bInRectC))
									{
										strText.Format("%d",num);
										m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
										//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
										m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
										m_pBtn[nIndex].Set_BackColor(RGB(0,0,128));
										m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
										m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
										m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));
										if(m_bDeviceAdjust)
										{
                                          m_pBtn[nIndex].EnableWindow(false);
										}
									}

								}
								else if (TWO_VALVE==g_pFrm->m_CmdRun.ProductParam.valveSelect)
								{
									bool bInRectA,bInRectB;
									bInRectA=bInRectB=false;
									if (g_pFrm->m_CmdRun.ProductParam.SecondDispColumn>1)
									{
										int nNum=(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn+1)/2;
										if ((n>=0)&&(n<nNum))
										{
											bInRectA=true;
											bInRectB=false;
										}
										else
										{
											bInRectA=false;
											bInRectB=true;
										}

									}
									else
									{
										int nNum=(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+1)/2;
										if ((l>=0)&&(l<nNum))
										{
											bInRectA=true;
											bInRectB=false;
										}
										else
										{
											bInRectA=false;
											bInRectB=true;
										}
									}
									if (bInRectA&&(!bInRectB))
									{
										strText.Format("%d",num);
										m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
										//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
										m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
										m_pBtn[nIndex].Set_BackColor(RGB(128,0,0));
										m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
										m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
										m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));
									}
									else if ((!bInRectA)&&bInRectB)
									{
										strText.Format("%d",num);
										m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
										//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
										m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
										m_pBtn[nIndex].Set_BackColor(RGB(0,128,0));
										m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
										m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
										m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));

									}

								}
								else
								{
									strText.Format("%d",num);
									m_pBtn[nIndex].Create(strText,WS_CHILD|BS_DEFPUSHBUTTON|WS_VISIBLE|BS_AUTOCHECKBOX,CRect(left,top,right,buttom),this,nButtonID+nIndex);
									//m_pBtn[nIndex].ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
									m_pBtn[nIndex].Set_BackColorSel(RGB(41,88,122));
									m_pBtn[nIndex].Set_BackColor(RGB(61,133,184));
									m_pBtn[nIndex].Set_TextColor(RGB(255,255,255));
									m_pBtn[nIndex].Set_BackColorSel(RGB(255,202,8));
									m_pBtn[nIndex].SetFont(80,_T("微软雅黑"));
								}

							}
						}
					}
				}
			}
		}
	}
	strText.Format("0");
	SetDlgItemText(IDC_EDIT_START_INDEX,strText);
	SetDlgItemText(IDC_EDIT_END_INDEX,strText);
	strText.Format("%d",m_nSelIndex);
	SetDlgItemText(IDC_EDIT_CURR_INDEX,strText);
	GetDlgItem(IDC_BTN_TARGETPOS)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL);
	if((m_bDeviceAdjust)/*&&(theApp.m_SysUser.m_CurUD.level>GM_PERSONNEL)*/)
	{
		GetDlgItem(IDC_BTN_TARGETPOS)->EnableWindow(true);
		GetDlgItem(IDC_BTN_MOVE_TO)->EnableWindow(false);  //调试禁用此按钮
		GetDlgItem(IDC_BTN_SELECT_DISP)->EnableWindow(false); //调试禁用此按钮
        GetDlgItem(IDC_BTN_ALL_DISP)->EnableWindow(false); //调试禁用此按钮
	}
	else
	{
        GetDlgItem(IDC_BTN_TARGETPOS)->EnableWindow(false);
	}
	return TRUE;
}



void CDlgRepair::OnBnClickedBtnAllDisp()
{
	// TODO: Add your control notification handler code here
	SetAuthority(false);
	g_pFrm->m_CmdRun.Run();
	SetAuthority(true);
}

void CDlgRepair::OnBnClickedBtnSelectRegion()
{
	// TODO: Add your control notification handler code here
	int nStartIndex = 0;
	int nEndIndex = 0;
	int nIndex = 0;
	int num = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_START_INDEX,str);
	nStartIndex = atoi(str);
	GetDlgItemText(IDC_EDIT_END_INDEX,str);
	nEndIndex = atoi(str);
	if(g_pFrm->m_CmdRun.ProductParam.IrregularMatrix)
	{
		if(nStartIndex<=nEndIndex&&nStartIndex>=0&&nEndIndex<nButtonNum)
		{
			for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.IrregularRow;i++)
			{
				for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.IrregularColumn;j++)
				{
					num = j+i*g_pFrm->m_CmdRun.ProductParam.IrregularColumn;
					nIndex = j+i*g_pFrm->m_CmdRun.ProductParam.IrregularColumn;
					if(num>=nStartIndex&&num<=nEndIndex)
					{
						m_pBtn[nIndex].SetCheck(1);
					}
					else
					{
						m_pBtn[nIndex].SetCheck(0);
					}
				}
			}
		}
	}
	else
	{
		int thirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
		int secondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
		if(nStartIndex<=nEndIndex&&nStartIndex>=0&&nEndIndex<nButtonNum)
		{
			for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.FirstDispRow;i++)
			{
				for(int j=0;j<g_pFrm->m_CmdRun.ProductParam.FirstDispColumn;j++)
				{
					for(int m=0;m<g_pFrm->m_CmdRun.ProductParam.SecondDispRow;m++)
					{
						for(int n=0;n<g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;n++)
						{
							for(int k=0;k<g_pFrm->m_CmdRun.ProductParam.ThirdDispRow;k++)
							{
								for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)
								{
									nIndex=l+k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+n*thirdNum+m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*thirdNum+j*secondNum*thirdNum+i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*secondNum*thirdNum;
									num = l+n*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+j*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+
										k*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+
										m*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*thirdNum +
										i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*secondNum*thirdNum;
									if(num>=nStartIndex&&num<=nEndIndex)
									{
										m_pBtn[nIndex].SetCheck(1);
									}
									else
									{
										m_pBtn[nIndex].SetCheck(0);
									}
								}
							}
						}
					}
				}
			}
		}
		//for(int i=0;i<nButtonNum;i++)
		//{
		//	if(i>=nStartIndex&&i<=nEndIndex)
		//	{
		//		btn[i].SetCheck(1);
		//	}
		//	else
		//	{
		//		btn[i].SetCheck(0);
		//	}
		//}
	}
}

void CDlgRepair::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDrag = true;
	m_ptStart = point;
	CDialog::OnLButtonDown(nFlags,point);
}

void CDlgRepair::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDrag)
	{
		CRect rcDrag = GetARect(m_ptStart,point);
		if(m_bIsFirstDraw)
		{
			m_bIsFirstDraw = false;
			this->GetDC()->DrawDragRect(&rcDrag,CSize(1,1),NULL,CSize(1,1),NULL,NULL);
		}
		else
		{
			this->GetDC()->DrawDragRect(&rcDrag,CSize(1,1),&this->m_rcPreRect,CSize(1,1),NULL,NULL);
		}
		this->m_rcPreRect.CopyRect(&rcDrag);
	}
	CDialog::OnMouseMove(nFlags,point);
}

void CDlgRepair::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = false;
	m_bIsFirstDraw = true;
	CRect rcDrag = GetARect(m_ptStart,point);
	for(int i=0;i<nButtonNum;i++)
	{
		if(IsInRect(10000+i,rcDrag))
		{
			if(m_pBtn[i].GetCheck())
			{
				m_pBtn[i].SetCheck(0);
			}
			else
			{
				m_pBtn[i].SetCheck(1);
			}
		}
	}
	this->Invalidate(TRUE);
	CDialog::OnLButtonUp(nFlags,point);
}

bool CDlgRepair::IsInRect(UINT nID,CRect rc)
{
	bool bRet = false;
	CRect rcWnd;
	GetDlgItem(nID)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);

	CPoint pt[4];
	pt[0].x = rcWnd.left;
	pt[0].y = rcWnd.top;
	pt[1].x = rcWnd.right;
	pt[1].y = rcWnd.top;
	pt[2].x = rcWnd.left;
	pt[2].y = rcWnd.bottom;
	pt[3].x = rcWnd.right;
	pt[3].y = rcWnd.bottom;

	for(int i=0;i<4;i++)
	{
		if(rc.PtInRect(pt[i]))
		{
			bRet = true;
			break;
		}
	}
	return bRet;
}

CRect CDlgRepair::GetARect(CPoint ptStart,CPoint ptEnd)
{
	CRect rc;
	rc.left = (ptStart.x < ptEnd.x)?ptStart.x:ptEnd.x;
	rc.top = (ptStart.y < ptEnd.y)?ptStart.y:ptEnd.y;
	rc.right = (ptStart.x < ptEnd.x)?ptEnd.x:ptStart.x;
	rc.bottom = (ptStart.y < ptEnd.y)?ptEnd.y:ptStart.y;
	return rc;
}

void CDlgRepair::OnPaint()
{
	//if(IsIconic())
	//{
	//	CPaintDC dc(this);
	//	SendMessage(WM_ICONERASEBKGND,reinterpret_cast<WPARAM>(dc.GetSafeHdc()),0);
	//	int cxIcon = GetSystemMetrics(SM_CXICON);
	//	int cyIcon = GetSystemMetrics(SM_CYICON);
	//	CRect rect;
	//	GetClientRect(&rect);
	//	int x = (rect.Width()-cxIcon+1)/2;
	//	int y = (rect.Height()-cyIcon+1)/2;
	//	dc.DrawIcon(x,y,m_)
	//}
	CDialog::OnPaint();
}
void CDlgRepair::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnOK();
}
void CDlgRepair::SetAuthority(bool bEnable)
{
	GetDlgItem(IDC_EDIT_START_INDEX)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_END_INDEX)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_SELECT_REGION)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_SELECT_DISP)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_ALL_DISP)->EnableWindow(bEnable);
}
void CDlgRepair::OnBnClickedBtnMoveTo()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_CURR_INDEX,str);
	m_nSelIndex = atoi(str);
	if(m_nSelIndex<0)
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return;
	}
	int nThirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow;
	int nSecondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispRow;
	int nFirstNum = g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.FirstDispRow;
	int nFirstRow,nFirstCol,nSecondRow,nSecondCol,nThirdRow,nThirdCol;
	nThirdCol = m_nSelIndex%g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	nThirdRow = ((m_nSelIndex - nThirdCol)%nThirdNum)/g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	nSecondCol = (m_nSelIndex/nThirdNum)%g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
	nSecondRow = m_nSelIndex/(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum)%g_pFrm->m_CmdRun.ProductParam.SecondDispRow;
	nFirstCol = m_nSelIndex/(nSecondNum*nThirdNum)%g_pFrm->m_CmdRun.ProductParam.FirstDispColumn;
	nFirstRow = m_nSelIndex/(g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum);
	while(nThirdCol>=(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+2)/3)
	{
		nThirdCol = nThirdCol - (g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+2)/3;
	}
	tgPos tgCurrPos = g_pFrm->m_CmdRun.GetPadDispensePos(nFirstRow,nFirstCol,nSecondRow,nSecondCol,nThirdRow,nThirdCol);
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgCurrPos.x,tgCurrPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,true))
	{
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgCurrPos.za,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,true))
	{
		return;
	}
}

void CDlgRepair::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnCancel();
}

void CDlgRepair::OnBnClickedBtnSafetyPos()
{
	// TODO: Add your control notification handler code here
	g_pFrm->m_CmdRun.MoveToZSafety();
}


void CDlgRepair::OnBnClickedBtnSelectDisp()
{
	// TODO: Add your control notification handler code here
	SetAuthority(false);
	CString str;
	bool *bIsDispense = new bool[nButtonNum];
	for(int i=0;i<nButtonNum;i++)
	{
		bIsDispense[i] = false;
		if(m_pBtn[i].GetCheck())
		{
			bIsDispense[i] = true;
			str.Format("bIsDispense[%d]:true",i);
			g_pFrm->m_CmdRun.AddMsg(str);
		}
	}
	g_pFrm->m_CmdRun.SelectDispense(bIsDispense);
	delete[] bIsDispense;
	bIsDispense = NULL;
	SetAuthority(true);
}
void CDlgRepair::OnBnClickedBtnTargetpos()
{
	CString str;
	int targetNo=-1;;
	double dx,dy,dz;

	str.Format("到选择点被触发...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	SetAuthority(false);
	for(int i=0;i<nButtonNum;i++)
	{
		if(m_pBtn[i].GetCheck())
		{
			targetNo=i;
			break;
		}
	}
	if(!g_pFrm->m_CmdRun.TargetISZoneA(targetNo))
	{
		AfxMessageBox("目标点仅限于阀A区域...");
		return;
	}

	if(targetNo<0)
	{
		return;
	}
	g_pFrm->m_CmdRun.PosInit();
	dx=g_pFrm->m_CmdRun.PDpostingBug[targetNo].Dx;
	dy=g_pFrm->m_CmdRun.PDpostingBug[targetNo].Dy;
	dz=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2];
	g_pFrm->m_CmdRun.MoveToZSafety();
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(dx,dy,true))
	{
		return ;
	}
	if(!g_pFrm->m_CmdRun.AdjustNeedle())
	{
		AfxMessageBox("调整电机移动失败！");
		return;
	}
	double dza,dzb,dzc;
	dza=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[2];
    dzb=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[3];
	dzc=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[4];
	g_pFrm->m_CmdRun.SynchronizeMoveZ(dza,dzb,dzc,false,true);
	SetAuthority(true);
}

void CDlgRepair::RuleDeviceAdjust(bool bAdjust)
{
	m_bDeviceAdjust=bAdjust;
}