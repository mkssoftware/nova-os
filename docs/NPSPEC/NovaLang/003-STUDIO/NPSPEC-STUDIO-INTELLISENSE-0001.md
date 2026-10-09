
# NPSPEC-STUDIO-INTELLISENSE-0001 – NovaLang Studio IntelliSense

## Status

Angenommen

## Kategorie

NovaLang Studio / Language Service / IntelliSense

## Zweck

Definiert die intelligente, kontextabhängige Codeunterstützung von NovaLang Studio.

IntelliSense verbindet Syntaxanalyse, Typprüfung, Symbolauflösung, Dokumentation und Codeaktionen zu einem einheitlichen Entwicklungsdienst.

Ziel ist, dass Entwickler verfügbare Funktionen, Datentypen und Schnittstellen unmittelbar verstehen und korrekt verwenden können, ohne ständig Dokumentationen durchsuchen zu müssen.

## Architektur

| Komponente | Aufgabe |
|---|---|
| IntelliSense Engine | Koordination aller Sprachdienste |
| Language Service | Gemeinsame Sprach- und Typanalyse |
| Symbol Index | Verwaltung und Suche von Symbolen |
| Context Analyzer | Ermittlung des aktuellen Codekontexts |
| Signature Provider | Funktions- und Parameterinformationen |
| Documentation Provider | Anzeige von API-Dokumentationen |
| Reference Resolver | Definitionen, Referenzen und Implementierungen |
| Code Action Provider | Korrekturen und Refactoring-Vorschläge |

IntelliSense verwendet die gemeinsame NovaLang-Compilerinfrastruktur und dupliziert keine Sprachsemantik.

## Kernfunktionen

IntelliSense unterstützt:

- Kontextabhängige Autovervollständigung
- Parameter- und Signaturhilfe
- Quick Info beim Überfahren von Symbolen
- Typinformationen
- Go to Definition
- Find All References
- Go to Implementation
- Symbolsuche
- Fehlerdiagnostik
- Quick Fixes
- Refactoring-Vorschläge
- Dokumentationsanzeige

Die eigentliche Vervollständigung wird durch `NPSPEC-STUDIO-AUTOCOMPLETION-0001` definiert.

## Semantische Analyse

IntelliSense berücksichtigt:

- NovaLang-Syntax und Typregeln
- Lokale und globale Gültigkeitsbereiche
- Sichtbarkeit von Symbolen
- Generische Typen
- Überladungen
- Nullability
- Modul- und Projektabhängigkeiten
- Aktuelle Compilerdiagnosen

Unvollständiger Quellcode muss soweit möglich analysierbar bleiben.

## Quick Info

Beim Überfahren oder Auswählen eines Symbols können angezeigt werden:

- Symbolname und Symbolart
- Vollständige Typdefinition
- Funktionssignatur
- Rückgabetyp
- Dokumentationskommentare
- Modulherkunft
- Verfügbarkeit und Versionshinweise

Die Darstellung soll kompakt bleiben und bei Bedarf zusätzliche Informationen anbieten.

## Signature Help

Bei Funktionsaufrufen zeigt IntelliSense die gültigen Signaturen.

Beispiel:

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function

Dim ergebnis = Addieren(10, 20)
```

Während der Eingabe werden Parameternamen, Typen und verfügbare Überladungen angezeigt.

Die aktuell bearbeitete Parameterposition wird hervorgehoben.

## Symbolnavigation

IntelliSense ermöglicht die Navigation zwischen:

- Symboldeklarationen
- Symbolverwendungen
- Interface-Implementierungen
- Vererbungsbeziehungen
- Projektübergreifenden Referenzen

Navigationsergebnisse müssen den aktuellen Workspace-Zustand berücksichtigen.

## Code Actions

IntelliSense stellt kontextabhängige Aktionen bereit.

Beispiele:

- Fehlenden Import ergänzen
- Nicht implementierte Interface-Member erzeugen
- Typkonflikt korrigieren
- Variable umbenennen
- Methode extrahieren
- Veraltete API ersetzen

Änderungen müssen vor ihrer Übernahme nachvollziehbar und rückgängig machbar sein.

## Dokumentationsintegration

IntelliSense verwendet die NovaLang-Dokumentationsinformationen, einschließlich `'''`-Dokumentationskommentaren.

Dokumentationen können enthalten:

- Beschreibung
- Parameter
- Rückgabewerte
- Exceptions
- Beispiele
- Versionsinformationen
- Capability-Anforderungen

Dokumentationsinformationen dürfen nicht mit tatsächlich erteilten Berechtigungen verwechselt werden.

## Logic-Graph-Integration

IntelliSense unterstützt `.nlf`-Dateien und Custom Scripts.

Zusätzlich berücksichtigt werden:

- Eingangs- und Ausgangstypen
- Datenverträge verbundener Graph-Knoten
- Bereitgestellte Capability-Handles
- Verfügbare Funktionen und Datentypen
- Fehlerhafte oder inkompatible Verbindungen

Custom Scripts dürfen keine zusätzlichen Capabilities selbst anfordern.

IntelliSense darf nur tatsächlich verfügbare Schnittstellen als unmittelbar nutzbar darstellen.

## UI-Designer-Integration

Für `.nui`-Dateien unterstützt IntelliSense:

- UI-Komponententypen
- Eigenschaften und Ereignisse
- Datenbindungen
- Layout-Definitionen
- Typprüfung gebundener Werte
- Navigation zu Logic-Graph-Ausgaben

Visueller UI Designer und Code Editor verwenden dieselben semantischen Informationen.

## Inkrementelle Verarbeitung

IntelliSense arbeitet auf versionierten Dokumentzuständen.

- Änderungen lösen gezielte Neuanalysen aus.
- Unveränderte Symbolinformationen werden wiederverwendet.
- Veraltete Ergebnisse werden verworfen.
- Laufende Anfragen müssen abbrechbar sein.
- Die Texteingabe bleibt unabhängig von Hintergrundanalysen reaktionsfähig.

## Benutzeroberfläche

Die Darstellung folgt der NovaOS-Designsprache.

- Kompakte Informationsfenster
- Direkt erreichbare Aktionen
- Tastatur- und Mausbedienung
- Einheitliche Symbolkennzeichnungen
- Dezente, nicht störende Hinweise
- Keine unerwarteten Layoutverschiebungen

Häufig verwendete Informationen sollen ohne zusätzliche Menüebenen erreichbar sein.

## Sicherheit

- IntelliSense führt analysierten Quellcode nicht aus.
- Projektdateien werden als potenziell nicht vertrauenswürdig behandelt.
- Symbolanalyse erteilt keine Capability-Berechtigungen.
- Geschützte Ressourcen dürfen nicht unautorisiert durchsucht werden.
- Externe Dokumentationsquellen benötigen entsprechende Zugriffsrechte.
- Optionale KI-Funktionen unterliegen denselben Sicherheitsregeln.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen IntelliSense-Dienst bereitstellen.
2. IntelliSense MUSS die gemeinsame NovaLang-Sprach- und Typanalyse verwenden.
3. Quick Info, Signature Help und Symbolnavigation MÜSSEN unterstützt werden.
4. Kontextabhängige Code Actions MÜSSEN verfügbar sein.
5. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Sprachsemantik verwenden.
6. Unvollständiger Quellcode MUSS soweit möglich analysierbar bleiben.
7. Dokumentationsinformationen MÜSSEN direkt im Editor angezeigt werden können.
8. Logic-Graph- und UI-spezifische Typinformationen MÜSSEN berücksichtigt werden.
9. Analysen MÜSSEN inkrementell und abbrechbar sein.
10. Veraltete Analyseergebnisse DÜRFEN nicht als aktuelle Informationen dargestellt werden.
11. IntelliSense DARF keine Capability-Berechtigungen erzeugen oder Sicherheitsgrenzen umgehen.
12. Sämtliche grundlegenden IntelliSense-Funktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält einen zentralen, leistungsfähigen IntelliSense-Dienst für Codeverständnis, Navigation, Dokumentation und kontextabhängige Entwicklungshilfe.

IntelliSense verbindet Code Editor, Logic Graph und UI Designer über eine gemeinsame semantische Analyse und bleibt vollständig unabhängig von KI funktionsfähig.
