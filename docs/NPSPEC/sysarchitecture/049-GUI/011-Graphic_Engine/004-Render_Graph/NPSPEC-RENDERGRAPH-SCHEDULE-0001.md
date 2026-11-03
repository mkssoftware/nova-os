# NPSPEC-RENDERGRAPH-SCHEDULE-0001 – Nova Render Graph Scheduling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Render Graph Scheduling definiert die Planung und Reihenfolge der Ausführung von Render Passes innerhalb des Nova Render Graph.

Ziel ist die effiziente Nutzung verfügbarer CPU- und GPU-Ressourcen bei minimaler Rendering-Latenz.

Die Planung berücksichtigt Abhängigkeiten, Ressourcenverfügbarkeit, Ausführungskosten und Parallelisierungsmöglichkeiten.

## 2. Architekturprinzipien

- **Dependency-Driven:** Abhängigkeiten bestimmen zulässige Ausführungsreihenfolgen.
- **Cost-Aware:** Geschätzte Ausführungskosten beeinflussen die Priorisierung.
- **Parallel:** Unabhängige Render Passes können gleichzeitig verarbeitet werden.
- **Resource-Aware:** Ressourcenverfügbarkeit und Speicherbedarf werden berücksichtigt.
- **Latency-Aware:** Kritische Ausführungspfade werden bevorzugt behandelt.
- **Incremental:** Unveränderte Ausführungspläne können wiederverwendet werden.
- **Adaptive:** Die Planung kann an Hardware und Arbeitslast angepasst werden.
- **Deterministic:** Ein reproduzierbarer Planungsmodus wird unterstützt.

## 3. Scheduling Model

Der Graph Scheduler verarbeitet einen validierten Render Graph und erzeugt einen ausführbaren Plan.

Ein Scheduling-Eintrag enthält mindestens:

- **Pass ID:** Zugehöriger Render Pass.
- **Dependencies:** Vorausgesetzte Operationen.
- **Execution Order:** Logische Ausführungsposition.
- **Execution Queue:** Vorgesehene GPU Queue oder CPU-Ausführung.
- **Resource Requirements:** Benötigte Ressourcen.
- **Synchronization Requirements:** Erforderliche Synchronisation.
- **Estimated Cost:** Erwarteter Ausführungsaufwand.

Der Ausführungsplan MUSS sämtliche relevanten Abhängigkeiten erhalten.

## 4. Scheduling Pipeline

Die Planung erfolgt grundsätzlich in folgenden Schritten:

1. Validierten Render Graph übernehmen.
2. Ausführbare Render Passes bestimmen.
3. Abhängigkeiten und kritische Pfade analysieren.
4. Verfügbare Ausführungsressourcen ermitteln.
5. Geeignete Queues und Ausführungspositionen zuweisen.
6. Parallelisierungsmöglichkeiten berücksichtigen.
7. Erforderliche Synchronisation einplanen.
8. Ausführungsplan validieren und bereitstellen.

Die konkrete GPU-Befehlsausführung erfolgt durch die Nova Render Engine.

## 5. Dependency Scheduling

Render Passes DÜRFEN erst ausgeführt werden, wenn ihre erforderlichen Abhängigkeiten erfüllt sind.

Dabei gilt:

- Abhängige Operationen benötigen eine gültige Ausführungsordnung.
- Unabhängige Operationen dürfen parallel ausgeführt werden.
- Ressourcenabhängigkeiten müssen erhalten bleiben.
- Explizite Ausführungsbedingungen müssen berücksichtigt werden.

Der Scheduler DARF keine Abhängigkeiten entfernen, die für korrekte Rendering-Ergebnisse erforderlich sind.

## 6. Critical Path Scheduling

NovaOS SOLL kritische Ausführungspfade innerhalb des Render Graph erkennen.

Ein kritischer Pfad beschreibt eine Abhängigkeitskette, die die frühestmögliche Fertigstellung des Graphen begrenzt.

Der Scheduler SOLL Render Passes auf kritischen Pfaden bevorzugt berücksichtigen, sofern dadurch keine anderen verbindlichen Anforderungen verletzt werden.

Die Priorisierung DARF nicht ausschließlich anhand einzelner Pass-Kosten erfolgen.

## 7. Parallel Scheduling

Unabhängige Render Passes DÜRFEN parallel vorbereitet oder ausgeführt werden.

NovaOS SOLL insbesondere unterstützen:

- Parallele CPU-Befehlsaufzeichnung
- Mehrere GPU Queues
- Asynchrone Compute-Operationen
- Unabhängige Copy-Operationen

Parallelisierung MUSS die Hardwarefähigkeiten und Synchronisationsanforderungen berücksichtigen.

Eine parallele Ausführung SOLL nur erfolgen, wenn ihr erwarteter Nutzen den zusätzlichen Verwaltungs- und Synchronisationsaufwand rechtfertigt.

## 8. Queue Assignment

Der Scheduler DARF Render Passes geeigneten Ausführungsqueues zuweisen.

Dabei werden berücksichtigt:

- Pass-Typ
- Unterstützte Queue-Funktionen
- Ressourcenabhängigkeiten
- Erwartete Ausführungsdauer
- Synchronisationskosten
- Verfügbare Hardwarekapazitäten

Nicht unterstützte Queue-Zuweisungen MÜSSEN ausgeschlossen werden.

Ein Backend mit nur einer geeigneten GPU Queue MUSS weiterhin unterstützt werden.

## 9. Resource-Aware Scheduling

Die Ausführungsplanung MUSS Ressourcenlebenszeiten und Verfügbarkeit berücksichtigen.

Dabei SOLL NovaOS:

- Unnötige Ressourcenbelegungen vermeiden.
- Temporäre Ressourcen effizient nutzen.
- Speicherbedarf paralleler Passes berücksichtigen.
- Ressourcenfreigaben nach Abschluss ihrer Verwendung ermöglichen.

Eine Optimierung der Ausführungszeit DARF nicht zu ungültigen Ressourcenzugriffen führen.

## 10. Incremental Scheduling

NovaOS SOLL vorhandene Ausführungspläne wiederverwenden können.

Eine Neuplanung ist insbesondere erforderlich bei:

- Veränderten Render-Pass-Abhängigkeiten
- Hinzugefügten oder entfernten Passes
- Geänderten Ressourcenanforderungen
- Veränderten Hardwarebedingungen
- Ungültigen Queue-Zuweisungen

Unveränderte Teilpläne SOLLEN erhalten bleiben, sofern ihre Voraussetzungen weiterhin gültig sind.

## 11. Adaptive Scheduling

NovaOS DARF die Ausführungsplanung anhand aktueller Leistungsinformationen anpassen.

Dazu gehören:

- CPU- und GPU-Auslastung
- Gemessene Pass-Ausführungszeiten
- Synchronisationswartezeiten
- Ressourcenverfügbarkeit
- Rendering-Latenz

Die Anpassung SOLL unnötige Planungswechsel vermeiden.

Die tatsächlichen Kosten der Neuplanung MÜSSEN bei der Optimierungsentscheidung berücksichtigt werden können.

## 12. Synchronization Integration

Nova Render Graph Scheduling bestimmt die zeitliche und logische Ausführungsordnung.

Nova Render Resource Barriers und Nova GPU Synchronization setzen die erforderlichen Synchronisationsmaßnahmen um.

Dabei gilt:

- Queue-übergreifende Abhängigkeiten benötigen geeignete Synchronisation.
- Speicherzugriffe müssen korrekt sichtbar gemacht werden.
- Ressourcenübergänge müssen berücksichtigt werden.
- GPU-Aufträge dürfen nicht vorzeitig als abgeschlossen gelten.

Die Scheduling-Reihenfolge allein ersetzt keine erforderlichen Speicherbarrieren.

## 13. Frame Scheduler Integration

Nova Render Graph Scheduling und Nova Frame Scheduling besitzen getrennte Verantwortlichkeiten.

- **Render Graph Scheduler:** Plant die Ausführung einzelner Rendering-Operationen.
- **Frame Scheduler:** Bestimmt, wann ein Frame erzeugt und zur Darstellung vorbereitet wird.

Der Render Graph Scheduler SOLL zeitliche Vorgaben des Frame Schedulers berücksichtigen können.

Er DARF keine kontinuierliche Frame-Erzeugung erzwingen.

Ohne erforderliche Rendering-Arbeit MUSS keine neue Graph-Ausführung gestartet werden.

## 14. Deterministic Scheduling

NovaOS MUSS einen deterministischen Planungsmodus unterstützen können.

In diesem Modus:

- Werden definierte Priorisierungsregeln verwendet.
- Bleibt die Queue-Zuweisung bei identischen Voraussetzungen reproduzierbar.
- Werden nicht deterministische Laufzeitheuristiken ausgeschlossen.
- Erzeugen identische Eingaben und Konfigurationen identische Ausführungspläne.

Die tatsächliche zeitliche GPU-Ausführung kann weiterhin hardwareabhängig sein.

## 15. Fehlerbehandlung

NovaOS MUSS ungültige oder nicht ausführbare Scheduling-Pläne erkennen.

Dazu gehören:

- Nicht erfüllte Abhängigkeiten
- Ungültige Queue-Zuweisungen
- Fehlende Ressourcen
- Nicht unterstützte Hardwarefunktionen
- Nicht erfüllbare Synchronisationsbedingungen

Bei fehlgeschlagener Optimierung MUSS eine konservative, gültige Ausführungsreihenfolge möglich sein.

Ein sequenzieller Fallback DARF verwendet werden, sofern alle Ressourcen- und Synchronisationsanforderungen erfüllt werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Render Graph Scheduling unterstützen.
2. Der Scheduler MUSS einen validierten Render Graph verarbeiten.
3. Sämtliche Render-Pass-Abhängigkeiten MÜSSEN erhalten bleiben.
4. Unabhängige Render Passes DÜRFEN parallel ausgeführt werden.
5. Kritische Ausführungspfade SOLLEN berücksichtigt werden.
6. Rendering-Kosten SOLLEN die Priorisierung beeinflussen können.
7. Queue-Zuweisungen MÜSSEN mit den Hardwarefähigkeiten kompatibel sein.
8. Ressourcenlebenszeiten MÜSSEN berücksichtigt werden.
9. Erforderliche Synchronisation MUSS eingeplant werden.
10. Unveränderte Ausführungspläne SOLLEN wiederverwendet werden.
11. Adaptive Planungsstrategien DÜRFEN eingesetzt werden.
12. Die Planung MUSS mit Nova Frame Scheduling integrierbar sein.
13. Ohne erforderliche Rendering-Arbeit DARF keine unnötige Graph-Ausführung erzwungen werden.
14. Ein deterministischer Planungsmodus MUSS unterstützt werden.
15. Ungültige Ausführungspläne DÜRFEN nicht ausgeführt werden.
16. Ein konservativer Scheduling-Fallback MUSS verfügbar sein.

## 17. Ergebnis

Nova Render Graph Scheduling organisiert die Ausführung grafischer Rendering-Operationen anhand ihrer Abhängigkeiten, Kosten und Ressourcenanforderungen.

Durch kritische Pfadanalyse, parallele Verarbeitung, adaptive Queue-Zuweisung und inkrementelle Wiederverwendung kann NovaOS Rendering-Aufträge mit möglichst geringer Latenz ausführen.

Damit bildet das Subsystem die zentrale Verbindung zwischen dem logischen Nova Render Graph und der tatsächlichen Ausführung durch die Nova Render Engine.