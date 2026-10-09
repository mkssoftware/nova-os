
# NPSPEC-STUDIO-UI-DESIGNER-0001 – NovaLang Studio UI Designer

## Status

Angenommen

## Kategorie

NovaLang Studio / UI Designer

## Zweck

Definiert den visuellen Designer zur Erstellung und Bearbeitung deklarativer NovaOS-Benutzeroberflächen.

Ziel ist die direkte Gestaltung moderner, responsiver Oberflächen ohne manuelle Quellcodebearbeitung, wobei visuelle Darstellung und `.nui`-Quelltext vollständig synchron bleiben.

## Architektur

Der UI Designer besteht aus:

- **Design Surface:** Visuelle Bearbeitungsfläche.
- **Component Toolbox:** Verfügbare UI-Komponenten.
- **Property Inspector:** Bearbeitung von Eigenschaften und Ereignissen.
- **Layout Engine:** Positionierung und responsives Layout.
- **Binding Editor:** Verknüpfung von UI und Datenquellen.
- **Preview Engine:** Darstellung und Interaktion in Echtzeit.
- **NUI Synchronizer:** Bidirektionale Synchronisierung zwischen Designer und `.nui`.

Der Designer verwendet denselben Renderer und dieselbe deklarative NovaLang-Semantik wie die NovaOS-Runtime.

## Funktionen

Unterstützt werden:

- Drag-and-Drop von UI-Komponenten
- Visuelle Layoutbearbeitung
- Eigenschaften und Styles
- Daten- und Ereignisbindungen
- Responsive Layouts und Skalierung
- Komponenten- und Vorlagenverwaltung
- Undo und Redo
- Live Preview
- Direkter Wechsel zwischen Designer und Quellcode

Die Oberfläche folgt der NovaOS-Designsprache und ermöglicht schnellen Zugriff auf häufig verwendete Werkzeuge.

## NUI-Integration

`.nui` verwendet exakt die entsprechende deklarative NovaLang-Syntax und Typsemantik.

- Visuelle Änderungen aktualisieren die deklarative Beschreibung.
- Quellcodeänderungen aktualisieren die Designansicht.
- Bindungen werden über den gemeinsamen Language Service geprüft.
- Unbekannte oder nicht darstellbare Konstrukte dürfen nicht stillschweigend entfernt werden.
- UI-Artefakte müssen versioniert und deterministisch rekonstruierbar sein.

## Solution-Integration

Der UI Designer ist Bestandteil des NovaOS-Solution-Workflows.

UI-Komponenten können mit Logic-Graph-Eingängen, Ausgängen und Ereignissen verbunden werden.

Systemzugriffe erfolgen ausschließlich über autorisierte Capabilities. UI-Bindungen selbst gewähren keine Berechtigungen.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten visuellen UI Designer bereitstellen.
2. Designer und `.nui`-Quelltext MÜSSEN bidirektional synchronisiert werden.
3. Beide Ansichten MÜSSEN denselben Dokumentzustand verwenden.
4. Komponenten, Eigenschaften, Layouts und Bindungen MÜSSEN visuell bearbeitbar sein.
5. Responsive Layouts und Live Preview MÜSSEN unterstützt werden.
6. Die Vorschau MUSS in einem kontrollierten Ausführungskontext erfolgen.
7. Änderungen MÜSSEN rückgängig gemacht und wiederhergestellt werden können.
8. Nicht unterstützte Konstrukte DÜRFEN nicht ohne ausdrückliche Zustimmung verändert oder verworfen werden.
9. Logic-Graph- und Capability-Verbindungen MÜSSEN die NovaOS-Sicherheitsregeln einhalten.
10. Generierte Oberflächen MÜSSEN ohne KI reproduzierbar und ausführbar sein.

## Ergebnis

NovaLang Studio erhält einen visuellen, quellcodesynchronen UI Designer, mit dem sich NovaOS-Oberflächen schnell erstellen, anpassen und direkt in Solutions integrieren lassen.
