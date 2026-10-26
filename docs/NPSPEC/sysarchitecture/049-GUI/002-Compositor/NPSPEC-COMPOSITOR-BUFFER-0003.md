# NPSPEC-COMPOSITOR-BUFFER-0003 – Nova Zero-Copy Buffer Sharing

**Status:** Angenommen  
**Bereich:** Compositor / Zero-Copy und Buffer-Sharing  
**System:** NovaOS

## 1. Zweck

Nova Zero-Copy Buffer Sharing definiert die gemeinsame Nutzung grafischer Speicherbuffer ohne unnötige Kopiervorgänge zwischen Anwendungen, Compositor und Grafikhardware.

Ziel ist die Reduzierung von Speicherbandbreite, CPU-Auslastung, Energieverbrauch und Darstellungslatenz bei vollständiger Wahrung der Speicher- und Prozessisolation.

## 2. Architektur

- Integration in Nova Graphics Buffer Management.
- Capability-basierte Freigabe grafischer Speicherressourcen.
- Prozessübergreifender Buffer-Austausch über sichere Handles.
- Unterstützung gemeinsam nutzbarer CPU- und GPU-Speicher.
- Hardwareunabhängige Schnittstelle mit gerätespezifischen Backends.
- Synchronisation über Nova Surface Synchronization.
- Integration in Nova Display Protocol und Compositor Pipeline.
- Unterstützung von Direct Scanout und Hardware-Overlays.
- Kontrollierter Fallback auf kopierende Übertragungsverfahren.

## 3. Kernfunktionen

- Zero-Copy-Übergabe von Grafikbuffern.
- Import und Export gemeinsam nutzbarer Buffer.
- Sichere Übertragung von Buffer-Handles über IPC.
- Verwaltung von Eigentum und Zugriffsrechten.
- Unterstützung unterschiedlicher Pixelformate und Speicherlayouts.
- Synchronisation zwischen Produzenten und Verbrauchern.
- Verwaltung gemeinsamer Buffer-Referenzen.
- Unterstützung von GPU-zu-GPU-Sharing.
- Prüfung der Kompatibilität von Speicherressourcen.
- Automatische Auswahl geeigneter Übertragungsverfahren.

## 4. Normative Anforderungen

- BUFFER-001: Zero-Copy-Sharing MUSS für kompatible Speicherressourcen unterstützt werden.
- BUFFER-002: Buffer-Freigaben MÜSSEN durch Capabilities autorisiert werden.
- BUFFER-003: Prozessübergreifende Freigaben DÜRFEN keine rohen virtuellen Speicheradressen übertragen.
- BUFFER-004: Importierte Buffer MÜSSEN hinsichtlich Größe, Format, Layout und Berechtigungen validiert werden.
- BUFFER-005: Zugriffsrechte MÜSSEN auf die erforderlichen Operationen begrenzt werden können.
- BUFFER-006: Lesende Freigaben DÜRFEN keine impliziten Schreibrechte gewähren.
- BUFFER-007: Gemeinsame Buffer-Zugriffe MÜSSEN synchronisiert werden.
- BUFFER-008: Acquire- und Release-Fences MÜSSEN unterstützt werden.
- BUFFER-009: Buffer DÜRFEN während aktiver Nutzung nicht vorzeitig freigegeben werden.
- BUFFER-010: Zero-Copy DARF die Speicherisolation zwischen Prozessen nicht umgehen.
- BUFFER-011: Nicht initialisierte oder fremde Speicherbereiche DÜRFEN nicht offengelegt werden.
- BUFFER-012: Unterschiedliche Speicherlayouts MÜSSEN auf Kompatibilität geprüft werden.
- BUFFER-013: Bei inkompatiblen Ressourcen MUSS ein sicherer Kopier-Fallback verfügbar sein.
- BUFFER-014: Direct Scanout SOLL ohne zusätzliche Buffer-Kopien ermöglicht werden.
- BUFFER-015: Der Widerruf von Freigaben MUSS neue Zugriffe verhindern und bestehende Zugriffe kontrolliert behandeln.
- BUFFER-016: Ressourcenverbrauch und Anzahl freigegebener Buffer MÜSSEN begrenzbar sein.
- BUFFER-017: Fehlerhafte Clients DÜRFEN gemeinsam genutzte Ressourcen anderer Clients nicht unkontrolliert beschädigen.
- BUFFER-018: Zero-Copy-Optimierungen DÜRFEN die Bildkonsistenz nicht beeinträchtigen.

## 5. Sharing-Modell

### Buffer Owner
Verantwortlich für die ursprüngliche Ressource und deren Freigabeberechtigungen.

### Buffer Export
Erzeugt einen sicheren, übertragbaren Handle mit definierten Zugriffsrechten.

### Buffer Import
Bindet einen freigegebenen Buffer in den Adress- oder Ressourcenraum eines berechtigten Verbrauchers ein.

### Shared Access
Ermöglicht die gemeinsame Nutzung derselben physischen Speicherressource.

### Synchronization
Koordiniert Zugriffe zwischen Rendering, Komposition und Präsentation.

### Buffer Release
Beendet die Nutzung durch einen Verbraucher, ohne die Ressource vorzeitig zu zerstören.

## 6. Zero-Copy-Ablauf

1. **Allocate:** Gemeinsam nutzbaren Grafikbuffer erstellen.
2. **Render:** Bildinhalt durch den Produzenten erzeugen.
3. **Export:** Sicheren Buffer-Handle bereitstellen.
4. **Authorize:** Zugriffsrechte des Empfängers prüfen.
5. **Import:** Buffer ohne Kopie einbinden.
6. **Synchronize:** Fertigstellung der Schreiboperationen abwarten.
7. **Consume:** Buffer für Komposition oder Scanout verwenden.
8. **Release:** Abschluss der Nutzung signalisieren.
9. **Reuse:** Buffer nach vollständiger Freigabe erneut verwenden.

## 7. Hardwarekompatibilität

Zero-Copy-Sharing berücksichtigt:

- Unterstützte Pixelformate und Farbtiefen.
- Speicherlayout, Stride und Alignment.
- GPU-spezifische Tiling- und Kompressionsverfahren.
- Speicherzugänglichkeit beteiligter Geräte.
- Anforderungen an Cache-Kohärenz.
- Synchronisationsfähigkeiten der Hardware.
- Unterstützung für Direct Scanout und Hardware-Overlays.

Bei mehreren GPUs DARF Zero-Copy nur verwendet werden, wenn die beteiligten Geräte den gemeinsamen Speicher sicher und korrekt nutzen können.

## 8. Performance und Zuverlässigkeit

- Vermeidung unnötiger CPU-GPU-Speicherkopien.
- Minimierung von Speicherbandbreite und Energieverbrauch.
- Asynchroner Buffer-Austausch.
- Wiederverwendung importierter Buffer-Handles.
- Reduzierung unnötiger Speicherallokationen.
- Begrenzung zusätzlicher Synchronisationskosten.
- Erkennung inkompatibler Speicherressourcen.
- Kontrollierter Fallback auf kopierende Verfahren.
- Schutz vor Use-after-Free und Datenrennen.
- Bereinigung verwaister Buffer-Freigaben.
- Wiederherstellung nach GPU- oder Compositor-Fehlern.
- Messung von Kopiervermeidung, Übertragungslatenz und Ressourcenverbrauch.

## 9. Ergebnis

NovaOS erhält eine sichere und leistungsfähige Zero-Copy-Architektur für den Austausch grafischer Speicherbuffer.

Anwendungen, Compositor und Display-Hardware können kompatible Speicherressourcen direkt gemeinsam verwenden, ohne Bilddaten unnötig zu kopieren.

Dadurch werden Speicherbandbreite, CPU-Auslastung und Darstellungslatenz reduziert, während Capability-Sicherheit, Synchronisation und Speicherisolation erhalten bleiben.