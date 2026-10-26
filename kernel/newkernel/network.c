/* NovaOS C-Kernel - Network Stack (Port aus network32.inc). */
#include "network.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t flags;
    uint32_t mtu;
    uint32_t ipv4_addr;
    uint32_t ipv4_mask;
    uint32_t ipv4_gw;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
    uint32_t tx_packets;
    uint32_t rx_packets;
    uint32_t ns_id;
    uint32_t qos_default;
} net_interface_t;

typedef struct {
    uint32_t dest;
    uint32_t mask;
    uint32_t gw;
    uint32_t iface;
    uint32_t metric;
    uint32_t flags;
    uint32_t multipath;
} net_route_t;

typedef struct {
    uint32_t priority;
    uint32_t src_addr;
    uint32_t src_mask;
    uint32_t dst_addr;
    uint32_t dst_mask;
    uint16_t proto;
    uint16_t port;
    uint32_t dir;
    uint32_t action;
} net_fw_rule_t;

typedef struct {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t proto;
    uint8_t state;
    uint8_t iface;
    uint8_t reserved;
    uint32_t bytes;
    uint32_t packets;
    uint32_t last_tick;
    uint32_t qos;
} net_flow_t;

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t iface_mask;
} net_namespace_t;

typedef struct {
    uint32_t hash;
    uint32_t addr;
    uint32_t ttl;
    uint32_t flags; /* 0x1=valid, 0x2=negative */
} dns_cache_entry_t;

static net_interface_t net_interfaces[NETIF_CAPACITY];
static net_route_t net_routes[ROUTE_CAPACITY];
static net_fw_rule_t net_fw_rules[FW_RULE_CAPACITY];
static net_flow_t net_flows[FLOW_CAPACITY];
static net_namespace_t net_namespaces[NET_NS_CAPACITY];
static dns_cache_entry_t net_dns_cache[DNS_CACHE_CAPACITY];

static uint32_t net_if_count = 0;
static uint32_t net_route_count = 0;
static uint32_t net_fw_count = 0;
static uint32_t net_flow_count = 0;
static uint32_t net_ns_count = 0;
static uint32_t net_dns_count = 0;
static uint32_t net_cong_algo = CONG_CUBIC;
static int net_stack_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

int network_if_register(uint32_t id, uint32_t type, uint32_t flags, uint32_t ipv4_addr) {
    if (net_if_count >= NETIF_CAPACITY) {
        return -1;
    }
    net_interface_t *slot = 0;
    for (uint32_t i = 0; i < NETIF_CAPACITY; ++i) {
        if (net_interfaces[i].id == 0) {
            slot = &net_interfaces[i];
            break;
        }
    }
    if (!slot) {
        return -1;
    }

    slot->id = id;
    slot->type = type;
    slot->flags = flags;
    slot->mtu = (type == NETIF_TYPE_LOOPBACK) ? 65536u : 1500u;
    slot->ipv4_addr = ipv4_addr;
    slot->ipv4_mask = 0x00FFFFFFu; /* /24 Default */
    slot->ipv4_gw = 0;
    slot->tx_bytes = 0;
    slot->rx_bytes = 0;
    slot->tx_packets = 0;
    slot->rx_packets = 0;
    slot->ns_id = 1; /* Default-Namespace */
    slot->qos_default = NET_QOS_BEST_EFFORT;

    ++net_if_count;
    return 0;
}

int network_route_add(uint32_t dest, uint32_t mask, uint32_t gw, uint32_t iface_id) {
    if (net_route_count >= ROUTE_CAPACITY) {
        return -1;
    }
    net_route_t *route = &net_routes[net_route_count];
    route->dest = dest;
    route->mask = mask;
    route->gw = gw;
    route->iface = iface_id;
    route->metric = 100;
    route->flags = 0x2; /* gateway */
    route->multipath = 1;
    ++net_route_count;
    return 0;
}

int network_route_lookup(uint32_t dst_addr, uint32_t *out_iface_id, uint32_t *out_gw) {
    for (uint32_t i = 0; i < net_route_count; ++i) {
        net_route_t *route = &net_routes[i];
        if ((route->mask & dst_addr) == route->dest) {
            *out_iface_id = route->iface;
            *out_gw = route->gw;
            return 0;
        }
    }
    return -1;
}

static void network_ns_create_default(void) {
    net_namespace_t *ns = &net_namespaces[0];
    ns->id = 1;
    ns->flags = 0;
    ns->iface_mask = 0xFFFFFFFFu; /* alle Interfaces */
    ++net_ns_count;
}

void network_stack_initialize(void) {
    for (uint32_t i = 0; i < NETIF_CAPACITY; ++i) net_interfaces[i] = (net_interface_t){0};
    for (uint32_t i = 0; i < ROUTE_CAPACITY; ++i) net_routes[i] = (net_route_t){0};
    for (uint32_t i = 0; i < FW_RULE_CAPACITY; ++i) net_fw_rules[i] = (net_fw_rule_t){0};
    for (uint32_t i = 0; i < FLOW_CAPACITY; ++i) net_flows[i] = (net_flow_t){0};
    for (uint32_t i = 0; i < NET_NS_CAPACITY; ++i) net_namespaces[i] = (net_namespace_t){0};
    for (uint32_t i = 0; i < DNS_CACHE_CAPACITY; ++i) net_dns_cache[i] = (dns_cache_entry_t){0};

    net_if_count = 0;
    net_route_count = 0;
    net_fw_count = 0;
    net_flow_count = 0;
    net_ns_count = 0;
    net_dns_count = 0;
    net_cong_algo = CONG_CUBIC;

    /* Loopback-Interface registrieren (127.0.0.1/8). */
    network_if_register(1, NETIF_TYPE_LOOPBACK,
                         NETIF_FLAG_UP | NETIF_FLAG_RUNNING | NETIF_FLAG_IPV4,
                         0x0100007Fu); /* 127.0.0.1 LE */

    /* Loopback-Route (127.0.0.0/8 -> lo). */
    network_route_add(0x0000007Fu, 0x000000FFu, 0, 1);

    network_ns_create_default();

    net_stack_ready = 1;
}

int network_fw_add_rule(uint32_t src, uint32_t dst, uint16_t proto, uint16_t port,
                         uint32_t dir, uint32_t action) {
    if (net_fw_count >= FW_RULE_CAPACITY) {
        return -1;
    }
    net_fw_rule_t *rule = &net_fw_rules[net_fw_count];
    rule->priority = 0;
    rule->src_addr = src;
    rule->src_mask = 0xFFFFFFFFu;
    rule->dst_addr = dst;
    rule->dst_mask = 0xFFFFFFFFu;
    rule->proto = proto;
    rule->port = port;
    rule->dir = dir;
    rule->action = action;
    ++net_fw_count;
    return 0;
}

uint32_t network_fw_check(uint32_t src_addr, uint32_t dst_addr, uint32_t proto, uint32_t dir) {
    for (uint32_t i = 0; i < net_fw_count; ++i) {
        net_fw_rule_t *rule = &net_fw_rules[i];
        if (rule->dir != dir) continue;
        if (rule->proto != 0 && rule->proto != proto) continue;
        if ((rule->src_mask & src_addr) != rule->src_addr) continue;
        if ((rule->dst_mask & dst_addr) != rule->dst_addr) continue;
        return rule->action;
    }
    return FW_ACTION_ACCEPT;
}

int network_flow_track(uint32_t src, uint32_t dst, uint16_t src_port, uint16_t dst_port, uint8_t proto) {
    for (uint32_t i = 0; i < net_flow_count; ++i) {
        net_flow_t *flow = &net_flows[i];
        if (flow->src_addr == src && flow->dst_addr == dst) {
            ++flow->packets;
            return 0;
        }
    }
    if (net_flow_count >= FLOW_CAPACITY) {
        return -1;
    }
    net_flow_t *flow = &net_flows[net_flow_count];
    flow->src_addr = src;
    flow->dst_addr = dst;
    flow->src_port = src_port;
    flow->dst_port = dst_port;
    flow->proto = proto;
    flow->bytes = 0;
    flow->packets = 1;
    flow->last_tick = current_timer_ticks();
    flow->qos = NET_QOS_BEST_EFFORT;
    ++net_flow_count;
    return 0;
}

int network_dns_resolve(uint32_t hostname_hash, uint32_t *out_addr) {
    for (uint32_t i = 0; i < DNS_CACHE_CAPACITY; ++i) {
        dns_cache_entry_t *entry = &net_dns_cache[i];
        if (entry->hash == hostname_hash) {
            if ((entry->flags & 1u) == 0) {
                return -1; /* nicht valid */
            }
            *out_addr = entry->addr;
            return 0;
        }
    }
    return -1;
}

int network_dns_cache_insert(uint32_t hash, uint32_t ipv4_addr, uint32_t ttl) {
    if (net_dns_count >= DNS_CACHE_CAPACITY) {
        return -1;
    }
    dns_cache_entry_t *entry = &net_dns_cache[net_dns_count];
    entry->hash = hash;
    entry->addr = ipv4_addr;
    entry->ttl = ttl;
    entry->flags = 1; /* valid */
    ++net_dns_count;
    return 0;
}

uint32_t network_qos_classify(uint32_t proto, uint32_t dst_port) {
    if (dst_port == 22 || dst_port == 443) {
        return NET_QOS_INTERACTIVE;
    }
    if (proto == NET_PROTO_UDP && dst_port == 443) {
        return NET_QOS_INTERACTIVE; /* QUIC */
    }
    if (proto == NET_PROTO_TCP) {
        if (dst_port == 80) {
            return NET_QOS_INTERACTIVE;
        }
        if (dst_port > 1023) {
            return NET_QOS_BULK;
        }
    }
    return NET_QOS_BEST_EFFORT;
}

int network_stack_self_test(void) {
    if (net_stack_ready != 1) return -1;
    if (net_if_count == 0) return -1;

    uint32_t iface_id = 0, gw = 0;
    if (network_route_lookup(0x0100007Fu, &iface_id, &gw) != 0) return -1; /* 127.0.0.1 */
    if (iface_id != 1) return -1; /* Interface 1 = loopback */

    if (network_fw_check(0x0100007Fu, 0x0100007Fu, NET_PROTO_TCP, FW_DIR_IN) != FW_ACTION_ACCEPT) return -1;

    if (network_qos_classify(NET_PROTO_TCP, 443) != NET_QOS_INTERACTIVE) return -1;

    if (network_dns_cache_insert(0xDEAD1234u, 0x01020304u, 300) != 0) return -1;
    uint32_t addr = 0;
    if (network_dns_resolve(0xDEAD1234u, &addr) != 0) return -1;

    return 0;
}
