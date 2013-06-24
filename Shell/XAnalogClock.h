// XAnalogClock.h  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// This software is released into the public domain.  You are free to use
// it in any way you like, except that you may not sell this source code.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this
// software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XANALOGCLOCK_H
#define XANALOGCLOCK_H

#include "CDCBuffer.h"

/////////////////////////////////////////////////////////////////////////////
// CXAnalogClock window

class CXAnalogClock
{
// Construction
public:
	CXAnalogClock();
	virtual ~CXAnalogClock();

// Attributes
public:
	SYSTEMTIME GetTime() { return m_time; }
	SIZE GetWindowSize();
	BOOL IsClockRunning() { return !m_bStopped; }
	BOOL IsSecondHandShowing() { return m_bShowSecondHand; }
	void SetBackgroundColor(COLORREF rgb) { m_rgbBackground = rgb; }
	void SetBitmaps(UINT nFaceBitmapId, UINT nDateBitmapId)
	{
		if (m_cdcClockFace.IsValid())
			m_cdcClockFace.DeleteObject();
		if (m_cdcDate.IsValid())
			m_cdcDate.DeleteObject();
		if (m_cdcPrevious.IsValid())
			m_cdcPrevious.DeleteObject();

		m_nFaceBitmapId = nFaceBitmapId;
		m_nDateBitmapId = nDateBitmapId;
	}
	void SetGmtAdjust(float dAdjust) { m_dGmtAdjust = dAdjust; }
	void SetHourHandColor(COLORREF rgb) { m_rgbHourHand = rgb; }
	void SetMinuteHandColor(COLORREF rgb) { m_rgbMinuteHand = rgb; }
	void SetSecondHandColor(COLORREF rgb) { m_rgbSecondHand = rgb; }
	void SetTime(SYSTEMTIME t) { m_time = t; }
	void SetTransparentColor(COLORREF rgb) { m_rgbTransparent = rgb; }
	void ShowDate(BOOL bShowDate) { m_bShowDate = bShowDate; }
	void ShowSecondHand(BOOL bShowSecondHand) { m_bShowSecondHand = bShowSecondHand; }
	void UseSystemTime(BOOL bUseSystemTime) { m_bUseSystemTime = bUseSystemTime; }
	void SetClockSize(int width, int height) { m_nClockWidth = width; m_nClockHeight = height; }

// Operations
public:
	void Run(HWND hWnd);
	void Stop();	
	void OnPaint(HWND hWnd);
	BOOL OnEraseBkgnd(HDC pDC);
	void OnTimer(HWND hWnd, UINT nIDEvent);
	void OnDestroy();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXAnalogClock)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GetDay();
	int GetHour();
	int GetMinute();
	int GetSecond();

	float ConvertToDegrees(float dTime);
	void LoadBitmaps(HDC pDC);
	void PaintDate(HDC pDC);
	void PaintMinuteAndHourHands(HDC pDC);
	void PaintSecondHand(HDC pDC);

	CDCBuffer	m_cdcClockFace;
	CDCBuffer	m_cdcDate;
	CDCBuffer	m_cdcPrevious;
	int			m_nPrevMinute;
	BOOL		m_bShowDate;
	BOOL		m_bShowSecondHand;
	BOOL		m_bUseSystemTime;
	BOOL		m_bStopped;
	float		m_dGmtAdjust;
	SYSTEMTIME	m_time;
	COLORREF	m_rgbBackground;
	COLORREF	m_rgbTransparent;
	COLORREF	m_rgbSecondHand;
	COLORREF	m_rgbMinuteHand;
	COLORREF	m_rgbHourHand;
	UINT		m_nFaceBitmapId;
	UINT		m_nDateBitmapId;
	int			m_nClockWidth;
	int			m_nClockHeight;

//	// Generated message map functions
//protected:
//	//{{AFX_MSG(CXAnalogClock)
//	afx_msg void OnPaint();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnTimer(UINT nIDEvent);
//	afx_msg void OnDestroy();
//	//}}AFX_MSG
//
//	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XANALOGCLOCK_H
