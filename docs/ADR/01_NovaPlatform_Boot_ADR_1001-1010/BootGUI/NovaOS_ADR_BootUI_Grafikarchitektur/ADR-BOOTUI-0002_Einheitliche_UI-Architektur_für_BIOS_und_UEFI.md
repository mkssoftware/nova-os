# ADR-BOOTUI-0002 – Einheitliche UI-Architektur für BIOS und UEFI

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0002 |
| Titel | Einheitliche UI-Architektur für BIOS und UEFI |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

NovaOS unterstützt BIOS und UEFI parallel. Beide Umgebungen unterscheiden sich erheblich bei Initialisierung, Grafikmodus, Eingabegeräten, Speicherzugriff und Firmware-Schnittstellen. Zwei getrennte UI-Implementierungen würden zu inkonsistentem Verhalten, doppelter Entwicklungsarbeit und abweichender visueller Qualität führen.

## 2. Entscheidung

Die gesamte Boot-UI oberhalb einer schmalen Plattformabstraktion wird gemeinsam implementiert. BIOS und UEFI stellen der UI ein identisches Plattforminterface bereit.

Das Interface umfasst:

- Framebuffer-Beschreibung,
- Modus- und Auflösungsinformationen,
- monotone Zeitquelle,
- Speicherreservierung,
- Tastaturereignisse,
- Pointer-Ereignisse,
- Neustart, Ausschalten und Firmware-Rückkehr,
- Diagnoseausgabe,
- Zugriff auf eingebettete oder geladene Ressourcen.

Plattformspezifischer Code darf keine UI-Layouts, Controls oder Designentscheidungen enthalten.

## 3. Technische Ausgestaltung

### Plattformvertrag

Das gemeinsame Interface muss mindestens folgende Daten liefern:

- physische und virtuelle Framebuffer-Adresse,
- Breite, Höhe, Pixelstride und Pixelformat,
- Bitmasken oder Kanalreihenfolge,
- Zeit in Mikrosekunden oder einer dokumentierten Tick-Einheit,
- Liste verfügbarer Eingabegeräte,
- Fähigkeit zur Modusumschaltung,
- Hardware- und Firmwaremerkmale.

### Normalisierung

Die Backend-Schicht normalisiert:

- BGR-, RGB- und maskenbasierte Pixelformate,
- unterschiedliche Tastencodes,
- absolute und relative Pointerdaten,
- BIOS- und UEFI-Zeitquellen,
- Fehlercodes,
- Speicherbereiche.

### Gleichheitsanforderung

Bei identischer Auflösung und identischem Theme muss das visuelle Ergebnis auf BIOS und UEFI pixelnah übereinstimmen. Abweichungen sind nur erlaubt, wenn Firmware oder Hardware bestimmte Funktionen nicht bereitstellen.

## 4. Konsequenzen

### Positive Folgen

- nur eine UI-Codebasis,
- identische Bedienung und Gestaltung,
- bessere Testbarkeit,
- leichtere spätere Erweiterung um weitere Plattformen,
- klare Trennung zwischen Firmwarezugriff und Benutzeroberfläche.

### Negative Folgen

- das gemeinsame Interface muss den kleinsten gemeinsamen Nenner berücksichtigen,
- BIOS benötigt mehr eigenen Initialisierungscode,
- plattformspezifische Optimierungen sind nur kontrolliert nutzbar.

## 5. Verworfene Alternativen

- **Separate BIOS- und UEFI-Oberflächen:** führt zu Drift, Doppelarbeit und höheren Fehlerquoten.
- **Nur UEFI unterstützen:** widerspricht der NovaOS-Zielsetzung für breite Hardwareunterstützung.
- **UI direkt an GOP beziehungsweise VBE koppeln:** verhindert Wiederverwendung und erschwert Tests.

## 6. Validierung und Abnahmekriterien

- identische UI-Szenen auf beiden Plattformen ausführen,
- Screenshot-Vergleich mit definierter Toleranz,
- automatisierte Tests gegen simulierte Pixelformate,
- Eingabetests für Tastatur und Pointer,
- Start auf realer Legacy- und UEFI-Hardware.

## 7. Sicherheits- und Robustheitsanforderungen

- Ein Fehler innerhalb der grafischen Oberfläche darf den eigentlichen Bootvorgang nicht dauerhaft blockieren.
- Alle extern oder aus dem Dateisystem geladenen Daten müssen vor der Nutzung validiert werden.
- Speicherzugriffe müssen auf reservierte und überprüfte Bereiche begrenzt bleiben.
- Für jede optionale Funktion muss ein definierter Fallback existieren.
- Diagnoseinformationen dürfen keine geheimen Schlüssel, Passwörter oder geschützten Bootparameter offenlegen.

## 8. Abhängigkeiten

- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements

## 9. Revisionsverlauf

| Version | Datum | Änderung |
|---|---|---|
| 1.0 | 2026-08-03 | Erstfassung |
