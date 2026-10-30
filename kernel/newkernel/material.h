/* NovaOS C-Kernel - Nova Material Engine: Material System (Kernmodul).
   Port aus docs/NPSPEC/sysarchitecture/049-GUI/010-Material_Engine/
   001-Architektur/NPSPEC-MATERIAL-ARCH-0001.md + -SYSTEM-0001.md und
   007-Konfiguration_Erweiterbarkeit/NPSPEC-MATERIAL-PROPERTY-0001.md.

   BEWUSSTE SCOPE-REDUKTION (wie bei allen bisherigen GUI-Modulen dieses
   Kernels - kein GPU-Treiber, keine dynamische Speicherverwaltung fuer
   UI-Objekte, siehe window.c/compositor.c): feste, kleine Tabellen statt
   dynamischer Allokation; "Material Cache"/"Material Manager" aus
   ARCH-0001 Abschnitt 2 sind keine eigenen Module, sondern Teil dieser
   Registry (Cache siehe matperf.c, das auf dieselben Definition-/
   Instance-IDs aufsetzt). PROPERTY-0001 verlangt ein generisches, pro
   Eigenschaft typisiertes Registrierungssystem (Abschnitt 2/7) - hier
   stark reduziert auf einen festen Satz von zehn Kern-Eigenschaften
   (direkt als Felder, kein generisches Typsystem noetig, da alle
   Werte uint32_t/0..255-Skalen oder RGB-Packs sind) plus einen
   generischen "Ext-Parameter"-Block fuer die materialtyp-spezifischen
   Zusatzeigenschaften (Saturation, Noise, Thickness, ...) aus den
   einzelnen Materialtyp-Specs (siehe materialtypes.c) - das erfuellt
   SYSTEM-0001 Abschnitt 6 ("nicht unterstuetzte Eigenschaften duerfen
   die Verarbeitung nicht beeintraechtigen") und ARCH-0001 Abschnitt 9
   (Erweiterbarkeit durch zusaetzliche Eigenschaften), ohne den Kern pro
   Materialtyp wachsen zu lassen. Keine eigene Werttyp-/Bereichs-
   Validierungs-Metatabelle (PROPERTY-0001 Abschnitt 2/7, "Datentyp
   und Wertebereich je Eigenschaft") - alle zehn Kernfelder sind per
   Konvention 0..255 (Ausnahmen: *_rgb sind 0xRRGGBB-Packs, blur_radius
   ist 0..16 Pixel); material_instance_set_property_u32() clamped
   defensiv, mehr Typsicherheit ist ohne generisches Variant-System
   nicht sinnvoll umsetzbar. */
#ifndef NOVA_MATERIAL_H
#define NOVA_MATERIAL_H

#include <stdint.h>

#define MATERIAL_MAX_DEFINITIONS 32u
#define MATERIAL_MAX_INSTANCES   64u

#define MATERIAL_TYPE_ACRYLIC 0u
#define MATERIAL_TYPE_LIQUID  1u
#define MATERIAL_TYPE_FROSTED 2u
#define MATERIAL_TYPE_CRYSTAL 3u
#define MATERIAL_TYPE_SOLID   4u
#define MATERIAL_TYPE_CUSTOM  5u

/* NPSPEC-MATERIAL-SYSTEM-0001 Abschnitt 7. */
#define MATERIAL_STATE_NORMAL   0u
#define MATERIAL_STATE_HOVER    1u
#define MATERIAL_STATE_PRESSED  2u
#define MATERIAL_STATE_FOCUSED  3u
#define MATERIAL_STATE_DISABLED 4u
#define MATERIAL_STATE_ACTIVE   5u
#define MATERIAL_STATE_INACTIVE 6u
#define MATERIAL_STATE_COUNT    7u

/* Kern-Eigenschaftsfelder (Bitmaske fuer Overrides, SYSTEM-0001 Abschnitt 6). */
#define MATERIAL_FIELD_COLOR          (1u << 0)
#define MATERIAL_FIELD_TINT_RGB       (1u << 1)
#define MATERIAL_FIELD_TINT_STRENGTH  (1u << 2)
#define MATERIAL_FIELD_OPACITY        (1u << 3)
#define MATERIAL_FIELD_BLUR           (1u << 4)
#define MATERIAL_FIELD_REFRACTION     (1u << 5)
#define MATERIAL_FIELD_REFLECTION     (1u << 6)
#define MATERIAL_FIELD_HIGHLIGHT      (1u << 7)
#define MATERIAL_FIELD_DISTORTION     (1u << 8)
#define MATERIAL_FIELD_ROUGHNESS      (1u << 9)
#define MATERIAL_FIELD_ALL            0x3FFu

/* Ext-Parameter-Keys fuer materialtyp-spezifische Zusatzeigenschaften
   (ACRYLIC/LIQUID/FROSTED/CRYSTAL/SOLID-Specs, siehe materialtypes.c). */
#define MATERIAL_EXT_SATURATION       0u
#define MATERIAL_EXT_NOISE            1u
#define MATERIAL_EXT_DIFFUSION        2u
#define MATERIAL_EXT_BRIGHTNESS       3u
#define MATERIAL_EXT_THICKNESS        4u
#define MATERIAL_EXT_EDGE_INTENSITY   5u
#define MATERIAL_EXT_FLUIDITY         6u
#define MATERIAL_EXT_LIGHT_RESPONSE   7u
#define MATERIAL_EXT_DISPERSION       8u
#define MATERIAL_EXT_CLARITY          9u
#define MATERIAL_EXT_GRADIENT_RGB     10u
#define MATERIAL_EXT_BORDER_COLOR     11u
#define MATERIAL_EXT_BORDER_THICKNESS 12u
#define MATERIAL_EXT_COUNT            13u

typedef struct {
    uint32_t color_rgb;
    uint32_t tint_rgb;
    uint32_t tint_strength;    /* 0..255 */
    uint32_t opacity;          /* 0..255 */
    uint32_t blur_radius;      /* 0..16 Pixel */
    uint32_t refraction;       /* 0..255 */
    uint32_t reflection;       /* 0..255 */
    uint32_t highlight;        /* 0..255 */
    uint32_t distortion;       /* 0..255 */
    uint32_t roughness;        /* 0..255 */
    uint32_t ext_present_mask; /* Bit i gesetzt -> ext[i] gueltig */
    uint32_t ext[MATERIAL_EXT_COUNT];
} material_properties_t;

/* Feste Built-in-IDs (Registrierungsreihenfolge in material_initialize(),
   siehe materialtypes.c). */
#define MATERIAL_ID_ACRYLIC 1u
#define MATERIAL_ID_LIQUID  2u
#define MATERIAL_ID_FROSTED 3u
#define MATERIAL_ID_CRYSTAL 4u
#define MATERIAL_ID_SOLID   5u

void material_initialize(void);
int material_self_test(void); /* 0 = ok */

/* 0 = Fehler (Tabelle voll/Name leer), sonst die neue Definition-ID. */
uint32_t material_register(const char *name, uint32_t type,
                            const material_properties_t *defaults);
/* Setzt/ueberschreibt die Eigenschafts-Overrides eines Zustands einer
   Materialdefinition (SYSTEM-0001 Abschnitt 7). 0 = Erfolg. */
int material_set_state_override(uint32_t definition_id, uint32_t state,
                                  const material_properties_t *props,
                                  uint32_t field_mask);
int material_definition_exists(uint32_t definition_id);
uint32_t material_definition_type(uint32_t definition_id); /* 0xFFFFFFFF wenn ungueltig */
/* Liefert die unveraenderten Basiseigenschaften einer Definition (ohne
   Zustands-/Instanz-Overrides) - fuer matanim.c's Transition-Zielwerte
   (TRANSITION-0001: "Ausgangs- und Zielmaterial muessen eindeutig
   definiert sein"). 0 = Erfolg, -1 wenn definition_id ungueltig. */
int material_definition_properties(uint32_t definition_id, material_properties_t *out);
/* SYSTEM-0001 Abschnitt 8: Fallback fuer ungueltige/nicht verfuegbare
   Materialien - immer das eingebaute Solid-Material. */
uint32_t material_fallback_id(void);

uint32_t material_instance_create(uint32_t definition_id);
int material_instance_destroy(uint32_t instance_id);
int material_instance_exists(uint32_t instance_id);
int material_instance_set_property_u32(uint32_t instance_id, uint32_t field, uint32_t value);
int material_instance_set_ext_u32(uint32_t instance_id, uint32_t ext_key, uint32_t value);
int material_instance_clear_overrides(uint32_t instance_id);
/* Hebt den Override genau eines Feldes wieder auf (z.B. nach Abschluss
   einer temporaeren Verformung, siehe matanim.c) - die Instanz faellt fuer
   dieses Feld auf Zustands-/Definitionswerte zurueck. 0 = Erfolg. */
int material_instance_clear_property(uint32_t instance_id, uint32_t field);
int material_instance_set_state(uint32_t instance_id, uint32_t state);
uint32_t material_instance_state(uint32_t instance_id);
uint32_t material_instance_definition(uint32_t instance_id);
/* Wendet auf die zur Laufzeit zugewiesene Materialdefinition eine andere
   Definition an, ohne das Instanz-Objekt neu zu erzeugen (SYSTEM-0001
   Abschnitt 5, "Materialinstanzen muessen ohne Neuerstellung des
   zugehoerigen UI-Elements austauschbar sein"). 0 = Erfolg, -1 wenn eine
   der beiden IDs ungueltig ist. */
int material_instance_rebind(uint32_t instance_id, uint32_t new_definition_id);

/* Berechnet die effektiven Eigenschaften: Definition-Basiswerte -> aktiver
   Zustands-Override (falls Feld-Bit gesetzt) -> Instanz-Override (falls
   Feld-Bit gesetzt). Fuer MATERIAL_TYPE_SOLID werden Blur/Refraction/
   Reflection/Distortion erzwungen auf 0 und Opacity auf 255 gesetzt
   (SOLID-Spec Abschnitt 2/3: feste Deckkraft, keine Glaseffekte - das ist
   eine Eigenschaft des Typs, nicht ueberschreibbar). 0 = Erfolg. */
int material_instance_resolve(uint32_t instance_id, material_properties_t *out);

#endif
