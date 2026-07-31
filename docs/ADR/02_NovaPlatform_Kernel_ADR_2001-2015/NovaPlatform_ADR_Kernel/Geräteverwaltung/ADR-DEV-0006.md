# ADR-DEV-0006 – Device Tree

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0006 |
| Titel | Device Tree |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Gerätebeschreibung und Hardwaretopologie |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0004, ADR-DEV-0005, ADR-KERNEL-0100 |
| Nachfolgende Spezifikation | NPSPEC-DEV-DEVICE-TREE |

---

## 1. Kontext

NovaOS benötigt eine einheitliche Darstellung der im System vorhandenen Geräte und ihrer Beziehungen.

Hardwareinformationen können aus unterschiedlichen Quellen stammen:

- ACPI
- UEFI
- Flattened Device Tree
- PCI- und USB-Enumeration
- statischer Plattformbeschreibung
- virtuellen Maschinen
- Gerätetreibern
- Hotplug-Ereignissen

Diese Informationen müssen in ein gemeinsames, plattformunabhängiges Modell überführt werden.

---

## 2. Problemstellung

Die verschiedenen Hardwarequellen verwenden unterschiedliche Formate und Identifikatoren.

Ohne Normalisierung müssten Device Manager und Treiber direkt mit ACPI, FDT, PCI oder anderen Plattformformaten arbeiten. Dadurch würden plattformabhängige Details in die gesamte Treiberarchitektur gelangen.

NovaOS benötigt deshalb einen zentralen Device Tree, der:

- Geräte hierarchisch abbildet,
- Hardwareeigenschaften speichert,
- Abhängigkeiten darstellt,
- dynamisch erweitert werden kann,
- keine Firmwarestrukturen direkt an Treiber weitergibt,
- in den Kernel Object Graph integriert ist.

---

## 3. Entscheidung

NovaOS führt einen kernelverwalteten, dynamischen Device Tree als primäre Darstellung der Gerätehierarchie ein.

Der NovaOS Device Tree ist nicht identisch mit dem Flattened Device Tree einer Firmware. FDT, ACPI und Bus-Enumeration sind lediglich Eingabequellen.

Alle Eingabedaten werden validiert und in native NovaOS Device Nodes überführt.

---

## 4. Architektur

```text
ACPI   FDT   UEFI   Bus-Enumeratoren
  \     |      |          /
   \    |      |         /
    v   v      v        v
    Hardware-Normalisierung
              |
              v
       NovaOS Device Tree
              |
              v
         Device Manager
              |
              v
      Treiber und Dienste
```

Treiber arbeiten ausschließlich mit normalisierten Device Nodes und kontrollierten Ressourcen-Capabilities.

---

## 5. Baumstruktur

Der Device Tree besitzt einen eindeutigen Root Node.

Beispiel:

```text
system
├── firmware
├── cpus
├── memory
├── platform
│   ├── interrupt-controller
│   └── timer
├── pci
│   ├── display-controller
│   ├── network-controller
│   └── usb-controller
│       └── usb-hub
│           ├── keyboard
│           └── mouse
└── virtual
```

Jeder Node besitzt höchstens einen strukturellen Parent, darf aber zusätzliche Abhängigkeiten zu anderen Nodes referenzieren.

Damit bleibt die Grunddarstellung ein Baum, während komplexere Beziehungen im Kernel Object Graph abgebildet werden.

---

## 6. Device Node

Ein Device Node enthält mindestens:

```c
typedef struct device_node_info {
    uint64_t node_id;
    uint64_t parent_node_id;
    uint32_t node_type;
    uint32_t state;
    uint64_t generation;
    uint32_t property_count;
    uint32_t child_count;
} device_node_info_t;
```

Ein Node speichert:

- stabile logische Node-ID
- Parent-Child-Beziehung
- Gerätetyp
- Quelle der Gerätebeschreibung
- Eigenschaften
- Ressourcenbeschreibungen
- Kompatibilitätskennungen
- aktuellen Lebenszykluszustand
- Referenz zum zugehörigen Device Object

---

## 7. Eigenschaften

Eigenschaften werden als typisierte Schlüssel-Wert-Einträge gespeichert.

Unterstützte Datentypen sind mindestens:

- Integer
- Boolean
- String
- Bytefolge
- Integer-Array
- String-Array
- Node-Referenz
- Ressourcenbeschreibung

Beispiele:

```text
device.vendor-id       = 0x1234
device.product-id      = 0x5678
device.class           = "network"
device.compatible      = ["nova,net-v2", "generic,ethernet"]
device.hotplug         = true
power.wakeup-capable   = false
```

Eigenschaftsnamen verwenden versionierte Namespaces, um Konflikte zu vermeiden.

---

## 8. Eigenschaftsquellen und Priorität

Eigenschaften können aus mehreren Quellen stammen.

NovaOS verwendet folgende grundsätzliche Priorität:

1. sicherheitsrelevante Kernelrichtlinie
2. geprüfte NovaOS-Systemkonfiguration
3. dynamische Bus-Enumeration
4. ACPI oder FDT
5. übrige Firmwareinformationen
6. Treiberergänzungen
7. diagnostische Benutzerwerte

Eine Quelle mit niedriger Priorität darf sicherheitsrelevante Werte einer höheren Quelle nicht überschreiben.

Die Herkunft jeder kritischen Eigenschaft muss intern nachvollziehbar bleiben.

---

## 9. Firmware Device Tree

Auf Plattformen mit FDT übernimmt NovaOS den vom Bootloader übergebenen Flattened Device Tree.

Vor der Verwendung werden geprüft:

- Header und Version
- Gesamtgröße
- Offsets
- Stringtabellen
- Node-Tiefe
- Property-Längen
- Referenzen und Phandles
- Speicherreservierungen
- Überläufe und überlappende Bereiche

Der ursprüngliche FDT wird nur als Eingabedokument behandelt. Treiber erhalten keinen unkontrollierten Direktzugriff darauf.

---

## 10. ACPI-Integration

Auf ACPI-Systemen werden geeignete Informationen in Device Nodes übersetzt.

Dazu gehören beispielsweise:

- Gerätepfade
- Hardwarekennungen
- Ressourcenvorgaben
- Interruptzuordnungen
- Energiezustände
- Abhängigkeiten
- Hotplug-Fähigkeiten

ACPI-Methoden bleiben hinter einem kontrollierten Firmwaredienst verborgen.

Device Nodes dürfen keine direkt ausführbaren ACPI-Methoden enthalten.

---

## 11. Dynamische Änderungen

Der Device Tree ist nach dem Systemstart nicht unveränderlich.

Nodes können durch folgende Ereignisse hinzugefügt, geändert oder entfernt werden:

- Hotplug
- Bus-Enumeration
- Treibererkennung untergeordneter Geräte
- virtuelle Geräteerzeugung
- Firmwareereignisse
- Geräteausfall

Änderungen erfolgen transaktional. Andere Komponenten dürfen keinen teilweise aktualisierten Baumzustand sehen.

---

## 12. Generationen

Jeder Device Node und der gesamte Device Tree besitzen eine Generationsnummer.

Bei einer strukturellen Änderung wird die betreffende Generation erhöht.

Clients können dadurch erkennen, ob:

- ein zuvor gelesener Node verändert wurde,
- eine Auflistung wiederholt werden muss,
- ein Cache ungültig geworden ist,
- ein Gerät entfernt und neu erkannt wurde.

Eine Node-ID darf nach endgültiger Entfernung während derselben Systemgeneration nicht für ein anderes Gerät wiederverwendet werden.

---

## 13. Node-Referenzen

Abhängigkeiten außerhalb der Parent-Child-Struktur verwenden sichere Node-Referenzen.

Beispiele:

- Interruptcontroller
- Clock Provider
- Stromversorgung
- DMA-Controller
- IOMMU
- Reset Controller
- thermische Zone

Eine Referenz enthält keine Kerneladresse.

Wird ein referenzierter Node entfernt, werden abhängige Nodes benachrichtigt oder in einen nicht betriebsfähigen Zustand versetzt.

---

## 14. Device Tree und Device Objects

Device Node und Device Object erfüllen unterschiedliche Aufgaben:

| Element | Aufgabe |
|---|---|
| Device Node | Beschreibt Topologie und Hardwareeigenschaften |
| Device Object | Verwaltet Lebenszyklus, Treiber und Zugriff |
| Device Interface | Stellt eine konkrete Funktion bereit |

Ein Node kann bereits existieren, bevor ein passender Treiber gefunden wurde.

Anwendungen sollen primär Device Interfaces verwenden und nicht direkt von der Struktur des Device Tree abhängig sein.

---

## 15. API

Der Device Manager stellt kontrollierte Leseoperationen bereit:

```c
status_t device_tree_get_root(
    cap_handle_t tree_handle,
    cap_handle_t *node_handle
);

status_t device_tree_get_child(
    cap_handle_t node_handle,
    uint32_t index,
    cap_handle_t *child_handle
);

status_t device_tree_get_property(
    cap_handle_t node_handle,
    const char *property_name,
    void *value,
    uint32_t *value_size
);

status_t device_tree_get_parent(
    cap_handle_t node_handle,
    cap_handle_t *parent_handle
);
```

Schreibzugriffe stehen nur Device Manager, autorisierten Enumerator- und Plattformkomponenten zur Verfügung.

---

## 16. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `DEVICE_TREE_RIGHT_READ` | Node-Informationen lesen |
| `DEVICE_TREE_RIGHT_ENUMERATE` | untergeordnete Nodes auflisten |
| `DEVICE_TREE_RIGHT_PROPERTIES` | freigegebene Eigenschaften lesen |
| `DEVICE_TREE_RIGHT_CREATE` | neue Nodes erzeugen |
| `DEVICE_TREE_RIGHT_MODIFY` | Eigenschaften ändern |
| `DEVICE_TREE_RIGHT_REMOVE` | Nodes entfernen |
| `DEVICE_TREE_RIGHT_ADMIN` | Baumstruktur verwalten |

Der Zugriff auf einen Parent Node erteilt nicht automatisch Zugriff auf alle untergeordneten Nodes.

Sensible Eigenschaften benötigen zusätzliche Rechte oder werden vollständig verborgen.

---

## 17. Sicherheit

Für den Device Tree gelten folgende Regeln:

- Firmware- und Gerätedaten gelten als nicht vertrauenswürdig.
- Alle Größen, Referenzen und Zeichenketten werden validiert.
- Physische Adressen werden nicht allgemein an Anwendungen ausgegeben.
- Seriennummern und dauerhafte Gerätekennungen gelten als sensible Daten.
- Node-Namen erteilen keine Zugriffsrechte.
- Änderungen benötigen administrative Device-Tree-Capabilities.
- Treiber dürfen nur freigegebene Eigenschaften lesen.
- Firmwarewerte dürfen keine Kernelrichtlinien überschreiben.

Fehlerhafte Firmwaredaten dürfen nicht zu Speicherbeschädigungen oder Rechteerweiterungen führen.

---

## 18. Fehlerbehandlung

Ungültige Hardwarebeschreibungen können folgende Reaktionen auslösen:

- fehlerhafte Eigenschaft ignorieren,
- sicheren Standardwert verwenden,
- einzelnen Node deaktivieren,
- abhängige Nodes blockieren,
- alternative Erkennungsquelle verwenden,
- Systemstart bei kritischer Plattformhardware abbrechen.

Die Reaktion hängt von der Bedeutung des betroffenen Geräts ab.

Fehler in einem optionalen Gerät dürfen nicht den gesamten Device Tree unbrauchbar machen.

---

## 19. Audit und Diagnose

Diagnosewerkzeuge dürfen folgende Informationen anzeigen:

- Gerätehierarchie
- Node-Quelle
- Zustände
- Treiberzuordnung
- freigegebene Eigenschaften
- Abhängigkeiten
- Fehler und Konflikte
- Generationsnummern

Auditiert werden insbesondere:

- administrative Änderungen,
- manipulierte Firmwarestrukturen,
- ungültige Ressourcenbeschreibungen,
- Ersetzung sicherheitsrelevanter Eigenschaften,
- Erzeugung privilegierter virtueller Geräte.

Sensible Hardwarekennungen werden nur autorisierten Diagnosewerkzeugen angezeigt.

---

## 20. Alternative Ansätze

### 20.1 FDT als internes Universalformat

**Vorteile:**

- bestehendes und verbreitetes Format
- einfache Übernahme auf eingebetteten Plattformen

**Nachteile:**

- nicht für alle dynamischen NovaOS-Zustände geeignet
- Firmwareformat würde zur internen Kernel-ABI

**Entscheidung:** Abgelehnt. FDT bleibt eine Eingabequelle.

### 20.2 ACPI als einziges Hardwaremodell

**Vorteile:**

- gute Unterstützung verbreiteter PC-Hardware

**Nachteile:**

- ungeeignet für viele ARM-, RISC-V- und Embedded-Plattformen
- starke Plattformabhängigkeit

**Entscheidung:** Abgelehnt.

### 20.3 Vollständig flacher Gerätekatalog

**Vorteile:**

- einfache Auflistung

**Nachteile:**

- Bus- und Energieabhängigkeiten gehen verloren
- ungeeignet für Hotplug und zusammengesetzte Geräte

**Entscheidung:** Abgelehnt.

---

## 21. Konsequenzen

### Positive Konsequenzen

- plattformunabhängige Gerätebeschreibung
- gemeinsame Darstellung von ACPI, FDT und Bus-Enumeration
- klare Parent-Child-Beziehungen
- Unterstützung dynamischer Geräteänderungen
- Integration in den Kernel Object Graph
- kontrollierter Zugriff auf Hardwareeigenschaften

### Negative Konsequenzen

- zusätzliche Normalisierungsschicht
- komplexe Zusammenführung mehrerer Datenquellen
- Eigenschaften und Namespaces müssen langfristig gepflegt werden
- einige Firmwareinformationen lassen sich nicht verlustfrei vereinheitlichen

### Risiken

- widersprüchliche Eigenschaften aus mehreren Quellen
- ungültige Node-Referenzen
- Race Conditions bei Hotplug
- Informationslecks durch sensible Geräteeigenschaften
- falsche Abhängigkeitsmodelle

---

## 22. Implementierungsrichtlinien

- Firmwareformate werden niemals direkt zur öffentlichen Kernel-ABI.
- Alle Eingabedaten müssen vor der Node-Erzeugung validiert werden.
- Eigenschaften verwenden typisierte und versionierte Namen.
- Strukturelle Änderungen erfolgen transaktional.
- Node-IDs werden innerhalb einer Systemgeneration nicht wiederverwendet.
- Abhängigkeiten außerhalb der Baumstruktur verwenden sichere Referenzen.
- Device Nodes dürfen keine ausführbaren Firmwaremethoden enthalten.
- Der Device Tree beschreibt Geräte, erteilt aber keine Hardwarezugriffsrechte.

---

## 23. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- FDT- und ACPI-Import
- PCI- und USB-Erweiterungen
- fehlerhafte Größen und Offsets
- ungültige Node-Referenzen
- widersprüchliche Eigenschaften
- Hotplug und Entfernung
- Generationswechsel
- Parent-Child-Abhängigkeiten
- sensible Eigenschaften
- Capability-Rechte
- sehr tiefe oder große Gerätebäume
- parallele Auflistung und Änderung
- Systeme ohne ACPI oder FDT

---

## 24. Verbindliche Invarianten

1. Der NovaOS Device Tree ist ein natives, plattformunabhängiges Modell.
2. ACPI und FDT sind Eingabequellen und keine öffentliche Geräte-ABI.
3. Jeder Device Node besitzt höchstens einen strukturellen Parent.
4. Zusätzliche Abhängigkeiten werden durch sichere Node-Referenzen dargestellt.
5. Firmware- und Gerätedaten gelten als nicht vertrauenswürdig.
6. Änderungen am Device Tree erfolgen transaktional.
7. Node-IDs werden innerhalb einer Systemgeneration nicht wiederverwendet.
8. Node-Namen und Hardwarekennungen stellen keine Zugriffsberechtigung dar.
9. Device Nodes beschreiben Geräte, während Device Objects deren Lebenszyklus verwalten.
10. Physische Adressen und sensible Kennungen werden nur autorisiert offengelegt.

---

## 25. Zusammenfassung

NovaOS verwendet einen dynamischen, kernelverwalteten Device Tree als einheitliche Darstellung der Hardwaretopologie.

Informationen aus ACPI, Flattened Device Tree, Firmware und Bus-Enumeratoren werden validiert und in native Device Nodes überführt. Der Baum beschreibt Geräte, Eigenschaften und Abhängigkeiten, während Device Objects den eigentlichen Lebenszyklus und Treiberzugriff verwalten.

Dadurch bleibt die NovaOS-Gerätearchitektur plattformunabhängig, hotplugfähig und mit dem Capability-Modell vereinbar.