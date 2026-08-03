# NPSPEC-KERNEL-0025 – Module Loader

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0025 |
| Titel | Module Loader |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Modulverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0007, NPSPEC-KERNEL-0008, NPSPEC-KERNEL-0011, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0016, NPSPEC-KERNEL-0020, NPSPEC-KERNEL-0023 |
| Zugehörige ADRs | ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105, ADR-DEV-0002, ADR-DEV-0003, ADR-SEC-0002, ADR-SEC-0004, ADR-SEC-0008 |

---

## 1. Zweck

Diese Spezifikation definiert den Module Loader des NovaOS-Kernels.

Der Module Loader verwaltet:

- Laden von Kernelmodulen,
- Modulvalidierung,
- Signatur- und Integritätsprüfung,
- ABI-Kompatibilität,
- Symbolauflösung,
- Relokationen,
- Modulabhängigkeiten,
- Initialisierung und Aktivierung,
- Entladung,
- Live-Update und Rollback,
- Diagnose und Audit.

## 2. Ziele

Der Module Loader muss:

- ausschließlich kompatible und autorisierte Module laden,
- Kernel-ABI-Grenzen erzwingen,
- W^X und NX konsequent anwenden,
- Abhängigkeiten deterministisch auflösen,
- teilweise geladene Module vollständig zurückrollen,
- Modulcode erst nach vollständiger Validierung ausführen,
- sichere Entladung ermöglichen,
- alte und aktuelle Systemgenerationen unterscheiden,
- Kerneladressen nicht unnötig offenlegen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Module Loader | Laden, Linken und Lebenszyklus von Kernelmodulen |
| Security Manager | Signatur, Trust und Policy |
| VFS | Lesen der Moduldatei |
| VMM | virtuelle Modulabbildungen und Schutzrechte |
| Kernel Heap | Laufzeitmetadaten |
| Object Manager | Modulobjekte und Beziehungen |
| Driver Framework | Registrierung von Treibern |
| System Call Interface | versionierte öffentliche Kernel-Services |
| Logging Framework | Moduldiagnose |

## 4. Modularten

```c
typedef enum np_module_type {
    NP_MODULE_KERNEL_CORE_EXTENSION,
    NP_MODULE_DRIVER,
    NP_MODULE_FILESYSTEM,
    NP_MODULE_NETWORK_PROTOCOL,
    NP_MODULE_SECURITY_POLICY,
    NP_MODULE_DIAGNOSTIC,
    NP_MODULE_PLATFORM,
    NP_MODULE_COMPATIBILITY
} np_module_type_t;
```

Die Modulart bestimmt zulässige Abhängigkeiten, Importgruppen und Sicherheitsrichtlinien.

## 5. Grundprinzip

Ein Kernelmodul ist kein beliebiger im Kernel ausführbarer Binärcode.

Es muss:

- ein gültiges NovaOS-Modulformat besitzen,
- zum Kernel und zur Architektur passen,
- eine zulässige Signatur besitzen,
- ausschließlich freigegebene Kernel-Symbole importieren,
- alle Schutz- und Lifecycle-Regeln erfüllen.

Kernelmodule sollen nur verwendet werden, wenn eine Userspace-Komponente technisch nicht ausreicht.

## 6. Modulkennung

```c
typedef np_uuid_t np_module_id_t;
typedef uint64_t np_module_instance_id_t;
```

Die Module ID identifiziert das Modulprojekt oder Paket dauerhaft.

Die Instance ID identifiziert eine konkrete geladene Instanz während eines Bootvorgangs.

Beide Kennungen verleihen keine Zugriffsrechte.

## 7. Modulzustände

```c
typedef enum np_module_state {
    NP_MODULE_DISCOVERED,
    NP_MODULE_VALIDATING,
    NP_MODULE_LOADING,
    NP_MODULE_RELOCATING,
    NP_MODULE_INITIALIZING,
    NP_MODULE_ACTIVE,
    NP_MODULE_QUIESCING,
    NP_MODULE_UNLOADING,
    NP_MODULE_FAILED,
    NP_MODULE_UNLOADED
} np_module_state_t;
```

Nur Module im Zustand `ACTIVE` dürfen reguläre Services bereitstellen.

## 8. Modulformat

NovaOS verwendet ein klar definiertes relocatable Modulformat.

Die initiale Implementierung kann ein eingeschränktes ELF-Format verwenden, sofern:

- ausschließlich freigegebene Abschnittstypen akzeptiert werden,
- alle Größen und Offsets validiert werden,
- unbekannte oder dynamische Host-Linker-Funktionen abgelehnt werden,
- das Format durch ein NovaOS-spezifisches Manifest ergänzt wird.

Das Dateiformat ist nicht identisch mit der stabilen Kernel-Service-ABI.

## 9. Modulmanifest

```c
typedef struct np_module_manifest {
    np_abi_header_t header;

    np_module_id_t module_id;
    np_string_view_t name;
    np_module_version_t version;
    np_module_type_t type;

    np_architecture_t architecture;
    np_abi_range_t kernel_abi;

    np_module_dependency_list_t dependencies;
    np_module_import_policy_t import_policy;

    uint64_t feature_flags;
    uint64_t security_flags;
} np_module_manifest_t;
```

Das Manifest ist Bestandteil der signierten Moduldaten.

## 10. Manifestanforderungen

Das Manifest enthält mindestens:

- Modul-ID,
- Name und Hersteller,
- Modulversion,
- Architektur,
- Modultyp,
- unterstützte Kernel-ABI,
- erforderliche Kernelservices,
- Abhängigkeiten,
- exportierte Services,
- Sicherheitsprofil,
- Entladefähigkeit,
- Live-Update-Fähigkeit,
- Integritäts- und Signaturinformationen.

Fehlende zwingende Angaben führen zur Ablehnung.

## 11. Modulversion

```c
typedef struct np_module_version {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t revision;
} np_module_version_t;
```

Versionsvergleiche erfolgen anhand definierter numerischer Felder und nicht anhand frei formatierter Zeichenketten.

Eine neue Version mit inkompatibler Zustands- oder Service-ABI benötigt eine neue Major-Version.

## 12. Kernel-ABI-Kompatibilität

Ein Modul deklariert einen unterstützten ABI-Bereich:

```c
typedef struct np_abi_range {
    uint16_t minimum_major;
    uint16_t minimum_minor;
    uint16_t maximum_major;
    uint16_t maximum_minor;
} np_abi_range_t;
```

Eine inkompatible Major-Version wird abgelehnt.

Build IDs können zusätzlich verwendet werden, dürfen aber eine sauber versionierte ABI nicht vollständig ersetzen.

## 13. Signaturprüfung

Vor jeder Codeausführung prüft der Security Manager:

- Signatur des Modulpakets,
- Hash aller geladenen Abschnitte,
- Modulmanifest,
- Herausgeber,
- Trust Chain,
- Sperrlisten,
- Systemgeneration,
- Sicherheitsprofil.

Eine Signatur über das Manifest allein reicht nicht aus. Alle relevanten Code- und Datensektionen müssen abgedeckt sein.

## 14. Unsichere Module

Unsichere, unsignierte oder manipulierte Module werden in Produktivprofilen abgelehnt.

Ein Entwicklungsprofil kann Ausnahmen erlauben, muss dann jedoch:

- den Sicherheitsstatus auf `DEGRADED` setzen,
- den Vorgang auditieren,
- geschützte Schlüssel und Unternehmensfunktionen gegebenenfalls sperren,
- den Debugstatus sichtbar kennzeichnen.

Recovery darf nicht automatisch beliebige unsignierte Module laden.

## 15. Ladequellen

Module können geladen werden aus:

- verifizierter Systemgeneration,
- Initial-RAM-Dateisystem,
- Recovery-System,
- signiertem Treiberpaket,
- administrativ freigegebenem Updatepaket.

Module aus regulären Benutzerverzeichnissen werden nicht direkt als Kernelcode geladen.

## 16. Ladeanforderung

```c
np_status_t np_module_load(
    const np_module_load_request_t* request,
    np_module_handle_t* module
);
```

Die Anfrage enthält:

- Modulquelle,
- erwartete Modul-ID,
- optionale Version,
- Ladeflags,
- Sicherheitskontext,
- Ziel-Systemgeneration.

Der aufrufende Prozess benötigt eine passende Modulverwaltungs-Capability.

## 17. Ladephasen

Der Ladeprozess erfolgt in folgenden Phasen:

```text
Moduldatei öffnen
  -> Datei und Größen validieren
  -> Manifest lesen
  -> Signatur und Trust prüfen
  -> Architektur und ABI prüfen
  -> Abhängigkeiten auflösen
  -> virtuellen Bereich reservieren
  -> Sektionen kopieren
  -> Symbole auflösen
  -> Relokationen anwenden
  -> Speicherrechte finalisieren
  -> Stack Protector und Schutzmetadaten prüfen
  -> Initialisierung ausführen
  -> Modul atomar veröffentlichen
```

Vor der Veröffentlichung bleibt das Modul für normale Aufrufer unsichtbar.

## 18. Dateivalidierung

Vor dem Laden werden geprüft:

- Magic Number,
- Dateiformatversion,
- Headergrößen,
- Abschnittsanzahl,
- Offsets und Längen,
- Ausrichtung,
- Überlappungen,
- Stringtabellen,
- Symboltabellen,
- Relokationstabellen,
- maximale Dateigröße.

Jede Größenberechnung muss auf Integerüberlauf geprüft werden.

## 19. Erlaubte Sektionen

Typische zulässige Sektionen sind:

- ausführbarer Code,
- schreibgeschützte Daten,
- veränderliche Daten,
- BSS,
- Symbolinformationen,
- Relokationen,
- Modulmanifest,
- Initialisierungs- und Beendigungsmetadaten,
- Unwind- und Diagnoseinformationen.

Unbekannte zwingende Sektionen führen zur Ablehnung.

Debugsektionen können vor der produktiven Abbildung entfernt werden.

## 20. Moduladressraum

Module werden in einen reservierten Bereich des Kerneladressraums geladen.

Die Position wird durch Kernel ASLR variiert, soweit Architektur und Adressraum dies erlauben.

Module dürfen:

- nicht in Userspace-Bereiche geladen werden,
- keine anderen Kernelmappings überlappen,
- keine festen Kerneladressen voraussetzen,
- keine nicht autorisierten physischen Bereiche abbilden.

## 21. Speicherrechte

Nach Abschluss der Relokation gelten:

| Abschnitt | Lesen | Schreiben | Ausführen |
|---|---:|---:|---:|
| Code | Ja | Nein | Ja |
| Read-only Data | Ja | Nein | Nein |
| Data | Ja | Ja | Nein |
| BSS | Ja | Ja | Nein |
| Init Code | Ja | Nein | Ja |
| Init Data | Ja | abhängig | Nein |

Write-and-Execute-Mappings sind unzulässig.

## 22. Relokationsphase

Während Relokationen kann eine Sektion vorübergehend schreibbar sein.

Der Ablauf ist:

1. Zielbereich nicht ausführbar abbilden,
2. Inhalt kopieren,
3. Relokationen validieren und anwenden,
4. Instruction Cache synchronisieren,
5. endgültige Rechte aktivieren,
6. TLB- und Cachezustand aktualisieren.

Zu keinem Zeitpunkt darf derselbe Bereich gleichzeitig beschreibbar und ausführbar sein.

## 23. Relokationstypen

Nur ausdrücklich unterstützte Relokationstypen der Zielarchitektur werden akzeptiert.

Jeder Eintrag wird geprüft auf:

- gültigen Zielabschnitt,
- gültigen Offset,
- korrekte Breite,
- zulässigen Symboltyp,
- Überlauf,
- erlaubten Adressbereich.

Unbekannte oder abgeschnittene Relokationen führen zur Ablehnung des Moduls.

## 24. Kernel-Symbole

Der Kernel exportiert ausschließlich ausdrücklich freigegebene Symbole.

```c
typedef struct np_exported_symbol {
    const char* name;
    uintptr_t address;

    np_symbol_version_t version;
    np_module_id_t owner;

    uint32_t type;
    uint32_t flags;
} np_exported_symbol_t;
```

Interne Symbole sind für Module nicht sichtbar.

## 25. Symbolversionierung

Ein importiertes Symbol enthält:

- Symbolname oder stabile ID,
- erforderliche Major-Version,
- minimale Minor-Version,
- erwarteten Typ oder ABI-Hash.

Ein gleichnamiges Symbol mit inkompatibler Version darf nicht gebunden werden.

Symbolversionierung ergänzt die Service-ABI, ersetzt sie aber nicht.

## 26. Symbolnamensräume

Exports werden logisch gruppiert:

```text
kernel.core
kernel.memory
kernel.object
kernel.device
kernel.vfs
kernel.network
kernel.security
kernel.diagnostic
```

Ein Modul darf nur aus Namensräumen importieren, die für seinen Typ und sein Sicherheitsprofil freigegeben sind.

## 27. Modul-Exports

Ein Modul kann eigene Symbole oder Services exportieren, wenn das Manifest dies deklariert.

Ein Export wird erst sichtbar, wenn:

- das Modul vollständig initialisiert ist,
- Version und Typ registriert wurden,
- keine Namens- oder Versionskonflikte bestehen,
- die Sicherheitsrichtlinie den Export erlaubt.

Exports werden vor der endgültigen Modulentladung entfernt.

## 28. Abhängigkeiten

```c
typedef struct np_module_dependency {
    np_module_id_t module_id;
    np_abi_range_t version_range;
    uint32_t flags;
} np_module_dependency_t;
```

Abhängigkeiten können sein:

- zwingend,
- optional,
- Feature-abhängig,
- nur zur Initialisierung erforderlich.

Zwingende Abhängigkeiten müssen vor dem Modul aktiv sein.

## 29. Abhängigkeitsgraph

Der Module Loader verwaltet Abhängigkeiten als Graph im Kernel Object Graph.

Er muss:

- fehlende Abhängigkeiten erkennen,
- Versionen prüfen,
- Zyklen erkennen,
- Initialisierungsreihenfolge bestimmen,
- Entladereihenfolge bestimmen,
- Referenzen zwischen Modulen verwalten.

Zyklische zwingende Initialisierungsabhängigkeiten werden abgelehnt.

## 30. Automatisches Nachladen

Zwingende Abhängigkeiten können automatisch geladen werden, wenn:

- die Policy dies erlaubt,
- die Quelle zur selben vertrauenswürdigen Systemgeneration gehört,
- Modul-ID und Version eindeutig sind,
- keine Ladezyklen entstehen.

Unkontrolliertes Nachladen anhand frei beeinflussbarer Symbolnamen ist unzulässig.

## 31. Modulinitialisierung

Jedes Modul besitzt einen definierten Initialisierungseinstieg:

```c
np_status_t np_module_initialize(
    const np_module_environment_t* environment,
    np_module_registration_t* registration
);
```

Der Einstieg erhält ausschließlich versionierte Kernelservices und einen begrenzten Modulkontext.

Er darf keine unveröffentlichten internen Kernelstrukturen voraussetzen.

## 32. Initialisierungsregeln

Während der Initialisierung darf ein Modul:

- Objekttypen registrieren,
- Treiber oder Dateisysteme registrieren,
- Kernelservices entsprechend seiner Policy verwenden,
- interne Caches und Zustände aufbauen.

Es darf nicht:

- sich vor erfolgreichem Abschluss als aktiv veröffentlichen,
- unkontrollierte Threads zurücklassen,
- Interrupts ohne vollständige Handler aktivieren,
- nicht deklarierte privilegierte Ressourcen verwenden.

## 33. Initialisierungsfehler

Schlägt die Initialisierung fehl, muss der Loader:

1. neue Aufrufe verhindern,
2. Teilregistrierungen zurücknehmen,
3. erzeugte Threads stoppen,
4. Interrupts und Timer entfernen,
5. Objekt- und Modulbeziehungen lösen,
6. Exports entfernen,
7. Speicherbereiche sicher freigeben,
8. Zustand auf `FAILED` setzen.

Fehlerhafte Initialisierung darf kein teilweise aktives Modul zurücklassen.

## 34. Initialisierungssektionen

Als `INIT_ONLY` markierter Code und Daten können nach erfolgreicher Initialisierung freigegeben werden.

Voraussetzungen sind:

- keine verbleibenden Funktionszeiger,
- keine aktiven Threads in diesem Code,
- keine Callbackregistrierungen,
- keine Unwind- oder Diagnosereferenzen, die den Bereich benötigen.

Freigegebene Init-Sektionen dürfen niemals erneut ausgeführt werden.

## 35. Modulobjekt

```c
typedef struct np_module {
    np_kernel_object_t object;

    np_module_id_t module_id;
    np_module_instance_id_t instance_id;
    np_module_version_t version;

    np_module_type_t type;
    np_module_state_t state;

    np_virtual_address_t image_base;
    uint64_t image_size;

    np_module_dependency_graph_t dependencies;
    np_module_export_table_t exports;

    np_atomic_uint64_t active_calls;
    uint32_t flags;
} np_module_t;
```

Das Modulobjekt hält alle für Lebenszyklus und Diagnose notwendigen Metadaten.

## 36. Modulhandles

Module werden über capability-geschützte Handles verwaltet.

Ein Handle kann getrennte Rechte besitzen für:

- Query,
- Load,
- Activate,
- Quiesce,
- Unload,
- Update,
- Diagnose,
- Administration.

Eine Modul-ID allein erlaubt keine Moduloperation.

## 37. Aktive Aufrufe

Jeder modulfremde Eintritt in Modulcode muss dessen Lebenszeit schützen.

Dies kann erfolgen über:

- aktive Aufrufzähler,
- referenzgezählte Serviceobjekte,
- RCU- oder Epoch-Verfahren,
- dispatcherbasierte Indirektion.

Ein Modul darf nicht entladen werden, solange eine CPU seinen Code ausführen kann.

## 38. Modulentladung

```c
np_status_t np_module_unload(
    np_module_handle_t module,
    uint32_t flags
);
```

Eine reguläre Entladung erfolgt:

1. neue Referenzen und Aufrufe verhindern,
2. Modul auf `QUIESCING` setzen,
3. abhängige Module prüfen,
4. Dienste und Exports zurückziehen,
5. Threads, Timer und Work Items stoppen,
6. Interrupt Handler entfernen,
7. aktive Aufrufe abwarten,
8. Beendigungsroutine ausführen,
9. Speicherrechte entfernen,
10. Modulabbildung freigeben.

## 39. Modul-Beendigungsroutine

```c
np_status_t np_module_shutdown(
    np_module_shutdown_reason_t reason
);
```

Die Routine muss:

- idempotent oder sicher gegen Wiederholung sein,
- keine neuen dauerhaften Ressourcen erzeugen,
- keine entfernten Exports verwenden,
- alle modulinternen Ressourcen freigeben,
- definierte Timeouts einhalten.

Ein Fehler verhindert die Entladung, sofern kein sicherer erzwungener Recovery-Pfad existiert.

## 40. Nicht entladbare Module

Module können als nicht entladbar markiert sein, wenn sie:

- kritische Kernelstrukturen erweitern,
- frühe Plattformfunktionen bereitstellen,
- aktive Seitentabellen- oder Interruptpfade besitzen,
- keine sichere Quiesce-Strategie unterstützen,
- für Root-Dateisystem oder Systemstart zwingend sind.

Ein erzwungenes Entfernen solchen Codes im laufenden System ist unzulässig.

## 41. Abhängige Module

Ein Modul kann nicht entladen werden, solange aktive zwingende Abhängige bestehen.

Mögliche Aktionen sind:

- Entladung ablehnen,
- abhängige Module in umgekehrter Reihenfolge entladen,
- Updategruppe transaktional austauschen.

Automatische Kaskaden benötigen eine ausdrückliche administrative Anforderung.

## 42. Live-Update

Ein live aktualisierbares Modul muss deklarieren:

- stabile externe Service-ABI,
- Zustandsformat,
- Quiesce-Fähigkeit,
- Export- und Importkompatibilität,
- Rollback-Fähigkeit,
- maximale Übergangszeit.

Nicht jedes Kernelmodul ist für Live-Update geeignet.

## 43. Live-Update-Ablauf

```text
Neue Modulversion laden und validieren
  -> neue Instanz noch nicht veröffentlichen
  -> alte Instanz quiescen
  -> laufende Aufrufe abschließen
  -> Zustand exportieren
  -> Zustand in neue Instanz importieren
  -> Services atomar umschalten
  -> neue Instanz aktivieren
  -> alte Instanz beobachten
  -> nach Grace Period entladen
```

Bei Fehlern wird auf die alte Instanz zurückgeschaltet, sofern diese noch sicher fortsetzbar ist.

## 44. Update-Zustand

Modulzustand für Live-Update muss:

- versioniert,
- größenvalidiert,
- gegen Manipulation geschützt,
- frei von direkten alten Codezeigern,
- frei von unkontrollierten Kerneladressen,
- rückwärtskompatibel oder migrierbar sein.

Nicht serialisierbare Zustände können ein Live-Update verhindern.

## 45. Rollback

Rollback muss möglich sein, solange:

- die alte Instanz noch vorhanden ist,
- ihre Ressourcen nicht irreversibel verändert wurden,
- keine inkompatible externe Zustandsänderung erfolgte,
- abhängige Services zurückgeschaltet werden können.

Ein Modul darf keine Rollback-Fähigkeit deklarieren, wenn sein Update irreversible Hardware- oder Datenformatänderungen erzwingt.

## 46. Kernel-ASLR

Module werden an zufällig ausgewählten zulässigen Kerneladressen geladen.

Die Zufälligkeit wird aus dem während der Bootsequenz initialisierten Sicherheitszustand abgeleitet.

Vollständige Modulbasen werden nur im autorisierten Secure-Debugging- oder Crash-Dump-Kontext offengelegt.

## 47. Stack Protector

Kernelmodule werden mit kompatibler Stack-Protector-Konfiguration gebaut.

Der Loader prüft:

- erforderliche Stack-Protector-Symbole,
- ABI-Kompatibilität,
- Modul-Build-Flags,
- Ausschluss unzulässiger eigener Canary-Implementierungen.

Module ohne vorgeschriebene Härtung werden in Produktivprofilen abgelehnt.

## 48. Control-Flow-Schutz

Wenn Plattform und Toolchain entsprechende Funktionen unterstützen, kann der Loader prüfen:

- Control-Flow Integrity,
- CET oder vergleichbare Mechanismen,
- gültige Indirect-Branch-Ziele,
- Shadow-Stack-Kompatibilität,
- signierte Call-Target-Metadaten.

Fehlende optionale Unterstützung wird im Sicherheitsstatus dokumentiert.

## 49. Ausnahme- und Unwind-Informationen

Ein Modul kann versionierte Unwind- oder Exception-Metadaten bereitstellen.

Diese werden verwendet für:

- sichere Stacktraces,
- Crash Dumps,
- autorisiertes Debugging,
- begrenzte Exception-Fixup-Tabellen.

Recovery-Einträge müssen statisch validiert und auf Modulcodebereiche begrenzt sein.

## 50. Modulfehler

Fehler eines Kernelmoduls können behandelt werden durch:

- einzelne Operation ablehnen,
- zugehöriges Gerät isolieren,
- Modul quiescen,
- abhängige Userspace-Dienste neu starten,
- Rollback auf vorherige Version,
- System in Degraded Mode versetzen,
- Kernel-Panic-Policy anwenden.

Eine lokale Fortsetzung ist nur zulässig, wenn keine kritische Kernelintegrität verletzt wurde.

## 51. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_MODULE_QUERY` | Modulmetadaten lesen |
| `CAP_MODULE_LOAD` | autorisierte Module laden |
| `CAP_MODULE_UNLOAD` | entladbare Module entfernen |
| `CAP_MODULE_UPDATE` | Live-Update ausführen |
| `CAP_MODULE_DIAGNOSTIC` | detaillierte Modulinformationen lesen |
| `CAP_MODULE_UNSAFE` | Entwicklungsmodul unter Degraded Security laden |
| `CAP_MODULE_POLICY` | Modulrichtlinien verwalten |
| `CAP_MODULE_ADMIN` | globale Modulverwaltung |

`CAP_MODULE_UNSAFE` ist in gehärteten Produktivprofilen nicht verfügbar.

## 52. Module-Loader-API

```c
np_status_t np_module_load(
    const np_module_load_request_t* request,
    np_module_handle_t* module
);

np_status_t np_module_query(
    np_module_handle_t module,
    np_module_information_t* information
);

np_status_t np_module_unload(
    np_module_handle_t module,
    uint32_t flags
);

np_status_t np_module_update(
    np_module_handle_t module,
    const np_module_update_request_t* request
);
```

Alle öffentlichen Strukturen besitzen Größen- und ABI-Versionsfelder.

## 53. Audit

Mindestens folgende Operationen werden auditiert:

- Modul geladen,
- Modul abgelehnt,
- Signaturprüfung fehlgeschlagen,
- unsicheres Modul im Entwicklungsmodus geladen,
- Modul entladen,
- Modulupdate,
- Rollback,
- Modulpolicy geändert,
- Sperrlistentreffer.

Audit enthält Hash, Modul-ID, Version und Sicherheitsstatus, aber keinen vollständigen Modulcode.

## 54. Diagnose

Der Module Loader stellt autorisierten Diagnosediensten bereit:

- geladene Module,
- Modul-ID und Version,
- Status,
- Abhängigkeiten,
- exportierte Services,
- ABI-Bereich,
- Signatur- und Trust-Status,
- aktive Aufrufzahl,
- Speicherverbrauch,
- Update- und Rollbackstatus,
- letzte Ladefehler.

Vollständige Kerneladressen erfordern Secure-Debugging-Rechte.

## 55. Statistik

```c
typedef struct np_module_loader_statistics {
    uint64_t loaded_modules;
    uint64_t active_modules;

    uint64_t successful_loads;
    uint64_t failed_loads;
    uint64_t signature_failures;
    uint64_t abi_rejections;

    uint64_t unloads;
    uint64_t failed_unloads;

    uint64_t live_updates;
    uint64_t update_rollbacks;
} np_module_loader_statistics_t;
```

Statistiken sollen pro Modultyp und System verfügbar sein.

## 56. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_MODULE_INVALID` | Modulformat ist ungültig |
| `NP_ERR_MODULE_NOT_FOUND` | Modul wurde nicht gefunden |
| `NP_ERR_MODULE_ALREADY_LOADED` | Modul ist bereits aktiv |
| `NP_ERR_MODULE_SIGNATURE` | Signaturprüfung fehlgeschlagen |
| `NP_ERR_MODULE_REVOKED` | Modul oder Herausgeber ist gesperrt |
| `NP_ERR_ABI_INCOMPATIBLE` | Kernel-ABI ist nicht kompatibel |
| `NP_ERR_ARCH_INCOMPATIBLE` | Architektur stimmt nicht überein |
| `NP_ERR_SYMBOL_UNRESOLVED` | erforderliches Symbol fehlt |
| `NP_ERR_SYMBOL_VERSION` | Symbolversion ist inkompatibel |
| `NP_ERR_RELOCATION_INVALID` | Relokation ist ungültig |
| `NP_ERR_DEPENDENCY_MISSING` | zwingende Abhängigkeit fehlt |
| `NP_ERR_DEPENDENCY_CYCLE` | Abhängigkeitszyklus erkannt |
| `NP_ERR_MODULE_BUSY` | Modul wird noch verwendet |
| `NP_ERR_MODULE_NOT_UNLOADABLE` | Modul darf nicht entladen werden |
| `NP_ERR_UPDATE_ROLLBACK` | Update wurde zurückgerollt |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |

## 57. Sicherheit

Der Module Loader muss sicherstellen, dass:

- Modulcode niemals vor vollständiger Prüfung ausgeführt wird,
- alle Datei-, Abschnitts- und Relokationsgrößen validiert werden,
- ausschließlich freigegebene Kernel-Symbole importiert werden,
- Module ihre deklarierten Importgruppen nicht überschreiten,
- W^X während Laden und Betrieb erhalten bleibt,
- Module nur in Kernel-Modulbereiche geladen werden,
- Signaturen Code, Daten und Manifest abdecken,
- unsichere Module den System-Sicherheitsstatus reduzieren,
- Modulcode nicht entladen wird, solange er ausgeführt werden kann,
- Kerneladressen nicht über unprivilegierte APIs offengelegt werden.

## 58. Datenschutz

Modulmetadaten können Hersteller-, Geräte- oder Systeminformationen enthalten.

Daher gelten:

- unprivilegierte Abfragen liefern nur notwendige Daten,
- vollständige Ladepfade können ausgeblendet werden,
- Kerneladressen bleiben geschützt,
- Modulcode wird nicht durch normale Diagnose-APIs exportiert,
- Logs und Audit bleiben standardmäßig lokal,
- externe Übertragung ist standardmäßig deaktiviert.

## 59. Fehlerbehandlung

Fehler vor der Aktivierung führen zu einem vollständigen Rollback der Ladeoperation.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- Ausführung aus einem noch beschreibbaren Modulbereich,
- Aufruf von bereits entladenem Modulcode,
- beschädigte globale Symboltabelle,
- Modul überschreibt fremde Kernelbereiche,
- aktive Interrupt- oder Callbackreferenz nach Entladung,
- Rechteausweitung durch nicht autorisierten Import,
- Modulinitialisierung beschädigt zentrale Kernelstrukturen.

Je nach Auswirkung wird der Ladeversuch abgebrochen, das Modul isoliert oder die Kernel-Panic-Policy aktiviert.

## 60. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Laden eines gültigen Moduls,
2. Ablehnung eines ungültigen Dateiformats,
3. Erkennung überlappender Sektionen,
4. Größen- und Offsetüberlauf,
5. gültige Signatur,
6. ungültige Signatur,
7. gesperrter Herausgeber,
8. inkompatible Architektur,
9. kompatible Kernel-ABI,
10. inkompatible ABI-Major-Version,
11. Auflösung eines gültigen Symbols,
12. unbekanntes Symbol,
13. inkompatible Symbolversion,
14. zulässige Relokation,
15. unbekannter Relokationstyp,
16. Relokationsüberlauf,
17. W^X während der Relokation,
18. zufällige Moduladresse,
19. zwingende Abhängigkeit,
20. optionale Abhängigkeit,
21. fehlende Abhängigkeit,
22. Abhängigkeitszyklus,
23. erfolgreicher Initialisierungseinstieg,
24. Rollback nach Initialisierungsfehler,
25. Freigabe von Init-Sektionen,
26. Modul-Export,
27. parallele Modulaufrufe,
28. reguläre Entladung,
29. Ablehnung der Entladung bei aktivem Aufruf,
30. nicht entladbares Modul,
31. Entladung abhängiger Module,
32. Live-Update,
33. Zustandstransfer,
34. Rollback nach fehlgeschlagenem Update,
35. Stack-Protector-Prüfung,
36. Control-Flow-Schutzprüfung,
37. Crash Dump mit Modulinformationen,
38. Capability- und Auditprüfung,
39. korrekte Diagnose und Statistik.

## 61. Verbindliche Invarianten

1. Nicht vollständig validierter Modulcode wird niemals ausgeführt.
2. Jedes aktive Modul besitzt eine eindeutige Modul- und Instanzkennung.
3. Ein Modul importiert ausschließlich ausdrücklich freigegebene Kernel-Symbole.
4. Modulcode ist ausführbar, aber nicht beschreibbar.
5. Moduldaten sind beschreibbar, aber nicht ausführbar.
6. Ein Modul wird erst nach erfolgreicher Initialisierung veröffentlicht.
7. Fehlgeschlagene Ladeoperationen hinterlassen keine aktiven Registrierungen.
8. Zwingende Abhängigkeiten bleiben während der Modullebenszeit referenziert.
9. Modulcode wird nicht freigegeben, solange eine CPU ihn ausführen kann.
10. Nicht entladbare Module werden im laufenden System nicht erzwungen entfernt.
11. Live-Updates schalten Services atomar auf die alte oder neue Instanz.
12. Modulzustandsänderungen erfolgen unter geeigneter Synchronisation.

## 62. Referenzablauf eines Modulladevorgangs

```text
Ladeanforderung empfangen
  -> Capability und Quelle prüfen
  -> Moduldatei vollständig validieren
  -> Manifest, Hash und Signatur prüfen
  -> Architektur und Kernel-ABI prüfen
  -> Abhängigkeiten auflösen und referenzieren
  -> zufälligen Kerneladressbereich reservieren
  -> Sektionen nicht ausführbar abbilden und kopieren
  -> Symbole und Versionen auflösen
  -> Relokationen validieren und anwenden
  -> Speicherrechte auf RX, RO und RW finalisieren
  -> Instruction Cache synchronisieren
  -> Modulinitialisierung ausführen
  -> Exports und Services atomar veröffentlichen
  -> Modulzustand auf ACTIVE setzen
  -> Ladeereignis auditieren
```

## 63. Zusammenfassung

Der NovaOS Module Loader lädt ausschließlich verifizierte, ABI-kompatible und sicher relocatable Kernelmodule.

Signaturprüfung, eingeschränkte Symbolnamensräume, versionierte Imports, Kernel ASLR und eine konsequente W^X-Trennung schützen die Kernelgrenze. Module werden erst nach erfolgreicher Initialisierung sichtbar und bei Fehlern vollständig zurückgerollt.

Referenzierte Abhängigkeiten, aktive Aufrufzähler und Grace Periods verhindern die Freigabe noch verwendeten Codes. Geeignete Module können über einen transaktionalen Zustands- und Servicewechsel live aktualisiert und bei Fehlern auf die vorherige Version zurückgerollt werden.