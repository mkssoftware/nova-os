# NPSPEC-PRIVACY-LABEL-0001 – Nova Privacy Labels

## Status

Angenommen

## Kategorie

Privacy / Data Protection / Privacy Labels

## Zweck

NovaOS definiert Privacy Labels zur maschinenlesbaren Kennzeichnung datenschutzrelevanter Eigenschaften von Daten.

```text
Data
 ↓
Privacy Label
 ↓
Privacy Policy
 ↓
Allowed Processing
```

Privacy Labels begleiten Daten über Speicherung, Verarbeitung und Übertragung hinweg.

## Grundprinzipien

```text
Privacy Label ≠ Permission
Classification ≠ Authorization
Personal Data ≠ Public Data
Copy ≠ Loss of Label
Location Change ≠ Policy Change
```

## Label-Modell

Ein Privacy Label besitzt mindestens:

```text
PrivacyLabel
├── LabelID
├── Data Category
├── Purpose
└── State
```

Optional:

```text
Sensitivity
Data Subject
Retention Policy
Expiration
Allowed Recipients
Processing Restrictions
Sovereignty Policy
Consent Reference
Provenance
```

## Datenkategorien

NovaOS muss unterschiedliche Kategorien unterstützen können:

```text
Public
Personal
Sensitive Personal
Confidential
Anonymous
Pseudonymous
System Internal
```

Zusätzliche Kategorien müssen policygesteuert definierbar sein.

## Zweckbindung

Privacy Labels können erlaubte Verarbeitungszwecke beschreiben.

```text
Data
 ↓
Purpose = Account Management
 ↓
Requested Processing
 ↓
Purpose Check
```

Ein Zugriff auf Daten bedeutet nicht automatisch, dass jede weitere Verarbeitung zulässig ist.

## Propagation

Privacy Labels müssen bei Datenbewegungen und Ableitungen berücksichtigt werden.

```text
Source Data
    ↓
Copy / IPC / Network / Derivation
    ↓
Destination Data
    ↓
Privacy Label
```

Kopieren oder Verschieben darf ein Privacy Label nicht automatisch entfernen.

## Abgeleitete Daten

Aus geschützten Daten erzeugte Informationen müssen neu bewertet werden.

```text
Personal Data
     ↓
Processing
     ↓
Derived Data
     ↓
Privacy Classification
```

Abgeleitete Daten dürfen nicht automatisch als ungeschützt gelten.

## Retention und Expiration

Privacy Labels sollen auf Retention- und Expiration-Regeln verweisen können.

```text
Privacy Label
├── Retention Policy
└── Expiration Policy
```

Dadurch können Lebensdauer und zulässige Nutzung direkt mit dem Datenobjekt verbunden werden.

## Datenminimierung

Privacy Labels müssen mit Data Minimization integrierbar sein.

```text
Requested Data
      ↓
Privacy Labels
      ↓
Purpose
      ↓
Minimum Required Data
```

## Information Flow

Privacy Labels können als Eingang für Information-Flow-Entscheidungen dienen.

```text
Source Label
     +
Destination
     +
Operation
     ↓
Privacy Policy
     ↓
Allow / Deny
```

## Sovereignty

Geografische oder organisatorische Einschränkungen können mit Privacy Labels verbunden werden.

Beispiel:

```text
Allowed Region = EU
Allowed Processing = Local
Remote Export = Denied
```

Die eigentliche Durchsetzung erfolgt über die Data-Sovereignty- und Security-Mechanismen.

## Label-Änderung

Änderungen sicherheits- oder datenschutzrelevanter Labels müssen autorisiert sein.

```text
Current Label
     ↓
Authorized Relabel
     ↓
New Label
```

Eine Herabstufung darf nicht allein durch die Anwendung erfolgen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
LabelID
Data Category
Purpose
Retention
Expiration
Sovereignty
Processing Restrictions
State
```

Die Sichtbarkeit des Labels selbst muss Datenschutzanforderungen berücksichtigen.

## Normative Anforderungen

1. NovaOS MUSS maschinenlesbare Privacy Labels unterstützen.
2. Privacy Labels DÜRFEN NICHT als Zugriffsberechtigung behandelt werden.
3. Labels MÜSSEN an stabile Datenidentitäten bindbar sein.
4. Kopieren oder Verschieben DARF Privacy Labels NICHT automatisch entfernen.
5. Abgeleitete Daten MÜSSEN datenschutzrechtlich neu klassifizierbar sein.
6. Purpose-, Retention- und Expiration-Regeln MÜSSEN referenzierbar sein.
7. Privacy Labels MÜSSEN mit Data Minimization integrierbar sein.
8. Information Flow und Data Sovereignty MÜSSEN Privacy Labels verwenden können.
9. Datenschutzrelevantes Relabeling MUSS autorisiert sein.
10. Privacy-Label-Zustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-PRIVACY-EXPIRATION-0001`
- `NPSPEC-SECURITY-LABEL-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `ADR-PRIVACY-0004`

## Ergebnis

```text
Data
 ↓
Privacy Label
 ↓
Purpose + Retention + Sovereignty
 ↓
Privacy Policy
 ↓
Controlled Processing
```

NovaOS erhält damit ein einheitliches Privacy-Label-Modell, mit dem Datenschutzanforderungen direkt an Daten gebunden und über deren gesamten Lebenszyklus berücksichtigt werden können.