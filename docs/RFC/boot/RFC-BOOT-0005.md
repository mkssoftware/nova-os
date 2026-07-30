# RFC-BOOT-0005: Nova Kernel Image Format (NKIF) (Teil 1)

* **Dokumenttyp:** Request for Comments (RFC)
* **Dokument-ID:** RFC-BOOT-0005
* **Titel:** Nova Kernel Image Format (NKIF)
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Protocol / Kernelformat
* **Gültigkeitsbereich:** Kernel Loader, Stage-2, Build-System, Recovery
* **Abhängigkeiten:** RFC-BOOT-0001, RFC-BOOT-0002, RFC-BOOT-0003, ADR-BOOT-1019, ADR-BOOT-1020
* **Letzte Aktualisierung:** 2026-07-29

---

# Inhaltsverzeichnis

1. Einleitung
2. Motivation
3. Ziele
4. Nicht-Ziele
5. Architektur
6. Designprinzipien
7. Dateiformat
8. Kernel Header
9. Versionsverwaltung
10. Image Layout
11. Einstiegspunkt
12. Speicheranforderungen
13. Validierung
14. Sicherheitsmodell

---

# 1. Einleitung

Dieses Dokument definiert das **Nova Kernel Image Format (NKIF)**.

Das NKIF beschreibt das standardisierte Dateiformat ausführbarer NovaOS-Kernel.

Es dient als verbindliche Schnittstelle zwischen Kernel Loader und Kernel.

---

# 2. Motivation

Der Kernel Loader benötigt ein eindeutig beschriebenes Dateiformat.

Dieses muss:

* unabhängig vom Compiler,
* unabhängig vom Linker,
* versionierbar,
* validierbar,
* und langfristig stabil

sein.

---

# 3. Ziele

Das NKIF verfolgt folgende Ziele:

* standardisiertes Kernelformat
* eindeutige Identifikation
* Versionsverwaltung
* Integritätsprüfung
* kryptografische Signierung
* Erweiterbarkeit
* Plattformunabhängigkeit

---

# 4. Nicht-Ziele

Das NKIF beschreibt nicht:

* ELF
* PE
* Mach-O
* Userspace-Programme
* Treibermodule

Es beschreibt ausschließlich Kernelabbilder.

---

# 5. Architekturübersicht

```text id="d33uvg"
Kernel Image

↓

Kernel Loader

↓

Validierung

↓

Speicher

↓

Kernel Entry
```

---

# 6. Grundprinzipien

Das NKIF basiert auf folgenden Prinzipien:

* Selbstbeschreibend
* Versioniert
* Deterministisch
* Signierbar
* Erweiterbar
* Validierbar

---

# 7. Kernelabbild

Ein Kernelabbild besteht logisch aus:

* Header
* Metadaten
* Kernelcode
* Konstanten
* Initialisierungsdaten
* Signaturblock

---

# 8. Dateiendung

Empfohlene Dateiendung:

```text id="cjlwm0"
.nki
```

Alternative Endungen dürfen implementierungsabhängig unterstützt werden.

---

# 9. Dateiaufbau

```text id="g1qtfr"
Kernel Header

↓

Metadata

↓

Executable Image

↓

Optional Resources

↓

Signature Block
```

---

# 10. Kernel Header

Jedes Kernelabbild beginnt mit einem standardisierten Header.

---

# 11. Headerfelder

Mindestens erforderlich:

* Magic Number
* Formatversion
* Architektur
* Kernelversion
* Imagegröße
* Einstiegspunkt
* Prüfsumme

---

# 12. Beispielheader

```c id="hn8qkr"
typedef struct
{
    uint32_t magic;
    uint16_t major;
    uint16_t minor;
    uint32_t architecture;
    uint64_t image_size;
    uint64_t entry_point;
    uint32_t checksum;
} nkif_header_t;
```

---

# 13. Magic Number

Die Magic Number identifiziert eindeutig ein gültiges NKIF.

Ungültige Werte führen zum sofortigen Abbruch des Ladevorgangs.

---

# 14. Formatversion

Das Dateiformat besitzt:

* Major-Version
* Minor-Version

Inkompatible Änderungen erfordern eine neue Major-Version.

---

# 15. Kernelversion

Zusätzlich besitzt jedes Image eine Kernelversion.

Beispiel:

```text id="jlwm9q"
1.0.0
```

Diese Version beschreibt den Kernel selbst und nicht das Dateiformat.

---

# 16. Architekturkennung

Das Image definiert eindeutig seine Zielarchitektur.

Beispiele:

* x86
* x86_64
* ARM64
* RISC-V

---

# 17. Imagegröße

Die Imagegröße beschreibt den ausführbaren Kernel einschließlich aller eingebetteten Daten, jedoch ohne den optionalen Signaturblock.

---

# 18. Einstiegspunkt

Der Einstiegspunkt definiert die erste Kernelroutine.

Der Kernel Loader übergibt ausschließlich an diesen Einstiegspunkt.

---

# 19. Prüfsumme

Die Prüfsumme dient der Erkennung beschädigter Dateien.

Sie ersetzt keine kryptografische Signatur.

---

# 20. Kernelmetadaten

Zusätzliche Metadaten können enthalten:

* Buildnummer
* Builddatum
* Compiler
* Linker
* Zielplattform

---

# 21. Herstellerinformationen

Optional:

* Projektname
* Copyright
* Lizenz
* Maintainer

Diese Informationen dienen ausschließlich Dokumentationszwecken.

---

# 22. Buildinformationen

Optional:

* Git-Revision
* Buildsystem
* Optimierungsprofil
* Debugstatus

---

# 23. Speicheranforderungen

Das Image kann Speicheranforderungen definieren.

Beispiele:

* minimale RAM-Größe
* bevorzugte Ausrichtung
* benötigte Seitengröße

---

# 24. Unterstützte Seitengrößen

Beispiele:

* 4 KiB
* 16 KiB
* 64 KiB

Die konkrete Unterstützung ist architekturabhängig.

---

# 25. Kompressionsinformationen

Optional kann das Kernelabbild komprimiert sein.

Mögliche Algorithmen:

* keine
* LZ4
* Zstandard
* Deflate

Der Kernel Loader muss den verwendeten Algorithmus erkennen.

---

# 26. Dekompression

Die Dekompression erfolgt ausschließlich im Kernel Loader.

Der Kernel erhält immer ein vollständig entpacktes Image.

---

# 27. Ressourcen

Ein Kernelabbild kann optionale Ressourcen enthalten.

Beispiele:

* Bitmap-Fonts
* Panic-Icons
* Standardfarben
* Bootmeldungen

---

# 28. Relokation

Das NKIF unterstützt positionsunabhängige Kernelabbilder.

Der Kernel Loader darf das Image an einer geeigneten Speicheradresse platzieren.

---

# 29. Schreibbare Daten

Schreibbare Bereiche werden getrennt von ausführbarem Code behandelt.

Dadurch bleiben Code und Daten klar voneinander getrennt.

---

# 30. Ladealgorithmus

```text id="qb1tzu"
Datei öffnen

↓

Header lesen

↓

Version prüfen

↓

Prüfsumme prüfen

↓

Signatur prüfen

↓

Speicher reservieren

↓

Image laden

↓

Dekomprimieren

↓

Relokation

↓

Kernel Entry
```

---

# 31. Qualitätsanforderungen

Ein gültiges Kernelabbild muss:

* einen gültigen Header besitzen,
* eindeutig identifizierbar sein,
* eine Zielarchitektur definieren,
* versioniert sein,
* validierbar sein,
* einen Einstiegspunkt besitzen,
* und deterministisch geladen werden können.

---

# 32. Zusammenfassung Teil 1

Das **Nova Kernel Image Format (NKIF)** definiert ein standardisiertes, selbstbeschreibendes und versioniertes Dateiformat für NovaOS-Kernel. Es trennt Kernelcode, Metadaten und Integritätsinformationen klar voneinander und stellt dem Kernel Loader alle erforderlichen Informationen für Validierung, Dekompression, Relokation und Start des Kernels bereit.



# RFC-BOOT-0005: Nova Kernel Image Format (NKIF) (Teil 2)

---

# 33. Signaturblock

Jedes produktive Kernelabbild muss einen kryptografischen Signaturblock besitzen.

Der Signaturblock befindet sich logisch am Ende des Images und wird nicht in die ausführbaren Kerneldaten eingebunden.

---

# 34. Bestandteile des Signaturblocks

Der Signaturblock enthält mindestens:

* Signaturalgorithmus
* Hashalgorithmus
* Digitale Signatur
* Schlüsselkennung
* Signaturversion
* Zertifikatskennung (optional)

---

# 35. Unterstützte Hashalgorithmen

Empfohlene Algorithmen:

* SHA-256
* SHA-384
* SHA-512

Neue Algorithmen können in zukünftigen Versionen ergänzt werden.

---

# 36. Signaturalgorithmen

Empfohlen:

* Ed25519
* ECDSA P-256
* RSA-3072

Die konkrete Auswahl wird durch die NovaOS-Trust-Policy festgelegt.

---

# 37. Integritätsprüfung

Vor dem Laden überprüft der Kernel Loader:

* Header
* Formatversion
* Dateigröße
* Prüfsumme
* Hash
* Signatur

Erst nach erfolgreicher Prüfung darf der Kernel geladen werden.

---

# 38. Chain of Trust

Das Kernelabbild ist Bestandteil der NovaOS-Vertrauenskette.

```text id="vopf5r"
Firmware

↓

Stage-1

↓

Stage-2

↓

Kernel Loader

↓

Kernel Image

↓

Kernel
```

---

# 39. Image-Sektionen

Ein Kernelabbild besteht logisch aus mehreren Bereichen.

```text id="xqqv4o"
Header

↓

Metadata

↓

.text

↓

.rodata

↓

.data

↓

.bss

↓

Resources

↓

Extensions

↓

Signature
```

---

# 40. Codebereich

Der `.text`-Bereich enthält ausschließlich ausführbaren Maschinencode.

Er wird nach dem Laden schreibgeschützt markiert.

---

# 41. Konstantenbereich

Der `.rodata`-Bereich enthält:

* Zeichenketten
* Tabellen
* Konstanten
* Standardwerte

Dieser Bereich ist ausschließlich lesbar.

---

# 42. Initialisierte Daten

Der `.data`-Bereich enthält initialisierte, schreibbare Daten.

Er wird beim Laden vollständig in den Arbeitsspeicher kopiert.

---

# 43. Nicht initialisierte Daten

Der `.bss`-Bereich wird nicht im Image gespeichert.

Der Kernel Loader reserviert den benötigten Speicher und initialisiert ihn mit Nullwerten.

---

# 44. Ressourcenbereich

Optionale Ressourcen können eingebettet werden.

Beispiele:

* Panic-Icons
* Standard-Schriftarten
* Boot-Logos
* Sprachtabellen

---

# 45. Erweiterungsbereich

Neue Informationen werden ausschließlich über definierte Erweiterungsbereiche ergänzt.

Bestehende Sektionen bleiben unverändert.

---

# 46. Ladezustände

Während des Ladevorgangs besitzt ein Kernelabbild folgende Zustände:

```text id="q4lgh4"
FOUND

↓

VALIDATED

↓

LOADED

↓

DECOMPRESSED

↓

RELOCATED

↓

READY

↓

RUNNING
```

---

# 47. Fehlerzustände

Zusätzliche Fehlerzustände:

* INVALID_HEADER
* INVALID_SIGNATURE
* INVALID_CHECKSUM
* UNSUPPORTED_VERSION
* UNSUPPORTED_ARCHITECTURE
* OUT_OF_MEMORY
* RELOCATION_FAILED

---

# 48. Speicherreservierung

Vor dem Laden reserviert der Kernel Loader ausreichend Speicher für:

* Kernelcode
* Datenbereiche
* BSS
* Relokation
* Initialen Stack

---

# 49. Relokation

Unterstützt das Image Relokation, erfolgt diese vollständig im Kernel Loader.

Nach dem Aufruf des Einstiegspunkts sind keine weiteren Relokationen zulässig.

---

# 50. Einstiegspunkt

Der Einstiegspunkt wird genau einmal aufgerufen.

Der Aufruf erfolgt ausschließlich nach erfolgreicher:

* Validierung
* Dekompression
* Relokation
* Speicherinitialisierung

---

# 51. Boot Information Structure

Der Einstiegspunkt erhält ausschließlich die **Nova Boot Information Structure (NBIS)**.

Weitere Übergabeparameter sind nicht zulässig.

---

# 52. Speicherfreigabe

Nach erfolgreicher Kernelinitialisierung kann der Kernel:

* Bootloader-Speicher
* temporäre Dekompressionspuffer
* Ladepuffer

freigeben.

---

# 53. Fehlerbehandlung

Schlägt das Laden fehl, erfolgt kein Sprung in den Kernel.

Mögliche Maßnahmen:

* Fehlerprotokoll
* Recovery
* Rollback
* Neustart

---

# 54. Recovery

Recovery kann automatisch gestartet werden bei:

* beschädigtem Image
* ungültiger Signatur
* nicht unterstützter Version
* fehlender Kerneldatei

---

# 55. Logging

Der Kernel Loader protokolliert mindestens:

* Kernelversion
* Dateigröße
* Architektur
* Prüfergebnisse
* Ladezeit
* Rückgabestatus

---

# 56. Kompatibilität

Innerhalb derselben Major-Version müssen Kernel Loader und Kernel Image kompatibel bleiben.

Neue optionale Felder dürfen ältere Loader nicht beeinträchtigen.

---

# 57. Erweiterbarkeit

Neue Headerfelder werden ausschließlich kompatibel ergänzt.

Vorhandene Felder dürfen:

* ihre Position,
* ihre Größe,
* ihre Bedeutung

nicht verändern.

---

# 58. Parseranforderungen

Ein NKIF-Parser muss:

* deterministisch arbeiten,
* beschädigte Images erkennen,
* unbekannte Erweiterungen überspringen,
* alle Pflichtfelder validieren.

---

# 59. Referenzlayout

```text id="iq9y4t"
Kernel Header

↓

Metadata

↓

Executable Sections

↓

Resources

↓

Extensions

↓

Signature Block
```

---

# 60. Referenzladeablauf

```text id="rtpk8l"
Kernel öffnen

↓

Header validieren

↓

Version prüfen

↓

Prüfsumme prüfen

↓

Signatur prüfen

↓

Speicher reservieren

↓

Image laden

↓

Dekomprimieren

↓

Relokation

↓

NBIS vorbereiten

↓

Kernel Entry
```

---

# 61. Teststrategie

Das NKIF wird geprüft durch:

* Unit-Tests
* Signaturtests
* Kompatibilitätstests
* Fuzzing
* Regressionstests
* Integrations-Tests

---

# 62. Debugimages

Debug-Kernel verwenden dasselbe Dateiformat.

Zusätzliche Debuginformationen dürfen enthalten sein, ändern jedoch nicht die Struktur des NKIF.

---

# 63. Mehrarchitektur-Unterstützung

Mehrere Kernelabbilder für unterschiedliche Architekturen dürfen parallel innerhalb eines BootFS vorhanden sein.

Der Kernel Loader wählt automatisch das passende Image aus.

---

# 64. Kompression

Komprimierte Kernelimages müssen vor der Relokation vollständig entpackt werden.

Teilweise Dekompression wird nicht unterstützt.

---

# 65. Sicherheitsrichtlinien

Das NKIF erlaubt keine Veränderung sicherheitsrelevanter Informationen zur Laufzeit.

Signatur, Version und Header gelten nach erfolgreicher Prüfung als unveränderlich.

---

# 66. Konformitätsanforderungen

Eine Implementierung erfüllt dieses RFC, wenn:

* jedes Kernelabbild einen gültigen NKIF-Header besitzt,
* die Formatversion geprüft wird,
* Prüfsumme und Signatur erfolgreich validiert werden,
* unterstützte Architekturen eindeutig angegeben sind,
* Relokation gemäß Spezifikation erfolgt,
* ausschließlich die NBIS an den Kernel übergeben wird,
* und der Einstiegspunkt erst nach vollständiger Initialisierung aufgerufen wird.

---

# 67. Referenzimplementierung

Die Referenzimplementierung dient ausschließlich als Nachweis der Spezifikation.

Abweichende Implementierungen sind zulässig, sofern sämtliche Anforderungen dieses RFC erfüllt werden.

---

# 68. Zukunftserweiterungen

Das NKIF ist ausdrücklich auf langfristige Erweiterbarkeit ausgelegt.

Mögliche zukünftige Erweiterungen:

* verschlüsselte Kernelimages
* Delta-Updates
* mehrstufige Signaturen
* integrierte Mikrocodepakete
* Architektur-spezifische Erweiterungsblöcke
* optionale Debugsymbole
* integrierte Integritätsmessungen

---

# 69. Abwärtskompatibilität

Neue Minor-Versionen dürfen ausschließlich kompatible Erweiterungen enthalten.

Inkompatible Änderungen am Dateiformat erfordern eine neue Major-Version.

---

# 70. Zusammenfassung

Das **Nova Kernel Image Format (NKIF)** definiert ein standardisiertes, selbstbeschreibendes und kryptografisch abgesichertes Dateiformat für NovaOS-Kernel. Es trennt Header, Metadaten, ausführbare Sektionen, Ressourcen und Signaturinformationen klar voneinander und ermöglicht dem Kernel Loader eine deterministische Validierung, Dekompression, Relokation und Initialisierung des Kernelabbilds.

Durch konsequente Versionierung, standardisierte Ladezustände, klar definierte Sicherheitsmechanismen und die ausschließliche Übergabe der **Nova Boot Information Structure (NBIS)** entsteht eine langfristig stabile und erweiterbare Schnittstelle zwischen Bootumgebung und Kernel. Das NKIF bildet damit die verbindliche Grundlage für alle ausführbaren Kernelimages innerhalb der NovaOS-Bootarchitektur.
