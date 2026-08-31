# ADR-STORAGE-0014 – Snapshots und Rollback

## Status

Angenommen

## Kategorie

Storage / Snapshots / Rollback / Versioning / Recovery

## Kontext

NovaOS besitzt mit Versioned Storage und Transactional Storage bereits die Grundlagen, um konsistente historische Storage-Zustände zu erhalten.

Darauf aufbauend werden Snapshots benötigt, um einen definierten Zustand mehrerer Objekte, eines Volumes oder einer anderen Storage-Domäne festzuhalten.

Snapshots sollen insbesondere verwendet werden für:

- Systemupdates,
- Softwareinstallationen,
- Konfigurationsänderungen,
- Recovery,
- Backups,
- Benutzer-Rollback.

Ein Snapshot darf dabei nicht grundsätzlich eine vollständige physische Kopie aller enthaltenen Daten erfordern.

---

## Entscheidung

NovaOS unterstützt **Snapshots und Rollback als native Funktionen des Storage-Modells**.

Ein Snapshot referenziert einen konsistenten, logisch unveränderlichen Storage-Zustand.

```text
Storage State
     ↓
Snapshot S1
     ↓
Further Changes
     ↓
Current State
```

Rollback veröffentlicht einen auf einem früheren Snapshot basierenden Zustand erneut als aktuellen Zustand.

Snapshots bauen auf Transactional Storage, Versioned Storage und Content-Addressed Storage auf.

---

## Grundprinzip

```text
Snapshot preserves a state.

Rollback restores its semantics.
```

---

## Snapshot-Modell

Ein Snapshot kann einen definierten Satz von Objektversionen und Storage-Zuständen zusammenfassen.

```text
Snapshot
 ├── Object A → Version 3
 ├── Object B → Version 7
 └── Object C → Version 2
```

Der Snapshot besitzt eine eigene stabile Snapshot Identity.

Die enthaltenen Versionen werden durch spätere Änderungen nicht verändert.

---

## Konsistenz

Snapshots müssen einen definierten Konsistenzpunkt repräsentieren.

Wenn mehrere zusammengehörende Objekte Bestandteil eines Snapshots sind, darf der Snapshot keinen beliebigen Zwischenzustand einer laufenden Transaktion erfassen.

```text
Transaction Commit
        ↓
Consistent State
        ↓
Snapshot
```

Transactional Storage bildet dafür die grundlegende Konsistenzgrenze.

---

## Effiziente Speicherung

Snapshots sollen vorhandene Daten und Versionen wiederverwenden.

Mögliche Mechanismen sind insbesondere:

- Content-Addressed Storage,
- Copy-on-Write,
- immutable Versionen,
- gemeinsame Content References.

```text
Snapshot S1 ─┐
             ├── Shared Content
Snapshot S2 ─┘
```

Die ADR schreibt keinen bestimmten physischen Snapshot-Algorithmus vor.

---

## Rollback

Rollback stellt einen früheren Snapshot-Zustand wieder als aktuellen logischen Zustand bereit.

Dabei soll die bestehende Historie grundsätzlich erhalten bleiben.

```text
S1 → S2 → S3

Rollback to S1

S1 → S2 → S3 → S4
               ↑
          based on S1
```

Rollback ist damit normalerweise eine neue Zustandsänderung und kein rückwirkendes Löschen der Geschichte.

---

## Snapshot-Scopes

Snapshots können unterschiedliche Storage-Scopes besitzen.

Beispiele:

```text
Object Set
Application Data
User Data
Named Volume
System State
```

Welche Scopes unterstützt werden und welche Konsistenzgarantien sie besitzen, wird über die jeweiligen Storage Contracts definiert.

---

## Systemupdates und Recovery

Systemkritische Änderungen sollen Snapshots als Recovery-Grundlage verwenden können.

Beispiel:

```text
Current System
      ↓
Pre-Update Snapshot
      ↓
System Update
      ↓
Health Verification
   ├── Success → Continue
   └── Failure → Rollback
```

Damit integriert sich die Storage-Architektur in die A/B-, Recovery- und Self-Healing-Mechanismen von NovaOS.

---

## Retention und Garbage Collection

Snapshots halten referenzierte Versionen und Inhalte erreichbar.

Sie können deshalb verhindern, dass ältere Daten durch Garbage Collection entfernt werden.

Retention Policies bestimmen:

- welche Snapshots geschützt sind,
- wie lange sie erhalten bleiben,
- welche automatisch entfernt werden dürfen.

Systemkritische Recovery-Snapshots dürfen besonderen Schutz besitzen.

---

## Security und Data Sovereignty

Snapshots übernehmen die relevanten Security- und Data-Sovereignty-Eigenschaften der enthaltenen Daten.

Ein Snapshot darf keine Möglichkeit darstellen, frühere geschützte Daten ohne entsprechende Authority zugänglich zu machen.

Auch Rollback muss aktuelle Hard Constraints berücksichtigen.

Ein historischer Zustand darf nicht wiederhergestellt werden, wenn dadurch zwingende aktuelle Sicherheitsanforderungen verletzt würden.

---

## Resource Economy

Snapshots verbrauchen Ressourcen durch:

- zusätzliche Versionen,
- Metadaten,
- gehaltene CAS-Inhalte,
- mögliche Copy-on-Write-Divergenzen.

Diese Ressourcen müssen der Resource Economy zugerechnet werden.

Retention Policies dürfen Storage Pressure und Budgets berücksichtigen, dürfen jedoch geschützte Recovery-Snapshots nicht unkontrolliert entfernen.

---

## Architecture Introspection

NovaOS muss Snapshots kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- Snapshot Identity,
- Scope,
- Erstellungszeitpunkt,
- referenzierte Versionen,
- Retention-Status,
- Schutzstatus,
- benötigte Storage-Ressourcen,
- Rollback-Fähigkeit.

---

## Normative Anforderungen

1. NovaOS MUSS native Storage Snapshots unterstützen.
2. Ein Snapshot MUSS einen definierten konsistenten Storage-Zustand referenzieren.
3. Snapshots MÜSSEN stabile Snapshot Identities besitzen.
4. Snapshot-Inhalte DÜRFEN durch spätere Änderungen nicht rückwirkend verändert werden.
5. Snapshots SOLLEN vorhandene immutable Daten und Versionen wiederverwenden.
6. Snapshots DÜRFEN NICHT grundsätzlich vollständige physische Kopien erfordern.
7. Snapshot-Konsistenz MUSS mit Transactional Storage koordiniert werden.
8. Rollback MUSS einen früheren gültigen Zustand wieder als aktuellen Zustand veröffentlichen können.
9. Rollback SOLL die bestehende Versionshistorie erhalten.
10. Snapshots MÜSSEN unterschiedliche definierte Storage-Scopes unterstützen können.
11. Systemupdates und Recovery MÜSSEN Snapshots als Wiederherstellungspunkt verwenden können.
12. Snapshot-Retention MUSS policygesteuert sein.
13. Referenzierte Versionen und Inhalte DÜRFEN während der gültigen Snapshot-Lebensdauer nicht unkontrolliert reclaimed werden.
14. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN auch für Snapshot und Rollback gelten.
15. Snapshot-Ressourcen MÜSSEN in die Resource Economy integriert und kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- schnelle Wiederherstellung konsistenter Storage-Zustände,
- sichere Grundlage für Systemupdates,
- effiziente Snapshots durch CAS und Versionierung,
- Unterstützung von Recovery und Self-Healing,
- nachvollziehbare Rollbacks ohne Zerstörung der Historie.

### Negative Konsequenzen

- Snapshots können alte Daten langfristig gebunden halten,
- Retention und Garbage Collection müssen Snapshot-Referenzen berücksichtigen,
- große Snapshot-Scopes erhöhen Anforderungen an Konsistenz und Ressourcenverwaltung.

---

## Verworfene Alternativen

### Snapshot als vollständige Datenkopie

Verworfen.

Dies würde unnötigen Storage- und I/O-Aufwand verursachen.

### Rollback durch Überschreiben der Historie

Verworfen.

Frühere Zustände und die Rollback-Operation selbst sollen nachvollziehbar bleiben.

### Snapshots ausschließlich auf Dateisystemebene

Verworfen.

Snapshots müssen mit dem nativen Object-, Versioning- und Transaction-Modell von NovaOS zusammenarbeiten.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0012_Named_Volumes_statt_Laufwerksbuchstaben`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-BOOT-0009_A_B_Boot_und_Rollback`
- `ADR-BOOT-0010_Boot_Health_Milestones`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-STORAGE-SNAPSHOT-SCOPE-0001`
- `NPSPEC-STORAGE-SNAPSHOT-CONSISTENCY-0001`
- `NPSPEC-STORAGE-ROLLBACK-0001`
- `NPSPEC-STORAGE-SNAPSHOT-RETENTION-0001`
- `NPSPEC-STORAGE-SNAPSHOT-RECOVERY-0001`
- `NPSPEC-STORAGE-SNAPSHOT-TEST-0001`

---

## Ergebnis

Snapshots werden zu einer nativen Sicht auf konsistente historische Storage-Zustände:

```text
Versioned Storage
       +
Transactional Storage
       +
Content Addressing
       ↓
    Snapshot
       ↓
Recovery / Rollback
```

Die zentrale Architekturregel lautet:

```text
Preserve consistent states.

Rollback by restoring state,
not by rewriting history.
```