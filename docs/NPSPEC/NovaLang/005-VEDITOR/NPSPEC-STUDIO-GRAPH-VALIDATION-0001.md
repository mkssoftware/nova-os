
# NPSPEC-STUDIO-GRAPH-VALIDATION-0001 – NovaLang Studio Graph Validation

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Validierung

## Zweck

Definiert die kontinuierliche Prüfung von Logic Graphs während ihrer visuellen Bearbeitung.

Ziel ist, strukturelle, typbezogene und sicherheitsrelevante Fehler frühzeitig zu erkennen und verständlich darzustellen.

## Architektur

Das Validation-System besteht aus:

- **Validation Controller:** Koordination der Prüfungen.
- **Incremental Validator:** Prüfung veränderter Graphbereiche.
- **Type Validation Bridge:** Integration des Graph Type Checkers.
- **Dependency Validator:** Prüfung von Referenzen und Abhängigkeiten.
- **Capability Validator:** Prüfung deklarierter Capability-Anforderungen.
- **Diagnostic Presenter:** Darstellung von Fehlern und Warnungen.
- **Quick Fix Provider:** Bereitstellung sicherer Korrekturvorschläge.

Die eigentliche Validierungslogik verwendet den zentralen Logic Graph Validator.

## Prüfbereiche

Unterstützt werden:

- Graphstruktur und Schema
- Knoten- und Portidentitäten
- Porttypen und Verbindungen
- Daten- und Steuerungsfluss
- Zyklen und Abhängigkeiten
- Subgraph-Schnittstellen
- NovaLang-Script-Schnittstellen
- Capability-Verträge
- Ressourcen- und Ausführungsregeln
- Versionskompatibilität

## Validierungsablauf

1. Eine Graphänderung wird erkannt.
2. Betroffene Knoten und Verbindungen werden ermittelt.
3. Abhängige Prüfungen werden ausgeführt.
4. Diagnosen werden aktualisiert.
5. Ergebnisse erscheinen unmittelbar im Editor.

Vor Build, Preview und Ausführung erfolgt zusätzlich eine vollständige Validierung.

## Diagnosemodell

Jede Diagnose enthält:

- Eindeutigen Diagnosecode
- Schweregrad: `Error`, `Warning` oder `Info`
- Betroffene Graph-, Node- oder Port-ID
- Verständliche Fehlerbeschreibung
- Optionale Korrekturempfehlung

Fehler werden direkt am betroffenen Element sowie in der zentralen Diagnoseliste angezeigt.

## Quick Fixes

Der Editor kann passende Korrekturen anbieten, beispielsweise:

- Fehlende Verbindungen ergänzen
- Kompatible Ports auswählen
- Zulässige Typkonvertierung einfügen
- Ungültige Referenzen korrigieren

Korrekturen müssen vor der Übernahme geprüft werden und über Undo rückgängig gemacht werden können.

## Sicherheit

Die Validierung darf keine Capabilities ausführen oder Berechtigungen erteilen.

Statische Prüfungen können fehlende Berechtigungen und ungültige Capability-Verträge erkennen, ersetzen jedoch nicht die Autorisierung zur Laufzeit.

Unbekannte oder nicht vertrauenswürdige Knotendefinitionen dürfen nicht ungeprüft ausgeführt werden.

## Performance

Validierungen erfolgen inkrementell und dürfen die Bedienoberfläche nicht blockieren.

Veraltete Prüfergebnisse müssen bei neuen Änderungen verworfen oder aktualisiert werden.

## Normative Anforderungen

1. Der Graph Editor MUSS kontinuierliche Validierung unterstützen.
2. Änderungen MÜSSEN inkrementelle Prüfungen auslösen können.
3. Struktur-, Typ- und Verbindungsfehler MÜSSEN erkannt werden.
4. Subgraph- und Script-Schnittstellen MÜSSEN geprüft werden.
5. Capability-Anforderungen MÜSSEN statisch überprüfbar sein.
6. Diagnosen MÜSSEN eindeutige Codes und Quellenreferenzen besitzen.
7. Fehler MÜSSEN direkt im Canvas erkennbar sein.
8. Quick Fixes DÜRFEN keine ungeprüften Änderungen übernehmen.
9. Vor der Ausführung MUSS eine vollständige Validierung erfolgen.
10. Ungültige Graphen DÜRFEN nicht produktiv ausgeführt werden.
11. Validierung DARF keine Capability-Berechtigungen erteilen oder umgehen.
12. Die Validierung MUSS unabhängig von KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine schnelle, integrierte und zuverlässige Graphvalidierung mit unmittelbarer Fehleranzeige, verständlichen Diagnosen und kontrollierten Korrekturmöglichkeiten.
