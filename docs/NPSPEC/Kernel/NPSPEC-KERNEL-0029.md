# NPSPEC-KERNEL-0029 – Kernel Configuration

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0029 |
| Titel | Kernel Configuration |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Konfiguration |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0011, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0020, NPSPEC-KERNEL-0023, NPSPEC-KERNEL-0025 |
| Zugehörige ADRs | ADR-KERNEL-0104, ADR-KERNEL-0105, ADR-SEC-0001, ADR-SEC-0008, ADR-SEC-0010 |

---

## 1. Zweck

Diese Spezifikation definiert das Kernel Configuration Framework von NovaOS.

Das Framework verwaltet:

- Build-Konfiguration,
- Kernel-Feature-Profile,
- Bootparameter,
- Plattformkonfiguration,
- Laufzeitparameter,
- Sicherheitsrichtlinien,
- Konfigurationsschemas,
- Validierung,
- Versionierung,
- atomare Änderungen,
- Diagnose und Audit.

## 2. Ziele

Das Konfigurationssystem muss:

- reproduzierbare Kernel-Builds ermöglichen,
- ungültige Kombinationen früh erkennen,
- BIOS- und UEFI-Bootpfade vereinheitlichen,
- Laufzeitänderungen kontrolliert erlauben,
- sicherheitskritische Einstellungen schützen,
- alte Hardware über explizite Kompatibilitätsprofile unterstützen,
- Konfigurationsänderungen nachvollziehbar machen,
- sichere Standardwerte verwenden,
- unbekannte zwingende Optionen ablehnen.

## 3. Konfigurationsebenen

NovaOS unterscheidet folgende Ebenen:

| Ebene | Zeitpunkt | Beispiel |
|---|---|---|
| Build | Übersetzung und Linken | Architektur, Debugsymbole |
| Image | Erzeugung der Systemgeneration | integrierte Module |
| Boot | vor oder während Kernelstart | Root-Dateisystem |
| Platform | Hardwareinitialisierung | APIC-, ACPI- oder Timeroptionen |
| Policy | Sicherheits- und Systemrichtlinien | Debugstatus, Modulregeln |
| Runtime | laufender Kernel | Logstufe, Schedulerparameter |
| Session | begrenzte temporäre Gültigkeit | Diagnosefilter |

Nicht jede Option darf auf jeder Ebene verändert werden.

## 4. Prioritätsreihenfolge

Die effektive Konfiguration wird grundsätzlich in folgender Reihenfolge gebildet:

```text
Kernel Build Defaults
  -> System-Image-Konfiguration
  -> verifizierte Plattformkonfiguration
  -> Boot-Konfiguration
  -> signierte Systemrichtlinie
  -> autorisierte Laufzeitüberschreibung
  -> zeitlich begrenzte Session-Überschreibung
```

Spätere Ebenen dürfen frühere Werte nur verändern, wenn das Schema dies ausdrücklich erlaubt.

Zwingende Sicherheitsgrenzen dürfen nicht durch niedrigere Vertrauensebenen gelockert werden.

## 5. Konfigurationsobjekt

Konfigurationen werden als typisierte Kernelobjekte dargestellt.

```c
typedef struct np_configuration_object {
    np_kernel_object_t object;

    np_configuration_scope_t scope;
    np_configuration_state_t state;

    np_config_store_t values;
    np_config_schema_t* schema;

    uint64_t generation;
    uint32_t flags;

    np_rwlock_t lock;
} np_configuration_object_t;
```

Userspace erhält keinen direkten Zugriff auf interne Konfigurationsstrukturen.

## 6. Konfigurationsbereiche

```c
typedef enum np_configuration_scope {
    NP_CONFIG_SCOPE_BUILD,
    NP_CONFIG_SCOPE_KERNEL,
    NP_CONFIG_SCOPE_PLATFORM,
    NP_CONFIG_SCOPE_SECURITY,
    NP_CONFIG_SCOPE_SUBSYSTEM,
    NP_CONFIG_SCOPE_DEVICE,
    NP_CONFIG_SCOPE_PROCESS,
    NP_CONFIG_SCOPE_SESSION
} np_configuration_scope_t;
```

Ein Wert muss einem eindeutig definierten Bereich zugeordnet sein.

## 7. Konfigurationszustände

```c
typedef enum np_configuration_state {
    NP_CONFIG_CREATED,
    NP_CONFIG_VALIDATING,
    NP_CONFIG_ACTIVE,
    NP_CONFIG_UPDATING,
    NP_CONFIG_ROLLBACK,
    NP_CONFIG_INVALID,
    NP_CONFIG_ARCHIVED
} np_configuration_state_t;
```

Nur vollständig validierte Konfigurationen dürfen `ACTIVE` werden.

## 8. Schlüsselmodell

Konfigurationsschlüssel verwenden hierarchische UTF-8-Namen.

Beispiele:

```text
kernel.scheduler.default_class
kernel.memory.guard_pages
kernel.logging.default_level
kernel.security.smap
kernel.module.allow_unsigned
device.pci.msi.enabled
network.ipv6.enabled
power.profile.default
```

Schlüssel sind case-sensitive und werden kanonisch normalisiert.

## 9. Schlüsselanforderungen

Ein Schlüssel muss:

- eine begrenzte Gesamtlänge besitzen,
- aus gültigen UTF-8-Zeichen bestehen,
- definierte Segmentregeln einhalten,
- einem registrierten Schemaeintrag entsprechen,
- innerhalb seines Bereichs eindeutig sein.

Unbekannte Schlüssel werden nicht automatisch als gültige Kerneloptionen übernommen.

## 10. Datentypen

```c
typedef enum np_config_value_type {
    NP_CONFIG_BOOLEAN,
    NP_CONFIG_SIGNED_INTEGER,
    NP_CONFIG_UNSIGNED_INTEGER,
    NP_CONFIG_STRING,
    NP_CONFIG_ENUM,
    NP_CONFIG_BITMASK,
    NP_CONFIG_DURATION,
    NP_CONFIG_SIZE,
    NP_CONFIG_UUID,
    NP_CONFIG_BINARY
} np_config_value_type_t;
```

Freie untypisierte Zeichenketten sollen nicht für Werte verwendet werden, die als Zahl, Boolean oder Enumeration dargestellt werden können.

## 11. Konfigurationswert

```c
typedef struct np_config_value {
    np_config_value_type_t type;
    uint32_t flags;

    union {
        bool boolean_value;
        int64_t signed_value;
        uint64_t unsigned_value;
        np_string_view_t string_value;
        np_uuid_t uuid_value;
        np_binary_view_t binary_value;
    };
} np_config_value_t;
```

Wert und Schematyp müssen übereinstimmen.

## 12. Konfigurationsschema

```c
typedef struct np_config_schema_entry {
    np_string_view_t key;
    np_config_value_type_t type;

    np_config_value_t default_value;
    np_config_constraints_t constraints;

    uint32_t allowed_sources;
    uint32_t mutability;
    uint32_t security_class;

    np_config_validation_callback_t validator;
    np_config_apply_callback_t apply;
    np_config_rollback_callback_t rollback;
} np_config_schema_entry_t;
```

Jede produktiv verwendete Kerneloption benötigt einen Schemaeintrag.

## 13. Schema-Versionierung

```c
typedef struct np_config_schema_version {
    uint16_t major;
    uint16_t minor;
} np_config_schema_version_t;
```

Eine inkompatible Major-Version wird abgelehnt.

Eine neuere Minor-Version darf akzeptiert werden, wenn unbekannte Felder optional und überspringbar sind.

Bestehende Schlüssel dürfen innerhalb einer Major-Version ihre Bedeutung nicht ändern.

## 14. Constraints

Ein Schema kann beschränken:

- Mindest- und Höchstwert,
- zulässige Enumeration,
- Stringlänge,
- Bitmasken,
- Ausrichtung,
- Größenmultiplikatoren,
- erforderliche oder verbotene Features,
- andere Konfigurationsschlüssel,
- Architektur oder Plattform,
- Sicherheitsprofil.

Constraints werden vor jedem Apply ausgeführt.

## 15. Abhängigkeiten

Konfigurationswerte können Abhängigkeiten besitzen.

Beispiel:

```text
kernel.security.smap = true
requires cpu.feature.smap = true
```

Weitere Beziehungen sind:

- `requires`,
- `conflicts`,
- `implies`,
- `requires_restart`,
- `requires_recovery`,
- `requires_module`.

Zyklen in zwingenden Konfigurationsabhängigkeiten werden abgelehnt.

## 16. Build-Konfiguration

Die Build-Konfiguration legt fest:

- Zielarchitektur,
- Wortbreite,
- Compiler und Toolchainprofil,
- Optimierungsstufe,
- Kernel-ABI-Major-Version,
- Debug- und Diagnosefunktionen,
- Sicherheits-Hardening,
- integrierte Kernelkomponenten,
- maximale statische Systemgrenzen.

Sie wird als maschinenlesbares Buildmanifest gespeichert.

## 17. Buildmanifest

```c
typedef struct np_kernel_build_manifest {
    np_uuid_t build_id;
    np_string_view_t source_revision;

    np_architecture_t architecture;
    np_abi_version_t kernel_abi;

    uint64_t feature_flags;
    uint64_t security_flags;

    uint8_t configuration_hash[64];
    uint8_t image_hash[64];
} np_kernel_build_manifest_t;
```

Das Buildmanifest ist Teil des verifizierten Kernel-Images.

## 18. Reproduzierbarkeit

Reproduzierbare Builds erfordern:

- deterministische Eingabereihenfolge,
- feste Toolchainversionen,
- kontrollierte Zeitstempel,
- dokumentierte Buildflags,
- festgelegte Abhängigkeiten,
- gespeicherten Konfigurationshash.

Nicht deterministische Werte werden getrennt vom reproduzierbaren Kernelinhalt gespeichert.

## 19. Build-Profile

NovaOS definiert mindestens:

```c
typedef enum np_kernel_build_profile {
    NP_BUILD_DEVELOPMENT,
    NP_BUILD_TEST,
    NP_BUILD_RELEASE,
    NP_BUILD_HARDENED,
    NP_BUILD_RECOVERY
} np_kernel_build_profile_t;
```

| Profil | Zweck |
|---|---|
| Development | Entwicklung und umfangreiche Diagnose |
| Test | automatisierte Tests und Instrumentierung |
| Release | produktiver Standardbetrieb |
| Hardened | maximale unterstützte Sicherheitsanforderungen |
| Recovery | kleine verifizierte Wiederherstellungsumgebung |

## 20. Sicherheits-Hardening

Produktive Kernel-Builds aktivieren mindestens:

- Stack Protector,
- NX Memory,
- W^X,
- Kernel ASLR, sofern unterstützt,
- schreibgeschützte Kernelabschnitte,
- Guard Pages,
- sichere Format- und Überlaufprüfungen,
- verfügbare CPU-Schutzfunktionen.

Eine globale Deaktivierung solcher Funktionen ist in Release- und Hardened-Profilen unzulässig.

## 21. Kompatibilitätsprofil

Ein Kompatibilitätsprofil kann für ältere Hardware optionale Funktionen deaktivieren.

Beispiele sind:

- APIC-Fallback auf PIC,
- PIT statt moderner Timer,
- fehlendes NX,
- fehlendes SMEP oder SMAP,
- kein IOMMU,
- eingeschränkte SMP-Unterstützung.

Der Security Manager reduziert entsprechend die ausgewiesene Sicherheitsstufe.

## 22. System-Image-Konfiguration

Eine Systemgeneration enthält eine signierte Konfiguration für:

- Kernelmodule,
- Treiberpakete,
- Root-Dateisystem,
- Startdienste,
- Standardrichtlinien,
- Recovery-Ziel,
- Updatekanal,
- Sicherheitsminimum.

Die Konfiguration gehört zur verifizierten Systemgeneration und wird nicht unabhängig unkontrolliert verändert.

## 23. Bootkonfiguration

Die Bootkonfiguration wird über das NovaOS Boot-Handoff Protocol übergeben.

Sie kann enthalten:

- ausgewählte Systemgeneration,
- Root-Dateisystem,
- Bootmodus,
- Recovery-Anforderung,
- Konsolenoptionen,
- Dumpziel,
- Debugprofil,
- Hardware-Workarounds,
- sichere Einmaloptionen.

Alle Bootdaten werden vor der Verwendung validiert.

## 24. Bootparameterformat

Bootparameter werden bevorzugt als typisierte, längenbegrenzte Struktur übergeben.

Ein textbasierter Kompatibilitätspfad kann Einträge verwenden wie:

```text
kernel.profile=release
root.generation=4f9c...
boot.mode=normal
logging.level=info
recovery.enabled=true
```

Textparameter werden in typisierte Schemawerte übersetzt.

## 25. Bootparametergrenzen

Die Bootkonfiguration besitzt feste Grenzen für:

- Gesamtgröße,
- Anzahl Einträge,
- Schlüssellänge,
- Wertlänge,
- Verschachtelung,
- Binärdaten.

Doppelte Schlüssel werden entsprechend dem Schema entweder abgelehnt oder deterministisch behandelt.

Unbekannte sicherheitskritische Parameter führen zur Ablehnung.

## 26. Sichere Bootparameter

Sicherheitskritische Bootparameter umfassen beispielsweise:

- Deaktivierung der Signaturprüfung,
- Debugmodus,
- Kernel-ASLR,
- Speicherprotektionsfunktionen,
- unsichere Module,
- Recovery-Verifikation,
- Dump-Verschlüsselung.

Sie dürfen nur aus einer ausreichend vertrauenswürdigen Quelle oder mit ausdrücklicher physischer bzw. administrativer Autorisierung übernommen werden.

## 27. Einmalige Bootoptionen

Bestimmte Parameter gelten nur für einen Bootvorgang.

Beispiele:

- Start in Recovery,
- Safe Mode,
- zusätzliche Diagnose,
- vorherige Systemgeneration,
- temporäre Treiber-Blacklist.

Sie dürfen nicht automatisch als dauerhafte Konfiguration gespeichert werden.

## 28. Plattformkonfiguration

Die Plattformkonfiguration beschreibt:

- Firmwaretyp,
- ACPI- oder Device-Tree-Modus,
- Interrupt-Controller,
- Timerquellen,
- NUMA-Topologie,
- IOMMU,
- verfügbare Energiemodi,
- Hardware-Workarounds.

Firmwareinformationen gelten als nicht vertrauenswürdig und werden gegen Hardwareerkennung und Sicherheitsrichtlinien geprüft.

## 29. Hardware-Workarounds

Workarounds werden über eindeutige IDs und Bedingungen aktiviert.

```c
typedef struct np_hardware_workaround {
    np_uuid_t workaround_id;
    np_hardware_match_t match;

    uint32_t flags;
    np_workaround_apply_t apply;
} np_hardware_workaround_t;
```

Ein Workaround muss:

- auf betroffene Hardware beschränkt sein,
- diagnostisch sichtbar sein,
- eine bekannte Auswirkung besitzen,
- möglichst deaktivierbar und testbar sein.

## 30. Laufzeitkonfiguration

Laufzeitänderungen sind nur für als veränderbar markierte Schlüssel zulässig.

Beispiele:

- Logging-Level,
- Scheduler-Tuning innerhalb sicherer Grenzen,
- Cachegrenzen,
- Netzwerk-Queue-Limits,
- Power-Profil,
- Diagnosefilter.

Architektur-, ABI- und frühe Speicherkonfigurationen sind zur Laufzeit nicht veränderbar.

## 31. Veränderbarkeitsklassen

```c
typedef enum np_config_mutability {
    NP_CONFIG_IMMUTABLE,
    NP_CONFIG_BOOT_ONLY,
    NP_CONFIG_RUNTIME,
    NP_CONFIG_RUNTIME_RESTART_REQUIRED,
    NP_CONFIG_SESSION_ONLY
} np_config_mutability_t;
```

| Klasse | Bedeutung |
|---|---|
| `IMMUTABLE` | Teil des Kernel- oder Systemimages |
| `BOOT_ONLY` | nur vor Abschluss der Bootphase änderbar |
| `RUNTIME` | atomar zur Laufzeit änderbar |
| `RUNTIME_RESTART_REQUIRED` | speicherbar, aber erst nach Neustart aktiv |
| `SESSION_ONLY` | temporär und nicht persistent |

## 32. Configuration Registry

Der Kernel stellt eine versionierte Registry aller veröffentlichten Konfigurationsschlüssel bereit.

```c
np_status_t np_config_register_schema(
    const np_config_schema_entry_t* entries,
    size_t entry_count
);
```

Module dürfen nur Schlüssel innerhalb ihres zugewiesenen Namensraums registrieren.

Nach Modulentladung dürfen keine Callbacks oder Schemazeiger auf entladenen Code verbleiben.

## 33. Lesen von Konfigurationen

```c
np_status_t np_config_get(
    np_string_view_t key,
    np_config_value_t* value
);
```

Das Lesen kann durch Sicherheitsklassen eingeschränkt sein.

Normale Anwendungen erhalten keinen vollständigen Einblick in:

- Kerneladressen,
- Schlüsselmaterial,
- Trust-Backend-Konfiguration,
- geschützte Debugoptionen,
- sicherheitskritische Hardwaredetails.

## 34. Ändern von Konfigurationen

```c
np_status_t np_config_set(
    np_string_view_t key,
    const np_config_value_t* value,
    uint32_t flags
);
```

Der Ablauf umfasst:

1. Capability prüfen,
2. Schema suchen,
3. Typ und Constraints prüfen,
4. Abhängigkeiten validieren,
5. Sicherheitsrichtlinie anwenden,
6. Änderung vorbereiten,
7. Apply-Callback ausführen,
8. Wert atomar veröffentlichen,
9. Ereignis und Audit erzeugen.

## 35. Transaktionen

Mehrere zusammengehörige Änderungen werden transaktional angewendet.

```c
np_status_t np_config_transaction_begin(
    np_config_transaction_handle_t* transaction
);

np_status_t np_config_transaction_set(
    np_config_transaction_handle_t transaction,
    np_string_view_t key,
    const np_config_value_t* value
);

np_status_t np_config_transaction_commit(
    np_config_transaction_handle_t transaction
);

void np_config_transaction_abort(
    np_config_transaction_handle_t transaction
);
```

## 36. Transaktionsvalidierung

Vor dem Commit werden alle Änderungen gemeinsam geprüft auf:

- Typen,
- Wertebereiche,
- Abhängigkeiten,
- Konflikte,
- Sicherheitsgrenzen,
- Ressourcenverfügbarkeit,
- Neustartanforderungen.

Eine Transaktion wird vollständig angewendet oder vollständig verworfen.

## 37. Rollback

Schlägt ein Apply-Callback fehl, werden bereits angewendete Änderungen in umgekehrter Reihenfolge zurückgerollt.

Ein Schema für runtime-veränderbare Werte muss entweder:

- einen sicheren Rollback unterstützen,
- oder seine Änderung erst nach vollständiger Vorbereitung atomar sichtbar machen.

Nicht rückrollbare Änderungen dürfen nicht als normale Runtime-Option definiert werden.

## 38. Generationen

Jeder erfolgreiche Konfigurations-Commit erhöht eine Generation.

```c
typedef uint64_t np_config_generation_t;
```

Leser können anhand der Generation erkennen, ob sich mehrere zusammengehörige Werte während einer Abfrage geändert haben.

Generationswerte dürfen während eines Bootvorgangs nicht rückwärts laufen.

## 39. Snapshots

```c
np_status_t np_config_snapshot_create(
    np_configuration_scope_t scope,
    np_config_snapshot_handle_t* snapshot
);
```

Ein Snapshot stellt eine konsistente schreibgeschützte Sicht auf eine Konfigurationsgeneration bereit.

Snapshots eignen sich für:

- Diagnose,
- Rollback,
- Serviceinitialisierung,
- Vergleich vor und nach Updates.

Sensible Werte bleiben entsprechend ihrer Sicherheitsklasse geschützt.

## 40. Persistenz

Persistierbare Runtime-Änderungen werden nicht direkt aus dem Kernel in beliebige Dateien geschrieben.

Ein autorisierter Userspace Configuration Service:

- liest freigegebene Änderungen,
- speichert sie transaktional,
- signiert oder schützt kritische Richtlinien,
- ordnet sie einer Systemgeneration zu,
- stellt sie beim nächsten Boot bereit.

Der Kernel bleibt bei Ausfall des Dienstes mit der aktiven Konfiguration funktionsfähig.

## 41. Sichere Standardwerte

Jeder Schemaeintrag besitzt einen sicheren Standardwert.

Fehlt ein optionaler Wert, wird dieser Standard verwendet.

Fehlt ein zwingender sicherheitskritischer Wert, darf der Kernel nicht unkontrolliert einen permissiven Zustand wählen.

Mögliche Reaktionen sind:

- restriktiver Fallback,
- Degraded Mode,
- Recovery,
- sicherer Bootabbruch.

## 42. Unbekannte Konfigurationen

Unbekannte Einträge werden behandelt anhand ihrer Kennzeichnung:

| Eintrag | Verhalten |
|---|---|
| optional | ignorieren und diagnostizieren |
| zwingend | Konfiguration ablehnen |
| veraltet | Ersatzwert anwenden, wenn eindeutig |
| sicherheitskritisch unbekannt | ablehnen |
| herstellerspezifisch | nur im registrierten Namespace akzeptieren |

Unbekannte Werte dürfen nicht an beliebige Kernelkomponenten durchgereicht werden.

## 43. Veraltete Schlüssel

Ein Schlüssel kann als deprecated markiert werden.

Das Schema beschreibt:

- Ersatzschlüssel,
- Umwandlungsregel,
- letzte unterstützte Version,
- Warnungsstufe.

Automatische Umwandlung erfolgt nur, wenn die Semantik eindeutig ist.

Ein alter Schlüssel darf nicht stillschweigend eine neue Bedeutung erhalten.

## 44. Modulkonfiguration

Kernelmodule registrieren ihre Schlüssel während der kontrollierten Initialisierung.

Beispiel:

```text
module.novafs.scrub.interval
module.novafs.cache.maximum
module.usb.polling.timeout
```

Der Module Loader prüft Namensraum, Lebenszeit und ABI.

Nach Modulentladung bleiben nur persistente, aber inaktive Konfigurationswerte im Userspace-Speicher bestehen.

## 45. Treiberkonfiguration

Treiberkonfigurationen können gelten für:

- Treibertyp,
- Treiberinstanz,
- Geräteklasse,
- konkretes Gerät,
- Hardware-ID.

Treiber dürfen nur auf ihre eigene Konfigurationsdomain zugreifen.

Sicherheitskritische Geräteoptionen werden zusätzlich durch Device und Security Manager geprüft.

## 46. Sicherheitskonfiguration

Sicherheitsparameter besitzen die höchste Schutzklasse.

Dazu gehören:

- Kernelmodul-Signaturen,
- Secure Debugging,
- W^X und NX,
- Capability-Policies,
- Audit,
- Trust Stores,
- Recovery-Verifikation,
- Dump-Verschlüsselung,
- IOMMU-Anforderungen.

Lockerungen erfordern administrative Rechte, Audit und gegebenenfalls Neustart oder physische Bestätigung.

## 47. Geheimnisse

Passwörter, private Schlüssel und Tokens werden nicht als normale Konfigurationswerte gespeichert.

Stattdessen referenziert die Konfiguration geschützte Key- oder Secret-Objekte.

```text
security.dump.key = key-object://dump-key
```

Generische Konfigurationsabfragen geben kein Schlüsselmaterial zurück.

## 48. Konfigurationsereignisse

Das Framework veröffentlicht:

```text
CONFIG_VALUE_CHANGED
CONFIG_TRANSACTION_COMMITTED
CONFIG_TRANSACTION_ROLLED_BACK
CONFIG_RESTART_REQUIRED
CONFIG_POLICY_REJECTED
CONFIG_SCHEMA_REGISTERED
CONFIG_SCHEMA_REMOVED
CONFIG_DEPRECATED_KEY_USED
CONFIG_SECURITY_LEVEL_CHANGED
```

Empfänger sehen nur Ereignisse innerhalb ihrer zulässigen Konfigurationsbereiche.

## 49. Beobachter

Kernelkomponenten können Beobachter registrieren.

```c
np_status_t np_config_observer_register(
    np_string_view_t key_prefix,
    np_config_observer_callback_t callback,
    void* context
);
```

Callbacks werden nach atomarer Veröffentlichung ausgeführt und dürfen den Commit nicht rückwirkend verändern.

Kritische Apply-Logik gehört in transaktionale Apply-Callbacks, nicht in Beobachter.

## 50. Callback-Regeln

Konfigurationscallbacks müssen:

- ihren Ausführungskontext dokumentieren,
- definierte Timeouts einhalten,
- keine rekursive unkontrollierte Konfigurationsänderung auslösen,
- bei Modulcallbacks durch Modulreferenzen geschützt sein,
- bei Fehlern einen eindeutigen Status liefern.

Lange Änderungen werden über vorbereitete asynchrone Transaktionen umgesetzt.

## 51. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_CONFIG_READ_PUBLIC` | öffentliche Werte lesen |
| `CAP_CONFIG_READ_SYSTEM` | geschützte Systemwerte lesen |
| `CAP_CONFIG_WRITE_SELF` | eigene zulässige Werte verändern |
| `CAP_CONFIG_WRITE_SUBSYSTEM` | Werte eines Subsystems verändern |
| `CAP_CONFIG_RUNTIME` | Runtime-Änderungen anwenden |
| `CAP_CONFIG_PERSIST` | Änderungen persistent speichern lassen |
| `CAP_CONFIG_SECURITY` | Sicherheitskonfiguration verwalten |
| `CAP_CONFIG_SCHEMA` | Schemas registrieren |
| `CAP_CONFIG_ADMIN` | globale Konfiguration verwalten |

Capabilities können auf Schlüsselpräfixe und Konfigurationsebenen begrenzt werden.

## 52. Audit

Mindestens folgende Änderungen werden auditiert:

- Sicherheitsparameter,
- Debugstatus,
- Modul- und Treiberregeln,
- Bootparameter mit Sicherheitswirkung,
- Recovery-Konfiguration,
- Dump- und Auditpolicy,
- globale Netzwerk- und Powerpolicy,
- Hardware-Workarounds,
- fehlgeschlagene administrative Änderungen.

Audit enthält alte und neue Werte nur, wenn diese nicht geheim oder personenbezogen sensibel sind.

## 53. Datenschutz

Konfigurationsdaten können personenbezogene oder geräteidentifizierende Informationen enthalten.

Daher gelten:

- Datenminimierung,
- getrennte Sicherheitsklassen,
- keine Geheimnisse in generischen Konfigurationswerten,
- eingeschränkter Zugriff auf Gerätekennungen,
- lokale Speicherung als Standard,
- keine automatische externe Übertragung,
- konfigurierbare Aufbewahrung der Änderungshistorie.

## 54. Diagnose

Das Framework stellt autorisierten Diagnosediensten bereit:

- aktive Konfigurationsgeneration,
- Quelle jedes effektiven Werts,
- Default- und überschriebenen Status,
- Schema-Version,
- Veränderbarkeitsklasse,
- Neustartanforderung,
- fehlgeschlagene Validierungen,
- aktive Hardware-Workarounds,
- veraltete Schlüssel,
- Rollbackereignisse.

Sensible Werte können vollständig verborgen oder redigiert werden.

## 55. Statistik

```c
typedef struct np_configuration_statistics {
    uint64_t registered_keys;
    uint64_t active_values;

    uint64_t read_operations;
    uint64_t write_operations;

    uint64_t committed_transactions;
    uint64_t rolled_back_transactions;

    uint64_t validation_failures;
    uint64_t access_denials;
    uint64_t deprecated_key_uses;

    uint64_t restart_required_changes;
} np_configuration_statistics_t;
```

Statistiken sollen pro Konfigurationsbereich und systemweit verfügbar sein.

## 56. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_CONFIG_NOT_FOUND` | Schlüssel wurde nicht gefunden |
| `NP_ERR_CONFIG_TYPE` | Wert besitzt den falschen Typ |
| `NP_ERR_CONFIG_RANGE` | Wert liegt außerhalb des erlaubten Bereichs |
| `NP_ERR_CONFIG_CONFLICT` | Wert widerspricht einer anderen Option |
| `NP_ERR_CONFIG_DEPENDENCY` | erforderliche Abhängigkeit fehlt |
| `NP_ERR_CONFIG_IMMUTABLE` | Wert ist nicht veränderbar |
| `NP_ERR_CONFIG_BOOT_ONLY` | Bootoption kann zur Laufzeit nicht geändert werden |
| `NP_ERR_CONFIG_RESTART_REQUIRED` | Änderung wird erst nach Neustart aktiv |
| `NP_ERR_CONFIG_SCHEMA` | Schema ist ungültig oder inkompatibel |
| `NP_ERR_CONFIG_TRANSACTION` | Transaktion konnte nicht abgeschlossen werden |
| `NP_ERR_CONFIG_ROLLBACK` | Rollback ist fehlgeschlagen |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_NOT_SUPPORTED` | Option wird nicht unterstützt |

## 57. Fehlerbehandlung

Ungültige Runtime-Änderungen werden abgelehnt, ohne die aktive Konfiguration zu verändern.

Fehlerhafte Boot- oder Systemkonfigurationen können führen zu:

- sicheren Standardwerten,
- Degraded Mode,
- Verwendung der letzten gültigen Konfiguration,
- Start einer vorherigen Systemgeneration,
- Recovery,
- kontrolliertem Bootabbruch.

Eine teilweise aktivierte Konfiguration ohne klaren Status ist unzulässig.

## 58. Kritische Fehler

Als kritisch gelten:

- Manipulation einer signierten Sicherheitskonfiguration,
- beschädigte Schema- oder Registry-Struktur,
- nicht rückrollbare teilweise Konfigurationsänderung,
- unautorisierte Lockerung zentraler Sicherheitsfunktionen,
- Verwendung inkompatibler ABI- oder Plattformwerte,
- Offenlegung von Secrets über Konfigurationsabfragen.

Je nach Auswirkung wird die Änderung verworfen, Recovery aktiviert oder die Kernel-Panic-Policy angewendet.

## 59. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Laden gültiger Build Defaults,
2. Buildmanifest und Konfigurationshash,
3. reproduzierbare Build-Konfiguration,
4. Build-Profile,
5. System-Image-Konfiguration,
6. typisierte Bootparameter,
7. textbasierte Bootparameterübersetzung,
8. unbekannter optionaler Parameter,
9. unbekannter zwingender Parameter,
10. sichere Bootparameterquelle,
11. einmalige Bootoption,
12. Plattformkonfiguration,
13. Hardware-Workaround,
14. Schema-Registrierung,
15. doppelter Schlüssel,
16. gültiger Boolean-Wert,
17. Integerbereich,
18. Enumeration,
19. Stringlänge,
20. `requires`-Abhängigkeit,
21. `conflicts`-Beziehung,
22. Abhängigkeitszyklus,
23. Lesen eines öffentlichen Werts,
24. Zugriffsschutz auf Systemwerte,
25. gültige Runtime-Änderung,
26. Ablehnung unveränderbarer Werte,
27. Neustartanforderung,
28. erfolgreiche Transaktion,
29. Transaktions-Rollback,
30. atomare Sichtbarkeit mehrerer Änderungen,
31. Konfigurationsgeneration,
32. Snapshot,
33. persistente Übergabe an Userspace-Dienst,
34. sichere Standardwerte,
35. veralteter Schlüssel,
36. Modulkonfiguration und Modulentladung,
37. Treiberinstanz-Konfiguration,
38. Sicherheitskonfiguration,
39. Secret-Objektreferenz,
40. Beobachterbenachrichtigung,
41. Callback-Timeout,
42. Capability- und Auditprüfung,
43. Datenschutzfilter,
44. korrekte Diagnose und Statistik.

## 60. Verbindliche Invarianten

1. Jeder aktive Kernelkonfigurationswert entspricht einem registrierten Schema.
2. Nur vollständig validierte Konfigurationen werden aktiviert.
3. Unbekannte zwingende oder sicherheitskritische Optionen werden abgelehnt.
4. Spätere Konfigurationsebenen dürfen zwingende Sicherheitsgrenzen nicht lockern.
5. Runtime-Transaktionen werden vollständig angewendet oder vollständig verworfen.
6. Nicht rückrollbare Optionen werden nicht als normale Runtime-Werte angeboten.
7. Geheimnisse werden nicht als generische Klartext-Konfigurationswerte gespeichert.
8. Bestehende Schlüssel ändern innerhalb einer Schema-Major-Version nicht ihre Bedeutung.
9. Jede erfolgreiche Änderung erhöht die Konfigurationsgeneration.
10. Boot-Only- und Immutable-Werte werden zur Laufzeit nicht verändert.
11. Konfigurationsrechte sind auf Schlüsselbereiche und Ebenen begrenzbar.
12. Konfigurationsänderungen erfolgen unter geeigneter Synchronisation.

## 61. Referenzablauf einer Runtime-Transaktion

```text
Autorisierter Dienst beginnt Transaktion
  -> gewünschte Schlüssel und Werte hinzufügen
  -> Schemas und Datentypen prüfen
  -> Constraints und Abhängigkeiten gemeinsam validieren
  -> Capability- und Sicherheitsprüfung durchführen
  -> Ressourcen und Apply-Schritte vorbereiten
  -> Konfiguration auf UPDATING setzen
  -> Apply-Callbacks in definierter Reihenfolge ausführen
  -> bei Fehler ausgeführte Schritte zurückrollen
  -> bei Erfolg Werte atomar veröffentlichen
  -> Konfigurationsgeneration erhöhen
  -> Ereignisse und Audit erzeugen
  -> Transaktion abschließen
```

## 62. Zusammenfassung

Das NovaOS Kernel Configuration Framework trennt Build-, Image-, Boot-, Plattform-, Policy- und Runtime-Konfigurationen klar voneinander.

Typisierte und versionierte Schemas definieren für jeden Schlüssel Datentyp, Wertebereich, Quelle, Veränderbarkeit und Sicherheitsklasse. Transaktionen gewährleisten, dass zusammengehörige Änderungen vollständig aktiviert oder vollständig zurückgerollt werden.

Sichere Standardwerte, signierte Systemkonfigurationen, geschützte Sicherheitsoptionen und nachvollziehbare Generationen ermöglichen reproduzierbare Builds und kontrollierte Anpassungen, ohne zentrale Kernel- oder Sicherheitsinvarianten durch beliebige Laufzeitparameter aufzuweichen.