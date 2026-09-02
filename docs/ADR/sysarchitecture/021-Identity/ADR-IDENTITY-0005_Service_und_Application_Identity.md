# ADR-IDENTITY-0005 – Service und Application Identity

## Status

Angenommen

## Kategorie

Identity Architecture / Application Identity / Service Identity

## Kontext

Anwendungen und Systemdienste benötigen eine stabile Identität, die unabhängig von Prozessinstanzen, Installationspfaden, Versionen, Netzwerkadressen oder konkreten Capability Providern besteht.

Dabei müssen zwei unterschiedliche Konzepte unterschieden werden:

- **Application Identity** – Identität einer Anwendung
- **Service Identity** – Identität eines angebotenen Dienstes

Eine Anwendung kann mehrere Dienste bereitstellen und ein Dienst kann durch mehrere Instanzen oder Provider realisiert werden.

## Entscheidung

NovaOS führt **Application Identity** und **Service Identity** als eigenständige stabile Identity-Klassen ein.

```text
Application Identity
      │
      ├── Service Identity A
      │       ├── Workload A
      │       └── Workload B
      │
      └── Service Identity B
              └── Workload C
```

Beide Identities sind von ihren aktuellen Ausführungsinstanzen getrennt.

## Grundprinzip

```text
Application Identity = Which application is this?

Service Identity     = Which logical service is this?

Workload Identity    = Which running instance is this?
```

Diese Ebenen dürfen nicht miteinander vermischt werden.

## Application Identity

Die Application Identity repräsentiert die logische Anwendung.

Sie bleibt grundsätzlich stabil über:

- Neustarts,
- Updates,
- Prozesswechsel,
- Installationspfadänderungen,
- mehrere laufende Instanzen.

Eine neue Anwendungsversion erzeugt daher nicht automatisch eine neue Application Identity.

## Service Identity

Die Service Identity repräsentiert einen logisch adressierbaren Dienst.

```text
Service Identity
      ↓
Service Instances
      ↓
Local / IPC / Remote Provider
```

Dadurch kann ein Dienst seine konkrete Instanz, seinen Provider oder seinen Ausführungsort wechseln, ohne seine logische Identität zu verlieren.

## Verhältnis zu Capabilities

Service Identity und Capability Identity erfüllen unterschiedliche Aufgaben.

```text
Service Identity    = Who provides the service?

Capability Identity = What functionality is provided?
```

Ein Service kann mehrere Capabilities bereitstellen und dieselbe Capability kann von mehreren Services oder Providern implementiert werden.

## Verhältnis zu Authority

Application oder Service Identity erzeugt keine automatische Authority.

```text
Identity ≠ Authority
```

Zugriffsrechte werden weiterhin durch Capabilities und Policies bestimmt.

Eine Identity dient dabei als verifizierbarer Kontext für Trust-, Security- und Policy-Entscheidungen.

## Updates und Replacement

Updates oder Hot Replacement dürfen die bestehende Identity übernehmen, wenn die Identity Continuity verifiziert wurde.

```text
Application V1
      ↓ Update
Application V2
      ↓
Same Application Identity
```

Dasselbe gilt für Service Replacement und Migration.

## Verteilte Dienste

Service Identities müssen unabhängig von Netzwerkadressen und Endpoints sein.

```text
Service Identity
      ↓
Resolution
      ↓
Current Endpoint(s)
```

Dadurch bleiben Service-Aufrufe auch bei Connection Migration, Failover oder Provider-Wechsel logisch auf denselben Dienst bezogen.

## Normative Anforderungen

1. NovaOS MUSS Application Identity und Service Identity als eigenständige Identity-Klassen unterstützen.
2. Beide MÜSSEN von Process IDs, Workload Identities, Adressen und Installationspfaden getrennt sein.
3. Application Identity MUSS über normale Updates und Neustarts stabil bleiben können.
4. Service Identity MUSS unabhängig von konkreten Service-Instanzen sein.
5. Mehrere Workloads MÜSSEN derselben Application oder Service Identity zugeordnet werden können.
6. Eine Anwendung MUSS mehrere Service Identities besitzen können.
7. Service Identity und Capability Identity MÜSSEN getrennte Konzepte bleiben.
8. Identity DARF keine automatische Authority erzeugen.
9. Identity Continuity bei Update, Migration oder Replacement MUSS verifizierbar sein.
10. Service Identity MUSS unabhängig von Netzwerkadressen und Endpoints sein.
11. Lokale und entfernte Services SOLLEN dasselbe Identity-Modell verwenden.
12. Identity-Bindings MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- stabile Anwendungsidentität über Updates hinweg,
- Services bleiben trotz Migration oder Failover identifizierbar,
- klare Trennung zwischen Anwendung, Dienst, Capability und Workload,
- bessere Unterstützung für Hot Replacement,
- einheitliches lokales und verteiltes Service-Modell.

### Negative Konsequenzen

- Application-, Service- und Workload-Identity müssen getrennt verwaltet werden,
- Identity Continuity benötigt verifizierte Bindings.

## Verworfene Alternativen

### Prozess als Application Identity

Verworfen, da Prozesse kurzlebige Ausführungsinstanzen sind.

### Capability als Service Identity

Verworfen, da Capability Funktionalität und nicht den Anbieter identifiziert.

### Netzwerkadresse als Service Identity

Verworfen, da Adressen veränderliche Erreichbarkeitsinformationen darstellen.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-IDENTITY-0003_Ephemeral_Workload_Identity`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-SERVICE-BINDING-0001`
- `NPSPEC-IDENTITY-APPLICATION-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-SERVICE-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-SERVICE-APPLICATION-TEST-0001`

## Ergebnis

NovaOS trennt Anwendung, Dienst und konkrete Ausführungsinstanz eindeutig:

```text
Application Identity
        ↓
Service Identity
        ↓
Ephemeral Workload Identity
        ↓
Process / Execution Domain / Endpoint
```

Damit bleiben Anwendungen und Dienste stabil identifizierbar, während ihre konkreten Implementierungen, Prozesse, Provider und Ausführungsorte unabhängig verändert werden können.