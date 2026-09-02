# ADR-SECURITY-0001 – Capability-Based Security als zentrales Autorisierungsmodell

## Status

Angenommen

## Kategorie

Security / Authorization / Capabilities / Nova.Trust

## Kontext

Klassische Betriebssysteme verwenden häufig mehrere voneinander getrennte Autorisierungsmodelle:

- Benutzer- und Gruppenrechte,
- ACLs,
- Prozessrechte,
- Dateirechte,
- Netzwerkregeln,
- privilegierte Systemaufrufe,
- Anwendungsberechtigungen,
- Service-spezifische Tokens.

Dadurch entsteht eine fragmentierte Sicherheitsarchitektur, in der die tatsächliche Authority eines Prozesses oder Dienstes nur schwer vollständig bestimmbar ist.

Zusätzlich sind klassische Identitätsmodelle häufig zu grob.

```text
Identity
   ↓
"Who are you?"
```

beantwortet nicht automatisch:

```text
"What exactly are you allowed to do?"
```

Für NovaOS sollen Anwendungen, Dienste, Treiber, KI-Agenten und verteilte Komponenten nur genau die Authority erhalten, die sie für ihre konkrete Aufgabe benötigen.

NovaOS benötigt deshalb ein einheitliches Autorisierungsmodell auf Basis expliziter Capabilities.

---

## Entscheidung

NovaOS verwendet **Capability-Based Security als zentrales Autorisierungsmodell**.

Eine Capability ist eine systemverifizierbare Authority-Referenz auf eine bestimmte zulässige Aktion oder Ressource.

Konzeptionell:

```text
Subject
   ↓
Capability
   ↓
Target
   ↓
Authorized Operation
```

Ohne geeignete Capability darf eine geschützte Operation nicht ausgeführt werden.

```text
Reachability
    ≠
Authority

Identity
    ≠
Authority

Knowledge
    ≠
Authority
```

Identität, Trust und Capabilities bleiben getrennte, aber miteinander kombinierbare Konzepte.

---

## Grundprinzip

```text
No authority without an explicit capability.
```

und:

```text
Possession does not imply unlimited authority.
```

---

## Capability als Authority

Eine Capability beschreibt explizit, welche Authority einem Holder zur Verfügung steht.

Konzeptionell kann sie enthalten beziehungsweise referenzieren:

```text
Capability
 ├── Target
 ├── Allowed Operations
 ├── Constraints
 ├── Holder Binding
 ├── Validity
 ├── Delegation Rules
 └── Audit Context
```

Die konkrete Repräsentation wird in NPSPECs definiert.

---

## Least Authority

NovaOS folgt dem Principle of Least Authority.

```text
Required Authority
        ↓
Minimal Capability Set
```

Eine Komponente soll nur die Capabilities erhalten, die zur Erfüllung ihrer aktuellen Aufgabe notwendig sind.

Breite globale Privilegien werden nicht als Standardmechanismus verwendet.

---

## Capability statt globalem Privileg

NovaOS vermeidet Autorisierung nach dem Muster:

```text
Process is privileged
      ↓
Can do almost everything
```

Stattdessen:

```text
Process
 ├── Capability A
 ├── Capability B
 └── Capability C
```

Eine Komponente kann dadurch für bestimmte Systemoperationen autorisiert und für alle anderen nicht autorisiert sein.

---

## Identity und Capability

Identity beantwortet:

```text
Who is this?
```

Capability beantwortet:

```text
What may this entity do?
```

Eine bekannte oder authentisierte Identity erzeugt keine automatische Authority.

```text
Authenticated
    ≠
Authorized
```

Identity kann jedoch für:

- Capability Issuance,
- Holder Binding,
- Audit,
- Trust Evaluation

verwendet werden.

---

## Trust und Capability

Nova.Trust bewertet die Vertrauenswürdigkeit einer Entität oder eines Artefakts.

Capability bestimmt die konkrete Authority.

```text
Trust
    ≠
Authority
```

Ein stark vertrauenswürdiger Service besitzt nicht automatisch beliebige Capabilities.

Ebenso kann eine begrenzt vertrauenswürdige Komponente eine eng eingeschränkte Capability erhalten.

---

## Capability Targets

Capabilities können auf unterschiedliche NovaOS-Objekte und Systemressourcen verweisen.

Beispiele:

```text
File Object
Storage Object
Device
Service
Process
Execution Domain
Network Service
Capability
System Configuration
User Data
Remote Service
```

Das Capability-Modell ist damit nicht auf Dateisystemrechte beschränkt.

---

## Operation-spezifische Authority

Capabilities sollen möglichst operation-spezifisch sein.

Beispielsweise:

```text
File
 ├── Read
 ├── Write
 ├── Append
 ├── Delete
 └── Share
```

Eine `Read`-Capability impliziert keine `Write`-Capability.

```text
Read
    ≠
Write
```

---

## Capability Constraints

Capabilities können zusätzliche Einschränkungen besitzen.

Beispielsweise:

- Zeitfenster,
- Resource Budget,
- Zielbereich,
- Datenklassifikation,
- erlaubte Operationen,
- Call Count,
- Data-Sovereignty Constraints,
- Trust Requirements,
- Delegation Depth.

Damit kann Authority präzise eingeschränkt werden.

---

## Capability Delegation

Capabilities können entsprechend ihrer Policy delegierbar sein.

```text
Original Capability
       ↓
Delegation
       ↓
Derived Capability
```

Dabei gilt:

```text
Authority(Derived)
    ⊆
Authority(Original)
```

Delegation darf Authority niemals erweitern.

---

## Attenuation

NovaOS unterstützt Capability Attenuation.

Eine delegierte oder abgeleitete Capability kann enger sein als ihre Quelle.

Beispiel:

```text
Original:
Read + Write
All Documents
Unlimited Duration

Derived:
Read
Document X
10 Minutes
```

Attenuation ist ein grundlegender Mechanismus für Least Authority.

---

## Non-Delegatable Capabilities

Nicht jede Capability muss delegierbar sein.

Capabilities können konzeptionell beispielsweise klassifiziert werden als:

```text
NonDelegatable
RestrictedDelegation
Delegatable
```

Die konkrete Semantik wird in NPSPECs definiert.

---

## Holder Binding

Capabilities können an einen bestimmten Holder oder Kontext gebunden sein.

Beispiele:

- Identity,
- Execution Domain,
- Service,
- Task Group,
- Session,
- Transaction.

Dadurch kann verhindert werden, dass eine Capability außerhalb ihres vorgesehenen Kontextes verwendet wird.

---

## Capability Lifetime

Capabilities besitzen eine explizite Lebensdauer.

Sie können beispielsweise an folgende Lebenszyklen gekoppelt sein:

```text
Process Lifetime
Execution Domain Lifetime
Task Group Lifetime
Session Lifetime
Transaction Lifetime
Explicit Expiration
```

Eine abgelaufene Capability darf keine Authority mehr verleihen.

---

## Revocation

NovaOS muss Capabilities widerrufen können, wenn ihre Semantik dies erfordert.

```text
Capability
   ↓
Revoked
   ↓
No Authority
```

Revocation darf nicht voraussetzen, dass jede physische Kopie der Capability zurückgeholt werden kann.

Die konkrete Revocation-Architektur wird in NPSPECs definiert.

---

## Capability Validation

Vor Verwendung einer Capability muss ihr relevanter Zustand überprüfbar sein.

Konzeptionell:

```text
Capability
   ↓
Validation
   ↓
Valid?
 ├── Yes → Evaluate Constraints
 └── No  → Deny
```

Zustände können beispielsweise sein:

```text
Valid
Expired
Revoked
Invalid
Unknown
```

Für sicherheitskritische Entscheidungen gilt:

```text
Unknown
    ≠
Allowed
```

---

## Fail-Safe Default

Fehlt eine erforderliche Capability oder kann ihre Gültigkeit nicht ausreichend bestimmt werden, wird die Operation abgelehnt.

```text
Capability available and valid
    → Evaluate

Missing / Invalid / Revoked / Unknown
    → Deny
```

NovaOS verwendet keine implizite Autorisierung aufgrund fehlender Sicherheitsinformationen.

---

## Capability Discovery

Eine Komponente darf nicht automatisch sämtliche verfügbaren Capabilities des Systems entdecken oder enumerieren können.

Capability Discovery selbst kann Authority benötigen.

Damit wird verhindert, dass bereits die Kenntnis aller vorhandenen Ressourcen unnötig offengelegt wird.

---

## Capability Transfer

Capability Transfer zwischen Execution Domains muss explizit erfolgen.

```text
Domain A
   ↓
Authorized Transfer
   ↓
Domain B
```

Ein normaler Datenkanal darf nicht automatisch sämtliche Authority des Senders übertragen.

---

## Confused Deputy Protection

NovaOS unterscheidet zwischen:

```text
Service Own Authority
```

und:

```text
Delegated Caller Authority
```

Ein Service darf nicht unbeabsichtigt seine stärkeren eigenen Rechte verwenden, um eine Operation im Auftrag eines weniger privilegierten Callers auszuführen.

```text
Caller
  ↓
Delegated Capability
  ↓
Service
  ↓
Operation using delegated authority
```

Dies ist insbesondere für Systemdienste und Distributed Services relevant.

---

## Capability und IPC

IPC-Aufrufe verwenden Capabilities für geschützte Operationen.

```text
Caller
   ↓
Typed IPC
   +
Capability
   ↓
Target Service
```

Das Erreichen eines IPC-Endpunkts allein erzeugt keine Authority.

---

## Capability und Nova.Call

`Nova.Call` integriert Capability Authority direkt in den Call Contract.

Konzeptionell:

```text
Nova.Call
 ├── Target
 ├── Operation
 ├── Capability
 └── Execution Contract
```

Lokale und Remote Calls verwenden damit dasselbe grundlegende Autorisierungsmodell.

---

## Remote Capabilities

Capabilities können über System- und Netzwerkgrenzen hinweg verwendet werden, wenn ihre Repräsentation und Validierung dies zulassen.

Remote Capabilities behalten dieselben grundlegenden Eigenschaften:

- Least Authority,
- Delegation,
- Attenuation,
- Revocation,
- Holder Binding,
- Validity.

Transport- oder Standortwechsel verändern ihre Authority nicht.

---

## Capability und Netzwerk

Netzwerkzugriff kann ebenfalls Capability-gesteuert sein.

Beispielsweise:

```text
Connect to Service X
Publish Event Y
Open Network Listener Z
Use VPN Provider A
```

IP-Adressen oder Ports sind keine Authority.

---

## Capability und Geräte

Hardwarezugriff erfolgt capability-basiert.

Ein Treiber oder Prozess kann beispielsweise Capabilities für:

```text
Device Control
DMA Mapping
Interrupt Registration
Firmware Operation
```

erhalten.

Der Besitz einer Device-Referenz allein reicht nicht aus.

---

## Capability und Storage

Storage-Zugriffe werden auf logische Storage- und Dateiobjekte abgebildet.

Beispielsweise:

```text
Read Object
Modify Metadata
Create Version
Delete Object
Share Object
```

Capability-basierte Authority ergänzt beziehungsweise ersetzt grobe globale Dateirechte für native NovaOS-Komponenten.

---

## Capability und Systemkonfiguration

Änderungen an kritischen Systemzuständen benötigen explizite Capabilities.

Beispiele:

```text
Modify Global System
Install Driver
Change Boot Configuration
Modify Security Policy
Create Execution Domain
```

Dies gilt insbesondere für Änderungen am globalen `/System`.

---

## Application SYS Overlay

Private Anwendungsabhängigkeiten im per-Application `SYS`-Namespace benötigen keine globale System-Modification-Authority.

```text
Application SYS Overlay
    → application-scoped authority

Global /System
    → elevated explicit capability
```

Damit bleibt die Isolation zwischen privatem Application State und globalem Systemzustand erhalten.

---

## Capability und KI-Agenten

KI-Agenten erhalten keine impliziten Benutzerrechte.

Ein Agent handelt ausschließlich innerhalb explizit bereitgestellter Capabilities.

```text
Agent
 ├── Read selected documents
 ├── Create draft
 └── No delete authority
```

Ein Modelloutput allein darf keine zusätzliche Authority erzeugen.

---

## Capability und Benutzerinteraktion

Benutzerentscheidungen können zur gezielten Capability-Erteilung führen.

Beispielsweise:

```text
Application requests access
        ↓
User / Policy Decision
        ↓
Capability issued
```

Die eigentliche Runtime-Autorisierung basiert danach auf der Capability und nicht darauf, dass eine UI-Abfrage irgendwann zuvor stattgefunden hat.

---

## Capability und Data Sovereignty

Eine Capability kann eine Operation autorisieren, ohne gleichzeitig jede mögliche Datenbewegung zu erlauben.

```text
Operation Authority
      +
Information Flow Permission
      +
Sovereignty Constraints
      =
Executable Operation
```

Capabilities dürfen Data-Sovereignty- oder Information-Flow-Policies nicht überschreiben.

---

## Capability und Resource Economy

Capabilities können Resource Budgets begrenzen oder mit ihnen verbunden werden.

Beispiele:

```text
GPU Time
Network Bandwidth
Pinned Memory
Storage Capacity
CPU Budget
```

Authority zur Nutzung einer Ressource bedeutet nicht automatisch unbegrenzte Ressourcennutzung.

---

## Capability Namespaces und IDs

Capability Types beziehungsweise Capability IDs verwenden die NovaOS-weite Identitätsstruktur:

```text
domain.authority.namespace.name
```

Beispiel:

```text
de.nova.image.filter.gaussian
```

Dabei gilt:

```text
de       = Domain
nova     = Authority
image.filter = Namespace
gaussian = Name
```

Der physische Pfad oder Kategorieordner bestimmt nicht die Identität einer Capability.

---

## Capability Type und Capability Instance

NovaOS unterscheidet zwischen:

```text
Capability Type
```

und:

```text
Capability Instance
```

Ein Capability Type beschreibt die semantische Art einer Authority.

Eine Capability Instance repräsentiert eine konkrete gewährte Authority für einen bestimmten Target- und Constraint-Kontext.

---

## Capability Composition

Komplexe Operationen können mehrere Capabilities benötigen.

```text
Operation
 ├── Capability A
 ├── Capability B
 └── Capability C
```

Mehrere Capabilities dürfen jedoch nicht implizit zu einer stärkeren, nicht ausdrücklich definierten Authority verschmolzen werden.

---

## No Ambient Authority

Native NovaOS-Komponenten sollen keine unnötige **Ambient Authority** besitzen.

Authority wird explizit über:

- Capability Parameter,
- Execution Context,
- Call Context,
- kontrollierte Capability Stores

bereitgestellt.

Eine globale implizite Umgebung mit umfangreichen Rechten wird vermieden.

---

## Privilege Elevation

NovaOS kann erhöhte Authority bereitstellen, aber nicht als pauschalen dauerhaften Superuser-Modus.

Stattdessen sollen zusätzliche Capabilities gezielt und möglichst zeitlich beziehungsweise funktional begrenzt erteilt werden.

```text
Need elevated operation
        ↓
Authorization
        ↓
Temporary Capability
        ↓
Operation
        ↓
Capability expires/revoked
```

---

## Legacy Compatibility

Legacy-Anwendungen können klassische:

- Benutzerrechte,
- ACLs,
- POSIX-artige Berechtigungen,
- Windows-artige Security Semantics

über Compatibility Domains verwenden.

Diese werden intern soweit möglich auf kontrollierte Capability-Authority abgebildet.

Legacy-Sicherheitsmodelle bilden jedoch nicht die native Sicherheitsgrundlage von NovaOS.

---

## Audit

Capability-relevante Sicherheitsentscheidungen müssen kontrolliert auditierbar sein.

Beispielsweise:

```text
Subject
 ├── Capability Type
 ├── Target
 ├── Requested Operation
 ├── Decision
 └── Reason
```

Geheime Capability-Repräsentationen oder kryptografische Materialien dürfen dabei nicht unnötig offengelegt werden.

---

## Architecture Introspection

NovaOS muss nachvollziehbar darstellen können, warum eine Komponente eine bestimmte Operation ausführen darf.

Konzeptionell:

```text
Execution Domain
   ↓
Capability
   ↓
Target
   ↓
Allowed Operation
   ↓
Constraints
```

Dies ermöglicht systemweite Analyse von:

- Authority,
- Delegation,
- Revocation,
- Capability Chains,
- Least-Authority-Verstößen.

---

## Deterministic Mode

Capability Validation und Authorization bleiben auch im Deterministic Mode verbindlich.

Adaptive Policy-Systeme dürfen Authority nicht dynamisch erweitern.

Determinismus kann beeinflussen, welche bereits autorisierten Ausführungsalternativen gewählt werden, nicht welche Authority existiert.

---

## Adaptive Systeme

NovaOS darf adaptive Systeme verwenden, um beispielsweise sinnvolle Capability-Anfragen oder Least-Authority-Vorschläge zu erzeugen.

Prediction Error kann solche Empfehlungen verbessern.

Ein adaptives Modell darf jedoch niemals selbständig Hard Security Authority erzeugen oder erweitern.

```text
Prediction
    → Recommendation

Policy / User / Authority System
    → Actual Capability
```

---

## Normative Anforderungen

1. NovaOS MUSS Capability-Based Security als zentrales natives Autorisierungsmodell verwenden.
2. Geschützte Operationen MÜSSEN eine geeignete Authority erfordern.
3. Identity DARF NICHT automatisch Authority erzeugen.
4. Trust DARF NICHT automatisch Authority erzeugen.
5. Reachability DARF NICHT automatisch Authority erzeugen.
6. Native Komponenten SOLLEN nach dem Principle of Least Authority ausgeführt werden.
7. Capabilities MÜSSEN operation-spezifische Authority ausdrücken können.
8. Capabilities MÜSSEN zusätzliche Constraints unterstützen können.
9. Capability Delegation DARF Authority NICHT erweitern.
10. Capability Attenuation MUSS unterstützt werden können.
11. Nicht delegierbare Capabilities MÜSSEN unterstützt werden können.
12. Capabilities MÜSSEN an Holder beziehungsweise Execution Contexts bindbar sein können.
13. Capabilities MÜSSEN explizite Lifetime- und Expiration-Semantik unterstützen können.
14. Revocation MUSS für geeignete Capability Types unterstützt werden können.
15. Missing, Invalid, Revoked, Expired oder nicht ausreichend validierbare Capabilities MÜSSEN fail-safe behandelt werden.
16. Capability Transfer zwischen Execution Domains MUSS explizit kontrolliert werden.
17. Vollständige Caller Authority DARF NICHT implizit über IPC- oder Network-Grenzen übertragen werden.
18. NovaOS MUSS Mechanismen gegen Confused-Deputy-Probleme unterstützen.
19. IPC MUSS mit Capability-basierter Authorization integrierbar sein.
20. `Nova.Call` MUSS Capability Authority in seinen Call Context integrieren können.
21. Remote Capabilities MÜSSEN dieselben grundlegenden Authority-Regeln wie lokale Capabilities verwenden.
22. Device-, Storage-, Netzwerk- und Systemoperationen MÜSSEN capability-basiert autorisierbar sein.
23. Globale Systemänderungen MÜSSEN explizit erhöhte Capabilities erfordern können.
24. Private Application-`SYS`-Overlays DÜRFEN NICHT automatisch globale System-Authority erhalten.
25. KI-Agenten DÜRFEN KEINE implizite Benutzer- oder Systemauthority besitzen.
26. Capability Authority DARF Data-Sovereignty-, Information-Flow- oder Security-Hard-Constraints NICHT überschreiben.
27. Authority zur Ressourcennutzung DARF NICHT automatisch unbegrenzte Resource Budgets implizieren.
28. Capability IDs MÜSSEN die NovaOS-weite Capability-ID-Struktur verwenden.
29. Capability Type und konkrete Capability Instance MÜSSEN unterscheidbar sein.
30. Native NovaOS-Komponenten SOLLEN Ambient Authority minimieren.
31. Privilege Elevation SOLL bevorzugt über begrenzte zusätzliche Capabilities erfolgen.
32. Legacy-Sicherheitsmodelle DÜRFEN als Compatibility Layer unterstützt werden, bilden aber NICHT das native zentrale Autorisierungsmodell.
33. Sicherheitsrelevante Capability-Entscheidungen MÜSSEN kontrolliert auditierbar sein.
34. Authority-, Delegation- und Capability-Beziehungen MÜSSEN kontrolliert introspektierbar sein.
35. Adaptive Systeme DÜRFEN Hard Security Authority NICHT selbständig erzeugen oder erweitern.

---

## Konsequenzen

### Positive Konsequenzen

- ein einheitliches Autorisierungsmodell für lokale und verteilte Systemkomponenten,
- präzise Least-Authority-Zuweisung,
- geringere Abhängigkeit von globalen privilegierten Prozessen,
- natürliche Integration mit IPC und `Nova.Call`,
- sichere Delegation zwischen Services und KI-Agenten,
- klare Trennung von Identity, Trust und Authority,
- bessere Isolation von Treibern und Systemdiensten,
- feinere Autorisierung als klassische Benutzer-/Gruppenmodelle,
- starke Grundlage für Architecture Introspection und Audit.

### Negative Konsequenzen

- Capability Lifetime, Delegation und Revocation benötigen zusätzliche Infrastruktur,
- APIs müssen Authority expliziter modellieren,
- Compatibility mit klassischen ACL- und Benutzerrechtsmodellen benötigt Übersetzungsschichten,
- unsauber definierte Capability Types können zu unnötig breiter Authority führen.

---

## Verworfene Alternativen

### Klassische Benutzer- und Gruppenrechte als zentrales Modell

Verworfen.

Sie sind zu grob für moderne komponentenbasierte, verteilte und agentische Systeme.

### ACLs als einziges Autorisierungsmodell

Verworfen.

ACLs sind nützlich für bestimmte Ressourcen, bilden aber keine ausreichende Grundlage für allgemeine Service-, Geräte-, IPC- und Distributed Authority.

### Globaler Superuser als primärer Privilegmechanismus

Verworfen.

Pauschale globale Authority widerspricht Least Authority und vergrößert die Auswirkungen kompromittierter Prozesse.

### Trust gleich Authority

Verworfen.

Vertrauenswürdigkeit und konkrete Berechtigung sind unterschiedliche Sicherheitsdimensionen.

### Netzwerkposition als Autorisierung

Verworfen.

IP-Adresse, Subnetz oder Erreichbarkeit stellen keine stabile Authority dar.

### Implizite Authority durch Prozessherkunft

Verworfen.

Die Tatsache, dass ein Prozess von einem privilegierten Prozess gestartet wurde, darf nicht automatisch unbegrenzte Authority übertragen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0017_Composability_und_Orthogonality`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Prerequisite`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`
- `ADR-DISTCOMM-0013_Integrierte_Service_Mesh_Funktionen`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-STORAGE-0018_Per_Application_System_Overlay`
- `ADR-STORAGE-0019_Private_Systemabhängigkeiten_im_Application_SYS_Namespace`
- `ADR-STORAGE-0020_Globales_System_nur_mit_erhöhter_Berechtigung`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-CAPABILITY-TYPE-0001`
- `NPSPEC-SECURITY-CAPABILITY-INSTANCE-0001`
- `NPSPEC-SECURITY-CAPABILITY-VALIDATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-DELEGATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-HOLDER-BINDING-0001`
- `NPSPEC-SECURITY-CAPABILITY-LIFETIME-0001`
- `NPSPEC-SECURITY-CAPABILITY-AUDIT-0001`
- `NPSPEC-SECURITY-CAPABILITY-TEST-0001`

---

## Ergebnis

Capability-Based Security bildet die zentrale Autorisierungsgrundlage von NovaOS:

```text
                        Subject
                           ↓
                    Identity / Trust
                           ↓
                     Capability Set
                           ↓
                Requested Operation
                           ↓
                 Capability Validation
                           ↓
            Target + Operation + Constraints
                           ↓
                ┌──────────┴──────────┐
                ↓                     ↓
             Denied                Authorized
                                      ↓
                                 Execution
                                      ↓
                                    Audit
```

Damit beantwortet NovaOS Sicherheitsentscheidungen nicht mit der groben Frage:

```text
"Is this process privileged?"
```

sondern mit der präziseren Frage:

```text
"Does this subject possess valid authority for this exact operation
on this exact target under the current constraints?"
```

Identity, Trust, Data Sovereignty und Resource Policies ergänzen diese Entscheidung, ersetzen jedoch nicht die Capability-basierte Authority.

Damit erhält NovaOS ein einheitliches, delegierbares, begrenzbares und introspektierbares Autorisierungsmodell für Anwendungen, Systemdienste, Treiber, Geräte, verteilte Services und KI-Agenten.