# ADR-PRIVACY-0003 – Semantic Data Labels

## Status

Angenommen

## Kategorie

Privacy Architecture / Semantic Data / Data Classification / Information Flow

## Kontext

NovaOS verarbeitet Daten nicht ausschließlich als primitive Werte oder unstrukturierte Bytefolgen.

Durch Semantic Types kennt das System bereits die fachliche Bedeutung vieler Daten.

Für Privacy-, Security- und Data-Sovereignty-Entscheidungen reicht der Datentyp allein jedoch nicht aus.

Zwei Objekte desselben Typs können unterschiedliche Schutzanforderungen besitzen.

Beispiel:

```text
Image
 ├── Public
 ├── Personal
 ├── Confidential
 └── Biometric
```

NovaOS benötigt deshalb eine standardisierte Möglichkeit, Daten mit maschinenlesbaren semantischen Datenschutz- und Schutzinformationen zu versehen.

---

## Entscheidung

NovaOS führt **Semantic Data Labels als native Metadaten für schutzrelevante Daten** ein.

```text
Data Object
    │
    ├── Semantic Type
    ├── Semantic Data Labels
    ├── Provenance
    └── Policy Context
```

Semantic Data Labels beschreiben relevante Eigenschaften eines Datenobjekts, die von Privacy-, Security-, Information-Flow- und Sovereignty-Policies ausgewertet werden können.

---

## Grundprinzip

```text
Type describes what data is.

Labels describe how data must be treated.
```

Semantic Type und Semantic Data Label sind deshalb getrennte, aber miteinander kombinierbare Konzepte.

---

## Label-Kategorien

Semantic Data Labels können unter anderem folgende Eigenschaften ausdrücken:

```text
Privacy Classification
Sensitivity
Personal Data Category
Confidentiality
Data Sovereignty
Retention Requirements
Purpose Restrictions
Sharing Restrictions
Trust Requirements
Provenance Properties
```

Die konkreten standardisierten Labels und ihre Kodierung werden in NPSPECs definiert.

---

## Beispiel

Ein Foto kann beispielsweise logisch folgende Informationen tragen:

```text
Semantic Type:
    Image<RGB>

Semantic Data Labels:
    PersonalData
    ContainsLocation
    ContainsBiometricInformation
    Private

Sovereignty:
    EU

Retention:
    UserControlled
```

Ein Capability Provider kann dadurch Schutzanforderungen erkennen, ohne den Inhalt selbst analysieren zu müssen.

---

## Labels als Policy-Input

Semantic Data Labels sind Eingaben für Policy-Systeme.

```text
Data
  ↓
Semantic Labels
  ↓
Security / Privacy / IFC / Sovereignty Policy
  ↓
Decision
```

Ein Label erteilt selbst keine Authority.

Beispielsweise bedeutet:

```text
Public
```

nicht automatisch:

```text
Everyone may access the object.
```

Capabilities, ABAC, MAC und andere Sicherheitsmechanismen bleiben weiterhin maßgeblich.

---

## Information Flow

Semantic Data Labels werden mit Information Flow Control integriert.

Bei Datenbewegungen können Labels bestimmen, welche Ziele zulässig sind.

```text
Labeled Data
     ↓
Information Flow Check
     ↓
Destination
```

Ein Datenobjekt mit:

```text
PersonalData
Sovereignty=EU
```

darf beispielsweise nicht allein deshalb an einen beliebigen Remote Provider übertragen werden, weil dieser technisch erreichbar ist.

---

## Label Propagation

Schutzrelevante Labels müssen bei Verarbeitung und Ableitung berücksichtigt werden.

```text
Input A ─┐
         ├→ Processing → Output
Input B ─┘
```

Der Output kann Labels:

- übernehmen,
- kombinieren,
- ableiten,
- verstärken,
- oder durch eine explizit autorisierte Transformation verändern.

Labels dürfen nicht allein durch Kopieren, Konvertieren oder Formatwechsel verloren gehen.

Die konkreten Propagationsregeln werden in NPSPECs definiert.

---

## Declassification

Das Entfernen oder Abschwächen eines schutzrelevanten Labels ist eine sicherheitsrelevante Operation.

```text
Sensitive Data
      ↓
Authorized Declassification
      ↓
Reduced Classification
```

Declassification benötigt eine explizite Authority und muss mit Information Flow Control integriert sein.

Eine normale Anwendung darf Schutzlabels nicht eigenmächtig entfernen, um Policies zu umgehen.

---

## Capability-System

Capability Contracts können deklarieren:

```text
Accepted Labels
Required Labels
Produced Labels
Preserved Labels
Label Transformations
```

Dadurch kann die Capability Resolution Provider ausschließen, die bestimmte Datenschutzanforderungen nicht erfüllen können.

---

## Data Minimization

Semantic Data Labels unterstützen Data Minimization.

Ein System kann beispielsweise erkennen:

```text
Object
 ├── Image Pixels
 ├── Location Metadata [PersonalData]
 └── Camera Metadata
```

Benötigt eine Operation nur die Pixel, kann NovaOS eine entsprechend minimierte Datenansicht bereitstellen.

---

## Retention

Semantic Data Labels können Retention Policies referenzieren oder beeinflussen.

Beispielsweise:

```text
DiagnosticData
Temporary
PersonalData
```

kann eine andere Retention Policy benötigen als:

```text
PublicSystemMetadata
```

Die eigentliche Retention-Semantik bleibt Bestandteil der Retention-Architektur.

---

## Provenance

Labels müssen mit Data Provenance kombinierbar sein.

```text
Source
  ↓
Provenance
  ↓
Semantic Labels
  ↓
Derived Data
```

Damit kann NovaOS nachvollziehen, warum bestimmte Schutzanforderungen für ein Datenobjekt gelten.

Provenance und Labels sind jedoch getrennte Konzepte:

```text
Provenance = Woher stammen die Daten?

Semantic Data Labels = Wie müssen die Daten behandelt werden?
```

---

## Speicherung und Übertragung

Schutzrelevante Labels müssen relevante Datenbewegungen überleben.

Dies umfasst insbesondere:

```text
Storage
NovaFile
Shared Objects
IPC
Nova.Call
Network Transfer
Backup
Replication
Remote Execution
```

Eine Serialisierung oder Übertragung darf notwendige Schutzinformationen nicht stillschweigend entfernen.

---

## Vertrauen von Labels

Nicht jede Quelle darf beliebige Labels setzen oder entfernen.

NovaOS muss zwischen beispielsweise folgenden Fällen unterscheiden können:

```text
Trusted System Label
Application Label
Derived Label
User Label
Remote Label
Unverified Label
```

Sicherheitskritische Entscheidungen dürfen nicht blind auf untrusted Labels vertrauen.

---

## Labels und Inhalt

Semantic Data Labels sollen möglichst ohne erneute vollständige Inhaltsanalyse nutzbar sein.

Sie ersetzen jedoch keine Inhaltsprüfung, wenn eine Policy eine solche ausdrücklich verlangt.

Ein Label ist eine deklarierte oder abgeleitete semantische Eigenschaft und keine automatische Garantie über den tatsächlichen Inhalt.

---

## Architecture Introspection

NovaOS muss kontrolliert introspektierbar machen können:

```text
Data Identity
Semantic Type
Semantic Labels
Label Source
Provenance
Propagation
Policy Effects
Declassification History
```

Die Introspection selbst muss Data Minimization und bestehende Zugriffsbeschränkungen beachten.

---

## Normative Anforderungen

1. NovaOS MUSS Semantic Data Labels als native Datenmetadaten unterstützen.
2. Semantic Data Labels MÜSSEN von Semantic Types getrennt modelliert werden.
3. Labels MÜSSEN als Eingaben für Privacy-, Security-, IFC- und Sovereignty-Policies verwendbar sein.
4. Labels DÜRFEN selbst keine Zugriffs-Authority erzeugen.
5. Schutzrelevante Labels MÜSSEN über relevante Datenbewegungen erhalten bleiben.
6. Label Propagation MUSS für abgeleitete Daten definiert werden können.
7. Kopieren oder Formatkonvertierung DARF Schutzlabels NICHT implizit entfernen.
8. Declassification MUSS eine explizit autorisierte Operation sein.
9. Capability Contracts MÜSSEN Label-Anforderungen und Label-Transformationen ausdrücken können.
10. Data Minimization und Retention MÜSSEN Semantic Data Labels verwenden können.
11. Labels MÜSSEN mit Data Provenance kombinierbar sein.
12. NovaOS MUSS Herkunft und Vertrauenswürdigkeit sicherheitsrelevanter Labels berücksichtigen können.
13. Untrusted Labels DÜRFEN nicht ungeprüft als Grundlage sicherheitskritischer Freigaben verwendet werden.
14. Semantic Data Labels und ihre relevanten Policy-Auswirkungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Privacy-Anforderungen werden maschinenlesbar,
- Daten behalten ihre Schutzsemantik über Systemgrenzen hinweg,
- Information Flow und Data Sovereignty können datenbezogen entscheiden,
- Capability Provider können anhand von Datenschutzanforderungen ausgewählt werden,
- Data Minimization und Retention erhalten semantischen Kontext,
- abgeleitete Daten können ihre Schutzanforderungen automatisch weitertragen.

### Negative Konsequenzen

- Label-Semantik und Propagation müssen präzise standardisiert werden,
- falsche oder veraltete Labels können zu falschen Policy-Entscheidungen führen,
- Declassification und Label-Vertrauen benötigen zusätzliche Sicherheitsmechanismen.

---

## Verworfene Alternativen

### Datenschutz ausschließlich aus dem Dateityp ableiten

Verworfen.

Gleiche Datentypen können völlig unterschiedliche Schutzanforderungen besitzen.

### Labels nur als Anwendungstags behandeln

Verworfen.

Privacy-, IFC- und Sovereignty-Entscheidungen benötigen systemweit verständliche Semantik.

### Labels bei jeder Transformation entfernen und neu bestimmen

Verworfen.

Dadurch würden Provenance und bestehende Schutzanforderungen unnötig verloren gehen.

### Anwendungen dürfen Labels beliebig entfernen

Verworfen.

Dies würde Information-Flow- und Privacy-Policies leicht umgehbar machen.

---

## Abhängigkeiten

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0002_Data_Retention_und_Expiration`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-STORAGE-0006_Semantic_und_Typed_Storage`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-SEMANTIC-LABEL-0001`
- `NPSPEC-PRIVACY-LABEL-TAXONOMY-0001`
- `NPSPEC-PRIVACY-LABEL-PROPAGATION-0001`
- `NPSPEC-PRIVACY-LABEL-DECLASSIFICATION-0001`
- `NPSPEC-PRIVACY-LABEL-TRUST-0001`
- `NPSPEC-PRIVACY-LABEL-SERIALIZATION-0001`
- `NPSPEC-PRIVACY-SEMANTIC-LABEL-TEST-0001`

---

## Ergebnis

NovaOS ergänzt Semantic Types um maschinenlesbare Semantic Data Labels:

```text
Data
 ├── Semantic Type
 ├── Semantic Data Labels
 ├── Provenance
 └── Lifecycle
        ↓
Privacy / Security / IFC / Sovereignty
        ↓
Controlled Processing and Data Flow
```

Damit kann die Schutzsemantik eines Datenobjekts Bestandteil seines gesamten Lifecycles werden, anstatt ausschließlich vom Speicherort oder von einzelnen Anwendungen abhängig zu sein.