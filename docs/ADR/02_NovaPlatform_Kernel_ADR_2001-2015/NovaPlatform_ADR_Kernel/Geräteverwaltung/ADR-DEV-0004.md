# ADR-DEV-0004 – Plug and Play

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0004 |
| Titel | Plug and Play |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteerkennung und Treiberverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0003, ADR-KERNEL-0101, ADR-IPC-0003 |
| Nachfolgende Spezifikation | NPSPEC-DEV-PLUG-AND-PLAY |

---

## 1. Kontext

NovaOS soll Geräte automatisch erkennen, konfigurieren und mit geeigneten Treibern verbinden.

Plug and Play wird sowohl während des Systemstarts als auch zur Laufzeit benötigt. Es betrifft unter anderem:

- PCI- und PCIe-Geräte
- USB-Geräte
- Speichergeräte
- Eingabegeräte
- Monitore und Grafikadapter
- Netzwerkgeräte
- virtuelle Geräte
- Firmware- und Plattformgeräte

Der Benutzer soll gewöhnliche Hardware ohne manuelle Ressourcen- oder Treiberkonfiguration verwenden können.

---

## 2. Problemstellung

Geräte können jederzeit hinzukommen, entfernt werden oder ihren Zustand ändern. Gleichzeitig benötigen sie begrenzte Hardware-Ressourcen und passende Treiber.

NovaOS muss deshalb sicherstellen, dass:

- Geräte eindeutig erkannt werden,
- Ressourcen konfliktfrei vergeben werden,
- nur geeignete Treiber geladen werden,
- abhängige Geräte in korrekter Reihenfolge starten,
- überraschendes Entfernen sicher behandelt wird,
- Nutzer und Systemdienste konsistente Geräteereignisse erhalten.

---

## 3. Entscheidung

NovaOS implementiert ein ereignisgesteuertes Plug-and-Play-System als Bestandteil des Device Managers.

Die Erkennung erfolgt durch Bus-Enumeratoren. Der Device Manager erzeugt daraus Device Objects, weist Ressourcen zu, wählt einen Treiber und koordiniert den vollständigen Gerätelebenszyklus.

Plug-and-Play-Ereignisse werden in den Kernel Object Graph integriert.

---

## 4. Grundablauf

```text
Gerät erkannt
      |
      v
Device Object erzeugen
      |
      v
Hardwarekennung auswerten
      |
      v
Ressourcen zuweisen
      |
      v
Treiber auswählen und prüfen
      |
      v
Treiber starten
      |
      v
Geräteschnittstellen veröffentlichen
```

Ein Gerät wird erst für Anwendungen sichtbar, nachdem seine erforderliche Initialisierung erfolgreich abgeschlossen wurde.

---

## 5. Erkennungsquellen

Geräte können aus verschiedenen Quellen gemeldet werden:

| Quelle | Beispiele |
|---|---|
| Firmware | ACPI, UEFI, Plattformtabellen |
| Bus-Enumeration | PCI, PCIe, USB |
| Speicher-Subsystem | SATA, NVMe, virtuelle Datenträger |
| Treiber | untergeordnete oder zusammengesetzte Geräte |
| Virtualisierung | virtuelle Busse und Geräte |
| Systemkonfiguration | statisch deklarierte Plattformgeräte |

Jede Quelle übermittelt standardisierte Geräteinformationen an den Device Manager.

---

## 6. Hardwarekennungen

Ein erkanntes Gerät erhält eine Menge von Kennungen.

Beispiele:

```text
PCI\VEN_1234&DEV_5678&REV_02
PCI\VEN_1234&DEV_5678
PCI\CLASS_020000

USB\VID_1234&PID_5678&REV_0100
USB\VID_1234&PID_5678
USB\CLASS_03
```

NovaOS kann folgende Kennungstypen verwenden:

- exakte Gerätekennung
- kompatible Kennung
- Geräteklasse
- Bustyp
- Hersteller- und Produktkennung
- Firmwarekennung
- Plattformkennung

Kennungen dienen dem Treiber-Matching und stellen keine Zugriffsberechtigung dar.

---

## 7. Geräteinstanz

Jede konkrete Geräteinstanz erhält eine Laufzeitkennung.

Zusätzlich kann eine stabile Instanzkennung erzeugt werden aus:

- Seriennummer
- Firmwarepfad
- physischem Buspfad
- Slot oder Port
- Herstellerdaten
- persistentem Geräte-Metadatum

Wenn keine stabile Kennung verfügbar ist, wird eine nur für den aktuellen Anschluss oder Systemstart gültige Kennung verwendet.

NovaOS darf instabile Merkmale nicht als dauerhaft eindeutige Identität darstellen.

---

## 8. Treiberauswahl

Der Device Manager vergleicht die Gerätekennungen mit den signierten Treibermanifesten.

Die Auswahl berücksichtigt:

1. exakte Hardwareübereinstimmung,
2. kompatible Hardwarekennung,
3. Geräteklassenübereinstimmung,
4. Treiberpriorität,
5. ABI- und Architekturkompatibilität,
6. Sicherheits- und Signaturstatus,
7. bekannte Kompatibilitätsausschlüsse.

Ein spezifischer, gültig signierter Treiber wird einem generischen Klassentreiber vorgezogen.

---

## 9. Treibersuche

NovaOS sucht Treiber in folgender Reihenfolge:

1. bereits aktiver kompatibler Systemtreiber,
2. lokaler vertrauenswürdiger Driver Store,
3. installiertes Treiberpaket,
4. generischer NovaOS-Klassentreiber,
5. optionaler Treiberbezug über den Softwaredienst.

Der Systemstart und die grundlegende Geräteerkennung dürfen nicht von einer Netzwerkverbindung abhängig sein.

Treiberdownloads erfolgen nur nach den Datenschutz-, Sicherheits- und Update-Richtlinien von NovaOS.

---

## 10. Ressourcenvergabe

Vor dem Treiberstart weist der Device Manager erforderliche Ressourcen zu:

- MMIO-Bereiche
- I/O-Ports
- Interrupts
- DMA-Kontexte
- Busnummern
- Adressbereiche
- Energieverwaltungsressourcen

Ressourcenkonflikte müssen erkannt werden, bevor der Treiber Zugriff erhält.

Kann keine sichere Zuweisung gefunden werden, bleibt das Gerät deaktiviert und erhält einen eindeutigen Fehlerstatus.

---

## 11. Abhängigkeiten

Plug and Play berücksichtigt Parent-Child- und Versorgungsabhängigkeiten.

Beispiele:

- Ein USB-Gerät benötigt einen aktiven USB-Controller.
- Ein Gerät hinter einem Hub benötigt Hub und Bus.
- Eine Partition benötigt ihr Blockgerät.
- Eine virtuelle Funktion benötigt ihr übergeordnetes Gerät.
- Ein Display benötigt den zugehörigen Grafikadapter.

Untergeordnete Geräte werden erst gestartet, wenn ihre notwendigen Elternobjekte aktiv sind.

Beim Entfernen erfolgt die Reihenfolge umgekehrt.

---

## 12. Gerätestart

Der Start eines Geräts erfolgt in einer kontrollierten Transaktion:

1. Ressourcen reservieren.
2. Treiberpaket und Signatur prüfen.
3. Treiberinstanz starten oder kontaktieren.
4. Capabilities übergeben.
5. Gerät initialisieren.
6. Geräteschnittstellen registrieren.
7. Zustand auf `ACTIVE` setzen.
8. Plug-and-Play-Ereignis veröffentlichen.

Schlägt ein Schritt fehl, werden zuvor reservierte Ressourcen zurückgenommen.

---

## 13. Hotplug

Hotplug-fähige Busse melden Geräteänderungen während des Betriebs.

Beim Hinzufügen:

- wird das Gerät entprellt und validiert,
- werden Kennungen gelesen,
- wird das Device Object erzeugt,
- erfolgt der normale Matching- und Startablauf.

Mehrfachmeldungen desselben physischen Ereignisses dürfen nicht zu mehrfachen Device Objects führen.

Plug-and-Play-Verarbeitung darf zeitkritische Interruptpfade nicht blockieren.

---

## 14. Geordnetes Entfernen

Vor einem geplanten Entfernen kann eine Remove-Anfrage ausgeführt werden.

```c
status_t device_request_remove(
    cap_handle_t device_handle,
    uint32_t flags
);
```

Der Device Manager prüft:

- offene Geräteschnittstellen,
- laufende I/O-Operationen,
- gemountete Dateisysteme,
- abhängige Geräte,
- nicht gespeicherte Daten,
- blockierende Systemdienste.

Ist das Entfernen nicht sicher, kann die Anfrage mit `STATUS_DEVICE_BUSY` abgelehnt werden.

---

## 15. Überraschendes Entfernen

Bei unerwartetem Entfernen:

1. wird das Gerät sofort als nicht mehr zugänglich markiert,
2. werden neue I/O-Anfragen abgelehnt,
3. werden Interrupt- und DMA-Bindungen widerrufen,
4. werden laufende Operationen mit Fehler beendet,
5. werden abhängige Geräte entfernt,
6. wird der Treiber gestoppt,
7. werden Schnittstellen und Ressourcen freigegeben.

Ein überraschend entferntes Gerät darf keinen weiteren DMA-Zugriff auf Systemspeicher besitzen.

---

## 16. Plug-and-Play-Ereignisse

Systemdienste können folgende Ereignisse abonnieren:

| Ereignis | Bedeutung |
|---|---|
| `DEVICE_DISCOVERED` | Neues Gerät wurde erkannt |
| `DEVICE_STARTING` | Geräteinitialisierung beginnt |
| `DEVICE_AVAILABLE` | Gerät kann verwendet werden |
| `DEVICE_CHANGED` | Eigenschaften haben sich geändert |
| `DEVICE_REMOVE_PENDING` | Geplantes Entfernen steht bevor |
| `DEVICE_REMOVED` | Gerät wurde entfernt |
| `DEVICE_FAILED` | Gerät oder Treiber ist ausgefallen |

Ereignisse enthalten nur autorisierte Metadaten. Geräteseriennummern werden nicht ohne Notwendigkeit allgemein veröffentlicht.

---

## 17. Benutzerinteraktion

Gewöhnliche Plug-and-Play-Vorgänge laufen ohne Benutzerabfrage ab.

Eine Interaktion ist nur erforderlich, wenn:

- kein geeigneter Treiber vorhanden ist,
- ein Gerät besondere Rechte benötigt,
- ein nicht vertrauenswürdiger Treiber angeboten wird,
- ein Gerät vor dem Entfernen noch verwendet wird,
- ein Neustart erforderlich ist,
- eine Sicherheitsrichtlinie den Zugriff blockiert.

Technische Hardwarekennungen sollen in der normalen Benutzeroberfläche durch verständliche Gerätenamen ergänzt werden.

---

## 18. Fehlerzustände

Ein Plug-and-Play-Fehler wird am Device Object gespeichert.

Mögliche Statuswerte sind:

| Status | Bedeutung |
|---|---|
| `NO_DRIVER` | Kein geeigneter Treiber gefunden |
| `DRIVER_BLOCKED` | Treiber wurde durch Richtlinie blockiert |
| `RESOURCE_CONFLICT` | Ressourcen konnten nicht zugewiesen werden |
| `START_FAILED` | Treiberinitialisierung fehlgeschlagen |
| `DEPENDENCY_FAILED` | Notwendiges übergeordnetes Gerät fehlt |
| `DEVICE_UNRESPONSIVE` | Gerät antwortet nicht |
| `DEVICE_REMOVED` | Gerät wurde entfernt |

Fehlerstatus und technische Details müssen über lokale Diagnosewerkzeuge abrufbar sein.

---

## 19. Sicherheit

Für Plug and Play gelten folgende Sicherheitsregeln:

- Neu erkannte Geräte gelten zunächst als nicht vertrauenswürdig.
- Treiber werden vor dem Laden verifiziert.
- Geräte erhalten keinen direkten Zugriff auf beliebigen Systemspeicher.
- DMA wird über IOMMU oder eingeschränkte DMA-Puffer kontrolliert.
- USB- und andere externe Geräte dürfen keine automatische Benutzerfreigabe anfordern oder simulieren.
- Geschützte Geräteklassen können vor der Aktivierung blockiert werden.
- Gerätekennungen werden nicht als Sicherheitsnachweis verwendet.

NovaOS kann Richtlinien für einzelne Geräteklassen, Ports, Hersteller oder Benutzerkontexte anwenden.

---

## 20. Ressourcenbegrenzung

Zum Schutz vor fehlerhaften oder bösartigen Geräten werden begrenzt:

- Anzahl gleichzeitig erkannter Geräte
- Hierarchietiefe
- Größe und Anzahl von Deskriptoren
- offene Initialisierungsoperationen
- Treiberstartversuche
- Hotplug-Ereignisrate
- Speicherverbrauch pro Enumerator

Ein Gerät mit fehlerhaften oder übergroßen Deskriptoren wird abgelehnt oder isoliert.

---

## 21. Audit und Diagnose

Audit-Ereignisse können erzeugt werden bei:

- Anschluss sicherheitsrelevanter Geräte
- blockierten Geräten
- fehlgeschlagenen Treibersignaturen
- Ressourcenverletzungen
- überraschendem Entfernen
- wiederholten Initialisierungsfehlern
- administrativer Gerätefreigabe
- Laden eines Entwicklungstreibers

Normale Geräteanschlüsse werden nur mit den für Diagnose und Sicherheit notwendigen Metadaten gespeichert.

---

## 22. Alternative Ansätze

### 22.1 Manuelle Gerätekonfiguration

**Vorteile:**

- vollständige administrative Kontrolle
- geringere automatische Komplexität

**Nachteile:**

- ungeeignet für moderne Hotplug-Hardware
- hohe Fehleranfälligkeit
- schlechte Benutzerfreundlichkeit

**Entscheidung:** Abgelehnt.

### 22.2 Treiber wählen sich ihre Geräte selbst

**Vorteile:**

- einfache Treiberentwicklung

**Nachteile:**

- konkurrierende Treiber
- unkontrolliertes Hardware-Scanning
- schwer durchsetzbare Sicherheitsrichtlinien

**Entscheidung:** Abgelehnt. Die Zuordnung erfolgt zentral durch den Device Manager.

### 22.3 Automatischer Download jedes passenden Treibers

**Vorteile:**

- hohe Geräteabdeckung

**Nachteile:**

- Sicherheits- und Datenschutzrisiken
- Netzwerkabhängigkeit
- möglicherweise unerwünschte Softwareinstallation

**Entscheidung:** Abgelehnt. Externe Treiber werden kontrolliert über den Softwaredienst bezogen.

---

## 23. Konsequenzen

### Positive Konsequenzen

- automatische Geräteerkennung
- deterministische Treiberzuordnung
- konfliktfreie Ressourcenverwaltung
- Unterstützung für Hotplug
- sichere Behandlung entfernter Geräte
- verständliche Geräte- und Fehlerzustände
- Integration in das Capability-Modell

### Negative Konsequenzen

- komplexe Zustands- und Abhängigkeitsverwaltung
- hoher Testaufwand für verschiedene Busse
- fehlerhafte Firmwaredaten müssen abgefangen werden
- nicht jedes Gerät kann automatisch konfiguriert werden

### Risiken

- Race Conditions beim schnellen Ein- und Ausstecken
- falsches Treiber-Matching
- Ressourcenlecks nach fehlgeschlagenem Start
- DMA nach überraschendem Entfernen
- Ereignisüberflutung durch defekte Geräte

---

## 24. Implementierungsrichtlinien

- Geräte werden erst nach erfolgreichem Start veröffentlicht.
- Hardwarekennungen müssen vollständig validiert werden.
- Treiber-Matching muss deterministisch sein.
- Ressourcen werden vor dem Treiberstart reserviert.
- Fehlgeschlagene Startvorgänge müssen vollständig zurückgerollt werden.
- Untergeordnete Geräte starten nach ihren Eltern und stoppen davor.
- Hotplug-Verarbeitung erfolgt außerhalb zeitkritischer Interruptpfade.
- Überraschendes Entfernen muss DMA und Interrupts unverzüglich sperren.
- Mehrfachmeldungen dürfen keine doppelten Geräteinstanzen erzeugen.

---

## 25. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Geräteerkennung während des Starts
- Hotplug und schnelles Wiedereinstecken
- Hardware- und Klassen-Matching
- mehrere passende Treiber
- fehlende und blockierte Treiber
- Ressourcenkonflikte
- Parent-Child-Abhängigkeiten
- erfolgreichen und fehlgeschlagenen Gerätestart
- geplantes und überraschendes Entfernen
- aktive I/O beim Entfernen
- fehlerhafte Gerätedeskriptoren
- Ereignisüberflutung
- Treiber-Rollback
- Mehrkernbetrieb

---

## 26. Verbindliche Invarianten

1. Plug-and-Play-Geräte werden durch Device Objects repräsentiert.
2. Treiber wählen Geräte nicht durch unkontrolliertes Scannen selbst aus.
3. Treiberauswahl und Ressourcenvergabe erfolgen zentral.
4. Geräte werden erst nach erfolgreicher Initialisierung veröffentlicht.
5. Fehlgeschlagene Initialisierungen hinterlassen keine Ressourcenbindungen.
6. Untergeordnete Geräte werden vor ihren Eltern entfernt.
7. Überraschendes Entfernen widerruft DMA- und Interruptzugriffe.
8. Hardwarekennungen stellen keine Zugriffsberechtigung dar.
9. Treiberdownloads sind für den Systemstart nicht erforderlich.
10. Sicherheitsrelevante Plug-and-Play-Vorgänge sind auditierbar.

---

## 27. Zusammenfassung

NovaOS implementiert ein zentrales, ereignisgesteuertes Plug-and-Play-System innerhalb des Device Managers.

Bus-Enumeratoren melden Geräte, der Device Manager erzeugt Device Objects, weist Ressourcen zu und wählt einen gültig signierten Treiber. Erst nach erfolgreicher Initialisierung werden die Geräteschnittstellen veröffentlicht.

Hotplug, Abhängigkeiten, Ressourcenfehler und überraschendes Entfernen werden durch einen einheitlichen und capability-geschützten Gerätelebenszyklus behandelt.