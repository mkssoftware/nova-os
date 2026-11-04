/* NovaOS C-Kernel - IPC Publish/Subscribe (NPSPEC-IPC-PUBSUB-0001, Phase 4
   des IPC-Neuentwurfs, AUFBAU AUF dem Phase-1-Endpoint-Modell nur insofern,
   als dieselben Grundprinzipien - getrennte Rechte, typisierte Nachrichten,
   begrenzte Warteschlangen - wiederverwendet werden; Topics/Subscriptions
   sind eine EIGENE Tabelle, kein Aufbau auf ipc_endpoint_t). */
#ifndef NOVA_PUBSUB_H
#define NOVA_PUBSUB_H

#include <stdint.h>
#include "ipc.h"

#define IPC_TOPIC_CAPACITY            8u
#define IPC_TOPIC_ALLOWED_PUBLISHERS  4u
#define IPC_TOPIC_ALLOWED_SUBSCRIBERS 4u
#define IPC_SUBSCRIPTION_CAPACITY     16u
#define IPC_SUBSCRIPTION_QUEUE_CAPACITY 8u

#define IPC_TOPIC_STATE_ACTIVE 1u
#define IPC_TOPIC_STATE_CLOSED 2u

#define IPC_SUB_STATE_ACTIVE 1u
#define IPC_SUB_STATE_PAUSED 2u
#define IPC_SUB_STATE_CLOSED 3u

#define IPC_PUBSUB_ERR_NOT_FOUND     (-1)
#define IPC_PUBSUB_ERR_DENIED        (-2)
#define IPC_PUBSUB_ERR_CAPACITY      (-3)
#define IPC_PUBSUB_ERR_CLOSED        (-4)
#define IPC_PUBSUB_ERR_INCOMPATIBLE  (-5)
#define IPC_PUBSUB_ERR_EMPTY         (-6)
#define IPC_PUBSUB_ERR_INVALID_STATE (-7)

typedef struct {
    uint32_t topic_id;
    uint32_t creator_pid;
    uint32_t namespace_id;
    uint32_t interface_id;
    uint32_t version;
    uint32_t allow_multi_publish;
    uint32_t state;
    uint32_t subscriber_count; /* aktuell lebende (nicht CLOSED) Subscriptions */
    uint32_t published_total;
    uint32_t delivered_total; /* Summe ueber alle Subscriptions erfolgreich eingereiht */
    uint32_t dropped_total;   /* Summe ueber alle Subscriptions wegen voller Queue verworfen */
    uint32_t denied_total;
} ipc_topic_info_t;

typedef struct {
    uint32_t subscription_id;
    uint32_t topic_id;
    uint32_t subscriber_pid;
    uint32_t state;
    uint32_t queue_depth;
    uint32_t received_total;
    uint32_t dropped_total;
} ipc_subscription_info_t;

void pubsub_manager_initialize(void);
int pubsub_manager_self_test(void); /* 0 = ok */

/* Nur creator_pid darf spaeter grant/close. allow_multi_publish==0 bindet
   das Topic policy-seitig an GENAU EINEN Publisher (den Creator) - jeder
   Versuch, per topic_grant_publish() einen weiteren Publisher einzutragen,
   wird dann abgewiesen (#6 "Mehrere Publisher...sofern Policy dies erlaubt").
   0 = Erfolg (*out_topic_id gesetzt), IPC_PUBSUB_ERR_CAPACITY wenn die
   Topic-Tabelle voll ist. */
int topic_create(uint32_t creator_pid, uint32_t namespace_id, uint32_t interface_id,
                  uint32_t version, uint32_t allow_multi_publish, uint32_t *out_topic_id);

/* Nur der Creator darf schliessen. Idempotent. Bereits bestehende
   Subscriptions bleiben erhalten (koennen weiter drainen), nur neues
   Publishing wird abgewiesen. */
int topic_close(uint32_t pid, uint32_t topic_id);

/* Trennung Publish-/Subscribe-Rechte (#4/#5): topic_grant_publish() gewaehrt
   AUSSCHLIESSLICH Publish-Recht, niemals Subscribe-Recht, und umgekehrt bei
   topic_grant_subscribe(). Nur der Creator darf gewaehren. 0 = Erfolg,
   IPC_PUBSUB_ERR_DENIED wenn allow_multi_publish==0 (nur fuer _publish) oder
   Aufrufer nicht Creator, IPC_PUBSUB_ERR_CAPACITY wenn Allowlist voll. */
int topic_grant_publish(uint32_t creator_pid, uint32_t topic_id, uint32_t publisher_pid);
int topic_grant_subscribe(uint32_t creator_pid, uint32_t topic_id, uint32_t subscriber_pid);

/* publisher_pid MUSS Creator oder per topic_grant_publish() autorisiert
   sein. interface_id/version MUESSEN exakt zum Topic-Vertrag passen (#3
   Typed Pub/Sub) - sonst INCOMPATIBLE. Liefert an jede ACTIVE Subscription
   des Topics best-effort zu (PAUSED wird uebersprungen, volle Queue zaehlt
   dropped_total hoch - #9 Backpressure, kein stilles Blockieren). Publish-
   Erfolg bedeutet NICHT, dass alle Subscriber die Nachricht erhalten haben
   (#7 "Publish != Delivery"). 0 = Erfolg (auch bei 0 Subscribern). */
int topic_publish(uint32_t publisher_pid, uint32_t topic_id, uint32_t interface_id,
                   uint32_t version, const ipc_message_t *msg);

/* subscriber_pid MUSS Creator oder per topic_grant_subscribe() autorisiert
   sein. Erzeugt eine neue, eigene, begrenzte Warteschlange (#9). 0 = Erfolg
   (*out_subscription_id gesetzt). */
int topic_subscribe(uint32_t subscriber_pid, uint32_t topic_id, uint32_t *out_subscription_id);

/* Nur der Subscriber selbst darf pausieren/fortsetzen/schliessen (#Lifecycle
   Create->Active->Pause->Close). PAUSED erhaelt keine neuen Nachrichten
   (kein Drop-Zaehler - bewusste Pause, kein Verlust durch Ueberlastung).
   Idempotent bei gleichem Zielzustand. */
int subscription_pause(uint32_t pid, uint32_t subscription_id);
int subscription_resume(uint32_t pid, uint32_t subscription_id);
int subscription_close(uint32_t pid, uint32_t subscription_id);

/* 0 = Erfolg (*out_msg gefuellt), IPC_PUBSUB_ERR_EMPTY wenn nichts wartet,
   IPC_PUBSUB_ERR_CLOSED wenn die Subscription bereits geschlossen ist. */
int subscription_receive(uint32_t pid, uint32_t subscription_id, ipc_message_t *out_msg);

/* Reine Metadaten, NIE Nachrichteninhalte (#Introspection Offenlegungsverbot). */
int topic_info(uint32_t topic_id, ipc_topic_info_t *out_info);
int subscription_info(uint32_t subscription_id, ipc_subscription_info_t *out_info);

#endif
