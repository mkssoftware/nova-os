# ADR-CAPABILITY-0014 – Capability Provider Austauschbarkeit

## Status

Angenommen

## Kategorie

Capability Architecture / Provider / Austauschbarkeit / Live Evolution

## Kontext

Eine NovaOS-Capability beschreibt eine semantische Fähigkeit unabhängig von ihrer konkreten Implementierung.

Dieselbe Capability kann beispielsweise durch unterschiedliche Provider bereitgestellt werden:

```text
image.resize
    │
    ├── CPU Provider
    ├── GPU Provider
    ├── NPU Provider
    └── Remote Provider
```

Provider können sich hinsichtlich Hardware, Implementierung, Performance, Energieverbrauch, Trust, Locality oder Verfügbarkeit unterscheiden.

Damit Anwendungen nicht dauerhaft an eine konkrete Implementierung gebunden werden, muss die Austauschbarkeit von Providern ein grundlegendes Architekturprinzip des Capability-Systems sein.

---

## Entscheidung

NovaOS definiert **Capability Provider als austauschbare Implementierungen eines stabilen Capability Contracts**.

```text
Consumer
    ↓
Capability Contract
    ↓
Provider Resolution
    ↓
Selected Provider
```

Consumer binden sich grundsätzlich an die Capability und ihren Contract, nicht an die interne Implementierung des Providers.

---

## Grundprinzip

```text
Stable capability contract.

Replaceable implementation.
```

Provider-Austauschbarkeit gilt nur innerhalb der durch Capability Contract und `Nova.ExecutionContract` definierten Grenzen.

---

## Contract als Austauschgrenze

Der Capability Contract bildet die primäre Austauschgrenze.

```text
Provider A ─┐
Provider B ─┼→ Capability Contract → Consumer
Provider C ─┘
```

Ein Provider gilt nur dann als austauschbar, wenn er die für die konkrete Operation erforderliche Semantik erfüllt.

Gleiche Funktionsnamen oder ähnliche Ein-/Ausgabetypen reichen dafür nicht aus.

---

## Provider-Unabhängigkeit

Consumer sollen keine Annahmen über interne Provider-Eigenschaften treffen, sofern diese nicht explizit Bestandteil des Contracts sind.

Nicht zur stabilen Capability-Semantik gehören beispielsweise:

```text
Implementierungssprache
interne Algorithmen
interne Thread-Struktur
physischer Installationspfad
interne Speicherstrukturen
```

Dadurch können Implementierungen verändert werden, ohne Consumer anzupassen.

---

## Provider Resolution

Sind mehrere kompatible Provider verfügbar, entscheidet die Capability Resolution anhand der geltenden Anforderungen.

```text
Capability Requirement
        ↓
Compatible Providers
        ↓
Hard Constraints
        ↓
Optimization
        ↓
Selected Provider
```

Dabei können unter anderem berücksichtigt werden:

- Determinismus,
- Deadline,
- Resource Budget,
- Security,
- Trust,
- Data Sovereignty,
- Locality,
- Provider Health,
- Energieverbrauch,
- Hardwarebeschleunigung.

---

## Explizite Provider-Auswahl

NovaOS wählt standardmäßig automatisch einen geeigneten Provider.

Consumer dürfen jedoch einen Provider bevorzugen oder explizit verlangen, sofern dies der Capability Contract erlaubt.

```text
Resize(image)
→ automatische Provider-Auswahl

Resize(image, provider=GPU)
→ explizite Provider-Anforderung
```

Eine explizite Auswahl darf Hard Constraints nicht umgehen.

---

## Austausch zur Laufzeit

Provider können während des laufenden Systems ausgetauscht werden.

Beispiele:

```text
Provider Failure
Device Removal
Driver Replacement
Software Update
Service Migration
Hot Replacement
```

NovaOS kann daraufhin einen kompatiblen alternativen Provider auflösen.

```text
Provider A
    ↓ unavailable
Resolution
    ↓
Provider B
```

---

## Laufende Operationen

Ein Provider-Wechsel während einer bereits laufenden Operation ist nicht automatisch zulässig.

Je nach Capability Contract kann eine Operation:

```text
weiterlaufen,
migriert,
neu gestartet,
zurückgerollt,
abgebrochen
```

werden.

Die Austauschbarkeit zukünftiger Aufrufe ist damit von der Migration einer bereits laufenden Ausführung zu unterscheiden.

---

## State und Provider-Wechsel

Zustandslose Provider sind grundsätzlich einfacher austauschbar.

Bei zustandsbehafteten Providern muss der relevante Zustand:

```text
übertragbar,
rekonstruierbar,
extern gespeichert
oder bewusst nicht migrierbar
```

sein.

Provider-Austauschbarkeit darf keine implizite Annahme über State-Kompatibilität erzeugen.

---

## Capability Composition

Innerhalb einer Capability Composition können einzelne Provider unabhängig ausgetauscht werden.

```text
A → B → C
    ↓
A → B2 → C
```

NovaOS darf anschließend die Gesamtpipeline neu planen, insbesondere wenn sich Data Movement, Locality oder Resource Usage ändern.

---

## Security und Trust

Ein alternativer Provider ist nur zulässig, wenn er sämtliche erforderlichen Security- und Trust-Bedingungen erfüllt.

```text
Functionally Compatible
        +
Security Compatible
        +
Trust Compatible
        +
Sovereignty Compatible
        ↓
Eligible Provider
```

Funktionale Kompatibilität allein reicht nicht aus.

---

## Deterministic Mode

Im Deterministic Mode muss Provider-Austausch kontrollierbar sein.

Eine reproduzierbare Ausführung kann beispielsweise verlangen:

```text
Fixed Provider
Fixed Provider Class
Fixed Algorithm
Deterministic Provider Set
```

Automatische Optimierungen dürfen diese Anforderungen nicht verletzen.

---

## Introspection

NovaOS muss Provider-Austauschbarkeit nachvollziehbar machen können.

Beispielsweise:

```text
Capability
Current Provider
Alternative Providers
Compatibility
Selection Constraints
Replacement Reason
Previous Provider
```

Damit kann erklärt werden, warum ein bestimmter Provider gewählt oder ersetzt wurde.

---

## Normative Anforderungen

1. NovaOS MUSS mehrere Provider für dieselbe Capability unterstützen.
2. Provider-Austauschbarkeit MUSS auf dem Capability Contract basieren.
3. Consumer SOLLEN an Capability Contracts statt konkrete Implementierungen gebunden sein.
4. Provider MÜSSEN unabhängig von ihrem physischen Installationsort austauschbar sein können.
5. Funktionale Kompatibilität MUSS vor einem Provider-Wechsel geprüft werden.
6. Hard Constraints MÜSSEN bei jeder Provider-Auswahl und jedem Austausch erneut gelten.
7. NovaOS MUSS automatische Provider-Auswahl unterstützen.
8. Explizite Provider-Auswahl MUSS möglich sein, sofern der Contract dies erlaubt.
9. Explizite Auswahl DARF Hard Constraints NICHT umgehen.
10. Provider MÜSSEN zur Laufzeit ersetzt werden können, wenn eine kompatible Alternative vorhanden ist.
11. Laufende Operationen DÜRFEN nur entsprechend ihres Contracts migriert oder neu gebunden werden.
12. Zustandsbehaftete Provider MÜSSEN ihre State-Kompatibilität explizit definieren.
13. Capability Composition MUSS den Austausch einzelner Provider unterstützen können.
14. Security, Trust und Data Sovereignty MÜSSEN bei alternativen Providern erhalten bleiben.
15. Deterministic Mode MUSS automatische Provider-Wechsel begrenzen können.
16. Provider-Auswahl und Provider-Wechsel MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen bleiben von konkreten Implementierungen entkoppelt,
- neue Hardware kann bestehende Capabilities beschleunigen,
- Software- und Hardwareprovider können gegenseitig als Fallback dienen,
- Provider können aktualisiert oder ersetzt werden,
- Fehler einzelner Provider müssen nicht automatisch zum Verlust der Capability führen,
- Live Evolution wird auf Capability-Ebene möglich.

### Negative Konsequenzen

- Capability Contracts müssen präzise genug für echte Austauschbarkeit sein,
- zustandsbehaftete Provider benötigen zusätzliche Migrationsregeln,
- Provider Resolution muss Kompatibilität und Hard Constraints zuverlässig prüfen.

---

## Verworfene Alternativen

### Capability fest an einen Provider binden

Verworfen.

Dies würde Capability Identity und Implementierung unnötig koppeln.

### Jeder funktional ähnliche Provider ist automatisch austauschbar

Verworfen.

Semantic Types, Contracts, Security, Trust und andere Hard Constraints müssen berücksichtigt werden.

### Provider-Wechsel nur nach Systemneustart

Verworfen.

Dies widerspricht Dynamic Capabilities und Live Evolution.

### Explizite Provider-Auswahl grundsätzlich verbieten

Verworfen.

NovaOS soll automatische Optimierung anbieten, Entwicklern aber kontrollierte explizite Auswahl ermöglichen.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-CAPABILITY-0006_Capability_Negotiation`
- `ADR-CAPABILITY-0013_Dynamic_Capabilities`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0018_Locality_as_Optimization_Objective`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-PROVIDER-REPLACEMENT-0001`
- `NPSPEC-CAPABILITY-PROVIDER-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-SELECTION-0001`
- `NPSPEC-CAPABILITY-PROVIDER-STATE-MIGRATION-0001`
- `NPSPEC-CAPABILITY-PROVIDER-FAILOVER-0001`
- `NPSPEC-CAPABILITY-PROVIDER-REPLACEMENT-TEST-0001`

---

## Ergebnis

NovaOS trennt die stabile semantische Fähigkeit von ihrer konkreten Implementierung:

```text
Capability
    ↓
Stable Contract
    ↓
Provider Resolution
    ↓
┌──────────┬──────────┬──────────┐
│Provider A│Provider B│Provider C│
└──────────┴──────────┴──────────┘
```

Dadurch können Provider aufgrund von Hardware, Performance, Verfügbarkeit, Updates oder Systemzustand ausgetauscht werden, während Anwendungen weiterhin gegen denselben Capability Contract arbeiten.