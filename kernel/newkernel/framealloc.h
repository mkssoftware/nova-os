/* NovaOS C-Kernel - Nova Physical Frame Allocator (NPSPEC-MEMORY-
   FRAMEALLOCATOR-0001). Spezialisierte Schicht ueber dem PMM (pmm.h):
   Referenzzaehlung fuer geteilte Frames (Grundlage fuer COW/Shared Memory,
   siehe cow.c/vas.h) und bestmoegliche Allokation zusammenhaengender
   Frame-Bereiche.

   Bewusste Scope-Reduktion: der PMM verwaltet Frames als reinen LIFO-Stack
   ohne Defragmentierung. "Contiguous"-Allokation ist deshalb best-effort -
   solange seit paging_initialize() kein Frame freigegeben wurde, liefert der
   PMM-Stack Frames in absteigender physischer Adresse zurueck (so wie sie
   beim Scannen der Speicherkarte eingetragen wurden); frame_alloc_contiguous
   nutzt genau das aus und gibt bei Fragmentierung ehrlich -1 zurueck, statt
   zu kompaktieren. */
#ifndef NOVA_FRAMEALLOC_H
#define NOVA_FRAMEALLOC_H

#include <stdint.h>

#define FRAMEALLOC_TRACKED_CAPACITY 64u
#define FRAMEALLOC_MAX_CONTIGUOUS 32u

void framealloc_initialize(void);
int framealloc_self_test(void); /* 0 = ok */

/* Einzelner Frame, Referenzzaehler startet bei 1 (ungetrackt, bis
   frame_ref() aufgerufen wird). 0 = PMM erschoepft. */
uint32_t frame_alloc(uint32_t *out_numa_node);

/* Bis zu FRAMEALLOC_MAX_CONTIGUOUS physisch zusammenhaengende Frames
   (aufsteigende Adresse). 0 = Erfolg (*out_base gesetzt), -1 = Pool
   erschoepft oder fragmentiert (keine Seiten wurden in diesem Fall
   einbehalten). */
int frame_alloc_contiguous(uint32_t count, uint32_t *out_base);

/* Erhoeht den Referenzzaehler eines (ggf. bereits von frame_alloc()
   gelieferten) Frames - ab hier wird er getrackt. 0 = Erfolg, -1 = Tabelle
   voll. */
int frame_ref(uint32_t page);

/* Verringert den Referenzzaehler; bei Erreichen von 0 wird der Frame an den
   PMM zurueckgegeben. Fuer ungetrackte Frames identisch zu einem direkten
   Free. 0 = Erfolg, -1 = ungueltiger/unbekannter Frame. */
int frame_unref(uint32_t page);

uint32_t frame_refcount(uint32_t page); /* 1 fuer ungetrackte, gueltige Frames */
uint32_t frame_tracked_count(void);

#endif
