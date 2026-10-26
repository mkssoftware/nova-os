# NPSPEC-COMPOSITOR-ARCH-0002 – Nova Compositor Pipeline

**Status:** Angenommen  
**Bereich:** Compositor / Rendering-Pipeline  
**System:** NovaOS

## 1. Zweck

Nova Compositor Pipeline definiert die Verarbeitung grafischer Oberflächen vom Eingang der Bilddaten bis zur Ausgabe des fertigen Frames.

Ziel ist eine latenzarme, effiziente und hardwareunabhängige Bildkomposition mit minimalem Ressourcenverbrauch.

## 2. Architektur

- Modulare, mehrstufige Kompositionspipeline.
- Asynchrone und ereignisgesteuerte Verarbeitung.
- Trennung von Szenenverwaltung, Rendering und Präsentation.
- GPU-beschleunigte Verarbeitung mit CPU-Fallback.
- Unabhängige Frame-Verarbeitung pro Display.
- Unterstützung von Zero-Copy und Direct Scanout.
- Dynamische Optimierung anhand sichtbarer Änderungen.
- Integration in Nova Display Server und Nova Color Management.

## 3. Pipeline-Stufen

### Input Collection
Erfassung neuer Surface-Zustände, Buffer und Änderungsereignisse.

### Scene Update
Aktualisierung der Szenenstruktur, Transformationen und Ebenenreihenfolge.

### Visibility Analysis
Ermittlung sichtbarer, verdeckter und veränderter Bildbereiche.

### Composition Planning
Auswahl geeigneter Kompositionsverfahren und Rendering-Ressourcen.

### Rendering
Zusammensetzen sichtbarer Oberflächen einschließlich Transparenz und Effekten.

### Color Processing
Farbtransformation, HDR-Verarbeitung und Anpassung an das Ziel-Display.

### Synchronization
Koordination von GPU-Operationen, Buffern und Präsentationszeitpunkten.

### Presentation
Übergabe des fertigen Frames an den Nova Display Server.

## 4. Normative Anforderungen

- COMPOSITOR-001: Die Pipeline MUSS klar definierte Verarbeitungsstufen besitzen.
- COMPOSITOR-002: Surface-Änderungen MÜSSEN konsistent und atomar übernommen werden.
- COMPOSITOR-003: Unveränderte Bildbereiche SOLLEN nicht erneut gerendert werden.
- COMPOSITOR-004: Vollständig verdeckte Oberflächen SOLLEN von unnötigen Rendering-Operationen ausgeschlossen werden.
- COMPOSITOR-005: Die Pipeline MUSS asynchrone GPU-Operationen unterstützen.
- COMPOSITOR-006: Buffer DÜRFEN während laufender Zugriffe nicht unkontrolliert wiederverwendet werden.
- COMPOSITOR-007: Direct Scanout SOLL bei geeigneten Oberflächen bevorzugt werden.
- COMPOSITOR-008: Hardware-Overlays SOLLEN berücksichtigt werden, sofern verfügbar.
- COMPOSITOR-009: Farbtransformationen MÜSSEN mit Nova Color Management abgestimmt werden.
- COMPOSITOR-010: Unterschiedliche Display-Frequenzen MÜSSEN unabhängig verarbeitet werden.
- COMPOSITOR-011: Die Pipeline MUSS bei fehlender GPU-Beschleunigung einen Softwarepfad bereitstellen.
- COMPOSITOR-012: Nicht benötigte Pipeline-Stufen SOLLEN übersprungen werden.
- COMPOSITOR-013: Frame-Verarbeitung MUSS definierte Zeitlimits und Synchronisationsregeln berücksichtigen.
- COMPOSITOR-014: Langsame Clients DÜRFEN die Bildausgabe anderer Clients nicht blockieren.
- COMPOSITOR-015: Grafische Effekte MÜSSEN bei Ressourcenknappheit kontrolliert reduzierbar sein.
- COMPOSITOR-016: Fehler einzelner Pipeline-Stufen MÜSSEN erkannt und kontrolliert behandelt werden.
- COMPOSITOR-017: Sicherheitskritische Oberflächen MÜSSEN ihre festgelegte Darstellungsebene und Schutzanforderungen behalten.
- COMPOSITOR-018: Die Pipeline MUSS eine bedarfsgerechte Frame-Erzeugung unterstützen.

## 5. Pipeline-Optimierung

### Damage Tracking
Nur tatsächlich veränderte Bildbereiche werden für eine erneute Komposition berücksichtigt.

### Occlusion Culling
Vollständig verdeckte Oberflächen werden von unnötigen Kompositionsoperationen ausgeschlossen.

### Direct Scanout
Geeignete Oberflächen werden ohne zusätzliche Komposition direkt ausgegeben.

### Zero-Copy
Grafikbuffer werden möglichst ohne zusätzliche Speicherkopien verarbeitet.

### Frame Coalescing
Mehrere Änderungen werden innerhalb eines Präsentationszyklus zusammengefasst.

### Adaptive Rendering
Rendering-Aufwand und Effektqualität werden bei Bedarf dynamisch angepasst.

## 6. Frame-Verarbeitung

1. **Acquire:** Verfügbare Surface-Buffer übernehmen.
2. **Update:** Szenenzustand aktualisieren.
3. **Analyze:** Sichtbarkeit und Änderungen bestimmen.
4. **Plan:** Rendering-Pfad und Ressourcen auswählen.
5. **Render:** Erforderliche Bildbereiche komponieren.
6. **Process:** Farb- und Ausgabeanpassungen durchführen.
7. **Sync:** Fertigstellung und Buffer-Zugriffe synchronisieren.
8. **Present:** Frame zur Bildausgabe übergeben.
9. **Release:** Nicht mehr benötigte Buffer freigeben.

Die Pipeline SOLL mehrere Frames parallel vorbereiten können, ohne unnötige Warteschlangen oder zusätzliche Eingabelatenz zu erzeugen.

## 7. Performance und Zuverlässigkeit

- Minimierung der Zeit zwischen Eingabe und Bildausgabe.
- Vermeidung unnötiger GPU- und CPU-Synchronisation.
- Wiederverwendung geeigneter Rendering-Ressourcen.
- Begrenzung gleichzeitig ausstehender Frames.
- Priorisierung aktueller gegenüber veralteten Frames.
- Unabhängige Verarbeitung mehrerer Displays.
- Reduzierung unnötiger Speicherbandbreite.
- Dynamische Anpassung an verfügbare Hardwareleistung.
- Kontrollierter Fallback bei Rendering-Fehlern.
- Erfassung von Frame-Zeiten, Latenzen und ausgelassenen Frames.

## 8. Ergebnis

NovaOS erhält eine modulare und optimierte Rendering-Pipeline für seinen Compositor.

Durch bedarfsgerechte Verarbeitung, intelligente Sichtbarkeitsanalyse und hardwarebeschleunigte Komposition werden unnötige Berechnungen und Speichertransfers minimiert.

Die Architektur ermöglicht flüssige Animationen, geringe Eingabelatenzen und eine energieeffiziente Bildausgabe auf unterschiedlich leistungsfähiger Hardware.