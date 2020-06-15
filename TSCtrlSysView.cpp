
// TSCtrlSysView.cpp : CTSCtrlSysView ���ʵ��
//

#include "stdafx.h"
#include "TSCtrlSys.h"

#include "TSCtrlSysDoc.h"
#include "TSCtrlSysView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTSCtrlSysView

IMPLEMENT_DYNCREATE(CTSCtrlSysView, CView)

BEGIN_MESSAGE_MAP(CTSCtrlSysView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTSCtrlSysView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTSCtrlSysView ����/����

CTSCtrlSysView::CTSCtrlSysView()
{
	// TODO: �ڴ˴���ӹ������

}

CTSCtrlSysView::~CTSCtrlSysView()
{
}

BOOL CTSCtrlSysView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CTSCtrlSysView ����

void CTSCtrlSysView::OnDraw(CDC* /*pDC*/)
{
	CTSCtrlSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CTSCtrlSysView ��ӡ


void CTSCtrlSysView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CTSCtrlSysView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CTSCtrlSysView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CTSCtrlSysView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CTSCtrlSysView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTSCtrlSysView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CTSCtrlSysView ���

#ifdef _DEBUG
void CTSCtrlSysView::AssertValid() const
{
	CView::AssertValid();
}

void CTSCtrlSysView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTSCtrlSysDoc* CTSCtrlSysView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTSCtrlSysDoc)));
	return (CTSCtrlSysDoc*)m_pDocument;
}
#endif //_DEBUG


// CTSCtrlSysView ��Ϣ�������

void CTSCtrlSysView::OnInitialUpdate()
{
	static BOOL b = TRUE;
	if(!b)return;
	b=FALSE;	
	
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	g_pView = this;


	m_ImgStatic.Create("IMAGE", WS_CHILD|WS_VISIBLE|SS_NOTIFY, CRect(0,0,20,20), this);
	if(!m_ImgStatic.ImgInit())
	{
		NULL;
	}
	else
	{
		m_ImgStatic.ImgLive();
	}
	SetTimer(0,500,NULL);

}

void CTSCtrlSysView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(m_ImgStatic.GetSafeHwnd())
	{
		m_ImgStatic.MoveWindow(0,0,cx, cy);
	}
}

void CTSCtrlSysView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
}

void CTSCtrlSysView::OnTimer(UINT nIDEvent)
{
	m_ImgStatic.Invalidate();
	CView::OnTimer(nIDEvent);
}