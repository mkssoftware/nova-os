
# NPSPEC-SOLUTION-VALIDATION-0001 – NovaOS Solution Validation

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Validierung

## Zweck

Definiert die vollständige Prüfung einer NovaOS-Solution vor ihrer Ausführung und bei sicherheitsrelevanten Änderungen.

Ziel ist, ausschließlich strukturell gültige, kompatible und ausreichend autorisierte Solutions auszuführen.

## Architektur

Das Validation-System besteht aus:

- **Solution Validator:** Koordination aller Prüfungen.
- **Manifest Validator:** Prüfung der `solution.xml`.
- **Identity Validator:** Prüfung der Solution-Identität.
- **Integrity Validator:** Prüfung von Integrität und Signaturen.
- **Dependency Validator:** Prüfung benötigter Komponenten.
- **Logic Validator:** Validierung der Logic Graphs.
- **UI Validator:** Prüfung deklarativer UI-Beschreibungen.
- **Permission Validator:** Abgleich mit der Berechtigungs-Registry.

Die Validierung verwendet die bestehenden NovaOS-Systemdienste.

## Prüfbereiche

Eine Solution wird auf folgende Eigenschaften geprüft:

- Gültige `solution.xml` mit eindeutiger GUID
- Unterstützte Manifest- und Solution-Version
- Integrität aller relevanten Bestandteile
- Gültige Logic Graphs und NovaLang-Scripts
- Korrekte UI- und Logic-Bindings
- Vorhandene und kompatible Capabilities
- Gültige Capability-Flüsse
- Ausreichende Berechtigungen
- Einhaltung von Ressourcen- und Sicherheitsrichtlinien

## Validierungsablauf

1. Solution-Manifest einlesen und prüfen.
2. Identität und Integrität verifizieren.
3. Abhängigkeiten und Capability-Verträge auflösen.
4. Logic Graphs und Scripts validieren.
5. UI-Beschreibungen und Bindings prüfen.
6. Berechtigungen und Sicherheitsrichtlinien abgleichen.
7. Validierungsergebnis an die Solution Runtime übergeben.

Die Ausführung darf nur bei erfolgreicher Validierung beginnen.

## Validierungszustände

- **Valid:** Alle erforderlichen Prüfungen erfolgreich.
- **Warning:** Ausführung zulässig, jedoch mit Hinweisen.
- **Invalid:** Strukturelle oder semantische Fehler.
- **Unauthorized:** Erforderliche Berechtigungen fehlen.
- **Incompatible:** Nicht unterstützte Versionen oder Abhängigkeiten.

Warnungen dürfen keine sicherheitsrelevanten Fehler herabstufen.

## Änderungen und erneute Prüfung

Änderungen an Manifest, Logic Graph, Scripts, UI oder Capability-Anforderungen machen betroffene Validierungsergebnisse ungültig.

Sicherheitsrelevante Änderungen müssen eine erneute Integritäts- und Berechtigungsprüfung auslösen.

Bereits erteilte Berechtigungen dürfen nicht allein aufgrund einer unveränderten GUID weiterverwendet werden.

## Fehlerbehandlung

Jeder Validierungsfehler erhält einen eindeutigen Diagnosecode und eine Quellenreferenz.

Fehler müssen der betroffenen Datei, Komponente oder Graphstruktur zugeordnet werden können.

Ungültige Solutions dürfen als Entwurf bearbeitet, jedoch nicht produktiv ausgeführt werden.

## Normative Anforderungen

1. Jede Solution MUSS vor ihrer Ausführung validiert werden.
2. Die `solution.xml` MUSS auf Struktur, GUID und Version geprüft werden.
3. Die Solution-Identität MUSS verifiziert werden.
4. Integrität und vorhandene Signaturen MÜSSEN gemäß Sicherheitsrichtlinie geprüft werden.
5. Logic Graphs, NovaLang-Scripts und UI-Bindings MÜSSEN validiert werden.
6. Capability-Verträge und Abhängigkeiten MÜSSEN kompatibel sein.
7. Berechtigungen MÜSSEN anhand der verifizierten Solution-Identität geprüft werden.
8. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Prüfung auslösen.
9. Ungültige oder nicht autorisierte Solutions DÜRFEN nicht ausgeführt werden.
10. Validierungsfehler MÜSSEN eindeutig diagnostizierbar sein.
11. Validierung DARF keine Berechtigungen selbstständig erteilen.
12. Die Solution Runtime MUSS die Sicherheitsregeln auch während der Ausführung durchsetzen.
13. Das Validation-System MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine zentrale Solution-Validierung, die Identität, Integrität, Logic Graphs, Benutzeroberflächen, Capabilities und Berechtigungen vor der Ausführung zuverlässig überprüft.
