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
#include "PlayList.h"

#include "..\Common\AFileList\AFileList.h"

#ifdef DEBUG
#pragma comment(lib, "..\\Common\\AFileList\\Debug\\AFileList.lib")
#pragma comment(lib, "..\\Common\\TinyXMLdll\\Debug\\TinyXMLdll.lib")
#else
#pragma comment(lib, "..\\Common\\AFileList\\Release\\AFileList.lib")
#pragma comment(lib, "..\\Common\\TinyXMLdll\\Release\\TinyXMLdll.lib")
#endif // DEBUG

//
// macro definition
//
#ifdef DEBUG
#define OutputMsg 1
#else
#define OutputMsg 0
#endif // DEBUG

#ifdef PLAY_TYPE_MUSIC
#define PLAY_PLAYLIST_PATH TEXT("USB Disk\\Config\\music.xml")
#define MEDIA_FILE_DIR TEXT("USB Disk")
#define MEDIA_FILE_TYPE mtMusic
#define BROWSER_PARAM TEXT("music")
#else
#ifdef PLAY_TYPE_VIDEO
#define PLAY_PLAYLIST_PATH TEXT("USB Disk\\Config\\video.xml")
#define MEDIA_FILE_DIR TEXT("USB DISK")
#define MEDIA_FILE_TYPE mtVideo
#define BROWSER_PARAM TEXT("video")
#endif // PLAY_TYPE_VIDEO
#endif // PLAY_TYPE_MUSIC

#define IDS_PLAYLIST_CLASS TEXT("PLAYLISTWND") 
#define IDS_PLAYLIST_TITLE TEXT("Play List") 

//
// global variables
//
FILE_LIST_ITEM *g_lpListItem = NULL;
CCoImage g_uiImageList;
CAFileList g_playList;
HWND g_hWndPlayList = NULL;

UINT g_nItemsOnPage = 0; // number of the items on current page
UINT g_nTotalPages = 0; // number of pages of the entire list
UINT g_nTotalItems = 0; //total numbers of the items
int g_nCurPage = 0; // current select page
int g_nCurItem = 0; // current select item

// list control item identifier
enum enumListControlID {
	LCID_PREVIOUS,
	LCID_NEXT,
	LCID_ITEMUP,
	LCID_ITEMDN,
	LCID_REMOVE,
	LCID_ADD,
};

// control button structure
struct BUTTON_CTRL_STRUCT g_listCtrl[] = {
	LCID_PREVIOUS, {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+PLAYLIST_BTN_HEIGHT}, L"Page Up", PLAYLIST_IMG_PREVIOUS, PLAYLIST_IMG_BTNFOCUS,
	LCID_NEXT, {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP+PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+2*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT}, L"Page Down", PLAYLIST_IMG_NEXT, PLAYLIST_IMG_BTNFOCUS,
	LCID_ITEMUP, {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP+2*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+3*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT}, L"Move Up", PLAYLIST_IMG_MOVEUP, PLAYLIST_IMG_BTNFOCUS,
	LCID_ITEMDN, {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP+3*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+4*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT}, L"Move Down", PLAYLIST_IMG_MOVEDOWN, PLAYLIST_IMG_BTNFOCUS,	
	LCID_REMOVE, {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP+4*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+5*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT}, L"Remove", PLAYLIST_IMG_REMOVE, PLAYLIST_IMG_BTNFOCUS, 
	LCID_ADD, {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP+5*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+6*PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT}, L"Add", PLAYLIST_IMG_ADD, PLAYLIST_IMG_BTNFOCUS,
};
// page number rectangle
RECT g_rcPageNumber = {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP+PLAYLIST_BTN_HEIGHT, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_TOP+PLAYLIST_BTN_HEIGHT+PLAYLIST_ITEM_HEIGHT};

//
// forward declarations of functions included in this code module
//
ATOM PlayListWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass);
LRESULT CALLBACK PlayListWndProc(HWND, UINT, WPARAM, LPARAM);
void OnListButtonPress(HWND hWnd, HDC hdc, UINT nIDCtrl);
DWORD AddItemThreadProc(LPVOID lpParam);

//
// message handler
//
LRESULT OnPlayListWndCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayListWndDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayListWndPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayListWndLBtnDown(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayListWndLBtnUp(HWND, UINT, WPARAM, LPARAM);
LRESULT OnPlayListWndLBtnDoubleClick(HWND, UINT, WPARAM, LPARAM);

//
// message map
//
const struct decodeUINT g_stMsg[] = {
	WM_CREATE, OnPlayListWndCreate,
	WM_DESTROY, OnPlayListWndDestroy,
	WM_PAINT, OnPlayListWndPaint,
	WM_LBUTTONDOWN, OnPlayListWndLBtnDown,
	WM_LBUTTONUP, OnPlayListWndLBtnUp,
	WM_LBUTTONDBLCLK, OnPlayListWndLBtnDoubleClick,
};

//
//  registers the window class
//
ATOM PlayListWndRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = PlayListWndProc;
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
HWND CreatePlayListWindow(HWND hWndParent, HINSTANCE hInstance)
{
	g_hWndPlayList = FindWindow(IDS_PLAYLIST_CLASS, IDS_PLAYLIST_TITLE);
	if (g_hWndPlayList)
	{
		return g_hWndPlayList;
	}

	if (!PlayListWndRegisterClass(hInstance, IDS_PLAYLIST_CLASS))
	{
		return NULL;
	}

	g_hWndPlayList = CreateWindow(IDS_PLAYLIST_CLASS, IDS_PLAYLIST_TITLE, WS_VISIBLE | WS_CHILD, 
		PLAYLIST_WND_LEFT, PLAYLIST_WND_TOP, PLAYLIST_WND_WIDTH, PLAYLIST_WND_HEIGHT, hWndParent, NULL, hInstance, NULL);

	ShowWindow(g_hWndPlayList, SW_HIDE);
	UpdateWindow(g_hWndPlayList);

	return g_hWndPlayList;
}

//
// process message for the main window.
//
LRESULT CALLBACK PlayListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
LRESULT OnPlayListWndCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_playList.OpenList(PLAY_PLAYLIST_PATH);
	g_nTotalItems = g_playList.LoadItems(&g_lpListItem); 
	return 0;
}

//
// WM_DESTROY
//
LRESULT OnPlayListWndDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_lpListItem)
	{
		delete []g_lpListItem;
	}
	return 0;
}

//
// WM_PAINT
//
LRESULT OnPlayListWndPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdcMem, hBrush);
	FillRect(hdcMem, &rcClient, hBrush);
	SelectObject(hdcMem, hBrushOld);
	DeleteObject(hBrush);

	RECT rcItem = {0, 0, PLAYLIST_WND_WIDTH-PLAYLIST_BTN_WIDTH-1, PLAYLIST_WND_HEIGHT};
	g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_ITEMBKGND, hdcMem, &rcItem);
	RECT rcBtn = {PLAYLIST_BTN_LEFT, PLAYLIST_BTN_TOP, PLAYLIST_BTN_RIGHT, PLAYLIST_BTN_BOTTOM};
	g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNBKGND, hdcMem, &rcBtn);

	for (int i = 0; i < dim(g_listCtrl); i++)
	{
		g_uiImageList.CoImgDisplayIcon(g_listCtrl[i].lpwszImg, hdcMem, &g_listCtrl[i].rcCtrl);
	}

	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, TEXT_COLOR_WHITE);

	if (g_nTotalItems > 0)
	{
		if (PLAYLIST_ITEM_PERPAGE > g_nTotalItems)
		{
			g_nTotalPages = 1;
		}
		else
		{
			g_nTotalPages = (g_nTotalItems*100/PLAYLIST_ITEM_PERPAGE+99)/100;
		}

		g_nItemsOnPage = g_nTotalItems-g_nCurPage*PLAYLIST_ITEM_PERPAGE;
		if (g_nItemsOnPage > PLAYLIST_ITEM_PERPAGE)
		{
			g_nItemsOnPage = PLAYLIST_ITEM_PERPAGE;
		}

		for (int i = 0; i < PLAYLIST_ITEM_PERPAGE && i < g_nItemsOnPage; i++)
		{
			int nIndex = g_nCurPage*PLAYLIST_ITEM_PERPAGE+i;
			RECT rcItemTitle = {PLAYLIST_ITEM_TEXTLEFT, PLAYLIST_ITEM_TOP+i*PLAYLIST_ITEM_HEIGHT, 
				PLAYLIST_ITEM_TEXTRIGHT, PLAYLIST_ITEM_TOP+(i+1)*PLAYLIST_ITEM_HEIGHT};
			DrawText(hdcMem, g_lpListItem[nIndex].wszName, -1, &rcItemTitle, DT_LEFT|DT_VCENTER);
		}
	}

	if (g_nCurPage == g_nCurItem/PLAYLIST_ITEM_PERPAGE && g_nCurItem < g_nTotalItems)
	{
		int nIndex = g_nCurItem - g_nCurPage*PLAYLIST_ITEM_PERPAGE;
		RECT rcFocus = {PLAYLIST_ITEM_LEFT, PLAYLIST_ITEM_TOP+nIndex*PLAYLIST_ITEM_HEIGHT, 
			PLAYLIST_ITEM_RIGHT, PLAYLIST_ITEM_TOP+(nIndex+1)*PLAYLIST_ITEM_HEIGHT};
		g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_ITEMFOCUS, hdcMem, &rcFocus);
	}

	// page numbers
	wchar_t wszItem[MAX_PATH] = {0};
	if (g_nTotalPages == 0)
	{
		swprintf(wszItem, L"%d / %d", g_nCurPage, g_nTotalPages);
	}
	else
	{
		swprintf(wszItem, L"%d / %d", g_nCurPage+1, g_nTotalPages);
	}
	DrawText(hdcMem, wszItem, -1, &g_rcPageNumber, DT_CENTER|DT_VCENTER);

	BitBlt(hdc, rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
		hdcMem, rcClient.left, rcClient.top, SRCCOPY);

	SelectObject(hdcMem, hBmpOld);
	DeleteObject(hBitmap);
	hBitmap = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	EndPaint(hWnd, &ps);
	return 0;
}

//
// WM_LBUTTONDOWN
//
LRESULT OnPlayListWndLBtnDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	// item selection
	if ( x > PLAYLIST_ITEM_LEFT && x < PLAYLIST_ITEM_RIGHT && y > PLAYLIST_ITEM_TOP && y < PLAYLIST_ITEM_BOTTOM)
	{
		int nIndex = (y-PLAYLIST_ITEM_TOP)/PLAYLIST_ITEM_HEIGHT;
		int nItem = g_nCurPage*PLAYLIST_ITEM_PERPAGE+nIndex;
		if (g_nTotalItems > nItem)
		{
			g_nCurItem = nItem;
		}
	}
	else // button pressing
	{
		for (int i = 0; i < dim(g_listCtrl); i++)
		{
			if (x > g_listCtrl[i].rcCtrl.left  && x < g_listCtrl[i].rcCtrl.right
				&& y > g_listCtrl[i].rcCtrl.top  && y < g_listCtrl[i].rcCtrl.bottom)
			{
				OnListButtonPress(hWnd, hdc, i);
				break;
			}
		}
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

//
// WM_LBUTTONUP
//
LRESULT OnPlayListWndLBtnUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	InvalidateRect(hWnd, &rcClient, FALSE);

	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_LBUTTONDBCLK
//
LRESULT OnPlayListWndLBtnDoubleClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if ( x > PLAYLIST_ITEM_LEFT && x < PLAYLIST_ITEM_RIGHT && y > PLAYLIST_ITEM_TOP && y < PLAYLIST_ITEM_BOTTOM)
	{
		int nIndex = (y-PLAYLIST_ITEM_TOP)/PLAYLIST_ITEM_HEIGHT;
		int nItem = g_nCurPage*PLAYLIST_ITEM_PERPAGE+nIndex;
		if (g_nTotalItems > nItem)
		{
			g_nCurItem = nItem;
		}

		if (g_playList.CheckExistence(g_lpListItem[g_nCurItem].wszPath))
		{
			PlayMediaFile(g_lpListItem[g_nCurItem].wszPath);
		}
		else
		{
			PlayMediaFile(NULL);
		}
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

//
// handle the button press event
//
void OnListButtonPress(HWND hWnd, HDC hdc, UINT nIDCtrl)
{
	RECT rcBtn = {g_listCtrl[nIDCtrl].rcCtrl.left, g_listCtrl[nIDCtrl].rcCtrl.top, 
		g_listCtrl[nIDCtrl].rcCtrl.right, g_listCtrl[nIDCtrl].rcCtrl.bottom};

	switch (nIDCtrl) {
		case LCID_PREVIOUS:
			if (g_nCurPage > 0)
			{
				g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNFOCUS, hdc, &g_listCtrl[nIDCtrl].rcCtrl);
				g_nCurPage -= 1;
			}
			break;
		case LCID_NEXT:
			if (g_nTotalPages > 0 && g_nCurPage < g_nTotalPages-1)
			{
				g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNFOCUS, hdc, &g_listCtrl[nIDCtrl].rcCtrl);
				g_nCurPage += 1;
			}
			break;
		case LCID_ITEMUP:
			if (g_nCurItem > 0 && g_nCurItem < g_nTotalItems)
			{
				g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNFOCUS, hdc, &g_listCtrl[nIDCtrl].rcCtrl);
				g_playList.SwitchItem(g_nCurItem+1, g_nCurItem);
				g_nCurItem -= 1;
				// page up
				if (g_nCurItem > (g_nCurPage-1)*PLAYLIST_ITEM_PERPAGE && g_nCurItem < g_nCurPage*PLAYLIST_ITEM_PERPAGE)
				{
					g_nCurPage -= 1;
				}
				// refresh the item list
				g_nTotalItems = g_playList.LoadItems(&g_lpListItem); 
			}
			break;
		case LCID_ITEMDN:
			if (g_nCurItem >= 0 && g_nCurItem+1 < g_nTotalItems)
			{
				g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNFOCUS, hdc, &g_listCtrl[nIDCtrl].rcCtrl);
				g_playList.SwitchItem(g_nCurItem+1, g_nCurItem+2);
				g_nCurItem += 1;
				// page down
				if (g_nCurPage < g_nTotalPages-1 && g_nCurItem >= (g_nCurPage+1)*PLAYLIST_ITEM_PERPAGE)
				{
					g_nCurPage += 1;
				}
				// refresh the item list
				g_nTotalItems = g_playList.LoadItems(&g_lpListItem); 
			}
			break;
		case LCID_REMOVE:
			if (g_nCurItem >= 0 && g_nCurItem < g_nTotalItems)
			{
				g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNFOCUS, hdc, &g_listCtrl[nIDCtrl].rcCtrl);
				g_playList.DeleteItem(g_nCurItem);
				g_nTotalItems = g_playList.LoadItems(&g_lpListItem); 

				if (g_nCurItem > 0)
				{
					g_nCurItem -= 1;
				}

				// page up
				if (g_nCurItem > (g_nCurPage-1)*PLAYLIST_ITEM_PERPAGE && g_nCurItem < g_nCurPage*PLAYLIST_ITEM_PERPAGE)
				{
					g_nCurPage -= 1;
				}
				// page down
				if (g_nCurPage < g_nTotalPages-1 && g_nCurItem >= (g_nCurPage+1)*PLAYLIST_ITEM_PERPAGE)
				{
					g_nCurPage += 1;
				}

				if (psPlaying == GetPlayState())
				{
					if (g_playList.CheckExistence(g_lpListItem[g_nCurItem].wszPath))
					{
						PlayMediaFile(g_lpListItem[g_nCurItem].wszPath);
					}
					else
					{
						PlayMediaFile(NULL);
					}
				}
			}
			break;
		case LCID_ADD:
			g_uiImageList.CoImgDisplayIcon(PLAYLIST_IMG_BTNFOCUS, hdc, &g_listCtrl[nIDCtrl].rcCtrl);
#ifndef USE_FIXED_DIRECTORY
			g_playList.AddItems(MEDIA_FILE_DIR, MEDIA_FILE_TYPE);
			g_nTotalItems = g_playList.LoadItems(&g_lpListItem);
#else
			PROCESS_INFORMATION pi;
			if (CreateProcess(APP_LAUNCH_BROWSER,BROWSER_PARAM,NULL,NULL,FALSE,0,NULL,NULL,NULL,&pi))
			{
				CloseHandle(pi.hThread);
				HANDLE hThread = CreateThread(NULL, 0, AddItemThreadProc, (LPVOID)pi.hProcess, 0, NULL);
				CloseHandle(hThread);
			}
#endif // USE_FIXED_DIRECTORY
			break;

		default:
			break;
	}
}

DWORD AddItemThreadProc(LPVOID lpParam)
{
	HPROCESS hProcess = (HPROCESS)lpParam;

	if (WAIT_OBJECT_0 == WaitForSingleObject(hProcess, INFINITE))
	{
		g_nTotalItems = g_playList.LoadItems(&g_lpListItem);

		RECT rc;
		GetClientRect(g_hWndPlayList, &rc);
		InvalidateRect(g_hWndPlayList, &rc, FALSE);

		CloseHandle(hProcess);
	}

	return 0;
}

void PlayListRefresh(void)
{
	g_nCurItem = 0;
	g_nCurPage = 0;

	RECT rcClient;
	GetClientRect(g_hWndPlayList, &rcClient);
	InvalidateRect(g_hWndPlayList, &rcClient, FALSE);
}
//
// go to the next item
//
int PlayListNext(int nRepeatMode, bool bShuffle)
{
	if (g_nCurItem < g_nTotalItems)
	{
		if (bShuffle)
		{
			g_nCurItem = rand()%g_nTotalItems;
		}
		else if (nRepeatMode == 1)
		{
			; // replay the current one
		}
		else if (nRepeatMode == 2)
		{
			if (g_nCurItem+1 == g_nTotalItems)
			{
				g_nCurItem = 0;
			}
			else
			{
				g_nCurItem += 1;
			}
		}
		else
		{
			if (g_nCurItem+1 < g_nTotalItems)
			{
				g_nCurItem += 1;
			}
		}

		g_nCurPage = g_nCurItem/PLAYLIST_ITEM_PERPAGE;

		// refresh the list display
		RECT rc;
		GetClientRect(g_hWndPlayList, &rc);
		InvalidateRect(g_hWndPlayList, &rc, FALSE);

		return g_nCurItem;
	}

	return -1;
}

//
// go to the previous item
//
int PlayListPrevious(int nRepeatMode, bool bShuffle)
{
	if (g_nCurItem > 0)
	{
		if (bShuffle)
		{
			g_nCurItem = rand()%g_nTotalItems;
		}
		else if (nRepeatMode == 1)
		{
			;
		}
		else
		{
			g_nCurItem -= 1;
		}

		g_nCurPage = g_nCurItem/PLAYLIST_ITEM_PERPAGE;

		// refresh the list display
		RECT rc;
		GetClientRect(g_hWndPlayList, &rc);
		InvalidateRect(g_hWndPlayList, &rc, FALSE);

		return g_nCurItem;
	}

	return -1;
}

//
// get item index of current selection
//
int PlayListCurrent(void)
{
	if (g_nCurItem < g_nTotalItems)
	{
		return g_nCurItem;
	}

	return -1;
}

//
// get the music file path
//
wchar_t *GetMediaFilePath(UINT nID)
{
	if (nID < g_nTotalItems)
	{
		RETAILMSG(OutputMsg, (L"GetMediaFilePath [%d]: %s\r\n", nID, g_lpListItem[g_nCurItem].wszPath));
		if (g_playList.CheckExistence(g_lpListItem[g_nCurItem].wszPath))
		{
			return g_lpListItem[g_nCurItem].wszPath;
		}
	}

	return NULL;
}