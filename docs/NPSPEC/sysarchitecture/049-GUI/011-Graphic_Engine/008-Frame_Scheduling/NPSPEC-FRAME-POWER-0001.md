# NPSPEC-FRAME-POWER-0001 – Nova Power-Aware Frame Scheduling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Power-Aware Frame Scheduling definiert die energieoptimierte Planung grafischer Frames innerhalb der NovaOS-Grafikpipeline.

Ziel ist die Reduzierung des Energieverbrauchs von CPU, GPU und Speicher, ohne notwendige Aktualisierungen, grafische Korrektheit oder die Reaktionsfähigkeit der Benutzeroberfläche zu beeinträchtigen.

Die Energieoptimierung erfolgt bedarfsgesteuert und berücksichtigt Systemzustand, Hardwarefähigkeiten und Benutzerpräferenzen.

## 2. Architekturprinzipien

- **Demand-Driven:** Keine Frame-Erzeugung ohne tatsächlichen Aktualisierungsbedarf.
- **Energy-Aware:** Energieverbrauch beeinflusst Scheduling-Entscheidungen.
- **Idle-First:** Unnötige Grafikaktivität wird konsequent vermieden.
- **Adaptive:** Dynamische Anpassung an Energiezustand und Systemlast.
- **Latency-Aware:** Interaktive Aktualisierungen behalten angemessene Reaktionszeiten.
- **Batching:** Zusammenfassung geeigneter Grafikoperationen.
- **Hardware-Aware:** Nutzung verfügbarer Energiesparfunktionen.
- **User-Controlled:** Benutzerpräferenzen werden berücksichtigt.

## 3. Power Scheduling Model

Der Frame Scheduler berücksichtigt mindestens folgende Informationen:

| Eigenschaft | Beschreibung |
|---|---|
| Power Mode | Aktuelles Energieprofil |
| Power Source | Netzbetrieb oder Akku |
| CPU Load | Prozessorbelastung |
| GPU Load | Grafikprozessorbelastung |
| Frame Demand | Tatsächlicher Aktualisierungsbedarf |
| Frame Priority | Dringlichkeit der Aktualisierung |
| Render Cost | Geschätzter Rendering-Aufwand |
| Display State | Zustand der Bildschirmausgabe |
| Visibility State | Sichtbarkeit grafischer Inhalte |

Nicht verfügbare Messwerte MÜSSEN durch definierte Standardstrategien behandelt werden.

## 4. Power Modes

NovaOS SOLL folgende Energieprofile unterstützen:

| Modus | Verhalten |
|---|---|
| Performance | Priorisierung geringer Latenz und hoher Leistung |
| Balanced | Ausgewogenes Verhältnis zwischen Leistung und Energieverbrauch |
| Power Saving | Reduzierung nicht notwendiger Grafikaktivität |
| Critical Power | Maximale Einsparung bei kritischem Energiezustand |

Die Profile definieren Scheduling-Richtlinien und keine festen globalen Rendering-Frequenzen.

Benutzerdefinierte Profile DÜRFEN unterstützt werden.

## 5. Energy-Aware Frame Scheduling

NovaOS SOLL die Verarbeitung erforderlicher Frames anhand ihres Energiebedarfs optimieren.

Mögliche Maßnahmen:

1. Zusammenfassung nicht dringender Frame-Anforderungen.
2. Reduzierung unnötiger Zwischenframes.
3. Anpassung von Frame-Deadlines.
4. Begrenzung nicht kritischer Aktualisierungsfrequenzen.
5. Wiederverwendung gültiger Rendering-Ergebnisse.
6. Vermeidung unnötiger GPU-Aktivierungen.

Energieoptimierungen DÜRFEN erforderlichen Damage nicht verlieren.

## 6. Idle und GPU Power States

Nova Power-Aware Frame Scheduling MUSS mit Nova Idle Frame Suppression zusammenarbeiten.

Dabei gilt:

- Ohne Aktualisierungsbedarf werden keine neuen Frames erzeugt.
- Unnötige GPU-Submissions werden vermieden.
- CPU und GPU können geeignete Energiesparzustände nutzen.
- Aktive Buffer und Synchronisationsanforderungen bleiben gültig.
- Neue Ereignisse aktivieren die erforderlichen Komponenten.

Der Frame Scheduler SOLL längere zusammenhängende Idle-Phasen ermöglichen, sofern dadurch keine notwendigen Deadlines verletzt werden.

Die tatsächliche Steuerung der Hardware-Energiezustände erfolgt durch die zuständigen Treiber und das Power Management.

## 7. Adaptive Frame Rate und Animationen

NovaOS DARF die Aktualisierungsfrequenz grafischer Inhalte energieabhängig begrenzen.

Dabei gilt:

- Interaktive Inhalte SOLLEN reaktionsschnell bleiben.
- Hintergrundanimationen DÜRFEN reduziert werden.
- Vollständig verdeckte Animationen DÜRFEN ausgesetzt werden.
- Zeitabhängige Zustände MÜSSEN beim Fortsetzen korrekt berechnet werden.
- Statische Inhalte benötigen keine regelmäßigen Rendering-Frames.

Eine reduzierte Frame Rate DARF nicht mit einer reduzierten physischen Display-Refresh-Rate gleichgesetzt werden.

## 8. Rendering und Composition Integration

Nova Power-Aware Frame Scheduling SOLL energieeffiziente Rendering- und Kompositionsstrategien berücksichtigen.

Dazu gehören:

- Partial Rendering
- Incremental Composition
- Render Result Caching
- Direct Scanout
- Hardware Overlays
- Adaptive Material Quality
- Zero-Copy Buffer Sharing

Die Auswahl erfolgt durch die jeweils zuständigen Grafikkomponenten.

Der Frame Scheduler DARF deren interne Rendering-Entscheidungen nicht ersetzen.

## 9. Multi-Display Power Management

Jede Bildschirmausgabe MUSS unabhängig energieoptimiert werden können.

Dabei gilt:

- Inaktive Ausgaben erzeugen keine unnötigen Frames.
- Unterschiedliche Bildwiederholraten werden berücksichtigt.
- Gemeinsame Ressourcen werden koordiniert.
- Änderungen einer Ausgabe sollen andere Ausgaben nicht unnötig aktivieren.
- Display-spezifische Energiesparfunktionen können genutzt werden.

Das Abschalten oder Abdunkeln eines Bildschirms bleibt Aufgabe des Display- und Power Managements.

## 10. Adaptive Power Decisions

NovaOS SOLL Energieentscheidungen anhand aktueller Betriebsbedingungen anpassen.

Dabei werden berücksichtigt:

- Energieprofil
- Akku- und Netzbetrieb
- CPU- und GPU-Auslastung
- Rendering-Kosten
- Präsentationslatenz
- Aktualisierungspriorität
- Hardwarefähigkeiten

Häufige unnötige Strategiewechsel SOLLEN durch Hysteresis vermieden werden.

Die Optimierung MUSS ohne zwingende Abhängigkeit von einer KI funktionieren.

## 11. Benutzerkontrolle

NovaOS MUSS Benutzerpräferenzen bei der Energieoptimierung berücksichtigen.

Dabei gilt:

- Energieprofile müssen auswählbar sein.
- Automatische Anpassungen müssen begrenzbar sein.
- Explizite Leistungsanforderungen dürfen nicht unbemerkt überschrieben werden.
- Kritische Systemschutzmaßnahmen bleiben zulässig.

Die Energieoptimierung SOLL möglichst unauffällig erfolgen.

## 12. Fehlerbehandlung

Bei fehlenden Energieinformationen oder unzuverlässigen Messwerten MUSS NovaOS auf eine sichere Scheduling-Strategie zurückfallen.

Dabei gilt:

- Erforderlicher Damage bleibt erhalten.
- Interaktive Aktualisierungen dürfen nicht dauerhaft blockiert werden.
- Ungültige Energieprofile werden zurückgewiesen.
- Fehlgeschlagene Hardware-Energiesparübergänge müssen kontrolliert behandelt werden.
- Grafische Korrektheit besitzt Vorrang vor Energieeinsparungen.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS energieabhängiges Frame Scheduling unterstützen.
2. Frame-Erzeugung MUSS weiterhin auf tatsächlichem Aktualisierungsbedarf beruhen.
3. Energieprofile MÜSSEN berücksichtigt werden können.
4. CPU- und GPU-Auslastung SOLLEN in Scheduling-Entscheidungen einfließen.
5. Nicht dringende Aktualisierungen DÜRFEN zusammengefasst werden.
6. Unnötige GPU-Aktivierungen SOLLEN vermieden werden.
7. Idle Frame Suppression MUSS integriert sein.
8. Hintergrundanimationen SOLLEN energieabhängig begrenzt werden können.
9. Interaktive Aktualisierungen SOLLEN angemessene Reaktionszeiten behalten.
10. Energieoptimierungen DÜRFEN erforderlichen Damage nicht verlieren.
11. Mehrere Bildschirmausgaben MÜSSEN unabhängig berücksichtigt werden.
12. Hardware-Energiesparfunktionen SOLLEN über zuständige Subsysteme nutzbar sein.
13. Benutzerpräferenzen MÜSSEN berücksichtigt werden.
14. Automatische Anpassungen SOLLEN durch Hysteresis stabilisiert werden.
15. Bei fehlenden Messwerten MUSS ein sicherer Fallback verfügbar sein.
16. Energieoptimierung MUSS ohne KI vollständig funktionsfähig sein.
17. Im deterministischen Modus MÜSSEN Scheduling-Entscheidungen anhand definierter Zustands- und Messwerte reproduzierbar sein.

## 14. Ergebnis

Nova Power-Aware Frame Scheduling erweitert die bedarfsgesteuerte Grafikarchitektur um eine energieoptimierte zeitliche Steuerung.

Durch adaptive Frame-Planung, reduzierte Hintergrundaktivität, effiziente Idle-Phasen und die Zusammenarbeit mit dem Power Management werden unnötige CPU- und GPU-Operationen vermieden.

Damit ermöglicht NovaOS eine energieeffiziente Grafikpipeline, die sich an die tatsächlichen Anforderungen des Benutzers und die verfügbaren Systemressourcen anpasst.