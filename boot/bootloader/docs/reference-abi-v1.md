# Nova BIOS Reference ABI v1

- Status: Referenzimplementierung
- Gültig für: BIOS-Prototyp
- Übergeordnet: NPSPEC-BOOTHANDOFF-0001, NPSPEC-KERNELLOADER-0001

Dieses Dokument beschreibt ausschließlich das gegenwärtige bytegenaue
Binärlayout des BIOS-Referenzpfads. Es ersetzt keine NPSPEC, NIS oder NFD.

## Nova Kernel Image

Ein NKI-v1-Image besteht aus einem 64 Byte großen Header und dem direkt
folgenden Kernel-Payload.

- Magic: `NOVANKI\0`
- Architektur: x86-32
- Kompression: keine
- Payload-Prüfsumme: CRC32/ISO-HDLC
- maximale Payloadgröße: 32 KiB
- Ladeadresse und Entry Point: `0x00100000`

Der Loader prüft Magic, Version, Headergröße, Architektur, Kompression,
Ladeadresse, Entry Point, Payloadgröße und CRC32 vor dem Kernelstart.

## Boot Information Block

Der BIB beginnt mit einem 32 Byte großen Header:

- Magic: `NBHPBIB\0`
- Version: 1.0.0
- Gesamtgröße
- CRC32
- Architektur
- Flags

Die CRC32 wird über den gesamten BIB berechnet. Das vier Byte große
Prüfsummenfeld wird dabei als null behandelt.

Auf den Header folgen 8-Byte-ausgerichtete TLV-Einträge. Der BIOS-Pfad
erzeugt aktuell:

1. Firmware
2. Memory Map
3. Graphics
4. Kernel
5. Security
6. Boot Options

Unbekannte TLV-Typen werden vom Kernel übersprungen. Firmware, Memory und
Kernel sind Pflicht. Graphics ist optional und besitzt einen Textmodus-Fallback.

## x86-32-Handoff

- Interrupts sind deaktiviert.
- `EAX` enthält `0x4E4F5641`.
- `EBX` zeigt auf den 8-Byte-ausgerichteten BIB.
- `ESP` zeigt auf den Kernel-Stack.
- Direction Flag ist gelöscht.
- Der Bootloader springt direkt zum validierten NKI-Entry-Point.

Der Kernel validiert den BIB vollständig und erzeugt daraus einen internen
Kernel Context. Nachfolgende Kernelteile greifen nicht direkt auf
Bootloader-interne Strukturen zu.
