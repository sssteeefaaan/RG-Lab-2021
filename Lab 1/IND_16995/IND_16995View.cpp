
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
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CIND16995View construction/destruction

CIND16995View::CIND16995View() noexcept
{
	this->windowSize.SetRect({ 0,0 }, { 500, 500 });
	this->gridCount = 20;
	this->gridSize = int(500 / this->gridCount + 0.5);
	this->backgroundColor = RGB(200, 200, 200);
	this->selectedPen = { PS_GEOMETRIC | PS_DASH | PS_JOIN_ROUND | PS_ENDCAP_ROUND, 4, { BS_SOLID, RGB(235, 0, 255) } };

	PEN pen = { PS_GEOMETRIC | PS_SOLID | PS_JOIN_ROUND | PS_ENDCAP_ROUND, 5, {BS_SOLID, RGB(235, 0, 255)} },
		hexagonPen = { PS_GEOMETRIC | PS_SOLID | PS_JOIN_ROUND | PS_ENDCAP_ROUND, 2, {BS_SOLID, RGB(235, 0, 255)} };
	BRUSH brush;

	// Veliki ljubičasti trougao
	brush = { -1, RGB(100, 0, 160) };
	this->largeTriangle1 = new RightTriangle({ 8, 15.2 }, 7, this->gridSize, 8, pen, hexagonPen, brush, 135);

	// Veliki narandžasti trougao
	brush = { -1, RGB(255, 150, 0) };
	this->largeTriangle2 = new RightTriangle({ 8, 8.2 }, 7, this->gridSize, 4, pen, hexagonPen, brush, -45);

	// Srednji žuti trougao
	brush = { -1, RGB(255, 255, 0) };
	this->mediumTriangle = new RightTriangle({ 7, 16.2 }, 5, this->gridSize, 5, pen, hexagonPen, brush, -45);

	// Mali roze trougao
	brush = { -1, RGB(255, 185, 200) };
	this->smallTriangle1 = new RightTriangle({ 11.5, 11.8 }, 2.5 * M_SQRT2, this->gridSize, 7, pen, hexagonPen, brush, -90);

	// Mali zeleni trougao
	brush = { -1, RGB(0, 200, 0) };
	this->smallTriangle2 = new RightTriangle({ 9.5, 16.2 }, 2.5 * M_SQRT2, this->gridSize, 6, pen, hexagonPen, brush, -90);

	// Crveni kvadrat
	brush = { -1, RGB(255, 0, 0) };
	this->square = new Square({ 13.25, 6.5 }, 3.5, gridSize, pen, brush, 45);

	// Šrafirani paralelogram (Dijagonalna šrafura plave boje sa belom pozadinom)
	brush = { HS_FDIAGONAL, RGB(0, 0, 255), RGB(255, 255, 255) };
	this->parallelogram = new Parallelogram({ 8.1, 3 }, 3.5, this->gridSize, pen, brush);

	this->grid = false;
	this->selected = nullptr;
}

CIND16995View::~CIND16995View()
{
	if (this->largeTriangle1 != nullptr)
	{
		delete this->largeTriangle1;
		this->largeTriangle1 = nullptr;
	}

	if (this->largeTriangle2 != nullptr)
	{
		delete this->largeTriangle2;
		this->largeTriangle2 = nullptr;
	}

	if (this->mediumTriangle != nullptr)
	{
		delete this->mediumTriangle;
		this->mediumTriangle = nullptr;
	}

	if (this->smallTriangle1 != nullptr)
	{
		delete this->smallTriangle1;
		this->smallTriangle1 = nullptr;
	}

	if (this->smallTriangle2 != nullptr)
	{
		delete this->smallTriangle2;
		this->smallTriangle2 = nullptr;
	}

	if (this->square != nullptr)
	{
		delete this->square;
		this->square = nullptr;
	}

	if (this->parallelogram != nullptr)
	{
		delete this->parallelogram;
		this->parallelogram = nullptr;
	}
}

BOOL CIND16995View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CIND16995View drawing

void CIND16995View::DrawBackground(CDC* pDC)
{
	CPen* oldPen = (CPen*) pDC->SelectStockObject(NULL_PEN);
	CBrush* oldBrush = pDC->SelectObject(new CBrush(this->backgroundColor));

	pDC->Rectangle(0, 0, windowSize.Width(), windowSize.Height());

	delete pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

void CIND16995View::DrawGrid(CDC* pDC)
{
	if (this->grid)
	{
		CPen* oldPen = pDC->SelectObject(new CPen(PS_SOLID, 1, RGB(255, 255, 255)));

		POINT* points = new POINT[(this->gridCount + 1) << 2];
		DWORD* lengths = new DWORD[(this->gridCount + 1) << 1];

		int endW = windowSize.Width(),
			endH = windowSize.Height(),
			step = this->gridSize,
			i = 0,
			j = 0;

		for (int par = 0; par <= endH; par += step)
		{
			points[i++] = { 0, par };
			points[i++] = { endW, par };
			lengths[j++] = 2;
		}

		for (int par = 0; par <= endW; par += step)
		{
			points[i++] = { par, 0 };
			points[i++] = { par, endH };
			lengths[j++] = 2;
		}

		pDC->PolyPolyline(points, lengths, (this->gridCount + 1) << 1);

		delete[] lengths;
		delete[] points;

		delete pDC->SelectObject(oldPen);
	}
}

void CIND16995View::OnDraw(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CPoint viewportOrg = pDC->SetViewportOrg((rect.Width() - this->windowSize.Width()) / 2, (rect.Height() - this->windowSize.Height()) / 2);

	this->DrawBackground(pDC);

	if(this->selected != this->largeTriangle1)
		this->largeTriangle1->Draw(pDC);
	if (this->selected != this->largeTriangle2)
		this->largeTriangle2->Draw(pDC);
	if (this->selected != this->mediumTriangle)
		this->mediumTriangle->Draw(pDC);
	if (this->selected != this->smallTriangle1)
		this->smallTriangle1->Draw(pDC);
	if (this->selected != this->smallTriangle2)
		this->smallTriangle2->Draw(pDC);
	if (this->selected != this->square)
		this->square->Draw(pDC);
	if (this->selected != this->parallelogram)
		this->parallelogram->Draw(pDC);

	if (this->selected != nullptr)
		this->selected->Draw(pDC);

	this->DrawGrid(pDC);

	pDC->SetViewportOrg(viewportOrg);
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
	if (nChar == VK_SPACE)
	{
		this->grid = !this->grid;
	}

	if (this->selected != nullptr)
	{
		switch (nChar)
		{
		case('W'):
			this->selected->Move({ 0, (double)this->gridSize / 2 * (-1) });
			break;
		case('A'):
			this->selected->Move({ (double)this->gridSize / 2 * (-1), 0 });
			break;
		case('D'):
			this->selected->Move({ (double)this->gridSize / 2, 0 });
			break;
		case('S'):
			this->selected->Move({ 0, (double)this->gridSize / 2 });
			break;
		case('Q'):
			this->selected->Rotate(5);
			break;
		case('E'):
			this->selected->Rotate(-5);
			break;
		case('X'):
			this->selected->Mirror({ -1, 1 });
			break;
		case('Y'):
			this->selected->Mirror({ 1, -1 });
			break;
		case('0'):
			this->selectedPen = this->selected->Select(this->selectedPen);
			this->selected = nullptr;
			break;
		default:
			break;
		}
	}
	else {
		switch (nChar)
		{
		case('1'):
			this->selected = this->largeTriangle1;
			break;
		case('2'):
			this->selected = this->largeTriangle2;
			break;
		case('3'):
			this->selected = this->mediumTriangle;
			break;
		case('4'):
			this->selected = this->smallTriangle1;
			break;
		case('5'):
			this->selected = this->smallTriangle2;
			break;
		case('6'):
			this->selected = this->square;
			break;
		case('7'):
			this->selected = this->parallelogram;
			break;
		default:
			break;
		}

		if (this->selected != nullptr)
			this->selectedPen = this->selected->Select(this->selectedPen);
	}

	Invalidate();
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
