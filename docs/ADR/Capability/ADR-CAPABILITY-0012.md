# ADR-CAPABILITY-0012: Fähigkeitspipelines sind reproduzierbare ausführbare Objekte

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0008, ADR-CAPABILITY-0009, ADR-CAPABILITY-0011
- **Folgedokumente:** NPSPEC-PIPELINE-0001, NPSPEC-PIPELINE-FORMAT-0001, NPSPEC-PIPELINE-RUNTIME-0001, NPSPEC-PROVENANCE-0001

---

# 1. Kontext

NovaOS betrachtet einzelne Fähigkeiten (Capabilities) als kleinste funktionale Einheiten.

Die eigentliche Arbeit eines Benutzers besteht jedoch selten aus einer einzelnen Fähigkeit.

Typische Arbeitsabläufe setzen sich aus vielen Fähigkeiten zusammen.

Beispiele:

- Dokument analysieren
- Bilder bearbeiten
- Daten importieren
- KI-Inferenz
- Simulationen
- Software-Deployment

Diese Abläufe müssen gespeichert, wiederverwendet, geteilt, versioniert und reproduzierbar ausgeführt werden können.

NovaOS behandelt deshalb eine Fähigkeitspipeline nicht lediglich als temporären Ablauf, sondern als eigenständiges ausführbares Objekt.

Dieses Prinzip verbessert Nachvollziehbarkeit, Wiederverwendbarkeit und Reproduzierbarkeit komplexer Prozesse. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Werden Pipelines lediglich zur Laufzeit erzeugt, entstehen zahlreiche Nachteile.

## 2.1 Fehlende Wiederverwendbarkeit

Komplexe Abläufe müssen jedes Mal neu erstellt werden.

---

## 2.2 Fehlende Reproduzierbarkeit

Es ist nicht eindeutig nachvollziehbar:

- welche Fähigkeiten verwendet wurden,
- welche Versionen beteiligt waren,
- welche Parameter verwendet wurden,
- welche Entscheidungen getroffen wurden.

---

## 2.3 Schlechte Zusammenarbeit

Pipelines lassen sich nur schwer austauschen oder gemeinsam bearbeiten.

---

## 2.4 Fehlende Versionierung

Änderungen an einem Ablauf können nicht zuverlässig nachvollzogen werden.

---

## 2.5 Erschwerte Optimierung

Die Runtime besitzt keine dauerhafte Beschreibung der Pipeline.

Dadurch können Optimierungen nur eingeschränkt erfolgen.

---

# 3. Entscheidung

NovaOS definiert Fähigkeitspipelines als **eigenständige, deklarative und ausführbare Objekte**.

Eine Pipeline besitzt:

- eine eindeutige Identität,
- eine Version,
- eine strukturierte Beschreibung,
- definierte Ein- und Ausgänge,
- Metadaten,
- optionale Signaturen.

Eine Pipeline kann gespeichert, verteilt, signiert, versioniert und jederzeit reproduzierbar erneut ausgeführt werden.

---

# 4. Definition einer Pipeline

Eine Pipeline beschreibt einen gerichteten Datenfluss zwischen mehreren Fähigkeiten.

Sie definiert:

- beteiligte Fähigkeiten,
- deren Verbindungen,
- Ein- und Ausgänge,
- Parameter,
- Ausführungsregeln,
- Fehlerbehandlung.

Eine Pipeline enthält keine Implementierungsdetails einzelner Fähigkeiten.

---

# 5. Grundprinzipien

## 5.1 Pipelines sind eigenständige Objekte

Eine Pipeline besitzt eine eigene Identität.

Beispiel:

```text
Pipeline

Dokument analysieren
```

Sie ist unabhängig von den verwendeten Fähigkeiten speicherbar.

---

## 5.2 Pipelines werden deklarativ beschrieben

Eine Pipeline beschreibt ausschließlich:

- Fähigkeiten,
- Ports,
- Verbindungen,
- Parameter,
- Regeln.

Sie enthält keinen imperativen Programmcode.

---

## 5.3 Pipelines sind reproduzierbar

Bei identischen Voraussetzungen soll dieselbe Pipeline reproduzierbar ausgeführt werden.

Hierzu gehören insbesondere:

- dieselbe Pipeline-Version,
- dieselben Capability Contracts,
- identische Parameter,
- identische Eingabedaten,
- identische Ausführungsregeln.

Soweit Fähigkeiten deterministisch arbeiten, muss auch das Ergebnis reproduzierbar sein.

Nichtdeterministische Fähigkeiten müssen ihr Verhalten deklarativ kennzeichnen.

---

## 5.4 Pipelines besitzen eine Version

Jede Pipeline wird versioniert.

Änderungen an:

- Fähigkeiten,
- Verbindungen,
- Parametern,
- Fehlerbehandlung,
- Metadaten

erzeugen eine neue Pipeline-Version.

---

## 5.5 Pipelines besitzen Ein- und Ausgänge

Eine Pipeline besitzt eigene Ports.

Beispiel:

```text
Input

PDF

↓

OCR

↓

Übersetzung

↓

Zusammenfassung

↓

Output

Markdown
```

Dadurch kann eine Pipeline selbst wieder Bestandteil einer größeren Pipeline werden.

---

## 5.6 Pipelines sind verschachtelbar

Eine Pipeline darf innerhalb einer anderen Pipeline verwendet werden.

Beispiel:

```text
Pipeline A

↓

Pipeline B

↓

Pipeline C
```

Dadurch entstehen hierarchische Arbeitsabläufe.

---

## 5.7 Pipelines sind unabhängig von der Runtime

Eine Pipeline beschreibt nicht:

- CPU,
- GPU,
- Cluster,
- Cloud,
- Prozesse,
- Threads.

Die Runtime entscheidet weiterhin über die tatsächliche Ausführung.

---

## 5.8 Pipelines besitzen Metadaten

Eine Pipeline kann Metadaten enthalten.

Beispiele:

- Name,
- Beschreibung,
- Autor,
- Lizenz,
- Tags,
- Kategorie,
- Sicherheitsklassifizierung,
- benötigte Berechtigungen,
- Zielplattformen.

---

## 5.9 Pipelines unterstützen Provenance

Jede Ausführung kann nachvollziehbar dokumentiert werden.

Beispielsweise:

- verwendete Pipeline-Version,
- verwendete Capability-Versionen,
- Eingabedaten,
- Zeitstempel,
- Ausführungsort,
- Fehler,
- Laufzeit,
- Ergebnisstatus.

Dadurch werden Auditierbarkeit und wissenschaftliche Reproduzierbarkeit verbessert. :contentReference[oaicite:1]{index=1}

---

## 5.10 Pipelines sind signierbar

Eine Pipeline darf digital signiert werden.

Dadurch können sichergestellt werden:

- Authentizität,
- Integrität,
- Herkunft,
- Vertrauenswürdigkeit.

---

# 6. Beispiel

```text
Pipeline

PDF

↓

OCR

↓

Sprache erkennen

↓

Übersetzen

↓

Zusammenfassen

↓

Markdown

↓

Speichern
```

Diese Pipeline kann gespeichert und beliebig oft erneut ausgeführt werden.

---

# 7. Lebenszyklus

Eine Pipeline durchläuft typischerweise folgende Phasen:

```text
Erstellen

↓

Validieren

↓

Versionieren

↓

Signieren

↓

Registrieren

↓

Ausführen

↓

Überwachen

↓

Archivieren
```

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- reproduzierbare Arbeitsabläufe,
- einfache Wiederverwendung,
- Versionierung,
- grafische Pipeline-Editoren,
- einfache Zusammenarbeit,
- bessere Dokumentation,
- Auditierbarkeit,
- langfristige Wartbarkeit,
- automatische Optimierungen,
- einfache Verteilung.

---

# 9. Nachteile

Dieses Modell erfordert:

- Pipeline-Descriptoren,
- Versionsverwaltung,
- Validierungsmechanismen,
- Provenance-Erfassung,
- zusätzliche Metadaten.

Diese Komplexität wird bewusst akzeptiert, da sie die Grundlage einer reproduzierbaren und automatisierbaren Plattform bildet.

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Pipeline Runtime
- Pipeline Registry
- Pipeline Planner
- Capability Runtime
- Solution Framework
- Visual Pipeline Editor
- Package Manager
- Version Manager
- Provenance Service
- Audit Service
- Nova Assistant

Alle diese Komponenten arbeiten mit Pipelines als eigenständigen Objekten.

---

# 11. Implementierungshinweise

Pipelines sollten in einem deklarativen Format gespeichert werden.

Eine Pipeline sollte unabhängig von ihrer eigentlichen Ausführung analysiert, validiert und versioniert werden können.

Die Runtime sollte jede Ausführung optional mit Provenance-Daten ergänzen, um spätere Analysen, Audits und eine möglichst genaue Reproduzierbarkeit zu ermöglichen. :contentReference[oaicite:2]{index=2}

---

# 12. Konformitätskriterien

Eine Pipeline gilt als konform, wenn:

- sie deklarativ beschrieben ist,
- sie eindeutig versioniert ist,
- alle Fähigkeiten über Capability Contracts referenziert werden,
- Ein- und Ausgänge vollständig beschrieben sind,
- sie unabhängig von einer konkreten Runtime ausführbar bleibt,
- sie validierbar ist,
- sie als eigenständiges Objekt gespeichert und verteilt werden kann,
- ihre Ausführung nachvollziehbar dokumentiert werden kann.

---

# 13. Zusammenfassung

NovaOS behandelt Fähigkeitspipelines als eigenständige, deklarative und ausführbare Objekte.

Sie beschreiben vollständige Arbeitsabläufe unabhängig von ihrer technischen Ausführung und können gespeichert, versioniert, signiert, verteilt und reproduzierbar erneut ausgeführt werden.

Durch ihre deklarative Struktur und optionale Provenance-Erfassung bilden Pipelines die Grundlage für nachvollziehbare, automatisierbare und langfristig wartbare Workflows innerhalb der Nova-Plattform. :contentReference[oaicite:3]{index=3}