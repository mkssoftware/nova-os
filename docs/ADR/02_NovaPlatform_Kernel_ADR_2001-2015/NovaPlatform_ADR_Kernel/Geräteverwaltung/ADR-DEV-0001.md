# ADR-DEV-0001 – Device Manager

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0001 |
| Titel | Device Manager |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteverwaltung und Treiberarchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-KERNEL-0100, ADR-KERNEL-0101, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-IPC-0006 |
| Nachfolgende Spezifikation | NPSPEC-DEV-DEVICE-MANAGER |

---

## 1. Kontext

NovaOS benötigt eine zentrale Geräteverwaltung für physische, virtuelle und softwarebasierte Geräte.

Der Device Manager koordiniert:

- Geräteerkennung
- Treiberzuordnung
- Gerätelebenszyklus
- Ressourcenvergabe
- Gerätehierarchie
- Hotplug
- Energiezustände
- Fehler- und Statusmeldungen

Er bildet die Verbindung zwischen Hardwareerkennung, Treibern, Kernelobjekten und Systemdiensten.

---

## 2. Problemstellung

Geräte unterscheiden sich stark hinsichtlich Bus, Ressourcen, Energieverwaltung und Hotplug-Verhalten.

Ohne eine einheitliche Verwaltung würden Treiber eigene Mechanismen für Erkennung, Initialisierung und Entfernung implementieren. Das würde zu inkonsistenten Zuständen und schwer kontrollierbaren Ressourcenzugriffen führen.

NovaOS benötigt deshalb ein zentrales, ereignisgesteuertes Gerätemodell.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen Device Manager auf Grundlage des Kernel Object Graph.

Jedes erkannte Gerät wird durch ein `Device Object` repräsentiert. Das Objekt enthält Geräteidentität, Eigenschaften, Beziehungen, Ressourcen und den aktuellen Zustand.

Treiber greifen nicht direkt anhand globaler Gerätekennungen zu, sondern erhalten beschränkte Capabilities für die ihnen zugewiesenen Geräte und Ressourcen.

---

## 4. Architektur

```text
Bus-Enumerator
      |
      v
 Device Manager
      |
      +--> Device Object Graph
      |
      +--> Driver Matching
      |
      +--> Resource Allocation
      |
      +--> Device Events
```

Der Device Manager entscheidet nicht selbst über gerätespezifische Funktionen. Diese werden durch den gebundenen Treiber bereitgestellt.

---

## 5. Gerätehierarchie

Geräte werden als gerichteter Objektgraph dargestellt.

Beispiel:

```text
System
└── PCI-Bus
    ├── Grafikkarte
    ├── Netzwerkcontroller
    └── USB-Controller
        ├── USB-Hub
        │   ├── Tastatur
        │   └── Maus
        └── USB-Speicher
```

Jedes Gerät besitzt mindestens:

- eine Parent-Beziehung,
- null oder mehrere Child-Beziehungen,
- eine Buszugehörigkeit,
- einen optional gebundenen Treiber,
- eine Menge kontrollierter Ressourcen.

Zusätzliche Beziehungen dürfen für logische und zusammengesetzte Geräte verwendet werden.

---

## 6. Geräteidentität

Ein Gerät erhält eine systemweit eindeutige, nur für die aktuelle Systemgeneration gültige Objekt-ID.

```c
typedef struct device_identity {
    uint64_t device_id;
    uint32_t bus_type;
    uint32_t device_class;
    uint32_t vendor_id;
    uint32_t product_id;
    uint32_t revision;
} device_identity_t;
```

Permanente Gerätezuordnungen dürfen nicht allein auf der flüchtigen `device_id` basieren.

Dafür werden, sofern verfügbar, stabile Merkmale wie Seriennummer, Firmwarepfad oder Hardwarekennung verwendet.

---

## 7. Gerätezustände

Ein Gerät durchläuft definierte Zustände:

```text
DISCOVERED
    |
    v
MATCHING
    |
    v
STARTING
    |
    v
ACTIVE
    |
    +--> SUSPENDED
    |
    +--> FAILED
    |
    v
REMOVING
    |
    v
REMOVED
```

Zustandsübergänge werden ausschließlich durch den Device Manager koordiniert.

Treiber dürfen den Gerätezustand nicht eigenständig manipulieren.

---

## 8. Geräteerkennung

Geräte werden durch Bus-Enumeratoren erkannt.

Zu den vorgesehenen Enumeratorquellen gehören:

- ACPI
- PCI und PCI Express
- USB
- ATA, SATA und NVMe
- virtuelle Geräte
- Firmwaregeräte
- statisch konfigurierte Plattformgeräte
- softwarebasierte Pseudogeräte

Ein Enumerator meldet gefundene Geräte an den Device Manager und erzeugt sie nicht direkt als frei zugängliche Systemobjekte.

---

## 9. Treiberzuordnung

Der Device Manager wählt einen geeigneten Treiber anhand deklarativer Matching-Regeln.

Mögliche Kriterien sind:

- Bustyp
- Geräteklasse
- Hersteller- und Produkt-ID
- kompatible Hardwarekennungen
- Firmwarekennung
- Treiberversion
- Sicherheitsstufe
- Plattformarchitektur

Die Zuordnung erfolgt deterministisch. Bei mehreren Kandidaten gelten definierte Prioritäten.

Treiber dürfen Geräte nicht durch unkontrolliertes globales Scannen übernehmen.

---

## 10. Treiberbindung

Nach erfolgreichem Matching:

1. reserviert der Device Manager die erforderlichen Ressourcen,
2. startet oder kontaktiert den Treiberdienst,
3. übergibt eingeschränkte Geräte-Capabilities,
4. fordert die Initialisierung an,
5. veröffentlicht das Gerät erst nach erfolgreichem Start.

Schlägt die Initialisierung fehl, wird das Gerät als `FAILED` markiert oder ein alternativer Treiber versucht.

Ein fehlgeschlagener Treiber darf keine Ressourcenreferenzen zurücklassen.

---

## 11. Ressourcenverwaltung

Der Device Manager koordiniert mindestens:

- I/O-Port-Bereiche
- MMIO-Bereiche
- Interrupts
- DMA-Berechtigungen
- Busadressen
- Gerätespeicher
- Energieverwaltungsressourcen

Treiber erhalten nur Capabilities für tatsächlich zugewiesene Ressourcen.

Überlappende oder widersprüchliche Zuweisungen werden abgelehnt.

DMA-Ressourcen müssen zusätzlich die Regeln der NovaOS-DMA- und IOMMU-Architektur einhalten.

---

## 12. Device Objects

Ein Device Object stellt ein Gerät im einheitlichen NovaOS-Objektmodell dar.

```c
typedef struct device_info {
    device_identity_t identity;
    uint32_t state;
    uint32_t flags;
    uint64_t parent_device_id;
    uint64_t driver_id;
    uint64_t generation;
} device_info_t;
```

Das Objekt darf nur kontrollierte Metadaten veröffentlichen. Interne Kernelzeiger, physische Adressen und vertrauliche Firmwaredaten bleiben verborgen.

---

## 13. Geräteschnittstellen

Ein aktives Gerät kann eine oder mehrere funktionale Schnittstellen veröffentlichen.

Beispiele:

- Block Device
- Network Device
- Display Adapter
- Audio Device
- Input Device
- Sensor
- Serial Device
- Human Interface Device

Anwendungen öffnen bevorzugt eine funktionale Schnittstelle und nicht das rohe physische Gerät.

Geräteschnittstellen können über kontrollierte Service- oder Device-Namespaces auffindbar gemacht werden.

---

## 14. Capability-Rechte

Device-Capabilities können mindestens folgende Rechte enthalten:

| Recht | Bedeutung |
|---|---|
| `DEVICE_RIGHT_INSPECT` | Gerätemetadaten lesen |
| `DEVICE_RIGHT_CONTROL` | Kontrolloperationen ausführen |
| `DEVICE_RIGHT_CONFIGURE` | Gerätekonfiguration ändern |
| `DEVICE_RIGHT_POWER` | Energiezustand ändern |
| `DEVICE_RIGHT_DRIVER_BIND` | Treiber binden |
| `DEVICE_RIGHT_DRIVER_UNBIND` | Treiber lösen |
| `DEVICE_RIGHT_OPEN_INTERFACE` | Geräteschnittstelle öffnen |
| `DEVICE_RIGHT_ADMIN` | Administrative Geräteverwaltung |

Direkter MMIO-, Port- oder DMA-Zugriff wird nicht allein durch eine allgemeine Device-Capability gewährt.

---

## 15. Device-Manager-API

Die zentrale API umfasst mindestens:

```c
status_t device_enumerate(
    cap_handle_t namespace_handle,
    uint64_t cursor,
    device_info_t *entries,
    uint32_t capacity,
    uint32_t *entry_count
);

status_t device_get_info(
    cap_handle_t device_handle,
    device_info_t *info
);

status_t device_open_interface(
    cap_handle_t device_handle,
    const interface_id_t *interface_id,
    cap_handle_t *interface_handle
);

status_t device_request_state(
    cap_handle_t device_handle,
    uint32_t requested_state
);
```

Administrative Treiberbindung verwendet getrennte, besonders geschützte APIs.

---

## 16. Hotplug

Der Device Manager unterstützt das Hinzufügen und Entfernen von Geräten während des Betriebs.

### Hinzufügen

1. Bus-Enumerator erkennt das Gerät.
2. Device Object wird erzeugt.
3. Ressourcen werden ermittelt.
4. Treiber wird ausgewählt und gestartet.
5. Schnittstellen werden veröffentlicht.
6. Interessierte Dienste erhalten ein Event.

### Entfernen

1. Gerät wird als `REMOVING` markiert.
2. Neue Zugriffe werden abgelehnt.
3. Treiber und Clients werden benachrichtigt.
4. laufende Operationen werden beendet oder abgebrochen.
5. Ressourcen und Schnittstellen werden entfernt.
6. Gerät wechselt zu `REMOVED`.

Ein überraschend entferntes Gerät darf keine ungültigen Speicher- oder DMA-Zugriffe ermöglichen.

---

## 17. Energieverwaltung

Der Device Manager koordiniert Geräteenergiezustände mit dem Power Manager.

Unterstützte logische Zustände sind mindestens:

- `ACTIVE`
- `IDLE`
- `SUSPENDED`
- `OFF`

Ein Gerät darf nur ausgeschaltet werden, wenn:

- abhängige Geräte dies erlauben,
- keine kritische Operation läuft,
- der Treiber den Übergang bestätigt oder ein sicherer Zwangspfad existiert.

Abhängigkeiten im Device Object Graph bestimmen die Reihenfolge beim Suspendieren und Fortsetzen.

---

## 18. Fehlerbehandlung

Geräte- und Treiberfehler werden getrennt erfasst.

Mögliche Reaktionen sind:

- erneute Initialisierung
- Treiberneustart
- Geräte-Reset
- Rückfall auf einen alternativen Treiber
- Deaktivierung des Geräts
- Wechsel in den Zustand `FAILED`
- Information des Benutzers oder Administrators

Ein Fehler eines Userspace-Treibers soll das restliche System nicht unmittelbar gefährden.

Kernel-Treiber werden nur eingesetzt, wenn ihre Funktion oder Leistung dies erfordert.

---

## 19. Sicherheit und Audit

Für die Geräteverwaltung gelten folgende Regeln:

- Gerätezugriff erfolgt über Capabilities.
- Treiber erhalten nur erforderliche Ressourcen.
- Anwendungen dürfen keine rohen Hardwarezugriffe erhalten.
- Treiberpakete müssen vor dem Laden verifiziert werden.
- Geschützte Geräte benötigen zusätzliche Richtlinien.
- Geräteseriennummern und vergleichbare Identifikatoren gelten als sensible Metadaten.
- Ressourcenfreigabe muss auch nach Treiberabstürzen funktionieren.

Auditiert werden können:

- Treiberbindung und -lösung
- Aktivierung unsignierter Entwicklungstreiber
- Zugriff auf geschützte Geräte
- Geräte-Reset
- wiederholte Treiberabstürze
- administrative Konfigurationsänderungen

---

## 20. Ressourcenbegrenzung

Der Device Manager begrenzt:

- Anzahl registrierter Geräteobjekte
- Anzahl offener Geräteschnittstellen
- Anzahl ausstehender Geräteoperationen
- Treiberspeicher
- DMA-Ressourcen
- Ereignis- und Fehlerwarteschlangen

Bus-Enumeratoren dürfen durch fehlerhafte Geräteinformationen keine unbegrenzten Objektgraphen erzeugen.

---

## 21. Alternative Ansätze

### 21.1 Vollständig treibergesteuerte Geräteverwaltung

**Vorteile:**

- einfache zentrale Architektur
- große Freiheit für Treiber

**Nachteile:**

- inkonsistente Lebenszyklen
- Ressourcen- und Sicherheitskonflikte
- schlechte Hotplug-Koordination

**Entscheidung:** Abgelehnt.

### 21.2 Alle Treiber im Kernel

**Vorteile:**

- geringer IPC-Aufwand
- direkter Hardwarezugriff

**Nachteile:**

- große Kernel-Angriffsfläche
- Treiberfehler gefährden das Gesamtsystem

**Entscheidung:** Nur für zwingend kernelnah benötigte Treiber. Userspace-Treiber werden bevorzugt.

### 21.3 Geräte ausschließlich als Dateien

**Vorteile:**

- bekannte Unix-Abstraktion

**Nachteile:**

- unzureichend für komplexe Beziehungen und Fähigkeiten
- schwächere Integration in das Capability-Modell

**Entscheidung:** Kompatibilitätsschichten dürfen Gerätedateien anbieten; das native Modell verwendet Device Objects.

---

## 22. Konsequenzen

### Positive Konsequenzen

- einheitlicher Gerätelebenszyklus
- zentral koordinierte Ressourcenvergabe
- Unterstützung für Hotplug und Energieverwaltung
- capability-basierte Treiberrechte
- bessere Isolation von Userspace-Treibern
- konsistenter Gerätegraph

### Negative Konsequenzen

- zentrale Zustandsverwaltung ist komplex
- Treiber benötigen standardisierte Schnittstellen
- Kernel-/Userspace-Treiberkommunikation verursacht zusätzlichen Aufwand
- Geräteabhängigkeiten müssen korrekt modelliert werden

### Risiken

- fehlerhafte Treiberzuordnung
- Race Conditions bei Hotplug
- hängenbleibende Geräteoperationen
- Ressourcenlecks nach Treiberabstürzen
- falsche Suspend- und Resume-Reihenfolge

---

## 23. Implementierungsrichtlinien

- Jedes Gerät wird durch genau ein primäres Device Object repräsentiert.
- Zustandsübergänge werden durch den Device Manager koordiniert.
- Treiber dürfen ausschließlich zugewiesene Ressourcen verwenden.
- Geräte werden erst nach erfolgreicher Initialisierung veröffentlicht.
- Hot-Unplug muss alle neuen Zugriffe zuverlässig sperren.
- Treiberfehlerpfade müssen Ressourcen vollständig freigeben.
- Stabile Hardwarekennungen dürfen nicht als alleinige Zugriffsberechtigung dienen.
- Der Objektgraph muss auch bei teilweise fehlgeschlagenen Initialisierungen konsistent bleiben.

---

## 24. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Geräteerkennung
- Treiber-Matching
- erfolgreiche und fehlerhafte Treiberbindung
- Ressourcenreservierung und Konflikte
- Hotplug und überraschendes Entfernen
- Treiberabstürze
- Geräte-Reset
- Suspend und Resume
- Parent-Child-Abhängigkeiten
- Capability-Rechte
- mehrere passende Treiber
- fehlende Treiber
- parallele Geräteereignisse
- Ressourcenlimits

---

## 25. Verbindliche Invarianten

1. Jedes verwaltete Gerät besitzt ein Device Object.
2. Gerätezustandsübergänge werden zentral koordiniert.
3. Treiberzugriff erfolgt ausschließlich über zugewiesene Capabilities.
4. Geräte werden erst nach erfolgreicher Initialisierung veröffentlicht.
5. Ressourcen dürfen nicht widersprüchlich vergeben werden.
6. Anwendungen verwenden bevorzugt funktionale Geräteschnittstellen.
7. Hot-Unplug darf keine gültigen Hardwarezugriffe auf entfernte Geräte zurücklassen.
8. Prozess- und Geräte-IDs sind keine Zugriffsberechtigungen.
9. Treiberfehler dürfen den Device Object Graph nicht inkonsistent hinterlassen.
10. Sensible Geräteinformationen werden nur autorisiert offengelegt.

---

## 26. Zusammenfassung

NovaOS verwendet einen zentralen, ereignisgesteuerten Device Manager auf Grundlage des Kernel Object Graph.

Physische, virtuelle und softwarebasierte Geräte werden als capability-geschützte Device Objects dargestellt. Der Device Manager koordiniert Geräteerkennung, Treiberbindung, Ressourcen, Hotplug, Fehlerbehandlung und Energiezustände.

Userspace-Treiber werden bevorzugt, während kernelnahe Treiber auf zwingend erforderliche Fälle begrenzt bleiben.