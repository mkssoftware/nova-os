# ADR-CAPABILITY-0015 – Legacy Provider als Capability Bridge

## Status

Angenommen

## Kategorie

Capability Architecture / Compatibility / Legacy Integration

## Kontext

NovaOS soll bestehende Software, Bibliotheken, Frameworks, Dienste und Legacy-Systemkomponenten weiterverwenden können, ohne deren Architektur zum nativen Systemmodell zu machen.

Legacy-Komponenten kennen das NovaOS-Capability-Modell jedoch nicht und stellen ihre Funktionen beispielsweise über:

- klassische Bibliotheks-APIs,
- POSIX-Schnittstellen,
- Windows-APIs,
- Prozesse und Dienste,
- bestehende Treiberinterfaces,
- Kommandozeilenprogramme,
- Netzwerkprotokolle

bereit.

NovaOS benötigt deshalb eine kontrollierte Brücke zwischen Legacy-Funktionalität und dem nativen Capability-System.

---

## Entscheidung

NovaOS führt **Legacy Provider als Capability Bridges** ein.

Eine Bridge adaptiert eine bestehende Legacy-Funktion auf einen nativen Capability Contract.

```text
NovaOS Consumer
      ↓
Capability Contract
      ↓
Legacy Capability Bridge
      ↓
Legacy Interface
      ↓
Legacy Provider
```

Für den Consumer erscheint die Funktion als reguläre Capability.

Der Legacy Provider selbst muss das NovaOS-Capability-Modell nicht implementieren.

---

## Grundprinzip

```text
Adapt legacy functionality into NovaOS.

Do not adapt NovaOS around legacy architecture.
```

Legacy-Kompatibilität bleibt damit eine kontrollierte Randzone der Architektur.

---

## Capability Bridge

Eine Bridge übersetzt zwischen:

```text
NovaOS Capability Semantics
            ↕
Legacy Interface Semantics
```

Sie kann insbesondere folgende Aufgaben übernehmen:

- Contract Mapping,
- Typkonvertierung,
- Semantic-Type-Mapping,
- Fehlerübersetzung,
- Lifecycle-Anpassung,
- Authority Mapping,
- Datenformat-Konvertierung.

Die Bridge muss Unterschiede explizit behandeln und darf inkompatible Semantik nicht als identisch darstellen.

---

## Capability Identity

Eine Legacy-Implementierung erhält keine eigene semantische Identität allein aufgrund ihrer Herkunft.

Wenn sie einen bestehenden Capability Contract korrekt erfüllt:

```text
Capability X
   │
   ├── Native Provider
   └── Legacy Bridge Provider
```

können beide dieselbe Capability bereitstellen.

Ist die Legacy-Semantik nicht kompatibel, muss eine getrennte Capability oder ein explizit eingeschränkter Contract verwendet werden.

---

## Provider Resolution

Legacy Bridges nehmen grundsätzlich an derselben Provider Resolution wie native Provider teil.

```text
Capability Requirement
        ↓
Native Provider
Legacy Bridge Provider
Hardware Provider
Remote Provider
        ↓
Resolution
```

NovaOS darf native Provider bevorzugen, wenn dies durch Policy oder Optimierungsziele sinnvoll ist.

Ein Legacy Provider darf jedoch nicht allein wegen seiner Herkunft ausgeschlossen werden, wenn er alle geltenden Anforderungen erfüllt.

---

## Security Boundary

Legacy-Komponenten dürfen native Sicherheitsmechanismen nicht umgehen.

```text
Capability
    ↓
Bridge
    ↓
Compatibility / Isolation Domain
    ↓
Legacy Provider
```

Die Bridge muss Legacy-Berechtigungen auf die effektive NovaOS-Authority begrenzen.

Legacy-Konzepte wie:

```text
root
Administrator
UID/GID
ACL
Process Token
```

dürfen keine zusätzliche native NovaOS-Authority erzeugen.

---

## Sandboxing

Legacy Provider sollen bevorzugt innerhalb geeigneter Compatibility- oder Sandbox-Domains ausgeführt werden.

Die Bridge bildet dabei die kontrollierte Grenze zwischen nativer Capability-Ausführung und Legacy-Umgebung.

Ein Fehler im Legacy Provider soll möglichst innerhalb dieser Domain verbleiben.

---

## Datenübergabe

Legacy Provider können andere Datenrepräsentationen benötigen als native NovaOS-Capabilities.

```text
Nova Object
    ↓
Bridge
    ↓
Legacy Representation
    ↓
Legacy Provider
```

Konvertierungen müssen explizit sein.

Zero-Copy oder Shared Buffers dürfen verwendet werden, wenn die Legacy-Schnittstelle dies sicher unterstützt.

Copy Avoidance darf jedoch keine Isolation oder Typsemantik verletzen.

---

## Execution Contract

Eine Legacy Bridge muss offenlegen, welche Teile eines `Nova.ExecutionContract` sie tatsächlich erfüllen kann.

Beispielsweise kann ein Legacy Provider möglicherweise keine Garantien für:

```text
Determinism
Hard Deadline
Zero-Copy
Data Sovereignty
State Migration
```

geben.

Solche Einschränkungen müssen bei Negotiation und Resolution berücksichtigt werden.

---

## Legacy Driver

Bestehende Legacy-Treiber dürfen nicht direkt in den nativen NovaOS-Kernel geladen werden.

Wenn ihre Nutzung unterstützt wird, erfolgt sie über die definierte Legacy Driver Compatibility Domain.

Darauf aufbauende Hardwarefunktionen können über Capability Bridges in das Capability-System projiziert werden.

```text
Legacy Driver
      ↓
Legacy Driver Compatibility Domain
      ↓
Capability Bridge
      ↓
NovaOS Capability
```

---

## Failure Mapping

Legacy-Fehler müssen auf definierte Capability-Fehler abgebildet werden.

```text
Legacy Error
     ↓
Bridge Translation
     ↓
Capability Result
```

Unbekannte oder nicht eindeutig übersetzbare Fehler dürfen nicht als Erfolg interpretiert werden.

---

## Austauschbarkeit

Eine Legacy Bridge ist aus Sicht der Capability Architecture ein Provider.

Dadurch kann sie durch einen nativen Provider ersetzt werden:

```text
Legacy Provider
      ↓
Capability Contract
      ↓
Native Provider
```

Consumer müssen dafür nicht verändert werden, sofern der Capability Contract kompatibel bleibt.

Dies ermöglicht eine schrittweise Ablösung von Legacy-Komponenten.

---

## Introspection

NovaOS muss sichtbar machen können, wenn eine Capability über eine Legacy Bridge bereitgestellt wird.

Beispielsweise:

```text
Capability
Provider
Provider Type: Legacy Bridge
Legacy Technology
Compatibility Domain
Contract Limitations
Isolation State
```

Legacy-Abhängigkeiten dürfen nicht hinter der Capability-Abstraktion unsichtbar werden.

---

## Normative Anforderungen

1. NovaOS MUSS Legacy-Funktionalität über Capability Bridges integrieren können.
2. Legacy Provider DÜRFEN das native Capability-Modell NICHT zur Voraussetzung haben.
3. Bridges MÜSSEN Legacy-Schnittstellen auf explizite Capability Contracts abbilden.
4. Semantische Unterschiede MÜSSEN explizit behandelt werden.
5. Legacy Provider DÜRFEN keine zusätzliche native Authority erzeugen.
6. Legacy-Berechtigungsmodelle MÜSSEN auf das NovaOS-Sicherheitsmodell begrenzt werden.
7. Legacy Provider SOLLEN in geeigneten Compatibility- oder Sandbox-Domains isoliert werden.
8. Legacy Bridges MÜSSEN ihre Execution-Contract-Einschränkungen deklarieren.
9. Provider Resolution MUSS Legacy Bridge Provider berücksichtigen können.
10. Hard Constraints DÜRFEN für Legacy Provider NICHT abgeschwächt werden.
11. Legacy-Fehler MÜSSEN kontrolliert auf Capability-Fehler abgebildet werden.
12. Legacy Driver DÜRFEN NICHT direkt in den nativen NovaOS-Kernel geladen werden.
13. Legacy Bridges MÜSSEN durch kompatible native Provider austauschbar sein können.
14. Die Verwendung einer Legacy Bridge MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- bestehende Software und Bibliotheken können weiterverwendet werden,
- Legacy-Komponenten kontaminieren nicht die native Architektur,
- Migration kann schrittweise erfolgen,
- Legacy- und native Provider können denselben Capability Contract bedienen,
- Anwendungen bleiben von der Legacy-Technologie entkoppelt.

### Negative Konsequenzen

- Bridges erzeugen zusätzlichen Integrationsaufwand,
- Daten- und Fehlerkonvertierung kann Performance kosten,
- Legacy Provider können weniger Execution-Contract-Eigenschaften unterstützen,
- vollständige semantische Abbildung ist nicht für jede Legacy-Schnittstelle möglich.

---

## Verworfene Alternativen

### Legacy-APIs zu nativen NovaOS-APIs machen

Verworfen.

Damit würden historische Architekturentscheidungen dauerhaft Bestandteil des nativen Systemmodells.

### Legacy-Komponenten ohne Bridge direkt verwenden

Verworfen.

Dies würde Capability Contracts, Security und Introspection umgehen.

### Legacy Provider grundsätzlich verbieten

Verworfen.

Dies würde Wiederverwendung bestehender Software unnötig erschweren.

### Legacy Provider als vollständig gleichwertig annehmen

Verworfen.

Kompatibilität muss anhand des tatsächlichen Contracts geprüft werden.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0006_Capability_Negotiation`
- `ADR-CAPABILITY-0010_Capability_Sandboxing`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-DRIVER-0007_Legacy_Driver_nicht_direkt_in_den_Nova_Kernel_laden`
- `ADR-DRIVER-0008_Legacy_Driver_Compatibility_Domain`
- `ADR-SECURITY-0004_DAC_als_Kompatibilitätsmodell`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-LEGACY-BRIDGE-0001`
- `NPSPEC-CAPABILITY-LEGACY-CONTRACT-MAPPING-0001`
- `NPSPEC-CAPABILITY-LEGACY-TYPE-MAPPING-0001`
- `NPSPEC-CAPABILITY-LEGACY-AUTHORITY-MAPPING-0001`
- `NPSPEC-CAPABILITY-LEGACY-ERROR-MAPPING-0001`
- `NPSPEC-CAPABILITY-LEGACY-DRIVER-BRIDGE-0001`
- `NPSPEC-CAPABILITY-LEGACY-BRIDGE-TEST-0001`

---

## Ergebnis

NovaOS integriert bestehende Technologien über kontrollierte Capability Bridges:

```text
Legacy Technology
       ↓
Compatibility Domain
       ↓
Capability Bridge
       ↓
Native Capability Contract
       ↓
NovaOS Consumer
```

Damit kann NovaOS vorhandene Software und Hardwareunterstützung nutzen, ohne Legacy-Architekturen zum Fundament des neuen Systems zu machen.

Langfristig können Legacy Provider schrittweise durch native Provider ersetzt werden, während der Capability Contract für Consumer stabil bleibt.