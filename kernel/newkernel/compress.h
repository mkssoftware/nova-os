/* NovaOS C-Kernel - Nova Memory Compression (NPSPEC-MEMORY-COMPRESSION-
   0001). Optionale Reclamation-Strategie: `compress_page()` komprimiert
   den Inhalt eines Frames in einen begrenzten Compression-Pool und gibt
   den urspruenglichen Frame ERST nach erfolgreicher Erstellung der
   komprimierten Repraesentation frei (Anforderung #4); `decompress_page()`
   stellt ihn in einen frisch allokierten Frame zurueck (fuer die
   Demand-Paging-Integration, Anforderung #7).

   BEWUSSTE SCOPE-REDUKTION: der Kompressor ist ein echter, aber bewusst
   einfacher Byte-RLE-Codec (Run-Length Encoding, kein LZ/Huffman/Zstd) -
   ausreichend fuer Seiten mit grossen gleichen Byte-Laeufen (z.B. frisch
   genullte oder duennbesetzte Seiten), ineffektiv fuer echte Zufallsdaten.
   Deshalb erzwingt `compress_page()` Anforderung #6 ("nur bei sinnvollem
   Gewinn") hart: liefert die RLE-Ausgabe keinen Gewinn unter
   `COMPRESS_MIN_RATIO_PERCENT`, wird die Seite NICHT komprimiert (Frame
   bleibt unveraendert, Rueckgabe -1). NUMA-Beruecksichtigung (Anforderung
   #9) ist nicht umgesetzt (kein Unterschied in diesem Bootstrap-PMM). */
#ifndef NOVA_COMPRESS_H
#define NOVA_COMPRESS_H

#include <stdint.h>

#define COMPRESS_POOL_CAPACITY 8u
#define COMPRESS_MAX_OUTPUT (4096u * 2u) /* RLE-Worst-Case: 2 Bytes pro Eingabebyte */
#define COMPRESS_MIN_RATIO_PERCENT 75u   /* komprimiert muss <= 75% der Originalgroesse sein */

void compress_initialize(void);
int compress_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt, Original-Frame per frame_unref()
   freigegeben), -1 = kein sinnvoller Gewinn (Anforderung #6, Frame
   bleibt unveraendert) oder COMPRESS_POOL_CAPACITY erschoepft. */
int compress_page(uint32_t page, uint32_t *out_slot);

/* 0 = Erfolg (*out_page gesetzt), -1 = ungueltiger/leerer Slot oder PMM
   erschoepft. */
int decompress_page(uint32_t slot, uint32_t *out_page);

uint32_t compress_used_slots(void);
uint32_t compress_total_bytes_saved(void);

#endif
