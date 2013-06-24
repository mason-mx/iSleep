/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/

#pragma once

// external functions
HWND CreatePlayListWindow(HWND hWndParent, HINSTANCE hInstance);
void PlayListRefresh(void);
int PlayListCurrent(void);
int PlayListNext(int nRepeatMode, bool bShuffle);
int PlayListPrevious(int nRepeatMode, bool bShuffle);
wchar_t *GetMediaFilePath(UINT nID);