/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/
#include "stdafx.h"
#include "AMediaPlayer.h"
#include "Duration.h"
#include "UserDef.h"

//
// macro definition
//
#ifdef DEBUG
#define OutputMsg 1
#else
#define OutputMsg 0
#endif // DEBUG

#define IDS_DURATION_CLASS TEXT("DURATIONWND") 
#define IDS_DURATION_TITLE TEXT("Duration") 

//
// global variables
//
CCoImage g_uiDurationImage;
const static UINT TID_REFRESH = 1;
const static UINT REFRESH_INTERVAL = 500; // ms

//
// forward declarations of functions included in this code module
//
ATOM PlayDurationWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass);
LRESULT CALLBACK PlayDurationWndProc(HWND, UINT, WPARAM, LPARAM);

//
// message handler
//
LRESULT OnPlayDurationWndCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayDurationWndDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayDurationWndPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayDurationWndTimer(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayDurationWndLBtnDown(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayDurationWndLBtnUp(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayDurationWndMouseMove(HWND, UINT, WPARAM, LPARAM);

//
// message map
//
const struct decodeUINT g_stMsg[] = {
	WM_CREATE, OnPlayDurationWndCreate,
	WM_DESTROY, OnPlayDurationWndDestroy,
	WM_PAINT, OnPlayDurationWndPaint,
	WM_TIMER, OnPlayDurationWndTimer,
	WM_LBUTTONDOWN, OnPlayDurationWndLBtnDown,
	WM_LBUTTONUP, OnPlayDurationWndLBtnUp,
	WM_MOUSEMOVE, OnPlayDurationWndMouseMove,
};

//
//  registers the window class
//
ATOM PlayDurationWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = PlayDurationWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//  Create window
//
HWND CreateDurationWindow(HWND hWndParent, HINSTANCE hInstance)
{
	HWND hWnd = FindWindow(IDS_DURATION_CLASS, IDS_DURATION_TITLE);
	if (hWnd)
	{
		ShowWindow(hWnd, SW_SHOW);
		return hWnd;
	}

	if (!PlayDurationWndRegisterClass(hInstance, IDS_DURATION_CLASS))
	{
		return NULL;
	}

	hWnd = CreateWindow(IDS_DURATION_CLASS, IDS_DURATION_TITLE, WS_VISIBLE | WS_CHILD, 
		DURATION_WND_LEFT, DURATION_WND_TOP, DURATION_WND_WIDTH, DURATION_WND_HEIGHT, hWndParent, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}

//
// process message for the main window.
//
LRESULT CALLBACK PlayDurationWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < dim(g_stMsg); i++)
	{
		if (message == g_stMsg[i].nID)
		{
			return (*g_stMsg[i].Fxn)(hWnd, message, wParam, lParam);
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_CREATE
//
LRESULT OnPlayDurationWndCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetTimer(hWnd, TID_REFRESH, REFRESH_INTERVAL, NULL);
	return 0;
}

//
// WM_DESTROY
//
LRESULT OnPlayDurationWndDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KillTimer(hWnd, TID_REFRESH);
	return 0;
}

//
// WM_PAINT
//
LRESULT OnPlayDurationWndPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, DURATION_WND_WIDTH, DURATION_WND_HEIGHT);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdcMem, hBrush);
	RECT rcClient = {0, 0, DURATION_WND_WIDTH, DURATION_WND_HEIGHT};
	FillRect(hdcMem, &rcClient, hBrush);
	SelectObject(hdcMem, hBrushOld);
	DeleteObject(hBrush);

	RECT rcDurationImg = {DURATION_IMG_LEFT, DURATION_IMG_TOP, DURATION_IMG_RIGHT, DURATION_IMG_BOTTOM};
	g_uiDurationImage.CoImgDisplayIcon(DURATION_DURATION, hdcMem, &rcDurationImg);

	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, TEXT_COLOR_WHITE);

	PLAYSTATE psCur = GetPlayState();
	if (psCur == psPlaying || psCur == psPause)
	{
		LONGLONG llDuration = 0;
		LONGLONG llPosition = 0;

		GetMediaDuration(&llDuration);
		GetCurrentPosition(&llPosition);
		int nSeek=((float)llPosition / (float)llDuration) * (DURATION_SEEK_RIGHT-DURATION_SEEK_LEFT);

		RECT rcSeek = {DURATION_SEEK_LEFT+nSeek, DURATION_SEEK_TOP, DURATION_SEEK_LEFT+DURATION_SEEK_WIDTH+nSeek, DURATION_SEEK_BOTTOM};
		g_uiDurationImage.CoImgDisplayIcon(DURATION_SEEK, hdcMem, &rcSeek);

		DWORD dwDuration = llDuration/10000000;
		DWORD dwPosition = llPosition/10000000;

		wchar_t wszDuration[MAX_PATH] = {0};
		swprintf(wszDuration, L"%02d:%02d:%02d", dwDuration/60/60, dwDuration/60%60, dwDuration%60);
		RECT rcDuration = {DURATION_TEXT_LEFT, DURATION_TEXT_TOP, DURATION_TEXT_RIGHT, DURATION_TEXT_BOTTOM};
		DrawText(hdcMem, wszDuration, -1, &rcDuration, DT_CENTER|DT_VCENTER);

		wchar_t wszPosition[MAX_PATH] = {0};
		swprintf(wszPosition, L"%02d:%02d:%02d", dwPosition/60/60, dwPosition/60%60, dwPosition%60);
		RECT rcPosition = {DURATION_SEEK_TEXT_LEFT, DURATION_SEEK_TEXT_TOP, DURATION_SEEK_TEXT_RIGHT, DURATION_SEEK_TEXT_BOTTOM};
		DrawText(hdcMem, wszPosition, -1, &rcPosition, DT_CENTER|DT_VCENTER);
	}

	BitBlt(hdc, 0, 0, DURATION_WND_WIDTH, DURATION_WND_HEIGHT, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hBmpOld);
	DeleteObject(hBitmap);
	hBitmap = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	EndPaint(hWnd, &ps);
	return 0;
}

//
// WM_TIMER
//
LRESULT OnPlayDurationWndTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (wParam == TID_REFRESH)
	{
		if (psPlaying == GetPlayState())
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			InvalidateRect(hWnd, &rc, FALSE);
		}
	}

	return 0;
}

//
// WM_LBUTTONDOWN
//
LRESULT OnPlayDurationWndLBtnDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if (y > 0 && y < DURATION_WND_HEIGHT)
	{
		if (x > DURATION_IMG_LEFT && x < DURATION_IMG_RIGHT)
		{
			if (psPlaying == GetPlayState())
			{
				int nPos = x - DURATION_IMG_LEFT;
				LONGLONG llDuration = 0;
				GetMediaDuration(&llDuration);

				LONGLONG llPosition = (float)llDuration / (DURATION_IMG_RIGHT-DURATION_IMG_LEFT) * nPos;
				JumpToPosition(llPosition);

				RECT rc;
				GetClientRect(hWnd, &rc);
				InvalidateRect(hWnd, &rc, FALSE);
			}
		}
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

//
// WM_LBUTTONUP
//
LRESULT OnPlayDurationWndLBtnUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_MOUSEMOVE
//
LRESULT OnPlayDurationWndMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}