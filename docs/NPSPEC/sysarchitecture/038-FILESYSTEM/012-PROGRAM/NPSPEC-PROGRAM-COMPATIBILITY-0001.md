# NPSPEC-PROGRAM-COMPATIBILITY-0001 – Nova Program Compatibility

## Status

Angenommen

## Kategorie

Program / Compatibility

## Zweck

NovaOS definiert ein einheitliches Modell zur Prüfung, ob ein Program Package in der aktuellen Systemumgebung ausführbar ist.

Kompatibilität wird vor Installation, Update und Ausführung anhand deklarierter Anforderungen bestimmt.

## Grundprinzipien

```text
Compatibility ≠ Trust
Compatibility ≠ Permission
Compatible ≠ Installed
Compatible ≠ Authorized
Compatibility Requirement ≠ Dependency
```

## Kompatibilitätsmodell

Ein Program Manifest kann Anforderungen definieren:

```text
Compatibility
├── NovaOS Version
├── Architecture
├── ABI
├── Runtime
├── Required Features
├── Required Interfaces
└── Compatibility Provider
```

Anforderungen können verpflichtend oder optional sein.

## Prüfung

NovaOS vergleicht die Programmanforderungen mit der tatsächlichen Systemumgebung:

```text
Program Requirements
        ↓
System Capabilities
        ↓
Compatibility Evaluation
        ↓
Result
```

Mögliche Ergebnisse:

```text
Compatible
CompatibleWithProvider
PartiallyCompatible
Incompatible
Unknown
```

`Unknown` darf nicht automatisch als `Compatible` behandelt werden.

## Native Programme

Native Programme können Anforderungen an NovaOS-Version, Architektur, ABI, Runtime oder Systemfunktionen deklarieren.

Nicht erfüllte zwingende Anforderungen verhindern die reguläre Ausführung.

## Legacy-Programme

Nicht native Programme können über einen Compatibility Provider unterstützt werden:

```text
Legacy Program
      ↓
Compatibility Provider
      ↓
NovaOS
```

Die Verfügbarkeit eines Providers kann damit Bestandteil der Kompatibilitätsbewertung sein.

Der Provider verändert nicht die Identität des Programms.

## Abhängigkeiten

Dependency-Auflösung und Compatibility-Prüfung sind getrennte Schritte.

```text
Compatibility
      ↓
Dependency Resolution
      ↓
Execution Environment
```

Eine vorhandene Dependency garantiert nicht automatisch die Kompatibilität des gesamten Programms.

## Updates

Kompatibilität muss bei Programmupdates erneut bewertet werden.

Ebenso können NovaOS-, Runtime-, ABI- oder Provider-Updates eine erneute Bewertung installierter Programme auslösen.

## Introspection

NovaOS soll den Grund einer Inkompatibilität darstellen können.

Beispiele:

```text
Unsupported Architecture
Missing Runtime
Unsupported ABI
Missing Feature
Missing Compatibility Provider
Unsupported NovaOS Version
```

## Sicherheit

Kompatibilität erzeugt keine Authority.

Ein kompatibles Programm muss weiterhin Trust-, Permission- und Capability-Prüfungen durchlaufen.

## Normative Anforderungen

1. Program Packages MÜSSEN Kompatibilitätsanforderungen deklarieren können.
2. NovaOS MUSS zwingende Anforderungen vor der Ausführung prüfen können.
3. Kompatibilitätsprüfung MUSS von Trust und Permissions getrennt bleiben.
4. `Unknown` DARF NICHT automatisch als kompatibel gelten.
5. Native Programme MÜSSEN Architektur-, ABI- und Runtime-Anforderungen deklarieren können.
6. Legacy-Programme DÜRFEN einen Compatibility Provider voraussetzen.
7. Fehlende zwingende Anforderungen MÜSSEN erkannt werden.
8. Updates MÜSSEN eine erneute Kompatibilitätsprüfung auslösen können.
9. Der Grund einer Inkompatibilität SOLL introspektierbar sein.
10. Kompatibilität DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-DEPENDENCY-0002`
- `NPSPEC-PROGRAM-INSTALL-0001`
- `NPSPEC-PROGRAM-UPDATE-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-PROGRAM-PERMISSION-0001`
- `NPSPEC-PROGRAM-TRUST-0001`

## Ergebnis

NovaOS kann vor Installation und Ausführung eindeutig bestimmen, ob die technischen Anforderungen eines Programms erfüllt sind. Native und Legacy-Programme werden über dasselbe grundlegende Kompatibilitätsmodell bewertet, ohne Kompatibilität mit Vertrauen oder Berechtigungen gleichzusetzen.