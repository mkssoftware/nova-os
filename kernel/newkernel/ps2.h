/* NovaOS C-Kernel - PS/2-Controller-Treiber (Port aus docs/NPSPEC/
   sysarchitecture/049-GUI/005-Inputsystem/NPSPEC-INPUT-DEVICE-0001.md
   (Keyboard Device Model) und -0002.md (Mouse Device Model). Reine
   Hardware-Schicht: Controller-Initialisierung (0x60/0x64-Dance),
   Scancode-Set-1-Decodierung, 3-Byte-Maus-Paket-Assemblierung. Kennt
   weder Fenster noch Fokus - das macht input.c (siehe dort).

   WICHTIG (Scope dieser Sitzung): Nur klassisches PS/2 (QEMU emuliert
   Tastatur/Maus standardmaessig darueber, auch ohne USB-Flags) - KEIN
   USB-HID (NPSPEC-INPUT-DEVICE-0005 Multi-Device/andere Geraeteklassen,
   BLOCKED: kein USB-HID-Treiber portiert). KEINE echte Tastaturlayout-
   Engine (NPSPEC-INPUT-ARCH-0001 Abschnitt 9, Input Mapping/Dead-Keys/
   IME, BLOCKED: kein Konfigurationsspeicher, kein Unicode-Text-Layer) -
   druckbare Tasten liefern direkt ihren US-QWERTY-Grundstellungs-ASCII-
   Code als vkey, OHNE dass Shift auf das Zeichen angewendet wird (Shift
   wird nur als Modifier-Bit gemeldet, siehe input.h). KEINE Tastatur-
   Wiederholungs-Steuerung (NPSPEC-INPUT-DEVICE-0001 Key-Repeat-
   Controller) - Hardware-Autorepeat des PS/2-Controllers wird 1:1 als
   normale Make-Wiederholung durchgereicht, ohne eigene Markierung.
   KEIN IntelliMouse-Scrollrad (4. Paket-Byte, NPSPEC-INPUT-DEVICE-0002
   Erweiterung) - nur das 3-Byte-Standardprotokoll. KEIN Hotplug
   (NPSPEC-INPUT-DEVICE-0006, BLOCKED: PS/2 hat kein Bus-Event). */
#ifndef NOVA_PS2_H
#define NOVA_PS2_H

#include <stdint.h>

/* Druckbare Zeichen (Ziffern/Buchstaben/Grundinterpunktion) nutzen direkt
   ihren ASCII-Code (0x20-0x7E) als vkey. Nicht druckbare/Sondertasten
   liegen bewusst oberhalb 0xFF, damit sie sich nie mit einem ASCII-Wert
   ueberschneiden koennen. */
#define VKEY_NONE      0x0000u
#define VKEY_ESCAPE    0x0100u
#define VKEY_BACKSPACE 0x0101u
#define VKEY_TAB       0x0102u
#define VKEY_ENTER     0x0103u
#define VKEY_LSHIFT    0x0104u
#define VKEY_RSHIFT    0x0105u
#define VKEY_LCTRL     0x0106u
#define VKEY_RCTRL     0x0107u
#define VKEY_LALT      0x0108u
#define VKEY_RALT      0x0109u
#define VKEY_CAPSLOCK  0x010Au
#define VKEY_UP        0x010Bu
#define VKEY_DOWN      0x010Cu
#define VKEY_LEFT      0x010Du
#define VKEY_RIGHT     0x010Eu
#define VKEY_DELETE    0x010Fu
#define VKEY_HOME      0x0110u
#define VKEY_END       0x0111u

typedef struct {
    uint16_t vkey;
    int is_break; /* 1 = Taste losgelassen (Break), 0 = gedrueckt (Make) */
} ps2_key_decode_t;

typedef struct {
    int32_t dx, dy; /* dy bereits auf Bildschirmkoordinaten invertiert
                        (PS/2 liefert positives dy = nach OBEN) */
    uint8_t buttons; /* Bit0=links, Bit1=rechts, Bit2=mitte */
} ps2_mouse_packet_t;

/* 0x60/0x64-Initialisierungssequenz (beide Ports deaktivieren, Output-
   Puffer leeren, Config-Byte lesen/setzen (IRQs zunaechst aus), Controller-
   Selbsttest 0xAA, Port-2-Vorhandensein pruefen 0xA9, beide Ports wieder
   aktivieren + IRQs im Config-Byte freigeben, bei vorhandener Maus
   Streaming per 0xF4 ueber den 0xD4-Umleitungsbefehl aktivieren). Legt
   KEINE IDT-Gates/PIC-Masken an - das bleibt interrupt.c's Aufgabe
   (siehe dort fuer IRQ1/IRQ12). Mehrfacher Aufruf ist sicher (wie bei
   jedem anderen *_initialize() in diesem Kernel). */
void ps2_controller_initialize(void);

/* 0 = Controller-Selbsttest (0xAA) war erfolgreich UND die reinen Decode-
   Funktionen (ps2_decode_scancode/ps2_mouse_assemble) liefern fuer
   synthetische Eingaben die erwarteten Ergebnisse - braucht keinen
   echten IRQ. -1 sonst (z.B. kein PS/2-Controller vorhanden). */
int ps2_controller_self_test(void);

/* 1 = waehrend ps2_controller_initialize() wurde ein funktionierender
   Maus-Port (Port 2) erkannt UND Streaming erfolgreich aktiviert. */
int ps2_mouse_available(void);

/* Reine Decodierfunktion (kein Zustand, siehe ps2_keyboard_isr_byte fuer
   die zustandsbehaftete 0xE0-Praefix-Verwaltung): scancode ohne das
   Break-Bit (0x80) wird gegen die Scancode-Set-1-Tabelle (bzw. die
   erweiterte Tabelle, wenn extended_prefix!=0) aufgeloest. 0 = Erfolg
   (*out gesetzt), -1 wenn scancode==0xE0 (reiner Praefix-Marker, noch
   kein Event) oder der Scancode nicht abgebildet ist (z.B. Funktions-
   tasten/Ziffernblock - bewusst nicht in dieser Sitzung abgebildet). */
int ps2_decode_scancode(uint8_t scancode, int extended_prefix, ps2_key_decode_t *out);

/* Fuettert ein Byte in die interne 3-Byte-Paket-State-Machine (Standard-
   PS/2-Mausprotokoll). Das erste Byte jedes Pakets muss Bit 3 gesetzt
   haben (Sync-Bit) - fehlt es, wird das Byte verworfen und die Suche
   nach dem Paketanfang faehrt beim naechsten Byte fort (Resync nach
   Paketverlust). 1 = *out ist ein vollstaendiges, fertiges Paket, 0 =
   Byte wurde zum laufenden Paket hinzugefuegt bzw. verworfen (noch kein
   fertiges Paket). */
int ps2_mouse_assemble(uint8_t byte, ps2_mouse_packet_t *out);

/* ISR-Einstiegspunkte (IF=0-Kontext, siehe interrupt.c Vektor 33/44):
   verwalten den 0xE0-Praefix- bzw. Paket-Byte-Zaehler-Zustand selbst,
   reichen ein fertiges Ergebnis per input_push_event() (input.h) weiter.
   Kein Blockieren, kein Allozieren - reine Zustandsmaschinen-Schritte. */
void ps2_keyboard_isr_byte(uint8_t scancode);
void ps2_mouse_isr_byte(uint8_t data);

#endif
