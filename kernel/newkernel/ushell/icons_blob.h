/* NovaOS - Generiert aus kernel/newkernel/ushell/icons_main.c per
   objcopy -O binary + PowerShell-Byte-Array-Emission (siehe
   ushell_process.c fuer den Verwender). NICHT HANDEDITIEREN - bei
   Aenderungen an icons_main.c neu generieren. ICONS_ENTRY_OFFSET ist
   der per 'nm' ermittelte reale Offset von icons_start() im
   gelinkten Binary (der Compiler platziert die Funktion NICHT
   zwingend bei Offset 0, siehe shell_main.c/USHELL_ENTRY_OFFSET fuer
   dieselbe bereits dokumentierte Falle). */
#ifndef NOVA_ICONS_BLOB_H
#define NOVA_ICONS_BLOB_H

#include <stdint.h>

#define ICONS_BLOB_SIZE 0u
#define ICONS_ENTRY_OFFSET 4816u

static const uint8_t icons_blob[ICONS_BLOB_SIZE] = {
};

#endif
