/* NovaOS C-Kernel - Nova Swap (NPSPEC-MEMORY-SWAP-0001). Optionaler
   Backing-Mechanismus: `swap_out()` sichert den Inhalt eines auslagerbaren
   physischen Frames in einen Swap-Slot und gibt den Frame erst NACH
   erfolgreicher Sicherung frei (Anforderung #5); `swap_in()` stellt den
   Inhalt in einen frisch allokierten Frame zurueck (fuer die Integration
   in den Demand-Paging-Pfad, Anforderung #7, siehe demandpage.h).

   BEWUSSTE SCOPE-REDUKTION: es gibt noch KEINEN echten Block-Swap-
   Provider (keine Platten-I/O-Anbindung aus dieser Speicherschicht
   heraus) - der "Swap-Speicher" ist ein fester In-Memory-Puffer
   (`SWAP_SLOT_CAPACITY` Slots je `PMM_PAGE_SIZE`). Das ist funktional
   identisch zum NPSPEC-Modell (ein Frame wird gesichert, freigegeben,
   spaeter wiederhergestellt) - erfuellt also die Mechanik, NICHT aber
   "echten" persistenten/externen Speicher. Persistenter, verschluesselter
   Swap (Anforderung #8) ist dadurch nicht erreichbar. Pinning
   (Anforderungen #3/#4) wird ueber eine explizite `swap_pin()`/
   `swap_unpin()`-Markierung durchgesetzt. */
#ifndef NOVA_SWAP_H
#define NOVA_SWAP_H

#include <stdint.h>

#define SWAP_SLOT_CAPACITY 16u
#define SWAP_PIN_CAPACITY  16u

void swap_initialize(void);
int swap_self_test(void); /* 0 = ok */

/* Markiert page als NICHT auslagerbar / hebt die Markierung auf
   (Anforderungen #3/#4). 0 = Erfolg, -1 = SWAP_PIN_CAPACITY erschoepft
   (nur bei swap_pin). */
int swap_pin(uint32_t page);
int swap_unpin(uint32_t page);
int swap_is_pinned(uint32_t page);

/* Sichert den Inhalt von page in einen freien Slot und gibt page per
   frame_unref() frei - ERST nach erfolgreicher Sicherung (Anforderung
   #5). 0 = Erfolg (*out_slot gesetzt), -1 = page ist gepinnt oder kein
   freier Slot vorhanden (page bleibt in diesem Fall unveraendert). */
int swap_out(uint32_t page, uint32_t *out_slot);

/* Stellt den Inhalt von slot in einen frisch allokierten Frame zurueck
   und gibt den Slot frei. 0 = Erfolg (*out_page gesetzt), -1 = ungueltiger/
   leerer Slot oder PMM erschoepft. */
int swap_in(uint32_t slot, uint32_t *out_page);

uint32_t swap_used_slots(void);
uint32_t swap_total_slots(void);

#endif
