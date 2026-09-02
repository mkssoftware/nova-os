# ADR-DISTCOMM-0001 – Nova.Call als einheitliche Distributed-Call-Abstraktion

## Status

Angenommen

## Kategorie

Distributed Communication / IPC / RPC / Location Transparency / Nova.Call

## Kontext

NovaOS trennt logische Kommunikation von ihrem konkreten Ausführungsort. Ein Dienst kann innerhalb derselben Execution Domain, in einer anderen lokalen Domain oder auf einem entfernten System bereitgestellt werden.

Klassische Architekturen verwenden dafür häufig unterschiedliche Programmiermodelle für Funktionsaufrufe, IPC und RPC. Dadurch entstehen unnötig getrennte APIs und Kommunikationsmodelle.

NovaOS benötigt deshalb eine einheitliche Call-Abstraktion, ohne dabei die unterschiedlichen Kosten, Fehlerfälle und Sicherheitsgrenzen lokaler und verteilter Kommunikation zu verbergen.

---

## Entscheidung

NovaOS führt **`Nova.Call` als einheitliche Distributed-Call-Abstraktion** ein.

```text
Caller
  ↓
Nova.Call
  ↓
Call Contract
  ↓
Target Resolution
  ↓
Local / IPC / Remote Provider
  ↓
Callee
```

Der Aufrufer beschreibt Ziel, Operation und erforderliche Eigenschaften.

Die konkrete Ausführung kann anschließend über einen lokalen Aufruf, IPC oder Netzwerkkommunikation erfolgen.

---

## Grundprinzip

```text
One call model.
Multiple execution locations.
Explicit distributed semantics.
```

`Nova.Call` vereinheitlicht die Kommunikationsabstraktion, nicht die physikalischen Eigenschaften der Kommunikation.

---

## Call Contract

Jeder `Nova.Call` basiert auf einem expliziten Contract.

Dieser kann insbesondere enthalten:

- Target Identity,
- Operation Identity,
- Semantic Types,
- Authority Requirements,
- Trust Requirements,
- Deadline,
- Cancellation,
- Determinism Requirement,
- Resource Budget,
- Data-Sovereignty Policy.

Die konkrete Contract-Struktur wird in den zugehörigen NPSPECs definiert.

---

## Einheitliche Zieladressierung

Ein Call richtet sich primär an eine logische Service-, Capability- oder Object Identity und nicht direkt an einen Prozess, Port oder Netzwerkendpunkt.

```text
Logical Target
      ↓
Resolution
      ↓
Local Object
IPC Endpoint
Remote Service
```

Damit bleibt die logische Schnittstelle vom aktuellen Ausführungsort entkoppelt.

---

## Provider-Modell

Die tatsächliche Ausführung erfolgt über austauschbare Call Provider.

```text
Nova.Call
   ├── Direct Provider
   ├── IPC Provider
   ├── Network Provider
   └── Future Provider
```

Provider implementieren unterschiedliche Transportmechanismen, müssen aber die gemeinsamen Call-Contracts und Sicherheitsregeln respektieren.

---

## Location Transparency mit sichtbarer Semantik

`Nova.Call` unterstützt Location Transparency, darf jedoch relevante Unterschiede zwischen lokalem und entferntem Aufruf nicht verschleiern.

Insbesondere können sich unterscheiden:

- Latenz,
- Ressourcenverbrauch,
- Erreichbarkeit,
- Fehlerwahrscheinlichkeit,
- Cancellation-Verhalten,
- Datenübertragungskosten.

```text
Logical Call
    ↓
Location Transparent
    +
Cost and Failure Visible
```

Ein verteilter Call darf daher nicht semantisch wie ein garantiert erfolgreicher lokaler Funktionsaufruf behandelt werden.

---

## Asynchronität und Structured Concurrency

`Nova.Call` ist mit der systemweiten Structured Concurrency integriert.

Calls können Bestandteil von Task Groups sein und erhalten deren:

- Cancellation,
- Deadlines,
- Resource Context,
- Causation Context.

Langlaufende oder entfernte Calls sollen grundsätzlich asynchron verwendbar sein.

---

## Datenübertragung

Call-Parameter und Ergebnisse verwenden Semantic Types und die systemweite Data/Object-Pipeline.

```text
Typed Input
    ↓
Nova.Call
    ↓
Provider
    ↓
Typed Result
```

Bei lokalen oder geeigneten IPC-Pfaden können Shared Objects und Zero-Copy verwendet werden.

Bei entfernten Calls übernimmt der jeweilige Provider die notwendige Repräsentation und Übertragung.

---

## Security und Capabilities

Das Wissen um ein Call Target erzeugt keine Authority zur Verwendung.

```text
Target Known
    ≠
Call Authorized
```

Vor der Ausführung müssen die erforderlichen Capability-, Identity-, Trust- und Security-Regeln geprüft werden.

Providerwechsel oder Location Transparency dürfen diese Prüfungen nicht umgehen.

---

## Data Sovereignty

Remote Calls unterliegen den Data-Sovereignty- und Information-Flow-Policies der übertragenen Daten und des Call Contracts.

Ein lokaler Call darf daher nicht automatisch in einen Remote Call umgewandelt werden, wenn dadurch verbindliche Sovereignty Constraints verletzt würden.

---

## Fehlersemantik

`Nova.Call` verwendet eine explizite Fehlersemantik für verteilte Kommunikation.

Fehler können unter anderem aus folgenden Bereichen stammen:

- Target Resolution,
- Authority,
- Trust,
- Deadline,
- Cancellation,
- Transport,
- Remote Execution,
- Resource Constraints.

Unsichere Zustände wie ein Verbindungsabbruch nach möglicherweise erfolgter Remote-Ausführung müssen darstellbar bleiben.

Die genaue Fehler- und Result-Semantik wird in NPSPECs definiert.

---

## Introspection und Causation

Calls werden in die systemweite Architecture Introspection und das Causation Tracking integriert.

```text
Caller
  ↓
Nova.Call
  ↓
Provider
  ↓
Callee
  ↓
Result
```

Damit können verteilte Aufrufketten über Execution-Domain- und Netzwerkgrenzen hinweg nachvollzogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS `Nova.Call` als einheitliche Call-Abstraktion für lokale und entfernte Kommunikation bereitstellen.
2. `Nova.Call` MUSS auf expliziten Contracts basieren.
3. Call Targets SOLLEN über logische Identities statt konkrete Transportendpunkte adressiert werden.
4. Lokale, IPC- und Remote-Ausführung MÜSSEN über Provider integrierbar sein.
5. Die Wahl des Providers DARF die logische Call-Schnittstelle nicht unnötig verändern.
6. Location Transparency DARF relevante Kosten- und Fehlerunterschiede NICHT verbergen.
7. `Nova.Call` MUSS mit Structured Concurrency, Cancellation und Deadlines integrierbar sein.
8. Parameter und Ergebnisse MÜSSEN mit Semantic Types und der systemweiten Data/Object-Pipeline integrierbar sein.
9. Geeignete lokale Datenpfade SOLLEN Zero-Copy beziehungsweise Shared Objects verwenden können.
10. Kenntnis eines Call Targets DARF keine implizite Authority erzeugen.
11. Capability-, Trust- und Security-Regeln MÜSSEN unabhängig vom verwendeten Provider gelten.
12. Data-Sovereignty-Constraints MÜSSEN Provider- und Location-Auswahl begrenzen können.
13. Verteilte Fehler und unsichere Ausführungszustände MÜSSEN explizit darstellbar sein.
14. Provider DÜRFEN keine stärkeren Garantien vortäuschen, als der zugrunde liegende Mechanismus tatsächlich bietet.
15. Call-Ketten MÜSSEN kontrolliert introspektierbar und über Causation nachvollziehbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- ein gemeinsames Programmiermodell für lokale, IPC- und Remote-Aufrufe,
- Dienste können ihren Ausführungsort ändern, ohne ihre logische Schnittstelle grundlegend zu verändern,
- Security, Trust, Deadlines und Resource Budgets werden systemweit vereinheitlicht,
- direkte Integration mit NetworkFlow, Structured Concurrency und Semantic Types,
- zukünftige Kommunikationsmechanismen können als Provider ergänzt werden.

### Negative Konsequenzen

- die gemeinsame Abstraktion muss unterschiedliche Fehler- und Kostenmodelle korrekt darstellen,
- Provider Resolution und Contract-Prüfung erzeugen zusätzlichen Systemzustand,
- Entwickler dürfen Location Transparency nicht mit identischer Laufzeitsemantik verwechseln.

---

## Verworfene Alternativen

### Separate APIs für IPC und RPC

Verworfen.

Dies würde logisch identische Serviceaufrufe unnötig an ihren aktuellen Ausführungsort koppeln.

### Remote Calls wie lokale Funktionsaufrufe behandeln

Verworfen.

Verteilte Kommunikation besitzt zusätzliche Fehler-, Latenz- und Konsistenzprobleme, die nicht verborgen werden dürfen.

### Transportendpunkte als primäre Service Identity

Verworfen.

IP-Adressen, Ports und IPC-Endpunkte beschreiben Erreichbarkeit, nicht die stabile Identität eines Services.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-IPC-0003_Zero_Copy_IPC`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0020_Intent_Based_Networking`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-CALL-0001`
- `NPSPEC-DISTCOMM-CALL-CONTRACT-0001`
- `NPSPEC-DISTCOMM-CALL-TARGET-0001`
- `NPSPEC-DISTCOMM-CALL-PROVIDER-0001`
- `NPSPEC-DISTCOMM-CALL-SECURITY-0001`
- `NPSPEC-DISTCOMM-CALL-ERROR-0001`
- `NPSPEC-DISTCOMM-CALL-TEST-0001`

---

## Ergebnis

NovaOS erhält mit `Nova.Call` eine gemeinsame Abstraktion für Aufrufe unabhängig vom aktuellen Ausführungsort:

```text
Caller
  ↓
Nova.Call
  ↓
Contract + Authority
  ↓
Target Resolution
  ↓
Direct / IPC / Network Provider
  ↓
Callee
```

Damit können lokale und verteilte Dienste unter einem gemeinsamen Contract angesprochen werden, während Kosten, Fehlersemantik, Security, Data Sovereignty und tatsächliche Ausführungsgrenzen weiterhin explizit erhalten bleiben.