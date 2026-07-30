# ADR-CAPABILITY-0008: Fähigkeiten werden deklarativ beschrieben

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0006, ADR-CAPABILITY-0007
- **Folgedokumente:** NPSPEC-CAPABILITY-DESCRIPTOR-0001, NPSPEC-CAPABILITY-MANIFEST-0001, NPSPEC-CAPABILITY-REGISTRY-0001

---

# 1. Kontext

NovaOS basiert auf einem vollständig fähigkeitsorientierten Architekturmodell.

Damit Fähigkeiten automatisch gefunden, validiert, kombiniert, verteilt und optimiert werden können, muss das Betriebssystem ihre Eigenschaften kennen, ohne die eigentliche Implementierung auszuführen.

In klassischen Softwaresystemen werden viele Informationen erst zur Laufzeit ermittelt.

Beispiele:

- Reflection
- Laufzeitinspektion
- Quellcodeanalyse
- DLL-Scanning
- Annotationen
- manuelle Registrierung

Diese Verfahren erhöhen die Komplexität, verlängern Startzeiten und erschweren automatische Optimierungen.

NovaOS verfolgt deshalb einen deklarativen Ansatz.

Jede Fähigkeit beschreibt sich selbst vollständig durch einen maschinenlesbaren Descriptor.

---

# 2. Problemstellung

Werden Fähigkeiten nicht deklarativ beschrieben, entstehen zahlreiche Probleme.

## 2.1 Späte Analyse

Das Betriebssystem muss Implementierungen laden, um Eigenschaften zu erkennen.

Dadurch steigen:

- Speicherbedarf
- Startzeit
- Energieverbrauch

---

## 2.2 Fehlende Planbarkeit

Der Pipeline Planner kennt vor der Ausführung nicht:

- Ports
- Datentypen
- Ressourcenbedarf
- Fähigkeiten
- Sicherheitsanforderungen

Dadurch werden automatische Optimierungen erschwert.

---

## 2.3 Sprachabhängigkeit

Metadaten befinden sich häufig direkt im Quellcode.

Dies erschwert eine sprachübergreifende Plattform.

---

## 2.4 Schlechte Wartbarkeit

Änderungen am Quellcode verändern häufig unbeabsichtigt die öffentliche Beschreibung einer Fähigkeit.

---

## 2.5 Fehlende Werkzeuge

Editoren, KI-Assistenten und grafische Pipeline-Builder können Fähigkeiten nicht analysieren, wenn keine deklarativen Informationen vorhanden sind.

---

# 3. Entscheidung

Jede Fähigkeit besitzt einen **deklarativen Capability Descriptor**.

Dieser beschreibt sämtliche öffentlichen Eigenschaften der Fähigkeit.

Der Descriptor ist unabhängig von:

- Programmiersprache
- Compiler
- Betriebssystem
- Laufzeit
- Implementierung

Die Runtime liest zunächst ausschließlich den Descriptor.

Die eigentliche Implementierung wird erst geladen, wenn eine Ausführung erforderlich ist.

---

# 4. Definition eines Capability Descriptors

Ein Capability Descriptor ist eine strukturierte Beschreibung einer Fähigkeit.

Er enthält sämtliche Informationen, die für Planung, Validierung und Ausführung erforderlich sind.

Der Descriptor beschreibt ausschließlich die öffentliche Schnittstelle.

Interne Implementierungsdetails gehören nicht zum Descriptor.

---

# 5. Grundprinzipien

## 5.1 Deklarativ statt imperativ

Ein Descriptor beschreibt Eigenschaften.

Er enthält keine Programmlogik.

Beispiel:

```text
Capability

Bild skalieren

Input:
Nova.Image.Raster

Output:
Nova.Image.Raster
```

Nicht zulässig:

```text
if(...)
for(...)
while(...)
```

Ein Descriptor ist keine Programmiersprache.

---

## 5.2 Vollständige Beschreibung

Ein Descriptor muss alle öffentlichen Eigenschaften enthalten.

Dazu gehören mindestens:

- Capability-ID
- Name
- Version
- Beschreibung
- Ports
- Datentypen
- Berechtigungen
- Eigenschaften
- Fehlerdefinitionen
- unterstützte Plattformen

---

## 5.3 Implementierungsunabhängigkeit

Der Descriptor beschreibt ausschließlich den Vertrag.

Nicht Bestandteil sind:

- Klassen
- Funktionen
- Dateipfade
- Speicheradressen
- Bibliotheken
- Algorithmen
- Compileroptionen

---

## 5.4 Maschinenlesbarkeit

Descriptoren müssen vollständig maschinenlesbar sein.

Sie dürfen ohne Programmausführung analysiert werden.

Dadurch können Werkzeuge automatisch:

- Fähigkeiten finden
- Pipelines erzeugen
- Dokumentationen erstellen
- Kompatibilität prüfen
- Optimierungen berechnen

---

## 5.5 Sprachneutralität

Descriptoren besitzen keine Abhängigkeit zu einer Programmiersprache.

Eine Fähigkeit kann beispielsweise implementiert werden in:

- NovaLang
- Rust
- C
- C++
- Zig
- Python
- Java
- C#

Der Descriptor bleibt identisch.

---

## 5.6 Versionierung

Jeder Descriptor besitzt eine eigene Version.

Änderungen werden entsprechend der Kompatibilitätsregeln versioniert.

Breaking Changes erfordern eine neue Hauptversion.

---

## 5.7 Validierbarkeit

Descriptoren müssen automatisch validierbar sein.

Geprüft werden beispielsweise:

- Pflichtfelder
- Datentypen
- Portdefinitionen
- Versionsangaben
- Referenzen
- Namenskonventionen

Ungültige Descriptoren dürfen nicht registriert werden.

---

## 5.8 Erweiterbarkeit

Neue Eigenschaften dürfen ergänzt werden.

Vorhandene Descriptoren müssen dabei weiterhin lesbar bleiben.

Unbekannte Felder werden ignoriert, sofern sie nicht verpflichtend sind.

---

## 5.9 Trennung von Beschreibung und Implementierung

Der Descriptor darf unabhängig von der eigentlichen Implementierung verteilt werden.

Beispielsweise:

```text
Capability Descriptor

↓

Capability Registry

↓

Pipeline Planner

↓

Runtime

↓

Implementierung laden
```

Die Implementierung wird erst im letzten Schritt benötigt.

---

## 5.10 Descriptoren sind Bestandteil jeder Fähigkeit

Jede registrierte Fähigkeit besitzt genau einen öffentlichen Descriptor.

Ohne Descriptor existiert aus Sicht der Plattform keine Capability.

---

# 6. Mindestinhalt eines Descriptors

Ein Descriptor sollte mindestens folgende Informationen enthalten:

```text
Capability-ID

Name

Version

Beschreibung

Autor

Lizenz

Ports

Datentypen

Eigenschaften

Berechtigungen

Unterstützte Plattformen

Abhängigkeiten

Fehlerdefinitionen

Kompatibilität

Metadaten
```

Weitere Felder können ergänzt werden.

---

# 7. Beispiel

Ein vereinfachter Descriptor:

```yaml
id: nova.image.resize

name: Resize Image

version: 1.0.0

description: Skaliert Rasterbilder.

inputs:
  image:
    type: Nova.Image.Raster

outputs:
  image:
    type: Nova.Image.Raster

properties:
  deterministic: true
  gpuSupported: true
  streaming: false

permissions:
  filesystem: false
  network: false
```

Die eigentliche Implementierung wird hier nicht beschrieben.

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- automatische Registrierung,
- schnelle Analyse,
- statische Validierung,
- sprachunabhängige Entwicklung,
- bessere Optimierung,
- automatische Dokumentation,
- grafische Pipeline-Editoren,
- KI-gestützte Planung,
- einfache Verteilung,
- langfristige Wartbarkeit.

---

# 9. Nachteile

Dieses Modell erfordert:

- zusätzliche Descriptor-Dateien,
- konsequente Pflege der Metadaten,
- Validierungswerkzeuge,
- Versionierung der Descriptoren.

Diese Nachteile werden bewusst akzeptiert, da sie eine wesentliche Voraussetzung für eine automatisierbare Plattform darstellen.

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Registry
- Pipeline Planner
- Pipeline Executor
- Capability Runtime
- Capability Store
- Solution Framework
- Typregister
- SDK
- Compiler
- Visual Pipeline Editor
- Nova Assistant
- Dokumentationsgenerator

Alle diese Komponenten arbeiten primär mit Capability Descriptoren.

---

# 11. Implementierungshinweise

Descriptoren sollten in einem standardisierten Format gespeichert werden.

Geeignete Formate sind beispielsweise:

- YAML
- JSON
- Binäres Descriptorformat

Die Runtime darf Descriptoren laden und validieren, ohne die Implementierung zu initialisieren.

Compiler und SDKs sollten Descriptoren möglichst automatisch erzeugen, um Inkonsistenzen zwischen Beschreibung und Implementierung zu vermeiden.

---

# 12. Konformitätskriterien

Eine Fähigkeit gilt als konform, wenn:

- ein vollständiger Descriptor vorhanden ist,
- alle Pflichtfelder definiert sind,
- Ports vollständig beschrieben werden,
- Datentypen gültig sind,
- Versionierung eingehalten wird,
- keine Implementierungsdetails offengelegt werden,
- der Descriptor maschinenlesbar und validierbar ist,
- die Implementierung den beschriebenen Vertrag vollständig erfüllt.

---

# 13. Zusammenfassung

NovaOS beschreibt jede Fähigkeit durch einen deklarativen Capability Descriptor.

Der Descriptor bildet die öffentliche Beschreibung einer Fähigkeit und ist vollständig unabhängig von ihrer Implementierung.

Dadurch können Registrierung, Validierung, Planung, Optimierung und Dokumentation erfolgen, ohne Programmlogik auszuführen.

Dieses Architekturprinzip schafft die Grundlage für eine sprachunabhängige, automatisierbare und langfristig stabile Capability-Plattform.