// XAnalogClock.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XAnalogClock implements CXAnalogClock, a skinnable analog clock control.
//
// History
//     Version 1.0 - 2006 March 23
//     - Initial public release
//
// Public APIs:
//          NAME                           DESCRIPTION
//     ---------------------    -------------------------------------------
//     GetTime()                Get currently displayed time
//     GetWindowSize()          Get control size
//     IsClockRunning()         Returns run state of clock
//     SetBackgroundColor()     Set background color
//     SetBitmaps()             Set bitmap resource IDs
//     SetGmtAdjust()           Not currently used
//     SetHourHandColor()       Set color of hour hand
//     SetMinuteHandColor()     Set color of minute hand
//     SetSecondHandColor()     Set color of second hand
//     SetTime()                Set starting time (if system time not used)
//     SetTransparentColor()    Set transparent color
//     ShowDate()               Show/hide date
//     ShowSecondHand()         Show/hide second hand
//     Run()                    Start the clock
//     Stop()                   Stop the clock
//     UseSystemTime()          Use system time, or user-supplied time (see
//                              SetTime()
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XAnalogClock.h"
#include "math.h"
#include "WuLine.h"
#include "Shell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// some definitions
/*#define DATE_BOX_WIDTH		39
#define DATE_BOX_HEIGHT		31
#define DATE_BOX_XPOS		410
#define DATE_BOX_YPOS		200
#define SECOND_HAND_LENGTH	190.0
#define MINUTE_HAND_LENGTH	150.0
#define HOUR_HAND_LENGTH	120.0
#define SECOND_HAND_COLOR	RGB(96,0,192)
#define MINUTE_HAND_COLOR	RGB(192,96,192)
#define HOUR_HAND_COLOR		RGB(255,251,239)*/
#define PI					3.1415926535

///////////////////////////////////////////////////////////////////////////////
// CXAnalogClock

///////////////////////////////////////////////////////////////////////////////
// ctor
CXAnalogClock::CXAnalogClock()
{
	m_nPrevMinute     = -1;
	m_bShowDate       = TRUE;
	m_bShowSecondHand = TRUE;
	m_bUseSystemTime  = TRUE;
	m_bStopped        = TRUE;
	//m_time            = CTime::GetCurrentTime();
	//GetSystemTime(&m_time);
	//m_time            = CTime(2006, 1, 15, 8, 40, 0);
	m_dGmtAdjust      = 0.0;
	m_rgbBackground   = RGB(255,0,255);
	m_rgbTransparent  = RGB(255,0,255);
	m_rgbSecondHand   = SECOND_HAND_COLOR;
	m_rgbMinuteHand   = MINUTE_HAND_COLOR;
	m_rgbHourHand     = HOUR_HAND_COLOR;
	m_nFaceBitmapId   = 0;
	m_nDateBitmapId   = 0;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXAnalogClock::~CXAnalogClock()
{
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXAnalogClock::PreSubclassWindow()
{
//	CClientDC dc(NULL);
//	LoadBitmaps(&dc);

//	CStatic::PreSubclassWindow();
}

///////////////////////////////////////////////////////////////////////////////
// GetWindowSize
SIZE CXAnalogClock::GetWindowSize()
{
	HDC dc = CreateCompatibleDC(NULL);
	LoadBitmaps(dc);

	return m_cdcClockFace.m_sizeBitmap;
}

///////////////////////////////////////////////////////////////////////////////
// LoadBitmaps
void CXAnalogClock::LoadBitmaps(HDC pDC)
{
	if ((m_nFaceBitmapId == 0) || (m_nDateBitmapId == 0))
	{
		return;
	}

	// load clockface.bmp
	if (!m_cdcClockFace.IsValid())
		VERIFY(m_cdcClockFace.LoadBitmap(m_nFaceBitmapId, pDC));

	// load date.bmp
	if (!m_cdcDate.IsValid())
		VERIFY(m_cdcDate.LoadBitmap(m_nDateBitmapId, pDC));

	if (!m_cdcPrevious.IsValid())
		VERIFY(m_cdcPrevious.LoadBitmap(m_nFaceBitmapId, pDC));
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXAnalogClock::OnPaint(HWND hWnd)
{
	RECT rc;
	PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
	
	//RECT rc1={0,0,600,300};
	//RECT rc2={20,300,580,430};
	HDC hdcMem;
	HBITMAP hbmMem;
	HANDLE hOld;

	GetClientRect(hWnd, &rc);
	LoadBitmaps(hdc);

	if (m_bUseSystemTime)
		GetSystemTime(&m_time);

	// Create an off-screen DC for double-buffering
	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	hOld   = SelectObject(hdcMem, hbmMem);
	SetBkMode(hdcMem, TRANSPARENT);

	// Draw into hdcMem
	FillRect(hdcMem, &rc, CreateSolidBrush(RGB(0,0,0)));

	TransparentBlt(hdcMem,
		(m_nClockWidth - m_cdcClockFace.m_sizeBitmap.cx)/2, (m_nClockHeight - m_cdcClockFace.m_sizeBitmap.cy)/2,
		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
		m_cdcClockFace.m_dcMemory,
		0, 0,
		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
		m_rgbTransparent);

	if (m_bShowDate)
			PaintDate(hdcMem);

	PaintMinuteAndHourHands(hdcMem);

	if (m_bShowSecondHand)
		PaintSecondHand(hdcMem);

	// Transfer the off-screen DC to the screen
	BitBlt(hdc, 0, 0,rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
	// Free-up the off-screen DC
	SelectObject(hdcMem, hOld);
	DeleteObject(hbmMem);
	DeleteDC    (hdcMem);

    EndPaint(hWnd, &ps);
	//if ((m_nFaceBitmapId == 0) || (m_nDateBitmapId == 0))
	//{
	//	return;
	//}

	//PAINTSTRUCT ps;
 //   HDC hdc = BeginPaint(hWnd, &ps);

	//LoadBitmaps(hdc);
	// HBITMAP bitmap;// = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLUE_CLOCK_FACE));

	//if (m_bUseSystemTime)
	//	GetSystemTime(&m_time);// = CTime::GetCurrentTime();

	//RECT rect;
	//GetClientRect(hWnd, &rect);
	//HDC memDC;
	//memDC = CreateCompatibleDC(hdc);
	//bitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
	//HBITMAP pOldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

	//// fill in background color
	//FillRect(memDC, &rect, CreateSolidBrush(m_rgbBackground));

	////if (m_nPrevMinute != GetMinute())
	//{
	//	// draw clock face
	//	TransparentBlt(memDC,
	//		(m_nClockWidth - m_cdcClockFace.m_sizeBitmap.cx)/2, (m_nClockHeight - m_cdcClockFace.m_sizeBitmap.cy)/2,
	//		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
	//		m_cdcClockFace.m_dcMemory,
	//		0, 0,
	//		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
	//		m_rgbTransparent);

	//	/*if (m_bShowDate)
	//		PaintDate(memDC);*/

	//	PaintMinuteAndHourHands(memDC);

	//	// save clock face & hands for this minute
	//	/*BitBlt(m_cdcPrevious.m_dcMemory, 0, 0,
	//		m_cdcClockFace.m_sizeBitmap.cx, m_cdcClockFace.m_sizeBitmap.cy,
	//		memDC, 0, 0, SRCCOPY);*/

	//	m_nPrevMinute = GetMinute();
	//}
	////else
	////{
	////	// minute hasn't changed, so use previous clock face & hands
	////	BitBlt(memDC, 0, 0,
	////		rect.right - rect.left, rect.bottom - rect.top,
	////		m_cdcPrevious.m_dcMemory,
	////		0, 0, SRCCOPY);
	////}

	//if (m_bShowSecondHand)
	//	PaintSecondHand(memDC);

	//// finally blt to screen
	//TransparentBlt(hdc,
	//	0, 0,
	//	rect.right - rect.left, rect.bottom - rect.top,
	//	memDC,
	//	0, 0,
	//	rect.right - rect.left, rect.bottom - rect.top,
	//	m_rgbTransparent);
	////BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

	//SelectObject(memDC, pOldBitmap);
	//DeleteObject(memDC);
	//DeleteDC    (memDC);
	//EndPaint(hWnd, &ps);

	// Do not call CStatic::OnPaint() for painting messages
}

///////////////////////////////////////////////////////////////////////////////
// ConvertToDegrees
float CXAnalogClock::ConvertToDegrees(float dTime)
{
	float degrees = 0.;

	degrees = (float)(90. - dTime * 6.0);
	if (degrees < 0.0)
		degrees = (float)360. + degrees;
	return degrees;
}

///////////////////////////////////////////////////////////////////////////////
// PaintDate
void CXAnalogClock::PaintDate(HDC pDC)
{
	HFONT pFont = CreateFont(XAC_DateFontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,NULL);
	
	if (pFont)
	{
		TCHAR strDate[3] = {0};
		wsprintf(strDate, TEXT("%02d"), GetDay());

		RECT rect = {0, 0, DATE_BOX_WIDTH, DATE_BOX_HEIGHT};
		HDC hdcMem;
		HBITMAP hbmMem;
		HANDLE hOld;

		// create temporary dc to hold the date numerals
		//CDCBuffer cdcTemp;
		//VERIFY(cdcTemp.LoadBitmap(m_nDateBitmapId, pDC));
		////HFONT pOldFont = (HFONT)SelectObject(cdcTemp.m_dcMemory, pFont);
		////CBrush brush(RGB(255,0,255));
		//FillRect(cdcTemp.m_dcMemory, &rect, CreateSolidBrush(RGB(255,0,255)));
		//SetBkColor(cdcTemp.m_dcMemory, RGB(255,0,255));
		//DrawText(cdcTemp.m_dcMemory, strDate, -1, &rect, DT_RIGHT | DT_VCENTER);
		//ExtTextOut(cdcTemp.m_dcMemory, 3, 1, 0, &rect, strDate, _tcslen(strDate), NULL);

		hdcMem = CreateCompatibleDC(pDC);
		hbmMem = CreateCompatibleBitmap(pDC, rect.right - rect.left, rect.bottom - rect.top);
		hOld   = SelectObject(hdcMem, hbmMem);
		FillRect(hdcMem, &rect, CreateSolidBrush(RGB(255,0,255)));

		HFONT pOldFont = (HFONT)SelectObject(hdcMem, pFont);
		SetBkMode(hdcMem, TRANSPARENT);
		SetTextColor(hdcMem, RGB(32,0,192));
		DrawText(hdcMem, strDate, -1, &rect, DT_CENTER | DT_VCENTER);

		// draw date box template
		BitBlt(pDC, DATE_BOX_XPOS, DATE_BOX_YPOS, DATE_BOX_WIDTH, DATE_BOX_HEIGHT,
			m_cdcDate.m_dcMemory, 0, 0, SRCCOPY);

		//// overlay with date numerals
		TransparentBlt(pDC,
			DATE_BOX_XPOS+1, DATE_BOX_YPOS-1,
			DATE_BOX_WIDTH, DATE_BOX_HEIGHT,
			hdcMem,
			0, 0,
			DATE_BOX_WIDTH, DATE_BOX_HEIGHT,
			RGB(255,0,255));

		SelectObject(hdcMem, pOldFont);
		DeleteObject(pFont);

		SelectObject(hdcMem, hOld);
		DeleteObject(hbmMem);
		DeleteDC    (hdcMem);
	}
}

///////////////////////////////////////////////////////////////////////////////
// PaintMinuteAndHourHands
void CXAnalogClock::PaintMinuteAndHourHands(HDC pDC)
{
	float degrees, radians, cosine, sine;
	short x1, y1, x2, y2;

	///////////////////////////////////////////////////////////////////////////
	// draw minute hand
	int minute = GetMinute();
	//if(minute < 0)
	//	minute = 59;
	int height = 6;

	degrees = ConvertToDegrees((float)minute);
	radians = (float) ((PI * degrees)/180.);
	cosine = (float)cos(radians);
	sine = (float)sin(radians);

	x1 = (short)(MINUTE_HAND_LENGTH * cosine + (float)m_nClockWidth/2);
	y1 = (short)(-MINUTE_HAND_LENGTH * sine + (float)m_nClockHeight/2);
	x2 = (short)(12.0 * cosine + (float)m_nClockWidth/2);
	y2 = (short)(-12.0 * sine + (float)m_nClockHeight/2);

	if (((minute >= 10) && (minute <= 20)) || ((minute >= 40) && (minute <= 50)))
	{
		//x1 = (short)(MINUTE_HAND_LENGTH * cosine + (float)m_nClockWidth/2);
		//y1 = (short)(-MINUTE_HAND_LENGTH * sine + (float)m_nClockHeight/2);
		DrawWuLine(pDC, x1, y1, x2, y2, m_rgbMinuteHand);
		//x1 = (short)((MINUTE_HAND_LENGTH-4) * cosine + (float)m_nClockWidth/2);
		//y1 = (short)((-MINUTE_HAND_LENGTH+4) * sine + (float)m_nClockHeight/2);
		while(height > 0)
		{
			DrawWuLine(pDC, x1, y1-height/2, x2, y2-height, m_rgbMinuteHand);
			DrawWuLine(pDC, x1, y1+height/2, x2, y2+height, m_rgbMinuteHand);
			height --;
		}
		/*DrawWuLine(pDC, x1, y1, x2, y2-2, m_rgbMinuteHand);
		DrawWuLine(pDC, x1, y1, x2, y2-1, m_rgbMinuteHand);
		DrawWuLine(pDC, x1, y1, x2, y2+1, m_rgbMinuteHand);
		DrawWuLine(pDC, x1, y1, x2, y2+2, m_rgbMinuteHand);*/
	}
	else
	{
		//x1 = (short)(MINUTE_HAND_LENGTH * cosine + (float)m_nClockWidth/2);
		//y1 = (short)(-MINUTE_HAND_LENGTH * sine + (float)m_nClockHeight/2);
		DrawWuLine(pDC, x1, y1, x2, y2, m_rgbMinuteHand);
		//x1 = (short)((MINUTE_HAND_LENGTH-4) * cosine + (float)m_nClockWidth/2);
		//y1 = (short)((-MINUTE_HAND_LENGTH+4) * sine + (float)m_nClockHeight/2);
		while(height > 0)
		{
			DrawWuLine(pDC, x1-height/2, y1, x2-height, y2, m_rgbMinuteHand);
			DrawWuLine(pDC, x1+height/2, y1, x2+height, y2, m_rgbMinuteHand);
			height --;
		}
		/*DrawWuLine(pDC, x1, y1, x2-2, y2, m_rgbMinuteHand);
		DrawWuLine(pDC, x1, y1, x2-1, y2, m_rgbMinuteHand);
		DrawWuLine(pDC, x1, y1, x2+1, y2, m_rgbMinuteHand);
		DrawWuLine(pDC, x1, y1, x2+2, y2, m_rgbMinuteHand);*/
	}

	///////////////////////////////////////////////////////////////////////////
	// draw hour hand
	int hour = GetHour();	

	if (hour > 12)
		hour -= 12;
	hour = hour*5 + GetMinute()/12;
	degrees = ConvertToDegrees((float)hour);
	radians = (float) ((PI * degrees)/180.);
	cosine = (float)cos(radians);
	sine = (float)sin(radians);
	height = 8;

	x1 = (short)(HOUR_HAND_LENGTH * cosine + (float)m_nClockWidth/2);
	y1 = (short)(-HOUR_HAND_LENGTH * sine + (float)m_nClockHeight/2);
	x2 = (short)(20.0 * cosine + (float)m_nClockWidth/2);
	y2 = (short)(-20.0 * sine + (float)m_nClockHeight/2);

	if (((hour >= 10) && (hour <= 20)) || ((hour >= 40) && (hour <= 50)))
	{
		DrawWuLine(pDC, x1, y1, x2, y2, m_rgbHourHand);
		//x1 = (short)((HOUR_HAND_LENGTH-4) * cosine + (float)m_nClockWidth/2);
		//y1 = (short)((-HOUR_HAND_LENGTH+4) * sine + (float)m_nClockHeight/2);
		while(height > 0)
		{
			DrawWuLine(pDC, x1, y1-height/2, x2, y2-height, m_rgbHourHand);
			DrawWuLine(pDC, x1, y1+height/2, x2, y2+height, m_rgbHourHand);
			height --;
		}
		/*
		DrawWuLine(pDC, x1, y1, x2, y2-2, m_rgbHourHand);
		DrawWuLine(pDC, x1, y1, x2, y2-1, m_rgbHourHand);
		DrawWuLine(pDC, x1, y1, x2, y2+1, m_rgbHourHand);
		DrawWuLine(pDC, x1, y1, x2, y2+2, m_rgbHourHand);
		*/
	}
	else
	{
		DrawWuLine(pDC, x1, y1, x2, y2, m_rgbHourHand);
		//x1 = (short)((HOUR_HAND_LENGTH-4) * cosine + (float)m_nClockWidth/2);
		//y1 = (short)((-HOUR_HAND_LENGTH+4) * sine + (float)m_nClockHeight/2);
		while(height > 0)
		{
			DrawWuLine(pDC, x1-height/2, y1, x2-height, y2, m_rgbHourHand);
			DrawWuLine(pDC, x1+height/2, y1, x2+height, y2, m_rgbHourHand);
			height --;
		}
		/*
		DrawWuLine(pDC, x1, y1, x2-2, y2, m_rgbHourHand);
		DrawWuLine(pDC, x1, y1, x2-1, y2, m_rgbHourHand);
		DrawWuLine(pDC, x1, y1, x2+1, y2, m_rgbHourHand);
		DrawWuLine(pDC, x1, y1, x2+2, y2, m_rgbHourHand);
		*/
	}
}

///////////////////////////////////////////////////////////////////////////////
// PaintSecondHand
void CXAnalogClock::PaintSecondHand(HDC pDC)
{
	if (m_bShowSecondHand)
	{
		// draw second hand
		float degrees, radians, cosine, sine;
		short x1, y1, x2, y2;

		degrees = ConvertToDegrees((float)GetSecond());
		radians = (float)((PI * degrees)/180.);
		cosine = (float)cos(radians);
		sine = (float)sin(radians);

		x1 = (short)(SECOND_HAND_LENGTH * cosine + (float)m_nClockWidth/2);
		y1 = (short)(-SECOND_HAND_LENGTH * sine + (float)m_nClockHeight/2);
		x2 = (short)(5.0 * cosine + (float)m_nClockWidth/2);
		y2 = (short)(-5.0 * sine + (float)m_nClockHeight/2);

		DrawWuLine(pDC, x1, y1, x2, y2, m_rgbSecondHand);
		DrawWuLine(pDC, x1+1, y1, x2+1, y2, m_rgbSecondHand);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL CXAnalogClock::OnEraseBkgnd(HDC pDC)
{
	return 0;;//CStatic::OnEraseBkgnd(pDC);
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXAnalogClock::OnTimer(HWND hWnd, UINT nIDEvent)
{
	if (!m_bUseSystemTime)
	{
		//CTimeSpan ts(0, 0, 0, 1);
		//m_time = m_time + ts;
	}
	RECT rt;
	GetClientRect(hWnd, &rt);
    InvalidateRect(hWnd, &rt, FALSE);
//	CStatic::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// Run
void CXAnalogClock::Run(HWND hWnd)
{
	m_nPrevMinute = -1;
	if (m_bShowSecondHand)
		SetTimer(hWnd, 9009, 1000,(TIMERPROC) NULL);
	else
		SetTimer(hWnd, 9009, 60000,(TIMERPROC) NULL);
	RECT rt;
	GetClientRect(hWnd, &rt);
    InvalidateRect(hWnd, &rt, TRUE);
	m_bStopped = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Stop
void CXAnalogClock::Stop()
{
	KillTimer(NULL, 1);
	m_bStopped = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetSecond
int CXAnalogClock::GetSecond()
{
	return m_time.wSecond;
}

///////////////////////////////////////////////////////////////////////////////
// GetMinute
int CXAnalogClock::GetMinute()
{
	return m_time.wMinute;
}

///////////////////////////////////////////////////////////////////////////////
// GetHour
int CXAnalogClock::GetHour()
{
	int hour = m_time.wHour;
	if (hour > 12)
		hour -= 12;
	return hour;
}

///////////////////////////////////////////////////////////////////////////////
// GetDay
int CXAnalogClock::GetDay()
{
	int day = m_time.wDay;
	return day;
}

///////////////////////////////////////////////////////////////////////////////
// PreTranslateMessage
BOOL CXAnalogClock::PreTranslateMessage(MSG* pMsg)
{
	return 0;//CStatic::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXAnalogClock::OnDestroy() 
{
	Stop();
//	CStatic::OnDestroy();
}

extern HINSTANCE hInst;
CXAnalogClock cxac;
HWND hWndXAC = NULL;

// Message handlers
LRESULT DoCreateXAC(HWND, UINT, WPARAM, LPARAM);
LRESULT DoPaintXAC(HWND, UINT, WPARAM, LPARAM);
LRESULT DoDestroyXAC(HWND, UINT, WPARAM, LPARAM);
LRESULT DoTimerXAC(HWND, UINT, WPARAM, LPARAM);

// Message dispatch table for MainWindowProc
const struct decodeUINT XACMessages[] = {
    WM_CREATE, DoCreateXAC,
	WM_PAINT, DoPaintXAC,
    WM_DESTROY, DoDestroyXAC,
	WM_TIMER, DoTimerXAC,
};

//======================================================================
// Message-handling procedures for AWWindow
//
//----------------------------------------------------------------------
// AWWndProc - Callback function for application window
//
LRESULT CALLBACK XACWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    INT i;
    //
    // Search message list to see if we need to handle this
    // message.  If in list, call procedure.
    //
    for (i = 0; i < dim(XACMessages); i++) {
		if (wMsg == XACMessages[i].Code)
            return (*XACMessages[i].Fxn)(hWnd, wMsg, wParam, lParam);
    }
    return DefWindowProc (hWnd, wMsg, wParam, lParam);
}

LRESULT DoCreateXAC(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	cxac.SetClockSize(XAC_WIN_WIDTH,XAC_WIN_HEIGHT);
#if MULTIMEDIAFEATURE
	cxac.SetBitmaps(IDB_BLUE_CLOCK_FACE, IDB_BLUE_DATE);
#else
	cxac.SetBitmaps(IDB_BLUE_CLOCK_FACE2, IDB_BLUE_DATE);
#endif
	cxac.Run(hWnd);
	return 0;
}

LRESULT DoPaintXAC(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	cxac.OnPaint(hWnd);
	return 0;
}

LRESULT DoTimerXAC(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	cxac.OnTimer(hWnd,9009);
	return 0;
}

LRESULT DoDestroyXAC(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	cxac.Stop();
	return 0;
}

BOOL XACRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wndclass = {0}; 
	DWORD    wStyle   = 0;

	wndclass.style         = 0;//CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) XACWndProc; 
	wndclass.hInstance     = hInstance; 
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
	wndclass.lpszClassName = TEXT ("XACWindow"); 

	if (!RegisterClass (&wndclass) ) 
	{
		return FALSE; 
	}
	return TRUE;
}

BOOL ShowImmeXACWnd(HWND hWnd,SYSTEMTIME immeTime)
{
	cxac.SetTime(immeTime);

	// Create main window.
    hWndXAC = CreateWindow (TEXT ("XACWindow"),            // Window class
							TEXT ("XAC Window"), // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							XAC_WIN_TOP,        // x position
							XAC_WIN_LEFT,        // y position
							XAC_WIN_WIDTH,        // Initial width
							XAC_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (hWndXAC)) return FALSE;

    // Standard show and update calls
    ShowWindow (hWndXAC, TRUE);
    UpdateWindow (hWndXAC);	
	
    return TRUE;
}

BOOL ShowXACWnd(HWND hWnd)
{
	// Create main window.
    hWndXAC = CreateWindow (TEXT ("XACWindow"),            // Window class
							TEXT ("XAC Window"), // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							XAC_WIN_TOP,        // x position
							XAC_WIN_LEFT,        // y position
							XAC_WIN_WIDTH,        // Initial width
							XAC_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (hWndXAC)) return FALSE;

    // Standard show and update calls
    ShowWindow (hWndXAC, TRUE);
    UpdateWindow (hWndXAC);	
	
    return TRUE;
}

void XACStop()
{
	cxac.Stop();
}

void XACStart()
{
	cxac.Run(hWndXAC);
}