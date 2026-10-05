# NPSPEC-REGISTRY-SERVICE-0001 – Nova Service Registry

## Status

Angenommen

## Kategorie

Registry / Service

## Zweck

NovaOS definiert die Service Registry als systemweites Verzeichnis registrierter System Services und ihrer verfügbaren Schnittstellen.

Sie ermöglicht Discovery und Auflösung von Services unabhängig von Prozess-ID, Speicherort oder konkreter Serviceinstanz.

## Grundprinzipien

```text
ServiceID ≠ ProcessID
ServiceID ≠ Endpoint
Service Registry ≠ Service State
Registration ≠ Execution
Discovery ≠ Authority
Service Reference ≠ Permission
```

## Registry-Modell

Ein Service-Eintrag kann enthalten:

```text
ServiceRegistryEntry
├── ServiceID
├── Version
├── Interfaces
├── Endpoints
├── State
├── ActivationPolicy
└── Compatibility
```

Optional:

```text
ProviderID
Dependencies
Trust State
Resource Requirements
Metadata
```

Die `ServiceID` bildet die stabile Identität des Dienstes.

## Registrierung

```text
System Service
      ↓
Validate Identity
      ↓
Validate Interfaces
      ↓
Register
      ↓
Publish
```

Nur gültige Serviceeinträge dürfen für Discovery veröffentlicht werden.

## Discovery

Clients können Services anhand ihrer Anforderungen suchen:

```text
Service Requirement
        ↓
Service Registry
        ↓
Candidate Services
        ↓
Compatibility Check
        ↓
Resolved Service
```

Die Registry darf dabei auch mehrere kompatible Service-Provider liefern.

## Serviceinstanzen

Ein logischer Service kann unterschiedliche Laufzeitinstanzen besitzen:

```text
ServiceID
   ↓
Service Instance
   ↓
ProcessID
   ↓
IPC Endpoint
```

Ein Neustart des Serviceprozesses verändert daher nicht automatisch die `ServiceID`.

## Aktivierung

Ist ein registrierter Service nicht aktiv, darf die Registry mit dem Service-Management eine bedarfsgesteuerte Aktivierung auslösen.

```text
Discovery
   ↓
Service Registered
   ↓
Not Running
   ↓
Activation Request
   ↓
Service Start
```

Registrierung bedeutet nicht, dass ein Service permanent laufen muss.

## Versionierung

Mehrere Serviceversionen dürfen parallel registriert sein, sofern dies erforderlich und kompatibel ist.

Die Auswahl erfolgt anhand von:

```text
Version
Interface
Compatibility
Policy
Trust
Requirements
```

## Ausfall

Wird eine Serviceinstanz beendet oder ersetzt, müssen veraltete Endpoints erkannt werden.

```text
Service Failure
      ↓
Registry State Update
      ↓
Restart / Replacement
      ↓
New Endpoint
```

Die stabile Serviceidentität bleibt erhalten.

## Sicherheit

Die Service Registry speichert keine aktiven Capability-Tokens.

```text
Service Discovery
      ↓
ServiceID
      ↓
Permission / Capability Check
      ↓
Authorized IPC Connection
```

Das Auffinden eines Service gewährt keine Berechtigung zu dessen Nutzung.

## Normative Anforderungen

1. NovaOS MUSS eine Service Registry bereitstellen.
2. Services MÜSSEN über stabile `ServiceID`s registrierbar sein.
3. `ServiceID` und `ProcessID` MÜSSEN getrennt bleiben.
4. Laufzeitendpoints DÜRFEN sich ändern, ohne die Serviceidentität zu verändern.
5. Service-Schnittstellen MÜSSEN versionierbar sein.
6. Discovery DARF keine Authority erzeugen.
7. Die Registry DARF keine aktiven Capability-Tokens speichern.
8. Bedarfsgesteuerte Serviceaktivierung MUSS integrierbar sein.
9. Veraltete Serviceinstanzen und Endpoints MÜSSEN erkennbar sein.
10. Mehrere kompatible Serviceversionen oder Provider MÜSSEN unterstützt werden können.
11. Servicezugriffe MÜSSEN den aktuellen Sicherheitskontext berücksichtigen.
12. Serviceidentität, Version, Zustand und verfügbare Interfaces MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-SYSTEM-SERVICES-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine zentrale Registry zur stabilen Discovery und Auflösung von System Services. Serviceidentität bleibt von Prozessinstanz und IPC-Endpoint getrennt, sodass Services neu gestartet, ersetzt oder bedarfsgesteuert aktiviert werden können, ohne Clients an konkrete Laufzeitinstanzen zu koppeln.