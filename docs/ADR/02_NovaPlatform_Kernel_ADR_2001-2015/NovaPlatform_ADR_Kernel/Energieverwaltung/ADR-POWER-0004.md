# ADR-POWER-0004 – Device Sleep

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-POWER-0004 |
| Titel | Device Sleep |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteenergieverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-POWER-0001, ADR-POWER-0002, ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0005, ADR-DEV-0009, ADR-DEV-0010 |
| Nachfolgende Spezifikation | NPSPEC-POWER-DEVICE-SLEEP |

---

## 1. Kontext

Viele Geräte können während des normalen Systembetriebs in einen energiesparenden Zustand versetzt werden, ohne das gesamte System zu suspendieren.

Typische Geräte sind:

- Netzwerkadapter
- USB-Geräte
- Massenspeicher
- Grafikadapter
- Audiohardware
- Kameras
- Sensoren
- Buscontroller
- virtuelle Geräte

Device Sleep ergänzt CPU Idle und System Suspend durch eine gerätespezifische Runtime-Energieverwaltung.

---

## 2. Problemstellung

Ein Gerät darf nur schlafen, wenn es nicht aktiv benötigt wird.

Vor dem Übergang müssen unter anderem berücksichtigt werden:

- laufende I/O-Operationen
- abhängige Geräte
- DMA-Zugriffe
- Interrupts
- Wake-up-Fähigkeit
- Wiederanlaufzeit
- Echtzeit- und Latenzanforderungen
- Shared Devices
- Treiber- und Firmwarefehler

Ein unkontrollierter Übergang kann Datenverlust, Geräteausfälle oder nicht mehr reagierende Hardware verursachen.

---

## 3. Entscheidung

NovaOS implementiert Device Sleep als zentral koordinierte Funktion des Power Managers und Device Managers.

Treiber melden die unterstützten Gerätezustände und führen die hardwarespezifischen Übergänge aus.

Die Entscheidung, ob und wann ein Gerät schlafen darf, trifft das Power-Management-Framework anhand von:

- Aktivitätszählern
- Referenzen
- I/O-Zustand
- Geräteabhängigkeiten
- Latenzgrenzen
- Energieprofil
- Wake-up-Fähigkeiten
- gemessenen Übergangskosten

Treiber dürfen Geräte nicht unkoordiniert in einen global unsichtbaren Schlafzustand versetzen.

---

## 4. Abgrenzung

| Mechanismus | Bedeutung |
|---|---|
| CPU Idle | Ruhezustand einer CPU ohne ausführbare Arbeit |
| Device Sleep | Ruhezustand eines einzelnen Geräts während des Betriebs |
| System Suspend | koordinierter Ruhezustand des gesamten Systems |
| Hibernate | persistentes Speichern der Systemsitzung |

Device Sleep kann unabhängig von einem System-Suspend stattfinden.

Während Suspend werden dieselben Treiberschnittstellen verwendet, aber durch eine globale Systemtransaktion koordiniert.

---

## 5. Gerätezustände

NovaOS verwendet folgende logische Zustände:

| Zustand | Bedeutung |
|---|---|
| `DEVICE_ACTIVE` | vollständig betriebsbereit |
| `DEVICE_IDLE` | aktiv, aber derzeit ohne Arbeit |
| `DEVICE_SLEEP_LIGHT` | geringe Einsparung, schneller Resume |
| `DEVICE_SLEEP_DEEP` | hohe Einsparung, längerer Resume |
| `DEVICE_OFF` | Gerät weitgehend oder vollständig abgeschaltet |
| `DEVICE_FAILED` | Zustand kann nicht sicher verwaltet werden |

Treiber übersetzen diese Zustände in geräte- oder busspezifische Hardwarezustände.

---

## 6. Zustandsbeschreibung

Ein Treiber meldet für jeden unterstützten Zustand:

```c
typedef struct device_sleep_state {
    uint32_t state_id;
    uint32_t flags;
    uint64_t entry_latency_ns;
    uint64_t exit_latency_ns;
    uint64_t minimum_residency_ns;
    uint64_t estimated_power_cost;
} device_sleep_state_t;
```

Zusätzlich werden angegeben:

- erhaltener Gerätekontext
- notwendige Wake-up-Quellen
- erforderlicher Reset nach Resume
- Auswirkungen auf Child Devices
- zulässige DMA- und Interruptzustände
- Plattform- oder Busabhängigkeiten

---

## 7. Zustandsmaschine

```text
ACTIVE
   |
   v
IDLE
   |
   v
SLEEP_PREPARING
   |
   v
SLEEPING
   |
   v
WAKING
   |
   v
ACTIVE
```

Ein fehlgeschlagener Übergang führt je nach Fehler zu:

- `ACTIVE`
- einem flacheren Sleep-Zustand
- `DEVICE_FAILED`

Zwischenzustände dürfen für Anwendungen nicht als normal nutzbares Gerät erscheinen.

---

## 8. Aktivitätsreferenzen

Jede Komponente, die ein aktives Gerät benötigt, hält eine Power Reference.

```c
status_t device_power_reference_acquire(
    cap_handle_t device_handle,
    uint32_t reason,
    cap_handle_t *reference_handle
);
```

Solange eine aktive Referenz besteht, darf das Gerät nicht unter den dafür zulässigen Zustand wechseln.

Referenzen besitzen:

- Besitzer
- Grund
- minimale Geräteleistung
- optionale Latenzgrenze
- Lebenszeit über die Capability
- optionalen Timeout

Verlorene Prozessreferenzen werden bei Prozessende automatisch freigegeben.

---

## 9. Idle-Erkennung

Ein Gerät gilt als inaktiv, wenn:

- keine I/O-Operation läuft,
- keine aktive Power Reference einen höheren Zustand verlangt,
- keine notwendige Timer- oder Wartungsoperation bevorsteht,
- abhängige Geräte einen Übergang erlauben,
- der Treiber keine interne Aktivität meldet.

Nach einer konfigurierbaren Idle-Zeit kann der Power Manager einen Sleep-Übergang anfordern.

Die Idle-Zeit wird dynamisch an Nutzungsmuster und Übergangskosten angepasst.

---

## 10. Auswahl des Sleep-Zustands

Der Power Manager berücksichtigt:

- erwartete Inaktivitätsdauer
- Entry- und Exit-Latenz
- Energieeinsparung
- nächsten bekannten Gerätezugriff
- Energieprofil
- Latenzanforderungen
- Parent- und Child-Abhängigkeiten
- gemessene Zuverlässigkeit des Zustands

Ein tiefer Zustand wird nur gewählt, wenn seine erwartete Einsparung die Übergangskosten rechtfertigt.

Die Prognose ist eine Optimierung und keine Garantie.

---

## 11. Sleep-Übergang

Vor dem Übergang:

1. werden neue I/O-Anfragen zurückgehalten,
2. werden laufende Operationen abgeschlossen oder abgebrochen,
3. werden Dirty-Daten synchronisiert,
4. werden DMA-Operationen beendet,
5. werden Interrupts angepasst,
6. wird der notwendige Gerätekontext gesichert,
7. werden Wake-up-Quellen konfiguriert,
8. wird der Hardwarezustand geändert.

Erst danach wird das Gerät als schlafend markiert.

---

## 12. Wake-up und Resume

Ein Gerät wird aufgeweckt durch:

- neue I/O-Anforderung
- Hardwareereignis
- Timer
- Parent-Gerät
- Systemdienst
- Benutzerinteraktion
- System-Resume

Der Ablauf ist:

1. Parent- und Ressourcenanbieter aktivieren,
2. Hardwareenergie wiederherstellen,
3. Gerät identifizieren,
4. Register- und Treiberzustand wiederherstellen,
5. DMA und Interrupts neu konfigurieren,
6. Gerät auf Funktionsfähigkeit prüfen,
7. wartende I/O-Anfragen freigeben.

I/O wird erst fortgesetzt, wenn das Gerät wieder vollständig nutzbar ist.

---

## 13. Geräteabhängigkeiten

Die Reihenfolge folgt dem Device Object Graph.

Beim Einschlafen:

```text
Child Devices vor Parent Devices
Verbraucher vor Ressourcenanbieter
```

Beim Aufwachen:

```text
Parent Devices vor Child Devices
Ressourcenanbieter vor Verbrauchern
```

Ein Parent-Gerät darf keinen Zustand wählen, der aktive Child Devices funktionsunfähig macht.

Zyklische Energieabhängigkeiten werden abgelehnt.

---

## 14. DMA

Vor Device Sleep müssen unzulässige DMA-Zugriffe beendet werden.

NovaOS:

- wartet auf bekannte Transfers,
- synchronisiert DMA-Puffer,
- widerruft temporäre Mappings,
- deaktiviert Bus Mastering, wenn erforderlich,
- aktualisiert IOMMU-Regeln,
- bereinigt Bounce Buffer.

Ein schlafendes oder ausgeschaltetes Gerät darf keinen beliebigen Zugriff auf Systemspeicher behalten.

Für Wake-on-Device notwendige DMA-Bereiche benötigen eine gesonderte, enge Freigabe.

---

## 15. Interrupts und Wake-up

Interrupts werden abhängig vom Zustand:

- maskiert,
- auf Wake-up-Ereignisse begrenzt,
- an einen Plattformcontroller umgeleitet,
- beim Resume neu gebunden.

Ein Gerät darf nur dann als Wake-up-Quelle dienen, wenn:

- Hardware und Bus dies unterstützen,
- der Treiber es korrekt implementiert,
- die Systemrichtlinie es erlaubt,
- die Quelle gegen Ereignisfluten geschützt ist.

Ein unerwarteter Interrupt eines schlafenden Geräts wird diagnostisch erfasst und sicher behandelt.

---

## 16. I/O während Device Sleep

Neue I/O-Anfragen können:

- den Wake-up auslösen,
- bis zum Resume warten,
- bei `NONBLOCK` mit `STATUS_WOULD_BLOCK` zurückkehren,
- bei Timeout fehlschlagen,
- bei Gerätefehler abgebrochen werden.

Die API darf eine Wake-up-Latenz nicht als gewöhnliche Gerätelatenz verbergen, wenn die Anwendung eine ausdrückliche Deadline gesetzt hat.

Zeitkritische Anwendungen können über Power References einen geeigneten Mindestzustand anfordern.

---

## 17. Busintegration

Busse besitzen eigene Sleep-Regeln.

Beispiele:

- PCIe Power Management und Link States
- USB Selective Suspend
- SATA- und NVMe-Power States
- ACPI-Gerätezustände
- virtuelle Gerätepausen

Der jeweilige Bus Manager übersetzt die logische NovaOS-Anforderung in sichere Busoperationen.

Öffentliche Treiber-APIs bleiben von ACPI-, PCIe- oder USB-spezifischen Zustandsnummern unabhängig.

---

## 18. Netzwerkgeräte

Netzwerkadapter können zusätzliche Funktionen unterstützen:

- Wake-on-LAN
- Mustererkennung
- Linküberwachung
- reduzierte Linkgeschwindigkeit
- energiesparendes Ethernet

Wake-on-LAN ist standardmäßig deaktiviert und benötigt eine ausdrückliche Freigabe.

Die Aktivierung darf nur die erforderlichen Empfangsmuster und Speicherbereiche freigeben.

Nach Resume werden Link, Adressen, Routen und Netzwerkzustände erneut geprüft.

---

## 19. Speichergeräte

Bei Speichergeräten gelten besonders strenge Regeln.

Vor Device Sleep:

- werden Schreibcaches entsprechend ihrer Garantie synchronisiert,
- werden Journaling- und Block-I/O-Abhängigkeiten beachtet,
- werden ausstehende Befehle abgeschlossen,
- wird der Medienzustand geprüft.

Ein Gerät darf nicht schlafen, wenn dadurch bestätigte, aber noch nicht persistierte Daten verloren gehen könnten.

Tiefe Sleep-Zustände mit Spin-up- oder Resume-Latenz werden bei interaktiver Nutzung zurückhaltend gewählt.

---

## 20. Grafik- und Audiogeräte

Grafik- und Audiogeräte können Teilbereiche unabhängig abschalten.

Das Framework darf dafür Subdevices oder Power Domains verwenden.

Beispiele:

- Display Engine
- Video Decoder
- Audio Codec
- GPU Compute Engine
- externe Displayausgänge

Ein aktiver Audio- oder Displaypfad darf nicht durch eine falsche Gesamtgeräteentscheidung unterbrochen werden.

Teilzustände bleiben unter Kontrolle des zuständigen Treibers, werden aber dem Power Manager gemeldet.

---

## 21. Fehlerbehandlung

Ein Sleep-Zustand wird deaktiviert, wenn:

- das Gerät nicht zuverlässig aufwacht,
- Daten oder Registerzustände verloren gehen,
- Wake-up-Ereignisse fehlen,
- DMA nach dem Übergang aktiv bleibt,
- wiederholt Timeouts auftreten,
- der Treiber inkonsistente Zustände meldet.

NovaOS fällt auf einen flacheren Zustand oder `DEVICE_ACTIVE` zurück.

Nicht kritische Geräte können bei dauerhaftem Fehler deaktiviert werden.

---

## 22. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `DEVICE_SLEEP_RIGHT_REFERENCE` | Power Reference halten |
| `DEVICE_SLEEP_RIGHT_WAKE` | Gerät aufwecken |
| `DEVICE_SLEEP_RIGHT_REQUEST` | Sleep-Zustand anfordern |
| `DEVICE_SLEEP_RIGHT_WAKE_SOURCE` | Wake-up-Funktion konfigurieren |
| `DEVICE_SLEEP_RIGHT_INSPECT` | Zustände und Statistiken lesen |
| `DEVICE_SLEEP_RIGHT_POLICY` | Richtlinien verändern |
| `DEVICE_SLEEP_RIGHT_ADMIN` | Device-Sleep-Subsystem verwalten |

Gewöhnliche Anwendungen können nur für ihnen zugewiesene Geräte begrenzte Power References anfordern.

---

## 23. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Power References
- Wake-up-Anforderungen
- Übergangsversuche
- ausstehende I/O während Wake-up
- Wiederholungen nach Fehlern
- Diagnoseereignisse
- reservierte Wake-DMA-Bereiche

Ein Prozess darf durch vergessene oder absichtlich gehaltene Referenzen nicht unbegrenzt alle Geräte aktiv halten.

Richtlinien können lang laufende Referenzen herabstufen oder ablehnen.

---

## 24. Sicherheit und Datenschutz

Für Device Sleep gelten folgende Regeln:

- Zustandsänderungen erfolgen nur über Capabilities.
- Treiber dürfen Hardwarezustände nicht am Device Manager vorbei verändern.
- DMA wird vor tiefen Zuständen beendet oder streng begrenzt.
- Wake-up-Quellen werden ausdrücklich autorisiert.
- sicherheitskritische Geräte können eigene Mindestzustände erzwingen.
- Geräteidentität wird nach Resume erneut geprüft.
- Power References sind besitzer- und lebenszeitgebunden.
- Nutzungsverhalten wird nicht als externe Telemetrie übertragen.

---

## 25. Diagnose

NovaOS erfasst lokal:

- Zeit pro Gerätezustand
- Anzahl der Sleep- und Wake-Übergänge
- durchschnittliche Wake-up-Latenz
- fehlgeschlagene Übergänge
- aktive Power References
- Wake-up-Ursachen
- deaktivierte Sleep-Zustände
- Energiezustand der Geräteabhängigkeiten

Detaillierte Anwendungsnutzung wird unprivilegierten Diagnoseaufrufern nicht offengelegt.

---

## 26. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Übergang erfolgreich |
| `STATUS_DEVICE_BUSY` | Gerät besitzt aktive Arbeit oder Referenzen |
| `STATUS_DEVICE_SLEEPING` | Gerät befindet sich im Sleep-Zustand |
| `STATUS_WAKE_IN_PROGRESS` | Gerät wird bereits aufgeweckt |
| `STATUS_WAKE_FAILED` | Gerät konnte nicht reaktiviert werden |
| `STATUS_DEPENDENCY_ACTIVE` | abhängiges Gerät verhindert den Übergang |
| `STATUS_DMA_ACTIVE` | DMA konnte nicht beendet werden |
| `STATUS_LATENCY_CONSTRAINT` | Zustand verletzt eine Latenzanforderung |
| `STATUS_NOT_SUPPORTED` | Zustand wird nicht unterstützt |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_TIMEOUT` | Übergang überschritt das Zeitlimit |

---

## 27. Alternative Ansätze

### 27.1 Treiber verwalten Device Sleep vollständig selbst

**Vorteile:**

- geringer zentraler Aufwand
- genaue Hardwarekenntnis im Treiber

**Nachteile:**

- fehlende Abhängigkeitskoordination
- inkonsistente Zustände
- unkontrollierte DMA- und Wake-up-Risiken

**Entscheidung:** Abgelehnt.

### 27.2 Geräte nur während System-Suspend schlafen lassen

**Vorteile:**

- einfacheres Laufzeitmodell

**Nachteile:**

- unnötiger Energieverbrauch während normaler Nutzung

**Entscheidung:** Abgelehnt. Runtime Device Sleep wird unterstützt.

### 27.3 Immer den tiefsten Zustand wählen

**Vorteile:**

- maximale theoretische Energieeinsparung

**Nachteile:**

- hohe Latenz und zusätzlicher Verschleiß
- häufige Fehlentscheidungen bei kurzer Inaktivität

**Entscheidung:** Zustände werden dynamisch nach Kosten und erwarteter Dauer gewählt.

---

## 28. Konsequenzen

### Positive Konsequenzen

- geringerer Energieverbrauch im normalen Betrieb
- weniger Wärmeentwicklung
- längere Akkulaufzeit
- koordinierte Bus- und Geräteabhängigkeiten
- kontrollierte DMA- und Interruptzustände
- dynamische Reaktion auf Nutzung und Latenzanforderungen
- einheitliche API für unterschiedliche Hardware

### Negative Konsequenzen

- zusätzlicher Aufwand für jeden Treiber
- Wake-up erhöht die erste I/O-Latenz
- fehlerhafte Firmware kann Zustände unbrauchbar machen
- Abhängigkeitsgraph und Teilgeräte erhöhen die Komplexität

### Risiken

- Datenverlust bei Speichergeräten
- DMA während des Sleep-Zustands
- verlorene Wake-up-Ereignisse
- häufiges Ein- und Ausschalten
- fehlerhafte Abhängigkeitsreihenfolge
- dauerhaft gehaltene Power References

---

## 29. Implementierungsrichtlinien

- Device Sleep wird zentral durch Power Manager und Device Manager koordiniert.
- Treiber melden Zustände und führen ausschließlich angeordnete Übergänge aus.
- Aktive Power References verhindern unzulässige Zustände.
- Child-Geräte schlafen vor ihren Parent-Geräten.
- Parent-Geräte wachen vor ihren Child-Geräten auf.
- Laufende I/O und DMA werden vor dem Übergang kontrolliert beendet.
- Wake-up-Quellen benötigen ausdrückliche Freigaben.
- Geräteidentität und Funktionsfähigkeit werden nach Wake-up geprüft.
- Fehlgeschlagene Zustände können dynamisch deaktiviert werden.
- Jede Übergangsphase besitzt einen Timeout.

---

## 30. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- alle logischen Gerätezustände
- Power References
- Idle-Erkennung
- Sleep- und Wake-up-Latenzen
- Parent-Child-Abhängigkeiten
- I/O während Sleep und Wake-up
- DMA-Widerruf
- Interrupt- und Wake-up-Quellen
- PCIe-, USB- und Speichergeräte
- Netzwerk-, Grafik- und Audiogeräte
- Geräteentfernung während eines Übergangs
- Treiberabsturz
- wiederholte Sleep-/Wake-Zyklen
- fehlerhafte Firmwarezustände
- Ressourcenlimits

---

## 31. Verbindliche Invarianten

1. Device Sleep wird durch Power Manager und Device Manager koordiniert.
2. Treiber dürfen globale Gerätezustände nicht unkontrolliert verändern.
3. Aktive Power References begrenzen die zulässige Sleep-Tiefe.
4. Child-Geräte werden vor Parent-Geräten schlafen gelegt.
5. Parent-Geräte werden vor Child-Geräten aufgeweckt.
6. Nicht zulässige DMA-Zugriffe werden vor Device Sleep beendet.
7. Wake-up-Quellen müssen ausdrücklich autorisiert sein.
8. I/O wird erst nach vollständigem Resume wieder freigegeben.
9. Fehlerhafte Sleep-Zustände können zur Laufzeit deaktiviert werden.
10. Device-Sleep-Telemetrie bleibt lokal und datensparsam.

---

## 32. Zusammenfassung

NovaOS implementiert Device Sleep als zentral koordinierte Runtime-Energieverwaltung für einzelne Geräte.

Der Power Manager wählt anhand von Aktivität, Übergangskosten, Latenzanforderungen und Geräteabhängigkeiten einen geeigneten Zustand. Treiber führen die hardwarespezifischen Übergänge aus, während Device Manager, DMA Framework und Interrupt Manager die Sicherheit des Gesamtzustands gewährleisten.

Power References verhindern das Einschlafen benötigter Geräte. Nicht zuverlässige Sleep-Zustände werden automatisch deaktiviert und durch flachere, sichere Zustände ersetzt.