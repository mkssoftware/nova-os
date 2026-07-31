# ADR-DEV-0009 – Interrupt Routing

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0009 |
| Titel | Interrupt Routing |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interruptverwaltung und Gerätearchitektur |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0006, ADR-DEV-0007, ADR-PROC-0007 |
| Nachfolgende Spezifikation | NPSPEC-DEV-INTERRUPT-ROUTING |

---

## 1. Kontext

Hardwaregeräte verwenden Interrupts, um Prozessoren über Ereignisse zu informieren.

Dazu gehören:

- abgeschlossene I/O-Operationen
- neu eingetroffene Netzwerkpakete
- Eingabegeräte
- Timer
- Hardwarefehler
- Hotplug-Ereignisse
- Energiezustandsänderungen

NovaOS muss Interruptquellen den geeigneten CPUs und Treibern sicher zuordnen.

---

## 2. Problemstellung

Unterschiedliche Plattformen stellen verschiedene Interruptcontroller und Routingverfahren bereit.

NovaOS muss unter anderem unterstützen:

- Legacy PIC
- I/O APIC und Local APIC
- MSI und MSI-X
- mehrere CPUs
- geteilte Interruptleitungen
- Interrupt-Affinität
- Userspace-Treiber
- Hotplug und Geräteentfernung

Fehlerhaftes Routing kann zu verlorenen Interrupts, Interruptstürmen, hoher Latenz oder einem nicht mehr reagierenden System führen.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen Interrupt Manager.

Er verwaltet:

- Interruptquellen
- globale Interruptkennungen
- Vektoren
- Controller-Routen
- CPU-Ziele
- Trigger- und Polaritätsmodi
- Treiberbindungen
- Maskierung und Quittierung
- Interruptstatistiken

Treiber erhalten Interrupt-Capabilities und dürfen Controller oder Routingtabellen nicht direkt konfigurieren.

---

## 4. Architektur

```text
Hardwarequelle
      |
      v
Interruptcontroller
      |
      v
Interrupt Manager
      |
      +--> Kernel Handler
      |
      +--> Deferred Work
      |
      +--> Userspace Driver Event
```

Der Interrupt Manager abstrahiert die plattformspezifischen Controller hinter einer gemeinsamen Schnittstelle.

---

## 5. Interrupt-Domains

Jeder Interruptcontroller stellt eine Interrupt Domain bereit.

Beispiele:

- PIC Domain
- I/O-APIC Domain
- Local-APIC Domain
- MSI Domain
- MSI-X Domain
- virtuelle Interrupt Domain

Eine Domain übersetzt eine controllerspezifische Hardwarekennung in eine globale NovaOS-Interruptkennung.

```c
typedef uint64_t interrupt_id_t;
```

Treiber verwenden globale Interruptkennungen nicht als Berechtigungsnachweis.

---

## 6. Interrupt-Modi

NovaOS unterstützt folgende Eigenschaften:

| Eigenschaft | Werte |
|---|---|
| Trigger | Edge oder Level |
| Polarität | High oder Low |
| Zustellung | Fixed, Lowest Priority oder plattformspezifisch |
| Ziel | einzelne CPU oder CPU-Menge |
| Teilung | exklusiv oder gemeinsam |

Trigger und Polarität werden aus Bus-, Firmware- oder Gerätekonfiguration ermittelt und validiert.

Falsche Werte dürfen nicht stillschweigend übernommen werden.

---

## 7. Vektorverwaltung

CPU-Interruptvektoren werden zentral vergeben.

Reservierte Vektoren umfassen unter anderem:

- CPU-Exceptions
- Systemtimer
- Inter-Processor Interrupts
- Scheduler-Interrupts
- Debug- und Diagnosevektoren
- Spurious Interrupts

Geräteinterrupts erhalten ausschließlich freie, geeignete Vektoren.

Treiber können keinen bestimmten CPU-Vektor direkt anfordern.

---

## 8. Controller-Priorität

Auf x86 verwendet NovaOS grundsätzlich folgende Reihenfolge:

1. Local APIC und I/O APIC
2. MSI oder MSI-X für geeignete PCIe-Geräte
3. Legacy PIC nur als Start- oder Kompatibilitätslösung

MSI-X wird bei Geräten mit mehreren Warteschlangen bevorzugt, sofern Hardware, Treiber und Sicherheitsrichtlinie dies unterstützen.

Der Legacy PIC wird nach erfolgreicher APIC-Initialisierung deaktiviert oder in einen sicheren Kompatibilitätszustand versetzt.

---

## 9. Registrierung

Ein Treiber registriert eine Interruptbindung über eine Capability:

```c
status_t interrupt_bind(
    cap_handle_t interrupt_source,
    uint32_t flags,
    cap_handle_t *binding_handle
);
```

Die Bindung beschreibt:

- Interruptquelle
- Zustellungsmodus
- Ziel-CPU oder Affinitätsrichtlinie
- Kernel- oder Userspace-Empfänger
- Maskierungszustand
- Generation der Geräteinstanz

Die Capability wird ungültig, wenn das Gerät entfernt oder die Ressource widerrufen wird.

---

## 10. Kernel-Interruptbehandlung

Kernel-Treiber registrieren einen kurzen Interrupt Handler.

```c
typedef interrupt_result_t (*interrupt_handler_t)(
    interrupt_context_t *context,
    void *driver_context
);
```

Ein Handler darf im Interruptkontext nur:

- die Quelle identifizieren,
- notwendige Register lesen oder quittieren,
- minimale Statusdaten sichern,
- Deferred Work einplanen,
- ein Kernel-Event signalisieren.

Blockierende Operationen, große Speicherallokationen und gewöhnliche RPC-Aufrufe sind im Interruptkontext verboten.

---

## 11. Userspace-Treiber

Userspace-Treiber erhalten Interrupts über ein kernelverwaltetes Event- oder Counter-Objekt.

Ablauf:

1. Interrupt tritt auf.
2. Kernel erkennt und quittiert beziehungsweise maskiert die Quelle.
3. Interruptzähler wird erhöht.
4. Treiber-Event wird signalisiert.
5. Userspace-Treiber verarbeitet das Gerät.
6. Treiber bestätigt den Abschluss.
7. Level-Interrupt wird gegebenenfalls wieder freigegeben.

Ein binäres Event allein darf nicht dazu führen, dass mehrere Interrupts unbemerkt verloren gehen. Deshalb wird zusätzlich ein monotoner Zähler geführt.

---

## 12. Deferred Work

Aufwendige Verarbeitung wird aus dem direkten Interruptkontext ausgelagert.

Mögliche Mechanismen:

- Deferred Procedure Call
- Kernel-Workqueue
- Treiber-Worker-Thread
- Userspace-Event
- Polling-Budget für Hochlastgeräte

Dadurch bleibt die Zeit mit deaktivierten oder eingeschränkten Interrupts kurz.

Deferred Work muss die Gerätegeneration prüfen, bevor es auf ein möglicherweise entferntes Gerät zugreift.

---

## 13. Geteilte Interrupts

Legacy-Interruptleitungen können von mehreren Geräten verwendet werden.

Bei einem geteilten Interrupt:

- werden alle registrierten Handler abgefragt,
- muss jeder Handler prüfen, ob sein Gerät die Quelle war,
- darf kein Handler einen fremden Interrupt als verarbeitet melden,
- werden unbehandelte Interrupts diagnostisch gezählt.

MSI und MSI-X werden bevorzugt, da sie normalerweise keine gemeinsam genutzten Leitungen benötigen.

---

## 14. Interrupt-Affinität

Interrupts können einer CPU oder CPU-Menge zugeordnet werden.

Die Auswahl berücksichtigt:

- Treiberanforderungen
- CPU-Auslastung
- NUMA-Nähe
- Queue-Affinität
- Cache-Lokalität
- Energieprofil
- Echtzeitanforderungen
- CPU-Hotplug

Treiber dürfen eine Affinitätspräferenz angeben. Die endgültige Entscheidung trifft der Interrupt Manager zusammen mit dem Scheduler.

Offline-CPUs dürfen keine aktiven Interruptziele bleiben.

---

## 15. MSI und MSI-X

Der PCI Manager beantragt MSI- oder MSI-X-Ressourcen beim Interrupt Manager.

Dabei gelten folgende Regeln:

- Nachrichtenadresse und Daten werden vom Kernel erzeugt.
- Treiber dürfen MSI-Zieladressen nicht frei bestimmen.
- MSI-X-Tabellenzugriff wird kontrolliert.
- Vektoren werden vor Aktivierung vollständig eingerichtet.
- Beim Entfernen werden MSI und MSI-X deaktiviert.
- Geräte erhalten nur die genehmigte Anzahl an Vektoren.

Interrupt Remapping durch eine IOMMU wird verwendet, wenn die Plattform es unterstützt.

---

## 16. Maskierung und Quittierung

Der Interrupt Manager unterscheidet:

- Controller-Maskierung
- geräteseitige Maskierung
- Quittierung
- End of Interrupt
- Abschluss der Treiberverarbeitung

Die genaue Reihenfolge hängt von Triggerart und Controller ab.

Level-triggered Interrupts dürfen erst wieder freigegeben werden, wenn die Quelle beseitigt oder sicher maskiert wurde. Andernfalls kann eine Interruptschleife entstehen.

---

## 17. Interrupt Storm Protection

NovaOS erkennt ungewöhnlich hohe Interruptfrequenzen.

Mögliche Reaktionen:

1. Diagnosezähler erhöhen.
2. Interrupt vorübergehend drosseln oder maskieren.
3. Treiber benachrichtigen.
4. Polling-Modus aktivieren.
5. Gerät oder Treiber zurücksetzen.
6. fehlerhaftes Gerät deaktivieren.

Kritische Systeminterrupts benötigen gesonderte Grenzwerte und dürfen nicht unkontrolliert abgeschaltet werden.

---

## 18. Hotplug und Widerruf

Beim Entfernen eines Geräts:

1. werden neue Interruptbindungen verhindert,
2. wird die Quelle maskiert,
3. werden geräteseitige Interrupts deaktiviert,
4. werden laufende Handler abgeschlossen,
5. wird ausstehendes Deferred Work markiert oder abgebrochen,
6. werden Vektoren und Controller-Routen freigegeben,
7. werden Interrupt-Capabilities widerrufen.

Nach Abschluss des Widerrufs darf kein Handler mehr auf das entfernte Gerät zugreifen.

---

## 19. Sicherheit

Für Interrupt Routing gelten folgende Regeln:

- Routingänderungen benötigen administrative Capabilities.
- Treiber erhalten nur Capabilities ihrer Geräteinterrupts.
- Interruptvektoren und MSI-Nachrichten werden vom Kernel erzeugt.
- Userspace kann keine beliebigen Interrupts auslösen oder quittieren.
- Firmware-Routinginformationen gelten als nicht vertrauenswürdig.
- Interrupt Handler dürfen keine Userspace-Adressen ungeprüft verwenden.
- Entfernte Geräte verlieren alle Interruptbindungen.
- Interrupt Remapping wird verwendet, wenn verfügbar.

---

## 20. API

Die zentrale API umfasst mindestens:

```c
status_t interrupt_set_affinity(
    cap_handle_t binding_handle,
    const cpu_set_t *cpu_set
);

status_t interrupt_mask(
    cap_handle_t binding_handle
);

status_t interrupt_unmask(
    cap_handle_t binding_handle
);

status_t interrupt_acknowledge(
    cap_handle_t binding_handle,
    uint64_t observed_count
);

status_t interrupt_get_statistics(
    cap_handle_t binding_handle,
    interrupt_statistics_t *statistics
);
```

Die erlaubten Operationen hängen von den Rechten der Capability und vom Interrupttyp ab.

---

## 21. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Ungültige Interrupt-Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_VECTOR_UNAVAILABLE` | Kein geeigneter Vektor verfügbar |
| `STATUS_INVALID_AFFINITY` | CPU-Ziel ist ungültig |
| `STATUS_INTERRUPT_SHARED` | Exklusive Bindung nicht möglich |
| `STATUS_DEVICE_REMOVED` | Interruptquelle wurde entfernt |
| `STATUS_INTERRUPT_STORM` | Quelle wurde wegen Überlastung begrenzt |
| `STATUS_NOT_SUPPORTED` | Modus wird nicht unterstützt |
| `STATUS_BUSY` | Bindung oder Quelle ist noch aktiv |

---

## 22. Audit und Diagnose

Der Interrupt Manager erfasst lokal:

- Interruptanzahl
- unbehandelte Interrupts
- Handlerlaufzeiten
- CPU-Verteilung
- maskierte Quellen
- Interruptstürme
- MSI-/MSI-X-Modus
- Routingänderungen
- Widerrufsfehler

Auditiert werden insbesondere:

- administrative Routingänderungen
- unerlaubte Bindungsversuche
- wiederholte Interruptstürme
- Verwendung von Debug-Interrupts
- fehlgeschlagene Widerrufe beim Hot-Unplug

---

## 23. Alternative Ansätze

### 23.1 Treiber konfigurieren Interruptcontroller direkt

**Vorteile:**

- geringe Abstraktion
- direkter Hardwarezugriff

**Nachteile:**

- Konflikte und Sicherheitsrisiken
- keine zentrale Vektorverwaltung
- ungeeignet für Userspace-Treiber

**Entscheidung:** Abgelehnt.

### 23.2 Alle Interrupts auf CPU 0

**Vorteile:**

- einfache Implementierung

**Nachteile:**

- schlechte Skalierung
- hohe Latenzen
- CPU 0 wird zum Engpass

**Entscheidung:** Nur als frühe Startlösung zulässig.

### 23.3 Ausschließlich Polling

**Vorteile:**

- keine Interrupt-Routing-Komplexität

**Nachteile:**

- hoher Energieverbrauch
- schlechte Latenz bei niedriger Last
- ungeeignet für viele Geräteklassen

**Entscheidung:** Polling bleibt eine gezielte Hochlast- oder Fehlerstrategie.

---

## 24. Konsequenzen

### Positive Konsequenzen

- zentrale und sichere Interruptverwaltung
- Unterstützung mehrerer Controllerarchitekturen
- skalierbare Mehrkernverteilung
- sichere Userspace-Treiberbenachrichtigung
- kontrolliertes MSI und MSI-X
- Schutz gegen Interruptstürme
- Integration mit CPU-Affinität und Hotplug

### Negative Konsequenzen

- komplexe Controllerabstraktion
- zusätzlicher Aufwand für Userspace-Zustellung
- Affinitätsentscheidungen benötigen Laufzeitstatistiken
- Legacy-Hardware erfordert Sonderbehandlung

### Risiken

- verlorene oder doppelte Interrupts
- fehlerhafte EOI-Reihenfolge
- Zugriff auf entfernte Geräte
- CPU-Überlastung durch falsche Affinität
- Race Conditions bei Maskierung und Widerruf

---

## 25. Implementierungsrichtlinien

- Interruptvektoren werden ausschließlich zentral vergeben.
- Hardware Handler bleiben kurz und nicht blockierend.
- Aufwendige Arbeit wird in Deferred Work verschoben.
- Userspace-Zustellung verwendet Event und Interruptzähler.
- Level-Interrupts werden erst nach Beseitigung der Quelle freigegeben.
- MSI-Daten und Zieladressen werden ausschließlich vom Kernel erzeugt.
- Offline-CPUs werden aus allen Affinitätsmasken entfernt.
- Hot-Unplug wartet auf aktive Handler und widerruft Deferred Work.
- Firmware-Routingdaten werden vor Verwendung validiert.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- PIC-, APIC- und I/O-APIC-Routing
- Edge- und Level-Interrupts
- geteilte Interruptleitungen
- MSI und MSI-X
- Userspace-Treiberzustellung
- Interruptzähler
- CPU-Affinität und CPU-Hotplug
- gleichzeitige Interrupts auf mehreren CPUs
- Geräteentfernung während eines Handlers
- Interrupt Storm Protection
- fehlerhafte Firmware-Routen
- Maskierung und Quittierung
- Spurious Interrupts
- Vektorerschöpfung

---

## 27. Verbindliche Invarianten

1. Interrupt Routing wird zentral durch den Interrupt Manager verwaltet.
2. Treiber konfigurieren Interruptcontroller nicht direkt.
3. CPU-Vektoren werden ausschließlich durch den Kernel vergeben.
4. Treiber erhalten Interruptzugriff nur über Capabilities.
5. Hardware-Interrupt-Handler dürfen nicht blockieren.
6. Aufwendige Verarbeitung erfolgt außerhalb des direkten Interruptkontexts.
7. MSI- und MSI-X-Nachrichten werden vom Kernel konfiguriert.
8. Offline-CPUs dürfen keine Interruptziele bleiben.
9. Geräteentfernung widerruft sämtliche Interruptbindungen.
10. Interruptstürme werden erkannt und kontrolliert begrenzt.

---

## 28. Zusammenfassung

NovaOS verwendet einen zentralen Interrupt Manager für Controller-Routing, Vektorvergabe, CPU-Affinität und Treiberzustellung.

Kernel-Treiber erhalten kurze Handler, während Userspace-Treiber über capability-geschützte Events und Interruptzähler benachrichtigt werden. MSI und MSI-X werden gegenüber geteilten Legacy-Interrupts bevorzugt.

Durch zentrale Routingkontrolle, Hot-Unplug-Widerruf und Schutz vor Interruptstürmen bleibt die Interruptverarbeitung skalierbar und mit dem NovaOS-Sicherheitsmodell vereinbar.