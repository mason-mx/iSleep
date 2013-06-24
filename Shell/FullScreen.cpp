/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/
#include "stdafx.h"
#include <windows.h>
#include "resource.h"
#include "APictureViewer.h"
#include "FullScreen.h"

//
// macro definition
//
#ifdef DEBUG
#define OutputMsg 1
#else
#define OutputMsg 0
#endif // DEBUG

#define IDS_FULLSCREEN_CLASS TEXT("FULLSCREENVIEW") 
#define IDS_FULLSCREEN_TITLE TEXT("Picture Full Screen View") 

#define FULL_SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define FULL_SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

//
// global variables
//
wchar_t *g_lpwszPicture = NULL;
CCoImage g_fullImage;
int g_nPreviousRotation = 0;

//
// forward declarations of functions included in this code module
//
ATOM FullScreenViewWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass);
LRESULT CALLBACK FullScreenViewWndProc(HWND, UINT, WPARAM, LPARAM);
void SetFilePath(wchar_t *lpwszFile);

//
// message handler
//
LRESULT OnFullScreenViewWndCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT OnFullScreenViewWndDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT OnFullScreenViewWndPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT OnFullScreenViewWndLBtnDown(HWND, UINT, WPARAM, LPARAM);
LRESULT OnFullScreenViewWndLBtnUp(HWND, UINT, WPARAM, LPARAM);
LRESULT OnFullScreenViewWndLBtnDoubleClick(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPictureRotate(HWND, UINT, WPARAM, LPARAM);

//
// message map
//
const struct decodeUINT g_stMsg[] = {
	WM_CREATE, OnFullScreenViewWndCreate,
	WM_DESTROY, OnFullScreenViewWndDestroy,
	WM_PAINT, OnFullScreenViewWndPaint,
	WM_LBUTTONDOWN, OnFullScreenViewWndLBtnDown,
	WM_LBUTTONUP, OnFullScreenViewWndLBtnUp,
	WM_LBUTTONDBLCLK, OnFullScreenViewWndLBtnDoubleClick,
};

//
//  registers the window class
//
ATOM FullScreenViewWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = FullScreenViewWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//  Create window
//
HWND CreateFullScreenViewWindow(int x, int y, int cx, int cy, HWND hWndParent, HINSTANCE hInstance)
{
	HWND hWnd = FindWindow(IDS_FULLSCREEN_CLASS, IDS_FULLSCREEN_TITLE);
	if (hWnd)
	{
		return hWnd;
	}

	if (!FullScreenViewWndRegisterClass(hInstance, IDS_FULLSCREEN_CLASS))
	{
		return NULL;
	}

	hWnd = CreateWindow(IDS_FULLSCREEN_CLASS, IDS_FULLSCREEN_TITLE, WS_VISIBLE/* | WS_CHILD*/, 
		x, y, cx, cy, /*hWndParent*/NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	return hWnd;
}

//
// process message for the main window.
//
LRESULT CALLBACK FullScreenViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
LRESULT OnFullScreenViewWndCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//
// WM_DESTROY
//
LRESULT OnFullScreenViewWndDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

//
// WM_PAINT
//
LRESULT OnFullScreenViewWndPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	// fill the back ground
	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdcMem, hBrush);
	FillRect(hdcMem, &rcClient, hBrush);
	SelectObject(hdcMem, hBrushOld);
	DeleteObject(hBrush);

	//SetBkMode(hdcMem, TRANSPARENT);
	//SetTextColor(hdcMem, TITLE_COLOR);

	if (g_lpwszPicture)
	{
		g_fullImage.CoImgDisplayImage(g_lpwszPicture, hdcMem, &rcClient);
	}

	BitBlt(hdc, rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
		hdcMem, rcClient.left, rcClient.top, SRCCOPY);

	SelectObject(hdcMem, hBmpOld);
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);

	EndPaint(hWnd, &ps);
	return 0;
}

//
// WM_LBUTTONDOWN
//
LRESULT OnFullScreenViewWndLBtnDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	if (x > 0 && x < FULL_SCREEN_WIDTH/3)
	{
		if (PreviousImage())
		{
			InvalidateRect(hWnd, &rcClient, FALSE);
		}
		else
		{
			ShowWindow(hWnd, SW_HIDE);
		}
	}
	else if (x > FULL_SCREEN_WIDTH*2/3 && x < FULL_SCREEN_WIDTH)
	{
		if (NextImage())
		{
			InvalidateRect(hWnd, &rcClient, FALSE);
		}
		else
		{
			ShowWindow(hWnd, SW_HIDE);
		}
	}

	return 0;
}

//
// WM_LBUTTONUP
//
LRESULT OnFullScreenViewWndLBtnUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_LBUTTONDBCLK
//
LRESULT OnFullScreenViewWndLBtnDoubleClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if (x > FULL_SCREEN_WIDTH/3 && x < FULL_SCREEN_WIDTH*2/3)
	{
		ShowWindow(hWnd, SW_HIDE);
	}
	return 0;
}

//
// WM_GSENSOR_ROTATE
//
LRESULT OnPictureRotate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	
	RETAILMSG(OutputMsg, (L"++++++++++ WM_GSENSOR_ROTATE [%d] PRE [%d]\r\n",  wParam, g_nPreviousRotation));
	switch (wParam)
	{
	case 0:
		if (g_nPreviousRotation != 0)
		{
			g_nPreviousRotation = 0;
			g_fullImage.CoImgDisplayRotateImage(hdc, &rcClient, 0);
		}
		break;
	case 1:
		if (g_nPreviousRotation != 1)
		{
			g_nPreviousRotation = 1;
			g_fullImage.CoImgDisplayRotateImage(hdc, &rcClient, 90);
		}
		break;
	case 2:
		if (g_nPreviousRotation != 2)
		{
			g_nPreviousRotation = 2;
			g_fullImage.CoImgDisplayRotateImage(hdc, &rcClient, 180);
		}
		break;
	case 3:
		if (g_nPreviousRotation != 3)
		{
			g_nPreviousRotation = 3;
			g_fullImage.CoImgDisplayRotateImage(hdc, &rcClient, 270);
		}
		break;

	default:
		break;
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

void SetFilePath(wchar_t *lpwszFile)
{
	g_lpwszPicture = lpwszFile;
}