/* NovaOS C-Kernel - USB-Stack: xHCI-Controller, USB-Core, Enumeration,
   HID, Mass Storage (Port aus usb32.inc, NPSPEC-USB-ARCHITECTURE-0001 und
   verwandte NPSPEC-USB-Spezifikationen).

   STAND (2026-11-24, siehe KERNEL_GAP_ANALYSIS.md Abschnitt 2.8): echte
   EP0-Control-Transfers sind jetzt implementiert (Enable Slot + Address-
   Device-Kommando ueber den Command-Ring, Setup/Data/Status-Stage-TRBs
   ueber einen echten Transfer-Ring pro Geraet, Event-Ring-Polling fuer
   Command-Completion/Transfer-Events) - der vorherige permanente Stub
   (immer Fehler) ist ersetzt. Geraete-Enumeration (GET_DESCRIPTOR/
   SET_CONFIGURATION) und HID-Erkennung funktionieren dadurch echt.

   WEITERHIN NICHT implementiert: echte BULK-Transfers (Mass-Storage BOT:
   CBW/Daten/CSW ueber Bulk-Endpoint-Transfer-Ringe) - `usb_blk_read()`/
   `usb_blk_write()` gaben vorher FAKE-ERFOLG zurueck, geben jetzt ehrlich
   -1 zurueck, bis dieser Mechanismus existiert (analog zum neuen EP0-
   Mechanismus aufzubauen). `usb_msc_init()` erkennt Mass-Storage-Geraete
   daher zwar an der Klasse, registriert aber bewusst KEIN Block-Device
   (bricht bei `usb_scsi_read_capacity()` ab). Ebenfalls fehlend: Interrupt-
   Transfers fuer HID-Tastatur/Maus-Eingaben (Geraet wird erkannt, liefert
   aber noch keine echten Events - input.c nutzt weiterhin PS/2). */
#ifndef NOVA_USB_H
#define NOVA_USB_H

#include <stdint.h>

#define USB_CORE_UNINIT   0u
#define USB_CORE_RUNNING  1u
#define USB_CORE_DEGRADED 2u
#define USB_CORE_STOPPED  3u

#define XHCI_CAPACITY    2u
#define USB_DEV_CAPACITY 8u

/* Durchsucht die Driver-Discovery-Tabelle (driver.c) nach xHCI-Controllern
   und initialisiert jeden vollstaendig (Reset, Start, Root-Hub-Port-Scan).
   Fehlende USB-Hardware ist kein Fehler (immer 0). */
int usb_initialize(void);
int usb_self_test(void); /* 0 = ok (kein Controller = ok, NPSPEC-konform) */

#endif
