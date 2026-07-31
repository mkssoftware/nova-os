# ADR-DEV-0005 – Power Management

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0005 |
| Titel | Power Management |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Geräteverwaltung und Energieverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0004, ADR-KERNEL-0100, ADR-KERNEL-0101 |
| Nachfolgende Spezifikation | NPSPEC-DEV-POWER-MANAGEMENT |

---

## 1. Kontext

NovaOS muss den Energieverbrauch von Prozessoren, Geräten und dem Gesamtsystem kontrollieren.

Die Energieverwaltung ist notwendig für:

- längere Akkulaufzeit,
- geringere Wärmeentwicklung,
- niedrigeren Stromverbrauch,
- leiseren Betrieb,
- sichere Suspend- und Resume-Vorgänge,
- kontrolliertes Herunterfahren,
- Unterstützung älterer und moderner Hardware.

Das Power Management muss mit Scheduler, Device Manager, Treibern und dem Kernel Object Graph zusammenarbeiten.

---

## 2. Problemstellung

Geräte besitzen unterschiedliche Energiezustände und Abhängigkeiten. Ein Gerät kann nur dann deaktiviert werden, wenn es nicht mehr benötigt wird und abhängige Komponenten den Zustandswechsel erlauben.

NovaOS muss außerdem verhindern, dass:

- laufende I/O-Operationen verloren gehen,
- Geräte in falscher Reihenfolge abgeschaltet werden,
- Treiber nach einem Resume mit ungültigem Zustand weiterarbeiten,
- DMA-Operationen während des Suspend aktiv bleiben,
- fehlerhafte Geräte den gesamten Suspend-Vorgang dauerhaft blockieren.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen Power Manager.

Der Power Manager koordiniert:

- Systemenergiezustände,
- Geräteenergiezustände,
- CPU-Leistungs- und Ruhezustände,
- Suspend und Resume,
- Wake-up-Quellen,
- Energieanforderungen von Prozessen und Treibern,
- thermische Schutzmaßnahmen.

Gerätespezifische Übergänge werden durch die jeweiligen Treiber ausgeführt, aber zentral durch den Power Manager angeordnet.

---

## 4. Architektur

```text
Anwendungen und Dienste
          |
          v
     Power Manager
      /    |     \
     v     v      v
Scheduler Geräte  Plattform
          Manager Firmware
```

Der Power Manager trifft globale Entscheidungen. Device Manager und Treiber setzen die gerätespezifischen Übergänge um.

---

## 5. Systemenergiezustände

NovaOS definiert folgende logische Systemzustände:

| Zustand | Bedeutung |
|---|---|
| `SYSTEM_ACTIVE` | System ist vollständig aktiv |
| `SYSTEM_IDLE` | System läuft, ist aber weitgehend inaktiv |
| `SYSTEM_SUSPEND` | Arbeitsspeicher bleibt erhalten |
| `SYSTEM_HIBERNATE` | Systemzustand wird persistent gespeichert |
| `SYSTEM_SHUTDOWN` | Geordnetes Herunterfahren |
| `SYSTEM_OFF` | Plattform ist ausgeschaltet |
| `SYSTEM_RESTART` | Geordneter Neustart |

Diese Zustände werden auf die Fähigkeiten von ACPI, Firmware und Hardwareplattform abgebildet.

Nicht jede Plattform muss alle Zustände unterstützen.

---

## 6. Geräteenergiezustände

Geräte verwenden ein plattformunabhängiges Zustandsmodell:

| Zustand | Bedeutung |
|---|---|
| `DEVICE_ACTIVE` | Gerät ist vollständig betriebsbereit |
| `DEVICE_IDLE` | Gerät ist aktiv, aber ohne aktuelle Arbeit |
| `DEVICE_LOW_POWER` | Reduzierter Energieverbrauch |
| `DEVICE_SUSPENDED` | Gerätezustand ist angehalten |
| `DEVICE_OFF` | Gerät ist ausgeschaltet |
| `DEVICE_FAILED` | Übergang oder Gerät ist fehlgeschlagen |

Treiber übersetzen diese Zustände in hardwarespezifische Register- oder Firmwareoperationen.

---

## 7. Abhängigkeitsmodell

Die Reihenfolge von Energieübergängen folgt dem Device Object Graph.

Beim Suspendieren gilt:

```text
untergeordnete Geräte zuerst
übergeordnete Geräte danach
```

Beim Resume gilt:

```text
übergeordnete Geräte zuerst
untergeordnete Geräte danach
```

Beispiel: Ein USB-Gerät wird vor dem USB-Controller suspendiert und nach ihm wieder aktiviert.

Zusätzliche Energieabhängigkeiten dürfen im Objektgraph modelliert werden.

---

## 8. Geräteübergang

Ein Geräteübergang wird über den Device Manager angefordert:

```c
status_t device_set_power_state(
    cap_handle_t device_handle,
    uint32_t target_state,
    uint64_t timeout_ns
);
```

Der Ablauf umfasst:

1. neue I/O-Anfragen begrenzen oder stoppen,
2. laufende Operationen abschließen oder abbrechen,
3. DMA synchronisieren und gegebenenfalls sperren,
4. Gerätezustand durch den Treiber sichern,
5. Hardwarezustand ändern,
6. Ergebnis an den Power Manager melden.

Der Zustand wird erst nach erfolgreichem Abschluss aktualisiert.

---

## 9. Suspend

Vor dem System-Suspend führt NovaOS mindestens folgende Schritte aus:

1. Suspend-Anforderung an Anwendungen und Systemdienste melden.
2. Speicherdaten und Dateisysteme synchronisieren.
3. neue nicht notwendige I/O-Operationen blockieren.
4. Geräte in Abhängigkeitsreihenfolge suspendieren.
5. nicht erlaubte DMA-Zugriffe widerrufen.
6. Wake-up-Quellen konfigurieren.
7. CPUs und Plattform in den Suspend-Zustand versetzen.

Scheitert ein kritischer Schritt, wird der Suspend abgebrochen oder eine definierte Fehlerstrategie angewendet.

---

## 10. Resume

Nach einem Wake-up:

1. initialisiert NovaOS den erforderlichen CPU- und Plattformzustand,
2. stellt Zeitbasis und Interruptsystem wieder her,
3. aktiviert Busse und übergeordnete Geräte,
4. reaktiviert abhängige Geräte,
5. stellt zulässige DMA-Mappings wieder her,
6. gibt blockierte I/O-Pfade frei,
7. informiert Anwendungen und Systemdienste.

Treiber dürfen nicht voraussetzen, dass Hardwarezustände den Suspend unverändert überstehen.

---

## 11. Runtime Power Management

Geräte können während des normalen Betriebs automatisch in einen niedrigeren Energiezustand wechseln.

Ein Übergang ist möglich, wenn:

- keine I/O-Operation aktiv ist,
- keine Power-Anforderung das Gerät aktiv hält,
- abhängige Geräte den Übergang erlauben,
- die erwartete Ruhezeit den Übergangsaufwand rechtfertigt,
- der Treiber Runtime Power Management unterstützt.

Neue Arbeit aktiviert das Gerät bei Bedarf automatisch.

Die zusätzliche Wake-up-Latenz muss bei zeitkritischen Geräten berücksichtigt werden.

---

## 12. Power Requests

Prozesse, Systemdienste und Treiber können zeitlich begrenzte Energieanforderungen erstellen.

```c
status_t power_request_create(
    uint32_t request_type,
    const char *reason,
    cap_handle_t *request_handle
);
```

Mögliche Anforderungen:

| Typ | Bedeutung |
|---|---|
| `POWER_REQUEST_SYSTEM_REQUIRED` | System soll aktiv bleiben |
| `POWER_REQUEST_DISPLAY_REQUIRED` | Display soll aktiv bleiben |
| `POWER_REQUEST_DEVICE_REQUIRED` | Bestimmtes Gerät aktiv halten |
| `POWER_REQUEST_NO_SUSPEND` | Suspend vorübergehend verhindern |
| `POWER_REQUEST_PERFORMANCE` | Höhere Leistung anfordern |

Die Anforderung gilt nur, solange ihre Capability existiert oder bis ihr Timeout abläuft.

Dauerhafte Anforderungen ohne Begründung und Begrenzung sind nicht zulässig.

---

## 13. CPU-Energieverwaltung

Der Power Manager arbeitet mit dem Scheduler zusammen, um:

- inaktive CPUs in Ruhezustände zu versetzen,
- Frequenz und Leistung anzupassen,
- Last auf geeignete CPUs zu verteilen,
- unnötige CPU-Aufweckvorgänge zu vermeiden,
- thermische Grenzen einzuhalten.

Der Scheduler liefert Last- und Latenzinformationen. Der Power Manager entscheidet über die zulässige Energiestrategie.

Echtzeitprozesse können tiefere Ruhezustände begrenzen, wenn deren Wake-up-Latenz nicht mit den zugesicherten Fristen vereinbar ist.

---

## 14. Energieprofile

NovaOS bietet mindestens folgende Profile:

| Profil | Ziel |
|---|---|
| `POWER_SAVER` | Minimaler Energieverbrauch |
| `BALANCED` | Ausgewogenes Verhältnis |
| `PERFORMANCE` | Geringe Latenz und hohe Leistung |
| `CUSTOM` | Benutzer- oder systemspezifische Richtlinie |

Profile setzen Richtlinien, greifen aber nicht direkt auf Hardware zu.

Sicherheits- und Temperaturschutz haben immer Vorrang vor dem gewählten Leistungsprofil.

---

## 15. Wake-up-Quellen

Nur ausdrücklich zugelassene Geräte dürfen das System aus einem Energiesparzustand aufwecken.

Mögliche Wake-up-Quellen:

- Ein-/Ausschalter
- Tastatur oder Maus
- Netzwerkadapter
- Echtzeituhr
- Deckelsensor
- ausgewählte USB-Geräte
- Plattformcontroller

Wake-up-Rechte werden pro Gerät und Sicherheitskontext verwaltet.

Externe Geräte dürfen nicht automatisch als Wake-up-Quelle aktiviert werden.

---

## 16. Thermische Verwaltung

NovaOS überwacht verfügbare Temperatur- und Plattformdaten.

Bei kritischen Temperaturen werden gestaffelte Maßnahmen ausgeführt:

1. Leistung begrenzen,
2. Hintergrundlast reduzieren,
3. Geräte in niedrigere Energiezustände versetzen,
4. Benutzer oder Systemdienst warnen,
5. geordnet herunterfahren,
6. bei unmittelbarer Gefahr Notabschaltung auslösen.

Thermischer Schutz darf nicht durch Anwendungen deaktiviert werden.

---

## 17. Fehlerbehandlung

Mögliche Fehler sind:

- Gerät reagiert nicht auf Suspend,
- Resume schlägt fehl,
- Firmware liefert ungültige Zustände,
- Wake-up-Quelle funktioniert nicht,
- Treiber überschreitet das Zeitlimit,
- Gerät verschwindet während des Übergangs.

Der Power Manager kann abhängig von der Kritikalität:

- den Übergang wiederholen,
- das Gerät zurücksetzen,
- den Treiber neu starten,
- den System-Suspend abbrechen,
- das Gerät deaktivieren,
- einen sicheren Neustart oder Shutdown ausführen.

Ein nicht kritisches Gerät soll den Suspend nicht unbegrenzt verhindern.

---

## 18. Sicherheit und Datenschutz

Für Power Management gelten folgende Regeln:

- Energiezustandsänderungen benötigen entsprechende Capabilities.
- Unprivilegierte Prozesse dürfen das System nicht unbegrenzt aktiv halten.
- Gründe für Power Requests müssen begrenzt und diagnostizierbar sein.
- Wake-up-Quellen werden explizit autorisiert.
- DMA wird während Suspend und Geräteabschaltung kontrolliert.
- Sicherheitsdienste dürfen notwendige Energieanforderungen stellen.
- Nutzungsverhalten wird nicht für externe Telemetrie erfasst.

Lokale Statistiken müssen auf Energieoptimierung und Diagnose begrenzt bleiben.

---

## 19. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Übergang erfolgreich |
| `STATUS_NOT_SUPPORTED` | Energiezustand wird nicht unterstützt |
| `STATUS_DEVICE_BUSY` | Gerät kann derzeit nicht wechseln |
| `STATUS_TIMEOUT` | Treiber oder Gerät reagiert nicht rechtzeitig |
| `STATUS_ACCESS_DENIED` | Erforderliche Capability fehlt |
| `STATUS_DEPENDENCY_ACTIVE` | Abhängiges Gerät verhindert den Übergang |
| `STATUS_POWER_REQUEST_ACTIVE` | Eine gültige Anforderung blockiert den Übergang |
| `STATUS_DEVICE_FAILED` | Gerät oder Treiber ist ausgefallen |
| `STATUS_SUSPEND_ABORTED` | System-Suspend wurde abgebrochen |
| `STATUS_INVALID_STATE` | Übergang ist aus aktuellem Zustand nicht zulässig |

---

## 20. Audit und Diagnose

Audit- und Diagnosedaten können umfassen:

- System-Suspend und Resume,
- fehlgeschlagene Geräteübergänge,
- lang laufende Power Requests,
- Änderungen von Wake-up-Rechten,
- thermische Schutzmaßnahmen,
- erzwungene Geräteabschaltungen,
- Suspend-Abbrüche,
- administrative Profiländerungen.

Die Diagnose muss erkennen lassen, welcher Prozess, Treiber oder welches Gerät einen Energiezustand verhindert hat.

---

## 21. Alternative Ansätze

### 21.1 Energieverwaltung ausschließlich durch Treiber

**Vorteile:**

- geringer zentraler Aufwand

**Nachteile:**

- fehlende globale Koordination,
- falsche Reihenfolge abhängiger Geräte,
- uneinheitliches Verhalten.

**Entscheidung:** Abgelehnt.

### 21.2 Nur globale Systemzustände

**Vorteile:**

- einfache Implementierung

**Nachteile:**

- unnötiger Energieverbrauch im laufenden Betrieb,
- keine gerätespezifische Optimierung.

**Entscheidung:** Abgelehnt. NovaOS unterstützt zusätzlich Runtime Power Management.

### 21.3 ACPI direkt als öffentliche API

**Vorteile:**

- direkte Abbildung verbreiteter PC-Hardware

**Nachteile:**

- plattformabhängig,
- ungeeignet für andere Firmware- und Geräteplattformen.

**Entscheidung:** Abgelehnt. ACPI ist ein Backend des plattformunabhängigen Power Managers.

---

## 22. Konsequenzen

### Positive Konsequenzen

- einheitliche Energieverwaltung,
- geringerer Stromverbrauch,
- Unterstützung für Suspend und Resume,
- koordinierte Geräteabhängigkeiten,
- kontrollierte Wake-up-Quellen,
- Integration mit Scheduler und Device Manager,
- Unterstützung verschiedener Hardwareplattformen.

### Negative Konsequenzen

- komplexe Zustandsübergänge,
- zusätzlicher Implementierungsaufwand für Treiber,
- fehlerhafte Firmware benötigt Sonderbehandlung,
- aggressive Energiesparmaßnahmen können Latenzen erhöhen.

### Risiken

- Datenverlust bei fehlerhaftem Suspend,
- Geräteausfälle nach Resume,
- Hängenbleiben durch fehlerhafte Treiber,
- übermäßige Power Requests,
- unzuverlässige ältere Hardware.

---

## 23. Implementierungsrichtlinien

- Power-Übergänge müssen definierte Timeouts besitzen.
- Geräteabhängigkeiten bestimmen die Suspend- und Resume-Reihenfolge.
- DMA muss vor Geräteabschaltung beendet oder widerrufen werden.
- Power Requests müssen über Capabilities und Lebenszeitgrenzen verfügen.
- Treiber müssen Hardware nach Resume erneut validieren.
- Thermischer Schutz hat Vorrang vor Leistungsanforderungen.
- ACPI-Details dürfen nicht Teil der öffentlichen Power-API sein.
- Fehlgeschlagene Übergänge dürfen den Gerätegraph nicht inkonsistent hinterlassen.

---

## 24. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- System-Suspend und Resume,
- Runtime Power Management,
- Geräteabhängigkeiten,
- aktive I/O während Suspend,
- Power Requests und Timeouts,
- Wake-up-Quellen,
- Treiber-Timeouts,
- Geräteausfall nach Resume,
- Hotplug während Energieübergängen,
- Systeme ohne ACPI,
- CPU-Ruhezustände,
- Echtzeitlast,
- thermische Schutzmaßnahmen,
- wiederholte Suspend-/Resume-Zyklen.

---

## 25. Verbindliche Invarianten

1. Der Power Manager koordiniert globale Energieentscheidungen.
2. Treiber führen nur angeordnete gerätespezifische Übergänge aus.
3. Untergeordnete Geräte werden vor ihren Eltern suspendiert.
4. Übergeordnete Geräte werden vor ihren Kindern reaktiviert.
5. Laufende DMA-Zugriffe werden vor Suspend oder Abschaltung kontrolliert beendet.
6. Power Requests sind capability-geschützt und zeitlich begrenzbar.
7. Wake-up-Quellen müssen ausdrücklich autorisiert sein.
8. Thermischer Schutz kann nicht durch Anwendungen außer Kraft gesetzt werden.
9. ACPI ist ein Backend und nicht die öffentliche NovaOS-API.
10. Fehlgeschlagene Übergänge dürfen keine inkonsistenten Gerätezustände hinterlassen.

---

## 26. Zusammenfassung

NovaOS verwendet einen zentralen Power Manager für System-, Geräte- und CPU-Energiezustände.

Der Power Manager koordiniert Suspend, Resume, Runtime Power Management, Wake-up-Quellen und thermischen Schutz. Device Manager und Treiber führen die konkreten Geräteübergänge in einer durch den Gerätegraph bestimmten Reihenfolge aus.

Die Architektur verbindet Energieeffizienz mit sicheren Zustandsübergängen und bleibt unabhängig von ACPI oder einer bestimmten Hardwareplattform.