/* NovaOS C-Kernel - Nova Desktop Wallpaper.

   Liest das Desktop-Hintergrundbild zur Laufzeit aus einer Datei im
   gemounteten NovaFS-Systemvolume (novafs.c) - NICHT fest in den Kernel
   einkompiliert. Das Bild liegt dort bereits als einfaches, unkompri-
   miertes "NBS1"-Rohformat vor (siehe scripts/build-wallpaper.ps1: selbes
   Format wie der bestehende ESP-Bootsplash aus build-bootsplash.ps1, 32
   Byte Header + Width*Height*3 Byte RGB888-Payload, CRC32/IEEE 802.3 ueber
   die Payload) - dieses Modul enthaelt bewusst KEINEN PNG-/zlib-Decoder;
   die Konvertierung von PNG auf dieses Rohformat passiert ausschliesslich
   im Host-Werkzeug, niemals im Kernel.

   wallpaper_load_from_novafs() kopiert die Pixel in einen eigenen
   statischen Puffer (fest, WALLPAPER_MAX_WIDTH x WALLPAPER_MAX_HEIGHT) -
   dadurch bleibt das geladene Bild gueltig, selbst wenn novafs.c's
   globaler Mount-Zustand spaeter durch einen der vielen NovaFS-
   Selbsttests (die jeweils ihr eigenes synthetisches Testvolume mounten)
   ueberschrieben wird. */
#ifndef NOVA_WALLPAPER_H
#define NOVA_WALLPAPER_H

#include <stdint.h>

#define WALLPAPER_MAX_WIDTH  1280u
#define WALLPAPER_MAX_HEIGHT 800u

/* Liest path (len Byte, KEIN Nullterminator, MUSS mit '/' beginnen - siehe
   novafs_resolve_path) aus dem aktuell gemounteten NovaFS-Volume, prueft
   Magie/Header-Groesse/Format/Abmessungen/CRC32 und kopiert die Pixel in
   den internen Puffer. 0 = Erfolg (wallpaper_available()/_pixels_rgb888()/
   _width()/_height() danach gueltig), -1 bei jedem Fehler (kein Mount,
   Datei fehlt, Format/Groesse/CRC ungueltig oder Bild grosser als
   WALLPAPER_MAX_WIDTH/_HEIGHT) - der vorherige Zustand (verfuegbar oder
   nicht) bleibt dann unveraendert, kein Panic. */
int wallpaper_load_from_novafs(const char *path, uint32_t len);

/* 1 = ein gueltiges Bild wurde per wallpaper_load_from_novafs() geladen. */
int wallpaper_available(void);

/* Width*Height*3 Byte, RGB888 (Byte 0=Rot), Zeilen zu je width()*3 Byte
   ohne Padding - direkt als Quelle fuer compositor_set_background_image()
   geeignet. NUR gueltig, wenn wallpaper_available() 1 ist. */
const uint8_t *wallpaper_pixels_rgb888(void);
uint32_t wallpaper_width(void);
uint32_t wallpaper_height(void);

int wallpaper_self_test(void); /* 0 = ok */

#endif
