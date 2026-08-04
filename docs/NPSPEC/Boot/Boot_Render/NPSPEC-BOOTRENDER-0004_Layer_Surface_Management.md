# NPSPEC-BOOTRENDER-0004
# Layer and Surface Management

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0004 |
| Titel | Layer and Surface Management |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0003 |
| Zugehörige ADRs | ADR-BOOTGFX-0004, ADR-BOOTGFX-0005, ADR-BOOTGFX-0006, ADR-BOOTUI-0005, ADR-BOOTPERF-0001, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Layer and Surface Management** der NovaOS Boot Rendering Engine.

Das System verwaltet:

- logische Darstellungsebenen
- sichtbare und nicht sichtbare Surfaces
- Front-, Back- und Zwischenpuffer
- Offscreen-Surfaces
- Dialog- und Overlay-Ebenen
- Clipping- und Maskenflächen
- Effekt- und Blur-Zwischenflächen
- Surface-Lebenszyklen
- Surface-Speicher
- Z-Reihenfolgen
- Compositing-Abhängigkeiten
- Damage- und Dirty-Regionen

Das Layer- und Surface-System bildet die Grundlage für:

- Double- und Triple-Buffering
- Glass-Dialoge
- Hintergrundunschärfe
- Schatten
- Animationen
- modale Overlays
- isoliertes Rendering
- partielle Aktualisierungen
- deterministisches Compositing

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle grafischen Ausgabepfade des NovaOS-Bootmanagers.

Dazu gehören:

- BIOS mit VBE
- UEFI mit GOP
- Software-Rendering
- optionale hardwarebeschleunigte Backends
- virtuelle Testbackends
- Safe-Mode-Rendering
- Recovery-Oberflächen

Nicht Bestandteil dieser Spezifikation sind:

- die konkrete Rasterisierung geometrischer Primitive
- das vollständige Dialogverhalten
- die Auswertung von Layoutregeln
- die Implementierung von Grafiktreibern
- die Verwaltung physischer GPU-Ressourcen nach dem Kernelstart

---

# 3. Grundbegriffe

## 3.1 Layer

Ein **Layer** ist eine logische Darstellungsebene innerhalb der Bootoberfläche.

Ein Layer:

- besitzt eine eindeutige ID
- besitzt eine definierte Z-Position
- enthält Scene-Graph-Nodes oder Render Commands
- kann sichtbar oder verborgen sein
- kann eine eigene Surface besitzen
- kann Opazität, Transformationen und Effekte besitzen
- kann unabhängig als Dirty markiert werden

Ein Layer ist zunächst eine logische Gruppierung und nicht zwingend ein separater Pixelpuffer.

---

## 3.2 Surface

Eine **Surface** ist ein adressierbarer zweidimensionaler Pixelspeicher.

Eine Surface besitzt mindestens:

- Breite
- Höhe
- Pixelstride
- Pixelformat
- Speicheradresse
- Speichergröße
- Eigentümer
- Lebensdauer
- Nutzungszweck
- Dirty-Informationen

Eine Surface kann sichtbar oder unsichtbar sein.

---

## 3.3 Frontbuffer

Der Frontbuffer ist die aktuell sichtbare Ausgabefläche.

Unter BIOS/VBE und UEFI/GOP ist dies üblicherweise der durch Firmware oder Grafikabstraktion bereitgestellte lineare Framebuffer.

Direktes Zeichnen durch Controls oder Scene-Graph-Nodes in den Frontbuffer ist verboten.

---

## 3.4 Backbuffer

Der Backbuffer ist die primäre Renderfläche eines Frames.

Die gesamte finale Szene wird zunächst in den Backbuffer gerendert und anschließend kontrolliert in den Frontbuffer übertragen.

---

## 3.5 Offscreen-Surface

Eine Offscreen-Surface ist eine nicht direkt sichtbare Renderfläche.

Sie wird verwendet für:

- isolierte Layer
- Dialoginhalte
- Blur-Quellen
- Masken
- Schatten
- Caches
- Übergangsanimationen
- Vorberechnung komplexer Komponenten

---

## 3.6 Intermediate Surface

Eine Intermediate Surface ist eine kurzlebige Zwischenfläche innerhalb eines einzelnen Render- oder Effektvorgangs.

Beispiele:

- horizontaler Blur-Pass
- vertikaler Blur-Pass
- temporäre Alpha-Maske
- Farbraumkonvertierung
- Skalierungszwischenschritt

Intermediate Surfaces werden aus einem temporären Surface-Pool bezogen.

---

# 4. Architekturübersicht

```text
Scene Graph
    │
    ▼
Layer Assignment
    │
    ▼
Layer Tree
    │
    ├── Background Layer
    ├── Content Layer
    ├── Control Layer
    ├── Popup Layer
    ├── Dialog Layer
    ├── Overlay Layer
    ├── Notification Layer
    └── Cursor Layer
    │
    ▼
Surface Allocation
    │
    ▼
Layer Rendering
    │
    ▼
Effect Processing
    │
    ▼
Compositing
    │
    ▼
Backbuffer
    │
    ▼
Present
    │
    ▼
Frontbuffer
```

---

# 5. Architekturprinzipien

## 5.1 Trennung von Layer und Surface

Ein Layer und eine Surface sind unterschiedliche Konzepte.

Ein Layer beschreibt:

- logische Gruppierung
- Reihenfolge
- Sichtbarkeit
- Transformationen
- Effekte

Eine Surface beschreibt:

- konkreten Pixelspeicher
- Pixelformat
- Speicherlayout
- Abmessungen

Ein Layer darf ohne eigene Surface existieren.

Mehrere Layer dürfen direkt in dieselbe Ziel-Surface rendern, sofern keine Isolation erforderlich ist.

---

## 5.2 Surface-Isolation nur bei Bedarf

Eine separate Surface wird nur erzeugt, wenn mindestens eine der folgenden Bedingungen erfüllt ist:

- Layer besitzt gruppenweite Opazität
- Layer wird als Einheit transformiert
- Layer benötigt Hintergrundunschärfe
- Layer verwendet eine Maske
- Layer wird zwischengespeichert
- Layer nimmt an einer Übergangsanimation teil
- Layer besitzt einen nichtlokalen Effekt
- Layer muss unabhängig aktualisiert werden
- Layer benötigt einen anderen Blend-Modus

Einfache statische Layer dürfen direkt in die übergeordnete Ziel-Surface gerendert werden.

---

## 5.3 Deterministische Reihenfolge

Die Reihenfolge aller Layer ist vollständig definiert.

Bei gleicher Z-Position entscheidet:

1. Layer-Klasse
2. expliziter Z-Index
3. Erstellungsreihenfolge
4. stabile Objekt-ID

Die Reihenfolge darf nicht von Speicheradressen, Hash-Reihenfolgen oder Thread-Timing abhängen.

---

## 5.4 Keine Allokation im Render-Hotpath

Alle dauerhaft benötigten Surfaces werden vor dem aktiven Rendering reserviert.

Temporäre Surfaces stammen aus vorbereiteten Pools.

Während der Ausführung eines Renderpasses sind folgende Operationen verboten:

- allgemeine Heap-Allokationen
- dynamische Poolvergrößerungen
- Dateizugriffe
- unkontrollierte Speicherreservierungen
- Surface-Erzeugung ohne Budgetprüfung

---

# 6. Standard-Layer-Modell

Die Boot-UI definiert folgende Standard-Layer.

| Reihenfolge | Layer | Zweck |
|---:|---|---|
| 0 | `BOOT_LAYER_BACKGROUND` | Hintergrundfarbe, Hintergrundbild, Wallpaper |
| 100 | `BOOT_LAYER_CONTENT` | Seiteninhalt und primäre Bootansicht |
| 200 | `BOOT_LAYER_CONTROL` | Buttons, Listen, Karten und Bedienelemente |
| 300 | `BOOT_LAYER_POPUP` | Menüs, Tooltips und nichtmodale Popups |
| 400 | `BOOT_LAYER_DIALOG` | Modale und nichtmodale Dialoge |
| 500 | `BOOT_LAYER_OVERLAY` | Abdunklung, Glass-Backdrop und Fokusblockade |
| 600 | `BOOT_LAYER_NOTIFICATION` | Warnungen, Statusmeldungen und Hinweise |
| 700 | `BOOT_LAYER_DIAGNOSTIC` | Debug- und Diagnoseeinblendungen |
| 800 | `BOOT_LAYER_CURSOR` | Mauszeiger und Touchindikator |
| 900 | `BOOT_LAYER_EMERGENCY` | kritische Fehler- und Recovery-Oberfläche |

Die Zahlenbereiche zwischen den Standardebenen bleiben für interne Unterebenen verfügbar.

---

# 7. Layer-Hierarchie

Layer dürfen hierarchisch verschachtelt werden.

Beispiel:

```text
Root Layer
├── Background Layer
├── Content Layer
│   ├── Navigation Layer
│   └── Boot Entry Layer
├── Dialog Layer
│   ├── Dialog Backdrop Layer
│   └── Dialog Content Layer
└── Cursor Layer
```

Ein Child-Layer erbt standardmäßig:

- Sichtbarkeit
- Opazität
- Transformation
- Clipping
- Qualitätsprofil

Ein Child-Layer darf geerbte Werte lokal einschränken, jedoch nicht ungültig erweitern.

Beispiel:

- Ein Child darf unsichtbar werden.
- Ein Child darf nicht sichtbar werden, wenn der Parent unsichtbar ist.
- Ein Child darf sein Clip-Rechteck weiter verkleinern.
- Ein Child darf den Parent-Clip nicht umgehen.

---

# 8. Layer-Datenstruktur

Eine Referenzstruktur kann wie folgt aussehen:

```c
typedef uint32_t nova_boot_layer_id_t;
typedef uint32_t nova_boot_surface_id_t;

typedef enum nova_boot_layer_type
{
    NOVA_BOOT_LAYER_ROOT = 0,
    NOVA_BOOT_LAYER_BACKGROUND,
    NOVA_BOOT_LAYER_CONTENT,
    NOVA_BOOT_LAYER_CONTROL,
    NOVA_BOOT_LAYER_POPUP,
    NOVA_BOOT_LAYER_DIALOG,
    NOVA_BOOT_LAYER_OVERLAY,
    NOVA_BOOT_LAYER_NOTIFICATION,
    NOVA_BOOT_LAYER_DIAGNOSTIC,
    NOVA_BOOT_LAYER_CURSOR,
    NOVA_BOOT_LAYER_EMERGENCY,
    NOVA_BOOT_LAYER_CUSTOM
} nova_boot_layer_type_t;

typedef struct nova_boot_layer
{
    nova_boot_layer_id_t id;
    nova_boot_layer_type_t type;

    nova_boot_layer_id_t parent_id;
    nova_boot_layer_id_t first_child_id;
    nova_boot_layer_id_t next_sibling_id;

    nova_boot_surface_id_t surface_id;

    int32_t z_index;

    float opacity;
    nova_mat3x3_t local_transform;
    nova_rect_f32_t logical_bounds;
    nova_rect_i32_t pixel_bounds;
    nova_rect_i32_t clip_bounds;

    uint32_t flags;
    uint32_t dirty_flags;
    uint32_t effect_flags;

    uint64_t creation_order;
    uint64_t generation;
} nova_boot_layer_t;
```

Direkte Pointer zwischen persistenten Layer-Objekten sollen vermieden werden, wenn stabile Handles eine sicherere Lebenszyklusverwaltung ermöglichen.

---

# 9. Layer-Flags

Mindestens folgende Flags müssen unterstützt werden:

```c
NOVA_BOOT_LAYER_VISIBLE
NOVA_BOOT_LAYER_ENABLED
NOVA_BOOT_LAYER_ISOLATED
NOVA_BOOT_LAYER_CACHEABLE
NOVA_BOOT_LAYER_CACHED
NOVA_BOOT_LAYER_MODAL
NOVA_BOOT_LAYER_INPUT_BLOCKING
NOVA_BOOT_LAYER_CLIPPED
NOVA_BOOT_LAYER_TRANSFORMED
NOVA_BOOT_LAYER_HAS_OPACITY
NOVA_BOOT_LAYER_HAS_BLUR
NOVA_BOOT_LAYER_HAS_SHADOW
NOVA_BOOT_LAYER_HAS_MASK
NOVA_BOOT_LAYER_DIRTY
NOVA_BOOT_LAYER_DESTROY_PENDING
```

Unbekannte Flags müssen ignoriert oder als inkompatibel zurückgewiesen werden, abhängig von der Formatversion.

---

# 10. Surface-Typen

```c
typedef enum nova_boot_surface_type
{
    NOVA_BOOT_SURFACE_FRONTBUFFER = 0,
    NOVA_BOOT_SURFACE_BACKBUFFER,
    NOVA_BOOT_SURFACE_PRESENT_BUFFER,
    NOVA_BOOT_SURFACE_LAYER,
    NOVA_BOOT_SURFACE_OFFSCREEN,
    NOVA_BOOT_SURFACE_EFFECT,
    NOVA_BOOT_SURFACE_MASK,
    NOVA_BOOT_SURFACE_GLYPH_ATLAS,
    NOVA_BOOT_SURFACE_IMAGE_CACHE,
    NOVA_BOOT_SURFACE_TEMPORARY,
    NOVA_BOOT_SURFACE_EMERGENCY
} nova_boot_surface_type_t;
```

---

# 11. Surface-Datenstruktur

```c
typedef struct nova_boot_surface
{
    nova_boot_surface_id_t id;
    nova_boot_surface_type_t type;

    void* pixels;
    uintptr_t physical_address;

    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bytes_per_pixel;

    nova_pixel_format_t pixel_format;
    nova_color_space_t color_space;

    uint64_t allocation_size;
    uint64_t generation;

    nova_boot_layer_id_t owner_layer_id;

    uint32_t flags;
    uint32_t reference_count;
    uint32_t lock_count;

    nova_rect_i32_t valid_region;
    nova_damage_set_t damage;

    nova_memory_pool_id_t memory_pool;
} nova_boot_surface_t;
```

---

# 12. Pixelformat

Interne Render-Surfaces verwenden verbindlich:

```text
32-Bit RGBA
8 Bit Rot
8 Bit Grün
8 Bit Blau
8 Bit Alpha
Premultiplied Alpha
```

Die tatsächliche Byte-Reihenfolge wird durch das interne Pixelformat eindeutig definiert.

Empfohlene kanonische Darstellung:

```text
RGBA8888
```

Bei Little-Endian-Systemen darf die physische Byte-Reihenfolge abweichen. Der Zugriff erfolgt ausschließlich über definierte Pixeloperationen oder validierte Formatkonvertierungen.

Firmware-Framebuffer mit anderen Kanalreihenfolgen werden erst beim Present oder durch das Graphics Backend konvertiert.

---

# 13. Surface-Flags

```c
NOVA_BOOT_SURFACE_READABLE
NOVA_BOOT_SURFACE_WRITABLE
NOVA_BOOT_SURFACE_PRESENTABLE
NOVA_BOOT_SURFACE_PREMULTIPLIED_ALPHA
NOVA_BOOT_SURFACE_OPAQUE
NOVA_BOOT_SURFACE_TRANSIENT
NOVA_BOOT_SURFACE_CACHED
NOVA_BOOT_SURFACE_PINNED
NOVA_BOOT_SURFACE_EXTERNAL_MEMORY
NOVA_BOOT_SURFACE_NEEDS_CLEAR
NOVA_BOOT_SURFACE_DIRTY
NOVA_BOOT_SURFACE_LOCKED
NOVA_BOOT_SURFACE_FAILED
```

---

# 14. Surface-Lebenszyklus

Eine Surface besitzt folgende Zustände:

```text
Unallocated
    ↓
Reserved
    ↓
Initialized
    ↓
Ready
    ↓
In Use
    ↓
Idle
    ↓
Recycled
    ↓
Released
```

Fehlerpfad:

```text
Reserved
    ↓
Initialization Failed
    ↓
Released
```

Eine Surface darf nur verwendet werden, wenn sie sich im Zustand `Ready`, `In Use` oder `Idle` befindet.

---

# 15. Surface-Erzeugung

Die Surface-Erzeugung erfolgt über einen zentralen Surface Manager.

```c
nova_result_t nova_boot_surface_create(
    const nova_boot_surface_descriptor_t* descriptor,
    nova_boot_surface_id_t* out_surface_id
);
```

Der Descriptor enthält mindestens:

```c
typedef struct nova_boot_surface_descriptor
{
    nova_boot_surface_type_t type;

    uint32_t width;
    uint32_t height;
    nova_pixel_format_t pixel_format;

    uint32_t usage_flags;
    uint32_t clear_mode;

    nova_boot_layer_id_t owner_layer_id;
    nova_memory_pool_id_t preferred_pool;

    uint64_t maximum_size;
} nova_boot_surface_descriptor_t;
```

Vor der Allokation werden geprüft:

- Breite größer als null
- Höhe größer als null
- Multiplikationen ohne Integer-Overflow
- gültiges Pixelformat
- ausreichendes Speicherbudget
- zulässige Maximalabmessungen
- gültiger Eigentümer
- verfügbare Poolkapazität

---

# 16. Größenberechnung

Die mindestens benötigte Speichergröße berechnet sich aus:

```text
AllocationSize = Pitch × Height
```

Der Pitch muss mindestens betragen:

```text
Width × BytesPerPixel
```

Pitch und AllocationSize müssen mit overflow-sicheren Operationen berechnet werden.

Referenzprüfung:

```c
bool nova_checked_mul_u64(
    uint64_t a,
    uint64_t b,
    uint64_t* result
);
```

Eine Surface-Erzeugung muss fehlschlagen, wenn:

- Width × BytesPerPixel überläuft
- Pitch × Height überläuft
- die maximale Surface-Größe überschritten wird
- das Speicherbudget nicht ausreicht

---

# 17. Surface-Alignment

Für Surface-Speicher gelten folgende Mindestanforderungen:

| Verwendungszweck | Mindest-Alignment |
|---|---:|
| Allgemeine Software-Surface | 16 Byte |
| SIMD-optimierte Pixeloperationen | 32 Byte |
| Cacheline-optimierte Surfaces | 64 Byte |
| Firmware-Framebuffer | durch Backend vorgegeben |
| DMA- oder Hardware-Surface | backendabhängig |

Der Pitch darf zur Optimierung auf 16, 32 oder 64 Byte aufgerundet werden.

---

# 18. Surface-Pools

Das System definiert mehrere Pools.

## 18.1 Persistent Surface Pool

Enthält:

- Backbuffer
- optionalen dritten Present-Buffer
- Glyphen-Atlas
- dauerhaft gecachte Icons
- Emergency Surface

Lebensdauer:

```text
Runtime-Initialisierung bis Runtime-Shutdown
```

---

## 18.2 Scene Surface Pool

Enthält:

- gecachte Control-Layer
- Dialog-Surfaces
- große Hintergrundflächen
- Seitenübergangsflächen

Lebensdauer:

```text
Scene-Erzeugung bis Scene-Zerstörung
```

---

## 18.3 Effect Surface Pool

Enthält:

- Blur-Zwischenflächen
- Schattenmasken
- temporäre Compositing-Flächen
- Masken

Lebensdauer:

```text
Effektstart bis Effektabschluss
```

---

## 18.4 Frame Surface Pool

Enthält kurzlebige Surfaces, die ausschließlich innerhalb eines Frames benötigt werden.

Der gesamte Pool wird nach Abschluss des Frames zurückgesetzt.

Einzelne Freigaben sind nicht erforderlich.

---

## 18.5 Emergency Surface Pool

Enthält mindestens eine vorreservierte Surface für:

- kritische Fehlermeldungen
- Safe Mode
- Recovery
- UI-Ausfall

Dieser Pool darf nicht von normalen Renderpfaden verwendet werden.

---

# 19. Speicherbudgets

Empfohlene Baseline bei einem UI-Gesamtbudget von 64 MiB:

| Bereich | Richtwert |
|---|---:|
| Frontbuffer-Mapping | extern oder firmwareverwaltet |
| Backbuffer | auflösungsabhängig |
| optionaler dritter Buffer | auflösungsabhängig |
| persistente Layer-Surfaces | maximal 12 MiB |
| temporäre Effect-Surfaces | maximal 8 MiB |
| Surface-Metadaten | maximal 1 MiB |
| Emergency Surface | maximal 4 MiB |
| Reserve | mindestens 10 % des Gesamtbudgets |

Bei 1920 × 1080 × 4 Byte benötigt eine vollständige Surface ungefähr:

```text
8.294.400 Byte
```

Damit beanspruchen zwei vollständige Softwarepuffer ungefähr 16,6 MB, ohne Alignment und Metadaten.

Triple-Buffering darf deshalb nicht ohne Budgetprüfung aktiviert werden.

---

# 20. Surface-Wiederverwendung

Freigegebene Surfaces werden nach Möglichkeit wiederverwendet.

Eine Surface darf wiederverwendet werden, wenn:

- Typ kompatibel ist
- Abmessungen ausreichend sind
- Pixelformat identisch ist
- Alignment erfüllt ist
- kein aktiver Besitzer existiert
- Lock Count null ist
- Reference Count null ist
- vorherige Inhalte nicht sicherheitsrelevant sind

Vor Wiederverwendung wird die Surface abhängig vom Clear-Modus:

- vollständig gelöscht
- nur in der Valid Region gelöscht
- als inhaltlich undefiniert markiert

---

# 21. Clear-Modi

```c
typedef enum nova_boot_surface_clear_mode
{
    NOVA_BOOT_SURFACE_CLEAR_NONE = 0,
    NOVA_BOOT_SURFACE_CLEAR_TRANSPARENT,
    NOVA_BOOT_SURFACE_CLEAR_OPAQUE_BLACK,
    NOVA_BOOT_SURFACE_CLEAR_THEME_BACKGROUND,
    NOVA_BOOT_SURFACE_CLEAR_CUSTOM
} nova_boot_surface_clear_mode_t;
```

`CLEAR_NONE` darf nur verwendet werden, wenn garantiert jede gelesene Pixelposition zuvor beschrieben wurde.

---

# 22. Surface-Locking

Surfaces dürfen für kontrollierte Zugriffe gesperrt werden.

```c
nova_result_t nova_boot_surface_lock(
    nova_boot_surface_id_t surface_id,
    nova_boot_surface_mapping_t* out_mapping
);

nova_result_t nova_boot_surface_unlock(
    nova_boot_surface_id_t surface_id,
    const nova_rect_i32_t* modified_region
);
```

Eine Mapping-Struktur enthält:

```c
typedef struct nova_boot_surface_mapping
{
    void* pixels;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    nova_pixel_format_t pixel_format;
} nova_boot_surface_mapping_t;
```

Regeln:

- mehrfaches Locking ist nur für kompatible Lesezugriffe erlaubt
- Schreibzugriffe benötigen exklusiven Zugriff
- eine gelockte Surface darf nicht freigegeben werden
- beim Unlock muss die veränderte Region angegeben werden
- fehlende Region bedeutet vollständige Beschädigung

---

# 23. Referenzverwaltung

Gemeinsam genutzte Surfaces verwenden stabile Referenzen.

```c
nova_result_t nova_boot_surface_retain(
    nova_boot_surface_id_t surface_id
);

nova_result_t nova_boot_surface_release(
    nova_boot_surface_id_t surface_id
);
```

Eine Surface wird erst recycelt, wenn:

```text
ReferenceCount = 0
AND
LockCount = 0
AND
Owner = None
```

Reference Count Overflow muss erkannt werden.

---

# 24. Layer-Erzeugung

```c
nova_result_t nova_boot_layer_create(
    const nova_boot_layer_descriptor_t* descriptor,
    nova_boot_layer_id_t* out_layer_id
);
```

Der Descriptor enthält mindestens:

```c
typedef struct nova_boot_layer_descriptor
{
    nova_boot_layer_type_t type;
    nova_boot_layer_id_t parent_id;

    int32_t z_index;

    nova_rect_f32_t bounds;
    float opacity;

    uint32_t flags;
    uint32_t effect_flags;

    nova_boot_surface_policy_t surface_policy;
} nova_boot_layer_descriptor_t;
```

---

# 25. Surface-Policy eines Layers

```c
typedef enum nova_boot_surface_policy
{
    NOVA_BOOT_SURFACE_POLICY_NONE = 0,
    NOVA_BOOT_SURFACE_POLICY_AUTO,
    NOVA_BOOT_SURFACE_POLICY_REQUIRED,
    NOVA_BOOT_SURFACE_POLICY_CACHED,
    NOVA_BOOT_SURFACE_POLICY_TRANSIENT
} nova_boot_surface_policy_t;
```

## NONE

Der Layer rendert direkt in die Ziel-Surface des Parents.

## AUTO

Der Layer Manager entscheidet anhand von Effekten und Eigenschaften.

## REQUIRED

Der Layer benötigt zwingend eine eigene Surface.

## CACHED

Der Layer wird in eine persistente Surface gerendert und nur bei Invalidierung aktualisiert.

## TRANSIENT

Der Layer erhält nur für den aktuellen Effekt oder Frame eine Surface.

---

# 26. Automatische Surface-Isolation

Bei `SURFACE_POLICY_AUTO` muss eine eigene Surface erzeugt werden, wenn:

```text
LayerOpacity < 1.0
OR
LayerHasBlur
OR
LayerHasMask
OR
LayerHasGroupTransform
OR
LayerHasNonStandardBlendMode
OR
LayerIsCached
OR
LayerParticipatesInTransition
```

Keine eigene Surface ist erforderlich, wenn:

- nur einzelne Child-Nodes transparente Farben besitzen
- keine gruppenweite Opazität angewendet wird
- keine nichtlokalen Effekte aktiv sind
- Parent und Child dasselbe Clipping verwenden
- direktes Rendering visuell identisch ist

---

# 27. Layer-Reihenfolge

Die endgültige Sortierreihenfolge wird durch einen stabilen Sortierschlüssel bestimmt.

```c
typedef struct nova_boot_layer_sort_key
{
    uint16_t layer_class;
    int32_t z_index;
    uint64_t creation_order;
    nova_boot_layer_id_t stable_id;
} nova_boot_layer_sort_key_t;
```

Sortierregeln:

1. kleinere Layer-Klasse zuerst
2. kleinerer Z-Index zuerst
3. ältere Erstellungsreihenfolge zuerst
4. kleinere stabile ID zuerst

Ein modaler Dialog muss oberhalb aller normalen Inhaltslayer liegen.

Der Cursor muss oberhalb regulärer Dialoge liegen.

Der Emergency Layer muss jede andere UI überdecken können.

---

# 28. Layer-Transformationen

Jeder Layer besitzt eine lokale 2D-Transformation.

```text
WorldTransform = ParentWorldTransform × LocalTransform
```

Die Welttransformation wird während der Layer-Traversierung berechnet.

Unterstützt werden:

- Translation
- Skalierung
- Rotation
- Pivot
- Kombinationen dieser Transformationen

Perspektivische Transformationen sind im Bootmanager nicht Bestandteil der Baseline.

---

# 29. Layer-Opazität

Die effektive Opazität berechnet sich aus:

```text
EffectiveOpacity =
    ParentEffectiveOpacity × LocalOpacity
```

Der gültige Wertebereich ist:

```text
0.0 bis 1.0
```

Werte außerhalb dieses Bereichs werden geklemmt.

Besitzt ein Layer Child-Nodes und eine gruppenweite Opazität kleiner als 1,0, muss der Layer in eine isolierte Surface gerendert und anschließend als Einheit komponiert werden.

---

# 30. Clipping

Layer können ein eigenes Clip-Rechteck oder eine Clip-Maske besitzen.

Das effektive Clipping ist die Schnittmenge aus:

```text
Parent Clip
∩
Layer Clip
∩
Surface Bounds
∩
Target Bounds
```

Leere Clip-Regionen verhindern die Verarbeitung des gesamten Layers.

Clipping muss vor jeder Pixeloperation wirksam sein.

---

# 31. Masken

Unterstützte Maskentypen:

- rechteckige Maske
- abgerundete Rechteckmaske
- Alpha-Maske
- Pfadmaske in vorgerasterter Form

Eine Maske wird als 8-Bit-Alpha-Surface oder als kanonische RGBA-Surface gespeichert.

Masken müssen:

- dieselbe oder eine eindeutig transformierbare Größe besitzen
- validiert sein
- innerhalb ihres Speicherbereichs gelesen werden
- Dirty-Regionen an den Layer propagieren

---

# 32. Dirty- und Damage-Verwaltung

Jeder Layer und jede Surface besitzt Damage-Informationen.

```c
typedef struct nova_damage_set
{
    nova_rect_i32_t regions[NOVA_BOOT_MAX_DAMAGE_REGIONS];
    uint32_t count;
    bool full_damage;
} nova_damage_set_t;
```

Dirty-Auslöser:

- Inhalt geändert
- Position geändert
- Größe geändert
- Sichtbarkeit geändert
- Opazität geändert
- Transformation geändert
- Clip geändert
- Effekt geändert
- Child hinzugefügt oder entfernt
- Surface ersetzt
- Ressource aktualisiert

---

# 33. Propagation von Damage

Wenn ein Child-Layer beschädigt ist:

1. wird die Damage Region in Parent-Koordinaten transformiert
2. um Effektbereiche erweitert
3. gegen Parent-Clip beschnitten
4. an den Parent weitergegeben
5. schließlich in Backbuffer-Koordinaten transformiert

Schatten und Blur müssen die beschädigte Region um ihren maximalen Wirkungsradius erweitern.

---

# 34. Damage-Zusammenführung

Überlappende oder benachbarte Regionen dürfen zusammengeführt werden.

Bei Überschreitung eines Schwellwerts wird `full_damage` gesetzt.

Empfohlene Schwellwerte:

- maximal 32 einzelne Damage Regions pro Surface
- Full-Frame-Rendering bei mehr als 50 % beschädigter Fläche
- Full-Frame-Rendering bei zu komplexer Regionenzusammenführung

Die konkreten Werte müssen konfigurierbar sein.

---

# 35. Layer-Caching

Ein Layer darf gecacht werden, wenn:

- sein Inhalt selten geändert wird
- seine Abmessungen stabil sind
- die Surface-Größe innerhalb des Cachebudgets liegt
- der Layer häufig erneut verwendet wird
- die Cachekosten geringer als erneutes Rendering sind

Typische Cachekandidaten:

- Hintergrundgrafik
- komplexe SVG-Illustration
- statische Bootkarte
- unveränderter Dialograhmen
- Glyphen-Atlas
- wiederkehrende Schattenmaske

---

# 36. Cache-Invalidierung

Ein Layer-Cache wird ungültig bei:

- Inhaltsänderung
- Größenänderung
- Themewechsel
- Skalierungswechsel
- Änderung des Pixelformats
- Änderung eines abhängigen Assets
- Änderung der Clip-Geometrie
- Änderung eines nicht nachträglich anwendbaren Effekts

Reine Translation oder gruppenweite Opazität dürfen einen Cache weiterverwenden, sofern der Cache als isolierte Surface vorliegt.

---

# 37. Cache-Eviction

Bei Speicherknappheit werden Cache-Surfaces nach folgender Reihenfolge entfernt:

1. unreferenzierte temporäre Caches
2. lange nicht verwendete Layer-Caches
3. große, leicht erneut erzeugbare Caches
4. optionale Effekt-Caches
5. Bild-Caches niedriger Priorität

Nicht entfernt werden dürfen:

- aktiver Backbuffer
- Frontbuffer
- Emergency Surface
- aktuell gelockte Surfaces
- Surfaces kritischer Dialoge
- Surface des laufenden Present-Vorgangs

---

# 38. Offscreen-Rendering

Offscreen-Rendering läuft in folgenden Schritten ab:

```text
Surface reservieren
    ↓
Surface löschen
    ↓
Layer lokal rendern
    ↓
Effekte anwenden
    ↓
Ergebnis in Ziel-Surface komponieren
    ↓
Surface cachen oder freigeben
```

Der lokale Koordinatenursprung einer Layer-Surface ist standardmäßig:

```text
0,0
```

Die Position im Parent wird erst beim Compositing angewendet.

---

# 39. Effekt-Surfaces

## 39.1 Schatten

Schatten können folgende Surfaces verwenden:

- Alpha-Maske des Quellobjekts
- horizontaler Blur-Zwischenpuffer
- vertikaler Blur-Zwischenpuffer
- finale Shadow Surface

Kleine Schatten dürfen über optimierte direkte Verfahren erzeugt werden.

---

## 39.2 Hintergrundunschärfe

Ein Glass-Layer benötigt:

1. Backdrop-Region aus bereits komponierten unteren Layern
2. Capture Surface
3. Blur-Zwischenfläche
4. finale Materialfläche

Der Layer darf keine Inhalte erfassen, die oberhalb seiner eigenen Z-Position liegen.

---

## 39.3 Maskierung

Eine Alpha-Maske darf in einer eigenen 8-Bit-Surface gespeichert werden, sofern der Renderer dieses Format unterstützt.

Andernfalls wird RGBA8888 mit ausschließlich verwendetem Alpha-Kanal genutzt.

---

# 40. Dialog-Layer

Ein modaler Dialog besteht mindestens aus:

```text
Dialog Group
├── Backdrop Layer
├── Glass Layer
├── Dialog Content Layer
└── Focus Indicator Layer
```

Der Backdrop Layer:

- blockiert Eingaben an untere Layer
- kann die Hintergrundhelligkeit reduzieren
- kann Hintergrundunschärfe auslösen
- besitzt keine eigenständige Navigation

Der Dialog Content Layer:

- erhält exklusiven Fokus
- liegt über dem Backdrop
- kann als isolierte Surface animiert werden

---

# 41. Overlay-Layer

Overlays werden verwendet für:

- modale Abdunklung
- Statusanzeigen
- Sperren der Eingabe
- globale Fortschrittsanzeigen
- Recovery-Hinweise

Overlays dürfen optional den gesamten Viewport abdecken.

Ein Overlay mit Input-Blocking muss verhindern, dass Pointer- oder Tastaturereignisse an darunterliegende Layer weitergeleitet werden.

---

# 42. Cursor-Layer

Der Cursor wird auf dem obersten regulären Layer gerendert.

Er besitzt:

- eine eigene kleine Surface oder direkte Sprite-Darstellung
- eine separate Dirty Region
- keine Auswirkung auf das Layout
- keine Aufnahme in Backdrop-Blur unterhalb liegender Dialoge

Der Cursor darf bei jedem Pointer-Move ausschließlich seine alte und neue Position beschädigen.

---

# 43. Emergency Layer

Der Emergency Layer besitzt höchste Priorität.

Er wird verwendet, wenn:

- der normale Scene Graph inkonsistent ist
- reguläre Layer nicht mehr gerendert werden können
- ein kritischer Ressourcenfehler vorliegt
- der Bootmanager in Safe Mode wechselt
- ein Text- oder Minimalgrafik-Fallback angezeigt werden muss

Der Emergency Layer verwendet ausschließlich:

- vorreservierte Surface
- eingebettete Basisschrift
- feste Farben
- minimale Zeichenprimitive
- keine Blur- oder Glass-Effekte

---

# 44. Double-Buffering

Im Double-Buffer-Modus existieren mindestens:

```text
Frontbuffer
Backbuffer
```

Ablauf:

1. Dirty Regions des Backbuffers vorbereiten
2. betroffene Layer rendern
3. Compositing abschließen
4. Backbuffer validieren
5. vollständige oder partielle Kopie zum Frontbuffer
6. Damage Regions zurücksetzen

Ein unvollständiger Frame darf niemals präsentiert werden.

---

# 45. Triple-Buffering

Im Triple-Buffer-Modus existieren:

```text
Frontbuffer
Renderbuffer
Pending Present Buffer
```

Triple-Buffering darf nur aktiviert werden, wenn:

- ausreichend Speicher vorhanden ist
- das Backend einen sinnvollen Present-Pfad besitzt
- die zusätzliche Latenz akzeptabel ist
- die Performance messbar verbessert wird

Bei Speicherknappheit wird automatisch auf Double-Buffering zurückgeschaltet.

---

# 46. Surface Presentation

Die Präsentation erfolgt ausschließlich über das Graphics Backend.

```c
nova_result_t nova_boot_surface_present(
    nova_boot_surface_id_t source_surface,
    const nova_damage_set_t* damage
);
```

Das Backend entscheidet:

- Page Flip
- vollständige Framebuffer-Kopie
- partielle Regionskopie
- Formatkonvertierung
- Pitch-Anpassung

Die Rendering Engine darf keine Firmware-spezifischen Present-Operationen direkt ausführen.

---

# 47. Formatkonvertierung beim Present

Wenn internes Format und Frontbufferformat abweichen:

```text
RGBA8888 intern
    ↓
Present Conversion
    ↓
GOP/VBE-Zielformat
```

Die Konvertierung darf:

- während des Present erfolgen
- durch einen vorbereiteten Konvertierungspuffer unterstützt werden
- über backendoptimierte Routinen beschleunigt werden

Die interne Surface-Darstellung bleibt unverändert.

---

# 48. Farbraum und Gamma

Surfaces müssen ihren Farbraum deklarieren.

Baseline:

```text
sRGB-kompatible Farbdaten
```

Alpha-Compositing soll gemäß den Vorgaben aus `ADR-BOOTGFX-0012` erfolgen.

Effekte, die linearen Farbraum benötigen, dürfen:

- eine angenäherte Lookup-Tabelle verwenden
- in einem temporären linearen Format arbeiten
- im reduzierten Qualitätsprofil auf vereinfachte Berechnung zurückfallen

---

# 49. Qualitätsprofile

Das Surface Management unterstützt folgende Qualitätsprofile.

## Full

- Layer-Isolation nach Bedarf
- Hintergrundunschärfe
- hochwertige Schatten
- Layer-Caching
- optional Triple-Buffering

## Balanced

- reduzierte Blur-Surfaces
- vereinfachte Schatten
- Double-Buffering
- selektives Layer-Caching

## Basic

- keine großflächige Hintergrundunschärfe
- begrenzte Offscreen-Surfaces
- einfache Alpha-Komposition
- Double-Buffering

## Safe

- opake Layer
- minimale Offscreen-Nutzung
- keine Blur-Zwischenflächen
- keine optionalen Caches

## Emergency

- nur Emergency Surface
- kein reguläres Layer-Compositing
- minimale Darstellung

---

# 50. Automatische Effektdegradation

Bei Surface- oder Speicherknappheit wird in folgender Reihenfolge degradiert:

1. optionale Cache-Surfaces freigeben
2. Blur-Auflösung reduzieren
3. Effekt-Surfaces verkleinern
4. Schattenqualität reduzieren
5. Hintergrundunschärfe deaktivieren
6. Triple-Buffering deaktivieren
7. isolierte Transparenz vereinfachen
8. Safe Mode aktivieren
9. Emergency Mode aktivieren

Die Bootfunktionalität muss erhalten bleiben.

---

# 51. Fehlerbehandlung

Mögliche Fehler:

- Surface-Pool erschöpft
- ungültige Surface-ID
- Surface-Größe überläuft
- Surface-Lock-Konflikt
- ungültiges Pixelformat
- beschädigte Layer-Hierarchie
- fehlgeschlagene Effekt-Surface
- Present-Fehler
- Damage-Region außerhalb der Bounds

Jeder Fehler muss:

- einen eindeutigen Fehlercode erzeugen
- protokolliert werden
- einen definierten Fallback auslösen
- den Bootvorgang nicht blockieren

---

# 52. Fehlercodes

Empfohlene Fehlercodes:

```c
NOVA_BOOT_SURFACE_OK = 0,

NOVA_BOOT_SURFACE_ERR_INVALID_ARGUMENT,
NOVA_BOOT_SURFACE_ERR_INVALID_ID,
NOVA_BOOT_SURFACE_ERR_OUT_OF_MEMORY,
NOVA_BOOT_SURFACE_ERR_BUDGET_EXCEEDED,
NOVA_BOOT_SURFACE_ERR_SIZE_OVERFLOW,
NOVA_BOOT_SURFACE_ERR_UNSUPPORTED_FORMAT,
NOVA_BOOT_SURFACE_ERR_LOCKED,
NOVA_BOOT_SURFACE_ERR_STILL_REFERENCED,
NOVA_BOOT_SURFACE_ERR_INVALID_STATE,
NOVA_BOOT_SURFACE_ERR_PRESENT_FAILED,

NOVA_BOOT_LAYER_ERR_INVALID_PARENT,
NOVA_BOOT_LAYER_ERR_CYCLE_DETECTED,
NOVA_BOOT_LAYER_ERR_INVALID_Z_ORDER,
NOVA_BOOT_LAYER_ERR_SURFACE_REQUIRED,
NOVA_BOOT_LAYER_ERR_EFFECT_UNAVAILABLE
```

---

# 53. Threadmodell

Die Baseline arbeitet single-threaded.

Alle Layer- und Surface-Änderungen erfolgen innerhalb definierter Runtime-Phasen:

```text
Input
    ↓
State Update
    ↓
Layer Update
    ↓
Layout
    ↓
Damage Calculation
    ↓
Rendering
    ↓
Compositing
    ↓
Present
```

Während Rendering und Present dürfen Layerstrukturen nicht verändert werden.

Spätere Parallelisierung muss über unveränderliche Snapshots oder getrennte Command Buffer erfolgen.

---

# 54. Mutationsregeln

Layer- und Surface-Mutationen sind nur in folgenden Phasen zulässig:

- Initialization
- State Update
- Scene Update
- Resource Completion
- Recovery

Nicht zulässig:

- während Layer-Traversierung
- während Compositing
- während Present
- während exklusivem Surface-Lock durch den Renderer

Verzögerte Änderungen werden in einer Mutation Queue gespeichert.

---

# 55. Mutation Queue

```c
typedef enum nova_boot_layer_mutation_type
{
    NOVA_BOOT_MUTATION_CREATE_LAYER,
    NOVA_BOOT_MUTATION_DESTROY_LAYER,
    NOVA_BOOT_MUTATION_ATTACH_LAYER,
    NOVA_BOOT_MUTATION_DETACH_LAYER,
    NOVA_BOOT_MUTATION_SET_VISIBILITY,
    NOVA_BOOT_MUTATION_SET_BOUNDS,
    NOVA_BOOT_MUTATION_SET_TRANSFORM,
    NOVA_BOOT_MUTATION_SET_OPACITY,
    NOVA_BOOT_MUTATION_SET_EFFECT,
    NOVA_BOOT_MUTATION_REPLACE_SURFACE
} nova_boot_layer_mutation_type_t;
```

Mutationen werden:

1. validiert
2. in stabiler Reihenfolge sortiert
3. atomar angewendet
4. als Dirty markiert
5. protokolliert

---

# 56. API-Übersicht

## Layer-Verwaltung

```c
nova_result_t nova_boot_layer_manager_initialize(
    const nova_boot_layer_manager_config_t* config
);

nova_result_t nova_boot_layer_create(
    const nova_boot_layer_descriptor_t* descriptor,
    nova_boot_layer_id_t* out_layer_id
);

nova_result_t nova_boot_layer_destroy(
    nova_boot_layer_id_t layer_id
);

nova_result_t nova_boot_layer_attach(
    nova_boot_layer_id_t parent_id,
    nova_boot_layer_id_t child_id
);

nova_result_t nova_boot_layer_detach(
    nova_boot_layer_id_t layer_id
);

nova_result_t nova_boot_layer_set_visible(
    nova_boot_layer_id_t layer_id,
    bool visible
);

nova_result_t nova_boot_layer_set_bounds(
    nova_boot_layer_id_t layer_id,
    const nova_rect_f32_t* bounds
);

nova_result_t nova_boot_layer_set_opacity(
    nova_boot_layer_id_t layer_id,
    float opacity
);

nova_result_t nova_boot_layer_set_transform(
    nova_boot_layer_id_t layer_id,
    const nova_mat3x3_t* transform
);

nova_result_t nova_boot_layer_mark_dirty(
    nova_boot_layer_id_t layer_id,
    const nova_rect_i32_t* region
);
```

---

## Surface-Verwaltung

```c
nova_result_t nova_boot_surface_manager_initialize(
    const nova_boot_surface_manager_config_t* config
);

nova_result_t nova_boot_surface_create(
    const nova_boot_surface_descriptor_t* descriptor,
    nova_boot_surface_id_t* out_surface_id
);

nova_result_t nova_boot_surface_destroy(
    nova_boot_surface_id_t surface_id
);

nova_result_t nova_boot_surface_retain(
    nova_boot_surface_id_t surface_id
);

nova_result_t nova_boot_surface_release(
    nova_boot_surface_id_t surface_id
);

nova_result_t nova_boot_surface_lock(
    nova_boot_surface_id_t surface_id,
    nova_boot_surface_mapping_t* out_mapping
);

nova_result_t nova_boot_surface_unlock(
    nova_boot_surface_id_t surface_id,
    const nova_rect_i32_t* modified_region
);

nova_result_t nova_boot_surface_clear(
    nova_boot_surface_id_t surface_id,
    nova_color_t color
);

nova_result_t nova_boot_surface_get_info(
    nova_boot_surface_id_t surface_id,
    nova_boot_surface_info_t* out_info
);
```

---

# 57. Konfiguration

```c
typedef struct nova_boot_surface_manager_config
{
    uint64_t total_surface_budget;
    uint64_t persistent_budget;
    uint64_t scene_budget;
    uint64_t effect_budget;
    uint64_t frame_budget;
    uint64_t emergency_budget;

    uint32_t maximum_surface_count;
    uint32_t maximum_layer_count;
    uint32_t maximum_layer_depth;
    uint32_t maximum_damage_regions;

    bool enable_surface_reuse;
    bool enable_layer_caching;
    bool enable_triple_buffering;

    nova_boot_quality_profile_t quality_profile;
} nova_boot_surface_manager_config_t;
```

Alle Konfigurationswerte müssen vor der Initialisierung validiert werden.

---

# 58. Diagnose

Das System erfasst mindestens:

- Anzahl aktiver Layer
- maximale Layer-Tiefe
- Anzahl aktiver Surfaces
- Anzahl persistenter Surfaces
- Anzahl temporärer Surfaces
- aktueller Surface-Speicherverbrauch
- maximaler Speicherverbrauch
- Cache-Hits
- Cache-Misses
- Surface-Wiederverwendungen
- Surface-Allokationsfehler
- durchschnittliche Damage-Fläche
- Anzahl vollständiger Frames
- Anzahl partieller Frames
- Present-Zeit
- Compositing-Zeit

---

# 59. Debug-Overlay

Im Debugmodus können dargestellt werden:

- Layer-Grenzen
- Surface-Grenzen
- Layer-IDs
- Surface-IDs
- Z-Reihenfolge
- Dirty Regions
- Clip-Regionen
- Surface-Speicherverbrauch
- Cache-Status
- isolierte Layer
- Effekt-Surfaces
- Present-Regionen

Das Debug-Overlay darf die reguläre Layerstruktur nicht verändern.

---

# 60. Sicherheitsanforderungen

Das System muss verhindern:

- Schreiben außerhalb einer Surface
- Integer-Overflow bei Größenberechnungen
- Use-after-free
- Double Release
- Zyklen in der Layerhierarchie
- ungültige Parent-Child-Zuordnungen
- Surface-Nutzung nach Generationenwechsel
- Lesen undefinierter Pixelbereiche
- Referenzzählerüberläufe
- Freigabe gelockter Surfaces
- unvalidierte externe Framebuffer-Adressen

Alle extern bereitgestellten Surfaces müssen als nicht vertrauenswürdig behandelt und validiert werden.

---

# 61. Generation Handles

Surface- und Layer-Handles sollen eine Generation enthalten.

Beispiel:

```c
typedef struct nova_boot_handle
{
    uint32_t index;
    uint32_t generation;
} nova_boot_handle_t;
```

Dadurch werden veraltete Handles erkannt.

Ein Handle ist ungültig, wenn seine Generation nicht mit der aktuellen Slot-Generation übereinstimmt.

---

# 62. Grenzwerte

Empfohlene Baseline:

| Wert | Grenze |
|---|---:|
| Maximale Layer | 512 |
| Maximale Layer-Tiefe | 32 |
| Maximale Surfaces | 256 |
| Maximale Damage Regions pro Surface | 32 |
| Maximale Surface-Breite | 8192 Pixel |
| Maximale Surface-Höhe | 8192 Pixel |
| Maximale einzelne Surface-Größe | konfigurierbar, standardmäßig 32 MiB |
| Maximale verschachtelte Masken | 8 |
| Maximale isolierte Effekt-Layer gleichzeitig | 16 |

Überschreitungen müssen kontrolliert abgewiesen werden.

---

# 63. Performanceziele

| Operation | Zielwert |
|---|---:|
| Layer-Lookup | O(1) |
| Surface-Lookup | O(1) |
| Layer-Erzeugung | unter 10 µs |
| Surface-Pool-Zuweisung | unter 20 µs |
| Surface-Reuse | unter 5 µs |
| Layer-Sortierung bei unverändertem Baum | keine erneute Vollsortierung |
| Damage-Zusammenführung | unter 250 µs |
| Layer-Traversierung | unter 1 ms bei 500 Layern |
| Surface-Metadatenverwaltung | unter 1 % der Framezeit |

Die Zielwerte gelten für typische Desktop-Hardware und dürfen auf sehr alter Hardware durch Qualitätsdegradation abgesichert werden.

---

# 64. Testanforderungen

## 64.1 Layer-Tests

- Layer erzeugen
- Layer zerstören
- Parent wechseln
- Child anhängen
- Child entfernen
- Z-Reihenfolge
- Sichtbarkeit
- Opazität
- Transformationen
- maximale Verschachtelung
- Zyklenerkennung

---

## 64.2 Surface-Tests

- Surface erzeugen
- Surface freigeben
- Surface wiederverwenden
- verschiedene Größen
- verschiedene Pitch-Werte
- verschiedene Pixelformate
- Lock und Unlock
- Referenzzählung
- Generation Handles
- Budgetüberschreitung

---

## 64.3 Damage-Tests

- einzelne Region
- mehrere Regionen
- überlappende Regionen
- Regionen außerhalb der Surface
- Transformation von Child zu Parent
- Blur-Ausweitung
- Schatten-Ausweitung
- Wechsel auf Full Damage

---

## 64.4 Buffering-Tests

- Double-Buffering
- Triple-Buffering
- Rückfall von Triple auf Double
- partielles Present
- vollständiges Present
- fehlgeschlagener Present-Vorgang

---

## 64.5 Effekt-Tests

- Layer-Opacity
- Masken
- Hintergrundunschärfe
- Schatten
- Dialog-Glass
- animierte Transformationen
- Effektdegradation

---

## 64.6 Fehler- und Recovery-Tests

- Surface-Pool erschöpft
- ungültige Surface-ID
- ungültige Layer-ID
- beschädigte Hierarchie
- gesperrte Surface
- Reference Count Overflow
- Present-Fehler
- Emergency-Surface-Aktivierung

---

# 65. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle visuellen Ebenen über das Layer-System verwaltet werden
- jeder Layer eine stabile und deterministische Z-Reihenfolge besitzt
- Offscreen-Surfaces nur bei technischem Bedarf erzeugt werden
- Surface-Allokationen das definierte Speicherbudget nicht überschreiten
- keine allgemeine Speicherallokation im Render-Hotpath erfolgt
- Dirty Regions korrekt zwischen Layern und Surfaces propagiert werden
- Double-Buffering zuverlässig funktioniert
- Triple-Buffering bei Speicherknappheit deaktiviert wird
- Glass-Dialoge mit korrekt isolierten Surfaces dargestellt werden
- ungültige Handles erkannt werden
- keine Surface außerhalb ihrer Grenzen gelesen oder beschrieben wird
- Fehler automatisch auf reduzierte Qualitätsstufen zurückfallen
- der Emergency Layer auch bei Ausfall regulärer Layer funktioniert
- BIOS und UEFI dieselbe logische Layerarchitektur verwenden
- alle verpflichtenden Tests bestanden werden

---

# 66. Referenzen

## ADRs

- ADR-BOOTUI-0005 – Double- und Triple-Buffering
- ADR-BOOTUI-0006 – Retained-Mode-Szenengraph
- ADR-BOOTUI-0007 – Dirty-Region-Rendering
- ADR-BOOTGFX-0003 – Premultiplied Alpha
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTGFX-0005 – Offscreen-Surfaces
- ADR-BOOTGFX-0006 – Clipping- und Maskensystem
- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTPERF-0001 – Festes Speicherbudget für die Boot-UI
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTPERF-0004 – Automatische Effektdegradation
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren

## NPSPECs

- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTUI-0004 – Boot UI State Model
- NPSPEC-BOOTUI-0007 – Boot UI Memory Model
- NPSPEC-BOOTUI-0008 – Boot UI Error Recovery
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0002 – Render Command System
- NPSPEC-BOOTRENDER-0003 – Scene Graph
- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation
- NPSPEC-BOOTRENDER-0009 – Dirty Region Rendering
- NPSPEC-BOOTRENDER-0010 – Clipping and Masks
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor

---

# 67. Zusammenfassung

Das **Layer and Surface Management** bildet die zentrale Verwaltungsarchitektur für Darstellungsebenen und Pixelspeicher der NovaOS-Bootoberfläche.

Layer beschreiben die logische Hierarchie, Z-Reihenfolge, Sichtbarkeit, Opazität, Transformation und Effekte der Benutzeroberfläche. Surfaces stellen den dafür erforderlichen konkreten Pixelspeicher bereit. Beide Konzepte bleiben strikt voneinander getrennt.

Durch bedarfsgesteuerte Surface-Isolation, feste Speicherpools, Surface-Recycling, stabile Handles, Damage-Tracking und automatische Effektdegradation kann die Boot-UI hochwertige Funktionen wie Glass-Dialoge, Schatten, Animationen und Hintergrundunschärfe bereitstellen, ohne ihre deterministische Speicher- und Laufzeitarchitektur zu gefährden.

Das System stellt außerdem sicher, dass Double- und optionales Triple-Buffering zuverlässig funktionieren, alle Layer in einer reproduzierbaren Reihenfolge komponiert werden und bei Speicher- oder Renderingfehlern jederzeit auf einfachere Qualitätsprofile oder den Emergency Layer zurückgegriffen werden kann.