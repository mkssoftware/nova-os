# NPSPEC-DATAFLOW-PORT-0001: Eingabe- und Ausgabeports (Teil 1)

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Dokument-ID:** NPSPEC-DATAFLOW-PORT-0001
- **Titel:** Eingabe- und Ausgabeports
- **Version:** 1.0.0
- **Status:** Verbindlich
- **Kategorie:** Dataflow Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform

## Referenzen

- NPSPEC-DATAFLOW-0001
- NPSPEC-DATATYPE-0001
- NPSPEC-DATATYPE-SCHEMA-0001
- NPSPEC-DATATYPE-COMPATIBILITY-0001
- NPSPEC-DATATYPE-CONVERSION-0001
- NPSPEC-DATAFLOW-NODE-0001
- NPSPEC-CAPABILITY-CONTRACT-0001
- NPSPEC-CAPABILITY-DISCOVERY-0001
- ADR-DATAFLOW-0001
- ADR-DATAFLOW-0002

---

# 1. Zweck

Diese Spezifikation definiert das universelle Portmodell der Nova Platform.

Ports bilden die einzige standardisierte Schnittstelle zwischen Dataflow-Knoten. Jeder Datenaustausch erfolgt ausschließlich über deklarierte Ports.

Ein Port beschreibt:

- Richtung
- Datentyp
- Kommunikationsmodell
- Lebenszyklus
- Qualitätsanforderungen
- Sicherheitsrichtlinien
- Versionierung
- Verbindungsregeln

---

# 2. Ziele

Das Portmodell verfolgt folgende Ziele:

- vollständig typisierte Schnittstellen
- deterministische Kommunikation
- lose Kopplung
- Wiederverwendbarkeit
- Compilerprüfbarkeit
- Runtime-Optimierung
- automatische Discovery
- Reflection
- Versionierung
- hohe Performance
- Zero-Copy-Unterstützung
- Netzwerktransparenz

---

# 3. Grundprinzipien

Das Nova-Portmodell basiert auf folgenden Regeln:

- Jeder Datenaustausch erfolgt ausschließlich über Ports.
- Jeder Port besitzt genau eine Richtung.
- Jeder Port besitzt einen Datentyp.
- Jeder Port besitzt einen eindeutigen Namen.
- Jeder Port besitzt eine stabile Identität.
- Ports besitzen keine versteckten Nebenwirkungen.
- Ports sind deklarativ beschrieben.
- Ports dürfen unabhängig vom Ausführungsort verwendet werden.
- Ports sind vollständig reflektierbar.

---

# 4. Architektur

```text
Node

├── Input Ports

├── Output Ports

└── Metadata

        │

        ▼

Port Connection

        │

        ▼

Data Channel

        │

        ▼

Receiving Node
```

---

# 5. Definition

Ein Port ist eine deklarierte Ein- oder Ausgabeschnittstelle eines Dataflow-Knotens.

Ein Port definiert:

- Richtung
- Datentyp
- Schema
- Version
- Eigenschaften
- Kommunikationsregeln

---

# 6. Portidentität

Jeder Port besitzt eine globale Identität.

Sie besteht aus:

```text
Node ID

Port ID

Version
```

Beispiel:

```text
nova.compute.image-loader.output.image
```

---

# 7. Portname

Der Portname dient der Lesbarkeit.

Beispiele:

```text
image

input

result

audio

command

temperature
```

Der Name ist unabhängig von der internen Port-ID.

---

# 8. Port-ID

Jeder Port besitzt eine unveränderliche Port-ID.

Eigenschaften:

- eindeutig
- stabil
- unveränderlich
- versioniert
- maschinenlesbar

Die Port-ID darf nach Veröffentlichung nicht geändert werden.

---

# 9. Portbeschreibung

Ein Port besitzt mindestens folgende Eigenschaften:

```text
Port ID

Name

Direction

Type

Schema

Version
```

Optional:

```text
Description

Default Value

Constraints

QoS

Security

Priority

Traits
```

---

# 10. Porttypen

Nova definiert:

```text
Input Port

Output Port

InOut Port

Virtual Port

Dynamic Port
```

---

# 11. Input Port

Ein Input Port empfängt Daten.

```text
↓

Input
```

Ein Input Port erzeugt selbst keine Daten.

---

# 12. Output Port

Ein Output Port erzeugt Daten.

```text
Output

↓

```

Output Ports empfangen keine Daten.

---

# 13. InOut Port

Ein InOut Port unterstützt beide Richtungen.

Beispiel:

```text
Client

⇄

Server
```

InOut Ports werden nur verwendet, wenn beide Kommunikationsrichtungen logisch zusammengehören.

---

# 14. Virtuelle Ports

Virtuelle Ports repräsentieren keine physische Verbindung.

Beispiele:

- Debug-Ausgabe
- Diagnosedaten
- Telemetrie
- Simulation

---

# 15. Dynamische Ports

Dynamische Ports entstehen zur Laufzeit.

Beispiele:

- Plugin-Port
- Benutzerdefinierter Port
- Laufzeitgenerierte Pipeline

Sie müssen vollständig beschrieben werden.

---

# 16. Portzustände

Ein Port besitzt folgende Zustände:

```text
Created

Initialized

Connected

Active

Paused

Closing

Closed

Error
```

---

# 17. Lebenszyklus

```text
Create

↓

Initialize

↓

Connect

↓

Activate

↓

Transfer

↓

Disconnect

↓

Destroy
```

Ein Port darf nur in gültigen Zustandsübergängen betrieben werden.

---

# 18. Portinitialisierung

Während der Initialisierung werden geprüft:

- Datentyp
- Schema
- Version
- Eigenschaften
- Berechtigungen
- Ressourcen

---

# 19. Aktivierung

Ein Port darf erst aktiviert werden, wenn:

- Initialisierung erfolgreich war
- Typprüfung erfolgreich war
- Verbindung gültig ist
- Sicherheitsprüfung bestanden wurde

---

# 20. Deaktivierung

Ein Port kann deaktiviert werden durch:

- Benutzer
- Runtime
- Fehler
- Ressourcenmangel
- Scheduler
- Sicherheitsrichtlinie

---

# 21. Portklassifikation

Ports können klassifiziert werden als:

```text
Data

Command

Control

Event

Message

Stream

Configuration
```

---

# 22. Datenport

Ein Datenport transportiert Werte.

Beispiel:

```text
Image

Number

Matrix

Document
```

---

# 23. Command-Port

Command-Ports übertragen Anweisungen.

Beispiele:

```text
Start

Stop

Pause

Reset

Shutdown
```

---

# 24. Event-Port

Event-Ports übertragen Ereignisse.

Beispiele:

```text
ButtonClicked

Connected

Timeout

SensorTriggered
```

---

# 25. Message-Port

Message-Ports transportieren vollständige Nachrichten.

Beispiele:

```text
ChatMessage

HTTPRequest

RPCMessage
```

---

# 26. Stream-Port

Ein Stream-Port transportiert kontinuierliche Daten.

Beispiele:

```text
Video

Audio

Telemetry

Measurement
```

---

# 27. Konfigurationsport

Ein Konfigurationsport überträgt Konfigurationsdaten.

Beispiele:

```text
Settings

Calibration

Theme

Profile
```

---

# 28. Datentypbindung

Jeder Port besitzt genau einen deklarierten Datentyp.

Beispiel:

```text
Image

Tensor

Markdown

Person

AudioFrame
```

---

# 29. Schemabindung

Zusätzlich besitzt jeder Port ein Schema.

Dadurch wird sichergestellt:

- Typprüfung
- Versionsprüfung
- Constraintprüfung
- Reflection

---

# 30. Versionierung

Ports besitzen:

```text
Major

Minor

Patch
```

Versionen beeinflussen die Kompatibilität.

---

# 31. Portkompatibilität

Vor jeder Verbindung wird geprüft:

- Richtung
- Datentyp
- Schema
- Version
- Sicherheitsrichtlinien

---

# 32. Richtungsprüfung

Nicht zulässig:

```text
Output

↓

Output
```

Nicht zulässig:

```text
Input

↓

Input
```

Zulässig:

```text
Output

↓

Input
```

---

# 33. Typprüfung

Die Runtime prüft:

```text
Output Type

↓

Input Type
```

Es gelten die Regeln aus:

NPSPEC-DATATYPE-COMPATIBILITY-0001

---

# 34. Schemakompatibilität

Zusätzlich werden geprüft:

- Schema-ID
- Schema-Version
- Constraints

---

# 35. Automatische Adapter

Sind zwei Ports nicht direkt kompatibel, darf die Runtime automatisch Adapter einsetzen.

Voraussetzungen:

- verlustfrei
- eindeutig
- registriert
- vertrauenswürdig

---

# 36. Converter

Ist kein Adapter vorhanden, kann ein Converter verwendet werden.

Der Converter muss im Capability-Registry registriert sein.

---

# 37. Portgruppen

Ports können logisch gruppiert werden.

Beispiel:

```text
Image

├── Color

├── Alpha

└── Metadata
```

---

# 38. Mehrfachports

Ein Port kann mehrere Verbindungen besitzen.

```text
Output
├── Input A
├── Input B
└── Input C
```

---

# 39. Broadcast

Broadcast verteilt Daten gleichzeitig an mehrere Ziele.

Die Runtime garantiert:

- identische Reihenfolge
- identischen Inhalt
- identische Zeitbasis (soweit möglich)

---

# 40. Sammelports

Mehrere Eingänge können zu einem Sammelport zusammengeführt werden.

Beispiel:

```text
Sensor A

Sensor B

Sensor C

↓

Sensor Collection
```

---

# 41. Portkapazität

Ports können Kapazitätsgrenzen definieren.

Beispiele:

```text
Queue Length

Maximum Buffer

Maximum Connections
```

---

# 42. Puffer

Ports dürfen interne Puffer besitzen.

Unterstützt werden:

```text
None

Single

Ring Buffer

FIFO

Priority Queue
```

---

# 43. Backpressure

Stream-Ports unterstützen Backpressure.

Strategien:

```text
Block

Drop

Latest

Throttle

Buffer
```

---

# 44. Prioritäten

Ports besitzen optionale Prioritäten.

Beispiele:

```text
Critical

High

Normal

Low

Background
```

---

# 45. Portattribute

Ein Port kann deklarieren:

```text
Realtime

Thread Safe

Encrypted

Persistent

Observable

Buffered
```

---

# 46. QoS

Ports unterstützen Quality-of-Service.

Beispiele:

```text
Reliable

Best Effort

Ordered

Low Latency

Lossless
```

---

# 47. Zusammenfassung Teil 1

Dieser erste Teil definiert das grundlegende Portmodell der Nova Platform: Portidentität, Richtungen, Lebenszyklus, Typbindung, Versionierung, Verbindungsregeln sowie die wichtigsten Portarten und deren Eigenschaften.

**Fortsetzung in Teil 2:** Verbindungsmanagement, Scheduler, Runtime, Discovery, Reflection, Sicherheit, Netzwerktrennung, YAML-Beispiele, Compilerregeln, Tests und Konformitätskriterien.


---

# 48. Verbindungsmodell

Eine Verbindung (Connection) beschreibt die logische Beziehung zwischen genau einem Output-Port und mindestens einem Input-Port.

```text
Output Port

↓

Connection

↓

Input Port
```

Eine Verbindung besitzt selbst Metadaten und eine eindeutige Identität.

---

# 49. Connection-ID

Jede Verbindung besitzt:

```text
Connection ID

Source Port

Target Port

Version

State
```

Die Connection-ID ist während ihrer gesamten Lebensdauer unveränderlich.

---

# 50. Verbindungstypen

Nova definiert folgende Verbindungstypen:

```text
Point-to-Point

Broadcast

Multicast

Pipeline

Loopback

Remote
```

---

# 51. Point-to-Point

Eine Quelle besitzt genau einen Empfänger.

```text
Output

↓

Input
```

Dies ist der Standardfall.

---

# 52. Broadcast

Ein Sender verteilt identische Daten gleichzeitig an mehrere Empfänger.

```text
Output

├── Input A

├── Input B

└── Input C
```

Alle Empfänger erhalten dieselbe Nachricht.

---

# 53. Multicast

Ein Sender verteilt Daten an eine definierte Teilmenge aller Empfänger.

Die Empfängerliste kann statisch oder dynamisch sein.

---

# 54. Pipeline

Eine Pipeline verbindet mehrere Verarbeitungsschritte.

```text
Input

↓

Filter

↓

Transformation

↓

Analyse

↓

Output
```

---

# 55. Loopback

Ein Output-Port darf mit einem eigenen Input-Port verbunden werden.

Loopbacks müssen explizit erlaubt werden.

---

# 56. Remote-Verbindungen

Ports dürfen sich auf unterschiedlichen Rechnern befinden.

Die Verbindung bleibt logisch identisch.

Die Runtime übernimmt:

- Serialisierung
- Transport
- Synchronisation
- Wiederherstellung

---

# 57. Port Discovery

Alle Ports sind über Reflection auffindbar.

Discovery unterstützt:

```text
Name

Port-ID

Datentyp

Capability

Node

Tags

Version
```

---

# 58. Reflection

Reflection liefert sämtliche Eigenschaften eines Ports.

Beispiel:

```text
Direction

Type

QoS

Security

Schema

State

Connections
```

Reflection darf keine Portzustände verändern.

---

# 59. Port Registry

Alle veröffentlichten Ports werden in einer zentralen Registry registriert.

Sie dient:

- Discovery
- Versionierung
- Reflection
- Dokumentation

---

# 60. Port Namespace

Jeder Port gehört zu einem Namespace.

Beispiel:

```text
nova.ai.image.input

nova.robot.sensor.temperature

nova.compute.audio.output
```

Namespaces verhindern Namenskollisionen.

---

# 61. Port Tags

Ports können beliebige Tags besitzen.

Beispiele:

```text
Realtime

AI

Vision

GPU

Experimental

Network
```

---

# 62. Verbindungsaufbau

Vor dem Aufbau prüft die Runtime:

- Richtung
- Typ
- Schema
- Version
- Berechtigungen
- Policies
- Ressourcen

Erst danach wird die Verbindung aktiviert.

---

# 63. Verbindungsabbau

Eine Verbindung wird beendet durch:

- Benutzer
- Runtime
- Fehler
- Timeout
- Policy
- Node-Ende

Alle Ressourcen sind vollständig freizugeben.

---

# 64. Port Policies

Jeder Port kann eigene Richtlinien definieren.

Beispiele:

```text
Nur lokal

Nur verschlüsselt

Nur signierte Daten

Nur Echtzeit

Nur vertrauenswürdige Quellen
```

---

# 65. Sicherheitsmodell

Ports besitzen optionale Sicherheitsstufen.

```text
Public

Protected

Internal

Confidential

Secret
```

Die Runtime muss jede Verbindung gegen diese Einstufung prüfen.

---

# 66. Authentifizierung

Ein Port kann eine Authentifizierung verlangen.

Unterstützt werden unter anderem:

- Zertifikate
- Capability-Tokens
- Signaturen
- lokale Vertrauensanker

---

# 67. Autorisierung

Auch nach erfolgreicher Authentifizierung muss geprüft werden, ob der Kommunikationspartner auf den Port zugreifen darf.

---

# 68. Verschlüsselung

Ports können deklarieren:

```text
Encryption Required
```

Die Runtime entscheidet selbstständig über den konkreten Transportmechanismus.

---

# 69. Signaturen

Ein Port kann digitale Signaturen verlangen.

Dies schützt vor:

- Manipulation
- Replay-Angriffen
- unautorisierten Daten

---

# 70. Datenintegrität

Alle übertragenen Daten können Integritätsinformationen enthalten.

Beispiele:

- Prüfsummen
- Hashes
- Signaturen

---

# 71. Scheduler

Der Scheduler entscheidet:

- Ausführungsreihenfolge
- Priorität
- Thread-Zuordnung
- CPU-Zuordnung

Ports beeinflussen diese Entscheidungen durch ihre Eigenschaften.

---

# 72. Echtzeitports

Realtime-Ports besitzen zusätzliche Garantien:

- deterministische Latenz
- feste Zeitfenster
- priorisierte Verarbeitung
- keine unkontrollierten Blockierungen

---

# 73. Synchronisation

Ports können arbeiten als:

```text
Synchron

Asynchron

Eventbasiert

Zeitgesteuert
```

---

# 74. Zeitmodell

Ein Port kann Zeitinformationen übertragen.

Beispiele:

```text
Timestamp

Frame Number

Sequence Number

Logical Clock
```

---

# 75. Reihenfolge

Ein Port kann garantieren:

```text
Ordered Delivery
```

oder

```text
Unordered Delivery
```

---

# 76. Duplikate

Ein Port kann deklarieren:

```text
Duplicate Free
```

Die Runtime muss doppelte Nachrichten erkennen und entsprechend behandeln.

---

# 77. Fehlertoleranz

Ports unterstützen:

- Wiederholung
- automatische Neuverbindung
- alternative Verbindungen
- Fallback-Nodes

---

# 78. Timeout

Ports können maximale Wartezeiten definieren.

Nach Ablauf entscheidet die Runtime gemäß der konfigurierten Policy.

---

# 79. Heartbeat

Lang laufende Verbindungen können Heartbeats verwenden.

Sie dienen zur Erkennung ausgefallener Kommunikationspartner.

---

# 80. Monitoring

Jeder Port kann Laufzeitstatistiken liefern.

Beispiele:

- Nachrichtenanzahl
- Datenmenge
- Fehlerrate
- Latenz
- Auslastung

---

# 81. Telemetrie

Ports können Telemetriedaten veröffentlichen.

Diese sind logisch von den eigentlichen Nutzdaten getrennt.

---

# 82. Logging

Ports dürfen Ereignisse protokollieren.

Mindestens:

- Verbindung aufgebaut
- Verbindung getrennt
- Fehler
- Policy-Verletzungen

---

# 83. Debugging

Debug-Ports dürfen ausschließlich Diagnoseinformationen transportieren.

Sie dürfen keine produktiven Daten verändern.

---

# 84. Simulation

Ein Port kann als Simulationsquelle markiert werden.

Die Runtime kennzeichnet diese Daten eindeutig.

---

# 85. Replay

Ports können aufgezeichnete Daten erneut abspielen.

Replay muss dieselbe Reihenfolge und dieselben Zeitinformationen bereitstellen.

---

# 86. Persistenz

Ports können deklarieren:

```text
Persistent
```

Dann dürfen Nachrichten zwischengespeichert werden.

---

# 87. Zero-Copy

Ein Port kann Zero-Copy unterstützen.

Die Runtime vermeidet unnötige Speicherkopien, sofern Typ und Plattform dies zulassen.

---

# 88. Shared Memory

Lokale Ports können Shared Memory verwenden.

Die Implementierung bleibt vollständig transparent.

---

# 89. Netzwerktransparenz

Für den Benutzer macht es keinen Unterschied, ob zwei Nodes lokal oder verteilt ausgeführt werden.

---

# 90. Portmigration

Ein Port darf zur Laufzeit auf einen anderen Node verschoben werden.

Bestehende Verbindungen sollen nach Möglichkeit erhalten bleiben.

---

# 91. YAML-Beispiel

```yaml
port:

  id: nova.image.output

  direction: output

  datatype: Image

  schema: ImageV2

  qos: Reliable

  priority: High
```

---

# 92. YAML-Verbindung

```yaml
connection:

  source: camera.output

  target: detector.input

  mode: point-to-point

  state: active
```

---

# 93. YAML-Broadcast

```yaml
broadcast:

  source: sensor.output

  targets:

    - logger.input

    - ui.input

    - recorder.input
```

---

# 94. YAML-Realtime-Port

```yaml
port:

  realtime: true

  latency: low

  ordered: true

  buffered: false
```

---

# 95. Compileranforderungen

Der Compiler muss prüfen:

- Richtungen
- Datentypen
- Versionen
- Policies
- QoS
- Reflection-Informationen

Ungültige Verbindungen sind Compile-Fehler, sofern sie statisch erkennbar sind.

---

# 96. Runtime-Anforderungen

Die Runtime muss:

- Discovery unterstützen
- Reflection bereitstellen
- Adapter auswählen
- Converter verwenden
- Policies durchsetzen
- Fehler erkennen
- Ressourcen freigeben

---

# 97. Testanforderungen

Jeder Port benötigt Tests für:

- Typprüfung
- Richtungsprüfung
- Discovery
- Reflection
- Versionierung
- Performance
- Sicherheit
- Fehlerszenarien

---

# 98. Performanceanforderungen

Das Portsystem muss:

- skalierbar
- nebenläufig
- deterministisch
- cachefähig
- Zero-Copy-fähig

sein.

---

# 99. Konformitätsstufen

Nova definiert:

```text
Core

Standard

Complete
```

---

# 100. Core

Unterstützt:

- Input-Port
- Output-Port
- Typprüfung
- Reflection
- Discovery

---

# 101. Standard

Zusätzlich:

- Broadcast
- Streams
- QoS
- Versionierung
- Policies
- Adapter

---

# 102. Complete

Zusätzlich:

- Remote-Verbindungen
- Zero-Copy
- Portmigration
- automatische Discovery
- Capability-Integration
- vollständige Telemetrie

---

# 103. Konformitätskriterien

Eine Implementierung erfüllt diese Spezifikation, wenn:

- sämtliche Ports eindeutig identifizierbar sind,
- alle Verbindungen typgeprüft werden,
- Richtungen eingehalten werden,
- Reflection vollständig verfügbar ist,
- Discovery alle veröffentlichten Ports findet,
- Versionen berücksichtigt werden,
- Policies durchgesetzt werden,
- Runtime und Compiler dieselben Kompatibilitätsregeln anwenden,
- Fehler deterministisch behandelt werden.

---

# 104. Zusammenfassung

Das Portmodell bildet die universelle Kommunikationsschnittstelle der Nova Platform.

Durch streng typisierte Ein- und Ausgabeports, eindeutige Identitäten, deklarative Eigenschaften, Reflection, Discovery sowie integrierte Sicherheits-, Qualitäts- und Laufzeitmechanismen entsteht ein einheitliches Kommunikationsmodell für lokale Anwendungen, verteilte Systeme, KI-Pipelines, Robotik, Echtzeitsysteme und zukünftige Nova-Komponenten.

Ports stellen damit den einzigen standardisierten Datenaustauschmechanismus innerhalb der gesamten Nova Platform dar und bilden gemeinsam mit Nodes, Channels und dem Capability-System das Fundament der Nova-Dataflow-Architektur.