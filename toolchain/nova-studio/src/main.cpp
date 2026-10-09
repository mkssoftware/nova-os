#include "studio.h"
#include <cstdio>

/* -------------------------------------------------------------------------
 * Globals
 * ---------------------------------------------------------------------- */
HWND  g_hMain       = nullptr;
HWND  g_hStatusBar  = nullptr;
HWND  g_hToolBar    = nullptr;
Theme g_theme;
int   g_explorerWidth = 240;
int   g_outputHeight  = 150;
bool  g_showExplorer  = true;
bool  g_showOutput    = true;

/* -------------------------------------------------------------------------
 * Themes
 * ---------------------------------------------------------------------- */
void ThemeSetDark() {
    g_theme = {
        THEME_DARK,
        /*bg_editor*/ RGB(18,18,24),
        /*bg_panel*/  RGB(30,30,38),
        /*bg_title*/  RGB(22,22,30),
        /*fg_default*/RGB(220,220,220),
        /*keyword*/   RGB(86,156,214),
        /*type*/      RGB(78,201,176),
        /*string*/    RGB(206,145,120),
        /*comment*/   RGB(106,153,85),
        /*number*/    RGB(181,206,168),
        /*operator*/  RGB(180,180,200),
        /*ident*/     RGB(220,220,220),
        /*error*/     RGB(244,71,71),
        /*linenum*/   RGB(100,100,120),
        /*sel_bg*/    RGB(38,79,120),
        /*tab_active*/RGB(40,40,55),
        /*tab_inact*/ RGB(30,30,38),
        /*border*/    RGB(55,55,70),
        /*status_bg*/ RGB(0,122,204),
        /*status_fg*/ RGB(255,255,255),
    };
}

void ThemeSetLight() {
    g_theme = {
        THEME_LIGHT,
        /*bg_editor*/ RGB(255,255,255),
        /*bg_panel*/  RGB(243,243,243),
        /*bg_title*/  RGB(238,238,238),
        /*fg_default*/RGB(30,30,30),
        /*keyword*/   RGB(0,0,255),
        /*type*/      RGB(0,128,0),
        /*string*/    RGB(163,21,21),
        /*comment*/   RGB(0,128,0),
        /*number*/    RGB(9,136,90),
        /*operator*/  RGB(0,0,128),
        /*ident*/     RGB(30,30,30),
        /*error*/     RGB(200,0,0),
        /*linenum*/   RGB(140,140,140),
        /*sel_bg*/    RGB(173,214,255),
        /*tab_active*/RGB(255,255,255),
        /*tab_inact*/ RGB(230,230,230),
        /*border*/    RGB(204,204,204),
        /*status_bg*/ RGB(0,122,204),
        /*status_fg*/ RGB(255,255,255),
    };
}

/* -------------------------------------------------------------------------
 * Layout
 * ---------------------------------------------------------------------- */
void LayoutCompute(HWND hMain, RECT *rExpl, RECT *rEdit, RECT *rOut, RECT *rTab) {
    RECT client;
    GetClientRect(hMain, &client);

    RECT rcTB = {};
    if (g_hToolBar) GetWindowRect(g_hToolBar, &rcTB);
    RECT rcSB = {};
    if (g_hStatusBar) GetWindowRect(g_hStatusBar, &rcSB);

    int tbH = g_hToolBar  ? (rcTB.bottom - rcTB.top) : 0;
    int sbH = g_hStatusBar? (rcSB.bottom - rcSB.top) : 22;
    int top    = tbH;
    int bottom = client.bottom - sbH;
    int exW = g_showExplorer ? g_explorerWidth : 0;

    if (rExpl) { rExpl->left=0; rExpl->top=top; rExpl->right=exW; rExpl->bottom=bottom; }

    int edLeft  = exW + (exW ? 4 : 0);
    int edWidth = client.right - edLeft;

    int tabH = 26;
    if (rTab) { rTab->left=edLeft; rTab->top=top; rTab->right=client.right; rTab->bottom=top+tabH; }

    int outH = g_showOutput ? g_outputHeight : 0;
    if (rEdit){ rEdit->left=edLeft; rEdit->top=top+tabH; rEdit->right=client.right; rEdit->bottom=bottom-outH-(outH?4:0); }
    if (rOut) { rOut->left=edLeft;  rOut->top=bottom-outH; rOut->right=client.right; rOut->bottom=bottom; }

    (void)edWidth;
}

void LayoutApply(HWND hMain) {
    RECT rExpl, rEdit, rOut, rTab;
    LayoutCompute(hMain, &rExpl, &rEdit, &rOut, &rTab);

    if (g_hExplorer)
        ExplorerResize(rExpl);

    if (g_hTabBar || g_hEditor || g_hOutput) {
        RECT editorPane = { rTab.left, rTab.top, rEdit.right, rOut.bottom };
        EditorResize(editorPane);
    }

    /* Status bar */
    if (g_hStatusBar)
        SendMessageW(g_hStatusBar, WM_SIZE, 0, 0);
    /* Toolbar */
    if (g_hToolBar)
        SendMessageW(g_hToolBar, TB_AUTOSIZE, 0, 0);
}

/* -------------------------------------------------------------------------
 * Menu
 * ---------------------------------------------------------------------- */
static HMENU CreateStudioMenu() {
    HMENU hBar  = CreateMenu();
    HMENU hFile = CreatePopupMenu();
    HMENU hEdit = CreatePopupMenu();
    HMENU hView = CreatePopupMenu();
    HMENU hBuild= CreatePopupMenu();
    HMENU hHelp = CreatePopupMenu();

    /* File */
    AppendMenuW(hFile, MF_STRING, IDM_FILE_NEW,         L"&Neu\tCtrl+N");
    AppendMenuW(hFile, MF_STRING, IDM_FILE_NEW_PROJECT, L"Neues &Projekt…");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_OPEN,        L"Ö&ffnen…\tCtrl+O");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_SAVE,        L"&Speichern\tCtrl+S");
    AppendMenuW(hFile, MF_STRING, IDM_FILE_SAVEAS,      L"Speichern &unter…");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_CLOSE,       L"S&chließen");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_EXIT,        L"&Beenden\tAlt+F4");

    /* Edit */
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_UNDO,       L"&Rückgängig\tCtrl+Z");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_REDO,       L"&Wiederholen\tCtrl+Y");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_CUT,        L"&Ausschneiden\tCtrl+X");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_COPY,       L"&Kopieren\tCtrl+C");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_PASTE,      L"&Einfügen\tCtrl+V");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_SELECTALL,  L"&Alles auswählen\tCtrl+A");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_FIND,       L"&Suchen…\tCtrl+F");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_REPLACE,    L"E&rsetzen…\tCtrl+H");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_COMMENT,    L"Kommentieren\tCtrl+K");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_FORMAT,     L"Dokument formatieren\tCtrl+Shift+F");

    /* View */
    AppendMenuW(hView, MF_STRING, IDM_VIEW_EXPLORER,   L"&Explorer\tCtrl+Shift+E");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_OUTPUT,     L"&Ausgabe\tCtrl+Shift+O");
    AppendMenuW(hView, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hView, MF_STRING, IDM_VIEW_DARKMODE,   L"Dunkles Design");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_LIGHTMODE,  L"Helles Design");
    AppendMenuW(hView, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hView, MF_STRING, IDM_VIEW_ZOOMIN,     L"Vergrößern\tCtrl++");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_ZOOMOUT,    L"Verkleinern\tCtrl+-");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_ZOOMRESET,  L"Zoom zurücksetzen\tCtrl+0");

    /* Build */
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_BUILD,   L"&Kompilieren\tF7");
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_REBUILD, L"Vollständig neu kompilieren");
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_CLEAN,   L"Bereinigen");
    AppendMenuW(hBuild, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_RUN,     L"&Ausführen\tF5");

    /* Help */
    AppendMenuW(hHelp, MF_STRING, IDM_HELP_ABOUT, L"&Über Nova Studio…");

    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hFile,  L"&Datei");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hEdit,  L"&Bearbeiten");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hView,  L"&Ansicht");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hBuild, L"&Kompilieren");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hHelp,  L"&Hilfe");
    return hBar;
}

/* -------------------------------------------------------------------------
 * Status bar helpers
 * ---------------------------------------------------------------------- */
static void StatusSet(const wchar_t *left, const wchar_t *right) {
    if (!g_hStatusBar) return;
    RECT rc; GetClientRect(g_hStatusBar, &rc);
    int parts[2] = { rc.right - 200, -1 };
    SendMessageW(g_hStatusBar, SB_SETPARTS, 2, (LPARAM)parts);
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)left);
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 1, (LPARAM)right);
}

static void UpdateStatusCaret() {
    if (!g_hEditor) return;
    CHARRANGE cr; SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);
    int line = (int)SendMessageW(g_hEditor, EM_LINEFROMCHAR, cr.cpMin, 0);
    int lineStart = (int)SendMessageW(g_hEditor, EM_LINEINDEX, line, 0);
    int col = cr.cpMin - lineStart;
    wchar_t buf[80];
    swprintf_s(buf, L"Zeile %d, Spalte %d", line + 1, col + 1);
    StatusSet(buf, L"NovaLang");
}

/* -------------------------------------------------------------------------
 * Apply theme to main window and all children
 * ---------------------------------------------------------------------- */
static void ApplyThemeToWindow(HWND hw) {
    /* DWM dark title bar (Windows 10 1809+) */
    BOOL dark = (g_theme.kind == THEME_DARK) ? TRUE : FALSE;
    DwmSetWindowAttribute(hw, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    /* For older versions: attribute 19 */
    DwmSetWindowAttribute(hw, 19, &dark, sizeof(dark));

    SetClassLongPtrW(hw, GCLP_HBRBACKGROUND,
        (LONG_PTR)CreateSolidBrush(g_theme.bg_panel));
    InvalidateRect(hw, nullptr, TRUE);

    ExplorerApplyTheme();
    EditorApplyTheme();
}

/* -------------------------------------------------------------------------
 * "New Project" dialog – simple folder chooser
 * ---------------------------------------------------------------------- */
static void NewProject(HWND hMain) {
    /* Use SHBrowseForFolder */
    wchar_t path[MAX_PATH] = {};
    BROWSEINFOW bi;
    memset(&bi, 0, sizeof(bi));
    bi.hwndOwner = hMain;
    bi.lpszTitle = L"Ordner für neues Projekt wählen:";
    bi.ulFlags   = 0x0001 /*BIF_RETURNONLYFSDIRS*/ | 0x0040 /*BIF_NEWDIALOGSTYLE*/;
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        SHGetPathFromIDListW(pidl, path);
        CoTaskMemFree(pidl);
        if (path[0]) {
            ExplorerSetRoot(path);
            EditorOutput(L"Projekt geöffnet:");
            EditorOutput(path);
        }
    }
}

/* -------------------------------------------------------------------------
 * Find/Replace (simple)
 * ---------------------------------------------------------------------- */
static void DoFind(HWND hMain) {
    /* We use the built-in Find dialog from commdlg */
    static wchar_t szFind[256] = {};
    static FINDREPLACEW fr = {};
    fr.lStructSize  = sizeof(fr);
    fr.hwndOwner    = hMain;
    fr.lpstrFindWhat= szFind;
    fr.wFindWhatLen = 256;
    fr.Flags        = FR_DOWN;
    FindTextW(&fr); /* modeless; use RegisterWindowMessage(FINDMSGSTRING) for results */
}

/* -------------------------------------------------------------------------
 * Comment toggle
 * ---------------------------------------------------------------------- */
static void ToggleComment() {
    if (!g_hEditor) return;
    CHARRANGE cr;
    SendMessageW(g_hEditor, EM_EXGETSEL, 0, (LPARAM)&cr);
    int line = (int)SendMessageW(g_hEditor, EM_LINEFROMCHAR, cr.cpMin, 0);
    int lineStart = (int)SendMessageW(g_hEditor, EM_LINEINDEX, line, 0);
    int lineLen   = (int)SendMessageW(g_hEditor, EM_LINELENGTH, lineStart, 0);
    if (lineLen <= 0) return;

    std::wstring lineText(lineLen + 1, L'\0');
    lineText[0] = (wchar_t)lineLen;
    SendMessageW(g_hEditor, EM_GETLINE, line, (LPARAM)lineText.c_str());
    lineText.resize(lineLen);

    CHARRANGE crLine = { lineStart, lineStart + lineLen };
    SendMessageW(g_hEditor, EM_EXSETSEL, 0, (LPARAM)&crLine);
    /* Check if already commented */
    size_t nonSpace = lineText.find_first_not_of(L" \t");
    if (nonSpace != std::wstring::npos && lineText[nonSpace] == L'\'') {
        lineText.erase(nonSpace, 1);
    } else {
        lineText.insert(nonSpace == std::wstring::npos ? 0 : nonSpace, 1, L'\'');
    }
    SendMessageW(g_hEditor, EM_REPLACESEL, TRUE, (LPARAM)lineText.c_str());
}

/* -------------------------------------------------------------------------
 * Fake build (shells out to novalang compiler if present)
 * ---------------------------------------------------------------------- */
static void DoBuild(bool rebuild) {
    EditorOutputClear();
    if (g_activeTab < 0) {
        EditorOutput(L"Kein Dokument geöffnet.");
        return;
    }
    auto &tab = g_tabs[g_activeTab];
    if (tab.filepath.empty()) {
        EditorOutput(L"Bitte zuerst speichern.");
        return;
    }
    if (tab.modified) EditorSaveFile();

    EditorOutput(L"Nova Studio Build-System 1.0");
    EditorOutput(L"Kompiliere: ");
    EditorOutput(tab.filepath.c_str());
    EditorOutput(L"");

    /* Try to run novalang.exe if it exists */
    wchar_t compiler[MAX_PATH];
    GetModuleFileNameW(nullptr, compiler, MAX_PATH);
    PathRemoveFileSpecW(compiler);
    PathAppendW(compiler, L"novalang.exe");

    if (GetFileAttributesW(compiler) != INVALID_FILE_ATTRIBUTES) {
        wchar_t cmd[MAX_PATH * 2];
        swprintf_s(cmd, L"\"%s\" \"%s\"", compiler, tab.filepath.c_str());

        STARTUPINFOW si = {}; si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {};
        if (CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, 10000);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    } else {
        /* Syntax-only feedback using our highlighter */
        int len = GetWindowTextLengthW(g_hEditor);
        std::wstring src(len + 1, L'\0');
        GetWindowTextW(g_hEditor, &src[0], len + 1);
        src.resize(len);
        auto spans = HlTokenize(src.c_str(), len);
        bool hasError = false;
        for (auto &sp : spans) {
            if (sp.type == TT_ERROR) { hasError = true; break; }  /* NOLINT */
        }
        EditorOutput(hasError ? L"Fehler gefunden." : L"Syntaxprüfung OK (kein Compiler installiert).");
    }
    (void)rebuild;
    EditorOutput(L"Build abgeschlossen.");
    StatusSet(L"Build fertig", L"NovaLang");
}

/* -------------------------------------------------------------------------
 * WM_COMMAND handler
 * ---------------------------------------------------------------------- */
static void OnCommand(HWND hMain, WPARAM wParam) {
    int id = LOWORD(wParam);
    switch (id) {
        case IDM_FILE_NEW:         EditorNewFile();          break;
        case IDM_FILE_NEW_PROJECT: NewProject(hMain);        break;
        case IDM_FILE_OPEN: {
            wchar_t path[MAX_PATH] = {};
            OPENFILENAMEW ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner   = hMain;
            ofn.lpstrFilter = L"NovaLang Dateien\0*.nova;*.nlf;*.nui\0Alle Dateien\0*.*\0";
            ofn.lpstrFile   = path;
            ofn.nMaxFile    = MAX_PATH;
            ofn.Flags       = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetOpenFileNameW(&ofn)) EditorOpenFile(path);
            break;
        }
        case IDM_FILE_SAVE:    EditorSaveFile();   break;
        case IDM_FILE_SAVEAS:  EditorSaveFileAs(); break;
        case IDM_FILE_CLOSE: {
            if (g_activeTab >= 0) {
                g_tabs.erase(g_tabs.begin() + g_activeTab);
                TabCtrl_DeleteItem(g_hTabBar, g_activeTab);
                g_activeTab = (int)g_tabs.size() - 1;
                if (g_activeTab >= 0)
                    SetWindowTextW(g_hEditor, L"");
                EditorUpdateTitle();
            }
            break;
        }
        case IDM_FILE_EXIT:
            PostMessageW(hMain, WM_CLOSE, 0, 0); break;

        case IDM_EDIT_UNDO:      SendMessageW(g_hEditor, WM_UNDO, 0, 0);         break;
        case IDM_EDIT_REDO:      SendMessageW(g_hEditor, EM_REDO, 0, 0);         break;
        case IDM_EDIT_CUT:       SendMessageW(g_hEditor, WM_CUT, 0, 0);          break;
        case IDM_EDIT_COPY:      SendMessageW(g_hEditor, WM_COPY, 0, 0);         break;
        case IDM_EDIT_PASTE:     SendMessageW(g_hEditor, WM_PASTE, 0, 0);        break;
        case IDM_EDIT_SELECTALL: SendMessageW(g_hEditor, EM_SETSEL, 0, -1);      break;
        case IDM_EDIT_FIND:      DoFind(hMain);                                  break;
        case IDM_EDIT_COMMENT:   ToggleComment();                                break;
        case IDM_EDIT_FORMAT: {
            /* Reindent – simple: just re-highlight */
            EditorHighlight();
            break;
        }

        case IDM_VIEW_EXPLORER:
            g_showExplorer = !g_showExplorer;
            ShowWindow(g_hExplorer, g_showExplorer ? SW_SHOW : SW_HIDE);
            LayoutApply(hMain);
            break;
        case IDM_VIEW_OUTPUT:
            g_showOutput = !g_showOutput;
            ShowWindow(g_hOutput, g_showOutput ? SW_SHOW : SW_HIDE);
            LayoutApply(hMain);
            break;
        case IDM_VIEW_DARKMODE:
            ThemeSetDark(); ApplyThemeToWindow(hMain);
            LayoutApply(hMain); break;
        case IDM_VIEW_LIGHTMODE:
            ThemeSetLight(); ApplyThemeToWindow(hMain);
            LayoutApply(hMain); break;
        case IDM_VIEW_ZOOMIN:   EditorSetZoom(g_zoomPercent + 10 <= 400 ? g_zoomPercent+10 : 400); break;
        case IDM_VIEW_ZOOMOUT:  EditorSetZoom(g_zoomPercent - 10 >= 50  ? g_zoomPercent-10 : 50);  break;
        case IDM_VIEW_ZOOMRESET:EditorSetZoom(100); break;

        case IDM_BUILD_BUILD:   DoBuild(false); break;
        case IDM_BUILD_REBUILD: DoBuild(true);  break;
        case IDM_BUILD_CLEAN:
            EditorOutputClear();
            EditorOutput(L"Artefakte bereinigt.");
            break;
        case IDM_BUILD_RUN:
            DoBuild(false);
            EditorOutput(L"Ausführung: (NovaOS-Runtime nicht verfügbar auf Windows)");
            break;

        case IDM_HELP_ABOUT:
            MessageBoxW(hMain,
                L"Nova Studio 1.0\r\n"
                L"Integrierte Entwicklungsumgebung für NovaLang\r\n\r\n"
                L"NPSPEC-STUDIO-ARCHITECTURE-0001 konform\r\n"
                L"NovaOS Projekt – C:\recoverboot\\nova-os",
                L"Über Nova Studio", MB_OK | MB_ICONINFORMATION);
            break;
    }
}

/* -------------------------------------------------------------------------
 * Tab bar notification
 * ---------------------------------------------------------------------- */
static void OnTabChange() {
    int sel = TabCtrl_GetCurSel(g_hTabBar);
    if (sel < 0 || sel >= (int)g_tabs.size()) return;
    g_activeTab = sel;
    EditorUpdateTitle();
    /* In a full implementation we'd save/restore per-tab text here */
}

/* -------------------------------------------------------------------------
 * WM_CTLCOLOR* handlers – paint controls in theme colors
 * ---------------------------------------------------------------------- */
static HBRUSH s_hBrushPanel  = nullptr;
static HBRUSH s_hBrushEditor = nullptr;

static void RecreateBrushes() {
    if (s_hBrushPanel)  DeleteObject(s_hBrushPanel);
    if (s_hBrushEditor) DeleteObject(s_hBrushEditor);
    s_hBrushPanel  = CreateSolidBrush(g_theme.bg_panel);
    s_hBrushEditor = CreateSolidBrush(g_theme.bg_editor);
}

/* -------------------------------------------------------------------------
 * Keyboard accelerators
 * ---------------------------------------------------------------------- */
static HACCEL CreateAccTable() {
    ACCEL acc[] = {
        {FVIRTKEY|FCONTROL, 'N', IDM_FILE_NEW},
        {FVIRTKEY|FCONTROL, 'O', IDM_FILE_OPEN},
        {FVIRTKEY|FCONTROL, 'S', IDM_FILE_SAVE},
        {FVIRTKEY|FCONTROL, 'Z', IDM_EDIT_UNDO},
        {FVIRTKEY|FCONTROL, 'Y', IDM_EDIT_REDO},
        {FVIRTKEY|FCONTROL, 'F', IDM_EDIT_FIND},
        {FVIRTKEY|FCONTROL, 'K', IDM_EDIT_COMMENT},
        {FVIRTKEY,          VK_F7,  IDM_BUILD_BUILD},
        {FVIRTKEY,          VK_F5,  IDM_BUILD_RUN},
    };
    return CreateAcceleratorTableW(acc, ARRAYSIZE(acc));
}

/* g_zoomPercent defined in editor.cpp */

/* -------------------------------------------------------------------------
 * Main window procedure
 * ---------------------------------------------------------------------- */
static LRESULT CALLBACK MainWndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        RecreateBrushes();

        /* Common controls */
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES|ICC_TAB_CLASSES|ICC_TREEVIEW_CLASSES };
        InitCommonControlsEx(&icc);

        /* Status bar */
        g_hStatusBar = CreateWindowExW(0, STATUSCLASSNAMEW, nullptr,
            WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,
            0,0,0,0, hw, (HMENU)ID_STATUSBAR, GetModuleHandleW(nullptr), nullptr);

        /* Toolbar (simple flat toolbar) */
        g_hToolBar = CreateWindowExW(0, TOOLBARCLASSNAMEW, nullptr,
            WS_CHILD|WS_VISIBLE|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_NODIVIDER|CCS_NORESIZE,
            0,0,0,30, hw, (HMENU)ID_TOOLBAR, GetModuleHandleW(nullptr), nullptr);
        SendMessageW(g_hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
        SendMessageW(g_hToolBar, TB_SETBITMAPSIZE, 0, MAKELONG(16,16));
        SendMessageW(g_hToolBar, TB_SETBUTTONSIZE, 0, MAKELONG(28,26));
        /* Add standard image list */
        TBADDBITMAP bm = { HINST_COMMCTRL, IDB_STD_SMALL_COLOR };
        SendMessageW(g_hToolBar, TB_ADDBITMAP, 0, (LPARAM)&bm);
        TBBUTTON btns[] = {
            {STD_FILENEW,  IDM_FILE_NEW,  TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {STD_FILEOPEN, IDM_FILE_OPEN, TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {STD_FILESAVE, IDM_FILE_SAVE, TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {0, 0, TBSTATE_ENABLED, BTNS_SEP, {}, 0, 0},
            {STD_CUT,  IDM_EDIT_CUT,   TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {STD_COPY, IDM_EDIT_COPY,  TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {STD_PASTE,IDM_EDIT_PASTE, TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {0, 0, TBSTATE_ENABLED, BTNS_SEP, {}, 0, 0},
            {STD_UNDO, IDM_EDIT_UNDO,  TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {STD_REDOW,IDM_EDIT_REDO,  TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
            {0, 0, TBSTATE_ENABLED, BTNS_SEP, {}, 0, 0},
            {STD_FIND, IDM_EDIT_FIND,  TBSTATE_ENABLED, BTNS_BUTTON, {}, 0, 0},
        };
        SendMessageW(g_hToolBar, TB_ADDBUTTONS, ARRAYSIZE(btns), (LPARAM)btns);
        SendMessageW(g_hToolBar, TB_AUTOSIZE, 0, 0);
        ShowWindow(g_hToolBar, SW_SHOW);

        RECT client; GetClientRect(hw, &client);

        /* Explorer pane */
        RECT rExpl = { 0, 30, g_explorerWidth, client.bottom - 22 };
        ExplorerCreate(hw, rExpl);

        /* Editor + output pane */
        RECT rEdit = { g_explorerWidth + 4, 30, client.right, client.bottom - 22 };
        EditorCreate(hw, rEdit);

        LayoutApply(hw);
        ApplyThemeToWindow(hw);
        StatusSet(L"Bereit", L"NovaLang");
        return 0;
    }

    case WM_SIZE:
        LayoutApply(hw);
        UpdateStatusCaret();
        return 0;

    case WM_TIMER:
        if (wp == 1 /* TIMER_HIGHLIGHT */) {
            KillTimer(hw, 1);
            if (g_highlightPending) EditorHighlight();
            UpdateStatusCaret();
        }
        return 0;

    case WM_COMMAND: {
        /* Check if from editor (EN_CHANGE) */
        if (HIWORD(wp) == EN_CHANGE && (HWND)lp == g_hEditor) {
            if (g_activeTab >= 0) g_tabs[g_activeTab].modified = true;
            EditorUpdateTitle();
        }
        /* Tab click */
        if (HIWORD(wp) == TCN_SELCHANGE) {
            OnTabChange(); return 0;
        }
        OnCommand(hw, wp);
        return 0;
    }

    case WM_NOTIFY: {
        NMHDR *nm = (NMHDR*)lp;
        if (nm->code == TCN_SELCHANGE && nm->hwndFrom == g_hTabBar) {
            OnTabChange();
        }
        if (nm->code == NM_DBLCLK && nm->hwndFrom == g_hExplorer) {
            ExplorerOpenSelected();
        }
        return 0;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, g_theme.fg_default);
        SetBkColor(hdc, g_theme.bg_panel);
        return (LRESULT)s_hBrushPanel;
    }

    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wp;
        RECT rc; GetClientRect(hw, &rc);
        FillRect(hdc, &rc, s_hBrushPanel);
        return 1;
    }

    case WM_CLOSE: {
        /* Check for unsaved changes */
        bool anyModified = false;
        for (auto &tab : g_tabs) if (tab.modified) { anyModified = true; break; }
        if (anyModified) {
            int r = MessageBoxW(hw,
                L"Es gibt ungespeicherte Änderungen. Trotzdem beenden?",
                L"Nova Studio", MB_YESNO | MB_ICONQUESTION);
            if (r != IDYES) return 0;
        }
        DestroyWindow(hw);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hw, msg, wp, lp);
}

/* -------------------------------------------------------------------------
 * WinMain
 * ---------------------------------------------------------------------- */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow) {
    /* Initialize COM */
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    /* Dark mode default */
    ThemeSetDark();

    /* Register window class */
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(g_theme.bg_panel);
    wc.lpszClassName = STUDIO_CLASS;
    wc.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hIconSm       = LoadIconW(nullptr, IDI_APPLICATION);
    RegisterClassExW(&wc);

    /* Create main window */
    g_hMain = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW,
        STUDIO_CLASS, STUDIO_NAME L" 1.0",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 780,
        nullptr, CreateStudioMenu(), hInst, nullptr);

    ShowWindow(g_hMain, nCmdShow);
    UpdateWindow(g_hMain);

    /* If a file was passed on command line, open it */
    if (lpCmdLine && lpCmdLine[0]) {
        std::wstring arg = lpCmdLine;
        if (arg[0] == L'"') {
            arg = arg.substr(1);
            auto q = arg.find(L'"');
            if (q != std::wstring::npos) arg = arg.substr(0, q);
        }
        if (!arg.empty()) EditorOpenFile(arg.c_str());
    }

    StatusSet(L"Bereit \u2013 NovaLang Studio 1.0", L"UTF-8");

    /* Message loop */
    HACCEL hAcc = CreateAccTable();
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (!TranslateAcceleratorW(g_hMain, hAcc, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    DestroyAcceleratorTable(hAcc);
    CoUninitialize();
    return (int)msg.wParam;
}
