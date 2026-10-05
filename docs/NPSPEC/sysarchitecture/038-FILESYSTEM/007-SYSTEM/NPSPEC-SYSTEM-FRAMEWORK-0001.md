# NPSPEC-SYSTEM-FRAMEWORK-0001 – Nova System Framework

## Status

Angenommen

## Kategorie

System / Framework

## Zweck

NovaOS definiert das System Framework als gemeinsame Plattform für höherwertige Systemfunktionen oberhalb der System Foundation.

Es stellt standardisierte APIs und Dienste bereit, damit Programme, Solutions und Systemkomponenten nicht eigene Implementierungen grundlegender Betriebssystemfunktionen benötigen.

## Grundprinzipien

```text
Framework ≠ Kernel
Framework ≠ Foundation
Framework ≠ Program
Framework ≠ Solution
Framework API ≠ Authority
```

## Architektur

```text
Programs / Solutions
        ↓
System Framework
        ↓
System Foundation
        ↓
System Services
        ↓
Kernel
```

Die Foundation stellt fundamentale Mechanismen bereit.

Das Framework stellt darauf aufbauende, höherwertige Funktionen und einheitliche APIs bereit.

## Framework-Bereiche

Das System Framework kann unter anderem Schnittstellen bereitstellen für:

```text
Filesystem
Storage
Networking
Devices
Media
Graphics
UI
Printing
Notifications
Localization
Settings
Search
Security
Cryptography
Authentication
Sharing
```

Die konkrete Funktion darf intern durch unterschiedliche Systemdienste oder Capability Provider umgesetzt werden.

## API-Modell

Programme sollen bevorzugt stabile Framework-Schnittstellen verwenden:

```text
Application Request
       ↓
Framework API
       ↓
Capability / Service Resolution
       ↓
Provider
       ↓
System Mechanism
```

Programme müssen dadurch die interne Implementierung eines Dienstes nicht kennen.

## Provider-Modell

Framework-Funktionen dürfen durch austauschbare Provider bereitgestellt werden.

```text
Framework Interface
      ↓
Provider A
Provider B
Provider C
```

Provider können aktualisiert oder ersetzt werden, solange der definierte Vertrag eingehalten wird.

## Capability-Integration

Framework-APIs erzeugen keine Authority.

Benötigt eine Operation geschützten Zugriff, muss die entsprechende Capability vorhanden sein.

```text
Framework Call
      ↓
Capability Check
      ↓
Provider
      ↓
Resource
```

## Versionierung

Framework-Schnittstellen müssen versionierbar und erweiterbar sein.

Neue Funktionen sollen bestehende kompatible Programme nicht unnötig brechen.

Nicht verfügbare Funktionen müssen eindeutig erkennbar sein.

## Introspection

Programme und Solutions müssen verfügbare Framework-Funktionen ermitteln können.

Dabei können mindestens folgende Informationen bereitgestellt werden:

```text
Interface
Version
Features
Provider
Availability
Requirements
```

Discovery erzeugt keine Authority.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames System Framework bereitstellen.
2. Das Framework MUSS logisch von Kernel und Foundation getrennt sein.
3. Programme und Solutions SOLLEN höherwertige Systemfunktionen über Framework-Schnittstellen verwenden.
4. Framework-Schnittstellen MÜSSEN von konkreten Provider-Implementierungen entkoppelt sein.
5. Mehrere Provider MÜSSEN für kompatible Funktionen unterstützt werden können.
6. Framework-APIs DÜRFEN keine zusätzliche Authority erzeugen.
7. Geschützte Operationen MÜSSEN bestehende Capability-Regeln einhalten.
8. Framework-Schnittstellen MÜSSEN versionierbar sein.
9. Nicht verfügbare Funktionen MÜSSEN eindeutig erkennbar sein.
10. Provider SOLLEN austauschbar und aktualisierbar sein.
11. Verfügbare Framework-Funktionen MÜSSEN introspektierbar sein.
12. Discovery und Introspection DÜRFEN keine Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt mit dem System Framework eine einheitliche höherwertige API-Schicht für Programme, Solutions und Systemkomponenten. Funktionen können durch austauschbare Provider umgesetzt werden, während stabile Schnittstellen, Capability-Sicherheit und die Trennung von Anwendung und interner Systemimplementierung erhalten bleiben.