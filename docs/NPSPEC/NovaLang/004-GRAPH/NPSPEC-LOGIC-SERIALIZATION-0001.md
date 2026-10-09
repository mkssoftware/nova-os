
# NPSPEC-LOGIC-SERIALIZATION-0001 – NovaOS Logic Graph Serialization

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Serialisierung

## Zweck

Definiert die verlustfreie Speicherung, Übertragung und Wiederherstellung von Logic Graphs.

Ziel ist ein portables, versioniertes und deterministisch verarbeitbares Datenformat, das unabhängig vom grafischen Editor funktioniert.

## Architektur

Das Serialization-System besteht aus:

- **Graph Serializer:** Umwandlung von Graphstrukturen in ein persistentes Format.
- **Graph Deserializer:** Wiederherstellung gespeicherter Graphstrukturen.
- **Schema Registry:** Verwaltung unterstützter Formatversionen.
- **Type Codec:** Kodierung von NovaLang-Typen und Werten.
- **Reference Resolver:** Auflösung von Knoten-, Port- und Subgraph-Referenzen.
- **Migration Manager:** Kontrollierte Migration älterer Formate.

## Datenmodell

Ein serialisierter Logic Graph enthält:

- Formatkennung und Schemaversion
- Eindeutige Graph-ID
- Knoten mit stabilen IDs und Typinformationen
- Ports und deren Datentypen
- Verbindungen und Abhängigkeiten
- Subgraph- und Funktionsreferenzen
- Deklarierte Capability-Anforderungen
- Referenzen auf Custom Scripts
- Konfigurationen und deklarierte Anfangszustände

Editor-spezifische Informationen wie Knotenpositionen dürfen getrennt gespeichert werden.

## Serialisierungsformat

Das Format MUSS eine eindeutig definierte, versionierte Struktur besitzen.

Eine kanonische Darstellung ermöglicht reproduzierbare Hashwerte, Integritätsprüfungen und Versionsvergleiche.

Die Serialisierung darf keine ausführbaren Speicheradressen, laufenden Tasks oder flüchtigen Runtime-Handles enthalten.

## NovaLang-Integration

Datentypen müssen den Definitionen des NovaLang-Typsystems entsprechen.

Custom Scripts werden als referenzierte NovaLang-Quellen oder versionierte Artefakte eingebunden.

`.nlf` verwendet unverändert die reguläre NovaLang-Semantik.

## Referenzen und Identität

Knoten, Ports und Verbindungen besitzen stabile Identifikatoren.

Interne Referenzen müssen beim Deserialisieren eindeutig aufgelöst werden.

Die Solution-Identität wird über `solution.xml` verwaltet und darf durch Graph-Serialisierung nicht ersetzt oder gefälscht werden.

## Sicherheit

Deserialisierte Graphen gelten zunächst als nicht vertrauenswürdig.

Vor ihrer Ausführung müssen Schema, Typen, Referenzen, Integrität und Capability-Anforderungen validiert werden.

Serialisierte Berechtigungsinformationen stellen keine erteilten Berechtigungen dar.

## Kompatibilität und Migration

Unbekannte Pflichtfelder oder nicht unterstützte Versionen müssen kontrolliert zurückgewiesen werden.

Optionale Erweiterungen dürfen gemäß Schemavertrag erhalten bleiben.

Migrationen müssen versioniert, nachvollziehbar und ohne stillschweigenden Bedeutungsverlust erfolgen.

## Normative Anforderungen

1. Logic Graphs MÜSSEN verlustfrei serialisierbar und deserialisierbar sein.
2. Das Datenformat MUSS eine explizite Schemaversion besitzen.
3. Knoten-, Port- und Verbindungsidentitäten MÜSSEN erhalten bleiben.
4. NovaLang-Typinformationen MÜSSEN eindeutig abgebildet werden.
5. Referenzen MÜSSEN beim Laden überprüft werden.
6. Eine kanonische Serialisierung MUSS unterstützt werden.
7. Flüchtige Runtime-Zustände DÜRFEN nicht Bestandteil der Graphdefinition sein.
8. Editor-Metadaten DÜRFEN die Ausführungssemantik nicht verändern.
9. Unbekannte oder inkompatible Formatversionen MÜSSEN erkannt werden.
10. Migrationen DÜRFEN keine stillschweigenden semantischen Änderungen verursachen.
11. Deserialisierte Graphen MÜSSEN vor der Ausführung validiert werden.
12. Serialisierte Daten DÜRFEN keine Berechtigungen erteilen oder Capability-Grenzen umgehen.
13. Das Serialization-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält ein portables, versioniertes und sicheres Serialisierungsformat für Logic Graphs, das zuverlässige Speicherung, Austausch, Migration und reproduzierbare Wiederherstellung ermöglicht.
