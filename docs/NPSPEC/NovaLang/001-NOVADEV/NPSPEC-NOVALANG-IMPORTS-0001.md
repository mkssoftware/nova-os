
# NPSPEC-NOVALANG-IMPORTS-0001 – NovaLang Imports

## Status

Angenommen

## Kategorie

NovaLang / Namensraumimporte

## Zweck

Definiert das Einbinden von Namensräumen und Typen zur vereinfachten Namensauflösung. Die Syntax orientiert sich an VB.NET.

## Syntax

Imports werden mit `Imports` am Anfang einer Quelldatei deklariert.

```vb
Imports Nova.System
Imports Nova.Math
Imports Nova.Text
```

Mehrere Imports dürfen kombiniert werden.

## Alias-Imports

Aliase ermöglichen alternative Bezeichnungen für Namensräume und Typen.

```vb
Imports MathLib = Nova.Math

Dim ergebnis = MathLib.Rechner.Addieren(10, 20)
```

Aliase gelten innerhalb der jeweiligen Quelldatei und müssen eindeutig sein.

## Namensauflösung

- Imports ermöglichen die Verwendung nicht vollständig qualifizierter Namen.
- Lokale Deklarationen und gültige Scope-Regeln besitzen Vorrang.
- Mehrdeutige Namen müssen einen Compilerfehler erzeugen.
- Groß- und Kleinschreibung wird ignoriert.
- Vollständig qualifizierte Namen bleiben unabhängig von Imports verwendbar.
- Imports dürfen keine bestehenden Typidentitäten verändern.

## Gültigkeitsbereich

Imports gelten grundsätzlich für die jeweilige Quelldatei.

Projektweite Imports können über die Projektkonfiguration definiert werden.

Imports werden statisch verarbeitet und erzeugen keine eigenen Laufzeitinstanzen.

## NovaOS-Integration

- Imports können NovaLang-Module, Standardbibliotheken und öffentliche Capability-Schnittstellen referenzieren.
- Abhängigkeiten müssen über das NovaOS-Modulsystem aufgelöst werden.
- Modulversionen und Sichtbarkeitsregeln müssen eingehalten werden.
- `Imports` erteilt keine Capability-Berechtigungen.
- Custom Scripts im Logic Graph dürfen durch Imports keine zusätzlichen Systemzugriffe erhalten.

## Normative Anforderungen

1. NovaLang MUSS `Imports` nach VB.NET-orientierter Syntax unterstützen.
2. Namensraum- und Typaliase MÜSSEN unterstützt werden.
3. Imports MÜSSEN statisch aufgelöst werden.
4. Mehrdeutige Referenzen MÜSSEN diagnostiziert werden.
5. Imports DÜRFEN keine Module automatisch ausführen oder Berechtigungen erzeugen.
6. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben Imports-Regeln verwenden.

## Ergebnis

NovaLang besitzt ein VB.NET-orientiertes Imports-System zur vereinfachten Verwendung von Namensräumen und Typen, mit statischer Auflösung und strikter Trennung zwischen Codezugriff und Capability-Autorisierung.
