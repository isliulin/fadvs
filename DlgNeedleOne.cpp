// DlgNeedleOne.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgNeedleOne.h"
#include "DlgNeedleTwo.h"
#include "DlgNeedleCamera.h"
#include "Function.h"
#include <math.h>


// CDlgNeedleOne dialog

IMPLEMENT_DYNAMIC(CDlgNeedleOne, CDialog)

CDlgNeedleOne::CDlgNeedleOne(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNeedleOne::IDD, pParent)
{
	m_nImageWidth = milApp.m_lNeedleBufSizeX;
	m_nImageHeight = milApp.m_lNeedleBufSizeY;

}

CDlgNeedleOne::~CDlgNeedleOne()
{
}

void CDlgNeedleOne::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_NEEDLEONE, m_picture);
	DDX_Control(pDX, IDC_SLIDER_NEEDLE_EXPOSURE_TIME, m_sliderShutter);
	DDX_Control(pDX, IDC_SLIDER_NEEDLE_GAIN, m_sliderGain);
}


BEGIN_MESSAGE_MAP(CDlgNeedleOne, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNeedleOne::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MOVETOCENTER_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnMovetocenterNdleone)
	ON_BN_CLICKED(IDC_BTN_LEARNMODEL_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnLearnmodelNdleone)
	ON_BN_CLICKED(IDC_BTN_COMPUTESCALE_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnComputescaleNdleone)
	ON_BN_CLICKED(IDC_BTN_SEARCHMODEL_NDLEONE, &CDlgNeedleOne::OnBnClickedBtnSearchmodelNdleone)
	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_NEEDLE_SPEED, &CDlgNeedleOne::OnNMCustomdrawSliderNeedleSpeed)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_NEEDLE_ACCEPTANCE, &CDlgNeedleOne::OnEnChangeEditNeedleAcceptance)
	ON_EN_CHANGE(IDC_EDIT_NEEDLE_ANGLE, &CDlgNeedleOne::OnEnChangeEditNeedleAngle)
	ON_BN_CLICKED(IDCANCEL, &CDlgNeedleOne::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgNeedleOne message handlers

void CDlgNeedleOne::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(g_pFrm->m_CmdRun.m_tStatus == K_RUN_STS_RUN)
	{
		g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
		AfxMessageBox("�˶�״̬���ܱ��浱ǰλ�ã�");
		return;
	}
	double dXOffset,dXNewPos;                                            //////2017-12-08
	double dYOffset,dYNewPos;                                           //////
	dXOffset=dYOffset=0;                                               //////
	dXNewPos=theApp.m_Mv400.GetPos(K_AXIS_X);                         //////
	dXOffset=dXNewPos-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x;  //////
	dYNewPos=theApp.m_Mv400.GetPos(K_AXIS_Y);                        //////
	dYOffset=dYNewPos-g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y; //////

    g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x=theApp.m_Mv400.GetPos(K_AXIS_X);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za=theApp.m_Mv400.GetPos(K_AXIS_ZA);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zb=theApp.m_Mv400.GetPos(K_AXIS_ZB);
	g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].zc=theApp.m_Mv400.GetPos(K_AXIS_ZC);
	///////////////////////////////////////////////////////////////////////////////////////////2017-12-08
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0]+=dXOffset; //�Զ���������Xƫ��ֵ     //��λ1
	theApp.m_tSysParam.CleanPosition[0]+=dXOffset;                                         //��λ2
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                         //��λ3
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].x+=dXOffset;
	}
	theApp.m_tSysParam.DischargeGluePostion[0]+=dXOffset;                                  //��λ4
	////
	g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1]+=dYOffset; //�Զ���������Yƫ��ֵ     //��λ1
	theApp.m_tSysParam.CleanPosition[1]+=dYOffset;                                         //��λ2
	for(int i=0;i<g_pFrm->m_CmdRun.ProductParam.TestDotNumber;i++)                         //��λ3
	{
		g_pFrm->m_CmdRun.ProductParam.tgTestDispPos[i].y+=dYOffset;
	}
	theApp.m_tSysParam.DischargeGluePostion[1]+=dYOffset;      
	////////////////////////////////////////////////////////////////////////////////////////////2017-12-08
	theApp.ProductParam(FALSE);
	CString str;
	str.Format("Needle 1 Z Value at last:%0.3f",g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].za);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		str.Format("�ֶ��궨��ͷ1���ʱXYZ�л�������ʱʧ�ܣ�");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		AfxMessageBox("���õ����������");
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	OnOK();
	CDlgNeedleTwo dlg;
	dlg.DoModal();
}

BOOL CDlgNeedleOne::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	for(int i=0;i<12;i++)
	{
		m_adjustButton[i].m_iIndex = i;
		m_adjustButton[i].SubclassDlgItem(IDC_BTN_NEEDLEONE_X_NEG_NDLEONE+i,this);
		//m_adjustButton[i].SetBitmapEx(IDB_BITMAP_ADJUST);
		//m_adjustButton[i].m_pMv = &theApp.m_Mv400;
	}

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;

	m_sliderShutter.SetRange(0,999);
	m_sliderGain.SetRange(0,63);
	m_sliderShutter.SetPos(theApp.m_tSysParam.NeedleCameraExposure);
	m_sliderGain.SetPos(theApp.m_tSysParam.NeedleCameraGain);
	g_pView->m_ImgStatic.m_pCameraNeedle.SetExposureTime(theApp.m_tSysParam.NeedleCameraExposure);
    g_pView->m_ImgStatic.m_pCameraNeedle.SetGain(theApp.m_tSysParam.NeedleCameraGain);
	g_pView->m_ImgStatic.ImgNeedleLive();

	str.Format("%0.3f",theApp.m_tSysParam.BmpNeedleMarkAcceptance);
	SetDlgItemText(IDC_EDIT_NEEDLE_ACCEPTANCE,str);
	str.Format("%0.3f",theApp.m_tSysParam.BmpNeedleMarkAngle);
	SetDlgItemText(IDC_EDIT_NEEDLE_ANGLE,str);

	//start the camera to grab:the camera of refpoint;
	//not pointer ,can not judge like detect camera;
	
	m_rectModelWin=theApp.m_tSysParam.BmpNeedleLearnWin;
	m_rectROI=theApp.m_tSysParam.BmpNeedleROI;
	m_picture.m_modtype = MOD_RECTANGLE;
	m_picture.m_iImgWidth = m_nImageWidth;
	m_picture.m_iImgHeight = m_nImageHeight;
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight*3];
	m_picture.m_mod_rectangle.dx = m_rectModelWin.TopLeft().x + m_rectModelWin.Width()/2;
	m_picture.m_mod_rectangle.dy = m_rectModelWin.TopLeft().y + m_rectModelWin.Height()/2;
	m_picture.m_mod_rectangle.width = m_rectModelWin.Width();
	m_picture.m_mod_rectangle.height = m_rectModelWin.Height();//ͼƬ��MARK���С����
	m_picture.m_mod_scale.length = 10;//20 -10
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;//ͼƬ��ROI���С����
	m_picture.SetDisplayMarkImage(false);
	m_picture.SetDisplayNeedleImage(true);/////////////////////////////////
	m_picture.yOffset=0;
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,24);//the distingwish of 8 and 24 in camera;

	//image process set
	CString strFile = g_pDoc->GetPadModelFile(11);
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);  //0630 patmatch
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	str.Format("���ֶ��ƶ�XYZ�Ὣ1#��ͷ�ƶ���������ģ�");
	SetDlgItemText(IDC_EDIT_PROMPT,str);

	//��Դ���ƣ�
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,TRUE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
    ::RegisterHotKey(this->GetSafeHwnd(),2001,MOD_ALT,VK_F4);
   
	//��ʼ����֮ǰ��ABCD �س�ʼ�㣺
	if(!g_pFrm->m_CmdRun.AdjustMotorHome())
	{
		str.Format("����������ʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		AfxMessageBox("������︴λʧ�ܣ������ų����������ϣ�");
	}
    //����Ԥ��3mm�ռ䣬�����Զ�����ʱ��ͷ2����ͷ3��Ҫ�������ʱû�пռ䡣
	theApp.m_Mv400.MoveDot(K_AXIS_C,theApp.m_tSysParam.MotorCInitPos,theApp.m_tSysParam.tAxis[K_AXIS_C-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_C-1].homeAcc,true,true);//8
	CFunction::DelayEx(2);
	theApp.m_Mv400.MoveDot(K_AXIS_A,theApp.m_tSysParam.MotorAInitPos,theApp.m_tSysParam.tAxis[K_AXIS_A-1].highHomeVel,theApp.m_tSysParam.tAxis[K_AXIS_A-1].homeAcc,true,true);//3

    //�ֶ��궨�������ģʽ
	if(!g_pFrm->m_CmdRun.SetAxisProfile(LOW_VEL))
	{
		str.Format("�ֶ��궨��ͷ1ʱ�л�������ʱʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		AfxMessageBox("���õ����������");
	}

	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		str.Format("�ֶ��궨��ͷ1ʱZ����ȫλʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return TRUE;
	}
	//XY ������λ��
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].x,g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0].y,true))
	{
		str.Format("�ֶ��궨��ͷ1ʱ��λ���ϴα궨λʧ�ܣ�");
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		return TRUE;
	}

	return TRUE; 
}

//�����ĵ�
void CDlgNeedleOne::OnBnClickedBtnMovetocenterNdleone()
{
	// TODO: Add your control notification handler code here
	double dCurXPos,dCurYpos;
	double dX,dY;
	CString str;
	OnBnClickedBtnSearchmodelNdleone();
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_Y);
	str.Format("����ͷ����ʱ��ƫǰʵ�����꣺X:%0.3f Y:%0.3f",dCurXPos,dCurYpos);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str); 

	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX-(milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY-(milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	//����ģ�����ʵʱ���������
	str.Format("������ʱ����ƫ������mm��:X:%0.3f Y:%0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	if ((fabs(dX)>2)||(fabs(dY)>2))
	{
		AfxMessageBox("ƫ�ƹ��������¼��㣡");
		return;
	}
	dCurXPos-=dX;
	dCurYpos-=dY;
	if (!g_pFrm->m_CmdRun.SynchronizeMoveXY(dCurXPos,dCurYpos,true))
	{
		return;
	}

	str.Format("��ͷ1������ʱִ����Ͼ�ƫ������");
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	Sleep(2000);
	dCurXPos=theApp.m_Mv400.GetPos(K_AXIS_X);
	dCurYpos=theApp.m_Mv400.GetPos(K_AXIS_Y);
	str.Format("��ƫ֮��ʵ�����꣺X:%0.3f Y:%0.3f",dCurXPos,dCurYpos);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
}

//ѧģ��
void CDlgNeedleOne::OnBnClickedBtnLearnmodelNdleone()
{
	// TODO: Add your control notification handler code here
	CString str;
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	str.Format("1����ͷ��ʼѧģ�壡");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	//prepare the image:copy to buffer;
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("1����ͷ���ȡ��ʧ�ܣ�");
		str.Format("1����ͷͼ�񿽱�ʧ�ܣ�");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("1����ͷͼ�񿽱��ɹ���");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	CString strFile;
	//prepare the image rect to buffer;
	m_rectModelWin.left=theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().x = long(m_picture.m_mod_rectangle.dx - m_picture.m_mod_rectangle.width/2);
	m_rectModelWin.top=theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().y = long(m_picture.m_mod_rectangle.dy - m_picture.m_mod_rectangle.height/2);
	m_rectModelWin.right=theApp.m_tSysParam.BmpNeedleLearnWin.BottomRight().x = theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().x + m_picture.m_mod_rectangle.width;
	m_rectModelWin.bottom=theApp.m_tSysParam.BmpNeedleLearnWin.BottomRight().y = theApp.m_tSysParam.BmpNeedleLearnWin.TopLeft().y + m_picture.m_mod_rectangle.height;
	theApp.BmpParam(FALSE);
	str.Format("1����ͷģ������꣺��%d///�ң�%d///��:%d///�£�%d",m_rectModelWin.left,m_rectModelWin.right,m_rectModelWin.top,m_rectModelWin.bottom);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	strFile=g_pDoc->GetModeFile(11);
	milApp.LearnModel(m_rectModelWin);
	//save the model file;
	milApp.SaveModel(strFile.GetBuffer(strFile.GetLength()));
	m_rectROI=m_picture.RoiRect;
	theApp.BmpParam(FALSE);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
}

//����ϵ��
void CDlgNeedleOne::OnBnClickedBtnComputescaleNdleone()
{
	// TODO: Add your control notification handler code here
	CString str;
	tgPos tgTempPixel;
	double dTempX,dTempY;
	double dTempA,dTempB;
	CRect ROI(0,0,milApp.m_lNeedleBufSizeX-1,milApp.m_lNeedleBufSizeY-1);
	//����ϵ��ǰ�����ߵ��߽�����
	//dTempX=theApp.m_Mv400.GetPos(K_AXIS_X);
	//dTempY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	//g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX-1.0),(dTempY-1.0),true);

	str.Format("��ͷ1����ϵ��ǰ��һ��ȡ��...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	if(!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("ץȡͼ��ʧ�ܣ�");
		str.Format("��ͷ1����ϵ��ʱ���״ο���ͼ��ʧ�ܣ�");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	str.Format("��ͷ1����ϵ��ʱ���״ο���ͼ��ɹ���");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
    
	str.Format("��ͷ1����ϵ��ʱ������ͼƬ����...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if (FindModelCenter(ROI))
	{
		tgTempPixel.x=dCenterX;
		tgTempPixel.y=dCenterY;
		str.Format("��ͷ1����ϵ��ʱ��ͼ��1���ĵ����꣺X:%d;Y:%d",dCenterX,dCenterY);
        g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
	}
	else
	{
		AfxMessageBox("����ģ��ʧ�ܣ�");
		str.Format("��ͷ1����ϵ��ʱ��ͼ��1������ʧ�ܣ�");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}

	dTempX=theApp.m_Mv400.GetPos(K_AXIS_X);
	dTempY=theApp.m_Mv400.GetPos(K_AXIS_Y);
	g_pFrm->m_CmdRun.SynchronizeMoveXY((dTempX+0.5),(dTempY+0.5),true);
	Sleep(2000);
	CFunction::DelayEx(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay);
	str.Format("[��ͷ1����ϵ��ʱ]����ʼ����ͼ��2");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if(!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))//only for the Main Camera to use.
	{
		AfxMessageBox("ץȡͼ��ʧ�ܣ�");
		str.Format("[��ͷ1����ϵ��ʱ]��ͼ��2����ʧ�ܣ�");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("[��ͷ1����ϵ��ʱ]��ͼ��2�����ɹ���");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

    str.Format("[��ͷ1����ϵ��ʱ]����ʼ��ͼ��2���ĵ�");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	if (FindModelCenter(ROI))
	{
		str.Format("ͼ��2�����ĳɹ���X��%d...Y:%d",dCenterX,dCenterY);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		dTempA=(dCenterX-tgTempPixel.x)/0.5;
		dTempB=(dCenterY-tgTempPixel.y)/0.5;
		if ((0==dTempA)||(0==dTempB))
		{
			str.Format("��ͷ1ϵ�������쳣��");
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			g_pFrm->m_CmdRun.AddMsg(str);
		}
	}
	else
	{
		AfxMessageBox("����ģ��ʧ�ܣ�");
		str.Format("ͼ��2������ʧ�ܣ�");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	str.Format("[��ͷ1����ϵ��]�ɹ���dTempA:%d;dTempB��%d",dTempA,dTempB);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	theApp.m_tSysParam.BmpNeedleCalibration.dCalibrationA =dTempA;
	theApp.m_tSysParam.BmpNeedleCalibration.dCalibrationB = dTempB;
	theApp.BmpParam(FALSE);
}


//��ģ������
bool CDlgNeedleOne::FindModelCenter(CRect ROI)
{
	CString str;
	double dX,dY;
	milApp.SetPatParam(theApp.m_tSysParam.BmpNeedleMarkAcceptance,theApp.m_tSysParam.BmpNeedleMarkAngle);
	milApp.SetSearchWindow(ROI);
	if(!milApp.FindModel(true))
	{
		CString str;
		str.Format("��ͷ1��ģ��ʧ��");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	dCenterX = milApp.m_stResult.dResultCenterX[0];
	dCenterY = milApp.m_stResult.dResultCenterY[0];
	str.Format("��ͷ1��ģ��ɹ�������X:%d;Y:%d",dCenterX,dCenterY);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	theApp.m_tSysParam.BmpNeedleCalibration.ConvertPixelToMM(dCenterX - (milApp.m_lNeedleBufSizeX-1)/2,
		dCenterY - (milApp.m_lNeedleBufSizeY-1)/2,dX,dY);
	str.Format("��ͷ1�ҵ�ģ��ƫ��:X:%0.3f Y: %0.3f",dX,dY);
	g_pFrm->m_CmdRun.AddMsg(str);
	g_pFrm->m_CmdRun.PutLogIntoList(str);

	//����ģ��߿�
	int left = int((dCenterX-(m_rectModelWin.Width()-1)/2)*m_picture.m_dScale);
	int top = int((dCenterY-(m_rectModelWin.Height()-1)/2)*m_picture.m_dScale);
	int width = int(m_rectModelWin.Width()*m_picture.m_dScale);
	int height = int(m_rectModelWin.Height()*m_picture.m_dScale);
	m_picture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
	return true;

}
//��ģ��
void CDlgNeedleOne::OnBnClickedBtnSearchmodelNdleone()
{
	// TODO: Add your control notification handler code here
	if (!g_pView->m_ImgStatic.CopyNeedleImage(RGB8))
	{
		AfxMessageBox("ȡ��ʧ�ܣ�");
			return;
	}
	m_rectROI=m_picture.RoiRect;
	//set model parameter:
	CString strFile = g_pDoc->GetPadModelFile(11);
	milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode); 
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	FindModelCenter(m_rectROI);

}



void CDlgNeedleOne::OnNMCustomdrawSliderNeedleSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_NEEDLE_SPEED))->GetPos();
	CJogButton::m_dJogSpdRatio = n;
	*pResult = 0;
}

void CDlgNeedleOne::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nValue = 0;
	if(pScrollBar == (CScrollBar *)&m_sliderShutter)
	{
		nValue = m_sliderShutter.GetPos();
		g_pView->m_ImgStatic.m_pCameraNeedle.SetExposureTime((long)nValue);
		theApp.m_tSysParam.NeedleCameraExposure = nValue;
	}
	else if(pScrollBar == (CScrollBar *)&m_sliderGain)
	{
		nValue = m_sliderGain.GetPos();
		g_pView->m_ImgStatic.m_pCameraNeedle.SetGain((long)nValue);
		theApp.m_tSysParam.NeedleCameraGain = nValue;
	}
	theApp.BmpParam(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//AUTO SAVE
void CDlgNeedleOne::OnEnChangeEditNeedleAcceptance()
{
 CString str;
 GetDlgItemText(IDC_EDIT_NEEDLE_ACCEPTANCE,str);
 theApp.m_tSysParam.BmpNeedleMarkAcceptance=atof(str);
 theApp.BmpParam(FALSE);	
}

void CDlgNeedleOne::OnEnChangeEditNeedleAngle()
{
	CString str;
	GetDlgItemText(IDC_EDIT_NEEDLE_ANGLE,str);
	theApp.m_tSysParam.BmpNeedleMarkAngle=atof(str);
	theApp.BmpParam(FALSE);
}

void CDlgNeedleOne::OnBnClickedCancel()
{
	//��Դ���ƣ�
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outNeedleLight,FALSE);
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outMarkLight,TRUE);

	if(!g_pFrm->m_CmdRun.SetAxisProfile(MEDIUM_VEL))
	{
		AfxMessageBox("���õ����������");
	}
	g_pFrm->m_CmdRun.MoveToZSafety();
	g_pFrm->m_CmdRun.m_bNeedleCalibrate=false;//20180814
	OnCancel();
}

BOOL CDlgNeedleOne::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_SYSKEYDOWN==pMsg->message&&VK_F4==pMsg->wParam) //Pretranslate
		return true;

	if(WM_KEYDOWN==pMsg->message&&VK_ESCAPE==pMsg->wParam) //Pretranslate
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}
