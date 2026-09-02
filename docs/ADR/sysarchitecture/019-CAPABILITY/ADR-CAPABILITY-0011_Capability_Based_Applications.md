# ADR-CAPABILITY-0011 – Capability-Based Applications

## Status

Angenommen

## Kategorie

Capability Architecture / Application Model / Composition

## Kontext

Klassische Anwendungen werden häufig als weitgehend monolithische Softwarepakete betrachtet, die einen festen Satz eigener Funktionen mitbringen.

NovaOS verfolgt dagegen ein Fähigkeitenmodell, in dem viele Funktionen als systemweit verfügbare Capabilities bereitgestellt werden.

Eine Bildbearbeitung muss beispielsweise nicht sämtliche Decoder, Filter, Skalierungsalgorithmen, Exporter und KI-Funktionen selbst implementieren.

Stattdessen kann eine Anwendung diese Fähigkeiten deklarieren, entdecken und zu einer Benutzerfunktion zusammensetzen.

---

## Entscheidung

NovaOS definiert **Capability-Based Applications** als natives Anwendungsmodell.

Eine Anwendung besteht konzeptionell aus:

```text
Application
 ├── Application Logic
 ├── UI
 ├── State
 ├── Capability Requirements
 └── Capability Compositions
```

Die Anwendung beschreibt ihre eigene Logik und Benutzerinteraktion, während wiederverwendbare Funktionen bevorzugt über Capabilities eingebunden werden.

---

## Grundprinzip

```text
Applications orchestrate capabilities
instead of reimplementing every function.
```

Capabilities ersetzen dabei nicht die Anwendung selbst.

Die Anwendung bleibt verantwortlich für ihre fachliche Logik, ihren Zustand und ihre Benutzerinteraktion.

---

## Capability Requirements

Eine Anwendung kann deklarieren, welche Fähigkeiten sie benötigt.

Beispiel:

```text
Application
 ├── image.decode
 ├── image.resize
 ├── image.filter
 ├── image.encode
 └── storage.save
```

Diese Anforderungen werden gegen die Capability Registry und verfügbare Provider aufgelöst.

Die Anwendung muss dadurch nicht zwingend wissen, welche konkrete Implementierung die jeweilige Fähigkeit bereitstellt.

---

## Anwendung und Provider

Die Beziehung lautet:

```text
Application
     ↓
Capability Requirement
     ↓
Discovery / Negotiation / Resolution
     ↓
Capability Provider
```

Provider können beispielsweise aus:

```text
System
Application Package
Driver
Hardware
Compatibility Layer
Remote Service
```

stammen.

Die funktionale Anwendung bleibt von der konkreten Provider-Implementierung entkoppelt.

---

## Capability Composition

Anwendungen können Capabilities zu komplexeren Funktionen zusammensetzen.

```text
Open Image
    ↓
Decode
    ↓
Enhance
    ↓
Resize
    ↓
Encode
    ↓
Save
```

Die Anwendung beschreibt dabei die gewünschte Composition.

NovaOS kann anschließend Provider, Datenpfade und Ausführungsorte optimieren.

---

## Adaptive Applications

Capability-Based Applications ermöglichen adaptive Funktionsangebote.

Eine Anwendung kann abhängig von verfügbaren Capabilities zusätzliche Funktionen anbieten.

```text
Base Application
      ↓
Capability Discovery
      ↓
Available Capabilities
      ↓
Available Application Functions
```

Fehlt beispielsweise eine optionale AI-Capability, kann die Anwendung weiterhin mit ihren übrigen Fähigkeiten funktionieren.

Verbindlich benötigte Capabilities bleiben davon getrennt.

---

## UI-Integration

Capability-Based Applications bilden die Grundlage für die adaptive NovaOS-Oberfläche.

Capability-Metadaten können zur Bereitstellung von Commands und anderen UI-Funktionen verwendet werden.

```text
Available Capabilities
        ↓
Application Commands
        ↓
Adaptive UI
```

Die konkrete Ribbon-, Command- und UI-Integration wird separat spezifiziert.

Die UI darf nicht die semantische Identität einer Capability bestimmen.

---

## NovaLang

Das Modell passt zur Trennung innerhalb von NovaLang:

```text
.nlf
→ Application Logic

.nui
→ Declarative UI
```

Beide können auf dieselben Capability Contracts und Semantic Types zugreifen.

Eine Capability ist dabei weder an `.nlf` noch an `.nui` gebunden.

---

## Required und Optional Capabilities

Anwendungen müssen zwischen erforderlichen und optionalen Capabilities unterscheiden können.

```text
Required Capability
→ Anwendung/Funktion benötigt sie zwingend

Optional Capability
→ erweitert die Anwendung
```

Fehlt eine optionale Capability, soll die Anwendung Graceful Degradation verwenden können.

Fehlt eine zwingend erforderliche Capability, muss dies explizit behandelt werden.

---

## Security

Eine Anwendung erhält durch die Deklaration einer Capability Requirement keine automatische Authority.

```text
Capability Required
      ≠
Capability Authorized
```

Capability Discovery, funktionale Verfügbarkeit und Security Authority bleiben getrennt.

Benötigte Authority muss über das Capability-Based Security Model bereitgestellt werden.

---

## Application Sandbox

Capability-Based Applications laufen innerhalb ihrer vorgesehenen Execution Domain und Sandbox.

Capability Provider können:

```text
In-Process
Sandboxed Local
System Service
Driver Domain
Remote
```

ausgeführt werden.

Die Provider-Auswahl darf die Sicherheitsgrenzen der Anwendung nicht implizit aufheben.

---

## Private Capability Provider

Eine Anwendung darf eigene Provider mitbringen.

Diese können ausschließlich für die Anwendung oder kontrolliert auch systemweit verfügbar sein.

```text
Application
 └── Private Provider
       ↓
   Capability
```

Private Provider verändern nicht die globale semantische Capability Identity.

Ihre Sichtbarkeit und Priorität werden durch Registry- und Resolution-Policies bestimmt.

---

## Application SYS Overlay

Private Systemabhängigkeiten einer Anwendung können über den per-Application `SYS`-Namespace bereitgestellt werden.

Dies bleibt von der Capability Identity getrennt.

```text
Application SYS Overlay
→ physische/private Abhängigkeiten

Capability Model
→ semantische Fähigkeiten
```

Eine Anwendung muss dadurch das globale `/System` nicht verändern, um private Capability Provider oder deren Abhängigkeiten bereitzustellen.

---

## Portabilität

Da Anwendungen primär Capability Requirements statt konkrete Hardwareimplementierungen adressieren, kann dieselbe Anwendung unterschiedliche Plattformen nutzen.

```text
Application
     ↓
image.resize
     ↓
 ┌────┼────┐
CPU  GPU  NPU
```

Hardwareunterschiede werden soweit möglich durch Provider Resolution und HAL abstrahiert.

---

## Introspection

NovaOS muss für eine Anwendung nachvollziehbar machen können:

```text
Required Capabilities
Optional Capabilities
Resolved Providers
Missing Capabilities
Capability Compositions
Effective Authority
Execution Location
```

Damit kann sowohl der Benutzer als auch die Entwicklungsumgebung verstehen, aus welchen Fähigkeiten eine Anwendung aufgebaut ist.

---

## Normative Anforderungen

1. NovaOS MUSS Capability-Based Applications als natives Anwendungsmodell unterstützen.
2. Anwendungen MÜSSEN Capability Requirements deklarieren können.
3. Erforderliche und optionale Capabilities MÜSSEN unterscheidbar sein.
4. Anwendungen SOLLEN wiederverwendbare Systemfunktionen bevorzugt über Capabilities verwenden können.
5. Anwendungen MÜSSEN Capabilities komponieren können.
6. Anwendungen DÜRFEN NICHT von einer einzelnen konkreten Provider-Implementierung abhängig sein müssen, sofern der Capability Contract dies nicht verlangt.
7. Optionale Capabilities MÜSSEN Graceful Degradation ermöglichen können.
8. Capability Requirements DÜRFEN NICHT automatisch Security Authority verleihen.
9. Capability Provider DÜRFEN die Sandbox-Grenzen einer Anwendung NICHT implizit aufheben.
10. Anwendungen MÜSSEN private Capability Provider bereitstellen können.
11. Private Provider MÜSSEN in die allgemeine Registry- und Resolution-Architektur integrierbar sein.
12. Das per-Application `SYS` Overlay MUSS für private Provider-Abhängigkeiten nutzbar sein können.
13. Capability Identity MUSS unabhängig von UI, Anwendungspaket und physischem Installationsort bleiben.
14. NovaLang `.nlf` und `.nui` MÜSSEN auf dasselbe Capability-Modell zugreifen können.
15. Capability-Auflösung einer Anwendung MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger Funktionsduplikation zwischen Anwendungen,
- Anwendungen können vorhandene Systemfähigkeiten wiederverwenden,
- Hardwarebeschleunigung kann ohne Anwendungsänderung genutzt werden,
- optionale Fähigkeiten ermöglichen adaptive Anwendungen,
- Capability Composition wird zum Fundament komplexer Software,
- Anwendungen werden stärker von konkreten Implementierungen entkoppelt.

### Negative Konsequenzen

- Anwendungen benötigen explizite Capability Requirements,
- fehlende oder inkompatible Capabilities müssen sauber behandelt werden,
- Provider Resolution wird Teil des Application Lifecycles.

---

## Verworfene Alternativen

### Anwendungen als vollständig monolithische Funktionspakete

Verworfen.

Dies würde wiederverwendbare Systemfähigkeiten duplizieren und systemweite Optimierung verhindern.

### Jede Anwendung bringt grundsätzlich alle Provider selbst mit

Verworfen.

Systemweite Capabilities und Hardwareprovider sollen gemeinsam genutzt werden können.

### Anwendungen erhalten automatisch Authority für deklarierte Capabilities

Verworfen.

Funktionale Anforderungen und Security Authority müssen getrennt bleiben.

### Anwendungen bestehen ausschließlich aus Capabilities

Verworfen.

Anwendungsspezifische Logik, Zustand und Benutzerinteraktion bleiben eigenständige Bestandteile.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-CAPABILITY-0004_Capability_Discovery`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-CAPABILITY-0006_Capability_Negotiation`
- `ADR-CAPABILITY-0010_Capability_Sandboxing`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-STORAGE-0018_Per_Application_System_Overlay`
- `ADR-STORAGE-0019_Private_Systemabhängigkeiten_im_Application_SYS_Namespace`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-APPLICATION-0001`
- `NPSPEC-CAPABILITY-APPLICATION-REQUIREMENTS-0001`
- `NPSPEC-CAPABILITY-APPLICATION-PROVIDER-0001`
- `NPSPEC-CAPABILITY-APPLICATION-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-APPLICATION-ADAPTATION-0001`
- `NPSPEC-CAPABILITY-APPLICATION-UI-0001`
- `NPSPEC-CAPABILITY-APPLICATION-TEST-0001`

---

## Ergebnis

NovaOS-Anwendungen werden nicht primär als abgeschlossene Sammlungen fest eingebauter Funktionen betrachtet, sondern als Orchestratoren wiederverwendbarer Systemfähigkeiten:

```text
Application Logic + UI + State
             ↓
    Capability Requirements
             ↓
 Discovery / Negotiation / Resolution
             ↓
      Capability Providers
             ↓
   Composed Application Function
```

Damit können Anwendungen klein, modular, adaptiv und hardwareunabhängiger aufgebaut werden, während NovaOS gemeinsame Fähigkeiten systemweit bereitstellt und optimiert.