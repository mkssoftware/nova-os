#include "studio.h"

/* -----------------------------------------------------------------------
 * Globals defined here
 * -------------------------------------------------------------------- */
HWND g_hEditor  = nullptr;
HWND g_hTabBar  = nullptr;
HWND g_hOutput  = nullptr;
HFONT g_hCodeFont  = nullptr;
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
 * Create editor pane
 * -------------------------------------------------------------------- */
void EditorCreate(HWND hParent, RECT rc) {
    /* Tab bar */
    g_hTabBar = CreateWindowExW(0, WC_TABCONTROLW, nullptr,
        WS_CHILD | WS_VISIBLE | TCS_FLATBUTTONS | TCS_FOCUSNEVER,
        rc.left, rc.top, rc.right - rc.left, 26,
        hParent, (HMENU)ID_TABBAR, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hTabBar, WM_SETFONT,
                 (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    /* RichEdit (prefer Msftedit) */
    HMODULE hRE = LoadLibraryW(L"Msftedit.dll");
    if (!hRE) hRE = LoadLibraryW(L"Riched20.dll");
    const wchar_t *reClass = hRE ? MSFTEDIT_CLASS : RICHEDIT_CLASSW;

    g_hEditor = CreateWindowExW(WS_EX_CLIENTEDGE, reClass, nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL,
        rc.left, rc.top + 26,
        rc.right - rc.left, rc.bottom - rc.top - 26,
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

    /* Output panel */
    g_hOutput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        rc.left, rc.bottom - g_outputHeight,
        rc.right - rc.left, g_outputHeight,
        hParent, (HMENU)ID_OUTPUT, GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hOutput, WM_SETFONT, (WPARAM)g_hCodeFont, TRUE);

    EditorApplyTheme();
    EditorNewFile();

    /* Register Find/Replace message */
    g_msgFindReplace = RegisterWindowMessageW(FINDMSGSTRINGW);
}

void EditorResize(RECT rc) {
    int tabH = 26;
    int outH = g_showOutput ? g_outputHeight : 0;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    int edH = h - tabH - outH - (outH > 0 ? 4 : 0);
    if (edH < 0) edH = 0;

    if (g_hTabBar)
        SetWindowPos(g_hTabBar, nullptr, rc.left, rc.top, w, tabH, SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hEditor)
        SetWindowPos(g_hEditor, nullptr, rc.left, rc.top + tabH, w, edH, SWP_NOZORDER | SWP_NOACTIVATE);
    if (g_hOutput)
        SetWindowPos(g_hOutput, nullptr, rc.left, rc.top + tabH + edH + (outH > 0 ? 4 : 0), w, outH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    ShowWindow(g_hOutput, g_showOutput ? SW_SHOW : SW_HIDE);
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

/* -----------------------------------------------------------------------
 * File operations
 * -------------------------------------------------------------------- */
void EditorNewFile() {
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
    /* Check if already open */
    for (int i = 0; i < (int)g_tabs.size(); i++) {
        if (_wcsicmp(g_tabs[i].filepath.c_str(), path) == 0) {
            g_activeTab = i;
            TabCtrl_SetCurSel(g_hTabBar, i);
            /* Reload content */
            break;
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

    /* Create new tab */
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
    TabSetTitle(g_activeTab, tab.title.c_str());
    EditorUpdateTitle();
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)L"  Saved");
    SetTimer(g_hMain, TIMER_STATUS, 2000, nullptr);
    return true;
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
    return EditorSaveFile();
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
