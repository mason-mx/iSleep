/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/

#pragma once
#include <windows.h>
#include "resource.h"
#include "resdefine.h"

#include "..\Common\AImage\AImage.h"
#include "..\Common\AMedia\AMedia.h"

//
// macro definitions
//
#define dim(x) (sizeof(x) / sizeof(x[0]))
#define SAFE_DELETE_OBJECT(x) if (x) {DeleteObject(x); x = NULL;}
#define TEXT_COLOR_WHITE RGB(255,255,255) // text color

// structure associates message with functions
struct decodeUINT {                       
	UINT nID;                                
	LRESULT (*Fxn)(HWND, UINT, WPARAM, LPARAM);
};

// control button struct
struct BUTTON_CTRL_STRUCT {
	UINT nIDCtrl; // control item identifier
	RECT rcCtrl; // control item position rectangle
	wchar_t *lpwszCaption; // menu caption
	wchar_t *lpwszImg; // the name of the default button image
	wchar_t *lpwszImgFocus; // the name of the focused button image
};

// item text rectangle struct
struct TEXT_RECT_STRUCT {
	UINT nID; // static text item identifier
	RECT rcItem; // text rectangle;
};

// media list item struct
struct MEDIA_LIST_ITEM {
	UINT nIndex; // item index identifier
	wchar_t wszName[MAX_PATH]; // media name
	wchar_t wszPath[MAX_PATH]; // media path
};

//
// external functions
//
void PlayMediaFile(wchar_t *lpwszFile);
void ResetVideoWindow(void);
PLAYSTATE GetPlayState();
HRESULT GetMediaDuration(LONGLONG *llDuration);
HRESULT GetCurrentPosition(LONGLONG *llPosition);
HRESULT JumpToPosition(LONGLONG llPosition);