
# NPSPEC-STUDIO-GRAPH-SCRIPT-EDITOR-0001 – NovaLang Studio Graph Script Editor

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Script Editor

## Zweck

Definiert die Integration des NovaLang-Codeeditors in den Logic Graph Editor.

Ziel ist die direkte Entwicklung und Bearbeitung von Custom Scripts innerhalb einer Solution, ohne zwischen getrennten Entwicklungsumgebungen wechseln zu müssen.

## Architektur

Der Graph Script Editor besteht aus:

- **Script Editor Bridge:** Verbindung zum regulären NovaLang Editor.
- **Script Node Manager:** Zuordnung von Scripts zu Graphknoten.
- **Port Binding Manager:** Verknüpfung von Script-Parametern mit Graphports.
- **Language Service Bridge:** Integration von IntelliSense und Typprüfung.
- **Compiler Bridge:** Kompilierung über den NovaLang Compiler.
- **Debug Bridge:** Gemeinsames Debugging von Script und Graph.

Es wird kein eigener Script-Compiler oder NovaLang-Dialekt eingeführt.

## Script-Modell

Custom Scripts verwenden `.nlf` und die reguläre NovaLang-Syntax.

Ein Script Node besitzt:

- Eindeutige Node-ID
- Referenz auf die Scriptdatei
- Typisierte Eingangs- und Ausgangsports
- Definierte Ausführungsschnittstelle
- Optionale Konfigurationsparameter

Scripts erhalten ihre Eingabedaten ausschließlich über die definierten Ports.

## Bedienung

Der Editor unterstützt:

- Script durch Doppelklick auf einen Knoten öffnen
- Codebearbeitung innerhalb von NovaLang Studio
- Syntaxhervorhebung und IntelliSense
- Autovervollständigung und Refactoring
- Fehleranzeige während der Eingabe
- Navigation zwischen Code und Graph
- Gemeinsame Undo/Redo-Integration mit getrennten Dokumentverläufen

Der Script Editor kann als Registerkarte oder integrierter Arbeitsbereich dargestellt werden.

## Port-Integration

Script-Eingaben und -Ausgaben werden über typisierte Schnittstellen definiert.

Änderungen an der Script-Schnittstelle müssen die zugehörigen Graphports aktualisieren.

Bestehende Verbindungen werden anschließend erneut validiert.

Inkompatible Änderungen dürfen nicht unbemerkt übernommen werden.

## Capability-Sicherheit

Custom Scripts dürfen keine System-Capabilities selbstständig anfordern oder aufrufen.

Benötigte Systemfunktionen werden ausschließlich durch vorgeschaltete Capability Nodes bereitgestellt.

Das Script verarbeitet nur die über seine Ports übergebenen Daten und autorisierten Ressourcen.

## Kompilierung und Debugging

Scripts werden mit dem regulären NovaLang Compiler übersetzt.

Compilerfehler werden sowohl im Codeeditor als auch am zugehörigen Script Node angezeigt.

Breakpoints und Einzelschritte müssen über die gemeinsame Graph- und NovaLang-Debugging-Infrastruktur funktionieren.

## Speicherung

Scriptquellen werden als `.nlf`-Dateien innerhalb der Solution verwaltet.

Der Logic Graph speichert die Scriptreferenz und den zugehörigen Schnittstellenvertrag.

Änderungen müssen mit Solution-Versionierung, Integritätsprüfung und Build-System synchronisiert werden.

## Normative Anforderungen

1. Der Logic Graph Editor MUSS Custom Scripts direkt öffnen und bearbeiten können.
2. `.nlf` MUSS die reguläre NovaLang-Syntax verwenden.
3. Der Script Editor MUSS den bestehenden NovaLang Language Service verwenden.
4. Script-Parameter MÜSSEN typisierten Graphports zugeordnet werden.
5. Schnittstellenänderungen MÜSSEN eine erneute Graphvalidierung auslösen.
6. Scripts MÜSSEN über den regulären NovaLang Compiler verarbeitet werden.
7. Compiler- und Laufzeitfehler MÜSSEN im Script und Graph sichtbar sein.
8. Graph- und Script-Debugging MÜSSEN integriert sein.
9. Custom Scripts DÜRFEN keine Capabilities selbstständig anfordern oder aufrufen.
10. Systemzugriffe MÜSSEN über autorisierte Capability Nodes erfolgen.
11. Scriptdateien und Graphreferenzen MÜSSEN konsistent gespeichert werden.
12. Der Script Editor MUSS ohne KI vollständig funktionsfähig sein.

## Ergebnis

NovaLang Studio erhält eine nahtlose Integration von NovaLang-Scripts in den Logic Graph Editor, mit gemeinsamer Typprüfung, Kompilierung und Debugging bei konsequenter Trennung zwischen Scriptlogik und System-Capabilities.
