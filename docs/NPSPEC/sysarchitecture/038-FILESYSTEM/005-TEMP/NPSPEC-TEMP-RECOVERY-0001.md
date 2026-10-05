# NPSPEC-TEMP-RECOVERY-0001 – Nova Temporary Resource Recovery

## Status

Angenommen

## Kategorie

Temporary Resources / Recovery

## Zweck

NovaOS definiert die Behandlung temporärer Ressourcen nach Abstürzen, unerwarteten Neustarts oder unterbrochenen Sitzungen.

Temporäre Ressourcen dürfen wiederhergestellt werden, wenn sie für Recovery relevant sind, bleiben jedoch grundsätzlich temporär und dürfen nicht automatisch zu persistenten Benutzerdaten werden.

## Grundprinzipien

```text
Recovery ≠ Persistence
Recovery ≠ Backup
Recoverable ≠ Authoritative
Recovered ≠ Trusted
Crash ≠ Automatic Data Loss
```

## Recovery-Modell

Temporäre Ressourcen können eine Recovery-Policy besitzen:

```text
TempResource
├── ResourceID
├── OwnerID
├── Scope
├── State
└── RecoveryPolicy
```

Mögliche Policies:

```text
Discard
RecoverIfValid
PreserveForDiagnostics
ExplicitRecovery
```

## Recovery-Ablauf

```text
Unexpected Termination
        ↓
Discover Temp Resources
        ↓
Validate
        ↓
Evaluate Recovery Policy
        ↓
Recover / Preserve / Discard
```

Nur eindeutig zuordenbare und ausreichend valide Ressourcen dürfen wiederhergestellt werden.

## Wiederherstellbare Daten

Recovery kann beispielsweise sinnvoll sein für:

```text
Uncommitted Work
Generated Intermediate Data
Workspace Recovery State
Crash Diagnostics
Temporary Editing State
```

Reine Cache-Daten sollen normalerweise verworfen und bei Bedarf neu erzeugt werden.

## Validierung

Vor einer Wiederverwendung muss NovaOS mindestens prüfen:

```text
Owner
Scope
Integrity
Version
Expiration
Security Context
```

Abgelaufene, beschädigte oder nicht eindeutig zuordenbare Ressourcen dürfen nicht stillschweigend wiederverwendet werden.

## Übernahme in persistente Daten

Soll eine wiederhergestellte temporäre Ressource dauerhaft erhalten bleiben, muss eine explizite Übernahme erfolgen:

```text
Recovered Temp Resource
        ↓
Explicit Commit / Save
        ↓
Persistent Object
```

Dabei entsteht die für den persistenten Speicher erforderliche Identität und Lebensdauer.

## Sicherheit

Recovery stellt keine frühere Authority automatisch wieder her.

Berechtigungen müssen im aktuellen Sicherheitskontext erneut validiert werden.

Sensible nicht wiederherstellbare Daten müssen sicher bereinigbar sein.

## Normative Anforderungen

1. Temporäre Ressourcen DÜRFEN eine Recovery-Policy besitzen.
2. NovaOS MUSS nach unerwartetem Abbruch verbleibende Temp-Ressourcen erkennen können.
3. Wiederherstellbare Ressourcen MÜSSEN vor Nutzung validiert werden.
4. Abgelaufene Ressourcen DÜRFEN nicht automatisch wiederhergestellt werden.
5. Cache-Daten SOLLEN bevorzugt neu erzeugt statt wiederhergestellt werden.
6. Recovery DARF temporäre Ressourcen nicht automatisch persistent machen.
7. Eine dauerhafte Übernahme MUSS explizit erfolgen.
8. Recovery DARF keine frühere Authority automatisch wiederherstellen.
9. Beschädigte oder nicht eindeutig zuordenbare Ressourcen MÜSSEN isoliert oder verworfen werden.
10. Sensible verworfene Ressourcen MÜSSEN sicher bereinigbar sein.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-TTL-0001`
- `NPSPEC-TEMP-CACHE-0001`
- `NPSPEC-WORKSPACE-RECOVERY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS kann nach Abstürzen oder unerwarteten Unterbrechungen relevante temporäre Ressourcen kontrolliert wiederherstellen, ohne temporäre Daten mit persistenten Daten gleichzusetzen oder frühere Berechtigungen ungeprüft zu übernehmen.