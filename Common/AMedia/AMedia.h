/* 
Requirements:
A: additional include directories:
		C:\WINCE500\PUBLIC\DIRECTX\SDK\INC
B: additional library directories:
		DEBUG: C:\WINCE500\PUBLIC\DIRECTX\SDK\LIB\MIPSII\DEBUG
		RELEASE: C:\WINCE500\PUBLIC\DIRECTX\SDK\LIB\MIPSII\RETAIL
*/
#pragma once
#pragma warning(disable:4430)

#include <dshow.h>
#include <qnetwork.h>

// media event notify message to be post to the handling window
#define WM_MEDIALIB_NOTIFY  RegisterWindowMessage(L"WM_MEDIALIB_NOTIFY")

// play states
enum PLAYSTATE { 
	psStop,
	psPause, 
	psPlaying, 
};

// MP3 tag structure
typedef struct
{
	char Identify[3]; // tag identifier
	char Title[30];   // 30 bytes for the title
	char Artist[30];  // 30 bytes for the artist
	char Album[30];  // 30 bytes for the album
	char Year[4];	  // 4 bytes for the year
	char Comment[30];  // 30 bytes for the comment
	char Genre[1]; // 1 byte for genre
/*
	0 - Blues
	1 - Classic Rock
	2 - Country
	3 - Dance 
	4 - Disco 
	5 - Funk
	6 - Grunge 
	7 - Hip-Hop 
	8 - Jazz 
	9 - Metal 
	10 - New Age 
	11 - Oldies 
	12 - Other
	13 - Pop 
	14 - R&B 
	15 - Rap 
	16 - Reggae 
	17 - Rock 
	18 - Techno 
	19 - Industrial 
	20 - Alternative 
	21 - Ska 
	22 - Death Metal 
	23 - Pranks 
	24 - Soundtrack 
	25 - Euro-Techno 
	26 - Ambient 
	27 - Trip-Hop 
	28 - Vocal 
	29 - Jazz+Funk 
	30 - Fusion 
	31 - Trance 
	32 - Classical 
	33 - Instrumental 
	34 - Acid 
	35 - House 
	36 - Game 
	37 - Sound Clip 
	38 - Gospel 
	39 - Noise 
	40 - Alternative Rock 
	41 - Bass 
	42 - Soul 
	43 - Punk 
	44 - Space 
	45 - Meditative 
	46 - Instrumental Pop
	47 - Instrumental Rock
	48 - Ethnic
	49 - Gothic 
	50 - Darkwave 
	51 - Techno-Industrial
	52 - Electronic
	53 - Pop-Folk
	54 - Eurodance
	55 - Dream 
	56 - Southern Rock 
	57 - Comedy 
	58 - Cult 
	59 - Gangsta 
	60 - Top 40 
	61 - Christian Rap 
	62 - Pop/Funk 
	63 - Jungle 
	64 - Native US 
	65 - Cabaret 
	66 - New Wave 
	67 - Psychadelic 
	68 - Rave 
	69 - Showtunes 
	70 - Trailer 
	71 - Lo-Fi 
	72 - Tribal 
	73 - Acid Punk 
	74 - Acid Jazz 
	75 - Polka 
	76 - Retro 
	77 - Musical 
	78 - Rock & Roll 
	79 - Hard Rock 
	80 - Folk 
	81 - Folk-Rock 
	82 - National Folk 
	83 - Swing 
	84 - Fast Fusion 
	85 - Bebop
	86 - Latin 
	87 - Revival 
	88 - Celtic 
	89 - Bluegrass 
	90 - Avantgarde 
	91 - Gothic Rock 
	92 - Progressive Rock 
	93 - Psychedelic Rock 
	94 - Symphonic Rock 
	95 - Slow Rock 
	96 - Big Band 
	97 - Chorus 
	98 - Easy Listening 
	99 - Acoustic 
	100 - Humour 
	101 - Speech 
	102 - Chanson 
	103 - Opera 
	104 - Chamber Music 
	105 - Sonata 
	106 - Symphony 
	107 - Booty Bass 
	108 - Primus 
	109 - Porn Groove 
	110 - Satire 
	111 - Slow Jam 
	112 - Club 
	113 - Tango 
	114 - Samba 
	115 - Folklore 
	116 - Ballad
	117 - Power Ballad
	118 - Rhytmic Soul
	119 - Freestyle 
	120 - Duet 
	121 - Punk Rock 
	122 - Drum Solo 
	123 - Acapella 
	124 - Euro-House
	125 - Dance Hall
	126 - Goa
	127 - Drum & Bass
*/
} MP3ID3V1TAG;

class CAMedia
{
public:
	CAMedia(void);
	~CAMedia(void);

public:
	HRESULT PlayFile(HWND hWnd, wchar_t *lpwszFile);
	HRESULT CompleteEventNotify(void); 
	HRESULT GetCompleteEvent(void);
	HRESULT SetVideoWindowPos(HWND hWnd, RECT rc, bool bFullScreen);
	
	HRESULT Play(void);
	HRESULT Pause(void);
	HRESULT Stop(void);
	PLAYSTATE GetPlayState(void);

	HRESULT GetDuration(LONGLONG *pllDuration);
	HRESULT GetCurrentPosition(LONGLONG *pllCurrentPos);
	HRESULT JumpToPosition(LONGLONG *pllPosition);
	HRESULT GetPlaybackRate(double *pdRate);
	HRESULT SetPlaybackRate(double dRate);

	void GetVolume(long *plVolume);
	void SetVolume(long lVolume);
	void GetBalance(long *plBalance);
	void SetBalance(long lBalance);

	void GetMP3ID3V1Tag(wchar_t *lpwszFile, MP3ID3V1TAG *pTag);

private:
	HRESULT RenderMediaFile(wchar_t *lpwszFile);
	HRESULT RegisterNotifyWindow(HWND hWnd);
	void CleanInterface(void);

private:
	PLAYSTATE m_PlayState;
	MP3ID3V1TAG *m_pMP3Tag;

	IGraphBuilder *m_pGraphBuilder;
	IMediaControl *m_pMediaControl;
	IMediaEventEx *m_pMediaEventEx;
	IVideoWindow *m_pVideoWindow;
	IBasicAudio *m_pBasicAudio;
	IBasicVideo *m_pBasicVideo;
	IMediaSeeking *m_pMediaSeeking;
};

