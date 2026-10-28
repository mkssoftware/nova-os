# NPSPEC-INPUT-EVENT-0004 – Nova Keyboard Events

**Status:** Angenommen  
**Bereich:** Input / Event / Keyboard  
**System:** NovaOS

## 1. Zweck

Nova Keyboard Events definiert das einheitliche Ereignismodell für tastaturbasierte Eingaben innerhalb von NovaOS.

Ziel ist eine hardwareunabhängige, deterministische und sichere Verarbeitung von Tastendrücken, Tastenfreigaben, Wiederholungen und Tastenkombinationen.

Physische Tastenereignisse, logische Tastenzuordnungen und Texteingaben werden voneinander getrennt.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Keyboard Event Processor | Normalisierung von Tastaturereignissen |
| Keyboard State Tracker | Verwaltung gedrückter Tasten |
| Modifier State Manager | Verwaltung von Modifikatortasten |
| Key Repeat Controller | Steuerung wiederholter Tasteneingaben |
| Keyboard Event Router | Weiterleitung an den zuständigen Eingabekontext |

Die Verarbeitung erfolgt innerhalb der Nova Input Event Pipeline.

Die Umsetzung physischer Tasten in logische Tasten erfolgt über die Nova Input Mapping Engine.

## 3. Ereignismodell

NovaOS unterscheidet drei Ebenen:

### Physical Key Events

Beschreiben die Betätigung einer physischen Taste unabhängig vom Tastaturlayout.

Enthalten:

- Gerätekennung.
- Physischen Tastencode.
- Betätigungszustand.
- Monotonen Zeitstempel.

### Logical Key Events

Beschreiben die durch das aktive Tastaturlayout bestimmte logische Taste.

Berücksichtigen:

- Tastaturlayout.
- Modifikatortasten.
- Aktiven Eingabekontext.
- Tastenzuordnung.

### Text Input Events

Übermitteln erzeugte Texte unabhängig von einzelnen Tastendrücken.

Text kann durch Tastaturlayouts, Dead Keys, Compose-Sequenzen oder Eingabemethoden entstehen.

Ein Tastendruck MUSS nicht zwangsläufig ein Textzeichen erzeugen.

## 4. Tastaturereignisse

NovaOS unterstützt mindestens folgende Ereignisse:

| Ereignis | Beschreibung |
|---|---|
| `KeyDown` | Eine Taste wurde gedrückt |
| `KeyUp` | Eine Taste wurde losgelassen |
| `KeyRepeat` | Eine gehaltene Taste wird wiederholt |
| `TextInput` | Ein Text wurde erzeugt |
| `KeyCancel` | Eine aktive Tastensequenz wurde abgebrochen |

Ein Tastaturereignis enthält mindestens:

- Ereignistyp.
- Gerätekennung.
- Input-Seat-Kennung.
- Physischen Tastencode, sofern zutreffend.
- Logischen Tastencode, sofern zutreffend.
- Modifikatorzustand.
- Monotonen Zeitstempel.
- Ereignissequenznummer.

`TextInput` enthält die erzeugte Unicode-Zeichenfolge statt einer zwingenden Zuordnung zu einer einzelnen Taste.

## 5. Tastenzustände und Modifikatoren

Der Keyboard State Tracker verwaltet gedrückte Tasten gerätebezogen.

Unterstützte Modifikatoren umfassen:

- Shift.
- Ctrl.
- Alt.
- AltGr.
- System-/Meta-Taste.
- Caps Lock.
- Num Lock.
- Scroll Lock.

Linke und rechte Modifikatortasten MÜSSEN unterscheidbar bleiben.

Aggregierte Tastenzustände mehrerer Tastaturen werden durch Nova Multi-Device Input koordiniert.

Das Loslassen einer Taste auf einem Gerät DARF einen weiterhin aktiven Tastenzustand eines anderen Geräts nicht unbeabsichtigt aufheben.

## 6. Tastenwiederholung

NovaOS unterstützt konfigurierbare Tastenwiederholungen.

Dabei gilt:

- Wiederholungsverzögerung und Wiederholungsrate sind konfigurierbar.
- Wiederholungen werden zentral gesteuert.
- Nicht wiederholbare Tasten werden berücksichtigt.
- `KeyRepeat` ist von einem neuen physischen Tastendruck unterscheidbar.
- Bei Tastenfreigabe oder Geräteverlust wird die Wiederholung beendet.

Die Wiederholungssteuerung MUSS unabhängig von der Rendering-Frequenz funktionieren.

## 7. Tastenkombinationen und Texteingabe

Tastenkombinationen werden anhand logischer Tasten und Modifikatorzustände ausgewertet.

Dabei gilt:

- Systemreservierte Tastenkombinationen besitzen Vorrang.
- Anwendungen dürfen eigene Tastenkombinationen innerhalb ihres Eingabekontexts definieren.
- Tastenkombinationen DÜRFEN keine geschützten Systemaktionen unberechtigt überschreiben.
- Texteingaben werden unabhängig von Tastenkombinationen verarbeitet.

Die Verarbeitung von Dead Keys, Compose-Sequenzen und Eingabemethoden erfolgt über die zuständigen Mapping- und Texteingabekomponenten.

Nova Keyboard Events stellt hierfür die erforderlichen Ereignisse bereit.

## 8. Fokus und Ereigniszustellung

Tastaturereignisse werden grundsätzlich an den aktiven Tastaturfokus zugestellt.

Dabei gilt:

- Die Zielermittlung erfolgt über Nova Event Routing.
- Die Zustellung übernimmt Nova Event Dispatcher.
- Fokuswechsel werden konsistent berücksichtigt.
- Geschützte Eingabekontexte besitzen Vorrang.
- Systemreservierte Eingaben werden vor regulären Anwendungseingaben verarbeitet.

Ein Fokuswechsel DARF keine unbeabsichtigte Weitergabe vertraulicher Tastatureingaben verursachen.

## 9. Sicherheit und Fehlerbehandlung

Nova Keyboard Events unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Anwendungen erhalten ausschließlich berechtigte Tastaturereignisse.
- Globale Tastaturüberwachung erfordert eine ausdrücklich autorisierte Capability.
- Virtuelle und injizierte Tastaturereignisse bleiben anhand ihrer Herkunft unterscheidbar.
- Geschützte Tastatureingaben DÜRFEN nicht unberechtigt protokolliert werden.
- Bei Geräteverlust werden aktive Tastenzustände kontrolliert bereinigt.
- Ungültige Tastencodes werden sicher behandelt.
- Fehlerhafte Ereignisfolgen DÜRFEN keine dauerhaft gedrückten Tasten verursachen.

## 10. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS ein hardwareunabhängiges Tastaturereignismodell bereitstellen.
- **INPUT-002:** Physische Tasten, logische Tasten und Texteingaben MÜSSEN getrennt verarbeitet werden.
- **INPUT-003:** `KeyDown`, `KeyUp`, `KeyRepeat` und `TextInput` MÜSSEN unterstützt werden.
- **INPUT-004:** Die Ereignisreihenfolge pro Tastatur MUSS erhalten bleiben.
- **INPUT-005:** Tastenzustände MÜSSEN gerätebezogen verwaltet werden.
- **INPUT-006:** Linke und rechte Modifikatortasten MÜSSEN unterscheidbar sein.
- **INPUT-007:** Mehrere Tastaturen MÜSSEN gleichzeitig unterstützt werden.
- **INPUT-008:** Tastenwiederholungen MÜSSEN konfigurierbar sein.
- **INPUT-009:** Systemreservierte Tastenkombinationen MÜSSEN Vorrang vor Anwendungseingaben besitzen.
- **INPUT-010:** Texteingaben MÜSSEN Unicode unterstützen.
- **INPUT-011:** Tastaturereignisse MÜSSEN über Nova Event Routing und Nova Event Dispatcher zugestellt werden.
- **INPUT-012:** Fokuswechsel MÜSSEN bei der Ereigniszustellung berücksichtigt werden.
- **INPUT-013:** Bei Geräteverlust MÜSSEN aktive Tastenzustände kontrolliert bereinigt werden.
- **INPUT-014:** Virtuelle Tastaturereignisse MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-015:** Die Tastaturereignisverarbeitung MUSS unabhängig von Nova UI und der Rendering-Frequenz funktionieren.
- **INPUT-016:** Bei identischem Ausgangszustand und identischer Ereignisfolge MUSS die Verarbeitung reproduzierbar sein.
- **INPUT-017:** Nova Keyboard Events MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 11. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Keyboard Input:** Erfassung physischer Tastatureingaben.
- **Nova Input Mapping Engine:** Tastaturlayouts und logische Tastenzuordnung.
- **Nova Multi-Device Input:** Verwaltung mehrerer Tastaturen.
- **Nova Input Hotplug:** Gerätewechsel und Zustandsbereinigung.
- **Nova Event Routing:** Fokusabhängige Zielermittlung.
- **Nova Event Dispatcher:** Ereigniszustellung.
- **Nova Input Focus Manager:** Tastaturfokus.
- **Nova UI:** Tastaturinteraktionen und Texteingabe.
- **Nova Input Security / Nova.Trust:** Berechtigungen und Isolation.

## 12. Ergebnis

NovaOS erhält ein einheitliches Ereignismodell für Tastatureingaben.

Die Trennung von physischen Tastendrücken, logischen Tastenzuordnungen und Texteingaben ermöglicht eine konsistente Verarbeitung unterschiedlicher Tastaturen, Layouts und Eingabemethoden.

**Grundsatz:** Eine physische Taste, ihre logische Bedeutung und der daraus entstehende Text sind unterschiedliche Informationen und werden von NovaOS entsprechend getrennt behandelt.