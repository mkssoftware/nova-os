# NPSPEC-GRAPHICS-DRIVER-0001 – Nova Graphics Driver Interface

**Status:** Angenommen  
**Bereich:** Graphics / Grafiktreiberschnittstelle  
**System:** NovaOS

## 1. Zweck

Nova Graphics Driver Interface definiert die einheitliche Schnittstelle zwischen dem Nova Graphics Core und den hardwarespezifischen Grafiktreibern.

Ziel ist eine modulare, sichere und erweiterbare Treiberarchitektur, die unterschiedliche Grafikgeräte unterstützt, ohne die übergeordneten Grafiksubsysteme an bestimmte Hersteller oder Hardwarearchitekturen zu binden.

Die Schnittstelle bildet die Grundlage für native GPU-Beschleunigung, Speicherverwaltung, Befehlsausführung und Gerätesynchronisation.

## 2. Architektur

Die Grafiktreiberarchitektur besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Graphics Device Manager | Erkennung und Verwaltung von Grafikgeräten |
| Graphics Driver Interface (GDI) | Einheitlicher Treibervertrag |
| Driver Registry | Registrierung verfügbarer Grafiktreiber |
| Driver Loader | Laden und Initialisieren geeigneter Treiber |
| Device Abstraction | Hardwareunabhängige Gerätebeschreibung |
| Graphics Memory Interface | Zugriff auf GPU-Speicherfunktionen |
| Graphics Command Interface | Übermittlung von Grafikbefehlen |
| Graphics Synchronization Interface | Synchronisation und Interrupt-Verarbeitung |
| Driver Recovery | Fehlererkennung und Gerätewiederherstellung |

Das Graphics Driver Interface ist Bestandteil der internen NovaOS-Grafikarchitektur.

Anwendungen greifen nicht unmittelbar auf Grafiktreiber zu, sondern verwenden die Nova Graphics API.

## 3. Treibermodell

NovaOS unterscheidet folgende Treiberklassen:

| Treiberklasse | Beschreibung |
|---|---|
| Firmware Framebuffer Driver | Zugriff auf BIOS/VBE- oder UEFI-GOP-Framebuffer |
| Basic Display Driver | Grundlegende Bildausgabe ohne vollständige GPU-Beschleunigung |
| Native GPU Driver | Hardwarebeschleunigter Grafiktreiber |
| Virtual GPU Driver | Treiber für virtualisierte Grafikhardware |

Ein Grafiktreiber kann mehrere Funktionen bereitstellen.

Die Fähigkeiten eines Treibers werden unabhängig voneinander registriert.

Ein Display-Treiber MUSS nicht zwangsläufig 3D-Beschleunigung unterstützen.

## 4. Driver Interface

Jeder Grafiktreiber implementiert einen versionierten Schnittstellenvertrag.

Dieser umfasst:

| Schnittstelle | Funktion |
|---|---|
| DriverInitialize | Treiber initialisieren |
| DriverShutdown | Treiber kontrolliert beenden |
| DeviceEnumerate | Unterstützte Geräte ermitteln |
| DeviceInitialize | Grafikgerät initialisieren |
| DeviceReset | Grafikgerät zurücksetzen |
| DeviceGetCapabilities | Gerätefähigkeiten abfragen |
| MemoryAllocate | Grafikspeicher reservieren |
| MemoryFree | Grafikspeicher freigeben |
| CommandSubmit | Grafikbefehle übermitteln |
| CommandSynchronize | Befehlsausführung synchronisieren |
| DeviceGetStatus | Gerätezustand abfragen |

Optionale Funktionen werden über gesonderte Capability-Schnittstellen bereitgestellt.

Nicht unterstützte Funktionen MÜSSEN eindeutig erkennbar sein.

## 5. Geräteerkennung und Initialisierung

Der Graphics Device Manager erkennt verfügbare Grafikgeräte über die zuständigen Hardware-Subsysteme.

Die Initialisierung erfolgt in folgenden Schritten:

1. Grafikgerät erkennen.
2. Hardwareidentität und Geräteinformationen ermitteln.
3. Geeigneten Grafiktreiber auswählen.
4. Treibersignatur und Berechtigungen prüfen.
5. Treiber laden.
6. Hardwarezugriffe autorisieren.
7. Gerät initialisieren.
8. Unterstützte Funktionen registrieren.
9. Gerät dem Graphics Core bereitstellen.

Ein fehlgeschlagener Treiberstart DARF die Initialisierung anderer Grafikgeräte nicht verhindern.

## 6. Gerätefähigkeiten

Grafiktreiber stellen ihre unterstützten Funktionen über strukturierte Capability-Abfragen bereit.

Dazu gehören:

- Unterstützte Grafikoperationen.
- Verfügbare Command Queues.
- Speicherarten und Speicherlimits.
- Unterstützte Buffer- und Texturformate.
- Shader- und Pipeline-Funktionen.
- Synchronisationsmechanismen.
- Unterstützte Displayfunktionen.
- Energieverwaltungsfunktionen.
- Reset- und Recovery-Fähigkeiten.

Der Graphics Core DARF ausschließlich Funktionen verwenden, die vom jeweiligen Gerät unterstützt werden.

## 7. Grafikspeicher

Das Graphics Driver Interface stellt hardwarespezifische Speicheroperationen bereit.

Unterstützt werden:

- GPU-lokaler Speicher.
- CPU-zugänglicher Grafikspeicher.
- Gemeinsam nutzbare Speicherbereiche.
- Buffer-Mapping.
- Speicherübertragungen.
- Speicherfreigaben.
- Geräteabhängige Speicheranforderungen.

Der Graphics Memory Manager verwaltet die logischen Ressourcen.

Der Grafiktreiber übernimmt die hardwarespezifische Umsetzung.

Speicherzugriffe MÜSSEN durch die geltenden Sicherheits- und Isolationsmechanismen begrenzt werden.

## 8. Grafikbefehle

Grafikbefehle werden durch den Graphics Core und das jeweilige Rendering-Backend vorbereitet.

Der Grafiktreiber übernimmt:

- Hardwarespezifische Befehlsübermittlung.
- Verwaltung von Hardware-Command-Queues.
- Start der GPU-Ausführung.
- Überwachung des Ausführungsfortschritts.
- Verarbeitung von Abschlussmeldungen.
- Fehlererkennung.

Der Treiber DARF keine unvalidierten Anwendungsbefehle unmittelbar mit privilegiertem Hardwarezugriff ausführen.

Die Validierung erfolgt entsprechend den definierten Verantwortlichkeiten von Graphics Core, Backend und Treiber.

## 9. Interrupts und Synchronisation

Grafiktreiber unterstützen die Verarbeitung gerätespezifischer Ereignisse.

Dazu gehören:

- Abschluss von Grafikoperationen.
- GPU-Interrupts.
- Speicherfehler.
- Gerätefehler.
- Reset-Ereignisse.

Die Synchronisation erfolgt über die Mechanismen des Nova Graphics Synchronization Systems.

Interrupt-Handler MÜSSEN zeitkritische Verarbeitung auf das notwendige Minimum beschränken.

Aufwendige Folgeoperationen SOLLEN außerhalb des unmittelbaren Interrupt-Kontexts ausgeführt werden.

## 10. Sicherheit und Isolation

Grafiktreiber arbeiten innerhalb eines definierten Sicherheitskontexts.

Dabei gilt:

- Treiber MÜSSEN vor dem Laden verifiziert werden.
- Hardwarezugriffe MÜSSEN ausdrücklich autorisiert sein.
- DMA-Zugriffe MÜSSEN auf zulässige Speicherbereiche beschränkt werden.
- Anwendungen DÜRFEN keine privilegierten Treiberoperationen unmittelbar ausführen.
- Treiber MÜSSEN die Ressourcenisolation des Graphics Core respektieren.
- Fehlerhafte Treiberoperationen DÜRFEN keine unkontrollierten Zugriffe auf fremde Ressourcen ermöglichen.

NovaOS SOLL Grafiktreiber soweit technisch möglich isolieren.

Nicht isolierbare Treiberfunktionen MÜSSEN besonders restriktiven Zugriffsregeln unterliegen.

## 11. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS eine einheitliche Grafiktreiberschnittstelle bereitstellen.
- GRAPHICS-002: Grafiktreiber MÜSSEN über versionierte Schnittstellen eingebunden werden.
- GRAPHICS-003: Treiber MÜSSEN ihre unterstützten Geräte und Funktionen deklarieren.
- GRAPHICS-004: Der Graphics Core DARF keine direkte Abhängigkeit von herstellerspezifischen Treibern besitzen.
- GRAPHICS-005: Grafiktreiber MÜSSEN kontrolliert initialisiert und beendet werden können.
- GRAPHICS-006: Treiber MÜSSEN vor ihrer Aktivierung autorisiert werden.
- GRAPHICS-007: Hardwarezugriffe MÜSSEN auf die erforderlichen Ressourcen beschränkt sein.
- GRAPHICS-008: Grafikspeicheroperationen MÜSSEN über definierte Schnittstellen erfolgen.
- GRAPHICS-009: Grafikbefehle MÜSSEN vor der Hardwareausführung ausreichend validiert werden.
- GRAPHICS-010: Asynchrone Befehlsausführung MUSS unterstützt werden können.
- GRAPHICS-011: Gerätefehler MÜSSEN erkannt und an den Graphics Core gemeldet werden.
- GRAPHICS-012: Ein GPU-Reset MUSS über eine definierte Treiberschnittstelle möglich sein, sofern die Hardware dies unterstützt.
- GRAPHICS-013: Mehrere Grafikgeräte und Treiber MÜSSEN gleichzeitig verwaltet werden können.
- GRAPHICS-014: Treiberfehler DÜRFEN andere Grafikgeräte nicht unkontrolliert beeinträchtigen.
- GRAPHICS-015: Nicht unterstützte Funktionen MÜSSEN eindeutig zurückgemeldet werden.
- GRAPHICS-016: Die Treiberschnittstelle MUSS erweiterbar bleiben.
- GRAPHICS-017: Grundlegende Framebuffer-Ausgaben MÜSSEN ohne vollständigen GPU-Treiber möglich sein.
- GRAPHICS-018: Die Grafiktreiberverwaltung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 12. Treiberwechsel und Hot Replacement

NovaOS unterstützt den kontrollierten Austausch von Grafiktreibern.

Mögliche Auslöser:

- Aktivierung eines nativen GPU-Treibers nach dem Systemstart.
- Treiberaktualisierung.
- Gerätewechsel.
- Treiberfehler.
- Wiederherstellung nach GPU-Reset.

Der Wechsel erfolgt in folgenden Schritten:

1. Betroffenes Gerät und abhängige Ressourcen bestimmen.
2. Neue Befehlsübermittlungen kontrolliert anhalten.
3. Laufende Operationen abschließen oder abbrechen.
4. Grafikressourcen sichern oder invalidieren.
5. Alten Treiber deaktivieren.
6. Neuen Treiber initialisieren.
7. Gerätefähigkeiten erneut ermitteln.
8. Ressourcen wiederherstellen.
9. Grafikbetrieb fortsetzen.

Ein Treiberwechsel MUSS mit dem Graphics Backend Manager koordiniert werden.

Hot Replacement DARF nur erfolgen, wenn die erforderlichen Sicherheits- und Konsistenzbedingungen erfüllt sind.

## 13. Fehlerbehandlung und Wiederherstellung

- Fehlgeschlagene Treiberinitialisierungen werden protokolliert.
- Nicht unterstützte Geräte bleiben ohne aktiven nativen Grafiktreiber.
- Gerätefehler werden an den Graphics Device Manager gemeldet.
- Hängende GPU-Operationen MÜSSEN über definierte Timeout-Mechanismen erkannt werden können.
- Bei unterstützter Hardware SOLL ein kontrollierter GPU-Reset versucht werden.
- Verlorene Ressourcen werden dem Graphics Resource Manager gemeldet.
- Fehlgeschlagene Treiberwechsel führen zu einem gültigen Fallback-Zustand.
- Kritische Systemausgaben SOLLEN über einen unabhängigen Framebuffer- oder Software-Rendering-Pfad verfügbar bleiben.
- Wiederholte Treiberfehler DÜRFEN zur kontrollierten Deaktivierung des betroffenen Treibers führen.

## 14. Ergebnis

NovaOS erhält eine modulare und sichere Grafiktreiberschnittstelle zur Unterstützung unterschiedlicher Grafikgeräte.

Die Architektur trennt hardwarespezifische Funktionen vom Graphics Core und ermöglicht den Einsatz nativer GPU-Treiber, virtueller Grafikgeräte und einfacher Framebuffer-Treiber.

Durch versionierte Schnittstellen, Capability-Abfragen und kontrollierte Wiederherstellungsmechanismen können Grafiktreiber unabhängig von übergeordneten Grafikkomponenten entwickelt und erweitert werden.

**Grundsatz:** Grafiktreiber stellen Hardwarefunktionen bereit, während NovaOS die Kontrolle über Ressourcen, Berechtigungen und den sicheren Grafikbetrieb behält.