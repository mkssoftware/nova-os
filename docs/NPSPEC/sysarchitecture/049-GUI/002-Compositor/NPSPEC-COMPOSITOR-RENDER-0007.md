# NPSPEC-COMPOSITOR-RENDER-0007 – Nova Direct Scanout

**Status:** Angenommen  
**Bereich:** Compositor / Direkte Bildausgabe  
**System:** NovaOS

## 1. Zweck

Nova Direct Scanout definiert die direkte Ausgabe geeigneter Grafikbuffer über die Display-Hardware, ohne diese zuvor in einen separaten Compositor-Framebuffer kopieren oder rendern zu müssen.

Ziel ist die Reduzierung von GPU-Auslastung, Speicherbandbreite, Energieverbrauch und Darstellungslatenz.

## 2. Architektur

- Integration in Nova Compositor Pipeline und Display Server.
- Direkte Übergabe kompatibler Surface-Buffer an den Display-Controller.
- Steuerung über hardwareunabhängige Display-Schnittstellen.
- Gerätespezifische Implementierung durch Grafiktreiber.
- Unterstützung von Vollbildfenstern und geeigneten Einzelflächen.
- Integration in Nova Buffer Synchronization.
- Unterstützung von VSync und Variable Refresh Rate.
- Unabhängige Direct-Scanout-Verwaltung pro Display.
- Automatischer Fallback auf reguläre GPU- oder CPU-Komposition.

## 3. Kernfunktionen

- Erkennung geeigneter Direct-Scanout-Kandidaten.
- Prüfung von Buffer-Format, Layout und Speicherplatzierung.
- Prüfung von Auflösung, Skalierung und Transformationen.
- Direkte Zuweisung von Grafikbuffern an Display-Planes.
- Synchronisierte Buffer-Übergabe.
- Unterstützung von Page Flipping.
- Kombination mit Hardware-Overlays.
- Dynamischer Wechsel zwischen Direct Scanout und regulärer Komposition.
- Unterstützung von SDR- und kompatiblen HDR-Inhalten.
- Erkennung inkompatibler Effekte und Überlagerungen.
- Verwaltung der Buffer-Lebensdauer während der Bildausgabe.

## 4. Normative Anforderungen

- RENDER-001: Direct Scanout MUSS bei geeigneter Hardware unterstützt werden.
- RENDER-002: Die Eignung eines Buffers MUSS vor seiner direkten Ausgabe geprüft werden.
- RENDER-003: Pixelformat, Speicherlayout, Alignment und Buffer-Eigenschaften MÜSSEN mit den Anforderungen des Display-Controllers kompatibel sein.
- RENDER-004: Auflösung, Skalierung und Transformationen MÜSSEN von der verwendeten Display-Plane unterstützt werden.
- RENDER-005: Direct Scanout DARF nur verwendet werden, wenn das resultierende Bild der vorgesehenen Szenendarstellung entspricht.
- RENDER-006: Transparente oder überlagerte Inhalte MÜSSEN korrekt berücksichtigt werden.
- RENDER-007: Nicht unterstützte Effekte MÜSSEN einen Fallback auf reguläre Komposition auslösen.
- RENDER-008: Geschützte Systemoberflächen MÜSSEN jederzeit korrekt dargestellt werden können.
- RENDER-009: Die direkte Bildausgabe MUSS mit Nova Buffer Synchronization abgestimmt werden.
- RENDER-010: Buffer DÜRFEN während aktiver Scanout-Nutzung nicht vorzeitig freigegeben oder überschrieben werden.
- RENDER-011: VSync und VRR MÜSSEN entsprechend den Hardwarefähigkeiten berücksichtigt werden.
- RENDER-012: Direct Scanout MUSS pro Display unabhängig verwaltet werden können.
- RENDER-013: Der Wechsel zwischen Direct Scanout und regulärer Komposition MUSS ohne undefinierte Bildinhalte erfolgen.
- RENDER-014: Hardware-Overlays SOLLEN genutzt werden, wenn dadurch zusätzliche Kompositionsoperationen vermieden werden.
- RENDER-015: Farbprofile, HDR-Eigenschaften und Farbraumtransformationen MÜSSEN berücksichtigt werden.
- RENDER-016: Bei fehlender Hardwareunterstützung MUSS reguläre Fensterkomposition verfügbar bleiben.
- RENDER-017: Fehlgeschlagene Scanout-Konfigurationen DÜRFEN den Compositor nicht destabilisieren.
- RENDER-018: Die Verwendung von Direct Scanout MUSS diagnostizierbar sein.

## 5. Direct-Scanout-Modell

### Primary Plane Scanout
Ein geeigneter Grafikbuffer wird direkt über die primäre Display-Plane ausgegeben.

### Overlay Plane Scanout
Zusätzliche kompatible Oberflächen werden über Hardware-Overlay-Planes dargestellt.

### Fullscreen Scanout
Ein vollständig sichtbares Vollbildfenster wird ohne zusätzliche Compositor-Renderdurchläufe ausgegeben.

### Direct Page Flip
Der Display-Controller wechselt synchronisiert zwischen geeigneten Grafikbuffern.

### Composited Fallback
Bei inkompatiblen Bedingungen übernimmt der Compositor die reguläre Zusammensetzung des Desktopbildes.

## 6. Eignungsprüfung

Direct Scanout ist nur zulässig, wenn alle relevanten Bedingungen erfüllt sind:

- Die Surface besitzt einen gültigen und vollständig initialisierten Buffer.
- Der Buffer ist für die Display-Hardware zugänglich.
- Format und Speicherlayout werden unterstützt.
- Skalierung und Transformationen sind kompatibel.
- Die erforderliche Farbverarbeitung ist gewährleistet.
- Sichtbare Überlagerungen können korrekt ausgegeben werden.
- Sicherheitsanforderungen werden eingehalten.
- Die notwendige Buffer-Synchronisation ist verfügbar.

Die Eignung MUSS bei relevanten Änderungen der Szene, des Displays oder der Buffer-Eigenschaften erneut geprüft werden.

## 7. Verarbeitungsablauf

1. **Detect:** Geeignete Scanout-Kandidaten ermitteln.
2. **Validate:** Buffer-, Display- und Szeneneigenschaften prüfen.
3. **Select:** Geeignete Display-Plane auswählen.
4. **Configure:** Scanout-Konfiguration vorbereiten und prüfen.
5. **Synchronize:** Fertigstellung der Buffer-Zugriffe sicherstellen.
6. **Commit:** Scanout-Konfiguration atomar übernehmen, sofern unterstützt.
7. **Present:** Buffer direkt über die Display-Hardware ausgeben.
8. **Feedback:** Präsentationsstatus erfassen.
9. **Release:** Nicht mehr verwendete Buffer freigeben.
10. **Fallback:** Bei Bedarf zur regulären Komposition wechseln.

## 8. Performance und Zuverlässigkeit

- Vermeidung zusätzlicher Compositor-Renderdurchläufe.
- Reduzierung von GPU-Auslastung und Speicherbandbreite.
- Minimierung zusätzlicher Framebuffer-Kopien.
- Geringere Eingabe-zu-Anzeige-Latenz.
- Energieeinsparung bei Vollbildanwendungen.
- Unterstützung hardwarebasierter Page-Flips.
- Dynamischer Wechsel zwischen Scanout und Komposition.
- Vermeidung unnötiger Moduswechsel.
- Sichere Buffer-Freigabe nach Abschluss der Scanout-Nutzung.
- Wiederherstellung nach GPU- und Display-Resets.
- Erfassung von Scanout-Nutzung, Fallback-Ursachen und Präsentationslatenzen.

## 9. Ergebnis

NovaOS erhält eine effiziente Architektur zur direkten Ausgabe geeigneter Grafikbuffer über die Display-Hardware.

Vollbildanwendungen und kompatible Oberflächen können ohne zusätzliche Compositor-Renderdurchläufe dargestellt werden.

Durch Direct Scanout, Hardware-Overlays und synchronisierte Page-Flips werden GPU-Auslastung, Speicherbandbreite, Energieverbrauch und Darstellungslatenz reduziert.

Bei inkompatiblen Bedingungen erfolgt automatisch die Rückkehr zur regulären Fensterkomposition.