# ADR-VERIFY-0001 – Formale Spezifikation für kritische Komponenten

## Status

Angenommen

## Kategorie

Verification / Formal Methods / Safety / Correctness

## Kontext

Einige NovaOS-Komponenten sind so kritisch, dass klassische Tests allein keine ausreichende Sicherheit bieten.

Dazu gehören insbesondere Komponenten, deren Fehler:

```text
Security Boundaries verletzen
Daten beschädigen
Systemintegrität gefährden
Hard-Realtime-Garantien brechen
Recovery verhindern
```

Eine vollständige formale Spezifikation des gesamten Betriebssystems wäre jedoch unverhältnismäßig aufwendig.

## Entscheidung

NovaOS verwendet **formale Spezifikationen gezielt für kritische Komponenten und Invarianten**.

```text
Critical Component
       ↓
Formal Specification
       ↓
Implementation
       ↓
Verification
```

Formale Methoden sind damit ein gezieltes Werkzeug und keine Pflicht für jede NovaOS-Komponente.

## Auswahl

Eine Komponente ist ein Kandidat für formale Spezifikation, wenn Fehler besonders schwerwiegende Folgen haben oder zentrale Systeminvarianten geschützt werden müssen.

Beispiele:

```text
Capability Enforcement
Memory Protection
Transactional Commit
Boot Integrity
Security State Transitions
Realtime Guarantees
Critical Recovery Logic
```

## Spezifikationsumfang

Bevorzugt werden kleine, klar definierte Eigenschaften formal beschrieben.

Beispiele:

```text
Unauthorized access is impossible

Committed transaction is never partially visible

Capability authority cannot increase through delegation

Hard realtime reservation cannot be stolen by best-effort work
```

Nicht jede Implementierungsdetails muss Bestandteil der formalen Spezifikation sein.

## Invarianten

Kritische Systemregeln sollen als überprüfbare Invarianten formuliert werden.

```text
System State
     ↓
Operation
     ↓
System State'

Invariant(State)
AND
Valid(Operation)
→
Invariant(State')
```

Damit kann geprüft werden, ob eine Operation zentrale Systemeigenschaften erhält.

## Verhältnis zu Tests

Formale Verifikation ersetzt keine Tests.

NovaOS verwendet:

```text
Formal Verification
+
Static Analysis
+
Testing
+
Runtime Verification
```

je nach Kritikalität und Komponente.

Tests prüfen insbesondere reale Implementierung, Hardwareinteraktion und Integrationsverhalten.

## Evolvierbarkeit

Formale Spezifikationen müssen gemeinsam mit der Architektur weiterentwickelt werden.

Ändert sich eine relevante Systemregel, muss geprüft werden, ob:

```text
Specification
Proof
Implementation
```

weiterhin übereinstimmen.

## Normative Anforderungen

1. NovaOS MUSS formale Spezifikation für ausgewählte kritische Komponenten unterstützen.
2. Nicht jede Komponente MUSS formal spezifiziert werden.
3. Kritische Systeminvarianten SOLLEN bevorzugt formal beschrieben werden.
4. Der Umfang formaler Modelle SOLL möglichst klein und klar begrenzt bleiben.
5. Formale Spezifikation DARF klassische Tests nicht ersetzen.
6. Änderungen an relevanten Komponenten MÜSSEN bestehende formale Eigenschaften berücksichtigen.
7. Sicherheits- und Korrektheitseigenschaften SOLLEN unabhängig von AI überprüfbar bleiben.
8. Verifikationsergebnisse MÜSSEN einer konkreten Spezifikations- und Implementierungsversion zuordenbar sein.

## Konsequenzen

### Positiv

- kritische Invarianten können mathematisch überprüft werden,
- Architekturfehler können vor der Implementierung sichtbar werden,
- besonders sicherheitskritische Komponenten erhalten stärkere Garantien,
- der Aufwand bleibt durch selektiven Einsatz begrenzt.

### Negativ

- formale Spezifikationen verursachen zusätzlichen Entwicklungsaufwand,
- Änderungen können neue Verifikation erfordern,
- geeignete Methoden und Werkzeuge müssen pro Komponente gewählt werden.

## Abhängigkeiten

- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-SECURITY-0001`
- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0007_Transactional_State`
- `ADR-REALTIME-0004_Deadline_Guarantees`

## Ergebnis

NovaOS verfolgt keinen Ansatz:

```text
Formal verify everything
```

sondern:

```text
Identify critical invariants
        ↓
Specify formally
        ↓
Verify critical mechanisms
        ↓
Test the complete implementation
```

Formale Methoden werden dort eingesetzt, wo ihr zusätzlicher Nachweis für Sicherheit und Korrektheit den Aufwand rechtfertigt.