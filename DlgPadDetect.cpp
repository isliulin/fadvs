// DlgPadDetect.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgPadDetect.h"
#include "DlgDetectCalib.h"
#include "DlgDetectBase.h"
#include <math.h>


// CDlgPadDetect dialog

IMPLEMENT_DYNAMIC(CDlgPadDetect, CDialog)

CDlgPadDetect::CDlgPadDetect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPadDetect::IDD, pParent)
{
	m_nImageWidth = milApp.m_lPadBufSizeX; //paddetect camera width
	m_nImageHeight = milApp.m_lPadBufSizeY;//paddetect camera height
}

CDlgPadDetect::~CDlgPadDetect()
{
}

void CDlgPadDetect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_PAD_DETECT_PIC,m_stPadPicture);
	DDX_Control(pDX,IDC_SLIDER_PAD_DETECT_EXPOSURE_TIME,m_sliderShutter);
	DDX_Control(pDX,IDC_SLIDER_PAD_DETECT_GAIN,m_sliderGain);
	DDX_Control(pDX, IDC_COMBO1, m_cbPadSelect);
}


BEGIN_MESSAGE_MAP(CDlgPadDetect, CDialog)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_PAD_DETECT_EXPOSURE_TIME, &CDlgPadDetect::OnEnChangeEditPadDetectExposureTime)
	ON_EN_CHANGE(IDC_EDIT_PAD_DETECT_GAIN, &CDlgPadDetect::OnEnChangeEditPadDetectGain)
  //ON_BN_CLICKED(IDC_BTN_PAD_DETECT_NEWPIC, &CDlgPadDetect::OnBnClickedBtnPadDetectNewpic)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_SAVE_LEARN_WIN, &CDlgPadDetect::OnBnClickedBtnPadDetectSaveLearnWin)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_SAVE_ROI, &CDlgPadDetect::OnBnClickedBtnPadDetectSaveRoi)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_SAVE_MODEL, &CDlgPadDetect::OnBnClickedBtnPadDetectSaveModel)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_TEST, &CDlgPadDetect::OnBnClickedBtnPadDetectTest)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_SEARCH, &CDlgPadDetect::OnBnClickedBtnPadDetectSearch)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_SCALE, &CDlgPadDetect::OnBnClickedBtnPadDetectScale)
	ON_BN_CLICKED(IDOK, &CDlgPadDetect::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgPadDetect::OnBnClickedCancel)
  //ON_BN_CLICKED(IDC_BTN_PAD_DETECT_GRABPOS, &CDlgPadDetect::OnBnClickedBtnPadDetectGrabpos)
	ON_BN_CLICKED(IDC_BTN_PAD_DETECT_LIGHT, &CDlgPadDetect::OnBnClickedBtnPadDetectLight)
  //ON_BN_CLICKED(IDC_BTN_PAD_DETECT_ROTATE, &CDlgPadDetect::OnBnClickedBtnPadDetectRotate)
   // ON_BN_CLICKED(IDC_BTN_FIRSTPIXSEL_SAVE, &CDlgPadDetect::OnBnClickedBtnFirstpixselSave)
   ON_WM_TIMER()
   //ON_BN_CLICKED(IDC_BTN_FINDBLOB, &CDlgPadDetect::OnBnClickedBtnFindblob)
   ON_CBN_SELCHANGE(IDC_COMBO_IMGSELECT, &CDlgPadDetect::OnCbnSelchangeComboImgselect)
END_MESSAGE_MAP()


// CDlgPadDetect message handlers

BOOL CDlgPadDetect::OnInitDialog()
{
	CDialog::OnInitDialog();
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,TRUE);//open light
	CString str;
	str.Format("510");
	theApp.m_Serial.WritePlcData(1,"R",str);
	InitImagePaint();
	for (int i=0;i<5;i++)
	{
		str.Format("Mark %d",i);
		m_cbPadSelect.AddString(str);
	}
	m_cbPadSelect.SetCurSel(0);
	nSelectID = m_cbPadSelect.GetCurSel();
	OnCbnSelchangeComboImgselect();
	GetDlgItem(IDC_BTN_PAD_DETECT_LIGHT)->EnableWindow(theApp.m_SysUser.m_CurUD.level>GM_ADMIN);

	if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		if((g_pView->m_ImgStatic.m_pDetectCamera!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit)) //1.pointer ;2.init success.
		{
			if(!g_pView->m_ImgStatic.DetectContinueGrab())
			{
				//AfxMessageBox("图像连续抓取开始失败！");
			}
		}
		else
		{
			AfxMessageBox("DaHua相机异常...");
		}
	}
	else
	{
		if((g_pView->m_ImgStatic.m_pPointGreyCam!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit)) 
		{
			SetTimer(0,200,NULL);
		}		
	}
	InitUI();
	UpdateUI();
	return TRUE;
}

void CDlgPadDetect::InitUI()
{
	CString str;
	m_sliderShutter.SetRange(0,999);
	m_sliderGain.SetRange(0,63);
	m_sliderShutter.SetPos(theApp.m_tSysParam.CameraDetectExposureTime);
	m_sliderGain.SetPos(theApp.m_tSysParam.CameraDetectGain);
	str.Format("%d",theApp.m_tSysParam.CameraDetectExposureTime);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_EXPOSURE_TIME,str);
	str.Format("%d",theApp.m_tSysParam.CameraDetectGain);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_GAIN,str);
	
}
//the axis current runtime position
//the padcheck position 
void CDlgPadDetect::UpdateUI()
{
	CString str;
	m_sliderShutter.SetPos(theApp.m_tSysParam.CameraDetectExposureTime);//when modify,update once
	m_sliderGain.SetPos(theApp.m_tSysParam.CameraDetectGain);
	str.Format("%d",theApp.m_tSysParam.CameraDetectExposureTime);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_EXPOSURE_TIME,str);
	str.Format("%d",theApp.m_tSysParam.CameraDetectGain);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_GAIN,str);

	str.Format("%.3f",theApp.m_tSysParam.BmpPadDetectAcceptance[0]);//when modify,update once
	SetDlgItemText(IDC_EDIT_PAD_DETECT_ACCEPTANCE,str);
	str.Format("%.1f",theApp.m_tSysParam.BmpPadDetectAngle[0]);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_ANGLE,str);

	str.Format("%0.5f",theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationA);//when modify,update once
	SetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_A,str);
	str.Format("%0.5f",theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationB);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_B,str);
	str.Format("%0.5f",theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationC);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_C,str);
	str.Format("%0.5f",theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationD);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_D,str);

	str.Format("%d",theApp.m_tSysParam.BmpPadDetectROI[0].TopLeft().x);//when modify,update once
	SetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_LEFT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPadDetectROI[0].TopLeft().y);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_TOP,str);
	str.Format("%d",theApp.m_tSysParam.BmpPadDetectROI[0].BottomRight().x);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_RIGHT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPadDetectROI[0].BottomRight().y);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_BOTTOM,str);

	str.Format("%d",theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].TopLeft().x);//when modify,update once
	SetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_LEFT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].TopLeft().y);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_TOP,str);
	str.Format("%d",theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].BottomRight().x);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_RIGHT,str);
	str.Format("%d",theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].BottomRight().y);
	SetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_BOTTOM,str);
   
	//Blob参数加载
	/*str.Format("%d",theApp.m_tSysParam.StandardLen);
	SetDlgItemText(IDC_EDIT_BLOB_THRESHOLD,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpWidthMin);
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_LENGTH,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpWidthMax);
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_LENGTH,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpHeighMin);
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_WIDTH,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpHeighMax);
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_WIDTH,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpAreaMin);
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_AREA,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpAreaMax);
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_AREA,str);*/

	//draw the elements of images:images,markbox,roibox again;
	//m_stPadPicture.m_modtype = MOD_RECTANGLE;
	//m_stPadPicture.m_iImgWidth = m_nImageWidth;
	//m_stPadPicture.m_iImgHeight = m_nImageHeight;
	//m_stPadPicture.SetDisplayMarkImage(false); //change data source before paint image;
	//m_stPadPicture.yOffset=0;
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	//m_stPadPicture.m_pImgBuffer = new BYTE[m_stPadPicture.m_iImgWidth*m_stPadPicture.m_iImgHeight];
	m_rectROI = theApp.m_tSysParam.BmpPadDetectROI[0];//load roi data
	m_rectModelWin = theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID];//load model data
	m_stPadPicture.m_mod_rectangle.dx = m_rectModelWin.TopLeft().x + m_rectModelWin.Width()/2;//transfer model data
	m_stPadPicture.m_mod_rectangle.dy = m_rectModelWin.TopLeft().y + m_rectModelWin.Height()/2;
	m_stPadPicture.m_mod_rectangle.width = m_rectModelWin.Width();
	m_stPadPicture.m_mod_rectangle.height = m_rectModelWin.Height();
	m_stPadPicture.m_mod_scale.length = 20;
	m_stPadPicture.m_mod_scale.width = 10;
	m_stPadPicture.m_mod_scale.height = 10;
	m_stPadPicture.RoiRect =  m_rectROI;//transfer roi data	
	m_stPadPicture.ImgInit(m_nImageWidth,m_nImageHeight,8);

	//Reload the greypatmatch data;
	CString strFile = g_pDoc->GetPadModelFile(nSelectID);
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
}

//保存搜索相关参数
void CDlgPadDetect::SaveParam()
{
	CString str;
	GetDlgItemText(IDC_EDIT_PAD_DETECT_ANGLE,str);
	theApp.m_tSysParam.BmpPadDetectAngle[0] = atof(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_ACCEPTANCE,str);
	theApp.m_tSysParam.BmpPadDetectAcceptance[0] = atof(str);

	GetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_LEFT,str);
	theApp.m_tSysParam.BmpPadDetectROI[0].left = atol(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_RIGHT,str);
	theApp.m_tSysParam.BmpPadDetectROI[0].right = atol(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_TOP,str);
	theApp.m_tSysParam.BmpPadDetectROI[0].top = atol(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_ROI_BOTTOM,str);
	theApp.m_tSysParam.BmpPadDetectROI[0].bottom = atol(str);

	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_LEFT,str);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].left = atol(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_RIGHT,str);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].right = atol(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_TOP,str);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].top = atol(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_BOTTOM,str);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].bottom = atol(str);

	GetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_A,str);
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationA = atof(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_B,str);
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationB = atof(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_C,str);
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationC = atof(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_CALIBRATION_D,str);
	theApp.m_tSysParam.BmpPadDetectCalibration.dCalibrationD = atof(str);

    //BLOB参数
	/*GetDlgItemText(IDC_EDIT_BLOB_THRESHOLD,str);
	theApp.m_tSysParam.StandardLen = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_WIDTH,str);
	theApp.m_tSysParam.BmpHeighMin = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_WIDTH,str);
	theApp.m_tSysParam.BmpHeighMax = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_LENGTH,str);
	theApp.m_tSysParam.BmpWidthMin = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_LENGTH,str);
	theApp.m_tSysParam.BmpWidthMax = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_AREA,str);
	theApp.m_tSysParam.BmpAreaMin = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_AREA,str);
	theApp.m_tSysParam.BmpAreaMax = atof(str);*/
	theApp.BmpParam(FALSE);

}
//edit control effect slider control
void CDlgPadDetect::OnEnChangeEditPadDetectExposureTime()
{
	int nValue = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_PAD_DETECT_EXPOSURE_TIME,str);
	nValue = atoi(str);
	m_sliderShutter.SetPos(nValue);
	if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		if((g_pView->m_ImgStatic.m_pDetectCamera!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
		{
			g_pView->m_ImgStatic.m_pDetectCamera->setExposure((float)nValue);
		}
		else
		{
			AfxMessageBox("DaHua相机异常，曝光时间设定失败...");
		}
	}
	else
	{
		if((g_pView->m_ImgStatic.m_pPointGreyCam!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
		{
			g_pView->m_ImgStatic.m_pPointGreyCam->SetShutter((float)nValue);
		}
		else
		{
			AfxMessageBox("PointGrey相机异常，曝光时间设定失败...");
		}
	}
	theApp.m_tSysParam.CameraDetectExposureTime = nValue;
	theApp.BmpParam(FALSE);
}

//edit control effect slider control
void CDlgPadDetect::OnEnChangeEditPadDetectGain()
{
	int nValue = 0;
	CString str;
	GetDlgItemText(IDC_EDIT_PAD_DETECT_GAIN,str);
	nValue = atoi(str);
	m_sliderGain.SetPos(nValue);
	if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
	{
		if((g_pView->m_ImgStatic.m_pDetectCamera!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
		{
			g_pView->m_ImgStatic.m_pDetectCamera->setGain((float)nValue);
		}
		else
		{
			AfxMessageBox("DaHua相机异常，增益设定失败...");
		}
	}
	else
	{
		if((g_pView->m_ImgStatic.m_pPointGreyCam!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
		{
			g_pView->m_ImgStatic.m_pPointGreyCam->SetGain((float)nValue);
		}
		else
		{
			AfxMessageBox("PointGrey相机异常，增益设定失败...");
		}
	}	
	theApp.m_tSysParam.CameraDetectGain = nValue;
	theApp.BmpParam(FALSE);
}

//滚动控制通用接口
void CDlgPadDetect::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nValue = 0;
	CString str;
	if(pScrollBar == (CScrollBar *)&m_sliderShutter)
	{
		nValue = m_sliderShutter.GetPos();
		if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
		{
			if((g_pView->m_ImgStatic.m_pDetectCamera!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
			{
				g_pView->m_ImgStatic.m_pDetectCamera->setExposure((float)nValue);//set shutter
			}
			else
			{
				AfxMessageBox("DaHua相机异常，曝光设定失败...");
			}
		}
		else
		{
			if((g_pView->m_ImgStatic.m_pPointGreyCam!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
			{
				g_pView->m_ImgStatic.m_pPointGreyCam->SetShutter((float)nValue);//set shutter
			}
			else
			{
				AfxMessageBox("PointGrey相机异常，曝光设定失败...");
			}
		}		
		theApp.m_tSysParam.CameraDetectExposureTime = nValue;
		str.Format("%d",nValue);
		SetDlgItemText(IDC_EDIT_PAD_DETECT_EXPOSURE_TIME,str);
	}
	else if(pScrollBar == (CScrollBar *)&m_sliderGain)
	{
		nValue = m_sliderGain.GetPos();
		if (theApp.m_tSysParam.detectCameraType==DETECT_CAMERA_MV)
		{
			if((g_pView->m_ImgStatic.m_pDetectCamera!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
			{
				g_pView->m_ImgStatic.m_pDetectCamera->setGain((float)nValue);//set gain
			}
			else
			{
				AfxMessageBox("DaHua相机异常，增益设定失败...");
			}
		}
		else
		{
			if((g_pView->m_ImgStatic.m_pPointGreyCam!=NULL)&&(g_pView->m_ImgStatic.m_bDetectInit))
			{
				g_pView->m_ImgStatic.m_pPointGreyCam->SetGain((float)nValue);//set gain
			}
			else
			{
				AfxMessageBox("PointGrey相机异常，增益设定失败...");
			}
		}		
		theApp.m_tSysParam.CameraDetectGain = nValue;
		str.Format("%d",nValue);
		SetDlgItemText(IDC_EDIT_PAD_DETECT_GAIN,str);
	}
	theApp.BmpParam(FALSE);
}

//保存模板窗口
void CDlgPadDetect::OnBnClickedBtnPadDetectSaveLearnWin()
{
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].TopLeft().x = long(m_stPadPicture.m_mod_rectangle.dx - m_stPadPicture.m_mod_rectangle.width/2);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].TopLeft().y = long(m_stPadPicture.m_mod_rectangle.dy - m_stPadPicture.m_mod_rectangle.height/2);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].BottomRight().x = theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].TopLeft().x + m_stPadPicture.m_mod_rectangle.width;
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].BottomRight().y = theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].TopLeft().y + m_stPadPicture.m_mod_rectangle.height;
	CString str;
	str.Format("Normal之前模板框[L:%d;R:%d;T:%d;B:%d.]",theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].left,theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].right,theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].top,theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].bottom);
	g_pFrm->m_CmdRun.AddMsg(str);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].NormalizeRect();
	str.Format("Normal之后模板框[L:%d;R:%d;T:%d;B:%d.]",theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].left,theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].right,theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].top,theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID].bottom);
	g_pFrm->m_CmdRun.AddMsg(str);
	UpdateUI();
	theApp.BmpParam(FALSE);
}

//保存ROI
void CDlgPadDetect::OnBnClickedBtnPadDetectSaveRoi()
{
	theApp.m_tSysParam.BmpPadDetectROI[0] = m_stPadPicture.RoiRect;
	
	CString str;
	str.Format("Normal之前ROI框[L:%d;R:%d;T:%d;B:%d.]",theApp.m_tSysParam.BmpPadDetectROI[0].left,theApp.m_tSysParam.BmpPadDetectROI[0].right,theApp.m_tSysParam.BmpPadDetectROI[0].top,theApp.m_tSysParam.BmpPadDetectROI[0].bottom);
	g_pFrm->m_CmdRun.AddMsg(str);
	theApp.m_tSysParam.BmpPadDetectROI[0].NormalizeRect();
	str.Format("Normal之后ROI框[L:%d;R:%d;T:%d;B:%d.]",theApp.m_tSysParam.BmpPadDetectROI[0].left,theApp.m_tSysParam.BmpPadDetectROI[0].right,theApp.m_tSysParam.BmpPadDetectROI[0].top,theApp.m_tSysParam.BmpPadDetectROI[0].bottom);
	g_pFrm->m_CmdRun.AddMsg(str);
	UpdateUI();
	theApp.BmpParam(FALSE);
}

//保存模板
void CDlgPadDetect::OnBnClickedBtnPadDetectSaveModel()
{
	CString strFile;
	strFile = g_pDoc->GetPadModelFile(nSelectID); //Load the file path 
	CString str;
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_LEFT,str);
	m_rectModelWin.left = atoi(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_TOP,str);
	m_rectModelWin.top = atoi(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_RIGHT,str);
	m_rectModelWin.right = atoi(str);
	GetDlgItemText(IDC_EDIT_PAD_DETECT_LEARN_WIN_BOTTOM,str);
	m_rectModelWin.bottom = atoi(str);
	theApp.m_tSysParam.BmpPadDetectLearnWin[nSelectID] = m_rectModelWin;
	milApp.LearnPadModel(m_rectModelWin);
	milApp.SavePadModel(strFile.GetBuffer(strFile.GetLength()));//only have to apply the path
}

void CDlgPadDetect::OnBnClickedBtnPadDetectTest()
{
	CString str;
	str.Format("缺颗粒界面-->测试开始...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(TRANSITION_INSPECT==g_pFrm->m_CmdRun.ProductParam.inspectMode)
	{
		Detect(false);
	}
	else if(SESSION_INDPECT==g_pFrm->m_CmdRun.ProductParam.inspectMode)
	{
		Detect(true);
	}
	str.Format("缺颗粒界面-->测试结束...");
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
}

bool CDlgPadDetect::Detect(bool bSplice)
{
	DWORD W_Startime,W_EndTime;
	W_Startime = W_EndTime = GetTickCount();
	int nRetryNum=0;

	CString str;
	tgPos tgStartPixel;
	double dFirstPixelX,dFirstPixelY,dSecondPixelX,dSecondPixelY,dThirdPixelX,dThirdPixelY;
	CRect rect;
	bool *pPadDetect = NULL;
	tgPos *pHasPadPos = NULL;
	int left,top,width,height;
	left=int(theApp.m_tSysParam.BmpPadDetectROI[0].left*m_stPadPicture.m_dScale);
	top=int(theApp.m_tSysParam.BmpPadDetectROI[0].top*m_stPadPicture.m_dScale);
	width=int(theApp.m_tSysParam.BmpPadDetectROI[0].Width()*m_stPadPicture.m_dScale);
	height=int(theApp.m_tSysParam.BmpPadDetectROI[0].Height()*m_stPadPicture.m_dScale);
	CDC * pDC=m_stPadPicture.GetDC();
	pDC->Draw3dRect(left,top,width,height,RGB(0,0,255),RGB(0,0,255)); //绘制pad搜索区域:绘制时注意比例 
	ReleaseDC(pDC);
	//1,2,3级产品规格数据转换		
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD,g_pFrm->m_CmdRun.ProductParam.FirstDispRowD,dFirstPixelX,dFirstPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD,g_pFrm->m_CmdRun.ProductParam.SecondDispRowD,dSecondPixelX,dSecondPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD,g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD,dThirdPixelX,dThirdPixelY);
	//局部搜索pad确定起点
	rect.left=theApp.m_tSysParam.BmpPadDetectROI[0].left+(theApp.m_tSysParam.BmpPadDetectROI[0].Width()/2);
	rect.top=theApp.m_tSysParam.BmpPadDetectROI[0].top;
	rect.right=theApp.m_tSysParam.BmpPadDetectROI[0].right;
	rect.bottom=theApp.m_tSysParam.BmpPadDetectROI[0].top+(theApp.m_tSysParam.BmpPadDetectROI[0].Height()/2);
	CString strFile = g_pDoc->GetPadModelFile(0);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPadSearchParam(theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	milApp.SetPadModelWindow(theApp.m_tSysParam.BmpPadDetectLearnWin[0]);
	milApp.SetPadSearchWindow(rect);
	if(!milApp.FindPadModels())
	{
		CString str;
		str.Format("缺颗粒调试界面：局部区域查找pad时失败...");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	if(milApp.m_stPadResult.nResultNum<=0)
	{
		CString str;
		str.Format("缺颗粒调试界面:局部区域查找pad是匹配数目为0...");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return false;
	}
	else
	{
		int SearchScoreNum=milApp.m_stPadResult.nResultNum;
		tgStartPixel.x=milApp.m_stPadResult.dResultCenterX[0];
		tgStartPixel.y=milApp.m_stPadResult.dResultCenterY[0];
		for (int i=0;i<SearchScoreNum;i++)
		{
			if (milApp.m_stPadResult.dResultCenterX[i]>tgStartPixel.x)
			{
				tgStartPixel.x=milApp.m_stPadResult.dResultCenterX[i];
			}
			if (milApp.m_stPadResult.dResultCenterY[i]<tgStartPixel.y)
			{
				tgStartPixel.y=milApp.m_stPadResult.dResultCenterY[i];
			}
		}
		if ((tgStartPixel.x<rect.left)||
			(tgStartPixel.x>rect.right)||
			(tgStartPixel.y<rect.top)||
			(tgStartPixel.y>rect.bottom)
			)
		{
			CString str;
			str.Format("定位搜索起点时失败，位置异常...");
			g_pFrm->m_CmdRun.PutLogIntoList(str);
			g_pFrm->m_CmdRun.AddMsg(str);
			return false;
		}

		str.Format("调试界面测试---【块查找】区域首点像素坐标：X:%0.3f,Y:%0.3f",tgStartPixel.x,tgStartPixel.y);
		g_pFrm->m_CmdRun.AddMsg(str);
		g_pFrm->m_CmdRun.PutLogIntoList(str);

		int Left,Top,Width,Height;
		Left=(tgStartPixel.x-(theApp.m_tSysParam.BmpPadDetectLearnWin[0].Width()/2))*m_stPadPicture.m_dScale;
		Top=(tgStartPixel.y-(theApp.m_tSysParam.BmpPadDetectLearnWin[0].Height()/2))*m_stPadPicture.m_dScale;
		Width=theApp.m_tSysParam.BmpPadDetectLearnWin[0].Width()*m_stPadPicture.m_dScale;
		Height=theApp.m_tSysParam.BmpPadDetectLearnWin[0].Height()*m_stPadPicture.m_dScale;
		CDC * pDC=m_stPadPicture.GetDC();
		pDC->Draw3dRect(Left,Top,Width,Height,RGB(255,255,255),RGB(255,255,255)); //绘制局部查找后首颗位置:白色
		ReleaseDC(pDC);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	if(pPadDetect!=NULL)
	{
		delete[] pPadDetect;
		pPadDetect = NULL;
	}
	if (pHasPadPos!=NULL)
	{
		delete[] pHasPadPos;
		pHasPadPos = NULL;
	}
	int nPadNumber = g_pFrm->m_CmdRun.ProductParam.FirstDispRow*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*g_pFrm->m_CmdRun.ProductParam.SecondDispRow*
		g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	pPadDetect = new bool[nPadNumber];   //堆上分配内存
	pHasPadPos = new tgPos[nPadNumber];

	for(int i=0;i<nPadNumber;i++)
	{
		pPadDetect[i] = false;
	}
	int nThirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	int nSecondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
	int nFindNum=0;
	if(true)
	{              
		//更新首点的像素坐标//每次固定的首颗的像素坐标值//规避首颗本身异常的误判
		//此处应用支架的偏移量来修正首点的偏移量；
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
							for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)//Right to left;
							{
								double dPixelX,dPixelY;
								int temID = l + k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn + n*nThirdNum + m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum +
									j*nSecondNum*nThirdNum + i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
								//当前颗粒与首颗的相对距离：mm
								double dDistanceX = j*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD + n*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD +
									l*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
								double dDistanceY = i*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD + m*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD +
									k*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
								//相对距离：mm-->pixsel
								theApp.m_tSysParam.BmpPadDetectCalibration.ConvertMMToPixsel(dDistanceX,dDistanceY,dPixelX,dPixelY);
								dPixelX=fabs(dPixelX);

								long lTempX,lTempY;
								//颗粒理论中心:最后实际中心位置////////
								lTempX=tgStartPixel.x - dPixelX;//on the left,X decrease;
								lTempY= tgStartPixel.y + dPixelY;//on the bottom:Y increased;
								//刷新搜索区域：
								rect.left =lTempX - (0.5*dThirdPixelX);
								rect.right = lTempX +  (0.5*dThirdPixelX);
								rect.top = lTempY - (0.5*dThirdPixelY);
								rect.bottom = lTempY + (0.5*dThirdPixelY);

								//根据压缩比显示搜索区域：
								int left = int((rect.left)*m_stPadPicture.m_dScale);
								int top = int((rect.top)*m_stPadPicture.m_dScale);
								int width = int(rect.Width()*m_stPadPicture.m_dScale);
								int height = int(rect.Height()*m_stPadPicture.m_dScale);
								CDC * pDC=m_stPadPicture.GetDC();
								pDC->Draw3dRect(left,top,width,height,RGB(0,0,255),RGB(0,0,255)); //绘制新的搜索区域				
								ReleaseDC(pDC);

								//str.Format("Pad检测测试：第%d检测开始",temID);
								//g_pFrm->PutLogIntoList(str);
								strFile = g_pDoc->GetPadModelFile(0);
								milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));//加载模板1...

								milApp.SetPadSearchWindow(rect);
								if(!milApp.FindPadModels())
								{
									if(pPadDetect!=NULL)
									{
										delete[] pPadDetect;
										pPadDetect = NULL;
									}
									if(pHasPadPos!=NULL)
									{
										delete[] pHasPadPos;
										pHasPadPos = NULL;
									}
									return false;
								}
								if(milApp.m_stPadResult.nResultNum>0)
								{
									//Draw the first pat result on UI;
									CRect RectPad=theApp.m_tSysParam.BmpPadDetectLearnWin[0];
									int left = int((milApp.m_stPadResult.dResultCenterX[0]-(RectPad.Width()-1)/2)*m_stPadPicture.m_dScale);
									int top = int((milApp.m_stPadResult.dResultCenterY[0]-(RectPad.Height()-1)/2)*m_stPadPicture.m_dScale);
									int width = int(RectPad.Width()*m_stPadPicture.m_dScale);
									int height = int(RectPad.Height()*m_stPadPicture.m_dScale);
									//CDC * pDC=m_stPadPicture.GetDC();
									//pDC->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0)); //绘制成功匹配颗粒；
									//ReleaseDC(pDC);
									pPadDetect[temID] = true;
									pHasPadPos[temID].x = milApp.m_stPadResult.dResultCenterX[0];
									pHasPadPos[temID].y = milApp.m_stPadResult.dResultCenterY[0];
									nFindNum++;
								}
								else
								{
									////////////////////////////再次多找几次/////////////////
									str.Format("[...有材料首次检测失败]");
									g_pFrm->m_CmdRun.PutLogIntoList(str);
									g_pFrm->m_CmdRun.AddMsg(str);

									str.Format("一级行:%d;一级列:%d;二级行:%d;二级列:%d;三级行%d;三级列:%d",i,j,m,n,k,l);
									g_pFrm->m_CmdRun.PutLogIntoList(str);
									g_pFrm->m_CmdRun.AddMsg(str);

									int recordInum=1;
									for (int i=1;i<5;i++)
									{
										str.Format("进入备用机制Pad：第%d检测!",i);
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										g_pFrm->m_CmdRun.AddMsg(str);

										strFile = g_pDoc->GetPadModelFile(i);
										milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
										milApp.SetPadSearchWindow(rect);
										if(!milApp.FindPadModels())
										{
											if(pPadDetect!=NULL)
											{
												delete[] pPadDetect;
												pPadDetect = NULL;
											}
											if(pHasPadPos!=NULL)
											{
												delete[] pHasPadPos;
												pHasPadPos = NULL;

											}
											return false;
										}
										if(milApp.m_stPadResult.nResultNum>0)
										{
											str.Format("进入备用机制第%d检测成功!",i);
											g_pFrm->m_CmdRun.PutLogIntoList(str);
											g_pFrm->m_CmdRun.AddMsg(str);
											recordInum=i;
											nRetryNum++;
											pHasPadPos[temID].x = milApp.m_stPadResult.dResultCenterX[0];
											pHasPadPos[temID].y = milApp.m_stPadResult.dResultCenterY[0];
											break;
										}
									}


									if(milApp.m_stPadResult.nResultNum>0)
									{
										//Draw the first pat result on UI;
										CRect RectPad=theApp.m_tSysParam.BmpPadDetectLearnWin[recordInum];
										int left = int((milApp.m_stPadResult.dResultCenterX[0]-(RectPad.Width()-1)/2)*m_stPadPicture.m_dScale);
										int top = int((milApp.m_stPadResult.dResultCenterY[0]-(RectPad.Height()-1)/2)*m_stPadPicture.m_dScale);
										int width = int(RectPad.Width()*m_stPadPicture.m_dScale);
										int height = int(RectPad.Height()*m_stPadPicture.m_dScale);
										//CDC * pDC=m_stPadPicture.GetDC();
										//pDC->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0)); //绘制成功匹配颗粒；
										//ReleaseDC(pDC);
										pPadDetect[temID] = true;
										nFindNum++;
									}
									else
									{
										str.Format("////****有材料最终NG****如下////");
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										g_pFrm->m_CmdRun.AddMsg(str);
										str.Format("一级行:%d;一级列:%d;二级行:%d;二级列:%d;三级行%d;三级列:%d",i,j,m,n,k,l);
										g_pFrm->m_CmdRun.PutLogIntoList(str);
										g_pFrm->m_CmdRun.AddMsg(str);
										pPadDetect[temID] = false;
									}
									///////////////////////////////////////////////////////////
									str.Format("Pad备用机制检测完毕!");
									g_pFrm->m_CmdRun.PutLogIntoList(str);
									g_pFrm->m_CmdRun.AddMsg(str);

								}
								//str.Format("Pad检测测试：第%d检测完成!",temID);
								//g_pFrm->PutLogIntoList(str);
							}
						}
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////
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
						for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)//Right to left;
						{
							double dPixelX,dPixelY;
							int temID = l + k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn + n*nThirdNum + m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum +
								j*nSecondNum*nThirdNum + i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
							if(pPadDetect[temID])
							{
								int left = int((pHasPadPos[temID].x-(m_rectModelWin.Width()-1)/2)*m_stPadPicture.m_dScale);
								int top = int((pHasPadPos[temID].y-(m_rectModelWin.Height()-1)/2)*m_stPadPicture.m_dScale);
								int width = int(m_rectModelWin.Width()*m_stPadPicture.m_dScale);
								int height = int(m_rectModelWin.Height()*m_stPadPicture.m_dScale);
								//As update the box position,include start point ,length and width;
								m_stPadPicture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));//Draw in green color;
					
							}
						}
					}
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////	
	//统计识别产品数目；
	str.Format("本次检测数目：%d.....识别到颗粒数目：%d...",nPadNumber,nFindNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	str.Format("有%d颗材料反复识别后成功...",nRetryNum);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);

	Sleep(2000);
	if(pPadDetect!=NULL)
	{
		delete[] pPadDetect;
		pPadDetect = NULL;
	}
	if (pHasPadPos!=NULL)
	{
		delete[] pHasPadPos;
		pHasPadPos=NULL;
	}

	W_EndTime=GetTickCount();
	double dTime=(W_EndTime-W_Startime)/1000.00;
	str.Format("缺颗粒测试耗时:%0.3f S...",dTime);
	g_pFrm->m_CmdRun.PutLogIntoList(str);
	g_pFrm->m_CmdRun.AddMsg(str);
	return true;
}

bool CDlgPadDetect::SpliceDetect(bool *pResult,int nStartColumn,int &nEndColumn)
{
	double dCenterX,dCenterY;
	CString str;
	tgPos tgStartPixel;
	double dFirstPixelX,dFirstPixelY,dSecondPixelX,dSecondPixelY,dThirdPixelX,dThirdPixelY;
	int nColumn = nStartColumn;
	int nFirstRow,nFirstColumn,nSecondRow,nSecondColumn,nThirdRow,nThirdColumn;
	nFirstColumn = nStartColumn/(g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn);
	nSecondColumn = nStartColumn/g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn%g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
	nThirdColumn = nStartColumn%g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertTableToPixel(g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD,g_pFrm->m_CmdRun.ProductParam.FirstDispRowD,dFirstPixelX,dFirstPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertTableToPixel(g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD,g_pFrm->m_CmdRun.ProductParam.SecondDispRowD,dSecondPixelX,dSecondPixelY);
	theApp.m_tSysParam.BmpPadDetectCalibration.ConvertTableToPixel(g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD,g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD,dThirdPixelX,dThirdPixelY);
	CRect rect;
	rect.left = theApp.m_tSysParam.BmpPadDetectROI[0].right - theApp.m_tSysParam.BmpPadDetectROI[0].Width()/5;
	rect.right = theApp.m_tSysParam.BmpPadDetectROI[0].right;
	rect.top = theApp.m_tSysParam.BmpPadDetectROI[0].top;
	rect.bottom = theApp.m_tSysParam.BmpPadDetectROI[0].top + theApp.m_tSysParam.BmpPadDetectROI[0].Height()/2;
	m_stPadPicture.GetDC()->Draw3dRect(int(rect.left*m_stPadPicture.m_dScale),int(rect.top*m_stPadPicture.m_dScale),int(rect.Width()*m_stPadPicture.m_dScale),int(rect.Height()*m_stPadPicture.m_dScale),RGB(0,0,255),RGB(0,0,255));
	str.Format("left:%d,right:%d,width:%d,height:%d",rect.left,rect.right,rect.Width(),rect.Height());
	g_pFrm->m_CmdRun.AddMsg(str);
	CString strFile = g_pDoc->GetPadModelFile(0);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPadSearchParam(theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	milApp.SetPadModelWindow(theApp.m_tSysParam.BmpPadDetectLearnWin[0]);
	milApp.SetPadSearchWindow(rect);
	if(!milApp.FindPadModels())
	{
		return false;
	}
	str.Format("PatNum:%d",milApp.m_stPadResult.nResultNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(milApp.m_stPadResult.nResultNum>0)
	{
		tgStartPixel.x = milApp.m_stPadResult.dResultCenterX[0];
		tgStartPixel.y = milApp.m_stPadResult.dResultCenterY[0];
		for(int i=0;i<milApp.m_stPadResult.nResultNum;i++)
		{
			int left = int((milApp.m_stPadResult.dResultCenterX[i]-(m_rectModelWin.Width()-1)/2)*m_stPadPicture.m_dScale);
			int top = int((milApp.m_stPadResult.dResultCenterY[i]-(m_rectModelWin.Height()-1)/2)*m_stPadPicture.m_dScale);
			int width = int(m_rectModelWin.Width()*m_stPadPicture.m_dScale);
			int height = int(m_rectModelWin.Height()*m_stPadPicture.m_dScale);
			m_stPadPicture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,255,0),RGB(255,255,0));
			if(tgStartPixel.x<milApp.m_stPadResult.dResultCenterX[i])
			{
				tgStartPixel.x = milApp.m_stPadResult.dResultCenterX[i];
			}
			if(tgStartPixel.y>milApp.m_stPadResult.dResultCenterY[i])
			{
				tgStartPixel.y = milApp.m_stPadResult.dResultCenterY[i];
			}
		}
		str.Format("tgStartPixel.x:%0.3f,tgStartPixel.y:%0.3f",tgStartPixel.x,tgStartPixel.y);
		g_pFrm->m_CmdRun.AddMsg(str);
		Sleep(1000);
		if(tgStartPixel.y>rect.top + dThirdPixelY)
		{
			tgStartPixel.y = rect.top + dThirdPixelY/2;
		}
		if(tgStartPixel.x<rect.right - dThirdPixelX)
		{
			tgStartPixel.x = rect.right - dThirdPixelX/2;
		}
		int nThirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
		int nSecondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
		double dRowDistance = dThirdPixelX;
		rect.bottom = theApp.m_tSysParam.BmpPadDetectROI[0].bottom;
		for(rect.left = long(tgStartPixel.x - dThirdPixelX/2),rect.right = long(tgStartPixel.x + dThirdPixelX/2);
			(rect.left+rect.right)/2>theApp.m_tSysParam.BmpPadDetectROI[0].left;rect.left-=long(dRowDistance),rect.right-=long(dRowDistance))
		{
			if(rect.left<0)
			{
				rect.left = 0;
			}
			if(rect.right>=IMAGEWIDTH1)
			{
				rect.right = IMAGEWIDTH1-1;
			}
			if(nFirstColumn>=g_pFrm->m_CmdRun.ProductParam.FirstDispColumn||nSecondColumn>=g_pFrm->m_CmdRun.ProductParam.SecondDispColumn||
				nThirdColumn>=g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn)
			{
				break;
			}
			m_stPadPicture.GetDC()->Draw3dRect(int(rect.left*m_stPadPicture.m_dScale),int(rect.top*m_stPadPicture.m_dScale),int(rect.Width()*m_stPadPicture.m_dScale),int(rect.Height()*m_stPadPicture.m_dScale),RGB(255,0,0),RGB(255,0,0));
			milApp.SetPadSearchWindow(rect);
			if(!milApp.FindPadModels())
			{
				return false;
			}
			str.Format("PatNum:%d",milApp.m_stPadResult.nResultNum);
			g_pFrm->m_CmdRun.AddMsg(str);
			//if(milApp.m_stPadResult.nResultNum==0)
			//{
			//	return false;
			//}
			for(int i=0;i<milApp.m_stPadResult.nResultNum;i++)
			{
				int nIndex = int((milApp.m_stPadResult.dResultCenterY[i]-tgStartPixel.y+dThirdPixelY/2)/dFirstPixelY);
				nFirstRow = nIndex;
				nSecondRow = int((milApp.m_stPadResult.dResultCenterY[i]-tgStartPixel.y-nFirstRow*dFirstPixelY+dThirdPixelY/2)/dSecondPixelY);
				nThirdRow = int((milApp.m_stPadResult.dResultCenterY[i]-tgStartPixel.y-nFirstRow*dFirstPixelY-nSecondRow*dSecondPixelY+dThirdPixelY/4)/dThirdPixelY);
				str.Format("nFirstRow = %d,nSecondRow = %d,nThirdRow = %d",nFirstRow,nSecondRow,nThirdRow);
				g_pFrm->m_CmdRun.AddMsg(str);
				if(nFirstRow>=g_pFrm->m_CmdRun.ProductParam.FirstDispRow||nSecondRow>=g_pFrm->m_CmdRun.ProductParam.SecondDispRow||
					nThirdRow>=g_pFrm->m_CmdRun.ProductParam.ThirdDispRow)
				{
					continue;
				}
				else
				{
					dCenterX = milApp.m_stPadResult.dResultCenterX[i];
					dCenterY = milApp.m_stPadResult.dResultCenterY[i];
					int left = int((dCenterX-(m_rectModelWin.Width()-1)/2)*m_stPadPicture.m_dScale);
					int top = int((dCenterY-(m_rectModelWin.Height()-1)/2)*m_stPadPicture.m_dScale);
					int width = int(m_rectModelWin.Width()*m_stPadPicture.m_dScale);
					int height = int(m_rectModelWin.Height()*m_stPadPicture.m_dScale);
					m_stPadPicture.GetDC()->Draw3dRect(left,top,width,height,RGB(255,0,0),RGB(255,0,0));
					nIndex = nFirstRow*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum+nFirstColumn*nSecondNum*nThirdNum+
						nSecondRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum+nSecondColumn*nThirdNum+nThirdRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn+nThirdColumn;
					pResult[nIndex] = true;
					str.Format("%d",nIndex);
					g_pFrm->m_CmdRun.AddMsg(str);
				}
			}
			nThirdColumn++;
			nColumn++;
			dRowDistance = dThirdPixelX;
			if(nThirdColumn>=g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn)
			{
				nThirdColumn = 0;
				nSecondColumn++;
				dRowDistance = dSecondPixelX - (g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn-1)*dThirdPixelX;
				if(nSecondColumn>=g_pFrm->m_CmdRun.ProductParam.SecondDispColumn)
				{
					nSecondColumn = 0;
					nFirstColumn++;
					dRowDistance = dFirstPixelX - (g_pFrm->m_CmdRun.ProductParam.SecondDispColumn-1)*dSecondPixelX;
					if(nFirstColumn>=g_pFrm->m_CmdRun.ProductParam.FirstDispColumn)
					{
						break;
					}
				}
			}
		}
	}
	nEndColumn = nColumn;
	return true;
}
void CDlgPadDetect::OnBnClickedBtnPadDetectSearch()
{
	CString strFile = g_pDoc->GetPadModelFile(nSelectID);////2017-04-13 by zwg nSelectID
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	milApp.SetPadSearchParam(theApp.m_tSysParam.BmpPadDetectAcceptance[0],theApp.m_tSysParam.BmpPadDetectAngle[0]);
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);	
	if(!milApp.FindPadModels())//MIL多模板查找
	{
		CString str;
		str.Format("缺颗粒调试界面：模板查找失败！");
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		return;
	}
	if(milApp.m_stPadResult.nResultNum>0)
	{
		CString str;
		str.Format("缺颗粒调试界面：模板查找成功。匹配数目：%d...",milApp.m_stPadResult.nResultNum);
		g_pFrm->m_CmdRun.PutLogIntoList(str);
		g_pFrm->m_CmdRun.AddMsg(str);
		if (true)
		{
			for(int i=0;i<milApp.m_stPadResult.nResultNum;i++)
			{   //逐个绘制，以模板中心点为基点，大小参考模板框大小以及图片控件的比例；
				//based in the center point;draw the frame box;
				int left = int((milApp.m_stPadResult.dResultCenterX[i]-(m_rectModelWin.Width()-1)/2)*m_stPadPicture.m_dScale);
				int top = int((milApp.m_stPadResult.dResultCenterY[i]-(m_rectModelWin.Height()-1)/2)*m_stPadPicture.m_dScale);
				int width = int(m_rectModelWin.Width()*m_stPadPicture.m_dScale);
				int height = int(m_rectModelWin.Height()*m_stPadPicture.m_dScale);
				//As update the box position,include start point ,length and width;
				m_stPadPicture.GetDC()->Draw3dRect(left,top,width,height,RGB(0,255,0),RGB(0,255,0));//Draw in green color;
				//update the pat-result on UI
				CString strResult;
				strResult.Format("%0.3f",milApp.m_stPadResult.dResultCenterX[i]);
				SetDlgItemText(IDC_EDIT_PAD_DETECT_RESULT_CENTER_X,strResult);
				strResult.Format("%0.3f",milApp.m_stPadResult.dResultCenterY[i]);
				SetDlgItemText(IDC_EDIT_PAD_DETECT_RESULT_CENTER_Y,strResult);
				strResult.Format("%0.3f",milApp.m_stPadResult.dResultScore[i]);
				SetDlgItemText(IDC_EDIT_PAD_DETECT_RESULT_SCORE,strResult);
				strResult.Format("%0.3f",milApp.m_stPadResult.dResultAngle[i]);
				SetDlgItemText(IDC_EDIT_PAD_DETECT_RESULT_ANGLE,strResult);
			}
		}
	}
	CString str;
	str.Format("缺颗粒调试界面：最终查找结果-ResultNum = %d",milApp.m_stPadResult.nResultNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	Sleep(2000);
	//2017-04-13 del sleep();
}

//转换搜索区域时用：mm->pixsel
void CDlgPadDetect::OnBnClickedBtnPadDetectScale()
{
	CDlgDetectCalib detectCalib;
	if(detectCalib.DoModal()==IDOK)
	{
		theApp.BmpParam(FALSE);
	}
	UpdateUI();
}

void CDlgPadDetect::OnBnClickedOk()
{
	CString str;
	str.Format("510");
	theApp.m_Serial.WritePlcData(0,"R",str);//Open sensor
	SaveParam();
	if (NULL!=m_stPadPicture.m_pImgBuffer)//free the buffer mallocated;
	{
		delete[] m_stPadPicture.m_pImgBuffer;
		m_stPadPicture.m_pImgBuffer=NULL;
	}
	Close();
	OnOK();
}

//关闭光源，停止相机
void CDlgPadDetect::Close()
{
	theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
	//g_pView->m_ImgStatic.DetectStop();  //2017-06-09 
}

//打开传感器电源
void CDlgPadDetect::OnBnClickedCancel()
{
	CString str;
	str.Format("510");
	theApp.m_Serial.WritePlcData(0,"R",str);
	Close();
	OnCancel();
}


//void CDlgPadDetect::OnBnClickedBtnPadDetectGrabpos()
//{
//	// TODO: Add your control notification handler code here
//	/*if(SESSION_INDPECT==g_pFrm->m_CmdRun.ProductParam.inspectMode)
//	{
//		tgPos tgPadDetect = g_pFrm->m_CmdRun.ProductParam.tPadDetectPos;
//		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgPadDetect.x,tgPadDetect.y,true))
//		{
//			AfxMessageBox("移动到取图位置失败！");
//			return;
//		}
//		if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgPadDetect.za,true))
//		{
//			AfxMessageBox("移动到取图位置失败！");
//			return;
//		}
//	}*/
//}

//光源打开与关闭
void CDlgPadDetect::OnBnClickedBtnPadDetectLight()
{
	g_pFrm->m_CmdRun.padLackTotal=0;
	/*if(!theApp.m_Mv400.IsInitOK())
	{
		return;
	}
	if(theApp.m_Mv400.GetOutput(theApp.m_tSysParam.outDetectLight))
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,FALSE);
	}
	else
	{
		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outDetectLight,TRUE);
	}*/
}

//void CDlgPadDetect::OnBnClickedBtnPadDetectRotate()
//{
//	// TODO: Add your control notification handler code here
//	if(!theApp.m_Mv400.IsInitOK())
//	{
//		return;
//	}
//	if(theApp.m_Mv400.GetOutput(theApp.m_tSysParam.outRotatingLight))
//	{
//		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outRotatingLight,FALSE);
//	}
//	else
//	{
//		theApp.m_Mv400.SetOutput(theApp.m_tSysParam.outRotatingLight,TRUE);
//	}
//}






//void CDlgPadDetect::OnBnClickedBtnFirstpixselSave()
//{
//	// TODO: Add your control notification handler code here
//	//if(AfxMessageBox("ROI是否已经锁定第一颗 ？",MB_YESNO)==IDYES)
//	//{
//	//	OnBnClickedBtnPadDetectSearch();
//	//	if(milApp.m_stPadResult.nResultNum>0)
//	//	{
//	//		theApp.m_tSysParam.BmpPadDetectFirstPixselX=milApp.m_stPadResult.dResultCenterX[0];
//	//		theApp.m_tSysParam.BmpPadDetectFirstPixselY=milApp.m_stPadResult.dResultCenterY[0];
//	//		CString str;
//	//		str.Format("%.3f",theApp.m_tSysParam.BmpPadDetectFirstPixselX);
//	//		SetDlgItemText(IDC_FIRST_PIXSEL_X,str);
//	//		str.Format("%.3f",theApp.m_tSysParam.BmpPadDetectFirstPixselY);
//	//		SetDlgItemText(IDC_FIRST_PIXSEL_Y,str);
//	//		theApp.BmpParam(FALSE);
//	//	}
//	//	else
//	//	{
//	//		theApp.m_tSysParam.BmpPadDetectFirstPixselX=0;
//	//		theApp.m_tSysParam.BmpPadDetectFirstPixselY=0;
//	//		MessageBox(_T("首点像素坐标计算失败！"));
//	//	}
//	//}
//	CDlgDetectBase dlgBasePoint;
//	if (dlgBasePoint.DoModal()==IDOK)
//	{
//		CString str;
//		str.Format("%.3f",theApp.m_tSysParam.BmpPadDetectFirstPixselX);
//		SetDlgItemText(IDC_FIRST_PIXSEL_X,str);
//		str.Format("%.3f",theApp.m_tSysParam.BmpPadDetectFirstPixselY);
//		SetDlgItemText(IDC_FIRST_PIXSEL_Y,str);
//		theApp.BmpParam(FALSE);
//	}
//
//}




void CDlgPadDetect::InitImagePaint()
{
	m_stPadPicture.m_modtype = MOD_RECTANGLE;
	m_stPadPicture.m_iImgWidth = m_nImageWidth;
	m_stPadPicture.m_iImgHeight = m_nImageHeight;
	m_stPadPicture.SetDisplayMarkImage(false); //change data source before paint image;
	m_stPadPicture.SetDisplayNeedleImage(false);
	m_stPadPicture.yOffset=0;
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetMatchMode(GRAYSCALE);
	m_stPadPicture.m_pImgBuffer = new BYTE[m_stPadPicture.m_iImgWidth*m_stPadPicture.m_iImgHeight];
}

void CDlgPadDetect::DTest()
{           
	//更新首点的像素坐标//每次固定的首颗的像素坐标值//规避首颗本身异常的误判
	//此处应用支架的偏移量来修正首点的偏移量；
	int nThirdNum = g_pFrm->m_CmdRun.ProductParam.ThirdDispRow*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;
	int nSecondNum = g_pFrm->m_CmdRun.ProductParam.SecondDispRow*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn;
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
						for(int l=0;l<g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn;l++)//Right to left;
						{
							double dPixelX,dPixelY;
							int temID = l + k*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumn + n*nThirdNum + m*g_pFrm->m_CmdRun.ProductParam.SecondDispColumn*nThirdNum +
								j*nSecondNum*nThirdNum + i*g_pFrm->m_CmdRun.ProductParam.FirstDispColumn*nSecondNum*nThirdNum;
							//当前颗粒与首颗的相对距离：mm
							double dDistanceX = j*g_pFrm->m_CmdRun.ProductParam.FirstDispColumnD + n*g_pFrm->m_CmdRun.ProductParam.SecondDispColumnD +
								l*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD;
							double dDistanceY = i*g_pFrm->m_CmdRun.ProductParam.FirstDispRowD + m*g_pFrm->m_CmdRun.ProductParam.SecondDispRowD +
								k*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD;
							//相对距离：mm-->pixsel


							long lTempX,lTempY;
							CRect rect;
							//颗粒理论中心:最后实际中心位置////////
							lTempX=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[0] - dDistanceX;//on the left,X decrease;
							lTempY=g_pFrm->m_CmdRun.ProductParam.DispenseStartPos[1] + dDistanceY;//on the bottom:Y increased;
							//刷新搜索区域：
							rect.left =lTempX - (1.0*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD/2);
							rect.right = lTempX +  (1.0*g_pFrm->m_CmdRun.ProductParam.ThirdDispColumnD/2);
							rect.top = lTempY - (1.0*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD/2);
							rect.bottom = lTempY +(1.0*g_pFrm->m_CmdRun.ProductParam.ThirdDispRowD/2);

							//根据压缩比显示搜索区域：
							int left = int((rect.left)*m_stPadPicture.m_dScale);
							int top = int((rect.top)*m_stPadPicture.m_dScale);
							int width = int(rect.Width()*m_stPadPicture.m_dScale);
							int height = int(rect.Height()*m_stPadPicture.m_dScale);
							CDC * pDC=m_stPadPicture.GetDC();
							pDC->Draw3dRect(left,top,width,height,RGB(0,0,255),RGB(0,0,255)); //绘制新的搜索区域				
							ReleaseDC(pDC);
						}
					}
				}
			}
		}
	}

}


void CDlgPadDetect::OnTimer(UINT_PTR nIDEvent)
{
	static bool bRun=false;
	if (bRun)
	{
		return;
	}
	bRun=true;
	if (!g_pView->m_ImgStatic.DetectSnap())
	{
        g_pView->m_ImgStatic.DetectCamReConnect();
		Sleep(3000);
	}
	bRun=false;
	CDialog::OnTimer(nIDEvent);
}


bool CDlgPadDetect::FindBlobCenter(CRect ROIF)
{
	CPoint point1,point2;
	milApp.SetBlobParam(theApp.m_tSysParam.StandardLen,dAreaMin,dAreaMax,1,1,false);
	double dMinX = 1000000;
	double dMinY = 1000000;
	double dMaxX = 0;
	double dMaxY = 0;
	double dCenterX = 0;
	double dCenterY = 0;
	double dHeight,dWidth;
	milApp.FindBlob(ROIF);
	int nFindNum = milApp.BlobResult.nBlobNum;
	CString str;
	str.Format("BlobFindNum:%d",nFindNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(nFindNum>0&&nFindNum<5000)
	{
		for(int i=0;i<nFindNum;i++)
		{
			dWidth = milApp.BlobResult.dBlobXMax[i]-milApp.BlobResult.dBlobXMin[i];
			dHeight = milApp.BlobResult.dBlobYMax[i]-milApp.BlobResult.dBlobYMin[i];
			if(dWidth>dWidthMax||dWidth<dWidthMin||dHeight>dHeightMax||dHeight<dHeightMin)
			{
				continue;
			}
			point1.x = long(milApp.BlobResult.dBlobXMin[i]-10);
			point1.y = long(milApp.BlobResult.dBlobCenterY[i]);
			point2.x = long(milApp.BlobResult.dBlobXMin[i]+10);
			point2.y = long(milApp.BlobResult.dBlobCenterY[i]);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);
			point1.x = long(milApp.BlobResult.dBlobXMin[i]);
			point1.y = long(milApp.BlobResult.dBlobCenterY[i]-10);
			point2.x = long(milApp.BlobResult.dBlobXMin[i]);
			point2.y = long(milApp.BlobResult.dBlobCenterY[i]+10);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);                                            //找到后的斑点最左侧十字架...

			if(milApp.BlobResult.dBlobXMin[i]<dMinX)
			{
				dMinX = milApp.BlobResult.dBlobXMin[i];
			}

			point1.x = long(milApp.BlobResult.dBlobXMax[i]-10);
			point1.y = long(milApp.BlobResult.dBlobCenterY[i]);
			point2.x = long(milApp.BlobResult.dBlobXMax[i]+10);
			point2.y = long(milApp.BlobResult.dBlobCenterY[i]);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);
			point1.x = long(milApp.BlobResult.dBlobXMax[i]);
			point1.y = long(milApp.BlobResult.dBlobCenterY[i]-10);
			point2.x = long(milApp.BlobResult.dBlobXMax[i]);
			point2.y = long(milApp.BlobResult.dBlobCenterY[i]);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);                                            //找到后的最右侧十字架

			if(milApp.BlobResult.dBlobXMax[i]>dMaxX)
			{
				dMaxX = milApp.BlobResult.dBlobXMax[i];
			}

			point1.x = long(milApp.BlobResult.dBlobCenterX[i]-10);
			point1.y = long(milApp.BlobResult.dBlobYMin[i]);
			point2.x = long(milApp.BlobResult.dBlobCenterX[i]+10);
			point2.y = long(milApp.BlobResult.dBlobYMin[i]);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);
			point1.x = long(milApp.BlobResult.dBlobCenterX[i]);
			point1.y = long(milApp.BlobResult.dBlobYMin[i]-10);
			point2.x = long(milApp.BlobResult.dBlobCenterX[i]);
			point2.y = long(milApp.BlobResult.dBlobYMin[i]+10);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);                                           //最上侧十字架

			if(milApp.BlobResult.dBlobYMin[i]<dMinY)
			{
				dMinY = milApp.BlobResult.dBlobYMin[i];
			}

			point1.x = long(milApp.BlobResult.dBlobCenterX[i]-10);
			point1.y = long(milApp.BlobResult.dBlobYMax[i]);
			point2.x = long(milApp.BlobResult.dBlobCenterX[i]+10);
			point2.y = long(milApp.BlobResult.dBlobYMax[i]);
			str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
			g_pFrm->m_CmdRun.AddMsg(str);
			//milApp.OverlayLine(2,point1,point2);
			OverlayLine(2,point1,point2);
			point1.x = long(milApp.BlobResult.dBlobCenterX[i]);
			point1.y = long(milApp.BlobResult.dBlobYMax[i]-10);
			point2.x = long(milApp.BlobResult.dBlobCenterX[i]);
			point2.y = long(milApp.BlobResult.dBlobYMax[i]+10);

			if(milApp.BlobResult.dBlobYMax[i]>dMaxY)
			{
				dMaxY = milApp.BlobResult.dBlobYMax[i];
			}
		}                                                                    //最下侧十字架
		dCenterX = (dMinX+dMaxX)/2;
		/*if(dCenterX>744)
		{
		return false;
		}*/
		dCenterY = (dMinY+dMaxY)/2;
		/*if(dCenterY>480)
		{
		return false;
		}*/

		point1.x = long(dCenterX-10);
		point1.y = long(dCenterY);
		point2.x = long(dCenterX+10);
		point2.y = long(dCenterY);
		str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
		g_pFrm->m_CmdRun.AddMsg(str);
		//milApp.OverlayLine(2,point1,point2);
		OverlayLine(2,point1,point2);
		point1.x = long(dCenterX);
		point1.y = long(dCenterY-10);
		point2.x = long(dCenterX);
		point2.y = long(dCenterY+10);
		str.Format("Overlay1:%d,%d,%d,%d",point1.x,point1.y,point2.x,point2.y);
		g_pFrm->m_CmdRun.AddMsg(str);
		//milApp.OverlayLine(2,point1,point2);
		OverlayLine(2,point1,point2);   //图像中心画十字架...
		return true;
	}
	return false;
}
void CDlgPadDetect::OverlayLine(int nColor,CPoint p1, CPoint p2)
{
	CPen pen(PS_SOLID, 5, RGB(255,0,0));
	CPen *oldPen = GetDC()->SelectObject(&pen);
	if(nColor==2)
	{
		GetDC()->SelectObject(oldPen);
		pen.DeleteObject();
		pen.CreatePen(PS_SOLID,5,RGB(0,255,0));
	}
	if(theApp.m_tSysParam.nSubstrateType==0)
	{
		GetDC()->MoveTo(p1.x/2,p1.y/2);
		GetDC()->LineTo(p2.x/2,p2.y/2);
	}
	else
	{
		GetDC()->MoveTo(p1);
		GetDC()->MoveTo(p2);
	}
	GetDC()->SelectObject(oldPen);
	pen.DeleteObject();
}

void CDlgPadDetect::OnCbnSelchangeComboImgselect()
{
	nSelectID = m_cbPadSelect.GetCurSel();
    UpdateUI();	
}
