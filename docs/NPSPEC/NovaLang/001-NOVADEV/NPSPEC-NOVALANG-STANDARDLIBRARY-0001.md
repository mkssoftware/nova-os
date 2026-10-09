
# NPSPEC-NOVALANG-STANDARDLIBRARY-0001 – NovaLang Standard Library

## Status

Angenommen

## Kategorie

NovaLang / Standardbibliothek

## Zweck

Definiert die native Standardbibliothek von NovaLang. Sie stellt grundlegende Datentypen, Algorithmen und Laufzeitfunktionen bereit, ohne von .NET oder externen Frameworks abhängig zu sein.

## Aufbau

Die Standardbibliothek verwendet den Namensraum `Nova` mit folgenden Kernmodulen:

| Namensraum | Funktion |
|---|---|
| `Nova.Core` | Basistypen, Objekte, Konvertierungen |
| `Nova.Text` | Zeichenketten, Unicode, Formatierung |
| `Nova.Math` | Mathematik, numerische Funktionen |
| `Nova.Collections` | Listen, Arrays, Dictionaries, Sets |
| `Nova.Linq` | Typisierte Datenabfragen und Transformationen |
| `Nova.IO` | Streams, Ein- und Ausgabe |
| `Nova.Time` | Datum, Uhrzeit, Zeitspannen |
| `Nova.Threading` | Tasks, Synchronisation, Cancellation |
| `Nova.Events` | Ereignisse und Delegates |
| `Nova.Diagnostics` | Logging, Fehlerdiagnostik |
| `Nova.Serialization` | Serialisierung und Deserialisierung |
| `Nova.Reflection` | Typinformationen und Metadaten |

Weitere Bibliotheken können modular ergänzt werden.

## Verwendung

```vb
Imports Nova.Text
Imports Nova.Collections

Dim namen As New List(Of String)

namen.Add("NovaOS")
namen.Add("NovaLang")

For Each name As String In namen
    Console.WriteLine(name)
Next
```

Die Standardbibliothek soll vertraute VB.NET-Konzepte und Methodennamen verwenden, soweit sie zur NovaLang-Semantik passen.

## Architektur

- Die Bibliothek ist modular aufgebaut und unabhängig von der .NET-Runtime.
- Grundlegende Datentypen und Operationen sind ohne Betriebssystemdienste verwendbar.
- Plattformabhängige Funktionen verwenden definierte NovaOS-Schnittstellen.
- Nicht benötigte Bibliotheksmodule müssen nicht geladen werden.
- Öffentliche APIs besitzen stabile, versionierte Verträge.
- Compiler, Interpreter und JIT verwenden dieselbe Bibliothekssemantik.

## Capabilities und Sicherheit

Reine Berechnungen benötigen keine Systemberechtigungen.

Dateisystem-, Netzwerk-, Geräte- und andere geschützte Operationen dürfen ausschließlich über autorisierte Capabilities erfolgen.

Ein `Imports` oder Bibliotheksaufruf erzeugt keine Berechtigung. Custom Scripts innerhalb eines Logic Graph erhalten Systemzugriffe nur über ausdrücklich verbundene Capability-Knoten.

## Fehlerbehandlung

Die Standardbibliothek unterstützt Exceptions und `Result(Of T, E)` entsprechend der NovaLang-Fehlersemantik.

Ungültige Argumente, Ressourcenüberschreitungen und fehlende Berechtigungen müssen kontrolliert behandelt werden.

## Normative Anforderungen

1. NovaLang MUSS eine eigenständige native Standardbibliothek besitzen.
2. Grundlegende Typen, Collections, Text-, Mathematik- und Laufzeitfunktionen MÜSSEN verfügbar sein.
3. Öffentliche APIs MÜSSEN statisch typisiert und versioniert sein.
4. Bibliotheksmodule MÜSSEN unabhängig voneinander nutzbar sein, soweit ihre Abhängigkeiten dies erlauben.
5. Geschützte Systemoperationen MÜSSEN das Capability-Modell verwenden.
6. Die Standardbibliothek DARF keine .NET-Runtime voraussetzen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe Standardbibliothek verwenden können.

## Ergebnis

NovaLang erhält eine modulare, ressourcenschonende und erweiterbare Standardbibliothek mit vertrauter VB.NET-orientierter API und nativer NovaOS-Integration.
