# NPSPEC-TEMP-TTL-0001 – Nova Temporary Resource TTL

## Status

Angenommen

## Kategorie

Temporary Resources / Lifetime

## Zweck

NovaOS definiert Time-to-Live-Regeln für temporäre Ressourcen.

TTL begrenzt die maximale Gültigkeitsdauer einer temporären Ressource und ermöglicht deren automatische Freigabe, wenn sie nicht mehr benötigt wird.

## Grundprinzipien

```text
TTL ≠ Lifetime Scope
TTL ≠ Permission
TTL Expired ≠ Immediate Physical Deletion
TTL Extension ≠ New Resource
Expiration → Reclaimable
```

## TTL-Modell

Eine temporäre Ressource kann zusätzlich zu ihrem normalen Lebenszyklus eine zeitliche Begrenzung besitzen:

```text
TempResource
├── ResourceID
├── CreatedAt
├── TTL
├── ExpiresAt
└── State
```

Dabei gilt grundsätzlich:

```text
ExpiresAt = CreatedAt + TTL
```

## Ablauf

```text
Resource Created
      ↓
TTL Active
      ↓
Resource Used
      ↓
TTL Expires
      ↓
Expired
      ↓
Reclaimable
      ↓
Destroyed
```

Eine Ressource darf bereits vor Ablauf der TTL explizit freigegeben werden.

## Verhältnis zum Scope

TTL ergänzt den normalen Scope-Lebenszyklus.

Beispiel:

```text
Process Exit
      oder
TTL Expiration
      ↓
Resource Reclaimable
```

Das zuerst eintretende gültige Lebensende kann die Ressource freigeben.

## TTL-Verlängerung

Eine TTL darf verlängert oder erneuert werden, sofern dies durch die jeweilige Policy erlaubt ist.

```text
Active Resource
      ↓
Renew TTL
      ↓
New ExpiresAt
```

Eine Verlängerung verändert nicht die `ResourceID`.

Unbegrenzte Verlängerungen können durch Ressourcenbudgets oder Policies verhindert werden.

## Ablauf und Nutzung

Nach Ablauf der TTL dürfen keine neuen regulären Zugriffe auf die Ressource begonnen werden.

Bereits laufende Operationen werden entsprechend der Ressourcen- und Sicherheits-Policy abgeschlossen, abgebrochen oder kontrolliert beendet.

## Resource Economy

TTL unterstützt die automatische Rückgewinnung temporärer Ressourcen.

Unter Ressourcendruck darf NovaOS bereits abgelaufene Ressourcen bevorzugt reclaimen.

TTL darf jedoch nicht verwendet werden, um bestehende Ressourcenbudgets zu umgehen.

## Sicherheit

TTL verändert keine Authority.

Der Ablauf einer Ressource darf keine Handles oder Referenzen erzeugen, die anschließend auf eine andere Ressource umgebunden werden.

Sensible Ressourcen müssen nach Ablauf sicher bereinigbar sein.

## Normative Anforderungen

1. Temporäre Ressourcen DÜRFEN eine TTL besitzen.
2. Eine TTL MUSS einen eindeutig bestimmbaren Ablaufzeitpunkt ergeben.
3. Abgelaufene Ressourcen MÜSSEN als `Expired` erkennbar sein.
4. Abgelaufene Ressourcen MÜSSEN reclaimable werden können.
5. Ressourcen DÜRFEN vor Ablauf ihrer TTL freigegeben werden.
6. TTL und Scope-Lebensdauer MÜSSEN gemeinsam berücksichtigt werden.
7. TTL-Verlängerungen MÜSSEN policykontrolliert sein.
8. Eine TTL-Verlängerung DARF die `ResourceID` nicht verändern.
9. TTL DARF Ressourcenbudgets nicht umgehen.
10. TTL-Ablauf DARF keine zusätzliche Authority erzeugen.
11. Abgelaufene Identitäten DÜRFEN nicht still auf andere Ressourcen umgebunden werden.
12. Sensible Ressourcen MÜSSEN nach Ablauf sicher bereinigbar sein.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-PROCESS-0001`
- `NPSPEC-TEMP-SESSION-0001`
- `NPSPEC-TEMP-BOOT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS kann temporäre Ressourcen zusätzlich zu ihrem Scope durch eine definierte maximale Gültigkeitsdauer begrenzen. Abgelaufene Ressourcen werden eindeutig erkannt, kontrolliert aus der Nutzung genommen und für die automatische Rückgewinnung freigegeben.