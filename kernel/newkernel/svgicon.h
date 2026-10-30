/* NovaOS C-Kernel - Nova SVG Icon Renderer (Analogon zu
   docs/NPSPEC/Boot/Boot_Ressourcen_Assets/NPSPEC-BOOTRESOURCE-0008_SVG_
   Icon_Renderer.md fuer den Hauptkernel - jenes Dokument beschreibt den
   Boot-Manager-Renderer, der boot/bootloader/ laut
   scripts/convert-boot-icons-to-c.ps1 TATSAECHLICH NICHT umsetzt (dort
   nur handgepflegte 10x10-Monochrom-Bitmaps) - dieses Modul ist die
   ERSTE echte Umsetzung des SVG-1.1-Tiny-Subsets im Projekt. Loest
   gleichzeitig die in render2d.h als BLOCKED dokumentierten Luecken
   NPSPEC-GRAPHICS-2D-0003 (Vektor-Pfad-Tessellator), -0006
   (Transformations-Matrix) und -0007 (Anti-Aliasing) fuer den
   Icon-Anwendungsfall; -0004 (Image Rendering) bleibt render2d_blit's
   Pixel-Bereich fuer bereits rasterisierte Bitmaps.

   SCOPE (bewusst reduziert gegenueber NPSPEC-BOOTRESOURCE-0008):
   - Elemente: svg, g, path, rect, circle, ellipse, line, polyline,
     polygon (= die "Pflicht"-Liste aus Abschnitt 8). defs/use/clipPath
     ("Optional") sind NICHT umgesetzt - jedes Vorkommen ist ein
     Validierungsfehler (betrifft z.B. die mitgelieferte WiFi-Color-
     Variante mit radialGradient/stop, siehe assets/icons/system/color/
     wifi.svg - BEWUSST NICHT renderbar mit diesem Modul).
   - Path-Kommandos in `d`: M/L/H/V/C/Z (gross+klein, d.h. absolut UND
     relativ), inkl. impliziter Kommando-Wiederholung bei weiteren
     Koordinatenpaaren ohne neuen Buchstaben. S/Q/T/A (Shorthand-/
     Quadratic-Bezier/Arc) sind NICHT unterstuetzt - jedes Vorkommen ist
     ein Validierungsfehler. Reicht fuer alle 16 unter assets/icons/
     system/{regular,filled} kuratierten Fluent-UI-Icons (gegengeprueft:
     ausschliesslich M/L/H/V/C/Z, siehe Icon-Auswahl-Sitzung).
   - Farben: #RRGGBB, #RRGGBBAA, none, currentColor, var(--nova-accent|
     --nova-text|--nova-error) (letztere drei werden durch
     theme_rgb_override in svgicon_render ersetzt - OHNE Override wird
     opakes Schwarz verwendet). Benannte Farben (red/blue/...) und
     rgb()/rgba()-Funktionsschreibweise sind NICHT unterstuetzt.
   - Transformationen: translate/scale/rotate(+optionalem Pivot)/matrix/
     skewX/skewY, listenweise verkettbar (space-separiert). Sinus/Cosinus
     laufen OHNE FPU/libm ueber Bhaskara-I-Approximation (ganzzahlige
     Gradauflösung) - fuer Icon-Transformationen in UI-Groesse
     ausreichend genau, siehe svgicon_sin_deg/svgicon_cos_deg in
     svgicon.c.
   - Rasterisierung: Punkt-Sampling mit 2x2-Supersampling pro Zielpixel
     (Nonzero-Winding- ODER Evenodd-Test je Sample, abhaengig von
     fill-rule) - kein analytisches Scanline-Flaechen-Coverage (einfacher
     und nachweisbar korrekt, auf Kosten etwas groeberer Kantenglaettung;
     bei den hier verwendeten 16x16..64x64-Icongroessen nicht
     wahrnehmbar). Kubische Bezierkurven werden mit fester Unterteilung
     (SVGICON_BEZIER_STEPS Liniensegmente) geflacht - deterministisches
     O(n) in der Pfadsegmentzahl.
   - Stroke-Attribute (stroke/stroke-width/stroke-linecap/-linejoin)
     werden geparst (fuer Validierungszwecke/Zukunftssicherheit NPSPEC-
     BOOTRESOURCE-0008 Abschnitt 9), aber NICHT gezeichnet - alle real
     kuratierten Icons sind reine Fill-Pfade, kein Stroke-Rendering in
     dieser Sitzung.
   - Speicher: ausschliesslich statische Pools (Pfadkommando-Pool,
     Kantenpuffer, Cache-Slots) - keine Heap-Allokation zur Laufzeit
     (Abschnitt 17).

   Cache (Abschnitt 15): siehe svgicon_render_cached - fester Satz
   SVGICON_CACHE_SLOTS Eintraege, FIFO-Ersetzung bei Erschoepfung, O(1)-
   Lookup ueber linearen Scan einer kleinen festen Tabelle (praktisch
   O(1) fuer die vorgesehene einstellige Icon-Anzahl). */
#ifndef NOVA_SVGICON_H
#define NOVA_SVGICON_H

#include <stdint.h>

/* Groessten/kleinste erlaubte Rasterzielgroesse (quadratische Icons,
   siehe NPSPEC-BOOTRESOURCE-0008 Abschnitt 12 "Empfohlene Zielgroessen"
   16..64). */
#define SVGICON_MIN_TARGET_SIZE 8u
#define SVGICON_MAX_TARGET_SIZE 64u

/* Pixelformat: 0xAARRGGBB, Alpha im oberen Byte (NICHT dasselbe Format
   wie render2d_surface_t's opakes 0xRRGGBB - siehe svgicon_blit_blend
   fuer die Umwandlung beim Komposit auf eine opake Zielflaeche). */
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pixels[SVGICON_MAX_TARGET_SIZE * SVGICON_MAX_TARGET_SIZE];
} svgicon_bitmap_t;

/* Sentinel fuer theme_rgb_override: 0 bedeutet "kein Override" (nur
   currentColor/var(--nova-*) faellt dann auf opakes Schwarz zurueck) -
   explizit 0xFF000000 uebergeben, falls wirklich opakes Schwarz als
   Theme-Farbe gewuenscht ist (mit Override-Semantik, nicht Fallback). */
#define SVGICON_NO_THEME_OVERRIDE 0u

void svgicon_initialize(void);
int svgicon_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*icon gefuellt), -1 = Validierungsfehler (unbekanntes
   Element/Attribut, unbekanntes Pfadkommando, Puffer-/Elementlimit
   ueberschritten, beschaedigte XML-Struktur, fehlendes viewBox) oder
   target_size ausserhalb [SVGICON_MIN_TARGET_SIZE,SVGICON_MAX_TARGET_SIZE].
   source MUSS nicht nullterminiert sein (source_size ist massgeblich). */
int svgicon_render(const char *source, uint32_t source_size, uint32_t target_size,
                    uint32_t theme_rgb_override, svgicon_bitmap_t *icon);

/* 1 = source ist eine gueltige, mit diesem Renderer darstellbare SVG-
   Datei (reine Struktur-/Elementpruefung, OHNE tatsaechlich zu
   rasterisieren), 0 = nein. */
int svgicon_validate(const char *source, uint32_t source_size);

#define SVGICON_CACHE_SLOTS 8u

/* cache_key identifiziert die Icon-QUELLE (z.B. ein Index in eine
   statische Icon-Tabelle des Aufrufers, siehe svgicon_assets.h) - diese
   Funktion rendert NUR bei Cache-Miss (Key+target_size+theme_rgb_override
   identisch zu einem vorhandenen Slot = Treffer) und liefert sonst einen
   Zeiger auf den bereits gerenderten, gecachten Bitmap-Puffer zurueck.
   0 = Erfolg (*out zeigt auf einen INTERN gehaltenen, bis zum naechsten
   Verdraengen dieses Slots gueltigen Puffer - NICHT freigeben/
   ueberschreiben), -1 = Validierungsfehler wie bei svgicon_render. */
int svgicon_render_cached(uint32_t cache_key, const char *source, uint32_t source_size,
                           uint32_t target_size, uint32_t theme_rgb_override,
                           svgicon_bitmap_t **out);
uint32_t svgicon_cache_hits(void);
uint32_t svgicon_cache_misses(void);

/* Alpha-Komposit (Porter-Duff "over") von icon auf eine bereits mit
   render2d_surface_t beschriebene, opake 0xRRGGBB-Zielflaeche - fuer
   Faelle, in denen (anders als bei desktop_shell.c's eigenem lokalen
   Icon-Puffer) direkt in eine fremde Surface geblendet werden soll.
   dst_x/dst_y duerfen ausserhalb der Surface liegen (wird wie bei allen
   render2d-Funktionen stillschweigend geclippt). surf ist ein
   render2d_surface_t*, als void* uebergeben, um keine Kopplung an
   render2d.h im Header zu erzwingen (svgicon.c inkludiert render2d.h
   intern). */
void svgicon_blit_blend(void *surf, uint32_t dst_x, uint32_t dst_y, const svgicon_bitmap_t *icon);

#endif
