
# NPSPEC-STUDIO-REFACTORING-0001 – NovaLang Studio Refactoring

## Status

Angenommen

## Kategorie

NovaLang Studio / Code Editor / Refactoring

## Zweck

Definiert die automatisierte Umstrukturierung von NovaLang-Quellcode innerhalb von NovaLang Studio.

Ziel ist die sichere Verbesserung von Codequalität, Wartbarkeit und Struktur, ohne das definierte Programmverhalten unbeabsichtigt zu verändern.

Refactoring muss projektübergreifend sowie innerhalb von Solutions, Logic Graph und deklarativen Benutzeroberflächen funktionieren.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Refactoring Engine | Koordination der Umstrukturierung |
| Semantic Analyzer | Prüfung von Typen und Symbolen |
| Reference Resolver | Ermittlung betroffener Referenzen |
| Transformation Engine | Durchführung strukturierter Änderungen |
| Conflict Detector | Erkennung von Namens- und Typkonflikten |
| Change Preview | Vorschau aller Änderungen |
| Transaction Manager | Atomare Übernahme und Rücknahme |

Die Refactoring Engine verwendet den zentralen NovaLang Language Service.

## Refactoring-Funktionen

NovaLang Studio unterstützt mindestens:

- Rename Symbol
- Extract Method
- Extract Variable
- Extract Constant
- Inline Variable
- Inline Method
- Move Type
- Move Method
- Change Signature
- Encapsulate Field
- Organize Imports
- Remove Unused Imports
- Implement Interface
- Generate Constructor

Weitere Refactorings können über definierte Erweiterungsschnittstellen ergänzt werden.

## Semantische Analyse

Refactorings basieren auf aufgelösten Symbolidentitäten und Typinformationen.

Dabei werden berücksichtigt:

- Gültigkeitsbereiche
- Sichtbarkeitsregeln
- Typkompatibilität
- Generische Typen
- Überladungen
- Vererbungsbeziehungen
- Projekt- und Modulabhängigkeiten
- Referenzen in anderen Dokumenten

Reine Textersetzung darf nicht als semantisch sicheres Refactoring verwendet werden.

## Rename Symbol

Beim Umbenennen eines Symbols müssen alle eindeutig zugehörigen Referenzen berücksichtigt werden.

Beispiel:

```vb
Public Function Berechnen(a As Integer) As Integer
    Return a * 2
End Function
```

Wird `Berechnen` in `Verdoppeln` umbenannt, werden sämtliche aufgelösten Verwendungen entsprechend aktualisiert.

Nicht eindeutig auflösbare Referenzen müssen als mögliche Konflikte angezeigt werden.

Die Groß- und Kleinschreibungsregeln von NovaLang müssen berücksichtigt werden.

## Extract Method

Die Refactoring Engine kann ausgewählte Codebereiche in eine neue Methode auslagern.

Dabei müssen automatisch analysiert werden:

- Benötigte Eingabeparameter
- Rückgabewerte
- Lokale Variablen
- Seiteneffekte
- Kontrollfluss
- Async-Kontext
- Exception-Verhalten

Eine Extraktion darf die Ausführungsreihenfolge oder Ressourcenlebensdauer nicht unbeabsichtigt verändern.

## Projektübergreifendes Refactoring

Refactorings können mehrere Dokumente und Projekte betreffen.

- Betroffene Dateien werden vorab ermittelt.
- Abhängigkeiten werden überprüft.
- Änderungen werden gemeinsam vorbereitet.
- Konflikte müssen vor der Übernahme angezeigt werden.
- Nicht zugängliche Referenzen müssen als Einschränkung gemeldet werden.

Eine erfolgreiche Änderung darf nicht behauptet werden, wenn relevante Referenzen nicht geprüft werden konnten.

## Logic-Graph-Integration

Refactorings berücksichtigen `.nlf`-Dateien und Logic-Graph-Verbindungen.

Unterstützt werden insbesondere:

- Umbenennen von Custom-Script-Funktionen
- Anpassung typisierter Ein- und Ausgänge
- Aktualisierung von Graph-Referenzen
- Erkennung ungültiger Verbindungen
- Prüfung betroffener Capability-Verträge

Capability-Identitäten dürfen nicht durch gewöhnliche Symbolumbenennungen verändert werden.

## UI-Designer-Integration

Refactorings berücksichtigen `.nui`-Dateien.

Bei Änderungen werden überprüft:

- Datenbindungen
- Ereignisbindungen
- UI-Eigenschaften
- Komponentenreferenzen
- Referenzierte NovaLang-Symbole

Code Editor und UI Designer müssen nach der Änderung denselben Dokumentzustand verwenden.

## Änderungsvorschau

Vor der Übernahme größerer Refactorings wird eine Vorschau bereitgestellt.

Diese zeigt:

- Betroffene Dateien
- Hinzugefügte und entfernte Codebereiche
- Umbenannte Symbole
- Mögliche Konflikte
- Auswirkungen auf Logic Graph und UI

Der Benutzer kann Änderungen prüfen und bestätigen.

## Transaktionen und Undo

Refactorings werden als zusammenhängende Änderungstransaktionen ausgeführt.

- Alle betroffenen Dokumentversionen werden geprüft.
- Änderungen werden konsistent übernommen.
- Bei Fehlern erfolgt ein vollständiger Rollback.
- Erfolgreiche Refactorings müssen rückgängig gemacht werden können.
- Externe Dateiänderungen müssen vor dem Speichern berücksichtigt werden.

Teilweise angewendete Refactorings dürfen keine unbemerkten Inkonsistenzen hinterlassen.

## Validierung

Nach einem Refactoring werden betroffene Bereiche erneut analysiert.

Geprüft werden:

- Syntaxgültigkeit
- Typkorrektheit
- Symbolauflösung
- Projektabhängigkeiten
- Logic-Graph-Verbindungen
- UI-Bindungen

Eine erfolgreiche statische Prüfung garantiert nicht automatisch vollständige Verhaltensgleichheit. Nicht nachweisbare Auswirkungen müssen kenntlich gemacht werden.

## Performance

- Symbolindizes und Analyseergebnisse werden wiederverwendet.
- Betroffene Dokumente werden inkrementell verarbeitet.
- Umfangreiche Refactorings laufen im Hintergrund.
- Operationen müssen abbrechbar sein.
- Ressourcenverbrauch und parallele Analyseaufgaben müssen begrenzbar sein.
- Die Benutzeroberfläche darf während der Vorbereitung nicht blockieren.

## Sicherheit

- Refactoring führt bearbeiteten Quellcode nicht aus.
- Dateiänderungen benötigen entsprechende Zugriffsrechte.
- Geschützte Ressourcen dürfen nicht unautorisiert verändert werden.
- Capability-Identitäten und Berechtigungen bleiben unverändert.
- Sicherheitsrelevante Änderungen an Solutions müssen durch deren Integritäts- und Autorisierungsprüfung erfasst werden.
- KI-generierte Refactoring-Vorschläge benötigen eine überprüfbare Änderungsvorschau.

## Normative Anforderungen

1. NovaLang Studio MUSS eine zentrale Refactoring Engine bereitstellen.
2. Refactorings MÜSSEN auf semantischen Symbol- und Typinformationen basieren.
3. Rename Symbol, Extract Method und Extract Variable MÜSSEN unterstützt werden.
4. Projektübergreifende Referenzen MÜSSEN berücksichtigt werden.
5. `.nova`, `.nlf` und `.nui` MÜSSEN gemeinsam verarbeitet werden können.
6. Logic-Graph-Verbindungen und UI-Bindungen MÜSSEN bei betroffenen Änderungen geprüft werden.
7. Konflikte und nicht eindeutig auflösbare Referenzen MÜSSEN angezeigt werden.
8. Mehrdateiänderungen MÜSSEN transaktional und rückgängig machbar sein.
9. Refactorings MÜSSEN vor ihrer Übernahme überprüfbar sein.
10. Nach Änderungen MUSS eine erneute semantische Validierung möglich sein.
11. Refactorings DÜRFEN keine Capability-Berechtigungen erzeugen oder Sicherheitsgrenzen umgehen.
12. Umfangreiche Operationen MÜSSEN asynchron und abbrechbar sein.
13. Die grundlegende Refactoring Engine MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine sichere, semantisch gesteuerte Refactoring-Infrastruktur für NovaLang-Code, Projekte und Solutions.

Änderungen können über Code Editor, Logic Graph und UI Designer hinweg konsistent, nachvollziehbar und rückgängig machbar durchgeführt werden.
