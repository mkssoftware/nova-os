# NPSPEC-EVIDENCE-0006 – Evidence Retention & Privacy

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie lange Evidence-Daten gespeichert werden und wie deren Datenschutz gewährleistet wird.

Ziel ist, Nachvollziehbarkeit zu erhalten, ohne unnötig sensible oder nicht mehr benötigte Informationen dauerhaft zu speichern.

## Grundprinzip

```text
Evidence
    ↓
Retention Policy
    ↓
KEEP / REDUCE / DELETE
```

## Retention

Evidence darf unterschiedliche Aufbewahrungsregeln besitzen.

Beispiele:

```text
SESSION
TASK
SHORT_TERM
LONG_TERM
PINNED
```

Die konkrete Dauer wird durch Policy festgelegt.

## Abhängigkeiten

Evidence darf nicht gelöscht werden, solange sie für aktive oder aufbewahrungspflichtige Objekte benötigt wird.

Beispiele:

```text
Result
TaskCapsule
Audit Record
Causality Reference
```

Vor Löschung müssen relevante Abhängigkeiten geprüft werden.

## Datenminimierung

Evidence soll nur Informationen enthalten und behalten, die für Nachvollziehbarkeit, Sicherheit oder definierte Anforderungen notwendig sind.

Nicht mehr benötigte Detailinformationen dürfen entfernt oder verdichtet werden.

## Privacy

Evidence unterliegt denselben:

```text
permissions
information_labels
privacy_policies
```

wie andere geschützte Informationen.

Das Vorhandensein eines Evidence Bundles erlaubt keinen zusätzlichen Zugriff auf seine Quellen.

## Redaction

Sensible Bestandteile dürfen kontrolliert entfernt oder verborgen werden.

Beispiel:

```text
Evidence Bundle
    ├── source identity
    ├── algorithm
    └── personal metadata
```

Eine erlaubte Redaction kann personenbezogene Metadaten entfernen, ohne notwendige technische Evidence zu zerstören.

## Retention nach Löschung

Wird das zugehörige Ergebnis gelöscht, muss die Retention Policy bestimmen, ob Evidence:

```text
DELETE
RETAIN
ANONYMIZE
```

wird.

Evidence darf keine unbeabsichtigte Kopie gelöschter sensibler Nutzdaten bilden.

## Beispiel

```text
Evidence {
    result:
        object:analysis:81

    retention:
        SHORT_TERM

    expires:
        policy_defined

    privacy:
        inherit_from_result
}
```

## Normative Anforderungen

1. Evidence MUSS einer definierbaren Retention Policy unterliegen.
2. Nicht mehr benötigte Evidence MUSS löschbar oder reduzierbar sein.
3. Abhängigkeiten MÜSSEN vor der Löschung relevanter Evidence berücksichtigt werden.
4. Evidence MUSS bestehende Zugriffs-, Privacy- und Information-Flow-Regeln einhalten.
5. Evidence SOLL nach dem Prinzip der Datenminimierung gespeichert werden.
6. Sensible Evidence MUSS kontrolliert redigierbar oder anonymisierbar sein.
7. Evidence DARF nicht unbeabsichtigt gelöschte sensible Nutzdaten dauerhaft erhalten.

## Abgrenzung

Diese NPSPEC definiert:

- Evidence Retention
- Datenminimierung
- Privacy
- Redaction und Löschung

Nicht Bestandteil sind:

- allgemeine Storage-Retention
- Evidence-Erzeugung
- Integrität und Signierung
- allgemeines Datenschutz-Policy-Modell

## Zugehörige NPSPECs

- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`
- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`
- `NPSPEC-EVIDENCE-0003 – Algorithm, Model & Tool Identity`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`
- `NPSPEC-INFOFLOW-0001 – Information Labels`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`