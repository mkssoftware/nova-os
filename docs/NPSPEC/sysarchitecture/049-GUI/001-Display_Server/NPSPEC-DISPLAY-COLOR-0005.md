# NPSPEC-DISPLAY-COLOR-0005 – Nova Color Space Conversion

**Status:** Angenommen  
**Bereich:** Display / Farbkonvertierung  
**System:** NovaOS

## 1. Zweck

Nova Color Space Conversion definiert die Umwandlung von Farbwerten zwischen unterschiedlichen Farbräumen, Farbmodellen und Übertragungsfunktionen.

Ziel ist eine präzise, performante und konsistente Farbverarbeitung innerhalb des gesamten NovaOS-Grafiksystems.

## 2. Architektur

- Integration in Nova Color Management.
- Zentrale Farbkonvertierungsfunktionen über das Nova Framework.
- Nutzung durch Compositor, Grafikbibliothek und Nova UI.
- Trennung zwischen Farbmodell, Farbraum und Übertragungsfunktion.
- Unterstützung CPU- und GPU-basierter Konvertierungen.
- Hardwareunabhängige Konvertierungsschnittstellen.
- Erweiterbare Unterstützung zusätzlicher Farbformate.
- Keine Abhängigkeit von einer externen Datenbank.

## 3. Kernfunktionen

- Konvertierung zwischen RGB-, XYZ-, Lab- und YCbCr-Farbmodellen.
- Unterstützung von sRGB, Display P3, Adobe RGB und Rec. 2020.
- Verarbeitung linearer und nichtlinearer Farbwerte.
- Unterstützung von Gamma-, PQ- und HLG-Übertragungsfunktionen.
- Konvertierung zwischen SDR- und HDR-Farbräumen.
- Anpassung unterschiedlicher Weißpunkte.
- Chromatische Adaption.
- Gamut Mapping bei unterschiedlichen Farbumfängen.
- Verarbeitung unterschiedlicher Farbtiefen.
- Unterstützung ICC-basierter Farbtransformationen.
- Bereitstellung vorberechneter Farbtransformationen.
- Batch-Konvertierung großer Pixelmengen.

## 4. Normative Anforderungen

- COLOR-001: Quell- und Zielfarbraum MÜSSEN eindeutig definiert sein.
- COLOR-002: Farbkonvertierungen MÜSSEN die jeweiligen Übertragungsfunktionen berücksichtigen.
- COLOR-003: sRGB, Display P3 und Rec. 2020 MÜSSEN unterstützt werden.
- COLOR-004: RGB- und XYZ-Konvertierungen MÜSSEN unterstützt werden.
- COLOR-005: Lab- und YCbCr-Konvertierungen SOLLEN unterstützt werden.
- COLOR-006: Nichtlineare Farbwerte MÜSSEN vor erforderlichen linearen Farboperationen korrekt dekodiert werden.
- COLOR-007: Unterschiedliche Weißpunkte MÜSSEN durch definierte chromatische Adaption berücksichtigt werden.
- COLOR-008: Farbtransformationen MÜSSEN mit ausreichender numerischer Präzision erfolgen.
- COLOR-009: Nicht darstellbare Farben MÜSSEN durch definierte Gamut-Mapping-Verfahren behandelt werden können.
- COLOR-010: Farbkonvertierungen DÜRFEN keine unbeabsichtigten Änderungen des Alphakanals verursachen.
- COLOR-011: Alpha-Komposition MUSS im dafür definierten Farbraum erfolgen.
- COLOR-012: SDR- und HDR-Übertragungsfunktionen MÜSSEN korrekt verarbeitet werden.
- COLOR-013: ICC-basierte Transformationen MÜSSEN mit Nova Color Profiles kompatibel sein.
- COLOR-014: Häufig verwendete Transformationen SOLLEN zwischengespeichert werden.
- COLOR-015: GPU-beschleunigte Konvertierung SOLL bevorzugt werden, sofern wirtschaftlich und verfügbar.
- COLOR-016: CPU- und GPU-Implementierungen MÜSSEN innerhalb definierter Fehlertoleranzen konsistente Ergebnisse liefern.
- COLOR-017: Ungültige Farbparameter MÜSSEN kontrolliert behandelt werden.
- COLOR-018: Unnötige mehrfache Farbkonvertierungen MÜSSEN vermieden werden.

## 5. Konvertierungsverfahren

### Transfer Function Conversion
Dekodierung und Kodierung nichtlinearer Farbwerte einschließlich Gamma, PQ und HLG.

### Matrix Transformation
Umwandlung zwischen kompatiblen Farbräumen über definierte Transformationsmatrizen.

### Chromatic Adaptation
Anpassung unterschiedlicher Weißpunkte durch standardisierte Adaptionsverfahren.

### ICC Transformation
Profilbasierte Farbkonvertierung unter Verwendung von ICC-Profilen und Lookup-Tabellen.

### Gamut Mapping
Kontrollierte Anpassung von Farben außerhalb des Zielfarbraums.

### Precision Conversion
Umwandlung zwischen unterschiedlichen Farbpräzisionen einschließlich Rundung und optionalem Dithering.

## 6. Konvertierungsablauf

1. **Identify:** Quell- und Zielfarbraum bestimmen.
2. **Decode:** Quellwerte entsprechend ihrer Übertragungsfunktion dekodieren.
3. **Transform:** Farbwerte in den Ziel- oder Zwischenfarbraum umwandeln.
4. **Adapt:** Erforderliche Weißpunktanpassung durchführen.
5. **Map:** Nicht darstellbare Farben kontrolliert behandeln.
6. **Encode:** Zielübertragungsfunktion anwenden.
7. **Output:** Farbwerte im gewünschten Zielformat bereitstellen.

Nicht erforderliche Verarbeitungsschritte SOLLEN automatisch übersprungen werden.

## 7. Performance und Zuverlässigkeit

- GPU-beschleunigte Matrix- und LUT-Transformationen.
- SIMD-optimierte CPU-Konvertierung.
- Caching häufig verwendeter Transformationsmatrizen.
- Wiederverwendung vorberechneter Lookup-Tabellen.
- Zusammenfassung aufeinanderfolgender Farbtransformationen.
- Vermeidung unnötiger Zwischenbuffer.
- Unterstützung hochpräziser Berechnungsformate.
- Kontrollierte Behandlung von Rundungsfehlern.
- Minimierung sichtbarer Farbabweichungen und Banding.
- Software-Fallback bei fehlender GPU-Unterstützung.

## 8. Ergebnis

NovaOS erhält eine einheitliche, präzise und leistungsfähige Farbkonvertierungsarchitektur.

Unterschiedliche Farbmodelle, Farbräume und Übertragungsfunktionen können zuverlässig ineinander umgewandelt werden.

Die Architektur bildet eine gemeinsame Grundlage für Nova Color Management, HDR, professionelle Bildverarbeitung und die farbkonsistente Darstellung grafischer Oberflächen.