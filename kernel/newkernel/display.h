/* NovaOS C-Kernel - Nova Display Manager (reduzierter In-Kernel-Port der
   Display-Objekt-Abstraktion aus docs/NPSPEC/sysarchitecture/049-GUI/
   001-Display_Server/NPSPEC-DISPLAY-ARCH-0001.md und -ARCH-0007.md).

   WICHTIG (Nutzer-Entscheidung 2026-10-26): Die vollen NPSPECs beschreiben
   einen eigenstaendigen IPC-Display-Server mit Multi-GPU/Multi-Display,
   Hotplug, Capability-Sicherheit pro Client usw. - das setzt echten
   Userspace mit Taskwechsel voraus, den dieser Kernel bewusst noch nicht
   hat. Dieses Modul bildet nur die Display-Objekt-Abstraktion (ID, Modus,
   Aufloesung, Farbtiefe, Skalierung, Ausrichtung, Zustand) ueber dem
   bereits vorhandenen GOP-Framebuffer (framebuffer.c) nach, mit dem
   Lifecycle Uninitialized->Initializing->BasicReady->Running aus
   NPSPEC-DISPLAY-ARCH-0007. Multi-Display, Hotplug, Farbmanagement, HDR,
   VRR usw. sind NICHT portiert (keine passende Hardware-Erkennung
   vorhanden). */
#ifndef NOVA_DISPLAY_H
#define NOVA_DISPLAY_H

#include <stdint.h>

#define DISPLAY_STATE_UNINITIALIZED 0u
#define DISPLAY_STATE_INITIALIZING  1u
#define DISPLAY_STATE_BASIC_READY   2u
#define DISPLAY_STATE_RUNNING       3u
#define DISPLAY_STATE_FAILED        4u

#define DISPLAY_PRIMARY_ID 1u

/* 0 = Erfolg (Display-Objekt aus dem GOP-Framebuffer uebernommen,
   Zustand -> BASIC_READY), -1 = kein Framebuffer verfuegbar (Zustand ->
   FAILED). */
int display_initialize(uint32_t address, uint32_t pitch, uint32_t width,
                        uint32_t height, uint32_t bits_per_pixel);
int display_self_test(void); /* 0 = ok */

uint32_t display_id(void);
uint32_t display_width(void);
uint32_t display_height(void);
/* Feste 60Hz-Annahme: keine echte Bildwiederholraten-Erkennung ohne
   EDID/Grafiktreiber vorhanden. */
uint32_t display_refresh_rate(void);
uint32_t display_state(void);

/* Markiert den Uebergang BASIC_READY -> RUNNING (NPSPEC-DISPLAY-ARCH-0007:
   "ServerReady"/"DesktopReady" sind ohne IPC nicht sinnvoll trennbar, daher
   zusammengefasst). 0 = Erfolg, -1 wenn Zustand nicht BASIC_READY ist. */
int display_set_running(void);

#endif
