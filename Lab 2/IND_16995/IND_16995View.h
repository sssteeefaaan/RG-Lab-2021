
// IND_16995View.h : interface of the CIND16995View class
//

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

struct DPOINT {
	double x;
	double y;
};

struct DSIZE {
	double cx;
	double cy;
};

class CIND16995View : public CView
{
protected: // create from serialization only
	CIND16995View() noexcept;
	DECLARE_DYNCREATE(CIND16995View)

// Attributes
public:
	CIND16995Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
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
private:
	CSize windowSize;
	CSize gridSize;
	int gridCount;
	bool grid;
	DPOINT* joints;
	int* branchAngles;
	double jointRadius;

	void DrawBackground(CDC* pDC, COLORREF color);
	void DrawGrid(CDC* pDC, COLORREF color);
	void DrawMyText(CDC* pDC, CString text, POINT from);
	void DrawPot(CDC* pDC);
	void DrawCactus(CDC* pDC);
	void DrawJoint(CDC* pDC, int index);
	void DrawBranch(CDC* pDC, CString fileName, int jointIndex, double sX, double sY);

	// Transformations
	void ModWorld(CDC* pDC, XFORM* matrix, bool rightMultiply);

	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Rotate(CDC* pDC, double angle, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Reflect(CDC* pDC, float dX, float dY, bool rightMultiply);

	void TRT(CDC* pDC, DPOINT center, double angle, bool rightMultiply);
	void TST(CDC* pDC, DPOINT center, double sX, double sY, bool rightMultiply);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in IND_16995View.cpp
inline CIND16995Doc* CIND16995View::GetDocument() const
   { return reinterpret_cast<CIND16995Doc*>(m_pDocument); }
#endif

