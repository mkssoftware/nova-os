
# NPSPEC-LOGIC-SCRIPT-ISOLATION-0001 – NovaOS Logic Graph Script Isolation

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Sicherheit / Isolation

## Zweck

Definiert die sichere Isolation von Custom-NovaLang-Skripten innerhalb eines Logic Graphs.

Ziel ist die Verhinderung unautorisierter Systemzugriffe, unkontrollierter Ressourcenverwendung und gegenseitiger Beeinflussung von Solutions.

## Architektur

Die Script Isolation besteht aus:

- **Isolation Manager:** Verwaltung isolierter Ausführungskontexte.
- **Script Sandbox:** Begrenzung der Skriptausführung.
- **Memory Boundary:** Schutz von Speicherbereichen.
- **Capability Guard:** Durchsetzung erlaubter Zugriffe.
- **Resource Limiter:** Begrenzung von CPU, Speicher und Laufzeit.
- **Fault Handler:** Behandlung von Sicherheits- und Laufzeitfehlern.

Die Isolation wird durch NovaLang Runtime und NovaOS-Sicherheitsmechanismen durchgesetzt.

## Isolationsmodell

Jeder Script Node wird in einem kontrollierten Ausführungskontext ausgeführt.

- Kein direkter Zugriff auf Kernel oder Hardware
- Kein unautorisierter Datei- oder Netzwerkzugriff
- Kein Zugriff auf fremde Solution-Zustände
- Kein Zugriff auf interne Speicherbereiche anderer Skripte
- Keine eigenständige Anforderung von System-Capabilities

Die Isolationsgrenzen gelten unabhängig von der verwendeten NovaLang-Ausführungsart.

## Datenaustausch

Skripte kommunizieren ausschließlich über deklarierte Ports und freigegebene Daten.

Autorisierte Ressourcenreferenzen dürfen nur gemäß ihrem ursprünglichen Vertrag verwendet werden.

Gemeinsamer Speicher ist ausschließlich über ausdrücklich kontrollierte Mechanismen zulässig.

## Capability-Sicherheit

Custom Scripts dürfen keine Capabilities selbstständig aufrufen oder anfordern.

Systemoperationen werden ausschließlich durch explizite Capability Nodes ausgeführt.

Der Capability Guard überprüft die Gültigkeit übergebener Ressourcen und verhindert die Erweiterung bestehender Zugriffsrechte.

## Ressourcen und Fehler

Jede Ausführung unterliegt definierten Ressourcenlimits.

Bei Zeitüberschreitung, Speicherverletzung oder unzulässigem Zugriff wird die betroffene Ausführung kontrolliert beendet.

Fehler dürfen nicht zum Absturz der gesamten Graph Runtime oder anderer Solutions führen.

## Normative Anforderungen

1. Custom Scripts MÜSSEN in isolierten Ausführungskontexten ausgeführt werden.
2. Speichergrenzen MÜSSEN durch die Runtime beziehungsweise NovaOS durchgesetzt werden.
3. Direkte Kernel- und Hardwarezugriffe MÜSSEN verhindert werden.
4. Scripts DÜRFEN keine Capabilities eigenständig anfordern oder aufrufen.
5. Systemoperationen MÜSSEN über explizite Capability Nodes erfolgen.
6. Datenzugriffe MÜSSEN auf deklarierte Eingaben und autorisierte Ressourcen begrenzt sein.
7. Ressourcenreferenzen DÜRFEN keine zusätzlichen Berechtigungen vermitteln.
8. CPU-, Speicher- und Laufzeitlimits MÜSSEN durchsetzbar sein.
9. Fehlerhafte Skripte MÜSSEN kontrolliert beendet werden können.
10. Eine Script-Verletzung DARF andere Solutions nicht kompromittieren.
11. Sicherheitsverletzungen MÜSSEN diagnostizierbar und protokollierbar sein.
12. Die Isolation MUSS für Interpreter, JIT und AOT dieselben Sicherheitsgarantien bieten und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine verbindliche Sicherheitsisolation für Custom Scripts, die kontrollierte Datenverarbeitung ermöglicht und gleichzeitig Speicher-, Ressourcen- und Capability-Grenzen zuverlässig durchsetzt.
