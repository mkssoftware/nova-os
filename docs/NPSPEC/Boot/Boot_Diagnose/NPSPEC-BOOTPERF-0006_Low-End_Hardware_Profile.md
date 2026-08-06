# NPSPEC-BOOTPERF-0006
# Low-End Hardware Profile

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTPERF-0006 |
| Titel | Low-End Hardware Profile |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Performance |
| Priorität | Hoch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTPERF-0001, NPSPEC-BOOTPERF-0002, NPSPEC-BOOTPERF-0003, NPSPEC-BOOTPERF-0005, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRESOURCE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Low-End Hardware Profile (LEHP)** der NovaOS Bootoberfläche.

Das Profil stellt sicher, dass sämtliche Bootfunktionen auch auf leistungsschwachen oder älteren Systemen zuverlässig, deterministisch und mit konsistenter Benutzererfahrung ausgeführt werden können.

Das Profil reduziert ausschließlich grafische Qualität und Ressourcenverbrauch. Bedienung, Sicherheit und Funktionalität bleiben vollständig erhalten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- BIOS-Systeme
- UEFI-Systeme
- Embedded-Systeme
- virtuelle Maschinen
- ältere Desktop-PCs
- industrielle Systeme
- Systeme ohne GPU-Beschleunigung

---

# 3. Ziele

Das Low-End Hardware Profile muss:

- auf geringer Hardware flüssig arbeiten
- Speicherverbrauch minimieren
- CPU-Auslastung reduzieren
- GPU-Unabhängigkeit gewährleisten
- deterministische Laufzeiten besitzen
- automatisch aktivierbar sein

---

# 4. Zielhardware

Typische Zielsysteme:

| Komponente | Zielwert |
|------------|----------|
| CPU | 1 Kern ≥ 1 GHz |
| RAM | ≥ 512 MiB |
| verfügbarer RAM für Boot UI | ≥ 32 MiB |
| Grafik | VESA/GOP Framebuffer |
| GPU | optional |
| Massenspeicher | HDD / SATA SSD / USB |

Das Profil muss auch auf moderner Hardware manuell aktivierbar sein.

---

# 5. Aktivierung

Das Profil wird aktiviert durch:

- automatische Hardwareerkennung
- Benutzereinstellung
- Recoverymodus
- Safe Mode
- Diagnosemodus (optional)

---

# 6. Rendering

Verwendet wird:

- Software Rendering
- Dirty Region Rendering
- Double Buffering
- feste Renderpipeline

Nicht verwendet werden:

- komplexe Shader
- GPU-spezifische Effekte

---

# 7. Qualitätsstufe

Standardprofil:

```text
Low
```

Alternativ:

```text
Safe
```

Das Profil basiert auf dem Adaptive Quality Management.

---

# 8. Animationen

Folgende Einschränkungen gelten:

- reduzierte Animationsdauer
- reduzierte Zwischenbilder
- keine optionalen Übergänge
- keine Hintergrundanimationen

Pflichtanimationen bleiben erhalten.

---

# 9. Glasmaterial

Das Glass Material wird vereinfacht:

- geringe Transparenz
- statischer Blur
- optional vollständig deaktiviert

Das visuelle Erscheinungsbild bleibt konsistent.

---

# 10. Schatten

Verwendet werden:

- vereinfachte Schatten
- geringe Weichzeichnung
- maximal eine Schattenebene

---

# 11. SVG

SVG-Rendering:

- geringere Rasterauflösung
- aggressiver Cache
- Wiederverwendung gerenderter Varianten

---

# 12. Textdarstellung

Verwendet werden:

- Glyph Atlas
- Glyph Cache
- vereinfachtes Hinting
- begrenzte Skalierungsstufen

Die Lesbarkeit besitzt höchste Priorität.

---

# 13. Ressourcen

Nicht benötigte Ressourcen werden:

- verzögert geladen
- nach Bedarf geladen
- früher freigegeben

Vorab geladen werden ausschließlich kritische Ressourcen.

---

# 14. Speicherbudget

Empfohlen:

| Bereich | Budget |
|----------|--------:|
| Gesamt | 32 MiB |
| Resource Cache | 6 MiB |
| Glyph Cache | 2 MiB |
| SVG Cache | 2 MiB |
| Theme Cache | 512 KiB |
| Animationen | 512 KiB |
| Controls | 2 MiB |
| Reserve | verbleibend |

---

# 15. CPU-Nutzung

Im Leerlauf:

- ausschließlich ereignisgesteuert
- keine Polling-Schleifen
- keine permanenten Animationen

---

# 16. Auflösung

Optimiert für:

- 800×600
- 1024×768
- 1280×720

Größere Auflösungen werden ebenfalls unterstützt.

---

# 17. Eingabe

Volle Unterstützung für:

- Tastatur
- Maus
- Touch (falls vorhanden)

Die Eingabelatenz bleibt unabhängig vom Profil.

---

# 18. Performanceziele

| Parameter | Ziel |
|-----------|------:|
| FPS | ≥ 30 |
| Framezeit | ≤ 33 ms |
| UI Initialisierung | ≤ 80 ms |
| erster vollständiger Bildschirm | ≤ 150 ms |

---

# 19. Sicherheit

Das Profil darf niemals:

- Integritätsprüfungen deaktivieren
- Signaturprüfungen umgehen
- Recoveryfunktionen einschränken
- Sicherheitsdialoge vereinfachen

Alle Sicherheitsfunktionen bleiben vollständig aktiv.

---

# 20. Speicherverwaltung

Es gelten:

- vorallokierte Objektpools
- keine Heap-Allokationen während der UI-Laufzeit
- aggressives Cache-Management
- deterministische Speicherverwaltung

---

# 21. API

```c
typedef struct
{
    bool enabled;
    bool software_renderer;
    bool reduced_effects;
    bool reduced_animations;
    uint32_t target_fps;
}
nova_low_end_profile_t;

nova_result_t
nova_low_end_profile_initialize(void);

nova_result_t
nova_low_end_profile_enable(void);

nova_result_t
nova_low_end_profile_disable(void);

const nova_low_end_profile_t*
nova_low_end_profile_status(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Profilstatus
- Hardwareklasse
- Software Rendering
- FPS
- Speicherverbrauch
- Cache-Auslastung
- Framezeit
- reduzierte Effekte

---

# 23. Testfälle

Zu testen sind:

- BIOS-System
- UEFI-System
- 512 MiB RAM
- Software Rendering
- virtuelle Maschine
- HDD
- Themewechsel
- Recoverymodus
- Speicherknappheit
- reduzierte Animationen

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- das Low-End Hardware Profile automatisch aktiviert werden kann.
- sämtliche Bootfunktionen auch ohne GPU-Beschleunigung vollständig verfügbar sind.
- mindestens **30 FPS** auf der definierten Zielhardware erreicht werden.
- das Speicherbudget von **32 MiB** eingehalten wird.
- ausschließlich grafische Qualität reduziert wird.
- Bedienbarkeit, Layout und Sicherheit unverändert bleiben.
- keine Heap-Allokationen während der regulären UI-Laufzeit stattfinden.
- sämtliche Profilinformationen über die Diagnose-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTPERF-0006 – Low-End Hardware Profile
- ADR-BOOTPERF-0005 – Adaptive Quality Management
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTPERF-0002 – Frame-Time Budget
- NPSPEC-BOOTPERF-0003 – Memory Budget
- NPSPEC-BOOTPERF-0005 – Adaptive Quality Management
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager

---

# 26. Zusammenfassung

Das **Low-End Hardware Profile** definiert ein optimiertes Ausführungsprofil für Systeme mit begrenzten Hardware-Ressourcen. Durch vereinfachte grafische Effekte, reduzierte Animationen, aggressives Cache-Management und Software-Rendering wird eine flüssige und deterministische Bootoberfläche mit mindestens 30 FPS erreicht. Sämtliche Funktionen, Sicherheitsmechanismen und Bedienkonzepte bleiben unverändert, sodass auch ältere oder eingebettete Systeme eine vollständig konsistente NovaOS-Booterfahrung bieten.