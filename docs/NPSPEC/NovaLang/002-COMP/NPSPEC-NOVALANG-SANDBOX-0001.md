
# NPSPEC-NOVALANG-SANDBOX-0001 – NovaLang Sandbox

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Sicherheit

## Zweck

Definiert die sichere Ausführung von NovaLang-Code innerhalb kontrollierter Sandbox-Umgebungen.

Ziel ist die Begrenzung von Systemzugriffen, Ressourcenverbrauch und möglichen Schäden durch fehlerhafte oder nicht vertrauenswürdige Komponenten.

## Architektur

Die Sandbox kombiniert Runtime-Kontrollen mit den Sicherheitsmechanismen von NovaOS.

| Komponente | Aufgabe |
|---|---|
| Sandbox Manager | Erstellung und Verwaltung |
| Execution Context | Isolierter Ausführungszustand |
| Capability Filter | Begrenzung erlaubter Systemzugriffe |
| Resource Controller | CPU-, Speicher- und Task-Limits |
| IPC Gateway | Kontrollierte Kommunikation |
| Runtime Monitor | Überwachung und Fehlererkennung |

Die verbindliche Sicherheitsisolation erfolgt durch NovaOS.

## Sandbox-Profile

| Profil | Verwendung |
|---|---|
| Restricted | Nicht vertrauenswürdiger Code |
| Standard | Gewöhnliche Anwendungen |
| Solution | Logic-Graph- und Solution-Komponenten |
| System | Autorisierte Systemkomponenten |

Profile definieren Einschränkungen, verleihen jedoch keine Berechtigungen.

Jede Sandbox erhält ausschließlich ausdrücklich autorisierte Capabilities.

## Ausführungsregeln

- Jede Sandbox besitzt eine eindeutige Identität.
- Speicher und Ressourcen werden ihrem Ausführungskontext zugeordnet.
- Direkte Zugriffe auf fremde Schutzdomänen sind untersagt.
- Systemzugriffe erfolgen ausschließlich über autorisierte Capability-Handles.
- Nicht benötigte Systemfunktionen bleiben unerreichbar.
- Berechtigungen dürfen während der Ausführung nur durch autorisierte NovaOS-Mechanismen verändert werden.

## Logic Graph und Solutions

Custom-NovaLang-Skripte innerhalb eines Logic Graph dürfen keine Capabilities selbstständig anfordern.

Benötigte Systemfunktionen werden durch explizit verbundene Capability-Knoten bereitgestellt.

Die Sandbox prüft die Verwendung übergebener Daten und Handles entsprechend deren Berechtigungen.

Die Solution-GUID dient der Identifikation, ersetzt jedoch weder Integritätsprüfung noch Autorisierung.

## Ressourcenbegrenzung

Jede Sandbox unterstützt konfigurierbare Grenzen für:

- Arbeitsspeicher
- CPU-Zeit
- Gleichzeitige Tasks und Threads
- Ausführungsdauer
- IPC- und Datenvolumen
- Offene Ressourcen und Handles

Grenzüberschreitungen führen zu kontrollierter Drosselung, Fehlerbehandlung oder Beendigung.

## Native und Unsafe-Komponenten

Nicht vertrauenswürdiger nativer Code darf nicht allein durch sprachinterne Prüfungen als sicher gelten.

Native und Unsafe-Komponenten benötigen geeignete Prozessisolation sowie ausdrücklich autorisierte Speicher- und Systemzugriffe.

AOT-, JIT- und Interpreter-Ausführung unterliegen denselben Sandbox-Policies.

## Fehlerbehandlung

Bei einem Sandbox-Fehler müssen betroffene Ressourcen kontrolliert freigegeben werden.

Diagnosen dürfen keine geschützten Informationen anderer Ausführungskontexte offenlegen.

Ein Neustart erfolgt ausschließlich durch einen autorisierten Supervisor.

## Normative Anforderungen

1. NovaLang MUSS kontrollierte Sandbox-Ausführung unterstützen.
2. Sandbox-Grenzen MÜSSEN durch NovaOS-Sicherheitsmechanismen abgesichert werden.
3. Jede Sandbox MUSS eigene Ressourcenlimits besitzen können.
4. Systemzugriffe MÜSSEN gültige Capability-Handles erfordern.
5. Sandbox-Profile DÜRFEN keine Berechtigungen automatisch erteilen.
6. Logic-Graph-Skripte DÜRFEN keine zusätzlichen Capabilities selbstständig anfordern.
7. Native und Unsafe-Komponenten DÜRFEN Sandbox-Grenzen nicht umgehen.
8. Ressourcenüberschreitungen und Fehler MÜSSEN kontrolliert behandelt werden.
9. AOT, JIT und Interpreter MÜSSEN dieselben Sandbox-Policies einhalten.

## Ergebnis

NovaLang erhält eine capabilitybasierte Sandbox-Architektur mit kontrollierten Systemzugriffen, Ressourcenbegrenzung und NovaOS-gestützter Isolation für Programme, Solutions und nicht vertrauenswürdigen Code.
