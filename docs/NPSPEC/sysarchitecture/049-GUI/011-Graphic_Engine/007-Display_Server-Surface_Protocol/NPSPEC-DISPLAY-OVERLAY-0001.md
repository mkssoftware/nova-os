# NPSPEC-DISPLAY-OVERLAY-0001 – Nova Hardware Overlay Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Hardware Overlay Management definiert die Verwaltung dedizierter Hardware-Planes zur direkten Darstellung grafischer Surfaces durch die Display-Hardware.

Ziel ist die Reduzierung unnötiger GPU-Komposition, Speichertransfers und Darstellungslatenzen.

Der Display Server verwaltet die tatsächlichen Hardware-Planes und deren Konfiguration. Die Auswahl geeigneter Surface-Kandidaten erfolgt in Zusammenarbeit mit dem Nova Compositor.

## 2. Architekturprinzipien

- **Hardware-Abstraction:** Einheitliche Verwaltung unterschiedlicher Display-Controller.
- **Zero-Copy:** Direkte Verwendung geeigneter Surface-Buffer.
- **Capability-Aware:** Berücksichtigung tatsächlicher Hardwarefähigkeiten.
- **Atomic:** Konsistente Übernahme zusammengehöriger Plane-Konfigurationen.
- **Synchronization-Aware:** Sichere Buffer-Übergabe an die Display-Hardware.
- **Damage-Aware:** Vermeidung unnötiger Aktualisierungen.
- **Adaptive:** Dynamische Zuweisung verfügbarer Hardware-Planes.
- **Fallback-Safe:** Rückkehr zur regulären Komposition bei fehlender Unterstützung.

## 3. Hardware Plane Model

NovaOS unterscheidet folgende logische Plane-Typen:

| Plane-Typ | Funktion |
|---|---|
| Primary Plane | Hauptbildinhalt einer Ausgabe |
| Overlay Plane | Zusätzliche Hardwareebene |
| Cursor Plane | Hardwarebeschleunigte Cursor-Darstellung |

Die tatsächlich verfügbaren Typen und Funktionen hängen vom Display-Controller ab.

Eine Hardware-Plane besitzt mindestens:

- **Plane ID:** Eindeutige Identifikation.
- **Output Compatibility:** Unterstützte Bildschirmausgaben.
- **Supported Formats:** Zulässige Pixelformate.
- **Memory Modifiers:** Unterstützte Speicherlayouts.
- **Scaling Limits:** Zulässige Skalierungsfaktoren.
- **Transform Support:** Unterstützte Transformationen.
- **Alpha Support:** Transparenzfunktionen.
- **Z-Order Constraints:** Einschränkungen der Ebenenreihenfolge.
- **Color Capabilities:** Unterstützte Farbräume und Farbverarbeitung.

Nicht jede Hardware-Plane MUSS sämtliche Eigenschaften unterstützen.

## 4. Plane Capability Discovery

Der Display Server MUSS die Fähigkeiten verfügbarer Hardware-Planes ermitteln und dem Compositor über eine definierte Schnittstelle bereitstellen.

Dabei MÜSSEN insbesondere berücksichtigt werden:

1. Anzahl und Typ verfügbarer Planes.
2. Unterstützte Ausgaben.
3. Pixel- und Speicherformate.
4. Skalierung und Rotation.
5. Transparenz und Blending.
6. Z-Order-Einschränkungen.
7. Farbverarbeitung und HDR-Fähigkeiten.
8. Synchronisationsanforderungen.

Hardwarefähigkeiten MÜSSEN bei Änderungen der Display-Konfiguration erneut bewertet werden können.

## 5. Plane Allocation

Der Compositor schlägt geeignete Surface-Kandidaten vor.

Der Display Server prüft die tatsächliche Zuweisbarkeit zu verfügbaren Hardware-Planes.

Dabei gilt:

- Eine Plane DARF nur einer kompatiblen Ausgabe zugeordnet werden.
- Buffer-Formate und Speicherlayouts MÜSSEN unterstützt werden.
- Skalierung, Clipping und Transformation MÜSSEN hardwareseitig korrekt ausführbar sein.
- Z-Order und Transparenz MÜSSEN dem vorgesehenen Kompositionsergebnis entsprechen.
- Gemeinsame Hardwarebeschränkungen MÜSSEN berücksichtigt werden.

Die Eignung einzelner Surfaces garantiert nicht die Gültigkeit der gesamten Plane-Konfiguration.

## 6. Atomic Plane Configuration

Zusammengehörige Plane-Änderungen SOLLEN atomar über das Display-Backend übernommen werden.

Eine Konfiguration kann enthalten:

- Plane-Aktivierung oder -Deaktivierung
- Buffer-Zuordnung
- Source- und Destination-Region
- Z-Order
- Alpha und Blending
- Farbparameter
- Synchronisationsinformationen

Vor der Übernahme MUSS die vollständige Konfiguration validiert werden.

Ein fehlgeschlagener Commit DARF nicht als erfolgreich übernommen gelten.

Unterstützt die Hardware keine atomaren Änderungen, MUSS ein sicherer alternativer Aktualisierungspfad verwendet werden.

## 7. Buffer und Synchronization

Hardware Overlays MÜSSEN mit Nova Display Buffer Management und Nova Surface Synchronization zusammenarbeiten.

Dabei gilt:

- Buffer MÜSSEN für die jeweilige Plane geeignet sein.
- Acquire Fences MÜSSEN vor der Verwendung berücksichtigt werden.
- Aktive Scanout-Buffer DÜRFEN nicht unzulässig verändert werden.
- Release-Synchronisation MUSS das tatsächliche Ende der Hardwareverwendung berücksichtigen.
- Buffer DÜRFEN nicht vorzeitig freigegeben werden.

Zero-Copy SOLL bevorzugt werden, sofern sämtliche Anforderungen erfüllt sind.

## 8. Damage und Frame Integration

Nova Hardware Overlay Management MUSS mit Nova Damage Tracking und Nova Frame Presentation zusammenarbeiten.

Dabei gilt:

- Unveränderte Plane-Konfigurationen SOLLEN beibehalten werden.
- Neue Buffer oder geänderte Plane-Eigenschaften lösen erforderliche Aktualisierungen aus.
- Änderungen an Position, Größe und Z-Order MÜSSEN berücksichtigt werden.
- Der Wechsel zwischen Overlay und regulärer Komposition MUSS betroffene Bildbereiche invalidieren.

Die Hardware DARF bestehende Buffer weiterhin auslesen, ohne dass der Compositor neue Inhalte berechnen muss.

## 9. Material und Composition Compatibility

Hardware Overlays DÜRFEN keine erforderlichen Kompositionseffekte umgehen.

Besonders zu berücksichtigen sind:

- Liquid Glass
- Acrylic
- Backdrop Blur
- Refraction
- Reflection
- Transparenz
- Farbkorrektur
- HDR-Verarbeitung

Kann eine Plane die erforderliche Darstellung nicht korrekt gewährleisten, MUSS die betreffende Surface regulär komponiert werden.

## 10. Multi-Display Management

NovaOS MUSS Hardware-Planes für mehrere Bildschirmausgaben verwalten können.

Dabei gilt:

- Jede Ausgabe besitzt eine eigene gültige Plane-Konfiguration.
- Hardwareabhängige gemeinsame Ressourcen werden berücksichtigt.
- Unterschiedliche Auflösungen und Bildwiederholraten werden unterstützt.
- Gemeinsame Buffer müssen über sämtliche Ausgaben hinweg synchronisiert werden.

Eine Plane DARF nicht gleichzeitig inkompatiblen Ausgaben zugewiesen werden.

## 11. Adaptive Plane Management

NovaOS SOLL Hardware-Planes dynamisch zuweisen können.

Die Entscheidung SOLL berücksichtigen:

- Eingesparte GPU-Kompositionsarbeit
- Speicherbandbreite
- Energieverbrauch
- Aktualisierungshäufigkeit
- Anzahl verfügbarer Planes
- Kosten eines Konfigurationswechsels

Häufige unnötige Wechsel zwischen Hardware Overlay und regulärer Komposition SOLLEN vermieden werden.

Die Optimierung DARF die grafische Korrektheit nicht beeinträchtigen.

## 12. Sicherheit und Fallback

Hardware-Plane-Zugriffe MÜSSEN durch den Display Server kontrolliert werden.

Anwendungen DÜRFEN Hardware-Planes nicht ohne entsprechende Berechtigung direkt konfigurieren.

Bei folgenden Situationen MUSS ein sicherer Fallback möglich sein:

- Fehlende Hardwareunterstützung
- Ungültige Plane-Konfiguration
- Inkompatibles Buffer-Format
- Nicht unterstützte Materialeffekte
- Fehlgeschlagene Synchronisation
- Geräteverlust

Der Fallback MUSS auf reguläre GPU- oder Software-Komposition zurückgreifen können.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS verfügbare Hardware-Planes erkennen und verwalten können.
2. Plane-Fähigkeiten MÜSSEN dem Compositor bereitgestellt werden.
3. Die tatsächliche Plane-Zuweisung MUSS durch den Display Server validiert werden.
4. Die vollständige Plane-Konfiguration MUSS hardwarekompatibel sein.
5. Buffer-Formate und Speicherlayouts MÜSSEN geprüft werden.
6. Z-Order, Transparenz und Transformationen MÜSSEN korrekt bleiben.
7. Atomare Plane-Commits SOLLEN unterstützt werden.
8. Buffer-Zugriffe MÜSSEN synchronisiert erfolgen.
9. Aktive Scanout-Buffer DÜRFEN nicht vorzeitig freigegeben werden.
10. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
11. Unveränderte Plane-Konfigurationen SOLLEN wiederverwendet werden.
12. Mehrere Bildschirmausgaben MÜSSEN unterstützt werden können.
13. Adaptive Plane-Zuweisung SOLL möglich sein.
14. Ein sicherer Fallback auf reguläre Komposition MUSS verfügbar sein.
15. Hardware-Plane-Zugriffe MÜSSEN capability-basiert geschützt werden.
16. Im deterministischen Modus MÜSSEN Plane-Zuweisungen bei identischen Eingaben und Hardwarefähigkeiten reproduzierbar sein.

## 14. Ergebnis

Nova Hardware Overlay Management stellt die zentrale Verwaltung dedizierter Display-Hardwareebenen bereit.

Durch Capability Discovery, validierte Plane-Zuweisung, Zero-Copy und atomare Konfigurationswechsel können geeignete Surfaces ohne zusätzliche GPU-Komposition dargestellt werden.

Damit ergänzt das Subsystem die NovaOS-Grafikpipeline um eine effiziente hardwarebeschleunigte Darstellung, während Kompatibilität, Synchronisation und Sicherheit gewährleistet bleiben.