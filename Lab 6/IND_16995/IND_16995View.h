
// IND_16995View.h : interface of the CIND16995View class
//

#pragma once
#include "CGLRenderer.h"

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
	CGLRenderer* m_glRenderer;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // debug version in IND_16995View.cpp
inline CIND16995Doc* CIND16995View::GetDocument() const
   { return reinterpret_cast<CIND16995Doc*>(m_pDocument); }
#endif

