# NPSPEC-PROGRAM-LEGACY-0001 – Nova Program Legacy Compatibility

## Status

Angenommen

## Kategorie

Program / Compatibility / Legacy

## Zweck

NovaOS definiert die kontrollierte Ausführung klassischer Programme, die nicht nativ für NovaOS entwickelt wurden.

Legacy-Kompatibilität wird als isolierte Kompatibilitätsschicht behandelt und darf die native NovaOS-Architektur nicht bestimmen oder deren Sicherheitsmodell umgehen.

## Grundprinzipien

```text
Legacy Program ≠ Native Nova Program
Compatibility ≠ Native Integration
Compatibility ≠ Trust
Legacy API ≠ NovaOS API
Compatibility Layer ≠ Security Bypass
```

## Legacy-Modell

Legacy-Programme werden über einen passenden Compatibility Provider ausgeführt:

```text
Legacy Program
      ↓
Compatibility Provider
      ↓
NovaOS Services
      ↓
Kernel / System
```

Mögliche Provider können unterschiedliche fremde Laufzeit- oder Betriebssystemumgebungen abbilden.

## Program Package

Legacy-Programme dürfen weiterhin als NovaOS Program Package verwaltet werden:

```text
/Apps/<Program>/
├── App/
├── Resources/
├── SYS/
└── Manifest
```

Die ursprünglichen Legacy-Dateien verbleiben innerhalb des Programmbereichs.

## Legacy Environment

Der Compatibility Provider stellt die vom Programm erwartete Umgebung bereit.

Dazu können gehören:

```text
Legacy APIs
Runtime Libraries
Filesystem Mapping
Environment Variables
Registry-like State
Process Semantics
Graphics Interfaces
```

Diese Umgebung wird auf kontrollierte NovaOS-Mechanismen abgebildet.

## Namespace

Legacy-Pfade werden in den Program Namespace übersetzt.

Beispielsweise darf eine erwartete fremde Verzeichnisstruktur als virtuelle oder projizierte Sicht bereitgestellt werden.

```text
Legacy Path
     ↓
Compatibility Mapping
     ↓
Program Namespace
     ↓
ObjectID
```

Die interne NovaOS-Struktur muss dafür nicht verändert werden.

## Abhängigkeiten

Legacy-Abhängigkeiten sollen bevorzugt programmspezifisch bereitgestellt werden.

Sie dürfen den privaten `SYS`-Bereich und den SYS Overlay verwenden, sofern dies mit dem jeweiligen Compatibility Provider vereinbar ist.

## Sicherheit

Legacy-Programme laufen innerhalb eines begrenzten NovaOS-Sicherheitskontexts.

```text
Legacy Request
      ↓
Compatibility Provider
      ↓
Capability Check
      ↓
NovaOS Resource
```

Legacy-APIs dürfen keine NovaOS-Capability-Prüfung umgehen.

Ein Legacy-Programm erhält nicht automatisch die Authority, die es auf seinem ursprünglichen Betriebssystem erwartet hätte.

## Isolation

Nicht vertrauenswürdige oder technisch unsichere Legacy-Komponenten können zusätzlich in einem stärker isolierten Compatibility Domain ausgeführt werden.

Fehler innerhalb dieser Umgebung sollen native NovaOS-Komponenten möglichst nicht beeinträchtigen.

## Normative Anforderungen

1. NovaOS DARF Legacy-Programme über austauschbare Compatibility Provider ausführen.
2. Legacy-Kompatibilität DARF die native NovaOS-Architektur nicht bestimmen.
3. Legacy-Programme MÜSSEN in einen NovaOS-Sicherheitskontext eingebunden werden.
4. Legacy-APIs DÜRFEN Capability-Prüfungen nicht umgehen.
5. Legacy-Pfade SOLLEN über Namespace-Mappings abgebildet werden.
6. Legacy-Kompatibilität DARF keine Änderung der nativen Namespace-Struktur erfordern.
7. Legacy-Abhängigkeiten SOLLEN programmspezifisch isoliert werden.
8. Unterschiedliche Compatibility Provider MÜSSEN parallel existieren können.
9. Unsichere Legacy-Komponenten MÜSSEN zusätzlich isolierbar sein.
10. Legacy-Unterstützung DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-PROGRAM-PERMISSION-0001`
- `NPSPEC-PROGRAM-TRUST-0001`

## Ergebnis

NovaOS kann klassische Fremdprogramme über isolierte Compatibility Provider ausführen, ohne deren historische Architektur in das native System zu übernehmen. Legacy-APIs, Pfade und Abhängigkeiten werden kontrolliert auf NovaOS-Mechanismen abgebildet und bleiben vollständig dem Capability-, Namespace- und Sicherheitsmodell von NovaOS untergeordnet.