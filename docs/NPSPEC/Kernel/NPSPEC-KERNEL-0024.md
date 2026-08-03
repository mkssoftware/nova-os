# NPSPEC-KERNEL-0024 – Crash Dump System

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0024 |
| Titel | Crash Dump System |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Diagnose / Fehlerbehandlung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0010, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0019, NPSPEC-KERNEL-0020, NPSPEC-KERNEL-0023 |
| Zugehörige ADRs | ADR-DIAG-0001, ADR-DIAG-0003, ADR-DIAG-0005, ADR-SEC-0008, ADR-SEC-0009, ADR-SEC-0010 |

---

## 1. Zweck

Diese Spezifikation definiert das Crash Dump System von NovaOS.

Das System erfasst Diagnoseinformationen bei:

- Kernel Panic,
- schwerwiegenden Kernel-Exceptions,
- Machine Checks,
- Watchdog-Timeouts,
- kritischen Treiberfehlern,
- Abstürzen von Userspace-Prozessen,
- manuell ausgelösten Diagnosedumps.

## 2. Ziele

Das Crash Dump System muss:

- auch bei teilweise beschädigtem Kernelzustand funktionieren,
- unterschiedliche Dumpgrößen unterstützen,
- keine reguläre Heapallokation im Panic-Pfad benötigen,
- sensible Daten erkennen und schützen,
- Dumps integritätsgesichert speichern,
- unvollständige Dumps eindeutig markieren,
- Recovery und Offline-Analyse ermöglichen,
- standardmäßig keine Daten automatisch extern übertragen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Exception Manager | Erkennung und Klassifizierung des Fehlers |
| Panic Manager | kontrollierter nicht rückkehrender Fehlerpfad |
| Crash Dump System | Erfassen und Speichern der Diagnosedaten |
| Logging Framework | letzte strukturierte Logeinträge |
| Secure Debugging | Zugriff auf geschützte Diagnosedaten |
| Recovery Manager | Prüfung, Export und Löschung von Dumps |
| Userspace Crash Service | Dumps regulärer Prozesse |
| Audit Subsystem | sicherheitsrelevante Dumpereignisse |

## 4. Dump-Klassen

```c
typedef enum np_crash_dump_class {
    NP_DUMP_USERSPACE,
    NP_DUMP_DRIVER,
    NP_DUMP_KERNEL_MINIMAL,
    NP_DUMP_KERNEL_STANDARD,
    NP_DUMP_KERNEL_FULL,
    NP_DUMP_MACHINE_CHECK,
    NP_DUMP_MANUAL
} np_crash_dump_class_t;
```

| Klasse | Inhalt |
|---|---|
| `USERSPACE` | Zustand eines Prozesses |
| `DRIVER` | Treiberprozess oder isolierbare Treiberinstanz |
| `KERNEL_MINIMAL` | wichtigste Panic- und CPU-Daten |
| `KERNEL_STANDARD` | Kernelzustand und ausgewählte Speicherbereiche |
| `KERNEL_FULL` | größtmöglicher physischer Speicherabbild |
| `MACHINE_CHECK` | Hardware- und CPU-Fehlerdaten |
| `MANUAL` | administrativ ausgelöster Diagnosedump |

## 5. Dump-Profile

```c
typedef enum np_dump_profile {
    NP_DUMP_PROFILE_DISABLED,
    NP_DUMP_PROFILE_MINIMAL,
    NP_DUMP_PROFILE_STANDARD,
    NP_DUMP_PROFILE_FULL,
    NP_DUMP_PROFILE_PRIVACY
} np_dump_profile_t;
```

`PRIVACY` begrenzt den Dump auf technische Metadaten und ausdrücklich freigegebene Speicherbereiche.

Das Standardprofil für Produktivsysteme ist `STANDARD` oder `PRIVACY`, abhängig von der Systemrichtlinie.

## 6. Dump-Status

```c
typedef enum np_dump_status {
    NP_DUMP_EMPTY,
    NP_DUMP_WRITING,
    NP_DUMP_COMPLETE,
    NP_DUMP_PARTIAL,
    NP_DUMP_CORRUPT,
    NP_DUMP_EXPORTED,
    NP_DUMP_DELETED
} np_dump_status_t;
```

Ein Dump im Zustand `WRITING` darf nach einem Neustart nicht als vollständig behandelt werden.

## 7. Dump-Lebenszyklus

```text
EMPTY
  -> WRITING
  -> COMPLETE oder PARTIAL
  -> EXPORTED
  -> DELETED
```

Ein beschädigter Dump wechselt nach `CORRUPT`.

Statusübergänge müssen auch bei Stromausfall möglichst eindeutig erkennbar bleiben.

## 8. Dump Header

```c
typedef struct np_crash_dump_header {
    uint64_t magic;
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t header_size;

    np_crash_dump_class_t dump_class;
    np_dump_status_t status;
    uint32_t flags;

    np_uuid_t dump_id;
    np_uuid_t boot_id;
    np_uuid_t kernel_build_id;
    np_uuid_t system_generation;

    np_time_ns_t monotonic_time;
    int64_t realtime_seconds;

    uint64_t total_size;
    uint64_t section_count;

    uint8_t integrity_hash[64];
} np_crash_dump_header_t;
```

Der Header wird vor der Auswertung vollständig validiert.

## 9. Dump-Sektionen

Ein Dump besteht aus versionierten Sektionen.

```c
typedef enum np_dump_section_type {
    NP_DUMP_SECTION_PANIC,
    NP_DUMP_SECTION_CPU_CONTEXT,
    NP_DUMP_SECTION_THREAD,
    NP_DUMP_SECTION_PROCESS,
    NP_DUMP_SECTION_MEMORY_MAP,
    NP_DUMP_SECTION_MEMORY_PAGES,
    NP_DUMP_SECTION_PAGE_TABLES,
    NP_DUMP_SECTION_MODULES,
    NP_DUMP_SECTION_LOGS,
    NP_DUMP_SECTION_DEVICES,
    NP_DUMP_SECTION_SECURITY,
    NP_DUMP_SECTION_MACHINE_CHECK,
    NP_DUMP_SECTION_METADATA
} np_dump_section_type_t;
```

Unbekannte optionale Sektionen können von kompatiblen Lesern übersprungen werden.

## 10. Sektions-Header

```c
typedef struct np_dump_section_header {
    uint32_t type;
    uint16_t version;
    uint16_t flags;

    uint64_t offset;
    uint64_t stored_size;
    uint64_t original_size;

    uint32_t compression;
    uint32_t encryption;

    uint8_t checksum[32];
} np_dump_section_header_t;
```

Offsets und Größen müssen auf Überlauf und Überschneidung geprüft werden.

## 11. Panic-Informationen

Die Panic-Sektion enthält mindestens:

- Panic-Code,
- kurze technische Beschreibung,
- betroffene Komponente,
- CPU-Kennung,
- Prozess- und Threadkennung,
- Instruction Pointer,
- aktuelle Bootphase,
- Sicherheitsstatus,
- letzte erfolgreiche Kernelphase,
- Ursache der Dump-Erzeugung.

Freitext wird begrenzt und muss keine sensiblen Inhalte enthalten.

## 12. CPU-Kontexte

Für jede erreichbare CPU werden soweit möglich erfasst:

- allgemeine Register,
- Instruction Pointer,
- Stack Pointer,
- Statusregister,
- Control Register,
- Seitentabellen-Root,
- CPU-lokaler Zustand,
- Interrupt- und Exceptionzustand,
- FPU-/SIMD-Zustand nach Richtlinie.

Nicht initialisierte oder nicht erreichbare CPUs werden entsprechend markiert.

## 13. Stoppen anderer CPUs

Bei einer Kernel Panic versucht der Panic Manager, andere CPUs über einen reservierten IPI-Pfad anzuhalten.

Jede CPU:

1. stoppt reguläre Ausführung,
2. sichert ihren Minimalzustand,
3. bestätigt den Halt,
4. bleibt im sicheren Wartezustand.

Fehlende Bestätigungen werden mit Timeout dokumentiert und blockieren die Dump-Erstellung nicht unbegrenzt.

## 14. Panic-CPU

Die zuerst in den Panic-Pfad eintretende CPU wird zur Panic-CPU.

Nur sie koordiniert standardmäßig die Dump-Erstellung.

Weitere gleichzeitig eintretende CPUs dürfen:

- ihren Kontext sichern,
- keine konkurrierende Dump-Erstellung beginnen,
- den gemeinsamen Writer nicht beschädigen,
- keine normalen Kernel-Sperren übernehmen.

## 15. Rekursive Panic

Eine weitere Exception oder Panic während der Dump-Erstellung aktiviert einen reduzierten Notfallpfad.

Dieser schreibt nur:

- Panic-Code,
- CPU-Kontext,
- Rekursionstiefe,
- aktuelle Dump-Phase,
- vorhandene Teilstatusdaten.

Eine rekursive Dump-Erstellung darf keine unendliche Schleife erzeugen.

## 16. Minimaler Kernel Dump

Ein Minimal Dump enthält mindestens:

- Dump Header,
- Panic Record,
- Kontext der Panic-CPU,
- soweit möglich Kontexte anderer CPUs,
- Kernelmodule und Build IDs,
- Speicherkarte,
- letzte kritische Logeinträge,
- aktuelle Prozess- und Threadkennung.

Er muss in einen kleinen reservierten Speicherbereich passen.

## 17. Standard Kernel Dump

Ein Standard Dump ergänzt:

- relevante Kernel-Stacks,
- aktive Thread- und Prozessmetadaten,
- ausgewählte Seitentabellen,
- Kernel Object Graph in reduzierter Form,
- Treiber- und Geräteinformationen,
- Scheduler- und Interruptzustände,
- markierte Kernel-Heapseiten,
- Sicherheitsstatus ohne Schlüsselmaterial.

## 18. Vollständiger Kernel Dump

Ein vollständiger Dump kann große Teile des physischen Speichers enthalten.

Ausgeschlossen bleiben standardmäßig:

- MMIO-Bereiche,
- als `NO_DUMP` markierte Seiten,
- Schlüssel- und Secret-Speicher,
- nicht lesbare Gerätebereiche,
- ausdrücklich geschützte Benutzerdaten,
- fehlerhafte physische Seiten.

Ein Full Dump benötigt ausreichend Zielkapazität und eine passende Sicherheitsrichtlinie.

## 19. Userspace-Dump

Ein Userspace-Dump kann enthalten:

- Exception Record,
- Threadkontexte,
- Prozessmetadaten,
- Modul- und Image-Liste,
- Stackbereiche,
- Speicherregionen nach Richtlinie,
- offene Handles in reduzierter Darstellung,
- letzte Prozesslogs.

Der Dump wird bevorzugt durch einen Userspace Crash Service erzeugt.

## 20. Treiber-Dump

Ein Treiber-Dump enthält zusätzlich:

- Driver ID und Instance ID,
- zugeordnetes Gerät,
- I/O-Queue-Zustände,
- letzte Requests,
- Interrupt- und DMA-Metadaten,
- Power- und PnP-Zustand,
- Restart-Zähler.

DMA-Pufferinhalte werden nicht standardmäßig aufgenommen.

## 21. Speicherklassifizierung

Speicherseiten werden vor der Aufnahme klassifiziert:

```c
typedef enum np_dump_memory_class {
    NP_DUMP_MEMORY_KERNEL_CODE,
    NP_DUMP_MEMORY_KERNEL_DATA,
    NP_DUMP_MEMORY_KERNEL_STACK,
    NP_DUMP_MEMORY_PAGE_TABLE,
    NP_DUMP_MEMORY_PROCESS,
    NP_DUMP_MEMORY_FILE_CACHE,
    NP_DUMP_MEMORY_DEVICE,
    NP_DUMP_MEMORY_SECRET,
    NP_DUMP_MEMORY_NO_DUMP,
    NP_DUMP_MEMORY_UNKNOWN
} np_dump_memory_class_t;
```

`SECRET` und `NO_DUMP` werden standardmäßig ausgeschlossen.

## 22. NO_DUMP-Attribut

Speicherregionen können als nicht dumpbar markiert werden.

```c
NP_MEMORY_NO_DUMP
```

Dieses Attribut wird verwendet für:

- kryptografische Schlüssel,
- Authentifizierungstokens,
- sichere Eingabepuffer,
- Trust-Backend-Zustände,
- besonders sensible Anwendungsdaten.

Nur eine ausdrücklich autorisierte forensische Richtlinie darf davon abweichen.

## 23. Schlüsselmaterial

Private Schlüssel, Stack Canaries, Master Secrets und vergleichbare Daten werden nicht in reguläre Dumps übernommen.

Kann eine Seite nicht sicher teilweise gefiltert werden, muss die gesamte Seite ausgeschlossen oder verschlüsselt und besonders geschützt behandelt werden.

Dump-Verschlüsselung rechtfertigt nicht automatisch die Aufnahme unnötiger Schlüssel.

## 24. Kerneladressen

Kerneladressen sind innerhalb des verschlüsselten Diagnosedumps zulässig, wenn sie für die Analyse erforderlich sind.

In unverschlüsselten oder exportierten Darstellungen werden sie:

- relativ zu Modulbasen dargestellt,
- tokenisiert,
- gehasht,
- oder vollständig entfernt.

ASLR-relevante Daten benötigen Secure-Debugging-Rechte.

## 25. Reservierter Dump-Puffer

NovaOS reserviert während des Bootvorgangs einen kleinen Panic-sicheren Speicherbereich.

Dieser enthält:

- Minimal-Dump-Puffer,
- CPU-Kontextbereiche,
- Sektionsmetadaten,
- Notfall-Logpuffer,
- Writerzustand.

Der Bereich darf nicht für normale Kernelallokationen verwendet werden.

## 26. Dumpziele

```c
typedef enum np_dump_target_type {
    NP_DUMP_TARGET_RESERVED_MEMORY,
    NP_DUMP_TARGET_BLOCK_DEVICE,
    NP_DUMP_TARGET_DEDICATED_PARTITION,
    NP_DUMP_TARGET_RECOVERY_VOLUME,
    NP_DUMP_TARGET_FIRMWARE_STORE,
    NP_DUMP_TARGET_NETWORK
} np_dump_target_type_t;
```

Ein Netzwerkziel ist standardmäßig deaktiviert und nur unter einer ausdrücklichen administrativen Richtlinie zulässig.

## 27. Zielpriorität

Empfohlene Priorität:

1. reservierter Minimal-Dump-Puffer,
2. dedizierte Crash-Dump-Partition,
3. verifiziertes Recovery-Volume,
4. reservierter Blockgerätebereich,
5. Firmware-Speicher für Minimalstatus,
6. autorisiertes Netzwerkziel.

Der Minimal Dump soll unabhängig vom Erfolg eines größeren Dumps erhalten bleiben.

## 28. Dedizierte Dump-Partition

Eine dedizierte Partition reduziert Abhängigkeiten auf komplexe Dateisysteme im Panic-Pfad.

Sie besitzt:

- eindeutige Partitionskennung,
- reservierte Slots,
- atomare Statusmarker,
- feste Größenlimits,
- Integritätsmetadaten,
- optionale Verschlüsselung.

Normale Anwendungen dürfen nicht direkt auf diese Partition zugreifen.

## 29. Dateisystemziel

Ein reguläres Dateisystemziel darf im Panic-Pfad nur verwendet werden, wenn ein speziell vorbereiteter crashsicherer Schreibpfad existiert.

Dieser darf nicht voraussetzen:

- reguläre Heapallokation,
- laufende Userspace-Dienste,
- normale Journalthreads,
- unbeschädigte Dateisystem-Sperren,
- dynamische Pfadauflösung.

Andernfalls wird erst in der Recovery- oder nächsten Bootphase aus reserviertem Speicher in eine Datei übertragen.

## 30. Dump-Writer

```c
typedef struct np_dump_writer_operations {
    np_status_t (*begin)(
        void* context,
        const np_crash_dump_header_t* header
    );

    np_status_t (*write)(
        void* context,
        uint64_t offset,
        const void* data,
        size_t size
    );

    np_status_t (*finish)(
        void* context,
        const np_crash_dump_header_t* final_header
    );

    void (*abort)(
        void* context
    );
} np_dump_writer_operations_t;
```

Panic-sichere Writer müssen ohne normale blockierende Kernelpfade funktionieren.

## 31. Raw-I/O-Pfad

Für Blockgeräte kann ein minimaler Raw-I/O-Pfad vorbereitet werden.

Er darf ausschließlich verwenden:

- vorab reservierte Requestobjekte,
- vorab zugewiesene DMA-Puffer,
- bekannte Blockbereiche,
- Polling oder panic-sichere Interruptverarbeitung,
- feste Timeouts.

Ein Treiber muss ausdrücklich als crash-dump-fähig gekennzeichnet und getestet sein.

## 32. Kompression

Dumpsektionen können komprimiert werden.

Zulässige Algorithmen müssen:

- bounded memory verwenden,
- ohne komplexe Laufzeitabhängigkeiten funktionieren,
- blockweise arbeiten,
- bei Fehlern einen unkomprimierten Fallback ermöglichen.

Kompression darf die Erstellung eines Minimal Dumps nicht verhindern.

## 33. Verschlüsselung

Crash Dumps können hochsensible Daten enthalten und sollen entsprechend der Sicherheitsrichtlinie verschlüsselt werden.

```c
typedef enum np_dump_encryption {
    NP_DUMP_ENCRYPTION_NONE,
    NP_DUMP_ENCRYPTION_SYSTEM_KEY,
    NP_DUMP_ENCRYPTION_RECOVERY_KEY,
    NP_DUMP_ENCRYPTION_ENTERPRISE_KEY
} np_dump_encryption_t;
```

Unverschlüsselte Full Dumps sind in produktiven Profilen standardmäßig unzulässig.

## 34. Dump-Schlüssel

Dump-Schlüssel können bereitgestellt werden durch:

- TPM-gestütztes Trust Backend,
- softwarebasierten geschützten Keystore,
- Recovery-Key,
- Unternehmens-Trust-Backend.

Ein TPM ist optional.

Kann kein geeigneter Schlüssel sicher bereitgestellt werden, muss auf ein datensparsames Minimal- oder Privacy-Profil zurückgefallen werden.

## 35. Integritätsschutz

Jede Dumpsektion besitzt eine Prüfsumme oder kryptografische Integritätsinformation.

Der Gesamtdump besitzt zusätzlich:

- Header-Integrität,
- Sektionsverzeichnis-Integrität,
- vollständigen Dump-Hash,
- optional eine Signatur oder MAC.

Beschädigte Sektionen werden als unzuverlässig markiert und nicht ungeprüft ausgewertet.

## 36. Atomare Fertigstellung

Der Dump wird zunächst mit dem Zustand `WRITING` angelegt.

Erst nach erfolgreichem Schreiben aller vorgesehenen Sektionen und Integritätsdaten wird der Status atomar auf `COMPLETE` gesetzt.

Bei Fehlern wird der Status `PARTIAL` gesetzt, soweit dies noch möglich ist.

## 37. Zeitlimits

Die Dump-Erstellung besitzt konfigurierbare Zeitlimits.

Diese können abhängig sein von:

- Dumpklasse,
- Zielmedium,
- Watchdogrichtlinie,
- Systemprofil,
- Batterie- oder Thermalzustand.

Nach Timeout wird der Dump als partiell abgeschlossen und die Panic Policy fortgesetzt.

## 38. Hardware-Watchdog

Vor einer längeren Dump-Erstellung kann ein Hardware-Watchdog:

- kontrolliert verlängert,
- in einen Dump-Modus versetzt,
- regelmäßig bestätigt,
- oder bewusst nicht deaktiviert werden.

Ein fehlerhafter Dump-Pfad darf das System nicht unbegrenzt hängen lassen.

## 39. Panic-Policy-Integration

Nach der Dump-Erstellung entscheidet die Panic Policy über:

- sicheren Halt,
- Neustart,
- Start der Recovery-Umgebung,
- Start einer älteren Systemgeneration,
- Abschaltung.

Das Crash Dump System selbst entscheidet nicht über die weitere Systemaktion.

## 40. Bootzeit-Erkennung

Beim nächsten Boot prüft NovaOS vorhandene Dumpziele.

Erkannt werden:

- neue vollständige Dumps,
- partielle Dumps,
- beschädigte Dumps,
- bereits exportierte Dumps,
- veraltete Dumps früherer Systemgenerationen.

Die Prüfung muss vor dem Überschreiben eines neuen reservierten Dumpbereichs erfolgen.

## 41. Recovery-Integration

Die Recovery-Umgebung kann:

- Dumps validieren,
- Dumps entschlüsseln, sofern autorisiert,
- technische Zusammenfassungen anzeigen,
- Dumps auf ein freigegebenes Medium exportieren,
- Dumps löschen,
- eine ältere Systemgeneration starten.

Recovery darf Dumpinhalte nicht automatisch extern übertragen.

## 42. Userspace-Dump-Service

Im normalen Betrieb verwaltet ein lokaler Crash Service:

- Userspace-Dumps,
- Dumpindex,
- Symbolzuordnung,
- Kompression,
- Aufbewahrungsrichtlinien,
- Benutzerhinweise,
- kontrollierten Export.

Der Kernel bleibt bei Ausfall dieses Dienstes funktionsfähig.

## 43. Symbolinformationen

Der Dump enthält Build IDs und Modul-Offsets, aber nicht zwingend vollständige Symbole.

Symbolauflösung erfolgt offline oder durch den lokalen Crash Service anhand passender Symbolpakete.

Ein Symbolpaket muss eindeutig zu Kernel, Modul oder Anwendung passen.

## 44. Stacktraces

Stacktraces werden nur erzeugt, wenn:

- der Stackbereich gültig erscheint,
- eine maximale Tiefe eingehalten wird,
- kein unkontrollierter Speicherzugriff erforderlich ist,
- Architektur- und Unwindinformationen verfügbar sind.

Bei beschädigtem Stack wird die Analyse abgebrochen und entsprechend markiert.

## 45. Kernel Object Graph

Ein Standard Dump kann eine reduzierte Momentaufnahme des Kernel Object Graph enthalten.

Erfasst werden können:

- Objekttyp und Objekt-ID,
- Zustand,
- Besitzerbeziehungen,
- Prozess- und Threadzuordnung,
- Geräte- und Treiberbindungen.

Objektinhalte und sensible Beziehungen werden entsprechend der Dump-Policy gefiltert.

## 46. Log-Integration

Das Crash Dump System übernimmt:

- letzten kritischen Reservepuffer,
- CPU-lokale Kernelringpuffer,
- Early-Boot-Logs, soweit vorhanden,
- Panic-Logs,
- ausgewählte Audit-Referenzen.

Secret-Felder bleiben ausgeschlossen.

Unvollständige oder beschädigte Logrecords werden nicht ungeprüft übernommen.

## 47. Audit

Mindestens folgende Ereignisse werden auditiert:

- Crash Dump erzeugt,
- Dump fehlgeschlagen,
- Dump entschlüsselt,
- Dump exportiert,
- Dump gelöscht,
- Dump-Policy geändert,
- Full Dump aktiviert,
- geschützte Dumpdaten geöffnet.

Audit enthält keine vollständigen Dumpinhalte.

## 48. Aufbewahrung

Aufbewahrungsrichtlinien können definieren:

- maximale Anzahl Dumps,
- maximales Gesamtvolumen,
- maximale Aufbewahrungsdauer,
- getrennte Regeln für Kernel- und Userspace-Dumps,
- Schutz des neuesten kritischen Dumps,
- automatische Löschung alter Dumps.

Ein voller Dumpbereich darf den Systemstart nicht blockieren.

## 49. Dump-Löschung

```c
np_status_t np_crash_dump_delete(
    np_crash_dump_handle_t dump,
    uint32_t flags
);
```

Abhängig vom Medium kann die Löschung umfassen:

- Entfernen der Referenz,
- kryptografisches Löschen des Dump-Schlüssels,
- Überschreiben der Metadaten,
- sicheres Löschen des Speicherbereichs.

Der Erfolg und die tatsächliche Löschgarantie müssen unterschieden werden.

## 50. Dump-Export

```c
np_status_t np_crash_dump_export(
    np_crash_dump_handle_t dump,
    np_handle_t target,
    uint32_t flags
);
```

Vor dem Export werden geprüft:

- Export-Capability,
- Dump-Sicherheitsklasse,
- Zielmedium,
- Verschlüsselungsstatus,
- Benutzer- oder Administratorfreigabe,
- Datenschutzrichtlinie.

Ein Export in ein Netzwerkziel benötigt eine zusätzliche ausdrückliche Freigabe.

## 51. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_DUMP_CREATE_USER` | Userspace-Dump anfordern |
| `CAP_DUMP_CREATE_KERNEL` | administrativen Kernel-Dump anfordern |
| `CAP_DUMP_QUERY` | Dump-Metadaten lesen |
| `CAP_DUMP_READ` | autorisierte Dumpsektionen lesen |
| `CAP_DUMP_DECRYPT` | verschlüsselten Dump öffnen |
| `CAP_DUMP_EXPORT` | Dump exportieren |
| `CAP_DUMP_DELETE` | Dump löschen |
| `CAP_DUMP_CONFIGURE` | Dump-Profile und Ziele konfigurieren |
| `CAP_DUMP_FULL_MEMORY` | Full-Memory-Dump aktivieren |
| `CAP_DUMP_ADMIN` | globale Dumpverwaltung |

Rechte müssen nach Dumpklasse und Sicherheitsstufe begrenzt werden können.

## 52. Crash-Dump-API

```c
np_status_t np_crash_dump_query(
    np_crash_dump_handle_t dump,
    np_crash_dump_information_t* information
);

np_status_t np_crash_dump_enumerate(
    np_crash_dump_information_t* dumps,
    size_t* dump_count
);

np_status_t np_crash_dump_create_process(
    np_process_handle_t process,
    np_dump_profile_t profile,
    np_crash_dump_handle_t* dump
);

np_status_t np_crash_dump_delete(
    np_crash_dump_handle_t dump,
    uint32_t flags
);
```

Kernel-Panic-Dumps werden über einen internen, nicht rückkehrenden Pfad erzeugt.

## 53. Sicherheit

Das Crash Dump System muss sicherstellen, dass:

- Dumps vor der Analyse vollständig validiert werden,
- Secret- und `NO_DUMP`-Speicher standardmäßig ausgeschlossen bleibt,
- Full Dumps in Produktivprofilen geschützt werden,
- Dump-Schlüssel nicht im selben ungeschützten Dump gespeichert werden,
- Kerneladressen nur autorisiert offengelegt werden,
- Dumpziele keine normalen Dateisystemgrenzen umgehen,
- Export und Entschlüsselung capability-geprüft und auditiert werden,
- unvollständige Dumps nicht als vollständig gelten,
- der Panic-Pfad keine unsicheren Userspace-Abhängigkeiten besitzt.

## 54. Datenschutz

Crash Dumps können personenbezogene und vertrauliche Daten enthalten.

Daher gelten:

- kleinstmöglicher geeigneter Dumpumfang,
- `PRIVACY` als verfügbares Produktivprofil,
- standardmäßige lokale Speicherung,
- keine automatische externe Übertragung,
- verschlüsselter Schutz sensibler Dumps,
- konfigurierbare Aufbewahrung,
- kontrollierter Export,
- sichere oder kryptografische Löschung,
- transparente Anzeige vorhandener Dumps.

## 55. Diagnose

Das Crash Dump System stellt ohne Öffnen des eigentlichen Inhalts folgende Metadaten bereit:

- Dump ID,
- Dumpklasse,
- Status,
- Erstellungszeit,
- Boot ID,
- Kernel Build ID,
- Systemgeneration,
- Größe,
- Verschlüsselungsstatus,
- Integritätsstatus,
,
- Fehlercode bei partiellen Dumps.

Detaillierte Sektionen benötigen erweiterte Rechte.

## 56. Statistik

```c
typedef struct np_crash_dump_statistics {
    uint64_t kernel_dumps_created;
    uint64_t userspace_dumps_created;
    uint64_t driver_dumps_created;

    uint64_t complete_dumps;
    uint64_t partial_dumps;
    uint64_t failed_dumps;
    uint64_t corrupted_dumps;

    uint64_t exported_dumps;
    uint64_t deleted_dumps;

    uint64_t total_stored_bytes;
    uint64_t maximum_write_time_ns;
} np_crash_dump_statistics_t;
```

Statistiken enthalten keine Dumpinhalte.

## 57. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_DUMP_NOT_FOUND` | Dump wurde nicht gefunden |
| `NP_ERR_DUMP_INCOMPLETE` | Dump ist unvollständig |
| `NP_ERR_DUMP_CORRUPT` | Integritätsprüfung fehlgeschlagen |
| `NP_ERR_DUMP_INCOMPATIBLE` | Format oder Build ist nicht kompatibel |
| `NP_ERR_DUMP_TARGET_FULL` | Ziel besitzt nicht genügend Kapazität |
| `NP_ERR_DUMP_TARGET_FAILED` | Schreiben auf das Ziel fehlgeschlagen |
| `NP_ERR_DUMP_KEY_UNAVAILABLE` | Verschlüsselungsschlüssel nicht verfügbar |
| `NP_ERR_DUMP_DECRYPTION` | Entschlüsselung fehlgeschlagen |
| `NP_ERR_DUMP_TIMEOUT` | Dump-Erstellung lief in einen Timeout |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 58. Fehlerbehandlung

Schlägt ein großer Dump fehl, muss nach Möglichkeit ein Minimal Dump erhalten bleiben.

Mögliche Reaktionen sind:

- Ziel wechseln,
- auf kleineres Profil zurückfallen,
- Kompression deaktivieren,
- nicht kritische Sektionen überspringen,
- Dump als `PARTIAL` abschließen,
- Fehlerstatus im reservierten Puffer speichern.

Ein Dumpfehler darf keine unendliche Panic- oder Neustartschleife erzeugen.

## 59. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. manueller Userspace-Dump,
2. Userspace-Dump nach Exception,
3. Treiber-Dump,
4. Minimal Kernel Dump,
5. Standard Kernel Dump,
6. Full Kernel Dump im autorisierten Profil,
7. Erfassung der Panic-CPU,
8. Anhalten und Erfassen weiterer CPUs,
9. Timeout einer nicht reagierenden CPU,
10. rekursive Panic während der Dump-Erstellung,
11. reservierter Minimal-Dump-Puffer,
12. Dump auf dedizierte Partition,
13. Raw-I/O-Dump-Pfad,
14. Zielausfall während des Schreibens,
15. Fallback auf Minimal Dump,
16. vollständiger Statuswechsel zu `COMPLETE`,
17. partieller Dump nach Timeout,
18. Erkennung eines unvollständigen Dumps,
19. Integritätsprüfung einer Sektion,
20. Erkennung manipulierter Dumpdaten,
21. Kompression und Dekompression,
22. Verschlüsselung mit TPM-Backend,
23. Verschlüsselung ohne TPM,
24. Fallback auf Privacy-Profil ohne Schlüssel,
25. Ausschluss von `NO_DUMP`-Seiten,
26. Ausschluss von Schlüsselmaterial,
27. Kerneladressfilter beim Export,
28. Übernahme der letzten Kernel-Logs,
29. begrenzter Stacktrace bei beschädigtem Stack,
30. Hibernate- und Systemgenerationserkennung,
31. Bootzeit-Erkennung eines neuen Dumps,
32. Recovery-Anzeige,
33. autorisierter Dump-Export,
34. Ablehnung eines unautorisierten Exports,
35. kryptografische Löschung,
36. Aufbewahrungs- und Größenlimits,
37. Hardware-Watchdog während des Dumps,
38. Capability- und Auditprüfung,
39. korrekte Diagnose und Statistik.

## 60. Verbindliche Invarianten

1. Ein Dump im Zustand `WRITING` gilt niemals als vollständig.
2. Der Minimal-Dump-Pfad benötigt keine reguläre Heapallokation.
3. Die Panic-CPU koordiniert höchstens eine primäre Kernel-Dump-Erstellung.
4. Rekursive Panic verwendet einen begrenzten Notfallpfad.
5. `SECRET`- und `NO_DUMP`-Speicher wird standardmäßig nicht aufgenommen.
6. Dump-Schlüssel werden nicht ungeschützt im zugehörigen Dump gespeichert.
7. Ein Dump wird vor der Analyse auf Format und Integrität geprüft.
8. Full Dumps erfordern eine ausdrückliche Sicherheitsrichtlinie.
9. Dump-Export und -Entschlüsselung werden capability-geprüft und auditiert.
10. Externe Übertragung erfolgt niemals automatisch als Standardverhalten.
11. Ein fehlgeschlagener großer Dump zerstört nicht den bereits vorhandenen Minimal Dump.
12. Die Dump-Erstellung endet durch Erfolg, Teilstatus oder definierten Timeout.

## 61. Referenzablauf eines Kernel-Dumps

```text
Kernel Panic wird ausgelöst
  -> Panic-CPU atomar bestimmen
  -> andere CPUs über Notfall-IPI anhalten
  -> CPU-Kontexte in reservierte Bereiche sichern
  -> Logging in Panic-Modus versetzen
  -> Minimal-Dump-Header mit WRITING anlegen
  -> Panic-, CPU-, Modul- und Logsektionen erfassen
  -> Minimal Dump dauerhaft sichern
  -> abhängig vom Profil weitere Sektionen sammeln
  -> sensible und NO_DUMP-Seiten filtern
  -> Sektionen komprimieren und verschlüsseln
  -> Integritätsinformationen schreiben
  -> Status auf COMPLETE oder PARTIAL setzen
  -> Panic Policy mit Halt, Neustart oder Recovery fortsetzen
```

## 62. Zusammenfassung

Das NovaOS Crash Dump System erzeugt versionierte und integritätsgeschützte Diagnoseabbilder für Userspace-, Treiber- und Kernelabstürze.

Ein reservierter, nicht blockierender Minimal-Dump-Pfad bleibt auch bei beschädigtem Kernelzustand verfügbar. Größere Dump-Profile ergänzen Speicher-, Objekt-, Geräte- und Logdaten, ohne `SECRET`- oder `NO_DUMP`-Bereiche standardmäßig aufzunehmen.

Verschlüsselung, Capability-Prüfungen, lokale Speicherung, kontrollierter Export und konfigurierbare Aufbewahrung schützen die möglicherweise sensiblen Dumpdaten. Ein TPM kann als Trust Backend dienen, ist aber keine zwingende Voraussetzung.