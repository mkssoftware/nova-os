
# NPSPEC-STUDIO-NUI-EDITOR-0001 – NovaLang Studio NUI Editor

## Status

Angenommen

## Kategorie

NovaLang Studio / UI Designer / NUI Editor

## Zweck

Definiert den spezialisierten Quelltexteditor für deklarative NovaOS-Benutzeroberflächen im `.nui`-Format.

Ziel ist die präzise Bearbeitung von UI-Strukturen mit vollständiger Sprachunterstützung und direkter Synchronisierung zum visuellen UI Designer.

## Architektur

Der NUI Editor verwendet den zentralen Code Editor und erweitert ihn um:

- **NUI Language Support:** Syntaxanalyse und Typprüfung.
- **Component Intelligence:** Autovervollständigung für UI-Komponenten und Eigenschaften.
- **Binding Analyzer:** Prüfung von Daten- und Ereignisbindungen.
- **Structure Navigator:** Navigation durch die UI-Hierarchie.
- **Designer Bridge:** Synchronisierung mit dem visuellen UI Designer.
- **Preview Integration:** Aktualisierung der Live Preview.

Es wird kein eigenständiger Parser oder Compiler für `.nui` eingeführt.

## Sprachmodell

`.nui` verwendet exakt die entsprechende deklarative NovaLang-Syntax und Typsemantik.

- Gemeinsamer Lexer, Parser und Language Service.
- Kontextabhängige Prüfung zulässiger UI-Deklarationen.
- Typisierte Eigenschaften und Bindungen.
- Keine eigenständige Skriptsprache.
- Keine von NovaLang abweichende Ausdruckssemantik.

## Funktionen

- Syntax Highlighting
- IntelliSense und Autocompletion
- Echtzeitdiagnostik
- Code Folding
- Formatierung
- Symbolnavigation
- Refactoring
- Komponenten- und Eigenschaftssuche
- Direkter Wechsel zum UI Designer
- Live Preview

## Synchronisierung

NUI Editor und UI Designer verwenden denselben versionierten Dokumentzustand.

Änderungen werden inkrementell übertragen.

Nicht darstellbare oder fehlerhafte Konstrukte bleiben im Quelltext erhalten und werden diagnostiziert.

Visuelle Änderungen dürfen gültige manuelle Deklarationen nicht unbeabsichtigt überschreiben.

## Solution-Integration

Der Editor unterstützt typisierte Verweise auf:

- UI-Komponenten
- Datenmodelle
- Ereignisse
- Logic-Graph-Schnittstellen
- Verfügbare Capability-Verträge

Referenzen auf Capabilities gewähren keine Zugriffsberechtigungen.

## Normative Anforderungen

1. NovaLang Studio MUSS einen spezialisierten `.nui`-Editor bereitstellen.
2. Der Editor MUSS die gemeinsame NovaLang-Sprachinfrastruktur verwenden.
3. Syntax Highlighting, IntelliSense und Echtzeitdiagnostik MÜSSEN unterstützt werden.
4. UI-Komponenten, Eigenschaften und Bindungen MÜSSEN semantisch geprüft werden.
5. NUI Editor und UI Designer MÜSSEN denselben Dokumentzustand verwenden.
6. Änderungen MÜSSEN bidirektional und inkrementell synchronisiert werden.
7. Nicht unterstützte Konstrukte DÜRFEN nicht stillschweigend entfernt werden.
8. Navigation und Refactoring MÜSSEN mit dem zentralen Language Service integriert sein.
9. Die Live Preview MUSS ohne KI funktionieren.
10. Capability- und Sicherheitsgrenzen MÜSSEN eingehalten werden.

## Ergebnis

NovaLang Studio erhält einen vollständig integrierten NUI Editor, der deklarative Oberflächen mit derselben Präzision wie regulären NovaLang-Code bearbeitet und nahtlos mit dem visuellen UI Designer zusammenarbeitet.
