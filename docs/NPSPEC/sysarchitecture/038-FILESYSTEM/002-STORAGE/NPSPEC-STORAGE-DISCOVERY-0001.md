# NPSPEC-STORAGE-DISCOVERY-0001 – Nova Storage Discovery

## Status

Angenommen

## Kategorie

Storage / Discovery

## Zweck

NovaOS erkennt verfügbare Storage-Ressourcen dynamisch und überführt sie in das einheitliche Storage-Modell.

Discovery erkennt Ressourcen, erzeugt jedoch keine Zugriffsberechtigung.

```text
Detected ≠ Trusted ≠ Authorized ≠ Mounted
```

## Discovery-Modell

```text
Hardware / Virtual Resource
        ↓
Driver / Provider
        ↓
Storage Discovery
        ↓
Identification
        ↓
Validation
        ↓
Storage Registry
```

Erkannt werden können insbesondere:

- physische Storage Devices
- virtuelle Devices
- Partitionen und Storage-Regionen
- Volumes
- unterstützte Filesysteme

## Identifikation

Erkannte Ressourcen werden mit ihrer bestehenden stabilen Identität verknüpft oder erhalten bei erstmaliger Registrierung eine entsprechende NovaOS-Identität.

```text
Device → DeviceID
Volume → VolumeID
```

Temporäre Hardwarepfade oder Anschlusspositionen dürfen nicht als dauerhafte Identität verwendet werden.

## Discovery Events

Storage Discovery muss dynamische Änderungen unterstützen:

```text
Added
Changed
Removed
Unavailable
```

Damit können Hotplug, virtuelle Storage-Ressourcen und Änderungen der Storage-Topologie verarbeitet werden.

## Validierung

Vor der weiteren Verwendung müssen erkannte Ressourcen geprüft werden.

Dazu können gehören:

```text
Device State
Identity
Storage Layout
Filesystem Type
Integrity
Encryption State
Compatibility
```

Unbekannte oder beschädigte Ressourcen dürfen nicht automatisch als sicher verwendbar gelten.

## Registry

Validierte Ressourcen werden in einer Storage Registry registriert.

```text
Storage Registry
├── DeviceID
├── VolumeID
├── Type
├── State
└── Capabilities
```

Die Registry dient Discovery und Introspection, nicht als Authority Store.

## Automatische Weiterverarbeitung

Nach erfolgreicher Discovery können weitere Mechanismen ausgelöst werden:

```text
Volume Discovery
Filesystem Detection
Health Check
Policy Evaluation
Mount Evaluation
```

Discovery selbst führt jedoch nicht automatisch zu einem Mount.

## Sicherheit

Für erkannte Ressourcen gilt:

```text
Discovered
   ↓
Policy / Trust / Capability
   ↓
Authorized Operation
```

Ein Prozess darf durch Discovery keine zusätzlichen Rechte auf das erkannte Device oder Volume erhalten.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Discovered Resources
Identity
Type
State
Capabilities
Discovery Source
```

## Normative Anforderungen

1. NovaOS MUSS Storage-Ressourcen dynamisch erkennen können.
2. Physische und virtuelle Storage Devices MÜSSEN unterstützt werden.
3. Discovery MUSS stabile Storage-Identitäten verwenden.
4. Temporäre Hardwarepfade DÜRFEN NICHT als dauerhafte Identität gelten.
5. Hotplug MUSS über Discovery Events abbildbar sein.
6. Erkannte Ressourcen MÜSSEN vor sicherheitskritischer Verwendung validierbar sein.
7. Discovery DARF KEINE Authority erzeugen.
8. Discovery DARF NICHT automatisch einen Mount erzwingen.
9. Entfernte Ressourcen MÜSSEN als nicht mehr verfügbar markiert werden.
10. Discovery-Ergebnisse MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-IDENTITY-0001`
- `NPSPEC-STORAGE-MOUNT-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `NPSPEC-DRIVER-MODEL-0001`

## Ergebnis

NovaOS kann Storage-Ressourcen automatisch erkennen, identifizieren und registrieren. Discovery bleibt dabei strikt von Mounting und Authority getrennt und bildet die Grundlage für die weitere sichere Verarbeitung durch Volume-, Filesystem- und Namespace-Schichten.