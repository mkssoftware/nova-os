# NPSPEC-BOOTSTAGE1-0001
# Nova Boot Stage 1 Framework


| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTSTAGE1-0001 |
| Titel | Nova Boot Stage 1 Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot Specification |
| Bereich | Nova Boot Platform |
| Übergeordnet | NPSPEC-BOOT-0001 |
| Abhängigkeiten | NPSPEC-BIOSBOOT-0001, NPSPEC-BOOTMEMORY-0001, NPSPEC-BOOTLOADER-0001 |
| Letzte Änderung | 2026-07-14 |

---

# 1. Zweck

Der **Nova Stage-1-Bootloader** bildet den ersten ausführbaren Code des Betriebssystems nach der Firmware.

Er besitzt nur eine einzige Aufgabe:

> Die Kontrolle möglichst sicher, reproduzierbar und deterministisch von der Firmware auf den Nova Boot Core zu übertragen.

Stage 1 besitzt absichtlich **keine Betriebssystemlogik**.

Er enthält weder Dateisysteme noch grafische Oberflächen oder komplexe Hardwareinitialisierung.

Seine Aufgabe besteht ausschließlich darin,

- den Prozessor zu stabilisieren
- den Bootdatenträger festzustellen
- Stage 2 zuverlässig zu laden
- bei Fehlern definierte Recoverypfade einzuleiten

---

# 2. Designphilosophie

Stage 1 folgt fünf Grundprinzipien.

## Minimalismus

Jede zusätzliche Funktion erhöht

- Fehlerwahrscheinlichkeit
- Wartungsaufwand
- Angriffsfläche

Deshalb muss Stage 1 so klein wie möglich bleiben.

---

## Determinismus

Unter identischen Voraussetzungen muss Stage 1 immer exakt denselben Ablauf besitzen.

Es dürfen keinerlei zufällige Entscheidungen getroffen werden.

---

## Plattformtreue

Stage 1 verwendet ausschließlich BIOS-Funktionen.

Eigene Treiber existieren in dieser Phase nicht.

---

## Stabilität

Stage 1 darf niemals versuchen,

- Speicher zu verwalten
- Hardware vollständig zu initialisieren
- Geräte zu konfigurieren

---

## Wartbarkeit

Der gesamte Quellcode soll

- klar strukturiert
- stark kommentiert
- leicht überprüfbar
- leicht portierbar

sein.

---

# 3. Verantwortlichkeiten

Stage 1 ist ausschließlich verantwortlich für

✔ CPU-Grundinitialisierung

✔ Segmentregister

✔ Stack

✔ Bootlaufwerk sichern

✔ Stage-2 laden

✔ Fehler erkennen

✔ BIOS-Reset bei Fehlern

✔ Wiederholungsversuche

✔ Übergabe an Stage 2

Alles andere gehört **nicht** zu Stage 1.

---

# 4. Nicht-Aufgaben

Stage 1 darf NICHT

- FAT lesen
- GPT lesen
- ELF laden
- Kernel laden
- ACPI lesen
- PCI scannen
- Grafik initialisieren
- Maus initialisieren
- USB initialisieren
- Secure Boot durchführen
- TPM verwenden
- Netzwerk verwenden

Diese Funktionen gehören ausschließlich Stage 2 oder späteren Komponenten.

---

# 5. Speichergrenzen

Da Stage 1 im MBR untergebracht ist gelten folgende Grenzen.

| Eigenschaft | Wert |
|-------------|------|
| Größe | 512 Byte |
| Signatur | 0xAA55 |
| Code | maximal verfügbarer Platz |
| Daten | minimal |
| Stack | extern |

Der Bootloader darf niemals voraussetzen, dass mehr Speicher vorhanden ist als durch das BIOS garantiert.

---

# 6. Architektur

```text
BIOS

↓

MBR

↓

Nova Stage 1

↓

Stage 2

↓

Nova Boot Core
```

Stage 1 besitzt keinerlei Kenntnis über

- Kernel
- Recovery
- Benutzer
- Dateisysteme

Er kennt ausschließlich Stage 2.

---

# 7. CPU-Startzustand

Beim Eintritt in Stage 1 wird folgender Zustand angenommen.

CPU Mode

```
Real Mode
```

Adressierung

```
20 Bit
```

Interrupts

```
aktiv
```

Paging

```
deaktiviert
```

Protected Mode

```
nicht aktiv
```

Long Mode

```
nicht aktiv
```

---

# 8. Erwarteter Registerzustand

Das BIOS garantiert typischerweise

```
DL = Bootlaufwerk
```

Weitere Register dürfen **nicht** vorausgesetzt werden.

Stage 1 muss deshalb alle relevanten Register selbst initialisieren.

---

# 9. CPU-Initialisierung

Unmittelbar nach Eintritt müssen folgende Register gesetzt werden.

```asm
cli

xor ax, ax

mov ds, ax
mov es, ax
mov ss, ax

mov sp, 0x7C00
```

Anschließend kann mit der eigentlichen Bootlogik begonnen werden.

---

# 10. Bootlaufwerk

Das BIOS übergibt das Bootlaufwerk in

```
DL
```

Dieses Register muss sofort gesichert werden.

Beispiel

```asm
mov [BootDrive], dl
```

Spätere BIOS-Aufrufe dürfen niemals davon ausgehen, dass DL unverändert geblieben ist.

---

# 11. Stack

Stage 1 verwendet einen einfachen Stack.

Eigenschaften

- wächst nach unten
- keine dynamische Speicherverwaltung
- keine Rekursion
- keine tiefen Funktionsaufrufe

Empfohlen

```
SS = 0x0000

SP = 0x7C00
```

---

# 12. Speicherlayout

Empfohlenes Layout

```text
00000 ------------------

BIOS Data

00400 ------------------

IVT

00500 ------------------

frei

07C00 ------------------

Stage 1

07E00 ------------------

Stage 2

08000 ------------------

frei

90000 ------------------

spätere Kerneldaten

A0000 ------------------

Video RAM
```

Dieses Layout entspricht dem bisherigen Nova-Prototyp.

---

# 13. Lebenszyklus

Der Lebenszyklus besitzt genau fünf Zustände.

```text
Start

↓

CPU Setup

↓

Boot Device

↓

Load Stage 2

↓

Jump Stage 2
```

Fehler führen in einen separaten Fehlerzustand.

---

# 14. Zustandsdiagramm

```text
Start

↓

Initialize

↓

Read Stage 2

↓

OK ?

├── Nein

│

Retry

│

↓

Fehler

│

↓

Stop

│

└── Ja

↓

Jump Stage 2
```

---

# 15. Anforderungen

Stage 1 MUSS

- DS initialisieren
- ES initialisieren
- SS initialisieren
- SP initialisieren
- DL sichern
- Stage 2 laden
- Fehler erkennen
- BIOS zurücksetzen
- Retry durchführen
- Sprung zu Stage 2 ausführen

---

# 16. SHOULD

Stage 1 SOLLTE

- möglichst klein bleiben
- keine globalen Seiteneffekte erzeugen
- Register dokumentieren
- Fehlercodes definieren
- Kommentare enthalten
- BIOS-Aufrufe kapseln

---

# 17. MAY

Stage 1 DARF

- Prüfsummen verwenden
- einfache Debugausgaben besitzen
- Bootzeiten messen
- Buildinformationen enthalten

Diese Funktionen dürfen jedoch die Hauptaufgabe nicht gefährden.

---

# 18. Sicherheitsmodell

Stage 1 besitzt noch keine vollständige Sicherheitsarchitektur.

Es soll jedoch

- keine undefinierten Speicherbereiche überschreiben
- keine BIOS-Strukturen zerstören
- keine unbekannten Interrupts verwenden
- keine Selbstmodifikation durchführen

---

# 19. Performance

Die Ladezeit von Stage 1 soll möglichst gering bleiben.

Zielwerte

| Vorgang | Ziel |
|----------|------|
| CPU Setup | < 1 ms |
| Bootlaufwerk sichern | < 1 ms |
| Laden Stage 2 | Firmwareabhängig |
| Übergabe | sofort |

---

# 20. Erweiterbarkeit

Stage 1 soll bewusst klein bleiben.

Neue Funktionen werden grundsätzlich in

- Stage 2
- Boot Core

integriert.

Stage 1 wird langfristig nur angepasst wenn

- neue Firmwareanforderungen entstehen
- Sicherheitslücken behoben werden
- neue Datenträgerformate zwingend erforderlich sind

---

---

# 21. Master Boot Record (MBR)

Der Nova Stage-1-Bootloader wird als klassischer **Master Boot Record (MBR)** implementiert.

Der MBR befindet sich immer im **ersten logischen Sektor eines bootfähigen Datenträgers**.

Eigenschaften:

| Eigenschaft | Wert |
|-------------|------|
| Größe | 512 Byte |
| LBA | 0 |
| CHS | 0/0/1 |
| Ausrichtung | 512 Byte |
| Signatur | 0xAA55 |

---

# 22. MBR-Aufbau

Der physikalische Aufbau ist fest definiert.

```text
+--------------------------------------------------+
| Bootloader Code                     446 Byte     |
+--------------------------------------------------+
| Partitionseintrag 1                  16 Byte     |
+--------------------------------------------------+
| Partitionseintrag 2                  16 Byte     |
+--------------------------------------------------+
| Partitionseintrag 3                  16 Byte     |
+--------------------------------------------------+
| Partitionseintrag 4                  16 Byte     |
+--------------------------------------------------+
| Boot Signature 0x55AA                 2 Byte     |
+--------------------------------------------------+
```

Nova verwendet diesen Standard vollständig.

---

# 23. Bootsignatur

Jeder Nova-Bootsektor MUSS enden mit

```text
Offset 510

55 AA
```

Fehlt diese Signatur,

muss das BIOS den Datenträger als **nicht bootfähig** betrachten.

---

# 24. Speicherposition

Nach erfolgreichem BIOS-Boot befindet sich Stage 1 an

```text
0000:7C00
```

physikalisch

```text
0x00007C00
```

Von dort beginnt die Ausführung.

---

# 25. Speicherbelegung

Nach dem BIOS-Start ergibt sich typischerweise folgendes Layout.

```text
00000 ----------------

Interrupt Vector Table

00400 ----------------

BIOS Data Area

00500 ----------------

frei

07C00 ----------------

Nova Stage 1

07E00 ----------------

Stage 2

08000 ----------------

temporär frei

90000 ----------------

später Kernel

A0000 ----------------

Grafikspeicher
```

Stage 1 darf ausschließlich die markierten Bereiche verändern.

---

# 26. Bootsignaturprüfung

Das BIOS übernimmt die Prüfung der Signatur.

Stage 1 selbst muss diese Signatur nicht erneut prüfen.

Stattdessen prüft Stage 1 später ausschließlich,

ob Stage 2 erfolgreich geladen werden konnte.

---

# 27. Bootlaufwerk

Der Datenträger wird ausschließlich über

```
DL
```

identifiziert.

Typische Werte

```text
00h

Diskette A
```

```text
80h

erste Festplatte
```

```text
81h

zweite Festplatte
```

Nova speichert diesen Wert sofort.

---

# 28. Registerkonvention

Vor dem ersten BIOS-Aufruf dürfen keinerlei Register vorausgesetzt werden.

Stage 1 initialisiert daher immer

```asm
DS

ES

SS

SP
```

anschließend werden Register nur lokal verwendet.

---

# 29. BIOS Interrupts

Stage 1 darf ausschließlich BIOS-Dienste verwenden.

Zugelassene Interrupts

| Interrupt | Zweck |
|-----------|-------|
| INT 10h | Video (optional Debug) |
| INT 13h | Datenträger |
| INT 16h | Tastatur (optional Debug) |
| INT 18h | BIOS Boot Fallback |
| INT 19h | BIOS Neustart |

Alle anderen Interrupts gelten als optional.

---

# 30. INT 13h

Der wichtigste BIOS-Dienst ist

```text
INT 13h
```

Nova verwendet ihn zum Lesen von Stage 2.

Unterstützte Funktionen

```text
02h

Read Sectors
```

```text
00h

Reset Disk
```

Später

```text
42h

Extended Read (LBA)
```

---

# 31. CHS-Unterstützung

Stage 1 MUSS mindestens CHS unterstützen.

Parameter

```
Cylinder

Head

Sector
```

Die Umrechnung erfolgt ausschließlich innerhalb des Stage-1-Codes.

---

# 32. LBA-Unterstützung

Falls vom BIOS verfügbar,

SOLL Stage 1 die INT-13h-Erweiterungen verwenden.

Dadurch werden

- große Festplatten
- SSDs
- NVMe-Controller mit BIOS-Unterstützung

besser unterstützt.

---

# 33. Datenträgerzugriff

Der Standardablauf lautet

```text
Boot Device

↓

Read Stage 2

↓

Carry Flag ?

├── Ja

↓

Retry

└── Nein

↓

Continue
```

---

# 34. Fehlererkennung

Nach jedem BIOS-Aufruf muss

```asm
JC
```

ausgewertet werden.

Carry Flag gesetzt

↓

Lesefehler

Carry Flag gelöscht

↓

weiter

---

# 35. Retry-Strategie

Nova verwendet standardmäßig

```text
3

Versuche
```

Algorithmus

```text
Read

↓

Fehler

↓

Reset Disk

↓

Read erneut

↓

Fehler

↓

Reset

↓

Read erneut

↓

Fehler

↓

Fatal Error
```

---

# 36. BIOS Reset

Vor jedem neuen Versuch wird

```text
INT 13h

AH = 00h
```

aufgerufen.

Dadurch wird der Controller zurückgesetzt.

---

# 37. Ladeadresse von Stage 2

Standardmäßig

```text
0000:7E00
```

physikalisch

```text
0x7E00
```

Diese Adresse darf in zukünftigen Versionen konfigurierbar werden.

---

# 38. Größe von Stage 2

Stage 1 besitzt keinerlei Wissen über interne Funktionen.

Es kennt ausschließlich

- Startsektor

- Anzahl Sektoren

Diese Informationen werden beim Build erzeugt.

---

# 39. Übergabe

Nach erfolgreichem Laden erfolgt

```asm
jmp 0x0000:0x7E00
```

Alternativ

```asm
push 0

push 0x7E00

retf
```

Die konkrete Implementierung wird in der Referenzimplementierung festgelegt.

---

# 40. CPU-Zustand beim Sprung

Beim Sprung zu Stage 2 gilt

```text
Real Mode
```

```text
Interrupts deaktiviert
```

```text
Stack gültig
```

```text
DS = ES = SS = 0
```

```text
DL = Bootlaufwerk
```

Stage 2 darf sich auf diesen Zustand verlassen.

---

# 41. Datenübergabe

Stage 1 übergibt zunächst nur

```c
typedef struct
{
    uint8_t boot_drive;

} nova_stage1_context_t;
```

Alle weiteren Informationen sammelt Stage 2.

---

# 42. Debugmodus

Ein optionaler Debug-Build darf

über

```text
INT 10h
```

Statusmeldungen ausgeben.

Beispiele

```
Loading Stage 2...
```

```
Disk Error
```

```
Retry...
```

Im Release-Build bleiben diese Ausgaben deaktiviert.

---

# 43. Kompatibilität

Stage 1 muss funktionieren mit

- klassischen BIOS-Systemen
- IDE
- SATA
- BIOS-kompatiblen SSDs
- USB-Boot (BIOS)
- virtuellen Maschinen

Unter anderem

- QEMU
- Bochs
- VirtualBox
- VMware

---

# 44. Anforderungen an boot1.asm

Die Referenzimplementierung MUSS

✔ NASM-kompatibel sein

✔ reproduzierbar bauen

✔ keine selbstmodifizierenden Routinen enthalten

✔ klar kommentiert sein

✔ keine versteckten Konstanten besitzen

✔ alle BIOS-Aufrufe dokumentieren

---

# 45. Stage-2-Loader

Der Stage-2-Loader ist die wichtigste Aufgabe von Stage 1.

Er muss zuverlässig

- Stage 2 lokalisieren
- Stage 2 vollständig laden
- Stage 2 validieren
- Stage 2 starten

Alle anderen Funktionen sind nachrangig.

---

# 46. Ladealgorithmus

Der Standardablauf lautet

```text
Start

↓

Bootlaufwerk bestimmen

↓

Ladeparameter vorbereiten

↓

Sektoren lesen

↓

Lesefehler?

├── Ja

↓

Retry

│

↓

Fehler

│

└── Nein

↓

Integritätsprüfung

↓

OK?

├── Nein

↓

Fatal Error

└── Ja

↓

Jump Stage 2
```

---

# 47. Loader-Zustandsautomat

```text
BOOT

↓

INITIALIZE

↓

LOAD_STAGE2

↓

VERIFY_STAGE2

↓

READY

↓

JUMP

↓

DONE
```

Fehler führen in

```text
ERROR
```

---

# 48. Stage-2-Parameter

Stage 1 kennt ausschließlich

```c
typedef struct
{
    uint64_t start_sector;

    uint32_t sector_count;

    uint32_t load_address;

} nova_stage2_descriptor_t;
```

Weitere Informationen gehören nicht in Stage 1.

---

# 49. Standardparameter

Nova verwendet standardmäßig

```text
Startsektor

2
```

```text
Ladeadresse

0x7E00
```

```text
Sektoranzahl

Buildabhängig
```

Die Buildumgebung erzeugt diese Werte automatisch.

---

# 50. Buildprozess

Beim Erzeugen des Bootimages werden

```text
boot1.bin

↓

boot2.bin

↓

kernel

↓

disk.img
```

zusammengeführt.

Der Buildprozess schreibt

- Startsektor
- Anzahl Sektoren

direkt in Stage 1.

Stage 1 muss diese Werte **nicht berechnen**.

---

# 51. Keine Dateisysteme

Stage 1 besitzt **keine Dateisystemlogik**.

Es kennt

NICHT

- FAT12

- FAT16

- FAT32

- GPT

- MBR Partitionen

- Verzeichnisse

- Dateien

Es liest ausschließlich Rohsektoren.

---

# 52. Integritätsprüfung

Nach dem Laden SOLL Stage 1 prüfen,

ob Stage 2 plausibel aussieht.

Mindestens

- Größe

- Startadresse

- Ladebereich

Optional

- CRC32

- SHA-256

- digitale Signatur

Die vollständige Sicherheitsprüfung erfolgt später in Stage 2.

---

# 53. Speicherüberschneidung

Vor dem Sprung muss geprüft werden,

ob

```text
Stage 2

```

nicht

```text
Stage 1

```

überschreibt.

Ebenso dürfen

BIOS-Bereiche

nicht überschrieben werden.

---

# 54. Speicherprüfung

Der zulässige Bereich lautet

```text
0x7E00

↓

0x9FFFF
```

Innerhalb dieses Bereiches darf Stage 2 geladen werden.

Andere Bereiche sind verboten.

---

# 55. Fehlerklassen

Stage 1 unterscheidet

```text
INFO
```

```text
WARNING
```

```text
RECOVERABLE
```

```text
FATAL
```

Ein fataler Fehler beendet den Bootvorgang.

---

# 56. Fehlercodes

Empfohlene Fehlercodes

| Code | Bedeutung |
|------|-----------|
| 0x01 | Disk Read Error |
| 0x02 | Invalid Stage 2 |
| 0x03 | Memory Overflow |
| 0x04 | Invalid Boot Device |
| 0x05 | Retry Failed |
| 0x06 | BIOS Failure |
| 0x07 | Unknown Error |

Diese Fehlercodes werden später in Stage 2 übernommen.

---

# 57. Retrystrategie

Nova verwendet

```text
Retry

↓

Disk Reset

↓

Retry

↓

Disk Reset

↓

Retry

↓

Abort
```

Mehr als drei Wiederholungen sind standardmäßig nicht vorgesehen.

---

# 58. Disk Reset

Vor jedem erneuten Lesen wird

```asm
mov ah,00h

int 13h
```

aufgerufen.

Dies verhindert,

dass Controllerfehler dauerhaft bestehen bleiben.

---

# 59. Zeitverhalten

Stage 1 besitzt keine komplexe Zeitsteuerung.

Optional kann

zwischen zwei Retries

eine kurze BIOS-Wartezeit erfolgen.

---

# 60. Sprung zu Stage 2

Nach erfolgreichem Laden erfolgt

```asm
jmp 0000:7E00
```

Der Sprung darf erst erfolgen,

wenn

- alle Register vorbereitet sind

- der Stack gültig ist

- DL korrekt gesetzt wurde

---

# 61. Registerzustand

Beim Eintritt in Stage 2 gelten

```text
CS

0000
```

```text
IP

7E00
```

```text
DS

0000
```

```text
ES

0000
```

```text
SS

0000
```

```text
SP

7C00
```

```text
DL

Boot Device
```

Dieser Zustand ist Bestandteil der ABI.

---

# 62. Stage-1-Kontext

Der Kontext wird minimal gehalten.

```c
typedef struct
{
    uint8_t boot_drive;

    uint8_t retry_count;

    uint16_t reserved;

} nova_stage1_context_t;
```

---

# 63. Bootprotokoll

Stage 1 besitzt kein vollständiges Logging.

Optional können Statuscodes in einem reservierten Speicherbereich abgelegt werden.

Beispiel

```text
0x0500

↓

Boot Status
```

Dieser Bereich wird später von Stage 2 ausgewertet.

---

# 64. Referenzalgorithmus

```text
CPU Setup

↓

Save Boot Drive

↓

Load Stage 2

↓

Verify

↓

Retry?

↓

Jump Stage 2
```

Der Algorithmus soll bewusst einfach bleiben.

---

# 65. Build-Anforderungen

Der Buildprozess MUSS

- boot1.bin erzeugen
- exakt 512 Byte groß sein
- Bootsignatur schreiben
- Ladeparameter eintragen
- reproduzierbar sein

---

# 66. Optimierungen

Stage 1 darf

NICHT

optimiert werden durch

- Selbstmodifikation

- dynamische Speicherverwaltung

- Rekursion

- komplexe Algorithmen

Die höchste Priorität besitzt

**Zuverlässigkeit**.

---

# 67. Kompatibilitätsanforderungen

Stage 1 soll kompatibel sein mit

- klassischen BIOS-PCs
- QEMU
- Bochs
- VirtualBox
- VMware
- älteren Industrie-PCs
- Embedded x86-Systemen

---

# 68. Portabilität

Der Quellcode soll

- NASM-kompatibel
- leicht verständlich
- klar kommentiert
- architekturneutral dokumentiert

sein.

Eine spätere ARM- oder RISC-V-Implementierung erhält eine eigene Stage-1-Spezifikation.

---

# 69. Qualitätsanforderungen

Die Referenzimplementierung muss

✔ keine Compilerwarnungen erzeugen

✔ deterministisch bauen

✔ keine undefinierten BIOS-Aufrufe enthalten

✔ keine Magic Numbers ohne Dokumentation enthalten

✔ reproduzierbare Binärdateien erzeugen

---

# 70. Sicherheitsarchitektur

Obwohl Stage 1 möglichst klein gehalten wird, bildet er den Beginn der gesamten Vertrauenskette (Chain of Trust).

```text
Firmware

↓

Stage 1

↓

Stage 2

↓

Boot Core

↓

Kernel

↓

Nova OS
```

Jede nachfolgende Komponente baut auf der Integrität der vorherigen auf.

---

# 71. Sicherheitsziele

Stage 1 soll

- ausschließlich erwarteten Code ausführen
- keine Speicherbereiche außerhalb seines Zuständigkeitsbereiches verändern
- Bootparameter unverändert weitergeben
- keine unkontrollierten BIOS-Aufrufe durchführen
- deterministisch arbeiten
- reproduzierbare Binärdateien erzeugen

---

# 72. Integritätsmodell

Stage 1 besitzt standardmäßig nur eine einfache Integritätsprüfung.

```text
Firmware

↓

Stage 1

↓

Stage 2 vorhanden?

↓

Ja

↓

Übergabe
```

Erweiterte Signaturprüfungen erfolgen später in Stage 2.

---

# 73. Erweiterte Integritätsprüfung

Spätere Versionen können zusätzlich prüfen

- CRC32
- SHA-256
- SHA-512
- Digitale Signaturen
- TPM Measurements
- Secure Boot Status

Diese Funktionen dürfen Stage 1 jedoch nicht unnötig vergrößern.

---

# 74. Speicherintegrität

Vor jedem Schreibzugriff MUSS geprüft werden, ob

- Zieladresse gültig
- Speicherbereich vorhanden
- keine Überschneidung mit Stage 1
- keine BIOS-Daten überschrieben werden

Speicher außerhalb des erlaubten Bereiches darf niemals beschrieben werden.

---

# 75. BIOS-Kompatibilität

Stage 1 MUSS sich ausschließlich auf dokumentierte BIOS-Funktionen verlassen.

Nicht zulässig sind

- BIOS-Erweiterungen unbekannter Hersteller
- nicht dokumentierte Register
- undefiniertes Verhalten

---

# 76. Fehlerklassen

Fehler werden in vier Klassen eingeteilt.

```text
INFO

↓

WARNING

↓

RECOVERABLE

↓

FATAL
```

Nur RECOVERABLE-Fehler dürfen automatisch behandelt werden.

---

# 77. Recoverystrategie

Bei einem behebbaren Fehler gilt

```text
Disk Error

↓

Reset Controller

↓

Retry

↓

Success

↓

Continue
```

Erst wenn alle Versuche scheitern,

endet der Bootvorgang.

---

# 78. BIOS-Fallback

Kann Stage 1 Stage 2 nicht laden,

soll optional einer der folgenden Wege genutzt werden.

```text
Retry
```

↓

```text
BIOS Boot Retry
```

↓

```text
INT 18h
```

oder

```text
INT 19h
```

Welcher Pfad verwendet wird,

legt NPSPEC-BIOSBOOT-0001 fest.

---

# 79. Recovery-Code

Stage 1 besitzt selbst keine Recoveryumgebung.

Es kann lediglich

- Fehler melden
- Boot stoppen
- BIOS erneut starten
- Stage 2 erneut laden

Alle eigentlichen Recoveryfunktionen gehören Stage 2.

---

# 80. Bootdiagnose

Optional darf ein Debugbuild Informationen ausgeben.

Beispiele

```text
Nova Stage 1
```

```text
Loading Stage 2...
```

```text
Retry 2/3
```

```text
Jump Stage 2
```

Diese Ausgaben dürfen im Releasebuild vollständig entfernt werden.

---

# 81. Logging

Stage 1 besitzt kein vollständiges Logsystem.

Es darf jedoch einfache Statusinformationen hinterlassen.

Beispiel

```text
0x0500

↓

Bootstatus
```

oder

```text
0x0600

↓

Fehlercode
```

Stage 2 kann diese Informationen später übernehmen.

---

# 82. Fehlercodes

Empfohlenes Format

```c
typedef enum
{
    NOVA_STAGE1_OK = 0,

    NOVA_STAGE1_DISK_ERROR,

    NOVA_STAGE1_RETRY,

    NOVA_STAGE1_INVALID_STAGE2,

    NOVA_STAGE1_MEMORY_ERROR,

    NOVA_STAGE1_FATAL_ERROR

} nova_stage1_error_t;
```

---

# 83. Panicstrategie

Stage 1 besitzt noch keinen grafischen Panic Screen.

Stattdessen erfolgt

```text
Fehler

↓

Boot stoppen

↓

optional BIOS Restart
```

Grafische Fehlermeldungen gehören Stage 2.

---

# 84. Debugmodus

Debugbuilds dürfen

- Register ausgeben
- Sektornummern anzeigen
- Retryzähler anzeigen
- BIOS-Status anzeigen

Releasebuilds enthalten diese Funktionen nicht.

---

# 85. Determinismus

Stage 1 darf niemals

- Zufallszahlen erzeugen
- Zeitabhängigkeiten besitzen
- asynchron arbeiten
- Threads verwenden

Der Ablauf muss immer identisch sein.

---

# 86. Erweiterbarkeit

Neue Funktionen dürfen Stage 1 nicht unnötig vergrößern.

Neue Features gehören grundsätzlich

- Stage 2
- Boot Core
- Kernel

Nur absolut notwendige Änderungen sind zulässig.

---

# 87. Secure Boot

Stage 1 selbst besitzt keine vollständige Secure-Boot-Unterstützung.

Langfristig kann jedoch geprüft werden,

ob

- Firmware Secure Boot aktiv

- TPM vorhanden

- Nova Trust Store aktiv

ist.

Die eigentliche Verifikation erfolgt später.

---

# 88. Measured Boot

Optional kann Stage 1

Messwerte an Stage 2 weiterreichen.

Beispiel

```text
Firmware Version

↓

Boot Device

↓

Boot Timestamp
```

Die TPM-Erweiterung erfolgt später.

---

# 89. Selbstheilung

Stage 1 besitzt keine Selbstheilung.

Es kann lediglich

- erneutes Lesen versuchen
- BIOS zurücksetzen
- Stage 2 erneut laden

Die eigentliche Selbstheilung gehört

NPSPEC-BOOTSELFHEAL-0001.

---

# 90. Wartbarkeit

Der gesamte Quellcode MUSS

- klar kommentiert
- modular
- nachvollziehbar
- reproduzierbar

sein.

Jeder BIOS-Aufruf erhält einen erklärenden Kommentar.

---

# 91. Coding Standard

Empfohlen

```asm
; ----------------------------------
; Load Stage 2
; ----------------------------------
```

Jede Funktion beginnt mit

- Zweck
- Register
- Ein-/Ausgabe
- Nebeneffekte

---

# 92. Buildanforderungen

Der Buildprozess MUSS prüfen

- Binärgröße

- Bootsignatur

- Sektoranzahl

- CRC

- Reproduzierbarkeit

Fehlerhafte Bootloader dürfen nicht ausgeliefert werden.

---

# 93. Zukunft

Langfristig bleibt Stage 1 nahezu unverändert.

Die meisten Weiterentwicklungen erfolgen

- Stage 2
- Boot Core
- Recovery
- Kernel Loader

Dadurch bleibt Stage 1 über viele Jahre stabil.

---

# 94. Öffentliche ABI

Stage 1 besitzt eine bewusst sehr kleine ABI.

Die einzige garantierte Information ist das Bootlaufwerk.

## x86 Real Mode

Beim Eintritt in Stage 2 gilt:

| Register | Bedeutung |
|----------|-----------|
| CS | 0x0000 |
| IP | 0x7E00 |
| DS | 0x0000 |
| ES | 0x0000 |
| SS | 0x0000 |
| SP | 0x7C00 |
| DL | Bootlaufwerk |

Alle anderen Register gelten als undefiniert und dürfen von Stage 2 nicht vorausgesetzt werden.

---

# 95. Stage-1-Kontext

Stage 1 übergibt ausschließlich minimale Informationen.

```c
typedef struct nova_stage1_context
{
    uint8_t boot_drive;

    uint8_t retry_count;

    uint8_t flags;

    uint8_t reserved;

} nova_stage1_context_t;
```

Die vollständige `nova_boot_info_t` wird erst in Stage 2 aufgebaut.

---

# 96. Interne Daten

Empfohlene interne Variablen

```asm
BootDrive      db 0

RetryCounter   db 0

Stage2Sector   dw 2

Stage2Count    dw 8
```

Alle Konstanten müssen dokumentiert sein.

---

# 97. Referenzimplementierung

Der empfohlene Programmablauf lautet:

```text
Firmware

↓

Stage 1 Start

↓

Register initialisieren

↓

Stack initialisieren

↓

Bootlaufwerk sichern

↓

Stage 2 laden

↓

Erfolg?

├── Nein

↓

Retry

↓

BIOS Reset

↓

Retry

↓

Fehler

└── Ja

↓

Jump Stage 2
```

Dieser Ablauf bildet die Referenzimplementierung.

---

# 98. Zustandsautomat

```text
RESET

↓

INITIALIZE

↓

LOAD

↓

VERIFY

↓

READY

↓

TRANSFER
```

Fehler führen nach

```text
ERROR

↓

STOP
```

---

# 99. Buildprozess

Der Buildprozess besteht aus:

```text
boot1.asm

↓

NASM

↓

boot1.bin

↓

Bootsignatur

↓

Größenprüfung

↓

CRC

↓

Integration in disk.img
```

---

# 100. Buildregeln

Der Build MUSS prüfen:

- exakt 512 Byte
- Signatur vorhanden
- keine Warnungen
- reproduzierbarer Build
- gültige Ladeparameter
- keine ungültigen Offsets
- Stage-2-Größe passt

---

# 101. Qualitätsanforderungen

Der Stage-1-Code muss:

✔ assemblerfreundlich sein

✔ vollständig kommentiert sein

✔ reproduzierbar sein

✔ keine Magic Numbers ohne Dokumentation enthalten

✔ keine unnötigen BIOS-Aufrufe besitzen

✔ deterministisch arbeiten

---

# 102. Teststrategie

Jede Änderung an Stage 1 muss automatisiert getestet werden.

Mindestens:

### Boottests

- BIOS startet
- MBR wird erkannt
- Signatur korrekt
- Stage 2 wird geladen

---

### Fehlertests

- Datenträger entfernt

- fehlerhafte Sektoren

- falsches Bootlaufwerk

- Stage 2 fehlt

- BIOS Read Error

---

### Regressionstests

- Buildgröße

- Registerzustand

- Ladeadresse

- Retryfunktion

---

### Hardwaretests

- QEMU

- Bochs

- VirtualBox

- VMware

- ältere BIOS-PCs

- moderne Mainboards mit CSM

---

# 103. Leistungsanforderungen

Stage 1 soll

- möglichst wenige BIOS-Aufrufe besitzen
- unnötige Speicherzugriffe vermeiden
- keine Schleifen ohne Abbruch besitzen

Der Code wird auf Lesbarkeit optimiert, nicht auf Mikrooptimierungen.

---

# 104. Dokumentation

Jede Funktion erhält mindestens:

```asm
;-----------------------------------------------------
; Funktion:
;
; Zweck:
;
; Eingabe:
;
; Ausgabe:
;
; Verändert:
;
; Fehler:
;-----------------------------------------------------
```

Dadurch bleibt der Bootloader langfristig wartbar.

---

# 105. Referenzprojektstruktur

```text
boot/
├── boot1.asm
├── boot1.inc
├── boot_error.inc
├── boot_memory.inc
├── boot_disk.inc
└── boot_constants.inc
```

Stage 1 soll möglichst modular aufgebaut sein, obwohl daraus nur eine Binärdatei erzeugt wird.

---

# 106. Beziehungen zu anderen NPSPECs

Stage 1 bildet die Grundlage für:

- NPSPEC-BOOTSTAGE2-0001
- NPSPEC-BOOTLOADER-0001
- NPSPEC-BIOSBOOT-0001
- NPSPEC-BOOTMEMORY-0001
- NPSPEC-BOOTSECURITY-0001
- NPSPEC-BOOTRECOVERY-0001
- NPSPEC-BOOTHANDOFF-0001

Änderungen an Stage 1 dürfen diese Spezifikationen nicht unbeabsichtigt verletzen.

---

# 107. Implementierungsphasen

## Phase 1

- Bootlaufwerk sichern
- Stage 2 laden
- Sprung

---

## Phase 2

- Retry
- BIOS Reset
- Fehlercodes

---

## Phase 3

- CRC
- Integritätsprüfung
- Debugmeldungen

---

## Phase 4

- Buildvalidierung
- Regressionstests
- Optimierung

---

## Phase 5

- Langfristige Wartung

---

# 108. Akzeptanzkriterien

NPSPEC-BOOTSTAGE1-0001 gilt als umgesetzt, wenn:

1. Stage 1 exakt 512 Byte groß ist.
2. Die Bootsignatur `0x55AA` korrekt geschrieben wird.
3. Der Prozessorzustand definiert initialisiert wird.
4. Das Bootlaufwerk zuverlässig gespeichert wird.
5. Stage 2 erfolgreich von einem BIOS-Datenträger geladen wird.
6. Lesefehler erkannt und bis zu drei Mal erneut versucht werden.
7. Nach erfolgreichem Laden kontrolliert an Stage 2 übergeben wird.
8. Der Registerzustand der ABI entspricht.
9. Keine BIOS-Datenbereiche überschrieben werden.
10. Der Bootloader auf QEMU und mindestens einem realen BIOS-System erfolgreich startet.

---

# 109. Zusammenfassung

Der **Nova Stage-1-Bootloader** ist bewusst minimal gehalten.

Er übernimmt ausschließlich die Aufgaben, die zwingend notwendig sind, um die Kontrolle von der Firmware auf den erweiterten Nova-Bootloader (Stage 2) zu übertragen.

Durch die strikte Trennung zwischen Stage 1 und Stage 2 bleibt der frühe Bootpfad:

- klein
- robust
- deterministisch
- leicht überprüfbar
- langfristig wartbar

Diese Architektur reduziert die Angriffsfläche und erleichtert die Portierung auf zukünftige Plattformen.

---

# 110. Langfristige Vision

Der Stage-1-Bootloader soll sich über viele Jahre kaum verändern.

Neue Funktionen werden grundsätzlich nicht in Stage 1 integriert, sondern in Stage 2 oder den Nova Boot Core verlagert.

Langfristige Ziele:

- maximale Stabilität
- reproduzierbare Builds
- vollständige BIOS-Kompatibilität
- klar definierte ABI
- minimale Codegröße
- einfacher Sicherheitsnachweis
- Wartbarkeit über mehrere Nova-Generationen
- gemeinsame Architektur mit zukünftigen UEFI-, ARM64- und RISC-V-Bootpfaden

Stage 1 bildet damit das unveränderliche Fundament der Nova-Bootplattform: klein genug, um vollständig verstanden und überprüft werden zu können, und gleichzeitig robust genug, um den Übergang von der Firmware in das Nova-Ökosystem zuverlässig einzuleiten.
