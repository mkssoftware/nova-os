# ADR-6014: Objektbasiertes Dateisystem

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Filesystem Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** NovaFS
- **Ersetzt:** Keine

---

## 1. Kontext

NovaFS soll als modernes Dateisystem entwickelt werden, das weit über die Fähigkeiten klassischer Dateisysteme hinausgeht.

Neben normalen Dateien und Verzeichnissen soll NovaFS unter anderem unterstützen:

- Snapshots
- Subvolumes
- Systemgenerationen
- Recovery-Images
- Streams
- Geräteobjekte
- Sicherheitsrichtlinien
- Clusterobjekte
- zukünftige Erweiterungen

Klassische Dateisysteme unterscheiden intern häufig zwischen vielen verschiedenen Verwaltungsstrukturen für Dateien, Verzeichnisse, Geräte oder spezielle Objekte. Dadurch entstehen unterschiedliche Implementierungen mit teilweise redundanter Logik.

NovaFS soll stattdessen auf einem einheitlichen Objektmodell basieren.

---

## 2. Problemstellung

Ein Dateisystem besteht nicht nur aus Dateien.

Intern müssen zahlreiche unterschiedliche Objekte verwaltet werden:

- Dateien
- Verzeichnisse
- symbolische Links
- Geräte
- Named Streams
- Snapshots
- Recovery-Images
- Subvolumes
- Systemgenerationen
- zukünftige Erweiterungen

Werden diese durch voneinander getrennte Verwaltungsstrukturen implementiert, entstehen:

- redundanter Code
- unterschiedliche APIs
- unterschiedliche Metadatenformate
- höherer Wartungsaufwand
- schwierigere Erweiterbarkeit

NovaFS benötigt daher ein universelles Datenmodell.

---

## 3. Betrachtete Alternativen

### Alternative A – Klassisches Datei-/Verzeichnis-Modell

Nur Dateien und Verzeichnisse besitzen eigene Datenstrukturen.

Weitere Objekte werden als Sonderfälle behandelt.

**Vorteile**

- einfache Implementierung
- bekannte Architektur

**Nachteile**

- viele Sonderfälle
- schlechte Erweiterbarkeit
- inkonsistente Metadaten

---

### Alternative B – Unterschiedliche Strukturen je Objekttyp

Jeder Objekttyp besitzt seine eigene interne Struktur.

**Vorteile**

- optimale Spezialisierung

**Nachteile**

- hoher Entwicklungsaufwand
- komplexe APIs
- redundante Logik
- schwierige Wartung

---

### Alternative C – Universelles Objektmodell

Alle persistenten Ressourcen werden als Objekte behandelt.

Der Objekttyp bestimmt lediglich das Verhalten.

Alle Objekte besitzen dieselbe grundlegende Metadatenstruktur.

**Vorteile**

- einheitliche Architektur
- sehr gut erweiterbar
- einfache APIs
- konsistente Metadaten
- optimale Integration in B+-Trees

**Nachteile**

- etwas größere Grundstruktur
- einige Objekte verwenden nicht alle Felder

---

## 4. Entscheidung

NovaFS verwendet ein **universelles objektbasiertes Dateisystemmodell**.

Jede persistente Ressource innerhalb des Dateisystems wird als Objekt dargestellt.

Jedes Objekt besitzt:

- eine eindeutige Objekt-ID,
- einen Objekttyp,
- gemeinsame Metadaten,
- optionale Attribute,
- optionale Extents,
- optionale Kindobjekte.

Das Verhalten eines Objekts ergibt sich ausschließlich aus seinem Typ.

---

## 5. Begründung

Ein objektbasiertes Modell vereinfacht die gesamte Architektur von NovaFS erheblich.

### Einheitliche Verwaltung

Alle Ressourcen werden identisch behandelt.

Unabhängig davon, ob es sich um:

- Dateien,
- Verzeichnisse,
- Snapshots,
- Recovery-Images oder
- Systemgenerationen

handelt.

---

### Einheitliche APIs

Alle Kernel- und Dateisystemfunktionen arbeiten mit Objekt-Handles.

Beispielsweise:

```c
novafs_open_object();
novafs_close_object();
novafs_get_attributes();
novafs_set_attributes();
```

Spezialfälle werden dadurch minimiert.

---

### Erweiterbarkeit

Neue Objekttypen können jederzeit ergänzt werden.

Bestehende Dateisystemstrukturen bleiben unverändert.

---

### Optimale Integration in B+-Trees

Da jedes Objekt eine eindeutige Objekt-ID besitzt, können sämtliche Objekte effizient im Object Tree verwaltet werden.

Dies ermöglicht logarithmische Zugriffszeiten selbst bei Milliarden von Objekten. B+-Trees sind speziell für blockorientierte Speichersysteme optimiert und werden daher auch in modernen Dateisystemen zur Metadatenverwaltung eingesetzt. :contentReference[oaicite:0]{index=0}

---

### Snapshot-Unterstützung

Snapshots bestehen lediglich aus zusätzlichen Referenzen auf vorhandene Objekte.

Neue Objekte müssen nur dann erzeugt werden, wenn sich tatsächlich Daten ändern.

---

### Unterstützung von Systemgenerationen

Auch eine komplette Betriebssystemgeneration wird lediglich als Objekt dargestellt.

Dadurch können:

- Aktivierung,
- Rollback,
- Verifikation

einheitlich implementiert werden.

---

## 6. Objektmodell

Jedes Objekt besitzt mindestens folgende Eigenschaften:

- Objekt-ID
- Objekttyp
- Eigentümer
- Berechtigungen
- Zeitstempel
- Generation
- Prüfsumme
- Attribute
- Extent-Referenzen
- Schutzrichtlinie

---

### Standard-Objekttypen

NovaFS definiert mindestens folgende Typen:

```c
typedef enum
{
    NOVAFS_OBJECT_UNKNOWN = 0,

    NOVAFS_OBJECT_FILE,
    NOVAFS_OBJECT_DIRECTORY,
    NOVAFS_OBJECT_SYMLINK,
    NOVAFS_OBJECT_DEVICE,

    NOVAFS_OBJECT_STREAM,

    NOVAFS_OBJECT_SNAPSHOT,

    NOVAFS_OBJECT_SUBVOLUME,

    NOVAFS_OBJECT_SYSTEM_GENERATION,

    NOVAFS_OBJECT_RECOVERY_IMAGE,

    NOVAFS_OBJECT_POLICY,

    NOVAFS_OBJECT_CLUSTER_NODE,

    NOVAFS_OBJECT_STORAGE_POOL,

    NOVAFS_OBJECT_SECURITY_CONTEXT

} novafs_object_type_t;
```

Weitere Typen können zukünftig ergänzt werden.

---

## 7. Objektidentifikation

Jedes Objekt erhält eine eindeutige 64-Bit-Objekt-ID.

```c
typedef uint64_t novafs_object_id_t;
```

Objekt-IDs werden niemals wiederverwendet.

Dadurch bleiben:

- Snapshots
- Rollbacks
- Cluster-Replikation
- Recovery

eindeutig nachvollziehbar.

---

## 8. Positive Konsequenzen

Die Entscheidung für ein objektbasiertes Modell bietet zahlreiche Vorteile.

### Einheitliche Architektur

Alle Komponenten verwenden dieselben Grundstrukturen.

---

### Weniger Sonderfälle

Fast sämtliche Kernel- und Dateisystemoperationen können unabhängig vom Objekttyp implementiert werden.

---

### Einfache Erweiterbarkeit

Neue Funktionen benötigen häufig lediglich einen neuen Objekttyp.

Die Kernarchitektur bleibt unverändert.

---

### Optimale Snapshot-Unterstützung

Objekte können zwischen Generationen gemeinsam genutzt werden.

---

### Clusterfähigkeit

Objekte können unabhängig repliziert werden.

Der Cluster muss lediglich Objekt-IDs synchronisieren.

---

### Gute Wartbarkeit

Durch die Vereinheitlichung sinkt die Komplexität der Implementierung erheblich.

---

## 9. Negative Konsequenzen und Risiken

### Größere Objektstruktur

Einige Objekttypen verwenden nicht alle vorhandenen Felder.

Dadurch entsteht ein geringer zusätzlicher Speicherbedarf.

---

### Typprüfung erforderlich

Nahezu jede Operation muss zunächst den Objekttyp prüfen.

Dieser Aufwand ist jedoch sehr gering.

---

### Erweiterte Objektverwaltung

Das Dateisystem benötigt:

- Object Manager
- Object Cache
- Object Tree
- Objekt-ID-Verwaltung

Diese Komponenten sind jedoch ohnehin Bestandteil der NovaFS-Architektur.

---

## 10. Auswirkungen auf andere Module

Diese Entscheidung betrifft nahezu sämtliche Komponenten von NovaFS.

Direkt betroffen sind:

- Object Manager
- Object Tree
- Directory Manager
- Snapshot Manager
- Recovery Manager
- Generation Manager
- Security Manager
- Cluster Manager
- Network Layer
- Protection Policy Manager
- Extent Manager
- Transaction Manager

Auch zukünftige Erweiterungen SOLLEN auf dem Objektmodell basieren.

---

## 11. Umsetzungsregeln

Für NovaFS gelten folgende verbindliche Regeln:

- Jede persistente Ressource MUSS ein Objekt sein.
- Jedes Objekt MUSS eine eindeutige Objekt-ID besitzen.
- Jede Objekt-ID MUSS dauerhaft eindeutig sein.
- Jeder Objekttyp MUSS über denselben Object Tree verwaltet werden.
- Objektmetadaten MÜSSEN transaktional gespeichert werden.
- Objektänderungen MÜSSEN ausschließlich über Copy-on-Write erfolgen.
- Objekte DÜRFEN niemals direkt überschrieben werden.
- Jeder Objekttyp MUSS erweiterbar sein.
- Neue Objekttypen DÜRFEN bestehende Typen nicht verändern.

---

## 12. Referenzen

- NPSPEC-NOVAFS-0001
- ADR-6001 – Extentbasierte Speicherung
- ADR-6012 – Copy-on-Write für Metadaten
- ADR-6013 – B+-Trees als zentrale Datenstruktur
- zukünftige ADR – Object Manager
- zukünftige ADR – Snapshot-Architektur
- zukünftige ADR – Systemgenerationen

---

## 13. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Objektmodell als grundlegende Architektur von NovaFS angenommen.
- **Zukünftige Änderungen:** Nur durch ein ersetzendes ADR zulässig.