#include "studio.h"
#include <cstdio>

/* -------------------------------------------------------------------------
 * Globals (defined in main.cpp, declared in studio.h)
 * ---------------------------------------------------------------------- */
HWND g_hEditor    = nullptr;
HWND g_hTabBar    = nullptr;
HWND g_hOutput    = nullptr;
HFONT g_hCodeFont = nullptr;
bool g_highlightPending = false;
std::vector<EditorTab> g_tabs;
int g_activeTab = -1;

static bool g_inHighlight = false;  /* recursion guard */
int         g_zoomPercent = 100;
static WNDPROC g_origRichEditProc = nullptr;

#define TIMER_HIGHLIGHT 1

/* -------------------------------------------------------------------------
 * RichEdit subclass – capture WM_KEYUP/WM_CHAR to schedule highlight
 * ---------------------------------------------------------------------- */
static LRESULT CALLBACK EditorSubclassProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_KEYUP || msg == WM_CHAR) {
        if (!g_inHighlight) {
            g_highlightPending = true;
            SetTimer(GetParent(hw), TIMER_HIGHLIGHT, 300, nullptr);
        }
    }
    return CallWindowProcW(g_origRichEditProc, hw, msg, wp, lp);
}

/* -------------------------------------------------------------------------
 * Tab bar helpers
 * ---------------------------------------------------------------------- */
static void TabAddItem(const wchar_t *title) {
    TCITEMW ti = {};
    ti.mask    = TCIF_TEXT;
    ti.pszText = (LPWSTR)title;
    int idx = TabCtrl_GetItemCount(g_hTabBar);
    TabCtrl_InsertItem(g_hTabBar, idx, &ti);
    TabCtrl_SetCurSel(g_hTabBar, idx);
}

static void TabUpdateTitle(int idx, const wchar_t *title) {
    TCITEMW ti = {};
    ti.mask    = TCIF_TEXT;
    ti.pszText = (LPWSTR)title;
    TabCtrl_SetItem(g_hTabBar, idx, &ti);
}

/* -------------------------------------------------------------------------
 * Syntax highlight the full editor content
 * ---------------------------------------------------------------------- */
void EditorHighlight() {
    if (!g_hEditor || g_inHighlight) return;
    g_inHighlight = true;

    /* Save selection */
    CHARRANGE cr_save;
    SendMessageW(g_hEditor, EM_GETSEL, 0, (LPARAM)&cr_save);

    int textLen = GetWindowTextLengthW(g_hEditor);
    if (textLen <= 0) { g_inHighlight = false; return; }

    std::wstring buf(textLen + 1, L'\0');
    GetWindowTextW(g_hEditor, &buf[0], textLen + 1);
    buf.resize(textLen);

    /* Freeze redraw */
    SendMessageW(g_hEditor, WM_SETREDRAW, FALSE, 0);
    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, 0);

    /* Reset all text to default color first */
    CHARFORMAT2W cf = {};
    cf.cbSize    = sizeof(cf);
    cf.dwMask    = CFM_COLOR | CFM_BACKCOLOR;
    cf.crTextColor = g_theme.fg_default;
    cf.crBackColor = g_theme.bg_editor;
    cf.dwEffects = 0;
    SendMessageW(g_hEditor, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    /* Apply token colors */
    auto spans = HlTokenize(buf.c_str(), textLen);
    for (auto &sp : spans) {
        COLORREF col = HlTokenColor(sp.type);
        if (col == g_theme.fg_default) continue; /* skip default, already set */
        CHARRANGE cr = { sp.start, sp.start + sp.length };
        SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr);
        CHARFORMAT2W cf2 = {};
        cf2.cbSize      = sizeof(cf2);
        cf2.dwMask      = CFM_COLOR;
        cf2.crTextColor = col;
        cf2.dwEffects   = 0;
        SendMessageW(g_hEditor, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2);
    }

    /* Restore selection */
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&cr_save);
    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, ENM_CHANGE);

    /* Unfreeze */
    SendMessageW(g_hEditor, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hEditor, nullptr, FALSE);

    g_inHighlight = false;
    g_highlightPending = false;
}

/* -------------------------------------------------------------------------
 * Create editor pane (TabBar + RichEdit)
 * ---------------------------------------------------------------------- */
void EditorCreate(HWND hParent, RECT rc) {
    /* Tab bar */
    g_hTabBar = CreateWindowExW(0, WC_TABCONTROLW, nullptr,
        WS_CHILD | WS_VISIBLE | TCS_FLATBUTTONS | TCS_FOCUSNEVER,
        rc.left, rc.top, rc.right - rc.left, 26,
        hParent, (HMENU)ID_TABBAR, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hTabBar, WM_SETFONT,
        (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    /* RichEdit */
    HMODULE hRE = LoadLibraryW(L"Msftedit.dll");
    if (!hRE) hRE = LoadLibraryW(L"Riched20.dll");
    const wchar_t *reClass = hRE ? MSFTEDIT_CLASS : RICHEDIT_CLASSW;

    g_hEditor = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        reClass, nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL,
        rc.left, rc.top + 26,
        rc.right - rc.left, rc.bottom - rc.top - 26,
        hParent, (HMENU)ID_EDITOR,
        GetModuleHandleW(nullptr), nullptr);

    /* Code font – Consolas 11pt */
    LOGFONTW lf = {};
    lf.lfHeight = -MulDiv(11, GetDeviceCaps(GetDC(nullptr), LOGPIXELSY), 72);
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
    wcscpy_s(lf.lfFaceName, L"Consolas");
    g_hCodeFont = CreateFontIndirectW(&lf);
    SendMessageW(g_hEditor, WM_SETFONT, (WPARAM)g_hCodeFont, TRUE);

    /* RichEdit settings */
    SendMessageW(g_hEditor, EM_SETEVENTMASK, 0, ENM_CHANGE);
    SendMessageW(g_hEditor, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
    SendMessageW(g_hEditor, EM_SETUNDOLIMIT, 1000, 0);

    /* Tab size: 4 spaces */
    PARAFORMAT2 pf = {};
    pf.cbSize    = sizeof(pf);
    pf.dwMask    = PFM_TABSTOPS;
    pf.cTabCount = 1;
    pf.rgxTabs[0] = 720; /* 0.5 inch in twips */
    SendMessageW(g_hEditor, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

    /* Subclass to detect keystrokes */
    g_origRichEditProc = (WNDPROC)SetWindowLongPtrW(g_hEditor, GWLP_WNDPROC,
        (LONG_PTR)EditorSubclassProc);

    /* Output panel */
    g_hOutput = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        rc.left, rc.bottom - 150,
        rc.right - rc.left, 150,
        hParent, (HMENU)ID_OUTPUT,
        GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hOutput, WM_SETFONT, (WPARAM)g_hCodeFont, TRUE);

    EditorApplyTheme();
    EditorNewFile();
}

void EditorResize(RECT rc) {
    int tabH = 26;
    int outH = 150;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    if (g_hTabBar)
        SetWindowPos(g_hTabBar, nullptr, rc.left, rc.top, w, tabH,
            SWP_NOZORDER | SWP_NOACTIVATE);

    int edH = h - tabH - outH - 4; /* 4px splitter */
    if (edH < 0) edH = 0;
    if (g_hEditor)
        SetWindowPos(g_hEditor, nullptr, rc.left, rc.top + tabH, w, edH,
            SWP_NOZORDER | SWP_NOACTIVATE);

    if (g_hOutput)
        SetWindowPos(g_hOutput, nullptr, rc.left, rc.top + tabH + edH + 4, w, outH,
            SWP_NOZORDER | SWP_NOACTIVATE);
}

void EditorApplyTheme() {
    if (!g_hEditor) return;
    SendMessageW(g_hEditor, EM_SETBKGNDCOLOR, 0, (LPARAM)g_theme.bg_editor);

    CHARFORMAT2W cf = {};
    cf.cbSize      = sizeof(cf);
    cf.dwMask      = CFM_COLOR | CFM_BACKCOLOR;
    cf.crTextColor = g_theme.fg_default;
    cf.crBackColor = g_theme.bg_editor;
    cf.dwEffects   = 0;
    SendMessageW(g_hEditor, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

    if (g_hOutput) {
        SendMessageW(g_hOutput, EM_SETBKGNDCOLOR, 0, (LPARAM)g_theme.bg_panel);
        CHARFORMAT2W cfo = {};
        cfo.cbSize      = sizeof(cfo);
        cfo.dwMask      = CFM_COLOR;
        cfo.crTextColor = g_theme.fg_default;
        cfo.dwEffects   = 0;
        SendMessageW(g_hOutput, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cfo);
    }
    EditorHighlight();
}

/* -------------------------------------------------------------------------
 * File operations
 * ---------------------------------------------------------------------- */
void EditorNewFile() {
    EditorTab tab;
    tab.filepath = L"";
    tab.title    = L"Untitled.nova";
    tab.modified = false;
    tab.scroll_pos = 0;
    tab.caret_pos  = 0;
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
    HANDLE hf = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) return false;

    DWORD sz = GetFileSize(hf, nullptr);
    std::string raw(sz, '\0');
    DWORD rd;
    ReadFile(hf, &raw[0], sz, &rd, nullptr);
    CloseHandle(hf);

    /* UTF-8 or ANSI → wchar */
    int wlen = MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, nullptr, 0);
    std::wstring wtext(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, &wtext[0], wlen);

    /* Add tab */
    EditorTab tab;
    tab.filepath = path;
    wchar_t name[MAX_PATH];
    wcscpy_s(name, path);
    PathStripPathW(name);
    tab.title    = name;
    tab.modified = false;
    g_tabs.push_back(tab);
    g_activeTab = (int)g_tabs.size() - 1;
    TabAddItem(tab.title.c_str());

    SetWindowTextW(g_hEditor, wtext.c_str());
    EditorHighlight();
    EditorUpdateTitle();
    return true;
}

bool EditorSaveFile() {
    if (g_activeTab < 0) return false;
    auto &tab = g_tabs[g_activeTab];
    if (tab.filepath.empty()) return EditorSaveFileAs();

    int len = GetWindowTextLengthW(g_hEditor);
    std::wstring wtext(len + 1, L'\0');
    GetWindowTextW(g_hEditor, &wtext[0], len + 1);
    wtext.resize(len);

    /* Convert to UTF-8 */
    int mblen = WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string raw(mblen, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, &raw[0], mblen, nullptr, nullptr);

    HANDLE hf = CreateFileW(tab.filepath.c_str(), GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) return false;
    DWORD written;
    WriteFile(hf, raw.c_str(), (DWORD)strlen(raw.c_str()), &written, nullptr);
    CloseHandle(hf);

    tab.modified = false;
    TabUpdateTitle(g_activeTab, tab.title.c_str());
    EditorUpdateTitle();
    return true;
}

bool EditorSaveFileAs() {
    if (g_activeTab < 0) return false;
    wchar_t path[MAX_PATH] = {};
    OPENFILENAMEW ofn = {};
    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = g_hMain;
    ofn.lpstrFilter  = L"NovaLang Files\0*.nova;*.nlf;*.nui\0All Files\0*.*\0";
    ofn.lpstrFile    = path;
    ofn.nMaxFile     = MAX_PATH;
    ofn.lpstrDefExt  = L"nova";
    ofn.Flags        = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&ofn)) return false;

    g_tabs[g_activeTab].filepath = path;
    wchar_t name[MAX_PATH];
    wcscpy_s(name, path);
    PathStripPathW(name);
    g_tabs[g_activeTab].title = name;
    return EditorSaveFile();
}

void EditorUpdateTitle() {
    if (g_activeTab < 0) {
        SetWindowTextW(g_hMain, STUDIO_NAME);
        return;
    }
    auto &tab = g_tabs[g_activeTab];
    std::wstring title = tab.title;
    if (tab.modified) title += L" ●";
    title += L" – " STUDIO_NAME;
    SetWindowTextW(g_hMain, title.c_str());
}

void EditorOutput(const wchar_t *text) {
    if (!g_hOutput) return;
    int len = GetWindowTextLengthW(g_hOutput);
    SendMessageW(g_hOutput, EM_SETSEL, len, len);
    SendMessageW(g_hOutput, EM_REPLACESEL, FALSE, (LPARAM)text);
    SendMessageW(g_hOutput, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
    SendMessageW(g_hOutput, EM_SCROLL, SB_BOTTOM, 0);
}

void EditorOutputClear() {
    if (g_hOutput) SetWindowTextW(g_hOutput, L"");
}

void EditorSetZoom(int percent) {
    g_zoomPercent = percent;
    /* RichEdit zoom: ratio numerator/denominator */
    if (g_hEditor)
        SendMessageW(g_hEditor, EM_SETZOOM, percent, 100);
}
