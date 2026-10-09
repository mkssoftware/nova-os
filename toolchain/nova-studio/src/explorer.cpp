#include "studio.h"

HWND g_hExplorer = nullptr;

static HTREEITEM AddTreeItem(HWND hTree, HTREEITEM hParent,
                              const wchar_t *text, bool isDir) {
    TVINSERTSTRUCTW tvis = {};
    tvis.hParent         = hParent;
    tvis.hInsertAfter    = TVI_SORT;
    tvis.item.mask       = TVIF_TEXT | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvis.item.pszText    = (LPWSTR)text;
    tvis.item.iImage     = isDir ? 0 : 1;
    tvis.item.iSelectedImage = isDir ? 0 : 1;
    return TreeView_InsertItem(hTree, &tvis);
}

static void PopulateDir(HWND hTree, HTREEITEM hParent, const wchar_t *dir) {
    wchar_t pattern[MAX_PATH];
    wcscpy_s(pattern, dir);
    PathAppendW(pattern, L"*");

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(pattern, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    /* First pass: directories */
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) continue;
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;

        HTREEITEM hItem = AddTreeItem(hTree, hParent, fd.cFileName, true);
        wchar_t subdir[MAX_PATH];
        wcscpy_s(subdir, dir);
        PathAppendW(subdir, fd.cFileName);
        PopulateDir(hTree, hItem, subdir);
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);

    hFind = FindFirstFileW(pattern, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;

    /* Second pass: .nova / .nlf / .nui / .xml files */
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const wchar_t *ext = PathFindExtensionW(fd.cFileName);
        if (!ext) continue;
        bool show = (wcsicmp(ext, L".nova") == 0 ||
                     wcsicmp(ext, L".nlf")  == 0 ||
                     wcsicmp(ext, L".nui")  == 0 ||
                     wcsicmp(ext, L".xml")  == 0 ||
                     wcsicmp(ext, L".md")   == 0 ||
                     wcsicmp(ext, L".txt")  == 0);
        if (!show) continue;

        /* Store full path in item lParam */
        wchar_t *fullPath = new wchar_t[MAX_PATH];
        wcscpy_s(fullPath, MAX_PATH, dir);
        PathAppendW(fullPath, fd.cFileName);

        TVINSERTSTRUCTW tvis = {};
        tvis.hParent         = hParent;
        tvis.hInsertAfter    = TVI_SORT;
        tvis.item.mask       = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvis.item.pszText    = fd.cFileName;
        tvis.item.lParam     = (LPARAM)fullPath;
        tvis.item.iImage     = 1;
        tvis.item.iSelectedImage = 1;
        TreeView_InsertItem(hTree, &tvis);
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
}

void ExplorerCreate(HWND hParent, RECT rc) {
    g_hExplorer = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WC_TREEVIEWW, nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | TVS_HASLINES |
        TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        hParent, (HMENU)ID_EXPLORER,
        GetModuleHandleW(nullptr), nullptr);
    SendMessageW(g_hExplorer, WM_SETFONT,
        (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
    ExplorerApplyTheme();
}

void ExplorerResize(RECT rc) {
    if (g_hExplorer)
        SetWindowPos(g_hExplorer, nullptr,
            rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            SWP_NOZORDER | SWP_NOACTIVATE);
}

void ExplorerApplyTheme() {
    if (!g_hExplorer) return;
    TreeView_SetBkColor(g_hExplorer, g_theme.bg_panel);
    TreeView_SetTextColor(g_hExplorer, g_theme.fg_default);
}

void ExplorerSetRoot(const wchar_t *path) {
    if (!g_hExplorer) return;
    /* Clear existing items, freeing lParam strings */
    HTREEITEM hItem = TreeView_GetRoot(g_hExplorer);
    while (hItem) {
        TVITEMW item = {};
        item.mask  = TVIF_PARAM;
        item.hItem = hItem;
        TreeView_GetItem(g_hExplorer, &item);
        if (item.lParam) delete[] (wchar_t*)item.lParam;
        hItem = TreeView_GetNextSibling(g_hExplorer, hItem);
    }
    TreeView_DeleteAllItems(g_hExplorer);

    wchar_t name[MAX_PATH];
    wcscpy_s(name, path);
    PathStripPathW(name);
    HTREEITEM hRoot = AddTreeItem(g_hExplorer, TVI_ROOT, name, true);
    PopulateDir(g_hExplorer, hRoot, path);
    TreeView_Expand(g_hExplorer, hRoot, TVE_EXPAND);
}

void ExplorerOpenSelected() {
    if (!g_hExplorer) return;
    HTREEITEM hSel = TreeView_GetSelection(g_hExplorer);
    if (!hSel) return;
    TVITEMW item = {};
    item.mask  = TVIF_PARAM;
    item.hItem = hSel;
    TreeView_GetItem(g_hExplorer, &item);
    if (!item.lParam) return;
    const wchar_t *path = (const wchar_t*)item.lParam;
    if (GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) return;
    EditorOpenFile(path);
}
