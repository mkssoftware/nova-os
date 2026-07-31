# ADR-SEC-0005 – SMEP/SMAP

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0005 |
| Titel | Supervisor Mode Execution Prevention und Supervisor Mode Access Prevention |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Speicherzugriff / CPU-Schutzfunktionen |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0003, ADR-SEC-0004, ADR-MM-0002, ADR-PROC-0008 |
| Nachfolgende Spezifikation | NPSPEC-SMEP-SMAP-0001 |

---

## 1. Kontext

NovaOS trennt den privilegierten Kerneladressraum vom nicht privilegierten Userspace.

Trotz dieser Trennung muss der Kernel regelmäßig Daten mit Userspace-Prozessen austauschen.

Beispiele:

- Systemaufrufparameter lesen,
- Ergebnisse in Userspace-Puffer schreiben,
- Zeichenketten aus Userspace übernehmen,
- IPC-Daten kopieren,
- Dateisystemdaten übertragen,
- Netzwerkpakete bereitstellen,
- Prozesskontexte und Debugdaten verarbeiten.

Ohne zusätzliche Hardwaremechanismen kann Kernelcode versehentlich oder durch einen Angriff:

- Code aus einer Userspace-Seite im Kernelmodus ausführen,
- Userspace-Daten direkt dereferenzieren,
- manipulierte Userspace-Zeiger verwenden,
- während eines privilegierten Vorgangs auf ausgetauschte Daten zugreifen,
- eine kompromittierte Funktionsadresse in den Userspace umleiten.

x86-Prozessoren stellen dafür zwei Schutzfunktionen bereit:

- SMEP – Supervisor Mode Execution Prevention,
- SMAP – Supervisor Mode Access Prevention.

SMEP verhindert im Supervisor Mode die Ausführung von Code aus Userspace-Seiten.

SMAP verhindert im Supervisor Mode standardmäßig das Lesen und Schreiben von Userspace-Seiten.

Andere CPU-Architekturen besitzen vergleichbare Mechanismen, beispielsweise:

- PXN – Privileged Execute Never,
- PAN – Privileged Access Never,
- UAO – User Access Override,
- entsprechende privilegierte Zugriffskontrollen.

NovaOS benötigt ein einheitliches Architekturmodell, das diese Funktionen nutzt und alle Kernelzugriffe auf Userspace-Speicher über kontrollierte Schnittstellen führt.

---

## 2. Problemstellung

Der Kernel darf Userspace-Speicher nicht wie normalen Kernel-Speicher behandeln.

Ein Userspace-Zeiger kann:

- auf eine nicht gemappte Seite zeigen,
- außerhalb des erlaubten Adressbereichs liegen,
- während der Operation geändert werden,
- auf einen schreibgeschützten Bereich zeigen,
- mit einem anderen Mapping überlappen,
- einen Seitenfehler auslösen,
- kurz vor dem Zugriff ungültig werden,
- absichtlich auf Kerneladressen gesetzt werden,
- eine Integerüberlaufprüfung umgehen.

Eine einmalige Zeigerprüfung reicht nicht aus.

Zwischen Prüfung und Nutzung kann sich der Userspace-Adressraum ändern. Außerdem muss der Kernel manche Seitenfehler beim Kopieren kontrolliert behandeln können.

SMEP und SMAP müssen daher mit:

- der virtuellen Speicherverwaltung,
- Systemaufrufen,
- Exception Handling,
- Context Switching,
- Interrupts,
- CPU-Hotplug,
- Debugging,
- Echtzeitpfaden

zusammenarbeiten.

---

## 3. Entscheidungsziele

Die SMEP/SMAP-Architektur soll:

1. Kernelcodeausführung aus Userspace-Speicher verhindern,
2. direkte Kernelzugriffe auf Userspace-Daten standardmäßig verhindern,
3. kontrollierte Copy-in- und Copy-out-Schnittstellen bereitstellen,
4. ungültige Userspace-Zeiger sicher behandeln,
5. Zugriffsfaults ohne Kernelkompromittierung abfangen,
6. Mehrkern- und Context-Switch-Semantik unterstützen,
7. Interrupt- und Exceptionpfade absichern,
8. TOCTOU-Risiken begrenzen,
9. architekturübergreifende Schutzmechanismen abstrahieren,
10. fehlende Hardwareunterstützung klar kennzeichnen,
11. Diagnose ermöglichen, ohne Kerneladressen offenzulegen,
12. mit NX, KASLR und Capability-Isolation zusammenarbeiten.

---

## 4. Begriffsdefinitionen

### 4.1 Supervisor Mode

Supervisor Mode ist der privilegierte CPU-Ausführungsmodus des Kernels.

---

### 4.2 Userspace-Seite

Eine Userspace-Seite ist eine virtuelle Speicherseite, die durch die Seitentabellen als für nicht privilegierte Ausführung zugänglich markiert ist.

---

### 4.3 SMEP

SMEP verhindert die Ausführung von Instruktionen aus Userspace-Seiten, während die CPU im Supervisor Mode läuft.

---

### 4.4 SMAP

SMAP verhindert standardmäßig Datenzugriffe des Kernels auf Userspace-Seiten.

Kontrollierte Zugriffsfenster müssen ausdrücklich geöffnet und anschließend wieder geschlossen werden.

---

### 4.5 User Access Window

Ein User Access Window ist ein kurzer, kontrollierter Kernelabschnitt, in dem ein definierter Zugriff auf Userspace-Speicher erlaubt wird.

---

### 4.6 Copy-in

Copy-in überträgt Daten aus Userspace-Speicher in einen Kernelpuffer.

---

### 4.7 Copy-out

Copy-out überträgt Daten aus einem Kernelpuffer in Userspace-Speicher.

---

### 4.8 Uaccess

Uaccess bezeichnet die Gesamtheit der kontrollierten Kernelmechanismen zum Zugriff auf Userspace-Speicher.

---

### 4.9 Fault Fixup

Ein Fault Fixup ist ein kontrollierter Wiederherstellungspfad für einen erwartbaren Speicherfehler während eines Uaccess-Vorgangs.

---

## 5. Bedrohungsmodell

SMEP erschwert insbesondere:

- Kernelkontrollfluss in eine Userspace-Seite,
- Ausführung von durch den Angreifer bereitgestelltem Userspace-Code im Kernelmodus,
- bestimmte Return-to-User- und Code-Injection-Angriffe.

SMAP erschwert insbesondere:

- unbeabsichtigte Dereferenzierung von Userspace-Zeigern,
- Confused-Deputy-Angriffe über manipulierte Userspace-Puffer,
- Ausnutzung fehlerhafter Kernelpointerprüfungen,
- direkte Nutzung von Userspace-Daten als vertrauenswürdige Kernelstrukturen.

SMEP und SMAP schützen nicht vollständig gegen:

- Code-Reuse innerhalb des Kernels,
- gültige Kernelpointer auf manipulierte Kerneldaten,
- Fehler innerhalb bewusst geöffneter Uaccess-Fenster,
- TOCTOU-Probleme bei mehrfacher Datennutzung,
- Hardware ohne entsprechende Schutzfunktionen,
- kompromittierte Seitentabellen,
- physische Angriffe,
- logische Berechtigungsfehler.

---

## 6. Betrachtete Alternativen

### 6.1 Keine Hardwaredurchsetzung

Der Kernel prüft Userspace-Zeiger ausschließlich per Software.

#### Vorteile

- funktioniert auf jeder Hardware,
- einfache frühe Implementierung,
- keine architekturspezifischen Kontrollregister.

#### Nachteile

- versehentliche direkte Zugriffe bleiben möglich,
- Kernelcode kann in Userspace umgeleitet werden,
- Softwareprüfungen können vergessen werden,
- geringere Defense-in-Depth.

Diese Alternative wird für unterstützte Hardware verworfen.

---

### 6.2 Nur SMEP

Der Kernel verhindert Userspace-Codeausführung, erlaubt aber weiterhin direkte Datenzugriffe.

#### Vorteile

- Schutz gegen Kernelkontrollfluss in Userspace-Code,
- geringere Komplexität als vollständiges SMAP.

#### Nachteile

- manipulierte Userspace-Zeiger bleiben gefährlich,
- direkte Dereferenzierungen werden nicht verhindert,
- unzureichender Schutz für Systemaufrufpfade.

Diese Alternative wird als mögliche Zwischenstufe, aber nicht als Zielarchitektur akzeptiert.

---

### 6.3 Nur SMAP

Der Kernel verhindert direkte Datenzugriffe, aber nicht die Ausführung von Userspace-Code.

#### Vorteile

- starke Trennung von Kernel- und Userspace-Datenzugriffen,
- erzwingt kontrollierte Copy-Schnittstellen.

#### Nachteile

- Kernelkontrollfluss könnte weiterhin in Userspace-Speicher gelangen,
- unvollständiger Schutz.

Diese Alternative wird verworfen.

---

### 6.4 SMEP und SMAP mit kontrollierter Uaccess-API

NovaOS aktiviert beide Mechanismen und erlaubt Userspace-Datenzugriffe ausschließlich über kleine kontrollierte Zugriffsfenster.

#### Vorteile

- starke Hardwaredurchsetzung,
- verhindert Kernelcodeausführung aus Userspace-Speicher,
- erzwingt zentrale Copy- und Validierungspfade,
- gut diagnostizierbare Verstöße,
- architekturübergreifend abstrahierbar.

#### Nachteile

- höhere Komplexität,
- Uaccess-Fault-Recovery notwendig,
- Interrupt- und Context-Switch-Regeln müssen exakt sein,
- ältere Hardware benötigt einen degradierten Modus.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS aktiviert SMEP und SMAP beziehungsweise architektonisch gleichwertige Mechanismen, sobald:

- die CPU sie unterstützt,
- die Seitentabellen korrekt eingerichtet sind,
- die Exceptionpfade bereitstehen,
- die Uaccess-API initialisiert ist.

Im normalen Kernelbetrieb gilt:

```text
Kernel darf Userspace-Code nicht ausführen.
Kernel darf Userspace-Daten nicht direkt lesen oder schreiben.
```

Zugriffe auf Userspace-Daten erfolgen ausschließlich über kontrollierte Uaccess-Funktionen.

---

## 8. Schutzmodell

NovaOS trennt vier grundlegende Fälle:

| Operation | Standard |
|---|---|
| Kernel führt Kernelcode aus | erlaubt |
| Kernel führt Userspace-Code aus | verboten |
| Kernel greift auf Kerneldata zu | erlaubt |
| Kernel greift direkt auf Userspace-Daten zu | verboten |
| kontrollierter Uaccess | zeitlich begrenzt erlaubt |
| Userspace führt Kernelcode aus | durch Seitentabellen verboten |
| Userspace greift auf Kerneldata zu | durch Seitentabellen verboten |

SMEP und SMAP ergänzen die normalen User/Supervisor-Seitenrechte.

---

## 9. SMEP-Aktivierung

Auf x86 wird SMEP aktiviert, wenn:

- CPUID die Fähigkeit meldet,
- Paging aktiv ist,
- Kernel- und Userspace-Seiten korrekt markiert sind,
- frühe Übergangsseiten überprüft wurden.

Die Aktivierung erfolgt über das architekturspezifische Kontrollregister.

Konzeptionell:

```c
if (nova_cpu_has_feature(NOVA_CPU_FEATURE_SMEP))
{
    nova_arch_enable_smep();
}
```

Die generische Sicherheitsschicht verwendet keine direkten x86-Kontrollregister.

---

## 10. SMAP-Aktivierung

SMAP wird aktiviert, nachdem die Uaccess- und Fault-Fixup-Infrastruktur einsatzbereit ist.

Konzeptionell:

```c
if (nova_cpu_has_feature(NOVA_CPU_FEATURE_SMAP))
{
    nova_arch_enable_smap();
}
```

Nach Aktivierung sind Userspace-Datenzugriffe aus dem Kernel standardmäßig gesperrt.

Eine versehentliche direkte Dereferenzierung löst einen Schutzfehler aus.

---

## 11. Aktivierungsreihenfolge

Die sichere Aktivierungsreihenfolge lautet:

```text
CPU-Fähigkeiten erkennen
        ↓
Kernel- und Userseiten korrekt markieren
        ↓
Exceptiontabellen initialisieren
        ↓
Uaccess-Fault-Fixups bereitstellen
        ↓
SMEP aktivieren
        ↓
SMAP aktivieren
        ↓
Schutzstatus prüfen
        ↓
normalen Userspace starten
```

Userspace-Prozesse dürfen nicht gestartet werden, bevor das aktive Schutzmodell festgelegt wurde.

---

## 12. Aktivierung auf allen CPUs

SMEP und SMAP sind CPU-lokale Hardwarezustände.

Jede neu gestartete CPU muss die Schutzfunktionen unabhängig aktivieren.

Eine CPU darf erst als online und schedulerfähig gelten, wenn:

- die erforderlichen Seitentabellen aktiv sind,
- SMEP entsprechend der Systempolicy gesetzt ist,
- SMAP entsprechend der Systempolicy gesetzt ist,
- die CPU-lokale Uaccess-Infrastruktur bereitsteht.

---

## 13. Einheitliche Systempolicy

NovaOS verhindert einen unkontrollierten Mischbetrieb, bei dem einige CPUs SMEP/SMAP verwenden und andere nicht, sofern Threads zwischen diesen CPUs migrieren können.

Mögliche Policies:

| Policy | Verhalten |
|---|---|
| `REQUIRE_UNIFORM` | nur CPUs mit erforderlichen Funktionen werden online geschaltet |
| `RESTRICT_CPUS` | sicherheitskritische Threads laufen nur auf geschützten CPUs |
| `SYSTEM_DEGRADED` | gesamtes System wird als degradiert markiert |
| `DISABLE_GLOBALLY` | Funktion wird systemweit deaktiviert |

Der Produktionsstandard ist `REQUIRE_UNIFORM`, soweit die Plattform dies unterstützt.

---

## 14. Architekturabstraktion

NovaOS definiert eine generische Privileged-Access-Schutzschicht.

Mögliche Architekturfunktionen:

```c
typedef struct nova_arch_user_access_ops
{
    nova_status_t (*initialize_cpu)(
        nova_cpu_t* cpu
    );

    void (*enable_execution_protection)(void);
    void (*enable_access_protection)(void);

    void (*begin_user_access)(void);
    void (*end_user_access)(void);

    bool (*is_user_access_enabled)(void);
    bool (*is_execution_protection_enabled)(void);
    bool (*is_access_protection_enabled)(void);
} nova_arch_user_access_ops_t;
```

Auf x86 werden diese Operationen durch SMEP, SMAP, `STAC` und `CLAC` umgesetzt.

Auf ARM können PXN, PAN und vergleichbare Mechanismen verwendet werden.

---

## 15. Uaccess-Grundregel

Kernelcode darf Userspace-Zeiger nicht direkt dereferenzieren.

Nicht zulässig:

```c
void syscall_write(const char* user_buffer, size_t size)
{
    kernel_output(user_buffer, size);
}
```

Zulässig:

```c
void syscall_write(const char* user_buffer, size_t size)
{
    char kernel_buffer[256];

    nova_status_t status = nova_copy_from_user(
        kernel_buffer,
        user_buffer,
        size
    );

    if (status != NOVA_STATUS_OK)
    {
        return;
    }

    kernel_output(kernel_buffer, size);
}
```

---

## 16. Uaccess-API

NovaOS stellt mindestens folgende Funktionen bereit:

```c
nova_status_t nova_copy_from_user(
    void* kernel_destination,
    const void* user_source,
    size_t size
);

nova_status_t nova_copy_to_user(
    void* user_destination,
    const void* kernel_source,
    size_t size
);

nova_status_t nova_clear_user(
    void* user_destination,
    size_t size
);

nova_status_t nova_copy_string_from_user(
    char* kernel_destination,
    size_t kernel_capacity,
    const char* user_source,
    size_t* copied_size
);

nova_status_t nova_user_range_validate(
    const void* user_address,
    size_t size,
    nova_user_access_t access
);
```

Die endgültige ABI wird in NPSPEC-SMEP-SMAP-0001 definiert.

---

## 17. Copy-in

`nova_copy_from_user()`:

1. validiert Adresse und Größe,
2. prüft Integerüberläufe,
3. prüft die Userspace-Adressgrenze,
4. öffnet ein kontrolliertes Lesezugriffsfenster,
5. kopiert die Daten,
6. behandelt erwartbare Seitenfehler,
7. schließt das Zugriffsfenster,
8. gibt den Status zurück.

Der Kernel verwendet anschließend ausschließlich die Kernelkopie.

---

## 18. Copy-out

`nova_copy_to_user()`:

1. validiert Zieladresse und Größe,
2. prüft Schreibberechtigung,
3. öffnet ein kontrolliertes Schreibzugriffsfenster,
4. kopiert die Daten,
5. behandelt erwartbare Seitenfehler,
6. schließt das Zugriffsfenster,
7. gibt den Status zurück.

Sensible Kernelpuffer müssen vor dem Copy-out vollständig initialisiert sein, damit keine nicht initialisierten Daten in den Userspace gelangen.

---

## 19. Bereichsvalidierung

Eine Userspace-Adresse wird nur als Bereich validiert, wenn:

```text
start liegt im Userspace
end liegt im Userspace
start + size erzeugt keinen Überlauf
gesamter Bereich liegt innerhalb einer zulässigen Grenze
```

Konzeptionell:

```c
bool nova_user_range_is_valid(
    uintptr_t address,
    size_t size
)
{
    if (size == 0)
    {
        return true;
    }

    if (address > NOVA_USER_ADDRESS_MAX)
    {
        return false;
    }

    if (size - 1 > NOVA_USER_ADDRESS_MAX - address)
    {
        return false;
    }

    return true;
}
```

Diese Prüfung ersetzt nicht den tatsächlichen zugriffsgeschützten Kopiervorgang.

---

## 20. Time-of-Check-to-Time-of-Use

Eine erfolgreiche Bereichsprüfung garantiert nicht, dass der Speicher beim späteren Zugriff unverändert bleibt.

Der Userspace kann möglicherweise:

- das Mapping ändern,
- Seiten freigeben,
- Schutzrechte ändern,
- Daten parallel verändern,
- einen anderen Thread schreiben lassen.

NovaOS verwendet deshalb die Regel:

> Userspace-Daten werden einmal kontrolliert in Kernelspeicher kopiert und anschließend dort validiert und verarbeitet.

Mehrfaches Nachlesen sicherheitsrelevanter Strukturen aus Userspace-Speicher ist zu vermeiden.

---

## 21. Verschachtelte Zeiger

Enthält eine kopierte Userspace-Struktur weitere Zeiger, werden diese nicht automatisch vertrauenswürdig.

Der Kernel muss:

1. äußere Struktur kopieren,
2. Feldgrößen validieren,
3. enthaltene Userspace-Zeiger einzeln prüfen,
4. referenzierte Daten separat kopieren,
5. eine vollständige Kernelrepräsentation erzeugen.

Beispiel:

```text
Userspace-Struktur
├── Länge
├── Flags
└── Zeiger auf Daten

Kernel:
1. Struktur kopieren
2. Länge und Flags prüfen
3. Daten über Zeiger getrennt kopieren
4. nur Kernelkopie weiterverwenden
```

---

## 22. Zeichenketten

Userspace-Zeichenketten werden immer mit einer maximalen Länge kopiert.

`nova_copy_string_from_user()` muss:

- einen Terminator innerhalb der Grenze verlangen,
- die Zielkapazität berücksichtigen,
- fehlenden Terminator als Fehler melden,
- partielle Ergebnisse klar behandeln,
- keine unbeschränkte Suche im Userspace durchführen.

Möglicher Fehler:

```text
NOVA_STATUS_STRING_TOO_LONG
```

---

## 23. Große Datenmengen

Große Datenmengen müssen nicht zwingend vollständig auf dem Kernelstack zwischengespeichert werden.

Zulässige Strategien:

- begrenzte Kernelheap-Puffer,
- gepinnte Seiten,
- kontrollierte Scatter/Gather-Listen,
- seitenweise Kopie,
- Shared-Memory-Objekte,
- asynchrone I/O-Objekte.

Auch diese Mechanismen müssen die Uaccess- und Capability-Regeln einhalten.

---

## 24. Gepinnte Userspace-Seiten

Für DMA oder asynchrone I/O können Userspace-Seiten vorübergehend gepinnt werden.

Der Ablauf muss:

1. Bereich validieren,
2. Seiten auflösen,
3. Zugriffsrechte prüfen,
4. Seiten referenzieren,
5. Änderungen oder Freigabe kontrollieren,
6. IOMMU- und DMA-Regeln anwenden,
7. Seiten nach Abschluss freigeben.

Das Pinnen erlaubt dem Kernel nicht, beliebige Userspace-Zeiger außerhalb kontrollierter APIs direkt zu verwenden.

---

## 25. User Access Window

Ein User Access Window ist immer:

- so kurz wie möglich,
- auf eine konkrete Operation begrenzt,
- nicht über allgemeine Funktionsaufrufe hinweg offen,
- nicht über einen Context Switch hinweg offen,
- nicht über eine Rückkehr in Userspace hinweg offen,
- nicht unbegrenzt verschachtelt.

Der bevorzugte Aufbau lautet:

```c
nova_user_access_begin();

result = nova_arch_copy_user(...);

nova_user_access_end();
```

Direkte Nutzung dieser Low-Level-Funktionen ist nur innerhalb der Uaccess-Implementierung zulässig.

---

## 26. x86-STAC/CLAC

Auf x86 verwendet NovaOS bei aktiviertem SMAP:

```text
STAC → kontrollierten Userspace-Datenzugriff erlauben
CLAC → Userspace-Datenzugriff wieder sperren
```

Die Sequenz muss sicherstellen, dass `CLAC` in allen Kontrollpfaden ausgeführt wird:

- Erfolg,
- Fehler,
- Seitenfehler,
- Exception Fixup,
- frühe Rückkehr.

Ein offenes Access-Flag darf nicht versehentlich bestehen bleiben.

---

## 27. Keine offenen Uaccess-Fenster bei Context Switch

Ein Thread darf keinen Context Switch ausführen, während sein User Access Window geöffnet ist.

Vor einem Schedulerwechsel muss gelten:

```text
user_access_depth == 0
access_override_disabled == true
```

Ein Verstoß stellt eine Kernelinvariante dar.

Blockierende Operationen innerhalb eines geöffneten Uaccess-Fensters sind verboten.

---

## 28. Präemption

Während eines Low-Level-Uaccess-Abschnitts kann Präemption vorübergehend deaktiviert werden, wenn die Architektur den Zugriffszustand CPU-lokal und nicht threadbezogen verwaltet.

Der Abschnitt muss sehr kurz bleiben.

Alternativ kann der Zugriffszustand im Threadkontext gespeichert und bei jedem Context Switch sicher behandelt werden. NovaOS bevorzugt jedoch:

```text
kein Context Switch bei offenem Uaccess-Fenster
```

Dadurch bleibt die Sicherheitssemantik einfacher und besser überprüfbar.

---

## 29. Interrupts

Ein Interrupt kann während eines Uaccess-Vorgangs auftreten.

Der Interrupt-Entry-Pfad muss sicherstellen, dass normaler Interruptcode nicht unbeabsichtigt mit geöffnetem Userspace-Zugriff läuft.

Auf unterstützten Architekturen gilt:

1. Zugriffszustand beim Eintritt sichern,
2. Userspace-Zugriff für den Interruptpfad sperren,
3. Interrupt behandeln,
4. vorherigen Zustand nur für den kontrollierten Uaccess-Pfad wiederherstellen.

Ein Interrupt-Handler darf nicht davon ausgehen, dass SMAP bereits gesperrt war.

---

## 30. Exceptions

Eine Exception während eines Uaccess-Vorgangs kann erwartet oder unerwartet sein.

Erwartbar sind beispielsweise:

- Seite nicht vorhanden,
- Userspace-Ziel schreibgeschützt,
- Mapping während der Kopie verändert.

Solche Fehler können über Fault Fixups in einen kontrollierten Status umgewandelt werden.

Unerwartete Kernelzugriffe außerhalb eines Uaccess-Bereichs werden als Kernel- oder Sicherheitsfehler behandelt.

---

## 31. Fault-Fixup-Tabellen

NovaOS verwendet architekturspezifische Exception- beziehungsweise Fixup-Tabellen.

Ein Eintrag verbindet:

- fehleranfällige Uaccess-Instruktion,
- sicheren Fortsetzungspunkt,
- zurückzugebenden Fehlerstatus.

Konzeptionell:

```text
fault_instruction → fixup_instruction
```

Die Tabellen:

- sind schreibgeschützt,
- werden durch Secure Startup verifiziert,
- enthalten nur kontrollierte Uaccess-Bereiche,
- dürfen nicht beliebige Kernel-Faults verschlucken.

---

## 32. Seitenfehler während Uaccess

Ein Seitenfehler während Uaccess kann abhängig vom Kontext:

- eine Seite regulär einlagern,
- Copy-on-Write auflösen,
- kontrolliert fehlschlagen,
- einen Teiltransfer melden.

Nicht zulässig ist eine unbeschränkte Fehlerbehandlung in:

- Interruptkontext,
- atomaren Kernelabschnitten,
- gehaltenen Spinlocks,
- harten Echtzeitabschnitten.

Die Uaccess-API muss angeben, ob sie Fehlerbehandlung mit potenzieller Blockierung erlaubt.

---

## 33. Atomare Uaccess-Varianten

NovaOS kann nicht blockierende Uaccess-Varianten bereitstellen.

Beispiel:

```c
nova_status_t nova_copy_from_user_atomic(
    void* kernel_destination,
    const void* user_source,
    size_t size
);
```

Diese Variante:

- löst keine blockierenden Seitenfehler auf,
- gibt bei nicht sofort zugreifbarem Speicher einen Fehler zurück,
- ist für kurze atomare Kernelpfade vorgesehen,
- darf keine normale Heap-Allokation durchführen.

---

## 34. Echtzeit-Uaccess

In harten Echtzeitabschnitten sind unvorhersehbare Seitenfehler unzulässig.

Userspace-Puffer müssen vorab:

- validiert,
- gepinnt,
- in den Speicher geladen,
- mit geeigneten Zugriffsrechten versehen

werden.

Der Echtzeitpfad verwendet ausschließlich vorab vorbereitete Uaccess-Objekte oder gebundene Shared-Memory-Bereiche.

SMEP und SMAP bleiben dabei aktiv.

---

## 35. Systemaufrufe

Der Systemaufruf-Dispatcher behandelt alle Pointerargumente zunächst als nicht vertrauenswürdige Userspace-Adressen.

Ein Systemaufruf darf:

- keinen Userspace-Zeiger als Kernelzeiger interpretieren,
- keine Userspace-Struktur ohne Kopie weiterreichen,
- keine Userspace-Funktionsadresse im Kernelmodus aufrufen,
- keine unvalidierte Länge verwenden.

Die Systemaufrufimplementierung verwendet ausschließlich die Uaccess-API.

---

## 36. IPC

IPC-Nachrichten werden entweder:

- in Kernelpuffer kopiert,
- über kontrollierte Shared-Memory-Objekte übertragen,
- über gepinnte Seiten beschrieben.

Ein IPC-Dienst darf keine rohen Zeiger aus dem Adressraum eines anderen Prozesses direkt dereferenzieren.

Prozessübergreifende Speicherübertragung erfolgt über explizite Speicherobjekte und Capabilities.

---

## 37. Kernelthreads

Kernelthreads besitzen keinen normalen Userspace-Adressraum.

Sie dürfen standardmäßig keine Uaccess-Operationen ausführen.

Benötigt ein Kernelthread Zugriff auf den Adressraum eines Prozesses, muss er:

- eine explizite Adressraumreferenz besitzen,
- die erforderliche Capability besitzen,
- den Zieladressraum kontrolliert aktivieren oder über Remote-Copy-APIs zugreifen,
- dieselben Uaccess-Regeln einhalten.

Der zuletzt auf der CPU aktive Userspace-Adressraum darf nicht stillschweigend weiterverwendet werden.

---

## 38. Context Switching

Beim Context Switch wird geprüft:

```text
user_access_depth == 0
SMAP override ist deaktiviert
kein offener Fault-Fixup-Zustand
```

Der Zielthread startet immer mit gesperrtem privilegiertem Userspace-Datenzugriff.

SMEP bleibt dauerhaft aktiv, sobald es auf der CPU eingeschaltet wurde.

Ein Userspace-Zugriffsfenster wird niemals von einem Thread an einen anderen vererbt.

---

## 39. Rückkehr in Userspace

Vor der Rückkehr in den Userspace prüft der Kernel mindestens:

- Uaccess-Fenster geschlossen,
- Access Override deaktiviert,
- SMEP aktiv,
- SMAP beziehungsweise Äquivalent aktiv,
- gültiger Userspace Instruction Pointer,
- gültiger Userspace Stack Pointer,
- Trap Frame validiert.

Ein offenes Uaccess-Fenster verhindert die Rückkehr und löst einen Kernelintegritätsfehler aus.

---

## 40. Kernel-Funktionszeiger

Alle Kernel-Funktionszeiger müssen auf ausführbare Kernelcodebereiche zeigen.

Vor besonders kritischen indirekten Aufrufen kann der Kernel optional prüfen:

```text
target liegt in:
    Kerneltext
    oder finalisiertem Modultext
    oder autorisiertem Codeobjekt
```

Eine Adresse in einer Userspace-Seite ist niemals ein gültiges Kernel-Aufrufziel.

SMEP bildet dafür die Hardware-Rückfallebene.

---

## 41. Upcalls und Callbacks

NovaOS führt Userspace-Callbacks nicht im Kernelmodus aus.

Ein Callback wird realisiert durch:

1. Ereignis im Kernel erzeugen,
2. Userspace-Thread vorbereiten oder aufwecken,
3. validierten Userspace-Kontext setzen,
4. kontrolliert in den Userspace zurückkehren.

Der Kernel springt niemals direkt im Supervisor Mode zu einer Userspace-Adresse.

---

## 42. KASLR-Integration

SMEP verhindert, dass ein Angreifer KASLR umgeht, indem der Kernelkontrollfluss einfach auf bekannten Userspace-Code umgeleitet wird.

KASLR schützt die Position des Kernelcodes.

SMEP stellt sicher, dass Userspace-Speicher kein Ersatz für Kernelcode sein kann.

Beide Mechanismen ergänzen sich, ersetzen aber keine Kontrollflussvalidierung.

---

## 43. NX-Integration

NX und SMEP erfüllen unterschiedliche Aufgaben.

| Mechanismus | Schutz |
|---|---|
| NX | verhindert Ausführung aus als NX markierten Seiten |
| SMEP | verhindert Kernel-Ausführung aus Userspace-Seiten, auch wenn diese im Userspace ausführbar sind |
| SMAP | verhindert Kernel-Datenzugriff auf Userspace-Seiten |
| User/Supervisor-Bit | verhindert Userspace-Zugriff auf Kernelseiten |

Eine Userspace-Codeseite kann für Userspace ausführbar sein und muss dennoch für den Kernel durch SMEP gesperrt bleiben.

---

## 44. Stack Protector

Ein Stack Overflow könnte versuchen, eine Kernel-Rücksprungadresse auf Userspace-Code umzulenken.

Der Stack Protector soll die Überschreibung erkennen.

SMEP verhindert zusätzlich die privilegierte Ausführung des Userspace-Ziels.

Beide Schutzmechanismen werden gemeinsam eingesetzt.

---

## 45. Capability-System

Eine Capability für ein Userspace-Speicherobjekt erlaubt nicht automatisch direkte Kernel-Dereferenzierung.

Capabilities autorisieren:

- welche Operation zulässig ist,
- welcher Bereich betroffen ist,
- welcher Zugriffstyp erlaubt ist.

Der tatsächliche Datentransfer erfolgt weiterhin über Uaccess- oder Shared-Memory-Schnittstellen.

Eine Capability darf SMEP oder SMAP nicht deaktivieren.

---

## 46. Copy statt dauerhafter Zugriff

Für kleine sicherheitsrelevante Strukturen gilt:

> kopieren, validieren, intern verwenden.

Beispiele:

- Systemaufrufparameter,
- Dateipfade,
- Sicherheitsbeschreibungen,
- Capability-Anforderungen,
- Modulmetadaten,
- Netzwerkadressstrukturen.

Dadurch verhindert NovaOS, dass Userspace-Daten nach ihrer Validierung unbemerkt verändert werden.

---

## 47. Shared-Memory-Objekte

Für große oder häufig verwendete Daten kann NovaOS Shared-Memory-Objekte einsetzen.

Der Kernel behandelt solche Objekte nicht automatisch als vertrauenswürdig.

Mögliche Sicherheitsmodelle:

- unveränderlicher Snapshot,
- Producer-/Consumer-Ring mit atomaren Protokollen,
- explizite Eigentumsübergabe,
- versiegelte Seiten,
- generationengesicherte Puffer.

SMAP bleibt für direkte Userspace-Mappings aktiv.

---

## 48. Debugger

Ein Kerneldebugger darf SMEP oder SMAP nicht dauerhaft deaktivieren.

Benötigt er Userspace-Speicherzugriff, verwendet er dieselben kontrollierten Uaccess-Schnittstellen.

Temporäre Debug-Ausnahmen benötigen:

- spezielle Capability,
- aktive Debugpolicy,
- lokale Telemetrie,
- garantierte Wiederaktivierung.

Ein Debugfehler darf keine CPU unbemerkt mit deaktiviertem Schutz zurücklassen.

---

## 49. CPU-Hotplug

Beim Online-Schalten einer CPU wird geprüft:

- SMEP-Unterstützung,
- SMAP-Unterstützung,
- Architekturäquivalente,
- Systempolicy,
- erfolgreiche Aktivierung.

Eine CPU, die die erforderliche Produktionspolicy nicht erfüllt, wird:

- nicht online geschaltet,
- einem eingeschränkten CPU-Pool zugeordnet oder
- führt zu einem degradierten Systemzustand.

Das Verhalten wird vor der Threadmigration festgelegt.

---

## 50. Suspend und Resume

Nach Suspend oder Hibernate müssen SMEP und SMAP auf jeder CPU erneut geprüft und aktiviert werden.

Vor Wiederaufnahme normaler Threads gelten mindestens:

```text
SMEP entsprechend Policy aktiv
SMAP entsprechend Policy aktiv
Uaccess Override geschlossen
Seitentabellen gültig
```

Ein manipuliertes oder inkompatibles Resume darf den Schutz nicht stillschweigend deaktivieren.

---

## 51. Virtualisierung

Unter einem Hypervisor kann SMEP/SMAP:

- direkt virtualisiert,
- emuliert,
- ausgeblendet,
- fehlerhaft implementiert

sein.

NovaOS prüft die tatsächlich sichtbaren CPU-Fähigkeiten und führt Selbsttests aus.

Das System verlässt sich nicht allein auf eine Hypervisorangabe.

Der Trust State kennzeichnet virtualisierte Schutzfunktionen.

---

## 52. Plattform ohne SMEP

Fehlt SMEP, verwendet NovaOS weiterhin:

- NX,
- KASLR,
- Kerneladressprüfung,
- Kontrollflussvalidierung,
- getrennte User/Supervisor-Seiten,
- Stack Protector.

Das System wird jedoch als SMEP-degradiert gekennzeichnet.

Eine Plattformpolicy kann den Produktionsstart ohne SMEP ablehnen.

---

## 53. Plattform ohne SMAP

Fehlt SMAP, bleiben die Uaccess-APIs trotzdem verpflichtend.

Softwareprüfungen umfassen:

- Userspace-Adressgrenzen,
- Zugriffsrechte,
- Exception Fixups,
- Copy-in/Copy-out,
- TOCTOU-Vermeidung.

Der Schutzstatus wird als SMAP-degradiert markiert.

Kernelcode darf fehlende Hardwareunterstützung nicht als Erlaubnis für direkte Userspace-Dereferenzierungen behandeln.

---

## 54. Sicherheitsmodi

Mögliche Policies sind:

| Modus | SMEP | SMAP | Verhalten |
|---|---:|---:|---|
| `ENFORCING` | erforderlich, wenn Plattformprofil dies verlangt | erforderlich, wenn Plattformprofil dies verlangt | Start oder CPU-Online kann abgelehnt werden |
| `STANDARD` | verwenden, wenn verfügbar | verwenden, wenn verfügbar | fehlende Funktionen werden als degradiert markiert |
| `LEGACY` | optional | optional | Software-Uaccess bleibt verpflichtend |
| `DEVELOPMENT` | kontrolliert testbar | kontrolliert testbar | Abweichungen werden deutlich markiert |

Ein stilles automatisches Abschalten nach einem Schutzfehler ist nicht zulässig.

---

## 55. Trust State

Der Sicherheitsstatus enthält mindestens:

```text
SMEP_SUPPORTED
SMEP_ENABLED
SMAP_SUPPORTED
SMAP_ENABLED
UACCESS_ENFORCED
FAULT_FIXUPS_ACTIVE
PRIVILEGED_ACCESS_DEGRADED
VIRTUALIZED_PROTECTION
```

Die Statuswerte werden pro System und gegebenenfalls pro CPU erfasst.

---

## 56. API-Grundmodell

Eine mögliche Uaccess-API lautet:

```c
typedef enum
{
    NOVA_USER_ACCESS_READ  = 1u << 0,
    NOVA_USER_ACCESS_WRITE = 1u << 1
} nova_user_access_t;

nova_status_t nova_user_range_validate(
    const void* user_address,
    size_t size,
    nova_user_access_t access
);

nova_status_t nova_copy_from_user(
    void* kernel_destination,
    const void* user_source,
    size_t size
);

nova_status_t nova_copy_to_user(
    void* user_destination,
    const void* kernel_source,
    size_t size
);

nova_status_t nova_copy_string_from_user(
    char* kernel_destination,
    size_t kernel_capacity,
    const char* user_source,
    size_t* copied_size
);

nova_status_t nova_clear_user(
    void* user_destination,
    size_t size
);
```

Low-Level-Funktionen zum Öffnen eines Zugriffsfeldes werden nicht als allgemeine Kernel-API veröffentlicht.

---

## 57. Uaccess-Ergebnis

Eine Copy-Funktion gibt einen eindeutigen Status und optional die tatsächlich kopierte Größe zurück.

```c
typedef struct
{
    nova_status_t status;
    size_t bytes_completed;
} nova_user_copy_result_t;
```

Mögliche Semantiken:

- vollständig oder Fehler,
- definierter Teiltransfer,
- kein Teiltransfer bei sicherheitskritischen Strukturen.

Die jeweilige API muss ihre Semantik eindeutig dokumentieren.

---

## 58. Kernelinterner Uaccess-Zustand

Jede CPU beziehungsweise jeder aktuelle Thread enthält mindestens:

```c
typedef struct
{
    uint32_t depth;
    uint32_t flags;

    uintptr_t active_start;
    size_t active_size;

    nova_user_access_t access;
    nova_status_t fault_status;
} nova_uaccess_state_t;
```

Die konkrete Implementierung darf kleiner sein, muss aber:

- Verschachtelung erkennen,
- offene Fenster vor Context Switch erkennen,
- Fault Fixups zuordnen,
- Diagnose ermöglichen.

---

## 59. Verschachtelung

Uaccess-Fenster sollen grundsätzlich nicht verschachtelt werden.

Falls eine interne Architekturimplementierung Verschachtelung benötigt, wird ein begrenzter Zähler verwendet.

Es gilt:

```text
begin:
    depth += 1

end:
    depth -= 1

depth == 0:
    Hardwarezugriff wieder vollständig sperren
```

Unterlauf, Überlauf oder Rückkehr mit einem nicht null gesetzten Zähler gilt als Kernelintegritätsfehler.

---

## 60. Compilerbarrieren

Der Compiler darf Speicherzugriffe nicht außerhalb des kontrollierten Uaccess-Fensters verschieben.

`begin_user_access()` und `end_user_access()` benötigen deshalb geeignete:

- Compilerbarrieren,
- Hardwarebarrieren,
- architekturspezifische Instruktionssemantik.

Eine reine Funktionskonvention ohne Barrieren ist nicht ausreichend.

---

## 61. Spekulative Ausführung

SMAP und Bereichsprüfung allein verhindern nicht jede spekulative Seiteneffektmöglichkeit.

Uaccess-Funktionen müssen bei Bedarf:

- Bounds Checks absichern,
- Spekulationsbarrieren verwenden,
- maskierte Indizes einsetzen,
- geheime Kernelwerte nicht abhängig von Userspace-Indizes laden.

Die konkrete Spectre-Hardening-Policy wird in einer gesonderten ADR spezifiziert.

---

## 62. Fehlerbehandlung

Mögliche Fehlercodes sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_INVALID_USER_ADDRESS
NOVA_STATUS_USER_RANGE_OVERFLOW
NOVA_STATUS_USER_READ_DENIED
NOVA_STATUS_USER_WRITE_DENIED
NOVA_STATUS_USER_PAGE_NOT_PRESENT
NOVA_STATUS_USER_COPY_PARTIAL
NOVA_STATUS_USER_ACCESS_FAULT
NOVA_STATUS_USER_STRING_TOO_LONG
NOVA_STATUS_UACCESS_IN_ATOMIC_CONTEXT
NOVA_STATUS_UACCESS_WINDOW_OPEN
NOVA_STATUS_SMEP_UNSUPPORTED
NOVA_STATUS_SMAP_UNSUPPORTED
NOVA_STATUS_PROTECTION_DEGRADED
```

Ungültige Userspace-Eingaben führen nicht zu einer Kernel-Panic.

Eine Kernel-Panic ist jedoch zulässig bei:

- SMEP-Verstoß im Kernel,
- unerwartetem SMAP-Verstoß außerhalb Uaccess,
- offenem Uaccess-Fenster bei Context Switch,
- beschädigter Fixup-Tabelle,
- deaktiviertem Schutz trotz Enforcing Policy.

---

## 63. Verhalten bei SMEP-Verstoß

Versucht der Kernel, Code aus einer Userspace-Seite auszuführen:

1. CPU-Fault erfassen,
2. Instruction Pointer und Seitentyp prüfen,
3. andere CPUs bei Bedarf stoppen,
4. geschützten Crash-Kontext sichern,
5. Sicherheitsereignis erzeugen,
6. Kernel-Panic auslösen.

Ein SMEP-Verstoß ist keine normale Userspace-Exception.

---

## 64. Verhalten bei SMAP-Verstoß

Ein SMAP-Verstoß wird unterschieden nach:

### 64.1 Innerhalb eines gültigen Uaccess-Fixups

Der Zugriff wird kontrolliert abgebrochen und als Uaccess-Fehler zurückgegeben.

### 64.2 Außerhalb eines Uaccess-Bereichs

Es liegt ein Kernelprogrammier- oder Sicherheitsfehler vor.

Standardreaktion:

- Diagnose sichern,
- Kernel-Panic auslösen,
- Recovery-Policy anwenden.

---

## 65. Telemetrie und Diagnose

NovaOS erfasst mindestens:

- SMEP-Unterstützung und Aktivierung,
- SMAP-Unterstützung und Aktivierung,
- Architekturäquivalent,
- Anzahl erfolgreicher Uaccess-Operationen,
- Anzahl fehlgeschlagener Copy-ins,
- Anzahl fehlgeschlagener Copy-outs,
- Anzahl partieller Transfers,
- Anzahl Fault Fixups,
- Anzahl unerwarteter Schutzverletzungen,
- degradierter Schutzstatus,
- CPU-Hotplug-Abweisungen.

Userspace-Adressen werden entsprechend der Datenschutz- und Adressschutzpolicy maskiert.

---

## 66. Performance

SMEP verursacht im normalen Betrieb geringe zusätzliche Kosten.

SMAP erzeugt Kosten durch:

- Öffnen und Schließen kurzer Zugriffsfelder,
- zusätzliche Bereichsprüfungen,
- Copy-in und Copy-out,
- Fault-Fixup-Infrastruktur.

NovaOS akzeptiert diese Kosten für die klare Trennung von Kernel- und Userspace-Speicher.

Optimierungen dürfen erfolgen durch:

- architekturoptimierte Copy-Routinen,
- blockweise Kopie,
- gepinnte Seiten für große Transfers,
- Shared-Memory-Protokolle,
- Vermeidung mehrfacher Kopien.

Optimierungen dürfen Uaccess- und Sicherheitsgrenzen nicht umgehen.

---

## 67. Konsequenzen

### 67.1 Positive Konsequenzen

- Kernel kann keinen Userspace-Code im Supervisor Mode ausführen,
- direkte Userspace-Dereferenzierungen werden verhindert,
- zentrale und überprüfbare Uaccess-Schnittstellen,
- Schutz gegen viele Return-to-User-Angriffe,
- klare Fehlerbehandlung für ungültige Zeiger,
- bessere Trennung von Kernel und Userspace,
- gute Integration mit NX, KASLR und Stack Protector,
- architekturübergreifendes Schutzmodell.

### 67.2 Negative Konsequenzen

- höhere Komplexität der Copy- und Fault-Pfade,
- zusätzliche Laufzeitkosten,
- Interrupt- und Context-Switch-Integration erforderlich,
- ältere Hardware bietet nur degradierten Schutz,
- Treiber und Kernelkomponenten müssen konsequent angepasst werden.

### 67.3 Risiken

- vergessenes Schließen eines Uaccess-Fensters,
- Context Switch mit geöffnetem SMAP-Override,
- Interruptcode läuft mit offenem Zugriff,
- fehlerhafte Fixup-Tabelle,
- direkte Userspace-Dereferenzierung auf Hardware ohne SMAP,
- TOCTOU bei mehrfach gelesenen Strukturen,
- unvollständige CPU-Hotplug-Aktivierung,
- unsichere Debugausnahmen.

Diese Risiken werden begrenzt durch:

- kleine zentrale Uaccess-API,
- automatische Scope-Helfer,
- Context-Switch-Invarianten,
- Interrupt-Entry-Hardening,
- schreibgeschützte Fixup-Tabellen,
- Buildprüfungen,
- Laufzeitselbsttests,
- vollständige Telemetrie.

---

## 68. Auswirkungen auf andere Subsysteme

### 68.1 Virtuelle Speicherverwaltung

Die VM-Schicht muss User/Supervisor-Seiten korrekt markieren und sichere Bereichsprüfungen bereitstellen.

### 68.2 Systemaufrufe

Alle Pointerargumente werden ausschließlich über Uaccess-Funktionen verarbeitet.

### 68.3 Context Switching

Offene Uaccess-Fenster sind bei einem Context Switch verboten.

### 68.4 Interruptsystem

Interrupt-Entry muss privilegierten Userspace-Zugriff unabhängig vom unterbrochenen Zustand sperren.

### 68.5 Exception Handling

Erwartbare Uaccess-Faults benötigen kontrollierte Fixup-Pfade.

### 68.6 Prozessverwaltung

Prozessadressräume müssen klare Userspace-Grenzen und Lebenszyklusregeln besitzen.

### 68.7 IPC

Prozessübergreifende Daten werden kopiert oder über explizite Shared-Memory-Objekte übertragen.

### 68.8 Echtzeitsystem

Echtzeit-Uaccess benötigt vorab gepinnte und validierte Speicherbereiche.

### 68.9 Capability-System

Capabilities autorisieren Speicherobjekte, deaktivieren aber niemals SMEP oder SMAP.

### 68.10 Debugger

Debugger verwenden dieselben kontrollierten Speicherzugriffsschnittstellen.

### 68.11 Secure Startup

Der aktive SMEP/SMAP-Status wird in den System-Trust-State übernommen.

---

## 69. Implementierungsrichtlinien

1. Kernel- und Userspace-Seiten werden eindeutig über Seitentabellenattribute getrennt.
2. SMEP wird aktiviert, sobald sichere Seitentabellen aktiv sind.
3. SMAP wird aktiviert, sobald Uaccess und Fault Fixups bereitstehen.
4. Jede online geschaltete CPU muss die aktive Systempolicy erfüllen.
5. Direkte Userspace-Zeiger-Dereferenzierung ist im Kernel verboten.
6. Alle Systemaufrufpointer werden über Uaccess verarbeitet.
7. Kleine sicherheitsrelevante Strukturen werden einmal in Kernelspeicher kopiert.
8. Uaccess-Fenster bleiben möglichst kurz.
9. Uaccess darf nicht über einen Context Switch hinweg offen bleiben.
10. Normale Interruptpfade laufen mit gesperrtem Userspace-Datenzugriff.
11. Fault-Fixup-Tabellen sind schreibgeschützt und auf Uaccess-Bereiche begrenzt.
12. Kernelthreads verwenden keine impliziten Userspace-Adressräume.
13. Hardware ohne SMAP verwendet weiterhin verpflichtende Software-Uaccess-APIs.
14. Fehlende Hardwarefunktionen werden im Trust State sichtbar gemacht.
15. Ein SMEP-Verstoß führt zu einer Kernel-Panic.
16. Ein unerwarteter SMAP-Verstoß außerhalb von Uaccess führt zu einer Kernel-Panic.
17. Compiler- und Speicherbarrieren verhindern das Verschieben von Zugriffen aus dem Uaccess-Fenster.
18. Debug- und Development-Ausnahmen dürfen den Schutz nicht dauerhaft deaktivieren.
19. Suspend und CPU-Hotplug prüfen die Schutzfunktionen erneut.
20. Uaccess-Code wird durch statische Analyse, Fuzzing und Laufzeittests besonders geprüft.

---

## 70. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- SMEP-Fähigkeitserkennung,
- SMAP-Fähigkeitserkennung,
- Aktivierung auf der Boot-CPU,
- Aktivierung auf zusätzlichen CPUs,
- CPU-Hotplug ohne erforderliche Funktionen,
- Kernelversuch, Userspace-Code auszuführen,
- direkter Kernelzugriff auf Userspace-Daten,
- gültiger `copy_from_user`,
- gültiger `copy_to_user`,
- ungültige Userspace-Adresse,
- Kerneladresse als Userspace-Zeiger,
- Bereichsüberlauf,
- nicht gemappte Seite,
- schreibgeschützte Zielseite,
- Mappingänderung während Copy-in,
- Mappingänderung während Copy-out,
- partielle Kopie,
- begrenzte Zeichenkette,
- fehlender Stringterminator,
- verschachtelte Pointerstruktur,
- großer gepinnter Puffer,
- atomarer Uaccess,
- Uaccess in hartem Echtzeitabschnitt,
- Page Fault mit gültigem Fixup,
- Page Fault ohne Fixup,
- beschädigte Fixup-Tabelle,
- Interrupt während Uaccess,
- verschachtelter Interrupt während Uaccess,
- Context-Switch-Versuch mit offenem Fenster,
- Rückkehr in Userspace mit offenem Fenster,
- Kernelthread ohne Userspace-Adressraum,
- CPU-Migration,
- Suspend und Resume,
- Hibernate,
- Virtualisierung,
- Hardware ohne SMEP,
- Hardware ohne SMAP,
- Legacy-Modus,
- Debuggerzugriff,
- NX und SMEP gemeinsam,
- Stack Protector und SMEP gemeinsam,
- TOCTOU-Fuzzing,
- Systemaufruf-Fuzzing,
- Telemetrie und Fault-Zähler.

---

## 71. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- die konkrete x86-Assembly der Uaccess-Routinen,
- die vollständige ARM-PAN/PXN-Implementierung,
- Spectre- und Meltdown-Mitigationen,
- vollständige Control-Flow Integrity,
- die genaue Seitentabellenarchitektur,
- Prozessübergreifende Remote-Copy-APIs,
- vollständige Shared-Memory-Protokolle,
- die endgültige Uaccess-ABI,
- konkrete CPU-Pool-Policies für heterogene Schutzfunktionen.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 72. Verbindliche Invarianten

1. Kernelcode darf niemals im Supervisor Mode aus Userspace-Seiten ausgeführt werden.
2. Direkte Kernelzugriffe auf Userspace-Daten sind standardmäßig verboten.
3. Userspace-Datenzugriffe erfolgen ausschließlich über kontrollierte Uaccess-Schnittstellen.
4. SMEP bleibt nach seiner Aktivierung während des normalen Kernelbetriebs aktiv.
5. SMAP beziehungsweise sein Architekturäquivalent bleibt außerhalb kurzer Uaccess-Fenster aktiv.
6. Ein Uaccess-Fenster darf nicht über einen Context Switch hinweg offen bleiben.
7. Ein Uaccess-Fenster darf nicht über eine Rückkehr in den Userspace hinweg offen bleiben.
8. Normaler Interruptcode darf nicht unbeabsichtigt mit geöffnetem Userspace-Zugriff laufen.
9. Jeder Userspace-Bereich wird auf Grenzen und Integerüberläufe geprüft.
10. Eine erfolgreiche Bereichsprüfung ersetzt nicht den faultgeschützten Zugriff.
11. Sicherheitsrelevante Userspace-Strukturen werden nach Möglichkeit einmal kopiert und anschließend als Kernelkopie verarbeitet.
12. Verschachtelte Userspace-Zeiger werden einzeln validiert und kopiert.
13. Fault Fixups dürfen nur bekannte Uaccess-Instruktionen behandeln.
14. Kernelthreads dürfen nicht implizit den Userspace-Adressraum des zuvor laufenden Threads verwenden.
15. Ein Kernel-Funktionszeiger darf niemals auf eine Userspace-Seite zeigen.
16. Userspace-Callbacks werden nur durch kontrollierte Rückkehr in den Userspace ausgeführt.
17. Eine Capability darf SMEP, SMAP oder die Uaccess-Prüfung nicht umgehen.
18. Fehlende Hardwareunterstützung muss im Sicherheitsstatus sichtbar sein.
19. Software-Uaccess-Regeln bleiben auch ohne SMAP verpflichtend.
20. Ein SMEP-Verstoß erlaubt keinen normalen Kernelweiterbetrieb.
21. Ein unerwarteter SMAP-Verstoß außerhalb eines gültigen Uaccess-Pfads erlaubt keinen normalen Kernelweiterbetrieb.
22. Jede online geschaltete CPU muss den für sie gemeldeten Schutz tatsächlich aktiviert haben.
23. SMEP/SMAP ersetzen weder NX, KASLR, Stack Protector, Capability-Prüfung noch Speichersicherheit.
24. KI- oder lernbasierte Komponenten dürfen SMEP-, SMAP-, Uaccess-, Fault-, Capability-, Debug- oder Secure-Startup-Regeln nicht umgehen.

---

## 73. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-SEC-0004 – NX Memory
- ADR-MM-0002 – Virtual Memory Management
- ADR-MM-0013 – Guard Pages
- ADR-MM-0015 – Kernel Stack Layout
- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0007 – CPU Affinity
- ADR-PROC-0008 – Context Switching
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- NPSPEC-SMEP-SMAP-0001 – SMEP/SMAP Specification
- NPSPEC-NXMEMORY-0001 – NX Memory Specification
- NPSPEC-VIRTUALMEMORY-0001 – Virtual Memory Specification
- NPSPEC-UACCESS-0001 – Userspace Access Specification
- NPSPEC-SYSCALL-0001 – System Call Specification
- NPSPEC-EXCEPTION-0001 – Exception Handling Specification

---

## 74. Zusammenfassung der Entscheidung

NovaOS aktiviert SMEP und SMAP beziehungsweise architektonisch gleichwertige Schutzmechanismen, sobald Seitentabellen, Exception Handling und Uaccess-Infrastruktur bereitstehen.

SMEP verhindert, dass der Kernel im privilegierten CPU-Modus Code aus Userspace-Seiten ausführt. SMAP verhindert, dass der Kernel Userspace-Daten außerhalb eines ausdrücklich geöffneten Zugriffsfeldes liest oder schreibt.

Kernelkomponenten dürfen Userspace-Zeiger nicht direkt dereferenzieren. Alle Datenübertragungen erfolgen über kontrollierte Funktionen wie:

- `nova_copy_from_user()`,
- `nova_copy_to_user()`,
- `nova_copy_string_from_user()`,
- `nova_clear_user()`.

Uaccess-Fenster bleiben kurz, dürfen keinen Context Switch überdauern und werden in Interruptpfaden standardmäßig geschlossen. Erwartbare Seitenfehler werden ausschließlich über schreibgeschützte Fault-Fixup-Tabellen behandelt.

Fehlt SMEP oder SMAP auf älterer Hardware, bleiben die Software-Uaccess-Regeln vollständig verbindlich. Das System wird jedoch als sicherheitstechnisch degradiert gekennzeichnet.

SMEP, SMAP, NX, KASLR, Stack Protector, User/Supervisor-Seitenrechte und das Capability-System bilden gemeinsam eine mehrschichtige Trennung zwischen Kernel und Userspace.