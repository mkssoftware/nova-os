# NPSPEC-INPUT-DATA-0002 – Nova Drag and Drop

**Status:** Angenommen  
**Bereich:** Input / Data / Drag and Drop  
**System:** NovaOS

## 1. Zweck

Nova Drag and Drop definiert die systemweite Übertragung von Daten durch Ziehen und Ablegen innerhalb von NovaOS.

Ziel ist ein sicherer, effizienter und formatunabhängiger Datenaustausch zwischen Fenstern, Anwendungen, Capabilities und Benutzeroberflächenelementen.

Drag and Drop verwendet das gemeinsame Nova-Datenmodell und unterstützt strukturierte Datenübertragungen ohne unnötige Kopien oder Konvertierungen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Drag and Drop Manager | Zentrale Verwaltung von Drag-and-Drop-Vorgängen |
| Drag Session Controller | Verwaltung aktiver Übertragungssitzungen |
| Drag Target Resolver | Ermittlung gültiger Ablageziele |
| Data Transfer Controller | Steuerung der Datenübertragung |
| Drop Operation Negotiator | Aushandlung der Übertragungsoperation |
| Drag Feedback Provider | Bereitstellung visueller Rückmeldungen |

Die Architektur verwendet Nova Pointer Events, Nova Input Capture und Nova Event Routing.

Die Datenübertragung erfolgt über die gemeinsame Datenübertragungsinfrastruktur von NovaOS.

## 3. Drag-and-Drop-Modell

Ein Drag-and-Drop-Vorgang besitzt mindestens:

- Eindeutige Drag-Session-ID.
- Datenquelle.
- Aktuelles Ablageziel.
- Input-Seat-Kennung.
- Auslösenden Pointer.
- Verfügbare Datenformate.
- Unterstützte Übertragungsoperationen.
- Aktuellen Zustand.
- Berechtigungsinformationen.

Die Datenquelle stellt die verfügbaren Datenformate bereit.

Das Ablageziel entscheidet, welche Formate und Operationen es unterstützt.

## 4. Unterstützte Operationen

NovaOS unterstützt mindestens folgende Operationen:

| Operation | Beschreibung |
|---|---|
| Copy | Daten werden kopiert |
| Move | Daten werden verschoben |
| Link | Eine Referenz auf die Daten wird erstellt |
| None | Keine Übertragung möglich |

Die tatsächlich ausgeführte Operation wird zwischen Quelle und Ziel ausgehandelt.

Dabei gilt:

- Quelle und Ziel MÜSSEN die Operation unterstützen.
- Benutzeraktionen können die gewünschte Operation beeinflussen.
- Sicherheitsrichtlinien besitzen Vorrang.
- Nicht unterstützte Operationen werden zurückgewiesen.

Ein `Move` DARF Quelldaten erst nach bestätigter erfolgreicher Übernahme durch das Ziel entfernen.

## 5. Lebenszyklus

Ein Drag-and-Drop-Vorgang durchläuft folgende Zustände:

1. **Idle:** Kein aktiver Vorgang.
2. **Started:** Ziehvorgang wurde gestartet.
3. **Dragging:** Pointer bewegt sich mit aktiver Drag-Session.
4. **Negotiating:** Ablageziel und Operation werden geprüft.
5. **Dropping:** Datenübertragung wurde angefordert.
6. **Completed:** Übertragung wurde erfolgreich abgeschlossen.
7. **Cancelled:** Vorgang wurde abgebrochen.
8. **Failed:** Übertragung ist fehlgeschlagen.

Zustandsübergänge MÜSSEN eindeutig definiert sein.

Jede Drag-Session MUSS kontrolliert abgeschlossen oder abgebrochen werden können.

## 6. Drag-and-Drop-Ereignisse

NovaOS stellt mindestens folgende Ereignisse bereit:

| Ereignis | Beschreibung |
|---|---|
| `DragStarted` | Ziehvorgang beginnt |
| `DragEnter` | Pointer betritt ein mögliches Ablageziel |
| `DragOver` | Pointer bewegt sich innerhalb des Ablageziels |
| `DragLeave` | Pointer verlässt das Ablageziel |
| `DropRequested` | Ablage wird angefordert |
| `DropCompleted` | Datenübertragung wurde abgeschlossen |
| `DragCancelled` | Vorgang wurde abgebrochen |
| `DragFailed` | Vorgang ist fehlgeschlagen |

Ereignisse enthalten die Drag-Session-ID, den aktuellen Eingabekontext und die erforderlichen Metadaten.

Die Ereigniszustellung erfolgt über Nova Event Dispatcher.

## 7. Zielermittlung

Das Ablageziel wird über Nova Event Routing und Hit Testing ermittelt.

Dabei werden berücksichtigt:

- Fensterposition und Z-Reihenfolge.
- Sichtbarkeit.
- Clipping.
- Geometrische Transformationen.
- Berechtigte Eingabebereiche.
- Unterstützte Datenformate.
- Zulässige Übertragungsoperationen.

Ein Ziel MUSS eine Ablage ausdrücklich akzeptieren.

Das bloße Überfahren eines Elements begründet keine Berechtigung zur Datenübertragung.

## 8. Datenformate und Übertragung

Nova Drag and Drop verwendet dieselben grundlegenden Datenformate und Transfermechanismen wie Nova Clipboard Management.

Unterstützt werden:

- Unicode-Text.
- Formatierter Text.
- Bilder.
- Datei- und Verzeichnisreferenzen.
- Strukturierte Daten.
- Binäre Daten.
- Anwendungsspezifische Formate.

Ein Vorgang DARF mehrere Repräsentationen derselben Daten anbieten.

Die Übertragung unterstützt:

- Direkte Datenübergabe.
- Referenzbasierte Übertragung.
- Verzögerte Datenbereitstellung.
- Zero-Copy, sofern technisch und sicherheitstechnisch möglich.

Daten werden erst übertragen, wenn ein berechtigtes Ziel sie tatsächlich anfordert.

## 9. Transaktionale Übertragung

Drag-and-Drop-Operationen SOLLEN transaktional ausgeführt werden.

Insbesondere bei `Move` gilt:

1. Ziel akzeptiert Datenformat und Operation.
2. Daten werden übertragen.
3. Ziel bestätigt die erfolgreiche Übernahme.
4. Quelle führt die vereinbarte Freigabe beziehungsweise Entfernung aus.
5. Vorgang wird abgeschlossen.

Fehler während der Übertragung DÜRFEN nicht zu einem unbeabsichtigten Verlust der Quelldaten führen.

Bei nicht atomar durchführbaren Operationen MUSS der tatsächliche Abschlusszustand eindeutig erkennbar sein.

## 10. Visuelle Rückmeldung

NovaOS stellt Informationen zur Darstellung des Drag-and-Drop-Zustands bereit.

Dazu gehören:

- Drag-Vorschau.
- Aktuelles Ablageziel.
- Zulässige Operation.
- Nicht zulässige Ablage.
- Übertragungsfortschritt bei Bedarf.

Die Darstellung erfolgt durch Nova UI, Window Manager beziehungsweise Compositor.

Die Drag-and-Drop-Logik bleibt unabhängig vom Renderer.

Visuelle Rückmeldungen SOLLEN ausschließlich bei Zustandsänderungen aktualisiert werden.

## 11. Abbruch und Fehlerbehandlung

Eine Drag-Session wird kontrolliert beendet bei:

- Benutzerabbruch.
- Geräteverlust.
- Verlust des auslösenden Pointers.
- Beendigung der Quelle.
- Zerstörung des Ablageziels.
- Sitzungswechsel.
- Entzug erforderlicher Berechtigungen.
- Nicht behebbaren Übertragungsfehlern.

Dabei MÜSSEN aktive Capture-Bindungen und temporäre Ressourcen freigegeben werden.

Fehler einzelner Anwendungen DÜRFEN die zentrale Eingabeverarbeitung nicht blockieren.

## 12. Sicherheit

Nova Drag and Drop unterliegt Nova.Trust.

Festlegungen:

- Datenübertragungen MÜSSEN autorisiert werden.
- Ablageziele erhalten ausschließlich freigegebene Daten.
- Datenreferenzen DÜRFEN keine Dateisystemberechtigungen umgehen.
- Sitzungsübergreifende Übertragungen erfordern eine ausdrückliche Berechtigung.
- Geschützte Inhalte können Drag and Drop untersagen.
- Anwendungen DÜRFEN keine fremden Drag-Sessions übernehmen.
- Drag and Drop DARF keine automatische Ausführung übertragener Inhalte auslösen.

Die Annahme einer Drag-Session verleiht keine zusätzlichen Systemberechtigungen.

## 13. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Drag-and-Drop-Verwaltung bereitstellen.
- **INPUT-002:** Drag-and-Drop-Vorgänge MÜSSEN eindeutig identifizierbare Sitzungen besitzen.
- **INPUT-003:** Copy, Move und Link MÜSSEN als Operationen unterstützt werden.
- **INPUT-004:** Quelle und Ziel MÜSSEN Datenformate und Operationen aushandeln können.
- **INPUT-005:** Ablageziele MÜSSEN eine Übertragung ausdrücklich akzeptieren.
- **INPUT-006:** Drag-and-Drop-Ereignisse MÜSSEN über Nova Event Dispatcher bereitgestellt werden.
- **INPUT-007:** Die Zielermittlung MUSS Nova Event Routing verwenden.
- **INPUT-008:** Aktive Ziehvorgänge MÜSSEN mit Nova Input Capture koordiniert werden.
- **INPUT-009:** Mehrere Datenformate pro Drag-Session MÜSSEN unterstützt werden.
- **INPUT-010:** Große Datenmengen MÜSSEN ohne zwingende vollständige Datenkopie übertragen werden können.
- **INPUT-011:** Move-Operationen DÜRFEN Quelldaten nicht vor bestätigter Übernahme entfernen.
- **INPUT-012:** Drag-Sessions MÜSSEN kontrolliert abgebrochen werden können.
- **INPUT-013:** Bei Geräte- oder Anwendungsverlust MÜSSEN betroffene Ressourcen freigegeben werden.
- **INPUT-014:** Datenübertragungen MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-015:** Drag and Drop MUSS unabhängig vom verwendeten UI-Renderer funktionieren.
- **INPUT-016:** Zustandsübergänge und Konfliktentscheidungen MÜSSEN deterministisch erfolgen.
- **INPUT-017:** Nova Drag and Drop MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Pointer Events:** Zeigerbewegungen und Kontaktzustände.
- **Nova Input Capture:** Bindung laufender Ziehvorgänge.
- **Nova Event Routing:** Ermittlung des Ablageziels.
- **Nova Event Dispatcher:** Zustellung der Drag-and-Drop-Ereignisse.
- **Nova Clipboard Management:** Gemeinsame Datenformate und Transfermechanismen.
- **Nova Window Manager:** Fensterübergreifende Zielermittlung.
- **Nova Compositor:** Darstellung von Drag-Vorschauen.
- **Nova UI:** Drag-and-Drop-Interaktionen.
- **Nova IPC:** Prozessübergreifende Datenübertragung.
- **Nova VFS:** Datei- und Verzeichnisoperationen.
- **Nova Capability System:** Semantischer Datenaustausch.
- **Nova.Trust:** Berechtigungen und Isolation.

## 15. Ergebnis

NovaOS erhält eine einheitliche Architektur für Drag and Drop zwischen Fenstern, Anwendungen und Capabilities.

Daten können in unterschiedlichen Formaten sicher und ressourcenschonend übertragen werden. Transaktionale Abläufe verhindern insbesondere bei Verschiebeoperationen unbeabsichtigte Datenverluste.

**Grundsatz:** Drag and Drop überträgt nicht nur sichtbare Objekte, sondern deren strukturierte Daten und Bedeutung – sicher, effizient und ohne unnötige Konvertierungen.