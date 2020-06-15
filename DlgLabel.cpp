// DlgLabel.cpp : implementation file
//

#include "stdafx.h"
#include "DlgLabel.h"
#include "TSCtrlSys.h"
#include <math.h>

// CDlgLabel dialog

IMPLEMENT_DYNAMIC(CDlgLabel, CDialog)

CDlgLabel::CDlgLabel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLabel::IDD, pParent)
{

}

CDlgLabel::~CDlgLabel()
{
}

void CDlgLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLabel, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LABEL_SPEED, &CDlgLabel::OnNMCustomdrawSliderLabelSpeed)
	ON_BN_CLICKED(IDC_BTN_LABEL_NEXT, &CDlgLabel::OnBnClickedBtnLabelNext)
	ON_BN_CLICKED(IDOK, &CDlgLabel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLabel::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_ZTOFOCUS, &CDlgLabel::OnBnClickedBtnZtofocus)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_ONE, &CDlgLabel::OnBnClickedBtnNeedleOne)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_TWO, &CDlgLabel::OnBnClickedBtnNeedleTwo)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_THIRD, &CDlgLabel::OnBnClickedBtnNeedleThird)
	ON_BN_CLICKED(IDC_BTN_CEMERA, &CDlgLabel::OnBnClickedBtnCemera)
	ON_BN_CLICKED(IDC_BTN_LASER, &CDlgLabel::OnBnClickedBtnLaser)
	ON_BN_CLICKED(IDC_BTN_LASER_DATAREAD, &CDlgLabel::OnBnClickedBtnLaserDataread)
END_MESSAGE_MAP()


// CDlgLabel message handlers
BOOL CDlgLabel::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i=0;i<10;i++)
	{
		m_colorButton[i].m_iIndex = i;
		m_colorButton[i].SubclassDlgItem(IDC_BTN_LABEL_X_NEG+i,this);
		m_colorButton[i].SetBitmapEx(IDB_BITMAP_MOVE);
		m_colorButton[i].m_pMv = &theApp.m_Mv400;
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LABEL_SPEED))->SetRange(1,2000);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LABEL_SPEED))->SetPos(1);
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LABEL_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;

	m_pBitmapInfo = (BITMAPINFO *)(new char[sizeof(BITMAPINFOHEADER)+256*4]);
	m_pBitmapInfo->bmiHeader.biBitCount = (UINT16)(24);

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biWidth = IMAGEWIDTH0;
	m_pBitmapInfo->bmiHeader.biHeight = IMAGEHEIGHT0;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;

	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = IMAGEWIDTH0*IMAGEHEIGHT0*3;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	m_pDC = GetDlgItem(IDC_STATIC_LABEL_PICTURE)->GetDC();
	m_nIndex = 0;
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	CString str;
	str.Format("%0.3f",g_pFrm->m_CmdRun.ProductParam.HeightStandardValue);
	SetDlgItemText(IDC_EDIT_LABEL_HEIGHT_STANDARD_VALUE,str);
	GetDlgItem(IDC_BTN_ZTOFOCUS)->EnableWindow(FALSE);
	BackUpHistoryData();
	MoveAdjustMotorToZero();
	SetTimer(0,200,NULL);
	return TRUE;
}

void CDlgLabel::OnNMCustomdrawSliderLabelSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LABEL_SPEED))->GetPos();
	CColorButton::m_iSpeedNo = n;
	CString strTemp;
	strTemp.Format("X:%.4f,Y:%.4f,ZA:%.4f,ZB:%.4f,ZC:%.4f", theApp.m_tSysParam.tAxis[0].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[1].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[2].vel[0] * n/2000.0,
		theApp.m_tSysParam.tAxis[3].vel[0] * n/2000.0,theApp.m_tSysParam.tAxis[4].vel[0] * n/2000.0);
	SetDlgItemText(IDC_EDIT_LABEL_SPEED, strTemp);
	*pResult = 0;
}

void CDlgLabel::OnTimer(UINT_PTR nIDEvent)
{
	UpdatePrompt();
	UpdateUI();
	Paint();
	CDialog::OnTimer(nIDEvent);
}

void CDlgLabel::Paint()
{
	if(g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer==NULL)
	{
		return;
	}
	HDRAWDIB m_hDrawDib = DrawDibOpen();
	m_pDC->SetBkMode(TRANSPARENT);
	DrawDibDraw(m_hDrawDib,m_pDC->GetSafeHdc(),0,0,IMAGEWIDTH0,IMAGEHEIGHT0,
		&m_pBitmapInfo->bmiHeader,g_pView->m_ImgStatic.m_pCamera.m_pImageBuffer,0,0,m_pBitmapInfo->bmiHeader.biWidth,
		m_pBitmapInfo->bmiHeader.biHeight,SRCCOPY);
	DrawCross(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),IMAGEWIDTH0/2,IMAGEHEIGHT0/2);
	PainScale(m_pDC,CPoint(IMAGEWIDTH0/2,IMAGEHEIGHT0/2),4,4,10);
	DrawDibClose(m_hDrawDib);
}

void CDlgLabel::DrawCross(CDC *pDC,CPoint point,int nWidth, int nHeight)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(point.x,point.y-nHeight);
	pDC->LineTo(point.x,point.y+nHeight);
	pDC->MoveTo(point.x-nWidth,point.y);
	pDC->LineTo(point.x+nWidth,point.y);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CDlgLabel::PainScale(CDC *pDC,CPoint point,int nWidth, int nHeight,int nLength)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	for(int i=point.x-nLength;i>0;i=i-nLength)
	{
		pDC->MoveTo(i,point.y-nHeight/2);
		pDC->LineTo(i,point.y+nHeight/2);
		pDC->MoveTo(IMAGEWIDTH0-i,point.y-nHeight/2);
		pDC->LineTo(IMAGEWIDTH0-i,point.y+nHeight/2);
	}
	for(int i=point.y-nLength;i>0;i=i-nLength)
	{
		pDC->MoveTo(point.x-nWidth/2,i);
		pDC->LineTo(point.x+nWidth/2,i);
		pDC->MoveTo(point.x-nWidth/2,IMAGEHEIGHT0-i);
		pDC->LineTo(point.x+nWidth/2,IMAGEHEIGHT0-i);
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}


void CDlgLabel::UpdateUI()
{
	CString str;
	double dValue = 0.0;
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_X));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_X,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_Y));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_Y,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_ZA));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_ZA,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_ZB));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_ZB,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_ZC));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_ZC,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_A));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_A,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_B));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_B,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_C));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_C,str);
	str.Format("%0.4f",theApp.m_Mv400.GetPos(K_AXIS_D));
	SetDlgItemText(IDC_EDIT_LABEL_CURR_POS_D,str);
	if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dValue))
	{
		str.Format("%0.3f",dValue);
		SetDlgItemText(IDC_EDIT_LABEL_HEIGHT_CURR_VALUE,str);
	}
}
void CDlgLabel::UpdatePrompt()
{
	CString str;
	if(0 == m_nIndex)
	{
		str.Format("�궨1#��ͷ���뽫1#��ͷ�ƶ����궨λ...");
		SetDlgItemText(IDC_EDIT_LABEL_PROMPT,str);
		GetDlgItem(IDC_BTN_NEEDLE_ONE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_NEEDLE_TWO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_THIRD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CEMERA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER_DATAREAD)->EnableWindow(FALSE);		
	}
	else if(1 == m_nIndex)
	{
		str.Format("�궨2#��ͷ���뽫2#��ͷ�ƶ����궨λ...");
		SetDlgItemText(IDC_EDIT_LABEL_PROMPT,str);
		GetDlgItem(IDC_BTN_NEEDLE_ONE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_TWO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_NEEDLE_THIRD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CEMERA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER_DATAREAD)->EnableWindow(FALSE);	
	}
	else if(2 == m_nIndex)
	{
		str.Format("�궨3#��ͷ���뽫3#��ͷ�ƶ����궨λ...");
		SetDlgItemText(IDC_EDIT_LABEL_PROMPT,str);
		GetDlgItem(IDC_BTN_NEEDLE_ONE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_TWO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_THIRD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CEMERA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER_DATAREAD)->EnableWindow(FALSE);	
	}
	else if(3 == m_nIndex)
	{
		str.Format("�궨������뽫����ƶ����궨λ...");
		SetDlgItemText(IDC_EDIT_LABEL_PROMPT,str);
		GetDlgItem(IDC_BTN_ZTOFOCUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_NEEDLE_ONE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_TWO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_THIRD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CEMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_LASER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER_DATAREAD)->EnableWindow(FALSE);	

	}
	else if(4 == m_nIndex)
	{
		GetDlgItem(IDC_BTN_ZTOFOCUS)->EnableWindow(FALSE);
		str.Format("�궨��ߴ��������뽫�������ƶ����궨λ...");
		SetDlgItemText(IDC_EDIT_LABEL_PROMPT,str);
		GetDlgItem(IDC_BTN_ZTOFOCUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_ONE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_TWO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_THIRD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CEMERA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_LASER_DATAREAD)->EnableWindow(FALSE);	

	}
	else if(5 == m_nIndex)
	{
		GetDlgItem(IDC_BTN_ZTOFOCUS)->EnableWindow(FALSE);
		str.Format("������λ�ã��뽫�������ƶ������λ...");
		SetDlgItemText(IDC_EDIT_LABEL_PROMPT,str);
		GetDlgItem(IDC_BTN_ZTOFOCUS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_ONE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_TWO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_NEEDLE_THIRD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CEMERA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_LASER_DATAREAD)->EnableWindow(TRUE);

	}
}
void CDlgLabel::Close()
{
	g_pFrm->m_CmdRun.MoveToZSafety();
	if(AfxMessageBox("��������ƶ�����¼λ�ã�",MB_YESNO)==IDYES)
	{
		g_pFrm->m_CmdRun.AdjustNeedle();
	}
	KillTimer(0);
	if(m_pBitmapInfo!=NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
	if(m_pDC!=NULL)
	{
		ReleaseDC(m_pDC);
	}
	theApp.ProductParam(FALSE);
}
void CDlgLabel::OnBnClickedBtnLabelNext()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theApp.m_Mv400.IsInitOK())
	{
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].x = theApp.m_Mv400.GetPos(K_AXIS_X);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].zb = theApp.m_Mv400.GetPos(K_AXIS_ZB);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].zc = theApp.m_Mv400.GetPos(K_AXIS_ZC);
	}
	g_pFrm->m_CmdRun.MoveToZSafety();

	m_nIndex++;
	if(5 <= m_nIndex)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_LABEL_NEXT)->EnableWindow(FALSE);
	}
}

void CDlgLabel::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theApp.m_Mv400.IsInitOK())
	{
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].x = theApp.m_Mv400.GetPos(K_AXIS_X);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].y = theApp.m_Mv400.GetPos(K_AXIS_Y);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].za = theApp.m_Mv400.GetPos(K_AXIS_ZA);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].zb = theApp.m_Mv400.GetPos(K_AXIS_ZB);
		g_pFrm->m_CmdRun.ProductParam.tgLabelPos[m_nIndex].zc = theApp.m_Mv400.GetPos(K_AXIS_ZC);
		double dHeightValue = 0.0;
		if(g_pFrm->m_CmdRun.ReadHeightSensorValue(&dHeightValue))  //��ȡ���⴫�����ڽӴ��������Ϸ�ʱ��ֵ��
		{
			if(fabs(dHeightValue)<4)
			{
				//g_pFrm->m_CmdRun.ProductParam.HeightStandardValue = dHeightValue;
			}
		}
	}
	Close();
	OnOK();
}

void CDlgLabel::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Close();
	OnCancel();
}

void CDlgLabel::MoveAdjustMotorToZero()
{
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		g_pFrm->m_CmdRun.AddMsg("Z���ƶ�����ȫλ��ʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		g_pFrm->m_CmdRun.AddMsg("��������ƶ�����λʧ�ܣ�");
	}
}

void CDlgLabel::OnBnClickedBtnZtofocus()
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("ZA���ƶ����������λ�ã�",MB_YESNO)==IDYES)
	{
		g_pFrm->m_CmdRun.SynchronizeMoveZ(theApp.m_tSysParam.dFocusZPos,true);

	}
}

void CDlgLabel::OnBnClickedBtnNeedleOne()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
		return;
	}
	//��궨ʱ��0�궨����ȷ��ǰ�ȹ�λ��
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("��������ƶ�ʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(LastLablePos[0].x,LastLablePos[0].y,true))
	{
		AfxMessageBox("�ƶ���1#��ͷ��ʷ�궨λʧ�ܣ�");
		return;
	}

}

void CDlgLabel::OnBnClickedBtnNeedleTwo()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
		return;
	}
	//��궨ʱ��0�궨����ȷ��ǰ�ȹ�λ��
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("��������ƶ�ʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(LastLablePos[1].x,LastLablePos[1].y,true))
	{
		AfxMessageBox("�ƶ���2#��ͷ��ʷ�궨λʧ�ܣ�");
		return;
	}

}

void CDlgLabel::OnBnClickedBtnNeedleThird()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
		return;
	}
	//��궨ʱ��0�궨����ȷ��ǰ�ȹ�λ��
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("��������ƶ�ʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(LastLablePos[2].x,LastLablePos[2].y,true))
	{
		AfxMessageBox("�ƶ���3#��ͷ��ʷ�궨λʧ�ܣ�");
		return;
	}

}

void CDlgLabel::OnBnClickedBtnCemera()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
		return;
	}
	//��궨ʱ��0�궨����ȷ��ǰ�ȹ�λ��
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("��������ƶ�ʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(LastLablePos[3].x,LastLablePos[3].y,true))
	{
		AfxMessageBox("�ƶ��������ʷ�궨λʧ�ܣ�");
		return;
	}

}

void CDlgLabel::OnBnClickedBtnLaser()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
		return;
	}
	//��궨ʱ��0�궨����ȷ��ǰ�ȹ�λ��
	if(!g_pFrm->m_CmdRun.AdjustMotorZero())
	{
		AfxMessageBox("��������ƶ�ʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(LastLablePos[4].x,LastLablePos[4].y,true))
	{
		AfxMessageBox("�ƶ�����������ʷ�궨λʧ�ܣ�");
		return;
	}

}

void CDlgLabel::BackUpHistoryData()
{
	LastLablePos[0]=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[0];
	LastLablePos[1]=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[1];
	LastLablePos[2]=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[2];
	LastLablePos[3]=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[3];
	LastLablePos[4]=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[4];
	LastLablePos[5]=g_pFrm->m_CmdRun.ProductParam.tgLabelPos[5];
}

void CDlgLabel::OnBnClickedBtnLaserDataread()
{
	// TODO: Add your control notification handler code here
	if(!g_pFrm->m_CmdRun.MoveToZSafety())
	{
		AfxMessageBox("Z��ذ�ȫλʧ�ܣ�");
		return;
	}
	if(!g_pFrm->m_CmdRun.SynchronizeMoveXY(LastLablePos[5].x,LastLablePos[5].y,true))
	{
		AfxMessageBox("�ƶ�����������ʷ����λʧ�ܣ�");
		return;
	}
	if (!g_pFrm->m_CmdRun.SynchronizeMoveZ(LastLablePos[5].za,0,0,false,true))
	{
		AfxMessageBox("�ƶ�����������ʷ����λʧ�ܣ�");
		return;

	}

}
