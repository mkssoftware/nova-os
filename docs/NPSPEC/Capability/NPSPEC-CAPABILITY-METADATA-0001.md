# NPSPEC-CAPABILITY-METADATA-0001: Metadaten, Kategorien und semantische Beschreibung

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0008, ADR-CAPABILITY-0013, ADR-CAPABILITY-0015, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001, NPSPEC-CAPABILITY-DISCOVERY-0001, NPSPEC-CAPABILITY-REGISTRY-0001

---

# 1. Zweck

Diese Spezifikation definiert das Metadatenmodell aller Nova Capabilities.

Metadaten beschreiben eine Capability fachlich und semantisch.

Sie ermöglichen:

- Discovery
- semantische Suche
- Intent Planning
- automatische Pipeline-Erstellung
- Dokumentation
- Klassifizierung
- Ranking
- Qualitätsbewertung

Metadaten beeinflussen niemals direkt die Ausführung einer Capability.

---

# 2. Ziele

Das Metadatenmodell verfolgt folgende Ziele:

- vollständige semantische Beschreibung
- menschenlesbare Dokumentation
- maschinenlesbare Struktur
- internationale Nutzbarkeit
- automatische Auffindbarkeit
- Wiederverwendbarkeit
- Erweiterbarkeit
- langfristige Stabilität

Strukturierte Metadaten verbessern Auffindbarkeit, Interoperabilität und automatische Verarbeitung komplexer Softwaresysteme. :contentReference[oaicite:0]{index=0}

---

# 3. Grundprinzipien

Metadaten sind:

- deklarativ
- versionsfähig
- sprachneutral
- erweiterbar
- validierbar
- unabhängig von Implementierungen

Metadaten beschreiben ausschließlich:

- Bedeutung
- Eigenschaften
- Einsatzgebiete
- Beziehungen

Nicht beschrieben werden:

- Quellcode
- Speicherlayout
- Algorithmen
- interne Optimierungen

---

# 4. Architekturübersicht

```text
Capability

│

├── Identity

├── Titel

├── Kurzbeschreibung

├── Beschreibung

├── Kategorie

├── Unterkategorie

├── Tags

├── Schlüsselwörter

├── Synonyme

├── Fachgebiete

├── Datentypen

├── Beispiele

├── Dokumentation

├── Qualitätsinformationen

├── Lokalisierungen

└── Erweiterungen
```

---

# 5. Titel

Jede Capability besitzt einen eindeutigen Anzeigenamen.

Beispiele:

```text
Resize Image

Translate Document

OCR

Speech Recognition
```

Der Titel dient ausschließlich der Benutzeroberfläche.

---

# 6. Kurzbeschreibung

Eine kurze Beschreibung fasst den Zweck zusammen.

Beispiel:

```text
Skaliert Rasterbilder auf eine gewünschte Größe.
```

Empfohlene Länge:

```text
50–200 Zeichen
```

---

# 7. Beschreibung

Eine ausführliche Beschreibung erläutert:

- Zweck
- Verhalten
- typische Einsatzgebiete
- Besonderheiten
- Einschränkungen

Die Beschreibung dient sowohl Entwicklern als auch KI-Systemen.

---

# 8. Kategorien

Capabilities werden in Hauptkategorien eingeordnet.

Beispiele:

```text
Image

Audio

Video

Document

AI

Office

Network

Robotics

Physics

Mathematics

Chemistry

Security

Automation

Visualization
```

Jede Capability besitzt mindestens eine Hauptkategorie.

---

# 9. Unterkategorien

Unterkategorien ermöglichen eine feinere Klassifizierung.

Beispiel:

```text
Image

↓

Processing

↓

Resize
```

Oder:

```text
AI

↓

Natural Language

↓

Translation
```

---

# 10. Tags

Tags beschreiben Eigenschaften einer Capability.

Beispiele:

```text
ocr

vision

translate

resize

speech

camera

gpu

offline

realtime
```

Tags dienen ausschließlich der Suche und dem Ranking.

---

# 11. Schlüsselwörter

Zusätzliche Schlüsselwörter verbessern die Auffindbarkeit.

Beispiele:

```text
Bild

Foto

Grafik

Skalieren

Verkleinern

Vergrößern
```

Mehrsprachige Schlüsselwörter sind zulässig.

---

# 12. Synonyme

Synonyme beschreiben alternative Bezeichnungen.

Beispiel:

```text
Resize

Scale

Rescale

Image Scaling
```

Die Discovery Engine verwendet Synonyme für semantische Suchanfragen.

---

# 13. Fachgebiete

Capabilities können einem oder mehreren Fachgebieten zugeordnet werden.

Beispiele:

```text
Maschinenbau

Elektrotechnik

Robotik

Medizin

Physik

Chemie

Astronomie

Geoinformatik

Bauwesen
```

Dadurch können spezialisierte Lösungen automatisch gefunden werden.

---

# 14. Datentypen

Metadaten beschreiben unterstützte Datentypen.

Beispiel:

```text
Input

RasterImage

↓

Output

RasterImage
```

Weitere Beispiele:

```text
Audio

Video

Tensor

Matrix

Document

Mesh

Measurement
```

Diese Informationen unterstützen Discovery und Pipeline-Planung.

---

# 15. Semantische Beschreibung

Jede Capability besitzt eine formale semantische Beschreibung.

Sie beantwortet:

- Was macht die Capability?
- Welche fachliche Bedeutung besitzt sie?
- Welche Ergebnisse liefert sie?
- Welche Domäne wird unterstützt?

Beispiel:

```text
Extrahiert Text aus Rasterbildern mittels optischer Zeichenerkennung.
```

Semantische Beschreibungen bilden die Grundlage für Intent-basierte Planung und automatisierte Auswahl geeigneter Capabilities. :contentReference[oaicite:1]{index=1}

---

# 16. Beispiele

Optional können Anwendungsbeispiele hinterlegt werden.

Beispiel:

```text
Foto

↓

OCR

↓

Text

↓

Übersetzung

↓

PDF
```

Beispiele unterstützen Dokumentation und Entwicklungswerkzeuge.

---

# 17. Dokumentation

Metadaten können Dokumentationsverweise enthalten.

Beispiele:

```text
Tutorial

API

Beispiele

FAQ

Referenz

Migration Guide
```

Die Runtime nutzt diese Informationen nicht.

---

# 18. Qualitätsinformationen

Optionale Qualitätsmerkmale:

```text
Reifegrad

Stabilität

Performanceklasse

Genauigkeit

Determinismus

Parallelisierbarkeit
```

Diese Informationen können im Ranking berücksichtigt werden.

---

# 19. Lokalisierung

Alle Benutzertexte dürfen lokalisiert werden.

Beispiel:

```text
Deutsch

English

Français

Español

日本語
```

Technische Identitäten bleiben unverändert.

---

# 20. Erweiterbare Metadaten

Eigene Metadatenfelder sind zulässig.

Beispiel:

```yaml
extensions:

  company:

    supportLevel: premium

    department: research
```

Erweiterungen dürfen Standardfelder nicht überschreiben.

---

# 21. Semantische Beziehungen

Capabilities können Beziehungen zu anderen Capabilities beschreiben.

Beispiele:

```text
verwendet

erweitert

ersetzt

ergänzt

empfohlen mit
```

Diese Beziehungen unterstützen automatische Pipeline-Vorschläge.

---

# 22. Discovery-Unterstützung

Die Discovery Engine nutzt:

- Titel
- Beschreibung
- Kategorien
- Tags
- Schlüsselwörter
- Synonyme
- Fachgebiete
- Datentypen

für:

- semantische Suche
- Intent Resolution
- Ranking
- automatische Vorschläge

---

# 23. YAML-Beispiel

```yaml
metadata:

  title: Resize Image

  shortDescription: Skaliert Rasterbilder.

  description: Ändert die Größe eines Rasterbildes unter Beibehaltung des Seitenverhältnisses.

  category: Image

  subcategory: Processing

  tags:

    - resize

    - image

    - scaling

    - graphics

  keywords:

    - Bild

    - Grafik

    - Skalieren

    - Verkleinern

  synonyms:

    - Scale Image

    - Image Resize

  domains:

    - Graphics

    - Photography

  examples:

    - photo -> resize -> png
```

---

# 24. Validierungsregeln

Metadaten gelten als gültig, wenn:

- ein Titel vorhanden ist,
- mindestens eine Kategorie definiert ist,
- die Beschreibung UTF-8-konform ist,
- Tags eindeutig sind,
- Schlüsselwörter keine Duplikate enthalten,
- Synonyme die technische Identität nicht verändern,
- Erweiterungen in eigenen Namespaces liegen.

---

# 25. Qualitätsanforderungen

Das Metadatenmodell muss:

- vollständig deklarativ sein,
- semantische Suche unterstützen,
- mehrsprachig sein,
- maschinenlesbar sein,
- menschenlesbar sein,
- validierbar sein,
- erweiterbar sein,
- langfristig kompatibel bleiben.

Ein standardisiertes Metadatenmodell erhöht Auffindbarkeit, Wiederverwendbarkeit und Interoperabilität über Werkzeuge und Plattformen hinweg. :contentReference[oaicite:2]{index=2}

---

# 26. Konformitätskriterien

Eine Capability erfüllt diese Spezifikation, wenn:

- vollständige Metadaten vorhanden sind,
- mindestens eine Hauptkategorie definiert ist,
- eine semantische Beschreibung existiert,
- Tags und Schlüsselwörter gepflegt sind,
- Datentypen dokumentiert sind,
- Discovery alle Metadaten auswerten kann,
- Erweiterungen standardkonform implementiert sind.

---

# 27. Zusammenfassung

Das Metadatenmodell definiert die fachliche und semantische Beschreibung jeder Nova Capability.

Durch strukturierte Kategorien, Tags, Schlüsselwörter, Synonyme, Fachgebiete und formale Beschreibungen können Capabilities unabhängig von ihrer technischen Implementierung effizient gefunden, klassifiziert und automatisch in Intent-basierte Lösungen sowie Capability-Pipelines integriert werden.

Das Modell bildet damit die Grundlage für semantische Discovery, intelligente Planung und eine langfristig skalierbare Capability-Plattform.