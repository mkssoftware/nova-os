# NPSPEC-PROGRAM-MANIFEST-0001 – Nova Program Manifest

## Status

Angenommen

## Kategorie

Program / Manifest

## Zweck

NovaOS definiert das Program Manifest als deklarative Beschreibung eines klassischen Program Packages.

Das Manifest beschreibt Identität, Version, Einstiegspunkte, Abhängigkeiten und benötigte Capabilities eines Programms.

## Grundprinzipien

```text
Manifest ≠ Program
Manifest ≠ Package Content
Manifest ≠ Permission
Manifest ≠ Authority
ProgramID ≠ Installation Path
```

Das Manifest ist Bestandteil des Program Packages und wird vor Installation und Ausführung validiert.

## Manifest-Modell

```text
ProgramManifest
├── ManifestVersion
├── ProgramID
├── Name
├── Version
├── Publisher
├── EntryPoints
├── Resources
├── Dependencies
├── CapabilityRequirements
└── Compatibility
```

Optionale Erweiterungen dürfen ergänzt werden, sofern unbekannte Felder kontrolliert behandelt werden.

## Identität

`ProgramID` bildet die stabile Identität des Programms.

```text
ProgramID ≠ Program Name
ProgramID ≠ Package Path
ProgramID ≠ ProcessID
```

Updates desselben Programms behalten grundsätzlich dieselbe `ProgramID`.

## Einstiegspunkte

Das Manifest definiert die ausführbaren Einstiegspunkte des Programms.

Beispiel:

```text
EntryPoints
├── Main
├── Background
└── FileHandler
```

NovaOS startet ausschließlich gültige und registrierte Einstiegspunkte.

## Abhängigkeiten

Das Manifest kann benötigte Abhängigkeiten deklarieren.

Private Abhängigkeiten können aus dem programmspezifischen `SYS`-Bereich bereitgestellt werden:

```text
Program Manifest
      ↓
Dependency Resolution
      ↓
Private SYS Overlay
      ↓
Effective Program Environment
```

Private Abhängigkeiten erzeugen keine Änderung am globalen `/System`.

## Capabilities

Benötigte Systemzugriffe werden als Capability-Anforderungen deklariert.

```text
Capability Requirement
        ↓
Permission Evaluation
        ↓
Granted Capability
```

Die Deklaration einer Capability im Manifest erzeugt keine Authority.

## Version und Kompatibilität

Das Manifest muss Programm- und Manifest-Version unterscheiden.

Kompatibilitätsanforderungen können beispielsweise NovaOS-Version, Architektur oder benötigte Systemfunktionen beschreiben.

Nicht erfüllte harte Anforderungen verhindern die Ausführung.

## Integrität und Vertrauen

Das Manifest muss eindeutig zum Program Package gehören und in dessen Integritäts- und Vertrauensprüfung einbezogen werden.

Sicherheitsrelevante Änderungen am Manifest müssen erkannt werden können.

## Normative Anforderungen

1. Jedes Program Package MUSS ein gültiges Program Manifest besitzen.
2. Das Manifest MUSS eine stabile `ProgramID` enthalten.
3. `ProgramID` DARF NICHT vom Installationspfad abhängen.
4. Manifest-Version und Programmversion MÜSSEN getrennt behandelt werden.
5. Ausführbare Einstiegspunkte MÜSSEN deklarierbar sein.
6. Private Abhängigkeiten MÜSSEN deklarierbar sein.
7. Capability-Anforderungen MÜSSEN deklarierbar sein.
8. Capability-Deklarationen DÜRFEN keine Authority erzeugen.
9. Sicherheitsrelevante Manifest-Änderungen MÜSSEN erkennbar sein.
10. Das Manifest MUSS vor Installation und Ausführung validierbar sein.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

Das Program Manifest stellt die stabile und validierbare Beschreibung eines klassischen NovaOS-Programms bereit. NovaOS kann damit Identität, Einstiegspunkte, Abhängigkeiten, Kompatibilität und Capability-Anforderungen eines Program Packages eindeutig bestimmen.