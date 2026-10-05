# NPSPEC-SYSTEM-BOOT-0001 – Nova System Boot Integration

## Status

Angenommen

## Kategorie

System / Boot

## Zweck

NovaOS definiert die Schnittstelle zwischen Bootumgebung und laufendem System.

Der Bootprozess lädt, validiert und übergibt die für den Systemstart erforderlichen Ressourcen, ohne `/Boot` und `/System` zu einem gemeinsamen Bereich zu vermischen.

## Grundprinzipien

```text
/Boot ≠ /System
Boot Environment ≠ Running System
Boot Authority ≠ Runtime Authority
Boot Resource ≠ System Resource
Handoff ≠ Shared Ownership
```

## Architektur

```text
Firmware
   ↓
Bootloader
   ↓
Boot Validation
   ↓
Kernel Load
   ↓
Boot Information Block
   ↓
Kernel Entry
   ↓
System Initialization
   ↓
Running NovaOS
```

Die Bootumgebung endet kontrolliert mit der Übergabe an das laufende System.

## Boot-Ressourcen

`/Boot` kann insbesondere enthalten:

```text
Bootloader
Kernel Images
Recovery Components
Boot Configuration
Verification Metadata
Boot Assets
```

Systemweite Laufzeitkomponenten gehören grundsätzlich nach `/System`.

## Systemübergabe

Der Bootloader übergibt dem Kernel ausschließlich definierte Informationen.

Dazu können gehören:

```text
Memory Map
Framebuffer Information
Firmware Information
Boot Device
Boot Volume
Loaded Modules
Verification State
Boot State
Command Line
```

Die Übergabe erfolgt über eine versionierte Boot-Schnittstelle.

## Identität

Bootpfade sind keine dauerhafte Identität von Systemkomponenten.

```text
Boot Resource
     ↓
Validated Identity
     ↓
System Resource
```

Nach dem Systemstart werden stabile NovaOS-Identitäten verwendet.

## Verifikation

Vor der Übergabe können sicherheitskritische Komponenten geprüft werden:

```text
Load
 ↓
Integrity Check
 ↓
Trust / Signature Check
 ↓
Verification
 ↓
Handoff
```

Fehler müssen einen kontrollierten Übergang in Bootkonsole, Recovery oder einen definierten Fehlerzustand ermöglichen.

## Boot-Temp

Temporäre Boot-Ressourcen bleiben vom persistenten `/Boot` getrennt.

Nur ausdrücklich übergebene Ressourcen dürfen nach Abschluss der Bootphase weiterbestehen.

## Berechtigungen

Privilegien der Bootumgebung werden nicht automatisch als Runtime-Authority übernommen.

Nach Kernelstart gelten die normalen NovaOS-Sicherheits-, Capability- und Policy-Regeln.

## Normative Anforderungen

1. `/Boot` und `/System` MÜSSEN logisch getrennte Bereiche bleiben.
2. Die Bootumgebung MUSS eine definierte Schnittstelle zum Kernel besitzen.
3. Bootinformationen MÜSSEN strukturiert und versionierbar übergeben werden.
4. Der Kernel MUSS übergebene Bootinformationen validieren können.
5. Bootpfade DÜRFEN nicht als dauerhafte Systemidentität verwendet werden.
6. Sicherheitskritische Bootkomponenten MÜSSEN vor Verwendung prüfbar sein.
7. Bootfehler MÜSSEN kontrolliert diagnostizierbar sein.
8. Boot-Temp-Ressourcen DÜRFEN nicht automatisch persistent werden.
9. Boot-Authority DARF nicht automatisch zur Runtime-Authority werden.
10. Nach erfolgreicher Übergabe MUSS NovaOS die Kontrolle über Ressourcen und Policies übernehmen.

## Abhängigkeiten

- `NPSPEC-SYSTEM-LAYOUT-0001`
- `NPSPEC-TEMP-BOOT-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-RECOVERY-0001`

## Ergebnis

NovaOS besitzt eine klare Grenze zwischen Bootumgebung und laufendem System. Der Bootprozess lädt und validiert die notwendigen Komponenten, übergibt einen definierten Systemzustand an den Kernel und gibt anschließend die Kontrolle vollständig an die NovaOS-Systemarchitektur ab.