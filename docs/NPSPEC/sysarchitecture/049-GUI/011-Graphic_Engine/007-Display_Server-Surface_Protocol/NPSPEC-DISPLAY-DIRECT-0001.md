# NPSPEC-DISPLAY-DIRECT-0001 – Nova Direct Scanout

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Direct Scanout definiert die direkte Ausgabe eines geeigneten Surface-Buffers über die Display-Hardware ohne vorherige Komposition in einen separaten Ausgabebuffer.

Ziel ist die Reduzierung von GPU-Auslastung, Speicherbandbreite, Energieverbrauch und Darstellungslatenz.

Direct Scanout SOLL bevorzugt werden, wenn die direkte Ausgabe technisch möglich und wirtschaftlich sinnvoll ist.

## 2. Architekturprinzipien

- **Zero-Copy:** Direkte Verwendung vorhandener Grafikbuffer.
- **Composition Bypass:** Vermeidung unnötiger Kompositionsdurchläufe.
- **Capability-Based:** Nutzung entsprechend den Hardwarefähigkeiten.
- **Synchronization-Aware:** Sichere Übergabe zwischen Producer und Display-Hardware.
- **Atomic:** Konsistente Umschaltung des Scanout-Buffers.
- **Adaptive:** Dynamischer Wechsel zwischen Direct Scanout und regulärer Komposition.
- **Fallback-Safe:** Sichere Rückkehr zur normalen Grafikpipeline.

## 3. Direct Scanout Model

Eine Direct-Scanout-Konfiguration enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Output ID | Zielbildschirm |
| Surface ID | Darzustellende Surface |
| Buffer Reference | Direkt verwendeter Grafikbuffer |
| Pixel Format | Pixelformat |
| Memory Layout | Strides, Offsets und Modifier |
| Source Region | Quellbereich |
| Destination Region | Ausgabebereich |
| Acquire Fence | Synchronisation des Buffer-Inhalts |
| State Version | Version der Konfiguration |

Die Display-Hardware liest den geeigneten Buffer direkt aus, ohne dass der Compositor dessen Inhalt zuvor in einen separaten Framebuffer kopieren muss.

## 4. Scanout Eligibility

Direct Scanout DARF nur aktiviert werden, wenn sämtliche erforderlichen Bedingungen erfüllt sind.

Dazu gehören:

1. Die Surface deckt den benötigten Ausgabebereich vollständig ab.
2. Pixel- und Speicherformat werden unterstützt.
3. Skalierung und Transformation sind hardwareseitig möglich.
4. Der Buffer ist für Scanout geeignet.
5. Die Synchronisation ist gewährleistet.
6. Farbverarbeitung und Transparenz sind kompatibel.
7. Keine erforderlichen Kompositionseffekte werden umgangen.

Nicht erfüllte Bedingungen MÜSSEN zur regulären Komposition oder einer anderen gültigen Hardware-Plane-Konfiguration führen.

## 5. Composition Bypass

Bei aktiviertem Direct Scanout DARF der Compositor die reguläre Komposition der betreffenden Ausgabe überspringen.

Dabei gilt:

- Die Surface wird unmittelbar als Scanout-Quelle verwendet.
- Unnötige Render Passes werden vermieden.
- Unveränderte Buffer bleiben weiterhin gültig.
- Die logische Surface- und Fensterverwaltung bleibt aktiv.
- Der Compositor behält die Kontrolle über die Auswahl des darzustellenden Inhalts.

Direct Scanout verändert nicht die Eigentumsverhältnisse oder Berechtigungen einer Surface.

## 6. Synchronization und Buffer Lifetime

NovaOS MUSS Direct Scanout mit Nova Surface Synchronization verbinden.

Dabei gilt:

- Acquire Fences MÜSSEN vor dem hardwareseitigen Lesen berücksichtigt werden.
- Buffer DÜRFEN während aktiver Scanout-Verwendung nicht unzulässig verändert werden.
- Die Freigabe erfolgt erst nach dem tatsächlichen Ende der Hardwareverwendung.
- Buffer-Wechsel MÜSSEN konsistent erfolgen.

Ein erfolgreicher Surface Commit bedeutet nicht automatisch, dass der Buffer bereits für Direct Scanout verfügbar ist.

## 7. Atomic Scanout Switching

Der Wechsel zwischen Scanout-Buffern SOLL atomar erfolgen.

Dies betrifft:

- Wechsel des dargestellten Buffers
- Aktivierung oder Deaktivierung von Direct Scanout
- Änderungen der Scanout-Konfiguration
- Übergänge zur regulären Komposition

Der Display Server MUSS die neue Konfiguration vor der Übernahme validieren.

Ein fehlgeschlagener Wechsel DARF keine teilweise übernommene Ausgabe erzeugen.

## 8. Damage und Frame Scheduling

Direct Scanout MUSS mit Nova Damage Tracking und Nova Frame Scheduling zusammenarbeiten.

Dabei gilt:

- Neue Surface-Inhalte können ohne reguläre Komposition präsentiert werden.
- Unveränderte Inhalte erfordern keinen neuen Scanout-Buffer.
- Damage Regions können für unterstützte Teilaktualisierungen verwendet werden.
- Die Display-Hardware kann den bestehenden Buffer weiterhin periodisch auslesen.

Direct Scanout verhindert unnötige Kompositionsarbeit, bedeutet jedoch nicht, dass die physische Bildschirmausgabe vollständig angehalten wird.

## 9. Material und Overlay Integration

Direct Scanout DARF keine erforderlichen Materialeffekte umgehen.

Insbesondere müssen berücksichtigt werden:

- Liquid Glass
- Acrylic
- Backdrop Blur
- Refraction
- Reflection
- Transparenz
- Überlagernde Fenster und Systemelemente

Sind zusätzliche sichtbare Inhalte erforderlich, kann NovaOS Hardware Overlays verwenden.

Sind diese nicht geeignet, MUSS reguläre Komposition erfolgen.

## 10. Adaptive Direct Scanout

NovaOS SOLL Direct Scanout dynamisch aktivieren und deaktivieren können.

Typische Einsatzbereiche:

- Vollbildanwendungen
- Videowiedergabe
- Spiele
- Geeignete Desktop-Surfaces
- Statische Bildschirminhalte

Die Auswahl SOLL berücksichtigen:

- GPU-Kompositionskosten
- Speicherbandbreite
- Darstellungslatenz
- Hardwarefähigkeiten
- Häufigkeit von Zustandswechseln

Häufige Wechsel zwischen Direct Scanout und regulärer Komposition SOLLEN vermieden werden.

## 11. Multi-Display Support

Direct Scanout MUSS für jede Bildschirmausgabe unabhängig verwaltet werden können.

Dabei gilt:

- Jede Ausgabe besitzt eigene Scanout-Fähigkeiten.
- Unterschiedliche Auflösungen und Bildwiederholraten werden berücksichtigt.
- Gemeinsame Buffer dürfen bei kompatibler Hardware mehrfach verwendet werden.
- Synchronisation und Ressourcenlebensdauer müssen sämtliche Ausgaben berücksichtigen.

Die Aktivierung von Direct Scanout auf einer Ausgabe DARF andere Ausgaben nicht unnötig beeinträchtigen.

## 12. Sicherheit und Fallback

Direct Scanout MUSS die NovaOS-Sicherheitsarchitektur einhalten.

Insbesondere:

- Buffer-Zugriffe MÜSSEN autorisiert sein.
- Speicherbereiche MÜSSEN validiert werden.
- Fremde Speicherinhalte DÜRFEN nicht sichtbar werden.
- Geschützte Inhalte MÜSSEN entsprechend ihren Sicherheitsanforderungen behandelt werden.
- Fehlerhafte Scanout-Konfigurationen MÜSSEN abgewiesen werden.

Bei fehlender Hardwareunterstützung oder ungültiger Konfiguration MUSS NovaOS auf reguläre Komposition zurückfallen können.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Direct Scanout als optionalen Präsentationspfad unterstützen können.
2. Scanout-Fähigkeiten MÜSSEN durch den Display Server ermittelt werden.
3. Buffer-Formate und Speicherlayouts MÜSSEN validiert werden.
4. Direct Scanout DARF nur bei vollständiger grafischer Korrektheit aktiviert werden.
5. Unnötige Kompositionsoperationen SOLLEN vermieden werden.
6. Buffer-Zugriffe MÜSSEN synchronisiert sein.
7. Aktive Scanout-Buffer DÜRFEN nicht vorzeitig freigegeben werden.
8. Buffer- und Konfigurationswechsel SOLLEN atomar erfolgen.
9. Material- und Overlay-Abhängigkeiten MÜSSEN berücksichtigt werden.
10. Direct Scanout MUSS mit Frame Scheduling zusammenarbeiten.
11. Mehrere Bildschirmausgaben MÜSSEN unabhängig unterstützt werden können.
12. Die Aktivierung SOLL kostenbasiert erfolgen.
13. Häufige unnötige Moduswechsel SOLLEN vermieden werden.
14. Ein sicherer Fallback auf reguläre Komposition MUSS verfügbar sein.
15. Direct Scanout DARF die Prozess- und Speicherisolation nicht umgehen.
16. Im deterministischen Modus MÜSSEN Scanout-Entscheidungen bei identischen Eingaben und Hardwarefähigkeiten reproduzierbar sein.

## 14. Ergebnis

Nova Direct Scanout ermöglicht die unmittelbare Darstellung geeigneter Surface-Buffer über die Display-Hardware.

Durch den Verzicht auf unnötige Kompositionsdurchläufe werden GPU-Arbeit, Speichertransfers und Darstellungslatenz reduziert.

Damit ergänzt Direct Scanout die inkrementelle Grafikarchitektur von NovaOS um einen besonders effizienten Präsentationspfad für geeignete Vollbild- und Desktop-Inhalte.