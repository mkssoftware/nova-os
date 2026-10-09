
# NPSPEC-SOLUTION-PACKAGING-0001 – NovaOS Solution Packaging

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Packaging

## Zweck

Definiert die Erstellung, Struktur und Prüfung installierbarer NovaOS-Solution-Pakete.

Ziel ist die sichere, portable und reproduzierbare Bereitstellung von Solutions einschließlich Logic Graphs, NovaLang-Scripts, Benutzeroberflächen und Ressourcen.

## Architektur

Das Packaging-System besteht aus:

- **Package Builder:** Erstellung des Solution-Pakets.
- **Manifest Processor:** Verarbeitung der `solution.xml`.
- **Dependency Resolver:** Prüfung benötigter Abhängigkeiten.
- **Integrity Generator:** Erstellung kryptografischer Integritätsnachweise.
- **Package Validator:** Prüfung des fertigen Pakets.
- **Package Installer Bridge:** Übergabe an die NovaOS-Installationsverwaltung.

## Paketinhalt

Ein Solution-Paket enthält:

- `solution.xml` mit GUID, Version und Metadaten
- Logic-Graph-Definitionen
- NovaLang-Scripts und benötigte Laufzeitartefakte
- Deklarative `.nui`-Oberflächen
- Ressourcen und Konfigurationen
- Deklarierte Capability-Anforderungen
- Abhängigkeits- und Integritätsinformationen

Nicht benötigte Entwicklungsdateien sollen ausgeschlossen werden.

## Paketmodell

Das Paket ist eine versionierte, in sich konsistente Einheit.

Dateipfade, Referenzen und Abhängigkeiten müssen eindeutig auflösbar sein.

Private Abhängigkeiten können über den vorgesehenen Application-SYS-Overlay bereitgestellt werden, ohne das globale `/System` zu verändern.

## Build und Validierung

Vor der Paketerstellung werden geprüft:

- Solution-Manifest und Identität
- Graphstruktur und NovaLang-Artefakte
- UI-Definitionen und Bindungen
- Capability-Verträge
- Abhängigkeiten und Versionen
- Dateiintegrität und Paketstruktur

Ungültige Solutions dürfen nicht als produktiv installierbare Pakete freigegeben werden.

## Identität und Integrität

Die Solution-GUID bleibt über Paketversionen hinweg erhalten.

Alle sicherheitsrelevanten Paketbestandteile müssen durch kryptografische Integritätsnachweise geschützt werden.

Eine GUID allein gilt nicht als Identitäts- oder Herkunftsnachweis.

Digitale Signaturen ermöglichen die Prüfung der vertrauenswürdigen Herausgeberidentität.

## Installation und Berechtigungen

Die Installation erfolgt über die NovaOS-Installationsverwaltung.

Capability-Anforderungen werden aus dem verifizierten Paket übernommen und durch die zentrale Berechtigungsverwaltung geprüft.

Das Paket darf keine vorab erteilten Berechtigungen als gültige Autorisierung mitliefern.

Sicherheitsrelevante Änderungen bei Updates müssen eine erneute Berechtigungsprüfung auslösen.

## Portabilität

Pakete müssen unabhängig von NovaLang Studio installierbar sein.

Plattformabhängige Artefakte und erforderliche Laufzeitversionen müssen eindeutig deklariert werden.

Fehlende oder inkompatible Abhängigkeiten müssen vor der Installation erkannt werden.

## Normative Anforderungen

1. Jede installierbare Solution MUSS ein versioniertes Paketformat verwenden.
2. Jedes Paket MUSS eine gültige `solution.xml` enthalten.
3. Die dauerhafte Solution-GUID MUSS erhalten bleiben.
4. Graphen, Scripts, UI und Ressourcen MÜSSEN vollständig referenzierbar sein.
5. Abhängigkeiten und Capability-Anforderungen MÜSSEN deklariert werden.
6. Paketinhalte MÜSSEN kryptografisch auf Integrität prüfbar sein.
7. Signierte Pakete MÜSSEN einer verifizierbaren Herausgeberidentität zugeordnet werden können.
8. Ungültige Pakete DÜRFEN nicht produktiv installiert werden.
9. Pakete DÜRFEN keine Berechtigungen selbst erteilen.
10. Updates MÜSSEN Integritäts- und Berechtigungsprüfungen durchlaufen.
11. Private Abhängigkeiten DÜRFEN das globale System nicht ohne Autorisierung verändern.
12. Installation und Ausführung MÜSSEN ohne NovaLang Studio und ohne KI möglich sein.

## Ergebnis

NovaOS erhält ein sicheres, versioniertes und portables Packaging-System zur zuverlässigen Verteilung, Installation und Aktualisierung vollständiger Solutions.
