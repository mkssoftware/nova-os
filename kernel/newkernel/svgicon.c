/* NovaOS C-Kernel - Nova SVG Icon Renderer (siehe svgicon.h fuer Scope/
   NPSPEC-Bezug). Reine Software-Implementierung, keine FPU/libm-Nutzung
   (Fixed-Point Q16.16 ueberall, Sinus/Cosinus per Bhaskara-I-Approximation -
   siehe svgicon_sin_deg/svgicon_cos_deg), keine Heap-Allokation (statische
   Pools je Aufruf als Stackvariable, siehe svgicon_render). */
#include "svgicon.h"
#include "render2d.h"

typedef int32_t svg_fx_t;
#define FX_SHIFT 16
#define FX_ONE ((svg_fx_t)(1 << FX_SHIFT))

static svg_fx_t fx_from_int(int32_t v) { return (svg_fx_t)(v << FX_SHIFT); }

static svg_fx_t fx_mul(svg_fx_t a, svg_fx_t b) {
    return (svg_fx_t)(((int64_t)a * (int64_t)b) >> FX_SHIFT);
}

/* 64-Bit-Dividend durch 32-Bit-Divisor, Quotient MUSS in 32 Bit passen
   (bei allen hier vorkommenden Q16.16-Werten/Icon-Koordinaten immer der
   Fall). Direkt per IDIV-Inline-Assembly statt des C-Operators '/' auf
   int64_t/int64_t - letzterer wuerde GCC zu einem Aufruf von __divdi3
   aus libgcc zwingen, das im Freestanding-Build NICHT gelinkt ist
   (exakt dasselbe Muster wie die bereits dokumentierte __udivdi3-Falle
   bei 64-Bit-Divisionen in time.c, siehe Projektgedaechtnis). IDIV
   selbst verarbeitet nativ einen 64-Bit-Dividend (EDX:EAX) geteilt durch
   einen 32-Bit-Divisor - exakt dieser Fall, keine Bibliotheksfunktion
   noetig. */
static int32_t fx_idiv64_32(int64_t dividend, int32_t divisor) {
    int32_t quotient;
    int32_t lo = (int32_t)(dividend & 0xFFFFFFFFu);
    int32_t hi = (int32_t)(dividend >> 32);
    __asm__ ("idivl %[divisor]"
             : "=a"(quotient)
             : "a"(lo), "d"(hi), [divisor] "r"(divisor)
             : "cc");
    return quotient;
}

static svg_fx_t fx_div(svg_fx_t a, svg_fx_t b) {
    if (b == 0) return 0;
    return (svg_fx_t)fx_idiv64_32((int64_t)a << FX_SHIFT, b);
}

/* Bhaskara-I-Approximation (|Fehler| < 0.0017 im gesamten Bereich) - ohne
   FPU/libm, nur ganzzahlige Grad-Aufloesung (fuer Icon-Transformationen in
   UI-Groessen voellig ausreichend, siehe svgicon.h). */
static svg_fx_t svgicon_sin_deg(int32_t deg) {
    int32_t d = deg % 360;
    if (d < 0) d += 360;
    int32_t sign = 1;
    if (d > 180) { d -= 180; sign = -1; }
    int32_t num = 4 * d * (180 - d);
    int32_t den = 40500 - d * (180 - d);
    int32_t v = fx_idiv64_32((int64_t)num << FX_SHIFT, den);
    return (svg_fx_t)(sign * v);
}

static svg_fx_t svgicon_cos_deg(int32_t deg) { return svgicon_sin_deg(deg + 90); }

/* ---- 2x3 affine Transformationsmatrix: x'=a*x+c*y+e, y'=b*x+d*y+f ---- */
typedef struct { svg_fx_t a, b, c, d, e, f; } svgicon_matrix_t;

static const svgicon_matrix_t SVGICON_IDENTITY = { FX_ONE, 0, 0, FX_ONE, 0, 0 };

static svgicon_matrix_t matrix_multiply(const svgicon_matrix_t *p, const svgicon_matrix_t *c) {
    svgicon_matrix_t r;
    r.a = fx_mul(p->a, c->a) + fx_mul(p->c, c->b);
    r.b = fx_mul(p->b, c->a) + fx_mul(p->d, c->b);
    r.c = fx_mul(p->a, c->c) + fx_mul(p->c, c->d);
    r.d = fx_mul(p->b, c->c) + fx_mul(p->d, c->d);
    r.e = fx_mul(p->a, c->e) + fx_mul(p->c, c->f) + p->e;
    r.f = fx_mul(p->b, c->e) + fx_mul(p->d, c->f) + p->f;
    return r;
}

static void matrix_apply(const svgicon_matrix_t *m, svg_fx_t x, svg_fx_t y, svg_fx_t *ox, svg_fx_t *oy) {
    *ox = fx_mul(m->a, x) + fx_mul(m->c, y) + m->e;
    *oy = fx_mul(m->b, x) + fx_mul(m->d, y) + m->f;
}

/* ---- kleine Byte-Helfer (kein libc im Freestanding-Build) ---- */
static int is_space_byte(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
static int is_digit_byte(char c) { return c >= '0' && c <= '9'; }

static int mem_eq(const char *a, const char *b, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) if (a[i] != b[i]) return 0;
    return 1;
}

static void skip_ws_commas(const char **p, const char *end) {
    while (*p < end && (is_space_byte(**p) || **p == ',')) (*p)++;
}

/* Deterministischer Zahlenparser ohne FPU: Vorzeichen + Ganzteil + bis zu
   8 Nachkommastellen (ausreichend fuer Q16.16-Genauigkeit), kein
   wissenschaftliches Format (Exponent -> Fehler, in allen 16 kuratierten
   Icons nicht vorhanden). */
static int parse_number(const char **p, const char *end, svg_fx_t *out) {
    const char *s = *p;
    skip_ws_commas(&s, end);
    int neg = 0;
    if (s < end && (*s == '+' || *s == '-')) { neg = (*s == '-'); s++; }
    if (s >= end || !(is_digit_byte(*s) || *s == '.')) return -1;
    int64_t intpart = 0;
    int had_digit = 0;
    while (s < end && is_digit_byte(*s)) { intpart = intpart * 10 + (*s - '0'); s++; had_digit = 1; }
    int64_t frac = 0;
    int fracdigits = 0;
    if (s < end && *s == '.') {
        s++;
        while (s < end && is_digit_byte(*s)) {
            if (fracdigits < 8) { frac = frac * 10 + (*s - '0'); fracdigits++; }
            s++; had_digit = 1;
        }
    }
    if (!had_digit) return -1;
    if (s < end && (*s == 'e' || *s == 'E')) return -1; /* Exponentialformat nicht unterstuetzt */
    int64_t scaled = intpart << FX_SHIFT;
    if (fracdigits > 0) {
        int32_t denom = 1;
        for (int i = 0; i < fracdigits; ++i) denom *= 10;
        scaled += fx_idiv64_32(frac << FX_SHIFT, denom);
    }
    if (neg) scaled = -scaled;
    *out = (svg_fx_t)scaled;
    *p = s;
    return 0;
}

/* ---- Farben: #RRGGBB, #RRGGBBAA, none, currentColor, var(--nova-*) ---- */
#define SVGICON_COLOR_NONE          0x00000000u /* Alpha 0 = kein Fill */
#define SVGICON_COLOR_CURRENT_MARK  0x00000001u
#define SVGICON_COLOR_THEME_MARK    0x00000002u

static int hex_nibble(char c, uint32_t *out) {
    if (c >= '0' && c <= '9') { *out = (uint32_t)(c - '0'); return 0; }
    if (c >= 'a' && c <= 'f') { *out = (uint32_t)(c - 'a' + 10); return 0; }
    if (c >= 'A' && c <= 'F') { *out = (uint32_t)(c - 'A' + 10); return 0; }
    return -1;
}

static int hex_byte(const char *p, uint32_t *out) {
    uint32_t hi, lo;
    if (hex_nibble(p[0], &hi) != 0 || hex_nibble(p[1], &lo) != 0) return -1;
    *out = (hi << 4) | lo;
    return 0;
}

static int parse_color(const char *val, uint32_t len, uint32_t *out_rgba) {
    if (len == 4 && mem_eq(val, "none", 4)) { *out_rgba = SVGICON_COLOR_NONE; return 0; }
    if (len == 12 && mem_eq(val, "currentColor", 12)) { *out_rgba = SVGICON_COLOR_CURRENT_MARK; return 0; }
    if (len >= 11 && mem_eq(val, "var(--nova-", 11)) { *out_rgba = SVGICON_COLOR_THEME_MARK; return 0; }
    if (len == 7 && val[0] == '#') {
        uint32_t r, g, b;
        if (hex_byte(val + 1, &r) != 0 || hex_byte(val + 3, &g) != 0 || hex_byte(val + 5, &b) != 0) return -1;
        *out_rgba = 0xFF000000u | (r << 16) | (g << 8) | b;
        return 0;
    }
    if (len == 9 && val[0] == '#') {
        uint32_t r, g, b, a;
        if (hex_byte(val + 1, &r) != 0 || hex_byte(val + 3, &g) != 0 ||
            hex_byte(val + 5, &b) != 0 || hex_byte(val + 7, &a) != 0) return -1;
        *out_rgba = (a << 24) | (r << 16) | (g << 8) | b;
        return 0;
    }
    return -1; /* rgb()/rgba()/benannte Farben nicht unterstuetzt */
}

/* ---- transform="..." -> Matrix ---- */
static int parse_transform_list(const char *val, uint32_t len, svgicon_matrix_t *out) {
    const char *p = val, *end = val + len;
    svgicon_matrix_t acc = SVGICON_IDENTITY;
    while (1) {
        skip_ws_commas(&p, end);
        if (p >= end) break;
        const char *name_start = p;
        while (p < end && *p != '(') p++;
        if (p >= end) return -1;
        uint32_t name_len = (uint32_t)(p - name_start);
        p++; /* '(' */
        svg_fx_t vals[6];
        uint32_t n = 0;
        while (p < end && *p != ')' && n < 6) {
            if (parse_number(&p, end, &vals[n]) != 0) return -1;
            n++;
            skip_ws_commas(&p, end);
        }
        if (p >= end || *p != ')') return -1;
        p++;
        svgicon_matrix_t m = SVGICON_IDENTITY;
        if (name_len == 9 && mem_eq(name_start, "translate", 9)) {
            if (n < 1) return -1;
            m.e = vals[0]; m.f = (n >= 2) ? vals[1] : 0;
        } else if (name_len == 5 && mem_eq(name_start, "scale", 5)) {
            if (n < 1) return -1;
            m.a = vals[0]; m.d = (n >= 2) ? vals[1] : vals[0];
        } else if (name_len == 6 && mem_eq(name_start, "rotate", 6)) {
            if (n < 1) return -1;
            int32_t deg = (int32_t)(vals[0] >> FX_SHIFT);
            svg_fx_t s = svgicon_sin_deg(deg), c = svgicon_cos_deg(deg);
            svgicon_matrix_t rot = { c, s, -s, c, 0, 0 };
            if (n >= 3) {
                svgicon_matrix_t t1 = SVGICON_IDENTITY, t2 = SVGICON_IDENTITY;
                t1.e = vals[1]; t1.f = vals[2];
                t2.e = -vals[1]; t2.f = -vals[2];
                svgicon_matrix_t tmp = matrix_multiply(&t1, &rot);
                m = matrix_multiply(&tmp, &t2);
            } else {
                m = rot;
            }
        } else if (name_len == 6 && mem_eq(name_start, "matrix", 6)) {
            if (n < 6) return -1;
            m.a = vals[0]; m.b = vals[1]; m.c = vals[2]; m.d = vals[3]; m.e = vals[4]; m.f = vals[5];
        } else if (name_len == 5 && mem_eq(name_start, "skewX", 5)) {
            if (n < 1) return -1;
            int32_t deg = (int32_t)(vals[0] >> FX_SHIFT);
            svg_fx_t t = fx_div(svgicon_sin_deg(deg), svgicon_cos_deg(deg));
            m.c = t;
        } else if (name_len == 5 && mem_eq(name_start, "skewY", 5)) {
            if (n < 1) return -1;
            int32_t deg = (int32_t)(vals[0] >> FX_SHIFT);
            svg_fx_t t = fx_div(svgicon_sin_deg(deg), svgicon_cos_deg(deg));
            m.b = t;
        } else {
            return -1;
        }
        acc = matrix_multiply(&acc, &m);
        skip_ws_commas(&p, end);
    }
    *out = acc;
    return 0;
}

/* ---- Pfadkommando-Pool / Szene ---- */
#define SVGICON_MAX_PATH_CMDS 160u
#define SVGICON_MAX_SHAPES 16u
#define SVGICON_MAX_EDGES 768u
#define SVGICON_MAX_G_DEPTH 8u
#define SVGICON_BEZIER_STEPS 8u
#define SVGICON_MAX_XML_BYTES 4096u

typedef enum { SVGICON_MOVETO, SVGICON_LINETO, SVGICON_CUBICTO, SVGICON_CLOSE } svgicon_cmdtype_t;

typedef struct {
    uint8_t type;
    svg_fx_t x, y;
    svg_fx_t c1x, c1y, c2x, c2y;
} svgicon_cmd_t;

typedef struct {
    uint32_t cmd_start, cmd_count;
    uint32_t fill_rgba;
    uint8_t fill_rule; /* 0 = nonzero, 1 = evenodd */
    uint8_t has_fill;
} svgicon_shape_t;

typedef struct {
    svgicon_cmd_t cmds[SVGICON_MAX_PATH_CMDS];
    uint32_t cmd_count;
    svgicon_shape_t shapes[SVGICON_MAX_SHAPES];
    uint32_t shape_count;
    svg_fx_t vb_x, vb_y, vb_w, vb_h;
    uint32_t element_count;
} svgicon_scene_t;

static int scene_push_cmd(svgicon_scene_t *sc, const svgicon_cmd_t *cmd) {
    if (sc->cmd_count >= SVGICON_MAX_PATH_CMDS) return -1;
    sc->cmds[sc->cmd_count++] = *cmd;
    return 0;
}

static int emit_moveto(svgicon_scene_t *sc, const svgicon_matrix_t *m, svg_fx_t x, svg_fx_t y) {
    svgicon_cmd_t c; c.type = SVGICON_MOVETO;
    matrix_apply(m, x, y, &c.x, &c.y);
    return scene_push_cmd(sc, &c);
}
static int emit_lineto(svgicon_scene_t *sc, const svgicon_matrix_t *m, svg_fx_t x, svg_fx_t y) {
    svgicon_cmd_t c; c.type = SVGICON_LINETO;
    matrix_apply(m, x, y, &c.x, &c.y);
    return scene_push_cmd(sc, &c);
}
static int emit_cubicto(svgicon_scene_t *sc, const svgicon_matrix_t *m,
                         svg_fx_t c1x, svg_fx_t c1y, svg_fx_t c2x, svg_fx_t c2y, svg_fx_t ex, svg_fx_t ey) {
    svgicon_cmd_t c; c.type = SVGICON_CUBICTO;
    matrix_apply(m, c1x, c1y, &c.c1x, &c.c1y);
    matrix_apply(m, c2x, c2y, &c.c2x, &c.c2y);
    matrix_apply(m, ex, ey, &c.x, &c.y);
    return scene_push_cmd(sc, &c);
}
static int emit_close(svgicon_scene_t *sc) {
    svgicon_cmd_t c; c.type = SVGICON_CLOSE;
    return scene_push_cmd(sc, &c);
}

static int is_cmd_letter(char ch) {
    char u = (ch >= 'a' && ch <= 'z') ? (char)(ch - 'a' + 'A') : ch;
    return u == 'M' || u == 'L' || u == 'H' || u == 'V' || u == 'C' || u == 'Z';
}

/* Parst das 'd'-Attribut (M/L/H/V/C/Z, absolut+relativ, implizite
   Kommando-Wiederholung) direkt in den Pfadkommando-Pool, unter
   Anwendung der aktuell gueltigen (g-vererbten) Transformationsmatrix. */
static int parse_path_d(svgicon_scene_t *sc, const svgicon_matrix_t *m, const char *d, uint32_t len) {
    const char *p = d, *end = d + len;
    svg_fx_t curx = 0, cury = 0, startx = 0, starty = 0;
    char cmd = 0;
    int have_cmd = 0;
    int have_subpath = 0;
    while (1) {
        skip_ws_commas(&p, end);
        if (p >= end) break;
        if (is_cmd_letter(*p)) { cmd = *p; have_cmd = 1; p++; skip_ws_commas(&p, end); }
        if (!have_cmd) return -1;
        char base = (cmd >= 'a' && cmd <= 'z') ? (char)(cmd - 'a' + 'A') : cmd;
        int rel = (cmd >= 'a' && cmd <= 'z');
        if (base == 'M') {
            svg_fx_t x, y;
            if (parse_number(&p, end, &x) != 0) return -1;
            if (parse_number(&p, end, &y) != 0) return -1;
            if (rel && have_subpath) { x += curx; y += cury; }
            curx = x; cury = y; startx = x; starty = y;
            if (emit_moveto(sc, m, x, y) != 0) return -1;
            have_subpath = 1;
            cmd = rel ? 'l' : 'L';
        } else if (base == 'L') {
            svg_fx_t x, y;
            if (parse_number(&p, end, &x) != 0) return -1;
            if (parse_number(&p, end, &y) != 0) return -1;
            if (rel) { x += curx; y += cury; }
            curx = x; cury = y;
            if (emit_lineto(sc, m, x, y) != 0) return -1;
        } else if (base == 'H') {
            svg_fx_t x;
            if (parse_number(&p, end, &x) != 0) return -1;
            if (rel) x += curx;
            curx = x;
            if (emit_lineto(sc, m, x, cury) != 0) return -1;
        } else if (base == 'V') {
            svg_fx_t y;
            if (parse_number(&p, end, &y) != 0) return -1;
            if (rel) y += cury;
            cury = y;
            if (emit_lineto(sc, m, curx, y) != 0) return -1;
        } else if (base == 'C') {
            svg_fx_t c1x, c1y, c2x, c2y, ex, ey;
            if (parse_number(&p, end, &c1x) != 0) return -1;
            if (parse_number(&p, end, &c1y) != 0) return -1;
            if (parse_number(&p, end, &c2x) != 0) return -1;
            if (parse_number(&p, end, &c2y) != 0) return -1;
            if (parse_number(&p, end, &ex) != 0) return -1;
            if (parse_number(&p, end, &ey) != 0) return -1;
            if (rel) { c1x += curx; c1y += cury; c2x += curx; c2y += cury; ex += curx; ey += cury; }
            if (emit_cubicto(sc, m, c1x, c1y, c2x, c2y, ex, ey) != 0) return -1;
            curx = ex; cury = ey;
        } else if (base == 'Z') {
            if (!have_subpath) return -1;
            if (emit_close(sc) != 0) return -1;
            curx = startx; cury = starty;
            have_cmd = 0;
        } else {
            return -1; /* S/Q/T/A nicht unterstuetzt */
        }
    }
    return 0;
}

/* rect/circle/ellipse/line/polyline/polygon -> aequivalente Pfadkommandos.
   Kreisbogen-Naeherung ueber die Standard-Kappa-Konstante (4*(sqrt(2)-1)/3
   ~ 0.5522847498, vorab in Q16.16 = 36195 umgerechnet). */
#define SVGICON_KAPPA_FX 36195

static int emit_ellipse(svgicon_scene_t *sc, const svgicon_matrix_t *m,
                         svg_fx_t cx, svg_fx_t cy, svg_fx_t rx, svg_fx_t ry) {
    svg_fx_t kx = fx_mul(rx, SVGICON_KAPPA_FX);
    svg_fx_t ky = fx_mul(ry, SVGICON_KAPPA_FX);
    if (emit_moveto(sc, m, cx + rx, cy) != 0) return -1;
    if (emit_cubicto(sc, m, cx + rx, cy + ky, cx + kx, cy + ry, cx, cy + ry) != 0) return -1;
    if (emit_cubicto(sc, m, cx - kx, cy + ry, cx - rx, cy + ky, cx - rx, cy) != 0) return -1;
    if (emit_cubicto(sc, m, cx - rx, cy - ky, cx - kx, cy - ry, cx, cy - ry) != 0) return -1;
    if (emit_cubicto(sc, m, cx + kx, cy - ry, cx + rx, cy - ky, cx + rx, cy) != 0) return -1;
    return emit_close(sc);
}

static int emit_rect(svgicon_scene_t *sc, const svgicon_matrix_t *m,
                      svg_fx_t x, svg_fx_t y, svg_fx_t w, svg_fx_t h, svg_fx_t rx, svg_fx_t ry) {
    if (rx <= 0 || ry <= 0) {
        if (emit_moveto(sc, m, x, y) != 0) return -1;
        if (emit_lineto(sc, m, x + w, y) != 0) return -1;
        if (emit_lineto(sc, m, x + w, y + h) != 0) return -1;
        if (emit_lineto(sc, m, x, y + h) != 0) return -1;
        return emit_close(sc);
    }
    svg_fx_t kx = fx_mul(rx, SVGICON_KAPPA_FX), ky = fx_mul(ry, SVGICON_KAPPA_FX);
    if (emit_moveto(sc, m, x + rx, y) != 0) return -1;
    if (emit_lineto(sc, m, x + w - rx, y) != 0) return -1;
    if (emit_cubicto(sc, m, x + w - rx + kx, y, x + w, y + ry - ky, x + w, y + ry) != 0) return -1;
    if (emit_lineto(sc, m, x + w, y + h - ry) != 0) return -1;
    if (emit_cubicto(sc, m, x + w, y + h - ry + ky, x + w - rx + kx, y + h, x + w - rx, y + h) != 0) return -1;
    if (emit_lineto(sc, m, x + rx, y + h) != 0) return -1;
    if (emit_cubicto(sc, m, x + rx - kx, y + h, x, y + h - ry + ky, x, y + h - ry) != 0) return -1;
    if (emit_lineto(sc, m, x, y + ry) != 0) return -1;
    if (emit_cubicto(sc, m, x, y + ry - ky, x + rx - kx, y, x + rx, y) != 0) return -1;
    return emit_close(sc);
}

static int emit_points_list(svgicon_scene_t *sc, const svgicon_matrix_t *m,
                             const char *val, uint32_t len, int close) {
    const char *p = val, *end = val + len;
    int first = 1;
    while (p < end) {
        skip_ws_commas(&p, end);
        if (p >= end) break;
        svg_fx_t x, y;
        if (parse_number(&p, end, &x) != 0) return -1;
        skip_ws_commas(&p, end);
        if (parse_number(&p, end, &y) != 0) return -1;
        if (first) { if (emit_moveto(sc, m, x, y) != 0) return -1; first = 0; }
        else { if (emit_lineto(sc, m, x, y) != 0) return -1; }
    }
    if (first) return -1; /* kein Punkt */
    if (close) return emit_close(sc);
    return 0;
}

/* ---- XML-Tokenizer (SAX-artig, O(n), keine Baumstruktur) ---- */
typedef struct {
    const char *name; uint32_t name_len;
    const char *attrs; uint32_t attrs_len;
    int self_closing;
    int is_end_tag;
} svgicon_tag_t;

static int next_tag(const char **p, const char *end, svgicon_tag_t *out) {
    const char *s = *p;
    while (1) {
        while (s < end && *s != '<') s++;
        if (s >= end) return -1;
        s++;
        if (s < end && *s == '?') { while (s + 1 < end && !(s[0] == '?' && s[1] == '>')) s++; s += 2; continue; }
        if (s < end && *s == '!') return -2; /* DTD/Kommentar/CDATA -> Fehler */
        break;
    }
    out->is_end_tag = 0;
    if (s < end && *s == '/') { out->is_end_tag = 1; s++; }
    const char *name_start = s;
    while (s < end && !is_space_byte(*s) && *s != '>' && *s != '/') s++;
    out->name = name_start;
    out->name_len = (uint32_t)(s - name_start);
    if (out->name_len == 0 || out->name_len > 32) return -3;
    const char *attrs_start = s;
    while (s < end && *s != '>') s++;
    if (s >= end) return -4;
    uint32_t raw_len = (uint32_t)(s - attrs_start);
    out->self_closing = (raw_len > 0 && attrs_start[raw_len - 1] == '/');
    out->attrs = attrs_start;
    out->attrs_len = out->self_closing ? raw_len - 1 : raw_len;
    s++;
    *p = s;
    return 0;
}

static int next_attr(const char **p, const char *end, const char **name, uint32_t *name_len,
                      const char **val, uint32_t *val_len) {
    const char *s = *p;
    while (s < end && is_space_byte(*s)) s++;
    if (s >= end) return -1;
    const char *ns = s;
    while (s < end && *s != '=' && !is_space_byte(*s)) s++;
    *name = ns; *name_len = (uint32_t)(s - ns);
    if (*name_len == 0) return -1;
    while (s < end && is_space_byte(*s)) s++;
    if (s >= end || *s != '=') return -2;
    s++;
    while (s < end && is_space_byte(*s)) s++;
    if (s >= end || (*s != '"' && *s != '\'')) return -3;
    char quote = *s; s++;
    const char *vs = s;
    while (s < end && *s != quote) s++;
    if (s >= end) return -4;
    *val = vs; *val_len = (uint32_t)(s - vs);
    s++;
    *p = s;
    return 0;
}

static int attr_eq(const char *name, uint32_t len, const char *lit) {
    uint32_t litlen = 0;
    while (lit[litlen] != 0) litlen++;
    return len == litlen && mem_eq(name, lit, len);
}

/* Vererbbarer Zustand ueber svg/g/path-Verschachtelung (max. Tiefe
   SVGICON_MAX_G_DEPTH, siehe Kommentarblock in svgicon.h). */
typedef struct {
    svgicon_matrix_t matrix;
    uint32_t fill_rgba;
    uint8_t has_fill;
    uint8_t fill_rule;
} svgicon_style_t;

static int apply_shape_attrs(const char *attrs, uint32_t attrs_len, svgicon_style_t *style, int *out_has_transform,
                              svgicon_matrix_t *out_local_transform) {
    const char *p = attrs, *end = attrs + attrs_len;
    const char *name; uint32_t name_len; const char *val; uint32_t val_len;
    *out_has_transform = 0;
    while (next_attr(&p, end, &name, &name_len, &val, &val_len) == 0) {
        if (attr_eq(name, name_len, "fill")) {
            uint32_t rgba;
            if (parse_color(val, val_len, &rgba) != 0) return -1;
            style->fill_rgba = rgba;
            style->has_fill = 1;
        } else if (attr_eq(name, name_len, "fill-rule")) {
            if (val_len == 7 && mem_eq(val, "evenodd", 7)) style->fill_rule = 1;
            else if (val_len == 7 && mem_eq(val, "nonzero", 7)) style->fill_rule = 0;
            else return -1;
        } else if (attr_eq(name, name_len, "transform")) {
            if (parse_transform_list(val, val_len, out_local_transform) != 0) return -1;
            *out_has_transform = 1;
        } else if (attr_eq(name, name_len, "stroke") || attr_eq(name, name_len, "stroke-width") ||
                   attr_eq(name, name_len, "stroke-linecap") || attr_eq(name, name_len, "stroke-linejoin") ||
                   attr_eq(name, name_len, "opacity") || attr_eq(name, name_len, "xmlns") ||
                   attr_eq(name, name_len, "id")) {
            /* geparst/validiert, aber fuer das reine Fill-Rendering dieser
               Sitzung ohne Wirkung - siehe svgicon.h Scope-Hinweis. */
        } else {
            /* d/x/y/width/height/cx/cy/r/rx/ry/x1/y1/x2/y2/points/viewBox
               werden vom jeweiligen Elementhandler selbst gelesen, nicht
               hier - hier nur die auf ALLEN Shapes gueltigen Attribute. */
        }
    }
    return 0;
}

/* Liest EIN benanntes numerisches Attribut aus attrs - fuer die
   elementspezifischen Geometrie-Attribute (x/y/width/.../points/d). */
static int find_attr(const char *attrs, uint32_t attrs_len, const char *lit, const char **val, uint32_t *val_len) {
    const char *p = attrs, *end = attrs + attrs_len;
    const char *name; uint32_t name_len; const char *v; uint32_t vlen;
    while (next_attr(&p, end, &name, &name_len, &v, &vlen) == 0) {
        if (attr_eq(name, name_len, lit)) { *val = v; *val_len = vlen; return 0; }
    }
    return -1;
}

static int find_num_attr(const char *attrs, uint32_t attrs_len, const char *lit, svg_fx_t *out, svg_fx_t def) {
    const char *v; uint32_t vlen;
    if (find_attr(attrs, attrs_len, lit, &v, &vlen) != 0) { *out = def; return 0; }
    const char *p = v, *end = v + vlen;
    return parse_number(&p, end, out);
}

static int validate_attrs_whitelist(const char *attrs, uint32_t attrs_len) {
    static const char *const allowed[] = {
        "viewBox", "fill", "fill-rule", "stroke", "stroke-width", "stroke-linecap",
        "stroke-linejoin", "transform", "d", "x", "y", "width", "height", "cx", "cy",
        "r", "rx", "ry", "x1", "y1", "x2", "y2", "points", "opacity", "xmlns", "id"
    };
    const char *p = attrs, *end = attrs + attrs_len;
    const char *name; uint32_t name_len; const char *val; uint32_t vlen;
    while (next_attr(&p, end, &name, &name_len, &val, &vlen) == 0) {
        int ok = 0;
        for (uint32_t i = 0; i < sizeof(allowed) / sizeof(allowed[0]); ++i) {
            if (attr_eq(name, name_len, allowed[i])) { ok = 1; break; }
        }
        if (!ok) return -1;
    }
    return 0;
}

/* Kernparser: einmaliger O(n)-Durchlauf ueber die XML-Bytes, baut direkt
   die Szene auf (keine Zwischenbaumstruktur). Rueckgabe 0 = Erfolg. */
static int parse_svg_scene(const char *source, uint32_t source_size, svgicon_scene_t *sc) {
    if (source_size == 0 || source_size > SVGICON_MAX_XML_BYTES) return -1;
    sc->cmd_count = 0; sc->shape_count = 0; sc->element_count = 0;
    sc->vb_x = sc->vb_y = 0; sc->vb_w = sc->vb_h = 0;
    int have_viewbox = 0;
    svgicon_style_t stack[SVGICON_MAX_G_DEPTH];
    uint32_t depth = 0;
    stack[0].matrix = SVGICON_IDENTITY;
    stack[0].has_fill = 1;
    stack[0].fill_rgba = 0xFF000000u; /* SVG-Default: opakes Schwarz */
    stack[0].fill_rule = 0;
    int svg_seen = 0;

    const char *p = source, *end = source + source_size;
    for (uint32_t ch = 0; ch < source_size; ++ch) {
        char c = source[ch];
        if (c == '&') return -1; /* Entities nicht unterstuetzt */
    }

    while (p < end) {
        svgicon_tag_t tag;
        int rc = next_tag(&p, end, &tag);
        if (rc == -1) break; /* kein weiteres Tag - normales Ende */
        if (rc != 0) return -1;

        if (tag.is_end_tag) {
            if (depth == 0) return -1;
            depth--;
            continue;
        }

        int is_svg = (tag.name_len == 3 && mem_eq(tag.name, "svg", 3));
        int is_g = (tag.name_len == 1 && mem_eq(tag.name, "g", 1));
        int is_path = (tag.name_len == 4 && mem_eq(tag.name, "path", 4));
        int is_rect = (tag.name_len == 4 && mem_eq(tag.name, "rect", 4));
        int is_circle = (tag.name_len == 6 && mem_eq(tag.name, "circle", 6));
        int is_ellipse = (tag.name_len == 7 && mem_eq(tag.name, "ellipse", 7));
        int is_line = (tag.name_len == 4 && mem_eq(tag.name, "line", 4));
        int is_polyline = (tag.name_len == 8 && mem_eq(tag.name, "polyline", 8));
        int is_polygon = (tag.name_len == 7 && mem_eq(tag.name, "polygon", 7));

        if (!is_svg && !is_g && !is_path && !is_rect && !is_circle && !is_ellipse &&
            !is_line && !is_polyline && !is_polygon) {
            return -1; /* unbekanntes/nicht unterstuetztes Element (defs/use/clipPath/...) */
        }
        if (validate_attrs_whitelist(tag.attrs, tag.attrs_len) != 0) return -1;

        sc->element_count++;
        if (sc->element_count > 64u) return -1; /* NPSPEC-BOOTRESOURCE-0008 Abschnitt 8: max. 64 Elemente */

        if (is_svg) {
            if (svg_seen) return -1; /* genau ein Root-Element */
            svg_seen = 1;
            const char *vb; uint32_t vblen;
            if (find_attr(tag.attrs, tag.attrs_len, "viewBox", &vb, &vblen) == 0) {
                const char *vp = vb, *vend = vb + vblen;
                if (parse_number(&vp, vend, &sc->vb_x) != 0) return -1;
                if (parse_number(&vp, vend, &sc->vb_y) != 0) return -1;
                if (parse_number(&vp, vend, &sc->vb_w) != 0) return -1;
                if (parse_number(&vp, vend, &sc->vb_h) != 0) return -1;
                have_viewbox = 1;
            } else {
                svg_fx_t w, h;
                if (find_num_attr(tag.attrs, tag.attrs_len, "width", &w, -1) != 0) return -1;
                if (find_num_attr(tag.attrs, tag.attrs_len, "height", &h, -1) != 0) return -1;
                if (w <= 0 || h <= 0) return -1;
                sc->vb_x = 0; sc->vb_y = 0; sc->vb_w = w; sc->vb_h = h;
                have_viewbox = 1;
            }
            if (sc->vb_w <= 0 || sc->vb_h <= 0) return -1;
            int has_transform; svgicon_matrix_t local_m = SVGICON_IDENTITY;
            if (apply_shape_attrs(tag.attrs, tag.attrs_len, &stack[0], &has_transform, &local_m) != 0) return -1;
            if (has_transform) stack[0].matrix = matrix_multiply(&stack[0].matrix, &local_m);
            if (!tag.self_closing) depth++;
            continue;
        }

        if (!have_viewbox) return -1; /* svg muss vor jeder Shape gesehen worden sein */
        if (depth == 0) return -1; /* alles ausser svg muss verschachtelt sein */

        svgicon_style_t cur = stack[depth - 1];
        int has_transform = 0; svgicon_matrix_t local_m = SVGICON_IDENTITY;
        if (apply_shape_attrs(tag.attrs, tag.attrs_len, &cur, &has_transform, &local_m) != 0) return -1;
        svgicon_matrix_t effective = has_transform ? matrix_multiply(&stack[depth - 1].matrix, &local_m)
                                                    : stack[depth - 1].matrix;
        cur.matrix = effective;

        if (is_g) {
            if (depth >= SVGICON_MAX_G_DEPTH) return -1;
            stack[depth] = cur;
            depth++;
            if (tag.self_closing) depth--;
            continue;
        }

        uint32_t cmd_start = sc->cmd_count;
        int shape_rc = -1;
        if (is_path) {
            const char *d; uint32_t dlen;
            if (find_attr(tag.attrs, tag.attrs_len, "d", &d, &dlen) == 0) {
                shape_rc = parse_path_d(sc, &cur.matrix, d, dlen);
            }
        } else if (is_rect) {
            svg_fx_t x, y, w, h, rx, ry;
            if (find_num_attr(tag.attrs, tag.attrs_len, "x", &x, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "y", &y, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "width", &w, -1) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "height", &h, -1) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "rx", &rx, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "ry", &ry, rx) == 0 &&
                w > 0 && h > 0) {
                shape_rc = emit_rect(sc, &cur.matrix, x, y, w, h, rx, ry);
            }
        } else if (is_circle) {
            svg_fx_t cx, cy, r;
            if (find_num_attr(tag.attrs, tag.attrs_len, "cx", &cx, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "cy", &cy, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "r", &r, -1) == 0 && r > 0) {
                shape_rc = emit_ellipse(sc, &cur.matrix, cx, cy, r, r);
            }
        } else if (is_ellipse) {
            svg_fx_t cx, cy, rx, ry;
            if (find_num_attr(tag.attrs, tag.attrs_len, "cx", &cx, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "cy", &cy, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "rx", &rx, -1) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "ry", &ry, -1) == 0 && rx > 0 && ry > 0) {
                shape_rc = emit_ellipse(sc, &cur.matrix, cx, cy, rx, ry);
            }
        } else if (is_line) {
            svg_fx_t x1, y1, x2, y2;
            if (find_num_attr(tag.attrs, tag.attrs_len, "x1", &x1, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "y1", &y1, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "x2", &x2, 0) == 0 &&
                find_num_attr(tag.attrs, tag.attrs_len, "y2", &y2, 0) == 0) {
                shape_rc = (emit_moveto(sc, &cur.matrix, x1, y1) == 0 &&
                            emit_lineto(sc, &cur.matrix, x2, y2) == 0) ? 0 : -1;
            }
        } else { /* polyline/polygon */
            const char *pts; uint32_t ptslen;
            if (find_attr(tag.attrs, tag.attrs_len, "points", &pts, &ptslen) == 0) {
                shape_rc = emit_points_list(sc, &cur.matrix, pts, ptslen, 1);
            }
        }
        if (shape_rc != 0) return -1;

        if (sc->cmd_count > cmd_start) {
            if (sc->shape_count >= SVGICON_MAX_SHAPES) return -1;
            svgicon_shape_t *shape = &sc->shapes[sc->shape_count++];
            shape->cmd_start = cmd_start;
            shape->cmd_count = sc->cmd_count - cmd_start;
            shape->fill_rgba = cur.fill_rgba;
            shape->has_fill = cur.has_fill && cur.fill_rgba != SVGICON_COLOR_NONE;
            shape->fill_rule = cur.fill_rule;
        }

        if (!tag.self_closing) depth++; /* path/rect/.../ ohne Kinder, aber gueltiges </...> muss folgen */
    }
    if (!svg_seen) return -1;
    if (depth != 0) return -1; /* unausgeglichene Tags */
    return 0;
}

/* ---- Rasterisierung: 2x2-Supersampling, Winding-Number je Sample ---- */
typedef struct { svg_fx_t x0, y0, x1, y1; } svgicon_edge_t;

static uint32_t flatten_shape_edges(const svgicon_scene_t *sc, const svgicon_shape_t *shape,
                                     const svgicon_matrix_t *vb_scale,
                                     svgicon_edge_t *edges, uint32_t max_edges) {
    uint32_t n = 0;
    svg_fx_t startx = 0, starty = 0, curx = 0, cury = 0;
    int have_point = 0;
    for (uint32_t i = 0; i < shape->cmd_count; ++i) {
        const svgicon_cmd_t *cmd = &sc->cmds[shape->cmd_start + i];
        svg_fx_t tx, ty;
        matrix_apply(vb_scale, cmd->x, cmd->y, &tx, &ty);
        if (cmd->type == SVGICON_MOVETO) {
            if (have_point && (curx != startx || cury != starty)) {
                if (n >= max_edges) return n;
                edges[n].x0 = curx; edges[n].y0 = cury; edges[n].x1 = startx; edges[n].y1 = starty; n++;
            }
            curx = tx; cury = ty; startx = tx; starty = ty;
            have_point = 1;
        } else if (cmd->type == SVGICON_LINETO) {
            if (n >= max_edges) return n;
            edges[n].x0 = curx; edges[n].y0 = cury; edges[n].x1 = tx; edges[n].y1 = ty; n++;
            curx = tx; cury = ty;
        } else if (cmd->type == SVGICON_CUBICTO) {
            svg_fx_t c1x, c1y, c2x, c2y;
            matrix_apply(vb_scale, cmd->c1x, cmd->c1y, &c1x, &c1y);
            matrix_apply(vb_scale, cmd->c2x, cmd->c2y, &c2x, &c2y);
            svg_fx_t px = curx, py = cury;
            for (uint32_t step = 1; step <= SVGICON_BEZIER_STEPS; ++step) {
                svg_fx_t t = fx_div(fx_from_int((int32_t)step), fx_from_int((int32_t)SVGICON_BEZIER_STEPS));
                svg_fx_t u = FX_ONE - t;
                svg_fx_t uu = fx_mul(u, u), tt = fx_mul(t, t);
                svg_fx_t uuu = fx_mul(uu, u), ttt = fx_mul(tt, t);
                svg_fx_t uut3 = fx_mul(fx_mul(uu, t), fx_from_int(3));
                svg_fx_t utt3 = fx_mul(fx_mul(u, tt), fx_from_int(3));
                svg_fx_t qx = fx_mul(uuu, curx) + fx_mul(uut3, c1x) + fx_mul(utt3, c2x) + fx_mul(ttt, tx);
                svg_fx_t qy = fx_mul(uuu, cury) + fx_mul(uut3, c1y) + fx_mul(utt3, c2y) + fx_mul(ttt, ty);
                if (n >= max_edges) return n;
                edges[n].x0 = px; edges[n].y0 = py; edges[n].x1 = qx; edges[n].y1 = qy; n++;
                px = qx; py = qy;
            }
            curx = tx; cury = ty;
        } else { /* SVGICON_CLOSE */
            if (have_point && (curx != startx || cury != starty)) {
                if (n >= max_edges) return n;
                edges[n].x0 = curx; edges[n].y0 = cury; edges[n].x1 = startx; edges[n].y1 = starty; n++;
            }
            curx = startx; cury = starty;
        }
    }
    if (have_point && (curx != startx || cury != starty)) {
        if (n < max_edges) { edges[n].x0 = curx; edges[n].y0 = cury; edges[n].x1 = startx; edges[n].y1 = starty; n++; }
    }
    return n;
}

static int sample_inside(const svgicon_edge_t *edges, uint32_t n, svg_fx_t px, svg_fx_t py, int evenodd) {
    int32_t winding = 0;
    uint32_t crossings = 0;
    for (uint32_t i = 0; i < n; ++i) {
        svg_fx_t y0 = edges[i].y0, y1 = edges[i].y1;
        if (y0 == y1) continue;
        int upward = (y1 > y0);
        svg_fx_t lo = upward ? y0 : y1, hi = upward ? y1 : y0;
        if (py < lo || py >= hi) continue;
        svg_fx_t t = fx_div(py - y0, y1 - y0);
        svg_fx_t ix = edges[i].x0 + fx_mul(t, edges[i].x1 - edges[i].x0);
        if (ix > px) { winding += upward ? 1 : -1; crossings++; }
    }
    return evenodd ? (int)(crossings & 1u) : (winding != 0);
}

static uint32_t blend_over(uint32_t dst_rgba, uint32_t src_rgba) {
    uint32_t sa = (src_rgba >> 24) & 0xFFu;
    if (sa == 0) return dst_rgba;
    if (sa == 255) return src_rgba;
    uint32_t da = (dst_rgba >> 24) & 0xFFu;
    uint32_t sr = (src_rgba >> 16) & 0xFFu, sg = (src_rgba >> 8) & 0xFFu, sb = src_rgba & 0xFFu;
    uint32_t dr = (dst_rgba >> 16) & 0xFFu, dg = (dst_rgba >> 8) & 0xFFu, db = dst_rgba & 0xFFu;
    uint32_t out_a = sa + (da * (255u - sa)) / 255u;
    if (out_a == 0) return 0;
    uint32_t out_r = (sr * sa + dr * da * (255u - sa) / 255u) / out_a;
    uint32_t out_g = (sg * sa + dg * da * (255u - sa) / 255u) / out_a;
    uint32_t out_b = (sb * sa + db * da * (255u - sa) / 255u) / out_a;
    return (out_a << 24) | ((out_r & 0xFFu) << 16) | ((out_g & 0xFFu) << 8) | (out_b & 0xFFu);
}

static void rasterize_scene(const svgicon_scene_t *sc, uint32_t target_size, uint32_t theme_rgb_override,
                             svgicon_bitmap_t *icon) {
    icon->width = target_size;
    icon->height = target_size;
    for (uint32_t i = 0; i < target_size * target_size; ++i) icon->pixels[i] = 0x00000000u;

    svgicon_matrix_t vb_scale = SVGICON_IDENTITY;
    vb_scale.a = fx_div(fx_from_int((int32_t)target_size), sc->vb_w);
    vb_scale.d = fx_div(fx_from_int((int32_t)target_size), sc->vb_h);
    vb_scale.e = -fx_mul(sc->vb_x, vb_scale.a);
    vb_scale.f = -fx_mul(sc->vb_y, vb_scale.d);

    static svgicon_edge_t edges[SVGICON_MAX_EDGES];

    for (uint32_t s = 0; s < sc->shape_count; ++s) {
        const svgicon_shape_t *shape = &sc->shapes[s];
        if (!shape->has_fill) continue;
        uint32_t fill = shape->fill_rgba;
        if (fill == SVGICON_COLOR_CURRENT_MARK || fill == SVGICON_COLOR_THEME_MARK) {
            fill = (theme_rgb_override != SVGICON_NO_THEME_OVERRIDE) ? theme_rgb_override : 0xFF000000u;
        }
        uint32_t edge_count = flatten_shape_edges(sc, shape, &vb_scale, edges, SVGICON_MAX_EDGES);
        if (edge_count == 0) continue;

        for (uint32_t y = 0; y < target_size; ++y) {
            svg_fx_t py0 = fx_from_int((int32_t)y) + (FX_ONE / 4);
            svg_fx_t py1 = fx_from_int((int32_t)y) + (FX_ONE * 3 / 4);
            for (uint32_t x = 0; x < target_size; ++x) {
                svg_fx_t px0 = fx_from_int((int32_t)x) + (FX_ONE / 4);
                svg_fx_t px1 = fx_from_int((int32_t)x) + (FX_ONE * 3 / 4);
                int hits = 0;
                hits += sample_inside(edges, edge_count, px0, py0, shape->fill_rule);
                hits += sample_inside(edges, edge_count, px1, py0, shape->fill_rule);
                hits += sample_inside(edges, edge_count, px0, py1, shape->fill_rule);
                hits += sample_inside(edges, edge_count, px1, py1, shape->fill_rule);
                if (hits == 0) continue;
                uint32_t coverage = (uint32_t)(hits * 255) / 4u;
                uint32_t src_a = ((fill >> 24) & 0xFFu) * coverage / 255u;
                uint32_t src = (src_a << 24) | (fill & 0x00FFFFFFu);
                uint32_t idx = y * target_size + x;
                icon->pixels[idx] = blend_over(icon->pixels[idx], src);
            }
        }
    }
}

/* ---- Oeffentliche API ---- */
static uint32_t g_svgicon_cache_hits = 0, g_svgicon_cache_misses = 0;

typedef struct {
    uint32_t cache_key;
    uint32_t target_size;
    uint32_t theme_rgb_override;
    int used;
    svgicon_bitmap_t bitmap;
} svgicon_cache_slot_t;

static svgicon_cache_slot_t g_svgicon_cache[SVGICON_CACHE_SLOTS];
static uint32_t g_svgicon_cache_next_victim = 0;

void svgicon_initialize(void) {
    g_svgicon_cache_hits = 0;
    g_svgicon_cache_misses = 0;
    g_svgicon_cache_next_victim = 0;
    for (uint32_t i = 0; i < SVGICON_CACHE_SLOTS; ++i) g_svgicon_cache[i].used = 0;
}

/* `svgicon_scene_t` (Pfadkommando-/Shape-Pool, einige KiB) und
   `svgicon_bitmap_t` (bis zu 64x64x4 Byte = 16 KiB) sind absichtlich NICHT
   als gewoehnliche Stackvariablen abgelegt: der Freestanding-Build linkt
   kein libgcc, und MinGW fuegt bei Stackframes oberhalb der Windows-
   Seitengroesse automatisch einen Aufruf von `__chkstk_ms` ein (Stack-
   Probing fuer Guard-Pages) - hier unaufloesbar, kein libgcc vorhanden.
   `static` legt diese Puffer stattdessen in .bss ab (gleiches Muster wie
   `edges[]` in rasterize_scene). Einzige Konsequenz: svgicon_validate/
   svgicon_render/svgicon_render_cached sind NICHT reentrant - im
   aktuellen Kernel (kein echter Taskwechsel, siehe scheduler.c) ohne
   praktische Auswirkung, wie bereits bei anderen Modulen dieser
   Sitzung (input.c/syscall.c) dokumentiert. */
static svgicon_scene_t g_svgicon_scene;
static svgicon_bitmap_t g_svgicon_scratch;

int svgicon_validate(const char *source, uint32_t source_size) {
    return parse_svg_scene(source, source_size, &g_svgicon_scene) == 0;
}

int svgicon_render(const char *source, uint32_t source_size, uint32_t target_size,
                    uint32_t theme_rgb_override, svgicon_bitmap_t *icon) {
    if (target_size < SVGICON_MIN_TARGET_SIZE || target_size > SVGICON_MAX_TARGET_SIZE) return -1;
    if (parse_svg_scene(source, source_size, &g_svgicon_scene) != 0) return -1;
    rasterize_scene(&g_svgicon_scene, target_size, theme_rgb_override, icon);
    return 0;
}

/* Manuelle Kopie statt Struct-Zuweisung (`dst = *src`) - letztere wuerde
   GCC fuer ein 16-KiB-Objekt zu einem Aufruf von `memcpy` kompilieren,
   das (wie __chkstk_ms oben) im Freestanding-Build nicht existiert. Eine
   simple Schleife erzeugt dagegen nur gewoehnliche mov-Instruktionen. */
static void svgicon_copy_bitmap(svgicon_bitmap_t *dst, const svgicon_bitmap_t *src) {
    dst->width = src->width;
    dst->height = src->height;
    uint32_t n = src->width * src->height;
    for (uint32_t i = 0; i < n; ++i) dst->pixels[i] = src->pixels[i];
}

int svgicon_render_cached(uint32_t cache_key, const char *source, uint32_t source_size,
                           uint32_t target_size, uint32_t theme_rgb_override, svgicon_bitmap_t **out) {
    for (uint32_t i = 0; i < SVGICON_CACHE_SLOTS; ++i) {
        svgicon_cache_slot_t *slot = &g_svgicon_cache[i];
        if (slot->used && slot->cache_key == cache_key && slot->target_size == target_size &&
            slot->theme_rgb_override == theme_rgb_override) {
            g_svgicon_cache_hits++;
            *out = &slot->bitmap;
            return 0;
        }
    }
    g_svgicon_cache_misses++;
    /* In einen gemeinsamen statischen Scratch-Puffer rendern (NICHT direkt
       in den spaeter gewaehlten Opfer-Slot) - schlaegt das Rendern fehl,
       bleibt dessen bisheriger, noch gueltiger Cache-Eintrag unangetastet. */
    if (svgicon_render(source, source_size, target_size, theme_rgb_override, &g_svgicon_scratch) != 0) return -1;
    svgicon_cache_slot_t *victim = (svgicon_cache_slot_t *)0;
    for (uint32_t i = 0; i < SVGICON_CACHE_SLOTS; ++i) {
        if (!g_svgicon_cache[i].used) { victim = &g_svgicon_cache[i]; break; }
    }
    if (victim == (svgicon_cache_slot_t *)0) {
        victim = &g_svgicon_cache[g_svgicon_cache_next_victim];
        g_svgicon_cache_next_victim = (g_svgicon_cache_next_victim + 1u) % SVGICON_CACHE_SLOTS;
    }
    victim->used = 1;
    victim->cache_key = cache_key;
    victim->target_size = target_size;
    victim->theme_rgb_override = theme_rgb_override;
    svgicon_copy_bitmap(&victim->bitmap, &g_svgicon_scratch);
    *out = &victim->bitmap;
    return 0;
}

uint32_t svgicon_cache_hits(void) { return g_svgicon_cache_hits; }
uint32_t svgicon_cache_misses(void) { return g_svgicon_cache_misses; }

void svgicon_blit_blend(void *surf_ptr, uint32_t dst_x, uint32_t dst_y, const svgicon_bitmap_t *icon) {
    render2d_surface_t *surf = (render2d_surface_t *)surf_ptr;
    for (uint32_t y = 0; y < icon->height; ++y) {
        uint32_t ty = dst_y + y;
        if (ty >= surf->height) break;
        for (uint32_t x = 0; x < icon->width; ++x) {
            uint32_t tx = dst_x + x;
            if (tx >= surf->width) continue;
            uint32_t src = icon->pixels[y * icon->width + x];
            uint32_t sa = (src >> 24) & 0xFFu;
            if (sa == 0) continue;
            uint32_t sr = (src >> 16) & 0xFFu, sg = (src >> 8) & 0xFFu, sb = src & 0xFFu;
            uint32_t *dst_px = &surf->pixels[ty * surf->stride + tx];
            if (sa == 255) { *dst_px = (sr << 16) | (sg << 8) | sb; continue; }
            uint32_t dr = (*dst_px >> 16) & 0xFFu, dg = (*dst_px >> 8) & 0xFFu, db = *dst_px & 0xFFu;
            uint32_t out_r = (sr * sa + dr * (255u - sa)) / 255u;
            uint32_t out_g = (sg * sa + dg * (255u - sa)) / 255u;
            uint32_t out_b = (sb * sa + db * (255u - sa)) / 255u;
            *dst_px = (out_r << 16) | (out_g << 8) | out_b;
        }
    }
}

/* ---- Selbsttest (Abschnitt 21: path/circle/rect/polygon/Transform/
   Theme/Skalierung/Alpha/beschaedigte XML/ungueltige Elemente) ---- */
static int bitmap_pixel_opaque(const svgicon_bitmap_t *icon, uint32_t x, uint32_t y) {
    return ((icon->pixels[y * icon->width + x] >> 24) & 0xFFu) > 0u;
}

int svgicon_self_test(void) {
    /* static statt Stackvariable - siehe Kommentar bei g_svgicon_scene/
       g_svgicon_scratch (__chkstk_ms-Vermeidung). */
    static svgicon_bitmap_t icon;

    /* 1) einfacher path (Dreieck-Quadrat), Mittelpunkt muss gefuellt sein,
       Ecke ausserhalb des viewBox-Bereichs leer. */
    static const char svg_path[] =
        "<svg viewBox='0 0 10 10'><path d='M1 1L9 1L9 9L1 9Z' fill='#112233'/></svg>";
    if (svgicon_render(svg_path, sizeof(svg_path) - 1, 16u, SVGICON_NO_THEME_OVERRIDE, &icon) != 0) return -1;
    if (!bitmap_pixel_opaque(&icon, 8u, 8u)) return -1;
    if (bitmap_pixel_opaque(&icon, 0u, 0u)) return -1;
    if ((icon.pixels[8u * 16u + 8u] & 0x00FFFFFFu) != 0x112233u) return -1;

    /* 2) circle */
    static const char svg_circle[] = "<svg viewBox='0 0 20 20'><circle cx='10' cy='10' r='8' fill='#FF0000'/></svg>";
    if (svgicon_render(svg_circle, sizeof(svg_circle) - 1, 24u, SVGICON_NO_THEME_OVERRIDE, &icon) != 0) return -1;
    if (!bitmap_pixel_opaque(&icon, 12u, 12u)) return -1; /* Zentrum */
    if (bitmap_pixel_opaque(&icon, 0u, 0u)) return -1;    /* Ecke ausserhalb des Kreises */

    /* 3) rect */
    static const char svg_rect[] = "<svg viewBox='0 0 10 10'><rect x='2' y='2' width='6' height='6' fill='#00FF00'/></svg>";
    if (svgicon_render(svg_rect, sizeof(svg_rect) - 1, 10u, SVGICON_NO_THEME_OVERRIDE, &icon) != 0) return -1;
    if (!bitmap_pixel_opaque(&icon, 5u, 5u)) return -1;
    if (bitmap_pixel_opaque(&icon, 0u, 0u)) return -1;

    /* 4) polygon (Dreieck) */
    static const char svg_poly[] = "<svg viewBox='0 0 10 10'><polygon points='5,1 9,9 1,9' fill='#0000FF'/></svg>";
    if (svgicon_render(svg_poly, sizeof(svg_poly) - 1, 10u, SVGICON_NO_THEME_OVERRIDE, &icon) != 0) return -1;
    if (!bitmap_pixel_opaque(&icon, 5u, 7u)) return -1;
    if (bitmap_pixel_opaque(&icon, 0u, 0u)) return -1;

    /* 5) Transform (translate) verschiebt ein Rechteck sichtbar */
    static const char svg_transform[] =
        "<svg viewBox='0 0 20 20'><g transform='translate(10,0)'>"
        "<rect x='0' y='0' width='5' height='5' fill='#AABBCC'/></g></svg>";
    if (svgicon_render(svg_transform, sizeof(svg_transform) - 1, 20u, SVGICON_NO_THEME_OVERRIDE, &icon) != 0) return -1;
    if (!bitmap_pixel_opaque(&icon, 12u, 2u)) return -1;  /* verschobenes Rechteck */
    if (bitmap_pixel_opaque(&icon, 2u, 2u)) return -1;    /* urspruengliche Position leer */

    /* 6) Theme-Farbe (var(--nova-accent)) wird durch Override ersetzt */
    static const char svg_theme[] = "<svg viewBox='0 0 10 10'><rect x='0' y='0' width='10' height='10' fill='var(--nova-accent)'/></svg>";
    if (svgicon_render(svg_theme, sizeof(svg_theme) - 1, 8u, 0xFF336699u, &icon) != 0) return -1;
    if ((icon.pixels[4u * 8u + 4u] & 0x00FFFFFFu) != 0x336699u) return -1;

    /* 7) Skalierung: dieselbe Szene bei zwei Zielgroessen bleibt proportional gefuellt */
    static svgicon_bitmap_t icon_small, icon_large;
    if (svgicon_render(svg_rect, sizeof(svg_rect) - 1, 16u, SVGICON_NO_THEME_OVERRIDE, &icon_small) != 0) return -1;
    if (svgicon_render(svg_rect, sizeof(svg_rect) - 1, 64u, SVGICON_NO_THEME_OVERRIDE, &icon_large) != 0) return -1;
    if (!bitmap_pixel_opaque(&icon_large, 32u, 32u)) return -1;
    if (bitmap_pixel_opaque(&icon_large, 2u, 2u)) return -1;

    /* 8) Alpha: #RRGGBBAA mit halbtransparentem Alpha muss einen von 0/255
       verschiedenen Alphawert im Ergebnis erzeugen (Antialiasing-Kante). */
    static const char svg_alpha[] = "<svg viewBox='0 0 10 10'><circle cx='5' cy='5' r='4.5' fill='#11223380'/></svg>";
    if (svgicon_render(svg_alpha, sizeof(svg_alpha) - 1, 16u, SVGICON_NO_THEME_OVERRIDE, &icon) != 0) return -1;
    uint32_t center_a = (icon.pixels[8u * 16u + 8u] >> 24) & 0xFFu;
    if (center_a == 0u || center_a == 255u) return -1; /* 0x80-Fill-Alpha darf nicht voll opak sein */

    /* 9) beschaedigte/unausgeglichene XML -> svgicon_validate muss 0 liefern */
    static const char svg_broken[] = "<svg viewBox='0 0 10 10'><path d='M1 1L9 1Z'";
    if (svgicon_validate(svg_broken, sizeof(svg_broken) - 1) != 0) return -1;

    /* 10) nicht unterstuetztes Element (defs/radialGradient) -> Fehler */
    static const char svg_unsupported[] =
        "<svg viewBox='0 0 10 10'><defs><radialGradient id='g'/></defs>"
        "<rect x='0' y='0' width='10' height='10' fill='#FFFFFF'/></svg>";
    if (svgicon_validate(svg_unsupported, sizeof(svg_unsupported) - 1) != 0) return -1;

    /* 11) nicht unterstuetztes Pfadkommando (Arc 'A') -> Fehler */
    static const char svg_arc[] = "<svg viewBox='0 0 10 10'><path d='M1 1A4 4 0 0 1 9 9Z' fill='#FFFFFF'/></svg>";
    if (svgicon_validate(svg_arc, sizeof(svg_arc) - 1) != 0) return -1;

    /* 12) gueltige Datei muss weiterhin validieren */
    if (svgicon_validate(svg_rect, sizeof(svg_rect) - 1) != 1) return -1;

    /* 13) Cache: zweiter Aufruf mit identischem Key/Groesse/Theme muss ein
       Treffer sein (Hit-Zaehler steigt, kein erneutes Rendern noetig). */
    svgicon_bitmap_t *cached_a, *cached_b;
    uint32_t misses_before = svgicon_cache_misses();
    uint32_t hits_before = svgicon_cache_hits();
    if (svgicon_render_cached(0xABCDu, svg_rect, sizeof(svg_rect) - 1, 16u, SVGICON_NO_THEME_OVERRIDE, &cached_a) != 0) return -1;
    if (svgicon_cache_misses() != misses_before + 1u) return -1;
    if (svgicon_render_cached(0xABCDu, svg_rect, sizeof(svg_rect) - 1, 16u, SVGICON_NO_THEME_OVERRIDE, &cached_b) != 0) return -1;
    if (svgicon_cache_hits() != hits_before + 1u) return -1;
    if (cached_a != cached_b) return -1;

    return 0;
}
