# ADR-RESILIENCE-0016 – Safe State für Safety und Realtime

## Status

Angenommen

## Kategorie

Resilience Architecture / Safety / Realtime / Safe State

## Kontext

Für sicherheitskritische oder echtzeitkritische Komponenten reicht klassische Recovery nicht immer aus.

Wenn eine Komponente ihre garantierten Zeit-, Sicherheits- oder Steuerungsbedingungen nicht mehr erfüllen kann, kann das Fortsetzen des normalen Betriebs gefährlicher sein als eine kontrollierte Funktionseinschränkung.

Dies betrifft beispielsweise:

- Hardwaresteuerung,
- industrielle Systeme,
- Robotik,
- Fahrzeuge,
- medizinische Systeme,
- zeitkritische Regelkreise,
- sicherheitskritische Geräte.

NovaOS benötigt deshalb ein explizites Konzept für einen definierten **Safe State**.

## Entscheidung

Safety- und Realtime-relevante Komponenten müssen einen definierten Safe State besitzen können, in den Nova.Resilience bei nicht mehr garantierbarem sicheren Betrieb wechseln kann.

```text
Normal Operation
       ↓
Safety / Realtime Violation
       ↓
Can guarantees be maintained?
   ├── Yes → Controlled Recovery
   └── No  → Safe State
```

Der Safe State hat Vorrang vor Verfügbarkeit und vollständiger Funktionalität.

## Grundprinzip

```text
Cannot guarantee safe operation
              ↓
        Enter Safe State
```

Recovery darf niemals auf Kosten notwendiger Safety-Garantien erzwungen werden.

## Safe State

Ein Safe State ist ein domänenspezifisch definierter Zustand, in dem das Risiko bei einer Störung kontrolliert minimiert wird.

Beispiele können sein:

```text
Stop Motion
Disable Actuator
Hold Position
Controlled Shutdown
Reduce Power
Switch to Redundant Controller
Enter Limited Operation
```

Es existiert kein universeller Safe State für alle Komponenten.

## Deklaration

Safety-relevante Komponenten müssen ihre Safe-State-Semantik explizit beschreiben können.

Dazu können gehören:

```text
Trigger Conditions
Entry Procedure
Required Resources
Maximum Entry Time
Allowed Operations
Exit Conditions
Verification
```

Die konkrete Darstellung wird in NPSPECs definiert.

## Realtime Deadlines

Eine verpasste harte Realtime-Deadline kann selbst einen Fehler darstellen.

```text
Operation
   ↓
Hard Deadline
   ↓ missed
Safety Evaluation
   ↓
Safe State
```

Nova.Resilience darf bei Hard-Realtime-Operationen nicht davon ausgehen, dass eine verspätete korrekte Ausführung noch gültig ist.

## Bounded Recovery

Recovery vor Eintritt in den Safe State ist nur zulässig, wenn sie innerhalb der verbleibenden Safety- und Realtime-Grenzen abgeschlossen werden kann.

```text
Failure
   ↓
Remaining Safety Budget
   ↓
Recovery possible in time?
 ├── Yes → Recover + Verify
 └── No  → Safe State
```

Unbegrenzte Retries oder unbestimmte Recovery-Zeiten sind für solche Pfade nicht zulässig.

## Ressourcen

Die Fähigkeit zum Eintritt in den Safe State darf nicht von Ressourcen abhängen, die im Fehlerfall wahrscheinlich nicht verfügbar sind.

Kritische Safe-State-Pfade sollen deshalb minimale und vorhersehbare Ressourcenanforderungen besitzen.

Wo erforderlich können Ressourcen dafür reserviert werden.

## Failure Isolation

Der Safe State soll grundsätzlich auf den kleinsten sicher ausreichenden Scope begrenzt werden.

```text
Actuator Failure
      ↓
Actuator Safe State
```

Ein vollständiger System-Safe-State ist nur erforderlich, wenn lokale Isolation keine ausreichende Sicherheit gewährleistet.

## Degraded Safe Operation

Ein Safe State muss nicht zwingend vollständige Abschaltung bedeuten.

Eine Komponente kann in einen verifizierten eingeschränkten Betriebsmodus wechseln:

```text
Normal
   ↓
Failure
   ↓
Safe Degraded Mode
```

Dieser Modus darf nur Funktionen bereitstellen, deren Safety- und Realtime-Anforderungen weiterhin garantiert werden können.

## Safe-State-Eintritt

Der Eintritt in einen Safe State muss deterministisch und zeitlich begrenzbar sein können.

Komplexe Recovery-Operationen, dynamische Provider-Suche oder nicht deterministische Abhängigkeiten sollen nicht Teil eines zeitkritischen Safe-State-Pfades sein, sofern dadurch dessen Garantien gefährdet werden.

## Safe-State-Verifikation

Nach dem Eintritt muss der erreichte Zustand soweit technisch möglich verifiziert werden.

```text
Enter Safe State
       ↓
Verify
   ├── Valid → Remain Safe
   └── Failed → Escalate
```

Kann ein lokaler Safe State nicht bestätigt werden, muss auf den nächsten geeigneten sicheren Scope eskaliert werden.

## Restore

Die Rückkehr aus einem Safe State erfolgt nicht automatisch aufgrund des Wegfalls eines Fehlers.

```text
Safe State
    ↓
Recovery
    ↓
Verification
    ↓
Safety Guarantees Restored?
 ├── Yes → Controlled Restore
 └── No  → Remain Safe
```

Alle erforderlichen Safety-, Realtime-, Health-, Trust- und Integrity-Bedingungen müssen erneut erfüllt sein.

## Security

Safety und Security dürfen nicht gegeneinander ausgespielt werden.

Ein Safe-State-Mechanismus darf keine unkontrollierte Möglichkeit bieten, Security-, Capability- oder Authority-Regeln zu umgehen.

Gleichzeitig dürfen Security-Mechanismen einen zwingend erforderlichen Safe-State-Eintritt nicht verhindern.

Die konkrete Priorisierung muss bereits in den System- und Safety-Contracts definiert sein.

## Determinismus

Safety-kritische Safe-State-Pfade müssen, soweit erforderlich, deterministisch ausführbar sein.

Insbesondere müssen relevante Worst-Case-Ausführungszeiten analysierbar sein können.

Adaptive Optimierungen dürfen keine harten Safety- oder Realtime-Garantien verändern.

## Normative Anforderungen

1. NovaOS MUSS explizite Safe States für Safety- und Realtime-relevante Komponenten unterstützen.
2. Safe States MÜSSEN domänenspezifisch definierbar sein.
3. Safety-kritische Komponenten MÜSSEN Trigger und Eintrittsbedingungen für ihren Safe State beschreiben können.
4. Hard-Realtime-Deadline-Verletzungen MÜSSEN als relevante Failure Condition behandelbar sein.
5. Eine verspätete Operation DARF bei Hard-Realtime-Semantik nicht automatisch als gültiger Erfolg gelten.
6. Recovery vor Safe State DARF nur erfolgen, wenn die erforderlichen Safety- und Realtime-Garantien eingehalten werden können.
7. Unbegrenzte Retries DÜRFEN nicht Teil eines Safety-kritischen Recovery-Pfades sein.
8. Safe-State-Pfade SOLLEN minimale und vorhersehbare Ressourcenanforderungen besitzen.
9. Kritische Ressourcen für Safe-State-Eintritt MÜSSEN reservierbar sein.
10. Nova.Resilience SOLL den kleinsten sicher ausreichenden Safe-State-Scope wählen.
11. Ein Safe State DARF als kontrollierter degradierter Betrieb implementiert werden, wenn dessen Garantien nachweisbar erfüllt werden.
12. Der Eintritt in einen zeitkritischen Safe State MUSS zeitlich begrenzbar und soweit erforderlich deterministisch sein.
13. Der erreichte Safe State MUSS soweit technisch möglich verifiziert werden.
14. Fehlgeschlagene Safe-State-Verifikation MUSS eine Eskalation ermöglichen.
15. Die Rückkehr aus einem Safe State MUSS eine erneute Recovery Verification erfordern.
16. Ein Safe State DARF nicht allein aufgrund des Wegfalls eines Fehlers automatisch verlassen werden.
17. Security-, Trust- und Capability-Anforderungen MÜSSEN mit der Safety-Semantik explizit abgestimmt sein.
18. Adaptive Optimierungen DÜRFEN harte Safety- oder Realtime-Garantien nicht abschwächen.
19. Safe-State-Eintritt, Verification, Eskalation und Restore MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Safety hat bei nicht mehr garantierbarem Betrieb Vorrang vor Verfügbarkeit,
- Hard-Realtime-Verletzungen werden semantisch korrekt behandelt,
- Recovery kann zeitlich begrenzt werden,
- lokale Safe States verhindern unnötig große Systemabschaltungen,
- degradierter, aber nachweislich sicherer Betrieb bleibt möglich.

### Negative Konsequenzen

- Safety-relevante Komponenten benötigen explizite Safe-State-Definitionen,
- Worst-Case-Verhalten kritischer Pfade muss analysiert werden,
- Ressourcen für kritische Safe-State-Pfade können reserviert werden müssen.

## Verworfene Alternativen

### Bei jedem Fehler sofort abschalten

Verworfen, da viele Fehler lokal isoliert oder durch einen sicheren degradierten Betrieb behandelt werden können.

### Recovery immer vor Safe State versuchen

Verworfen, da Recovery selbst Deadlines überschreiten oder einen unsicheren Zustand verlängern kann.

### Universeller Safe State für NovaOS

Verworfen, da ein sicherer Zustand von der jeweiligen Hardware, Funktion und Domäne abhängt.

### Automatische Rückkehr nach Fehlerende

Verworfen, da zunächst erneut bestätigt werden muss, dass alle erforderlichen Garantien erfüllt sind.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0013_Recovery_Storm_Protection`
- `ADR-RESILIENCE-0015_Resource_Exhaustion_Recovery`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Deterministic_Mode`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-SAFE-STATE-0001`
- `NPSPEC-RESILIENCE-SAFE-STATE-CONTRACT-0001`
- `NPSPEC-RESILIENCE-SAFE-STATE-ENTRY-0001`
- `NPSPEC-RESILIENCE-SAFE-STATE-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-SAFE-DEGRADED-MODE-0001`
- `NPSPEC-RESILIENCE-REALTIME-RECOVERY-0001`
- `NPSPEC-RESILIENCE-SAFE-STATE-TEST-0001`

## Ergebnis

Nova.Resilience versucht bei Safety- und Hard-Realtime-Systemen nicht um jeden Preis den normalen Betrieb aufrechtzuerhalten:

```text
Failure
   ↓
Safety / Realtime Evaluation
   ↓
Can guarantees still be met?
   ├── Yes
   │    ↓
   │ Bounded Recovery
   │    ↓
   │ Verification
   │    ├── Valid → Controlled Restore
   │    └── Invalid → Safe State
   │
   └── No
        ↓
     Safe State
        ↓
     Verification
        ├── Valid → Remain Safe
        └── Invalid → Escalate
```

Damit besitzt NovaOS einen expliziten Mechanismus für Situationen, in denen die sichere oder zeitgerechte Ausführung nicht mehr garantiert werden kann. In solchen Fällen hat ein definierter Safe State Vorrang vor Verfügbarkeit, automatischer Recovery und vollständiger Funktionalität.