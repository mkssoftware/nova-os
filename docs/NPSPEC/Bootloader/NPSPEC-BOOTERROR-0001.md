# NPSPEC-BOOTERROR-0001: Nova Boot Error & Recovery Interface

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Boot Architecture
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova Bootloader, Nova Kernel Loader und Nova Recovery
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert die **Nova Boot Error & Recovery Interface (NBERI)**.

Sie beschreibt das Verhalten des Nova Bootloaders bei Fehlern, die während des Startvorgangs auftreten.

Im Gegensatz zu klassischen Textfehlermeldungen verfolgt Nova OS das Ziel, bereits im Bootloader eine moderne, hochwertige und vertrauenswürdige Benutzeroberfläche bereitzustellen.

Der Benutzer soll bereits in den ersten Millisekunden erkennen, dass es sich um ein modernes Betriebssystem handelt.

---

# 2. Zielsetzung

Die Boot-Fehleroberfläche soll:

- modern aussehen
- beruhigend wirken
- technisch präzise sein
- verständlich bleiben
- Touch und Maus unterstützen
- Tastatur vollständig unterstützen
- auf allen Auflösungen funktionieren
- vollständig im Nova Design gestaltet sein

Ein Bootfehler darf niemals den Eindruck eines "abgestürzten PCs" vermitteln.

Stattdessen soll er als kontrollierter Systemzustand erscheinen.

---

# 3. Designphilosophie

Die Oberfläche orientiert sich an folgenden Prinzipien:

- Fluent Design
- Glass Design
- ruhige Animationen
- dunkles Farbschema
- viel Freiraum
- große Typografie
- minimale Ablenkung

Der Benutzer soll das Gefühl erhalten:

> "Das System weiß genau, was passiert ist und kümmert sich bereits selber darum."

---

# 4. Designsprache

## Hintergrund

- sehr dunkles Anthrazit
- leichter Verlauf
- optional Glasstruktur
- feines Rauschen gegen Color Banding

---

## Primärfarbe

Nova Cyan

```text
#4CC2FF
```

---

## Fehlerfarbe

Nova Orange

```text
#FFB347
```

---

## Kritischer Fehler

Nova Rot

```text
#E53935
```

---

## Text

Weiß

```text
#FFFFFF
```

---

## Sekundärtext

Hellgrau

```text
#CFCFCF
```

---

# 5. Layout

Die Oberfläche besitzt drei Bereiche.

```text
┌───────────────────────────────────────────────┐
│                                               │
│                     :(                        │
│                                               │
│       Nova OS konnte nicht gestartet werden   │
│                                               │
│     Das System versucht automatisch eine      │
│             Wiederherstellung.                │
│                                               │
│───────────────────────────────────────────────│
│                                               │
│   Fortschritt                                │
│   Recovery Status                            │
│                                               │
│───────────────────────────────────────────────│
│                                               │
│   Weitere Optionen                           │
│                                               │
└───────────────────────────────────────────────┘
```

---

# 6. Das Nova-Symbol

Nova verwendet bewusst ein minimalistisches Symbol.

```text
;(
```

Das Symbol besitzt:

- große Darstellung
- leicht abgerundete Linien
- Fluent Animation
- weiches Einblenden

Es erinnert bewusst an Windows, besitzt jedoch eine eigene Identität.

---

# 7. Hauptmeldung

Standardtext:

```text
Nova OS konnte nicht gestartet werden.

Das System versucht automatisch,
den Fehler zu beheben.
```

Die Formulierung bleibt bewusst ruhig.

---

# 8. Detailbereich

Standardmäßig sichtbar:

- Fehlercode
- Bootphase
- Recovery Status

Beispiel:

```text
Fehlercode:

BOOT-1004

Phase:

Kernel Loader

Status:

Automatische Wiederherstellung läuft
```

---

# 9. Erweiterte Informationen

Über einen Button können Entwicklerdetails angezeigt werden.

```text
▼ Technische Details
```

Beispiel:

```text
Kernel Version

Storage Pool

NovaFS Status

GPT Status

Recovery verfügbar

Speicher

CPU

Boot Device
```

Der normale Benutzer wird hiervon nicht überfordert.

---

# 10. Fortschrittsanzeige

Während Recovery:

```text
━━━━━━━━━━━━━━━━━━━━━━

System wird überprüft

48 %
```

Die Animation läuft mit 60 FPS.

---

# 11. Automatische Recovery

Nach einem Fehler beginnt automatisch:

```text
Bootfehler

↓

Diagnose

↓

Recovery

↓

Validierung

↓

Neustart
```

Nur wenn Recovery fehlschlägt, erscheint das Bootmenü.

---

# 12. Recovery-Schritte

Die Oberfläche zeigt:

```text
✓ GPT geprüft

✓ NovaFS geprüft

✓ Prüfsummen geprüft

✓ Snapshots geprüft

✓ Recovery gefunden

→ Wiederherstellung läuft
```

Dadurch erkennt der Benutzer, dass tatsächlich gearbeitet wird.

---

# 13. Fehlerklassifizierung

Nova Boot definiert folgende Klassen.

## Information

Blau

---

## Warnung

Orange

---

## Kritischer Fehler

Rot

---

## Recovery

Cyan

---

## Erfolgreich

Grün

---

# 14. Fehlercodes

Alle Fehler besitzen ein einheitliches Schema.

```text
BOOT-1000

BOOT-1001

BOOT-2005

BOOT-3002
```

Beispiel:

| Code | Beschreibung |
|-------|--------------|
| BOOT-1001 | Kernel nicht gefunden |
| BOOT-1002 | Kernel beschädigt |
| BOOT-1003 | Signatur ungültig |
| BOOT-1004 | NovaFS konnte nicht eingebunden werden |
| BOOT-2001 | Speicherinitialisierung fehlgeschlagen |
| BOOT-3001 | Recovery nicht verfügbar |
| BOOT-4001 | TPM-Initialisierung fehlgeschlagen |
| BOOT-5001 | Snapshot beschädigt |

---

# 15. Benutzeroptionen

Standardmäßig:

```text
[ Neu starten ]

[ Recovery starten ]

[ Letzten Snapshot laden ]

[ Backup-Kernel starten ]

[ Erweiterte Optionen ]
```

Die Reihenfolge darf nicht verändert werden.

---

# 16. Erweiterte Optionen

Hier befinden sich:

- Bootmenü
- Recovery Shell
- Diagnose
- Speichertest
- Secure Boot Informationen
- TPM Informationen
- Dateisystemprüfung
- Bootlog exportieren

---

# 17. Animationen

Erlaubte Animationen:

- Fade
- Blur
- Progress
- Icon Glow
- sanfte Übergänge

Nicht erlaubt:

- hektische Animationen
- Blinken
- Springen
- Flackern

---

# 18. Bootlog

Alle Fehler werden automatisch gespeichert.

Mindestens:

```text
Zeit

Kernel

Recovery

Fehlercode

Stack

Firmware

Storage

CPU
```

Die Logs können später exportiert werden.

---

# 19. Accessibility

Die Oberfläche unterstützt:

- hohe Kontraste
- Tastatursteuerung
- Screenreader-Metadaten
- Farbfehlsichtigkeit
- große Schriftarten

---

# 20. OEM-Anpassungen

OEMs dürfen:

- Logo ergänzen
- Hintergrundbild ändern
- Firmenname anzeigen

OEMs dürfen NICHT ändern:

- Layout
- Fehlermeldungen
- Recovery-Reihenfolge
- Sicherheitsinformationen

Dadurch bleibt Nova OS auf jedem Gerät wiedererkennbar.

---

# 21. Zukunft

Geplante Erweiterungen:

- QR-Code zum Bootlog
- Smartphone-Diagnose
- Remote-Recovery
- Cloud-Recovery
- KI-gestützte Fehleranalyse
- Online-Hilfe
- Sprachunterstützung

---

# 22. Positive Konsequenzen

Die Architektur bietet zahlreiche Vorteile.

## Vertrauenswürdigkeit

Der Benutzer erkennt sofort, dass das System kontrolliert arbeitet.

---

## Einheitliches Erscheinungsbild

Bootloader und Betriebssystem besitzen dieselbe Designsprache.

---

## Geringere Unsicherheit

Klare Informationen reduzieren Verunsicherung bei Fehlern.

---

## Moderne Benutzererfahrung

Auch Fehlersituationen wirken hochwertig und professionell.

---

## Erweiterbarkeit

Neue Diagnose- und Recovery-Funktionen lassen sich integrieren, ohne das Grundlayout zu verändern.

---

# 23. Negative Konsequenzen und Risiken

## Höherer Implementierungsaufwand

Grafik-Engine, Animationen und Layout müssen bereits im Bootloader verfügbar sein.

---

## Zusätzlicher Speicherbedarf

Schriftarten, Symbole und grafische Ressourcen erhöhen die Größe des Bootloaders.

---

## Plattformtests

Die Oberfläche muss auf unterschiedlichen Auflösungen, Grafikmodi und Firmwarevarianten getestet werden.

---

# 24. Auswirkungen auf andere Module

Diese Spezifikation betrifft insbesondere:

- Nova Bootloader
- Nova Kernel Loader
- Recovery Manager
- Graphics Manager
- Theme Manager
- Localization Manager
- Logging Manager
- Self-Healing Manager
- Snapshot Manager
- Boot Configuration Manager

Alle Bootfehler MÜSSEN ausschließlich über diese standardisierte Oberfläche dargestellt werden.

---

# 25. Umsetzungsregeln

Für die Nova Boot Error & Recovery Interface gelten folgende verbindliche Regeln:

- Jeder Bootfehler MUSS eine grafische Oberfläche verwenden.
- Das Layout MUSS unabhängig von der Bildschirmauflösung funktionieren.
- Automatische Recovery MUSS vor manuellen Eingriffen versucht werden.
- Jeder Fehler MUSS einen eindeutigen Fehlercode besitzen.
- Bootlogs MÜSSEN automatisch erzeugt werden.
- Technische Details DÜRFEN standardmäßig nicht eingeblendet sein.
- Die Benutzeroberfläche MUSS mit Tastatur, Maus und Touch bedienbar sein.
- Animationen DÜRFEN den Bootvorgang nicht merklich verzögern.
- OEM-Anpassungen DÜRFEN die Benutzerführung nicht verändern.

---

# 26. Referenzen

- NPSPEC-BOOT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-KERNELLOADER-0001
- NPSPEC-BOOTHANDOFF-0001
- NPSPEC-KERNELENTRY-0001
- ADR-6033 – Separates Recovery-Subvolume
- ADR-6034 – Snapshot-Architektur
- ADR-6040 – Selbstheilung und Scrubbing

---

# 27. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Boot Error & Recovery Interface als verbindliche Fehleroberfläche für den Nova-Bootprozess definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.