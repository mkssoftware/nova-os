
# NPSPEC-LOGIC-EVENTS-0001 – NovaOS Logic Graph Events

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Ereignisverarbeitung

## Zweck

Definiert die ereignisgesteuerte Kommunikation innerhalb eines NovaOS Logic Graphs.

Ziel ist die zuverlässige, typisierte und kontrollierte Verarbeitung von System-, Benutzer- und Anwendungsereignissen.

## Architektur

Das Event-System besteht aus:

- **Event Manager:** Zentrale Verwaltung von Ereignissen.
- **Event Registry:** Registrierung verfügbarer Ereignistypen.
- **Event Dispatcher:** Zustellung an verbundene Knoten.
- **Event Queue:** Zwischenspeicherung eingehender Ereignisse.
- **Subscription Manager:** Verwaltung von Ereignisabonnements.
- **Event Monitor:** Diagnose und Nachverfolgung.

Die Ereignisverarbeitung ist in die Graph Runtime integriert.

## Ereignistypen

Unterstützt werden:

- **UI Events:** Benutzerinteraktionen.
- **System Events:** Ereignisse autorisierter NovaOS-Capabilities.
- **Data Events:** Änderungen von Daten und Zuständen.
- **Timer Events:** Zeitgesteuerte Auslösung.
- **Custom Events:** Benutzerdefinierte Ereignisse.
- **Lifecycle Events:** Start, Beendigung und Zustandswechsel.

## Ereignismodell

Jedes Ereignis besitzt:

- Eindeutigen Ereignistyp
- Typisierte Nutzdaten
- Quellenreferenz
- Optionale Zeit- und Korrelationsinformationen

Ereignisse werden über Event Ports und Event Edges übertragen.

Die Zustellung erfolgt gemäß den definierten Ausführungs- und Reihenfolgeregeln.

## Verarbeitung

Ereignisse können Graph-Ausführungen starten, fortsetzen oder beeinflussen.

Mehrere Ereignisse dürfen parallel verarbeitet werden, sofern keine Abhängigkeiten oder Synchronisationsregeln entgegenstehen.

Ereigniswarteschlangen müssen begrenzt und bei Überlast kontrolliert behandelt werden.

## Capability-Integration

Systemereignisse werden ausschließlich über autorisierte Capability Nodes bereitgestellt.

Custom Scripts dürfen keine nicht autorisierten Systemereignisse abonnieren.

Ereignisabonnements unterliegen den Berechtigungen der verifizierten Solution-Identität.

## Lebenszyklus

Abonnements müssen beim Beenden oder Entfernen betroffener Knoten zuverlässig freigegeben werden.

Ausstehende Ereignisse müssen beim Abbruch kontrolliert verworfen oder gemäß ihrem Vertrag abgeschlossen werden.

## Normative Anforderungen

1. Die Graph Runtime MUSS typisierte Ereignisse unterstützen.
2. Ereignistypen MÜSSEN eindeutig identifizierbar sein.
3. Ereignisdaten MÜSSEN mit dem NovaLang-Typsystem kompatibel sein.
4. Ereignisse MÜSSEN über definierte Event Ports übertragen werden.
5. Abonnements MÜSSEN kontrolliert erstellt und entfernt werden.
6. Ereigniswarteschlangen MÜSSEN Ressourcenlimits einhalten.
7. Überlast und Rückstau MÜSSEN definiert behandelt werden.
8. Ereignisreihenfolgen MÜSSEN gemäß ihrem Vertrag eingehalten werden.
9. Systemereignisse MÜSSEN über autorisierte Capability Nodes bereitgestellt werden.
10. Ereignisse DÜRFEN keine zusätzlichen Berechtigungen erzeugen.
11. Fehler, verlorene Ereignisse und Abbrüche MÜSSEN diagnostizierbar sein.
12. Das Event-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält ein typisiertes, ressourcenkontrolliertes und sicheres Ereignissystem für die reaktive Ausführung von Logic Graphs.
