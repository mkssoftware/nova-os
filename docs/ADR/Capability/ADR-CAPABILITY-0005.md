# ADR-CAPABILITY-0005: Fähigkeiten werden über stabile Verträge statt Implementierungsdetails angesprochen

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0002, ADR-CAPABILITY-0003, ADR-DATAFLOW-0001
- **Folgedokumente:** NPSPEC-CAPABILITY-CONTRACT-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001, NPSPEC-TYPEREGISTRY-0001

---

## 1. Kontext

NovaOS basiert auf einem fähigkeitsorientierten Softwaremodell.

Fähigkeiten werden von unterschiedlichen Entwicklern, Unternehmen oder der NovaOS-Plattform selbst bereitgestellt.

Damit Fähigkeiten langfristig austauschbar bleiben, dürfen andere Komponenten nicht von ihrer konkreten Implementierung abhängig sein.

Stattdessen muss ausschließlich beschrieben werden:

- welche Funktion bereitgestellt wird,
- welche Ein- und Ausgänge existieren,
- welche Datentypen verwendet werden,
- welche Eigenschaften garantiert werden.

Die interne Implementierung bleibt vollständig verborgen.

Dieses Prinzip entspricht dem Grundsatz **"Program against contracts, not implementations."**

---

## 2. Problemstellung

Werden Fähigkeiten direkt über Implementierungsdetails verwendet, entstehen zahlreiche Probleme.

### 2.1 Starke Kopplung

Andere Komponenten kennen interne Klassen, Bibliotheken oder Programmlogik.

Dadurch werden Änderungen erschwert.

### 2.2 Fehlende Austauschbarkeit

Eine neue Implementierung kann nicht eingesetzt werden, ohne abhängige Komponenten anzupassen.

### 2.3 Eingeschränkte Optimierung

Die Runtime kann Implementierungen nicht austauschen oder optimieren, wenn diese direkt referenziert werden.

### 2.4 Schlechte Wartbarkeit

Interne Änderungen führen häufig zu Inkompatibilitäten.

Dies erhöht den Wartungsaufwand erheblich.

### 2.5 Fehlende Verteilung

Direkte Implementierungsreferenzen funktionieren häufig nur lokal.

Eine Ausführung auf anderen Geräten oder in einer Cloud wird dadurch erschwert.

---

## 3. Entscheidung

Fähigkeiten werden ausschließlich über **stabile Capability-Verträge (Capability Contracts)** angesprochen.

Ein Capability Contract beschreibt vollständig:

- die Identität einer Fähigkeit,
- ihre Version,
- ihre Ein- und Ausgangsports,
- Datentypen,
- garantierte Eigenschaften,
- unterstützte Optionen,
- Qualitätsmerkmale,
- Sicherheitsanforderungen,
- Laufzeiteigenschaften.

Der Vertrag beschreibt **nicht**, wie eine Fähigkeit intern implementiert wird.

---

## 4. Definition eines Capability Contracts

Ein Capability Contract ist die öffentliche Schnittstelle einer Fähigkeit.

Er definiert alles, was andere Komponenten wissen dürfen.

Ein Contract enthält insbesondere:

- Capability-ID,
- Name,
- Version,
- Beschreibung,
- Portdefinitionen,
- Datentypen,
- unterstützte Features,
- Fehlerdefinitionen,
- Berechtigungen,
- Kompatibilitätsinformationen.

Alle internen Implementierungsdetails bleiben privat.

---

## 5. Grundprinzipien

### 5.1 Verträge definieren Verhalten

Ein Contract beschreibt ausschließlich das beobachtbare Verhalten.

Beispiel:

```text
Capability

Translate Text

Input:
Nova.Document.PlainText

Output:
Nova.Document.PlainText
```

Der Contract beschreibt nicht:

- verwendete Bibliotheken,
- KI-Modell,
- Datenbank,
- Programmiersprache,
- Algorithmen.

---

### 5.2 Implementierungen sind austauschbar

Mehrere Implementierungen dürfen denselben Contract erfüllen.

Beispiel:

```text
Contract

Text Translation
```

Implementierungen:

```text
NovaTranslate

DeepL Adapter

LibreTranslate

Offline LLM

Cloud Translator
```

Alle erfüllen denselben Vertrag.

---

### 5.3 Aufruf erfolgt ausschließlich über den Contract

Andere Fähigkeiten kennen ausschließlich:

- Capability-ID,
- Version,
- Ports,
- Datentypen.

Sie kennen niemals:

- Klassen,
- Funktionen,
- DLL-Namen,
- Dateipfade,
- Speicheradressen,
- Prozesse.

---

### 5.4 Runtime wählt die Implementierung

Die Auswahl erfolgt ausschließlich durch NovaOS.

Mögliche Kriterien:

- Geschwindigkeit,
- Energieverbrauch,
- Hardware,
- Sicherheitsrichtlinien,
- Kosten,
- Benutzerpräferenzen,
- Offlinefähigkeit,
- Vertrauensniveau.

Die aufrufende Fähigkeit bleibt unverändert.

---

### 5.5 Verträge bleiben stabil

Ein Contract darf sich nur im Rahmen definierter Kompatibilitätsregeln ändern.

Breaking Changes erfordern eine neue Hauptversion.

Nicht brechende Erweiterungen dürfen beispielsweise sein:

- optionale Parameter,
- neue optionale Ports,
- zusätzliche Metadaten,
- zusätzliche Qualitätsmerkmale.

---

### 5.6 Verträge sind deklarativ

Capability Contracts werden nicht im Quellcode analysiert.

Sie werden deklarativ beschrieben.

Beispielsweise in:

- YAML,
- JSON,
- Binary Descriptor,
- Nova Descriptor Format.

Dadurch kann die Runtime Contracts lesen, ohne die Fähigkeit laden zu müssen.

---

### 5.7 Verträge sind sprachunabhängig

Der Contract besitzt keinerlei Abhängigkeit von einer Programmiersprache.

Eine Fähigkeit kann beispielsweise implementiert sein in:

- NovaLang,
- Rust,
- C,
- C++,
- Zig,
- Python,
- Java,
- C#.

Für andere Komponenten bleibt der Contract identisch.

---

### 5.8 Verträge definieren Garantien

Ein Contract beschreibt nicht nur Datentypen.

Er kann zusätzlich garantieren:

- maximale Laufzeit,
- deterministisches Verhalten,
- Thread-Sicherheit,
- Streaming-Unterstützung,
- GPU-Unterstützung,
- Zero-Copy-Fähigkeit,
- Echtzeitfähigkeit,
- Offlinefähigkeit.

Diese Eigenschaften können vom Pipeline Planner berücksichtigt werden.

---

## 6. Beispiel

Ein Capability Contract könnte vereinfacht wie folgt aussehen:

```yaml
id: nova.image.resize

version: 1.0

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
```

Keine Information über die interne Implementierung ist Bestandteil des Contracts.

---

## 7. Versionierung

Jeder Capability Contract besitzt eine Version.

Änderungen werden unterschieden in:

- Patch-Version,
- Minor-Version,
- Major-Version.

Eine neue Major-Version ist erforderlich bei:

- geänderten Datentypen,
- entfernten Ports,
- geänderter Semantik,
- inkompatiblen Änderungen.

---

## 8. Austauschbarkeit

Eine Implementierung kann jederzeit ersetzt werden.

Beispiel:

Vorher:

```text
NovaOCR
```

Nachher:

```text
NovaOCR 2
```

oder

```text
FastOCR
```

oder

```text
GPUOCR
```

Alle erfüllen denselben Contract.

Für bestehende Pipelines entsteht dadurch kein Änderungsbedarf.

---

## 9. Vorteile

Dieses Architekturprinzip ermöglicht:

- geringe Kopplung,
- langfristige Stabilität,
- einfache Austauschbarkeit,
- automatische Optimierung,
- parallele Implementierungen,
- sprachunabhängige Entwicklung,
- einfache Verteilung,
- bessere Testbarkeit,
- klar definierte APIs,
- langfristige Kompatibilität.

---

## 10. Nachteile

Dieses Modell erfordert:

- sorgfältig definierte Contracts,
- Versionierung,
- Kompatibilitätsprüfungen,
- Descriptor-Dateien,
- zusätzlichen Metadatenaufwand.

Diese Nachteile werden bewusst akzeptiert, da sie die Grundlage einer langfristig stabilen Plattform bilden.

---

## 11. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Registry
- Capability Descriptor
- Capability Runtime
- Pipeline Planner
- Pipeline Executor
- Typregister
- Capability SDK
- Solution Framework
- Capability Store
- Remote Execution
- Debugger
- KI-Assistent

Alle diese Komponenten arbeiten ausschließlich mit Capability Contracts.

---

## 12. Implementierungshinweise

Jede Fähigkeit muss mindestens einen öffentlichen Contract bereitstellen.

Der Contract muss unabhängig von der eigentlichen Implementierung gelesen werden können.

Die Runtime lädt zunächst ausschließlich den Contract.

Die eigentliche Implementierung wird erst ausgewählt und geladen, wenn die Fähigkeit tatsächlich ausgeführt werden soll.

Dadurch können Planung, Validierung und Optimierung erfolgen, ohne Programmlogik auszuführen.

---

## 13. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- sie einen deklarativen Capability Contract besitzt,
- ausschließlich der Contract öffentlich sichtbar ist,
- Implementierungsdetails verborgen bleiben,
- Capability-IDs stabil bleiben,
- Versionierung eingehalten wird,
- Contracts sprachunabhängig sind,
- mehrere Implementierungen denselben Contract erfüllen können,
- alle externen Zugriffe ausschließlich über den Contract erfolgen.

---

## 14. Zusammenfassung

NovaOS trennt konsequent zwischen der öffentlichen Beschreibung einer Fähigkeit und ihrer internen Implementierung.

Jede Fähigkeit wird ausschließlich über einen stabilen Capability Contract angesprochen.

Dadurch bleiben Implementierungen austauschbar, sprachunabhängig und langfristig kompatibel, während die Runtime jederzeit die technisch optimale Umsetzung auswählen kann, ohne bestehende Pipelines oder Anwendungen zu verändern.