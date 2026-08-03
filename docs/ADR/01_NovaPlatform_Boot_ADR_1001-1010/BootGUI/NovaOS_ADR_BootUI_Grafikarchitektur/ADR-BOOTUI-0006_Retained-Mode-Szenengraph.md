# ADR-BOOTUI-0006 – Retained-Mode-Szenengraph

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0006 |
| Titel | Retained-Mode-Szenengraph |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Die Bootoberfläche enthält mehrere Ansichten, verschachtelte Controls, modale Dialoge, Animationen und Zustandsübergänge. Eine rein unmittelbare Zeichenlogik würde Layout, Fokus, Hit-Testing und inkrementelles Rendering stark erschweren.

## 2. Entscheidung

Die Boot-UI verwendet einen Retained-Mode-Szenengraphen. UI-Elemente bleiben als Objekte mit Eigenschaften, Hierarchie, Zustand und Renderinformationen erhalten.

Jeder Knoten besitzt mindestens:

- Typ und stabile ID,
- Parent- und Child-Beziehungen,
- Position und Größe,
- Sichtbarkeit und Aktivierungszustand,
- Transformationsdaten,
- Opazität,
- Clip-Informationen,
- Style- und Design-Token-Referenzen,
- Dirty Flags,
- Eingabe- und Fokusmetadaten.

## 3. Technische Ausgestaltung

### Phasen

1. Zustandsänderung der Anwendung.
2. Aktualisierung betroffener UI-Knoten.
3. Layoutphase.
4. Animationsupdate.
5. Ermittlung beschädigter Bereiche.
6. Render-Traversierung.
7. Präsentation.

### Knotenklassen

- Container,
- Layout-Container,
- Text,
- Bild und Icon,
- Shape,
- Control,
- Overlay,
- Dialog,
- Effekt- oder Surface-Knoten.

### Speicherverwaltung

Knoten werden bevorzugt aus festen Pools oder Arenen erzeugt. Unbegrenzte Objektallokationen sind nicht zulässig. Szenen besitzen definierte Höchstzahlen für Knoten, Ebenen und Verschachtelungstiefe.

### Ereignisse

Eingabeereignisse werden über Hit-Testing einem Zielknoten zugeordnet und können kontrolliert entlang der Hierarchie weitergegeben werden.

## 4. Konsequenzen

### Positive Folgen

- saubere Trennung von Zustand und Rendering,
- natürliche Unterstützung für Dialoge und Animationen,
- effizientes Dirty-Region-Rendering,
- zentrale Fokus-, Layout- und Eingabelogik,
- bessere Testbarkeit.

### Negative Folgen

- höherer Speicherbedarf als bei Immediate Mode,
- komplexere Lebenszyklus- und Invalidierungslogik,
- Gefahr zyklischer oder inkonsistenter Zustände.

## 5. Verworfene Alternativen

- **Immediate Mode:** einfacher Einstieg, aber ungeeignet für komplexe Szenen.
- **Fest codierte Bildschirmfunktionen:** nicht wiederverwendbar.
- **DOM-ähnliche vollständige Dokumentstruktur:** für den Bootmanager zu umfangreich.

## 6. Validierung und Abnahmekriterien

- Tests für Baumoperationen und Lebenszyklen,
- Layout- und Hit-Test-Tests,
- maximale Knotenanzahl ohne Speicherüberschreitung,
- korrekte Dirty-Propagation,
- Dialoge und Overlays dürfen keine Ereignisse an blockierte Ebenen durchlassen.

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
