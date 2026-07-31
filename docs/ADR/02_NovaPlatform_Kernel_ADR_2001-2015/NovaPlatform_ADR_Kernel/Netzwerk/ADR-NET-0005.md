# ADR-NET-0005 – Routing

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0005 |
| Titel | Routing |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkarchitektur und Paketweiterleitung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-NET-0003, ADR-NET-0004, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-NET-ROUTING |

---

## 1. Kontext

NovaOS benötigt eine gemeinsame Routinginfrastruktur für IPv4 und IPv6.

Routing bestimmt für ein Paket:

- das Ausgangsinterface,
- den nächsten Hop,
- die zu verwendende Quelladresse,
- die zulässige MTU,
- die Sicherheits- und Filterrichtlinie,
- ob das Paket lokal zugestellt, weitergeleitet oder verworfen wird.

Die Infrastruktur muss mehrere Interfaces, VPNs, virtuelle Netzwerke und getrennte Netzwerk-Namespaces unterstützen.

---

## 2. Problemstellung

Eine einfache globale Routingtabelle reicht für moderne Systeme nicht aus.

NovaOS muss unter anderem behandeln:

- IPv4 und IPv6,
- mehrere Default Routes,
- WLAN, Ethernet und VPN gleichzeitig,
- unterschiedliche Benutzer- und Anwendungsrichtlinien,
- dynamische Interfacewechsel,
- lokale und entfernte Ziele,
- Routingtabellen pro Namespace,
- atomare Routenänderungen,
- Schutz vor manipulierten dynamischen Routen.

---

## 3. Entscheidung

NovaOS implementiert eine protokollunabhängige Routing Engine mit getrennten IPv4- und IPv6-Präfixdaten.

Die Routenauswahl erfolgt grundsätzlich über:

1. Netzwerk-Namespace bestimmen,
2. Policy-Regeln auswerten,
3. passende Routingtabelle auswählen,
4. Longest Prefix Match durchführen,
5. Metrik und Routenstatus berücksichtigen,
6. nächsten Hop und Ausgangsinterface bestimmen,
7. Quelladresse auswählen,
8. Sicherheits- und Paketfilterregeln anwenden.

Routingtabellen werden als unveränderliche Generationen veröffentlicht und atomar ersetzt.

---

## 4. Architektur

```text
Paket oder Socket
       |
       v
Policy Routing
       |
       v
Routingtabelle
       |
       v
Longest Prefix Match
       |
       v
Route + Next Hop + Interface
       |
       v
Paketfilter und Ausgabe
```

Routing trifft keine alleinige Sicherheitsentscheidung. Paketfilter, Socketrechte und Netzwerkprofile bleiben zusätzliche Prüfungen.

---

## 5. Routentypen

NovaOS unterstützt mindestens:

| Routentyp | Bedeutung |
|---|---|
| Local | Zieladresse gehört dem lokalen System |
| Unicast | normaler Pfad zu einem Ziel |
| Connected | Ziel liegt direkt auf einem Interface |
| Gateway | Ziel wird über einen Router erreicht |
| Broadcast | IPv4-Broadcast-Ziel |
| Multicast | Multicast-Routing oder lokale Gruppe |
| Blackhole | Paket lautlos verwerfen |
| Reject | Paket verwerfen und Fehler melden |
| Unreachable | Ziel ausdrücklich nicht erreichbar |
| Prohibit | Route durch Richtlinie verboten |

Die Routentypen `Blackhole`, `Reject`, `Unreachable` und `Prohibit` besitzen unterschiedliche Diagnose- und Fehlersignale.

---

## 6. Routeneintrag

```c
typedef struct net_route_entry {
    uint32_t structure_size;
    uint16_t address_family;
    uint16_t route_type;

    net_prefix_t destination;
    net_address_t next_hop;

    uint64_t interface_id;
    uint64_t table_id;

    uint32_t metric;
    uint32_t flags;
    uint64_t preferred_source_id;
    uint64_t expires_at;
} net_route_entry_t;
```

Ein Routeneintrag enthält keine internen Kernelzeiger.

Alle IDs und Adressen werden vor der Veröffentlichung validiert.

---

## 7. Präfixe

Routen verwenden ausschließlich CIDR-Präfixe.

Gültige Präfixlängen:

| Protokoll | Bereich |
|---|---:|
| IPv4 | 0 bis 32 |
| IPv6 | 0 bis 128 |

Eine Präfixlänge von null beschreibt eine Default Route.

Hostbits außerhalb des Präfixes müssen beim Einfügen null sein oder vom Kernel kanonisch normalisiert werden.

---

## 8. Longest Prefix Match

Innerhalb einer Routingtabelle wird die spezifischste passende Route bevorzugt.

Beispiel:

```text
0.0.0.0/0
10.0.0.0/8
10.20.0.0/16
10.20.30.0/24
```

Für `10.20.30.40` wird `10.20.30.0/24` gewählt, sofern keine Policy-Regel eine andere Tabelle vorgibt.

Die Metrik wird erst zwischen Routen gleicher Präfixlänge und vergleichbarer Richtlinie ausgewertet.

---

## 9. Routingtabellen

Jeder Netzwerk-Namespace kann mehrere Routingtabellen besitzen.

Vorgesehene Tabellen sind:

- Local Table
- Main Table
- Default Table
- VPN-spezifische Tabellen
- anwendungs- oder richtlinienspezifische Tabellen

Tabellen-IDs sind lokale Identifikatoren innerhalb des Netzwerk-Namespace und keine Zugriffsberechtigungen.

Die Local Table wird durch den Netzwerk-Stack aus aktiven Interface-Adressen erzeugt und besonders geschützt.

---

## 10. Policy Routing

Policy-Regeln können vor der eigentlichen Routensuche eine Tabelle auswählen.

Mögliche Kriterien sind:

- Quelladresse
- Zieladresse
- Adressfamilie
- Protokoll
- Quell- und Zielport
- eingehendes Interface
- Anwendung
- Benutzer oder Session
- Sicherheitsdomäne
- Traffic Class
- Firewallmarkierung
- VPN-Profil

Regeln werden deterministisch nach Priorität ausgewertet.

Die maximale Anzahl und Komplexität der Regeln wird begrenzt.

---

## 11. Metriken

Die Metrik bestimmt die Präferenz gleichwertiger Routen.

Kleinere Werte werden bevorzugt.

NovaOS kann Metriken berücksichtigen aus:

- statischer Konfiguration
- Interfacepriorität
- Verbindungstyp
- VPN-Richtlinie
- gemessener Erreichbarkeit
- Energieprofil
- administrativer Vorgabe

Automatische Metrikänderungen dürfen bestehende Verbindungen nicht unnötig zwischen Pfaden verschieben.

---

## 12. Next-Hop-Auflösung

Nach Auswahl einer Route wird der nächste Hop bestimmt.

Mögliche Ergebnisse:

- Ziel liegt direkt am Link,
- IPv4-Gateway,
- IPv6-Router,
- Tunnelendpunkt,
- virtuelles Interface,
- lokale Zustellung.

Die Link-Layer-Adresse wird anschließend über ARP oder IPv6 Neighbor Discovery ermittelt.

Routingtabelle und Neighbor Cache bleiben getrennte Subsysteme.

---

## 13. Quelladressauswahl

Die Routing Engine liefert Informationen für die Auswahl einer geeigneten Quelladresse.

Berücksichtigt werden:

- explizite Socketbindung
- Ausgangsinterface
- Adressscope
- Präfixübereinstimmung
- bevorzugte Quelladresse der Route
- temporäre oder stabile IPv6-Adresse
- Adresszustand und Lebenszeit
- Anwendungsrichtlinie

Eine Route darf keine Quelladresse auswählen, die auf dem Ausgangsinterface nicht gültig oder nicht entsprechend autorisiert ist.

---

## 14. Netzwerk-Namespaces

Jeder Netzwerk-Namespace besitzt eigene:

- Routingtabellen
- Policy-Regeln
- Interfaceansichten
- Adressen
- Neighbor Caches
- Paketfilterkontexte

Routen eines Namespace sind für andere Namespaces nicht automatisch sichtbar.

Ein physisches Interface kann über kontrollierte virtuelle Schnittstellen oder explizite Zuweisungen mit mehreren Namespaces verbunden werden.

---

## 15. Route hinzufügen und entfernen

```c
status_t net_route_add(
    cap_handle_t routing_namespace,
    const net_route_entry_t *route
);

status_t net_route_remove(
    cap_handle_t routing_namespace,
    const net_route_key_t *route_key
);
```

Der Aufrufer benötigt `NET_RIGHT_ROUTE`.

Der Kernel prüft:

- Adressfamilie und Präfix
- Interface
- Next Hop
- Namespacezugehörigkeit
- Routenflags
- Sicherheitsrichtlinie
- Ressourcengrenzen

Einzelne Änderungen werden atomar veröffentlicht.

---

## 16. Transaktionale Änderungen

Mehrere zusammengehörige Änderungen können als Routingtransaktion ausgeführt werden:

```c
status_t net_route_transaction_begin(
    cap_handle_t routing_namespace,
    cap_handle_t *transaction_handle
);

status_t net_route_transaction_commit(
    cap_handle_t transaction_handle
);
```

Damit können beispielsweise VPN-Routen und Policy-Regeln gemeinsam aktiviert werden.

Andere CPUs sehen entweder die alte oder die neue Generation, niemals eine teilweise angewendete Konfiguration.

---

## 17. Dynamische Routen

Routen können erzeugt werden durch:

- statische Administration
- DHCP
- IPv6 Router Advertisements
- VPN-Dienste
- Routingprotokolle
- virtuelle Netzwerkdienste
- direkt verbundene Interfacepräfixe

Jede Route speichert ihre Quelle und optional eine Ablaufzeit.

Dynamisch gelernte Routen dürfen geschützte statische oder sicherheitskritische Routen nicht ohne ausdrückliche Richtlinie ersetzen.

---

## 18. Interfacezustände

Wird ein Interface inaktiv oder entfernt:

- werden zugehörige Routen deaktiviert oder entfernt,
- werden Route-Cache-Einträge invalidiert,
- werden wartende Next-Hop-Auflösungen beendet,
- wird für neue Verbindungen ein alternativer Pfad gesucht,
- erhalten betroffene Sockets geeignete Fehler.

Eine bestehende Verbindung darf nur auf einen neuen Pfad wechseln, wenn das Transportprotokoll und die Sicherheitsrichtlinie dies erlauben.

---

## 19. Multipath Routing

NovaOS kann mehrere gleichwertige Next Hops für ein Ziel unterstützen.

Die Auswahl kann erfolgen anhand von:

- stabiler Fluss-Hashbildung
- Gewichtung
- Interfacezustand
- NUMA- oder Queue-Lokalität
- Richtlinie
- Erreichbarkeit

Pakete desselben TCP-Flusses sollen nicht ohne Notwendigkeit auf wechselnde Pfade verteilt werden.

Multipath-Auswahl muss gegen manipulierbare Hashkollisionen geschützt werden.

---

## 20. Route Cache

Ergebnisse der Routensuche dürfen zwischengespeichert werden.

Ein Cacheeintrag berücksichtigt mindestens:

- Netzwerk-Namespace
- Adressfamilie
- Ziel und gegebenenfalls Quelle
- Policy-Kontext
- Interfacegeneration
- Routinggeneration
- Filter- und Sicherheitsrichtlinienversion

Ändert sich eine relevante Generation, wird der Eintrag ungültig.

Der Cache darf keine aktuelle Policy- oder Capability-Prüfung umgehen.

---

## 21. Forwarding

Routing für lokal erzeugte Pakete und das Weiterleiten fremder Pakete sind getrennte Funktionen.

IP-Forwarding ist standardmäßig deaktiviert und benötigt:

- administrative Capability
- freigegebene Eingangs- und Ausgangsinterfaces
- Paketfilterregeln
- geeignete Routen
- TTL- beziehungsweise Hop-Limit-Verarbeitung
- ICMP-Fehlerbehandlung

Eine vorhandene Route aktiviert nicht automatisch Forwarding.

---

## 22. Schleifenvermeidung

NovaOS reduziert Routingloops durch:

- TTL bei IPv4
- Hop Limit bei IPv6
- Validierung rekursiver Gatewayrouten
- Begrenzung der Next-Hop-Auflösung
- Erkennung zyklischer Tunnel- und virtueller Routen
- atomare Routinggenerationen

Eine Route darf nicht über sich selbst oder einen unauflösbaren rekursiven Next Hop führen.

---

## 23. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `NET_ROUTE_RIGHT_LOOKUP` | Routenauswahl diagnostisch abfragen |
| `NET_ROUTE_RIGHT_ENUMERATE` | sichtbare Routen auflisten |
| `NET_ROUTE_RIGHT_ADD` | Route hinzufügen |
| `NET_ROUTE_RIGHT_REMOVE` | Route entfernen |
| `NET_ROUTE_RIGHT_POLICY` | Policy-Regeln verwalten |
| `NET_ROUTE_RIGHT_FORWARDING` | Paketweiterleitung konfigurieren |
| `NET_ROUTE_RIGHT_ADMIN` | Routing-Namespace verwalten |

Gewöhnliche Anwendungen erhalten normalerweise nur indirekten Routingzugriff über ihre Sockets.

---

## 24. Sicherheit

Für Routing gelten folgende Regeln:

- Routingänderungen benötigen Capabilities.
- Routingtabellen sind pro Netzwerk-Namespace isoliert.
- dynamische Routen gelten als nicht vertrauenswürdig,
- Local Routes werden besonders geschützt,
- Gateway- und Interfacezugehörigkeit werden validiert,
- Policy Routing darf Anwendungsrechte nicht erweitern,
- Route Caches ersetzen keine Sicherheitsprüfung,
- Forwarding ist standardmäßig deaktiviert,
- Routenänderungen werden atomar veröffentlicht.

Eine Route entscheidet über den Pfad, nicht über die grundsätzliche Berechtigung zur Netzwerkkommunikation.

---

## 25. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_ROUTE_NOT_FOUND` | keine passende Route vorhanden |
| `STATUS_ROUTE_EXISTS` | Route existiert bereits |
| `STATUS_ROUTE_INVALID` | Routeneintrag ist ungültig |
| `STATUS_GATEWAY_UNREACHABLE` | Next Hop kann nicht erreicht werden |
| `STATUS_INTERFACE_DOWN` | Ausgangsinterface ist inaktiv |
| `STATUS_POLICY_DENIED` | Policy verhindert die Route |
| `STATUS_ACCESS_DENIED` | erforderliches Routingrecht fehlt |
| `STATUS_ROUTING_LOOP` | zyklische Auflösung erkannt |
| `STATUS_GENERATION_CHANGED` | Routingzustand wurde parallel geändert |
| `STATUS_LIMIT_EXCEEDED` | Routingressourcenlimit überschritten |

---

## 26. Audit und Diagnose

Auditiert werden insbesondere:

- Änderungen der Default Route,
- Aktivierung von Forwarding,
- Änderungen geschützter Policy-Regeln,
- VPN-Routen,
- fehlgeschlagene administrative Routingänderungen,
- erkannte Routingloops,
- unerlaubte Cross-Namespace-Routen.

Diagnosewerkzeuge dürfen anzeigen:

- gewählte Route
- Ausgangsinterface
- Next Hop
- Quelladresse
- Metrik
- Tabelle und Policy-Regel
- Routenquelle
- Ablaufzeit
- Routinggeneration

Paketinhalte werden dabei nicht erfasst.

---

## 27. Alternative Ansätze

### 27.1 Eine globale Routingtabelle

**Vorteile:**

- einfache Implementierung

**Nachteile:**

- keine Isolation für Anwendungen, VPNs oder Container
- unzureichend für Policy Routing

**Entscheidung:** Abgelehnt.

### 27.2 Nur zielbasierte Routensuche

**Vorteile:**

- geringer Laufzeitaufwand

**Nachteile:**

- ungeeignet für mehrere Benutzer, VPNs und Sicherheitsdomänen

**Entscheidung:** Longest Prefix Match bleibt Kern, wird aber durch Policy Routing ergänzt.

### 27.3 Veränderbare Routingtabelle unter globaler Sperre

**Vorteile:**

- einfache Synchronisation

**Nachteile:**

- schlechte Mehrkernskalierung
- Leser können blockiert werden

**Entscheidung:** Routingtabellen werden generationsbasiert und atomar veröffentlicht.

---

## 28. Konsequenzen

### Positive Konsequenzen

- gemeinsame Routinginfrastruktur für IPv4 und IPv6
- Unterstützung mehrerer Netzwerk-Namespaces
- Policy Routing für Anwendungen und VPNs
- atomare Konfigurationsänderungen
- skalierbare, cachefähige Routensuche
- kontrolliertes Multipath Routing
- klare Trennung von Routing und Autorisierung

### Negative Konsequenzen

- Policy Routing erhöht die Komplexität
- mehrere Tabellen benötigen zusätzliche Diagnosewerkzeuge
- Routinggenerationen und Caches müssen korrekt invalidiert werden
- dynamische Quellen können widersprüchliche Routen liefern

### Risiken

- falsche Default Route
- Routinglecks zwischen VPN und normalem Netzwerk
- veraltete Cacheeinträge
- rekursive Gatewayloops
- unkontrollierte Multipath-Umsortierung
- manipulierte dynamische Routen

---

## 29. Implementierungsrichtlinien

- IPv4- und IPv6-Präfixe verwenden gemeinsame abstrakte Routing-APIs.
- Longest Prefix Match ist die Grundlage der Routenauswahl.
- Metriken werden erst zwischen vergleichbaren Routen ausgewertet.
- Routingtabellen sind pro Netzwerk-Namespace isoliert.
- Änderungen werden als neue Generation atomar veröffentlicht.
- Dynamische Routen dürfen geschützte statische Regeln nicht unautorisiert ersetzen.
- Route Caches enthalten Routing-, Interface- und Policygenerationen.
- Forwarding ist unabhängig von vorhandenen Routen zu aktivieren.
- Rekursive Next Hops und Tunnelpfade werden auf Schleifen geprüft.
- Ein Routenergebnis erteilt keine zusätzlichen Socket- oder Netzwerkrechte.

---

## 30. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- IPv4- und IPv6-Routen
- Longest Prefix Match
- Default Routes
- Metriken
- mehrere Routingtabellen
- Policy Routing
- Netzwerk-Namespaces
- dynamische Routen und Ablaufzeiten
- Interfaceausfall
- Quelladressauswahl
- Multipath Routing
- Route-Cache-Invalidierung
- transaktionale Änderungen
- Forwarding
- rekursive Gateway- und Tunnelloops

---

## 31. Verbindliche Invarianten

1. Routing unterstützt IPv4 und IPv6 über gemeinsame abstrakte Schnittstellen.
2. Jeder Netzwerk-Namespace besitzt eigene Routingtabellen.
3. Longest Prefix Match bildet die Grundlage der Routenauswahl.
4. Policy Routing wird vor der tabelleninternen Präfixsuche ausgewertet.
5. Routenänderungen werden atomar als Generation veröffentlicht.
6. Route Caches dürfen keine veralteten Policy- oder Interfacezustände verwenden.
7. Eine Route stellt keine Netzwerkzugriffsberechtigung dar.
8. Dynamische Routen können geschützte Routen nicht unautorisiert ersetzen.
9. Forwarding ist standardmäßig deaktiviert.
10. Zyklische Next-Hop- und Tunnelauflösungen werden abgelehnt.

---

## 32. Zusammenfassung

NovaOS verwendet eine protokollunabhängige, generationsbasierte Routing Engine für IPv4 und IPv6.

Die Routenauswahl kombiniert Policy Routing, Longest Prefix Match, Metriken, Interfacezustände und Quelladressauswahl. Getrennte Netzwerk-Namespaces ermöglichen isolierte Routen für Anwendungen, VPNs, Container und Systemdienste.

Atomar veröffentlichte Routinggenerationen und streng invalidierte Route Caches ermöglichen schnelle Mehrkern-Lookups, ohne teilweise aktualisierte oder sicherheitspolitisch veraltete Routen sichtbar zu machen.