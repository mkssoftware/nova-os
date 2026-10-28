# NPSPEC-INPUT-DEVICE-0002 – Nova Mouse Input

**Status:** Angenommen  
**Bereich:** Input / Device / Mouse  
**System:** NovaOS

## 1. Zweck

Nova Mouse Input definiert die hardwareunabhängige Erfassung und Verarbeitung von Mauseingaben innerhalb von NovaOS.

Ziel ist eine präzise, latenzarme und zuverlässige Steuerung des Mauszeigers sowie die einheitliche Verarbeitung von Maustasten, Scrollbewegungen und erweiterten Mausfunktionen.

Das Subsystem funktioniert unabhängig von Nova UI und unterstützt sowohl grafische Oberflächen als auch andere Eingabekontexte.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Mouse Driver Adapter | Anbindung hardwareabhängiger Maustreiber |
| Mouse Device Manager | Verwaltung angeschlossener Mäuse |
| Mouse Event Processor | Normalisierung von Mausereignissen |
| Mouse State Manager | Verwaltung von Tasten und Bewegungszuständen |
| Pointer Motion Controller | Verarbeitung von Zeigerbewegungen |
| Mouse Configuration Manager | Verwaltung von Empfindlichkeit und Tastenbelegung |

Die Komponenten integrieren sich in die Nova Input Device Abstraction und Nova Input Event Pipeline.

Die Hardwaretreiber liefern Bewegungsdaten und Tastenzustände. Die Interpretation erfolgt durch die übergeordneten Input-Komponenten.

## 3. Mausmodell

NovaOS unterscheidet:

### Physical Mouse Input

Hardwarebezogene Eingaben:

- Relative Bewegungen auf X- und Y-Achse.
- Maustasten.
- Vertikale und horizontale Scrollbewegungen.
- Zusätzliche gerätespezifische Funktionen.

### Logical Mouse Input

Normalisierte Eingaben:

- Zeigerbewegung.
- Primäre und sekundäre Maustaste.
- Mittlere Maustaste.
- Zusatztasten.
- Scrollereignisse.

### Semantic Mouse Input

Kontextabhängige Aktionen:

- Auswählen.
- Aktivieren.
- Kontextmenü öffnen.
- Ziehen und Ablegen.
- Scrollen.
- Navigation.

Die Interpretation semantischer Aktionen erfolgt durch Nova UI beziehungsweise die zuständige Anwendung.

## 4. Mausereignisse

NovaOS unterstützt mindestens:

- `MouseMove`
- `MouseButtonDown`
- `MouseButtonUp`
- `MouseScroll`
- `MouseConnected`
- `MouseDisconnected`

Ein Mausereignis enthält:

- Gerätekennung.
- Ereignistyp.
- Monotonen Zeitstempel.
- Bewegungsdaten beziehungsweise Tastenkennung.
- Relevante Modifier-Zustände.
- Kennzeichnung der Eingabequelle.

Mausereignisse werden über die Nova Input Event Pipeline verarbeitet.

Die logische Reihenfolge zusammengehöriger Ereignisse MUSS erhalten bleiben.

## 5. Zeigerbewegung

Der Pointer Motion Controller verarbeitet relative Mausbewegungen.

Dabei werden berücksichtigt:

- Bewegungsempfindlichkeit.
- Zeigerbeschleunigung.
- Bewegungsauflösung.
- Display-Skalierung.
- Display-Anordnung.
- Zeigerbegrenzungen.

Die ursprünglichen relativen Bewegungsdaten MÜSSEN für berechtigte Anwendungen unverändert verfügbar gemacht werden können.

Zeigerbeschleunigung und Empfindlichkeit werden unabhängig vom Hardwaretreiber konfiguriert.

Die Umrechnung in Bildschirmkoordinaten erfolgt durch das zuständige Pointer- beziehungsweise Display-System.

## 6. Maustasten

NovaOS unterstützt mindestens:

- Primäre Maustaste.
- Sekundäre Maustaste.
- Mittlere Maustaste.
- Zusätzliche Maustasten.

Maustasten besitzen geräteunabhängige logische Kennungen.

Die Zuordnung physischer Tasten zu logischen Funktionen MUSS konfigurierbar sein.

Links- und Rechtshänderprofile werden unterstützt.

Mehrere gleichzeitig gedrückte Maustasten MÜSSEN korrekt verarbeitet werden.

## 7. Scrollverarbeitung

NovaOS unterstützt:

- Vertikales Scrollen.
- Horizontales Scrollen.
- Hochauflösende Scrollbewegungen.
- Klassische Scrollräder mit diskreten Schritten.

Scrollereignisse MÜSSEN ihre Bewegungsrichtung und ihren Wert enthalten.

Hochauflösende Scrollinformationen SOLLEN erhalten bleiben.

Die Umrechnung in konkrete Scrollbewegungen erfolgt durch den jeweiligen Eingabekontext.

## 8. Mehrere Mäuse

NovaOS unterstützt mehrere gleichzeitig angeschlossene Mäuse.

Dabei gilt:

- Jede Maus besitzt eine eigene Gerätekennung.
- Tasten- und Bewegungszustände werden gerätebezogen verwaltet.
- Mehrere Mäuse können einen gemeinsamen Systemzeiger steuern.
- Die Zuordnung zu unterschiedlichen Eingabekontexten MUSS möglich sein.
- Geräte können unabhängig hinzugefügt und entfernt werden.

Ein Mehrzeigerbetrieb DARF als optionale Erweiterung implementiert werden.

## 9. Pointer Capture und Raw Input

NovaOS unterstützt kontrolliertes Pointer Capture.

Damit können berechtigte Anwendungen Mausbewegungen innerhalb eines definierten Eingabekontexts empfangen.

Typische Anwendungsfälle sind:

- Drag-and-Drop.
- Zeichenprogramme.
- CAD-Anwendungen.
- Spiele.
- 3D-Kamerasteuerung.

Zusätzlich wird Raw Mouse Input unterstützt.

Raw Input stellt relative Bewegungsdaten ohne Zeigerbeschleunigung bereit.

Pointer Capture und Raw Input unterliegen den Berechtigungs- und Fokusregeln von Nova Input Security.

## 10. Performance und Fehlerbehandlung

Die Mausverarbeitung erfolgt ereignisgesteuert und mit geringer Latenz.

Festlegungen:

- Mausbewegungen werden zeitnah verarbeitet.
- Aufeinanderfolgende Bewegungsereignisse DÜRFEN zusammengefasst werden.
- Maustastenereignisse DÜRFEN dabei nicht verloren gehen.
- Die Eingabeverarbeitung bleibt unabhängig von der Rendering-Frequenz.
- Bei Geräteverlust werden aktive Tastenzustände bereinigt.
- Ungültige Bewegungsdaten werden kontrolliert behandelt.

Die Zeigerdarstellung SOLL unabhängig von vollständigen Desktop-Neuberechnungen aktualisiert werden können.

Hardware-Cursor SOLLEN verwendet werden, sofern das Grafikbackend diese unterstützt.

## 11. Sicherheit

Nova Mouse Input unterliegt Nova Input Security und Nova.Trust.

Dabei gilt:

- Mausereignisse werden ausschließlich an berechtigte Empfänger weitergeleitet.
- Pointer Capture MUSS kontrolliert und widerrufbar sein.
- Raw Input erfordert die entsprechenden Zugriffsrechte.
- Virtuelle Mausereignisse MÜSSEN als solche identifizierbar sein.
- Nicht autorisierte Eingabeinjektionen werden zurückgewiesen.
- Anwendungen DÜRFEN den Systemzeiger nicht ohne Berechtigung dauerhaft blockieren.

Sicherheitskritische Systeminteraktionen besitzen Vorrang vor anwendungsspezifischem Pointer Capture.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine hardwareunabhängige Mausverarbeitung bereitstellen.
- **INPUT-002:** Relative Mausbewegungen MÜSSEN unterstützt werden.
- **INPUT-003:** Maustasten und Scrollbewegungen MÜSSEN als eigenständige Ereignisse verarbeitet werden.
- **INPUT-004:** Jede Maus MUSS eine eindeutige Gerätekennung besitzen.
- **INPUT-005:** Mehrere Mäuse MÜSSEN gleichzeitig unterstützt werden.
- **INPUT-006:** Maustastenzustände MÜSSEN gerätebezogen verwaltet werden.
- **INPUT-007:** Mausgeschwindigkeit und Zeigerbeschleunigung MÜSSEN konfigurierbar sein.
- **INPUT-008:** Die Zuordnung physischer Maustasten zu logischen Funktionen MUSS konfigurierbar sein.
- **INPUT-009:** Vertikales und horizontales Scrollen MÜSSEN unterstützt werden.
- **INPUT-010:** Hochauflösende Scrollinformationen SOLLEN erhalten bleiben.
- **INPUT-011:** Raw Mouse Input MUSS für berechtigte Anwendungen verfügbar sein.
- **INPUT-012:** Pointer Capture MUSS kontrolliert und widerrufbar sein.
- **INPUT-013:** Bei Geräteverlust MÜSSEN aktive Maustastenzustände bereinigt werden.
- **INPUT-014:** Mausereignisse MÜSSEN über die Nova Input Event Pipeline verarbeitet werden.
- **INPUT-015:** Die Mausverarbeitung DARF nicht von der Rendering-Frequenz abhängig sein.
- **INPUT-016:** Die grundlegende Mausverarbeitung MUSS ohne Nova UI funktionieren.
- **INPUT-017:** Virtuelle Mausereignisse MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-018:** Nova Mouse Input MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Input Device Abstraction:** Hardwareunabhängiges Mausmodell.
- **Nova Input Event Pipeline:** Verarbeitung und Zustellung der Mausereignisse.
- **Nova Input Security:** Berechtigungen und Eingabeisolation.
- **Nova Input Mapping Engine:** Maustastenbelegung und Eingabeprofile.
- **Nova USB/HID:** USB-Mäuse und kompatible Eingabegeräte.
- **Nova PS/2:** Unterstützung entsprechender Legacy-Mäuse.
- **Nova Window Manager:** Pointer-Fokus und Eingabeziele.
- **Nova Display System:** Zeigerkoordinaten und Display-Anordnung.
- **Nova Compositor:** Darstellung und Aktualisierung des Mauszeigers.
- **Nova UI:** Mausinteraktionen und Steuerelemente.

## 14. Ergebnis

NovaOS erhält eine einheitliche Mausarchitektur für klassische und moderne Zeigegeräte.

Die Trennung von Hardwareerfassung, Bewegungsverarbeitung, Tastenverwaltung und semantischer Interpretation ermöglicht eine präzise und flexible Mausbedienung.

Durch Raw Input, konfigurierbare Zeigerbewegungen und kontrolliertes Pointer Capture werden sowohl alltägliche Desktopanwendungen als auch professionelle Grafikprogramme und Spiele unterstützt.

**Grundsatz:** Mausbewegungen und Maustasten werden unabhängig von der Hardware präzise erfasst, mit möglichst geringer Latenz verarbeitet und ausschließlich dort wirksam, wo sie benötigt werden.