/* NovaOS C-Kernel - Nova Transaction Rollback Journal (Port aus
   adaptive32.inc, NPSPEC-TRANSACTION-ROLLBACK-0001). Rollback-Journal fuer
   atomare Operationen: vor jeder Aenderung txroll_save, bei Fehler
   txroll_undo (stellt alle gespeicherten Werte wieder her), bei Erfolg
   txroll_commit (loescht das Journal). */
#ifndef NOVA_TXROLL_H
#define NOVA_TXROLL_H

#include <stdint.h>

#define TXROLL_CAPACITY 8u

void txroll_initialize(void);
int txroll_self_test(void); /* 0 = ok */

/* 0 = Erfolg (aktueller Wert von *address gespeichert), -1 wenn Journal
   voll. */
int txroll_save(uint32_t *address);
/* Stellt alle gespeicherten Werte wieder her und leert das Journal. */
void txroll_undo(void);
/* Leert das Journal ohne Wiederherstellung (Aenderungen bleiben). */
void txroll_commit(void);

#endif
