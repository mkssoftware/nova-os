# NPSPEC-BOOTUI-0009
# Boot UI Configuration

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0009 |
| Titel | Boot UI Configuration |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0008 |
| ADRs | ADR-BOOTRESOURCE-0009, ADR-BOOTRESOURCE-0010, ADR-BOOTDESIGN-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige Konfigurationssystem der NovaOS Boot-UI.

Die Konfiguration beschreibt sämtliche Einstellungen der grafischen Oberfläche, ohne dass Programmcode geändert werden muss.

Sie umfasst unter anderem:

- Themes
- Farben
- Animationen
- Layout
- Sprache
- Skalierung
- Eingabeverhalten
- Debugoptionen
- Recoveryoptionen
- Performanceprofile

Das Ziel ist ein vollständig deterministisches, versionsfähiges und validierbares Konfigurationssystem.

---

# 2. Architekturziele

Das Konfigurationssystem verfolgt folgende Ziele.

- vollständige Trennung zwischen Code und Konfiguration
- reproduzierbare Systeme
- versionsfähige Konfigurationsdaten
- atomare Änderungen
- automatische Validierung
- sichere Standardwerte
- Vorwärts- und Rückwärtskompatibilität
- keine Benutzerinteraktion während der Initialisierung erforderlich

---

# 3. Architektur

```
                    Boot UI
                       │
              Configuration Manager
                       │
        ┌──────────────┼──────────────┐
      Loader        Validator      Defaults
                       │
               Runtime Configuration
                       │
          ┌───────┌────────┌─────────┐
        Theme   Layout   Input   Rendering
```

---

# 4. Verantwortlichkeiten

Der Configuration Manager ist verantwortlich für

- Laden der Konfiguration
- Validierung
- Versionsprüfung
- Migration
- Bereitstellung für alle Subsysteme
- Änderungsbenachrichtigung
- Wiederherstellung auf Standardwerte

---

# 5. Konfigurationsquellen

Die Boot-UI darf Konfigurationen ausschließlich aus definierten Quellen laden.

Priorität

```
Hardcoded Defaults
↓
Boot Configuration Package
↓
OEM Configuration
↓
Benutzerkonfiguration
↓
Temporäre Runtime Overrides
```

Alle Ebenen überschreiben ausschließlich erlaubte Werte.

---

# 6. Konfigurationsbereiche

## Darstellung

- Theme
- Farben
- Blur
- Transparenz
- Schatten
- Rundungen

---

## Layout

- Skalierung
- DPI
- Safe Area
- Dialoggrößen
- Raster

---

## Animation

- Dauer
- Geschwindigkeit
- Reduced Motion
- Übergänge
- Easing

---

## Eingabe

- Tastatur
- Maus
- Touch
- Fokusverhalten
- Wiederholrate

---

## Rendering

- Double Buffer
- Triple Buffer
- Software Renderer
- Hardware Renderer
- Qualitätsstufe

---

## Diagnose

- Logging
- Debug Overlay
- FPS Anzeige
- Speichernutzung
- Renderstatistik

---

## Recovery

- Safe Mode
- Textmodus
- Automatische Degradation
- Watchdog

---

# 7. Konfigurationsformat

Die interne Struktur wird als binäres, versionsfähiges Datenformat gespeichert.

Jede Konfiguration besitzt

```cpp
ConfigurationID

Version

Checksum

Timestamp

Flags

Sections
```

Das interne Format wird bewusst unabhängig von JSON, XML oder YAML definiert, um Speicherverbrauch, Parsingzeit und Integritätsprüfung zu optimieren.

---

# 8. Versionsverwaltung

Jede Konfiguration besitzt

```
Major

Minor

Patch
```

Beispiel

```
1.0.0
```
```
Beim Laden
↓
Versionsprüfung
↓
Migration
↓
Validierung
↓
Runtime
```

---

# 9. Standardkonfiguration

```
Falls keine Konfiguration gefunden wird
↓
Hardcoded Defaults
↓
Boot fortsetzen
```

Der Bootmanager muss jederzeit ohne externe Konfigurationsdateien startfähig bleiben.

---

# 10. Validierung

Vor Verwendung werden geprüft

- Version
- Größe
- Prüfsumme
- Struktur
- Wertebereiche
- Abhängigkeiten
- Pflichtfelder

Ungültige Konfigurationen werden verworfen.

---

# 11. Konfigurationsobjekte

Beispiel

```cpp
BootUIConfiguration
{
    Theme

    Language

    Scale

    Motion

    Input

    Rendering

    Diagnostics

    Recovery
}
```

---

# 12. Theme-Konfiguration

Beispiel

```cpp
Theme
↓
Dark
↓
Glass Enabled
↓
Corner Radius
↓
Blur Radius
↓
Accent Color
```

---

# 13. Layout-Konfiguration

Speichert

- Grid
- Margin
- Padding
- Dialoggrößen
- Mindestgrößen
- Responsive Regeln

---

# 14. Animationseinstellungen

Konfigurierbar

- Geschwindigkeit
- Dauer
- Reduced Motion
- Spring
- Fade
- Blur Animation

---

# 15. Rendering-Einstellungen

Konfigurierbar

```
Software

Hardware

Quality

Frame Limit

Present Mode

Buffer Count
```

---

# 16. Eingabeeinstellungen

Konfigurierbar

- Fokusnavigation
- Mausgeschwindigkeit
- Touch-Schwellenwerte
- Tastaturwiederholung
- Doppelklickzeit

---

# 17. Sprachkonfiguration

Speichert

- Sprache
- Region
- Datumsformat
- Zahlenformat

UTF-8 ist verpflichtend.

Nicht verfügbare Übersetzungen verwenden automatisch die Standardsprache.

---

# 18. Recovery-Konfiguration

Beispiel

```cpp
EnableSafeMode

EnableTextFallback

RetryCount

WatchdogTimeout

AutoRecovery
```

---

# 19. Runtime Overrides

Bestimmte Einstellungen dürfen temporär überschrieben werden.

Beispiele

```
Safe Mode
↓
Blur deaktivieren
↓
Animationen deaktivieren
```

Nach Neustart gelten wieder die gespeicherten Werte.

---

# 20. Änderungsmodell

Konfigurationsänderungen erfolgen atomar.

```
Alte Konfiguration
↓
Validierung
↓
Neue Konfiguration
↓
Aktivieren
```

Ungültige Änderungen werden verworfen.

---

# 21. Integrität

Jede Konfiguration besitzt

- Prüfsumme
- Versionsnummer
- Formatkennung

Optional zusätzlich

- digitale Signatur
- OEM-Signatur

Beschädigte Konfigurationen werden nicht verwendet.

---

# 22. Fehlerbehandlung

```
Ungültige Konfiguration
↓
Standardwerte
↓
Log
↓
Weiterarbeiten
Fehlende Bereiche
↓
auffüllen
↓
weiterarbeiten
```

---

# 23. API

```cpp
LoadConfiguration()

SaveConfiguration()

ValidateConfiguration()

ResetDefaults()

ReloadConfiguration()

GetConfiguration()

SetConfigurationValue()

GetConfigurationValue()
```

---

# 24. Sicherheit

Die Konfiguration darf niemals

- Code enthalten
- Skripte ausführen
- Zeiger speichern
- Speicheradressen enthalten

Alle Werte werden vor Verwendung validiert.

---

# 25. Performance

Maximale Ladezeit

```
20 ms
```

Validierung

```
< 5 ms
```

Konfigurationszugriffe

```
O(1)
```

Keine Heap-Allokationen während normaler Lesezugriffe.

---

# 26. UEFI-Konfiguration

Unter UEFI können bestimmte Firmware-Einstellungen zusätzlich berücksichtigt werden.

Beispiele

- aktuelle Bildschirmauflösung
- GOP-Informationen
- Firmware-Sprache
- Firmware-Konsole

Diese Informationen werden ausschließlich gelesen und nicht verändert. Die UEFI-Spezifikation definiert hierfür standardisierte Boot-Variablen sowie Konsolen- und Sprachinformationen, die von Anwendungen genutzt werden können. :contentReference[oaicite:0]{index=0}

---

# 27. BIOS-Konfiguration

Unter BIOS stehen keine standardisierten Firmware-Konfigurationsvariablen zur Verfügung.

Alle UI-Einstellungen stammen daher ausschließlich aus

- Standardwerten
- Boot-Konfiguration
- OEM-Konfiguration

---

# 28. Testanforderungen

Zu testen sind

## Konfiguration

- gültige Version
- ungültige Version
- beschädigte Prüfsumme
- fehlende Bereiche

## Darstellung

- Themewechsel
- Skalierung
- Sprache

## Rendering

- Qualitätsprofile
- Safe Mode

## Recovery

- Standardwerte
- Reset
- Migration

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt wenn

- jede Konfiguration validiert wird
- ungültige Daten niemals verwendet werden
- Standardwerte jederzeit funktionieren
- Migrationen reproduzierbar arbeiten
- Änderungen atomar erfolgen
- Boot ohne Konfigurationsdatei möglich ist
- alle Einstellungen versionsfähig sind

---

# 30. Referenzen

## ADR

- ADR-BOOTRESOURCE-0009 – Design-Token-Paket für Bootmanager und NovaOS
- ADR-BOOTRESOURCE-0010 – Versionsgebundene Theme-Ressourcen
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language

---

# 31. Zusammenfassung

Die **Boot UI Configuration** definiert ein vollständig versionsfähiges, deterministisches und validierbares Konfigurationssystem für die NovaOS-Bootoberfläche. Sämtliche Einstellungen – von Theme, Layout und Animationen bis hin zu Rendering-, Eingabe- und Recovery-Optionen – werden zentral verwaltet und vor ihrer Verwendung geprüft. Durch atomare Änderungen, definierte Standardwerte und automatische Validierung bleibt die Bootoberfläche jederzeit konsistent, reproduzierbar und fehlertolerant. Das Konfigurationssystem bildet die Grundlage für eine langfristig wartbare und erweiterbare Boot-UI-Architektur.