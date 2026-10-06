# NPSPEC-APP-CAPABILITYCOMPOSED-0001 – Nova Capability-Composed App

## Status

Angenommen

## Kategorie

App / Capability Composition

## Zweck

NovaOS definiert Capability-Composed Apps als Apps, deren Funktionalität überwiegend durch die Komposition vorhandener Capabilities entsteht.

Die App beschreibt dabei Aufgabe, Datenfluss, UI und benötigte Fähigkeiten, anstatt alle Funktionen selbst zu implementieren.

## Grundprinzipien

```text
App Function ≠ App-Owned Implementation
Capability Composition ≠ Capability Authority
Capability Discovery ≠ Permission
CapabilityID ≠ Provider
Composition ≠ Monolithic Program
UI ≠ Execution Logic
```

## Modell

```text
CapabilityComposedApp
├── AppID
├── UI
├── Composition
├── CapabilityRequirements[]
├── State
└── ExecutionContract
```

Die konkrete Funktion entsteht durch verbundene Capabilities:

```text
Input
  ↓
Capability A
  ↓
Capability B
  ↓
Custom Logic
  ↓
Capability C
  ↓
Output
```

## Komposition

Eine Komposition beschreibt:

```text
Nodes
Connections
Inputs
Outputs
Parameters
Control Flow
Data Flow
Error Paths
```

Capability-Nodes werden über stabile `CapabilityID`s referenziert.

Der konkrete Provider oder die Implementierung wird separat aufgelöst.

## Provider-Auflösung

```text
CapabilityID
     ↓
Discovery
     ↓
Compatible Providers
     ↓
Policy / Trust / Resources
     ↓
Selected Implementation
```

Die App darf dadurch dieselbe Capability verwenden, auch wenn deren konkrete Implementierung ersetzt, aktualisiert oder an einem anderen Ort ausgeführt wird.

## Authority

Das Vorhandensein eines Capability-Nodes erzeugt keine Berechtigung.

```text
Requested Capability
        ∩
Granted Authority
        ∩
Policy
        ∩
Execution Contract
        =
Effective Authority
```

Jeder Capability-Aufruf muss innerhalb der tatsächlich verfügbaren Authority erfolgen.

## Custom Logic

Eigene Logik darf zwischen Capability-Nodes ausgeführt werden.

Custom Logic erhält dadurch jedoch keinen impliziten Systemzugriff.

```text
Network Capability
       ↓
Custom Logic
       ↓
Storage Capability
```

Die Custom Logic verarbeitet die explizit bereitgestellten Daten und Handles.

Systemzugriffe erfolgen ausschließlich über entsprechend autorisierte Capabilities oder andere ausdrücklich autorisierte NovaOS-Schnittstellen.

## Datenfluss

Daten sollen direkt zwischen kompatiblen Capability-Nodes übertragen werden können.

```text
Capability Output
       ↓
Semantic Type
       ↓
Capability Input
```

Bei kompatiblen Ausführungskontexten darf NovaOS Zero-Copy oder Shared Buffers verwenden.

## Ausführung

NovaOS darf einzelne Capability-Nodes abhängig vom Execution Contract unterschiedlich platzieren:

```text
Local
Isolated
Accelerated
Remote
Provider Process
System Service
```

Identität und logische Komposition bleiben davon unabhängig.

## Fehlerbehandlung

Fällt eine Capability oder Implementierung aus, darf NovaOS:

```text
Retry
Fallback Provider
Degrade
Recompose
Fail
```

verwenden, sofern Semantik, Sicherheit und Execution Contract dies erlauben.

## Beziehung zu Solutions

Solutions sind ein primäres Modell für Capability-Komposition und können Capability-Composed Apps darstellen.

Das App-Modell bleibt jedoch allgemeiner und bindet das Konzept nicht ausschließlich an das Solution-Format.

## Normative Anforderungen

1. Capability-Composed Apps MÜSSEN Capabilities über stabile `CapabilityID`s referenzieren.
2. CapabilityID und konkrete Implementierung MÜSSEN getrennt bleiben.
3. Capability Discovery DARF keine Authority erzeugen.
4. Capability-Komposition MUSS explizite Daten- und Kontrollflüsse beschreiben können.
5. Custom Logic DARF keine implizite System-Authority erhalten.
6. Systemzugriffe MÜSSEN über autorisierte Capabilities oder ausdrücklich autorisierte Schnittstellen erfolgen.
7. Capability-Ausgaben SOLLEN über semantische Typen verbunden werden.
8. Provider MÜSSEN austauschbar sein können, sofern Vertrag und Semantik kompatibel bleiben.
9. NovaOS DARF Capability-Nodes unterschiedlich platzieren und isolieren.
10. Alternative Provider DÜRFEN bei Fehlern verwendet werden, sofern der Execution Contract dies erlaubt.
11. Komposition und effektive Authority MÜSSEN getrennt bleiben.
12. Capability-Nodes, Provider, Datenfluss, Zustand und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-SOLUTION-PACKAGE-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Apps aus vorhandenen Capabilities zusammensetzen, ohne deren Funktionalität monolithisch neu implementieren zu müssen. Identität, Provider, Ausführungsort und Authority bleiben getrennt, wodurch Apps modular, austauschbar, sicher und dynamisch ausführbar werden.