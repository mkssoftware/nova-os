/* NovaOS C-Kernel - IPC Event Bus (NPSPEC-IPC-EVENTBUS-0001, Phase 5 des
   IPC-Neuentwurfs, ECHTER AUFBAU AUF Phase 4/pubsub.h - jeder EventType IST
   intern ein Pub/Sub-Topic, kein Parallel-Zustand fuer Autorisierung/Queue/
   Backpressure/lazy Cleanup, siehe pubsub.h. NICHT zu verwechseln mit
   evbus.c/.h (das ist §101/NPSPEC-KERNEL-0101, ein synchroner Kernel-
   interner Callback-Bus fuer Kernelkomponenten ohne Prozess-Capabilities -
   ein voellig anderer Scope als dieser hier, der zwischen USERSPACE-
   Prozessen mit echter PID-basierter Autorisierung vermittelt). */
#ifndef NOVA_EVENTBUS_H
#define NOVA_EVENTBUS_H

#include <stdint.h>
#include "ipc.h"

#define EVENTBUS_SUBSCRIPTION_CAPACITY 16u

#define EVENTBUS_ERR_NOT_FOUND     (-1)
#define EVENTBUS_ERR_DENIED       (-2)
#define EVENTBUS_ERR_CAPACITY     (-3)
#define EVENTBUS_ERR_CLOSED       (-4)
#define EVENTBUS_ERR_INCOMPATIBLE (-5)
#define EVENTBUS_ERR_EMPTY        (-6)
#define EVENTBUS_ERR_INVALID_STATE (-7)

/* Event-Payload bleibt wie im Rest des IPC-Neuentwurfs bewusst klein: ein
   einzelnes 32-Bit-Wort statt der vollen ipc_message_t, weil die anderen
   drei Woerter der zugrundeliegenden Nachricht fuer EventID/Source/
   Timestamp verbraucht werden (#Event-Modell). */
typedef struct {
    uint32_t event_id;
    uint32_t event_type;  /* == interface_id des zugrundeliegenden Topics */
    uint32_t source_pid;
    uint32_t timestamp;   /* timer_get_ticks() beim Publish */
    uint32_t payload;
} eventbus_event_t;

typedef struct {
    uint32_t subscription_id;
    uint32_t topic_id;
    uint32_t subscriber_pid;
    uint32_t source_filter; /* 0 = jede Quelle */
    uint32_t state;         /* IPC_SUB_STATE_* aus pubsub.h */
    uint32_t queue_depth;
    uint32_t received_total;
    uint32_t filtered_total; /* durch source_filter verborgen, kein Verlust */
} eventbus_subscription_info_t;

void eventbus_manager_initialize(void);
int eventbus_manager_self_test(void); /* 0 = ok */

/* Duenner Wrapper um topic_create(): registriert event_type als eigenes
   Pub/Sub-Topic (#Event Types "muessen typisierbar und versionierbar
   sein" - version ist hier immer 1, da dieser Kernel noch kein Schema-
   Versionsschema jenseits roher IDs hat). allow_multi_publish wie bei
   Topics. 0 = Erfolg (*out_topic_id gesetzt - dient als Handle fuer alle
   weiteren Aufrufe). */
int eventbus_register_type(uint32_t creator_pid, uint32_t event_type,
                            uint32_t allow_multi_publish, uint32_t *out_topic_id);

/* Duenne Wrapper um topic_grant_publish()/topic_grant_subscribe() - Trennung
   der Rechte bleibt vollstaendig durch pubsub.c garantiert (#6 Capability-
   Modell: Subscribe-Capability darf keine Publish-Rechte erzeugen). */
int eventbus_grant_publish(uint32_t creator_pid, uint32_t topic_id, uint32_t publisher_pid);
int eventbus_grant_subscribe(uint32_t creator_pid, uint32_t topic_id, uint32_t subscriber_pid);

/* Erzeugt EventID (fortlaufend) und Timestamp automatisch, verpackt beides
   plus source_pid in die zugrundeliegende ipc_message_t und liefert sie
   per topic_publish() aus - erbt damit Autorisierung, Typed-Check,
   Backpressure und lazy Subscriber-Cleanup vollstaendig von Phase 4.
   0 = Erfolg (*out_event_id gesetzt). */
int eventbus_publish(uint32_t publisher_pid, uint32_t topic_id, uint32_t payload,
                      uint32_t *out_event_id);

/* source_filter: 0 = alle Quellen, sonst nur Events von genau dieser PID
   (#Subscription "kann filtern nach Source") - eine ECHTE EventBus-
   Erweiterung ueber das reine Topic-Modell hinaus, da pubsub.c selbst
   keine Pro-Subscription-Filterung kennt. */
int eventbus_subscribe(uint32_t subscriber_pid, uint32_t topic_id, uint32_t source_filter,
                        uint32_t *out_subscription_id);

/* Liest das naechste Event, das den source_filter der Subscription erfuellt
   - nicht passende Events werden intern uebersprungen (filtered_total
   zaehlt hoch, kein Drop/Verlust-Zaehler, da bewusst ausgefiltert statt
   durch Ueberlastung verworfen). 0 = Erfolg (*out_event gefuellt),
   EVENTBUS_ERR_EMPTY wenn nichts (mehr) passendes wartet,
   EVENTBUS_ERR_CLOSED wenn die Subscription bereits geschlossen ist. */
int eventbus_receive(uint32_t pid, uint32_t subscription_id, eventbus_event_t *out_event);

int eventbus_unsubscribe(uint32_t pid, uint32_t subscription_id);

/* Reine Metadaten, NIE Event-Payload (#Introspection Offenlegungsverbot). */
int eventbus_subscription_info(uint32_t subscription_id, eventbus_subscription_info_t *out_info);

#endif
