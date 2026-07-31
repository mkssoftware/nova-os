# ADR-POWER-0002 – Suspend

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-POWER-0002 |
| Titel | Suspend |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Systemenergieverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-POWER-0001, ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0005, ADR-DEV-0009, ADR-DEV-0010 |
| Nachfolgende Spezifikation | NPSPEC-POWER-SUSPEND |

---

## 1. Kontext

NovaOS benötigt einen Energiesparzustand, bei dem die laufende Sitzung im Arbeitsspeicher erhalten bleibt und das System schnell fortgesetzt werden kann.

Suspend soll insbesondere:

- Energieverbrauch reduzieren
- aktive Anwendungen erhalten
- Geräte kontrolliert anhalten
- CPUs in tiefe Ruhezustände versetzen
- einen schnellen Resume ermöglichen
- auf älterer und moderner Hardware funktionieren

Hibernate wird in einer eigenen ADR behandelt und ist nicht Bestandteil dieser Entscheidung.

---

## 2. Problemstellung

Suspend betrifft nahezu alle laufenden Subsysteme gleichzeitig.

Vor dem Übergang muss NovaOS sicherstellen, dass:

- Dateisysteme einen konsistenten Zustand besitzen
- keine gefährlichen DMA-Operationen aktiv bleiben
- Geräte in korrekter Reihenfolge angehalten werden
- Wake-up-Quellen vorbereitet sind
- Timer nach Resume korrekt weiterlaufen
- Treiber Fehler und Timeouts melden können
- ein fehlgeschlagener Suspend sauber zurückgerollt wird

Ein teilweise suspendiertes System darf nicht als erfolgreicher Suspend behandelt werden.

---

## 3. Entscheidung

NovaOS implementiert Suspend als zentral koordinierte, transaktionale Systemoperation.

Der Power Manager führt den Übergang durch eine definierte Phasenfolge. Jede Phase muss erfolgreich abgeschlossen werden, bevor die nächste beginnt.

Schlägt eine reversible Phase fehl, wird der bereits ausgeführte Teil in umgekehrter Reihenfolge zurückgerollt.

NovaOS unterstützt mehrere Plattform-Backends hinter einem gemeinsamen logischen Suspend-Modell.

---

## 4. Suspend-Modi

NovaOS unterscheidet mindestens:

| Modus | Bedeutung |
|---|---|
| `SUSPEND_IDLE` | System läuft weiter, Geräte und CPUs gehen weitgehend in Idle |
| `SUSPEND_TO_RAM` | Sitzung bleibt im RAM, Plattform wird weitgehend abgeschaltet |
| `SUSPEND_PLATFORM` | plattformspezifischer Firmware-Suspend |
| `SUSPEND_AUTO` | Power Manager wählt den besten verfügbaren Modus |

Auf x86 kann `SUSPEND_TO_RAM` beispielsweise über ACPI S3 oder eine moderne vergleichbare Plattformmethode umgesetzt werden.

Die öffentliche API verwendet keine ACPI-spezifischen Zustandsnamen.

---

## 5. Suspend-Zustandsmaschine

```text
ACTIVE
   |
   v
PREPARING
   |
   v
FREEZING
   |
   v
SUSPENDING_DEVICES
   |
   v
SUSPENDING_CPUS
   |
   v
SUSPENDED
   |
   v
RESUMING
   |
   v
ACTIVE
```

Fehler vor dem endgültigen Plattformübergang führen über `ROLLING_BACK` zurück zu `ACTIVE`.

Nicht behebbar fehlgeschlagene Resume-Vorgänge führen in einen Recovery- oder sicheren Neustartpfad.

---

## 6. Suspend-Anforderung

```c
status_t power_suspend(
    uint32_t suspend_mode,
    uint32_t flags,
    uint64_t timeout_ns
);
```

Der Aufrufer benötigt eine System-Power-Capability.

Mögliche Auslöser:

- Benutzeraktion
- Inaktivitätsrichtlinie
- Schließen eines Notebookdeckels
- Energiesparprofil
- administrativer Befehl
- kritischer Akkustand
- Systemdienst

Eine gewöhnliche Anwendung darf das System nicht ohne Autorisierung suspendieren.

---

## 7. Vorbereitungsphase

In `PREPARING` führt NovaOS mindestens aus:

1. Suspend-Anforderung autorisieren.
2. geeigneten Suspend-Modus prüfen.
3. Power Requests und Blocker auswerten.
4. freien Speicher für den Resume-Pfad reservieren.
5. Wake-up-Quellen bestimmen.
6. Systemdienste benachrichtigen.
7. Suspend-Transaktion erzeugen.
8. globale Deadline festlegen.

Der Übergang darf abgebrochen werden, solange keine irreversible Plattformphase erreicht wurde.

---

## 8. Suspend Blocker

Komponenten können einen Suspend vorübergehend verhindern.

Zulässige Gründe sind beispielsweise:

- nicht abschließbare Speicheroperation
- laufendes Firmwareupdate
- kritische Dateisystemtransaktion
- Geräteinitialisierung
- zeitkritischer Systemdienst
- ausdrückliche administrative Sperre

Jeder Blocker benötigt:

- eine Capability
- einen Grund
- einen Besitzer
- möglichst ein Timeout
- Diagnoseinformationen

Normale Hintergrundarbeit ist kein ausreichender Grund, Suspend unbegrenzt zu verhindern.

---

## 9. Prozesse und Anwendungen

Vor Suspend werden Anwendungen und Dienste über ein Ereignis informiert.

Sie können daraufhin:

- Anwendungszustände speichern
- temporäre Operationen abschließen
- Netzwerkaktivität reduzieren
- nicht benötigte Geräte freigeben

Anwendungen erhalten keine unbegrenzte Verzögerungsmöglichkeit.

Nach Ablauf der Frist fährt NovaOS fort, bricht die Anforderung ab oder beendet ausschließlich dafür vorgesehene blockierende Prozesse gemäß Systemrichtlinie.

---

## 10. Dateisysteme

Vor dem Geräte-Suspend:

1. werden kritische VFS-Transaktionen abgeschlossen,
2. werden Journale oder CoW-Commits vorbereitet,
3. werden Dirty Pages entsprechend der Richtlinie geschrieben,
4. werden notwendige Barrieren ausgeführt,
5. werden Dateisysteme über den Suspend informiert.

Suspend verlangt keinen vollständigen Unmount.

Dateisysteme müssen jedoch nach Resume einen konsistenten und eindeutig bekannten Zustand besitzen.

Ein nicht bestätigter kritischer Writeback kann Suspend blockieren oder einen sicheren Fehlerpfad auslösen.

---

## 11. Geräte-Suspend

Geräte werden anhand ihrer Abhängigkeiten suspendiert.

Die Reihenfolge lautet grundsätzlich:

```text
Child Devices vor Parent Devices
Verbraucher vor Ressourcenanbieter
virtuelle Funktionen vor physischen Funktionen
```

Für jedes Gerät:

1. neue I/O-Anfragen stoppen,
2. laufende Operationen abschließen oder abbrechen,
3. Gerätezustand sichern,
4. Interruptquelle kontrollieren,
5. DMA beenden,
6. Wake-up-Funktion konfigurieren,
7. Hardwarezustand wechseln.

Der Device Manager protokolliert den erfolgreichen Übergang jedes Geräts in der Suspend-Transaktion.

---

## 12. DMA und Bus Mastering

Vor dem Plattform-Suspend müssen nicht zulässige DMA-Zugriffe vollständig beendet sein.

NovaOS:

- stoppt Gerätewarteschlangen,
- synchronisiert DMA-Puffer,
- widerruft temporäre DMA-Mappings,
- deaktiviert Bus Mastering, wenn erforderlich,
- programmiert IOMMU- oder Plattformregeln,
- erhält nur ausdrücklich für Wake-up benötigte Ressourcen.

Ein Gerät, dessen DMA nicht sicher angehalten werden kann, darf den Suspend nicht unkontrolliert fortsetzen.

---

## 13. Interrupts und Wake-up-Quellen

Vor Suspend werden normale Interruptquellen maskiert oder in einen geeigneten Zustand versetzt.

Nur freigegebene Wake-up-Quellen bleiben aktiv.

Mögliche Wake-up-Quellen sind:

- Power-Taste
- Echtzeituhr
- Tastatur oder Maus
- Deckelsensor
- ausgewählte USB-Geräte
- Netzwerkadapter
- Plattformcontroller

Externe Geräte werden nicht automatisch als Wake-up-Quelle zugelassen.

Die gewählte Quelle muss den verwendeten Suspend-Modus unterstützen.

---

## 14. CPU-Suspend

Nachdem Geräte und Dienste angehalten wurden:

1. werden sekundäre CPUs aus dem normalen Scheduling entfernt,
2. werden CPU-lokale Zustände gespeichert,
3. werden Cache- und Speicherbarrieren ausgeführt,
4. wird die Boot-CPU für den Plattformübergang vorbereitet,
5. wird der Resume-Einstiegspunkt registriert,
6. wird der architekturspezifische Suspend-Befehl ausgeführt.

Der Resume-Code muss in einem Speicherbereich liegen, der während Suspend erhalten und eindeutig adressierbar bleibt.

---

## 15. Plattform-Backend

Der endgültige Übergang wird durch ein Plattform-Backend ausgeführt.

Mögliche Backends:

- ACPI
- Device Tree und Plattformfirmware
- UEFI- oder Systemcontroller-Schnittstelle
- Hypervisor
- architekturspezifischer System-Suspend

Firmwaredaten und Rückgabewerte gelten als nicht vertrauenswürdig und werden validiert.

Ein fehlendes oder instabiles Backend darf nicht als erfolgreicher Suspend dargestellt werden.

---

## 16. Resume-Einstieg

Nach dem Wake-up übernimmt zunächst ein minimaler Resume-Pfad.

Er stellt mindestens wieder her:

- CPU-Ausführungsmodus
- Seitentabellen
- Kernel-Stack
- Interruptcontroller-Grundzustand
- Zeitbasis
- Speichercontroller-Zugriff
- sichere Kernelkontrolle

Erst danach beginnt der reguläre Resume der Systemkomponenten.

Der minimale Resume-Pfad darf nicht von noch nicht reaktivierten Geräten oder Dateisystemen abhängen.

---

## 17. Geräte-Resume

Geräte werden in umgekehrter Suspend-Reihenfolge reaktiviert:

```text
Parent Devices vor Child Devices
Ressourcenanbieter vor Verbrauchern
physische Funktionen vor virtuellen Funktionen
```

Treiber müssen davon ausgehen, dass Hardwarezustände verloren gegangen sein können.

Sie müssen:

- Gerät erneut identifizieren
- Registerzustände wiederherstellen
- DMA-Kontexte neu aufbauen
- Interrupts erneut konfigurieren
- laufende I/O-Zustände prüfen
- Ausfall oder Entfernung melden

Ein nach Resume fehlendes Gerät wird über den Plug-and-Play-Entfernungspfad behandelt.

---

## 18. Zeitverwaltung

Während Suspend läuft nicht jede CPU-Zeitquelle weiter.

NovaOS unterscheidet:

- monotone aktive Laufzeit
- Bootzeit einschließlich Suspend
- Echtzeit
- Prozess-CPU-Zeit
- Timer-Deadlines

Nach Resume wird die verstrichene Suspend-Dauer aus einer zuverlässigen Plattform- oder Echtzeitquelle bestimmt.

Prozess-CPU-Zeit darf durch Suspend nicht erhöht werden.

Abgelaufene Timer werden kontrolliert und begrenzt nachgeholt.

---

## 19. Netzwerkzustand

Netzwerkverbindungen können Suspend abhängig vom Protokoll und Interface nicht überleben.

Nach Resume:

- wird der Linkzustand neu geprüft,
- werden Adressen und Routen validiert,
- werden Neighbor Caches aktualisiert oder invalidiert,
- werden VPN-Verbindungen neu bewertet,
- werden DHCP- und IPv6-Laufzeiten geprüft,
- werden betroffene Sockets informiert.

NovaOS darf Netzwerkverbindungen nicht als aktiv darstellen, wenn das zugrunde liegende Interface oder Routing nicht wiederhergestellt wurde.

---

## 20. Rollback

Schlägt der Suspend vor dem Plattformübergang fehl, führt NovaOS einen Rollback aus.

Die Reihenfolge ist umgekehrt zur Suspend-Reihenfolge:

1. CPUs und Schedulerzustände wiederherstellen,
2. Parent-Geräte aktivieren,
3. Child-Geräte fortsetzen,
4. I/O freigeben,
5. Dateisysteme und Dienste informieren,
6. Anwendungen über den Abbruch benachrichtigen.

Ein Rollback gilt erst als erfolgreich, wenn das System wieder einen konsistenten `ACTIVE`-Zustand erreicht hat.

---

## 21. Resume-Fehler

Bei einem Resume-Fehler kann NovaOS:

- ein einzelnes Gerät deaktivieren,
- einen Treiber neu starten,
- ein Gerät zurücksetzen,
- Netzwerk oder Grafik neu initialisieren,
- in einen eingeschränkten Modus wechseln,
- einen sicheren Neustart auslösen,
- Recovery starten.

Ein nicht kritisches Gerät soll nicht automatisch zum Verlust der gesamten Sitzung führen.

Fehler bei Speicher, CPU-Grundzustand oder zentraler Dateisystemintegrität gelten dagegen als kritisch.

---

## 22. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `SUSPEND_RIGHT_REQUEST` | Suspend anfordern |
| `SUSPEND_RIGHT_BLOCK` | begrenzten Suspend Blocker erstellen |
| `SUSPEND_RIGHT_WAKE_SOURCE` | Wake-up-Quelle konfigurieren |
| `SUSPEND_RIGHT_INSPECT` | Status und Diagnose lesen |
| `SUSPEND_RIGHT_POLICY` | Suspend-Richtlinien verändern |
| `SUSPEND_RIGHT_FORCE` | Suspend trotz ausgewählter Blocker erzwingen |
| `SUSPEND_RIGHT_ADMIN` | Suspend-Subsystem verwalten |

Ein erzwungener Suspend benötigt besondere administrative Rechte und darf keine unvermeidbare Datenbeschädigung ignorieren.

---

## 23. Sicherheit

Für Suspend gelten folgende Regeln:

- Suspend-Anforderungen benötigen Capabilities.
- Resume setzt die vorherige Sicherheitskonfiguration wieder in Kraft.
- DMA wird vor Suspend kontrolliert beendet oder isoliert.
- nicht autorisierte Geräte werden nicht als Wake-up-Quelle aktiviert.
- Secure-Debugging- und Kernelisolationsregeln bleiben nach Resume aktiv.
- sensible Schlüssel dürfen nicht durch Geräte oder Diagnosepfade offengelegt werden.
- der Resume-Einstiegspunkt wird gegen Manipulation geschützt.
- Systemintegrität wird vor Wiederfreigabe kritischer Dienste geprüft.

Suspend verschlüsselt den Arbeitsspeicher nicht automatisch. Ein ausgeschalteter oder gestohlener Rechner kann daher ein physisches Angriffsrisiko darstellen.

---

## 24. Datenschutz und Diagnose

NovaOS erfasst lokal:

- angeforderten Suspend-Modus
- Phasenlaufzeiten
- blockierende Komponenten
- Geräte- und Treiberfehler
- Wake-up-Quelle
- Resume-Dauer
- Rollback-Ergebnis
- wiederholte Suspendfehler

Anwendungsinhalte, Arbeitsspeicherinhalte und private Schlüssel werden nicht in gewöhnlichen Suspend-Logs gespeichert.

Wake-up-Ursachen werden nur mit der technisch notwendigen Genauigkeit erfasst.

---

## 25. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Suspend und Resume erfolgreich |
| `STATUS_SUSPEND_BLOCKED` | ein aktiver Blocker verhindert Suspend |
| `STATUS_SUSPEND_UNSUPPORTED` | gewünschter Modus wird nicht unterstützt |
| `STATUS_SUSPEND_TIMEOUT` | eine Phase überschritt ihr Zeitlimit |
| `STATUS_DEVICE_BUSY` | Gerät kann nicht sicher angehalten werden |
| `STATUS_DMA_ACTIVE` | DMA konnte nicht beendet werden |
| `STATUS_WAKE_SOURCE_INVALID` | Wake-up-Quelle ist ungeeignet |
| `STATUS_PLATFORM_ERROR` | Plattformübergang ist fehlgeschlagen |
| `STATUS_SUSPEND_ROLLED_BACK` | Suspend wurde sicher zurückgerollt |
| `STATUS_RESUME_FAILED` | Resume konnte nicht vollständig abgeschlossen werden |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |

---

## 26. Alternative Ansätze

### 26.1 Suspend ohne transaktionale Phasen

**Vorteile:**

- einfachere Implementierung

**Nachteile:**

- schwerer Rollback
- teilweise suspendierte Systemzustände
- höhere Gefahr von Daten- und Gerätefehlern

**Entscheidung:** Abgelehnt.

### 26.2 Nur ACPI S3 unterstützen

**Vorteile:**

- klare Umsetzung für klassische x86-PCs

**Nachteile:**

- ungeeignet für moderne und nicht-x86-basierte Plattformen
- keine abstrakte s2idle- oder Hypervisor-Unterstützung

**Entscheidung:** ACPI ist ein Backend, nicht die öffentliche Suspend-Architektur.

### 26.3 Suspend trotz aller Blocker erzwingen

**Vorteile:**

- Benutzeranforderung wird immer ausgeführt

**Nachteile:**

- Gefahr von Datenverlust und Hardwarefehlern

**Entscheidung:** Nur über einen besonders autorisierten und klar gekennzeichneten Force-Pfad.

---

## 27. Konsequenzen

### Positive Konsequenzen

- schneller Energiesparzustand mit erhaltener Sitzung
- kontrollierte Geräte- und Dateisystemübergänge
- sichere DMA- und Interruptbehandlung
- Rollback bei vorbereitenden Fehlern
- plattformunabhängige Suspend-API
- Unterstützung alter und moderner Hardware
- nachvollziehbare Blocker und Fehler

### Negative Konsequenzen

- hohe subsystemübergreifende Komplexität
- jeder Treiber benötigt Suspend- und Resume-Unterstützung
- fehlerhafte Firmware kann den Übergang verhindern
- Arbeitsspeicher bleibt physisch mit Energie versorgt
- nicht alle Netzwerkverbindungen überleben Suspend

### Risiken

- verlorene Wake-up-Ereignisse
- Geräteausfall nach Resume
- unvollständiger DMA-Widerruf
- fehlerhafte Zeitbasis
- Datenverlust bei Writeback-Fehlern
- physischer Zugriff auf den erhaltenen RAM-Inhalt

---

## 28. Implementierungsrichtlinien

- Suspend wird als transaktionale Phasenfolge implementiert.
- Jeder Schritt besitzt einen definierten Timeout.
- Child-Geräte werden vor ihren Parent-Geräten suspendiert.
- Parent-Geräte werden vor ihren Child-Geräten reaktiviert.
- Kritische Dateisystemtransaktionen werden vor Geräte-Suspend abgeschlossen.
- Nicht benötigte DMA-Mappings und Bus-Master-Zugriffe werden widerrufen.
- Nur ausdrücklich freigegebene Wake-up-Quellen bleiben aktiv.
- Der minimale Resume-Pfad darf nicht von normalen Treibern abhängen.
- ACPI und andere Firmwaremodelle bleiben austauschbare Backends.
- Ein fehlgeschlagener Suspend wird nicht als erfolgreicher Energiesparzustand gemeldet.

---

## 29. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- alle unterstützten Suspend-Modi
- Suspend Blocker und Timeouts
- Dateisystem-Writeback
- Geräteabhängigkeiten
- DMA- und Interruptwiderruf
- Wake-up durch verschiedene Quellen
- Mehrkern-CPU-Suspend
- Timer- und Zeitkorrektur
- Netzwerk- und VPN-Wiederherstellung
- Rollback in jeder vorbereitenden Phase
- fehlende Geräte nach Resume
- Treiberabsturz während Suspend und Resume
- Systeme mit fehlerhafter Firmware
- wiederholte Suspend-/Resume-Zyklen
- Recovery nach kritischem Resume-Fehler

---

## 30. Verbindliche Invarianten

1. Suspend ist eine zentral koordinierte, transaktionale Systemoperation.
2. Jede Suspend-Phase muss vor Beginn der nächsten abgeschlossen sein.
3. Fehlgeschlagene reversible Phasen werden in umgekehrter Reihenfolge zurückgerollt.
4. Child-Geräte werden vor ihren Parent-Geräten suspendiert.
5. Parent-Geräte werden vor ihren Child-Geräten reaktiviert.
6. Unzulässige DMA-Zugriffe werden vor dem Plattformübergang beendet.
7. Nur autorisierte Wake-up-Quellen bleiben aktiv.
8. Der Resume-Einstiegspunkt bleibt ohne normale Treiber ausführbar.
9. Suspend verschlüsselt den Arbeitsspeicher nicht automatisch.
10. Ein teilweise ausgeführter Suspend wird niemals als Erfolg gemeldet.

---

## 31. Zusammenfassung

NovaOS implementiert Suspend als transaktionale, vom Power Manager koordinierte Systemoperation.

Anwendungen, Dateisysteme, Geräte, DMA, Interrupts, CPUs und Plattformfirmware werden in einer festen Phasenfolge angehalten. Vorbereitende Fehler führen zu einem kontrollierten Rollback, während der Resume die Abhängigkeiten in umgekehrter Reihenfolge wiederherstellt.

Mehrere Plattform-Backends ermöglichen Unterstützung für klassische ACPI-Systeme, moderne Low-Power-Plattformen, andere Architekturen und virtuelle Maschinen, ohne diese Unterschiede in der öffentlichen NovaOS-API sichtbar zu machen.