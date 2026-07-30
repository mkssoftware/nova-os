# ADR-CAPABILITY-0007: Fähigkeiten sind systemweit nutzbar und nicht an einzelne Oberflächen gebunden

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0006
- **Folgedokumente:** NPSPEC-CAPABILITY-0001, NPSPEC-SOLUTION-0001, NPSPEC-UI-FRAGMENT-0001

---

## 1. Kontext

Klassische Betriebssysteme koppeln Funktionen häufig eng an eine bestimmte Anwendung oder Benutzeroberfläche.

Beispiele:

- Ein Bild kann nur innerhalb eines Bildbearbeitungsprogramms bearbeitet werden.
- Eine Übersetzungsfunktion existiert nur in einer bestimmten Anwendung.
- Eine PDF-Erzeugung ist ausschließlich innerhalb eines Texteditors verfügbar.
- Eine OCR-Funktion kann nur über eine spezielle Oberfläche gestartet werden.

Dadurch entstehen voneinander isolierte Funktionsinseln.

NovaOS verfolgt einen anderen Ansatz.

Funktionen gehören nicht einer Anwendung oder Benutzeroberfläche, sondern dem gesamten Betriebssystem.

Jede Fähigkeit kann systemweit verwendet werden, unabhängig davon, über welche Oberfläche sie aufgerufen wird.

---

## 2. Problemstellung

Werden Fähigkeiten an einzelne Oberflächen gekoppelt, entstehen zahlreiche Nachteile.

### 2.1 Funktionsduplikate

Mehrere Anwendungen implementieren dieselbe Funktion mit unterschiedlicher Qualität.

### 2.2 Inkonsistentes Verhalten

Dieselbe Aufgabe liefert je nach Anwendung unterschiedliche Ergebnisse.

### 2.3 Begrenzte Wiederverwendung

Eine Funktion kann nur innerhalb der ursprünglichen Anwendung genutzt werden.

### 2.4 Schlechte Automatisierung

Das Betriebssystem kann Funktionen nicht systemweit kombinieren.

### 2.5 Hoher Wartungsaufwand

Fehlerkorrekturen oder Verbesserungen müssen mehrfach implementiert werden.

---

## 3. Entscheidung

Fähigkeiten gehören ausschließlich zur Capability-Plattform von NovaOS.

Sie sind **nicht Bestandteil einer bestimmten Benutzeroberfläche oder Anwendung**.

Jede Fähigkeit kann systemweit verwendet werden:

- durch grafische Oberflächen,
- durch den Nova-Assistenten,
- durch Pipelines,
- durch Skripte,
- durch APIs,
- durch Automatisierungen,
- durch andere Fähigkeiten,
- durch Hintergrunddienste.

Die Art des Aufrufs hat keinen Einfluss auf die Funktion der Fähigkeit.

---

## 4. Grundprinzipien

### 4.1 Fähigkeiten gehören dem Betriebssystem

Eine Fähigkeit ist Bestandteil der Nova-Plattform.

Sie gehört niemals exklusiv zu:

- einer Anwendung,
- einer Desktopoberfläche,
- einem Widget,
- einem Dialog,
- einer Shell,
- einem Dienst.

Alle Komponenten greifen auf dieselbe Capability zu.

---

### 4.2 Benutzeroberflächen sind lediglich Clients

Eine grafische Oberfläche verwendet Fähigkeiten.

Sie besitzt diese jedoch nicht.

Beispiel:

```text
Nova Explorer
        │
        ▼
Capability

Datei kopieren
```

Ebenso:

```text
Nova Assistant
        │
        ▼
Capability

Datei kopieren
```

Beide nutzen dieselbe Fähigkeit.

---

### 4.3 Eine Fähigkeit besitzt keine feste Oberfläche

Eine Fähigkeit kann gleichzeitig verwendet werden durch:

- Desktop,
- Tablet,
- Smartphone,
- Sprachsteuerung,
- Terminal,
- Pipeline,
- REST API,
- Remote-Aufruf.

Die Fähigkeit kennt keine dieser Oberflächen.

---

### 4.4 Lösungen verwenden Fähigkeiten

Eine Solution beschreibt lediglich:

- Layout,
- Navigation,
- Arbeitsabläufe,
- gespeicherte Konfigurationen,
- bevorzugte Fähigkeiten.

Die eigentliche Funktionalität stammt ausschließlich aus den verwendeten Fähigkeiten.

---

### 4.5 Mehrere Oberflächen können dieselbe Fähigkeit verwenden

Beispiel:

```text
Bild verkleinern
```

Nutzung durch:

```text
Nova Explorer

Nova Photo

Nova Assistant

Visual Pipeline

CLI

REST API

Automation
```

Alle greifen auf dieselbe Capability zurück.

---

### 4.6 Fähigkeiten bleiben zustandsunabhängig

Eine Fähigkeit besitzt keinen dauerhaft an eine Oberfläche gebundenen Zustand.

Benutzerspezifische Informationen werden außerhalb der Fähigkeit gespeichert.

Beispielsweise:

- in einer Solution,
- in einem Projekt,
- in einer Sitzung,
- in Benutzerprofilen.

Dadurch kann dieselbe Fähigkeit parallel von mehreren Oberflächen genutzt werden.

---

### 4.7 Oberflächen dürfen Fähigkeiten unterschiedlich präsentieren

Die Darstellung darf variieren.

Beispiele:

Desktop:

```text
Dialog
```

Sprachsteuerung:

```text
Gespräch
```

Pipeline:

```text
Node
```

REST:

```text
HTTP Request
```

CLI:

```text
Kommando
```

Die zugrunde liegende Fähigkeit bleibt identisch.

---

### 4.8 Fähigkeiten dürfen UI-Fragmente bereitstellen

Eine Fähigkeit kann optionale UI-Fragmente bereitstellen.

Beispiele:

- Konfigurationsdialog,
- Vorschaufenster,
- Statusanzeige,
- Eigenschafteneditor,
- Assistent.

Diese Fragmente gehören jedoch **nicht zur Fähigkeit selbst**.

Sie sind optionale Darstellungen derselben Funktion.

---

### 4.9 Fähigkeiten besitzen keine festen Navigationspfade

Eine Fähigkeit darf nicht voraussetzen, über welche Menüstruktur sie erreichbar ist.

Die Navigation gehört ausschließlich zur jeweiligen Oberfläche.

---

### 4.10 Fähigkeiten unterstützen Headless-Ausführung

Jede Fähigkeit soll grundsätzlich ohne grafische Oberfläche ausführbar sein.

Ausnahmen sind ausschließlich Fähigkeiten, deren eigentliche Aufgabe unmittelbar eine Benutzerinteraktion erfordert.

Beispiele:

- Dateiauswahldialog,
- Kameraaufnahme,
- Benutzerauthentifizierung.

---

## 5. Beispiele

### OCR

Nutzung durch:

```text
Explorer

Scanner-App

Nova Assistant

Visual Pipeline

REST API
```

Alle verwenden dieselbe OCR-Fähigkeit.

---

### PDF-Erzeugung

Nutzung durch:

```text
Texteditor

Berichtsgenerator

Rechnungssoftware

Automation

CLI
```

Keine Anwendung besitzt ihre eigene PDF-Funktion.

---

### Übersetzung

Nutzung durch:

```text
Browser

Messenger

E-Mail

Office

Terminal

Nova Assistant
```

Alle greifen auf dieselbe Übersetzungsfähigkeit zu.

---

## 6. Vorteile

Dieses Architekturprinzip ermöglicht:

- systemweite Wiederverwendbarkeit,
- einheitliches Verhalten,
- geringeren Wartungsaufwand,
- weniger Funktionsduplikate,
- bessere Automatisierung,
- konsistente Benutzererfahrung,
- einfachere Erweiterbarkeit,
- flexible Oberflächen,
- zentrale Optimierungen.

---

## 7. Nachteile

Dieses Modell erfordert:

- eine klare Trennung zwischen Oberfläche und Funktion,
- zusätzliche Descriptoren,
- UI-Fragmente als optionale Komponenten,
- konsequente Zustandsverwaltung außerhalb der Fähigkeit.

Diese Nachteile werden bewusst akzeptiert, da sie die langfristige Modularität und Wiederverwendbarkeit erheblich verbessern.

---

## 8. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Solution Framework
- Capability Runtime
- Nova Shell
- Explorer
- Nova Assistant
- Visual Pipeline Editor
- CLI
- REST Gateway
- Remote Execution
- UI Fragment Framework
- Automatisierungsplattform

Alle diese Komponenten verwenden dieselben Fähigkeiten.

---

## 9. Implementierungshinweise

Jede Fähigkeit muss vollständig unabhängig von einer bestimmten Benutzeroberfläche entwickelt werden.

Optionale UI-Komponenten werden separat registriert und können von unterschiedlichen Oberflächen eingebunden werden.

Die Runtime stellt sicher, dass Fähigkeiten sowohl interaktiv als auch vollständig headless ausgeführt werden können.

---

## 10. Konformitätskriterien

Eine Fähigkeit gilt als konform, wenn:

- sie unabhängig von einer Benutzeroberfläche funktioniert,
- sie systemweit verwendet werden kann,
- sie keine Navigation voraussetzt,
- sie keine UI-spezifischen Zustände speichert,
- sie sowohl interaktiv als auch headless nutzbar ist,
- optionale UI-Fragmente getrennt registriert werden,
- ihre Funktion unabhängig von der verwendeten Oberfläche identisch bleibt.

---

## 11. Zusammenfassung

NovaOS trennt konsequent zwischen Funktionalität und Benutzeroberfläche.

Fähigkeiten gehören ausschließlich zur Capability-Plattform und stehen dem gesamten Betriebssystem zur Verfügung.

Grafische Oberflächen, Assistenten, Automatisierungen, Skripte und APIs greifen gleichermaßen auf dieselben Fähigkeiten zu.

Dieses Architekturprinzip verhindert Funktionsduplikate, verbessert die Wiederverwendbarkeit und ermöglicht eine konsistente, systemweite Nutzung aller Fähigkeiten unabhängig von der jeweiligen Benutzeroberfläche.