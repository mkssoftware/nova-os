#include "studio.h"

/* =========================================================================
 * Globals defined here
 * ====================================================================== */
HWND g_hMain           = nullptr;
HWND g_hExplorer       = nullptr;
HWND g_hStatusBar      = nullptr;
HWND g_hToolBar        = nullptr;
HWND g_hDocOutline     = nullptr;
HWND g_hBottomContainer = nullptr;
HWND g_hBottomTabs     = nullptr;
/* g_hRibbon ist in ribbon.cpp definiert */

Theme g_theme          = {};

int  g_explorerWidth = 240;
int  g_outputHeight  = 170;
int  g_outlineWidth  = 220;
bool g_showExplorer  = true;
bool g_showOutput    = true;
bool g_showOutline   = true;

/* =========================================================================
 * Theme
 * ====================================================================== */
void ThemeSetDark() {
    g_theme.kind      = THEME_DARK;
    /* Abgestufte dunkle Flächen, kein reines Schwarz (NPSPEC-STUDIO-DARK-THEME-0001) */
    g_theme.bg_editor    = RGB(0x1E, 0x1E, 0x2E); /* Catppuccin-like dark */
    g_theme.bg_panel     = RGB(0x16, 0x16, 0x20);
    g_theme.bg_statusbar = RGB(0x12, 0x12, 0x1A);
    g_theme.fg_default   = RGB(0xCD, 0xD6, 0xF4);
    g_theme.fg_keyword   = RGB(0xCB, 0xA6, 0xF7); /* violet – keywords */
    g_theme.fg_type      = RGB(0x89, 0xDC, 0xEB); /* sky – built-in types */
    g_theme.fg_string    = RGB(0xA6, 0xE3, 0xA1); /* green – strings */
    g_theme.fg_comment   = RGB(0x58, 0x5B, 0x70); /* overlay – comments */
    g_theme.fg_number    = RGB(0xFA, 0xB3, 0x87); /* peach – numbers */
    g_theme.fg_operator  = RGB(0x89, 0xB4, 0xFA); /* blue – operators */
    g_theme.fg_ident     = RGB(0xCD, 0xD6, 0xF4);
    g_theme.fg_error     = RGB(0xF3, 0x8B, 0xA8); /* red */
    g_theme.fg_linenum   = RGB(0x6C, 0x70, 0x86);
    g_theme.status_bg    = RGB(0x18, 0x18, 0x26);
    g_theme.status_fg    = RGB(0xA6, 0xAD, 0xC8);
    g_theme.border       = RGB(0x31, 0x32, 0x44);
}

void ThemeSetLight() {
    g_theme.kind      = THEME_LIGHT;
    g_theme.bg_editor    = RGB(0xFF, 0xFF, 0xFF);
    g_theme.bg_panel     = RGB(0xF3, 0xF3, 0xF3);
    g_theme.bg_statusbar = RGB(0xE8, 0xE8, 0xE8);
    g_theme.fg_default   = RGB(0x1F, 0x1F, 0x1F);
    g_theme.fg_keyword   = RGB(0x00, 0x00, 0xFF); /* blue – keywords */
    g_theme.fg_type      = RGB(0x26, 0x7F, 0x99); /* teal – types */
    g_theme.fg_string    = RGB(0xA3, 0x15, 0x15); /* dark red – strings */
    g_theme.fg_comment   = RGB(0x00, 0x8B, 0x00); /* green – comments */
    g_theme.fg_number    = RGB(0x09, 0x88, 0x5A); /* teal-green – numbers */
    g_theme.fg_operator  = RGB(0x00, 0x00, 0x00);
    g_theme.fg_ident     = RGB(0x1F, 0x1F, 0x1F);
    g_theme.fg_error     = RGB(0xCC, 0x00, 0x00);
    g_theme.fg_linenum   = RGB(0x99, 0x99, 0x99);
    g_theme.status_bg    = RGB(0xDD, 0xDD, 0xDD);
    g_theme.status_fg    = RGB(0x33, 0x33, 0x33);
    g_theme.border       = RGB(0xCC, 0xCC, 0xCC);
}

static void ApplyDwmTheme(HWND hw) {
    BOOL dark = (g_theme.kind == THEME_DARK) ? TRUE : FALSE;
    DwmSetWindowAttribute(hw, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
}

/* =========================================================================
 * Status bar
 * ====================================================================== */
static void StatusSetParts(HWND hParent) {
    RECT rc; GetClientRect(hParent, &rc);
    int W = rc.right;
    /* Segmente: Bereit | Zeile/Spalte | Leerzeichen | Kodierung | Zeilenende |
                 Sprache | Vorschau */
    int parts[7];
    parts[0] = 200;
    parts[1] = parts[0] + 140;
    parts[2] = parts[1] + 100;
    parts[3] = parts[2] + 75;
    parts[4] = parts[3] + 65;
    parts[5] = W - 120;
    parts[6] = -1;
    SendMessageW(g_hStatusBar, SB_SETPARTS, 7, (LPARAM)parts);
}

static void StatusCreate(HWND hParent) {
    g_hStatusBar = CreateWindowExW(0, STATUSCLASSNAMEW, nullptr,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        hParent, (HMENU)ID_STATUSBAR, GetModuleHandleW(nullptr), nullptr);

    StatusSetParts(hParent);
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 0, (LPARAM)L"  ● Bereit");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 1, (LPARAM)L"  Zl 1, Sp 1");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 2, (LPARAM)L"  Leerzeichen: 4");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 3, (LPARAM)L"  UTF-8");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 4, (LPARAM)L"  CRLF");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 5, (LPARAM)L"  NovaLang");
    SendMessageW(g_hStatusBar, SB_SETTEXTW, 6, (LPARAM)L"  Vorschau");
}

static void StatusResize(HWND hParent) {
    if (!g_hStatusBar) return;
    StatusSetParts(hParent);
    RECT rc; GetClientRect(hParent, &rc);
    SendMessageW(g_hStatusBar, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
}

/* =========================================================================
 * Menu
 * ====================================================================== */
static HMENU CreateStudioMenu() {
    HMENU hBar  = CreateMenu();

    /* Datei */
    HMENU hFile = CreatePopupMenu();
    AppendMenuW(hFile, MF_STRING, IDM_FILE_NEW,         L"&Neu\tCtrl+N");
    AppendMenuW(hFile, MF_STRING, IDM_FILE_NEW_PROJECT, L"Neues &Projekt...");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_OPEN,        L"&Öffnen...\tCtrl+O");
    AppendMenuW(hFile, MF_STRING, IDM_FILE_OPEN_PROJECT,L"Projekt öffnen...");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_SAVE,        L"&Speichern\tCtrl+S");
    AppendMenuW(hFile, MF_STRING, IDM_FILE_SAVEAS,      L"Speichern &unter...\tCtrl+Shift+S");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_CLOSE_TAB,   L"Tab s&chließen\tCtrl+W");
    AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hFile, MF_STRING, IDM_FILE_EXIT,        L"&Beenden\tAlt+F4");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hFile, L"&Datei");

    /* Bearbeiten */
    HMENU hEdit = CreatePopupMenu();
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_UNDO,      L"&Rückgängig\tCtrl+Z");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_REDO,      L"&Wiederholen\tCtrl+Y");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_CUT,       L"&Ausschneiden\tCtrl+X");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_COPY,      L"&Kopieren\tCtrl+C");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_PASTE,     L"&Einfügen\tCtrl+V");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_SELECTALL, L"&Alles markieren\tCtrl+A");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_FIND,      L"&Suchen...\tCtrl+F");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_FINDNEXT,  L"Weitersuchen\tF3");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_REPLACE,   L"&Ersetzen...\tCtrl+H");
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_GOTO,      L"&Gehe zu Zeile...\tCtrl+G");
    AppendMenuW(hEdit, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hEdit, MF_STRING, IDM_EDIT_COMMENT,   L"&Kommentar umschalten\tCtrl+/");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hEdit, L"&Bearbeiten");

    /* Ansicht */
    HMENU hView = CreatePopupMenu();
    AppendMenuW(hView, MF_STRING, IDM_VIEW_EXPLORER,  L"&Explorer ein/aus\tCtrl+E");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_OUTPUT,    L"&Ausgabe ein/aus\tCtrl+Shift+O");
    AppendMenuW(hView, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hView, MF_STRING, IDM_VIEW_DARKMODE,  L"Dark &Mode");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_LIGHTMODE, L"&Light Mode");
    AppendMenuW(hView, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hView, MF_STRING, IDM_VIEW_ZOOMIN,    L"Zoom &+\tCtrl+=");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_ZOOMOUT,   L"Zoom &-\tCtrl+-");
    AppendMenuW(hView, MF_STRING, IDM_VIEW_ZOOMRESET, L"Zoom &zurücksetzen\tCtrl+0");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hView, L"&Ansicht");

    /* Kompilieren */
    HMENU hBuild = CreatePopupMenu();
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_BUILD,   L"&Erstellen\tF7");
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_REBUILD, L"Neu &erstellen\tCtrl+Shift+B");
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_CLEAN,   L"&Bereinigen");
    AppendMenuW(hBuild, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hBuild, MF_STRING, IDM_BUILD_RUN,     L"&Ausführen\tF5");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hBuild, L"&Kompilieren");

    /* Hilfe */
    HMENU hHelp = CreatePopupMenu();
    AppendMenuW(hHelp, MF_STRING, IDM_HELP_ABOUT, L"&Über Nova Studio...");
    AppendMenuW(hBar, MF_POPUP, (UINT_PTR)hHelp, L"&Hilfe");

    return hBar;
}

/* =========================================================================
 * Layout
 * ====================================================================== */
void LayoutCompute(HWND hw, RECT *rExp, RECT *rEd, RECT *rOut, RECT *rTab) {
    RECT cl;
    GetClientRect(hw, &cl);

    RECT sbrc = {};
    if (g_hStatusBar) GetWindowRect(g_hStatusBar, &sbrc);
    int sbH = sbrc.bottom - sbrc.top;

    int top = RIBBON_HEIGHT;
    int h   = cl.bottom - sbH;
    int w   = cl.right;

    int expW = g_showExplorer ? g_explorerWidth : 0;
    int outW = g_showOutline  ? g_outlineWidth  : 0;
    int outH = g_showOutput   ? g_outputHeight  : 0;

    if (rExp) {
        rExp->left = 0; rExp->top = top;
        rExp->right = expW; rExp->bottom = h;
    }
    if (rEd) {
        rEd->left   = expW;
        rEd->top    = top;
        rEd->right  = w - outW;
        rEd->bottom = h;
    }
    if (rOut) {
        rOut->left   = expW;
        rOut->top    = h - outH;
        rOut->right  = w - outW;
        rOut->bottom = h;
    }
    if (rTab) { *rTab = *rEd; }
}

static void OutlineResize(HWND hw) {
    /* Find the outline container (parent of g_hDocOutline) */
    HWND hOutCont = g_hDocOutline ? GetParent(g_hDocOutline) : nullptr;
    if (!hOutCont) return;
    RECT cl; GetClientRect(hw, &cl);
    RECT sbrc = {};
    if (g_hStatusBar) GetWindowRect(g_hStatusBar, &sbrc);
    int sbH  = sbrc.bottom - sbrc.top;
    int top  = RIBBON_HEIGHT;
    int h    = cl.bottom - sbH;
    int outW = g_showOutline ? g_outlineWidth : 0;
    SetWindowPos(hOutCont, nullptr,
                 cl.right - outW, top, outW, h - top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    ShowWindow(hOutCont, g_showOutline ? SW_SHOW : SW_HIDE);
    SendMessageW(hOutCont, WM_SIZE, 0, MAKELPARAM(outW, h - top));
}

void LayoutApply(HWND hw) {
    RECT cl; GetClientRect(hw, &cl);
    RibbonResize(0, 0, cl.right);

    RECT rExp, rEd, rOut, rTab;
    LayoutCompute(hw, &rExp, &rEd, &rOut, &rTab);

    if (g_hExplorer)
        ExplorerResize(rExp);

    EditorResize(rEd);
    OutlineResize(hw);
}

/* =========================================================================
 * Build system
 * ====================================================================== */
static bool RunCommand(const wchar_t *cmdLine, wchar_t *outBuf, int outBufLen) {
    SECURITY_ATTRIBUTES sa = {};
    sa.nLength              = sizeof(sa);
    sa.bInheritHandle       = TRUE;

    HANDLE hReadPipe, hWritePipe;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) return false;
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si = {};
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput  = hWritePipe;
    si.hStdError   = hWritePipe;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {};
    std::wstring cmd = cmdLine;
    bool ok = CreateProcessW(nullptr, &cmd[0], nullptr, nullptr, TRUE,
                             CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi) != 0;
    CloseHandle(hWritePipe);

    if (!ok) { CloseHandle(hReadPipe); return false; }

    std::string accum;
    char buf[1024];
    DWORD rd;
    while (ReadFile(hReadPipe, buf, sizeof(buf) - 1, &rd, nullptr) && rd > 0) {
        buf[rd] = '\0';
        accum += buf;
    }
    CloseHandle(hReadPipe);
    WaitForSingleObject(pi.hProcess, 5000);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    /* Convert to wchar */
    int wlen = MultiByteToWideChar(CP_ACP, 0, accum.c_str(), -1, nullptr, 0);
    if (wlen > 0 && outBuf && outBufLen > 0)
        MultiByteToWideChar(CP_ACP, 0, accum.c_str(), -1, outBuf, outBufLen);

    return true;
}

void DoBuild(bool rebuild) {
    EditorOutputClear();
    EditorOutput(rebuild ? L"──── Neu erstellen ────" : L"──── Erstellen ────");

    if (g_activeTab < 0 || g_activeTab >= (int)g_tabs.size()) {
        EditorOutput(L"Fehler: Keine Datei geöffnet.");
        return;
    }

    /* Locate novalang.exe next to nova-studio.exe */
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wchar_t nlExe[MAX_PATH];
    PathCombineW(nlExe, exePath, L"novalang.exe");

    const wchar_t *srcFile = g_tabs[g_activeTab].filepath.c_str();
    if (!*srcFile) {
        EditorOutput(L"Fehler: Datei nicht gespeichert.");
        return;
    }

    if (GetFileAttributesW(nlExe) != INVALID_FILE_ATTRIBUTES) {
        /* Real compiler available */
        wchar_t cmdLine[MAX_PATH * 2];
        swprintf_s(cmdLine, L"\"%s\" \"%s\"", nlExe, srcFile);
        if (rebuild)
            wcscat_s(cmdLine, L" --rebuild");

        wchar_t out[8192] = {};
        EditorOutput(L"Aufruf: novalang.exe ...");
        RunCommand(cmdLine, out, 8192);

        /* Print output line by line */
        wchar_t *line = out;
        wchar_t *end;
        while (*line) {
            end = wcschr(line, L'\n');
            if (end) *end = L'\0';
            EditorOutput(line);
            if (!end) break;
            line = end + 1;
        }
        EditorOutput(L"──── Fertig ────");
    } else {
        /* Syntax check via tokenizer */
        HANDLE hf = CreateFileW(srcFile, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hf == INVALID_HANDLE_VALUE) {
            EditorOutput(L"Fehler: Datei konnte nicht gelesen werden.");
            return;
        }
        DWORD sz = GetFileSize(hf, nullptr);
        std::string raw(sz + 1, '\0');
        DWORD rd;
        ReadFile(hf, &raw[0], sz, &rd, nullptr);
        CloseHandle(hf);

        int wlen = MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, nullptr, 0);
        std::wstring wtext(wlen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, &wtext[0], wlen);

        auto spans = HlTokenize(wtext.c_str(), wlen);
        int errors = 0;
        for (auto &s : spans)
            if (s.type == TT_ERROR) errors++;

        wchar_t fname[MAX_PATH];
        wcscpy_s(fname, srcFile);
        PathStripPathW(fname);

        wchar_t buf[256];
        swprintf_s(buf, L"%s: %d Token(s) – %d Fehler", fname,
                   (int)spans.size(), errors);
        EditorOutput(buf);
        EditorOutput(errors == 0 ? L"Syntaxprüfung OK." : L"Syntaxfehler gefunden.");
        EditorOutput(L"[novalang.exe nicht gefunden – nur Syntaxprüfung]");
        EditorOutput(L"──── Fertig ────");
    }

    if (g_showOutput) {
        RECT rExp, rEd, rOut, rTab;
        LayoutCompute(g_hMain, &rExp, &rEd, &rOut, &rTab);
        EditorResize(rEd);
    }
}

void DoRun() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);

    /* Look for built exe next to novalang.exe */
    wchar_t runExe[MAX_PATH] = {};
    if (g_activeTab >= 0 && !g_tabs[g_activeTab].filepath.empty()) {
        wchar_t base[MAX_PATH];
        wcscpy_s(base, g_tabs[g_activeTab].filepath.c_str());
        PathRemoveExtensionW(base);
        wcscat_s(base, L".exe");
        if (GetFileAttributesW(base) != INVALID_FILE_ATTRIBUTES)
            wcscpy_s(runExe, base);
    }
    if (!*runExe) {
        EditorOutput(L"Ausführen: Kein kompiliertes Programm gefunden.");
        EditorOutput(L"Bitte zuerst 'Erstellen' ausführen.");
        return;
    }
    ShellExecuteW(nullptr, L"open", runExe, nullptr, nullptr, SW_SHOW);
    EditorOutput(L"Programm gestartet.");
}

void DoClean() {
    EditorOutputClear();
    EditorOutput(L"──── Bereinigen ────");
    if (g_activeTab >= 0 && !g_tabs[g_activeTab].filepath.empty()) {
        wchar_t base[MAX_PATH];
        wcscpy_s(base, g_tabs[g_activeTab].filepath.c_str());
        PathRemoveExtensionW(base);
        wchar_t exePath[MAX_PATH];
        swprintf_s(exePath, L"%s.exe", base);
        if (DeleteFileW(exePath))
            EditorOutput(L"Artefakte gelöscht.");
        else
            EditorOutput(L"Keine Artefakte gefunden.");
    }
    EditorOutput(L"──── Fertig ────");
}

/* =========================================================================
 * Open project from folder
 * ====================================================================== */
static void OpenProject(const wchar_t *folder) {
    /* Find project.xml */
    wchar_t xmlPath[MAX_PATH];
    PathCombineW(xmlPath, folder, L"project.xml");

    std::wstring name = folder;
    wchar_t baseName[MAX_PATH];
    wcscpy_s(baseName, folder);
    PathStripPathW(baseName);

    /* Try to read Name from project.xml */
    HANDLE hf = CreateFileW(xmlPath, GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf != INVALID_HANDLE_VALUE) {
        char buf[4096] = {};
        DWORD rd;
        ReadFile(hf, buf, sizeof(buf) - 1, &rd, nullptr);
        CloseHandle(hf);
        /* Simple scan for <Name>...</Name> */
        const char *tag = strstr(buf, "<Name>");
        const char *end = tag ? strstr(tag + 6, "</Name>") : nullptr;
        if (tag && end) {
            int len = (int)(end - (tag + 6));
            std::string n(tag + 6, len);
            int wl = MultiByteToWideChar(CP_UTF8, 0, n.c_str(), -1, nullptr, 0);
            std::wstring wn(wl, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, n.c_str(), -1, &wn[0], wl);
            wn.resize(wcslen(wn.c_str()));
            g_project.name = wn;
        } else {
            g_project.name = baseName;
        }
    } else {
        g_project.name = baseName;
    }

    g_project.rootPath = folder;
    g_project.open     = true;
    ExplorerSetRoot(folder);
    EditorUpdateTitle();

    wchar_t msg[MAX_PATH + 32];
    swprintf_s(msg, L"Projekt '%s' geöffnet.", g_project.name.c_str());
    EditorOutput(msg);
}

/* =========================================================================
 * Command dispatcher
 * ====================================================================== */
static void OnCommand(HWND hw, WPARAM wp) {
    switch (LOWORD(wp)) {

    /* ----- Datei ----- */
    case IDM_FILE_NEW:
        EditorNewFile();
        break;

    case IDM_FILE_NEW_PROJECT:
        DoNewProject();
        break;

    case IDM_FILE_OPEN: {
        wchar_t path[MAX_PATH] = {};
        OPENFILENAMEW ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = hw;
        ofn.lpstrFilter = L"NovaLang\0*.nova;*.nlf;*.nui\0XML\0*.xml\0Markdown\0*.md\0Alle Dateien\0*.*\0";
        ofn.lpstrFile   = path;
        ofn.nMaxFile    = MAX_PATH;
        ofn.Flags       = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileNameW(&ofn))
            EditorOpenFile(path);
        break;
    }

    case IDM_FILE_OPEN_PROJECT: {
        BROWSEINFOW bi = {};
        bi.hwndOwner = hw;
        bi.lpszTitle = L"Projektordner öffnen";
        bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pil = SHBrowseForFolderW(&bi);
        if (pil) {
            wchar_t folder[MAX_PATH];
            SHGetPathFromIDListW(pil, folder);
            OpenProject(folder);
            CoTaskMemFree(pil);
        }
        break;
    }

    case IDM_FILE_SAVE:
        EditorSaveFile();
        break;

    case IDM_FILE_SAVEAS:
        EditorSaveFileAs();
        break;

    case IDM_FILE_CLOSE_TAB:
        EditorCloseTab(g_activeTab);
        break;

    case IDM_FILE_EXIT:
        SendMessageW(hw, WM_CLOSE, 0, 0);
        break;

    /* ----- Bearbeiten ----- */
    case IDM_EDIT_UNDO:
        SendMessageW(g_hEditor, WM_UNDO, 0, 0);
        break;

    case IDM_EDIT_REDO:
        SendMessageW(g_hEditor, EM_REDO, 0, 0);
        break;

    case IDM_EDIT_CUT:
        SendMessageW(g_hEditor, WM_CUT, 0, 0);
        break;

    case IDM_EDIT_COPY:
        SendMessageW(g_hEditor, WM_COPY, 0, 0);
        break;

    case IDM_EDIT_PASTE:
        SendMessageW(g_hEditor, WM_PASTE, 0, 0);
        break;

    case IDM_EDIT_SELECTALL:
        SendMessageW(g_hEditor, EM_SETSEL, 0, -1);
        break;

    case IDM_EDIT_FIND:
        DoFind();
        break;

    case IDM_EDIT_FINDNEXT:
        if (g_szFindBuf[0])
            EditorFindNext(g_szFindBuf, g_fr.Flags | FR_DOWN);
        else
            DoFind();
        break;

    case IDM_EDIT_REPLACE:
        DoReplace();
        break;

    case IDM_EDIT_GOTO:
        DoGoToLine();
        break;

    case IDM_EDIT_COMMENT:
        EditorCommentToggle();
        break;

    /* ----- Ansicht ----- */
    case IDM_VIEW_EXPLORER:
        g_showExplorer = !g_showExplorer;
        if (g_hExplorer) ShowWindow(g_hExplorer, g_showExplorer ? SW_SHOW : SW_HIDE);
        LayoutApply(hw);
        break;

    case IDM_VIEW_OUTPUT:
        g_showOutput = !g_showOutput;
        LayoutApply(hw);
        break;

    case IDM_VIEW_DARKMODE:
        ThemeSetDark();
        ApplyDwmTheme(hw);
        EditorApplyTheme();
        ExplorerApplyTheme();
        RibbonApplyTheme();
        InvalidateRect(hw, nullptr, TRUE);
        break;

    case IDM_VIEW_LIGHTMODE:
        ThemeSetLight();
        ApplyDwmTheme(hw);
        EditorApplyTheme();
        ExplorerApplyTheme();
        RibbonApplyTheme();
        InvalidateRect(hw, nullptr, TRUE);
        break;

    case IDM_VIEW_ZOOMIN: {
        int z = g_zoomPercent + 10;
        if (z > 300) z = 300;
        EditorSetZoom(z);
        break;
    }
    case IDM_VIEW_ZOOMOUT: {
        int z = g_zoomPercent - 10;
        if (z < 50) z = 50;
        EditorSetZoom(z);
        break;
    }
    case IDM_VIEW_ZOOMRESET:
        EditorSetZoom(100);
        break;

    case IDM_FILE_SAVE_ALL:
        EditorSaveFile();
        break;

    /* ----- Kompilieren ----- */
    case IDM_BUILD_BUILD:
        if (!g_showOutput) {
            g_showOutput = true;
            LayoutApply(hw);
        }
        DoBuild(false);
        break;

    case IDM_BUILD_REBUILD:
        if (!g_showOutput) {
            g_showOutput = true;
            LayoutApply(hw);
        }
        DoBuild(true);
        break;

    case IDM_BUILD_CLEAN:
        if (!g_showOutput) {
            g_showOutput = true;
            LayoutApply(hw);
        }
        DoClean();
        break;

    case IDM_BUILD_RUN:
        DoRun();
        break;

    /* ----- Debug ----- */
    case IDM_DEBUG_START:
        if (!g_showOutput) { g_showOutput = true; LayoutApply(hw); }
        DoBuild(false);
        DoRun();
        break;

    case IDM_DEBUG_START_NO_DBG:
        DoRun();
        break;

    case IDM_DEBUG_STOP:
        EditorOutput(L"Debug gestoppt.");
        break;

    /* ----- Git ----- */
    case IDM_GIT_COMMIT:
        MessageBoxW(hw, L"Git Commit ist noch nicht implementiert.",
                    L"Git", MB_OK | MB_ICONINFORMATION);
        break;

    case IDM_GIT_PUSH:
        MessageBoxW(hw, L"Git Push ist noch nicht implementiert.",
                    L"Git", MB_OK | MB_ICONINFORMATION);
        break;

    case IDM_GIT_PULL:
        MessageBoxW(hw, L"Git Pull ist noch nicht implementiert.",
                    L"Git", MB_OK | MB_ICONINFORMATION);
        break;

    /* ----- Hilfe ----- */
    case IDM_HELP_ABOUT:
        DoAbout();
        break;
    }
}

/* =========================================================================
 * Keyboard accelerators
 * ====================================================================== */
static HACCEL CreateAccelerators() {
    ACCEL acc[] = {
        { FVIRTKEY | FCONTROL,          'N',        IDM_FILE_NEW           },
        { FVIRTKEY | FCONTROL,          'O',        IDM_FILE_OPEN          },
        { FVIRTKEY | FCONTROL,          'S',        IDM_FILE_SAVE          },
        { FVIRTKEY | FCONTROL | FSHIFT, 'S',        IDM_FILE_SAVEAS        },
        { FVIRTKEY | FCONTROL,          'W',        IDM_FILE_CLOSE_TAB     },
        { FVIRTKEY | FCONTROL,          'Z',        IDM_EDIT_UNDO          },
        { FVIRTKEY | FCONTROL,          'Y',        IDM_EDIT_REDO          },
        { FVIRTKEY | FCONTROL,          'X',        IDM_EDIT_CUT           },
        { FVIRTKEY | FCONTROL,          'C',        IDM_EDIT_COPY          },
        { FVIRTKEY | FCONTROL,          'V',        IDM_EDIT_PASTE         },
        { FVIRTKEY | FCONTROL,          'A',        IDM_EDIT_SELECTALL     },
        { FVIRTKEY | FCONTROL,          'F',        IDM_EDIT_FIND          },
        { FVIRTKEY,                     VK_F3,      IDM_EDIT_FINDNEXT      },
        { FVIRTKEY | FCONTROL,          'H',        IDM_EDIT_REPLACE       },
        { FVIRTKEY | FCONTROL,          'G',        IDM_EDIT_GOTO          },
        { FVIRTKEY | FCONTROL,          'E',        IDM_VIEW_EXPLORER      },
        { FVIRTKEY,                     VK_F7,      IDM_BUILD_BUILD        },
        { FVIRTKEY | FCONTROL | FSHIFT, 'B',        IDM_BUILD_REBUILD      },
        { FVIRTKEY,                     VK_F5,      IDM_BUILD_RUN          },
        /* Zoom: VK_OEM_PLUS (+), VK_OEM_MINUS (-), '0' */
        { FVIRTKEY | FCONTROL,          VK_OEM_PLUS,  IDM_VIEW_ZOOMIN     },
        { FVIRTKEY | FCONTROL,          VK_OEM_MINUS, IDM_VIEW_ZOOMOUT    },
        { FVIRTKEY | FCONTROL,          '0',        IDM_VIEW_ZOOMRESET     },
    };
    return CreateAcceleratorTableW(acc, sizeof(acc) / sizeof(acc[0]));
}

/* =========================================================================
 * Main window procedure
 * ====================================================================== */
static LRESULT CALLBACK MainWndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_CREATE: {
        RECT cl; GetClientRect(hw, &cl);

        /* Ribbon (oben) */
        RibbonCreate(hw, cl.right);

        /* Status bar (unten) */
        StatusCreate(hw);

        /* === Explorer-Panel (links) === */
        RECT rExp = {0, RIBBON_HEIGHT, g_explorerWidth, cl.bottom};
        ExplorerCreate(hw, rExp);

        /* === Document-Outline-Panel (rechts) mit Header + Suche + Tree === */
        {
            int outW = g_showOutline ? g_outlineWidth : 0;
            int outX = cl.right - outW;
            int outH = cl.bottom - RIBBON_HEIGHT;

            /* Äußeres Container-Fenster */
            WNDCLASSEXW wcOut = {};
            wcOut.cbSize        = sizeof(wcOut);
            wcOut.hInstance     = GetModuleHandleW(nullptr);
            wcOut.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
            wcOut.hbrBackground = nullptr;
            wcOut.lpszClassName = L"NovaDocOutlineContainer";
            wcOut.lpfnWndProc   = [](HWND hw2, UINT m, WPARAM w, LPARAM l) -> LRESULT {
                static HWND s_search = nullptr, s_tree = nullptr;
                enum { HDR_H = 28, SRH_H = 26 };
                switch (m) {
                case WM_CREATE:
                    return 0;
                case WM_ERASEBKGND:
                    return 1;
                case WM_PAINT: {
                    PAINTSTRUCT ps; HDC hdc = BeginPaint(hw2, &ps);
                    RECT cl2; GetClientRect(hw2, &cl2);
                    HBRUSH brBg = CreateSolidBrush(g_theme.bg_panel);
                    FillRect(hdc, &cl2, brBg); DeleteObject(brBg);
                    /* Header */
                    RECT hdr = {0, 0, cl2.right, HDR_H};
                    COLORREF hdrBg = (g_theme.kind==THEME_DARK)
                        ? RGB(0x1A,0x1A,0x2A) : RGB(0xDC,0xDC,0xEA);
                    HBRUSH brH = CreateSolidBrush(hdrBg);
                    FillRect(hdc, &hdr, brH); DeleteObject(brH);
                    COLORREF sep = (g_theme.kind==THEME_DARK)
                        ? RGB(0x38,0x38,0x52) : RGB(0xCC,0xCC,0xDD);
                    HPEN pen = CreatePen(PS_SOLID,1,sep);
                    HPEN op  = (HPEN)SelectObject(hdc,pen);
                    MoveToEx(hdc,0,HDR_H-1,nullptr); LineTo(hdc,cl2.right,HDR_H-1);
                    SelectObject(hdc,op); DeleteObject(pen);
                    /* Linker Rand-Separator */
                    pen = CreatePen(PS_SOLID,1,sep);
                    op  = (HPEN)SelectObject(hdc,pen);
                    MoveToEx(hdc,0,0,nullptr); LineTo(hdc,0,cl2.bottom);
                    SelectObject(hdc,op); DeleteObject(pen);
                    HFONT fnt = CreateFontW(13,0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,
                        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                        CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
                    HFONT of = (HFONT)SelectObject(hdc,fnt);
                    SetBkMode(hdc,TRANSPARENT);
                    COLORREF tc = (g_theme.kind==THEME_DARK)
                        ? RGB(0xCD,0xD6,0xF4) : RGB(0x1F,0x1F,0x3F);
                    SetTextColor(hdc,tc);
                    RECT tr = {8,0,cl2.right-8,HDR_H};
                    DrawTextW(hdc,L"Dokumentgliederung",-1,&tr,
                        DT_LEFT|DT_VCENTER|DT_SINGLELINE);
                    DeleteObject(SelectObject(hdc,of));
                    EndPaint(hw2,&ps); return 0;
                }
                case WM_SIZE: {
                    RECT cl2; GetClientRect(hw2,&cl2);
                    int W2=cl2.right, H2=cl2.bottom;
                    HWND srch = GetDlgItem(hw2, ID_DOCOUTLINE+1);
                    HWND tree = GetDlgItem(hw2, ID_DOCOUTLINE);
                    if (srch) SetWindowPos(srch,nullptr,
                        5,HDR_H+2,W2-10,SRH_H-4,SWP_NOZORDER|SWP_NOACTIVATE);
                    int tt = HDR_H+SRH_H+2;
                    if (tree) SetWindowPos(tree,nullptr,
                        1,tt,W2-1,H2-tt,SWP_NOZORDER|SWP_NOACTIVATE);
                    return 0;
                }
                }
                return DefWindowProcW(hw2,m,w,l);
            };
            RegisterClassExW(&wcOut);

            HWND hOutCont = CreateWindowExW(0, L"NovaDocOutlineContainer", nullptr,
                WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
                outX, RIBBON_HEIGHT, outW, outH,
                hw, nullptr, GetModuleHandleW(nullptr), nullptr);

            /* Suchfeld */
            HWND hOutSearch = CreateWindowExW(WS_EX_CLIENTEDGE,
                L"EDIT", L"Symbole suchen (Strg+Alt+S)",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                5, HDR_H + 2, outW - 10, 22,
                hOutCont, (HMENU)(ID_DOCOUTLINE + 1),
                GetModuleHandleW(nullptr), nullptr);
            HFONT fntSm = CreateFontW(12,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
            SendMessageW(hOutSearch, WM_SETFONT, (WPARAM)fntSm, TRUE);

            /* TreeView */
            g_hDocOutline = CreateWindowExW(0,
                WC_TREEVIEWW, nullptr,
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | TVS_HASLINES |
                TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
                1, HDR_H + 28, outW - 1, outH - HDR_H - 28,
                hOutCont, (HMENU)ID_DOCOUTLINE,
                GetModuleHandleW(nullptr), nullptr);

            SetWindowTheme(g_hDocOutline,
                g_theme.kind==THEME_DARK ? L"DarkMode_Explorer" : L"Explorer", nullptr);
            TreeView_SetBkColor(g_hDocOutline, g_theme.bg_panel);
            TreeView_SetTextColor(g_hDocOutline, g_theme.fg_default);

            /* Gliederungsbaum füllen */
            if (g_hDocOutline) {
                TVINSERTSTRUCT tis = {};
                tis.hParent      = TVI_ROOT;
                tis.hInsertAfter = TVI_LAST;
                tis.item.mask    = TVIF_TEXT;
                tis.item.pszText = (LPWSTR)L"MainWindow";
                HTREEITEM hRoot = TreeView_InsertItem(g_hDocOutline, &tis);

                struct { HTREEITEM par; LPCWSTR name; } cats[] = {
                    {hRoot, L"Felder"},
                    {hRoot, L"Konstruktoren"},
                    {hRoot, L"Methoden"},
                    {hRoot, L"Eigenschaften"},
                    {hRoot, L"Ereignisse"},
                };
                for (auto &c : cats) {
                    tis.hParent = c.par;
                    tis.item.pszText = (LPWSTR)c.name;
                    TreeView_InsertItem(g_hDocOutline, &tis);
                }
                TreeView_Expand(g_hDocOutline, hRoot, TVE_EXPAND);
            }

            /* WM_SIZE initial trigger */
            SendMessageW(hOutCont, WM_SIZE, 0, MAKELPARAM(outW, outH));
        }

        /* === Editor (Mitte) === */
        RECT rEd = {g_explorerWidth, RIBBON_HEIGHT,
                    cl.right - g_outlineWidth, cl.bottom};
        EditorCreate(hw, rEd);
        break;
    }

    case WM_SIZE:
        LayoutApply(hw);
        StatusResize(hw);
        break;

    case WM_COMMAND:
        /* Check if it's from the editor RichEdit (EN_CHANGE) */
        if (HIWORD(wp) == EN_CHANGE && (HWND)lp == g_hEditor) {
            if (g_activeTab >= 0 && g_activeTab < (int)g_tabs.size()) {
                if (!g_tabs[g_activeTab].modified) {
                    g_tabs[g_activeTab].modified = true;
                    std::wstring t = g_tabs[g_activeTab].title + L" ●";
                    TCITEMW ti = {}; ti.mask = TCIF_TEXT; ti.pszText = (LPWSTR)t.c_str();
                    TabCtrl_SetItem(g_hTabBar, g_activeTab, &ti);
                    EditorUpdateTitle();
                }
            }
            break;
        }
        OnCommand(hw, wp);
        break;

    case WM_NOTIFY: {
        NMHDR *nm = (NMHDR*)lp;
        /* Tab selection changed */
        if (nm->idFrom == ID_TABBAR && nm->code == TCN_SELCHANGE) {
            int idx = TabCtrl_GetCurSel(g_hTabBar);
            if (idx >= 0 && idx < (int)g_tabs.size()) {
                g_activeTab = idx;
                auto &tab = g_tabs[idx];
                if (!tab.filepath.empty()) {
                    /* Reload from disk */
                    HANDLE hf = CreateFileW(tab.filepath.c_str(), GENERIC_READ,
                                FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, nullptr);
                    if (hf != INVALID_HANDLE_VALUE) {
                        DWORD sz = GetFileSize(hf, nullptr);
                        if (sz != INVALID_FILE_SIZE && sz < 8*1024*1024) {
                            std::string raw(sz + 1, '\0');
                            DWORD rd;
                            ReadFile(hf, &raw[0], sz, &rd, nullptr);
                            CloseHandle(hf);
                            int wl = MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, nullptr, 0);
                            std::wstring wt(wl, L'\0');
                            MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, &wt[0], wl);
                            SetWindowTextW(g_hEditor, wt.c_str());
                            EditorHighlight();
                        } else {
                            CloseHandle(hf);
                        }
                    }
                }
                EditorUpdateTitle();
            }
        }
        /* RichEdit selection changed → update status bar */
        if (nm->hwndFrom == g_hEditor && nm->code == EN_SELCHANGE) {
            EditorOnSelChange();
        }
        /* TreeView double-click → open file */
        if (nm->idFrom == ID_EXPLORER && nm->code == NM_DBLCLK) {
            ExplorerOpenSelected();
        }
        break;
    }

    case WM_TIMER:
        if (wp == TIMER_HIGHLIGHT) {
            KillTimer(hw, TIMER_HIGHLIGHT);
            EditorHighlight();
        }
        if (wp == TIMER_STATUS) {
            KillTimer(hw, TIMER_STATUS);
            EditorUpdateStatusBar();
        }
        break;

    case WM_KEYDOWN:
        /* Ctrl+/ for comment toggle */
        if (wp == VK_OEM_2 && (GetKeyState(VK_CONTROL) & 0x8000)) {
            EditorCommentToggle();
            return 0;
        }
        break;

    case WM_CLOSE: {
        /* Check unsaved tabs */
        for (int i = 0; i < (int)g_tabs.size(); i++) {
            if (g_tabs[i].modified) {
                wchar_t msg[MAX_PATH + 80];
                swprintf_s(msg, L"Ungespeicherte Änderungen in '%s'.\nBeenden ohne Speichern?",
                           g_tabs[i].title.c_str());
                int r = MessageBoxW(hw, msg, L"Nova Studio", MB_YESNO | MB_ICONQUESTION);
                if (r != IDYES) return 0;
            }
        }
        DestroyWindow(hw);
        break;
    }

    case WM_DESTROY:
        if (g_hFindDlg) { DestroyWindow(g_hFindDlg); g_hFindDlg = nullptr; }
        PostQuitMessage(0);
        break;

    default:
        /* Handle Find/Replace common dialog messages */
        if (msg == g_msgFindReplace && g_msgFindReplace != 0) {
            FINDREPLACEW *pfr = (FINDREPLACEW*)lp;
            if (pfr->Flags & FR_DIALOGTERM)
                g_hFindDlg = nullptr;
            if (pfr->Flags & FR_FINDNEXT)
                EditorFindNext(pfr->lpstrFindWhat, pfr->Flags);
            if (pfr->Flags & FR_REPLACE)
                EditorReplaceNext(pfr->lpstrFindWhat, pfr->lpstrReplaceWith, pfr->Flags);
            if (pfr->Flags & FR_REPLACEALL) {
                int n = EditorReplaceAll(pfr->lpstrFindWhat, pfr->lpstrReplaceWith, pfr->Flags);
                wchar_t s[64];
                swprintf_s(s, L"%d Ersetzung(en) vorgenommen.", n);
                MessageBoxW(hw, s, L"Ersetzen", MB_OK | MB_ICONINFORMATION);
            }
            return 0;
        }
        return DefWindowProcW(hw, msg, wp, lp);
    }
    return 0;
}

/* =========================================================================
 * WinMain
 * ====================================================================== */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nShow) {
    /* Initialize Common Controls */
    INITCOMMONCONTROLSEX icc = {};
    icc.dwSize = sizeof(icc);
    icc.dwICC  = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_TREEVIEW_CLASSES |
                 ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);
    CoInitialize(nullptr);

    /* Default theme */
    ThemeSetDark();

    /* Register window class */
    WNDCLASSEXW wc  = {};
    wc.cbSize       = sizeof(wc);
    wc.style        = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc  = MainWndProc;
    wc.hInstance    = hInst;
    wc.hIcon        = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hCursor      = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(g_theme.bg_panel);
    wc.lpszClassName = STUDIO_CLASS;
    RegisterClassExW(&wc);

    /* Create main window (ohne HMENU – Ribbon übernimmt die Navigation) */
    g_hMain = CreateWindowExW(
        WS_EX_APPWINDOW,
        STUDIO_CLASS, STUDIO_NAME L" " STUDIO_VERSION,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1340, 820,
        nullptr, nullptr, hInst, nullptr);

    if (!g_hMain) return 1;

    ApplyDwmTheme(g_hMain);
    ShowWindow(g_hMain, nShow);
    UpdateWindow(g_hMain);

    HACCEL hAccel = CreateAccelerators();

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0)) {
        /* Let Find/Replace dialog process its messages */
        if (g_hFindDlg && IsDialogMessageW(g_hFindDlg, &message))
            continue;
        if (!TranslateAcceleratorW(g_hMain, hAccel, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    DestroyAcceleratorTable(hAccel);
    CoUninitialize();
    return (int)message.wParam;
}
