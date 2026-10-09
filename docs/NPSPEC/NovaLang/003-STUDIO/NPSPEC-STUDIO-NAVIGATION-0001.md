
# NPSPEC-STUDIO-NAVIGATION-0001 – NovaLang Studio Navigation

## Status

Angenommen

## Kategorie

NovaLang Studio / Code Editor / Navigation

## Zweck

Definiert die zentrale Navigation innerhalb von NovaLang Studio.

Ziel ist ein schneller, direkter Wechsel zwischen Dateien, Symbolen, Projekten, Solutions, Logic-Graph-Knoten und UI-Komponenten.

Häufig benötigte Navigationsfunktionen müssen ohne umständliche Menüstrukturen erreichbar sein.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Navigation Engine | Zentrale Steuerung der Navigation |
| Symbol Navigator | Navigation zwischen Symbolen |
| File Navigator | Suche und Öffnen von Dateien |
| Reference Navigator | Auflösung von Symbolreferenzen |
| Workspace Navigator | Navigation zwischen Projekten und Solutions |
| Graph Navigator | Navigation innerhalb des Logic Graph |
| UI Navigator | Navigation zu UI-Komponenten und Bindungen |
| Navigation History | Verwaltung bisheriger Positionen |

Die Navigation Engine verwendet den zentralen Language Service und Workspace Manager.

## Navigationsfunktionen

NovaLang Studio unterstützt mindestens:

- Go to Definition
- Go to Declaration
- Go to Implementation
- Find All References
- Go to Type Definition
- Go to File
- Go to Symbol
- Go to Line
- Navigate Back
- Navigate Forward
- Navigate to Diagnostic
- Navigate to Logic Graph Node
- Navigate to UI Component

Alle grundlegenden Funktionen müssen per Tastatur erreichbar sein.

## Symbolnavigation

Symbole werden anhand ihrer semantischen Identität aufgelöst.

Unterstützt werden:

- Klassen und Strukturen
- Interfaces und Module
- Funktionen und Methoden
- Eigenschaften und Felder
- Variablen und Parameter
- Generische Typen
- Namespaces
- Projektübergreifende Referenzen

Die Navigation muss Gültigkeitsbereiche, Sichtbarkeitsregeln und Überladungen berücksichtigen.

## Go to Definition

Beim Auswählen eines Symbols wird dessen tatsächliche Definition geöffnet.

Beispiel:

```vb
Dim ergebnis = Berechnen(10)
```

Die Navigation auf `Berechnen` führt zur entsprechenden Funktionsdefinition.

Existieren mehrere mögliche Ziele, wird eine kompakte Auswahlliste angezeigt.

## Datei- und Symbolsuche

Die Navigation Engine ermöglicht die schnelle Suche innerhalb des aktuellen Workspace.

Suchergebnisse berücksichtigen:

- Dateinamen
- Symbolnamen
- Projektnamen
- Solution-Namen
- Namespaces
- Pfade
- Übereinstimmungsqualität

Ergebnisse werden während der Eingabe inkrementell aktualisiert.

## Navigation History

NovaLang Studio speichert Navigationspositionen innerhalb einer Sitzung.

Eine Position enthält:

- Dokumentidentität
- Dokumentversion beziehungsweise gültige Positionsreferenz
- Cursorposition
- Auswahlbereich
- Zugehörigen Editorbereich

Zurück- und Vorwärtsnavigation müssen unabhängig von Undo und Redo funktionieren.

Veraltete Positionen werden soweit möglich auf den aktuellen Dokumentzustand abgebildet.

## Workspace-Integration

Der Workspace Navigator unterstützt:

- Wechsel zwischen Projekten
- Wechsel zwischen Solutions
- Öffnen referenzierter Dateien
- Navigation zu Projektabhängigkeiten
- Anzeige zugehöriger Ressourcen

Nicht zugängliche oder fehlende Dateien müssen eindeutig gekennzeichnet werden.

## Logic-Graph-Integration

Die Navigation verbindet Quellcode und Logic Graph.

Unterstützt werden:

- Vom Graph-Knoten zum Custom Script
- Vom Custom Script zum Graph-Knoten
- Von einer Verbindung zu ihrem Typvertrag
- Von einem Capability-Knoten zur Schnittstellendokumentation
- Von einer Diagnose zum betroffenen Graph-Knoten

Graph-Knoten benötigen stabile Identitäten innerhalb der jeweiligen Solution.

## UI-Designer-Integration

Bei `.nui`-Dateien ermöglicht die Navigation:

- Vom Quellcode zur visuellen UI-Komponente
- Von der UI-Komponente zur Deklaration
- Von Datenbindungen zu ihren Quellen
- Von Ereignisbindungen zu ihren Handlern
- Von Diagnosen zu betroffenen UI-Elementen

Code Editor und UI Designer müssen denselben Dokumentzustand verwenden.

## Commandbar

Die systemweite NovaOS-Commandbar kann Navigationsbefehle bereitstellen.

Beispiele:

- `Datei öffnen`
- `Symbol suchen`
- `Zur Definition`
- `Referenzen anzeigen`
- `Logic Graph öffnen`

Die Navigation muss auch ohne KI vollständig funktionieren.

## Benutzeroberfläche

- Kompakte Such- und Ergebnislisten
- Direkt erreichbare Navigationsbefehle
- Tastatur- und Mausbedienung
- Breadcrumb-Navigation
- Vorschau von Suchergebnissen
- Hervorhebung der Zielposition
- Keine unnötigen Fensterwechsel

Die aktuelle Arbeitsposition soll beim Öffnen einer Vorschau erhalten bleiben.

## Performance

- Symbol- und Dateiindizes werden wiederverwendet.
- Suchanfragen werden inkrementell verarbeitet.
- Veraltete Anfragen müssen abbrechbar sein.
- Große Workspaces werden bedarfsgerecht durchsucht.
- Die Navigation darf die Benutzeroberfläche nicht blockieren.
- Speicherverbrauch und Ergebnisanzahl müssen begrenzbar sein.

## Sicherheit

- Navigation gewährt keine zusätzlichen Dateizugriffsrechte.
- Geschützte Symbole und Ressourcen dürfen nicht unautorisiert offengelegt werden.
- Capability-Referenzen stellen keine Berechtigungen dar.
- Externe Navigationsziele müssen anhand der NovaOS-Zugriffsregeln geprüft werden.
- Erweiterungen dürfen nur autorisierte Workspace-Informationen verwenden.

## Normative Anforderungen

1. NovaLang Studio MUSS eine zentrale Navigation Engine bereitstellen.
2. Symbolnavigation MUSS auf semantischen Sprachinformationen basieren.
3. Go to Definition, Find All References und Go to Implementation MÜSSEN unterstützt werden.
4. Datei-, Symbol- und Zeilennavigation MÜSSEN verfügbar sein.
5. Zurück- und Vorwärtsnavigation MÜSSEN unabhängig von Undo und Redo funktionieren.
6. Navigation MUSS projekt- und solutionübergreifend möglich sein, soweit Zugriffsrechte bestehen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN in das gemeinsame Navigationsmodell integriert sein.
8. Code Editor, Logic Graph und UI Designer MÜSSEN gegenseitige Navigation unterstützen.
9. Mehrdeutige oder nicht auflösbare Navigationsziele MÜSSEN eindeutig behandelt werden.
10. Such- und Navigationsoperationen MÜSSEN inkrementell und abbrechbar sein.
11. Navigationsfunktionen DÜRFEN keine Capability- oder Sicherheitsgrenzen umgehen.
12. Häufig verwendete Navigationsfunktionen MÜSSEN unmittelbar per Tastatur erreichbar sein.
13. Die vollständige grundlegende Navigation MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine zentrale, schnelle und semantisch präzise Navigation für Dateien, Symbole, Projekte, Solutions, Logic Graph und UI-Komponenten.

Entwickler können unmittelbar zwischen zusammengehörigen Elementen wechseln, ohne ihre Arbeitsumgebung oder den aktuellen Kontext unnötig verlassen zu müssen.
