# NPSPEC-STORAGE-APP-SYS-0001 – Nova App/System Storage Separation

## Status

Entwurf

## Kategorie

Storage / Application Storage / System Storage / Isolation

## Zweck

NovaOS definiert eine klare Trennung zwischen System-, App- und Benutzerdaten.

```text
Storage
├── System
├── Apps
└── Benutzer
```

Apps dürfen ihre Dateien nicht beliebig im System verteilen. Dadurch bleiben Installation, Updates, Entfernung, Recovery und Berechtigungen kontrollierbar.

## Grundprinzip

```text
App Storage ≠ System Storage
App Storage ≠ User Storage
Installation ≠ Arbitrary File Placement
```

Jede Datenklasse besitzt einen definierten Verantwortungs- und Sicherheitsbereich.

## Storage-Bereiche

NovaOS unterscheidet mindestens:

```text
System:/
Apps:/
Benutzer:/
Boot:/
```

Dabei sind die sichtbaren Namen Namespace-Projektionen und nicht zwingend physische Volumes.

## System Storage

`System:/` enthält vom Betriebssystem verwaltete Komponenten.

Beispiele:

```text
Kernel
Drivers
System Modules
Capabilities
System Configuration
Trusted Runtime Components
```

Normale Apps dürfen diesen Bereich nicht direkt verändern.

Systemänderungen müssen über autorisierte Systemmechanismen erfolgen.

## App Storage

Apps erhalten einen eigenen Storage-Bereich.

```text
Apps:/
└── <AppID>/
    ├── Package
    ├── Data
    ├── Cache
    └── State
```

Die stabile `AppID` dient als primäre Zuordnung.

Eine App darf standardmäßig nicht auf private Daten anderer Apps zugreifen.

## Benutzer Storage

Benutzerdaten bleiben von App-Installationen getrennt.

```text
Benutzer:/
├── Dokumente
├── Bilder
├── Projekte
└── ...
```

Apps erhalten Zugriff auf Benutzerobjekte nur über entsprechende Capabilities, Freigaben oder explizite Benutzerentscheidungen.

## App-Paket

Installierte App-Komponenten sollen von veränderlichen App-Daten getrennt werden.

```text
App
├── Package   → möglichst unveränderlich
├── Data      → persistent
├── Cache     → reclaimable
└── State     → Laufzeit-/Sessionzustand
```

Dadurch kann ein App-Paket aktualisiert oder ersetzt werden, ohne automatisch Benutzerdaten zu verändern.

## Installation

Eine Installation muss über den kontrollierten NovaOS-App-Mechanismus erfolgen.

```text
Package
   ↓
Verify
   ↓
Authorize
   ↓
Transaction
   ↓
Apps:/<AppID>
```

Apps dürfen während der Installation keine beliebigen Systempfade beschreiben.

## Updates

App-Updates sollen das bestehende Paket kontrolliert ersetzen.

```text
Old Package
     ↓
New Package
     ↓
Verify
     ↓
Atomic Switch
```

App-Daten bleiben dabei grundsätzlich erhalten.

Ein fehlgeschlagenes Update soll auf den vorherigen funktionsfähigen Zustand zurückfallen können.

## Deinstallation

Bei der Deinstallation müssen Paket und App-Daten getrennt behandelt werden.

```text
Remove App
├── Package → Remove
├── Cache   → Remove
└── Data    → Policy / User Decision
```

Persistente Benutzerdaten dürfen nicht unbeabsichtigt gelöscht werden.

## Shared Data

Apps dürfen gemeinsame Daten verwenden, wenn dies explizit autorisiert ist.

```text
App A ─┐
       ├→ Shared Object
App B ─┘
```

Gemeinsame Daten sollen über stabile Storage-Objekte und Capabilities bereitgestellt werden, nicht über unkontrollierte gemeinsame Verzeichnisse.

## Systemschutz

Kritische Systembereiche sollen standardmäßig read-only oder anderweitig gegen direkte Veränderungen geschützt sein.

Temporäre Änderungen dürfen über kontrollierte Overlays erfolgen.

```text
System Base
    +
Authorized Overlay
    ↓
System View
```

## Recovery

Die Trennung muss ermöglichen, Systemkomponenten wiederherzustellen, ohne Benutzer- oder App-Daten unnötig zu verändern.

```text
System Recovery
      ↓
Restore System
      ↓
Preserve Apps / User Data
```

Welche Bereiche erhalten bleiben, wird durch die jeweilige Recovery-Policy bestimmt.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
AppID
Package Location
Data Location
Storage Usage
Permissions
Shared Objects
Cache Usage
Storage State
```

## Normative Anforderungen

1. NovaOS MUSS System-, App- und Benutzerspeicher logisch trennen.
2. Apps DÜRFEN System Storage NICHT direkt verändern.
3. Jede installierte App MUSS über eine stabile `AppID` identifizierbar sein.
4. App-Paket und veränderliche App-Daten SOLLEN getrennt gespeichert werden.
5. Apps DÜRFEN standardmäßig NICHT auf private Daten anderer Apps zugreifen.
6. Zugriff auf Benutzerdaten MUSS durch Capabilities oder explizite Freigaben kontrolliert werden.
7. Apps DÜRFEN während der Installation keine beliebigen Systempfade beschreiben.
8. App-Updates SOLLEN transaktional und rollbackfähig erfolgen.
9. Deinstallation DARF persistente Benutzerdaten NICHT unbeabsichtigt entfernen.
10. App-Caches MÜSSEN als reclaimable gekennzeichnet werden können.
11. System-Recovery SOLL unabhängig von Benutzer- und App-Daten möglich sein.
12. Storage-Nutzung und Zuordnung einer App SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-OVERLAY-0001`
- `ADR-STORAGE-0016`

## Ergebnis

```text
System:/   → geschütztes Betriebssystem
Apps:/     → kontrollierte App-Bereiche
Benutzer:/ → persistente Benutzerdaten
```

NovaOS erhält damit eine klare Storage-Trennung, durch die Apps kontrolliert installiert, aktualisiert und entfernt werden können, ohne System- oder Benutzerdaten unnötig miteinander zu vermischen.