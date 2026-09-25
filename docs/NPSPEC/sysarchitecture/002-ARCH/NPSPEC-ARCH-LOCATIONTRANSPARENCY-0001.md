# NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001 – Nova Location Transparency

## Status

Angenommen

## Kategorie

Architecture / Distribution / Resources / Execution

## Zweck

NovaOS trennt die Identität und Funktion einer Ressource von ihrem physischen Ausführungs- oder Speicherort.

```text
Object / Capability / Resource
            ↓
        Stable ID
            ↓
    Location Resolution
       ↙          ↘
    Local        Remote
```

Aufrufer sollen eine Ressource nach Möglichkeit über ihre semantische Identität verwenden, ohne deren konkreten Standort kennen zu müssen.

## Grundprinzip

```text
Identity ≠ Location
```

Eine Ressource kann ihren Standort ändern, ohne dadurch ihre logische Identität zu verlieren.

## Unterstützte Ressourcen

Location Transparency kann gelten für:

```text
Objects
Capabilities
Services
Compute Provider
Storage
Devices
Data
```

Nicht jede Ressource muss remote verwendbar sein.

## Location Resolution

NovaOS bestimmt den geeigneten oder vorgeschriebenen Standort anhand von:

```text
Execution Contract
Security
Trust
Data Sovereignty
Latency
Resources
Availability
User Policy
```

Beispiel:

```text
Capability Request
      ↓
Local Provider
Remote Provider
      ↓
Constraint Filtering
      ↓
Selected Provider
```

## Execution

Eine Operation kann lokal oder remote ausgeführt werden.

```text
Intent
  ↓
Execution Contract
  ↓
Execution Planning
  ↓
Local / Remote
```

Der Aufrufer verwendet dabei weiterhin denselben semantischen Capability-Vertrag.

## Compute-to-Data

Wenn sinnvoll, soll NovaOS bevorzugt Berechnung zu den Daten verschieben statt große Datenmengen unnötig zu übertragen.

```text
Data Location
      ↓
Execution Placement
```

Dies gilt nur, wenn Security-, Trust- und Sovereignty-Regeln dies erlauben.

## Standortwechsel

Ressourcen dürfen zwischen Standorten migriert oder durch andere Provider bereitgestellt werden.

```text
Location A
    ↓
Migration / Rebinding
    ↓
Location B
```

Stabile Identitäten bleiben erhalten, sofern es sich weiterhin um dieselbe logische Entität handelt.

## Fehlerverhalten

Remote-Ressourcen können zusätzliche Zustände besitzen:

```text
Available
Unavailable
Disconnected
Unknown
```

Ein Verbindungsverlust darf nicht automatisch als Fehlschlag einer bereits gestarteten Remote-Operation interpretiert werden.

```text
Unknown ≠ Failed
Unknown ≠ Completed
```

## Sicherheit

Location Transparency darf Sicherheitsgrenzen niemals unsichtbar umgehen.

```text
Transparent Location
        ≠
Transparent Authority
```

Remote-Ausführung muss dieselben oder strengere Anforderungen an:

```text
Capabilities
Trust
Security
Data Sovereignty
```

erfüllen.

## Normative Anforderungen

1. NovaOS MUSS Identität und physischen Standort logisch trennen können.
2. Stabile IDs DÜRFEN NICHT vom aktuellen Standort abhängen.
3. Capabilities SOLLEN unabhängig vom konkreten Provider-Standort aufrufbar sein.
4. Execution Planning SOLL lokale und entfernte Provider berücksichtigen können.
5. Location Selection MUSS Execution Contracts berücksichtigen.
6. Security-, Trust- und Sovereignty-Regeln MÜSSEN vor Standortoptimierungen gelten.
7. Location Transparency DARF NICHT zusätzliche Berechtigungen erzeugen.
8. Compute-to-Data SOLL unterstützt werden, wenn dies effizienter und zulässig ist.
9. Standortwechsel DÜRFEN die logische Identität einer Ressource nicht unnötig verändern.
10. Ein unbekannter Remote-Zustand DARF NICHT als Erfolg oder Fehlschlag interpretiert werden.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Ergebnis

```text
Stable Identity
      ↓
Location Resolution
      ↓
Local / Remote Resource
      ↓
Controlled Execution
```

NovaOS kann Ressourcen und Capabilities damit unabhängig von ihrem konkreten Standort verwenden, ohne Security, Trust oder Data Sovereignty der Transparenz zu opfern.