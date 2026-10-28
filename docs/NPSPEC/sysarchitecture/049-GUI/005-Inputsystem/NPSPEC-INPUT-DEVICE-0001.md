# NPSPEC-INPUT-DEVICE-0001 – Nova Keyboard Input

**Status:** Angenommen  
**Bereich:** Input / Device / Keyboard  
**System:** NovaOS

## 1. Zweck

Nova Keyboard Input definiert die Erfassung und Verarbeitung von Tastatureingaben innerhalb von NovaOS.

Ziel ist eine hardwareunabhängige, latenzarme und zuverlässige Tastaturunterstützung für grafische Oberflächen, Systemkonsolen, Anwendungen und Recovery-Umgebungen.

Die Architektur trennt physische Tasten, logische Tastenbelegungen und Texteingaben.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Keyboard Driver Adapter | Anbindung der Tastaturtreiber |
| Keyboard Device Manager | Verwaltung angeschlossener Tastaturen |
| Key Event Processor | Verarbeitung von Tastendrücken und Tastenfreigaben |
| Keyboard State Manager | Verwaltung gedrückter Tasten und Modifier |
| Keyboard Layout Engine | Zuordnung physischer zu logischen Tasten |
| Key Repeat Controller | Steuerung der Tastenwiederholung |

Die Komponenten integrieren sich in die Nova Input Device Abstraction und Nova Input Event Pipeline.

Hardwareprotokolle werden durch die jeweiligen Treiber verarbeitet.

## 3. Tastaturmodell

NovaOS unterscheidet drei Ebenen:

### Physical Key

Identifiziert die physische Taste unabhängig von Tastaturlayout und Sprache.

Beispiele:
- Linke Umschalttaste.
- Rechte Umschalttaste.
- Taste an einer definierten physischen Position.
- Funktionstasten.

### Logical Key

Beschreibt die durch das aktive Tastaturlayout zugeordnete Taste beziehungsweise Funktion.

Beispiele:
- A.
- Z.
- Enter.
- Escape.
- Pfeiltasten.

### Text Input

Beschreibt die tatsächlich erzeugte Texteingabe.

Eine physische Taste kann abhängig von Layout, Modifiern und Eingabemethode unterschiedliche Zeichen erzeugen.

Physische Tastendrücke DÜRFEN nicht unmittelbar mit Unicode-Zeichen gleichgesetzt werden.

## 4. Tastaturereignisse

NovaOS unterstützt mindestens:

- `KeyDown`
- `KeyUp`
- `KeyRepeat`
- `KeyboardConnected`
- `KeyboardDisconnected`
- `KeyboardLayoutChanged`

Ein Tastenereignis enthält:

- Gerätekennung.
- Physische Tastenkennung.
- Logische Tastenkennung.
- Ereigniszeitstempel.
- Modifier-Zustand.
- Wiederholungskennzeichnung.

Die Ereignisse werden über die Nova Input Event Pipeline weitergeleitet.

Textbezogene Ereignisse werden durch die zuständige Texteingabekomponente erzeugt.

## 5. Tastaturzustand

Der Keyboard State Manager verwaltet:

- Gedrückte Tasten.
- Linke und rechte Modifier.
- Caps Lock.
- Num Lock.
- Scroll Lock.
- Aktives Tastaturlayout.
- Wiederholungszustand.

Modifier werden einzeln erfasst und können zusätzlich zu logischen Gruppen zusammengefasst werden.

Bei Geräteverlust MÜSSEN alle zugehörigen gedrückten Tasten kontrolliert freigegeben werden.

Die Zustände mehrerer Tastaturen MÜSSEN gerätebezogen verwaltet werden können.

## 6. Tastaturlayouts

Die Keyboard Layout Engine unterstützt:

- Unterschiedliche nationale Tastaturlayouts.
- Benutzerdefinierte Tastenbelegungen.
- Modifier-Kombinationen.
- AltGr.
- Dead Keys.
- Compose-Sequenzen.
- Layoutwechsel während des Betriebs.

Tastaturlayouts werden unabhängig von den Hardwaretreibern verwaltet.

Die Zuordnung physischer Tasten zu logischen Tasten MUSS reproduzierbar erfolgen.

Die eigentliche Unicode-Texterzeugung und komplexe Eingabemethoden werden durch das übergeordnete Texteingabesystem bereitgestellt.

## 7. Tastenwiederholung

Der Key Repeat Controller verwaltet die automatische Wiederholung gedrückter Tasten.

Konfigurierbar sind:

- Wiederholungsverzögerung.
- Wiederholungsfrequenz.
- Aktivierung und Deaktivierung.
- Tastenbezogene Wiederholungsregeln.

Nicht wiederholbare Tasten werden entsprechend ihrer Definition behandelt.

Tastenwiederholungen MÜSSEN als synthetisch erzeugte Wiederholungsereignisse erkennbar sein.

Die Wiederholung MUSS beim Loslassen der Taste, beim Geräteverlust oder beim Verlust des zulässigen Eingabekontexts beendet werden.

## 8. Mehrere Tastaturen

NovaOS unterstützt mehrere gleichzeitig angeschlossene Tastaturen.

Dabei gilt:

- Jede Tastatur besitzt eine eigene Gerätekennung.
- Physische Tastenzustände werden getrennt verwaltet.
- Ereignisse können einem gemeinsamen Eingabekontext zugeordnet werden.
- Geräte können unabhängig hinzugefügt und entfernt werden.
- Eine Tastatur DARF nicht die Zustände einer anderen Tastatur verändern.

Die Zuordnung zu Benutzersitzungen erfolgt über die zentrale Input-Architektur.

## 9. Sicherheit

Nova Keyboard Input unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Tastatureingaben werden ausschließlich an berechtigte Empfänger weitergeleitet.
- Globale Tastaturüberwachung erfordert eine entsprechende Capability.
- Geschützte Texteingaben dürfen nicht unberechtigt mitgelesen werden.
- Systemreservierte Tastenkombinationen werden durch vertrauenswürdige Komponenten verarbeitet.
- Virtuelle Tastatureingaben müssen als solche identifizierbar sein.
- Nicht autorisierte Eingabeinjektionen werden zurückgewiesen.

Tastatureingaben DÜRFEN nicht standardmäßig protokolliert werden.

## 10. Performance und Fehlerbehandlung

Die Tastaturverarbeitung erfolgt ereignisgesteuert.

- Tastaturereignisse werden ohne unnötige Verzögerung verarbeitet.
- Die Verarbeitung ist unabhängig von der Rendering-Frequenz.
- Fehlerhafte Tastencodes werden kontrolliert behandelt.
- Unbekannte Tasten DÜRFEN als physische Eingaben erhalten bleiben.
- Verlorene Ereignisse MÜSSEN durch eine definierte Zustandssynchronisation behandelt werden.
- Geräteausfälle DÜRFEN andere Tastaturen nicht beeinträchtigen.

Für grundlegende Systemfunktionen MUSS eine Tastaturverarbeitung ohne grafische Oberfläche möglich sein.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine hardwareunabhängige Tastaturverarbeitung bereitstellen.
- **INPUT-002:** Physische Tasten, logische Tasten und Texteingaben MÜSSEN getrennt behandelt werden.
- **INPUT-003:** Tastendrücke und Tastenfreigaben MÜSSEN als eigenständige Ereignisse verarbeitet werden.
- **INPUT-004:** Jede Tastatur MUSS eine eindeutige Gerätekennung besitzen.
- **INPUT-005:** Gedrückte Tasten und Modifier MÜSSEN gerätebezogen verwaltet werden.
- **INPUT-006:** Mehrere Tastaturen MÜSSEN gleichzeitig unterstützt werden.
- **INPUT-007:** Tastaturlayouts MÜSSEN unabhängig von Hardwaretreibern funktionieren.
- **INPUT-008:** Tastaturlayouts MÜSSEN während des Betriebs gewechselt werden können.
- **INPUT-009:** Tastenwiederholung MUSS konfigurierbar sein.
- **INPUT-010:** Tastenwiederholungen MÜSSEN von physischen Tastendrücken unterscheidbar sein.
- **INPUT-011:** Bei Geräteverlust MÜSSEN aktive Tastenzustände kontrolliert bereinigt werden.
- **INPUT-012:** Tastaturereignisse MÜSSEN über die Nova Input Event Pipeline verarbeitet werden.
- **INPUT-013:** Sicherheitskritische Tastenkombinationen MÜSSEN vor der regulären Anwendungszustellung behandelt werden.
- **INPUT-014:** Anwendungen DÜRFEN fremde Tastatureingaben nicht ohne Berechtigung überwachen.
- **INPUT-015:** Die grundlegende Tastaturverarbeitung MUSS ohne Nova UI funktionieren.
- **INPUT-016:** Nova Keyboard Input MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Input Architecture:** Übergeordnete Eingabeverarbeitung.
- **Nova Input Device Abstraction:** Hardwareunabhängiges Tastaturmodell.
- **Nova Input Event Pipeline:** Ereignisverarbeitung und Zustellung.
- **Nova Input Security:** Eingabeschutz und Berechtigungen.
- **Nova Input Mapping Engine:** Benutzerdefinierte Tastenbelegungen.
- **Nova USB/HID:** USB-Tastaturen.
- **Nova PS/2:** Unterstützung entsprechender Legacy-Tastaturen.
- **Nova UI:** Tastaturbedienung und Texteingabe.
- **NovaDOS / Recovery:** Tastatureingaben ohne grafische Oberfläche.

## 13. Ergebnis

NovaOS erhält eine einheitliche Tastaturarchitektur für physische und virtuelle Tastaturen.

Die Trennung von Hardwareerfassung, Tastenzuständen, Layoutverarbeitung und Texteingabe ermöglicht eine flexible und zuverlässige Nutzung in allen Systemumgebungen.

**Grundsatz:** Eine Taste ist zunächst eine physische Eingabe. Welche Funktion oder welches Zeichen daraus entsteht, entscheidet der jeweilige Eingabekontext.