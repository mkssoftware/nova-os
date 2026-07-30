# ADR-DATAFLOW-0006: Datenobjekte behalten Herkunft und Verarbeitungshistorie

- **Status:** Akzeptiert
- **Datum:** 2026-07-27
- **Kategorie:** Data Flow
- **Betrifft:** Nova Runtime, Pipeline Engine, Capability Runtime, Object Store, Capability Registry, Observability
- **Entscheider:** Nova Architecture Board

---

# Kontext

In klassischen Softwaresystemen geht nach mehreren Verarbeitungsschritten häufig verloren,

- woher Daten stammen,
- welche Verarbeitungsschritte durchgeführt wurden,
- welche Versionen verwendet wurden,
- welche Algorithmen beteiligt waren,
- welche Parameter verwendet wurden.

Beispiel:

```text
Bild
↓
OCR
↓
Übersetzung
↓
Zusammenfassung
↓
PDF
```

Am Ende ist oftmals nicht mehr nachvollziehbar:

- welches Ursprungsbild verwendet wurde,
- welche OCR-Version genutzt wurde,
- welches Übersetzungsmodell eingesetzt wurde,
- welche Parameter aktiv waren,
- wann einzelne Verarbeitungsschritte stattfanden.

Dies erschwert:

- Fehlersuche
- Reproduzierbarkeit
- Qualitätssicherung
- wissenschaftliche Nachvollziehbarkeit
- Audits
- KI-Transparenz

Die systematische Erfassung von Herkunft (Provenance) verbessert Nachvollziehbarkeit, Reproduzierbarkeit und Vertrauenswürdigkeit komplexer Datenverarbeitungen. Das zugrunde liegende Konzept ist unter anderem im W3C PROV-Datenmodell beschrieben. ([w3.org](https://www.w3.org/TR/prov-overview/))

---

# Problem

Moderne Nova-Pipelines können aus hunderten Capabilities bestehen.

Beispiel:

```text
Scan
↓
Bildverbesserung
↓
OCR
↓
Spracherkennung
↓
Übersetzung
↓
Zusammenfassung
↓
PDF
```

Ohne Herkunftsinformationen lässt sich später nicht mehr eindeutig bestimmen:

- welche Datenbasis verwendet wurde,
- welche Capability welche Daten erzeugt hat,
- welche Version beteiligt war,
- welche Parameter das Ergebnis beeinflusst haben.

Dadurch gehen wichtige Informationen dauerhaft verloren.

---

# Entscheidung

Jedes Nova-Datenobjekt besitzt eine unveränderliche Provenance (Herkunfts- und Verarbeitungshistorie).

Diese Historie begleitet das Datenobjekt während seines gesamten Lebenszyklus.

Sie wird von der Runtime automatisch gepflegt.

Capabilities dürfen Provenance lesen, jedoch nicht manipulieren.

---

# Architektur

```text
Data Object
├── Object ID
├── Technical Type
├── Semantic Type
├── Metadata
├── Version
├── References
└── Provenance
```

Die Provenance ist ein fester Bestandteil jedes Datenobjekts.

---

# Provenance-Modell

Die Provenance beschreibt:

- Ursprung
- Verarbeitung
- Transformation
- Versionierung
- Beziehungen
- Zeitpunkte

Sie bildet einen gerichteten Herkunftsgraphen.

---

# Ursprung

Der Ursprung beschreibt,

wo ein Datenobjekt erstmals entstanden ist.

Beispiele:

```text
Datei
Scanner
Kamera
Sensor
Benutzereingabe
Netzwerk
API
Simulation
KI-Modell
```

Der Ursprung bleibt dauerhaft erhalten.

---

# Verarbeitungsschritte

Jede Capability ergänzt einen neuen Provenance-Eintrag.

Beispiel:

```text
Image
↓
Resize
↓
OCR
↓
Translation
↓
Summary
```

Jeder Schritt wird einzeln gespeichert.

---

# Provenance-Eintrag

Ein Verarbeitungsschritt enthält mindestens:

```text
Capability ID
Capability Version
Ausführungszeit
Input Objects
Output Object
Parameter
Status
Dauer
```

Alle Einträge sind unveränderlich.

---

# Objektbeziehungen

Ein Datenobjekt kennt seine Beziehungen.

Beispiele:

```text
Erzeugt aus
Abgeleitet von
Zusammengeführt mit
Aufgeteilt in
Transformiert zu
```

Dadurch entsteht ein vollständiger Herkunftsgraph.

---

# Versionierung

Die Provenance dokumentiert jede Version.

```text
Version 1
↓
Version 2
↓
Version 3
```

Alle Versionen bleiben logisch miteinander verknüpft.

---

# Pipeline-Historie

Komplexe Pipelines erzeugen automatisch einen vollständigen Graphen.

```text
Image
↓
OCR
↓
Markdown
↓
Translation
↓
HTML
↓
PDF
```

Der gesamte Ablauf bleibt nachvollziehbar.

---

# Parameter

Die Provenance speichert verwendete Parameter.

Beispiele:

```text
Resolution = 300 DPI
Language = de
Compression = Lossless
Temperature = 0.2
```

Nur dokumentierte Parameter beeinflussen die Reproduzierbarkeit.

---

# KI-Modelle

Bei KI-Capabilities werden zusätzlich gespeichert:

```text
Model ID
Model Version
Prompt
Inference Profile
Seed
Hardware
Provider
```

Dadurch bleiben KI-Ergebnisse reproduzierbar, soweit das jeweilige Modell deterministisch arbeitet.

---

# Referenzierte Objekte

Ein Provenance-Eintrag verweist auf alle verwendeten Eingabeobjekte.

Beispiel:

```text
Input
↓
Object A
Object B
Object C
↓
Output D
```

Dadurch entsteht ein vollständiger Datenflussgraph.

---

# Zeitmodell

Jeder Provenance-Eintrag enthält:

```text
Startzeit
Endzeit
Dauer
Zeitzone
```

Zeitinformationen werden standardisiert gespeichert.

---

# Integrität

Provenance-Einträge sind unveränderlich.

Nachträgliche Änderungen sind nicht zulässig.

Optional können Einträge kryptographisch signiert oder mit Prüfsummen versehen werden.

---

# Sicherheit

Nicht jede Capability darf vollständige Provenance lesen.

Zugriffsrechte können getrennt geregelt werden:

```text
Keine Einsicht
Zusammenfassung
Vollständige Historie
Administrativ
```

Die Runtime erzwingt diese Berechtigungen.

---

# Datenschutz

Personenbezogene Informationen dürfen nicht ungeprüft Bestandteil der Provenance werden.

Erforderlich sind:

- Datenminimierung
- Zugriffskontrolle
- definierte Aufbewahrungsfristen
- Anonymisierung oder Pseudonymisierung, sofern erforderlich

Die Provenance muss mit den Datenschutzrichtlinien der Plattform vereinbar sein.

---

# Observability

Die Provenance ergänzt die klassische Observability.

Während Observability beschreibt,

```text
Was gerade passiert
```

beschreibt Provenance,

```text
Wie dieses Objekt entstanden ist.
```

Beide Konzepte ergänzen sich.

---

# Discovery

Discovery kann Provenance nutzen.

Beispiele:

```text
Zeige alle Daten,
die durch Capability X erzeugt wurden.
```

oder

```text
Finde alle PDFs,
die aus Scan-Dokumenten entstanden sind.
```

Dies ermöglicht semantische Herkunftssuchen.

---

# Debugging

Fehler lassen sich auf einzelne Verarbeitungsschritte zurückführen.

Beispiel:

```text
PDF
↓
HTML
↓
Translation
↓
OCR
↓
Originalbild
```

Dadurch können Fehlerquellen gezielt identifiziert werden.

---

# Wissenschaftliche Nutzung

Für wissenschaftliche Anwendungen ermöglicht Provenance:

- vollständige Nachvollziehbarkeit
- Reproduzierbarkeit
- Zitierfähigkeit
- Dokumentation von Berechnungswegen
- Auditierbarkeit

Dies erhöht die Vertrauenswürdigkeit der Ergebnisse.

---

# Erweiterbarkeit

Das Provenance-Modell kann erweitert werden.

Beispiele:

```text
Laborinformationen
Robotik
Geodaten
Medizin
Industrie
Simulation
Digitale Zwillinge
```

Die Kernstruktur bleibt unverändert.

---

# Vorteile

- vollständige Nachvollziehbarkeit
- reproduzierbare Verarbeitung
- bessere Fehlersuche
- transparente KI-Ausführung
- wissenschaftliche Dokumentation
- Audits werden vereinfacht
- bessere Qualitätssicherung
- nachvollziehbare Datenherkunft
- langfristige Archivierung

---

# Nachteile

- zusätzlicher Speicherbedarf
- größere Metadaten
- komplexere Runtime
- höherer Verwaltungsaufwand
- Datenschutzanforderungen müssen berücksichtigt werden

Diese Nachteile werden als akzeptabel bewertet.

---

# Konsequenzen

Nova behandelt Provenance als integralen Bestandteil jedes Datenobjekts.

Jedes Objekt besitzt eine unveränderliche Herkunfts- und Verarbeitungshistorie, die automatisch durch die Runtime aufgebaut und gepflegt wird.

Alle Verarbeitungsschritte, Versionen, Eingabeobjekte, Parameter und beteiligten Capabilities bleiben dauerhaft nachvollziehbar.

Dadurch entstehen reproduzierbare, auditierbare und KI-transparente Datenflüsse, die sowohl klassische Anwendungen als auch wissenschaftliche Berechnungen, Industrieprozesse und komplexe KI-Pipelines zuverlässig dokumentieren.

Die Provenance bildet somit eine zentrale Grundlage für Vertrauen, Qualitätssicherung und langfristige Wartbarkeit innerhalb der Nova Dataflow-Architektur.