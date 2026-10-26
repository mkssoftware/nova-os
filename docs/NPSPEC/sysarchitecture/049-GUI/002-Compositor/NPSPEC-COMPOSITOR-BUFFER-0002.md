# NPSPEC-COMPOSITOR-BUFFER-0002 – Nova Double and Triple Buffering

**Status:** Angenommen  
**Bereich:** Compositor / Buffering und Frame-Verarbeitung  
**System:** NovaOS

## 1. Zweck

Nova Double and Triple Buffering definiert die Verwendung mehrerer Grafikbuffer zur parallelen Verarbeitung und Darstellung von Frames.

Ziel ist eine flüssige, latenzarme und ressourcenschonende Bildausgabe ohne Tearing, unnötige Wartezeiten oder übermäßigen Speicherverbrauch.

## 2. Architektur

- Integration in Nova Graphics Buffer Management.
- Unterstützung von Single-, Double- und Triple-Buffering.
- Dynamische Auswahl geeigneter Buffering-Strategien.
- Trennung zwischen Rendering-, Ready- und Presentation-Buffern.
- Synchronisation über Nova Surface Synchronization.
- Integration in Nova Compositor Scheduling.
- Unabhängige Buffer-Verwaltung pro Surface und Display.
- Unterstützung hardwarebeschleunigter und softwarebasierter Ausgabe.

## 3. Buffering-Strategien

### Single Buffering
Direkte Verarbeitung eines einzelnen Buffers.

Nur zulässig, wenn konkurrierende Zugriffe und sichtbares Tearing zuverlässig verhindert werden.

### Double Buffering
Verwendung von zwei Buffern für Rendering und Präsentation.

Bevorzugter Standard für geringe Latenz und moderaten Speicherverbrauch.

### Triple Buffering
Verwendung von drei Buffern zur Entkopplung von Rendering und Präsentation.

Geeignet bei schwankenden Rendering-Zeiten oder zur besseren Auslastung der Grafikhardware.

### Adaptive Buffering
Dynamische Anpassung der Buffer-Anzahl anhand von Latenz, Frame-Zeiten, Speicherverbrauch und Hardwarefähigkeiten.

## 4. Normative Anforderungen

- BUFFER-001: Double Buffering MUSS unterstützt werden.
- BUFFER-002: Triple Buffering MUSS optional verfügbar sein.
- BUFFER-003: Die Buffer-Anzahl MUSS dynamisch anpassbar sein.
- BUFFER-004: Double Buffering SOLL die bevorzugte Standardstrategie sein.
- BUFFER-005: Triple Buffering SOLL nur eingesetzt werden, wenn ein messbarer Vorteil besteht.
- BUFFER-006: Rendering und Präsentation DÜRFEN keine inkompatiblen gleichzeitigen Buffer-Zugriffe verursachen.
- BUFFER-007: Buffer-Wechsel MÜSSEN mit Nova Surface Synchronization abgestimmt werden.
- BUFFER-008: Die Präsentation MUSS vollständig gerenderte Frames verwenden.
- BUFFER-009: Veraltete, noch nicht präsentierte Frames SOLLEN verworfen werden können.
- BUFFER-010: Die Anzahl ausstehender Frames MUSS begrenzbar sein.
- BUFFER-011: Buffering DÜRFEN keine unbegrenzte Eingabelatenz verursachen.
- BUFFER-012: Unterschiedliche Displays MÜSSEN unabhängige Buffering-Strategien verwenden können.
- BUFFER-013: Buffer-Wiederverwendung MUSS laufende GPU- und Scanout-Zugriffe berücksichtigen.
- BUFFER-014: Bei Speicherknappheit MUSS die Buffer-Anzahl kontrolliert reduziert werden können.
- BUFFER-015: Direct Scanout MUSS mit den unterstützten Buffering-Strategien kompatibel sein.
- BUFFER-016: Änderungen der Buffering-Strategie SOLLEN ohne sichtbare Unterbrechung erfolgen.
- BUFFER-017: Frame-Drops und Präsentationslatenzen MÜSSEN messbar sein.
- BUFFER-018: Buffering-Strategien MÜSSEN ohne KI deterministisch gesteuert werden können.

## 5. Buffer-Zustände

- **Available:** Buffer steht zur Verwendung bereit.
- **Rendering:** Bildinhalt wird erzeugt.
- **Ready:** Rendering abgeschlossen; Buffer wartet auf Präsentation.
- **Presenting:** Buffer wird zur Bildausgabe verwendet.
- **Releasing:** Ausstehende Zugriffe werden abgeschlossen.

Ein Buffer DARF erst nach vollständiger Freigabe wieder als Available gelten.

## 6. Frame-Verarbeitung

1. **Acquire:** Verfügbaren Buffer auswählen.
2. **Render:** Neuen Frame erzeugen.
3. **Signal:** Rendering-Abschluss signalisieren.
4. **Queue:** Frame zur Präsentation bereitstellen.
5. **Select:** Geeigneten aktuellen Frame auswählen.
6. **Present:** Frame synchronisiert ausgeben.
7. **Release:** Nicht mehr verwendete Buffer freigeben.
8. **Reuse:** Freigegebene Buffer erneut bereitstellen.

Der Scheduler SOLL möglichst aktuelle Frames bevorzugen und unnötige Warteschlangen vermeiden.

## 7. Adaptive Steuerung

Die Auswahl der Buffering-Strategie berücksichtigt:

- Verfügbare Grafik- und Systemspeicherressourcen.
- Gemessene Rendering- und Präsentationszeiten.
- Eingabe-zu-Anzeige-Latenz.
- Bildwiederholrate und VRR-Unterstützung.
- Anzahl wartender Frames.
- Aktuelle GPU-Auslastung.
- Anforderungen interaktiver Anwendungen.

Die Strategie DARF automatisch angepasst werden, sofern Synchronisation und Bildkonsistenz gewährleistet bleiben.

## 8. Performance und Zuverlässigkeit

- Minimierung der Eingabelatenz.
- Vermeidung unnötiger Frame-Warteschlangen.
- Wiederverwendung bestehender Grafikbuffer.
- Reduzierung des Speicherverbrauchs bei geringer Systemlast.
- Unterstützung asynchroner Rendering-Operationen.
- Vermeidung von Tearing durch geeignete Präsentationssynchronisation.
- Kontrollierte Behandlung verspäteter Frames.
- Vermeidung von Buffer-Starvation.
- Wiederherstellung gültiger Buffer-Zustände nach Grafikfehlern.
- Integration in Nova Compositor Fault Recovery.

## 9. Ergebnis

NovaOS erhält eine flexible und adaptive Architektur für Double und Triple Buffering.

Double Buffering bildet den ressourcenschonenden Standard, während Triple Buffering bei Bedarf eine bessere Entkopplung von Rendering und Präsentation ermöglicht.

Die dynamische Steuerung gewährleistet eine ausgewogene Balance zwischen Bildqualität, Eingabelatenz, Speicherverbrauch und Grafikleistung.