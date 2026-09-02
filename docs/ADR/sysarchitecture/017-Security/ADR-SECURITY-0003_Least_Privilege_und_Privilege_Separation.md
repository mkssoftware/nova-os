# ADR-SECURITY-0003 – Least Privilege und Privilege Separation

## Status

Angenommen

## Kategorie

Security / Authorization / Least Privilege / Privilege Separation / Capability-Based Security

## Kontext

NovaOS verwendet Capability-Based Security als zentrales Autorisierungsmodell und verzichtet auf ein universelles Root-Privileg.

Damit allein ist jedoch noch nicht sichergestellt, dass Komponenten tatsächlich nur die Authority besitzen, die sie für ihre aktuelle Aufgabe benötigen.

Auch in einem Capability-System können zu breite Capability-Sets entstehen, beispielsweise wenn:

- Anwendungen mehr Authority erhalten als notwendig,
- Systemdienste mehrere unabhängige Aufgaben in einem privilegierten Prozess bündeln,
- Treiber unnötig große Geräte- oder Speicherbereiche kontrollieren,
- administrative Werkzeuge dauerhaft erhöhte Authority behalten,
- Child Processes unkontrolliert Authority erben,
- Services eigene Authority und delegierte Caller-Authority vermischen.

NovaOS benötigt deshalb zwei miteinander verbundene Architekturprinzipien:

```text
Least Privilege
        +
Privilege Separation
```

Least Privilege begrenzt die Menge vorhandener Authority.

Privilege Separation verhindert, dass unterschiedliche sensitive Authorities unnötig in derselben Komponente oder Sicherheitsdomäne konzentriert werden.

---

## Entscheidung

NovaOS verwendet **Least Privilege** und **Privilege Separation** als verbindliche systemweite Sicherheitsprinzipien.

Jede native Komponente soll nur die Authority erhalten, die sie für ihre konkrete Funktion und ihren aktuellen Ausführungskontext benötigt.

Unabhängige oder besonders sensitive Authorities sollen, soweit technisch sinnvoll, auf getrennte:

- Execution Domains,
- Services,
- Prozesse,
- Tasks,
- Capability Sets

verteilt werden.

Konzeptionell:

```text
Large Privileged Component
           ↓
      Decomposition
           ↓
 ┌─────────┼─────────┐
 ↓         ↓         ↓
Domain A  Domain B  Domain C
Cap A     Cap B     Cap C
```

Damit wird verhindert, dass die Kompromittierung einer einzelnen Komponente automatisch Zugriff auf alle zugehörigen Systemfunktionen ermöglicht.

---

## Grundprinzip

```text
Grant only what is needed.
Separate what does not need to coexist.
```

und:

```text
Authority follows responsibility.
```

---

## Least Privilege

Least Privilege bedeutet, dass eine Komponente nur die minimale Authority erhält, die zur Erfüllung ihrer definierten Aufgabe notwendig ist.

Dabei wird nicht gefragt:

```text
What might this component possibly need?
```

sondern:

```text
What authority is required for this exact responsibility?
```

Capabilities sollen entsprechend eng definiert werden.

---

## Least Authority statt nur Benutzerprivileg

Least Privilege wird in NovaOS nicht ausschließlich auf Benutzerkonten angewendet.

Es gilt unter anderem für:

- Anwendungen,
- Prozesse,
- Threads und Tasks,
- Execution Domains,
- Systemdienste,
- Treiber,
- Netzwerkdienste,
- KI-Agenten,
- administrative Werkzeuge,
- Recovery-Komponenten.

Damit wird Least Privilege zu einer systemweiten Eigenschaft und nicht lediglich zu einer Benutzerverwaltungsregel.

---

## Privilege Separation

Privilege Separation bedeutet, dass unterschiedliche sensitive Verantwortlichkeiten nicht ohne Notwendigkeit in derselben Security Domain zusammengeführt werden.

Beispielsweise:

```text
Monolithic Service
 ├── Parse untrusted data
 ├── Modify system configuration
 └── Access secret keys
```

soll soweit sinnvoll aufgeteilt werden in:

```text
Parser Domain
    ↓
Validated Result
    ↓
Configuration Service

Key Service
    ↓
Controlled Cryptographic Operation
```

Dadurch muss die exponierte Parsing-Komponente keine direkte Authority über kritische Systemkonfiguration oder Schlüssel besitzen.

---

## Separation nach Verantwortlichkeit

Privilege Separation orientiert sich an fachlichen und sicherheitsrelevanten Verantwortlichkeiten.

Typische Trennungsgrenzen sind:

```text
Parsing
Policy Decision
Privileged Execution
Secret Handling
Device Access
Network Access
Persistent Storage
User Interaction
```

Nicht jede Funktion benötigt eine eigene Execution Domain.

Die Trennung soll dort erfolgen, wo dadurch die Sicherheitsgrenze wesentlich verbessert wird.

---

## Keine künstliche Maximalfragmentierung

Privilege Separation bedeutet nicht:

```text
one tiny process for every function
```

Eine übermäßige Fragmentierung würde:

- Kommunikation erhöhen,
- Ressourcen verschwenden,
- Komplexität steigern,
- Debugging erschweren,
- zusätzliche Fehlergrenzen erzeugen.

NovaOS verwendet daher risikobasierte Separation.

```text
Security Benefit
      versus
Complexity / Cost
```

Hard Security Requirements haben Vorrang vor Performance-Optimierung.

---

## Capability Minimization

Capability-Sets sollen minimiert werden.

Beispiel:

```text
Image Viewer
 ├── Read selected image
 ├── Use graphics service
 └── No arbitrary filesystem access
```

statt:

```text
Image Viewer
 └── Read all user files
```

Die Fähigkeit, eine Ressource technisch zu adressieren, darf nicht automatisch Authority über diese Ressource erzeugen.

---

## Objektbezogene Authority

Least Privilege gilt nicht nur für Operationstypen, sondern auch für Targets.

Beispielsweise soll eine Anwendung möglichst erhalten:

```text
Read Document X
```

statt:

```text
Read all documents
```

Capability Scope umfasst daher sowohl:

```text
Operation
    +
Target
    +
Constraints
```

---

## Zeitlich begrenzte Authority

Authority soll nicht länger bestehen als erforderlich.

Beispielsweise:

```text
User selects file
      ↓
Temporary File Capability
      ↓
Application operation
      ↓
Capability released / expires
```

Temporäre oder kontextgebundene Capabilities werden gegenüber dauerhaftem privilegiertem Zugriff bevorzugt.

---

## Task-bezogene Authority

Structured Concurrency ermöglicht Authority an die Lebensdauer eines Task Groups oder einer Operation zu binden.

```text
Parent Task
   ↓
Delegated Capability
   ↓
Child Task
   ↓
Task Complete
   ↓
Capability no longer usable
```

Dadurch kann Authority enger an den tatsächlichen Ausführungskontext gekoppelt werden.

---

## Authority-Vererbung

Child Processes und Child Tasks dürfen Authority nicht unkontrolliert vollständig erben.

Die Übergabe von Capabilities muss:

- explizit,
- begrenzt,
- nachvollziehbar

sein.

Konzeptionell:

```text
Parent Capabilities
 ├── A
 ├── B
 └── C

Child receives:
 └── B
```

Nicht:

```text
Child receives everything by default
```

---

## Privilege Drop

Komponenten müssen Authority, die nach einer Initialisierungsphase nicht mehr benötigt wird, dauerhaft oder temporär abgeben können.

Beispiel:

```text
Service Start
   ↓
Bind privileged resource
   ↓
Drop setup capability
   ↓
Normal operation
```

Die konkrete Capability-Lifecycle-Semantik wird in NPSPECs definiert.

---

## Privilege Acquisition

Eine Komponente soll fehlende erhöhte Authority gezielt nachfordern können.

Dabei gilt:

```text
Need additional authority
        ↓
Explicit request
        ↓
Policy evaluation
        ↓
Scoped capability
```

Ein temporärer Bedarf darf nicht zu dauerhaft erweitertem Privilege führen.

---

## Systemdienste

Systemdienste sollen nach Funktionsgrenzen getrennt werden, wenn dadurch sensitive Authority isoliert werden kann.

Beispiel:

```text
Update UI
    ↓
Update Coordinator
    ↓
Package Verification Service
    ↓
System Modification Service
```

Die Benutzeroberfläche benötigt dadurch keine direkte globale System-Modification-Capability.

---

## Frontend und privilegiertes Backend

Administrative Anwendungen sollen bevorzugt in einen weniger privilegierten Frontend-Teil und einen eng definierten privilegierten Backend-Service getrennt werden.

```text
Administrative UI
       ↓
Typed Nova.Call
       ↓
Privileged Service
       ↓
Specific Operation
```

Das Frontend benötigt nur die Authority, den entsprechenden Service für bestimmte Operationen aufzurufen.

---

## Parser Isolation

Komponenten, die nicht vertrauenswürdige oder komplexe Eingaben verarbeiten, sollen möglichst keine unnötige kritische Authority besitzen.

Dies gilt insbesondere für Parser von:

- Netzwerkdaten,
- Dokumentformaten,
- Medienformaten,
- Archiven,
- Treiber- oder Firmwaredaten,
- externen Protokollen.

```text
Untrusted Input
      ↓
Low-Privilege Parser
      ↓
Validated Typed Object
      ↓
Privileged Consumer
```

---

## Secret Separation

Langfristige Geheimnisse sollen nicht unnötig in Komponenten geladen werden, die lediglich kryptografische Operationen benötigen.

Beispielsweise:

```text
Application
    ↓
Sign Request
    ↓
Key Service
    ↓
Signature
```

anstatt:

```text
Application
    ↓
Raw Private Key
```

So bleibt die Schlüssel-Authority auf einen engeren Sicherheitskontext beschränkt.

---

## Driver Separation

Treiber erhalten nur Authority für ihre zugewiesenen Geräte und notwendigen Systemressourcen.

Beispielsweise:

```text
Network Driver
 ├── Assigned NIC
 ├── Assigned DMA Domain
 ├── Assigned Interrupts
 └── Network Driver APIs
```

Nicht automatisch:

```text
All devices
All physical memory
All kernel state
```

Wo möglich, werden besonders riskante Treiber in isolierten Driver Domains ausgeführt.

---

## DMA und IOMMU

Privilege Separation muss auch Hardwarezugriffe berücksichtigen.

Eine isolierte Komponente wäre nicht tatsächlich isoliert, wenn sie über unrestricted DMA beliebigen Systemspeicher verändern könnte.

Daher müssen Device Authority und DMA Authority gemeinsam begrenzt werden.

```text
Driver Domain
     ↓
Device Capability
     +
IOMMU Mapping
     ↓
Allowed Memory Regions
```

---

## Netzwerkprivilegien

Netzwerk-Authority wird ebenfalls nach Least Privilege vergeben.

Eine Anwendung kann beispielsweise erhalten:

```text
Connect to Service X
```

ohne:

```text
Open arbitrary outbound network connections
```

Ein Server kann:

```text
Accept connections for Service Y
```

erhalten, ohne allgemeine Kontrolle über alle Netzwerkinterfaces zu besitzen.

---

## Storage Privileges

Storage Authority wird möglichst auf konkrete Objekte und Operationen begrenzt.

Beispielsweise:

```text
Read Object A
Write Object B
Create objects in Container C
```

statt pauschaler Kontrolle über ein gesamtes Volume.

Legacy-Dateirechte dürfen in Compatibility Domains breiter ausfallen, bilden jedoch nicht das native Ideal.

---

## Application SYS Overlay

Eine Anwendung, die private Systemabhängigkeiten in ihrem eigenen `SYS`-Overlay verwendet, erhält dadurch keine Authority über das globale `/System`.

```text
Application SYS
     ↓
Private scoped authority

Global /System
     ↓
Separate elevated capability
```

Damit wird Privilege Separation zwischen Anwendungseigentum und globalem Systemzustand erhalten.

---

## Security Administration

Besonders sensitive Sicherheitsfunktionen sollen getrennte Capabilities verwenden.

Beispiele:

```text
Modify Security Policy
Issue Capabilities
Manage Trust Anchors
Read Security Audit
Modify Security Audit
```

Diese Authorities sollen nicht automatisch gemeinsam vergeben werden.

---

## Separation of Duties

Privilege Separation unterstützt auch organisatorische Separation of Duties.

Beispielsweise kann ein Administrator berechtigt sein:

```text
Install approved updates
```

ohne:

```text
Approve signing keys
```

Ein Security Auditor kann:

```text
Read audit data
```

ohne:

```text
Modify audit data
```

erhalten.

---

## Capability Issuance Separation

Die Fähigkeit, eine Capability zu verwenden, und die Fähigkeit, dieselbe Capability auszustellen, sind getrennte Authorities.

```text
Use Capability
      ≠
Issue Capability
```

Dadurch kann ein Service nicht allein aufgrund eigener Rechte beliebig gleichwertige Authority an andere Komponenten verteilen.

---

## Policy und Enforcement Separation

Wo sinnvoll, werden Policy Decision und Enforcement getrennt.

```text
Policy Engine
    ↓
Authorization Decision
    ↓
Enforcement Point
    ↓
Operation
```

Der Policy Engine muss nicht notwendigerweise selbst Zugriff auf die geschützte Ressource besitzen.

Das Enforcement Point benötigt wiederum nicht automatisch Authority zur Änderung der Policy.

---

## Code und Data Privilege Separation

NovaOS soll auch Speicher- und Ausführungsprivilegien differenzieren.

Beispiele:

```text
Executable
Writable
Readable
Shared
Device Mapped
```

Ein Speicherbereich soll nicht mehr Privilegien besitzen als notwendig.

Dies ergänzt klassische Mechanismen wie:

- NX,
- Guard Pages,
- Memory Protection,
- Protection Keys.

---

## Privilege Separation über Execution Domains

Execution Domains sind die primäre grobe Isolationseinheit für voneinander getrennte Authority.

```text
Execution Domain A
 ├── Address Space
 ├── Resource Budget
 └── Capability Set

Execution Domain B
 ├── Address Space
 ├── Resource Budget
 └── Capability Set
```

Kommunikation zwischen ihnen erfolgt über explizite IPC- beziehungsweise `Nova.Call`-Contracts.

---

## Privilege Separation innerhalb einer Execution Domain

Nicht jede Authority-Trennung erfordert eine eigene Execution Domain.

Innerhalb einer Domain können zusätzliche Mechanismen verwendet werden, beispielsweise:

- Task-local Capability Contexts,
- Memory Protection Keys,
- getrennte Object Handles,
- Scoped Capability Stores,
- Language-Level Isolation.

Diese Mechanismen ergänzen, ersetzen aber nicht notwendige harte Domain-Isolation.

---

## Own Authority und Delegated Authority

Systemdienste müssen zwischen eigener Authority und Authority unterscheiden, die ihnen für eine konkrete Operation delegiert wurde.

```text
Service
 ├── Own Capabilities
 └── Delegated Caller Capabilities
```

Operationen im Auftrag eines Callers sollen, soweit semantisch passend, mit dessen delegierter Authority ausgeführt werden.

Dadurch wird Confused-Deputy-Risiken entgegengewirkt.

---

## Ambient Authority

NovaOS minimiert Ambient Authority.

Komponenten sollen nicht aufgrund ihrer Umgebung automatisch Zugriff auf:

- alle Dateien eines Benutzers,
- alle Netzwerkverbindungen,
- alle Geräte,
- alle IPC-Endpunkte,
- alle Environment Secrets

erhalten.

Authority soll explizit im Execution- oder Call Context sichtbar sein.

---

## KI-Agenten

Least Privilege ist für KI-Agenten zwingend.

Ein Agent erhält ausschließlich die Capabilities, die für seinen aktuellen Auftrag notwendig sind.

Beispiel:

```text
Agent Task:
"Summarize selected documents"

Capabilities:
 ├── Read Document A
 ├── Read Document B
 └── Write Draft C
```

Nicht:

```text
Full user filesystem
Full email account
System administration
```

Ein Agent darf neue Authority nicht allein durch eigene Planung erzeugen.

---

## Plugin- und Erweiterungsmodell

Plugins und Erweiterungen sollen nicht automatisch die gesamte Authority ihrer Host-Anwendung erhalten.

Wo möglich, werden:

- Plugin Capability Sets,
- getrennte Execution Contexts,
- explizite Host APIs

verwendet.

```text
Host Application
      ↓
Restricted Plugin Interface
      ↓
Plugin
```

---

## Kompromittierungsradius

Privilege Separation soll den Blast Radius einer kompromittierten Komponente begrenzen.

Konzeptionell:

```text
Compromise Component A
       ↓
Authority of A exposed
       ↓
Not automatically B + C + D
```

Die Architektur soll daher bevorzugt Sicherheitsgrenzen dort setzen, wo eine Kompromittierung andernfalls unverhältnismäßig große Auswirkungen hätte.

---

## Resource Privileges

Least Privilege gilt auch für Ressourcennutzung.

Authority zur Nutzung einer Ressource bedeutet nicht unbegrenzte Nutzung.

Beispielsweise:

```text
GPU Capability
    +
GPU Budget
```

oder:

```text
Network Capability
    +
Bandwidth Budget
```

Security Authority und Resource Economy ergänzen sich.

---

## Privilege und Data Sovereignty

Eine Capability kann funktionale Authority verleihen, während Data-Sovereignty- oder Information-Flow-Policies die tatsächliche Verwendung weiter begrenzen.

```text
Functional Authority
       +
Information Flow Policy
       =
Allowed Operation
```

Least Privilege darf nicht als Mechanismus verstanden werden, andere Hard Constraints zu ersetzen.

---

## Temporäre Privilege Separation bei Transaktionen

Für transaktionale Systemoperationen können Capabilities gezielt an den Transaktionskontext gebunden werden.

```text
Transaction
   ↓
Scoped Capabilities
   ↓
Commit / Rollback
   ↓
Authority released
```

Dadurch wird verhindert, dass für eine einmalige Systemänderung dauerhaft erhöhte Authority bestehen bleibt.

---

## Hot Replacement

Beim Hot Replacement einer Komponente darf die Ersatzkomponente nicht automatisch mehr Authority erhalten als die ersetzte Instanz.

Capability Transfer muss an:

- Identität,
- Compatibility,
- Trust,
- Lifecycle

gebunden sein.

---

## Checkpoint und Migration

Checkpointbare oder migrierbare Execution Domains dürfen Capability Authority nicht unkontrolliert vervielfältigen.

Eine Migration muss klären, ob die Authority:

```text
Transferred
Reissued
Revalidated
Revoked at source
```

wird.

Die konkrete Semantik wird in NPSPECs definiert.

---

## Audit

Privilege-relevante Entscheidungen müssen kontrolliert auditierbar sein.

Beispielsweise:

```text
Component
 ├── Granted Capability
 ├── Source
 ├── Scope
 ├── Lifetime
 ├── Delegation
 └── Revocation
```

Besonders relevant sind:

- Privilege Elevation,
- Capability Delegation,
- Capability Issuance,
- administrative Authority,
- Separation Boundary Crossing.

---

## Architecture Introspection

NovaOS soll darstellen können:

```text
Component A
 ├── Capabilities
 ├── Resource Authority
 ├── Delegated Authority
 └── Communication Boundaries
```

und:

```text
Why does this component have this capability?
```

Dies ermöglicht die Analyse unnötiger Privilegien.

Tools können dadurch beispielsweise aufzeigen:

```text
Unused Capability
Overly Broad Target Scope
Long-Lived Elevated Authority
Unexpected Delegation Chain
```

---

## Adaptive Least-Privilege-Unterstützung

NovaOS darf adaptive Analysen verwenden, um:

- ungenutzte Capabilities,
- ungewöhnlich breite Authority,
- mögliche Separation Boundaries

vorzuschlagen.

Solche Systeme dürfen jedoch nicht selbständig Hard Security Authority verändern.

```text
Adaptive Analysis
      ↓
Recommendation

Explicit Security Policy
      ↓
Authority Change
```

---

## Fail-Safe Default

Fehlt eine benötigte Authority, wird eine Operation nicht dadurch erlaubt, dass die Komponente „normalerweise vertrauenswürdig“ ist.

```text
Required capability missing
        ↓
Deny
```

Eine Separation Boundary darf nicht automatisch umgangen werden, nur weil die direkte Kommunikation oder Ausführung einfacher wäre.

---

## Normative Anforderungen

1. NovaOS MUSS Least Privilege als systemweites Sicherheitsprinzip verwenden.
2. Native Komponenten SOLLEN nur die für ihre definierte Verantwortung erforderliche Authority erhalten.
3. Least Privilege MUSS auf Anwendungen, Systemdienste, Treiber, KI-Agenten und administrative Komponenten anwendbar sein.
4. Capability-Sets SOLLEN hinsichtlich Operation, Target und Constraints möglichst eng definiert werden.
5. Authority SOLL nicht länger bestehen als für ihren vorgesehenen Zweck erforderlich.
6. Temporäre und kontextgebundene Capabilities MÜSSEN unterstützt werden können.
7. Child Processes und Child Tasks DÜRFEN Authority NICHT unkontrolliert vollständig erben.
8. Capability Transfer an Child Contexts MUSS explizit begrenzbar sein.
9. Komponenten MÜSSEN nicht mehr benötigte Authority abgeben beziehungsweise verlieren können.
10. Privilege Elevation MUSS gezielt auf benötigte Authority begrenzbar sein.
11. NovaOS MUSS Privilege Separation zwischen sicherheitsrelevant unterschiedlichen Verantwortlichkeiten unterstützen.
12. Privilege Separation SOLL risikobasiert erfolgen und DARF nicht unnötige Maximalfragmentierung erzwingen.
13. Untrusted Parsing SOLL möglichst von unnötiger kritischer Authority getrennt werden.
14. Geheimnisverwaltung SOLL von Komponenten ohne Bedarf an direktem Secret-Zugriff getrennt werden.
15. Treiber MÜSSEN auf notwendige Device-, DMA- und Interrupt-Authority begrenzbar sein.
16. IOMMU- beziehungsweise DMA-Grenzen MÜSSEN mit Driver Privilege Separation integrierbar sein.
17. Netzwerkauthority MUSS auf konkrete notwendige Services, Operationen oder Endpunkte begrenzbar sein.
18. Storage Authority MUSS auf konkrete Objekte und Operationen begrenzbar sein.
19. Private Application-`SYS`-Authority DARF NICHT globale `/System`-Authority implizieren.
20. Kritische Security Authorities SOLLEN voneinander getrennt vergeben werden können.
21. Capability-Nutzung und Capability-Issuance MÜSSEN getrennte Authorities sein können.
22. Policy Decision und Enforcement SOLLEN voneinander trennbar sein, wo dies die Sicherheitsarchitektur verbessert.
23. Execution Domains MÜSSEN unterschiedliche Capability Sets besitzen können.
24. Kommunikation über Privilege-Separation-Grenzen MUSS über kontrollierte Contracts erfolgen.
25. Systemdienste MÜSSEN eigene Authority und delegierte Caller-Authority unterscheiden können.
26. Native NovaOS-Komponenten SOLLEN Ambient Authority minimieren.
27. KI-Agenten MÜSSEN taskbezogen und nach Least Privilege autorisiert werden.
28. Plugins DÜRFEN NICHT automatisch die vollständige Authority ihres Hosts erhalten müssen.
29. Resource Authority MUSS von Resource Budgets begrenzbar sein.
30. Least Privilege DARF Data-Sovereignty-, Trust- oder andere Security-Hard-Constraints NICHT ersetzen.
31. Hot Replacement DARF Authority NICHT unbeabsichtigt erweitern.
32. Checkpoint und Migration DÜRFEN Capability Authority NICHT unkontrolliert vervielfältigen.
33. Privilege Elevation, Delegation und Issuance MÜSSEN kontrolliert auditierbar sein.
34. Capability- und Separation-Beziehungen MÜSSEN kontrolliert introspektierbar sein.
35. Adaptive Systeme DÜRFEN Hard Security Authority NICHT selbständig erweitern oder reduzieren.
36. Fehlende Authority MUSS nach Fail-Safe Defaults zur Ablehnung der geschützten Operation führen.

---

## Konsequenzen

### Positive Konsequenzen

- geringerer Blast Radius kompromittierter Komponenten,
- präzisere Capability-Sets,
- weniger Ambient Authority,
- stärkere Isolation von Treibern und Systemdiensten,
- bessere Sicherheit bei KI-Agenten und Plugins,
- sichere Trennung von Benutzeroberfläche und privilegierten Operationen,
- reduzierte Confused-Deputy-Risiken,
- bessere Auditierbarkeit administrativer Authority,
- Architecture Introspection kann überprivilegierte Komponenten sichtbar machen.

### Negative Konsequenzen

- zusätzliche Security Domains und Services erhöhen die Architekturkomplexität,
- IPC- und `Nova.Call`-Kommunikation zwischen getrennten Komponenten erzeugt zusätzlichen Aufwand,
- Capability-Sets müssen sorgfältig modelliert werden,
- zu aggressive Fragmentierung kann Performance und Wartbarkeit verschlechtern,
- Migration und Hot Replacement müssen Capability-Lifetimes berücksichtigen.

---

## Verworfene Alternativen

### Allen Systemdiensten breite Privilegien geben

Verworfen.

Eine Kompromittierung eines einzelnen Systemdienstes hätte unnötig große Auswirkungen.

### Least Privilege nur für Benutzeranwendungen

Verworfen.

Kernel-nahe Dienste, Treiber und administrative Werkzeuge stellen mindestens ebenso kritische Angriffsflächen dar.

### Nur Prozessgrenzen als Privilege Separation

Verworfen.

Auch Capability Scope, Task Context, Hardwarezugriff und Resource Authority müssen berücksichtigt werden.

### Jede Funktion in eigenen Prozess auslagern

Verworfen.

Maximale Fragmentierung erzeugt erhebliche Komplexität und ist nicht für jede Sicherheitsgrenze gerechtfertigt.

### Privilege Separation ausschließlich durch Programmiersprachenmechanismen

Verworfen.

Language-Level Isolation ist wertvoll, ersetzt aber keine harten Isolation Domains bei kritischen Authority-Grenzen.

### Dauerhafte Elevation nach einmaliger Zustimmung

Verworfen.

Ein temporärer Bedarf rechtfertigt keine dauerhaft erweiterte Authority.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0017_Composability_und_Orthogonality`
- `ADR-ARCH-0020_AI_nicht_Kernel_Correctness_Prerequisite`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-PROCESS-0007_Migrierbare_Workloads`
- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0002_Kein_universelles_Root_Privileg`
- `ADR-STORAGE-0018_Per_Application_System_Overlay`
- `ADR-STORAGE-0019_Private_Systemabhängigkeiten_im_Application_SYS_Namespace`
- `ADR-STORAGE-0020_Globales_System_nur_mit_erhöhter_Berechtigung`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-LEAST-PRIVILEGE-0001`
- `NPSPEC-SECURITY-PRIVILEGE-SEPARATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-MINIMIZATION-0001`
- `NPSPEC-SECURITY-AUTHORITY-INHERITANCE-0001`
- `NPSPEC-SECURITY-PRIVILEGE-DROP-0001`
- `NPSPEC-SECURITY-PRIVILEGE-ACQUISITION-0001`
- `NPSPEC-SECURITY-PARSER-ISOLATION-0001`
- `NPSPEC-SECURITY-SECRET-SEPARATION-0001`
- `NPSPEC-SECURITY-DELEGATED-AUTHORITY-0001`
- `NPSPEC-SECURITY-PRIVILEGE-INTROSPECTION-0001`
- `NPSPEC-SECURITY-LEAST-PRIVILEGE-TEST-0001`

---

## Ergebnis

NovaOS kombiniert Capability-Based Security mit konsequentem Least Privilege und Privilege Separation:

```text
                       System Function
                             ↓
                    Required Responsibilities
                             ↓
                    Required Authorities
                             ↓
                   Capability Minimization
                             ↓
                  Privilege Separation
                             ↓
       ┌─────────────────────┼─────────────────────┐
       ↓                     ↓                     ↓
   Domain A               Domain B              Domain C
   Capability A           Capability B          Capability C
       ↓                     ↓                     ↓
 Specific Work          Specific Work         Specific Work
```

Damit wird Authority nicht nur explizit modelliert, sondern auch möglichst klein gehalten und auf sinnvolle Sicherheitsgrenzen verteilt.

Die Sicherheitsfrage lautet dadurch nicht nur:

```text
"Is this operation authorized?"
```

sondern zusätzlich:

```text
"Why does this component possess this authority,
how narrowly is it scoped,
and does this authority really need to coexist
with its other privileges?"
```

Least Privilege begrenzt die vorhandene Authority.

Privilege Separation begrenzt deren Konzentration.

Gemeinsam bilden beide Prinzipien eine zentrale Grundlage dafür, dass die Kompromittierung einer einzelnen NovaOS-Komponente nicht automatisch zur Kompromittierung des gesamten Systems führt.