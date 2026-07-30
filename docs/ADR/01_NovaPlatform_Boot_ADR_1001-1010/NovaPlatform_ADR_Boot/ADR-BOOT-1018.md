# ADR-BOOT-1018: Recovery als Bootloader-Modul (Teil 1)

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1018
* **Titel:** Recovery als Bootloader-Modul
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture / Recovery
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** NovaOS Bootloader, Recovery-System, BootFS und Kernelstart

---

# Referenzen

* ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* ADR-BOOT-1013 – Stage-1 bleibt minimal
* ADR-BOOT-1016 – Framebuffer-Abstraktion statt direkter VBE-Nutzung
* NPSPEC-BOOTFS-0001
* NPSPEC-BOOTERROR-0001
* NPSPEC-KERNELLOADER-0001

---

# 1. Kontext

NovaOS soll über ein integriertes Wiederherstellungssystem verfügen.

Das Recovery-System soll bereits vor dem eigentlichen Kernelstart verfügbar sein.

Dadurch können Fehler behoben werden, selbst wenn der Kernel nicht mehr gestartet werden kann.

---

# 2. Problemstellung

Es muss entschieden werden, wo das Recovery-System architektonisch angesiedelt wird.

Mögliche Varianten:

* Bestandteil von Stage-1
* Bestandteil von Stage-2
* Bestandteil des Kernels
* Eigenständiges Bootloader-Modul
* Externes Rettungsmedium

Jede Variante besitzt unterschiedliche Vor- und Nachteile.

---

# 3. Entscheidung

Das Recovery-System wird als **eigenständiges Bootloader-Modul** implementiert.

Die Architektur lautet:

```text id="8kdv7x"
Stage-1

↓

Stage-2

↓

Recovery-Modul

↓

Kernel Loader

↓

Kernel
```

Das Recovery-Modul ist unabhängig vom eigentlichen Kernel.

---

# 4. Ziele

Diese Architektur verfolgt folgende Ziele:

* Wiederherstellung ohne Kernel
* geringe Kopplung
* modulare Bootarchitektur
* einfache Erweiterbarkeit
* klare Verantwortlichkeiten
* hohe Ausfallsicherheit

---

# 5. Warum kein Kernel?

Ein beschädigter Kernel darf das Recovery-System nicht unbrauchbar machen.

Das Recovery-System muss auch dann funktionieren, wenn:

* Kernel beschädigt
* Kernel gelöscht
* Kernel inkompatibel
* Kernelupdate fehlgeschlagen

ist.

---

# 6. Warum kein Stage-1?

Stage-1 besitzt bewusst nur minimale Aufgaben.

Ein Recovery-System würde:

* Codegröße erhöhen
* Komplexität erhöhen
* Testaufwand steigern

und damit gegen ADR-BOOT-1013 verstoßen.

---

# 7. Eigenständiges Modul

Recovery wird als eigenständiges Bootartefakt behandelt.

Beispiel:

```text id="2fd3vw"
boot/

stage1.bin

stage2.bin

recovery.mod

kernel_loader.mod
```

---

# 8. Vorteile

Das Recovery-Modul kann unabhängig aktualisiert werden.

Der Bootloader selbst bleibt weitgehend unverändert.

---

# 9. Verantwortlichkeiten

Das Recovery-Modul ist verantwortlich für:

* Wiederherstellung
* Diagnose
* Bootreparatur
* Dateisystemprüfung
* Snapshot-Auswahl
* Wiederanlauf

Nicht verantwortlich für:

* Desktop
* Benutzeranmeldung
* Anwendungen

---

# 10. Architektur

```text id="9q7e6j"
Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Recovery

↓

Kernel Loader
```

---

# 11. Modulprinzip

Recovery besitzt dieselbe Modulstruktur wie andere Bootkomponenten.

Dadurch entsteht keine Sonderbehandlung.

---

# 12. Boot Module Manager

Stage-2 lädt nicht unmittelbar den Kernel.

Stattdessen entscheidet zunächst ein Boot Module Manager, welches Modul gestartet wird.

Beispiele:

* Kernel Loader
* Recovery
* Diagnose
* Memtest
* Rescue

---

# 13. Recovery-Auswahl

Recovery kann gestartet werden durch:

* Tastenkombination
* Bootmenü
* Bootfehler
* automatische Erkennung
* Konfiguration

---

# 14. Automatische Aktivierung

Recovery darf automatisch gestartet werden, wenn:

* Kernel mehrfach abstürzt
* Bootloader Kernel nicht laden kann
* BootFS beschädigt ist
* Update unvollständig ist

---

# 15. Manueller Start

Der Benutzer kann Recovery jederzeit über das Bootmenü auswählen.

---

# 16. Eigenes Modul

Recovery besitzt:

* eigenen Einstiegspunkt
* eigene Initialisierung
* eigene Oberfläche
* eigene Fehlerbehandlung

---

# 17. Keine Kernelabhängigkeit

Recovery verwendet keine Kernel-APIs.

Es arbeitet vollständig eigenständig.

---

# 18. Gemeinsame Bibliotheken

Recovery darf gemeinsame Bibliotheken verwenden.

Beispiele:

* Grafik
* Fonts
* PNG
* Framebuffer
* Eingabe
* BootFS

---

# 19. Grafik

Recovery verwendet dieselbe Nova-Framebuffer-Abstraktion wie der Bootloader.

---

# 20. Oberfläche

Recovery besitzt eine moderne grafische Oberfläche.

Sie verwendet:

* Fenster
* Listen
* Buttons
* Dialoge
* Fortschrittsanzeigen

---

# 21. Einheitliches Design

Recovery verwendet dieselbe Designsprache wie NovaOS.

Dadurch entsteht ein konsistenter Gesamteindruck.

---

# 22. Navigation

Die Navigation unterstützt:

* Tastatur
* Maus
* spätere Touch-Unterstützung

---

# 23. Sprache

Recovery unterstützt später Mehrsprachigkeit.

---

# 24. BootFS

Recovery besitzt vollständigen Zugriff auf BootFS.

---

# 25. Dateisysteme

Recovery darf Dateisysteme lesen und schreiben.

Beispielsweise:

* NovaFS
* FAT32
* EFI-Systempartition

Weitere Dateisysteme können später ergänzt werden.

---

# 26. Snapshot-Unterstützung

Recovery darf vorhandene Snapshots anzeigen.

---

# 27. Snapshot-Wiederherstellung

Recovery kann frühere Systemstände wiederherstellen.

---

# 28. Rollback

Nach einem fehlerhaften Update kann automatisch ein Rollback erfolgen.

---

# 29. Integritätsprüfung

Recovery darf Integritätsprüfungen durchführen.

Beispiele:

* Prüfsummen
* Bootdateien
* Module
* Kernel

---

# 30. Reparatur

Recovery darf:

* Bootkonfiguration reparieren
* Module ersetzen
* Bootdateien neu erzeugen
* BootFS reparieren

---

# 31. Modulare Erweiterung

Neue Recovery-Funktionen können als zusätzliche Module integriert werden.

---

# 32. Trennung

Recovery besitzt einen klar abgegrenzten Verantwortungsbereich.

Es ersetzt weder den Kernel noch den Installer.

---

# 33. Fehlerisolierung

Ein Fehler im Recovery-Modul darf den Bootloader nicht beschädigen.

---

# 34. Aktualisierung

Recovery kann unabhängig vom Kernel aktualisiert werden.

---

# 35. Sicherheitsmodell

Recovery läuft mit hohen Rechten.

Es muss deshalb besonders sorgfältig validiert werden.

---

# 36. Signaturen

Recovery-Module sollen später digital signiert werden.

---

# 37. Boot-Handoff

Recovery erhält dieselben Bootinformationen wie der Kernel Loader.

---

# 38. Ressourcen

Recovery darf:

* Grafik
* Eingabe
* Speicher
* BootFS

verwenden.

---

# 39. Speicherverwaltung

Recovery verwendet die Bootloader-Speicherverwaltung.

Eine vollständige Kernel-Speicherverwaltung ist nicht erforderlich.

---

# 40. Zusammenfassung Teil 1

Das Recovery-System wird nicht Bestandteil von Stage-1 und auch nicht Teil des Kernels. Stattdessen wird es als eigenständiges Bootloader-Modul implementiert. Dadurch bleibt Stage-1 minimal, der Kernel unabhängig und das Recovery-System kann selbst dann arbeiten, wenn der eigentliche Kernel nicht mehr startfähig ist.


# ADR-BOOT-1018: Recovery als Bootloader-Modul (Teil 2)

---

# 41. Modulverwaltung

Der Boot Module Manager verwaltet sämtliche Bootmodule nach einem einheitlichen Schema.

Jedes Modul besitzt mindestens:

* Modul-ID
* Name
* Version
* Einstiegspunkt
* Abhängigkeiten
* Signaturstatus
* Kompatibilitätsinformationen

---

# 42. Modulregistrierung

Recovery wird wie jedes andere Bootmodul registriert.

Beispiel:

```text
Recovery
Version: 1.0
Typ: Boot Module
Priorität: Optional
```

---

# 43. Modulinitialisierung

Nach dem Laden führt Recovery ausschließlich seine eigene Initialisierung durch.

Diese umfasst beispielsweise:

* Speicherprüfung
* Initialisierung der Grafikbibliothek
* Eingabesystem
* BootFS-Zugriff
* Benutzeroberfläche

---

# 44. Startablauf

Der typische Ablauf lautet:

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Recovery

↓

Benutzeroberfläche
```

---

# 45. Fehlerbehandlung

Tritt innerhalb des Recovery-Moduls ein Fehler auf, darf dies nicht zum Absturz des gesamten Bootloaders führen.

Stattdessen muss:

* eine Fehlermeldung angezeigt werden,
* ein Neustart möglich sein,
* oder zum Bootmenü zurückgekehrt werden.

---

# 46. Diagnosefunktionen

Das Recovery-Modul kann Diagnosefunktionen bereitstellen.

Beispiele:

* CPU-Informationen
* Arbeitsspeicherinformationen
* Bootkonfiguration
* erkannte Datenträger
* erkannte Partitionen
* installierte Bootmodule

---

# 47. Datenträgerverwaltung

Recovery darf Datenträger verwalten.

Unterstützte Funktionen können sein:

* Partitionen anzeigen
* Partitionen prüfen
* Partitionen einbinden
* Partitionstabellen analysieren

---

# 48. Dateiverwaltung

Ein einfacher Dateimanager darf Bestandteil des Recovery-Moduls sein.

Er ermöglicht:

* Dateien kopieren
* Dateien löschen
* Dateien umbenennen
* Bootdateien ersetzen
* Konfigurationsdateien bearbeiten

---

# 49. Protokollierung

Recovery erstellt eigene Logdateien.

Diese können gespeichert werden:

* auf BootFS
* auf einer separaten Recovery-Partition
* auf externen Datenträgern

---

# 50. Notfallmodus

Falls grafische Komponenten nicht verfügbar sind, muss Recovery in einen einfachen Textmodus wechseln können.

---

# 51. Selbstdiagnose

Beim Start führt Recovery eine interne Selbstprüfung durch.

Geprüft werden unter anderem:

* Modulintegrität
* Speicherzugriff
* BootFS-Verfügbarkeit
* Grafikinitialisierung

---

# 52. Backup-Unterstützung

Recovery unterstützt später Systembackups.

Beispiele:

* vollständige Systemabbilder
* inkrementelle Sicherungen
* Konfigurationssicherungen

---

# 53. Wiederherstellung

Recovery kann verschiedene Wiederherstellungsarten unterstützen:

* vollständige Wiederherstellung
* selektive Wiederherstellung
* Bootreparatur
* Konfigurationsrücksetzung

---

# 54. Update-Rettung

Nach einem fehlgeschlagenen Systemupdate kann Recovery automatisch:

* den vorherigen Zustand wiederherstellen,
* das Update zurücknehmen,
* oder den Benutzer durch den Reparaturprozess führen.

---

# 55. Modulabhängigkeiten

Recovery besitzt möglichst wenige Abhängigkeiten.

Pflichtabhängigkeiten beschränken sich auf:

* Speicherverwaltung
* Grafik
* Eingabe
* BootFS

---

# 56. Ressourcenverbrauch

Recovery soll ressourcenschonend arbeiten.

Nicht benötigte Komponenten dürfen erst bei Bedarf geladen werden.

---

# 57. Erweiterbarkeit

Neue Recovery-Funktionen werden als eigenständige Erweiterungsmodule integriert.

Dadurch bleibt der Kern des Recovery-Moduls übersichtlich.

---

# 58. API

Recovery verwendet definierte Bootloader-Schnittstellen.

Direkte Zugriffe auf interne Bootloaderstrukturen sind zu vermeiden.

---

# 59. Benutzerführung

Die Oberfläche soll auch für unerfahrene Benutzer verständlich sein.

Jede Funktion erhält:

* kurze Beschreibung
* Warnhinweise
* Bestätigungsdialoge bei kritischen Aktionen

---

# 60. Kritische Operationen

Folgende Aktionen müssen ausdrücklich bestätigt werden:

* Datenträger formatieren
* Partition löschen
* Snapshot überschreiben
* Bootdateien ersetzen
* Werkseinstellungen wiederherstellen

---

# 61. Auditierbarkeit

Sicherheitsrelevante Aktionen werden protokolliert.

Beispiele:

* Wiederherstellungen
* Bootreparaturen
* Löschvorgänge
* Konfigurationsänderungen

---

# 62. Sicherheitsmodell

Recovery arbeitet mit erhöhten Rechten.

Daher gelten zusätzliche Anforderungen:

* Integritätsprüfung
* Signaturprüfung
* Plausibilitätsprüfung
* Schutz vor unbeabsichtigten Änderungen

---

# 63. Vertrauensmodell

Recovery wird als vertrauenswürdige Systemkomponente betrachtet.

Daher muss seine Integrität bereits vor dem Start überprüft werden.

---

# 64. Digitale Signaturen

Langfristig sollen Recovery-Module digital signiert werden.

Nicht signierte Module dürfen – abhängig von der Sicherheitsrichtlinie – blockiert oder nur nach ausdrücklicher Benutzerbestätigung gestartet werden.

---

# 65. Versionsverwaltung

Recovery besitzt eine eigene Versionsnummer.

Dadurch kann es unabhängig vom Kernel weiterentwickelt werden.

---

# 66. Kompatibilität

Recovery muss mit mehreren Kernelversionen zusammenarbeiten können.

Dadurch bleibt die Wiederherstellung auch nach Kernelupdates möglich.

---

# 67. Modularität

Recovery bildet eine eigenständige Komponente innerhalb der Bootarchitektur.

Es ist weder Teil des Kernels noch Teil von Stage-1.

---

# 68. Testbarkeit

Recovery kann unabhängig getestet werden.

Dies umfasst:

* Unit-Tests
* Integrationstests
* Boottests
* Wiederherstellungstests

---

# 69. Regressionstests

Vor jeder Veröffentlichung müssen mindestens folgende Szenarien geprüft werden:

* beschädigter Kernel
* fehlgeschlagenes Update
* beschädigtes BootFS
* manuell gestartetes Recovery
* automatischer Recovery-Start

---

# 70. Qualitätsanforderungen

Recovery muss:

* robust
* modular
* nachvollziehbar
* fehlertolerant
* unabhängig vom Kernel
* wartbar

sein.

---

# 71. Alternativen

## Recovery im Kernel

**Abgelehnt**

Nachteil:

Ein beschädigter Kernel verhindert den Start des Recovery-Systems.

---

## Recovery in Stage-1

**Abgelehnt**

Nachteile:

* deutlich größere Stage-1
* höhere Komplexität
* längere Startzeit
* Verstoß gegen das Minimalitätsprinzip

---

## Externes Rettungsmedium

**Teilweise geeignet**

Ein externes Rettungsmedium bleibt als zusätzliche Option sinnvoll.

Es ersetzt jedoch nicht das integrierte Recovery-System.

---

## Eigenständiges Bootloader-Modul

**Angenommen**

Es verbindet hohe Unabhängigkeit mit guter Erweiterbarkeit und klarer Architektur.

---

# 72. Implementierungsstrategie

## Phase 1

* Modulstruktur
* grafische Oberfläche
* Bootmenüintegration

## Phase 2

* Snapshot-Auswahl
* Rollback
* Bootreparatur

## Phase 3

* Dateimanager
* Diagnosewerkzeuge
* Logsystem

## Phase 4

* Backup
* Wiederherstellung
* Signaturprüfung

## Phase 5

* Erweiterungsmodule
* Netzwerkfunktionen
* Fernwartung (optional)

---

# 73. Beispielarchitektur

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Recovery Module

↓

Recovery Services

↓

BootFS
```

---

# 74. Dokumentation

Das Recovery-Modul muss dokumentieren:

* Modulformat
* API
* unterstützte Dateisysteme
* Wiederherstellungsabläufe
* Sicherheitsmechanismen

---

# 75. Zukunftssicherheit

Neue Funktionen dürfen ergänzt werden, ohne die Grundarchitektur zu verändern.

Hierzu zählen beispielsweise:

* Cloud-Backups
* Netzwerkdiagnose
* Fernwartung
* automatisierte Reparaturassistenten

---

# 76. Plattformunabhängigkeit

Recovery darf keine festen Abhängigkeiten zu:

* BIOS
* UEFI
* x86
* ARM
* RISC-V

enthalten.

Hardwareabhängigkeiten werden ausschließlich über Bootloader-Abstraktionen angesprochen.

---

# 77. Konformitätskriterien

Eine Implementierung erfüllt dieses ADR, wenn:

* Recovery ein eigenständiges Bootmodul ist,
* Stage-1 frei von Recovery-Funktionalität bleibt,
* Recovery unabhängig vom Kernel gestartet werden kann,
* Recovery dieselben Bootinformationen wie andere Module erhält,
* Recovery über definierte Bootloader-Schnittstellen arbeitet,
* Recovery unabhängig aktualisiert werden kann,
* und Fehler im Recovery-Modul den Bootloader nicht zum Absturz bringen.

---

# 78. Entscheidungsergebnis

NovaOS implementiert das Recovery-System als eigenständiges Bootloader-Modul.

Diese Architektur ermöglicht eine Wiederherstellung auch dann, wenn Kernel oder Betriebssystem nicht mehr funktionsfähig sind. Gleichzeitig bleiben Stage-1 minimal und der Kernel frei von Recovery-spezifischer Logik.

---

# 79. Zusammenfassung

Das Recovery-System ist eine zentrale Komponente der NovaOS-Bootarchitektur, wird jedoch bewusst nicht in Stage-1 oder den Kernel integriert. Stattdessen bildet es ein eigenständiges Bootmodul, das über den Boot Module Manager gestartet wird und dieselben grundlegenden Dienste wie andere Bootmodule nutzt.

Durch diese Trennung entstehen eine klare Verantwortungsaufteilung, eine hohe Ausfallsicherheit und die Möglichkeit, das Recovery-System unabhängig vom Kernel weiterzuentwickeln und zu aktualisieren. Fehlerhafte Kernel, beschädigte Bootdateien oder fehlgeschlagene Updates können so behandelt werden, ohne dass ein externes Rettungsmedium zwingend erforderlich ist.
