# ADR-NET-0007 – Packet Filter

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0007 |
| Titel | Packet Filter |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerk-Datenpfad und Paketverarbeitung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0003, ADR-NET-0004, ADR-NET-0005, ADR-NET-0006, ADR-KERNEL-0105 |
| Nachfolgende Spezifikation | NPSPEC-NET-PACKET-FILTER |

---

## 1. Kontext

Die NovaOS-Firewall benötigt eine sichere und schnelle Ausführungsumgebung für Paketfilterregeln.

Der Packet Filter ist dabei nicht mit der Firewall gleichzusetzen:

| Komponente | Aufgabe |
|---|---|
| Firewall | Sicherheitsrichtlinien, Profile und Verbindungsregeln |
| Packet Filter | technische Ausführung geprüfter Regeln im Paketpfad |
| Connection Tracker | Zustand von Verbindungen und Flows |
| Routing Engine | Bestimmung des Paketpfads |

Der Packet Filter soll zusätzlich für kontrollierte Netzwerkfunktionen wie Markierung, Statistik und Diagnose verwendet werden können.

---

## 2. Problemstellung

Paketfilter laufen in einem besonders häufig ausgeführten und sicherheitskritischen Datenpfad.

Eine fehlerhafte Filterimplementierung könnte:

- Kernel-Speicher beschädigen,
- Paketgrenzen überschreiten,
- Endlosschleifen verursachen,
- den Netzwerkpfad blockieren,
- Sicherheitsregeln umgehen,
- übermäßige CPU-Zeit verbrauchen,
- vertrauliche Paketdaten offenlegen.

NovaOS benötigt deshalb eine begrenzte und verifizierbare Filterausführungsumgebung.

---

## 3. Entscheidung

NovaOS implementiert eine eigene, typisierte Packet-Filter-VM mit verifizierten Programmen.

Firewallregeln und andere zugelassene Filterdefinitionen werden in eine interne Zwischendarstellung kompiliert. Vor der Aktivierung prüft ein Kernel-Verifier das vollständige Programm.

Nur erfolgreich verifizierte Programme dürfen im Paketpfad ausgeführt werden.

Ein optionaler JIT-Compiler kann später ergänzt werden. Die verifizierte VM-Ausführung bleibt immer als sicherer Fallback erhalten.

---

## 4. Architektur

```text
Regeldefinition
      |
      v
Userspace-Compiler
      |
      v
Filter-Zwischencode
      |
      v
Kernel-Verifier
      |
      v
VM oder geprüfter JIT-Code
      |
      v
Paketfilterpunkt
```

Der Userspace-Compiler gilt nicht als Sicherheitsgrenze. Der Kernel validiert jedes Filterprogramm unabhängig erneut.

---

## 5. Filterpunkte

Programme können an definierte Hooks gebunden werden:

| Hook | Zeitpunkt |
|---|---|
| `LINK_INGRESS` | nach Empfang vom Netzwerktreiber |
| `NETWORK_INGRESS` | nach grundlegender IP-Validierung |
| `LOCAL_INPUT` | vor lokaler Zustellung |
| `FORWARD` | vor Paketweiterleitung |
| `LOCAL_OUTPUT` | nach lokaler Paketerzeugung |
| `NETWORK_EGRESS` | vor Übergabe an Link Layer |
| `SOCKET_LOOKUP` | vor Socketzuordnung |
| `SOCKET_CONNECT` | vor ausgehendem Verbindungsaufbau |

Nicht jede Hilfsfunktion steht an jedem Hook zur Verfügung.

Frühe Hooks besitzen weniger Metadaten, können unerwünschte Pakete aber früher verwerfen.

---

## 6. Paketansicht

Ein Filterprogramm erhält keine rohen Kernelzeiger.

Stattdessen verwendet es einen begrenzten Paketkontext:

```c
typedef struct packet_filter_context {
    uint32_t hook;
    uint32_t packet_length;
    uint32_t address_family;
    uint32_t protocol;
    uint64_t ingress_interface;
    uint64_t egress_interface;
    uint64_t network_namespace;
    uint64_t connection_state;
} packet_filter_context_t;
```

Paketdaten werden nur über verifizierte Ladeoperationen oder freigegebene Helper gelesen.

---

## 7. Filteraktionen

Ein Programm liefert eine definierte Aktion zurück:

| Aktion | Bedeutung |
|---|---|
| `PASS` | Verarbeitung fortsetzen |
| `DROP` | Paket lautlos verwerfen |
| `REJECT` | Paket verwerfen und Fehlerantwort anfordern |
| `REDIRECT` | Paket an ein erlaubtes Ziel umleiten |
| `MARK` | internes Paketmerkmal setzen |
| `QUEUE` | Paket an einen autorisierten Dienst übergeben |
| `MIRROR` | kontrollierte Kopie für Diagnose erzeugen |
| `RATE_LIMIT` | Rate-Limit-Entscheidung anwenden |

Privilegierte Aktionen wie `REDIRECT`, `QUEUE` und `MIRROR` benötigen zusätzliche Programm- und Ladeberechtigungen.

---

## 8. Instruktionsmodell

Die Filter-VM verwendet eine kleine, typisierte Instruktionsmenge für:

- Ganzzahlarithmetik
- Bitoperationen
- Vergleiche
- bedingte Sprünge
- begrenzten Stackzugriff
- Lesen validierter Paketfelder
- Zugriff auf Maps
- Aufruf freigegebener Helper
- Rückgabe einer Filteraktion

Nicht unterstützt werden:

- beliebige Kernelzeiger
- dynamische Codeerzeugung
- Rekursion
- unbeschränkte Schleifen
- direkte Systemaufrufe
- frei adressierbarer Kernelspeicher
- Gleitkommaoperationen

---

## 9. Verifier

Der Verifier prüft vor der Aktivierung:

1. gültige Programmversion,
2. bekannte Instruktionen,
3. erreichbare Kontrollflüsse,
4. garantierte Terminierung,
5. initialisierte Register,
6. Typen aller Werte,
7. Stackgrenzen,
8. Paketgrenzen,
9. Helper-Berechtigungen,
10. maximale Ausführungskosten,
11. gültige Rückgabeaktion,
12. Hook-Kompatibilität.

Kann die Sicherheit nicht bewiesen werden, wird das Programm vollständig abgelehnt.

---

## 10. Schleifen und Terminierung

Schleifen sind nur erlaubt, wenn der Verifier eine feste obere Grenze beweisen kann.

Beispiele:

- Iteration über maximal 16 Headeroptionen
- Verarbeitung einer statisch begrenzten Segmentliste
- feste Anzahl von Tabellenzugriffen

Unbegrenzte, datenabhängige Schleifen sind verboten.

Zusätzlich besitzt jedes Programm ein festes Instruktionsbudget.

---

## 11. Paketgrenzen

Jeder Zugriff auf Paketdaten benötigt einen beweisbar gültigen Offset und eine gültige Länge.

Der Verifier beziehungsweise ein Helper prüft:

```text
offset + length <= packet_length
```

Alle Berechnungen werden gegen Integerüberläufe geschützt.

Variable IPv4-Header, IPv6 Extension Header und Transportheader dürfen nur über validierte Parserzustände verwendet werden.

---

## 12. Helper-Funktionen

Programme dürfen nur registrierte Helper aufrufen.

Mögliche Helper sind:

- Paketfeld lesen
- sichere Prüfsumme aktualisieren
- Connection State abfragen
- Routingmarkierung setzen
- Interfaceinformationen lesen
- Map-Eintrag lesen oder ändern
- Rate Limit prüfen
- begrenzten Statistikzähler erhöhen
- Diagnoseereignis erzeugen

Jeder Helper definiert:

- erlaubte Hooks
- erforderliche Rechte
- Argumenttypen
- Seiteneffekte
- maximale Kosten
- mögliche Fehler

---

## 13. Maps

Filterprogramme können begrenzte Maps für Zustände und Konfiguration verwenden.

Unterstützte Typen können sein:

- Array
- Hash Map
- Präfixbaum
- Per-CPU-Zähler
- LRU-Flow-Map
- Rate-Limit-Map

Maps sind eigene capability-geschützte Kernelobjekte.

Für jede Map gelten feste Limits für Schlüsselgröße, Wertgröße, Eintragsanzahl und Speicherverbrauch.

---

## 14. Unveränderliche Programme

Ein geladenes Filterprogramm ist unveränderlich.

Änderungen erfolgen durch:

1. neues Programm erzeugen,
2. verifizieren,
3. Maps und Bindungen vorbereiten,
4. neue Generation atomar aktivieren,
5. alte Generation nach Ende aktiver Leser freigeben.

Programmcode darf nach erfolgreicher Prüfung nicht mehr verändert werden.

Veränderbare Konfiguration wird ausschließlich über autorisierte Maps bereitgestellt.

---

## 15. Programmbindung

```c
status_t packet_filter_attach(
    cap_handle_t filter_program,
    cap_handle_t network_namespace,
    uint32_t hook,
    uint32_t priority,
    cap_handle_t *binding_handle
);
```

Die Bindung enthält:

- Programm
- Hook
- Priorität
- Netzwerk-Namespace
- Interfacebeschränkung
- Sicherheitsrichtlinie
- Generation

Ein Programm kann nur an Hooks gebunden werden, für die es verifiziert wurde.

---

## 16. Reihenfolge mehrerer Programme

Mehrere Programme an einem Hook werden deterministisch ausgeführt.

Die Reihenfolge ergibt sich aus:

1. geschützter Systemebene,
2. Namespace-Ebene,
3. Firewall-Ebene,
4. administrativer Erweiterungsebene,
5. Priorität innerhalb der Ebene.

Untergeordnete Programme dürfen ein `DROP` oder `REJECT` einer höheren Schutzebene nicht in `PASS` umwandeln.

Die effektive Entscheidung kann daher nur gleich bleiben oder restriktiver werden.

---

## 17. Paketänderungen

Der Basis-Filter darf Paketdaten standardmäßig nur lesen.

Schreibende Änderungen benötigen:

- einen dafür verifizierten Programmtyp,
- einen geeigneten Hook,
- besondere Capability-Rechte,
- erneute Validierung betroffener Header,
- Aktualisierung notwendiger Prüfsummen.

Direkte Größenänderungen eines Packets sind in der ersten Implementierung nicht erlaubt.

NAT und komplexe Pakettransformationen werden über gesonderte, kontrollierte Module umgesetzt.

---

## 18. Userspace Queue

Die Aktion `QUEUE` darf ausgewählte Pakete an einen autorisierten Userspace-Dienst weiterleiten.

Dabei gelten:

- begrenzte Warteschlangen
- feste Paketgrößen
- Timeout
- definierte Standardaktion bei Dienstausfall
- keine unbegrenzte Blockierung des Paketpfads
- capability-geschützte Rückgabeentscheidung

Für sicherheitskritische Filter ist die Standardaktion bei Überlastung `DROP`.

Userspace Queue ist nicht für den gewöhnlichen Hochgeschwindigkeitsdatenpfad vorgesehen.

---

## 19. JIT-Kompilierung

Ein optionaler JIT-Compiler darf verifizierten Zwischencode in nativen CPU-Code übersetzen.

Voraussetzungen:

- Programm wurde zuvor vollständig verifiziert,
- JIT-Ausgabe wird in schreibbaren Speicher erzeugt,
- danach wird der Speicher schreibgeschützt und ausführbar gemacht,
- gleichzeitiges W+X ist verboten,
- generierter Code wird gegen Programm und Plattform gebunden,
- VM-Interpreter bleibt verfügbar.

JIT-Fehler führen zum sicheren VM-Fallback und nicht zur ungeprüften Programmausführung.

---

## 20. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `FILTER_RIGHT_LOAD` | Filterprogramm zur Prüfung laden |
| `FILTER_RIGHT_ATTACH` | Programm an erlaubten Hook binden |
| `FILTER_RIGHT_DETACH` | Bindung entfernen |
| `FILTER_RIGHT_MAP_READ` | Map lesen |
| `FILTER_RIGHT_MAP_WRITE` | Map verändern |
| `FILTER_RIGHT_REDIRECT` | Redirect-Aktionen verwenden |
| `FILTER_RIGHT_CAPTURE` | Queue oder Mirror verwenden |
| `FILTER_RIGHT_INSPECT` | Programm und Statistiken untersuchen |
| `FILTER_RIGHT_ADMIN` | geschützte Filterebenen verwalten |

Gewöhnliche Anwendungen dürfen keine systemweiten Paketfilterprogramme laden.

---

## 21. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Programmgröße
- Instruktionsanzahl
- Stackgröße
- Anzahl der Helper-Aufrufe
- Schleifeniterationen
- Maps pro Programm
- Einträge pro Map
- Programme pro Hook
- Diagnoseereignisse
- Userspace-Queue-Größe
- JIT-Code-Speicher

Limits gelten pro Benutzer, Sicherheitsdomäne, Netzwerk-Namespace und systemweit.

---

## 22. Fehlerbehandlung

Wird während der Filterausführung ein unerwarteter interner Fehler erkannt, gilt standardmäßig:

- sicherheitskritischer Eingangsfilter: `DROP`
- Diagnosefilter: Filter überspringen und Fehler melden
- geschützter Systemfilter: `DROP` und Audit
- ausgehender Anwendungsfilter: Richtlinie des Namespace anwenden

Ein Filterfehler darf keinen Absturz des Netzwerk-Stacks verursachen.

Wiederholt fehlerhafte Programme werden automatisch deaktiviert oder auf den VM-Interpreter zurückgesetzt.

---

## 23. Sicherheit

Für den Packet Filter gelten folgende Regeln:

- nur verifizierte Programme dürfen ausgeführt werden,
- Programme besitzen keinen direkten Kernelzeigerzugriff,
- alle Paketlesevorgänge sind grenzgeprüft,
- Terminierung und maximale Kosten müssen beweisbar sein,
- Helper sind typisiert und hookabhängig,
- Programme sind nach dem Laden unveränderlich,
- JIT-Code ist niemals gleichzeitig schreibbar und ausführbar,
- privilegierte Aktionen benötigen eigene Capabilities,
- untergeordnete Filter dürfen Systemverbote nicht abschwächen.

---

## 24. Audit und Diagnose

Auditiert werden insbesondere:

- Laden und Binden privilegierter Filterprogramme
- Verifier-Ablehnungen mit sicherheitsrelevanter Ursache
- Verwendung von Redirect, Queue oder Mirror
- Änderungen geschützter Filterbindungen
- Überschreitung von Ausführungs- oder Map-Limits
- automatische Deaktivierung fehlerhafter Programme

Diagnoseinformationen können enthalten:

- Programm-ID und Hash
- Verifier-Version
- Hook und Priorität
- Ausführungsanzahl
- Laufzeit
- Rückgabeaktionen
- Map-Speicher
- JIT- oder VM-Modus

Paketnutzdaten werden nicht standardmäßig aufgezeichnet.

---

## 25. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_FILTER_VERIFY_FAILED` | Programm konnte nicht verifiziert werden |
| `STATUS_FILTER_INVALID_INSTRUCTION` | unbekannte oder ungültige Instruktion |
| `STATUS_FILTER_OUT_OF_BOUNDS` | möglicher ungültiger Speicherzugriff |
| `STATUS_FILTER_NOT_TERMINATING` | Terminierung ist nicht beweisbar |
| `STATUS_FILTER_COST_EXCEEDED` | Ausführungskosten überschreiten das Limit |
| `STATUS_FILTER_HELPER_DENIED` | Helper ist nicht erlaubt |
| `STATUS_FILTER_HOOK_MISMATCH` | Programm ist für den Hook ungeeignet |
| `STATUS_ACCESS_DENIED` | erforderliches Capability-Recht fehlt |
| `STATUS_LIMIT_EXCEEDED` | Programm-, Map- oder Bindungslimit überschritten |
| `STATUS_GENERATION_CHANGED` | Bindungsgeneration wurde parallel verändert |

---

## 26. Alternative Ansätze

### 26.1 Nur fest programmierte Firewallregeln

**Vorteile:**

- geringe Ausführungs- und Verifier-Komplexität

**Nachteile:**

- schlecht erweiterbar
- jede neue Match-Funktion benötigt Änderungen am Netzwerk-Stack

**Entscheidung:** Die Standardfirewall nutzt eine verifizierte interne Filterdarstellung.

### 26.2 Native Kernelmodule als Paketfilter

**Vorteile:**

- maximale Leistung und Flexibilität

**Nachteile:**

- vollständige Kernelrechte
- Filterfehler gefährden das gesamte System

**Entscheidung:** Abgelehnt.

### 26.3 Ungeprüfter Userspace-Bytecode

**Vorteile:**

- einfache Erweiterbarkeit

**Nachteile:**

- nicht sicher im Kernel-Datenpfad ausführbar
- Endlosschleifen und Speicherfehler möglich

**Entscheidung:** Abgelehnt. Jedes Programm wird durch den Kernel verifiziert.

---

## 27. Konsequenzen

### Positive Konsequenzen

- sicher erweiterbarer Paketfilter
- schnelle Ausführung im Netzwerk-Datenpfad
- atomarer Programmwechsel
- begrenzte und typisierte Helper
- Unterstützung für Firewall, Statistik und Diagnose
- sicherer VM-Fallback bei JIT-Problemen
- Isolation durch Netzwerk-Namespaces und Capabilities

### Negative Konsequenzen

- Verifier und VM sind komplex
- Programmiersprache bleibt bewusst eingeschränkt
- JIT erhöht die plattformspezifische Angriffsfläche
- Maps benötigen zusätzliche Ressourcenverwaltung

### Risiken

- Verifier-Fehler
- fehlerhafte Helper
- JIT-Abweichungen von der VM-Semantik
- Zustandstabellenüberlastung
- hohe CPU-Last durch zu komplexe Programme
- unbeabsichtigte Datenoffenlegung durch Diagnoseaktionen

---

## 28. Implementierungsrichtlinien

- Der Kernel vertraut dem Userspace-Compiler nicht.
- Programme werden vor jeder Aktivierung vollständig verifiziert.
- Paketdaten sind nur über grenzgeprüfte Zugriffe lesbar.
- Rekursion und unbegrenzte Schleifen sind verboten.
- Filterprogramme sind nach dem Laden unveränderlich.
- Konfiguration erfolgt über capability-geschützte Maps.
- Programmwechsel werden atomar als neue Generation veröffentlicht.
- JIT-Code verwendet strikt Write XOR Execute.
- VM und JIT müssen dieselbe beobachtbare Semantik besitzen.
- Systemfilter können durch untergeordnete Filter nicht abgeschwächt werden.

---

## 29. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- gültige und ungültige Programme
- Kontrollflussanalyse
- nicht initialisierte Register
- Stackgrenzen
- Paketgrenzen
- feste und variable Header
- begrenzte Schleifen
- Helper-Berechtigungen
- Maps und Ressourcenlimits
- atomaren Programmwechsel
- Netzwerk-Namespace-Isolation
- Userspace Queue
- VM-/JIT-Semantikgleichheit
- Filterfehler und Fail-closed-Verhalten
- Fuzzing von Bytecode und Verifier

---

## 30. Verbindliche Invarianten

1. Nur erfolgreich verifizierte Programme dürfen im Paketpfad ausgeführt werden.
2. Filterprogramme erhalten keine direkten Kernelzeiger.
3. Jeder Paketdatenzugriff ist vollständig grenzgeprüft.
4. Terminierung und maximale Ausführungskosten müssen beweisbar sein.
5. Rekursion und unbegrenzte Schleifen sind verboten.
6. Geladene Programme sind unveränderlich.
7. Privilegierte Aktionen benötigen gesonderte Capabilities.
8. JIT-Code ist niemals gleichzeitig schreibbar und ausführbar.
9. Untergeordnete Filter dürfen Entscheidungen geschützter Systemfilter nicht abschwächen.
10. Paketnutzdaten werden nicht standardmäßig protokolliert.

---

## 31. Zusammenfassung

NovaOS implementiert eine verifizierte Packet-Filter-VM als technische Ausführungsgrundlage der Firewall und weiterer kontrollierter Netzwerkfunktionen.

Filterprogramme werden vor dem Laden vollständig auf Speichergrenzen, Typen, Terminierung, Kosten und zulässige Helper geprüft. Sie laufen anschließend in einer sicheren VM oder optional als verifizierter JIT-Code.

Unveränderliche Programme, atomare Generationen, capability-geschützte Maps und eine strikt begrenzte Ausführungsumgebung ermöglichen flexible Paketfilterung, ohne beliebigen Code in den Kernel-Datenpfad einzuführen.