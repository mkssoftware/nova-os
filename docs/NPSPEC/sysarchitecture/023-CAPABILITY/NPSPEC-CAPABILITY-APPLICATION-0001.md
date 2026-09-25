# NPSPEC-CAPABILITY-APPLICATION-0001 – Nova Application Capability

## Status

Angenommen

## Kategorie

Capability / Application / Security

## Zweck

NovaOS definiert die Autorität von Anwendungen vollständig über explizite Capabilities.

Eine installierte oder gestartete Anwendung erhält nicht automatisch die Rechte des Benutzers.

```text
Application Identity
        ↓
Application Sandbox
        ↓
Capability Set
        ↓
Authorized Resources
```

## Grundprinzipien

```text
Application Identity ≠ Application Authority
Installed ≠ Authorized
Running ≠ Trusted
User Authority ≠ Application Authority
File Visibility ≠ File Access
Network Availability ≠ Network Authority
Signed Application ≠ Unlimited Authority
Application Update ≠ Automatic Authority Change
```

## Application Capability Domain

Jede laufende Anwendung besitzt einen eigenen Capability-Kontext.

```text
ApplicationCapabilityDomain
├── Application Identity
├── Workload Identity
├── Security Domain
├── Capability Set
├── Resource Budget
└── State
```

Mehrere Instanzen derselben Anwendung dürfen unterschiedliche Capability Sets besitzen.

## Standardautorität

Anwendungen starten mit minimaler Autorität.

```text
Application Start
       ↓
Minimal Runtime Capabilities
       ↓
Explicit Capability Acquisition
       ↓
Operation
```

Globale Benutzerrechte dürfen nicht automatisch übernommen werden.

## Ressourcen

Anwendungszugriffe werden über spezifische Capabilities modelliert:

```text
File Capability
Directory Capability
NovaFile Capability
Network Capability
Device Capability
Service Capability
IPC Capability
Compute Capability
Clipboard Capability
Notification Capability
```

Eine Capability für eine Ressource erzeugt keine Autorität über andere Ressourcen derselben Klasse.

## Benutzerfreigabe

Eine Benutzerentscheidung kann zur Erteilung einer Capability führen.

```text
Application Request
       ↓
Policy Evaluation
       ↓
User Decision
       ↓
Attenuated Capability
       ↓
Application
```

Die Oberfläche soll dabei den tatsächlichen Autoritätsumfang verständlich darstellen.

Beispiel:

```text
Erlauben:
"Dieses Dokument lesen"

statt:

"Zugriff auf alle Dateien"
```

## Dateiöffnung

Wählt der Benutzer eine Datei aus, kann NovaOS eine objektbezogene Capability erzeugen.

```text
User selects Document A
        ↓
Read Capability(Document A)
        ↓
Application
```

Die Anwendung benötigt dadurch keinen allgemeinen Zugriff auf das Benutzerverzeichnis.

## Services und Capabilities

Anwendungen sollen Systemfunktionen bevorzugt über Capabilities nutzen.

```text
Application
    ↓
Service Capability
    ↓
Nova Capability / Service
```

Dadurch können integrierte NovaOS-Fähigkeiten genutzt werden, ohne breite Systemrechte bereitzustellen.

## Dynamische Autorität

Anwendungen dürfen zusätzliche Autorität während der Laufzeit anfordern.

```text
Need
 ↓
Discovery
 ↓
Request
 ↓
Policy / User Decision
 ↓
Capability
```

Nicht mehr benötigte temporäre Capabilities sollen freigegeben werden können.

## Child Workloads

Von Anwendungen gestartete Prozesse erhalten keine automatische vollständige Autorität.

```text
Application
    ↓
Spawn Contract
    ↓
Selected + Attenuated Capabilities
    ↓
Child Workload
```

## Updates

Ein Anwendungsupdate darf bestehende Autorität nicht automatisch erweitern.

```text
Application v1
     ↓
Update
     ↓
Application v2
     ↓
Identity + Trust + Policy Re-Evaluation
```

Änderungen von Publisher, Application Identity oder sicherheitsrelevanten Eigenschaften können eine erneute Autorisierung erfordern.

## Revocation

Anwendungs-Capabilities müssen während der Laufzeit widerrufbar sein.

```text
Capability Revoked
       ↓
Application Capability Set
       ↓
Access Denied
```

Ein Neustart darf widerrufene Autorität nicht automatisch wiederherstellen.

## Persistente Berechtigungen

Dauerhafte Benutzerentscheidungen sollen als Policy oder Grant gespeichert werden, nicht als dauerhaft offengelegter Capability Token.

```text
Persistent Grant
      ↓
Application Start
      ↓
Policy Validation
      ↓
Fresh Capability
```

Damit können Trust-, Revocation- und Policy-Änderungen berücksichtigt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Application Identity
Workload Identity
Granted Capabilities
Persistent Grants
Temporary Capabilities
Delegation Relations
Revoked Capabilities
Security Domain
State
```

Dies ermöglicht eine verständliche Berechtigungsübersicht für Benutzer und Systemverwaltung.

## Normative Anforderungen

1. NovaOS MUSS Anwendungsautorität über explizite Capabilities modellieren.
2. Anwendungen DÜRFEN NICHT automatisch die vollständige Benutzerautorität übernehmen.
3. Anwendungen SOLLEN standardmäßig mit minimaler Autorität starten.
4. Datei-, Netzwerk-, Geräte-, IPC- und Service-Autorität MÜSSEN getrennt kontrollierbar sein.
5. Benutzerfreigaben SOLLEN möglichst objekt- und zweckbezogene Capabilities erzeugen.
6. Child Workloads DÜRFEN Autorität nur explizit erhalten.
7. Updates DÜRFEN bestehende Autorität NICHT automatisch erweitern.
8. Persistente Grants MÜSSEN bei Nutzung erneut gegen aktuelle Policy geprüft werden können.
9. Capability Revocation MUSS während laufender Anwendungen wirksam werden können.
10. Application Capability Sets MÜSSEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-CAPABILITY-SANDBOX-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-CAPABILITY-0014`

## Ergebnis

```text
Application Identity
        ↓
Capability Sandbox
        ↓
Minimal Explicit Authority
        ↓
Object- and Purpose-bound Capabilities
        ↓
Authorized Operation
```

NovaOS erhält damit ein Anwendungsmodell, bei dem Programme nicht mit den umfassenden Rechten des Benutzers arbeiten, sondern ausschließlich die für ihre konkrete Aufgabe erforderlichen Capabilities erhalten.