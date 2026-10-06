# NPSPEC-APP-COMPONENT-0001 – Nova App Component

## Status

Angenommen

## Kategorie

App / Component

## Zweck

NovaOS definiert App Components als klar abgegrenzte funktionale Bestandteile einer App.

Components ermöglichen die interne Zerlegung einer App in unabhängige Einheiten für UI, Logik, Hintergrundaufgaben, Datenverarbeitung oder Systemintegration, ohne daraus automatisch eigenständige Apps zu machen.

## Grundprinzipien

```text
Component ≠ App
Component ≠ Process
Component ≠ Capability
Component ≠ Service
Component ≠ Package
Component Boundary ≠ Security Boundary
```

## Modell

```text
App
├── Component
├── Component
└── Component
```

Ein Component besitzt mindestens:

```text
AppComponent
├── ComponentID
├── Type
├── EntryPoint
├── Interfaces[]
├── Dependencies[]
├── Requirements[]
└── Lifecycle
```

`ComponentID` ist innerhalb der zugehörigen `AppID` eindeutig.

Die vollständige Identität ergibt sich aus:

```text
AppID + ComponentID
```

## Component-Typen

NovaOS kann unter anderem folgende Component-Typen unterstützen:

```text
UI
Logic
Background
Data
Integration
Extension
Worker
Provider
```

Weitere Typen dürfen registriert werden.

Der Typ beschreibt die Rolle des Components und erzeugt keine zusätzliche Authority.

## Zusammensetzung

Components können über explizite Interfaces miteinander verbunden werden:

```text
Component A
    ↓
Interface
    ↓
Component B
```

Abhängigkeiten müssen deklarierbar sein und dürfen nicht ausschließlich aus impliziten globalen Zuständen entstehen.

## Ausführung

Ein Component muss nicht einem eigenen Prozess entsprechen.

Je nach Execution Contract kann NovaOS Components:

```text
In-Process
Out-of-Process
Isolated
On-Demand
Background
Remote
```

ausführen.

Die Platzierung darf geändert werden, solange Vertrag, Sicherheit und beobachtbares Verhalten erhalten bleiben.

## Lifecycle

Components besitzen einen kontrollierbaren Lifecycle:

```text
Discover
 ↓
Resolve
 ↓
Initialize
 ↓
Activate
 ↓
Running
 ↓
Deactivate
 ↓
Release
```

Nicht benötigte Components dürfen verzögert geladen oder wieder freigegeben werden.

## Capabilities

Ein Component erhält keine Authority allein durch seine Zugehörigkeit zu einer App.

```text
App Authority
      ∩
Component Requirements
      ∩
Policy
      =
Effective Component Authority
```

Authority darf gegenüber dem App-Kontext weiter eingeschränkt werden.

Eine Erweiterung über die Authority der App hinaus erfordert einen ausdrücklich autorisierten Mechanismus.

## Fehlerisolation

Fehler eines Components sollen nach Möglichkeit lokal begrenzt werden.

NovaOS darf Components abhängig von Kritikalität und Isolation getrennt neu starten, deaktivieren oder ersetzen, ohne die gesamte App zu beenden.

## Introspection

NovaOS muss für aktive Components mindestens folgende Informationen ermitteln können:

```text
AppID
ComponentID
Type
State
Dependencies
Execution Location
Effective Authority
Resource Usage
```

## Normative Anforderungen

1. Components MÜSSEN eindeutig ihrer App zugeordnet sein.
2. `ComponentID` MUSS innerhalb einer `AppID` eindeutig sein.
3. Component und App MÜSSEN getrennte Konzepte bleiben.
4. Ein Component DARF nicht automatisch einem Prozess entsprechen.
5. Abhängigkeiten zwischen Components MÜSSEN explizit beschreibbar sein.
6. Component-Kommunikation SOLL über definierte Interfaces erfolgen.
7. Components DÜRFEN keine implizite zusätzliche Authority erhalten.
8. Component Authority DARF gegenüber der App weiter eingeschränkt werden.
9. Components SOLLEN bedarfsgesteuert aktivierbar und deaktivierbar sein.
10. NovaOS DARF Components zur Isolation in getrennten Ausführungskontexten platzieren.
11. Component-Fehler SOLLEN soweit möglich lokal isoliert werden.
12. Zustand, Ausführung und effektive Authority eines Components MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Component-Modell zur modularen Strukturierung von Apps. Components bleiben Teil ihrer App, können jedoch unabhängig aufgelöst, aktiviert, isoliert und ressourcengesteuert werden, ohne Prozessgrenzen oder zusätzliche Berechtigungen vorauszusetzen.