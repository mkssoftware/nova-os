# ADR-UPDATE-0017 – Canary und Staged Rollout

## Status

Angenommen

## Kategorie

Update / Rollout / Resilience

## Kontext

Auch vollständig getestete Updates können auf realer Hardware, bestimmten Konfigurationen oder seltenen Workloads unerwartete Fehler verursachen.

Eine sofortige Verteilung an alle NovaOS-Systeme würde solche Fehler gleichzeitig auf eine große Anzahl von Geräten übertragen.

## Entscheidung

NovaOS unterstützt **Canary und Staged Rollouts**.

Updates können schrittweise an definierte Gerätegruppen verteilt werden.

```text
Update
  ↓
Canary
  ↓
Stage 1
  ↓
Stage 2
  ↓
General Rollout
```

Der Übergang zur nächsten Stufe erfolgt nur, wenn die vorherige Stufe die definierten Health-Kriterien erfüllt.

## Canary

Eine kleine Gruppe erhält das Update zuerst.

```text
Candidate Update
      ↓
Canary Group
      ↓
Health Evaluation
```

Die Canary-Gruppe soll repräsentative Hardware- und Systemkonfigurationen enthalten.

## Staged Rollout

Nach erfolgreicher Canary-Phase wird die Verteilung kontrolliert erweitert.

```text
1 %
 ↓
10 %
 ↓
25 %
 ↓
50 %
 ↓
100 %
```

Die konkreten Stufen werden durch Rollout Policy definiert und sind nicht fest vorgegeben.

## Health-Kriterien

Vor der Erweiterung des Rollouts können mindestens folgende Signale berücksichtigt werden:

```text
Boot Success
Crash Rate
Update Failure Rate
Rollback Rate
Service Health
Critical Errors
```

Die Kriterien müssen für das jeweilige Update definiert sein.

## Rollout Stop

Werden definierte Fehlergrenzen überschritten, wird die weitere Verteilung gestoppt.

```text
Health Failure
     ↓
Pause Rollout
     ↓
Analyze
     ↓
Rollback / Fix / Continue
```

Bereits aktualisierte Systeme werden entsprechend ihrer Recovery Policy behandelt.

## Gerätegruppen

Rollout-Gruppen können anhand geeigneter technischer Eigenschaften gebildet werden.

Beispiele:

```text
Hardware Class
Architecture
Device Model
System Configuration
Update Channel
```

Die Gruppierung darf keine unnötige personenbezogene Profilbildung erzeugen.

## Offline-Systeme

Ein System, das während einer Rollout-Stufe offline war, erhält nicht automatisch eine inzwischen zurückgezogene Version.

Vor der Installation muss der aktuelle Rollout- und Trust-Status erneut geprüft werden, soweit dieser verfügbar ist.

## Security Updates

Kritische Sicherheitsupdates können beschleunigte Rollout Policies verwenden.

```text
Normal Update
→ Conservative Staging

Critical Security Update
→ Accelerated Staging
```

Auch beschleunigte Rollouts dürfen grundlegende Integritäts- und Trust-Prüfungen nicht umgehen.

## Normative Anforderungen

1. NovaOS MUSS Canary und Staged Rollouts unterstützen können.
2. Rollout-Stufen MÜSSEN durch Policy definierbar sein.
3. Der Übergang zur nächsten Stufe MUSS von definierten Health-Kriterien abhängig gemacht werden können.
4. Fehlergrenzen MÜSSEN einen Rollout automatisch pausieren können.
5. Zurückgezogene Updates DÜRFEN nicht weiter regulär verteilt werden.
6. Rollout-Gruppen SOLLEN technische statt personenbezogene Merkmale verwenden.
7. Kritische Sicherheitsupdates DÜRFEN beschleunigte Rollout Policies verwenden.
8. Staged Rollout DARF Integritäts-, Trust- oder Update-Verification nicht ersetzen.

## Konsequenzen

### Positive Konsequenzen

- fehlerhafte Updates erreichen zunächst nur wenige Systeme,
- reale Probleme werden früher erkannt,
- Rollouts können automatisch gestoppt werden,
- unterschiedliche Hardwareklassen können kontrolliert getestet werden.

### Negative Konsequenzen

- nicht alle Systeme erhalten Updates gleichzeitig,
- Rollout-Zustände und Health-Metriken müssen verwaltet werden,
- zusätzliche Rollout-Infrastruktur wird benötigt.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-UPDATE-0010_Package_Signing_Provenance_und_SBOM`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-CANARY-0001`
- `NPSPEC-UPDATE-STAGED-ROLLOUT-0001`
- `NPSPEC-UPDATE-ROLLOUT-HEALTH-0001`

## Ergebnis

NovaOS verteilt neue Updates kontrolliert:

```text
Candidate
   ↓
Canary
   ↓
Verify Health
   ↓
Staged Expansion
   ↓
General Release
```

Der zentrale Grundsatz lautet:

```text
Erst klein validieren,
dann kontrolliert ausweiten.
```