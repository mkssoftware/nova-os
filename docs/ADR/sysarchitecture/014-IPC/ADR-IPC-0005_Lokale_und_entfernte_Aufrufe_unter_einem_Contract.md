# ADR-IPC-0005 – Lokale und entfernte Aufrufe unter einem Contract

## Status

Angenommen

## Kategorie

IPC / Location Transparency / Contracts / Distributed Communication / Services

## Kontext

`ADR-IPC-0004` definiert eine Unified Call Abstraction, durch die Aufrufer nicht für jeden Ausführungsort einer Funktion ein vollständig anderes Programmiermodell benötigen.

Darauf aufbauend muss festgelegt werden, wie lokale und entfernte Implementierungen derselben Funktion beschrieben werden.

Ein Service kann innerhalb derselben Execution Domain, in einer anderen lokalen Domain, auf einem anderen Gerät oder über einen externen Provider ausgeführt werden.

Die fachliche Semantik soll dabei stabil bleiben, ohne Unterschiede bei Latenz, Verfügbarkeit und Fehlern zu verschleiern.

---

## Entscheidung

NovaOS erlaubt **lokale und entfernte Aufrufe unter demselben expliziten Call Contract**.

```text
Call Contract
     ↓
Resolution
 ┌────┼─────────┐
Local IPC     Remote
```

Der Contract beschreibt die logische Operation unabhängig vom aktuellen Ausführungsort.

Die konkrete Location ist eine Eigenschaft der Ausführung und nicht der fachlichen API.

---

## Grundprinzip

```text
Same contract.

Different execution location.

Visible execution characteristics.
```

---

## Gemeinsamer Contract

Lokale und entfernte Provider können denselben Contract implementieren.

Der Contract definiert insbesondere:

- Operation,
- Input Types,
- Output Types,
- Error Semantics,
- Authority Requirements,
- Execution Constraints.

Damit muss eine Capability nicht allein deshalb eine andere API besitzen, weil ihre Implementierung verschoben wird.

---

## Ausführungsorte

Ein Provider kann beispielsweise ausgeführt werden:

```text
Same Execution Domain
Other Local Execution Domain
Other System / Device
External Provider
```

Weitere Location-Typen können zukünftig ergänzt werden, ohne das grundlegende Call-Modell zu verändern.

---

## Location Resolution

NovaOS darf anhand des Contracts und der verfügbaren Provider einen geeigneten Ausführungsort bestimmen.

```text
Call
 ↓
Hard Constraints
 ↓
Eligible Providers
 ↓
Location / Cost Evaluation
 ↓
Selected Provider
```

Security, Data Sovereignty, Trust, Determinism und andere Hard Constraints müssen vor Performance- oder Locality-Optimierungen ausgewertet werden.

---

## Explizite Location Constraints

Ein Contract darf Anforderungen an den Ausführungsort enthalten.

Beispiele:

```text
Local Required
Device Local Required
Specific Trust Domain
Specific Sovereignty Domain
Remote Allowed
```

Damit bedeutet Location Transparency nicht, dass der Ausführungsort immer beliebig ist.

---

## Fehlersemantik

Entfernte Ausführung besitzt zusätzliche mögliche Fehlerbedingungen.

Dazu gehören insbesondere:

- Verbindungsausfall,
- Provider-Nichterreichbarkeit,
- Timeout,
- Remote-System-Ausfall.

Diese Fehler dürfen durch die gemeinsame Call-Abstraktion nicht verborgen werden.

Der Contract muss entsprechende Fehlersemantik ausdrücken können.

---

## Latenz und Kosten

Lokale und entfernte Calls dürfen nicht als kostenidentisch behandelt werden.

```text
Local Call
→ usually low communication cost

Remote Call
→ transport + latency + availability cost
```

Execution Contracts können deshalb Deadline-, Latency-, Resource- und Locality-Anforderungen enthalten.

NovaOS darf diese Informationen bei der Provider-Auswahl berücksichtigen.

---

## Datenübertragung

Der logische Contract ist unabhängig vom Transport.

Lokale Calls können beispielsweise Shared Objects oder Zero-Copy IPC verwenden.

Entfernte Calls können einen geeigneten Transport und eine notwendige Repräsentation verwenden.

Die Transportentscheidung darf die logische Typ- und Contract-Semantik nicht verändern.

---

## Authority und Trust

Eine Capability für einen logischen Service darf nur innerhalb der dafür autorisierten Provider- und Location-Grenzen verwendet werden.

Remote-Ausführung darf keine Security- oder Trust-Prüfung umgehen.

Die Identität und Vertrauenswürdigkeit eines entfernten Providers muss entsprechend dem Contract überprüfbar sein.

---

## Data Sovereignty

Data Sovereignty ist ein Hard Constraint der Location Resolution.

Daten dürfen nicht allein deshalb an einen entfernten Provider übertragen werden, weil dieser schneller oder kostengünstiger erscheint.

```text
Data Policy
    ↓
Allowed Locations
    ↓
Provider Selection
```

Nicht erlaubte Locations werden vor der Optimierung aus der zulässigen Provider-Menge entfernt.

---

## Location Transparency

Location Transparency bedeutet in NovaOS:

```text
Stable logical contract
+
replaceable execution location
+
visible costs and failure boundaries
```

Sie bedeutet ausdrücklich nicht:

```text
Local == Remote
```

Anwendungen können den Ausführungsort abstrahieren, ohne dessen relevante Eigenschaften ignorieren zu müssen.

---

## Normative Anforderungen

1. NovaOS MUSS lokale und entfernte Provider unter demselben logischen Call Contract unterstützen können.
2. Der Call Contract MUSS grundsätzlich unabhängig vom aktuellen Ausführungsort definierbar sein.
3. Lokale und entfernte Provider MÜSSEN dieselbe definierte fachliche Contract-Semantik einhalten.
4. NovaOS MUSS Provider anhand von Location- und Execution-Constraints auswählen können.
5. Hard Constraints MÜSSEN vor Location- und Performance-Optimierungen ausgewertet werden.
6. Contracts MÜSSEN explizite Location Constraints ausdrücken können.
7. Location Transparency DARF zusätzliche Remote-Fehlerbedingungen NICHT verbergen.
8. Lokale und entfernte Calls DÜRFEN NICHT als kostenidentisch angenommen werden.
9. Deadline-, Latency- und Resource-Anforderungen MÜSSEN bei der Provider-Auswahl berücksichtigt werden können.
10. Der konkrete Transport DARF die logische Contract-Semantik NICHT verändern.
11. Remote-Ausführung MUSS dem Capability-, Security- und Trust-Modell unterliegen.
12. Data Sovereignty MUSS als Hard Constraint der Location Resolution behandelt werden.
13. Nicht zulässige Locations DÜRFEN NICHT durch adaptive Optimierung ausgewählt werden.
14. Tatsächlicher Provider, Location und relevante Ausführungskosten MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Services können zwischen lokalen und entfernten Providern verschoben werden,
- Anwendungen bleiben weniger an physische Ausführungsorte gekoppelt,
- Edge-, Cloud- und lokale Verarbeitung können denselben Contract verwenden,
- Data Sovereignty und Trust bleiben Bestandteil der Provider-Auswahl,
- Location kann systemweit optimiert werden.

### Negative Konsequenzen

- Contracts müssen Remote-Fehler berücksichtigen können,
- Provider Resolution wird komplexer,
- lokale und entfernte Ausführung besitzen unterschiedliche Kostenprofile.

---

## Verworfene Alternativen

### Separate API für Remote-Aufrufe

Verworfen.

Dies würde die fachliche API unnötig an den aktuellen Ausführungsort koppeln.

### Vollständig transparente Remote-Ausführung

Verworfen.

Latenz, Fehlergrenzen, Trust und Data Sovereignty dürfen nicht verborgen werden.

### Remote Provider ohne Location Constraints

Verworfen.

Nicht jeder Workload und nicht jedes Datum darf an jedem Ort verarbeitet werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IPC-0001_Typed_IPC`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-IPC-0003_Zero_Copy_IPC`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-LOCATION-CONTRACT-0001`
- `NPSPEC-IPC-REMOTE-CALL-0001`
- `NPSPEC-IPC-PROVIDER-RESOLUTION-0001`
- `NPSPEC-IPC-LOCATION-CONSTRAINT-0001`
- `NPSPEC-IPC-REMOTE-ERROR-0001`
- `NPSPEC-IPC-LOCATION-TEST-0001`

---

## Ergebnis

Lokale und entfernte Ausführung werden in NovaOS unter einer gemeinsamen fachlichen Schnittstelle zusammengeführt:

```text
        Call Contract
             ↓
      Provider Resolution
       /      |       \
   Local     IPC     Remote
```

Der Ausführungsort kann dadurch verändert und optimiert werden, ohne den logischen Contract zu verändern.

Kosten, Fehlergrenzen, Trust und Data Sovereignty bleiben dabei ausdrücklich sichtbar.