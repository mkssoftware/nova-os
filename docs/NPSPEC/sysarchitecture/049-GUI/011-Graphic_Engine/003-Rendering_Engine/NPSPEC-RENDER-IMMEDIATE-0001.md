# NPSPEC-RENDER-IMMEDIATE-0001 – Nova Immediate Rendering

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Immediate Rendering definiert die direkte Erzeugung und Ausführung grafischer Rendering-Aufträge innerhalb von NovaOS.

Im Gegensatz zu Retained Rendering müssen Anwendungen ihre grafischen Inhalte nicht dauerhaft im Nova Scene Graph verwalten.

Ziel ist die Unterstützung dynamischer Grafikverarbeitung für Spiele, CAD-Anwendungen, Simulationen, eigene Rendering-Engines und Kompatibilitätsschichten.

## 2. Architekturprinzipien

- **Command-Driven:** Anwendungen erzeugen explizite Rendering-Aufträge.
- **Stateless Submission:** Die Render Engine benötigt keine persistente Szenenbeschreibung.
- **Explicit:** Anwendungen kontrollieren ihre grafischen Inhalte.
- **Asynchronous:** Rendering-Aufträge können asynchron ausgeführt werden.
- **Hardware-Accelerated:** Verfügbare GPU-Funktionen werden unterstützt.
- **Resource-Aware:** Grafikressourcen werden kontrolliert verwaltet.
- **Composable:** Ergebnisse können mit Retained-Rendering-Inhalten kombiniert werden.
- **Backend-Independent:** Die Schnittstellen bleiben unabhängig von konkreten Grafiktreibern.
- **Fallback-Safe:** Unterstützte Operationen können softwarebasiert ausgeführt werden.

## 3. Rendering-Modell

Immediate Rendering basiert auf explizit bereitgestellten Rendering-Aufträgen.

Ein Rendering-Auftrag enthält insbesondere:

- Zieloberfläche oder Render Target
- Rendering-Befehle
- Verwendete Grafikressourcen
- Pipeline- und Shader-Zustände
- Ressourcenabhängigkeiten
- Synchronisationsinformationen
- Optionale Damage Regions

Die Anwendung ist für die Erzeugung ihrer grafischen Inhalte verantwortlich.

NovaOS übernimmt die kontrollierte Ausführung, Ressourcenkoordination und Integration in die grafische Umgebung.

## 4. Rendering-Schnittstellen

NovaOS MUSS definierte Schnittstellen zur direkten Grafikverarbeitung bereitstellen.

Diese ermöglichen insbesondere:

- Erstellen und Verwalten von Render Targets
- Bereitstellen von Geometrien
- Verwenden von Texturen
- Konfigurieren von Pipeline States
- Binden von Shadern und Ressourcen
- Erzeugen von Draw Commands
- Erzeugen von Compute Commands
- Übermitteln von Rendering-Aufträgen
- Synchronisieren abgeschlossener Operationen

Die konkrete Implementierung DARF unterschiedliche Hardware-Backends verwenden.

## 5. Rendering-Ablauf

Die Verarbeitung erfolgt grundsätzlich durch:

1. Auswahl eines gültigen Rendering-Ziels.
2. Vorbereitung erforderlicher Ressourcen.
3. Konfiguration der Rendering-Zustände.
4. Erzeugung der Rendering-Befehle.
5. Definition erforderlicher Abhängigkeiten.
6. Übermittlung der Rendering-Aufträge.
7. Ausführung durch das Rendering-Backend.
8. Synchronisation des Ergebnisbuffers.
9. Übergabe an den Nova Compositor.

Die Anwendung DARF mehrere Rendering-Aufträge zusammenfassen.

## 6. Command Buffers

Immediate Rendering SOLL die Verwendung von Command Buffers unterstützen.

Diese ermöglichen:

- Zusammenfassung mehrerer Rendering-Befehle
- Reduzierung von CPU-GPU-Kommunikationsaufwand
- Parallele Befehlsvorbereitung
- Asynchrone Ausführung
- Wiederverwendung geeigneter Befehlsfolgen

Command Buffers MÜSSEN vor ihrer Ausführung validiert werden.

Die Wiederverwendung DARF nur erfolgen, wenn Ressourcenreferenzen und Ausführungsbedingungen weiterhin gültig sind.

## 7. Render Graph Integration

Immediate Rendering DARF über den Nova Render Graph verarbeitet werden.

Der Render Graph kann dabei:

- Rendering-Aufträge organisieren
- Ressourcenabhängigkeiten auswerten
- Ausführungsreihenfolgen bestimmen
- Synchronisationsoperationen einfügen
- Unabhängige Aufträge parallelisieren
- Temporäre Ressourcen verwalten

Anwendungen mit eigenen Render Graphs DÜRFEN ihre Ergebnisse über definierte Surface-Schnittstellen bereitstellen.

Die Verwendung des Nova Render Graph ist für externe Rendering-Engines nicht verpflichtend.

## 8. Ressourcenverwaltung

Immediate Rendering verwendet die zentrale Grafikressourcenverwaltung.

Dabei MUSS NovaOS:

- Ressourcen eindeutig identifizieren.
- Zugriffsberechtigungen überprüfen.
- Speicherbereiche kontrolliert verwalten.
- Ressourcenlebenszeiten berücksichtigen.
- Gleichzeitige Zugriffe synchronisieren.
- Ungültige Ressourcenreferenzen erkennen.

Anwendungen DÜRFEN keine unberechtigten direkten Zugriffe auf fremde GPU-Ressourcen erhalten.

## 9. Synchronisation

Rendering-Aufträge können unabhängig von CPU und anderen GPU-Aufträgen ausgeführt werden.

NovaOS MUSS geeignete Synchronisationsmechanismen unterstützen.

Dazu gehören insbesondere:

- GPU Fences
- Semaphores
- Timeline Synchronization
- Resource Barriers
- Surface Synchronization

Ein Ergebnisbuffer DARF erst verwendet werden, wenn sämtliche erforderlichen Schreiboperationen abgeschlossen sind.

## 10. Damage Tracking

Immediate Rendering MUSS keine vollständigen Szenenänderungen an NovaOS melden.

Anwendungen SOLLEN jedoch Damage Regions bereitstellen können.

Diese ermöglichen:

- Gezielte Oberflächenaktualisierungen
- Inkrementelle Komposition
- Reduzierung unnötiger Speicherzugriffe
- Effizientere Frame-Verarbeitung

Fehlen gültige Damage-Informationen, MUSS NovaOS den betroffenen Buffer konservativ als vollständig verändert behandeln können.

## 11. Frame Management

Immediate Rendering kann durch Anwendungen selbstständig ausgelöst werden.

Dies ermöglicht insbesondere:

- Kontinuierliches Echtzeit-Rendering
- Ereignisgesteuerte Aktualisierungen
- Variable Bildraten
- Synchronisation mit der Bildschirmausgabe

NovaOS MUSS Anwendungen Präsentations- und Synchronisationsinformationen bereitstellen können.

Die Frame-Erzeugung DARF durch Systemrichtlinien begrenzt werden, beispielsweise bei Hintergrundanwendungen oder Ressourcenengpässen.

## 12. Integration mit Retained Rendering

Immediate und Retained Rendering MÜSSEN innerhalb derselben grafischen Umgebung kombinierbar sein.

Dabei gilt:

- Retained Rendering verwaltet persistente grafische Szenen.
- Immediate Rendering erzeugt explizite Rendering-Aufträge.
- Beide Verfahren können eigenständige Oberflächen erzeugen.
- Der Nova Compositor führt die Ergebnisse zusammen.

Immediate-Rendering-Inhalte DÜRFEN als Surface Nodes in den Nova Scene Graph eingebunden werden.

Der Scene Graph verwaltet dabei die Einbindung der Oberfläche, nicht zwingend deren interne grafische Inhalte.

## 13. Zero-Copy Integration

NovaOS SOLL Rendering-Ergebnisse ohne unnötige Buffer-Kopien an den Compositor übergeben.

Voraussetzungen sind:

- Kompatible Buffer-Formate
- Geeignete Speicherbereiche
- Gültige Ressourcenreferenzen
- Korrekte Synchronisation
- Unterstützende Hardware

Ist Zero-Copy nicht möglich, MUSS ein alternativer Übergabepfad verfügbar sein.

## 14. Software Rendering

Immediate Rendering MUSS über einen definierten Software-Fallback verfügen.

Dieser kann verwendet werden bei:

- Fehlender GPU-Beschleunigung
- Nicht unterstützten Hardwarefunktionen
- Recovery-Umgebungen
- Diagnose
- Kompatibilitätsanforderungen

Der Software-Renderer MUSS nur die ausdrücklich unterstützten Rendering-Funktionen bereitstellen.

Nicht unterstützte Funktionen MÜSSEN kontrolliert gemeldet werden.

## 15. Sicherheit und Isolation

Immediate Rendering MUSS die Sicherheitsarchitektur von NovaOS berücksichtigen.

Dabei gilt:

- Anwendungen dürfen nur autorisierte Grafikressourcen verwenden.
- Rendering-Aufträge müssen validiert werden.
- GPU-Speicherzugriffe müssen isoliert werden.
- Ressourcenfreigaben dürfen laufende Aufträge nicht beschädigen.
- Fehlerhafte Anwendungen dürfen keine fremden Oberflächen manipulieren.

Die direkte Erzeugung von Rendering-Befehlen bedeutet keinen uneingeschränkten Hardwarezugriff.

## 16. Fehlerbehandlung

Fehlgeschlagene Rendering-Aufträge DÜRFEN keine ungültigen Ergebnisbuffer veröffentlichen.

NovaOS MUSS:

- Ungültige Befehle erkennen.
- Ressourcenfehler kontrolliert behandeln.
- Fehlgeschlagene Aufträge abbrechen können.
- Synchronisationsfehler erkennen.
- Geeignete Wiederherstellungsmechanismen bereitstellen.

Fehler einzelner Anwendungen SOLLEN die übrige grafische Umgebung nicht beeinträchtigen.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS Immediate Rendering unterstützen.
2. Anwendungen MÜSSEN Rendering-Aufträge ohne persistenten Scene Graph erzeugen können.
3. Rendering-Aufträge MÜSSEN über definierte Schnittstellen übermittelt werden.
4. Command Buffers SOLLEN unterstützt werden.
5. Grafikressourcen MÜSSEN kontrolliert verwaltet werden.
6. Rendering-Aufträge MÜSSEN validierbar sein.
7. CPU- und GPU-Verarbeitung MÜSSEN synchronisiert werden können.
8. Immediate Rendering MUSS mit dem Nova Compositor integrierbar sein.
9. Immediate und Retained Rendering MÜSSEN kombinierbar sein.
10. Anwendungen SOLLEN Damage Regions bereitstellen können.
11. Fehlende Damage-Informationen MÜSSEN konservativ behandelt werden.
12. Zero-Copy-Übergaben SOLLEN unterstützt werden.
13. Ein definierter Software-Fallback MUSS verfügbar sein.
14. Anwendungen DÜRFEN keinen unberechtigten GPU- oder Speicherzugriff erhalten.
15. Die Schnittstellen MÜSSEN unterschiedliche Rendering-Backends unterstützen.
16. Fehlerhafte Rendering-Aufträge DÜRFEN die Systemstabilität nicht gefährden.

## 18. Ergebnis

Nova Immediate Rendering ermöglicht die direkte und flexible Erzeugung grafischer Inhalte ohne verpflichtende persistente Szenenverwaltung.

Durch explizite Rendering-Aufträge, Command Buffers, kontrollierte Ressourcenverwaltung und asynchrone GPU-Verarbeitung unterstützt NovaOS leistungsfähige Echtzeitgrafik und spezialisierte Rendering-Engines.

Die Integration mit Retained Rendering und dem Nova Compositor gewährleistet, dass beide Rendering-Modelle innerhalb derselben grafischen Architektur zusammenarbeiten können.