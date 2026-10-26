# NPSPEC-COMPOSITOR-BUFFER-0001 – Nova Graphics Buffer Management

**Status:** Angenommen  
**Bereich:** Compositor / Grafikbuffer-Verwaltung  
**System:** NovaOS

## 1. Zweck

Nova Graphics Buffer Management definiert die Erstellung, Verwaltung, Nutzung und Freigabe grafischer Speicherbuffer.

Ziel ist eine sichere, performante und ressourcenschonende Verarbeitung von Bilddaten zwischen Anwendungen, Compositor und Grafikhardware.

## 2. Architektur

- Zentraler Graphics Buffer Manager für den Nova Compositor.
- Trennung zwischen Surface-Objekten und Grafikbuffern.
- Hardwareunabhängige Buffer-Schnittstelle.
- Unterstützung von CPU- und GPU-Speicher.
- Integration in Nova Surface Management und Synchronization.
- Capability-basierte Zugriffssteuerung.
- Unterstützung von Zero-Copy und Shared Buffers.
- Austauschbare hardwarespezifische Speicher-Backends.
- Integration in das NovaOS-Ressourcenmanagement.

## 3. Kernfunktionen

- Erstellung und Freigabe grafischer Buffer.
- Verwaltung von Größe, Pixelformat und Speicherlayout.
- Unterstützung unterschiedlicher Farbtiefen.
- Verwaltung von CPU- und GPU-Zugriffen.
- Import und Export freigegebener Buffer.
- Unterstützung von Buffer-Pools.
- Verwaltung von Buffer-Referenzen.
- Synchronisation paralleler Zugriffe.
- Unterstützung von Direct Scanout.
- Überwachung des Grafikspeicherverbrauchs.
- Wiederherstellung nach Ressourcenverlust.

## 4. Normative Anforderungen

- BUFFER-001: Jeder Grafikbuffer MUSS eine eindeutige, gegen veraltete Referenzen geschützte Identität besitzen.
- BUFFER-002: Grafikbuffer MÜSSEN unabhängig von Surfaces verwaltet werden.
- BUFFER-003: Buffer-Erstellung und -Zugriffe MÜSSEN durch Capabilities kontrolliert werden.
- BUFFER-004: Größe, Pixelformat, Speicherlayout und Nutzungsrechte MÜSSEN eindeutig definiert sein.
- BUFFER-005: Speicherzugriffe MÜSSEN auf gültige Buffer-Grenzen beschränkt werden.
- BUFFER-006: CPU- und GPU-basierte Buffer MÜSSEN unterstützt werden.
- BUFFER-007: Gemeinsame Buffer-Nutzung MUSS mit Nova Surface Synchronization kompatibel sein.
- BUFFER-008: Buffer DÜRFEN während aktiver Nutzung nicht freigegeben oder inkompatibel verändert werden.
- BUFFER-009: Zero-Copy-Verarbeitung SOLL bevorzugt werden, sofern technisch möglich und sicher.
- BUFFER-010: Buffer-Importe MÜSSEN hinsichtlich Format, Größe, Layout und Berechtigungen validiert werden.
- BUFFER-011: Nicht initialisierte Speicherinhalte DÜRFEN nicht an unberechtigte Clients gelangen.
- BUFFER-012: Buffer-Ressourcen MÜSSEN pro Client begrenzbar sein.
- BUFFER-013: Speicherknappheit MUSS kontrolliert behandelt werden.
- BUFFER-014: Wiederverwendbare Buffer SOLLEN über geeignete Pools verwaltet werden.
- BUFFER-015: Direct Scanout MUSS bei geeigneten Buffern ermöglicht werden können.
- BUFFER-016: Verlorene GPU-Ressourcen MÜSSEN erkannt und invalidiert werden.
- BUFFER-017: Die Ressourcenfreigabe MUSS nach Abschluss aller relevanten Zugriffe erfolgen.
- BUFFER-018: Fehlerhafte Clients DÜRFEN keine fremden Buffer beschädigen.

## 5. Buffer-Modell

Jeder Grafikbuffer besitzt folgende grundlegende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Buffer ID | Eindeutige Identität |
| Owner | Eigentümer der Ressource |
| Dimensions | Breite und Höhe |
| Pixel Format | Format der Bilddaten |
| Stride | Speicherabstand zwischen Bildzeilen |
| Layout | Lineares oder hardwarespezifisches Speicherlayout |
| Memory Type | CPU-, GPU- oder gemeinsam nutzbarer Speicher |
| Usage | Rendering, Sampling, Komposition oder Scanout |
| Access | Zugriffsberechtigungen |
| Synchronization | Zugeordnete Synchronisationsobjekte |
| State | Aktueller Buffer-Zustand |

## 6. Speicherverwaltung

### System Memory
CPU-zugänglicher Speicher für Software-Rendering und kompatible Grafikoperationen.

### GPU Memory
Grafikspeicher für hardwarebeschleunigtes Rendering und Komposition.

### Shared Memory
Kontrolliert freigegebener Speicher für prozessübergreifende Buffer-Nutzung.

### Buffer Pool
Wiederverwendung geeigneter Buffer zur Reduzierung von Allokationsaufwand.

### Buffer Import/Export
Sicherer Austausch von Grafikressourcen über definierte Handles.

Die konkrete Speicherplatzierung DARF durch das jeweilige Backend optimiert werden.

## 7. Buffer-Lebenszyklus

1. **Allocate:** Speicher entsprechend den Anforderungen reservieren.
2. **Initialize:** Buffer-Metadaten und Speicherzustand initialisieren.
3. **Acquire:** Berechtigten Zugriff auf den Buffer erhalten.
4. **Use:** Rendering-, Kompositions- oder Scanout-Operationen durchführen.
5. **Synchronize:** Laufende Zugriffe koordinieren.
6. **Release:** Nutzung durch einen Verbraucher abschließen.
7. **Recycle:** Geeigneten Buffer einem Pool zurückgeben.
8. **Destroy:** Nicht mehr benötigte Ressourcen endgültig freigeben.

Ein Buffer DARF mehreren Surfaces oder Verbrauchern zugeordnet sein, sofern Zugriffsrechte und Synchronisation dies erlauben.

## 8. Performance und Zuverlässigkeit

- Minimierung unnötiger Speicherallokationen.
- Wiederverwendung vorhandener Grafikbuffer.
- Zero-Copy-Verarbeitung bei kompatiblen Speicherlayouts.
- Vermeidung unnötiger CPU-GPU-Kopien.
- Unterstützung hardwarespezifischer Speicheroptimierungen.
- Begrenzung des Grafikspeicherverbrauchs.
- Kontrollierte Behandlung von Speicherfragmentierung.
- Asynchrone Ressourcenfreigabe.
- Schutz vor Speicherlecks und Use-after-Free.
- Automatische Bereinigung verwaister Buffer.
- Software-Fallback bei fehlender GPU-Unterstützung.
- Integration in Nova Compositor Fault Recovery.

## 9. Ergebnis

NovaOS erhält eine zentrale, sichere und hardwareunabhängige Verwaltung grafischer Speicherbuffer.

Grafikressourcen können effizient erstellt, gemeinsam genutzt, synchronisiert und wiederverwendet werden.

Die Architektur bildet die Grundlage für Zero-Copy-Rendering, hardwarebeschleunigte Bildkomposition und eine ressourcenschonende grafische Desktopumgebung.