# NPSPEC-UI-WINDOWSERVER-0001 – Nova Window Server

## Status

Angenommen

## Kategorie

UI / Window Server / Compositor / Display

## Zweck

Der Nova Window Server bildet die zentrale Systemkomponente für Darstellung, Fensterverwaltung und sichere Übergabe von Benutzerinteraktionen.

Er stellt die technische Grundlage für:

```text
Desktop
Fenster
Panels
Overlays
Dialoge
Benachrichtigungen
Lock Screen
System UI
```

bereit.

## Architektur

Der Window Server ist kein Bestandteil einzelner Anwendungen.

```text
Applications
     ↓
UI / Surface API
     ↓
Nova Window Server
     ↓
Compositor
     ↓
Display / GPU
```

Anwendungen liefern darstellbare Surfaces. Der Window Server bestimmt deren Position, Sichtbarkeit, Z-Order und Zusammensetzung.

## Kernkomponenten

```text
Nova.WindowServer
 ├── Surface Manager
 ├── Window Manager
 ├── Compositor
 ├── Input Router
 ├── Display Manager
 └── Policy Interface
```

### Surface Manager

Verwaltet darstellbare Inhalte.

Eine Surface besitzt mindestens:

```text
SurfaceID
Owner
Dimensions
Format
State
Damage Region
Security Context
```

### Window Manager

Verwaltet Fensterzustände und deren Beziehungen.

Typische Zustände:

```text
Normal
Minimized
Maximized
Fullscreen
Hidden
Suspended
```

Fensterzustand und eigentlicher Anwendungsinhalt bleiben getrennt.

### Compositor

Der Compositor setzt sichtbare Surfaces zum finalen Bild zusammen.

```text
Surfaces
   ↓
Composition
   ↓
Final Frame
   ↓
Display
```

GPU-Beschleunigung wird bevorzugt, darf aber keine funktionale Voraussetzung für grundlegende Darstellung sein.

### Input Router

Eingaben werden anhand von Fokus, Position, Security Context und UI-Zustand an den berechtigten Empfänger weitergeleitet.

```text
Keyboard / Mouse / Touch
          ↓
     Input Router
          ↓
Authorized Surface
```

Eine Anwendung erhält nur Eingaben, die für ihren Kontext bestimmt sind.

### Display Manager

Verwaltet physische und virtuelle Displays einschließlich:

```text
Resolution
Refresh Rate
Scaling
Orientation
Display Topology
Color Information
```

Mehrere Displays werden als zusammenhängende oder getrennte Display Spaces unterstützt.

## Fensteridentität

Fenster und Surfaces besitzen stabile IDs.

```text
Application
   ↓
WindowID
   ↓
SurfaceID
```

Titel, Position oder Prozess-ID dienen nicht als primäre Identität.

## Rendering

Anwendungen rendern ihren Inhalt selbst oder über Nova UI Services.

Der Window Server soll möglichst keine Anwendungsgrafik kopieren müssen.

Bevorzugter Datenpfad:

```text
Application
     ↓
Shared / GPU Buffer
     ↓
Window Server
     ↓
Compositor
     ↓
Display
```

Zero-Copy wird verwendet, wenn Hardware und Sicherheitsbedingungen dies erlauben.

## Damage Tracking

Anwendungen melden geänderte Bereiche einer Surface.

```text
Previous Frame
      +
Damage Region
      ↓
Composition Update
```

Unveränderte Bereiche müssen nicht vollständig neu verarbeitet werden.

## System UI

Bestimmte Oberflächen besitzen besondere Systemrollen.

Beispiele:

```text
Desktop
Taskbar
Start UI
Notifications
Lock Screen
Permission Dialog
Security Prompt
```

Systemrollen werden ausschließlich über explizite Capabilities vergeben.

Eine normale Anwendung darf sich nicht selbst als vertrauenswürdige Systemoberfläche deklarieren.

## Isolation

Anwendungen dürfen grundsätzlich nicht:

```text
fremde Surfaces lesen
fremde Fenster verändern
globale Eingaben abfangen
Security UI überdecken
fremde Input Events empfangen
```

Solche Operationen benötigen explizite Capabilities.

## Failure Handling

Der Window Server muss Fehler einzelner Anwendungen isolieren.

```text
Application Crash
       ↓
Surface removed/frozen
       ↓
Window Server continues
```

Ein Fehler einer Anwendung darf den Window Server nicht zum Absturz bringen.

Bei einem Fehler des Window Servers muss NovaOS eine definierte Recovery-Strategie besitzen.

## Normative Anforderungen

1. Der Window Server MUSS außerhalb einzelner Anwendungen betrieben werden.
2. Anwendungen MÜSSEN Inhalte über kontrollierte Surfaces bereitstellen.
3. Fenster und Surfaces MÜSSEN eindeutige IDs besitzen.
4. Anwendungen DÜRFEN fremde Surfaces nicht ohne Capability lesen oder verändern.
5. Input MUSS ausschließlich an autorisierte Empfänger weitergeleitet werden.
6. Vertrauenswürdige System-UI-Rollen MÜSSEN capability-basiert vergeben werden.
7. Der Compositor MUSS mehrere Surfaces zu einem Display Frame zusammensetzen können.
8. Zero-Copy SOLL verwendet werden, wenn Sicherheit und Hardware dies erlauben.
9. Damage Tracking SOLL partielle Aktualisierungen ermöglichen.
10. Fehler einzelner Anwendungen DÜRFEN den Window Server nicht kompromittieren.
11. Mehrere Displays und unterschiedliche Skalierungsfaktoren MÜSSEN unterstützt werden.
12. Grundlegende Darstellung MUSS auch ohne spezielle GPU-Beschleunigung möglich bleiben.

## Abhängigkeiten

- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0001`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-VERIFY-0005_Capability_Safety`

## Ergebnis

Der Nova Window Server bildet eine kleine, kontrollierte Grenze zwischen Anwendungen und Display-System:

```text
Applications
     ↓
Surfaces
     ↓
Nova Window Server
     ↓
Composition
     ↓
Display
```

Fensterverwaltung, Composition und Input Routing werden zentral koordiniert, während Anwendungen voneinander isoliert bleiben.