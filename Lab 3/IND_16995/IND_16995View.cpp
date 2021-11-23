
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
	//	ON_WM_ERASEBKGND()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CIND16995View construction/destruction

CIND16995View::CIND16995View() noexcept
{
	this->windowSize = 500;
	this->gridNumb = 20;
	this->gridSize = windowSize / gridNumb;
	this->picSize = 10 * gridSize;

	this->pieces = new PuzzlePiece[9]
	{
		{ L"resources\\8.dib", 127,	true,	false,	0,	0,	{ 18,  74 } },	{ L"resources\\1.dib", -96, true, false, 0, 1, { -63, -39 } },		{ L"resources\\2.dib", 106,	true,	false,	0,	2, { 41, 61 } },
		{ L"resources\\9.dib", 89,	true,	false,	1,	0,	{ 59,   47 } },	{ L"resources\\3.dib", 112, true, false, 1, 1, { 36, 65 } },		{ L"resources\\4.dib", 49,	true,	false,	1,	2, { 73, -4 } },
		{ L"resources\\6.dib", 40,	true,	false,	2,	0,	{ 75, -14 } },	{ L"resources\\5.dib", 117, true, false, 2, 1, { 30, 67 } },		{ L"resources\\7.dib", 141,	false,	true,	2,	2, { -1, -77 } }
	};

	this->selected = nullptr;
}

CIND16995View::~CIND16995View()
{
	if (this->pieces != nullptr)
	{
		delete[] this->pieces;
		this->pieces = nullptr;
	}
}

BOOL CIND16995View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CIND16995View drawing

void CIND16995View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM matrix = { 1, 0, 0, 1, dX, dY };
	pDC->ModifyWorldTransform(&matrix, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CIND16995View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM matrix = { sX, 0, 0, sY, 0, 0 };
	pDC->ModifyWorldTransform(&matrix, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CIND16995View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	double dW = angle * M_PI / 180,
		sine = sin(dW),
		cosine = cos(dW);

	XFORM matrix = { cosine, sine, -sine, cosine, 0, 0 };
	pDC->ModifyWorldTransform(&matrix, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CIND16995View::Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply)
{
	Scale(pDC, mx ? -1 : 1, my ? -1 : 1, rightMultiply);
}
void CIND16995View::TRT(CDC* pDC, POINT center, double angle, bool rightMultiply)
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
void CIND16995View::TMT(CDC* pDC, POINT center, bool mx, bool my, bool rightMultiply)
{
	if (rightMultiply)
	{
		Translate(pDC, -center.x, -center.y, true);
		Mirror(pDC, mx, my, true);
		Translate(pDC, center.x, center.y, true);
	}
	else
	{
		Translate(pDC, center.x, center.y, false);
		Mirror(pDC, mx, my, false);
		Translate(pDC, -center.x, -center.y, false);
	}
}
void CIND16995View::Transform(CDC* pDC, POINT diff, POINT center, double angle, bool mx, bool my, bool rightMultiply)
{
	if (rightMultiply)
	{
		Translate(pDC, -center.x, -center.y, true);
		Mirror(pDC, mx, my, true);
		Rotate(pDC, angle, true);
		Translate(pDC, center.x + diff.x, center.y + diff.y, true);
	}
	else {
		Translate(pDC, center.x + diff.x, center.y + diff.y, false);
		Rotate(pDC, angle, false);
		Mirror(pDC, mx, my, false);
		Translate(pDC, -center.x, -center.y, false);
	}
}
void CIND16995View::LoadIdentity(CDC* pDC)
{
	pDC->ModifyWorldTransform(nullptr, MWT_IDENTITY);
}

CBitmap* CIND16995View::GetBitmap(CDC* memDC, CString& fileName)
{
	DImage img;
	img.Load(fileName);

	CDC* tempDC = new CDC();
	tempDC->CreateCompatibleDC(memDC);

	CBitmap* ret = new CBitmap();
	ret->CreateCompatibleBitmap(memDC, picSize, picSize);

	CBitmap* oldBM = tempDC->SelectObject(ret);

	CRect posSrc, posDst;
	posSrc.SetRect(0, 0, img.Width(), img.Width());
	posDst.SetRect(0, 0, picSize, picSize);

	img.Draw(tempDC, posSrc, posDst);

	tempDC->SelectObject(oldBM);
	delete tempDC;

	return ret;
}

void CIND16995View::MakeTransparent(CDC* memDC, CBitmap*& subject, CBitmap*& mask, CString& fileName)
{
	BITMAP bm;
	subject = GetBitmap(memDC, fileName);
	subject->GetBitmap(&bm);

	mask = new CBitmap();
	mask->CreateBitmap(picSize, picSize, 1, 1, nullptr);

	CDC* src = new CDC(),
		* dst = new CDC();

	src->CreateCompatibleDC(memDC);
	dst->CreateCompatibleDC(memDC);

	CBitmap* oldBMPSRC = src->SelectObject(subject);
	CBitmap* oldBMPDST = dst->SelectObject(mask);

	COLORREF oldBkColorSRC = src->SetBkColor(src->GetPixel(0, 0));
	dst->BitBlt(0, 0, picSize, picSize, src, 0, 0, SRCCOPY);

	COLORREF oldTXTColorSRC = src->SetTextColor(RGB(0, 0, 0));
	src->SetBkColor(RGB(255, 255, 255));
	src->BitBlt(0, 0, picSize, picSize, dst, 0, 0, SRCPAINT);

	src->SetTextColor(oldTXTColorSRC);
	src->SetBkColor(oldBkColorSRC);

	dst->SelectObject(oldBMPDST);
	src->SelectObject(oldBMPSRC);

	delete dst;
	delete src;
}

void CIND16995View::DrawPuzzlePiece(CDC* memDC, int i, int j)
{
	PuzzlePiece* piece = &(this->pieces[i * 3 + j]);

	CBitmap* subject = NULL,
		* mask = NULL;
	MakeTransparent(memDC, subject, mask, piece->file);

	BITMAP bmp{};
	subject->GetBitmap(&bmp);

	int oldGM = memDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldWT;
	memDC->GetWorldTransform(&oldWT);
	{
		CDC* newDC = new CDC();
		newDC->CreateCompatibleDC(memDC);

		CRect pos;
		pos.SetRect(piece->j * 6 * gridSize,
			piece->i * 6 * gridSize,
			(piece->j + 1) * 6 * gridSize,
			(piece->i + 1) * 6 * gridSize);

		Transform(memDC, piece->pos, { pos.left + pos.Width() / 2, pos.top + pos.Height() / 2 }, piece->angle, piece->mx, piece->my, false);

		CBitmap* oldBM = newDC->SelectObject(mask);
		memDC->SetBkColor(RGB(0, 0, 0));
		memDC->SetTextColor(RGB(255, 255, 255));
		memDC->BitBlt(pos.left, pos.top, picSize, picSize, newDC, 0, 0, SRCPAINT);

		newDC->SelectObject(subject);
		memDC->BitBlt(pos.left, pos.top, picSize, picSize, newDC, 0, 0, SRCAND);

		//Transform(memDC, { -piece->pos.x, -piece->pos.y }, { pos.left + pos.Width() / 2, pos.top + pos.Height() / 2 }, -piece->angle, !piece->mx, !piece->my, false);

		newDC->SelectObject(oldBM);
		delete newDC;
	}
	memDC->SetWorldTransform(&oldWT);
	memDC->SetGraphicsMode(oldGM);

	if (subject != nullptr)
		delete subject;
	if (mask != nullptr)
		delete mask;
}

void CIND16995View::Grayscale(CBitmap* bitmap)
{
	BITMAP b;
	bitmap->GetBitmap(&b);

	BYTE* bits = new BYTE[b.bmWidthBytes * b.bmHeight];
	bitmap->GetBitmapBits(b.bmWidthBytes * b.bmHeight, bits);

	for (int i = 0; i < b.bmWidthBytes * b.bmHeight; i += 4)
	{
		bits[i] =
			bits[i + 1] =
			bits[i + 2] = min(255, 64 + (bits[i] + bits[i + 1] + bits[i + 2]) / 3);
		bits[i + 3] = 0;
	}

	bitmap->SetBitmapBits(b.bmWidthBytes * b.bmHeight, bits);

	if (bits)
		delete[] bits;

	bits = nullptr;
}

void CIND16995View::DrawPicture(CDC* memDC)
{
	CDC* newDC = new CDC();
	newDC->CreateCompatibleDC(memDC);

	CBitmap* newBMP = new CBitmap();
	newBMP->CreateCompatibleBitmap(memDC, windowSize - 2 * gridSize, windowSize - 2 * gridSize);

	CBitmap* oldBMP = newDC->SelectObject(newBMP);
	newDC->Rectangle(0, 0, windowSize - 2 * gridSize, windowSize - 2 * gridSize);

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			DrawPuzzlePiece(newDC, i, j);

	this->Grayscale(newBMP);
	memDC->BitBlt(gridSize, gridSize, windowSize - 2 * gridSize, windowSize - 2 * gridSize, newDC, 0, 0, SRCCOPY);

	delete newDC->SelectObject(oldBMP);;
	delete newDC;
}

void CIND16995View::DrawGrid(CDC* pDC)
{
	int pointNumb = (gridNumb + 1) * 4,
		lengthNumb = (gridNumb + 1) * 2;

	POINT* points = new POINT[pointNumb];
	DWORD* lenghts = new DWORD[lengthNumb];

	int j = 0;
	for (int i = 0; i < lengthNumb; i += 2)
	{
		points[i] = { 0, int(j * gridSize + 0.5) };
		points[i + 1] = { windowSize, int(j * gridSize + 0.5) };

		points[i + lengthNumb] = { int(j * gridSize + 0.5) , 0 };
		points[i + lengthNumb + 1] = { int(j * gridSize + 0.5), windowSize };

		j++;
	}

	for (int i = 0; i < lengthNumb; i++)
		lenghts[i] = 2;

	CPen* oldPen = pDC->SelectObject(new CPen(PS_SOLID, 0, RGB(0, 0, 0)));
	pDC->PolyPolyline(points, lenghts, lengthNumb);
	delete pDC->SelectObject(oldPen);

	if (lenghts)
	{
		delete[] lenghts;
		lenghts = nullptr;
	}
	if (points)
	{
		delete[] points;
		points = nullptr;
	}
}

void CIND16995View::DrawInMemory(CDC* memDC)
{
	CRect client;
	GetClientRect(&client);

	CPoint oldVPOrg = memDC->SetViewportOrg((client.Width() - windowSize) / 2, (client.Height() - windowSize) / 2);

	DrawGrid(memDC);
	DrawPicture(memDC);

	memDC->SetViewportOrg(oldVPOrg);
}

void CIND16995View::OnDraw(CDC* pDC)
{
	CRect client;
	GetClientRect(&client);

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, client.Width(), client.Height());

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);

	CBitmap* oldBMP = memDC->SelectObject(&bitmap);
	memDC->Rectangle(0, 0, client.Width(), client.Height());

	DrawInMemory(memDC);

	pDC->BitBlt(0, 0, client.Width(), client.Height(), memDC, 0, 0, SRCCOPY);

	delete memDC;
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
	int step = GetKeyState(VK_SHIFT) & 0x8000 ? 1 : 10;
	if (nChar > '0' && nChar <= '9')
	{
		int ind = (int)(nChar - '0') - 1;

		if (this->selected != nullptr) {
			int indTemp = this->selected->i * 3 + this->selected->j;

			PuzzlePiece temp1 = this->pieces[ind],
				temp2 = this->pieces[indTemp];

			temp1.i = this->pieces[indTemp].i;
			temp1.j = this->pieces[indTemp].j;

			temp2.i = this->pieces[ind].i;
			temp2.j = this->pieces[ind].j;

			this->pieces[ind] = temp2;
			this->pieces[indTemp] = temp1;
		}

		this->selected = &this->pieces[ind];
	}

	if (this->selected)
	{
		switch (nChar)
		{
		case('0'):
			this->selected = nullptr;
			break;
		case('X'):
			this->selected->mx = !this->selected->mx;
			break;
		case('Y'):
			this->selected->my = !this->selected->my;
			break;
		case('Q'):
			this->selected->angle -= step;
			break;
		case('E'):
			this->selected->angle += step;
			break;
		case(VK_UP):
			this->selected->pos.y -= step;
			break;
		case(VK_DOWN):
			this->selected->pos.y += step;
			break;
		case(VK_LEFT):
			this->selected->pos.x -= step;
			break;
		case(VK_RIGHT):
			this->selected->pos.x += step;
			break;
		}
	}

	Invalidate(0);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CIND16995View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return false;
	return CView::OnEraseBkgnd(pDC);
}
