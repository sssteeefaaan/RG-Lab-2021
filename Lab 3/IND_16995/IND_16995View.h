
// IND_16995View.h : interface of the CIND16995View class
//

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "DImage.h"

typedef struct PuzzlePiece {
	CString file;
	int angle;
	bool mx;
	bool my;
	int i;
	int j;
	POINT pos = { 0,0 };
	bool blueGray;
};

class CIND16995View : public CView
{
protected: // create from serialization only
	CIND16995View() noexcept;
	DECLARE_DYNCREATE(CIND16995View)

	// Attributes
public:
	CIND16995Doc* GetDocument() const;

private:
	int windowSize;
	int gridNumb;
	double gridSize;
	double picSize;
	int picRotate;
	PuzzlePiece* pieces;
	PuzzlePiece* selected;

	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


	// Transformations
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply);
	void TRT(CDC* pDC, POINT center, double angle, bool rightMultiply);
	void TMT(CDC* pDC, POINT center, bool mx, bool my, bool rightMultiply);
	void Transform(CDC* pDC, POINT diff, POINT center, double angle, bool mx, bool my, bool rightMultiply);

	//
	void LoadIdentity(CDC* pDC);
	void DrawInMemory(CDC* memDC);
	void DrawGrid(CDC* pDC);
	void DrawPicture(CDC* memDC);
	void DrawPuzzlePiece(CDC* memDC, int i, int j);
	void Grayscale(CBitmap* bits);
	void Bluescale(CBitmap* bitmap);

	void MakeTransparent(CDC* memDC, CBitmap*& subject, CBitmap*& mask, CString& fileName, bool grayScale);
	CBitmap* GetBitmap(CDC* memDC, CString& fileName);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Implementation
public:
	virtual ~CIND16995View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in IND_16995View.cpp
inline CIND16995Doc* CIND16995View::GetDocument() const
{
	return reinterpret_cast<CIND16995Doc*>(m_pDocument);
}
#endif
