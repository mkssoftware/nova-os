# ADR-BOOT-1016: Framebuffer-Abstraktion statt direkter VBE-Nutzung (Teil 1)

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1016
* **Titel:** Framebuffer-Abstraktion statt direkter VBE-Nutzung
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture / Graphics
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** NovaOS Bootloader, Kernel, Grafiksystem und Treiberarchitektur
* **Verantwortlich:** Nova Platform Architecture
* **Ersetzt:** Keine
* **Ersetzt durch:** Keine

---

# Referenzen

* ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* ADR-BOOT-1015 – 64-Bit als langfristige Zielarchitektur
* NPSPEC-KERNELENTRY-0001
* NPSPEC-BOOTHANDOFF-0001
* NPSPEC-GRAPHICS-0001 *(geplant)*
* NPSPEC-WINDOW-0001
* NPSPEC-SHELL-0001

---

# 1. Kontext

NovaOS besitzt einen eigenen Bootloader.

Bereits während des Bootvorgangs soll eine moderne grafische Oberfläche verfügbar sein.

Diese Oberfläche umfasst unter anderem:

* Bootlogo
* Ladeanimation
* Bootmenü
* Recovery
* Diagnose
* Bootfehlerbildschirm
* Fortschrittsanzeigen

Dazu benötigt der Bootloader einen Grafikmodus.

Unter BIOS wird hierfür typischerweise:

```text
VESA BIOS Extensions (VBE)
```

verwendet.

Unter UEFI dagegen:

```text
Graphics Output Protocol (GOP)
```

Nach dem Start des Kernels soll dieselbe grafische Oberfläche möglichst ohne sichtbaren Wechsel weiterverwendet werden.

---

# 2. Problemstellung

Es muss entschieden werden, ob Kernel und Anwendungen direkt auf:

* VBE
* GOP

zugreifen oder ob eine eigene Abstraktionsschicht eingeführt wird.

Ein direkter Zugriff hätte zunächst geringeren Entwicklungsaufwand.

Langfristig entstehen jedoch erhebliche Nachteile:

* BIOS-Abhängigkeit
* UEFI-Abhängigkeit
* unterschiedliche APIs
* unterschiedliche Initialisierung
* erschwerte Treiberentwicklung
* schwierige Portierung auf ARM
* schwierige Portierung auf RISC-V
* keine einheitliche Grafikarchitektur

---

# 3. Entscheidung

NovaOS führt eine eigene Framebuffer-Abstraktion ein.

Der Bootloader verwendet:

```text
BIOS

↓

VBE

↓

Framebuffer

↓

Nova Framebuffer API
```

bzw.

```text
UEFI

↓

GOP

↓

Framebuffer

↓

Nova Framebuffer API
```

Ab dem Kernelstart greifen sämtliche Komponenten ausschließlich auf die Nova-Framebuffer-Abstraktion zu.

Direkte Zugriffe auf VBE oder GOP außerhalb des Bootloaders sind nicht zulässig.

---

# 4. Ziele

Die Entscheidung verfolgt folgende Ziele:

* Firmwareunabhängigkeit
* Hardwareunabhängigkeit
* einheitliche API
* austauschbare Grafiktreiber
* einfachere Portierung
* stabilere Kernelarchitektur
* moderne Rendering-Pipeline
* zukünftige GPU-Unterstützung

---

# 5. Grundprinzip

VBE oder GOP sind ausschließlich Initialisierungstechnologien.

Sie sind **nicht** Bestandteil der dauerhaften Grafikarchitektur.

Sie liefern lediglich:

* Framebufferadresse
* Auflösung
* Pixelformat
* Zeilenlänge

Ab diesem Zeitpunkt übernimmt ausschließlich NovaOS die Kontrolle.

---

# 6. Architektur

Die Architektur lautet:

```text
Firmware

↓

VBE oder GOP

↓

Linearer Framebuffer

↓

Nova Bootloader

↓

Nova Framebuffer

↓

Renderer

↓

Window Manager

↓

Desktop
```

---

# 7. Verantwortlichkeiten

## VBE / GOP

Aufgaben:

* Grafikmodus wählen
* Framebuffer bereitstellen
* Pixelformat liefern

Nicht verantwortlich für:

* Fenster
* Schrift
* Icons
* Animationen
* Compositing
* Transparenz

---

# 8. Aufgaben der Nova-Framebuffer-Abstraktion

Die Abstraktion übernimmt:

* Framebufferverwaltung
* Pixelformat
* Clipping
* Zeichenoperationen
* Synchronisation
* Bufferwechsel
* Renderziele
* spätere GPU-Anbindung

---

# 9. Kein direkter Firmwarezugriff

Kernelmodule dürfen niemals:

```text
INT 10h

VBE Functions

UEFI GOP Services
```

direkt verwenden.

Nach dem Boot existiert ausschließlich die Nova-Grafikschicht.

---

# 10. Warum?

Firmwaredienste sind nicht für ein modernes Betriebssystem entwickelt.

Sie besitzen Einschränkungen:

* geringe Geschwindigkeit
* unterschiedliche Implementierungen
* keine Hardwarebeschleunigung
* keine moderne Speicherverwaltung
* keine Thread-Sicherheit

Sie eignen sich ausschließlich zum Initialisieren.

---

# 11. Firmwareunabhängigkeit

Der Kernel darf nicht erkennen müssen, ob der Framebuffer ursprünglich stammt von:

```text
BIOS

oder

UEFI
```

Für sämtliche Kernelkomponenten existiert nur:

```text
Nova Framebuffer
```

---

# 12. Framebufferbeschreibung

Die Bootloader-Übergabe enthält mindestens:

```text
Framebuffer Address

Width

Height

Pitch

Bits Per Pixel

Pixel Format

Memory Size
```

---

# 13. Beispielstruktur

```c
typedef struct
{
    uint64_t framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t pixel_format;
} nova_framebuffer_t;
```

Diese Struktur ist Bestandteil des Nova Boot-Handoff.

---

# 14. Keine VBE-Strukturen im Kernel

Folgende VBE-Datenstrukturen dürfen den Kernel nicht erreichen:

* VBE Controller Info
* Mode Info Block
* BIOS Register

Sie werden bereits im Bootloader in die Nova-Struktur übersetzt.

---

# 15. Keine GOP-Strukturen im Kernel

Ebenso dürfen keine nativen GOP-Strukturen verwendet werden.

Der Kernel kennt ausschließlich:

```text
nova_framebuffer_t
```

---

# 16. Unterstützte Pixelformate

Mindestens:

```text
RGB888

BGR888

ARGB8888

XRGB8888
```

Weitere Formate können ergänzt werden.

---

# 17. Interne Normalisierung

Unterschiedliche Firmwareformate werden intern vereinheitlicht.

Beispiel:

```text
VBE

↓

BGR

↓

Nova Internal Format

↓

Renderer
```

---

# 18. Framebufferobjekt

Der Kernel behandelt den Framebuffer als normales Kernelobjekt.

Eigenschaften:

* Referenzzählung
* Reflection
* Capability-System
* Rechteverwaltung

---

# 19. Framebuffer ist kein Gerät

Der Framebuffer ist eine Ressource.

Nicht:

* Grafikkarte
* Treiber
* Monitor

Er beschreibt lediglich einen Speicherbereich.

---

# 20. Renderer

Der Software-Renderer arbeitet ausschließlich mit:

```text
nova_framebuffer_t
```

Er kennt keine Firmware.

---

# 21. Zeichenprimitive

Die Framebuffer-API stellt mindestens bereit:

* PutPixel
* DrawLine
* DrawRect
* FillRect
* Blit
* Copy
* Clear

---

# 22. Textausgabe

Schrift wird vollständig durch NovaOS erzeugt.

Nicht durch:

* BIOS
* VBE
* GOP

---

# 23. Icons

Icons werden ebenfalls durch NovaOS gerendert.

Nicht durch Firmwarefunktionen.

---

# 24. Animationen

Animationen greifen ausschließlich auf die Framebuffer-API zu.

Beispiele:

* Bootlogo
* Spinner
* Ladebalken
* Transparenzeffekte

---

# 25. Fenster

Der Window Manager kennt ausschließlich Render Targets.

Nicht:

* VBE
* GOP

---

# 26. Mehrere Renderziele

Die API unterstützt später:

* Front Buffer
* Back Buffer
* Offscreen Buffer
* Texture Buffer

ohne Änderungen an Anwendungen.

---

# 27. Double Buffering

Die Abstraktion unterstützt:

```text
Front Buffer

↓

Back Buffer

↓

Swap
```

---

# 28. Triple Buffering

Optional:

```text
Front

Middle

Back
```

---

# 29. Dirty Rectangles

Die API unterstützt später:

```text
Dirty Regions
```

zur Reduzierung unnötiger Speicherzugriffe.

---

# 30. Skalierung

Skalierung erfolgt innerhalb der Nova-Grafikbibliothek.

Nicht durch Firmware.

---

# 31. Rotation

Bildschirmrotation ist Bestandteil der Abstraktion.

---

# 32. Clipping

Alle Zeichenoperationen unterstützen Clipping.

---

# 33. Alpha-Blending

Die Grafikbibliothek unterstützt Alpha-Blending unabhängig von VBE oder GOP.

---

# 34. Transparenz

Glaseffekte werden vollständig im Renderer erzeugt.

Nicht durch Firmware.

---

# 35. Farbverwaltung

Farbräume werden innerhalb der Nova-Grafikbibliothek verwaltet.

---

# 36. Gamma

Spätere Gamma-Unterstützung erfolgt über Grafikkartentreiber.

Nicht über die Boot-Firmware.

---

# 37. Cursor

Der Softwarecursor verwendet dieselbe Framebuffer-API.

---

# 38. Bootlogo

Das Bootlogo wird bereits mit der Nova-Framebuffer-Abstraktion gezeichnet.

Dadurch kann dieselbe Renderingbibliothek später im Kernel weiterverwendet werden.

---

# 39. Bootmenü

Auch das Bootmenü verwendet ausschließlich die Nova-Framebuffer-Abstraktion.

---

# 40. Zusammenfassung Teil 1

Die Firmware (VBE oder GOP) dient ausschließlich dazu, einen initialen linearen Framebuffer bereitzustellen. Ab dem Übergang zum Kernel existiert nur noch die Nova-Framebuffer-Abstraktion. Dadurch bleiben alle höheren Grafikkomponenten unabhängig von Firmware, Plattform und späteren Grafikkartentreibern.
---
# ADR-BOOT-1016: Framebuffer-Abstraktion statt direkter VBE-Nutzung (Teil 2)

---

# 41. Hardwarebeschleunigung

Die Framebuffer-Abstraktion muss so entworfen werden, dass sie später vollständig hardwarebeschleunigte Grafiktreiber unterstützen kann.

Der Software-Renderer bleibt dabei unverändert.

Die eigentliche Implementierung der Zeichenoperationen kann später auf:

* GPU
* Display Controller
* 2D-Beschleuniger
* 3D-Renderer

delegiert werden.

---

# 42. Backend-Prinzip

Die Nova-Framebuffer-API trennt Schnittstelle und Implementierung.

```text id="4eqcgo"
Window Manager

↓

Nova Graphics API

↓

Framebuffer Backend

↓

Software

oder

GPU

oder

VirtIO GPU

oder

VMware SVGA

oder

Intel

oder

AMD

oder

NVIDIA
```

Alle höheren Ebenen bleiben unverändert.

---

# 43. Software Renderer

Der erste Renderer arbeitet vollständig im Hauptspeicher.

Eigenschaften:

* deterministisch
* einfach testbar
* unabhängig von Grafikhardware
* ideal für frühe Kernelentwicklung
* identisches Verhalten auf allen Plattformen

---

# 44. GPU-Renderer

Spätere GPU-Treiber ersetzen ausschließlich das Backend.

Die öffentliche API bleibt identisch.

Dadurch müssen Anwendungen nicht angepasst werden.

---

# 45. Render Pipeline

Die langfristige Pipeline lautet:

```text id="5w9iuz"
Application

↓

Widget Toolkit

↓

Window Manager

↓

Compositor

↓

Nova Graphics API

↓

Framebuffer Backend

↓

GPU

↓

Monitor
```

---

# 46. Boot-Handoff

Der Bootloader übergibt ausschließlich standardisierte Framebufferinformationen.

Nicht übergeben werden:

* VBE-Datenstrukturen
* GOP-Protokollobjekte
* BIOS-Zustände
* Firmwarehandles

---

# 47. Handoff-Struktur

Die Übergabestruktur enthält mindestens:

```text id="5b4yqa"
Magic

Framebuffer Address

Width

Height

Pitch

Bits Per Pixel

Pixel Format

Framebuffer Size
```

Spätere Erweiterungen dürfen zusätzliche Felder ergänzen.

---

# 48. Keine Firmware-Handles

Firmwareinterne Zeiger dürfen niemals Bestandteil des Kernel-Handoffs sein.

Beispiele:

* EFI_HANDLE
* GOP Interface Pointer
* BIOS Interrupt Tabellen

---

# 49. Lebensdauer

Der Bootloader übergibt lediglich die Eigenschaften des Framebuffers.

Nach erfolgreichem Kernelstart besitzt ausschließlich der Kernel die Kontrolle über diesen Speicherbereich.

---

# 50. Speicherverwaltung

Der Framebuffer wird in das Kernel-Speichermanagement integriert.

Eigenschaften:

* reservierter Speicherbereich
* nicht pageable
* referenzierbar
* Kernelobjekt

---

# 51. Virtueller Speicher

Langfristig wird der Framebuffer zusätzlich in den virtuellen Adressraum eingebunden.

Dadurch kann jede Kernelkomponente über dieselbe virtuelle Adresse auf den Bildspeicher zugreifen.

---

# 52. Memory Mapping

Die physische Framebufferadresse wird während der Initialisierung gemappt.

```text id="zjlwm9"
Physical Framebuffer

↓

Virtual Mapping

↓

Nova Graphics API
```

---

# 53. Mehrere Monitore

Die Architektur unterstützt später mehrere Framebuffer gleichzeitig.

Beispiele:

```text id="gk1t3x"
Display 1

Display 2

Display 3
```

Jeder Bildschirm erhält ein eigenes Framebufferobjekt.

---

# 54. Hotplug

Neue Displays können später dynamisch registriert werden.

Die API bleibt unverändert.

---

# 55. Displayobjekte

Ein Display wird als Kernelobjekt modelliert.

Eigenschaften:

* ID
* Auflösung
* Rotation
* Skalierung
* Framebuffer
* Status

---

# 56. Framebufferobjekte

Mehrere Framebuffer dürfen einem Display zugeordnet sein.

Beispiele:

* Front Buffer
* Back Buffer
* Capture Buffer
* Debug Buffer

---

# 57. Compositor

Der Compositor arbeitet ausschließlich mit Render Targets.

Er kennt weder:

* BIOS
* VBE
* GOP

---

# 58. Fensterverwaltung

Fenster zeichnen niemals direkt in den physikalischen Framebuffer.

Sie rendern zunächst in Offscreen-Puffer.

Erst der Compositor erzeugt das endgültige Bild.

---

# 59. Boot Splash

Der Boot Splash verwendet dieselbe Renderingbibliothek wie später der Desktop.

Dadurch entfällt ein Wechsel zwischen verschiedenen Grafiksystemen.

---

# 60. Panic Screen

Auch der Kernel-Panic-Bildschirm verwendet ausschließlich die Nova-Framebuffer-API.

Dies ermöglicht:

* identische Schriftarten
* identische Icons
* identisches Layout
* konsistente Fehlerdarstellung

---

# 61. Recovery

Das Recovery-System verwendet dieselbe Grafikbibliothek.

Eine spezielle Firmwareoberfläche ist nicht erforderlich.

---

# 62. Safe Mode

Auch der abgesicherte Modus verwendet die Framebuffer-Abstraktion.

Ein GPU-Treiber ist hierfür nicht erforderlich.

---

# 63. Virtuelle Maschinen

Virtuelle Grafikkarten erscheinen ausschließlich als Backend.

Beispiele:

* VirtIO GPU
* VMware SVGA
* VirtualBox VGA
* Bochs Graphics Adapter

Die API bleibt identisch.

---

# 64. Remote Desktop

Ein zukünftiger Remote-Desktop-Treiber kann ebenfalls als Backend implementiert werden.

Dadurch wird kein spezieller Grafikmodus benötigt.

---

# 65. Bildschirmaufnahme

Screen Recording kann direkt auf Render Targets zugreifen.

Nicht auf Firmwarefunktionen.

---

# 66. Screenshots

Screenshots werden durch Kopieren des Framebuffers erstellt.

Keine Firmwareunterstützung erforderlich.

---

# 67. Druckausgabe

Spätere Drucksysteme können dieselben Render Targets verwenden.

---

# 68
