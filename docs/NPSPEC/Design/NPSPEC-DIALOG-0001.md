# NPSPEC-DIALOG-0001: Nova Dialog System

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** User Interface
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova OS, Nova UI Framework, Nova SDK und alle Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert das **Nova Dialog System (NDS)**.

Dialoge sind temporäre Benutzeroberflächen zur Anzeige wichtiger Informationen, zur Abfrage von Entscheidungen oder zur Eingabe zusätzlicher Daten.

Dialoge sollen den Benutzer nur dann unterbrechen, wenn dies tatsächlich notwendig ist.

---

# 2. Zielsetzung

Das Nova Dialog System verfolgt folgende Ziele:

- klare Kommunikation
- minimale Unterbrechung
- konsistente Gestaltung
- vollständige Tastaturbedienung
- Touch-Unterstützung
- hohe Accessibility
- moderne Animationen
- GPU-beschleunigtes Rendering

---

# 3. Designphilosophie

Dialoge sind bewusst selten eingesetzte Werkzeuge.

Sie sollen:

- informieren
- bestätigen
- Eingaben ermöglichen
- Fortschritt anzeigen

Dialoge dürfen niemals den normalen Arbeitsablauf ersetzen.

Dialoge sollten sparsam eingesetzt werden und nur erscheinen, wenn eine unmittelbare Entscheidung oder Eingabe erforderlich ist. Für viele Aufgaben sind Inline-Komponenten oder Panels besser geeignet. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova Dialog System

├── Dialog Manager
├── Modal Dialogs
├── Modeless Dialogs
├── Sheet Dialogs
├── Wizard Dialogs
├── Progress Dialogs
├── Input Dialogs
├── Confirmation Dialogs
├── Error Dialogs
└── Notification Dialogs
```

---

# 5. Dialoglebenszyklus

```text
Anforderung

↓

Erzeugen

↓

Animation

↓

Interaktion

↓

Bestätigung

↓

Schließen

↓

Freigeben
```

---

# 6. Dialogtypen

Nova definiert folgende Standarddialoge:

- Informationsdialog
- Bestätigungsdialog
- Warnungsdialog
- Fehlerdialog
- Eingabedialog
- Fortschrittsdialog
- Assistent (Wizard)
- Eigenschaften
- Einstellungen
- Datei öffnen
- Datei speichern

Alle Anwendungen verwenden ausschließlich diese Standardtypen.

---

# 7. Modalität

Nova unterscheidet zwei Dialogarten.

## Modal

Der Benutzer muss den Dialog abschließen, bevor er mit dem Hauptfenster weiterarbeiten kann.

Verwendung:

- Löschen
- Formatieren
- Administratorrechte
- Verschlüsselung
- Kritische Warnungen

---

## Nicht-modal

Der Benutzer kann parallel weiterarbeiten.

Verwendung:

- Suche
- Eigenschaften
- Werkzeuge
- Live-Monitoring

Modalität soll nur verwendet werden, wenn eine Unterbrechung des Arbeitsflusses gerechtfertigt ist. :contentReference[oaicite:1]{index=1}

---

# 8. Dialogstruktur

```text
┌─────────────────────────────────────────────┐
│ Symbol  Titel                       X       │
├─────────────────────────────────────────────┤
│ Hauptüberschrift                            │
│                                             │
│ Beschreibung                                │
│                                             │
│ Inhalt                                      │
│                                             │
├─────────────────────────────────────────────┤
│ Zurück      Abbrechen      Bestätigen       │
└─────────────────────────────────────────────┘
```

---

# 9. Titel

Jeder Dialog besitzt:

- eindeutigen Titel
- eindeutige Aufgabe
- kurze Formulierung

Beispiele:

```text
Datei löschen

Netzwerk verbinden

Projekt speichern
```

---

# 10. Symbol

Standardisierte Symbole:

```text
Information

Warnung

Fehler

Frage

Erfolg

Fortschritt
```

Es werden ausschließlich Fluent SVG Icons verwendet.

---

# 11. Hauptüberschrift

Die Hauptüberschrift beschreibt die eigentliche Aufgabe.

Beispiele:

```text
Möchten Sie diese Datei löschen?

Neue Verbindung herstellen

Projekt speichern?
```

---

# 12. Beschreibung

Die Beschreibung erklärt:

- Auswirkungen
- Konsequenzen
- benötigte Eingaben

Sie soll kurz und eindeutig sein.

---

# 13. Inhalt

Der Inhaltsbereich kann enthalten:

- Text
- Formulare
- Listen
- Bilder
- Tabellen
- Dateiauswahl
- Fortschrittsanzeigen

Lange Dokumente gehören nicht in Dialoge. Stattdessen sollen eigene Fenster oder Seiten verwendet werden. :contentReference[oaicite:2]{index=2}

---

# 14. Buttons

Standardbuttons:

```text
OK

Abbrechen

Ja

Nein

Speichern

Löschen

Weiter

Zurück

Fertig

Schließen
```

Buttons besitzen immer eindeutige Beschriftungen.

Verben sind generischen Bezeichnungen wie „OK“ vorzuziehen, wenn dadurch die Aktion klarer beschrieben wird. :contentReference[oaicite:3]{index=3}

---

# 15. Buttonreihenfolge

Standard:

```text
Sekundär

↓

Abbrechen

↓

Primäraktion
```

Die Primäraktion wird hervorgehoben.

---

# 16. Tastatur

Standardbelegung:

```text
ENTER

↓

Primäraktion

ESC

↓

Abbrechen

TAB

↓

Nächstes Element

SHIFT+TAB

↓

Vorheriges Element
```

---

# 17. Fokus

Beim Öffnen erhält das sinnvollste Steuerelement den Fokus.

Regeln:

- niemals kein Fokus
- Fokus sichtbar
- Fokus bleibt im Dialog

---

# 18. Animationen

Dialoge verwenden:

- Fade
- Blur
- Scale
- Spring

Öffnen:

```text
95 %

↓

100 %

↓

Fokus
```

Schließen:

```text
100 %

↓

97 %

↓

Fade
```

---

# 19. Hintergrund

Während modaler Dialoge:

- Hintergrund wird leicht abgedunkelt
- Blur wird aktiviert
- Fokus bleibt auf dem Dialog

---

# 20. Fortschrittsdialoge

Unterstützen:

- Spinner
- Fortschrittsbalken
- Prozent
- Zeitabschätzung
- Abbrechen

Bei langen Vorgängen muss ein Fortschritt angezeigt werden. :contentReference[oaicite:4]{index=4}

---

# 21. Eingabedialoge

Unterstützen:

- Validierung
- Live-Feedback
- Hilfetexte
- Platzhalter
- Standardwerte

Ungültige Eingaben deaktivieren die Primäraktion.

---

# 22. Fehlerdialoge

Fehlerdialoge zeigen:

- Fehlercode
- Ursache
- Lösungsvorschläge
- Details
- Kopieren
- Supportinformationen

---

# 23. Bestätigungsdialoge

Nur verwenden für:

- Datenverlust
- Löschen
- Formatieren
- Sicherheitskritische Aktionen
- Administratorrechte

Unkritische Aktionen benötigen keine Bestätigung.

---

# 24. Accessibility

Unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- Sprachsteuerung
- große Schrift
- reduzierte Animationen

---

# 25. Performance

Dialoge sollen:

- unter 100 ms erscheinen
- GPU-beschleunigt rendern
- keine Layoutsprünge erzeugen
- geringe Speicherbelegung besitzen

---

# 26. Entwickler-API

Das Nova SDK stellt bereit:

```text
Dialog

↓

AlertDialog

↓

ConfirmDialog

↓

InputDialog

↓

ProgressDialog

↓

WizardDialog
```

Alle Anwendungen verwenden dieselbe API.

---

# 27. Erweiterbarkeit

Neue Dialogtypen müssen:

- dokumentiert sein
- standardisierte Buttons verwenden
- Design Tokens verwenden
- Motion Tokens verwenden
- Accessibility unterstützen

---

# 28. Positive Konsequenzen

- konsistente Benutzerführung
- klare Kommunikation
- geringere Fehlerquote
- hohe Wiedererkennbarkeit
- einfache Entwicklung

---

# 29. Negative Konsequenzen

- höherer Implementierungsaufwand
- umfangreiche UX-Tests erforderlich
- Standardisierung schränkt Sonderlösungen ein

---

# 30. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-UI-0001 – Nova UI Architecture
- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-WINDOW-0001 – Nova Window System
- Theme Manager
- Window Manager
- Accessibility Manager
- Notification Center

Alle Dialoge innerhalb von Nova OS MÜSSEN diesem Standard entsprechen.

---

# 31. Umsetzungsregeln

- Dialoge MÜSSEN nur verwendet werden, wenn eine Unterbrechung erforderlich ist.
- Jeder Dialog MUSS eine eindeutige Aufgabe besitzen.
- Die Primäraktion MUSS eindeutig hervorgehoben werden.
- Dialoge MÜSSEN vollständig per Tastatur bedienbar sein.
- Modalität MUSS sparsam eingesetzt werden.
- Alle Dialoge MÜSSEN GPU-beschleunigt dargestellt werden.
- Alle Dialoge MÜSSEN die Nova Design Language verwenden.
- Animationen MÜSSEN dem Nova Motion System folgen.
- Accessibility MUSS vollständig unterstützt werden.

---

# 32. Referenzen

- NPSPEC-UI-0001 – Nova UI Architecture
- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-WINDOW-0001 – Nova Window System
- Microsoft Dialog Design Guidelines :contentReference[oaicite:5]{index=5}
- KERN UX-Standard Dialoge :contentReference[oaicite:6]{index=6}
- GNOME Human Interface Guidelines – Dialogs :contentReference[oaicite:7]{index=7}

---

# 33. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Dialog System als verbindlicher Dialogstandard für Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.