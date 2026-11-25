#include "studio.h"

/* =========================================================================
 * Find / Replace (Windows common dialogs – modeless)
 * ====================================================================== */
void DoFind() {
    if (g_hFindDlg) { SetForegroundWindow(g_hFindDlg); return; }
    g_fr.lStructSize     = sizeof(g_fr);
    g_fr.hwndOwner       = g_hMain;
    g_fr.lpstrFindWhat   = g_szFindBuf;
    g_fr.wFindWhatLen    = sizeof(g_szFindBuf) / sizeof(wchar_t);
    g_fr.Flags           = FR_DOWN | FR_HIDEWHOLEWORD;
    g_hFindDlg = FindTextW(&g_fr);
    if (g_hFindDlg) SetFocus(g_hFindDlg);
}

void DoReplace() {
    if (g_hFindDlg) { SetForegroundWindow(g_hFindDlg); return; }
    g_fr.lStructSize       = sizeof(g_fr);
    g_fr.hwndOwner         = g_hMain;
    g_fr.lpstrFindWhat     = g_szFindBuf;
    g_fr.wFindWhatLen      = sizeof(g_szFindBuf) / sizeof(wchar_t);
    g_fr.lpstrReplaceWith  = g_szReplaceBuf;
    g_fr.wReplaceWithLen   = sizeof(g_szReplaceBuf) / sizeof(wchar_t);
    g_fr.Flags             = FR_DOWN | FR_HIDEWHOLEWORD;
    g_hFindDlg = ReplaceTextW(&g_fr);
    if (g_hFindDlg) SetFocus(g_hFindDlg);
}

/* =========================================================================
 * Go To Line dialog
 * ====================================================================== */
static int g_gotoLine = 0;

static INT_PTR CALLBACK GotoDlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp) {
    switch (uMsg) {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, 101));
        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wp) == IDOK) {
            wchar_t buf[32] = {};
            GetDlgItemTextW(hDlg, 101, buf, 32);
            g_gotoLine = _wtoi(buf);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wp) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void DoGoToLine() {
    /* Build dialog template in memory */
    DlgBuilder d;
    d.beginDialog(4, 50, 50, 165, 52, L"Gehe zu Zeile");

    /* "Zeile:" label */
    d.addItem(WS_CHILD | WS_VISIBLE | SS_LEFT,
              0, 5, 12, 40, 10, 0xFFFF, CLS_STATIC, L"Zeile:");

    /* Edit control for line number */
    d.addItem(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER,
              WS_EX_CLIENTEDGE, 50, 10, 105, 12, 101, CLS_EDIT, L"");

    /* OK */
    d.addItem(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
              0, 25, 32, 50, 14, IDOK, CLS_BUTTON, L"OK");

    /* Abbrechen */
    d.addItem(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
              0, 85, 32, 65, 14, IDCANCEL, CLS_BUTTON, L"Abbrechen");

    g_gotoLine = 0;
    INT_PTR r = DialogBoxIndirectParamW(
        GetModuleHandleW(nullptr), d.ptr(),
        g_hMain, GotoDlgProc, 0);

    if (r == IDOK && g_gotoLine > 0)
        EditorGoToLine(g_gotoLine);
}

/* =========================================================================
 * Projektweite Suche (NPSPEC-STUDIO-EDITOR-0001-Nachtrag)
 * ====================================================================== */
static wchar_t g_projectSearchQuery[256] = {};

static INT_PTR CALLBACK ProjectSearchDlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp) {
    switch (uMsg) {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, 101));
        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wp) == IDOK) {
            GetDlgItemTextW(hDlg, 101, g_projectSearchQuery, 256);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wp) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void DoProjectSearch() {
    DlgBuilder d;
    d.beginDialog(4, 50, 50, 220, 52, L"Projektweite Suche");

    d.addItem(WS_CHILD | WS_VISIBLE | SS_LEFT,
              0, 5, 12, 50, 10, 0xFFFF, CLS_STATIC, L"Suchtext:");

    d.addItem(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
              WS_EX_CLIENTEDGE, 60, 10, 150, 12, 101, CLS_EDIT, L"");

    d.addItem(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
              0, 55, 32, 50, 14, IDOK, CLS_BUTTON, L"Suchen");

    d.addItem(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
              0, 115, 32, 65, 14, IDCANCEL, CLS_BUTTON, L"Abbrechen");

    g_projectSearchQuery[0] = L'\0';
    INT_PTR r = DialogBoxIndirectParamW(
        GetModuleHandleW(nullptr), d.ptr(),
        g_hMain, ProjectSearchDlgProc, 0);

    if (r == IDOK && g_projectSearchQuery[0] != L'\0')
        RunProjectSearch(g_projectSearchQuery);
}

/* =========================================================================
 * New Project dialog
 * ====================================================================== */
wchar_t g_szFindBuf[512]    = {};
wchar_t g_szReplaceBuf[512] = {};

static wchar_t g_newProjectName[128]  = {};
static wchar_t g_newProjectPath[MAX_PATH] = {};

static INT_PTR CALLBACK NewProjDlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp) {
    switch (uMsg) {
    case WM_INITDIALOG:
        SetDlgItemTextW(hDlg, 201, L"MeinProjekt");
        SetDlgItemTextW(hDlg, 202, g_newProjectPath);
        SetFocus(GetDlgItem(hDlg, 201));
        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wp) == 203) {  /* Durchsuchen */
            BROWSEINFOW bi = {};
            bi.hwndOwner = hDlg;
            bi.lpszTitle = L"Projektordner wählen";
            bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
            LPITEMIDLIST pil = SHBrowseForFolderW(&bi);
            if (pil) {
                wchar_t path[MAX_PATH];
                SHGetPathFromIDListW(pil, path);
                SetDlgItemTextW(hDlg, 202, path);
                CoTaskMemFree(pil);
            }
            return TRUE;
        }
        if (LOWORD(wp) == IDOK) {
            GetDlgItemTextW(hDlg, 201, g_newProjectName, 128);
            GetDlgItemTextW(hDlg, 202, g_newProjectPath, MAX_PATH);
            if (!g_newProjectName[0]) {
                MessageBoxW(hDlg, L"Bitte einen Projektnamen eingeben.",
                            L"Nova Studio", MB_OK | MB_ICONWARNING);
                return TRUE;
            }
            if (!g_newProjectPath[0]) {
                MessageBoxW(hDlg, L"Bitte einen Speicherort wählen.",
                            L"Nova Studio", MB_OK | MB_ICONWARNING);
                return TRUE;
            }
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wp) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

bool DoNewProject() {
    /* Get initial path: Desktop */
    SHGetSpecialFolderPathW(nullptr, g_newProjectPath, CSIDL_DESKTOPDIRECTORY, FALSE);
    g_newProjectName[0] = L'\0';

    /* Build dialog: Name, Pfad, Durchsuchen-Button */
    DlgBuilder d;
    d.beginDialog(7, 30, 30, 230, 90, L"Neues Projekt");

    /* Row 1: Projektname */
    d.addItem(WS_CHILD | WS_VISIBLE | SS_LEFT,
              0, 5, 8, 70, 10, 0xFFFF, CLS_STATIC, L"Projektname:");
    d.addItem(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
              WS_EX_CLIENTEDGE, 80, 6, 140, 12, 201, CLS_EDIT, L"");

    /* Row 2: Speicherort */
    d.addItem(WS_CHILD | WS_VISIBLE | SS_LEFT,
              0, 5, 26, 70, 10, 0xFFFF, CLS_STATIC, L"Speicherort:");
    d.addItem(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
              WS_EX_CLIENTEDGE, 80, 24, 110, 12, 202, CLS_EDIT, L"");
    d.addItem(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
              0, 195, 23, 30, 14, 203, CLS_BUTTON, L"...");

    /* Buttons */
    d.addItem(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
              0, 55, 68, 55, 14, IDOK, CLS_BUTTON, L"Erstellen");
    d.addItem(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
              0, 120, 68, 65, 14, IDCANCEL, CLS_BUTTON, L"Abbrechen");

    INT_PTR r = DialogBoxIndirectParamW(
        GetModuleHandleW(nullptr), d.ptr(),
        g_hMain, NewProjDlgProc, 0);

    if (r != IDOK) return false;

    /* Create project directory */
    wchar_t projDir[MAX_PATH];
    PathCombineW(projDir, g_newProjectPath, g_newProjectName);

    if (!CreateDirectoryW(projDir, nullptr)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            MessageBoxW(g_hMain, L"Projektordner konnte nicht erstellt werden.",
                        L"Nova Studio", MB_OK | MB_ICONERROR);
            return false;
        }
    }

    /* Create src/ subdirectory */
    wchar_t srcDir[MAX_PATH];
    PathCombineW(srcDir, projDir, L"src");
    CreateDirectoryW(srcDir, nullptr);

    /* Write project.xml */
    wchar_t xmlPath[MAX_PATH];
    PathCombineW(xmlPath, projDir, L"project.xml");

    /* Convert project name to UTF-8 for the XML file */
    char projNameUtf8[256] = {};
    WideCharToMultiByte(CP_UTF8, 0, g_newProjectName, -1, projNameUtf8, sizeof(projNameUtf8), nullptr, nullptr);

    char xmlBuf[2048];
    int xmlLen = (int)snprintf(xmlBuf, sizeof(xmlBuf),
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<NovaProject version=\"1.0\">\n"
        "  <Identity>\n"
        "    <Name>%s</Name>\n"
        "    <Type>NovaLang Application</Type>\n"
        "    <Language>NovaLang 1.0</Language>\n"
        "  </Identity>\n"
        "  <Build>\n"
        "    <Configurations>\n"
        "      <Config name=\"Debug\"   optimize=\"false\" debugSymbols=\"true\"/>\n"
        "      <Config name=\"Release\" optimize=\"true\"  debugSymbols=\"false\"/>\n"
        "    </Configurations>\n"
        "    <Target>x86_32</Target>\n"
        "    <Output>Build/</Output>\n"
        "  </Build>\n"
        "  <Sources>\n"
        "    <File path=\"src/main.nova\"/>\n"
        "  </Sources>\n"
        "</NovaProject>\n",
        projNameUtf8);

    HANDLE hXml = CreateFileW(xmlPath, GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hXml != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(hXml, xmlBuf, (DWORD)xmlLen, &wr, nullptr);
        CloseHandle(hXml);
    }

    /* Write src/main.nova */
    wchar_t novaPath[MAX_PATH];
    PathCombineW(novaPath, srcDir, L"main.nova");

    char novaBuf[512];
    int novaLen = (int)snprintf(novaBuf, sizeof(novaBuf),
        "' %s – Nova Studio Projekt\n"
        "Imports Nova.System\n"
        "\n"
        "Module Main\n"
        "\n"
        "    Sub Main()\n"
        "        Console.WriteLine(\"Hallo, NovaOS!\")\n"
        "    End Sub\n"
        "\n"
        "End Module\n",
        projNameUtf8);

    HANDLE hNova = CreateFileW(novaPath, GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hNova != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(hNova, novaBuf, (DWORD)novaLen, &wr, nullptr);
        CloseHandle(hNova);
    }

    /* Open project in studio */
    g_project.name      = g_newProjectName;
    g_project.rootPath  = projDir;
    g_project.open      = true;
    g_project.sources   = { novaPath };
    g_project.entryFile = novaPath;

    ExplorerSetRoot(projDir);
    EditorOpenFile(novaPath);
    EditorUpdateTitle();

    wchar_t msg[512];
    swprintf_s(msg, L"Projekt '%s' wurde erstellt.", g_newProjectName);
    EditorOutput(msg);

    return true;
}

/* =========================================================================
 * About dialog
 * ====================================================================== */
void DoAbout() {
    wchar_t msg[512];
    swprintf_s(msg,
        L"Nova Studio " STUDIO_VERSION L"\n\n"
        L"Integrierte Entwicklungsumgebung für NovaLang\n"
        L"Teil von NovaOS\n\n"
        L"Win32 C++  –  keine externen Abhängigkeiten\n\n"
        L"NPSPEC: NPSPEC-STUDIO-ARCHITECTURE-0001\n"
        L"        NPSPEC-STUDIO-EDITOR-0001");
    MessageBoxW(g_hMain, msg, L"Über Nova Studio",
                MB_OK | MB_ICONINFORMATION);
}

