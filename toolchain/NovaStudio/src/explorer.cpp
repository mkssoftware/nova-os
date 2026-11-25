/* explorer.cpp – Projektmappen-Explorer mit Header + Suchfeld
 *
 * Layout (von oben nach unten):
 *   EXPL_HDR_H  px  Titelleiste  "Projektmappen-Explorer"
 *   EXPL_SRH_H  px  Suchfeld     "Suchen (Strg+;)"
 *   rest             TreeView
 */
#include "studio.h"

#define EXPL_HDR_H   28
#define EXPL_SRH_H   26
#define ID_EXPLTREE  1200
#define ID_EXPLSRCH  1201

static std::wstring g_explorerRoot;
static HWND g_hExplorerContainer = nullptr;
HWND g_hExplorerSearch           = nullptr;
static HWND g_hExplorerTree      = nullptr;   /* alias for g_hExplorer */

/* -----------------------------------------------------------------------
 * Accepted file extensions
 * -------------------------------------------------------------------- */
static bool IsNovaFile(const wchar_t *name) {
    const wchar_t *ext = PathFindExtensionW(name);
    if (!ext) return false;
    return (_wcsicmp(ext, L".nova") == 0 ||
            _wcsicmp(ext, L".nlf")  == 0 ||
            _wcsicmp(ext, L".nui")  == 0 ||
            _wcsicmp(ext, L".xml")  == 0 ||
            _wcsicmp(ext, L".md")   == 0);
}

/* -----------------------------------------------------------------------
 * Populate directory tree
 * -------------------------------------------------------------------- */
static void PopulateDir(HWND hTree, HTREEITEM hParent,
                        const wchar_t *dir, int depth) {
    if (depth > 8) return;

    wchar_t pattern[MAX_PATH];
    PathCombineW(pattern, dir, L"*");

    WIN32_FIND_DATAW fd;
    HANDLE hf = FindFirstFileW(pattern, &fd);
    if (hf == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) continue;

        wchar_t *fullPath = new wchar_t[MAX_PATH];
        PathCombineW(fullPath, dir, fd.cFileName);

        bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (isDir || IsNovaFile(fd.cFileName)) {
            TVINSERTSTRUCTW tvis = {};
            tvis.hParent         = hParent;
            tvis.hInsertAfter    = TVI_SORT;
            tvis.item.mask       = TVIF_TEXT | TVIF_PARAM |
                                   (isDir ? TVIF_STATE : 0);
            tvis.item.pszText    = fd.cFileName;
            tvis.item.lParam     = (LPARAM)fullPath;
            if (isDir) {
                tvis.item.stateMask = TVIS_EXPANDED;
                tvis.item.state     = (depth == 0) ? TVIS_EXPANDED : 0;
            }
            HTREEITEM hItem = TreeView_InsertItem(hTree, &tvis);
            if (isDir)
                PopulateDir(hTree, hItem, fullPath, depth + 1);
            else
                delete[] fullPath;
        } else {
            delete[] fullPath;
        }
    } while (FindNextFileW(hf, &fd));

    FindClose(hf);
}

static void FreeTreeData(HWND hTree, HTREEITEM hItem) {
    TVITEMW item = {};
    item.mask    = TVIF_PARAM | TVIF_HANDLE;
    item.hItem   = hItem;
    TreeView_GetItem(hTree, &item);
    if (item.lParam) delete[] (wchar_t*)item.lParam;

    HTREEITEM hChild = TreeView_GetChild(hTree, hItem);
    while (hChild) {
        HTREEITEM hNext = TreeView_GetNextSibling(hTree, hChild);
        FreeTreeData(hTree, hChild);
        hChild = hNext;
    }
}

/* -----------------------------------------------------------------------
 * Container window: draws the header "Projektmappen-Explorer"
 * -------------------------------------------------------------------- */
static LRESULT CALLBACK ExplContainerProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hw, &ps);
        RECT cl; GetClientRect(hw, &cl);

        /* Panel background */
        HBRUSH brBg = CreateSolidBrush(g_theme.bg_panel);
        FillRect(hdc, &cl, brBg);
        DeleteObject(brBg);

        /* Header area */
        RECT hdrRc = {0, 0, cl.right, EXPL_HDR_H};
        COLORREF hdrBg = (g_theme.kind == THEME_DARK)
            ? RGB(0x1A, 0x1A, 0x2A) : RGB(0xDC, 0xDC, 0xEA);
        HBRUSH brHdr = CreateSolidBrush(hdrBg);
        FillRect(hdc, &hdrRc, brHdr);
        DeleteObject(brHdr);

        /* Bottom border on header */
        COLORREF sepClr = (g_theme.kind == THEME_DARK)
            ? RGB(0x38, 0x38, 0x52) : RGB(0xCC, 0xCC, 0xDD);
        HPEN pen = CreatePen(PS_SOLID, 1, sepClr);
        HPEN oldP = (HPEN)SelectObject(hdc, pen);
        MoveToEx(hdc, 0, EXPL_HDR_H - 1, nullptr);
        LineTo(hdc, cl.right, EXPL_HDR_H - 1);
        SelectObject(hdc, oldP);
        DeleteObject(pen);

        /* Header title text */
        HFONT fnt = CreateFontW(13, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        HFONT oldF = (HFONT)SelectObject(hdc, fnt);
        SetBkMode(hdc, TRANSPARENT);
        COLORREF textClr = (g_theme.kind == THEME_DARK)
            ? RGB(0xCD, 0xD6, 0xF4) : RGB(0x1F, 0x1F, 0x3F);
        SetTextColor(hdc, textClr);
        RECT txtRc = {8, 0, cl.right - 40, EXPL_HDR_H};
        DrawTextW(hdc, L"Projektmappen-Explorer", -1, &txtRc,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        DeleteObject(SelectObject(hdc, oldF));

        EndPaint(hw, &ps);
        return 0;
    }

    case WM_SIZE: {
        RECT cl; GetClientRect(hw, &cl);
        int W = cl.right, H = cl.bottom;
        int top = EXPL_HDR_H;

        /* Search box */
        if (g_hExplorerSearch)
            SetWindowPos(g_hExplorerSearch, nullptr,
                         4, top + 2, W - 8, EXPL_SRH_H - 4,
                         SWP_NOZORDER | SWP_NOACTIVATE);

        /* TreeView */
        int treeTop = top + EXPL_SRH_H + 2;
        if (g_hExplorer)
            SetWindowPos(g_hExplorer, nullptr,
                         0, treeTop, W, H - treeTop,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wp;
        COLORREF bg = (g_theme.kind == THEME_DARK)
            ? RGB(0x2A, 0x2A, 0x3E) : RGB(0xFF, 0xFF, 0xFF);
        COLORREF fg = g_theme.fg_default;
        SetBkColor(hdcEdit, bg);
        SetTextColor(hdcEdit, fg);
        static HBRUSH brEdit = nullptr;
        if (brEdit) DeleteObject(brEdit);
        brEdit = CreateSolidBrush(bg);
        return (LRESULT)brEdit;
    }

    case WM_SETFOCUS:
        if (g_hExplorer) SetFocus(g_hExplorer);
        return 0;
    }
    return DefWindowProcW(hw, msg, wp, lp);
}

/* -----------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------- */
void ExplorerCreate(HWND hParent, RECT rc) {
    /* Register container class */
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = ExplContainerProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"NovaExplorerContainer";
    RegisterClassExW(&wc);

    /* Container */
    g_hExplorerContainer = CreateWindowExW(
        0, L"NovaExplorerContainer", nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        rc.left, rc.top,
        rc.right - rc.left, rc.bottom - rc.top,
        hParent, nullptr, GetModuleHandleW(nullptr), nullptr);

    /* Search edit - war bisher rein dekorativ (literaler Text, kein
       EN_CHANGE-Handler, keine Filterwirkung, siehe IMPLEMENTATION_
       STATUS.md). Jetzt ein echtes Platzhalter-Feld (EM_SETCUEBANNER,
       verschwindet beim Tippen statt geloescht werden zu muessen) mit
       echter Live-Filterung ueber ExplorerFilter(). */
    g_hExplorerSearch = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        4, EXPL_HDR_H + 2,
        (rc.right - rc.left) - 8, EXPL_SRH_H - 4,
        g_hExplorerContainer,
        (HMENU)ID_EXPLSRCH,
        GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hExplorerSearch, EM_SETCUEBANNER, FALSE, (LPARAM)L"Suchen (Strg+;)");

    /* Font for search box */
    HFONT fntSm = CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    SendMessageW(g_hExplorerSearch, WM_SETFONT, (WPARAM)fntSm, TRUE);

    /* TreeView */
    int treeTop = EXPL_HDR_H + EXPL_SRH_H + 2;
    int W = rc.right - rc.left;
    int H = rc.bottom - rc.top;
    g_hExplorer = CreateWindowExW(
        0,
        WC_TREEVIEWW, nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
        TVS_SHOWSELALWAYS | TVS_INFOTIP,
        0, treeTop, W, H - treeTop,
        g_hExplorerContainer,
        (HMENU)ID_EXPLTREE,
        GetModuleHandleW(nullptr), nullptr);

    g_hExplorerTree = g_hExplorer;
    ExplorerApplyTheme();
}

void ExplorerResize(RECT rc) {
    if (g_hExplorerContainer)
        SetWindowPos(g_hExplorerContainer, nullptr,
                     rc.left, rc.top,
                     rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    /* WM_SIZE handler in ExplContainerProc repositions children */
    if (g_hExplorerContainer) {
        RECT cl; GetClientRect(g_hExplorerContainer, &cl);
        SendMessageW(g_hExplorerContainer, WM_SIZE, 0,
                     MAKELPARAM(cl.right, cl.bottom));
    }
}

void ExplorerApplyTheme() {
    if (g_hExplorer) {
        SetWindowTheme(g_hExplorer,
                       g_theme.kind == THEME_DARK ? L"DarkMode_Explorer" : L"Explorer",
                       nullptr);
        TreeView_SetBkColor(g_hExplorer, g_theme.bg_panel);
        TreeView_SetTextColor(g_hExplorer, g_theme.fg_default);
    }
    if (g_hExplorerContainer)
        InvalidateRect(g_hExplorerContainer, nullptr, TRUE);
}

void ExplorerSetRoot(const wchar_t *path) {
    if (!g_hExplorer) return;
    g_explorerRoot = path ? path : L"";

    HTREEITEM hRoot = TreeView_GetRoot(g_hExplorer);
    while (hRoot) {
        HTREEITEM hNext = TreeView_GetNextSibling(g_hExplorer, hRoot);
        FreeTreeData(g_hExplorer, hRoot);
        hRoot = hNext;
    }
    TreeView_DeleteAllItems(g_hExplorer);

    if (path && *path) {
        /* Solution root node */
        wchar_t solLabel[MAX_PATH + 32];
        wchar_t projName[MAX_PATH];
        wcscpy_s(projName, path);
        PathStripPathW(projName);
        swprintf_s(solLabel, L"Projektmappe '%s' (1 Projekt)", projName);

        wchar_t *fullPath = new wchar_t[MAX_PATH];
        wcscpy_s(fullPath, MAX_PATH, path);

        TVINSERTSTRUCTW tvis = {};
        tvis.hParent         = TVI_ROOT;
        tvis.hInsertAfter    = TVI_LAST;
        tvis.item.mask       = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
        tvis.item.pszText    = solLabel;
        tvis.item.lParam     = (LPARAM)fullPath;
        tvis.item.stateMask  = TVIS_EXPANDED | TVIS_BOLD;
        tvis.item.state      = TVIS_EXPANDED | TVIS_BOLD;

        HTREEITEM hSolRoot = TreeView_InsertItem(g_hExplorer, &tvis);

        /* Project node */
        wchar_t *projPath = new wchar_t[MAX_PATH];
        wcscpy_s(projPath, MAX_PATH, path);
        TVINSERTSTRUCTW tvis2 = {};
        tvis2.hParent         = hSolRoot;
        tvis2.hInsertAfter    = TVI_LAST;
        tvis2.item.mask       = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
        tvis2.item.pszText    = projName;
        tvis2.item.lParam     = (LPARAM)projPath;
        tvis2.item.stateMask  = TVIS_EXPANDED;
        tvis2.item.state      = TVIS_EXPANDED;
        HTREEITEM hProjRoot = TreeView_InsertItem(g_hExplorer, &tvis2);

        /* Populate project children */
        PopulateDir(g_hExplorer, hProjRoot, path, 0);
        TreeView_Expand(g_hExplorer, hSolRoot, TVE_EXPAND);
        TreeView_Expand(g_hExplorer, hProjRoot, TVE_EXPAND);
    }
}

/* Rekursive Helferin fuer ExplorerFilter(): sammelt alle Dateien unter
   dir, deren Dateiname query (case-insensitiv) enthaelt, als flache
   Liste (kein Unterordner-Baum - bei einer Filteransicht ist die
   Ordnerstruktur zweitrangig, der Dateiname zaehlt). */
static void CollectMatchingFiles(const std::wstring &dir, const std::wstring &queryLower,
                                  std::vector<std::wstring> *out) {
    wchar_t pattern[MAX_PATH];
    PathCombineW(pattern, dir.c_str(), L"*");
    WIN32_FIND_DATAW fd;
    HANDLE hf = FindFirstFileW(pattern, &fd);
    if (hf == INVALID_HANDLE_VALUE) return;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        wchar_t fullPath[MAX_PATH];
        PathCombineW(fullPath, dir.c_str(), fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (fd.cFileName[0] == L'.') continue; /* .git etc. ueberspringen */
            CollectMatchingFiles(fullPath, queryLower, out);
            continue;
        }
        if (!IsNovaFile(fd.cFileName)) continue;

        std::wstring nameLower = fd.cFileName;
        for (auto &c : nameLower) c = towlower(c);
        if (nameLower.find(queryLower) != std::wstring::npos) {
            out->push_back(fullPath);
        }
    } while (FindNextFileW(hf, &fd));
    FindClose(hf);
}

void ExplorerFilter(const wchar_t *query) {
    if (!g_hExplorer || g_explorerRoot.empty()) return;

    if (!query || !*query) {
        ExplorerSetRoot(g_explorerRoot.c_str());
        return;
    }

    std::wstring queryLower = query;
    for (auto &c : queryLower) c = towlower(c);

    std::vector<std::wstring> matches;
    CollectMatchingFiles(g_explorerRoot, queryLower, &matches);

    HTREEITEM hOldRoot = TreeView_GetRoot(g_hExplorer);
    while (hOldRoot) {
        HTREEITEM hNext = TreeView_GetNextSibling(g_hExplorer, hOldRoot);
        FreeTreeData(g_hExplorer, hOldRoot);
        hOldRoot = hNext;
    }
    TreeView_DeleteAllItems(g_hExplorer);

    wchar_t rootLabel[64];
    swprintf_s(rootLabel, L"Suchergebnisse (%zu)", matches.size());
    wchar_t *rootData = new wchar_t[MAX_PATH];
    wcscpy_s(rootData, MAX_PATH, g_explorerRoot.c_str());

    TVINSERTSTRUCTW rootIns = {};
    rootIns.hParent        = TVI_ROOT;
    rootIns.hInsertAfter   = TVI_LAST;
    rootIns.item.mask      = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    rootIns.item.pszText   = rootLabel;
    rootIns.item.lParam    = (LPARAM)rootData;
    rootIns.item.stateMask = TVIS_EXPANDED | TVIS_BOLD;
    rootIns.item.state     = TVIS_EXPANDED | TVIS_BOLD;
    HTREEITEM hRoot = TreeView_InsertItem(g_hExplorer, &rootIns);

    for (const auto &m : matches) {
        std::wstring rel;
        wchar_t relBuf[MAX_PATH];
        if (PathRelativePathToW(relBuf, g_explorerRoot.c_str(), FILE_ATTRIBUTE_DIRECTORY,
                                 m.c_str(), FILE_ATTRIBUTE_NORMAL)) {
            rel = relBuf;
            if (rel.size() >= 2 && rel[0] == L'.' && rel[1] == L'\\') rel = rel.substr(2);
        } else {
            rel = m;
        }

        wchar_t *itemData = new wchar_t[MAX_PATH];
        wcscpy_s(itemData, MAX_PATH, m.c_str());

        TVINSERTSTRUCTW tvis = {};
        tvis.hParent      = hRoot;
        tvis.hInsertAfter = TVI_SORT;
        tvis.item.mask    = TVIF_TEXT | TVIF_PARAM;
        tvis.item.pszText = (LPWSTR)rel.c_str();
        tvis.item.lParam  = (LPARAM)itemData;
        TreeView_InsertItem(g_hExplorer, &tvis);
    }
}

void ExplorerRefresh() {
    if (!g_explorerRoot.empty())
        ExplorerSetRoot(g_explorerRoot.c_str());
}

void ExplorerOpenSelected() {
    if (!g_hExplorer) return;
    HTREEITEM hSel = TreeView_GetSelection(g_hExplorer);
    if (!hSel) return;

    TVITEMW item = {};
    item.mask    = TVIF_PARAM | TVIF_HANDLE;
    item.hItem   = hSel;
    TreeView_GetItem(g_hExplorer, &item);

    if (!item.lParam) return;
    const wchar_t *path = (const wchar_t *)item.lParam;

    DWORD attr = GetFileAttributesW(path);
    if (attr == INVALID_FILE_ATTRIBUTES) return;
    if (attr & FILE_ATTRIBUTE_DIRECTORY) return;

    EditorOpenFile(path);
}

bool ExplorerGetSelected(std::wstring *outPath, bool *outIsDir) {
    if (!g_hExplorer) return false;
    HTREEITEM hSel = TreeView_GetSelection(g_hExplorer);
    if (!hSel) return false;

    TVITEMW item = {};
    item.mask  = TVIF_PARAM | TVIF_HANDLE;
    item.hItem = hSel;
    TreeView_GetItem(g_hExplorer, &item);
    if (!item.lParam) return false;

    const wchar_t *path = (const wchar_t *)item.lParam;
    DWORD attr = GetFileAttributesW(path);
    if (attr == INVALID_FILE_ATTRIBUTES) return false;

    *outPath  = path;
    *outIsDir = (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    return true;
}

/* -----------------------------------------------------------------------
 * Kontextmenue-Aktionen (NPSPEC-STUDIO-EXPLORER-0001-Nachtrag)
 * -------------------------------------------------------------------- */
static wchar_t g_newFileNameBuf[256] = {};

static bool HasKnownSourceExtension(const wchar_t *name) {
    const wchar_t *ext = PathFindExtensionW(name);
    if (!ext || !*ext) return false;
    static const wchar_t *exts[] = { L".nova", L".nlf", L".nui", L".xml", L".md", L".txt" };
    for (const wchar_t *e : exts) if (_wcsicmp(ext, e) == 0) return true;
    return false;
}

static INT_PTR CALLBACK NewFileNameDlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp) {
    (void)lp;
    switch (uMsg) {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, 101));
        return FALSE;
    case WM_COMMAND:
        if (LOWORD(wp) == IDOK) {
            GetDlgItemTextW(hDlg, 101, g_newFileNameBuf, 256);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wp) == IDCANCEL) { EndDialog(hDlg, IDCANCEL); return TRUE; }
        break;
    }
    return FALSE;
}

void ExplorerContextNewFile(HWND hOwner) {
    if (g_explorerRoot.empty()) {
        MessageBoxW(hOwner, L"Kein Projekt/Ordner geöffnet.", L"Nova Studio", MB_OK | MB_ICONINFORMATION);
        return;
    }

    std::wstring selPath;
    bool selIsDir = false;
    std::wstring targetDir = g_explorerRoot;
    if (ExplorerGetSelected(&selPath, &selIsDir)) {
        if (selIsDir) {
            targetDir = selPath;
        } else {
            wchar_t parent[MAX_PATH];
            wcscpy_s(parent, selPath.c_str());
            PathRemoveFileSpecW(parent);
            targetDir = parent;
        }
    }

    DlgBuilder d;
    d.beginDialog(4, 50, 50, 180, 52, L"Neue Datei");
    d.addItem(WS_CHILD | WS_VISIBLE | SS_LEFT,
              0, 5, 12, 60, 10, 0xFFFF, CLS_STATIC, L"Dateiname:");
    d.addItem(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
              WS_EX_CLIENTEDGE, 65, 10, 105, 12, 101, CLS_EDIT, L"neu.nova");
    d.addItem(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
              0, 20, 32, 50, 14, IDOK, CLS_BUTTON, L"Erstellen");
    d.addItem(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
              0, 80, 32, 65, 14, IDCANCEL, CLS_BUTTON, L"Abbrechen");

    g_newFileNameBuf[0] = L'\0';
    INT_PTR r = DialogBoxIndirectParamW(GetModuleHandleW(nullptr), d.ptr(),
                                         hOwner, NewFileNameDlgProc, 0);
    if (r != IDOK) return;

    wchar_t name[256] = {};
    wcscpy_s(name, g_newFileNameBuf);
    if (!*name) return;
    if (!HasKnownSourceExtension(name)) wcscat_s(name, L".nova");

    wchar_t fullPath[MAX_PATH];
    PathCombineW(fullPath, targetDir.c_str(), name);

    if (GetFileAttributesW(fullPath) != INVALID_FILE_ATTRIBUTES) {
        MessageBoxW(hOwner, L"Eine Datei mit diesem Namen existiert bereits.",
                    L"Nova Studio", MB_OK | MB_ICONWARNING);
        return;
    }

    HANDLE hf = CreateFileW(fullPath, GENERIC_WRITE, 0, nullptr,
                            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hf == INVALID_HANDLE_VALUE) {
        MessageBoxW(hOwner, L"Datei konnte nicht erstellt werden.", L"Fehler", MB_OK | MB_ICONERROR);
        return;
    }
    CloseHandle(hf);

    ExplorerRefresh();
    EditorOpenFile(fullPath);
    ProjectRegisterSource(fullPath);
}

void ExplorerContextDelete(HWND hOwner) {
    std::wstring selPath;
    bool selIsDir = false;
    if (!ExplorerGetSelected(&selPath, &selIsDir)) return;

    if (selIsDir) {
        MessageBoxW(hOwner, L"Ordner löschen ist in dieser NovaStudio-Version noch nicht implementiert "
                             L"(kein rekursives Löschen, um versehentlichen Datenverlust zu vermeiden).",
                    L"Nova Studio", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (selPath == g_explorerRoot) return; /* Projektwurzel selbst nicht loeschbar */

    wchar_t name[MAX_PATH];
    wcscpy_s(name, selPath.c_str());
    PathStripPathW(name);
    wchar_t msg[MAX_PATH + 96];
    swprintf_s(msg, L"'%s' in den Papierkorb verschieben?", name);
    if (MessageBoxW(hOwner, msg, L"Nova Studio", MB_YESNO | MB_ICONQUESTION) != IDYES) return;

    /* SHFileOperationW erwartet einen DOPPELT nullterminierten String. */
    wchar_t pathBuf[MAX_PATH + 1] = {};
    wcscpy_s(pathBuf, selPath.c_str());
    pathBuf[selPath.size() + 1] = L'\0';

    SHFILEOPSTRUCTW fo = {};
    fo.wFunc  = FO_DELETE;
    fo.pFrom  = pathBuf;
    fo.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;
    int rc = SHFileOperationW(&fo);

    /* Falls die geloeschte Datei gerade in einer Registerkarte offen
       ist: deren Dateibezug entfernen, damit ein spaeteres "Speichern"
       nicht versehentlich die geloeschte Datei unter dem alten Pfad neu
       erzeugt, ohne dass der Nutzer das erwartet. */
    if (rc == 0) {
        for (auto &tab : g_tabs) {
            if (_wcsicmp(tab.filepath.c_str(), selPath.c_str()) == 0) {
                tab.filepath.clear();
                tab.title = L"Untitled.nova";
            }
        }
        EditorUpdateTitle();
        ExplorerRefresh();
    }
}

static wchar_t g_renameBuf[256] = {};

static INT_PTR CALLBACK RenameDlgProc(HWND hDlg, UINT uMsg, WPARAM wp, LPARAM lp) {
    (void)lp;
    switch (uMsg) {
    case WM_INITDIALOG: {
        SetDlgItemTextW(hDlg, 101, g_renameBuf);
        HWND hEdit = GetDlgItem(hDlg, 101);
        SetFocus(hEdit);
        SendMessageW(hEdit, EM_SETSEL, 0, -1);
        return FALSE;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == IDOK) {
            GetDlgItemTextW(hDlg, 101, g_renameBuf, 256);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wp) == IDCANCEL) { EndDialog(hDlg, IDCANCEL); return TRUE; }
        break;
    }
    return FALSE;
}

void ExplorerContextRename(HWND hOwner) {
    std::wstring selPath;
    bool selIsDir = false;
    if (!ExplorerGetSelected(&selPath, &selIsDir)) return;

    if (selIsDir) {
        MessageBoxW(hOwner, L"Ordner umbenennen ist in dieser NovaStudio-Version noch nicht implementiert.",
                    L"Nova Studio", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (selPath == g_explorerRoot) return; /* Projektwurzel selbst nicht umbenennbar */

    wchar_t oldName[MAX_PATH];
    wcscpy_s(oldName, selPath.c_str());
    PathStripPathW(oldName);
    wcscpy_s(g_renameBuf, oldName);

    DlgBuilder d;
    d.beginDialog(4, 50, 50, 180, 52, L"Umbenennen");
    d.addItem(WS_CHILD | WS_VISIBLE | SS_LEFT,
              0, 5, 12, 60, 10, 0xFFFF, CLS_STATIC, L"Neuer Name:");
    d.addItem(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
              WS_EX_CLIENTEDGE, 65, 10, 105, 12, 101, CLS_EDIT, L"");
    d.addItem(WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
              0, 20, 32, 50, 14, IDOK, CLS_BUTTON, L"Umbenennen");
    d.addItem(WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
              0, 80, 32, 65, 14, IDCANCEL, CLS_BUTTON, L"Abbrechen");

    INT_PTR r = DialogBoxIndirectParamW(GetModuleHandleW(nullptr), d.ptr(),
                                         hOwner, RenameDlgProc, 0);
    if (r != IDOK) return;
    if (!g_renameBuf[0] || wcscmp(g_renameBuf, oldName) == 0) return;

    wchar_t parent[MAX_PATH];
    wcscpy_s(parent, selPath.c_str());
    PathRemoveFileSpecW(parent);
    wchar_t newPath[MAX_PATH];
    PathCombineW(newPath, parent, g_renameBuf);

    if (GetFileAttributesW(newPath) != INVALID_FILE_ATTRIBUTES) {
        MessageBoxW(hOwner, L"Eine Datei mit diesem Namen existiert bereits.",
                    L"Nova Studio", MB_OK | MB_ICONWARNING);
        return;
    }

    if (!MoveFileW(selPath.c_str(), newPath)) {
        MessageBoxW(hOwner, L"Umbenennen fehlgeschlagen.", L"Fehler", MB_OK | MB_ICONERROR);
        return;
    }

    /* Offene Registerkarten UND die project.xml-Quellenliste auf den
       neuen Pfad umziehen - sonst wuerde ein spaeteres "Speichern" die
       Datei unter dem alten (jetzt nicht mehr existierenden) Namen neu
       anlegen und project.xml zeigte auf eine Datei, die es so nicht
       mehr gibt. */
    RetargetPathReferences(selPath, newPath);
    ExplorerRefresh();
}
