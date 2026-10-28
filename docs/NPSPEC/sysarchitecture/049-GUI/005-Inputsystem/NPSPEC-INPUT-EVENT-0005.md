# NPSPEC-INPUT-EVENT-0005 – Nova Gesture Recognition

**Status:** Angenommen  
**Bereich:** Input / Event / Gesture Recognition  
**System:** NovaOS

## 1. Zweck

Nova Gesture Recognition definiert die hardwareunabhängige Erkennung und Verarbeitung von Eingabegesten innerhalb von NovaOS.

Ziel ist eine präzise, latenzarme und deterministische Interpretation von Maus-, Touch-, Touchpad- und Stifteingaben.

Die Gestenerkennung erfolgt regelbasiert und funktioniert vollständig ohne KI.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Gesture Recognition Engine | Zentrale Erkennung von Gesten |
| Gesture State Manager | Verwaltung laufender Gestensequenzen |
| Gesture Pattern Matcher | Auswertung von Bewegungen und Kontaktmustern |
| Gesture Configuration Manager | Verwaltung von Schwellwerten und Einstellungen |
| Gesture Arbitration Controller | Auflösung konkurrierender Gesten |
| Gesture Event Generator | Erzeugung semantischer Gestenereignisse |

Die Gestenerkennung verarbeitet normalisierte Pointer Events.

Die Zustellung erkannter Gesten erfolgt über Nova Event Routing und Nova Event Dispatcher.

## 3. Gestenmodell

NovaOS unterscheidet drei Ebenen:

### Raw Input

Physische Eingabeinformationen:

- Pointerposition.
- Kontaktbeginn und Kontaktende.
- Bewegungsdaten.
- Zeitstempel.
- Kontaktdruck und weitere optionale Eigenschaften.

### Gesture Recognition

Analyse der Eingabesequenz anhand definierter Regeln:

- Bewegungsrichtung.
- Bewegungsdistanz.
- Geschwindigkeit.
- Kontaktdauer.
- Anzahl gleichzeitiger Kontakte.
- Relative Positionen mehrerer Kontakte.

### Semantic Gesture

Erkannte Benutzeraktion, beispielsweise:

- Tippen.
- Ziehen.
- Scrollen.
- Vergrößern.
- Drehen.

Die Zuordnung einer Geste zu einer konkreten Anwendungsfunktion erfolgt durch den jeweiligen Eingabekontext.

## 4. Unterstützte Gesten

NovaOS unterstützt mindestens folgende Gestentypen:

| Geste | Beschreibung |
|---|---|
| Tap | Kurze Berührung oder Betätigung |
| DoubleTap | Zwei aufeinanderfolgende Betätigungen |
| LongPress | Längeres Halten einer Eingabe |
| Pan | Kontinuierliche Verschiebung |
| Swipe | Gerichtete Wischbewegung |
| Pinch | Veränderung des Abstands mehrerer Kontakte |
| Rotate | Drehbewegung mehrerer Kontakte |
| Drag | Ziehbewegung mit aktivem Kontakt |

Weitere Gestentypen DÜRFEN durch registrierte Gesture Recognizer ergänzt werden.

Die Verfügbarkeit einzelner Gesten hängt von den Fähigkeiten des Eingabegeräts ab.

## 5. Gestenzustände

Jeder Gesture Recognizer verwendet einen definierten Zustandsautomaten.

Unterstützte Zustände:

1. **Possible:** Eine Geste könnte vorliegen.
2. **Recognized:** Das Gestenmuster wurde erkannt.
3. **Active:** Eine kontinuierliche Geste wird ausgeführt.
4. **Completed:** Die Geste wurde regulär abgeschlossen.
5. **Failed:** Das Gestenmuster wurde nicht erfüllt.
6. **Cancelled:** Die Gestensequenz wurde abgebrochen.

Zustandsübergänge MÜSSEN eindeutig definiert sein.

Bei Geräteverlust, Fokusverlust oder ungültigem Eingabekontext werden betroffene Gesten kontrolliert abgebrochen.

## 6. Gestenereignisse

NovaOS stellt mindestens folgende Ereignisse bereit:

- `GestureStarted`
- `GestureUpdated`
- `GestureCompleted`
- `GestureCancelled`

Ein Gestenereignis enthält:

- Gestentyp.
- Eindeutige Gestenkennung.
- Beteiligte Pointer-IDs.
- Eingabekontext.
- Monotonen Zeitstempel.
- Aktuellen Gestenzustand.
- Gestenspezifische Parameter.

Gestenspezifische Parameter können sein:

- Verschiebung.
- Geschwindigkeit.
- Skalierungsfaktor.
- Drehwinkel.
- Mittelpunkt.
- Bewegungsrichtung.

Kontinuierliche Gesten MÜSSEN Zwischenstände bereitstellen können.

## 7. Gestenerkennung und Konfliktauflösung

Mehrere Gesture Recognizer können dieselbe Eingabesequenz untersuchen.

Dabei gilt:

- Konkurrierende Gesten werden anhand definierter Regeln aufgelöst.
- Anwendungen können zulässige Gesten für ihren Eingabekontext konfigurieren.
- Exklusive Gesten verhindern widersprüchliche parallele Aktionen.
- Kompatible Gesten DÜRFEN gleichzeitig erkannt werden.
- Bereits erkannte Gesten können bei Konflikten kontrolliert abgebrochen werden.

Beispielsweise können `Pinch` und `Rotate` gleichzeitig aktiv sein, während `Tap` und `Pan` innerhalb derselben exklusiven Eingabesequenz konkurrieren.

Die Konfliktauflösung MUSS reproduzierbar sein.

## 8. Konfiguration

NovaOS unterstützt konfigurierbare Erkennungsparameter.

Dazu gehören:

- Maximale Tap-Dauer.
- Maximale Bewegungstoleranz.
- Double-Tap-Zeitfenster.
- Long-Press-Dauer.
- Mindestdistanz für Swipe.
- Mindestgeschwindigkeit für Swipe.
- Bewegungsschwellwerte für Pan.
- Mindeständerungen für Pinch und Rotate.

Die Parameter können geräte- und benutzerspezifisch angepasst werden.

Systemweite Standardwerte MÜSSEN vorhanden sein.

Änderungen der Parameter DÜRFEN laufende Gestensequenzen nicht unkontrolliert verändern.

## 9. Integration mit Nova UI

Nova UI verwendet Gestenereignisse für die Interaktion mit Steuerelementen.

Beispiele:

- Scrollen von Listen.
- Verschieben von Objekten.
- Zoomen von Bildern.
- Drehen von CAD-Objekten.
- Öffnen von Kontextmenüs.
- Bedienung von Touch-Oberflächen.

Nova UI DARF eigene Gesture Recognizer registrieren.

Die zentrale Gestenerkennung bleibt unabhängig von Nova UI und kann auch durch andere Eingabekontexte verwendet werden.

## 10. Performance und Ressourcenökonomie

Die Gestenerkennung erfolgt ereignisgesteuert.

Festlegungen:

- Nur relevante Pointer-Ereignisse werden ausgewertet.
- Inaktive Gesture Recognizer verursachen keine permanente Verarbeitung.
- Zeitabhängige Gesten verwenden den zentralen Zeitdienst.
- Bewegungsdaten DÜRFEN kontrolliert zusammengefasst werden.
- Zustandskritische Eingaben MÜSSEN erhalten bleiben.
- Die Erkennung erfolgt unabhängig von der Rendering-Frequenz.

Die Verarbeitung SOLL ohne unnötige Speicherallokationen während laufender Gestensequenzen auskommen.

## 11. Sicherheit und Fehlerbehandlung

Nova Gesture Recognition unterliegt Nova Input Security und Nova.Trust.

- Gesten werden ausschließlich innerhalb berechtigter Eingabekontexte verarbeitet.
- Systemreservierte Gesten besitzen Vorrang vor Anwendungsgesten.
- Anwendungen DÜRFEN keine fremden Eingabesequenzen überwachen.
- Virtuelle Eingaben unterliegen denselben Sicherheitsprüfungen wie physische Eingaben.
- Ungültige Pointer-Zustände führen zum kontrollierten Abbruch.
- Fehler einzelner Gesture Recognizer DÜRFEN die zentrale Eingabeverarbeitung nicht blockieren.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale, hardwareunabhängige Gestenerkennung bereitstellen.
- **INPUT-002:** Die Gestenerkennung MUSS normalisierte Pointer Events verwenden.
- **INPUT-003:** Tap, DoubleTap, LongPress, Pan, Swipe, Pinch, Rotate und Drag MÜSSEN unterstützt werden.
- **INPUT-004:** Gesten MÜSSEN über definierte Zustandsautomaten verarbeitet werden.
- **INPUT-005:** Kontinuierliche Gesten MÜSSEN Zwischenstände bereitstellen können.
- **INPUT-006:** Mehrere gleichzeitige Kontakte MÜSSEN unterstützt werden.
- **INPUT-007:** Konkurrierende Gesten MÜSSEN kontrolliert aufgelöst werden.
- **INPUT-008:** Kompatible Gesten DÜRFEN gleichzeitig verarbeitet werden.
- **INPUT-009:** Erkennungsschwellwerte MÜSSEN konfigurierbar sein.
- **INPUT-010:** Gestenereignisse MÜSSEN über Nova Event Routing und Nova Event Dispatcher zugestellt werden.
- **INPUT-011:** Bei Geräte- oder Kontextverlust MÜSSEN betroffene Gesten kontrolliert abgebrochen werden.
- **INPUT-012:** Systemreservierte Gesten MÜSSEN Vorrang vor regulären Anwendungsgesten besitzen.
- **INPUT-013:** Die Gestenerkennung MUSS unabhängig von Nova UI und der Rendering-Frequenz funktionieren.
- **INPUT-014:** Identische Eingabesequenzen und Konfigurationen MÜSSEN reproduzierbare Erkennungsergebnisse liefern.
- **INPUT-015:** Nova Gesture Recognition MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Pointer Events:** Einheitliche Zeigerereignisse.
- **Nova Touch Input:** Multitouch-Kontakte.
- **Nova Mouse Input:** Mausbasierte Gesten.
- **Nova Pen and Stylus Input:** Stiftbasierte Gesten.
- **Nova Event Routing:** Zielermittlung.
- **Nova Event Dispatcher:** Zustellung erkannter Gesten.
- **Nova Input Focus Manager:** Eingabekontext und Fokus.
- **Nova UI:** Gestenbasierte Bedienung.
- **Nova Time:** Zeitmessung und Erkennungsschwellwerte.
- **Nova Input Security / Nova.Trust:** Berechtigungen und Isolation.

## 14. Ergebnis

NovaOS erhält eine einheitliche und erweiterbare Architektur zur Erkennung von Eingabegesten.

Durch regelbasierte Zustandsautomaten, konfigurierbare Schwellwerte und kontrollierte Konfliktauflösung können unterschiedliche Eingabegeräte konsistente Gestenereignisse erzeugen.

**Grundsatz:** NovaOS erkennt die Absicht einer Eingabebewegung anhand definierter Regeln. Welche Funktion dadurch ausgelöst wird, entscheidet der jeweilige Eingabekontext.