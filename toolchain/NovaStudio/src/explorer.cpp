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
static HWND g_hExplorerSearch    = nullptr;
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

    /* Search edit */
    g_hExplorerSearch = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT", L"Suchen (Strg+;)",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        4, EXPL_HDR_H + 2,
        (rc.right - rc.left) - 8, EXPL_SRH_H - 4,
        g_hExplorerContainer,
        (HMENU)ID_EXPLSRCH,
        GetModuleHandleW(nullptr), nullptr);

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
