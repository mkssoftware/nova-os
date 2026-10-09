
# NPSPEC-STUDIO-AUTOCOMPLETION-0001 – NovaLang Studio Autocompletion

## Status

Angenommen

## Kategorie

NovaLang Studio / Code Editor / Autocompletion

## Zweck

Definiert die automatische Codevervollständigung von NovaLang Studio.

Ziel ist eine schnelle, kontextabhängige und typensichere Unterstützung bei der Entwicklung von NovaLang-Programmen, Logic-Graph-Skripten und deklarativen Benutzeroberflächen.

Die Autovervollständigung muss vollständig ohne KI funktionieren. KI-basierte Vorschläge können optional ergänzt werden.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Completion Engine | Ermittlung und Verwaltung von Vorschlägen |
| Context Analyzer | Analyse der aktuellen Codeposition |
| Symbol Resolver | Auflösung verfügbarer Symbole |
| Type Resolver | Prüfung der Typkompatibilität |
| Completion Ranking | Sortierung relevanter Vorschläge |
| Snippet Engine | Einfügen strukturierter Codevorlagen |
| Completion UI | Darstellung und Auswahl der Vorschläge |

Die Komponenten verwenden den gemeinsamen NovaLang Language Service.

## Vervollständigungsarten

NovaLang Studio unterstützt:

- Schlüsselwortvervollständigung
- Variablen und Konstanten
- Klassen, Strukturen und Interfaces
- Funktionen und Methoden
- Eigenschaften und Felder
- Namespaces und Imports
- Generische Typen
- Funktionsparameter
- Code-Snippets
- UI-Komponenten und Eigenschaften
- Capability-Schnittstellen

## Kontextanalyse

Die Completion Engine berücksichtigt:

- Aktuelle Cursorposition
- Syntaktischen Kontext
- Gültigkeitsbereich
- Sichtbarkeit von Symbolen
- Erwarteten Datentyp
- Bereits eingegebene Zeichen
- Projekt- und Modulabhängigkeiten

Nicht zugängliche oder typinkompatible Symbole dürfen nicht als unmittelbar gültige Vorschläge dargestellt werden.

## Beispiel

```vb
Public Class Benutzer
    Public Property Name As String
    Public Property Alter As Integer
End Class

Dim person As New Benutzer()
person.
```

Nach `person.` werden beispielsweise vorgeschlagen:

- `Name As String`
- `Alter As Integer`

Die Vorschläge werden aus den tatsächlichen Typinformationen ermittelt.

## Intelligente Sortierung

Vorschläge werden nach Relevanz sortiert.

Berücksichtigt werden:

1. Typkompatibilität
2. Syntaktischer Kontext
3. Lokaler Gültigkeitsbereich
4. Übereinstimmung mit der Eingabe
5. Symbolverfügbarkeit
6. Nutzungshäufigkeit als nachrangiges Kriterium

Die Sortierung muss deterministisch reproduzierbar sein, sofern Eingaben und Analysezustand identisch sind.

## Funktionssignaturen

Beim Aufruf einer Funktion zeigt der Editor:

- Funktionsname
- Parameter und Datentypen
- Rückgabetyp
- Optionale Parameter
- Überladungen
- Dokumentationskommentare

Der aktuell bearbeitete Parameter wird hervorgehoben.

## Automatische Imports

Fehlende Imports können vorgeschlagen werden.

Dabei gilt:

- Nur tatsächlich verfügbare Module werden berücksichtigt.
- Mehrdeutige Symbole müssen eindeutig aufgelöst werden.
- Imports dürfen keine Capability-Berechtigungen erzeugen.
- Änderungen müssen vor ihrer Übernahme nachvollziehbar sein.

## Code-Snippets

Die Snippet Engine unterstützt strukturierte Vorlagen.

Beispiel:

```vb
Public Function ${Name}(${Parameter}) As ${Typ}
    ${Cursor}
End Function
```

Platzhalter können über Tastaturbefehle durchlaufen werden.

Snippets müssen erweiterbar und benutzerdefinierbar sein.

## Logic-Graph-Integration

Innerhalb von `.nlf`-Dateien werden zusätzlich vorgeschlagen:

- Verfügbare Eingabeparameter
- Definierte Ausgabetypen
- Lokale Funktionen
- Bereitgestellte Datenstrukturen
- Zulässige Operationen auf übergebenen Objekten

Custom Scripts dürfen durch Autovervollständigung keine zusätzlichen Capabilities anfordern.

Capability-Zugriffe werden nur vorgeschlagen, wenn entsprechende autorisierte Schnittstellen im Ausführungskontext tatsächlich verfügbar sind.

## UI-Designer-Integration

Bei `.nui`-Dateien unterstützt die Autovervollständigung:

- UI-Komponenten
- Eigenschaften und Ereignisse
- Datenbindungen
- Layout-Parameter
- Unterstützte Typen und Werte
- Referenzen auf Logic-Graph-Ausgaben

Die Vorschläge müssen mit der gemeinsamen NovaLang-Typsemantik übereinstimmen.

## Benutzeroberfläche

Die Vorschlagsliste verwendet die NovaOS-Designsprache.

- Kompakte Darstellung
- Symbol- und Typkennzeichnung
- Tastatur- und Mausbedienung
- Schnelle Filterung während der Eingabe
- Optionale Dokumentationsvorschau
- Keine unnötige Überdeckung des Quellcodes

Vorschläge dürfen nicht ohne bewusste Benutzeraktion übernommen werden.

## Performance

- Die Eingabe darf durch Autovervollständigung nicht blockiert werden.
- Symbolinformationen müssen wiederverwendbar sein.
- Analysen erfolgen inkrementell.
- Veraltete Anfragen müssen abgebrochen oder verworfen werden können.
- Große Projekte müssen effizient verarbeitet werden.
- Ressourcenverbrauch und Ergebnisanzahl müssen begrenzbar sein.

## KI-Unterstützung

Optional kann eine KI zusätzliche Codevorschläge erzeugen.

Dabei gilt:

- Die deterministische Completion Engine bleibt unabhängig funktionsfähig.
- KI-Vorschläge werden eindeutig gekennzeichnet.
- Vorschläge müssen vor der Übernahme überprüfbar sein.
- KI erhält keinen automatischen Zugriff auf geschützte Projektinhalte.
- KI-generierter Code erhält keine zusätzlichen Berechtigungen.

## Normative Anforderungen

1. NovaLang Studio MUSS eine kontextabhängige Autovervollständigung bereitstellen.
2. Vorschläge MÜSSEN auf Syntax-, Symbol- und Typinformationen basieren.
3. Gültigkeitsbereiche und Sichtbarkeitsregeln MÜSSEN berücksichtigt werden.
4. Funktionssignaturen und Parameterinformationen MÜSSEN angezeigt werden können.
5. Automatische Imports und Code-Snippets MÜSSEN unterstützt werden.
6. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe Sprach- und Typanalyse verwenden.
7. Logic-Graph- und UI-spezifische Vorschläge MÜSSEN unterstützt werden.
8. Veraltete Analyseergebnisse DÜRFEN nicht als aktuelle Vorschläge ausgegeben werden.
9. Autovervollständigung DARF keine Capability-Berechtigungen erzeugen oder Sicherheitsgrenzen umgehen.
10. Hintergrundanalysen DÜRFEN die Texteingabe nicht blockieren.
11. Vorschläge MÜSSEN durch den Benutzer kontrolliert übernommen werden.
12. Die vollständige grundlegende Autovervollständigung MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine schnelle, typensichere und kontextabhängige Autovervollständigung für NovaLang, Logic Graph und UI-Definitionen.

Die Funktion kombiniert klassische IntelliSense-Technologie mit optionaler KI-Unterstützung, ohne von KI abhängig zu sein.
