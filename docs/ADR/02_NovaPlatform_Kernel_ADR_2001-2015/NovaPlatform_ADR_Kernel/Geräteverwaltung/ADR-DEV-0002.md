# ADR-DEV-0002 – Driver Framework

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0002 |
| Titel | Driver Framework |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Gerätetreiber und Hardwareabstraktion |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105, ADR-IPC-0006 |
| Nachfolgende Spezifikation | NPSPEC-DEV-DRIVER-FRAMEWORK |

---

## 1. Kontext

NovaOS benötigt ein einheitliches Framework für die Entwicklung, Registrierung und Ausführung von Gerätetreibern.

Treiber verbinden den Device Manager mit physischer Hardware, virtuellen Geräten und softwarebasierten Geräteschnittstellen.

Das Driver Framework soll:

- gemeinsame Lebenszyklen definieren,
- Treiber sicher an Geräte binden,
- Hardware-Ressourcen kontrolliert bereitstellen,
- Userspace- und Kernel-Treiber unterstützen,
- Fehlerisolierung ermöglichen,
- Treiberupdates und Neustarts vereinheitlichen.

---

## 2. Problemstellung

Treiber gehören zu den fehleranfälligsten und sicherheitskritischsten Systemkomponenten.

Ein fehlerhafter Treiber kann:

- Kernel-Speicher beschädigen,
- unzulässige DMA-Zugriffe ausführen,
- Interrupts blockieren,
- Hardware falsch konfigurieren,
- Ressourcen nicht freigeben,
- Suspend und Resume verhindern,
- das Gesamtsystem zum Absturz bringen.

NovaOS benötigt daher eine einheitliche und capability-basierte Treiberarchitektur.

---

## 3. Entscheidung

NovaOS implementiert ein gemeinsames Driver Framework mit zwei Ausführungsmodellen:

| Modell | Verwendung |
|---|---|
| Userspace Driver | Standardmodell für isolierbare Treiber |
| Kernel Driver | nur für zwingend kernelnahe Funktionen |

Beide Modelle verwenden dasselbe logische Geräte-, Ressourcen- und Lebenszyklusmodell.

Userspace-Treiber werden bevorzugt, sofern Hardwarezugriff, Latenz oder frühe Systeminitialisierung keinen Kernel-Treiber erfordern.

---

## 4. Architektur

```text
Device Manager
      |
      v
Driver Framework
      |
      +--> Userspace Driver Host
      |
      +--> Kernel Driver Module
      |
      v
Device Resources
```

Das Framework übernimmt:

- Treiberregistrierung,
- ABI-Prüfung,
- Treiber-Matching,
- Ressourcenübergabe,
- Lebenszyklusaufrufe,
- Fehlerüberwachung,
- Ereignisweiterleitung,
- Diagnoseintegration.

---

## 5. Treiberrollen

NovaOS unterscheidet folgende Treiberrollen:

| Rolle | Aufgabe |
|---|---|
| Bus Driver | erkennt untergeordnete Geräte |
| Function Driver | implementiert die Hauptfunktion eines Geräts |
| Class Driver | implementiert gemeinsame Geräteklassenlogik |
| Filter Driver | ergänzt oder überwacht einen Gerätepfad |
| Virtual Driver | stellt ein softwarebasiertes Gerät bereit |
| Platform Driver | verwaltet plattformspezifische Hardware |

Filtertreiber dürfen nur mit ausdrücklicher Berechtigung in einen Gerätepfad eingebunden werden.

---

## 6. Treibermanifest

Jeder Treiber besitzt ein signiertes Manifest.

Beispiel:

```toml
id = "nova.driver.usb.hid"
name = "NovaOS USB HID Driver"
version = "1.0.0"
abi_version = 1
execution_model = "userspace"
driver_role = "function"

[[matches]]
bus = "usb"
class = 3

[permissions]
interrupts = true
dma = false
mmio = false
io_ports = false
```

Das Manifest beschreibt mindestens:

- eindeutige Treiber-ID,
- Treiberversion,
- unterstützte Driver ABI,
- Ausführungsmodell,
- Treiberrolle,
- Matching-Regeln,
- benötigte Ressourcen,
- Sicherheitsberechtigungen,
- Abhängigkeiten,
- unterstützte Plattformen.

Treiber dürfen keine zusätzlichen Rechte erhalten, die nicht im Manifest deklariert und durch die Systemrichtlinie genehmigt wurden.

---

## 7. Treiber-Matching

Der Device Manager bewertet Treiber anhand deklarativer Matching-Regeln.

Mögliche Kriterien:

- Bustyp,
- Geräteklasse,
- Hersteller-ID,
- Produkt-ID,
- Revision,
- kompatible Hardwarekennung,
- Firmwarekennung,
- Plattformarchitektur.

Die Auswahl berücksichtigt:

1. spezifische Hardwareübereinstimmung,
2. kompatible Gerätekennung,
3. Klassentreiber,
4. Treiberpriorität,
5. ABI-Kompatibilität,
6. Signaturstatus,
7. Sicherheitsrichtlinie.

Treiber dürfen Geräte nicht durch unkontrolliertes globales Scannen übernehmen.

---

## 8. Driver ABI

Das Driver Framework verwendet eine versionierte ABI.

```c
typedef struct nova_driver_descriptor {
    uint32_t structure_size;
    uint32_t abi_version;
    const char *driver_id;
    const nova_driver_operations_t *operations;
} nova_driver_descriptor_t;
```

Jede öffentliche ABI-Struktur enthält mindestens:

- Strukturgröße,
- ABI-Version,
- reservierte Felder für Erweiterungen.

Eine inkompatible Änderung erfordert eine neue Major-Version.

Ein Treiber darf nur geladen werden, wenn seine ABI ausdrücklich unterstützt wird.

---

## 9. Treiberlebenszyklus

Ein Treiber unterstützt definierte Lebenszyklusoperationen:

```c
typedef struct nova_driver_operations {
    status_t (*initialize)(driver_context_t *context);
    status_t (*start_device)(device_handle_t device);
    status_t (*stop_device)(device_handle_t device);
    status_t (*remove_device)(device_handle_t device);
    status_t (*suspend_device)(device_handle_t device);
    status_t (*resume_device)(device_handle_t device);
    status_t (*reset_device)(device_handle_t device);
    void (*shutdown)(driver_context_t *context);
} nova_driver_operations_t;
```

Nicht unterstützte optionale Operationen werden eindeutig als solche gekennzeichnet.

Die Reihenfolge der Aufrufe wird durch Device Manager und Driver Framework festgelegt.

---

## 10. Treiberzustände

Ein Treiber durchläuft mindestens folgende Zustände:

```text
DISCOVERED
    |
    v
VALIDATING
    |
    v
LOADING
    |
    v
INITIALIZED
    |
    v
RUNNING
    |
    +--> SUSPENDED
    |
    +--> FAILED
    |
    v
STOPPING
    |
    v
UNLOADED
```

Zustandsübergänge erfolgen atomar.

Ein Treiber wird erst für Gerätebindungen verwendet, nachdem Signatur, ABI, Manifest und Initialisierung erfolgreich geprüft wurden.

---

## 11. Userspace-Treiber

Userspace-Treiber laufen in einem eingeschränkten Driver Host.

Der Driver Host erhält nur:

- Capabilities für zugewiesene Geräte,
- Capabilities für benötigte Ressourcen,
- RPC-Endpunkte zum Device Manager,
- definierte Diagnose- und Ereignisschnittstellen,
- begrenzte Speicher- und CPU-Ressourcen.

Ein Absturz des Driver Hosts darf den Kernel nicht unmittelbar beschädigen.

Mehrere Treiber dürfen nur dann denselben Driver Host verwenden, wenn ihre Sicherheits- und Stabilitätsanforderungen dies erlauben.

---

## 12. Kernel-Treiber

Kernel-Treiber laufen im privilegierten Kerneladressraum und besitzen ein höheres Schadenspotenzial.

Sie sind nur vorgesehen für:

- frühe Bootgeräte,
- Interrupt- und Timergrundlagen,
- Speicher- und Businfrastruktur,
- besonders latenzkritische Datenpfade,
- Funktionen, die technisch nicht sicher im Userspace umgesetzt werden können.

Kernel-Treiber müssen:

- gültig signiert sein,
- die genaue Kernel-ABI unterstützen,
- W^X und NX einhalten,
- alle Ressourcen registrieren,
- besonders streng geprüft werden.

Ein bloßer Performancevorteil reicht nicht automatisch als Begründung für einen Kernel-Treiber.

---

## 13. Ressourcenübergabe

Treiber erkennen Hardware-Ressourcen nicht durch unkontrolliertes Scannen.

Der Device Manager übergibt beschränkte Ressourcen-Capabilities für:

- MMIO-Bereiche,
- I/O-Ports,
- Interrupts,
- DMA-Kontexte,
- Buszugriffe,
- Gerätespeicher,
- Energieverwaltungsfunktionen.

Ressourcen dürfen nur innerhalb des zugewiesenen Bereichs verwendet werden.

Direkte physische Adressen sind keine allgemeine Zugriffsberechtigung.

---

## 14. MMIO und I/O-Ports

MMIO- und I/O-Port-Zugriffe erfolgen über eigene Ressourcencapabilities.

Das Framework prüft:

- Adressbereich,
- Länge,
- Zugriffsbreite,
- Ausrichtung,
- Lese- oder Schreibrecht,
- Cacheattribute,
- Gerätegeneration.

Treiber dürfen keine angrenzenden oder nicht zugewiesenen Registerbereiche verwenden.

MMIO-Mappings werden beim Stoppen oder Entfernen des Geräts widerrufen.

---

## 15. Interruptbehandlung

Interrupts werden über registrierte Interruptobjekte verwaltet.

Kernel-Treiber dürfen kurze Interrupt Handler ausführen. Umfangreiche Verarbeitung muss in Deferred Work verschoben werden.

Userspace-Treiber erhalten Interruptbenachrichtigungen über kontrollierte Events oder Interrupt-Capabilities.

Für alle Treiber gilt:

- Interruptpfade dürfen nicht blockieren,
- Interruptquellen müssen quittiert oder maskiert werden können,
- Interruptstürme müssen erkannt werden,
- entfernte Geräte dürfen keine aktiven Interruptbindungen behalten.

---

## 16. DMA

DMA ist ausschließlich über das NovaOS DMA Framework zulässig.

```c
status_t driver_dma_map(
    cap_handle_t dma_context,
    cap_handle_t memory_object,
    uint64_t offset,
    uint64_t length,
    uint32_t direction,
    dma_address_t *device_address
);
```

Treiber dürfen keine beliebigen physischen Speicherseiten an Geräte übergeben.

Wenn keine IOMMU vorhanden ist, verwendet NovaOS eingeschränkte DMA-Pools oder Bounce Buffer.

DMA-Mappings werden bei Treiberabsturz, Geräteentfernung oder Treiberstopp widerrufen.

---

## 17. Geräteschnittstellen

Ein Treiber veröffentlicht funktionale Schnittstellen über den Device Manager.

Beispiele:

- `BLOCK_DEVICE_INTERFACE`
- `NETWORK_DEVICE_INTERFACE`
- `DISPLAY_INTERFACE`
- `AUDIO_INTERFACE`
- `INPUT_INTERFACE`
- `SENSOR_INTERFACE`

Anwendungen greifen nicht direkt auf den Treiber zu.

Sie erhalten eine Capability für die benötigte Geräteschnittstelle. Dadurch kann die Treiberimplementierung ersetzt werden, ohne die öffentliche Geräteschnittstelle zu verändern.

---

## 18. Kommunikation

Userspace-Treiber kommunizieren über versionierte RPC-Schnittstellen.

Große Datenmengen können über Shared Memory übertragen werden.

Treiberkommunikation muss:

- Timeouts unterstützen,
- Abbruch ermöglichen,
- Capability-Rechte prüfen,
- Serverausfälle erkennen,
- ungültige Nachrichten ablehnen,
- Ressourcenlimits berücksichtigen.

Ein Treiber darf keine unbegrenzten synchronen RPC-Ketten erzeugen.

---

## 19. Parallelität

Das Driver Framework unterstützt parallele I/O-Operationen.

Jeder Treiber muss dokumentieren:

- thread-sichere Operationen,
- erlaubte Aufrufkontexte,
- interne Sperrregeln,
- Reihenfolgeabhängigkeiten,
- maximale Parallelität,
- Verhalten bei Stop und Remove.

Der Device Manager darf keine neue Operation an einen Treiber übergeben, der sich bereits in `STOPPING`, `FAILED` oder `UNLOADED` befindet.

---

## 20. Hotplug

Bei Hotplug koordiniert das Framework:

### Hinzufügen

1. Device Object erzeugen.
2. Treiber auswählen.
3. Ressourcen reservieren.
4. Treiber starten.
5. Geräteschnittstellen veröffentlichen.

### Entfernen

1. neue Operationen blockieren,
2. laufende I/O beenden oder abbrechen,
3. DMA und Interrupts widerrufen,
4. Geräteschnittstellen entfernen,
5. Treiberbindung lösen,
6. Ressourcen freigeben.

Überraschendes Entfernen darf keine weiteren Hardwarezugriffe ermöglichen.

---

## 21. Energieverwaltung

Treiber unterstützen die durch Power Manager und Device Manager koordinierten Zustandswechsel:

- Active
- Idle
- Light Sleep
- Deep Sleep
- Suspended
- Off

Treiber dürfen Geräte nicht unkoordiniert in einen für andere Subsysteme unsichtbaren Energiezustand versetzen.

Vor Suspend werden I/O, DMA und Interruptzustände stabilisiert.

Nach Resume muss der Treiber die Hardwareidentität und Konfiguration erneut prüfen.

---

## 22. Fehlerbehandlung und Neustart

Das Framework überwacht Userspace-Treiber auf:

- Prozessabsturz,
- nicht beantwortete Lebenszyklusaufrufe,
- wiederholte Timeouts,
- ungültige RPC-Nachrichten,
- übermäßigen Ressourcenverbrauch,
- Hardwarefehler.

Mögliche Reaktionen:

1. Treiberprozess beenden,
2. Ressourcen und DMA-Mappings widerrufen,
3. Gerät zurücksetzen,
4. Treiber neu starten,
5. Gerätezustand wiederherstellen,
6. Gerät bei wiederholtem Fehler deaktivieren.

Nicht jede Geräteoperation kann transparent wiederhergestellt werden. Betroffene Clients erhalten einen eindeutigen Fehler.

---

## 23. Neustartgrenzen

Automatische Treiberneustarts sind begrenzt.

Die Richtlinie berücksichtigt:

- Anzahl der Abstürze,
- Zeitfenster,
- Gerätekritikalität,
- Ursache,
- Wiederherstellbarkeit,
- Benutzer- und Systemmodus.

Wiederholt abstürzende Treiber werden deaktiviert, damit keine dauerhafte Crash-Schleife entsteht.

Bootkritische Treiber können stattdessen einen Recovery- oder sicheren Neustartpfad auslösen.

---

## 24. Treiberaktualisierung

Userspace-Treiber können grundsätzlich unabhängig vom Kernel aktualisiert werden.

Eine laufende Aktualisierung ist nur zulässig, wenn:

- Geräteschnittstellen kompatibel bleiben,
- der alte Treiber das Gerät sicher anhält,
- Ressourcen vollständig neu zugewiesen werden,
- laufende Operationen abgeschlossen oder kontrolliert abgebrochen werden,
- ein Rollback möglich ist.

Kernel-Treiberaktualisierungen benötigen in der ersten Implementierung grundsätzlich einen Neustart.

---

## 25. Treiberentladung

Ein Treiber darf nur entladen werden, wenn:

- keine Gerätebindung mehr besteht,
- keine aktiven I/O-Operationen laufen,
- keine Interrupt Handler aktiv sind,
- keine DMA-Mappings bestehen,
- keine Geräteschnittstellen referenziert werden,
- kein Deferred Work aussteht,
- alle internen Referenzen freigegeben wurden.

Kann dieser Zustand nicht sicher erreicht werden, wird die Entladung abgelehnt.

---

## 26. Sicherheit

Für das Driver Framework gelten folgende Regeln:

- Treiberpakete müssen authentisch und integritätsgeprüft sein.
- Treiber erhalten nur deklarierte Capabilities.
- Userspace-Treiber werden mit minimalen Rechten gestartet.
- DMA, MMIO, Ports und Interrupts benötigen getrennte Berechtigungen.
- Geräte- und Firmwaredaten gelten als nicht vertrauenswürdig.
- Treiber dürfen keine Kerneladressen oder fremden Prozessspeicher erhalten.
- nicht signierte Treiber bleiben im Produktivmodus blockiert.
- Entwicklungstreiber werden sichtbar als unsicherer Systemzustand markiert.

---

## 27. Ressourcenbegrenzung

Für jeden Treiber oder Driver Host gelten Limits für:

- Arbeitsspeicher,
- CPU-Zeit,
- Threads,
- offene Capabilities,
- DMA-Mappings,
- ausstehende I/O-Operationen,
- IPC-Nachrichten,
- Interruptfrequenz,
- Neustartversuche,
- Diagnoseereignisse.

Ein Treiber darf durch wartende Geräteoperationen keinen unbegrenzten Kernelspeicher reservieren.

---

## 28. Audit und Diagnose

Das Framework erfasst lokal:

- geladene Treiberversion,
- Ausführungsmodell,
- Treiberbindung und -lösung,
- Start- und Stoppfehler,
- Abstürze und Neustarts,
- Ressourcenverletzungen,
- Geräte-Resets,
- Verwendung von Entwicklungstreibern,
- ABI- und Signaturfehler.

Diagnosedaten dürfen keine unnötigen Geräteinhalte, privaten Schlüssel oder personenbezogenen Daten enthalten.

---

## 29. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_DRIVER_NOT_FOUND` | kein geeigneter Treiber verfügbar |
| `STATUS_DRIVER_ABI_MISMATCH` | Driver ABI ist inkompatibel |
| `STATUS_DRIVER_SIGNATURE_INVALID` | Signaturprüfung fehlgeschlagen |
| `STATUS_DRIVER_START_FAILED` | Treiber konnte nicht gestartet werden |
| `STATUS_DRIVER_STOP_FAILED` | Treiber konnte nicht sicher gestoppt werden |
| `STATUS_DRIVER_CRASHED` | Driver Host ist abgestürzt |
| `STATUS_DEVICE_REMOVED` | Gerät wurde entfernt |
| `STATUS_RESOURCE_CONFLICT` | benötigte Ressource ist nicht verfügbar |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_TIMEOUT` | Treiberoperation überschritt ihr Zeitlimit |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit wurde überschritten |

---

## 30. Alternative Ansätze

### 30.1 Ausschließlich Kernel-Treiber

**Vorteile:**

- geringer IPC-Aufwand,
- direkter Hardwarezugriff.

**Nachteile:**

- große Kernel-Angriffsfläche,
- jeder Treiberfehler kann das System gefährden.

**Entscheidung:** Abgelehnt.

### 30.2 Ausschließlich Userspace-Treiber

**Vorteile:**

- starke Isolation,
- einfache Neustarts und Updates.

**Nachteile:**

- ungeeignet für sehr frühe oder kernkritische Komponenten,
- zusätzlicher IPC-Aufwand.

**Entscheidung:** Userspace ist Standard, Kernel-Treiber bleiben für begründete Ausnahmen möglich.

### 30.3 Individuelle API pro Treiber

**Vorteile:**

- maximale Freiheit.

**Nachteile:**

- keine einheitliche Verwaltung,
- schwierige Diagnose,
- inkompatible Sicherheitsmodelle.

**Entscheidung:** Abgelehnt.

---

## 31. Konsequenzen

### Positive Konsequenzen

- einheitliches Treibermodell,
- stärkere Fehlerisolation,
- capability-basierter Hardwarezugriff,
- standardisierte Lebenszyklen,
- einfachere Diagnose und Neustarts,
- versionierbare Driver ABI,
- unabhängige Updates von Userspace-Treibern.

### Negative Konsequenzen

- zusätzlicher Aufwand für RPC und Driver Hosts,
- komplexe Ressourcenübergabe,
- zwei Ausführungsmodelle müssen unterstützt werden,
- nicht alle Geräte lassen sich transparent wiederherstellen.

### Risiken

- fehlerhafte Capability-Zuweisung,
- unzureichende DMA-Isolation,
- Race Conditions bei Hotplug,
- inkompatible ABI-Änderungen,
- Neustartschleifen,
- Performanceverluste in Userspace-Datenpfaden.

---

## 32. Implementierungsrichtlinien

- Userspace-Treiber sind das bevorzugte Ausführungsmodell.
- Kernel-Treiber benötigen eine dokumentierte technische Begründung.
- Jeder Treiber muss ein validiertes und signiertes Manifest besitzen.
- Lebenszyklusoperationen müssen Timeouts haben.
- Ressourcen werden ausschließlich durch den Device Manager vergeben.
- DMA-Mappings werden beim Stoppen oder Absturz widerrufen.
- Geräteschnittstellen bleiben von der Treiberimplementierung getrennt.
- Fehlerpfade müssen Ressourcen und Interruptbindungen vollständig freigeben.
- ABI-Strukturen müssen größen- und versionsgeprüft werden.
- Wiederholt fehlerhafte Treiber werden kontrolliert deaktiviert.

---

## 33. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Manifest- und Signaturprüfung,
- Treiber-Matching,
- Userspace- und Kernel-Treiber,
- Lebenszyklusoperationen,
- Ressourcen- und Capability-Rechte,
- MMIO- und Portbegrenzungen,
- Interruptweiterleitung,
- DMA-Mappings und Widerruf,
- Treiberabstürze und Neustarts,
- Hotplug während aktiver I/O,
- Suspend und Resume,
- Treiberupdates,
- ABI-Kompatibilität,
- Ressourcenlimits,
- fehlerhafte und bösartige Treiberantworten.

---

## 34. Verbindliche Invarianten

1. Jeder Treiber besitzt ein geprüftes und signiertes Manifest.
2. Userspace-Treiber sind das Standardmodell.
3. Kernel-Treiber werden auf technisch notwendige Fälle begrenzt.
4. Treiber erhalten ausschließlich zugewiesene Ressourcen-Capabilities.
5. Direkte physische Adressen stellen keine Berechtigung dar.
6. DMA wird nur über kontrollierte DMA-Mappings ausgeführt.
7. Lebenszyklusübergänge werden durch den Device Manager koordiniert.
8. Geräteschnittstellen bleiben von konkreten Treibern getrennt.
9. Treiberabstürze widerrufen Ressourcen-, Interrupt- und DMA-Zugriffe.
10. Nicht verifizierte Treiber werden nur in einem sichtbaren Entwicklungsmodus geladen.

---

## 35. Zusammenfassung

NovaOS verwendet ein einheitliches Driver Framework für Userspace- und Kernel-Treiber.

Userspace-Treiber werden wegen ihrer besseren Fehlerisolation bevorzugt. Kernel-Treiber bleiben auf frühe, kernkritische oder technisch besonders latenzabhängige Komponenten beschränkt.

Versionierte ABIs, signierte Manifeste, capability-geschützte Ressourcen und standardisierte Lebenszyklen bilden die Grundlage für sichere, austauschbare und diagnostizierbare Gerätetreiber.