# RFC-0001: Nova Platform – Projektphilosophie und Systemarchitektur

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Architektur
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Boot Manager, Nova Kernel, Nova Rescue, NovaOS
- **Abhängigkeiten:** Keine
- **Ersetzt:** Keine

---

## 1. Zusammenfassung

Dieses RFC definiert die grundlegende Projektphilosophie, die Systemarchitektur und die verbindlichen Entwicklungsprinzipien der Nova Platform.

Die Nova Platform ist keine einzelne Anwendung und kein isoliertes Betriebssystem. Sie ist eine modular aufgebaute Systemplattform, auf der mehrere Produkte aufbauen:

- Nova Boot Manager
- Nova Rescue
- NovaOS
- Nova Installer
- Nova Deploy
- zukünftige Nova-Systemwerkzeuge

Alle Produkte sollen möglichst viele gemeinsame Subsysteme, Bibliotheken, Treiber, Dienste und Werkzeuge verwenden.

---

## 2. Motivation

Nova Rescue und NovaOS benötigen viele identische technische Komponenten:

- Boot-Infrastruktur
- Hardware-Abstraktion
- Speicherverwaltung
- Grafik
- Schrift- und Bilddarstellung
- Eingabeverarbeitung
- Dateisystemzugriff
- Storage-Verwaltung
- Dienste
- Benutzeroberflächen
- Asset-Verwaltung
- Themes
- Logging
- Fehlerbehandlung

Eine getrennte Implementierung dieser Funktionen würde zu folgenden Problemen führen:

- doppelter Quellcode
- unterschiedliche APIs
- höhere Fehleranfälligkeit
- unnötiger Wartungsaufwand
- inkonsistentes Verhalten
- erschwerte Weiterentwicklung

Die Nova Platform stellt deshalb ein gemeinsames technisches Fundament bereit.

---

## 3. Ziele

Die Nova Platform verfolgt folgende Hauptziele:

1. Wiederverwendbare Subsysteme
2. Klare Modulgrenzen
3. Stabile öffentliche Schnittstellen
4. Austauschbare Implementierungen
5. Kontrollierter Hardwarezugriff
6. Hohe Fehlertoleranz
7. Gute Debug- und Diagnosefähigkeit
8. Langfristige Wartbarkeit
9. Unterstützung verschiedener Nova-Produkte
10. Schrittweise Erweiterbarkeit

---

## 4. Nicht-Ziele der ersten Entwicklungsphase

Die folgenden Punkte sind ausdrücklich keine Anforderungen der ersten Version:

- vollständige POSIX-Kompatibilität
- vollständige Windows-ABI-Kompatibilität
- vollständige Multicore-Unterstützung
- vollständige GPU-Beschleunigung
- vollständige Netzwerkunterstützung
- vollständige Unterstützung aller Dateisysteme
- dynamisches Laden unsignierter Kernelmodule
- verteilte Systemdienste
- vollständige Benutzer- und Rechteverwaltung

Diese Funktionen dürfen später ergänzt werden, müssen aber die in diesem RFC definierten Architekturgrenzen respektieren.

---

## 5. Plattformprodukte

### 5.1 Nova Boot Manager

Der Nova Boot Manager initialisiert die minimale Hardwareumgebung und lädt eines der verfügbaren Nova-Produkte.

Aufgaben:

- BIOS- und später UEFI-Start
- Bootmedium erkennen
- Speicherinformationen erfassen
- Grafikmodus vorbereiten
- Boot-Konfiguration lesen
- Bootmenü darstellen
- Kernel und Module laden
- BootInfo an den Kernel übergeben

Der Boot Manager darf keine Funktionen enthalten, die zwingend in den Kernel oder in Nova Rescue gehören.

---

### 5.2 Nova Kernel

Der Nova Kernel stellt die grundlegende Laufzeitumgebung bereit.

Aufgaben:

- Speicherverwaltung
- Interruptverwaltung
- Timer
- Scheduler
- Prozess- und Threadverwaltung
- Systemaufrufe
- Treiberschnittstellen
- Kernel-Dienste
- Fehler- und Panic-Behandlung

Der Kernel enthält keine anwendungsspezifische Logik des Explorers, Backup-Systems oder der grafischen Anwendungen.

---

### 5.3 Nova Rescue

Nova Rescue ist eine unabhängige Rettungs- und Wartungsumgebung.

Geplante Aufgaben:

- Dateien kopieren
- Daten sichern
- Backups wiederherstellen
- Datenträger analysieren
- Dateisysteme prüfen
- Bootprobleme reparieren
- Hardwareinformationen anzeigen
- Datenträger sicher löschen
- gelöschte Dateien wiederherstellen
- Diagnoseberichte erzeugen

Nova Rescue muss auch dann startfähig bleiben, wenn das installierte NovaOS beschädigt ist.

---

### 5.4 NovaOS

NovaOS ist das vollständige Desktop-Betriebssystem der Nova Platform.

Es verwendet dieselben grundlegenden Subsysteme wie Nova Rescue, ergänzt diese jedoch um:

- Desktop-Umgebung
- Benutzerkonten
- Anwendungen
- Softwareverwaltung
- Benachrichtigungen
- Multimedia
- lokale KI-Dienste
- dauerhafte Systemdienste

---

## 6. Architekturübersicht

```text
+-------------------------------------------------------+
| Anwendungen                                           |
| Explorer, Backup, Restore, Settings, Terminal         |
+-------------------------------------------------------+
| Nova SDK                                              |
+-------------------------------------------------------+
| Nova UI Framework                                     |
+-------------------------------------------------------+
| Nova Graphics Engine                                  |
+-------------------------------------------------------+
| Nova Core Services                                    |
| Storage, VFS, Assets, Themes, Tasks, Logging, IPC     |
+-------------------------------------------------------+
| Nova Kernel                                           |
+-------------------------------------------------------+
| Nova Hardware Abstraction Layer                       |
+-------------------------------------------------------+
| Gerätetreiber                                         |
+-------------------------------------------------------+
| Hardware                                              |
+-------------------------------------------------------+