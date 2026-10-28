# NPSPEC-UI-DESIGNER-0001 – Nova Visual UI Designer Integration

**Status:** Angenommen  
**Bereich:** UI / Designer / NovaStudio  
**System:** NovaOS

## 1. Zweck

Nova Visual UI Designer Integration definiert die Integration eines visuellen UI-Designers in NovaStudio.

Ziel ist die grafische Erstellung, Bearbeitung und Vorschau nativer NovaOS-Benutzeroberflächen, ohne dass deren deklarativer Quelltext manuell geschrieben werden muss.

Der Designer verwendet dasselbe UI-Modell wie die Nova UI Runtime. Visuelle Bearbeitung und Quelltextbearbeitung sind gleichwertige Zugriffswege auf dieselbe Benutzeroberfläche.

## 2. Architektur

Der Visual UI Designer umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Designer Host | Integration in NovaStudio |
| Design Surface | Visuelle Bearbeitungsfläche |
| UI Document Model | Gemeinsames deklaratives UI-Modell |
| Component Toolbox | Verfügbare Controls und Layouts |
| Property Inspector | Bearbeitung von Eigenschaften |
| Visual Tree Explorer | Darstellung der UI-Hierarchie |
| Layout Editor | Visuelle Layoutbearbeitung |
| Resource Editor | Bearbeitung von Styles und Ressourcen |
| Preview Runtime | Ausführung der UI-Vorschau |
| Designer Serializer | Lesen und Schreiben von `.nui`-Dateien |

Der Designer MUSS die bestehenden Komponenten des Nova UI Frameworks verwenden.

Eine separate UI-Technologie für die Designer-Vorschau ist nicht vorgesehen.

## 3. Gemeinsames UI-Modell

Der Designer arbeitet auf dem Nova Declarative UI Model.

Dabei gilt:

- `.nui` ist das maßgebliche Format für die deklarative Benutzeroberfläche.
- Visuelle Änderungen MÜSSEN in das UI-Dokument übernommen werden.
- Änderungen am Quelltext MÜSSEN im Designer sichtbar werden.
- Beide Bearbeitungsformen MÜSSEN dieselben Controls, Eigenschaften und Ressourcen unterstützen.
- Unbekannte, aber gültige Erweiterungen DÜRFEN beim Speichern nicht verloren gehen.
- Die Serialisierung SOLL stabile, nachvollziehbare Änderungen erzeugen.

Der Designer DARF keine zweite, inkompatible UI-Beschreibung einführen.

## 4. Design Surface

Die Design Surface stellt die Benutzeroberfläche während der Entwicklung visuell dar.

Unterstützt werden:

- Auswahl einzelner und mehrerer Elemente.
- Hinzufügen und Entfernen von Controls.
- Verschieben und Anordnen von Elementen.
- Bearbeitung von Größen und Abständen.
- Ausrichtung und Verteilung.
- Drag & Drop aus der Component Toolbox.
- Zoom und Navigation.
- Kontextabhängige Bearbeitungswerkzeuge.

Dabei gilt:

- Die Darstellung MUSS dem Verhalten der Nova UI Runtime möglichst genau entsprechen.
- Layoutregeln MÜSSEN bei visuellen Änderungen erhalten bleiben.
- Der Designer MUSS zwischen Bearbeitungsaktionen und tatsächlichen UI-Interaktionen unterscheiden.

## 5. Layoutbearbeitung

Der Designer unterstützt die visuellen Layoutsysteme von NovaOS.

Dazu gehören:

- Grid Layout.
- Stack Layout.
- Dock Layout.
- Adaptive Layout.
- Verschachtelte Layoutcontainer.

Dabei gilt:

- Layoutänderungen MÜSSEN über die Nova Layout Engine ausgewertet werden.
- Grid-Zeilen und -Spalten MÜSSEN visuell bearbeitbar sein.
- Abstände, Ausrichtung und Größenregeln MÜSSEN über den Property Inspector zugänglich sein.
- Adaptive Layouts MÜSSEN bei unterschiedlichen Fenstergrößen geprüft werden können.
- Der Designer DARF keine unnötigen absoluten Positionierungen erzwingen.

## 6. Controls und Komponenten

Die Component Toolbox stellt verfügbare UI-Komponenten bereit.

Dabei gilt:

- Nova Standard Controls MÜSSEN verfügbar sein.
- Registrierte Custom Controls MÜSSEN integrierbar sein.
- Control Templates und Data Templates MÜSSEN unterstützt werden.
- Controls MÜSSEN ihre editierbaren Eigenschaften bereitstellen können.
- Fehlende oder nicht ladbare Komponenten MÜSSEN kontrolliert dargestellt werden.

Custom Controls SOLLEN optionale Design-Time-Metadaten bereitstellen können.

## 7. Property Inspector

Der Property Inspector ermöglicht die Bearbeitung von UI-Eigenschaften.

Unterstützt werden:

- Standardwerte und lokale Werte.
- Farben, Größen und Abstände.
- Styles und Templates.
- Resource Bindings.
- Data Bindings.
- Events und Commands.
- Accessibility-Eigenschaften.
- Animationen und Transitions.

Dabei gilt:

- Eigenschaften MÜSSEN entsprechend ihrem Datentyp bearbeitet werden.
- Ungültige Werte MÜSSEN erkannt werden.
- Änderungen MÜSSEN unmittelbar in der Vorschau erscheinen.
- Property-Prioritäten und Bindings DÜRFEN nicht unbeabsichtigt überschrieben werden.

## 8. Quelltext- und Designer-Synchronisation

NovaStudio unterstützt die Bearbeitung derselben `.nui`-Datei im visuellen Designer und im Quelltexteditor.

Dabei gilt:

- Änderungen MÜSSEN bidirektional synchronisiert werden.
- Die Synchronisation MUSS auf einem gemeinsamen Dokumentmodell beruhen.
- Ungültiger Quelltext DARF nicht zum Verlust des letzten gültigen Designerzustands führen.
- Nicht interpretierbare Dokumentbereiche MÜSSEN nachvollziehbar gekennzeichnet werden.
- Undo und Redo MÜSSEN beide Bearbeitungsformen berücksichtigen.

Die Synchronisation SOLL inkrementell erfolgen.

## 9. Events, Commands und Logic Graph

Der Designer integriert sich in NovaLang und den NovaStudio Logic Graph.

Dabei gilt:

- Events MÜSSEN über den Property Inspector zugewiesen werden können.
- Event Handler MÜSSEN im NovaLang-Editor geöffnet werden können.
- Commands und Actions MÜSSEN visuell gebunden werden können.
- Logic-Graph-Verknüpfungen MÜSSEN unterstützt werden.
- Bestehende Event Handler DÜRFEN nicht unbeabsichtigt überschrieben werden.

Ein Doppelklick auf ein geeignetes Control SOLL dessen Standardereignis öffnen oder einen entsprechenden Handler erzeugen.

Die Ereignissyntax richtet sich nach der NovaLang-Sprachdefinition.

## 10. Styles, Themes und Ressourcen

Der Designer verwendet das Nova UI Styling System.

Dabei gilt:

- Styles und Templates MÜSSEN visuell bearbeitbar sein.
- Resource Dictionaries MÜSSEN eingebunden werden können.
- Themewechsel MÜSSEN in der Vorschau möglich sein.
- Light, Dark und High Contrast MÜSSEN geprüft werden können.
- Dynamische Ressourcen MÜSSEN zur Laufzeit korrekt aufgelöst werden.

## 11. Vorschau und Laufzeitverhalten

Die Preview Runtime stellt die Benutzeroberfläche unter kontrollierten Entwicklungsbedingungen dar.

Dabei gilt:

- Vorschau und tatsächliche Anwendung MÜSSEN dieselbe UI Runtime verwenden.
- Unterschiedliche Fenstergrößen und Skalierungsfaktoren MÜSSEN simulierbar sein.
- Visual States MÜSSEN gezielt dargestellt werden können.
- Data Bindings MÜSSEN mit geeigneten Testdaten prüfbar sein.
- Animationen und Transitions MÜSSEN in der Vorschau unterstützt werden.
- Die Vorschau DARF keine unautorisierten Systemaktionen ausführen.

Design-Time-Daten MÜSSEN von produktiven Anwendungsdaten getrennt bleiben.

## 12. Accessibility

Der Designer integriert die Nova Accessibility Architecture.

Dabei gilt:

- Accessibility-Eigenschaften MÜSSEN bearbeitbar sein.
- Der Accessibility Tree SOLL visualisiert werden können.
- Fokusreihenfolge und Tastaturbedienbarkeit MÜSSEN prüfbar sein.
- High Contrast und Reduced Motion MÜSSEN simulierbar sein.
- Fehlende zugängliche Beschriftungen SOLLEN erkannt werden.

## 13. Sicherheit und Isolation

Die Designer-Vorschau unterliegt Nova.Trust.

Dabei gilt:

- Fremde Custom Controls MÜSSEN innerhalb eines kontrollierten Sicherheitskontexts ausgeführt werden.
- Design-Time-Code DARF keine zusätzlichen Berechtigungen erhalten.
- Fehlerhafte Komponenten DÜRFEN NovaStudio nicht unkontrolliert beenden.
- Die Vorschau MUSS bei Bedarf isoliert neu gestartet werden können.
- Die grundlegende Designerfunktion MUSS vollständig offline verfügbar sein.

## 14. Performance

Der Designer MUSS auch bei komplexen Benutzeroberflächen reaktionsfähig bleiben.

Dabei gilt:

- Änderungen SOLLEN inkrementell verarbeitet werden.
- Unveränderte Layoutbereiche SOLLEN nicht erneut berechnet werden.
- Damage Tracking MUSS für die Vorschau genutzt werden.
- Der Designer DARF keine permanente vollständige Neuzeichnung erzwingen.
- Aufwendige Analyseaufgaben SOLLEN die UI Event Loop nicht blockieren.

## 15. Normative Anforderungen

- **UI-001:** NovaStudio MUSS einen nativen visuellen UI-Designer bereitstellen.
- **UI-002:** Der Designer MUSS das Nova Declarative UI Model verwenden.
- **UI-003:** `.nui` MUSS als gemeinsames UI-Dokumentformat unterstützt werden.
- **UI-004:** Visuelle und textuelle Bearbeitung MÜSSEN bidirektional synchronisiert werden.
- **UI-005:** Standard Controls und Custom Controls MÜSSEN integrierbar sein.
- **UI-006:** Die Nova Layout Engine MUSS für die Layoutvorschau verwendet werden.
- **UI-007:** UI-Eigenschaften MÜSSEN visuell bearbeitbar sein.
- **UI-008:** Styles, Templates und Resource Bindings MÜSSEN unterstützt werden.
- **UI-009:** Events und Commands MÜSSEN mit NovaLang und Logic Graph verknüpfbar sein.
- **UI-010:** Die Vorschau MUSS dieselbe UI Runtime wie die Anwendung verwenden.
- **UI-011:** Unterschiedliche Themes und Skalierungen MÜSSEN simulierbar sein.
- **UI-012:** Accessibility-Eigenschaften MÜSSEN prüfbar sein.
- **UI-013:** Undo und Redo MÜSSEN designerübergreifend funktionieren.
- **UI-014:** Ungültige Dokumentänderungen DÜRFEN keine gültigen UI-Daten zerstören.
- **UI-015:** Design-Time-Komponenten MÜSSEN den Sicherheitsregeln von Nova.Trust unterliegen.
- **UI-016:** Die Vorschau MUSS bedarfsgesteuerte Renderaktualisierungen verwenden.
- **UI-017:** Der Designer MUSS unabhängig von einem konkreten Renderbackend aufgebaut sein.
- **UI-018:** Nova Visual UI Designer Integration MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Ergebnis

NovaStudio erhält einen integrierten visuellen Designer zur Erstellung nativer NovaOS-Benutzeroberflächen.

Entwickler können Controls, Layouts, Styles, Bindings und Ereignisse grafisch bearbeiten oder direkt im deklarativen Quelltext definieren.

Beide Bearbeitungsformen verwenden dasselbe UI-Modell und dieselbe Runtime.

**Grundsatz:** Der visuelle Designer und der Quelltexteditor sind zwei Ansichten derselben Benutzeroberfläche – keine voneinander getrennten Entwicklungssysteme.