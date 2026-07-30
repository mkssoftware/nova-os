# ADR-CAPABILITY-0004: Fähigkeiten dürfen zu optimierten Modulen gebündelt werden

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0002, ADR-CAPABILITY-0003, ADR-DATAFLOW-0001
- **Folgedokumente:** NPSPEC-CAPABILITY-MODULE-0001, NPSPEC-CAPABILITY-RUNTIME-0001

---

## 1. Kontext

NovaOS verwendet Fähigkeiten (Capabilities) als kleinste funktionale Einheit der Softwarearchitektur.

Diese starke Modularisierung verbessert Wartbarkeit, Wiederverwendbarkeit und automatische Planung. Gleichzeitig kann eine sehr feine Zerlegung zu zusätzlichem Laufzeitaufwand führen.

Typische Ursachen sind:

- viele Prozesswechsel,
- häufige Interprozesskommunikation,
- wiederholte Serialisierung,
- unnötige Speicherkopien,
- zahlreiche Scheduler-Wechsel,
- erhöhter Energieverbrauch.

Insbesondere bei Pipelines mit vielen kleinen Verarbeitungsschritten kann dieser Overhead die eigentliche Berechnungszeit deutlich übersteigen.

NovaOS benötigt daher einen Mechanismus, um eng zusammengehörige Fähigkeiten gemeinsam auszuführen, ohne das logische Capability-Modell aufzugeben.

---

## 2. Problemstellung

Ein strikt getrenntes Capability-Modell bringt folgende Herausforderungen mit sich.

### 2.1 Kommunikationsaufwand

Jede Fähigkeit kommuniziert über definierte Ports.

Bei sehr kleinen Verarbeitungsschritten kann der Kommunikationsaufwand größer sein als die eigentliche Berechnung.

### 2.2 Prozesswechsel

Werden alle Fähigkeiten in separaten Prozessen ausgeführt, entstehen zusätzliche Kontextwechsel.

Diese erhöhen:

- CPU-Last,
- Speicherbedarf,
- Energieverbrauch,
- Latenz.

### 2.3 Mehrfache Serialisierung

Daten müssen zwischen Prozessen häufig mehrfach serialisiert und deserialisiert werden.

Gerade große Bilder, Videos oder Tensoren verursachen dadurch erheblichen Aufwand.

### 2.4 Verpasste Optimierungen

Viele Fähigkeiten arbeiten unmittelbar hintereinander.

Beispielsweise:

```text
Bild laden
    ↓
Bild zuschneiden
    ↓
Bild skalieren
    ↓
Farbraum konvertieren
```

Zwischen diesen Schritten existieren oft keine externen Abhängigkeiten.

Eine gemeinsame Ausführung wäre deutlich effizienter.

---

## 3. Entscheidung

NovaOS unterscheidet konsequent zwischen:

- **logischer Architektur**
- **physischer Ausführung**

Logisch bleiben Fähigkeiten vollständig getrennte Komponenten.

Zur Laufzeit dürfen mehrere Fähigkeiten jedoch zu einem **optimierten Modul** zusammengefasst werden.

Diese Bündelung verändert **nicht** das logische Capability-Modell.

Nach außen bleiben sämtliche Fähigkeiten weiterhin einzeln sichtbar.

---

## 4. Definition eines Moduls

Ein Modul ist eine Laufzeiteinheit, welche mehrere Fähigkeiten gemeinsam ausführt.

Ein Modul besitzt insbesondere:

- eine gemeinsame Initialisierung,
- einen gemeinsamen Speicherbereich,
- gemeinsame Ressourcen,
- eine gemeinsame Optimierungsstrategie,
- interne direkte Funktionsaufrufe.

Nach außen bleiben sämtliche Fähigkeiten weiterhin eigenständig adressierbar.

---

## 5. Grundprinzipien

### 5.1 Fähigkeiten bleiben logisch unabhängig

Die Bündelung betrifft ausschließlich die Laufzeit.

Es entstehen keine neuen fachlichen Abhängigkeiten.

Beispiel:

```text
Capability A

Capability B

Capability C
```

Zur Laufzeit:

```text
Module X
├── Capability A
├── Capability B
└── Capability C
```

Für den Pipeline Planner existieren weiterhin drei eigenständige Fähigkeiten.

---

### 5.2 Module sind transparent

Eine Pipeline beschreibt ausschließlich Fähigkeiten.

Sie enthält niemals Module.

Beispiel:

```text
Bild laden
      ↓
Bild skalieren
      ↓
JPEG exportieren
```

Ob diese drei Fähigkeiten gemeinsam oder getrennt ausgeführt werden, entscheidet ausschließlich die Runtime.

---

### 5.3 Module dienen ausschließlich der Optimierung

Module dürfen ausschließlich technische Optimierungen ermöglichen.

Sie verändern nicht:

- Datentypen,
- Portdefinitionen,
- Semantik,
- Berechtigungen,
- Capability-IDs.

---

### 5.4 Module sind austauschbar

Eine Pipeline muss identische Ergebnisse liefern, unabhängig davon, ob Fähigkeiten einzeln oder gemeinsam ausgeführt werden.

Das Modul darf keine fachliche Bedeutung besitzen.

---

### 5.5 Interne Kommunikation kann optimiert werden

Innerhalb eines Moduls dürfen Fähigkeiten kommunizieren über:

- direkte Funktionsaufrufe,
- gemeinsame Speicherbereiche,
- Zero-Copy-Puffer,
- interne Objektreferenzen,
- GPU-Speicher,
- SIMD-Puffer.

Diese Optimierungen bleiben vollständig transparent.

---

### 5.6 Module besitzen keine eigene API

Ein Modul wird niemals direkt verwendet.

Es besitzt:

- keine Capability-ID,
- keine eigenen Ports,
- keine eigene Dokumentation als Fachkomponente.

Alle Schnittstellen gehören weiterhin den einzelnen Fähigkeiten.

---

### 5.7 Module können dynamisch entstehen

Die Runtime darf Module zur Laufzeit erzeugen.

Mögliche Kriterien:

- häufig gemeinsam verwendete Fähigkeiten,
- identischer Speicherbedarf,
- identische Hardware,
- identische Sicherheitsklasse,
- geringe Kommunikationskosten,
- bekannte Optimierungsmuster.

---

### 5.8 Module können wieder aufgelöst werden

Eine Bündelung ist nicht dauerhaft.

Die Runtime darf Module jederzeit:

- neu bilden,
- auflösen,
- erweitern,
- verkleinern.

Dadurch kann sich die Ausführungsstrategie dynamisch anpassen.

---

## 6. Beispiele

### Bildverarbeitung

```text
Capability

Bild laden
      ↓
Bild zuschneiden
      ↓
Bild skalieren
      ↓
JPEG exportieren
```

Laufzeit:

```text
Image Processing Module

├── Bild laden
├── Bild zuschneiden
├── Bild skalieren
└── JPEG exportieren
```

---

### Dokumentenverarbeitung

```text
OCR
      ↓
Übersetzung
      ↓
Zusammenfassung
      ↓
PDF
```

Diese Fähigkeiten können gemeinsam oder getrennt ausgeführt werden.

Die Entscheidung trifft ausschließlich die Runtime.

---

### GPU-Modul

```text
Tensor Multiply

Tensor Add

Tensor Normalize

Activation
```

Alle Fähigkeiten können innerhalb eines GPU-Moduls zusammengefasst werden.

Dadurch entfallen unnötige Speicherkopien zwischen CPU und GPU.

---

## 7. Optimierungskriterien

Eine Bündelung ist insbesondere sinnvoll bei:

- häufigen gemeinsamen Aufrufen,
- identischen Berechtigungen,
- identischer Sicherheitsklasse,
- großen Datenmengen,
- Zero-Copy-Übertragung,
- GPU-Ausführung,
- SIMD-Verarbeitung,
- identischer Hardware,
- kurzen Verarbeitungsschritten,
- hoher Kommunikationslast.

---

## 8. Grenzen

Module dürfen niemals:

- Fähigkeiten verschmelzen,
- Capability-IDs verändern,
- Portdefinitionen ändern,
- Berechtigungen umgehen,
- Typprüfungen deaktivieren,
- Debugging erschweren,
- Versionen verändern.

Die logische Architektur bleibt unverändert bestehen.

---

## 9. Vorteile

Dieses Architekturprinzip ermöglicht:

- geringere Latenzen,
- weniger Speicherkopien,
- reduzierte IPC,
- geringeren Energieverbrauch,
- bessere CPU-Auslastung,
- effizientere GPU-Nutzung,
- dynamische Laufzeitoptimierung,
- vollständige Beibehaltung des Capability-Modells.

---

## 10. Nachteile

Die Runtime wird komplexer.

Zusätzlich erforderlich sind:

- Modulbildung,
- Optimierungsalgorithmen,
- Laufzeitanalyse,
- dynamische Planung,
- Modulverwaltung.

Diese Komplexität wird bewusst akzeptiert, da sie ausschließlich innerhalb der Runtime verbleibt und für Entwickler vollständig transparent ist.

---

## 11. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Runtime
- Pipeline Executor
- Scheduler
- Optimizer
- Memory Manager
- GPU Runtime
- IPC Framework
- Remote Execution
- Capability SDK
- Profiler
- Debugger

Der Pipeline Planner arbeitet weiterhin ausschließlich auf Ebene einzelner Fähigkeiten.

---

## 12. Implementierungshinweise

Die Runtime sollte regelmäßig analysieren:

- welche Fähigkeiten häufig gemeinsam verwendet werden,
- welche Kommunikationskosten entstehen,
- welche Datenmengen übertragen werden,
- welche Hardware verfügbar ist.

Auf Basis dieser Informationen können Module automatisch erzeugt werden.

Die Modulbildung muss jederzeit deaktivierbar sein, um Debugging und Tests zu vereinfachen.

---

## 13. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- Fähigkeiten logisch unabhängig bleiben,
- Module ausschließlich Laufzeitoptimierungen darstellen,
- alle Capability-IDs erhalten bleiben,
- Portdefinitionen unverändert bleiben,
- Pipeline-Ergebnisse unabhängig von der Modulbildung identisch sind,
- Module transparent für Entwickler und Anwender bleiben,
- Module jederzeit wieder aufgelöst werden können.

---

## 14. Zusammenfassung

NovaOS trennt konsequent zwischen der logischen Capability-Architektur und ihrer physischen Ausführung.

Fähigkeiten bleiben die kleinste funktionale Einheit des Systems.

Zur Verbesserung der Laufzeiteffizienz dürfen mehrere eng zusammengehörige Fähigkeiten temporär zu optimierten Modulen gebündelt werden.

Dadurch werden Kommunikationsaufwand, Speicherkopien und Prozesswechsel reduziert, ohne die Modularität, Austauschbarkeit oder Wartbarkeit der Architektur zu beeinträchtigen.