# NPSPEC-UI-DESIGNER-0003 – Nova Live Preview

**Status:** Angenommen  
**Bereich:** UI / Designer / Live Preview  
**System:** NovaOS

## 1. Zweck

Nova Live Preview definiert die unmittelbare visuelle Vorschau nativer NovaOS-Benutzeroberflächen während ihrer Entwicklung in NovaStudio.

Änderungen an `.nui`-Dateien, Eigenschaften, Layouts, Styles und Bindings sollen ohne erneutes Kompilieren oder Starten der gesamten Anwendung sichtbar werden.

Die Vorschau verwendet die native Nova UI Runtime und denselben Renderer wie die Zielanwendung.

## 2. Architektur

Nova Live Preview besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Live Preview Manager | Verwaltung der Vorschau |
| Preview Runtime | Ausführung der UI im Vorschaukontext |
| Document Change Observer | Erkennung von Änderungen |
| UI Update Engine | Inkrementelle Aktualisierung der UI |
| Preview State Manager | Verwaltung des Vorschauzustands |
| Preview Data Provider | Bereitstellung von Testdaten |
| Preview Diagnostics | Fehleranalyse und Statusmeldungen |

Die Vorschau MUSS unabhängig von der eigentlichen Anwendungslogik betrieben werden können.

## 3. Live-Aktualisierung

Änderungen am UI-Dokument werden automatisch in die Vorschau übernommen.

Unterstützt werden:

- Hinzufügen und Entfernen von Controls.
- Änderungen von Eigenschaften.
- Layoutänderungen.
- Styles und Templates.
- Resource Dictionaries.
- Data Bindings.
- Animationen und Transitions.
- Accessibility-Eigenschaften.

Dabei gilt:

- Änderungen SOLLEN inkrementell übernommen werden.
- Unveränderte UI-Elemente SOLLEN erhalten bleiben.
- Eine vollständige Neuerstellung erfolgt nur bei technischer Notwendigkeit.
- Mehrere unmittelbar aufeinanderfolgende Änderungen SOLLEN zusammengefasst werden.

## 4. Gemeinsame Runtime

Die Live Preview verwendet die Nova UI Runtime.

Dabei gilt:

- Layoutberechnung MUSS über die Nova Layout Engine erfolgen.
- Styles und Ressourcen MÜSSEN identisch zur Zielanwendung aufgelöst werden.
- Controls MÜSSEN ihre regulären Templates verwenden.
- Data Bindings MÜSSEN über das Nova Data Binding System ausgewertet werden.
- Die Vorschau DARF keine abweichende UI-Implementierung verwenden.

Design-Time-spezifische Funktionen werden über einen gesonderten Vorschaukontext bereitgestellt.

## 5. Vorschauzustand

Die Live Preview verwaltet einen eigenständigen UI-Zustand.

Dabei gilt:

- Fokus, Auswahl und Scrollposition SOLLEN bei Aktualisierungen erhalten bleiben.
- Unveränderte Controls SOLLEN ihre gültigen Zustände behalten.
- Entfernte Elemente MÜSSEN kontrolliert freigegeben werden.
- Änderungen an Bindings MÜSSEN konsistent übernommen werden.
- Die Vorschau MUSS jederzeit zurückgesetzt werden können.

## 6. Design-Time-Daten

Die Vorschau unterstützt simulierte Daten und Zustände.

Dazu gehören:

- Beispieltexte und Datensätze.
- Testwerte für Data Bindings.
- Unterschiedliche Control States.
- Lade- und Fehlerzustände.
- Leere und umfangreiche Datenmengen.

Design-Time-Daten MÜSSEN von produktiven Anwendungsdaten getrennt bleiben.

Die Vorschau DARF keine produktiven Daten verändern, sofern dies nicht ausdrücklich in einem autorisierten Testkontext vorgesehen ist.

## 7. Interaktive Vorschau

NovaStudio unterscheidet zwischen zwei Betriebsarten:

**Design Mode**
- Auswahl und Bearbeitung von UI-Elementen.
- Darstellung von Layoutgrenzen und Bearbeitungshilfen.
- Keine unbeabsichtigte Ausführung von Anwendungsaktionen.

**Interaction Mode**
- Testen von Controls und Navigation.
- Auslösen von Events und Commands innerhalb des Vorschaukontexts.
- Prüfung von Fokus, Bindings und Visual States.

Der Wechsel zwischen beiden Betriebsarten MUSS ohne Neustart der Vorschau möglich sein.

## 8. Geräte- und Darstellungssimulation

Nova Live Preview unterstützt unterschiedliche Darstellungsbedingungen.

Dazu gehören:

- Fenstergrößen.
- Bildschirmauflösungen.
- DPI- und UI-Skalierung.
- Light und Dark Theme.
- High Contrast.
- Reduced Motion.
- Unterschiedliche Schriftgrößen.

Die Vorschau MUSS adaptive Layouts unter diesen Bedingungen prüfen können.

## 9. Fehlerbehandlung

Fehlerhafte Änderungen DÜRFEN die gesamte Vorschau nicht unkontrolliert beenden.

Dabei gilt:

- Ungültige `.nui`-Änderungen MÜSSEN erkannt werden.
- Der letzte gültige Vorschauzustand SOLL erhalten bleiben.
- Fehler MÜSSEN mit Bezug zur betroffenen Datei und Position gemeldet werden.
- Fehlerhafte Controls MÜSSEN kontrolliert isoliert werden können.
- Nach einer Korrektur MUSS die Vorschau automatisch fortgesetzt werden können.

## 10. Rendering und Performance

Die Live Preview arbeitet bedarfsgesteuert.

Dabei gilt:

- Nur veränderte UI-Bereiche werden invalidiert.
- Damage Tracking MUSS verwendet werden.
- Unveränderte Layoutbereiche SOLLEN nicht erneut berechnet werden.
- Ohne Änderungen oder aktive Animationen DÜRFEN keine unnötigen Frames erzeugt werden.
- Aufwendige Dokumentanalysen DÜRFEN die NovaStudio-Oberfläche nicht dauerhaft blockieren.

## 11. Sicherheit

Nova Live Preview unterliegt Nova.Trust.

Dabei gilt:

- Vorschaukomponenten MÜSSEN in einem kontrollierten Sicherheitskontext ausgeführt werden.
- Custom Controls DÜRFEN keine zusätzlichen Capabilities erhalten.
- Datei-, Netzwerk- und Systemzugriffe MÜSSEN autorisiert werden.
- Fehlerhafte Komponenten DÜRFEN NovaStudio nicht unkontrolliert beeinträchtigen.
- Die Vorschau MUSS vollständig offline funktionieren.

## 12. Normative Anforderungen

- **UI-001:** NovaStudio MUSS eine native Live Preview bereitstellen.
- **UI-002:** Änderungen an `.nui`-Dokumenten MÜSSEN automatisch erkannt werden.
- **UI-003:** Die Vorschau MUSS dieselbe Nova UI Runtime wie die Zielanwendung verwenden.
- **UI-004:** Änderungen SOLLEN inkrementell übernommen werden.
- **UI-005:** Unveränderte UI-Zustände SOLLEN erhalten bleiben.
- **UI-006:** Design-Time-Daten MÜSSEN unterstützt werden.
- **UI-007:** Design Mode und Interaction Mode MÜSSEN getrennt sein.
- **UI-008:** Unterschiedliche Fenstergrößen, Skalierungen und Themes MÜSSEN simulierbar sein.
- **UI-009:** Fehlerhafte Dokumentänderungen DÜRFEN den letzten gültigen Vorschauzustand nicht zerstören.
- **UI-010:** Fehler MÜSSEN nachvollziehbar diagnostiziert werden.
- **UI-011:** Die Vorschau MUSS Damage Tracking und bedarfsgesteuertes Rendering verwenden.
- **UI-012:** Die Vorschau MUSS unabhängig von einer vollständigen Anwendungsausführung funktionieren.
- **UI-013:** Vorschaukomponenten MÜSSEN Nova.Trust berücksichtigen.
- **UI-014:** Nova Live Preview MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Visual UI Designer:** Visuelle Bearbeitung.
- **Nova UI Serialization:** Laden und Aktualisieren von `.nui`-Dokumenten.
- **Nova Declarative UI Model:** Gemeinsames Dokumentmodell.
- **Nova UI Runtime:** Ausführung der Vorschau.
- **Nova Layout Engine:** Layoutberechnung.
- **Nova Styling System:** Styles und Templates.
- **Nova Data Binding:** Testdaten und Bindings.
- **Nova Animation Engine:** Animationen und Transitions.
- **Nova UI Renderer Abstraction:** Native Darstellung.
- **Nova Compositor:** Damage Tracking und Präsentation.
- **Nova Accessibility Architecture:** Barrierefreiheitsprüfung.
- **NovaStudio:** Editor, Designer und Diagnose.
- **Nova.Trust:** Isolation und Berechtigungen.

## 14. Ergebnis

NovaStudio erhält eine native Live Preview, die Änderungen an Benutzeroberflächen unmittelbar sichtbar macht.

Die Vorschau verwendet dieselben UI-Komponenten wie die spätere Anwendung und aktualisiert ausschließlich betroffene Bereiche.

Entwickler können Layouts, Controls, Styles, Bindings und Zustände direkt überprüfen, ohne die gesamte Anwendung neu starten zu müssen.

**Grundsatz:** Was im Designer verändert wird, soll unmittelbar sichtbar sein – mit derselben Darstellung und demselben Verhalten wie in der fertigen Anwendung.