# ADR-BOOTUI-0010 – Gemeinsame Rendering-API für Bootmanager und NovaOS

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-BOOTUI-0010 |
| Titel | Gemeinsame Rendering-API für Bootmanager und NovaOS |
| Kategorie | NovaOS Bootmanager / Grafikarchitektur |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Verantwortlich | NovaOS Architecture |
| Gültigkeitsbereich | BIOS- und UEFI-Bootmanager |

## 1. Entscheidungskontext

Bootmanager und NovaOS sollen visuell eng zusammenpassen. Zwei vollständig unterschiedliche Renderingmodelle würden doppelte Konzepte, abweichende Effekte und zusätzlichen Aufwand bei Design und Tests verursachen. Eine direkte gemeinsame Binärimplementierung ist wegen der unterschiedlichen Laufzeitumgebungen jedoch nicht sinnvoll.

## 2. Entscheidung

Bootmanager und NovaOS verwenden einen gemeinsamen, versionierten Rendering-Vertrag auf Quell- und Datenmodellebene. Der Vertrag beschreibt primitive Zeichenoperationen, Farbmodelle, Geometrie, Transformationen, Compositing, Textmetriken, Design-Tokens und Ressourcenformate.

Die Implementierungen bleiben getrennt:

- Bootmanager: kleine freestanding Referenzimplementierung,
- NovaOS: vollständige, gegebenenfalls hardwarebeschleunigte Systemimplementierung.

## 3. Technische Ausgestaltung

### Gemeinsamer Kern

- Farb- und Alpha-Konventionen,
- Rechtecke, Radien, Pfade und Transformationen,
- Surface- und Layerbegriffe,
- Blend-Modi der Baseline,
- Textmessung und Glyphenpositionierung,
- Design-Token-IDs,
- Asset-Metadaten,
- Fehler- und Capability-Codes.

### Versionierung

Der Vertrag erhält eine explizite Major- und Minor-Version. Erweiterungen müssen feature-erkennbar sein. Der Bootmanager darf nur die von ihm unterstützte Teilmenge akzeptieren.

### Referenzbilder

Zentrale Komponenten erhalten gemeinsame Referenzszenen. Abweichungen zwischen Boot- und Systemrenderer werden anhand definierter Toleranzen bewertet.

## 4. Konsequenzen

### Positive Folgen

- hohe visuelle Konsistenz,
- gemeinsame Tools und Asset-Pipeline,
- weniger widersprüchliche Konzepte,
- Referenztests über beide Laufzeiten.

### Negative Folgen

- gemeinsame Spezifikation muss sehr stabil sein,
- Bootrestriktionen können den Baseline-Vertrag begrenzen,
- zwei Implementierungen müssen synchron gehalten werden.

## 5. Verworfene Alternativen

- **Vollständig getrennte APIs:** hohe Driftgefahr.
- **Gemeinsame Binärbibliothek:** wegen Laufzeit- und Treiberunterschieden ungeeignet.
- **Bootmanager direkt mit Desktop-Renderer:** zu groß und abhängig.

## 6. Validierung und Abnahmekriterien

- gemeinsame Conformance-Tests,
- identische Referenzszenen,
- Versions- und Capability-Tests,
- Asset- und Token-Kompatibilität,
- Nachweis, dass beide Implementierungen unabhängig booten beziehungsweise starten.

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
