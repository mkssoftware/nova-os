# ADR-UPDATE-0018 – Rolling Cluster Updates

## Status

Angenommen

## Kategorie

Update / Distributed Systems / Availability

## Kontext

NovaOS kann in Verbünden aus mehreren Nodes betrieben werden.

Ein gleichzeitiges Update aller Nodes kann die Verfügbarkeit eines Dienstes gefährden und Fehler auf den gesamten Cluster übertragen.

Updates müssen deshalb schrittweise erfolgen können, während der Cluster weiterhin funktionsfähig bleibt.

## Entscheidung

NovaOS unterstützt **Rolling Cluster Updates**.

Nodes werden kontrolliert nacheinander oder in begrenzten Gruppen aktualisiert.

```text
Cluster
 ├── Node A → Update
 ├── Node B → Running
 └── Node C → Running
```

Erst nach erfolgreicher Verification wird die nächste Gruppe aktualisiert.

## Grundprinzip

```text
Drain
  ↓
Update
  ↓
Verify
  ↓
Rejoin
  ↓
Next Node
```

Während des Rollouts muss die erforderliche Cluster-Verfügbarkeit erhalten bleiben.

## Quorum

Vor jedem Update-Schritt muss geprüft werden, ob Quorum und notwendige Redundanz erhalten bleiben.

```text
Can remove Node?
      ↓
Quorum safe?
 ┌────┴────┐
 ↓         ↓
Yes        No
 ↓         ↓
Update    Wait
```

Ein Update darf keinen vorhersehbaren Verlust des erforderlichen Quorums verursachen.

## Drain

Vor dem Update wird ein Node soweit erforderlich aus aktiven Workloads genommen.

```text
Active Node
    ↓
Drain
    ↓
Quiescent
    ↓
Update
```

Laufende Arbeit wird abgeschlossen, übertragen oder entsprechend ihrer Semantik behandelt.

## Versionskompatibilität

Während eines Rolling Updates können unterschiedliche Versionen gleichzeitig aktiv sein.

```text
Node A → V2
Node B → V1
Node C → V1
```

Cluster-Protokolle müssen deshalb für den definierten Update-Pfad kompatible Mischversionen unterstützen.

Nicht kompatible Versionssprünge benötigen einen anderen Update-Mechanismus.

## Verification

Nach dem Update eines Nodes werden mindestens dessen:

```text
Boot Health
Service Health
Cluster Membership
Protocol Compatibility
```

geprüft.

Erst danach wird der Node wieder vollständig in den Cluster aufgenommen.

## Rollout Stop

Schlägt ein Update oder die Verification fehl, wird der Rolling Update gestoppt.

```text
Node Update Failed
       ↓
Stop Rollout
       ↓
Rollback / Recover
```

Weitere Nodes dürfen nicht automatisch mit derselben fehlerhaften Version aktualisiert werden.

## Normative Anforderungen

1. NovaOS MUSS Rolling Updates für Cluster unterstützen können.
2. Updates MÜSSEN in begrenzten Node-Gruppen ausführbar sein.
3. Quorum und erforderliche Redundanz MÜSSEN vor jedem Update-Schritt geprüft werden.
4. Nodes MÜSSEN vor dem Update kontrolliert drainbar sein.
5. Unterstützte Mischversionen MÜSSEN explizit definiert sein.
6. Aktualisierte Nodes MÜSSEN vor dem Rejoin verifiziert werden.
7. Ein fehlgeschlagener Update-Schritt MUSS den weiteren Rollout stoppen können.
8. Rolling Updates DÜRFEN definierte Availability- und Consistency-Contracts nicht verletzen.

## Konsequenzen

### Positive Konsequenzen

- Cluster können während Updates verfügbar bleiben.
- Fehler betreffen zunächst nur einen begrenzten Teil des Clusters.
- Updates können kontrolliert gestoppt und zurückgerollt werden.
- Redundanz und Quorum werden Teil der Update-Entscheidung.

### Negative Konsequenzen

- Mischversionen müssen zeitweise unterstützt werden.
- Cluster-Updates benötigen zusätzliche Orchestrierung.
- Updates können länger dauern.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0008_Side_By_Side_Versionen`
- `ADR-UPDATE-0017_Canary_und_Staged_Rollout`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0008_Health_State_Model`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-ROLLING-CLUSTER-0001`
- `NPSPEC-UPDATE-CLUSTER-DRAIN-0001`
- `NPSPEC-UPDATE-CLUSTER-COMPATIBILITY-0001`

## Ergebnis

NovaOS aktualisiert Cluster schrittweise:

```text
Select Node
    ↓
Check Quorum
    ↓
Drain
    ↓
Update
    ↓
Verify
    ↓
Rejoin
    ↓
Next Node
```

Der zentrale Grundsatz lautet:

```text
Ein Cluster wird aktualisiert,
ohne seine notwendige
Verfügbarkeit aufzugeben.
```