// DlgBlobSet.cpp : implementation file
//

#include "stdafx.h"
#include "TSCtrlSys.h"
#include "DlgBlobSet.h"


// CDlgBlobSet dialog

IMPLEMENT_DYNAMIC(CDlgBlobSet, CDialog)

CDlgBlobSet::CDlgBlobSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBlobSet::IDD, pParent)
{
	lImageLeft = 0;
	lImageTop = 0;
	lImageWidth = milApp.m_lPadBufSizeX;
	lImageHeight = milApp.m_lPadBufSizeY;
	//lImageWidth = milApp.m_lBufSizeX;
	//lImageHeight = milApp.m_lBufSizeY;
}

CDlgBlobSet::~CDlgBlobSet()
{
}

void CDlgBlobSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_BLOB_PIC,m_picture);
	DDX_Control(pDX,IDC_COMBO_BLOB,m_cbSelect);
}


BEGIN_MESSAGE_MAP(CDlgBlobSet, CDialog)
	//ON_WM_MOUSEMOVE()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()
	ON_CBN_SELCHANGE(IDC_COMBO_BLOB, &CDlgBlobSet::OnCbnSelchangeComboBlob)
	ON_BN_CLICKED(IDOK, &CDlgBlobSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgBlobSet::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_BLOB_COM_SCALE, &CDlgBlobSet::OnBnClickedBtnBlobComScale)
	ON_BN_CLICKED(IDC_BTN_BLOB_NEW_PIC, &CDlgBlobSet::OnBnClickedBtnBlobNewPic)
	ON_BN_CLICKED(IDC_BTN_BLOB_SAVE_ROI, &CDlgBlobSet::OnBnClickedBtnBlobSaveRoi)
	ON_BN_CLICKED(IDC_BTN_BLOB_SAVE_CENTER, &CDlgBlobSet::OnBnClickedBtnBlobSaveCenter)
	ON_BN_CLICKED(IDC_BTN_BLOB_TEST, &CDlgBlobSet::OnBnClickedBtnBlobTest)
	ON_BN_CLICKED(IDC_BTN_BLOB_MOVE_GRABPOS, &CDlgBlobSet::OnBnClickedBtnBlobMoveGrabpos)
	ON_BN_CLICKED(IDC_BTN_BLOB_SHOW_ROI, &CDlgBlobSet::OnBnClickedBtnBlobShowRoi)
	ON_BN_CLICKED(IDC_BTN_BLOB_CLEAR_ROI, &CDlgBlobSet::OnBnClickedBtnBlobClearRoi)
	ON_EN_CHANGE(IDC_EDIT_BLOB_ROI_LEFT, &CDlgBlobSet::OnEnChangeEditBlobRoiLeft)
	ON_EN_CHANGE(IDC_EDIT_BLOB_ROI_RIGHT, &CDlgBlobSet::OnEnChangeEditBlobRoiRight)
	ON_EN_CHANGE(IDC_EDIT_BLOB_ROI_TOP, &CDlgBlobSet::OnEnChangeEditBlobRoiTop)
	ON_EN_CHANGE(IDC_EDIT_BLOB_ROI_BOTTOM, &CDlgBlobSet::OnEnChangeEditBlobRoiBottom)
END_MESSAGE_MAP()


// CDlgBlobSet message handlers
BOOL CDlgBlobSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	if(milApp.m_MilImage!=NULL)
	{
		m_picture.MoveWindow(0,0,lImageWidth,lImageHeight,true);
		milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_BLOB_PIC)->m_hWnd);
		bPatLearn = true;
		bFirstROI = true;
		//bGetROI = true;
	}
	m_cbSelect.AddString("点胶起点图像参数设定");
	m_cbSelect.AddString("点胶终点图像参数设定");
	m_cbSelect.SetCurSel(0);
	nSelectID = m_cbSelect.GetCurSel();
	OnCbnSelchangeComboBlob();
	RefreshPosition();
	str.Format("%d",theApp.m_tSysParam.StandardLen);
	SetDlgItemText(IDC_EDIT_BLOB_THRESHOLD,str);
	str.Format("%.3f",theApp.m_tSysParam.BmpScale);
	SetDlgItemText(IDC_EDIT_BLOB_SCALE,str);
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
	SetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_AREA,str);
	str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].TopLeft().x);
	SetDlgItemText(IDC_EDIT_BLOB_ROI_LEFT,str);
	str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].TopLeft().y);
	SetDlgItemText(IDC_EDIT_BLOB_ROI_TOP,str);
	str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].BottomRight().x);
	SetDlgItemText(IDC_EDIT_BLOB_ROI_RIGHT,str);
	str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].BottomRight().y);
	SetDlgItemText(IDC_EDIT_BLOB_ROI_BOTTOM,str);
	ROI = theApp.m_tSysParam.BmpROI[nSelectID];

	return TRUE;
}

void CDlgBlobSet::OnCbnSelchangeComboBlob()
{
	// TODO: Add your control notification handler code here
	CString str;
	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();
	tgPos tgEndPos = g_pFrm->m_CmdRun.GetLastCameraPos();
	nSelectID = m_cbSelect.GetCurSel();

	if(nSelectID<1)
	{
		str.Format("%.3f",tgStartPos.x);
		SetDlgItemText(IDC_EDIT_BLOB_IMAGE_POS_X,str);
		str.Format("%.3f",tgStartPos.y);
		SetDlgItemText(IDC_EDIT_BLOB_IMAGE_POS_Y,str);
		str.Format("%.3f",tgStartPos.za);
		SetDlgItemText(IDC_EDIT_BLOB_IMAGE_POS_Z,str);
	}
	else
	{
		str.Format("%.3f",tgEndPos.x);
		SetDlgItemText(IDC_EDIT_BLOB_IMAGE_POS_X,str);
		str.Format("%.3f",tgEndPos.y);
		SetDlgItemText(IDC_EDIT_BLOB_IMAGE_POS_Y,str);
		str.Format("%.3f",tgEndPos.za);
		SetDlgItemText(IDC_EDIT_BLOB_IMAGE_POS_Z,str);
	}

	if((nSelectID==1)||(nSelectID==0))
	{
		str.Format("%.3f",theApp.m_tSysParam.BmpCenterX[nSelectID]);
		SetDlgItemText(IDC_EDIT_BLOB_PARAM_POS_X,str);
		str.Format("%.3f",theApp.m_tSysParam.BmpCenterY[nSelectID]);
		SetDlgItemText(IDC_EDIT_BLOB_PARAM_POS_Y,str);
		str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].TopLeft().x);
		SetDlgItemText(IDC_EDIT_BLOB_ROI_LEFT,str);
		str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].TopLeft().y);
		SetDlgItemText(IDC_EDIT_BLOB_ROI_TOP,str);
		str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].BottomRight().x);
		SetDlgItemText(IDC_EDIT_BLOB_ROI_RIGHT,str);
		str.Format("%d",theApp.m_tSysParam.BmpROI[nSelectID].BottomRight().y);
		SetDlgItemText(IDC_EDIT_BLOB_ROI_BOTTOM,str);
		ROI = theApp.m_tSysParam.BmpROI[nSelectID];
	}
}

void CDlgBlobSet::RefreshPosition()
{
	CString str;
	double dTempPosition;
	for(int i=0;i<3;i++)
	{
		dTempPosition = theApp.m_Mv400.GetPos(K_AXIS_X+i);
		str.Format("%.3f",dTempPosition);
		SetDlgItemText(IDC_EDIT_BLOB_CURRENT_POS_X+i,str);
	}
}

void CDlgBlobSet::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str;

	GetDlgItemText(IDC_EDIT_BLOB_THRESHOLD,str);
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
	theApp.m_tSysParam.BmpAreaMax = atof(str);
	theApp.BmpParam(FALSE);
	OnOK();
}

void CDlgBlobSet::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgBlobSet::OnBnClickedBtnBlobComScale()
{
	// TODO: Add your control notification handler code here
	CString str;
	double dTempCenter;
	GetDlgItemText(IDC_EDIT_BLOB_POS_X,str);
	dTempCenter = atof(str);

	CRect ROI(0,0,milApp.m_lBufSizeX-1,milApp.m_lBufSizeY-1);
	if(ROI.IsRectEmpty()||ROI.IsRectNull())
	{
		MessageBox("请设定图像的ROI区域！");
		return;
	}

	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();
	tgPos tgEndPos = g_pFrm->m_CmdRun.GetLastCameraPos();

	if(nSelectID<1)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x,tgStartPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
		{
			return;
		}
		if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgStartPos.za,true))
		{
			return;
		}
	}
	else
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x,tgEndPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
		{
			return;
		}
		if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgEndPos.za,true))
		{
			return;
		}
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	OnBnClickedBtnBlobNewPic();
	if(FindBlobCenter(ROI))
	{
		dTempCenter = dCenterX;
	}
	else
	{
		AfxMessageBox("查找中心失败！");
	}
	if(nSelectID<1)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x+1.5,tgStartPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x+1.5,tgStartPos.y,true))
		{
			return;
		}
	}
	else
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x+1.5,tgEndPos.y,g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x+1.5,tgEndPos.y,true))
		{
			return;
		}
	}
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	OnBnClickedBtnBlobNewPic();
	if(FindBlobCenter(ROI))
	{
		theApp.m_tSysParam.BmpScale = 1.5/(dTempCenter-dCenterX);
	}
	else
	{
		AfxMessageBox("查找中心失败！");
	}
	str.Format("%.3f",theApp.m_tSysParam.BmpScale);
	SetDlgItemText(IDC_EDIT_BLOB_SCALE,str);
}

void CDlgBlobSet::OnBnClickedBtnBlobNewPic()
{
	// TODO: Add your control notification handler code here
	//if(!g_pView->m_ImgStatic.CopyImage(RGB8))
	/*if(!g_pView->m_ImgStatic.CopyDetectImage(RGB8))
	{
		AfxMessageBox("抓取图像失败！");
		return;
	}*/
	milApp.DisSelectWindow(GetDlgItem(IDC_STATIC_BLOB_PIC)->m_hWnd);
}

bool CDlgBlobSet::FindBlobCenter(CRect ROIF)
{
	CPoint point1,point2;
	milApp.SetBlobParam(theApp.m_tSysParam.StandardLen,dAreaMin,dAreaMax,1,1,false);
	dMinX = 1000000;
	dMinY = 1000000;
	dMaxX = 0;
	dMaxY = 0;
	dCenterX = 0;
	dCenterY = 0;
	double dHeight,dWidth;
	milApp.FindBlob(ROIF);
	int nFindNum = milApp.BlobResult.nBlobNum;
	CString str;
	str.Format("FindNum:%d",nFindNum);
	g_pFrm->m_CmdRun.AddMsg(str);
	if(nFindNum>0&&nFindNum<20)
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
		if(dCenterX>744)
		{
			return false;
		}
		dCenterY = (dMinY+dMaxY)/2;
		if(dCenterY>480)
		{
			return false;
		}

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

void CDlgBlobSet::OnBnClickedBtnBlobSaveRoi()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_ROI_LEFT,str);
	theApp.m_tSysParam.BmpROI[nSelectID].TopLeft().x = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_ROI_TOP,str);
	theApp.m_tSysParam.BmpROI[nSelectID].TopLeft().y = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_ROI_RIGHT,str);
	theApp.m_tSysParam.BmpROI[nSelectID].BottomRight().x = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_ROI_BOTTOM,str);
	theApp.m_tSysParam.BmpROI[nSelectID].BottomRight().y = atoi(str);
	theApp.BmpParam(false);
}

void CDlgBlobSet::OnBnClickedBtnBlobSaveCenter()
{
	// TODO: Add your control notification handler code here
	CString str;
	if(nSelectID<1)
	{
		theApp.m_tSysParam.BmpCenterX[0] = dCenterX;
		theApp.m_tSysParam.BmpCenterY[0] = dCenterY;
	}
	else
	{
		theApp.m_tSysParam.BmpCenterX[1] = dCenterX;
		theApp.m_tSysParam.BmpCenterY[1] = dCenterY;
	}

	GetDlgItemText(IDC_EDIT_BLOB_THRESHOLD,str);
	theApp.m_tSysParam.StandardLen=atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_LENGTH,str);
	theApp.m_tSysParam.BmpWidthMin=atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_LENGTH,str);
	theApp.m_tSysParam.BmpWidthMax=atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_WIDTH,str);
	theApp.m_tSysParam.BmpHeighMin=atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_WIDTH,str);
	theApp.m_tSysParam.BmpHeighMax=atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_AREA,str);
	theApp.m_tSysParam.BmpAreaMin=atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_AREA,str);
	theApp.m_tSysParam.BmpAreaMax=atof(str);
	theApp.BmpParam(FALSE);
}

void CDlgBlobSet::OnBnClickedBtnBlobTest()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBtnBlobNewPic();
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_THRESHOLD,str);
	theApp.m_tSysParam.StandardLen = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_LENGTH,str);
	dWidthMin = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_LENGTH,str);
	dWidthMax = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_WIDTH,str);
	dHeightMin = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_WIDTH,str);
	dHeightMax = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MIN_AREA,str);
	dAreaMin = atof(str);
	GetDlgItemText(IDC_EDIT_BLOB_PARAM_MAX_AREA,str);
	dAreaMax = atof(str);

	ROI = theApp.m_tSysParam.BmpROI[nSelectID];

	if(ROI.IsRectEmpty()||ROI.IsRectNull())
	{
		ROI.left = 10;
		ROI.right = 600;
		ROI.top = 10;
		ROI.bottom = 450;
	}
	if(FindBlobCenter(ROI))
	{
		str.Format("%.3f",dCenterX);
		SetDlgItemText(IDC_EDIT_BLOB_PARAM_POS_X,str);
		str.Format("%.3f",dCenterY);
		SetDlgItemText(IDC_EDIT_BLOB_PARAM_POS_Y,str);
	}
}

void CDlgBlobSet::OnBnClickedBtnBlobMoveGrabpos()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		return;
	}
	tgPos tgStartPos = g_pFrm->m_CmdRun.GetFirstCameraPos();
	tgPos tgEndPos = g_pFrm->m_CmdRun.GetLastCameraPos();
	if(nSelectID<1)
	{
		//theApp.m_Mv400.MoveXY(tgStartPos.x,tgStartPos.y,
		//	g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgStartPos.x,tgStartPos.y,true))
		{
			return;
		}
		if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgStartPos.za,true))
		{
			return;
		}
	}
	else
	{
		//theApp.m_Mv400.MoveXY(tgEndPos.x,tgEndPos.y,
		//	g_pFrm->m_CmdRun.ProductParam.FreeVal,g_pFrm->m_CmdRun.ProductParam.FreeAcc,TRUE);
		if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(tgEndPos.x,tgEndPos.y,true))
		{
			return;
		}
		if(!g_pFrm->m_CmdRun.SynchronizeMoveZ(tgEndPos.za,true))
		{
			return;
		}
	}
	theApp.m_Mv400.WaitStop();
	RefreshPosition();
	Sleep(DWORD(g_pFrm->m_CmdRun.ProductParam.dGrabImageDelay*1000));
	OnBnClickedBtnBlobNewPic();
}

void CDlgBlobSet::OverlayLine(int nColor,CPoint p1, CPoint p2)
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
void CDlgBlobSet::OnBnClickedBtnBlobShowRoi()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_ROI_LEFT,str);
	int nLeft = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_ROI_TOP,str);
	int nRight = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_ROI_RIGHT,str);
	int nTop = atoi(str);
	GetDlgItemText(IDC_EDIT_BLOB_ROI_BOTTOM,str);
	int nBottom = atoi(str);
	
	GetDC()->Draw3dRect(nLeft,nTop,nRight-nLeft,nBottom-nTop,RGB(255,0,0),RGB(255,0,0));
}

void CDlgBlobSet::OnBnClickedBtnBlobClearRoi()
{
	// TODO: Add your control notification handler code here
	Invalidate();
}

void CDlgBlobSet::OnEnChangeEditBlobRoiLeft()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_ROI_LEFT,str);
	ROI.TopLeft().x = atoi(str);
}

void CDlgBlobSet::OnEnChangeEditBlobRoiRight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_ROI_RIGHT,str);
	ROI.BottomRight().x = atoi(str);
}

void CDlgBlobSet::OnEnChangeEditBlobRoiTop()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_ROI_TOP,str);
	ROI.TopLeft().y = atoi(str);
}

void CDlgBlobSet::OnEnChangeEditBlobRoiBottom()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_BLOB_ROI_BOTTOM,str);
	ROI.BottomRight().y = atoi(str);
}