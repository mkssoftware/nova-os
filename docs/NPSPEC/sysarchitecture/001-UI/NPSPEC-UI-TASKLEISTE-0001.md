# NPSPEC-UI-TASKLEISTE-0001 – Nova Taskleiste

## Status

Angenommen

## Kategorie

UI / Taskleiste / Desktop / Window Management / System UI

## Zweck

Die Nova Taskleiste ist die dauerhaft oder kontextabhängig verfügbare zentrale Navigationsleiste des Desktops.

Sie verbindet:

```text
Startmenü
Angeheftete Funktionen
Anwendungen
Offene Fenster
Aktivitäten
Systemstatus
Benachrichtigungen
Schnelleinstellungen
Uhrzeit / Datum
Dashboard
```

Die Taskleiste soll vertraut und ruhig wirken, gleichzeitig aber die objekt- und capability-orientierte Architektur von NovaOS unterstützen.

## Grundprinzip

Die Taskleiste ist keine einfache Liste laufender Programme.

```text
Taskleiste
 ├── Start
 ├── Angeheftet
 ├── Aktive Fenster
 ├── Aktivitäten
 └── Systembereich
```

Ein Eintrag kann repräsentieren:

```text
Application
Window
Object
Capability
Activity
System Function
```

## Architektur

```text
Applications
Windows
Objects
Capabilities
System Services
        ↓
Taskbar Model
        ↓
Declarative UI
        ↓
Nova Taskleiste
        ↓
Window Server / Scene Graph
```

Die Taskleiste ist geschützte NovaOS-System-UI.

## Grundaufbau

Eine typische Desktop-Darstellung kann sein:

```text
┌─────────────────────────────────────────────────────────────────────┐
│ Start │ Suche │ App │ App │ App │ offene Fenster │ ... │ System │
└─────────────────────────────────────────────────────────────────────┘
```

oder kompakter:

```text
┌─────────────────────────────────────────────────────────────────────┐
│ N │ 🔍 │ ◉ │ ◉ │ ◉ │              │ Netz │ Ton │ 18:42 │ 🔔 │
└─────────────────────────────────────────────────────────────────────┘
```

Die genaue visuelle Darstellung wird durch Theme und Layout bestimmt.

## Start

Die Taskleiste stellt einen direkten Zugang zum Nova Startmenü bereit.

```text
Start Action
     ↓
NPSPEC-UI-STARTMENÜ-0001
```

Das Nova-Symbol darf gleichzeitig als zentraler Einstiegspunkt in die Systemnavigation dienen.

## Suche

Optional kann die Taskleiste direkten Zugriff auf die systemweite Suche bieten.

```text
Search
  ↓
Objects
Applications
Capabilities
Settings
Commands
Content
```

Die Suche kann als:

```text
Icon
Compact Field
Expanded Field
```

dargestellt werden.

## Angeheftete Elemente

Nutzer können häufig verwendete Elemente anheften.

Beispiele:

```text
Application
Capability
Object
Folder
Workspace
System Function
```

Angeheftete Objekte sollen stabile IDs statt ausschließlich Dateipfade verwenden.

## Aktive Anwendungen

Laufende Anwendungen können in der Taskleiste dargestellt werden.

```text
Application
     ↓
Active Instance
     ↓
Taskbar Entry
```

Mehrere Fenster derselben Anwendung können gruppiert werden.

```text
Application
 ├── Window A
 ├── Window B
 └── Window C
```

## Fensterdarstellung

Taskleisten-Einträge können Fensterzustände darstellen.

```text
Active
Inactive
Minimized
Attention Required
Busy
```

Die Darstellung muss eindeutig erkennen lassen, welche Elemente aktuell aktiv sind.

## Fenster-Vorschau

Beim Zeigen oder Fokussieren eines Eintrags kann eine Fenster-Vorschau erscheinen.

```text
Taskbar Entry
      ↓
Window Preview
      ↓
Window Selection
```

Beispiel:

```text
┌──────────────────────┐
│ Dokument A           │
│ [Live Preview]       │
├──────────────────────┤
│ Dokument B           │
│ [Live Preview]       │
└──────────────────────┘
```

Vorschauen dürfen nur Inhalte anzeigen, für die der aktuelle Kontext autorisiert ist.

Geschützte Fenster können ihre Vorschau deaktivieren oder reduzieren.

## Objektorientierte Einträge

Ein Taskleisten-Eintrag muss nicht zwingend einer Anwendung entsprechen.

Beispiel:

```text
Projekt.nf
    ↓
Taskbar Entry
    ↓
Project Workspace
```

oder:

```text
Image.Edit
    ↓
Editing Session
    ↓
Taskbar Entry
```

Dadurch kann die Taskleiste laufende Aufgaben statt ausschließlich Prozesse darstellen.

## Aktivitäten

Länger laufende Systemaktivitäten können sichtbar gemacht werden.

Beispiele:

```text
Download
Copy
Backup
Synchronization
Installation
Rendering
Computation
```

Ein Fortschritt kann direkt am entsprechenden Eintrag dargestellt werden.

```text
[████████░░] 80 %
```

Die Taskleiste soll jedoch nicht mit Hintergrundaktivitäten überladen werden.

Nur relevante Aktivitäten werden direkt hervorgehoben.

## Systembereich

Der Systembereich stellt wichtige Systemzustände kompakt dar.

Beispiele:

```text
Network
Audio
Battery / Power
Bluetooth
VPN
Devices
Privacy Status
Notifications
Clock
```

Nicht jeder Hintergrunddienst erhält automatisch ein dauerhaftes Symbol.

## Schnelleinstellungen

Mehrere Systemstatus-Elemente können zu einem gemeinsamen Quick-Settings-Bereich zusammengefasst werden.

```text
Network
Volume
Bluetooth
Display
Power
Accessibility
      ↓
Quick Settings
```

Dadurch wird eine überfüllte Taskleiste vermieden.

## Benachrichtigungen

Die Taskleiste stellt den Zugang zum Notification Center bereit.

```text
Notification Service
       ↓
Notification Indicator
       ↓
Notification Center
```

Neue Benachrichtigungen dürfen dezent signalisiert werden.

Dringlichkeit und Sicherheitsrelevanz bestimmen die Hervorhebung.

## Uhrzeit und Datum

Die Taskleiste kann anzeigen:

```text
Time
Date
Calendar Status
```

Eine Aktivierung kann eine kompakte Kalender- oder Terminansicht öffnen.

## Dashboard

Die Taskleiste kann direkten Zugang zum Nova Dashboard bereitstellen.

```text
Taskbar
   ↓
Dashboard
   ↓
Weather
News
Calendar
Activities
System Information
```

Wetter und Nachrichten werden dabei primär im Dashboard dargestellt und müssen die Taskleiste nicht dauerhaft belegen.

Optional kann eine kompakte Information wie:

```text
18 °C  ☁
```

als Dashboard-Einstieg angezeigt werden.

## Kontextmenüs

Taskleisten-Einträge können semantische Kontextaktionen anbieten.

Beispiel:

```text
Document
 ├── Open
 ├── New Window
 ├── Recent Objects
 ├── Pin
 └── Close
```

Die verfügbaren Aktionen werden aus Kontext, Objekt und Capabilities bestimmt.

## Drag & Drop

Taskleisten-Einträge können Drag & Drop unterstützen.

Beispiel:

```text
Image
  ↓
Drag onto Image Editor
  ↓
Image.Edit
```

Die Übergabe erfolgt über kontrollierte Objekt- oder Capability-Delegation.

## Aufmerksamkeit

Anwendungen dürfen Aufmerksamkeit anfordern.

```text
Application
     ↓
Attention Request
     ↓
Window Server Policy
     ↓
Taskbar Indicator
```

Sie dürfen jedoch nicht unkontrolliert:

```text
Fenster fokussieren
Fenster nach vorne holen
Taskleiste dauerhaft blinken lassen
```

Der Nutzer behält die Kontrolle.

## Position

Die Taskleiste kann abhängig von Systemdesign und Nutzerpräferenz positioniert werden.

Beispiele:

```text
Bottom
Top
Left
Right
Floating
```

Die Standardkonfiguration von NovaOS darf eine schwebende untere Taskleiste verwenden.

## Ausrichtung

Elemente können:

```text
Centered
Left Aligned
Adaptive
```

angeordnet werden.

Die Position des Startmenüs und zentraler Navigationselemente muss vorhersehbar bleiben.

## Automatisches Ausblenden

Die Taskleiste kann optional automatisch ausgeblendet werden.

```text
Visible
   ↓
Inactive
   ↓
Hidden
   ↓
Pointer / Gesture / Shortcut
   ↓
Visible
```

Kritische Systemzustände müssen trotzdem signalisiert werden können.

## Multi-Display

NovaOS unterstützt Taskleisten auf mehreren Displays.

Mögliche Modi:

```text
Primary Display Only

All Displays

Relevant Windows per Display
```

Die Darstellung muss mit dem jeweiligen Workspace und Displayzustand konsistent bleiben.

## Workspaces

Die Taskleiste kann Workspace-Wechsel unterstützen.

```text
Workspace 1
Workspace 2
Workspace 3
```

Dabei kann konfiguriert werden, ob:

```text
alle Fenster

oder

nur Fenster des aktuellen Workspace
```

angezeigt werden.

## Responsive Verhalten

Die Taskleiste passt sich an den verfügbaren Platz an.

```text
Large Display
→ vollständige Taskleiste

Medium
→ kompaktere Elemente

Small
→ priorisierte Elemente + Overflow
```

Wichtige Funktionen dürfen nicht einfach unerreichbar werden.

## Touch

Im Touch-Modus können:

```text
größere Targets
größere Abstände
vereinfachte Gruppen
Touch Gestures
```

verwendet werden.

Die zugrunde liegende Semantik bleibt identisch.

## Adaptive Verhalten

NovaOS darf die Taskleiste vorsichtig an den Nutzungskontext anpassen.

Beispiele:

```text
häufig verwendete Aktionen vorschlagen
relevante Aktivitäten hervorheben
unwichtige Statusanzeigen zusammenfassen
```

Adaptive Mechanismen dürfen nicht:

```text
Nutzer-Pins entfernen
Elemente ständig verschieben
grundlegende Navigation verändern
Berechtigungen ableiten
```

Explizite Nutzerkonfiguration besitzt Vorrang.

## Ressourcenverhalten

Taskleisten-Vorschauen und Animationen sollen nur bei Bedarf erzeugt werden.

```text
No Preview Required
→ no preview rendering

Preview Requested
→ obtain authorized preview
→ display
```

Dadurch werden unnötige GPU- und Speicheraktivitäten vermieden.

## Datenschutz

Taskleisten-Inhalte können sensible Informationen enthalten.

Beispiele:

```text
Window Titles
Document Names
Previews
Notifications
Activities
```

NovaOS muss diese Informationen entsprechend Session-, Privacy- und Security-Kontext behandeln.

## Sicherheit

Die Taskleiste besitzt eine geschützte System-UI-Rolle.

Normale Anwendungen dürfen nicht:

```text
Taskleisten-Einträge anderer Anwendungen manipulieren
Systemstatus vortäuschen
geschützte Systembereiche überlagern
fremde Fenster-Vorschauen lesen
Systemaktionen imitieren
```

Die Sichtbarkeit eines Eintrags erzeugt keine zusätzliche Capability.

## Fehlerverhalten

Der Ausfall einer Anwendung darf die Taskleiste nicht beeinträchtigen.

```text
Application Crash
       ↓
Window State Update
       ↓
Taskbar Model Update
       ↓
Invalid Entry Removed
```

Auch der Ausfall optionaler Dienste wie Wetter, Nachrichten oder Synchronisation darf die grundlegende Taskleistenfunktion nicht blockieren.

## Accessibility

Die Taskleiste muss vollständig über den Accessibility Tree verfügbar sein.

Elemente müssen semantisch unterscheidbar sein als:

```text
Start
Search
Application
Window
Activity
System Status
Notification
Clock
Dashboard
```

Tastaturnavigation muss ohne Pointer möglich sein.

## Normative Anforderungen

1. Die Nova Taskleiste MUSS als geschützte System-UI ausgeführt werden.
2. Die Taskleiste MUSS aktive Fenster und Anwendungen darstellen können.
3. Die Taskleiste SOLL zusätzlich Objekte, Capabilities und Aktivitäten repräsentieren können.
4. Nutzer MÜSSEN Elemente anheften und entfernen können.
5. Nutzer-Pins MÜSSEN gegenüber adaptiven Änderungen Vorrang besitzen.
6. Mehrere Fenster einer Anwendung SOLLEN gruppierbar sein.
7. Fenster-Vorschauen SOLLEN unterstützt werden.
8. Geschützte Fenster MÜSSEN Vorschauen einschränken oder verhindern können.
9. Die Taskleiste MUSS Zugang zu Startmenü und Systemstatus bereitstellen.
10. Quick Settings und Notification Center SOLLEN direkt erreichbar sein.
11. Das Dashboard SOLL direkt über die Taskleiste erreichbar sein.
12. Wetter darf optional kompakt als Dashboard-Einstieg dargestellt werden.
13. Taskleisten-Einträge SOLLEN semantische Kontextaktionen anbieten können.
14. Anwendungen DÜRFEN den Fokus nicht allein durch einen Attention Request übernehmen.
15. Die Taskleiste MUSS Multi-Display- und Workspace-Konfigurationen unterstützen können.
16. Die Taskleiste MUSS responsive auf verfügbaren Platz reagieren.
17. Die Taskleiste MUSS per Maus, Tastatur und Touch bedienbar sein.
18. Anwendungen DÜRFEN fremde Taskleisten-Einträge oder geschützte Systembereiche nicht manipulieren.
19. Sensible Titel, Vorschauen und Benachrichtigungen MÜSSEN Privacy-Regeln unterliegen.
20. Der Ausfall einzelner Anwendungen oder Provider DARF die Taskleiste nicht blockieren.
21. Alle wesentlichen Taskleistenfunktionen MÜSSEN im Accessibility Tree repräsentiert werden.

## Abhängigkeiten

- `NPSPEC-UI-DESKTOP-0001`
- `NPSPEC-UI-STARTMENÜ-0001`
- `NPSPEC-UI-DASHBOARD-0001`
- `NPSPEC-UI-FENSTER-0001`
- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-COMPONENT-0001`
- `NPSPEC-UI-THEME-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-INPUTROUTING-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`

## Ergebnis

Die Nova Taskleiste verbindet klassische Desktop-Navigation mit dem objekt- und capability-orientierten Modell von NovaOS:

```text
Start
Pinned Objects
Applications
Windows
Capabilities
Activities
        ↓
Nova Taskleiste
        ↓
System Status
Quick Settings
Notifications
Dashboard
```

Damit bleibt die Taskleiste vertraut wie bei modernen Desktop-Systemen, kann aber neben Anwendungen auch Objekte, laufende Aufgaben und NovaOS-Capabilities darstellen, ohne die Oberfläche mit permanenten Informationen zu überladen.