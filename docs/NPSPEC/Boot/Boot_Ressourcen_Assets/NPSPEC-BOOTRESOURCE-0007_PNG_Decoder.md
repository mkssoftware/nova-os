# NPSPEC-BOOTRESOURCE-0007
# PNG Decoder

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0007 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTRESOURCE-0006, NPSPEC-BOOTRENDER-0016, NPSPEC-BOOTMEMORY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **PNG Decoder** der NovaOS-Bootoberfläche.

Der PNG Decoder verarbeitet PNG-Bilddateien innerhalb des Boot Managers und stellt sie dem Rendering-System als standardisierte Bitmap-Ressourcen zur Verfügung. Dabei werden alle notwendigen Prüfungen, Dekomprimierungen und Konvertierungen durchgeführt.

Der Decoder arbeitet vollständig unabhängig von BIOS, UEFI und der zugrunde liegenden Grafikhardware.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Logos
- Hintergrundbilder
- Icons
- Illustrationen
- Splash-Grafiken
- Dialoggrafiken
- UI-Grafiken
- Theme-Ressourcen

---

# 3. Ziele

Der PNG Decoder muss:

- PNG vollständig unterstützen
- Alpha-Kanäle verarbeiten
- Integrität prüfen
- deterministisch arbeiten
- geringe Speicheranforderungen besitzen
- zukünftige Erweiterungen ermöglichen

---

# 4. Architektur

```text
PNG-Datei

↓

Resource Loader

↓

PNG Decoder

├── Header Reader
├── Chunk Parser
├── CRC Validator
├── Inflater
├── Filter Processor
└── Pixel Converter

↓

Bitmap

↓

Renderer
```

---

# 5. Komponenten

Der PNG Decoder besteht aus:

- PNG Header Reader
- Chunk Parser
- CRC Validator
- Inflate Engine
- Filter Processor
- Pixel Converter
- Bitmap Builder

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
PNG

↓

Header

↓

Chunks

↓

CRC

↓

Inflate

↓

Filter

↓

RGBA Bitmap

↓

Renderer
```

Die Bitmap wird vollständig erzeugt, bevor sie registriert wird.

---

# 7. Unterstützte PNG-Version

Unterstützt wird:

- PNG Version 1.2

Kompatible Dateien neuerer Versionen können verarbeitet werden, sofern keine unbekannten Pflichtfunktionen verwendet werden.

---

# 8. Unterstützte Farbtypen

Pflicht:

- Graustufen
- RGB
- RGBA
- Indexed Color

Optional:

- Graustufen mit Alpha

Nicht unterstützte Farbtypen werden abgelehnt.

---

# 9. Bit-Tiefen

Unterstützt werden:

- 1 Bit
- 2 Bit
- 4 Bit
- 8 Bit
- 16 Bit (optional)

Intern erfolgt die Konvertierung auf:

```text
32 Bit RGBA
```

---

# 10. PNG-Chunks

Pflicht:

- IHDR
- IDAT
- IEND

Optional:

- PLTE
- tRNS
- gAMA
- pHYs
- sRGB
- tEXt

Unbekannte optionale Chunks werden übersprungen.

Unbekannte Pflicht-Chunks führen zum Abbruch.

---

# 11. Dekomprimierung

Die IDAT-Daten werden mittels Deflate verarbeitet.

Der Decoder muss:

- mehrere IDAT-Chunks unterstützen
- Datenströme korrekt zusammenführen
- beschädigte Daten erkennen

---

# 12. Filter

Unterstützt werden sämtliche PNG-Filter:

- None
- Sub
- Up
- Average
- Paeth

Die Filter werden zeilenweise angewendet.

---

# 13. Transparenz

Unterstützt werden:

- vollständiger Alpha-Kanal
- tRNS-Transparenz
- RGBA

Das Ergebnis wird immer als RGBA-Bitmap ausgegeben.

---

# 14. Integritätsprüfung

Vor der Registrierung werden geprüft:

- PNG-Signatur
- Header
- CRC sämtlicher Chunks
- Dateigröße
- Chunk-Reihenfolge

Beschädigte Dateien werden verworfen.

---

# 15. Fehlerbehandlung

Bei Fehlern:

```text
PNG lesen

↓

Fehler

↓

Default Image

↓

Fehler protokollieren
```

Die Oberfläche bleibt funktionsfähig.

---

# 16. Sicherheit

Das System verhindert:

- manipulierte Chunks
- CRC-Fehler
- Buffer Overflows
- Integerüberläufe
- beschädigte Inflate-Daten
- ungültige Bildgrößen

Alle Daten werden vor der Verarbeitung validiert.

---

# 17. Speicherverwaltung

Der PNG Decoder verwendet:

- statische Zeilenpuffer
- Bitmap-Pools
- Objektpools

Streaming-Dekodierung wird bevorzugt.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(n)-Dekodierung
- O(1)-Chunk-Lookup
- O(n)-Filterberechnung
- deterministische Laufzeit

Die Laufzeit wächst ausschließlich linear mit der Anzahl der Bildpixel.

---

# 19. API

```c
typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    nova_pixel_format_t format;
    void* pixels;
}
nova_bitmap_t;

nova_result_t
nova_png_initialize(void);

nova_result_t
nova_png_decode(
    const void* source,
    size_t source_size,
    nova_bitmap_t* bitmap);

bool
nova_png_validate(
    const void* source,
    size_t source_size);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Bildgröße
- Farbtyp
- Bit-Tiefe
- Anzahl Chunks
- Dekodierungszeit
- CRC-Fehler
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- RGB
- RGBA
- Indexed Color
- Graustufen
- Alpha-Kanal
- PLTE
- mehrere IDAT-Chunks
- beschädigte CRC
- ungültige Header
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten PNG-Dateien korrekt dekodiert werden.
- sämtliche PNG-Filter korrekt verarbeitet werden.
- Transparenz vollständig unterstützt wird.
- CRC-Fehler erkannt werden.
- beschädigte Dateien sicher verworfen werden.
- das Ergebnis immer als RGBA-Bitmap bereitgestellt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0007 – PNG Decoder
- ADR-BOOTRESOURCE-0004 – Resource Loading
- ADR-BOOTRESOURCE-0006 – Resource Integrity Verification
- ADR-BOOTRENDER-0016 – Image Rendering

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTRESOURCE-0006 – Resource Integrity Verification
- NPSPEC-BOOTRENDER-0016 – Image Rendering
- NPSPEC-BOOTMEMORY-0001 – Boot Memory Management

---

# 24. Zusammenfassung

Der **PNG Decoder** definiert die standardisierte Verarbeitung von PNG-Bilddateien innerhalb der NovaOS-Bootoberfläche. Er unterstützt die vollständige Dekodierung von PNG-Dateien einschließlich Deflate-Dekomprimierung, Filterverarbeitung, Transparenz und Integritätsprüfung. Durch Streaming-Dekodierung, deterministische Algorithmen, statische Speicherverwaltung und die Ausgabe einer standardisierten RGBA-Bitmap stellt der Decoder eine sichere und performante Bildverarbeitung für BIOS- und UEFI-basierte Bootumgebungen bereit.