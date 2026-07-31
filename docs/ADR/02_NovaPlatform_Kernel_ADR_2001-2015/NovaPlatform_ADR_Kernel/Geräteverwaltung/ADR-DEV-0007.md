# ADR-DEV-0007 – PCI Manager

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0007 |
| Titel | PCI Manager |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteverwaltung und PCI/PCIe |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0004, ADR-DEV-0006, ADR-MM-0014 |
| Nachfolgende Spezifikation | NPSPEC-DEV-PCI-MANAGER |

---

## 1. Kontext

PCI und PCI Express sind zentrale Bussysteme für moderne Computerhardware.

Über PCI beziehungsweise PCIe werden unter anderem folgende Geräte angebunden:

- Grafikadapter
- Netzwerkcontroller
- Speichercontroller
- USB-Controller
- Audiohardware
- NVMe-Geräte
- Erweiterungskarten
- virtuelle Geräte

NovaOS benötigt einen PCI Manager, der diese Geräte erkennt, konfiguriert und in den Device Tree integriert.

---

## 2. Problemstellung

PCI-Geräte stellen Konfigurationsregister, Speicherbereiche, Interrupts und optionale Fähigkeiten bereit. Fehlerhafte Konfiguration kann zu Ressourcenkonflikten, Speicherbeschädigungen oder unkontrolliertem DMA führen.

Der PCI Manager muss daher:

- PCI-Segmente, Busse und Geräte erkennen,
- Konfigurationsräume sicher lesen und schreiben,
- Bridges und untergeordnete Busse verwalten,
- BAR-Ressourcen erfassen und zuweisen,
- Interrupts konfigurieren,
- Treiber-Matching vorbereiten,
- DMA-Zugriff kontrollieren,
- Hotplug und Energiezustände unterstützen.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen PCI Manager als Bus-Enumerator des Device Managers.

Der PCI Manager erzeugt für jedes erkannte PCI-Gerät einen Device Node und ein zugehöriges Device Object.

Treiber erhalten keinen uneingeschränkten Zugriff auf den PCI-Konfigurationsraum. Zulässige Operationen erfolgen über eingeschränkte PCI- und Ressourcen-Capabilities.

---

## 4. Unterstützter Umfang

Die erste Implementierung unterstützt:

- PCI 2.x
- PCI Express
- PCI-to-PCI-Bridges
- mehrere PCI-Busse
- mehrere PCI-Segmente
- 32-Bit- und 64-Bit-BARs
- I/O-BARs
- Legacy-Interrupts
- MSI
- MSI-X
- PCIe Capability-Strukturen

Spätere Erweiterungen können PCI Hotplug, SR-IOV und erweiterte Fehlerdiagnose ergänzen.

---

## 5. Geräteadressierung

Ein PCI-Gerät wird logisch über folgende Adresse identifiziert:

```c
typedef struct pci_address {
    uint16_t segment;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
} pci_address_t;
```

Die Kurzform lautet:

```text
Segment:Bus:Device.Function
```

Beispiel:

```text
0000:02:00.0
```

Die PCI-Adresse beschreibt die aktuelle Bustopologie und ist keine dauerhafte Geräteidentität oder Zugriffsberechtigung.

---

## 6. Konfigurationsraum

NovaOS unterstützt zwei Zugriffsverfahren:

| Verfahren | Verwendung |
|---|---|
| Legacy Configuration Mechanism | ältere x86-PCs |
| PCIe ECAM | moderne PCIe-Systeme |

Die verfügbaren ECAM-Bereiche werden über ACPI MCFG oder eine andere vertrauenswürdige Plattformbeschreibung ermittelt.

Der PCI Manager prüft alle Segment-, Bus-, Geräte-, Funktions- und Offsetangaben vor dem Zugriff.

---

## 7. Enumeration

Die Enumeration erfolgt hierarchisch:

1. PCI-Segmente ermitteln.
2. Root-Busse untersuchen.
3. Geräte und Funktionen erkennen.
4. Multifunction-Geräte berücksichtigen.
5. Bridges identifizieren.
6. Untergeordnete Busse rekursiv untersuchen.
7. Hardwarekennungen und Capabilities erfassen.
8. Device Nodes erzeugen.
9. Ressourcen und Treiberzuordnung vorbereiten.

Rekursionstiefe und Anzahl erkannter Geräte werden begrenzt.

---

## 8. Gerätekennungen

Der PCI Manager erfasst mindestens:

```c
typedef struct pci_device_id {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t programming_interface;
    uint8_t revision;
} pci_device_id_t;
```

Diese Werte werden für das Treiber-Matching verwendet.

Hersteller- und Gerätekennungen stellen keine Sicherheits- oder Vertrauensentscheidung dar.

---

## 9. PCI-Bridges

PCI-Bridges werden als Parent Nodes ihrer untergeordneten Busse dargestellt.

Der PCI Manager verwaltet:

- primäre Busnummer
- sekundäre Busnummer
- untergeordnete Busnummer
- I/O-Fenster
- Speicherfenster
- Prefetchable-Memory-Fenster

Bridges werden vor ihren untergeordneten Geräten initialisiert und nach ihnen deaktiviert.

Ungültige oder zyklische Bustopologien werden abgelehnt.

---

## 10. Base Address Registers

BARs beschreiben die vom Gerät benötigten Ressourcen.

Der PCI Manager erkennt:

- I/O-Port-Bereiche
- 32-Bit-MMIO
- 64-Bit-MMIO
- prefetchable MMIO
- Expansion ROM BARs

Ein Treiber erhält nicht direkt den BAR-Wert, sondern eine Capability für den zugewiesenen Ressourcenbereich.

BAR-Größenbestimmung und Neuzuweisung erfolgen nur, wenn das Gerät sicher deaktiviert ist.

---

## 11. Ressourcenvergabe

Der PCI Manager koordiniert PCI-Ressourcen mit dem globalen Ressourcenmanager.

Dabei gelten folgende Regeln:

- Ressourcenbereiche dürfen sich nicht überschneiden.
- Firmwarezuweisungen werden validiert.
- Ungültige Zuweisungen können ersetzt werden.
- Bridge-Fenster müssen alle untergeordneten Ressourcen abdecken.
- Reservierte Plattformbereiche dürfen nicht verwendet werden.
- 64-Bit-BARs werden nur in erreichbare Adressräume gelegt.

Wenn keine sichere Zuweisung möglich ist, bleibt das Gerät deaktiviert.

---

## 12. Bus Mastering und DMA

Bus Mastering ist nach der Erkennung standardmäßig deaktiviert.

Es wird erst aktiviert, wenn:

- ein verifizierter Treiber gebunden ist,
- ein gültiger DMA-Kontext besteht,
- notwendige IOMMU- oder Bounce-Buffer-Regeln aktiv sind,
- das Gerät vollständig initialisiert wurde.

Beim Stoppen, Entfernen oder Absturz des Treibers wird Bus Mastering deaktiviert und der DMA-Kontext widerrufen.

---

## 13. Interrupts

Der PCI Manager unterstützt:

- Legacy INTx
- Message Signaled Interrupts
- MSI-X

MSI beziehungsweise MSI-X werden bevorzugt, wenn Gerät und Plattform sie sicher unterstützen.

Treiber erhalten Interrupt-Capabilities und keine unkontrollierten Interruptnummern.

Interruptzuordnungen werden bei Geräteentfernung oder Treiberabsturz vollständig gelöst.

---

## 14. PCI Capabilities

Der PCI Manager durchsucht die Standard- und Extended-Capability-Listen kontrolliert.

Mögliche Capabilities sind:

- Power Management
- MSI
- MSI-X
- PCI Express
- Advanced Error Reporting
- Virtual Channel
- SR-IOV
- Resizable BAR

Capability-Listen werden auf Schleifen, ungültige Offsets und fehlerhafte Längen geprüft.

Unbekannte Capabilities werden nicht verändert.

---

## 15. Treiberzugriff

Treiber können standardisierte PCI-Operationen über eine eingeschränkte Schnittstelle anfordern:

```c
status_t pci_config_read(
    cap_handle_t pci_device,
    uint16_t offset,
    uint8_t width,
    uint64_t *value
);

status_t pci_config_write(
    cap_handle_t pci_device,
    uint16_t offset,
    uint8_t width,
    uint64_t value
);
```

Schreibzugriffe werden durch eine Registerrichtlinie begrenzt.

Kritische Register wie BARs, Busnummern, Command Register und Interruptkonfiguration bleiben unter Kontrolle des PCI Managers.

---

## 16. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `PCI_RIGHT_INSPECT` | PCI-Metadaten lesen |
| `PCI_RIGHT_CONFIG_READ` | erlaubte Konfigurationsregister lesen |
| `PCI_RIGHT_CONFIG_WRITE` | erlaubte Register verändern |
| `PCI_RIGHT_MAP_BAR` | zugewiesene BAR-Ressourcen abbilden |
| `PCI_RIGHT_INTERRUPT` | Interruptressourcen verwenden |
| `PCI_RIGHT_DMA` | kontrollierten DMA-Kontext verwenden |
| `PCI_RIGHT_POWER` | Energiezustände anfordern |
| `PCI_RIGHT_ADMIN` | PCI-Infrastruktur verwalten |

Allgemeine Treiber erhalten kein `PCI_RIGHT_ADMIN`.

---

## 17. Energieverwaltung

PCI-Energiezustände werden in das allgemeine Power-Management-Modell übersetzt.

Unterstützte Hardwarezustände können sein:

- D0
- D1
- D2
- D3hot
- D3cold

Der Treiber fordert einen logischen Gerätezustand an. PCI Manager und Power Manager wählen gemeinsam den geeigneten Hardwarezustand.

Vor dem Wechsel müssen DMA und laufende Transaktionen sicher beendet werden.

---

## 18. Hotplug

PCIe-Hotplug wird über den allgemeinen Plug-and-Play-Lebenszyklus behandelt.

Beim Entfernen eines Geräts:

1. werden neue Zugriffe gesperrt,
2. wird Bus Mastering deaktiviert,
3. werden DMA-Mappings widerrufen,
4. werden Interrupts getrennt,
5. wird der Treiber gestoppt,
6. werden BAR-Mappings entfernt,
7. werden Device Node und Schnittstellen entfernt.

Überraschendes Entfernen darf keine weiteren Hardwarezugriffe ermöglichen.

---

## 19. Sicherheit

Für den PCI Manager gelten folgende Regeln:

- PCI-Konfigurationsdaten gelten als nicht vertrauenswürdig.
- Bus Mastering bleibt bis zur sicheren Treiberbindung deaktiviert.
- BARs dürfen keine reservierten Systembereiche überdecken.
- Treiber erhalten nur zugewiesene MMIO- und I/O-Ressourcen.
- PCI-Adressen und Kennungen stellen keine Berechtigung dar.
- Capability-Listen werden gegen Schleifen und ungültige Offsets geprüft.
- Expansion ROMs werden nicht automatisch ausgeführt.
- DMA benötigt einen kontrollierten DMA-Kontext.

---

## 20. Fehlerbehandlung

Mögliche Fehlerzustände sind:

| Status | Bedeutung |
|---|---|
| `PCI_DEVICE_UNAVAILABLE` | Gerät reagiert nicht |
| `PCI_RESOURCE_CONFLICT` | BAR oder Bridge-Fenster kollidiert |
| `PCI_CONFIG_INVALID` | Konfigurationsraum ist ungültig |
| `PCI_DMA_BLOCKED` | DMA konnte nicht sicher aktiviert werden |
| `PCI_INTERRUPT_FAILED` | Interruptkonfiguration fehlgeschlagen |
| `PCI_BRIDGE_INVALID` | Bridge-Konfiguration ist fehlerhaft |
| `PCI_DEVICE_REMOVED` | Gerät wurde entfernt |

Ein fehlerhaftes optionales PCI-Gerät darf nicht den gesamten PCI-Baum unbrauchbar machen.

---

## 21. Audit und Diagnose

Diagnosewerkzeuge dürfen autorisiert anzeigen:

- PCI-Adresse
- Hersteller- und Gerätekennung
- Geräteklasse
- Treiberzuordnung
- BAR-Größen und Typen
- Interruptmodus
- Energiezustand
- DMA- und IOMMU-Status
- Fehlerzustände

Auditiert werden insbesondere:

- Aktivierung von Bus Mastering
- Ressourcenkonflikte
- DMA-Verstöße
- administrative Konfigurationsänderungen
- wiederholte PCIe-Fehler
- überraschendes Entfernen

---

## 22. Alternative Ansätze

### 22.1 Direkter PCI-Zugriff durch Treiber

**Vorteile:**

- einfache und schnelle Treiberimplementierung

**Nachteile:**

- unkontrollierte Registeränderungen
- Ressourcen- und Sicherheitskonflikte
- schwache Treiberisolation

**Entscheidung:** Abgelehnt.

### 22.2 Firmwarezuweisungen immer unverändert übernehmen

**Vorteile:**

- geringer Initialisierungsaufwand

**Nachteile:**

- fehlerhafte oder überlappende Ressourcen bleiben bestehen
- ungeeignet für Hotplug und virtuelle Plattformen

**Entscheidung:** Firmwarewerte werden bevorzugt, aber immer validiert.

### 22.3 Nur Legacy-PCI unterstützen

**Vorteile:**

- einfachere erste Implementierung

**Nachteile:**

- unzureichend für moderne Hardware
- keine vollständige PCIe-Unterstützung

**Entscheidung:** Abgelehnt.

---

## 23. Konsequenzen

### Positive Konsequenzen

- zentrale und sichere PCI-Konfiguration
- Unterstützung für PCI und PCIe
- kontrollierte BAR-, Interrupt- und DMA-Ressourcen
- Integration in Device Tree und Plug and Play
- bessere Isolation von Treibern
- Unterstützung älterer und moderner x86-Systeme

### Negative Konsequenzen

- komplexe Bridge- und Ressourcenverwaltung
- hoher Testaufwand auf unterschiedlicher Hardware
- IOMMU-lose Systeme benötigen sichere Ersatzpfade
- einige Firmwarefehler erfordern Plattformausnahmen

### Risiken

- fehlerhafte BAR-Größenbestimmung
- Ressourcenüberlappungen
- DMA vor vollständiger Isolation
- ungültige Bridge-Topologien
- Race Conditions bei PCIe-Hotplug

---

## 24. Implementierungsrichtlinien

- Bus Mastering bleibt standardmäßig deaktiviert.
- Konfigurationsraumzugriffe werden zentral validiert.
- BAR-Größen werden nur bei deaktiviertem Gerät bestimmt.
- Firmwarezuweisungen werden vor Verwendung geprüft.
- Kritische PCI-Register bleiben unter Kontrolle des PCI Managers.
- Treiber erhalten Ressourcen ausschließlich als Capabilities.
- Capability-Listen müssen gegen Schleifen geschützt werden.
- Geräteentfernung widerruft Interrupt-, MMIO- und DMA-Zugriffe.

---

## 25. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Legacy-PCI- und ECAM-Zugriff
- mehrere Segmente und Busse
- Multifunction-Geräte
- PCI-Bridges
- 32-Bit- und 64-Bit-BARs
- Ressourcenüberlappungen
- MSI und MSI-X
- Bus Mastering und DMA-Widerruf
- fehlerhafte Capability-Listen
- Hotplug und überraschendes Entfernen
- Energiezustände
- ungültige Firmwarezuweisungen
- Geräte ohne passenden Treiber
- virtuelle PCI-Geräte

---

## 26. Verbindliche Invarianten

1. Der PCI Manager ist der zentrale PCI-Bus-Enumerator.
2. Jedes erkannte PCI-Gerät wird als Device Node und Device Object dargestellt.
3. PCI-Adressen und Hardwarekennungen erteilen keine Zugriffsrechte.
4. Bus Mastering bleibt bis zur sicheren Treiberbindung deaktiviert.
5. Treiber erhalten BARs, Interrupts und DMA ausschließlich über Capabilities.
6. Firmwarezuweisungen werden vor der Verwendung validiert.
7. Kritische Konfigurationsregister bleiben unter Kontrolle des PCI Managers.
8. Expansion ROMs werden nicht automatisch ausgeführt.
9. Geräteentfernung widerruft alle Hardwarezugriffe.
10. Fehler eines optionalen PCI-Geräts dürfen die übrige Bustopologie nicht zerstören.

---

## 27. Zusammenfassung

NovaOS verwendet einen zentralen PCI Manager zur Erkennung und Verwaltung von PCI- und PCIe-Geräten.

Der PCI Manager enumeriert Segmente, Busse, Funktionen und Bridges, koordiniert BAR-Ressourcen, Interrupts, DMA und Energiezustände und integriert alle Geräte in den Device Tree.

Treiber erhalten ausschließlich beschränkte Capabilities für die ihnen zugewiesenen Ressourcen. Dadurch bleibt PCI-Hardwarezugriff kontrolliert und mit dem Sicherheitsmodell von NovaOS vereinbar.