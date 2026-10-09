
# NPSPEC-LOGIC-VALIDATION-0001 – NovaOS Logic Graph Validation

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Validierung

## Zweck

Definiert die vollständige Validierung eines Logic Graphs vor seiner Ausführung.

Ziel ist die frühzeitige Erkennung struktureller, semantischer und sicherheitsrelevanter Fehler.

## Architektur

Das Validation-System besteht aus:

- **Graph Validator:** Koordination aller Prüfungen.
- **Schema Validator:** Prüfung der Graphstruktur.
- **Connection Validator:** Prüfung von Ports und Verbindungen.
- **Type Validator:** Prüfung der NovaLang-Typkompatibilität.
- **Execution Validator:** Prüfung von Ablauf- und Datenabhängigkeiten.
- **Capability Validator:** Prüfung deklarierter Capability-Anforderungen.
- **Diagnostic Reporter:** Bereitstellung der Prüfergebnisse.

Die Validierung arbeitet unabhängig vom grafischen Editor.

## Validierungsbereiche

Geprüft werden:

- Schema- und Versionskompatibilität
- Eindeutigkeit von Knoten- und Portidentitäten
- Existenz referenzierter Knoten und Ports
- Typkompatibilität und Kardinalität
- Daten- und Steuerungsabhängigkeiten
- Unzulässige Zyklen
- Subgraph- und Funktionsreferenzen
- NovaLang-Skripte und Signaturen
- Capability-Verträge und Berechtigungsanforderungen
- Ressourcen- und Ausführungsverträge

## Validierungsablauf

1. Graphstruktur einlesen.
2. Schema und Referenzen prüfen.
3. Typen und Verbindungen validieren.
4. Ausführungsabhängigkeiten analysieren.
5. Skripte und Subgraphs prüfen.
6. Capability-Anforderungen ermitteln.
7. Diagnosen und Validierungsstatus erzeugen.

Die Validierung erteilt selbst keine Berechtigungen.

## Inkrementelle Validierung

Bei Änderungen werden betroffene Graphbereiche und ihre Abhängigkeiten erneut geprüft.

Nicht mehr gültige Prüfergebnisse müssen verworfen werden.

Eine vollständige Validierung muss vor Build und produktiver Ausführung möglich sein.

## Diagnosen

Prüfergebnisse werden eingeteilt in:

- **Fehler:** Verhindern die Ausführung.
- **Warnungen:** Kennzeichnen potenzielle Probleme.
- **Hinweise:** Unterstützen die Entwicklung.

Jede Diagnose muss dem betroffenen Graph, Knoten, Port oder der Verbindung zugeordnet werden können.

## Normative Anforderungen

1. Logic Graphs MÜSSEN vor der Ausführung validiert werden.
2. Schema, Knoten, Ports und Verbindungen MÜSSEN geprüft werden.
3. Typprüfungen MÜSSEN die reguläre NovaLang-Semantik verwenden.
4. Ungültige Ausführungsabhängigkeiten MÜSSEN erkannt werden.
5. Subgraphs und Funktionsreferenzen MÜSSEN validiert werden.
6. Custom Scripts MÜSSEN durch den NovaLang-Compiler geprüft werden.
7. Capability-Anforderungen MÜSSEN vollständig ermittelt werden.
8. Validierung DARF keine Capability-Berechtigungen erteilen.
9. Änderungen MÜSSEN eine erneute Prüfung betroffener Abhängigkeiten auslösen.
10. Blockierende Fehler MÜSSEN die Ausführung verhindern.
11. Diagnosen MÜSSEN eindeutig, nachvollziehbar und maschinenlesbar sein.
12. Das Validation-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine zentrale, inkrementelle und sicherheitsbewusste Validierungsinfrastruktur, die Logic Graphs vor Build und Ausführung auf strukturelle, semantische und ausführungsrelevante Fehler prüft.
