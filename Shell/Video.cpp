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

//
// macro definition
//
#ifdef DEBUG
#define OutputMsg 1
#else
#define OutputMsg 0
#endif // DEBUG

#define IDS_VIDEO_CLASS TEXT("AMEDIAPLAYERVIDEOWND") 
#define IDS_VIDEO_TITLE TEXT("Video Window") 

//
// forward declarations of functions included in this code module
//
ATOM VideoWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass);
LRESULT CALLBACK VideoWndProc(HWND, UINT, WPARAM, LPARAM);

//
// message handler
//
LRESULT OnVideoWndCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndLBtnDown(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndLBtnUp(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndMouseMove(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndLBtnDoubleClick(HWND, UINT, WPARAM, LPARAM);
LRESULT OnVideoWndMediaNotify(HWND, UINT, WPARAM, LPARAM);

//
// message map
//
const struct decodeUINT g_stMsg[] = {
	WM_CREATE, OnVideoWndCreate,
	WM_DESTROY, OnVideoWndDestroy,
	WM_PAINT, OnVideoWndPaint,
	WM_LBUTTONDOWN, OnVideoWndLBtnDown,
	WM_LBUTTONUP, OnVideoWndLBtnUp,
	WM_MOUSEMOVE, OnVideoWndMouseMove,
	WM_LBUTTONDBLCLK, OnVideoWndLBtnDoubleClick,
	WM_MEDIALIB_NOTIFY, OnVideoWndMediaNotify,
};

//
//  registers the window class
//
ATOM VideoWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = VideoWndProc;
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
HWND CreateVideoWindow(HWND hWndParent, HINSTANCE hInstance)
{
	HWND hWnd = FindWindow(IDS_VIDEO_CLASS, IDS_VIDEO_TITLE);
	if (hWnd)
	{
		ShowWindow(hWnd, SW_SHOW);
		return hWnd;
	}

	if (!VideoWndRegisterClass(hInstance, IDS_VIDEO_CLASS))
	{
		return NULL;
	}

	hWnd = CreateWindow(IDS_VIDEO_CLASS, IDS_VIDEO_TITLE, WS_VISIBLE | WS_CHILD, 
		VIDEO_WND_LEFT, VIDEO_WND_TOP, VIDEO_WND_WIDTH, VIDEO_WND_HEIGHT, hWndParent, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}

//
// process message for the main window.
//
LRESULT CALLBACK VideoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
LRESULT OnVideoWndCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_DESTROY
//
LRESULT OnVideoWndDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_PAINT
//
LRESULT OnVideoWndPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	EndPaint(hWnd, &ps);
	return 0;
}

//
// WM_LBUTTONDOWN
//
LRESULT OnVideoWndLBtnDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ResetVideoWindow();
	return 0;
}

//
// WM_LBUTTONUP
//
LRESULT OnVideoWndLBtnUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_MOUSEMOVE
//
LRESULT OnVideoWndMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_LBUTTONDBLCLK
//
LRESULT OnVideoWndLBtnDoubleClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_MEDIALIB_NOTIFY
//
LRESULT OnVideoWndMediaNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SendMessage(GetParent(hWnd), message, wParam, lParam);
	return 0; 
}