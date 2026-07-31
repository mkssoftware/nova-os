# ADR-SEC-0008 – Audit

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0008 |
| Titel | Audit |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Telemetrie / Nachvollziehbarkeit |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-KERNEL-0103, ADR-KERNEL-0104, ADR-KERNEL-0105, ADR-PROC-0001, ADR-PROC-0009 |
| Nachfolgende Spezifikation | NPSPEC-AUDIT-0001 |

---

## 1. Kontext

NovaOS benötigt eine zuverlässige Möglichkeit, sicherheitsrelevante Vorgänge nachvollziehbar zu erfassen.

Dazu gehören beispielsweise:

- erfolgreiche und fehlgeschlagene Anmeldungen,
- Capability-Erteilungen und -Entziehungen,
- Prozess- und Threadstarts,
- Änderungen an Sicherheitsrichtlinien,
- Laden von Kernelmodulen,
- Zugriffe auf geschützte Ressourcen,
- Änderungen an Systemgenerationen,
- Recovery-Vorgänge,
- Debugzugriffe,
- Integritätsverletzungen,
- Kernel- und Boot-Sicherheitsereignisse.

Normale Logs reichen hierfür nicht aus.

Ein gewöhnliches Log kann:

- optional sein,
- bei hoher Last Einträge verlieren,
- von einer Anwendung verändert werden,
- keine stabile Ereignisidentität besitzen,
- unvollständige Sicherheitskontexte enthalten,
- Geheimnisse oder personenbezogene Daten unkontrolliert speichern,
- keine Manipulationserkennung besitzen.

Audit muss deshalb als eigener Sicherheitsdienst mit verbindlichem Ereignismodell, Zugriffskontrolle, Integritätsschutz und klaren Datenschutzregeln entworfen werden.

---

## 2. Problemstellung

Das Audit-System muss mehrere widersprüchliche Anforderungen erfüllen:

- sicherheitsrelevante Ereignisse dürfen nicht unbemerkt verloren gehen,
- der Kernel darf nicht durch Audit-I/O blockiert oder zum Absturz gebracht werden,
- ein Angreifer darf Auditdaten nicht unbemerkt verändern,
- Auditdaten dürfen keine unnötigen personenbezogenen Daten enthalten,
- Geheimnisse, Passwörter, Schlüssel und vollständige Speicherinhalte dürfen nicht protokolliert werden,
- Ereignisse müssen auch bei hoher Parallelität eine nachvollziehbare Reihenfolge besitzen,
- frühe Bootereignisse müssen übernommen werden,
- Audit muss lokal funktionieren,
- externe Übertragung darf nicht vorausgesetzt werden,
- Speicher- und Aufbewahrungsgrenzen müssen durchgesetzt werden,
- Ausfall und Überlastung müssen selbst auditierbar sein.

---

## 3. Entscheidungsziele

Das Audit-System soll:

1. sicherheitsrelevante Ereignisse strukturiert erfassen,
2. Kernel- und Userspace-Ereignisse eindeutig unterscheiden,
3. manipulationserschwerende Ereignisketten verwenden,
4. lokale Speicherung als Standard festlegen,
5. definierte Überlastungs- und Ausfallstrategien besitzen,
6. vertrauliche Inhalte standardmäßig ausschließen,
7. DSGVO-konforme Datenminimierung ermöglichen,
8. Zugriffe durch Capabilities schützen,
9. Boot-, Kernel- und Recovery-Ereignisse zusammenführen,
10. hohe Mehrkernlast ohne globale Hotpath-Sperre verarbeiten,
11. stabile, versionierte Ereignisformate bereitstellen,
12. autorisierte Offlineanalyse und Export ermöglichen.

---

## 4. Begriffsdefinitionen

### 4.1 Audit Event

Ein Audit Event ist ein strukturiertes, sicherheitsrelevantes Ereignis mit definierter Identität, Ursache, Ergebnis und Sicherheitskontext.

---

### 4.2 Audit Record

Ein Audit Record ist die serialisierte Darstellung eines Audit Events.

---

### 4.3 Audit Trail

Der Audit Trail ist die geordnete Folge gespeicherter Audit Records.

---

### 4.4 Audit Source

Eine Audit Source ist die Kernel- oder Userspace-Komponente, die ein Ereignis erzeugt.

---

### 4.5 Audit Subject

Das Audit Subject ist das handelnde Sicherheitsobjekt.

Beispiele:

- Prozess,
- Thread,
- Systemdienst,
- Benutzeridentität,
- Kernelkomponente,
- Remote Principal.

---

### 4.6 Audit Object

Das Audit Object ist die Ressource, auf die sich die Aktion bezieht.

Beispiele:

- Datei,
- Prozess,
- Capability,
- Gerät,
- Kernelmodul,
- Job Object,
- Sicherheitsrichtlinie.

---

### 4.7 Audit Sink

Ein Audit Sink ist ein autorisiertes Ziel für Audit Records.

Beispiele:

- lokaler Auditdienst,
- geschützte Datei,
- unveränderliches Subvolume,
- lokaler Ringpuffer,
- kontrollierter Exportkanal.

---

### 4.8 Audit Policy

Die Audit Policy legt fest, welche Ereignisse mit welcher Priorität, Detailstufe, Aufbewahrung und Überlastungsreaktion erfasst werden.

---

## 5. Abgrenzung zu Telemetrie und Logging

Audit, Telemetrie und Logging erfüllen unterschiedliche Zwecke.

| Eigenschaft | Audit | Telemetrie | Allgemeines Logging |
|---|---|---|---|
| Hauptzweck | Sicherheitsnachweis | Systemzustand und Leistung | Diagnose und Entwicklung |
| Verbindliches Schema | Ja | Ja | optional |
| Manipulationsschutz | Ja | abhängig von Policy | normalerweise nein |
| Datenminimierung | zwingend | zwingend | empfohlen |
| Verlustbehandlung | explizit | meist tolerierbar | oft Best Effort |
| Capability-Schutz | Ja | Ja | abhängig vom Subsystem |
| rechtliche Relevanz | möglich | begrenzt | abhängig vom Inhalt |
| externe Übertragung | standardmäßig nein | standardmäßig nein | standardmäßig nein |

Audit verwendet die lokale Telemetrieinfrastruktur aus ADR-KERNEL-0104, bleibt aber ein eigenständiger Sicherheitsdienst.

---

## 6. Betrachtete Alternativen

### 6.1 Nur allgemeine Textlogs

Sicherheitsereignisse werden als freie Textzeilen protokolliert.

#### Vorteile

- einfache Implementierung,
- leicht lesbar,
- geringe Schemaanforderungen.

#### Nachteile

- schwer maschinell auswertbar,
- uneinheitliche Felder,
- anfällig für Log Injection,
- keine stabile Versionierung,
- schwierige Manipulationserkennung.

Diese Alternative wird verworfen.

---

### 6.2 Vollständige synchrone Speicherung jedes Ereignisses

Jedes Ereignis wird vor Abschluss der verursachenden Operation dauerhaft gespeichert.

#### Vorteile

- geringe Verlustwahrscheinlichkeit,
- klare Reihenfolge,
- starke Nachweisbarkeit.

#### Nachteile

- hohe Latenz,
- Audit-I/O kann Kerneloperationen blockieren,
- ungeeignet für Interrupt- und Echtzeitpfade,
- Auditmedium kann zum Denial-of-Service-Ziel werden.

Diese Alternative wird als allgemeines Modell verworfen.

---

### 6.3 Ausschließlich flüchtige Ringpuffer

Auditdaten werden nur im Arbeitsspeicher gehalten.

#### Vorteile

- hohe Geschwindigkeit,
- geringe I/O-Kosten,
- einfache Kernelintegration.

#### Nachteile

- Verlust bei Neustart oder Stromausfall,
- keine langfristige Nachvollziehbarkeit,
- unzureichend für kritische Sicherheitsereignisse.

Diese Alternative wird allein verworfen.

---

### 6.4 Mehrstufige lokale Audit-Pipeline

Ereignisse werden zunächst in begrenzten CPU-lokalen Kernelpuffern erfasst und anschließend durch einen privilegierten Auditdienst in einen manipulationserschwerenden lokalen Audit Trail übernommen.

#### Vorteile

- kleiner Kernel-Hotpath,
- hohe Mehrkernskalierbarkeit,
- kontrollierte persistente Speicherung,
- definierte Überlastungsstrategien,
- gute Trennung von Erfassung und Aufbereitung,
- lokale Nutzung ohne Cloudabhängigkeit.

#### Nachteile

- komplexere Pipeline,
- flüchtiges Zeitfenster zwischen Erfassung und Persistenz,
- Auditdienst wird sicherheitskritisch,
- Sequenzierung und Integritätsverkettung erforderlich.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS verwendet eine mehrstufige lokale Audit-Pipeline.

```text
Sicherheitsereignis
        ↓
Kernel-Audit-API
        ↓
CPU-lokaler Auditpuffer
        ↓
privilegierter Auditdienst
        ↓
Validierung und globale Sequenzierung
        ↓
Integritätsverkettung
        ↓
lokaler persistenter Audit Trail
```

Auditdaten werden standardmäßig ausschließlich lokal verarbeitet und gespeichert.

Eine externe Übertragung erfolgt nur nach ausdrücklicher, autorisierter Richtlinie.

---

## 8. Audit-Architektur

Das Audit-System besteht mindestens aus:

- Kernel Audit Core,
- CPU-lokalen Auditpuffern,
- Audit Policy Engine,
- privilegiertem Auditdienst,
- persistentem Audit Store,
- Audit Query Service,
- optionalem Exportdienst,
- Integritäts- und Rotationsverwaltung.

Der Kernel ist für die unvermeidbare Erfassung kritischer Kernelereignisse verantwortlich.

Komplexe Filterung, Speicherung und Benutzerabfragen erfolgen überwiegend im privilegierten Userspace-Dienst.

---

## 9. Ereignisklassen

NovaOS definiert mindestens folgende Auditklassen:

| Klasse | Inhalt |
|---|---|
| `BOOT` | Bootloader, Secure Startup, Systemgeneration |
| `AUTHENTICATION` | Anmeldung, Entsperrung, Authentisierungsfehler |
| `AUTHORIZATION` | Capability- und Berechtigungsentscheidungen |
| `PROCESS` | Prozess-, Thread-, Job- und Gruppenereignisse |
| `MEMORY` | sicherheitsrelevante Mapping- und Schutzänderungen |
| `MODULE` | Kernelmodul- und Treiberverwaltung |
| `FILESYSTEM` | geschützte Dateien, Richtlinien und Integrität |
| `DEVICE` | Gerätezugriff und Gerätezuordnung |
| `NETWORK` | sicherheitsrelevante Netzwerkoperationen |
| `POLICY` | Änderung von Sicherheitsrichtlinien |
| `RECOVERY` | Recovery, Rollback und Reparatur |
| `DEBUG` | privilegierte Debug- und Diagnosezugriffe |
| `INTEGRITY` | Integritätsverletzungen und Selbstheilung |
| `AUDIT` | Zustand des Audit-Systems selbst |
| `KERNEL` | Kernel-Panic und interne Sicherheitsverletzungen |

---

## 10. Ereignisprioritäten

Audit Events besitzen eine Priorität.

| Priorität | Bedeutung |
|---|---|
| `DEBUG` | nur im autorisierten Entwicklungsmodus |
| `INFO` | normale sicherheitsrelevante Aktivität |
| `NOTICE` | bemerkenswerte Sicherheitsänderung |
| `WARNING` | potenzielles Problem oder Policy-Abweichung |
| `ERROR` | fehlgeschlagene kritische Operation |
| `CRITICAL` | unmittelbare Sicherheitsgefährdung |
| `EMERGENCY` | Systemintegrität nicht mehr gewährleistet |

Die Priorität beeinflusst:

- Pufferreservierung,
- Verlustpolicy,
- Persistenzdringlichkeit,
- Benachrichtigungen,
- Aufbewahrungsdauer.

---

## 11. Verbindlich zu erfassende Ereignisse

Mindestens folgende Ereignisse sind in Produktionssystemen verpflichtend:

- Secure-Startup-Ergebnis,
- verwendete Systemgeneration,
- Wechsel des Sicherheitsmodus,
- fehlgeschlagene Kernelverifikation,
- Anmeldeerfolg und Anmeldefehler,
- Capability-Erteilung und -Entziehung,
- Änderung von Sicherheitsrichtlinien,
- Laden und Entladen von Kernelmodulen,
- Aktivierung privilegierter Debugfunktionen,
- Recovery- und Rollback-Vorgänge,
- Manipulations- oder Integritätsfehler,
- Auditüberlastung,
- Auditdienst-Ausfall,
- Änderung der Audit Policy,
- Löschung oder Export von Auditdaten,
- Kernel-Panic aufgrund einer Sicherheitsverletzung.

---

## 12. Nicht standardmäßig zu auditierende Ereignisse

Nicht jede normale Operation wird automatisch vollständig protokolliert.

Standardmäßig nicht als Einzelereignis erfasst werden:

- jeder Schedulerwechsel,
- jeder Speicherzugriff,
- jede Dateilesoperation,
- jedes Netzwerkpaket,
- jede GUI-Interaktion,
- jeder Timer,
- jede Heapallokation.

Solche Vorgänge würden:

- unverhältnismäßig große Datenmengen erzeugen,
- die Privatsphäre beeinträchtigen,
- das System verlangsamen,
- wichtige Ereignisse verdecken.

Sie können für gezielte, zeitlich begrenzte Diagnose aktiviert werden.

---

## 13. Ereignisschema

Jedes Audit Event enthält einen stabilen Kern von Pflichtfeldern.

```text
schema_version
event_id
event_class
event_type
severity
timestamp
sequence
cpu_id
subject
action
object
result
security_context
policy_generation
payload
integrity_metadata
```

Optionale Felder werden über Typ, Länge und Version beschrieben.

Unbekannte kritische Felder führen bei der Verarbeitung zu einem klaren Fehler.

---

## 14. Ereignisidentität

Jedes Audit Event erhält eine eindeutige Ereignis-ID.

Die ID kann zusammengesetzt sein aus:

```text
boot_id
cpu_id
cpu_local_sequence
```

Nach der Übernahme durch den Auditdienst erhält das Ereignis zusätzlich eine globale Auditsequenz.

Die Ereignis-ID bleibt unverändert.

---

## 15. Boot-ID

Jeder Systemstart erhält eine zufällige Boot-ID.

Sie verbindet alle Auditereignisse einer laufenden Systeminstanz.

Die Boot-ID:

- enthält keine Geräte- oder Benutzeridentität,
- wird bei jedem vollständigen Neustart neu erzeugt,
- wird in den Secure-Startup-Daten erfasst,
- ermöglicht die Trennung mehrerer Boot-Sessions.

---

## 16. Zeitstempel

Audit Events verwenden mindestens eine monotone Zeitquelle.

Mögliche Zeitfelder:

```text
monotonic_time_ns
wall_clock_time
wall_clock_status
```

Die monotone Zeit ist für Ereignisreihenfolge und Dauer verbindlich.

Kalenderzeit wird nur verwendet, wenn ihre Vertrauensqualität bekannt ist.

Eine Änderung der Systemuhr verändert keine bereits vergebenen monotonen Zeitstempel.

---

## 17. Zeitvertrauensstatus

Der Audit Record kennzeichnet die Qualität der Kalenderzeit.

Mögliche Werte:

```text
UNAVAILABLE
UNTRUSTED
FIRMWARE
LOCALLY_SYNCHRONIZED
AUTHENTICATED
```

Ein Ereignis mit unbestätigter Kalenderzeit bleibt über Boot-ID, Sequenz und monotone Zeit eindeutig einordbar.

---

## 18. Subject

Das handelnde Subjekt wird strukturiert beschrieben.

Mögliche Felder:

```text
process_id
thread_id
job_id
process_group_id
session_id
user_identity_id
security_domain_id
executable_id
capability_context
remote_principal_id
```

Nicht jedes Ereignis besitzt alle Felder.

Kernelinterne Ereignisse können ein Kernel-Subsystem als Subject verwenden.

---

## 19. Object

Das betroffene Objekt wird über stabile IDs beschrieben.

Mögliche Felder:

```text
object_type
object_id
object_generation
namespace_id
path_token
device_id
module_id
capability_id
policy_id
```

Kernelpointer werden niemals als öffentliche Objekt-IDs verwendet.

---

## 20. Action und Result

Die Aktion wird über einen stabilen Ereignistyp beschrieben.

Beispiele:

```text
CAPABILITY_GRANTED
CAPABILITY_REVOKED
PROCESS_CREATED
MODULE_LOADED
POLICY_CHANGED
RECOVERY_STARTED
AUDIT_EXPORT_CREATED
```

Das Ergebnis enthält mindestens:

```text
success
status_code
denial_reason
policy_rule_id
```

Freie Textmeldungen sind nur ergänzend und nicht die primäre Ereignisbedeutung.

---

## 21. Sicherheitskontext

Ein Audit Event kann den wirksamen Sicherheitskontext enthalten:

- Sicherheitsdomäne,
- Capability-Menge oder relevante Capability-ID,
- Job Object,
- Session,
- Vertrauensstufe des gestarteten Systems,
- Kernelisolationsmodus,
- aktive Richtliniengeneration,
- Debug- oder Development-Zustand.

Es werden nur die für das Ereignis relevanten Informationen gespeichert.

---

## 22. Datenschutz und Datenminimierung

Auditdaten werden nach dem Prinzip der Datenminimierung erzeugt.

Es werden nur Daten gespeichert, die für:

- Sicherheitsnachweis,
- Fehleranalyse,
- Missbrauchserkennung,
- Richtlinienprüfung,
- rechtliche Verpflichtungen

erforderlich sind.

Nicht automatisch gespeichert werden:

- vollständige Dokumentinhalte,
- Chat- oder Nachrichtentexte,
- Tastatureingaben,
- Passwörter,
- biometrische Rohdaten,
- vollständige Dateiinhalte,
- private Kommunikationsinhalte,
- unnötige Standortdaten.

---

## 23. Personenbezogene Daten

Audit Records können personenbezogene Daten enthalten, beispielsweise:

- Benutzeridentität,
- Anmeldezeitpunkt,
- Gerätezugriff,
- administrative Handlung.

Für solche Felder gelten:

- Zweckbindung,
- Zugriffsbeschränkung,
- definierte Aufbewahrungsfristen,
- Datenminimierung,
- kontrollierter Export,
- nachvollziehbare Löschung,
- Datenschutzkennzeichnung.

Die Audit Policy muss zwischen Sicherheitsbedarf und Datenschutz abwägen.

---

## 24. Pseudonymisierung

Wo eine direkte Identität nicht erforderlich ist, verwendet NovaOS:

- pseudonyme Identity-IDs,
- sessionbezogene Kennungen,
- gehashte externe Identifikatoren,
- getrennte Identitätsauflösung.

Die Zuordnung einer pseudonymen ID zu einer realen Person benötigt zusätzliche Rechte.

Pseudonymisierung ist nicht mit vollständiger Anonymisierung gleichzusetzen.

---

## 25. Verbotene Inhalte

Folgende Inhalte dürfen niemals als normale Audit-Nutzlast gespeichert werden:

- Passwörter,
- private kryptografische Schlüssel,
- Sitzungstoken,
- vollständige Capability-Geheimnisse,
- unmaskierte Authentisierungscodes,
- vollständige Speicherabbilder,
- Stack-Canaries,
- KASLR-Seeds,
- vollständige Kernel-Slides,
- Recovery-Geheimnisse,
- ungeschützte biometrische Rohdaten.

Ein Hash eines Geheimnisses kann ebenfalls sensibel sein und darf nicht automatisch gespeichert werden.

---

## 26. Pfade und Dateinamen

Dateipfade können personenbezogene oder vertrauliche Informationen enthalten.

NovaOS unterstützt deshalb mehrere Darstellungen:

- vollständiger Pfad für besonders autorisierte Sicherheitsfälle,
- Namespace-relative Objekt-ID,
- gehashter oder tokenisierter Pfad,
- Dateisystemobjekt-ID,
- redigierter Pfad.

Die Standardpolicy soll stabile Objekt-IDs gegenüber vollständigen Pfaden bevorzugen.

---

## 27. Netzwerkdaten

Bei Netzwerkereignissen werden standardmäßig nur notwendige Metadaten erfasst.

Mögliche Felder:

- Protokoll,
- lokale und entfernte Endpunkt-ID,
- Port,
- Verbindungsentscheidung,
- Sicherheitsrichtlinie,
- Ergebnis.

Paketinhalte und Anwendungsdaten werden nicht automatisch auditierbar gespeichert.

IP-Adressen können personenbezogene Daten sein und unterliegen der Aufbewahrungs- und Zugriffspolicy.

---

## 28. Kernel-Audit-API

Kernelkomponenten verwenden eine strukturierte Audit-API.

Eine mögliche Schnittstelle lautet:

```c
nova_status_t nova_audit_emit(
    const nova_audit_event_t* event
);
```

Für häufige Ereignisse können typsichere Helfer bereitgestellt werden:

```c
nova_status_t nova_audit_capability_decision(
    const nova_audit_capability_event_t* event
);

nova_status_t nova_audit_process_event(
    const nova_audit_process_event_t* event
);

nova_status_t nova_audit_policy_event(
    const nova_audit_policy_event_t* event
);
```

Freie Formatstrings sind für verbindliche Audit Events nicht zulässig.

---

## 29. Ereignisstruktur

Eine mögliche Kernstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t schema_version;

    nova_audit_event_id_t event_id;

    uint32_t event_class;
    uint32_t event_type;
    uint32_t severity;
    uint32_t flags;

    uint64_t monotonic_time_ns;
    uint64_t local_sequence;

    nova_cpu_id_t cpu_id;

    nova_audit_subject_t subject;
    nova_audit_object_t object;
    nova_audit_result_t result;

    uint64_t policy_generation;

    const void* payload;
    uint32_t payload_size;
    uint32_t payload_type;
} nova_audit_event_t;
```

Pointer innerhalb dieser Struktur werden nicht direkt persistent gespeichert. Der Kernel kopiert und serialisiert die Daten in einen eigenen Audit Record.

---

## 30. CPU-lokale Auditpuffer

Jede CPU besitzt einen begrenzten Audit-Ringpuffer.

Vorteile:

- keine globale Sperre im normalen Erfassungspfad,
- geringe Latenz,
- gute Mehrkernskalierbarkeit,
- Nutzung in Interrupt- und Exceptionpfaden möglich.

Jeder Puffer besitzt:

- CPU-lokale Sequenz,
- Schreibposition,
- Leseposition,
- Überlaufzähler,
- Prioritätsreserven,
- Integritätsmetadaten.

---

## 31. Auditpuffer und Kernelstacks

Audit Records werden nicht dauerhaft auf dem Kernelstack aufgebaut.

Größere Ereignisse verwenden:

- vorab reservierte CPU-lokale Speicherbereiche,
- begrenzte Kernelpools,
- feste Inline-Felder,
- referenzierte, sofort kopierte Nutzlasten.

Der Audit-Hotpath führt keine unbeschränkte Heapallokation durch.

---

## 32. Ereignisgrößen

Jeder Ereignistyp besitzt eine maximale Größe.

Der Kernel lehnt übergroße Auditnutzlasten ab oder reduziert sie auf eine sichere Zusammenfassung.

Mögliche Grenzen:

```text
max_kernel_record_size
max_userspace_record_size
max_optional_payload_size
max_string_length
```

Unbegrenzte Zeichenketten oder Binärdaten sind nicht zulässig.

---

## 33. Userspace-Auditquellen

Autorisierte Systemdienste können Audit Events erzeugen.

Diese Ereignisse werden als Userspace-erzeugt markiert.

Der Kernel beziehungsweise Auditdienst ergänzt vertrauenswürdige Metadaten wie:

- tatsächliche Prozess-ID,
- tatsächliche Sicherheitsdomäne,
- tatsächliche Benutzeridentität,
- aktive Capability-Rechte,
- Empfangszeitpunkt.

Ein Userspace-Dienst darf diese Felder nicht selbst fälschen.

---

## 34. Vertrauensstufen von Auditquellen

Auditquellen werden klassifiziert:

| Quelle | Vertrauensstufe |
|---|---|
| Kernel Core | höchste technische Vertrauensstufe |
| Kernelmodul | signatur- und policyabhängig |
| privilegierter Systemdienst | authentisiert |
| normale Anwendung | nicht vertrauenswürdig |
| externe Quelle | explizit gekennzeichnet |

Ein Ereignis behält die Herkunftskennzeichnung über seinen gesamten Lebenszyklus.

---

## 35. Globale Sequenzierung

CPU-lokale Ereignisse entstehen parallel.

Der Auditdienst ordnet sie anhand von:

- Boot-ID,
- monotonem Zeitstempel,
- CPU-ID,
- CPU-lokaler Sequenz,
- globaler Übernahmesequenz.

Es wird keine absolut physikalisch perfekte Reihenfolge behauptet, wenn Ereignisse gleichzeitig auf verschiedenen CPUs auftreten.

Stattdessen wird eine stabile, nachvollziehbare Ordnungsrelation erzeugt.

---

## 36. Integritätsverkettung

Persistente Audit Records werden kryptografisch verkettet.

Konzeptionell:

```text
record_hash[n] =
    Hash(
        record_hash[n-1]
        || canonical_record[n]
        || segment_metadata
    )
```

Dadurch werden nachträgliche:

- Änderungen,
- Löschungen innerhalb eines Segments,
- Umordnungen,
- Einfügungen

erkennbar.

---

## 37. Segmentierung

Der Audit Trail wird in begrenzte Segmente aufgeteilt.

Ein Segment besitzt mindestens:

```text
segment_id
boot_id
first_sequence
last_sequence
previous_segment_hash
record_count
creation_time
closing_time
policy_generation
final_hash
signature_or_mac
```

Geschlossene Segmente werden unveränderlich behandelt.

---

## 38. Signatur und MAC

Auditsegmente können geschützt werden durch:

- digitalen Signaturschlüssel,
- gerätegebundenen MAC-Schlüssel,
- TPM- oder HSM-gestützten Schlüssel,
- signierten Merkle-Root.

Die konkrete Methode hängt vom verfügbaren Trust Backend ab.

Ohne Hardwareanker bleibt die kryptografische Verkettung nützlich, bietet aber geringeren Schutz gegen einen vollständig kompromittierten Kernel oder Administrator.

---

## 39. Schlüsseltrennung

Audit-Schlüssel werden von anderen Systemschlüsseln getrennt.

Nicht gemeinsam verwendet werden:

- Secure-Boot-Signaturschlüssel,
- Dateisystem-Verschlüsselungsschlüssel,
- Benutzeranmeldeschlüssel,
- Audit-Integritätsschlüssel.

Mögliche Schlüsselrollen:

```text
AUDIT_SEGMENT_SIGNING
AUDIT_CHAIN_MAC
AUDIT_EXPORT_SIGNING
AUDIT_ARCHIVE_ENCRYPTION
```

---

## 40. Manipulationsgrenzen

Ein manipulationserschwerender Audit Trail kann nachträgliche Änderungen erkennen.

Er kann nicht vollständig garantieren, dass ein bereits kompromittierter Kernel:

- jedes Ereignis korrekt erzeugt,
- keine falschen Ereignisse erzeugt,
- keine Ereignisse vor der Erfassung unterdrückt,
- keine Schlüssel missbraucht.

Deshalb gilt:

> Audit ist ein Nachweis- und Erkennungsmechanismus, keine absolute Vertrauensquelle nach vollständiger Kernelkompromittierung.

Diese Grenze muss dokumentiert bleiben.

---

## 41. Boot-Audit

Der Bootloader sammelt frühe Sicherheitsereignisse.

Dazu gehören:

- Secure-Startup-Prüfungen,
- Manifestverifikation,
- Kernelverifikation,
- Rollbackentscheidung,
- Recovery-Auswahl,
- Trust-Backend-Status,
- KASLR- und Sicherheitsmodus.

Diese Ereignisse werden über das Boot-Handoff an den Kernel übergeben.

Der Kernel validiert und übernimmt sie in den Audit Trail.

---

## 42. Audit vor Start des Auditdienstes

Bevor der Userspace-Auditdienst aktiv ist, bleiben Ereignisse in:

- Boot Event Log,
- frühen Kernelpuffern,
- CPU-lokalen Auditpuffern.

Nach dem Start übernimmt der Auditdienst diese Ereignisse in definierter Reihenfolge.

Der Übergang wird selbst als Audit Event erfasst.

---

## 43. Auditdienst

Der Auditdienst ist ein privilegierter, isolierter Systemdienst.

Er ist verantwortlich für:

- Lesen der Kernelpuffer,
- Schemavalidierung,
- globale Sequenzierung,
- Filterung gemäß Policy,
- Integritätsverkettung,
- persistente Speicherung,
- Rotation,
- Abfrage,
- Export,
- Aufbewahrung und Löschung.

Der Auditdienst läuft in einem eigenen Job Object mit festen Ressourcenlimits und geeigneter Supervisor-Policy.

---

## 44. Auditdienst-Isolation

Der Auditdienst erhält nur die erforderlichen Capabilities.

Beispiele:

```text
CAP_AUDIT_READ_KERNEL
CAP_AUDIT_WRITE_STORE
CAP_AUDIT_SIGN_SEGMENT
CAP_AUDIT_QUERY
CAP_AUDIT_ROTATE
```

Er benötigt nicht automatisch:

- beliebigen Kernel-Speicherzugriff,
- vollständige Dateisystemadministration,
- Zugriff auf Benutzerinhalte,
- Netzwerkzugriff,
- Modul-Loader-Rechte.

---

## 45. Audit Store

Der persistente Audit Store verwendet:

- transaktionale Schreibvorgänge,
- Prüfsummen,
- Segmentgrenzen,
- Integritätsverkettung,
- restriktive Zugriffsrechte,
- definierte Speicherquoten.

Bevorzugt wird ein geschützter NovaFS-Bereich oder ein unveränderliches Audit-Subvolume.

Der Audit Store darf nicht als normale frei bearbeitbare Textdatei behandelt werden.

---

## 46. Lokalität

Auditdaten werden standardmäßig lokal gespeichert.

Es findet keine automatische Übertragung an:

- Cloudanbieter,
- Hersteller,
- zentrale Telemetriedienste,
- Werbenetzwerke,
- externe Analyseplattformen

statt.

Ein externer Export benötigt:

- eine ausdrückliche lokale Richtlinie,
- passende Capabilities,
- definierte Empfänger,
- Datenminimierung,
- Verschlüsselung,
- ein eigenes Audit Event.

---

## 47. Export

Auditdaten können autorisiert exportiert werden.

Ein Export enthält:

- ausgewählte Ereignisbereiche,
- Schema- und Versionsinformationen,
- Integritätsnachweise,
- Redaktionsinformationen,
- Exportzeitpunkt,
- Exporteuridentität,
- Exportpolicy.

Mögliche Formate:

- kanonisches Nova Audit Format,
- signiertes JSON,
- CBOR,
- kompatibles SIEM-Format.

Das interne kanonische Format bleibt maßgeblich.

---

## 48. Redaction

Vor einem Export können sensible Felder redigiert oder pseudonymisiert werden.

Der Export kennzeichnet:

- welche Felder entfernt wurden,
- welche Felder pseudonymisiert wurden,
- welche Policy angewendet wurde,
- ob die interne Ereignissignatur noch direkt prüfbar ist.

Redaction verändert nicht den internen Original-Audit-Trail.

---

## 49. Abfrage

Auditabfragen erfolgen über einen kontrollierten Query Service.

Mögliche Filter:

- Zeitbereich,
- Boot-ID,
- Ereignisklasse,
- Ereignistyp,
- Priorität,
- Prozess-ID,
- Job-ID,
- Benutzeridentität,
- Objekt-ID,
- Ergebnis,
- Policygeneration.

Freie direkte Dateizugriffe auf den Audit Store sind für normale Werkzeuge nicht vorgesehen.

---

## 50. Capability-Modell

Mögliche Audit-Capabilities sind:

```text
CAP_AUDIT_EMIT
CAP_AUDIT_QUERY
CAP_AUDIT_QUERY_IDENTITIES
CAP_AUDIT_READ_SENSITIVE
CAP_AUDIT_EXPORT
CAP_AUDIT_REDACT
CAP_AUDIT_ROTATE
CAP_AUDIT_DELETE
CAP_AUDIT_SET_POLICY
CAP_AUDIT_VERIFY
CAP_AUDIT_ADMIN
```

Capabilities können eingeschränkt werden auf:

- bestimmte Ereignisklassen,
- bestimmte Jobs oder Sicherheitsdomänen,
- bestimmte Zeiträume,
- bestimmte Identitätsfelder,
- bestimmte Exportziele,
- reine Integritätsprüfung.

---

## 51. Rollen- und Rechteverteilung

NovaOS trennt mindestens folgende Rollen:

| Rolle | Rechte |
|---|---|
| Audit Producer | darf definierte Ereignisse erzeugen |
| Audit Reader | darf freigegebene Ereignisse lesen |
| Security Analyst | darf sicherheitsrelevante Details analysieren |
| Privacy Officer | darf personenbezogene Verarbeitung prüfen |
| Audit Administrator | darf Policy und Rotation verwalten |
| Integrity Verifier | darf Verkettung und Signaturen prüfen |
| Export Operator | darf kontrollierte Exporte erzeugen |

Eine einzelne Rolle erhält nicht automatisch sämtliche Rechte.

---

## 52. Audit Policy

Die Audit Policy definiert:

```text
enabled_event_classes
mandatory_event_types
severity_thresholds
sampling_rules
retention_rules
redaction_rules
overflow_actions
notification_rules
export_rules
storage_quota
```

Die Policy ist versioniert und besitzt eine Generation.

Jedes Ereignis enthält die bei seiner Erzeugung beziehungsweise Übernahme aktive Policygeneration.

---

## 53. Policyänderungen

Eine Änderung der Audit Policy muss:

1. autorisiert werden,
2. syntaktisch und semantisch validiert werden,
3. atomar aktiviert werden,
4. eine neue Generation erhalten,
5. selbst auditiert werden.

Eine Policy darf nicht so geändert werden, dass ihre eigene Änderung unprotokolliert bleibt.

---

## 54. Unvermeidbare Ereignisse

Bestimmte Audit Events können durch normale Richtlinien nicht deaktiviert werden.

Dazu gehören:

- Audit-Systemstart,
- Audit-Systemstopp,
- Policyänderung,
- Auditüberlauf,
- Integritätsfehler,
- Auditexport,
- Auditlöschung,
- Aktivierung des Development Mode,
- Secure-Startup-Fehler,
- Kernel-Panic aufgrund einer Sicherheitsverletzung.

Nur eine speziell autorisierte Development- oder Recovery-Policy kann die Detailstufe verändern.

---

## 55. Filterung

Filterung erfolgt möglichst außerhalb des kritischsten Kernelpfads.

Der Kernel entscheidet nur:

- ob ein Ereignistyp grundsätzlich auditrelevant ist,
- welche Mindestpriorität gilt,
- welcher Puffer verwendet wird.

Komplexe Regeln werden durch den Auditdienst angewendet.

Verbindliche kritische Ereignisse dürfen nicht durch Userspace-Filter entfernt werden.

---

## 56. Sampling

Häufige, niedrig priorisierte Ereignisse können gesampelt oder aggregiert werden.

Sampling ist nicht zulässig für:

- Policyänderungen,
- Capability-Eskalationen,
- Kernelmodul-Laden,
- Auditfehler,
- Integritätsverletzungen,
- Recovery- und Rollback-Vorgänge,
- kritische Authentisierungsereignisse.

Gesampelte Ereignistypen enthalten Informationen über:

- Samplingrate,
- ausgelassene Anzahl,
- Aggregationsfenster.

---

## 57. Aggregation

Wiederholte gleichartige Ereignisse können zusammengefasst werden.

Beispiel:

```text
1000 identische verweigerte Zugriffe
innerhalb von 10 Sekunden
```

werden zu einem aggregierten Record mit:

```text
first_occurrence
last_occurrence
count
representative_subject
representative_object
status
```

Kritische Einzelereignisse bleiben separat erhalten.

---

## 58. Überlastungsmodell

Auditpuffer und Audit Store besitzen feste Grenzen.

Bei Überlastung gelten prioritätsabhängige Regeln.

Mögliche Reaktionen:

| Ereignisklasse | Reaktion |
|---|---|
| niedrig | Sampling oder Aggregation |
| normal | begrenztes Verwerfen mit Zähler |
| hoch | reservierten Puffer verwenden |
| kritisch | Operation blockieren, verweigern oder Safe-State-Policy |
| emergency | minimalen Notfallrecord sichern und Panic-/Recovery-Pfad nutzen |

Ein Überlauf wird immer selbst erfasst.

---

## 59. Reservierte Puffer

Für kritische Ereignisse existieren reservierte Pufferbereiche.

Normale Ereignisse dürfen diese Reserve nicht verbrauchen.

Reserviert werden mindestens Plätze für:

- Auditüberlauf,
- Auditdienst-Ausfall,
- Policyänderung,
- Integritätsverletzung,
- Kernel-Panic,
- Secure-Startup-Fehler.

Auch reservierte Bereiche bleiben begrenzt.

---

## 60. Verhalten bei vollem Auditpuffer

Mögliche Policies sind:

```text
DROP_LOW_PRIORITY
AGGREGATE
DENY_AUDITED_OPERATION
BLOCK_NON_REALTIME_CALLER
ENTER_DEGRADED_MODE
ENTER_SAFE_STATE
PANIC_ON_CRITICAL_LOSS
```

Der Kernel darf nicht unbegrenzt auf einen ausgefallenen Auditdienst warten.

Die konkrete Reaktion hängt von Ereignispriorität und Systemprofil ab.

---

## 61. Fail Open und Fail Closed

Auditoperationen können bei Ausfall unterschiedlich behandelt werden.

### 61.1 Fail Open

Die verursachende Operation wird fortgesetzt, der Auditverlust wird markiert.

Geeignet für:

- normale nicht kritische Leseoperationen,
- zeitkritische Systemfunktionen,
- bestimmte Verfügbarkeitsprofile.

### 61.2 Fail Closed

Die verursachende Operation wird abgelehnt, wenn sie nicht sicher auditiert werden kann.

Geeignet für:

- Änderung von Sicherheitsrichtlinien,
- Erteilung privilegierter Capabilities,
- Laden eines Kernelmoduls,
- Aktivierung eines Debugmodus,
- Auditexport oder -löschung.

Die Policy wird pro Ereignistyp festgelegt.

---

## 62. Echtzeitpfade

Echtzeitpfade dürfen nicht durch normale Audit-I/O blockiert werden.

Sie verwenden:

- vorab reservierte Records,
- CPU-lokale Puffer,
- feste Ereignisgrößen,
- keine dynamische Allokation,
- keine synchrone Datenträgerspeicherung.

Ist ein kritisches Echtzeitereignis nicht speicherbar, wird die vorher definierte Safety- oder Degraded-Policy ausgeführt.

---

## 63. Interrupt- und Exceptionpfade

Audit aus Interrupt- oder Exceptionkontext verwendet ausschließlich nicht blockierende Kernelpfade.

Nicht zulässig sind:

- normale Heapallokation,
- Dateisystemzugriff,
- komplexe Formatierung,
- Namensauflösung,
- externe Übertragung.

Der Record wird minimal erfasst und später durch den Auditdienst ergänzt.

---

## 64. Kernel-Panic

Bei einer Kernel-Panic versucht NovaOS, einen minimalen Panic-Audit-Record zu sichern.

Dieser kann enthalten:

- Boot-ID,
- Panic-ID,
- CPU-ID,
- Thread- und Prozess-ID,
- Sicherheitsmodus,
- Fehlerklasse,
- Kernel-Build-ID,
- monotone Zeit,
- letzte Auditsequenz,
- Auditkettenzustand.

Der Panicpfad darf nicht von einem funktionsfähigen normalen Auditdienst abhängen.

---

## 65. Panic-Persistenz

Je nach Plattform kann der Panic-Record gespeichert werden in:

- vorab reserviertem persistentem Speicher,
- Crash-Dump-Header,
- Recovery-Metadaten,
- TPM- oder Firmware-Ereignisspeicher,
- geschütztem NVRAM.

Ist keine Persistenz möglich, bleibt der Record im Crash-Dump oder geht beim Neustart verloren. Dieser Verlust darf nicht als erfolgreiche Persistenz gemeldet werden.

---

## 66. Auditdienst-Ausfall

Der Kernel überwacht den Auditdienst über:

- Heartbeat,
- Pufferverbrauch,
- Lesefortschritt,
- Supervisorstatus.

Bei Ausfall:

1. Kernelpuffer weiterverwenden,
2. reservierte Kapazität schützen,
3. Supervisor benachrichtigen,
4. Dienst neu starten,
5. Ausfallereignis erzeugen,
6. bei anhaltendem Ausfall Policy anwenden.

Der Auditdienst läuft außerhalb der von ihm überwachten normalen Anwendungsjobs.

---

## 67. Neustart des Auditdienstes

Nach einem Neustart:

1. authentisiert sich der Auditdienst,
2. übernimmt die aktuelle Policygeneration,
3. liest noch vorhandene Kernelrecords,
4. prüft den letzten persistenten Segmentzustand,
5. setzt die Integritätskette fort,
6. erzeugt ein Restart-Ereignis.

Ein Kettenbruch wird ausdrücklich dokumentiert und nicht stillschweigend repariert.

---

## 68. Speicherquoten

Der Audit Store besitzt eine reservierte Speicherquote.

Diese verhindert:

- Verdrängung durch normale Benutzerdaten,
- vollständige Systembelegung durch Auditdaten,
- unkontrolliertes Wachstum.

Die Quote kann getrennte Reserven für:

- aktuelle Segmente,
- kritische Ereignisse,
- Recovery,
- Exportvorbereitung

enthalten.

---

## 69. Rotation

Auditsegmente werden rotiert nach:

- Größe,
- Zeit,
- Ereignisanzahl,
- Systemgeneration,
- Bootgrenze,
- Policyänderung.

Vor der Rotation wird das aktuelle Segment:

1. vollständig geschrieben,
2. gehasht,
3. verkettet,
4. signiert oder mit MAC geschützt,
5. als geschlossen markiert.

Ein offenes Segment wird nach einem Absturz kontrolliert wiederhergestellt oder als unvollständig gekennzeichnet.

---

## 70. Aufbewahrung

Aufbewahrungsregeln können abhängig sein von:

- Ereignisklasse,
- Priorität,
- Datenschutzkategorie,
- gesetzlicher Verpflichtung,
- Systemprofil,
- verfügbarem Speicher.

Mögliche Regeln:

```text
max_age
max_storage
minimum_retention
legal_hold
security_hold
delete_after_export
```

Eine längere Aufbewahrung ist nicht automatisch sicherer oder datenschutzfreundlicher.

---

## 71. Löschung

Auditdaten werden nur über eine kontrollierte Löschoperation entfernt.

Die Löschung:

- benötigt eine Capability,
- berücksichtigt Legal- und Security-Holds,
- wird selbst auditiert,
- dokumentiert Segment-IDs und Grund,
- verändert geschlossene Segmente nicht heimlich.

Bei normaler Rotation kann ein signierter Tombstone beziehungsweise Löschrecord erhalten bleiben.

---

## 72. Wiederherstellung nach Stromausfall

Der Audit Store verwendet transaktionale Segmentmetadaten.

Nach einem Stromausfall wird:

1. letztes bestätigtes Segment ermittelt,
2. offenes Segment validiert,
3. gültige vollständige Records übernommen,
4. beschädigter Rest verworfen und dokumentiert,
5. Kettenfortsetzung geprüft,
6. Recovery-Ereignis erzeugt.

Beschädigungen werden nicht als normaler sauberer Abschluss dargestellt.

---

## 73. Selbst-Auditierung

Das Audit-System protokolliert sicherheitsrelevante Vorgänge an sich selbst.

Dazu gehören:

- Start und Stopp,
- Policyänderung,
- Leserzugriff auf sensible Daten,
- Export,
- Rotation,
- Löschung,
- Integritätsprüfung,
- Schlüsselrotation,
- Überlastung,
- Dienstneustart,
- Speicherquotenänderung.

Dadurch bleibt die Verwaltung des Audit-Systems nachvollziehbar.

---

## 74. Integritätsprüfung

Ein autorisierter Verifier kann prüfen:

- Record-Hashes,
- Segmentverkettung,
- Segmentabschluss,
- Signatur oder MAC,
- Boot-ID-Kontinuität,
- Sequenzlücken,
- Policygenerationen,
- Tombstones,
- Kettenbrüche.

Das Ergebnis wird als eigener Audit-Verifikationsbericht gespeichert.

---

## 75. Audit und Job Objects

Job Objects liefern einen stabilen Verwaltungskontext für mehrere Prozesse.

Audit Events können enthalten:

- direktes Job Object,
- übergeordnete Jobhierarchie,
- Ressourcenlimitverletzungen,
- Jobbeendigung,
- Supervisoraktionen.

Die vollständige Jobhierarchie wird nicht bei jedem Ereignis dupliziert. Stattdessen werden stabile Job-IDs und Generationen verwendet.

---

## 76. Audit und Capabilities

Capability-Entscheidungen gehören zu den zentralen Auditquellen.

Erfasst werden können:

- Capability-Typ,
- Quelle,
- Zielsubjekt,
- Umfang,
- Gültigkeitsdauer,
- Delegation,
- Ergebnis,
- Policyregel,
- Widerruf.

Capability-Geheimnisse oder nicht öffentliche Tokenwerte werden nicht gespeichert.

---

## 77. Audit und Prozesslebenszyklus

Prozessereignisse können umfassen:

- Prozess erstellt,
- Prozess gestartet,
- ausführbare Identität,
- Parent-Prozess,
- Job Object,
- Sicherheitsdomäne,
- Prozess beendet,
- Exitstatus,
- ungewöhnliche Beendigung,
- Neustart durch Supervisor.

Normale Threadwechsel werden nicht als Einzelereignisse auditiert.

---

## 78. Audit und Dateisystem

Dateisystem-Audit wird policybasiert aktiviert.

Typische auditierbare Operationen:

- Änderung geschützter Systemdateien,
- Änderung von Dateischutzrichtlinien,
- Zugriff auf besonders sensible Objekte,
- Mounten und Aushängen,
- Aktivierung eines Snapshots,
- Recovery oder Scrubbing,
- Integritätsfehler.

Nicht jeder normale Dateizugriff wird protokolliert.

---

## 79. Audit und Kernelmodule

Für Kernelmodule werden mindestens erfasst:

- Modul-ID,
- Signaturschlüssel-ID,
- Hash,
- ABI-Version,
- Ladezeitpunkt,
- anforderndes Subjekt,
- Ergebnis,
- Entladezeitpunkt,
- erteilte Kernel-Capabilities.

Moduladressen und KASLR-relevante Werte werden nicht in normalen Audit Records gespeichert.

---

## 80. Audit und Recovery

Recovery-Vorgänge erhalten eine eigene Ereignisklasse.

Erfasst werden:

- Recovery-Auslöser,
- gewählte Systemgeneration,
- Integritätsstatus,
- Reparaturoperationen,
- Rollbackentscheidung,
- verwendete Autorisierung,
- Ergebnis,
- Änderungen an Sicherheitsversionen.

Recovery darf Audit nicht als unkontrollierten Umgehungspfad deaktivieren.

---

## 81. API-Grundmodell

Eine mögliche Userspace-API lautet:

```c
nova_status_t nova_audit_query(
    const nova_audit_query_t* query,
    nova_handle_t* result_stream
);

nova_status_t nova_audit_verify(
    const nova_audit_verify_request_t* request,
    nova_audit_verify_result_t* result
);

nova_status_t nova_audit_export(
    const nova_audit_export_request_t* request,
    nova_handle_t* export_object
);

nova_status_t nova_audit_get_status(
    nova_audit_status_t* status
);
```

Direkter Schreibzugriff auf den persistenten Audit Store ist nicht Teil der allgemeinen API.

---

## 82. Auditstatus

Eine mögliche Statusstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    bool enabled;
    bool kernel_capture_active;
    bool audit_service_active;
    bool persistent_store_available;
    bool integrity_chain_valid;
    bool degraded;

    uint64_t active_policy_generation;
    uint64_t global_sequence;
    uint64_t buffered_records;
    uint64_t persistent_records;
    uint64_t dropped_records;
    uint64_t aggregated_records;

    uint64_t storage_used;
    uint64_t storage_limit;

    nova_status_t last_error;
} nova_audit_status_t;
```

---

## 83. Fehlercodes

Mögliche Status- und Fehlercodes sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_AUDIT_DISABLED
NOVA_STATUS_AUDIT_BUFFER_FULL
NOVA_STATUS_AUDIT_RECORD_TOO_LARGE
NOVA_STATUS_AUDIT_SCHEMA_INVALID
NOVA_STATUS_AUDIT_POLICY_DENIED
NOVA_STATUS_AUDIT_SERVICE_UNAVAILABLE
NOVA_STATUS_AUDIT_STORE_FULL
NOVA_STATUS_AUDIT_STORE_CORRUPTED
NOVA_STATUS_AUDIT_CHAIN_BROKEN
NOVA_STATUS_AUDIT_SIGNATURE_INVALID
NOVA_STATUS_AUDIT_QUERY_DENIED
NOVA_STATUS_AUDIT_EXPORT_DENIED
NOVA_STATUS_AUDIT_RETENTION_CONFLICT
NOVA_STATUS_AUDIT_DEGRADED
```

Ein fehlerhaftes normales Userspace-Auditereignis darf keinen Kernel-Panic auslösen.

---

## 84. Sicherheitsmodi

NovaOS definiert folgende Auditmodi:

| Modus | Verhalten |
|---|---|
| `ENFORCING` | kritische Ereignisse und Policies sind verbindlich |
| `STANDARD` | normale Produktionsauditierung |
| `PRIVACY_MINIMAL` | nur zwingende Sicherheitsereignisse |
| `FORENSIC` | zeitlich begrenzte erhöhte Detailstufe |
| `DEVELOPMENT` | zusätzliche Debugereignisse |
| `DEGRADED` | Speicher- oder Dienstfehler, reduzierte Garantien |
| `RECOVERY` | minimale Recovery-Auditierung |

Der Moduswechsel wird immer selbst auditiert.

---

## 85. Forensic Mode

Der Forensic Mode kann für einen begrenzten Zeitraum zusätzliche Ereignisse erfassen.

Er benötigt:

- besondere Capability,
- definierten Zweck,
- begrenzte Dauer,
- ausgewählte Ereignisklassen,
- Datenschutzprüfung,
- automatische Rückkehr zur Standardpolicy.

Eine dauerhafte Totalüberwachung ist nicht das Ziel des Audit-Systems.

---

## 86. Kein verborgenes Audit

NovaOS verwendet Audit nicht als geheime Benutzerüberwachung.

Systemweit auditierte Kategorien und Aufbewahrungsregeln sollen für autorisierte lokale Benutzer und Administratoren transparent sein.

Geheime Ermittlungs- oder Behördenfunktionen sind nicht Bestandteil der Kernel-Auditarchitektur.

---

## 87. Performance

Der Kernel-Audit-Hotpath muss:

- CPU-lokale Puffer verwenden,
- keine normale Dateisystem-I/O ausführen,
- keine unbeschränkte Heapallokation durchführen,
- keine komplexe Textformatierung durchführen,
- globale Sperren vermeiden,
- feste Größenlimits einhalten.

Aufwendige Verarbeitung erfolgt im Auditdienst.

Audit darf nicht global deaktiviert werden, nur weil einzelne Ereignisklassen hohe Last erzeugen. Stattdessen werden Filterung, Aggregation und Sampling angepasst.

---

## 88. Konsequenzen

### 88.1 Positive Konsequenzen

- sicherheitsrelevante Vorgänge werden nachvollziehbar,
- strukturierte und versionierte Ereignisse,
- lokale Verarbeitung ohne Cloudzwang,
- Integritätsverkettung erkennt nachträgliche Manipulationen,
- klare Capability- und Datenschutzregeln,
- Mehrkernskalierbarkeit durch CPU-lokale Puffer,
- kontrollierte Überlastungsstrategien,
- Integration von Boot, Kernel, Userspace und Recovery,
- Grundlage für Forensik und Compliance.

### 88.2 Negative Konsequenzen

- zusätzlicher Speicher- und I/O-Verbrauch,
- Auditdienst wird sicherheitskritisch,
- komplexe Aufbewahrungs- und Datenschutzrichtlinien,
- Integritätsschlüssel müssen verwaltet werden,
- Ereignisschemata müssen langfristig gepflegt werden,
- Audit kann bei Fehlkonfiguration zu hoher Last führen.

### 88.3 Risiken

- Auditüberlauf,
- Verlust vor Persistenz,
- kompromittierter Auditdienst,
- Offenlegung personenbezogener Daten,
- Speicherung von Geheimnissen,
- unbemerkte Kettenbrüche,
- Denial of Service durch Ereignisfluten,
- zu lange Aufbewahrung,
- missbräuchlicher Forensic Mode.

Diese Risiken werden begrenzt durch:

- reservierte Puffer,
- feste Größenlimits,
- Datenminimierung,
- Capabilities,
- Integritätsverkettung,
- Job- und Ressourcenlimits,
- automatische Rotation,
- Policygenerationen,
- Selbst-Auditierung.

---

## 89. Auswirkungen auf andere Subsysteme

### 89.1 Secure Kernel Startup

Boot- und Trust-State-Ereignisse werden in den Audit Trail übernommen.

### 89.2 Kerneltelemetrie

Audit verwendet CPU-lokale, nicht blockierende Telemetriemechanismen.

### 89.3 Capability-System

Capability-Entscheidungen und Audit-Zugriffsrechte werden gemeinsam verwaltet.

### 89.4 Prozessverwaltung

Prozess-, Job-, Session- und Sicherheitsdomänen-IDs werden als Subject-Kontext verwendet.

### 89.5 Dateisystem

Der Audit Store benötigt transaktionale, integritätsgeschützte und quotareservierte Speicherung.

### 89.6 Recovery

Recovery-Aktionen und Kettenbrüche müssen nachvollziehbar bleiben.

### 89.7 Kernelmodulsystem

Laden, Entladen und Signaturstatus werden verbindlich auditiert.

### 89.8 Authentisierung

Anmelde- und Berechtigungsentscheidungen liefern strukturierte Audit Events.

### 89.9 Netzwerk

Exporte und sicherheitsrelevante Netzwerkentscheidungen werden policybasiert erfasst.

### 89.10 Datenschutz

Aufbewahrung, Redaction, Pseudonymisierung und Export müssen zentral steuerbar sein.

---

## 90. Implementierungsrichtlinien

1. Audit Events verwenden strukturierte, versionierte Schemas.
2. Freie Textlogs ersetzen keine verbindlichen Audit Records.
3. Der Kernel verwendet CPU-lokale, begrenzte Auditpuffer.
4. Der Audit-Hotpath führt keine normale Dateisystem-I/O aus.
5. Ein privilegierter Auditdienst übernimmt Sequenzierung und Persistenz.
6. Auditdaten werden standardmäßig ausschließlich lokal gespeichert.
7. Persistente Records werden kryptografisch verkettet.
8. Geschlossene Auditsegmente werden unveränderlich behandelt.
9. Kritische Ereignisse besitzen reservierte Pufferkapazität.
10. Auditüberlauf und Auditdienst-Ausfall werden selbst auditiert.
11. Geheimnisse und vollständige Inhaltsdaten werden nicht als normale Auditnutzlast gespeichert.
12. Personenbezogene Daten werden minimiert und klassifiziert.
13. Policyänderungen sind atomar, generationengesichert und selbst auditierbar.
14. Fail-Open- und Fail-Closed-Verhalten wird pro Ereignistyp festgelegt.
15. Echtzeit- und Interruptpfade verwenden ausschließlich nicht blockierende Erfassung.
16. Der Auditdienst läuft in einem isolierten Job Object.
17. Auditabfragen und Exporte benötigen feingranulare Capabilities.
18. Löschung, Export und Rotation werden selbst auditiert.
19. Ein Kettenbruch wird niemals stillschweigend verborgen.
20. Ein externer Export erfolgt nur nach ausdrücklicher lokaler Autorisierung.

---

## 91. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Erzeugung eines gültigen Kernel-Audit-Events,
- Erzeugung eines Userspace-Audit-Events,
- Ergänzung vertrauenswürdiger Subject-Metadaten,
- ungültiges Ereignisschema,
- übergroßer Audit Record,
- CPU-lokale Sequenzen,
- globale Sequenzierung,
- parallele Ereignisse auf mehreren CPUs,
- CPU-Hotplug,
- früher Boot-Audit,
- Übernahme des Boot Event Logs,
- Start des Auditdienstes,
- Neustart des Auditdienstes,
- Auditdienst-Ausfall,
- voller CPU-lokaler Puffer,
- Nutzung reservierter Puffer,
- voller Audit Store,
- Fail-Open-Operation,
- Fail-Closed-Operation,
- Sampling,
- Aggregation,
- kritisches Ereignis ohne Sampling,
- Segmentabschluss,
- Record-Verkettung,
- Segmentverkettung,
- manipuliertes Auditsegment,
- fehlender Record,
- umgeordneter Record,
- ungültige Signatur,
- Schlüsselrotation,
- Stromausfall während eines offenen Segments,
- Recovery eines unvollständigen Segments,
- Policyänderung,
- unvermeidbares Ereignis,
- Aufbewahrungsregel,
- Legal Hold,
- kontrollierte Löschung,
- signierter Export,
- Redaction,
- pseudonymisierte Identität,
- verweigerte sensible Abfrage,
- Forensic Mode,
- automatisches Ende des Forensic Mode,
- Echtzeit-Auditpfad,
- Interrupt-Auditpfad,
- Kernel-Panic-Record,
- kein Geheimnis in Audit Records,
- kein KASLR-Slide oder Stack-Canary im Export,
- Job- und Capability-Kontext,
- Performance unter Ereignislast,
- Fuzzing des Record-Parsers,
- Fuzzing der Query- und Exportfilter.

---

## 92. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- das vollständige binäre Auditformat,
- den konkreten Hash- und Signaturalgorithmus,
- gesetzliche Aufbewahrungsfristen einzelner Einsatzbereiche,
- eine zentrale Cloud- oder SIEM-Infrastruktur,
- vollständige Intrusion Detection,
- automatische Verhaltensbewertung,
- geheimdienstliche oder verdeckte Überwachungsfunktionen,
- die vollständige Benutzeroberfläche des Audit Viewers,
- die endgültige öffentliche Query-ABI,
- Remote Attestation des Audit Trails.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 93. Verbindliche Invarianten

1. Sicherheitsrelevante Audit Events verwenden strukturierte, versionierte Records.
2. Freie Textlogs sind kein Ersatz für verbindliche Audit Events.
3. Jeder Systemstart besitzt eine eigene Boot-ID.
4. Jedes Audit Event besitzt eine eindeutige Ereignis-ID.
5. CPU-lokale und globale Sequenzen dürfen nicht stillschweigend zurückgesetzt werden.
6. Kritische Ereignisse besitzen reservierte Erfassungskapazität.
7. Ein Auditüberlauf muss selbst erkennbar und auditierbar sein.
8. Der Kernel-Hotpath führt keine normale persistente Audit-I/O aus.
9. Persistente Audit Records werden manipulationserschwerend verkettet.
10. Geschlossene Segmente werden nicht nachträglich unbemerkt verändert.
11. Ein Kettenbruch wird ausdrücklich gekennzeichnet.
12. Passwörter, private Schlüssel, Tokens, Stack-Canaries und KASLR-Seeds werden nicht als normale Auditnutzlast gespeichert.
13. Kernelpointer dienen nicht als öffentliche Auditobjekt-IDs.
14. Personenbezogene Daten werden auf den erforderlichen Umfang begrenzt.
15. Auditdaten werden standardmäßig lokal gespeichert.
16. Externe Übertragung erfordert eine ausdrückliche Richtlinie und Capability.
17. Exporte, Löschungen und Policyänderungen werden selbst auditiert.
18. Verbindliche Auditereignisse dürfen nicht durch normale Filter deaktiviert werden.
19. Eine Policyänderung wird atomar aktiviert und erhält eine neue Generation.
20. Ein kompromittierter Auditdienst darf keine Kernel-Auditpuffer oder geschlossenen Segmente unkontrolliert verändern.
21. Der Auditdienst erhält nur die für seine Aufgaben erforderlichen Capabilities.
22. Audit darf Echtzeit- und Interruptpfade nicht durch normale I/O blockieren.
23. Fail-Open- und Fail-Closed-Verhalten wird für kritische Operationen ausdrücklich definiert.
24. Audit darf nicht als verborgene allgemeine Benutzerüberwachung verwendet werden.
25. Eine Capability allein darf keine Auditspur unbemerkt löschen oder deaktivieren.
26. KI- oder lernbasierte Komponenten dürfen Audit-, Aufbewahrungs-, Datenschutz-, Integritäts-, Capability- oder Exportregeln nicht umgehen.

---

## 94. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-SEC-0004 – NX Memory
- ADR-SEC-0005 – SMEP/SMAP
- ADR-SEC-0006 – Kernel Isolation
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0009 – Job Objects
- ADR-PROC-0010 – Prozessgruppen
- NPSPEC-AUDIT-0001 – Audit Specification
- NPSPEC-SECURESTARTUP-0001 – Secure Kernel Startup Specification
- NPSPEC-TELEMETRY-0001 – Local Telemetry Specification
- NPSPEC-CAPABILITY-0001 – Capability Specification
- NPSPEC-JOB-0001 – Job Object Specification
- NPSPEC-RECOVERY-0001 – Recovery Specification

---

## 95. Zusammenfassung der Entscheidung

NovaOS verwendet eine mehrstufige lokale Audit-Pipeline.

Sicherheitsrelevante Ereignisse werden zunächst in begrenzten CPU-lokalen Kernelpuffern erfasst. Ein privilegierter und isolierter Auditdienst übernimmt anschließend:

- Schemavalidierung,
- globale Sequenzierung,
- Filterung,
- Integritätsverkettung,
- persistente Speicherung,
- Rotation,
- Abfrage,
- Export.

Persistente Audit Records werden in kryptografisch verketteten Segmenten gespeichert. Nachträgliche Änderungen, Umordnungen und Löschungen innerhalb geschlossener Segmente werden dadurch erkennbar.

Auditdaten bleiben standardmäßig lokal. Eine externe Übertragung benötigt eine ausdrückliche Richtlinie, passende Capabilities, Datenminimierung und einen eigenen Audit Record.

Passwörter, private Schlüssel, Tokens, Stack-Canaries, KASLR-Seeds und vollständige Benutzerinhalte werden nicht als normale Auditdaten gespeichert. Personenbezogene Daten unterliegen Zweckbindung, Zugriffsschutz und definierten Aufbewahrungsregeln.

Auditüberlastung, Auditdienst-Ausfall, Policyänderungen, Exporte und Löschungen werden selbst auditiert. Für kritische Ereignisse existiert reservierte Pufferkapazität und ein ausdrücklich definiertes Fail-Open- oder Fail-Closed-Verhalten.