# NPSPEC-TEMP-SECURITY-0001 – Nova Temporary Resource Security

## Status

Angenommen

## Kategorie

Temporary Resources / Security

## Zweck

NovaOS definiert die Sicherheitsregeln für temporäre Ressourcen.

Temporäre Daten unterliegen denselben grundlegenden Schutzprinzipien wie persistente Daten und dürfen nicht aufgrund ihrer kurzen Lebensdauer schwächer geschützt werden.

## Grundprinzipien

```text
Temporary ≠ Unprotected
Visibility ≠ Authority
ResourceID Knowledge ≠ Access
Shared Temp ≠ Public Temp
Expired ≠ Immediately Inaccessible
```

## Sicherheitskontext

Jede temporäre Ressource ist einem Owner und Sicherheitskontext zugeordnet:

```text
Owner
  ↓
Security Context
  ↓
TempResource
  ↓
Authorized Handle
```

Der Scope allein erzeugt keine Zugriffsberechtigung.

## Zugriff

Zugriffe erfolgen capability-basiert.

```text
Access Request
      ↓
Capability Check
      ↓
TempResource
      ↓
Authorized Handle
```

Andere Prozesse, Programme, Solutions, Sessions oder Benutzer dürfen nicht allein durch Kenntnis einer `ResourceID` zugreifen.

## Isolation

NovaOS muss Temp-Ressourcen zwischen Sicherheitskontexten isolieren können.

Dies gilt insbesondere für:

```text
Process
Program
Solution
Workspace
Session
User
System
```

Gemeinsame Nutzung muss explizit autorisiert werden.

## Delegation

Temporäre Ressourcen dürfen kontrolliert an andere Sicherheitskontexte delegiert werden.

Dabei gilt:

```text
Delegated Authority
⊆
Original Authority
```

Eine Delegation darf keine stärkeren Rechte erzeugen als der delegierende Kontext besitzt.

## Sensitive Data

Temporäre Ressourcen können sensible Inhalte enthalten, beispielsweise:

```text
Credentials
Cryptographic Material
Private Documents
Decrypted Data
Authentication Data
```

Solche Ressourcen müssen entsprechend ihrer Security Policy geschützt und nach ihrer Verwendung sicher bereinigbar sein.

## Ablauf und Cleanup

Eine abgelaufene oder freigegebene Ressource darf nicht erneut einem anderen Kontext zugänglich gemacht werden, solange sensible Restdaten vorhanden sind.

```text
Release
  ↓
Revoke Access
  ↓
Secure Cleanup
  ↓
Reuse
```

## Recovery

Recovery darf keine frühere Authority automatisch wiederherstellen.

Wiederhergestellte Temp-Ressourcen müssen im aktuellen Sicherheitskontext erneut autorisiert werden.

## Cache

Cache-Einträge dürfen Schutzgrenzen ihrer Quelldaten nicht abschwächen.

Ein gemeinsamer Cache darf keine Informationen zwischen nicht autorisierten Sicherheitskontexten übertragen.

## Normative Anforderungen

1. Temp-Ressourcen MÜSSEN einem Sicherheitskontext zugeordnet sein.
2. Temp-Zugriffe MÜSSEN capability-basiert autorisierbar sein.
3. Kenntnis einer `ResourceID` DARF keine Authority erzeugen.
4. Unterschiedliche Sicherheitskontexte MÜSSEN isolierbar sein.
5. Gemeinsame Nutzung MUSS explizit autorisiert werden.
6. Delegierte Authority DARF die ursprüngliche Authority nicht überschreiten.
7. Sensible Temp-Daten MÜSSEN sicher bereinigbar sein.
8. Freigegebene Ressourcen DÜRFEN keine Restdaten an neue Owner offenlegen.
9. Recovery DARF frühere Authority nicht automatisch wiederherstellen.
10. Cache und Temp-Sharing DÜRFEN Sicherheitsgrenzen nicht umgehen.
11. Widerrufene Zugriffe MÜSSEN gemäß Revocation-Policy unwirksam werden können.
12. Temporäre Ressourcen DÜRFEN nicht schwächer geschützt werden, nur weil sie kurzlebig sind.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-RECOVERY-0001`
- `NPSPEC-TEMP-CLEANUP-0001`
- `NPSPEC-TEMP-CACHE-0001`
- `NPSPEC-TEMP-QUOTA-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS schützt temporäre Ressourcen durch dieselben grundlegenden Capability-, Isolations- und Sicherheitsprinzipien wie andere Systemressourcen. Kurzlebigkeit reduziert weder Schutzbedarf noch Zugriffsanforderungen, und sensible Restdaten werden vor einer Wiederverwendung kontrolliert bereinigt.