# NPSPEC-KERNEL-0030 – Kernel ABI

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0030 |
| Titel | Kernel ABI |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / ABI / Kompatibilität |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0011, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0018, NPSPEC-KERNEL-0025, NPSPEC-KERNEL-0029 |
| Zugehörige ADRs | ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105, ADR-DEV-0002, ADR-IPC-0001, ADR-VFS-0007 |

---

## 1. Zweck

Diese Spezifikation definiert die Kernel Application Binary Interface von NovaOS.

Die Kernel ABI beschreibt die binäre Grenze zwischen:

- Userspace und Kernel,
- Nova Runtime und System Call Interface,
- Kernel und Kernelmodulen,
- Kernel und Treibern,
- Kernel und Kompatibilitätsschichten,
- unterschiedlichen Versionen von Kernel-Services.

## 2. Ziele

Die Kernel ABI muss:

- langfristig stabile Userspace-Schnittstellen bereitstellen,
- mehrere Prozessorarchitekturen unterstützen,
- 32- und später 64-Bit-Umgebungen ermöglichen,
- versionierte Kernel-Services verwenden,
- interne Kernelstrukturen von öffentlichen Strukturen trennen,
- sichere Erweiterungen ohne ABI-Bruch ermöglichen,
- inkompatible Komponenten eindeutig ablehnen,
- Feature Discovery bereitstellen,
- keine Kerneladressen an Userspace offenlegen.

## 3. ABI-Bereiche

NovaOS unterscheidet folgende ABI-Bereiche:

| ABI | Verwendung |
|---|---|
| Userspace ABI | Anwendungen und Systemdienste |
| System Call ABI | architekturspezifischer Kernelübergang |
| Kernel Service ABI | versionierte Kerneloperationen |
| Module ABI | Kernelmodule |
| Driver ABI | Kernel- und Userspace-Treiber |
| Boot-Handoff ABI | Bootloader zu Kernel |
| Crash-Dump ABI | Offline-Diagnose |
| Compatibility ABI | fremde oder ältere Anwendungsmodelle |

Diese Bereiche werden unabhängig versioniert.

## 4. Nicht Bestandteil der stabilen ABI

Nicht stabil und nicht öffentlich sind:

- interne Kernelstrukturen,
- Kernelzeiger,
- interne Sperrtypen,
- Runqueue-Layouts,
- Seitentabellenstrukturen,
- interne Symbolnamen,
- Heap-Metadaten,
- private Subsystem-Callbacks,
- Compilerinterne Datenstrukturen.

Komponenten dürfen keine Layoutannahmen über diese Strukturen treffen.

## 5. ABI-Version

```c
typedef struct np_abi_version {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t revision;
} np_abi_version_t;
```

| Feld | Bedeutung |
|---|---|
| Major | inkompatible ABI-Änderung |
| Minor | kompatible Erweiterung |
| Patch | Fehlerkorrektur ohne Strukturänderung |
| Revision | Build- oder Implementierungsrevision |

Kompatibilität wird primär über Major und Minor bestimmt.

## 6. Kompatibilitätsregel

Es gilt grundsätzlich:

```text
requested.major == provided.major
and requested.minor <= provided.minor
```

Zusätzlich müssen alle verwendeten Feature-Flags und Strukturgrößen unterstützt werden.

Eine gleiche Major-Version allein garantiert keine Unterstützung optionaler Features.

## 7. ABI-Header

Öffentliche erweiterbare Strukturen beginnen mit:

```c
typedef struct np_abi_header {
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t structure_size;
    uint64_t feature_flags;
} np_abi_header_t;
```

Der Empfänger liest nur Felder, die innerhalb von `structure_size` liegen.

## 8. Strukturerweiterung

Strukturen werden kompatibel erweitert, indem neue Felder am Ende ergänzt werden.

Unzulässig sind:

- Einfügen neuer Felder zwischen bestehenden Feldern,
- Ändern bestehender Feldtypen,
- Ändern der Feldbedeutung,
- Entfernen bestehender Felder,
- Ändern der festgelegten Ausrichtung.

Neue Felder müssen sichere Standardsemantik besitzen, wenn sie fehlen.

## 9. Reservierte Felder

Öffentliche Strukturen können reservierte Felder enthalten:

```c
uint64_t reserved[4];
```

Der Sender muss reservierte Felder auf null setzen.

Der Empfänger darf sie nicht als gültige Daten interpretieren, solange keine spätere kompatible Version ihre Bedeutung definiert.

## 10. Datentypen

Öffentliche ABI-Strukturen verwenden Typen mit festgelegter Breite:

```c
int8_t;
uint8_t;
int16_t;
uint16_t;
int32_t;
uint32_t;
int64_t;
uint64_t;
```

Compilerabhängige Typen wie `long`, `enum` ohne definierte Breite oder unversionierte Bitfelder werden vermieden.

## 11. Zeigertypen

Userspace-Zeiger werden innerhalb architekturspezifischer ABIs als `uintptr_t` oder explizite ABI-Zeigertypen dargestellt.

Für architekturübergreifende persistente Formate werden keine nativen Zeiger gespeichert.

```c
typedef uint32_t np_user_pointer32_t;
typedef uint64_t np_user_pointer64_t;
```

Kernelzeiger sind niemals Bestandteil einer öffentlichen Userspace-Struktur.

## 12. Größen und Offsets

Größen, Längen und Offsets verwenden grundsätzlich `uint64_t`, wenn sie architekturübergreifend oder datei-/speicherbezogen sind.

Kleinere Typen sind nur zulässig, wenn die Grenze bewusst Teil der ABI ist.

Jede Berechnung wird auf Überlauf geprüft:

```text
offset + length
count × element_size
header_size + payload_size
```

## 13. Boolean-Werte

ABI-Booleans werden als `uint8_t` oder `uint32_t` mit definierter Breite dargestellt.

Zulässige Werte sind:

```text
0 = false
1 = true
```

Andere Werte müssen normalisiert oder als ungültig abgelehnt werden.

Compilerabhängige `bool`-Layouts werden nicht in persistente oder fremdsprachige ABI-Strukturen übernommen.

## 14. Enumerationen

Öffentliche Enumerationen besitzen einen explizit definierten zugrunde liegenden Ganzzahltyp.

Unbekannte Werte werden:

- abgelehnt, wenn sie zwingend sind,
- als `UNKNOWN` behandelt, wenn dies definiert ist,
- übersprungen, wenn sie optionale Records beschreiben.

Eine Enumeration darf bestehende numerische Werte nicht neu vergeben.

## 15. Bitmasken

Feature- und Optionsbitmasken verwenden Typen fester Breite.

Unbekannte Bits werden anhand der API behandelt:

- optionale Bits ignorieren,
- unbekannte zwingende Bits ablehnen,
- reservierte Bits müssen beim Sender null sein.

Ein Kernel darf unbekannte Bits nicht ungeprüft an Hardware oder andere Subsysteme weiterreichen.

## 16. Byte-Reihenfolge

Die native System-Call ABI verwendet die Byte-Reihenfolge der Zielarchitektur.

Persistente und transportierbare NovaOS-Formate definieren ihre Byte-Reihenfolge ausdrücklich.

Netzwerkprotokolle verwenden die jeweilige Protokoll-Byte-Reihenfolge.

Eine Struktur darf nicht ohne definierte Serialisierung zwischen Plattformen übertragen werden.

## 17. Struktur-Ausrichtung

Jede ABI-Struktur besitzt definierte Ausrichtungsregeln.

Grundsätzlich gelten:

- natürliche Ausrichtung fester Ganzzahltypen,
- explizites Padding bei Bedarf,
- keine compilerabhängigen versteckten Layoutannahmen,
- statische Größen- und Offsetprüfungen.

```c
_Static_assert(
    sizeof(np_abi_header_t) == 16,
    "invalid ABI header size"
);
```

## 18. Packing

Globales `packed` wird für System-Call-Strukturen vermieden, wenn dadurch unausgerichtete Zugriffe entstehen.

Packed Structures sind nur zulässig für:

- fest definierte On-Disk-Formate,
- Netzwerkheader,
- Firmwaretabellen,
- Hardwaredeskriptoren.

Solche Daten werden vor Verwendung in ausgerichtete interne Strukturen übersetzt.

## 19. Zeichenketten

Öffentliche ABI-Zeichenketten verwenden UTF-8 und eine explizite Länge.

```c
typedef struct np_abi_string {
    uint64_t data;
    uint64_t length;
    uint32_t encoding;
    uint32_t flags;
} np_abi_string_t;
```

Zeichenketten müssen nicht nullterminiert sein.

Der Kernel validiert Länge, Adresse und UTF-8-Regeln entsprechend der jeweiligen API.

## 20. Binärpuffer

```c
typedef struct np_abi_buffer {
    uint64_t address;
    uint64_t size;
    uint32_t flags;
    uint32_t reserved;
} np_abi_buffer_t;
```

Der Kernel prüft:

- Adressbereich,
- Zugriffsrichtung,
- vollständige Größe,
- Überlauf,
- Ausrichtung,
- Prozessadressraum.

Ein Pointer-Check ersetzt nicht den geschützten Kopiervorgang.

## 21. Listen und Arrays

Variable Arrays verwenden:

- Pufferadresse,
- Elementanzahl,
- Elementgröße,
- Gesamtpuffergröße.

```c
typedef struct np_abi_array {
    uint64_t address;
    uint64_t count;
    uint32_t element_size;
    uint32_t flags;
} np_abi_array_t;
```

Die Berechnung `count × element_size` muss auf Überlauf geprüft werden.

## 22. Handles

```c
typedef uint64_t np_handle_t;
```

Handles:

- enthalten keine direkt nutzbaren Kernelzeiger,
- gelten nur in ihrer Handle-Tabelle,
- verwenden Generationsschutz,
- besitzen objektspezifische Rechte,
- können nicht allein anhand ihres Werts typisiert werden.

Jede ABI-Operation dokumentiert den erwarteten Handletyp und die erforderlichen Rechte.

## 23. Objektkennungen

Objekt-, Prozess-, Thread-, CPU- und Gerätekennungen dienen der Identifikation.

Sie sind keine Handles und verleihen keine Rechte.

Beispiele:

```c
typedef uint64_t np_object_id_t;
typedef uint64_t np_process_id_t;
typedef uint64_t np_thread_id_t;
typedef uint32_t np_cpu_id_t;
typedef uint64_t np_device_id_t;
```

Administrative Operationen verwenden Handles statt bloßer Kennungen.

## 24. Statuscodes

```c
typedef int32_t np_status_t;
```

Es gilt:

```text
0   = Erfolg
< 0 = Fehler
> 0 = erfolgreicher operationenspezifischer Status
```

Statuscodes werden in zentralen Bereichen registriert und nicht willkürlich pro Modul neu verwendet.

## 25. Statusbereiche

| Bereich | Verwendung |
|---|---|
| Core | allgemeine Kernelstatuscodes |
| Process | Prozess- und Threadfehler |
| Memory | Speicherverwaltung |
| IPC | Kommunikationsfehler |
| VFS | Datei- und Mountfehler |
| Device | Geräte- und Treiberfehler |
| Network | Socket- und Netzwerkfehler |
| Security | Capability- und Policyfehler |
| Diagnostic | Logging- und Dumpfehler |
| Power | Energie- und Thermalfehler |

Ein Statuscode muss maschinenlesbar und dokumentiert sein.

## 26. System-Call ABI

Ein Systemaufruf wird identifiziert durch:

- Service-ID,
- Operations-ID,
- ABI-Version,
- Argumentstruktur,
- Argumentgröße.

```c
typedef struct np_syscall_request {
    uint32_t service_id;
    uint32_t operation_id;
    np_abi_version_t version;

    uint64_t arguments;
    uint64_t argument_size;
} np_syscall_request_t;
```

Die architekturspezifische Registerbelegung wird separat definiert.

## 27. Service IDs

Service IDs werden zentral vergeben.

```text
Core
Process
Thread
Memory
IPC
VFS
Device
Network
Security
Diagnostic
Power
```

Veröffentlichte IDs werden niemals mit anderer Bedeutung wiederverwendet.

Entfernte Services bleiben als reserviert dokumentiert.

## 28. Operations-IDs

Operations-IDs sind innerhalb eines Services eindeutig.

Neue Operationen können ergänzt werden, ohne bestehende IDs zu verändern.

Eine bestehende Operation erhält nur dann eine neue Semantik, wenn eine neue ABI-Major-Version verwendet wird.

## 29. System-Call-Eintritt

Die konkrete CPU-Instruktion ist architekturabhängig.

Mögliche x86-Pfade sind:

- `SYSENTER/SYSEXIT`,
- `SYSCALL/SYSRET`,
- kontrolliertes Interrupt-Gate als Fallback.

Unabhängig vom Eintrittsmechanismus bleibt die logische Service-ABI identisch.

## 30. Registerkonvention

Jede Architektur definiert:

- Register für Service-ID,
- Register für Operations-ID,
- Register für Argumentzeiger,
- Register für Argumentgröße,
- Rückgaberegister,
- zu erhaltende Register,
- Stack- und Alignmentregeln.

Komplexe Parameter werden über Argumentstrukturen und nicht über viele einzelne Register übertragen.

## 31. Calling Convention

Die native Userspace ABI definiert pro Architektur:

- Parameterübergabe,
- Rückgabewerte,
- Stackausrichtung,
- Caller- und Callee-Saved Register,
- FPU-/SIMD-Regeln,
- TLS-Register,
- Exception- und Signalframes.

Für x86-32 wird eine eigene NovaOS-Konvention festgelegt und nicht implizit eine Host-OS-ABI übernommen.

## 32. Kernel-Stack-Wechsel

Jeder Systemaufruf wechselt auf einen kontrollierten Kernel-Stack.

Der Kernel darf nicht auf dem ungeprüften Userspace-Stack weiterarbeiten.

Vor der Rückkehr werden geprüft:

- Instruction Pointer,
- Stack Pointer,
- Statusregister,
- Privilegienstufe,
- Segment- oder TLS-Zustand,
- ausstehende Exceptions und Signale.

## 33. Userspace-Stubs

Die Nova Runtime stellt architekturspezifische System-Call-Stubs bereit.

```c
np_status_t np_system_call(
    uint32_t service_id,
    uint32_t operation_id,
    const void* arguments,
    size_t argument_size
);
```

Anwendungen sollen bevorzugt typisierte Runtime-Funktionen statt des generischen Stubs verwenden.

## 34. Shared Service Page

Der Kernel kann eine schreibgeschützte Shared Service Page in Prozesse einblenden.

Sie kann enthalten:

- Kernel-ABI-Version,
- Service-Featureinformationen,
- monotone Zeitdaten,
- CPU-Grundfunktionen,
- optimierte Runtime-Stubs,
- Boot- und Build-ID.

Die Seite enthält keine schreibbaren Kernelzeiger oder geheimen Daten.

## 35. Kernel Service ABI

Kernel-Services werden als versionierte logische Schnittstellen veröffentlicht.

Ein Service besitzt:

- stabile Service-ID,
- Version,
- Feature-Flags,
- definierte Argumentstrukturen,
- Statuscodes,
- Capability-Anforderungen.

Userspace hängt nicht von internen Kernel-Funktionsadressen ab.

## 36. Feature Discovery

```c
np_status_t np_abi_query_service(
    uint32_t service_id,
    np_service_abi_information_t* information
);
```

Abfragbar sind:

- unterstützte Versionen,
- Feature-Flags,
- Strukturgrößen,
- optionale Operationen,
- Architekturstatus,
- Einschränkungen.

Feature Discovery ist verbindlich für optionale ABI-Funktionen.

## 37. Module ABI

Kernelmodule verwenden eine getrennte versionierte ABI.

Sie dürfen ausschließlich:

- freigegebene Kernelservices,
- versionierte Symbolgruppen,
- dokumentierte Callbackstrukturen,
- registrierte Objekttyp- und Subsysteminterfaces

verwenden.

Interne Kernelheader sind keine stabile Module ABI.

## 38. Symbolversionierung

Ein Modulsymbol besitzt mindestens:

- Name oder stabile Symbol-ID,
- Major- und Minor-Version,
- Symboltyp,
- Besitzer,
- optionale ABI-Signatur.

Ein gleichnamiges Symbol mit inkompatibler Version wird nicht gebunden.

Nicht exportierte Symbole bleiben für Module unsichtbar.

## 39. Module-Importgruppen

Module deklarieren erforderliche Importgruppen, beispielsweise:

```text
kernel.object.v1
kernel.memory.v1
kernel.device.v2
kernel.vfs.v1
```

Der Module Loader prüft, ob:

- Gruppe existiert,
- Version kompatibel ist,
- Modultyp die Gruppe verwenden darf,
- Sicherheitsprofil den Import erlaubt.

## 40. Driver ABI

Das Driver Framework verwendet versionierte Operations- und Callbacktabellen.

```c
typedef struct np_driver_operations_v1 {
    np_abi_header_t header;

    np_status_t (*probe)(np_device_handle_t device);
    np_status_t (*bind)(np_device_handle_t device);
    np_status_t (*start)(np_device_handle_t device);
    np_status_t (*stop)(np_device_handle_t device, uint32_t reason);
    np_status_t (*remove)(np_device_handle_t device);
} np_driver_operations_v1_t;
```

Neue Callbacks werden am Ende ergänzt oder über neue Featuretabellen bereitgestellt.

## 41. Userspace-Driver-ABI

Userspace-Treiber verwenden:

- IPC-Protokolle,
- versionierte Treiberservices,
- Gerätehandles,
- Shared-Memory-Objekte,
- Interrupt-Endpunkte,
- DMA-Objekte.

Sie erhalten keine direkten Kernelcallback-Zeiger.

Dadurch kann die Userspace-Driver-ABI stabiler und sicherer als eine direkte Kernelstruktur-ABI gehalten werden.

## 42. Boot-Handoff ABI

Das Boot-Handoff Protocol besitzt eine eigene Versionierung.

Der Kernel prüft:

- Magic Number,
- Major- und Minor-Version,
- Gesamtgröße,
- Prüfsumme,
- Featureflags,
- Offsets,
- Pflichtrecords.

Eine inkompatible Major-Version führt zum sicheren Bootabbruch.

## 43. Exception ABI

Userspace-Exception-Records und Kontexte sind architekturspezifisch versioniert.

Sie enthalten:

- generischen Exception-Code,
- Architekturkennung,
- Registerstrukturgröße,
- freigegebene Fehlerdaten,
- Fortsetzungsflags.

Ein Handler darf nur die für seine Architektur und ABI definierte Kontextstruktur verändern.

## 44. Signal ABI

Signalframes enthalten:

- Signal- oder Ereignistyp,
- ursprünglichen Userspace-Kontext,
- optionale Parameter,
- Restorer- oder Rückkehrinformationen,
- Integritätsmarker.

Der Kernel validiert den vollständigen Signalframe vor der Rückkehr.

Userspace kann darüber keinen privilegierten CPU-Zustand erzeugen.

## 45. VFS ABI

Datei- und Verzeichnisoperationen verwenden:

- File Handles,
- UTF-8-Pfade mit expliziter Länge,
- 64-Bit-Dateioffsets,
- versionierte Attributstrukturen,
- eindeutige Zeitdarstellung,
- typisierte Fehlercodes.

On-Disk-Formate sind nicht automatisch Teil der Kernel ABI.

## 46. Netzwerk-ABI

Socket- und Netzwerkstrukturen verwenden:

- explizite Adressfamilien,
- definierte Strukturgrößen,
- Netzwerk-Byte-Reihenfolge für Protokollfelder,
- native ABI-Reihenfolge für Kontrollstrukturen,
- versionierte Socketoptionen.

Unbekannte Adressfamilien und Optionen werden definiert abgelehnt.

## 47. Zeit-ABI

Zeitwerte werden grundsätzlich in Nanosekunden dargestellt:

```c
typedef uint64_t np_time_ns_t;
typedef int64_t np_time_delta_ns_t;
```

Timeouts verwenden bevorzugt absolute monotone Deadlines.

Kalender-, Zeitzonen- und Sommerzeitlogik liegt im Userspace.

## 48. 32-Bit-ABI

Die initiale NovaOS-Version verwendet eine 32-Bit-x86-Kernel- und Userspace-ABI.

Dabei gelten:

- 32-Bit-Userspace-Zeiger,
- 64-Bit-Dateioffsets,
- 64-Bit-Zeitwerte,
- 64-Bit-Objekt- und Handlekennungen,
- definierte 8-Byte-Ausrichtung für ausgewählte 64-Bit-Felder,
- kein implizites Compilerlayout.

64-Bit-Werte können abhängig von der Calling Convention über Speicherstrukturen übertragen werden.

## 49. Zukünftige 64-Bit-ABI

Die spätere x86-64-ABI verwendet:

- 64-Bit-Zeiger,
- erweiterten virtuellen Adressraum,
- `SYSCALL/SYSRET`,
- eigene Register-Calling-Convention,
- kompatible logische Service-IDs,
- dieselben versionierten Servicekonzepte.

Binäre 32-Bit-Anwendungen benötigen eine ausdrückliche Kompatibilitätsschicht.

## 50. Kompatibilitätsschicht

Eine Compatibility Persona übersetzt eine fremde oder ältere ABI in native Kernelservices.

```text
Fremder Systemaufruf
  -> Persona Dispatcher
  -> Struktur- und Statusübersetzung
  -> nativer NovaOS Kernel-Service
  -> Rückübersetzung
```

Die Kompatibilitätsschicht darf Capabilities, Namespaces oder Sicherheitsrichtlinien nicht umgehen.

## 51. ABI-Persona

```c
typedef enum np_abi_persona {
    NP_ABI_NOVA_NATIVE,
    NP_ABI_NOVA_LEGACY,
    NP_ABI_COMPAT_32,
    NP_ABI_LINUX_COMPAT,
    NP_ABI_WINDOWS_COMPAT
} np_abi_persona_t;
```

Nicht jede Persona ist Bestandteil des Kernels.

Komplexe Kompatibilität soll möglichst in isolierten Userspace-Diensten implementiert werden.

## 52. ABI-Aushandlung

Beim Prozessstart werden geprüft:

- ausführbare Architektur,
- ABI-Persona,
- ABI-Major-Version,
- benötigte Services,
- Feature-Flags,
- Sicherheitsprofil.

Ein Prozess startet nicht, wenn zwingende ABI-Anforderungen fehlen.

Optionale Features werden über Runtime-Abfragen behandelt.

## 53. ABI-Kompatibilitätsmanifest

Ausführbare Dateien können ein Manifest enthalten:

```c
typedef struct np_application_abi_manifest {
    np_abi_header_t header;

    np_abi_persona_t persona;
    np_abi_version_t required_kernel_abi;

    np_required_service_list_t services;
    uint64_t required_features;
    uint64_t optional_features;
} np_application_abi_manifest_t;
```

Das Manifest ist Teil der Image- und Sicherheitsprüfung.

## 54. Deprecation

Ein ABI-Service kann als veraltet markiert werden.

Der Ablauf ist:

1. Deprecation dokumentieren,
2. Ersatzservice bereitstellen,
3. Runtime- und Entwicklerinformationen ausgeben,
4. alten Service für den definierten Zeitraum erhalten,
5. Entfernung nur mit neuer Major-Version oder klarer Sicherheitsausnahme.

Veraltete IDs bleiben reserviert.

## 55. Sicherheitsbedingte ABI-Sperrung

Eine schwerwiegend unsichere Operation kann trotz ABI-Stabilitätsziel deaktiviert werden.

In diesem Fall muss NovaOS:

- einen eindeutigen Statuscode zurückgeben,
- die betroffene Version dokumentieren,
- soweit möglich einen sicheren Ersatz bereitstellen,
- die Sperrung auditieren,
- keine stille Semantikänderung durchführen.

## 56. ABI-Registry

NovaOS führt eine zentrale Registry für:

- Service IDs,
- Operations-IDs,
- Statuscodes,
- Objekttypen,
- Capability-Rechte,
- Strukturversionen,
- Symbolgruppen,
- Feature-Flags.

Doppelte oder widersprüchliche Einträge werden während Build, Modul-Laden oder Service-Registrierung abgelehnt.

## 57. ABI-Beschreibung

Aus der Registry soll eine maschinenlesbare ABI-Beschreibung erzeugt werden.

Sie kann verwendet werden für:

- Headergenerierung,
- Nova Runtime,
- Bindings anderer Programmiersprachen,
- ABI-Tests,
- Dokumentation,
- Kompatibilitätsprüfung,
- Fuzzing.

Generierter Code muss reproduzierbar aus der versionierten Quelle entstehen.

## 58. Sprachbindungen

Die Kernel ABI soll Bindings für mehrere Sprachen ermöglichen.

Dafür gelten:

- keine compilerabhängigen Klassenlayouts,
- keine C++-Namensmangling-Abhängigkeit,
- explizite Datentypbreiten,
- längenbasierte Strings und Arrays,
- stabile Calling Conventions,
- keine direkten Kernelzeiger.

NovaLang kann native Bindings aus der ABI-Beschreibung generieren.

## 59. ABI-Hash

Strukturen und Services können einen maschinenlesbaren ABI-Hash besitzen.

Der Hash berücksichtigt beispielsweise:

- Feldnamen,
- Reihenfolge,
- Typbreite,
- Offsets,
- Calling Convention,
- Versionsinformationen.

Der Hash dient der Diagnose und Buildprüfung, ersetzt aber keine semantische Versionierung.

## 60. Build ID

Jeder Kernel, jedes Modul und jedes Symbolpaket besitzt eine Build ID.

```c
typedef np_uuid_t np_build_id_t;
```

Build IDs werden verwendet für:

- Crash Dumps,
- Symbolauflösung,
- Diagnose,
- Modulprüfung,
- Reproduzierbarkeit.

Anwendungen dürfen nicht allein von einer exakten Kernel Build ID abhängen, wenn eine stabile Service-ABI existiert.

## 61. Capability-Rechte

ABI-Verwaltungsoperationen verwenden mindestens:

| Capability | Bedeutung |
|---|---|
| `CAP_ABI_QUERY` | öffentliche ABI-Informationen abfragen |
| `CAP_ABI_QUERY_SYSTEM` | geschützte System-ABI-Daten lesen |
| `CAP_ABI_COMPATIBILITY` | Compatibility Persona verwalten |
| `CAP_ABI_REGISTER_SERVICE` | autorisierte Kernelservices registrieren |
| `CAP_ABI_REGISTER_MODULE` | Modul- und Symbolgruppen registrieren |
| `CAP_ABI_DIAGNOSTIC` | detaillierte ABI-Diagnosen lesen |
| `CAP_ABI_ADMIN` | globale ABI-Richtlinien verwalten |

Normale Anwendungen benötigen keine administrative Capability, um öffentliche Serviceversionen abzufragen.

## 62. Sicherheit

Die Kernel ABI muss sicherstellen, dass:

- öffentliche Strukturen keine Kernelzeiger enthalten,
- alle Größen, Offsets und Zähler validiert werden,
- unbekannte zwingende Features abgelehnt werden,
- Handles auf Typ, Generation und Rechte geprüft werden,
- Rückkehrkontexte keine privilegierten CPU-Zustände enthalten,
- Compatibility Personas keine Sicherheitsgrenzen umgehen,
- Module nur freigegebene Symbolgruppen importieren,
- W^X und Kernelisolation durch keine ABI-Operation geschwächt werden,
- Kerneladressen nicht über Fehler oder Diagnose unkontrolliert offengelegt werden.

## 63. Datenschutz

ABI-Informationen können Hardware-, Build- und Systemdetails enthalten.

Daher gelten:

- normale Anwendungen erhalten nur benötigte Featureinformationen,
- vollständige Build- und Hardwaredetails können eingeschränkt werden,
- Kerneladressen sind nicht Teil öffentlicher ABI-Abfragen,
- ABI-Diagnose bleibt standardmäßig lokal,
- externe Telemetrie ist standardmäßig deaktiviert.

## 64. Diagnose

Autorisierte Diagnosedienste können abrufen:

- native Kernel-ABI-Version,
- verfügbare Serviceversionen,
- aktive ABI-Personas,
- registrierte Module-Importgruppen,
- abgelehnte ABI-Anfragen,
- inkompatible Strukturen,
- verwendete veraltete Services,
- Build IDs,
- Feature-Flags.

Interne Funktionsadressen werden nicht ausgegeben.

## 65. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_ABI_INCOMPATIBLE` | Major-Version ist inkompatibel |
| `NP_ERR_ABI_TOO_OLD` | bereitgestellte ABI ist zu alt |
| `NP_ERR_ABI_FEATURE_MISSING` | zwingendes Feature fehlt |
| `NP_ERR_ABI_STRUCTURE_SIZE` | Strukturgröße ist ungültig |
| `NP_ERR_ABI_RESERVED_FIELD` | reserviertes Feld besitzt unzulässigen Wert |
| `NP_ERR_ABI_ALIGNMENT` | Ausrichtung ist ungültig |
| `NP_ERR_SERVICE_UNKNOWN` | Service-ID ist unbekannt |
| `NP_ERR_OPERATION_UNKNOWN` | Operations-ID ist unbekannt |
| `NP_ERR_STATUS_UNKNOWN` | Statusbereich ist nicht registriert |
| `NP_ERR_PERSONA_UNAVAILABLE` | ABI-Persona ist nicht verfügbar |
| `NP_ERR_SYMBOL_VERSION` | Modulsymbol ist inkompatibel |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 66. Fehlerbehandlung

Inkompatible Userspace-Anfragen werden mit einem definierten ABI-Fehler abgelehnt.

Ein fehlerhaftes Userspace-Layout darf den Kernel nicht beschädigen.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- öffentliche ABI gibt einen Kernelzeiger aus,
- Dispatcher bindet falschen Servicehandler,
- aktive Service-ID besitzt mehrere widersprüchliche Bedeutungen,
- Modul wird trotz inkompatibler Symbolversion ausgeführt,
- Rückkehr in Userspace mit privilegiertem Kontext,
- interne Struktur wird ungeprüft als öffentliche ABI interpretiert.

## 67. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. kompatible Major- und Minor-Version,
2. inkompatible Major-Version,
3. zu alte Minor-Version,
4. unterstütztes Feature-Flag,
5. fehlendes zwingendes Feature,
6. minimale Strukturgröße,
7. größere kompatible Struktur,
8. zu kleine Struktur,
9. reserviertes Feld ungleich null,
10. feste Datentypbreiten,
11. Strukturgrößen und Offsets,
12. 64-Bit-Wert in 32-Bit-ABI,
13. ungültiger Userspace-Zeiger,
14. Arraygrößenüberlauf,
15. UTF-8-String mit Länge,
16. Handletyp- und Rechteprüfung,
17. unbekannter Service,
18. unbekannte Operation,
19. Statuscodebereiche,
20. System-Call-Eintritt über schnellen x86-Pfad,
21. Fallback über Interrupt-Gate,
22. Kernel-Stack-Wechsel,
23. validierter Userspace-Rückkehrkontext,
24. Shared Service Page,
25. Service Feature Discovery,
26. Modul-Symbolversionierung,
27. Importgruppenprüfung,
28. Driver-Operations-Tabelle mit älterer Größe,
29. Boot-Handoff-Versionierung,
30. Exception-Kontext-Versionierung,
31. File-Offset über 4 GiB in 32-Bit-ABI,
32. zukünftige 64-Bit-Struktursimulation,
33. Compatibility Persona,
34. Sicherheitsbedingte Service-Sperrung,
35. Deprecation eines Services,
36. ABI-Registry-Konflikt,
37. maschinenlesbare ABI-Beschreibung,
38. Sprachbinding-Generierung,
39. ABI-Hash und Build ID,
40. Fuzzing öffentlicher ABI-Strukturen.

## 68. Verbindliche Invarianten

1. Öffentliche ABI-Strukturen enthalten keine direkt nutzbaren Kernelzeiger.
2. Veröffentlichte IDs werden niemals mit anderer Bedeutung wiederverwendet.
3. Bestehende Strukturfelder ändern innerhalb einer Major-Version weder Position noch Bedeutung.
4. Neue kompatible Felder werden ausschließlich am Strukturende ergänzt.
5. Jede erweiterbare Struktur enthält Version und Gesamtgröße.
6. Unbekannte zwingende Features werden abgelehnt.
7. Handles werden bei jeder Operation auf Typ, Generation und Rechte geprüft.
8. Interne Kernelstrukturen sind nicht Bestandteil der stabilen ABI.
9. Module importieren ausschließlich freigegebene und versionierte Symbole.
10. Compatibility Personas unterliegen denselben Capability- und Sicherheitsgrenzen wie native Services.
11. Die 32-Bit-ABI verwendet 64-Bit-Typen für Zeit, Dateioffsets, Handles und Objektkennungen, soweit festgelegt.
12. ABI-Änderungen werden durch Registry, Tests und maschinenlesbare Beschreibung nachvollziehbar verwaltet.

## 69. Referenzablauf einer ABI-Anfrage

```text
Anwendung ruft Nova Runtime auf
  -> Runtime wählt Service und ABI-Version
  -> versionierte Argumentstruktur erzeugen
  -> architekturspezifischen System-Call-Eintritt ausführen
  -> Kernel validiert Service- und Operations-ID
  -> Major-, Minor- und Feature-Kompatibilität prüfen
  -> Strukturgröße und reservierte Felder prüfen
  -> Userspace-Daten sicher in Kernel kopieren
  -> Handles, Typen und Capabilities prüfen
  -> internen Kernel-Service ausführen
  -> Ergebnis in öffentliche ABI-Struktur übersetzen
  -> Daten sicher in Userspace kopieren
  -> Rückkehrkontext validieren
  -> Anwendung fortsetzen
```

## 70. Zusammenfassung

Die NovaOS Kernel ABI trennt öffentliche, langfristig stabile Servicegrenzen konsequent von internen Kernelstrukturen.

Versionierte Strukturen mit festen Datentypbreiten, expliziten Größen, Feature-Flags und stabilen Servicekennungen ermöglichen kompatible Erweiterungen. Handles und Objektkennungen ersetzen direkte Kernelzeiger, während alle Aufrufe Capability- und Typprüfungen durchlaufen.

Userspace-, Modul-, Treiber-, Boot- und Kompatibilitäts-ABIs werden getrennt versioniert. Dadurch kann NovaOS zunächst eine 32-Bit-x86-Plattform bereitstellen und später 64-Bit- sowie weitere Architekturen ergänzen, ohne die logische Kernel-Service-Architektur neu entwerfen zu müssen.