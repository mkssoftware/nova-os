# ADR-DECLARATIVE-0008 – Intent-Based APIs

## Status

Angenommen

## Kategorie

Declarative Architecture / API Design

## Kontext

Traditionelle APIs verlangen häufig, dass der Aufrufer konkrete technische Schritte vorgibt:

```text
create()
configure()
mount()
start()
connect()
```

Dadurch muss der Consumer wissen, **wie** ein gewünschtes Ergebnis technisch hergestellt wird.

NovaOS soll stattdessen APIs bereitstellen, über die ein Consumer primär ausdrückt, **was erreicht werden soll**.

## Entscheidung

NovaOS verwendet bevorzugt **Intent-Based APIs**.

```text
Intent
   ↓
Requirements
   ↓
Resolution
   ↓
Plan
   ↓
Execution
   ↓
Verification
```

Der Consumer beschreibt Ziel und Constraints. NovaOS bestimmt die geeignete Umsetzung.

## Intent

Ein Intent beschreibt das gewünschte Ergebnis.

Beispiel:

```text
Intent:
    Store Object

Requirements:
    Persistent
    Encrypted
    Local
```

Der Consumer muss nicht bestimmen:

```text
Filesystem
Volume
Encryption Provider
Storage Driver
```

sofern dies nicht ausdrücklich erforderlich ist.

## Intent und Desired State

Intent und Desired State sind verwandt, aber nicht identisch.

```text
Intent
   ↓
Interpretation
   ↓
Desired State
   ↓
Reconciliation
```

Ein Intent beschreibt die Absicht des Consumers.

Der daraus erzeugte Desired State beschreibt den konkreten gewünschten Systemzustand.

## Semantic Types

Inputs und Outputs von Intent-Based APIs sollen Semantic Types verwenden.

```text
Typed Input
    ↓
Intent
    ↓
Capability Resolution
    ↓
Typed Output
```

Dadurch kann NovaOS geeignete Capabilities und Provider bestimmen.

## Constraints

Ein Intent kann zusätzliche Anforderungen enthalten:

```text
Security
Trust
Data Sovereignty
Determinism
Latency
Resource Budget
Locality
```

Diese werden Teil des Execution Contracts oder des daraus erzeugten Desired State.

## Provider-Unabhängigkeit

Intent-Based APIs sollen nicht unnötig an konkrete Provider gekoppelt sein.

```text
Intent
   ↓
Capability
   ↓
Provider Resolution
   ↓
Provider
```

Provider dürfen austauschbar bleiben, solange sie den geforderten Contract erfüllen.

## Explizite Kontrolle

Intent-Based APIs verbieten keine explizite Steuerung.

Falls technisch oder fachlich notwendig, kann ein Consumer zusätzliche Constraints oder einen bestimmten Provider verlangen.

Dies muss jedoch explizit erfolgen.

## Fehler

Kann kein gültiger Plan für einen Intent erzeugt werden, muss dies strukturiert gemeldet werden.

```text
Intent
   ↓
No Valid Resolution
   ↓
Reason
```

NovaOS darf Constraints nicht stillschweigend abschwächen, um einen Intent ausführbar zu machen.

## Normative Anforderungen

1. NovaOS SOLL für systemweite Funktionen Intent-Based APIs bevorzugen.
2. Ein Intent MUSS das gewünschte Ergebnis beschreiben können.
3. Technische Implementierungsdetails SOLLEN nicht erforderlich sein, sofern sie nicht Teil des gewünschten Contracts sind.
4. Intent-Based APIs SOLLEN Semantic Types verwenden.
5. Intents MÜSSEN Constraints ausdrücken können.
6. Provider Resolution MUSS von der Beschreibung des Intents getrennt bleiben.
7. Harte Constraints DÜRFEN bei der Resolution nicht stillschweigend abgeschwächt werden.
8. Ein Intent MUSS in einen ausführbaren Plan oder Desired State überführbar sein.
9. Nicht erfüllbare Intents MÜSSEN strukturiert fehlschlagen.
10. Explizite Provider- oder Implementierungsvorgaben MÜSSEN weiterhin möglich sein, wenn sie erforderlich sind.

## Konsequenzen

### Positive Konsequenzen

- APIs werden stärker zielorientiert,
- Consumer benötigen weniger Implementierungswissen,
- Provider bleiben austauschbar,
- NovaOS kann Hardware, Ressourcen und Capabilities dynamisch auswählen,
- deklarative Systemsteuerung wird bis in die API-Ebene fortgeführt.

### Negative Konsequenzen

- Intent Resolution benötigt eindeutige Contracts,
- bei mehreren gültigen Lösungen werden definierte Auswahlregeln benötigt.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0011_Semantic_Discovery`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-INTENT-0001`
- `NPSPEC-DECLARATIVE-INTENT-RESOLUTION-0001`
- `NPSPEC-DECLARATIVE-INTENT-API-0001`

## Ergebnis

NovaOS APIs beschreiben bevorzugt die gewünschte Wirkung statt einer technischen Befehlsfolge:

```text
Consumer Intent
      ↓
Semantic Requirements
      ↓
Constraints
      ↓
Capability Resolution
      ↓
Execution Plan
      ↓
Verified Result
```

Der zentrale Grundsatz lautet:

```text
Der Consumer beschreibt,
was erreicht werden soll.

NovaOS bestimmt,
wie es sicher und vertragskonform erreicht wird.
```