/* NovaOS C-Kernel - USB-Stack: xHCI-Controller, USB-Core, Enumeration,
   HID, Mass Storage (Port aus usb32.inc, NPSPEC-USB-ARCHITECTURE-0001 und
   verwandte NPSPEC-USB-Spezifikationen).

   WICHTIG: Schon im alten ASM war `usb_control_read_descriptor` (echte
   EP0-Control-Transfers ueber xHCI Transfer-Rings) ein permanenter Stub,
   der immer CF=1 zurueckgab ("bis der Transfer-Ring-Mechanismus verfuegbar
   ist"). Dadurch schlaegt jede Geraete-Enumeration bereits beim ersten
   Schritt fehl - HID/Mass-Storage-Klassentreiber und die Block-I/O-
   Callbacks sind im alten UND im neuen Code strukturell vollstaendig,
   aber praktisch unerreichbarer Code, bis ein echter Transfer-Ring-
   Mechanismus existiert. Nur Controller-Reset/Start und Port-Scan (reale
   MMIO-Zugriffe) werden tatsaechlich ausgefuehrt. */
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
