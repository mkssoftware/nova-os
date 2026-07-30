# RFC-BOOT-0001: Nova Boot Protocol (Teil 1)

* **Dokumenttyp:** Request for Comments (RFC)
* **Dokument-ID:** RFC-BOOT-0001
* **Titel:** Nova Boot Protocol
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture
* **Gültigkeitsbereich:** NovaOS Bootloader, Kernel, BootFS und Bootmodule
* **Autorität:** Nova Platform Architecture
* **Sprache:** Deutsch
* **Letzte Aktualisierung:** 2026-07-29

---

# Inhaltsverzeichnis

1. Einleitung
2. Motivation
3. Ziele
4. Nicht-Ziele
5. Begriffe
6. Architekturübersicht
7. Designprinzipien
8. Bootphasen
9. Verantwortlichkeiten
10. Bootzustände
11. Boot Module Manager
12. Boot ABI
13. BootFS
14. Kernel Loader
15. Recovery
16. Sicherheitsmodell
17. Erweiterbarkeit

---

# 1. Einleitung

Dieses Dokument beschreibt das **Nova Boot Protocol (NBP)**.

Es definiert den vollständigen Startvorgang von NovaOS – beginnend mit der Firmware bis zur Übergabe an den Kernel.

Das Ziel ist die Schaffung eines vollständig dokumentierten, modularen und langfristig stabilen Bootprotokolls, das unabhängig von einer bestimmten Firmware oder Prozessorarchitektur implementiert werden kann.

---

# 2. Motivation

Die meisten bestehenden Bootprotokolle sind entweder:

* eng an eine Firmware gebunden,
* historisch gewachsen,
* schwer erweiterbar,
* oder nur unvollständig dokumentiert.

NovaOS verfolgt deshalb einen vollständig dokumentierten Ansatz.

Das Bootprotokoll soll:

* verständlich,
* reproduzierbar,
* modular,
* versioniert
* und langfristig wartbar

sein.

---

# 3. Ziele

Das Nova Boot Protocol verfolgt folgende Hauptziele:

* klar definierte Bootphasen
* eindeutige Verantwortlichkeiten
* stabile ABI
* modulare Architektur
* Firmwareunabhängigkeit
* Plattformunabhängigkeit
* hohe Sicherheit
* Erweiterbarkeit
* deterministischer Ablauf

---

# 4. Nicht-Ziele

Dieses RFC definiert nicht:

* interne Scheduler
* Dateisystemdetails von NovaFS
* Window Manager
* Desktop
* Benutzerverwaltung
* Netzwerkprotokolle

Diese Themen werden in eigenen RFCs beschrieben.

---

# 5. Begriffe

## Stage-1

Erste Bootstufe.

Verantwortlich für:

* minimale Hardwareinitialisierung
* Laden von Stage-2

---

## Stage-2

Erweiterte Bootumgebung.

Verantwortlich für:

* Grafik
* Eingabe
* BootFS
* Bootmodule
* Recovery
* Kernel Loader

---

## Boot Module

Eigenständige ausführbare Komponenten innerhalb der Bootumgebung.

Beispiele:

* Recovery
* Diagnose
* Memtest
* Kernel Loader

---

## BootFS

Dateisystem für Bootkomponenten.

---

## Boot ABI

Versionierte Datenschnittstelle zwischen Bootloader und Kernel.

---

## Kernel Loader

Komponente zum Laden und Verifizieren des Kernels.

---

# 6. Architekturübersicht

Die Architektur besteht aus mehreren unabhängigen Komponenten.

```text id="p6m3hz"
Firmware

↓

Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Bootmodule

↓

Kernel Loader

↓

Kernel
```

Jede Ebene besitzt klar definierte Aufgaben.

---

# 7. Designprinzipien

Das Nova Boot Protocol folgt den folgenden Grundprinzipien.

## 7.1 Modularität

Jede Komponente besitzt genau einen Verantwortungsbereich.

---

## 7.2 Geringe Kopplung

Bootkomponenten kommunizieren ausschließlich über definierte Schnittstellen.

---

## 7.3 Versionierung

Alle öffentlichen Schnittstellen werden versioniert.

---

## 7.4 Erweiterbarkeit

Neue Funktionen dürfen ergänzt werden, ohne bestehende Implementierungen zu brechen.

---

## 7.5 Plattformunabhängigkeit

Das Protokoll funktioniert unabhängig von:

* BIOS
* UEFI
* x86
* ARM64
* RISC-V

---

## 7.6 Firmwareunabhängigkeit

Firmware stellt ausschließlich die minimale Startumgebung bereit.

Die eigentliche Bootarchitektur gehört vollständig zu NovaOS.

---

## 7.7 Determinismus

Identische Eingaben müssen stets identische Bootabläufe erzeugen.

---

## 7.8 Dokumentationspflicht

Jede öffentliche Struktur wird vollständig dokumentiert.

---

# 8. Bootphasen

Der Bootvorgang besteht aus mehreren logisch getrennten Phasen.

```text id="gj5mwj"
Firmware

↓

Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Kernel Loader

↓

Kernel

↓

Kernel Initialization

↓

Userspace
```

---

# 9. Firmwarephase

Die Firmware übernimmt ausschließlich:

* Hardwaregrundinitialisierung
* Speicherbereitstellung
* Start des ersten Bootcodes

Weitere Aufgaben gehören nicht zur Firmware.

---

# 10. Stage-1

Stage-1 besitzt ausschließlich minimale Aufgaben.

### Verantwortlichkeiten

* CPU vorbereiten
* Stack initialisieren
* Stage-2 finden
* Stage-2 laden
* Signatur prüfen
* Übergabe

Nicht Bestandteil:

* Dateisystemparser
* Recovery
* PNG
* TTF
* Netzwerk
* Kernel

---

# 11. Stage-2

Stage-2 bildet die eigentliche Bootumgebung.

Sie initialisiert:

* Grafik
* Maus
* Tastatur
* Speicherverwaltung
* BootFS
* Modulverwaltung

---

# 12. Boot Module Manager

Der Boot Module Manager verwaltet sämtliche Bootmodule.

Aufgaben:

* Module finden
* Module prüfen
* Module laden
* Module starten

---

# 13. Bootmodule

Bootmodule sind vollständig voneinander getrennt.

Beispiele:

```text id="4p2ryj"
Recovery

Diagnose

Memtest

Kernel Loader
```

Weitere Module können ergänzt werden.

---

# 14. Kernel Loader

Der Kernel Loader besitzt ausschließlich folgende Aufgaben:

* Kernel finden
* Signatur prüfen
* Kernel laden
* Boot ABI erzeugen
* Kontrolle übergeben

---

# 15. Recovery

Recovery ist ein eigenständiges Bootmodul.

Es besitzt:

* eigene Oberfläche
* eigene Initialisierung
* eigene Fehlerbehandlung

Recovery ist unabhängig vom Kernel.

---

# 16. BootFS

BootFS dient als Speicherort sämtlicher Bootkomponenten.

Typische Inhalte:

```text id="n55d4z"
stage2

kernel_loader

recovery

diagnose

kernel

fonts

themes

icons
```

Die konkrete Spezifikation erfolgt in **RFC-BOOTFS-0001**.

---

# 17. Boot ABI

Die Kommunikation zwischen Bootloader und Kernel erfolgt ausschließlich über das Nova Boot ABI.

Eigenschaften:

* versioniert
* dokumentiert
* validierbar
* sprachunabhängig

---

# 18. Speicherübergabe

Der Bootloader beschreibt den verfügbaren Arbeitsspeicher über standardisierte ABI-Strukturen.

Interne Speicherverwaltung wird nicht übergeben.

---

# 19. Grafikübergabe

Der Kernel erhält ausschließlich:

* Framebufferadresse
* Auflösung
* Pitch
* Pixelformat

Nicht jedoch:

* VBE-Strukturen
* GOP-Handles
* Firmwareobjekte

---

# 20. CPU-Informationen

Der Bootloader stellt CPU-Informationen bereit.

Beispiele:

* Architektur
* Features
* APIC
* TSC
* SIMD-Unterstützung

---

# 21. ACPI

Falls vorhanden, werden ACPI-Informationen standardisiert an den Kernel übergeben.

---

# 22. Device Tree

Nicht-x86-Plattformen können stattdessen Device Trees verwenden.

Das Bootprotokoll unterstützt beide Verfahren.

---

# 23. Sicherheitsmodell

Jede ausführbare Bootkomponente wird vor ihrer Ausführung kryptografisch verifiziert.

Die Signaturprüfung erfolgt gemäß **ADR-BOOT-1019**.

---

# 24. Chain of Trust

Die Vertrauenskette lautet:

```text id="p90s6u"
Firmware

↓

Stage-1

↓

Stage-2

↓

Bootmodule

↓

Kernel Loader

↓

Kernel
```

---

# 25. Fehlerbehandlung

Fehler führen niemals zu undefiniertem Verhalten.

Mögliche Reaktionen:

* Retry
* Recovery
* Fehlermeldung
* sicherer Abbruch

---

# 26. Versionierung

Alle öffentlichen Schnittstellen besitzen:

* Major
* Minor
* optionale Patch-Version

---

# 27. Erweiterbarkeit

Neue Funktionen werden ausschließlich über definierte Erweiterungsmechanismen ergänzt.

Bereits dokumentierte Strukturen bleiben kompatibel.

---

# 28. Qualitätsanforderungen

Das Nova Boot Protocol muss:

* modular
* nachvollziehbar
* dokumentiert
* testbar
* sicher
* deterministisch
* firmwareunabhängig
* langfristig stabil

sein.

---

# 29. Zusammenfassung Teil 1

Das Nova Boot Protocol definiert eine klar geschichtete Bootarchitektur, in der jede Komponente einen eindeutig abgegrenzten Verantwortungsbereich besitzt. Stage-1 bleibt bewusst minimal und lädt ausschließlich Stage-2. Stage-2 stellt die eigentliche Bootumgebung bereit, verwaltet Bootmodule, initialisiert die benötigte Infrastruktur und startet über den Kernel Loader den Kernel. Sämtliche Kommunikation erfolgt über ein versioniertes Boot-ABI, während Integrität und Authentizität aller ausführbaren Komponenten durch eine kryptografische Vertrauenskette sichergestellt werden.


# RFC-BOOT-0001: Nova Boot Protocol (Teil 2)

---

# 30. Bootzustandsmaschine

Der Bootprozess wird als deterministische Zustandsmaschine definiert.

Jeder Zustand besitzt:

* einen eindeutigen Namen,
* definierte Ein- und Austrittsbedingungen,
* erlaubte Folgezustände,
* definierte Fehlerpfade.

---

# 31. Zustandsdiagramm

```text
POWER_ON
    │
    ▼
FIRMWARE
    │
    ▼
STAGE1
    │
    ▼
STAGE2
    │
    ▼
MODULE_MANAGER
    │
    ├─► RECOVERY
    │
    ├─► DIAGNOSE
    │
    ├─► MEMTEST
    │
    ▼
KERNEL_LOADER
    │
    ▼
KERNEL_ENTRY
    │
    ▼
KERNEL_INIT
    │
    ▼
USERSPACE
```

---

# 32. Zustand POWER_ON

Das System befindet sich unmittelbar nach dem Einschalten.

Eigenschaften:

* CPU im Resetzustand
* Speicher nicht initialisiert
* Firmware übernimmt Kontrolle

---

# 33. Zustand FIRMWARE

Verantwortlichkeiten:

* Hardwareinitialisierung
* Speicherbereitstellung
* Bootmedium bestimmen
* Stage-1 laden

Die Firmware kennt keine NovaOS-internen Strukturen.

---

# 34. Zustand STAGE1

Stage-1 besitzt ausschließlich folgende Aufgaben:

* Stack initialisieren
* CPU vorbereiten
* Stage-2 finden
* Stage-2 verifizieren
* Stage-2 starten

---

# 35. Zustand STAGE2

Stage-2 initialisiert:

* Framebuffer
* Eingabesystem
* BootFS
* Modulverwaltung
* ABI-Grundstrukturen

---

# 36. Zustand MODULE_MANAGER

Der Boot Module Manager entscheidet, welches Bootmodul gestartet wird.

Mögliche Entscheidungen:

* Kernel Loader
* Recovery
* Diagnose
* Memtest
* zukünftige Erweiterungen

---

# 37. Zustand RECOVERY

Recovery besitzt einen vollständig eigenständigen Ablauf.

Mögliche Aktionen:

* Rollback
* Snapshot-Wiederherstellung
* Bootreparatur
* Datenträgerdiagnose
* Dateimanager

Nach Abschluss kann entweder:

* neu gestartet,
* das Bootmenü angezeigt,
* oder der Kernel geladen werden.

---

# 38. Zustand DIAGNOSE

Diagnosemodule dienen ausschließlich der Analyse.

Beispiele:

* CPU-Informationen
* RAM-Test
* Bootprotokoll
* Hardwareübersicht
* Signaturstatus

---

# 39. Zustand KERNEL_LOADER

Der Kernel Loader führt aus:

* Kernel lokalisieren
* Signatur prüfen
* Speicher reservieren
* Kernel laden
* Boot-ABI erzeugen

---

# 40. Zustand KERNEL_ENTRY

Ab diesem Zeitpunkt besitzt der Kernel die Kontrolle.

Der Bootloader beendet seine aktive Rolle.

---

# 41. Kernelinitialisierung

Der Kernel übernimmt:

* Speicherverwaltung
* Interruptsystem
* Scheduler
* Gerätetreiber
* Userspace-Start

Diese Schritte gehören nicht mehr zum Bootprotokoll.

---

# 42. Bootmodulformat

Jedes Bootmodul besitzt einen standardisierten Aufbau.

```text
Header

↓

Metadaten

↓

Code

↓

Daten

↓

Signatur
```

---

# 43. Modulheader

Ein Modulheader enthält mindestens:

* Magic Number
* Modul-ID
* Version
* Architektur
* Dateigröße
* Einstiegspunkt

---

# 44. Modul-ID

Jedes Bootmodul besitzt eine weltweit eindeutige Modulkennung.

Beispiele:

```text
BOOT_RECOVERY

BOOT_KERNEL_LOADER

BOOT_MEMTEST

BOOT_DIAGNOSE
```

---

# 45. Modulversion

Jedes Modul besitzt:

* Major
* Minor
* Patch

---

# 46. Modulstatus

Ein Modul kann folgende Zustände besitzen:

* vorhanden
* geladen
* geprüft
* gestartet
* beendet
* fehlerhaft

---

# 47. Modulabhängigkeiten

Bootmodule dürfen Abhängigkeiten definieren.

Beispiel:

```text
Recovery

↓

Framebuffer

↓

Input

↓

BootFS
```

---

# 48. Boot-ABI

Das Nova Boot ABI bildet die einzige Kommunikationsschnittstelle zwischen Bootloader und Kernel.

Eigenschaften:

* sprachunabhängig
* compilerunabhängig
* versioniert
* dokumentiert

---

# 49. ABI-Header

Jede ABI-Struktur beginnt mit:

* Magic Number
* Version
* Größe
* Typkennung

---

# 50. ABI-Erweiterungen

Neue Daten werden ausschließlich über definierte Erweiterungsblöcke ergänzt.

Bereits definierte Felder bleiben unverändert.

---

# 51. Speicherlayout

Der Bootloader beschreibt den physikalischen Speicher ausschließlich über das ABI.

Nicht übergeben werden interne Verwaltungsstrukturen.

---

# 52. Framebuffer

Die Grafikübergabe umfasst mindestens:

* Adresse
* Breite
* Höhe
* Pitch
* Bits pro Pixel
* Pixelformat

---

# 53. CPU-Informationen

Die CPU-Beschreibung enthält beispielsweise:

* Architektur
* Feature Bits
* APIC
* Anzahl Kerne
* Timerinformationen

---

# 54. Plattforminformationen

Zusätzliche Plattforminformationen können enthalten:

* BIOS
* UEFI
* Device Tree
* ACPI

Die konkrete Quelle bleibt für den Kernel transparent.

---

# 55. Bootparameter

Bootparameter werden in standardisierter Form übergeben.

Beispiele:

* Debugmodus
* Safe Mode
* Recovery erzwingen
* Konsoleneinstellungen

---

# 56. Fehlercodes

Das Bootprotokoll definiert standardisierte Fehlercodes.

Beispiele:

* ungültige Signatur
* Modul fehlt
* ABI inkompatibel
* Kernel nicht gefunden
* Speicherfehler

---

# 57. Logging

Während des Bootvorgangs dürfen Protokolle erzeugt werden.

Diese können später:

* im Recovery,
* im Kernel
* oder im Diagnosesystem

ausgewertet werden.

---

# 58. Recovery-Integration

Recovery ist vollständig in den Bootablauf integriert.

Es kann automatisch gestartet werden bei:

* Signaturfehlern
* Kernelproblemen
* beschädigtem BootFS
* fehlgeschlagenen Updates

---

# 59. Sicherheitsrichtlinien

Alle ausführbaren Komponenten müssen:

* kryptografisch geprüft,
* versionsgeprüft,
* validiert

werden.

---

# 60. Erweiterungsmechanismus

Neue Bootmodule können ergänzt werden, ohne bestehende Module zu verändern.

Der Boot Module Manager erkennt neue Module automatisch anhand ihres Modulheaders.

---

# 61. Kompatibilität

Innerhalb derselben Major-Version müssen Bootloader und Kernel kompatibel bleiben.

Neue optionale Funktionen dürfen ältere Implementierungen nicht beeinträchtigen.

---

# 62. Referenzstart

Referenzablauf:

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Kernel Loader

↓

Kernel

↓

Userspace
```

---

# 63. Referenzstart mit Recovery

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Recovery

↓

Bootmenü

↓

Kernel Loader

↓

Kernel
```

---

# 64. Referenzstart nach Fehler

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Kernel Loader

↓

Signaturfehler

↓

Recovery

↓

Rollback

↓

Neustart
```

---

# 65. Konformitätsanforderungen

Eine Implementierung erfüllt dieses RFC, wenn:

* Stage-1 ausschließlich minimale Aufgaben übernimmt,
* Stage-2 die Bootumgebung bereitstellt,
* Bootmodule standardisierte Modulheader verwenden,
* sämtliche Kommunikation über das Nova Boot ABI erfolgt,
* alle ausführbaren Komponenten kryptografisch verifiziert werden,
* Recovery unabhängig vom Kernel funktioniert,
* und der Bootablauf deterministisch sowie vollständig dokumentiert ist.

---

# 66. Implementierungsreihenfolge

Empfohlene Reihenfolge:

1. Stage-1
2. Stage-2
3. Framebuffer
4. BootFS
5. Boot Module Manager
6. Signaturbibliothek
7. Recovery
8. Kernel Loader
9. Boot-ABI
10. Kernel

---

# 67. Referenzimplementierung

Die Referenzimplementierung dient ausschließlich als Nachweis der Spezifikation.

Andere Implementierungen sind zulässig, sofern sie sämtliche Anforderungen dieses RFC erfüllen.

---

# 68. Zukunftserweiterungen

Das Nova Boot Protocol ist ausdrücklich auf Erweiterbarkeit ausgelegt.

Mögliche zukünftige Erweiterungen:

* Netzwerkboot
* Cluster-Boot
* Remote-Recovery
* Hardware-Attestation
* Mehrkern-Initialisierung
* Virtuelle Maschinen
* Hot-Plug-Unterstützung

---

# 69. Abwärtskompatibilität

Neue Versionen des Bootprotokolls sollen bestehende Implementierungen möglichst nicht beeinträchtigen.

Inkompatible Änderungen erfordern eine neue Major-Version.

---

# 70. Zusammenfassung

Das **Nova Boot Protocol (NBP)** definiert eine vollständig modulare und versionierte Bootarchitektur für NovaOS. Der Bootprozess ist in klar abgegrenzte Phasen unterteilt, die jeweils genau definierte Verantwortlichkeiten besitzen. Stage-1 bleibt bewusst minimal, Stage-2 stellt die eigentliche Bootumgebung bereit, und alle weiteren Funktionen – einschließlich Recovery und Kernel Loader – werden als eigenständige Bootmodule ausgeführt.

Die Kommunikation zwischen Bootloader und Kernel erfolgt ausschließlich über das versionierte **Nova Boot ABI**. Durch kryptografisch signierte Bootkomponenten, eine durchgängige Vertrauenskette, standardisierte Modulformate und klar definierte Erweiterungsmechanismen entsteht ein Bootprotokoll, das unabhängig von Firmware, Prozessorarchitektur und internen Implementierungsdetails langfristig stabil, sicher und erweiterbar bleibt.
