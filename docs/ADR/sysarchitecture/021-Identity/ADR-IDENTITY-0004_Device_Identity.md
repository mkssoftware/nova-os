# ADR-IDENTITY-0004 – Device Identity

## Status

Angenommen

## Kategorie

Identity Architecture / Device Identity / Hardware

## Kontext

NovaOS muss physische und virtuelle Geräte eindeutig identifizieren können.

Hardwareadressen, Buspositionen, Gerätenamen oder Treiberinstanzen sind dafür ungeeignet, da sie sich durch Neustart, Hotplug, Migration oder Hardwarekonfiguration ändern können.

Gleichzeitig muss zwischen einem logischen Gerät und seiner aktuellen Verbindung oder Laufzeitinstanz unterschieden werden.

## Entscheidung

NovaOS führt **Device Identity als eigenständige Identity-Klasse** ein.

```text
Device Identity
      ↓
Current Device Instance
      ↓
Bus / Driver / Address / Interface
```

Die Device Identity beschreibt das Gerät unabhängig von seiner aktuellen technischen Anbindung.

## Grundprinzip

```text
Device Identity = What device is this?

Device Address = Where is it connected?

Driver = How is it controlled?
```

Diese Konzepte bleiben voneinander getrennt.

## Stabile und temporäre Device Identity

Wo Hardware eine verlässlich überprüfbare langlebige Identität besitzt, kann NovaOS eine stabile Device Identity verwenden.

Ist dies nicht möglich, kann eine lifecyclegebundene Device Identity erzeugt werden.

```text
Device
 ├── Stable Identity
 └── Ephemeral Identity
```

NovaOS darf keine Stabilität vortäuschen, die technisch nicht verlässlich festgestellt werden kann.

## Identity Sources

Zur Ermittlung oder Verifikation einer Device Identity können geeignete Hardware- und Plattforminformationen verwendet werden, beispielsweise:

```text
Hardware-backed Identity
Device Certificates
Secure Element
Firmware Identity
Platform-provided Identifiers
System-generated Bindings
```

Ein einzelner veränderlicher Identifier muss nicht zwangsläufig die Device Identity darstellen.

Die konkrete Ableitung und Vertrauensbewertung wird in NPSPECs definiert.

## Device Instance

Eine konkrete Verbindung oder Aktivierung eines Geräts wird von seiner stabilen Identity getrennt.

```text
Device Identity
    ├── Instance / Connection A
    └── Instance / Connection B
```

Hotplug oder erneute Enumeration dürfen daher nicht automatisch eine neue stabile Device Identity erzeugen.

## HAL und Device Model

Device Identity wird in den deklarativen Hardware-Systemgraph integriert.

```text
Hardware System Graph
        ↓
Device Node
        ↓
Device Identity
        ↓
Driver Binding
        ↓
Device Capabilities
```

Busposition, Treiber und Capability Provider bleiben austauschbare oder veränderliche Eigenschaften des Geräts.

## Security und Trust

Device Identity erzeugt keine automatische Authority oder Vertrauensstellung.

```text
Device Identity ≠ Trust
Device Identity ≠ Capability
```

Trust kann anhand verifizierter Hardwareeigenschaften, Provenance, Attestation oder Policy bestimmt werden.

Zugriff auf das Gerät bleibt Capability-basiert.

## Austausch und Ersatz

Der physische Austausch eines Geräts darf nicht automatisch als Fortsetzung derselben Device Identity gelten.

Soll ein Ersatzgerät eine bestehende logische Rolle übernehmen, muss diese Zuordnung explizit erfolgen.

Damit werden:

```text
Physical Device Identity
```

und:

```text
Logical Device Role
```

nicht miteinander vermischt.

## Privacy

Hardware-Identifier können Tracking ermöglichen.

Device Identity darf deshalb nicht automatisch vollständig gegenüber Anwendungen oder entfernten Diensten offengelegt werden.

NovaOS muss pseudonyme, eingeschränkte oder kontextgebundene Device References bereitstellen können.

## Normative Anforderungen

1. NovaOS MUSS Device Identity als eigenständige Identity-Klasse unterstützen.
2. Device Identity MUSS von Busadresse, Gerätename, Treiber und Runtime Handle getrennt sein.
3. Hotplug und erneute Enumeration DÜRFEN eine stabile Device Identity nicht automatisch verändern.
4. NovaOS MUSS stabile und lifecyclegebundene Device Identities unterstützen können.
5. Nicht verifizierbare Stabilität DARF NICHT als stabile Identity dargestellt werden.
6. Device Identity MUSS in HAL und deklaratives Device Model integrierbar sein.
7. Driver Replacement DARF die zugrunde liegende Device Identity nicht automatisch verändern.
8. Device Identity DARF keine automatische Authority oder Trust-Einstufung erzeugen.
9. Physische Device Identity und logische Device Role MÜSSEN getrennt bleiben.
10. Device Identity MUSS datenschutzgerecht selektiv oder pseudonymisiert offengelegt werden können.
11. Anwendungen DÜRFEN nicht automatisch Zugriff auf globale Hardware-Identifier erhalten.
12. Device Identity und relevante Bindings MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Geräte bleiben über Hotplug und Treiberwechsel identifizierbar,
- Hardwareidentität wird von technischer Anbindung getrennt,
- Device Trust und Device Authority können sauber getrennt werden,
- bessere Unterstützung für Hot Replacement und Hardwaretopologie,
- globale Hardware-Identifier müssen Anwendungen nicht direkt offengelegt werden.

### Negative Konsequenzen

- nicht jede Hardware besitzt eine verlässlich stabile Identität,
- Identity Binding und Privacy-Projektion benötigen zusätzliche Systemlogik.

## Verworfene Alternativen

### Busadresse als Device Identity

Verworfen, da Buspositionen veränderlich sind.

### Treiberinstanz als Device Identity

Verworfen, da Treiber austauschbar sind.

### Seriennummer grundsätzlich als Device Identity

Verworfen, da Seriennummern fehlen, unzuverlässig oder datenschutzkritisch sein können.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-DRIVER-0009_Device_Model_als_deklarativer_Systemgraph`
- `ADR-CAPABILITY-0012_Capability_Based_Drivers`
- `ADR-PRIVACY-0005_Selective_Disclosure`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-DEVICE-BINDING-0001`
- `NPSPEC-IDENTITY-DEVICE-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-DEVICE-PRIVACY-0001`
- `NPSPEC-IDENTITY-DEVICE-TEST-0001`

## Ergebnis

NovaOS trennt die Identität eines Geräts von seiner aktuellen technischen Einbindung:

```text
Device Identity
      ↓
Device Instance
      ↓
HAL / Driver
      ↓
Device Capabilities
```

Damit können Geräte stabil und sicher referenziert werden, ohne Busadressen, Treiber oder globale Hardware-Identifier fälschlich zur eigentlichen Identität des Geräts zu machen.