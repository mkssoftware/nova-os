# ADR-COMPAT-0015 – Legacy Application als Capability Provider

## Status

Angenommen

## Kategorie

Compatibility / Capabilities / Legacy Integration

## Kontext

Legacy-Anwendungen können Funktionen bereitstellen, die auch von nativen NovaOS-Komponenten genutzt werden sollen.

Beispiele:

```text
Bildkonvertierung
Dokumentenverarbeitung
Codec
Datenimport/-export
Berechnung
Legacy-Fachsoftware
```

Ohne definierte Integration müssten andere Anwendungen direkt mit der Legacy-Anwendung, deren Dateien, Prozessen oder proprietären Schnittstellen kommunizieren.

Dadurch würden Legacy-Konzepte in die native NovaOS-Architektur eindringen.

## Entscheidung

NovaOS erlaubt Legacy-Anwendungen, ihre Funktionen über einen **Compatibility Adapter als Nova Capability Provider** bereitzustellen.

```text
Nova Consumer
      ↓
Nova Capability
      ↓
Compatibility Adapter
      ↓
Legacy Application
```

Die Legacy-Anwendung selbst wird dadurch nicht zu einer nativen NovaOS-Komponente.

## Capability Adapter

Der Adapter übersetzt zwischen dem nativen Capability Contract und der Legacy-Schnittstelle.

```text
Nova Request
    ↓
Semantic Validation
    ↓
Compatibility Translation
    ↓
Legacy API / IPC / CLI
    ↓
Legacy Application
```

Legacy-spezifische Details bleiben hinter dem Adapter verborgen.

## Registrierung

Eine Legacy-Anwendung darf nur Capabilities anbieten, die explizit registriert wurden.

Die Registrierung beschreibt mindestens:

```text
Capability ID
Provider Identity
Supported Operations
Input / Output Types
Compatibility Domain
Version
Trust State
```

Die Existenz einer Legacy-Anwendung erzeugt nicht automatisch einen Capability Provider.

## Semantic Translation

Legacy-Datenformate können durch den Adapter auf Nova Semantic Types abgebildet werden.

```text
Nova Semantic Object
        ↓
Adapter
        ↓
Legacy Format
        ↓
Legacy Application
```

Das Legacy-Format wird dadurch nicht zum nativen Datentyp des Systems.

## Isolation

Der Provider bleibt innerhalb seiner Compatibility Domain.

```text
Nova Capability Call
        ↓
Controlled Boundary
        ↓
Compatibility Domain
        ↓
Legacy Application
```

Der Aufrufer erhält keinen direkten Zugriff auf den Legacy-Prozess oder dessen Umgebung.

## Autorität

Ein Capability-Aufruf darf der Legacy-Anwendung nur die für diesen Auftrag erforderliche Autorität übertragen.

```text
Consumer Authority
       ↓
Attenuation
       ↓
Operation Capability
       ↓
Legacy Provider
```

Die Legacy-Anwendung erhält dadurch keine allgemeinen Rechte des aufrufenden Prozesses.

## Fehlerbehandlung

Fehler des Legacy Providers werden in definierte NovaOS-Ergebnisse übersetzt.

```text
Legacy Failure
      ↓
Compatibility Adapter
      ↓
Nova Error / Provider Failure
```

Ein Absturz des Legacy Providers darf nicht automatisch den Consumer oder das Gesamtsystem beschädigen.

## Provider-Auswahl

Ein Legacy Provider kann neben nativen Providern existieren.

```text
Capability
   ├── Native Provider
   ├── Legacy Provider
   └── Remote Provider
```

Der normale NovaOS-Execution- und Capability-Mechanismus kann zwischen geeigneten Providern wählen.

## Normative Anforderungen

1. Legacy-Anwendungen DÜRFEN über Compatibility Adapter als Capability Provider eingebunden werden.
2. Legacy-Anwendungen DÜRFEN nicht allein durch ihre Installation automatisch Capabilities bereitstellen.
3. Provider MÜSSEN explizit registriert und identifizierbar sein.
4. Legacy-spezifische Schnittstellen MÜSSEN hinter dem Capability Contract verborgen bleiben.
5. Legacy Provider MÜSSEN innerhalb ihrer Compatibility Domain bleiben.
6. Capability-Aufrufe DÜRFEN nur die erforderliche Autorität übertragen.
7. Legacy Provider DÜRFEN keine zusätzliche Systemautorität ableiten.
8. Fehler MÜSSEN an der Compatibility-Grenze kontrolliert behandelt werden.
9. Native und Legacy Provider MÜSSEN für denselben Capability-Typ parallel existieren können.
10. Das Entfernen eines Legacy Providers DARF den Capability Contract nicht verändern.

## Konsequenzen

### Positiv

- vorhandene Legacy-Funktionen können systemweit wiederverwendet werden,
- native Anwendungen müssen keine Legacy-APIs kennen,
- Legacy-Software kann schrittweise durch native Provider ersetzt werden,
- Capability-basierte Komposition bleibt erhalten.

### Negativ

- Adapter verursachen zusätzlichen Implementierungsaufwand,
- Übersetzung kann Performancekosten erzeugen,
- Qualität und Zuverlässigkeit bleiben teilweise vom Legacy Provider abhängig.

## Abhängigkeiten

- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0007_Composition`
- `ADR-CAPABILITY-0009_Delegation`
- `ADR-CAPABILITY-0010_Attenuation`
- `ADR-EXECUTION-0010_Automatic_Execution_Planning`
- `ADR-SEMANTIC-0001`

## Ergebnis

Legacy-Anwendungen können in NovaOS Funktionen bereitstellen, ohne Teil der nativen Architektur zu werden:

```text
Legacy Application
       ↓
Compatibility Adapter
       ↓
Nova Capability Provider
       ↓
Native Nova Ecosystem
```

Damit können bestehende Programme als austauschbare Funktionsanbieter genutzt und später durch native Provider ersetzt werden.