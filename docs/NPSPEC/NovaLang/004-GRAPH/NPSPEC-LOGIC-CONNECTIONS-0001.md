
# NPSPEC-LOGIC-CONNECTIONS-0001 – NovaOS Logic Graph Connections

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Verbindungsverwaltung

## Zweck

Definiert die Erstellung, Änderung, Validierung und Verwaltung von Verbindungen zwischen Logic-Graph-Knoten.

Während `NPSPEC-LOGIC-EDGES-0001` die Verbindungsstruktur beschreibt, regelt diese Spezifikation deren Verwaltung und Lebenszyklus.

## Architektur

Das Connection-System besteht aus:

- **Connection Manager:** Verwaltung aller Verbindungen.
- **Connection Validator:** Prüfung neuer und geänderter Verbindungen.
- **Type Resolver:** Ermittlung der Portkompatibilität.
- **Connection Editor:** Interaktive Erstellung und Bearbeitung.
- **Change Tracker:** Nachverfolgung von Änderungen.
- **Runtime Synchronizer:** Übernahme gültiger Änderungen in die Ausführung.

Die Verwaltung arbeitet auf dem einheitlichen Logic-Graph-Schema.

## Verbindungsoperationen

Unterstützt werden:

- Verbindung erstellen
- Verbindung entfernen
- Verbindung umleiten
- Verbindung ersetzen
- Mehrere Verbindungen bearbeiten
- Verbindungen suchen und hervorheben
- Änderungen rückgängig machen und wiederherstellen

Alle Operationen müssen konsistente Graphzustände gewährleisten.

## Verbindungsprüfung

Vor dem Erstellen oder Ändern werden geprüft:

- Existenz der beteiligten Knoten und Ports
- Richtung und Art der Ports
- NovaLang-Typkompatibilität
- Kardinalität
- Ausführungsabhängigkeiten
- Zulässigkeit von Zyklen
- Ressourcen- und Capability-Verträge

Ungültige Verbindungen müssen mit einer verständlichen Diagnose abgelehnt werden.

## Automatische Verbindungshilfe

Der Editor darf kompatible Ports hervorheben und passende Verbindungen vorschlagen.

Automatische Typkonvertierungen dürfen nur über eindeutig definierte, zulässige Konvertierungen erfolgen.

Verbindungen mit sicherheitsrelevanten Auswirkungen dürfen nicht unbemerkt hergestellt werden.

## Änderungen zur Laufzeit

Graphänderungen können während einer Preview-Sitzung übernommen werden.

Die Runtime darf ausschließlich vollständig validierte Änderungen aktivieren.

Nicht kompatible Änderungen erfordern eine kontrollierte Unterbrechung oder Neuinitialisierung betroffener Graphteile.

## Normative Anforderungen

1. Der Connection Manager MUSS Verbindungen erstellen, ändern und entfernen können.
2. Jede Operation MUSS die Regeln aus `NPSPEC-LOGIC-PORTS-0001` und `NPSPEC-LOGIC-EDGES-0001` einhalten.
3. Änderungen MÜSSEN vor ihrer Übernahme validiert werden.
4. Mehrstufige Änderungen MÜSSEN atomar übernommen oder zurückgerollt werden.
5. Ungültige Verbindungen DÜRFEN nicht aktiviert werden.
6. Undo und Redo MÜSSEN unterstützt werden.
7. Kompatible Ports SOLLEN visuell hervorgehoben werden.
8. Capability-Grenzen DÜRFEN durch automatische Verbindungen nicht umgangen werden.
9. Laufzeitänderungen MÜSSEN kontrolliert synchronisiert werden.
10. Verbindungsoperationen MÜSSEN unabhängig vom grafischen Editor verfügbar sein.
11. Änderungen MÜSSEN versionierbar und diagnostizierbar sein.
12. Das Connection-System MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält eine sichere und transaktionale Verbindungsverwaltung für die interaktive Bearbeitung und kontrollierte Ausführung von Logic Graphs.
