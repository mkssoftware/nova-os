# ADR-IPC-0002 – Shared Memory

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0002 |
| Titel | Shared Memory |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation und Speicherverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-MM-0001, ADR-MM-0013, ADR-SEC-0004, ADR-SEC-0006 |
| Nachfolgende Spezifikation | NPSPEC-IPC-SHARED-MEMORY |

---

## 1. Kontext

NovaOS benötigt neben Message Passing einen effizienten IPC-Mechanismus für größere Datenmengen.

Das Kopieren großer Datenblöcke durch den Kernel verursacht unnötige Speicherbandbreite, höhere Latenzen und zusätzliche CPU-Last. Shared Memory ermöglicht mehreren Prozessen, kontrolliert auf dieselben physischen Speicherseiten zuzugreifen.

Typische Einsatzbereiche sind:

- Grafik- und Bildpuffer
- Audio- und Videodaten
- große Dateien oder Datenblöcke
- Datenbanken und Caches
- Kommunikation mit Gerätediensten
- gemeinsam genutzte Ringpuffer
- Zero-Copy-Datenübertragung

---

## 2. Problemstellung

Shared Memory verbessert die Leistung, schwächt bei unkontrollierter Verwendung jedoch die Isolation zwischen Prozessen.

Die Architektur muss deshalb sicherstellen, dass:

- nur autorisierte Prozesse Speicher einblenden können,
- Zugriffsrechte pro Mapping festgelegt werden,
- ausführbarer gemeinsamer Speicher eingeschränkt bleibt,
- Lebenszeit und Eigentümerschaft eindeutig sind,
- Größenänderungen keine ungültigen Speicherzugriffe verursachen,
- Prozessfehler andere Teilnehmer möglichst wenig beeinflussen.

Shared Memory benötigt außerdem einen getrennten Synchronisationsmechanismus, da gemeinsam sichtbarer Speicher allein keine geordnete Kommunikation garantiert.

---

## 3. Entscheidung

NovaOS führt capability-geschützte Shared-Memory-Objekte als native Kernelobjekte ein.

Ein Shared-Memory-Objekt repräsentiert eine Menge physischer Speicherseiten, die in die virtuellen Adressräume mehrerer Prozesse eingeblendet werden können.

Die Berechtigung zum Zugriff wird ausschließlich über Capabilities und Mapping-Rechte gesteuert.

Message Passing bleibt der bevorzugte Mechanismus für Steuerinformationen. Shared Memory wird für größere Nutzdaten verwendet.

---

## 4. Ziele

- effiziente Übertragung großer Datenmengen
- kontrollierte gemeinsame Speichernutzung
- Integration in das Capability-Modell
- getrennte Rechte für Lesen und Schreiben
- Unterstützung unterschiedlicher virtueller Adressen
- klar definierte Lebenszeit
- Schutz vor ausführbarem Shared Memory
- Kombination mit Message Passing

---

## 5. Nichtziele

Diese ADR definiert nicht:

- Synchronisationsprimitive
- persistente Memory-Mapped Files
- verteilten gemeinsamen Speicher
- Zugriff auf physischen Gerätespeicher
- DMA-Puffer
- die interne Struktur anwendungsspezifischer Ringpuffer

Diese Bereiche werden in eigenen ADRs oder Spezifikationen behandelt.

---

## 6. Shared-Memory-Objekt

Ein Shared-Memory-Objekt besitzt mindestens folgende Eigenschaften:

```c
typedef struct shared_memory_info {
    uint64_t object_id;
    uint64_t size;
    uint32_t flags;
    uint32_t mapping_count;
    process_id_t owner;
} shared_memory_info_t;
```

| Eigenschaft | Bedeutung |
|---|---|
| Objekt-ID | Eindeutige Kernelkennung |
| Größe | Reservierte Objektgröße in Bytes |
| Eigentümer | Erzeugender Prozess oder Systemdienst |
| Flags | Speicher- und Sicherheitsoptionen |
| Mapping-Anzahl | Anzahl aktiver Einblendungen |
| Seitenliste | Zugeordnete physische Speicherseiten |

Das Kernelobjekt enthält keine feste virtuelle Adresse. Jeder Prozess kann das Objekt an einer anderen geeigneten Adresse einblenden.

---

## 7. Erzeugung

Ein Prozess erzeugt Shared Memory über eine Kerneloperation:

```c
status_t shm_create(
    uint64_t size,
    uint32_t flags,
    cap_handle_t *shm_handle
);
```

Unterstützte Erzeugungsflags:

```c
#define SHM_FLAG_ZEROED          (1u << 0)
#define SHM_FLAG_LAZY_ALLOC      (1u << 1)
#define SHM_FLAG_FIXED_SIZE      (1u << 2)
#define SHM_FLAG_SEALABLE        (1u << 3)
```

Neue Shared-Memory-Seiten müssen vor der ersten Sichtbarkeit vollständig mit Null initialisiert werden.

`SHM_FLAG_LAZY_ALLOC` erlaubt die physische Seitenerzeugung erst beim ersten Zugriff.

---

## 8. Mapping

Ein Shared-Memory-Objekt wird über eine Capability in den virtuellen Adressraum eines Prozesses eingeblendet:

```c
status_t shm_map(
    cap_handle_t shm_handle,
    uintptr_t address_hint,
    uint64_t offset,
    uint64_t length,
    uint32_t protection,
    void **mapped_address
);
```

Unterstützte Schutzrechte:

```c
#define SHM_PROT_READ       (1u << 0)
#define SHM_PROT_WRITE      (1u << 1)
#define SHM_PROT_EXECUTE    (1u << 2)
```

Die angeforderten Mapping-Rechte dürfen die Rechte der verwendeten Capability nicht überschreiten.

`address_hint` ist grundsätzlich nur ein Vorschlag. Der Kernel darf eine andere geeignete Adresse auswählen.

---

## 9. Unmapping

Ein Prozess kann ein Mapping entfernen:

```c
status_t shm_unmap(
    void *mapped_address,
    uint64_t length
);
```

Das Entfernen eines Mappings:

- entfernt die Seitentabelleneinträge,
- invalidiert erforderliche TLB-Einträge,
- reduziert die Referenzanzahl,
- verändert andere Mappings desselben Objekts nicht.

Der Speicher wird erst freigegeben, wenn keine Objekt- und Mapping-Referenzen mehr bestehen.

---

## 10. Zugriffsrechte

Eine Shared-Memory-Capability kann folgende Rechte enthalten:

| Recht | Bedeutung |
|---|---|
| `SHM_RIGHT_MAP_READ` | Lesendes Mapping erzeugen |
| `SHM_RIGHT_MAP_WRITE` | Schreibendes Mapping erzeugen |
| `SHM_RIGHT_MAP_EXECUTE` | Ausführbares Mapping erzeugen |
| `SHM_RIGHT_RESIZE` | Objektgröße verändern |
| `SHM_RIGHT_SEAL` | Objekt versiegeln |
| `SHM_RIGHT_TRANSFER` | Capability weitergeben |
| `SHM_RIGHT_INSPECT` | Metadaten abfragen |

Bei der Weitergabe einer Capability dürfen Rechte reduziert, aber niemals erweitert werden.

---

## 11. Sicherheitsregeln

Shared Memory unterliegt folgenden verbindlichen Regeln:

- Der Zugriff erfolgt ausschließlich über gültige Capabilities.
- Neu zugewiesene Seiten werden mit Null initialisiert.
- Nicht gemappte Bereiche dürfen nicht zugänglich sein.
- Schreibrechte müssen ausdrücklich vergeben werden.
- Schreibbare und gleichzeitig ausführbare Mappings sind standardmäßig verboten.
- Kerneladressräume dürfen nicht als gewöhnliches Shared Memory exportiert werden.
- Guard Pages dürfen optional vor und hinter einem Mapping angelegt werden.
- Prozessbeendigung entfernt alle Mappings des Prozesses automatisch.

Die Kombination `WRITE | EXECUTE` ist nur für besonders autorisierte Laufzeitsysteme zulässig und muss über eine separate Sicherheitsrichtlinie freigegeben werden.

---

## 12. Capability-Übergabe

Ein Shared-Memory-Objekt wird normalerweise über Message Passing an einen anderen Prozess übertragen.

Typischer Ablauf:

1. Prozess A erzeugt ein Shared-Memory-Objekt.
2. Prozess A schreibt die Nutzdaten.
3. Prozess A reduziert die Rechte für den Empfänger.
4. Prozess A sendet die Capability über einen Message Channel.
5. Prozess B mappt das Objekt in seinen Adressraum.
6. Steuer- und Statusmeldungen werden weiter über Message Passing ausgetauscht.

Der Empfänger erhält keine Informationen über die virtuelle Adresse des Absenders.

---

## 13. Größenänderung

Shared-Memory-Objekte können nur dann verändert werden, wenn:

- die Capability `SHM_RIGHT_RESIZE` besitzt,
- das Objekt nicht gegen Größenänderungen versiegelt wurde,
- die neue Größe innerhalb der Ressourcenlimits liegt.

```c
status_t shm_resize(
    cap_handle_t shm_handle,
    uint64_t new_size
);
```

Eine Verkleinerung ist nur zulässig, wenn keine aktiven Mappings den entfernten Bereich verwenden. Andernfalls schlägt die Operation mit `STATUS_BUSY` fehl.

Für die erste Implementierung werden feste Objektgrößen bevorzugt.

---

## 14. Versiegelung

Ein Shared-Memory-Objekt kann dauerhaft gegen bestimmte Änderungen versiegelt werden.

```c
#define SHM_SEAL_RESIZE       (1u << 0)
#define SHM_SEAL_WRITE        (1u << 1)
#define SHM_SEAL_EXECUTE      (1u << 2)
#define SHM_SEAL_TRANSFER     (1u << 3)
```

Beispiel:

```c
status_t shm_seal(
    cap_handle_t shm_handle,
    uint32_t seals
);
```

Versiegelungen sind dauerhaft und können nicht entfernt werden.

Ein Produzent kann dadurch Daten schreiben, das Objekt gegen weitere Änderungen versiegeln und anschließend nur lesend an einen Verbraucher übergeben.

---

## 15. Synchronisation

Shared Memory enthält keine automatische Synchronisation.

Prozesse müssen den Zugriff über geeignete Mechanismen koordinieren, beispielsweise:

- atomare Operationen
- Mutex-Objekte
- Semaphore
- Ereignisobjekte
- Futex-ähnliche Warteoperationen
- Nachrichten über einen Message Channel

Der Kernel garantiert lediglich die Sichtbarkeit entsprechend dem Speichermodell der jeweiligen CPU-Architektur.

Anwendungen müssen notwendige Memory Barriers korrekt verwenden.

---

## 16. Cache- und Speicherkonsistenz

Normales Shared Memory verwendet cachebaren Arbeitsspeicher.

Die CPU-Architektur und der Kernel müssen sicherstellen, dass:

- alle Teilnehmer konsistente physische Seiten sehen,
- Seitentabellenänderungen korrekt sichtbar werden,
- TLB-Invalidierungen vollständig ausgeführt werden,
- Speicherattribute für alle Mappings kompatibel sind.

Unterschiedliche Cacheattribute für dieselbe physische Seite sind nicht zulässig.

Geräte- und DMA-Speicher werden nicht durch diese ADR abgedeckt.

---

## 17. Ressourcenbegrenzung

Shared Memory wird auf folgenden Ebenen begrenzt:

- pro Prozess
- pro Job Object
- pro Benutzer- oder Sicherheitskontext
- systemweit

Berücksichtigt werden:

- reservierter virtueller Speicher
- tatsächlich belegte physische Seiten
- Anzahl der Shared-Memory-Objekte
- Anzahl aktiver Mappings
- Anzahl übertragener Capabilities

Das Erzeugen eines Objekts darf nicht automatisch unbegrenzt physischen Speicher reservieren.

---

## 18. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Ungültige Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_INVALID_SIZE` | Größe oder Länge ist ungültig |
| `STATUS_INVALID_OFFSET` | Offset ist ungültig oder nicht ausgerichtet |
| `STATUS_ADDRESS_CONFLICT` | Adressbereich ist bereits belegt |
| `STATUS_OUT_OF_MEMORY` | Speicher konnte nicht bereitgestellt werden |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit wurde überschritten |
| `STATUS_BUSY` | Aktive Mappings verhindern die Operation |
| `STATUS_SEALED` | Die Operation wurde durch eine Versiegelung verboten |
| `STATUS_NOT_SUPPORTED` | Angeforderte Schutzkombination wird nicht unterstützt |

---

## 19. Seiten- und Adressausrichtung

Offsets und Mapping-Längen müssen an der Systemseitengröße ausgerichtet sein.

Für die erste x86-Implementierung gilt:

```text
Basisseitengröße: 4096 Byte
```

Der Kernel darf intern größere Seiten verwenden, sofern dadurch die geforderten Schutzgrenzen nicht verletzt werden.

Teilbereiche einer Seite können nicht mit unterschiedlichen Zugriffsrechten geschützt werden.

---

## 20. Eigentümerschaft und Lebenszeit

Das Shared-Memory-Objekt bleibt bestehen, solange mindestens eine der folgenden Referenzen existiert:

- eine Capability auf das Objekt
- ein aktives Mapping
- eine interne Kernelreferenz

Das Beenden des erzeugenden Prozesses zerstört das Objekt nicht automatisch, wenn andere Prozesse weiterhin gültige Referenzen besitzen.

Der Eigentümer dient vor allem:

- der Abrechnung von Ressourcen,
- der Audit-Zuordnung,
- der Anwendung von Prozess- und Job-Limits.

---

## 21. Audit und Diagnose

Normale Lese- und Schreibzugriffe werden nicht protokolliert.

Audit-Ereignisse können erzeugt werden bei:

- verweigerten Mapping-Versuchen
- Anforderungen ausführbarer Mappings
- Versuchen, Versiegelungen zu umgehen
- Überschreitung von Ressourcenlimits
- administrativer Inspektion
- fehlerhaften Capability-Übertragungen

Speicherinhalte dürfen nicht Bestandteil gewöhnlicher Audit-Einträge sein.

Diagnoseschnittstellen dürfen Metadaten wie Größe, Eigentümer, Mapping-Anzahl und Rechte anzeigen.

---

## 22. Alternative Ansätze

### 22.1 Ausschließlich Message Passing

**Vorteile:**

- starke Isolation
- einfache Eigentumsverhältnisse
- klare Nachrichtengrenzen

**Nachteile:**

- hoher Kopieraufwand bei großen Datenmengen
- ungeeignet für Grafik-, Audio- und Videopuffer

**Entscheidung:** Für Steuerinformationen geeignet, aber nicht als alleiniger IPC-Mechanismus.

### 22.2 Global benannter Shared Memory

**Vorteile:**

- einfache Auffindbarkeit

**Nachteile:**

- globaler Namensraum
- Risiko von Namenskonflikten und Spoofing
- schwierigere Zugriffskontrolle

**Entscheidung:** Abgelehnt. Zugriff erfolgt über Capabilities.

### 22.3 Beliebige physische Seitenfreigabe

**Vorteile:**

- einfache Low-Level-Implementierung

**Nachteile:**

- hohes Risiko für Informationslecks
- Umgehung der Speicherisolation
- schwer kontrollierbare Lebenszeit

**Entscheidung:** Abgelehnt.

---

## 23. Konsequenzen

### Positive Konsequenzen

- geringe Kopierkosten bei großen Datenmengen
- gute Grundlage für Grafik- und Mediendienste
- klare Integration in das Capability-System
- flexible Rechtevergabe
- kontrollierte Zero-Copy-Kommunikation
- Prozessadressen bleiben voneinander unabhängig

### Negative Konsequenzen

- Anwendungen benötigen eigene Synchronisation
- Fehler eines Teilnehmers können gemeinsame Daten beschädigen
- komplexere Lebenszeitverwaltung
- zusätzliche Anforderungen an Seitentabellen und TLB-Verwaltung

### Risiken

- Race Conditions
- Deadlocks in der Synchronisation
- fehlerhafte Memory Barriers
- Ressourcenerschöpfung
- versehentlich zu weitreichende Schreibrechte
- Use-after-unmap-Fehler in Anwendungen

---

## 24. Implementierungsrichtlinien

- Alle Seiten müssen vor ihrer Freigabe an den Userspace bereinigt werden.
- Mapping-Rechte müssen bei jeder Operation gegen die Capability geprüft werden.
- W+X-Mappings sind standardmäßig abzulehnen.
- Größen- und Offsetberechnungen müssen gegen Überläufe geschützt sein.
- Fehlerpfade müssen reservierte Seiten und Referenzen vollständig freigeben.
- Versiegelungen müssen atomar gesetzt werden.
- Mappings dürfen keine bestehenden Guard Pages überschreiben.
- Ressourcenabrechnung muss auch bei Prozessabstürzen korrekt bleiben.

---

## 25. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Erzeugung und Zerstörung
- lesende und schreibende Mappings
- Sichtbarkeit von Änderungen zwischen Prozessen
- unterschiedliche virtuelle Mapping-Adressen
- Capability-Übertragung
- reduzierte Rechte
- verweigerte W+X-Mappings
- Größenänderung
- Versiegelungen
- Prozessbeendigung mit aktiven Mappings
- Ressourcenlimits
- konkurrierende Mapping-Operationen
- Out-of-Memory-Situationen
- ungültige Offsets und Größen
- TLB-Invalidierung auf Mehrkernsystemen

---

## 26. Verbindliche Invarianten

1. Shared Memory ist nur über eine gültige Capability zugänglich.
2. Neue Seiten sind vor der ersten Verwendung vollständig mit Null gefüllt.
3. Mapping-Rechte dürfen die Capability-Rechte nicht überschreiten.
4. Virtuelle Adressen werden niemals zwischen Prozessen übernommen.
5. Schreibbare und ausführbare Mappings sind standardmäßig verboten.
6. Versiegelungen können nicht zurückgenommen werden.
7. Ein Prozessende entfernt alle Mappings dieses Prozesses.
8. Das Objekt bleibt bestehen, solange gültige Referenzen existieren.
9. Shared Memory stellt keine automatische Synchronisation bereit.
10. Speicherinhalte werden nicht standardmäßig protokolliert.

---

## 27. Zusammenfassung

NovaOS implementiert Shared Memory als capability-geschütztes Kernelobjekt für die effiziente gemeinsame Nutzung größerer Datenmengen.

Prozesse können dasselbe Objekt mit unterschiedlichen virtuellen Adressen und Zugriffsrechten einblenden. Message Passing wird weiterhin für Steuerinformationen und die Capability-Übergabe verwendet.

Durch begrenzte Ressourcen, getrennte Rechte, Versiegelungen und das standardmäßige Verbot von W+X-Mappings bleibt Shared Memory mit dem Sicherheits- und Isolationsmodell von NovaOS vereinbar.