# ADR-PRIVACY-0005 – Selective Disclosure

## Status

Angenommen

## Kategorie

Privacy Architecture / Data Sharing / Information Flow

## Kontext

Viele Operationen benötigen nicht ein vollständiges Datenobjekt, sondern nur einzelne Eigenschaften daraus.

Ein klassisches Berechtigungsmodell führt häufig dazu, dass ein Consumer entweder Zugriff auf das gesamte Objekt erhält oder gar keinen Zugriff.

Beispiele:

```text
Identity
 ├── Name
 ├── Address
 ├── Birth Date
 └── Account Data

Contact
 ├── Name
 ├── Email
 ├── Phone
 └── Address

Photo
 ├── Pixels
 ├── Location
 ├── Timestamp
 └── Device Metadata
```

Benötigt eine Capability lediglich einen Teil dieser Informationen, würde die Weitergabe des vollständigen Objekts dem NovaOS-Prinzip der Data Minimization widersprechen.

NovaOS benötigt deshalb eine native Möglichkeit, gezielt nur die tatsächlich erforderlichen Informationen offenzulegen.

---

## Entscheidung

NovaOS unterstützt **Selective Disclosure als native Eigenschaft des Daten- und Capability-Modells**.

Ein Consumer kann eine begrenzte Sicht auf ein Datenobjekt erhalten, ohne Zugriff auf das vollständige Objekt zu bekommen.

```text
Complete Object
      ↓
Disclosure Policy
      ↓
Selective View
      ↓
Consumer
```

Selective Disclosure soll bevorzugt durch semantisch definierte Views, Claims oder abgeleitete Datenobjekte erfolgen.

---

## Grundprinzip

```text
Disclose what is required.

Do not disclose the complete source by default.
```

Authority auf einen Teil eines Objekts darf nicht automatisch Authority auf das vollständige Objekt erzeugen.

---

## Selective Views

NovaOS kann aus einem Datenobjekt eine begrenzte semantische Sicht bereitstellen.

Beispiel:

```text
Contact
 ├── Name
 ├── Email
 ├── Phone
 └── Address
        ↓
Selective Disclosure
        ↓
ContactView
 ├── Name
 └── Email
```

Der Consumer erhält keine implizite Möglichkeit, die ausgeblendeten Eigenschaften nachzuladen.

---

## Claims statt Rohdaten

Wenn eine Operation lediglich eine Eigenschaft bestätigen muss, soll NovaOS möglichst nicht die zugrunde liegenden Rohdaten offenlegen.

Beispiel:

```text
Birth Date
    ↓
Verification
    ↓
Claim: Age >= 18
```

statt:

```text
Birth Date → Consumer
```

Dadurch können Anwendungen Entscheidungen treffen, ohne unnötige personenbezogene Informationen zu erhalten.

---

## Capability Contracts

Capability Contracts können deklarieren, welche Daten tatsächlich benötigt werden.

```text
Capability
    ↓
Required Semantic Inputs
    ↓
Disclosure Planning
    ↓
Minimum Required View
```

Die Capability darf daraus keine weitergehende Authority auf das Ursprungsobjekt ableiten.

---

## Semantic Data Labels

Selective Disclosure wird mit Semantic Data Labels integriert.

Ein Objekt kann beispielsweise enthalten:

```text
Photo
 ├── Pixels
 ├── Location [PersonalData]
 └── Device ID [DeviceIdentifier]
```

Eine Bildverarbeitungs-Capability kann ausschließlich eine Pixel-View erhalten.

Schutzrelevante Metadaten bleiben außerhalb ihres Disclosure Contexts.

---

## Disclosure Authority

Die Freigabe bestimmter Informationen ist eine eigenständige autorisierte Operation.

```text
Source Authority
      +
Disclosure Policy
      ↓
Authorized View
```

Die Authority zum Lesen eines Objekts bedeutet nicht automatisch Authority, beliebige Teile davon an andere Komponenten weiterzugeben.

Capability Delegation und Attenuation können verwendet werden, um Disclosure-Rechte weiter einzuschränken.

---

## Abgeleitete Informationen

Auch abgeleitete Informationen können schützenswert sein.

Beispielsweise:

```text
Birth Date
    ↓
Age Calculation
    ↓
Exact Age
```

kann mehr Information offenlegen als erforderlich.

Wenn lediglich eine Altersgrenze relevant ist, soll bevorzugt nur das notwendige Ergebnis bereitgestellt werden:

```text
Age >= 18
```

Selective Disclosure gilt deshalb sowohl für gespeicherte als auch für berechnete Informationen.

---

## Privacy-Preserving Proofs

Die Architektur muss Verfahren unterstützen können, bei denen eine Eigenschaft nachgewiesen wird, ohne die zugrunde liegenden Daten vollständig offenzulegen.

Konzeptionell:

```text
Protected Data
      ↓
Proof / Attestation
      ↓
Verified Claim
```

Mögliche kryptographische Verfahren werden nicht in dieser ADR festgelegt und können durch Crypto Provider bereitgestellt werden.

---

## Lokale und entfernte Disclosure

Selective Disclosure gilt unabhängig vom Ausführungsort.

```text
Local Consumer
IPC Consumer
Nova.Call Consumer
Remote Service
```

Bei Remote Disclosure gelten zusätzlich:

- Data Sovereignty,
- Information Flow Control,
- Trust,
- Transport Security,
- DLP.

Remote-Ausführung darf nicht automatisch einen größeren Datenumfang erhalten als lokale Ausführung.

---

## Disclosure und Provenance

Eine selektiv offengelegte Information kann relevante Provenance behalten.

Beispielsweise kann ein verifizierter Claim enthalten:

```text
Claim
Issuer
Verification Context
Validity
Trust Information
```

ohne das vollständige Ursprungsobjekt offenzulegen.

Provenance darf jedoch selbst keine unnötigen sensiblen Informationen preisgeben.

---

## Disclosure Lifecycle

Selektiv bereitgestellte Views oder Claims können einen eigenen begrenzten Lifecycle besitzen.

```text
Source Object
      ↓
Selective Disclosure
      ↓
Temporary View
      ↓
Expiration
```

Sie können an:

```text
Operation
Nova.Call
Task Group
Session
Execution Domain
Deadline
```

gebunden werden.

Damit muss eine einmalige Freigabe nicht automatisch zu dauerhaftem Datenzugriff führen.

---

## Zusammenspiel mit DLP

Selective Disclosure kann als kontrollierte Alternative zur vollständigen Blockierung eines Datenflusses dienen.

```text
Requested Full Disclosure
        ↓
DLP Policy
        ↓
Full Disclosure denied
        ↓
Selective Disclosure allowed
```

Eine solche Reduktion darf nur erfolgen, wenn der verbleibende Datenfluss weiterhin alle Hard Constraints erfüllt.

---

## Kein implizites Reassembly

Mehrere Selective Disclosures dürfen nicht unbeachtet zu einer Rekonstruktion des vollständigen geschützten Objekts führen.

NovaOS-Policies müssen deshalb bei sensitiven Daten auch kumulative Disclosure berücksichtigen können.

```text
Disclosure A
     +
Disclosure B
     +
Disclosure C
     ↓
Possible Information Reconstruction
```

Die konkreten Regeln für Disclosure Budgets und Korrelation werden in NPSPECs definiert.

---

## KI und Agenten

KI-Systeme und autonome Agenten unterliegen denselben Selective-Disclosure-Regeln.

Ein Agent soll bevorzugt genau die Informationen erhalten, die für seine aktuelle Aufgabe erforderlich sind.

```text
Agent Task
    ↓
Required Information
    ↓
Selective View
```

Die Fähigkeit eines Agenten, weitere Daten anzufordern, erzeugt keine automatische Authority auf diese Daten.

---

## Introspection und Audit

NovaOS soll kontrolliert nachvollziehbar machen können:

```text
Source Object
Requested Information
Released Information
Recipient
Purpose
Authority
Disclosure Policy
Lifetime
Derived Claims
```

Dabei dürfen nicht freigegebene Daten selbst nicht durch Audit oder Introspection sichtbar werden.

---

## Normative Anforderungen

1. NovaOS MUSS Selective Disclosure als native Privacy-Funktion unterstützen.
2. Datenobjekte MÜSSEN begrenzte semantische Views bereitstellen können.
3. Consumer DÜRFEN durch eine Selective View KEINE implizite Authority auf das vollständige Ursprungsobjekt erhalten.
4. Capability Contracts MÜSSEN den tatsächlich benötigten Datenumfang ausdrücken können.
5. NovaOS SOLL verifizierte Claims statt Rohdaten verwenden können, wenn dies für den Zweck ausreicht.
6. Semantic Data Labels MÜSSEN bei Disclosure-Entscheidungen berücksichtigt werden können.
7. Disclosure MUSS mit Capability-Based Security, IFC und DLP integriert sein.
8. Remote Disclosure MUSS zusätzlich Data Sovereignty und Trust berücksichtigen.
9. Selective Views und Claims MÜSSEN an begrenzte Lifecycles bindbar sein.
10. Abgeleitete Informationen MÜSSEN ebenfalls Data-Minimization- und Disclosure-Regeln unterliegen.
11. Privacy-Preserving Proofs MÜSSEN architektonisch integrierbar sein.
12. Sensitive Policies MÜSSEN kumulative Disclosure berücksichtigen können.
13. KI-Systeme und Agenten DÜRFEN Selective Disclosure NICHT umgehen.
14. Disclosure-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger unnötige Offenlegung personenbezogener Daten,
- Capability Provider erhalten nur benötigte Informationen,
- verifizierte Eigenschaften können Rohdaten ersetzen,
- Remote Services benötigen weniger vollständige Datensätze,
- Data Minimization wird praktisch durchsetzbar,
- Privacy-Preserving Credentials und Proof-Systeme können später integriert werden.

### Negative Konsequenzen

- Datenobjekte benötigen geeignete View- und Claim-Semantik,
- kumulative Offenlegung kann komplexe Policy-Entscheidungen erfordern,
- kryptographische Selective-Disclosure-Verfahren können zusätzliche Ressourcen benötigen.

---

## Verworfene Alternativen

### Ganzes Objekt oder kein Zugriff

Verworfen.

Dieses Modell führt zu unnötiger Datenoffenlegung.

### Selective Disclosure ausschließlich durch Anwendungen

Verworfen.

Systemdienste, Capabilities, Agenten und Remote Execution benötigen dieselben Schutzmechanismen.

### Rohdaten immer übertragen und Empfänger zur Minimierung verpflichten

Verworfen.

Nicht übertragene Daten können beim Empfänger nicht unnötig verarbeitet oder kompromittiert werden.

### Selective Disclosure ersetzt Autorisierung

Verworfen.

Selective Disclosure bestimmt den Umfang einer Offenlegung, erzeugt aber keine fehlende Authority.

---

## Abhängigkeiten

- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0002_Data_Retention_und_Expiration`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`
- `ADR-PRIVACY-0004_Data_Loss_Prevention_über_semantische_Informationen`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-SELECTIVE-DISCLOSURE-0001`
- `NPSPEC-PRIVACY-DATA-VIEW-0001`
- `NPSPEC-PRIVACY-CLAIM-0001`
- `NPSPEC-PRIVACY-DISCLOSURE-AUTHORITY-0001`
- `NPSPEC-PRIVACY-DISCLOSURE-LIFECYCLE-0001`
- `NPSPEC-PRIVACY-DISCLOSURE-BUDGET-0001`
- `NPSPEC-PRIVACY-SELECTIVE-DISCLOSURE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Datenfreigabe nicht als binäre Entscheidung zwischen vollständigem Zugriff und vollständiger Ablehnung:

```text
Protected Data
      ↓
Purpose + Authority + Policy
      ↓
Selective Disclosure
      ↓
Minimum Required View / Claim
      ↓
Consumer
```

Damit kann NovaOS gezielt Informationen, Eigenschaften oder verifizierte Claims bereitstellen, ohne unnötig das vollständige zugrunde liegende Datenobjekt offenzulegen.