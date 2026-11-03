# NPSPEC-COMPOSITOR-OVERLAY-0001 – Nova Hardware Overlay Integration

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Compositor

## 1. Zweck

Nova Hardware Overlay Integration definiert die Nutzung dedizierter Display-Hardwareebenen zur direkten Darstellung geeigneter Surfaces.

Ziel ist die Entlastung der GPU-Rendering-Pipeline durch die Vermeidung unnötiger Kompositionsoperationen und Speichertransfers.

Geeignete Inhalte SOLLEN direkt über Hardware Overlay Planes dargestellt werden, sofern dies effizient und ohne Einschränkung der grafischen Korrektheit möglich ist.

## 2. Architekturprinzipien

- **Hardware-Aware:** Verfügbare Display-Planes werden berücksichtigt.
- **Zero-Copy:** Geeignete Surface-Buffer werden direkt verwendet.
- **Adaptive:** Die Zuweisung erfolgt anhand von Hardwarefähigkeiten und Kosten.
- **Damage-Aware:** Unveränderte Overlays werden nicht unnötig aktualisiert.
- **Material-Aware:** Transparenz und Effektabhängigkeiten bleiben korrekt.
- **Atomic:** Änderungen mehrerer Display-Planes erfolgen konsistent.
- **Fallback-Safe:** Nicht unterstützte Inhalte werden regulär komponiert.

## 3. Overlay Model

Ein Hardware Overlay beschreibt die direkte Zuordnung einer Surface zu einer Display-Plane.

Die Zuordnung enthält mindestens:

- **Surface ID:** Referenzierte grafische Oberfläche.
- **Buffer Reference:** Darzustellender Buffer.
- **Output ID:** Zugehörige Bildschirmausgabe.
- **Plane ID:** Verwendete Hardwareebene.
- **Source Region:** Quellbereich innerhalb der Surface.
- **Destination Region:** Zielbereich auf der Ausgabe.
- **Z-Order:** Darstellungsreihenfolge.
- **Format:** Pixel- und Speicherformat.
- **Synchronization State:** Verfügbarkeit des Buffers.
- **State Version:** Version der Overlay-Konfiguration.

Die tatsächlichen Hardwarefähigkeiten werden durch den Nova Display Server bereitgestellt.

## 4. Overlay Eligibility

Eine Surface DARF als Hardware Overlay verwendet werden, wenn sämtliche erforderlichen Bedingungen erfüllt sind.

Dazu gehören:

- Unterstütztes Pixel- und Speicherformat
- Kompatible Skalierung und Transformation
- Zulässige Z-Order
- Unterstützte Transparenz
- Gültiger Buffer-Zugriff
- Erfüllte Synchronisationsanforderungen
- Kompatible Farbverarbeitung
- Keine unberücksichtigten Material- oder Backdrop-Abhängigkeiten

Die Eignung MUSS vor jeder neuen Overlay-Zuweisung geprüft werden.

## 5. Overlay Allocation

NovaOS SOLL verfügbare Hardware-Planes anhand der erwarteten Einsparungen zuweisen.

Dabei werden berücksichtigt:

1. Anzahl verfügbarer Planes.
2. Kompatibilität der Surfaces.
3. Größe und Aktualisierungshäufigkeit.
4. Vermiedene Kompositionskosten.
5. Speicherbandbreite.
6. Hardwarebeschränkungen.

Die Zuweisung MUSS als vollständige, hardwareseitig zulässige Konfiguration geprüft werden.

Eine einzeln geeignete Surface garantiert nicht, dass ihre gemeinsame Verwendung mit anderen Overlays möglich ist.

## 6. Composition Integration

Der Compositor MUSS Hardware Overlays in die reguläre Kompositionsplanung integrieren können.

Dabei gilt:

- Zugewiesene Overlay-Surfaces werden nicht unnötig in das Hauptbild gerendert.
- Nicht zugewiesene Surfaces verbleiben in der regulären Komposition.
- Z-Order und Clipping müssen korrekt umgesetzt werden.
- Änderungen der Overlay-Zuweisung müssen betroffene Bildbereiche invalidieren.

Der Display Server entscheidet über die endgültige Programmierung der Hardware-Planes.

## 7. Material und Transparenz

NovaOS MUSS Materialabhängigkeiten vor einer Overlay-Zuweisung berücksichtigen.

Insbesondere Liquid Glass, Acrylic, Blur, Refraction und Reflection können die reguläre Komposition erfordern.

Ein Overlay DARF verwendet werden, wenn seine Darstellung einschließlich aller benötigten Effekte und Hintergrundabhängigkeiten korrekt gewährleistet ist.

Andernfalls MUSS die Surface über den regulären Compositor verarbeitet werden.

## 8. Damage und Synchronization

Overlay-Inhalte MÜSSEN mit Nova Damage Tracking und Nova GPU Synchronization zusammenarbeiten.

Dabei gilt:

- Unveränderte Overlay-Konfigurationen SOLLEN erhalten bleiben.
- Neue Buffer MÜSSEN vor ihrer Verwendung bereitstehen.
- Änderungen an Position, Größe oder Z-Order MÜSSEN berücksichtigt werden.
- Buffer dürfen während aktiver Display-Nutzung nicht unzulässig überschrieben oder freigegeben werden.

Änderungen mehrerer Planes SOLLEN über einen atomaren Display-Commit veröffentlicht werden.

## 9. Adaptive Overlay Management

NovaOS SOLL Overlay-Zuweisungen dynamisch anpassen können.

Eine Neubewertung ist insbesondere erforderlich bei:

- Änderungen der Fensteranordnung
- Änderungen von Surface-Formaten
- Aktivierung von Materialeffekten
- Wechsel der Bildschirmausgabe
- Änderungen der Hardwareverfügbarkeit
- Veränderten Rendering-Kosten

Häufige Wechsel zwischen Overlay und regulärer Komposition SOLLEN durch geeignete Stabilitätsregeln vermieden werden.

## 10. Sicherheit und Fallback

Overlay-Buffer MÜSSEN dieselben Zugriffs- und Isolationsanforderungen erfüllen wie reguläre Surface-Buffer.

Bei fehlgeschlagener Zuweisung MUSS NovaOS auf reguläre Komposition zurückfallen können.

Ein fehlgeschlagener Hardware-Commit DARF nicht als erfolgreich präsentierter Frame behandelt werden.

Der letzte gültige Ausgabezustand SOLL erhalten bleiben, bis eine korrekte neue Konfiguration verfügbar ist.

## 11. Verbindliche Anforderungen

1. NovaOS MUSS Hardware Overlay Planes integrieren können.
2. Der Display Server MUSS verfügbare Plane-Fähigkeiten bereitstellen.
3. Overlay-Zuweisungen MÜSSEN auf Hardwarekompatibilität geprüft werden.
4. Die gemeinsame Plane-Konfiguration MUSS gültig sein.
5. Geeignete Surfaces SOLLEN ohne zusätzliche Komposition dargestellt werden.
6. Z-Order, Transparenz und Farbverarbeitung MÜSSEN korrekt bleiben.
7. Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
8. Buffer-Zugriffe MÜSSEN synchronisiert erfolgen.
9. Plane-Änderungen SOLLEN atomar veröffentlicht werden.
10. Overlay-Zuweisungen SOLLEN kostenbasiert erfolgen.
11. Unnötige Wechsel zwischen Kompositionsverfahren SOLLEN vermieden werden.
12. Ein Fallback auf reguläre Komposition MUSS verfügbar sein.
13. Hardware Overlays DÜRFEN die Prozessisolation nicht umgehen.
14. Im deterministischen Modus MÜSSEN Overlay-Entscheidungen bei identischen Eingaben und Hardwarefähigkeiten reproduzierbar sein.

## 12. Ergebnis

Nova Hardware Overlay Integration ermöglicht die direkte Darstellung geeigneter Surfaces über dedizierte Display-Hardwareebenen.

Durch adaptive Plane-Zuweisung, Zero-Copy und atomare Präsentation werden GPU-Komposition und Speichertransfers reduziert.

Damit kann NovaOS insbesondere bei Videos, Vollbildinhalten und geeigneten Desktop-Surfaces die Darstellungseffizienz verbessern, ohne grafische Korrektheit oder Sicherheit einzuschränken.