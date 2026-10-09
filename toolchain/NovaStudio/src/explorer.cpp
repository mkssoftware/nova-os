#include "studio.h"

static std::wstring g_explorerRoot;

/* Accepted file extensions for the explorer */
static bool IsNovaFile(const wchar_t *name) {
    const wchar_t *ext = PathFindExtensionW(name);
    if (!ext) return false;
    return (_wcsicmp(ext, L".nova") == 0 ||
            _wcsicmp(ext, L".nlf")  == 0 ||
            _wcsicmp(ext, L".nui")  == 0 ||
            _wcsicmp(ext, L".xml")  == 0 ||
            _wcsicmp(ext, L".md")   == 0);
}

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

void ExplorerCreate(HWND hParent, RECT rc) {
    g_hExplorer = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WC_TREEVIEWW, nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
        TVS_SHOWSELALWAYS | TVS_INFOTIP,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        hParent, (HMENU)ID_EXPLORER,
        GetModuleHandleW(nullptr), nullptr);

    ExplorerApplyTheme();
}

void ExplorerResize(RECT rc) {
    if (g_hExplorer)
        SetWindowPos(g_hExplorer, nullptr,
                     rc.left, rc.top,
                     rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
}

void ExplorerApplyTheme() {
    if (!g_hExplorer) return;
    SetWindowTheme(g_hExplorer,
                   g_theme.kind == THEME_DARK ? L"DarkMode_Explorer" : L"Explorer",
                   nullptr);
    TreeView_SetBkColor(g_hExplorer,  g_theme.bg_panel);
    TreeView_SetTextColor(g_hExplorer, g_theme.fg_default);
}

void ExplorerSetRoot(const wchar_t *path) {
    if (!g_hExplorer) return;
    g_explorerRoot = path ? path : L"";

    /* Free old data */
    HTREEITEM hRoot = TreeView_GetRoot(g_hExplorer);
    while (hRoot) {
        HTREEITEM hNext = TreeView_GetNextSibling(g_hExplorer, hRoot);
        FreeTreeData(g_hExplorer, hRoot);
        hRoot = hNext;
    }
    TreeView_DeleteAllItems(g_hExplorer);

    if (path && *path) {
        wchar_t name[MAX_PATH];
        wcscpy_s(name, path);
        PathStripPathW(name);

        wchar_t *fullPath = new wchar_t[MAX_PATH];
        wcscpy_s(fullPath, MAX_PATH, path);

        TVINSERTSTRUCTW tvis  = {};
        tvis.hParent          = TVI_ROOT;
        tvis.hInsertAfter     = TVI_LAST;
        tvis.item.mask        = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
        tvis.item.pszText     = name;
        tvis.item.lParam      = (LPARAM)fullPath;
        tvis.item.stateMask   = TVIS_EXPANDED | TVIS_BOLD;
        tvis.item.state       = TVIS_EXPANDED | TVIS_BOLD;

        HTREEITEM hRoot = TreeView_InsertItem(g_hExplorer, &tvis);
        PopulateDir(g_hExplorer, hRoot, path, 0);
        TreeView_Expand(g_hExplorer, hRoot, TVE_EXPAND);
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
