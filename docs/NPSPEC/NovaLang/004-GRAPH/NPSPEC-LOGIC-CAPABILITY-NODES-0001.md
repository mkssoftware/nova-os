
# NPSPEC-LOGIC-CAPABILITY-NODES-0001 – NovaOS Logic Graph Capability Nodes

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Capabilities

## Zweck

Definiert Capability Nodes als kontrollierte Schnittstelle zwischen Logic Graphs und den Systemfähigkeiten von NovaOS.

Ziel ist die sichere Nutzung von Systemfunktionen ohne direkte Systemzugriffe durch Custom Scripts.

## Architektur

Das Capability-Node-System besteht aus:

- **Capability Node Registry:** Registrierung verfügbarer Capability-Knoten.
- **Capability Resolver:** Auflösung der Capability-ID.
- **Permission Validator:** Prüfung der Solution-Berechtigungen.
- **Capability Bridge:** Verbindung zu autorisierten Systemdiensten.
- **Execution Adapter:** Ausführung von Capability-Operationen.
- **Result Mapper:** Bereitstellung typisierter Ergebnisse.

Die Berechtigungsverwaltung bleibt ausschließlich Aufgabe von NovaOS.

## Capability-Modell

Jeder Capability Node besitzt:

- Eindeutige Node-ID
- Vollständige Capability-ID
- Versionierten Capability-Vertrag
- Typisierte Eingangsports
- Typisierte Ausgangsports
- Deklarierte Berechtigungsanforderungen
- Definiertes Fehlerverhalten

Capability-IDs folgen dem NovaOS-Namensschema `domain.authority.namespace.name`.

## Ausführungsmodell

Ein Capability Node wird über den Logic Graph aktiviert.

1. Eingabedaten werden validiert.
2. NovaOS prüft die Berechtigung der Solution.
3. Die Capability Bridge führt die autorisierte Operation aus.
4. Ergebnisse werden über typisierte Ausgangsports bereitgestellt.
5. Fehler werden über definierte Fehlerausgänge gemeldet.

Eine vorhandene Capability-Referenz stellt keine Berechtigung dar.

## Script-Integration

Custom Scripts verwenden reguläres NovaLang innerhalb von `.nlf`.

Ein Script Node darf keine Capabilities eigenständig anfordern oder aufrufen.

Benötigte Daten und autorisierte Ressourcen werden ausschließlich über vorgeschaltete Capability Nodes und deren Verbindungen bereitgestellt.

## Sicherheit

Berechtigungen sind an die verifizierte Solution-Identität gebunden.

Die dauerhafte GUID allein genügt nicht als Sicherheitsnachweis.

Sicherheitsrelevante Änderungen an einer Solution können eine erneute Berechtigungsprüfung erfordern.

Ressourcenreferenzen dürfen nur innerhalb ihrer autorisierten Zugriffsrechte weitergegeben werden.

## Lebenszyklus

Capability Nodes müssen Initialisierung, Ausführung, Abbruch und Freigabe unterstützen.

Nicht verfügbare oder verweigerte Capabilities müssen definierte Fehler liefern.

Laufende Operationen unterliegen den Ressourcenlimits und Abbruchregeln der Graph Runtime.

## Normative Anforderungen

1. Systemzugriffe aus Logic Graphs MÜSSEN über explizite Capability Nodes erfolgen.
2. Jeder Capability Node MUSS eine vollständige Capability-ID referenzieren.
3. Ein- und Ausgänge MÜSSEN typisierte Verträge besitzen.
4. NovaOS MUSS Berechtigungen vor geschützten Operationen prüfen.
5. Custom Scripts DÜRFEN Capabilities nicht eigenständig anfordern oder aufrufen.
6. Capability-Referenzen DÜRFEN keine Berechtigungen erzeugen.
7. Berechtigungen MÜSSEN an die verifizierte Solution-Identität gebunden sein.
8. Ressourcenrechte DÜRFEN bei der Weitergabe nicht erweitert werden.
9. Verweigerte oder nicht verfügbare Capabilities MÜSSEN definierte Fehler liefern.
10. Operationen MÜSSEN kontrolliert abbrechbar sein.
11. Capability-Nutzung MUSS diagnostizierbar und sicherheitsrelevant protokollierbar sein.
12. Capability Nodes MÜSSEN unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält sichere, typisierte Capability Nodes als verbindliche Schnittstelle zwischen Solutions, Logic Graphs und autorisierten Systemfunktionen.
