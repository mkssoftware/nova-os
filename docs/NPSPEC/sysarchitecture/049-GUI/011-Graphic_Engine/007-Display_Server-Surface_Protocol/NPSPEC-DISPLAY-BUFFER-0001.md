# NPSPEC-DISPLAY-BUFFER-0001 – Nova Display Buffer Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Display Buffer Management definiert die Verwaltung grafischer Speicherbuffer für Rendering, Surface Sharing, Komposition und Bildschirmausgabe.

Ziel ist eine sichere, effiziente und hardwareunabhängige Buffer-Verwaltung mit minimalen Speichertransfers und kontrollierter Ressourcenlebensdauer.

## 2. Architekturprinzipien

- **Zero-Copy:** Buffer werden nach Möglichkeit ohne Kopieren gemeinsam genutzt.
- **Capability-Based:** Speicherzugriffe erfolgen ausschließlich über autorisierte Handles.
- **Reference-Counted:** Ressourcen bleiben während aktiver Verwendung gültig.
- **Synchronization-Aware:** CPU-, GPU- und Display-Zugriffe werden koordiniert.
- **Format-Aware:** Pixel- und Speicherformate werden explizit beschrieben.
- **Damage-Aware:** Gültige und veränderte Buffer-Bereiche werden berücksichtigt.
- **Backend-Independent:** Die Verwaltung bleibt unabhängig von einer bestimmten GPU-API.

## 3. Display Buffer Model

Ein Display Buffer enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Buffer ID | Eindeutige Identifikation |
| Resource Handle | Autorisierte Speicherreferenz |
| Width / Height | Buffer-Abmessungen |
| Pixel Format | Pixelformat |
| Stride | Speicherabstand zwischen Bildzeilen |
| Memory Layout | Speicheranordnung und Modifier |
| Usage Flags | Zulässige Verwendungsarten |
| Access Rights | Lese- und Schreibberechtigungen |
| Synchronization | Zugehörige Synchronisationsobjekte |
| Content Version | Version des Buffer-Inhalts |

Mehrere Speicherebenen, unterschiedliche Strides und Offsets MÜSSEN bei entsprechenden Formaten unterstützt werden können.

## 4. Buffer Lifecycle

Ein Buffer durchläuft folgende logische Phasen:

`Created → Available → In Use → Released → Destroyed`

Dabei gilt:

1. Buffer werden über definierte Speicherverwaltungsdienste erstellt oder importiert.
2. Die Verwendung erfolgt über gültige Referenzen.
3. Mehrere Komponenten dürfen denselben Buffer kontrolliert nutzen.
4. Zugriffe werden synchronisiert.
5. Die physische Freigabe erfolgt erst nach Abschluss sämtlicher Verwendungen.

Die Freigabe eines Client-Handles DARF nicht zur vorzeitigen Freigabe eines noch verwendeten Buffers führen.

## 5. Buffer Allocation und Import

NovaOS MUSS folgende Verfahren unterstützen können:

- **Allocation:** Erzeugung neuer grafischer Buffer.
- **Import:** Übernahme vorhandener Buffer-Handles.
- **Export:** Autorisierte Weitergabe von Buffer-Referenzen.
- **Mapping:** Kontrollierter CPU-Zugriff.
- **Unmapping:** Beendigung eines CPU-Mappings.
- **Release:** Freigabe einer Ressourcenreferenz.

Die tatsächliche Speicherallokation erfolgt über Nova Memory Management beziehungsweise den zuständigen Grafiktreiber.

Der Display Server verwaltet die Nutzung und Validierung der Buffer, nicht zwingend deren physische Speicherzuweisung.

## 6. Buffer Sharing und Zero-Copy

NovaOS SOLL Buffer zwischen Anwendungen, Render Engine, Compositor und Display Server ohne zusätzliche Kopien übertragen können.

Voraussetzungen sind:

- Kompatible Speicherformate
- Unterstützte Speicherlayouts
- Gültige Zugriffsrechte
- Geeignete Synchronisationsmechanismen
- Kompatible Hardwarezugriffe

Ist Zero-Copy nicht möglich, MUSS ein kontrollierter Kopier- oder Konvertierungspfad verfügbar sein.

Zero-Copy DARF Sicherheits- und Synchronisationsanforderungen nicht umgehen.

## 7. Buffer Synchronization

Jeder Buffer MUSS über definierte Zugriffs- und Synchronisationsregeln verfügen.

Unterstützt werden SOLLEN:

- Acquire Fences
- Release Fences
- CPU-GPU-Synchronisation
- GPU-Display-Synchronisation
- Read-/Write-Hazards
- Presentation Completion

Ein Buffer DARF nicht unzulässig verändert werden, während GPU oder Display-Hardware auf dessen Inhalt zugreifen.

Die Freigabe zur Wiederverwendung MUSS von der Freigabe der zugrunde liegenden Speicherressource unterscheidbar sein.

## 8. Buffer Validity und Damage

NovaOS MUSS die Gültigkeit von Buffer-Inhalten berücksichtigen.

Dabei gilt:

- Damage beschreibt veränderte Inhaltsbereiche.
- Validity beschreibt nachweislich gültige Inhaltsbereiche.
- Buffer Age kann zur Rekonstruktion erforderlicher Aktualisierungen verwendet werden.
- Unbekannte Inhaltsbereiche MÜSSEN vor ihrer Darstellung korrekt initialisiert werden.
- Ein neuer Buffer darf nicht automatisch als vollständig gültig betrachtet werden.

Partial Rendering DARF nur erfolgen, wenn alle außerhalb der aktualisierten Bereiche benötigten Pixel gültig sind.

## 9. Multi-Buffering

Nova Display Buffer Management MUSS mehrere Buffer pro Surface unterstützen können.

Mögliche Verfahren sind:

- Single Buffering
- Double Buffering
- Triple Buffering

Die Auswahl erfolgt anhand von Hardwarefähigkeiten, Latenzanforderungen und Ressourcenverfügbarkeit.

Der Display Server DARF keine feste Anzahl von Buffern für sämtliche Anwendungen erzwingen.

## 10. Ressourcenverwaltung

NovaOS MUSS den Speicherverbrauch grafischer Buffer kontrollieren können.

Dazu gehören:

- Ressourcenlimits pro Client
- Speicherbudgets
- Referenzverwaltung
- Erkennung ungenutzter Buffer
- Kontrollierte Ressourcenfreigabe

Aktiv verwendete Scanout- oder Rendering-Buffer DÜRFEN nicht durch Speicherbereinigung ungültig werden.

Bei Ressourcenmangel MUSS eine definierte Fehlerbehandlung erfolgen.

## 11. Sicherheit und Isolation

Buffer-Zugriffe MÜSSEN über Nova Capabilities abgesichert werden.

Insbesondere gilt:

- Physische Speicheradressen werden nicht unkontrolliert offengelegt.
- Importierte Buffer werden vollständig validiert.
- Speicherbereiche dürfen keine fremden Daten offenlegen.
- Schreibrechte werden nur bei entsprechender Autorisierung vergeben.
- Ungültige Handles werden abgewiesen.
- Speichergrenzen und Layoutangaben werden geprüft.

Bei der Wiederverwendung von Speicher MÜSSEN mögliche Informationslecks verhindert werden.

## 12. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler behandeln können:

- Fehlgeschlagene Speicherallokation
- Ungültige Buffer-Handles
- Nicht unterstützte Pixelformate
- Inkompatible Speicherlayouts
- Synchronisationsfehler
- Ressourcenerschöpfung
- GPU- oder Display-Geräteverlust

Bei inkompatiblen Buffern SOLL eine sichere Konvertierung oder Kopie ermöglicht werden.

Ein fehlgeschlagener Buffer-Import DARF den zuletzt gültigen Surface-Zustand nicht beschädigen.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS grafische Buffer unabhängig von Surface-Objekten verwalten.
2. Buffer MÜSSEN über eindeutige Handles referenzierbar sein.
3. Pixel- und Speicherformate MÜSSEN explizit beschrieben werden.
4. Buffer-Import und -Export MÜSSEN kontrolliert erfolgen.
5. Zero-Copy SOLL bevorzugt werden.
6. Buffer-Zugriffe MÜSSEN synchronisiert sein.
7. Aktive Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
8. Buffer Validity und Damage MÜSSEN getrennt behandelt werden.
9. Multi-Buffering MUSS unterstützt werden.
10. Speicherverbrauch MUSS begrenzbar sein.
11. Buffer-Zugriffe MÜSSEN capability-basiert geschützt sein.
12. Nicht unterstützte Formate MÜSSEN sicher behandelt werden.
13. Ein Kopier- oder Konvertierungsfallback MUSS möglich sein.
14. Der Display Server MUSS unabhängig von einer bestimmten GPU-API bleiben.
15. Im deterministischen Modus MÜSSEN Buffer-Zustandsübergänge reproduzierbar sein.

## 14. Ergebnis

Nova Display Buffer Management stellt eine sichere und effiziente Verwaltung grafischer Speicherressourcen bereit.

Durch Zero-Copy Sharing, explizite Synchronisation, Buffer Validity und kontrollierte Ressourcenlebensdauer werden unnötige Kopien und Speicherzugriffe reduziert.

Damit bildet das Subsystem die Grundlage für Shared Surfaces, Incremental Rendering, Direct Scanout und eine leistungsfähige Frame-Präsentation innerhalb von NovaOS.