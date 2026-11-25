#include "studio.h"

/* -----------------------------------------------------------------------
 * Globals defined here
 * -------------------------------------------------------------------- */
HWND g_hEditor   = nullptr;
HWND g_hTabBar   = nullptr;
HWND g_hOutput   = nullptr;
HWND g_hProblems = nullptr;
HWND g_hSyscalls = nullptr;
HWND g_hCapabilities = nullptr;
HWND g_hSearch = nullptr;

static std::vector<SearchResult> g_searchResults;
HFONT g_hCodeFont  = nullptr;

static std::vector<BuildDiagnostic> g_problems;
int  g_zoomPercent = 100;

std::vector<EditorTab> g_tabs;
int                    g_activeTab = -1;
Project                g_project   = {L"", L"", false};

static bool     g_inHighlight     = false;
static WNDPROC  g_origREProc      = nullptr;

/* Find state */
FINDREPLACEW g_fr          = {};
HWND         g_hFindDlg    = nullptr;
UINT         g_msgFindReplace = 0;

static int   g_findStart   = 0;

/* -----------------------------------------------------------------------
 * RichEdit subclass – schedule highlight + zoom on Ctrl+wheel
 * -------------------------------------------------------------------- */
static LRESULT CALLBACK EditorSubclassProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    if ((msg == WM_KEYUP || msg == WM_CHAR) && !g_inHighlight) {
        SetTimer(GetParent(hw), TIMER_HIGHLIGHT, 350, nullptr);
    }
    /* Ctrl+mouse wheel → zoom */
    if (msg == WM_MOUSEWHEEL && (GetKeyState(VK_CONTROL) & 0x8000)) {
        int delta = GET_WHEEL_DELTA_WPARAM(wp);
        int z = g_zoomPercent + (delta > 0 ? 10 : -10);
        if (z < 50)  z = 50;
        if (z > 300) z = 300;
        EditorSetZoom(z);
        return 0;
    }
    return CallWindowProcW(g_origREProc, hw, msg, wp, lp);
}

/* -----------------------------------------------------------------------
 * Tab helpers
 * -------------------------------------------------------------------- */
static void TabAddItem(const wchar_t *title) {
    TCITEMW ti = {};
    ti.mask    = TCIF_TEXT;
    ti.pszText = (LPWSTR)title;
    int idx    = TabCtrl_GetItemCount(g_hTabBar);
    TabCtrl_InsertItem(g_hTabBar, idx, &ti);
    TabCtrl_SetCurSel(g_hTabBar, idx);
}

static void TabSetTitle(int idx, const wchar_t *title) {
    TCITEMW ti = {};
    ti.mask    = TCIF_TEXT;
    ti.pszText = (LPWSTR)title;
    TabCtrl_SetItem(g_hTabBar, idx, &ti);
}

/* -----------------------------------------------------------------------
 * Syntax highlighting
 * -------------------------------------------------------------------- */
void EditorHighlight() {
    if (!g_hEditor || g_inHighlight) return;
    g_inHighlight = true;

    CHARRANGE cr_save;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr_save);

    int len = GetWindowTextLengthW(g_hEditor);
    if (len <= 0) { g_inHighlight = false; return; }

    std::wstring buf(len + 1, L'\0');
    GetWindowTextW(g_hEditor, &buf[0], len + 1);
    buf.resize(len);

    SendMessageW(g_hEditor, WM_SETREDRAW, FALSE, 0);
    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, 0);

    /* Reset all to default */
    CHARFORMAT2W cf = {};
    cf.cbSize      = sizeof(cf);
    cf.dwMask      = CFM_COLOR | CFM_BACKCOLOR;
    cf.crTextColor = g_theme.fg_default;
    cf.crBackColor = g_theme.bg_editor;
    cf.dwEffects   = 0;
    SendMessageW(g_hEditor, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    /* Apply token colours */
    auto spans = HlTokenize(buf.c_str(), len);
    for (auto &sp : spans) {
        COLORREF col = HlTokenColor(sp.type);
        if (col == g_theme.fg_default) continue;
        CHARRANGE cr = { sp.start, sp.start + sp.length };
        SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr);
        CHARFORMAT2W cf2 = {};
        cf2.cbSize      = sizeof(cf2);
        cf2.dwMask      = CFM_COLOR;
        cf2.crTextColor = col;
        cf2.dwEffects   = 0;
        SendMessageW(g_hEditor, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
    }

    SendMessageW(g_hEditor, EM_EXSETSEL,   0, (LPARAM)&cr_save);
    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE);
    SendMessageW(g_hEditor, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hEditor, nullptr, FALSE);

    g_inHighlight = false;
}

/* -----------------------------------------------------------------------
 * Bottom panel (Ausgabe/Probleme/Terminal/Git) – container + TabCtrl + Output
 * -------------------------------------------------------------------- */
#define BOTTOM_TAB_H   26   /* Höhe des Tab-Streifens */

static HWND g_hBottomCont  = nullptr;  /* äußerer Container */
static HWND g_hBottomTabCtrl = nullptr;

static void BottomContainerResize(HWND hCont, int w, int h) {
    if (!hCont) return;
    if (g_hBottomTabCtrl)
        SetWindowPos(g_hBottomTabCtrl, nullptr, 0, 0, w, BOTTOM_TAB_H,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hOutput)
        SetWindowPos(g_hOutput, nullptr, 0, BOTTOM_TAB_H, w, h - BOTTOM_TAB_H,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hProblems)
        SetWindowPos(g_hProblems, nullptr, 0, BOTTOM_TAB_H, w, h - BOTTOM_TAB_H,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hSyscalls)
        SetWindowPos(g_hSyscalls, nullptr, 0, BOTTOM_TAB_H, w, h - BOTTOM_TAB_H,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hCapabilities)
        SetWindowPos(g_hCapabilities, nullptr, 0, BOTTOM_TAB_H, w, h - BOTTOM_TAB_H,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hSearch)
        SetWindowPos(g_hSearch, nullptr, 0, BOTTOM_TAB_H, w, h - BOTTOM_TAB_H,
                     SWP_NOZORDER | SWP_NOACTIVATE);
}

/* -----------------------------------------------------------------------
 * Create editor pane
 * -------------------------------------------------------------------- */
void EditorCreate(HWND hParent, RECT rc) {
    int W = rc.right  - rc.left;
    int H = rc.bottom - rc.top;

    /* Tab bar (Datei-Tabs oben) */
    g_hTabBar = CreateWindowExW(0, WC_TABCONTROLW, nullptr,
        WS_CHILD | WS_VISIBLE | TCS_FLATBUTTONS | TCS_FOCUSNEVER,
        rc.left, rc.top, W, 26,
        hParent, (HMENU)ID_TABBAR, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hTabBar, WM_SETFONT,
                 (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    /* RichEdit (prefer Msftedit) */
    HMODULE hRE = LoadLibraryW(L"Msftedit.dll");
    if (!hRE) hRE = LoadLibraryW(L"Riched20.dll");
    const wchar_t *reClass = hRE ? MSFTEDIT_CLASS : RICHEDIT_CLASSW;

    int outH  = g_showOutput ? g_outputHeight : 0;
    int edTop = rc.top + 26;
    int edH   = H - 26 - outH - (outH > 0 ? 1 : 0);
    if (edH < 0) edH = 0;

    g_hEditor = CreateWindowExW(WS_EX_CLIENTEDGE, reClass, nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL,
        rc.left, edTop, W, edH,
        hParent, (HMENU)ID_EDITOR, GetModuleHandleW(nullptr), nullptr);

    /* Consolas 11pt */
    LOGFONTW lf = {};
    lf.lfHeight = -MulDiv(11, GetDeviceCaps(GetDC(nullptr), LOGPIXELSY), 72);
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
    wcscpy_s(lf.lfFaceName, L"Consolas");
    g_hCodeFont = CreateFontIndirectW(&lf);
    SendMessageW(g_hEditor, WM_SETFONT, (WPARAM)g_hCodeFont, TRUE);

    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE);
    SendMessageW(g_hEditor, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
    SendMessageW(g_hEditor, EM_SETUNDOLIMIT, 500, 0);

    PARAFORMAT2 pf = {};
    pf.cbSize    = sizeof(pf);
    pf.dwMask    = PFM_TABSTOPS;
    pf.cTabCount = 1;
    pf.rgxTabs[0] = 720;
    SendMessageW(g_hEditor, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

    g_origREProc = (WNDPROC)SetWindowLongPtrW(g_hEditor, GWLP_WNDPROC,
                                               (LONG_PTR)EditorSubclassProc);

    /* ---- Bottom container mit Tabs (Ausgabe / Probleme / Terminal / Git) ---- */
    int botTop = edTop + edH + 1;
    g_hBottomCont = CreateWindowExW(0, L"STATIC", nullptr,
        WS_CHILD | (g_showOutput ? WS_VISIBLE : 0) | WS_CLIPCHILDREN | SS_OWNERDRAW,
        rc.left, botTop, W, outH,
        hParent, (HMENU)ID_BOTTOMPANEL, GetModuleHandleW(nullptr), nullptr);

    /* Tab-Streifen im Bottom-Container */
    g_hBottomTabCtrl = CreateWindowExW(0, WC_TABCONTROLW, nullptr,
        WS_CHILD | WS_VISIBLE | TCS_FLATBUTTONS | TCS_FOCUSNEVER,
        0, 0, W, BOTTOM_TAB_H,
        g_hBottomCont, (HMENU)ID_BOTTOMTABS, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hBottomTabCtrl, WM_SETFONT,
                 (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    /* Tabs einfügen */
    {
        TCITEMW ti = {};
        ti.mask = TCIF_TEXT;
        struct { LPCWSTR name; } tabs[] = {
            {L"Ausgabe"}, {L"Probleme (0)"}, {L"Syscalls"}, {L"Capabilities"}, {L"Suche"}, {L"Terminal"}, {L"Git"}
        };
        for (int i = 0; i < 7; i++) {
            ti.pszText = (LPWSTR)tabs[i].name;
            TabCtrl_InsertItem(g_hBottomTabCtrl, i, &ti);
        }
        TabCtrl_SetCurSel(g_hBottomTabCtrl, 0);
    }

    /* Output RichEdit */
    g_hOutput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        0, BOTTOM_TAB_H, W, outH - BOTTOM_TAB_H,
        g_hBottomCont, (HMENU)ID_OUTPUT, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hOutput, WM_SETFONT, (WPARAM)g_hCodeFont, TRUE);

    /* Probleme-ListView (NPSPEC-STUDIO-BUILD-0001): zeigt echte Build-
       Diagnostik mit Dateibezug, anfangs ausgeblendet (Tab "Ausgabe" ist
       aktiv) - siehe ProblemsShow()/DoBuild(). */
    g_hProblems = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
        WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        0, BOTTOM_TAB_H, W, outH - BOTTOM_TAB_H,
        g_hBottomCont, (HMENU)ID_PROBLEMS, GetModuleHandleW(nullptr), nullptr);
    ListView_SetExtendedListViewStyle(g_hProblems, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    SendMessageW(g_hProblems, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 220; col.pszText = (LPWSTR)L"Datei";
        ListView_InsertColumn(g_hProblems, 0, &col);
        col.cx = 60;  col.pszText = (LPWSTR)L"Zeile";
        ListView_InsertColumn(g_hProblems, 1, &col);
        col.cx = 600; col.pszText = (LPWSTR)L"Meldung";
        ListView_InsertColumn(g_hProblems, 2, &col);
    }

    /* Syscall-Browser-ListView (siehe SyscallBrowserLoad() in main.cpp). */
    g_hSyscalls = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
        WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        0, BOTTOM_TAB_H, W, outH - BOTTOM_TAB_H,
        g_hBottomCont, (HMENU)ID_SYSCALLS, GetModuleHandleW(nullptr), nullptr);
    ListView_SetExtendedListViewStyle(g_hSyscalls, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    SendMessageW(g_hSyscalls, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 320; col.pszText = (LPWSTR)L"Name";
        ListView_InsertColumn(g_hSyscalls, 0, &col);
        col.cx = 80;  col.pszText = (LPWSTR)L"Wert";
        ListView_InsertColumn(g_hSyscalls, 1, &col);
        col.cx = 480; col.pszText = (LPWSTR)L"Kommentar (gleiche Zeile)";
        ListView_InsertColumn(g_hSyscalls, 2, &col);
    }

    /* Capability-Manager-ListView (siehe CapabilityManagerLoad() in main.cpp). */
    g_hCapabilities = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
        WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        0, BOTTOM_TAB_H, W, outH - BOTTOM_TAB_H,
        g_hBottomCont, (HMENU)ID_CAPABILITIES, GetModuleHandleW(nullptr), nullptr);
    ListView_SetExtendedListViewStyle(g_hCapabilities, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    SendMessageW(g_hCapabilities, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 320; col.pszText = (LPWSTR)L"Name";
        ListView_InsertColumn(g_hCapabilities, 0, &col);
        col.cx = 120; col.pszText = (LPWSTR)L"Wert (Hex)";
        ListView_InsertColumn(g_hCapabilities, 1, &col);
        col.cx = 80;  col.pszText = (LPWSTR)L"Bit";
        ListView_InsertColumn(g_hCapabilities, 2, &col);
    }

    /* Projektweite-Suche-ListView (siehe RunProjectSearch() in main.cpp). */
    g_hSearch = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
        WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        0, BOTTOM_TAB_H, W, outH - BOTTOM_TAB_H,
        g_hBottomCont, (HMENU)ID_SEARCH, GetModuleHandleW(nullptr), nullptr);
    ListView_SetExtendedListViewStyle(g_hSearch, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    SendMessageW(g_hSearch, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 220; col.pszText = (LPWSTR)L"Datei";
        ListView_InsertColumn(g_hSearch, 0, &col);
        col.cx = 60;  col.pszText = (LPWSTR)L"Zeile";
        ListView_InsertColumn(g_hSearch, 1, &col);
        col.cx = 600; col.pszText = (LPWSTR)L"Text";
        ListView_InsertColumn(g_hSearch, 2, &col);
    }

    /* g_hBottomContainer / g_hBottomTabs für externe Zugriffe */
    g_hBottomContainer = g_hBottomCont;
    g_hBottomTabs      = g_hBottomTabCtrl;

    EditorApplyTheme();
    EditorNewFile();

    /* Register Find/Replace message */
    g_msgFindReplace = RegisterWindowMessageW(FINDMSGSTRINGW);
}

void EditorResize(RECT rc) {
    int tabH  = 26;
    int outH  = g_showOutput ? g_outputHeight : 0;
    int w     = rc.right  - rc.left;
    int h     = rc.bottom - rc.top;
    int edH   = h - tabH - outH - (outH > 0 ? 1 : 0);
    if (edH < 0) edH = 0;

    if (g_hTabBar)
        SetWindowPos(g_hTabBar, nullptr,
                     rc.left, rc.top, w, tabH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hEditor)
        SetWindowPos(g_hEditor, nullptr,
                     rc.left, rc.top + tabH, w, edH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hBottomCont) {
        int botY = rc.top + tabH + edH + (outH > 0 ? 1 : 0);
        SetWindowPos(g_hBottomCont, nullptr,
                     rc.left, botY, w, outH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        BottomContainerResize(g_hBottomCont, w, outH);
        ShowWindow(g_hBottomCont, g_showOutput ? SW_SHOW : SW_HIDE);
    }
}

void EditorApplyTheme() {
    if (g_hEditor) {
        SendMessageW(g_hEditor, EM_SETBKGNDCOLOR, 0, (LPARAM)g_theme.bg_editor);
        CHARFORMAT2W cf = {};
        cf.cbSize      = sizeof(cf);
        cf.dwMask      = CFM_COLOR | CFM_BACKCOLOR;
        cf.crTextColor = g_theme.fg_default;
        cf.crBackColor = g_theme.bg_editor;
        cf.dwEffects   = 0;
        SendMessageW(g_hEditor, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    }
    if (g_hOutput) {
        SendMessageW(g_hOutput, EM_SETBKGNDCOLOR, 0, (LPARAM)g_theme.bg_panel);
    }
    /* Alle ListView-Werkzeug-Panels (Probleme/Syscalls/Capabilities/
       Suche) - vorher nicht eingefaerbt, zeigten im Dunkel-Design einen
       optisch inkonsistenten Standard-Weiss/Schwarz-Hintergrund. */
    for (HWND h : { g_hProblems, g_hSyscalls, g_hCapabilities, g_hSearch }) {
        if (!h) continue;
        ListView_SetBkColor(h, g_theme.bg_panel);
        ListView_SetTextColor(h, g_theme.fg_default);
        ListView_SetTextBkColor(h, g_theme.bg_panel);
    }
    EditorHighlight();
}

/* -----------------------------------------------------------------------
 * Line/column status
 * -------------------------------------------------------------------- */
void EditorGetCaretPos(int *pLine, int *pCol) {
    if (!g_hEditor) { *pLine = 1; *pCol = 1; return; }
    CHARRANGE cr;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);
    int line = (int)SendMessageW(g_hEditor, EM_EXLINEFROMCHAR, 0, (LPARAM)cr.cpMin);
    int lineStart = (int)SendMessageW(g_hEditor, EM_LINEINDEX, (WPARAM)line, 0);
    *pLine = line + 1;
    *pCol  = cr.cpMin - lineStart + 1;
}

void EditorUpdateStatusBar() {
    if (!g_hStatusBar) return;

    int line, col;
    EditorGetCaretPos(&line, &col);

    /* Part 0: Ready */
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)L"  Ready");

    /* Part 1: Ln / Col */
    wchar_t buf[128];
    swprintf_s(buf, L"  Ln %d, Col %d", line, col);
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 1, (LPARAM)buf);

    /* Parts 2-5 are static (set in StatusCreate), just refresh language */
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 5, (LPARAM)L"  NovaLang");
}

/* Called from WM_NOTIFY EN_SELCHANGE */
void EditorOnSelChange() {
    EditorUpdateStatusBar();
    /* Reset find start position on cursor move */
    if (g_hEditor) {
        CHARRANGE cr;
        SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);
        g_findStart = cr.cpMin;
    }
}

void EditorSyncActiveTabContent() {
    if (g_activeTab < 0 || g_activeTab >= (int)g_tabs.size() || !g_hEditor) return;
    int len = GetWindowTextLengthW(g_hEditor);
    std::wstring wt(len + 1, L'\0');
    GetWindowTextW(g_hEditor, &wt[0], len + 1);
    wt.resize(len);
    g_tabs[g_activeTab].content = wt;
}

/* -----------------------------------------------------------------------
 * File operations
 * -------------------------------------------------------------------- */
void EditorNewFile() {
    EditorSyncActiveTabContent(); /* siehe EditorOpenFile() - gleicher Grund */
    EditorTab tab;
    tab.filepath = L"";
    tab.title    = L"Untitled.nova";
    tab.modified = false;
    g_tabs.push_back(tab);
    g_activeTab = (int)g_tabs.size() - 1;
    TabAddItem(tab.title.c_str());
    if (g_hEditor) {
        SetWindowTextW(g_hEditor, L"");
        EditorHighlight();
    }
    EditorUpdateTitle();
}

bool EditorOpenFile(const wchar_t *path) {
    /* .nef-Dateien sind kein Text - im RichEdit-Editor wuerden sie nur
       als Byte-Muell erscheinen (MultiByteToWideChar auf rohe
       Maschinencode-/Daten-Bytes). Stattdessen echte NEF-Inspektion
       (siehe DoInspectNef(), NEF Inspector). */
    size_t plen = wcslen(path);
    if (plen > 4 && _wcsicmp(path + plen - 4, L".nef") == 0) {
        DoInspectNef(path);
        return true;
    }

    /* Bereits offen? Dann NICHT neu von der Festplatte laden (das wuerde
       ungespeicherte Aenderungen verwerfen) und KEINEN zweiten Tab fuer
       dieselbe Datei anlegen (vorher: das "break" unten beendete nur die
       Schleife, der Rest der Funktion lief trotzdem weiter und erzeugte
       immer einen neuen Tab - echter, bereits vorbestehender Bug). */
    for (int i = 0; i < (int)g_tabs.size(); i++) {
        if (_wcsicmp(g_tabs[i].filepath.c_str(), path) == 0) {
            if (i != g_activeTab) EditorSyncActiveTabContent();
            g_activeTab = i;
            TabCtrl_SetCurSel(g_hTabBar, i);
            SetWindowTextW(g_hEditor, g_tabs[i].content.c_str());
            EditorHighlight();
            EditorUpdateTitle();
            return true;
        }
    }

    HANDLE hf = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) return false;

    DWORD sz = GetFileSize(hf, nullptr);
    if (sz == INVALID_FILE_SIZE || sz > 8 * 1024 * 1024) { CloseHandle(hf); return false; }

    std::string raw(sz + 1, '\0');
    DWORD rd = 0;
    ReadFile(hf, &raw[0], sz, &rd, nullptr);
    CloseHandle(hf);

    int wlen = MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, nullptr, 0);
    std::wstring wtext(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, &wtext[0], wlen);

    /* Bisherige aktive Registerkarte sichern, BEVOR g_hEditor mit dem
       Inhalt der neuen Registerkarte ueberschrieben wird - sonst gehen
       ihre ungespeicherten Aenderungen verloren (derselbe Bug wie beim
       Tab-Wechsel, hier beim Anlegen eines NEUEN Tabs fuer eine noch
       nicht geoeffnete Datei). */
    EditorSyncActiveTabContent();

    /* Create new tab */
    EditorTab tab;
    tab.filepath = path;
    wchar_t name[MAX_PATH];
    wcscpy_s(name, path);
    PathStripPathW(name);
    tab.title    = name;
    tab.modified = false;
    tab.content  = wtext;
    g_tabs.push_back(tab);
    g_activeTab = (int)g_tabs.size() - 1;
    TabAddItem(tab.title.c_str());

    SetWindowTextW(g_hEditor, wtext.c_str());
    EditorHighlight();
    EditorUpdateTitle();

    /* Show file type in status bar (part 5 = language) */
    const wchar_t *ext = PathFindExtensionW(name);
    wchar_t ft[64];
    if      (_wcsicmp(ext, L".nova") == 0) wcscpy_s(ft, L"  NovaLang");
    else if (_wcsicmp(ext, L".nlf")  == 0) wcscpy_s(ft, L"  NovaLogic");
    else if (_wcsicmp(ext, L".nui")  == 0) wcscpy_s(ft, L"  NovaUI");
    else if (_wcsicmp(ext, L".xml")  == 0) wcscpy_s(ft, L"  XML");
    else if (_wcsicmp(ext, L".md")   == 0) wcscpy_s(ft, L"  Markdown");
    else                                    wcscpy_s(ft, L"  Text");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 5, (LPARAM)ft);

    return true;
}

bool EditorSaveFile() {
    if (g_activeTab < 0 || g_activeTab >= (int)g_tabs.size()) return false;
    auto &tab = g_tabs[g_activeTab];
    if (tab.filepath.empty()) return EditorSaveFileAs();

    int len = GetWindowTextLengthW(g_hEditor);
    std::wstring wtext(len + 1, L'\0');
    GetWindowTextW(g_hEditor, &wtext[0], len + 1);
    wtext.resize(len);

    int mblen = WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string raw(mblen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, &raw[0], mblen, nullptr, nullptr);

    HANDLE hf = CreateFileW(tab.filepath.c_str(), GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        MessageBoxW(g_hMain, L"Datei konnte nicht gespeichert werden.",
                    L"Fehler", MB_OK | MB_ICONERROR);
        return false;
    }
    DWORD written;
    WriteFile(hf, raw.c_str(), (DWORD)(mblen - 1), &written, nullptr);
    CloseHandle(hf);

    tab.modified = false;
    tab.content  = wtext;
    TabSetTitle(g_activeTab, tab.title.c_str());
    EditorUpdateTitle();
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)L"  Gespeichert");
    SetTimer(g_hMain, TIMER_STATUS, 2000, nullptr);
    return true;
}

/* "Alle speichern" (vorher ein echter Bug: rief nur EditorSaveFile() auf
   die aktuell aktive Registerkarte auf - alle anderen Tabs blieben
   trotz des Knopfnamens ungespeichert). Sichert zuerst die aktive
   Registerkarte (live aus g_hEditor), dann jede weitere modifizierte
   Registerkarte ueber ihren jeweiligen Content-Spiegel (siehe
   EditorTab::content), ohne die sichtbare aktive Registerkarte zu
   aendern. */
void EditorSaveAllFiles() {
    if (g_activeTab < 0 || g_activeTab >= (int)g_tabs.size()) return;

    /* Aktive Registerkarte zuerst: ihr Live-Text steht in g_hEditor,
       nicht im Content-Spiegel (der wird erst beim Verlassen aktualisiert). */
    if (g_tabs[g_activeTab].modified) EditorSaveFile();

    const int activeBefore = g_activeTab;
    for (int i = 0; i < (int)g_tabs.size(); i++) {
        if (i == activeBefore || !g_tabs[i].modified) continue;

        /* Temporaer in den Editor laden, speichern, dann zurueckwechseln -
           EditorSaveFile() arbeitet immer auf g_activeTab/g_hEditor. */
        g_activeTab = i;
        SetWindowTextW(g_hEditor, g_tabs[i].content.c_str());
        EditorSaveFile();
    }

    g_activeTab = activeBefore;
    SetWindowTextW(g_hEditor, g_tabs[activeBefore].content.c_str());
    EditorHighlight();
    EditorUpdateTitle();
}

bool EditorSaveFileAs() {
    if (g_activeTab < 0) return false;
    wchar_t path[MAX_PATH] = {};
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = g_hMain;
    ofn.lpstrFilter = L"NovaLang\0*.nova;*.nlf;*.nui\0XML\0*.xml\0Alle Dateien\0*.*\0";
    ofn.lpstrFile   = path;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrDefExt = L"nova";
    ofn.Flags       = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&ofn)) return false;

    g_tabs[g_activeTab].filepath = path;
    wchar_t name[MAX_PATH];
    wcscpy_s(name, path);
    PathStripPathW(name);
    g_tabs[g_activeTab].title = name;
    bool ok = EditorSaveFile();
    if (ok) ProjectRegisterSource(path);
    return ok;
}

void EditorCloseTab(int idx) {
    if (idx < 0 || idx >= (int)g_tabs.size()) return;
    auto &tab = g_tabs[idx];
    if (tab.modified) {
        wchar_t msg[MAX_PATH + 64];
        swprintf_s(msg, L"Änderungen in '%s' speichern?", tab.title.c_str());
        int r = MessageBoxW(g_hMain, msg, L"Nova Studio", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (r == IDCANCEL) return;
        if (r == IDYES) {
            g_activeTab = idx;
            EditorSaveFile();
        }
    }
    TabCtrl_DeleteItem(g_hTabBar, idx);
    g_tabs.erase(g_tabs.begin() + idx);
    if (g_tabs.empty()) {
        SetWindowTextW(g_hEditor, L"");
        g_activeTab = -1;
        EditorNewFile();
    } else {
        g_activeTab = std::min(idx, (int)g_tabs.size() - 1);
        TabCtrl_SetCurSel(g_hTabBar, g_activeTab);
        EditorOpenFile(g_tabs[g_activeTab].filepath.c_str());
    }
}

void EditorUpdateTitle() {
    if (g_activeTab < 0 || g_activeTab >= (int)g_tabs.size()) {
        SetWindowTextW(g_hMain, STUDIO_NAME L" " STUDIO_VERSION);
        return;
    }
    auto &tab = g_tabs[g_activeTab];
    /* Format: "ProjectName - Debug | NovaStudio" or "file.nova | NovaStudio" */
    std::wstring title;
    if (!g_project.name.empty())
        title = g_project.name + L" - Debug";
    else
        title = tab.title;
    if (tab.modified) title += L" ●";
    title += L" | " STUDIO_NAME;
    SetWindowTextW(g_hMain, title.c_str());
}

/* -----------------------------------------------------------------------
 * Output panel
 * -------------------------------------------------------------------- */
void EditorOutput(const wchar_t *text, bool newline) {
    if (!g_hOutput) return;
    int len = GetWindowTextLengthW(g_hOutput);
    SendMessageW(g_hOutput, EM_SETSEL, len, len);
    SendMessageW(g_hOutput, EM_REPLACESEL, FALSE, (LPARAM)text);
    if (newline)
        SendMessageW(g_hOutput, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
    SendMessageW(g_hOutput, EM_SCROLL, SB_BOTTOM, 0);
}

void EditorOutputClear() {
    if (g_hOutput) SetWindowTextW(g_hOutput, L"");
}

/* -----------------------------------------------------------------------
 * Probleme-Panel (echte Build-Diagnostik, siehe DoBuild() in main.cpp)
 * -------------------------------------------------------------------- */
static void ProblemsUpdateTabLabel() {
    if (!g_hBottomTabs) return;
    wchar_t buf[32];
    swprintf_s(buf, L"Probleme (%d)", (int)g_problems.size());
    TCITEMW ti = {};
    ti.mask = TCIF_TEXT;
    ti.pszText = buf;
    TabCtrl_SetItem(g_hBottomTabs, 1, &ti);
}

void ProblemsClear() {
    g_problems.clear();
    if (g_hProblems) ListView_DeleteAllItems(g_hProblems);
    ProblemsUpdateTabLabel();
}

void ProblemsAdd(const BuildDiagnostic &diag) {
    g_problems.push_back(diag);
    if (g_hProblems) {
        int idx = (int)g_problems.size() - 1;

        /* Anzeige: nur der Dateiname (kurz, lesbar). diag.file selbst
           bleibt der VOLLE Pfad - ProblemsNavigateTo() braucht ihn, um
           die Datei ueberhaupt oeffnen zu koennen. */
        wchar_t shortName[MAX_PATH];
        wcscpy_s(shortName, diag.file.c_str());
        PathStripPathW(shortName);

        LVITEMW it = {};
        it.mask = LVIF_TEXT;
        it.iItem = idx;
        it.iSubItem = 0;
        it.pszText = shortName;
        ListView_InsertItem(g_hProblems, &it);

        wchar_t lineBuf[16];
        if (diag.line > 0) swprintf_s(lineBuf, L"%d", diag.line);
        else wcscpy_s(lineBuf, L"");
        ListView_SetItemText(g_hProblems, idx, 1, lineBuf);
        ListView_SetItemText(g_hProblems, idx, 2, (LPWSTR)diag.message.c_str());
    }
    ProblemsUpdateTabLabel();
}

void BottomPanelShow(int panel) {
    if (g_hOutput)       ShowWindow(g_hOutput,       panel == 0 ? SW_SHOW : SW_HIDE);
    if (g_hProblems)     ShowWindow(g_hProblems,     panel == 1 ? SW_SHOW : SW_HIDE);
    if (g_hSyscalls)     ShowWindow(g_hSyscalls,     panel == 2 ? SW_SHOW : SW_HIDE);
    if (g_hCapabilities) ShowWindow(g_hCapabilities, panel == 3 ? SW_SHOW : SW_HIDE);
    if (g_hSearch)       ShowWindow(g_hSearch,       panel == 4 ? SW_SHOW : SW_HIDE);
}

void SearchResultNavigateTo(int index) {
    if (index < 0 || index >= (int)g_searchResults.size()) return;
    const SearchResult &r = g_searchResults[index];
    if (!r.file.empty()) EditorOpenFile(r.file.c_str());
    if (r.line > 0) EditorGoToLine(r.line);
}

void SearchResultsClear() {
    g_searchResults.clear();
    if (g_hSearch) ListView_DeleteAllItems(g_hSearch);
}

void SearchResultsAdd(const SearchResult &r) {
    g_searchResults.push_back(r);
    if (!g_hSearch) return;
    int idx = (int)g_searchResults.size() - 1;

    wchar_t shortName[MAX_PATH];
    wcscpy_s(shortName, r.file.c_str());
    PathStripPathW(shortName);

    LVITEMW it = {};
    it.mask = LVIF_TEXT;
    it.iItem = idx;
    it.iSubItem = 0;
    it.pszText = shortName;
    ListView_InsertItem(g_hSearch, &it);

    wchar_t lineBuf[16];
    swprintf_s(lineBuf, L"%d", r.line);
    ListView_SetItemText(g_hSearch, idx, 1, lineBuf);
    ListView_SetItemText(g_hSearch, idx, 2, (LPWSTR)r.text.c_str());
}

void ProblemsNavigateTo(int index) {
    if (index < 0 || index >= (int)g_problems.size()) return;
    const BuildDiagnostic &d = g_problems[index];
    if (!d.file.empty()) EditorOpenFile(d.file.c_str());
    if (d.line > 0) EditorGoToLine(d.line);
}

/* -----------------------------------------------------------------------
 * Zoom
 * -------------------------------------------------------------------- */
void EditorSetZoom(int percent) {
    g_zoomPercent = percent;
    if (g_hEditor)
        SendMessageW(g_hEditor, EM_SETZOOM, percent, 100);
    EditorUpdateStatusBar();
}

/* -----------------------------------------------------------------------
 * Go to line
 * -------------------------------------------------------------------- */
void EditorGoToLine(int line) {
    if (!g_hEditor || line < 1) return;
    int totalLines = (int)SendMessageW(g_hEditor, EM_GETLINECOUNT, 0, 0);
    if (line > totalLines) line = totalLines;
    int idx = (int)SendMessageW(g_hEditor, EM_LINEINDEX, (WPARAM)(line - 1), 0);
    if (idx < 0) return;
    CHARRANGE cr = { idx, idx };
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr);
    SendMessageW(g_hEditor, EM_SCROLLCARET, 0, 0);
    SetFocus(g_hEditor);
}

/* -----------------------------------------------------------------------
 * Find / Replace
 * -------------------------------------------------------------------- */
bool EditorFindNext(const wchar_t *text, DWORD flags) {
    if (!g_hEditor || !text || !*text) return false;

    CHARRANGE crSel;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&crSel);

    FINDTEXTEXW ft = {};
    ft.chrg.cpMin = crSel.cpMax;
    ft.chrg.cpMax = -1;
    ft.lpstrText  = text;

    DWORD findFlags = (flags & FR_DOWN) ? FR_DOWN : 0;
    if (flags & FR_MATCHCASE) findFlags |= FR_MATCHCASE;
    if (flags & FR_WHOLEWORD) findFlags |= FR_WHOLEWORD;

    LRESULT pos = SendMessageW(g_hEditor, EM_FINDTEXTEXW,
                               (WPARAM)findFlags, (LPARAM)&ft);
    if (pos < 0) {
        /* Wrap around */
        ft.chrg.cpMin = 0;
        ft.chrg.cpMax = crSel.cpMax;
        pos = SendMessageW(g_hEditor, EM_FINDTEXTEXW,
                           (WPARAM)findFlags, (LPARAM)&ft);
        if (pos < 0) {
            MessageBeep(MB_ICONASTERISK);
            return false;
        }
    }
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
    SendMessageW(g_hEditor, EM_SCROLLCARET, 0, 0);
    return true;
}

bool EditorReplaceNext(const wchar_t *findText, const wchar_t *replaceText, DWORD flags) {
    if (!g_hEditor || !findText || !*findText) return false;

    CHARRANGE cr;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);

    if (cr.cpMin != cr.cpMax) {
        int selLen = GetWindowTextLengthW(g_hEditor);
        std::wstring buf(selLen + 1, L'\0');
        GetWindowTextW(g_hEditor, &buf[0], selLen + 1);
        buf.resize(selLen);
        std::wstring sel = buf.substr(cr.cpMin, cr.cpMax - cr.cpMin);
        std::wstring find = findText;
        bool match;
        if (flags & FR_MATCHCASE)
            match = (sel == find);
        else
            match = (_wcsicmp(sel.c_str(), find.c_str()) == 0);
        if (match) {
            SendMessageW(g_hEditor, EM_REPLACESEL, TRUE, (LPARAM)replaceText);
        }
    }
    return EditorFindNext(findText, flags);
}

int EditorReplaceAll(const wchar_t *findText, const wchar_t *replaceText, DWORD flags) {
    if (!g_hEditor || !findText || !*findText) return 0;

    int count = 0;
    CHARRANGE crOrig;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&crOrig);

    CHARRANGE cr0 = { 0, 0 };
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr0);

    FINDTEXTEXW ft = {};
    DWORD findFlags = (flags & FR_MATCHCASE) ? FR_MATCHCASE : 0;
    if (flags & FR_WHOLEWORD) findFlags |= FR_WHOLEWORD;

    SendMessageW(g_hEditor, WM_SETREDRAW, FALSE, 0);
    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, 0);

    ft.chrg.cpMin = 0;
    ft.chrg.cpMax = -1;
    ft.lpstrText  = findText;

    while (true) {
        LRESULT pos = SendMessageW(g_hEditor, EM_FINDTEXTEXW,
                                   (WPARAM)findFlags, (LPARAM)&ft);
        if (pos < 0) break;
        SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
        SendMessageW(g_hEditor, EM_REPLACESEL, TRUE, (LPARAM)replaceText);
        ft.chrg.cpMin = ft.chrgText.cpMin + (LONG)wcslen(replaceText);
        ft.chrg.cpMax = -1;
        count++;
    }

    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE);
    SendMessageW(g_hEditor, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hEditor, nullptr, FALSE);
    EditorHighlight();
    return count;
}

/* -----------------------------------------------------------------------
 * Comment/uncomment current line
 * -------------------------------------------------------------------- */
void EditorCommentToggle() {
    if (!g_hEditor) return;
    CHARRANGE cr;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);
    int line = (int)SendMessageW(g_hEditor, EM_EXLINEFROMCHAR, 0, (LPARAM)cr.cpMin);
    int lineStart = (int)SendMessageW(g_hEditor, EM_LINEINDEX, (WPARAM)line, 0);
    int lineLen   = (int)SendMessageW(g_hEditor, EM_LINELENGTH, (WPARAM)lineStart, 0);

    if (lineLen <= 0) return;

    std::wstring lineBuf(lineLen + 1, L'\0');
    WORD wl = (WORD)lineLen;
    memcpy(&lineBuf[0], &wl, sizeof(WORD));
    int got = (int)SendMessageW(g_hEditor, EM_GETLINE, (WPARAM)line, (LPARAM)lineBuf.c_str());
    lineBuf.resize(got);

    /* Find first non-space */
    int firstNonSpace = 0;
    while (firstNonSpace < (int)lineBuf.size() && lineBuf[firstNonSpace] == L' ')
        firstNonSpace++;

    bool isCommented = (firstNonSpace < (int)lineBuf.size() &&
                        lineBuf[firstNonSpace] == L'\'');

    CHARRANGE selLine = { lineStart, lineStart + lineLen };
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&selLine);

    std::wstring newLine;
    if (isCommented) {
        /* Remove leading ' */
        newLine = lineBuf.substr(0, firstNonSpace) +
                  lineBuf.substr(firstNonSpace + 1);
    } else {
        /* Add ' at first non-space position */
        newLine = lineBuf.substr(0, firstNonSpace) + L"'" +
                  lineBuf.substr(firstNonSpace);
    }
    SendMessageW(g_hEditor, EM_REPLACESEL, TRUE, (LPARAM)newLine.c_str());

    /* Restore caret */
    CHARRANGE crNew = { lineStart + (LONG)newLine.size(),
                        lineStart + (LONG)newLine.size() };
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&crNew);
    EditorHighlight();
}
