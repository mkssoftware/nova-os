# ADR-BOOTUI-0011 – Begrenzte Wiederverwendung der Nova Design Engine

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0011 |
| Titel | Begrenzte Wiederverwendung der Nova Design Engine |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Der Bootmanager soll bereits wie NovaOS wirken, kann aber nicht die vollständige Desktop-Design- und UI-Laufzeit übernehmen. Eine vollständige Wiederverwendung würde zu übermäßiger Größe, Abhängigkeiten und schwer kontrollierbarem Verhalten führen. Gar keine Wiederverwendung würde visuelle Inkonsistenz erzeugen.

## 2. Entscheidung

Aus der Nova Design Engine werden ausschließlich deklarative und statisch auswertbare Teile wiederverwendet. Dazu gehören:

- Design-Tokens,
- Farbrollen,
- Typografierollen,
- Abstände und Radien,
- Elevation- und Schattenstufen,
- Motion-Tokens,
- Icon-Metadaten,
- Komponentenmetriken,
- vereinfachte Materialdefinitionen.

Nicht wiederverwendet werden:

- Desktop-Windowing,
- dynamische Stylesheets,
- Skript- oder Plugin-Laufzeiten,
- komplexe Shader,
- Accessibility-Dienste des laufenden Systems,
- dynamische Theme-Erweiterungen.

## 3. Technische Ausgestaltung

### Exportpipeline

Die vollständige Nova Design Engine erzeugt ein kompaktes, versioniertes Boot-Design-Paket. Dieses enthält nur validierte Werte und vorverarbeitete Ressourcen.

### Unterstützte Komponenten

Nur explizit spezifizierte Bootkomponenten dürfen exportiert werden, beispielsweise Button, Liste, Boot-Tile, Dialog, Fortschrittsanzeige und Statusbadge.

### Fallback

Jedes Material besitzt eine vereinfachte Darstellung. Glass kann beispielsweise auf transparente oder opake Flächen mit Rand und Schatten zurückfallen.

### Unabhängigkeit

Der Bootmanager muss auch dann funktionieren, wenn kein externes Designpaket geladen werden kann. Ein internes Basistheme ist verpflichtend.

## 4. Konsequenzen

### Positive Folgen

- klare visuelle Verwandtschaft mit NovaOS,
- gemeinsame Designentscheidungen,
- kleine und überprüfbare Bootressourcen,
- kein Kopieren kompletter Desktopabhängigkeiten.

### Negative Folgen

- Exportpipeline muss gepflegt werden,
- nicht alle Desktop-Effekte sind im Boot identisch,
- Komponenten müssen bewusst auf eine Boot-Teilmenge begrenzt werden.

## 5. Verworfene Alternativen

- **Vollständige Wiederverwendung:** technisch zu schwergewichtig.
- **Manuelle Nachbildung:** hohe Driftgefahr.
- **Eigenständiges Bootdesign:** schwächere Marken- und Bedienkonsistenz.

## 6. Validierung und Abnahmekriterien

- Token- und Paketversionen prüfen,
- Referenzvergleich zwischen Boot- und Desktopkomponenten,
- Tests ohne externes Designpaket,
- Prüfung aller Material-Fallbacks,
- Größenlimit des exportierten Pakets.

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
