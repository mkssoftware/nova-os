# NPSPEC-DISPLAY-COLOR-0001 – Nova Color Management

**Status:** Angenommen  
**Bereich:** Display / Farbverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Color Management definiert die systemweite Verwaltung, Verarbeitung und korrekte Darstellung von Farben.

Ziel ist eine konsistente, präzise und performante Farbwiedergabe über unterschiedliche Displays, Grafikkarten und Anwendungen hinweg.

## 2. Architektur

- Zentrale Farbverwaltung innerhalb des Nova Display Servers.
- Zusammenarbeit mit Compositor, Grafiktreibern und Nova UI.
- Unterstützung standardisierter Farbräume und Farbprofile.
- Unabhängige Farbkonfiguration pro Display.
- Trennung zwischen Quellfarbraum, Arbeitsfarbraum und Ausgabefarbraum.
- Hardwarebeschleunigte Farbtransformationen, sofern verfügbar.
- Einheitliche Schnittstellen über das Nova Framework.
- Keine Abhängigkeit von einer externen Datenbank.

## 3. Kernfunktionen

- Verwaltung von Farbräumen und Farbprofilen.
- Unterstützung von sRGB, Display P3, Adobe RGB und Rec. 2020.
- Verarbeitung und Zuordnung von ICC-Profilen.
- Farbkonvertierung zwischen unterschiedlichen Farbräumen.
- Verwaltung von Gamma, Weißpunkt und Farbtemperatur.
- Unterstützung von SDR und HDR.
- Verwaltung von Farbtiefe und Farbpräzision.
- Unterstützung von Farbkalibrierung und Display-Profilierung.
- Automatische Zuordnung geeigneter Display-Farbprofile.
- Farbkorrektur und Gamut Mapping.
- Bereitstellung von Farbmetadaten für Anwendungen.
- Speicherung benutzerspezifischer Farbeinstellungen.

## 4. Normative Anforderungen

- COLOR-001: Jeder Display-Ausgang MUSS unabhängig farbverwaltet werden können.
- COLOR-002: Grafikressourcen MÜSSEN ihren Farbraum eindeutig beschreiben können.
- COLOR-003: Nicht gekennzeichnete SDR-Inhalte MÜSSEN standardmäßig als sRGB interpretiert werden, sofern keine andere verbindliche Festlegung existiert.
- COLOR-004: Farbtransformationen MÜSSEN zwischen unterstützten Farbräumen möglich sein.
- COLOR-005: ICC-Farbprofile MÜSSEN unterstützt und vor ihrer Verwendung validiert werden.
- COLOR-006: Der Compositor MUSS unterschiedliche Quellfarbräume innerhalb einer Bildkomposition berücksichtigen.
- COLOR-007: Farbtransformationen SOLLEN hardwarebeschleunigt erfolgen.
- COLOR-008: Unnötige mehrfache Farbkonvertierungen MÜSSEN vermieden werden.
- COLOR-009: Unterschiedliche Farbprofile MÜSSEN gleichzeitig auf mehreren Displays verwendet werden können.
- COLOR-010: SDR- und HDR-Inhalte MÜSSEN entsprechend ihrer Farb- und Helligkeitsmetadaten verarbeitet werden können.
- COLOR-011: Nicht darstellbare Farben MÜSSEN durch definierte Gamut-Mapping-Verfahren behandelt werden.
- COLOR-012: Farbkorrekturen DÜRFEN die ursprünglichen Bilddaten nicht unkontrolliert verändern.
- COLOR-013: Anwendungen MÜSSEN relevante Farbraum- und Displayinformationen abfragen können.
- COLOR-014: Änderungen der Farbkonfiguration MÜSSEN zur Laufzeit möglich sein.
- COLOR-015: Farbprofile und Benutzereinstellungen MÜSSEN persistent gespeichert werden können.
- COLOR-016: Fehlerhafte Farbprofile DÜRFEN den Display Server nicht destabilisieren.
- COLOR-017: Bei fehlender Farbmanagement-Unterstützung MUSS ein definierter SDR-Fallback verfügbar sein.
- COLOR-018: Sicherheitskritische Systemoberflächen MÜSSEN unabhängig von fehlerhaften Anwendungsprofilen lesbar bleiben.

## 5. Farbverarbeitungsmodell

### Source Color Space
Beschreibung des Farbraums und der Übertragungsfunktion einer grafischen Ressource.

### Working Color Space
Definierter Verarbeitungsfarbraum für Farboperationen und Bildkomposition.

### Color Transformation
Konvertierung zwischen Quell-, Arbeits- und Ausgabefarbräumen.

### Display Color Profile
Beschreibung der individuellen Farbwiedergabeeigenschaften eines Displays.

### Output Color Space
Anpassung der komponierten Bilddaten an den tatsächlichen Ausgabefarbraum.

### Gamut Mapping
Kontrollierte Behandlung von Farben außerhalb des darstellbaren Farbumfangs.

## 6. Farbverarbeitung und Darstellung

1. **Identify:** Quellfarbraum und Farbmetadaten bestimmen.
2. **Decode:** Farbwerte entsprechend ihrer Übertragungsfunktion interpretieren.
3. **Transform:** Farben in einen geeigneten Arbeitsfarbraum überführen.
4. **Composite:** Grafische Oberflächen farbkonsistent zusammensetzen.
5. **Map:** Farben und Helligkeitswerte an den Ausgabebereich anpassen.
6. **Output:** Farbkorrektur anwenden und Bilddaten an das Display übergeben.

Die Verarbeitung SOLL mit ausreichender interner Präzision erfolgen, um sichtbare Farbverluste und Banding zu minimieren.

## 7. Performance und Zuverlässigkeit

- GPU-beschleunigte Farbtransformationen.
- Verwendung vorberechneter Lookup-Tabellen (LUTs).
- Caching häufig verwendeter Farbtransformationen.
- Vermeidung unnötiger Farbraumwechsel.
- Unterstützung höherer interner Farbpräzision.
- Effiziente Verarbeitung gemischter SDR- und HDR-Inhalte.
- Wiederverwendung gültiger Farbprofile und Transformationsdaten.
- Kontrollierter Fallback bei nicht unterstützten Farbräumen.
- Dynamische Aktualisierung bei Display- und Profiländerungen.
- Minimierung zusätzlicher Latenzen während der Bildkomposition.

## 8. Ergebnis

NovaOS erhält eine zentrale, hardwareunabhängige und leistungsfähige Farbverwaltung.

Anwendungen können unterschiedliche Farbräume verwenden, während der Compositor eine konsistente und möglichst farbgetreue Darstellung auf allen angeschlossenen Displays gewährleistet.

Die Architektur bildet die Grundlage für professionelle Farbdarstellung, HDR, Display-Kalibrierung und zukünftige Erweiterungen des grafischen Subsystems.