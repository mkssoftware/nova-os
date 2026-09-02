# ADR-CAPABILITY-0001 – Capabilities als primäre Systemfähigkeiten

## Status

Angenommen

## Kategorie

Capability Architecture / System Services / Application Model

## Kontext

Klassische Betriebssysteme organisieren Funktionen häufig entlang von Anwendungen, Bibliotheken, Prozessen oder fest definierten Systemdiensten.

NovaOS verfolgt stattdessen ein Fähigkeitenmodell.

Eine Funktion wie:

```text
Bild skalieren
PDF erzeugen
Text übersetzen
Audio dekodieren
Daten sortieren
Video transkodieren
Dokument drucken
```

soll nicht zwingend an eine bestimmte Anwendung gebunden sein.

Sie wird als eigenständige, beschreibbare und aufrufbare **Capability** betrachtet.

Dadurch können Anwendungen, Systemdienste, Pipelines und KI-Agenten dieselben Systemfähigkeiten verwenden und kombinieren.

---

## Entscheidung

NovaOS definiert **Capabilities als primäre Systemfähigkeiten**.

```text
Application
System Service
AI Agent
Pipeline
     ↓
Capability Requirement
     ↓
Capability System
     ↓
Suitable Implementation
     ↓
Execution
```

Eine Capability beschreibt dabei zunächst **was das System leisten kann**, nicht welche konkrete Implementierung diese Fähigkeit bereitstellt.

---

## Grundprinzip

```text
Capability = Was getan werden kann

Implementation = Wie es getan wird
```

Capabilities bilden damit eine stabile semantische Ebene oberhalb konkreter Bibliotheken, Algorithmen, Prozesse oder Hardwareimplementierungen.

---

## Capability Identity

Jede Capability besitzt eine stabile, globale Capability-ID.

Das verbindliche Format lautet:

```text
domain.authority.namespace.name
```

Beispiel:

```text
de.nova.image.filter.gaussian
```

mit:

```text
Domain:     de
Authority:  nova
Namespace:  image.filter
Name:       gaussian
```

Alle Segmente zwischen Authority und Name bilden gemeinsam den Namespace.

Der physische Speicherort oder Kategorieordner bestimmt nicht die Identität einer Capability.

---

## Capability und Implementierung

Eine Capability kann durch mehrere Implementierungen bereitgestellt werden.

```text
Capability
    │
    ├── CPU Implementation
    ├── SIMD Implementation
    ├── GPU Implementation
    ├── NPU Implementation
    ├── Hardware Provider
    └── Remote Provider
```

Die Capability bleibt dabei semantisch dieselbe.

NovaOS kann anhand des Execution Contracts, der Hardware, der Daten und der aktuellen Systembedingungen eine geeignete Implementierung auswählen.

---

## Abstrakte und konkrete Fähigkeiten

Capabilities können unterschiedliche Abstraktionsebenen besitzen.

Beispielsweise kann eine Anwendung eine allgemeine Fähigkeit anfordern:

```text
Sort(data)
```

NovaOS wählt daraufhin einen geeigneten Algorithmus.

Eine explizitere Capability oder ein Execution Contract kann dagegen eine bestimmte Implementierung verlangen:

```text
Sort(data, algorithm = TimSort)
```

Automatische Auswahl ist der Standard.

Explizite Auswahl bleibt möglich, sofern keine harten Systemanforderungen verletzt werden.

---

## Capability Contract

Eine Capability besitzt einen maschinenlesbaren Vertrag.

Dieser beschreibt mindestens die für ihre Verwendung relevanten Eigenschaften, beispielsweise:

```text
Capability Identity
Input Types
Output Types
Semantic Types
Supported Operations
Constraints
Provider Requirements
Execution Properties
```

Konkrete Vertragsformate werden durch NPSPECs definiert.

---

## Integration mit Nova.ExecutionContract

Capability und Execution Contract erfüllen unterschiedliche Aufgaben.

```text
Capability
    ↓
What should be done?

ExecutionContract
    ↓
Under which constraints?

Provider / Algorithm
    ↓
How should it be done?
```

Der Execution Contract kann unter anderem Anforderungen an:

- Determinismus,
- Deadline,
- Latenz,
- Ressourcenbudget,
- Data Sovereignty,
- Trust,
- Locality,
- bevorzugte oder erzwungene Algorithmen

enthalten.

---

## Capability Composition

Capabilities müssen miteinander kombinierbar sein.

```text
Decode Image
     ↓
Resize Image
     ↓
Apply Filter
     ↓
Encode Image
```

Solche Kombinationen können über das einheitliche Data/Object Pipelining ausgeführt werden.

Damit werden komplexe Anwendungen zunehmend zu Kompositionen wiederverwendbarer Fähigkeiten.

---

## Capability Discovery

NovaOS stellt eine systemweite Möglichkeit bereit, verfügbare Capabilities zu ermitteln.

Discovery kann beispielsweise berücksichtigen:

```text
Capability Identity
Semantic Function
Input / Output Types
Provider
Availability
Trust
Execution Properties
```

Discovery bedeutet jedoch nicht automatisch Autorisierung zur Verwendung.

---

## Capability und Security Capability

Der Begriff der funktionalen Capability ist von der sicherheitstechnischen Autorisierung zu unterscheiden.

```text
Functional Capability
→ Welche Fähigkeit existiert?

Security Capability
→ Welche Authority besitzt der Aufrufer?
```

Eine vorhandene Systemfähigkeit darf nur ausgeführt werden, wenn der Aufrufer auch die dafür erforderliche Authority besitzt.

Beide Konzepte können miteinander verknüpft werden, bleiben semantisch jedoch unterscheidbar.

---

## Capability Provider

Implementierungen werden durch Capability Provider bereitgestellt.

Provider können unter anderem sein:

```text
NovaOS System Component
Application
System Library
Driver
Hardware Accelerator
Compatibility Component
Remote Service
```

Provider müssen ihre unterstützten Capabilities und relevanten Eigenschaften deklarieren.

---

## Hardwareunabhängigkeit

Capabilities dürfen nicht unnötig an bestimmte Hardware gebunden werden.

Beispiel:

```text
Image.Resize
     ↓
Capability Resolution
     ↓
CPU / GPU / NPU / Remote
```

Damit kann dieselbe Anwendung auf unterschiedlichen Geräten unterschiedliche Ausführungspfade verwenden.

---

## Introspection

Das Capability-System ist Bestandteil der Architecture Introspection.

NovaOS muss nachvollziehbar machen können:

```text
Capability
Provider
Implementation
Execution Contract
Selection Reason
Resource Usage
Dependencies
```

Dadurch können Entwickler und Systemwerkzeuge erkennen, welche Fähigkeiten vorhanden sind und wie sie tatsächlich ausgeführt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Capabilities als primäre Abstraktion für wiederverwendbare Systemfähigkeiten unterstützen.
2. Eine Capability MUSS semantisch von ihrer konkreten Implementierung getrennt sein.
3. Capability-IDs MÜSSEN dem Schema `domain.authority.namespace.name` folgen.
4. Der physische Speicherort DARF NICHT die Identität einer Capability bestimmen.
5. Eine Capability MUSS durch mehrere Provider oder Implementierungen bereitgestellt werden können.
6. Capability-Verträge MÜSSEN Ein- und Ausgaben sowie relevante semantische Eigenschaften beschreiben können.
7. Capabilities MÜSSEN mit Semantic Types integrierbar sein.
8. Capabilities MÜSSEN mit `Nova.ExecutionContract` kombinierbar sein.
9. NovaOS SOLL standardmäßig eine geeignete Implementierung automatisch auswählen können.
10. Eine explizite Implementierungs- oder Algorithmuswahl MUSS möglich sein, sofern Hard Constraints dies erlauben.
11. Capabilities MÜSSEN über Data/Object Pipelines komponierbar sein können.
12. Capability Discovery MUSS unterstützt werden.
13. Discovery DARF NICHT automatisch Authority zur Nutzung einer Capability gewähren.
14. Funktionale Capabilities und Security Authority MÜSSEN semantisch unterscheidbar bleiben.
15. Capability Provider MÜSSEN ihre bereitgestellten Fähigkeiten deklarieren.
16. Capability-Auflösung und Provider-Auswahl MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Funktionen werden systemweit wiederverwendbar,
- Anwendungen müssen weniger Funktionalität selbst implementieren,
- CPU-, GPU-, NPU- und zukünftige Provider können transparent konkurrieren,
- Fähigkeiten können zu komplexen Pipelines kombiniert werden,
- automatische Algorithmusauswahl wird architektonisch möglich,
- Anwendungen werden stärker von konkreten Implementierungen entkoppelt.

### Negative Konsequenzen

- Capability-Verträge müssen langfristig stabil und versionierbar sein,
- Provider-Auswahl benötigt eine zentrale Architektur,
- funktionale Capabilities und Security Capabilities müssen terminologisch klar getrennt werden.

---

## Verworfene Alternativen

### Fähigkeiten ausschließlich als Bibliotheksfunktionen

Verworfen.

Bibliotheken koppeln Anwendungen stärker an konkrete Implementierungen und Deployment-Strukturen.

### Jede Anwendung implementiert ihre Fähigkeiten selbst

Verworfen.

Dies führt zu Duplikation und verhindert systemweite Optimierung.

### Capability-ID aus dem Dateipfad ableiten

Verworfen.

Physische Organisation und semantische Identität müssen unabhängig voneinander evolvieren können.

### Immer eine feste Implementierung pro Capability

Verworfen.

NovaOS soll abhängig von Hardware, Daten und Execution Contract unterschiedliche Implementierungen auswählen können.

---

## Abhängigkeiten

- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0017_Composability_and_Orthogonality`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-MODEL-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-CONTRACT-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-RESOLUTION-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-TEST-0001`

---

## Ergebnis

Capabilities werden zu einer grundlegenden funktionalen Abstraktion von NovaOS:

```text
What?
 ↓
Capability

Under which constraints?
 ↓
Execution Contract

How?
 ↓
Provider / Implementation / Algorithm

Where?
 ↓
CPU / GPU / NPU / Device / Remote
```

Damit beschreibt Software primär die benötigte Fähigkeit und ihre Anforderungen, während NovaOS die konkrete Ausführung entsprechend der verfügbaren Provider, Hardware und Systembedingungen planen kann.