
# NPSPEC-LOGIC-VERSIONING-0001 – NovaOS Logic Graph Versioning

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Versionsverwaltung

## Zweck

Definiert die Versionierung, Kompatibilitätsprüfung und Weiterentwicklung von Logic Graphs innerhalb von NovaOS-Solutions.

Ziel ist, Graphen langfristig nutzbar zu halten, Änderungen nachvollziehbar zu machen und bestehende Solutions vor inkompatiblen Änderungen zu schützen.

## Architektur

Das Versioning-System besteht aus:

- **Version Manager:** Verwaltung der Graphversionen.
- **Compatibility Checker:** Prüfung der Kompatibilität.
- **Change Tracker:** Erkennung struktureller und semantischer Änderungen.
- **Dependency Resolver:** Prüfung versionierter Abhängigkeiten.
- **Migration Coordinator:** Koordination erforderlicher Migrationen.
- **Version Diagnostics:** Meldung von Versionskonflikten.

## Versionsmodell

Folgende Versionen werden unterschieden:

- **Schema Version:** Version des Serialisierungsformats.
- **Graph Version:** Version der Graphdefinition.
- **Node Contract Version:** Version der Knotenschnittstellen.
- **Capability Contract Version:** Version verwendeter Fähigkeiten.
- **Solution Version:** Version der übergeordneten Solution.

Die Graphversion verwendet `MAJOR.MINOR.PATCH`.

- **MAJOR:** Inkompatible Änderungen.
- **MINOR:** Rückwärtskompatible Erweiterungen.
- **PATCH:** Kompatible Fehlerkorrekturen.

Die Schemaversion wird unabhängig von der Graphversion verwaltet.

## Änderungsarten

Das System unterscheidet:

- **Strukturelle Änderungen:** Knoten, Ports und Verbindungen.
- **Typänderungen:** Datentypen und Schnittstellen.
- **Verhaltensänderungen:** Ausführungslogik und Seiteneffekte.
- **Abhängigkeitsänderungen:** Capabilities, Subgraphs und Scripts.
- **Metadatenänderungen:** Dokumentation und Editorinformationen.

Reine Layoutänderungen dürfen die semantische Graphversion unverändert lassen.

## Kompatibilität

Eine neue Graphversion ist rückwärtskompatibel, wenn bestehende gültige Aufrufer ihre vertraglich zugesicherte Funktionalität weiterhin nutzen können.

Inkompatible Änderungen umfassen beispielsweise:

- Entfernte öffentliche Eingangs- oder Ausgangsports
- Nicht kompatible Typänderungen
- Veränderte Pflichtparameter
- Geänderte Fehler- oder Ausführungsverträge
- Neue verpflichtende Capability-Anforderungen

Kompatibilität muss anhand der tatsächlichen Schnittstellen und Verträge geprüft werden, nicht allein anhand der Versionsnummer.

## Abhängigkeiten

Subgraphs, Funktionen und Capability Nodes können versionierte Verträge referenzieren.

Der Dependency Resolver prüft, ob die benötigten Versionen verfügbar und kompatibel sind.

Nicht auflösbare Abhängigkeiten verhindern die betroffene Ausführung.

## Migration

Ältere Graphversionen können über definierte Migrationsregeln aktualisiert werden.

Migrationen müssen:

- Ausgangs- und Zielversion benennen.
- Änderungen nachvollziehbar dokumentieren.
- Bestehende Identitäten möglichst erhalten.
- Daten- und Schnittstellenverluste erkennen.
- Vor der Übernahme validiert werden.

Inkompatible Änderungen dürfen nicht stillschweigend übernommen werden.

## Sicherheit und Solution-Identität

Graphänderungen dürfen die Solution-Identität nicht umgehen.

Die dauerhafte Solution-GUID wird über `solution.xml` verwaltet.

Sicherheitsrelevante Änderungen, insbesondere neue oder erweiterte Capability-Anforderungen, müssen eine erneute Integritäts- und Berechtigungsprüfung auslösen.

Eine Versionsnummer allein gilt nicht als Integritätsnachweis.

## Normative Anforderungen

1. Jeder persistente Logic Graph MUSS eine eindeutige Graph-ID und Graphversion besitzen.
2. Schema- und Graphversion MÜSSEN getrennt verwaltet werden.
3. Graphversionen MÜSSEN `MAJOR.MINOR.PATCH` verwenden.
4. Öffentliche Knoten- und Portverträge MÜSSEN versionierbar sein.
5. Inkompatible Schnittstellenänderungen MÜSSEN erkannt werden.
6. Versionsnummern DÜRFEN nicht als alleiniger Kompatibilitätsnachweis gelten.
7. Versionierte Abhängigkeiten MÜSSEN vor der Ausführung geprüft werden.
8. Migrationen MÜSSEN nachvollziehbar und validierbar sein.
9. Inkompatible Migrationen DÜRFEN nicht automatisch ohne ausdrückliche Freigabe erfolgen.
10. Editor-Metadaten DÜRFEN die Ausführungssemantik nicht verändern.
11. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Berechtigungsprüfung ermöglichen beziehungsweise auslösen.
12. Versionierung DARF keine Capability- oder Integritätsprüfungen umgehen.
13. Das Versioning-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine langfristig wartbare und sichere Versionsverwaltung für Logic Graphs mit klaren Kompatibilitätsregeln, versionierten Abhängigkeiten und kontrollierten Migrationen.
