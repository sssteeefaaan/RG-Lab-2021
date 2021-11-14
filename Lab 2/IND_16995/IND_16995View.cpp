
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
	this->windowSize.SetSize(800, 800);
	this->gridCount = 20;
	this->gridSize = { int(this->windowSize.cx / this->gridCount + 0.5), int(this->windowSize.cy / this->gridCount + 0.5) };

	this->grid = false;
	
	this->joints = new DPOINT[7]
	{
		{	double(10	* this->gridSize.cx),		double(17	* this->gridSize.cy) },
		{	double(10	* this->gridSize.cx),		double(14	* this->gridSize.cy) },
		{	double(10	* this->gridSize.cx),		double(11	* this->gridSize.cy) },
		{	double(10	* this->gridSize.cx),		double(8	* this->gridSize.cy) },
		{	double(10	* this->gridSize.cx),		double(11	* this->gridSize.cy) },
		{	double(10	* this->gridSize.cx),		double(11	* this->gridSize.cy) },
		{	double(10	* this->gridSize.cx),		double(8	* this->gridSize.cy) }
	};
	this->jointRadius = this->gridSize.cx / 2;

	this->branchAngles = new int[11] { 0, 45, 0, -45, 45, 0, 0, -45, 45, -45, 0 };
}

CIND16995View::~CIND16995View()
{
	if (this->joints != nullptr)
	{
		delete[] this->joints;
		this->joints = nullptr;
	}

	if (this->branchAngles != nullptr)
	{
		delete[] this->branchAngles;
		this->branchAngles = nullptr;
	}
}

BOOL CIND16995View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// CIND16995View drawing
void CIND16995View::DrawBackground(CDC* pDC, COLORREF color)
{
	CPen* oldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	CBrush* oldBrush = pDC->SelectObject(new CBrush(color));

	pDC->Rectangle(0, 0, this->windowSize.cx, this->windowSize.cy);

	delete pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}
void CIND16995View::DrawGrid(CDC* pDC, COLORREF color)
{
	CPen* oldPen = pDC->SelectObject(new CPen(PS_SOLID, 1, color));

	int pointsSize = (this->gridCount + 1) << 2,
		lengthsSize = (this->gridCount + 1) << 1;

	POINT* points = new POINT[pointsSize];
	DWORD* lenghts = new DWORD[lengthsSize];

	for (int i = 0; i < pointsSize; i += 4)
	{
		int tempY = (i >> 2) * this->gridSize.cy,
			tempX = (i >> 2) * this->gridSize.cx;

		points[i]		= {			  0,					tempY		};
		points[i + 1]	= { this->windowSize.cx,			tempY		};
		points[i + 2]	= {			tempX,					  0			};
		points[i + 3]	= {			tempX,			this->windowSize.cy };
	}

	for (int i = 0; i < lengthsSize; i++)
		lenghts[i] = 2;

	pDC->PolyPolyline(points, lenghts, lengthsSize);

	delete pDC->SelectObject(oldPen);
}

void CIND16995View::DrawPot(CDC* pDC)
{
	pDC->MoveTo(int(8.5 * this->gridSize.cx + 0.5), this->windowSize.cy - 1);

	pDC->BeginPath();

	POINT points[] = {
		{int(8 * this->gridSize.cx + 0.5), int(18 * this->gridSize.cy + 0.5)},
		{int(12 * this->gridSize.cx + 0.5), int(18 * this->gridSize.cy + 0.5)},
		{int(11.5 * this->gridSize.cx + 0.5), this->windowSize.cy - 1 },
		{int(8.5 * this->gridSize.cx + 0.5), this->windowSize.cy - 1}
	};

	pDC->Polygon(points, 4);
	pDC->Rectangle(int(7.6 * this->gridSize.cx + 0.5), int(17.2 * this->gridSize.cy + 0.5), int(12.4 * this->gridSize.cx + 0.5), int(18 * this->gridSize.cy + 0.5) + 1);

	pDC->EndPath();

	CBrush* oldBrush = pDC->SelectObject(new CBrush(RGB(200, 100, 0)));
	CPen* oldPen = pDC->SelectObject(new CPen(PS_SOLID, 0, RGB(0, 0, 0)));

	pDC->StrokeAndFillPath();

	delete pDC->SelectObject(oldPen);
	delete pDC->SelectObject(oldBrush);
}

void CIND16995View::DrawJoint(CDC* pDC, int index)
{
	CBrush* oldBrush = pDC->SelectObject(new CBrush(RGB(0, 180, 0)));

	pDC->Ellipse(	
					int(this->joints[index].x - this->jointRadius + 0.5),
					int(this->joints[index].y - this->jointRadius + 0.5),
					int(this->joints[index].x + this->jointRadius + 0.5),
					int(this->joints[index].y + this->jointRadius + 0.5)
				);

	delete pDC->SelectObject(oldBrush);
}

void CIND16995View::DrawBranch(CDC* pDC, DSIZE size, CString fileName, int jointIndex)
{
	CRect placement;
	double dx = (size.cx / 2);
	placement.SetRect(
		{
			int(this->joints[jointIndex].x - dx + 0.5),
			int(this->joints[jointIndex].y - size.cy + 0.5)
		},
		{
			int(this->joints[jointIndex].x + dx + 0.5),
			int(this->joints[jointIndex].y + 0.5) // - this->jointRadius
		});
	
	HENHMETAFILE mf = GetEnhMetaFile(fileName);
	pDC->PlayMetaFile(mf, placement);
	DeleteEnhMetaFile(mf);
}

void CIND16995View::DrawMyText(CDC* pDC, CString text, POINT from)
{
	LOGFONT logF{};
	logF.lfHeight = this->gridSize.cx + 5;
	logF.lfEscapement = -900;
	logF.lfOrientation = -900;
	logF.lfWeight = FW_DEMIBOLD; // | FW_NORMAL;
	logF.lfCharSet = ANSI_CHARSET;
	wcscpy_s(logF.lfFaceName, 32, L"Times New Roman");

	CFont font;
	//font.CreateFont(this->gridSize.cx + 5, 0, -900, -900, FW_DEMIBOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, L"Times New Roman");
	//font.CreateFontIndirect(&logF);
	font.CreateFontIndirect(&logF);

	CFont* oldFont = pDC->SelectObject(&font);

	int bkMode = pDC->SetBkMode(TRANSPARENT);
	UINT align = pDC->SetTextAlign(TA_LEFT | TA_TOP);

	COLORREF oldColor = pDC->SetTextColor(RGB(0, 0, 0));
	pDC->TextOut(from.x + 2, from.y + 2, text);

	pDC->SetTextColor(RGB(255, 255, 0));
	pDC->TextOut(from.x, from.y, text);

	pDC->SetTextColor(oldColor);
	pDC->SetTextAlign(align);
	pDC->SetBkMode(bkMode);
	pDC->SelectObject(oldFont);

	font.DeleteObject();
}

// Transformations
void CIND16995View::ModWorld(CDC* pDC, XFORM* matrix, bool rightMultiply)
{
	if (rightMultiply)
		pDC->ModifyWorldTransform(matrix, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(matrix, MWT_LEFTMULTIPLY);
}

void CIND16995View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM matrix = { 1, 0, 0, 1, dX, dY };

	this->ModWorld(pDC, &matrix, rightMultiply);
}
void CIND16995View::Rotate(CDC* pDC, double angle, bool rightMultiply = true)
{
	double dw = angle * M_PI / 180;

	XFORM matrix;
	matrix.eM11 = matrix.eM22 = cos(dw);
	matrix.eM12 = sin(dw);
	matrix.eM21 = -1 * matrix.eM12;
	matrix.eDx = matrix.eDy = 0;

	this->ModWorld(pDC, &matrix, rightMultiply);
}

void CIND16995View::RotationAround(CDC* pDC, DPOINT center, double angle, bool rightMultiply)
{
	if (rightMultiply)
	{
		Translate(pDC, -center.x, -center.y, true);
		Rotate(pDC, angle, true);
		Translate(pDC, center.x, center.y, true);
	}
	else
	{
		Translate(pDC, center.x, center.y, false);
		Rotate(pDC, angle, false);
		Translate(pDC, -center.x, -center.y, false);
	}
}

void CIND16995View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM matrix = { sX, 0, 0, sY, 0, 0 };

	this->ModWorld(pDC, &matrix, rightMultiply);
}
void CIND16995View::Reflect(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	this->Scale(pDC, dX, dY, rightMultiply);
}

void CIND16995View::DrawCactus(CDC* pDC)
{
	int oldGM = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldWT{};
	pDC->GetWorldTransform(&oldWT);

	RotationAround(pDC, joints[0], branchAngles[0], false);
	DrawBranch(pDC, { 2.2 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part_light.emf", 0);

	RotationAround(pDC, joints[1], branchAngles[1], false);
	DrawBranch(pDC, { 0.8 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 1);

	RotationAround(pDC, joints[2], branchAngles[2], false);
	DrawBranch(pDC, { 2.2 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 2);

	RotationAround(pDC, joints[3], branchAngles[3], false);
	DrawBranch(pDC, { 1.5 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 3);
	
	RotationAround(pDC, joints[3], branchAngles[4] - branchAngles[3], false);
	DrawBranch(pDC, { 1.5 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 3);
				
	RotationAround(pDC, joints[3], -branchAngles[4], false);
	DrawJoint(pDC, 3);

	RotationAround(pDC, joints[2], -branchAngles[2], false);
	DrawJoint(pDC, 2);
	
	RotationAround(pDC, joints[1], branchAngles[5] - branchAngles[1], false);
	DrawBranch(pDC, { 0.8 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 1);

	RotationAround(pDC, joints[4], branchAngles[6], false);
	DrawBranch(pDC, { 2.2 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part_light.emf", 4);
			
	RotationAround(pDC, joints[4], -branchAngles[6], false);
	DrawJoint(pDC, 4);
		
	RotationAround(pDC, joints[1], branchAngles[7] - branchAngles[5], false);
	DrawBranch(pDC, { 0.8 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 1);

	RotationAround(pDC, joints[5], branchAngles[8], false);
	DrawBranch(pDC, { 1.5 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 5);
		
	RotationAround(pDC, joints[5], branchAngles[9] - branchAngles[8], false);
	DrawBranch(pDC, { 1.5 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 5);

	RotationAround(pDC, joints[6], branchAngles[10], false);
	DrawBranch(pDC, { 2.2 * gridSize.cx, 3.0 * gridSize.cy }, (CString)"cactus_part.emf", 6);
		
	RotationAround(pDC, joints[6], -branchAngles[10], false);
	DrawJoint(pDC, 6);
		
	RotationAround(pDC, joints[5], -branchAngles[9], false);
	DrawJoint(pDC, 5);
	
	RotationAround(pDC, joints[1], -branchAngles[7], false);
	DrawJoint(pDC, 1);

	RotationAround(pDC, joints[0], -branchAngles[0], false);
	DrawJoint(pDC, 0);

	pDC->SetWorldTransform(&oldWT);
	pDC->SetGraphicsMode(oldGM);
}

void CIND16995View::OnDraw(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CPoint viewportOrg = pDC->SetViewportOrg((rect.Width() - windowSize.cx) / 2, (rect.Height() - windowSize.cy) / 2);

	DrawBackground(pDC, RGB(160, 210, 230));
	DrawCactus(pDC);
	DrawPot(pDC);
	DrawMyText(pDC, (CString)L"16995 Stefan Aleksić", { int(19 * gridSize.cx), int(gridSize.cy) });

	if (grid)
		DrawGrid(pDC, RGB(255, 255, 255));

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
	int val = 5;
	if(GetKeyState(VK_SHIFT) & 0x8000)
		val *= -1;

	switch (nChar)
	{
	case(VK_SPACE):
		this->grid = !this->grid;
		break;
	case('Q'):
		this->branchAngles[0] += val;
		break;
	case('W'):
		this->branchAngles[1] += val;
		break;
	case('E'):
		this->branchAngles[2] += val;
		break;
	case('R'):
		this->branchAngles[3] += val;
		break;
	case('T'):
		this->branchAngles[4] += val;
		break;
	case('Z'):
		this->branchAngles[5] += val;
		break;
	case('U'):
		this->branchAngles[6] += val;
		break;
	case('I'):
		this->branchAngles[7] += val;
		break;
	case('O'):
		this->branchAngles[8] += val;
		break;
	case('P'):
		this->branchAngles[9] += val;
		break;
	case('A'):
		this->branchAngles[10] += val;
		break;
	default:
		break;
	}

	Invalidate();
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}