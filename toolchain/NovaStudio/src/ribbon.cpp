/* ribbon.cpp – Custom Win32 Ribbon für Nova Studio
 * NPSPEC: NPSPEC-STUDIO-RIBBON-0001, NPSPEC-STUDIO-RIBBON-ARCHITECTURE-0001,
 *         NPSPEC-STUDIO-RIBBON-TABS-0001, NPSPEC-STUDIO-DARK-THEME-0001
 */
#include "studio.h"
#include <cmath>

HWND g_hRibbon = nullptr;

/* =========================================================================
 * Dimensionen (px)
 * ====================================================================== */
#define RBN_TAB_H    32   /* Höhe der Tab-Leiste */
#define RBN_CONT_H   70   /* Höhe des Inhaltsbereichs */
#define RBN_GRP_TTL  16   /* Gruppenbezeichnung unten */
#define RBN_BTN_AREA (RBN_CONT_H - RBN_GRP_TTL)  /* = 54 px */
#define RBN_L_W      54   /* Breite großer Button */
#define RBN_S_COL_W  90   /* Breite einer Kleinen-Button-Spalte */
#define RBN_S_H      18   /* Höhe kleiner Button (3 × 18 = 54) */
#define RBN_GRP_PAD   6   /* Abstand links/rechts in Gruppe */
#define RBN_GRP_SEP   8   /* Breite des Gruppen-Trennbereichs */
#define RBN_TAB_PAD  14   /* Polster je Seite eines Tab-Labels */
#define RBN_APP_W    52   /* „Nova"-App-Button-Breite */

/* =========================================================================
 * Icon-IDs
 * ====================================================================== */
enum RbnIcon {
    RI_NEW=0, RI_OPEN, RI_SAVE, RI_SAVEALL,
    RI_NEWPROJ, RI_OPENPROJ, RI_EXIT,
    RI_CUT, RI_COPY, RI_PASTE,
    RI_UNDO, RI_REDO,
    RI_FIND, RI_REPLACE, RI_GOTO, RI_SELECTALL,
    RI_COMMENT,
    RI_BUILD, RI_REBUILD, RI_RUN, RI_CLEAN,
    RI_EXPLORER, RI_OUTPUT,
    RI_ZOOMIN, RI_ZOOMOUT, RI_ZOOMRESET,
    RI_DARKMODE, RI_LIGHTMODE,
    RI_ABOUT,
    RI_DEBUG_START, RI_DEBUG_STOP,
    RI_GIT_COMMIT, RI_GIT_PUSH, RI_GIT_PULL, RI_GIT_MORE,
    RI_COUNT
};

/* =========================================================================
 * Datenstrukturen
 * ====================================================================== */
struct RBtn {
    UINT           cmd;
    const wchar_t *label;   /* Zeilenumbrüche mit \n */
    RbnIcon        icon;
    bool           large;
};

struct RGrp {
    const wchar_t       *title;
    std::vector<RBtn>    btns;
};

struct RTab {
    const wchar_t       *title;
    std::vector<RGrp>    groups;
};

/* =========================================================================
 * Zustand
 * ====================================================================== */
static std::vector<RTab> s_tabs;
static int               s_curTab   = 1;   /* Default: Start */
static int               s_hovTab   = -1;
static bool              s_tracking = false;
static int               s_ribbonW  = 800;

struct BtnInfo { RECT rc; UINT cmd; bool large; };
static std::vector<RECT>    s_tabRects;
static std::vector<BtnInfo> s_btnInfos;
static int                  s_hovBtn  = -1;
static RECT                 s_appRect;

/* =========================================================================
 * Tab-Definitionen (gemäß NPSPEC-Ribbon-Tabs)
 * ====================================================================== */
static void InitTabs() {
    s_tabs.clear();

    /* === Tab 0: Home – NPSPEC-STUDIO-RIBBON-HOME-0001 === */
    s_tabs.push_back({ L"Home", {
        { L"Project", {
            { IDM_FILE_NEW_PROJECT,   L"New\nProject",    RI_NEWPROJ,  true  },
            { IDM_FILE_NEW,           L"New\nFile",       RI_NEW,      true  },
            { IDM_FILE_OPEN,          L"Open",            RI_OPEN,     true  },
            { IDM_FILE_SAVE,          L"Save",            RI_SAVE,     true  },
            { IDM_FILE_SAVE_ALL,      L"Save All",        RI_SAVEALL,  true  },
        }},
        { L"Edit", {
            { IDM_EDIT_CUT,           L"Cut",             RI_CUT,      true  },
            { IDM_EDIT_COPY,          L"Copy",            RI_COPY,     true  },
            { IDM_EDIT_PASTE,         L"Paste",           RI_PASTE,    true  },
            { IDM_EDIT_UNDO,          L"Undo",            RI_UNDO,     true  },
            { IDM_EDIT_REDO,          L"Redo",            RI_REDO,     true  },
        }},
        { L"Build", {
            { IDM_BUILD_BUILD,        L"Build\nSolution", RI_BUILD,    true  },
            { IDM_BUILD_REBUILD,      L"Rebuild",         RI_REBUILD,  true  },
            { IDM_BUILD_CLEAN,        L"Clean",           RI_CLEAN,    true  },
        }},
        { L"Debug", {
            { IDM_DEBUG_START,        L"Start\nDebugging",RI_DEBUG_START, true },
            { IDM_DEBUG_START_NO_DBG, L"Start Without\nDebugging", RI_RUN, false },
            { IDM_DEBUG_STOP,         L"Stop",            RI_DEBUG_STOP, false },
        }},
        { L"Git", {
            { IDM_GIT_COMMIT,         L"Commit",          RI_GIT_COMMIT, true  },
            { IDM_GIT_PUSH,           L"Push",            RI_GIT_PUSH,   true  },
            { IDM_GIT_PULL,           L"Pull",            RI_GIT_PULL,   true  },
        }},
    }});

    /* === Tab 1: Edit === */
    s_tabs.push_back({ L"Edit", {
        { L"Edit", {
            { IDM_EDIT_FIND,          L"Find",            RI_FIND,     true  },
            { IDM_EDIT_REPLACE,       L"Replace",         RI_REPLACE,  true  },
            { IDM_EDIT_GOTO,          L"Go to\nLine",     RI_GOTO,     true  },
        }},
        { L"Selection", {
            { IDM_EDIT_SELECTALL,     L"Select\nAll",     RI_SELECTALL,true  },
        }},
        { L"Code", {
            { IDM_EDIT_COMMENT,       L"Toggle\nComment", RI_COMMENT,  true  },
        }},
    }});

    /* === Tab 2: View === */
    s_tabs.push_back({ L"View", {
        { L"Panels", {
            { IDM_VIEW_EXPLORER,      L"Explorer",        RI_EXPLORER, true  },
            { IDM_VIEW_OUTPUT,        L"Output",          RI_OUTPUT,   true  },
        }},
        { L"Zoom", {
            { IDM_VIEW_ZOOMIN,        L"Zoom In",         RI_ZOOMIN,   true  },
            { IDM_VIEW_ZOOMRESET,     L"Reset",           RI_ZOOMRESET,true  },
            { IDM_VIEW_ZOOMOUT,       L"Zoom Out",        RI_ZOOMOUT,  true  },
        }},
        { L"Theme", {
            { IDM_VIEW_DARKMODE,      L"Dark",            RI_DARKMODE, true  },
            { IDM_VIEW_LIGHTMODE,     L"Light",           RI_LIGHTMODE,true  },
        }},
    }});

    /* === Tab 3: Build === */
    s_tabs.push_back({ L"Build", {
        { L"Build", {
            { IDM_BUILD_BUILD,        L"Build\nSolution", RI_BUILD,    true  },
            { IDM_BUILD_REBUILD,      L"Rebuild",         RI_REBUILD,  true  },
            { IDM_BUILD_CLEAN,        L"Clean",           RI_CLEAN,    true  },
        }},
    }});

    /* === Tab 4: Debug === */
    s_tabs.push_back({ L"Debug", {
        { L"Run", {
            { IDM_DEBUG_START,        L"Start\nDebugging",RI_DEBUG_START, true },
            { IDM_DEBUG_START_NO_DBG, L"Start Without\nDebugging", RI_RUN, true },
            { IDM_DEBUG_STOP,         L"Stop",            RI_DEBUG_STOP,  true },
        }},
    }});

    /* === Tab 5: Git === */
    s_tabs.push_back({ L"Git", {
        { L"Repository", {
            { IDM_GIT_COMMIT,         L"Commit",          RI_GIT_COMMIT, true },
            { IDM_GIT_PUSH,           L"Push",            RI_GIT_PUSH,   true },
            { IDM_GIT_PULL,           L"Pull",            RI_GIT_PULL,   true },
        }},
    }});

    /* === Tab 6: Tools === */
    s_tabs.push_back({ L"Tools", {
        { L"Options", {
            { IDM_VIEW_DARKMODE,      L"Dark\nTheme",     RI_DARKMODE, true  },
            { IDM_VIEW_LIGHTMODE,     L"Light\nTheme",    RI_LIGHTMODE,true  },
            { IDM_HELP_ABOUT,         L"About\nNovaStudio", RI_ABOUT,  true  },
        }},
    }});
}

/* =========================================================================
 * Icon-Zeichnung (GDI, kein externer Ressource-Import)
 * ====================================================================== */
static void DrawRbnIcon(HDC hdc, int x, int y, int sz, RbnIcon ico, COLORREF fg) {
    HPEN   penT = CreatePen(PS_SOLID, sz/14 > 1 ? sz/14 : 1,   fg);
    HPEN   penK = CreatePen(PS_SOLID, sz/8  > 2 ? sz/8  : 2,   fg);
    HBRUSH brFg = CreateSolidBrush(fg);
    HBRUSH brNo = (HBRUSH)GetStockObject(NULL_BRUSH);

    HPEN   oldP = (HPEN)  SelectObject(hdc, penT);
    HBRUSH oldB = (HBRUSH)SelectObject(hdc, brFg);

#define SEL_T()  SelectObject(hdc, penT)
#define SEL_K()  SelectObject(hdc, penK)
#define SEL_FG() SelectObject(hdc, brFg)
#define SEL_NO() SelectObject(hdc, brNo)

    switch (ico) {

    case RI_NEW: {
        int fold = sz/4;
        POINT pts[5] = {
            {x,          y           },
            {x+sz-fold,  y           },
            {x+sz,       y+fold      },
            {x+sz,       y+sz        },
            {x,          y+sz        }
        };
        SEL_NO(); Polygon(hdc, pts, 5);
        MoveToEx(hdc, x+sz-fold, y,     nullptr); LineTo(hdc, x+sz-fold, y+fold);
        MoveToEx(hdc, x+sz-fold, y+fold,nullptr); LineTo(hdc, x+sz,      y+fold);
        break;
    }

    case RI_OPEN:
    case RI_OPENPROJ: {
        SEL_NO();
        /* Ordner-Körper */
        Rectangle(hdc, x+1, y+sz/4, x+sz-1, y+sz-1);
        /* Lasche oben */
        MoveToEx(hdc, x+1,         y+sz/4, nullptr);
        LineTo  (hdc, x+sz/3,      y+sz/4);
        LineTo  (hdc, x+sz/2,      y+sz/8);
        LineTo  (hdc, x+sz*2/3,    y+sz/8);
        LineTo  (hdc, x+sz*2/3,    y+sz/4);
        break;
    }

    case RI_SAVE: {
        SEL_FG();
        Rectangle(hdc, x, y, x+sz, y+sz);
        /* Label-Ausschnitt oben */
        COLORREF bg = (g_theme.kind==THEME_DARK) ? g_theme.bg_panel : RGB(0xF0,0xF0,0xF0);
        HBRUSH bBg = CreateSolidBrush(bg);
        SelectObject(hdc, bBg);
        Rectangle(hdc, x+sz/5, y+1, x+sz-sz/5, y+sz/3);
        DeleteObject(bBg);
        /* Speicher-Slot */
        SEL_FG();
        Rectangle(hdc, x+sz/4, y+sz/2, x+sz*3/4, y+sz-sz/8);
        break;
    }

    case RI_SAVEALL: {
        /* Zwei versetzte Disketten */
        int off = sz/5;
        SEL_NO(); Rectangle(hdc, x+off, y+off, x+sz, y+sz);
        SEL_FG(); Rectangle(hdc, x,     y,     x+sz-off, y+sz-off);
        COLORREF bg = (g_theme.kind==THEME_DARK) ? g_theme.bg_panel : RGB(0xF0,0xF0,0xF0);
        HBRUSH bBg = CreateSolidBrush(bg);
        SelectObject(hdc, bBg);
        Rectangle(hdc, x+sz/5, y+1, x+sz-sz/5-off, y+sz/3-off);
        DeleteObject(bBg);
        SEL_FG();
        Rectangle(hdc, x+sz/4, y+sz/2-off, x+sz*3/4-off, y+sz-sz/8-off);
        break;
    }

    case RI_NEWPROJ: {
        SEL_NO();
        Rectangle(hdc, x+1, y+sz/4+1, x+sz-1, y+sz-1);
        MoveToEx(hdc, x+1, y+sz/4, nullptr); LineTo(hdc, x+sz/3, y+sz/4);
        LineTo(hdc, x+sz/2, y+sz/8); LineTo(hdc, x+sz/2, y+sz/4);
        /* Plus */
        SEL_K();
        int mx = x+sz*2/3, my = y+sz*2/3;
        MoveToEx(hdc, mx, my-sz/5, nullptr); LineTo(hdc, mx, my+sz/5);
        MoveToEx(hdc, mx-sz/5, my, nullptr); LineTo(hdc, mx+sz/5, my);
        break;
    }

    case RI_EXIT: {
        /* Tür mit Pfeil */
        SEL_NO();
        Rectangle(hdc, x, y, x+sz*2/3, y+sz);
        SEL_K();
        int my = y+sz/2;
        MoveToEx(hdc, x+sz/2, my, nullptr); LineTo(hdc, x+sz, my);
        POINT arr[3] = {{x+sz-sz/4,my-sz/6},{x+sz-sz/4,my+sz/6},{x+sz,my}};
        SEL_FG(); Polygon(hdc, arr, 3);
        break;
    }

    case RI_CUT: {
        /* Schere */
        SEL_K();
        MoveToEx(hdc, x+sz/4, y+sz/4, nullptr); LineTo(hdc, x+sz*3/4, y+sz*3/4);
        MoveToEx(hdc, x+sz*3/4, y+sz/4, nullptr); LineTo(hdc, x+sz/4, y+sz*3/4);
        SEL_NO();
        Ellipse(hdc, x,        y+sz*3/5, x+sz*2/5, y+sz);
        Ellipse(hdc, x+sz*3/5, y+sz*3/5, x+sz,     y+sz);
        break;
    }

    case RI_COPY: {
        SEL_NO();
        Rectangle(hdc, x+sz/4, y,        x+sz,    y+sz*3/4);
        Rectangle(hdc, x,      y+sz/4,   x+sz*3/4,y+sz);
        break;
    }

    case RI_PASTE: {
        SEL_NO();
        Rectangle(hdc, x+sz/8, y+sz/5, x+sz-sz/8, y+sz);
        SEL_FG();
        /* Klemm-Clip oben */
        Rectangle(hdc, x+sz*2/5, y, x+sz*3/5, y+sz/5+1);
        break;
    }

    case RI_UNDO: {
        SEL_NO(); SEL_K();
        Arc(hdc, x+sz/6, y+sz/6, x+sz*5/6, y+sz*5/6,
            x+sz/2, y+sz/6, x+sz/6, y+sz/2);
        POINT ar[3] = {
            {x+sz/6, y+sz/2-sz/5},
            {x+sz/6, y+sz/2+sz/5},
            {x,      y+sz/2     }
        };
        SEL_FG(); Polygon(hdc, ar, 3);
        break;
    }

    case RI_REDO: {
        SEL_NO(); SEL_K();
        Arc(hdc, x+sz/6, y+sz/6, x+sz*5/6, y+sz*5/6,
            x+sz*5/6, y+sz/2, x+sz/2, y+sz/6);
        POINT ar[3] = {
            {x+sz*5/6, y+sz/2-sz/5},
            {x+sz*5/6, y+sz/2+sz/5},
            {x+sz,     y+sz/2     }
        };
        SEL_FG(); Polygon(hdc, ar, 3);
        break;
    }

    case RI_FIND:
    case RI_ZOOMIN:
    case RI_ZOOMOUT:
    case RI_ZOOMRESET: {
        int cr = sz*11/20;
        SEL_NO(); SEL_K();
        Ellipse(hdc, x, y, x+cr*2, y+cr*2);
        MoveToEx(hdc, x+cr*2-sz/6, y+cr*2-sz/6, nullptr);
        LineTo  (hdc, x+sz-1,      y+sz-1);
        if (ico == RI_ZOOMIN) {
            SEL_T();
            MoveToEx(hdc, x+cr,    y+sz/8, nullptr); LineTo(hdc, x+cr, y+cr*2-sz/8);
            MoveToEx(hdc, x+sz/8,  y+cr,   nullptr); LineTo(hdc, x+cr*2-sz/8, y+cr);
        } else if (ico == RI_ZOOMOUT) {
            SEL_T();
            MoveToEx(hdc, x+sz/8, y+cr, nullptr); LineTo(hdc, x+cr*2-sz/8, y+cr);
        } else if (ico == RI_ZOOMRESET) {
            SEL_T();
            MoveToEx(hdc, x+sz/8, y+cr-1, nullptr); LineTo(hdc, x+cr*2-sz/8, y+cr-1);
            MoveToEx(hdc, x+sz/8, y+cr+1, nullptr); LineTo(hdc, x+cr*2-sz/8, y+cr+1);
        }
        break;
    }

    case RI_REPLACE: {
        SEL_K();
        int q = sz/4;
        MoveToEx(hdc, x+q, y+q, nullptr); LineTo(hdc, x+sz-q, y+q);
        MoveToEx(hdc, x+q, y+sz-q, nullptr); LineTo(hdc, x+sz-q, y+sz-q);
        SEL_FG();
        POINT a1[3] = {{x+sz-q, y+q-sz/7},{x+sz-q, y+q+sz/7},{x+sz, y+q}};
        POINT a2[3] = {{x+q, y+sz-q-sz/7},{x+q, y+sz-q+sz/7},{x, y+sz-q}};
        Polygon(hdc, a1, 3); Polygon(hdc, a2, 3);
        break;
    }

    case RI_GOTO: {
        SEL_K();
        int my = y+sz/2;
        MoveToEx(hdc, x, my, nullptr); LineTo(hdc, x+sz*3/4, my);
        POINT ar[3] = {{x+sz*3/4,my-sz/6},{x+sz*3/4,my+sz/6},{x+sz,my}};
        SEL_FG(); Polygon(hdc, ar, 3);
        /* Senkrechter Strich rechts */
        SEL_T();
        MoveToEx(hdc, x+sz-sz/8, y+sz/4, nullptr);
        LineTo  (hdc, x+sz-sz/8, y+sz*3/4);
        break;
    }

    case RI_SELECTALL: {
        /* Gestrichelter Rahmen */
        HPEN dash = CreatePen(PS_DASH, 1, fg);
        SelectObject(hdc, dash);
        SEL_NO();
        Rectangle(hdc, x+sz/8, y+sz/8, x+sz-sz/8, y+sz-sz/8);
        SelectObject(hdc, penT);
        DeleteObject(dash);
        break;
    }

    case RI_COMMENT: {
        SEL_K();
        MoveToEx(hdc, x+sz/4,   y+sz*7/8, nullptr); LineTo(hdc, x+sz/2,   y+sz/8);
        MoveToEx(hdc, x+sz/2,   y+sz*7/8, nullptr); LineTo(hdc, x+sz*3/4, y+sz/8);
        break;
    }

    case RI_BUILD:
    case RI_REBUILD: {
        /* Hammer */
        SEL_FG();
        /* Kopf */
        Rectangle(hdc, x, y, x+sz*2/3, y+sz/3);
        /* Stiel */
        POINT sti[4] = {
            {x+sz*5/12-sz/12, y+sz/3   },
            {x+sz*5/12+sz/12, y+sz/3   },
            {x+sz-sz/8,       y+sz      },
            {x+sz-sz/8-sz/6,  y+sz      }
        };
        Polygon(hdc, sti, 4);
        if (ico == RI_REBUILD) {
            SEL_NO(); SEL_T();
            Arc(hdc, x+sz/2, y+sz/2, x+sz-1, y+sz-1,
                x+sz-1, y+sz*3/4, x+sz*3/4, y+sz/2);
        }
        break;
    }

    case RI_RUN: {
        POINT tri[3] = {
            {x+sz/6,   y+sz/8  },
            {x+sz*5/6, y+sz/2  },
            {x+sz/6,   y+sz*7/8}
        };
        SEL_FG(); Polygon(hdc, tri, 3);
        break;
    }

    case RI_CLEAN: {
        SEL_K();
        MoveToEx(hdc, x+sz/4, y+sz/4, nullptr); LineTo(hdc, x+sz*3/4, y+sz*3/4);
        SEL_T();
        for (int i = 0; i < 3; i++) {
            int bx = x+sz/2 + i*(sz/5);
            int by = y+sz/2 + i*(sz/5);
            MoveToEx(hdc, bx, by, nullptr); LineTo(hdc, bx+sz/5, by+sz/5);
        }
        break;
    }

    case RI_EXPLORER: {
        SEL_FG();
        int lx = x + sz/6;
        int rw = sz*2/5;
        Rectangle(hdc, lx, y, lx+rw, y+sz/5);
        SEL_T();
        MoveToEx(hdc, lx+rw/2, y+sz/5, nullptr);  LineTo(hdc, lx+rw/2, y+sz*7/10);
        MoveToEx(hdc, lx+rw/2, y+sz*2/5, nullptr); LineTo(hdc, lx+rw/2+sz/4, y+sz*2/5);
        MoveToEx(hdc, lx+rw/2, y+sz*7/10,nullptr); LineTo(hdc, lx+rw/2+sz/4, y+sz*7/10);
        SEL_FG();
        Rectangle(hdc, lx+rw/2+sz/4, y+sz/4,     lx+rw/2+sz*3/4, y+sz/2);
        Rectangle(hdc, lx+rw/2+sz/4, y+sz*9/16,  lx+rw/2+sz*3/4, y+sz*5/6);
        break;
    }

    case RI_OUTPUT: {
        SEL_T();
        int lh = sz/5;
        for (int i = 0; i < 4; i++) {
            int ly = y + sz/8 + i*lh;
            int lw = (i==0 || i==2) ? sz*3/4 : sz/2;
            MoveToEx(hdc, x+sz/8, ly, nullptr); LineTo(hdc, x+sz/8+lw, ly);
        }
        SEL_FG();
        Rectangle(hdc, x+sz/8, y+sz*3/4, x+sz/4, y+sz*7/8);
        break;
    }

    case RI_DARKMODE: {
        /* Halbmond */
        SEL_FG();
        Chord(hdc, x, y, x+sz, y+sz, x+sz/2, y, x+sz/2, y+sz);
        break;
    }

    case RI_LIGHTMODE: {
        /* Sonne */
        SEL_NO(); SEL_K();
        int r = sz/4, cx = x+sz/2, cy = y+sz/2;
        Ellipse(hdc, cx-r, cy-r, cx+r, cy+r);
        SEL_T();
        for (int a = 0; a < 8; a++) {
            double ang = a * 3.14159265 / 4.0;
            int x1 = cx + (int)((r+2)*cos(ang));
            int y1 = cy + (int)((r+2)*sin(ang));
            int x2 = cx + (int)((r*2)*cos(ang));
            int y2 = cy + (int)((r*2)*sin(ang));
            MoveToEx(hdc, x1, y1, nullptr); LineTo(hdc, x2, y2);
        }
        break;
    }

    case RI_ABOUT: {
        SEL_NO(); SEL_K();
        Ellipse(hdc, x, y, x+sz, y+sz);
        int dot = sz/8 > 2 ? sz/8 : 2;
        SEL_FG();
        Ellipse(hdc, x+sz/2-dot, y+sz/5, x+sz/2+dot, y+sz/5+dot*2);
        SEL_K();
        MoveToEx(hdc, x+sz/2, y+sz*2/5, nullptr);
        LineTo  (hdc, x+sz/2, y+sz*4/5);
        break;
    }

    case RI_DEBUG_START: {
        /* Solid green play triangle */
        HBRUSH greenBr = CreateSolidBrush(RGB(0x16, 0xC6, 0x0A));
        HBRUSH oldBr2  = (HBRUSH)SelectObject(hdc, greenBr);
        HPEN   greenPn = CreatePen(PS_SOLID, 1, RGB(0x0A, 0xA0, 0x05));
        HPEN   oldP2   = (HPEN)SelectObject(hdc, greenPn);
        POINT  tri[3] = {
            {x+sz/6,   y+sz/8  },
            {x+sz*5/6, y+sz/2  },
            {x+sz/6,   y+sz*7/8}
        };
        Polygon(hdc, tri, 3);
        SelectObject(hdc, oldBr2); DeleteObject(greenBr);
        SelectObject(hdc, oldP2);  DeleteObject(greenPn);
        break;
    }

    case RI_DEBUG_STOP: {
        /* Red square */
        HBRUSH redBr = CreateSolidBrush(RGB(0xE5, 0x1A, 0x1A));
        HBRUSH oldBr2 = (HBRUSH)SelectObject(hdc, redBr);
        Rectangle(hdc, x+sz/5, y+sz/5, x+sz*4/5, y+sz*4/5);
        SelectObject(hdc, oldBr2); DeleteObject(redBr);
        break;
    }

    case RI_GIT_COMMIT: {
        /* Circle with lines (commit node) */
        SEL_NO(); SEL_K();
        int r = sz/4, cx2 = x+sz/2, cy2 = y+sz/2;
        Ellipse(hdc, cx2-r, cy2-r, cx2+r, cy2+r);
        MoveToEx(hdc, cx2, y,    nullptr); LineTo(hdc, cx2, cy2-r);
        MoveToEx(hdc, cx2, cy2+r,nullptr); LineTo(hdc, cx2, y+sz);
        break;
    }

    case RI_GIT_PUSH: {
        /* Arrow up with base line */
        SEL_K();
        int mx = x+sz/2;
        MoveToEx(hdc, mx, y+sz, nullptr); LineTo(hdc, mx, y+sz/4);
        SEL_FG();
        POINT arr[3] = {{mx-sz/4, y+sz/3},{mx+sz/4, y+sz/3},{mx, y}};
        Polygon(hdc, arr, 3);
        SEL_T();
        MoveToEx(hdc, x+sz/8, y+sz*7/8, nullptr); LineTo(hdc, x+sz*7/8, y+sz*7/8);
        break;
    }

    case RI_GIT_PULL: {
        /* Arrow down with base line */
        SEL_K();
        int mx = x+sz/2;
        MoveToEx(hdc, mx, y, nullptr); LineTo(hdc, mx, y+sz*3/4);
        SEL_FG();
        POINT arr[3] = {{mx-sz/4, y+sz*2/3},{mx+sz/4, y+sz*2/3},{mx, y+sz}};
        Polygon(hdc, arr, 3);
        SEL_T();
        MoveToEx(hdc, x+sz/8, y+sz/8, nullptr); LineTo(hdc, x+sz*7/8, y+sz/8);
        break;
    }

    case RI_GIT_MORE: {
        /* Three dots */
        SEL_FG();
        int cy2 = y+sz/2, r2 = sz/8 > 2 ? sz/8 : 2;
        Ellipse(hdc, x+sz/8-r2,   cy2-r2, x+sz/8+r2,   cy2+r2);
        Ellipse(hdc, x+sz/2-r2,   cy2-r2, x+sz/2+r2,   cy2+r2);
        Ellipse(hdc, x+sz*7/8-r2, cy2-r2, x+sz*7/8+r2, cy2+r2);
        break;
    }

    default:
        SEL_NO();
        Rectangle(hdc, x+sz/4, y+sz/4, x+sz*3/4, y+sz*3/4);
        break;
    }

#undef SEL_T
#undef SEL_K
#undef SEL_FG
#undef SEL_NO

    SelectObject(hdc, oldP);
    SelectObject(hdc, oldB);
    DeleteObject(penT);
    DeleteObject(penK);
    DeleteObject(brFg);
}

/* =========================================================================
 * Layout berechnen (Tab-Rects + Button-Rects für aktuellen Tab)
 * ====================================================================== */
static void RebuildLayout() {
    if (s_tabs.empty()) return;
    s_tabRects.clear();
    s_btnInfos.clear();

    /* App-Button links */
    s_appRect = { 0, 0, RBN_APP_W, RBN_TAB_H };

    /* Tab-Labels */
    HDC hdc = GetDC(g_hRibbon);
    HFONT hf = (HFONT)SendMessageW(g_hRibbon, WM_GETFONT, 0, 0);
    if (!hf) hf = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT oldF = (HFONT)SelectObject(hdc, hf);

    int tx = RBN_APP_W;
    for (auto &tab : s_tabs) {
        SIZE sz = {};
        GetTextExtentPoint32W(hdc, tab.title, (int)wcslen(tab.title), &sz);
        int w = sz.cx + RBN_TAB_PAD * 2;
        RECT r = { tx, 0, tx+w, RBN_TAB_H };
        s_tabRects.push_back(r);
        tx += w;
    }
    SelectObject(hdc, oldF);
    ReleaseDC(g_hRibbon, hdc);

    /* Buttons des aktuellen Tabs */
    if (s_curTab < 0 || s_curTab >= (int)s_tabs.size()) return;
    auto &tab = s_tabs[s_curTab];

    int gx = RBN_GRP_PAD;
    int cy = RBN_TAB_H;

    for (auto &grp : tab.groups) {
        int startX = gx + RBN_GRP_PAD;
        int bx = startX;

        /* Buttons: large = volle Höhe, small = gestapelt (3×) */
        int si = 0;  /* small-button-Index im aktuellen Stapel */
        int colX = -1;

        for (int bi = 0; bi < (int)grp.btns.size(); bi++) {
            auto &btn = grp.btns[bi];
            if (btn.large) {
                /* Eventuell offener Kleinen-Stapel abschließen */
                if (si > 0) { bx = colX + RBN_S_COL_W; si = 0; colX = -1; }
                RECT rc = { bx, cy, bx + RBN_L_W, cy + RBN_BTN_AREA };
                s_btnInfos.push_back({ rc, btn.cmd, true });
                bx += RBN_L_W;
            } else {
                if (si == 0) { colX = bx; }
                RECT rc = {
                    colX,
                    cy + si * RBN_S_H,
                    colX + RBN_S_COL_W,
                    cy + si * RBN_S_H + RBN_S_H
                };
                s_btnInfos.push_back({ rc, btn.cmd, false });
                si++;
                if (si == 3) { bx = colX + RBN_S_COL_W; si = 0; colX = -1; }
            }
        }
        if (si > 0) bx = colX + RBN_S_COL_W;

        gx = bx + RBN_GRP_PAD + RBN_GRP_SEP;
    }
}

/* =========================================================================
 * Zeichnen
 * ====================================================================== */
static void PaintRibbon(HDC hdc, HWND hw) {
    RECT cl; GetClientRect(hw, &cl);
    int W = cl.right;
    int H = RBN_TAB_H + RBN_CONT_H;

    /* Hintergrund */
    COLORREF tabBg   = (g_theme.kind==THEME_DARK) ? RGB(0x1A,0x1A,0x2A) : RGB(0xDC,0xDC,0xEA);
    COLORREF contBg  = (g_theme.kind==THEME_DARK) ? RGB(0x22,0x22,0x36) : RGB(0xF0,0xF0,0xF8);
    COLORREF sepClr  = (g_theme.kind==THEME_DARK) ? RGB(0x38,0x38,0x52) : RGB(0xCC,0xCC,0xDD);
    COLORREF textClr = (g_theme.kind==THEME_DARK) ? RGB(0xCD,0xD6,0xF4) : RGB(0x1F,0x1F,0x3F);
    COLORREF dimClr  = (g_theme.kind==THEME_DARK) ? RGB(0x6C,0x70,0x86) : RGB(0x88,0x88,0xAA);
    COLORREF actClr  = (g_theme.kind==THEME_DARK) ? RGB(0x89,0xB4,0xFA) : RGB(0x00,0x5F,0xCF);
    COLORREF hovBg   = (g_theme.kind==THEME_DARK) ? RGB(0x30,0x30,0x48) : RGB(0xE4,0xE4,0xF4);
    COLORREF actTabBg= contBg;

    /* Tab-Leiste */
    HBRUSH brTab = CreateSolidBrush(tabBg);
    RECT tabBar = {0, 0, W, RBN_TAB_H};
    FillRect(hdc, &tabBar, brTab);
    DeleteObject(brTab);

    /* Inhaltsbereich */
    HBRUSH brCont = CreateSolidBrush(contBg);
    RECT contArea = {0, RBN_TAB_H, W, H};
    FillRect(hdc, &contArea, brCont);
    DeleteObject(brCont);

    /* Trennlinie Tab→Content */
    HPEN linPen = CreatePen(PS_SOLID, 1, sepClr);
    HPEN oldP   = (HPEN)SelectObject(hdc, linPen);
    MoveToEx(hdc, 0, RBN_TAB_H-1, nullptr);
    LineTo  (hdc, W, RBN_TAB_H-1);

    /* App-Button „Nova" */
    COLORREF appBg = (g_theme.kind==THEME_DARK) ? RGB(0x45,0x6E,0xC8) : RGB(0x00,0x66,0xCC);
    HBRUSH brApp = CreateSolidBrush(appBg);
    RECT appRc = s_appRect;
    FillRect(hdc, &appRc, brApp);
    DeleteObject(brApp);
    /* Nova-Text */
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0xFF,0xFF,0xFF));
    HFONT fntBold = CreateFontW(13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_SWISS, L"Segoe UI");
    HFONT oldF = (HFONT)SelectObject(hdc, fntBold);
    DrawTextW(hdc, L"Nova", 4, &appRc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    DeleteObject(SelectObject(hdc, oldF));

    /* Tab-Labels */
    HFONT fntUI = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH|FF_SWISS, L"Segoe UI");
    SelectObject(hdc, fntUI);

    for (int i = 0; i < (int)s_tabRects.size(); i++) {
        RECT tr = s_tabRects[i];
        bool active = (i == s_curTab);
        bool hover  = (i == s_hovTab);

        if (active) {
            HBRUSH ba = CreateSolidBrush(actTabBg);
            FillRect(hdc, &tr, ba);
            DeleteObject(ba);
            /* Akzent-Linie oben */
            HPEN acP = CreatePen(PS_SOLID, 2, actClr);
            SelectObject(hdc, acP);
            MoveToEx(hdc, tr.left,  tr.top, nullptr);
            LineTo  (hdc, tr.right, tr.top);
            DeleteObject(SelectObject(hdc, linPen));
            SetTextColor(hdc, actClr);
        } else if (hover) {
            HBRUSH bh = CreateSolidBrush(hovBg);
            FillRect(hdc, &tr, bh);
            DeleteObject(bh);
            SetTextColor(hdc, textClr);
        } else {
            SetTextColor(hdc, dimClr);
        }
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, s_tabs[i].title, -1, &tr, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    /* Button-Inhalte */
    if (s_curTab >= 0 && s_curTab < (int)s_tabs.size()) {
        auto &tab = s_tabs[s_curTab];

        /* Gruppen: Trennlinien + Titel */
        int gx = RBN_GRP_PAD;
        for (auto &grp : tab.groups) {
            /* Gruppen-Breite bestimmen aus s_btnInfos */
            int grpRight = gx;
            for (auto &bi : s_btnInfos) {
                if (bi.rc.left >= gx && bi.rc.right > grpRight)
                    grpRight = bi.rc.right;
            }
            grpRight += RBN_GRP_PAD;

            /* Gruppen-Titel */
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, dimClr);
            RECT grpTitleRc = { gx, RBN_TAB_H + RBN_BTN_AREA, grpRight, RBN_TAB_H + RBN_CONT_H };
            DrawTextW(hdc, grp.title, -1, &grpTitleRc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

            /* Trennlinie rechts */
            if (&grp != &tab.groups.back()) {
                SelectObject(hdc, linPen);
                int lx = grpRight + (RBN_GRP_SEP-1)/2;
                MoveToEx(hdc, lx, RBN_TAB_H+4, nullptr);
                LineTo  (hdc, lx, RBN_TAB_H+RBN_CONT_H-4);
            }

            gx = grpRight + RBN_GRP_SEP;
        }

        /* Buttons */
        int flatIdx = 0;
        for (int gi = 0; gi < (int)tab.groups.size(); gi++) {
            auto &grp = tab.groups[gi];
            for (int bi = 0; bi < (int)grp.btns.size(); bi++, flatIdx++) {
                if (flatIdx >= (int)s_btnInfos.size()) break;
                auto &btn = grp.btns[bi];
                auto &inf = s_btnInfos[flatIdx];
                RECT rc   = inf.rc;

                /* Hover-Hintergrund */
                if (flatIdx == s_hovBtn) {
                    HBRUSH bh = CreateSolidBrush(hovBg);
                    HBRUSH nb = (HBRUSH)SelectObject(hdc, bh);
                    SelectObject(hdc, GetStockObject(NULL_PEN));
                    Rectangle(hdc, rc.left, rc.top, rc.right+1, rc.bottom+1);
                    SelectObject(hdc, nb); DeleteObject(bh);
                    SelectObject(hdc, linPen);
                }

                /* Icon */
                COLORREF icFg = (flatIdx==s_hovBtn) ? actClr : textClr;
                if (inf.large) {
                    RECT iconRc = { rc.left, rc.top, rc.right, rc.top + RBN_BTN_AREA - 20 };
                    DrawRbnIcon(hdc, iconRc.left + (rc.right-rc.left-28)/2,
                                iconRc.top  + (iconRc.bottom-iconRc.top-28)/2,
                                28, btn.icon, icFg);
                    /* Label (2 Zeilen möglich) */
                    RECT lblRc = { rc.left, rc.top+RBN_BTN_AREA-20, rc.right, rc.top+RBN_BTN_AREA };
                    SetBkMode(hdc, TRANSPARENT);
                    SetTextColor(hdc, icFg);
                    /* Zeilenumbrüche via DT_CENTER + DT_WORDBREAK */
                    std::wstring lbl = btn.label;
                    for (auto &c : lbl) if (c==L'\n') c=L' ';
                    DrawTextW(hdc, lbl.c_str(), -1, &lblRc, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
                } else {
                    DrawRbnIcon(hdc, rc.left+2, rc.top+(RBN_S_H-14)/2, 14, btn.icon, icFg);
                    RECT lblRc = { rc.left+20, rc.top, rc.right-2, rc.bottom };
                    SetBkMode(hdc, TRANSPARENT);
                    SetTextColor(hdc, icFg);
                    std::wstring lbl = btn.label;
                    for (auto &c : lbl) if (c==L'\n') c=L' ';
                    DrawTextW(hdc, lbl.c_str(), -1, &lblRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
                }
            }
        }
    }

    /* Abschluß-Linie unten */
    SelectObject(hdc, linPen);
    MoveToEx(hdc, 0,   H-1, nullptr);
    LineTo  (hdc, W-1, H-1);

    SelectObject(hdc, oldP);
    DeleteObject(linPen);
    DeleteObject(fntUI);
}

/* =========================================================================
 * Fenster-Prozedur
 * ====================================================================== */
static LRESULT CALLBACK RibbonWndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hw, &ps);

        /* Double-Buffer */
        RECT cl; GetClientRect(hw, &cl);
        HDC mdc  = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, cl.right, cl.bottom);
        HBITMAP old = (HBITMAP)SelectObject(mdc, bmp);

        PaintRibbon(mdc, hw);

        BitBlt(hdc, 0, 0, cl.right, cl.bottom, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, old);
        DeleteObject(bmp);
        DeleteDC(mdc);

        EndPaint(hw, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int mx = LOWORD(lp), my = HIWORD(lp);

        /* App-Button */
        if (PtInRect(&s_appRect, {mx, my})) {
            /* Datei-Tab aktivieren */
            s_curTab = 0;
            RebuildLayout();
            InvalidateRect(hw, nullptr, FALSE);
            return 0;
        }

        /* Tab-Klick */
        for (int i = 0; i < (int)s_tabRects.size(); i++) {
            if (PtInRect(&s_tabRects[i], {mx, my})) {
                s_curTab = i;
                RebuildLayout();
                InvalidateRect(hw, nullptr, FALSE);
                return 0;
            }
        }

        /* Button-Klick → WM_COMMAND ans Elternfenster */
        for (int i = 0; i < (int)s_btnInfos.size(); i++) {
            if (PtInRect(&s_btnInfos[i].rc, {mx, my})) {
                SendMessageW(GetParent(hw), WM_COMMAND,
                             MAKEWPARAM(s_btnInfos[i].cmd, 0), 0);
                return 0;
            }
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        int mx = LOWORD(lp), my = HIWORD(lp);
        int newHovTab = -1, newHovBtn = -1;

        for (int i = 0; i < (int)s_tabRects.size(); i++)
            if (PtInRect(&s_tabRects[i], {mx, my})) { newHovTab = i; break; }

        for (int i = 0; i < (int)s_btnInfos.size(); i++)
            if (PtInRect(&s_btnInfos[i].rc, {mx, my})) { newHovBtn = i; break; }

        if (newHovTab != s_hovTab || newHovBtn != s_hovBtn) {
            s_hovTab = newHovTab;
            s_hovBtn = newHovBtn;
            InvalidateRect(hw, nullptr, FALSE);
        }

        if (!s_tracking) {
            TRACKMOUSEEVENT tme = {};
            tme.cbSize    = sizeof(tme);
            tme.dwFlags   = TME_LEAVE;
            tme.hwndTrack = hw;
            TrackMouseEvent(&tme);
            s_tracking = true;
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        s_hovTab = -1; s_hovBtn = -1; s_tracking = false;
        InvalidateRect(hw, nullptr, FALSE);
        return 0;

    case WM_SIZE:
        RebuildLayout();
        InvalidateRect(hw, nullptr, FALSE);
        return 0;
    }
    return DefWindowProcW(hw, msg, wp, lp);
}

/* =========================================================================
 * Öffentliche API
 * ====================================================================== */
void RibbonCreate(HWND hParent, int width) {
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = RibbonWndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"NovaRibbon";
    RegisterClassExW(&wc);   /* ignoriert Fehler bei Doppel-Registrierung */

    InitTabs();

    g_hRibbon = CreateWindowExW(
        0, L"NovaRibbon", nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        0, 0, width, RIBBON_HEIGHT,
        hParent, (HMENU)0, GetModuleHandleW(nullptr), nullptr);

    s_ribbonW = width;
    RebuildLayout();
}

void RibbonResize(int x, int y, int width) {
    if (!g_hRibbon) return;
    SetWindowPos(g_hRibbon, nullptr, x, y, width, RIBBON_HEIGHT,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    s_ribbonW = width;
    RebuildLayout();
}

void RibbonApplyTheme() {
    if (g_hRibbon) InvalidateRect(g_hRibbon, nullptr, FALSE);
}

void RibbonSetTab(int idx) {
    if (idx < 0 || idx >= (int)s_tabs.size()) return;
    s_curTab = idx;
    RebuildLayout();
    if (g_hRibbon) InvalidateRect(g_hRibbon, nullptr, FALSE);
}
