/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:
	UI relative definitions

Author:
    Franklin January 2010
    
Revision History:
*/

#pragma once

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// external applications
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define APP_LAUNCH_VOLUME TEXT("USB Disk\\resource\\bin\\Volume.exe")
#define  APP_LAUNCH_BROWSER TEXT("USB Disk\\resource\\bin\\RFileBrowser.exe")

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// coordinates
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define PLAYER_MENUBTN_WIDTH 64 // bottom menu button width
#define PLAYER_MENUBTN_HEIGHT 40 // bottom menu button height
#define PLAYER_MENUBTN_TOP (PLAYER_WND_HEIGHT-PLAYER_MENUBAR_HEIGHT+(PLAYER_MENUBAR_HEIGHT-PLAYER_MENUBTN_HEIGHT)/2)
#define PLAYER_MENUBTN_BOTTOM (PLAYER_MENUBTN_TOP+PLAYER_MENUBTN_HEIGHT)
// width and the left position of 4 main control buttons
#define PLAYER_MENUBTN_MAINSPACE COMMON_WIDTH
#define PLAYER_MENUBTN_MAINLEFT ((PLAYER_WND_WIDTH-4*PLAYER_MENUBTN_WIDTH-3*PLAYER_MENUBTN_MAINSPACE)/2)

// play list window coordinates
#define PLAYLIST_WND_WIDTH 320
#define PLAYLIST_WND_HEIGHT (DURATION_WND_TOP-2*COMMON_HEIGHT-PLAYER_TITLEBAR_HEIGHT)
#define PLAYLIST_WND_LEFT (WINDOW_WIDTH-PLAYLIST_WND_WIDTH-COMMON_WIDTH)
#define PLAYLIST_WND_TOP (PLAYER_TITLEBAR_HEIGHT+COMMON_HEIGHT)
#define PLAYLIST_SPACE 10

#define PLAYLIST_ITEM_HEIGHT 24

#define PLAYLIST_BTN_WIDTH 48
#define PLAYLIST_BTN_HEIGHT ((PLAYLIST_WND_HEIGHT-PLAYLIST_ITEM_HEIGHT)/6)
#define PLAYLIST_BTN_LEFT (PLAYLIST_WND_WIDTH-PLAYLIST_BTN_WIDTH)
#define PLAYLIST_BTN_RIGHT (PLAYLIST_BTN_LEFT+PLAYLIST_BTN_WIDTH)
#define PLAYLIST_BTN_TOP 0
#define PLAYLIST_BTN_BOTTOM PLAYLIST_WND_HEIGHT

#define PLAYLIST_ITEM_LEFT PLAYLIST_SPACE
#define PLAYLIST_ITEM_RIGHT (PLAYLIST_WND_WIDTH-PLAYLIST_BTN_WIDTH-PLAYLIST_SPACE)
#define PLAYLIST_ITEM_PERPAGE (PLAYLIST_WND_HEIGHT/PLAYLIST_ITEM_HEIGHT) // items per page
#define PLAYLIST_ITEM_TOP (PLAYLIST_WND_HEIGHT-PLAYLIST_ITEM_PERPAGE*PLAYLIST_ITEM_HEIGHT)/2
#define PLAYLIST_ITEM_BOTTOM (PLAYLIST_WND_HEIGHT-PLAYLIST_ITEM_TOP)

#define PLAYLIST_ITEM_TEXTLEFT (PLAYLIST_ITEM_LEFT+PLAYLIST_SPACE)
#define PLAYLIST_ITEM_TEXTRIGHT (PLAYLIST_ITEM_RIGHT-PLAYLIST_SPACE)

// duration image position
#define DURATION_IMG_LEFT 100
#define DURATION_IMG_TOP 12
#define DURATION_IMG_RIGHT (DURATION_WND_WIDTH-DURATION_IMG_LEFT)
#define DURATION_IMG_BOTTOM (DURATION_WND_HEIGHT-DURATION_IMG_TOP)
// seek image position
#define DURATION_SEEK_LEFT (DURATION_IMG_LEFT+1)
#define DURATION_SEEK_TOP (DURATION_IMG_TOP+1)
#define DURATION_SEEK_WIDTH 6
#define DURATION_SEEK_RIGHT (DURATION_WND_WIDTH-DURATION_IMG_LEFT-DURATION_SEEK_WIDTH-1)
#define DURATION_SEEK_BOTTOM (DURATION_IMG_BOTTOM-1)
// duration text position
#define DURATION_TEXT_LEFT 0
#define DURATION_TEXT_TOP 6
#define DURATION_TEXT_RIGHT DURATION_IMG_LEFT
#define DURATION_TEXT_BOTTOM (DURATION_WND_HEIGHT-DURATION_TEXT_TOP)
// playing seek text position
#define DURATION_SEEK_TEXT_LEFT DURATION_IMG_RIGHT
#define DURATION_SEEK_TEXT_TOP DURATION_TEXT_TOP
#define DURATION_SEEK_TEXT_RIGHT DURATION_WND_WIDTH
#define DURATION_SEEK_TEXT_BOTTOM DURATION_TEXT_BOTTOM

// video window coordinates
#define VIDEO_WND_WIDTH (320)
#define VIDEO_WND_HEIGHT (200)
#define VIDEO_WND_LEFT  (10)
#define VIDEO_WND_TOP  10

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// images
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// background images
#define PLAYER_IMG_BACKGROUND TEXT("USB Disk\\Resource\\AMediaPlayer\\background.png") 
#define PLAYER_IMG_TOP TEXT("USB Disk\\Resource\\AMediaPlayer\\top.png") 
#define PLAYER_IMG_BOTTOM TEXT("USB Disk\\Resource\\AMediaPlayer\\bottom.png") 
// album cover images
#define PLAYER_IMG_ALBUM TEXT("USB Disk\\Resource\\AMediaPlayer\\Album\\default.png") 
// button images
#define PLAYER_IMG_BTN_FOCUS TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\focus.png") 
#define PLAYER_IMG_CYCLE_FOCUS TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\focus_cycle.png") 
#define PLAYER_IMG_LIST TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\list.png")
#define PLAYER_IMG_REPEAT TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\repeat.png")
#define PLAYER_IMG_REPEAT_ONE TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\repeat_one.png")
#define PLAYER_IMG_REPEAT_ALL TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\repeat_all.png")
#define PLAYER_IMG_SHUFFLE TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\shuffle.png")
#define PLAYER_IMG_PREVIOUS TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\previous.png")
#define PLAYER_IMG_PLAY TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\play.png")
#define PLAYER_IMG_PAUSE TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\pause.png")
#define PLAYER_IMG_STOP TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\stop.png")
#define PLAYER_IMG_NEXT TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\next.png")
#define PLAYER_IMG_VOLUME TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\volume.png")
#define PLAYER_IMG_EXIT TEXT("USB Disk\\Resource\\AMediaPlayer\\Control\\exit.png")

// duration window images
#define DURATION_DURATION TEXT("USB Disk\\Resource\\AMediaPlayer\\duration.png") 
#define DURATION_SEEK TEXT("USB Disk\\Resource\\AMediaPlayer\\seek.png") 

// play list window images
#define PLAYLIST_IMG_ITEMBKGND TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\item-bg.png") 
#define PLAYLIST_IMG_ITEMFOCUS TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\item-focus.png") 
#define PLAYLIST_IMG_BTNBKGND TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\btn-bg.png") 
#define PLAYLIST_IMG_BTNFOCUS TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\btn-focus.png") 

#define PLAYLIST_IMG_PREVIOUS TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\previous.png")
#define PLAYLIST_IMG_NEXT TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\next.png")
#define PLAYLIST_IMG_MOVEUP TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\moveup.png")
#define PLAYLIST_IMG_MOVEDOWN TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\movedown.png")
#define PLAYLIST_IMG_REMOVE TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\remove.png")
#define PLAYLIST_IMG_ADD TEXT("USB Disk\\Resource\\AMediaPlayer\\List\\add.png")

