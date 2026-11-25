#include "studio.h"
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstdint>

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

/* Hinweis: eine klassische Win32-Menüleiste (CreateStudioMenu()) wurde
   hier entfernt - vollstaendig durch die Ribbonbar (ribbon.cpp) ersetzt,
   nie an das Hauptfenster angehaengt (toter Code, nie aufgerufen). Beim
   Entfernen wurde geprueft, dass JEDER dort gelistete Befehl ueber Ribbon
   und/oder Tastenkuerzel weiterhin erreichbar ist - "Projekt öffnen"
   (IDM_FILE_OPEN_PROJECT) war das einzige Kommando ohne echten
   UI-Zugang und wurde deshalb in ribbon.cpp ergaenzt (Strg+Umschalt+O). */

/* =========================================================================
 * Custom Title Bar
 * ====================================================================== */
static int  g_tbHover  = 0;     /* 0=none 1=min 2=max 3=close */
static bool g_tbActive = true;

static void InvalidateTitleBar(HWND hw) {
    RECT rc; GetClientRect(hw, &rc);
    rc.bottom = TITLEBAR_H;
    InvalidateRect(hw, &rc, FALSE);
}

/* Draw 6-arm asterisk logo */
static void DrawLogo(HDC hdc, int cx, int cy) {
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0x89, 0xB4, 0xFA));
    HPEN old = (HPEN)SelectObject(hdc, pen);
    for (int i = 0; i < 6; i++) {
        double a = i * 3.14159265 / 3.0;
        int x2 = cx + (int)(9.0 * cos(a));
        int y2 = cy + (int)(9.0 * sin(a));
        MoveToEx(hdc, cx, cy, nullptr);
        LineTo(hdc, x2, y2);
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
    /* Center dot */
    HBRUSH br = CreateSolidBrush(RGB(0x89, 0xB4, 0xFA));
    HPEN   np = CreatePen(PS_NULL, 0, 0);
    SelectObject(hdc, br); SelectObject(hdc, np);
    Ellipse(hdc, cx-2, cy-2, cx+3, cy+3);
    SelectObject(hdc, old);
    DeleteObject(br); DeleteObject(np);
}

static void DrawTitleBar(HDC hdc, HWND hw) {
    RECT cl; GetClientRect(hw, &cl);
    int W = cl.right;
    int H = TITLEBAR_H;

    /* ---- Background ---- */
    COLORREF tbBg = RGB(0x18, 0x18, 0x27);
    HBRUSH brBg = CreateSolidBrush(tbBg);
    RECT tb = {0, 0, W, H};
    FillRect(hdc, &tb, brBg);
    DeleteObject(brBg);

    /* ---- Bottom separator ---- */
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0x2E, 0x2E, 0x45));
    HPEN opn = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, 0, H - 1, nullptr);
    LineTo(hdc, W - 138, H - 1);
    SelectObject(hdc, opn); DeleteObject(pen);

    /* ---- Logo (x=14, vertically centered) ---- */
    DrawLogo(hdc, 16, H / 2);

    /* ---- "NovaStudio" text ---- */
    HFONT fBold = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT fNorm = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT old = (HFONT)SelectObject(hdc, fBold);
    SetBkMode(hdc, TRANSPARENT);
    COLORREF textClr = g_tbActive ? RGB(0xFF, 0xFF, 0xFF) : RGB(0x88, 0x88, 0xA0);
    SetTextColor(hdc, textClr);
    RECT rNS = {30, 0, 150, H};
    DrawTextW(hdc, L"NovaStudio", -1, &rNS, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    /* ---- Separator "|" ---- */
    SelectObject(hdc, fNorm);
    SetTextColor(hdc, RGB(0x44, 0x44, 0x60));
    RECT rSep = {148, 0, 162, H};
    DrawTextW(hdc, L"|", -1, &rSep, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    /* ---- "ProjektName - Debug  ∨" ---- */
    std::wstring proj;
    if (g_project.open && !g_project.name.empty())
        proj = g_project.name + L" - Debug  ⌄";
    else
        proj = L"NovaStudio  ⌄";
    SetTextColor(hdc, g_tbActive ? RGB(0xA0, 0xA8, 0xBE) : RGB(0x66, 0x66, 0x80));
    RECT rProj = {162, 0, 162 + 240, H};
    DrawTextW(hdc, proj.c_str(), -1, &rProj, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    /* ---- Search box ---- */
    int sbW = 300, sbH = 20;
    /* center between project label end and buttons; minimum x=420 */
    int sbX = (W - sbW) / 2;
    if (sbX < 420) sbX = 420;
    /* don't overlap buttons (138px from right) */
    if (sbX + sbW > W - 150) sbX = W - 150 - sbW;
    int sbY = (H - sbH) / 2;

    if (sbX > 410 && sbX + sbW < W - 140) {
        /* Background pill */
        HBRUSH srBr = CreateSolidBrush(RGB(0x26, 0x26, 0x3C));
        HPEN   srPn = CreatePen(PS_SOLID, 1, RGB(0x3A, 0x3A, 0x54));
        SelectObject(hdc, srBr); SelectObject(hdc, srPn);
        RoundRect(hdc, sbX, sbY, sbX + sbW, sbY + sbH, 10, 10);
        SelectObject(hdc, old); DeleteObject(srBr); DeleteObject(srPn);

        /* Search icon: simple magnifier drawn with GDI */
        int icx = sbX + 11, icy = sbY + sbH/2;
        HPEN icPen = CreatePen(PS_SOLID, 1, RGB(0x80, 0x88, 0xA0));
        SelectObject(hdc, icPen);
        HBRUSH noBr = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, noBr);
        Ellipse(hdc, icx-4, icy-4, icx+4, icy+4);
        MoveToEx(hdc, icx+3, icy+3, nullptr);
        LineTo(hdc, icx+6, icy+6);
        SelectObject(hdc, old); DeleteObject(icPen);

        /* Placeholder text */
        SelectObject(hdc, fNorm);
        SetTextColor(hdc, RGB(0x60, 0x68, 0x80));
        RECT rST = {sbX + 18, sbY, sbX + sbW - 6, sbY + sbH};
        DrawTextW(hdc, L"Suchen (Strg+Q)", -1, &rST, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    /* ---- Window control buttons (min / max / close) ---- */
    int btnW = 46;
    int clsX = W - btnW,         maxX = W - btnW*2, minX = W - btnW*3;

    auto fillBtn = [&](int x, int hover, COLORREF hoverClr) {
        COLORREF c = (g_tbHover == hover) ? hoverClr : tbBg;
        HBRUSH b = CreateSolidBrush(c);
        RECT r = {x, 0, x + btnW, H};
        FillRect(hdc, &r, b);
        DeleteObject(b);
    };
    fillBtn(clsX, 3, RGB(0xC4, 0x2B, 0x1A));
    fillBtn(maxX, 2, RGB(0x30, 0x30, 0x4A));
    fillBtn(minX, 1, RGB(0x30, 0x30, 0x4A));

    /* Draw icons via GDI (no font dependency) */
    COLORREF icActive = g_tbActive ? RGB(0xCC, 0xD4, 0xE4) : RGB(0x66, 0x66, 0x80);
    auto iconPen = [&](int hover) -> HPEN {
        COLORREF c = (g_tbHover == hover && hover == 3)
            ? RGB(0xFF,0xFF,0xFF) : icActive;
        return CreatePen(PS_SOLID, 1, c);
    };

    int iy = H / 2;
    /* Minimize: horizontal bar */
    {   HPEN p2 = iconPen(1); SelectObject(hdc, p2);
        MoveToEx(hdc, minX+16, iy, nullptr);
        LineTo(hdc, minX+30, iy);
        DeleteObject(SelectObject(hdc, old)); }

    /* Maximize / Restore: square or overlapping squares */
    {   HPEN p2 = iconPen(2); HBRUSH nb = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, p2); SelectObject(hdc, nb);
        bool maxed = IsZoomed(hw) != 0;
        if (!maxed) {
            Rectangle(hdc, maxX+16, iy-6, maxX+30, iy+6);
        } else {
            /* two overlapping squares for "restore" */
            Rectangle(hdc, maxX+18, iy-4, maxX+30, iy+6);
            MoveToEx(hdc, maxX+16, iy-6, nullptr); LineTo(hdc, maxX+28, iy-6);
            LineTo(hdc, maxX+28, iy+4);
        }
        DeleteObject(SelectObject(hdc, old)); }

    /* Close: X */
    {   HPEN p2 = iconPen(3); SelectObject(hdc, p2);
        MoveToEx(hdc, clsX+16, iy-6, nullptr); LineTo(hdc, clsX+30, iy+6);
        MoveToEx(hdc, clsX+30, iy-6, nullptr); LineTo(hdc, clsX+16, iy+6);
        DeleteObject(SelectObject(hdc, old)); }

    /* Cleanup fonts */
    SelectObject(hdc, old);
    DeleteObject(fBold); DeleteObject(fNorm);
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

    int top = TITLEBAR_H + RIBBON_HEIGHT;
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
    int top  = TITLEBAR_H + RIBBON_HEIGHT;
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
    RibbonResize(0, TITLEBAR_H, cl.right);

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
static bool RunCommand(const wchar_t *cmdLine, wchar_t *outBuf, int outBufLen, DWORD *outExitCode = nullptr) {
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
    WaitForSingleObject(pi.hProcess, 15000);
    if (outExitCode) {
        DWORD ec = 1;
        GetExitCodeProcess(pi.hProcess, &ec);
        *outExitCode = ec;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    /* Convert to wchar */
    int wlen = MultiByteToWideChar(CP_ACP, 0, accum.c_str(), -1, nullptr, 0);
    if (wlen > 0 && outBuf && outBufLen > 0)
        MultiByteToWideChar(CP_ACP, 0, accum.c_str(), -1, outBuf, outBufLen);

    return true;
}

/* -----------------------------------------------------------------------
 * Echte Toolchain-Anbindung (NPSPEC-STUDIO-BUILD-0001)
 *
 * NovaStudio rief bisher ein "novalang.exe" auf, das NovaLang nach einer
 * direkt unter Windows ausfuehrbaren .exe transpiliert (siehe
 * toolchain/novalang/) - das ist NICHT die fuer NovaOS tatsaechlich
 * verwendete, boot-verifizierte Toolchain. Die echte, aktiv gepflegte
 * Toolchain ist novalang/compiler/novac.exe (direkter x86-32-Codegen,
 * NOVALANG_SPEC v0.1-v0.16) + tools/nef/nef.exe (NEF-1.0-Linker), die
 * reale NovaOS-NEF-Dateien erzeugen (siehe KERNEL_GAP_ANALYSIS.md /
 * IMPLEMENTATION_STATUS.md). NovaStudio.exe liegt unter
 * toolchain/NovaStudio/ - Repo-Wurzel ist zwei Ebenen darueber. */
static std::wstring g_lastBuiltNef;

static std::wstring RepoRoot() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);      /* .../toolchain/NovaStudio */
    PathRemoveFileSpecW(exePath);      /* .../toolchain */
    PathRemoveFileSpecW(exePath);      /* Repo-Wurzel */
    return exePath;
}

static std::wstring NovacPath() { return RepoRoot() + L"\\novalang\\compiler\\novac.exe"; }
static std::wstring NefPath()   { return RepoRoot() + L"\\tools\\nef\\nef.exe"; }

/* Zerlegt "novac: Syntaxfehler: Zeile <N>: <Text>" in Zeilennummer+Text.
   Liefert false, wenn die Zeile nicht diesem Muster entspricht (dann wird
   sie trotzdem roh als Diagnose ohne Zeilenbezug aufgenommen, siehe
   DoBuild() - nichts wird unterschlagen). */
static bool ParseNovacErrorLine(const std::wstring &line, int *outLine, std::wstring *outMsg) {
    const wchar_t *marker = L"Zeile ";
    size_t pos = line.find(marker);
    if (pos == std::wstring::npos) return false;
    pos += wcslen(marker);
    size_t numEnd = pos;
    while (numEnd < line.size() && iswdigit(line[numEnd])) numEnd++;
    if (numEnd == pos) return false;
    *outLine = _wtoi(line.substr(pos, numEnd - pos).c_str());
    size_t colon = line.find(L':', numEnd);
    *outMsg = (colon != std::wstring::npos) ? line.substr(colon + 2) : line;
    return true;
}

void DoBuild(bool rebuild) {
    EditorOutputClear();
    ProblemsClear();
    EditorOutput(rebuild ? L"──── Neu erstellen ────" : L"──── Erstellen ────");
    g_lastBuiltNef.clear();

    /* Bei offenem Projekt wird IMMER die Entry-Datei des Projekts gebaut
       (projekt.xml, erstes <Sources><File>), NICHT einfach die zufaellig
       aktive Registerkarte - sonst baut "Erstellen" je nachdem, welcher
       Tab gerade fokussiert ist, eine andere Datei. Ohne offenes Projekt
       bleibt das bisherige Verhalten (aktive Registerkarte, Einzeldatei-
       Modus) erhalten. */
    std::wstring srcFile;
    if (g_project.open && !g_project.entryFile.empty()) {
        srcFile = g_project.entryFile;
    } else if (g_activeTab >= 0 && g_activeTab < (int)g_tabs.size()) {
        srcFile = g_tabs[g_activeTab].filepath;
    }

    if (srcFile.empty()) {
        EditorOutput(g_project.open
            ? L"Fehler: Projekt hat keine Einstiegsdatei (project.xml <Sources>)."
            : L"Fehler: Keine Datei geöffnet.");
        return;
    }

    std::wstring novac = NovacPath();
    std::wstring nef = NefPath();
    if (GetFileAttributesW(novac.c_str()) == INVALID_FILE_ATTRIBUTES) {
        EditorOutput((L"Fehler: Compiler nicht gefunden: " + novac).c_str());
        EditorOutput(L"(erwartet unter novalang\\compiler\\novac.exe relativ zur Repo-Wurzel)");
        return;
    }
    if (GetFileAttributesW(nef.c_str()) == INVALID_FILE_ATTRIBUTES) {
        EditorOutput((L"Fehler: NEF-Werkzeug nicht gefunden: " + nef).c_str());
        EditorOutput(L"(erwartet unter tools\\nef\\nef.exe relativ zur Repo-Wurzel)");
        return;
    }

    /* Praefix = Quelldatei ohne Endung (novac-Konvention, siehe Aufruf
       "novac <quelle.nova> -o <praefix>"). rebuild entfernt zuerst alte
       Artefakte, damit keine veralteten .bin/.nef-Dateien stehen bleiben. */
    wchar_t prefix[MAX_PATH];
    wcscpy_s(prefix, srcFile.c_str());
    PathRemoveExtensionW(prefix);
    std::wstring nefOut = std::wstring(prefix) + L".nef";

    if (rebuild) {
        DeleteFileW((std::wstring(prefix) + L".code.bin").c_str());
        DeleteFileW((std::wstring(prefix) + L".rodata.bin").c_str());
        DeleteFileW((std::wstring(prefix) + L".data.bin").c_str());
        DeleteFileW(nefOut.c_str());
    }

    /* Schritt 1: novac (Lexer/Parser/Codegen -> .code.bin/.rodata.bin/.data.bin). */
    wchar_t cmdLine[MAX_PATH * 3];
    swprintf_s(cmdLine, L"\"%s\" \"%s\" -o \"%s\"", novac.c_str(), srcFile.c_str(), prefix);
    wchar_t outBuf[8192] = {};
    DWORD exitCode = 1;
    EditorOutput((L"> novac \"" + srcFile + L"\" -o \"" + prefix + L"\"").c_str());
    RunCommand(cmdLine, outBuf, 8192, &exitCode);

    bool hadError = (exitCode != 0);
    std::wstring nefLinkCmd;
    {
        std::wstring all = outBuf;
        size_t pos = 0;
        while (pos < all.size()) {
            size_t nl = all.find(L'\n', pos);
            std::wstring line = (nl == std::wstring::npos) ? all.substr(pos) : all.substr(pos, nl - pos);
            while (!line.empty() && (line.back() == L'\r')) line.pop_back();
            if (!line.empty()) {
                EditorOutput(line.c_str());
                const wchar_t *nextStepMarker = L"novac: naechster Schritt: ";
                size_t m = line.find(nextStepMarker);
                if (m != std::wstring::npos) {
                    nefLinkCmd = line.substr(m + wcslen(nextStepMarker));
                } else if (line.find(L"Syntaxfehler") != std::wstring::npos ||
                           line.find(L"Fehler") != std::wstring::npos) {
                    hadError = true;
                    int errLine = 0;
                    std::wstring msg;
                    if (ParseNovacErrorLine(line, &errLine, &msg))
                        ProblemsAdd({ srcFile, errLine, msg });
                    else
                        ProblemsAdd({ srcFile, 0, line });
                }
            }
            if (nl == std::wstring::npos) break;
            pos = nl + 1;
        }
    }

    if (hadError || nefLinkCmd.empty()) {
        EditorOutput(L"──── Fehlgeschlagen (novac) ────");
        RECT rExp, rEd, rOut, rTab;
        LayoutCompute(g_hMain, &rExp, &rEd, &rOut, &rTab);
        EditorResize(rEd);
        /* Bei Fehlern automatisch auf "Probleme" umschalten. */
        TabCtrl_SetCurSel(g_hBottomTabs, 1);
        BottomPanelShow(1);
        return;
    }

    /* Schritt 2: nef link - Kommandozeile stammt EXAKT von novac (keine
       selbst erratenen Sektionslayouts), nur der Platzhalter "<ausgabe.nef>"
       wird durch den echten Zielpfad ersetzt. */
    const wchar_t *placeholder = L"<ausgabe.nef>";
    size_t ph = nefLinkCmd.find(placeholder);
    if (ph != std::wstring::npos) {
        nefLinkCmd = nefLinkCmd.substr(0, ph) + L"\"" + nefOut + L"\"" +
                     nefLinkCmd.substr(ph + wcslen(placeholder));
    }
    std::wstring nefCmdLine = L"\"" + nef + L"\" " + nefLinkCmd;
    EditorOutput((L"> nef " + nefLinkCmd).c_str());

    wchar_t nefOutBuf[8192] = {};
    DWORD nefExit = 1;
    RunCommand(nefCmdLine.c_str(), nefOutBuf, 8192, &nefExit);
    {
        std::wstring all = nefOutBuf;
        size_t pos = 0;
        while (pos < all.size()) {
            size_t nl = all.find(L'\n', pos);
            std::wstring line = (nl == std::wstring::npos) ? all.substr(pos) : all.substr(pos, nl - pos);
            while (!line.empty() && line.back() == L'\r') line.pop_back();
            if (!line.empty()) EditorOutput(line.c_str());
            if (nl == std::wstring::npos) break;
            pos = nl + 1;
        }
    }

    if (nefExit != 0) {
        EditorOutput(L"──── Fehlgeschlagen (nef link) ────");
        ProblemsAdd({ srcFile, 0, L"nef link fehlgeschlagen - siehe Ausgabe" });
        TabCtrl_SetCurSel(g_hBottomTabs, 1);
        BottomPanelShow(1);
        return;
    }

    /* Schritt 3: validieren (beweist eine echte, gueltige NEF-Datei statt
       nur "Linker meldete Erfolg"). */
    std::wstring validateCmd = L"\"" + nef + L"\" validate \"" + nefOut + L"\"";
    wchar_t valBuf[512] = {};
    DWORD valExit = 1;
    RunCommand(validateCmd.c_str(), valBuf, 512, &valExit);
    EditorOutput((L"> nef validate: " + std::wstring(valBuf)).c_str());

    if (valExit == 0) {
        g_lastBuiltNef = nefOut;
        EditorOutput((L"NEF erzeugt: " + nefOut).c_str());
        EditorOutput(L"──── Erfolgreich ────");
    } else {
        ProblemsAdd({ srcFile, 0, L"Erzeugte NEF-Datei ist nicht gueltig (nef validate)" });
        EditorOutput(L"──── Fehlgeschlagen (nef validate) ────");
        TabCtrl_SetCurSel(g_hBottomTabs, 1);
        BottomPanelShow(1);
    }

    RECT rExp, rEd, rOut, rTab;
    LayoutCompute(g_hMain, &rExp, &rEd, &rOut, &rTab);
    EditorResize(rEd);
}

void DoRun() {
    if (g_lastBuiltNef.empty()) {
        EditorOutput(L"Ausführen: Keine gebaute NEF-Datei vorhanden.");
        EditorOutput(L"Bitte zuerst 'Erstellen' ausführen.");
        return;
    }
    /* Eine NEF-Datei ist KEIN unter Windows ausfuehrbares Programm - sie
       muss unter NovaOS (QEMU) laufen. Die automatische Uebertragung in
       ein NovaFS-Testimage und der QEMU-Start sind in dieser NovaStudio-
       Version noch NICHT implementiert (siehe IMPLEMENTATION_STATUS.md,
       Abschnitt QEMU-Integration) - hier wird das ehrlich gemeldet statt
       etwas vorzutaeuschen. */
    EditorOutput((L"NEF-Datei bereit: " + g_lastBuiltNef).c_str());
    EditorOutput(L"[QEMU-Ausfuehrung ist in dieser NovaStudio-Version noch nicht implementiert]");
    EditorOutput(L"Pruefen Sie die Datei z.B. per \"tools\\nef\\nef.exe inspect\" oder booten Sie sie manuell per NovaFS-Transfer+QEMU.");
}

void DoClean() {
    EditorOutputClear();
    ProblemsClear();
    EditorOutput(L"──── Bereinigen ────");
    g_lastBuiltNef.clear();
    if (g_activeTab >= 0 && !g_tabs[g_activeTab].filepath.empty()) {
        wchar_t base[MAX_PATH];
        wcscpy_s(base, g_tabs[g_activeTab].filepath.c_str());
        PathRemoveExtensionW(base);
        int removed = 0;
        for (const wchar_t *ext : { L".code.bin", L".rodata.bin", L".data.bin", L".nef" }) {
            std::wstring p = std::wstring(base) + ext;
            if (DeleteFileW(p.c_str())) removed++;
        }
        EditorOutput(removed > 0 ? L"Artefakte gelöscht." : L"Keine Artefakte gefunden.");
    }
    EditorOutput(L"──── Fertig ────");
}

/* Syscall Browser (siehe SDK_SPECIFICATION.md): liest kernel/newkernel/
   syscall.h EIN (keine erfundene Liste) und zeigt jede tatsaechlich
   dort per #define deklarierte Syscall-/Operation-Konstante. Nur
   gleichzeilige Kommentare werden uebernommen - die Datei enthaelt
   auch mehrzeilige Kommentarbloecke VOR manchen #define-Zeilen, die
   sich nicht zuverlaessig genau EINER Konstante zuordnen lassen (siehe
   SDK_SPECIFICATION.md) und deshalb bewusst NICHT geraten werden. */
static bool g_syscallsLoaded = false;

static bool IsSyscallRelatedName(const char *name) {
    static const char *prefixes[] = { "SYSCALL_", "GUI_OP_", "VFS_OP_", "INPUT_OP_" };
    for (const char *p : prefixes) {
        if (strncmp(name, p, strlen(p)) == 0) return true;
    }
    return false;
}

void SyscallBrowserLoad() {
    if (g_syscallsLoaded) return; /* einmal pro Prozesslaufzeit genuegt - statische Header-Datei */
    g_syscallsLoaded = true;

    std::wstring headerPath = RepoRoot() + L"\\kernel\\newkernel\\syscall.h";
    HANDLE hf = CreateFileW(headerPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        EditorOutput((L"Syscall Browser: Datei nicht gefunden: " + headerPath).c_str());
        return;
    }
    DWORD sz = GetFileSize(hf, nullptr);
    std::string buf(sz + 1, '\0');
    DWORD rd = 0;
    ReadFile(hf, &buf[0], sz, &rd, nullptr);
    CloseHandle(hf);
    buf.resize(rd);

    if (g_hSyscalls) ListView_DeleteAllItems(g_hSyscalls);
    int count = 0;

    size_t pos = 0;
    while (pos < buf.size()) {
        size_t nl = buf.find('\n', pos);
        std::string line = (nl == std::string::npos) ? buf.substr(pos) : buf.substr(pos, nl - pos);
        pos = (nl == std::string::npos) ? buf.size() : nl + 1;

        /* Erwartetes Muster: "#define NAME VALUE" gefolgt von optional
           einem Block- oder Zeilenkommentar. */
        size_t p = line.find("#define");
        if (p == std::string::npos) continue;
        p += 7;
        while (p < line.size() && isspace((unsigned char)line[p])) p++;
        size_t nameStart = p;
        while (p < line.size() && (isalnum((unsigned char)line[p]) || line[p] == '_')) p++;
        std::string name = line.substr(nameStart, p - nameStart);
        if (name.empty() || !IsSyscallRelatedName(name.c_str())) continue;

        while (p < line.size() && isspace((unsigned char)line[p])) p++;
        size_t valStart = p;
        while (p < line.size() && line[p] != ' ' && line[p] != '\t' && line[p] != '/') p++;
        std::string value = line.substr(valStart, p - valStart);

        std::string comment;
        size_t cPos = line.find("/*", p);
        if (cPos != std::string::npos) {
            size_t cEnd = line.find("*/", cPos + 2);
            comment = (cEnd != std::string::npos) ? line.substr(cPos + 2, cEnd - cPos - 2)
                                                   : line.substr(cPos + 2);
        } else {
            cPos = line.find("//", p);
            if (cPos != std::string::npos) comment = line.substr(cPos + 2);
        }
        /* fuehrende/folgende Leerzeichen des Kommentars trimmen */
        size_t a = comment.find_first_not_of(" \t");
        size_t b = comment.find_last_not_of(" \t");
        comment = (a == std::string::npos) ? "" : comment.substr(a, b - a + 1);

        if (!g_hSyscalls) continue;
        auto toW = [](const std::string &s) {
            int wl = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
            std::wstring w(wl, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], wl);
            w.resize(wcslen(w.c_str()));
            return w;
        };
        std::wstring wName = toW(name), wValue = toW(value), wComment = toW(comment);

        LVITEMW it = {};
        it.mask = LVIF_TEXT;
        it.iItem = count;
        it.iSubItem = 0;
        it.pszText = (LPWSTR)wName.c_str();
        ListView_InsertItem(g_hSyscalls, &it);
        ListView_SetItemText(g_hSyscalls, count, 1, (LPWSTR)wValue.c_str());
        ListView_SetItemText(g_hSyscalls, count, 2, (LPWSTR)wComment.c_str());
        ++count;
    }

    EditorOutput((L"Syscall Browser: " + std::to_wstring(count) + L" Konstante(n) aus syscall.h geladen").c_str());
}

/* Capability Manager (siehe SDK_SPECIFICATION.md): liest kernel/
   newkernel/security.h EIN und zeigt jede dort per #define deklarierte
   SECURITY_CAP_*-Bitmaske mit Name, Hex-Wert und (falls es eine reine
   Einzelbit-Maske ist) der Bitposition. Deckt bewusst NUR dieses eine
   Capability-System ab (siehe security.h-Dateikopf: cap_core.c/
   program.c/verify.c sind DREI WEITERE, separate Konzepte, hier nicht
   erfasst - keine Vermischung vortaeuschen). */
static bool g_capabilitiesLoaded = false;

void CapabilityManagerLoad() {
    if (g_capabilitiesLoaded) return;
    g_capabilitiesLoaded = true;

    std::wstring headerPath = RepoRoot() + L"\\kernel\\newkernel\\security.h";
    HANDLE hf = CreateFileW(headerPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        EditorOutput((L"Capability Manager: Datei nicht gefunden: " + headerPath).c_str());
        return;
    }
    DWORD sz = GetFileSize(hf, nullptr);
    std::string buf(sz + 1, '\0');
    DWORD rd = 0;
    ReadFile(hf, &buf[0], sz, &rd, nullptr);
    CloseHandle(hf);
    buf.resize(rd);

    if (g_hCapabilities) ListView_DeleteAllItems(g_hCapabilities);
    int count = 0;

    size_t pos = 0;
    while (pos < buf.size()) {
        size_t nl = buf.find('\n', pos);
        std::string line = (nl == std::string::npos) ? buf.substr(pos) : buf.substr(pos, nl - pos);
        pos = (nl == std::string::npos) ? buf.size() : nl + 1;

        size_t p = line.find("#define");
        if (p == std::string::npos) continue;
        p += 7;
        while (p < line.size() && isspace((unsigned char)line[p])) p++;
        size_t nameStart = p;
        while (p < line.size() && (isalnum((unsigned char)line[p]) || line[p] == '_')) p++;
        std::string name = line.substr(nameStart, p - nameStart);
        if (name.find("SECURITY_CAP_") != 0 && name != "SECURITY_KERNEL_CAPS") continue;

        while (p < line.size() && isspace((unsigned char)line[p])) p++;
        size_t valStart = p;
        while (p < line.size() && line[p] != ' ' && line[p] != '\t' && line[p] != '/') p++;
        std::string value = line.substr(valStart, p - valStart);

        /* Bitposition nur anzeigen, wenn der Wert eine reine Einzelbit-
           Maske ist (genau 1 gesetztes Bit) - bei SECURITY_KERNEL_CAPS
           (eine kombinierte Maske aller Caps) bewusst leer lassen, um
           keine falsche Einzelbit-Bedeutung vorzutaeuschen. */
        unsigned long numeric = strtoul(value.c_str(), nullptr, 0);
        std::wstring bitStr;
        if (numeric != 0 && (numeric & (numeric - 1)) == 0) {
            int bit = 0;
            while ((numeric >> bit) != 1) bit++;
            bitStr = std::to_wstring(bit);
        }

        auto toW = [](const std::string &s) {
            int wl = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
            std::wstring w(wl, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], wl);
            w.resize(wcslen(w.c_str()));
            return w;
        };
        std::wstring wName = toW(name), wValue = toW(value);

        if (!g_hCapabilities) continue;
        LVITEMW it = {};
        it.mask = LVIF_TEXT;
        it.iItem = count;
        it.iSubItem = 0;
        it.pszText = (LPWSTR)wName.c_str();
        ListView_InsertItem(g_hCapabilities, &it);
        ListView_SetItemText(g_hCapabilities, count, 1, (LPWSTR)wValue.c_str());
        ListView_SetItemText(g_hCapabilities, count, 2, (LPWSTR)bitStr.c_str());
        ++count;
    }

    EditorOutput((L"Capability Manager: " + std::to_wstring(count) + L" Capability/-ies aus security.h geladen").c_str());
}

void DoInspectNef(const wchar_t *path) {
    EditorOutputClear();
    TabCtrl_SetCurSel(g_hBottomTabs, 0);
    BottomPanelShow(0);
    if (!g_showOutput) {
        /* Ausgabe-Panel war ausgeblendet - fuer das Inspektionsergebnis
           sichtbar machen, sonst sieht der Nutzer nichts. */
        g_showOutput = true;
        RECT rExp, rEd, rOut, rTab;
        LayoutCompute(g_hMain, &rExp, &rEd, &rOut, &rTab);
        EditorResize(rEd);
    }

    EditorOutput((L"──── NEF Inspector: " + std::wstring(path) + L" ────").c_str());

    std::wstring nef = NefPath();
    if (GetFileAttributesW(nef.c_str()) == INVALID_FILE_ATTRIBUTES) {
        EditorOutput((L"Fehler: NEF-Werkzeug nicht gefunden: " + nef).c_str());
        return;
    }

    for (const wchar_t *sub : { L"validate", L"inspect" }) {
        std::wstring cmd = L"\"" + nef + L"\" " + sub + L" \"" + path + L"\"";
        wchar_t outBuf[8192] = {};
        DWORD exitCode = 1;
        RunCommand(cmd.c_str(), outBuf, 8192, &exitCode);

        std::wstring all = outBuf;
        size_t pos = 0;
        while (pos < all.size()) {
            size_t nl = all.find(L'\n', pos);
            std::wstring line = (nl == std::wstring::npos) ? all.substr(pos) : all.substr(pos, nl - pos);
            while (!line.empty() && line.back() == L'\r') line.pop_back();
            if (!line.empty()) EditorOutput(line.c_str());
            if (nl == std::wstring::npos) break;
            pos = nl + 1;
        }
        EditorOutput(L"");
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
    g_project.sources.clear();
    g_project.entryFile.clear();

    /* <Sources><File path="..."/>...</Sources> einlesen - jeder Pfad ist
       relativ zum Projektordner (Konvention aus DoNewProject()). Das
       erste <File> ist die Datei, die "Erstellen" tatsaechlich uebersetzt
       (siehe Project::entryFile-Kommentar in studio.h). */
    if (hf != INVALID_HANDLE_VALUE) {
        /* Datei wurde oben schon einmal gelesen und geschlossen - fuer
           die Sources-Liste erneut (eigener, kleiner Lesevorgang, kein
           vollwertiger XML-Parser notwendig fuer dieses einfache,
           selbst kontrollierte Format). */
        HANDLE hf2 = CreateFileW(xmlPath, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hf2 != INVALID_HANDLE_VALUE) {
            char buf[8192] = {};
            DWORD rd2;
            ReadFile(hf2, buf, sizeof(buf) - 1, &rd2, nullptr);
            CloseHandle(hf2);

            const char *search = buf;
            for (;;) {
                const char *tag = strstr(search, "<File path=\"");
                if (!tag) break;
                tag += strlen("<File path=\"");
                const char *end = strchr(tag, '"');
                if (!end) break;
                std::string rel(tag, end - tag);
                int wl = MultiByteToWideChar(CP_UTF8, 0, rel.c_str(), -1, nullptr, 0);
                std::wstring wrel(wl, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, rel.c_str(), -1, &wrel[0], wl);
                wrel.resize(wcslen(wrel.c_str()));

                wchar_t abs[MAX_PATH];
                PathCombineW(abs, folder, wrel.c_str());
                g_project.sources.push_back(abs);
                search = end + 1;
            }
        }
    }
    if (!g_project.sources.empty()) {
        g_project.entryFile = g_project.sources[0];
    } else {
        EditorOutput(L"Warnung: project.xml enthält keine <Sources> - 'Erstellen' baut die aktive Registerkarte.");
    }

    ExplorerSetRoot(folder);
    EditorUpdateTitle();
    InvalidateTitleBar(g_hMain);

    wchar_t msg[MAX_PATH + 32];
    swprintf_s(msg, L"Projekt '%s' geöffnet (%d Quelldatei(en)).",
               g_project.name.c_str(), (int)g_project.sources.size());
    EditorOutput(msg);
}

/* Macht aus einem absoluten Pfad einen zu g_project.rootPath relativen
   Pfad mit "/"-Trennzeichen (project.xml-Konvention, siehe DoNewProject()).
   Liefert L"" wenn absolutePath NICHT unterhalb von rootPath liegt. */
static std::wstring MakeRelativeToProject(const wchar_t *absolutePath) {
    wchar_t rel[MAX_PATH] = {};
    wchar_t rootAbs[MAX_PATH], pathAbs[MAX_PATH];
    GetFullPathNameW(g_project.rootPath.c_str(), MAX_PATH, rootAbs, nullptr);
    GetFullPathNameW(absolutePath, MAX_PATH, pathAbs, nullptr);
    if (!PathRelativePathToW(rel, rootAbs, FILE_ATTRIBUTE_DIRECTORY, pathAbs, FILE_ATTRIBUTE_NORMAL)) {
        return L"";
    }
    std::wstring r = rel;
    /* PathRelativePathToW liefert oft ".\foo\bar" - fuehrendes ".\" entfernen. */
    if (r.size() >= 2 && r[0] == L'.' && r[1] == L'\\') r = r.substr(2);
    if (r.find(L"..") == 0) return L""; /* ausserhalb des Projektordners */
    for (auto &c : r) if (c == L'\\') c = L'/';
    return r;
}

/* Schreibt den <Sources>-Block von project.xml komplett aus
   g_project.sources neu (einfache Textsubstitution, siehe Kommentar
   bei OpenProject() - kein vollwertiger XML-Parser fuer dieses selbst
   kontrollierte, einfache Format notwendig). Gemeinsam genutzt von
   ProjectRegisterSource() (neue Datei) und ExplorerContextRename()
   (Datei umbenannt). */
static void RewriteProjectXmlSources() {
    if (!g_project.open) return;

    wchar_t xmlPath[MAX_PATH];
    PathCombineW(xmlPath, g_project.rootPath.c_str(), L"project.xml");

    HANDLE hf = CreateFileW(xmlPath, GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) return;
    char buf[8192] = {};
    DWORD rd;
    ReadFile(hf, buf, sizeof(buf) - 1, &rd, nullptr);
    CloseHandle(hf);

    std::string content(buf, rd);
    size_t start = content.find("<Sources>");
    size_t end = content.find("</Sources>");
    if (start == std::string::npos || end == std::string::npos || end < start) return;
    end += strlen("</Sources>");

    std::string newBlock = "<Sources>\n";
    for (const auto &s : g_project.sources) {
        std::wstring r = MakeRelativeToProject(s.c_str());
        if (r.empty()) continue;
        int len = WideCharToMultiByte(CP_UTF8, 0, r.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string r8(len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, r.c_str(), -1, &r8[0], len, nullptr, nullptr);
        r8.resize(strlen(r8.c_str()));
        newBlock += "    <File path=\"" + r8 + "\"/>\n";
    }
    newBlock += "  </Sources>";

    content = content.substr(0, start) + newBlock + content.substr(end);

    HANDLE hw = CreateFileW(xmlPath, GENERIC_WRITE, 0, nullptr,
                            CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hw != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(hw, content.c_str(), (DWORD)content.size(), &wr, nullptr);
        CloseHandle(hw);
    }
}

void RetargetPathReferences(const std::wstring &oldPath, const std::wstring &newPath) {
    for (auto &tab : g_tabs) {
        if (_wcsicmp(tab.filepath.c_str(), oldPath.c_str()) == 0) {
            tab.filepath = newPath;
            wchar_t name[MAX_PATH];
            wcscpy_s(name, newPath.c_str());
            PathStripPathW(name);
            tab.title = name;
        }
    }
    bool sourcesChanged = false;
    for (auto &s : g_project.sources) {
        if (_wcsicmp(s.c_str(), oldPath.c_str()) == 0) { s = newPath; sourcesChanged = true; }
    }
    if (_wcsicmp(g_project.entryFile.c_str(), oldPath.c_str()) == 0) g_project.entryFile = newPath;
    if (sourcesChanged) RewriteProjectXmlSources();
    EditorUpdateTitle();
}

void ProjectRegisterSource(const wchar_t *absolutePath) {
    if (!g_project.open) return;

    std::wstring rel = MakeRelativeToProject(absolutePath);
    if (rel.empty()) return; /* liegt nicht im Projektordner */

    for (const auto &s : g_project.sources) {
        if (_wcsicmp(s.c_str(), absolutePath) == 0) return; /* schon bekannt */
    }

    g_project.sources.push_back(absolutePath);
    if (g_project.entryFile.empty()) g_project.entryFile = absolutePath;

    RewriteProjectXmlSources();

    EditorOutput((L"Zum Projekt hinzugefügt: " + rel).c_str());
}

/* Projektweite Suche (nur Lesen, siehe studio.h-Kommentar zu
   RunProjectSearch - KEIN projektweites Ersetzen, bewusst zurueckgestellt). */
static bool HasRecognizedTextExtension(const wchar_t *path) {
    const wchar_t *ext = PathFindExtensionW(path);
    static const wchar_t *exts[] = { L".nova", L".nlf", L".nui", L".xml", L".md", L".txt" };
    for (const wchar_t *e : exts) {
        if (_wcsicmp(ext, e) == 0) return true;
    }
    return false;
}

#define PROJECT_SEARCH_MAX_RESULTS 500u

static void SearchDirectoryRecursive(const std::wstring &dir, const std::wstring &queryLower,
                                      uint32_t *resultCount) {
    if (*resultCount >= PROJECT_SEARCH_MAX_RESULTS) return;

    WIN32_FIND_DATAW fd;
    std::wstring pattern = dir + L"\\*";
    HANDLE hFind = FindFirstFileW(pattern.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        std::wstring full = dir + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            /* ".git"/versteckte Ordner ueberspringen - keine Build-/VCS-
               Interna durchsuchen, die ohnehin kein Quelltext sind. */
            if (fd.cFileName[0] == L'.') continue;
            SearchDirectoryRecursive(full, queryLower, resultCount);
            continue;
        }

        if (!HasRecognizedTextExtension(full.c_str())) continue;

        HANDLE hf = CreateFileW(full.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hf == INVALID_HANDLE_VALUE) continue;
        DWORD sz = GetFileSize(hf, nullptr);
        if (sz == INVALID_FILE_SIZE || sz > 8 * 1024 * 1024) { CloseHandle(hf); continue; }
        std::string raw(sz + 1, '\0');
        DWORD rd = 0;
        ReadFile(hf, &raw[0], sz, &rd, nullptr);
        CloseHandle(hf);
        raw.resize(rd);

        int wlen = MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, nullptr, 0);
        std::wstring wtext(wlen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, raw.c_str(), (int)rd, &wtext[0], wlen);

        int lineNo = 1;
        size_t pos = 0;
        while (pos < wtext.size() && *resultCount < PROJECT_SEARCH_MAX_RESULTS) {
            size_t nl = wtext.find(L'\n', pos);
            std::wstring line = (nl == std::wstring::npos) ? wtext.substr(pos) : wtext.substr(pos, nl - pos);
            if (!line.empty() && line.back() == L'\r') line.pop_back();

            std::wstring lineLower = line;
            for (auto &c : lineLower) c = towlower(c);
            if (lineLower.find(queryLower) != std::wstring::npos) {
                SearchResultsAdd({ full, lineNo, line });
                ++(*resultCount);
            }

            if (nl == std::wstring::npos) break;
            pos = nl + 1;
            ++lineNo;
        }
    } while (FindNextFileW(hFind, &fd) && *resultCount < PROJECT_SEARCH_MAX_RESULTS);
    FindClose(hFind);
}

void RunProjectSearch(const wchar_t *query) {
    SearchResultsClear();
    EditorOutputClear();

    if (!g_project.open) {
        EditorOutput(L"Projektweite Suche: kein Projekt geöffnet.");
        return;
    }
    if (!query || !*query) return;

    std::wstring queryLower = query;
    for (auto &c : queryLower) c = towlower(c);

    uint32_t resultCount = 0;
    SearchDirectoryRecursive(g_project.rootPath, queryLower, &resultCount);

    TabCtrl_SetCurSel(g_hBottomTabs, 4);
    BottomPanelShow(4);

    wchar_t msg[160];
    swprintf_s(msg, L"Projektweite Suche nach \"%s\": %u Treffer%s",
               query, resultCount,
               resultCount >= PROJECT_SEARCH_MAX_RESULTS ? L" (Obergrenze erreicht, es gibt evtl. mehr)" : L"");
    EditorOutput(msg);
    EditorOutput(L"(siehe Tab \"Suche\")");
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

    case IDM_EDIT_FIND_PROJECT:
        if (!g_showOutput) {
            g_showOutput = true;
            LayoutApply(hw);
        }
        DoProjectSearch();
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
        EditorSaveAllFiles();
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

    /* ----- Explorer-Kontextmenü ----- */
    case IDM_EXPL_NEWFILE:
        ExplorerContextNewFile(hw);
        break;

    case IDM_EXPL_DELETE:
        ExplorerContextDelete(hw);
        break;

    case IDM_EXPL_RENAME:
        ExplorerContextRename(hw);
        break;

    case IDM_EXPL_REFRESH:
        ExplorerRefresh();
        break;

    /* ----- Extras/Tools ----- */
    case IDM_TOOLS_SYSCALLS:
        if (!g_showOutput) {
            g_showOutput = true;
            LayoutApply(hw);
        }
        TabCtrl_SetCurSel(g_hBottomTabs, 2);
        SyscallBrowserLoad();
        BottomPanelShow(2);
        break;

    case IDM_TOOLS_CAPS:
        if (!g_showOutput) {
            g_showOutput = true;
            LayoutApply(hw);
        }
        TabCtrl_SetCurSel(g_hBottomTabs, 3);
        CapabilityManagerLoad();
        BottomPanelShow(3);
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
        { FVIRTKEY | FCONTROL | FSHIFT, 'O',        IDM_FILE_OPEN_PROJECT  },
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
        { FVIRTKEY | FCONTROL | FSHIFT, 'F',        IDM_EDIT_FIND_PROJECT  },
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

    /* ---- Custom title bar ---- */
    case WM_NCHITTEST: {
        /* Let DefWindowProc handle resize borders (WS_THICKFRAME) */
        LRESULT hit = DefWindowProcW(hw, msg, wp, lp);
        if (hit == HTCLIENT) {
            POINT pt = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
            ScreenToClient(hw, &pt);
            if (pt.y >= 0 && pt.y < TITLEBAR_H) {
                RECT cl2; GetClientRect(hw, &cl2);
                int X = pt.x, W2 = cl2.right;
                if (X >= W2 - 46)  return HTCLOSE;
                if (X >= W2 - 92)  return HTMAXBUTTON;
                if (X >= W2 - 138) return HTMINBUTTON;
                return HTCAPTION;
            }
        }
        return hit;
    }

    case WM_GETMINMAXINFO: {
        /* Prevent maximized window covering taskbar */
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfoW(MonitorFromWindow(hw, MONITOR_DEFAULTTONEAREST), &mi);
        MINMAXINFO *mmi = (MINMAXINFO*)lp;
        mmi->ptMaxPosition.x = mi.rcWork.left;
        mmi->ptMaxPosition.y = mi.rcWork.top;
        mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
        mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
        mmi->ptMinTrackSize.x = 600;
        mmi->ptMinTrackSize.y = 400;
        return 0;
    }

    case WM_NCMOUSEMOVE: {
        int newH = 0;
        if      (wp == HTCLOSE)     newH = 3;
        else if (wp == HTMAXBUTTON) newH = 2;
        else if (wp == HTMINBUTTON) newH = 1;
        if (newH != g_tbHover) { g_tbHover = newH; InvalidateTitleBar(hw); UpdateWindow(hw); }
        return DefWindowProcW(hw, msg, wp, lp);
    }
    case WM_NCMOUSELEAVE:
        if (g_tbHover) { g_tbHover = 0; InvalidateTitleBar(hw); UpdateWindow(hw); }
        return DefWindowProcW(hw, msg, wp, lp);

    case WM_NCACTIVATE:
        /* Return TRUE so Windows doesn't redraw a default NC caption */
        g_tbActive = (wp != FALSE);
        InvalidateTitleBar(hw);
        return TRUE;

    case WM_ACTIVATE:
        g_tbActive = (LOWORD(wp) != WA_INACTIVE);
        InvalidateTitleBar(hw);
        break;

    case WM_NCLBUTTONDOWN:
        if (wp == HTCLOSE)     { SendMessageW(hw, WM_SYSCOMMAND, SC_CLOSE,   lp); return 0; }
        if (wp == HTMAXBUTTON) { SendMessageW(hw, WM_SYSCOMMAND, IsZoomed(hw) ? SC_RESTORE : SC_MAXIMIZE, lp); return 0; }
        if (wp == HTMINBUTTON) { SendMessageW(hw, WM_SYSCOMMAND, SC_MINIMIZE, lp); return 0; }
        return DefWindowProcW(hw, msg, wp, lp);

    case WM_NCLBUTTONDBLCLK:
        if (wp == HTCAPTION) {
            SendMessageW(hw, WM_SYSCOMMAND,
                IsZoomed(hw) ? SC_RESTORE : SC_MAXIMIZE, lp);
            return 0;
        }
        return DefWindowProcW(hw, msg, wp, lp);

    case WM_ERASEBKGND: {
        /* Fill only the title bar strip; child windows handle the rest */
        HDC hdc2 = (HDC)wp;
        RECT tb2 = {0, 0, 9999, TITLEBAR_H};
        RECT cl2; GetClientRect(hw, &cl2);
        tb2.right = cl2.right;
        HBRUSH b2 = CreateSolidBrush(RGB(0x18, 0x18, 0x27));
        FillRect(hdc2, &tb2, b2);
        DeleteObject(b2);
        return 1;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc2 = BeginPaint(hw, &ps);
        if (ps.rcPaint.top < TITLEBAR_H)
            DrawTitleBar(hdc2, hw);
        EndPaint(hw, &ps);
        return 0;
    }

    case WM_CREATE: {
        RECT cl; GetClientRect(hw, &cl);

        /* Ribbon (oben) */
        RibbonCreate(hw, cl.right);

        /* Status bar (unten) */
        StatusCreate(hw);

        /* === Explorer-Panel (links) === */
        RECT rExp = {0, TITLEBAR_H + RIBBON_HEIGHT, g_explorerWidth, cl.bottom};
        ExplorerCreate(hw, rExp);

        /* === Document-Outline-Panel (rechts) mit Header + Suche + Tree === */
        {
            int outW = g_showOutline ? g_outlineWidth : 0;
            int outX = cl.right - outW;
            int outH = cl.bottom - (TITLEBAR_H + RIBBON_HEIGHT);

            /* Äußeres Container-Fenster */
            WNDCLASSEXW wcOut = {};
            wcOut.cbSize        = sizeof(wcOut);
            wcOut.hInstance     = GetModuleHandleW(nullptr);
            wcOut.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
            wcOut.hbrBackground = nullptr;
            wcOut.lpszClassName = L"NovaDocOutlineContainer";
            wcOut.lpfnWndProc   = [](HWND hw2, UINT m, WPARAM w, LPARAM l) -> LRESULT {
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
                outX, TITLEBAR_H + RIBBON_HEIGHT, outW, outH,
                hw, nullptr, GetModuleHandleW(nullptr), nullptr);

            /* Suchfeld */
            HWND hOutSearch = CreateWindowExW(WS_EX_CLIENTEDGE,
                L"EDIT", L"Symbole suchen (Strg+Alt+S)",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                5, 28 + 2, outW - 10, 22,
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
                1, 28 + 28, outW - 1, outH - 28 - 28,
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
        RECT rEd = {g_explorerWidth, TITLEBAR_H + RIBBON_HEIGHT,
                    cl.right - g_outlineWidth, cl.bottom};
        EditorCreate(hw, rEd);

        /* Tell DWM our frame extends TITLEBAR_H into client area
           → removes system caption, keeps drop shadow */
        MARGINS m = {0, 0, TITLEBAR_H, 0};
        DwmExtendFrameIntoClientArea(hw, &m);
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
        /* Explorer-Suchfeld: bisher rein dekorativ (kein Handler, siehe
           IMPLEMENTATION_STATUS.md) - jetzt echte Live-Filterung. */
        if (HIWORD(wp) == EN_CHANGE && (HWND)lp == g_hExplorerSearch) {
            wchar_t buf[256];
            GetWindowTextW(g_hExplorerSearch, buf, 256);
            ExplorerFilter(buf);
            break;
        }
        OnCommand(hw, wp);
        break;

    case WM_CONTEXTMENU: {
        if ((HWND)wp == g_hExplorer) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, IDM_EXPL_NEWFILE, L"Neue Datei...");
            AppendMenuW(hMenu, MF_STRING, IDM_EXPL_RENAME,  L"Umbenennen...");
            AppendMenuW(hMenu, MF_STRING, IDM_EXPL_DELETE,  L"Löschen (Papierkorb)");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING, IDM_EXPL_REFRESH, L"Aktualisieren");
            int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
            if (x == -1 && y == -1) {
                RECT rc; GetWindowRect(g_hExplorer, &rc);
                x = rc.left; y = rc.top;
            }
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, x, y, 0, hw, nullptr);
            DestroyMenu(hMenu);
            return 0;
        }
        break;
    }

    case WM_NOTIFY: {
        NMHDR *nm = (NMHDR*)lp;
        /* Tab selection changed */
        if (nm->idFrom == ID_TABBAR && nm->code == TCN_SELCHANGE) {
            int idx = TabCtrl_GetCurSel(g_hTabBar);
            if (idx >= 0 && idx < (int)g_tabs.size() && idx != g_activeTab) {
                /* Behobener Datenverlust-Bug (siehe EditorTab::content in
                   studio.h): die verlassene Registerkarte zuerst sichern,
                   NICHT die Zieldatei von der Festplatte neu laden. */
                EditorSyncActiveTabContent();
                g_activeTab = idx;
                SetWindowTextW(g_hEditor, g_tabs[idx].content.c_str());
                EditorHighlight();
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
        /* Unteres Panel umgeschaltet: Ausgabe(0)/Probleme(1)/Syscalls(2)/
           Capabilities(3)/Suche(4) sind echte Panels; Terminal(5)/Git(6)
           sind NICHT implementiert (siehe IMPLEMENTATION_STATUS.md) -
           zeigen bewusst nur einen Hinweistext statt etwas vorzutaeuschen. */
        if (nm->idFrom == ID_BOTTOMTABS && nm->code == TCN_SELCHANGE) {
            int idx = TabCtrl_GetCurSel(g_hBottomTabs);
            if (idx >= 0 && idx <= 4) {
                if (idx == 2) SyscallBrowserLoad();
                if (idx == 3) CapabilityManagerLoad();
                BottomPanelShow(idx);
            } else if (idx == 5 || idx == 6) {
                EditorOutputClear();
                EditorOutput(idx == 5
                    ? L"[Terminal ist in dieser NovaStudio-Version noch nicht implementiert]"
                    : L"[Git-Integration ist in dieser NovaStudio-Version noch nicht implementiert]");
                BottomPanelShow(0);
            }
        }
        /* Probleme-Liste: Doppelklick navigiert zur Quellcodeposition */
        if (nm->idFrom == ID_PROBLEMS && nm->code == NM_DBLCLK) {
            int idx = ListView_GetNextItem(g_hProblems, -1, LVNI_SELECTED);
            ProblemsNavigateTo(idx);
        }
        /* Suchergebnisse: Doppelklick navigiert zur Fundstelle */
        if (nm->idFrom == ID_SEARCH && nm->code == NM_DBLCLK) {
            int idx = ListView_GetNextItem(g_hSearch, -1, LVNI_SELECTED);
            SearchResultNavigateTo(idx);
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

    /* Create main window – WS_POPUP+WS_THICKFRAME: custom title bar,
       resize borders via DWM, no system caption */
    g_hMain = CreateWindowExW(
        WS_EX_APPWINDOW,
        STUDIO_CLASS, STUDIO_NAME,
        WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN,
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
