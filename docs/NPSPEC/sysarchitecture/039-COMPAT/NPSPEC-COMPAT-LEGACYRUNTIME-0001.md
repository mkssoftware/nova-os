# NPSPEC-COMPAT-LEGACYRUNTIME-0001 – Nova Legacy Runtime

## Status

Angenommen

## Kategorie

Compatibility / Legacy Runtime

## Zweck

NovaOS definiert eine isolierte Laufzeitumgebung für ältere Software, deren ursprüngliche Runtime, Bibliotheken oder Systemannahmen von der nativen NovaOS-Umgebung nicht mehr bereitgestellt werden.

Legacy Runtimes ermöglichen langfristige Softwarekompatibilität, ohne veraltete Komponenten dauerhaft in die native NovaOS-Systemarchitektur zu übernehmen.

## Grundprinzipien

```text
Legacy Runtime ≠ Native Runtime
Legacy Library ≠ System Library
Legacy Compatibility ≠ Trust
Legacy Privilege ≠ Nova Authority
Runtime Isolation ≠ Virtual Machine
Old Software ≠ Old System Architecture
```

## Modell

```text
Legacy Application
       ↓
Compatibility Personality
       ↓
Legacy Runtime
├── Runtime Libraries
├── Frameworks
├── Compatibility APIs
├── Runtime State
└── Dependencies
       ↓
Nova Compatibility Layer
       ↓
NovaOS
```

## Runtime-Profil

Eine Legacy Runtime wird mindestens beschrieben durch:

```text
LegacyRuntime
├── RuntimeID
├── RuntimeVersion
├── TargetPlatform
├── Architecture
├── ABIRequirements
├── Dependencies
├── CompatibilityProfile
└── SecurityProfile
```

Mehrere Versionen derselben Runtime dürfen parallel vorhanden sein.

## Isolation

Legacy-Komponenten werden von nativen NovaOS-Komponenten getrennt gehalten:

```text
Legacy Application
├── Private Runtime
├── Private Libraries
├── Private Configuration
└── Runtime State
```

Veraltete Bibliotheken dürfen native NovaOS-Systembibliotheken nicht ersetzen.

## Abhängigkeiten

Legacy-Anwendungen dürfen exakt die Runtime- und Bibliotheksversionen erhalten, die sie benötigen.

```text
Application
    ↓
Runtime Requirement
    ↓
Compatible Legacy Runtime
    ↓
Private Dependencies
```

Unterschiedliche Anwendungen dürfen dadurch inkompatible Versionen parallel verwenden.

## Compatibility Integration

Eine Legacy Runtime darf weitere Compatibility-Komponenten verwenden:

```text
Legacy Runtime
├── ABI Translation
├── API Translation
├── Syscall Translation
├── Binary Translation
├── CPU Emulation
└── Hardware Emulation
```

Nur die tatsächlich benötigten Schichten werden aktiviert.

## Systemzugriff

Legacy-Anwendungen erhalten keinen direkten Zugriff auf native NovaOS-Systemressourcen.

```text
Legacy Operation
       ↓
Compatibility Provider
       ↓
Capability / Policy Check
       ↓
Authorized Nova Operation
```

Die Runtime selbst erzeugt keine Authority.

## Veraltete Sicherheitsmodelle

Historische Privileg-, Benutzer- oder Sicherheitsmodelle werden ausschließlich innerhalb der Compatibility-Umgebung interpretiert.

Sie dürfen das native NovaOS-Capability- und Policy-Modell nicht ersetzen oder umgehen.

## Lebenszyklus

Legacy Runtimes können unabhängig vom Basissystem:

```text
Installiert
Versioniert
Aktiviert
Deaktiviert
Aktualisiert
Archiviert
Entfernt
```

werden.

Eine nicht mehr benötigte Runtime darf entfernt werden, ohne die native NovaOS-Runtime zu verändern.

## Sicherheit

Bekannte unsichere oder nicht mehr gepflegte Runtimes dürfen mit stärkeren Einschränkungen ausgeführt werden:

```text
Reduced Capabilities
Stronger Isolation
Network Restrictions
Read-only Resources
Resource Limits
Execution Restrictions
```

Kompatibilität hat keinen Vorrang vor zwingenden Sicherheitsanforderungen.

## Normative Anforderungen

1. Legacy Runtimes MÜSSEN von der nativen NovaOS-Runtime getrennt bleiben.
2. Jede Legacy Runtime MUSS eindeutig identifizierbar und versionierbar sein.
3. Mehrere Runtime-Versionen MÜSSEN parallel unterstützt werden können.
4. Legacy-Bibliotheken DÜRFEN native NovaOS-Systembibliotheken nicht ersetzen.
5. Runtime-Abhängigkeiten MÜSSEN isoliert bereitgestellt werden können.
6. Legacy Runtimes DÜRFEN keine zusätzliche Authority erzeugen.
7. Geschützte Systemzugriffe MÜSSEN Capability- und Policy-Prüfungen durchlaufen.
8. Veraltete Privilegmodelle DÜRFEN NovaOS-Sicherheitsgrenzen nicht umgehen.
9. Zusätzliche Compatibility-Schichten DÜRFEN bedarfsgerecht kombiniert werden.
10. Unsichere Runtimes MÜSSEN stärker eingeschränkt werden können.
11. Legacy Runtimes MÜSSEN unabhängig vom nativen System aktualisier- und entfernbar sein.
12. RuntimeID, Version, Abhängigkeiten, Isolation und Compatibility-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-API-0001`
- `NPSPEC-COMPAT-SYSCALL-0001`
- `NPSPEC-COMPAT-ABITRANSLATION-0001`
- `NPSPEC-COMPAT-BINARYTRANSLATION-0001`
- `NPSPEC-COMPAT-CPUEMULATION-0001`
- `NPSPEC-COMPAT-HARDWAREEMULATION-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS kann ältere Laufzeitumgebungen langfristig und isoliert bereitstellen, ohne veraltete Frameworks, Bibliotheken oder Sicherheitsmodelle in das native System zu übernehmen. Anwendungen erhalten gezielt ihre benötigte Legacy Runtime, während NovaOS-Capabilities, Policies, Isolation und native Systemkomponenten maßgeblich bleiben.