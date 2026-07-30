# ADR-CAPABILITY-0015: Automatische Entscheidungen müssen für den Benutzer nachvollziehbar sein

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture / Explainability
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0008, ADR-CAPABILITY-0011, ADR-CAPABILITY-0012, ADR-CAPABILITY-0013
- **Folgedokumente:** NPSPEC-EXPLAINABILITY-0001, NPSPEC-INTENT-0001, NPSPEC-PROVENANCE-0001, NPSPEC-PIPELINE-RUNTIME-0001

---

# 1. Kontext

NovaOS automatisiert zahlreiche Entscheidungen.

Beispiele sind:

- Auswahl geeigneter Fähigkeiten,
- Pipeline-Planung,
- Hardwareauswahl,
- Priorisierung von Aufgaben,
- Optimierung von Datenflüssen,
- Auswahl einer Implementierung,
- Remote-Ausführung,
- KI-gestützte Planung.

Automatisierung darf jedoch nicht dazu führen, dass das Betriebssystem zu einer "Black Box" wird.

Der Benutzer muss jederzeit nachvollziehen können, **welche Entscheidung getroffen wurde, warum sie getroffen wurde und welche Alternativen existierten**.

Eine transparente Entscheidungsfindung stärkt Vertrauen, erleichtert Fehlersuche und verbessert die langfristige Wartbarkeit der Plattform. :contentReference[oaicite:0]{index=0}

---

# 2. Problemstellung

Werden automatische Entscheidungen nicht erklärt, entstehen erhebliche Nachteile.

## 2.1 Vertrauensverlust

Benutzer verstehen nicht, warum das System eine bestimmte Entscheidung getroffen hat.

---

## 2.2 Erschwerte Fehlersuche

Fehler können kaum analysiert werden, wenn Entscheidungswege verborgen bleiben.

---

## 2.3 Fehlende Kontrollmöglichkeiten

Der Benutzer kann Entscheidungen nicht überprüfen oder bei Bedarf korrigieren.

---

## 2.4 Geringe Akzeptanz

Automatische Systeme werden schlechter akzeptiert, wenn ihre Entscheidungen nicht nachvollziehbar sind.

---

## 2.5 Erschwerte Weiterentwicklung

Entwickler können Optimierungen nur schwer durchführen, wenn die ursprünglichen Entscheidungsgründe nicht dokumentiert sind.

---

# 3. Entscheidung

NovaOS verpflichtet sämtliche automatisierten Planungs- und Entscheidungsmechanismen zur Erklärbarkeit.

Jede automatisch getroffene Entscheidung muss auf Anfrage nachvollziehbar dargestellt werden.

Dabei werden mindestens dokumentiert:

- die getroffene Entscheidung,
- ihre Begründung,
- die verwendeten Informationen,
- verworfene Alternativen,
- die Auswirkungen auf den weiteren Ablauf.

Die Erklärung erfolgt unabhängig von der eigentlichen Ausführung.

---

# 4. Definition einer automatischen Entscheidung

Eine automatische Entscheidung liegt vor, wenn NovaOS ohne unmittelbare Benutzeranweisung zwischen mehreren gültigen Alternativen auswählt.

Beispiele:

- Capability-Auswahl,
- Pipeline-Optimierung,
- Hardwareauswahl,
- Scheduling,
- Priorisierung,
- Remote-Ausführung,
- Wahl einer Implementierung,
- Auswahl eines KI-Modells.

---

# 5. Grundprinzipien

## 5.1 Entscheidungen sind erklärbar

Jede automatische Entscheidung besitzt eine maschinenlesbare und menschenverständliche Begründung.

---

## 5.2 Entscheidungen bleiben nachvollziehbar

Zu jeder Entscheidung werden mindestens gespeichert:

- Zeitpunkt,
- Entscheidung,
- Begründung,
- verwendete Regeln,
- beteiligte Fähigkeiten,
- verwendete Parameter.

---

## 5.3 Alternativen werden dokumentiert

Soweit sinnvoll werden auch verworfene Alternativen dokumentiert.

Beispiel:

```text
Gewählt:

Lokale GPU

Nicht gewählt:

CPU

Grund:

GPU verfügbar und schneller
```

---

## 5.4 Entscheidungen sind reproduzierbar

Werden dieselben Eingaben und Rahmenbedingungen verwendet, muss dieselbe Entscheidung reproduzierbar sein, sofern keine explizit nichtdeterministischen Strategien eingesetzt werden.

---

## 5.5 Erklärungen beeinflussen die Ausführung nicht

Die Dokumentation einer Entscheidung verändert deren fachliche Bedeutung nicht.

Erklärung und Ausführung bleiben voneinander getrennt.

---

## 5.6 Benutzer können Details wählen

NovaOS unterstützt unterschiedliche Erklärungsebenen.

Beispiele:

```text
Kurz

↓

"GPU gewählt"
```

```text
Standard

↓

"GPU gewählt, da verfügbar und schneller."
```

```text
Experte

↓

Vollständige Entscheidungsregeln

Leistungsbewertung

Alternative Implementierungen

Benchmarkdaten
```

---

## 5.7 Entscheidungen sind auditierbar

Automatische Entscheidungen können Bestandteil der Provenance-Dokumentation werden.

Dadurch lassen sich komplette Ausführungen später nachvollziehen.

---

## 5.8 KI bleibt transparent

KI-Systeme dürfen Vorschläge erzeugen.

Die endgültige Ausführung erfolgt jedoch ausschließlich auf Basis expliziter Capability Contracts, Pipeline-Regeln und Runtime-Entscheidungen.

Die KI ersetzt keine nachvollziehbare Architekturentscheidung.

---

## 5.9 Benutzer behalten die Kontrolle

Benutzer dürfen automatische Entscheidungen übersteuern, sofern Sicherheitsrichtlinien oder Systemintegrität dadurch nicht verletzt werden.

Beispiele:

- andere Hardware wählen,
- andere Capability verwenden,
- Remote-Ausführung deaktivieren,
- Pipeline anpassen.

---

## 5.10 Erklärungen sind Bestandteil der Plattform

Erklärbarkeit ist keine optionale Funktion einzelner Fähigkeiten.

Sie ist ein zentraler Bestandteil der Nova-Plattform und gilt für sämtliche automatisierten Entscheidungsprozesse.

---

# 6. Beispiele

## Hardwareauswahl

```text
Entscheidung:

GPU
```

```text
Begründung:

GPU verfügbar

Genügend Speicher

Kürzere Laufzeit erwartet
```

---

## Capability-Auswahl

```text
Entscheidung:

OCR Version 3
```

```text
Begründung:

Neueste stabile Version

Deutsch optimiert

Passender Capability Contract
```

---

## Pipelineplanung

```text
Entscheidung:

Parallelisierung aktiviert
```

```text
Begründung:

Keine Datenabhängigkeiten

Vier CPU-Kerne verfügbar
```

---

# 7. Lebenszyklus

Ein typischer Ablauf:

```text
Benutzerabsicht

↓

Planung

↓

Automatische Entscheidung

↓

Begründung erzeugen

↓

Ausführung

↓

Provenance speichern
```

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- höhere Transparenz,
- größeres Vertrauen,
- bessere Fehlersuche,
- reproduzierbare Entscheidungen,
- einfachere Audits,
- bessere Wartbarkeit,
- nachvollziehbare KI-Unterstützung,
- höhere Benutzerkontrolle,
- langfristige Dokumentation.

---

# 9. Nachteile

Dieses Modell erfordert:

- zusätzliche Metadaten,
- Dokumentation der Entscheidungsgründe,
- Provenance-Erfassung,
- Explainability-Komponenten,
- zusätzlichen Speicherbedarf.

Diese Mehrkosten werden bewusst akzeptiert, da Transparenz und Nachvollziehbarkeit grundlegende Qualitätsmerkmale einer modernen, automatisierten Plattform darstellen. :contentReference[oaicite:1]{index=1}

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Intent Planner
- Capability Runtime
- Pipeline Runtime
- Optimizer
- Scheduler
- Hardware Manager
- Provenance Service
- Audit Service
- Nova Assistant
- Capability Registry

Alle diese Komponenten müssen ihre automatischen Entscheidungen nachvollziehbar dokumentieren.

---

# 11. Implementierungshinweise

Jede automatische Entscheidung sollte als strukturierter Entscheidungsdatensatz gespeichert werden.

Dabei sollten Entscheidung, Begründung, Alternativen und die zugrunde liegenden Regeln getrennt von der eigentlichen Ausführung abgelegt werden.

Die Benutzeroberfläche sollte verschiedene Erklärungsebenen anbieten, sodass sowohl Einsteiger als auch Experten die für sie passende Detailtiefe erhalten.

Erklärungen sollten grundsätzlich aus den tatsächlich verwendeten Entscheidungsdaten erzeugt werden und nicht lediglich nachträglich formulierte Begründungen darstellen. Architekturentscheidungen profitieren nachweislich von strukturierter Nachvollziehbarkeit und expliziter Begründung. :contentReference[oaicite:2]{index=2}

---

# 12. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- automatische Entscheidungen dokumentiert werden,
- ihre Begründung nachvollziehbar verfügbar ist,
- verwendete Regeln und Parameter erfasst werden,
- Entscheidungen reproduzierbar sind,
- unterschiedliche Erklärungsebenen unterstützt werden,
- Entscheidungen Bestandteil der Provenance sein können,
- Benutzer zulässige Entscheidungen übersteuern können.

---

# 13. Zusammenfassung

NovaOS verpflichtet sämtliche automatisierten Entscheidungsprozesse zur Transparenz.

Jede automatisch getroffene Entscheidung muss nachvollziehbar, reproduzierbar und auf Wunsch erklärbar sein.

Dadurch bleibt das System trotz umfangreicher Automatisierung kontrollierbar und überprüfbar.

Dieses Architekturprinzip bildet eine wesentliche Grundlage für Vertrauen, Auditierbarkeit und eine langfristig verständliche Capability-Plattform.