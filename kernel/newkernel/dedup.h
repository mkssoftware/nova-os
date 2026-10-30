/* NovaOS C-Kernel - Nova Memory Deduplication (NPSPEC-MEMORY-DEDUP-0001).
   Liefert die Hash+Verify+Merge-PRIMITIVE fuer zwei Frames mit
   wahrscheinlich identischem Inhalt: `dedup_check_pages()` berechnet
   einen schnellen Hash fuer beide Seiten UND vergleicht bei Uebereinstimmung
   zusaetzlich den vollstaendigen Byteinhalt (Anforderungen #3/#4 - ein
   Hash-Treffer alleine ist NIEMALS Beweis genug). `dedup_merge()` erhoeht
   den Referenzzaehler des zu behaltenden Frames und gibt den redundanten
   frei - OHNE Ownership/Permissions zu veraendern (Anforderung #5).

   BEWUSSTE SCOPE-REDUKTION: das tatsaechliche UMHAENGEN bestehender
   VAS-Mappings auf den gemeinsamen Frame (damit ein Merge auch wirklich
   Speicher spart) ist NICHT Teil dieses Moduls - das wuerde ein
   Durchsuchen ALLER VAS-Instanzen nach Mappings auf den redundanten
   Frame voraussetzen (kein Rueckwaerts-Index physischer Adresse ->
   Mappings vorhanden). `dedup_merge()` liefert nur die sichere
   Referenzzaehl-Grundoperation; das Umhaengen ist Aufgabe eines
   spaeteren Hintergrund-Scanners/Aufrufers, der die betroffenen
   VAS-IDs/Adressen bereits kennt. Schreibbare private Mappings nach
   einem Merge (Anforderung #6, COW-Trennung) sind damit automatisch
   durch das BESTEHENDE `cow.c`/Referenzzaehl-Verhalten abgedeckt, SOBALD
   ein Aufrufer die Mappings tatsaechlich umgehaengt hat - dieses Modul
   erzwingt das nicht selbst. Cross-Security-Domain-Policy (Anforderung
   #7) und Realtime-Ausschluss (Anforderung #11) sind nicht umgesetzt
   (haengen an der noch nicht konsolidierten 018-SECURITY-Policy-Engine). */
#ifndef NOVA_DEDUP_H
#define NOVA_DEDUP_H

#include <stdint.h>

void dedup_initialize(void);
int dedup_self_test(void); /* 0 = ok */

/* Schneller Hash (FNV-1a) ueber den vollstaendigen Seiteninhalt - NUR ein
   Vorfilter, niemals alleiniger Beweis (Anforderung #3). */
uint32_t dedup_hash_page(uint32_t page);

/* 1 = page_a und page_b haben IDENTISCHEN Inhalt (Hash-Vorfilter UND
   vollstaendiger Byte-Vergleich bestaetigt), 0 = unterschiedlich. */
int dedup_check_pages(uint32_t page_a, uint32_t page_b);

/* Erhoeht den Referenzzaehler von keep_page und gibt redundant_page frei
   (frame_unref) - NUR aufrufen, wenn dedup_check_pages() zuvor 1
   geliefert hat. 0 = Erfolg, -1 = frame_ref() schlug fehl (Tabelle voll). */
int dedup_merge(uint32_t keep_page, uint32_t redundant_page);

uint32_t dedup_scan_count(void);
uint32_t dedup_merge_count(void);
uint32_t dedup_false_positive_count(void); /* Hash-Treffer, aber Inhalt unterschiedlich */

#endif
