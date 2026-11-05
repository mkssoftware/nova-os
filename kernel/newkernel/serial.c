/* NovaOS C-Kernel - serielle Grundausgabe (COM1, 16550-kompatibel).

   SMP-sicheres Logging: VOR dieser Aenderung war serial_putc()/serial_puts()
   weder gegen gleichzeitige Aufrufe von mehreren CPUs (SMP, siehe smp.h -
   "SMP Online-APs" im Boot-Log) noch gegen einen Aufruf aus einem
   Interrupt-Handler WAEHREND ein anderer Kontext bereits mitten in einem
   serial_puts()-Aufruf steckt abgesichert:
   - Zwei Schreiber, die gleichzeitig `inb(0x3FD) & 0x20` (Transmitter
     Holding Register Empty) pruefen und beide "bereit" sehen, koennen
     beide outb() auf denselben UART-Port ausfuehren - ein Byte geht
     verloren/wird ueberschrieben (klassischer TOCTOU-Race).
   - `serial_at_line_start` ist ein geteiltes globales Flag ohne jeden
     Schutz - ein zweiter Schreiber kann mitten im Zeitstempel-Praefix
     eines ersten Schreibers eigene Zeichen einstreuen (zerrissene/
     verschachtelte Log-Zeilen, insbesondere gefaehrlich bei einem
     Kernel-Panic-Handler, der GENAU DANN protokollieren will, wenn ein
     anderer Kontext bereits mitten im Schreiben war).

   Fix: ein IRQ-sicherer Spinlock (sync.h, bereits fuer genau diesen
   Zweck vorgesehen - Bootstrap-Synchronisation OHNE Scheduler-
   Abhaengigkeit) schuetzt JEDEN einzelnen Aufruf von serial_putc()/
   serial_puts()/serial_put_hex32() als atomare Einheit:
   sync_spin_lock_irqsave() deaktiviert zusaetzlich Interrupts auf der
   aufrufenden CPU WAEHREND der Lock gehalten wird - das verhindert nicht
   nur echte SMP-Races (andere CPU), sondern auch das Selbst-Deadlock-
   Szenario auf EINER CPU: haelt ein Thread den Lock und wird durch einen
   Timer-Tick unterbrochen, dessen Interrupt-Handler ebenfalls protokol-
   lieren will, wuerde dieser ohne IRQ-Deaktivierung ewig auf einen Lock
   warten, den der unterbrochene (und nun nicht weiterlaufende) Thread
   nie freigibt. sync_spin_lock_irqsave()/_unlock_irqrestore() arbeiten
   als reine, zustandslose Primitive direkt auf dem uebergebenen Lock-Wort
   (siehe sync.c) - sie benoetigen KEIN vorheriges sync_initialize() und
   koennen daher bereits in serial_init() (der erste Aufruf im gesamten
   Kernel, lange vor jedem anderen Modul-Init) sicher verwendet werden.

   Bewusste Scope-Grenze: NUR jeder EINZELNE Aufruf ist atomar. Die im
   gesamten Kernel etablierte Mehr-Aufruf-Zeile (`serial_puts("Text=");
   serial_put_hex32(wert); serial_puts("\r\n");`) bleibt zwischen ihren
   drei Teilaufrufen weiterhin fuer eine Verschachtelung durch einen
   anderen Schreiber offen - eine echte Gesamtzeilen-Atomaritaet wuerde
   eine neue, formatierende Logging-API (ein einziger Aufruf pro Zeile)
   voraussetzen und haette Aenderungen an hunderten bestehenden
   Aufrufstellen im gesamten Kernel zur Folge - bewusst nicht Teil dieser
   Aenderung (reine Byte-/UART-Race-Sicherheit war die gestellte
   Aufgabe). */
#include "serial.h"
#include "sync.h"
#include <stdint.h>

static volatile uint32_t serial_lock = SYNC_SPIN_UNLOCKED;

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint64_t serial_rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static void serial_raw_putc(unsigned char c) {
    while (!(inb(0x3FD) & 0x20)) { }
    outb(0x3F8, c);
}

/* Jede neue Zeile beginnt mit einem Millisekunden-Zeitstempel seit Boot
   [NNNNNms], damit sich die Boot-Zeit zwischen beliebigen zwei
   Ausgabezeilen direkt in Millisekunden nachpruefen laesst. Kalibrierung
   erfolgt einmalig in serial_init() ueber einen echten PIT-Zaehler-
   Durchlauf (Polling, OHNE IRQs - zu diesem Zeitpunkt ist IF=0, also vor
   interrupt.c's sti; selbe PIT-Konstanten wie interrupt.c's timer_initialize,
   das den Zaehler danach ohnehin erneut programmiert). */
static uint64_t serial_boot_tsc = 0;
static uint32_t serial_tsc_khz = 1; /* 1 = sicherer Nenner, falls Kalibrierung fehlschlaegt */
static int serial_at_line_start = 1;

/* 64-Bit-durch-32-Bit-Division ohne __udivdi3 (im Freestanding-Build nicht
   linkbar, siehe die bestehende 64-Bit-Regel): einfache bitweise
   Restoring-Division nur mit Shift/Vergleich/Subtraktion. */
static uint32_t serial_udiv64_32(uint64_t numerator, uint32_t denom) {
    uint64_t remainder = 0;
    uint64_t quotient = 0;
    for (int i = 63; i >= 0; --i) {
        remainder = (remainder << 1) | ((numerator >> i) & 1u);
        quotient <<= 1;
        if (remainder >= denom) {
            remainder -= denom;
            quotient |= 1u;
        }
    }
    return (uint32_t)quotient;
}

static uint16_t serial_pit_read_count(void) {
    outb(0x43, 0x00); /* Latch-Kommando, Channel 0 */
    uint8_t lo = inb(0x40);
    uint8_t hi = inb(0x40);
    return (uint16_t)lo | ((uint16_t)hi << 8);
}

static void serial_calibrate_tsc(void) {
    /* Channel 0, lobyte/hibyte, Mode 3, binaer - identische Konstanten wie
       interrupt.c's PIT_COMMAND/PIT_DIVISOR (~100Hz, 10ms pro Durchlauf). */
    const uint16_t divisor = 11932u;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFFu));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFFu));

    uint16_t last = serial_pit_read_count();
    uint64_t start = serial_rdtsc();

    /* Auf einen vollen Zaehler-Durchlauf warten (Counter zaehlt runter und
       springt beim Nachladen wieder hoch) - signalisiert ein volles
       10ms-Intervall. */
    uint16_t now;
    do {
        now = serial_pit_read_count();
        if (now > last) break;
        last = now;
    } while (1);

    uint64_t end = serial_rdtsc();
    uint32_t delta = (uint32_t)(end - start);
    uint32_t khz = delta / 10u; /* 10ms Intervall -> kHz = Zyklen/10 */
    if (khz > 0) {
        serial_tsc_khz = khz;
    }
}

static void serial_put_timestamp(void) {
    uint64_t elapsed_cycles = serial_rdtsc() - serial_boot_tsc;
    uint32_t elapsed_ms = serial_udiv64_32(elapsed_cycles, serial_tsc_khz);

    serial_raw_putc('[');

    char digits[10];
    int n = 0;
    if (elapsed_ms == 0) {
        digits[n++] = '0';
    } else {
        while (elapsed_ms > 0) {
            digits[n++] = (char)('0' + (elapsed_ms % 10u));
            elapsed_ms /= 10u;
        }
    }
    while (n > 0) {
        serial_raw_putc((unsigned char)digits[--n]);
    }

    serial_raw_putc('m');
    serial_raw_putc('s');
    serial_raw_putc(']');
    serial_raw_putc(' ');
}

void serial_init(void) {
    sync_spin_init(&serial_lock);

    outb(0x3F9, 0x00);
    outb(0x3FB, 0x80);
    outb(0x3F8, 0x01);
    outb(0x3F9, 0x00);
    outb(0x3FB, 0x03);
    outb(0x3FA, 0xC7);
    outb(0x3FC, 0x0B);

    serial_boot_tsc = serial_rdtsc();
    serial_calibrate_tsc();
}

void serial_putc(char c) {
    uint32_t flags = sync_spin_lock_irqsave(&serial_lock);
    serial_raw_putc((unsigned char)c);
    sync_spin_unlock_irqrestore(&serial_lock, flags);
}

void serial_puts(const char *s) {
    uint32_t flags = sync_spin_lock_irqsave(&serial_lock);
    while (*s) {
        if (serial_at_line_start) {
            serial_put_timestamp();
            serial_at_line_start = 0;
        }
        serial_raw_putc((unsigned char)*s);
        if (*s == '\n') {
            serial_at_line_start = 1;
        }
        ++s;
    }
    sync_spin_unlock_irqrestore(&serial_lock, flags);
}

void serial_put_hex32(unsigned int value) {
    static const char digits[] = "0123456789ABCDEF";
    uint32_t flags = sync_spin_lock_irqsave(&serial_lock);
    for (int shift = 28; shift >= 0; shift -= 4) {
        serial_raw_putc((unsigned char)digits[(value >> shift) & 0xF]);
    }
    sync_spin_unlock_irqrestore(&serial_lock, flags);
}

int serial_self_test(void) {
    /* sync_spin_try_lock() auf demselben Lock-Wort darf NACH jedem
       abgeschlossenen oeffentlichen Aufruf sofort erfolgreich sein -
       das beweist (ohne internen Zustand nach aussen zu exponieren),
       dass jeder Aufruf seinen Lock korrekt wieder freigibt (kein
       Leck, kein Selbst-Deadlock). Ein haengen gebliebener Lock wuerde
       hier sofort als Fehlschlag sichtbar statt den Kernel beim
       naechsten echten serial_puts()-Aufruf stillschweigend einzufrieren. */
    serial_putc(' ');
    if (sync_spin_try_lock(&serial_lock) != 0) return -1;
    sync_spin_unlock(&serial_lock);

    serial_puts("\r\n");
    if (sync_spin_try_lock(&serial_lock) != 0) return -1;
    sync_spin_unlock(&serial_lock);

    serial_put_hex32(0xDEADBEEFu);
    serial_puts("\r\n");
    if (sync_spin_try_lock(&serial_lock) != 0) return -1;
    sync_spin_unlock(&serial_lock);

    /* Spinlock-Grundverhalten direkt am echten Logging-Lock bewiesen
       (kein separater Test-Lock noetig, siehe sync_self_test() fuer die
       allgemeine Spinlock-Abdeckung): ein bereits gehaltener Lock lehnt
       einen zweiten try_lock ab. */
    uint32_t flags = sync_spin_lock_irqsave(&serial_lock);
    int try_while_held = sync_spin_try_lock(&serial_lock);
    sync_spin_unlock_irqrestore(&serial_lock, flags);
    if (try_while_held == 0) return -1; /* haette belegt sein muessen */
    if (sync_spin_try_lock(&serial_lock) != 0) return -1;
    sync_spin_unlock(&serial_lock);

    return 0;
}
