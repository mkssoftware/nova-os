/* NovaOS C-Kernel - Nova Material Engine: Konfiguration und
   Erweiterbarkeit. Port aus docs/NPSPEC/sysarchitecture/049-GUI/
   010-Material_Engine/007-Konfiguration_Erweiterbarkeit/
   NPSPEC-MATERIAL-{CUSTOM,PRESET,ACCESS}-0001.md. NPSPEC-MATERIAL-
   PROPERTY-0001 ist bereits in material.h abgedeckt (siehe dessen
   Scope-Note: festes Eigenschaftsmodell + genereischer Ext-Block statt
   eines generischen Typsystems fuer Boolean/Float/Vector2/Matrix/...).

   BEWUSSTE SCOPE-REDUKTION:
   - CUSTOM-0001 "Materialvererbung" wird als EINMALIGE, flache Kopie zum
     Erstellzeitpunkt aufgeloest (`matext_custom_derive()` kopiert die
     Basiseigenschaften sofort in eine neue, eigenstaendige Definition) -
     es gibt KEINEN dynamischen Live-Link zur Basis. Das erfuellt
     "deterministisch" und "keine zyklischen Vererbungen" trivial (eine
     einmalig aufgeloeste Kopie kann keinen Zyklus bilden) und passt zu
     material.h's bereits bestehendem Prinzip unveraenderlicher
     Definitionen.
   - ACCESS-0001's Kontrastpruefung ist eine vereinfachte Helligkeits-
     Heuristik (ITU-R-BT.601-Luminanzgewichtung, einfache Differenz statt
     des vollen WCAG-Kontrastverhaeltnis-Logarithmus) - fuer eine grobe
     "ausreichend/nicht ausreichend"-Entscheidung ohne Farbraum-
     Management ausreichend, aber nicht WCAG-zertifizierbar. */
#ifndef NOVA_MATEXT_H
#define NOVA_MATEXT_H

#include <stdint.h>
#include "material.h"

void matext_initialize(void);
int matext_self_test(void); /* 0 = ok */

/* --- Custom Materials (CUSTOM-0001) --- */
/* Erstellt eine neue, eigenstaendige Materialdefinition als Ableitung von
   base_definition_id: uebernimmt dessen Basiseigenschaften, wendet dann
   override_props (nur die in field_mask gesetzten Felder, inkl. Ext-
   Werte aus override_props->ext_present_mask) an. 0 = Fehler, sonst neue
   Definition-ID. */
uint32_t matext_custom_derive(const char *name, uint32_t base_definition_id,
                                const material_properties_t *override_props,
                                uint32_t field_mask);

/* --- Presets (PRESET-0001) --- */
#define MATEXT_MAX_PRESETS 32u

/* 0 = Fehler, sonst Preset-ID. Ein Preset referenziert eine bestehende
   Definition und eine Eigenschafts-Ueberlagerung. */
uint32_t matext_preset_register(const char *name, uint32_t definition_id,
                                  const material_properties_t *override_props,
                                  uint32_t field_mask);
uint32_t matext_preset_definition(uint32_t preset_id); /* 0 wenn ungueltig */
/* Wendet die Preset-Ueberlagerung auf eine Instanz an (PRESET-0001 "ohne
   Neuerstellung des UI-Elements"); die Instanz kann anschliessend
   weiterhin individuelle Eigenschaften ueberschreiben, ohne die
   gemeinsame Preset-Definition zu veraendern. 0 = Erfolg. */
int matext_preset_apply(uint32_t preset_id, uint32_t instance_id);

/* Registriert die in PRESET-0001 Abschnitt 4 genannten 20 Standard-
   Presets (je 4 fuer Acrylic/LiquidGlass/Frosted/Crystal/Solid) ueber den
   gemeinsamen Standardtypen aus materialtypes.h. Muss NACH
   materialtypes_initialize() aufgerufen werden. */
void matext_presets_register_standard(void);
uint32_t matext_preset_acrylic_light(void);
uint32_t matext_preset_acrylic_dark(void);
uint32_t matext_preset_acrylic_subtle(void);
uint32_t matext_preset_acrylic_strong(void);
/* Generische Suche nach dem Namen eines ueber matext_preset_register()
   oder matext_presets_register_standard() registrierten Presets (z.B.
   "LiquidGlassSoft", "FrostedDark", "CrystalMinimal", "SolidAccent").
   0 wenn kein Preset mit diesem Namen existiert. */
uint32_t matext_preset_find(const char *name);

/* --- Barrierefreiheit (ACCESS-0001) --- */
void matext_accessibility_set_reduce_transparency(int enabled);
int matext_accessibility_reduce_transparency(void);
/* Bei aktivierter Transparenzreduzierung: erhoeht Opacity auf mindestens
   230 und entfernt Blur/Reflection/Highlight/Distortion (ACCESS-0001
   Abschnitt 5 "hintergrundabhaengige Materialien muessen durch
   deckendere Varianten ersetzt werden koennen"). */
void matext_accessibility_apply(material_properties_t *props);
/* 0 wenn bei aktiver Transparenzreduzierung KEINE Hintergrundabtastung
   mehr erfolgen soll (ACCESS-0001 "bei deaktivierter Transparenz duerfen
   keine unnoetigen Hintergrundabtastungen erfolgen") - vom Aufrufer vor
   matfx_apply() zu pruefen (bei 0 wird backdrop=NULL uebergeben). */
int matext_accessibility_should_sample_backdrop(void);

/* Grobe Helligkeits-Heuristik (siehe Scope-Note): 1 = ausreichender
   Kontrast zwischen den beiden 0xRRGGBB-Farben, 0 = nicht ausreichend. */
int matext_contrast_sufficient(uint32_t foreground_rgb, uint32_t background_rgb);
/* Hellt oder dunkelt props->color_rgb ab, bis ausreichender Kontrast zu
   foreground_rgb erreicht ist (ACCESS-0001 "bei unzureichendem Kontrast
   muessen geeignete Materialanpassungen moeglich sein"). */
void matext_contrast_ensure(material_properties_t *props, uint32_t foreground_rgb);

#endif
