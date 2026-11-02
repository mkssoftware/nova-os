/* NovaOS C-Kernel - Nova Material Engine: Integration (Layer Composition,
   Styles/Templates, Theme, UI-Bindung). Port aus docs/NPSPEC/
   sysarchitecture/049-GUI/010-Material_Engine/005-Integration/
   NPSPEC-MATERIAL-{LAYER,STYLE,THEME,UI}-0001.md.

   BEWUSSTE SCOPE-REDUKTION: dieser Kernel hat kein generisches "Resource
   System" (THEME-0001 "Theme-Ressourcen werden ueber das bestehende
   Resource System aufgelöst") und kein eigenstaendiges Theme-Rollensystem
   - Themes sind hier auf Hell-/Dunkelmodus-Paare pro benannter Rolle
   reduziert (THEME-0001 Abschnitt 6). Dieses Modul bindet Materialien an
   Fenster-IDs (compositor.c/window.c), rendert aber NICHT selbst - die
   eigentliche Verdrahtung in den Compositor-Zeichenpfad (compositor.c
   zeichnet Fenster aktuell als einfache RGB+Alpha-Rechtecke, siehe
   compositor.h) ist bewusst NICHT Teil dieser Aenderung, um das bereits
   boot-verifizierte Compositor-Rendering nicht zu gefaehrden - das ist
   eine separate, spaetere Integrationsarbeit. Dieses Modul stellt die
   von LAYER/STYLE/THEME/UI-0001 verlangte Verwaltungs-API bereit und ist
   eigenstaendig selbsttestbar. */
#ifndef NOVA_MATINTEGRATE_H
#define NOVA_MATINTEGRATE_H

#include <stdint.h>
#include "material.h"
#include "render2d.h"

void matintegrate_initialize(void);
int matintegrate_self_test(void); /* 0 = ok */

/* --- Layer Composition (LAYER-0001) --- */
#define MATINTEGRATE_MAX_LAYERS 4u

/* Setzt/ersetzt Ebene `index` (0 = unterste) der Materialinstanz
   `instance_id`. layer_opacity (0..255) wirkt zusaetzlich zur Opacity der
   Ebenen-Definition. 0 = Erfolg. */
int matlayer_set(uint32_t instance_id, uint32_t index, uint32_t definition_id,
                  uint32_t layer_opacity, int enabled);
int matlayer_clear(uint32_t instance_id);

/* Setzt alle konfigurierten, aktivierten Ebenen von instance_id in
   deterministischer Reihenfolge (0..MATINTEGRATE_MAX_LAYERS-1) uebereinander
   auf dst um; jede Ebene > 0 verwendet das bereits komponierte Zwischen-
   ergebnis von dst als eigenen Backdrop (Ebenen-Stapel INNERHALB einer
   Instanz - kein Selbst-Sampling des fertigen Endergebnisses einer
   ANDEREN Materialinstanz, BACKDROP-0001 bleibt dadurch unverletzt).
   Deaktivierte Ebenen loesen keine Rendering-Operation aus (LAYER-0001).
   Sind keine Ebenen konfiguriert, wird ersatzweise die Instanz selbst
   (material_instance_resolve) ueber matfx_apply gezeichnet. */
void matlayer_composite(render2d_surface_t *dst, uint32_t x, uint32_t y,
                          uint32_t w, uint32_t h,
                          const render2d_surface_t *backdrop, uint32_t bx, uint32_t by,
                          uint32_t instance_id);

/* --- Styles und Templates (STYLE-0001) --- */
#define MATINTEGRATE_MAX_STYLES 16u

/* 0 = Fehler, sonst Style-ID. */
uint32_t matstyle_register(const char *name, uint32_t definition_id);
/* Tauscht die Definition eines Styles zur Laufzeit aus und aktualisiert
   sofort alle daran gebundenen Instanzen (STYLE-0001 "Aenderungen eines
   Styles muessen betroffene Materialinstanzen aktualisieren") - deren
   individuelle Instanz-Overrides bleiben dabei unveraendert erhalten. */
int matstyle_set_definition(uint32_t style_id, uint32_t new_definition_id);
uint32_t matstyle_definition(uint32_t style_id); /* 0 wenn ungueltig */
int matstyle_bind_instance(uint32_t instance_id, uint32_t style_id);
int matstyle_unbind_instance(uint32_t instance_id);

/* --- Theme-Integration (THEME-0001) --- */
#define MATINTEGRATE_MAX_THEME_ROLES 16u

/* 0 = Fehler, sonst Rollen-ID. Registriert eine benannte Theme-Rolle
   (z.B. "window-background") mit je einer Definition fuer Hell- und
   Dunkelmodus. */
uint32_t matintegrate_theme_register_role(const char *role_name,
                                            uint32_t light_definition_id,
                                            uint32_t dark_definition_id);
void matintegrate_set_dark_mode(int enabled);
int matintegrate_dark_mode(void);
/* Liefert die zum aktuellen Modus passende Definition-ID; faellt auf
   material_fallback_id() zurueck, wenn role_id ungueltig ist oder die
   passende Modus-Definition nicht registriert wurde (0) - THEME-0001
   "Fehlende Theme-Ressourcen verwenden definierte Standardwerte". */
uint32_t matintegrate_theme_resolve(uint32_t role_id);

/* --- UI-Integration (UI-0001) --- */
#define MATINTEGRATE_MAX_BINDINGS 32u

int matintegrate_ui_bind_instance(uint32_t window_id, uint32_t instance_id);
int matintegrate_ui_bind_style(uint32_t window_id, uint32_t style_id);
int matintegrate_ui_bind_theme_role(uint32_t window_id, uint32_t theme_role_id);
int matintegrate_ui_unbind(uint32_t window_id);

/* Prioritaet: explizite Instanz > Style > Theme-Rolle > Fallback (Solid).
   Erzeugt bei Bedarf einmalig eine Instanz und cached sie in der Bindung,
   damit Materialwechsel keine Neuerstellung des UI-Elements erfordern
   (UI-0001). 0 nur wenn window_id selbst nie gebunden wurde. */
uint32_t matintegrate_ui_resolve_instance(uint32_t window_id);

#endif
