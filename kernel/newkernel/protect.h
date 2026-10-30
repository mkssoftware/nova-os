/* NovaOS C-Kernel - Nova Memory Protection Policy (NPSPEC-MEMORY-
   PROTECTION-0001). Haengt sich vor vas_protect() (vas.h) und erzwingt
   zusaetzlich zur dort bereits vorhandenen W^X-Durchsetzung, dass
   Rechte-ERWEITERUNGEN (z.B. READ -> READ+WRITE) nur mit expliziter
   Autorisierung moeglich sind (Anforderung #7 "Rechteerweiterungen
   MUESSEN durch die zustaendige Security Policy autorisiert sein") - eine
   reine Rechte-EINSCHRAENKUNG (z.B. READ+WRITE -> READ) ist immer erlaubt.

   BEWUSSTE SCOPE-REDUKTION: "die zustaendige Security Policy" ist in
   diesem Kernel noch kein eigenstaendiges Subsystem (haengt an
   018-SECURITY, noch nicht konsolidiert) - die Autorisierung ist deshalb
   hier ein expliziter Caller-Parameter (`authorized`), kein Nachschlagen
   in einer echten Policy-Engine. DMA/IOMMU-Isolation (Anforderung #11)
   ist nicht umgesetzt (kein IOMMU-Treiber vorhanden). */
#ifndef NOVA_PROTECT_H
#define NOVA_PROTECT_H

#include <stdint.h>

void protect_initialize(void);
int protect_self_test(void); /* 0 = ok */

/* Wendet eine neue Permission-Maske auf ein bestehendes VAS-Mapping an.
   Erweitert die aktuelle Mapping-Permission um mindestens ein Bit, OHNE
   dass `authorized` != 0 ist, wird abgelehnt (Protection Fault gezaehlt).
   Reine Einschraenkungen sind immer erlaubt. 0 = Erfolg, -1 = abgelehnt
   (W^X-Verstoss, fehlende Autorisierung oder kein passendes Mapping). */
int protect_change(uint32_t vas_id, uint32_t virtual_base, uint32_t new_permissions, int authorized);

uint32_t protect_fault_count(void);

#endif
