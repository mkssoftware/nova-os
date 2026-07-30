# ADR-BOOT-1013: Stage-1 bleibt minimal

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1013
* **Titel:** Stage-1 bleibt minimal
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** NovaOS BIOS-Bootpfad, Stage-1-Bootloader und Boot-Medium-Layout
* **Verantwortlich:** Nova Platform Architecture
* **Ersetzt:** Keine
* **Ersetzt durch:** Keine

## Referenzen

* ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
* ADR-BOOT-0004 – ELF als Kernel-Format
* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* NPSPEC-BOOTFS-0001
* NPSPEC-KERNELLOADER-0001
* NPSPEC-BOOTHANDOFF-0001
* NPSPEC-KERNELENTRY-0001
* NPSPEC-BOOTERROR-0001

---

## 1. Kontext

Der BIOS-Bootpfad von NovaOS beginnt mit einem kleinen Programm, das durch die Firmware aus dem ersten bootfähigen Sektor eines Datenträgers geladen wird.

Diese erste Bootstufe wird als:

```text
Stage-1
```

bezeichnet.

Im klassischen BIOS-Bootmodell wird der erste Sektor typischerweise an folgende Adresse geladen:

```text
0x0000:0x7C00
```

Die tatsächlich verfügbare Größe ist stark begrenzt.

Bei einem klassischen Master Boot Record stehen insgesamt 512 Byte zur Verfügung. Davon werden üblicherweise verwendet:

```text
446 Byte    Bootcode

64 Byte     Partitionstabelle

2 Byte      Bootsignatur
```

Die Bootsignatur lautet:

```text
0x55 0xAA
```

NovaOS unterstützt sowohl partitionierte als auch eigens kontrollierte Bootmedien. Unabhängig vom konkreten Layout bleibt Stage-1 jedoch die kleinste und früheste ausführbare Komponente der Bootkette.

Stage-1 wird in einer besonders eingeschränkten Umgebung ausgeführt:

* 16-Bit Real Mode
* keine initialisierte Runtime
* kein Heap
* kein vollständiger Stack
* keine Speicherverwaltung
* keine Treiberarchitektur
* keine Dateisystembibliothek
* keine standardisierte Fehlerbehandlung
* BIOS-Dienste mit hardwareabhängigem Verhalten
* begrenzter verfügbarer Speicher
* keine garantierte CPU-Konfiguration außerhalb des BIOS-Standards

Jede zusätzliche Funktion in Stage-1 erhöht:

* die Codegröße,
* die Komplexität,
* die Fehlerwahrscheinlichkeit,
* den Testaufwand,
* die Hardwareabhängigkeit,
* und das Risiko eines nicht mehr startfähigen Systems.

Daher muss klar festgelegt werden, welche Aufgaben Stage-1 übernehmen darf.

---

## 2. Problemstellung

Es muss entschieden werden, ob Stage-1 bereits umfangreiche Bootfunktionen implementiert oder ausschließlich eine nachfolgende Bootstufe lädt.

Mögliche Funktionen für Stage-1 wären unter anderem:

* Dateisystemzugriff
* Bootmenü
* Grafikmodus
* Mausunterstützung
* Tastatursteuerung
* Kernel Loading
* ELF Parsing
* Speichererkennung
* Recovery
* Kryptografie
* Integritätsprüfung
* Konfigurationsverarbeitung
* Fehlerbildschirm
* Mehrfachboot
* Netzwerkboot
* Entschlüsselung
* Komprimierung

Eine umfangreiche Stage-1 könnte theoretisch früh mehr Kontrolle übernehmen.

Dem stehen jedoch erhebliche Nachteile gegenüber:

* extrem knapper Speicherplatz
* schwer überprüfbarer Assemblercode
* hohe Abhängigkeit von BIOS-Eigenheiten
* eingeschränkte Diagnostik
* schwierige Erweiterbarkeit
* hohes Risiko regressiver Bootfehler
* starke Kopplung zwischen Bootmedium und Bootlogik

NovaOS benötigt eine langfristig wartbare Bootarchitektur. Deshalb darf Stage-1 nicht schrittweise zu einem vollständigen Bootloader anwachsen.

---

## 3. Entscheidung

Stage-1 bleibt dauerhaft minimal.

Stage-1 besitzt ausschließlich die Aufgabe:

```text
eine definierte nachfolgende Bootstufe zu lokalisieren,
in den Arbeitsspeicher zu laden,
grundlegend zu validieren
und an diese zu übergeben.
```

Stage-1 darf keine allgemeine Bootlogik, keine Benutzeroberfläche und keine Kernel-Ladefunktion implementieren.

Die eigentliche Bootlogik wird vollständig in spätere Stufen verlagert.

---

## 4. Verbindliche Kernaufgaben

Stage-1 muss ausschließlich folgende Aufgaben erfüllen:

1. Minimalen Prozessorzustand herstellen.
2. Segmentregister initialisieren.
3. Einen gültigen Stack einrichten.
4. Das BIOS-Bootlaufwerk sichern.
5. Die festgelegte Stage-2-Position bestimmen.
6. Stage-2 in einen definierten Speicherbereich laden.
7. Eine minimale Validierung von Stage-2 durchführen.
8. Notwendige Übergabeinformationen bereitstellen.
9. Die Kontrolle an Stage-2 übertragen.
10. Bei einem Fehler kontrolliert anhalten oder einen minimalen Fehlercode ausgeben.

Alle weiteren Aufgaben sind Stage-1 untersagt, sofern dieses ADR nicht ausdrücklich geändert wird.

---

## 5. Minimalitätsprinzip

Minimalität bedeutet nicht ausschließlich minimale Byteanzahl.

Stage-1 soll gleichzeitig:

* klein,
* deterministisch,
* robust,
* überschaubar,
* testbar,
* und hardwarekompatibel

sein.

Eine kürzere Implementierung ist nicht automatisch besser, wenn sie:

* undefiniertes Verhalten verwendet,
* Validierungen entfernt,
* BIOS-Rückgabewerte ignoriert,
* nicht deterministisch arbeitet,
* oder bekannte Hardwareprobleme erzeugt.

Stage-1 muss so klein wie sinnvoll und so robust wie erforderlich bleiben.

---

## 6. Verantwortungsgrenze

Die Verantwortung von Stage-1 endet unmittelbar nach der erfolgreichen Übergabe an Stage-2.

```text
BIOS

↓

Stage-1

↓

Stage-2

↓

Kernel Loader

↓

Kernel
```

Stage-1 kennt nicht:

* den vollständigen Kernelpfad,
* das Kernel-Dateiformat,
* den Kernel-Einstiegspunkt,
* das Bootmenü,
* das Recovery-System,
* Benutzerprofile,
* Grafikthemen,
* Treiber,
* Netzwerkprotokolle,
* oder die interne Kernelarchitektur.

Stage-1 kennt ausschließlich die Informationen, die zum Laden von Stage-2 notwendig sind.

---

## 7. Zulässige Funktionen

Folgende Funktionen sind in Stage-1 zulässig:

### 7.1 Prozessorinitialisierung

Stage-1 darf:

* Interrupts temporär deaktivieren,
* Segmentregister initialisieren,
* den Direction Flag löschen,
* einen Stack einrichten,
* und einen definierten Registerzustand herstellen.

### 7.2 Bootlaufwerk sichern

Das von BIOS in:

```text
DL
```

bereitgestellte Bootlaufwerk muss gesichert werden.

Dieses Laufwerk wird an Stage-2 übergeben.

### 7.3 Datenträgerzugriff

Stage-1 darf BIOS-Datenträgerdienste verwenden, um Stage-2 zu laden.

Zulässige Mechanismen:

```text
INT 13h CHS

INT 13h Extensions / LBA
```

Welche Methode verwendet wird, hängt von der jeweiligen Stage-1-Variante und den Plattformanforderungen ab.

### 7.4 Laden von Stage-2

Stage-1 darf einen oder mehrere fest definierte Sektoren laden.

Die Ladeadresse muss statisch oder über minimale Bootmetadaten bestimmt sein.

### 7.5 Wiederholungsversuche

Stage-1 darf fehlgeschlagene Leseoperationen begrenzt wiederholen.

### 7.6 Laufwerksreset

Vor einem erneuten Leseversuch darf Stage-1 das BIOS-Laufwerk zurücksetzen.

### 7.7 Minimalvalidierung

Stage-1 darf Stage-2 anhand einer einfachen Kennung prüfen.

Beispiele:

* Magic Number
* feste Signatur
* Größenfeld
* einfache Prüfsumme

### 7.8 Minimaldiagnostik

Stage-1 darf einen kurzen Fehlercode oder eine kurze ASCII-Meldung ausgeben.

### 7.9 Kontrolltransfer

Stage-1 darf per Far Jump oder geeignetem Kontrolltransfer an Stage-2 übergeben.

---

## 8. Unzulässige Funktionen

Stage-1 darf insbesondere folgende Funktionen nicht implementieren:

* grafische Benutzeroberfläche
* Bootmenü
* Mausunterstützung
* vollständige Tastaturnavigation
* VBE-Modusauswahl
* UEFI-Unterstützung
* ELF-Parsing
* Kernel Loading
* Dateisystemparser
* vollständige BootFS-Auswertung
* Netzwerkzugriff
* HTTP
* TFTP
* USB-Treiber
* NVMe-Treiber
* AHCI-Treiber
* Kryptografie
* Signaturprüfung komplexer Formate
* TPM-Kommunikation
* Speicherkomprimierung
* Kerneldekomprimierung
* Recovery-Menü
* Betriebssystemauswahl
* Partitionierung
* Formatierung
* Verschlüsselung
* Benutzeranmeldung
* Konfigurationseditor
* umfangreiche Hardwareerkennung
* ACPI-Auswertung
* SMBIOS-Auswertung
* Multiboot-Protokoll
* Nova Boot-Handoff
* Long-Mode-Aktivierung
* vollständiges Paging
* Treiberverwaltung

Diese Aufgaben gehören in Stage-2 oder spätere Bootkomponenten.

---

## 9. Kein Dateisystemparser

Stage-1 implementiert keinen allgemeinen Dateisystemparser.

Insbesondere werden nicht direkt unterstützt:

```text
FAT

FAT32

ext2

ext4

NovaFS

NTFS

ISO 9660
```

Ein Dateisystemparser würde:

* erheblichen Codeplatz benötigen,
* Fehlerfälle vervielfachen,
* die Bootarchitektur an ein konkretes Dateisystem koppeln,
* und den Testaufwand deutlich erhöhen.

Stage-1 lädt Stage-2 daher über:

* feste Sektoren,
* eine bekannte Blockliste,
* eine kleine Stage-2-Locatortabelle,
* oder ein vergleichbar kompaktes Verfahren.

Die konkrete Ablagestrategie wird durch BootFS- und Image-Build-Spezifikationen definiert.

---

## 10. Kein Kernel Loading

Stage-1 lädt niemals direkt den NovaOS-Kernel.

Der Kernel kann:

* größer als der direkt adressierbare Stage-1-Ladebereich sein,
* fragmentiert gespeichert sein,
* als ELF-Datei vorliegen,
* komprimiert sein,
* signiert sein,
* oder zusätzliche Module benötigen.

Diese Aufgaben erfordern eine wesentlich leistungsfähigere Umgebung.

Daher gilt verbindlich:

```text
Stage-1 lädt Stage-2.

Stage-2 oder eine spätere Loader-Stufe lädt den Kernel.
```

---

## 11. Kein ELF-Parser

Obwohl NovaOS ELF als Kernel-Format verwendet, darf Stage-1 kein ELF parsen.

Die ELF-Verarbeitung erfolgt gemäß:

```text
ADR-BOOT-0004
```

in einer späteren Bootstufe.

Stage-1 behandelt Stage-2 grundsätzlich als einfach ladbares Bootartefakt und nicht als vollständiges ausführbares Standardformat.

---

## 12. Stage-2-Ablage

Stage-2 muss so gespeichert werden, dass Stage-1 sie mit minimalem Aufwand laden kann.

Zulässige Varianten sind:

### 12.1 Direkt nach Stage-1

Stage-2 befindet sich in unmittelbar folgenden Sektoren.

```text
Sektor 0      Stage-1

Sektor 1..N   Stage-2
```

Vorteile:

* extrem einfache Ladeoperation
* keine Suchlogik
* deterministische Position

### 12.2 Reservierter Bootbereich

Stage-2 befindet sich in einem fest reservierten Bootbereich.

### 12.3 Blockliste

Stage-1 enthält oder liest eine kleine Liste physischer Blockbereiche.

### 12.4 Locator-Struktur

Eine kompakte Metadatenstruktur beschreibt:

* Start-LBA
* Sektoranzahl
* Ladeadresse
* Prüfsumme
* Einstiegsoffset

Die Locator-Struktur muss klein und einfach validierbar bleiben.

---

## 13. Keine Fragmentierung

Die bevorzugte Stage-2-Ablage ist zusammenhängend.

Stage-1 soll Stage-2 nicht aus einer großen Anzahl verteilter Fragmente zusammensetzen müssen.

Empfohlene Regel:

```text
Stage-2 wird in einem zusammenhängenden Block gespeichert.
```

Eine kleine Anzahl von Extents darf unterstützt werden, wenn dies für das Bootmedium zwingend erforderlich ist.

---

## 14. Stage-2-Größe

Stage-1 darf eine definierte maximale Stage-2-Größe unterstützen.

Diese Grenze wird durch folgende Faktoren bestimmt:

* verfügbarer Real-Mode-Speicher
* Ladeadresse
* Bootmedium
* BIOS-Transfergrenzen
* Segmentgrenzen
* Implementierung des Lesealgorithmus

Beispiel:

```text
Maximale Stage-2-Größe:

64 KiB

128 KiB

oder architekturabhängig mehr
```

Die konkrete Grenze ist Bestandteil der jeweiligen Bootloader-Implementierung.

Stage-1 muss Größenangaben vor dem Laden validieren.

---

## 15. Speicherlayout

Stage-1 muss ein fest definiertes frühes Speicherlayout einhalten.

Beispiel:

```text
0x00000500    Übergabestruktur

0x00007C00    Stage-1

0x00007E00    Stage-2

0x00090000    Früher Stack

0x000A0000    Video- und BIOS-Bereich
```

Das konkrete Layout kann abweichen, muss jedoch dokumentiert und kollisionsfrei sein.

Stage-1 darf insbesondere nicht überschreiben:

* BIOS Data Area
* Interrupt Vector Table
* eigene Bootcodebereiche
* Übergabedaten
* reservierte Firmwarebereiche
* Zielbereich von Stage-2 außerhalb der festgelegten Region

---

## 16. Segmentgrenzen

Beim BIOS-Lesen müssen mögliche Segmentgrenzen berücksichtigt werden.

Ein einzelner Transfer darf nicht unkontrolliert eine 64-KiB-Grenze überschreiten, wenn die verwendete BIOS-Funktion oder Zieladressierung dies nicht unterstützt.

Stage-1 muss gegebenenfalls:

* Transfers aufteilen,
* Segment und Offset aktualisieren,
* und die maximale Sektoranzahl pro Operation begrenzen.

---

## 17. BIOS-Leseverfahren

Stage-1 soll bevorzugt BIOS INT-13h-Erweiterungen verwenden, sofern verfügbar.

### 17.1 LBA-Verfahren

Das LBA-Verfahren verwendet einen Disk Address Packet.

Beispielhafte Struktur:

```text
Size

Reserved

Sector Count

Target Offset

Target Segment

Start LBA
```

Vorteile:

* keine CHS-Geometrieberechnung
* Unterstützung großer Datenträger
* robustere Adressierung

### 17.2 CHS-Fallback

Für ältere BIOS-Implementierungen darf ein CHS-Fallback verwendet werden.

Der CHS-Code muss klein und vollständig begrenzt bleiben.

---

## 18. BIOS-Erweiterungserkennung

Vor der Verwendung von LBA muss Stage-1 prüfen, ob INT-13h-Erweiterungen unterstützt werden.

Falls nicht, darf:

* auf CHS zurückgefallen,
* eine alternative Stage-1-Variante verwendet,
* oder kontrolliert abgebrochen werden.

Ein nicht unterstütztes BIOS darf nicht zu undefiniertem Verhalten führen.

---

## 19. Leseversuche

Fehlgeschlagene BIOS-Leseoperationen dürfen wiederholt werden.

Empfohlene maximale Anzahl:

```text
3 Versuche
```

Vor einem erneuten Versuch darf ein Laufwerksreset erfolgen.

Nach Ausschöpfen der Versuche muss Stage-1:

* einen Fehlercode ausgeben,
* und den Bootvorgang kontrolliert anhalten.

Unbegrenzte Wiederholungen sind nicht zulässig.

---

## 20. Stage-2-Validierung

Stage-1 muss mindestens prüfen, ob der geladene Stage-2-Code grundsätzlich plausibel ist.

Eine minimale Stage-2-Kopfstruktur kann enthalten:

```text
Magic

Header Size

Image Size

Entry Offset

Version

Checksum
```

Beispiel:

```text
"NOVA2"
```

Die Validierung soll erkennen:

* falsche Sektoren,
* beschädigte Images,
* falsche Stage-2-Versionen,
* unvollständige Lesevorgänge,
* und offensichtlich ungültige Einstiegspunkte.

---

## 21. Keine komplexe Kryptografie

Stage-1 führt keine vollständige kryptografische Signaturprüfung durch.

Gründe:

* Codegröße
* Speicherbedarf
* Rechenaufwand
* Komplexität
* fehlende Runtime-Unterstützung
* schwierige sichere Schlüsselverwaltung

Eine einfache Prüfsumme in Stage-1 dient ausschließlich der Erkennung zufälliger Beschädigungen.

Die kryptografische Vertrauensprüfung erfolgt in Stage-2 oder einer späteren Sicherheitsstufe.

---

## 22. Prüfsummen

Stage-1 darf eine kleine Prüfsumme verwenden.

Mögliche Verfahren:

```text
Additive Checksum

Fletcher Checksum

CRC32
```

CRC32 darf nur verwendet werden, wenn die Implementierung weiterhin klein und überprüfbar bleibt.

Eine Prüfsumme stellt keine Authentizitätsgarantie bereit.

---

## 23. Stage-2-Header

Eine mögliche minimale Stage-2-Struktur lautet:

```c
struct nova_stage2_header
{
    uint32_t magic;
    uint16_t header_size;
    uint16_t version;
    uint32_t image_size;
    uint32_t entry_offset;
    uint32_t checksum;
};
```

Die konkrete Binärstruktur wird in einer separaten Bootartefakt-Spezifikation festgelegt.

Stage-1 darf ausschließlich stabile, versionsgebundene Headerfelder auswerten.

---

## 24. Übergabe an Stage-2

Stage-1 muss mindestens folgende Informationen an Stage-2 übergeben:

* BIOS-Bootlaufwerk
* Startadresse von Stage-2
* optional verwendete Lademethode
* optional Stage-1-Fehlerstatus
* optional Adresse der Stage-2-Metadaten

Die Übergabe kann erfolgen über:

* Register
* feste Speicherstruktur
* oder eine Kombination aus beidem.

---

## 25. Registervertrag

Ein möglicher Übergabevertrag lautet:

```text
DL = BIOS-Bootlaufwerk

DS:SI = Adresse der Stage-1-Übergabestruktur
```

Alle weiteren Register müssen entweder:

* definiert,
* oder ausdrücklich als undefiniert dokumentiert

sein.

Stage-2 darf nicht von zufälligen Registerinhalten abhängig sein.

---

## 26. Übergabestruktur

Eine minimale Übergabestruktur kann enthalten:

```c
struct nova_stage1_handoff
{
    uint32_t magic;
    uint16_t version;
    uint8_t boot_drive;
    uint8_t load_method;
    uint32_t stage2_address;
    uint32_t stage2_size;
};
```

Diese Struktur ist nicht mit dem vollständigen Nova Boot-Handoff-Protokoll gleichzusetzen.

Sie dient ausschließlich der Übergabe zwischen Stage-1 und Stage-2.

---

## 27. Eigenständiges Stage-1-Handoff

Das Stage-1-Handoff ist ein internes Loader-Protokoll.

Es darf nicht direkt an den Kernel weitergereicht werden.

Stage-2 wandelt relevante Informationen später in das vollständige Nova Boot-Handoff-Format um.

```text
Stage-1-Handoff

↓

Stage-2

↓

Nova Boot-Handoff

↓

Kernel
```

---

## 28. Kontrolltransfer

Nach erfolgreichem Laden und Validieren von Stage-2 muss Stage-1:

1. den definierten Registerzustand herstellen,
2. die Übergabedaten bereitstellen,
3. den Einstiegspunkt berechnen,
4. und die Kontrolle übertragen.

Der Kontrolltransfer darf beispielsweise erfolgen durch:

```asm
jmp segment:offset
```

oder:

```asm
call segment:offset
```

Ein Rücksprung zu Stage-1 ist im Normalfall nicht vorgesehen.

---

## 29. Rückkehr aus Stage-2

Stage-2 darf im regulären Bootpfad nicht zu Stage-1 zurückkehren.

Eine Rückkehr gilt als Fehler.

Stage-1 darf für diesen Fall:

* einen Fehlercode ausgeben,
* den Prozessor anhalten,
* oder einen BIOS-Neustart auslösen.

---

## 30. Fehlerausgabe

Stage-1 verwendet ausschließlich minimale Fehlerausgaben.

Beispiele:

```text
E1

E2

Disk

Load

S2
```

Lange Texte, grafische Fehlerseiten und lokalisierte Meldungen gehören nicht in Stage-1.

---

## 31. Fehlercodes

Mindestens folgende Fehlerzustände sollen unterscheidbar sein:

```text
BOOT_STAGE1_INVALID_ENVIRONMENT

BOOT_STAGE1_DISK_RESET_FAILED

BOOT_STAGE1_READ_FAILED

BOOT_STAGE1_LBA_UNSUPPORTED

BOOT_STAGE1_INVALID_STAGE2_HEADER

BOOT_STAGE1_STAGE2_TOO_LARGE

BOOT_STAGE1_STAGE2_CHECKSUM_FAILED

BOOT_STAGE1_INVALID_ENTRY_POINT

BOOT_STAGE1_HANDOFF_FAILED

BOOT_STAGE1_STAGE2_RETURNED
```

Aufgrund der Größenbegrenzung darf Stage-1 intern kompaktere numerische Codes verwenden.

---

## 32. Minimaler Fehlercode

Eine mögliche sichtbare Zuordnung lautet:

```text
01  Datenträgerfehler

02  Stage-2 nicht gefunden

03  Stage-2 beschädigt

04  Stage-2 zu groß

05  ungültiger Einstiegspunkt
```

Die ausführliche Interpretation übernimmt die technische Dokumentation.

---

## 33. Kein grafischer Fehlerbildschirm

Stage-1 implementiert keinen grafischen Bootfehlerbildschirm.

Der in NPSPEC-BOOTERROR-0001 definierte Nova-Bootfehlerbildschirm wird erst in einer späteren Bootstufe angezeigt, sobald:

* Grafikinitialisierung,
* Schriftwiedergabe,
* Speicherverwaltung,
* und Eingabeverarbeitung

verfügbar sind.

Stage-1 beschränkt sich auf Textmodus oder minimale BIOS-Ausgabe.

---

## 34. Textausgabe

Falls Fehlertexte verwendet werden, darf Stage-1 BIOS INT-10h verwenden.

Die Ausgabe muss:

* kurz,
* ASCII-basiert,
* und optional

sein.

Ein Fehler in der Textausgabe darf den eigentlichen Fehlerpfad nicht weiter verschärfen.

---

## 35. Kein Logging-System

Stage-1 besitzt kein allgemeines Logging-System.

Es darf jedoch einen kompakten Statuswert in der Übergabestruktur oder an einer bekannten Speicheradresse hinterlegen.

Stage-2 kann diesen Wert später in das vollständige Bootprotokoll übernehmen.

---

## 36. Stack

Stage-1 muss vor Funktionsaufrufen oder Push-Operationen einen gültigen Stack einrichten.

Der Stackbereich muss:

* bekannt,
* ausgerichtet,
* kollisionsfrei,
* und ausreichend groß

sein.

Der Stack darf weder Stage-1 noch Stage-2 überschreiben.

---

## 37. Interruptzustand

Stage-1 muss den Interruptzustand kontrolliert behandeln.

Vor kritischen Initialisierungen dürfen Interrupts deaktiviert werden.

Vor BIOS-Aufrufen muss der erwartete BIOS-kompatible Zustand wiederhergestellt sein.

Stage-1 darf keine eigene dauerhafte Interruptarchitektur implementieren.

---

## 38. A20-Gate

Stage-1 soll das A20-Gate nicht aktivieren, sofern dies für das Laden von Stage-2 nicht zwingend erforderlich ist.

Die Aktivierung des A20-Gates gehört grundsätzlich in Stage-2.

Eine Ausnahme ist zulässig, wenn Stage-2 oberhalb der 1-MiB-Grenze geladen werden muss.

Die bevorzugte Architektur lädt Stage-2 jedoch unterhalb von 1 MiB.

---

## 39. Protected Mode

Stage-1 wechselt nicht in den Protected Mode.

Die Umschaltung in:

```text
Protected Mode
```

oder:

```text
Long Mode
```

erfolgt in Stage-2 oder einer späteren Architekturstufe.

Dadurch bleibt Stage-1 unabhängig von:

* GDT-Konfiguration,
* Paging,
* CPU-Moduswechsel,
* und architekturspezifischer Kernelvorbereitung.

---

## 40. Hardwareerkennung

Stage-1 führt keine allgemeine Hardwareerkennung durch.

Zulässig ist ausschließlich die Erkennung, die unmittelbar zum Laden von Stage-2 erforderlich ist.

Beispiele:

* BIOS-LBA-Unterstützung
* Laufwerksnummer
* gegebenenfalls BIOS-Laufwerksgeometrie

Nicht zulässig sind:

* CPU-Feature-Enumeration
* PCI-Scan
* RAM-Erkennung
* ACPI-Auswertung
* Grafikadaptererkennung
* USB-Erkennung

---

## 41. Partitionstabellen

Stage-1 darf minimale Partitionsinformationen verwenden, wenn dies für die Stage-2-Lokalisierung erforderlich ist.

Stage-1 darf jedoch keinen vollständigen Partitionsmanager implementieren.

Bei MBR-Medien darf Stage-1 beispielsweise:

* einen festen Partitionseintrag verwenden,
* eine aktive Partition erkennen,
* oder einen vorgegebenen LBA-Startwert auswerten.

Komplexe GPT-Auswertung gehört in spätere Bootstufen.

---

## 42. GPT und Protective MBR

Bei GPT-basierten BIOS-Bootmedien soll Stage-1 weiterhin über einen dafür reservierten BIOS-Bootbereich oder eine BIOS-Bootpartition Stage-2 laden.

Stage-1 soll keinen vollständigen GPT-Parser enthalten.

Die Image-Erstellung muss sicherstellen, dass die Stage-2-Position für Stage-1 eindeutig verfügbar ist.

---

## 43. Mehrere Stage-1-Varianten

NovaOS darf mehrere spezialisierte Stage-1-Varianten verwenden.

Beispiele:

```text
MBR Stage-1

Partition Boot Record Stage-1

El-Torito Stage-1

Floppy Stage-1

Recovery-Medium Stage-1
```

Alle Varianten müssen dieselbe Architekturentscheidung einhalten:

```text
Stage-1 bleibt minimal und lädt ausschließlich die nächste Bootstufe.
```

---

## 44. UEFI-Abgrenzung

UEFI verwendet keine klassische BIOS-Stage-1 im MBR-Sinn.

Der UEFI-Bootpfad startet eine PE/COFF-Anwendung über die Firmware.

Dieses ADR gilt primär für den BIOS-Bootpfad.

Das zugrunde liegende Prinzip gilt jedoch auch für UEFI:

* Die früheste Firmware-Einstiegskomponente soll klein bleiben.
* Komplexe Bootlogik soll in klar getrennte Module verlagert werden.
* Kernel Loading und Boot-Handoff bleiben eigenständige Komponenten.

---

## 45. Stage-1 und BootFS

Stage-1 wertet BootFS nicht vollständig aus.

BootFS kann jedoch einen reservierten Bereich oder eine kleine Locator-Struktur definieren, über die Stage-1 Stage-2 findet.

Stage-1 darf ausschließlich den minimal erforderlichen Locator-Teil interpretieren.

Die vollständige BootFS-Verarbeitung erfolgt in Stage-2.

---

## 46. Stage-1-Update

Stage-1 soll selten geändert werden.

Änderungen an Stage-1 sind besonders risikoreich, da ein Fehler das gesamte System unbootbar machen kann.

Daher gilt:

* Stage-1 erhält nur notwendige Fehlerkorrekturen.
* Neue Funktionen werden bevorzugt Stage-2 hinzugefügt.
* Das binäre Layout bleibt möglichst stabil.
* Jede Änderung benötigt vollständige Bootmedientests.

---

## 47. Aktualisierungssicherheit

Beim Aktualisieren von Stage-1 muss verhindert werden, dass ein Stromausfall oder Schreibfehler das Bootmedium unbrauchbar macht.

Mögliche Maßnahmen:

* Backup-Bootsektor
* atomare Aktualisierungsstrategie
* Bootsektor-Checksumme
* Recovery-Medium
* unveränderliche Stage-1-Version
* Update ausschließlich im Offline-Modus

Die konkrete Updatearchitektur wird separat spezifiziert.

---

## 48. Stage-2-Update unabhängig von Stage-1

Stage-2 soll aktualisiert werden können, ohne Stage-1 zu verändern, sofern:

* Position,
* maximale Größe,
* Headerformat,
* und Übergabevertrag

kompatibel bleiben.

Dies ist ein zentraler Vorteil der minimalen Stage-1-Architektur.

---

## 49. Versionskompatibilität

Stage-1 und Stage-2 benötigen eine minimale Versionsbeziehung.

Stage-1 darf im Stage-2-Header prüfen:

```text
Minimum Stage-1 Version

Stage-2 Header Version
```

Eine neue Stage-2-Version darf ältere Stage-1-Versionen nicht unbeabsichtigt brechen.

Falls eine inkompatible Version erkannt wird, muss Stage-1 kontrolliert abbrechen.

---

## 50. Stabiler Stage-2-Header

Der durch Stage-1 ausgewertete Bereich des Stage-2-Headers muss langfristig stabil bleiben.

Neue Felder werden:

* am Ende ergänzt,
* über Headergröße erkannt,
* und von älteren Stage-1-Versionen ignoriert.

Vorhandene Felder dürfen nicht:

* umgedeutet,
* verschoben,
* oder in ihrer Größe verändert

werden.

---

## 51. Build-System

Das Build-System muss Stage-1 und Stage-2 getrennt erzeugen.

Beispiel:

```text
build/boot/bios/stage1.bin

build/boot/bios/stage2.bin
```

Das Image-Build-System muss:

* Stage-1 an der korrekten Position schreiben,
* Stage-2 korrekt platzieren,
* Locatorinformationen aktualisieren,
* Größenlimits prüfen,
* Prüfsummen erzeugen,
* und das vollständige Image validieren.

---

## 52. Größenprüfung

Der Build muss fehlschlagen, wenn Stage-1 die zulässige Größe überschreitet.

Beispiel für einen MBR:

```text
Maximaler Stage-1-Bootcode:

446 Byte
```

Bei einem reinen Bootsektor ohne Partitionstabelle kann eine andere Grenze gelten.

Die Grenze muss pro Zielmedium explizit definiert sein.

---

## 53. Keine stillschweigende Kürzung

Das Build-System darf Stage-1 niemals automatisch kürzen oder überschüssige Daten verwerfen.

Ein Größenüberschritt ist ein harter Buildfehler.

---

## 54. Binärlayoutprüfung

Das Build-System muss mindestens prüfen:

* Bootsignatur vorhanden
* Stage-1-Größe gültig
* Partitionstabelle nicht überschrieben
* Stage-2-Locator gültig
* Stage-2-Größe innerhalb des Limits
* Stage-2-Prüfsumme korrekt
* Einstiegspunkt innerhalb des Stage-2-Images

---

## 55. Assembleranforderungen

Stage-1 wird vorzugsweise in Assembler implementiert.

Der Code muss:

* freestanding,
* positionsbewusst,
* ohne Runtimeabhängigkeiten,
* und vollständig nachvollziehbar

sein.

Makros dürfen verwendet werden, wenn sie die resultierende Binärgröße und Kontrolle nicht verschleiern.

---

## 56. Keine Hochsprachen-Runtime

Stage-1 darf keine Laufzeitbibliotheken aus C, C++, Rust oder anderen Hochsprachen voraussetzen.

Eine teilweise Erzeugung durch Hochsprachen ist nur zulässig, wenn:

* vollständig freestanding kompiliert wird,
* die Binärgröße eingehalten wird,
* keine versteckten Abhängigkeiten entstehen,
* und das resultierende Maschinenverhalten vollständig überprüfbar bleibt.

Für die erste Implementierung bleibt Assembler die bevorzugte Sprache.

---

## 57. Codequalität

Trotz der geringen Größe muss Stage-1 klar strukturiert sein.

Erforderlich sind:

* benannte Konstanten
* dokumentierte Speicheradressen
* definierte Registerverträge
* klare Fehlerpfade
* begrenzte Schleifen
* keine unbegründeten Magic Numbers
* keine impliziten BIOS-Annahmen

---

## 58. Kommentare

Stage-1-Assembler soll gezielt kommentiert werden.

Kommentare müssen insbesondere erklären:

* BIOS-Voraussetzungen
* Registerbelegung
* Speicherlayout
* Disk Address Packet
* Segmentberechnungen
* Fehlerbehandlung
* Übergabevertrag

Kommentare sollen nicht jede offensichtliche Instruktion wiederholen.

---

## 59. Determinismus

Stage-1 muss bei identischem Medium und identischer Firmwareumgebung deterministisch arbeiten.

Es darf keine:

* zufällige Speicherplatzierung,
* dynamische Featureauswahl ohne feste Regeln,
* heuristische Stage-2-Suche,
* oder unbeschränkte Wiederholungslogik

geben.

---

## 60. Sicherheitsmodell

Stage-1 ist Teil der Trusted Computing Base des Bootpfads.

Obwohl Stage-1 keine vollständige kryptografische Prüfung durchführt, muss sie:

* Speichergrenzen einhalten,
* Größenfelder prüfen,
* Offsetüberläufe verhindern,
* ungültige Einstiegspunkte ablehnen,
* und BIOS-Rückgabewerte kontrollieren.

---

## 61. Integerüberläufe

Alle Berechnungen für:

* Sektoranzahl,
* Ladeadresse,
* Zielende,
* Einstiegsoffset,
* und Stage-2-Größe

müssen gegen Überläufe geprüft werden.

Insbesondere darf folgende Berechnung nicht ungeprüft erfolgen:

```text
sector_count × sector_size
```

---

## 62. Speicherüberläufe

Stage-1 muss sicherstellen, dass:

```text
load_address + image_size
```

innerhalb des reservierten Stage-2-Bereichs liegt.

Stage-2 darf nicht geladen werden, wenn dadurch:

* Stage-1,
* Stack,
* BIOS-Speicher,
* Übergabedaten,
* oder andere reservierte Bereiche

überschrieben würden.

---

## 63. Einstiegspunktprüfung

Der Stage-2-Einstiegspunkt muss innerhalb des geladenen Stage-2-Images liegen.

Formal:

```text
stage2_base
<=
entry_address
<
stage2_base + stage2_size
```

Ein Einstiegspunkt außerhalb dieses Bereichs ist ungültig.

---

## 64. Selbstüberschreibung

Stage-1 darf sich während des Ladens von Stage-2 nicht selbst überschreiben, solange noch Code aus dem überschriebenen Bereich ausgeführt wird.

Eine Überlagerung ist nur zulässig, wenn sie durch eine speziell entworfene Relokationsstrategie sicher beherrscht wird.

Die bevorzugte Implementierung verwendet getrennte Speicherbereiche.

---

## 65. BIOS-Register

Stage-1 darf nicht voraussetzen, dass BIOS alle Register erhält.

Nach BIOS-Interrupts müssen benötigte Register:

* gesichert,
* wiederhergestellt,
* oder neu gesetzt

werden.

Dies gilt insbesondere für:

* `DL`
* Segmentregister
* Zieladresse
* Sektorzähler

---

## 66. Firmwareabweichungen

Stage-1 muss mit üblichen BIOS-Abweichungen umgehen.

Beispiele:

* veränderte Register
* begrenzte Sektoranzahl pro Transfer
* Fehler bei Transfers über Trackgrenzen
* Fehler bei 64-KiB-Grenzen
* inkonsistente Laufwerksgeometrie
* fehlerhafte LBA-Unterstützung

Die Implementierung soll konservative Transfergrößen verwenden.

---

## 67. Transfergröße

Stage-1 darf größere Stage-2-Images in mehreren Operationen laden.

Eine einzelne Operation soll die von BIOS und Zieladressierung sicher unterstützte Sektoranzahl nicht überschreiten.

Die genaue Grenze kann beispielsweise sein:

```text
127 Sektoren
```

oder niedriger, abhängig von der Implementierung.

---

## 68. Emulation und Virtualisierung

Stage-1 muss mindestens in folgenden Umgebungen getestet werden:

* QEMU
* Bochs oder vergleichbare x86-Emulation
* VirtualBox
* VMware, sofern verfügbar
* mindestens ein physisches BIOS-System
* mindestens ein BIOS-Kompatibilitätsmodul, sofern unterstützt

Erfolgreiches Booten in QEMU allein genügt nicht als Hardwarevalidierung.

---

## 69. Bootmedien

Stage-1 soll für relevante Bootmedien getestet werden:

* Festplattenimage
* USB-Stick
* SATA-Datenträger
* virtuelle IDE-Festplatte
* virtuelle SATA-Festplatte
* gegebenenfalls Diskettenimage
* optisches Bootimage

Nicht jede Stage-1-Variante muss jedes Medium unterstützen.

---

## 70. Testanforderungen

Stage-1 muss mindestens folgende Tests bestehen:

### 70.1 Gültige Stage-2

Stage-2 wird korrekt geladen und ausgeführt.

### 70.2 Datenträgerlesefehler

Der Fehler wird erkannt und begrenzt wiederholt.

### 70.3 Ungültige Stage-2-Kennung

Der Kontrolltransfer wird verhindert.

### 70.4 Beschädigte Prüfsumme

Stage-2 wird abgelehnt.

### 70.5 Zu große Stage-2

Das Laden wird vor einer Speicherüberschreibung abgebrochen.

### 70.6 Ungültiger Einstiegspunkt

Der Kontrolltransfer wird verhindert.

### 70.7 LBA nicht unterstützt

Fallback oder kontrollierter Fehler wird ausgeführt.

### 70.8 Transfer über Segmentgrenze

Transfers werden korrekt aufgeteilt.

### 70.9 BIOS verändert Register

Stage-1 arbeitet weiterhin korrekt.

### 70.10 Stage-2 kehrt zurück

Stage-1 behandelt dies als Fehler.

### 70.11 Beschädigte Locatorstruktur

Stage-2 wird nicht aus einer ungültigen Position geladen.

### 70.12 Maximale Stage-2-Größe

Der Grenzfall wird korrekt geladen.

---

## 71. Negative Tests

Zusätzlich müssen manipulierte Images getestet werden:

* Start-LBA außerhalb des Mediums
* Sektoranzahl null
* übergroße Sektoranzahl
* überlaufende Zieladresse
* überlaufender Einstiegsoffset
* unvollständiger Stage-2-Header
* falsche Headergröße
* unbekannte inkompatible Version
* Überschneidung mit Stage-1
* Überschneidung mit Stack

---

## 72. Regressionstests

Jede Änderung an Stage-1 muss sämtliche Boot- und Negativtests erneut ausführen.

Zusätzlich soll das resultierende Stage-1-Binärimage mit der vorherigen Version verglichen werden.

Unerwartete Binäränderungen müssen untersucht werden.

---

## 73. Binärinspektion

Empfohlene Werkzeuge:

```text
ndisasm

objdump

hexdump

xxd
```

Zu prüfen sind:

* Instruktionslayout
* Bootsignatur
* Locatorfelder
* Partitionstabellengrenzen
* Sprungziele
* unbeabsichtigte Fülldaten

---

## 74. Debug-Ausgabe

Entwicklungsvarianten von Stage-1 dürfen zusätzliche Debugzeichen ausgeben.

Beispiel:

```text
1

R

V

J
```

Dabei kann jedes Zeichen eine abgeschlossene Phase markieren.

Release-Builds sollen solche Ausgaben reduzieren oder entfernen.

---

## 75. Keine Feature-Flags in Stage-1

Stage-1 soll möglichst keine umfangreichen Feature-Flags enthalten.

Zu viele Buildvarianten erhöhen:

* Testmatrix
* Fehlerwahrscheinlichkeit
* Binärunterschiede
* Wartungsaufwand

Unvermeidbare Varianten sollen vollständig getrennte, klar benannte Targets sein.

---

## 76. Konfigurationsfreiheit

Stage-1 ist nicht benutzerkonfigurierbar.

Benutzeroptionen gehören in Stage-2 oder spätere Bootstufen.

Stage-1 liest keine allgemeine Konfigurationsdatei.

---

## 77. Keine Lokalisierung

Stage-1 enthält keine mehrsprachigen Texte.

Minimalmeldungen und Fehlercodes sind sprachneutral oder technisch englisch.

Lokalisierte Bootfehler werden erst in späteren Stufen bereitgestellt.

---

## 78. Recovery

Stage-1 implementiert kein vollständiges Recovery-System.

Sie darf jedoch alternativ eine zweite Stage-2-Kopie laden, wenn dies mit sehr geringer Zusatzkomplexität möglich ist.

Beispiel:

```text
Primary Stage-2

↓

bei Fehler

↓

Backup Stage-2
```

Diese Funktion ist nur zulässig, wenn:

* die Größenbegrenzung eingehalten wird,
* die Fehlerpfade überschaubar bleiben,
* und die Stage-1-Minimalität nicht wesentlich verletzt wird.

---

## 79. Backup-Stage-2

Eine Backup-Stage-2 kann über:

* zweiten festen LBA-Bereich,
* zweiten Locatoreintrag,
* oder ein Recovery-Bootmedium

bereitgestellt werden.

Stage-1 darf höchstens eine kleine, deterministische Fallbacklogik besitzen.

Sie darf keine komplexe Versionsauswahl durchführen.

---

## 80. Bootmenü

Ein Bootmenü ist in Stage-1 ausdrücklich verboten.

Das NovaOS-Bootmenü wird von Stage-2 oder einer späteren UI-Stufe bereitgestellt.

Stage-1 darf höchstens eine sehr einfache Notfalltaste auswerten, wenn dies für die Auswahl einer Backup-Stage-2 zwingend erforderlich ist.

Diese Ausnahme soll vermieden werden.

---

## 81. Tastatureingabe

Stage-1 wartet im normalen Bootpfad nicht auf Benutzereingaben.

Der Bootvorgang soll unmittelbar fortgesetzt werden.

Dadurch werden:

* Verzögerungen,
* Tastaturabhängigkeiten,
* und zusätzliche Fehlerzustände

vermieden.

---

## 82. Zeitlimits

Stage-1 besitzt keine sichtbaren Bootmenü-Timeouts.

Interne BIOS-Leseversuche müssen jedoch begrenzt sein.

Stage-1 darf nicht unbegrenzt auf ein nicht reagierendes Gerät warten, soweit dies kontrollierbar ist.

---

## 83. Performance

Stage-1 soll schnell laden, jedoch besitzt Robustheit Vorrang vor minimalen Millisekunden.

Optimierungen sind zulässig, wenn sie:

* die Codegröße nicht unverhältnismäßig erhöhen,
* die Kompatibilität nicht reduzieren,
* und die Validierung nicht umgehen.

---

## 84. Wartbarkeit

Stage-1 soll nach Erreichen eines stabilen Zustands möglichst unverändert bleiben.

Neue Bootfunktionen werden grundsätzlich in Stage-2 implementiert.

Bei jeder gewünschten Stage-1-Erweiterung ist zuerst zu prüfen:

```text
Kann diese Funktion in Stage-2 umgesetzt werden?
```

Wenn ja, muss sie in Stage-2 umgesetzt werden.

---

## 85. Zulassung neuer Stage-1-Funktionen

Eine neue Funktion darf nur in Stage-1 aufgenommen werden, wenn sämtliche folgenden Bedingungen erfüllt sind:

1. Sie ist für das Laden von Stage-2 zwingend erforderlich.
2. Sie kann nicht zuverlässig in Stage-2 umgesetzt werden.
3. Sie verletzt das Größenlimit nicht.
4. Sie erhöht die Komplexität nur geringfügig.
5. Sie ist vollständig testbar.
6. Sie bleibt über Bootmedien hinweg deterministisch.
7. Sie wurde durch ein eigenes ADR genehmigt.

---

## 86. Architekturregel

Die folgende Regel gilt normativ:

```text
Stage-1 darf nur Funktionen enthalten,
die für das sichere Laden und Starten von Stage-2
unmittelbar erforderlich sind.
```

Funktionen, die lediglich:

* komfortabel,
* optisch ansprechend,
* zukünftig nützlich,
* oder theoretisch effizienter

wären, gehören nicht in Stage-1.

---

## 87. Alternativen

### 87.1 Vollständiger Bootloader im Bootsektor

Der gesamte Bootloader könnte in Stage-1 implementiert werden.

Vorteile:

* keine zusätzliche Bootstufe
* direkter Kernelstart
* theoretisch kürzere Bootkette

Nachteile:

* praktisch nicht ausreichend Platz
* extreme Codeverdichtung erforderlich
* schlechte Wartbarkeit
* kaum erweiterbar
* hohe Fehleranfälligkeit
* keine realistische Unterstützung moderner Anforderungen

**Entscheidung:** Abgelehnt.

---

### 87.2 Dateisystemzugriff in Stage-1

Stage-1 könnte den Pfad einer Stage-2- oder Kerneldatei direkt aus einem Dateisystem laden.

Vorteile:

* flexiblere Dateiablage
* Updates ohne feste Sektorposition
* leichter verständliches Bootmedium

Nachteile:

* großer Codeumfang
* Fragmentierungsprobleme
* Dateisystemabhängigkeit
* komplexe Fehlerfälle
* höhere Sicherheitsrisiken
* erschwerte MBR-Größenbegrenzung

**Entscheidung:** Abgelehnt.

Die Dateisystemverarbeitung erfolgt in Stage-2.

---

### 87.3 Direkter Kernelstart aus Stage-1

Stage-1 könnte einen flachen Kernel direkt laden.

Vorteile:

* weniger Bootstufen
* einfacher früher Prototyp

Nachteile:

* Kernel muss an festen Sektoren liegen
* keine ELF-Unterstützung
* starke Größenbegrenzung
* starke Kopplung
* keine Module
* keine Recoverylogik
* schlechte Zukunftsfähigkeit

**Entscheidung:** Abgelehnt.

---

### 87.4 Komprimierte Stage-2 mit Dekompressor in Stage-1

Vorteile:

* kleinere Bootmedienbelegung
* potenziell größere Stage-2

Nachteile:

* zusätzlicher Code
* zusätzlicher Speicherbedarf
* komplexere Fehlerbehandlung
* Dekompressionsfehler
* längere Trusted Computing Base

**Entscheidung:** Abgelehnt.

Stage-2 wird unkomprimiert oder durch eine spätere Zwischenstufe geladen.

---

### 87.5 Eigener vollständiger Disk-Treiber

Stage-1 könnte BIOS umgehen und Hardware direkt ansprechen.

Vorteile:

* unabhängig von BIOS-Fehlern
* potenziell höhere Leistung

Nachteile:

* extrem hohe Komplexität
* viele Controller
* unvertretbare Codegröße
* starke Hardwareabhängigkeit

**Entscheidung:** Abgelehnt.

Stage-1 verwendet ausschließlich Firmwaredienste.

---

### 87.6 Keine Stage-1-Validierung

Stage-1 könnte Stage-2 ohne jede Prüfung starten.

Vorteile:

* minimaler Code
* geringste Größe

Nachteile:

* beschädigte oder falsche Daten werden ausgeführt
* schwer diagnostizierbare Fehler
* Sicherheits- und Stabilitätsrisiko

**Entscheidung:** Abgelehnt.

Eine minimale Plausibilitäts- und Integritätsprüfung bleibt erforderlich.

---

### 87.7 Größere Stage-1 über mehrere Sektoren

Stage-1 könnte technisch aus mehreren direkt hintereinanderliegenden Sektoren bestehen.

Dies entspricht jedoch konzeptionell bereits einer zusätzlichen Bootstufe.

**Entscheidung:** Der erste BIOS-geladene Sektor bleibt Stage-1. Weitere geladene Sektoren gelten als Stage-2 oder Stage-1.5 und unterliegen einer separaten Verantwortungsgrenze.

---

## 88. Stage-1.5

Eine optionale Zwischenstufe kann als:

```text
Stage-1.5
```

bezeichnet werden.

Sie darf verwendet werden, wenn:

* das Bootmedium komplexere Lokalisierung erfordert,
* Stage-2 nicht direkt erreichbar ist,
* oder ein minimaler Dateisystemloader notwendig wird.

Stage-1.5 ist nicht Teil des ersten Bootsektors und unterliegt nicht demselben Byte-Limit.

Trotzdem soll auch Stage-1.5 klein und zweckgebunden bleiben.

---

## 89. Abgrenzung zwischen Stage-1.5 und Stage-2

Stage-1.5 darf ausschließlich die technische Lücke zwischen Stage-1 und Stage-2 schließen.

Beispiele:

* Stage-2 aus einem einfachen Dateisystem laden
* Wechsel auf erweiterte Datenträgerzugriffe
* größere Loaderdatei lokalisieren

Stage-1.5 soll nicht:

* das Bootmenü,
* den Kernel Loader,
* die grafische Oberfläche,
* oder das Recovery-System

implementieren.

---

## 90. Konsequenzen

### 90.1 Positive Konsequenzen

* Stage-1 bleibt klein und überschaubar.
* Der BIOS-Bootsektor bleibt langfristig stabil.
* Die Wahrscheinlichkeit kritischer Bootfehler sinkt.
* Die Hardwarekompatibilität verbessert sich.
* Der Code kann vollständig manuell geprüft werden.
* Stage-2 kann unabhängig weiterentwickelt werden.
* Neue Funktionen benötigen keine Änderung des Bootsektors.
* Kernel- und Dateisystemformate bleiben von Stage-1 entkoppelt.
* Recovery und UI können in geeigneter Umgebung implementiert werden.
* BIOS-Eigenheiten bleiben auf einen kleinen Codebereich begrenzt.

### 90.2 Negative Konsequenzen

* Der Bootpfad enthält mindestens zwei Stufen.
* Stage-2 muss zuverlässig auf dem Medium platziert werden.
* Zwischen Stage-1 und Stage-2 wird ein eigenes Übergabeformat benötigt.
* Das Image-Build-System muss feste Bootbereiche verwalten.
* Fehler können in mehreren Bootstufen auftreten.
* Debugging erfordert klare Trennung der jeweiligen Verantwortlichkeiten.

### 90.3 Neutrale Konsequenzen

* Stage-1 wird weiterhin hauptsächlich in Assembler geschrieben.
* Stage-2 kann Assembler, C oder Rust verwenden.
* Der Kernel bleibt ein separates ELF-Artefakt.
* BIOS und UEFI verwenden unterschiedliche früheste Einstiegskomponenten.
* Beide Bootpfade können später in gemeinsame Loaderlogik zusammenlaufen.

---

## 91. Implementierungsstrategie

### Phase 1: Direkte Stage-2-Sektoren

Stage-1 lädt eine fest konfigurierte Anzahl unmittelbar folgender Sektoren.

Unterstützt:

```text
BIOS Real Mode

INT 13h

feste Ladeadresse

feste Stage-2-Sektoranzahl

minimaler Fehlertext
```

### Phase 2: LBA-Unterstützung

Ergänzt:

```text
INT 13h Extensions

Disk Address Packet

begrenzte Retry-Logik
```

### Phase 3: Stage-2-Header

Ergänzt:

```text
Magic

Größe

Version

Entry Offset

Prüfsumme
```

### Phase 4: Backup-Stage-2

Optional ergänzt:

```text
primärer Stage-2-Bereich

sekundärer Stage-2-Bereich
```

### Phase 5: Stabilisierung

Nach erfolgreicher Hardwarevalidierung wird Stage-1 eingefroren und nur noch bei zwingenden Fehlerkorrekturen geändert.

---

## 92. Beispielhafter Ablauf

```text
1. BIOS lädt Stage-1 nach 0x7C00.

2. Stage-1 deaktiviert kurzzeitig Interrupts.

3. Segmentregister werden initialisiert.

4. Stack wird eingerichtet.

5. BIOS-Bootlaufwerk aus DL wird gesichert.

6. LBA-Unterstützung wird geprüft.

7. Stage-2-Locator wird ausgewertet.

8. Stage-2 wird in begrenzten Blöcken geladen.

9. BIOS-Fehler werden bis zur Maximalzahl wiederholt.

10. Stage-2-Header wird geprüft.

11. Stage-2-Prüfsumme wird geprüft.

12. Übergabestruktur wird erstellt.

13. Registervertrag wird hergestellt.

14. Kontrolltransfer an Stage-2.

15. Stage-1 wird nicht erneut ausgeführt.
```

---

## 93. Beispielhafter Pseudocode

```text
initialize_segments()

initialize_stack()

boot_drive = DL

if lba_supported(boot_drive):
    result = load_stage2_lba()
else:
    result = load_stage2_chs()

if result != success:
    show_error(ERROR_READ)
    halt()

if not validate_stage2_header():
    show_error(ERROR_HEADER)
    halt()

if not validate_stage2_checksum():
    show_error(ERROR_CHECKSUM)
    halt()

prepare_stage1_handoff()

jump_to_stage2()
```

---

## 94. Beispielhafte Assemblerstruktur

```asm
bits 16
org 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    cld
    sti

    mov [boot_drive], dl

    call load_stage2
    jc disk_error

    call validate_stage2
    jc stage2_error

    mov dl, [boot_drive]
    jmp 0x0000:0x7E00

disk_error:
    mov si, msg_disk
    call print_string
    jmp halt

stage2_error:
    mov si, msg_stage2
    call print_string

halt:
    cli
.hang:
    hlt
    jmp .hang

boot_drive db 0
msg_disk   db "E1", 0
msg_stage2 db "E2", 0

times 510 - ($ - $$) db 0
dw 0xAA55
```

Dieses Beispiel beschreibt lediglich die Struktur und ist keine vollständige normative Implementierung.

---

## 95. Qualitätsanforderungen

Stage-1 muss:

* deterministisch,
* reproduzierbar,
* klein,
* robust,
* defensiv,
* hardwarekompatibel,
* statisch analysierbar,
* und vollständig testbar

sein.

Die Optimierung auf minimale Größe darf keine grundlegenden Sicherheitsprüfungen entfernen.

---

## 96. Dokumentationsanforderungen

Für jede Stage-1-Variante müssen dokumentiert werden:

* Zielmedium
* maximale Codegröße
* Stage-2-Position
* maximale Stage-2-Größe
* Ladeadresse
* Stackadresse
* BIOS-Zugriffsmethode
* Fallbackstrategie
* Fehlercodes
* Übergabevertrag
* unterstützte Firmwarevoraussetzungen

---

## 97. Konformitätskriterien

Eine Stage-1-Implementierung erfüllt dieses ADR, wenn:

* sie ausschließlich Stage-2 lädt und startet,
* sie keinen Kernel direkt lädt,
* sie keinen vollständigen Dateisystemparser enthält,
* sie keine grafische Benutzeroberfläche enthält,
* sie keinen Protected- oder Long-Mode-Wechsel durchführt,
* sie BIOS-Datenträgerfehler kontrolliert behandelt,
* sie Stage-2 minimal validiert,
* sie Speicher- und Größenbegrenzungen prüft,
* sie das BIOS-Bootlaufwerk korrekt übergibt,
* sie das definierte Größenlimit einhält,
* und alle zusätzlichen Funktionen unmittelbar für das Laden von Stage-2 erforderlich sind.

---

## 98. Entscheidungsergebnis

Stage-1 bleibt die dauerhaft minimale erste BIOS-Bootstufe von NovaOS.

Ihre verbindliche Verantwortung lautet:

```text
Initialisieren.

Stage-2 laden.

Stage-2 minimal validieren.

Übergabe vorbereiten.

Stage-2 starten.
```

Alle komplexeren Funktionen werden in Stage-2 oder spätere Bootkomponenten verlagert.

---

## 99. Zusammenfassung

NovaOS hält Stage-1 bewusst klein und funktional begrenzt.

Der erste Bootsektor übernimmt keine allgemeine Bootloader-, Dateisystem-, Grafik-, Recovery- oder Kernel-Logik. Er stellt lediglich einen definierten frühen Prozessorzustand her, lädt eine bekannte Stage-2-Komponente, prüft diese minimal und übergibt die Ausführung.

Diese Architektur reduziert die kritische Codefläche, verbessert die BIOS-Kompatibilität und ermöglicht eine unabhängige Weiterentwicklung aller späteren Bootfunktionen.

Stage-1 wird damit als stabile, selten veränderte Grundlage des BIOS-Bootpfads behandelt. Innovationen und zusätzliche Funktionen werden nicht in den Bootsektor integriert, sondern konsequent in Stage-2 und nachfolgende Komponenten verschoben.
