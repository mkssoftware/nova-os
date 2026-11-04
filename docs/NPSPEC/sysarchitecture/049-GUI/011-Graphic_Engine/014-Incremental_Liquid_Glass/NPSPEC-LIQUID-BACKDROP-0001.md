# NPSPEC-LIQUID-BACKDROP-0001 – Nova Liquid Glass Backdrop Sampling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova Liquid Glass

## 1. Zweck

Nova Liquid Glass Backdrop Sampling definiert die Erfassung und Bereitstellung von Hintergrundpixeln, die für die Darstellung von Liquid-Glass-Materialien benötigt werden.

Ziel ist die effiziente Verarbeitung von Hintergrundinformationen für Blur, Refraction, Reflection und weitere Materialeffekte.

Backdrop Sampling MUSS ausschließlich die tatsächlich benötigten Hintergrundbereiche verarbeiten und unveränderte Ergebnisse möglichst wiederverwenden.

## 2. Architekturprinzipien

- **Backdrop-Aware:** Hintergrundinhalte werden entsprechend ihrer tatsächlichen Sichtbarkeit und Effektabhängigkeit berücksichtigt.
- **Region-Based:** Abtastungen erfolgen auf begrenzten Pixelregionen.
- **Incremental:** Nur veränderte Hintergrundbereiche werden aktualisiert.
- **Dependency-Driven:** Materialeffekte definieren ihre benötigten Eingaberegionen.
- **Zero-Copy:** Vorhandene GPU-Ressourcen werden möglichst direkt verwendet.
- **Cache-Aware:** Gültige Hintergrundinformationen werden wiederverwendet.
- **Compositor-Integrated:** Hintergrundinformationen stammen aus dem definierten Kompositionszustand.
- **Secure:** Geschützte Oberflächeninhalte bleiben isoliert.

## 3. Architektur

Nova Liquid Glass Backdrop Sampling besteht aus folgenden logischen Komponenten:

| Komponente | Verantwortung |
|---|---|
| Backdrop Resolver | Ermittlung relevanter Hintergrundinhalte |
| Sampling Region Mapper | Berechnung benötigter Eingaberegionen |
| Backdrop Source Manager | Bereitstellung geeigneter Hintergrundressourcen |
| Backdrop Dependency Tracker | Erfassung relevanter Änderungen |
| Backdrop Cache Controller | Wiederverwendung gültiger Ergebnisse |
| Backdrop Sampling Executor | Ausführung der erforderlichen Abtastoperationen |

Die Implementierung MUSS die bestehenden Funktionen von Compositor, Render Graph, Damage Tracking und GPU Resource Management verwenden.

Eine unabhängige zweite Kompositionsinfrastruktur DARF nicht entstehen.

## 4. Backdrop Model

Jede Backdrop-Anforderung MUSS mindestens folgende Informationen enthalten:

| Eigenschaft | Beschreibung |
|---|---|
| Surface ID | Zugehörige Glasoberfläche |
| Backdrop Source | Logische Hintergrundquelle |
| Composition Version | Version des zugrunde liegenden Kompositionszustands |
| Sampling Region | Benötigter Eingabebereich |
| Coordinate Transform | Abbildung zwischen Material- und Hintergrundkoordinaten |
| Sampling Parameters | Filterung und Abtastverhalten |
| Color Space | Farbraum der Eingabedaten |
| Resource Reference | Referenz auf die verwendete Ressource |
| Dependency Version | Version der relevanten Abhängigkeiten |

Backdrop-Anforderungen MÜSSEN unabhängig von physischen GPU-Speicheradressen beschrieben werden.

## 5. Backdrop Source Resolution

NovaOS MUSS Hintergrundquellen anhand der tatsächlichen Kompositionsreihenfolge ermitteln.

Dabei gilt:

- Nur Inhalte hinter der jeweiligen Glasoberfläche gehören zum Backdrop.
- Vordergrundinhalte DÜRFEN nicht versehentlich in die Hintergrundabtastung einfließen.
- Transparente und teilweise transparente Oberflächen MÜSSEN korrekt berücksichtigt werden.
- Transformationen, Clipping und Ebenenreihenfolge MÜSSEN beachtet werden.
- Änderungen der Fensterreihenfolge MÜSSEN relevante Backdrop-Ergebnisse invalidieren.

Der Backdrop MUSS einen definierten Kompositionszustand repräsentieren.

## 6. Sampling Regions

NovaOS MUSS die benötigten Eingaberegionen aus den Materialeffekten ableiten.

Dabei gilt:

- Blur erweitert den benötigten Eingabebereich entsprechend seiner Filterwirkung.
- Refraction berücksichtigt mögliche Abtastverschiebungen.
- Transformationen MÜSSEN korrekt auf Hintergrundkoordinaten abgebildet werden.
- Clipping DARF erforderliche Eingabepixel nicht vorzeitig entfernen.
- Nicht sicher begrenzbare Abtastbereiche MÜSSEN konservativ erweitert werden.

Die benötigte Eingaberegion kann größer als die sichtbare Glasfläche sein.

## 7. Sampling Methods

NovaOS SOLL unterschiedliche Abtastverfahren unterstützen.

Dazu gehören:

- Direkte Texturabtastung.
- Bilineare Filterung.
- Höherwertige Filterverfahren, sofern unterstützt.
- Abtastung vorbereiteter Backdrop-Texturen.
- Abtastung geeigneter Zwischenauflösungen.
- GPU-basierte und softwarebasierte Verfahren.

Das verwendete Verfahren MUSS mit den Materialanforderungen und dem gewählten Qualitätsprofil kompatibel sein.

## 8. Incremental Backdrop Sampling

NovaOS MUSS inkrementelle Hintergrundabtastung unterstützen.

Der logische Ablauf lautet:

`Backdrop Change → Dependency Analysis → Sampling Region Mapping → Cache Validation → Partial Update`

Dabei gilt:

1. Hintergrundänderungen werden erkannt.
2. Betroffene Glasoberflächen werden ermittelt.
3. Benötigte Eingaberegionen werden berechnet.
4. Gültige Backdrop-Ergebnisse werden wiederverwendet.
5. Ungültige Bereiche werden aktualisiert.
6. Abhängige Materialeffekte erhalten die neuen Ergebnisse.

Unveränderte Hintergrundbereiche SOLLEN keine erneute Abtastung verursachen.

## 9. Backdrop Caching

NovaOS SOLL vorbereitete Backdrop-Ergebnisse zwischenspeichern.

Dabei gilt:

- Cache-Einträge MÜSSEN an den relevanten Kompositionszustand gebunden sein.
- Gültige Teilregionen SOLLEN wiederverwendet werden.
- Änderungen hinter Glasflächen MÜSSEN betroffene Bereiche invalidieren.
- Unterschiedliche Sampling-Konfigurationen MÜSSEN berücksichtigt werden.
- Gemeinsame Backdrop-Ergebnisse DÜRFEN von kompatiblen Materialinstanzen verwendet werden.

Die Speicherung erfolgt über Nova Material Effect Caching.

## 10. Compositor Integration

Nova Liquid Glass Backdrop Sampling MUSS mit Nova Compositor zusammenarbeiten.

Dabei gilt:

- Der Compositor stellt die relevante Ebenen- und Oberflächenstruktur bereit.
- Backdrop-Ergebnisse MÜSSEN die korrekte Kompositionsreihenfolge berücksichtigen.
- Hardware Overlays und Direct Scanout DÜRFEN erforderliche Backdrop-Inhalte nicht unzugänglich machen.
- Falls ein direkter Hardwarepfad die korrekte Hintergrundabtastung verhindert, MUSS eine geeignete Kompositionsalternative verwendet werden.
- Unveränderte Kompositionsergebnisse SOLLEN wiederverwendet werden.

## 11. Render Graph Integration

Backdrop Sampling MUSS in Nova Render Graph integrierbar sein.

Dabei gilt:

- Benötigte Hintergrundressourcen werden als Eingaben beschrieben.
- Abhängigkeiten zu vorgelagerten Kompositions- und Renderoperationen MÜSSEN berücksichtigt werden.
- Sampling-Ergebnisse können nachfolgenden Blur-, Refraction- und Material-Passes bereitgestellt werden.
- GPU Resource Barriers MÜSSEN korrekt geplant werden.
- Geeignete Verarbeitungsschritte SOLLEN zusammengefasst werden.

Eine Abtastung DARF nicht auf unvollständige oder noch nicht freigegebene GPU-Ergebnisse zugreifen.

## 12. Zero-Copy und Resource Management

NovaOS SOLL vorhandene GPU-Ressourcen direkt für Backdrop Sampling verwenden.

Dabei gilt:

- Unnötige CPU-GPU-Kopien SOLLEN vermieden werden.
- Ressourcenformate und Zugriffsrechte MÜSSEN kompatibel sein.
- Aktive Ressourcen bleiben bis zum Abschluss aller Zugriffe gültig.
- Temporäre Backdrop-Texturen SOLLEN wiederverwendet werden.
- Ressourcenbudgets MÜSSEN berücksichtigt werden.

Zero-Copy DARF nicht zulasten von Sicherheit oder korrekter Synchronisation erzwungen werden.

## 13. Color Management

Backdrop Sampling MUSS die Farbraum- und Alpha-Eigenschaften der verwendeten Hintergrunddaten berücksichtigen.

Dabei gilt:

- Unterschiedliche Farbräume MÜSSEN korrekt behandelt werden.
- Alpha-Komposition MUSS konsistent erfolgen.
- Filteroperationen SOLLEN in einem dafür geeigneten Farbraum ausgeführt werden.
- HDR- und SDR-Inhalte MÜSSEN entsprechend den Fähigkeiten des Display- und Color-Management-Systems verarbeitet werden.

Unterschiedliche Farbformate DÜRFEN keine unkontrollierten Darstellungsabweichungen verursachen.

## 14. Sicherheit und Isolation

Backdrop Sampling MUSS die NovaOS-Capability-Architektur respektieren.

Dabei gilt:

- Anwendungen DÜRFEN nicht automatisch direkten Zugriff auf fremde Oberflächeninhalte erhalten.
- Hintergrundabtastungen für systemseitige Materialeffekte MÜSSEN durch den autorisierten Compositor erfolgen.
- Geschützte Inhalte MÜSSEN entsprechend ihrer Sicherheitsrichtlinien ausgeschlossen oder durch zulässige Ersatzwerte dargestellt werden.
- Gemeinsam genutzte Backdrop-Caches DÜRFEN keine geschützten Informationen offenlegen.
- Ressourcenhandles und Zugriffsrechte MÜSSEN validiert werden.

## 15. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Fehlende Backdrop-Ressourcen.
- Ungültige Sampling-Regionen.
- Nicht unterstützte Texturformate.
- Fehlgeschlagene GPU-Synchronisation.
- Ressourcenmangel.
- Ungültige Kompositionszustände.
- GPU-Geräteverlust.

Bei nicht verfügbarer Hintergrundabtastung MUSS ein geeigneter Material-Fallback verwendet werden.

Eine unvollständige oder ungültige Backdrop-Ressource DARF nicht als gültiges Ergebnis veröffentlicht werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Backdrop Sampling für Liquid Glass unterstützen.
2. Hintergrundquellen MÜSSEN anhand des tatsächlichen Kompositionszustands ermittelt werden.
3. Sampling-Regionen MÜSSEN aus den Materialabhängigkeiten abgeleitet werden.
4. Blur- und Refraction-Abtastbereiche MÜSSEN berücksichtigt werden.
5. Inkrementelle Hintergrundabtastung MUSS unterstützt werden.
6. Unveränderte Backdrop-Ergebnisse SOLLEN wiederverwendet werden.
7. Nova Liquid Glass Dependency Tracking MUSS integriert werden.
8. Nova Damage Tracking MUSS relevante Hintergrundänderungen bereitstellen.
9. Nova Material Effect Caching MUSS integrierbar sein.
10. Nova Compositor MUSS die erforderlichen Hintergrundinformationen bereitstellen.
11. Nova Render Graph MUSS Sampling-Abhängigkeiten berücksichtigen.
12. Zero-Copy SOLL bevorzugt werden, sofern sicher und technisch möglich.
13. GPU-Ressourcen und Synchronisation MÜSSEN korrekt verwaltet werden.
14. Farbräume und Alpha-Eigenschaften MÜSSEN berücksichtigt werden.
15. Geschützte Oberflächeninhalte DÜRFEN nicht unautorisiert offengelegt werden.
16. Bei fehlenden Hintergrunddaten MUSS ein geeigneter Fallback verfügbar sein.
17. Im deterministischen Modus MÜSSEN logische Sampling-Entscheidungen bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Liquid Glass Backdrop Sampling ermöglicht die präzise und effiziente Bereitstellung der Hintergrundinformationen für Liquid-Glass-Effekte.

Durch räumlich begrenzte Abtastung, inkrementelle Aktualisierung, Cache-Wiederverwendung und direkte GPU-Ressourcennutzung werden unnötige Berechnungen und Speicheroperationen vermieden.

Damit erhält NovaOS eine leistungsfähige Grundlage für Blur, Refraction und weitere hintergrundabhängige Liquid-Glass-Effekte, ohne unveränderte Hintergrundbereiche permanent neu verarbeiten zu müssen.