# NPSPEC-SYSTEM-FOUNDATION-0001 – Nova System Foundation

## Status

Angenommen

## Kategorie

System / Foundation

## Zweck

NovaOS definiert die System Foundation als minimale gemeinsame Laufzeitbasis zwischen Kernel und höheren Systemdiensten.

Sie stellt fundamentale Systemfunktionen bereit, auf denen Services, Runtimes, Programme und Solutions aufbauen können, ohne direkt von Kernel-internen Implementierungen abhängig zu sein.

## Grundprinzipien

```text
Foundation ≠ Kernel
Foundation ≠ Application Runtime
Foundation ≠ Desktop Environment
Foundation ≠ System Service Collection
Stable Interface ≠ Stable Implementation
```

## Architektur

```text
Programs / Solutions
        ↓
System Services / Runtimes
        ↓
System Foundation
        ↓
Kernel Interfaces
        ↓
Kernel
```

Die Foundation bildet eine kontrollierte Grenze zwischen Kernelmechanismen und höherer Systemsoftware.

## Verantwortlichkeiten

Die System Foundation stellt grundlegende Abstraktionen bereit für:

```text
System Types
Object Identity
Error Model
Time
Memory Interfaces
Process / Task Interfaces
IPC Primitives
Capability Interfaces
Resource Handles
System Information
Lifecycle
```

Komplexe Fachlogik gehört nicht in die Foundation.

## Stable Core

Die Foundation soll einen kleinen, langfristig stabilen Kern besitzen.

```text
Stable Foundation API
        ↓
Internal Implementation
        ↓
Kernel ABI / IPC
```

Interne Implementierungen dürfen sich ändern, solange der definierte Vertrag erhalten bleibt.

## System Types

Gemeinsame grundlegende Typen werden systemweit eindeutig definiert.

Dazu gehören beispielsweise:

```text
ObjectID
ProcessID
TaskID
CapabilityID
ResourceID
Timestamp
Duration
Status
Error
Version
```

Subsysteme sollen keine inkompatiblen Paralleltypen für dieselbe grundlegende Bedeutung erzeugen.

## Fehlerbehandlung

Die Foundation stellt ein gemeinsames strukturiertes Fehlermodell bereit.

```text
Operation
   ↓
Success
oder
Structured Error
```

Fehler müssen maschinenlesbar sein und dürfen zusätzlich lokalisierbare Benutzerinformationen besitzen.

## Ressourcen

Systemressourcen werden über kontrollierte Handles oder stabile Identitäten angesprochen.

```text
Identity
   ↓
Authority Check
   ↓
Handle
   ↓
Operation
```

Ein Handle erzeugt keine über seinen ursprünglichen Sicherheitskontext hinausgehende Authority.

## Versionierung

Foundation-Schnittstellen müssen versionierbar sein.

Neue Funktionen sollen ergänzt werden können, ohne bestehende kompatible Software unnötig zu brechen.

Nicht unterstützte Funktionen müssen eindeutig erkennbar sein.

## Determinismus

Fundamentale Operationen sollen bei identischen definierten Eingaben ein vorhersehbares Verhalten besitzen.

Nichtdeterministische Eigenschaften wie Zeit, Zufall oder externe Ereignisse müssen explizit als solche behandelt werden.

## Sicherheit

Die Foundation darf Kernelmechanismen nicht ungeprüft an höhere Ebenen weiterreichen.

Capability-, Isolation- und Sicherheitsgrenzen des Kernels bleiben vollständig erhalten.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame System Foundation bereitstellen.
2. Die Foundation MUSS vom Kernel logisch getrennt sein.
3. Höhere Systemsoftware SOLL Kernel-internen Implementierungen nicht direkt entsprechen müssen.
4. Fundamentale Systemtypen MÜSSEN eindeutig definiert sein.
5. Ressourcen SOLLEN über stabile Identitäten und kontrollierte Handles angesprochen werden.
6. Die Foundation MUSS ein strukturiertes Fehlermodell bereitstellen.
7. Foundation-Schnittstellen MÜSSEN versionierbar sein.
8. Interne Implementierungen DÜRFEN ohne Änderung stabiler Verträge austauschbar sein.
9. Nicht unterstützte Funktionen MÜSSEN eindeutig erkennbar sein.
10. Die Foundation DARF keine Capability- oder Sicherheitsgrenzen umgehen.
11. Komplexe Anwendungs- und Fachlogik DARF nicht Bestandteil der Foundation sein.
12. Foundation-Zustand und verfügbare Funktionen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-HAL-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine kleine, stabile und versionierbare System Foundation zwischen Kernel und höherer Systemsoftware. Gemeinsame Typen, Fehler, Handles und fundamentale Systemabstraktionen erhalten dadurch ein einheitliches Modell, ohne höhere Komponenten an interne Kernelimplementierungen zu koppeln.