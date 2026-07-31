# ADR-SEC-0009 – Kernel Panic Policy

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0009 |
| Titel | Kernel Panic Policy |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Fehlerbehandlung / Recovery |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0003, ADR-SEC-0004, ADR-SEC-0005, ADR-SEC-0006, ADR-SEC-0008, ADR-KERNEL-0104 |
| Nachfolgende Spezifikation | NPSPEC-KERNELPANIC-0001 |

---

## 1. Kontext

Ein Kernel-Panic tritt ein, wenn NovaOS einen Fehler erkennt, nach dem ein sicherer und konsistenter Weiterbetrieb nicht mehr gewährleistet werden kann.

Mögliche Ursachen sind:

- beschädigte Kernelstacks,
- ungültige Seitentabellen,
- nicht wiederherstellbare Speicherfehler,
- verletzte Scheduler-Invarianten,
- beschädigte Kernelobjekte,
- unerwartete SMEP- oder SMAP-Verstöße,
- Ausführung aus NX-Speicher,
- fehlgeschlagene Kernel-Sicherheitsprüfungen,
- Double Faults,
- Machine Checks,
- Deadlocks kritischer Kernelkomponenten,
- Watchdog-Lockups,
- fehlerhafte Kernelmodule,
- Integritätsverletzungen,
- nicht behandelbare Hardwarefehler.

Ein Kernel-Panic ist nicht nur ein technischer Absturz. Er ist eine sicherheitsrelevante Zustandsentscheidung.

Nach einer schwerwiegenden Kernelkorruption kann der Kernel nicht mehr zuverlässig beurteilen:

- welche Daten bereits verändert wurden,
- ob Sicherheitsgrenzen noch gelten,
- ob Dateisystemschreibvorgänge sicher sind,
- ob weitere Prozesse korrekt isoliert werden,
- ob Diagnoseinformationen vertrauenswürdig sind,
- ob ein normaler Shutdown noch möglich ist.

Die Panic Policy muss deshalb Sicherheit, Diagnose, Datenintegrität, Verfügbarkeit und Recovery kontrolliert gegeneinander abwägen.

---

## 2. Problemstellung

Ohne eine verbindliche Panic Policy könnten verschiedene Kernelkomponenten bei kritischen Fehlern unterschiedlich reagieren.

Mögliche problematische Reaktionen wären:

- Weiterbetrieb trotz beschädigter Kernelstrukturen,
- sofortiger Neustart ohne Diagnose,
- unbegrenztes Hängen auf dem Panic-Bildschirm,
- unsicheres Schreiben auf Dateisysteme,
- Offenlegung von Kerneladressen oder Geheimnissen,
- mehrfach gleichzeitig gestartete Panic-Pfade,
- Deadlocks zwischen CPUs,
- nicht nachvollziehbare Neustartschleifen,
- Aktivierung eines ungeprüften Recovery-Systems,
- unkontrollierte Aktorzustände bei Robotik- oder Maschinenanwendungen.

NovaOS benötigt deshalb eine zentrale Panic Policy mit:

- eindeutigen Auslösekriterien,
- einem Single-Panic-Owner,
- mehrstufiger Diagnose,
- sicherer Mehrkernkoordination,
- kontrollierter Crash-Dump-Erzeugung,
- definierter Neustart- und Recovery-Strategie.

---

## 3. Entscheidungsziele

Die Kernel Panic Policy soll:

1. unsicheren Kernelweiterbetrieb verhindern,
2. genau eine CPU als Panic Owner bestimmen,
3. andere CPUs kontrolliert stoppen oder isolieren,
4. minimale Diagnose auch bei schwerer Korruption ermöglichen,
5. Geheimnisse und Kerneladressen schützen,
6. Crash-Dumps kontrolliert und nach Möglichkeit verschlüsselt speichern,
7. rekursive Panic-Situationen behandeln,
8. Hardware-Watchdogs und Neustartmechanismen integrieren,
9. Bootfehlerzähler und Recovery auslösen können,
10. Echtzeit- und Safety-Systeme in einen definierten Zustand überführen,
11. eine klare Benutzeranzeige bereitstellen,
12. Panic-Ursachen lokal auditierbar machen.

---

## 4. Begriffsdefinitionen

### 4.1 Kernel Panic

Ein Kernel-Panic ist der kontrollierte Übergang in einen nicht rückkehrenden Kernelnotfallzustand.

---

### 4.2 Recoverable Fault

Ein Recoverable Fault ist ein Fehler, den das zuständige Subsystem vollständig isolieren und behandeln kann, ohne Kernelinvarianten zu verletzen.

Er löst keinen Kernel-Panic aus.

---

### 4.3 Kernel Bug Check

Ein Kernel Bug Check ist eine erkannte Verletzung einer Kernelinvariante, die zur Panic-Eskalation führen kann.

---

### 4.4 Panic Owner

Der Panic Owner ist die CPU, die den primären Panic-Pfad koordiniert.

---

### 4.5 Secondary Panic CPU

Eine Secondary Panic CPU ist eine weitere CPU, die während eines bereits laufenden Panic-Vorgangs ebenfalls einen kritischen Fehler erkennt oder den Panic-Stopp bestätigt.

---

### 4.6 Nested Panic

Eine Nested Panic tritt auf, wenn innerhalb des Panic-Pfads ein weiterer nicht behandelbarer Fehler entsteht.

---

### 4.7 Crash Dump

Ein Crash Dump ist eine geschützte Momentaufnahme ausgewählter Kernel-, CPU- und Systemzustände zur späteren Diagnose.

---

### 4.8 Safe State

Ein Safe State ist ein durch Plattform oder Anwendung definierter Zustand, der gefährliche physische Auswirkungen begrenzen soll.

Ein Kernel-Panic ist nicht automatisch ein Safe State.

---

## 5. Grundsatz

NovaOS verwendet folgende Grundregel:

> Wenn die Integrität zentraler Kernel- oder Sicherheitsinvarianten nicht mehr zuverlässig gewährleistet werden kann, wird der normale Betrieb beendet.

Ein Kernel-Panic kehrt niemals zur unterbrochenen Kernel- oder Userspace-Ausführung zurück.

NovaOS versucht nicht, eine schwerwiegend beschädigte Kernelinstanz als normal funktionsfähig weiterzubetreiben.

---

## 6. Fehlerklassen

NovaOS unterscheidet mindestens folgende Fehlerklassen:

| Klasse | Beschreibung | Standardreaktion |
|---|---|---|
| `USER_FAULT` | Fehler eines Userspace-Prozesses | Prozess behandeln oder beenden |
| `DRIVER_RECOVERABLE` | isolierbarer Treiberfehler | Treiber neu starten oder Gerät isolieren |
| `SUBSYSTEM_DEGRADED` | begrenzter Subsystemfehler | Funktion deaktivieren und melden |
| `KERNEL_BUG` | verletzte Kernelinvariante | Panic |
| `SECURITY_VIOLATION` | mögliche Kernelkompromittierung | sofortige Panic |
| `HARDWARE_FATAL` | nicht korrigierbarer Hardwarefehler | Panic oder Plattformstopp |
| `SAFETY_CRITICAL` | Gefahr für physisches System | Safe State und Panic |
| `PANIC_RECURSION` | Fehler im Panic-Pfad | Minimal-Panic |

Nicht jeder Fehler im Kernelmodus führt automatisch zur Panic. Entscheidend ist, ob der Fehler vollständig und sicher isoliert werden kann.

---

## 7. Panic-Auslöser

Verbindliche Panic-Auslöser sind mindestens:

- Stack-Protector-Verletzung im Kernel,
- ungültiger Kernelstack,
- Double Fault,
- Ausführung des Kernels aus einer NX-Seite,
- SMEP-Verstoß,
- unerwarteter SMAP-Verstoß außerhalb Uaccess,
- falsche Kernel-Seitentabellensicht,
- beschädigte Scheduler-Run-Queue,
- derselbe Thread gleichzeitig auf mehreren CPUs,
- ungültiger Context-Switch-Zustand,
- beschädigte Capability-Kernstrukturen,
- nicht wiederherstellbarer Kernelheap-Schaden,
- kritischer Seitentabellenfehler,
- erkannte Manipulation des laufenden Kernelcodes,
- nicht korrigierbarer Machine Check,
- Watchdog-bestätigter Hard Lockup,
- explizite `nova_panic()`-Anforderung einer kritischen Kernelkomponente.

---

## 8. Keine Panic bei normalen Userspace-Fehlern

Folgende Fehler lösen normalerweise keinen Kernel-Panic aus:

- ungültiger Userspace-Zeiger,
- Userspace Page Fault,
- Division durch null im Userspace,
- ungültige Instruktion im Userspace,
- Prozesszugriff ohne Capability,
- fehlgeschlagene Speicherallokation eines normalen Prozesses,
- ungültiger Systemaufrufparameter,
- Prozessabsturz,
- Userspace Stack Overflow.

Solche Fehler werden durch Prozess-, Exception- oder Sicherheitsrichtlinien behandelt.

---

## 9. Isolierbare Treiberfehler

Ein Treiberfehler muss nicht zwangsläufig den gesamten Kernel stoppen, wenn:

- der Treiber in einem isolierten Userspace-Dienst läuft,
- das Gerät über eine IOMMU begrenzt ist,
- keine Kernelstrukturen beschädigt wurden,
- Ressourcen vollständig widerrufen werden können,
- der Treiber kontrolliert neu gestartet werden kann.

Ein Fehler eines privilegierten In-Kernel-Treibers führt dagegen zur Panic, wenn Speicher- oder Kontrollflusskorruption nicht ausgeschlossen werden kann.

---

## 10. Keine allgemeine Oops-Fortsetzung

NovaOS verwendet keinen allgemeinen „Oops und weiter“-Modus für beliebige Kernelverletzungen.

Eine bloße Warnung mit anschließendem Weiterbetrieb ist nur zulässig, wenn:

- die betroffene Invariante ausdrücklich als nicht fatal klassifiziert ist,
- keine Speicher- oder Sicherheitskorruption möglich ist,
- ein definierter Degraded Mode existiert,
- das Ereignis auditiert wird.

Unbekannte Kernelverletzungen werden nicht als harmlos angenommen.

---

## 11. Panic-Zustandsautomat

Der globale Panic-Zustand lautet:

```text
NORMAL
   ↓
PANIC_ELECTING
   ↓
PANIC_PRIMARY
   ↓
PANIC_STOPPING_CPUS
   ↓
PANIC_CAPTURING
   ↓
PANIC_FINALIZING
   ↓
PANIC_REBOOT
   oder
PANIC_HALT
```

Bei einem Fehler im Panic-Pfad:

```text
beliebiger Panic-Zustand
        ↓
PANIC_MINIMAL
        ↓
REBOOT oder HALT
```

Zustandsübergänge sind atomar.

---

## 12. Panic Owner

Die erste CPU, die den globalen Panic-Zustand erfolgreich von `NORMAL` auf `PANIC_ELECTING` setzt, wird Panic Owner.

Konzeptionell:

```c
if (atomic_compare_exchange(
        &global_panic_state,
        NOVA_PANIC_NORMAL,
        NOVA_PANIC_ELECTING))
{
    current_cpu->is_panic_owner = true;
}
else
{
    nova_panic_secondary_cpu();
}
```

Nur der Panic Owner führt den vollständigen primären Panic-Pfad aus.

---

## 13. Gleichzeitige Panic-Ereignisse

Erkennen mehrere CPUs gleichzeitig kritische Fehler:

- genau eine CPU wird Panic Owner,
- alle anderen CPUs wechseln in den Secondary-Panic-Pfad,
- ihre lokalen Fehlerursachen können in CPU-lokalen Notfallrecords gespeichert werden,
- sie versuchen nicht, gleichzeitig denselben Crash Dump zu schreiben.

Die globale Panic-Ursache bleibt die des Panic Owners. Zusätzliche Ursachen werden als sekundäre Ereignisse erfasst.

---

## 14. Secondary-Panic-Pfad

Eine Secondary Panic CPU:

1. deaktiviert normale Scheduleraktivität,
2. schließt offene Uaccess-Fenster, soweit sicher möglich,
3. speichert einen minimalen CPU-Kontext,
4. bestätigt ihren Panic-Zustand,
5. stoppt in einer architekturspezifischen Schleife,
6. wartet auf Reboot, Reset oder Halt.

Sie führt keine normale Dateisystem- oder Auditoperation aus.

---

## 15. Stoppen anderer CPUs

Der Panic Owner fordert alle anderen online geschalteten CPUs auf, in den Panic-Stop-Pfad zu wechseln.

Mögliche Mechanismen:

- Non-Maskable Interrupt,
- Inter-Processor Interrupt,
- architekturspezifischer Stop-Request,
- Hypervisor-Schnittstelle.

Der Panic Owner wartet nur eine begrenzte Zeit auf Bestätigungen.

Eine nicht reagierende CPU wird als:

```text
PANIC_CPU_UNRESPONSIVE
```

markiert.

---

## 16. Kein unbegrenztes Warten

Der Panic Owner darf nicht unbegrenzt auf andere CPUs warten.

Es gelten feste Zeitgrenzen für:

- CPU-Stopp,
- Crash-Dump-Erzeugung,
- Audit-Persistenz,
- Geräte-Quiesce,
- Neustart.

Nach Ablauf wird mit dem bestmöglichen verbleibenden Panic-Pfad fortgefahren.

---

## 17. Interruptzustand

Im Panic-Pfad werden normale maskierbare Interrupts grundsätzlich deaktiviert.

Ausnahmen können sein:

- Panic-Timer,
- Watchdog,
- minimaler serieller Diagnosekanal,
- dedizierter Crash-Dump-Controller,
- Notfall-Reset.

Der Panic-Pfad darf nicht von normalem Interruptbetrieb abhängig sein.

---

## 18. Panic-Stack

Jede CPU besitzt einen vorab reservierten Panic-Stack.

Der Panic-Stack:

- ist unabhängig vom normalen Thread-Kernelstack,
- besitzt Guard Pages, soweit möglich,
- ist NX,
- wird während der CPU-Initialisierung vorbereitet,
- führt keine normale Kernelarbeit aus,
- bleibt für Panic und bestimmte kritische Exceptions reserviert.

Der Panic Owner wechselt so früh wie möglich auf diesen Stack.

---

## 19. Stackkorruption

Wurde die Panic durch Stackkorruption ausgelöst, darf der Panic-Pfad den beschädigten Stack nicht weiter umfangreich verwenden.

Der Low-Level-Entry:

1. sichert minimale Register,
2. wechselt auf den Panic-Stack,
3. markiert den alten Stack als nicht vertrauenswürdig,
4. vermeidet komplexes Stack-Unwinding auf dem beschädigten Stack.

Ein Stacktrace kann als unvollständig oder nicht vertrauenswürdig markiert werden.

---

## 20. Panic-Kontext

Der primäre Panic-Kontext enthält mindestens:

```text
panic_id
boot_id
panic_reason
panic_code
cpu_id
process_id
thread_id
job_id
security_domain_id
kernel_build_id
system_generation
security_version
trust_state
monotonic_timestamp
instruction_pointer
stack_pointer
fault_address
architecture_state
```

Sensible Werte werden nur in geschützten Diagnoseartefakten gespeichert.

---

## 21. Panic-ID

Jeder Panic-Vorgang erhält eine eindeutige Panic-ID.

Die Panic-ID dient zur Verbindung von:

- Panic-Bildschirm,
- Audit Record,
- Crash Dump,
- Boot-Failure-Metadaten,
- Recovery-Bericht.

Die sichtbare Panic-ID enthält keine Kerneladresse oder geheimen Daten.

---

## 22. Panic-Codes

Panic-Codes sind stabile, maschinenlesbare Werte.

Beispiele:

```text
NOVA_PANIC_STACK_CORRUPTION
NOVA_PANIC_DOUBLE_FAULT
NOVA_PANIC_INVALID_CONTEXT_SWITCH
NOVA_PANIC_SCHEDULER_CORRUPTION
NOVA_PANIC_KERNEL_EXECUTE_FAULT
NOVA_PANIC_SMEP_VIOLATION
NOVA_PANIC_SMAP_VIOLATION
NOVA_PANIC_PAGE_TABLE_CORRUPTION
NOVA_PANIC_CAPABILITY_CORRUPTION
NOVA_PANIC_KERNEL_INTEGRITY_FAILURE
NOVA_PANIC_MACHINE_CHECK
NOVA_PANIC_HARD_LOCKUP
NOVA_PANIC_DEADLOCK
NOVA_PANIC_EXPLICIT
NOVA_PANIC_NESTED
```

Freie Textmeldungen ergänzen den Code, ersetzen ihn aber nicht.

---

## 23. Panic-API

Kernelkomponenten verwenden eine zentrale Panic-API.

```c
NOVA_NORETURN
NOVA_NO_STACK_PROTECTOR
void nova_panic(
    nova_panic_code_t code,
    const nova_panic_context_t* context
);
```

Zusätzliche Helfer können sein:

```c
#define NOVA_PANIC(code) \
    nova_panic((code), NOVA_PANIC_CONTEXT_CURRENT())

#define NOVA_BUG_ON(condition, code) \
    do \
    { \
        if (condition) \
        { \
            NOVA_PANIC(code); \
        } \
    } while (0)
```

Der Panic-Pfad kehrt niemals zurück.

---

## 24. Keine dynamische Speicherallokation

Der Panic-Pfad verwendet keine normale dynamische Speicherallokation.

Vorab reserviert werden:

- Panic-Stacks,
- Panic Records,
- CPU-Kontextpuffer,
- Crash-Dump-Metadaten,
- minimale Grafik- und Textpuffer,
- Audit-Notfallrecords,
- Geräteinformationen für den Dump-Pfad.

Fehlende normale Heapfunktion darf den Panic-Pfad nicht verhindern.

---

## 25. Keine normalen Sperren

Der Panic-Pfad wartet nicht auf gewöhnliche Mutexe, Semaphoren oder blockierende Kernelobjekte.

Solche Sperren könnten:

- vom fehlerhaften Thread gehalten werden,
- auf einer gestoppten CPU liegen,
- durch Speicherbeschädigung ungültig sein.

Erlaubt sind nur speziell entworfene:

- atomare Panic-Zustände,
- CPU-lokale Operationen,
- begrenzte Try-Locks,
- lockfreie Notfallpuffer.

---

## 26. Panic-Safe-Funktionen

Nur ausdrücklich als panic-safe gekennzeichnete Funktionen dürfen im Panic-Pfad verwendet werden.

Eine panic-safe Funktion muss:

- nicht blockieren,
- keinen normalen Heap verwenden,
- keine normale Schedulerfunktion benötigen,
- keine unbeschränkte Rekursion verwenden,
- mit deaktivierten Interrupts arbeiten können,
- feste Laufzeit- und Größengrenzen besitzen.

Mögliche Kennzeichnung:

```c
NOVA_PANIC_SAFE
```

---

## 27. Geräte-Quiesce

NovaOS versucht nicht, im Panic-Pfad sämtliche Geräte normal herunterzufahren.

Ein begrenztes Quiesce ist zulässig für:

- Stoppen gefährlicher Aktoren,
- Verhindern weiterer DMA-Schreibzugriffe,
- Maskieren nicht benötigter Interrupts,
- Einfrieren eines Crash-Dump-Ziels,
- Stoppen eines Grafik- oder Storage-Controllers.

Normale komplexe Treiber-Shutdown-Routinen werden nicht blind aufgerufen.

---

## 28. DMA und IOMMU

Bei einem Panic soll NovaOS nach Möglichkeit:

- neue DMA-Anforderungen stoppen,
- IOMMU-Domänen einfrieren,
- gefährliche Gerätezugriffe widerrufen,
- laufende DMA-Schreibvorgänge begrenzen.

Ein vollständiger Schutz ist nicht garantiert, wenn:

- keine IOMMU vorhanden ist,
- Hardware defekt ist,
- der IOMMU-Zustand selbst beschädigt wurde.

Der Zustand wird im Panic-Bericht dokumentiert.

---

## 29. Dateisystemzugriffe

Der Panic-Pfad verwendet keine normalen Dateisystemoperationen auf möglicherweise inkonsistenten Dateisystemen.

Nicht zulässig sind:

- normale Dateierstellung,
- Journaltransaktionen,
- Verzeichnisänderungen,
- beliebige Logrotation,
- Mount- oder Unmount-Vorgänge.

Crash-Dumps werden nur über einen speziell vorbereiteten, begrenzten Dump-Pfad geschrieben.

---

## 30. Crash-Dump-Ziele

Zulässige Crash-Dump-Ziele sind beispielsweise:

- vorab reservierte Dump-Partition,
- reserviertes Raw Block Device,
- vorab reservierter persistenter Speicher,
- geschützter NVRAM-Bereich,
- Hypervisor-Crash-Kanal,
- dedizierter Recovery-Speicher.

Ein normales Benutzerdateisystem ist kein bevorzugtes Panic-Dump-Ziel.

---

## 31. Crash-Dump-Stufen

NovaOS unterstützt mehrere Dump-Stufen.

| Stufe | Inhalt |
|---|---|
| `NONE` | kein Dump |
| `MINIMAL` | Panic-Kontext und CPU-Register |
| `KERNEL` | ausgewählte Kernelbereiche |
| `FULL_KERNEL` | vollständiger Kerneladressraum |
| `SYSTEM` | Kernel und ausgewählte Prozesszustände |
| `FULL_MEMORY` | gesamter physischer Speicher |

Der Produktionsstandard ist:

```text
MINIMAL oder KERNEL
```

Vollständige Speicherabbilder benötigen eine ausdrückliche Sicherheits- und Datenschutzpolicy.

---

## 32. Minimaler Crash Dump

Ein minimaler Dump enthält mindestens:

- Panic-ID,
- Panic-Code,
- Kernel-Build-ID,
- Systemgeneration,
- Trust State,
- primären CPU-Kontext,
- Zustände anderer erreichbarer CPUs,
- relevante Page-Fault-Informationen,
- letzte kritische Audit Records,
- Modul-Build-IDs,
- Prüfsumme.

Er enthält nicht automatisch sämtliche Prozess- oder Benutzerdaten.

---

## 33. Kernel-Dump

Ein Kernel-Dump kann zusätzlich enthalten:

- Kerneltext-Metadaten,
- Kerneldatenbereiche,
- Thread Control Blocks,
- Schedulerzustände,
- Seitentabellenmetadaten,
- Kernelstacks,
- Kernelobjektgraph,
- Modulliste.

Jeder Bereich wird nach Vertrauens- und Datenschutzstatus gekennzeichnet.

---

## 34. Vollständiger Speicherdump

Ein vollständiger physischer Speicherdump kann enthalten:

- Passwörter,
- Schlüssel,
- Dokumentinhalte,
- Kommunikationsdaten,
- Tokens,
- biometrische Daten,
- Benutzerspeicher.

Er ist deshalb standardmäßig deaktiviert.

Eine Aktivierung benötigt:

- besondere Capability,
- ausdrückliche lokale Policy,
- verschlüsseltes Dump-Ziel,
- Zugriffsschutz,
- definierte Aufbewahrung,
- Auditierung der Aktivierung und Auswertung.

---

## 35. Dump-Verschlüsselung

Crash-Dumps werden nach Möglichkeit bereits beim Schreiben verschlüsselt.

Der Dump-Schlüssel kann stammen aus:

- vorab erzeugtem flüchtigem Dump-Schlüssel,
- TPM- oder HSM-Backend,
- Recovery Public Key,
- lokalem Diagnose Public Key.

Der Panic-Pfad soll möglichst mit einem Public-Key- oder vorab vorbereiteten symmetrischen Verfahren arbeiten, ohne geheime langfristige Schlüssel ungeschützt zu laden.

---

## 36. Dump-Integrität

Jeder Dump enthält:

- Formatversion,
- Blockprüfsummen,
- Gesamt-Hash,
- Erzeugungsstatus,
- Abschlussmarker,
- optional Signatur oder MAC.

Ein unvollständiger Dump wird als unvollständig markiert.

Er darf nicht als vollständig vertrauenswürdiger Diagnosezustand dargestellt werden.

---

## 37. Redaction

Soweit im Panic-Pfad sicher möglich, werden besonders sensible Bereiche ausgeschlossen oder redigiert.

Beispiele:

- bekannte Schlüsselbereiche,
- Stack-Canary-Speicher,
- KASLR-Seeds,
- Authentisierungstoken,
- verschlüsselte Secret Stores.

Bei vollständiger Kernelkompromittierung kann eine perfekte Redaction nicht garantiert werden.

---

## 38. KASLR-Informationen

Crash-Dumps benötigen KASLR-Informationen zur Symbolauflösung.

Der konkrete Slide wird:

- im verschlüsselten Dump gespeichert oder
- durch eine geschützte Layoutreferenz ersetzt.

Der Panic-Bildschirm und allgemeine Audit Record zeigen keinen vollständigen Kernel-Slide.

---

## 39. Audit-Integration

Der Panic-Pfad erzeugt einen reservierten Audit-Notfallrecord.

Dieser enthält mindestens:

- Panic-ID,
- Panic-Code,
- Boot-ID,
- CPU-ID,
- Kernel-Build-ID,
- Systemgeneration,
- Sicherheitsmodus,
- Dump-Status,
- geplante Abschlussaktion.

Der Audit Record enthält keine geheimen Speicherinhalte oder vollständigen Kerneladressen.

---

## 40. Letzte Auditereignisse

Ein Crash Dump kann eine begrenzte Anzahl der letzten kritischen Audit Records enthalten.

Diese Records werden:

- aus reservierten CPU-lokalen Puffern kopiert,
- nicht über normale Auditdienst-Abfragen geladen,
- mit ihrem Integritätsstatus versehen,
- datenschutzkonform begrenzt.

Der Panic-Pfad darf keine vollständige Auditdatenbank durchsuchen.

---

## 41. Benutzeranzeige

NovaOS zeigt bei einem Kernel-Panic einen klaren, ruhigen Fehlerbildschirm an.

Der Bildschirm enthält:

- NovaOS-Panic-Symbol,
- verständliche Kurzmeldung,
- Panic-ID,
- stabilen Fehlercode,
- aktuellen Vorgang, beispielsweise Dump-Erzeugung,
- geplante Neustart- oder Recovery-Aktion,
- optionalen QR-Code oder Diagnosecode ohne Geheimnisse.

Die Anzeige soll nicht mit technischen Speicheradressen überladen werden.

---

## 42. Panic-Bildschirm ohne Grafiksystem

Der Panic-Pfad darf nicht vom normalen Window Manager, Compositor oder Grafikdienst abhängen.

Er verwendet:

- vorbereiteten Framebuffer,
- einfache panic-safe Text- und Zeichenroutinen,
- serielle Notfallausgabe,
- architekturspezifische Konsole.

Ist keine Grafik verfügbar, bleibt eine minimale Textausgabe möglich.

---

## 43. Schutz sichtbarer Informationen

Der sichtbare Panic-Bildschirm zeigt standardmäßig nicht:

- vollständige Kerneladressen,
- Stack-Canaries,
- KASLR-Slides,
- Schlüssel,
- Tokens,
- Dateiinhalte,
- vollständige Benutzernamen,
- sensible Prozessargumente.

Technische Details verbleiben im geschützten Crash Dump.

---

## 44. Headless-Systeme

Auf Servern, Robotern oder eingebetteten Geräten kann keine lokale Anzeige verfügbar sein.

Dann verwendet NovaOS abhängig von der Policy:

- serielle Notfallkonsole,
- lokale Status-LED,
- Hardwaredisplay,
- geschützten Managementkanal,
- Recovery-Metadaten.

Eine externe Panic-Meldung darf keine sensiblen Dumpdaten enthalten.

---

## 45. Safety-Systeme

Bei Robotik-, Maschinen- oder Steuerungsanwendungen muss vor Reboot oder Halt ein definierter Safe-State-Handler ausgeführt werden.

Beispiele:

- Motoren stoppen,
- Ausgänge stromlos setzen,
- Bremsen aktivieren,
- Aktoren in sichere Position bringen,
- Steuerung an einen Fallback-Controller übergeben.

Der Safe-State-Handler:

- ist plattformspezifisch,
- wird vorab registriert,
- ist panic-safe,
- besitzt eine feste Laufzeitgrenze,
- verwendet keine normalen komplexen Treiberpfade.

---

## 46. Keine allgemeine Safe-State-Garantie

Der Kernel kann einen Safe State nur anfordern.

Eine Garantie ist nur möglich, wenn:

- Hardware dies unterstützt,
- der notwendige Treiberpfad funktionsfähig ist,
- Aktoren kontrollierbar bleiben,
- die Panic-Ursache den Pfad nicht beschädigt hat,
- ein unabhängiger Safety-Controller vorhanden ist.

Für sicherheitskritische Systeme ist ein unabhängiger Hardware-Fallback vorzuziehen.

---

## 47. Panic-Abschlussaktionen

NovaOS unterstützt folgende Abschlussaktionen:

| Aktion | Verhalten |
|---|---|
| `HALT` | CPUs bleiben im gestoppten Zustand |
| `REBOOT` | kontrollierter Hardware-Neustart |
| `RECOVERY` | Neustart in verifiziertes Recovery-System |
| `FALLBACK_GENERATION` | Neustart in ältere sichere Systemgeneration |
| `POWER_OFF` | kontrolliertes Ausschalten, soweit panic-safe möglich |
| `WATCHDOG_RESET` | Reset über Hardware-Watchdog |
| `PLATFORM_SAFE_STATE` | Safe-State-Anforderung und anschließender Halt oder Reset |

---

## 48. Standardabschlussaktion

Die Standardpolicy hängt vom Systemprofil ab.

| Systemprofil | Standard |
|---|---|
| Desktop | Dump, kurze Anzeige, Reboot |
| Server | Dump, Reboot oder Halt nach Policy |
| Entwicklung | Dump und Halt |
| Kiosk | Dump und automatischer Recovery-Neustart |
| Robotik | Safe State, Dump, Recovery oder Halt |
| sicherheitskritisch | unabhängigen Safe State auslösen und kontrolliert stoppen |

Ein automatischer Reboot darf die Diagnose nicht vollständig verhindern.

---

## 49. Neustartverzögerung

Vor einem automatischen Neustart kann NovaOS eine kurze Verzögerung verwenden.

Die Verzögerung dient:

- sichtbarer Fehlerinformation,
- Abschluss des minimalen Dumps,
- Audit-Notfallrecord,
- Safe-State-Handler,
- Watchdogkoordination.

Die Verzögerung besitzt eine feste Obergrenze.

---

## 50. Benutzerkonfigurierbare Verzögerung

Die sichtbare Verzögerung kann innerhalb sicherer Grenzen konfiguriert werden.

Beispiele:

```text
panic.reboot_delay = 5s
panic.action = recovery
panic.dump_level = minimal
```

Sicherheitskritische Mindest- und Höchstgrenzen können nicht durch normale Benutzerkonfiguration überschrieben werden.

---

## 51. Hardware-Watchdog

Der Panic-Pfad koordiniert den Hardware-Watchdog.

Mögliche Strategien:

- Watchdog bewusst weiterlaufen lassen,
- Timeout für Dump verlängern,
- nach Dump sofort Reset auslösen,
- Watchdog als letzte Rückfallebene verwenden.

Der Panic-Pfad darf den Watchdog nicht unbegrenzt deaktivieren.

---

## 52. Neustartmechanismen

NovaOS versucht Neustartmechanismen in definierter Reihenfolge.

Beispiel:

```text
Plattform-Reset
    ↓ fehlgeschlagen
ACPI-Reset
    ↓ fehlgeschlagen
Firmware-Reset
    ↓ fehlgeschlagen
Watchdog-Reset
    ↓ fehlgeschlagen
CPU-Triple-Fault oder architekturspezifischer Reset
    ↓ fehlgeschlagen
Halt
```

Die konkrete Reihenfolge ist plattformabhängig.

---

## 53. Boot-Failure-Zähler

Ein Panic vor erfolgreicher Bootbestätigung erhöht den Boot-Failure-Zähler der aktiven Systemgeneration.

Ein Panic nach erfolgreicher Bestätigung kann ebenfalls einen Laufzeitfehlerzähler erhöhen.

Die Metadaten enthalten:

```text
system_generation
panic_id
boot_confirmed
failure_count
last_panic_code
```

Diese Daten werden atomar und integritätsgeschützt gespeichert, soweit der Panic-Pfad dies zulässt.

---

## 54. Recovery-Auswahl

NovaOS kann Recovery automatisch wählen, wenn:

- mehrere Panic-Vorgänge während des Starts auftreten,
- dieselbe Systemgeneration wiederholt früh abstürzt,
- eine Integritätsverletzung erkannt wurde,
- der normale Dump- oder Neustartpfad fehlschlägt,
- die Panic Policy Recovery erzwingt.

Nur ein gemäß ADR-SEC-0001 verifiziertes Recovery-System darf gestartet werden.

---

## 55. Fallback-Systemgeneration

NovaOS kann auf eine ältere Systemgeneration zurückfallen, wenn:

- sie weiterhin signiert und gültig ist,
- ihre Sicherheitsversion erlaubt ist,
- sie als zuvor funktionsfähig bestätigt wurde,
- keine Policy den Rollback verbietet.

Ein Panic darf keinen Rollback auf eine sicherheitskritisch gesperrte Generation erzwingen.

---

## 56. Neustartschleifen

NovaOS verhindert endlose Panic-Neustartschleifen.

Nach einer konfigurierten Anzahl fehlgeschlagener Starts kann das System:

- Recovery starten,
- auf eine sichere Generation wechseln,
- im Bootmanager auf Benutzereingabe warten,
- automatische Neustarts vorübergehend stoppen.

Der Status wird klar angezeigt.

---

## 57. Nested Panic

Tritt im Panic-Pfad ein weiterer kritischer Fehler auf, wechselt NovaOS in `PANIC_MINIMAL`.

Der Minimal-Panic-Pfad:

- verwendet nur CPU-lokale vorab reservierte Daten,
- vermeidet Grafik, Dateisystem und komplexe Dumpfunktionen,
- schreibt höchstens einen minimalen Notfallrecord,
- führt schnellstmöglich Halt oder Reset aus.

Ein Nested Panic darf keine unendliche Rekursion erzeugen.

---

## 58. Panic-Rekursionserkennung

Jede CPU besitzt einen Panic-Depth-Zähler.

```text
panic_depth == 0:
    normaler Panic-Eintritt

panic_depth == 1:
    bereits im Panic-Pfad

panic_depth > 1:
    sofortiger Minimal-Panic
```

Ein globaler Zustand ergänzt die CPU-lokale Rekursionserkennung.

---

## 59. Fehler im Crash-Dump-Pfad

Schlägt die Dump-Erzeugung fehl:

1. Fehlerstatus im Panic Record setzen,
2. keine normale Fehlerbehandlung starten,
3. verbleibende Diagnose abschließen,
4. Abschlussaktion fortsetzen.

Ein fehlgeschlagener Dump darf den Panic-Pfad nicht unbegrenzt blockieren.

---

## 60. Fehler im Panic-UI-Pfad

Schlägt die grafische Panic-Anzeige fehl, verwendet NovaOS:

- serielle Ausgabe,
- Textmodus,
- Firmwarekonsole,
- LED- oder Statuscode,
- keine Anzeige.

Die Abschlussaktion wird unabhängig von der Anzeige fortgesetzt.

---

## 61. Fehler im Audit-Pfad

Kann der Audit-Notfallrecord nicht gespeichert werden, wird dies im Crash-Dump-Header oder in den Recovery-Metadaten markiert.

Der Panic-Pfad versucht nicht, den normalen Auditdienst neu zu starten.

---

## 62. Panic und Verschlüsselung

Ein Panic kann auftreten, während verschlüsselte Datenträger geöffnet sind.

NovaOS darf Schlüssel nicht auf dem Bildschirm oder in ungeschützten Dumps ausgeben.

Vor einem Reset werden Schlüsselbereiche nach Möglichkeit:

- nicht in den Dump aufgenommen,
- redigiert,
- auf der CPU überschrieben,
- durch Hardwareisolierung geschützt.

Bei schwerer Speicherkorruption kann vollständige Schlüssellöschung nicht garantiert werden.

---

## 63. Registerbereinigung

Vor Halt, Reboot oder Übergabe an Recovery versucht NovaOS, sensitive Register zu bereinigen.

Dazu können gehören:

- Vektorregister,
- FPU-Zustände,
- Debugregister,
- Pointer-Authentifizierungszustände,
- temporäre Schlüsselregister,
- Uaccess-Overrides.

Diese Bereinigung ist Best Effort und darf den Minimal-Panic-Pfad nicht gefährden.

---

## 64. Panic und Dateisystemintegrität

NovaOS führt im Panic-Pfad keinen normalen sauberen Unmount durch.

NovaFS und andere Dateisysteme müssen durch:

- transaktionale Metadaten,
- Journaling,
- Copy-on-Write,
- Prüfsummen,
- Recovery beim nächsten Start

mit einem abrupten Stopp umgehen können.

Der nächste Start führt bei Bedarf Integritätsprüfungen und Recovery aus.

---

## 65. Panic und laufende Updates

Tritt ein Panic während eines Systemupdates auf, darf keine teilweise installierte Generation automatisch bestätigt werden.

Beim nächsten Start:

- wird die aktive Generation validiert,
- bleibt eine unbestätigte Generation `PENDING` oder wird `FAILED`,
- wird der atomare Updatezustand geprüft,
- kann Recovery oder Fallback erfolgen.

---

## 66. Panic und Live-Update

Ein Panic während eines Live-Kernelupdates wird mit folgenden Informationen erfasst:

- alte Kernelgeneration,
- neue Kernelgeneration,
- Updatephase,
- aktiver Namespace oder Service-Switch,
- betroffene Module.

NovaOS darf nach einem solchen Panic nicht unkontrolliert einen teilweise umgeschalteten Zustand wiederaufnehmen.

---

## 67. Panic und virtuelle Maschinen

Unter einem Hypervisor kann NovaOS:

- einen Hypervisor-Crash-Notify senden,
- einen paravirtualisierten Dump-Kanal verwenden,
- die virtuelle CPU stoppen,
- einen virtuellen Reset auslösen.

Der Gast darf nicht voraussetzen, dass der Hypervisor vertraulich mit vollständigen Speicherabbildern umgeht.

Dump- und Datenschutzpolicy bleiben aktiv.

---

## 68. Panic und Debugger

Im Development Mode kann ein Kerneldebugger vor dem Reboot aktiviert werden.

Dies benötigt:

- aktive Development-Policy,
- spezielle Debug-Capability,
- lokale oder gesicherte Verbindung,
- Timeout,
- sichtbare Kennzeichnung.

Im Produktionsmodus wird nicht automatisch auf einen externen Debugger gewartet.

---

## 69. Remote-Diagnose

NovaOS überträgt bei einem Panic standardmäßig keine Crash-Dumps automatisch an externe Dienste.

Ein Remote-Diagnoseexport benötigt:

- vorher konfigurierte Policy,
- definierten Empfänger,
- Verschlüsselung,
- Authentisierung,
- Datenminimierung,
- Auditierung beim nächsten Start.

Ein Panic allein gilt nicht als Einwilligung zur Datenübertragung.

---

## 70. Datenschutz

Crash-Dumps und Panic-Auditdaten können personenbezogene oder vertrauliche Daten enthalten.

Daher gelten:

- minimale Standard-Dumpstufe,
- Verschlüsselung,
- Zugriffsschutz,
- Aufbewahrungsfristen,
- kontrollierte Löschung,
- kontrollierter Export,
- Redaction.

Vollständige Speicherdumps sind nur für besonders autorisierte Diagnosefälle vorgesehen.

---

## 71. Panic Policy

Die konfigurierbare Panic Policy enthält mindestens:

```text
panic_action
dump_level
reboot_delay
cpu_stop_timeout
dump_timeout
safe_state_timeout
allow_remote_debug
allow_remote_export
maximum_boot_failures
recovery_threshold
```

Die Policy ist:

- signiert oder integritätsgeschützt,
- versioniert,
- capabilitygeschützt,
- lokal diagnostizierbar.

---

## 72. Sicherheitskritische Policyfelder

Folgende Policyfelder können nicht durch normale Anwendungen verändert werden:

- Zulässigkeit vollständiger Speicherdumps,
- Remote-Dump-Export,
- Recovery-Vertrauensanforderungen,
- Safe-State-Handler,
- Kerneldebugger-Aktivierung,
- maximale Panic-Wartezeit,
- Boot-Failure-Schwellen,
- Schlüssel für Dump-Verschlüsselung.

Änderungen werden auditiert.

---

## 73. API-Grundmodell

Eine mögliche Panic-Policy-Struktur lautet:

```c
typedef enum
{
    NOVA_PANIC_ACTION_HALT = 0,
    NOVA_PANIC_ACTION_REBOOT,
    NOVA_PANIC_ACTION_RECOVERY,
    NOVA_PANIC_ACTION_FALLBACK_GENERATION,
    NOVA_PANIC_ACTION_POWER_OFF,
    NOVA_PANIC_ACTION_WATCHDOG_RESET,
    NOVA_PANIC_ACTION_PLATFORM_SAFE_STATE
} nova_panic_action_t;

typedef enum
{
    NOVA_DUMP_NONE = 0,
    NOVA_DUMP_MINIMAL,
    NOVA_DUMP_KERNEL,
    NOVA_DUMP_FULL_KERNEL,
    NOVA_DUMP_SYSTEM,
    NOVA_DUMP_FULL_MEMORY
} nova_dump_level_t;

typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    nova_panic_action_t action;
    nova_dump_level_t dump_level;

    uint64_t reboot_delay_ns;
    uint64_t cpu_stop_timeout_ns;
    uint64_t dump_timeout_ns;
    uint64_t safe_state_timeout_ns;

    uint32_t maximum_boot_failures;
    uint32_t recovery_threshold;

    uint32_t flags;
    uint32_t reserved;
} nova_panic_policy_t;
```

---

## 74. Statusabfrage

Autorisierte Dienste können den Panic- und Dumpstatus abfragen.

```c
nova_status_t nova_security_get_panic_policy(
    nova_panic_policy_t* policy
);

nova_status_t nova_security_get_last_panic_info(
    nova_last_panic_info_t* info
);

nova_status_t nova_crash_dump_list(
    nova_crash_dump_list_t* dumps
);
```

Normale Anwendungen erhalten keine vollständigen Kerneladressen oder Dumpinhalte.

---

## 75. Capability-Modell

Mögliche Capabilities sind:

```text
CAP_PANIC_QUERY_POLICY
CAP_PANIC_SET_POLICY
CAP_PANIC_TRIGGER_TEST
CAP_CRASH_DUMP_LIST
CAP_CRASH_DUMP_READ
CAP_CRASH_DUMP_DECRYPT
CAP_CRASH_DUMP_EXPORT
CAP_CRASH_DUMP_DELETE
CAP_RECOVERY_SELECT
CAP_KERNEL_DEBUG_AFTER_PANIC
CAP_PANIC_ADMIN
```

Das absichtliche Auslösen eines Test-Panics ist ausschließlich im autorisierten Development Mode zulässig.

---

## 76. Panic-Selbsttest

NovaOS kann einen kontrollierten Panic-Selbsttest bereitstellen.

Dieser prüft:

- Panic-Owner-Wahl,
- Stoppen zusätzlicher CPUs,
- Panic-Stack,
- minimale Anzeige,
- Audit-Notfallrecord,
- Dump-Ziel,
- Neustart- oder Recoveryaktion.

Der Test benötigt ausdrückliche Bestätigung, da er das laufende System absichtlich beendet.

---

## 77. Telemetrie und Diagnose

Der Panic-Pfad erfasst mindestens:

- Panic-Code,
- Panic-ID,
- Panic Owner,
- reagierende und nicht reagierende CPUs,
- Dumpstufe,
- Dumpstatus,
- Dumpdauer,
- Safe-State-Status,
- UI-Status,
- Auditstatus,
- Abschlussaktion,
- Boot-Failure-Zähler.

Ausführliche Telemetrie wird erst beim nächsten Start aus dem geschützten Panic Record übernommen.

---

## 78. Verhalten nach dem Neustart

Beim nächsten Start prüft NovaOS:

1. ob ein Panic Record vorliegt,
2. ob der Dump vollständig und integritätsgültig ist,
3. welche Systemgeneration betroffen war,
4. ob der Start zuvor bestätigt war,
5. ob Recovery oder Fallback erforderlich ist,
6. ob der Benutzer informiert werden soll.

Der Record wird anschließend in den persistenten Audit Trail übernommen.

---

## 79. Benutzerinformation nach Neustart

Nach erfolgreichem Neustart kann NovaOS anzeigen:

```text
NovaOS wurde nach einem schweren Systemfehler neu gestartet.

Fehlercode: NOVA_PANIC_...
Panic-ID: ...
Diagnosedaten: lokal gespeichert
Recovery-Status: ...
```

Ein Upload wird nicht automatisch gestartet.

Der Benutzer beziehungsweise Administrator kann einen kontrollierten Diagnoseexport anstoßen.

---

## 80. Aufbewahrung von Crash-Dumps

Crash-Dumps besitzen:

- Speicherquote,
- maximale Anzahl,
- maximale Aufbewahrungsdauer,
- Sicherheitsklassifikation,
- Legal- oder Security-Hold,
- Löschpolicy.

Ältere Dumps können nach erfolgreicher Rotation kontrolliert gelöscht werden.

Die Löschung wird auditiert.

---

## 81. Kein automatisches Löschen kritischer Beweise

Dumps mit:

- Integritätsverletzung,
- möglicher Kernelkompromittierung,
- wiederholtem identischem Panic,
- Safety-Ereignis,
- aktivem Security Hold

werden nicht durch normale Speicherdruckrotation entfernt.

Eine autorisierte Löschentscheidung bleibt erforderlich.

---

## 82. Performance

Der normale Kernelbetrieb trägt nur geringe Kosten für die Panic-Infrastruktur.

Vorab reserviert werden:

- kleine Panic-Stacks,
- CPU-Kontextpuffer,
- minimale Dumpmetadaten,
- Audit-Notfallrecords.

Der vollständige Panic-Pfad ist kein normaler Performancepfad.

Seine Prioritäten sind:

1. Sicherheit,
2. Safe State,
3. minimale Diagnose,
4. kontrollierter Abschluss,
5. Geschwindigkeit.

---

## 83. Konsequenzen

### 83.1 Positive Konsequenzen

- kein unsicherer Weiterbetrieb nach zentraler Kernelkorruption,
- deterministische Mehrkern-Panic-Koordination,
- geschützte und begrenzte Crash-Dumps,
- klare Recovery- und Neustartregeln,
- Panic-Bildschirm ohne Offenlegung von Geheimnissen,
- Unterstützung für Robotik- und Safety-Safe-States,
- Integration mit Audit und Secure Startup,
- Schutz vor Neustartschleifen,
- kontrollierter Minimal-Panic bei Rekursion.

### 83.2 Negative Konsequenzen

- zusätzlicher reservierter Speicher pro CPU,
- hohe Implementierungskomplexität,
- Panic-safe Treiber- und Plattformpfade erforderlich,
- Crash-Dump-Verschlüsselung und Schlüsselverwaltung,
- vollständige Diagnose ist bei schwerer Korruption nicht immer möglich,
- automatische Reboots können Fehleranalyse erschweren.

### 83.3 Risiken

- Deadlock im Panic-Pfad,
- Nested Panic,
- nicht reagierende CPUs,
- beschädigter Crash Dump,
- Offenlegung sensibler Daten,
- fehlgeschlagener Safe State,
- Neustartschleife,
- unzuverlässiger Storage- oder Grafiktreiber,
- unvollständige Audit-Persistenz.

Diese Risiken werden begrenzt durch:

- Single-Panic-Owner,
- CPU-lokale Panic-Stacks,
- keine normalen Sperren,
- feste Zeitgrenzen,
- reservierte Puffer,
- Minimal-Panic-Pfad,
- verifiziertes Recovery,
- verschlüsselte Dumps,
- Boot-Failure-Zähler.

---

## 84. Auswirkungen auf andere Subsysteme

### 84.1 Scheduler und SMP

Andere CPUs müssen über einen nicht blockierenden Panic-Stop-Pfad angehalten werden können.

### 84.2 Threadverwaltung

Jede CPU benötigt einen Panic-Stack und minimale Kontextpuffer.

### 84.3 Speicherverwaltung

Crash-Dump-Bereiche und Panic-Stacks werden vorab reserviert und geschützt.

### 84.4 Stack Protector

Eine Stack-Protector-Verletzung führt unmittelbar in den Panic-Pfad.

### 84.5 NX, SMEP und SMAP

Unerwartete Schutzverletzungen im Kernel sind verbindliche Panic-Auslöser.

### 84.6 Kernel Isolation

Der Panic-Pfad benötigt eine bekannte sichere Kernel-Seitentabellensicht.

### 84.7 Audit

Panic-Ursache und Abschlussstatus werden über reservierte Notfallrecords erfasst.

### 84.8 Dateisystem

Dateisysteme müssen abrupten Systemstopp über Transaktionen, Journaling oder Copy-on-Write verkraften.

### 84.9 Recovery

Wiederholte Panic-Vorgänge können ein verifiziertes Recovery-System oder eine sichere Fallback-Generation aktivieren.

### 84.10 Treibermodell

Nur ausdrücklich panic-safe Funktionen dürfen Geräte im Panic-Pfad ansprechen.

### 84.11 Energie- und Plattformverwaltung

Reboot, Power-Off, Halt und Watchdog-Reset benötigen architekturspezifische Notfallpfade.

### 84.12 Robotik und Safety

Plattformen können einen vorab registrierten Safe-State-Handler bereitstellen.

---

## 85. Implementierungsrichtlinien

1. Ein Kernel-Panic ist ein nicht rückkehrender Zustand.
2. Genau eine CPU wird Panic Owner.
3. Andere CPUs wechseln in einen minimalen Secondary-Panic-Pfad.
4. Der Panic Owner wartet nur begrenzt auf andere CPUs.
5. Jede CPU besitzt einen vorab reservierten Panic-Stack.
6. Der Panic-Pfad verwendet keine normale Heapallokation.
7. Der Panic-Pfad wartet nicht auf gewöhnliche Kernelmutexe.
8. Nur panic-safe Funktionen dürfen verwendet werden.
9. Normale Dateisystemoperationen sind im Panic-Pfad verboten.
10. Crash-Dumps verwenden ein vorbereitetes Raw- oder Notfallziel.
11. Die Standard-Dumpstufe minimiert Benutzer- und Geheimnisdaten.
12. Vollständige Speicherdumps benötigen eine besondere Policy.
13. Crash-Dumps werden nach Möglichkeit verschlüsselt und integritätsgeschützt.
14. Panic-Bildschirme zeigen keine vollständigen Kerneladressen oder Geheimnisse.
15. Ein Nested Panic wechselt sofort in den Minimal-Panic-Pfad.
16. Jede Panic-Phase besitzt feste Timeouts.
17. Ein fehlgeschlagener Dump verhindert nicht die Abschlussaktion.
18. Boot-Failure-Zähler und Systemgeneration werden berücksichtigt.
19. Recovery und Fallback müssen weiterhin Secure Startup erfüllen.
20. Safety-Handler sind vorab registriert, klein und panic-safe.
21. Remote-Diagnose ist standardmäßig deaktiviert.
22. Nach dem Neustart wird der Panic Record in den lokalen Audit Trail übernommen.

---

## 86. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- expliziter Test-Panic,
- Panic auf der Boot-CPU,
- Panic auf einer zusätzlichen CPU,
- gleichzeitige Panic-Ereignisse auf mehreren CPUs,
- korrekte Panic-Owner-Wahl,
- Secondary-Panic-Pfad,
- nicht reagierende CPU,
- CPU-Stopp-Timeout,
- Wechsel auf Panic-Stack,
- beschädigter normaler Kernelstack,
- Stack-Protector-Panic,
- Double Fault,
- Kernel-NX-Verstoß,
- SMEP-Verstoß,
- SMAP-Verstoß,
- Seitentabellenkorruption,
- Schedulerkorruption,
- Machine Check,
- Watchdog-Hard-Lockup,
- Nested Panic,
- dreifache Panic-Rekursion,
- minimaler Panic-Pfad,
- grafischer Panic-Bildschirm,
- Text- und serielle Fallbackausgabe,
- Headless-System,
- minimaler Crash Dump,
- Kernel-Crash-Dump,
- vollständiger Speicherdump im Development Mode,
- Dump-Verschlüsselung,
- Dump-Integritätsfehler,
- volles Dump-Ziel,
- Dump-Timeout,
- Fehler im Dump-Treiber,
- Audit-Notfallrecord,
- ausgefallener Auditpfad,
- Safe-State-Erfolg,
- Safe-State-Timeout,
- DMA-Quiesce,
- IOMMU-Freeze,
- Hardware-Watchdog,
- Plattform-Reboot,
- fehlgeschlagener Reboot,
- Fallback auf Halt,
- Boot-Failure-Zähler,
- automatische Recovery,
- sichere Fallback-Generation,
- Rollback-Schutz,
- Neustartschleifenbegrenzung,
- Panic während eines Updates,
- Panic während eines Live-Updates,
- Panic unter Virtualisierung,
- Kerneldebugger im Development Mode,
- Datenschutz und Redaction,
- Übernahme des Panic Records nach Neustart,
- kontrollierte Crash-Dump-Löschung.

---

## 87. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- das vollständige Crash-Dump-Binärformat,
- den konkreten Verschlüsselungsalgorithmus,
- plattformspezifische Safe-State-Implementierungen,
- vollständige funktionale Sicherheitszertifizierung,
- konkrete Grafikgestaltung des Panic-Bildschirms,
- genaue Watchdog-Hardwaretreiber,
- externe Support- oder Uploaddienste,
- automatische KI-Analyse von Crash-Dumps,
- die endgültige öffentliche Diagnose-ABI,
- sämtliche architekturspezifischen Resetmechanismen.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 88. Verbindliche Invarianten

1. Ein Kernel-Panic kehrt niemals zur normalen Ausführung zurück.
2. NovaOS führt nach einer nicht isolierbaren Kernelkorruption nicht normal weiter.
3. Genau eine CPU wird Panic Owner.
4. Secondary Panic CPUs schreiben nicht gleichzeitig denselben Crash Dump.
5. Der Panic Owner wartet nicht unbegrenzt auf andere CPUs oder Geräte.
6. Jede CPU besitzt einen vorab reservierten Panic-Stack.
7. Der Panic-Pfad verwendet keine normale Heapallokation.
8. Der Panic-Pfad wartet nicht auf gewöhnliche blockierende Kernelobjekte.
9. Nur ausdrücklich panic-safe Funktionen dürfen im Panic-Pfad verwendet werden.
10. Normale Dateisystemoperationen sind im Panic-Pfad verboten.
11. Ein Stack-Protector-Verstoß im Kernel führt zur Panic.
12. Kernel-NX-, SMEP- und unerwartete SMAP-Verstöße führen zur Panic.
13. Ein beschädigter Context-Switch- oder Schedulerzustand führt zur Panic.
14. Ein Nested Panic führt in einen begrenzten Minimal-Panic-Pfad.
15. Crash-Dump-Fehler dürfen die Abschlussaktion nicht unbegrenzt verhindern.
16. Vollständige Speicherdumps sind standardmäßig deaktiviert.
17. Crash-Dumps werden nach Möglichkeit verschlüsselt und integritätsgeschützt.
18. Panic-Anzeige und normales Audit enthalten keine vollständigen Kerneladressen oder Geheimnisse.
19. Remote-Übertragung erfolgt nicht automatisch.
20. Recovery und Fallback-Generationen müssen vollständig verifiziert sein.
21. Ein Panic darf Rollback Protection nicht umgehen.
22. Boot-Failure-Zähler müssen Neustartschleifen begrenzen.
23. Ein Kernel-Panic ist nicht automatisch ein funktionaler Safe State.
24. Safety-Systeme benötigen einen unabhängigen oder vorab geprüften Safe-State-Pfad.
25. Panic Policy und Dumpzugriffe sind capabilitygeschützt.
26. Änderungen an sicherheitskritischen Panic-Policy-Feldern werden auditiert.
27. Eine Capability allein darf keine unverschlüsselte Offenlegung vollständiger Speicherdumps erzwingen.
28. KI- oder lernbasierte Komponenten dürfen Panic-, Dump-, Recovery-, Datenschutz-, Capability- oder Safe-State-Regeln nicht umgehen.

---

## 89. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-SEC-0004 – NX Memory
- ADR-SEC-0005 – SMEP/SMAP
- ADR-SEC-0006 – Kernel Isolation
- ADR-SEC-0008 – Audit
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-MM-0013 – Guard Pages
- ADR-MM-0014 – DMA Memory
- ADR-MM-0015 – Kernel Stack Layout
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0006 – Idle Thread
- ADR-PROC-0008 – Context Switching
- NPSPEC-KERNELPANIC-0001 – Kernel Panic Specification
- NPSPEC-CRASHDUMP-0001 – Kernel Crash Dump Specification
- NPSPEC-BOOTERROR-0001 – Boot Error Specification
- NPSPEC-RECOVERY-0001 – Recovery Specification
- NPSPEC-AUDIT-0001 – Audit Specification
- NPSPEC-SECURESTARTUP-0001 – Secure Kernel Startup Specification

---

## 90. Zusammenfassung der Entscheidung

NovaOS verwendet eine zentrale, nicht rückkehrende Kernel Panic Policy.

Wenn die Integrität zentraler Kernel- oder Sicherheitsinvarianten nicht mehr gewährleistet werden kann, wird der normale Betrieb beendet. NovaOS verwendet keinen allgemeinen „Kernel-Oops und weiter“-Modus für unbekannte oder potenziell korrumpierende Kernelverletzungen.

Bei einer Panic wird genau eine CPU zum Panic Owner. Alle anderen CPUs wechseln in einen begrenzten Secondary-Panic-Pfad und werden kontrolliert gestoppt. Jede CPU besitzt dafür einen vorab reservierten Panic-Stack.

Der Panic-Pfad:

- verwendet keine normale Heapallokation,
- wartet nicht auf gewöhnliche Kernelmutexe,
- führt keine normalen Dateisystemoperationen aus,
- besitzt feste Timeouts,
- verwendet ausschließlich panic-safe Funktionen.

Crash-Dumps werden über vorbereitete Notfallziele gespeichert. Der Standarddump enthält nur die für die Kernelanalyse erforderlichen Informationen. Vollständige Speicherdumps sind wegen möglicher Benutzer-, Schlüssel- und Inhaltsdaten standardmäßig deaktiviert.

Panic-Anzeige und Audit Record zeigen keine vollständigen Kerneladressen, KASLR-Slides, Stack-Canaries oder Schlüssel. Geschützte Dumps können die für eine Offlineanalyse erforderlichen Informationen verschlüsselt enthalten.

Wiederholte Panic-Vorgänge erhöhen den Boot-Failure-Zähler und können ein verifiziertes Recovery-System oder eine weiterhin sichere Fallback-Systemgeneration aktivieren. Rollback Protection bleibt dabei verbindlich.

Bei Robotik-, Maschinen- und Safety-Systemen wird vor Reboot oder Halt ein vorab registrierter panic-safe Safe-State-Handler aufgerufen. Ein Kernel-Panic allein gilt jedoch nicht automatisch als garantierter funktionaler Safe State.