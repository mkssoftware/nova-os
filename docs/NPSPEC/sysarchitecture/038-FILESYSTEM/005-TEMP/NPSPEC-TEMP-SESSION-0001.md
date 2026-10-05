# NPSPEC-TEMP-SESSION-0001 – Nova Session Temporary Resources

## Status

Angenommen

## Kategorie

Temporary Resources / Session

## Zweck

NovaOS definiert temporäre Ressourcen, deren Lebensdauer an eine Benutzersitzung gebunden ist.

Session-Temp ermöglicht Daten und Ressourcen, die von mehreren Prozessen derselben Sitzung verwendet werden können, aber nach Ende der Sitzung nicht dauerhaft erhalten bleiben müssen.

## Grundprinzipien

```text
Session Temp ≠ User Data
Session Temp ≠ Process Temp
Session Temp ≠ Persistent Storage
Session Lifetime → Temp Lifetime
Session End → Resource Cleanup
```

## Sitzungsbindung

Jede Session-Temp-Ressource wird einer konkreten Sitzung zugeordnet:

```text
SessionID
   ↓
Session Temp Scope
   ↓
TempResource
```

Andere Sitzungen erhalten dadurch keinen automatischen Zugriff.

## Lebenszyklus

```text
Session Start
     ↓
Create Temp Resources
     ↓
Use / Share
     ↓
Session End
     ↓
Reclaim
     ↓
Destroy
```

Ressourcen können bereits während der Sitzung explizit freigegeben werden.

## Nutzung

Session-Temp eignet sich beispielsweise für:

```text
Session Cache
Generated Data
Clipboard-related Data
Temporary UI Data
Shared Intermediate Results
Session IPC Resources
```

Mehrere Prozesse derselben Sitzung dürfen Ressourcen gemeinsam verwenden, sofern die erforderliche Authority vorhanden ist.

## Speicherung

Programme dürfen keine feste physische Position für Session-Temp voraussetzen.

```text
Temp Request
     ↓
Temp Service
     ↓
Session Scope
     ↓
Storage / Memory Selection
```

NovaOS entscheidet über die tatsächliche Ablage.

## Sitzungsende

Nach Logout oder anderweitigem Ende der Sitzung werden verbleibende Session-Temp-Ressourcen freigebbar.

```text
Active
  ↓
Session End
  ↓
Reclaimable
  ↓
Destroyed
```

Ein Neustart darf ebenfalls alle nicht ausdrücklich anders definierten Session-Temp-Ressourcen verwerfen.

## Isolation

Verschiedene Sitzungen desselben oder unterschiedlicher Benutzer bleiben voneinander getrennt.

```text
Session A Temp
      ≠
Session B Temp
```

Das Wissen über eine Ressource erzeugt keine Zugriffsberechtigung.

## Resource Economy

Session-Temp unterliegt Ressourcenbudgets.

NovaOS kann Grenzen für:

```text
Storage
Memory
Object Count
Lifetime
```

festlegen und nicht mehr benötigte Ressourcen zurückgewinnen.

## Sicherheit

Session-Temp erzeugt keine zusätzliche Authority.

Zugriffe werden weiterhin über den jeweiligen Capability- und Sicherheitskontext geprüft.

Sensible Session-Daten müssen sicher bereinigbar sein.

## Normative Anforderungen

1. Session-Temp-Ressourcen MÜSSEN einer `SessionID` zugeordnet sein.
2. Unterschiedliche Sitzungen MÜSSEN voneinander isolierbar sein.
3. Session-Temp DARF nicht als dauerhafte Benutzerdatenspeicherung verwendet werden.
4. Ressourcen MÜSSEN während der Sitzung explizit freigegeben werden können.
5. Nach Sitzungsende MÜSSEN verbleibende Ressourcen reclaimable werden.
6. Programme DÜRFEN keine feste physische Session-Temp-Position voraussetzen.
7. Gemeinsame Nutzung zwischen Prozessen MUSS autorisiert sein.
8. Session-Temp MUSS Ressourcenbudgets berücksichtigen.
9. Session-Temp DARF keine zusätzliche Authority erzeugen.
10. Sensible Session-Temp-Ressourcen MÜSSEN sicher bereinigbar sein.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-PROCESS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-USERSPACE-TEMP-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS besitzt einen isolierten temporären Ressourcenbereich pro Sitzung. Ressourcen können kontrolliert zwischen autorisierten Prozessen derselben Sitzung genutzt und nach Sitzungsende automatisch zurückgewonnen werden.