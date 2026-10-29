/* NovaOS C-Kernel - PS/2-Controller-Treiber (siehe ps2.h fuer Scope). */
#include "ps2.h"
#include "input.h"
#include "interrupt.h"

#define PS2_PORT_DATA    0x60u
#define PS2_PORT_STATUS  0x64u
#define PS2_PORT_COMMAND 0x64u

#define PS2_STATUS_OUTPUT_FULL 0x01u
#define PS2_STATUS_INPUT_FULL  0x02u

static inline void ps2_outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t ps2_inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Begrenzte Spin-Waits statt endlosem Polling - ein fehlender/defekter
   PS/2-Controller darf den Boot nicht aufhaengen (gleiche Vorsicht wie
   bei jeder anderen Hardware-Initialisierung in diesem Kernel). */
static int ps2_wait_write(void) {
    uint32_t spins = 100000u;
    while (ps2_inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_FULL) {
        if (--spins == 0u) return -1;
    }
    return 0;
}

static int ps2_wait_read(void) {
    uint32_t spins = 100000u;
    while (!(ps2_inb(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT_FULL)) {
        if (--spins == 0u) return -1;
    }
    return 0;
}

static void ps2_flush_output(void) {
    uint32_t guard = 16u;
    while ((ps2_inb(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT_FULL) && guard > 0u) {
        (void)ps2_inb(PS2_PORT_DATA);
        --guard;
    }
}

static int ps2_controller_ok = 0;
static int ps2_mouse_present = 0;

int ps2_mouse_available(void) {
    return ps2_mouse_present;
}

void ps2_controller_initialize(void) {
    ps2_controller_ok = 0;
    ps2_mouse_present = 0;

    /* Beide Ports deaktivieren, damit waehrend der Konfiguration keine
       Bytes dazwischenfunken, dann den Ausgabepuffer leeren. */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0xADu); /* Port 1 (Tastatur) deaktivieren */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0xA7u); /* Port 2 (Maus) deaktivieren */
    ps2_flush_output();

    /* Config-Byte lesen, IRQs/Translation voruebergehend abschalten. */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0x20u); /* Read Controller Configuration Byte */
    if (ps2_wait_read() != 0) return;
    uint8_t config = ps2_inb(PS2_PORT_DATA);
    config &= (uint8_t)~0x43u; /* Bit0=IRQ1, Bit1=IRQ12, Bit6=Translation */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0x60u); /* Write Controller Configuration Byte */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_DATA, config);

    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0xAAu); /* Controller-Selbsttest */
    if (ps2_wait_read() != 0) return;
    if (ps2_inb(PS2_PORT_DATA) != 0x55u) return; /* Controller defekt/nicht vorhanden */

    /* Port 2 (Maus) vorhanden? 0xA9 testet ihn, Antwort 0x00 = ok. */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0xA9u);
    if (ps2_wait_read() != 0) return;
    int port2_present = (ps2_inb(PS2_PORT_DATA) == 0x00u);

    /* Beide Ports (wieder) aktivieren, IRQs im Config-Byte freigeben -
       die tatsaechliche IDT-Gate-/PIC-Masken-Freigabe bleibt
       interrupt.c's Aufgabe (siehe dort). */
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0xAEu); /* Port 1 (Tastatur) aktivieren */
    config |= 0x01u;
    if (port2_present) {
        if (ps2_wait_write() != 0) return;
        ps2_outb(PS2_PORT_COMMAND, 0xA8u); /* Port 2 (Maus) aktivieren */
        config |= 0x02u;
    }
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_COMMAND, 0x60u);
    if (ps2_wait_write() != 0) return;
    ps2_outb(PS2_PORT_DATA, config);

    if (port2_present) {
        /* Naechstes Datenbyte an Port 2 (Maus) statt Port 1 umleiten
           (0xD4), dann Streaming aktivieren (0xF4) - Antwort 0xFA (ACK)
           erwartet. */
        if (ps2_wait_write() == 0) {
            ps2_outb(PS2_PORT_COMMAND, 0xD4u);
            if (ps2_wait_write() == 0) {
                ps2_outb(PS2_PORT_DATA, 0xF4u);
                if (ps2_wait_read() == 0 && ps2_inb(PS2_PORT_DATA) == 0xFAu) {
                    ps2_mouse_present = 1;
                }
            }
        }
    }

    ps2_controller_ok = 1;
}

/* Scancode-Set-1 (Make-Codes), nur Grundbelegung - siehe ps2.h fuer die
   bewusst nicht abgebildeten Tasten (Funktionstasten, Ziffernblock,
   Pause/Druck, Feststelltaste-Ausnahme CAPSLOCK ist mit drin). Index =
   Scancode ohne Break-Bit (0x80). */
static const uint16_t ps2_set1_table[0x3Bu] = {
    [0x01] = VKEY_ESCAPE, [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8', [0x0A] = '9', [0x0B] = '0',
    [0x0C] = '-', [0x0D] = '=', [0x0E] = VKEY_BACKSPACE, [0x0F] = VKEY_TAB,
    [0x10] = 'Q', [0x11] = 'W', [0x12] = 'E', [0x13] = 'R', [0x14] = 'T', [0x15] = 'Y',
    [0x16] = 'U', [0x17] = 'I', [0x18] = 'O', [0x19] = 'P', [0x1A] = '[', [0x1B] = ']',
    [0x1C] = VKEY_ENTER, [0x1D] = VKEY_LCTRL,
    [0x1E] = 'A', [0x1F] = 'S', [0x20] = 'D', [0x21] = 'F', [0x22] = 'G', [0x23] = 'H',
    [0x24] = 'J', [0x25] = 'K', [0x26] = 'L', [0x27] = ';', [0x28] = '\'', [0x29] = '`',
    [0x2A] = VKEY_LSHIFT, [0x2B] = '\\',
    [0x2C] = 'Z', [0x2D] = 'X', [0x2E] = 'C', [0x2F] = 'V', [0x30] = 'B', [0x31] = 'N',
    [0x32] = 'M', [0x33] = ',', [0x34] = '.', [0x35] = '/', [0x36] = VKEY_RSHIFT,
    [0x38] = VKEY_LALT, [0x39] = ' ', [0x3A] = VKEY_CAPSLOCK,
};

static uint16_t ps2_set1_lookup(uint8_t code) {
    if (code >= (sizeof(ps2_set1_table) / sizeof(ps2_set1_table[0]))) return VKEY_NONE;
    return ps2_set1_table[code];
}

/* Erweiterte Scancodes (0xE0-Praefix) - nur die wenigen, die ohne echte
   Layout-Tabelle sinnvoll sind (rechte Modifier, Pfeiltasten, Pos1/Ende/
   Entf). Numpad-Varianten (0xE0 0x1C = Numpad-Enter usw.) bewusst nicht
   unterschieden. */
static uint16_t ps2_set1_extended_lookup(uint8_t code) {
    switch (code) {
        case 0x1Du: return VKEY_RCTRL;
        case 0x38u: return VKEY_RALT;
        case 0x48u: return VKEY_UP;
        case 0x50u: return VKEY_DOWN;
        case 0x4Bu: return VKEY_LEFT;
        case 0x4Du: return VKEY_RIGHT;
        case 0x47u: return VKEY_HOME;
        case 0x4Fu: return VKEY_END;
        case 0x53u: return VKEY_DELETE;
        default: return VKEY_NONE;
    }
}

int ps2_decode_scancode(uint8_t scancode, int extended_prefix, ps2_key_decode_t *out) {
    if (scancode == 0xE0u) return -1;

    uint8_t code = scancode & 0x7Fu;
    int is_break = (scancode & 0x80u) != 0;
    uint16_t vkey = extended_prefix ? ps2_set1_extended_lookup(code) : ps2_set1_lookup(code);
    if (vkey == VKEY_NONE) return -1;

    out->vkey = vkey;
    out->is_break = is_break;
    return 0;
}

static uint8_t ps2_mouse_bytes[3];
static uint32_t ps2_mouse_cycle = 0;

int ps2_mouse_assemble(uint8_t byte, ps2_mouse_packet_t *out) {
    if (ps2_mouse_cycle == 0u) {
        if ((byte & 0x08u) == 0u) return 0; /* Sync-Bit fehlt: verwerfen, auf Paketanfang warten */
    }
    ps2_mouse_bytes[ps2_mouse_cycle++] = byte;
    if (ps2_mouse_cycle < 3u) return 0;
    ps2_mouse_cycle = 0u;

    uint8_t status = ps2_mouse_bytes[0];
    int32_t dx = (int32_t)ps2_mouse_bytes[1];
    int32_t dy = (int32_t)ps2_mouse_bytes[2];
    if (status & 0x10u) dx -= 256; /* Vorzeichenbit X */
    if (status & 0x20u) dy -= 256; /* Vorzeichenbit Y */

    out->dx = dx;
    out->dy = -dy; /* PS/2: positives dy = nach oben - auf Bildschirmkoordinaten (nach unten) drehen */
    out->buttons = status & 0x07u;
    return 1;
}

static int ps2_kbd_extended_pending = 0;
static uint8_t ps2_kbd_modifiers = 0;

static void ps2_kbd_track_modifier(uint16_t vkey, int is_break) {
    uint8_t bit;
    switch (vkey) {
        case VKEY_LSHIFT: bit = INPUT_MOD_LSHIFT; break;
        case VKEY_RSHIFT: bit = INPUT_MOD_RSHIFT; break;
        case VKEY_LCTRL:  bit = INPUT_MOD_LCTRL;  break;
        case VKEY_RCTRL:  bit = INPUT_MOD_RCTRL;  break;
        case VKEY_LALT:   bit = INPUT_MOD_LALT;   break;
        case VKEY_RALT:   bit = INPUT_MOD_RALT;   break;
        default: return;
    }
    if (is_break) {
        ps2_kbd_modifiers = (uint8_t)(ps2_kbd_modifiers & ~bit);
    } else {
        ps2_kbd_modifiers = (uint8_t)(ps2_kbd_modifiers | bit);
    }
}

void ps2_keyboard_isr_byte(uint8_t scancode) {
    if (scancode == 0xE0u) {
        ps2_kbd_extended_pending = 1;
        return;
    }

    int extended = ps2_kbd_extended_pending;
    ps2_kbd_extended_pending = 0;

    ps2_key_decode_t decoded;
    if (ps2_decode_scancode(scancode, extended, &decoded) != 0) return;

    ps2_kbd_track_modifier(decoded.vkey, decoded.is_break);

    input_event_t evt = {0};
    evt.type = decoded.is_break ? INPUT_EVENT_KEY_UP : INPUT_EVENT_KEY_DOWN;
    evt.timestamp_ticks = timer_get_ticks();
    evt.key_vkey = decoded.vkey;
    evt.modifiers = ps2_kbd_modifiers;
    input_push_event(&evt);
}

void ps2_mouse_isr_byte(uint8_t data) {
    static uint8_t prev_buttons = 0;

    ps2_mouse_packet_t pkt;
    if (ps2_mouse_assemble(data, &pkt) != 1) return;

    uint32_t now = timer_get_ticks();

    if (pkt.dx != 0 || pkt.dy != 0) {
        input_event_t evt = {0};
        evt.type = INPUT_EVENT_MOUSE_MOVE;
        evt.timestamp_ticks = now;
        evt.mouse_dx = pkt.dx;
        evt.mouse_dy = pkt.dy;
        evt.mouse_buttons = pkt.buttons;
        input_push_event(&evt);
    }

    uint8_t changed = (uint8_t)(pkt.buttons ^ prev_buttons);
    if (changed != 0u) {
        for (uint8_t bit = 0; bit < 3u; ++bit) {
            uint8_t mask = (uint8_t)(1u << bit);
            if ((changed & mask) == 0u) continue;
            input_event_t evt = {0};
            evt.type = (pkt.buttons & mask) ? INPUT_EVENT_MOUSE_BUTTON_DOWN
                                             : INPUT_EVENT_MOUSE_BUTTON_UP;
            evt.timestamp_ticks = now;
            evt.mouse_buttons = mask;
            input_push_event(&evt);
        }
        prev_buttons = pkt.buttons;
    }
}

int ps2_controller_self_test(void) {
    if (!ps2_controller_ok) return -1;

    /* 'A' druecken (Make 0x1E) und loslassen (Break 0x9E). */
    ps2_key_decode_t decoded;
    if (ps2_decode_scancode(0x1Eu, 0, &decoded) != 0) return -1;
    if (decoded.vkey != 'A' || decoded.is_break != 0) return -1;
    if (ps2_decode_scancode(0x9Eu, 0, &decoded) != 0) return -1;
    if (decoded.vkey != 'A' || decoded.is_break != 1) return -1;

    /* Erweiterter Scancode (Pfeiltaste rechts, 0xE0 0x4D). */
    if (ps2_decode_scancode(0x4Du, 1, &decoded) != 0) return -1;
    if (decoded.vkey != VKEY_RIGHT) return -1;

    /* 0xE0 allein liefert (noch) kein Event. */
    if (ps2_decode_scancode(0xE0u, 0, &decoded) == 0) return -1;

    /* Unbekannter Scancode (Funktionstaste F1, bewusst nicht abgebildet)
       liefert -1, kein Absturz. */
    if (ps2_decode_scancode(0x3Bu, 0, &decoded) == 0) return -1;

    /* Maus-Paket-Assemblierung: positive Bewegung. ps2_mouse_cycle ist
       ein globaler Automaten-Zustand, den ps2_mouse_isr_byte (echte
       IRQ12-Bytes, seit der PIC-Freigabe in interrupt.c tatsaechlich
       asynchron im Hintergrund aktiv) sich mit diesem Selbsttest teilt -
       ohne expliziten Reset + kurze cli-Klammer koennte ein echtes,
       zwischenzeitlich eingetroffenes Maus-Byte den Zyklenzaehler auf
       einem unerwarteten Stand hinterlassen und den allerersten
       synthetischen Test-Byte-Schritt faelschlich als "Paket komplett"
       auswerten. */
    interrupt_disable();
    ps2_mouse_cycle = 0u;
    ps2_mouse_packet_t pkt;
    int mouse_ok = 1;

    if (mouse_ok && ps2_mouse_assemble(0x08u, &pkt) != 0) mouse_ok = 0;
    if (mouse_ok && ps2_mouse_assemble(5u, &pkt) != 0) mouse_ok = 0;
    if (mouse_ok && ps2_mouse_assemble(3u, &pkt) != 1) mouse_ok = 0;
    if (mouse_ok && (pkt.dx != 5 || pkt.dy != -3 || pkt.buttons != 0u)) mouse_ok = 0;

    /* Negative Bewegung (Vorzeichenbits in Byte 0 gesetzt) + linke Taste. */
    if (mouse_ok && ps2_mouse_assemble(0x39u, &pkt) != 0) mouse_ok = 0;
    if (mouse_ok && ps2_mouse_assemble(0xFBu, &pkt) != 0) mouse_ok = 0; /* dx_raw = -5 */
    if (mouse_ok && ps2_mouse_assemble(0xFEu, &pkt) != 1) mouse_ok = 0; /* dy_raw = -2 */
    if (mouse_ok && (pkt.dx != -5 || pkt.dy != 2 || pkt.buttons != 0x01u)) mouse_ok = 0;

    /* Byte ohne Sync-Bit zu Beginn eines Pakets wird verworfen, kein
       falsches Resync. */
    if (mouse_ok && ps2_mouse_assemble(0x00u, &pkt) != 0) mouse_ok = 0;
    if (mouse_ok && ps2_mouse_assemble(0x08u, &pkt) != 0) mouse_ok = 0;
    if (mouse_ok && ps2_mouse_assemble(1u, &pkt) != 0) mouse_ok = 0;
    if (mouse_ok && ps2_mouse_assemble(1u, &pkt) != 1) mouse_ok = 0;
    if (mouse_ok && (pkt.dx != 1 || pkt.dy != -1)) mouse_ok = 0;

    interrupt_enable();

    return mouse_ok ? 0 : -1;
}
