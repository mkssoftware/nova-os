# ADR-ARCH-0015 – Graceful Degradation als Systemprinzip

## Status

Angenommen

## Kategorie

Systemarchitektur / Resilienz / Ressourcenmanagement / Execution / Recovery / Quality of Service

## Kontext

NovaOS ist als adaptives, capability-basiertes und langfristig evolvierbares Betriebssystem konzipiert.

Das System soll auf sehr unterschiedlichen Hardwareklassen und unter stark variierenden Laufzeitbedingungen funktionieren.

Dazu gehören beispielsweise:

- leistungsfähige Workstations,
- Notebooks,
- Tablets,
- Smartphones,
- Embedded-Systeme,
- Edge-Geräte,
- Server,
- virtuelle Maschinen,
- Systeme mit GPU,
- Systeme ohne GPU,
- Systeme mit NPU,
- Systeme ohne NPU,
- Systeme mit eingeschränktem Arbeitsspeicher,
- Systeme mit geringer Energieverfügbarkeit,
- Systeme unter hoher thermischer Last,
- Systeme mit eingeschränkter Netzwerkverbindung,
- vollständig offline arbeitende Systeme,
- teilweise ausgefallene Systeme,
- Systeme während Updates,
- Systeme während Hot Replacement,
- Systeme während Recovery.

Ein traditionelles System besitzt häufig nur wenige Reaktionen auf Ressourcenknappheit oder Komponentenfehler:

```text
Normal Operation
      ↓
Resource Problem
      ↓
Failure
```

oder:

```text
Preferred Provider unavailable
      ↓
Operation fails
```

Für NovaOS ist dieses Verhalten nicht ausreichend.

Viele Operationen besitzen nicht nur zwei mögliche Zustände:

```text
Perfect
Failed
```

sondern mehrere technisch und semantisch akzeptable Qualitätsstufen.

Beispiel:

```text
Ray Tracing
    ↓
High Quality Rasterization
    ↓
Reduced Effects
    ↓
Basic Rendering
```

oder:

```text
4K Video
    ↓
1440p
    ↓
1080p
    ↓
720p
```

oder:

```text
Remote AI Model
    ↓
Local Large Model
    ↓
Local Small Model
    ↓
Rule-Based Fallback
```

oder:

```text
GPU Image Processing
    ↓
CPU SIMD Processing
    ↓
CPU Scalar Processing
```

Solange die reduzierte Variante die erforderliche Semantik, Sicherheit und Mindestqualität erfüllt, ist eine kontrollierte Degradation häufig besser als ein vollständiger Ausfall.

NovaOS benötigt deshalb **Graceful Degradation als systemweites Architekturprinzip**.

---

# Entscheidung

NovaOS MUSS bei Ressourcenknappheit, Provider-Ausfall, Hardwareeinschränkungen, thermischen Grenzen, Energieknappheit, Netzwerkproblemen oder anderen Laufzeitproblemen nach Möglichkeit kontrolliert auf eine zulässige niedrigere Betriebs- oder Qualitätsstufe wechseln, anstatt unnötig vollständig auszufallen.

Das grundlegende Modell lautet:

```text
Preferred State
      ↓
Constraint Violation
      ↓
Evaluate Degradation Policy
      ↓
Select Next Valid State
      ↓
Reconfigure
      ↓
Verify
      ↓
Continue Operation
```

Graceful Degradation ist dabei **kein Freibrief zur Verletzung von Hard Constraints**.

Die zentrale Regel lautet:

```text
Degrade quality before violating correctness.
```

und erweitert:

```text
Degrade optional properties
before failing required functionality.

Never degrade below
hard safety, security,
correctness or sovereignty requirements.
```

---

# Grundprinzip

NovaOS unterscheidet zwischen:

```text
Required Properties
```

und:

```text
Degradable Properties
```

Beispiel:

```text
Required:
    Correct output type
    Security isolation
    Data sovereignty
    Minimum accuracy

Degradable:
    Resolution
    Frame rate
    Visual effects
    Precision above minimum
    Optional AI enhancement
```

Nur degradierbare Eigenschaften dürfen automatisch reduziert werden.

---

# Kein stiller Qualitätsverlust

Graceful Degradation bedeutet nicht:

```text
Do whatever still works.
```

NovaOS MUSS wissen:

```text
What may degrade?

How far may it degrade?

Under which conditions?

What must never degrade?

How can the original state be restored?
```

---

# Degradation Contract

Graceful Degradation wird über explizite Contracts beschrieben.

Diese ADR baut insbesondere auf:

```text
ADR-ARCH-0014
Explizite Contracts zwischen Systemkomponenten
```

auf.

Ein Contract kann beispielsweise definieren:

```text
Quality {
    preferred = Ultra
    acceptable = [
        High,
        Medium
    ]
    minimum = Medium
}
```

NovaOS darf automatisch wechseln:

```text
Ultra
  ↓
High
  ↓
Medium
```

aber nicht:

```text
Medium
  ↓
Low
```

wenn:

```text
minimum = Medium
```

gilt.

---

# Hard Minimum

Jede degradierbare Eigenschaft kann ein Hard Minimum besitzen.

Beispiel:

```text
FrameRate {
    preferred = 120 fps
    minimum   = 30 fps
}
```

---

# Hard Maximum

Bestimmte Eigenschaften können zusätzlich ein Hard Maximum besitzen.

Beispiel:

```text
Latency {
    preferred <= 5 ms
    maximum   <= 16 ms
}
```

---

# Preferred Value

Preferred beschreibt den gewünschten Normalzustand.

Beispiel:

```text
Resolution {
    preferred = 3840x2160
}
```

---

# Acceptable Range

Ein Contract kann einen Bereich zulässiger Werte definieren.

Beispiel:

```text
Resolution {
    preferred = 3840x2160
    minimum   = 1280x720
}
```

---

# Forbidden State

Bestimmte Degradationsstufen können explizit verboten sein.

Beispiel:

```text
CloudFallback = Forbidden
```

---

# Degradation Ladder

NovaOS verwendet das Konzept einer **Degradation Ladder**.

Eine Degradation Ladder beschreibt geordnete Betriebsstufen.

Beispiel:

```text
Level 0:
    Full Quality

Level 1:
    Reduced Optional Effects

Level 2:
    Reduced Resolution

Level 3:
    Reduced Frame Rate

Level 4:
    Minimum Functional Mode
```

---

# Degradation Level

Jede Stufe soll eine stabile Identität besitzen können.

Beispiel:

```text
QualityLevel.Full
QualityLevel.High
QualityLevel.Medium
QualityLevel.Minimum
```

---

# Keine universelle numerische Bedeutung

Ein niedriger oder höherer numerischer Level besitzt nicht automatisch dieselbe Bedeutung für alle Subsysteme.

Die Semantik wird durch den jeweiligen Contract definiert.

---

# Degradation Profile

Komponenten können vordefinierte Degradation Profiles anbieten.

Beispiel:

```text
Profile.Performance
Profile.Balanced
Profile.EnergySaving
Profile.ThermalEmergency
Profile.MinimumFunctional
```

---

# Degradation Dimension

Qualität ist mehrdimensional.

NovaOS darf Degradation deshalb nicht ausschließlich als einzelnen Quality-Wert modellieren.

Mögliche Dimensionen sind:

```text
Resolution
FrameRate
Precision
Latency
Accuracy
VisualQuality
AudioQuality
FeatureSet
Parallelism
CacheSize
ModelSize
DataFreshness
Redundancy
Availability
EnergyConsumption
MemoryConsumption
BandwidthConsumption
```

---

# Multi-Dimensional Degradation

Beispiel:

```text
Preferred {
    resolution = 4K
    frameRate  = 120 fps
    effects    = Ultra
}
```

Bei Ressourcenknappheit könnte NovaOS wählen:

```text
resolution = 4K
frameRate  = 60 fps
effects    = High
```

anstatt sofort:

```text
resolution = 1080p
frameRate  = 30 fps
effects    = Low
```

---

# Degradation Planning

NovaOS soll Degradation als Optimierungsproblem innerhalb zulässiger Contracts behandeln.

Konzeptionell:

```text
Current State
     ↓
Constraint Pressure
     ↓
Generate Valid Alternatives
     ↓
Evaluate Cost
     ↓
Evaluate User Impact
     ↓
Evaluate Recovery Cost
     ↓
Select Best Valid Alternative
```

---

# Hard Constraints zuerst

Vor jeder Degradation werden Hard Constraints geprüft.

Die Prioritätsreihenfolge bleibt grundsätzlich:

```text
Safety
    ↓
Security
    ↓
Data Sovereignty
    ↓
Hard Realtime
    ↓
Determinism
    ↓
Correctness
    ↓
Explicit User Requirement
    ↓
Resource Budget
    ↓
Performance
    ↓
Energy Efficiency
    ↓
Adaptive Optimization
    ↓
Preference
```

---

# Safety darf nicht degradiert werden

Ein System darf beispielsweise nicht:

```text
Safety Check
    ↓
disabled
```

nur um Ressourcen zu sparen.

---

# Security darf nicht degradiert werden

Nicht erlaubt:

```text
Encrypted Connection
    ↓
Unencrypted Connection
```

nur weil Verschlüsselung zusätzliche CPU-Zeit benötigt.

---

# Data Sovereignty darf nicht degradiert werden

Nicht erlaubt:

```text
Local Processing
    ↓
Remote Cloud Processing
```

wenn:

```text
Sovereignty = LocalOnly
```

gilt.

---

# Correctness darf nicht degradiert werden

Graceful Degradation darf nicht aus:

```text
Correct Result
```

ein:

```text
Possibly Incorrect Result
```

machen, sofern Approximation nicht ausdrücklich erlaubt ist.

---

# Precision

Numerische Präzision kann degradierbar sein, wenn der Contract dies erlaubt.

Beispiel:

```text
Precision {
    preferred = Float64
    acceptable = Float32
    minimumAccuracy = 1e-5
}
```

---

# Approximation

Approximation MUSS explizit erlaubt sein.

Beispiel:

```text
Approximation {
    allowed = true
    maximumError = 0.001
}
```

---

# Semantic Quality

Qualitätsreduktion muss semantisch beschrieben werden können.

Beispiel:

```text
MedicalImageAnalysis {
    diagnosticAccuracy >= RequiredThreshold
}
```

Eine Degradation unter den medizinisch erforderlichen Mindestwert wäre nicht zulässig.

---

# Functional Degradation

Nicht nur Qualität, sondern auch optionale Funktionen können reduziert werden.

Beispiel:

```text
Full Mode:
    Core Function
    AI Suggestions
    Live Preview
    Background Analysis
    Cloud Synchronization

Reduced Mode:
    Core Function
    Live Preview

Minimum Mode:
    Core Function
```

---

# Core Functionality

Komponenten sollen definieren können:

```text
Core
Required
Optional
Enhancement
```

---

# Optional Feature Shedding

Unter Druck können optionale Features kontrolliert deaktiviert werden.

Beispiel:

```text
AI Suggestions
Background Indexing
Visual Effects
Preloading
Speculative Execution
Telemetry
Thumbnail Generation
```

---

# Load Shedding

Graceful Degradation umfasst systemweites Load Shedding.

Bei Überlast kann NovaOS:

- optionale Arbeit verzögern,
- Hintergrundarbeit pausieren,
- Preloading reduzieren,
- speculative Tasks abbrechen,
- Cache-Aufbau reduzieren,
- Quality Levels senken,
- Best-Effort-Arbeit verwerfen.

---

# Keine willkürliche Prozessbeendigung

Load Shedding soll möglichst semantisch erfolgen.

Bevor ein kompletter Prozess beendet wird, soll geprüft werden, ob degradierbare Arbeit reduziert werden kann.

---

# Resource Economy

Diese ADR integriert:

```text
ADR-ARCH-0005
Systemweite Ressourcenökonomie
```

Die Resource Economy liefert Pressure Signals.

Beispiele:

```text
CPU Pressure
Memory Pressure
GPU Pressure
NPU Pressure
Storage Pressure
Network Pressure
Energy Pressure
Thermal Pressure
```

---

# Resource Pressure

Resource Pressure ist nicht automatisch ein Fehler.

Es ist zunächst ein Signal für mögliche Anpassung.

```text
Normal
   ↓
Pressure
   ↓
Degradation
   ↓
Critical
   ↓
Failure / Emergency
```

---

# Pressure Levels

Ein mögliches Modell lautet:

```text
Normal
Elevated
High
Critical
Emergency
```

---

# Pressure Hysteresis

Degradation und Recovery dürfen nicht bei kleinen Schwankungen ständig hin- und herschalten.

NovaOS MUSS Hysterese unterstützen können.

Beispiel:

```text
Degrade at:
    GPU > 90 %

Recover at:
    GPU < 70 %
```

---

# Debouncing

Kurzzeitige Pressure Peaks sollen nicht automatisch große Systemänderungen verursachen.

---

# Stability Window

Ein Recovery kann verlangen:

```text
ResourceStableFor >= 5 s
```

bevor die Qualität wieder erhöht wird.

---

# Memory Pressure

Unter Memory Pressure kann NovaOS beispielsweise:

```text
Drop Reconstructable Cache
    ↓
Reduce Cache Targets
    ↓
Suspend Background Work
    ↓
Reduce Application Quality
    ↓
Compress Cold Memory
    ↓
Reclaim Memory
    ↓
Last-Resort Termination
```

---

# Memory Quality Contracts

Anwendungen können beschreiben, welche Daten:

```text
Critical
Reconstructable
Cache
Optional
Discardable
```

sind.

---

# Cache Degradation

Caches sind typischerweise bevorzugte Degradationsziele, sofern ihr Verlust keine Korrektheit verletzt.

---

# GPU Pressure

Unter GPU Pressure können mögliche Maßnahmen sein:

```text
Reduce Effects
Reduce Resolution
Reduce Frame Rate
Move Compatible Work to CPU
Delay Background GPU Work
```

---

# CPU Pressure

Unter CPU Pressure:

```text
Reduce Parallelism
Pause Background Tasks
Reduce Update Frequency
Use Cheaper Algorithm
Reduce Optional Analysis
```

---

# NPU Pressure

KI-Workloads können beispielsweise wechseln:

```text
Large NPU Model
    ↓
Small NPU Model
    ↓
GPU Model
    ↓
CPU Model
    ↓
Non-AI Fallback
```

sofern der Contract dies erlaubt.

---

# Storage Pressure

Mögliche Degradation:

```text
Reduce Cache
Reduce Temporary Data
Delay Noncritical Writes
Reduce History Retention
Compress Data
```

---

# Network Pressure

Mögliche Degradation:

```text
Reduce Bitrate
Reduce Resolution
Batch Requests
Reduce Synchronization Frequency
Use Local Cache
Switch to Offline Mode
```

---

# Energy Pressure

Auf mobilen Systemen kann Energie ein erstklassiger Degradationsauslöser sein.

Beispiel:

```text
Battery High
    ↓
Full Performance

Battery Low
    ↓
Energy Efficient

Battery Critical
    ↓
Minimum Functional
```

---

# Thermal Pressure

Thermische Grenzen sind Teil der Resource Economy.

Mögliche Reaktion:

```text
Reduce GPU Frequency
Reduce Parallelism
Reduce Frame Rate
Move Work
Pause Background Tasks
```

---

# Thermal Safety

Thermal Safety ist ein Hard Constraint.

Wenn Degradation nicht ausreicht, muss Arbeit gestoppt werden können.

---

# Hardware Capability Degradation

NovaOS muss auch auf Hardware ohne bevorzugte Beschleuniger funktionieren können.

Beispiel:

```text
NPU
 ↓ unavailable
GPU
 ↓ unavailable
CPU SIMD
 ↓ unavailable
CPU Scalar
```

sofern die Capability einen entsprechenden Provider besitzt.

---

# Provider Degradation

Diese ADR integriert das Capability-System.

Eine Capability kann mehrere Provider besitzen.

```text
Capability
    │
    ├── Preferred Provider
    ├── Alternative Provider
    └── Minimum Provider
```

---

# Provider Fallback

Provider Fallback ist eine Form von Graceful Degradation, wenn der alternative Provider geringere Eigenschaften besitzt.

---

# Kein automatischer semantischer Fallback

Provider dürfen nur ausgetauscht werden, wenn der resultierende Contract weiterhin erfüllt wird.

---

# Algorithm Degradation

NovaOS kann einen günstigeren Algorithmus auswählen.

Beispiel:

```text
High Quality Algorithm
       ↓
Balanced Algorithm
       ↓
Fast Approximation
```

nur wenn Approximation zulässig ist.

---

# Automatische Algorithmusauswahl

Diese ADR integriert das NovaOS-Prinzip:

```text
Function(data)
```

verwendet standardmäßig automatische Algorithmusauswahl.

Ein expliziter Algorithmus kann erzwungen werden.

---

# Forced Algorithm

Wenn ein Algorithmus explizit erzwungen wurde:

```text
algorithm = Forced
```

darf NovaOS nicht automatisch auf einen anderen Algorithmus degradieren.

Falls der Algorithmus nicht mehr ausführbar ist, muss die Operation entsprechend dem Contract fehlschlagen.

---

# Preferred Algorithm

Bei:

```text
algorithm = Preferred
```

ist Fallback erlaubt.

---

# Execution Contract

`Nova.ExecutionContract` kann Degradation beschreiben.

Konzeptionell:

```text
ExecutionContract {
    latency {
        preferred = 5 ms
        maximum   = 16 ms
    }

    memory {
        preferred = 256 MiB
        maximum   = 512 MiB
    }

    quality {
        preferred = High
        minimum   = Medium
    }

    degradation = Allowed
}
```

---

# Degradation Policy

Ein Contract kann definieren:

```text
Degradation {
    mode = Allowed
}
```

oder:

```text
Degradation {
    mode = Forbidden
}
```

---

# Degradation Order

Der Consumer kann bevorzugte Degradationsreihenfolgen angeben.

Beispiel:

```text
DegradationOrder {
    1 = VisualEffects
    2 = Resolution
    3 = FrameRate
}
```

---

# Policy darf Reihenfolge optimieren

Wenn die Reihenfolge nur Preferred ist, darf NovaOS anhand der aktuellen Ressourcenlage eine bessere Alternative wählen.

---

# User Preference

Benutzerpräferenzen können die Degradation beeinflussen.

Beispiele:

```text
PreferQuality
Balanced
PreferBattery
PreferPerformance
```

---

# Explizite Benutzerentscheidung

Eine explizite Benutzerentscheidung besitzt höhere Priorität als adaptive Optimierung, solange keine höheren Hard Constraints verletzt werden.

---

# Application Preference

Anwendungen können eigene Präferenzen deklarieren.

Diese dürfen System-Hard-Constraints nicht überschreiben.

---

# Adaptive Degradation

NovaOS kann anhand historischer Daten lernen, welche Degradationsstrategie für einen bestimmten Nutzer oder Workload am besten funktioniert.

---

# Prediction Error

Das adaptive Modell kann verwenden:

```text
Prediction
    ↓
Selected Degradation
    ↓
User Reaction
    ↓
Prediction Error
    ↓
Model Adjustment
```

---

# Keine adaptive Zwangssteuerung

Das System darf den Nutzer nicht zwingen, eine gelernte Präferenz zu übernehmen.

---

# AI Independence

Graceful Degradation MUSS ohne KI funktionieren.

KI darf lediglich:

- Vorhersagen,
- Optimierungen,
- Empfehlungen

verbessern.

---

# Static Fallback Policy

Für fundamentale Systemkomponenten muss eine statische sichere Degradationsstrategie verfügbar sein.

---

# Declarative System Model

Diese ADR integriert:

```text
ADR-ARCH-0003
Deklaratives Systemmodell als Architekturgrundlage
```

Desired State:

```text
Desired {
    quality = High
}
```

Actual State:

```text
Actual {
    quality = Medium
    reason = ThermalPressure
}
```

---

# Reconciliation

Sobald die Ursache entfällt:

```text
Actual != Desired
      ↓
Reconciliation
      ↓
Attempt Recovery
      ↓
Verify Resources
      ↓
Restore Preferred State
```

---

# Degradation ist temporärer Actual State

Eine Degradation verändert nicht automatisch den langfristigen Desired State.

Beispiel:

```text
Desired = High

Actual = Medium
Reason = MemoryPressure
```

Wenn Memory Pressure verschwindet:

```text
Actual → High
```

---

# Persistente Degradation

Eine Degradation darf nur dann persistent werden, wenn:

- Benutzer,
- Administrator,
- Policy,
- aktualisierter Desired State

dies explizit festlegt.

---

# Recovery

Graceful Degradation benötigt den Gegenprozess:

```text
Recovery / Restoration
```

---

# Restoration Ladder

Beispiel:

```text
Minimum
   ↓
Medium
   ↓
High
   ↓
Full
```

---

# Keine sofortige vollständige Restoration

Bei instabilen Ressourcen soll Recovery schrittweise erfolgen.

---

# Recovery Verification

Nach jeder Restoration-Stufe müssen relevante Ressourcen und Contracts erneut geprüft werden.

---

# Restoration Cost

Eine Rückkehr auf höhere Qualität kann selbst teuer sein.

Beispiel:

- Model Loading,
- Cache Rebuild,
- GPU Resource Allocation,
- State Migration.

Der Planner soll Restoration Cost berücksichtigen.

---

# Restoration Hysteresis

Zur Vermeidung von Oscillation gelten getrennte Degradation- und Recovery-Schwellen.

---

# Oscillation Prevention

NovaOS MUSS sogenannte Quality Flapping vermeiden.

Nicht:

```text
High
Medium
High
Medium
High
Medium
```

innerhalb kurzer Zeit.

---

# Cooldown

Nach einer Degradation kann eine Cooldown-Periode verwendet werden.

---

# Rate Limit

Die Anzahl von Degradation- und Restoration-Transitions kann begrenzt werden.

---

# Structured Concurrency

Diese ADR integriert:

```text
ADR-ARCH-0010
Structured Concurrency als systemweites Ausführungsmodell
```

Degradation darf Child Tasks kontrolliert beeinflussen.

---

# Optional Child Tasks

Tasks können klassifiziert werden:

```text
Required
Preferred
Optional
Speculative
```

---

# Task Shedding

Unter Resource Pressure können zuerst:

```text
Speculative
    ↓
Optional
    ↓
Preferred
```

Tasks reduziert werden.

Required Tasks werden nur beendet, wenn die Operation insgesamt nicht mehr erfüllbar ist.

---

# Cancellation

Task Shedding verwendet Structured Cancellation.

Nicht:

```text
kill arbitrary thread
```

sondern:

```text
Cancel Scope
    ↓
Propagate Cancellation
    ↓
Cleanup
    ↓
Release Resources
```

---

# Deadline Pressure

Wenn eine Deadline gefährdet ist, kann NovaOS Qualität reduzieren, sofern der Contract dies erlaubt.

Beispiel:

```text
High Quality
estimated 20 ms

Deadline
16 ms

Balanced Quality
estimated 11 ms
```

---

# Hard Realtime

Bei Hard Realtime muss jede Degradationsstufe weiterhin beweisbare beziehungsweise garantierte Timing-Eigenschaften besitzen.

---

# Deterministic Mode

Diese ADR integriert:

```text
ADR-ARCH-0011
Deterministic Mode
```

Deterministic Mode schränkt adaptive Degradation ein.

---

# Deterministische Degradation

Eine deterministische Ausführung kann eine fest definierte Degradation Ladder besitzen.

Beispiel:

```text
If memory reservation fails:
    use Profile B

If Profile B fails:
    abort
```

---

# Keine heuristische Degradation im strikten Deterministic Mode

Nicht reproduzierbare adaptive Entscheidungen dürfen dort deaktiviert werden.

---

# Hot Replacement

Diese ADR integriert:

```text
ADR-ARCH-0012
Hot Replacement und Live Evolution
```

Wenn eine neue Provider-Generation nicht verfügbar oder fehlerhaft ist, kann NovaOS kontrolliert auf:

```text
Previous Generation
```

oder einen alternativen Provider zurückfallen.

---

# Generation Fallback

Generation Fallback muss Contract-kompatibel sein.

---

# Upgrade Degradation

Während eines Updates kann temporär ein reduzierter Service-Level zulässig sein.

Beispiel:

```text
Full Service
    ↓
Update
    ↓
ReadOnly Mode
    ↓
Full Service
```

---

# Transactional Degradation

Diese ADR integriert:

```text
ADR-ARCH-0004
Transaktionale Systemoperationen
```

Komplexe Degradation soll transaktional erfolgen können.

Beispiel:

```text
Current Profile
     ↓
Plan Reduced Profile
     ↓
Validate
     ↓
Reserve Resources
     ↓
Stage
     ↓
Activate
     ↓
Verify
     ↓
Commit
```

---

# Rollback

Wenn die neue Degradationsstufe nicht funktioniert:

```text
Rollback
```

oder:

```text
Try Next Valid Degradation Level
```

---

# Point of No Return

Nicht vollständig reversible Degradation muss dies explizit deklarieren.

---

# Architecture Introspection

Diese ADR integriert:

```text
ADR-ARCH-0013
Architecture Introspection
```

Jede relevante Degradation soll introspektierbar sein.

---

# Introspection State

Beispiel:

```text
Capability:
    Video.Decode

Desired:
    4K120

Actual:
    4K60

State:
    Degraded

Reason:
    GPUPressure

Since:
    14:32:18
```

---

# Degradation Reason

Gründe müssen strukturiert sein.

Beispiele:

```text
CPUPressure
MemoryPressure
GPUPressure
NPUUnavailable
ThermalPressure
EnergyPressure
NetworkPressure
ProviderFailure
DeadlineRisk
TrustChange
LocationUnavailable
ResourceReservationFailure
```

---

# Degradation Trace

Architecture Introspection soll erklären können:

```text
Why was quality reduced?

Which constraint triggered it?

Which alternatives were evaluated?

Why was this level selected?

What is required for restoration?
```

---

# Beispiel Decision Trace

```text
Desired:
    4K120 Ultra

GPU Pressure:
    Critical

Candidates:

4K120 High
    rejected:
        estimated GPU usage still above limit

4K60 High
    accepted

1440p120 High
    accepted

Selected:
    4K60 High

Reason:
    lowest estimated user-visible degradation
    while satisfying GPU budget
```

---

# Contract Violation vs Degradation

Die Zustände müssen unterschieden werden.

```text
Satisfied
Degraded
Violated
Failed
```

---

# Satisfied

Alle Preferred- und Hard-Anforderungen sind erfüllt.

---

# Degraded

Alle Hard-Anforderungen sind erfüllt, aber mindestens eine Preferred-Eigenschaft wurde reduziert.

---

# Violated

Mindestens ein Hard Contract ist nicht mehr erfüllt.

---

# Failed

Die Operation beziehungsweise Capability kann nicht mehr sinnvoll fortgeführt werden.

---

# Degradation darf Violation vermeiden

Der Zweck ist:

```text
Satisfied
    ↓
Degraded
```

statt:

```text
Satisfied
    ↓
Violated
    ↓
Failed
```

wenn eine gültige Degradationsstufe existiert.

---

# Self-Healing

Graceful Degradation und Self-Healing sind unterschiedliche, aber ergänzende Konzepte.

```text
Graceful Degradation:
    continue with reduced service

Self-Healing:
    repair or replace failed condition
```

---

# Kombination

Beispiel:

```text
GPU Provider fails
      ↓
Degrade to CPU Provider
      ↓
Continue Operation
      ↓
Self-Healing restarts GPU Provider
      ↓
Verify
      ↓
Restore GPU Provider
```

---

# Nova.Resilience

Nova.Resilience soll Degradation als Recovery-Instrument verwenden können.

---

# Failure Containment

Degradation soll möglichst innerhalb der kleinsten sinnvollen Failure Domain erfolgen.

Nicht jeder lokale Providerfehler darf eine systemweite Degradation verursachen.

---

# Hierarchische Degradation

Degradation kann auf unterschiedlichen Ebenen stattfinden:

```text
Task
Application
Service
Capability
Execution Domain
Device
User Session
System
Node
Cluster
```

---

# Local Degradation First

Wenn möglich soll zuerst die kleinste betroffene Ebene degradiert werden.

---

# Systemweite Degradation

Bei systemweiter Ressourcenknappheit kann eine globale Policy erforderlich sein.

Beispiel:

```text
System Thermal Emergency
```

---

# Fairness

Graceful Degradation darf nicht dazu führen, dass immer dieselbe Anwendung sämtliche Qualität verliert.

---

# Resource Fairness

Die Resource Economy berücksichtigt:

- Priorität,
- Benutzerfokus,
- Kritikalität,
- Reservierungen,
- Fairness,
- historische Nutzung.

---

# Foreground Preference

Interaktive Vordergrundarbeit kann gegenüber Best-Effort-Hintergrundarbeit bevorzugt werden.

---

# Kritische Dienste

Systemkritische Dienste besitzen Mindestressourcen, die nicht zugunsten optionaler Anwendungen degradiert werden dürfen.

---

# Emergency Reserve

NovaOS kann Ressourcen für kritische Recovery- und Systemfunktionen reservieren.

---

# Brownout Mode

NovaOS kann einen systemweiten **Brownout Mode** besitzen.

Brownout bedeutet:

```text
System remains operational
with intentionally reduced optional functionality.
```

---

# Brownout Trigger

Mögliche Auslöser:

```text
Extreme Memory Pressure
Thermal Emergency
Critical Battery
Storage Exhaustion
Large Provider Failure
Network Isolation
```

---

# Brownout Levels

Beispiel:

```text
Brownout.None
Brownout.Light
Brownout.Moderate
Brownout.Severe
Brownout.Emergency
```

---

# Brownout Policy

Bei steigenden Levels werden zunehmend optionale Funktionen deaktiviert.

---

# Emergency Mode

Emergency Mode ist stärker als normale Graceful Degradation.

Priorität:

```text
Preserve:
    Safety
    Security
    Data Integrity
    Recovery Capability
    Critical User Data
```

---

# User Interface

Das UI soll relevante Degradation sichtbar machen, ohne den Nutzer mit technischen Details zu überlasten.

---

# Keine unnötigen Warnungen

Kurzzeitige harmlose interne Degradation muss nicht zwingend als störende Warnung angezeigt werden.

---

# Relevante Benutzerinformation

Der Nutzer soll informiert werden, wenn die Degradation:

- sichtbar länger anhält,
- wichtige Funktionalität betrifft,
- Qualität erheblich reduziert,
- eine Benutzerentscheidung benötigt,
- Daten oder Workflow beeinflusst.

---

# UI-Beispiel

```text
Energiesparmodus aktiv

Videowiedergabe wurde vorübergehend
auf 1080p reduziert.
```

---

# Developer Diagnostics

Entwickler benötigen detailliertere Informationen:

```text
Contract
Pressure Source
Previous Level
Current Level
Trigger
Decision Trace
Resource Delta
Recovery Condition
```

---

# Telemetry

Degradation Events können lokale Telemetrie erzeugen.

Data Sovereignty und Privacy Policies müssen dabei eingehalten werden.

---

# Privacy

Degradation Telemetry darf nicht automatisch sensible Nutzerdaten exportieren.

---

# Remote Reporting

Remote Reporting benötigt explizite Berechtigung und entsprechende Contracts.

---

# Data Pipelines

Diese ADR integriert:

```text
ADR-ARCH-0009
Einheitliches Daten- und Objekt-Pipelining
```

Pipelines können degradierbare Stufen besitzen.

Beispiel:

```text
Input
  ↓
Decode
  ↓
AI Enhancement
  ↓
Color Correction
  ↓
Output
```

Unter Druck:

```text
Input
  ↓
Decode
  ↓
Color Correction
  ↓
Output
```

AI Enhancement wird ausgelassen.

---

# Optional Pipeline Stages

Pipeline-Stages müssen klassifizierbar sein:

```text
Required
Optional
Conditional
Enhancement
```

---

# Pipeline Replanning

Bei Resource Pressure kann die Pipeline neu geplant werden.

---

# Semantic Preservation

Pipeline Degradation muss Semantic Type Contracts erhalten.

---

# Zero-Copy

Degradation darf Data Movement verändern.

Beispiel:

```text
GPU Zero-Copy Path
```

kann bei GPU-Ausfall zu:

```text
CPU Path with Copy
```

werden, sofern:

```text
CopyAllowed = true
```

---

# Hard Zero-Copy

Wenn:

```text
CopyForbidden = true
```

gilt, darf ein kopierender Fallback nicht verwendet werden.

---

# Location Transparency

Diese ADR integriert:

```text
ADR-ARCH-0007
Location Transparency mit sichtbaren Kosten und Fehlern
```

Eine lokale Capability kann bei Ausfall theoretisch remote ausgeführt werden.

Dies ist nur zulässig, wenn:

```text
RemoteAllowed = true
```

---

# Offline Mode

Offline Mode ist eine wichtige Form von Graceful Degradation.

Beispiel:

```text
Online:
    Cloud Sync
    Remote Search
    Remote AI

Offline:
    Local Cache
    Local Search
    Local AI
```

---

# Offline First

Systemkritische NovaOS-Funktionen dürfen nicht unnötig von Cloud-Verfügbarkeit abhängen.

---

# Data Freshness

Offline-Degradation kann geringere Datenaktualität verursachen.

Dies muss als Freshness-Eigenschaft sichtbar sein.

Beispiel:

```text
DataFreshness {
    preferred <= 5 min
    degraded  <= 24 h
}
```

---

# Stale Data

Veraltete Daten dürfen nur verwendet werden, wenn der Contract dies erlaubt.

---

# Trust Degradation

Trust darf nicht im Sinne von:

```text
Verified Provider
    ↓
Untrusted Provider
```

automatisch degradiert werden, wenn der Contract `Verified` verlangt.

---

# Trust Fallback

Ein alternativer Provider muss weiterhin das Mindest-Trust-Level erfüllen.

---

# Security Isolation

Auch Isolation darf nicht unter ein Hard Minimum degradiert werden.

---

# Deterministische Qualität

Deterministic Mode kann eine feste Quality-Konfiguration erzwingen.

Falls diese nicht erfüllbar ist, kann der Contract:

```text
Fail
```

anstatt adaptiv zu degradieren.

---

# Application Lifecycle

Eine Anwendung kann ihren Degradation State über ihren Lifecycle erhalten.

---

# Suspend

Vor Suspend können optionale Ressourcen reduziert werden.

---

# Resume

Nach Resume kann die Anwendung zunächst in einem reduzierten Zustand starten und schrittweise restauriert werden.

---

# Startup Degradation

Wenn bevorzugte Provider beim Start noch nicht verfügbar sind, kann eine Anwendung mit einem zulässigen Fallback starten.

---

# Progressive Enhancement

Graceful Degradation besitzt eine Gegenrichtung:

```text
Progressive Enhancement
```

Ein System kann zunächst mit minimaler Funktion starten und zusätzliche Features aktivieren, sobald Ressourcen verfügbar sind.

---

# Boot

Auch Boot kann Progressive Enhancement verwenden.

Beispiel:

```text
Basic Boot UI
    ↓
Graphics Driver Available
    ↓
Accelerated UI
```

---

# Boot Safety

Fundamentale Boot- und Recovery-Funktionen dürfen nicht von optionalen Enhancement-Stufen abhängen.

---

# NovaDOS

NovaDOS stellt einen besonders starken Recovery-Fallback dar.

Es ist jedoch kein normaler Degradation Level des laufenden NovaOS.

NovaDOS bleibt eine separate Offline-Recovery-Umgebung.

---

# Boot Fallback

Wenn der normale NovaOS-Start nicht möglich ist:

```text
NovaOS
   ↓
Recovery Path
   ↓
NovaDOS
```

Dies ist Recovery/Fallback und nicht lediglich Quality Degradation.

---

# Compatibility Layer

Legacy-Anwendungen können nur begrenzte Degradation Contracts bereitstellen.

NovaOS darf deshalb konservative Standardwerte verwenden.

---

# Legacy Application

Wenn eine Anwendung keine semantische Degradation unterstützt, darf NovaOS nicht beliebige interne Qualitätsparameter verändern.

---

# External Formats

Dateiformate dürfen nicht stillschweigend verlustbehaftet degradiert werden.

---

# Lossy Conversion

Lossy Conversion benötigt explizite Erlaubnis.

---

# Semantic Types

Diese ADR integriert:

```text
ADR-ARCH-0008
Semantic Types als systemweites Typsystem
```

Semantic Types helfen zu bestimmen, welche Degradation semantisch zulässig ist.

---

# Beispiel

```text
Image<RGBA16F>
```

kann möglicherweise zu:

```text
Image<RGBA8>
```

konvertiert werden.

Dies ist jedoch eine potenziell verlustbehaftete Degradation und benötigt Contract-Erlaubnis.

---

# Medical Data

Bei medizinischen oder wissenschaftlichen Daten dürfen semantisch relevante Präzisionsverluste nicht automatisch als normale Qualitätsreduktion behandelt werden.

---

# Data Sovereignty

Data Sovereignty ist grundsätzlich nicht degradierbar.

---

# Information Flow

Information Flow Restrictions sind grundsätzlich nicht degradierbar.

---

# Audit

Degradation sicherheitskritischer oder geschäftskritischer Komponenten soll auditierbar sein.

---

# Audit Event

Beispiel:

```text
DegradationEvent {
    Component
    PreviousState
    NewState
    Reason
    Trigger
    Contract
    Timestamp
}
```

---

# Decision Trace

Jede bedeutende automatische Degradation soll einen strukturierten Decision Trace erzeugen können.

---

# Explainability

Das System soll erklären können:

```text
Why did this degrade?
```

und:

```text
Why can it not restore yet?
```

---

# Restoration Condition

Beispiel:

```text
Restore when:

GPU pressure < 70 %
for at least 5 seconds
```

---

# Metrics

Systemweite Metrics können umfassen:

```text
DegradationEvents
RestorationEvents
DegradationDuration
CurrentDegradationLevel
DegradationFailures
RestorationFailures
QualityFlapsPrevented
TasksShed
OptionalFeaturesDisabled
FallbackProviderSelections
AlgorithmFallbacks
BrownoutTransitions
```

---

# Resource Savings

Zusätzlich soll messbar sein:

```text
MemorySaved
CPUSaved
GPUSaved
EnergySaved
BandwidthSaved
```

---

# User Impact

Wenn sinnvoll:

```text
ResolutionReduction
FrameRateReduction
FeatureReduction
LatencyIncrease
FreshnessReduction
```

---

# Degradation Budget

Eine Komponente kann begrenzen, wie viel kumulative Degradation zulässig ist.

Beispiel:

```text
DegradationBudget {
    maximumQualityLoss = 20 %
}
```

Die konkrete Metrik ist domainspezifisch.

---

# Kein universeller Quality Score

NovaOS definiert keinen universellen systemweiten numerischen Quality Score für sämtliche Workloads.

Qualität bleibt semantisch und domainspezifisch.

---

# Quality Utility

Subsysteme können jedoch Utility Functions bereitstellen, um gültige Alternativen zu vergleichen.

---

# Cost Function

Konzeptionell:

```text
Utility =
    UserValue
    - ResourceCost
    - EnergyCost
    - LatencyCost
    - QualityLoss
    - TransitionCost
```

Hard Constraints werden vorher gefiltert und nicht durch Utility kompensiert.

---

# Keine monetäre Universalwährung

Wie bei der Resource Economy wird keine universelle künstliche Einheit verwendet, die sämtliche Ressourcen und Qualität exakt vergleichbar machen soll.

---

# Degradation Scope

Jede Degradation benötigt einen Scope.

Beispiel:

```text
Task
Pipeline
Capability
Application
UserSession
System
```

---

# Scope Isolation

Eine lokale Degradation soll nicht unnötig auf größere Scopes propagieren.

---

# Propagation

Bestimmte Degradationen können jedoch propagiert werden.

Beispiel:

```text
GPU unavailable
    ↓
Graphics Service degraded
    ↓
Applications receive reduced graphics capability
```

---

# Degradation Dependency Graph

Architecture Introspection kann Abhängigkeiten darstellen.

```text
GPU0 Failure
    │
    ├── Compositor
    │      ↓
    │   Reduced Effects
    │
    └── Video Decode
           ↓
        CPU Fallback
```

---

# Cascading Degradation

NovaOS MUSS unkontrollierte Degradationskaskaden vermeiden.

---

# Cascade Analysis

Der Planner soll prüfen können, ob eine lokale Entscheidung andere Ressourcen stärker belastet.

Beispiel:

```text
GPU overloaded
    ↓
Move work to CPU
    ↓
CPU overloaded
```

---

# Cross-Resource Effects

Degradation Planning muss Cross-Resource Effects berücksichtigen.

---

# Example

Ein CPU-Fallback kann:

- GPU entlasten,
- CPU belasten,
- Energieverbrauch erhöhen,
- Latenz erhöhen,
- Speicherbandbreite erhöhen.

---

# Resource Transfer

Degradation ist daher nicht immer reine Ressourcenreduktion.

Sie kann Last zwischen Ressourcenklassen verschieben.

---

# Global Optimization

Die Resource Economy kann globale Auswirkungen berücksichtigen.

---

# Local Autonomy

Subsysteme dürfen schnelle lokale Degradation durchführen, wenn:

- Regeln vorvalidiert sind,
- Hard Constraints erhalten bleiben,
- keine gefährlichen globalen Effekte entstehen.

---

# Fast Degradation Path

Für Echtzeitpfade kann eine vorvalidierte Degradation Ladder verwendet werden.

---

# Control Plane

Komplexe Neuplanung erfolgt im Control Plane.

---

# Fast Path

Der Fast Path darf vorbereitete Profile verwenden.

```text
Profile A
Profile B
Profile C
```

statt während kritischer Ausführung komplexe globale Solver aufzurufen.

---

# Real-Time Profile Switching

Echtzeitfähige Komponenten können vorbereitete Profile atomar wechseln.

---

# Preallocation

Degradation Profiles können vorallokierte Ressourcen verwenden, wenn schnelle Umschaltung erforderlich ist.

---

# Resource Reservation

Minimum Functional Mode kann garantierte Ressourcen reservieren.

---

# Minimum Viable Service

Jede kritische Komponente soll einen:

```text
Minimum Viable Service
```

definieren können.

---

# Minimum Functional Profile

Beispiel:

```text
MinimumFunctional {
    coreOperation = Required
    diagnostics   = Required
    recovery      = Required

    enhancements  = Disabled
}
```

---

# Failure Boundary

Wenn selbst der Minimum Functional Profile nicht erfüllbar ist:

```text
Degradation exhausted
        ↓
Failure
```

---

# Degradation Exhausted

Dieser Zustand muss explizit sein.

```text
DegradationExhausted
```

---

# Fail Safe

Nach ausgeschöpfter Degradation muss das System fail-safe reagieren.

---

# Fail Operational

Kritische Systeme können mehrere redundante Stufen besitzen, um möglichst lange operational zu bleiben.

---

# Redundancy

Redundanz selbst kann degradierbar sein.

Beispiel:

```text
3 replicas
    ↓
2 replicas
    ↓
1 replica
```

aber nur wenn der Contract dies zulässt.

---

# Availability vs Redundancy

Reduzierte Redundanz bedeutet nicht automatisch Funktionsausfall, aber erhöhtes Risiko.

Dieser Zustand muss sichtbar sein.

---

# Storage Redundancy

Bei Storage-Systemen darf Degradation der Redundanz nicht stillschweigend ignoriert werden.

---

# Data Integrity

Data Integrity ist nicht degradierbar.

---

# Replication

Replication Level kann nur reduziert werden, wenn der entsprechende Contract dies erlaubt.

---

# Network Redundancy

Multi-Path-Verbindungen können auf Single Path degradieren.

---

# Failure Domain Awareness

Redundanzplanung muss Failure Domains berücksichtigen.

---

# Location-Aware Degradation

Ein Fallback auf einen anderen Node darf keine verbotene Location verwenden.

---

# Monetary Cost

Remote- oder Cloud-Fallback kann zusätzliche Kosten erzeugen.

Contracts können definieren:

```text
MonetaryCost {
    maximum = ...
}
```

---

# Kein kostenpflichtiger automatischer Fallback ohne Policy

NovaOS darf nicht unkontrolliert kostenpflichtige Remote-Ressourcen aktivieren.

---

# User Confirmation

Bestimmte Degradation/Fallback-Entscheidungen können Benutzerbestätigung verlangen.

Beispiel:

```text
Local GPU unavailable.

Use paid cloud GPU?
```

---

# Automatic vs Confirmed

Policies können definieren:

```text
Automatic
AskUser
Forbidden
```

---

# Accessibility

Graceful Degradation darf Accessibility-Funktionen nicht wie beliebige visuelle Extras behandeln.

---

# Accessibility Requirements

Wenn eine Accessibility-Funktion für den Nutzer erforderlich ist, wird sie zu einem Hard Requirement.

---

# Beispiel

Nicht zulässig:

```text
Screen Reader
    ↓
disabled due to CPU pressure
```

wenn der Nutzer ihn benötigt.

---

# Reduced Motion

Reduced Motion ist eine Benutzeranforderung und keine Degradation.

Das System darf nicht bei Recovery automatisch Animationen aktivieren, wenn Reduced Motion konfiguriert ist.

---

# UI Effects

Optionale Acrylic-, Blur-, Shadow- und Animationseffekte sind typische degradierbare Eigenschaften.

---

# Compositor Degradation

Beispiel:

```text
Full:
    Acrylic
    Blur
    Shadows
    Animations

Reduced:
    Simple Transparency
    Reduced Shadows

Minimum:
    Opaque Surfaces
    Minimal Animation
```

---

# Boot UI

Auch die Boot UI kann degradieren:

```text
Accelerated Graphics
    ↓
Framebuffer Rendering
    ↓
Basic Safe Rendering
```

---

# Text Rendering

Fundamentale Lesbarkeit darf nicht zugunsten dekorativer Effekte degradiert werden.

---

# Audio

Audio kann beispielsweise degradieren:

```text
Spatial Audio
    ↓
Stereo
    ↓
Basic PCM
```

sofern dies zulässig ist.

---

# Audio Realtime

Audio Dropouts können stärker gewichtet werden als bestimmte Qualitätsverluste.

Beispiel:

```text
Reduce effects
before causing buffer underrun.
```

---

# Video

Video kann degradieren:

```text
HDR
    ↓
SDR

4K
    ↓
1080p

120 fps
    ↓
60 fps
    ↓
30 fps
```

nur wenn semantisch und vom Nutzer akzeptiert.

---

# AI

KI-Workloads besitzen besondere Degradationsdimensionen:

```text
Model Size
Quantization
Context Length
Sampling Strategy
Tool Availability
Reasoning Budget
Remote vs Local
```

---

# AI Accuracy

Ein kleineres Modell darf nur verwendet werden, wenn der Contract die daraus resultierende Qualitätsklasse zulässt.

---

# AI Trust

Ein weniger vertrauenswürdiger AI Provider ist keine zulässige Qualitätsdegradation, wenn Trust ein Hard Requirement ist.

---

# AI Data Sovereignty

Remote AI ist kein zulässiger Fallback bei LocalOnly.

---

# AI Non-AI Fallback

Bestimmte Funktionen können auf deterministische klassische Algorithmen zurückfallen.

Beispiel:

```text
AI Image Enhancement
    ↓
Classical Image Filter
```

---

# Scientific Computing

Scientific Computing darf Präzision nur reduzieren, wenn Error Bounds dies erlauben.

---

# Medical Computing

Medizinische Workloads benötigen besonders strenge Degradation Contracts.

---

# Safety-Critical Computing

Safety-kritische Funktionen dürfen nur auf zertifizierte beziehungsweise ausdrücklich zugelassene Fallback-Pfade wechseln.

---

# Security-Critical Computing

Kryptographische Sicherheit darf nicht durch schwächere Algorithmen degradiert werden, wenn Mindestanforderungen verletzt würden.

---

# Cryptographic Agility

Ein alternativer kryptographischer Algorithmus ist nur zulässig, wenn er den Security Contract erfüllt.

---

# Logging

Logging kann unter Pressure degradiert werden, aber sicherheitsrelevante Audit Logs dürfen nicht wie normale Debug Logs behandelt werden.

---

# Logging Classes

Beispiel:

```text
Audit
Critical
Operational
Debug
Trace
```

Unter Pressure:

```text
Trace
 ↓
Debug
 ↓
Operational
```

kann reduziert werden.

Audit bleibt entsprechend Policy erhalten.

---

# Observability Degradation

Observability selbst kann Ressourcen benötigen.

Nicht kritische Metrics können reduziert werden.

Kritische Health- und Security-Signale müssen erhalten bleiben.

---

# Telemetry Sampling

Sampling Rate kann degradierbar sein.

---

# Architecture Introspection Availability

Fundamentale Architecture Introspection für Recovery und Diagnose soll auch im Brownout Mode verfügbar bleiben.

---

# Contract Introspection

Das System muss darstellen können:

```text
Preferred
Current
Minimum
Reason
RecoveryCondition
```

---

# Normative Festlegungen

1. NovaOS MUSS Graceful Degradation als systemweites Architekturprinzip unterstützen.

2. Graceful Degradation MUSS kontrollierte reduzierte Betriebszustände ermöglichen.

3. Graceful Degradation DARF Hard Constraints NICHT stillschweigend verletzen.

4. Safety Requirements DÜRFEN NICHT automatisch degradiert werden.

5. Security Requirements DÜRFEN NICHT unter ihr Hard Minimum degradiert werden.

6. Data Sovereignty DÜRFEN NICHT automatisch abgeschwächt werden.

7. Information Flow Restrictions DÜRFEN NICHT automatisch abgeschwächt werden.

8. Correctness DÜRFEN NICHT zugunsten von Performance verletzt werden.

9. Approximation MUSS explizit erlaubt sein.

10. Lossy Conversion MUSS explizit erlaubt sein.

11. Degradierbare Eigenschaften MÜSSEN von nicht degradierbaren Eigenschaften unterscheidbar sein.

12. Contracts MÜSSEN Preferred Values beschreiben können.

13. Contracts MÜSSEN Hard Minimums beschreiben können.

14. Contracts MÜSSEN Hard Maximums beschreiben können.

15. Contracts SOLLEN Acceptable Ranges beschreiben können.

16. Contracts MÜSSEN Forbidden States beschreiben können.

17. Degradation MUSS über explizite Contracts steuerbar sein.

18. Degradation MUSS vollständig verboten werden können.

19. Degradation MUSS explizit erlaubt werden können.

20. Degradation Ladders MÜSSEN beschreibbar sein.

21. Degradation Levels SOLLEN stabile Identitäten besitzen können.

22. Degradation Profiles SOLLEN unterstützt werden.

23. Qualität MUSS mehrdimensional modellierbar sein.

24. NovaOS DARF keinen universellen Quality Score voraussetzen.

25. Resolution MUSS als degradierbare Dimension beschreibbar sein.

26. Frame Rate MUSS als degradierbare Dimension beschreibbar sein.

27. Precision MUSS als degradierbare Dimension beschreibbar sein.

28. Feature Set MUSS als degradierbare Dimension beschreibbar sein.

29. Model Size SOLL als degradierbare Dimension beschreibbar sein.

30. Data Freshness SOLL als degradierbare Dimension beschreibbar sein.

31. Redundancy SOLL als degradierbare Dimension beschreibbar sein.

32. Degradation Planning MUSS Hard Constraints zuerst prüfen.

33. Soft Preferences DÜRFEN nur innerhalb der Hard Constraints optimiert werden.

34. Safety MUSS höhere Priorität als Performance besitzen.

35. Security MUSS höhere Priorität als Performance besitzen.

36. Data Sovereignty MUSS höhere Priorität als Performance besitzen.

37. Hard Realtime MUSS höhere Priorität als adaptive Optimierung besitzen.

38. Determinism MUSS höhere Priorität als adaptive Optimierung besitzen, wenn es als Hard Requirement gilt.

39. Explizite Benutzeranforderungen MÜSSEN innerhalb höherer Hard Constraints berücksichtigt werden.

40. Resource Economy MUSS Pressure Signals für Degradation bereitstellen können.

41. CPU Pressure MUSS als Degradation Trigger verwendet werden können.

42. Memory Pressure MUSS als Degradation Trigger verwendet werden können.

43. GPU Pressure MUSS als Degradation Trigger verwendet werden können.

44. NPU Pressure MUSS als Degradation Trigger verwendet werden können.

45. Storage Pressure MUSS als Degradation Trigger verwendet werden können.

46. Network Pressure MUSS als Degradation Trigger verwendet werden können.

47. Energy Pressure MUSS als Degradation Trigger verwendet werden können.

48. Thermal Pressure MUSS als Degradation Trigger verwendet werden können.

49. Pressure MUSS von Failure unterscheidbar sein.

50. Pressure Levels SOLLEN unterstützt werden.

51. Degradation MUSS Hysterese unterstützen können.

52. Restoration MUSS Hysterese unterstützen können.

53. Kurzzeitige Pressure Peaks SOLLEN debounced werden können.

54. Stability Windows SOLLEN unterstützt werden.

55. Quality Flapping MUSS vermieden werden.

56. Cooldown Periods SOLLEN unterstützt werden.

57. Transition Rate Limits SOLLEN unterstützt werden.

58. Memory Pressure SOLL zunächst rekonstruierbare und optionale Daten berücksichtigen.

59. Kritische Daten DÜRFEN NICHT wie Cache behandelt werden.

60. Cache Targets SOLLEN unter Memory Pressure reduziert werden können.

61. Optional Background Work SOLL unter Pressure pausiert werden können.

62. GPU Effects SOLLEN unter GPU Pressure reduzierbar sein.

63. GPU Work SOLL auf kompatible alternative Provider verschoben werden können.

64. CPU Parallelism SOLL unter CPU Pressure reduzierbar sein.

65. Optional CPU Analysis SOLL pausiert werden können.

66. KI-Workloads SOLLEN kleinere Modelle verwenden können, wenn der Contract dies erlaubt.

67. KI-Workloads SOLLEN auf Nicht-KI-Fallbacks wechseln können, wenn der Contract dies erlaubt.

68. Storage Cache SOLL unter Storage Pressure reduzierbar sein.

69. Nicht kritische Writes SOLLEN verzögerbar sein.

70. Network Bitrate SOLL unter Network Pressure reduzierbar sein.

71. Synchronization Frequency SOLL reduzierbar sein.

72. Offline Mode MUSS unterstützt werden können.

73. Energieverbrauch MUSS als systemweite Optimierungsdimension berücksichtigt werden können.

74. Thermal Safety MUSS Hard Constraint bleiben.

75. Hardwarebeschleuniger DÜRFEN NICHT Voraussetzung für semantisch allgemein definierte Capabilities sein, sofern ein sinnvoller alternativer Provider existiert.

76. Provider Fallback MUSS Contract-kompatibel sein.

77. Provider Fallback DARF Security NICHT abschwächen.

78. Provider Fallback DARF Trust NICHT unter das Minimum reduzieren.

79. Provider Fallback DARF Sovereignty NICHT verletzen.

80. Provider Fallback DARF Forced Provider Requirements NICHT ignorieren.

81. Algorithm Degradation MUSS Contract-kompatibel sein.

82. Approximate Algorithms DÜRFEN nur verwendet werden, wenn Approximation erlaubt ist.

83. Forced Algorithms DÜRFEN NICHT automatisch ersetzt werden.

84. Preferred Algorithms DÜRFEN ersetzt werden, wenn der Contract dies erlaubt.

85. Execution Contracts MÜSSEN Degradation Requirements beschreiben können.

86. Execution Contracts MÜSSEN Preferred Quality beschreiben können.

87. Execution Contracts MÜSSEN Minimum Quality beschreiben können.

88. Degradation Order SOLL deklarierbar sein.

89. Degradation Order SOLL als Hard oder Preferred klassifizierbar sein.

90. Benutzerpräferenzen SOLLEN Degradation beeinflussen können.

91. Benutzerpräferenzen DÜRFEN Hard Constraints NICHT überschreiben.

92. Application Preferences SOLLEN berücksichtigt werden.

93. Application Preferences DÜRFEN System-Hard-Constraints NICHT überschreiben.

94. Adaptive Degradation SOLL unterstützt werden können.

95. Prediction Error DARF adaptive Degradation verbessern.

96. Adaptive Modelle DÜRFEN Hard Constraints NICHT verändern.

97. Graceful Degradation MUSS ohne KI funktionieren.

98. Fundamentale Systemkomponenten MÜSSEN statische sichere Fallback Policies besitzen können.

99. Degradation MUSS in Desired/Actual State integrierbar sein.

100. Degraded Actual State DARF Desired State NICHT automatisch dauerhaft verändern.

101. Reconciliation MUSS eine Restoration versuchen können.

102. Persistente Degradation MUSS explizit autorisiert werden.

103. Restoration MUSS als Gegenprozess zur Degradation unterstützt werden.

104. Restoration MUSS schrittweise erfolgen können.

105. Restoration MUSS Ressourcen erneut validieren.

106. Restoration MUSS Contracts erneut validieren.

107. Restoration Cost SOLL berücksichtigt werden.

108. Model Loading Cost SOLL bei AI Restoration berücksichtigt werden.

109. Cache Rebuild Cost SOLL bei Restoration berücksichtigt werden.

110. State Migration Cost SOLL bei Restoration berücksichtigt werden.

111. Structured Concurrency MUSS bei Task Shedding berücksichtigt werden.

112. Tasks SOLLEN als Required, Preferred, Optional oder Speculative klassifizierbar sein.

113. Speculative Tasks SOLLEN zuerst abgebrochen werden können.

114. Optional Tasks SOLLEN vor Required Tasks reduziert werden.

115. Task Shedding MUSS Structured Cancellation verwenden können.

116. Task Shedding MUSS Ressourcen kontrolliert freigeben.

117. Deadline Risk MUSS Degradation auslösen können.

118. Deadline-basierte Degradation MUSS Contract-kompatibel sein.

119. Hard-Realtime-Degradation MUSS weiterhin Timing Guarantees erfüllen.

120. Deterministic Mode MUSS adaptive Degradation einschränken können.

121. Deterministic Mode SOLL feste Degradation Ladders verwenden können.

122. Strict Deterministic Mode SOLL heuristische Degradation deaktivieren können.

123. Hot Replacement MUSS Graceful Degradation berücksichtigen können.

124. Generation Fallback MUSS Contract-kompatibel sein.

125. Update-Prozesse SOLLEN temporäre reduzierte Service Levels definieren können.

126. Komplexe Degradation MUSS transaktional implementierbar sein.

127. Eine neue Degradation Stage MUSS vor Commit validiert werden können.

128. Fehlgeschlagene Degradation MUSS Rollback oder weiteren zulässigen Fallback ermöglichen.

129. Nicht reversible Degradation MUSS Point-of-No-Return-Semantik besitzen können.

130. Degradation State MUSS über Architecture Introspection sichtbar sein.

131. Desired Quality MUSS introspektierbar sein.

132. Actual Quality MUSS introspektierbar sein.

133. Degradation Reason MUSS introspektierbar sein.

134. Degradation Duration SOLL introspektierbar sein.

135. Restoration Condition SOLL introspektierbar sein.

136. Degradation Decision Trace SOLL introspektierbar sein.

137. Contract State `Degraded` MUSS von `Violated` unterscheidbar sein.

138. `Satisfied` MUSS von `Degraded` unterscheidbar sein.

139. `Degraded` MUSS von `Failed` unterscheidbar sein.

140. Degradation MUSS versuchen, Contract Violation zu vermeiden.

141. Self-Healing MUSS von Graceful Degradation konzeptionell getrennt bleiben.

142. Self-Healing und Graceful Degradation MÜSSEN kombinierbar sein.

143. Nova.Resilience SOLL Degradation als Recovery-Instrument verwenden können.

144. Degradation SOLL innerhalb der kleinsten sinnvollen Failure Domain erfolgen.

145. Lokale Providerfehler SOLLEN NICHT unnötig systemweite Degradation verursachen.

146. Degradation Scopes MÜSSEN hierarchisch modellierbar sein.

147. Task-Level Degradation MUSS unterstützt werden können.

148. Application-Level Degradation MUSS unterstützt werden können.

149. Capability-Level Degradation MUSS unterstützt werden können.

150. System-Level Degradation MUSS unterstützt werden können.

151. Systemweite Degradation MUSS Fairness berücksichtigen.

152. Vordergrundarbeit SOLL gegenüber Best-Effort-Hintergrundarbeit bevorzugt werden können.

153. Kritische Dienste MÜSSEN Mindestressourcen besitzen können.

154. Emergency Reserves SOLLEN unterstützt werden.

155. NovaOS SOLL einen Brownout Mode unterstützen können.

156. Brownout MUSS optionale Funktionalität kontrolliert reduzieren.

157. Brownout MUSS mehrere Severity Levels unterstützen können.

158. Brownout DARF Safety NICHT reduzieren.

159. Brownout DARF Security NICHT unter Hard Requirements reduzieren.

160. Brownout MUSS Recovery Capability erhalten.

161. Emergency Mode MUSS Data Integrity priorisieren.

162. Relevante Degradation SOLL im UI sichtbar gemacht werden.

163. Kurzzeitige interne Degradation MUSS NICHT zwingend als Benutzerwarnung erscheinen.

164. Langfristige erhebliche Degradation SOLL dem Nutzer angezeigt werden.

165. Benutzerentscheidungen SOLLEN angefordert werden, wenn Policies dies verlangen.

166. Developer Diagnostics MÜSSEN detaillierte Degradation Information liefern können.

167. Telemetry MUSS Privacy Policies respektieren.

168. Remote Telemetry MUSS Data Sovereignty respektieren.

169. Pipeline Stages MÜSSEN als Required oder Optional klassifizierbar sein können.

170. Pipeline Enhancements SOLLEN separat klassifizierbar sein.

171. Pipeline Replanning MUSS Graceful Degradation unterstützen können.

172. Pipeline Degradation MUSS Semantic Types respektieren.

173. Pipeline Degradation MUSS Hard Data Contracts respektieren.

174. Zero-Copy Preferences DÜRFEN bei Degradation geändert werden, wenn sie Soft Requirements sind.

175. Hard No-Copy Requirements DÜRFEN NICHT durch Degradation verletzt werden.

176. Remote Fallback MUSS explizit erlaubt sein.

177. Offline Mode MUSS lokale Fallbacks verwenden können.

178. Data Freshness MUSS bei Offline-Degradation sichtbar sein.

179. Stale Data DÜRFEN nur verwendet werden, wenn der Contract dies erlaubt.

180. Trust Requirements DÜRFEN NICHT als normale Quality Dimension unter ihr Minimum degradiert werden.

181. Security Isolation DÜRFEN NICHT unter ihr Hard Minimum degradiert werden.

182. Application Startup MUSS mit zulässigen Fallback Providern möglich sein können.

183. Progressive Enhancement SOLL unterstützt werden.

184. Progressive Enhancement MUSS dieselben Contract-Grenzen wie Restoration respektieren.

185. Boot MUSS mit reduzierten sicheren Rendering Paths funktionieren können.

186. Fundamentale Boot-Funktionen DÜRFEN NICHT von optionaler Beschleunigung abhängen.

187. NovaDOS MUSS als separate Recovery-Umgebung behandelt werden.

188. NovaDOS DARF NICHT mit normalem Quality Degradation State gleichgesetzt werden.

189. Legacy-Anwendungen DÜRFEN NICHT willkürlich semantisch degradiert werden.

190. Conservative Defaults SOLLEN für unbekannte Legacy-Degradation-Semantik verwendet werden.

191. Lossy Format Conversion MUSS explizit erlaubt sein.

192. Semantic Types MÜSSEN bei Degradation berücksichtigt werden.

193. Medizinische Daten DÜRFEN NICHT ohne explizite Erlaubnis präzisionsreduziert werden.

194. Wissenschaftliche Daten DÜRFEN NICHT ohne zulässige Error Bounds approximiert werden.

195. Data Integrity MUSS nicht degradierbar sein.

196. Audit-relevante Degradation SOLL protokolliert werden.

197. Degradation Events SOLLEN Previous und New State enthalten.

198. Degradation Events SOLLEN Trigger und Reason enthalten.

199. Decision Tracing SOLL für bedeutende automatische Degradation verfügbar sein.

200. Restoration Conditions SOLLEN strukturiert darstellbar sein.

201. Degradation Metrics MÜSSEN erfassbar sein können.

202. Restoration Metrics MÜSSEN erfassbar sein können.

203. Degradation Duration SOLL messbar sein.

204. Task Shedding SOLL messbar sein.

205. Fallback Provider Selection SOLL messbar sein.

206. Algorithm Fallback SOLL messbar sein.

207. Brownout Transitions SOLLEN messbar sein.

208. Resource Savings SOLLEN messbar sein.

209. User-visible Quality Loss SOLL domainspezifisch messbar sein können.

210. NovaOS DARF keinen universellen Quality-Loss-Wert für alle Domains erzwingen.

211. Domain-specific Utility Functions SOLLEN unterstützt werden können.

212. Hard Constraints DÜRFEN NICHT durch Utility Functions kompensiert werden.

213. Degradation MUSS einen definierten Scope besitzen können.

214. Lokale Degradation SOLL lokal bleiben, wenn keine Propagation erforderlich ist.

215. Degradation Propagation MUSS explizit modellierbar sein.

216. Architecture Introspection SOLL Degradation Dependency Graphs darstellen können.

217. Unkontrollierte Degradationskaskaden MÜSSEN vermieden werden.

218. Cross-Resource Effects MÜSSEN berücksichtigt werden können.

219. GPU-to-CPU-Fallback MUSS CPU Pressure berücksichtigen.

220. CPU-to-Remote-Fallback MUSS Network, Trust und Sovereignty berücksichtigen.

221. Degradation DARF Last zwischen Ressourcenklassen verschieben.

222. Resource Economy MUSS solche Lastverschiebungen berücksichtigen können.

223. Subsysteme SOLLEN schnelle lokale Degradation durchführen können.

224. Schnelle lokale Degradation MUSS vorvalidierte Regeln verwenden können.

225. Echtzeitpfade SOLLEN vorvalidierte Degradation Profiles verwenden.

226. Fast Paths DÜRFEN NICHT von komplexer globaler Neuplanung abhängig sein.

227. Control Plane SOLL komplexe Degradation Planning übernehmen.

228. Profile Switching SOLL atomar implementierbar sein.

229. Minimum Functional Profiles SOLLEN vorallokierte Ressourcen verwenden können.

230. Kritische Komponenten SOLLEN einen Minimum Viable Service definieren.

231. Minimum Viable Service MUSS Safety und Security erhalten.

232. Wenn Minimum Viable Service nicht erfüllbar ist, MUSS Failure möglich sein.

233. `DegradationExhausted` MUSS als eigener Zustand darstellbar sein.

234. Nach ausgeschöpfter Degradation MUSS Fail-Safe-Verhalten möglich sein.

235. Redundancy MUSS als degradierbare Eigenschaft beschreibbar sein.

236. Redundancy DARF nur innerhalb Contract Limits reduziert werden.

237. Reduzierte Redundanz MUSS sichtbar sein.

238. Data Integrity DARF durch Redundanzreduktion NICHT stillschweigend aufgegeben werden.

239. Replication Levels MÜSSEN Contract-basiert steuerbar sein.

240. Failure Domains MÜSSEN bei Redundanzdegradation berücksichtigt werden.

241. Location Constraints MÜSSEN bei Fallback berücksichtigt werden.

242. Monetary Cost SOLL bei Remote-Fallback berücksichtigt werden.

243. Kostenpflichtiger Remote-Fallback SOLL Policy-gesteuert sein.

244. Benutzerbestätigung MUSS für kostenpflichtigen Fallback konfigurierbar sein.

245. Accessibility Requirements MÜSSEN als Hard Requirements klassifizierbar sein.

246. Benötigte Accessibility Features DÜRFEN NICHT wie optionale Effekte deaktiviert werden.

247. Reduced Motion MUSS als Benutzeranforderung erhalten bleiben.

248. Dekorative UI-Effekte SOLLEN degradierbar sein.

249. UI-Lesbarkeit MUSS erhalten bleiben.

250. Compositor MUSS einen Minimum Functional Rendering Path besitzen können.

251. Audio Effects SOLLEN degradierbar sein.

252. Audio-Realtime-Anforderungen MÜSSEN vor optionalen Effekten priorisiert werden können.

253. Video Quality SOLL mehrdimensional degradierbar sein.

254. HDR-to-SDR-Fallback MUSS semantisch explizit sein.

255. AI Model Size SOLL degradierbar sein.

256. AI Context Length SOLL degradierbar sein, wenn der Contract dies erlaubt.

257. AI Quantization SOLL degradierbar sein, wenn Accuracy Requirements erhalten bleiben.

258. AI Trust DARF NICHT unter Hard Requirements degradiert werden.

259. AI Data Sovereignty DARF NICHT degradiert werden.

260. AI-to-Classical-Fallback SOLL möglich sein.

261. Scientific Precision MUSS Error Bounds respektieren.

262. Medical Degradation MUSS domainspezifische Sicherheitsanforderungen respektieren.

263. Safety-Critical Fallbacks MÜSSEN explizit zugelassen sein.

264. Kryptographische Mindeststärke DARF NICHT degradiert werden.

265. Cryptographic Agility MUSS Security Contracts respektieren.

266. Debug Logging SOLL unter Pressure reduzierbar sein.

267. Audit Logging MUSS separat klassifiziert werden.

268. Security Audit Requirements DÜRFEN NICHT wie Debug Logging behandelt werden.

269. Nicht kritische Observability SOLL degradierbar sein.

270. Kritische Health Signals MÜSSEN im Minimum Functional Mode verfügbar bleiben.

271. Kritische Security Signals MÜSSEN im Minimum Functional Mode verfügbar bleiben.

272. Architecture Introspection SOLL im Brownout Mode weiterhin verfügbar sein.

273. Contract Introspection MUSS Preferred, Actual und Minimum unterscheiden können.

274. Degradation MUSS nachvollziehbar erklären können, warum sie ausgelöst wurde.

275. Restoration MUSS nachvollziehbar erklären können, warum sie noch nicht möglich ist.

276. Degradation Policy MUSS versionierbar sein.

277. Degradation Profiles MÜSSEN versionierbar sein können.

278. Hot Replacement einer Degradation Policy MUSS Contract-kompatibel sein.

279. Degradation State MUSS bei Provider Replacement erhalten oder kontrolliert migriert werden können.

280. Degradation History SOLL für Diagnose verfügbar sein.

281. Degradation History MUSS begrenzbar sein.

282. History Growth DARF NICHT unkontrolliert Ressourcen verbrauchen.

283. Degradation State MUSS bei Crash Recovery rekonstruierbar sein können, wenn dies erforderlich ist.

284. Flüchtige Quality States MÜSSEN NICHT zwingend persistent gespeichert werden.

285. Persistenz MUSS domainspezifisch steuerbar sein.

286. Degradation darf fundamentale Kernel-Korrektheit NICHT von Userspace-Optimierern abhängig machen.

287. Kernelkritische Degradation Paths MÜSSEN statisch sicher implementierbar sein.

288. Resource Pressure Handling MUSS auch bei Ausfall adaptiver Dienste funktionieren.

289. Graceful Degradation MUSS ohne Netzwerk funktionieren.

290. Graceful Degradation MUSS ohne Cloud funktionieren.

291. Graceful Degradation MUSS ohne KI funktionieren.

292. Graceful Degradation MUSS ohne GPU funktionieren können, soweit die jeweilige Capability einen CPU-Fallback vorsieht.

293. Graceful Degradation MUSS mit Location Transparency kompatibel sein.

294. Graceful Degradation MUSS mit Zero-Copy kompatibel sein.

295. Graceful Degradation MUSS mit Semantic Types kompatibel sein.

296. Graceful Degradation MUSS mit Structured Concurrency kompatibel sein.

297. Graceful Degradation MUSS mit Deterministic Mode kompatibel sein.

298. Graceful Degradation MUSS mit Hot Replacement kompatibel sein.

299. Graceful Degradation MUSS mit Architecture Introspection kompatibel sein.

300. Graceful Degradation MUSS mit expliziten Component Contracts kompatibel sein.

301. Degradation DARF nicht als versteckter Fehlerpfad implementiert werden.

302. Jede relevante Degradation Stage MUSS einen definierten Zustand besitzen.

303. Jede relevante Degradation Stage SOLL einen definierten Eintrittsgrund besitzen.

304. Jede relevante Degradation Stage SOLL eine definierte Restoration Condition besitzen.

305. Jede relevante Degradation Stage SOLL eine definierte Resource-Auswirkung besitzen.

306. Jede relevante Degradation Stage SOLL eine definierte User-Impact-Beschreibung besitzen können.

307. Degradation Transitions SOLLEN atomar oder transaktional erfolgen, wenn inkonsistente Zwischenzustände gefährlich wären.

308. Parallel auftretende Degradation Trigger MÜSSEN konsistent zusammengeführt werden können.

309. Ein später eintretender schwächerer Trigger DARF eine weiterhin notwendige stärkere Degradation NICHT aufheben.

310. Restoration DARF erst erfolgen, wenn alle relevanten Blocking Constraints dies erlauben.

311. Mehrere Resource Pressure Sources MÜSSEN gemeinsam berücksichtigt werden können.

312. Degradation Planning MUSS vermeiden, ein Problem lediglich in eine andere kritische Ressource zu verschieben.

313. Emergency Degradation MUSS normale Soft Preferences überstimmen können.

314. Emergency Degradation DARF Hard Safety, Security und Sovereignty NICHT überstimmen.

315. Degradation Policies SOLLEN testbar sein.

316. Degradation Ladders SOLLEN automatisch validierbar sein.

317. Ungültige Ladders MÜSSEN erkannt werden können.

318. Eine Ladder DARF keine Stufe enthalten, die ihr eigenes Hard Minimum verletzt.

319. Restoration Paths SOLLEN auf Erreichbarkeit geprüft werden können.

320. NovaOS MUSS Graceful Degradation als kontrollierten Übergang zwischen gültigen Systemzuständen behandeln.

---

# Konsequenzen

## Positive Konsequenzen

- NovaOS kann unter Ressourcenknappheit länger funktionsfähig bleiben.
- Ein einzelner Provider-Ausfall führt seltener zum vollständigen Funktionsausfall.
- Anwendungen können auf schwächerer Hardware sinnvoll funktionieren.
- GPU-, NPU- und andere Beschleuniger bleiben Optimierungen statt zwingende Voraussetzungen.
- Offline-Betrieb wird robuster.
- Netzwerkprobleme können kontrolliert abgefangen werden.
- Energieverbrauch kann dynamisch reduziert werden.
- thermische Probleme können frühzeitig behandelt werden.
- Memory Pressure kann semantisch statt nur durch aggressive Prozessbeendigung behandelt werden.
- optionale Funktionen können vor Kernfunktionen reduziert werden.
- Structured Concurrency ermöglicht kontrolliertes Task Shedding.
- Resource Economy erhält eine systemweite Reaktionsstrategie.
- Execution Contracts können Mindestqualität garantieren.
- Provider- und Algorithmusauswahl können Degradation berücksichtigen.
- Hot Replacement wird robuster.
- Self-Healing kann während der Reparatur einen reduzierten Service bereitstellen.
- Architecture Introspection kann Qualitätszustände erklären.
- Benutzer können zwischen Performance, Qualität und Energie priorisieren.
- deterministische Workloads können feste Fallback-Pfade verwenden.
- Brownout Modes ermöglichen kontrollierten Systembetrieb in Extremsituationen.
- NovaOS wird weniger abhängig von Cloud, Netzwerk und spezifischer Hardware.
- Progressive Enhancement ermöglicht schnellen Start mit späterer Qualitätssteigerung.

---

# Negative Konsequenzen

- Subsysteme müssen mehrere gültige Betriebsstufen definieren.
- Testing wird komplexer.
- Degradation Ladders müssen gepflegt werden.
- Cross-Resource-Effekte erschweren Planung.
- falsche Degradation Policies können Qualität unnötig reduzieren.
- zu aggressive Restoration kann Quality Flapping verursachen.
- Hysterese und Stability Windows erhöhen Zustandskomplexität.
- Anwendungen müssen Mindestqualität explizit definieren.
- Provider müssen unterschiedliche Quality Profiles deklarieren.
- Architecture Introspection benötigt zusätzliche Zustandsinformationen.
- Resource Economy muss User Impact berücksichtigen.
- Legacy-Anwendungen können nur eingeschränkt semantisch degradiert werden.
- verteilte Systeme benötigen zusätzliche Location-, Trust- und Sovereignty-Prüfungen.
- Degradation kann zusätzlichen Speicher für alternative Provider oder vorbereitete Profiles benötigen.
- Echtzeitsysteme benötigen vorvalidierte Degradation Paths.
- nicht alle Domains besitzen einfach vergleichbare Quality Metrics.

---

# Verworfene Alternativen

## Fail Fast bei jeder Ressourcenknappheit

Verworfen.

Resource Pressure bedeutet nicht automatisch, dass eine Operation unmöglich ist.

---

## Immer maximale Qualität

Verworfen.

Dies führt unter begrenzten Ressourcen zu:

- schlechterer Responsiveness,
- Deadline Misses,
- Thermal Throttling,
- unnötigem Energieverbrauch,
- vollständigen Ausfällen.

---

## Betriebssystem entscheidet Qualität vollständig selbst

Verworfen.

Das Betriebssystem kennt nicht automatisch alle semantischen Mindestanforderungen einer Anwendung.

Contracts sind erforderlich.

---

## Anwendung entscheidet vollständig selbst

Verworfen.

Die Anwendung besitzt keinen vollständigen Überblick über:

- systemweite Ressourcen,
- andere Anwendungen,
- Thermal State,
- Energie,
- globale Policies,
- Hardwarekonkurrenz.

---

## Nur binärer Fallback

```text
Primary
Fallback
```

Verworfen als allgemeines Modell.

Viele Workloads benötigen mehrere Degradationsstufen.

---

## Universeller Quality Score

Verworfen.

Qualität ist domainspezifisch und mehrdimensional.

---

## Automatische Cloud-Auslagerung

Verworfen.

Kann verletzen:

- Data Sovereignty,
- Privacy,
- Trust,
- Kostenlimits,
- Offline-Anforderungen.

---

## Automatisches Abschwächen von Security

Verworfen.

Security ist kein normaler Quality-Regler.

---

## Automatisches Abschwächen von Trust

Verworfen.

Ein weniger vertrauenswürdiger Provider ist nicht automatisch ein zulässiger Fallback.

---

## Automatisches Abschwächen von Data Sovereignty

Verworfen.

LocalOnly bleibt LocalOnly.

---

## Unbegrenztes Task Killing

Verworfen.

Task Shedding muss Structured Concurrency und Cleanup berücksichtigen.

---

## Beliebiges Algorithmus-Fallback

Verworfen.

Forced Algorithms und Semantic Requirements müssen respektiert werden.

---

## KI als notwendiger Degradation Planner

Verworfen.

Fundamentale Resilienz darf nicht von KI-Verfügbarkeit abhängen.

---

## Sofortige Restoration nach Resource Recovery

Verworfen.

Führt zu Oscillation und Quality Flapping.

---

## Degradation ohne Introspection

Verworfen.

Automatische Qualitätsänderungen müssen diagnostizierbar sein.

---

## Degradation als Fehler

Verworfen.

Ein gültiger reduzierter Zustand ist kein Fehler.

---

## Degradation als dauerhafte Konfigurationsänderung

Verworfen.

Der Desired State bleibt grundsätzlich bestehen.

---

# Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0008_Semantic_Types_als_systemweites_Typsystem`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell`

---

# Zugehörige NPSPECs

- `NPSPEC-ARCH-GRACEFULDEGRADATION-0001`
- `NPSPEC-DEGRADATION-MODEL-0001`
- `NPSPEC-DEGRADATION-STATE-0001`
- `NPSPEC-DEGRADATION-LEVEL-0001`
- `NPSPEC-DEGRADATION-PROFILE-0001`
- `NPSPEC-DEGRADATION-LADDER-0001`
- `NPSPEC-DEGRADATION-DIMENSION-0001`
- `NPSPEC-DEGRADATION-POLICY-0001`
- `NPSPEC-DEGRADATION-PLANNER-0001`
- `NPSPEC-DEGRADATION-TRANSITION-0001`
- `NPSPEC-DEGRADATION-HYSTERESIS-0001`
- `NPSPEC-DEGRADATION-STABILITY-0001`
- `NPSPEC-DEGRADATION-RESTORATION-0001`
- `NPSPEC-DEGRADATION-RECOVERY-0001`
- `NPSPEC-DEGRADATION-BROWNOUT-0001`
- `NPSPEC-DEGRADATION-EMERGENCY-0001`
- `NPSPEC-DEGRADATION-LOADSHEDDING-0001`
- `NPSPEC-DEGRADATION-TASKSHEDDING-0001`
- `NPSPEC-DEGRADATION-QUALITY-0001`
- `NPSPEC-DEGRADATION-FEATURE-0001`
- `NPSPEC-DEGRADATION-PRECISION-0001`
- `NPSPEC-DEGRADATION-FRESHNESS-0001`
- `NPSPEC-DEGRADATION-REDUNDANCY-0001`
- `NPSPEC-DEGRADATION-INTROSPECTION-0001`
- `NPSPEC-DEGRADATION-DECISIONTRACE-0001`
- `NPSPEC-DEGRADATION-METRICS-0001`
- `NPSPEC-DEGRADATION-AUDIT-0001`
- `NPSPEC-DEGRADATION-TESTING-0001`
- `NPSPEC-DEGRADATION-FAULTINJECTION-0001`
- `NPSPEC-RESOURCE-PRESSURE-0001`
- `NPSPEC-RESOURCE-CPU-PRESSURE-0001`
- `NPSPEC-RESOURCE-MEMORY-PRESSURE-0001`
- `NPSPEC-RESOURCE-GPU-PRESSURE-0001`
- `NPSPEC-RESOURCE-NPU-PRESSURE-0001`
- `NPSPEC-RESOURCE-STORAGE-PRESSURE-0001`
- `NPSPEC-RESOURCE-NETWORK-PRESSURE-0001`
- `NPSPEC-RESOURCE-ENERGY-PRESSURE-0001`
- `NPSPEC-RESOURCE-THERMAL-PRESSURE-0001`
- `NPSPEC-RESOURCE-LOADSHEDDING-0001`
- `NPSPEC-RESOURCE-EMERGENCYRESERVE-0001`
- `NPSPEC-EXECUTION-DEGRADATION-0001`
- `NPSPEC-EXECUTION-QUALITY-0001`
- `NPSPEC-EXECUTION-FALLBACK-0001`
- `NPSPEC-EXECUTION-PROGRESSIVEENHANCEMENT-0001`
- `NPSPEC-CAPABILITY-DEGRADATION-0001`
- `NPSPEC-CAPABILITY-FALLBACK-0001`
- `NPSPEC-PROVIDER-DEGRADATION-0001`
- `NPSPEC-PROVIDER-FALLBACK-0001`
- `NPSPEC-ALGORITHM-DEGRADATION-0001`
- `NPSPEC-ALGORITHM-FALLBACK-0001`
- `NPSPEC-PIPELINE-DEGRADATION-0001`
- `NPSPEC-PIPELINE-OPTIONALSTAGE-0001`
- `NPSPEC-PIPELINE-REPLANNING-0001`
- `NPSPEC-MEMORY-DEGRADATION-0001`
- `NPSPEC-GRAPHICS-DEGRADATION-0001`
- `NPSPEC-AUDIO-DEGRADATION-0001`
- `NPSPEC-VIDEO-DEGRADATION-0001`
- `NPSPEC-NETWORK-DEGRADATION-0001`
- `NPSPEC-STORAGE-DEGRADATION-0001`
- `NPSPEC-AI-DEGRADATION-0001`
- `NPSPEC-AI-MODEL-FALLBACK-0001`
- `NPSPEC-UI-DEGRADATION-0001`
- `NPSPEC-COMPOSITOR-DEGRADATION-0001`
- `NPSPEC-OBSERVABILITY-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-DEGRADATION-0001`
- `NPSPEC-RESILIENCE-BROWNOUT-0001`
- `NPSPEC-RESILIENCE-RESTORATION-0001`

---

# Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch Unit-, Integration-, Stress-, Fault-Injection-, Resource-Pressure-, Recovery-, Contract-, Determinism-, Security- und Long-Running-Tests überprüft werden.

## Basic Degradation Test

Ausgangszustand:

```text
Quality = High
```

Bei erlaubtem Resource Pressure muss NovaOS auf:

```text
Quality = Medium
```

wechseln können.

---

## Hard Minimum Test

Bei:

```text
minimum = Medium
```

darf NovaOS nicht automatisch auf:

```text
Low
```

wechseln.

---

## Forbidden Degradation Test

Bei:

```text
degradation = Forbidden
```

muss die Operation fehlschlagen, wenn der ursprüngliche Contract nicht mehr erfüllt werden kann.

---

## Security Test

Eine Degradation darf Security Requirements nicht abschwächen.

---

## Sovereignty Test

Bei:

```text
LocalOnly
```

darf kein Remote Fallback verwendet werden.

---

## Trust Test

Ein Provider unterhalb des Trust Minimum darf nicht als Degradation Fallback verwendet werden.

---

## Correctness Test

Ein schnellerer, aber semantisch inkorrekter Algorithmus darf nicht ausgewählt werden.

---

## Approximation Test

Approximate Algorithm darf nur verwendet werden, wenn Approximation explizit erlaubt ist.

---

## Precision Test

Numerische Präzision darf nicht unter die erlaubte Error Bound sinken.

---

## Resource Pressure Test

Getrennt zu testen sind:

```text
CPU
Memory
GPU
NPU
Storage
Network
Energy
Thermal
```

---

## Multi-Pressure Test

Mehrere gleichzeitig aktive Pressure Sources müssen korrekt kombiniert werden.

---

## Cross-Resource Test

GPU-to-CPU-Fallback darf keine unkontrollierte CPU-Überlast erzeugen.

---

## Hysteresis Test

Eine Ressource knapp um den Threshold darf kein permanentes Quality Flapping verursachen.

---

## Stability Window Test

Restoration darf erst nach erfülltem Stability Window erfolgen.

---

## Cooldown Test

Cooldown muss schnelle Wiederholungsübergänge verhindern.

---

## Memory Test

Unter Memory Pressure müssen rekonstruierbare Caches vor kritischen Daten freigegeben werden können.

---

## GPU Test

GPU Pressure muss definierte Graphics Quality Levels auslösen können.

---

## CPU Test

CPU Pressure muss Optional Tasks reduzieren können.

---

## NPU Test

NPU-Ausfall muss auf zulässigen GPU-, CPU- oder Non-AI-Fallback wechseln können.

---

## Network Test

Network Pressure muss Bitrate beziehungsweise Synchronization Frequency reduzieren können.

---

## Offline Test

Netzwerkausfall muss zulässige lokale Funktionen weiterarbeiten lassen.

---

## Freshness Test

Offline verwendete Daten müssen ihre Freshness korrekt deklarieren.

---

## Energy Test

Critical Battery muss einen zulässigen Energy-Saving Profile aktivieren können.

---

## Thermal Test

Thermal Pressure muss kontrollierte Degradation auslösen.

---

## Thermal Emergency Test

Wenn Degradation nicht genügt, muss Safety-bedingtes Stoppen möglich sein.

---

## Provider Fallback Test

Ausfall des Preferred Providers muss einen kompatiblen Fallback auswählen.

---

## Forced Provider Test

Ein Forced Provider darf nicht automatisch ersetzt werden.

---

## Algorithm Fallback Test

Preferred Algorithm darf auf kompatiblen Algorithmus wechseln.

---

## Forced Algorithm Test

Forced Algorithm darf nicht automatisch ersetzt werden.

---

## Structured Concurrency Test

Speculative Child Tasks müssen kontrolliert gecancelt werden können.

---

## Required Task Test

Required Tasks dürfen nicht vor Optional Tasks verworfen werden, sofern keine höhere Policy dies verlangt.

---

## Deadline Test

Deadline Risk muss auf eine schnellere zulässige Quality Stage wechseln können.

---

## Determinism Test

Strict Deterministic Mode muss reproduzierbare Degradation Decisions erzeugen.

---

## Adaptive Test

Adaptive Degradation darf ausschließlich innerhalb des erlaubten Contract-Raums arbeiten.

---

## AI Independence Test

Das Degradation-System muss bei vollständiger Nichtverfügbarkeit von AI Services weiterarbeiten.

---

## Transaction Test

Komplexe Degradation muss ohne inkonsistenten Zwischenzustand durchgeführt werden können.

---

## Rollback Test

Fehlgeschlagene neue Degradation Stage muss auf vorherigen sicheren Zustand zurückrollen können.

---

## Restoration Test

Nach Ende des Resource Pressure muss der Preferred State wiederherstellbar sein.

---

## Gradual Restoration Test

Restoration muss stufenweise erfolgen können.

---

## Restoration Failure Test

Fehlgeschlagene Restoration darf den aktuell funktionierenden Degraded State nicht unnötig zerstören.

---

## Hot Replacement Test

Provider Replacement muss bestehenden Degradation State korrekt übernehmen beziehungsweise neu bewerten.

---

## Self-Healing Test

Ein ausgefallener Provider muss:

```text
Fallback
    ↓
Repair
    ↓
Verify
    ↓
Restore
```

unterstützen können.

---

## Introspection Test

Architecture Introspection muss mindestens darstellen:

```text
Desired
Actual
Minimum
Reason
Since
RecoveryCondition
```

---

## Decision Trace Test

Der Degradation Planner muss erklären können, warum Alternativen ausgewählt oder verworfen wurden.

---

## Brownout Test

Brownout muss optionale Funktionen systemweit reduzieren können.

---

## Brownout Recovery Test

Nach Ende des Brownout muss kontrollierte Restoration erfolgen.

---

## Emergency Test

Emergency Mode muss Safety, Security, Data Integrity und Recovery priorisieren.

---

## Accessibility Test

Erforderliche Accessibility Features dürfen nicht als optionale Features deaktiviert werden.

---

## UI Degradation Test

Dekorative Effekte müssen reduzierbar sein, während Lesbarkeit erhalten bleibt.

---

## Audio Test

Audio Effects müssen reduziert werden können, bevor Echtzeit-Audio unnötig ausfällt.

---

## Video Test

Video muss entlang zulässiger Resolution-, FrameRate- und Quality-Dimensionen degradieren können.

---

## AI Model Test

Ein großes Modell muss auf ein kleineres Modell wechseln können, wenn der Contract dies erlaubt.

---

## AI Sovereignty Test

LocalOnly AI darf niemals auf Remote AI degradieren.

---

## Scientific Test

Scientific Workloads müssen Error Bounds einhalten.

---

## Medical Test

Medizinische Workloads dürfen keine nicht autorisierte Precision Degradation durchführen.

---

## Logging Test

Debug Logging muss unter Pressure reduzierbar sein.

Audit Logging muss entsprechend seiner eigenen Hard Requirements erhalten bleiben.

---

## Observability Test

Nicht kritische Telemetrie darf reduziert werden.

Kritische Health Information muss verfügbar bleiben.

---

## Cascade Test

Ein Fallback darf keine unkontrollierte systemweite Resource Cascade auslösen.

---

## Failure Domain Test

Lokaler Provider-Ausfall soll nur den kleinstmöglichen Scope degradieren.

---

## Degradation Exhaustion Test

Wenn keine gültige Stufe mehr existiert:

```text
DegradationExhausted
```

muss erzeugt werden.

---

## Fail-Safe Test

Nach Degradation Exhaustion muss ein definierter sicherer Fehlerpfad verwendet werden.

---

## Long-Running Test

Langzeitbetrieb darf nicht zu:

- Quality Flapping,
- Degradation State Leaks,
- Resource Reservation Leaks,
- stale Restoration Conditions,
- unkontrolliertem History Growth

führen.

---

## Stress Test

Zu testen sind:

- tausende parallele degradierbare Tasks,
- schnelle Resource Pressure Changes,
- mehrere Provider-Ausfälle,
- gleichzeitige Thermal- und Memory-Pressure,
- wiederholte Hot Replacements,
- häufige Offline-/Online-Wechsel,
- hohe Pipeline-Last,
- Brownout Transitions.

---

# Ergebnis

Mit `ADR-ARCH-0015` wird Graceful Degradation zu einem fundamentalen NovaOS-Systemprinzip.

NovaOS betrachtet eine Funktion nicht mehr ausschließlich als:

```text
Available
```

oder:

```text
Failed
```

sondern kann mehrere gültige Betriebszustände besitzen:

```text
Preferred
    ↓
Degraded
    ↓
Minimum Functional
    ↓
Failure
```

Die entscheidende Grenze bleibt dabei:

```text
Hard Contract
```

Graceful Degradation findet ausschließlich innerhalb der zulässigen Lösungsmenge statt.

Das Gesamtmodell lautet:

```text
Desired State
      ↓
Execution Contract
      ↓
Preferred Quality
      ↓
Resource Economy
      ↓
Runtime Pressure
      ↓
Degradation Planner
      ↓
Hard Constraint Validation
      ↓
Valid Alternatives
      ↓
Policy Selection
      ↓
Transactional Transition
      ↓
Degraded Actual State
      ↓
Architecture Introspection
      ↓
Resource Recovery
      ↓
Reconciliation
      ↓
Progressive Restoration
      ↓
Desired State
```

Damit werden die bisherigen NovaOS-Architekturprinzipien miteinander verbunden:

```text
Declarative System Model
        │
        ▼
Explicit Contracts
        │
        ▼
Execution Contracts
        │
        ▼
Resource Economy
        │
        ▼
Graceful Degradation
        │
        ├── Provider Fallback
        ├── Algorithm Fallback
        ├── Quality Reduction
        ├── Feature Shedding
        ├── Task Shedding
        ├── Offline Mode
        ├── Brownout Mode
        └── Minimum Functional Mode
        │
        ▼
Structured Concurrency
        │
        ▼
Transactional Transition
        │
        ▼
Architecture Introspection
        │
        ▼
Self-Healing
        │
        ▼
Progressive Restoration
```

Das abschließende Architekturprinzip lautet:

```text
Preserve correctness before quality.

Preserve required functionality
before optional functionality.

Preserve the system
before preserving enhancements.
```

und:

```text
Degrade gracefully.

Never degrade blindly.
```

sowie:

```text
Preferred when possible.

Reduced when necessary.

Minimum when required.

Fail only when no valid state remains.
```