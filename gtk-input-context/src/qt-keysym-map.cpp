
#include <X11/keysym.h>
#include <qstring.h>

#include "qt-keysym-map.h"
#include "debug.h"

typedef struct {
	int QtKey;
	uint XKeySym;
} KeySymMap;


static const KeySymMap QtKeyXSymMaps[] =
{
	{ '/',			XK_KP_Divide },
	{ '*',			XK_KP_Multiply },
	{ '-',			XK_KP_Subtract },
	{ '+',			XK_KP_Add },
	{ Qt::Key_Escape,	XK_Escape },
	{ Qt::Key_Tab,		XK_Tab },
	{ Qt::Key_Backtab,	XK_ISO_Left_Tab },
	{ Qt::Key_Backspace,	XK_BackSpace },
	{ Qt::Key_Return,	XK_Return },
	{ Qt::Key_Enter,	XK_KP_Enter },
	{ Qt::Key_Insert,	XK_Insert },
	{ Qt::Key_Delete,	XK_Delete },
	{ Qt::Key_Pause,	XK_Pause },
	{ Qt::Key_Print,	XK_Print },
	{ Qt::Key_SysReq,	XK_Sys_Req },
	{ Qt::Key_Home,		XK_Home },
	{ Qt::Key_End,		XK_End },
	{ Qt::Key_Left,		XK_Left },
	{ Qt::Key_Up,		XK_Up },
	{ Qt::Key_Right,	XK_Right },
	{ Qt::Key_Down,		XK_Down },
	{ Qt::Key_CapsLock,	XK_Caps_Lock },
	{ Qt::Key_NumLock,	XK_Num_Lock },
	{ Qt::Key_ScrollLock,	XK_Scroll_Lock },
	{ Qt::Key_F1,		XK_F1 },
	{ Qt::Key_F2,		XK_F2 },
	{ Qt::Key_F3,		XK_F3 },
	{ Qt::Key_F4,		XK_F4 },
	{ Qt::Key_F5,		XK_F5 },
	{ Qt::Key_F6,		XK_F6 },
	{ Qt::Key_F7,		XK_F7 },
	{ Qt::Key_F8,		XK_F8 },
	{ Qt::Key_F9,		XK_F9 },
	{ Qt::Key_F10,		XK_F10 },
	{ Qt::Key_F11,		XK_F11 },
	{ Qt::Key_F12,		XK_F12 },
	{ Qt::Key_F13,		XK_F13 },
	{ Qt::Key_F14,		XK_F14 },
	{ Qt::Key_F15,		XK_F15 },
	{ Qt::Key_F16,		XK_F16 },
	{ Qt::Key_F17,		XK_F17 },
	{ Qt::Key_F18,		XK_F18 },
	{ Qt::Key_F19,		XK_F19 },
	{ Qt::Key_F20,		XK_F20 },
	{ Qt::Key_F21,		XK_F21 },
	{ Qt::Key_F22,		XK_F22 },
	{ Qt::Key_F23,		XK_F23 },
	{ Qt::Key_F24,		XK_F24 },
	{ Qt::Key_F25,		XK_F25 },
	{ Qt::Key_F26,		XK_F26 },
	{ Qt::Key_F27,		XK_F27 },
	{ Qt::Key_F28,		XK_F28 },
	{ Qt::Key_F29,		XK_F29 },
	{ Qt::Key_F30,		XK_F30 },
	{ Qt::Key_F31,		XK_F31 },
	{ Qt::Key_F32,		XK_F32 },
	{ Qt::Key_F33,		XK_F33 },
	{ Qt::Key_F34,		XK_F34 },
	{ Qt::Key_F35,		XK_F35 },
	{ Qt::Key_Super_L,	XK_Super_L },
	{ Qt::Key_Super_R,	XK_Super_R },
	{ Qt::Key_Menu,		XK_Menu },
	{ Qt::Key_Hyper_L,	XK_Hyper_L },
	{ Qt::Key_Hyper_R,	XK_Hyper_R },
	{ Qt::Key_Help,		XK_Help },
	{ Qt::Key_Return,	XK_KP_Enter },

	{ Qt::Key_Multi_key,	XK_Multi_key },
	{ Qt::Key_Codeinput,	XK_Codeinput },
	{ Qt::Key_SingleCandidate,	XK_SingleCandidate },
	{ Qt::Key_MultipleCandidate,	XK_MultipleCandidate },
	{ Qt::Key_PreviousCandidate,	XK_PreviousCandidate },
	{ Qt::Key_Mode_switch,	XK_Mode_switch },
	{ Qt::Key_Kanji,	XK_Kanji },
	{ Qt::Key_Muhenkan,	XK_Muhenkan },
	{ Qt::Key_Henkan,	XK_Henkan },
	{ Qt::Key_Romaji,	XK_Romaji },
	{ Qt::Key_Hiragana,	XK_Hiragana },
	{ Qt::Key_Katakana,	XK_Katakana },
	{ Qt::Key_Hiragana_Katakana,	XK_Hiragana_Katakana },
	{ Qt::Key_Zenkaku,	XK_Zenkaku },
	{ Qt::Key_Hankaku,	XK_Hankaku },
	{ Qt::Key_Zenkaku_Hankaku,	XK_Zenkaku_Hankaku },
	{ Qt::Key_Touroku,	XK_Touroku },
	{ Qt::Key_Massyo,	XK_Massyo },
	{ Qt::Key_Kana_Lock,	XK_Kana_Lock },
	{ Qt::Key_Kana_Shift,	XK_Kana_Shift },
	{ Qt::Key_Eisu_Shift,	XK_Eisu_Shift },
	{ Qt::Key_Eisu_toggle,	XK_Eisu_toggle },
	{ Qt::Key_Hangul,	XK_Hangul },
	{ Qt::Key_Hangul_Start,	XK_Hangul_Start },
	{ Qt::Key_Hangul_End,	XK_Hangul_End },
	{ Qt::Key_Hangul_Hanja,	XK_Hangul_Hanja },
	{ Qt::Key_Hangul_Jamo,	XK_Hangul_Jamo },
	{ Qt::Key_Hangul_Romaja,	XK_Hangul_Romaja },
	{ Qt::Key_Hangul_Jeonja,	XK_Hangul_Jeonja },
	{ Qt::Key_Hangul_Banja,	XK_Hangul_Banja },
	{ Qt::Key_Hangul_PreHanja,	XK_Hangul_PreHanja },
	{ Qt::Key_Hangul_Special,	XK_Hangul_Special },

	{ 0, 0 }
};


int
QtKeyToXKeySym(int qtKey) {

	// TODO: Need to complete this function for all scenario.
	int i;
	int count = sizeof(QtKeyXSymMaps)/sizeof(KeySymMap);

	if (qtKey < 0x1000) {
		return QChar(qtKey).toLower().unicode();
	}

	for(i = 0; i < count; i++ ) {
		if( QtKeyXSymMaps[i].QtKey == qtKey ) {
			return QtKeyXSymMaps[i].XKeySym;
		}
	}

        return 0;
}


int
XKeySymToQTKey(uint keySym)
{
	int i;
	int count = sizeof(QtKeyXSymMaps)/sizeof(KeySymMap);

	STEP();
	if ((keySym < 0x1000)) {
		if (keySym >= 'a' && keySym <= 'z')
			return QChar(keySym).toUpper().toAscii();
		return keySym;
	}

#ifdef Q_WS_WIN
        if(keySym < 0x3000)
                return keySym;
#else
	if (keySym < 0x3000 )
		return keySym | Qt::UNICODE_ACCEL;

	for(i = 0; i < count; i++)
		if(QtKeyXSymMaps[i].XKeySym == keySym)
			return QtKeyXSymMaps[i].QtKey;
#endif
        return Qt::Key_unknown;
}

