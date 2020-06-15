
// TSCtrlSysView.cpp : CTSCtrlSysView 类的实现
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTSCtrlSysView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTSCtrlSysView 构造/析构

CTSCtrlSysView::CTSCtrlSysView()
{
	// TODO: 在此处添加构造代码

}

CTSCtrlSysView::~CTSCtrlSysView()
{
}

BOOL CTSCtrlSysView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CTSCtrlSysView 绘制

void CTSCtrlSysView::OnDraw(CDC* /*pDC*/)
{
	CTSCtrlSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CTSCtrlSysView 打印


void CTSCtrlSysView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CTSCtrlSysView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CTSCtrlSysView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CTSCtrlSysView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CTSCtrlSysView 诊断

#ifdef _DEBUG
void CTSCtrlSysView::AssertValid() const
{
	CView::AssertValid();
}

void CTSCtrlSysView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTSCtrlSysDoc* CTSCtrlSysView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTSCtrlSysDoc)));
	return (CTSCtrlSysDoc*)m_pDocument;
}
#endif //_DEBUG


// CTSCtrlSysView 消息处理程序

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