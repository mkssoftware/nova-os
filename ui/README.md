# Nova UI Runtime

Dieses Subsystem ist die erste ausführbare Referenzimplementierung für die 26
angenommenen Spezifikationen unter `docs/NPSPEC/sysarchitecture/001-UI`.

## Implementierter Kern

- stark typisierte `NodeID`, `AccessibilityID`, `DisplayID`, `SurfaceID`,
  `WindowID`, `ObjectID`, `CapabilityID`, `ProviderID` und `OwnerID`
- deklaratives Desired-/Actual-State-Modell mit stabilen Retained-Mode-Nodes
- versionierter Scene Graph mit Parent-Prüfung, Reconciliation und Damage
- separater, inkrementell generationierter Accessibility Tree einschließlich
  Virtualisierung, eigenem Fokus und capabilitygeprüften Actions
- semantische Rollen, Actions, Objektziele und Capability-Anforderungen
- semantische Color-/Theme-Tokens, Dark, Light und High Contrast sowie
  Accessibility- und Ressourcen-Fallbacks
- DLU-basierte responsive Layoutanpassung und sichere Adaptive-UI-Regeln
- Display-, Surface- und Window-Verträge mit Hotplug-Reconciliation
- GPU-, Software- und Direct-Scanout-Entscheidung mit Software-Fallback
- begrenzte Damage-Listen, Buffer Age, Frame-Coalescing, Backpressure und VRR
- session-isoliertes Input-Routing mit Z-Order-Hit-Test, Fokus, Touch-Zielbindung,
  Secure Input und widerrufbarem Pointer Capture
- dynamische Capability-Verfügbarkeit, getrennte Discovery/Authorization und
  kontrollierte Start-, Ribbon-, Dashboard- und Taskleisten-Contributions
- Fehlerisolation für ungültige Surfaces, Provider, Owner und Displays

## Systemgrenze

Die Runtime definiert und testet die UI-Verträge ohne Heap- oder
Betriebssystemabhängigkeit. Reale GPU-/Displaytreiber, Prozessgrenzen,
Shared-/GPU-Buffer, persistente Desktopdaten, Suchindizes, Privacy-Dienste und
der ausführende Capability Broker werden später als Provider an diese Grenze
angebunden. Bis dahin bleibt Software Composition der funktionale Fallback.

## Test

```text
make ui-architecture-runtime-check
```

Der Test wird mit C17, `-Wall -Wextra -Werror` gebaut und prüft die zentralen
Invarianten aller Architekturblöcke einschließlich negativer Sicherheitsfälle.
