# NPSPEC-NETWORK-MIGRATION-0001 – Nova Network Migration

## Status

Angenommen

## Kategorie

Network / Mobility / Connection Migration

## Zweck

NovaOS definiert Network Migration als kontrollierten Wechsel eines bestehenden Netzwerkflows oder einer Verbindung zwischen Netzwerkpfaden, Interfaces oder Adressen.

```text
Active Flow
    ↓
Path Change
    ↓
Validation
    ↓
Migration
    ↓
Continued Flow
```

Ziel ist, Netzwerkkommunikation bei Wi-Fi-/Ethernet-Wechseln, VPN-Wechseln, Mobilität oder Pfadausfällen möglichst ohne unnötigen Verbindungsabbruch fortzuführen.

## Grundprinzip

```text
Connection Identity ≠ Network Path
Flow Identity ≠ IP Address
Migration ≠ Reconnection
New Path ≠ Trusted Path
```

Eine Migration darf Identität, Sicherheit und bestehende Berechtigungen nicht verändern.

## Migrationsmodell

Ein Migrationsvorgang besitzt mindestens:

```text
NetworkMigration
├── MigrationID
├── FlowID
├── SourcePath
├── TargetPath
├── State
└── Reason
```

Optional:

```text
ConnectionID
Target Interface
Target Address
Security Context
ExecutionContract
Deadline
```

## Zustandsmodell

```text
Requested
    ↓
Validating
    ↓
Preparing
    ↓
Switching
    ↓
Verifying
    ↓
Completed
```

Fehler führen kontrolliert zu:

```text
Rollback
Failed
Reconnect Required
```

Der bisherige Pfad soll während der Vorbereitung erhalten bleiben, sofern dies technisch möglich und sicher ist.

## Auslöser

Migration darf ausgelöst werden durch:

```text
Interface Loss
Path Degradation
Network Change
VPN Change
Mobility
QoS Requirement
Explicit Policy
User Decision
```

Eine reine Optimierung darf Hard Security-, Trust- oder Sovereignty-Anforderungen nicht überschreiben.

## Pfadvalidierung

Vor einer Migration muss der Zielpfad geprüft werden.

```text
Candidate Path
     ↓
Routing
Security
Trust
Policy
QoS
Reachability
     ↓
Valid / Invalid
```

`Available` oder `Reachable` allein reicht nicht als Autorisierung.

## Transport-Integration

Nicht jedes Transportprotokoll unterstützt echte Connection Migration.

```text
QUIC → Native Migration
TCP  → abhängig von Erweiterung / Reconnect
UDP  → Flow-/Anwendungssemantik
```

NovaOS darf keine transparente Migration vortäuschen, wenn das verwendete Transportprotokoll die notwendige Semantik nicht unterstützt.

In diesem Fall muss ein kontrollierter Reconnect oder eine anwendungsspezifische Wiederaufnahme erfolgen.

## QUIC

QUIC soll als primärer Transport für native Connection Migration integriert werden.

```text
QUIC Connection
      ↓
New Network Path
      ↓
Path Validation
      ↓
Connection Continues
```

Connection IDs dürfen die Verbindung unabhängig von einer einzelnen IP-Adresse oder einem einzelnen Netzwerkpfad identifizieren.

## Multipath

Network Migration verwendet die gemeinsame Multipath-Infrastruktur zur Auswahl alternativer Pfade.

```text
Current Path
     ↓ degraded
Multipath
 ├── Path A
 ├── Path B
 └── Path C
     ↓
Target Path
```

Pfadauswahl und Migration bleiben logisch getrennte Schritte.

## VPN

VPN-Verbindungen dürfen bei einem Wechsel des zugrunde liegenden Interfaces migriert werden, sofern das VPN-Protokoll dies unterstützt.

```text
VPN
 ↓
Wi-Fi
 ↓ migration
Ethernet
```

Ein VPN-Kill-Switch darf während einer Migration nicht unbeabsichtigt umgangen werden.

## Network Namespace

Migration darf Namespace-Grenzen nicht automatisch überschreiten.

Ein Wechsel in einen anderen Network Namespace benötigt eine explizite Autorisierung und darf als eigenständige Netzwerkoperation behandelt werden.

## QoS und Congestion

Pfadmetriken dürfen Migrationen beeinflussen.

```text
Latency
Bandwidth
Loss
Congestion
Stability
```

Kurze Schwankungen sollen nicht zu ständigem Wechsel zwischen Pfaden führen.

Hysterese und Mindeststabilitätszeiten dürfen verwendet werden.

## Sicherheit

Bei jeder Migration müssen mindestens geprüft werden:

```text
Target Path Authorization
Firewall Policy
Network Namespace
Trust Requirements
Sovereignty Requirements
Transport Security
```

Eine bestehende sichere Verbindung darf durch Migration nicht unbemerkt auf einen unzulässigen Pfad wechseln.

## Fehlerbehandlung

Schlägt die Migration fehl, muss NovaOS abhängig von Transport und Policy:

```text
Keep Old Path
Rollback
Try Alternative Path
Reconnect
Fail Operation
```

verwenden können.

Ein Fehler darf nicht automatisch zu einem ungeschützten Fallback führen.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Migration Attempts
Candidate Paths
Parallel Validation
Temporary Buffers
Migration State
Reconnect Attempts
```

Wiederholte Pfadwechsel dürfen keine unbegrenzten Ressourcen verbrauchen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
MigrationID
FlowID
ConnectionID
SourcePath
TargetPath
Reason
State
Start Time
Duration
Result
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Network Migration unterstützen können.
2. Flow- und Connection-Identität DÜRFEN NICHT ausschließlich an eine IP-Adresse gebunden sein.
3. Ein Zielpfad MUSS vor der Migration validiert werden.
4. Migration DARF Sicherheits-, Firewall-, Trust- oder Sovereignty-Regeln NICHT umgehen.
5. NovaOS MUSS Transportprotokollgrenzen bei Migration berücksichtigen.
6. QUIC SOLL native Connection Migration verwenden können.
7. Nicht migrationsfähige Transporte MÜSSEN kontrollierten Reconnect ermöglichen können.
8. Multipath SOLL Kandidaten für Migration bereitstellen können.
9. VPN- und Namespace-Regeln MÜSSEN während Migration erhalten bleiben.
10. Kurzzeitige Netzwerkschwankungen SOLLEN nicht unnötig Migrationen auslösen.
11. Fehlgeschlagene Migrationen MÜSSEN kontrolliert behandelt werden.
12. Migrationen und ihre Ergebnisse SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-NETWORK-ROUTING-0001`
- `NPSPEC-NETWORK-MULTIPATH-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-VPN-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-CONGESTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-NETWORK-0023`

## Ergebnis

```text
Stable Flow / Connection Identity
              ↓
       Path Monitoring
              ↓
      Candidate Selection
              ↓
       Path Validation
              ↓
     Controlled Migration
              ↓
 Continued Communication
```

NovaOS erhält damit eine gemeinsame Migrationsarchitektur, durch die Netzwerkkommunikation kontrolliert zwischen Interfaces und Pfaden wechseln kann, ohne Identität, Sicherheit oder Ressourcenregeln an einen einzelnen physischen Netzwerkpfad zu binden.