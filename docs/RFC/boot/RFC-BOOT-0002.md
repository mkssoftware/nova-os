# RFC-BOOT-0002: Nova Boot Information Structure (Teil 1)

* **Dokumenttyp:** Request for Comments (RFC)
* **Dokument-ID:** RFC-BOOT-0002
* **Titel:** Nova Boot Information Structure (NBIS)
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Protocol / Boot ABI
* **Gültigkeitsbereich:** Stage-2, Boot Module Manager, Kernel Loader, Kernel
* **Abhängigkeiten:** RFC-BOOT-0001, ADR-BOOT-1012, ADR-BOOT-1020
* **Letzte Aktualisierung:** 2026-07-29

---

# Inhaltsverzeichnis

1. Einleitung
2. Motivation
3. Ziele
4. Nicht-Ziele
5. Grundprinzipien
6. Architektur
7. Lebenszyklus
8. Header
9. Versionierung
10. Erweiterungsmodell
11. Objektmodell
12. Datentypen
13. Validierung
14. Fehlerbehandlung

---

# 1. Einleitung

Die **Nova Boot Information Structure (NBIS)** ist die zentrale Datenstruktur des Nova Boot Protocols.

Sie stellt sämtliche Informationen bereit, welche der Bootloader dem Kernel beim Systemstart übergibt.

Die NBIS bildet die einzige standardisierte Datenschnittstelle zwischen Bootloader und Kernel.

---

# 2. Motivation

Ein moderner Bootprozess muss zahlreiche Informationen übergeben:

* Speicherlayout
* Framebuffer
* CPU
* ACPI
* Device Tree
* Bootparameter
* Bootmodule
* Sicherheitsstatus
* Firmwareinformationen

Diese Informationen dürfen nicht über lose Zeiger oder interne Strukturen ausgetauscht werden.

Stattdessen definiert die NBIS ein vollständig versioniertes und dokumentiertes Übergabeformat.

---

# 3. Ziele

Die NBIS verfolgt folgende Ziele:

* eindeutige Datenübergabe
* stabile ABI
* Versionierbarkeit
* Erweiterbarkeit
* Compilerunabhängigkeit
* Sprachunabhängigkeit
* Plattformunabhängigkeit
* einfache Validierung

---

# 4. Nicht-Ziele

Die NBIS beschreibt nicht:

* interne Kernelobjekte
* Scheduler
* Prozessverwaltung
* Dateisysteme
* Gerätetreiber
* Userspace

Sie enthält ausschließlich Bootinformationen.

---

# 5. Grundprinzipien

Die NBIS basiert auf folgenden Prinzipien:

* Selbstbeschreibend
* Versioniert
* Deterministisch
* Erweiterbar
* Validierbar
* Rückwärtskompatibel innerhalb einer Major-Version

---

# 6. Architekturübersicht

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Kernel Loader

↓

Nova Boot Information Structure

↓

Kernel
```

Die NBIS ist ausschließlich Transportmedium.

Sie besitzt keinerlei Programmlogik.

---

# 7. Verantwortlichkeiten

Die Erzeugung erfolgt ausschließlich durch den Bootloader.

Die Verarbeitung erfolgt ausschließlich durch den Kernel.

Nach erfolgreicher Initialisierung übernimmt der Kernel alle benötigten Daten in eigene interne Strukturen.

---

# 8. Lebensdauer

Die NBIS existiert nur während des Bootvorgangs.

Nach erfolgreicher Übernahme darf sie vom Kernel freigegeben werden.

---

# 9. Eigentümer

Vor Kernelstart:

Bootloader

Nach erfolgreicher Übernahme:

Kernel

Es existiert niemals gemeinsames Eigentum.

---

# 10. Selbstbeschreibende Struktur

Jede NBIS beginnt mit einem Header.

Dieser enthält mindestens:

* Magic Number
* Version
* Gesamtgröße
* Prüfinformationen

Dadurch kann jede Struktur unabhängig validiert werden.

---

# 11. Beispielheader

```c
typedef struct
{
    uint32_t magic;
    uint16_t major;
    uint16_t minor;
    uint32_t total_size;
    uint32_t checksum;
} nbis_header_t;
```

---

# 12. Magic Number

Die Magic Number identifiziert eindeutig eine gültige NBIS.

Ungültige Magic Numbers führen zum sofortigen Abbruch der Verarbeitung.

---

# 13. Versionsnummer

Jede NBIS besitzt:

* Major
* Minor

Optional kann zusätzlich eine Patch-Version dokumentiert werden.

---

# 14. Major-Version

Eine neue Major-Version bedeutet:

* inkompatible Änderungen
* neue Pflichtfelder
* geänderte Semantik

Kernel und Bootloader müssen dieselbe Major-Version unterstützen.

---

# 15. Minor-Version

Minor-Versionen dürfen ausschließlich kompatible Erweiterungen enthalten.

Bestehende Felder dürfen nicht verändert werden.

---

# 16. Gesamtgröße

Das Größenfeld beschreibt die komplette NBIS.

Dadurch können zukünftige Erweiterungen sicher übersprungen werden.

---

# 17. Prüfsumme

Optional kann eine Prüfsumme enthalten sein.

Sie dient ausschließlich der Integritätsprüfung der Struktur.

---

# 18. Ausrichtung

Alle Strukturen müssen auf natürliche Speichergrenzen ausgerichtet werden.

Nicht dokumentiertes Padding ist unzulässig.

---

# 19. Datentypen

Es werden ausschließlich fest definierte Ganzzahltypen verwendet:

* uint8_t
* uint16_t
* uint32_t
* uint64_t

Compilerabhängige Typen dürfen nicht Bestandteil der Spezifikation sein.

---

# 20. Byte-Reihenfolge

Die NBIS definiert eine eindeutige Endianness.

Alle Felder werden entsprechend dieser Vorgabe interpretiert.

---

# 21. Sprachunabhängigkeit

Die Spezifikation darf nicht an C gebunden sein.

Sie muss ebenso in:

* Rust
* Zig
* C++
* Assembly

implementierbar sein.

---

# 22. Compilerunabhängigkeit

Die Struktur darf keine Annahmen über Compilerlayouts enthalten.

Alle Größen und Offsets werden explizit definiert.

---

# 23. Objektmodell

Die NBIS besteht aus einzelnen Informationsblöcken.

```text
NBIS Header

↓

Memory Block

↓

Framebuffer Block

↓

CPU Block

↓

Firmware Block

↓

Boot Parameter Block

↓

Extension Blocks
```

---

# 24. Blockprinzip

Jeder Block besitzt:

* Typ
* Größe
* Version
* Nutzdaten

Dadurch können unbekannte Blöcke sicher übersprungen werden.

---

# 25. Blockheader

Jeder Block beginnt mit:

```c
typedef struct
{
    uint16_t type;
    uint16_t version;
    uint32_t size;
} nbis_block_header_t;
```

---

# 26. Blocktypen

Beispiele:

* MEMORY
* FRAMEBUFFER
* CPU
* ACPI
* DEVICE_TREE
* MODULES
* SECURITY
* COMMANDLINE

Neue Blocktypen können ergänzt werden.

---

# 27. Erweiterungsblöcke

Zusätzliche Informationen werden ausschließlich über neue Blöcke eingeführt.

Vorhandene Blockdefinitionen bleiben unverändert.

---

# 28. Reihenfolge

Die Reihenfolge der Blöcke ist definiert.

Unbekannte optionale Blöcke dürfen übersprungen werden.

---

# 29. Pflichtblöcke

Mindestens erforderlich sind:

* Header
* Speicher
* CPU
* Framebuffer
* Bootparameter

Weitere Pflichtblöcke können architekturabhängig definiert werden.

---

# 30. Optionale Blöcke

Beispiele:

* TPM
* Secure Boot
* Hypervisor
* NUMA
* Device Tree
* SMBIOS
* Netzwerkboot

---

# 31. Validierung

Vor der Verarbeitung überprüft der Kernel mindestens:

* Magic Number
* Version
* Gesamtgröße
* Blockgrößen
* Blockreihenfolge
* Überlappungen

---

# 32. Fehlerbehandlung

Ungültige NBIS-Daten führen niemals zu undefiniertem Verhalten.

Mögliche Reaktionen:

* Boot abbrechen
* Recovery starten
* Fehler protokollieren

---

# 33. Sicherheitsmodell

Jeder Informationsblock wird als nicht vertrauenswürdig behandelt, bis seine Konsistenz erfolgreich geprüft wurde.

---

# 34. Qualitätsanforderungen

Die Nova Boot Information Structure muss:

* vollständig dokumentiert,
* versioniert,
* validierbar,
* erweiterbar,
* sprachunabhängig,
* deterministisch

sein.

---

# 35. Zusammenfassung Teil 1

Die Nova Boot Information Structure definiert ein selbstbeschreibendes, versioniertes und blockbasiertes Übergabeformat für sämtliche Bootinformationen zwischen Bootloader und Kernel. Durch standardisierte Header, klar definierte Informationsblöcke und ein konsequentes Erweiterungsmodell entsteht eine stabile Grundlage für eine langfristig wartbare und plattformunabhängige Bootarchitektur.



# RFC-BOOT-0002: Nova Boot Information Structure (Teil 2)

---

# 36. Speicherblock

Der Speicherblock beschreibt den physikalisch verfügbaren Arbeitsspeicher.

Er enthält ausschließlich Informationen über den Systemzustand zum Zeitpunkt der Übergabe.

Der Kernel erzeugt daraus seine eigene Speicherverwaltung.

---

# 37. Aufgaben des Speicherblocks

Der Speicherblock dient zur Beschreibung von:

* verfügbarem RAM
* reservierten Bereichen
* Firmware-Speicher
* MMIO-Bereichen
* ACPI-Reservierungen
* Bootloader-Speicher
* Kernel-Ladebereich

---

# 38. Speicherdeskriptor

Jeder Speicherbereich wird durch einen standardisierten Deskriptor beschrieben.

Beispiel:

```c id="9cn1kw"
typedef struct
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} nbis_memory_descriptor_t;
```

---

# 39. Speicherklassen

Folgende Speichertypen sind definiert:

* Available
* Reserved
* Firmware
* ACPI
* MMIO
* Bootloader
* Kernel
* Framebuffer
* Persistent Memory
* Unknown

Neue Typen dürfen ergänzt werden.

---

# 40. Reihenfolge

Die Speicherdeskriptoren werden nach aufsteigender Basisadresse sortiert.

Überlappende Einträge sind unzulässig.

---

# 41. CPU-Block

Der CPU-Block beschreibt die Prozessorumgebung.

Er dient ausschließlich der Initialisierung.

---

# 42. CPU-Informationen

Beispiele:

* Architektur
* Hersteller
* Modell
* Family
* Stepping
* Feature Flags
* APIC
* Anzahl logischer Prozessoren

---

# 43. CPU-Features

Mögliche Feature Flags:

* SSE
* SSE2
* SSE3
* AVX
* AVX2
* AVX-512
* AES
* NX
* SMEP
* SMAP
* VT-x
* SVM

---

# 44. Timerinformationen

Optional können folgende Timer beschrieben werden:

* PIT
* HPET
* APIC Timer
* TSC

---

# 45. Framebuffer-Block

Der Framebuffer-Block beschreibt ausschließlich den initialen Grafikpuffer.

Firmwareobjekte werden nicht übergeben.

---

# 46. Framebufferinformationen

Mindestens erforderlich:

* Basisadresse
* Breite
* Höhe
* Pitch
* Bits pro Pixel
* Pixelformat

---

# 47. Pixelformate

Unterstützte Standardformate:

* RGB565
* XRGB8888
* ARGB8888

Weitere Formate dürfen ergänzt werden.

---

# 48. Firmware-Block

Dieser Block beschreibt die Firmwareumgebung.

Beispiele:

* BIOS
* UEFI
* Coreboot
* Libreboot

---

# 49. Firmwareinformationen

Mögliche Felder:

* Firmwaretyp
* Versionsnummer
* Hersteller
* Startmodus
* Laufzeitdienste verfügbar

---

# 50. ACPI-Block

Der ACPI-Block enthält ausschließlich Zeiger auf gültige ACPI-Tabellen.

Die Interpretation erfolgt durch den Kernel.

---

# 51. Device-Tree-Block

Nicht-x86-Systeme können einen Device Tree übergeben.

Dadurch bleibt die NBIS plattformunabhängig.

---

# 52. Bootparameter

Bootparameter beschreiben das gewünschte Startverhalten.

Beispiele:

* Debug
* Safe Mode
* Verbose
* Recovery
* Single User
* No GUI

---

# 53. Befehlszeile

Optional kann eine standardisierte Kommandozeile übergeben werden.

Die Kodierung muss eindeutig definiert sein.

---

# 54. Modulblock

Dieser Block beschreibt sämtliche geladenen Bootmodule.

---

# 55. Modulinformationen

Für jedes Modul werden mindestens gespeichert:

* Modul-ID
* Version
* Speicheradresse
* Größe
* Status
* Signaturstatus

---

# 56. Sicherheitsblock

Der Sicherheitsblock beschreibt den Sicherheitszustand des Bootvorgangs.

---

# 57. Sicherheitsinformationen

Beispiele:

* Secure Boot aktiv
* TPM vorhanden
* Signaturprüfung erfolgreich
* Recovery gestartet
* Debugmodus

---

# 58. Kryptografische Informationen

Optional können enthalten sein:

* verwendeter Signaturalgorithmus
* Hashalgorithmus
* Zertifikatskennung
* Trust-Level

---

# 59. Erweiterungsblöcke

Neue Informationsarten werden ausschließlich über Erweiterungsblöcke eingeführt.

Vorhandene Blöcke bleiben unverändert.

---

# 60. Unbekannte Blöcke

Der Kernel muss unbekannte Blöcke sicher überspringen können.

Dies ermöglicht zukünftige Erweiterungen ohne ABI-Bruch.

---

# 61. Integritätsprüfung

Vor der Verarbeitung wird geprüft:

* Gesamtgröße
* Blockgröße
* Reihenfolge
* Überlappungen
* Alignment
* Versionsnummern

---

# 62. Speicherübernahme

Nach erfolgreicher Validierung kopiert der Kernel sämtliche benötigten Informationen in eigene interne Datenstrukturen.

Anschließend darf nicht mehr direkt auf die NBIS zugegriffen werden.

---

# 63. Lebensende

Nach Abschluss der Initialisierung besitzt die NBIS keine Funktion mehr.

Der zugehörige Speicher kann freigegeben oder anderweitig verwendet werden.

---

# 64. Referenzlayout

```text id="r2qsl9"
NBIS Header

↓

Memory Block

↓

CPU Block

↓

Framebuffer Block

↓

Firmware Block

↓

ACPI Block

↓

Device Tree Block

↓

Boot Parameters

↓

Modules

↓

Security

↓

Extensions
```

---

# 65. Referenzinitialisierung

```text id="7iqv3x"
Kernel Entry

↓

NBIS prüfen

↓

Version prüfen

↓

Header validieren

↓

Blöcke validieren

↓

Interne Daten erzeugen

↓

NBIS freigeben

↓

Kernelinitialisierung
```

---

# 66. Fehlerfälle

Typische Fehler:

* ungültige Magic Number
* beschädigte Blockgröße
* doppelte Pflichtblöcke
* fehlende Pflichtblöcke
* falsche Version
* fehlerhafte Prüfsumme

---

# 67. Recovery

Kann die NBIS nicht verarbeitet werden, darf der Kernel den Bootvorgang nicht fortsetzen.

Stattdessen erfolgt:

* Diagnose
* Fehlerprotokoll
* Recovery
* sicherer Neustart

---

# 68. Teststrategie

Die NBIS wird getestet durch:

* Unit-Tests
* ABI-Tests
* Fuzzing
* Negativtests
* Versionsmigrationen
* Regressionstests

---

# 69. Konformitätsanforderungen

Eine Implementierung erfüllt dieses RFC, wenn:

* jede NBIS einen gültigen Header besitzt,
* sämtliche Pflichtblöcke vorhanden sind,
* alle Blockgrößen validiert werden,
* unbekannte Erweiterungsblöcke übersprungen werden,
* ausschließlich standardisierte Datentypen verwendet werden,
* der Kernel alle benötigten Daten in eigene Strukturen übernimmt,
* und keine internen Bootloaderstrukturen direkt genutzt werden.

---

# 70. Referenzimplementierung

Die Referenzimplementierung dient ausschließlich als Nachweis der Spezifikation.

Abweichende Implementierungen sind zulässig, sofern sämtliche Anforderungen dieses RFC erfüllt werden.

---

# 71. Erweiterbarkeit

Zukünftige Versionen können zusätzliche Blöcke definieren.

Beispiele:

* NUMA
* IOMMU
* Virtualisierung
* PCI-Topologie
* Energieverwaltung
* Mikrocodeinformationen

---

# 72. Abwärtskompatibilität

Innerhalb einer Major-Version müssen neue NBIS-Versionen ältere Implementierungen weiterhin unterstützen, sofern ausschließlich kompatible Erweiterungen vorgenommen wurden.

---

# 73. Referenzdiagramm

```text id="i4lzhm"
Stage-2

↓

Kernel Loader

↓

Nova Boot Information Structure

↓

Kernel

↓

Interne Kernelobjekte
```

---

# 74. Zusammenfassung

Die **Nova Boot Information Structure (NBIS)** definiert das standardisierte Übergabeformat zwischen Bootloader und Kernel innerhalb des Nova Boot Protocols. Sie basiert auf einem selbstbeschreibenden, blockorientierten Aufbau mit versionierten Headern und klar definierten Informationsblöcken für Speicher, CPU, Grafik, Firmware, Bootparameter, Module und Sicherheitsinformationen.

Durch die strikte Trennung von Übergabedaten und internen Implementierungen ermöglicht die NBIS eine stabile, erweiterbare und plattformunabhängige Boot-ABI. Der Kernel validiert sämtliche Informationen, übernimmt sie in eigene Datenstrukturen und arbeitet anschließend vollständig unabhängig von den ursprünglichen Bootloaderdaten. Damit bildet die NBIS das zentrale Fundament für eine langfristig wartbare und kompatible NovaOS-Bootarchitektur.
