#define FOR_DAMEI_VERSION 1
#define USEANALOGCLOCK 1

#define IDT_TIMER1		9001

#if FOR_DAMEI_VERSION

#undef USEANALOGCLOCK
#define USEANALOGCLOCK 1

#define PLAY_TYPE_MUSIC

//------------TimeWindow UI---------------
#define TW_TimeFontSize 260
#define TW_DateFontSize 80

#define TW_WIN_TOP 50
#define TW_WIN_LEFT 50
#define TW_WIN_WIDTH 921
#define TW_WIN_HEIGHT 463

#define TW_TIME_RECT_LEFT 0 
#define TW_TIME_RECT_TOP 0
#define TW_TIME_RECT_RIGHT 921
#define TW_TIME_RECT_BOTTOM 463

#define TW_DATE_RECT_LEFT 20 
#define TW_DATE_RECT_TOP 500
#define TW_DATE_RECT_RIGHT 921
#define TW_DATE_RECT_BOTTOM 668

//-------------XAnalogClock---------------
#define XAC_DateFontSize 20

#define DATE_BOX_WIDTH		39
#define DATE_BOX_HEIGHT		31
#define DATE_BOX_XPOS		530
#define DATE_BOX_YPOS		330
#define SECOND_HAND_LENGTH	260.0
#define MINUTE_HAND_LENGTH	230.0
#define HOUR_HAND_LENGTH	200.0
#define SECOND_HAND_COLOR	RGB(96,0,192)
#define MINUTE_HAND_COLOR	RGB(192,96,192)
#define HOUR_HAND_COLOR		RGB(255,251,239)

#define XAC_WIN_TOP 50
#define XAC_WIN_LEFT 50
#define XAC_WIN_WIDTH 690
#define XAC_WIN_HEIGHT 668

//-------------AlertWindow-------------
#define AW_AlertFontSize 28
#define AW_MiscFontSize 16

#define AW_WIN_LEFT 50
#define AW_WIN_TOP 518
#define AW_WIN_WIDTH 224
#define AW_WIN_HEIGHT 200

#define AW_ALERT_RECT_LEFT 10 
#define AW_ALERT_RECT_TOP 10
#define AW_ALERT_RECT_RIGHT 220
#define AW_ALERT_RECT_BOTTOM 100

#define AW_MISC_RECT_LEFT 10 
#define AW_MISC_RECT_TOP 110
#define AW_MISC_RECT_RIGHT 220
#define AW_MISC_RECT_BOTTOM 190

//-------------MediaWindow--------------
#define MW_WIN_LEFT 0
#define MW_WIN_TOP 0
#define MW_WIN_WIDTH 2
#define MW_WIN_HEIGHT 2

#define COMMON_HEIGHT 0 // height of the text string, space and other common purpose
#define COMMON_WIDTH 0 // width for common purpose use

// player window coordinates
#define PLAYER_WND_WIDTH MW_WIN_WIDTH
#define PLAYER_WND_HEIGHT MW_WIN_HEIGHT

#define PLAYER_TITLEBAR_HEIGHT 0 // title bar height
#define PLAYER_MENUBAR_HEIGHT 0 // bottom menu bar height
#define PLAYER_TITLE_LEFT COMMON_WIDTH // window caption string left position

// album info window position
#define PLAYER_ALBUM_WND_WIDTH (0)
#define PLAYER_ALBUM_WND_HEIGHT (0)
#define PLAYER_ALBUM_WND_LEFT COMMON_WIDTH
#define PLAYER_ALBUM_WND_TOP (PLAYER_TITLEBAR_HEIGHT+COMMON_HEIGHT)
// album cover position
#define PLAYER_ALBUM_COVER_WIDTH 0
#define PLAYER_ALBUM_COVER_HEIGHT 0
#define PLAYER_ALBUM_COVER_LEFT COMMON_WIDTH
#define PLAYER_ALBUM_COVER_TOP COMMON_HEIGHT
// album info text height
#define PLAYER_ALBUMINFO_HEIGHT COMMON_HEIGHT

//-------------DurationWindow------------
#define DURATION_WND_LEFT  2
#define DURATION_WND_TOP  0
#define DURATION_WND_WIDTH  2
#define DURATION_WND_HEIGHT  2

//-------------ImageVeiwerWindow------------
#define PV_WIN_LEFT 750
#define PV_WIN_TOP 290
#define PV_WIN_WIDTH 225
#define PV_WIN_HEIGHT 200

#else
#if USEANALOGCLOCK

#define PLAY_TYPE_VIDEO

//------------TimeWindow UI---------------
#define TW_TimeFontSize 0
#define TW_DateFontSize 0

#define TW_WIN_TOP 0
#define TW_WIN_LEFT 0
#define TW_WIN_WIDTH 0
#define TW_WIN_HEIGHT 0

#define TW_TIME_RECT_LEFT 0 
#define TW_TIME_RECT_TOP 0
#define TW_TIME_RECT_RIGHT 0
#define TW_TIME_RECT_BOTTOM 0

#define TW_DATE_RECT_LEFT 0 
#define TW_DATE_RECT_TOP 0
#define TW_DATE_RECT_RIGHT 0
#define TW_DATE_RECT_BOTTOM 0

//-------------XAnalogClock---------------
#define XAC_DateFontSize 20

#define DATE_BOX_WIDTH		39
#define DATE_BOX_HEIGHT		31
#define DATE_BOX_XPOS		530
#define DATE_BOX_YPOS		330
#define SECOND_HAND_LENGTH	260.0
#define MINUTE_HAND_LENGTH	230.0
#define HOUR_HAND_LENGTH	200.0
#define SECOND_HAND_COLOR	RGB(96,0,192)
#define MINUTE_HAND_COLOR	RGB(192,96,192)
#define HOUR_HAND_COLOR		RGB(255,251,239)

#define XAC_WIN_TOP 50
#define XAC_WIN_LEFT 50
#define XAC_WIN_WIDTH 690
#define XAC_WIN_HEIGHT 668

//-------------AlertWindow-------------
#define AW_AlertFontSize 28
#define AW_MiscFontSize 16

#define AW_WIN_LEFT 750
#define AW_WIN_TOP 50
#define AW_WIN_WIDTH 225
#define AW_WIN_HEIGHT 200

#define AW_ALERT_RECT_LEFT 10 
#define AW_ALERT_RECT_TOP 10
#define AW_ALERT_RECT_RIGHT 220
#define AW_ALERT_RECT_BOTTOM 100

#define AW_MISC_RECT_LEFT 10 
#define AW_MISC_RECT_TOP 110
#define AW_MISC_RECT_RIGHT 220
#define AW_MISC_RECT_BOTTOM 190

//-------------MediaWindow--------------
#define MW_WIN_LEFT 750
#define MW_WIN_TOP 500
#define MW_WIN_WIDTH 225
#define MW_WIN_HEIGHT 220

#define COMMON_HEIGHT 20 // height of the text string, space and other common purpose
#define COMMON_WIDTH 20 // width for common purpose use

// player window coordinates
#define PLAYER_WND_WIDTH MW_WIN_WIDTH
#define PLAYER_WND_HEIGHT MW_WIN_HEIGHT

#define PLAYER_TITLEBAR_HEIGHT 32 // title bar height
#define PLAYER_MENUBAR_HEIGHT 64 // bottom menu bar height
#define PLAYER_TITLE_LEFT COMMON_WIDTH // window caption string left position

// album info window position
#define PLAYER_ALBUM_WND_WIDTH (100)
#define PLAYER_ALBUM_WND_HEIGHT (150)
#define PLAYER_ALBUM_WND_LEFT COMMON_WIDTH
#define PLAYER_ALBUM_WND_TOP (PLAYER_TITLEBAR_HEIGHT+COMMON_HEIGHT)
// album cover position
#define PLAYER_ALBUM_COVER_WIDTH 200
#define PLAYER_ALBUM_COVER_HEIGHT 200
#define PLAYER_ALBUM_COVER_LEFT COMMON_WIDTH
#define PLAYER_ALBUM_COVER_TOP COMMON_HEIGHT
// album info text height
#define PLAYER_ALBUMINFO_HEIGHT COMMON_HEIGHT

//-------------DurationWindow------------
#define DURATION_WND_LEFT  10
#define DURATION_WND_TOP  160
#define DURATION_WND_WIDTH  205
#define DURATION_WND_HEIGHT  32

//-------------ImageVeiwerWindow------------
#define PV_WIN_LEFT 750
#define PV_WIN_TOP 290
#define PV_WIN_WIDTH 225
#define PV_WIN_HEIGHT 200

#else //if using digital clock
#define PLAY_TYPE_MUSIC

//------------TimeWindow UI---------------
#define TW_TimeFontSize 260
#define TW_DateFontSize 80

#define TW_WIN_TOP 50
#define TW_WIN_LEFT 50
#define TW_WIN_WIDTH 921
#define TW_WIN_HEIGHT 463

#define TW_TIME_RECT_LEFT 0 
#define TW_TIME_RECT_TOP 0
#define TW_TIME_RECT_RIGHT 921
#define TW_TIME_RECT_BOTTOM 463

#define TW_DATE_RECT_LEFT 20 
#define TW_DATE_RECT_TOP 500
#define TW_DATE_RECT_RIGHT 921
#define TW_DATE_RECT_BOTTOM 668

//-------------XAnalogClock---------------
#define XAC_DateFontSize 20

#define DATE_BOX_WIDTH		39
#define DATE_BOX_HEIGHT		31
#define DATE_BOX_XPOS		530
#define DATE_BOX_YPOS		330
#define SECOND_HAND_LENGTH	260.0
#define MINUTE_HAND_LENGTH	230.0
#define HOUR_HAND_LENGTH	200.0
#define SECOND_HAND_COLOR	RGB(96,0,192)
#define MINUTE_HAND_COLOR	RGB(192,96,192)
#define HOUR_HAND_COLOR		RGB(255,251,239)

#define XAC_WIN_TOP 50
#define XAC_WIN_LEFT 50
#define XAC_WIN_WIDTH 690
#define XAC_WIN_HEIGHT 668

//-------------AlertWindow-------------
#define AW_AlertFontSize 28
#define AW_MiscFontSize 16

#define AW_WIN_LEFT 50
#define AW_WIN_TOP 518
#define AW_WIN_WIDTH 224
#define AW_WIN_HEIGHT 200

#define AW_ALERT_RECT_LEFT 10 
#define AW_ALERT_RECT_TOP 10
#define AW_ALERT_RECT_RIGHT 220
#define AW_ALERT_RECT_BOTTOM 100

#define AW_MISC_RECT_LEFT 10 
#define AW_MISC_RECT_TOP 110
#define AW_MISC_RECT_RIGHT 220
#define AW_MISC_RECT_BOTTOM 190

//-------------MediaWindow--------------
#define MW_WIN_LEFT 280
#define MW_WIN_TOP 518
#define MW_WIN_WIDTH 485
#define MW_WIN_HEIGHT 200

#define COMMON_HEIGHT 20 // height of the text string, space and other common purpose
#define COMMON_WIDTH 20 // width for common purpose use

// player window coordinates
#define PLAYER_WND_WIDTH MW_WIN_WIDTH
#define PLAYER_WND_HEIGHT MW_WIN_HEIGHT

#define PLAYER_TITLEBAR_HEIGHT 32 // title bar height
#define PLAYER_MENUBAR_HEIGHT 64 // bottom menu bar height
#define PLAYER_TITLE_LEFT COMMON_WIDTH // window caption string left position

// album info window position
#define PLAYER_ALBUM_WND_WIDTH (100)
#define PLAYER_ALBUM_WND_HEIGHT (150)
#define PLAYER_ALBUM_WND_LEFT COMMON_WIDTH
#define PLAYER_ALBUM_WND_TOP (PLAYER_TITLEBAR_HEIGHT+COMMON_HEIGHT)
// album cover position
#define PLAYER_ALBUM_COVER_WIDTH 200
#define PLAYER_ALBUM_COVER_HEIGHT 200
#define PLAYER_ALBUM_COVER_LEFT COMMON_WIDTH
#define PLAYER_ALBUM_COVER_TOP COMMON_HEIGHT
// album info text height
#define PLAYER_ALBUMINFO_HEIGHT COMMON_HEIGHT

//-------------DurationWindow------------
#define DURATION_WND_LEFT  250
#define DURATION_WND_TOP  150
#define DURATION_WND_WIDTH  220
#define DURATION_WND_HEIGHT  32

//-------------ImageVeiwerWindow------------
#define PV_WIN_LEFT 750
#define PV_WIN_TOP 290
#define PV_WIN_WIDTH 225
#define PV_WIN_HEIGHT 200

#endif //endif analogclock

#endif