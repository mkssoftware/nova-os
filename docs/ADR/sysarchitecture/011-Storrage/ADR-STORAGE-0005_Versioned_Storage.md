# ADR-STORAGE-0005 – Versioned Storage

## Status

Angenommen

## Kategorie

Kernel / Storage / Versioning / Object Storage / Recovery

## Kontext

NovaOS trennt mit dem nativen Object-Storage-Modell die logische Identität eines Objekts von seinem konkreten Inhalt. Content-Addressed Storage ermöglicht zusätzlich unveränderliche Content-Versionen, während Transactional Storage konsistente Zustandsänderungen sicherstellt.

Darauf aufbauend soll Storage-Versionierung keine anwendungsspezifische Sonderlösung sein. Das System benötigt eine gemeinsame Grundlage für:

- frühere Objektzustände,
- Snapshots,
- Rollback,
- Recovery,
- Provenance,
- nachvollziehbare Änderungen.

Versionierung darf jedoch nicht bedeuten, dass jede Änderung unbegrenzt gespeichert werden muss.

---

## Entscheidung

NovaOS unterstützt **Versioned Storage als native Fähigkeit des Storage-Modells**.

Ein Storage Object behält seine stabile Object Identity, während seine Zustände als unterschiedliche Versionen repräsentiert werden können.

```text
Object Identity
      │
      ├── Version 1 → Content A
      ├── Version 2 → Content B
      └── Version 3 → Content C
```

Versionen sind logisch unveränderliche, identifizierbare Zustände eines Objekts.

Die aktuelle Version ist lediglich der gegenwärtig veröffentlichte Zustand des Objekts.

---

## Grundprinzip

```text
Stable Object Identity
        +
Immutable Versions
        =
Versioned Storage
```

---

## Version Identity

Jede persistierte Version muss innerhalb ihres Objektkontexts eindeutig identifizierbar sein.

Eine Version kann insbesondere auf folgende Bestandteile verweisen:

```text
Version
 ├── Object Identity
 ├── Content Identity
 ├── Metadata State
 ├── Relations
 └── Provenance
```

Die konkrete interne Repräsentation wird durch NPSPECs definiert.

---

## Verhältnis zu Content-Addressed Storage

Versioned Storage und CAS ergänzen sich.

CAS identifiziert konkrete immutable Inhalte. Versioned Storage beschreibt dagegen den vollständigen logischen Zustand eines Storage Objects zu einem bestimmten Versionspunkt.

Mehrere Versionen dürfen denselben CAS-Inhalt referenzieren, wenn sich beispielsweise nur Metadaten oder Beziehungen geändert haben.

---

## Verhältnis zu Transactional Storage

Neue Versionen werden über Transactional Storage veröffentlicht.

```text
Current Version
      ↓
Transaction
      ↓
New Version
      ↓
Atomic Publish
```

Eine fehlgeschlagene Transaktion darf keine teilweise veröffentlichte Version erzeugen.

Mehrere zusammengehörende Objekte können innerhalb einer übergeordneten Transaktion konsistent auf neue Versionen wechseln.

---

## Snapshots

Versioned Storage bildet die Grundlage für Storage Snapshots.

Ein Snapshot beschreibt einen konsistenten Bezug auf bestimmte Objektversionen beziehungsweise einen definierten Storage-Zustand.

Snapshots müssen nicht zwangsläufig vollständige physische Kopien erzeugen.

CAS, Copy-on-Write oder andere geeignete Storage-Mechanismen dürfen zur effizienten Umsetzung verwendet werden.

---

## Rollback

NovaOS darf einen früheren gültigen Versionszustand wieder als aktuellen Zustand veröffentlichen.

Rollback bedeutet dabei grundsätzlich nicht, historische Versionen rückwirkend zu verändern.

```text
V1 → V2 → V3

Rollback to V1

V1 → V2 → V3 → V4
               ↑
          State based on V1
```

Damit bleibt die Änderungshistorie nachvollziehbar.

---

## Retention und Garbage Collection

Nicht jede Version muss unbegrenzt erhalten bleiben.

Retention Policies bestimmen, welche Versionen aufbewahrt werden.

Dabei können beispielsweise berücksichtigt werden:

- explizit geschützte Versionen,
- Snapshots,
- Recovery-Anforderungen,
- Provenance-Anforderungen,
- Storage Budgets.

Nicht mehr benötigte Versionen und CAS-Inhalte dürfen kontrolliert reclaimt werden.

---

## Security und Data Sovereignty

Versionen unterliegen denselben Security-, Trust- und Data-Sovereignty-Regeln wie aktuelle Objektdaten.

Historische Versionen dürfen insbesondere nicht zu einem Weg werden, gelöschte oder nicht mehr autorisierte Informationen ohne entsprechende Authority zugänglich zu machen.

Retention und Replikation müssen bestehende Sovereignty-Vorgaben berücksichtigen.

---

## Resource Economy

Versionierung verbraucht Storage-, Metadata- und gegebenenfalls Replikationsressourcen.

Diese Ressourcen müssen der systemweiten Resource Economy zugeordnet werden.

Retention Policies dürfen deshalb Storage Budgets berücksichtigen.

Systemkritische Recovery-Versionen können explizit reserviert oder geschützt werden.

---

## Architecture Introspection

NovaOS muss Versionierungsinformationen kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- Object Identity,
- Version Identity,
- aktuelle Version,
- vorherige Versionen,
- Content References,
- Provenance,
- Snapshot-Zugehörigkeit,
- Retention-Status.

---

## Normative Anforderungen

1. NovaOS MUSS native Storage-Versionierung unterstützen.
2. Object Identity MUSS über unterschiedliche Versionen stabil bleiben.
3. Persistierte Versionen MÜSSEN eindeutig identifizierbar sein.
4. Veröffentlichte Versionen MÜSSEN logisch unveränderlich behandelt werden.
5. Neue Versionen SOLLEN über Transactional Storage veröffentlicht werden.
6. Fehlgeschlagene Transaktionen DÜRFEN keine teilweise veröffentlichten Versionen hinterlassen.
7. Versionen DÜRFEN CAS-Inhalte referenzieren und gemeinsam verwenden.
8. Versioned Storage MUSS als Grundlage für konsistente Snapshots verwendbar sein.
9. Rollback DARF frühere Versionen NICHT rückwirkend verändern.
10. Retention Policies MÜSSEN die Lebensdauer historischer Versionen kontrollieren können.
11. Nicht mehr benötigte Versionen MÜSSEN kontrolliert reclaimbar sein.
12. Security- und Data-Sovereignty-Regeln MÜSSEN auch für historische Versionen gelten.
13. Versionierungsressourcen MÜSSEN in die Resource Economy integriert sein.
14. Versionen, Snapshots und Retention-Zustände MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- native Grundlage für Snapshots und Rollback,
- nachvollziehbare Objektentwicklung,
- bessere Recovery-Fähigkeiten,
- effiziente Kombination mit CAS,
- konsistente Versionserzeugung über Transactional Storage,
- gemeinsame Versionierungsarchitektur für System und Anwendungen.

### Negative Konsequenzen

- zusätzlicher Storage- und Metadata-Verbrauch,
- Retention und Garbage Collection werden notwendig,
- historische Daten erhöhen Anforderungen an Security und Data Sovereignty.

---

## Verworfene Alternativen

### Versionierung ausschließlich durch Anwendungen

Verworfen.

Systemweite Funktionen wie Recovery, Snapshots und Provenance benötigen eine gemeinsame Storage-Grundlage.

### Jede Version als vollständige Kopie speichern

Verworfen.

CAS, Copy-on-Write und andere Mechanismen ermöglichen effizientere Implementierungen.

### Unbegrenzte Speicherung aller Versionen

Verworfen.

Versionierung muss durch Retention Policies und Resource Budgets kontrollierbar bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-VERSION-0001`
- `NPSPEC-STORAGE-VERSION-ID-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-STORAGE-ROLLBACK-0001`
- `NPSPEC-STORAGE-RETENTION-0001`
- `NPSPEC-STORAGE-VERSION-GC-0001`
- `NPSPEC-STORAGE-VERSION-TEST-0001`

---

## Ergebnis

NovaOS verbindet stabile Objektidentitäten mit unveränderlichen historischen Zuständen:

```text
Object Identity
      ↓
Version History
      ↓
V1 → V2 → V3 → ...
```

Content-Addressed Storage stellt dabei wiederverwendbare Inhalte bereit, Transactional Storage veröffentlicht konsistente neue Zustände und Retention Policies kontrollieren deren Lebensdauer.

Die zentrale Architekturregel lautet:

```text
Objects persist.

Their states evolve through versions.
```