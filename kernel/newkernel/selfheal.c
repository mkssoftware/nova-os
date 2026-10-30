/* NovaOS C-Kernel - Nova Autonomous Self-Healing (siehe selfheal.h fuer
   Scope/Anpassungs-Hinweis). */
#include "selfheal.h"
#include "integration.h"
#include "resilience.h"

static selfheal_record_t g_heal_table[HEAL_CAPACITY];
static uint32_t g_heal_total_repairs = 0;

void selfheal_initialize(void) {
    for (uint32_t i = 0; i < HEAL_CAPACITY; ++i) {
        g_heal_table[i].id = 0;
        g_heal_table[i].subsystem_id = 0;
        g_heal_table[i].state = HEAL_STATE_OK;
        g_heal_table[i].attempts = 0;
    }
    g_heal_total_repairs = 0;
}

int selfheal_register(uint32_t subsystem_id, uint32_t *out_slot) {
    for (uint32_t i = 0; i < HEAL_CAPACITY; ++i) {
        if (g_heal_table[i].id != 0) continue;
        g_heal_table[i].subsystem_id = subsystem_id;
        g_heal_table[i].state = HEAL_STATE_OK;
        g_heal_table[i].attempts = 0;
        g_heal_table[i].id = i + 1; /* 1-basiert, Slot 0 -> ID 1, nie 0 */
        *out_slot = i;
        return 0;
    }
    return -1;
}

int selfheal_repair(uint32_t slot, uint32_t fault_type) {
    if (slot >= HEAL_CAPACITY) return -1;
    selfheal_record_t *rec = &g_heal_table[slot];
    if (rec->id == 0) return -1;

    rec->state = HEAL_STATE_HEALING;
    rec->attempts++;
    g_heal_total_repairs++;

    resilience_fault_pipeline(fault_type, rec->subsystem_id, 0);

    int isolated = 0;
    uint32_t cont_slot;
    if (containment_find_by_subsystem(rec->subsystem_id, &cont_slot) == 0) {
        isolated = (containment_check(cont_slot) != 0);
    }
    if (isolated) {
        rec->state = HEAL_STATE_FAILED;
        return -1;
    }
    rec->state = HEAL_STATE_OK;
    return 0;
}

uint32_t selfheal_total_repairs(void) { return g_heal_total_repairs; }

int selfheal_record_at(uint32_t slot, const selfheal_record_t **out) {
    if (slot >= HEAL_CAPACITY || g_heal_table[slot].id == 0) return -1;
    *out = &g_heal_table[slot];
    return 0;
}

/* Eigene, private Test-Subsystem-ID statt INTRO_SYS_KERNEL (=1): genau
   dieser Wert wird bereits von containment_self_test() (resilience.c)
   als Subsystem registriert UND dort absichtlich bis zur Isolation
   hochgezaehlt, OHNE am Ende zurueckgesetzt zu werden - containment_
   self_test() laeuft frueh in kmain.c (vor selfheal), daher waere
   INTRO_SYS_KERNEL's Containment-Zustand hier bereits ISOLATED (real im
   Boot aufgetreten, FEHLGESCHLAGEN-Report gegengeprueft). Eine private
   ID vermeidet die Kollision mit diesem geteilten globalen Zustand. */
#define SELFHEAL_TEST_SUBSYS_ID 0xDEAD5E1Fu

/* ---- Selbsttest (Register+Anfangszustand aus dem alten ASM uebernommen,
   plus echter Repair-Pfad, Kapazitaetsgrenze und ungueltiger Slot - das
   alte selfheal_self_test pruefte NUR den Registrierungspfad, siehe
   selfheal.h) ---- */
int selfheal_self_test(void) {
    selfheal_initialize();

    uint32_t slot;
    if (selfheal_register(SELFHEAL_TEST_SUBSYS_ID, &slot) != 0) return -1;
    const selfheal_record_t *rec;
    if (selfheal_record_at(slot, &rec) != 0) return -1;
    if (rec->state != HEAL_STATE_OK) return -1;
    if (rec->subsystem_id != SELFHEAL_TEST_SUBSYS_ID) return -1;

    /* SELFHEAL_TEST_SUBSYS_ID ist nicht per containment_register()
       angemeldet - der Reparaturversuch muss deshalb erfolgreich sein
       (siehe selfheal.h Anpassungs-Hinweis). */
    if (selfheal_repair(slot, FD_TYPE_SUBSYS) != 0) return -1;
    if (selfheal_record_at(slot, &rec) != 0) return -1;
    if (rec->state != HEAL_STATE_OK) return -1;
    if (rec->attempts != 1) return -1;
    if (selfheal_total_repairs() != 1) return -1;

    /* Ungueltiger/leerer Slot -> Fehler. */
    if (selfheal_repair(HEAL_CAPACITY, FD_TYPE_SUBSYS) == 0) return -1;
    if (selfheal_repair(slot + 1u, FD_TYPE_SUBSYS) == 0) return -1;

    /* Kapazitaet: der bereits belegte Slot zaehlt mit, HEAL_CAPACITY-1
       weitere Registrierungen fuellen die Tabelle vollstaendig. */
    for (uint32_t i = 0; i < HEAL_CAPACITY - 1u; ++i) {
        uint32_t new_slot;
        if (selfheal_register(100u + i, &new_slot) != 0) return -1;
    }
    uint32_t overflow_slot;
    if (selfheal_register(999u, &overflow_slot) == 0) return -1;

    return 0;
}
