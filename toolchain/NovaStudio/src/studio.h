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
#include <windowsx.h>
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
#define ID_BOTTOMTABS   108
#define ID_PROBLEMS     109
#define ID_SYSCALLS     110
#define ID_CAPABILITIES 111
#define ID_SEARCH       112

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
#define IDM_EDIT_FIND_PROJECT 2012

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

// Extras/Tools
#define IDM_TOOLS_SYSCALLS   9002
#define IDM_TOOLS_CAPS       9003

// Explorer-Kontextmenü
#define IDM_EXPL_NEWFILE     9101
#define IDM_EXPL_DELETE      9102
#define IDM_EXPL_REFRESH     9103
#define IDM_EXPL_RENAME      9104

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
    /* Live-Inhaltsspiegel (NPSPEC-STUDIO-EDITOR-0001-Nachtrag): g_hEditor
       ist EIN gemeinsames RichEdit-Control fuer alle Tabs. Beim Verlassen
       einer Registerkarte wird ihr aktueller Text HIER zwischengespeichert
       (siehe WM_NOTIFY/TCN_SELCHANGE in main.cpp), NICHT von der
       Festplatte neu geladen - vorher gingen dadurch ungespeicherte
       Aenderungen beim Tab-Wechsel kommentarlos verloren. */
    std::wstring content;
};

/* -------------------------------------------------------------------------
 * Project state
 *
 * NovaStudio liest/schreibt project.xml (<NovaProject>, siehe
 * dialogs.cpp::DoNewProject()). WICHTIGE EINSCHRAENKUNG (siehe
 * IMPLEMENTATION_STATUS.md): novac.exe kompiliert genau EINE Quelldatei
 * pro Aufruf - es gibt noch keine Mehrdateien-/Modul-uebergreifende
 * Verlinkung im Compiler. "sources" haelt daher alle im Projekt
 * bekannten .nova-Dateien (fuer Explorer/Zukunft), "entryFile" ist die
 * EINE Datei, die "Erstellen" tatsaechlich uebersetzt. ---------------- */
struct Project {
    std::wstring              name;
    std::wstring              rootPath;
    bool                      open;
    std::vector<std::wstring> sources;    /* absolute Pfade, aus <Sources> gelesen */
    std::wstring              entryFile;  /* absoluter Pfad, erstes <File> bzw. <EntryPoint> */
};

/* -------------------------------------------------------------------------
 * Global handles
 * ---------------------------------------------------------------------- */
extern HWND g_hMain;
extern HWND g_hEditor;
extern HWND g_hExplorer;
extern HWND g_hExplorerSearch;
extern HWND g_hOutput;
extern HWND g_hProblems;          /* ListView: Build-Diagnostik (Datei/Zeile/Meldung) */
extern HWND g_hSyscalls;          /* ListView: Syscall Browser (Name/Wert/Kommentar) */
extern HWND g_hCapabilities;      /* ListView: Capability Manager (Name/Hex-Wert/Bit) */
extern HWND g_hSearch;            /* ListView: projektweite Suche (Datei/Zeile/Text) */
extern HWND g_hTabBar;
extern HWND g_hStatusBar;
extern HWND g_hDocOutline;        /* TreeView inside outline container */
extern HWND g_hBottomContainer;   /* Container for tabs + output */
extern HWND g_hBottomTabs;        /* TabCtrl in bottom container */
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
void EditorSaveAllFiles();
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

/* Sichert den LIVE-Text aus g_hEditor in g_tabs[g_activeTab].content
   (No-Op ohne gueltigen g_activeTab). MUSS vor JEDER Zuweisung an
   g_activeTab aufgerufen werden, die auf eine ANDERE Registerkarte
   wechselt - sonst gehen ungespeicherte Aenderungen der bisherigen
   Registerkarte verloren (siehe EditorTab::content-Kommentar). */
void EditorSyncActiveTabContent();

/* -------------------------------------------------------------------------
 * Explorer API
 * ---------------------------------------------------------------------- */
void ExplorerCreate(HWND hParent, RECT rc);
void ExplorerResize(RECT rc);
void ExplorerApplyTheme();
void ExplorerSetRoot(const wchar_t *path);
void ExplorerOpenSelected();
void ExplorerRefresh();

/* Filtert den Explorer-Baum live nach query (Teilstring im Dateinamen,
   case-insensitiv, rekursiv ab dem Projektordner). Leeres query stellt
   die normale Ordneransicht wieder her. War vorher nur eine
   dekorative, funktionslose Eingabezeile (siehe IMPLEMENTATION_
   STATUS.md). */
void ExplorerFilter(const wchar_t *query);

/* Fuer das Kontextmenue: Pfad + Verzeichnis-Flag der aktuell im
   Explorer ausgewaehlten Zeile. Liefert false, wenn nichts ausgewaehlt
   ist (outPath/outIsDir dann unveraendert). */
bool ExplorerGetSelected(std::wstring *outPath, bool *outIsDir);

/* Kontextmenue-Aktionen (NPSPEC-STUDIO-EXPLORER-0001-Nachtrag). Nur
   Dateien, KEINE Ordner loeschbar (kein rekursives Loeschen - bewusst
   zurueckgestellt, siehe IMPLEMENTATION_STATUS.md). Loeschen verschiebt
   in den Papierkorb (SHFileOperationW+FOF_ALLOWUNDO), kein endgueltiges
   Loeschen. */
void ExplorerContextNewFile(HWND hOwner);
void ExplorerContextDelete(HWND hOwner);
void ExplorerContextRename(HWND hOwner);

/* -------------------------------------------------------------------------
 * Highlighter API
 * ---------------------------------------------------------------------- */
std::vector<TokenSpan> HlTokenize(const wchar_t *text, int length);
COLORREF HlTokenColor(NlSyntaxToken tt);

/* -------------------------------------------------------------------------
 * In-memory DLGTEMPLATE builder (avoids the need for a .rc resource file).
 * Urspruenglich nur in dialogs.cpp, hierher verschoben, damit auch
 * explorer.cpp eigene einfache Dialoge (z.B. "Neue Datei") bauen kann,
 * ohne eine zweite, parallele Kopie zu pflegen.
 * ---------------------------------------------------------------------- */
struct DlgBuilder {
    std::vector<BYTE> buf;
    int itemCount = 0;

    void addW(WORD w) {
        buf.push_back((BYTE)(w & 0xFF));
        buf.push_back((BYTE)((w >> 8) & 0xFF));
    }
    void addD(DWORD d) {
        buf.push_back((BYTE)(d       & 0xFF));
        buf.push_back((BYTE)((d>>8)  & 0xFF));
        buf.push_back((BYTE)((d>>16) & 0xFF));
        buf.push_back((BYTE)((d>>24) & 0xFF));
    }
    void addS(short s) { addW((WORD)s); }
    void addWStr(const wchar_t *s) {
        if (!s) { addW(0); return; }
        for (; *s; ++s) addW((WORD)*s);
        addW(0);
    }
    void align4() { while (buf.size() & 3) buf.push_back(0); }

    /* Begin: DS_MODALFRAME | DS_SETFONT | WS_POPUP | WS_CAPTION | WS_SYSMENU */
    void beginDialog(int nItems, short x, short y, short cx, short cy,
                     const wchar_t *title, const wchar_t *font = L"Segoe UI",
                     WORD fontSize = 9) {
        DWORD style = DS_MODALFRAME | DS_SETFONT | WS_POPUP |
                      WS_CAPTION | WS_SYSMENU;
        addD(style);         /* style */
        addD(0);             /* exStyle */
        addW((WORD)nItems);  /* cdit */
        addS(x); addS(y); addS(cx); addS(cy);
        addW(0);             /* menu: none */
        addW(0);             /* window class: default */
        addWStr(title);      /* title */
        addW(fontSize);      /* font size */
        addWStr(font);       /* font name */
    }

    /* Add a control (DLGITEMTEMPLATE) */
    void addItem(DWORD style, DWORD exStyle,
                 short x, short y, short cx, short cy,
                 WORD id, WORD classAtom, const wchar_t *text) {
        align4();
        addD(style);
        addD(exStyle);
        addS(x); addS(y); addS(cx); addS(cy);
        addW(id);
        addW(0xFFFF); addW(classAtom);  /* predefined class */
        addWStr(text);
        addW(0);  /* extra data: none */
        itemCount++;
    }

    DLGTEMPLATE *ptr() { return (DLGTEMPLATE*)buf.data(); }
};

/* Predefined Win32 control class atoms */
static const WORD CLS_BUTTON = 0x0080;
static const WORD CLS_EDIT   = 0x0081;
static const WORD CLS_STATIC = 0x0082;

/* -------------------------------------------------------------------------
 * Dialog API
 * ---------------------------------------------------------------------- */
void DoFind();
void DoReplace();
void DoGoToLine();
void DoAbout();
bool DoNewProject();   // returns true if project was created
void DoProjectSearch();

/* -------------------------------------------------------------------------
 * Layout
 * ---------------------------------------------------------------------- */
void LayoutCompute(HWND hw, RECT *rExp, RECT *rEd, RECT *rOut, RECT *rTab);
void LayoutApply(HWND hw);

/* -------------------------------------------------------------------------
 * Custom title bar
 * ---------------------------------------------------------------------- */
#define TITLEBAR_H     32    /* custom title bar height in px */

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
 * Build-Diagnostik ("Probleme"-Panel, NPSPEC-STUDIO-BUILD-0001)
 * Jeder Eintrag entspricht GENAU einer vom Compiler/Linker tatsaechlich
 * ausgegebenen Zeile mit Dateibezug - keine erfundenen Spalten-/Fehler-
 * code-Werte, da novac.exe diese aktuell nicht liefert (siehe
 * IMPLEMENTATION_STATUS.md). */
struct BuildDiagnostic {
    std::wstring file;
    int          line;    /* 1-basiert, 0 = unbekannt */
    std::wstring message;
};

void ProblemsClear();
void ProblemsAdd(const BuildDiagnostic &diag);
void ProblemsNavigateTo(int index);

/* 0 = Ausgabe, 1 = Probleme, 2 = Syscalls, 3 = Capabilities, 4 = Suche.
   Blendet genau EINES der fuenf Panels ein, die anderen aus (Terminal/
   Git haben kein eigenes Panel - siehe WM_NOTIFY/ID_BOTTOMTABS in
   main.cpp, zeigen nur einen Hinweis im Ausgabe-Panel). */
void BottomPanelShow(int panel);

/* Syscall Browser (NPSPEC-STUDIO-TOOLS-0001, siehe SDK_SPECIFICATION.md
   "naechster realistischer Schritt"): liest kernel/newkernel/syscall.h
   tatsaechlich ein (keine erfundene Liste) und fuellt g_hSyscalls.
   Laedt nur einmal pro Prozesslaufzeit. */
void SyscallBrowserLoad();

/* Capability Manager: liest kernel/newkernel/security.h tatsaechlich
   ein (SECURITY_CAP_*-Bitmasken) und fuellt g_hCapabilities. Deckt
   bewusst NUR das security.c-Flat-Bitmask-System ab (das einzige, das
   von Syscalls direkt gegen Ring-3-Prozesse geprueft wird) - NICHT
   cap_core.c/program.c/verify.c (laut security.h-Dateikopf drei
   WEITERE, separate Capability-Konzepte im Kernel) - siehe
   SDK_SPECIFICATION.md. Laedt nur einmal pro Prozesslaufzeit. */
void CapabilityManagerLoad();

/* Projektweite Suche (nur Lesen - KEIN "projektweites Ersetzen", das
   waere mehrdateiuebergreifend schreibend und bewusst zurueckgestellt,
   siehe IMPLEMENTATION_STATUS.md). Durchsucht rekursiv g_project.
   rootPath nach .nova/.nlf/.nui/.xml/.md/.txt-Dateien (dieselben
   Textdateitypen, die EditorOpenFile() als Text behandelt), zeigt
   Treffer in g_hSearch (Datei/Zeile/Text). Ohne offenes Projekt wird
   eine Meldung ausgegeben, es wird nichts durchsucht. */
void RunProjectSearch(const wchar_t *query);
void SearchResultNavigateTo(int index);

/* -------------------------------------------------------------------------
 * Build
 * ---------------------------------------------------------------------- */
void DoBuild(bool rebuild);
void DoRun();
void DoClean();

/* NEF Inspector (NPSPEC-STUDIO-TOOLS-0001, minimaler echter Umfang):
   ruft tools/nef/nef.exe inspect+validate auf eine .nef-Datei auf und
   zeigt das Ergebnis im Ausgabe-Panel. Wird aufgerufen, wenn eine .nef-
   Datei geoeffnet werden soll - eine NEF-Datei ist kein Text und wuerde
   im RichEdit-Editor nur als Byte-Muell erscheinen. */
void DoInspectNef(const wchar_t *path);

/* Traegt eine Datei in das offene Projekt ein (project.xml <Sources> +
   g_project.sources), falls sie innerhalb von g_project.rootPath liegt
   und noch nicht bekannt ist. Kein Effekt ohne offenes Projekt oder bei
   Dateien ausserhalb des Projektordners. Erste registrierte Datei wird
   automatisch zur neuen entryFile, falls noch keine gesetzt ist. */
void ProjectRegisterSource(const wchar_t *absolutePath);

/* Aktualisiert jede Registerkarte, Projekt-Quelle (project.xml-Eintrag
   inklusive) und Einstiegsdatei, die auf oldPath verweist, auf
   newPath - fuer ExplorerContextRename() (Datei umbenannt). Ohne
   offenes Projekt werden nur die Registerkarten aktualisiert. */
void RetargetPathReferences(const std::wstring &oldPath, const std::wstring &newPath);

struct SearchResult {
    std::wstring file;
    int          line;
    std::wstring text;
};
void SearchResultsClear();
void SearchResultsAdd(const SearchResult &r);
