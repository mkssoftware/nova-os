# ADR-CAPABILITY-0002: Fähigkeiten folgen dem Prinzip „eine Aufgabe, eine klar definierte Funktion“

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001
- **Folgedokumente:** NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001

---

## 1. Kontext

Mit der Einführung des Capability-Modells werden Fähigkeiten zur kleinsten funktionalen Einheit von NovaOS.

Damit dieses Modell langfristig wartbar, kombinierbar und automatisierbar bleibt, muss jede Fähigkeit klar abgegrenzt sein.

Erfahrungen aus klassischen Betriebssystemen zeigen, dass Anwendungen mit der Zeit immer mehr Funktionen aufnehmen.

Beispiele:

- Ein Bildbetrachter enthält plötzlich einen Bildeditor.
- Ein Texteditor enthält einen PDF-Generator.
- Ein Dateimanager enthält Cloud-Synchronisation.
- Ein Mediaplayer enthält einen Downloader.
- Ein Browser enthält einen Passwortmanager, Messenger und KI-Assistenten.

Dadurch entstehen monolithische Komponenten mit vielen Abhängigkeiten und unklaren Verantwortlichkeiten.

NovaOS soll diese Entwicklung bewusst vermeiden.

---

## 2. Problemstellung

Werden Fähigkeiten zu groß oder übernehmen mehrere Aufgaben gleichzeitig, entstehen zahlreiche Nachteile.

### 2.1 Geringe Wiederverwendbarkeit

Eine große Fähigkeit kann häufig nur vollständig verwendet werden.

Andere Projekte benötigen jedoch oft nur einen kleinen Teil ihrer Funktionalität.

### 2.2 Schwierige Wartung

Je mehr Aufgaben eine Fähigkeit übernimmt, desto komplexer wird ihre Weiterentwicklung.

Änderungen an einer Teilfunktion können unbeabsichtigt andere Bereiche beeinflussen.

### 2.3 Schlechte Austauschbarkeit

Eine Fähigkeit kann nur vollständig ersetzt werden.

Ein einzelner Verarbeitungsschritt lässt sich nicht unabhängig verbessern.

### 2.4 Eingeschränkte Parallelisierung

Mehrere voneinander unabhängige Aufgaben können nicht getrennt geplant oder auf unterschiedliche Ressourcen verteilt werden.

### 2.5 Erschwerte automatische Planung

Der Pipeline Planner kann große Mehrzweckfähigkeiten schlechter analysieren und optimieren.

Er erkennt nicht mehr eindeutig, welche Funktion tatsächlich bereitgestellt wird.

### 2.6 Unklare Verantwortlichkeiten

Je mehr Aufgaben eine Fähigkeit übernimmt, desto schwieriger wird ihre Dokumentation, Testbarkeit und Fehleranalyse.

---

## 3. Entscheidung

Jede Fähigkeit erfüllt genau **eine klar definierte fachliche Aufgabe**.

Eine Fähigkeit besitzt:

- einen eindeutigen Zweck,
- einen klar beschriebenen Ein- und Ausgang,
- eine klar definierte Verantwortung,
- ein nachvollziehbares Verhalten.

Sobald mehrere unabhängige Aufgaben entstehen, müssen diese in getrennte Fähigkeiten aufgeteilt werden.

Dieses Prinzip orientiert sich am **Single Responsibility Principle (SRP)**, wird jedoch auf die gesamte Capability-Architektur von NovaOS angewendet.

---

## 4. Grundprinzipien

### 4.1 Eine Fähigkeit besitzt genau einen Zweck

Die Beschreibung einer Fähigkeit muss in einem einzigen Satz möglich sein.

Beispiele:

✔ Text übersetzen

✔ PDF erzeugen

✔ Bild skalieren

✔ Sprache erkennen

✔ Barcode lesen

✔ Audio normalisieren

Wenn mehrere unterschiedliche Tätigkeiten beschrieben werden müssen, handelt es sich wahrscheinlich um mehrere Fähigkeiten.

---

### 4.2 Eine Fähigkeit besitzt genau eine Verantwortung

Eine Fähigkeit darf nicht gleichzeitig:

- Daten importieren,
- Daten bearbeiten,
- Daten visualisieren,
- Daten exportieren.

Diese Schritte werden in einzelne Fähigkeiten zerlegt.

Beispiel:

```text
CSV lesen
      │
      ▼
Datensatz validieren
      │
      ▼
Datensatz filtern
      │
      ▼
Diagramm erzeugen
      │
      ▼
PDF erzeugen
```

Jede Fähigkeit besitzt dabei genau eine Verantwortung.

---

### 4.3 Fähigkeiten bleiben klein

Eine Fähigkeit soll möglichst klein gehalten werden.

Nicht die Größe des Quellcodes ist entscheidend, sondern die fachliche Verantwortung.

Eine Fähigkeit mit 5000 Zeilen Code kann trotzdem genau eine Aufgabe erfüllen.

Ebenso kann eine Fähigkeit mit 100 Zeilen bereits mehrere Verantwortlichkeiten vermischen.

---

### 4.4 Fähigkeiten dürfen kombiniert werden

Komplexe Abläufe entstehen ausschließlich durch Kombination kleiner Fähigkeiten.

Beispiel:

```text
Bild laden
      │
      ▼
Bild zuschneiden
      │
      ▼
Bild skalieren
      │
      ▼
Wasserzeichen hinzufügen
      │
      ▼
JPEG exportieren
```

Keine dieser Fähigkeiten kennt den vollständigen Arbeitsablauf.

---

### 4.5 Fähigkeiten besitzen keine versteckten Nebenaufgaben

Eine Fähigkeit darf ausschließlich die dokumentierte Aufgabe ausführen.

Unzulässig wären beispielsweise:

- ungefragte Internetzugriffe,
- automatische Datensynchronisation,
- Hintergrundanalysen,
- Telemetrie,
- automatische Formatänderungen,
- zusätzliche Dateierzeugung,
- versteckte Konvertierungen.

Alle Nebeneffekte müssen explizit beschrieben oder in eigene Fähigkeiten ausgelagert werden.

---

### 4.6 Fähigkeiten sind austauschbar

Mehrere Implementierungen dürfen dieselbe Aufgabe erfüllen.

Beispiel:

```text
Bild skalieren

├── CPUScaler
├── GPURasterScaler
├── VulkanScaler
├── AIUpscaler
└── FPGAImageScaler
```

Da alle dieselbe Aufgabe erfüllen, können sie vom System gegeneinander ausgetauscht werden.

---

### 4.7 Fähigkeiten besitzen definierte Ein- und Ausgänge

Eine Fähigkeit darf nur über ihre definierten Ports mit der Außenwelt kommunizieren.

Versteckte Seiteneffekte sind zu vermeiden.

Beispiel:

```text
Input:
    Nova.Image.Raster

Output:
    Nova.Image.Raster
```

Alle Änderungen erfolgen ausschließlich über diese Schnittstellen.

---

### 4.8 Fähigkeiten sind deterministisch

Eine Fähigkeit soll bei identischen Eingaben unter identischen Bedingungen dasselbe Ergebnis liefern.

Nicht deterministisches Verhalten muss ausdrücklich dokumentiert werden.

Beispiele:

- Zufallszahlengenerator
- KI-Modell mit Sampling
- Echtzeit-Sensordaten
- Netzwerkabfragen

---

## 5. Kriterien zur Zerlegung

Eine Fähigkeit sollte aufgeteilt werden, wenn mindestens eines der folgenden Merkmale zutrifft:

- mehrere unabhängige Aufgaben,
- mehrere unterschiedliche Datentypen,
- mehrere Ausführungsstrategien,
- unterschiedliche Berechtigungen,
- unterschiedliche Lebenszyklen,
- unterschiedliche Versionierungszyklen,
- unterschiedliche Fehlerbehandlungen,
- unterschiedliche Optimierungsziele.

---

## 6. Beispiele

### Positives Beispiel

```text
OCR
```

Aufgabe:

> Erzeugt Text aus einem Bild.

Keine weiteren Verantwortlichkeiten.

---

### Negatives Beispiel

```text
Dokumentenverwaltung
```

Enthält:

- OCR
- Übersetzung
- PDF
- Cloud
- Suche
- Signatur
- Druck

Diese Aufgaben müssen getrennte Fähigkeiten werden.

---

### Positives Beispiel

```text
Audio normalisieren
```

Nur:

- Lautstärke analysieren
- Pegel anpassen

Nicht:

- Schneiden
- Exportieren
- Konvertieren
- Streaming

---

## 7. Vorteile

Dieses Prinzip ermöglicht:

- hohe Wiederverwendbarkeit,
- einfache Austauschbarkeit,
- bessere Testbarkeit,
- klare Dokumentation,
- geringere Kopplung,
- bessere Parallelisierung,
- automatische Pipeline-Optimierung,
- einfachere Versionsverwaltung,
- bessere Fehlerdiagnose,
- langfristige Wartbarkeit.

---

## 8. Nachteile

Das Prinzip führt zu:

- einer größeren Anzahl einzelner Fähigkeiten,
- mehr Metadaten,
- größeren Capability-Registern,
- mehr Pipeline-Knoten.

Diese Nachteile werden bewusst akzeptiert, da sie die Architektur langfristig deutlich flexibler machen.

---

## 9. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Registry
- Capability SDK
- Pipeline Planner
- Pipeline Executor
- Solution Framework
- Visual Pipeline Editor
- Capability Store
- Runtime
- Scheduler
- Debugger
- KI-Assistent

Alle diese Komponenten gehen davon aus, dass Fähigkeiten klein, klar abgegrenzt und unabhängig voneinander sind.

---

## 10. Implementierungshinweise

Die Einhaltung dieses Prinzips wird bereits bei der Registrierung einer Fähigkeit überprüft.

Das Capability SDK sollte Werkzeuge bereitstellen, um:

- Verantwortlichkeiten zu dokumentieren,
- Ein- und Ausgänge eindeutig zu beschreiben,
- Seiteneffekte zu deklarieren,
- Abhängigkeiten zu minimieren.

Zusätzlich sollten statische Analysewerkzeuge Hinweise geben, wenn Fähigkeiten übermäßig viele Aufgaben übernehmen.

---

## 11. Konformitätskriterien

Eine Fähigkeit gilt als konform, wenn:

- sie genau eine fachliche Aufgabe erfüllt,
- ihre Verantwortung eindeutig beschrieben werden kann,
- Ein- und Ausgänge klar definiert sind,
- keine versteckten Nebenaufgaben vorhanden sind,
- sie unabhängig von anderen Fähigkeiten getestet werden kann,
- sie ohne Änderungen mit alternativen Implementierungen derselben Aufgabe austauschbar ist.

---

## 12. Zusammenfassung

NovaOS schreibt vor, dass jede Fähigkeit genau eine klar definierte Aufgabe erfüllt.

Dieses Prinzip bildet die Grundlage für eine modulare, wartbare und automatisierbare Capability-Plattform.

Komplexe Anwendungen entstehen nicht durch große Mehrzweckkomponenten, sondern durch die Kombination vieler kleiner, spezialisierter Fähigkeiten.