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

/* -------------------------------------------------------------------------
 * Version / identity
 * ---------------------------------------------------------------------- */
#define STUDIO_NAME     L"Nova Studio"
#define STUDIO_VERSION  L"1.0"
#define STUDIO_CLASS    L"NovaStudioMain"

/* -------------------------------------------------------------------------
 * Child control IDs
 * ---------------------------------------------------------------------- */
#define ID_TOOLBAR       100
#define ID_STATUSBAR     101
#define ID_EXPLORER      102
#define ID_TABBAR        103
#define ID_EDITOR        104
#define ID_OUTPUT        105
#define ID_SPLITTER_V    106  /* vertical splitter explorer|editor */
#define ID_SPLITTER_H    107  /* horizontal splitter editor|output */

/* -------------------------------------------------------------------------
 * Menu command IDs
 * ---------------------------------------------------------------------- */
#define IDM_FILE_NEW         1001
#define IDM_FILE_NEW_PROJECT 1002
#define IDM_FILE_OPEN        1003
#define IDM_FILE_SAVE        1004
#define IDM_FILE_SAVEAS      1005
#define IDM_FILE_CLOSE       1006
#define IDM_FILE_EXIT        1007

#define IDM_EDIT_UNDO        2001
#define IDM_EDIT_REDO        2002
#define IDM_EDIT_CUT         2003
#define IDM_EDIT_COPY        2004
#define IDM_EDIT_PASTE       2005
#define IDM_EDIT_SELECTALL   2006
#define IDM_EDIT_FIND        2007
#define IDM_EDIT_REPLACE     2008
#define IDM_EDIT_GOTO        2009
#define IDM_EDIT_COMMENT     2010
#define IDM_EDIT_FORMAT      2011

#define IDM_VIEW_EXPLORER    3001
#define IDM_VIEW_OUTPUT      3002
#define IDM_VIEW_DARKMODE    3003
#define IDM_VIEW_LIGHTMODE   3004
#define IDM_VIEW_ZOOMIN      3005
#define IDM_VIEW_ZOOMOUT     3006
#define IDM_VIEW_ZOOMRESET   3007

#define IDM_BUILD_BUILD      4001
#define IDM_BUILD_REBUILD    4002
#define IDM_BUILD_CLEAN      4003
#define IDM_BUILD_RUN        4004

#define IDM_HELP_ABOUT       9001

/* -------------------------------------------------------------------------
 * Theme
 * ---------------------------------------------------------------------- */
enum ThemeKind { THEME_DARK, THEME_LIGHT };

struct Theme {
    ThemeKind kind;
    COLORREF  bg_editor;    /* editor background */
    COLORREF  bg_panel;     /* panels */
    COLORREF  bg_titlebar;  /* window / menu bg */
    COLORREF  fg_default;   /* default text */
    COLORREF  fg_keyword;
    COLORREF  fg_type;
    COLORREF  fg_string;
    COLORREF  fg_comment;
    COLORREF  fg_number;
    COLORREF  fg_operator;
    COLORREF  fg_ident;
    COLORREF  fg_error;
    COLORREF  fg_linenum;
    COLORREF  sel_bg;
    COLORREF  tab_active;
    COLORREF  tab_inactive;
    COLORREF  border;
    COLORREF  status_bg;
    COLORREF  status_fg;
};

extern Theme g_theme;

void ThemeSetDark();
void ThemeSetLight();

/* -------------------------------------------------------------------------
 * Token types for syntax highlighting
 * ---------------------------------------------------------------------- */
enum NlSyntaxToken {
    TT_DEFAULT = 0,
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
    int            start;
    int            length;
    NlSyntaxToken  type;
    TokenSpan(int s, int l, NlSyntaxToken t) : start(s), length(l), type(t) {}
};

/* -------------------------------------------------------------------------
 * Editor tab
 * ---------------------------------------------------------------------- */
struct EditorTab {
    std::wstring filepath;   /* empty = untitled */
    std::wstring title;
    bool         modified;
    int          scroll_pos;
    int          caret_pos;
};

/* -------------------------------------------------------------------------
 * Global state
 * ---------------------------------------------------------------------- */
extern HWND g_hMain;
extern HWND g_hEditor;     /* RichEdit */
extern HWND g_hExplorer;   /* TreeView */
extern HWND g_hOutput;     /* Edit */
extern HWND g_hTabBar;     /* TabControl */
extern HWND g_hStatusBar;
extern HWND g_hToolBar;
extern HFONT g_hCodeFont;
extern int  g_explorerWidth;
extern int  g_outputHeight;
extern bool g_showExplorer;
extern bool g_showOutput;
extern bool g_highlightPending;
extern int  g_zoomPercent;

extern std::vector<EditorTab> g_tabs;
extern int g_activeTab;

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
void EditorUpdateTitle();
void EditorOutput(const wchar_t *text);
void EditorOutputClear();
void EditorSetZoom(int percent);

/* -------------------------------------------------------------------------
 * Explorer API
 * ---------------------------------------------------------------------- */
void ExplorerCreate(HWND hParent, RECT rc);
void ExplorerResize(RECT rc);
void ExplorerApplyTheme();
void ExplorerSetRoot(const wchar_t *path);
void ExplorerOpenSelected();

/* -------------------------------------------------------------------------
 * Highlighter API
 * ---------------------------------------------------------------------- */
std::vector<TokenSpan> HlTokenize(const wchar_t *text, int length);
COLORREF HlTokenColor(NlSyntaxToken tt);

/* -------------------------------------------------------------------------
 * Layout helper
 * ---------------------------------------------------------------------- */
void LayoutCompute(HWND hMain, RECT *rExplorer, RECT *rEditor,
                   RECT *rOutput, RECT *rTabBar);
void LayoutApply(HWND hMain);
