# ADR-NET-0008 – Quality of Service

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0008 |
| Titel | Quality of Service |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkverkehr und Ressourcensteuerung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-NET-0005, ADR-NET-0006, ADR-NET-0007 |
| Nachfolgende Spezifikation | NPSPEC-NET-QOS |

---

## 1. Kontext

NovaOS muss Netzwerkressourcen zwischen unterschiedlichen Anwendungen und Diensten kontrolliert verteilen.

Ohne Quality of Service können große Dateiübertragungen oder Hintergrundprozesse zeitkritischen Verkehr beeinträchtigen, beispielsweise:

- Audio- und Videoanrufe
- interaktive Remote-Sitzungen
- Online-Spiele
- Sprachsteuerung
- Systemupdates
- Cloud- und Netzwerkbackups
- Streaming
- industrielle Steuerdaten

QoS soll Bandbreite, Latenz und Fairness innerhalb des lokalen Systems beeinflussen.

---

## 2. Problemstellung

Netzwerkbandbreite ist begrenzt und kann sich dynamisch verändern. Anwendungen dürfen sich nicht selbst ungeprüft die höchste Priorität zuweisen.

NovaOS muss daher:

- Verkehr klassifizieren,
- priorisierte Warteschlangen verwalten,
- Bandbreite begrenzen und reservieren,
- Anwendungen und Dienste fair behandeln,
- Bufferbloat reduzieren,
- IPv4 und IPv6 gleichwertig berücksichtigen,
- Überlastung kontrolliert behandeln,
- falsche Leistungsgarantien vermeiden.

QoS kann nur Ressourcen kontrollieren, die NovaOS selbst verwaltet. Das Betriebssystem kann keine Ende-zu-Ende-Garantie für das Internet geben.

---

## 3. Entscheidung

NovaOS implementiert ein hierarchisches QoS-Framework im Netzwerk-Stack.

Das Framework besteht aus:

- Verkehrsklassifizierung,
- Traffic Classes,
- per-Interface Warteschlangen,
- Bandbreitenbegrenzung,
- fairer Flussverteilung,
- Active Queue Management,
- optionaler Reservierung,
- lokalen Statistiken und Diagnose.

Die Verwaltung erfolgt über capability-geschützte QoS-Profile. Der schnelle Schedulingpfad läuft innerhalb des Netzwerk-Stacks.

---

## 4. Architektur

```text
Socket oder Paket
       |
       v
Klassifizierung
       |
       v
Traffic Class
       |
       v
Flow Queue
       |
       v
Interface Scheduler
       |
       v
Netzwerktreiber
```

Die Firewall entscheidet, ob Verkehr erlaubt ist. QoS entscheidet anschließend, wann und mit welchem Anteil erlaubter Verkehr verarbeitet wird.

---

## 5. Traffic Classes

NovaOS definiert folgende logische Klassen:

| Klasse | Zweck |
|---|---|
| `CONTROL` | kritischer Netzwerk- und Systemkontrollverkehr |
| `REALTIME` | stark latenzempfindlicher Verkehr |
| `INTERACTIVE` | interaktive Anwendungen |
| `STREAMING` | kontinuierliche Medienübertragung |
| `NORMAL` | gewöhnlicher Anwendungsverkehr |
| `BACKGROUND` | Updates, Synchronisation und Downloads |
| `BULK` | große, nicht zeitkritische Übertragungen |
| `SCAVENGER` | nur bei verfügbaren Restressourcen |

`NORMAL` ist die Standardklasse.

Eine hohe Klasse bedeutet nicht, dass beliebig viel Bandbreite verbraucht werden darf.

---

## 6. Klassifizierung

Verkehr kann klassifiziert werden anhand von:

- kernelbestätigter Anwendungsidentität
- Systemdienst
- Benutzer und Session
- Job Object
- Socketoption
- Transportprotokoll
- Quell- und Zielport
- Netzwerkprofil
- Firewallmarkierung
- Interface
- DSCP-Wert
- administrativer Regel

Selbst angegebene Portnummern oder DSCP-Werte einer Anwendung gelten nicht automatisch als vertrauenswürdig.

---

## 7. Anwendungsanforderungen

Eine Anwendung kann eine bevorzugte Traffic Class anfordern:

```c
status_t net_socket_set_qos(
    cap_handle_t socket_handle,
    uint32_t traffic_class,
    const net_qos_request_t *request
);
```

Der QoS Manager prüft:

- Anwendungsidentität
- vorhandene QoS-Capabilities
- Benutzer- und Systemrichtlinien
- Netzwerkprofil
- verfügbare Ressourcen
- angeforderte Klasse und Grenzwerte

Die tatsächlich gewährte Klasse kann niedriger als die angeforderte Klasse sein.

---

## 8. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `QOS_RIGHT_REQUEST` | eine erlaubte Klasse anfordern |
| `QOS_RIGHT_REALTIME` | Echtzeitklasse verwenden |
| `QOS_RIGHT_RESERVE` | Bandbreite reservieren |
| `QOS_RIGHT_LIMIT` | eigene oder delegierte Limits setzen |
| `QOS_RIGHT_CLASSIFY` | Klassifizierungsregeln verwalten |
| `QOS_RIGHT_INSPECT` | QoS-Status und Statistiken lesen |
| `QOS_RIGHT_ADMIN` | globale QoS-Profile verwalten |

Gewöhnliche Anwendungen erhalten keine automatische Berechtigung für `REALTIME`, `CONTROL` oder garantierte Reservierungen.

---

## 9. Hierarchisches Scheduling

QoS wird hierarchisch organisiert:

```text
System
└── Netzwerk-Namespace
    └── Benutzer oder Job
        └── Anwendung
            └── Traffic Class
                └── Flow
```

Jede Ebene kann besitzen:

- Mindestanteil
- Maximalrate
- Priorität
- Burst-Limit
- Gewichtung
- Queue-Limit

Ein untergeordnetes Objekt kann die Grenzen seiner übergeordneten Ebene nicht überschreiten.

---

## 10. Scheduler

Der Standard-Interface-Scheduler kombiniert:

- priorisierte Klassen,
- Weighted Fair Queuing,
- flussbasierte Unterwarteschlangen,
- Active Queue Management.

Strikte Priorität wird nur für sehr begrenzten Kontrollverkehr verwendet.

Alle anderen Klassen erhalten gewichtete Anteile, damit niedrige Klassen nicht dauerhaft verhungern.

Der konkrete Scheduler bleibt austauschbar, sofern die öffentlichen QoS-Garantien erhalten bleiben.

---

## 11. Flow Queues

Verkehr wird innerhalb einer Klasse nach Flows getrennt.

Ein Flow wird typischerweise bestimmt durch:

```text
Adressfamilie
Quelladresse
Zieladresse
Protokoll
Quellport
Zielport
Netzwerk-Namespace
```

Die Trennung verhindert, dass eine einzelne große Verbindung alle anderen Verbindungen derselben Klasse verdrängt.

Anzahl und Lebenszeit von Flow Queues sind begrenzt.

---

## 12. Bandbreitenbegrenzung

NovaOS verwendet Token-Bucket- oder vergleichbare Verfahren für:

- Maximalrate
- Bursts
- anwendungsbezogene Limits
- Hintergrundverkehr
- Netzwerkprofile
- geteilte Interfaces

Ein Limit kann definiert werden in:

- Bit pro Sekunde
- Paketen pro Sekunde
- maximaler Burst-Größe
- maximaler Queue-Länge

Eine Begrenzung wird nicht durch das Öffnen mehrerer Sockets umgangen, wenn sie auf Anwendungs- oder Jobebene gilt.

---

## 13. Bandbreitenreservierung

Zeitkritische Dienste können eine Mindestbandbreite anfordern.

Eine Reservierung wird nur gewährt, wenn:

- das Interface eine ausreichende bekannte Kapazität besitzt,
- höhere Systemrichtlinien dies erlauben,
- vorhandene Reservierungen nicht verletzt werden,
- die Anwendung eine passende Capability besitzt,
- eine Ablaufzeit oder Lebenszeit definiert ist.

Reservierungen gelten nur innerhalb des NovaOS-Ausgangsschedulers. Sie garantieren keine Bandbreite außerhalb des lokalen Geräts.

---

## 14. Admission Control

Vor einer neuen Reservierung prüft der QoS Manager die verfügbaren Ressourcen.

Mögliche Ergebnisse:

- vollständig gewährt
- mit reduzierter Rate gewährt
- nur als Prioritätsklasse gewährt
- abgelehnt

Eine einmal gewährte Reservierung kann bei Interfacewechsel oder verringerter Linkkapazität herabgestuft werden.

Die betroffene Anwendung muss darüber ein Ereignis erhalten.

---

## 15. Active Queue Management

NovaOS verwendet Active Queue Management, um dauerhaft volle Warteschlangen und hohe Latenzen zu vermeiden.

Der Algorithmus darf:

- Pakete frühzeitig verwerfen,
- ECN markieren,
- Flows mit übermäßiger Queuebelegung begrenzen,
- Queue-Latenz statt nur Byteanzahl berücksichtigen.

ECN wird verwendet, wenn Transportprotokoll und Gegenstelle es unterstützen.

Die konkrete AQM-Implementierung kann beispielsweise CoDel- oder PIE-ähnliche Verfahren verwenden.

---

## 16. DSCP und Traffic Class

IPv4 DSCP und IPv6 Traffic Class können als Klassifizierungshinweis verwendet werden.

Dabei gilt:

- Anwendungen dürfen Markierungen nicht ungeprüft setzen.
- ausgehende Werte können durch NovaOS neu geschrieben werden.
- eingehende Werte gelten als nicht vertrauenswürdig.
- Netzwerkprofile können DSCP vollständig ignorieren.
- Markierungen dürfen keine Capability-Prüfung ersetzen.

NovaOS muss auch ohne DSCP-Unterstützung des Netzwerks korrekt funktionieren.

---

## 17. Eingehender Verkehr

Ausgehender Verkehr kann direkt durch den Interface-Scheduler geordnet werden. Eingehender Verkehr hat das lokale Interface bereits erreicht.

Für eingehenden Verkehr kann NovaOS:

- Socketpuffer begrenzen,
- Paketverarbeitung priorisieren,
- Flows rate-limitieren,
- Pakete früh verwerfen,
- CPU-Verarbeitung fair verteilen,
- Protokoll-Backpressure auslösen.

Ingress-QoS kann keine bereits verbrauchte externe Linkbandbreite zurückgewinnen.

---

## 18. CPU- und Interrupt-Affinität

QoS kann mit CPU- und Interruptsteuerung kooperieren.

Berücksichtigt werden:

- Receive-Side-Scaling-Queues
- Interrupt-Affinität
- Netzwerk-Worker
- NUMA-Lokalität
- Echtzeit- und Systemlast
- Energieprofile

Eine hohe Netzwerkklasse gewährt keine allgemeine hohe Schedulerpriorität für den gesamten Prozess.

Nur die notwendige Netzwerkverarbeitung wird entsprechend priorisiert.

---

## 19. Interfacewechsel

Bei einem Wechsel zwischen Ethernet, WLAN, Mobilfunk oder VPN können sich Bandbreite und Latenz stark verändern.

Der QoS Manager:

1. aktualisiert die geschätzte Linkkapazität,
2. berechnet Klassenanteile neu,
3. prüft Reservierungen,
4. informiert betroffene Anwendungen,
5. passt Queue-Limits und AQM an,
6. übernimmt keine ungültigen Interfaceannahmen.

Bestehende Verbindungen dürfen nur weiterlaufen, wenn Routing und Transport dies unterstützen.

---

## 20. Netzwerkprofile

QoS-Richtlinien können vom Netzwerkprofil abhängen:

| Profil | Beispielrichtlinie |
|---|---|
| Public | restriktive Hintergrundübertragung |
| Private | normale lokale Bandbreitennutzung |
| Metered | strenge Limits für Updates und Backups |
| VPN | priorisierte Tunnelkontrolle |
| Low Bandwidth | geringe Queuegrößen und reduzierte Hintergrundlast |
| Offline | kein externer Verkehr |

Die Klassifizierung als getaktetes oder begrenztes Netzwerk muss für Anwendungen sichtbar sein.

---

## 21. Überlastungsverhalten

Bei Überlastung gilt grundsätzlich folgende Reihenfolge:

1. Scavenger-Verkehr begrenzen oder verwerfen,
2. Bulk-Verkehr reduzieren,
3. Background-Verkehr drosseln,
4. Normal- und Streaming-Verkehr fair behandeln,
5. Interactive und Real-time bevorzugen,
6. Control-Verkehr innerhalb enger Limits erhalten.

Auch hohe Klassen unterliegen Queue- und Rate-Limits.

Ein fehlerhafter Echtzeitdienst darf das Interface nicht vollständig blockieren.

---

## 22. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Traffic Classes pro Namespace
- Flow Queues
- Reservierungen
- Klassifizierungsregeln
- Queue-Speicher
- Burst-Größen
- Statistikobjekte
- ausstehende QoS-Anforderungen
- Rate-Limit-Zustände

QoS-Metadaten dürfen nicht mehr Speicher verbrauchen als die eigentlichen Netzwerkpuffer rechtfertigen.

---

## 23. Sicherheit

Für QoS gelten folgende Regeln:

- Anwendungen können hohe Prioritäten nur anfordern, nicht selbst gewähren.
- Klassen und Reservierungen benötigen Capabilities.
- eingehende DSCP-Werte gelten als nicht vertrauenswürdig.
- mehrere Sockets dürfen Anwendungs- oder Joblimits nicht umgehen.
- Systemkontrollverkehr besitzt feste Obergrenzen.
- QoS verändert keine Firewallentscheidung von `DROP` zu `ALLOW`.
- untergeordnete Namespaces dürfen globale Limits nicht überschreiten.
- Queue- und Flowzustände sind immer begrenzt.

---

## 24. Datenschutz und Diagnose

Lokale QoS-Diagnose darf enthalten:

- gewährte Traffic Class
- Bandbreitenlimit
- reservierte Rate
- Queue-Länge und -Latenz
- Paketverluste
- ECN-Markierungen
- Interfacekapazität
- Herabstufungen
- Anwendungs- oder Jobzuordnung

Paketinhalte und vollständige Zielverläufe werden nicht standardmäßig erfasst.

Statistiken werden auf die notwendige technische Granularität begrenzt.

---

## 25. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | QoS-Anforderung wurde gewährt |
| `STATUS_QOS_DEGRADED` | Anforderung wurde nur teilweise gewährt |
| `STATUS_QOS_UNAVAILABLE` | gewünschte QoS-Leistung ist nicht verfügbar |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_INVALID_CLASS` | Traffic Class ist ungültig |
| `STATUS_RESERVATION_CONFLICT` | Bandbreite ist bereits reserviert |
| `STATUS_INTERFACE_DOWN` | Interface ist nicht aktiv |
| `STATUS_POLICY_DENIED` | Netzwerkprofil verbietet die Anforderung |
| `STATUS_LIMIT_EXCEEDED` | QoS-Ressourcenlimit wurde überschritten |
| `STATUS_NOT_SUPPORTED` | Interface oder Backend unterstützt die Funktion nicht |

---

## 26. Audit

Auditiert werden insbesondere:

- Vergabe privilegierter Echtzeitklassen
- Bandbreitenreservierungen
- administrative QoS-Regeländerungen
- wiederholte Umgehungsversuche
- Überschreitungen geschützter Limits
- unerwartete Herabstufung kritischer Systemdienste

Gewöhnliche Klassifizierungsentscheidungen werden nicht einzeln auditiert.

Dadurch wird eine unkontrollierte Ereignismenge vermieden.

---

## 27. Alternative Ansätze

### 27.1 Strikte Prioritätswarteschlangen

**Vorteile:**

- einfach und geringe Latenz für hohe Klassen

**Nachteile:**

- niedrige Klassen können vollständig verhungern
- leicht missbrauchbar

**Entscheidung:** Nur für eng begrenzten Kontrollverkehr. Andere Klassen verwenden gewichtete Fairness.

### 27.2 QoS ausschließlich über DSCP

**Vorteile:**

- bestehender Netzwerkstandard

**Nachteile:**

- Markierungen können manipuliert oder im Netzwerk ignoriert werden
- keine anwendungsbezogene lokale Kontrolle

**Entscheidung:** DSCP ist nur ein Hinweis innerhalb der NovaOS-Richtlinien.

### 27.3 Keine QoS, nur FIFO

**Vorteile:**

- sehr einfache Implementierung

**Nachteile:**

- hohe Latenz unter Last
- Hintergrundverkehr stört interaktive Anwendungen

**Entscheidung:** Abgelehnt.

---

## 28. Konsequenzen

### Positive Konsequenzen

- geringere Latenz für interaktive Anwendungen
- faire Verteilung zwischen Flows
- kontrollierter Hintergrundverkehr
- Unterstützung für Bandbreitenlimits und Reservierungen
- Reduzierung von Bufferbloat
- anwendungs- und namespacebezogene Richtlinien
- Anpassung an verschiedene Netzwerkprofile

### Negative Konsequenzen

- komplexere Warteschlangen- und Klassifizierungslogik
- Linkkapazität kann nur geschätzt werden
- Reservierungen können bei Interfacewechseln nicht gehalten werden
- zusätzliche Zustände und Statistiken benötigen Speicher

### Risiken

- falsche Klassifizierung
- Aushungern niedriger Klassen
- Missbrauch privilegierter QoS-Capabilities
- instabile Kapazitätsschätzung
- zu große Warteschlangen
- irreführende Interpretation lokaler Reservierungen als Internetgarantie

---

## 29. Implementierungsrichtlinien

- `NORMAL` ist die Standardklasse.
- Anwendungen dürfen Prioritäten nur anfordern.
- Die endgültige Klassifizierung erfolgt durch NovaOS-Richtlinien.
- Strikte Priorität bleibt auf begrenzten Kontrollverkehr beschränkt.
- Gewöhnliche Klassen verwenden flussbasierte gewichtete Fairness.
- Jede Queue besitzt ein festes Speicher- und Latenzlimit.
- AQM wird zur Reduzierung von Bufferbloat verwendet.
- DSCP wird als Hinweis und nicht als Berechtigung behandelt.
- Reservierungen gelten nur für lokal kontrollierte Ressourcen.
- QoS darf Firewall- oder Capability-Entscheidungen nicht abschwächen.

---

## 30. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- alle Traffic Classes
- faire Verteilung zwischen Anwendungen und Flows
- Bandbreitenlimits
- Burst-Verhalten
- Reservierungen und Admission Control
- Echtzeit- und Hintergrundverkehr
- AQM und ECN
- IPv4 DSCP und IPv6 Traffic Class
- Interfacewechsel
- Netzwerkprofile
- Namespace- und Joblimits
- mehrere Sockets derselben Anwendung
- Queue-Überlastung
- Mehrkern- und NUMA-Verarbeitung
- Herabstufung laufender Reservierungen

---

## 31. Verbindliche Invarianten

1. `NORMAL` ist die Standardklasse für gewöhnlichen Netzwerkverkehr.
2. Anwendungen können QoS anfordern, aber nicht selbst uneingeschränkt gewähren.
3. Hohe Traffic Classes bleiben capability- und richtliniengeschützt.
4. Strikte Priorität wird nur für begrenzten Kontrollverkehr verwendet.
5. Niedrige Klassen dürfen nicht dauerhaft verhungern.
6. QoS-Queues und Flowzustände sind immer begrenzt.
7. DSCP- und Traffic-Class-Werte gelten nicht als Berechtigung.
8. QoS kann blockierten Verkehr nicht erlauben.
9. Bandbreitenreservierungen gelten nur für lokal kontrollierte Netzwerkressourcen.
10. Paketnutzdaten werden nicht standardmäßig durch QoS-Diagnose erfasst.

---

## 32. Zusammenfassung

NovaOS implementiert ein hierarchisches QoS-Framework mit Traffic Classes, flussbasierter Fairness, Bandbreitenlimits, optionalen Reservierungen und Active Queue Management.

Anwendungen dürfen gewünschte Klassen anfordern, während NovaOS anhand von Capabilities, Netzwerkprofilen und Systemrichtlinien über die tatsächliche Einstufung entscheidet. Auch hoch priorisierter Verkehr bleibt begrenzt.

QoS verbessert Latenz und Fairness innerhalb des lokalen Systems, stellt aber ausdrücklich keine Ende-zu-Ende-Garantie für externe Netzwerke oder das Internet dar.