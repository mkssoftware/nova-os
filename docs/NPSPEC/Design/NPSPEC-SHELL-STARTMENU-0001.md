# NPSPEC-SHELL-STARTMENU-0001: Nova Start Menu

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Desktop Shell
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova Desktop Shell, Nova UI, Nova SDK und alle Desktop-Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert das **Nova Start Menu (NSM)**.

Das Nova Start Menu ist der zentrale Einstiegspunkt für Anwendungen, Dateien, Einstellungen und KI-Funktionen innerhalb von Nova OS.

Es kombiniert klassische Desktop-Bedienung mit moderner semantischer Suche und intelligenter Personalisierung.

Das Startmenü soll den Benutzer in möglichst wenigen Schritten zum gewünschten Ziel führen.

---

# 2. Zielsetzung

Das Nova Start Menu verfolgt folgende Ziele:

- schneller Zugriff auf Anwendungen
- sofortige Suche
- geringe Klickanzahl
- intelligente Organisation
- vollständige Tastaturbedienung
- Touch-Unterstützung
- KI-Unterstützung
- hohe Personalisierbarkeit
- vollständige Accessibility

---

# 3. Designphilosophie

Das Nova Start Menu basiert auf folgenden Prinzipien:

- Suche zuerst
- Inhalte statt Kategorien
- möglichst wenige Klicks
- ruhiges Erscheinungsbild
- konsistente Navigation
- hohe Geschwindigkeit

Das Startmenü soll niemals überladen wirken.

Moderne Startmenüs kombinieren Suche, angeheftete Anwendungen, zuletzt verwendete Inhalte und personalisierbare Bereiche für schnellen Zugriff. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova Start Menu

├── Search
├── AI Assistant
├── Pinned Applications
├── Recommended
├── All Applications
├── Categories
├── Documents
├── Settings
├── User
├── Power
└── Extensions
```

---

# 5. Layout

```text
┌────────────────────────────────────────────┐
│ Suche                                     │
├────────────────────────────────────────────┤
│ Angeheftete Apps                          │
├────────────────────────────────────────────┤
│ Empfehlungen                              │
├────────────────────────────────────────────┤
│ Alle Apps                                │
├────────────────────────────────────────────┤
│ Benutzer                  Energie          │
└────────────────────────────────────────────┘
```

Alle Bereiche besitzen feste Abstände gemäß der Nova Design Language.

---

# 6. Suchfeld

Das Suchfeld befindet sich immer am oberen Rand.

Die Suche unterstützt:

- Anwendungen
- Dateien
- Ordner
- Einstellungen
- Dokumente
- Kontakte
- Verlauf
- KI-Wissen
- Befehle

Die Suche beginnt unmittelbar während der Eingabe.

Sofortige Suchergebnisse reduzieren Navigationsaufwand und gehören zu den wichtigsten Elementen moderner Desktop-Oberflächen. :contentReference[oaicite:1]{index=1}

---

# 7. Semantische Suche

Nova Search unterstützt zusätzlich:

- Synonyme
- natürliche Sprache
- Dateiinhalte
- OCR
- Bilderkennung
- Programmfunktionen
- KI-generierte Antworten

Beispiele:

```text
"Öffne meine Urlaubsbilder"

"Zeige PDF von gestern"

"Starte Visual Studio"

"Wo liegt meine Steuererklärung?"
```

---

# 8. Angeheftete Anwendungen

Eigenschaften:

- frei sortierbar
- Drag & Drop
- Ordner
- Gruppen
- mehrere Seiten
- unbegrenzte Anzahl

Jede Anwendung besitzt:

- Icon
- Name
- Kontextmenü
- Badge
- Schnellaktionen

---

# 9. Ordner

Mehrere Anwendungen können gruppiert werden.

```text
Entwicklung

├── VS Code
├── Nova IDE
├── Git
└── Terminal
```

Ordner können umbenannt werden.

---

# 10. Empfehlungen

Empfehlungen enthalten ausschließlich:

- zuletzt verwendete Dateien
- zuletzt installierte Programme
- häufig verwendete Anwendungen
- angefangene Dokumente

Werbung ist nicht erlaubt.

Der Bereich kann vollständig deaktiviert werden. Diese Möglichkeit wird von vielen Nutzern gewünscht und moderne Betriebssysteme bieten zunehmend entsprechende Optionen. :contentReference[oaicite:2]{index=2}

---

# 11. Alle Anwendungen

Alle Anwendungen können dargestellt werden als:

- alphabetische Liste
- Kategorien
- Raster
- kompakte Liste

Der Benutzer kann frei wählen.

---

# 12. Kategorien

Automatische Kategorien:

```text
Entwicklung

Office

Grafik

Multimedia

Internet

Spiele

Werkzeuge

System

KI

Sonstige
```

Benutzer können Kategorien anpassen.

---

# 13. Dokumente

Das Startmenü zeigt:

- zuletzt geöffnet
- häufig genutzt
- angeheftet
- gemeinsam bearbeitet

Dokumente werden lokal indexiert.

---

# 14. Einstellungen

Schnellzugriff auf:

- Netzwerk
- Bluetooth
- Anzeige
- Personalisierung
- Datenschutz
- Benutzer
- Updates
- Sicherheit

Einstellungen erscheinen ebenfalls in der Suche.

---

# 15. Benutzerbereich

Zeigt:

- Benutzerbild
- Name
- Status
- Sperren
- Benutzerwechsel
- Abmelden

---

# 16. Energie-Menü

Unterstützt:

- Sperren
- Energiesparen
- Ruhezustand
- Neustart
- Herunterfahren

Erweiterte Optionen:

- Firmware
- Recovery
- Boot-Menü

---

# 17. KI-Assistent

Das Startmenü integriert den Nova Assistant.

Möglichkeiten:

- Fragen beantworten
- Programme starten
- Dateien finden
- Dokumente zusammenfassen
- Systemeinstellungen ändern
- Automatisierungen erzeugen

Der Benutzer entscheidet, ob KI aktiv ist.

---

# 18. Kontextmenüs

Jeder Eintrag besitzt:

- Öffnen
- An Start anheften
- Von Start lösen
- Als Administrator starten
- Speicherort öffnen
- Eigenschaften

Erweiterungen können zusätzliche Aktionen registrieren.

---

# 19. Tastaturbedienung

Standard:

```text
WIN

↓

Startmenü

↓

Tippen

↓

Ergebnis

↓

ENTER
```

Weitere Shortcuts:

```text
ESC

TAB

SHIFT+TAB

Pfeiltasten

CTRL

ALT
```

Das komplette Menü ist ohne Maus bedienbar.

---

# 20. Touchmodus

Im Touchmodus:

- größere Schaltflächen
- größere Trefferflächen
- größere Abstände
- Scrollen mit Gesten

---

# 21. Animationen

Das Startmenü verwendet:

- Fade
- Blur
- Scale
- Spring
- Glow

Alle Animationen folgen dem Nova Motion System.

---

# 22. Personalisierung

Der Benutzer kann ändern:

- Größe
- Layout
- Farben
- Transparenz
- Kategorien
- Empfehlungen
- Symbolgröße
- Startbereich

Alle Änderungen erfolgen ohne Neustart.

---

# 23. Erweiterungen

Das Startmenü unterstützt Erweiterungen.

Beispiele:

- Unternehmenssoftware
- VPN
- Smart Home
- Wetter
- Kalender
- KI-Plugins

Erweiterungen laufen isoliert.

---

# 24. Sicherheit

Das Startmenü:

- verarbeitet keine Werbung
- verwendet keine Cloud-Daten ohne Zustimmung
- startet Programme mit Benutzerrechten
- schützt Administratorfunktionen

---

# 25. Performance

Das Startmenü soll:

- unter 100 ms öffnen
- GPU-beschleunigt rendern
- flüssig bis 240 Hz arbeiten
- Hintergrundlast minimieren

---

# 26. Accessibility

Unterstützt:

- Screenreader
- Tastatursteuerung
- Sprachsteuerung
- hohe Kontraste
- große Schrift
- reduzierte Animationen

---

# 27. Positive Konsequenzen

- schnelle Navigation
- geringe Lernkurve
- hohe Produktivität
- moderne Benutzererfahrung
- hohe Anpassbarkeit
- konsistentes Design

---

# 28. Negative Konsequenzen

- komplexere Sucharchitektur
- umfangreiche Indizierung
- höherer Entwicklungsaufwand

---

# 29. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-SHELL-0001 – Nova Desktop Shell
- NPSPEC-SHELL-TASKBAR-0001
- NPSPEC-UI-0001
- NPSPEC-DESIGN-0001
- NPSPEC-MOTION-0001
- NPSPEC-INTERACTION-0001
- Search Manager
- AI Assistant
- Explorer
- Settings

Alle Startmenüfunktionen MÜSSEN Bestandteil der Nova Desktop Shell sein.

---

# 30. Umsetzungsregeln

- Das Suchfeld MUSS immer sichtbar sein.
- Die Suche MUSS sofort während der Eingabe beginnen.
- Das Startmenü MUSS vollständig per Tastatur bedienbar sein.
- Empfehlungen DÜRFEN vollständig deaktiviert werden.
- Werbung DARF niemals angezeigt werden.
- Alle Komponenten MÜSSEN Design Tokens verwenden.
- Alle Animationen MÜSSEN dem Nova Motion System entsprechen.
- Erweiterungen MÜSSEN sandboxed ausgeführt werden.
- Die KI-Integration MUSS optional sein.

---

# 31. Referenzen

- NPSPEC-SHELL-0001 – Nova Desktop Shell
- NPSPEC-SHELL-TASKBAR-0001 – Nova Taskbar
- NPSPEC-UI-0001 – Nova UI Architecture
- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- Microsoft – Windows Start Menu Overview :contentReference[oaicite:3]{index=3}
- Microsoft – Search Box Design Guidelines :contentReference[oaicite:4]{index=4}

---

# 32. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Start Menu als verbindliche Startmenüarchitektur für Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.