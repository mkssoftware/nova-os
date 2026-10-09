# NPSPEC-STUDIO-GRAPH-SCRIPT-BINDING-0001

**Status:** Angenommen

## Zweck
Custom-NovaLang-Skripte an Knotenports und Solution-Kontext binden.

## Festlegungen
- Skriptparameter und Rückgabewerte werden aus der NovaLang-Signatur abgeleitet.
- Änderungen der Signatur aktualisieren Pins transaktional; defekte Verbindungen werden angezeigt.
- Skripte werden mit der Solution-Version und ihren geprüften Artefakten verknüpft.
- Nur explizit verbundene Daten werden übergeben; Skripte können keine Capability-Rechte nachfordern.

## Ergebnis
Skript und Graph behalten eine überprüfbare, stabile Bindung.
