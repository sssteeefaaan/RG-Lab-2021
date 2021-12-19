
// IND_16995.h : main header file for the IND_16995 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CIND16995App:
// See IND_16995.cpp for the implementation of this class
//

class CIND16995App : public CWinApp
{
public:
	CIND16995App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIND16995App theApp;
