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
//#include "PlayList.h"
#include "Duration.h"
#include "Video.h"
#include "UserDef.h"

#ifdef DEBUG
#pragma comment(lib, "..\\Common\\AImage\\Debug\\AImage.lib")
#pragma comment(lib, "..\\Common\\AMedia\\Debug\\AMedia.lib")
#else
#pragma comment(lib, "..\\Common\\AImage\\Release\\AImage.lib")
#pragma comment(lib, "..\\Common\\AMedia\\Release\\AMedia.lib")
#endif // DEBUG

#define DEAFULT_MEDIA_LOCATION TEXT("USB Disk\\MEDIA\\")
#define DEAFULT_MEDIA_LOCATION2 TEXT("USB Disk2\\MEDIA\\")

//
// macro definitions
//
#ifdef DEBUG
#define OutputMsg 1
#else
#define OutputMsg 0
#endif // DEBUG

#ifdef PLAY_TYPE_MUSIC//
#define IDS_WND_CLASS TEXT("MUSICPLAYER")
#define IDS_WND_TITLE TEXT("Music Player")
#else
#ifdef PLAY_TYPE_VIDEO
#define IDS_WND_CLASS TEXT("VIDEOPLAYER")
#define IDS_WND_TITLE TEXT("Video Player")
#endif // PLAY_TYPE_VIDEO
#endif // PLAY_TYPE_MUSIC

//
// global variables
//
extern HINSTANCE hInst;
HWND g_hMPWnd = NULL;
wchar_t *g_lpwszMediaPath = NULL;

// child windows
HWND g_hWndDuration = NULL;
HWND g_hWndList = NULL; 
HWND g_hWndVideo = NULL; 

CAMedia g_mediaPlay;
CCoImage g_uiImages;
MP3ID3V1TAG g_mp3Tag[1] = {0};

UINT g_nRepeatMode = 2;
bool g_bShuffle = true;
bool g_bShowList = true;
UINT g_nVideoMode = 0;

BOOL bMediaScanIsDone = FALSE;

UINT g_nItems = 0; // number of Media files
UINT g_nCurItems = 0; // current item
MEDIA_LIST_ITEM *g_lpItems = NULL; // Media list

// playing control item identifier
enum enumPlayingControlID {
	//PCID_VOLUME,
	//PCID_REPEAT,
	PCID_SHUFFLE,
	//PCID_PREVIOUS,
	//PCID_PLAYPAUSE,
	//PCID_STOP,
	//PCID_NEXT,
	//PCID_LIST,
	//PCID_EXIT,
};

// playing control button structure
struct BUTTON_CTRL_STRUCT g_playCtrl[] = {
//	PCID_VOLUME, {PLAYER_WND_WIDTH-PLAYER_MENUBTN_WIDTH, 0, PLAYER_WND_WIDTH, PLAYER_TITLEBAR_HEIGHT}, L"Volume", PLAYER_IMG_VOLUME, PLAYER_IMG_BTN_FOCUS,
//	PCID_REPEAT, {0, PLAYER_MENUBTN_TOP, PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_BOTTOM}, L"Repeat", PLAYER_IMG_REPEAT, PLAYER_IMG_CYCLE_FOCUS,
	PCID_SHUFFLE, {PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_TOP, 2*PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_BOTTOM}, L"Shuffle", PLAYER_IMG_SHUFFLE, PLAYER_IMG_CYCLE_FOCUS,
//	PCID_PREVIOUS, {PLAYER_MENUBTN_MAINLEFT, PLAYER_MENUBTN_TOP, PLAYER_MENUBTN_MAINLEFT+PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_BOTTOM}, L"Previous", PLAYER_IMG_PREVIOUS, PLAYER_IMG_BTN_FOCUS,
//	PCID_PLAYPAUSE, {PLAYER_MENUBTN_MAINLEFT+PLAYER_MENUBTN_WIDTH+PLAYER_MENUBTN_MAINSPACE, PLAYER_MENUBTN_TOP, PLAYER_MENUBTN_MAINLEFT+2*PLAYER_MENUBTN_WIDTH+PLAYER_MENUBTN_MAINSPACE, PLAYER_MENUBTN_BOTTOM}, L"play/Pause", PLAYER_IMG_PLAY, PLAYER_IMG_BTN_FOCUS,
//	PCID_STOP, {PLAYER_MENUBTN_MAINLEFT+2*PLAYER_MENUBTN_WIDTH+2*PLAYER_MENUBTN_MAINSPACE, PLAYER_MENUBTN_TOP, PLAYER_MENUBTN_MAINLEFT+3*PLAYER_MENUBTN_WIDTH+2*PLAYER_MENUBTN_MAINSPACE, PLAYER_MENUBTN_BOTTOM}, L"Stop", PLAYER_IMG_STOP, PLAYER_IMG_BTN_FOCUS,
//	PCID_NEXT, {PLAYER_MENUBTN_MAINLEFT+3*PLAYER_MENUBTN_WIDTH+3*PLAYER_MENUBTN_MAINSPACE, PLAYER_MENUBTN_TOP, PLAYER_MENUBTN_MAINLEFT+4*PLAYER_MENUBTN_WIDTH+3*PLAYER_MENUBTN_MAINSPACE, PLAYER_MENUBTN_BOTTOM}, L"Next", PLAYER_IMG_NEXT, PLAYER_IMG_BTN_FOCUS,
//	PCID_LIST, {PLAYER_WND_WIDTH-2*PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_TOP, PLAYER_WND_WIDTH-PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_BOTTOM}, L"List", PLAYER_IMG_LIST, PLAYER_IMG_CYCLE_FOCUS,
//	PCID_EXIT, {PLAYER_WND_WIDTH-PLAYER_MENUBTN_WIDTH, PLAYER_MENUBTN_TOP, PLAYER_WND_WIDTH, PLAYER_MENUBTN_BOTTOM}, L"Exit", PLAYER_IMG_EXIT, PLAYER_IMG_CYCLE_FOCUS,
};

LRESULT CALLBACK MediaWndProc(HWND, UINT, WPARAM, LPARAM);
void OnCtrlBtnPress(HWND hWnd, HDC hdc, UINT nIDCtrl);

UINT SearchMediaFiles(wchar_t *lpwszPath, MEDIA_LIST_ITEM **lppItem);
bool MediaTypeFilter(wchar_t *lpwszFile);
int PlayListNext(int nRepeatMode, bool bShuffle);
int PlayListCurrent(void);
wchar_t *GetMediaFilePath(UINT nID);
void SetMediaFilePath(wchar_t *lpwszFile);
bool IsAudioFile(wchar_t *lpwszFile);

//
// message handler
//
LRESULT OnMPWndCreate(HWND, UINT, WPARAM, LPARAM);
LRESULT OnMPWndDestroy(HWND, UINT, WPARAM, LPARAM);
LRESULT OnMPWndPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT OnMPWndTimer(HWND, UINT, WPARAM, LPARAM);
LRESULT OnMPWndLBtnDown(HWND, UINT, WPARAM, LPARAM);
LRESULT OnMPWndLBtnUp(HWND, UINT, WPARAM, LPARAM);
LRESULT OnMPWndMediaNotify(HWND, UINT, WPARAM, LPARAM);

//
// message map
//
const struct decodeUINT g_stMsg[] = {
	WM_CREATE, OnMPWndCreate,
	WM_DESTROY, OnMPWndDestroy,
	WM_PAINT, OnMPWndPaint,
	WM_TIMER, OnMPWndTimer,
	WM_LBUTTONDOWN, OnMPWndLBtnDown,
	WM_LBUTTONUP, OnMPWndLBtnUp,
	WM_MEDIALIB_NOTIFY, OnMPWndMediaNotify,
};

//
//  registers the window class
//
BOOL MediaRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = MediaWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = IDS_WND_CLASS;

	if (!RegisterClass (&wc)) 
	{
		return FALSE; 
	}
	return TRUE;
}

BOOL ShowMediaWnd(HWND hWnd)
{
	// Create main window.
    g_hMPWnd = CreateWindow (IDS_WND_CLASS,            // Window class
							IDS_WND_TITLE, // Window title
							WS_VISIBLE|WS_CHILD,           // Style flags
							MW_WIN_LEFT,        // x position
							MW_WIN_TOP,        // y position
							MW_WIN_WIDTH,        // Initial width
							MW_WIN_HEIGHT,        // Initial height
							hWnd,                 // Parent
							NULL,                 // Menu, must be null
							hInst,            // Application instance
							NULL);                // Pointer to create
                                               // parameters
    // Return fail code if window not created.
    if (!IsWindow (g_hMPWnd)) return FALSE;

    // Standard show and update calls
    ShowWindow (g_hMPWnd, TRUE);
    UpdateWindow (g_hMPWnd);

    return TRUE;
}

//
// process message for the main window.
//
LRESULT CALLBACK MediaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
LRESULT OnMPWndCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	g_hWndList = CreatePlayListWindow(hWnd, hInst);
	g_hWndDuration = CreateDurationWindow(hWnd, hInst);
	//if(!IsAudioFile(lpwszFile))
	{
#ifdef PLAY_TYPE_VIDEO
		g_hWndVideo = CreateVideoWindow(hWnd, hInst);
#endif // PLAY_TYPE_VIDEO	
	}
	ShowWindow(g_hWndDuration, SW_HIDE);
	ShowWindow(g_hWndVideo, SW_HIDE);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_DESTROY
//
LRESULT OnMPWndDestroy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PostQuitMessage(0);
	return 0;
}

//
// WM_PAINT
//
LRESULT OnMPWndPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	// background
#if 1
	HBRUSH hBrush = CreateSolidBrush(RGB(20,0,0));
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hdcMem, hBrush);
	FillRect(hdcMem, &rcClient, hBrush);
	SelectObject(hdcMem, hBrushOld);
	DeleteObject(hBrush);
#else
	g_uiImages.CoImgDisplayIcon(PLAYER_IMG_BACKGROUND, hdcMem, &rcClient);
#endif 

	// title bar
	//RECT rcTop = {0, 0, PLAYER_WND_WIDTH, PLAYER_TITLEBAR_HEIGHT};
	//g_uiImages.CoImgDisplayIcon(PLAYER_IMG_TOP, hdcMem, &rcTop);
	//// bottom menu bar
	//RECT rcBottom = {0, PLAYER_WND_HEIGHT-PLAYER_MENUBAR_HEIGHT, PLAYER_WND_WIDTH, PLAYER_WND_HEIGHT};
	//g_uiImages.CoImgDisplayIcon(PLAYER_IMG_BOTTOM, hdcMem, &rcBottom);

	// draw the button images
	//for (int i = 0; i < dim(g_playCtrl); i++)
	//{
	//	if (i == PCID_REPEAT)
	//	{
	//		if (g_nRepeatMode == 0)
	//		{
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//		else if (g_nRepeatMode == 1)
	//		{
	//			g_uiImages.CoImgDisplayIcon(PLAYER_IMG_REPEAT_ONE, hdcMem, &g_playCtrl[i].rcCtrl);
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImgFocus, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//		else if (g_nRepeatMode == 2)
	//		{
	//			g_uiImages.CoImgDisplayIcon(PLAYER_IMG_REPEAT_ALL, hdcMem, &g_playCtrl[i].rcCtrl);
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImgFocus, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//	}
	//	else if (i == PCID_SHUFFLE)
	//	{
	//		if (g_bShuffle)
	//		{
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImgFocus, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//		else
	//		{
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//	}
	//	else if (i == PCID_LIST)
	//	{
	//		if (g_bShowList)
	//		{
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImgFocus, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//		else
	//		{
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//	}
	//	else if (i == PCID_PLAYPAUSE)
	//	{
	//		UINT psMode = g_mediaPlay.GetPlayState();
	//		if (psMode == psStop || psMode == psPause)
	//		{
	//			g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//		else if (psMode == psPlaying)
	//		{
	//			g_uiImages.CoImgDisplayIcon(PLAYER_IMG_PAUSE, hdcMem, &g_playCtrl[i].rcCtrl);
	//		}
	//	}
	//	else
	//	{
	//		g_uiImages.CoImgDisplayIcon(g_playCtrl[i].lpwszImg, hdcMem, &g_playCtrl[i].rcCtrl);
	//	}
	//}

#ifdef PLAY_TYPE_MUSIC
	// album infomation
	RECT rcAlbumCover = {PLAYER_ALBUM_COVER_LEFT, PLAYER_ALBUM_COVER_TOP, 
		PLAYER_ALBUM_COVER_LEFT+PLAYER_ALBUM_COVER_WIDTH, 
		PLAYER_ALBUM_WND_TOP+PLAYER_ALBUM_COVER_HEIGHT};
	g_uiImages.CoImgDisplayIcon(PLAYER_IMG_ALBUM, hdcMem, &rcAlbumCover);

	RECT rcAlbumInfo = {PLAYER_ALBUM_WND_LEFT, 
		PLAYER_ALBUM_WND_TOP, 
		PLAYER_ALBUM_WND_LEFT+PLAYER_ALBUM_WND_WIDTH, 
		PLAYER_ALBUM_WND_TOP+PLAYER_ALBUM_WND_HEIGHT};
#else
#ifdef PLAY_TYPE_VIDEO
	// show video window
	//ShowWindow(g_hWndVideo, SW_SHOW);
#endif // PLAY_TYPE_VIDEO
#endif //PLAY_TYPE_MUSIC

	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, TEXT_COLOR_WHITE);

	RECT rcTitle = {PLAYER_TITLE_LEFT,0,PLAYER_WND_WIDTH-PLAYER_ALBUM_WND_LEFT,PLAYER_TITLEBAR_HEIGHT};
	int nIndex = PlayListCurrent();
	if (nIndex >= 0)
	{
		// window title
		//wchar_t *lpwszPath = GetMediaFilePath(nIndex);
		if (g_nItems > 0)
		{
#if USEANALOGCLOCK
			DrawText(hdcMem, g_lpItems[nIndex].wszName, -1, &rcTitle, DT_RIGHT|DT_VCENTER);
#else
			DrawText(hdcMem, g_lpItems[nIndex].wszPath, -1, &rcTitle, DT_RIGHT|DT_VCENTER);
#endif

#ifdef PLAY_TYPE_MUSIC
			// title
			wchar_t wszItem[MAX_PATH] = {0};
			wchar_t wszInfo[32] = {0};

			int nLen = MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Title, -1, wszInfo, 0);
			MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Title, -1, wszInfo, nLen);
			wcscpy(wszItem, L"Title: ");
			wcscat(wszItem, wszInfo);

			RECT rcName = {rcAlbumInfo.left,rcAlbumInfo.top,rcAlbumInfo.right,rcAlbumInfo.top+PLAYER_ALBUMINFO_HEIGHT};
			DrawText(hdcMem, wszItem, -1, &rcName, DT_LEFT|DT_VCENTER);

			// artist
			ZeroMemory(wszItem, MAX_PATH);
			ZeroMemory(wszInfo, sizeof(wszInfo));

			nLen = MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Artist, -1, wszInfo, 0);
			MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Artist, -1, wszInfo, nLen);
			wcscpy(wszItem, L"Artist: ");
			wcscat(wszItem, wszInfo);

			RECT rcArtist = {rcAlbumInfo.left,rcAlbumInfo.top+PLAYER_ALBUMINFO_HEIGHT,rcAlbumInfo.right,rcAlbumInfo.top+2*PLAYER_ALBUMINFO_HEIGHT};
			DrawText(hdcMem, wszItem, -1, &rcArtist, DT_LEFT|DT_VCENTER);

			// album
			ZeroMemory(wszItem, MAX_PATH);
			ZeroMemory(wszInfo, sizeof(wszInfo));
			
			nLen = MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Album, -1, wszInfo, 0);
			MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Album, -1, wszInfo, nLen);
			wcscpy(wszItem, L"Album: ");
			wcscat(wszItem, wszInfo);

			RECT rcAlbum = {rcAlbumInfo.left,rcAlbumInfo.top+2*PLAYER_ALBUMINFO_HEIGHT,rcAlbumInfo.right,rcAlbumInfo.top+3*PLAYER_ALBUMINFO_HEIGHT};
			DrawText(hdcMem, wszItem, -1, &rcAlbum, DT_LEFT|DT_VCENTER);

			// year
			ZeroMemory(wszItem, MAX_PATH);
			ZeroMemory(wszInfo, sizeof(wszInfo));
			
			nLen = MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Year, -1, wszInfo, 0);
			MultiByteToWideChar(CP_UTF8, 0, g_mp3Tag->Year, -1, wszInfo, nLen);
			wcscpy(wszItem, L"Year: ");
			wcsncat(wszItem, wszInfo, 4);

			RECT rcYear = {rcAlbumInfo.left,rcAlbumInfo.top+3*PLAYER_ALBUMINFO_HEIGHT,rcAlbumInfo.right,rcAlbumInfo.top+4*PLAYER_ALBUMINFO_HEIGHT};
			DrawText(hdcMem, wszItem, -1, &rcYear, DT_LEFT|DT_VCENTER);
#endif // PLAY_TYPE_MUSIC
		}
		else
		{
			DrawText(hdcMem, L"Can not find the specified file.", -1, &rcTitle, DT_LEFT|DT_VCENTER);
		}
	}
	
	BitBlt(hdc, rcClient.left, rcClient.top, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, 
		hdcMem, rcClient.left, rcClient.top, SRCCOPY);

	SelectObject(hdcMem, hBmpOld);
	DeleteObject(hBitmap);
	hBitmap = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	// play list window
	//if (g_bShowList)
	//{
	//	ShowWindow(g_hWndList, SW_SHOW);
	//}

	EndPaint(hWnd, &ps);
	return 0;
}

//
// WM_TIMER
//
LRESULT OnMPWndTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// WM_LBUTTONDOWN
//
LRESULT OnMPWndLBtnDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	for (int i = 0; i < dim(g_playCtrl); i++)
	{
		if (x > g_playCtrl[i].rcCtrl.left  && x < g_playCtrl[i].rcCtrl.right
			&& y > g_playCtrl[i].rcCtrl.top  && y < g_playCtrl[i].rcCtrl.bottom)
		{
			OnCtrlBtnPress(hWnd, hdc, i);
			break;
		}
	}

	ReleaseDC(hWnd, hdc);
	return 0;
}

//
// WM_LBUTTONUP
//
LRESULT OnMPWndLBtnUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	InvalidateRect(hWnd, &rc, FALSE);

	/*int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	for (int i = 0; i < dim(g_playCtrl); i++)
	{
		if (x > g_playCtrl[i].rcCtrl.left  && x < g_playCtrl[i].rcCtrl.right
			&& y > g_playCtrl[i].rcCtrl.top  && y < g_playCtrl[i].rcCtrl.bottom)
		{
			RECT rcBtn = {g_playCtrl[i].rcCtrl.left, g_playCtrl[i].rcCtrl.top, 
				g_playCtrl[i].rcCtrl.right, g_playCtrl[i].rcCtrl.bottom};
			InvalidateRect(hWnd, &rcBtn, FALSE);
			break;
		}
	}*/

	return 0;
}

//
// WM_MEDIALIB_NOTIFY
//
LRESULT OnMPWndMediaNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (S_OK == g_mediaPlay.CompleteEventNotify())
	{
		RETAILMSG(1, (L"[OnDShowEvent]: PLAY COMPLETE\r\n")); 
		g_mediaPlay.Stop(); // make sure the playing is stopped

		int nIndex = PlayListNext(g_nRepeatMode, g_bShuffle);//why can not be -1
		if (nIndex >= 0)
		{
			PlayMediaFile(GetMediaFilePath(nIndex));
		}
		else
		{
			// refresh control button states
			RECT rc;
			GetClientRect(g_hMPWnd, &rc);
			InvalidateRect(g_hMPWnd, &rc, FALSE);

			// refresh file list window
			//PlayListRefresh();

			// refresh duration window
			//RECT rcDuration;
			//GetClientRect(g_hWndDuration, &rcDuration);
			//InvalidateRect(g_hWndDuration, &rcDuration, FALSE);
		}
	}

	return 0; 
}

//
// handle the control button press event
//
void OnCtrlBtnPress(HWND hWnd, HDC hdc, UINT nIDCtrl)
{
	RECT rcBtn = {g_playCtrl[nIDCtrl].rcCtrl.left, g_playCtrl[nIDCtrl].rcCtrl.top, 
		g_playCtrl[nIDCtrl].rcCtrl.right, g_playCtrl[nIDCtrl].rcCtrl.bottom};

	switch (nIDCtrl) {
		case PCID_SHUFFLE:
			if (g_bShuffle)
			{
				g_bShuffle = false;
			}
			else
			{
				g_nRepeatMode = 0;
				g_bShuffle = true;
			}
			g_uiImages.CoImgDisplayIcon(g_playCtrl[nIDCtrl].lpwszImgFocus, hdc, &rcBtn);
			break;
		//case PCID_PLAYPAUSE:
		//	if (g_mediaPlay.GetPlayState() == psPause)
		//	{
		//		g_uiImages.CoImgDisplayIcon(g_playCtrl[nIDCtrl].lpwszImgFocus, hdc, &rcBtn);
		//		g_mediaPlay.Play();
		//	}
		//	else if (g_mediaPlay.GetPlayState() == psPlaying)
		//	{
		//		g_uiImages.CoImgDisplayIcon(g_playCtrl[nIDCtrl].lpwszImgFocus, hdc, &rcBtn);
		//		g_mediaPlay.Pause();
		//	}
		//	else
		//	{
		//		int nIndex = PlayListCurrent();
		//		if (nIndex == -1)
		//		{
		//			nIndex = PlayListNext(g_nRepeatMode, g_bShuffle);
		//		}

		//		if (nIndex >= 0)
		//		{
		//			g_uiImages.CoImgDisplayIcon(g_playCtrl[nIDCtrl].lpwszImgFocus, hdc, &rcBtn);
		//			PlayMediaFile(GetMediaFilePath(nIndex));
		//		}
		//	}
		//	break;
		//case PCID_STOP:
		//	{
		//		g_uiImages.CoImgDisplayIcon(g_playCtrl[nIDCtrl].lpwszImgFocus, hdc, &rcBtn);
		//		g_mediaPlay.Stop();
		//		
		//		RECT rcDuration;
		//		GetClientRect(g_hWndDuration, &rcDuration);
		//		InvalidateRect(g_hWndDuration, &rcDuration, FALSE);

		//		RECT rcClient;
		//		GetClientRect(hWnd, &rcClient);
		//		InvalidateRect(hWnd, &rcClient, FALSE);
		//	}
		//	break;
		default:
			break;
	}
}

//
// play a specified media file
//
void PlayMediaFile(wchar_t *lpwszFile)
{
	RETAILMSG(1, (L"Play media file: %s\r\n", lpwszFile));
	if (!lpwszFile)
	{
		g_mediaPlay.Stop();
		RECT rcDuration;
		GetClientRect(g_hWndDuration, &rcDuration);
		InvalidateRect(g_hWndDuration, &rcDuration, FALSE);
		goto END;
	}

#ifdef PLAY_TYPE_MUSIC
	g_mediaPlay.GetMP3ID3V1Tag(lpwszFile, g_mp3Tag);
	g_mediaPlay.PlayFile(g_hMPWnd, lpwszFile);
#else
#ifdef PLAY_TYPE_VIDEO
	if(!IsAudioFile(lpwszFile))
	{
		ShowWindow(g_hWndVideo, SW_SHOW);
	}
	else
	{
		ShowWindow(g_hWndVideo, SW_HIDE);
	}
	g_mediaPlay.PlayFile(g_hMPWnd, lpwszFile);
	RECT rcVideo;
	GetClientRect(g_hWndVideo, &rcVideo);

	if (g_nVideoMode == 2)
	{
		g_mediaPlay.SetVideoWindowPos(g_hWndVideo, rcVideo, true);
	}
	else
	{
		g_mediaPlay.SetVideoWindowPos(g_hWndVideo, rcVideo, false);
	}

#endif // PLAY_TYPE_VIDEO
#endif // PLAY_TYPE_MUSIC
	g_mediaPlay.Play();

END:
	RECT rcClient;
	GetClientRect(g_hMPWnd, &rcClient);
	InvalidateRect(g_hMPWnd, &rcClient, FALSE);
}

void ResetVideoWindow()
{
	RECT rcVideo;

	if (g_nVideoMode == 0) // normal -> hide list
	{
		ShowWindow(g_hWndList, SW_HIDE);
		g_bShowList = false;

		if (g_mediaPlay.GetPlayState() == psPlaying)
		{
			g_mediaPlay.Pause();
			SetWindowPos(g_hWndVideo, NULL, VIDEO_WND_LEFT, VIDEO_WND_TOP,
				 PLAYER_WND_WIDTH-2*VIDEO_WND_LEFT, VIDEO_WND_HEIGHT, SWP_SHOWWINDOW);
			GetClientRect(g_hWndVideo, &rcVideo);
			g_mediaPlay.SetVideoWindowPos(g_hWndVideo, rcVideo, false);
			g_mediaPlay.Play();
		}
		g_nVideoMode = 1;
	}
	else if (g_nVideoMode == 1) // hide list -> fullscreen
	{
		ShowWindow(g_hWndList, SW_HIDE);
		XACStop();
		g_bShowList = false;
		if (g_mediaPlay.GetPlayState() == psPlaying)
		{
				g_mediaPlay.Pause();
				GetClientRect(g_hMPWnd, &rcVideo);
				SetWindowPos(g_hWndVideo, NULL, rcVideo.left, rcVideo.bottom, rcVideo.right-rcVideo.left,
					rcVideo.bottom-rcVideo.top, SWP_SHOWWINDOW);
				GetClientRect(g_hWndVideo, &rcVideo);
				g_mediaPlay.SetVideoWindowPos(g_hWndVideo, rcVideo, true);
				g_mediaPlay.Play();
		}
		g_nVideoMode = 2;
	}
	else if (g_nVideoMode == 2) // fullscreen->normal
	{
		ShowWindow(g_hWndList, SW_SHOW);
		XACStart();
		g_bShowList = true;
		if (g_mediaPlay.GetPlayState() == psPlaying)
		{
			g_mediaPlay.Pause();
			SetWindowPos(g_hWndVideo, NULL, VIDEO_WND_LEFT, VIDEO_WND_TOP,
				VIDEO_WND_WIDTH, VIDEO_WND_HEIGHT, SWP_SHOWWINDOW);
			GetClientRect(g_hWndVideo, &rcVideo);
			g_mediaPlay.SetVideoWindowPos(g_hWndVideo, rcVideo, false);
			g_mediaPlay.Play();
		}
		g_nVideoMode = 0;
	}

	RECT rcClient;
	GetClientRect(g_hMPWnd, &rcClient);
	InvalidateRect(g_hMPWnd, &rcClient, FALSE);
}

PLAYSTATE GetPlayState()
{
	return g_mediaPlay.GetPlayState();
}

HRESULT GetMediaDuration(LONGLONG *llDuration)
{
	return g_mediaPlay.GetDuration(llDuration);
}

HRESULT GetCurrentPosition(LONGLONG *llPosition)
{
	return g_mediaPlay.GetCurrentPosition(llPosition);
}

HRESULT JumpToPosition(LONGLONG llPosition)
{
	return g_mediaPlay.JumpToPosition(&llPosition);
}

//
// search media files on certain location
//
UINT SearchMediaFiles(wchar_t *lpwszPath, MEDIA_LIST_ITEM **lppItem)
{
	WIN32_FIND_DATA wfd;
	wchar_t wszPath[MAX_PATH] = {0};

	wcscpy(wszPath, lpwszPath);
	wcscat(wszPath, L"*.*");

	HANDLE hFind = FindFirstFile(wszPath, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	int nCount = -1;
	MEDIA_LIST_ITEM *lpItem = NULL;

	if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY && MediaTypeFilter(wfd.cFileName))
	{
		nCount = 0;
		lpItem = new MEDIA_LIST_ITEM[1];
		lpItem->nIndex = nCount;
		wcscpy(lpItem[lpItem->nIndex ].wszName, wfd.cFileName);
		wcscpy(lpItem[lpItem->nIndex ].wszPath, lpwszPath);
		wcscat(lpItem[lpItem->nIndex ].wszPath, wfd.cFileName);
		//RETAILMSG(1, (L"Media file [%d] %s\r\n", nCount, lpItem[nCount].wszName));
	}

	while (FindNextFile(hFind, &wfd))
	{
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY && MediaTypeFilter(wfd.cFileName))
		{
			MEDIA_LIST_ITEM *lpTemp = new MEDIA_LIST_ITEM[nCount+1];
			memcpy(lpTemp, lpItem, (nCount+1)*sizeof(MEDIA_LIST_ITEM));

			nCount += 1;
			delete []lpItem;

			lpItem = new MEDIA_LIST_ITEM[nCount+1];
			memcpy(lpItem, lpTemp, nCount*sizeof(MEDIA_LIST_ITEM));
			delete []lpTemp;

			lpItem->nIndex = nCount;
			wcscpy(lpItem[lpItem->nIndex].wszName, wfd.cFileName);
			wcscpy(lpItem[lpItem->nIndex].wszPath, lpwszPath);
			wcscat(lpItem[lpItem->nIndex].wszPath, wfd.cFileName);
			RETAILMSG(1, (L"Media file [%d] %s\r\n", nCount, lpItem[lpItem->nIndex].wszName));
		}
	}

	*lppItem = lpItem;
	FindClose(hFind);
	return (nCount+1);
}

UINT FindMediaFiles(int index)
{	
	if(index == 0)
	{
		g_nItems = SearchMediaFiles(DEAFULT_MEDIA_LOCATION, &g_lpItems);
	}
	else if(index == 1)//USB DISK2
	{
		g_nItems = SearchMediaFiles(DEAFULT_MEDIA_LOCATION2, &g_lpItems);
	}
	bMediaScanIsDone = TRUE;
	RECT rcClient;
	GetClientRect(g_hMPWnd, &rcClient);
	InvalidateRect(g_hMPWnd, &rcClient, FALSE);
	return g_nItems;
}

//
// media file type filter
//
bool MediaTypeFilter(wchar_t *lpwszFile)
{
	int nLen = wcslen(lpwszFile);
	if (!wcscmp(lpwszFile + nLen - 4, L".mp3") ||
		!wcscmp(lpwszFile + nLen - 4, L".MP3") ||
		!wcscmp(lpwszFile + nLen - 4, L".wav") ||
		!wcscmp(lpwszFile + nLen - 4, L".WAV") ||
		!wcscmp(lpwszFile + nLen - 4, L".m4a") ||
		!wcscmp(lpwszFile + nLen - 4, L".M4A") ||
		!wcscmp(lpwszFile + nLen - 4, L".mp4") ||
		!wcscmp(lpwszFile + nLen - 4, L".MP4") ||
		!wcscmp(lpwszFile + nLen - 4, L".avi") ||
		!wcscmp(lpwszFile + nLen - 4, L".AVI") ||
		!wcscmp(lpwszFile + nLen - 4, L".wmv") ||
		!wcscmp(lpwszFile + nLen - 4, L".WMV") ||		
		!wcscmp(lpwszFile + nLen - 5, L".rmvb") ||
		!wcscmp(lpwszFile + nLen - 5, L".RMVB"))
	{
		return true;
	}

	return false;
}

bool IsAudioFile(wchar_t *lpwszFile)
{
	int nLen = wcslen(lpwszFile);
	if (!wcscmp(lpwszFile + nLen - 4, L".mp3") ||
		!wcscmp(lpwszFile + nLen - 4, L".MP3") ||
		!wcscmp(lpwszFile + nLen - 4, L".wav") ||
		!wcscmp(lpwszFile + nLen - 4, L".WAV") ||
		!wcscmp(lpwszFile + nLen - 4, L".m4a") ||
		!wcscmp(lpwszFile + nLen - 4, L".M4A"))
	{
		return true;
	}

	return false;
}

bool NextMedia()
{
	if (g_nCurItems < g_nItems-1)
	{
		g_nCurItems += 1;
		SetMediaFilePath(g_lpItems[g_nCurItems].wszPath);
		return true;
	}

	return false;
}

bool PreviousMedia()
{
	if (g_nCurItems > 0)
	{
		g_nCurItems -= 1;
		SetMediaFilePath(g_lpItems[g_nCurItems].wszPath);
		return true;
	}

	return false;
}

void SetMediaFilePath(wchar_t *lpwszFile)
{
	g_lpwszMediaPath = lpwszFile;
}

int PlayListNext(int nRepeatMode, bool bShuffle)
{
	if (g_nCurItems < g_nItems)
	{
		if (bShuffle)
		{
			g_nCurItems = rand()%g_nItems;
		}
		else if (nRepeatMode == 1)
		{
			; // replay the current one
		}
		else if (nRepeatMode == 2)
		{
			if (g_nCurItems+1 == g_nItems)
			{
				g_nCurItems = 0;
			}
			else
			{
				g_nCurItems += 1;
			}
		}
		else
		{
			if (g_nCurItems+1 < g_nItems)
			{
				g_nCurItems += 1;
			}
			else
			{
				return -1;
			}
		}

		return g_nCurItems;
	}

	return -1;
}

int PlayListCurrent(void)
{
	if (g_nCurItems < g_nItems)
	{
		return g_nCurItems;
	}

	return -1;
}

wchar_t *GetMediaFilePath(UINT nID)
{
	if (nID < g_nItems)
	{
//		RETAILMSG(OutputMsg, (L"GetMediaFilePath [%d]: %s\r\n", nID, g_lpListItem[g_nCurItem].wszPath));
//		if (g_playList.CheckExistence(g_lpListItem[g_nCurItems].wszPath))
		{
			return g_lpItems[nID].wszPath;
		}
	}

	return NULL;
}

void PlayPauseMedia()
{
	if(!bMediaScanIsDone)
		return;
	ShowWindow(g_hWndDuration, SW_SHOW);
	if (g_mediaPlay.GetPlayState() == psPause)
	{
		g_mediaPlay.Play();
	}
	else if (g_mediaPlay.GetPlayState() == psPlaying)
	{
		g_mediaPlay.Pause();
	}
	else
	{
		int nIndex = PlayListCurrent();
		if (nIndex == -1)
		{
			nIndex = PlayListNext(g_nRepeatMode, g_bShuffle);
		}

		if (nIndex >= 0)
		{
			PlayMediaFile(GetMediaFilePath(nIndex));
		}
	}
}

void PauseMedia()
{
	if(!bMediaScanIsDone)
		return;
	if (g_mediaPlay.GetPlayState() == psPause)
	{
		; //Do nothing
	}
	else if (g_mediaPlay.GetPlayState() == psPlaying)
	{
		g_mediaPlay.Pause();
	}	
}

void PlayMedia()
{
	if(!bMediaScanIsDone)
		return;
	if (g_mediaPlay.GetPlayState() == psPause)
	{
		g_mediaPlay.Play();
	}
	else if (g_mediaPlay.GetPlayState() == psPlaying)
	{
		; //Do nothing
	}
	else
	{
		int nIndex = PlayListCurrent();
		if (nIndex == -1)
		{
			nIndex = PlayListNext(g_nRepeatMode, g_bShuffle);
		}

		if (nIndex >= 0)
		{
			PlayMediaFile(GetMediaFilePath(nIndex));
		}
	}
}
