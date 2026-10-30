/* NovaOS C-Kernel - Nova Material Engine: Optical-Effects-Pipeline. Port
   aus docs/NPSPEC/sysarchitecture/049-GUI/010-Material_Engine/
   001-Architektur/NPSPEC-MATERIAL-RENDER-0001.md (Rendering Pipeline),
   -COMPOSITOR-0001.md (Compositor-Zusammenarbeit) und
   005-Integration/NPSPEC-MATERIAL-BACKDROP-0001.md, sowie allen Specs aus
   003-Optische_Effekte/ (BLUR, DISTORTION, HIGHLIGHT, LIGHTING,
   REFLECTION, REFRACTION, TINT, TRANSPARENCY).

   BEWUSSTE SCOPE-REDUKTION (wie compositor.h's eigener Hinweis zu Blur/
   Acrylic): dieser Kernel hat keinen GPU-Treiber, keine Shader, keine
   Gleitkomma-Hardware-Nutzung (siehe render2d.c/compositor.c - durchgaengig
   reine Integer-Arithmetik) und keine echte 3D-/Lichtsimulation. Die
   folgenden Effekte sind deshalb bewusst als visuell plausible, aber
   NICHT physikalisch korrekte Software-Approximationen umgesetzt:
   - BLUR (BLUR-0001): echter Box-Blur ueber den Backdrop, aber der
     effektive Radius ist intern auf 4 Pixel begrenzt (PERF-0001 "Qualitaet
     darf Bedienbarkeit nicht beeintraechtigen" - ein ungedeckelter
     Box-Blur ohne SIMD/GPU waere pro Redraw zu teuer).
   - DISTORTION/REFRACTION (DISTORTION-0001 "Displacement Map"-Verfahren,
     REFRACTION-0001): Verschiebungsvektor aus einer festen 16-Eintrag-
     Integer-Wellentabelle (kein sin()/libm in diesem freistehenden
     Kernel) statt einer echten Displacement-Textur; liefert bei
     identischen Eingaben reproduzierbare Ergebnisse (REFRACTION-0001
     Abschnitt "Verbindliche Anforderungen"). EdgeIntensity verstaerkt den
     Versatz nahe den Materialkanten (REFRACTION-0001/CRYSTAL-0001).
   - HIGHLIGHT (HIGHLIGHT-0001): nur "Surface Highlight" - ein einzelnes,
     diagonal verlaufendes Glanzlicht-Band, dessen Breite von Roughness
     abhaengt. Weitere Glanzlichttypen (Abschnitt 5) sind nicht umgesetzt.
   - LIGHTING (LIGHTING-0001): nur Ambient- + ein fest von oben-links
     einfallendes Directional-Light, als einfacher Helligkeits-Multiplikator.
     Point Light (Abschnitt 5) ist nicht umgesetzt.
   - REFLECTION (REFLECTION-0001): vertikal gespiegelte, abgedunkelte
     Backdrop-Probe, mit einem einfachen (nicht echten) Fresnel-Faktor
     nach Abstand zur Materialkante gewichtet. Keine Reflexionsquellen
     aus Abschnitt 5 (Umgebungs-/Fenster-Reflexionen).
   - TINT (TINT-0001): Normal-Mischung vollstaendig, Multiply/Screen
     (Abschnitt 5) als einfache Kanaloperation ohne Farbraumkonvertierung
     (kein SDR/HDR-Colormanagement).
   - TRANSPARENCY (TRANSPARENCY-0001): nur "Static" (Abschnitt 5) - die in
     material.h resolvte Opacity wird direkt verwendet; dynamische,
     umgebungsabhaengige Anpassung ist nicht umgesetzt (keine
     Umgebungslichtsensorik in diesem Kernel).
   Alle Effekte sind unabhaengig voneinander aktivierbar (Eigenschaftswert
   0 = Effekt wirkungslos) und miteinander kombinierbar, wie von den
   jeweiligen Specs verbindlich gefordert. */
#ifndef NOVA_MATFX_H
#define NOVA_MATFX_H

#include <stdint.h>
#include "render2d.h"
#include "material.h"

#define MATFX_BLEND_NORMAL   0u
#define MATFX_BLEND_MULTIPLY 1u
#define MATFX_BLEND_SCREEN   2u

/* Fuehrt die vollstaendige Effekt-Pipeline fuer ein Rechteck aus (deterministische
   Reihenfolge, LAYER-0001: Distortion/Refraction-Sampling -> Blur ->
   Tint/Transparency -> Lighting -> Highlight -> Reflection). backdrop ist
   der vom Compositor bereits unterhalb dieses Materials zusammengesetzte
   Hintergrund (BACKDROP-0001 - ein eigener Puffer, niemals dst selbst,
   verhindert strukturell das verbotene Selbst-Sampling); bx/by ist der
   Offset des Materialbereichs innerhalb von backdrop. Fuer
   MATERIAL_TYPE_SOLID (keine Hintergrundabtastung, SOLID-0001) darf
   backdrop NULL sein - es wird dann nur props->color_rgb/tint flaechig
   gezeichnet. */
void matfx_apply(render2d_surface_t *dst, uint32_t x, uint32_t y,
                  uint32_t w, uint32_t h,
                  const render2d_surface_t *backdrop, uint32_t bx, uint32_t by,
                  const material_properties_t *props, uint32_t blend_mode);

int matfx_self_test(void); /* 0 = ok */

#endif
