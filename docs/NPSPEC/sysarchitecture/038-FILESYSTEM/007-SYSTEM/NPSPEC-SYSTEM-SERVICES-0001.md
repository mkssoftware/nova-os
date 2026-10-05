# NPSPEC-SYSTEM-SERVICES-0001 – Nova System Services

## Status

Angenommen

## Kategorie

System / Services

## Zweck

NovaOS definiert System Services als dauerhaft oder bedarfsgesteuert laufende Systemkomponenten, die Funktionen für andere Systembereiche, Programme und Solutions bereitstellen.

Services laufen außerhalb des Kernels und kommunizieren über definierte Schnittstellen und IPC.

## Grundprinzipien

```text
Service ≠ Kernel
Service ≠ Module
Service ≠ Program
Service Identity ≠ ProcessID
Service Availability ≠ Authority
Service Crash ≠ System Crash
```

## Servicemodell

Ein System Service besitzt mindestens:

```text
SystemService
├── ServiceID
├── Version
├── Interfaces
├── Dependencies
├── State
└── ExecutionContext
```

Optional:

```text
Capabilities
ResourceBudget
RestartPolicy
ActivationPolicy
HealthState
```

Die `ServiceID` bleibt unabhängig von Prozess, Pfad oder aktueller Instanz.

## Architektur

```text
Programs / Solutions / System
            ↓
      Framework / IPC
            ↓
       System Service
            ↓
 Foundation / Runtime
            ↓
          Kernel
```

Clients verwenden definierte Service-Schnittstellen und greifen nicht auf interne Servicezustände zu.

## Aktivierung

Services können unterschiedliche Aktivierungsmodelle besitzen:

```text
Boot
On Demand
Event Driven
Dependency Driven
Manual
```

Nicht permanent benötigte Services sollen bedarfsgesteuert aktiviert werden können.

## Lifecycle

```text
Registered
    ↓
Starting
    ↓
Running
    ↓
Stopping
    ↓
Stopped
```

Zusätzliche Zustände können sein:

```text
Degraded
Failed
Restarting
Unavailable
```

## Abhängigkeiten

Service-Abhängigkeiten müssen explizit beschrieben werden.

```text
Service A
   ↓
requires
   ↓
Service B
```

Zirkuläre oder nicht erfüllbare Abhängigkeiten müssen erkannt werden.

Startreihenfolgen sollen aus Abhängigkeiten abgeleitet werden, statt global fest codiert zu sein.

## Supervision

System Services werden überwacht.

```text
Service
  ↓
Health Monitoring
  ↓
Supervisor
  ↓
Restart / Degrade / Disable
```

Ein abgestürzter Service soll nach Policy neu gestartet werden können, ohne das gesamte System neu zu starten.

## Capability-Modell

Jeder Service besitzt einen eigenen Sicherheitskontext.

```text
Service
  ↓
Granted Capabilities
  ↓
Authorized Resources
```

Ein System Service erhält nicht automatisch globale Authority.

Clients übertragen nur die für eine Operation erforderliche Authority.

## Resource Economy

Services erhalten definierbare Ressourcenbudgets für beispielsweise:

```text
CPU
Memory
I/O
Storage
Network
Handles
```

Fehlerhafte Services dürfen Systemressourcen nicht unbegrenzt verbrauchen.

## Live Evolution

Services sollen kontrolliert aktualisiert oder ersetzt werden können:

```text
Old Service
    ↓
Prepare
    ↓
Start New Version
    ↓
Transfer State
    ↓
Switch
    ↓
Verify
    ↓
Retire Old Version
```

## Normative Anforderungen

1. System Services MÜSSEN stabile `ServiceID`s besitzen.
2. `ServiceID` und `ProcessID` MÜSSEN getrennt bleiben.
3. Services SOLLEN außerhalb des Kernels ausgeführt werden.
4. Kommunikation MUSS über definierte Schnittstellen erfolgen.
5. Service-Abhängigkeiten MÜSSEN explizit deklarierbar sein.
6. Bedarfsgesteuerte Aktivierung MUSS unterstützt werden können.
7. Services MÜSSEN überwacht und kontrolliert neu gestartet werden können.
8. Servicefehler SOLLEN nicht automatisch zum Systemausfall führen.
9. Services DÜRFEN keine implizite globale Authority erhalten.
10. Ressourcenverbrauch MUSS begrenzbar sein.
11. Services SOLLEN kontrolliert live ersetzbar sein.
12. Zustand, Version, Abhängigkeiten, Health und Ressourcenverbrauch MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-FRAMEWORK-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-MODULES-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Modell für isolierte und überwachte System Services. Dienste können bedarfsgesteuert gestartet, capability-basiert begrenzt, nach Fehlern neu gestartet und unabhängig vom Kernel aktualisiert oder ersetzt werden.