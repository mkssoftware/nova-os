# NPSPEC-SCENE-STATE-0001 – Nova Persistent Scene State

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Nova Persistent Scene State definiert die dauerhafte Verwaltung grafischer Szenenzustände während ihrer Lebensdauer.

Grafische Zustände bleiben zwischen Rendering-Zyklen erhalten und werden ausschließlich bei relevanten Änderungen aktualisiert.

Ziel ist die Wiederverwendung unveränderter Szeneninformationen, die Vermeidung unnötiger Berechnungen und die Bereitstellung konsistenter Zustände für Rendering und Komposition.

## 2. Architekturprinzipien

- **Persistent:** Szenenzustände bleiben unabhängig von einzelnen Rendering-Zyklen erhalten.
- **State-Driven:** Die Darstellung basiert auf dem veröffentlichten Szenenzustand.
- **Versioned:** Veröffentlichte Zustände besitzen eindeutige Versionen.
- **Transactional:** Zustandsänderungen werden atomar übernommen.
- **Immutable Snapshots:** Veröffentlichte Zustandsversionen bleiben während ihrer Verwendung unveränderlich.
- **Incremental:** Unveränderte Zustandsbereiche werden wiederverwendet.
- **Deterministic:** Identische Ausgangszustände und Änderungen erzeugen identische logische Ergebnisse.
- **Renderer-Independent:** Die Zustandsverwaltung ist unabhängig von konkreten Rendering-Backends.

## 3. Szenenzustandsmodell

Jede Szene besitzt einen persistent verwalteten Zustand.

Dieser umfasst insbesondere:

- Eindeutige Szenenidentität
- Aktuelle Zustandsversion
- Wurzelelement und Szenenhierarchie
- Grafische Eigenschaften der Scene Nodes
- Geometrie und Transformationen
- Sichtbarkeit und Zeichenreihenfolge
- Material- und Effektzuordnungen
- Ressourcenreferenzen

Der Szenenzustand beschreibt die logische Darstellung und enthält keine hardwareabhängigen Rendering-Befehle.

## 4. Zustandsversionierung

Jede erfolgreich veröffentlichte Zustandsänderung MUSS einer eindeutig identifizierbaren Zustandsversion zugeordnet werden.

NovaOS MUSS:

- Die aktuelle veröffentlichte Version bereitstellen.
- Zustandsversionen eindeutig unterscheiden.
- Konsistente Lesezugriffe ermöglichen.
- Veraltete Zustandsreferenzen erkennen.
- Noch verwendete Zustandsversionen gültig halten.

Unveränderte Teilstrukturen DÜRFEN zwischen mehreren Zustandsversionen gemeinsam verwendet werden.

## 5. Zustandsänderungen

Änderungen erfolgen ausschließlich über definierte Zustandsoperationen und Nova Graphics State Transactions.

Die Verarbeitung umfasst:

1. Erfassen der gewünschten Änderungen.
2. Validieren des neuen Zustands.
3. Erzeugen einer konsistenten Zielversion.
4. Atomare Veröffentlichung der neuen Version.
5. Bereitstellen der Änderungen für Change Tracking und Dependency Tracking.
6. Freigeben nicht mehr benötigter Zustandsinformationen.

Nicht abgeschlossene Änderungen DÜRFEN den veröffentlichten Szenenzustand nicht verändern.

## 6. Immutable Scene Snapshots

Veröffentlichte Szenenzustände werden als unveränderliche Snapshots bereitgestellt.

Ein Snapshot repräsentiert eine konsistente Szenenversion.

Snapshots ermöglichen:

- Parallele Lesezugriffe
- Konsistente Rendering-Durchläufe
- Sichere Zustandsvergleiche
- Wiederverwendung unveränderter Teilstrukturen
- Entkopplung von Zustandsänderungen und Rendering

Ein Snapshot MUSS während seiner gesamten Verwendung gültig bleiben.

## 7. Inkrementelle Zustandsverwaltung

NovaOS SOLL unveränderte Zustandsbereiche zwischen Versionen wiederverwenden.

Dazu können verwendet werden:

- Structural Sharing
- Copy-on-Write
- Versionsbasierte Änderungsverfolgung
- Wiederverwendung unveränderlicher Scene Nodes
- Gezielte Aktualisierung betroffener Teilstrukturen

Die konkrete Implementierung bleibt austauschbar.

Eine vollständige Kopie des Szenenzustands SOLL vermieden werden, sofern eine sichere und effizientere Alternative verfügbar ist.

## 8. Zustandslebensdauer

Ein Szenenzustand bleibt während der Lebensdauer seiner Szene erhalten.

Ältere Zustandsversionen DÜRFEN freigegeben werden, sobald keine aktiven Referenzen mehr bestehen.

NovaOS MUSS sicherstellen, dass:

- Aktive Snapshots gültig bleiben.
- Ressourcenreferenzen nicht vorzeitig freigegeben werden.
- Entfernte Scene Nodes kontrolliert freigegeben werden.
- Nicht mehr benötigte Zustandsversionen Speicher freigeben können.
- Die Freigabe keine laufenden Rendering-Operationen beeinträchtigt.

Persistenz bezeichnet hierbei die Erhaltung im Arbeitsspeicher und erfordert keine dauerhafte Speicherung auf Datenträgern.

## 9. Rendering-Integration

Render Graph und Render Engine verwenden ausschließlich veröffentlichte, konsistente Szenenzustände.

Während eines Rendering-Durchlaufs DARF sich die verwendete Szenenversion nicht verändern.

Neue Zustandsversionen können parallel vorbereitet und veröffentlicht werden.

Die Rendering-Architektur entscheidet, ob eine neue Version unmittelbar oder bei einem späteren Frame verarbeitet wird.

Unveränderte Zustandsbereiche SOLLEN keine unnötigen Rendering-Operationen verursachen.

## 10. Fehlerbehandlung

Fehlgeschlagene Zustandsänderungen DÜRFEN den zuletzt gültigen veröffentlichten Zustand nicht beschädigen.

Bei Speicherengpässen oder ungültigen Zustandsoperationen MUSS NovaOS:

- Die Änderung kontrolliert ablehnen können.
- Den letzten gültigen Zustand erhalten.
- Nicht benötigte temporäre Ressourcen freigeben.
- Den Fehler nachvollziehbar melden.

Die Zustandsverwaltung DARF keine unvollständigen Snapshots veröffentlichen.

## 11. Verbindliche Anforderungen

1. Szenenzustände MÜSSEN zwischen Rendering-Zyklen erhalten bleiben.
2. Veröffentlichte Zustände MÜSSEN eindeutig versioniert sein.
3. Zustandsänderungen MÜSSEN atomar veröffentlicht werden.
4. Veröffentlichte Snapshots MÜSSEN während ihrer Verwendung unveränderlich bleiben.
5. Parallele Lesezugriffe MÜSSEN konsistent möglich sein.
6. Unveränderte Zustandsbereiche SOLLEN zwischen Versionen wiederverwendet werden.
7. Aktive Zustandsversionen und Ressourcenreferenzen DÜRFEN nicht vorzeitig freigegeben werden.
8. Fehlgeschlagene Änderungen DÜRFEN den veröffentlichten Zustand nicht verändern.
9. Rendering MUSS auf konsistenten Szenenversionen basieren.
10. Die Zustandsverwaltung MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 12. Ergebnis

Nova Persistent Scene State ermöglicht die persistente, versionsbasierte und transaktionssichere Verwaltung grafischer Szenenzustände.

Durch unveränderliche Snapshots, inkrementelle Aktualisierungen und die Wiederverwendung unveränderter Teilstrukturen reduziert NovaOS unnötige Berechnungen und Speicheroperationen.

Damit entsteht eine konsistente Grundlage für parallele Grafikverarbeitung, State-Driven Graphics und effizientes inkrementelles Rendering.