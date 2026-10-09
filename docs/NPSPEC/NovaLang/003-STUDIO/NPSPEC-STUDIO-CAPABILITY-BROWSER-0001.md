
# NPSPEC-STUDIO-CAPABILITY-BROWSER-0001 – NovaLang Studio Capability Browser

## Status

Angenommen

## Kategorie

NovaLang Studio / Capabilities / Entwicklungswerkzeuge

## Zweck

Definiert den integrierten Capability Browser zur Suche, Untersuchung und Verwendung verfügbarer NovaOS-Fähigkeiten.

Ziel ist, Systemfunktionen direkt in Solutions und Logic Graphs einzubinden, ohne deren technische Implementierung kennen zu müssen.

## Architektur

Der Capability Browser besteht aus:

- **Capability Registry Connector:** Zugriff auf registrierte Fähigkeiten.
- **Capability Catalog:** Strukturierte Übersicht verfügbarer Capabilities.
- **Search Engine:** Suche und Filterung.
- **Contract Inspector:** Anzeige typisierter Schnittstellen.
- **Dependency Resolver:** Prüfung von Abhängigkeiten und Versionen.
- **Graph Integration:** Einfügen von Capabilities in den Logic Graph.

Die Capability Registry bleibt die maßgebliche Quelle für Identität und Verfügbarkeit.

## Capability-Modell

Jede Capability besitzt mindestens:

- Eindeutige Capability-ID
- Name und Beschreibung
- Version
- Typisierte Ein- und Ausgänge
- Ereignisse und Fehlerverträge
- Erforderliche Berechtigungen
- Verfügbarkeitsstatus

Capability-IDs folgen dem Schema:

`domain.authority.namespace.name`

Beispiel: `de.nova.network.http.request`

Die Kategorie im Browser bestimmt nicht die Identität einer Capability.

## Funktionen

- Suche nach Name, ID und Funktion
- Kategorien und Filter
- Anzeige von Schnittstellen und Dokumentation
- Prüfung der Versionskompatibilität
- Anzeige erforderlicher Berechtigungen
- Drag-and-Drop in den Logic Graph
- Navigation zu verwendeten Capabilities
- Anzeige fehlender oder inkompatibler Fähigkeiten

Häufig benötigte Funktionen müssen unmittelbar erreichbar sein.

## Logic-Graph-Integration

Capabilities werden als typisierte Knoten in den Logic Graph eingefügt.

Sie können mit anderen Capabilities und Custom-NovaLang-Skripten verbunden werden.

Custom Scripts erhalten ausschließlich die über ihre Graph-Verbindungen bereitgestellten Daten und Handles.

Sie dürfen keine eigenständigen Systemberechtigungen anfordern.

## Berechtigungen

Der Browser unterscheidet:

- Capability verfügbar
- Capability nicht verfügbar
- Berechtigung erforderlich
- Berechtigung erteilt
- Zugriff verweigert
- Version inkompatibel

Das Auswählen oder Einfügen einer Capability erteilt keine Berechtigung.

Berechtigungen werden ausschließlich durch NovaOS anhand der verifizierten Solution-Identität verwaltet.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Capability Browser bereitstellen.
2. Capabilities MÜSSEN über ihre eindeutigen IDs identifiziert werden.
3. Suche, Filterung und Schnittstelleninspektion MÜSSEN unterstützt werden.
4. Ein- und Ausgänge MÜSSEN typisiert dargestellt werden.
5. Capabilities MÜSSEN direkt in den Logic Graph eingefügt werden können.
6. Versionen und Abhängigkeiten MÜSSEN geprüft werden.
7. Berechtigungsstatus und Verfügbarkeit MÜSSEN getrennt dargestellt werden.
8. Das Einfügen einer Capability DARF keine Berechtigung erteilen.
9. Custom Scripts DÜRFEN Systemzugriffe nur über explizite Capability-Verbindungen erhalten.
10. Capability- und Sicherheitsgrenzen MÜSSEN vollständig eingehalten werden.
11. Der Capability Browser MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält einen zentralen Capability Browser, mit dem Entwickler NovaOS-Fähigkeiten entdecken, verstehen und sicher zu Solutions und Logic Graphs zusammenstellen können.
