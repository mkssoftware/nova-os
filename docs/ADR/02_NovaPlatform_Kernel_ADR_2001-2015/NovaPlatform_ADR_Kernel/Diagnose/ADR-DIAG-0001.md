# ADR-DIAG-0001 – Kernel Logging

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DIAG-0001 |
| Titel | Kernel Logging |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernel-Diagnose und lokale Telemetrie |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-KERNEL-0101, ADR-KERNEL-0104, ADR-SEC-0008, ADR-SEC-0009, ADR-SEC-0010 |
| Nachfolgende Spezifikation | NPSPEC-DIAG-KERNEL-LOGGING |

---

## 1. Kontext

NovaOS benötigt eine zuverlässige Kernelprotokollierung für:

- frühen Systemstart
- Treiberinitialisierung
- Gerätefehler
- Speicher- und Schedulerprobleme
- Sicherheitsverletzungen
- Diagnose von Abstürzen
- Recovery
- Entwicklung und Tests

Kernel Logging muss bereits funktionieren, bevor Dateisysteme, Userspace-Dienste oder die grafische Oberfläche verfügbar sind.

---

## 2. Problemstellung

Der Kernel kann Logeinträge aus sehr unterschiedlichen Kontexten erzeugen:

- normaler Threadkontext
- Systemaufrufe
- Interrupt Handler
- Exception Handler
- früher Bootprozess
- Speicherknappheit
- Kernel Panic
- Mehrkernbetrieb

Die Protokollierung darf dabei keine Deadlocks, unbegrenzte Speicherbelegung oder rekursive Fehler verursachen.

Zusätzlich können Logs sensible Daten enthalten und müssen deshalb datensparsam behandelt werden.

---

## 3. Entscheidung

NovaOS implementiert ein strukturiertes, nicht blockierendes Kernel-Logging-System mit CPU-lokalen Ringpuffern.

Der Kernel schreibt Logeinträge zunächst in speicherbasierte Buffer. Ein privilegierter Userspace-Diagnosedienst kann sie später auslesen, filtern und in lokale persistente Logs übertragen.

Für frühen Boot, Recovery und Kernel Panic stehen unabhängige Ausgabepfade zur Verfügung.

---

## 4. Architektur

```text
Kernelkomponente
      |
      v
CPU-lokaler Ringpuffer
      |
      v
Globaler Log-Reader
      |
      +--> lokaler Diagnosedienst
      +--> Debug-Konsole
      +--> Panic-Dump
      +--> Recovery
```

Ein langsamer oder ausgefallener Log-Reader darf den Kernel nicht blockieren.

---

## 5. Logstufen

NovaOS definiert folgende Logstufen:

| Stufe | Bedeutung |
|---|---|
| `TRACE` | sehr detaillierte Ablaufdiagnose |
| `DEBUG` | Entwicklungs- und Debugdaten |
| `INFO` | normale Systemereignisse |
| `NOTICE` | bedeutende, aber erwartete Zustandsänderung |
| `WARNING` | ungewöhnlicher oder degradierter Zustand |
| `ERROR` | fehlgeschlagene Operation |
| `CRITICAL` | schwerer Subsystemfehler |
| `PANIC` | nicht fortsetzbarer Kernelzustand |

Produktionssysteme erfassen `TRACE` und `DEBUG` nur, wenn dies ausdrücklich aktiviert wurde.

Sicherheitsereignisse werden zusätzlich durch das Audit-System behandelt und nicht allein über Logstufen definiert.

---

## 6. Kategorien

Jeder Eintrag besitzt eine Kategorie.

Vorgesehene Kategorien sind beispielsweise:

```text
boot
kernel
memory
scheduler
process
ipc
vfs
filesystem
device
driver
network
power
security
audit
recovery
debug
```

Kategorien verwenden stabile numerische IDs und menschenlesbare Namen.

Neue Kategorien können versioniert ergänzt werden.

---

## 7. Strukturierter Logeintrag

```c
typedef struct kernel_log_record {
    uint32_t structure_size;
    uint16_t version;
    uint8_t severity;
    uint8_t flags;

    uint64_t sequence_number;
    uint64_t timestamp_ns;

    uint32_t cpu_id;
    uint32_t category_id;

    uint64_t thread_id;
    uint64_t process_id;
    uint64_t event_id;

    uint32_t payload_size;
    uint32_t field_count;
} kernel_log_record_t;
```

Der Payload kann enthalten:

- kurze Textnachricht
- typisierte Schlüssel-Wert-Felder
- Fehlercode
- Objekt-ID
- Geräte- oder Subsystemstatus
- Quellmodul und Ereignistyp

Kernelzeiger werden nicht als gewöhnliche Logfelder ausgegeben.

---

## 8. Ereignis-IDs

Wiederkehrende Kernelereignisse erhalten stabile Ereignis-IDs.

Beispiele:

```text
MEMORY_ALLOCATION_FAILED
DRIVER_START_FAILED
DEVICE_REMOVED
FILESYSTEM_WRITEBACK_ERROR
NETWORK_PACKET_DROPPED
SUSPEND_ABORTED
THERMAL_LIMIT_REACHED
```

Ereignis-IDs ermöglichen:

- sprachunabhängige Auswertung
- strukturierte Filterung
- stabile Diagnose
- maschinenlesbare Tests
- lokale statistische Zusammenfassung

Freitext ergänzt die Ereignis-ID, ersetzt sie aber nicht.

---

## 9. Ringpuffer

Jede CPU besitzt einen eigenen begrenzten Ringpuffer.

Vorteile:

- keine globale Sperre im häufigen Schreibpfad
- geringe Mehrkernkonkurrenz
- Logging aus Interruptkontexten
- klare Begrenzung des Speicherverbrauchs
- bessere Reihenfolge pro CPU

Bei vollem Buffer werden abhängig von Stufe und Richtlinie ältere oder neue Einträge verworfen.

Der Kernel darf nicht unbegrenzt auf freien Logspeicher warten.

---

## 10. Globale Reihenfolge

Jeder Logeintrag erhält eine globale oder global vergleichbare Sequenznummer.

Da CPU-lokale Buffer parallel schreiben, wird keine perfekte physische Schreibreihenfolge vorausgesetzt.

Der Log-Reader rekonstruiert eine bestmögliche Reihenfolge anhand von:

- Sequenznummer
- monotonem Zeitstempel
- CPU-ID
- CPU-lokaler Reihenfolge

Bei unsynchronisierten Zeitquellen bleibt die Sequenznummer die maßgebliche Ordnungsinformation.

---

## 11. Zeitstempel

Kernel-Logs verwenden eine monotone Zeitbasis.

Optional kann der Userspace-Diagnosedienst zusätzlich eine Zuordnung zur Echtzeit herstellen.

Dadurch bleiben Logs auch korrekt sortierbar, wenn:

- die Systemuhr geändert wird,
- keine Echtzeituhr verfügbar ist,
- Netzwerkzeit noch nicht synchronisiert wurde,
- Suspend oder Hibernate stattfindet.

Die Zeitbasis und ihre Auflösung werden in den Logmetadaten angegeben.

---

## 12. Schreib-API

```c
status_t kernel_log_write(
    uint8_t severity,
    uint32_t category_id,
    uint64_t event_id,
    const kernel_log_field_t *fields,
    uint32_t field_count
);
```

Zusätzlich kann eine eingeschränkte Format-API bereitgestellt werden:

```c
void kernel_log_message(
    uint8_t severity,
    uint32_t category_id,
    const char *message
);
```

Formatierung muss begrenzt sein und darf keine dynamische Speicherallokation voraussetzen.

Formatstrings aus dem Userspace dürfen niemals direkt verwendet werden.

---

## 13. Kontextregeln

### Normaler Threadkontext

Darf strukturierte Felder und begrenzte Formatierung verwenden.

### Interruptkontext

Darf nicht blockieren und keine gewöhnliche Speicherallokation ausführen.

### Exception- und Panic-Kontext

Verwendet einen minimalen, reentranten Notfallpfad.

### Früher Bootprozess

Verwendet statisch reservierte Early-Boot-Buffer, die später in das normale Logging übernommen werden.

---

## 14. Early Boot Logging

Vor Initialisierung des vollständigen Memory Managers verwendet NovaOS einen statisch reservierten Early-Boot-Ringpuffer.

Er erfasst mindestens:

- Boot-Handoff-Prüfung
- Speicherkarte
- CPU-Erkennung
- Seitentabellenaufbau
- Interruptinitialisierung
- frühe Treiber
- Kernel-ASLR-Status
- Secure-Startup-Ergebnis

Nach Aktivierung des regulären Logsystems werden die Einträge übernommen und als Early-Boot-Ereignisse markiert.

---

## 15. Panic Logging

Bei einer Kernel Panic wechselt das Logging in einen minimalen Panic-Modus.

Dieser darf:

- keine blockierenden Sperren verwenden
- keinen gewöhnlichen Heap benötigen
- keine Dateisystemoperationen voraussetzen
- keine fehlerhaften Treiber erneut aufrufen
- keine rekursive Panic auslösen

Der Panic-Pfad sichert nach Möglichkeit:

- Panic-Grund
- CPU und Thread
- Registerzustand
- Stacktrace
- letzte kritische Logeinträge
- Kernel-Build-ID
- Systemgeneration

Die Ausgabe wird an das Kernel-Panic-System übergeben.

---

## 16. Persistente Speicherung

Der Kernel selbst schreibt gewöhnliche Logs nicht direkt in ein Dateisystem.

Ein privilegierter lokaler Diagnosedienst:

1. liest Logeinträge aus dem Kernel,
2. validiert Struktur und Größe,
3. filtert nach Richtlinie,
4. redigiert sensible Felder,
5. speichert ausgewählte Einträge lokal,
6. rotiert und begrenzt Logdateien.

Dadurch bleibt der Kernel unabhängig von VFS- und Dateisystemfehlern.

---

## 17. Log-Reader-API

```c
status_t kernel_log_open(
    uint32_t flags,
    cap_handle_t *reader_handle
);

status_t kernel_log_read(
    cap_handle_t reader_handle,
    void *buffer,
    uint32_t capacity,
    uint32_t *bytes_read,
    uint64_t timeout_ns
);
```

Ein Reader besitzt:

- eigene Leseposition
- Kategorie- und Stufenfilter
- Sichtbarkeitsrechte
- Verlustzähler
- Startgeneration

Langsame Reader verhindern nicht das Überschreiben alter Ringpuffereinträge.

---

## 18. Zugriffsrechte

| Recht | Bedeutung |
|---|---|
| `KLOG_RIGHT_READ_PUBLIC` | freigegebene Systemlogs lesen |
| `KLOG_RIGHT_READ_KERNEL` | detaillierte Kernelereignisse lesen |
| `KLOG_RIGHT_READ_SECURITY` | geschützte Sicherheitsereignisse lesen |
| `KLOG_RIGHT_SET_LEVEL` | Logstufen konfigurieren |
| `KLOG_RIGHT_SET_FILTER` | Kategorien und Filter verwalten |
| `KLOG_RIGHT_CLEAR` | lokale Buffer kontrolliert leeren |
| `KLOG_RIGHT_EXPORT` | Logs ausdrücklich exportieren |
| `KLOG_RIGHT_ADMIN` | Logging vollständig verwalten |

Gewöhnliche Anwendungen erhalten keinen vollständigen Kernel-Logzugriff.

---

## 19. Datenschutz und Geheimnisse

Folgende Inhalte dürfen nicht in gewöhnlichen Kernel-Logs erscheinen:

- Passwörter
- private Schlüssel
- Sitzungstoken
- vollständige Speicherinhalte
- TLS- oder VPN-Schlüssel
- unnötige Dateiinhalte
- unmaskierte personenbezogene Daten
- Klartext sensibler Netzwerkpakete
- Kernelzeiger im Produktionsmodus

Adressen und Identifikatoren werden gekürzt, gehasht oder vollständig ausgelassen, wenn sie für die Diagnose nicht erforderlich sind.

---

## 20. Kernelzeiger

Kernelzeiger können Layoutinformationen offenlegen und Kernel-ASLR schwächen.

Deshalb gilt:

- Produktionslogs enthalten keine rohen Kernelzeiger.
- Objektinformationen verwenden stabile Diagnose-IDs.
- Stacktraces verwenden intern Modul- und Offsetinformationen.
- Symbolauflösung erfolgt nur in autorisierten Diagnosewerkzeugen.
- Secure Debugging kann detailliertere Daten nur in einem ausdrücklich aktivierten Modus freigeben.

Eine Formatoption wie `%p` muss im Produktionsmodus automatisch geschützt dargestellt werden.

---

## 21. Rate Limiting

Wiederholte identische Ereignisse werden rate-limitiert.

Die Begrenzung kann erfolgen pro:

- Ereignis-ID
- Kategorie
- Gerät
- Prozess
- CPU
- Quellmodul

Unterdrückte Einträge werden zusammengefasst:

```text
DRIVER_TIMEOUT repeated 824 times
```

`CRITICAL`- und `PANIC`-Ereignisse dürfen nicht vollständig durch gewöhnliche Rate Limits verschwinden.

---

## 22. Speicher- und Größenlimits

Begrenzt werden:

- Ringpuffergröße pro CPU
- maximale Recordgröße
- Anzahl strukturierter Felder
- Textlänge
- Anzahl aktiver Reader
- persistente Loggröße
- Aufbewahrungsdauer
- Ereignisrate
- Debug-Buffer

Ein einzelner Logeintrag darf niemals den vollständigen Ringpuffer beanspruchen.

Übergroße Einträge werden gekürzt oder mit einem Fehler verworfen.

---

## 23. Logverlust

Da Kernel Logging nicht blockierend arbeitet, können Einträge verloren gehen.

Der Verlust wird sichtbar gemacht durch:

- Sequenzlücken
- Drop-Zähler pro CPU
- Reader-spezifische Verlustmarker
- zusammengefasste Überlastungsereignisse

Das System darf keine vollständige und lückenlose Aufzeichnung behaupten, wenn Buffer überlaufen sind.

Sicherheitsrelevante, revisionspflichtige Ereignisse verwenden zusätzlich das getrennte Audit-Subsystem.

---

## 24. Audit-Abgrenzung

Kernel Logging und Audit besitzen unterschiedliche Ziele:

| Kernel Logging | Audit |
|---|---|
| technische Diagnose | sicherheitsrelevante Nachvollziehbarkeit |
| darf bei Überlastung Einträge verlieren | besitzt strengere Verlustregeln |
| enthält Debug- und Fehlerdaten | enthält autorisierte Sicherheitsereignisse |
| kurze Aufbewahrung | richtliniengesteuerte Aufbewahrung |
| entwicklungsorientiert | revisions- und sicherheitsorientiert |

Ein sicherheitskritisches Ereignis kann sowohl einen Log- als auch einen Audit-Eintrag erzeugen.

Der Logeintrag ersetzt niemals den Audit-Datensatz.

---

## 25. Ausgabeziele

Unterstützte Ausgabeziele können sein:

- speicherbasierter Ringpuffer
- serielle Debug-Konsole
- lokale Diagnoseoberfläche
- Panic- oder Crash-Dump
- Recovery-Konsole
- lokaler persistenter Logdienst

Netzwerkexport ist standardmäßig deaktiviert.

Ein externer Export benötigt eine ausdrückliche administrative Konfiguration, Verschlüsselung und Datenschutzrichtlinie.

---

## 26. Sicherheit

Für Kernel Logging gelten folgende Regeln:

- Logschreiben darf sicherheitskritische Kernelpfade nicht blockieren.
- Userspace kann keine Kernel-Logeinträge als vertrauenswürdig fälschen.
- Reader benötigen Capabilities.
- Logdaten werden nach Sicherheitsstufe gefiltert.
- sensible Inhalte und rohe Kernelzeiger werden entfernt.
- Debug Logging ist im Produktionsmodus eingeschränkt.
- externe Übertragung ist standardmäßig deaktiviert.
- Logkonfiguration darf Panic- und Critical-Ereignisse nicht vollständig unterdrücken.
- Buffer und Readerzustände sind begrenzt.

---

## 27. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_LOG_RECORD_TOO_LARGE` | Eintrag überschreitet das Limit |
| `STATUS_LOG_BUFFER_FULL` | Eintrag konnte nicht gespeichert werden |
| `STATUS_LOG_RECORD_LOST` | ältere Einträge wurden überschrieben |
| `STATUS_INVALID_LOG_LEVEL` | Logstufe ist ungültig |
| `STATUS_INVALID_LOG_CATEGORY` | Kategorie ist unbekannt |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_READER_OVERRUN` | Reader wurde vom Ringpuffer überholt |
| `STATUS_LIMIT_EXCEEDED` | Reader- oder Ressourcenlimit überschritten |
| `STATUS_NOT_SUPPORTED` | Ausgabeziel wird nicht unterstützt |

Der Kernel darf Fehler beim gewöhnlichen Logging nicht rekursiv über denselben fehlerhaften Pfad protokollieren.

---

## 28. Alternative Ansätze

### 28.1 Ein globaler Logpuffer mit Sperre

**Vorteile:**

- einfache Reihenfolge
- einfache Implementierung

**Nachteile:**

- hohe Mehrkernkonkurrenz
- Deadlock-Risiko in Interrupt- und Panic-Kontexten

**Entscheidung:** Abgelehnt. NovaOS verwendet CPU-lokale Ringpuffer.

### 28.2 Direkte Dateischreibvorgänge aus dem Kernel

**Vorteile:**

- Logs sind sofort persistent

**Nachteile:**

- Abhängigkeit von VFS und Treibern
- Blockierung und rekursive Fehler möglich

**Entscheidung:** Abgelehnt.

### 28.3 Ausschließlich Textlogs

**Vorteile:**

- einfach lesbar

**Nachteile:**

- schwer zuverlässig auszuwerten
- uneinheitliche Felder

**Entscheidung:** Strukturierte Records mit optionalem Text werden verwendet.

---

## 29. Konsequenzen

### Positive Konsequenzen

- Logging bereits im frühen Bootprozess
- nicht blockierender Mehrkern-Schreibpfad
- strukturierte und maschinenlesbare Ereignisse
- sichere Panic-Integration
- begrenzter Speicherverbrauch
- Datenschutz- und ASLR-Schutz
- getrennte technische Logs und Sicherheits-Audits

### Negative Konsequenzen

- CPU-lokale Buffer erschweren die globale Sortierung
- nicht blockierendes Logging kann Einträge verlieren
- strukturierte Ereignisse benötigen stabile IDs und Schemata
- persistente Speicherung benötigt einen Userspace-Dienst

### Risiken

- Verlust wichtiger Einträge bei hoher Ereignisrate
- sensible Daten durch ungeeignete Logfelder
- Rekursion bei Loggingfehlern
- Performanceverlust durch übermäßiges Debug Logging
- falsche Zeitreihenfolge zwischen CPUs
- unkontrolliertes Wachstum persistenter Logs

---

## 30. Implementierungsrichtlinien

- Der normale Schreibpfad verwendet CPU-lokale Ringpuffer.
- Logging aus Interrupt- und Panic-Kontexten darf nicht blockieren.
- Early-Boot-Logs werden später in das normale System übernommen.
- Wiederkehrende Ereignisse erhalten stabile Ereignis-IDs.
- Logrecords besitzen feste Größen- und Feldlimits.
- Rohe Kernelzeiger werden im Produktionsmodus nicht ausgegeben.
- Secrets und sensible Nutzdaten dürfen nicht protokolliert werden.
- Langsame Reader dürfen den Kernel nicht blockieren.
- Überläufe und verlorene Records müssen sichtbar markiert werden.
- Persistente Speicherung erfolgt durch einen privilegierten lokalen Dienst.
- Audit und Kernel Logging bleiben getrennte Subsysteme.

---

## 31. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Early-Boot-Logging
- CPU-lokale Ringpuffer
- parallele Schreiber auf mehreren CPUs
- Interrupt- und Exceptionkontext
- Panic-Modus
- Bufferüberlauf
- langsame Reader
- Sequenzlücken und Verlustmarker
- Rate Limiting
- strukturierte Felder
- Größenlimits
- Zeiger- und Secret-Redaktion
- persistente Userspace-Übernahme
- Suspend, Hibernate und Resume
- rekursive Loggingfehler

---

## 32. Verbindliche Invarianten

1. Kernel Logging funktioniert bereits vor dem vollständigen Userspace.
2. Der normale Schreibpfad ist nicht blockierend.
3. Jede CPU verwendet einen eigenen begrenzten Ringpuffer.
4. Logeinträge besitzen strukturierte Metadaten und stabile Ereignis-IDs.
5. Rohe Kernelzeiger erscheinen nicht in Produktionslogs.
6. Passwörter, Schlüssel und sensible Nutzdaten dürfen nicht geloggt werden.
7. Langsame Reader blockieren keine Kernelkomponenten.
8. Logverlust wird durch Zähler oder Sequenzlücken sichtbar gemacht.
9. Persistente Speicherung erfolgt außerhalb des gewöhnlichen Kernelpfads.
10. Kernel Logging ersetzt niemals das sicherheitsrelevante Audit-System.

---

## 33. Zusammenfassung

NovaOS implementiert ein strukturiertes, nicht blockierendes Kernel-Logging-System mit CPU-lokalen Ringpuffern.

Das System funktioniert im frühen Bootprozess, im normalen Mehrkernbetrieb, aus Interruptkontexten und während einer Kernel Panic. Ein privilegierter Userspace-Diagnosedienst übernimmt ausgewählte Einträge für die lokale persistente Speicherung.

Feste Ressourcenlimits, Rate Limiting, Secret-Redaktion und der Schutz von Kernelzeigern ermöglichen aussagekräftige Diagnose, ohne Stabilität, Datenschutz oder Kernel-ASLR unnötig zu gefährden.