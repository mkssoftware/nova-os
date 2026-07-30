# ADR-CAPABILITY-0013: Benutzerabsichten werden in Fähigkeiten-Graphen übersetzt

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0012
- **Folgedokumente:** NPSPEC-INTENT-0001, NPSPEC-PLANNER-0001, NPSPEC-GRAPH-0001, NPSPEC-ASSISTANT-0001

---

# 1. Kontext

Klassische Betriebssysteme erwarten, dass Benutzer bereits wissen, welche Anwendung sie starten und welche Arbeitsschritte sie ausführen müssen.

NovaOS verfolgt einen anderen Ansatz.

Benutzer beschreiben nicht mehr den Lösungsweg, sondern lediglich ihre Absicht (Intent).

Beispiele:

- „Übersetze dieses Dokument."
- „Finde alle Bilder mit Sonnenuntergang."
- „Erstelle aus diesen Daten einen Bericht."
- „Optimiere dieses Foto."
- „Analysiere diese Logdatei."

NovaOS übersetzt diese Benutzerabsicht automatisch in einen ausführbaren Graphen aus Fähigkeiten (Capability Graph).

Dadurch rückt das gewünschte Ergebnis in den Mittelpunkt und nicht mehr die Auswahl einzelner Programme oder Werkzeuge.

---

# 2. Problemstellung

Werden Benutzer gezwungen, den Lösungsweg selbst zu definieren, entstehen zahlreiche Nachteile.

## 2.1 Hohe Komplexität

Benutzer müssen wissen:

- welche Anwendungen benötigt werden,
- welche Reihenfolge erforderlich ist,
- welche Dateiformate unterstützt werden,
- welche Zwischenschritte notwendig sind.

---

## 2.2 Geringe Automatisierung

Das Betriebssystem kann keine intelligenten Optimierungen durchführen.

---

## 2.3 Schlechte Wiederverwendbarkeit

Ähnliche Aufgaben müssen immer wieder manuell erstellt werden.

---

## 2.4 Hohe Einstiegshürde

Neue Benutzer kennen die verfügbaren Fähigkeiten häufig nicht.

---

## 2.5 Begrenzte KI-Integration

Eine KI kann nur eingeschränkt unterstützen, wenn das Betriebssystem ausschließlich auf explizite Befehle reagiert.

---

# 3. Entscheidung

NovaOS interpretiert Benutzerabsichten als fachliche Ziele.

Eine Benutzerabsicht wird durch den Intent Planner automatisch in einen gerichteten Fähigkeiten-Graphen übersetzt.

Dieser Graph beschreibt:

- benötigte Fähigkeiten,
- deren Reihenfolge,
- Datenflüsse,
- Verzweigungen,
- Parallelisierung,
- Fehlerbehandlung.

Der Fähigkeiten-Graph wird anschließend validiert, optimiert und von der Pipeline Runtime ausgeführt.

---

# 4. Definition einer Benutzerabsicht

Eine Benutzerabsicht beschreibt **was** erreicht werden soll.

Sie beschreibt nicht **wie** die Aufgabe technisch umgesetzt wird.

Beispiele:

```text
Bilder sortieren

Dokument übersetzen

Musik transkribieren

PDF zusammenfassen

Backup erstellen
```

Die technische Umsetzung erfolgt ausschließlich durch die Plattform.

---

# 5. Definition eines Fähigkeiten-Graphen

Ein Fähigkeiten-Graph ist ein gerichteter Graph aus Capabilities.

Die Knoten repräsentieren Fähigkeiten.

Die Kanten repräsentieren typisierte Datenflüsse zwischen den Fähigkeiten.

Ein Graph kann:

- linear,
- verzweigt,
- parallel,
- rekursiv,
- verschachtelt

sein.

---

# 6. Grundprinzipien

## 6.1 Benutzer beschreiben Ziele

Benutzer formulieren ausschließlich ihre Absicht.

Beispiel:

```text
"Übersetze dieses Dokument ins Englische."
```

Nicht erforderlich sind Angaben wie:

- OCR starten,
- Sprache erkennen,
- Übersetzer auswählen,
- PDF erzeugen.

---

## 6.2 Die Plattform plant den Lösungsweg

Der Intent Planner analysiert die Benutzerabsicht und erstellt automatisch einen geeigneten Fähigkeiten-Graphen.

Dabei berücksichtigt er:

- Capability Contracts,
- Datentypen,
- verfügbare Implementierungen,
- Berechtigungen,
- Sicherheitsrichtlinien,
- Benutzerpräferenzen.

---

## 6.3 Fähigkeiten bleiben explizit

Auch wenn der Graph automatisch erzeugt wird, besteht er ausschließlich aus eindeutig definierten Fähigkeiten.

Es werden keine unbekannten oder impliziten Verarbeitungsschritte ausgeführt.

Jeder Schritt bleibt nachvollziehbar.

---

## 6.4 Der Graph ist editierbar

Ein automatisch erzeugter Graph darf vom Benutzer angepasst werden.

Beispielsweise können:

- Fähigkeiten ersetzt,
- Parameter geändert,
- zusätzliche Verarbeitungsschritte eingefügt,
- Ausführungsregeln angepasst

werden.

---

## 6.5 Der Graph ist reproduzierbar

Nach der Erstellung wird der Fähigkeiten-Graph als Pipeline gespeichert.

Weitere Ausführungen verwenden denselben validierten Graphen.

---

## 6.6 Planung und Ausführung sind getrennt

Die Planung endet mit einem vollständigen Fähigkeiten-Graphen.

Die Ausführung übernimmt anschließend die Pipeline Runtime.

Dadurch bleiben Planung und Ausführung unabhängig voneinander.

---

## 6.7 Mehrere Lösungswege sind zulässig

Für dieselbe Benutzerabsicht können mehrere gültige Fähigkeiten-Graphen existieren.

Beispiel:

```text
OCR

↓

Lokale Übersetzung
```

oder

```text
OCR

↓

Cloud-Übersetzung
```

Die Runtime oder der Benutzer kann den geeigneten Graphen auswählen.

---

## 6.8 Optimierung erfolgt nach der Planung

Nach der Graphenerstellung können Optimierungen erfolgen.

Beispiele:

- Parallelisierung,
- Modulbildung,
- Hardwarebeschleunigung,
- Zero-Copy,
- Remote-Ausführung.

Diese verändern nicht die fachliche Bedeutung des Graphen.

---

## 6.9 Jeder Graph ist nachvollziehbar

Der Benutzer kann jederzeit einsehen:

- welche Fähigkeiten verwendet werden,
- welche Daten fließen,
- welche Parameter gesetzt sind,
- welche Implementierungen gewählt wurden.

Dadurch bleibt die automatische Planung transparent.

---

## 6.10 KI unterstützt ausschließlich die Planung

KI-Systeme unterstützen:

- Interpretation der Benutzerabsicht,
- Auswahl geeigneter Fähigkeiten,
- Parametervorschläge,
- Optimierungsvorschläge.

Die eigentliche Ausführung erfolgt ausschließlich über Capability Contracts und die Pipeline Runtime.

Dadurch bleibt das System deterministisch, überprüfbar und unabhängig vom verwendeten KI-Modell.

---

# 7. Beispiel

Benutzer:

```text
"Erstelle aus diesem eingescannten Vertrag eine englische Zusammenfassung."
```

Automatisch erzeugter Fähigkeiten-Graph:

```text
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

Markdown erzeugen

↓

Speichern
```

Der Benutzer musste keinen einzelnen Verarbeitungsschritt selbst auswählen.

---

# 8. Lebenszyklus

Ein typischer Ablauf:

```text
Benutzerabsicht

↓

Intent Analyse

↓

Capability Suche

↓

Graph-Erstellung

↓

Validierung

↓

Optimierung

↓

Pipeline

↓

Ausführung

↓

Ergebnis
```

---

# 9. Vorteile

Dieses Architekturprinzip ermöglicht:

- aufgabenorientierte Bedienung,
- geringere Einstiegshürden,
- automatische Workflow-Erstellung,
- bessere Wiederverwendbarkeit,
- transparente KI-Unterstützung,
- reproduzierbare Pipelines,
- automatische Optimierungen,
- konsistente Ergebnisse,
- höhere Produktivität.

---

# 10. Nachteile

Dieses Modell erfordert:

- einen leistungsfähigen Intent Planner,
- semantische Analyse,
- Capability-Suche,
- Graph-Optimierung,
- Konfliktauflösung,
- zusätzliche Metadaten.

Diese Komplexität wird bewusst akzeptiert, da sie zentral innerhalb der Plattform gekapselt ist und den Benutzer deutlich entlastet.

---

# 11. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Intent Planner
- Capability Registry
- Pipeline Planner
- Pipeline Runtime
- Nova Assistant
- Semantic Search
- Capability Graph Engine
- Solution Framework
- Visual Pipeline Editor
- Optimizer
- Provenance Service

Alle diese Komponenten arbeiten gemeinsam an der Übersetzung einer Benutzerabsicht in einen ausführbaren Fähigkeiten-Graphen.

---

# 12. Implementierungshinweise

Der Intent Planner sollte Benutzerabsichten zunächst in eine semantische Zwischenrepräsentation überführen.

Auf Basis dieser Repräsentation werden geeignete Fähigkeiten über ihre Capability Contracts ausgewählt und zu einem gerichteten Graphen verbunden.

Der erzeugte Graph wird anschließend validiert, optimiert und als Pipeline gespeichert.

Die Architektur trennt dabei klar zwischen der Interpretation der Benutzerabsicht und der deterministischen Ausführung des resultierenden Graphen. Dieses Vorgehen entspricht dem allgemeinen Prinzip, Architekturentscheidungen und deren Begründungen klar zu dokumentieren und von ihrer konkreten Umsetzung zu trennen. :contentReference[oaicite:0]{index=0}

---

# 13. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- Benutzerabsichten unabhängig von konkreten Anwendungen beschrieben werden,
- jede Absicht in einen expliziten Fähigkeiten-Graphen übersetzt wird,
- ausschließlich registrierte Capabilities verwendet werden,
- Capability Contracts eingehalten werden,
- der Graph validierbar und reproduzierbar ist,
- Planung und Ausführung getrennt erfolgen,
- automatisch erzeugte Graphen für Benutzer nachvollziehbar bleiben.

---

# 14. Zusammenfassung

NovaOS stellt die Benutzerabsicht in den Mittelpunkt der Interaktion.

Anstatt Anwendungen oder Verarbeitungsschritte auszuwählen, beschreibt der Benutzer lediglich das gewünschte Ergebnis.

Der Intent Planner übersetzt diese Absicht automatisch in einen validierten Fähigkeiten-Graphen, der anschließend als reproduzierbare Pipeline ausgeführt wird.

Dieses Architekturprinzip verbindet semantische Benutzerinteraktion mit einer transparenten, überprüfbaren und modularen Capability-Architektur und bildet damit eine zentrale Grundlage für die KI-gestützte Bedienung von NovaOS.