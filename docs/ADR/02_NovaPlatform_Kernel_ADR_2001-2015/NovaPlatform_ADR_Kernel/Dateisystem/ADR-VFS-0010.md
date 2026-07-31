# ADR-VFS-0010 – Dateiattribute

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0010 |
| Titel | Dateiattribute |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Virtual File System und Dateimetadaten |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0004, ADR-VFS-0007, ADR-VFS-0009, ADR-KERNEL-0105 |
| Nachfolgende Spezifikation | NPSPEC-VFS-FILE-ATTRIBUTES |

---

## 1. Kontext

Dateisystemobjekte benötigen neben Name und Inhalt zusätzliche Metadaten.

Dazu gehören:

- Dateityp
- Größe
- Zeitstempel
- Eigentümer
- Schutz- und Sichtbarkeitsmerkmale
- Archivierungsstatus
- Komprimierung und Verschlüsselung
- benutzerdefinierte Metadaten
- dateisystemspezifische Eigenschaften

NovaOS benötigt dafür ein einheitliches, versionierbares Attributmodell im VFS.

---

## 2. Problemstellung

Dateisysteme unterstützen unterschiedliche Mengen und Arten von Attributen. Eine vollständig starre Struktur würde künftige Erweiterungen erschweren, während unkontrollierte Schlüssel-Wert-Metadaten zu Namenskonflikten und Sicherheitsproblemen führen können.

Das VFS muss deshalb festlegen:

- welche Attribute systemweit standardisiert werden,
- welche Attribute veränderbar sind,
- wie Erweiterungen benannt werden,
- wie Rechte geprüft werden,
- wie Attribute atomar gelesen und geändert werden,
- wie nicht unterstützte Attribute behandelt werden,
- welche Metadaten sicherheitsrelevant sind.

---

## 3. Entscheidung

NovaOS verwendet ein zweistufiges Attributmodell:

1. standardisierte VFS-Kernattribute,
2. versionierte Extended Attributes in kontrollierten Namespaces.

Kernattribute besitzen eine feste Semantik im gesamten VFS. Extended Attributes erlauben Erweiterungen durch Dateisysteme, Anwendungen und Systemdienste.

Nicht jedes Dateisystem muss jedes optionale Attribut unterstützen. Unterstützte Fähigkeiten müssen abfragbar sein.

---

## 4. Attributklassen

| Klasse | Inhalt |
|---|---|
| Identität | Objekt-ID, Dateisystem-ID, Generation |
| Typ | Datei, Verzeichnis, symbolischer Link oder Spezialobjekt |
| Größe | logische und physisch belegte Größe |
| Zeit | Erstellung, Änderung, Zugriff und Metadatenänderung |
| Eigentum | Eigentümer und Gruppe |
| Schutz | Read-only, Immutable, Append-only |
| Darstellung | Hidden, System, Archive |
| Speicherung | Sparse, Compressed, Encrypted |
| Integrität | Prüfsummen- und Verifikationsstatus |
| Erweiterung | benutzer- oder systemspezifische Extended Attributes |

---

## 5. Kernattributstruktur

```c
typedef struct vfs_file_attributes {
    uint32_t structure_size;
    uint32_t version;

    uint64_t object_id;
    uint64_t filesystem_id;
    uint64_t generation;

    uint32_t object_type;
    uint32_t attribute_flags;

    uint64_t logical_size;
    uint64_t allocated_size;

    vfs_timestamp_t creation_time;
    vfs_timestamp_t modification_time;
    vfs_timestamp_t access_time;
    vfs_timestamp_t metadata_change_time;

    security_principal_id_t owner;
    security_principal_id_t owner_group;
} vfs_file_attributes_t;
```

`structure_size` und `version` ermöglichen kompatible Erweiterungen.

---

## 6. Objektidentität

Die Kombination aus folgenden Werten identifiziert ein Objekt innerhalb einer Dateisystemgeneration:

- Dateisystem-ID
- Objekt-ID
- Generation

Objekt-IDs dürfen nach einer Löschung nicht ohne geänderte Generation für sicherheitsrelevante Referenzen wiederverwendet werden.

Objektidentitäten dienen Diagnose und Cacheverwaltung. Sie stellen keine Zugriffsberechtigung dar.

---

## 7. Objektarten

NovaOS definiert mindestens:

```c
#define VFS_TYPE_REGULAR_FILE  1
#define VFS_TYPE_DIRECTORY     2
#define VFS_TYPE_SYMLINK       3
#define VFS_TYPE_DEVICE_VIEW   4
#define VFS_TYPE_SOCKET_VIEW   5
#define VFS_TYPE_VIRTUAL       6
```

Der Objekttyp wird durch das Dateisystem bestimmt und darf nicht durch eine gewöhnliche Attributänderung gewechselt werden.

Unbekannte zukünftige Typen müssen über Versions- und Capability-Abfragen erkannt werden.

---

## 8. Standardflags

Das VFS standardisiert folgende Attribute:

| Flag | Bedeutung |
|---|---|
| `VFS_ATTR_READ_ONLY` | Objekt darf nicht regulär verändert werden |
| `VFS_ATTR_HIDDEN` | Standardoberflächen blenden das Objekt aus |
| `VFS_ATTR_SYSTEM` | Objekt gehört zu einer Systemkomponente |
| `VFS_ATTR_ARCHIVE` | Objekt wurde seit der letzten Archivierung verändert |
| `VFS_ATTR_TEMPORARY` | Objekt ist für temporäre Nutzung vorgesehen |
| `VFS_ATTR_IMMUTABLE` | Inhalt, Name und kritische Metadaten sind geschützt |
| `VFS_ATTR_APPEND_ONLY` | Daten dürfen ausschließlich angehängt werden |
| `VFS_ATTR_SPARSE` | Datei kann nicht belegte Bereiche besitzen |
| `VFS_ATTR_COMPRESSED` | Daten werden komprimiert gespeichert |
| `VFS_ATTR_ENCRYPTED` | Daten werden verschlüsselt gespeichert |
| `VFS_ATTR_INTEGRITY` | Integritätsprüfung ist aktiviert |

Darstellungsattribute wie `HIDDEN` sind keine Sicherheitsgrenze.

---

## 9. Read-only, Immutable und Append-only

Die Attribute besitzen unterschiedliche Semantik:

| Attribut | Wirkung |
|---|---|
| Read-only | gewöhnliche Schreibvorgänge werden verweigert |
| Immutable | Inhalt, Löschung, Umbenennung und kritische Metadatenänderungen werden blockiert |
| Append-only | vorhandene Daten bleiben unverändert, neue Daten dürfen angehängt werden |

`IMMUTABLE` und `APPEND_ONLY` sind sicherheitsrelevante Attribute und benötigen besondere Rechte.

Eine ACL-Erlaubnis allein darf ein aktives Immutable-Attribut nicht umgehen.

---

## 10. Zeitstempel

NovaOS unterstützt mindestens:

| Zeitstempel | Bedeutung |
|---|---|
| Creation Time | Zeitpunkt der Objekterzeugung |
| Modification Time | letzte Inhaltsänderung |
| Access Time | letzter relevanter Lesezugriff |
| Metadata Change Time | letzte Metadatenänderung |

Zeitstempel werden in einer plattformunabhängigen UTC-Darstellung mit Nanosekundenfeld gespeichert.

Die tatsächliche Auflösung hängt vom Dateisystem ab und muss abfragbar sein.

---

## 11. Access-Time-Richtlinie

Das Aktualisieren der Access Time bei jedem Lesezugriff verursacht unnötige Schreiboperationen.

NovaOS unterstützt daher:

- `NO_ATIME`
- `RELATIVE_ATIME`
- `STRICT_ATIME`
- `LAZY_ATIME`

`RELATIVE_ATIME` ist der Standard. Dabei wird Access Time nur aktualisiert, wenn sie älter als Modification Time oder ein definiertes Zeitfenster ist.

Anwendungen dürfen sich nicht auf eine Aktualisierung bei jedem einzelnen Lesezugriff verlassen.

---

## 12. Dateigrößen

Das VFS unterscheidet:

| Größe | Bedeutung |
|---|---|
| Logical Size | für Anwendungen sichtbare Dateilänge |
| Allocated Size | tatsächlich belegter Speicher |
| Compressed Size | optional belegter komprimierter Speicher |
| Exclusive Size | optional nur diesem Objekt zugeordneter Speicher |

Bei Sparse Files kann die Allocated Size kleiner als die Logical Size sein.

Bei Copy-on-Write oder Deduplizierung kann physischer Speicher mehreren Objekten zugeordnet sein. Größenangaben müssen deshalb klar benennen, was sie messen.

---

## 13. Extended Attributes

Extended Attributes verwenden typisierte Namensräume.

Beispiele:

```text
user.comment
user.tags
system.mime-type
security.classification
security.integrity-policy
application.nova-editor.state
filesystem.novafs.protection-level
```

Ein Extended Attribute enthält:

- Namespace
- Namen
- Typ
- Datenlänge
- Wert
- optionale Version

Namen und Werte unterliegen festen Größenlimits.

---

## 14. Attribut-Namespaces

| Namespace | Verwendung |
|---|---|
| `user.*` | benutzerdefinierte Metadaten |
| `application.*` | anwendungsspezifische Informationen |
| `system.*` | vom Betriebssystem verwaltete Metadaten |
| `security.*` | sicherheitskritische Richtlinien |
| `filesystem.*` | dateisystemspezifische Eigenschaften |
| `compat.*` | Kompatibilitätsmetadaten anderer Systeme |

Gewöhnliche Anwendungen dürfen nur autorisierte Namespaces verändern.

`security.*` und geschützte `system.*`-Attribute benötigen besondere Capabilities.

---

## 15. Kernattribute lesen

```c
status_t vfs_get_attributes(
    cap_handle_t object_handle,
    vfs_file_attributes_t *attributes
);
```

Der Aufrufer benötigt `FILE_RIGHT_READ_METADATA`.

Bestimmte sensible Informationen können zusätzlich gefiltert werden, beispielsweise:

- dauerhafte Hardware- oder Dateisystemkennungen,
- Sicherheitsklassifikation,
- interne Speicherorte,
- nicht öffentliche Integritätsdaten.

---

## 16. Kernattribute ändern

```c
status_t vfs_set_attributes(
    cap_handle_t object_handle,
    const vfs_file_attributes_update_t *update
);
```

Die Update-Struktur enthält eine Änderungsmaske. Dadurch wird eindeutig festgelegt, welche Felder verändert werden sollen.

Nicht veränderbare Felder wie Objekt-ID und Dateisystem-ID werden immer abgelehnt.

Die gesamte Attributänderung erfolgt atomar oder schlägt vollständig fehl.

---

## 17. Extended-Attribute-API

```c
status_t vfs_xattr_get(
    cap_handle_t object_handle,
    const char *name,
    void *value,
    uint32_t *value_size
);

status_t vfs_xattr_set(
    cap_handle_t object_handle,
    const char *name,
    const void *value,
    uint32_t value_size,
    uint32_t flags
);

status_t vfs_xattr_remove(
    cap_handle_t object_handle,
    const char *name
);
```

Mögliche Flags:

```c
#define VFS_XATTR_CREATE_ONLY   (1u << 0)
#define VFS_XATTR_REPLACE_ONLY  (1u << 1)
```

Dadurch können Race Conditions zwischen Prüfung und Änderung vermieden werden.

---

## 18. Auflistung von Extended Attributes

```c
status_t vfs_xattr_list(
    cap_handle_t object_handle,
    uint64_t cursor,
    vfs_xattr_entry_t *entries,
    uint32_t capacity,
    uint32_t *entry_count,
    uint64_t *next_cursor
);
```

Die Auflistung zeigt nur Attribute, für die der Aufrufer Sichtbarkeitsrechte besitzt.

Ein nicht sichtbares Sicherheitsattribut darf nicht allein durch seinen Namen offengelegt werden.

---

## 19. Rechteprüfung

Für Attribute gelten mindestens folgende Rechte:

| Recht | Bedeutung |
|---|---|
| `FILE_RIGHT_READ_METADATA` | gewöhnliche Kernattribute lesen |
| `FILE_RIGHT_WRITE_METADATA` | veränderbare Kernattribute ändern |
| `FILE_RIGHT_READ_XATTR` | Extended Attributes lesen |
| `FILE_RIGHT_WRITE_XATTR` | erlaubte Extended Attributes ändern |
| `FILE_RIGHT_SET_SECURITY` | Sicherheitsattribute ändern |
| `FILE_RIGHT_SET_IMMUTABLE` | Immutable oder Append-only verändern |
| `FILE_RIGHT_CHANGE_OWNER` | Eigentümer oder Gruppe ändern |

Mount-, ACL- und Systemrichtlinien können Attributänderungen zusätzlich einschränken.

---

## 20. Cache und Konsistenz

Dateiattribute dürfen im VNode- und Metadatencache gespeichert werden.

Cache-Einträge werden invalidiert bei:

- erfolgreicher Attributänderung
- Dateiänderung
- Größenänderung
- ACL-Änderung
- Snapshot- oder Generationswechsel
- extern erkannten Änderungen
- Unmount oder Medienwechsel

Jede Änderung erhöht eine Metadaten- oder Objektgeneration, sofern das Dateisystem dies unterstützt.

Veraltete Sicherheitsattribute dürfen nicht aus einem Cache übernommen werden.

---

## 21. Dateisystemunterstützung

Nicht jedes Dateisystem unterstützt alle Attribute.

Der Dateisystemtreiber veröffentlicht deshalb Featureinformationen für:

- Zeitstempelauflösung
- Extended Attributes
- Sparse Files
- Komprimierung
- Verschlüsselung
- Immutable und Append-only
- Creation Time
- physische Größenangaben

Das VFS darf nicht unterstützte persistente Attribute nicht nur im flüchtigen RAM simulieren, sofern die Anwendung dauerhafte Speicherung erwartet.

---

## 22. Kopieren und Verschieben

Beim Kopieren eines Objekts werden Attribute nach einer kontrollierten Richtlinie übertragen.

Standardmäßig:

- grundlegende Zeit- und Darstellungsattribute können übernommen werden,
- ACLs werden nach Zielrichtlinie neu bewertet,
- `security.*`-Attribute benötigen besondere Berechtigung,
- dateisystemspezifische Attribute werden nur bei Kompatibilität übertragen,
- `IMMUTABLE` wird erst nach Abschluss des Kopiervorgangs gesetzt,
- Eigentümer wird nicht ohne Berechtigung übernommen.

Ein Rename innerhalb desselben Dateisystems erhält die Attribute unverändert.

---

## 23. Sicherheit und Datenschutz

Für Dateiattribute gelten folgende Regeln:

- Attribute werden ausschließlich über File- oder Directory-Capabilities gelesen.
- Darstellungsattribute sind keine Sicherheitsmechanismen.
- Sicherheitsattribute besitzen getrennte Zugriffsrechte.
- Nicht unterstützte Attribute dürfen nicht stillschweigend als gespeichert gemeldet werden.
- Attributwerte gelten beim Einlesen aus Dateisystemen als nicht vertrauenswürdig.
- Größen, Typen und Kodierungen werden vollständig validiert.
- Extended Attributes dürfen keine direkt ausführbaren Kernelobjekte enthalten.
- Sensible Attribute werden nicht unautorisiert aufgelistet.

---

## 24. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_ATTRIBUTE_NOT_FOUND` | Attribut existiert nicht |
| `STATUS_ATTRIBUTE_EXISTS` | Attribut existiert bereits |
| `STATUS_INVALID_ATTRIBUTE` | Name, Typ oder Wert ist ungültig |
| `STATUS_ATTRIBUTE_TOO_LARGE` | Wert überschreitet den Grenzwert |
| `STATUS_NOT_SUPPORTED` | Dateisystem unterstützt das Attribut nicht |
| `STATUS_READ_ONLY` | Objekt oder Dateisystem ist schreibgeschützt |
| `STATUS_IMMUTABLE` | Immutable-Attribut verhindert die Änderung |
| `STATUS_POLICY_DENIED` | Sicherheitsrichtlinie verbietet die Operation |
| `STATUS_GENERATION_CHANGED` | Metadaten wurden gleichzeitig verändert |

---

## 25. Audit und Diagnose

Auditiert werden insbesondere:

- Änderungen von `security.*`-Attributen,
- Aktivierung oder Entfernung von `IMMUTABLE`,
- Eigentümerwechsel,
- Änderungen an System- und Recovery-Dateien,
- fehlgeschlagene Integritätsprüfungen,
- administrative Änderung geschützter Attribute.

Gewöhnliche benutzerdefinierte Attribute werden nicht standardmäßig auditiert.

Diagnosewerkzeuge dürfen unterstützte Attribute, Generationen und Dateisystemfähigkeiten anzeigen.

---

## 26. Alternative Ansätze

### 26.1 Nur eine feste Attributstruktur

**Vorteile:**

- einfache API
- eindeutige Typen

**Nachteile:**

- schlecht erweiterbar
- keine dateisystem- oder anwendungsspezifischen Metadaten

**Entscheidung:** Kernattribute bleiben fest, Extended Attributes ergänzen sie.

### 26.2 Alle Metadaten als freie Schlüssel-Wert-Paare

**Vorteile:**

- maximale Flexibilität

**Nachteile:**

- keine einheitliche Semantik
- Namenskonflikte
- schwer kontrollierbare Sicherheitsattribute

**Entscheidung:** Abgelehnt.

### 26.3 Hidden als Sicherheitsmerkmal

**Vorteile:**

- einfache Benutzeroberflächenlogik

**Nachteile:**

- leicht umgehbar
- verwechselt Darstellung mit Zugriffsschutz

**Entscheidung:** Abgelehnt. Zugriffsschutz erfolgt über ACLs und Capabilities.

---

## 27. Konsequenzen

### Positive Konsequenzen

- einheitliche Kernmetadaten
- erweiterbare Extended Attributes
- klare Attribut-Namespaces
- Unterstützung für Komprimierung, Verschlüsselung und Integrität
- atomare Attributänderungen
- kontrollierte Sicherheitsattribute
- bessere Kompatibilität mit unterschiedlichen Dateisystemen

### Negative Konsequenzen

- unterschiedliche Dateisysteme unterstützen unterschiedliche Teilmengen
- Kopieren zwischen Dateisystemen kann Attribute verlieren
- Metadatencaches benötigen zuverlässige Invalidierung
- Extended Attributes erhöhen Parser- und Speicheraufwand

### Risiken

- Missbrauch von Extended Attributes für große Datenmengen
- veraltete Sicherheitsattribute im Cache
- unbemerkter Attributverlust beim Kopieren
- Namenskonflikte
- inkonsistente Zeitstempel auf Dateisystemen mit geringer Auflösung

---

## 28. Implementierungsrichtlinien

- Kernattribute besitzen VFS-weit feste Semantik.
- Erweiterungen verwenden registrierte Attribut-Namespaces.
- Sicherheitsattribute benötigen getrennte Rechte.
- Attributänderungen verwenden eine explizite Änderungsmaske.
- Objekt-ID, Dateisystem-ID und Objekttyp sind nicht regulär veränderbar.
- Nicht unterstützte Attribute liefern einen eindeutigen Fehler.
- Darstellungsattribute dürfen nicht als Zugriffsschutz verwendet werden.
- Metadatencaches werden bei Änderungen zuverlässig invalidiert.
- Extended-Attribute-Größe und -Anzahl werden begrenzt.
- Attributänderungen erfolgen atomar oder gar nicht.

---

## 29. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Lesen und Ändern von Kernattributen,
- Zeitstempel und unterschiedliche Auflösungen,
- Read-only, Immutable und Append-only,
- Sparse-, Compressed- und Encrypted-Flags,
- Extended-Attribute-Namespaces,
- Create-only und Replace-only,
- Attributauflistung und Sichtbarkeit,
- Rechteprüfungen,
- Cacheinvalidierung,
- parallele Attributänderungen,
- Kopieren zwischen verschiedenen Dateisystemen,
- nicht unterstützte Attribute,
- übergroße und ungültige Werte,
- Medienwechsel und Unmount.

---

## 30. Verbindliche Invarianten

1. NovaOS unterscheidet standardisierte Kernattribute und Extended Attributes.
2. Kernattribute besitzen im gesamten VFS eine feste Semantik.
3. Extended Attributes verwenden kontrollierte Namensräume.
4. Dateiattribute werden ausschließlich über gültige Capabilities angesprochen.
5. Darstellungsattribute wie `HIDDEN` erteilen oder verweigern keine Zugriffsrechte.
6. Sicherheitsattribute benötigen besondere Rechte.
7. Nicht unterstützte Attribute dürfen nicht als erfolgreich gespeichert gemeldet werden.
8. Attributänderungen erfolgen atomar.
9. Objektidentität und Objekttyp sind nicht regulär veränderbar.
10. Sensible Attribute werden nicht unautorisiert aufgelistet oder protokolliert.

---

## 31. Zusammenfassung

NovaOS verwendet ein zweistufiges Modell aus standardisierten VFS-Kernattributen und versionierten Extended Attributes.

Kernattribute beschreiben Identität, Typ, Größe, Zeit, Eigentum und zentrale Schutzmerkmale. Extended Attributes ermöglichen kontrollierte Erweiterungen für Benutzer, Anwendungen, Systemdienste, Sicherheit und Dateisysteme.

Klare Namespaces, capability-basierte Rechte und atomare Änderungen verhindern, dass flexible Metadaten die Sicherheits- und Konsistenzregeln des VFS umgehen.