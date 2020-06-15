// DlgDetectBase.cpp : implementation file
//

#include "stdafx.h"
#include "DlgDetectBase.h"
#include "TSCtrlSys.h"


// CDlgDetectBase dialog

IMPLEMENT_DYNAMIC(CDlgDetectBase, CDialog)

CDlgDetectBase::CDlgDetectBase(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDetectBase::IDD, pParent)
{
	m_nImageWidth = IMAGEWIDTH1;
	m_nImageHeight = IMAGEHEIGHT1;

}

CDlgDetectBase::~CDlgDetectBase()
{
}

void CDlgDetectBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DETECT_BASEPOINT_PICTURE, m_picture);
}


BEGIN_MESSAGE_MAP(CDlgDetectBase, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgDetectBase::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DETECT_BASEPOINT_NEXT, &CDlgDetectBase::OnBnClickedBtnDetectBasepointNext)
END_MESSAGE_MAP()


// CDlgDetectBase message handlers

void CDlgDetectBase::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CDlgDetectBase::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	//MoveWindow(10,10,1260,984);  //主窗体位置
	m_rectModelWin = theApp.m_tSysParam.BmpPadDetectLearnWin[0];
	CRect rect(0,0,m_rectModelWin.Width(),m_rectModelWin.Height());
	m_rectROI = rect;
	//m_picture.MoveWindow(0,0,m_nImageWidth,m_nImageHeight,true);
	m_picture.m_modtype = MOD_RECTANGLE; //模板类型
	m_picture.m_iImgWidth = m_nImageWidth;//图像宽度
	m_picture.m_iImgHeight = m_nImageHeight;//图像高度
	m_picture.m_pImgBuffer = new BYTE[m_picture.m_iImgWidth*m_picture.m_iImgHeight];
	m_picture.m_mod_rectangle.dx = m_rectModelWin.TopLeft().x + m_rectModelWin.Width()/2;
	m_picture.m_mod_rectangle.dy = m_rectModelWin.TopLeft().y + m_rectModelWin.Height()/2;
	m_picture.m_mod_rectangle.width = m_rectModelWin.Width();
	m_picture.m_mod_rectangle.height = m_rectModelWin.Height();
	m_picture.m_mod_scale.length = 20;
	m_picture.m_mod_scale.width = 10;
	m_picture.m_mod_scale.height = 10;
	m_picture.RoiRect =  m_rectROI;
	m_picture.SetDisplayMarkImage(false);
	m_picture.ImgInit(m_nImageWidth,m_nImageHeight,8);//初始化控件
	
	//m_picture.MoveWindow(0,0,1252,920);
	//GetDlgItem(IDC_EDIT_DETECT_BASEPOINT_PROMPT)->MoveWindow(40,925,100,25);
	//GetDlgItem(IDC_BTN_DETECT_BASEPOINT_NEXT)->MoveWindow(180,925,40,25);
	//GetDlgItem(IDOK)->MoveWindow(260,925,40,25);

	CString strFile = g_pDoc->GetPadModelFile(0);//Load Mark File;
	//milApp.SetMatchMode(g_pFrm->m_CmdRun.ProductParam.matchMode);
	milApp.SetPadModelWindow(m_rectModelWin);
	milApp.SetPadSearchWindow(m_rectROI);
	milApp.RestorePadModel(strFile.GetBuffer(strFile.GetLength()));
	CString str;
	str.Format("请移动蓝框到右上角第一颗料！");
	SetDlgItemText(IDC_EDIT_DETECT_BASEPOINT_PROMPT,str);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetTimer(0,300,NULL);

	return TRUE;  
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDetectBase::OnBnClickedBtnDetectBasepointNext()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_rectROI = m_picture.RoiRect;
	str.Format("left = %d,right = %d,top = %d,bottom = %d",m_rectROI.left,m_rectROI.right,m_rectROI.top,m_rectROI.bottom);
	g_pFrm->m_CmdRun.AddMsg(str);
	milApp.SetPadSearchWindow(m_rectROI);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	if(!milApp.FindPadModels(true))
	{
		g_pFrm->m_CmdRun.AddMsg("首颗查找定位失败！");
		CString str;
		str.Format("查找失败，请检查相关条件！");
		SetDlgItemText(IDC_EDIT_DETECT_BASEPOINT_PROMPT,str);
		return;
	}
	if(milApp.m_stPadResult.nResultNum>=1)
	{
		g_pFrm->m_CmdRun.AddMsg("首颗查找定位成功!");
		CString str;
		str.Format("首颗定位成功！");
		SetDlgItemText(IDC_EDIT_DETECT_BASEPOINT_PROMPT,str);
		theApp.m_tSysParam.BmpPadDetectFirstPixselX = milApp.m_stPadResult.dResultCenterX[0];
		theApp.m_tSysParam.BmpPadDetectFirstPixselY = milApp.m_stPadResult.dResultCenterY[0];		
	}
}
