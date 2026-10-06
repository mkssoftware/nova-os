# NPSPEC-USERSPACE-LAYOUT-0001 – Nova Userspace Layout

## Status

Angenommen

## Kategorie

Userspace / Layout

## Zweck

NovaOS definiert eine einheitliche logische Grundstruktur des Userspace.

Die Struktur beschreibt die sichtbaren Systembereiche, ohne deren physischen Speicherort festzulegen.

```text
/
├── System/
├── Benutzer/
├── Apps/
├── Solutions/
├── Boot/
└── Volumes/
```

## Grundprinzipien

```text
Namespace Layout ≠ Physical Storage
Path ≠ Object Identity
Visibility ≠ Authority
```

Das Layout bleibt unabhängig davon, auf welchem Device oder Volume die zugrunde liegenden Daten gespeichert sind.

## System

```text
/System
```

Enthält systemweite Komponenten, Dienste, Bibliotheken, Konfigurationen und Ressourcen.

Normale Programme dürfen `/System` nicht ohne entsprechende Capability verändern.

Private Programmabhängigkeiten werden nicht global installiert, sondern über das programmspezifische `SYS`-Overlay eingebunden.

## Benutzer

```text
/Benutzer
```

Enthält die persönlichen Bereiche der Benutzer.

```text
/Benutzer/<User>/
```

Benutzerdaten bleiben logisch von System-, Programm- und Solution-Ressourcen getrennt.

## Apps

```text
/Apps
```

Enthält klassische installierte Programme und deren private Ressourcen.

Beispiel:

```text
/Apps/Example/
├── App
├── Resources/
└── SYS/
```

`SYS` kann private Abhängigkeiten enthalten, die für das jeweilige Programm in dessen effektiven System-Namespace projiziert werden.

## Solutions

```text
/Solutions
```

Enthält NovaOS-Solutions einschließlich ihrer UI-, Logic- und zugehörigen Solution-Ressourcen.

Solutions bleiben logisch von klassischen monolithischen Programmen unter `/Apps` getrennt.

## Boot

```text
/Boot
```

Stellt kontrolliert benötigte Boot- und Recovery-Ressourcen bereit.

Der Bereich unterliegt erhöhten Sicherheitsanforderungen und darf nicht als allgemeiner Benutzerspeicher verwendet werden.

## Volumes

```text
/Volumes
```

Dient als Namespace-Bereich für zusätzlich eingebundene Volumes.

Beispiel:

```text
/Volumes/Daten
/Volumes/Backup
```

Der sichtbare Pfad ist nicht die Identität des Volumes.

## Erweiterbarkeit

Weitere logische Bereiche dürfen ergänzt oder als Projections bereitgestellt werden.

Das Root-Layout soll jedoch klein und stabil bleiben.

Semantische Ansichten müssen nicht als dauerhaft physische Root-Verzeichnisse existieren.

## Lokalisierung

Sichtbare Systembezeichnungen dürfen lokalisiert werden.

Die interne Namespace-Identität bleibt davon unabhängig.

```text
Benutzer ↔ User
```

Ein Sprachwechsel verändert weder Objekte noch Namespace-Identitäten.

## Sicherheit

Jeder Bereich unterliegt dem Capability- und Permission-Modell.

Die Position eines Objekts innerhalb des Layouts verleiht keine Authority.

## Normative Anforderungen

1. NovaOS MUSS einen gemeinsamen logischen Root-Namespace verwenden.
2. `/System`, `/Benutzer`, `/Apps`, `/Solutions`, `/Boot` und `/Volumes` MÜSSEN als grundlegende logische Bereiche unterstützt werden.
3. Das Userspace-Layout DARF NICHT an physische Storage-Strukturen gekoppelt sein.
4. `/Apps` und `/Solutions` MÜSSEN logisch getrennt bleiben.
5. Private Programmabhängigkeiten SOLLEN über das `SYS`-Overlay bereitgestellt werden.
6. Zusätzliche Volumes SOLLEN über `/Volumes` in den Namespace integriert werden.
7. Systembereiche MÜSSEN lokalisierbar sein, ohne ihre interne Identität zu verändern.
8. Das Root-Layout SOLL klein und stabil bleiben.
9. Namespace-Position DARF KEINE Authority erzeugen.
10. Erweiterungen SOLLEN bevorzugt über Namespace-Projections erfolgen.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-LOCALIZATION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-MOUNT-0001`

## Ergebnis

NovaOS erhält ein kleines und stabiles Userspace-Grundlayout. Die sichtbare Struktur bleibt von physischer Speicherung, Volume-Zuordnung und Objektidentität getrennt und kann durch Projections erweitert werden, ohne den Root-Namespace unnötig aufzublähen.