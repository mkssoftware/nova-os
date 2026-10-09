#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <richedit.h>
#include <dwmapi.h>
#include <vector>
#include <string>
#include <functional>
#include <cstdio>
#include <cstring>
#include <cwctype>
#include <algorithm>

/* -------------------------------------------------------------------------
 * Identity
 * ---------------------------------------------------------------------- */
#define STUDIO_NAME     L"NovaStudio"
#define STUDIO_VERSION  L"2.0"
#define STUDIO_CLASS    L"NovaStudioWnd"

/* -------------------------------------------------------------------------
 * Child IDs
 * ---------------------------------------------------------------------- */
#define ID_TOOLBAR      100
#define ID_STATUSBAR    101
#define ID_EXPLORER     102
#define ID_TABBAR       103
#define ID_EDITOR       104
#define ID_OUTPUT       105
#define ID_DOCOUTLINE   106
#define ID_BOTTOMPANEL  107

/* -------------------------------------------------------------------------
 * Menu IDs
 * ---------------------------------------------------------------------- */
// Datei
#define IDM_FILE_NEW         1001
#define IDM_FILE_NEW_PROJECT 1002
#define IDM_FILE_OPEN        1003
#define IDM_FILE_OPEN_PROJECT 1004
#define IDM_FILE_SAVE        1005
#define IDM_FILE_SAVEAS      1006
#define IDM_FILE_CLOSE_TAB   1007
#define IDM_FILE_EXIT        1008

// Bearbeiten
#define IDM_EDIT_UNDO        2001
#define IDM_EDIT_REDO        2002
#define IDM_EDIT_CUT         2003
#define IDM_EDIT_COPY        2004
#define IDM_EDIT_PASTE       2005
#define IDM_EDIT_SELECTALL   2006
#define IDM_EDIT_FIND        2007
#define IDM_EDIT_FINDNEXT    2008
#define IDM_EDIT_REPLACE     2009
#define IDM_EDIT_GOTO        2010
#define IDM_EDIT_COMMENT     2011

// Ansicht
#define IDM_VIEW_EXPLORER    3001
#define IDM_VIEW_OUTPUT      3002
#define IDM_VIEW_DARKMODE    3003
#define IDM_VIEW_LIGHTMODE   3004
#define IDM_VIEW_ZOOMIN      3005
#define IDM_VIEW_ZOOMOUT     3006
#define IDM_VIEW_ZOOMRESET   3007

// Kompilieren
#define IDM_BUILD_BUILD      4001
#define IDM_BUILD_REBUILD    4002
#define IDM_BUILD_CLEAN      4003
#define IDM_BUILD_RUN        4004

// Hilfe
#define IDM_HELP_ABOUT       9001

// Debug
#define IDM_DEBUG_START         5001
#define IDM_DEBUG_START_NO_DBG  5002
#define IDM_DEBUG_STOP          5003

// Git
#define IDM_GIT_COMMIT          6001
#define IDM_GIT_PUSH            6002
#define IDM_GIT_PULL            6003

// Datei: Alle speichern
#define IDM_FILE_SAVE_ALL       1009

/* -------------------------------------------------------------------------
 * Timers
 * ---------------------------------------------------------------------- */
#define TIMER_HIGHLIGHT  1
#define TIMER_STATUS     2

/* -------------------------------------------------------------------------
 * Theme
 * ---------------------------------------------------------------------- */
enum ThemeKind { THEME_DARK, THEME_LIGHT };

struct Theme {
    ThemeKind kind;
    COLORREF  bg_editor;
    COLORREF  bg_panel;
    COLORREF  bg_statusbar;
    COLORREF  fg_default;
    COLORREF  fg_keyword;
    COLORREF  fg_type;
    COLORREF  fg_string;
    COLORREF  fg_comment;
    COLORREF  fg_number;
    COLORREF  fg_operator;
    COLORREF  fg_ident;
    COLORREF  fg_error;
    COLORREF  fg_linenum;
    COLORREF  status_bg;
    COLORREF  status_fg;
    COLORREF  border;
};

extern Theme g_theme;
void ThemeSetDark();
void ThemeSetLight();

/* -------------------------------------------------------------------------
 * Syntax token types
 * ---------------------------------------------------------------------- */
enum NlSyntaxToken {
    TT_DEFAULT  = 0,
    TT_KEYWORD,
    TT_TYPE_KW,
    TT_OPERATOR,
    TT_NUMBER,
    TT_STRING,
    TT_COMMENT,
    TT_IDENT,
    TT_ERROR
};

struct TokenSpan {
    int           start;
    int           length;
    NlSyntaxToken type;
    TokenSpan(int s, int l, NlSyntaxToken t) : start(s), length(l), type(t) {}
};

/* -------------------------------------------------------------------------
 * Editor tab
 * ---------------------------------------------------------------------- */
struct EditorTab {
    std::wstring filepath;
    std::wstring title;
    bool         modified;
};

/* -------------------------------------------------------------------------
 * Project state
 * ---------------------------------------------------------------------- */
struct Project {
    std::wstring name;
    std::wstring rootPath;
    bool         open;
};

/* -------------------------------------------------------------------------
 * Global handles
 * ---------------------------------------------------------------------- */
extern HWND g_hMain;
extern HWND g_hEditor;
extern HWND g_hExplorer;
extern HWND g_hOutput;
extern HWND g_hTabBar;
extern HWND g_hStatusBar;
extern HWND g_hDocOutline;
extern HFONT g_hCodeFont;

extern int  g_explorerWidth;
extern int  g_outputHeight;
extern int  g_outlineWidth;
extern bool g_showExplorer;
extern bool g_showOutput;
extern bool g_showOutline;
extern int  g_zoomPercent;

extern std::vector<EditorTab> g_tabs;
extern int                    g_activeTab;
extern Project                g_project;

extern FINDREPLACEW g_fr;
extern HWND         g_hFindDlg;
extern UINT         g_msgFindReplace;
extern wchar_t      g_szFindBuf[512];
extern wchar_t      g_szReplaceBuf[512];

/* -------------------------------------------------------------------------
 * Editor API
 * ---------------------------------------------------------------------- */
void EditorCreate(HWND hParent, RECT rc);
void EditorResize(RECT rc);
void EditorApplyTheme();
void EditorHighlight();
void EditorNewFile();
bool EditorOpenFile(const wchar_t *path);
bool EditorSaveFile();
bool EditorSaveFileAs();
void EditorCloseTab(int idx);
void EditorUpdateTitle();
void EditorOutput(const wchar_t *text, bool newline = true);
void EditorOutputClear();
void EditorSetZoom(int percent);
void EditorGoToLine(int line);
bool EditorFindNext(const wchar_t *text, DWORD flags);
bool EditorReplaceNext(const wchar_t *findText, const wchar_t *replaceText, DWORD flags);
int  EditorReplaceAll(const wchar_t *findText, const wchar_t *replaceText, DWORD flags);
void EditorCommentToggle();
void EditorUpdateStatusBar();
void EditorGetCaretPos(int *pLine, int *pCol);
void EditorOnSelChange();

/* -------------------------------------------------------------------------
 * Explorer API
 * ---------------------------------------------------------------------- */
void ExplorerCreate(HWND hParent, RECT rc);
void ExplorerResize(RECT rc);
void ExplorerApplyTheme();
void ExplorerSetRoot(const wchar_t *path);
void ExplorerOpenSelected();
void ExplorerRefresh();

/* -------------------------------------------------------------------------
 * Highlighter API
 * ---------------------------------------------------------------------- */
std::vector<TokenSpan> HlTokenize(const wchar_t *text, int length);
COLORREF HlTokenColor(NlSyntaxToken tt);

/* -------------------------------------------------------------------------
 * Dialog API
 * ---------------------------------------------------------------------- */
void DoFind();
void DoReplace();
void DoGoToLine();
void DoAbout();
bool DoNewProject();   // returns true if project was created

/* -------------------------------------------------------------------------
 * Layout
 * ---------------------------------------------------------------------- */
void LayoutCompute(HWND hw, RECT *rExp, RECT *rEd, RECT *rOut, RECT *rTab);
void LayoutApply(HWND hw);

/* -------------------------------------------------------------------------
 * Ribbon (NPSPEC-STUDIO-RIBBON-0001)
 * ---------------------------------------------------------------------- */
#define RIBBON_HEIGHT  102   /* total ribbon height in px */

extern HWND g_hRibbon;

void RibbonCreate(HWND hParent, int width);
void RibbonResize(int x, int y, int width);
void RibbonApplyTheme();
void RibbonSetTab(int idx);

/* -------------------------------------------------------------------------
 * Build
 * ---------------------------------------------------------------------- */
void DoBuild(bool rebuild);
void DoRun();
void DoClean();
