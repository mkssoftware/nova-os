# NPSPEC-UNITS-0003
# Unit Registry and Conversion Engine

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0003 |
| Titel | Unit Registry and Conversion Engine |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Teil | 1 von 6 |
| Kapitel | 1–10 |
| Abhängigkeiten | NPSPEC-COMPUTE-0001, NPSPEC-UNITS-0001, NPSPEC-UNITS-0002, NPSPEC-OBJECT-0001 |
| Zielplattformen | x86_64, AArch64, RISC-V |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Diese Spezifikation definiert die Unit Registry and Conversion Engine von Nova OS.

Die Unit Registry and Conversion Engine ist der zentrale Systemdienst für:

- Registrierung physikalischer Einheiten,
- Registrierung physikalischer Dimensionen,
- Registrierung von Präfixen,
- Registrierung von Einheitensystemen,
- Auflösung von Einheiten über Namen, Symbole und IDs,
- Validierung von Einheitenbeschreibungen,
- Konvertierung zwischen kompatiblen Einheiten,
- Verwaltung benutzerdefinierter Einheiten,
- Versionsverwaltung,
- Namespaces,
- Reflection,
- Caching,
- Serialisierung,
- Sicherheitsprüfung,
- Integration in Compiler und Compute Runtime.

Die Engine ist Bestandteil der Nova Compute Platform und stellt die verbindliche Referenz für alle Einheitenoperationen innerhalb von Nova OS dar.

---

# 2. Zielsetzung

Die Unit Registry and Conversion Engine verfolgt folgende Hauptziele:

1. Jede Einheit besitzt eine eindeutige Identität.
2. Jede Einheit ist eindeutig einer Dimension zugeordnet.
3. Jede Konvertierung ist vollständig definiert und nachvollziehbar.
4. Keine Anwendung darf eigene inkompatible Parallelregister verwenden.
5. Systemweite Einheiten müssen langzeitstabil adressierbar sein.
6. Einheiten müssen über Prozessgrenzen hinweg verlustfrei übertragen werden können.
7. Konvertierungen müssen deterministisch, sicher und performant sein.
8. Statisch bekannte Konvertierungen sollen zur Compile-Zeit ausgeführt werden können.
9. Dynamische Einheiten müssen zur Laufzeit sicher registrierbar sein.
10. Alle Compute-Dienste müssen dieselbe Registry verwenden.

---

# 3. Nicht-Ziele

Diese Spezifikation definiert nicht:

- mathematische Formelbibliotheken,
- konkrete Physikmodelle,
- vollständige Unsicherheitsrechnung,
- vollständige Naturkonstantenverwaltung,
- Tensorimplementierungen,
- Compilerinternas außerhalb der Unit-Metadaten,
- Benutzeroberflächen für wissenschaftliche Anwendungen,
- externe Währungsdienste,
- Sensor-Kalibrierungsmodelle.

Diese Bereiche werden in separaten NPSPEC-Dokumenten behandelt.

---

# 4. Grundprinzipien

## 4.1 Eine globale semantische Quelle

Nova OS besitzt eine zentrale, systemweit gültige Registry für Einheiten und Dimensionen.

Es darf keine voneinander abweichenden Systemdefinitionen für dieselbe Einheit geben.

Beispiel:

```text
Meter
```

muss in allen Nova-Komponenten dieselbe Dimension, dieselbe Basiseinheit, dieselbe Skalierung und dieselbe Identität besitzen.

## 4.2 Keine Konvertierung ohne Dimensionskompatibilität

Eine Konvertierung ist nur zulässig, wenn Quell- und Zieleinheit dimensionskompatibel sind.

Zulässig:

```text
Kilometer → Meter
Grad Celsius → Kelvin
bar → Pascal
```

Nicht zulässig:

```text
Kilogramm → Meter
Sekunde → Volt
Kelvin → Joule
```

## 4.3 Einheiten sind Objekte

Jede Einheit ist ein registriertes Nova-Objekt mit:

- stabiler ID,
- Namespace,
- Namen,
- Symbolen,
- Dimension,
- Konvertierungsmodell,
- Version,
- Herkunft,
- Vertrauensstufe,
- Dokumentation,
- optionaler Signatur.

## 4.4 IDs sind stabiler als Namen

Namen und Symbole dürfen lokalisiert oder ergänzt werden.

Die Identität einer Einheit darf jedoch nicht von ihrer sichtbaren Bezeichnung abhängen.

## 4.5 Konvertierungen sind explizit klassifiziert

Die Engine unterscheidet mindestens:

- lineare Konvertierung,
- affine Konvertierung,
- logarithmische Konvertierung,
- tabellenbasierte Konvertierung,
- kontextabhängige Konvertierung,
- benutzerdefinierte Konvertierung,
- nicht konvertierbare Beziehung.

## 4.6 Sicherheit vor Erweiterbarkeit

Benutzerdefinierte Einheiten dürfen die Integrität der Systemregistry nicht gefährden.

Erweiterbarkeit ist nur innerhalb klar definierter Namespaces und Berechtigungsgrenzen zulässig.

---

# 5. Systemarchitektur

Die Unit Registry and Conversion Engine wird als Kernel-naher Systemdienst ausgeführt.

```text
+----------------------------------------------------+
| Anwendungen, Treiber, Compute-Dienste             |
+----------------------------------------------------+
| Nova Units API                                     |
+----------------------------------------------------+
| Unit Registry Service                              |
| Dimension Registry                                 |
| Prefix Registry                                    |
| Namespace Registry                                 |
| Conversion Engine                                  |
| Conversion Cache                                   |
+----------------------------------------------------+
| Nova Object System                                 |
| Nova IPC                                           |
| Nova Security                                      |
| Nova Memory                                        |
+----------------------------------------------------+
| Nova Kernel                                        |
+----------------------------------------------------+
```

Die Engine besteht mindestens aus folgenden Kernkomponenten:

```text
nova_units/
├── registry/
│   ├── unit_registry
│   ├── dimension_registry
│   ├── prefix_registry
│   ├── namespace_registry
│   └── system_registry_loader
├── conversion/
│   ├── conversion_engine
│   ├── linear_converter
│   ├── affine_converter
│   ├── logarithmic_converter
│   ├── contextual_converter
│   └── conversion_graph
├── runtime/
│   ├── handle_manager
│   ├── registry_cache
│   ├── shared_registry_view
│   └── validation
└── api/
    ├── units
    ├── dimensions
    ├── prefixes
    ├── namespaces
    └── conversion
```

---

# 6. Betriebsmodi

## 6.1 Boot Registry Mode

Während des frühen Systemstarts steht eine minimale schreibgeschützte Registry bereit.

Sie enthält mindestens:

- sieben SI-Basisdimensionen,
- sieben SI-Basiseinheiten,
- elementare abgeleitete SI-Einheiten,
- grundlegende SI-Präfixe,
- elementare Konvertierungsregeln.

Dieser Modus darf keine dynamischen Registrierungen zulassen.

## 6.2 System Registry Mode

Nach dem Start des Unit Registry Service wird die vollständige Systemregistry geladen.

Sie enthält:

- offizielle Nova-Systemeinheiten,
- offizielle Dimensionen,
- Präfixe,
- Einheitensysteme,
- Aliasnamen,
- lokalisierte Namen,
- Versionsinformationen,
- Signaturen,
- Testmetadaten.

## 6.3 User Extension Mode

Benutzer und Anwendungen dürfen zusätzliche Einheiten innerhalb eigener Namespaces registrieren.

Beispiel:

```text
user.matthias.workshop.encoder_tick
com.example.robot.motor_step
org.research.lab.custom_scale
```

## 6.4 Restricted Mode

Im Recovery-, Safe- oder Security-Modus wird nur die signierte Systemregistry geladen.

Dynamische Erweiterungen und benutzerdefinierte Konvertierungen bleiben deaktiviert.

---

# 7. Identitätsmodell

## 7.1 Unit ID

Jede Einheit besitzt eine stabile Unit ID.

Die öffentliche Identität wird als 128-Bit-Wert definiert.

```c
typedef struct nova_unit_uuid
{
    uint64_t high;
    uint64_t low;
} nova_unit_uuid_t;
```

Die Unit ID darf nach Veröffentlichung nicht verändert oder wiederverwendet werden.

## 7.2 Dimension ID

Jede Dimension besitzt eine stabile Dimension ID.

```c
typedef struct nova_dimension_uuid
{
    uint64_t high;
    uint64_t low;
} nova_dimension_uuid_t;
```

## 7.3 Namespace ID

Namespaces besitzen ebenfalls stabile IDs.

```c
typedef uint64_t nova_namespace_id_t;
```

## 7.4 Runtime Handle

Für schnelle Laufzeitzugriffe werden kompakte Handles verwendet.

```c
typedef uint32_t nova_unit_handle_t;
typedef uint32_t nova_dimension_handle_t;
```

Ein Handle ist nur innerhalb einer Registry-Sitzung stabil.

Eine UUID ist langfristig und systemübergreifend stabil.

## 7.5 Reservierte IDs

Folgende Bereiche werden reserviert:

```text
00000000-0000-0000-0000-000000000000
Ungültige oder nicht gesetzte ID

00000000-0000-0000-0000-000000000001
Dimensionslos

00000000-0000-0000-0000-000000000002
Unbekannte Einheit

00000000-0000-0000-0000-000000000003
Benutzerdefinierte dynamische Einheit
```

Weitere reservierte Bereiche werden durch die Nova Assigned Identifier Registry verwaltet.

---

# 8. Namespace-Modell

## 8.1 Zweck

Namespaces verhindern Namenskollisionen und definieren Eigentümerschaft.

Beispiele:

```text
org.nova.si
org.nova.derived
org.nova.imperial
org.nova.astronomy
org.nova.information
com.vendor.product
user.account.project
```

## 8.2 System-Namespaces

System-Namespaces beginnen mit:

```text
org.nova.
```

Sie dürfen nur durch signierte Nova-Systemkomponenten verändert werden.

## 8.3 Hersteller-Namespaces

Drittanbieter verwenden einen organisationsbezogenen Namespace.

Beispiel:

```text
com.acme.robotics
de.example.measurement
org.research.institute
```

## 8.4 Benutzer-Namespaces

Benutzerdefinierte Einheiten werden standardmäßig unter einem privaten Namespace registriert.

```text
user.<account-id>.<subnamespace>
```

## 8.5 Namespace Descriptor

```c
typedef struct nova_namespace_descriptor
{
    nova_namespace_id_t id;
    const char* canonical_name;
    const char* owner;
    nova_namespace_flags_t flags;
    nova_trust_level_t trust_level;
    nova_version_t version;
} nova_namespace_descriptor_t;
```

## 8.6 Namespace-Auflösung

Die Auflösung erfolgt in folgender Reihenfolge:

1. vollständig qualifizierter Name,
2. explizit importierter Namespace,
3. aktueller Anwendungsnamespace,
4. offizielle Nova-Namespaces,
5. lokale Aliasauflösung.

Mehrdeutige Namen dürfen nicht stillschweigend aufgelöst werden.

---

# 9. Unit Descriptor

## 9.1 Grundstruktur

```c
typedef struct nova_unit_descriptor
{
    nova_object_header_t object;

    nova_unit_uuid_t uuid;
    nova_namespace_id_t namespace_id;

    const char* canonical_name;
    const char* primary_symbol;

    nova_dimension_uuid_t dimension_uuid;
    nova_unit_uuid_t reference_unit_uuid;

    nova_conversion_kind_t conversion_kind;

    long double scale;
    long double offset;

    nova_unit_flags_t flags;
    nova_version_t version;
    nova_trust_level_t trust_level;

    const char* source;
    const char* documentation_uri;
} nova_unit_descriptor_t;
```

## 9.2 Pflichtfelder

Jede Einheit muss mindestens definieren:

- UUID,
- Namespace,
- kanonischen Namen,
- Primärsymbol,
- Dimension,
- Referenzeinheit,
- Konvertierungsart,
- Skalierung oder Konvertierungsfunktion,
- Version,
- Vertrauensstufe.

## 9.3 Optionale Felder

Eine Einheit darf zusätzlich enthalten:

- alternative Symbole,
- lokalisierte Namen,
- veraltete Namen,
- Kurzbeschreibung,
- Normreferenz,
- Gültigkeitsbereich,
- zulässigen Wertebereich,
- Rundungsanforderung,
- Unsicherheitsmetadaten,
- digitale Signatur,
- Testvektoren.

## 9.4 Unit Flags

```c
typedef enum nova_unit_flags
{
    NOVA_UNIT_FLAG_NONE              = 0,
    NOVA_UNIT_FLAG_SYSTEM            = 1u << 0,
    NOVA_UNIT_FLAG_BASE              = 1u << 1,
    NOVA_UNIT_FLAG_DERIVED           = 1u << 2,
    NOVA_UNIT_FLAG_AFFINE            = 1u << 3,
    NOVA_UNIT_FLAG_LOGARITHMIC       = 1u << 4,
    NOVA_UNIT_FLAG_CONTEXTUAL        = 1u << 5,
    NOVA_UNIT_FLAG_DEPRECATED        = 1u << 6,
    NOVA_UNIT_FLAG_EXPERIMENTAL      = 1u << 7,
    NOVA_UNIT_FLAG_NON_SI            = 1u << 8,
    NOVA_UNIT_FLAG_LOCALIZED_SYMBOL  = 1u << 9,
    NOVA_UNIT_FLAG_SIGNED            = 1u << 10,
    NOVA_UNIT_FLAG_READ_ONLY         = 1u << 11
} nova_unit_flags_t;
```

## 9.5 Beispiel Meter

```text
Namespace:
org.nova.si

Canonical Name:
meter

Symbol:
m

Dimension:
length

Reference Unit:
meter

Conversion Kind:
linear

Scale:
1

Offset:
0

Flags:
SYSTEM | BASE | READ_ONLY
```

---

# 10. Dimension Descriptor

## 10.1 Grundstruktur

```c
typedef struct nova_dimension_descriptor
{
    nova_object_header_t object;

    nova_dimension_uuid_t uuid;
    nova_namespace_id_t namespace_id;

    const char* canonical_name;
    const char* symbol;

    nova_dimension_vector_t vector;
    nova_dimension_flags_t flags;

    nova_version_t version;
    nova_trust_level_t trust_level;

    const char* source;
    const char* documentation_uri;
} nova_dimension_descriptor_t;
```

## 10.2 Pflichtfelder

Jede Dimension muss mindestens definieren:

- UUID,
- Namespace,
- kanonischen Namen,
- Dimensionsvektor,
- Version,
- Vertrauensstufe.

## 10.3 Semantische Dimensionen

Mehrere semantische Größen dürfen denselben Dimensionsvektor besitzen.

Beispiel:

```text
Energie
Drehmoment
```

Beide besitzen dimensional:

```text
kg·m²·s⁻²
```

Sie sind semantisch jedoch nicht vollständig austauschbar.

Nova unterscheidet daher:

- physikalische Basisdimension,
- abgeleitete Dimension,
- semantische Quantity-Klasse.

## 10.4 Dimension Flags

```c
typedef enum nova_dimension_flags
{
    NOVA_DIMENSION_FLAG_NONE          = 0,
    NOVA_DIMENSION_FLAG_SYSTEM        = 1u << 0,
    NOVA_DIMENSION_FLAG_BASE          = 1u << 1,
    NOVA_DIMENSION_FLAG_DERIVED       = 1u << 2,
    NOVA_DIMENSION_FLAG_DIMENSIONLESS = 1u << 3,
    NOVA_DIMENSION_FLAG_SEMANTIC      = 1u << 4,
    NOVA_DIMENSION_FLAG_DEPRECATED    = 1u << 5,
    NOVA_DIMENSION_FLAG_READ_ONLY     = 1u << 6
} nova_dimension_flags_t;
```

## 10.5 Beispiel Kraft

```text
Canonical Name:
force

Symbol:
F

Dimensionsvektor:
[1, 1, -2, 0, 0, 0, 0]

SI-Einheit:
newton

Semantische Klasse:
mechanics.force
```

---

# Anhang A – Vorläufige Implementierungsstruktur für Teil 1

```text
include/nova/units/
├── unit_id.h
├── dimension_id.h
├── namespace.h
├── unit_descriptor.h
├── dimension_descriptor.h
├── registry_mode.h
└── unit_flags.h

src/units/
├── registry/
│   ├── unit_registry.c
│   ├── dimension_registry.c
│   ├── namespace_registry.c
│   └── system_registry_loader.c
├── runtime/
│   ├── unit_handle_table.c
│   ├── dimension_handle_table.c
│   └── registry_mode.c
└── validation/
    ├── unit_descriptor_validation.c
    ├── dimension_descriptor_validation.c
    └── namespace_validation.c
```

---

# Anhang B – Anforderungen aus Teil 1

Die Implementierung muss nach Abschluss von Teil 1 mindestens folgende Fähigkeiten bereitstellen:

- stabile Unit UUIDs,
- stabile Dimension UUIDs,
- Laufzeit-Handles,
- System- und Benutzer-Namespaces,
- schreibgeschützte Systemeinträge,
- Unit Descriptor,
- Dimension Descriptor,
- Registry-Betriebsmodi,
- frühe Boot-Registry,
- Validierung grundlegender Descriptoren,
- semantische Unterscheidung dimensionsgleicher Größen.

---

# Ausblick auf Teil 2

Teil 2 definiert die Kapitel 11–20:

```text
11. Registry Lifecycle
12. Registry Storage Model
13. System Registry Loading
14. Unit Registration
15. Dimension Registration
16. Alias and Symbol Registry
17. Prefix Registry
18. Unit Systems
19. Registry Lookup
20. Registry Validation
```
