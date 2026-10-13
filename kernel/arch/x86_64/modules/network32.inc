; ---------------------------------------------------------------------------
; Nova Network Stack
; NPSPEC-NETWORK-STACK-0001      - Stack-Architektur
; NPSPEC-NETWORK-IPV4-0001       - IPv4
; NPSPEC-NETWORK-IPV6-0001       - IPv6
; NPSPEC-NETWORK-TCP-0001        - TCP
; NPSPEC-NETWORK-UDP-0001        - UDP
; NPSPEC-NETWORK-DNS-0001        - DNS-Resolver (Stub)
; NPSPEC-NETWORK-DHCP-0001       - DHCP-Client (Stub)
; NPSPEC-NETWORK-SLAAC-0001      - IPv6 SLAAC (Stub)
; NPSPEC-NETWORK-FIREWALL-0001   - Paketfilter
; NPSPEC-NETWORK-ROUTING-0001    - Routing-Tabelle
; NPSPEC-NETWORK-QOS-0001        - QoS-Klassifizierung
; NPSPEC-NETWORK-FLOW-0001       - Flow-Tracking
; NPSPEC-NETWORK-NAMESPACE-0001  - Netzwerk-Namespaces
; NPSPEC-NETWORK-NAT-0001        - NAT (Stub)
; NPSPEC-NETWORK-VPN-0001        - VPN-Interface (Stub)
; NPSPEC-NETWORK-QUIC-0001       - QUIC-Protokoll (Stub)
; NPSPEC-NETWORK-DISCOVERY-0001  - Geraete-Erkennung
; NPSPEC-NETWORK-CONGESTION-0001 - Congestion-Control
; NPSPEC-NETWORK-MULTIPATH-0001  - Multipath-Routing
; NPSPEC-NETWORK-MIGRATION-0001  - Connection-Migration
; NPSPEC-NETWORK-VIRTUAL-0001    - Virtuelle Netzwerke
; NPSPEC-NETWORK-ZEROCOPY-0001   - Zero-Copy (Stub)
; NPSPEC-NETWORK-KERNELBYPASS-0001 - Kernel-Bypass (Stub)
; NPSPEC-NETWORK-INTENT-0001     - Intent-basiertes Netz
; NPSPEC-NETWORK-SOVEREIGNTY-0001- Datensouveraenitaet
; NPSPEC-NETWORK-INTROSPECTION-0001 - Introspection
; ---------------------------------------------------------------------------

; Netzwerk-Interface-Typen (NPSPEC-NETWORK-STACK-0001)
NETIF_TYPE_NONE       equ 0
NETIF_TYPE_ETHERNET   equ 1
NETIF_TYPE_LOOPBACK   equ 2
NETIF_TYPE_WIFI       equ 3
NETIF_TYPE_VIRTUAL    equ 4    ; (NPSPEC-NETWORK-VIRTUAL-0001)
NETIF_TYPE_VPN        equ 5    ; (NPSPEC-NETWORK-VPN-0001)
NETIF_TYPE_TUNNEL     equ 6

; Interface-Flags
NETIF_FLAG_UP         equ 0x01
NETIF_FLAG_RUNNING    equ 0x02
NETIF_FLAG_PROMISC    equ 0x04
NETIF_FLAG_MULTICAST  equ 0x08
NETIF_FLAG_IPV4       equ 0x10
NETIF_FLAG_IPV6       equ 0x20
NETIF_FLAG_DHCP       equ 0x40  ; Adresse per DHCP (NPSPEC-NETWORK-DHCP-0001)
NETIF_FLAG_SLAAC      equ 0x80  ; IPv6-SLAAC (NPSPEC-NETWORK-SLAAC-0001)

; Protokoll-Typen
NET_PROTO_NONE        equ 0
NET_PROTO_ICMP        equ 1
NET_PROTO_TCP         equ 6
NET_PROTO_UDP         equ 17
NET_PROTO_QUIC        equ 0x51  ; QUIC ueber UDP (NPSPEC-NETWORK-QUIC-0001)
NET_PROTO_ICMPV6      equ 58

; Firewall-Aktionen (NPSPEC-NETWORK-FIREWALL-0001)
FW_ACTION_ACCEPT      equ 0
FW_ACTION_DROP        equ 1
FW_ACTION_REJECT      equ 2
FW_ACTION_LOG         equ 3

; Firewall-Richtungen
FW_DIR_IN             equ 0
FW_DIR_OUT            equ 1

; QoS-Klassen (NPSPEC-NETWORK-QOS-0001)
NET_QOS_REALTIME      equ 0    ; Niedrigste Latenz
NET_QOS_INTERACTIVE   equ 1    ; Interaktiv
NET_QOS_BULK          equ 2    ; Bulk-Transfer
NET_QOS_BACKGROUND    equ 3    ; Hintergrund
NET_QOS_BEST_EFFORT   equ 4    ; Kein Vorrang

; Congestion-Algorithmen (NPSPEC-NETWORK-CONGESTION-0001)
CONG_RENO             equ 0
CONG_CUBIC            equ 1
CONG_BBR              equ 2    ; BBR (Stub)

; Netzwerk-Interface-Record (64 Bytes)
NETIF_SIZE            equ 64
NI_ID                 equ 0    ; Interface-ID
NI_TYPE               equ 4    ; NETIF_TYPE_*
NI_FLAGS              equ 8    ; NETIF_FLAG_*
NI_MTU                equ 12   ; MTU in Bytes
NI_IPV4_ADDR          equ 16   ; IPv4-Adresse (Big-Endian)
NI_IPV4_MASK          equ 20   ; IPv4-Netzmaske
NI_IPV4_GW            equ 24   ; Default-Gateway
NI_MAC                equ 28   ; MAC-Adresse (6 Bytes + 2 Pad)
NI_TX_BYTES           equ 36   ; Gesendete Bytes
NI_RX_BYTES           equ 40   ; Empfangene Bytes
NI_TX_PACKETS         equ 44   ; Gesendete Pakete
NI_RX_PACKETS         equ 48   ; Empfangene Pakete
NI_NS_ID              equ 52   ; Namespace-ID (NPSPEC-NETWORK-NAMESPACE-0001)
NI_QOS_DEFAULT        equ 56   ; Standard-QoS-Klasse
NI_RESERVED           equ 60

NETIF_CAPACITY        equ 4

; Routing-Eintrag (32 Bytes, NPSPEC-NETWORK-ROUTING-0001)
ROUTE_SIZE            equ 32
RT_DEST               equ 0    ; Ziel-Netz (IPv4)
RT_MASK               equ 4    ; Netzmaske
RT_GW                 equ 8    ; Gateway
RT_IFACE              equ 12   ; Interface-ID
RT_METRIC             equ 16   ; Metrik (kleiner = bevorzugt)
RT_FLAGS              equ 20   ; 0x1=host, 0x2=gateway, 0x4=static
RT_MULTIPATH          equ 24   ; Multipath-Gewicht (NPSPEC-NETWORK-MULTIPATH-0001)
RT_RESERVED           equ 28

ROUTE_CAPACITY        equ 8

; Firewall-Regel (32 Bytes, NPSPEC-NETWORK-FIREWALL-0001)
FW_RULE_SIZE          equ 32
FR_PRIORITY           equ 0
FR_SRC_ADDR           equ 4
FR_SRC_MASK           equ 8
FR_DST_ADDR           equ 12
FR_DST_MASK           equ 16
FR_PROTO              equ 20   ; NET_PROTO_*
FR_PORT               equ 22   ; Ziel-Port (0=alle)
FR_DIR                equ 24   ; FW_DIR_*
FR_ACTION             equ 28   ; FW_ACTION_*

FW_RULE_CAPACITY      equ 8

; Flow-Record (32 Bytes, NPSPEC-NETWORK-FLOW-0001)
FLOW_SIZE             equ 32
FL_SRC_ADDR           equ 0
FL_DST_ADDR           equ 4
FL_SRC_PORT           equ 8
FL_DST_PORT           equ 10
FL_PROTO              equ 12
FL_STATE              equ 13   ; TCP-State oder 0
FL_IFACE              equ 14
FL_RESERVED           equ 15
FL_BYTES              equ 16
FL_PACKETS            equ 20
FL_LAST_TICK          equ 24
FL_QOS                equ 28

FLOW_CAPACITY         equ 16

; Netzwerk-Namespace (16 Bytes, NPSPEC-NETWORK-NAMESPACE-0001)
NET_NS_SIZE           equ 16
NN_ID                 equ 0
NN_FLAGS              equ 4    ; 0x1=isoliert, 0x2=NAT
NN_IFACE_MASK         equ 8    ; Bitmaske erlaubter Interfaces
NN_RESERVED           equ 12

NET_NS_CAPACITY       equ 4

; DNS-Cache-Eintrag (32 Bytes, NPSPEC-NETWORK-DNS-0001)
DNS_CACHE_SIZE        equ 32
DC_HASH               equ 0    ; CRC32 des Hostnamens
DC_ADDR               equ 4    ; IPv4-Adresse
DC_TTL                equ 8    ; TTL in Sekunden
DC_FLAGS              equ 12   ; 0x1=valid, 0x2=negative
DC_NAME               equ 16   ; 16 Bytes Hostname-Anfang

DNS_CACHE_CAPACITY    equ 8

; ---------------------------------------------------------------------------
; network_stack_initialize
; CF=0 Erfolg
; ---------------------------------------------------------------------------
network_stack_initialize:
    ; Tabellen loeschen
    mov edi, net_interfaces
    xor eax, eax
    mov ecx, (NETIF_CAPACITY * NETIF_SIZE) / 4
    rep stosd
    mov edi, net_routes
    mov ecx, (ROUTE_CAPACITY * ROUTE_SIZE) / 4
    rep stosd
    mov edi, net_fw_rules
    mov ecx, (FW_RULE_CAPACITY * FW_RULE_SIZE) / 4
    rep stosd
    mov edi, net_flows
    mov ecx, (FLOW_CAPACITY * FLOW_SIZE) / 4
    rep stosd
    mov edi, net_namespaces
    mov ecx, (NET_NS_CAPACITY * NET_NS_SIZE) / 4
    rep stosd
    mov edi, net_dns_cache
    mov ecx, (DNS_CACHE_CAPACITY * DNS_CACHE_SIZE) / 4
    rep stosd

    mov dword [net_if_count], 0
    mov dword [net_route_count], 0
    mov dword [net_fw_count], 0
    mov dword [net_flow_count], 0
    mov dword [net_ns_count], 0
    mov dword [net_dns_count], 0
    mov dword [net_cong_algo], CONG_CUBIC
    mov dword [net_total_tx], 0
    mov dword [net_total_rx], 0

    ; Loopback-Interface registrieren (127.0.0.1/8)
    mov eax, 1                        ; ID
    mov ebx, NETIF_TYPE_LOOPBACK
    mov ecx, NETIF_FLAG_UP | NETIF_FLAG_RUNNING | NETIF_FLAG_IPV4
    mov edx, 0x0100007F               ; 127.0.0.1 LE
    call network_if_register

    ; Loopback-Route (127.0.0.0/8 -> lo)
    mov eax, 0x0000007F               ; 127.0.0.0
    mov ebx, 0x000000FF               ; /8 Maske
    xor ecx, ecx                      ; kein Gateway
    mov edx, 1                        ; Interface 1 (lo)
    call network_route_add

    ; Default-Namespace anlegen
    call network_ns_create_default

    mov dword [net_stack_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; network_if_register: EAX=id, EBX=type, ECX=flags, EDX=ipv4_addr
; CF=0 Erfolg
; ---------------------------------------------------------------------------
network_if_register:
    cmp dword [net_if_count], NETIF_CAPACITY
    jae .full
    push edi
    push esi
    mov edi, net_interfaces
    push ecx
    mov ecx, NETIF_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    cmp dword [edi + NI_ID], 0
    je .got_slot
    add edi, NETIF_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop ecx
    pop esi
    pop edi
    stc
    ret
.got_slot:
    pop ecx
    mov [edi + NI_ID], eax
    mov [edi + NI_TYPE], ebx
    mov [edi + NI_FLAGS], ecx
    mov dword [edi + NI_MTU], 1500
    mov [edi + NI_IPV4_ADDR], edx
    cmp ebx, NETIF_TYPE_LOOPBACK
    jne .normal_mtu
    mov dword [edi + NI_MTU], 65536
.normal_mtu:
    mov dword [edi + NI_IPV4_MASK], 0x00FFFFFF  ; /24 Default
    mov dword [edi + NI_IPV4_GW], 0
    mov dword [edi + NI_TX_BYTES], 0
    mov dword [edi + NI_RX_BYTES], 0
    mov dword [edi + NI_TX_PACKETS], 0
    mov dword [edi + NI_RX_PACKETS], 0
    mov dword [edi + NI_NS_ID], 1       ; Default-Namespace
    mov dword [edi + NI_QOS_DEFAULT], NET_QOS_BEST_EFFORT
    inc dword [net_if_count]
    pop esi
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; network_route_add: EAX=dest, EBX=mask, ECX=gw, EDX=iface_id
; (NPSPEC-NETWORK-ROUTING-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
network_route_add:
    cmp dword [net_route_count], ROUTE_CAPACITY
    jae .full
    push edi
    mov edi, net_routes
    push ecx
    push edx
    mov ecx, [net_route_count]
    imul ecx, ROUTE_SIZE
    add edi, ecx
    pop edx
    pop ecx
    mov [edi + RT_DEST], eax
    mov [edi + RT_MASK], ebx
    mov [edi + RT_GW], ecx
    mov [edi + RT_IFACE], edx
    mov dword [edi + RT_METRIC], 100
    mov dword [edi + RT_FLAGS], 0x2    ; gateway
    mov dword [edi + RT_MULTIPATH], 1
    inc dword [net_route_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; network_route_lookup: EAX=dst_addr -> EBX=iface_id, ECX=gw (CF=0) oder CF=1
; (NPSPEC-NETWORK-ROUTING-0001, NPSPEC-NETWORK-MULTIPATH-0001)
; Longest-Prefix-Match (vereinfacht: erste passende Route)
; ---------------------------------------------------------------------------
network_route_lookup:
    push esi
    push edx
    mov esi, net_routes
    mov edx, [net_route_count]
.scan:
    test edx, edx
    jz .not_found
    mov ecx, [esi + RT_MASK]
    and ecx, eax
    cmp ecx, [esi + RT_DEST]
    je .found
    add esi, ROUTE_SIZE
    dec edx
    jmp .scan
.found:
    mov ebx, [esi + RT_IFACE]
    mov ecx, [esi + RT_GW]
    pop edx
    pop esi
    clc
    ret
.not_found:
    pop edx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; network_fw_add_rule: EAX=src, EBX=dst, ECX=proto|port, EDX=dir|action
; (NPSPEC-NETWORK-FIREWALL-0001)
; CF=0 Erfolg
; ---------------------------------------------------------------------------
network_fw_add_rule:
    cmp dword [net_fw_count], FW_RULE_CAPACITY
    jae .full
    push edi
    mov edi, net_fw_rules
    push ecx
    mov ecx, [net_fw_count]
    imul ecx, FW_RULE_SIZE
    add edi, ecx
    pop ecx
    mov dword [edi + FR_PRIORITY], 0
    mov [edi + FR_SRC_ADDR], eax
    mov dword [edi + FR_SRC_MASK], 0xFFFFFFFF
    mov [edi + FR_DST_ADDR], ebx
    mov dword [edi + FR_DST_MASK], 0xFFFFFFFF
    ; ECX: lo-Wort=proto, hi-Wort=port
    movzx eax, cx
    mov [edi + FR_PROTO], ax
    shr ecx, 16
    mov [edi + FR_PORT], cx
    ; EDX: lo-Byte=dir, hi-Byte=action
    movzx eax, dl
    mov [edi + FR_DIR], eax
    shr edx, 8
    and edx, 0xFF
    mov [edi + FR_ACTION], edx
    inc dword [net_fw_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; network_fw_check: EAX=src_addr, EBX=dst_addr, ECX=proto, EDX=dir
; Rueckgabe: EAX=FW_ACTION_* (NPSPEC-NETWORK-FIREWALL-0001)
; ---------------------------------------------------------------------------
network_fw_check:
    push esi
    push ecx
    mov esi, net_fw_rules
    push edx
    mov edx, [net_fw_count]
.scan:
    test edx, edx
    jz .default_accept
    ; Richtung pruefen
    mov ecx, [esp]          ; gespeicherter original-EDX (dir)
    cmp [esi + FR_DIR], ecx
    jne .next
    ; Protokoll
    cmp dword [esi + FR_PROTO], 0
    je .match_addr          ; 0 = alle Protokolle
    cmp [esi + FR_PROTO], cx
    jne .next
.match_addr:
    ; Quell-Adresse
    mov ecx, [esi + FR_SRC_MASK]
    and ecx, eax
    cmp ecx, [esi + FR_SRC_ADDR]
    jne .next
    ; Ziel-Adresse
    mov ecx, [esi + FR_DST_MASK]
    and ecx, ebx
    cmp ecx, [esi + FR_DST_ADDR]
    jne .next
    ; Treffer
    mov eax, [esi + FR_ACTION]
    pop edx
    pop ecx
    pop esi
    ret
.next:
    add esi, FW_RULE_SIZE
    dec edx
    jmp .scan
.default_accept:
    pop edx
    pop ecx
    pop esi
    mov eax, FW_ACTION_ACCEPT
    ret

; ---------------------------------------------------------------------------
; network_flow_track: EAX=src, EBX=dst, ECX=src_port|dst_port, EDX=proto
; Verfolgt Verbindung in Flow-Tabelle (NPSPEC-NETWORK-FLOW-0001).
; CF=0 Erfolg
; ---------------------------------------------------------------------------
network_flow_track:
    push edi
    ; Existierenden Flow suchen
    mov edi, net_flows
    push ecx
    mov ecx, [net_flow_count]
.scan:
    test ecx, ecx
    jz .new_flow
    cmp [edi + FL_SRC_ADDR], eax
    jne .next
    cmp [edi + FL_DST_ADDR], ebx
    jne .next
    ; Paket-Zaehler erhoehen
    inc dword [edi + FL_PACKETS]
    pop ecx
    pop edi
    clc
    ret
.next:
    add edi, FLOW_SIZE
    dec ecx
    jmp .scan
.new_flow:
    pop ecx
    cmp dword [net_flow_count], FLOW_CAPACITY
    jae .overflow
    ; Neuen Flow anlegen
    mov edi, net_flows
    push ecx
    mov ecx, [net_flow_count]
    imul ecx, FLOW_SIZE
    add edi, ecx
    pop ecx
    mov [edi + FL_SRC_ADDR], eax
    mov [edi + FL_DST_ADDR], ebx
    ; Ports aus ECX
    mov [edi + FL_SRC_PORT], cx
    shr ecx, 16
    mov [edi + FL_DST_PORT], cx
    mov [edi + FL_PROTO], dl
    mov dword [edi + FL_BYTES], 0
    mov dword [edi + FL_PACKETS], 1
    mov eax, [timer_ticks]
    mov [edi + FL_LAST_TICK], eax
    mov dword [edi + FL_QOS], NET_QOS_BEST_EFFORT
    inc dword [net_flow_count]
    pop edi
    clc
    ret
.overflow:
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; network_dns_resolve: EAX=hostname_hash -> EBX=ipv4_addr (CF=0) oder CF=1
; Stub: schaut nur im Cache nach (NPSPEC-NETWORK-DNS-0001).
; ---------------------------------------------------------------------------
network_dns_resolve:
    push esi
    push ecx
    mov esi, net_dns_cache
    mov ecx, DNS_CACHE_CAPACITY
.scan:
    test ecx, ecx
    jz .miss
    cmp [esi + DC_HASH], eax
    je .hit
    add esi, DNS_CACHE_SIZE
    dec ecx
    jmp .scan
.hit:
    test dword [esi + DC_FLAGS], 1   ; valid?
    jz .miss
    mov ebx, [esi + DC_ADDR]
    pop ecx
    pop esi
    clc
    ret
.miss:
    pop ecx
    pop esi
    stc
    ret

; ---------------------------------------------------------------------------
; network_dns_cache_insert: EAX=hash, EBX=ipv4_addr, ECX=ttl
; ---------------------------------------------------------------------------
network_dns_cache_insert:
    cmp dword [net_dns_count], DNS_CACHE_CAPACITY
    jae .full
    push edi
    mov edi, net_dns_cache
    push ecx
    mov ecx, [net_dns_count]
    imul ecx, DNS_CACHE_SIZE
    add edi, ecx
    pop ecx
    mov [edi + DC_HASH], eax
    mov [edi + DC_ADDR], ebx
    mov [edi + DC_TTL], ecx
    mov dword [edi + DC_FLAGS], 1    ; valid
    inc dword [net_dns_count]
    pop edi
    clc
    ret
.full:
    stc
    ret

; ---------------------------------------------------------------------------
; network_ns_create_default
; Legt Standard-Netzwerk-Namespace (ID=1) an (NPSPEC-NETWORK-NAMESPACE-0001).
; ---------------------------------------------------------------------------
network_ns_create_default:
    mov edi, net_namespaces
    mov dword [edi + NN_ID], 1
    mov dword [edi + NN_FLAGS], 0
    mov dword [edi + NN_IFACE_MASK], 0xFFFFFFFF  ; alle Interfaces
    inc dword [net_ns_count]
    ret

; ---------------------------------------------------------------------------
; network_qos_classify: EAX=proto, EBX=dst_port -> EAX=NET_QOS_*
; (NPSPEC-NETWORK-QOS-0001)
; ---------------------------------------------------------------------------
network_qos_classify:
    ; Port 22 (SSH), 443 (HTTPS) -> INTERACTIVE
    cmp ebx, 22
    je .interactive
    cmp ebx, 443
    je .interactive
    ; QUIC (UDP/443) -> INTERACTIVE
    cmp eax, NET_PROTO_UDP
    jne .check_tcp
    cmp ebx, 443
    je .interactive
.check_tcp:
    cmp eax, NET_PROTO_TCP
    jne .bulk
    ; Port 80 -> INTERACTIVE
    cmp ebx, 80
    je .interactive
    ; Port > 1023 -> BULK
    cmp ebx, 1023
    ja .bulk
.best_effort:
    mov eax, NET_QOS_BEST_EFFORT
    ret
.interactive:
    mov eax, NET_QOS_INTERACTIVE
    ret
.bulk:
    mov eax, NET_QOS_BULK
    ret

; ---------------------------------------------------------------------------
; network_stack_self_test
; ---------------------------------------------------------------------------
network_stack_self_test:
    push esi
    xor esi, esi

    cmp dword [net_stack_ready], 1
    je .t2
    inc esi
.t2:
    ; Loopback-Interface vorhanden?
    cmp dword [net_if_count], 0
    jne .t3
    inc esi
.t3:
    ; Loopback-Route testen: 127.0.0.1 -> iface 1
    mov eax, 0x0100007F        ; 127.0.0.1
    call network_route_lookup
    jnc .t4
    inc esi
.t4:
    cmp ebx, 1                 ; Interface 1 = loopback
    je .t5
    inc esi
.t5:
    ; Firewall: Default-Akzeptanz
    mov eax, 0x0100007F
    mov ebx, 0x0100007F
    mov ecx, NET_PROTO_TCP
    mov edx, FW_DIR_IN
    call network_fw_check
    cmp eax, FW_ACTION_ACCEPT
    je .t6
    inc esi
.t6:
    ; QoS: Port 443 -> INTERACTIVE
    mov eax, NET_PROTO_TCP
    mov ebx, 443
    call network_qos_classify
    cmp eax, NET_QOS_INTERACTIVE
    je .t7
    inc esi
.t7:
    ; DNS-Cache: Insert + Lookup
    mov eax, 0xDEAD1234        ; Hostname-Hash
    mov ebx, 0x01020304        ; 1.2.3.4
    mov ecx, 300
    call network_dns_cache_insert
    mov eax, 0xDEAD1234
    call network_dns_resolve
    jnc .done
    inc esi

.done:
    test esi, esi
    jz .ok
    pop esi
    stc
    ret
.ok:
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
net_stack_ready:   dd 0
net_if_count:      dd 0
net_route_count:   dd 0
net_fw_count:      dd 0
net_flow_count:    dd 0
net_ns_count:      dd 0
net_dns_count:     dd 0
net_cong_algo:     dd CONG_CUBIC
net_total_tx:      dd 0
net_total_rx:      dd 0

align 4
net_interfaces:
    times NETIF_CAPACITY * NETIF_SIZE db 0

align 4
net_routes:
    times ROUTE_CAPACITY * ROUTE_SIZE db 0

align 4
net_fw_rules:
    times FW_RULE_CAPACITY * FW_RULE_SIZE db 0

align 4
net_flows:
    times FLOW_CAPACITY * FLOW_SIZE db 0

align 4
net_namespaces:
    times NET_NS_CAPACITY * NET_NS_SIZE db 0

align 4
net_dns_cache:
    times DNS_CACHE_CAPACITY * DNS_CACHE_SIZE db 0
