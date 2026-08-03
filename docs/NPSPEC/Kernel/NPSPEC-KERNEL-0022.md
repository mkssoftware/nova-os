# NPSPEC-KERNEL-0022 – Network Stack

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0022 |
| Titel | Network Stack |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Netzwerk |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0017, NPSPEC-KERNEL-0018, NPSPEC-KERNEL-0020 |
| Zugehörige ADRs | ADR-NET-0001, ADR-NET-0002, ADR-NET-0003, ADR-NET-0004, ADR-NET-0005, ADR-NET-0006, ADR-NET-0007, ADR-NET-0008, ADR-NET-0009, ADR-NET-0010 |

---

## 1. Zweck

Diese Spezifikation definiert den Network Stack von NovaOS.

Der Netzwerk-Stack stellt ein einheitliches Framework für folgende Funktionen bereit:

- Netzwerkinterfaces,
- Ethernet und Link Layer,
- ARP und Neighbor Discovery,
- IPv4 und IPv6,
- ICMP und ICMPv6,
- UDP und TCP,
- Routing,
- Socket API,
- Firewall und Packet Filter,
- QoS,
- VPN und Tunnel,
- Netzwerk-Namespaces,
- Hardware-Offloading.

## 2. Ziele

Der Network Stack muss:

- IPv4 und IPv6 gleichwertig unterstützen,
- capability-geschützten Netzwerkzugriff erzwingen,
- mehrere Netzwerkinterfaces verwalten,
- lokale und geroutete Kommunikation ermöglichen,
- hohe Paketlast auf SMP-Systemen verarbeiten,
- Userspace-Netzwerkdienste integrieren,
- Paketfilterung und Firewallrichtlinien bereitstellen,
- alte und moderne Netzwerkhardware unterstützen,
- lokale Diagnose ohne standardmäßige externe Telemetrie ermöglichen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Network Stack | Protokolle, Routing und Paketverarbeitung |
| Driver Framework | Netzwerkgerät und Hardwarequeues |
| Socket Layer | anwendungsorientierte Netzwerkobjekte |
| Firewall | verbindungs- und richtlinienbasierte Filterung |
| Packet Filter | programmierbare Paketentscheidungen |
| VPN Framework | Tunnel und virtuelle Interfaces |
| Security Manager | Capabilities und Sicherheitsprofile |
| Power Manager | Energiezustände der Netzwerkgeräte |
| Userspace Services | DNS, DHCP, Zeitsynchronisation und Konfiguration |

## 4. Schichtenmodell

NovaOS verwendet folgende logische Schichten:

```text
Anwendung
  -> Socket API
  -> TCP / UDP / ICMP
  -> IPv4 / IPv6
  -> Routing / Firewall / Packet Filter
  -> Ethernet / Link Layer
  -> Network Interface
  -> Driver
  -> Hardware
```

Die Implementierung darf optimierte Fast Paths verwenden, solange die Sicherheits- und Protokollsemantik erhalten bleibt.

## 5. Netzwerkobjekte

Mindestens folgende Ressourcen werden als Kernelobjekte dargestellt:

```c
typedef enum np_network_object_type {
    NP_NET_INTERFACE,
    NP_NET_ADDRESS,
    NP_NET_ROUTE,
    NP_NET_SOCKET,
    NP_NET_PACKET_FILTER,
    NP_NET_FIREWALL_POLICY,
    NP_NET_NAMESPACE,
    NP_NET_TUNNEL,
    NP_NET_NEIGHBOR
} np_network_object_type_t;
```

Userspace greift ausschließlich über Handles und versionierte Services auf diese Objekte zu.

## 6. Netzwerk-Namespace

Ein Netzwerk-Namespace isoliert:

- Interfaces,
- Adressen,
- Routingtabellen,
- Firewallregeln,
- Socketbindungen,
- Neighbor Caches,
- Portbereiche,
- Tunnel.

```c
typedef struct np_network_namespace {
    np_kernel_object_t object;

    np_interface_table_t interfaces;
    np_route_tables_t routes;
    np_firewall_context_t firewall;

    uint64_t generation;
    np_rwlock_t lock;
} np_network_namespace_t;
```

Prozesse sehen standardmäßig nur den ihnen zugeordneten Netzwerk-Namespace.

## 7. Netzwerkinterface

```c
typedef struct np_network_interface {
    np_kernel_object_t object;

    np_interface_id_t interface_id;
    np_string_t name;
    np_interface_type_t type;
    np_interface_state_t state;

    np_link_address_t link_address;
    uint32_t mtu;
    uint64_t features;

    np_device_t* device;
    np_network_driver_ops_t* driver_operations;
} np_network_interface_t;
```

Virtuelle Interfaces können ohne direktes Geräteobjekt existieren.

## 8. Interfacetypen

```c
typedef enum np_interface_type {
    NP_INTERFACE_LOOPBACK,
    NP_INTERFACE_ETHERNET,
    NP_INTERFACE_WIFI,
    NP_INTERFACE_CELLULAR,
    NP_INTERFACE_TUNNEL,
    NP_INTERFACE_BRIDGE,
    NP_INTERFACE_VIRTUAL,
    NP_INTERFACE_POINT_TO_POINT
} np_interface_type_t;
```

Medienabhängige Eigenschaften werden über versionierte Erweiterungen bereitgestellt.

## 9. Interfacestatus

```c
typedef enum np_interface_state {
    NP_INTERFACE_DOWN,
    NP_INTERFACE_STARTING,
    NP_INTERFACE_UP,
    NP_INTERFACE_DORMANT,
    NP_INTERFACE_DEGRADED,
    NP_INTERFACE_STOPPING,
    NP_INTERFACE_REMOVED
} np_interface_state_t;
```

Der administrative Status und der physische Linkstatus müssen getrennt abfragbar sein.

Ein administrativ aktives Interface kann weiterhin keinen physischen Link besitzen.

## 10. Interface-Namen

Interface-Namen dienen der Darstellung und Konfiguration.

Sie müssen innerhalb eines Netzwerk-Namespace eindeutig sein.

Beispiele:

```text
lo0
eth0
wifi0
vpn0
bridge0
```

Namen verleihen keine Zugriffsrechte und ersetzen nicht die stabile Interface ID.

## 11. Paketpuffer

```c
typedef struct np_packet_buffer {
    np_atomic_uint32_t reference_count;

    uint8_t* data;
    size_t data_offset;
    size_t data_length;
    size_t capacity;

    np_interface_id_t input_interface;
    np_interface_id_t output_interface;

    uint32_t protocol;
    uint32_t flags;

    np_packet_metadata_t metadata;
} np_packet_buffer_t;
```

Paketpuffer können mehrere Fragmente oder Scatter-Gather-Segmente referenzieren.

## 12. Packet-Buffer-Anforderungen

Ein Paketpuffer muss unterstützen:

- Headroom für zusätzliche Header,
- Tailroom für Trailer und Padding,
- sichere Längenprüfungen,
- Referenzzählung,
- Copy-on-Write,
- Hardware-Offload-Metadaten,
- Zeitstempel,
- Interface- und Routinginformationen.

Paketdaten gelten bis zur vollständigen Validierung als nicht vertrauenswürdig.

## 13. Paketallokation

Häufig verwendete Paketpuffer werden über begrenzte Object Caches und CPU-lokale Pools bereitgestellt.

Bei Speicherknappheit darf der Netzwerk-Stack:

- Pakete verwerfen,
- Queuegrenzen reduzieren,
- Flow Control auslösen,
- weniger kritische Traffic-Klassen drosseln.

Normale Paketzustellung darf keine Kernel-Notfallreserve unbegrenzt verbrauchen.

## 14. Empfangspfad

Der grundlegende Empfangspfad lautet:

```text
Netzwerkhardware
  -> Treiber-RX-Queue
  -> Paketpuffer erzeugen
  -> Link-Layer validieren
  -> Ingress Packet Filter
  -> IPv4- oder IPv6-Verarbeitung
  -> Routingentscheidung
  -> lokale Firewall
  -> Transportprotokoll
  -> Socket-Empfangsqueue
  -> Anwendung
```

Unnötige Kopien sollen vermieden werden, ohne Paketlebenszeit oder Isolation zu verletzen.

## 15. Sendepfad

Der grundlegende Sendepfad lautet:

```text
Anwendung
  -> Socket-Sendepuffer
  -> Transportprotokoll
  -> IPv4- oder IPv6
  -> Routingentscheidung
  -> Egress Firewall und QoS
  -> Neighbor-Auflösung
  -> Link-Layer-Header
  -> Treiber-TX-Queue
  -> Netzwerkhardware
```

Fehler müssen mit einem definierten Socket- oder Netzwerkstatus an den Aufrufer zurückgegeben werden.

## 16. Netzwerkqueues

Jedes Interface kann mehrere Empfangs- und Sendewarteschlangen besitzen.

Queues können zugeordnet werden nach:

- CPU,
- NUMA-Node,
- Traffic-Klasse,
- Hardwarequeue,
- Flow,
- Realtime- oder QoS-Richtlinie.

Queuekapazitäten müssen begrenzt sein.

## 17. Interrupt- und Polling-Modell

Netzwerktreiber können Interrupts mit begrenztem Polling kombinieren.

Bei hoher Last:

1. Interruptquelle vorübergehend begrenzen,
2. mehrere Pakete in einem Durchlauf verarbeiten,
3. ein definiertes Paket- oder Zeitbudget einhalten,
4. verbleibende Arbeit erneut einplanen,
5. Interrupts anschließend wieder aktivieren.

Ein einzelnes Interface darf andere Kernelarbeit nicht unbegrenzt verdrängen.

## 18. Ethernet

Die Ethernet-Schicht unterstützt mindestens:

- Ethernet-II-Frames,
- Unicast,
- Broadcast,
- Multicast,
- VLAN-Tags,
- MTU-Prüfung,
- optional Jumbo Frames.

Frames unterhalb oder oberhalb zulässiger Grenzen werden verworfen.

Die Frame Check Sequence kann von der Hardware verarbeitet und muss über Offload-Metadaten korrekt beschrieben werden.

## 19. Link-Adressen

```c
typedef struct np_link_address {
    uint8_t length;
    uint8_t bytes[32];
} np_link_address_t;
```

Link-Adressen werden nicht auf sechs Byte festgelegt, da unterschiedliche Linktypen unterstützt werden können.

Ethernet-MAC-Adressen verwenden sechs Byte.

## 20. VLAN

VLAN-Unterstützung kann als virtuelles Interface implementiert werden.

```c
typedef struct np_vlan_config {
    np_interface_id_t parent_interface;
    uint16_t vlan_id;
    uint8_t priority;
} np_vlan_config_t;
```

VLAN IDs und Prioritäten werden validiert.

Hardware-Offloading darf die logische Firewall- und Namespace-Sicht nicht umgehen.

## 21. ARP

ARP wird für IPv4-Nachbarschaftsauflösung auf geeigneten Linktypen verwendet.

Der ARP Cache speichert:

- IPv4-Adresse,
- Link-Adresse,
- Interface,
- Zustand,
- Ablaufzeit,
- Vertrauensstatus.

ARP-Antworten werden auf Paketstruktur, Interface und Richtlinie geprüft.

## 22. Neighbor Discovery

IPv6 verwendet Neighbor Discovery für:

- Adressauflösung,
- Router Discovery,
- Prefix Discovery,
- Neighbor Unreachability Detection,
- Duplicate Address Detection.

ICMPv6-Nachrichten für Neighbor Discovery müssen gegen Hop-Limit-, Interface- und Strukturregeln validiert werden.

## 23. Neighbor-Zustände

```c
typedef enum np_neighbor_state {
    NP_NEIGHBOR_INCOMPLETE,
    NP_NEIGHBOR_REACHABLE,
    NP_NEIGHBOR_STALE,
    NP_NEIGHBOR_DELAY,
    NP_NEIGHBOR_PROBE,
    NP_NEIGHBOR_FAILED,
    NP_NEIGHBOR_PERMANENT
} np_neighbor_state_t;
```

Ausstehende Pakete pro ungelöstem Neighbor sind mengen- und zeitbegrenzt.

## 24. IPv4

Die IPv4-Implementierung unterstützt mindestens:

- Unicast,
- Broadcast,
- Multicast-Grundfunktionen,
- Fragmentierung und Reassembly,
- TTL,
- Headerprüfsumme,
- ICMPv4,
- mehrere Adressen pro Interface,
- Routing über mehrere Tabellen.

IPv4-Optionen werden restriktiv verarbeitet und unbekannte unsichere Optionen verworfen.

## 25. IPv4-Fragmentierung

Eingehende Fragmente werden nur unter begrenzten Ressourcen wieder zusammengesetzt.

Begrenzt werden:

- Fragmente pro Datagramm,
- Gesamtgröße,
- offene Reassembly-Kontexte,
- Timeout,
- Speicherverbrauch pro Quelle und Namespace.

Überlappende oder widersprüchliche Fragmente werden verworfen.

## 26. IPv6

Die IPv6-Implementierung unterstützt mindestens:

- Unicast,
- Link-Local-Adressen,
- Multicast,
- Stateless Address Autoconfiguration,
- Neighbor Discovery,
- ICMPv6,
- Extension Header,
- Path MTU Discovery,
- mehrere Adressen und Präfixe.

IPv6 wird nicht als optionaler Zusatz zu IPv4 behandelt.

## 27. IPv6 Extension Header

Extension Header werden in definierter Reihenfolge und mit festen Grenzen verarbeitet.

Zu begrenzen sind:

- Anzahl der Header,
- gesamte Headerlänge,
- Verschachtelung,
- unbekannte Optionen,
- Fragmentkontexte.

Ein Paket darf nicht durch übermäßige Headerverarbeitung unbegrenzt CPU-Zeit verbrauchen.

## 28. ICMP und ICMPv6

ICMP wird verwendet für:

- Fehlerberichte,
- Echo-Anfragen,
- Path MTU Discovery,
- Router- und Neighbor-Funktionen,
- Diagnose.

ICMP-Fehler dürfen nicht unkontrolliert als Antwort auf andere Fehler erzeugt werden.

Antworten werden rate-limitiert.

## 29. UDP

UDP unterstützt:

- verbindungslose Datagramme,
- IPv4 und IPv6,
- Unicast und Multicast,
- Checksummen,
- Portbindung,
- Interfacebindung,
- asynchrone Fehlerzustellung.

Bei IPv6 ist eine gültige UDP-Prüfsumme grundsätzlich erforderlich.

## 30. TCP

TCP unterstützt mindestens:

- Verbindungsaufbau und -abbau,
- zuverlässige geordnete Byteübertragung,
- Flusskontrolle,
- Congestion Control,
- Retransmission,
- Selective Acknowledgment,
- Window Scaling,
- Path MTU Discovery,
- IPv4 und IPv6.

Congestion-Control-Algorithmen werden über eine versionierte interne Schnittstelle austauschbar gehalten.

## 31. TCP-Zustände

```c
typedef enum np_tcp_state {
    NP_TCP_CLOSED,
    NP_TCP_LISTEN,
    NP_TCP_SYN_SENT,
    NP_TCP_SYN_RECEIVED,
    NP_TCP_ESTABLISHED,
    NP_TCP_FIN_WAIT_1,
    NP_TCP_FIN_WAIT_2,
    NP_TCP_CLOSE_WAIT,
    NP_TCP_CLOSING,
    NP_TCP_LAST_ACK,
    NP_TCP_TIME_WAIT
} np_tcp_state_t;
```

Zustandsübergänge müssen gegen ungültige oder unerwartete Pakete abgesichert sein.

## 32. Socketobjekt

```c
typedef struct np_socket {
    np_kernel_object_t object;

    np_address_family_t family;
    np_socket_type_t type;
    uint32_t protocol;

    np_socket_state_t state;
    np_network_namespace_t* network_namespace;

    np_socket_buffer_t receive_buffer;
    np_socket_buffer_t send_buffer;

    np_socket_operations_t* operations;
    np_mutex_t lock;
} np_socket_t;
```

Sockets sind wartbare Kernelobjekte und werden über Handles angesprochen.

## 33. Adressfamilien

```c
typedef enum np_address_family {
    NP_AF_LOCAL,
    NP_AF_IPV4,
    NP_AF_IPV6,
    NP_AF_PACKET
} np_address_family_t;
```

Raw- und Packet-Sockets benötigen spezielle Capabilities.

Lokale Sockets verwenden das IPC Framework und sind nicht über externe Interfaces erreichbar.

## 34. Sockettypen

```c
typedef enum np_socket_type {
    NP_SOCKET_STREAM,
    NP_SOCKET_DATAGRAM,
    NP_SOCKET_SEQPACKET,
    NP_SOCKET_RAW
} np_socket_type_t;
```

Nicht jede Kombination aus Adressfamilie, Typ und Protokoll ist gültig.

Ungültige Kombinationen werden beim Erzeugen abgelehnt.

## 35. Socket-API

```c
np_status_t np_socket_create(
    np_address_family_t family,
    np_socket_type_t type,
    uint32_t protocol,
    np_handle_t* socket
);

np_status_t np_socket_bind(
    np_handle_t socket,
    const np_socket_address_t* address
);

np_status_t np_socket_connect(
    np_handle_t socket,
    const np_socket_address_t* address,
    np_time_ns_t deadline
);

np_status_t np_socket_listen(
    np_handle_t socket,
    uint32_t backlog
);

np_status_t np_socket_accept(
    np_handle_t socket,
    np_time_ns_t deadline,
    np_handle_t* client_socket
);
```

## 36. Socket-I/O

```c
np_status_t np_socket_send(
    np_handle_t socket,
    const np_socket_io_args_t* arguments,
    size_t* bytes_sent
);

np_status_t np_socket_receive(
    np_handle_t socket,
    np_socket_io_args_t* arguments,
    size_t* bytes_received
);
```

Unterstützt werden:

- blockierende Operationen,
- nicht blockierende Operationen,
- Timeouts,
- asynchrone Completion,
- Scatter-Gather-I/O,
- partielle Ergebnisse.

## 37. Portverwaltung

Portbindungen werden pro Netzwerk-Namespace verwaltet.

Der Stack unterstützt:

- explizite Ports,
- ephemere Ports,
- Listener,
- Interface- und Adressbindung,
- kontrollierte Portwiederverwendung.

Privilegierte oder reservierte Ports können besondere Capabilities erfordern.

## 38. Routingtabellen

Routing wird über mehrere Tabellen und Richtlinien unterstützt.

```c
typedef struct np_route_entry {
    np_network_prefix_t destination;
    np_network_address_t gateway;

    np_interface_id_t interface_id;
    uint32_t metric;
    uint32_t table_id;
    uint32_t flags;

    np_time_ns_t expiration;
} np_route_entry_t;
```

Die längste passende Präfixübereinstimmung hat grundsätzlich Vorrang.

## 39. Policy Routing

Eine Routingentscheidung kann zusätzlich berücksichtigen:

- Quelladresse,
- Zieladresse,
- Interface,
- Traffic-Klasse,
- Sicherheitsdomain,
- Socketmarkierung,
- VPN-Richtlinie,
- Benutzer oder Anwendung,
- Firewallmarkierung.

Policy Routing darf Capability- und Namespacegrenzen nicht umgehen.

## 40. Routing Cache

Routingentscheidungen dürfen gecacht werden.

Cacheeinträge müssen bei Änderungen an folgenden Daten invalidiert werden:

- Routingtabellen,
- Interfacezuständen,
- Adressen,
- Firewall- oder Policyregeln,
- Netzwerk-Namespaces,
- Tunnelzuständen.

Veraltete Cacheeinträge dürfen keine neuen Sicherheitsrichtlinien umgehen.

## 41. IP Forwarding

IP Forwarding ist pro Netzwerk-Namespace konfigurierbar und standardmäßig deaktiviert, sofern das Systemprofil nichts anderes vorsieht.

Für weitergeleitete Pakete gelten:

- Ingress-Filter,
- Routingentscheidung,
- Forward-Firewall,
- TTL- oder Hop-Limit-Verarbeitung,
- Egress-Filter,
- QoS.

Die Aktivierung erfordert administrative Netzwerkrechte.

## 42. Firewall

Die Firewall verarbeitet zustandslose und zustandsbehaftete Regeln.

Mögliche Aktionen sind:

```c
typedef enum np_firewall_action {
    NP_FIREWALL_ALLOW,
    NP_FIREWALL_DROP,
    NP_FIREWALL_REJECT,
    NP_FIREWALL_LOG,
    NP_FIREWALL_RATE_LIMIT,
    NP_FIREWALL_REDIRECT
} np_firewall_action_t;
```

Standardrichtlinien sind pro Netzwerk-Namespace definierbar.

## 43. Stateful Firewall

Connection Tracking kann Zustände unterscheiden:

```c
typedef enum np_connection_state {
    NP_CONNECTION_NEW,
    NP_CONNECTION_ESTABLISHED,
    NP_CONNECTION_RELATED,
    NP_CONNECTION_INVALID
} np_connection_state_t;
```

Connection-Tracking-Tabellen besitzen feste Ressourcenlimits und Timeouts.

Ungültige Zustände werden standardmäßig restriktiv behandelt.

## 44. Packet Filter

Programmierbare Paketfilter können an definierten Hooks ausgeführt werden:

- Interface Ingress,
- Pre-Routing,
- Local Input,
- Forward,
- Local Output,
- Post-Routing,
- Interface Egress,
- Socketebene.

Filterprogramme müssen vor der Aktivierung verifiziert werden.

## 45. Filterverifikation

Ein Filterprogramm muss:

- garantierte Beendigung besitzen,
- gültige Paketgrenzen prüfen,
- begrenzten Stack und Speicher verwenden,
- ausschließlich erlaubte Hilfsfunktionen aufrufen,
- keine Kernelzeiger offenlegen,
- definierte maximale Ausführungszeit einhalten.

Nicht verifizierbare Programme werden abgelehnt.

## 46. QoS

QoS unterstützt:

- Traffic-Klassen,
- Prioritätsqueues,
- Bandbreitenlimits,
- Token-Bucket-Verfahren,
- Fair Queuing,
- Latenzklassen,
- Anwendungspolicies.

QoS-Prioritäten dürfen Kernelkontrollverkehr nicht unkontrolliert verdrängen.

## 47. Traffic Shaping

Traffic Shaping kann angewendet werden pro:

- Interface,
- Socket,
- Prozess,
- Job Object,
- Sicherheitsdomain,
- Netzwerk-Namespace,
- Traffic-Klasse.

Bandbreitenlimits werden mit monotone Zeit und begrenzten Queuegrößen umgesetzt.

## 48. VPN und Tunnel

Tunnel werden als virtuelle Netzwerkinterfaces dargestellt.

Unterstützte Tunnelklassen können sein:

- IP-in-IP,
- verschlüsselte VPN-Tunnel,
- Userspace-VPN,
- virtuelle Overlay-Netze,
- Unternehmens-Tunnel.

Schlüsselverwaltung und Kryptografie erfolgen über den Security Manager oder autorisierte Userspace-Dienste.

## 49. VPN-Routing

VPN-Richtlinien können definieren:

- Full Tunnel,
- Split Tunnel,
- anwendungsbezogenes Routing,
- zielbezogenes Routing,
- DNS-Zuordnung,
- Kill Switch.

Ein Kill Switch muss verhindern, dass geschützter Verkehr bei Tunnelausfall unverschlüsselt über ein anderes Interface gesendet wird.

## 50. TLS Offloading

Der Netzwerk-Stack kann TLS-Offloading unterstützen.

Mögliche Formen sind:

- Hardware-Offload,
- Kernel-Datapath-Offload,
- Userspace-Kryptodienst,
- NIC-basierte Verschlüsselung.

Private Schlüssel dürfen nur an ein Offload-Ziel übergeben werden, wenn Trust, Capability und Gerätesicherheit ausreichend geprüft wurden.

TLS-Offloading darf die Socket- und Sicherheitssemantik nicht verändern.

## 51. Hardware-Offloading

Unterstützte Offloads können sein:

- TX Checksum,
- RX Checksum,
- TCP Segmentation Offload,
- Generic Segmentation Offload,
- Large Receive Offload,
- Receive Side Scaling,
- VLAN Offload,
- Flow Steering,
- TLS Offload.

Treiber melden die tatsächlich verfügbaren Funktionen.

Der Stack validiert Offload-Metadaten und kann Funktionen bei Fehlern deaktivieren.

## 52. Zero-Copy

Zero-Copy kann für große Sockettransfers bereitgestellt werden.

Voraussetzungen sind:

- klar definierte Pufferlebenszeit,
- Pinning- oder Shared-Memory-Grenzen,
- Abschlussbenachrichtigung,
- Prozess- und Job-Limits,
- sichere Behandlung bei Abbruch.

Kleine Pakete sollen nicht zwangsläufig durch aufwendiges Pinning optimiert werden.

## 53. Multicast

IPv4- und IPv6-Multicast unterstützen:

- Gruppenbeitritt und -austritt,
- Interfaceauswahl,
- Quellfilterung, sofern implementiert,
- Loopback-Konfiguration,
- TTL oder Hop Limit.

Gruppenmitgliedschaften werden pro Socket und Netzwerk-Namespace verwaltet.

## 54. Adresskonfiguration

Adresskonfiguration kann erfolgen durch:

- statische Konfiguration,
- DHCPv4,
- DHCPv6,
- IPv6 SLAAC,
- Link-Local-Autokonfiguration,
- VPN- oder Unternehmensrichtlinie.

DHCP und komplexe Policylogik werden bevorzugt durch capability-geschützte Userspace-Dienste ausgeführt.

## 55. DNS

DNS-Auflösung ist kein Bestandteil der grundlegenden Kernel-Socket-API.

Ein lokaler Userspace-Resolver stellt bereit:

- DNS-Caching,
- DNSSEC-Validierung,
- Split DNS,
- VPN-DNS-Richtlinien,
- lokale Namensauflösung,
- Datenschutzrichtlinien.

Der Kernel verwaltet ausschließlich Netzwerktransport und notwendige Resolverendpunkte.

## 56. Netzwerkereignisse

Das Subsystem veröffentlicht unter anderem:

```text
INTERFACE_ADDED
INTERFACE_REMOVED
INTERFACE_UP
INTERFACE_DOWN
LINK_STATE_CHANGED
ADDRESS_ADDED
ADDRESS_REMOVED
ROUTE_CHANGED
CONNECTIVITY_CHANGED
FIREWALL_POLICY_CHANGED
VPN_STATE_CHANGED
NETWORK_ERROR
```

Empfänger sehen nur Ereignisse ihres Netzwerk-Namespace oder ausdrücklich freigegebener Interfaces.

## 57. Ressourcenlimits

Limits können gelten für:

- offene Sockets,
- Socketpuffer,
- Connection-Tracking-Einträge,
- Reassembly-Kontexte,
- Neighbor-Einträge,
- ausstehende Verbindungen,
- Packet-Filter-Programme,
- Routingeinträge,
- Multicast-Gruppen,
- gepinnte Zero-Copy-Seiten.

Limits werden pro Prozess, Job, Namespace und System verwaltet.

## 58. Synchronisation

Der Network Stack verwendet:

- per-CPU-Paketqueues,
- interface- und socketlokale Sperren,
- RCU-ähnliche Routingtabellen,
- atomare Statistiken,
- begrenzte Wait Queues,
- referenzgezählte Paketpuffer.

Eine globale Netzwerksperre im regulären Empfangs- und Sendepfad ist unzulässig.

## 59. CPU- und NUMA-Lokalität

Hardwarequeues, Interrupts und Paketverarbeitung sollen passend zu CPU- und NUMA-Topologie zugeordnet werden.

Zu berücksichtigen sind:

- NIC-NUMA-Node,
- RX-/TX-Queue,
- Interrupt-Affinität,
- Anwendungsthread,
- Speicherort der Paketpuffer,
- CPU-Auslastung.

Migrationen dürfen die Paketreihenfolge innerhalb eines Flows nicht unkontrolliert verändern.

## 60. Power Management

Bei Interface-Suspend muss der Network Stack:

- neue Übertragungen begrenzen,
- ausstehende Pakete abschließen oder verwerfen,
- Wake-on-LAN konfigurieren,
- Treiberzustand sichern,
- Routingstatus aktualisieren.

Nach Resume werden Link, Adressen, Neighbor Cache und Verbindungen geprüft.

Nicht fortsetzbare Verbindungen erhalten einen definierten Fehler.

## 61. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_NET_CONNECT` | ausgehende Verbindungen herstellen |
| `CAP_NET_LISTEN` | eingehende Listener erstellen |
| `CAP_NET_LOCAL` | lokale IPC-Sockets verwenden |
| `CAP_NET_RAW` | Raw- oder Packet-Sockets verwenden |
| `CAP_NET_INTERFACE` | Interface konfigurieren |
| `CAP_NET_ADDRESS` | Netzwerkadressen verwalten |
| `CAP_NET_ROUTE` | Routingtabellen verändern |
| `CAP_NET_FIREWALL` | Firewallregeln verwalten |
| `CAP_NET_FILTER` | Packet Filter laden |
| `CAP_NET_VPN` | Tunnel und VPN-Richtlinien verwalten |
| `CAP_NET_NAMESPACE` | Netzwerk-Namespaces verwalten |
| `CAP_NET_ADMIN` | globale Netzwerkkonfiguration verwalten |

Rechte können auf Ziele, Ports, Protokolle, Interfaces und Namespaces beschränkt werden.

## 62. Sicherheit

Der Network Stack muss sicherstellen, dass:

- Pakete vor Headerzugriffen längenvalidiert werden,
- Integer- und Längenüberläufe verhindert werden,
- fragmentierte Pakete feste Ressourcenlimits besitzen,
- Raw Sockets besondere Capabilities benötigen,
- Netzwerk-Namespaces vollständig getrennt bleiben,
- Firewall- und Filterregeln Offloads einschließen,
- DMA- und Paketpuffer nur autorisierten Treibern zugänglich sind,
- Routing- und Firewall-Caches bei Policyänderungen invalidiert werden,
- VPN-Schlüssel nicht über Diagnose-APIs offengelegt werden,
- Paketfilterprogramme vor der Ausführung verifiziert werden.

## 63. Datenschutz

Netzwerkdaten können besonders sensible Informationen enthalten.

Daher gelten:

- Paketpayloads werden standardmäßig nicht aufgezeichnet,
- detailliertes Packet Capture benötigt besondere Capabilities,
- Verbindungsmetadaten bleiben standardmäßig lokal,
- DNS- und VPN-Richtlinien respektieren die Benutzerkonfiguration,
- externe Telemetrie ist standardmäßig deaktiviert,
- Anwendungen sehen nur ihre eigenen oder ausdrücklich freigegebenen Sockets,
- MAC- und Gerätekennungen werden nur bei Bedarf offengelegt.

## 64. Diagnose

Der Network Stack stellt autorisierten Diagnosediensten bereit:

- Interfacezustände,
- Adressen und Routen,
- Socketzustände,
- Paket- und Bytezähler,
- Fehler und Drops,
- Queueauslastung,
- Firewallentscheidungen,
- Connection Tracking,
- Fragment-Reassembly,
- Neighbor-Zustände,
- Offload-Status,
- VPN- und Tunnelzustände.

Payload-Inhalte erfordern eine separate Packet-Capture-Berechtigung.

## 65. Statistik

```c
typedef struct np_network_statistics {
    uint64_t received_packets;
    uint64_t transmitted_packets;
    uint64_t received_bytes;
    uint64_t transmitted_bytes;

    uint64_t dropped_packets;
    uint64_t checksum_errors;
    uint64_t malformed_packets;

    uint64_t active_sockets;
    uint64_t active_connections;
    uint64_t firewall_drops;

    uint64_t route_lookups;
    uint64_t route_cache_hits;
} np_network_statistics_t;
```

Statistiken sollen pro Interface, Namespace, Protokoll und System verfügbar sein.

## 66. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_ADDRESS_INVALID` | Netzwerkadresse ist ungültig |
| `NP_ERR_ADDRESS_IN_USE` | Adresse oder Port ist bereits belegt |
| `NP_ERR_ADDRESS_UNAVAILABLE` | Adresse ist lokal nicht verfügbar |
| `NP_ERR_NETWORK_UNREACHABLE` | Netzwerk ist nicht erreichbar |
| `NP_ERR_HOST_UNREACHABLE` | Zielhost ist nicht erreichbar |
| `NP_ERR_CONNECTION_REFUSED` | Verbindung wurde abgelehnt |
| `NP_ERR_CONNECTION_RESET` | Verbindung wurde zurückgesetzt |
| `NP_ERR_CONNECTION_CLOSED` | Verbindung wurde geschlossen |
| `NP_ERR_TIMEOUT` | Netzwerkoperation lief in einen Timeout |
| `NP_ERR_MESSAGE_TOO_LARGE` | Paket überschreitet die zulässige Größe |
| `NP_ERR_WOULD_BLOCK` | Operation würde blockieren |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_INTERFACE_DOWN` | Interface ist nicht aktiv |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 67. Fehlerbehandlung

Fehlerhafte oder ungültige Pakete werden verworfen und gegebenenfalls rate-limitiert diagnostiziert.

Ein Treiber- oder Interfacefehler kann führen zu:

- Queue-Stopp,
- Link Reset,
- Treiberneustart,
- Interface-Deaktivierung,
- Routing auf ein anderes Interface,
- VPN-Failover,
- definierten Socketfehlern.

Ein einzelnes fehlerhaftes Paket darf keine Kernel Panic auslösen.

## 68. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Loopback-Interface,
2. Ethernet-Empfang und -Versand,
3. VLAN-Interface,
4. Interface-Up und -Down,
5. ARP-Auflösung,
6. IPv6 Neighbor Discovery,
7. IPv4-Unicast,
8. IPv4-Fragmentierung und Reassembly,
9. Ablehnung überlappender Fragmente,
10. IPv6-Unicast,
11. IPv6 Extension Header,
12. ICMPv4 Echo,
13. ICMPv6 Echo,
14. UDP über IPv4,
15. UDP über IPv6,
16. TCP-Verbindungsaufbau,
17. TCP-Datenübertragung,
18. TCP-Verbindungsabbau,
19. TCP-Retransmission,
20. Socket Bind und Connect,
21. Listen und Accept,
22. nicht blockierende Socket-I/O,
23. Socket-Timeout,
24. IPv4- und IPv6-Multicast,
25. längste Präfixübereinstimmung,
26. Policy Routing,
27. Routing-Cache-Invalidierung,
28. IP Forwarding,
29. zustandslose Firewall,
30. Stateful Firewall,
31. Packet-Filter-Verifikation,
32. QoS und Bandbreitenlimit,
33. VPN Full Tunnel,
34. VPN Split Tunnel,
35. VPN Kill Switch,
36. Netzwerk-Namespace-Isolation,
37. Raw-Socket-Capability,
38. Hardware-Checksum-Offload,
39. Segmentation Offload,
40. Zero-Copy-Transfer,
41. Multi-Queue-NIC auf mehreren CPUs,
42. Netzwerkgerät-Suspend und Resume,
43. Treiberabsturz und Interface-Recovery,
44. Paket- und Queue-Ressourcenlimits,
45. Schutz gegen malformed Packets,
46. korrekte Diagnose und Statistik.

## 69. Verbindliche Invarianten

1. Jeder Paketheader wird vor dem Zugriff vollständig längenvalidiert.
2. Ein Paketpuffer wird nicht nach seiner endgültigen Freigabe verwendet.
3. Netzwerk-Namespaces teilen keine Ressourcen ohne ausdrückliche Freigabe.
4. Raw- und Packet-Sockets erfordern besondere Capabilities.
5. Routing- und Firewallentscheidungen beachten den aktiven Namespace.
6. Paketfilterprogramme werden vor ihrer Aktivierung verifiziert.
7. Reassembly-Kontexte besitzen feste Zeit- und Speicherlimits.
8. Hardware-Offloading umgeht keine Firewall- oder Sicherheitsrichtlinie.
9. Ein Socket wird ausschließlich über capability-geschützte Handles verwendet.
10. VPN-Kill-Switch-Richtlinien verhindern ungeschützten Fallbackverkehr.
11. Ein fehlerhaftes Netzwerkpaket bleibt auf den Paketverarbeitungspfad begrenzt.
12. Netzwerkzustandsänderungen erfolgen unter geeigneter Synchronisation.

## 70. Referenzablauf eines eingehenden Pakets

```text
Netzwerkhardware empfängt Frame
  -> Treiber legt Paket in RX-Queue
  -> Paketpuffer und Offload-Metadaten validieren
  -> Ethernet- oder Link-Layer verarbeiten
  -> Ingress Packet Filter ausführen
  -> IPv4- oder IPv6-Header prüfen
  -> Fragmentierung gegebenenfalls behandeln
  -> Routingentscheidung treffen
  -> Firewall- und Namespace-Regeln anwenden
  -> TCP-, UDP- oder ICMP-Verarbeitung
  -> passenden Socket bestimmen
  -> Paket in begrenzte Empfangsqueue einfügen
  -> wartenden Anwendungsthread aufwecken
```

## 71. Zusammenfassung

Der NovaOS Network Stack stellt IPv4 und IPv6 als gleichwertige Protokolle über ein gemeinsames objekt- und capability-basiertes Framework bereit.

Per-CPU-Queues, Multi-Queue-Treiber, Polling unter Last und kontrollierte Hardware-Offloads ermöglichen hohe Leistung. Routing, Firewall, Packet Filter, QoS, VPN und Netzwerk-Namespaces bleiben dabei Teil derselben Sicherheits- und Objektarchitektur.

Feste Ressourcenlimits, vollständige Paketvalidierung, isolierte Treiberdomains und standardmäßig lokale Diagnose verhindern, dass fehlerhafte Pakete oder unprivilegierte Anwendungen den Kernel oder fremde Netzwerkbereiche unkontrolliert beeinflussen.