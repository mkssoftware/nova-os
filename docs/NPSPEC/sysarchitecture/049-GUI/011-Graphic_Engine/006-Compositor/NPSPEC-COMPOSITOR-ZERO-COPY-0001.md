# NPSPEC-COMPOSITOR-ZERO-COPY-0001 – Nova Zero-Copy Composition

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Zero-Copy Composition definiert die direkte Verwendung grafischer Surface-Buffer während der Komposition, ohne deren Inhalte unnötig zwischen Speicherbereichen zu kopieren.

Ziel ist die Reduzierung von Speicherbandbreite, CPU- und GPU-Belastung sowie Darstellungslatenzen.

Zero-Copy ist eine Optimierung der Datenübertragung und bedeutet nicht, dass Blending, Materialeffekte oder die eigentliche Komposition entfallen.

## 2. Architekturprinzipien

- **Reference-Based:** Surfaces werden über Ressourcenreferenzen eingebunden.
- **Shared Resources:** Kompatible Buffer können gemeinsam verwendet werden.
- **Copy Avoidance:** Unnötige Speichertransfers werden vermieden.
- **Synchronization-Safe:** Zugriffe erfolgen ausschließlich synchronisiert.
- **Ownership-Aware:** Ressourcenbesitz und Lebensdauer bleiben eindeutig.
- **Hardware-Aware:** GPU-Funktionen und Display-Planes werden berücksichtigt.
- **Fallback-Safe:** Nicht kompatible Ressourcen können regulär verarbeitet werden.

## 3. Zero-Copy Model

Eine Zero-Copy Surface Reference enthält mindestens:

- **Resource ID:** Eindeutige Ressourcenidentifikation.
- **Buffer Handle:** Kontrollierte Referenz auf den Buffer.
- **Format:** Pixel- und Speicherformat.
- **Dimensions:** Größe und Layout.
- **Memory Domain:** CPU-, GPU- oder gemeinsam zugänglicher Speicher.
- **Access Rights:** Zulässige Zugriffsarten.
- **Synchronization State:** Verfügbarkeit der Ressource.
- **Content Version:** Gültige Inhaltsversion.

Eine Referenz MUSS auf eine gültige und für den Compositor zugängliche Ressource verweisen.

## 4. Zero-Copy Composition

NovaOS SOLL folgende Verfahren unterstützen:

| Verfahren | Beschreibung |
|---|---|
| Direct Sampling | Direkte Verwendung einer Surface als Texture |
| Shared GPU Buffer | Gemeinsame GPU-Ressource zwischen Producer und Compositor |
| Buffer Import | Einbindung kompatibler externer Buffer |
| Hardware Overlay | Direkte Darstellung über eine Display-Plane |
| Direct Scanout | Direkte Ausgabe eines geeigneten Buffers |

Die Auswahl MUSS anhand der verfügbaren Hardwarefunktionen und Ressourcenanforderungen erfolgen.

## 5. Buffer Sharing

Anwendungen und Systemdienste DÜRFEN grafische Buffer über kontrollierte Referenzen bereitstellen.

Dabei gilt:

1. Der Producer erstellt oder erhält einen geeigneten Buffer.
2. Die Ressource wird mit definierten Zugriffsrechten freigegeben.
3. Der Compositor importiert die Referenz.
4. Die Verfügbarkeit wird synchronisiert.
5. Der Buffer wird ohne zusätzliche Inhaltskopie verwendet.
6. Nach Abschluss der Nutzung wird die Referenz freigegeben.

Die Freigabe einer Referenz DARF keine noch verwendete Ressource zerstören.

## 6. Resource Compatibility

Zero-Copy DARF nur verwendet werden, wenn die beteiligten Komponenten kompatible Ressourcenformate unterstützen.

Zu prüfen sind insbesondere:

- Pixel- und Farbformat
- Speicherlayout und Modifier
- GPU- und Speicherzugänglichkeit
- Texture- und Sampling-Fähigkeiten
- Synchronisationsmechanismen
- Sicherheits- und Zugriffsrechte

Bei fehlender Kompatibilität MUSS ein alternativer Verarbeitungspfad verfügbar sein.

## 7. Synchronization und Ownership

NovaOS MUSS unzulässige gleichzeitige Zugriffe verhindern.

Dazu gehören:

- Producer-Consumer-Synchronisation
- GPU Fences und Semaphores
- Buffer Ownership
- Resource Lifetime Tracking
- Read/Write Access Control

Ein Producer DARF einen Buffer nicht überschreiben, solange der Compositor oder Display Server diesen unzulässig gleichzeitig verwendet.

Buffer-Freigaben MÜSSEN erst nach Abschluss aller relevanten Zugriffe erfolgen.

## 8. Damage Integration

Zero-Copy Composition MUSS mit Nova Damage Tracking zusammenarbeiten.

Dabei gilt:

- Surface-Inhalte werden nicht allein wegen einer neuen Komposition kopiert.
- Damage Regions bestimmen erforderliche Aktualisierungen.
- Unveränderte Buffer DÜRFEN wiederverwendet werden.
- Inhaltsversionen MÜSSEN berücksichtigt werden.

Zero-Copy DARF nicht dazu führen, dass veraltete Surface-Inhalte dargestellt werden.

## 9. Material Integration

Materialeffekte können zusätzliche Rendering-Ziele erfordern.

Insbesondere bei:

- Liquid Glass
- Acrylic
- Frosted Glass
- Blur
- Refraction
- Reflection

Soweit möglich SOLLEN vorhandene Surface-Buffer direkt als Eingaberessourcen verwendet werden.

Erforderliche Zwischenberechnungen DÜRFEN zusätzliche Buffer verwenden.

Zero-Copy MUSS nicht erzwungen werden, wenn dadurch die Gesamtleistung sinkt oder die Darstellung fehlerhaft wird.

## 10. Hardware Overlays und Direct Scanout

Der Compositor SOLL geeignete Surfaces direkt an den Display Server übergeben können.

Hardware Overlays und Direct Scanout DÜRFEN verwendet werden, wenn:

- Format und Speicherlayout unterstützt werden.
- Skalierung und Transformation kompatibel sind.
- Z-Order und Transparenz korrekt darstellbar sind.
- Keine unberücksichtigten Materialeffekte erforderlich sind.
- Synchronisation und Zugriffsrechte gewährleistet sind.

Die endgültige Entscheidung über Display-Planes und Scanout trifft der Display Server.

## 11. Adaptive Strategy

NovaOS SOLL zwischen Zero-Copy und regulärer Komposition kostenbasiert wählen.

Berücksichtigt werden:

- Speicherbandbreite
- GPU-Sampling-Kosten
- Buffer-Konvertierungen
- Synchronisationsaufwand
- Hardwarefähigkeiten
- Materialeffekte
- Anzahl beteiligter Surfaces

Eine Kopie DARF erfolgen, wenn sie insgesamt effizienter oder technisch erforderlich ist.

## 12. Sicherheit

Gemeinsam verwendete Buffer MÜSSEN durch kontrollierte Ressourcenreferenzen geschützt werden.

Dabei gilt:

- Anwendungen dürfen keine unberechtigten Speicherbereiche freigeben.
- Importierte Buffer müssen validiert werden.
- Zugriffsrechte müssen durchgesetzt werden.
- Speicher darf nicht vorzeitig wiederverwendet werden.
- Freigegebene Ressourcen dürfen keine unzulässigen Datenzugriffe ermöglichen.

Zero-Copy DARF die Prozessisolation von NovaOS nicht umgehen.

## 13. Fallback

Ist Zero-Copy nicht möglich, MUSS NovaOS auf einen kompatiblen Verarbeitungspfad zurückfallen.

Mögliche Verfahren:

1. Direkte GPU-Komposition
2. GPU-interne Kopie oder Formatkonvertierung
3. CPU-basierte Buffer-Konvertierung
4. Software-Komposition

Der Fallback MUSS die grafische Korrektheit und Ressourcenisolation erhalten.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS die direkte Referenzierung grafischer Surface-Buffer unterstützen.
2. Zero-Copy SOLL bei kompatiblen Ressourcen bevorzugt werden.
3. Buffer-Formate und Speicherlayouts MÜSSEN validiert werden.
4. Zugriffsrechte und Ressourcenlebenszeiten MÜSSEN kontrolliert werden.
5. Producer und Consumer MÜSSEN korrekt synchronisiert werden.
6. Unveränderte Buffer SOLLEN ohne Inhaltskopie wiederverwendet werden.
7. Damage Tracking MUSS berücksichtigt werden.
8. Materialeffekte DÜRFEN zusätzliche Zwischenbuffer verwenden.
9. Hardware Overlays und Direct Scanout SOLLEN unterstützt werden.
10. Zero-Copy DARF die Prozessisolation nicht beeinträchtigen.
11. Die Auswahl des Verarbeitungsverfahrens SOLL kostenbasiert erfolgen.
12. Ein kompatibler Fallback MUSS verfügbar sein.
13. Identische Eingaben MÜSSEN im deterministischen Modus reproduzierbare Entscheidungen ermöglichen.

## 15. Ergebnis

Nova Zero-Copy Composition ermöglicht die effiziente Verwendung grafischer Ressourcen ohne unnötige Speichertransfers.

Durch kontrolliertes Buffer Sharing, sichere Synchronisation und hardwareabhängige Optimierungen werden Speicherbandbreite und Darstellungslatenzen reduziert.

Damit unterstützt NovaOS eine ressourcenschonende Desktop-Komposition, ohne die grafische Korrektheit oder Prozessisolation zu beeinträchtigen.