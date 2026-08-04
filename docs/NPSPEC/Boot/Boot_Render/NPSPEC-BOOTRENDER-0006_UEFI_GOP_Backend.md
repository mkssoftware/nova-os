# NPSPEC-BOOTRENDER-0006
# UEFI GOP Backend

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0006 |
| Titel | UEFI GOP Backend |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0005, NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0005 |
| Zugehörige ADRs | ADR-BOOTUI-0002, ADR-BOOTGFX-0001, ADR-BOOTPERF-0002, ADR-BOOTSAFE-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **UEFI Graphics Output Protocol (GOP) Backend** der NovaOS Boot Rendering Engine.

Das Backend stellt eine einheitliche Abstraktionsschicht zwischen der Rendering Engine und der UEFI-Firmware bereit. Ziel ist eine deterministische und plattformunabhängige Grafikausgabe ohne direkte Abhängigkeiten der UI-Komponenten vom UEFI Graphics Output Protocol.

---

# 2. Geltungsbereich

Das GOP Backend ist für alle Systeme verantwortlich, die über UEFI mit einem GOP-konformen Grafikmodus gestartet werden.

Es umfasst:

- Initialisierung des GOP-Protokolls
- Auswahl und Validierung des Grafikmodus
- Bereitstellung der Framebuffer-Informationen
- Präsentation der gerenderten Frames
- Fehlerbehandlung
- Übergabe der Framebuffer-Daten an den Kernel

Nicht Bestandteil sind:

- Rendering
- Animationen
- UI-Logik
- Scene Graph
- Compositing

---

# 3. Architektur

```text
Boot UI Runtime
        │
        ▼
 Rendering Engine
        │
        ▼
 Framebuffer Backend
        │
        ▼
 UEFI GOP Backend
        │
        ▼
EFI_GRAPHICS_OUTPUT_PROTOCOL
        │
        ▼
Framebuffer
```

---

# 4. Aufgaben

Das Backend übernimmt folgende Aufgaben:

- GOP-Protokoll ermitteln
- GOP initialisieren
- Grafikmodus auswählen
- Framebuffer validieren
- Pixelformat bestimmen
- Pitch bestimmen
- Framebuffer Descriptor erzeugen
- Present durchführen
- Fehler melden
- Kernel-Handoff vorbereiten

---

# 5. Initialisierung

Die Initialisierung erfolgt in folgender Reihenfolge:

```text
LocateProtocol()
        │
        ▼
GraphicsOutputProtocol gefunden
        │
        ▼
Framebufferinformationen lesen
        │
        ▼
Modus validieren
        │
        ▼
Descriptor erzeugen
        │
        ▼
Backend Ready
```

---

# 6. GOP-Protokoll

Das Backend verwendet ausschließlich das standardisierte UEFI Graphics Output Protocol.

```c
EFI_GRAPHICS_OUTPUT_PROTOCOL
```

Direkte Hardwarezugriffe sind nicht zulässig.

---

# 7. Moduserkennung

Folgende Informationen müssen übernommen werden:

- Auflösung
- PixelsPerScanLine
- PixelFormat
- FrameBufferBase
- FrameBufferSize

Alle Werte sind vor ihrer Verwendung zu validieren.

---

# 8. Unterstützte Pixelformate

Mindestens folgende GOP-Formate werden unterstützt:

- PixelRedGreenBlueReserved8BitPerColor
- PixelBlueGreenRedReserved8BitPerColor
- PixelBitMask

Nicht unterstützte Formate müssen kontrolliert abgelehnt werden.

---

# 9. Framebuffer Descriptor

Nach erfolgreicher Initialisierung erzeugt das Backend einen Framebuffer Descriptor.

```c
typedef struct
{
    void* framebuffer;
    uint64_t framebuffer_size;

    uint32_t width;
    uint32_t height;

    uint32_t pitch;
    uint32_t bytes_per_pixel;

    nova_pixel_format_t pixel_format;
} nova_boot_framebuffer_descriptor_t;
```

---

# 10. Present

Das GOP Backend unterstützt:

- Full Present
- Partial Present

Die eigentliche Present-Logik wird vom Framebuffer Backend ausgeführt.

Das GOP Backend stellt ausschließlich den gültigen Zielbuffer bereit.

---

# 11. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- GOP nicht gefunden
- ungültiger Modus
- ungültige Framebuffer-Adresse
- Framebuffer zu klein
- unbekanntes Pixelformat
- ungültiger Pitch

Alle Fehler müssen an die Boot UI Runtime gemeldet werden.

---

# 12. Recovery

Kann GOP nicht initialisiert werden, erfolgt:

```text
GOP Fehler
      │
      ▼
Software Rendering prüfen
      │
      ▼
Safe Mode
      │
      ▼
Textmodus
```

Ein GOP-Fehler darf den Bootprozess nicht blockieren.

---

# 13. Kernel-Handoff

Vor dem Kernelstart werden folgende Informationen übergeben:

- Framebuffer-Adresse
- Größe
- Auflösung
- Pitch
- Pixelformat
- Farbmasks
- Firmwaretyp

Die Übergabe erfolgt über das NovaOS Boot-Handoff-Protokoll.

---

# 14. Performance

Anforderungen:

- keine Heap-Allokationen im Present
- keine GOP-Aufrufe im Render-Hotpath
- deterministische Laufzeit
- konstante Speicherkomplexität

---

# 15. Sicherheit

Das Backend muss verhindern:

- Nullpointerzugriffe
- Integer Overflow
- Out-of-Bounds-Schreibzugriffe
- ungültige GOP-Strukturen
- ungültige Framebuffergrößen

Firmwaredaten gelten grundsätzlich als nicht vertrauenswürdig und müssen vollständig validiert werden.

---

# 16. API

```c
nova_result_t
nova_boot_gop_initialize(void);

nova_result_t
nova_boot_gop_shutdown(void);

nova_result_t
nova_boot_gop_get_descriptor(
    nova_boot_framebuffer_descriptor_t* descriptor);

nova_result_t
nova_boot_gop_present(
    const nova_damage_set_t* damage);

bool
nova_boot_gop_is_available(void);
```

---

# 17. Testfälle

Zu testen sind mindestens:

- GOP erfolgreich gefunden
- GOP nicht vorhanden
- ungültige Framebuffergröße
- ungültiges Pixelformat
- verschiedene Auflösungen
- Partial Present
- Full Present
- Kernel-Handoff
- Recovery
- Safe Mode

---

# 18. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- GOP auf allen unterstützten UEFI-Systemen erkannt wird.
- Alle Framebufferdaten validiert werden.
- Rendering und GOP vollständig getrennt sind.
- Das Backend deterministisch arbeitet.
- Fehler den Bootprozess nicht blockieren.
- Kernel und Bootmanager identische Framebufferinformationen verwenden.
- Die Rendering Engine keine GOP-spezifischen Daten kennt.

---

# 19. Referenzen

## ADRs

- ADR-BOOTUI-0002 – Einheitliche UI-Architektur für BIOS und UEFI
- ADR-BOOTGFX-0001 – Lineares 32-Bit-RGBA-Farbformat
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren

## NPSPECs

- NPSPEC-BOOTUI-0005 – BIOS/UEFI Graphics Abstraction
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0005 – Framebuffer Backend
- NPSPEC-BOOTHANDOFF-0001 – Boot Handoff Protocol

---

# 20. Zusammenfassung

Das **UEFI GOP Backend** bildet die standardisierte Verbindung zwischen der NovaOS Boot Rendering Engine und dem UEFI Graphics Output Protocol. Es abstrahiert sämtliche GOP-spezifischen Details, stellt validierte Framebufferinformationen bereit und ermöglicht eine sichere, deterministische und plattformunabhängige Grafikausgabe. Die eigentliche Rendering Engine bleibt dadurch vollständig firmwareunabhängig und kann ohne Änderungen sowohl mit BIOS/VBE- als auch mit UEFI/GOP-Backends betrieben werden.