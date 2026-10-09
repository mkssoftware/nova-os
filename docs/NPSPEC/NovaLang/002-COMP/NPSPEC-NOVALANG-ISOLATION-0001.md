
# NPSPEC-NOVALANG-ISOLATION-0001 – NovaLang Execution Isolation

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Isolation

## Zweck

Definiert die Isolation von NovaLang-Programmen, Tasks und Solution-Komponenten.

Ziel ist die sichere Trennung von Speicher, Ressourcen und Berechtigungen sowie die Begrenzung von Fehlerauswirkungen.

## Isolationsmodell

NovaLang unterscheidet drei Isolationsebenen:

| Ebene | Beschreibung |
|---|---|
| Task Isolation | Getrennte Ausführungszustände innerhalb eines Kontexts |
| Runtime Isolation | Getrennte Heaps, Module und Ressourcen |
| Process Isolation | Hardwaregestützte Speicher- und Prozessisolation durch NovaOS |

Task-Isolation allein stellt keine Sicherheitsgrenze dar.

Unterschiedliche Vertrauensbereiche müssen durch geeignete NovaOS-Schutzmechanismen getrennt werden.

## Execution Context

Jeder isolierte Ausführungskontext besitzt:

- Eindeutige Identität
- Definierte Modul- und Laufzeitumgebung
- Zugewiesene Speicher- und Ressourcenbudgets
- Autorisierte Capability-Handles
- Eigene Fehler- und Cancellation-Zustände

Berechtigungen dürfen nicht automatisch zwischen Kontexten übertragen werden.

## Speicherisolation

- Direkte Zugriffe auf fremde Speicherbereiche sind unzulässig.
- Verwaltete Referenzen dürfen Schutzdomänen nicht unkontrolliert überschreiten.
- Shared Memory benötigt explizite Freigabe und Zugriffsrechte.
- Native und Unsafe-Komponenten benötigen zusätzliche Schutzmaßnahmen.
- Garbage Collection darf keine fremden Schutzdomänen beschädigen.

## Kommunikation

Isolierte Komponenten kommunizieren über definierte IPC-Schnittstellen.

Zulässige Mechanismen:

- Typisierte Nachrichten
- Serialisierte Daten
- Autorisierte Shared-Memory-Bereiche
- Explizit übertragbare Capability-Handles

Capability-Übertragungen müssen durch die NovaOS-Policy erlaubt sein.

## Fehlerisolation

Ein Fehler innerhalb eines isolierten Kontexts darf andere Schutzdomänen nicht unkontrolliert beeinflussen.

Die Runtime unterstützt:

- Kontrollierten Kontextabbruch
- Hierarchische Cancellation
- Ressourcenbereinigung
- Fehlerdiagnostik
- Neustart durch autorisierte Supervisoren

Ein Neustart darf keine zusätzlichen Berechtigungen erzeugen.

## NovaOS-Integration

- NovaOS stellt die verbindlichen Sicherheitsgrenzen bereit.
- Die NovaLang Runtime verwaltet logische Ausführungskontexte.
- Der Bytecode-Verifier ergänzt die Isolation durch statische Prüfungen.
- AOT-, JIT- und Interpreter-Code unterliegen denselben Schutzregeln.
- Logic-Graph-Skripte erhalten ausschließlich explizit bereitgestellte Capabilities.
- Nicht vertrauenswürdige native Module müssen separat isoliert werden.

## Normative Anforderungen

1. NovaLang MUSS getrennte Ausführungskontexte unterstützen.
2. Unterschiedliche Vertrauensbereiche MÜSSEN durch wirksame NovaOS-Schutzgrenzen isoliert werden.
3. Speicherzugriffe über Schutzdomänengrenzen hinweg DÜRFEN NICHT unautorisiert erfolgen.
4. Kommunikation zwischen Schutzdomänen MUSS über kontrollierte Schnittstellen erfolgen.
5. Capability-Handles DÜRFEN nur nach gültiger Autorisierung übertragen werden.
6. Fehler und Ressourcenüberschreitungen MÜSSEN auf den betroffenen Kontext begrenzt werden, soweit die Schutzgrenze dies gewährleistet.
7. Unsafe- und native Komponenten DÜRFEN die Isolation nicht umgehen.
8. AOT, JIT und Interpreter MÜSSEN dieselben Sicherheitsanforderungen erfüllen.
9. Isolation DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält ein mehrstufiges Isolationsmodell für Tasks, Runtime-Kontexte und Prozesse. Die Runtime organisiert die Ausführung, während NovaOS verbindliche Speicher-, Prozess- und Capability-Sicherheitsgrenzen durchsetzt.
