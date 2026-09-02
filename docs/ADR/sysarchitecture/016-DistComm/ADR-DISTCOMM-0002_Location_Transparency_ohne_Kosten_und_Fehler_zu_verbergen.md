# ADR-DISTCOMM-0002 – Location Transparency ohne Kosten und Fehler zu verbergen

## Status

Angenommen

## Kategorie

Distributed Communication / Location Transparency / Nova.Call / Distributed Systems

## Kontext

NovaOS unterstützt Location Transparency. Dienste, Capabilities und Objekte sollen über stabile logische Identitäten angesprochen werden können, unabhängig davon, ob ihre konkrete Instanz:

- innerhalb derselben Execution Domain,
- in einer anderen lokalen Execution Domain,
- auf einem anderen System,
- oder über eine verteilte Infrastruktur

ausgeführt wird.

Mit `Nova.Call` existiert dafür eine einheitliche Call-Abstraktion.

Eine vollständige Gleichsetzung lokaler und entfernter Kommunikation wäre jedoch architektonisch falsch.

Remote-Kommunikation besitzt Eigenschaften, die bei lokalen Aufrufen nicht oder in wesentlich anderer Form auftreten:

- höhere und variable Latenz,
- Netzwerkfehler,
- partielle Ausfälle,
- unbekannter Ausführungszustand nach Verbindungsabbruch,
- zusätzliche Security- und Trust-Grenzen,
- Datenübertragungskosten,
- Data-Sovereignty-Anforderungen,
- Bandbreiten- und Ressourcenverbrauch.

NovaOS benötigt deshalb Location Transparency, ohne die physikalischen und verteilten Eigenschaften eines Calls zu verschleiern.

---

## Entscheidung

NovaOS implementiert **Location Transparency auf Ebene der logischen Identität und Schnittstelle**, nicht als Illusion identischer Ausführungseigenschaften.

```text
Logical Target
      ↓
Nova.Call
      ↓
Target Resolution
      ↓
┌────────────┬────────────┬────────────┐
│   Direct   │    IPC     │   Remote   │
└────────────┴────────────┴────────────┘
      ↓
Explicit Cost and Failure Semantics
```

Der Aufrufer kann dieselbe logische Schnittstelle verwenden, unabhängig vom aktuellen Ausführungsort.

Kosten, Risiken und relevante Fehlersemantik bleiben jedoch Bestandteil des Call Contracts und des Resultats.

---

## Grundprinzip

```text
Location transparent.
Cost aware.
Failure explicit.
```

Location Transparency bedeutet:

```text
Same logical target
Same logical contract
Different physical execution possible
```

Sie bedeutet ausdrücklich nicht:

```text
Local Call == Remote Call
```

---

## Transparente Eigenschaften

NovaOS darf folgende Eigenschaften abstrahieren:

- konkrete Prozess-ID,
- konkrete Execution Domain,
- IPC-Endpunkt,
- IP-Adresse,
- Port,
- Netzwerkinterface,
- konkrete Route,
- aktuelle Service Instance.

Anwendungen sollen diese Details für normale Distributed Calls nicht kennen müssen.

---

## Nicht zu verbergende Eigenschaften

Architektonisch relevante Unterschiede müssen sichtbar beziehungsweise über Contracts ausdrückbar bleiben.

Dazu gehören insbesondere:

- Latenz,
- Deadline-Risiko,
- Erreichbarkeit,
- mögliche partielle Fehler,
- Datenbewegung,
- Resource Cost,
- Security Boundary,
- Trust Boundary,
- Data-Sovereignty-Auswirkungen.

Die Abstraktion darf keine Garantien vortäuschen, die der tatsächliche Ausführungspfad nicht besitzt.

---

## Call Cost

`Nova.Call` muss unterschiedliche Ausführungskosten berücksichtigen können.

```text
Direct Call
   ↓
Very Low Cost

IPC Call
   ↓
Domain Crossing Cost

Remote Call
   ↓
Serialization / Network / Remote Execution Cost
```

Diese Unterschiede müssen nicht zwingend Bestandteil jeder Anwendungslogik sein, müssen aber für:

- Scheduling,
- Resource Economy,
- Deadlines,
- Planning,
- Introspection

verfügbar sein.

---

## Explizite Fehlersemantik

Distributed Calls können Fehlerzustände besitzen, die bei normalen lokalen Aufrufen nicht auftreten.

Beispielsweise:

```text
Request sent
     ↓
Remote execution?
     ↓
Connection lost
```

In diesem Fall kann der lokale Knoten möglicherweise nicht eindeutig feststellen, ob die Operation:

- nicht ausgeführt,
- teilweise ausgeführt,
- vollständig ausgeführt

wurde.

NovaOS darf einen solchen Zustand nicht fälschlich als einfachen lokalen Fehler darstellen.

---

## Unknown Outcome

`Nova.Call` muss einen semantisch expliziten unbekannten Ausführungszustand darstellen können.

```text
Success
Failure
Cancelled
DeadlineExceeded
UnknownOutcome
```

Die konkrete Result- und Fehlerstruktur wird in NPSPECs definiert.

`UnknownOutcome` darf nicht automatisch als `Failure` interpretiert werden.

---

## Deadlines

Deadlines gelten über den gesamten Call-Pfad.

```text
Caller
  ↓
Resolution
  ↓
Transport
  ↓
Remote Execution
  ↓
Response
```

Provider dürfen eine Deadline nicht ausschließlich auf einen einzelnen Teilabschnitt des Calls beziehen, wenn der Contract eine End-to-End-Deadline beschreibt.

---

## Cancellation

Cancellation muss über lokale und entfernte Grenzen propagiert werden können.

Eine Cancellation-Anforderung bedeutet jedoch nicht automatisch, dass eine bereits entfernte Operation garantiert gestoppt wurde.

NovaOS muss zwischen:

```text
Cancellation Requested
Cancellation Delivered
Cancellation Confirmed
```

unterscheiden können, sofern diese Unterscheidung für den Contract relevant ist.

---

## Datenbewegung

Location Transparency darf Datenbewegung nicht konzeptionell unsichtbar machen.

Ein Remote Call kann eine physische Übertragung von Daten verursachen.

Diese Übertragung unterliegt:

- Resource Budgets,
- Security Policies,
- Data Sovereignty,
- Information Flow,
- Bandbreitenbeschränkungen.

Große Datenobjekte dürfen deshalb nicht allein aufgrund einer transparenten Call-Schnittstelle unkontrolliert zwischen Systemen verschoben werden.

---

## Security und Trust

Ein Wechsel des Ausführungsortes kann zusätzliche Security- oder Trust-Grenzen erzeugen.

```text
Local Trusted Domain
        ↓
Remote Trust Boundary
```

Location Transparency darf diese Grenze nicht umgehen.

Ein Call darf nur dann remote ausgeführt werden, wenn der resultierende Pfad alle erforderlichen:

- Capabilities,
- Permissions,
- Trust Requirements,
- Security Policies

erfüllt.

---

## Data Sovereignty

Der physische Ausführungsort kann für Data Sovereignty relevant sein.

Ein logisch location-transparenter Service darf deshalb nicht automatisch an einen beliebigen Standort verlagert oder dort aufgerufen werden.

```text
Logical Service
      ↓
Possible Locations
      ↓
Sovereignty Constraints
      ↓
Permitted Locations
```

Location Transparency endet dort, wo verbindliche Location Constraints gelten.

---

## Resource Economy

Distributed Calls werden in die systemweite Resource Economy integriert.

Relevante Kosten können insbesondere umfassen:

- CPU,
- Memory,
- Bandwidth,
- NetworkFlow Resources,
- Serialization,
- Remote Compute,
- Energy,
- Accelerator Resources.

Damit kann NovaOS unterschiedliche mögliche Ausführungsorte auch unter Ressourcengesichtspunkten bewerten.

---

## Introspection

NovaOS muss kontrolliert darstellen können, wo und wie ein Call tatsächlich ausgeführt wurde.

```text
Nova.Call
   ↓
Logical Target
   ↓
Resolved Location
   ↓
Provider
   ↓
Observed Cost
   ↓
Result
```

Location Transparency darf Architecture Introspection nicht verhindern.

Die logische Abstraktion und die tatsächliche physische Ausführung müssen gleichzeitig darstellbar bleiben.

---

## Normative Anforderungen

1. NovaOS MUSS Location Transparency für Distributed Calls unterstützen.
2. Location Transparency MUSS auf logischen Identities und Contracts basieren.
3. Anwendungen SOLLEN konkrete Prozess-, IPC- und Netzwerkendpunkte nicht kennen müssen.
4. Lokale und entfernte Calls DÜRFEN NICHT als physikalisch identische Operationen behandelt werden.
5. Relevante Kosten verteilter Calls MÜSSEN systemseitig darstellbar sein.
6. Distributed-spezifische Fehlerzustände MÜSSEN explizit repräsentierbar sein.
7. Ein unbekannter Remote-Ausführungszustand DARF NICHT automatisch als erfolgreicher oder fehlgeschlagener Call interpretiert werden.
8. Deadlines MÜSSEN End-to-End über relevante Call-Grenzen propagierbar sein.
9. Cancellation MUSS über Distributed-Call-Grenzen propagierbar sein.
10. Cancellation DARF NICHT automatisch als bestätigter Abbruch einer entfernten Operation interpretiert werden.
11. Datenbewegung MUSS trotz Location Transparency Resource-, Security- und Sovereignty-Policies unterliegen.
12. Ein Wechsel des Ausführungsortes DARF Authority-, Capability- oder Trust-Anforderungen NICHT umgehen.
13. Data-Sovereignty-Constraints MÜSSEN mögliche Ausführungsorte begrenzen können.
14. Distributed Calls MÜSSEN in die systemweite Resource Economy integrierbar sein.
15. Logischer Call und tatsächlicher Ausführungspfad MÜSSEN kontrolliert introspektierbar bleiben.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches Programmiermodell für lokale und verteilte Dienste,
- Dienste können ihren Ausführungsort verändern,
- physikalische Netzwerkdetails bleiben von Anwendungen entkoppelt,
- verteilte Fehler werden nicht durch eine zu starke Abstraktion verborgen,
- Deadlines, Ressourcen und Data Sovereignty bleiben durchsetzbar,
- NovaOS kann lokale und entfernte Ausführung intelligent planen.

### Negative Konsequenzen

- `Nova.Call` benötigt eine reichere Fehlersemantik als klassische Funktionsaufrufe,
- Anwendungen müssen bei relevanten Operationen mit verteilten Fehlerzuständen umgehen können,
- Location Transparency kann nicht vollständig alle Unterschiede zwischen lokaler und entfernter Ausführung abstrahieren.

---

## Verworfene Alternativen

### Vollständige Illusion lokaler Calls

Verworfen.

Netzwerkpartitionen, partielle Fehler, Latenz und unbekannte Remote-Ausführungszustände können nicht zuverlässig verborgen werden.

### Keine Location Transparency

Verworfen.

Dies würde Anwendungen unnötig an Prozesse, Maschinen, Netzwerkadressen und konkrete Deployment-Topologien koppeln.

### Remote Calls ausschließlich über separate RPC-APIs

Verworfen.

Dies würde die einheitliche `Nova.Call`-Architektur und die dynamische Platzierung von Diensten unnötig einschränken.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0020_Intent_Based_Networking`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-LOCATION-TRANSPARENCY-0001`
- `NPSPEC-DISTCOMM-CALL-COST-0001`
- `NPSPEC-DISTCOMM-FAILURE-SEMANTICS-0001`
- `NPSPEC-DISTCOMM-UNKNOWN-OUTCOME-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `NPSPEC-DISTCOMM-CANCELLATION-0001`
- `NPSPEC-DISTCOMM-LOCATION-INTROSPECTION-0001`
- `NPSPEC-DISTCOMM-LOCATION-TRANSPARENCY-TEST-0001`

---

## Ergebnis

NovaOS bietet Location Transparency, ohne die fundamentalen Eigenschaften verteilter Systeme zu verschleiern:

```text
                 Logical Interface
                        ↓
                    Nova.Call
                        ↓
                Target Resolution
                        ↓
          ┌─────────────┼─────────────┐
          ↓             ↓             ↓
       Direct          IPC          Remote
          │             │             │
          └─────────────┼─────────────┘
                        ↓
             Explicit Cost / Failure
                        ↓
                      Result
```

Damit bleibt der logische Kommunikationsvertrag unabhängig vom aktuellen Ausführungsort, während Latenz, Ressourcenverbrauch, Datenbewegung, Security-Grenzen und verteilte Fehler weiterhin explizite Bestandteile der NovaOS-Architektur bleiben.