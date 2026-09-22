# NPSPEC-UI-DASHBOARD-0001 – Nova Dashboard

## Status

Angenommen

## Kategorie

UI / Dashboard / Widgets / Information / System UI

## Zweck

Das Nova Dashboard stellt dem Nutzer eine zentrale, persönliche Übersicht über relevante Informationen, Nachrichten, Wetter, Termine, Systemzustände und laufende Aktivitäten bereit.

Es orientiert sich funktional an der Widget-Idee moderner Desktops wie Windows 11, wird jedoch tiefer in die semantische und Capability-basierte Architektur von NovaOS integriert.

```text
Wetter
Nachrichten
Kalender
Termine
Aufgaben
Systemstatus
Benachrichtigungen
Medien
Persönliche Informationen
      ↓
Nova Dashboard
```

Das Dashboard soll auf einen Blick beantworten:

```text
Was passiert gerade?
Was ist heute wichtig?
Was gibt es Neues?
Was benötigt meine Aufmerksamkeit?
```

## Architektur

```text
System Services
Online Services
Capabilities
Applications
User Context
      ↓
Dashboard Contributions
      ↓
Dashboard Model
      ↓
Declarative UI
      ↓
Responsive Dashboard
```

Das Dashboard ist keine monolithische Anwendung, sondern eine System-UI-Projektion verschiedener Informationsquellen und Capabilities.

## Dashboard-Bereiche

Das Dashboard kann unter anderem enthalten:

```text
Dashboard
 ├── Wetter
 ├── Nachrichten
 ├── Kalender
 ├── Termine
 ├── Aufgaben
 ├── Benachrichtigungen
 ├── Systemstatus
 ├── Laufende Aktivitäten
 ├── Medien
 ├── Geräte
 └── Persönliche Widgets
```

Welche Bereiche sichtbar sind, entscheidet der Nutzer.

## Dashboard Cards

Informationen werden bevorzugt als Cards bzw. Widgets dargestellt.

```text
┌─────────────────────┐
│ Wetter              │
│ 18 °C   ☁          │
│ Suhl                │
│ Regen ab 17:00      │
└─────────────────────┘

┌─────────────────────┐
│ Nachrichten         │
│ Lokale Nachrichten  │
│ Technik             │
│ Wissenschaft        │
└─────────────────────┘
```

Eine Card kann enthalten:

```text
CardID
Semantic Type
Title
Content
Source
State
Actions
Priority
Update Policy
Privacy Policy
```

## Wetter

Das Dashboard kann aktuelle Wetterinformationen anzeigen.

```text
Current Weather
Forecast
Temperature
Precipitation
Warnings
Wind
Location-based Forecast
```

Die Wetterquelle wird über eine Capability abstrahiert.

```text
Weather.Query
      ↓
Weather Provider
      ↓
Semantic Weather Data
      ↓
Weather Card
```

Der Dashboard-Code ist dadurch nicht fest an einen bestimmten Wetteranbieter gebunden.

## Nachrichten

Das Dashboard kann personalisierte Nachrichtenfeeds anzeigen.

```text
Local News
Germany
World
Technology
Science
Economy
Sports
User-defined Topics
```

Architektur:

```text
News Providers
      ↓
News Capability
      ↓
Semantic Articles
      ↓
Filtering / Ranking
      ↓
News Feed
```

Quellen müssen für den Nutzer erkennbar bleiben.

NovaOS darf Inhalte nicht so darstellen, als stammten sie vom Betriebssystem selbst.

## Lokale Informationen

Mit entsprechender Berechtigung können lokale Informationen angezeigt werden.

Beispiele:

```text
Local News
Weather
Traffic
Public Transport
Events
Warnings
```

Standortinformationen dürfen nur entsprechend der Privacy- und Capability-Regeln verwendet werden.

## Kalender und Termine

Das Dashboard kann bevorstehende Termine darstellen.

```text
Today
 ├── 09:00 Meeting
 ├── 14:30 Appointment
 └── 18:00 Event
```

Mehrere Kalenderquellen können über ein gemeinsames semantisches Modell zusammengeführt werden.

## Aufgaben

Aufgaben und Erinnerungen können als eigene Cards erscheinen.

```text
Tasks
 ├── Due Today
 ├── Upcoming
 └── Overdue
```

Die Quelle kann lokal oder über autorisierte externe Provider angebunden sein.

## Systeminformationen

NovaOS-Systemdienste können relevante Zustände bereitstellen.

Beispiele:

```text
Battery
Storage
Network
Updates
Backup
Security
Connected Devices
Downloads
Synchronization
```

Das Dashboard soll jedoch keine permanenten technischen Detailinformationen anzeigen, wenn diese für den Nutzer aktuell nicht relevant sind.

## Laufende Aktivitäten

Länger laufende Vorgänge können dargestellt werden.

```text
Copying Files
Downloading
Installing
Rendering
Backup
Synchronization
Computation
```

Status:

```text
Pending
Running
Paused
Completed
Failed
```

## Medien

Aktive Medienwiedergabe kann integriert werden.

```text
Album Art
Title
Artist
Progress
Play / Pause
Previous / Next
```

Die Steuerung erfolgt über entsprechende Media Capabilities.

## Drittanbieter-Widgets

Anwendungen und Dienste dürfen eigene Dashboard Contributions bereitstellen.

```text
Provider
   ↓
Semantic Dashboard Contribution
   ↓
Nova Dashboard
```

Ein Provider liefert keine vollständig unkontrollierte Oberfläche, sondern definierte semantische Inhalte und Aktionen.

Dadurch bleiben:

```text
Design
Security
Accessibility
Responsive Behaviour
Privacy
```

unter Kontrolle von NovaOS.

## Personalisierung

Der Nutzer kann das Dashboard konfigurieren.

```text
Widget hinzufügen
Widget entfernen
Widget anheften
Widget verschieben
Widget vergrößern
Widget verkleinern
Kategorie auswählen
Quelle auswählen
```

Beispiel:

```text
┌───────────────┬───────────────┐
│ Wetter        │ Kalender      │
├───────────────┴───────────────┤
│ Nachrichten                   │
├───────────────┬───────────────┤
│ Systemstatus  │ Medien        │
└───────────────┴───────────────┘
```

Explizite Nutzeranordnung hat Vorrang vor automatischer Anpassung.

## Adaptive Inhalte

NovaOS darf Inhalte anhand des Nutzungskontexts priorisieren.

```text
Morning
   ↓
Weather + Calendar + News

Work Context
   ↓
Tasks + Calendar + Activities

Evening
   ↓
News + Media + Personal Content
```

Dies ist eine Priorisierung, keine zwingende automatische Umgestaltung.

Der Nutzer behält die Kontrolle.

## Nachrichten-Personalisierung

Nachrichten können anhand explizit gewählter Interessen gefiltert werden.

```text
Selected Topics
      ↓
News Sources
      ↓
Semantic Filtering
      ↓
Personal News Feed
```

Adaptive Empfehlungen dürfen ergänzend verwendet werden.

Explizit gewählte Themen und Quellen haben Vorrang.

## Aktualisierung

Dashboard-Daten besitzen eigene Update Policies.

Beispiele:

```text
Weather       → periodically / event based
News          → periodically
Calendar      → event based
System State  → event based
Media         → live state
```

Unnötiges permanentes Polling soll vermieden werden.

## Offline-Verhalten

Das Dashboard muss auch ohne Internet funktionsfähig bleiben.

```text
Internet unavailable
        ↓
Local Widgets remain active
        ↓
Online Cards use cached state
        ↓
Clearly mark stale information
```

Online-Dienste dürfen keine Voraussetzung für den Desktop darstellen.

## Datenschutz

Jede Dashboard Contribution erhält nur die Informationen, für die sie autorisiert wurde.

Beispiele:

```text
Weather → optional location
Calendar → calendar capability
News → selected interests
System Card → specific system state
```

Eine News-Card benötigt beispielsweise keinen Zugriff auf Kalender oder persönliche Dateien.

## Sperrbildschirm und sensible Daten

Für sensible Informationen muss definiert werden, ob sie bei gesperrter Sitzung sichtbar sein dürfen.

```text
Public
Protected
Private
```

Beispielsweise können:

```text
Weather → Public

Calendar Title → Protected

Private Message Content → Private
```

klassifiziert werden.

## Fehlerisolation

Der Ausfall eines Providers darf das Dashboard nicht blockieren.

```text
News Provider Failure
        ↓
News Card unavailable
        ↓
Other Cards continue
```

Alternative Provider dürfen verwendet werden, wenn dies mit Nutzerpräferenzen und Verträgen vereinbar ist.

## Accessibility

Alle Cards und Aktionen müssen semantisch über den Accessibility Tree erreichbar sein.

Information darf nicht ausschließlich über:

```text
Color
Position
Animation
Icon
```

vermittelt werden.

## Normative Anforderungen

1. Das Nova Dashboard MUSS deklarativ aufgebaut sein.
2. Wetter, Nachrichten, Kalender und vergleichbare Informationsdienste MÜSSEN als Dashboard Contributions integrierbar sein.
3. Online-Dienste SOLLEN über Capabilities abstrahiert werden.
4. Das Dashboard DARF nicht fest an einzelne Wetter-, News- oder andere Datenanbieter gekoppelt sein.
5. Nachrichtenquellen MÜSSEN für den Nutzer erkennbar sein.
6. Der Nutzer MUSS Widgets hinzufügen, entfernen und anordnen können.
7. Explizite Nutzerkonfiguration MUSS gegenüber adaptiver Anordnung Vorrang besitzen.
8. Dashboard Contributions MÜSSEN responsive dargestellt werden können.
9. Online-Widgets MÜSSEN ihren Aktualitätszustand darstellen können.
10. Der Desktop MUSS auch bei Ausfall aller Online-Dashboard-Dienste funktionsfähig bleiben.
11. Standort- und persönliche Daten MÜSSEN capability- und privacy-basiert geschützt werden.
12. Drittanbieter DÜRFEN keine unkontrollierte System-UI in das Dashboard injizieren.
13. Der Ausfall einer Contribution DARF andere Dashboard-Bereiche nicht blockieren.
14. Dashboard-Aktionen DÜRFEN keine Autorisierung allein durch ihre Sichtbarkeit erhalten.
15. Dashboard-Inhalte MÜSSEN im Accessibility Tree repräsentierbar sein.
16. Kritische Systeminformationen MÜSSEN gegenüber Nachrichten und Empfehlungen priorisiert werden.

## Abhängigkeiten

- `NPSPEC-UI-DESKTOP-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-CAPABILITY-0001`
- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`

## Ergebnis

Das Nova Dashboard wird zu einer persönlichen Informationszentrale ähnlich dem Widget-Konzept moderner Desktops, aber vollständig in die NovaOS-Architektur integriert:

```text
Weather
News
Calendar
Tasks
System
Media
External Services
       ↓
Semantic Contributions
       ↓
Capabilities + Privacy
       ↓
Personalization
       ↓
Nova Dashboard
```

Damit erhält der Nutzer auf einen Blick Wetter, Nachrichten, Termine, laufende Aktivitäten und wichtige Systeminformationen, ohne dass diese Funktionen fest in eine einzelne monolithische Dashboard-Anwendung eingebaut werden müssen.