/* NovaOS C-Kernel - Network Stack (Port aus network32.inc,
   NPSPEC-NETWORK-STACK-0001 und verwandte NPSPEC-NETWORK-Spezifikationen). */
#ifndef NOVA_NETWORK_H
#define NOVA_NETWORK_H

#include <stdint.h>

#define NETIF_TYPE_NONE     0u
#define NETIF_TYPE_ETHERNET 1u
#define NETIF_TYPE_LOOPBACK 2u
#define NETIF_TYPE_WIFI     3u
#define NETIF_TYPE_VIRTUAL  4u
#define NETIF_TYPE_VPN      5u
#define NETIF_TYPE_TUNNEL   6u

#define NETIF_FLAG_UP        0x01u
#define NETIF_FLAG_RUNNING   0x02u
#define NETIF_FLAG_PROMISC   0x04u
#define NETIF_FLAG_MULTICAST 0x08u
#define NETIF_FLAG_IPV4      0x10u
#define NETIF_FLAG_IPV6      0x20u
#define NETIF_FLAG_DHCP      0x40u
#define NETIF_FLAG_SLAAC     0x80u

#define NET_PROTO_NONE   0u
#define NET_PROTO_ICMP   1u
#define NET_PROTO_TCP    6u
#define NET_PROTO_UDP    17u
#define NET_PROTO_QUIC   0x51u
#define NET_PROTO_ICMPV6 58u

#define FW_ACTION_ACCEPT 0u
#define FW_ACTION_DROP   1u
#define FW_ACTION_REJECT 2u
#define FW_ACTION_LOG    3u

#define FW_DIR_IN  0u
#define FW_DIR_OUT 1u

#define NET_QOS_REALTIME    0u
#define NET_QOS_INTERACTIVE 1u
#define NET_QOS_BULK        2u
#define NET_QOS_BACKGROUND  3u
#define NET_QOS_BEST_EFFORT 4u

#define CONG_RENO  0u
#define CONG_CUBIC 1u
#define CONG_BBR   2u

#define NETIF_CAPACITY     4u
#define ROUTE_CAPACITY     8u
#define FW_RULE_CAPACITY   8u
#define FLOW_CAPACITY      16u
#define NET_NS_CAPACITY    4u
#define DNS_CACHE_CAPACITY 8u

/* Registriert ein Loopback-Interface (127.0.0.1/8), eine Loopback-Route und
   den Default-Netzwerk-Namespace. */
void network_stack_initialize(void);
int network_stack_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll (kein freier Slot mit ID=0). */
int network_if_register(uint32_t id, uint32_t type, uint32_t flags, uint32_t ipv4_addr);

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int network_route_add(uint32_t dest, uint32_t mask, uint32_t gw, uint32_t iface_id);

/* 0 = Erfolg (out_iface_id/out_gw gesetzt), -1 wenn keine Route passt.
   Vereinfachtes Longest-Prefix-Match: erste passende Route. */
int network_route_lookup(uint32_t dst_addr, uint32_t *out_iface_id, uint32_t *out_gw);

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int network_fw_add_rule(uint32_t src, uint32_t dst, uint16_t proto, uint16_t port,
                         uint32_t dir, uint32_t action);

/* Rueckgabe: FW_ACTION_* (Default: ACCEPT, wenn keine Regel passt). */
uint32_t network_fw_check(uint32_t src_addr, uint32_t dst_addr, uint32_t proto, uint32_t dir);

/* 0 = Erfolg, -1 wenn Tabelle voll (bei neuem Flow). */
int network_flow_track(uint32_t src, uint32_t dst, uint16_t src_port, uint16_t dst_port, uint8_t proto);

/* 0 = Erfolg (out_addr gesetzt), -1 wenn nicht im Cache / ungueltig. */
int network_dns_resolve(uint32_t hostname_hash, uint32_t *out_addr);

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int network_dns_cache_insert(uint32_t hash, uint32_t ipv4_addr, uint32_t ttl);

/* Rueckgabe: NET_QOS_*. */
uint32_t network_qos_classify(uint32_t proto, uint32_t dst_port);

#endif
