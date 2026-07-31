# ADR-DIAG-0003 – Crash Dumps

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DIAG-0003 |
| Titel | Crash Dumps |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernel-Diagnose und Fehleranalyse |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DIAG-0001, ADR-DIAG-0002, ADR-SEC-0002, ADR-SEC-0008, ADR-SEC-0009, ADR-SEC-0010 |
| Nachfolgende Spezifikation | NPSPEC-DIAG-CRASH-DUMPS |

---

## 1. Kontext

Nach einer Kernel Panic oder einem schwerwiegenden Systemfehler sind gewöhnliche Diagnosemöglichkeiten nur eingeschränkt verfügbar.

Ein Crash Dump soll genügend Informationen sichern, um Ursachen wie folgende untersuchen zu können:

- ungültige Speicherzugriffe
- Treiberfehler
- Deadlocks
- beschädigte Kernelobjekte
- Interruptprobleme
- Schedulerfehler
- Dateisystemfehler
- Sicherheitsverletzungen
- Mehrkern-Race-Conditions

Crash Dumps können sensible Speicherinhalte enthalten und müssen deshalb besonders geschützt werden.

---

## 2. Problemstellung

Während einer Kernel Panic können zentrale Subsysteme bereits beschädigt sein.

Nicht zuverlässig verfügbar sind möglicherweise:

- Heap
- Scheduler
- Dateisystem
- Gerätetreiber
- Netzwerk-Stack
- normale Sperren
- Userspace-Dienste
- vollständige Mehrkernkoordination

Der Dump-Pfad muss daher minimal, vorab vorbereitet und möglichst unabhängig vom gewöhnlichen Systemzustand sein.

---

## 3. Entscheidung

NovaOS implementiert ein mehrstufiges Crash-Dump-System mit einem panic-sicheren Kernelpfad.

Unterstützte Dump-Typen:

| Typ | Inhalt |
|---|---|
| Mini Dump | Panic-Kontext und wichtigste Kernelmetadaten |
| Kernel Dump | relevante Kernelbereiche und Kernelobjekte |
| Selective Dump | durch Richtlinie ausgewählte Speicherbereiche |
| Full Memory Dump | vollständiger physischer Speicher, nur ausdrücklich aktiviert |

Der Mini Dump ist im Produktivmodus der Standard.

Crash Dumps werden standardmäßig lokal, authentifiziert und verschlüsselt gespeichert.

---

## 4. Architektur

```text
Kernel Panic
     |
     v
Andere CPUs stoppen
     |
     v
Panic-Kontext erfassen
     |
     v
Dump-Typ und Ziel wählen
     |
     v
Daten blockweise schreiben
     |
     v
Dump abschließen
     |
     v
Neustart, Shutdown oder Recovery
```

Der Dump-Pfad verwendet keine gewöhnlichen VFS- oder Userspace-Abhängigkeiten.

---

## 5. Dump-Inhalte

Ein Mini Dump enthält mindestens:

- Panic-Code
- Panic-Nachricht in begrenzter Form
- Kernel-Build-ID
- Systemgeneration
- CPU-Architektur
- betroffene CPU
- Registerzustand
- aktueller Prozess und Thread
- geschützter Stacktrace
- letzte kritische Kernel-Logs
- ausgewählte Trace-Ereignisse
- geladene Kernelmodule
- Treiber- und Geräteübersicht
- Speicher- und Schedulerstatus
- Prüfsummen- und Dumpmetadaten

Vollständige Nutzdaten oder fremde Prozessspeicher werden im Mini Dump nicht standardmäßig aufgenommen.

---

## 6. Dump-Format

Crash Dumps verwenden ein versioniertes, blockorientiertes Format.

```c
typedef struct crash_dump_header {
    uint32_t magic;
    uint16_t format_version;
    uint16_t architecture;

    uint64_t dump_id;
    uint64_t timestamp_ns;
    uint64_t kernel_build_id;
    uint64_t system_generation;

    uint32_t dump_type;
    uint32_t panic_code;
    uint64_t total_size;
    uint32_t section_count;
    uint32_t flags;
} crash_dump_header_t;
```

Der Dump besteht aus typisierten Sektionen mit:

- Typ
- Version
- Länge
- Offset
- Kompressionsstatus
- Sensitivitätsklasse
- Integritätswert

Unbekannte Sektionen können anhand ihrer Länge übersprungen werden.

---

## 7. Dump-Sektionen

Vorgesehene Sektionstypen sind:

```text
SYSTEM_INFO
PANIC_CONTEXT
CPU_CONTEXT
STACK_TRACE
MODULE_LIST
PROCESS_SUMMARY
THREAD_SUMMARY
MEMORY_SUMMARY
DEVICE_SUMMARY
KERNEL_LOG
TRACE_SNAPSHOT
MEMORY_RANGES
SECURITY_STATE
```

Jede Sektion besitzt eigene Größenlimits.

Der Dump Writer darf nicht von einer vollständig intakten Quellstruktur ausgehen.

---

## 8. Panic-sicherer Dump Writer

Der Dump Writer wird vorab initialisiert und verwendet:

- statisch oder früh reservierten Speicher
- eigene kleine Stacks
- einfache blockierende Schreiboperationen
- vorab ausgewähltes Dump-Ziel
- keine gewöhnlichen Kernel-Heap-Allokationen
- keine komplexen Dateisystempfade
- keine nicht reentranten Sperren
- feste Timeouts

Ein Fehler im Dump Writer darf keine endlose Folge weiterer Panic-Zustände erzeugen.

---

## 9. Mehrkernverhalten

Bei einer Kernel Panic:

1. übernimmt eine CPU die Dump-Koordination,
2. werden andere CPUs per Inter-Processor Interrupt gestoppt,
3. sichern diese ihren minimalen CPU-Kontext,
4. wechseln sie in einen definierten Haltzustand,
5. schreibt die koordinierende CPU den Dump.

Reagiert eine CPU nicht, wird dies im Dump markiert.

Der Dump darf nicht unbegrenzt auf eine nicht reagierende CPU warten.

---

## 10. Dump-Ziele

Unterstützte Ziele können sein:

| Ziel | Verwendung |
|---|---|
| reservierte Dump-Partition | bevorzugter persistenter Pfad |
| reservierter NovaFS-Bereich | kontrollierte Integration |
| persistenter Firmwarebereich | sehr kleine Notfallinformationen |
| RAM-Puffer | Übergabe an Recovery nach Neustart |
| serielle Schnittstelle | Entwicklungsmodus |
| Netzwerkziel | nur kontrollierter Debugmodus |

Ein Netzwerkdump ist im Produktionsmodus standardmäßig deaktiviert.

Das Ziel wird möglichst vor der Panic vorbereitet und auf Funktionsfähigkeit geprüft.

---

## 11. Dump-Partition

Die bevorzugte erste Implementierung verwendet einen reservierten, blockorientierten Dump-Bereich.

Dieser besitzt:

- feste Lage
- definierte Maximalgröße
- minimale Metadatenstruktur
- atomaren Commit-Record
- begrenzte Anzahl von Dump-Slots
- Verschlüsselungs- und Integritätsinformationen
- Wiederherstellung durch das Recovery-System

Der Dump Writer benötigt dadurch keinen vollständigen Dateisystemtreiber.

---

## 12. Transaktionales Schreiben

Ein Crash Dump wird in folgenden Schritten geschrieben:

1. Zielslot als `INCOMPLETE` markieren.
2. Header und Sektionsverzeichnis vorbereiten.
3. Sektionen blockweise schreiben.
4. Prüfsummen oder Authentifizierungstags speichern.
5. verfügbaren Geräte-Flush ausführen.
6. abschließenden Commit-Record schreiben.
7. Slot als `COMPLETE` markieren.

Ein unvollständiger Dump bleibt als solcher erkennbar und wird nicht als vollständig analysierbar dargestellt.

Teilinformationen dürfen dennoch für Diagnosezwecke verwendet werden, wenn ihre Sektionen einzeln gültig sind.

---

## 13. Komprimierung

Crash-Dump-Sektionen dürfen blockweise komprimiert werden.

Anforderungen:

- kein großer dynamischer Speicherbedarf
- feste obere Laufzeitgrenzen
- Streaming-Verarbeitung
- unabhängige Blöcke
- sicherer Fallback auf unkomprimierte Speicherung

Komprimierung ist optional.

Bei einer zeitkritischen thermischen oder Hardware-Notlage darf NovaOS einen kleineren unkomprimierten Dump bevorzugen.

---

## 14. Verschlüsselung

Persistente Crash Dumps werden im Produktivmodus verschlüsselt.

Der Dump-Schlüssel wird:

- vorab sicher vorbereitet oder ableitbar gehalten,
- an den lokalen Systemkontext gebunden,
- nicht im Dump im Klartext gespeichert,
- nach der Nutzung aus temporären Bereichen entfernt,
- optional durch TPM oder ein anderes Trust Backend geschützt.

TPM-Unterstützung ist optional.

Ist kein sicherer Schlüssel verfügbar, wird im Produktivmodus nur ein stark reduzierter, nicht sensibler Notfalldatensatz gespeichert.

---

## 15. Integrität und Authentizität

Jede Dump-Sektion wird gegen Beschädigung und Manipulation geschützt.

Der Analyseprozess prüft:

- Header
- Formatversion
- Sektionsgrenzen
- Blockintegrität
- Authentifizierungstags
- Commit-Status
- Kernel-Build-ID
- Systemgeneration

Ein manipulierter Dump darf nicht als vertrauenswürdiger Systemzustand dargestellt werden.

Analysewerkzeuge behandeln alle Dumpdaten als nicht vertrauenswürdige Eingabe.

---

## 16. Kernel ASLR

Crash Dumps dürfen Kernel-ASLR nicht unnötig offenlegen.

Im Produktionsmodus werden Adressen nach Möglichkeit dargestellt als:

```text
Modul-ID + Offset
```

Rohe Kerneladressen werden:

- verschlüsselt gespeichert,
- nur autorisierten Analysewerkzeugen zugänglich gemacht,
- bei einem exportierten Standardbericht entfernt,
- nicht in gewöhnliche Logs übernommen.

Die Symbolauflösung erfolgt gegen die passende Kernel-Build-ID.

---

## 17. Speicherbereiche

Ein Kernel oder Full Dump verwendet eine vorab erstellte Liste zulässiger Speicherbereiche.

Ausgeschlossen werden standardmäßig:

- MMIO-Bereiche
- nicht lesbare Gerätebereiche
- Firmwarelaufzeitbereiche ohne Freigabe
- Schlüssel- und Secret-Seiten, sofern technisch trennbar
- nicht initialisierter Speicher
- freie Seiten ohne Diagnosewert
- Seiten anderer Sicherheitswelten

Fehlerhafte Speicherbereiche werden übersprungen und als unlesbar markiert.

---

## 18. Secret-Redaktion

NovaOS markiert besonders sensible Speicherseiten nach Möglichkeit mit einer Dump-Richtlinie.

Beispiele:

- private Kryptoschlüssel
- TLS- und VPN-Sitzungsschlüssel
- Passwortpuffer
- Authentifizierungstoken
- Key-Store-Speicher
- Hibernate-Schlüssel
- geschützte Debug-Secrets

Mögliche Richtlinien:

| Richtlinie | Verhalten |
|---|---|
| `DUMP_ALLOW` | Bereich darf aufgenommen werden |
| `DUMP_REDACT` | Inhalt wird durch Metadaten ersetzt |
| `DUMP_HASH_ONLY` | nur geschützter Hash wird gespeichert |
| `DUMP_DENY` | Bereich wird ausgeschlossen |

Eine Markierung ist zusätzliche Schadensbegrenzung, keine Garantie gegen alle Speicherduplikate eines Geheimnisses.

---

## 19. Dump-Größenrichtlinie

Die Dump-Auswahl berücksichtigt:

- verfügbare Zielgröße
- Panic-Ursache
- verfügbare Zeit
- Sicherheitsprofil
- Systemmodus
- thermischen Zustand
- Fehlerzustand des Speichergeräts

Priorität:

1. Panic-Kontext
2. CPU- und Stackinformationen
3. Kernel-Logs und Trace-Snapshot
4. Kernelobjekt-Zusammenfassungen
5. ausgewählte Speicherbereiche
6. vollständiger Speicher

Ein kleiner vollständiger Mini Dump ist wertvoller als ein unvollständiger Full Dump ohne Kontext.

---

## 20. Recovery-Integration

Beim nächsten Start erkennt das Recovery-System vorhandene Crash Dumps.

Es kann:

- Vollständigkeit prüfen
- Dump entschlüsseln
- Metadaten anzeigen
- lokalen Diagnosebericht erzeugen
- Dump in geschützten Speicher übernehmen
- beschädigte Slots markieren
- alte Dumps kontrolliert löschen
- Export nach Benutzerfreigabe vorbereiten

Der normale Systemstart darf durch einen vorhandenen Dump nicht unnötig blockiert werden.

---

## 21. Symbolauflösung

Stacktraces und Moduladressen werden über passende Symboldaten aufgelöst.

Erforderliche Zuordnung:

- Kernel-Build-ID
- Modul-Build-ID
- Architektur
- Symbolpaketversion
- Systemgeneration

Fehlen Symbole, bleibt die Darstellung als Modul-ID und Offset erhalten.

NovaOS darf nicht automatisch Symboldaten oder Dumps an externe Server senden.

---

## 22. Dump-Aufbewahrung

Crash Dumps unterliegen festen Grenzen für:

- Anzahl
- Gesamtgröße
- Aufbewahrungsdauer
- Sicherheitsklasse
- Benutzer- oder Administratorzugriff

Bei vollem Speicher kann NovaOS:

- den ältesten freigegebenen Dump überschreiben,
- nur einen Mini Dump erzeugen,
- den neuen Dump als nicht speicherbar melden.

Geschützte oder noch nicht analysierte Dumps werden nicht ohne definierte Richtlinie überschrieben.

---

## 23. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `DUMP_RIGHT_CREATE` | Crash Dump intern erzeugen |
| `DUMP_RIGHT_READ_SUMMARY` | nicht sensible Zusammenfassung lesen |
| `DUMP_RIGHT_READ_KERNEL` | Kernelbereiche analysieren |
| `DUMP_RIGHT_READ_MEMORY` | Speicherbereiche lesen |
| `DUMP_RIGHT_DECRYPT` | verschlüsselten Dump öffnen |
| `DUMP_RIGHT_EXPORT` | Dump ausdrücklich exportieren |
| `DUMP_RIGHT_DELETE` | Dump löschen |
| `DUMP_RIGHT_CONFIGURE` | Dump-Richtlinie ändern |
| `DUMP_RIGHT_ADMIN` | Dump-Subsystem verwalten |

Gewöhnliche Anwendungen erhalten keinen Zugriff auf Kernel- oder Full-Memory-Dumps.

---

## 24. Export

Ein Crash Dump bleibt standardmäßig lokal.

Ein Export benötigt:

- ausdrückliche Benutzer- oder Administratoraktion,
- Berechtigungsprüfung,
- Redaktionsprüfung,
- verschlüsselte Übertragung oder verschlüsseltes Archiv,
- sichtbare Zielangabe,
- Audit-Eintrag.

Automatisches Hochladen an Hersteller oder NovaOS-Dienste ist standardmäßig deaktiviert.

Ein Diagnosebericht ohne Speicherinhalte kann separat erzeugt werden.

---

## 25. Sicherheit und Datenschutz

Für Crash Dumps gelten folgende Regeln:

- Mini Dumps sind der Produktionsstandard.
- persistente Dumps werden verschlüsselt und authentifiziert,
- Secret-Seiten werden nach Möglichkeit ausgeschlossen,
- rohe Kerneladressen bleiben geschützt,
- Dumps werden nicht automatisch extern übertragen,
- Dump Reader benötigen Capabilities,
- Analysewerkzeuge behandeln Dumps als nicht vertrauenswürdige Dateien,
- Full Dumps benötigen eine ausdrückliche administrative Aktivierung,
- Dump-Ziele und Schlüssel werden vor einer Panic vorbereitet,
- unvollständige Dumps werden eindeutig gekennzeichnet.

---

## 26. Audit

Auditiert werden insbesondere:

- Aktivierung von Kernel- oder Full Dumps
- Lesen sensibler Dumps
- Entschlüsselung
- Export
- Löschung geschützter Dumps
- Änderung der Dump-Richtlinie
- fehlgeschlagene Authentifizierung
- Netzwerkdump im Debugmodus

Private Schlüssel, Speicherinhalte und vollständige Stackdaten werden nicht in den Audit-Datensatz kopiert.

---

## 27. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Dump wurde erfolgreich erstellt |
| `STATUS_DUMP_INCOMPLETE` | Dump wurde nur teilweise geschrieben |
| `STATUS_DUMP_TARGET_UNAVAILABLE` | kein Dump-Ziel verfügbar |
| `STATUS_DUMP_NO_SPACE` | Zielspeicher reicht nicht aus |
| `STATUS_DUMP_WRITE_FAILED` | Schreiben ist fehlgeschlagen |
| `STATUS_DUMP_AUTH_FAILED` | Integritäts- oder Authentifizierungsprüfung fehlgeschlagen |
| `STATUS_DUMP_KEY_UNAVAILABLE` | Verschlüsselungsschlüssel ist nicht verfügbar |
| `STATUS_DUMP_FORMAT_UNSUPPORTED` | Formatversion wird nicht unterstützt |
| `STATUS_DUMP_REDACTED` | angeforderter Bereich wurde redigiert |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_TIMEOUT` | Dump-Erstellung überschritt das Zeitlimit |

---

## 28. Alternative Ansätze

### 28.1 Immer vollständigen RAM speichern

**Vorteile:**

- maximale Diagnoseinformationen

**Nachteile:**

- sehr große Dumps
- lange Schreibzeit
- erhebliche Datenschutzrisiken

**Entscheidung:** Full Dumps nur nach ausdrücklicher Aktivierung.

### 28.2 Crash Dump über das normale VFS schreiben

**Vorteile:**

- einfache Dateiverwaltung

**Nachteile:**

- VFS oder Dateisystem kann Teil des Fehlers sein
- Deadlock- und Rekursionsrisiko

**Entscheidung:** Bevorzugt wird ein vorbereiteter blockorientierter Dump-Bereich.

### 28.3 Automatischer Cloud-Upload

**Vorteile:**

- schnelle zentrale Fehleranalyse

**Nachteile:**

- erhebliche Datenschutz- und Geheimnisrisiken
- Netzwerkabhängigkeit

**Entscheidung:** Abgelehnt. Export erfolgt nur ausdrücklich.

---

## 29. Konsequenzen

### Positive Konsequenzen

- Diagnose auch nach schweren Kernelabstürzen
- panic-sicherer, VFS-unabhängiger Schreibpfad
- abgestufte Dump-Größen
- verschlüsselte und authentifizierte Speicherung
- Schutz von Kernel-ASLR und Secrets
- Recovery-Integration
- kontrollierter lokaler Export

### Negative Konsequenzen

- reservierter Speicherbereich erforderlich
- Panic-Dump-Code muss besonders klein und robust bleiben
- Secret-Redaktion kann nicht alle Kopien sensibler Daten erkennen
- vollständige Dumps benötigen viel Zeit und Speicher

### Risiken

- Fehler im Dump Writer
- beschädigtes Speichergerät
- unvollständige Mehrkernzustände
- Schlüssel nicht verfügbar
- sensible Daten in nicht markierten Speicherbereichen
- manipulierte Dumps oder Analysewerkzeuge

---

## 30. Implementierungsrichtlinien

- Der Mini Dump ist der Standard im Produktivmodus.
- Der Dump Writer verwendet keine gewöhnlichen VFS- oder Heap-Abhängigkeiten.
- Dump-Ziel, Buffer und Schlüsselpfad werden vor einer Panic vorbereitet.
- Andere CPUs werden mit festen Timeouts gestoppt.
- Dumps werden sektioniert und blockweise geschrieben.
- Ein Commit-Record kennzeichnet vollständig geschriebene Dumps.
- Persistente Dumps werden verschlüsselt und authentifiziert.
- Kerneladressen werden als Modul-ID und Offset dargestellt.
- sensible Seiten werden nach Möglichkeit redigiert.
- Netzwerkexport ist standardmäßig deaktiviert.
- Dumpfehler dürfen keine endlose Panic-Rekursion erzeugen.

---

## 31. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Mini-, Kernel-, Selective- und Full Dumps
- Panic auf jeder CPU
- nicht reagierende Sekundär-CPU
- Panic während Interrupt oder Exception
- beschädigten Heap und Scheduler
- nicht verfügbares VFS
- volles oder defektes Dump-Ziel
- Stromausfall während des Schreibens
- Verschlüsselung und Authentifizierung
- Secret-Redaktion
- ASLR-geschützte Stacktraces
- unvollständige Dumps
- Recovery-Import
- Capability- und Exportrechte
- Fehler im Dump Writer

---

## 32. Verbindliche Invarianten

1. Crash-Dump-Erstellung darf nicht vom normalen Userspace abhängen.
2. Der Dump Writer verwendet keinen gewöhnlichen Kernel-Heap oder VFS-Pfad.
3. Mini Dumps sind der Standard im Produktivmodus.
4. Persistente Dumps werden authentifiziert und grundsätzlich verschlüsselt.
5. Unvollständige Dumps werden niemals als vollständig gekennzeichnet.
6. Rohe Kerneladressen bleiben im Produktionsmodus geschützt.
7. Sensible Speicherseiten werden nach Möglichkeit redigiert oder ausgeschlossen.
8. Full Memory Dumps benötigen eine ausdrückliche administrative Aktivierung.
9. Crash Dumps werden nicht automatisch extern übertragen.
10. Dumpanalysewerkzeuge behandeln jeden Dump als nicht vertrauenswürdige Eingabe.

---

## 33. Zusammenfassung

NovaOS implementiert ein mehrstufiges Crash-Dump-System mit einem minimalen, panic-sicheren und VFS-unabhängigen Schreibpfad.

Der standardmäßige Mini Dump enthält den Panic-Kontext, CPU-Zustände, geschützte Stacktraces sowie letzte Logs und Trace-Ereignisse. Umfangreichere Kernel- oder Full Dumps müssen ausdrücklich aktiviert werden.

Transaktionale Speicherung, Verschlüsselung, Authentifizierung, Secret-Redaktion und kontrollierter Export ermöglichen eine aussagekräftige Fehleranalyse, ohne Kerneladressen, Schlüssel oder Prozessspeicher unnötig offenzulegen.