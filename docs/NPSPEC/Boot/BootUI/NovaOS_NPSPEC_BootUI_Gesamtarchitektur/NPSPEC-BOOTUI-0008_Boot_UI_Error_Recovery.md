# NPSPEC-BOOTUI-0008
# Boot UI Error Recovery

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0008 |
| Titel | Boot UI Error Recovery |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0007 |
| ADRs | ADR-BOOTSAFE-0001 bis ADR-BOOTSAFE-0005, ADR-BOOTPERF-0004 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige Fehlerbehandlungs- und Wiederherstellungssystem (Error Recovery System) der NovaOS Boot-UI.

Das Ziel besteht darin, dass **kein Fehler innerhalb der grafischen Benutzeroberfläche jemals den Bootvorgang verhindert.**

Jeder Fehler muss:

- erkannt werden
- klassifiziert werden
- protokolliert werden
- isoliert werden
- automatisch behandelt werden
- falls notwendig auf einen Fallback wechseln
- den Bootprozess möglichst ohne Benutzerinteraktion fortsetzen

Die Boot-UI ist **nicht kritisch für das Starten des Betriebssystems**.

Der Kernelstart besitzt grundsätzlich eine höhere Priorität als jede grafische Funktion.

---

# 2. Architekturziele

Das Error-Recovery-System verfolgt folgende Ziele.

- Kein UI-Fehler blockiert den Kernelstart.
- Keine Endlosschleifen.
- Keine Speicherkorruption.
- Automatische Fehlererkennung.
- Automatische Wiederherstellung.
- Deterministische Recovery.
- Vollständige Diagnose.
- Sichere Fallbacks.
- Definierte Fehlercodes.
- Reproduzierbare Fehlerabläufe.

---

# 3. Architektur

```
                 Boot UI
                     │
              Error Detector
                     │
            Error Classification
                     │
             Recovery Manager
                     │
        ┌────────────┼────────────┐
      Retry       Fallback     Continue
                     │
              Diagnostics Log
```

---

# 4. Fehlerklassen

Jeder Fehler wird genau einer Klasse zugeordnet.

| Klasse | Bedeutung |
|---------|-----------|
| INFO | reine Information |
| WARNING | Funktion eingeschränkt |
| RECOVERABLE | automatisch behebbar |
| CRITICAL | UI-Funktion fällt aus |
| FATAL | Boot-UI wird beendet |

---

# 5. Fehlerquellen

Fehler können entstehen durch

- beschädigte PNG
- beschädigte SVG
- fehlende Schriftarten
- ungültige Themes
- beschädigte Konfiguration
- Speichermangel
- ungültige Layouts
- Renderfehler
- Firmwarefehler
- Eingabefehler
- Initialisierungsfehler
- Grafikmodusfehler
- GPU-Inkompatibilität
- Watchdog Timeout

---

# 6. Recovery-Prinzip

Jeder Fehler folgt exakt demselben Ablauf.

```
Fehler
↓
Erkennen
↓
Klassifizieren
↓
Diagnose
↓
Recovery auswählen
↓
Recovery durchführen
↓
Validieren
↓
Weiterarbeiten
```

---

# 7. Recovery-Stufen

## Level 0

```
Keine Aktion
↓
Logging
```

---

## Level 1

```
Retry
↓
Operation erneut ausführen
```

---

## Level 2

Lokalen Fallback aktivieren

Beispiel

```
SVG
↓
PNG
```

---

## Level 3

Komponente neu initialisieren

Beispiel

```
Renderer
↓
Restart
```

---

## Level 4

Subsystem deaktivieren

Beispiel

```
Blur
↓
aus
```

---

## Level 5

```
Safe Mode
↓
Animationen deaktivieren
↓
Glass deaktivieren
↓
vereinfachtes Rendering
```

---

## Level 6

```
Textmodus
↓
Boot fortsetzen
```

---

# 8. Fehlerbehandlung pro Subsystem

## Rendering

```
Fehler
↓
Renderer neu initialisieren
↓
wenn Fehler
↓
Software Renderer
↓
wenn Fehler
↓
Textmodus
```

---

## Ressourcen

```
PNG fehlt
↓
Placeholder
SVG beschädigt
↓
PNG
Font fehlt
↓
Fallback Font
Theme beschädigt
↓
Default Theme
```

---

## Animationen

```
Animation hängt
↓
Stoppen
↓
Zustand zurücksetzen
↓
Frame fortsetzen
Animationen dürfen niemals den Renderthread blockieren.
```

---

## Layout

```
Ungültiges Layout
↓
Neu berechnen
↓
Default Layout
↓
Weiterarbeiten
```

---

## Navigation

```
Ungültiger Zustand
↓
vorherige Seite
↓
Navigation zurücksetzen
↓
Recovery
```

---

## Dialoge

```
Dialog beschädigt
↓
Dialog schließen
↓
Overlay entfernen
↓
Navigation fortsetzen
```

---

# 9. Grafikmodus-Recovery

Falls Grafikinitialisierung fehlschlägt

```
Native Mode
↓
niedrigere Auflösung
↓
VESA
↓
GOP Standardmodus
↓
Textmodus
```

Der Bootprozess läuft anschließend weiter.

---

# 10. Speicher-Recovery

```
Pool erschöpft
↓
Cache freigeben
↓
Animationen deaktivieren
↓
Glass deaktivieren
↓
Blur deaktivieren
↓
Retry
↓
wenn Fehler
↓
Textmodus
```

---

# 11. Ressourcen-Recovery

```
Ungültige Ressource
↓
Hash prüfen
↓
Version prüfen
↓
Placeholder
↓
Log
↓
Weiterarbeiten
```

---

# 12. Watchdog

Alle Hauptkomponenten besitzen einen Watchdog.

Überwacht werden

- Rendering
- Layout
- Input
- Animation
- Navigation
- Ressourcen

```
Timeout
↓
Restart
↓
wenn Fehler
↓
Fallback
```

---

# 13. Safe Mode

Im Safe Mode werden deaktiviert

- Blur
- Glass
- Glow
- Schatten
- Animationen
- SVG Filter

Aktiv bleiben

- Texte
- Buttons
- Listen
- Dialoge
- Navigation

---

# 14. Textmodus

Falls sämtliche grafischen Verfahren fehlschlagen

↓

Textmodus

Anzeige

```
NovaOS Boot Manager

Graphics unavailable

Fallback Mode Active

Press Enter to Continue
```

Der Benutzer kann trotzdem

- Kernel starten
- Recovery öffnen
- Speicherdiagnose starten

---

# 15. Fehlercodes

Alle Fehler besitzen

```cpp
ErrorCode

Subsystem

Severity

Description

Recovery

Timestamp
```

Beispiel

```
BOOT_GFX_0001

Framebuffer unavailable
```

---

# 16. Logging

Jeder Fehler erzeugt

```
Severity
↓
Subsystem
↓
Beschreibung
↓
Recovery
↓
Ergebnis
```

Schweregrade

```
Trace

Debug

Info

Warning

Error

Critical

Fatal
```

---

# 17. Recovery API

```cpp
Recover()

Retry()

Fallback()

DisableFeature()

ResetSubsystem()

ShutdownSubsystem()

ContinueBoot()
```

---

# 18. Recovery State Machine

```
Healthy
↓
Failure
↓
Recovery
↓
Validation
↓
Healthy
```

oder

```
Failure
↓
Recovery
↓
Fallback
↓
Healthy
```

oder

```
Failure
↓
Fatal
↓
Shutdown UI
↓
Continue Boot
```

---

# 19. UEFI Recovery

Unter UEFI dürfen Recovery-Mechanismen ausschließlich Boot Services verwenden.

Nach

```
ExitBootServices()
```

dürfen keinerlei UEFI-Boot-Services mehr verwendet werden. Daher müssen sämtliche Recovery-Maßnahmen der Boot-UI vor diesem Zeitpunkt abgeschlossen sein. :contentReference[oaicite:0]{index=0}

---

# 20. BIOS Recovery

Unter BIOS

- keine Rückkehr in den Real Mode
- keine erneute Firmwareinitialisierung
- ausschließlich interne Recovery

---

# 21. Benutzerinformation

Bei kritischen Fehlern erscheint

```
────────────────────────────

Graphics Error

Boot UI recovered successfully.

System continues normally.

────────────────────────────
```

Nur bei fatalen Fehlern erfolgt eine sichtbare Meldung.

---

# 22. Performance

Recovery darf maximal benötigen

| Recovery | Zeit |
|-----------|------|
| Retry | < 5 ms |
| Renderer Restart | < 100 ms |
| Layout Restart | < 20 ms |
| Theme Fallback | < 10 ms |
| Textmodus | < 50 ms |

---

# 23. Testanforderungen

Es müssen simuliert werden

## Rendering

- ungültiger Framebuffer
- fehlender Backbuffer

## Speicher

- Pool Overflow
- Cache Overflow

## Ressourcen

- beschädigte PNG
- beschädigte SVG
- ungültige Fonts
- ungültiges Theme

## Layout

- ungültige Größen
- ungültige Constraints

## Navigation

- beschädigter Stack
- Dialogfehler

## Firmware

- GOP nicht verfügbar
- VBE nicht verfügbar
- Moduswechsel fehlgeschlagen

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt wenn

- kein UI-Fehler den Bootprozess verhindert
- jeder Fehler eindeutig klassifiziert wird
- jede Recovery deterministisch arbeitet
- jeder Fehler protokolliert wird
- alle Fallbacks automatisch funktionieren
- der Kernelstart jederzeit möglich bleibt
- keine Recovery Speicherlecks erzeugt
- keine Endlosschleifen auftreten

---

# 25. Referenzen

## ADR

- ADR-BOOTSAFE-0001 – Funktionaler Textmodus als letzter Fallback
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren
- ADR-BOOTSAFE-0003 – Watchdog für Rendering und Eingabe
- ADR-BOOTSAFE-0004 – Transaktionale Ausführung kritischer Bootaktionen
- ADR-BOOTSAFE-0005 – Bestätigungsdialoge für destruktive Aktionen
- ADR-BOOTPERF-0004 – Automatische Effektdegradation

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTUI-0007 – Boot UI Memory Model
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTRESOURCE-0001 – Resource Manager
- NPSPEC-BOOTDIAG-0001 – Diagnostics Framework

---

# 26. Zusammenfassung

Das **Boot UI Error Recovery System** stellt sicher, dass Fehler innerhalb der grafischen Benutzeroberfläche niemals den eigentlichen Bootprozess gefährden. Jeder Fehler wird erkannt, klassifiziert, protokolliert und anhand definierter Recovery-Stufen automatisch behandelt. Reichen lokale Wiederherstellungsmaßnahmen nicht aus, reduziert das System schrittweise den Funktionsumfang – von einzelnen Effekten über den Safe Mode bis hin zum funktionalen Textmodus. Dadurch bleibt der Bootmanager auch bei beschädigten Ressourcen, Grafikproblemen oder Speicherengpässen jederzeit bedienbar und kann den Kernelstart zuverlässig fortsetzen.