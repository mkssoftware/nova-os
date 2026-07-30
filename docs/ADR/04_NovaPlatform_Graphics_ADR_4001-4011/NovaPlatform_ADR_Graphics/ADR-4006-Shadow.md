# ADR-4006: Shadow-Rendering

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Nova Graphics Engine (NGE)
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses ADR beschreibt die Architekturentscheidung für **Shadow-Rendering** innerhalb der Nova Graphics Engine (NGE).

## 2. Problemstellung

Die Grafikschicht muss Boot Manager, Nova Rescue und NovaOS mit derselben Rendering-API versorgen und gleichzeitig unterschiedliche Hardware-Backends unterstützen.

## 3. Entscheidung

Die NGE wird schichtweise aufgebaut:
- Rendering-API
- Surfaces
- Renderer
- Effekte
- GPU-/Framebuffer-Backend

Alle höheren Schichten verwenden ausschließlich die öffentliche NGE-API.

## 4. Begründung

- Einheitliche Rendering-Schnittstelle
- Austauschbare Backends
- Wiederverwendbar in Bootloader, Rescue und NovaOS
- Gute Testbarkeit

## 5. Konsequenzen

### Positiv
- Einheitliche Grafikarchitektur
- Klare Trennung zwischen API und Backend
- Erweiterbar um GPU-Beschleunigung

### Risiken
- Zusätzliche Abstraktionsebene
- Strikte API-Disziplin erforderlich

## 6. Auswirkungen

Dieses ADR beeinflusst alle Module, die **Shadow-Rendering** verwenden.

## 7. Referenzen

- RFC-0001
- ADR-0011
- ADR-0012

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
