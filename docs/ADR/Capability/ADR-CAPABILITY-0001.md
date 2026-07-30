# ADR-CAPABILITY-0001: Fähigkeiten ersetzen klassische Anwendungen als primäres Softwaremodell

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** Keine
- **Folgedokumente:** NPSPEC-CAPABILITY-0001, ADR-DATAFLOW-0001

---

## 1. Kontext

Klassische Betriebssysteme basieren auf Anwendungen (Applications) als kleinster ausführbarer Einheit.

Eine Anwendung bündelt häufig eine Vielzahl unterschiedlicher Funktionen in einem einzigen Programm. Dadurch entstehen monolithische Anwendungen mit eigenen Benutzeroberflächen, eigenen Datenformaten, eigenen Einstellungen und oft redundanten Implementierungen derselben Funktionen.

Beispiele:

- Bildbearbeitung
- PDF-Erzeugung
- OCR
- Übersetzung
- Datenanalyse
- Dateiverwaltung

Diese Funktionen sind häufig nur innerhalb der jeweiligen Anwendung verfügbar und können nur eingeschränkt von anderen Anwendungen genutzt werden.

Das erschwert:

- Wiederverwendbarkeit
- Automatisierung
- Parallelisierung
- Integration
- Optimierung
- Verteilung

NovaOS verfolgt deshalb ein anderes Softwaremodell.

---

## 2. Problemstellung

Das klassische Anwendungsmodell bringt mehrere strukturelle Nachteile mit sich.

### 2.1 Funktionsduplikate

Viele Programme implementieren dieselben Funktionen mehrfach.

Beispiele:

- PDF-Export
- Bildskalierung
- Druckfunktionen
- OCR
- Rechtschreibprüfung
- Dateidialoge

Dies erhöht Wartungsaufwand und Ressourcenverbrauch.

### 2.2 Geschlossene Anwendungen

Funktionen befinden sich innerhalb einer Anwendung und sind von außen oft nicht direkt nutzbar.

Andere Programme müssen dieselbe Funktion erneut implementieren oder externe Bibliotheken integrieren.

### 2.3 Schlechte Automatisierbarkeit

Das Betriebssystem kennt lediglich ausführbare Programme.

Es versteht jedoch nicht, welche Fähigkeiten diese Programme tatsächlich besitzen.

Dadurch kann das Betriebssystem keine intelligenten Arbeitsabläufe zusammenstellen.

### 2.4 Begrenzte Optimierung

Da Anwendungen als geschlossene Einheiten betrachtet werden, kann das Betriebssystem deren interne Verarbeitung kaum optimieren.

Beispielsweise kann es nicht automatisch:

- Berechnungen parallelisieren,
- GPU-Beschleunigung auswählen,
- Verarbeitungsschritte zusammenführen,
- unnötige Datenkopien vermeiden.

### 2.5 Starre Benutzeroberflächen

Der Benutzer muss zunächst die passende Anwendung auswählen, obwohl er eigentlich nur ein konkretes Ziel erreichen möchte.

Beispiele:

- „Ich möchte diesen Text übersetzen.“
- „Ich möchte das Bild verkleinern.“
- „Ich möchte ein PDF erzeugen.“

Die Auswahl der Anwendung ist ein technisches Detail und nicht Teil des eigentlichen Arbeitsziels.

---

## 3. Entscheidung

NovaOS ersetzt klassische Anwendungen durch **Fähigkeiten (Capabilities)** als primäres Softwaremodell.

Eine Fähigkeit beschreibt genau eine klar definierte Funktion.

Beispiele:

- OCR
- Bildskalierung
- Textübersetzung
- PDF-Erzeugung
- ZIP-Komprimierung
- Gesichtserkennung
- Tabellenanalyse
- Audiotranskription

Mehrere Fähigkeiten können zu einer Pipeline kombiniert werden.

Eine klassische Anwendung wird lediglich als Sammlung von Fähigkeiten betrachtet.

---

## 4. Definition einer Fähigkeit

Eine Fähigkeit ist die kleinste eigenständig beschreibbare Funktionseinheit innerhalb von NovaOS.

Jede Fähigkeit besitzt mindestens:

- eine eindeutige Identität,
- eine Version,
- Ein- und Ausgangsports,
- definierte Datentypen,
- Metadaten,
- Berechtigungen,
- Ressourcenanforderungen,
- Qualitätsmerkmale,
- Dokumentation.

Eine Fähigkeit besitzt keine verpflichtende Benutzeroberfläche.

Sie kann vollständig im Hintergrund arbeiten.

---

## 5. Grundprinzipien

### 5.1 Eine Fähigkeit erfüllt genau eine Aufgabe

Jede Fähigkeit besitzt eine klar abgegrenzte Verantwortung.

Beispiele:

✔ Bild skalieren

✔ Sprache erkennen

✔ Dokument signieren

✘ Komplettes Office-Paket

✘ Vollständiger Bildeditor

Je kleiner und eindeutiger eine Fähigkeit definiert ist, desto besser kann sie wiederverwendet werden.

### 5.2 Fähigkeiten sind kombinierbar

Komplexe Abläufe entstehen durch Kombination mehrerer Fähigkeiten.

Beispiel:

```text
Scan
   │
   ▼
OCR
   │
   ▼
Übersetzung
   │
   ▼
Zusammenfassung
   │
   ▼
PDF-Erzeugung
```

Die Fähigkeiten bleiben unabhängig voneinander.

### 5.3 Fähigkeiten sind austauschbar

Mehrere Fähigkeiten dürfen dieselbe Aufgabe erfüllen.

Beispiel:

```text
OCR
├── Tesseract
├── EasyOCR
├── CloudOCR
└── NovaOCR
```

NovaOS entscheidet anhand definierter Kriterien, welche Implementierung verwendet wird.

### 5.4 Fähigkeiten sind deklarativ beschrieben

Jede Fähigkeit beschreibt:

- was sie kann,
- welche Daten sie benötigt,
- welche Daten sie erzeugt,
- welche Voraussetzungen gelten,
- welche Qualität sie erreicht.

Das Betriebssystem kann dadurch Fähigkeiten automatisch finden und kombinieren.

### 5.5 Fähigkeiten sind unabhängig von ihrer Ausführung

Eine Fähigkeit beschreibt ausschließlich die Funktion.

Die Ausführung kann erfolgen:

- lokal,
- in einem Prozess,
- in einem Container,
- auf einem anderen Rechner,
- auf einem Cluster,
- auf einer GPU,
- auf einem Mikrocontroller,
- in einer Sandbox.

Für andere Fähigkeiten bleibt die Schnittstelle identisch.

### 5.6 Fähigkeiten besitzen keine feste Benutzeroberfläche

Eine Fähigkeit kann:

- vollständig automatisch,
- über eine grafische Oberfläche,
- über Sprache,
- über ein Skript,
- über eine Pipeline,
- über eine API,
- über einen Assistenten

genutzt werden.

Die Benutzeroberfläche ist nicht Bestandteil der Fähigkeit.

---

## 6. Anwendungen werden zu Lösungen

Eine klassische Anwendung existiert in NovaOS nur noch als sogenannte **Lösung (Solution)**.

Eine Lösung definiert:

- Benutzeroberfläche,
- Navigation,
- Layout,
- Arbeitsabläufe,
- gespeicherte Konfigurationen,
- bevorzugte Fähigkeiten.

Die eigentliche Funktionalität stammt ausschließlich von Fähigkeiten.

Dadurch können mehrere Lösungen dieselben Fähigkeiten verwenden.

---

## 7. Vorteile des Fähigkeitsmodells

Das neue Modell ermöglicht:

- maximale Wiederverwendung,
- geringe Redundanz,
- automatische Pipeline-Erstellung,
- intelligente Optimierung,
- parallele Ausführung,
- verteilte Berechnung,
- bessere Testbarkeit,
- einfachere Wartung,
- höhere Modularität,
- einheitliche Schnittstellen.

---

## 8. Konsequenzen

### Vorteile

- Funktionen werden unabhängig von Anwendungen nutzbar.
- Fähigkeiten können beliebig kombiniert werden.
- Das Betriebssystem versteht erstmals die eigentliche Bedeutung einer Softwarefunktion.
- KI kann Fähigkeiten automatisch auswählen und verbinden.
- Lösungen werden wesentlich schlanker.
- Neue Implementierungen können bestehende Fähigkeiten ersetzen, ohne Benutzeroberflächen anzupassen.
- Das System bleibt langfristig besser wartbar.

### Nachteile

- Höherer initialer Modellierungsaufwand.
- Größeres Metadatenmodell.
- Erfordert ein zentrales Capability-Register.
- Entwickler müssen kleinere, stärker spezialisierte Komponenten erstellen.
- Bestehende Anwendungen müssen in Fähigkeiten zerlegt werden.

Diese Nachteile werden zugunsten der langfristigen Skalierbarkeit und Flexibilität bewusst akzeptiert.

---

## 9. Auswirkungen auf andere Komponenten

Dieses ADR bildet die Grundlage für nahezu alle Kernkomponenten von NovaOS.

Betroffen sind insbesondere:

- Capability Registry
- Capability SDK
- Pipeline Planner
- Pipeline Executor
- Typregister
- Datenflussmodell
- Berechtigungssystem
- Capability Store
- Visual Pipeline Editor
- Solution Framework
- KI-Assistent
- Scheduler
- Runtime
- Remote Execution

Alle diese Komponenten müssen das Fähigkeitsmodell als primäre Softwareeinheit verwenden.

---

## 10. Implementierungshinweise

Die Einführung erfolgt schrittweise.

### Phase 1

- Definition des Capability-Modells
- Capability Descriptor
- Capability Registry

### Phase 2

- Pipeline Planner
- Datenflussmodell
- Typprüfung

### Phase 3

- Solution Framework
- Capability Store
- Visual Pipeline Editor

### Phase 4

- Verteilte Ausführung
- Optimierungen
- Selbstoptimierende Pipelines
- KI-gestützte Planung

---

## 11. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- Funktionen als Fähigkeiten modelliert werden.
- Fähigkeiten unabhängig voneinander ausführbar sind.
- Fähigkeiten deklarativ beschrieben werden.
- Ein- und Ausgänge typisiert sind.
- Fähigkeiten ohne feste Benutzeroberfläche funktionieren.
- Lösungen ausschließlich Fähigkeiten orchestrieren.
- Fähigkeiten austauschbar implementiert werden können.
- Das Betriebssystem Fähigkeiten automatisch erkennen und kombinieren kann.

---

## 12. Zusammenfassung

NovaOS ersetzt das klassische Anwendungsmodell durch ein vollständig fähigkeitsbasiertes Architekturmodell.

Fähigkeiten bilden die kleinste funktionale Einheit des Systems und beschreiben ausschließlich eine klar definierte Aufgabe.

Komplexe Anwendungen entstehen durch die Kombination vieler kleiner Fähigkeiten innerhalb einer Lösung.

Dieses Architekturprinzip bildet die Grundlage für automatische Pipeline-Erstellung, KI-gestützte Arbeitsabläufe, verteilte Ausführung, bessere Wiederverwendbarkeit und eine langfristig wartbare Softwareplattform.