
// IND_16995View.cpp : implementation of the CIND16995View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IND_16995.h"
#endif

#include "IND_16995Doc.h"
#include "IND_16995View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIND16995View

IMPLEMENT_DYNCREATE(CIND16995View, CView)

BEGIN_MESSAGE_MAP(CIND16995View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEHWHEEL()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CIND16995View construction/destruction

CIND16995View::CIND16995View() noexcept
{
	// TODO: add construction code here
	this->m_glRenderer = new CGLRender();
}

CIND16995View::~CIND16995View()
{
	if (this->m_glRenderer != nullptr)
	{
		delete this->m_glRenderer;
		this->m_glRenderer = nullptr;
	}
}

BOOL CIND16995View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CIND16995View drawing

void CIND16995View::OnDraw(CDC* pDC)
{
	CIND16995Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	this->m_glRenderer->DrawScene(pDC);

	// TODO: add draw code for native data here
}


// CIND16995View printing

BOOL CIND16995View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIND16995View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIND16995View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CIND16995View diagnostics

#ifdef _DEBUG
void CIND16995View::AssertValid() const
{
	CView::AssertValid();
}

void CIND16995View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIND16995Doc* CIND16995View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIND16995Doc)));
	return (CIND16995Doc*)m_pDocument;
}
#endif //_DEBUG


// CIND16995View message handlers

void CIND16995View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (nChar)
	{
	case('P'):
		this->m_glRenderer->ToggleGrid();
		break;
	case('C'):
		this->m_glRenderer->ToggleAxes();
		break;
	case('Q'):
		this->m_glRenderer->RotatePiece(5);
		break;
	case('E'):
		this->m_glRenderer->RotatePiece(-5);
		break;
	case('A'):
		this->m_glRenderer->RotateView(0, -5);
		break;
	case('D'):
		this->m_glRenderer->RotateView(0, 5);
		break;
	case('W'):
		this->m_glRenderer->RotateView(5, 0);
		break;
	case('S'):
		this->m_glRenderer->RotateView(-5, 0);
		break;
	default:
		break;
	}

	Invalidate();
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CIND16995View::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnMouseHWheel(nFlags, zDelta, pt);
}

int CIND16995View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();
	this->m_glRenderer->CreateGLContext(pDC);
	ReleaseDC(pDC);

	return 0;
}


void CIND16995View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	this->m_glRenderer->DestroyScene(pDC);
	ReleaseDC(pDC);
}


void CIND16995View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	this->m_glRenderer->Reshape(pDC, cx, cy);
	ReleaseDC(pDC);
}


void CIND16995View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CDC* pDC = GetDC();
	this->m_glRenderer->PrepareScene(pDC);
	ReleaseDC(pDC);
}

BOOL CIND16995View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CView::OnEraseBkgnd(pDC);
}

BOOL CIND16995View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	this->m_glRenderer->Zoom(zDelta < 0);
	Invalidate();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
