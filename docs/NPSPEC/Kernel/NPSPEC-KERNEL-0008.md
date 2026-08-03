# NPSPEC-KERNEL-0008 – Kernel Heap

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0008 |
| Titel | Kernel Heap |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Speicherverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0006, NPSPEC-KERNEL-0007 |
| Zugehörige ADRs | ADR-MM-0006, ADR-MM-0007, ADR-MM-0008, ADR-MM-0013 |

---

## 1. Zweck

Diese Spezifikation definiert den Kernel Heap von NovaOS.

Der Kernel Heap stellt dynamischen Speicher für Kernelkomponenten bereit und verwaltet:

- kleine und mittelgroße Kernelallokationen,
- Objekt- und Strukturallokationen,
- Cache-basierte Allokationen,
- große virtuelle Kernelbereiche,
- Speicherfreigaben,
- Guard- und Diagnosefunktionen,
- Notfall- und atomare Allokationen.

## 2. Abgrenzung

Der Kernel Heap verwaltet dynamischen virtuellen Kernelspeicher.

| Komponente | Verantwortung |
|---|---|
| Physical Memory Manager | physische Seitenrahmen |
| Virtual Memory Manager | virtuelle Kernelabbildungen |
| Kernel Heap | dynamische Speicherblöcke |
| Object Cache | typisierte Kernelobjekte |
| Userspace Allocator | Speicherverwaltung innerhalb eines Prozesses |

Der Kernel Heap ist nicht für normale Userspace-Allokationen zuständig.

## 3. Anforderungen

Der Kernel Heap muss:

- auf Mehrprozessorsystemen skalieren,
- kleine Allokationen effizient behandeln,
- Speicherfragmentierung begrenzen,
- unterschiedliche Allokationskontexte unterstützen,
- Speicherfehler früh erkennen,
- sensible Speicherbereiche sicher behandeln,
- Statistiken pro Komponente bereitstellen,
- definierte Fehler statt undefinierten Verhaltens liefern.

## 4. Initialisierungsphasen

Der Kernel Heap wird in mehreren Stufen initialisiert:

| Phase | Funktion |
|---|---|
| Bootstrap Heap | frühe, begrenzte Allokationen |
| Primary Heap | reguläre Kernelallokationen |
| Object Caches | typisierte Objektallokationen |
| Diagnostic Mode | Guarding, Poisoning und Tracing |
| Operational | vollständiger Mehrprozessorbetrieb |

Vor der Initialisierung des Primary Heap darf `np_kalloc()` nicht verwendet werden.

## 5. Bootstrap Heap

Der Bootstrap Heap wird nach Aufbau der grundlegenden virtuellen Speicherverwaltung aktiviert.

Er darf:

- Speicher aus einem reservierten Bereich verwenden,
- einfache ausgerichtete Allokationen bereitstellen,
- auf Freigaben verzichten oder diese nur eingeschränkt unterstützen,
- keine komplexen Abhängigkeiten auf andere Kernelkomponenten besitzen.

Alle dauerhaft benötigten Bootstrap-Allokationen müssen später übernommen oder ausdrücklich reserviert werden.

## 6. Allokator-Architektur

NovaOS verwendet ein kombiniertes Verfahren:

- Größenklassen für kleine und mittelgroße Allokationen,
- typisierte Object Caches für häufige Kernelobjekte,
- direkte VMM-/PMM-Allokationen für große Bereiche,
- Per-CPU-Caches für häufig verwendete Blöcke,
- Guard Pages für ausgewählte sensible Allokationen.

Die konkrete interne Implementierung darf weiterentwickelt werden, ohne die öffentliche Kernel-API zu verändern.

## 7. Größenklassen

Kleine und mittelgroße Allokationen werden auf feste Größenklassen aufgerundet.

Empfohlene Ausgangsklassen sind:

```text
16, 32, 64, 96, 128, 192, 256, 384,
512, 768, 1024, 1536, 2048, 4096,
8192, 16384 und 32768 Byte
```

Die tatsächlichen Klassen dürfen je nach Architektur und Messwerten angepasst werden.

Die gewählte Klasse muss mindestens die angeforderte Größe und Ausrichtung aufnehmen können.

## 8. Große Allokationen

Allokationen oberhalb der höchsten regulären Größenklasse werden direkt über den VMM bereitgestellt.

Große Allokationen können bestehen aus:

- zusammenhängendem virtuellen Speicher,
- mehreren nicht zusammenhängenden physischen Seiten,
- optionalen Guard Pages,
- eigenen Allokationsmetadaten.

Physisch zusammenhängender Speicher muss ausdrücklich über das PMM- oder DMA-Framework angefordert werden.

## 9. Heap-Blöcke

Jede Allokation besitzt intern nachvollziehbare Metadaten.

```c
typedef struct np_heap_block_header {
    uint32_t magic;
    uint32_t flags;

    size_t requested_size;
    size_t allocated_size;

    uint32_t owner_tag;
    uint32_t generation;
} np_heap_block_header_t;
```

Die Metadaten dürfen gegen einfache Über- und Unterschreibungen geschützt werden.

Produktive Builds können kompaktere Metadaten verwenden, solange die Sicherheitsinvarianten erhalten bleiben.

## 10. Ausrichtung

Die Standardausrichtung muss mindestens für alle grundlegenden Datentypen der Architektur geeignet sein.

```c
#define NP_HEAP_DEFAULT_ALIGNMENT 16
```

Spezielle Ausrichtungen werden über eine eigene API angefordert.

Die Ausrichtung muss:

- eine Zweierpotenz sein,
- mindestens der Mindestanforderung entsprechen,
- innerhalb des unterstützten Bereichs liegen,
- ohne Größenüberlauf berechnet werden können.

## 11. Basis-API

```c
void* np_kalloc(
    size_t size,
    uint32_t flags
);

void* np_kcalloc(
    size_t count,
    size_t element_size,
    uint32_t flags
);

void* np_krealloc(
    void* memory,
    size_t new_size,
    uint32_t flags
);

void np_kfree(
    void* memory
);
```

`np_kcalloc()` muss die Multiplikation auf Überlauf prüfen.

`np_kfree(NULL)` ist zulässig und führt keine Operation aus.

## 12. Ausgerichtete Allokation

```c
void* np_kalloc_aligned(
    size_t size,
    size_t alignment,
    uint32_t flags
);
```

Der zurückgegebene Zeiger muss die angeforderte Ausrichtung erfüllen.

Die Freigabe erfolgt regulär über `np_kfree()`. Der Aufrufer darf den ursprünglichen Blockanfang nicht selbst rekonstruieren müssen.

## 13. Allokationsflags

```c
typedef enum np_heap_flags {
    NP_HEAP_ZERO          = 1U << 0,
    NP_HEAP_NO_WAIT       = 1U << 1,
    NP_HEAP_CAN_RECLAIM   = 1U << 2,
    NP_HEAP_CRITICAL      = 1U << 3,
    NP_HEAP_ATOMIC        = 1U << 4,
    NP_HEAP_GUARDED       = 1U << 5,
    NP_HEAP_SENSITIVE     = 1U << 6,
    NP_HEAP_NO_DUMP       = 1U << 7,
    NP_HEAP_EXECUTABLE    = 1U << 8
} np_heap_flags_t;
```

Unbekannte oder widersprüchliche Flags müssen abgelehnt werden.

## 14. Nullinitialisierung

Das Flag `NP_HEAP_ZERO` garantiert, dass der vollständig angeforderte Speicherbereich vor der Rückgabe genullt wurde.

Ohne dieses Flag ist der Inhalt einer neuen Allokation nicht definiert.

Speicher aus einer anderen Sicherheitsdomain darf dennoch keine sensiblen Altdaten offenlegen. Die dafür erforderliche Seitennullung wird durch PMM und VMM sichergestellt.

## 15. Atomare Allokationen

Allokationen mit `NP_HEAP_ATOMIC` dürfen:

- nicht blockieren,
- keinen synchronen Reclaim auslösen,
- keine schlafenden Sperren verwenden,
- nur vorbereitete Reserven oder geeignete CPU-lokale Caches verwenden.

Atomare Allokationen können trotz insgesamt verfügbarem Speicher fehlschlagen.

Sie sind nur für kurze hardwarenahe oder nicht blockierbare Pfade vorgesehen.

## 16. Kritische Allokationen

`NP_HEAP_CRITICAL` erlaubt die kontrollierte Verwendung einer begrenzten Notfallreserve.

Es darf nur durch autorisierte Kernelkomponenten verwendet werden, beispielsweise:

- Speicherbereinigung,
- Prozessbeendigung,
- Fehlerbehandlung,
- Recovery,
- Crash-Dump-Erzeugung.

Eine kritische Allokation garantiert keinen Erfolg.

## 17. Sensitive Allokationen

Allokationen mit `NP_HEAP_SENSITIVE` werden für geheime oder sicherheitsrelevante Daten verwendet.

Dazu können gehören:

- kryptografische Schlüssel,
- Authentifizierungsdaten,
- Stack Canaries,
- Trust-Backend-Zustände,
- geschützte Sicherheitskontexte.

Bei der Freigabe muss der Speicher mit einer nicht optimierbaren Löschoperation überschrieben werden.

```c
void np_secure_zero(
    void* memory,
    size_t size
);
```

## 18. Nicht ausführbarer Heap

Kernel-Heap-Speicher ist standardmäßig nicht ausführbar.

Das Flag `NP_HEAP_EXECUTABLE` ist nur für ausdrücklich autorisierte Komponenten zulässig und erfordert eine spezielle Capability oder interne Kernelberechtigung.

Beschreibbarer und gleichzeitig ausführbarer Heap-Speicher ist unzulässig.

Codegenerierung muss über einen kontrollierten Übergang erfolgen:

```text
Writable + Non-Executable
  -> Inhalt erzeugen
  -> Cache synchronisieren
  -> Read-only + Executable
```

## 19. Guarded Allocations

Sicherheitskritische oder diagnostische Allokationen können durch Guard Pages geschützt werden.

```text
Guard Page
Allocation
Guard Page
```

Guarded Allocations eignen sich insbesondere für:

- große Parserpuffer,
- sicherheitskritische Tabellen,
- Treiberzustände,
- Debug- und Test-Builds.

Wegen des höheren Speicherverbrauchs müssen sie gezielt eingesetzt werden.

## 20. Object Caches

Häufig verwendete Kernelobjekte erhalten typisierte Caches.

```c
typedef struct np_object_cache np_object_cache_t;

np_status_t np_object_cache_create(
    const np_object_cache_config_t* config,
    np_object_cache_t** cache
);

void* np_object_cache_allocate(
    np_object_cache_t* cache,
    uint32_t flags
);

void np_object_cache_free(
    np_object_cache_t* cache,
    void* object
);
```

Typische Cacheobjekte sind:

- Threads,
- Prozesse,
- Handles,
- VFS-Nodes,
- Nachrichten,
- Timer,
- Speicherregionen,
- Capability-Einträge.

## 21. Object-Cache-Konfiguration

```c
typedef struct np_object_cache_config {
    const char* name;
    size_t object_size;
    size_t alignment;

    void (*constructor)(void* object);
    void (*destructor)(void* object);

    uint32_t flags;
    uint32_t owner_tag;
} np_object_cache_config_t;
```

Konstruktoren und Destruktoren dürfen nicht voraussetzen, dass sie in jedem Kontext blockieren können.

Fehleranfällige Objektinitialisierung sollte außerhalb des Cache-Konstruktors erfolgen.

## 22. Slabs

Object Caches und Größenklassen können Speicher in Slabs organisieren.

Ein Slab enthält:

- eine oder mehrere physische Seiten,
- Objekte derselben Größenklasse,
- Belegungsinformationen,
- Referenz auf seinen Cache,
- Listenverknüpfungen für Voll-, Teil- und Leerzustände.

```c
typedef enum np_slab_state {
    NP_SLAB_EMPTY,
    NP_SLAB_PARTIAL,
    NP_SLAB_FULL
} np_slab_state_t;
```

Teilbelegte Slabs werden bevorzugt verwendet, um Fragmentierung zu reduzieren.

## 23. Per-CPU-Caches

Kleine Allokationen können über CPU-lokale Caches beschleunigt werden.

Per-CPU-Caches reduzieren:

- globale Sperrkonflikte,
- Cache-Line-Migrationen,
- Allokationslatenz,
- NUMA-fremde Zugriffe.

Sie müssen begrenzt bleiben und bei folgenden Ereignissen geleert werden können:

- Speicherknappheit,
- CPU-Hotplug,
- Cache-Zerstörung,
- Kernel-Diagnose,
- Systemabschaltung.

## 24. NUMA-Verhalten

Allokationen sollen bevorzugt Speicher des lokalen NUMA-Nodes verwenden.

Object Caches dürfen Node-lokale Slabs verwalten.

Ein Ausweichen auf andere Nodes ist zulässig, wenn:

- die lokale Zone erschöpft ist,
- die Allokationsrichtlinie dies erlaubt,
- keine feste NUMA-Bindung besteht.

Der Besitzer-Tag bleibt unabhängig vom NUMA-Node erhalten.

## 25. Freigabe

Bei `np_kfree()` muss der Kernel prüfen:

- ob der Zeiger zu einer gültigen Allokation gehört,
- ob der Block noch nicht freigegeben wurde,
- welcher Cache oder große Allocator zuständig ist,
- ob Sicherheitslöschung erforderlich ist,
- ob Guard- oder Metadaten beschädigt wurden.

Die Freigabe eines beliebigen oder verschobenen Zeigers ist unzulässig.

## 26. Doppelte Freigabe

Eine doppelte Freigabe muss möglichst früh erkannt werden.

Mögliche Schutzmechanismen sind:

- Allokationszustand in Metadaten,
- Generationswerte,
- Pointer-Quarantäne,
- Poisoning freigegebener Blöcke,
- getrennte Freilistenkodierung.

Eine erkannte doppelte Freigabe ist ein schwerer Kernel-Speicherfehler.

## 27. Poisoning

In Diagnose- und Härtungsprofilen können Speicherbereiche mit definierten Mustern überschrieben werden.

Beispiel:

| Zustand | Muster |
|---|---|
| neu, nicht initialisiert | `0xCD` |
| freigegeben | `0xDD` |
| Guard-Bereich | `0xFD` |

Die Muster sind nicht Bestandteil der stabilen ABI.

Sensitive Allokationen werden sicher genullt und nicht ausschließlich mit Diagnosemustern überschrieben.

## 28. Quarantäne

Freigegebene Blöcke können vorübergehend in einer Quarantäne verbleiben.

Dies erschwert und erkennt:

- Use-after-free,
- unmittelbare Wiederverwendung manipulierter Objekte,
- bestimmte Heap-Spraying-Angriffe.

Die Quarantäne muss größenbegrenzt sein und bei Speicherknappheit verkleinert werden können.

## 29. Reallokation

`np_krealloc()` verhält sich wie folgt:

- bei `memory == NULL` wie `np_kalloc()`,
- bei `new_size == 0` wie `np_kfree()` und liefert `NULL`,
- bei ausreichender bestehender Kapazität kann derselbe Zeiger zurückgegeben werden,
- andernfalls wird ein neuer Block erzeugt und der alte Inhalt kopiert.

Kopiert wird höchstens:

```text
min(old_requested_size, new_size)
```

Schlägt die neue Allokation fehl, bleibt der alte Block unverändert gültig.

## 30. Fragmentierung

Der Kernel Heap unterscheidet:

- interne Fragmentierung durch Größenklassen,
- externe Fragmentierung virtueller oder physischer Bereiche,
- ungenutzte Kapazität in teilweise belegten Slabs.

Gegenmaßnahmen sind:

- geeignete Größenklassen,
- Wiederverwendung teilbelegter Slabs,
- Rückgabe vollständig leerer Slabs,
- begrenzte Per-CPU-Caches,
- getrennte Behandlung großer Allokationen,
- regelmäßige lokale Statistikauswertung.

## 31. Reclaim

Der Kernel Heap kann Speicher an PMM und VMM zurückgeben.

Reclaim-fähig sind beispielsweise:

- vollständig leere Slabs,
- überfüllte Per-CPU-Caches,
- Quarantäneblöcke,
- optionale Diagnosepuffer,
- nicht benötigte Object-Cache-Reserven.

Der Heap darf belegte Kernelobjekte nicht eigenständig verwerfen.

## 32. Speicherknappheit

Bei Speicherknappheit erfolgt:

1. lokale Cachebereinigung,
2. Rückgabe leerer Slabs,
3. Verkleinerung der Quarantäne,
4. systemweiter Kernel-Reclaim,
5. kontrollierte Nutzung der Notfallreserve,
6. Rückgabe eines Allokationsfehlers,
7. gegebenenfalls Aktivierung der OOM-Policy.

Normale Allokationsfehler dürfen nicht automatisch eine Kernel Panic auslösen.

## 33. Owner Tags

Jede Allokation kann einer Kernelkomponente zugeordnet werden.

```c
typedef uint32_t np_heap_owner_tag_t;
```

Beispiele sind:

```text
PROCESS_MANAGER
THREAD_MANAGER
VMM
VFS
NETWORK
IPC
DEVICE_MANAGER
SECURITY
DIAGNOSTICS
```

Owner Tags dienen Abrechnung und Diagnose. Sie ersetzen keine Capability-Prüfung.

## 34. Allokationskontexte

| Kontext | Blockieren | Reclaim | Erlaubte Quelle |
|---|---:|---:|---|
| Normal | Ja | Ja | regulärer Heap |
| No-Wait | Nein | Nein | verfügbare Caches |
| Atomic | Nein | Nein | atomare Reserven |
| Critical | begrenzt | Ja | Heap und Notfallreserve |
| Realtime | nach Richtlinie | möglichst Nein | vorbereitete Pools |

Kernelkomponenten müssen den richtigen Kontext angeben.

## 35. Sperrregeln

Der Heap verwendet:

- CPU-lokale Daten,
- Cache-lokale Sperren,
- atomare Freilisten,
- kurze Spinlocks,
- schlaffähige Sperren nur in erlaubten Kontexten.

Es darf keine einzelne globale Heap-Sperre im normalen Allokationspfad geben.

Eine Allokation darf nicht rekursiv denselben nicht rekursiven Heap-Pfad betreten.

## 36. Rekursionserkennung

Heapinterne Diagnose, Logging- oder Fehlerpfade dürfen keine unkontrollierte weitere Heapallokation auslösen.

Der Kernel soll pro Thread oder CPU erkennen, wenn sich eine Allokation bereits in einem kritischen Heapabschnitt befindet.

Im Rekursionsfall wird:

- ein vorab reservierter Diagnosepuffer verwendet,
- die optionale Diagnose reduziert,
- oder die Operation mit einem definierten Fehler beendet.

## 37. CPU-Hotplug

Vor dem Offline-Schalten einer CPU müssen deren Heap-Caches:

- für neue Allokationen gesperrt,
- geleert,
- an den zuständigen Node- oder globalen Cache zurückgegeben,
- diagnostisch abgeschlossen werden.

Nach dem Online-Schalten werden neue CPU-lokale Caches initialisiert.

## 38. Cache-Zerstörung

Ein Object Cache darf nur zerstört werden, wenn keine aktiven Objekte mehr existieren.

```c
np_status_t np_object_cache_destroy(
    np_object_cache_t* cache
);
```

Vor der Zerstörung müssen:

- neue Allokationen verhindert,
- Per-CPU-Bestände eingesammelt,
- leere Slabs freigegeben,
- aktive Objekte geprüft werden.

Bei vorhandenen aktiven Objekten wird `NP_ERR_BUSY` zurückgegeben.

## 39. Statistik

```c
typedef struct np_heap_statistics {
    uint64_t allocated_bytes;
    uint64_t requested_bytes;
    uint64_t reserved_bytes;
    uint64_t peak_allocated_bytes;

    uint64_t active_allocations;
    uint64_t allocation_count;
    uint64_t free_count;
    uint64_t failed_allocations;

    uint64_t slab_count;
    uint64_t quarantined_bytes;
} np_heap_statistics_t;
```

Statistiken sollen systemweit, pro Cache, pro NUMA-Node und pro Owner Tag abrufbar sein.

## 40. Diagnose

Der Kernel Heap kann folgende lokale Diagnosefunktionen bereitstellen:

- Speicherverbrauch nach Komponente,
- aktive Allokationsanzahl,
- größte Allokationen,
- Fragmentierungswerte,
- fehlgeschlagene Allokationen,
- Guard-Verletzungen,
- doppelte Freigaben,
- Use-after-free-Indikatoren,
- Quarantäneauslastung,
- Cache- und Slab-Zustände.

Vollständige Allokations-Stacks dürfen nur in autorisierten Diagnoseprofilen aufgezeichnet werden.

## 41. Leak Detection

In Diagnose-Builds kann jede Allokation zusätzliche Informationen erhalten:

- Besitzer-Tag,
- Threadkennung,
- Zeitpunkt,
- Aufrufstelle,
- Generationskennung,
- optional einen begrenzten Stacktrace.

Beim Herunterfahren oder Entladen einer Komponente können verbliebene Allokationen gemeldet werden.

Leak Detection ist standardmäßig lokal und darf keine sensiblen Speicherinhalte erfassen.

## 42. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_POINTER` | Zeiger gehört nicht zum Heap |
| `NP_ERR_INVALID_ALIGNMENT` | Ausrichtung ist ungültig |
| `NP_ERR_OUT_OF_MEMORY` | kein geeigneter Speicher verfügbar |
| `NP_ERR_NO_ATOMIC_MEMORY` | atomare Reserve erschöpft |
| `NP_ERR_DOUBLE_FREE` | Block wurde bereits freigegeben |
| `NP_ERR_HEAP_CORRUPTION` | Heapmetadaten wurden beschädigt |
| `NP_ERR_OVERFLOW` | Größenberechnung ist übergelaufen |
| `NP_ERR_ACCESS_DENIED` | Operation ist nicht autorisiert |
| `NP_ERR_BUSY` | Cache enthält aktive Objekte |
| `NP_ERR_NOT_SUPPORTED` | angeforderte Funktion nicht unterstützt |

Zeigerbasierte Standardfunktionen melden Fehler normalerweise über `NULL` und einen kernelinternen Fehlerstatus.

## 43. Sicherheitsanforderungen

Der Kernel Heap muss sicherstellen, dass:

- Heap-Speicher standardmäßig nicht ausführbar ist,
- neue Sicherheitsdomains keine sensiblen Altdaten erhalten,
- sensitive Allokationen bei Freigabe sicher gelöscht werden,
- Größenberechnungen auf Überlauf geprüft werden,
- doppelte und ungültige Freigaben erkannt werden,
- Metadaten nicht ungeprüft aus Nutzdaten abgeleitet werden,
- Guarded Allocations tatsächlich unzugängliche Guard Pages verwenden,
- Notfallreserven nur autorisierten Pfaden zur Verfügung stehen,
- Diagnoseinformationen keine Speicherinhalte offenlegen,
- CPU- und NUMA-lokale Caches ihre Grenzen einhalten.

## 44. Fehlerbehandlung

Normale Speicherknappheit wird als behandelbarer Fehler an den Aufrufer zurückgegeben.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- beschädigte Heapmetadaten,
- doppelte Freigabe,
- Freigabe eines unbekannten Zeigers,
- Schreibzugriff in eine Guard Page,
- Use-after-free eines kritischen Kernelobjekts,
- beschädigte Slab- oder Freilistenstruktur,
- ausführbare und beschreibbare Heapabbildung ohne autorisierten Übergang.

Je nach Auswirkung wird der betroffene Cache isoliert oder die Kernel-Panic-Policy aktiviert.

## 45. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Allokation jeder Größenklasse,
2. Allokation mit Größe null,
3. Nullinitialisierung,
4. ausgerichtete Allokation,
5. Ablehnung ungültiger Ausrichtungen,
6. Überlauferkennung bei `np_kcalloc()`,
7. reguläre Freigabe,
8. `np_kfree(NULL)`,
9. Erkennung doppelter Freigaben,
10. Erkennung ungültiger Zeiger,
11. Reallokation auf größere Größe,
12. Reallokation auf kleinere Größe,
13. Erhalt des alten Blocks bei fehlgeschlagener Reallokation,
14. große direkte VMM-Allokation,
15. Object-Cache-Allokation,
16. Konstruktor und Destruktor,
17. Rückgabe leerer Slabs,
18. Per-CPU-Caches,
19. parallele Allokationen auf mehreren CPUs,
20. NUMA-lokale Allokationen,
21. atomare Allokation,
22. Verhalten bei erschöpfter atomarer Reserve,
23. kritische Allokation aus der Notfallreserve,
24. Guard-Page-Verletzung,
25. Poisoning freigegebener Blöcke,
26. Quarantäne und deren Begrenzung,
27. sichere Löschung sensitiver Daten,
28. Verhinderung ausführbarer Standardspeicherblöcke,
29. CPU-Hotplug mit gefüllten Caches,
30. Cache-Zerstörung mit aktiven Objekten,
31. Leak Detection,
32. Reclaim bei Speicherknappheit,
33. korrekte Statistiken und Owner Tags,
34. vollständige Freigabe großer Allokationen.

## 46. Verbindliche Invarianten

1. Jede aktive Heapallokation besitzt einen eindeutig bestimmbaren Besitzer und eine bekannte Größe.
2. Ein Speicherblock ist niemals gleichzeitig frei und allokiert.
3. Ein freigegebener Block darf nicht erneut freigegeben werden.
4. Kernel-Heap-Speicher ist standardmäßig nicht ausführbar.
5. Sensitive Allokationen werden vor ihrer Wiederverwendung sicher gelöscht.
6. Größen- und Ausrichtungsberechnungen werden auf Überlauf geprüft.
7. Atomare Allokationen blockieren nicht.
8. Per-CPU-Caches halten nur eine begrenzte Speichermenge zurück.
9. Große Allokationen werden über VMM und PMM nachvollziehbar verwaltet.
10. Object Caches werden nicht mit aktiven Objekten zerstört.
11. Guard Pages besitzen keine zugängliche reguläre Abbildung.
12. Heapzustandsänderungen erfolgen unter geeigneter Synchronisation.

## 47. Referenzablauf einer kleinen Allokation

```text
Allokationsanfrage
  -> Größe und Flags prüfen
  -> Größenklasse bestimmen
  -> CPU-lokalen Cache prüfen
  -> teilbelegten Slab suchen
  -> bei Bedarf neuen Slab anlegen
  -> Block als allokiert markieren
  -> Metadaten und Owner Tag setzen
  -> Speicher bei Bedarf nullen
  -> Nutzzeiger zurückgeben
```

## 48. Referenzablauf einer Freigabe

```text
Freigabeanforderung
  -> NULL-Prüfung
  -> Block und Metadaten validieren
  -> doppelte Freigabe ausschließen
  -> Guard- und Integritätsdaten prüfen
  -> sensitive Daten sicher löschen
  -> Block optional vergiften
  -> Block in Quarantäne oder Cache überführen
  -> vollständig leeren Slab bei Bedarf freigeben
  -> Statistik aktualisieren
```

## 49. Zusammenfassung

Der NovaOS Kernel Heap verwendet Größenklassen, Slabs, Object Caches und CPU-lokale Caches für schnelle und skalierbare Kernelallokationen. Große Speicherbereiche werden direkt über Virtual und Physical Memory Manager bereitgestellt.

Guard Pages, Poisoning, Quarantäne, sichere Löschung und Integritätsprüfungen helfen bei der Erkennung von Heap Corruption, Use-after-free und doppelten Freigaben.

Getrennte Allokationskontexte, Notfallreserven und nachvollziehbare Owner Tags ermöglichen einen stabilen Kernelbetrieb auch bei Speicherknappheit und in nicht blockierbaren Ausführungspfaden.