# NPSPEC-BOOTPERF-0004
# Startup-Time Budget

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTPERF-0004 |
| Titel | Startup-Time Budget |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Performance |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot Pipeline |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTPERF-0001, NPSPEC-BOOTPERF-0002, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTMANAGER-UI-0012 |

---

# 1. Ziel

Diese Spezifikation definiert das **Startup-Time Budget** der NovaOS-Bootumgebung.

Sie legt maximale Zeitbudgets für sämtliche Phasen des Systemstarts fest, sodass der Benutzer einen reproduzierbaren, schnellen und transparenten Bootvorgang erlebt. Jede Bootphase besitzt ein eigenes Zeitbudget, das kontinuierlich überwacht wird.

Die Einhaltung der Budgets ist Bestandteil der Qualitätsanforderungen der Bootplattform.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bootloader Stage 1
- Bootloader Stage 2
- Boot Manager
- Recovery
- Self-Healing
- Kernel Loader
- Resource Manager
- UI Engine
- Rendering Engine
- Diagnostics

---

# 3. Ziele

Das Startup-Time Budget muss:

- reproduzierbare Bootzeiten gewährleisten
- langsame Bootphasen erkennen
- Performance regressions verhindern
- Diagnosen ermöglichen
- unterschiedliche Hardwareklassen unterstützen
- deterministisch arbeiten

---

# 4. Bootphasen

Der Bootvorgang wird in folgende Phasen unterteilt:

1. Firmware
2. Bootloader Stage 1
3. Bootloader Stage 2
4. Hardwareinitialisierung
5. Resource Initialisierung
6. UI Initialisierung
7. Boot Manager
8. Kernel Loading
9. Kernel Handoff

Jede Phase besitzt ein eigenes Zeitbudget.

---

# 5. Zielwerte

| Bootphase | Budget |
|-----------|--------:|
| Bootloader Stage 1 | 50 ms |
| Bootloader Stage 2 | 150 ms |
| Grafikinitialisierung | 100 ms |
| Ressourcen laden | 80 ms |
| Theme laden | 20 ms |
| UI Engine | 30 ms |
| Boot Manager darstellen | 50 ms |
| Kernel laden | 300 ms |
| Kernelübergabe | 20 ms |

---

# 6. Gesamtes Budget

Empfohlene Zielwerte:

| Starttyp | Ziel |
|-----------|------:|
| Standardstart | ≤ 1 s |
| Recovery | ≤ 2 s |
| Netzwerk-Boot | abhängig vom Netzwerk |
| Diagnosemodus | ≤ 3 s |

Diese Werte beziehen sich auf die NovaOS-Komponenten und schließen Firmware- oder Hardwareinitialisierungszeiten außerhalb der Kontrolle des Boot Managers nicht ein.

---

# 7. Kaltstart

Beim Kaltstart gelten:

- vollständige Hardwareerkennung
- vollständiger Ressourcenaufbau
- vollständige Themeinitialisierung

Caches dürfen nicht vorausgesetzt werden.

---

# 8. Warmstart

Beim Warmstart dürfen verwendet werden:

- Firmwareinformationen
- vorbereitete Speicherbereiche
- Bootkonfiguration
- bereits validierte Datenstrukturen

Nicht zulässig ist die Wiederverwendung flüchtiger UI-Zustände.

---

# 9. Ressourceninitialisierung

Folgende Komponenten müssen parallel vorbereitet werden:

- Theme
- Fonts
- Icons
- SVG
- Lokalisierungen
- Animationen

Die Initialisierung endet vor der ersten Darstellung.

---

# 10. UI-Start

Zeitbudget:

```text
≤ 50 ms
```

Die erste vollständige Benutzeroberfläche muss innerhalb dieses Zeitfensters erscheinen.

---

# 11. Boot Manager

Nach der Initialisierung müssen verfügbar sein:

- Bootliste
- Hintergrund
- Theme
- Eingabesystem
- Countdown
- Statusinformationen

---

# 12. Kernel Loading

Während des Kernel-Ladens dürfen erfolgen:

- Fortschrittsanzeige
- Statusmeldungen
- Diagnosedaten
- Animationen

Es dürfen keine blockierenden UI-Operationen auftreten.

---

# 13. Verzögerungen

Zulässige Verzögerungen:

- Benutzer-Countdown
- Passwortabfrage
- Recoveryauswahl
- Netzwerkkommunikation

Nicht zulässig:

- unnötige Wartezeiten
- künstliche Animationen
- blockierende Initialisierung

---

# 14. Parallelisierung

Parallel ausführbar:

- Ressourcenladen
- Fontinitialisierung
- SVG-Rasterisierung
- Themeaufbau
- Diagnosesammlung

Die Bootreihenfolge bleibt dennoch deterministisch.

---

# 15. Überwachung

Für jede Bootphase werden erfasst:

- Startzeit
- Endzeit
- Dauer
- Budget
- Überschreitung
- Ursache

---

# 16. Budgetverletzungen

Bei Überschreitung werden:

1. Diagnoseeintrag erzeugt
2. Phase markiert
3. Dauer gespeichert
4. Optimierungshinweis erstellt

Die Überschreitung verhindert den Bootvorgang nicht automatisch.

---

# 17. Performanceklassen

| Klasse | Gesamtdauer |
|---------|------------:|
| A | ≤ 1,0 s |
| B | ≤ 1,5 s |
| C | ≤ 2,0 s |
| D | ≤ 3,0 s |
| E | > 3,0 s |

Klasse A stellt das Ziel für moderne Systeme dar.

---

# 18. Hardwareklassen

Das Budget berücksichtigt:

- Embedded-Systeme
- Desktop-PCs
- Workstations
- Virtuelle Maschinen

Die Budgetüberwachung erfolgt unabhängig von der Hardwareklasse.

---

# 19. Sicherheit

Zeitoptimierungen dürfen niemals:

- Integritätsprüfungen überspringen
- Signaturprüfungen deaktivieren
- TPM-Prüfungen umgehen
- Recoverymechanismen auslassen

Sicherheitsprüfungen besitzen Vorrang vor Performance.

---

# 20. Speicherverwaltung

Während des Bootvorgangs gelten:

- vorallokierte Objektpools
- deterministische Speicherverwaltung
- keine unnötigen Heap-Allokationen
- Ressourcen-Caching

---

# 21. API

```c
typedef enum
{
    NOVA_BOOT_PHASE_STAGE1,
    NOVA_BOOT_PHASE_STAGE2,
    NOVA_BOOT_PHASE_GRAPHICS,
    NOVA_BOOT_PHASE_RESOURCES,
    NOVA_BOOT_PHASE_UI,
    NOVA_BOOT_PHASE_MANAGER,
    NOVA_BOOT_PHASE_KERNEL,
    NOVA_BOOT_PHASE_HANDOFF
}
nova_boot_phase_t;

typedef struct
{
    nova_boot_phase_t phase;
    uint64_t start_time_us;
    uint64_t end_time_us;
    uint64_t duration_us;
    uint64_t budget_us;
    bool exceeded;
}
nova_boot_time_budget_t;

nova_result_t
nova_boot_budget_initialize(void);

void
nova_boot_budget_begin(
    nova_boot_phase_t phase);

void
nova_boot_budget_end(
    nova_boot_phase_t phase);

const nova_boot_time_budget_t*
nova_boot_budget_status(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Bootphase
- Startzeit
- Endzeit
- Dauer
- Budget
- Überschreitungen
- Gesamtstartzeit
- Hardwareklasse

---

# 23. Testfälle

Zu testen sind:

- Kaltstart
- Warmstart
- Recovery
- Netzwerk-Boot
- Installer
- Themewechsel
- langsame Datenträger
- virtuelle Maschinen
- Speicherknappheit
- Budgetüberschreitungen

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Bootphase ein definiertes Zeitbudget besitzt.
- sämtliche Phasen einzeln überwacht werden.
- Budgetüberschreitungen erkannt und protokolliert werden.
- die erste vollständige Bootoberfläche innerhalb von maximal **50 ms** nach Abschluss der UI-Initialisierung erscheint.
- der Standardstart auf Referenzhardware innerhalb des definierten Zielbudgets abgeschlossen wird.
- keine künstlichen Verzögerungen eingefügt werden.
- sicherheitsrelevante Prüfungen trotz Performanceoptimierung vollständig ausgeführt werden.
- alle Zeitdaten über die Diagnose-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTPERF-0004 – Startup-Time Budget
- ADR-BOOTPERF-0001 – Performance Requirements
- ADR-BOOTRESOURCE-0001 – Resource Manager
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTPERF-0002 – Frame-Time Budget
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTMANAGER-UI-0012 – Boot Progress Screen

---

# 26. Zusammenfassung

Das **Startup-Time Budget** definiert die maximal zulässigen Ausführungszeiten sämtlicher Phasen der NovaOS-Bootpipeline – von der Initialisierung des Bootloaders bis zur Übergabe an den Kernel. Durch klar definierte Zeitbudgets, kontinuierliche Überwachung und Diagnosemöglichkeiten werden Performanceprobleme frühzeitig erkannt und reproduzierbare Bootzeiten sichergestellt. Sicherheitskritische Prüfungen besitzen dabei stets Vorrang vor Zeitoptimierungen, sodass ein schneller und gleichzeitig vertrauenswürdiger Systemstart gewährleistet wird.