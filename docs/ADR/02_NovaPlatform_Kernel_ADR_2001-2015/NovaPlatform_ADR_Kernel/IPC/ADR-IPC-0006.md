# ADR-IPC-0006 – Remote Procedure Calls

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0006 |
| Titel | Remote Procedure Calls |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation und Systemdienste |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-IPC-0002, ADR-IPC-0003, ADR-IPC-0005, ADR-KERNEL-0105 |
| Nachfolgende Spezifikation | NPSPEC-IPC-RPC |

---

## 1. Kontext

NovaOS benötigt eine einheitliche Schnittstelle für strukturierte Aufrufe zwischen Anwendungen, Systemdiensten und ausgewählten Userspace-Treibern.

Message Passing stellt bereits den grundlegenden Nachrichtentransport bereit. Anwendungen sollen jedoch nicht für jeden Dienst manuell Nachrichtenheader, Serialisierung, Antwortzuordnung und Fehlerbehandlung implementieren müssen.

RPC bildet deshalb eine höhere Abstraktion über dem NovaOS Message Passing.

---

## 2. Problemstellung

Ein RPC-System muss folgende Anforderungen erfüllen:

- stabile und versionierbare Schnittstellen
- eindeutige Zuordnung von Anfrage und Antwort
- kontrollierte Übergabe von Capabilities
- Unterstützung synchroner und asynchroner Aufrufe
- Timeouts und Abbruch
- zuverlässige Fehlerübertragung
- Schutz vor ungültigen oder bösartigen Nachrichten
- Begrenzung von Ressourcen und Aufrufketten

RPC darf nicht den Eindruck erzeugen, ein Aufruf in einen anderen Prozess verhalte sich exakt wie ein lokaler Funktionsaufruf.

---

## 3. Entscheidung

NovaOS implementiert ein capability-basiertes RPC-System auf Grundlage von Message Channels.

Ein RPC-Endpunkt ist ein Message-Channel-Endpunkt, der ein versioniertes Interface anbietet.

RPC wird zunächst ausschließlich für lokale Prozesskommunikation definiert. Netzwerk-RPC ist nicht Bestandteil dieser ADR.

---

## 4. Architekturmodell

```text
Client
  |
  | RPC-Anfrage
  v
Message Channel
  |
  v
RPC-Dispatcher
  |
  v
Servermethode
  |
  | RPC-Antwort
  v
Client
```

Der Kernel transportiert die Nachrichten und Capabilities. Die RPC-Laufzeit übernimmt:

- Kodierung und Dekodierung
- Methodenauflösung
- Korrelations-IDs
- Timeoutverwaltung
- Fehlerabbildung
- Interfaceprüfung

---

## 5. Interface-Identität

Jedes RPC-Interface besitzt:

- eine global eindeutige Interface-ID,
- einen symbolischen Namen,
- eine Major-Version,
- eine Minor-Version,
- eine Menge definierter Methoden.

```c
typedef struct rpc_interface_id {
    uint64_t high;
    uint64_t low;
    uint16_t major_version;
    uint16_t minor_version;
} rpc_interface_id_t;
```

Die numerische Interface-ID ist für die technische Identifikation verbindlich. Der Name dient der Dokumentation und Entwicklung.

---

## 6. Versionsmodell

RPC-Interfaces verwenden semantisch getrennte Major- und Minor-Versionen.

| Änderung | Versionsfolge |
|---|---|
| Rückwärtskompatible Erweiterung | Minor-Version erhöhen |
| Neue optionale Methode | Minor-Version erhöhen |
| Änderung bestehender Feldbedeutung | Major-Version erhöhen |
| Entfernen einer Methode | Major-Version erhöhen |
| Inkompatible Änderung eines Datentyps | Major-Version erhöhen |

Ein Client darf einen Server nur verwenden, wenn eine kompatible Interfaceversion ausgehandelt wurde.

Unbekannte optionale Felder müssen übersprungen werden können.

---

## 7. Methodenidentität

Jede Methode besitzt innerhalb eines Interfaces eine stabile numerische ID.

```c
typedef uint32_t rpc_method_id_t;
```

Einmal veröffentlichte Methoden-IDs dürfen innerhalb derselben Major-Version nicht neu vergeben oder mit anderer Bedeutung verwendet werden.

Methoden erhalten zusätzlich einen menschenlesbaren Namen, der jedoch nicht für die Laufzeitauflösung verwendet wird.

---

## 8. Nachrichtenformat

Eine RPC-Anfrage enthält mindestens:

```c
typedef struct rpc_request_header {
    rpc_interface_id_t interface_id;
    uint32_t method_id;
    uint32_t flags;
    uint64_t call_id;
    uint64_t deadline_ns;
    uint32_t payload_size;
    uint32_t capability_count;
} rpc_request_header_t;
```

Eine Antwort enthält:

```c
typedef struct rpc_response_header {
    uint64_t call_id;
    int32_t status;
    uint32_t flags;
    uint32_t payload_size;
    uint32_t capability_count;
} rpc_response_header_t;
```

`call_id` ordnet die Antwort eindeutig einer Anfrage zu.

Alle Größenangaben werden vor dem Zugriff vollständig validiert.

---

## 9. Aufrufarten

NovaOS unterstützt folgende RPC-Arten:

| Aufrufart | Verhalten |
|---|---|
| Synchron | Client wartet auf die Antwort |
| Asynchron | Client erhält ein Future- oder Completion-Objekt |
| One-Way | Keine reguläre Antwort erwartet |
| Streaming | Spätere Erweiterung für mehrere Datenblöcke |

One-Way-Aufrufe dürfen nur für Methoden verwendet werden, bei denen der Client keine fachliche Erfolgsbestätigung benötigt.

Die Annahme der Nachricht durch den Transport bedeutet nicht, dass der Server die Methode erfolgreich ausgeführt hat.

---

## 10. API

### 10.1 Synchroner Aufruf

```c
status_t rpc_call(
    cap_handle_t endpoint,
    const rpc_request_t *request,
    rpc_response_t *response,
    uint64_t timeout_ns
);
```

### 10.2 Asynchroner Aufruf

```c
status_t rpc_call_async(
    cap_handle_t endpoint,
    const rpc_request_t *request,
    cap_handle_t *completion_handle
);
```

### 10.3 Auf Abschluss warten

```c
status_t rpc_await(
    cap_handle_t completion_handle,
    rpc_response_t *response,
    uint64_t timeout_ns
);
```

### 10.4 Aufruf abbrechen

```c
status_t rpc_cancel(
    cap_handle_t completion_handle
);
```

---

## 11. Discovery und Verbindung

Dienste können ihren RPC-Endpunkt über einen Named Object Namespace veröffentlichen.

Beispiel:

```text
/system/services/display
/system/services/audio
/session/services/clipboard
```

Der Client löst den Namen auf und erhält eine Capability für den RPC-Endpunkt.

Die Kenntnis des Servicenamens allein berechtigt nicht zum Zugriff. Der Namespace und der Namenseintrag begrenzen die ausgegebenen Rechte.

---

## 12. Serialisierung

NovaOS verwendet ein binäres, schemaorientiertes Datenformat.

Das Format muss:

- feste primitive Datentypen besitzen,
- Längen eindeutig kodieren,
- unbekannte optionale Felder überspringen,
- verschachtelte Strukturen begrenzen,
- Größen- und Integerüberläufe verhindern,
- zwischen 32-Bit- und 64-Bit-Prozessen kompatibel sein,
- von der nativen Speicherstruktur unabhängig sein.

Rohes Übertragen von C-Strukturen ist nicht zulässig, da Padding, Alignment und Zeigergrößen variieren können.

---

## 13. Capability-Übergabe

RPC-Anfragen und Antworten dürfen Capabilities übertragen.

Die Capability-Anhänge werden getrennt von der serialisierten Nutzlast transportiert und dort über Indizes referenziert.

Beispiel:

```text
payload.shared_buffer = capability[0]
```

Der Empfänger darf nur die vom Kernel tatsächlich übertragenen Capabilities verwenden.

Capability-Übertragungen bleiben atomarer Bestandteil der zugrunde liegenden IPC-Nachricht.

---

## 14. Große Datenmengen

RPC-Nachrichten sind für Steuerdaten und kleinere Nutzlasten vorgesehen.

Große Datenmengen werden über Shared Memory übertragen:

1. Client oder Server erzeugt ein Shared-Memory-Objekt.
2. Die Daten werden dort abgelegt.
3. Die Capability wird mit dem RPC-Aufruf übertragen.
4. Die RPC-Nutzlast beschreibt Offset, Länge und Datenformat.
5. Der Empfänger mappt den erforderlichen Bereich.

Der Empfänger muss alle Bereiche gegen die tatsächliche Objektgröße prüfen.

---

## 15. Timeouts und Deadlines

Jeder RPC-Aufruf kann eine Deadline besitzen.

Nach Überschreitung der Deadline:

- beendet der Client sein Warten,
- wird der Aufruf als abgelaufen markiert,
- darf eine Abbruchanforderung an den Server gesendet werden,
- werden verspätete Antworten verworfen.

Ein Timeout garantiert nicht, dass der Server die Verarbeitung beendet hat.

Methoden mit dauerhaften Nebenwirkungen müssen deshalb so gestaltet werden, dass Wiederholungen und unklare Ergebnisse sicher behandelt werden können.

---

## 16. Abbruch

Ein Client kann einen asynchronen Aufruf abbrechen.

Der Server erhält eine Abbruchbenachrichtigung, sofern die Methode noch aktiv ist.

Abbruch ist kooperativ:

- Noch nicht gestartete Aufrufe können verworfen werden.
- Laufende Methoden prüfen definierte Abbruchpunkte.
- Nicht unterbrechbare Änderungen dürfen konsistent abgeschlossen werden.
- Bereits abgeschlossene Operationen können nicht rückgängig gemacht werden.

Der Server darf keinen inkonsistenten Zustand hinterlassen.

---

## 17. Fehlerbehandlung

RPC unterscheidet drei Fehlerklassen:

| Fehlerklasse | Beispiel |
|---|---|
| Transportfehler | Endpunkt geschlossen, Timeout |
| Protokollfehler | Ungültige Nachricht, unbekannte Methode |
| Dienstfehler | Datei nicht gefunden, Zugriff verweigert |

Transport- und Protokollfehler werden durch standardisierte NovaOS-Statuscodes dargestellt.

Dienstspezifische Fehler werden innerhalb des versionierten Interfaces definiert.

Fehlermeldungen dürfen keine vertraulichen Kernel- oder Serverdetails offenlegen.

---

## 18. Server-Dispatcher

Ein RPC-Server registriert einen Dispatcher für ein Interface:

```c
status_t rpc_server_register(
    cap_handle_t endpoint,
    const rpc_interface_descriptor_t *interface,
    rpc_dispatch_fn dispatcher
);
```

Der Dispatcher muss vor dem Methodenaufruf prüfen:

- Interface-ID und Version
- Methoden-ID
- Nachrichtengröße
- Anzahl der Capability-Anhänge
- Parametergrenzen
- aufruferspezifische Berechtigungen
- Deadline und Abbruchstatus

Ungültige Aufrufe dürfen nicht an die eigentliche Dienstimplementierung weitergegeben werden.

---

## 19. Identität und Autorisierung

Der Besitz einer Endpoint-Capability autorisiert die Verbindung, aber nicht zwingend jede Methode.

Der Server kann zusätzlich prüfen:

- Sicherheitskontext des Clients
- Capability-Rechte
- Session- und Benutzerzugehörigkeit
- Job-Object-Zugehörigkeit
- methodenspezifische Berechtigungen
- übertragene Objekt-Capabilities

Der Server darf niemals vom Inhalt eines vom Client selbst angegebenen Identitätsfeldes ausgehen.

Vertrauenswürdige Identitätsinformationen müssen vom Kernel oder der IPC-Laufzeit bereitgestellt werden.

---

## 20. Deadlocks und verschachtelte Aufrufe

Synchrone RPC-Aufrufe können Deadlocks verursachen, insbesondere wenn Server während der Bearbeitung synchron zum Client zurückrufen.

Deshalb gelten folgende Regeln:

- Verschachtelte synchrone Rückrufe sind zu vermeiden.
- Server dürfen keine internen Sperren über blockierende RPC-Aufrufe hinweg halten.
- Aufrufketten erhalten eine begrenzte Tiefe.
- Kritische Systemdienste sollen asynchrone Schnittstellen bevorzugen.
- Timeouts ersetzen kein korrektes Deadlock-Design.

Die RPC-Laufzeit darf zyklische Aufrufabhängigkeiten diagnostisch erfassen, garantiert aber keine vollständige Deadlock-Erkennung.

---

## 21. Ressourcenbegrenzung

RPC-Ressourcen werden begrenzt:

- maximale Nachrichtengröße
- maximale Capability-Anzahl
- maximale parallele Aufrufe pro Client
- maximale Aufrufwarteschlange pro Server
- maximale Verschachtelungstiefe
- maximale Dekodierungstiefe
- maximale Antwortgröße
- maximale Lebenszeit eines Aufrufs

Bei Überlastung darf der Server Aufrufe mit `STATUS_BUSY` oder `STATUS_LIMIT_EXCEEDED` ablehnen.

---

## 22. Sicherheit und Audit

Für RPC gelten folgende Sicherheitsregeln:

- Jeder Aufruf benötigt eine gültige Endpoint-Capability.
- Alle Nachrichten werden als nicht vertrauenswürdig behandelt.
- Serialisierte Zeiger sind verboten.
- Capability-Indizes werden vollständig validiert.
- Dekodierungsgrößen und Verschachtelungstiefen sind begrenzt.
- Ungültige Methoden dürfen keine Serverfunktionen aufrufen.
- W+X-Shared-Memory darf nicht durch RPC-Berechtigungen umgangen werden.

Audit-Ereignisse können erzeugt werden bei:

- verweigerten privilegierten Methoden
- ungültigen Capability-Übertragungen
- wiederholten Protokollverletzungen
- Aufrufen sicherheitskritischer Dienste
- administrativen RPC-Operationen
- außergewöhnlicher Ressourcenüberschreitung

Normale RPC-Nutzdaten werden nicht standardmäßig protokolliert.

---

## 23. Alternative Ansätze

### 23.1 Manuelles Message Passing für jeden Dienst

**Vorteile:**

- geringe zentrale Komplexität
- maximale Freiheit für einzelne Dienste

**Nachteile:**

- viele inkompatible Protokolle
- wiederholte Implementierung derselben Mechanismen
- schwierige Versionierung

**Entscheidung:** Message Passing bleibt Transport, RPC stellt die gemeinsame Dienstabstraktion bereit.

### 23.2 RPC als transparenter lokaler Funktionsaufruf

**Vorteile:**

- einfache Verwendung

**Nachteile:**

- Netzwerk- und Prozessfehler werden verborgen
- Timeouts und Teilerfolge sind schwer erkennbar
- fördert unsicheres API-Design

**Entscheidung:** Abgelehnt. RPC bleibt im API und Fehlermodell ausdrücklich sichtbar.

### 23.3 Textbasierte Serialisierung

**Vorteile:**

- leicht lesbar und testbar
- einfache Diagnose

**Nachteile:**

- höherer Speicher- und Rechenaufwand
- weniger geeignet für zentrale Systemdienste

**Entscheidung:** Binäres Schemaformat für die native RPC-ABI; Textformate bleiben für externe APIs möglich.

---

## 24. Konsequenzen

### Positive Konsequenzen

- einheitliche Dienstschnittstellen
- stabile Interfaceversionierung
- automatische Zuordnung von Antworten
- standardisierte Timeouts und Fehler
- kontrollierte Capability-Übergabe
- kompatibel mit 32-Bit- und 64-Bit-Prozessen
- gute Grundlage für generierte Client- und Server-Stubs

### Negative Konsequenzen

- zusätzliche RPC-Laufzeit und Serialisierung
- komplexere Fehlerfälle als bei lokalen Aufrufen
- synchrone Aufrufe können Deadlocks verursachen
- Interfaces müssen langfristig gepflegt werden

### Risiken

- fehlerhafte Deserialisierung
- unklare Ergebnisse nach Timeouts
- Überlastung eines zentralen Dienstes
- zu tiefe RPC-Abhängigkeiten
- inkompatible Interfaceänderungen

---

## 25. Implementierungsrichtlinien

- Interface- und Methoden-IDs müssen nach Veröffentlichung stabil bleiben.
- Rohzeiger dürfen niemals serialisiert werden.
- Jede Länge und jeder Index muss validiert werden.
- Server dürfen keine Sperren über blockierende RPC-Aufrufe halten.
- Timeouts müssen Teil öffentlicher Dienstschnittstellen sein.
- Methoden mit Nebenwirkungen benötigen eindeutige Wiederholungsregeln.
- Generierte Stubs dürfen Sicherheitsprüfungen des Servers nicht ersetzen.
- Antworten müssen immer die zugehörige `call_id` enthalten.
- Verspätete oder unbekannte Antworten müssen sicher verworfen werden.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- synchrone und asynchrone Aufrufe
- One-Way-Aufrufe
- Interface- und Versionsprüfung
- unbekannte Methoden
- Capability-Übergabe
- Shared-Memory-Nutzlasten
- Timeouts und Abbruch
- verspätete Antworten
- Server- und Clientabstürze
- ungültige Längen und Capability-Indizes
- Ressourcenlimits
- 32-Bit-/64-Bit-Kompatibilität
- parallele Aufrufe
- verschachtelte Aufrufketten
- Fuzzing der RPC-Dekodierung

---

## 27. Verbindliche Invarianten

1. NovaOS-RPC verwendet Message Passing als Transport.
2. Jeder RPC-Endpunkt ist durch eine Capability geschützt.
3. Interface- und Methoden-IDs bleiben innerhalb einer Major-Version stabil.
4. Rohzeiger und native C-Strukturen werden nicht direkt übertragen.
5. Capability-Anhänge werden durch den Kernel autorisiert und atomar übertragen.
6. Große Datenmengen werden bevorzugt über Shared Memory bereitgestellt.
7. Ein Timeout garantiert keinen Abbruch der Serveroperation.
8. Alle eingehenden RPC-Daten gelten als nicht vertrauenswürdig.
9. RPC-Aufrufketten und Ressourcen sind begrenzt.
10. RPC bleibt als Prozessgrenze im API- und Fehlermodell sichtbar.

---

## 28. Zusammenfassung

NovaOS implementiert ein lokales, capability-basiertes RPC-System über Message Channels.

Versionierte Interfaces, stabile Methoden-IDs, schemaorientierte Serialisierung und standardisierte Fehler ermöglichen langfristig kompatible Kommunikation zwischen Anwendungen und Systemdiensten.

Shared Memory übernimmt große Datenmengen, während Capabilities Identität und Zugriffsrechte kontrollieren. RPC vereinfacht die Dienstkommunikation, ohne Prozessgrenzen und mögliche Fehler zu verbergen.