
# NPSPEC-LOGIC-BRANCHING-0001 – NovaOS Logic Graph Branching

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Verzweigungen

## Zweck

Definiert die Auswahl und Ausführung alternativer Ablaufpfade innerhalb eines NovaOS Logic Graphs.

Ziel ist eine typsichere, eindeutige und kontrollierte Verzweigungslogik auf Grundlage von Bedingungen und Ereignissen.

## Architektur

Das Branching-System besteht aus:

- **Branch Controller:** Steuerung der Verzweigung.
- **Condition Resolver:** Auswertung der Auswahlbedingungen.
- **Path Selector:** Auswahl des auszuführenden Zweigs.
- **Branch Scheduler:** Aktivierung ausgewählter Ausführungspfade.
- **Branch Diagnostics:** Überwachung und Fehlerdiagnose.

Die Ausführung verwendet die Controlflow Engine.

## Verzweigungstypen

Unterstützt werden:

- **If/Else:** Auswahl anhand einer booleschen Bedingung.
- **Else If:** Prüfung mehrerer Bedingungen in definierter Reihenfolge.
- **Switch/Select Case:** Auswahl anhand eines typisierten Wertes.
- **Pattern Branch:** Auswahl anhand unterstützter NovaLang-Muster.
- **Event Branch:** Auswahl anhand eines eingehenden Ereignisses.

## Ausführungsmodell

Bei einer exklusiven Verzweigung wird genau ein passender Ausführungspfad aktiviert.

- Bedingungen werden in definierter Reihenfolge geprüft.
- Nicht ausgewählte Zweige bleiben inaktiv.
- Ein optionaler Standardzweig behandelt nicht zugeordnete Werte.
- Verzweigungen dürfen verschachtelt werden.

Die Bedingungsauswertung richtet sich nach `NPSPEC-LOGIC-CONDITIONS-0001`.

## Zusammenführung

Ausführungspfade können über definierte Join-Knoten zusammengeführt werden.

Die Zusammenführung muss Datenabhängigkeiten und Typkompatibilität berücksichtigen.

Ein Join darf nicht unbegrenzt auf Zweige warten, die aufgrund einer exklusiven Verzweigung nicht aktiviert wurden.

## Fehlerbehandlung

Ungültige Bedingungen, fehlende Werte und nicht behandelte Auswahlfälle müssen diagnostizierbar sein.

Fehler dürfen nicht automatisch zur Aktivierung eines alternativen Zweigs führen, sofern dies nicht ausdrücklich definiert wurde.

## Normative Anforderungen

1. Die Graph Runtime MUSS bedingte Verzweigungen unterstützen.
2. `If/Else` und `Switch/Select Case` MÜSSEN verfügbar sein.
3. Bedingungen MÜSSEN gemäß NovaLang-Semantik ausgewertet werden.
4. Exklusive Verzweigungen DÜRFEN nur einen Ausführungspfad aktivieren.
5. Die Auswertungsreihenfolge MUSS eindeutig definiert sein.
6. Standardzweige MÜSSEN unterstützt werden.
7. Verschachtelte Verzweigungen MÜSSEN möglich sein.
8. Zusammenführungen MÜSSEN inaktive Zweige korrekt berücksichtigen.
9. Ein- und Ausgangstypen MÜSSEN validiert werden.
10. Fehlerhafte Auswahlbedingungen MÜSSEN diagnostiziert werden.
11. Verzweigungen DÜRFEN keine Capability-Berechtigungen erzeugen oder erweitern.
12. Das Branching-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine typsichere und deterministisch kontrollierbare Verzweigungssteuerung für alternative Ausführungspfade innerhalb von Logic Graphs.
