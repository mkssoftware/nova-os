# NPSPEC-BOOTPERF-0005
# Adaptive Quality Management

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTPERF-0005 |
| Titel | Adaptive Quality Management |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Performance |
| Priorität | Hoch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTPERF-0001, NPSPEC-BOOTPERF-0002, NPSPEC-BOOTPERF-0003, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTRESOURCE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Adaptive Quality Management (AQM)** der NovaOS Bootoberfläche.

AQM passt die grafische Qualität der Bootoberfläche automatisch an die tatsächlich verfügbare Rechenleistung und den verfügbaren Speicher an, ohne dabei die funktionale Integrität oder Konsistenz der Benutzeroberfläche zu verändern.

Der Benutzer darf keine Funktionsunterschiede feststellen – ausschließlich die visuelle Qualität kann angepasst werden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Rendering Engine
- Motion Engine
- Compositor
- Resource Manager
- Theme Engine
- Text Engine
- Boot Manager
- Recovery
- Installer
- Self-Healing

---

# 3. Ziele

Das Adaptive Quality Management muss:

- Performance automatisch stabilisieren
- konstante Frametimes ermöglichen
- Speicherengpässe vermeiden
- Prioritäten berücksichtigen
- deterministisch arbeiten
- jederzeit deaktivierbar sein

---

# 4. Grundprinzip

Die Bootoberfläche besitzt mehrere Qualitätsstufen.

Nur die Darstellung darf angepasst werden.

Nicht verändert werden dürfen:

- Funktionalität
- Bedienbarkeit
- Layout
- Navigation
- Eingabeverarbeitung
- Sicherheit

---

# 5. Qualitätsstufen

Es existieren folgende Qualitätsprofile:

| Profil | Beschreibung |
|---------|--------------|
| Ultra | Maximale Qualität |
| High | Standardprofil |
| Balanced | Optimierte Qualität |
| Low | Reduzierte Qualität |
| Safe | Minimale grafische Effekte |

---

# 6. Automatische Auswahl

Die Qualitätsstufe wird anhand folgender Werte bestimmt:

- CPU-Leistung
- verfügbare GPU
- freier Arbeitsspeicher
- aktuelle Framezeit
- Cache-Auslastung
- Bildschirmauflösung

Die Auswahl erfolgt ausschließlich beim Start sowie bei definierten Performanceereignissen.

---

# 7. Qualitätsparameter

Folgende Eigenschaften können angepasst werden:

- Schattenqualität
- Blur-Qualität
- Transparenzqualität
- SVG-Rasterauflösung
- Animationstiefe
- Antialiasing
- Verlaufsgüte
- Iconauflösung

---

# 8. Animationen

AQM darf:

- Animationsdauer verkürzen
- Zwischenbilder reduzieren
- Bewegungsunschärfe deaktivieren
- optionale Animationen entfernen

Pflichtanimationen bleiben erhalten.

---

# 9. Blur

Blur besitzt folgende Qualitätsstufen:

- Hoch
- Mittel
- Niedrig
- Deaktiviert

Das Materialsystem bleibt unabhängig davon unverändert.

---

# 10. Schatten

Folgende Anpassungen sind zulässig:

- geringere Unschärfe
- geringere Ebenenzahl
- kleinere Schattenradien
- vollständiges Deaktivieren optionaler Schatten

---

# 11. SVG Rendering

AQM darf:

- Rasterauflösung reduzieren
- Cache aggressiver verwenden
- komplexe Pfade vereinfachen

Die Geometrie bleibt unverändert.

---

# 12. Glyph Rendering

Folgende Optimierungen sind zulässig:

- kleinere Atlasgrößen
- aggressiver Cache
- weniger Skalierungsstufen

Die Lesbarkeit muss erhalten bleiben.

---

# 13. Ressourcenverwaltung

AQM kann:

- Cachegrößen reduzieren
- selten genutzte Ressourcen freigeben
- SVG-Caches verkleinern
- Animationen vorzeitig entfernen

Kritische Ressourcen bleiben dauerhaft reserviert.

---

# 14. Qualitätswechsel

Ein Wechsel erfolgt:

- ohne sichtbares Flackern
- ohne Neustart
- ohne Verlust von UI-Zuständen

Alle Änderungen erfolgen zwischen zwei Frames.

---

# 15. Performanceüberwachung

AQM überwacht:

- FPS
- Framezeit
- CPU-Auslastung
- GPU-Auslastung
- Speichernutzung
- Cache-Auslastung

---

# 16. Auslöser

Ein Qualitätswechsel erfolgt bei:

- dauerhafter Budgetüberschreitung
- Speichermangel
- GPU-Ausfall
- Software-Rendering
- Benutzerkonfiguration

Kurzzeitige Lastspitzen lösen keinen Wechsel aus.

---

# 17. Qualitätsregeln

AQM darf niemals:

- Controls entfernen
- Texte ausblenden
- Layout verändern
- Farben verändern
- Theme wechseln

---

# 18. Benutzersteuerung

Der Benutzer kann wählen:

- Automatisch
- Ultra
- High
- Balanced
- Low
- Safe

Die Einstellung wird dauerhaft gespeichert.

---

# 19. Sicherheit

Qualitätsoptimierungen dürfen niemals:

- Sicherheitsdialoge verändern
- Warnhinweise reduzieren
- Recoveryfunktionen beeinflussen
- kryptographische Informationen verändern

---

# 20. Speicherverwaltung

AQM verwendet:

- Cache Manager
- Quality Manager
- Resource Manager
- Performance Monitor

Heap-Allokationen während eines Qualitätswechsels sind unzulässig.

---

# 21. API

```c
typedef enum
{
    NOVA_BOOT_QUALITY_ULTRA,
    NOVA_BOOT_QUALITY_HIGH,
    NOVA_BOOT_QUALITY_BALANCED,
    NOVA_BOOT_QUALITY_LOW,
    NOVA_BOOT_QUALITY_SAFE,
    NOVA_BOOT_QUALITY_AUTO
}
nova_boot_quality_t;

typedef struct
{
    nova_boot_quality_t current_quality;
    bool automatic;
    bool performance_limited;
}
nova_boot_quality_status_t;

nova_result_t
nova_boot_quality_initialize(void);

nova_result_t
nova_boot_quality_set(
    nova_boot_quality_t quality);

const nova_boot_quality_status_t*
nova_boot_quality_status(void);

nova_result_t
nova_boot_quality_update(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- aktuelle Qualitätsstufe
- Qualitätswechsel
- Auslöser
- FPS
- Framezeit
- Speicherverbrauch
- Cache-Auslastung
- GPU-Status

---

# 23. Testfälle

Zu testen sind:

- Ultra-Profil
- High-Profil
- Balanced-Profil
- Low-Profil
- Safe-Profil
- automatischer Wechsel
- Speicherknappheit
- GPU-Ausfall
- Software Rendering
- Themewechsel

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Qualitätsstufen automatisch gewählt werden können.
- Qualitätswechsel ohne sichtbare Artefakte erfolgen.
- keine Änderungen an Layout oder Funktionalität auftreten.
- alle Performancewerte kontinuierlich überwacht werden.
- die Darstellung auch bei reduzierter Qualität vollständig nutzbar bleibt.
- keine Heap-Allokationen während eines Qualitätswechsels stattfinden.
- Benutzer eine feste Qualitätsstufe auswählen können.
- sämtliche Qualitätsparameter über die Diagnose-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTPERF-0005 – Adaptive Quality Management
- ADR-BOOTRENDER-0001 – Rendering Engine
- ADR-BOOTRESOURCE-0001 – Resource Manager

## NPSPECs

- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTPERF-0002 – Frame-Time Budget
- NPSPEC-BOOTPERF-0003 – Memory Budget
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager

---

# 26. Zusammenfassung

Das **Adaptive Quality Management (AQM)** ermöglicht der NovaOS-Bootoberfläche eine dynamische Anpassung der grafischen Qualität an die verfügbaren Systemressourcen. Durch automatische oder benutzerdefinierte Qualitätsprofile werden Rendering-Effekte, Animationen und Cachegrößen optimiert, ohne Funktionalität, Layout oder Bedienbarkeit zu verändern. Dadurch bleibt die Benutzeroberfläche auch auf leistungsschwächeren Systemen flüssig, deterministisch und vollständig nutzbar, während leistungsfähige Hardware die maximale Darstellungsqualität ausschöpfen kann.