# ADR-BOOT-0002 – UEFI bevorzugen mit BIOS Fallback

## Status

Angenommen

## Kategorie

Boot / Firmwareauswahl / UEFI / BIOS / Fallback / Plattformkompatibilität

## Kontext

NovaOS unterstützt gemäß `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung` sowohl moderne UEFI-Systeme als auch klassische Legacy-BIOS-Plattformen.

Damit stellt sich für Systeme, die mehrere Firmwarepfade anbieten können, die Frage, welcher Startpfad bevorzugt werden soll.

Insbesondere moderne x86-PCs können je nach Firmwarekonfiguration:

```text
Native UEFI
```

oder:

```text
Legacy BIOS / CSM
```

verwenden.

Eine gleichwertige Behandlung beider Wege ohne klare Präferenz würde:

- unnötige Legacy-Pfade fördern,
- moderne Firmwarefunktionen ungenutzt lassen,
- Tests und Support komplexer machen,
- Plattforminformationen verschlechtern,
- den Übergang weg von BIOS erschweren.

NovaOS soll BIOS weiterhin vollständig unterstützen, aber den nativen modernen Bootpfad bevorzugen.

---

## Entscheidung

NovaOS bevorzugt auf unterstützten Systemen grundsätzlich **native UEFI-Bootausführung**.

BIOS bleibt ein vollwertiger Fallback- und Kompatibilitätspfad.

Das Grundmodell lautet:

```text
Firmware Environment
        │
        ▼
Native UEFI available?
        │
   ┌────┴────┐
   │         │
  Yes        No
   │         │
   ▼         ▼
 UEFI       BIOS
   │         │
   └────┬────┘
        ▼
Nova Boot Core
        │
        ▼
Common Kernel Handoff
```

Die Präferenz betrifft ausschließlich die Auswahl des Firmware-Einstiegspfades.

Nach der frühen Bootphase konvergieren beide Wege weiterhin in dieselbe gemeinsame NovaOS-Bootarchitektur.

---

## Grundprinzip

Es gilt:

```text
Prefer the native modern firmware path.

Keep the legacy path available
when it is actually needed.
```

und:

```text
UEFI is preferred.

BIOS is supported.

Neither defines the NovaOS kernel.
```

---

## Bedeutung von „bevorzugen“

UEFI bevorzugen bedeutet:

Wenn NovaOS auf einer Plattform sowohl über native UEFI-Firmware als auch über einen Legacy-/Compatibility-Pfad gestartet werden kann, soll die Installation und Bootkonfiguration standardmäßig UEFI verwenden.

Dies bedeutet nicht:

```text
BIOS deprecated
```

oder:

```text
BIOS unsupported
```

BIOS bleibt ein offiziell unterstützter Bootpfad für Plattformen, die kein natives UEFI besitzen oder auf denen UEFI aus berechtigten Gründen nicht verwendet werden kann.

---

## Native UEFI

Als bevorzugter Pfad gilt nur ein nativer UEFI-Start.

Beispiel:

```text
Firmware
    ↓
UEFI Boot Manager
    ↓
NovaOS EFI Loader
```

Ein Legacy-Boot über Compatibility Support Module zählt als BIOS-/Legacy-Pfad.

---

## Kein CSM als bevorzugte Lösung

Wenn sowohl:

```text
Native UEFI
```

als auch:

```text
CSM / Legacy
```

verfügbar sind, wird native UEFI-Ausführung bevorzugt.

Das Modell lautet:

```text
Native UEFI
    ↓ preferred

CSM / Legacy BIOS
    ↓ fallback
```

---

## Gründe für die UEFI-Präferenz

UEFI bietet gegenüber klassischem BIOS eine bessere langfristige Grundlage für moderne Systeme.

Relevante Vorteile sind unter anderem:

- standardisierte Executables,
- moderne Firmwareprotokolle,
- native GPT-Integration,
- GOP für Grafik,
- strukturierte Memory Maps,
- Configuration Tables,
- bessere Integration in moderne Plattformfirmware,
- Unterstützung von Secure Boot,
- besserer Übergang zu zukünftiger PC-Hardware.

Diese Vorteile rechtfertigen die Präferenz, ohne BIOS-Kompatibilität aufzugeben.

---

## Keine UEFI-Abhängigkeit des Kernels

Die UEFI-Präferenz darf nicht dazu führen, dass höhere Kernelkomponenten UEFI als zwingende Voraussetzung behandeln.

Nach Kernelübergabe gilt weiterhin:

```text
Firmware
    ↓
Platform Provider
    ↓
Normalized Platform Model
```

Der Kernel verwendet semantische Plattforminformationen.

---

## Bootloader-Auswahl

Bei einer NovaOS-Installation auf einem UEFI-System soll standardmäßig ein nativer EFI-Bootloader eingerichtet werden.

Beispiel:

```text
EFI System Partition
        │
        ▼
NovaOS EFI Loader
```

BIOS-kompatible Bootstrukturen dürfen zusätzlich installiert werden, wenn dies explizit gewünscht oder für das Zielsystem sinnvoll ist.

---

## Installationsmodus

Die Installationsumgebung soll erkennen, über welchen Firmwarepfad sie selbst gestartet wurde.

Beispiel:

```text
Installer Booted via UEFI
        ↓
Prefer UEFI installation
```

oder:

```text
Installer Booted via BIOS
        ↓
Install BIOS boot path
```

Diese Erkennung ist ein Default und keine unüberwindbare Einschränkung.

---

## Explizite Auswahl

Fortgeschrittene Benutzer oder Deployment-Systeme dürfen einen bestimmten Bootpfad explizit wählen.

Beispiel:

```text
BootMode = UEFI
```

oder:

```text
BootMode = BIOS
```

Eine explizite Auswahl muss jedoch auf der Zielhardware technisch unterstützt sein.

---

## Automatic Mode

Der Standardmodus lautet konzeptionell:

```text
BootMode = Auto
```

Dabei gilt:

```text
UEFI available and valid
        ↓
UEFI

otherwise
        ↓
BIOS
```

Die genaue Detection findet vor beziehungsweise während der Installation und beim Firmwarestart statt.

---

## BIOS Fallback

BIOS wird verwendet, wenn mindestens einer der folgenden Fälle zutrifft:

- keine UEFI-Firmware vorhanden,
- NovaOS wird tatsächlich über Legacy BIOS gestartet,
- native UEFI-Ausführung ist auf der Plattform nicht funktionsfähig,
- administrativ explizit BIOS gewählt,
- spezielle Legacy-Hardware erfordert den BIOS-Pfad.

Der Fallback ist ein unterstützter Betriebsmodus und kein Fehlerzustand an sich.

---

## Kein automatischer Runtime-Wechsel

UEFI und BIOS sind Firmware Entry Paths.

Ein laufender Bootvorgang kann nicht beliebig nachträglich von UEFI zu BIOS wechseln.

Fallback bedeutet deshalb primär:

```text
select appropriate boot path
```

vor dem jeweiligen Firmwarestart beziehungsweise im Rahmen der Bootkonfiguration.

Es bedeutet nicht:

```text
UEFI loader failed
    ↓
switch CPU into BIOS firmware
```

innerhalb desselben gestarteten Firmwarepfades.

---

## Fallback bei beschädigtem UEFI-Eintrag

Wenn ein UEFI Boot Entry beschädigt oder verloren ist, dürfen Recovery-Mechanismen:

- den EFI-Loader erneut registrieren,
- einen Fallback-EFI-Pfad nutzen,
- die Bootkonfiguration reparieren.

Wenn zusätzlich ein installierter BIOS-Pfad vorhanden und die Hardware Legacy-Boot unterstützt, kann dieser ebenfalls als äußerer Recoverypfad dienen.

---

## UEFI Removable Media Path

NovaOS soll für geeignete Installations- und Recoverymedien den standardisierten UEFI-Fallbackpfad unterstützen können.

Dies ermöglicht den Start auch ohne vorher registrierten Firmware-Boot-Eintrag.

Die konkrete Dateipfadkonvention wird in einer NPSPEC festgelegt.

---

## Bootpriorität in Dual-Mode-Installationen

Wenn eine Installation sowohl UEFI- als auch BIOS-fähig eingerichtet wurde, soll die gewünschte Priorität lauten:

```text
1. Native UEFI
2. Legacy BIOS
```

Die tatsächliche Auswahl kann letztlich auch von Firmwarekonfiguration und Bootreihenfolge abhängen.

NovaOS soll seine eigenen Bootartefakte entsprechend vorbereiten.

---

## GPT

Die UEFI-Präferenz korrespondiert mit GPT als bevorzugtem modernen Partitionsmodell.

Es gilt jedoch:

```text
UEFI ≠ mandatory GPT-only architecture
```

und ebenso:

```text
BIOS ≠ MBR-only architecture
```

NovaOS kann BIOS auf GPT-basierten Datenträgern unterstützen.

---

## EFI System Partition

Auf UEFI-Installationen wird eine EFI System Partition gemäß den dafür geltenden Plattformanforderungen verwendet.

Die ESP ist Bootinfrastruktur und nicht das native NovaOS-Systemvolume.

Kernel, Systemzustand und höhere NovaOS-Strukturen sollen nicht architektonisch von der ESP abhängen.

---

## Bootdateien

UEFI-spezifische Dateien und BIOS-spezifische Bootsektoren sind unterschiedliche technische Darstellungen derselben höheren NovaOS-Bootarchitektur.

Beispiel:

```text
UEFI:
    EFI executable

BIOS:
    Stage 1 + Stage 2

Both:
    Shared Nova Boot Core
```

---

## Gemeinsamer Boot Core

Die UEFI-Präferenz darf nicht zu einer Abspaltung der BIOS-Implementierung führen.

Beide Pfade verwenden weiterhin möglichst viel gemeinsamen Code für:

- Kernel Loading,
- Signaturprüfung,
- Bootkonfiguration,
- Recovery,
- Bootmenü,
- Bootressourcen,
- Kernel Handoff.

---

## Feature-Parität

UEFI darf zusätzliche Firmwarefunktionen bereitstellen.

Das bedeutet nicht, dass BIOS jede UEFI-Funktion emulieren muss.

Erforderlich ist Parität bei den wesentlichen NovaOS-Bootfunktionen:

```text
Boot NovaOS
Verify kernel
Select recovery
Start NovaDOS
Provide BootInfo
Handoff to kernel
```

---

## Secure Boot

Wenn UEFI Secure Boot verfügbar und gemäß NovaOS-Policy aktiviert ist, darf es als zusätzliche Vertrauensschicht genutzt werden.

Die UEFI-Präferenz ist jedoch nicht ausschließlich durch Secure Boot begründet.

NovaOS Verified Boot bleibt firmwareunabhängig.

---

## Secure Boot nicht verfügbar

Ein UEFI-System ohne Secure Boot bleibt ein gültiges UEFI-Ziel.

Es gilt nicht:

```text
No Secure Boot
    ↓
Fallback to BIOS
```

Firmwarepfad und Vertrauensmechanismus sind getrennte Dimensionen.

---

## Secure Boot fehlerhaft

Ein Trust- oder Signaturfehler darf nicht automatisch durch BIOS-Boot umgangen werden.

Beispiel:

```text
UEFI boot rejected because kernel signature invalid
```

darf nicht führen zu:

```text
Try BIOS without verification
```

Dies wäre ein Sicherheitsdowngrade.

---

## Keine Security-Downgrades

Die Fallbacklogik darf niemals bedeuten:

```text
Secure path failed
    ↓
use weaker path
```

wenn dadurch ein Hard Security Contract verletzt würde.

Es gilt:

```text
Fallback may change mechanism.

Fallback must not weaken required trust.
```

---

## Firmwarefehler

Nicht jeder UEFI-Fehler erfordert BIOS-Fallback.

Zuerst können UEFI-spezifische Recoverymechanismen versucht werden, beispielsweise:

```text
recreate boot entry
use fallback EFI path
reload boot configuration
select known-good bootloader
```

BIOS wird nur verwendet, wenn es tatsächlich als separater unterstützter Bootpfad verfügbar ist.

---

## UEFI Runtime Services

Die Bevorzugung von UEFI bedeutet nicht, dass NovaOS zwingend UEFI Runtime Services verwenden muss.

Nach `ExitBootServices()` kann NovaOS weitgehend unabhängig von Firmwarediensten arbeiten.

Runtime Services bleiben optionale Plattformservices gemäß HAL- und Firmwarearchitektur.

---

## Boot Performance

UEFI- und BIOS-Pfade dürfen unterschiedliche Bootzeiten besitzen.

Die Auswahl darf jedoch nicht allein anhand einer minimalen Bootzeitdifferenz erfolgen.

Architekturkompatibilität und langfristige Plattformqualität haben Vorrang.

---

## Plattforminformationen

UEFI bietet häufig bessere oder strukturiertere initiale Firmwareinformationen.

Diese werden jedoch anschließend normalisiert.

Beispiel:

```text
UEFI Configuration Tables
        ↓
Platform Provider
        ↓
Platform Model
```

---

## Architekturidentität

UEFI-Präferenz gilt nur für eine kompatible Firmware-/Kernelarchitektur.

Ein vorhandener UEFI-Pfad darf nicht gewählt werden, wenn der verfügbare NovaOS-Bootloader diesen Firmwaremodus nicht korrekt unterstützt.

---

## 32-Bit-UEFI und 64-Bit-Kernel

Sonderfälle wie:

```text
32-bit UEFI firmware
64-bit CPU
64-bit kernel
```

müssen explizit unterstützt oder abgelehnt werden.

Das bloße Vorhandensein von UEFI reicht nicht als Auswahlkriterium.

Der UEFI-Pfad muss für die konkrete Kombination kompatibel sein.

---

## Plattformvalidierung

Die automatische Auswahl berücksichtigt mindestens:

```text
Firmware capability
Bootloader compatibility
Architecture compatibility
Required boot services
Trust policy
```

Erst dann gilt ein UEFI-Pfad als tatsächlich verwendbar.

---

## Fallback-Kriterien

Konzeptionell:

```text
CanUseUEFI =
    UEFI present
    AND compatible
    AND boot artifacts available
    AND required trust policy satisfiable
```

Wenn `CanUseUEFI = false`, kann BIOS geprüft werden.

---

## BIOS-Kompatibilität

Analog gilt:

```text
CanUseBIOS =
    BIOS or legacy firmware path present
    AND architecture supported
    AND BIOS boot artifacts available
    AND required trust policy satisfiable
```

Kann keiner der Pfade alle Hard Requirements erfüllen, muss der Bootvorgang fehlschlagen oder Recovery anbieten.

---

## Kein „irgendwie booten“

Es gilt ausdrücklich nicht:

```text
UEFI failed
    ↓
BIOS failed
    ↓
disable checks until something boots
```

Stattdessen:

```text
No valid boot path
    ↓
Explicit boot failure / recovery
```

---

## Boot Policy

Die Auswahl kann als Boot Policy modelliert werden.

Beispiel:

```text
FirmwarePreference {
    preferred = UEFI
    fallback = BIOS
    securityDowngrade = Forbidden
}
```

Die Firmwareauswahl ist damit explizit statt über verstreute Sonderfälle implementiert.

---

## Recoverymedien

NovaOS-Installations- und Recoverymedien sollen nach Möglichkeit sowohl UEFI als auch BIOS unterstützen.

Das Medium kann damit auf:

```text
modern UEFI machine
```

und:

```text
legacy BIOS machine
```

starten.

Auch hier wird auf Dual-Mode-Systemen UEFI bevorzugt.

---

## NovaDOS

NovaDOS bleibt über beide Pfade erreichbar.

Die Auswahl des Firmwarepfades darf nicht beeinflussen, welche semantischen Recoveryfunktionen NovaDOS bereitstellt.

---

## Known-Good Boot

Wenn eine aktuelle UEFI-Bootloadergeneration fehlerhaft ist, soll zunächst ein Known-Good-UEFI-Artefakt verwendet werden können.

Nicht jeder Bootloaderfehler erfordert deshalb einen Firmwarewechsel.

Konzeptionell:

```text
UEFI current
    ↓ failure
UEFI known-good
    ↓ failure
Recovery
```

Ein verfügbarer BIOS-Pfad kann zusätzlich existieren, aber nicht als Ersatz für verletzte Sicherheitsanforderungen.

---

## Updates

Updates müssen UEFI- und BIOS-Artefakte entsprechend der installierten Bootmodi verwalten.

Bei Dual-Mode-Installationen dürfen beide Artefaktsätze aktualisiert werden.

Die Aktivierung soll transaktional beziehungsweise Known-Good-fähig erfolgen.

---

## Firmware Migration

Ein bestehendes NovaOS-System darf von BIOS-Boot auf UEFI-Boot migriert werden, wenn:

- Hardware UEFI unterstützt,
- Disklayout kompatibel oder migrierbar ist,
- notwendige Bootartefakte erstellt werden,
- Firmwareeinträge konfiguriert werden,
- die neue Bootkette validiert wurde.

Ein vollständiger Neuaufbau des Betriebssystems soll dafür nicht grundsätzlich nötig sein.

---

## UEFI zu BIOS

Eine Migration von UEFI zu BIOS darf ebenfalls technisch möglich sein, wenn Hardware und Datenträgerlayout dies unterstützen.

Sie gilt jedoch nicht als bevorzugte Richtung.

---

## Architektur-Evolution

Die Präferenzregel muss evolvierbar bleiben.

Heute lautet sie:

```text
UEFI > BIOS
```

Sollte zukünftig ein neues Firmwaremodell UEFI ablösen, muss eine zukünftige ADR die Präferenzhierarchie erweitern können.

Das Kernprinzip bleibt:

```text
Prefer the best native supported platform path.

Keep compatible fallbacks where justified.
```

---

## Architecture Introspection

Der tatsächlich verwendete Firmwarepfad muss introspektierbar sein.

Beispiel:

```text
Firmware:
    UEFI

Selection Mode:
    Auto

Preferred Path:
    UEFI

Fallback Available:
    BIOS
```

oder:

```text
Firmware:
    BIOS

Reason:
    UEFI not available
```

---

## Selection Trace

Bei automatischer Auswahl soll die Entscheidung erklärbar sein.

Beispiel:

```text
UEFI:
    available
    compatible
    verified

Selected:
    UEFI
```

oder:

```text
UEFI:
    present
    incompatible firmware bitness

BIOS:
    compatible

Selected:
    BIOS
```

---

## Logging

Firmwareauswahl und relevante Fallbackgründe sollen im Bootlog erfasst werden.

Beispiele:

```text
Firmware preference: UEFI
UEFI path validated
UEFI selected
```

oder:

```text
UEFI unavailable
BIOS selected
```

---

## Deterministic Mode

Für reproduzierbare Tests oder definierte Deploymentumgebungen darf der Firmwarepfad explizit fixiert werden.

Beispiel:

```text
BootMode = UEFI
Fallback = Forbidden
```

Dies verhindert, dass ein Test unbemerkt über einen anderen Firmwarepfad startet.

---

## Testbarkeit

BIOS- und UEFI-Pfade müssen unabhängig getestet werden können.

Geeignete Testumgebungen umfassen:

```text
QEMU + BIOS firmware
QEMU + OVMF
physical BIOS hardware
physical UEFI hardware
```

Dual-Mode-Szenarien müssen ebenfalls geprüft werden.

---

## KI-Unabhängigkeit

Die Auswahl zwischen UEFI und BIOS muss vollständig deterministisch und ohne KI erfolgen können.

KI darf für Bootdiagnose eingesetzt werden, aber nicht benötigt werden, um den grundlegenden Firmwarepfad festzulegen.

---

## Normative Anforderungen

1. NovaOS MUSS auf kompatiblen Systemen native UEFI-Ausführung bevorzugen.
2. BIOS MUSS als unterstützter Fallback- und Kompatibilitätspfad erhalten bleiben.
3. Die UEFI-Präferenz DARF BIOS-Unterstützung NICHT entfernen.
4. Native UEFI-Ausführung MUSS gegenüber CSM-/Legacy-Boot bevorzugt werden.
5. NovaOS DARF NICHT von CSM als Voraussetzung für moderne Systeme abhängen.
6. Der Standardauswahlmodus SOLL automatische Firmwareauswahl unterstützen.
7. Im automatischen Modus MUSS ein gültiger nativer UEFI-Pfad vor BIOS bevorzugt werden.
8. UEFI DARF nur gewählt werden, wenn Firmware- und Architekturkombination unterstützt wird.
9. BIOS DARF als Fallback nur gewählt werden, wenn der BIOS-Pfad tatsächlich unterstützt wird.
10. Ein expliziter Benutzer- oder Deployment-Override MUSS möglich sein.
11. Ein expliziter Override DARF technische Hard Requirements NICHT umgehen.
12. UEFI- und BIOS-Pfade MÜSSEN in denselben Shared Boot Core konvergieren.
13. Der Kernel DARF durch diese Präferenz NICHT von UEFI abhängig werden.
14. UEFI- und BIOS-Pfade MÜSSEN dieselbe Kernel-Handoff-Semantik verwenden.
15. Installationen, die unter UEFI gestartet werden, SOLLEN standardmäßig UEFI-Boot konfigurieren.
16. Installationen, die unter BIOS gestartet werden, MÜSSEN BIOS-Boot konfigurieren können.
17. Dual-Mode-Installationen DÜRFEN beide Bootpfade installieren.
18. In Dual-Mode-Installationen SOLL UEFI Priorität erhalten.
19. UEFI-Installationen SOLLEN eine EFI System Partition verwenden.
20. Die ESP DARF NICHT als native NovaOS-Systemarchitektur missverstanden werden.
21. BIOS-Unterstützung DARF NovaOS NICHT auf MBR beschränken.
22. GPT MUSS mit dem bevorzugten UEFI-Modell kompatibel sein.
23. BIOS auf GPT SOLL weiterhin möglich sein.
24. UEFI Secure Boot DARF als zusätzliche Vertrauensschicht verwendet werden.
25. Fehlendes Secure Boot DARF NICHT automatisch BIOS-Fallback auslösen.
26. Ein Secure-Boot- oder Signaturfehler DARF NICHT durch unsichereren BIOS-Boot umgangen werden.
27. Firmware-Fallback DARF Hard Security Requirements NICHT abschwächen.
28. Verified Boot MUSS über beide Firmwarepfade möglich bleiben.
29. UEFI Runtime Services DÜRFEN NICHT Voraussetzung der UEFI-Präferenz sein.
30. Ein Fehler eines UEFI Boot Entry SOLL zunächst über UEFI-Recoverymechanismen reparierbar sein.
31. UEFI-Fallbackpfade für Recoverymedien SOLLEN unterstützt werden.
32. BIOS-Fallback DARF NICHT als dynamischer Runtime-Wechsel innerhalb eines bereits gestarteten UEFI-Bootvorgangs modelliert werden.
33. Der ausgewählte Firmwarepfad MUSS frühzeitig feststehen.
34. Wenn weder UEFI noch BIOS alle Hard Requirements erfüllen, MUSS NovaOS explizit fehlschlagen oder Recovery anbieten.
35. NovaOS DARF NICHT Sicherheitsprüfungen abschalten, nur um einen Fallback-Boot zu ermöglichen.
36. Firmwareauswahl SOLL als explizite Boot Policy modelliert werden.
37. Recoverymedien SOLLEN UEFI und BIOS parallel unterstützen können.
38. NovaDOS MUSS unabhängig von der Firmwareauswahl erreichbar bleiben.
39. Known-Good-Bootmechanismen SOLLEN innerhalb desselben Firmwarepfades bevorzugt werden können.
40. Bootupdates MÜSSEN installierte Firmwarepfade konsistent berücksichtigen.
41. Dual-Mode-Installationen SOLLEN UEFI- und BIOS-Artefakte gemeinsam aktualisieren können.
42. Eine Migration von BIOS zu UEFI SOLL ohne vollständige Neuinstallation möglich sein können.
43. Eine UEFI-zu-BIOS-Migration DARF unterstützt werden, ist aber nicht bevorzugt.
44. Firmwarebitness und Kernelarchitektur MÜSSEN bei der Auswahl berücksichtigt werden.
45. Ein formal vorhandener, aber inkompatibler UEFI-Pfad DARF NICHT bevorzugt werden.
46. Der tatsächlich verwendete Firmwarepfad MUSS introspektierbar sein.
47. Der Grund für einen BIOS-Fallback SOLL introspektierbar sein.
48. Firmwareauswahl SOLL im Bootlog protokolliert werden.
49. Deterministische Deployments MÜSSEN einen Firmwarepfad fixieren können.
50. BIOS- und UEFI-Pfade MÜSSEN separat testbar sein.
51. Dual-Mode-Auswahl MUSS testbar sein.
52. Firmwareauswahl MUSS ohne KI korrekt funktionieren.
53. KI DARF Firmwarediagnose unterstützen, aber die Basisauswahl NICHT ersetzen.
54. Die Präferenzhierarchie MUSS zukünftig um weitere Firmwaremodelle erweiterbar sein.
55. UEFI-Bevorzugung MUSS als Policy und nicht als Kernelmechanismus behandelt werden.
56. NovaOS MUSS moderne Firmware bevorzugen, ohne funktionierende Legacy-Kompatibilität unnötig aufzugeben.

---

## Konsequenzen

### Positive Konsequenzen

- moderne Systeme verwenden standardmäßig ihren nativen Firmwarepfad,
- Legacy-Kompatibilität bleibt erhalten,
- weniger Abhängigkeit von CSM,
- saubere Migration weg von BIOS,
- bessere Nutzung von GPT, GOP und modernen Firmwarestrukturen,
- Bootarchitektur bleibt trotzdem firmwareunabhängig,
- Dual-Mode-Medien und -Installationen bleiben möglich,
- Sicherheitsfallbacks bleiben kontrolliert.

### Negative Konsequenzen

- Dual-Mode-Installationen benötigen zusätzliche Pflege,
- unterschiedliche Firmwarekonfigurationen müssen erkannt werden,
- bestimmte Sonderfälle wie 32-Bit-UEFI auf 64-Bit-Hardware benötigen explizite Behandlung,
- BIOS-Fallback kann zusätzliche Bootartefakte erfordern,
- Firmware-Migration erhöht Installer- und Recovery-Komplexität.

---

## Verworfene Alternativen

### BIOS und UEFI ohne Präferenz gleich behandeln

Verworfen.

Auf modernen Systemen würde dadurch unnötig Legacy-Technik weiterverwendet.

### BIOS grundsätzlich bevorzugen

Verworfen.

Dies wäre für moderne Hardware langfristig die schlechtere Plattformstrategie.

### UEFI exklusiv verwenden

Verworfen.

Legacy-Systeme und bestimmte Spezialplattformen sollen weiterhin unterstützt werden.

### UEFI nur verwenden, wenn Secure Boot aktiv ist

Verworfen.

UEFI und Secure Boot sind getrennte Konzepte.

### Bei jedem UEFI-Fehler automatisch BIOS versuchen

Verworfen.

Nicht jeder UEFI-Fehler rechtfertigt einen Firmwarewechsel, und Sicherheitsfehler dürfen nicht umgangen werden.

### CSM als einheitliche Kompatibilitätsschicht verwenden

Verworfen.

CSM ist auf modernen Systemen häufig nicht mehr vorhanden und würde native UEFI-Unterstützung unnötig umgehen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0014_Explizite_Contracts_zwischen_Systemkomponenten`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-FIRMWARE-SELECTION-0001`
- `NPSPEC-BOOT-FIRMWARE-POLICY-0001`
- `NPSPEC-BOOT-UEFI-PREFERENCE-0001`
- `NPSPEC-BOOT-BIOS-FALLBACK-0001`
- `NPSPEC-BOOT-DUALMODE-0001`
- `NPSPEC-BOOT-UEFI-DETECTION-0001`
- `NPSPEC-BOOT-BIOS-DETECTION-0001`
- `NPSPEC-BOOT-UEFI-COMPATIBILITY-0001`
- `NPSPEC-BOOT-UEFI-FALLBACKPATH-0001`
- `NPSPEC-BOOT-FIRMWARE-MIGRATION-0001`
- `NPSPEC-BOOT-FIRMWARE-INTROSPECTION-0001`
- `NPSPEC-BOOT-FIRMWARE-SELECTION-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens folgende Szenarien prüfen:

- reines UEFI-System,
- reines Legacy-BIOS-System,
- Dual-Mode-System mit UEFI und CSM,
- automatischer Start über UEFI auf Dual-Mode-System,
- explizit erzwungener BIOS-Start,
- explizit erzwungener UEFI-Start,
- UEFI-Firmware mit inkompatibler Architektur,
- fehlender UEFI-Boot-Eintrag,
- funktionierender UEFI-Fallbackpfad,
- BIOS-Fallback bei nicht vorhandenem UEFI,
- BIOS auf GPT,
- Dual-Mode-Recoverymedium,
- Secure Boot aktiv,
- Secure Boot nicht verfügbar,
- Signaturfehler ohne Security-Downgrade,
- Known-Good-UEFI-Bootloader,
- BIOS-zu-UEFI-Migration,
- UEFI-zu-BIOS-Migration,
- introspektierbarer Auswahlgrund,
- deterministisch auf UEFI fixierter Test,
- deterministisch auf BIOS fixierter Test.

---

## Ergebnis

NovaOS unterstützt weiterhin beide relevanten PC-Firmwarewelten, behandelt sie aber nicht als gleichwertige langfristige Standardpfade.

Die Auswahlstrategie lautet:

```text
Native UEFI
    ↓
Preferred

Legacy BIOS
    ↓
Fallback / Compatibility
```

Beide Pfade münden anschließend in:

```text
Shared Nova Boot Core
        ↓
Common BootInfo
        ↓
Common Kernel Entry
```

Die zentrale Architekturregel lautet:

```text
Prefer UEFI when it is natively
and correctly available.

Use BIOS when it is needed.

Never trade security
for a firmware fallback.
```