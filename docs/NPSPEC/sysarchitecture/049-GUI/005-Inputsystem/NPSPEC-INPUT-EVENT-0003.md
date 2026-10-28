# NPSPEC-INPUT-EVENT-0003 – Nova Pointer Events

**Status:** Angenommen  
**Bereich:** Input / Event / Pointer  
**System:** NovaOS

## 1. Zweck

Nova Pointer Events definiert ein einheitliches Ereignismodell für zeigerbasierte Eingaben innerhalb von NovaOS.

Ziel ist die hardwareunabhängige Verarbeitung von Maus-, Touch- und Stifteingaben über eine gemeinsame Schnittstelle.

Gerätespezifische Eigenschaften bleiben erhalten, während Anwendungen grundlegende Zeigerinteraktionen unabhängig vom verwendeten Eingabegerät verarbeiten können.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Pointer Event Processor | Erzeugung normalisierter Pointer-Ereignisse |
| Pointer State Manager | Verwaltung aktiver Zeigerzustände |
| Pointer Identity Manager | Eindeutige Identifikation von Zeigern |
| Pointer Event Router | Weiterleitung über Nova Event Routing |
| Pointer Capture Controller | Verwaltung zeigerbezogener Capture-Zuordnungen |

Die Verarbeitung erfolgt innerhalb der Nova Input Event Pipeline.

Nova Pointer Events ergänzt die gerätespezifischen Eingabemodelle, ersetzt diese jedoch nicht.

## 3. Pointer-Modell

Ein Pointer repräsentiert eine logische Zeigereingabe.

NovaOS unterscheidet folgende Pointer-Typen:

- `Mouse`
- `Touch`
- `Pen`
- `Virtual`
- `Other`

Ein Pointer besitzt mindestens:

- Pointer-ID.
- Gerätekennung.
- Input-Seat-Kennung.
- Pointer-Typ.
- Position.
- Tasten- beziehungsweise Kontaktstatus.
- Monotonen Zeitstempel.

Optionale Eigenschaften können Druck, Neigung, Kontaktfläche und weitere gerätespezifische Informationen enthalten.

Pointer-IDs MÜSSEN innerhalb ihres gültigen Lebenszyklus eindeutig sein.

## 4. Pointer-Ereignisse

NovaOS unterstützt mindestens folgende Ereignisse:

| Ereignis | Beschreibung |
|---|---|
| `PointerEnter` | Zeiger betritt einen Eingabebereich |
| `PointerLeave` | Zeiger verlässt einen Eingabebereich |
| `PointerMove` | Zeigerposition verändert sich |
| `PointerDown` | Taste oder Kontakt wird aktiviert |
| `PointerUp` | Taste oder Kontakt wird beendet |
| `PointerCancel` | Eingabesequenz wird abgebrochen |
| `PointerWheel` | Scrollbewegung eines Zeigegeräts |

Jedes Ereignis enthält die zur Verarbeitung erforderlichen Pointer-Informationen.

Gerätespezifische Ereignisse DÜRFEN zusätzlich bereitgestellt werden.

## 5. Pointer-Zustände

Der Pointer State Manager verwaltet die Zustände jedes aktiven Pointers.

Dabei gilt:

- Mauszeiger können dauerhaft bestehen.
- Touch-Pointer entstehen bei Kontaktbeginn und enden bei Kontaktende.
- Stift-Pointer können zwischen Hover und Kontakt wechseln.
- Mehrere Pointer können gleichzeitig aktiv sein.
- Jeder Pointer besitzt einen unabhängigen Zustand.

Ein `PointerCancel` beendet die betroffene Eingabesequenz kontrolliert.

Ungültige Ereignisfolgen DÜRFEN keine dauerhaft aktiven Pointer-Zustände verursachen.

## 6. Koordinaten

Pointer-Ereignisse unterstützen unterschiedliche Koordinatenräume:

- Gerätekoordinaten.
- Globale logische Displaykoordinaten.
- Fensterkoordinaten.
- Lokale UI-Elementkoordinaten.

Die Transformation erfolgt über die zuständigen Display-, Window- und UI-Komponenten.

Dabei MÜSSEN Skalierung, Rotation und geometrische Transformationen berücksichtigt werden.

Relative Bewegungsinformationen DÜRFEN zusätzlich bereitgestellt werden.

## 7. Pointer Capture

Pointer Capture ermöglicht die kontrollierte Bindung eines Pointers an einen Eingabeempfänger.

Dabei gilt:

- Capture wird für einen bestimmten Pointer verwaltet.
- Mehrere Pointer können unterschiedliche Capture-Empfänger besitzen.
- Capture wird bei Verlust der Berechtigung aufgehoben.
- Bei Zerstörung des Empfängers wird die betroffene Eingabesequenz kontrolliert beendet.
- Capture DARF keine Sicherheits- oder Sitzungsgrenzen umgehen.

Die Verwaltung erfolgt in Zusammenarbeit mit Nova Event Routing und Nova Input Focus Manager.

## 8. Ereignisverarbeitung

Pointer-Ereignisse werden über Nova Event Routing und Nova Event Dispatcher verarbeitet.

Dabei gilt:

- Die Ereignisreihenfolge pro Pointer bleibt erhalten.
- Zustandsänderungen werden konsistent verarbeitet.
- Bewegungsereignisse DÜRFEN kontrolliert zusammengefasst werden.
- `PointerDown`, `PointerUp` und `PointerCancel` DÜRFEN nicht unkontrolliert verworfen werden.
- Langsame Anwendungen DÜRFEN andere Eingabeempfänger nicht blockieren.

Pointer-Ereignisse bleiben unabhängig von der Rendering-Frequenz.

## 9. Zusammenspiel mit Nova UI

Nova UI verwendet Pointer Events als gemeinsame Grundlage für zeigerbasierte Interaktionen.

Dazu gehören:

- Hover.
- Drücken und Loslassen.
- Ziehen.
- Auswahl.
- Scrollen.
- Kontextabhängige Bedienung.

Die Interpretation von Klicks, Doppelklicks und Gesten erfolgt durch übergeordnete Eingabekomponenten beziehungsweise Nova UI.

Eine Anwendung DARF gerätespezifische Eigenschaften auswerten, sofern diese verfügbar sind.

## 10. Sicherheit und Fehlerbehandlung

Nova Pointer Events unterliegt Nova Input Security und Nova.Trust.

- Ereignisse werden ausschließlich berechtigten Empfängern zugestellt.
- Virtuelle und injizierte Pointer bleiben anhand ihrer Herkunft unterscheidbar.
- Ungültige Pointer-IDs werden zurückgewiesen.
- Bei Geräteverlust werden betroffene Pointer-Zustände bereinigt.
- Fehlerhafte Koordinaten und Zusatzinformationen werden kontrolliert behandelt.
- Anwendungen DÜRFEN keine fremden Pointer-Ereignisse ohne Berechtigung überwachen.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS ein einheitliches Pointer-Ereignismodell bereitstellen.
- **INPUT-002:** Maus-, Touch- und Stifteingaben MÜSSEN als Pointer Events abgebildet werden können.
- **INPUT-003:** Gerätespezifische Eigenschaften MÜSSEN bei der Normalisierung erhalten bleiben können.
- **INPUT-004:** Jeder aktive Pointer MUSS eindeutig identifizierbar sein.
- **INPUT-005:** Mehrere Pointer MÜSSEN gleichzeitig unterstützt werden.
- **INPUT-006:** Pointer-Bewegungen, Kontaktbeginn, Kontaktende und Abbruch MÜSSEN unterscheidbar sein.
- **INPUT-007:** Die Ereignisreihenfolge pro Pointer MUSS erhalten bleiben.
- **INPUT-008:** Pointer-Koordinaten MÜSSEN in unterschiedliche logische Koordinatenräume transformierbar sein.
- **INPUT-009:** Pointer Capture MUSS zeigerbezogen verwaltet werden.
- **INPUT-010:** Bewegungsereignisse DÜRFEN kontrolliert zusammengefasst werden.
- **INPUT-011:** Zustandskritische Pointer-Ereignisse DÜRFEN nicht unkontrolliert verloren gehen.
- **INPUT-012:** Bei Geräteverlust MÜSSEN betroffene Pointer-Zustände kontrolliert beendet werden.
- **INPUT-013:** Pointer-Ereignisse MÜSSEN über Nova Event Routing und Nova Event Dispatcher zugestellt werden.
- **INPUT-014:** Pointer-Ereignisse MÜSSEN unabhängig von der Rendering-Frequenz verarbeitet werden.
- **INPUT-015:** Virtuelle Pointer-Ereignisse MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-016:** Nova Pointer Events MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Mouse Input:** Mausbewegungen und Tasten.
- **Nova Touch Input:** Touchkontakte und Multitouch.
- **Nova Pen and Stylus Input:** Stiftpositionen und Zusatzinformationen.
- **Nova Multi-Device Input:** Mehrere gleichzeitige Eingabegeräte.
- **Nova Cursor Management:** Sichtbare Cursorposition.
- **Nova Event Routing:** Ermittlung des Eingabeempfängers.
- **Nova Event Dispatcher:** Ereigniszustellung.
- **Nova Window Manager:** Fensterbezogene Eingabebereiche.
- **Nova UI:** Verarbeitung zeigerbasierter Interaktionen.
- **Nova Input Security / Nova.Trust:** Berechtigungen und Isolation.

## 13. Ergebnis

NovaOS erhält ein einheitliches Ereignismodell für sämtliche zeigerbasierten Eingabegeräte.

Anwendungen können grundlegende Pointer-Interaktionen unabhängig davon verarbeiten, ob sie durch Maus, Touchscreen oder digitalen Stift ausgelöst wurden.

Gerätespezifische Funktionen bleiben verfügbar, ohne die gemeinsame Eingabearchitektur zu beeinträchtigen.

**Grundsatz:** Unterschiedliche Eingabegeräte verwenden eine gemeinsame Pointer-Schnittstelle, ohne ihre individuellen Fähigkeiten zu verlieren.