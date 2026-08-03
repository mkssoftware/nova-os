# NPSPEC-KERNEL-0021 – Power Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0021 |
| Titel | Power Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Energieverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0003, NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0014, NPSPEC-KERNEL-0017, NPSPEC-KERNEL-0018, NPSPEC-KERNEL-0020 |
| Zugehörige ADRs | ADR-POWER-0001, ADR-POWER-0002, ADR-POWER-0003, ADR-POWER-0004, ADR-POWER-0005, ADR-DEV-0005 |

---

## 1. Zweck

Diese Spezifikation definiert den Power Manager des NovaOS-Kernels.

Der Power Manager koordiniert:

- CPU-Idle-Zustände,
- CPU-Frequenz und Leistungszustände,
- Geräteschlafzustände,
- System Suspend,
- Hibernate,
- Resume,
- Wake-Quellen,
- Thermal Management,
- Batterie- und Netzteilstatus,
- Energieprofile,
- kontrolliertes Herunterfahren und Neustarten.

## 2. Ziele

Der Power Manager muss:

- Energieverbrauch ohne unnötige Leistungseinbußen reduzieren,
- alte und moderne Hardware unterstützen,
- Geräteabhängigkeiten berücksichtigen,
- Suspend und Resume transaktional koordinieren,
- Datenverlust beim Hibernate verhindern,
- Thermalgrenzen sicher einhalten,
- Benutzer- und Systemrichtlinien trennen,
- Realtime- und Latenzanforderungen berücksichtigen,
- Fehler transparent und lokal diagnostizierbar machen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Power Manager | globale Energieentscheidungen und Übergänge |
| Scheduler | CPU-Last, Konsolidierung und Latenzanforderungen |
| Timer Subsystem | Wake-Timer und Zeitkorrektur |
| Device Manager | Gerätezustände und Abhängigkeiten |
| Driver Framework | treiberspezifische Power-Callbacks |
| Thermal Manager | Sensoren, Grenzwerte und Kühlmaßnahmen |
| Firmware Layer | ACPI- oder plattformspezifische Übergänge |
| Security Manager | Rechte und Sicherheitsrichtlinien |
| VFS | Flush und Dateisystem-Synchronisierung |

## 4. Grundprinzipien

Für Energieübergänge gelten:

1. Datenintegrität hat Vorrang vor Energieeinsparung.
2. Kein Gerät wird in einen unzulässigen Zustand versetzt.
3. Abhängigkeiten werden vor jedem Übergang berücksichtigt.
4. Wake-Quellen werden vor Suspend vollständig eingerichtet.
5. Fehler führen zu Rollback oder einem klar definierten sicheren Zustand.
6. Nicht unterstützte Hardwarefunktionen werden nicht vorgetäuscht.
7. Unprivilegierte Prozesse dürfen globale Energiezustände nicht beliebig verändern.

## 5. Energieobjekte

Der Power Manager verwaltet mindestens:

```c
typedef enum np_power_object_type {
    NP_POWER_SYSTEM,
    NP_POWER_CPU,
    NP_POWER_DEVICE,
    NP_POWER_DOMAIN,
    NP_POWER_SUPPLY,
    NP_POWER_BATTERY,
    NP_POWER_THERMAL_ZONE,
    NP_POWER_COOLING_DEVICE
} np_power_object_type_t;
```

Alle dauerhaft referenzierbaren Energieobjekte werden in den Kernel Object Graph integriert.

## 6. System-Energiezustände

```c
typedef enum np_system_power_state {
    NP_SYSTEM_POWER_RUNNING,
    NP_SYSTEM_POWER_IDLE,
    NP_SYSTEM_POWER_SUSPEND_TO_IDLE,
    NP_SYSTEM_POWER_SUSPEND_TO_RAM,
    NP_SYSTEM_POWER_HIBERNATE,
    NP_SYSTEM_POWER_HYBRID_SLEEP,
    NP_SYSTEM_POWER_SHUTDOWN,
    NP_SYSTEM_POWER_RESTART,
    NP_SYSTEM_POWER_OFF
} np_system_power_state_t;
```

Nicht jede Plattform muss alle Zustände unterstützen.

Die verfügbaren Zustände müssen zur Laufzeit abfragbar sein.

## 7. Laufender Zustand

Im Zustand `RUNNING` bleiben alle für den Betrieb benötigten Kernel- und Gerätekomponenten verfügbar.

Der Power Manager darf dennoch:

- CPUs in Idle-Zustände versetzen,
- Frequenzen anpassen,
- ungenutzte Geräte schlafen legen,
- Hintergrundarbeit bündeln,
- Display und Funkgeräte nach Richtlinie drosseln.

Diese Laufzeitoptimierungen gelten nicht als vollständiger System-Suspend.

## 8. Suspend to Idle

Suspend to Idle verwendet weiterhin einen laufenden Kernel, reduziert aber aktive Arbeit auf ein Minimum.

Dabei werden:

- normale Userspace-Aktivitäten angehalten,
- Geräte in geeignete Schlafzustände versetzt,
- CPUs in tiefe Idle-Zustände geschickt,
- ausschließlich Wake-Ereignisse verarbeitet.

Dieser Modus benötigt keinen vollständigen Firmware-Suspend.

## 9. Suspend to RAM

Bei Suspend to RAM bleibt der Arbeitsspeicher erhalten, während große Teile des Systems abgeschaltet werden.

Der Übergang umfasst:

- Einfrieren von Prozessen,
- Synchronisieren von Dateisystemen,
- Suspendieren von Geräten,
- Stoppen sekundärer CPUs,
- Konfigurieren von Wake-Quellen,
- Aufrufen der Plattform-Suspend-Funktion.

Nach Resume wird die Reihenfolge kontrolliert umgekehrt.

## 10. Hibernate

Hibernate speichert einen wiederherstellbaren Systemzustand auf einem persistenten Medium.

Der Zustand muss mindestens umfassen:

- benötigte physische Speicherseiten,
- CPU- und Kernelzustand,
- Prozess- und Threadzustände,
- virtuelle Speicherverwaltung,
- Geräte-Reinitialisierungsinformationen,
- Integritäts- und Versionsmetadaten.

Nicht jeder Hardwarezustand wird direkt wiederhergestellt. Geräte können nach Resume vollständig neu initialisiert werden.

## 11. Hybrid Sleep

Hybrid Sleep kombiniert Suspend to RAM mit einem vorher erzeugten Hibernate-Image.

Bei normalem Wake wird aus RAM fortgesetzt.

Bei Stromverlust kann das System aus dem Hibernate-Image wiederhergestellt werden.

Der Zustand gilt erst als bereit, wenn das Hibernate-Image vollständig und dauerhaft geschrieben wurde.

## 12. Geräte-Energiezustände

```c
typedef enum np_device_power_state {
    NP_DEVICE_POWER_D0,
    NP_DEVICE_POWER_D1,
    NP_DEVICE_POWER_D2,
    NP_DEVICE_POWER_D3_HOT,
    NP_DEVICE_POWER_D3_COLD,
    NP_DEVICE_POWER_OFF
} np_device_power_state_t;
```

| Zustand | Bedeutung |
|---|---|
| `D0` | vollständig aktiv |
| `D1` | leicht reduzierter Zustand |
| `D2` | tieferer Geräteschlaf |
| `D3_HOT` | weitgehend aus, Konfiguration teilweise erreichbar |
| `D3_COLD` | Stromversorgung weitgehend entfernt |
| `OFF` | Gerät vollständig deaktiviert |

Die tatsächliche Semantik ist geräte- und busspezifisch.

## 13. Power Domains

Eine Power Domain umfasst mehrere Geräte oder Komponenten mit gemeinsamer Energieversorgung.

```c
typedef struct np_power_domain {
    np_kernel_object_t object;

    np_power_domain_id_t domain_id;
    np_power_state_t state;

    np_list_t members;
    np_list_t dependencies;

    np_atomic_uint32_t active_references;
    np_mutex_t transition_lock;
} np_power_domain_t;
```

Eine Domain darf erst abgeschaltet werden, wenn keine aktive Referenz oder Wake-Abhängigkeit besteht.

## 14. Runtime Power Management

Ein Gerät kann während des normalen Betriebs automatisch schlafen gelegt werden, wenn:

- keine offenen aktiven Anfragen bestehen,
- keine DMA-Operation läuft,
- keine unbestätigten Interrupts bestehen,
- keine abhängigen Geräte aktiv sind,
- die erwartete Leerlaufzeit den Übergang rechtfertigt.

Bei neuer Nutzung wird das Gerät vor der Operation wieder aktiviert.

## 15. Aktivitätsreferenzen

Treiber und Kernelkomponenten verwenden Aktivitätsreferenzen:

```c
np_status_t np_power_runtime_get(
    np_device_handle_t device
);

void np_power_runtime_put(
    np_device_handle_t device
);
```

Die erste Referenz kann das Gerät aufwecken.

Nach Freigabe der letzten Referenz kann eine Idle-Verzögerung gestartet werden.

Referenzzähler müssen gegen Über- und Unterlauf geschützt sein.

## 16. Autosuspend

```c
np_status_t np_power_set_autosuspend(
    np_device_handle_t device,
    np_time_ns_t delay
);
```

Autosuspend wird nur aktiviert, wenn Gerät und Treiber ihn unterstützen.

Eine Verzögerung verhindert ineffizientes Schlafen und sofortiges Wiederaufwecken bei kurzzeitiger Inaktivität.

## 17. CPU-Idle-Zustände

```c
typedef struct np_cpu_idle_state {
    uint32_t state_id;
    const char* name;

    np_time_ns_t exit_latency;
    np_time_ns_t target_residency;

    uint64_t estimated_power;
    uint32_t flags;
} np_cpu_idle_state_t;
```

Je tiefer der Zustand, desto geringer kann der Energieverbrauch und desto höher kann die Aufwachlatenz sein.

## 18. Auswahl des CPU-Idle-Zustands

Der Idle Governor berücksichtigt:

- nächste Timerdeadline,
- Schedulerlast,
- Wake-Latenzanforderungen,
- Realtime- und Deadline-Threads,
- CPU-Topologie,
- erwartete Leerlaufdauer,
- Thermalzustand,
- bekannte Hardwareeinschränkungen.

Ein Zustand darf nur gewählt werden, wenn seine Exit-Latenz mit den aktiven Anforderungen vereinbar ist.

## 19. Idle-Thread-Integration

Der CPU-lokale Idle-Thread ruft den Power Manager auf, wenn keine ausführbaren Threads vorhanden sind.

```c
void np_power_cpu_idle(
    np_cpu_id_t cpu_id
);
```

Der Power Manager:

1. prüft ausstehende Arbeit,
2. ermittelt die nächste Deadline,
3. wählt einen Idle-Zustand,
4. programmiert das Wake-Ereignis,
5. betritt den Zustand,
6. erfasst die tatsächliche Aufenthaltszeit.

## 20. CPU-Leistungszustände

CPU-Leistungszustände steuern Frequenz und gegebenenfalls Spannung.

```c
typedef struct np_cpu_performance_state {
    uint32_t state_id;
    uint64_t frequency_hz;
    uint64_t capacity;
    uint64_t estimated_power;
} np_cpu_performance_state_t;
```

Die konkrete Steuerung kann durch Hardware, Firmware oder Kernel erfolgen.

## 21. Performance Governor

```c
typedef enum np_performance_governor {
    NP_GOVERNOR_PERFORMANCE,
    NP_GOVERNOR_BALANCED,
    NP_GOVERNOR_EFFICIENCY,
    NP_GOVERNOR_POWERSAVE,
    NP_GOVERNOR_CUSTOM
} np_performance_governor_t;
```

Der Governor verwendet:

- CPU-Auslastung,
- Runqueue-Länge,
- Interaktivitätsanforderungen,
- Energieprofil,
- Batterielevel,
- Thermalgrenzen,
- Hardwarekapazität.

## 22. Energieprofile

NovaOS stellt mindestens bereit:

| Profil | Verhalten |
|---|---|
| Performance | Leistung und geringe Latenz bevorzugen |
| Balanced | Leistung und Energieverbrauch ausgleichen |
| Efficiency | Energieeffizienz bevorzugen |
| Battery Saver | Laufzeit maximieren und Hintergrundarbeit begrenzen |
| Realtime | zugesagte Latenzgrenzen schützen |
| Thermal Safe | Wärmeentwicklung stark begrenzen |

Profile definieren Richtlinien, keine garantierten festen Frequenzen.

## 23. Scheduler-Integration

Der Scheduler liefert dem Power Manager:

- CPU-Auslastung,
- Runqueue-Längen,
- Thread-Latenzanforderungen,
- Realtime- und Deadline-Budgets,
- CPU-Affinitäten,
- erwartete Idle-Zeiten.

Der Power Manager kann dem Scheduler melden:

- reduzierte CPU-Kapazität,
- bevorzugte CPUs,
- offline geschaltete CPUs,
- Thermalbeschränkungen,
- Energieprofil.

## 24. Heterogene CPUs

Bei heterogenen Prozessoren berücksichtigt der Power Manager:

- Performance- und Efficiency-Kerne,
- unterschiedliche Frequenzbereiche,
- gemeinsame Power Domains,
- thermische Kopplung,
- Energieeffizienz pro Lastklasse.

Interaktive und rechenintensive Arbeit kann auf leistungsstarke Kerne konzentriert werden, während Hintergrundarbeit bevorzugt effiziente Kerne nutzt.

## 25. CPU Hotplug

CPUs können zur Energieeinsparung logisch offline geschaltet werden.

Vor dem Offlining müssen:

- Threads migriert,
- Interrupts umgeroutet,
- Timer verschoben,
- RCU- und Per-CPU-Arbeit abgeschlossen,
- lokale Caches geleert,
- Power- und Schedulerzustände aktualisiert werden.

Mindestens eine funktionsfähige CPU bleibt online.

## 26. Wake-Quellen

```c
typedef enum np_wake_source_type {
    NP_WAKE_TIMER,
    NP_WAKE_POWER_BUTTON,
    NP_WAKE_KEYBOARD,
    NP_WAKE_MOUSE,
    NP_WAKE_NETWORK,
    NP_WAKE_USB,
    NP_WAKE_RTC,
    NP_WAKE_LID,
    NP_WAKE_GPIO,
    NP_WAKE_PLATFORM
} np_wake_source_type_t;
```

Nur ausdrücklich aktivierte und unterstützte Quellen dürfen das System wecken.

## 27. Wake-Source-Objekt

```c
typedef struct np_wake_source {
    np_kernel_object_t object;

    np_wake_source_type_t type;
    np_device_t* device;

    np_atomic_uint32_t active_count;
    uint32_t flags;
} np_wake_source_t;
```

Eine aktive Wake Source kann verhindern, dass ein unvereinbarer tiefer Schlafzustand betreten wird.

## 28. Wake Locks

Kernelkomponenten können einen System-Suspend vorübergehend verhindern.

```c
np_status_t np_wake_lock_acquire(
    np_wake_lock_handle_t wake_lock,
    np_time_ns_t maximum_duration
);

void np_wake_lock_release(
    np_wake_lock_handle_t wake_lock
);
```

Wake Locks müssen:

- einen dokumentierten Grund besitzen,
- zeitlich begrenzt werden können,
- diagnostisch sichtbar sein,
- beim Prozess- oder Treiberende freigegeben werden.

## 29. Suspend-Phasen

Ein System-Suspend erfolgt in folgenden Phasen:

```text
REQUEST
  -> PREPARE
  -> FREEZE_USERSPACE
  -> SYNC_STORAGE
  -> SUSPEND_DEVICES
  -> SUSPEND_CPUS
  -> ENTER_PLATFORM_STATE
  -> WAKE
  -> RESUME_CPUS
  -> RESUME_DEVICES
  -> THAW_USERSPACE
  -> COMPLETE
```

Jede Phase besitzt einen definierten Status und Rollback-Punkt.

## 30. Suspend-Anforderung

```c
np_status_t np_power_request_state(
    np_system_power_state_t target_state,
    uint32_t reason,
    uint32_t flags
);
```

Vor der Annahme werden geprüft:

- Capability des Aufrufers,
- Plattformunterstützung,
- aktive Sperrgründe,
- laufende Updates,
- kritische I/O-Operationen,
- Sicherheitsrichtlinie,
- verfügbare Wake-Quellen.

## 31. Userspace Freeze

Vor einem tiefen Schlafzustand werden reguläre Userspace-Prozesse kontrolliert eingefroren.

Ausgenommen bleiben nur notwendige Systemdienste für:

- Dateisystem-Synchronisierung,
- Hibernate-Image-Erzeugung,
- Energiekoordination,
- Sicherheit und Recovery.

Ein Prozess, der nicht rechtzeitig einfriert, kann abhängig von seiner Kritikalität beendet oder der Suspend kann abgebrochen werden.

## 32. Dateisystem-Synchronisierung

Vor Suspend oder Hibernate fordert der Power Manager das VFS auf:

- Dirty Pages zu schreiben,
- Journale zu committen,
- kritische Metadaten zu synchronisieren,
- Geräte-Flushes auszuführen.

Suspend to RAM kann abhängig von der Richtlinie geringere Dauerhaftigkeitsanforderungen besitzen als Hibernate oder Shutdown.

## 33. Gerätesuspend-Reihenfolge

Geräte werden grundsätzlich von Kindern zu Eltern suspendiert.

Beispiel:

```text
USB-Gerät
  -> USB-Hub
  -> USB-Controller
  -> PCI-Bridge
  -> Plattform-Power-Domain
```

Beim Resume erfolgt die Reihenfolge von Eltern zu Kindern.

Zusätzliche Graphabhängigkeiten müssen berücksichtigt werden.

## 34. Suspend-Rollback

Schlägt eine Suspend-Phase fehl, werden bereits suspendierte Komponenten in umgekehrter Reihenfolge reaktiviert.

Der Rollback muss:

- Gerätelisten und Zustände konsistent halten,
- Interrupts und DMA kontrolliert wieder aktivieren,
- eingefrorene Prozesse auftauen,
- Fehlerdiagnose erzeugen,
- das System in `RUNNING` oder einen klar definierten Degraded Mode zurückführen.

## 35. Resume

Nach einem Wake-Ereignis muss NovaOS:

1. minimalen CPU- und Speicherzustand wiederherstellen,
2. Zeitbasis korrigieren,
3. Interrupt-Controller aktivieren,
4. sekundäre CPUs starten,
5. Power Domains aktivieren,
6. Geräte in Abhängigkeitsreihenfolge fortsetzen,
7. Dateisystem- und Netzwerkzustände prüfen,
8. Userspace-Prozesse auftauen,
9. Wake-Grund veröffentlichen.

## 36. Hibernate-Image

```c
typedef struct np_hibernate_image_header {
    uint64_t magic;
    uint32_t format_version;
    uint32_t flags;

    np_uuid_t boot_generation;
    np_uuid_t kernel_build_id;

    uint64_t page_count;
    uint64_t image_size;

    uint8_t integrity_hash[64];
} np_hibernate_image_header_t;
```

Das Format muss versioniert und gegen Manipulation geschützt sein.

## 37. Hibernate-Kompatibilität

Ein Hibernate-Image darf nur wiederhergestellt werden, wenn mindestens übereinstimmen:

- Kernel-ABI und Build-Kompatibilität,
- Systemgeneration,
- Speicherlayoutanforderungen,
- Sicherheitsprofil,
- Hardware- und Firmwarezustand innerhalb definierter Grenzen,
- Image-Integrität.

Bei Inkompatibilität wird ein normaler Bootvorgang gestartet.

## 38. Hibernate-Sicherheit

Das Hibernate-Image kann sensible Speicherinhalte enthalten.

Daher muss es:

- verschlüsselt werden, wenn das Sicherheitsprofil dies verlangt,
- authentifiziert und integritätsgeprüft sein,
- Schlüssel über ein geeignetes Trust Backend erhalten,
- gegen Replay entsprechend der Systemgeneration geschützt sein,
- nach erfolgreicher Wiederherstellung oder Ungültigkeit sicher behandelt werden.

Ein TPM kann verwendet werden, ist jedoch nicht zwingend.

## 39. Hibernate-Schreibvorgang

Der Schreibvorgang erfolgt transaktional:

1. Zielbereich reservieren,
2. Dateisystemzustand stabilisieren,
3. Speicherabbild aufnehmen,
4. Daten komprimieren oder verschlüsseln,
5. Image schreiben,
6. Integritätsmetadaten schreiben,
7. Image atomar als vollständig markieren,
8. Plattform abschalten.

Ein unvollständiges Image darf nicht als wiederherstellbar gelten.

## 40. Thermal Zones

```c
typedef struct np_thermal_zone {
    np_kernel_object_t object;

    np_thermal_zone_id_t zone_id;
    np_temperature_t current_temperature;

    np_thermal_trip_table_t trip_points;
    np_list_t sensors;
    np_list_t cooling_devices;

    uint32_t state;
} np_thermal_zone_t;
```

Eine Thermal Zone kann CPU, GPU, Batterie, Mainboard oder andere Komponenten repräsentieren.

## 41. Temperaturdarstellung

```c
typedef int32_t np_temperature_mc_t;
```

Temperaturen werden in Milligrad Celsius dargestellt.

Beispiel:

```text
42500 = 42,5 °C
```

Ungültige oder nicht verfügbare Sensordaten müssen eindeutig gekennzeichnet werden.

## 42. Thermal Trip Points

```c
typedef enum np_thermal_trip_type {
    NP_THERMAL_PASSIVE,
    NP_THERMAL_ACTIVE,
    NP_THERMAL_HOT,
    NP_THERMAL_CRITICAL
} np_thermal_trip_type_t;
```

| Trip Point | Reaktion |
|---|---|
| Passive | Leistung reduzieren |
| Active | Lüfter oder Kühlgerät aktivieren |
| Hot | starke Drosselung und Warnung |
| Critical | sofortige sichere Abschaltung |

Kritische Hardwaregrenzen dürfen nicht durch Benutzerprofile überschrieben werden.

## 43. Kühlgeräte

Mögliche Cooling Devices sind:

- Lüfter,
- CPU-Frequenzbegrenzung,
- CPU-Core-Offlining,
- GPU-Leistungsbegrenzung,
- Displayhelligkeit,
- Ladeleistungsbegrenzung,
- Geräte-Drosselung.

Der Power Manager wählt Maßnahmen entsprechend Wirkung, Latenz und Systemrichtlinie.

## 44. Batterieobjekt

```c
typedef struct np_battery_information {
    uint64_t design_capacity_mwh;
    uint64_t full_charge_capacity_mwh;
    uint64_t remaining_capacity_mwh;

    int64_t current_rate_mw;
    uint32_t charge_percent;
    uint32_t cycle_count;

    np_battery_state_t state;
    np_time_ns_t estimated_runtime;
} np_battery_information_t;
```

Schätzwerte müssen als solche gekennzeichnet werden.

## 45. Netzteile und Ladezustand

Der Power Manager verwaltet:

- Netzstrom vorhanden,
- Batteriebetrieb,
- Laden,
- Vollgeladen,
- Entladen,
- kritischer Batteriestand,
- unbekannter oder fehlerhafter Zustand.

Ein kritischer Batteriestand kann Hibernate oder kontrolliertes Herunterfahren auslösen.

## 46. Energieereignisse

Das Subsystem veröffentlicht unter anderem:

```text
POWER_PROFILE_CHANGED
POWER_SUPPLY_CHANGED
BATTERY_LOW
BATTERY_CRITICAL
SUSPEND_REQUESTED
SUSPEND_CANCELLED
SYSTEM_SUSPENDED
SYSTEM_RESUMED
WAKE_SOURCE_TRIGGERED
THERMAL_LIMIT
THERMAL_CRITICAL
SHUTDOWN_REQUESTED
```

Ereignisse werden über capability-geschützte Endpunkte zugestellt.

## 47. Shutdown

Ein kontrolliertes Herunterfahren erfolgt in Phasen:

1. neue Sitzungen und Aufgaben begrenzen,
2. Anwendungen benachrichtigen,
3. Prozesse gruppenweise beenden,
4. Netzwerkdienste stoppen,
5. Dateisysteme synchronisieren und aushängen,
6. Geräte stoppen,
7. Crash- und Auditdaten sichern,
8. Firmware-Power-Off aufrufen.

Timeouts verhindern unbegrenztes Blockieren.

## 48. Neustart

Beim Neustart werden dieselben Datenintegritätsanforderungen wie beim Shutdown angewendet.

Abhängig vom Neustartgrund können zusätzlich ausgeführt werden:

- Start in Recovery,
- Start einer neuen Systemgeneration,
- Firmware-Setup,
- Diagnosemodus,
- Treiber-Safe-Mode.

Der Neustartgrund wird in einem geschützten Bootstatusobjekt hinterlegt.

## 49. Notabschaltung

Eine Notabschaltung ist nur vorgesehen bei:

- kritischer Temperatur,
- Batterie- oder Stromversorgungsgefahr,
- schwerem Hardwarefehler,
- nicht fortsetzbarer Sicherheitsverletzung,
- ausdrücklich definierter Panic Policy.

Sie darf normale Flush- und Shutdown-Schritte überspringen, wenn deren Ausführung das System weiter gefährden würde.

## 50. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_POWER_QUERY` | Energie- und Batteriestatus lesen |
| `CAP_POWER_PROFILE` | eigenes oder freigegebenes Profil wählen |
| `CAP_POWER_DEVICE` | zugewiesenen Gerätezustand verwalten |
| `CAP_POWER_WAKE` | Wake-Quelle oder Wake-Timer konfigurieren |
| `CAP_POWER_SUSPEND` | System-Suspend anfordern |
| `CAP_POWER_HIBERNATE` | Hibernate anfordern |
| `CAP_POWER_SHUTDOWN` | Herunterfahren anfordern |
| `CAP_POWER_RESTART` | Neustart anfordern |
| `CAP_POWER_THERMAL` | Thermalrichtlinien verwalten |
| `CAP_POWER_ADMIN` | globale Energierichtlinien verwalten |

Rechte können zusätzlich durch Sitzung, Benutzerrolle und Systemrichtlinie begrenzt sein.

## 51. Power-API

```c
np_status_t np_power_query_system(
    np_system_power_information_t* information
);

np_status_t np_power_query_device(
    np_device_handle_t device,
    np_device_power_information_t* information
);

np_status_t np_power_set_profile(
    np_performance_governor_t profile
);

np_status_t np_power_request_state(
    np_system_power_state_t target_state,
    uint32_t reason,
    uint32_t flags
);
```

Alle öffentlichen Strukturen enthalten Größe und ABI-Version.

## 52. Sicherheit

Der Power Manager muss sicherstellen, dass:

- globale Energieübergänge eine passende Capability benötigen,
- Hibernate-Images integritätsgeschützt sind,
- sensible Hibernate-Daten nach Richtlinie verschlüsselt werden,
- Wake-Quellen nur autorisiert aktiviert werden,
- Treiber keine fremden Power Domains steuern,
- kritische Thermalgrenzen nicht deaktiviert werden,
- ein Suspend keine ungeschützten DMA-Zugriffe zurücklässt,
- Resume-Zustände vor ihrer Verwendung validiert werden,
- alte Hardwarefunktionen nur bei tatsächlicher Unterstützung angeboten werden.

## 53. Datenschutz

Energiedaten können Rückschlüsse auf Anwesenheit, Nutzung und Geräteverhalten ermöglichen.

Daher gelten:

- Anwendungen sehen standardmäßig nur notwendige aggregierte Informationen,
- detaillierte Geräte- und Batteriedaten können capability-geschützt sein,
- Wake-Gründe werden nicht unnötig extern übertragen,
- Diagnose und Telemetrie bleiben standardmäßig lokal,
- Aufbewahrung von Energieereignissen muss begrenzbar sein.

## 54. Diagnose

Der Power Manager stellt autorisierten Diagnosediensten bereit:

- aktuelles Energieprofil,
- unterstützte Systemzustände,
- CPU-Idle- und Performance-Zustände,
- Geräte-Power-States,
- aktive Wake Locks,
- Wake-Quellen und Wake-Gründe,
- Suspend- und Resume-Latenzen,
- abgelehnte Übergänge,
- Batterie- und Netzteilstatus,
- Thermalzonen und Drosselungsereignisse.

## 55. Statistik

```c
typedef struct np_power_statistics {
    uint64_t suspend_attempts;
    uint64_t successful_suspends;
    uint64_t failed_suspendsusp;
    uint64_t successful_resumes;

    uint64_t hibernate_attempts;
    uint64_t hibernate_failures;

    uint64_t device_runtime_suspends;
    uint64_t device_runtime_resumes;

    uint64_t thermal_throttles;
    uint64_t thermal_shutdowns;

    uint64_t total_idle_time_ns;
} np_power_statistics_t;
```

Statistiken sollen pro CPU, Gerät, Power Domain und System verfügbar sein.

## 56. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_POWER_STATE` | Zustand ist ungültig |
| `NP_ERR_NOT_SUPPORTED` | Zustand wird nicht unterstützt |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_DEVICE_BUSY` | Gerät kann nicht schlafen |
| `NP_ERR_WAKE_SOURCE_ACTIVE` | Wake-Bedingung verhindert den Übergang |
| `NP_ERR_DEPENDENCY_ACTIVE` | abhängige Komponente ist noch aktiv |
| `NP_ERR_SUSPEND_ABORTED` | Suspend wurde zurückgerollt |
| `NP_ERR_HIBERNATE_INVALID` | Hibernate-Image ist ungültig |
| `NP_ERR_HIBERNATE_INCOMPATIBLE` | Image passt nicht zum Systemzustand |
| `NP_ERR_THERMAL_LIMIT` | Operation durch Thermalrichtlinie begrenzt |
| `NP_ERR_TIMEOUT` | Power-Callback lief in einen Timeout |
| `NP_ERR_POWER_TRANSITION` | allgemeiner Übergangsfehler |

## 57. Fehlerbehandlung

Ein fehlgeschlagener Runtime-Suspend bleibt auf das betroffene Gerät begrenzt.

Ein fehlgeschlagener System-Suspend wird möglichst vollständig zurückgerollt.

Mögliche Reaktionen sind:

- vorherigen Power State wiederherstellen,
- Gerät oder Treiber neu starten,
- tiefen Schlafzustand deaktivieren,
- auf Suspend to Idle zurückfallen,
- Hibernate-Image verwerfen,
- Degraded Mode aktivieren,
- kontrolliert herunterfahren.

## 58. Kritische Fehler

Als kritisch gelten unter anderem:

- Speicherzustand nach Resume nicht konsistent,
- unkontrollierte DMA-Aktivität während Suspend,
- beschädigtes, aber als gültig akzeptiertes Hibernate-Image,
- Ausfall der Thermal-Notabschaltung,
- Verlust kritischer Dateisystemdaten während eines Übergangs,
- Resume mit nicht wiederherstellbarem Kernelzustand.

In diesen Fällen wird Recovery oder die Kernel-Panic-Policy aktiviert.

## 59. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Abfrage unterstützter Energiezustände,
2. CPU Idle mit kurzer Deadline,
3. CPU Idle mit langer Deadline,
4. Realtime-Latenzgrenze,
5. CPU-Frequenzanpassung,
6. Wechsel zwischen Energieprofilen,
7. heterogene CPU-Platzierung,
8. CPU-Offlining und -Onlining,
9. Runtime-Suspend eines Geräts,
10. automatisches Aufwecken bei Nutzung,
11. Autosuspend-Verzögerung,
12. Power-Domain-Abhängigkeit,
13. Wake Lock mit Timeout,
14. Suspend to Idle,
15. Suspend to RAM,
16. Gerätesuspend in Abhängigkeitsreihenfolge,
17. Suspend-Rollback nach Treiberfehler,
18. Resume-Reihenfolge,
19. Zeitkorrektur nach Resume,
20. Wake durch Timer,
21. Wake durch Eingabegerät,
22. Hibernate-Image-Erzeugung,
23. verschlüsseltes Hibernate-Image,
24. Wiederherstellung aus Hibernate,
25. Ablehnung eines manipulierten Images,
26. Ablehnung eines inkompatiblen Images,
27. Hybrid Sleep,
28. kritischer Batteriestand,
29. kontrollierter Shutdown,
30. Neustart in Recovery,
31. passive Thermal-Drosselung,
32. aktive Lüftersteuerung,
33. kritische Thermalabschaltung,
34. Prozessende mit aktivem Wake Lock,
35. Treiber-Timeout während Suspend,
36. Capability-Prüfung globaler Energieoperationen,
37. parallele Geräteübergänge,
38. korrekte Diagnose und Statistik.

## 60. Verbindliche Invarianten

1. Ein Gerät wird nur in einen von Hardware und Treiber unterstützten Zustand versetzt.
2. Geräte werden von Kindern zu Eltern suspendiert und von Eltern zu Kindern fortgesetzt.
3. Suspend konfiguriert alle erforderlichen Wake-Quellen vor dem Plattformübergang.
4. Ein fehlgeschlagener Suspend führt zu Rollback oder einem klar definierten sicheren Zustand.
5. Hibernate-Images werden vor der Wiederherstellung vollständig validiert.
6. Ein unvollständiges Hibernate-Image wird niemals wiederhergestellt.
7. Kritische Thermalgrenzen können nicht durch normale Benutzerprofile deaktiviert werden.
8. Aktive DMA-Zugriffe werden vor einem inkompatiblen Schlafzustand beendet oder sicher isoliert.
9. CPU-Idle-Zustände beachten die nächste Deadline und Latenzanforderungen.
10. Wake Locks werden spätestens beim Ende ihres Besitzers freigegeben.
11. Globale Energieübergänge erfordern passende Capabilities.
12. Power-Zustandsänderungen erfolgen unter geeigneter Synchronisation.

## 61. Referenzablauf eines Suspend-to-RAM

```text
Suspend wird angefordert
  -> Capability und Plattformunterstützung prüfen
  -> Wake Locks und Sperrgründe prüfen
  -> Userspace benachrichtigen und einfrieren
  -> Dateisysteme und Blockgeräte synchronisieren
  -> Geräte von Kindern zu Eltern suspendieren
  -> Wake-Quellen konfigurieren
  -> sekundäre CPUs stoppen
  -> Timer- und Interruptzustand sichern
  -> Plattform-Suspend ausführen
  -> Wake-Ereignis empfangen
  -> CPU-, Timer- und Interruptzustand wiederherstellen
  -> Geräte von Eltern zu Kindern fortsetzen
  -> Userspace auftauen
  -> Wake-Grund und Ergebnis veröffentlichen
```

## 62. Zusammenfassung

Der NovaOS Power Manager koordiniert CPU-, Geräte- und systemweite Energiezustände über ein gemeinsames, capability-geschütztes Modell.

CPU Idle, Frequenzsteuerung und Runtime Device Sleep reduzieren den Energieverbrauch während des normalen Betriebs. Suspend, Hibernate und Hybrid Sleep verwenden definierte Phasen, Abhängigkeitsreihenfolgen und Rollback-Pfade.

Thermal Management, Wake-Quellen, verschlüsselte Hibernate-Images und ehrliche Hardware-Fähigkeitsprüfung gewährleisten, dass Energieeinsparungen weder Datenintegrität noch Sicherheits- und Latenzanforderungen unkontrolliert gefährden.