# NPSPEC-UI-ACCESS-0005 – Nova Assistive Input Support

**Status:** Angenommen  
**Bereich:** UI / Accessibility / Input  
**System:** NovaOS

## 1. Zweck

Nova Assistive Input Support definiert die Unterstützung alternativer und barrierefreier Eingabemethoden innerhalb von NovaOS.

Ziel ist die vollständige Bedienbarkeit des Systems unabhängig davon, ob klassische Eingabegeräte wie Maus und Tastatur verwendet werden können.

Alternative Eingabemethoden werden in das bestehende Nova Input System integriert und verwenden dieselben UI-, Event- und Command-Schnittstellen wie herkömmliche Eingaben.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Assistive Input Manager | Verwaltung unterstützender Eingabemethoden |
| Input Adapter Interface | Einheitliche Anbindung alternativer Eingabequellen |
| Input Translation Engine | Umsetzung von Eingaben in standardisierte Aktionen |
| Assistive Navigation Controller | Navigation durch die Benutzeroberfläche |
| Input Configuration Manager | Verwaltung individueller Eingabeeinstellungen |
| Assistive Input Feedback | Rückmeldung über erkannte Eingaben und Aktionen |

Die Architektur MUSS unabhängig von konkreten Eingabegeräten und UI-Renderern funktionieren.

## 3. Unterstützte Eingabemethoden

NovaOS sieht folgende Eingabemethoden vor:

- Bildschirmtastatur.
- Einhandbedienung.
- Schaltersteuerung (Switch Control).
- Alternative Zeigegeräte.
- Joystick- und Gamepadnavigation.
- Sprachsteuerung.
- Augensteuerung (Eye Tracking).
- Kopf- und Bewegungssteuerung.
- Angepasste Tastaturbedienung.

Weitere Eingabemethoden MÜSSEN über standardisierte Adapter integrierbar sein.

Die Architektur definiert die gemeinsame Schnittstelle. Konkrete Geräteunterstützung wird durch entsprechende Treiber und Eingabekomponenten bereitgestellt.

## 4. Einheitliches Eingabemodell

Alternative Eingaben werden in standardisierte NovaOS-Eingabeaktionen übersetzt.

Dazu gehören:

- Navigate.
- Focus.
- Activate.
- Select.
- Back.
- Scroll.
- MovePointer.
- Press.
- Release.
- TextInput.

Dabei gilt:

- Eingabeaktionen MÜSSEN unabhängig von ihrer ursprünglichen Quelle verarbeitet werden können.
- Die Übersetzung MUSS deterministisch erfolgen.
- Eingaben MÜSSEN den regulären Fokus- und Ereignisregeln folgen.
- Alternative Eingabemethoden DÜRFEN keine Sicherheitsprüfungen umgehen.

## 5. Tastaturunterstützung

NovaOS unterstützt erweiterte Tastaturfunktionen.

Dazu gehören:

- Sticky Keys.
- Slow Keys.
- Filter Keys.
- Anpassbare Tastenwiederholung.
- Individuelle Tastenzuordnungen.
- Alternative Tastenkombinationen.

Dabei gilt:

- Die Funktionen MÜSSEN unabhängig voneinander konfigurierbar sein.
- Einstellungen MÜSSEN dauerhaft gespeichert werden können.
- Änderungen SOLLEN ohne Neustart wirksam werden.
- Kritische Systemtastenkombinationen MÜSSEN geschützt bleiben.

## 6. Schaltersteuerung

NovaOS unterstützt die Bedienung über einen oder mehrere Schalter.

Dabei gilt:

- UI-Elemente MÜSSEN systematisch durchlaufen werden können.
- Automatisches und manuelles Scanning SOLLEN unterstützt werden.
- Scanintervall und Auswahlverhalten MÜSSEN konfigurierbar sein.
- Der aktuell ausgewählte Bereich MUSS eindeutig erkennbar sein.
- Die Bedienung MUSS ohne präzise Zeigerbewegungen möglich sein.

Die Navigation verwendet den Accessibility Tree und die Fokusverwaltung des Nova UI Frameworks.

## 7. Bildschirmtastatur

NovaOS stellt eine Schnittstelle für native Bildschirmtastaturen bereit.

Dabei gilt:

- Textfelder MÜSSEN über die Bildschirmtastatur bedienbar sein.
- Die Bildschirmtastatur SOLL kontextabhängig eingeblendet werden können.
- Unterschiedliche Tastaturlayouts MÜSSEN unterstützt werden können.
- Eingaben MÜSSEN über die reguläre Texteingabeinfrastruktur erfolgen.
- Geschützte Eingaben MÜSSEN vertraulich behandelt werden.

Die konkrete Bildschirmtastatur ist eine eigenständige Systemkomponente.

## 8. Sprach- und Blicksteuerung

Sprachsteuerung und Eye Tracking können über Assistive Input Adapter angebunden werden.

Dabei gilt:

- Erkannte Eingaben werden in standardisierte Aktionen übersetzt.
- Die Verarbeitung SOLL lokal erfolgen können.
- Die grundlegende Assistive-Input-Architektur DARF keine KI voraussetzen.
- Unsichere oder mehrdeutige Eingaben DÜRFEN nicht ungeprüft kritische Aktionen auslösen.
- Benutzer MÜSSEN die jeweilige Eingabemethode deaktivieren können.

Die Erkennung von Sprache oder Blickbewegungen ist nicht Bestandteil dieser Spezifikation.

## 9. Fokus und Navigation

Assistive Input Support verwendet die zentrale Fokusverwaltung von NovaOS.

Dabei gilt:

- Interaktive Elemente MÜSSEN über alternative Eingaben erreichbar sein.
- Die Navigationsreihenfolge MUSS nachvollziehbar sein.
- Der aktuelle Fokus MUSS visuell oder über unterstützende Technologien erkennbar bleiben.
- Modale Dialoge MÜSSEN vollständig bedienbar sein.
- Custom Controls MÜSSEN standardisierte Navigations- und Aktionsschnittstellen bereitstellen.

## 10. Benutzerprofile

NovaOS unterstützt individuelle Eingabekonfigurationen.

Dazu gehören:

- Aktivierte Eingabemethoden.
- Tastenbelegungen.
- Eingabeverzögerungen.
- Wiederholungsintervalle.
- Scanning-Geschwindigkeit.
- Navigationsverhalten.
- Rückmeldeoptionen.

Dabei gilt:

- Einstellungen MÜSSEN benutzerbezogen gespeichert werden können.
- Profile SOLLEN exportierbar und übertragbar sein.
- Änderungen MÜSSEN rückgängig gemacht werden können.
- Eine funktionsfähige alternative Eingabemethode MUSS während der Konfiguration erreichbar bleiben.

## 11. Ereignisse und Commands

Assistive Input Support integriert sich in das Nova UI Event System.

Dabei gilt:

- Alternative Eingaben erzeugen standardisierte UI-Ereignisse.
- Routed Events MÜSSEN unterstützt werden.
- Commands und Actions MÜSSEN unabhängig von der Eingabequelle ausführbar sein.
- Event Handler DÜRFEN keine bestimmte physische Eingabequelle voraussetzen, sofern dies funktional nicht erforderlich ist.
- Die Herkunft einer Eingabe MUSS bei sicherheitsrelevanten Aktionen nachvollziehbar bleiben.

## 12. Sicherheit und Datenschutz

Nova Assistive Input Support unterliegt Nova.Trust.

Dabei gilt:

- Eingabeadapter MÜSSEN innerhalb ihrer autorisierten Capabilities arbeiten.
- Alternative Eingabemethoden DÜRFEN keine privilegierten Aktionen ohne Berechtigung ausführen.
- Sprach- und Blickdaten MÜSSEN vor unautorisiertem Zugriff geschützt werden.
- Anwendungen DÜRFEN systemweite Eingabeeinstellungen nicht eigenmächtig verändern.
- Die grundlegende Bedienbarkeit MUSS vollständig offline funktionieren.

## 13. Performance

Assistive Input Support arbeitet ereignisgesteuert.

Dabei gilt:

- Inaktive Eingabemethoden DÜRFEN keine unnötige Rechenzeit beanspruchen.
- Eingaben MÜSSEN mit möglichst geringer Latenz verarbeitet werden.
- Langsame Eingabeadapter DÜRFEN die UI Event Loop nicht dauerhaft blockieren.
- Eingabeereignisse MÜSSEN in definierter Reihenfolge verarbeitet werden.
- Ressourcen MÜSSEN beim Deaktivieren einer Eingabemethode freigegeben werden.

## 14. NovaStudio

NovaStudio unterstützt:

- Prüfung alternativer Bedienwege.
- Simulation von Tastatur- und Schaltersteuerung.
- Analyse der Fokusreihenfolge.
- Prüfung der Command-Erreichbarkeit.
- Diagnose von Assistive-Input-Ereignissen.
- Erkennung nicht erreichbarer Controls.

## 15. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine einheitliche Architektur für alternative Eingabemethoden bereitstellen.
- **UI-002:** Eingabemethoden MÜSSEN über standardisierte Adapter integrierbar sein.
- **UI-003:** Alternative Eingaben MÜSSEN in standardisierte UI-Aktionen übersetzt werden können.
- **UI-004:** Die Verarbeitung MUSS deterministisch erfolgen.
- **UI-005:** Erweiterte Tastaturfunktionen MÜSSEN unterstützt werden.
- **UI-006:** Schaltersteuerung MUSS architektonisch unterstützt werden.
- **UI-007:** Bildschirmtastaturen MÜSSEN integrierbar sein.
- **UI-008:** Sprach- und Blicksteuerung MÜSSEN über Adapter integrierbar sein.
- **UI-009:** Alternative Eingaben MÜSSEN die zentrale Fokusverwaltung verwenden.
- **UI-010:** Standard Controls MÜSSEN über alternative Eingaben bedienbar sein.
- **UI-011:** Custom Controls MÜSSEN alternative Bedienwege ermöglichen.
- **UI-012:** Benutzerdefinierte Eingabeprofile MÜSSEN unterstützt werden.
- **UI-013:** Eingabeeinstellungen SOLLEN ohne Neustart änderbar sein.
- **UI-014:** Alternative Eingaben MÜSSEN reguläre Events und Commands verwenden.
- **UI-015:** Eingabeadapter DÜRFEN die UI Event Loop nicht dauerhaft blockieren.
- **UI-016:** Sämtliche Eingaben MÜSSEN Nova.Trust berücksichtigen.
- **UI-017:** NovaStudio MUSS alternative Bedienwege prüfen können.
- **UI-018:** Die Architektur MUSS unabhängig vom Renderbackend funktionieren.
- **UI-019:** Die grundlegende Assistive-Input-Unterstützung MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova Accessibility Architecture:** Übergeordnete Barrierefreiheit.
- **Nova Accessibility Tree:** Semantische Navigation.
- **Nova Screen Reader Interface:** Sprachliche und taktile Rückmeldungen.
- **Nova Input System:** Eingabeverarbeitung.
- **Nova UI Event System:** Standardisierte Eingabeereignisse.
- **Nova Routed Events:** Ereignisweiterleitung.
- **Nova Commands and Actions:** Ausführung von Bedienaktionen.
- **Nova Control Framework:** Bedienbarkeit nativer Controls.
- **Nova UI Focus Management:** Fokussteuerung.
- **Nova UI Threading Model:** Synchronisierte Eingabeverarbeitung.
- **NovaStudio:** Prüfung und Diagnose.
- **Nova.Trust:** Berechtigungen und Isolation.

## 17. Ergebnis

NovaOS erhält eine einheitliche Architektur zur Integration alternativer Eingabemethoden.

Benutzer können das Betriebssystem über Tastaturhilfen, Schaltersteuerung, Bildschirmtastaturen und weitere unterstützende Technologien bedienen.

Durch die Integration in das bestehende Input-, Event- und Command-System benötigen Anwendungen keine getrennte Bedienlogik für jede Eingabemethode.

**Grundsatz:** NovaOS richtet sich nach den Eingabemöglichkeiten des Menschen – nicht der Mensch nach den Eingabegeräten des Systems.