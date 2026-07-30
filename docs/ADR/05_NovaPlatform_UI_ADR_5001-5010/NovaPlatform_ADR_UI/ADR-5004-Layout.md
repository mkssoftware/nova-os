# ADR-5004: Layout-Engine

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Nova UI (NUI)
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses ADR definiert die Architekturentscheidung für **Layout-Engine** innerhalb des Nova UI Frameworks.

## 2. Problemstellung

Die Benutzeroberfläche soll in Boot Manager, Nova Rescue und NovaOS konsistent funktionieren und gleichzeitig moderne Effekte, Barrierefreiheit und unterschiedliche Eingabegeräte unterstützen.

## 3. Entscheidung

Die NUI besteht aus einem modularen Widget-System mit klar getrennten Schichten:
- Window Manager
- Widget Framework
- Layout Engine
- Event System
- Theme Engine

Alle Anwendungen greifen ausschließlich über die öffentliche NUI-API auf diese Komponenten zu.

## 4. Begründung

- Wiederverwendbare Widgets
- Einheitliches Verhalten
- Trennung von Logik, Layout und Darstellung
- Gute Erweiterbarkeit

## 5. Konsequenzen

### Positiv
- Konsistentes UI
- Austauschbare Implementierungen
- Gute Testbarkeit

### Risiken
- Zusätzliche Abstraktion
- API muss stabil bleiben

## 6. Auswirkungen

Dieses ADR beeinflusst alle Module, die **Layout-Engine** verwenden.

## 7. Referenzen

- RFC-0001
- ADR-0012
- ADR-0015
- ADR-0019

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
