# ADR-SECURITY-0002 – Kein universelles Root-Privileg

## Status

Angenommen

## Kategorie

Security / Privilege Model / Least Authority / Capability-Based Security

## Kontext

Klassische Betriebssysteme verwenden häufig ein besonders privilegiertes Konto oder einen besonders privilegierten Ausführungskontext wie:

```text
root
Administrator
SYSTEM
```

Ein solcher Kontext besitzt typischerweise weitreichende oder nahezu uneingeschränkte Authority über das gesamte System.

Dieses Modell ist einfach zu verstehen und erleichtert administrative Aufgaben, erzeugt jedoch eine zentrale Sicherheitskonzentration:

```text
Compromise Root
      ↓
Compromise Everything
```

Für NovaOS widerspricht ein universelles Root-Privileg mehreren zentralen Architekturprinzipien:

- Capability-Based Security,
- Principle of Least Authority,
- Fail-Safe Defaults,
- Isolation Domains,
- explizite Contracts,
- Data Sovereignty,
- kontrollierte Delegation,
- Architecture Introspection.

NovaOS benötigt weiterhin administrative und systemkritische Operationen, darf deren Authority jedoch nicht pauschal in einer einzelnen universellen Rolle bündeln.

---

## Entscheidung

NovaOS besitzt **kein universelles Root-Privileg**, das automatisch sämtliche Sicherheitsgrenzen umgehen kann.

Stattdessen werden administrative und systemkritische Operationen über explizite, begrenzte Capabilities autorisiert.

```text
Administrative Operation
        ↓
Required Capability
        ↓
Authorization
        ↓
Execution
```

Es existiert kein allgemeiner Sicherheitszustand:

```text
Root = may do everything
```

Stattdessen gilt:

```text
Authority is explicit,
scoped,
and operation-specific.
```

---

## Grundprinzip

```text
There is no universal authority.
```

und:

```text
Administrative power is a set of capabilities,
not a magic identity.
```

---

## Keine globale Sicherheitsumgehung

Keine Identity, kein Prozess und keine Execution Domain erhält allein aufgrund eines globalen Status das Recht, sämtliche Security Checks zu umgehen.

Insbesondere darf es keinen allgemeinen Mechanismus geben nach dem Muster:

```text
if privileged:
    bypass_security()
```

Sicherheitsentscheidungen bleiben auch für hochprivilegierte Systemkomponenten explizit.

---

## Administrative Capabilities

Administrative Aufgaben werden durch dedizierte Capabilities modelliert.

Beispiele:

```text
ModifyBootConfiguration
InstallKernelComponent
ManageDriver
ModifyGlobalSystem
ManageUsers
ModifySecurityPolicy
ManageNetworkConfiguration
ManageStoragePolicy
ShutdownSystem
```

Eine Capability für eine administrative Operation impliziert keine Authority für andere administrative Operationen.

```text
ManageNetwork
    ≠
ManageStorage

InstallDriver
    ≠
ModifySecurityPolicy
```

---

## Least Authority für Administration

Administrative Werkzeuge und Dienste erhalten nur die Authority, die für ihre konkrete Funktion erforderlich ist.

Beispiel:

```text
Network Configuration Tool
 ├── ModifyNetworkConfiguration
 ├── ReadNetworkState
 └── No Storage Administration
```

Dadurch reduziert NovaOS die Auswirkungen kompromittierter administrativer Komponenten.

---

## Benutzer und Administratorrollen

NovaOS darf weiterhin Benutzerrollen wie:

```text
Administrator
System Operator
Device Manager
Security Administrator
```

bereitstellen.

Diese Rollen sind jedoch keine unmittelbare universelle Authority.

Sie dienen als Policy- und Capability-Issuance-Kontext.

```text
Role
   ↓
Policy
   ↓
Capability Set
```

Eine Rolle ist damit eine organisatorische Abstraktion über konkrete Capabilities.

---

## Kein implizites Authority-Erbe

Ein Prozess erhält nicht automatisch universelle Authority, nur weil er von einem administrativen Prozess gestartet wurde.

```text
Privileged Parent
      ↓
Child Process
```

bedeutet nicht:

```text
Child inherits all authority
```

Capability Transfer beziehungsweise Delegation muss explizit erfolgen.

---

## Systemdienste

Auch zentrale Systemdienste besitzen keine automatisch unbegrenzte Authority.

Sie erhalten nur die für ihre Funktion notwendigen Capabilities.

Beispielsweise:

```text
Storage Service
 ├── Storage Management
 ├── Device Access for assigned devices
 └── No Boot Policy Modification
```

oder:

```text
Update Service
 ├── Install signed system updates
 ├── Manage update transactions
 └── No arbitrary user-data access
```

---

## Kernel Authority

Der Kernel ist technisch für die Durchsetzung grundlegender Sicherheitsmechanismen verantwortlich.

Dies bedeutet jedoch nicht, dass Kernel-interne Komponenten semantisch als universeller `root` behandelt werden.

Kernel-Subsysteme sollen intern ebenfalls klare:

- Authority Boundaries,
- Capability Checks,
- Module Responsibilities,
- Resource Ownership

besitzen.

Der Kernel ist Enforcement Mechanism, nicht automatisch ein monolithischer semantischer Administrator.

---

## Treiber

Treiber erhalten keine universelle Systemauthority.

Ein Treiber erhält beispielsweise gezielte Capabilities für:

```text
Assigned Device
DMA Mapping
Interrupt Registration
Specific Firmware Interface
```

Ein Grafiktreiber benötigt keine Authority für:

- Benutzerkonten,
- Bootkonfiguration,
- Netzwerkpolicy,
- fremde Storage-Daten.

---

## Privilege Elevation

NovaOS unterstützt kontrollierte Privilege Elevation.

Dabei wird jedoch kein allgemeiner Root-Modus aktiviert.

Stattdessen:

```text
Operation requires additional authority
          ↓
Authorization Request
          ↓
Policy / User Approval
          ↓
Temporary Capability
          ↓
Operation
          ↓
Capability expires / revoked
```

Elevation ist damit:

- zielgerichtet,
- zeitlich begrenzbar,
- operation-spezifisch,
- auditierbar.

---

## Keine globale sudo-Semantik

Ein Kompatibilitätswerkzeug darf eine `sudo`-ähnliche Benutzeroberfläche bereitstellen.

Die native Semantik lautet jedoch nicht:

```text
run this program as root
```

sondern eher:

```text
grant this execution the capabilities required
for this approved operation
```

Eine Anwendung erhält dadurch nicht automatisch sämtliche administrativen Capabilities.

---

## Capability Bundles

Für Bedienbarkeit dürfen mehrere Capabilities zu administrativen Bundles zusammengefasst werden.

Beispielsweise:

```text
System Maintenance Bundle
 ├── ManageUpdates
 ├── RestartServices
 ├── ReadSystemDiagnostics
 └── ManageRecoveryState
```

Ein Bundle ist jedoch lediglich eine Policy- oder Deployment-Abstraktion.

Die zugrunde liegende Authority bleibt aus einzelnen Capabilities zusammengesetzt und introspektierbar.

---

## Break-Glass Administration

NovaOS darf für Notfälle besonders weitreichende administrative Verfahren unterstützen.

Ein solches Break-Glass-Verfahren muss jedoch:

- explizit aktiviert werden,
- stark authentisiert sein,
- zeitlich beziehungsweise kontextuell begrenzt sein,
- auditierbar sein,
- aus konkreten Capabilities bestehen.

```text
Emergency Authorization
        ↓
Temporary Elevated Capability Set
```

Auch Break-Glass ist kein permanentes universelles Root-Prinzip.

---

## Recovery

NovaDOS und andere Recovery-Umgebungen können weitreichende Offline-Administrative Capabilities erhalten.

Diese Authority gilt jedoch nur innerhalb des klar definierten Recovery-Kontexts.

```text
Recovery Environment
      ↓
Recovery Capabilities
```

Recovery Authority ist nicht automatisch auf das laufende normale NovaOS übertragbar.

---

## Globale Systemänderungen

Änderungen am globalen Systemzustand benötigen explizite erhöhte Capabilities.

Dies gilt insbesondere für:

```text
/System
Boot Configuration
Kernel Components
Global Drivers
Security Policies
Trust Anchors
```

Der normale Zugriff auf private Application-`SYS`-Overlays erzeugt keine entsprechende globale Authority.

---

## Security Policy Administration

Die Fähigkeit, Security Policies zu verändern, wird getrennt von der Fähigkeit behandelt, normale Systemoperationen auszuführen.

```text
Use Permission
    ≠
Change Permission Policy
```

Eine Komponente, die eine Capability verwenden darf, erhält dadurch nicht automatisch Authority, Capability-Regeln oder Trust Policies zu verändern.

---

## Capability Issuance

Die Authority zum Ausstellen von Capabilities ist selbst capability-basiert.

```text
Capability Issuer
      ↓
Issuance Authority
      ↓
Derived Capability
```

Ein Issuer darf nur Authority vergeben, die ihm selbst beziehungsweise seiner Issuance Policy ausdrücklich zur Verfügung steht.

Damit darf auch Capability-Issuance keinen versteckten universellen Root-Pfad erzeugen.

---

## Delegation und Attenuation

Administrative Authority kann delegiert werden, jedoch nur innerhalb bestehender Grenzen.

```text
Authority(Delegated)
    ⊆
Authority(Original)
```

Beispielsweise kann ein Systemadministrator:

```text
Full Device Management
```

für eine konkrete Aufgabe attenuieren zu:

```text
Update Driver X
Device Y
Valid for 10 minutes
```

---

## Separation of Duties

NovaOS unterstützt die Trennung kritischer administrativer Verantwortlichkeiten.

Beispielsweise können unterschiedliche Capabilities erforderlich sein für:

```text
Security Policy Administration
Trust Anchor Administration
System Update Administration
Audit Administration
```

Dadurch müssen besonders kritische Funktionen nicht zwingend in einer einzigen Rolle oder Komponente gebündelt werden.

---

## Multi-Party Authorization

Für besonders kritische Operationen darf NovaOS mehrere unabhängige Autorisierungen verlangen.

Beispielsweise:

```text
Critical Trust Anchor Change

Security Authority
       +
System Authority
       ↓
Approved
```

Die konkrete Multi-Party-Semantik wird in NPSPECs definiert.

Dies ist nur möglich, weil NovaOS Authority nicht als einzelne Root-Bit-Entscheidung modelliert.

---

## Sandboxing administrativer Tools

Auch ein administratives Werkzeug bleibt innerhalb einer Execution Domain isoliert.

Eine GUI zur Treiberverwaltung benötigt beispielsweise nicht allein deshalb Zugriff auf sämtliche Speicherbereiche oder Prozesse.

```text
Administrative UI
      ↓
Narrow Capabilities
      ↓
System Service
```

Wo sinnvoll, wird die eigentliche privilegierte Operation von einem spezialisierten Systemdienst ausgeführt.

---

## Confused Deputy Protection

Das Fehlen eines universellen Root-Modells reduziert auch Confused-Deputy-Risiken.

Ein privilegierter Dienst soll unterscheiden zwischen:

```text
Own Service Authority
```

und:

```text
Authority delegated by caller
```

Der Dienst darf nicht automatisch seine gesamte eigene administrative Authority im Auftrag eines schwächer autorisierten Callers einsetzen.

---

## KI-Agenten

Ein KI-Agent darf niemals durch einen pauschalen „Administrator Mode“ universelle Authority erhalten.

Stattdessen werden Capabilities gezielt bereitgestellt.

Beispiel:

```text
System Maintenance Agent
 ├── Read diagnostics
 ├── Restart selected service
 ├── Install approved update
 └── No arbitrary security policy modification
```

Adaptive oder generative Entscheidungen dürfen die Capability-Grenzen nicht erweitern.

---

## Security UI

NovaOS soll Benutzer bei Privilege Elevation möglichst über die konkrete angeforderte Authority informieren.

Nicht:

```text
This application requires administrator privileges.
```

sondern beispielsweise:

```text
This application requests permission to:

- install driver X
- modify device Y
- restart service Z
```

Die genaue UI-Spezifikation gehört in entsprechende NPSPECs.

---

## Legacy Compatibility

Legacy-Anwendungen können innerhalb einer Compatibility Domain ein klassisches Root-/Administrator-Modell erwarten.

NovaOS darf dieses Verhalten dort emulieren.

Konzeptionell:

```text
Legacy "root"
      ↓
Compatibility Policy
      ↓
Mapped Capability Set
```

Das emulierte Root-Privileg bleibt auf die Compatibility Domain beschränkt.

Es darf nicht automatisch universelle native NovaOS-Authority verleihen.

---

## Native API Design

Native NovaOS-APIs dürfen nicht darauf angewiesen sein, dass ein Caller „root“ ist.

Stattdessen müssen sie die für eine Operation notwendige Capability beziehungsweise Authority explizit definieren.

```text
Native API
    ↓
Required Capability
```

Damit bleibt die Security-Semantik auch bei zukünftigen Rollen- oder Deployment-Modellen stabil.

---

## Audit

Administrative Authority muss nachvollziehbar sein.

Audit kann beispielsweise erfassen:

```text
Subject
 ├── Requested Operation
 ├── Capability Used
 ├── Capability Issuer
 ├── Elevation Context
 ├── Target
 └── Decision
```

Ein Audit-Eintrag:

```text
executed as root
```

ist für native NovaOS-Autorisierung nicht ausreichend präzise.

---

## Architecture Introspection

NovaOS muss darstellen können:

```text
Why can this component perform this operation?
```

Die Antwort soll auf konkreten Capability-Beziehungen beruhen.

Beispiel:

```text
Execution Domain
      ↓
Temporary Capability
      ↓
Issued by Security Service
      ↓
Target: Driver X
      ↓
Operation: Update
      ↓
Valid until T
```

Es soll keinen versteckten universellen Root-Pfad geben, der diese Analyse umgeht.

---

## Fail-Safe Default

Ist unklar, ob eine Komponente die notwendige administrative Authority besitzt, wird die Operation abgelehnt.

```text
Unknown Authority
      ↓
Deny
```

Es darf keinen Fallback geben nach dem Muster:

```text
caller is privileged enough
```

ohne konkrete Authority-Prüfung.

---

## Normative Anforderungen

1. NovaOS DARF KEIN universelles Root-Privileg als natives Sicherheitsmodell besitzen.
2. Keine Identity DARF allein automatisch unbegrenzte Systemauthority verleihen.
3. Administrative Operationen MÜSSEN durch explizite Capabilities autorisierbar sein.
4. Administrative Capabilities SOLLEN nach dem Principle of Least Authority zugeschnitten werden.
5. Eine administrative Capability DARF NICHT automatisch andere administrative Authority implizieren.
6. Benutzer- und Administratorrollen DÜRFEN als Policy- beziehungsweise Capability-Bundles existieren, sind aber NICHT selbst universelle Authority.
7. Child Processes DÜRFEN NICHT automatisch sämtliche Authority eines administrativen Parent-Prozesses erhalten.
8. Systemdienste MÜSSEN auf die für ihre Funktion notwendige Authority begrenzbar sein.
9. Treiber DÜRFEN KEINE implizite universelle Systemauthority erhalten.
10. Privilege Elevation MUSS gezielte zusätzliche Capabilities vergeben können.
11. Privilege Elevation SOLL zeitlich, funktional oder kontextuell begrenzbar sein.
12. Native Elevation DARF NICHT als pauschaler `run as root`-Mechanismus definiert werden.
13. Administrative Capability Bundles MÜSSEN in ihre konkrete Authority introspektierbar bleiben.
14. Break-Glass-Verfahren DÜRFEN weitreichende Authority bereitstellen, MÜSSEN jedoch explizit, begrenzt und auditierbar sein.
15. Recovery Authority MUSS vom normalen Runtime-Authority-Kontext trennbar sein.
16. Globale Systemänderungen MÜSSEN explizite erhöhte Capabilities erfordern können.
17. Authority zur Nutzung einer Security Policy DARF NICHT automatisch Authority zur Änderung dieser Policy erzeugen.
18. Capability-Issuance-Authority MUSS selbst begrenzt und autorisiert sein.
19. Capability Issuer DÜRFEN keine Authority außerhalb ihrer zulässigen Issuance-Grenzen erzeugen.
20. Delegierte administrative Authority DARF die ursprüngliche Authority NICHT erweitern.
21. NovaOS MUSS Separation of Duties für kritische administrative Bereiche unterstützen können.
22. Multi-Party Authorization MUSS für besonders kritische Operationen grundsätzlich möglich sein.
23. Administrative Anwendungen MÜSSEN weiterhin sandboxbar und isolierbar sein.
24. Systemdienste MÜSSEN zwischen eigener Authority und delegierter Caller-Authority unterscheiden können.
25. KI-Agenten DÜRFEN KEINE universelle Administratorauthority erhalten.
26. Native APIs DÜRFEN NICHT von einem universellen `root`-Status abhängig sein.
27. Legacy-Root-Semantik DARF nur innerhalb kontrollierter Compatibility Domains emuliert werden.
28. Emuliertes Legacy-Root DARF NICHT automatisch native NovaOS-Authority verleihen.
29. Administrative Operationen und Elevation MÜSSEN kontrolliert auditierbar sein.
30. Administrative Authority MÜSSEN über konkrete Capability-Beziehungen introspektierbar sein.
31. Es DARF keinen versteckten generischen Security-Bypass für „privilegierte“ native Prozesse geben.
32. Unklare oder fehlende administrative Authority MUSS nach Fail-Safe Defaults abgelehnt werden.

---

## Konsequenzen

### Positive Konsequenzen

- Kompromittierung eines einzelnen administrativen Prozesses führt nicht automatisch zur vollständigen Systemübernahme,
- administrative Authority kann präzise auf Aufgaben begrenzt werden,
- Privilege Elevation wird nachvollziehbarer und temporär begrenzbar,
- Systemdienste und Treiber benötigen keine pauschale Root-Authority,
- Capability Delegation und Attenuation können auch für Administration genutzt werden,
- Separation of Duties wird architektonisch möglich,
- KI-Agenten können administrative Aufgaben sicherer übernehmen,
- Security Introspection kann konkrete Authority-Ketten darstellen.

### Negative Konsequenzen

- Administration wird komplexer als ein einzelnes Root-/Administrator-Bit,
- Capability-Sets und Issuance Policies müssen sorgfältig entworfen werden,
- Legacy-Software erwartet teilweise universelle Root-Semantik und benötigt Compatibility Mapping,
- administrative Tools müssen ihre benötigte Authority explizit deklarieren.

---

## Verworfene Alternativen

### Klassischer universeller Root-Benutzer

Verworfen.

Eine kompromittierte Root-Identity würde praktisch sämtliche Sicherheitsgrenzen aufheben.

### Root plus zusätzliche Capabilities

Verworfen als natives Modell.

Ein universelles Root-Privileg würde die Vorteile der Capability-basierten Autorisierung weiterhin umgehen können.

### Ein globales Administrator-Flag pro Prozess

Verworfen.

Ein einzelnes Flag ist zu grob und verhindert Least Authority.

### Vollständige Authority-Vererbung an Child Processes

Verworfen.

Dies erzeugt unnötige Ambient Authority und vergrößert Angriffsflächen.

### Sicherheitschecks für Systemdienste überspringen

Verworfen.

Auch vertrauenswürdige Systemdienste können Fehler enthalten oder kompromittiert werden.

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
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-STORAGE-0018_Per_Application_System_Overlay`
- `ADR-STORAGE-0019_Private_Systemabhängigkeiten_im_Application_SYS_Namespace`
- `ADR-STORAGE-0020_Globales_System_nur_mit_erhöhter_Berechtigung`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-PRIVILEGE-MODEL-0001`
- `NPSPEC-SECURITY-ADMIN-CAPABILITY-0001`
- `NPSPEC-SECURITY-PRIVILEGE-ELEVATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-BUNDLE-0001`
- `NPSPEC-SECURITY-BREAK-GLASS-0001`
- `NPSPEC-SECURITY-SEPARATION-OF-DUTIES-0001`
- `NPSPEC-SECURITY-MULTI-PARTY-AUTHORIZATION-0001`
- `NPSPEC-SECURITY-LEGACY-ROOT-COMPATIBILITY-0001`
- `NPSPEC-SECURITY-ADMIN-AUDIT-0001`
- `NPSPEC-SECURITY-PRIVILEGE-MODEL-TEST-0001`

---

## Ergebnis

NovaOS ersetzt das klassische universelle Root-Modell durch explizite administrative Authority:

```text
                      User / Service
                            ↓
                     Authentication
                            ↓
                    Authorization Policy
                            ↓
                    Capability Issuance
                            ↓
           ┌────────────────┼────────────────┐
           ↓                ↓                ↓
      Boot Admin       Driver Admin     Network Admin
           ↓                ↓                ↓
       Scoped           Scoped           Scoped
      Authority        Authority        Authority
           └────────────────┬────────────────┘
                            ↓
                     System Operations
```

Es existiert kein einzelner nativer Zustand:

```text
"this subject may bypass every security boundary"
```

Stattdessen besitzt jede administrative Operation eine nachvollziehbare und begrenzte Authority.

Damit bleibt auch hochprivilegierte Systemverwaltung dem zentralen NovaOS-Prinzip unterworfen:

```text
No universal root.
Only explicit authority.
```