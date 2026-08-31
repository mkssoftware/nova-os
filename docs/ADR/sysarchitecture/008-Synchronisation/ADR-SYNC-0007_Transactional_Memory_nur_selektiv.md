# ADR-SYNC-0007 – Transactional Memory nur selektiv

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / Concurrency / Transactional Memory

## Kontext

Transactional Memory (TM) ermöglicht es, mehrere Speicherzugriffe als logische Transaktion auszuführen:

```text
Begin Transaction
      ↓
Read / Modify State
      ↓
Commit
   ┌──┴──┐
Success Conflict
          ↓
         Abort
```

Hardware Transactional Memory (HTM) kann auf unterstützten Prozessoren komplexe kritische Abschnitte spekulativ ohne klassischen Lock ausführen. Software Transactional Memory (STM) kann ähnliche Semantik softwareseitig bereitstellen.

Dies kann bei bestimmten Workloads Vorteile bieten:

- geringere Lock-Contention,
- optimistische Parallelität,
- Vermeidung unnötiger Serialisierung,
- Beschleunigung bestimmter kurzer kritischer Abschnitte.

Transactional Memory besitzt jedoch erhebliche Einschränkungen.

Transaktionen können aus unterschiedlichen Gründen abbrechen:

- konkurrierende Speicherzugriffe,
- begrenzte Hardwarekapazität,
- Interrupts oder Context Switches,
- Cache-Konflikte,
- nicht unterstützte Instruktionen,
- Plattformunterschiede.

Damit ist insbesondere HTM kein ausreichend vorhersehbares Fundament für allgemeine Kernel-Synchronisation oder Hard-Realtime-Garantien.

---

## Entscheidung

NovaOS unterstützt Transactional Memory ausschließlich als **optionale und selektive Synchronisationsoptimierung**.

```text
Correct Synchronization Mechanism
          ↓
TM-capable Optimization?
       ┌──┴──┐
      No    Yes
       ↓      ↓
   Base Path  Transaction
                 ↓
           Commit successful?
             ┌──┴──┐
            Yes    No
             ↓      ↓
          Complete  Fallback
```

Jeder Einsatz von Transactional Memory muss einen korrekten nicht-transaktionalen Fallback besitzen.

Die Systemkorrektheit darf niemals davon abhängen, dass eine Hardware- oder Softwaretransaktion erfolgreich abgeschlossen werden kann.

---

## Grundprinzip

```text
Transactional Memory is an optimization,
not a synchronization foundation.

Correctness must exist without it.

Abort must always have a safe path.
```

---

## Hardware Transactional Memory

NovaOS darf Hardware Transactional Memory verwenden, wenn die jeweilige CPU und Plattform geeignete Fähigkeiten bereitstellen.

Die Verfügbarkeit wird über die CPU-Capability- und HAL-Infrastruktur erkannt.

Kernelcode darf HTM-Unterstützung nicht global voraussetzen.

```text
CPU Capability
      ↓
HTM available?
   ┌──┴──┐
  No    Yes
   ↓      ↓
Fallback  Optional HTM Path
```

---

## Software Transactional Memory

Software Transactional Memory wird nicht als allgemeines Kernel-Synchronisationsmodell vorgeschrieben.

STM darf für spezialisierte Subsysteme oder Runtimes eingesetzt werden, wenn ein konkreter Nutzen nachgewiesen wird.

Die deutlich höheren Verwaltungs- und Instrumentierungskosten müssen dabei berücksichtigt werden.

---

## Fallback

Jeder TM-basierte Pfad muss einen semantisch korrekten Fallback besitzen.

Typische Struktur:

```text
Try Transaction
      ↓
Commit?
 ┌────┴────┐
Yes        No
 ↓          ↓
Done    Retry Policy
             ↓
          Fallback Lock
```

Der Fallback darf nicht lediglich als seltene Fehlerbehandlung betrachtet werden.

Er ist Bestandteil des normalen Korrektheitsmodells.

---

## Abort-Semantik

Ein Transaction Abort ist grundsätzlich kein Systemfehler.

Er bedeutet lediglich, dass der spekulative Transaktionspfad nicht abgeschlossen werden konnte.

NovaOS muss zwischen:

```text
Expected Transaction Abort

und

Actual Synchronization Failure
```

unterscheiden.

Ein Abort darf keine teilweise sichtbare Änderung des transaktionalen Zustands hinterlassen.

---

## Retry

Ein Abort darf einen begrenzten Retry auslösen.

Unbeschränkte Wiederholungen sind nicht zulässig.

```text
Abort
  ↓
Bounded Retry
  ↓
Still failing?
  ↓
Fallback
```

Die Retry-Policy kann anhand von Konfliktart, Plattform und Workload variieren.

Adaptive Policies dürfen Retry-Grenzen innerhalb sicherer Grenzen optimieren.

---

## Lock Elision

Ein bevorzugter Anwendungsfall für HTM ist selektive Lock Elision.

Dabei bleibt ein klassischer Lock das Korrektheitsfundament.

```text
Existing Lock
     ↓
Try Transactional Elision
     ↓
Success → execute without acquiring lock

Abort
     ↓
Acquire normal lock
```

Dadurch bleibt die Synchronisationssemantik auch auf Hardware ohne Transactional Memory vollständig erhalten.

---

## Konflikte mit Fallback Locks

TM- und Fallback-Pfade müssen so koordiniert werden, dass eine aktive Transaktion nicht inkonsistent parallel zu einem nicht-transaktionalen Lock-Owner arbeitet.

Die konkrete Lock-Elision-Semantik muss deshalb den Zustand des Fallback Locks berücksichtigen.

---

## Geeignete Einsatzgebiete

Transactional Memory soll nur für Strukturen erwogen werden, bei denen:

- kritische Abschnitte kurz sind,
- Konflikte typischerweise selten auftreten,
- der transaktionale Working Set klein genug ist,
- keine ungeeigneten Seiteneffekte auftreten,
- ein einfacher Fallback existiert,
- Messungen einen tatsächlichen Vorteil zeigen.

Beispiele können bestimmte:

```text
Kernel Metadata Updates
Short Shared-State Updates
Selected Runtime Structures
Lock-Elision Paths
```

sein.

---

## Ungeeignete Einsatzgebiete

Transactional Memory soll nicht für Pfade verwendet werden, die:

- lange kritische Abschnitte besitzen,
- häufige Konflikte erwarten,
- blockierende Operationen enthalten,
- irreversible externe Seiteneffekte ausführen,
- garantierten Transaktionserfolg benötigen,
- Hard-Realtime-Worst-Case-Verhalten auf spekulative Commits stützen.

---

## Seiteneffekte

Speichertransaktionen dürfen keine Operationen enthalten, deren externe Wirkung bei einem Abort nicht sicher zurückgenommen werden kann.

Besonders kritisch sind:

- Device I/O,
- MMIO,
- externe IPC-Effekte,
- irreversible Systemzustandsänderungen.

Transactional Memory ist damit nicht mit den systemweiten transaktionalen Operationen aus `ADR-ARCH-0004` gleichzusetzen.

---

## Transactional Memory vs. System Transactions

NovaOS unterscheidet ausdrücklich:

```text
Transactional Memory
```

und:

```text
Transactional System Operations
```

Transactional Memory betrifft spekulative Speicherzugriffe innerhalb eines begrenzten Ausführungspfades.

Transactional System Operations besitzen dagegen explizite:

- Commit-Semantik,
- Rollback-Semantik,
- Ressourcenoperationen,
- Systemzustände,
- Recovery-Regeln.

HTM ersetzt keine systemweite Transaktionsarchitektur.

---

## Interrupts und Preemption

Interrupts, Exceptions, Preemption oder andere CPU-Ereignisse können abhängig von der Plattform zum Abbruch einer Hardwaretransaktion führen.

Kernelcode darf deshalb nicht voraussetzen, dass eine Transaktion über solche Ereignisse hinweg fortgesetzt werden kann.

Der Fallback muss jederzeit korrekt bleiben.

---

## Realtime

Transactional Memory wird nicht als Fundament für Hard-Realtime-Synchronisation verwendet.

Der Erfolg und die Anzahl möglicher Aborts sind auf vielen Plattformen nicht ausreichend deterministisch begrenzbar.

Hard-Realtime-Pfade dürfen TM höchstens als optionale Optimierung verwenden, wenn der garantierte Worst Case vollständig durch den nicht-transaktionalen Pfad abgedeckt wird.

---

## Deterministic Mode

Deterministic Mode darf Transactional Memory deaktivieren.

Dies ist insbesondere dann erforderlich, wenn:

- Abort-Verhalten nicht reproduzierbar ist,
- unterschiedliche Hardware unterschiedliche Commit-Ergebnisse erzeugt,
- Scheduling- oder Timing-Verhalten dadurch variiert.

Der Fallback-Pfad muss daher auch im Deterministic Mode vollständig funktionsfähig sein.

---

## Portabilität

Transactional Memory ist keine vorausgesetzte CPU-Fähigkeit von NovaOS.

Die Architektur muss auf:

```text
x86 with HTM
x86 without HTM
ARM with suitable extensions
ARM without TM
Future architectures
```

korrekt funktionieren.

Architekturspezifische TM-Instruktionen werden über geeignete Low-Level-Abstraktionen gekapselt.

---

## Security

Spekulative Hardwaremechanismen können sicherheitsrelevante Eigenschaften besitzen.

NovaOS darf Transactional Memory deaktivieren oder einschränken, wenn:

- bekannte Hardwareprobleme bestehen,
- Mitigations dies verlangen,
- Isolationseigenschaften nicht ausreichend gewährleistet werden können.

Security Constraints besitzen Vorrang vor Performancevorteilen durch TM.

---

## Resource Economy

Transactional Memory kann Ressourcen verbrauchen, ohne bei einem Abort sichtbaren Fortschritt zu erzeugen.

Insbesondere wiederholte Aborts können:

- CPU-Zeit,
- Cache-Kapazität,
- Speicherbandbreite,
- Energie

verbrauchen.

Diese Kosten müssen bei adaptiven TM-Policies berücksichtigt werden.

---

## Adaptive Auswahl

NovaOS darf anhand beobachteter Ergebnisse entscheiden, ob ein TM-Pfad für eine bestimmte Struktur oder Plattform weiterhin sinnvoll ist.

Beispiel:

```text
TM Attempts
     ↓
Commit / Abort Statistics
     ↓
Prediction Error
     ↓
Policy Adjustment
```

Bei dauerhaft hoher Abort-Rate kann Transactional Memory deaktiviert oder seltener versucht werden.

Diese Anpassung betrifft ausschließlich die Optimierung.

Der Fallback bleibt das Korrektheitsfundament.

---

## Introspection

Transactional-Memory-Pfade sollen ausreichend introspektierbar sein.

Relevante Metriken sind insbesondere:

```text
Transaction Attempts
Successful Commits
Aborts
Retry Count
Fallback Count
Conflict Aborts
Capacity Aborts
```

Damit kann überprüft werden, ob der TM-Pfad tatsächlich einen Vorteil bietet.

---

## Normative Anforderungen

1. NovaOS DARF Transactional Memory als optionale Synchronisationsoptimierung unterstützen.
2. Transactional Memory DARF NICHT das allgemeine Synchronisationsfundament von NovaOS bilden.
3. Jeder TM-basierte Pfad MUSS einen korrekten nicht-transaktionalen Fallback besitzen.
4. Systemkorrektheit DARF NICHT vom erfolgreichen Commit einer Hardwaretransaktion abhängen.
5. HTM-Verfügbarkeit MUSS als explizite Hardwarefähigkeit erkannt werden.
6. Kernelcode DARF HTM-Unterstützung NICHT global voraussetzen.
7. Transaction Aborts MÜSSEN ohne teilweise sichtbare Speicheränderungen behandelt werden.
8. Aborts DÜRFEN als normaler Bestandteil der TM-Ausführung behandelt werden.
9. Retry-Schleifen nach Aborts MÜSSEN begrenzt sein.
10. Nach wiederholtem Abort MUSS ein definierter Fallback möglich sein.
11. Lock Elision SOLL auf einem weiterhin korrekten Lock-basierten Pfad aufbauen.
12. TM- und Fallback-Lock-Pfade MÜSSEN miteinander konsistent synchronisiert werden.
13. Transactional Memory SOLL nur für kurze und geeignete kritische Abschnitte eingesetzt werden.
14. Irreversible externe Seiteneffekte DÜRFEN NICHT unkontrolliert innerhalb abbrechbarer Speichertransaktionen ausgeführt werden.
15. Transactional Memory DARF NICHT mit transaktionalen Systemoperationen gleichgesetzt werden.
16. Hard-Realtime-Garantien DÜRFEN NICHT vom erfolgreichen TM-Commit abhängen.
17. Der garantierte Realtime-Worst-Case MUSS ohne TM erfüllbar sein, wenn TM in einem Realtime-Pfad verwendet wird.
18. Deterministic Mode MUSS Transactional Memory deaktivieren oder kontrolliert einschränken können.
19. NovaOS MUSS auf Plattformen ohne Transactional Memory vollständig funktionsfähig bleiben.
20. Architekturspezifische TM-Fähigkeiten MÜSSEN hinter einer geeigneten Low-Level-Abstraktion gekapselt werden.
21. Security-Constraints MÜSSEN Transactional Memory deaktivieren können.
22. Wiederholte Aborts MÜSSEN als Ressourcenverbrauch berücksichtigt werden können.
23. Adaptive Policies DÜRFEN TM-Nutzung anhand beobachteter Commit- und Abort-Raten optimieren.
24. Adaptive TM-Entscheidungen DÜRFEN die Korrektheitssemantik NICHT verändern.
25. TM-Commit-, Abort- und Fallback-Verhalten SOLL introspektierbar sein.
26. Der Einsatz von Transactional Memory SOLL durch messbare Vorteile gegenüber dem Fallback begründet werden.

---

## Konsequenzen

### Positive Konsequenzen

- ausgewählte kritische Abschnitte können Lock-Contention vermeiden,
- HTM kann als transparente Optimierung bestehender Lock-Pfade eingesetzt werden,
- Hardware ohne Transactional Memory bleibt vollständig unterstützt,
- adaptive Policies können ineffektive TM-Nutzung automatisch reduzieren,
- Synchronisationskorrektheit bleibt unabhängig von spekulativem Hardwareverhalten.

### Negative Konsequenzen

- zusätzlicher Implementierungs- und Testaufwand für TM- und Fallback-Pfade,
- Abort-Verhalten ist hardwareabhängig und teilweise schwer vorhersehbar,
- Performancegewinne sind stark workloadabhängig,
- Debugging spekulativer Synchronisationspfade ist komplexer.

---

## Verworfene Alternativen

### Transactional Memory als primäres Kernel-Synchronisationsmodell

Verworfen.

Hardwareunterstützung, Commit-Erfolg und Worst-Case-Verhalten sind nicht universell verfügbar oder ausreichend vorhersehbar.

### HTM ohne Fallback

Verworfen.

Transaktionen können aus legitimen Gründen jederzeit abbrechen.

### Unbegrenzte Transaction Retries

Verworfen.

Hohe Contention oder strukturelle Abort-Ursachen könnten unbegrenzten Ressourcenverbrauch ohne Fortschritt erzeugen.

### Transactional Memory vollständig verbieten

Verworfen.

Auf geeigneter Hardware und für passende Workloads kann TM eine wertvolle optionale Optimierung darstellen.

### HTM als Implementierung systemweiter Transaktionen

Verworfen.

Speichertransaktionen besitzen nicht die notwendigen Semantiken für persistente, verteilte oder ressourcenübergreifende Systemoperationen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-SYNC-0002_Lock_Free_Strukturen_gezielt_einsetzen`
- `ADR-SYNC-0005_Priority_Inheritance_für_Realtime`
- `ADR-SYNC-0006_Deadlock_Detection_und_Diagnostics`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-SCHED-0011_Predictive_und_Adaptive_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-TM-0001`
- `NPSPEC-SYNC-HTM-0001`
- `NPSPEC-SYNC-TM-FALLBACK-0001`
- `NPSPEC-SYNC-TM-LOCK-ELISION-0001`
- `NPSPEC-SYNC-TM-ABORT-0001`
- `NPSPEC-SYNC-TM-ADAPTIVE-0001`
- `NPSPEC-SYNC-TM-INTROSPECTION-0001`
- `NPSPEC-SYNC-TM-TEST-0001`

---

## Ergebnis

NovaOS behandelt Transactional Memory als optionale Optimierung über einem unabhängig korrekten Synchronisationspfad:

```text
Synchronization Operation
          ↓
TM suitable and available?
       ┌──────┴──────┐
      No            Yes
       ↓              ↓
Fallback Path    Try Transaction
                      ↓
                  Commit?
                 ┌────┴────┐
                Yes        No
                 ↓          ↓
               Done     Bounded Retry
                             ↓
                          Fallback
```

Damit kann NovaOS zukünftige und bestehende Transactional-Memory-Hardware nutzen, ohne Portabilität, Determinismus, Realtime-Verhalten oder Systemkorrektheit davon abhängig zu machen.

Die zentrale Architekturregel lautet:

```text
Transactional Memory is optional.

Fallback is mandatory.

Abort is expected.

Correctness never depends on speculation.
```