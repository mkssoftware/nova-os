# ADR-DEV-0008 – USB Core

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0008 |
| Titel | USB Core |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteverwaltung und USB-Subsystem |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0004, ADR-DEV-0006, ADR-DEV-0007 |
| Nachfolgende Spezifikation | NPSPEC-DEV-USB-CORE |

---

## 1. Kontext

NovaOS benötigt ein zentrales USB-Subsystem für die Verwaltung von USB-Controllern, Hubs und angeschlossenen Geräten.

Über USB werden unter anderem folgende Geräte angebunden:

- Tastaturen und Mäuse
- Massenspeicher
- Audio- und Videogeräte
- Drucker
- Netzwerkadapter
- serielle Adapter
- Sensoren
- Smartphones
- Sicherheitstoken

Das USB-Subsystem muss dynamische Gerätewechsel, unterschiedliche Geschwindigkeiten und zusammengesetzte Geräte unterstützen.

---

## 2. Problemstellung

USB-Geräte werden zur Laufzeit verbunden und entfernt. Ihre Deskriptoren stammen direkt vom Gerät und können fehlerhaft oder manipuliert sein.

NovaOS muss daher:

- Host Controller einheitlich abstrahieren,
- Geräte sicher enumerieren,
- Deskriptoren vollständig validieren,
- Adressen und Endpunkte verwalten,
- passende Klassen- oder Herstellertreiber auswählen,
- Transfers und Bandbreite koordinieren,
- Hotplug und überraschendes Entfernen behandeln,
- USB-Geräte gegenüber Kernel und Anwendungen isolieren.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen USB Core als Teil des Driver Framework.

Der USB Core verwaltet die protokollunabhängige USB-Logik. Hardwareabhängige Host Controller Driver stellen eine einheitliche Schnittstelle für konkrete Controller bereit.

```text
Device Manager
      |
      v
   USB Core
      |
      +--> xHCI Driver
      +--> EHCI Driver
      +--> OHCI/UHCI Driver
      |
      v
 Hubs und USB-Geräte
```

USB-Klassentreiber verwenden die USB-Core-API und greifen nicht direkt auf Host-Controller-Register zu.

---

## 4. Unterstützte USB-Versionen

NovaOS sieht Unterstützung für folgende USB-Generationen vor:

| Standard | Geschwindigkeit |
|---|---|
| USB 1.x | Low Speed und Full Speed |
| USB 2.0 | High Speed |
| USB 3.x | SuperSpeed und höher |
| USB4 | Spätere gesonderte Erweiterung |

xHCI ist der bevorzugte Host-Controller-Standard für moderne Systeme.

Ältere Controller werden über EHCI, OHCI oder UHCI unterstützt, wenn entsprechende Treiber vorhanden sind.

---

## 5. USB-Objektmodell

Der USB Core bildet folgende Objekte ab:

- Host Controller
- Root Hub
- externer Hub
- USB Device
- Configuration
- Interface
- Endpoint
- Transfer

Ein physisches USB-Gerät kann mehrere Interfaces besitzen. Jedes Interface darf durch einen eigenen Funktionstreiber verwaltet werden.

Dadurch können zusammengesetzte Geräte ohne monolithischen Gesamttreiber unterstützt werden.

---

## 6. Gerätehierarchie

USB-Geräte werden in den NovaOS Device Tree integriert.

Beispiel:

```text
pci
└── xhci-controller
    └── root-hub
        ├── keyboard
        └── external-hub
            ├── mouse
            └── mass-storage
```

Die Parent-Child-Struktur beschreibt Controller, Ports, Hubs und angeschlossene Geräte.

Die maximale Hub-Tiefe und Gerätezahl werden begrenzt.

---

## 7. Enumeration

Nach Erkennung eines neuen Geräts führt der USB Core folgende Schritte aus:

1. Portänderung entprellen.
2. Port zurücksetzen.
3. Geschwindigkeit erkennen.
4. ersten Device Descriptor lesen.
5. eindeutige USB-Adresse vergeben.
6. vollständige Deskriptoren abrufen.
7. Deskriptoren validieren.
8. geeignete Konfiguration auswählen.
9. Interfaces als Device Objects erzeugen.
10. passende Treiber binden.
11. Gerät als verfügbar veröffentlichen.

Ein Gerät wird erst nach erfolgreicher Validierung und Konfiguration freigegeben.

---

## 8. Deskriptoren

Der USB Core verarbeitet mindestens:

- Device Descriptor
- Configuration Descriptor
- Interface Descriptor
- Endpoint Descriptor
- String Descriptor
- Binary Object Store Descriptor
- klassenspezifische Deskriptoren

Alle Deskriptoren gelten als nicht vertrauenswürdig.

Geprüft werden insbesondere:

- Typ und Mindestgröße
- Gesamtlänge
- Anzahl der Interfaces
- Anzahl der Endpunkte
- Verschachtelung
- Stringkodierung
- doppelte oder widersprüchliche Angaben
- Integer- und Offsetüberläufe

---

## 9. Treiber-Matching

Treiber können auf unterschiedlichen Ebenen gebunden werden:

| Ebene | Beispiel |
|---|---|
| Gerät | herstellerspezifisches Gesamtgerät |
| Interface | HID-, Audio- oder Mass-Storage-Interface |
| Klasse | generischer Klassentreiber |
| Hersteller/Produkt | gerätespezifischer Treiber |

Das Matching berücksichtigt:

- Vendor ID
- Product ID
- Device Class
- Interface Class
- Subclass
- Protocol
- Revision
- explizite Kompatibilitätsregeln

Ein gerätespezifischer Treiber wird nur bevorzugt, wenn er gültig signiert und mit der Hardware kompatibel ist.

---

## 10. Endpunkte und Transfertypen

USB unterstützt folgende Transfertypen:

| Transfertyp | Verwendung |
|---|---|
| Control | Konfiguration und Steuerung |
| Bulk | große, zuverlässige Datenmengen |
| Interrupt | kleine, regelmäßig geprüfte Daten |
| Isochronous | zeitkritische Audio- und Videodaten |

Endpoint 0 wird ausschließlich für Control Transfers verwendet.

Der USB Core prüft Endpunktrichtung, Typ, Paketgröße, Intervall und Geschwindigkeitsgrenzen.

---

## 11. Transfermodell

Treiber reichen Transfers asynchron ein:

```c
status_t usb_transfer_submit(
    cap_handle_t endpoint_handle,
    usb_transfer_t *transfer,
    cap_handle_t completion_event
);
```

Ein Transfer enthält mindestens:

```c
typedef struct usb_transfer {
    uint32_t transfer_type;
    uint32_t direction;
    uint64_t buffer_length;
    uint64_t timeout_ns;
    uint32_t flags;
} usb_transfer_t;
```

Der Abschluss wird über Event, Completion-Objekt oder Callback im Treiberkontext signalisiert.

Synchrone APIs dürfen als Wrapper über dem asynchronen Modell bereitgestellt werden.

---

## 12. Speicher und DMA

USB-Transferpuffer müssen für den jeweiligen Host Controller geeignet sein.

Der USB Core koordiniert:

- DMA-Mappings
- IOMMU-Zuordnung
- Scatter/Gather-Listen
- Cache-Synchronisation
- Bounce Buffer auf eingeschränkter Hardware
- Größen- und Ausrichtungsanforderungen

Treiber dürfen keine beliebigen physischen Speicheradressen an den USB-Controller übergeben.

Beim Entfernen eines Geräts oder Controllers werden aktive DMA-Mappings widerrufen.

---

## 13. Bandbreitenverwaltung

Periodische Transfers wie Interrupt und Isochronous benötigen reservierte Buszeit.

Der USB Core prüft vor der Aktivierung:

- verfügbare Busbandbreite
- Transferintervall
- Paketgröße
- Gerätegeschwindigkeit
- Hub- und Controllergrenzen
- bereits reservierte Transfers

Kann die notwendige Bandbreite nicht garantiert werden, wird der Endpoint nicht aktiviert.

Bulk Transfers verwenden verbleibende Bandbreite und erhalten keine feste Latenzgarantie.

---

## 14. Hubs

USB-Hubs werden durch einen standardisierten Hub Driver verwaltet.

Der Hub Driver übernimmt:

- Portstatus
- Portstromversorgung
- Anschluss- und Entfernungsereignisse
- Port-Reset
- Geschwindigkeitsmeldung
- Overcurrent-Erkennung
- selektives Suspendieren

Ein fehlerhafter Hub darf keine unbegrenzte Anzahl von Geräteereignissen oder Child Nodes erzeugen.

---

## 15. Hotplug und Entfernung

Beim Entfernen eines USB-Geräts:

1. wird das Gerät als `REMOVING` markiert,
2. werden neue Transfers abgelehnt,
3. werden ausstehende Transfers abgebrochen,
4. werden DMA-Mappings widerrufen,
5. werden Interface-Treiber gestoppt,
6. werden Endpunkt-Capabilities ungültig,
7. werden Device Nodes entfernt,
8. werden Clients benachrichtigt.

Ein erneut angeschlossenes Gerät wird als neue Geräteinstanz behandelt, auch wenn Hersteller- und Produktkennung identisch sind.

---

## 16. Energieverwaltung

Der USB Core unterstützt:

- selektives Suspendieren einzelner Geräte
- Portenergieverwaltung
- Remote Wake-up
- Controller-Suspend
- System-Suspend und Resume

Remote Wake-up wird nur aktiviert, wenn:

- das Gerät es unterstützt,
- der Treiber es anfordert,
- der Benutzer oder die Systemrichtlinie es erlaubt,
- der übergeordnete Hub und Controller es unterstützen.

Nach Resume werden Geräte- und Endpunktzustände erneut validiert.

---

## 17. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `USB_RIGHT_INSPECT` | freigegebene Geräteinformationen lesen |
| `USB_RIGHT_CONTROL` | erlaubte Control Transfers ausführen |
| `USB_RIGHT_TRANSFER_IN` | Daten vom Gerät empfangen |
| `USB_RIGHT_TRANSFER_OUT` | Daten an das Gerät senden |
| `USB_RIGHT_CONFIGURE` | Konfiguration oder Interface setzen |
| `USB_RIGHT_RESET` | Gerät oder Endpoint zurücksetzen |
| `USB_RIGHT_POWER` | Energiezustand anfordern |
| `USB_RIGHT_ADMIN` | USB-Infrastruktur verwalten |

Control Transfers mit sicherheitskritischen Standardanforderungen bleiben dem USB Core vorbehalten.

---

## 18. Sicherheitsrichtlinien

USB-Geräte gelten grundsätzlich als externe und nicht vertrauenswürdige Komponenten.

NovaOS kann Richtlinien anwenden für:

- erlaubte Geräteklassen
- Hersteller- und Produktkennungen
- einzelne Ports
- Benutzer und Sessions
- automatische Treiberbindung
- Massenspeicherzugriff
- Netzwerkadapter
- Eingabegeräte
- Debug- und Testgeräte

Neu angeschlossene Geräte dürfen nicht automatisch weitreichende Berechtigungen erhalten.

USB-Geräte-Firmware und Deskriptoren werden niemals allein aufgrund ihrer Herstellerangabe als vertrauenswürdig behandelt.

---

## 19. Fehlerbehandlung

Mögliche USB-Fehler sind:

| Fehler | Bedeutung |
|---|---|
| `USB_STALL` | Endpoint hat den Transfer angehalten |
| `USB_TIMEOUT` | Transfer wurde nicht rechtzeitig abgeschlossen |
| `USB_CRC_ERROR` | Übertragungsfehler |
| `USB_BABBLE` | Gerät sendet mehr Daten als zulässig |
| `USB_DISCONNECTED` | Gerät wurde entfernt |
| `USB_BANDWIDTH_EXCEEDED` | Keine ausreichende Busbandbreite |
| `USB_DESCRIPTOR_INVALID` | Deskriptor ist ungültig |
| `USB_CONTROLLER_FAILED` | Host Controller ist ausgefallen |

Je nach Fehler kann NovaOS einen Endpoint, Port, das Gerät oder den Controller zurücksetzen.

Wiederholte Fehler führen zur kontrollierten Deaktivierung des betroffenen Geräts.

---

## 20. Audit und Datenschutz

Audit-Ereignisse können erzeugt werden bei:

- Anschluss blockierter Geräteklassen
- ungültigen oder manipulierten Deskriptoren
- wiederholten USB-Protokollverletzungen
- Freigabe geschützter USB-Geräte
- Aktivierung von USB-Debugging
- unerwartetem Massenspeicher- oder Netzwerkgerät
- administrativem Geräte-Reset

Seriennummern und andere dauerhafte Gerätekennungen gelten als sensible Daten und werden nur bei technischer oder sicherheitsbezogener Notwendigkeit gespeichert.

USB-Nutzdaten werden nicht protokolliert.

---

## 21. Alternative Ansätze

### 21.1 Vollständige USB-Logik in jedem Host-Controller-Treiber

**Vorteile:**

- direkte Hardwareintegration

**Nachteile:**

- doppelte Implementierungen
- inkonsistentes Verhalten
- schwierige Unterstützung von Klassentreibern

**Entscheidung:** Abgelehnt. Der USB Core enthält die gemeinsame Protokolllogik.

### 21.2 USB-Treiber vollständig im Kernel

**Vorteile:**

- geringe IPC-Latenz

**Nachteile:**

- große Kernel-Angriffsfläche
- Deskriptor- und Gerätedefekte gefährden das System stärker

**Entscheidung:** Host Controller und notwendige Kernteile dürfen kernelnah sein; Klassen- und Funktionstreiber werden bevorzugt im Userspace ausgeführt.

### 21.3 Automatische Freigabe aller USB-Geräte

**Vorteile:**

- maximale Benutzerfreundlichkeit

**Nachteile:**

- hohes Risiko durch externe Geräte
- ungeprüfte Eingabe-, Netzwerk- und Speichergeräte

**Entscheidung:** Abgelehnt. Geräteklassen unterliegen Sicherheitsrichtlinien.

---

## 22. Konsequenzen

### Positive Konsequenzen

- einheitliche Unterstützung mehrerer Host Controller
- sichere Deskriptorvalidierung
- dynamische Geräte- und Interface-Erkennung
- Unterstützung für zusammengesetzte Geräte
- zentrale Bandbreiten- und Transferverwaltung
- kontrollierte Userspace-Treiber
- Integration in Plug and Play und Power Management

### Negative Konsequenzen

- komplexes Protokoll- und Zustandsmodell
- hoher Testaufwand mit fehlerhafter Hardware
- DMA- und Bandbreitenverwaltung benötigen enge Controllerintegration
- ältere Controller erfordern zusätzliche Treiber

### Risiken

- bösartige USB-Deskriptoren
- Race Conditions beim Entfernen
- aktive DMA-Zugriffe nach Disconnect
- Ereignisüberflutung durch defekte Hubs
- nicht eingehaltene isochrone Zeitvorgaben
- fehlerhafte Control Transfers

---

## 23. Implementierungsrichtlinien

- Alle USB-Deskriptoren werden als nicht vertrauenswürdig behandelt.
- Geräte werden erst nach erfolgreicher Enumeration veröffentlicht.
- Host Controller Driver implementieren ausschließlich die definierte USB-Core-Schnittstelle.
- Klassentreiber greifen nicht direkt auf Controllerregister zu.
- Control Transfers auf kritische Standardregister bleiben kontrolliert.
- Transfer- und DMA-Ressourcen werden beim Disconnect widerrufen.
- Hotplug-Verarbeitung erfolgt außerhalb des Interruptkontexts.
- USB-Adressen werden nach dem Entfernen nicht sofort für bestehende Referenzen wiederverwendet.
- Dauerhafte Gerätekennungen dürfen keine Zugriffsrechte erteilen.

---

## 24. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- xHCI und mindestens einen älteren Controller
- Low-, Full-, High- und SuperSpeed
- Geräte- und Interface-Matching
- zusammengesetzte Geräte
- Control-, Bulk-, Interrupt- und Isochronous-Transfers
- fehlerhafte Deskriptoren
- Hubs und mehrere Ebenen
- schnelles Ein- und Ausstecken
- Entfernung während aktiver Transfers
- DMA-Widerruf
- Bandbreitenüberschreitung
- Suspend und Resume
- blockierte Geräteklassen
- Controller-Reset und -Ausfall
- Fuzzing von USB-Deskriptoren

---

## 25. Verbindliche Invarianten

1. Der USB Core enthält die gemeinsame USB-Protokolllogik.
2. Host Controller Driver verwenden eine einheitliche Schnittstelle.
3. USB-Deskriptoren gelten immer als nicht vertrauenswürdig.
4. Geräte werden erst nach vollständiger Enumeration veröffentlicht.
5. Zusammengesetzte Geräte dürfen getrennte Interface-Treiber besitzen.
6. Treiber erhalten Endpunkte und Ressourcen ausschließlich über Capabilities.
7. USB-Geräte dürfen keine beliebigen physischen DMA-Adressen erhalten.
8. Beim Entfernen werden Transfers, Endpunkte und DMA-Mappings widerrufen.
9. Dauerhafte Gerätekennungen stellen keine Zugriffsberechtigung dar.
10. USB-Nutzdaten werden nicht standardmäßig protokolliert.

---

## 26. Zusammenfassung

NovaOS implementiert einen zentralen USB Core für Host Controller, Hubs, Geräte, Interfaces, Endpunkte und Transfers.

Hardwareabhängige Host Controller Driver werden von der gemeinsamen USB-Protokolllogik getrennt. Geräte werden erst nach sicherer Deskriptorvalidierung und erfolgreicher Enumeration in den Device Tree aufgenommen.

Capability-geschützte Endpunkte, kontrollierte DMA-Mappings und bevorzugt isolierte Userspace-Klassentreiber reduzieren die Risiken externer USB-Hardware.