# NPSPEC-KERNEL-0017 – Device Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0017 |
| Titel | Device Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Geräteverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0009, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0016 |
| Zugehörige ADRs | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0003, ADR-DEV-0004, ADR-DEV-0005, ADR-DEV-0006, ADR-DEV-0007, ADR-DEV-0008, ADR-DEV-0009, ADR-DEV-0010 |

---

## 1. Zweck

Diese Spezifikation definiert den Device Manager des NovaOS-Kernels.

Der Device Manager verwaltet:

- Erkennung von Geräten,
- Geräteobjekte und Gerätehierarchien,
- Hardware- und Firmwarebeschreibungen,
- Zuordnung geeigneter Treiber,
- Gerätelebenszyklen,
- Plug-and-Play,
- Ressourcenvergabe,
- Gerätezugriffe,
- Energiezustände,
- Fehlerbehandlung und Recovery.

## 2. Ziele

Der Device Manager muss:

- ein einheitliches Modell für physische und virtuelle Geräte bereitstellen,
- Kernel- und Userspace-Treiber unterstützen,
- capability-geschützte Gerätezugriffe erzwingen,
- dynamisches Hinzufügen und Entfernen ermöglichen,
- Gerätekonflikte verhindern,
- Treiberabstürze möglichst isolieren,
- Abhängigkeiten und Gerätebeziehungen darstellen,
- Energieverwaltung integrieren,
- lokale Diagnoseinformationen bereitstellen.

## 3. Grundmodell

Jedes erkannte Gerät wird als Objekt in den Kernel Object Graph aufgenommen.

```text
Machine
  -> Firmware oder Root Bus
  -> Bus
  -> Bridge oder Hub
  -> Device
  -> Function
  -> Driver Binding
```

Ein Gerät kann physisch, virtuell, firmwaredefiniert oder softwarebereitgestellt sein.

## 4. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Device Manager | Geräteobjekte, Zustände und Treiberbindung |
| Driver Framework | Treiberlaufzeit und Treiberschnittstellen |
| Bus Manager | busspezifische Erkennung und Konfiguration |
| Interrupt Manager | Interruptquellen und Routing |
| DMA Framework | DMA-Objekte und IOMMU-Mappings |
| Power Manager | globale Energieentscheidungen |
| VFS | Darstellung ausgewählter Geräte als Dateiobjekte |
| Object Manager | Identität, Handles und Kernel Object Graph |

## 5. Gerätetypen

```c
typedef enum np_device_kind {
    NP_DEVICE_PHYSICAL,
    NP_DEVICE_VIRTUAL,
    NP_DEVICE_PSEUDO,
    NP_DEVICE_FIRMWARE,
    NP_DEVICE_BUS,
    NP_DEVICE_BRIDGE,
    NP_DEVICE_FUNCTION
} np_device_kind_t;
```

| Typ | Bedeutung |
|---|---|
| `PHYSICAL` | direkt vorhandene Hardware |
| `VIRTUAL` | von Hypervisor oder Software emuliertes Gerät |
| `PSEUDO` | rein logisches Kernel- oder Systemgerät |
| `FIRMWARE` | durch Firmware bereitgestellte Funktion |
| `BUS` | Gerätebus |
| `BRIDGE` | Verbindung zwischen Bussen |
| `FUNCTION` | einzelne Funktion eines Mehrfunktionsgeräts |

## 6. Geräteklassen

```c
typedef enum np_device_class {
    NP_DEVICE_CLASS_UNKNOWN,
    NP_DEVICE_CLASS_PROCESSOR,
    NP_DEVICE_CLASS_MEMORY,
    NP_DEVICE_CLASS_STORAGE,
    NP_DEVICE_CLASS_NETWORK,
    NP_DEVICE_CLASS_DISPLAY,
    NP_DEVICE_CLASS_AUDIO,
    NP_DEVICE_CLASS_INPUT,
    NP_DEVICE_CLASS_USB,
    NP_DEVICE_CLASS_PCI,
    NP_DEVICE_CLASS_SERIAL,
    NP_DEVICE_CLASS_SENSOR,
    NP_DEVICE_CLASS_POWER,
    NP_DEVICE_CLASS_SECURITY,
    NP_DEVICE_CLASS_VIRTUAL
} np_device_class_t;
```

Hersteller- und busspezifische Unterklassen werden über zusätzliche Eigenschaften beschrieben.

## 7. Gerätekennung

```c
typedef uint64_t np_device_id_t;
```

Jedes Geräteobjekt besitzt eine während seiner Lebensdauer eindeutige Gerätekennung.

Die Kennung:

- dient interner Zuordnung und Diagnose,
- ist kein Handle,
- verleiht keine Zugriffsrechte,
- kann sich nach vollständigem Entfernen und erneutem Einstecken ändern.

Persistente Geräteidentitäten werden getrennt behandelt.

## 8. Persistente Identität

Wenn verfügbar, kann ein Gerät eine persistente Identität besitzen, beispielsweise:

- Seriennummer,
- Firmware-UID,
- PCI-Pfad,
- USB-Portpfad und Seriennummer,
- Storage-WWN,
- virtuelles Plattformkennzeichen.

Persistente Kennungen müssen als nicht vertrauenswürdige Gerätedaten validiert werden.

Sie dürfen nicht allein als Sicherheitsnachweis dienen.

## 9. Geräteobjekt

```c
typedef struct np_device {
    np_kernel_object_t object;

    np_device_id_t device_id;
    np_device_kind_t kind;
    np_device_class_t device_class;
    np_device_state_t state;

    np_device_t* parent;
    np_list_t children;

    np_property_store_t properties;
    np_resource_set_t resources;

    np_driver_binding_t* driver_binding;
    np_power_state_t power_state;

    uint32_t flags;
    np_rwlock_t lock;
} np_device_t;
```

Die interne Struktur ist nicht Bestandteil der öffentlichen ABI.

## 10. Gerätezustände

```c
typedef enum np_device_state {
    NP_DEVICE_DISCOVERED,
    NP_DEVICE_ENUMERATING,
    NP_DEVICE_UNBOUND,
    NP_DEVICE_BINDING,
    NP_DEVICE_STARTING,
    NP_DEVICE_ACTIVE,
    NP_DEVICE_SUSPENDING,
    NP_DEVICE_SUSPENDED,
    NP_DEVICE_STOPPING,
    NP_DEVICE_FAILED,
    NP_DEVICE_REMOVING,
    NP_DEVICE_REMOVED
} np_device_state_t;
```

Jeder Zustandswechsel muss atomar und diagnostisch nachvollziehbar sein.

## 11. Zustandsübergänge

Ein regulärer Geräteablauf ist:

```text
DISCOVERED
  -> ENUMERATING
  -> UNBOUND
  -> BINDING
  -> STARTING
  -> ACTIVE
  -> SUSPENDING
  -> SUSPENDED
  -> ACTIVE
  -> STOPPING
  -> REMOVING
  -> REMOVED
```

Ein Fehler kann aus `BINDING`, `STARTING`, `ACTIVE` oder `SUSPENDED` nach `FAILED` führen.

Ein entferntes Gerät darf nicht erneut aktiviert werden.

## 12. Gerätehierarchie

Jedes Gerät besitzt höchstens ein direktes Elternobjekt und kann mehrere Kindgeräte enthalten.

Beispiele:

```text
PCI Root
  -> USB Controller
      -> USB Root Hub
          -> USB Hub
              -> Keyboard
```

Das Entfernen eines Elternobjekts löst die kontrollierte Behandlung seiner Kindgeräte aus.

## 13. Gerätegraph

Neben der Baumhierarchie kann der Kernel Object Graph zusätzliche Beziehungen abbilden:

- Gerät verwendet Interrupt-Controller,
- Gerät gehört zu IOMMU-Domain,
- Storage-Volume basiert auf mehreren Geräten,
- virtuelles Gerät hängt von einem Backend ab,
- Treiberprozess steuert mehrere Funktionen,
- Power Domain versorgt mehrere Geräte.

Starke Besitz- und schwache Abhängigkeitsbeziehungen müssen eindeutig unterschieden werden.

## 14. Gerätequellen

Geräte können erkannt werden über:

- ACPI,
- Device Tree,
- PCI-Enumeration,
- USB-Enumeration,
- Firmwaretabellen,
- virtuelle Plattformbusse,
- statische Plattformdefinitionen,
- Treibererzeugung von Kindgeräten,
- Software-Pseudogeräte.

Alle extern gelieferten Beschreibungen müssen validiert werden.

## 15. Device Tree und Firmwaredaten

Firmwarebeschreibungen werden in ein einheitliches internes Eigenschaftenmodell übersetzt.

Der Device Manager darf später nicht von einem bestimmten Firmwareformat abhängig sein.

Originaldaten können für busspezifische Treiber referenziert werden, müssen aber:

- reserviert oder kopiert,
- größenvalidiert,
- gegen Überläufe geprüft,
- als nicht vertrauenswürdig behandelt werden.

## 16. Geräteeigenschaften

```c
typedef enum np_device_property_type {
    NP_PROPERTY_INTEGER,
    NP_PROPERTY_BOOLEAN,
    NP_PROPERTY_STRING,
    NP_PROPERTY_BINARY,
    NP_PROPERTY_REFERENCE,
    NP_PROPERTY_INTEGER_ARRAY,
    NP_PROPERTY_STRING_ARRAY
} np_device_property_type_t;
```

Typische Eigenschaften sind:

- Herstellerkennung,
- Produktkennung,
- Klassenkennung,
- Revision,
- Seriennummer,
- Busadresse,
- kompatible Treiberkennungen,
- Power-Fähigkeiten,
- Ressourcenanforderungen.

## 17. Property Store

```c
np_status_t np_device_property_get(
    np_device_handle_t device,
    np_string_view_t key,
    np_property_value_t* value
);

np_status_t np_device_property_set(
    np_device_handle_t device,
    np_string_view_t key,
    const np_property_value_t* value
);
```

Treiber dürfen nur Eigenschaften verändern, für die sie ausdrücklich zuständig sind.

Hardware- und Firmwareeigenschaften können schreibgeschützt sein.

## 18. Geräteregistrierung

```c
np_status_t np_device_register(
    np_device_t* parent,
    const np_device_descriptor_t* descriptor,
    np_device_t** device
);
```

Die Registrierung umfasst:

1. Descriptor validieren,
2. Geräteobjekt erzeugen,
3. Elternbeziehung anlegen,
4. Eigenschaften übernehmen,
5. Ressourcenanforderungen registrieren,
6. Objekt in den Kernel Object Graph aufnehmen,
7. Device-Discovered-Ereignis veröffentlichen.

Eine Registrierung aktiviert das Gerät noch nicht automatisch.

## 19. Enumeration

Ein Bus- oder Parent-Treiber enumeriert seine Kindgeräte.

```c
np_status_t np_device_enumerate_children(
    np_device_handle_t parent
);
```

Die Enumeration muss:

- mehrfach sicher aufrufbar sein,
- vorhandene Geräte wiedererkennen,
- neue Geräte hinzufügen,
- verschwundene Geräte markieren,
- Duplikate verhindern,
- Hotplug-Ereignisse berücksichtigen.

## 20. Treiber-Matching

Treiber werden anhand deklarierter Matching-Regeln bewertet.

Mögliche Kriterien sind:

- Bus-Typ,
- Hersteller- und Gerätekennung,
- Klasse und Unterklasse,
- kompatible Zeichenketten,
- ACPI-HID oder CID,
- Device-Tree-Compatible,
- Treiberversion,
- Sicherheitsstatus,
- Plattformprofil.

Das Ergebnis ist eine sortierte Kandidatenliste.

## 21. Match-Priorität

```c
typedef struct np_driver_match_result {
    np_driver_id_t driver_id;
    uint32_t score;
    uint32_t flags;
} np_driver_match_result_t;
```

Eine höhere Bewertung bedeutet eine spezifischere oder bevorzugte Übereinstimmung.

Generische Treiber sollen nur verwendet werden, wenn kein geeigneter spezifischer Treiber verfügbar ist.

Die endgültige Auswahl berücksichtigt zusätzlich Signatur, Richtlinie und Laufzeitumgebung.

## 22. Treiberbindung

```c
np_status_t np_device_bind_driver(
    np_device_handle_t device,
    np_driver_handle_t driver
);
```

Die Bindung erfolgt:

1. Gerätezustand prüfen,
2. Treibersignatur und Richtlinie prüfen,
3. Match-Ergebnis validieren,
4. Ressourcen reservieren,
5. Treiberinstanz erzeugen,
6. Capability-Domain einrichten,
7. Gerät initialisieren,
8. Zustand auf `ACTIVE` setzen.

Bei Fehlern muss die Bindung vollständig zurückgerollt werden.

## 23. Automatische Bindung

Der Device Manager darf automatisch den besten zulässigen Treiber auswählen.

Automatische Bindung kann deaktiviert oder überschrieben werden durch:

- administrative Richtlinie,
- Recovery-Modus,
- Treiber-Blacklist,
- Safe Mode,
- Geräteisolierung,
- manuelle Diagnosekonfiguration.

Eine manuelle Auswahl darf keine Signatur- oder Capability-Prüfung umgehen.

## 24. Kernel- und Userspace-Treiber

NovaOS bevorzugt Userspace-Treiber, wenn die Hardware und Leistungsanforderungen dies erlauben.

Kernel-Treiber sind vorgesehen für:

- frühe Bootgeräte,
- Interrupt-Controller,
- grundlegende Timer,
- Speicherverwaltung,
- hardwarenahe Busfunktionen,
- besonders latenzkritische Pfade.

Userspace-Treiber erhalten ausschließlich die für ihr Gerät erforderlichen Ressourcenhandles.

## 25. Treiberprozess

Ein Userspace-Treiber läuft in einem isolierten Prozess.

Er kann erhalten:

- Gerätehandle,
- MMIO-Memory-Object,
- Interrupt-Endpunkt,
- DMA-Domain,
- Konfigurationsendpunkt,
- Logging-Endpunkt,
- eingeschränkte Power-Management-Rechte.

Der Treiber erhält keinen allgemeinen Zugriff auf physische Speicheradressen oder fremde Geräte.

## 26. Gerätestart

```c
np_status_t np_device_start(
    np_device_handle_t device
);
```

Vor dem Start müssen:

- Treiberbindung bestehen,
- Ressourcen zugewiesen sein,
- Interrupts vorbereitet, aber gegebenenfalls maskiert sein,
- DMA-Domain eingerichtet sein,
- Power State ausreichend aktiv sein,
- abhängige Eltern- und Backendgeräte verfügbar sein.

Interrupts werden erst freigegeben, wenn der Treiber vollständig empfangsbereit ist.

## 27. Gerätestopp

```c
np_status_t np_device_stop(
    np_device_handle_t device,
    uint32_t reason
);
```

Der Stopp umfasst:

1. neue Clientoperationen verhindern,
2. I/O-Anfragen abschließen oder abbrechen,
3. Interruptquellen maskieren,
4. DMA stoppen,
5. Kindgeräte behandeln,
6. Hardware in sicheren Zustand versetzen,
7. Ressourcen freigeben,
8. Zustand aktualisieren.

## 28. Hotplug

Hotplug-Ereignisse werden vom zuständigen Bus- oder Plattformtreiber gemeldet.

Der Device Manager unterscheidet:

- Hinzufügen,
- angekündigtes Entfernen,
- überraschendes Entfernen,
- Zustandsänderung,
- erneute Enumeration.

Ereignisse müssen entprellt und gegen doppelte Verarbeitung geschützt werden.

## 29. Sicheres Entfernen

Vor einem angekündigten Entfernen wird geprüft:

- aktive Handles,
- offene Dateien oder Volumes,
- laufende I/O-Anfragen,
- DMA-Operationen,
- abhängige Kindgeräte,
- Dateisystem-Mounts,
- nicht migrierbare Ressourcen.

Kann das Gerät nicht sicher entfernt werden, wird die Operation mit einer Begründung abgelehnt.

## 30. Überraschendes Entfernen

Bei überraschendem Entfernen muss der Device Manager:

1. Gerät sofort als nicht verfügbar markieren,
2. neue Operationen ablehnen,
3. Interrupts und DMA stoppen,
4. wartende Clients mit Fehler aufwecken,
5. Treiber benachrichtigen oder beenden,
6. Kindgeräte entfernen,
7. Ressourcen freigeben,
8. Diagnoseereignis erzeugen.

Treiber dürfen nach diesem Ereignis nicht mehr auf MMIO- oder Portbereiche des Geräts zugreifen.

## 31. Hardware-Ressourcen

```c
typedef enum np_device_resource_type {
    NP_RESOURCE_MMIO,
    NP_RESOURCE_IO_PORT,
    NP_RESOURCE_INTERRUPT,
    NP_RESOURCE_DMA,
    NP_RESOURCE_BUS_RANGE,
    NP_RESOURCE_MEMORY_WINDOW
} np_device_resource_type_t;
```

Ressourcen werden durch zentrale Manager reserviert und dem Gerät capability-geschützt zugeordnet.

## 32. Ressourcenkonflikte

Der Device Manager muss doppelte oder widersprüchliche Zuweisungen verhindern.

Dies betrifft insbesondere:

- MMIO-Bereiche,
- I/O-Ports,
- exklusive Interrupts,
- Busnummern,
- DMA-Kanäle,
- Speicherfenster.

Gemeinsame Nutzung ist nur zulässig, wenn Ressourcentyp, Controller und alle beteiligten Treiber sie ausdrücklich unterstützen.

## 33. MMIO

MMIO-Bereiche werden über den Virtual Memory Manager abgebildet.

Sie müssen:

- nicht ausführbar sein,
- den richtigen Cachemodus verwenden,
- auf die Gerätegrenzen beschränkt sein,
- beim Entfernen ungültig gemacht werden,
- nur der zugewiesenen Treiberdomain zugänglich sein.

Userspace-Treiber erhalten ein eingeschränktes Memory-Object statt einer ungeprüften physischen Adresse.

## 34. Interrupts

Interruptquellen werden über den Interrupt Manager eingerichtet.

Der Device Manager stellt sicher, dass:

- Quelle und Gerät zusammengehören,
- Trigger-Modus und Polarität korrekt sind,
- MSI oder MSI-X sicher programmiert werden,
- Userspace-Treiber einen Interrupt-Endpunkt erhalten,
- Quellen beim Treiberstopp maskiert werden.

Die Interrupt-Affinität kann an Gerätequeues und NUMA-Topologie angepasst werden.

## 35. DMA

DMA wird ausschließlich über das DMA Framework eingerichtet.

Vor einer DMA-Freigabe müssen:

- Gerät und Treiberdomain validiert,
- IOMMU-Domain zugeordnet,
- Puffer gepinnt oder kontrolliert abgebildet,
- Richtung und Länge definiert,
- Cache-Kohärenz berücksichtigt werden.

Ein Gerät darf nicht auf beliebigen physischen Speicher zugreifen.

## 36. Gerätezugriff

Clients öffnen Geräte über capability-geschützte Handles.

```c
np_status_t np_device_open(
    np_device_handle_t device,
    uint32_t access,
    uint32_t flags,
    np_handle_t* session
);
```

Der Device Manager oder der zuständige Treiber erzeugt ein Sessionobjekt.

Mehrere Clients dürfen nur entsprechend der Gerätesemantik gleichzeitig zugreifen.

## 37. Gerätesitzungen

Eine Gerätesitzung enthält:

- Clientidentität,
- erlaubte Operationen,
- Treiberendpunkt,
- laufende I/O-Anfragen,
- Sitzungszustand,
- optionale Exklusivsperre.

Beim Prozessende werden seine Sitzungen automatisch geschlossen.

Das Schließen einer Sitzung darf nicht automatisch das physische Gerät entfernen.

## 38. Geräte-Namespaces

Geräte können in einem kontrollierten Namespace veröffentlicht werden.

Beispiele:

```text
/device/pci/0000:00:1f.2
/device/usb/controller0/port2
/device/storage/system
/device/input/keyboard0
```

Ein Name dient der Auffindbarkeit. Zugriff erfordert weiterhin ein passendes Handle oder eine Capability.

Sandboxen können eine eingeschränkte Gerätesicht erhalten.

## 39. Geräte-Aliase

Stabile logische Aliase können auf wechselnde physische Geräteobjekte verweisen.

Beispiele:

```text
/device/display/primary
/device/audio/default-output
/device/network/default
```

Aliasänderungen müssen atomar erfolgen und ein Zustandsereignis erzeugen.

Ein Alias verleiht keine zusätzlichen Zugriffsrechte.

## 40. Energiezustände

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

Nicht jede Plattform oder Geräteklasse unterstützt alle Zustände.

Der Device Manager übersetzt generische Anforderungen in busspezifische Zustände.

## 41. Power-Übergänge

Vor einem niedrigeren Energiezustand müssen:

- neue I/O-Anfragen kontrolliert werden,
- bestehende Transfers abgeschlossen oder abgebrochen,
- DMA gestoppt,
- Wake-Quellen konfiguriert,
- Treiberzustände gespeichert werden.

Nach dem Aufwachen werden Ressourcen, Interrupts und Gerätezustand erneut validiert.

## 42. Wake-fähige Geräte

Ein Gerät kann als Wake-Quelle dienen, wenn:

- Hardware und Plattform dies unterstützen,
- der Treiber eine Wake-Methode bereitstellt,
- die Power-Richtlinie dies erlaubt,
- eine geeignete Capability vorliegt.

Unprivilegierte Prozesse dürfen Wake-Funktionen nicht unbegrenzt aktivieren.

## 43. Abhängigkeiten

Geräte können Start- und Laufzeitabhängigkeiten besitzen.

Beispiele:

- USB-Gerät benötigt Host Controller und Hub,
- Netzwerkgerät benötigt PCI-Funktion und IOMMU-Domain,
- virtuelles Blockgerät benötigt Backendservice,
- Display benötigt Firmware- und Power-Domain.

Abhängigkeiten werden als Graphbeziehungen verwaltet und vor Start oder Stopp ausgewertet.

## 44. Startreihenfolge

Geräte werden grundsätzlich von Eltern zu Kindern gestartet.

Empfohlene Reihenfolge während des Bootvorgangs:

1. Firmware- und Plattformgeräte,
2. Interrupt- und Timercontroller,
3. Busse und Bridges,
4. IOMMU- und DMA-Infrastruktur,
5. Konsolen- und Diagnosegeräte,
6. Storage-Controller,
7. Root-Dateisystem-Geräte,
8. Eingabe- und Anzeigegeräte,
9. Netzwerkgeräte,
10. optionale Geräte.

Unabhängige Geräte dürfen nach Aktivierung des Schedulers parallel starten.

## 45. Treiberfehler

Bei einem Treiberfehler kann der Device Manager:

- einzelne I/O-Anfrage abbrechen,
- Gerät zurücksetzen,
- Treiberinstanz neu starten,
- Userspace-Treiberprozess neu starten,
- Gerät neu binden,
- Fallback-Treiber auswählen,
- Gerät isolieren.

Ein automatischer Neustart muss rate-limitiert werden.

## 46. Geräte-Reset

```c
np_status_t np_device_reset(
    np_device_handle_t device,
    uint32_t reset_type
);
```

Reset-Arten können sein:

- Function Reset,
- Bus Reset,
- Power Cycle,
- Software Reset,
- Platform Reset.

Ein Bus Reset kann mehrere Geräte betreffen und erfordert daher eine Abhängigkeitsprüfung.

## 47. Recovery-Richtlinie

```c
typedef struct np_device_recovery_policy {
    uint32_t maximum_restarts;
    uint64_t restart_window_ns;
    uint32_t fallback_action;
    uint32_t flags;
} np_device_recovery_policy_t;
```

Nach wiederholtem Fehler wird ein Gerät nicht unbegrenzt neu gestartet.

Mögliche Fallback-Aktionen sind:

- deaktivieren,
- generischen Treiber verwenden,
- Safe Mode aktivieren,
- Recovery-Umgebung benachrichtigen.

## 48. Treiber-Update

Ein Treiber kann nur dann live aktualisiert werden, wenn:

- der Treiberzustand exportiert oder neu aufgebaut werden kann,
- Gerät und Clients vorübergehend angehalten werden können,
- die neue Version kompatibel und verifiziert ist,
- Rollback möglich bleibt.

Kernel-Treiberupdates können strengere Bedingungen oder einen Neustart erfordern.

## 49. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_DEVICE_QUERY` | Geräteinformationen lesen |
| `CAP_DEVICE_OPEN` | Gerät oder Gerätesitzung öffnen |
| `CAP_DEVICE_CONFIGURE` | Geräteeinstellungen verändern |
| `CAP_DEVICE_CONTROL` | gerätespezifische Steueroperationen ausführen |
| `CAP_DEVICE_POWER` | Energiezustand verändern |
| `CAP_DEVICE_RESET` | Gerät zurücksetzen |
| `CAP_DEVICE_BIND_DRIVER` | Treiber binden oder wechseln |
| `CAP_DEVICE_RESOURCE` | Hardware-Ressourcen verwalten |
| `CAP_DEVICE_REMOVE` | Gerät kontrolliert entfernen |
| `CAP_DEVICE_ADMIN` | globale Geräterichtlinien verwalten |

Rechte müssen auf konkrete Geräte, Klassen oder Subtrees beschränkbar sein.

## 50. Device-Manager-API

```c
np_status_t np_device_query(
    np_device_handle_t device,
    uint32_t information_class,
    void* output_buffer,
    size_t output_size
);

np_status_t np_device_enumerate(
    np_device_handle_t parent,
    np_device_query_filter_t* filter,
    np_device_handle_t* devices,
    size_t* device_count
);

np_status_t np_device_start(
    np_device_handle_t device
);

np_status_t np_device_stop(
    np_device_handle_t device,
    uint32_t reason
);

np_status_t np_device_remove(
    np_device_handle_t device,
    uint32_t flags
);
```

Alle öffentlichen Strukturen verwenden Größen- und Versionsfelder.

## 51. Geräteereignisse

Der Device Manager veröffentlicht unter anderem:

```text
DEVICE_DISCOVERED
DEVICE_REGISTERED
DEVICE_DRIVER_BOUND
DEVICE_STARTED
DEVICE_STOPPED
DEVICE_SUSPENDED
DEVICE_RESUMED
DEVICE_REMOVAL_REQUESTED
DEVICE_SURPRISE_REMOVED
DEVICE_FAILED
DEVICE_RECOVERED
DEVICE_REMOVED
```

Empfänger sehen nur Ereignisse für Geräte, für die sie eine passende Sicht- oder Beobachtungscapability besitzen.

## 52. Audit

Sicherheitsrelevante Operationen können Auditereignisse erzeugen:

- Treiberbindung oder Treiberwechsel,
- Zugriff auf geschützte Geräte,
- MMIO- und DMA-Freigaben,
- Geräte-Reset,
- Power- und Wake-Konfiguration,
- manuelle Geräteisolierung,
- wiederholte Zugriffsverletzungen.

Audit bleibt standardmäßig lokal.

## 53. Diagnose

Der Device Manager stellt autorisierten Diagnosediensten bereit:

- Gerätehierarchie und Graphbeziehungen,
- Gerätezustand,
- gebundener Treiber,
- Ressourcenbelegung,
- Power State,
- Fehler- und Reset-Zähler,
- Hotplug-Ereignisse,
- aktive Sitzungen,
- Treiberneustarts,
- Start- und Bindungslatenzen.

Sensible Seriennummern oder Standortinformationen können ausgeblendet werden.

## 54. Statistik

```c
typedef struct np_device_manager_statistics {
    uint64_t registered_devices;
    uint64_t active_devices;
    uint64_t failed_devices;

    uint64_t driver_bindings;
    uint64_t binding_failures;
    uint64_t hotplug_additions;
    uint64_t hotplug_removals;

    uint64_t device_resets;
    uint64_t driver_restarts;
    uint64_t resource_conflicts;
} np_device_manager_statistics_t;
```

Statistiken sollen pro Geräteklasse, Bus und System verfügbar sein.

## 55. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_DEVICE` | Gerätehandle ist ungültig |
| `NP_ERR_INVALID_STATE` | Operation im aktuellen Zustand unzulässig |
| `NP_ERR_DEVICE_NOT_FOUND` | Gerät wurde nicht gefunden |
| `NP_ERR_DEVICE_REMOVED` | Gerät wurde entfernt |
| `NP_ERR_DEVICE_BUSY` | Gerät wird noch verwendet |
| `NP_ERR_DRIVER_NOT_FOUND` | kein geeigneter Treiber verfügbar |
| `NP_ERR_DRIVER_REJECTED` | Treiber wurde durch Sicherheitsrichtlinie abgelehnt |
| `NP_ERR_BINDING_FAILED` | Treiberbindung fehlgeschlagen |
| `NP_ERR_RESOURCE_CONFLICT` | Hardware-Ressource ist bereits belegt |
| `NP_ERR_DEPENDENCY_UNAVAILABLE` | erforderliches Gerät oder Backend fehlt |
| `NP_ERR_ACCESS_DENIED` | erforderliche Capability fehlt |
| `NP_ERR_TIMEOUT` | Geräteoperation lief in einen Timeout |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 56. Sicherheitsanforderungen

Der Device Manager muss sicherstellen, dass:

- Geräte nur über capability-geschützte Handles erreichbar sind,
- Treiber ausschließlich zugewiesene Ressourcen verwenden,
- MMIO-Bereiche nicht ausführbar sind,
- DMA keinen beliebigen Systemspeicher erreicht,
- Userspace-Treiber Interrupt-Controller nicht direkt programmieren,
- nicht vertrauenswürdige Firmware- und Gerätedaten validiert werden,
- entfernte Geräte keine gültigen Ressourcenmappings behalten,
- Treibersignatur und Sicherheitsprofil vor der Bindung geprüft werden,
- Geräte-Namespaces keine Capability-Prüfungen ersetzen,
- Treiberfehler möglichst auf Gerät und Treiberdomain begrenzt bleiben.

## 57. Datenschutz

Geräteeigenschaften können identifizierende Daten enthalten.

Dazu gehören:

- Seriennummern,
- Hardware-IDs,
- Funkkennungen,
- Standort- und Sensordaten,
- Benutzergerätebezeichnungen.

Daher gelten:

- unprivilegierte Anwendungen sehen nur erforderliche Eigenschaften,
- sensible Daten erfordern zusätzliche Rechte,
- Diagnose bleibt standardmäßig lokal,
- externe Geräte-Telemetrie ist standardmäßig deaktiviert,
- Audit- und Diagnosedaten müssen begrenzt aufbewahrt werden können.

## 58. Fehlerbehandlung

Ein Geräte- oder Userspace-Treiberfehler darf nicht automatisch eine Kernel Panic auslösen.

Folgende Situationen können einen schweren Kernelzustand darstellen:

- beschädigte Ressourcenverwaltung,
- unkontrollierte DMA-Zugriffe ohne wirksame Isolation,
- Ausfall eines nicht ersetzbaren Interrupt-Controllers,
- Verlust des einzigen Root-Dateisystemgeräts ohne Recovery-Pfad,
- Kernel-Treiber beschädigt kritische Kernelstrukturen,
- Geräteentfernung hinterlässt aktive MMIO- oder DMA-Zugriffe.

Je nach Auswirkung wird das Gerät isoliert, Recovery aktiviert oder die Kernel-Panic-Policy angewendet.

## 59. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Registrierung eines Plattformgeräts,
2. hierarchische Registrierung von Kindgeräten,
3. Enumeration eines Busses,
4. wiederholte idempotente Enumeration,
5. Treiber-Matching,
6. Auswahl des spezifischsten Treibers,
7. automatische Treiberbindung,
8. manuelle Treiberbindung,
9. Ablehnung eines nicht vertrauenswürdigen Treibers,
10. erfolgreicher Gerätestart,
11. Rollback eines fehlgeschlagenen Starts,
12. kontrollierter Gerätestopp,
13. Hotplug-Hinzufügen,
14. angekündigtes Entfernen,
15. Ablehnung eines belegten Geräts beim sicheren Entfernen,
16. überraschendes Entfernen,
17. Entfernung eines Elternobjekts mit Kindgeräten,
18. MMIO-Ressourcenzuweisung,
19. Erkennung eines Ressourcenkonflikts,
20. Interrupt-Zuordnung,
21. MSI-X-Zuordnung,
22. DMA-Domain-Zuordnung,
23. Isolation eines Userspace-Treibers,
24. Absturz und Neustart eines Userspace-Treibers,
25. Begrenzung wiederholter Neustarts,
26. Geräte-Reset,
27. Power Suspend und Resume,
28. Wake-fähiges Gerät,
29. CPU- und NUMA-Affinität,
30. Geräte-Namespaces und Sandbox-Sicht,
31. Prozessende mit offenen Gerätesitzungen,
32. Live-Treiberupdate mit Rollback,
33. Capability-Prüfung administrativer Operationen,
34. korrekte Audit- und Diagnosedaten,
35. vollständige Ressourcenfreigabe nach Geräteentfernung.

## 60. Verbindliche Invarianten

1. Jedes registrierte Gerät ist ein Objekt im Kernel Object Graph.
2. Jedes Geräteobjekt besitzt höchstens ein direktes Elternobjekt.
3. Ein Gerät im Zustand `REMOVED` kann nicht erneut gestartet werden.
4. Ein Gerät wird erst aktiviert, wenn Treiber und Ressourcen vollständig eingerichtet sind.
5. Hardware-Ressourcen werden nicht widersprüchlich exklusiv vergeben.
6. Treiber erhalten nur Capabilities für ihre zugewiesenen Geräte und Ressourcen.
7. MMIO-Abbildungen sind nicht ausführbar.
8. DMA wird ausschließlich über kontrollierte DMA-Objekte und Domains eingerichtet.
9. Interruptquellen werden vor Treiberstopp oder -entfernung maskiert.
10. Überraschend entfernte Geräte nehmen keine neuen Operationen an.
11. Ein Gerätefehler wird möglichst auf Gerät und Treiberdomain begrenzt.
12. Zustands- und Bindungsänderungen erfolgen unter geeigneter Synchronisation.

## 61. Referenzablauf einer Gerätebindung

```text
Gerät wird erkannt
  -> Geräteobjekt registrieren
  -> Eigenschaften und Ressourcenanforderungen erfassen
  -> passende Treiber ermitteln
  -> Signatur und Sicherheitsrichtlinie prüfen
  -> besten zulässigen Treiber auswählen
  -> Ressourcen reservieren
  -> Treiberdomain und Capabilities erzeugen
  -> Treiberinstanz starten
  -> Hardware initialisieren
  -> Interrupts und DMA aktivieren
  -> Gerät auf ACTIVE setzen
  -> Geräteereignis veröffentlichen
```

## 62. Zusammenfassung

Der NovaOS Device Manager verwaltet physische, virtuelle und logische Geräte als einheitliche Objekte innerhalb des Kernel Object Graph.

Geräte werden aus Firmware-, Bus- und Treiberinformationen erkannt, mit passenden verifizierten Treibern verbunden und über capability-geschützte Ressourcen betrieben. Userspace-Treiber werden bevorzugt isoliert und erhalten nur die für ihr Gerät erforderlichen MMIO-, Interrupt- und DMA-Objekte.

Plug-and-Play, Power Management, Ressourcenprüfung und begrenzte Recovery-Richtlinien ermöglichen das sichere Hinzufügen, Entfernen und Wiederherstellen von Geräten, ohne unnötig den gesamten Kernel zu gefährden.