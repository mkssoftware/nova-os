# NPSPEC-BOOTDIAG-0001
# Boot UI Diagnostics

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDIAG-0001 |
| Titel | Boot UI Diagnostics |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Diagnostics |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTPERF-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTMANAGER-UI-0014, NPSPEC-BOOTERROR-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Boot UI Diagnostics Framework** der NovaOS-Bootumgebung.

Das Framework dient der Erfassung, Speicherung, Analyse und Darstellung sämtlicher Diagnoseinformationen der grafischen Bootoberfläche. Es ermöglicht eine vollständige Nachvollziehbarkeit des Bootvorgangs und unterstützt Fehleranalyse, Performanceoptimierung sowie Systemvalidierung.

Das Diagnosesystem arbeitet vollständig lokal und unabhängig vom installierten Betriebssystem.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bootloader
- Boot Manager
- Rendering Engine
- Compositor
- Motion Engine
- Resource Manager
- Text Engine
- Theme Engine
- Recovery
- Self-Healing
- Installer
- Netzwerk-Boot

---

# 3. Ziele

Das Diagnosesystem muss:

- sämtliche relevanten Bootinformationen erfassen
- Performance überwachen
- Fehler eindeutig protokollieren
- reproduzierbare Diagnosen ermöglichen
- modular erweiterbar sein
- deterministisch arbeiten

---

# 4. Architektur

Das Framework besteht aus:

- Event Collector
- Performance Monitor
- Log Manager
- Error Collector
- Diagnostic Storage
- Export Service
- UI Integration
- Diagnostics API

Alle Komponenten arbeiten unabhängig voneinander.

---

# 5. Diagnosebereiche

Folgende Bereiche werden unterstützt:

- Bootstatus
- Hardware
- Firmware
- Rendering
- Speicher
- Performance
- Ressourcen
- Animationen
- Eingaben
- Netzwerk
- Sicherheit

Neue Module können zusätzliche Diagnosebereiche registrieren.

---

# 6. Diagnoseereignisse

Erfasst werden:

- Initialisierung
- Statusänderungen
- Fehler
- Warnungen
- Benutzeraktionen
- Recovery-Ereignisse
- Performancewarnungen
- Ressourcenänderungen

Jedes Ereignis besitzt eine eindeutige Ereignis-ID.

---

# 7. Ereignisstruktur

Jedes Ereignis enthält:

- Ereignis-ID
- Zeitstempel
- Kategorie
- Schweregrad
- Quelle
- Beschreibung
- optionale Metadaten

---

# 8. Schweregrade

Folgende Schweregrade werden verwendet:

| Stufe | Beschreibung |
|--------|--------------|
| Trace | Detailinformationen |
| Debug | Entwicklerdiagnose |
| Information | Normaler Ablauf |
| Warning | Warnung |
| Error | Fehler |
| Critical | Kritischer Fehler |

---

# 9. Performanceüberwachung

Permanent überwacht werden:

- FPS
- Framezeit
- CPU-Auslastung
- GPU-Auslastung
- Renderzeit
- Layoutzeit
- Eingabelatenz
- Speicherverbrauch
- Cache-Auslastung

Die Performanceüberwachung erfolgt kontinuierlich während der gesamten Bootoberfläche.

---

# 10. Ressourcenüberwachung

Zu erfassen sind:

- Resource Cache
- Glyph Cache
- SVG Cache
- Theme Cache
- Draw Command Buffer
- Objektpools

---

# 11. Speicherdiagnose

Überwacht werden:

- belegter Speicher
- freier Speicher
- Spitzenverbrauch
- Fragmentierung
- Cache-Evictions
- Budgetüberschreitungen

---

# 12. Fehlerdiagnose

Für jeden Fehler werden gespeichert:

- Fehlercode
- Fehlerklasse
- betroffenes Modul
- Bootphase
- Ursache
- empfohlene Maßnahme

Die Struktur orientiert sich an standardisierten Boot- und Firmware-Fehlerprotokollen, damit Fehler systematisch ausgewertet werden können. :contentReference[oaicite:0]{index=0}

---

# 13. Bootphasen

Die Diagnose unterscheidet:

- Firmware
- Stage 1
- Stage 2
- Hardwareinitialisierung
- Ressourceninitialisierung
- UI Initialisierung
- Boot Manager
- Kernel Loading
- Kernel Handoff

---

# 14. Diagnoseprotokoll

Das Protokoll wird chronologisch geführt.

Jeder Eintrag besitzt:

- Sequenznummer
- Zeitstempel
- Kategorie
- Schweregrad
- Beschreibung

---

# 15. Filter

Unterstützte Filter:

- Zeitraum
- Modul
- Schweregrad
- Kategorie
- Bootphase
- Fehlerklasse

---

# 16. Export

Unterstützte Exportformate:

- Nova Diagnostic Format (NDF)
- JSON
- Plain Text
- Binärformat

Der Export erfolgt ausschließlich auf ausdrückliche Benutzeraktion.

---

# 17. Live-Diagnose

Während des Bootvorgangs können angezeigt werden:

- FPS
- Framezeit
- Speicherverbrauch
- Bootphase
- Renderzeit
- Ressourcenstatus

Die Live-Anzeige ist standardmäßig deaktiviert.

---

# 18. Datenschutz

Diagnosedaten enthalten standardmäßig keine:

- Passwörter
- Recovery-Schlüssel
- kryptographischen Schlüssel
- TPM-Geheimnisse
- Benutzerdaten

---

# 19. Sicherheit

Das Diagnosesystem darf:

- keine Integritätsprüfungen beeinflussen
- keine Recoveryfunktionen verändern
- keine Bootentscheidungen beeinflussen
- keine vertraulichen Daten offenlegen

Diagnoseinformationen sind ausschließlich lesend.

---

# 20. Speicherverwaltung

Das Diagnosesystem verwendet:

- Event Pool
- Log Pool
- Diagnostics Cache
- Ring Buffer

Die maximale Größe des Diagnosepuffers ist konfigurierbar.

---

# 21. API

```c
typedef enum
{
    NOVA_DIAG_TRACE,
    NOVA_DIAG_DEBUG,
    NOVA_DIAG_INFO,
    NOVA_DIAG_WARNING,
    NOVA_DIAG_ERROR,
    NOVA_DIAG_CRITICAL
}
nova_diag_level_t;

typedef struct
{
    uint64_t timestamp_us;
    nova_diag_level_t level;
    uint32_t category;
    uint32_t event_id;
    const char* module;
    const char* message;
}
nova_diag_event_t;

nova_result_t
nova_diag_initialize(void);

nova_result_t
nova_diag_log(
    const nova_diag_event_t* event);

const nova_diag_event_t*
nova_diag_get(
    uint32_t index);

nova_result_t
nova_diag_export(void);

void
nova_diag_reset(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Bootdauer
- Bootphasen
- Framezeiten
- Speicherverbrauch
- CPU-Auslastung
- GPU-Auslastung
- Cache-Auslastung
- Benutzeraktionen
- Fehler
- Warnungen
- Ressourcenstatus

---

# 23. Testfälle

Zu testen sind:

- vollständiger Boot
- Recovery
- Self-Healing
- Themewechsel
- Performanceüberwachung
- Speicherdiagnose
- Fehlerprotokollierung
- Export
- Live-Diagnose
- Ring-Buffer-Überlauf

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Diagnoseereignisse strukturiert protokolliert werden.
- Performance-, Speicher- und Fehlerdaten kontinuierlich erfasst werden.
- Ereignisse nach Kategorie und Schweregrad filterbar sind.
- Diagnosedaten exportiert werden können.
- keine vertraulichen Informationen gespeichert werden.
- das Diagnosesystem den Bootvorgang nicht messbar verzögert.
- alle Daten über die Diagnostics API verfügbar sind.
- sämtliche Diagnosemodule modular erweitert werden können.

---

# 25. Referenzen

## ADRs

- ADR-BOOTDIAG-0001 – Boot UI Diagnostics
- ADR-BOOTPERF-0001 – Performance Requirements
- ADR-BOOTRESOURCE-0001 – Resource Manager
- ADR-BOOTERROR-0001 – Error Framework

## NPSPECs

- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTMANAGER-UI-0014 – Boot Diagnostics Screen
- NPSPEC-BOOTERROR-0001 – Boot Error Framework

---

# 26. Zusammenfassung

Das **Boot UI Diagnostics Framework** bildet die zentrale Diagnoseinfrastruktur der NovaOS-Bootoberfläche. Es sammelt Performance-, Speicher-, Ressourcen- und Fehlerdaten während aller Bootphasen, stellt diese strukturiert über eine einheitliche API bereit und unterstützt Export-, Filter- und Live-Diagnosefunktionen. Durch den modularen Aufbau, deterministische Speicherverwaltung und die Trennung von Diagnose und Bootlogik ermöglicht das Framework eine umfassende Analyse des Bootvorgangs, ohne dessen Stabilität oder Sicherheit zu beeinträchtigen. Die Trennung von Bootereignissen, Fehlerprotokollen und Diagnoseinformationen orientiert sich an etablierten Konzepten moderner Boot- und Firmwareumgebungen. :contentReference[oaicite:1]{index=1}