# NPSPEC-BOOTRENDER-0007
# BIOS VBE Backend

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0007 |
| Titel | BIOS VBE Backend |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0005, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRENDER-0005 |
| Zugehörige ADRs | ADR-BOOTUI-0002, ADR-BOOTUI-0003, ADR-BOOTGFX-0001, ADR-BOOTPERF-0002, ADR-BOOTSAFE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **BIOS VBE Backend** der NovaOS Boot Rendering Engine.

Das Backend kapselt sämtliche BIOS- und **VESA BIOS Extensions (VBE)**-spezifischen Funktionen und stellt der Rendering Engine einen einheitlichen Framebuffer zur Verfügung. Alle Firmwaredetails werden vollständig abstrahiert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt ausschließlich für Systeme, die über ein klassisches BIOS starten und einen **VBE Linear Framebuffer (LFB)** bereitstellen.

Nicht Bestandteil dieser Spezifikation sind:

- Rendering Engine
- Scene Graph
- Layer Management
- Compositing
- Animationen
- UEFI GOP

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
BIOS VBE Backend
        │
        ▼
VESA BIOS Extensions
        │
        ▼
Linear Framebuffer
```

---

# 4. Aufgaben

Das BIOS VBE Backend übernimmt folgende Aufgaben:

- Initialisierung des Grafikmodus
- Auswahl eines geeigneten VBE-Modus
- Validierung der VBE-Modusinformationen
- Bereitstellung des Linear Framebuffers
- Erzeugung des Framebuffer Descriptors
- Übergabe an das Framebuffer Backend
- Fehlerbehandlung
- Kernel-Handoff

---

# 5. Voraussetzungen

Das Backend setzt voraus:

- BIOS-System
- VBE 2.0 oder höher
- Linear Framebuffer
- 32-Bit-Farbmodus

Banked Graphics Modes werden nicht unterstützt.

---

# 6. Initialisierung

Die Initialisierung erfolgt in folgender Reihenfolge:

```text
VBE Controller Info
        │
        ▼
Liste der Modi lesen
        │
        ▼
Geeigneten Modus wählen
        │
        ▼
Grafikmodus setzen
        │
        ▼
ModeInfo validieren
        │
        ▼
Framebuffer Descriptor erzeugen
        │
        ▼
Backend Ready
```

---

# 7. Modusauswahl

Das Backend bevorzugt folgende Reihenfolge:

1. 1920 × 1080 × 32
2. 1600 × 900 × 32
3. 1366 × 768 × 32
4. 1280 × 720 × 32
5. 1024 × 768 × 32
6. 800 × 600 × 32

Andere Modi dürfen verwendet werden, wenn sie kompatibel sind.

---

# 8. Mindestanforderungen

Ein Grafikmodus ist nur zulässig, wenn:

- Linear Framebuffer vorhanden
- 32 Bit Farbtiefe
- MemoryModel = Direct Color
- PhysBasePtr gültig
- BytesPerScanLine gültig
- Auflösung größer als Null

---

# 9. Framebuffer Descriptor

Nach erfolgreicher Initialisierung erzeugt das Backend:

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

# 10. VBE ModeInfo

Folgende Felder müssen ausgewertet werden:

- XResolution
- YResolution
- BitsPerPixel
- BytesPerScanLine
- PhysBasePtr
- RedMaskSize
- GreenMaskSize
- BlueMaskSize
- RedFieldPosition
- GreenFieldPosition
- BlueFieldPosition
- MemoryModel

Alle Werte sind vollständig zu validieren.

---

# 11. Unterstützte Pixelformate

Unterstützt werden:

- XRGB8888
- RGB888
- RGB565

Bevorzugt wird ein 32-Bit-Modus.

Nicht unterstützte Formate werden abgelehnt.

---

# 12. Speicherlayout

Die Pixeladresse berechnet sich über:

```text
Framebuffer
+
Y × Pitch
+
X × BytesPerPixel
```

Padding am Zeilenende muss berücksichtigt werden.

---

# 13. Present

Das BIOS Backend unterstützt:

- Full Present
- Partial Present

Das eigentliche Kopieren übernimmt das Framebuffer Backend.

---

# 14. Speicheranforderungen

Es gelten folgende Regeln:

- keine Heap-Allokation im Present
- keine temporären Vollbildpuffer
- deterministische Speicherbelegung
- Pitch berücksichtigen
- keine Schreibzugriffe außerhalb des Framebuffers

---

# 15. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- VBE nicht vorhanden
- Controller Info ungültig
- ModeInfo ungültig
- Linear Framebuffer fehlt
- ungültige PhysBasePtr
- ungültiger Pitch
- ungültige Farbtiefe

Alle Fehler werden an die Boot UI Runtime gemeldet.

---

# 16. Recovery

Kann kein geeigneter VBE-Modus verwendet werden:

```text
VBE Fehler
      │
      ▼
Software Renderer
      │
      ▼
Safe Mode
      │
      ▼
Textmodus
```

Der Bootprozess darf dadurch nicht blockiert werden.

---

# 17. Kernel-Handoff

Vor dem Kernelstart werden übergeben:

- PhysBasePtr
- Auflösung
- Pitch
- BitsPerPixel
- Pixelformat
- Farbmasks
- Firmwaretyp = BIOS

Die Übergabe erfolgt über das NovaOS Boot-Handoff-Protokoll.

---

# 18. API

```c
nova_result_t
nova_boot_vbe_initialize(void);

nova_result_t
nova_boot_vbe_shutdown(void);

nova_result_t
nova_boot_vbe_get_descriptor(
    nova_boot_framebuffer_descriptor_t* descriptor);

nova_result_t
nova_boot_vbe_present(
    const nova_damage_set_t* damage);

bool
nova_boot_vbe_is_available(void);
```

---

# 19. Performance

Anforderungen:

- konstante Laufzeit
- deterministische Speicherbelegung
- keine BIOS-Interrupts im Render-Hotpath
- minimale Speicherzugriffe
- keine unnötigen Kopien

---

# 20. Sicherheit

Das Backend muss verhindern:

- Nullpointerzugriffe
- Integer Overflow
- Out-of-Bounds-Schreibzugriffe
- ungültige PhysBasePtr
- ungültige Pitch-Werte
- ungültige ModeInfo-Strukturen

Firmwaredaten dürfen niemals ungeprüft verwendet werden.

---

# 21. Testfälle

Folgende Tests sind verpflichtend:

- VBE gefunden
- VBE nicht gefunden
- ungültiger Grafikmodus
- ungültige ModeInfo
- verschiedene Auflösungen
- Full Present
- Partial Present
- Kernel-Handoff
- Safe Mode
- Textmodus

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- ausschließlich Linear Framebuffer verwendet wird.
- alle VBE-Daten validiert werden.
- Rendering und BIOS vollständig getrennt sind.
- deterministische Present-Vorgänge erfolgen.
- Fehler nicht zum Abbruch des Bootvorgangs führen.
- das Kernel-Handoff vollständige Grafikdaten enthält.
- keine BIOS-Aufrufe im Rendering erfolgen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTUI-0002 – Einheitliche UI-Architektur für BIOS und UEFI
- ADR-BOOTUI-0003 – Software-Rendering als garantierter Fallback
- ADR-BOOTGFX-0001 – Lineares 32-Bit-RGBA-Farbformat
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTSAFE-0001 – Funktionaler Textmodus als letzter Fallback

## NPSPECs

- NPSPEC-BOOTUI-0005 – BIOS/UEFI Graphics Abstraction
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0005 – Framebuffer Backend
- NPSPEC-BOOTHANDOFF-0001 – Boot Handoff Protocol

---

# 24. Zusammenfassung

Das **BIOS VBE Backend** stellt die standardisierte Anbindung klassischer BIOS-Systeme an die NovaOS Boot Rendering Engine bereit. Es abstrahiert sämtliche VBE-spezifischen Details, validiert alle Firmwaredaten und erzeugt einen einheitlichen Framebuffer Descriptor. Dadurch arbeitet die Rendering Engine unabhängig von der Firmware und kann ohne Änderungen sowohl mit BIOS/VBE- als auch mit UEFI/GOP-Systemen betrieben werden.