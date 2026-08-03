# ADR-BOOTUI-0009 – Skalierung anhand Auflösung und Pixeldichte

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0009 |
| Titel | Skalierung anhand Auflösung und Pixeldichte |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Die physische Pixeldichte ist in BIOS- und UEFI-Umgebungen häufig unbekannt oder fehlerhaft. Ausschließlich DPI-basierte Skalierung ist daher nicht zuverlässig. Eine reine Auflösungsskalierung kann auf kleinen hochauflösenden Displays dennoch zu kleinen Elementen führen.

## 2. Entscheidung

NovaOS verwendet eine mehrstufige Skalierungsstrategie:

1. verlässliche physische Displayinformationen verwenden,
2. andernfalls Skalierung aus Auflösung und typischer Displayklasse ableiten,
3. benutzer- oder systemseitig gespeicherte Präferenz berücksichtigen,
4. Mindestgrößen für Text, Fokus und Bedienelemente erzwingen.

Die berechnete Skalierung wird auf definierte Stufen quantisiert, beispielsweise 100 %, 125 %, 150 %, 175 % und 200 %.

## 3. Technische Ausgestaltung

### Eingangsdaten

- horizontale und vertikale Auflösung,
- optional physische Displaygröße,
- Seitenverhältnis,
- Firmwareinformationen,
- gespeicherte NovaOS-Anzeigepräferenz,
- Barrierefreiheitsprofil.

### Heuristik

Fehlen physische Maße, wird eine konservative Skalierung gewählt. Kritische Texte und Controls dürfen niemals unter ihre Mindestgröße fallen.

### Moduswechsel

Wird die Grafikauflösung während der Laufzeit geändert, werden Skalierung, Layout, Glyphen-Atlas und Dirty Regions vollständig neu berechnet.

### Begrenzung

Extremwerte werden geklemmt. Eine Skalierung darf nicht dazu führen, dass primäre Bootaktionen außerhalb des sichtbaren Bereichs liegen.

## 4. Konsequenzen

### Positive Folgen

- bessere Lesbarkeit auf vielen Displayklassen,
- robuste Funktion trotz fehlender DPI-Daten,
- konsistente Designwirkung,
- Unterstützung von Barrierefreiheitsprofilen.

### Negative Folgen

- heuristische Entscheidungen können im Einzelfall unpassend sein,
- zusätzliche Testmatrix,
- Neuberechnung bei Moduswechsel erforderlich.

## 5. Verworfene Alternativen

- **Nur DPI verwenden:** im frühen Boot oft nicht zuverlässig.
- **Nur Auflösung verwenden:** ignoriert physische Displaygröße.
- **Feste 100-Prozent-Skalierung:** auf modernen Displays unzureichend.

## 6. Validierung und Abnahmekriterien

- Testmatrix aus Auflösung, Displaygröße und Seitenverhältnis,
- fehlerhafte oder fehlende Firmwarewerte,
- Prüfung aller quantisierten Skalierungsstufen,
- Mindestgrößen und Sichtbarkeit kritischer Aktionen.

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
