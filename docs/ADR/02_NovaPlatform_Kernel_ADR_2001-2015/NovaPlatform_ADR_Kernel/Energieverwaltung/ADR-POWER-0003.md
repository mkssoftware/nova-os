# ADR-POWER-0003 – Hibernate

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-POWER-0003 |
| Titel | Hibernate |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Systemenergieverwaltung und Sitzungswiederherstellung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-POWER-0002, ADR-DEV-0005, ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0006, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-POWER-HIBERNATE |

---

## 1. Kontext

Hibernate speichert den laufenden Systemzustand persistent und schaltet den Rechner anschließend vollständig aus.

Im Gegensatz zu Suspend benötigt Hibernate keine dauerhafte Stromversorgung des Arbeitsspeichers.

Nach dem Einschalten soll NovaOS die vorherige Sitzung wiederherstellen können, einschließlich:

- Kernelzustand
- Prozesse und Threads
- virtuelle Adressräume
- Anwendungen
- ausgewählte Geräte- und Treiberzustände
- offene Systemdienste
- Mount- und Netzwerkzustände

Das Hibernate-Abbild enthält große Mengen sensibler Daten und muss deshalb besonders geschützt werden.

---

## 2. Problemstellung

Ein Speicherabbild kann enthalten:

- Passwörter und Schlüsselmaterial
- private Dokumente
- Prozessspeicher
- Netzwerk- und Sitzungstoken
- Kernelstrukturen
- Dateisystemcaches
- Sicherheitskontexte
- ASLR-Zustände

Ein manipuliertes oder veraltetes Hibernate-Abbild könnte außerdem zur Codeausführung oder zum Zurücksetzen eines früheren Sicherheitszustands verwendet werden.

NovaOS muss daher Vertraulichkeit, Integrität, Aktualität und atomare Wiederherstellung sicherstellen.

---

## 3. Entscheidung

NovaOS implementiert Hibernate als verifizierte, verschlüsselte und transaktional erzeugte Systemabbildfunktion.

Das Hibernate-Abbild wird:

- vollständig authentifiziert
- standardmäßig verschlüsselt
- an die laufende Systemgeneration gebunden
- mit einer eindeutigen Resume-Generation versehen
- erst nach vollständigem Schreiben als gültig markiert
- nach erfolgreichem Resume verbraucht oder ungültig markiert

Ein TPM kann die Schlüssel- und Rollback-Sicherheit verbessern, ist aber keine zwingende Voraussetzung.

---

## 4. Architektur

```text
Laufendes System
      |
      v
Prozesse und Geräte einfrieren
      |
      v
Speicherabbild erzeugen
      |
      v
Komprimieren, verschlüsseln, authentifizieren
      |
      v
Hibernate-Speicher
      |
      v
System ausschalten
```

Beim nächsten Start:

```text
Bootloader
    |
    v
Hibernate-Abbild erkennen
    |
    v
Resume Loader verifizieren
    |
    v
Abbild authentifizieren und entschlüsseln
    |
    v
Kernel- und Speicherzustand wiederherstellen
    |
    v
Geräte und Prozesse fortsetzen
```

---

## 5. Hibernate-Modi

NovaOS unterstützt mindestens:

| Modus | Bedeutung |
|---|---|
| `HIBERNATE` | Zustand speichern und ausschalten |
| `HYBRID_SLEEP` | Zustand speichern und anschließend Suspend versuchen |
| `HIBERNATE_THEN_SUSPEND` | zunächst Suspend, später zeitgesteuert Hibernate |
| `HIBERNATE_AUTO` | Power Manager wählt eine geeignete Strategie |

Hybridmodi verwenden dieselben Sicherheits- und Integritätsanforderungen wie ein gewöhnliches Hibernate-Abbild.

---

## 6. Hibernate-Anforderung

```c
status_t power_hibernate(
    uint32_t mode,
    uint32_t flags,
    uint64_t timeout_ns
);
```

Der Aufrufer benötigt eine Hibernate-Capability.

Vor Beginn prüft NovaOS:

- verfügbaren Hibernate-Speicher
- aktiven Systemzustand
- Blocker
- Dateisystemintegrität
- Schlüsselverfügbarkeit
- Systemgeneration
- inkompatible Geräte oder Treiber
- aktive Updates

Ein laufendes Systemupdate oder eine kritische Dateisystemreparatur darf Hibernate blockieren.

---

## 7. Zustandsmaschine

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
SNAPSHOTTING
   |
   v
WRITING_IMAGE
   |
   v
COMMITTING
   |
   v
POWERING_OFF
```

Beim Start:

```text
IMAGE_DETECTED
      |
      v
VERIFYING
      |
      v
RESTORING
      |
      v
RESUMING_DEVICES
      |
      v
RESUMING_PROCESSES
      |
      v
ACTIVE
```

Fehler vor dem Ausschalten führen zu einem Rollback in den aktiven Zustand.

---

## 8. Vorbereitungsphase

Vor dem Snapshot:

1. Hibernate-Anforderung autorisieren.
2. Hibernate-Speicher reservieren.
3. aktiven Schlüsselkontext vorbereiten.
4. Anwendungen und Dienste informieren.
5. nicht wiederherstellbare Operationen erkennen.
6. kritische Dateisystemtransaktionen abschließen.
7. neue Systemänderungen begrenzen.
8. globale Hibernate-Generation erzeugen.

Die Vorbereitung besitzt feste Timeouts und kann bis zum Commit abgebrochen werden.

---

## 9. Einfrieren von Prozessen

Der Process Manager stoppt Prozesse an sicheren Punkten.

Dabei werden:

- neue Prozesserzeugungen blockiert,
- Userspace-Threads eingefroren,
- unterbrechbare Systemaufrufe abgeschlossen oder abgebrochen,
- asynchrone Operationen stabilisiert,
- Sicherheitskontexte und Capability-Spaces erfasst.

Kernel-Worker werden nur soweit eingefroren, wie es der Snapshot-Ablauf erlaubt.

Ein Prozess darf das Einfrieren nicht unbegrenzt verhindern.

---

## 10. Dateisysteme

Vor dem Speicherabbild:

- werden kritische Journaling-Transaktionen committed,
- wird die Hibernate-Datei oder Partition vorbereitet,
- werden notwendige Metadaten dauerhaft geschrieben,
- werden Dateisystemgenerationen erfasst,
- wird rekursiver Cachezugriff auf das Hibernate-Ziel vermieden.

Die Speicherung des Hibernate-Abbilds darf nicht von Speicherbereichen abhängen, die während des Schreibens selbst verändert oder ausgelagert werden müssen.

Das Ziel benötigt einen dedizierten, früh bootfähig lesbaren Zugriffspfad.

---

## 11. Geräte und Treiber

Treiber müssen angeben, ob ihr Gerät:

- vollständig wiederherstellbar ist,
- nach Resume neu initialisiert werden muss,
- keinen persistenten Zustand benötigt,
- Hibernate nicht unterstützt.

Vor dem Snapshot werden:

- I/O-Warteschlangen gestoppt,
- DMA-Operationen beendet,
- Bus Mastering kontrolliert,
- Geräte in einen sicheren Zustand versetzt,
- notwendige logische Zustände gespeichert.

Hardwarezustände werden nach dem Einschalten grundsätzlich neu validiert.

---

## 12. Auswahl der Speicherseiten

Das Abbild enthält nur Seiten, die für die Wiederherstellung erforderlich sind.

Ausgeschlossen oder speziell behandelt werden können:

- freie Seiten
- rekonstruierbare Clean File Cache Pages
- verworfene temporäre Buffer
- Gerätespeicher
- MMIO-Bereiche
- DMA-Seiten mit unsicherem Zustand
- Firmwarebereiche
- explizit als nicht hibernierbar markierte Seiten

Nicht gespeicherte rekonstruierbare Seiten werden nach Resume erneut geladen.

---

## 13. Abbildformat

Das Abbild verwendet ein versioniertes, blockorientiertes Format.

Es enthält mindestens:

```c
typedef struct hibernate_image_header {
    uint32_t magic;
    uint16_t format_version;
    uint16_t architecture;
    uint64_t image_generation;
    uint64_t system_generation;
    uint64_t kernel_build_id;
    uint64_t page_count;
    uint64_t image_size;
    uint32_t flags;
    uint32_t header_size;
} hibernate_image_header_t;
```

Zusätzlich enthält es:

- Seitenindex
- Speicherlayout
- CPU-Zustände
- Kernel-Resume-Metadaten
- Prüfsummen oder Authentifizierungstags
- Kompressionsinformationen
- verschlüsselte Nutzdaten
- abschließenden Commit-Datensatz

---

## 14. Transaktionales Schreiben

Ein Hibernate-Abbild wird niemals direkt als gültiges Image überschrieben.

Der Ablauf ist:

1. Zielbereich als `INCOMPLETE` markieren.
2. Header und Metadaten schreiben.
3. Speicherseiten blockweise schreiben.
4. Authentifizierungstags speichern.
5. Speichergerät flushen.
6. vollständige Integrität verifizieren.
7. Commit-Datensatz atomar schreiben.
8. erneut flushen.
9. Image als `VALID` markieren.
10. System ausschalten.

Ein Stromausfall während des Schreibens hinterlässt kein gültig erscheinendes Teilabbild.

---

## 15. Komprimierung

Speicherseiten dürfen vor der Verschlüsselung komprimiert werden.

Ziele:

- geringere Schreibmenge
- kürzere Hibernate-Zeit
- kleinerer Speicherbedarf
- schnellere Wiederherstellung auf langsamen Datenträgern

Die Kompression:

- erfolgt blockweise,
- besitzt feste Speicherlimits,
- muss Streaming unterstützen,
- darf keine unkontrollierten Wörterbücher verwenden,
- wird vor der Verschlüsselung angewendet.

Ein nicht komprimierbarer Block wird unverändert gespeichert.

---

## 16. Verschlüsselung und Integrität

Das gesamte sensible Abbild wird mit authentifizierter Verschlüsselung geschützt.

Nicht verschlüsselte Boot-Metadaten werden auf das notwendige Minimum reduziert und vollständig authentifiziert.

Der Schlüssel wird abgeleitet aus:

- einem zufälligen Image-Schlüssel,
- dem geschützten NovaOS Key Store,
- optional einem Benutzergeheimnis,
- optional einem TPM- oder Trust-Backend,
- der Hibernate- und Systemgeneration.

Ein Integritätsfehler führt immer zur Verwerfung des Abbilds.

---

## 17. Systeme ohne TPM

Hibernate muss auch ohne TPM funktionieren.

Mögliche Schlüsselstrategien sind:

- geschützter Systemschlüssel
- Benutzerentsperrung beim Start
- Schlüsselableitung aus verschlüsseltem Systemvolume
- Recovery- oder Administratorschlüssel

Ohne Hardware-Trust-Backend kann NovaOS physische Manipulation und Rollback nicht im gleichen Umfang erkennen.

Dieser reduzierte Vertrauensstatus muss ehrlich angezeigt werden.

Unverschlüsseltes Hibernate ist im normalen Produktivmodus nicht zulässig.

---

## 18. Bindung an System und Kernel

Das Abbild wird mindestens gebunden an:

- CPU-Architektur
- Kernel-Build-ID
- Systemgeneration
- Hibernate-Formatversion
- Sicherheitsrichtlinienversion
- Resume-Loader-Version
- Speicherlayoutanforderungen

Ein Abbild eines inkompatiblen Kernels oder einer anderen Systemgeneration wird nicht geladen.

Systemupdates müssen ein vorhandenes Hibernate-Abbild entweder erhalten können oder es vor der Aktivierung der neuen Generation sicher invalidieren.

---

## 19. Kernel ASLR

Das Hibernate-Abbild enthält bereits einen laufenden Kerneladressraum.

Beim Resume wird deshalb der ursprüngliche Kernel-ASLR-Zustand derselben Sitzung wiederhergestellt.

Das Abbild darf nicht verwendet werden, um diesen Zustand unautorisiert auszulesen oder zu verändern.

Nach einem normalen Neustart wird eine neue ASLR-Randomisierung erzeugt.

Ein Hibernate-Resume ist eine Fortsetzung derselben Sitzung und kein neuer Sicherheitsstart.

---

## 20. Resume Loader

Der Resume Loader ist ein minimaler, verifizierter Bestandteil des sicheren Startpfads.

Er muss:

1. Image-Header lesen,
2. Version und Plattform prüfen,
3. gültigen Commit-Datensatz finden,
4. Schlüsselzugriff herstellen,
5. Authentizität prüfen,
6. Speicherzielbereiche vorbereiten,
7. Seiten entschlüsseln und dekomprimieren,
8. Kernel-Resume-Einstiegspunkt aktivieren.

Er enthält keine vollständige Dateisystem- oder Anwendungslaufzeit.

Wenn möglich, verwendet er dieselbe verifizierte Boot- und Speicherinfrastruktur wie das Recovery-System.

---

## 21. Wiederherstellung des Speichers

Der Resume Loader darf sich nicht selbst überschreiben, während er das alte Speicherlayout rekonstruiert.

Dafür verwendet NovaOS:

- reservierte Resume-Seiten
- einen temporären Page Table Context
- kontrollierte Wiederherstellungsreihenfolge
- späteres Kopieren konfliktbehafteter Seiten
- abschließenden Wechsel in den restaurierten Kerneladressraum

Vor der Übergabe an den Kernel wird die Integrität aller erforderlichen Blöcke bestätigt.

---

## 22. Resume von CPUs und Geräten

Nach Wiederherstellung des Kernels:

1. startet die Boot-CPU im Resume-Pfad,
2. werden grundlegende Plattformkomponenten initialisiert,
3. werden sekundäre CPUs neu gestartet,
4. werden Parent-Geräte vor Child-Geräten reaktiviert,
5. werden Interrupts und DMA-Kontexte neu aufgebaut,
6. werden Geräteidentitäten erneut geprüft,
7. werden nicht mehr vorhandene Geräte entfernt,
8. werden Prozesse fortgesetzt.

Hardware wird nicht allein aufgrund gespeicherter Zustände als unverändert vorausgesetzt.

---

## 23. Netzwerkzustand

Netzwerkhardware, Adressen und Routen können sich während der Ausschaltzeit geändert haben.

Nach Resume werden daher:

- Interfaces neu erkannt,
- Linkzustände geprüft,
- DHCP- und IPv6-Laufzeiten bewertet,
- Neighbor Caches invalidiert,
- VPN-Verbindungen neu aufgebaut,
- TCP- und andere Verbindungen gegebenenfalls als unterbrochen gemeldet,
- Firewall- und QoS-Richtlinien erneut angewendet.

Hibernate garantiert nicht, dass entfernte Netzwerkgegenstellen bestehende Verbindungen erhalten haben.

---

## 24. Image-Verbrauch und Replay-Schutz

Nach erfolgreichem Resume wird das Image als verbraucht markiert.

Ein erneutes Laden desselben Images ist standardmäßig verboten.

Schutzmechanismen:

- monotone Resume-Generation
- Verbrauchsmarker
- atomare Image-Invalidierung
- optionaler TPM-Zähler
- Bindung an Dateisystem- und Systemgeneration
- Erkennung bereits fortgesetzter Sitzung

Ohne TPM bleibt Schutz gegen einen Angreifer mit vollständiger physischer Datenträgerkontrolle eingeschränkt.

---

## 25. Abbruch und Fallback

Kann ein Image nicht sicher geladen werden, startet NovaOS regulär oder wechselt in Recovery.

Gründe sind beispielsweise:

- ungültige Authentifizierung
- inkompatibler Kernel
- beschädigte Blöcke
- fehlender Schlüssel
- falsche Systemgeneration
- bereits verbrauchtes Image
- nicht unterstützte Plattform

NovaOS versucht niemals, nur unbeschädigte Teile einer Sitzung fortzusetzen.

Ein Hibernate-Abbild wird vollständig akzeptiert oder vollständig verworfen.

---

## 26. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `HIBERNATE_RIGHT_REQUEST` | Hibernate anfordern |
| `HIBERNATE_RIGHT_BLOCK` | begrenzten Blocker erstellen |
| `HIBERNATE_RIGHT_IMAGE` | Image-Ziel verwalten |
| `HIBERNATE_RIGHT_RESUME` | Resume autorisieren |
| `HIBERNATE_RIGHT_INVALIDATE` | Image sicher ungültig machen |
| `HIBERNATE_RIGHT_INSPECT` | nicht sensible Statusdaten lesen |
| `HIBERNATE_RIGHT_POLICY` | Hibernate-Richtlinien ändern |
| `HIBERNATE_RIGHT_ADMIN` | Hibernate-Subsystem verwalten |

Anwendungen erhalten keinen direkten Zugriff auf das Image oder dessen Schlüsselmaterial.

---

## 27. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Image-Größe
- Kompressionsspeicher
- parallele Schreiboperationen
- Dauer der Freeze-Phase
- Anzahl der Resume-Versuche
- Diagnosemetadaten
- reservierter Resume-Speicher
- Anzahl gespeicherter Images

NovaOS unterstützt standardmäßig höchstens ein gültiges Hibernate-Abbild pro fortsetzbarer Systeminstanz.

---

## 28. Sicherheit und Datenschutz

Für Hibernate gelten folgende Regeln:

- das Image ist standardmäßig verschlüsselt und authentifiziert,
- ein Integritätsfehler führt zum vollständigen Verwerfen,
- langfristige Schlüssel werden nicht direkt im Image gespeichert,
- das Image ist an Kernel und Systemgeneration gebunden,
- ein erfolgreich verwendetes Image wird ungültig gemacht,
- private Schlüssel und Klartextseiten erscheinen nicht in Logs,
- Resume Loader und Bootpfad werden verifiziert,
- unverschlüsseltes Hibernate ist im Produktivmodus verboten,
- fehlender TPM-Schutz wird als reduzierter Vertrauensstatus angezeigt.

---

## 29. Audit und Diagnose

Auditiert werden insbesondere:

- Hibernate-Anforderung und Ergebnis
- fehlgeschlagene Image-Authentifizierung
- inkompatible oder alte Images
- fehlende Schlüssel
- erzwungene Image-Invalidierung
- wiederholte Resume-Versuche
- Abweichungen der Systemgeneration
- Wechsel in Recovery

Diagnoseinformationen dürfen enthalten:

- Image-Version
- Größe
- Kompressionsrate
- Schreib- und Resume-Dauer
- Fehlerphase
- Systemgeneration
- verwendetes Trust Backend

Speicherinhalte, Schlüssel und Authentifizierungstags werden nicht offen ausgegeben.

---

## 30. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Hibernate oder Resume erfolgreich |
| `STATUS_HIBERNATE_BLOCKED` | ein Blocker verhindert Hibernate |
| `STATUS_HIBERNATE_NO_SPACE` | Ziel besitzt nicht genug Speicher |
| `STATUS_HIBERNATE_IMAGE_INVALID` | Image ist strukturell ungültig |
| `STATUS_HIBERNATE_AUTH_FAILED` | Authentifizierung ist fehlgeschlagen |
| `STATUS_HIBERNATE_KEY_UNAVAILABLE` | Schlüssel ist nicht verfügbar |
| `STATUS_HIBERNATE_INCOMPATIBLE` | Kernel oder Systemgeneration passt nicht |
| `STATUS_HIBERNATE_IMAGE_USED` | Image wurde bereits fortgesetzt |
| `STATUS_HIBERNATE_WRITE_FAILED` | Schreiben des Images ist fehlgeschlagen |
| `STATUS_HIBERNATE_RESUME_FAILED` | Wiederherstellung ist fehlgeschlagen |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |

---

## 31. Alternative Ansätze

### 31.1 Unverschlüsseltes Hibernate-Abbild

**Vorteile:**

- einfache und schnelle Implementierung

**Nachteile:**

- vollständiger Prozess- und Schlüsselspeicher liegt offen auf dem Datenträger

**Entscheidung:** Im Produktivmodus abgelehnt.

### 31.2 TPM als zwingende Voraussetzung

**Vorteile:**

- starke Schlüsselbindung und besserer Rollback-Schutz

**Nachteile:**

- ältere Hardware wäre ausgeschlossen
- widerspricht der NovaOS-Hardwarestrategie

**Entscheidung:** TPM bleibt optionales Trust Backend.

### 31.3 Anwendungssitzungen statt RAM-Abbild speichern

**Vorteile:**

- kleinere und portablere Datenmenge
- geringere Kernelkomplexität

**Nachteile:**

- keine vollständige Sitzungsfortsetzung
- Anwendungen benötigen eigene Wiederherstellungslogik

**Entscheidung:** Kann Hibernate ergänzen, ersetzt es aber nicht.

---

## 32. Konsequenzen

### Positive Konsequenzen

- vollständige Sitzungsfortsetzung ohne dauerhafte RAM-Versorgung
- verschlüsseltes und authentifiziertes Abbild
- Unterstützung mit und ohne TPM
- transaktionales Schreiben
- Schutz vor Teilabbildern
- Bindung an Kernel und Systemgeneration
- sicherer Fallback auf normalen Start oder Recovery

### Negative Konsequenzen

- hoher Implementierungsaufwand
- zusätzlicher Speicherplatz erforderlich
- Hibernate und Resume können auf langsamer Hardware lange dauern
- Gerätetreiber benötigen verlässliche Wiederherstellungspfade
- ohne TPM bleibt physischer Rollback-Schutz begrenzt

### Risiken

- Schlüsselverlust
- Speicherabbildmanipulation
- Replay eines alten Images
- Geräteabweichungen nach Resume
- unvollständiges Image bei Stromausfall
- Sicherheitslücken im Resume Loader
- inkompatible Systemupdates

---

## 33. Implementierungsrichtlinien

- Hibernate wird als transaktionale Systemoperation umgesetzt.
- Das Image wird blockweise komprimiert, verschlüsselt und authentifiziert.
- Ein Image wird erst nach vollständigem Flush als gültig markiert.
- Der Resume Loader bleibt minimal und verifiziert.
- Das Image ist an Architektur, Kernel und Systemgeneration gebunden.
- Hardwarezustände werden nach Resume neu validiert.
- Alte oder teilweise beschädigte Images werden niemals teilweise geladen.
- Nach erfolgreichem Resume wird das Image ungültig gemacht.
- TPM-Unterstützung bleibt optional.
- Unverschlüsseltes Hibernate ist im Produktivmodus verboten.

---

## 34. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- vollständiges Hibernate und Resume
- Stromausfall in jeder Schreibphase
- Komprimierung
- Verschlüsselung und Authentifizierung
- falsche oder fehlende Schlüssel
- manipulierte Image-Blöcke
- inkompatible Kernel- und Systemgenerationen
- Image-Replay
- Systeme mit und ohne TPM
- Geräteänderungen während der Ausschaltzeit
- Netzwerk- und VPN-Wiederherstellung
- Dateisystem- und Writeback-Fehler
- Mehrkernsysteme
- große und fast vollständig belegte RAM-Konfigurationen
- Recovery-Fallback

---

## 35. Verbindliche Invarianten

1. Hibernate speichert eine Fortsetzung derselben Systemsitzung.
2. Das Hibernate-Abbild ist im Produktivmodus immer verschlüsselt.
3. Alle Image-Bestandteile sind authentifiziert.
4. Teilweise geschriebene Images dürfen niemals als gültig erscheinen.
5. Das Image ist an Kernel, Architektur und Systemgeneration gebunden.
6. Ein Integritätsfehler verwirft das vollständige Image.
7. Hardwarezustände werden nach Resume erneut validiert.
8. Ein erfolgreich verwendetes Image darf nicht erneut fortgesetzt werden.
9. TPM-Unterstützung ist optional und ihr Fehlen wird ehrlich ausgewiesen.
10. Speicherinhalte und Schlüssel werden nicht in gewöhnlichen Logs offengelegt.

---

## 36. Zusammenfassung

NovaOS implementiert Hibernate als verschlüsselte, authentifizierte und transaktional erzeugte Systemsitzung.

Prozesse, Kernelzustand und erforderliche Speicherseiten werden eingefroren, blockweise komprimiert und sicher auf einem persistenten Medium gespeichert. Ein minimaler, verifizierter Resume Loader stellt das Abbild beim nächsten Start vollständig wieder her.

Beschädigte, manipulierte, inkompatible oder bereits verwendete Abbilder werden vollständig verworfen. TPM-Unterstützung verbessert Schlüsselbindung und Replay-Schutz, bleibt aber optional, damit Hibernate auch auf älterer Hardware verfügbar ist.