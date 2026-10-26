# NPSPEC-DISPLAY-COLOR-0004 – Nova HDR Support

**Status:** Angenommen  
**Bereich:** Display / HDR und Farbverwaltung  
**System:** NovaOS

## 1. Zweck

Nova HDR Support definiert die Verarbeitung und Ausgabe von High Dynamic Range (HDR) innerhalb des NovaOS-Grafiksystems.

Ziel ist eine hochwertige Darstellung erweiterter Helligkeits- und Farbbereiche sowie die nahtlose Kombination von SDR- und HDR-Inhalten.

## 2. Architektur

- Integration in Nova Color Management und den Nova Display Server.
- HDR-fähige Bildkomposition durch den Compositor.
- Unabhängige HDR-Verwaltung pro Display.
- Unterstützung von HDR10, PQ und HLG.
- Unterstützung erweiterter Farbräume wie Rec. 2020.
- Trennung zwischen HDR-Quellformat, internem Arbeitsfarbraum und Display-Ausgabe.
- Hardwarebeschleunigte Verarbeitung, sofern verfügbar.
- Automatischer SDR-Fallback bei fehlender HDR-Unterstützung.

## 3. Kernfunktionen

- Erkennung HDR-fähiger Displays.
- Verwaltung unterstützter HDR-Ausgabemodi.
- Verarbeitung von HDR-Metadaten.
- Unterstützung von 10-Bit- und höheren Farbpräzisionen.
- Verarbeitung von PQ- und HLG-Übertragungsfunktionen.
- HDR- und SDR-Komposition innerhalb eines Desktops.
- Tone Mapping zwischen unterschiedlichen Helligkeitsbereichen.
- Gamut Mapping für erweiterte Farbräume.
- Verwaltung von Referenzweiß und Spitzenhelligkeit.
- Dynamische Aktivierung und Deaktivierung von HDR.
- HDR-Präsentationsfeedback für Anwendungen.
- Speicherung benutzerspezifischer HDR-Einstellungen.

## 4. Normative Anforderungen

- COLOR-001: HDR-Fähigkeiten MÜSSEN über die zuständigen Grafiktreiber ermittelt werden.
- COLOR-002: HDR MUSS pro Display unabhängig aktiviert werden können.
- COLOR-003: HDR10 mit PQ-Übertragungsfunktion MUSS unterstützt werden.
- COLOR-004: HLG SOLL unterstützt werden.
- COLOR-005: HDR-Inhalte MÜSSEN ihren Farbraum und ihre Übertragungsfunktion eindeutig kennzeichnen.
- COLOR-006: Der Compositor MUSS SDR- und HDR-Inhalte gemeinsam verarbeiten können.
- COLOR-007: Die interne Farbverarbeitung MUSS ausreichende Präzision für HDR gewährleisten.
- COLOR-008: Tone Mapping MUSS unterschiedliche Display-Helligkeitsbereiche berücksichtigen.
- COLOR-009: SDR-Inhalte MÜSSEN bei aktivem HDR-Modus mit kontrollierter Helligkeit dargestellt werden.
- COLOR-010: HDR-Metadaten MÜSSEN validiert und entsprechend dem Ausgabepfad verarbeitet werden.
- COLOR-011: Nicht unterstützte HDR-Inhalte MÜSSEN über einen definierten SDR-Fallback darstellbar sein.
- COLOR-012: HDR-Moduswechsel MÜSSEN ohne Systemneustart möglich sein.
- COLOR-013: Unterschiedliche HDR-Fähigkeiten mehrerer Displays MÜSSEN unabhängig berücksichtigt werden.
- COLOR-014: Anwendungen MÜSSEN unterstützte HDR-Funktionen abfragen können.
- COLOR-015: HDR-Verarbeitung SOLL hardwarebeschleunigt erfolgen.
- COLOR-016: Unnötige Farbkonvertierungen und Tone-Mapping-Durchläufe MÜSSEN vermieden werden.
- COLOR-017: HDR-Einstellungen MÜSSEN persistent gespeichert werden können.
- COLOR-018: Fehlerhafte HDR-Konfigurationen DÜRFEN nicht zum dauerhaften Verlust der Bildausgabe führen.
- COLOR-019: HDR-Verarbeitung MUSS mit Farbprofilen und Gamma-Korrektur abgestimmt werden.
- COLOR-020: Sicherheitskritische Systemoberflächen MÜSSEN auch bei aktivem HDR-Modus zuverlässig lesbar bleiben.

## 5. HDR-Verarbeitungsmodell

### HDR Detection
Erkennung unterstützter HDR-Funktionen, Farbräume und Helligkeitsgrenzen.

### HDR Metadata
Verwaltung statischer HDR-Metadaten und erweiterbarer Metadatentypen.

### HDR Composition
Gemeinsame Verarbeitung von SDR- und HDR-Oberflächen mit geeigneter interner Farbpräzision.

### Tone Mapping
Anpassung der Helligkeitsverteilung an die Fähigkeiten des Ziel-Displays.

### Gamut Mapping
Anpassung des Farbumfangs an den unterstützten Ausgabefarbraum.

### HDR Output
Ausgabe der Bilddaten einschließlich erforderlicher Farb- und HDR-Signalisierung.

## 6. HDR-Verarbeitung

1. **Detect:** HDR-Fähigkeiten des Displays ermitteln.
2. **Negotiate:** Unterstützte HDR-Formate und Ausgabeparameter festlegen.
3. **Decode:** Farbwerte und Übertragungsfunktionen interpretieren.
4. **Composite:** SDR- und HDR-Inhalte in einem geeigneten Arbeitsfarbraum zusammensetzen.
5. **Tone Map:** Helligkeitsbereiche an das Ziel-Display anpassen.
6. **Convert:** Farben in den Ausgabefarbraum transformieren.
7. **Present:** Bilddaten und erforderliche HDR-Metadaten ausgeben.

## 7. Performance und Zuverlässigkeit

- GPU-beschleunigte HDR-Verarbeitung.
- Verwendung hochpräziser interner Farbformate.
- Caching von Farbtransformationen und Tone-Mapping-Parametern.
- Unterstützung von Direct Scanout bei kompatiblen HDR-Inhalten.
- Vermeidung unnötiger Zwischenbuffer.
- Minimierung zusätzlicher Präsentationslatenzen.
- Dynamische Anpassung bei Änderungen der Display-Fähigkeiten.
- Kontrollierter Wechsel zwischen SDR und HDR.
- Automatischer SDR-Fallback bei HDR-Ausgabefehlern.
- Vermeidung von Banding, Clipping und unnötigen Helligkeitssprüngen.

## 8. Ergebnis

NovaOS erhält eine moderne, hardwareunabhängige HDR-Architektur.

SDR- und HDR-Anwendungen können gleichzeitig betrieben und farbkonsistent dargestellt werden.

Die Architektur ermöglicht hochwertige HDR-Ausgabe, präzises Tone Mapping und eine zuverlässige Anpassung an unterschiedliche Display-Technologien.