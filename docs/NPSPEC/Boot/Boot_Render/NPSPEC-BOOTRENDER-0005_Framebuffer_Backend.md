# NPSPEC-BOOTRENDER-0005
# Framebuffer Backend

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRENDER-0005 |
| Titel | Framebuffer Backend |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Rendering |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot Rendering Engine |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRENDER-0002, NPSPEC-BOOTRENDER-0003, NPSPEC-BOOTRENDER-0004, NPSPEC-BOOT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Framebuffer Backend** der NovaOS Boot Rendering Engine.

Das Framebuffer Backend bildet die niedrigste Abstraktionsschicht der Rendering Engine und kapselt sämtliche Zugriffe auf den physischen Bildspeicher. Es stellt eine einheitliche API für BIOS- und UEFI-basierte Framebuffer bereit und entkoppelt die höheren Rendering-Komponenten vollständig von firmware- oder hardwareabhängigen Implementierungsdetails.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- BIOS VBE Framebuffer
- UEFI GOP Framebuffer
- Software Renderer
- Compositor
- Rendering Engine
- Double Buffering
- Boot Manager
- Recovery
- Installer

---

# 3. Ziele

Das Backend muss:

- BIOS und UEFI abstrahieren
- eine einheitliche API bereitstellen
- deterministisch arbeiten
- hardwareunabhängig sein
- hohe Performance liefern
- zukünftige Backends unterstützen

---

# 4. Architektur

Das Backend besteht aus:

- Backend Interface
- BIOS Adapter
- UEFI Adapter
- Memory Mapper
- Presentation Layer
- Backend Diagnostics

Die oberen Render-Schichten kommunizieren ausschließlich mit dem Backend Interface.

---

# 5. Backendmodell

```text
Rendering Engine

↓

Compositor

↓

Framebuffer Backend

↓

BIOS VBE
oder
UEFI GOP

↓

Physischer Framebuffer
```

---

# 6. Aufgaben

Das Backend übernimmt:

- Initialisierung
- Framebuffer-Mapping
- Pixelausgabe
- Buffer-Präsentation
- Auflösungsinformationen
- Farbinformationen
- Backenddiagnose

---

# 7. Unterstützte Backends

Aktuell:

- BIOS VBE
- UEFI GOP

Geplant:

- Virtuelles Backend
- Test Backend
- Remote Backend

---

# 8. Initialisierung

Während der Initialisierung werden ermittelt:

- Auflösung
- Farbtiefe
- Pixelformat
- Pitch
- Speicheradresse
- Gesamtgröße

Die Initialisierung erfolgt genau einmal.

---

# 9. Framebufferinformationen

Für jedes Backend werden bereitgestellt:

- Width
- Height
- Pitch
- Bits per Pixel
- Pixelformat
- Base Address
- Framebuffergröße

---

# 10. Pixelzugriffe

Unterstützt werden:

- Pixel schreiben
- Pixel lesen
- horizontale Linien
- vertikale Linien
- Rechtecke
- Blockkopien

Alle Zugriffe erfolgen über die Backend-API.

---

# 11. Präsentation

Das Backend unterstützt:

- Single Buffer
- Double Buffer

Triple Buffering ist für spätere Versionen vorgesehen.

---

# 12. Pixelformate

Mindestens:

- XRGB8888
- ARGB8888
- RGB565

Weitere Formate können ergänzt werden.

---

# 13. Speicherlayout

Das Backend berücksichtigt:

- Pitch
- Alignment
- Zeilenabstände
- Speichergrenzen

Direkte Annahmen über zusammenhängende Pixelzeilen sind unzulässig.

---

# 14. Speicherzugriffe

Optimierungen:

- sequenzielle Schreibzugriffe
- Cachefreundliche Reihenfolge
- Blockkopien
- Dirty Regions

---

# 15. Fehlerbehandlung

Zu erkennen sind:

- ungültige Framebufferadresse
- ungültiger Pitch
- unbekanntes Pixelformat
- Mappingfehler
- Präsentationsfehler

---

# 16. Synchronisation

Das Backend garantiert:

- atomare Bufferwechsel
- konsistente Framebufferzustände
- vollständige Bildausgabe

---

# 17. Performance

Das Backend muss:

- minimale Kopieroperationen verwenden
- unnötige Speicherzugriffe vermeiden
- Dirty Regions unterstützen
- deterministische Laufzeiten besitzen

---

# 18. Sicherheit

Das Backend darf:

- ausschließlich den zugewiesenen Framebufferbereich beschreiben
- niemals außerhalb des gemappten Speichers schreiben
- keine Firmwaredaten überschreiben

Alle Speichergrenzen werden geprüft.

---

# 19. Diagnostik

Bereitgestellt werden:

- Backendtyp
- Auflösung
- Pixelformat
- Pitch
- Speicheradresse
- Present Count
- Frame Count
- Fehlerstatus

---

# 20. Speicherverwaltung

Das Backend reserviert:

- Front Buffer
- Back Buffer
- Statusbereich
- Diagnosestrukturen

Dynamische Speicherallokationen während des Renderbetriebs sind nicht zulässig.

---

# 21. API

```c
typedef enum
{
    NOVA_FB_BACKEND_VBE,
    NOVA_FB_BACKEND_GOP
}
nova_fb_backend_type_t;

typedef enum
{
    NOVA_PIXEL_XRGB8888,
    NOVA_PIXEL_ARGB8888,
    NOVA_PIXEL_RGB565
}
nova_pixel_format_t;

typedef struct
{
    nova_fb_backend_type_t backend;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bits_per_pixel;
    nova_pixel_format_t format;
    void* framebuffer;
    size_t framebuffer_size;
}
nova_framebuffer_info_t;

nova_result_t
nova_framebuffer_initialize(void);

const nova_framebuffer_info_t*
nova_framebuffer_info(void);

nova_result_t
nova_framebuffer_present(void);

void
nova_framebuffer_shutdown(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Backendtyp
- Auflösung
- Pitch
- Pixelformat
- Speichergröße
- Present Count
- Frame Count
- Fehler
- Initialisierungsdauer

---

# 23. Testfälle

Zu testen sind:

- BIOS VBE
- UEFI GOP
- Single Buffer
- Double Buffer
- verschiedene Pixelformate
- verschiedene Auflösungen
- Mappingfehler
- ungültige Framebuffer
- Dirty Regions
- Bufferwechsel

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- BIOS- und UEFI-Framebuffer vollständig abstrahiert werden.
- sämtliche Renderzugriffe ausschließlich über das Backend erfolgen.
- alle unterstützten Pixelformate korrekt verarbeitet werden.
- Single- und Double-Buffering fehlerfrei funktionieren.
- keine Speicherzugriffe außerhalb des Framebuffers erfolgen.
- deterministische Laufzeiten eingehalten werden.
- sämtliche Diagnosedaten über die Backend-API verfügbar sind.
- zukünftige Backendtypen ohne Änderungen an höheren Render-Schichten integrierbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTRENDER-0005 – Framebuffer Backend
- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTRENDER-0002 – Render Command Queue
- NPSPEC-BOOTRENDER-0003 – Render Pipeline
- NPSPEC-BOOTRENDER-0004 – Surface Management
- NPSPEC-BOOT-0001 – BIOS and UEFI Support

---

# 26. Zusammenfassung

Das **Framebuffer Backend** bildet die Hardware-Abstraktionsschicht der NovaOS Boot Rendering Engine. Es vereinheitlicht den Zugriff auf BIOS-VBE- und UEFI-GOP-Framebuffer, stellt konsistente Informationen über Auflösung, Pixelformat und Speicherlayout bereit und übernimmt die sichere Präsentation gerenderter Bilder. Durch eine klar definierte Backend-API, deterministische Speicherverwaltung und vollständige Entkopplung der höheren Rendering-Komponenten schafft es die Grundlage für eine portable, erweiterbare und leistungsfähige Bootgrafikarchitektur.