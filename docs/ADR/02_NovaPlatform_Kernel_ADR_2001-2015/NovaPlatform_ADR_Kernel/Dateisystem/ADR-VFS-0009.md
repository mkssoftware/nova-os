# ADR-VFS-0009 – Access Control Lists

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0009 |
| Titel | Access Control Lists |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | VFS, Dateiberechtigungen und Sicherheit |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0003, ADR-VFS-0007, ADR-KERNEL-0103, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-VFS-ACL |

---

## 1. Kontext

NovaOS benötigt feingranulare Zugriffsregeln für Dateien und Verzeichnisse.

Einfache Eigentümer-, Gruppen- und Modusbits reichen nicht für alle vorgesehenen Anwendungsfälle aus:

- mehrere Benutzer und Gruppen
- Systemdienste
- Anwendungen und Sandboxen
- gemeinsam verwendete Verzeichnisse
- administrative Delegation
- geschützte Systemdateien
- vererbte Berechtigungen
- zeitlich oder organisatorisch getrennte Zugriffsrollen

Daher wird eine native ACL-Unterstützung im VFS benötigt.

---

## 2. Problemstellung

ACLs können sehr komplex werden und widersprüchliche Regeln enthalten. Gleichzeitig verwendet NovaOS Capabilities als unmittelbaren Zugriffsnachweis.

Es muss deshalb eindeutig festgelegt werden:

- wie ACLs und Capabilities zusammenwirken,
- welche Identitäten in ACLs verwendet werden,
- wie Allow- und Deny-Regeln ausgewertet werden,
- wie Rechte vererbt werden,
- wer ACLs ändern darf,
- wie Dateisysteme ohne native ACL-Unterstützung behandelt werden,
- wie Änderungen auf bereits geöffnete Handles wirken.

---

## 3. Entscheidung

NovaOS implementiert versionierte Access Control Lists als VFS-Sicherheitsmetadaten.

ACLs entscheiden bei Pfadauflösung und Öffnung darüber, welche Rechte in eine neue File- oder Directory-Capability aufgenommen werden dürfen.

Nach erfolgreicher Öffnung ist die Capability der unmittelbare Zugriffsnachweis.

Damit gilt:

```text
Identität + ACL + Sicherheitsrichtlinie
                 |
                 v
       Capability-Erzeugung
                 |
                 v
          Dateioperation
```

ACLs ersetzen das Capability-Modell nicht, sondern steuern die Ausgabe und Erweiterbarkeit von Capabilities.

---

## 4. ACL-Struktur

Eine ACL besteht aus einem Header und einer geordneten Menge von Access Control Entries.

```c
typedef struct vfs_acl_header {
    uint32_t structure_size;
    uint16_t version;
    uint16_t flags;
    uint32_t entry_count;
} vfs_acl_header_t;
```

Ein ACL-Eintrag enthält:

```c
typedef struct vfs_acl_entry {
    uint32_t entry_type;
    uint32_t flags;
    uint64_t rights;
    security_principal_id_t principal;
} vfs_acl_entry_t;
```

Alle Strukturen sind längencodiert und versioniert.

---

## 5. Principals

ACL-Einträge können sich auf folgende Principal-Typen beziehen:

| Principal | Bedeutung |
|---|---|
| User | bestimmter Benutzer |
| Group | bestimmte Benutzergruppe |
| Service | registrierter Systemdienst |
| Application | bestimmte Anwendungsidentität |
| Security Domain | Sandbox oder Sicherheitsdomäne |
| Owner | aktueller Eigentümer des Objekts |
| Owner Group | primäre Objektgruppe |
| Everyone | alle authentifizierten Kontexte |
| System | NovaOS-Systemkern und autorisierte Kerndienste |

Principals werden durch stabile Security IDs und nicht durch Namen identifiziert.

Benutzernamen und Gruppennamen dienen nur der Darstellung.

---

## 6. ACL-Eintragstypen

NovaOS unterstützt mindestens:

```c
#define VFS_ACE_ALLOW  1
#define VFS_ACE_DENY   2
#define VFS_ACE_AUDIT  3
```

| Typ | Bedeutung |
|---|---|
| `ALLOW` | gewährt angegebene Rechte |
| `DENY` | verweigert angegebene Rechte |
| `AUDIT` | erzeugt bei zutreffender Operation ein Audit-Ereignis |

Audit-Einträge erteilen oder verweigern selbst keine Zugriffsrechte.

---

## 7. Dateirechte

ACLs können mindestens folgende Rechte steuern:

| Recht | Bedeutung |
|---|---|
| `ACL_READ_DATA` | Dateiinhalte lesen |
| `ACL_WRITE_DATA` | Dateiinhalte verändern |
| `ACL_APPEND_DATA` | Daten anhängen |
| `ACL_EXECUTE` | Datei ausführen |
| `ACL_READ_ATTRIBUTES` | Attribute und Metadaten lesen |
| `ACL_WRITE_ATTRIBUTES` | Attribute verändern |
| `ACL_READ_ACL` | ACL lesen |
| `ACL_WRITE_ACL` | ACL verändern |
| `ACL_CHANGE_OWNER` | Eigentümer ändern |
| `ACL_DELETE` | Objekt löschen |
| `ACL_SYNCHRONIZE` | Objekt in Wait-Operationen verwenden |

Verzeichnisrechte werden gesondert interpretiert.

---

## 8. Verzeichnisrechte

| Recht | Bedeutung |
|---|---|
| `ACL_LIST_DIRECTORY` | Inhalt auflisten |
| `ACL_TRAVERSE` | Verzeichnis bei Pfadauflösung durchlaufen |
| `ACL_CREATE_FILE` | Datei erzeugen |
| `ACL_CREATE_DIRECTORY` | Unterverzeichnis erzeugen |
| `ACL_DELETE_CHILD` | untergeordnetes Objekt entfernen |
| `ACL_READ_ATTRIBUTES` | Verzeichnisattribute lesen |
| `ACL_WRITE_ATTRIBUTES` | Attribute ändern |
| `ACL_READ_ACL` | ACL lesen |
| `ACL_WRITE_ACL` | ACL verändern |

`TRAVERSE` und `LIST_DIRECTORY` bleiben getrennte Rechte. Ein Prozess kann dadurch einen bekannten Pfad verwenden, ohne den gesamten Verzeichnisinhalt auflisten zu dürfen.

---

## 9. Auswertungsreihenfolge

ACLs werden in kanonischer Reihenfolge ausgewertet:

1. explizite Deny-Einträge,
2. explizite Allow-Einträge,
3. geerbte Deny-Einträge,
4. geerbte Allow-Einträge.

Für jedes angeforderte Recht gilt:

- Ein zutreffendes Deny verweigert das Recht.
- Ein zutreffendes Allow kann das Recht gewähren.
- Nicht ausdrücklich gewährte Rechte bleiben verweigert.
- Übergeordnete Systemrichtlinien können Rechte zusätzlich verweigern.
- ACLs können keine Rechte oberhalb der Systemrichtlinie erzeugen.

Nicht kanonische ACLs werden beim Setzen normalisiert oder abgelehnt.

---

## 10. Standardverweigerung

NovaOS verwendet das Prinzip „Default Deny“.

Wenn keine passende Allow-Regel vorhanden ist, wird das angeforderte Recht nicht gewährt.

Eine leere ACL bedeutet daher nicht vollständigen Zugriff.

System- und Recovery-Komponenten erhalten notwendige Zugriffe über ausdrücklich geschützte Capabilities und Systemrichtlinien, nicht über versteckte ACL-Ausnahmen.

---

## 11. Vererbung

Verzeichnisse können ACL-Einträge an neue untergeordnete Objekte vererben.

Unterstützte Flags:

```c
#define ACE_INHERIT_FILES        (1u << 0)
#define ACE_INHERIT_DIRECTORIES  (1u << 1)
#define ACE_INHERIT_ONLY         (1u << 2)
#define ACE_NO_PROPAGATE         (1u << 3)
#define ACE_INHERITED            (1u << 4)
```

Beim Erstellen eines Objekts wird eine effektive ACL aus folgenden Quellen gebildet:

- vererbbaren Einträgen des Parent-Verzeichnisses,
- expliziten Einträgen der Erzeugungsoperation,
- verbindlichen Systemrichtlinien,
- dateisystemspezifischen Standardregeln.

Die resultierende ACL wird als Metadatum des neuen Objekts gespeichert.

---

## 12. Geschützte ACLs

Ein Objekt kann seine ACL gegen weitere Vererbung schützen.

```c
#define ACL_FLAG_PROTECTED  (1u << 0)
```

Bei einer geschützten ACL werden spätere Änderungen der Parent-ACL nicht automatisch übernommen.

NovaOS verwendet grundsätzlich Vererbung beim Erstellen. Eine dynamische rückwirkende Änderung aller vorhandenen Child-Objekte erfolgt nur über eine ausdrückliche administrative Operation.

---

## 13. Eigentümer

Jedes VFS-Objekt besitzt einen Eigentümer-Principal.

Der Eigentümer erhält nicht automatisch uneingeschränkten Zugriff. Er kann jedoch standardmäßig das Recht zum Lesen oder Verwalten der ACL erhalten, sofern keine höhere Sicherheitsrichtlinie dies untersagt.

Das Ändern des Eigentümers benötigt:

- `ACL_CHANGE_OWNER` oder
- eine besondere administrative Capability.

Ein Prozess darf ein Objekt nicht ohne Autorisierung an einen beliebigen Principal übertragen.

---

## 14. ACL lesen und ändern

```c
status_t vfs_acl_get(
    cap_handle_t object_handle,
    void *acl_buffer,
    uint32_t *buffer_size
);

status_t vfs_acl_set(
    cap_handle_t object_handle,
    const void *acl_buffer,
    uint32_t buffer_size,
    uint32_t flags
);
```

Zum Lesen wird `ACL_READ_ACL` benötigt.

Zum Ändern wird `ACL_WRITE_ACL` benötigt.

Vor dem Speichern prüft das VFS:

- Strukturversion
- Eintragszahl
- Principal-IDs
- Rechtebits
- Vererbungsflags
- kanonische Reihenfolge
- Größen- und Integerüberläufe
- Schutz der Systemobjekte

---

## 15. ACL und File Handles

ACLs werden beim Erzeugen einer File-Handle-Capability ausgewertet.

Eine spätere ACL-Änderung reduziert bestehende Handle-Rechte standardmäßig nicht automatisch.

Dies ist notwendig für:

- stabile laufende Operationen,
- eindeutige Capability-Semantik,
- zuverlässige Ressourcenübergabe.

Für dringende Sicherheitsfälle kann ein gesonderter Widerrufsmechanismus vorhandene Capabilities invalidieren.

Eine ACL-Änderung allein ist kein Capability-Widerruf.

---

## 16. Capability-Übertragung

Wird ein File Handle an einen anderen Prozess übertragen, werden nicht automatisch die ACLs gegen die Identität des Empfängers neu ausgewertet.

Die Übertragung ist eine ausdrückliche Delegation durch den Besitzer der Capability.

Dabei gelten folgende Regeln:

- Die Capability benötigt ein Übertragungsrecht.
- Rechte dürfen nur reduziert werden.
- Systemrichtlinien können die Delegation verbieten.
- Geschützte Dateien können nicht delegierbare Capabilities verwenden.
- Sicherheitskritische Übertragungen können auditiert werden.

---

## 17. Löschen und Umbenennen

Das Löschen eines Objekts wird durch Rechte auf dem Objekt und dem Parent-Verzeichnis bestimmt.

NovaOS berücksichtigt:

- `ACL_DELETE` am Objekt,
- `ACL_DELETE_CHILD` am Parent,
- geschützte Systemrichtlinien,
- schreibgeschützte Mounts,
- aktive Unveränderlichkeits- oder Aufbewahrungsflags.

Umbenennen benötigt geeignete Rechte im Quell- und Zielverzeichnis.

Ein ACL-Allow kann keinen Read-only-Mount überschreiben.

---

## 18. Kompatibilität mit einfachen Modusbits

Eine POSIX-Kompatibilitätsschicht kann Owner-, Group- und Other-Modusbits auf eine vereinfachte ACL abbilden.

Beispiel:

```text
Owner  -> expliziter User-Eintrag
Group  -> expliziter Group-Eintrag
Other  -> Everyone-Eintrag
```

Enthält eine ACL zusätzliche Einträge, kann sie nicht immer verlustfrei auf klassische Modusbits reduziert werden.

Kompatibilitätswerkzeuge müssen diesen Verlust sichtbar machen und dürfen komplexe ACLs nicht stillschweigend zerstören.

---

## 19. Dateisystemunterstützung

Native NovaOS-Dateisysteme speichern ACLs als versionierte Sicherheitsmetadaten.

Dateisysteme ohne native ACL-Unterstützung können:

- ACLs über eine sichere Erweiterungsstruktur emulieren,
- nur eine vereinfachte ACL unterstützen,
- beim Setzen komplexer ACLs `STATUS_NOT_SUPPORTED` liefern,
- mit restriktiven Standardrechten gemountet werden.

ACLs dürfen nicht in unsicheren, frei manipulierbaren Begleitdateien gespeichert werden.

---

## 20. Caching

Effektive ACL-Ergebnisse dürfen zwischengespeichert werden.

Der Cache-Schlüssel berücksichtigt mindestens:

- VNode-ID
- Dateisystemgeneration
- ACL-Version
- Principal und Gruppenmitgliedschaften
- Systemrichtlinienversion
- Mount-Sicherheitsflags

Ändert sich einer dieser Werte, muss der Cacheeintrag invalidiert werden.

Ein ACL-Cache darf keine veralteten Rechte gewähren.

---

## 21. Audit

ACL-Audit-Einträge können erfolgreiche oder verweigerte Zugriffe erfassen.

Mögliche Audit-Flags:

```c
#define ACE_AUDIT_SUCCESS  (1u << 0)
#define ACE_AUDIT_FAILURE  (1u << 1)
```

Auditiert werden außerdem:

- Änderungen geschützter ACLs,
- Eigentümerwechsel,
- wiederholte Zugriffsverletzungen,
- Delegation privilegierter Handles,
- Änderungen an System- und Recovery-Dateien.

Audit-Regeln müssen begrenzt werden, damit gewöhnliche Dateizugriffe keine unkontrollierte Ereignisflut erzeugen.

Dateiinhalte werden nicht protokolliert.

---

## 22. Sicherheit

Für ACLs gelten folgende Regeln:

- Principals werden über stabile Security IDs identifiziert.
- Namen sind keine Sicherheitsidentität.
- Nicht gewährte Rechte bleiben verweigert.
- Deny-Einträge haben innerhalb ihrer Kategorie Vorrang.
- ACLs können Mount- und Systemrichtlinien nicht abschwächen.
- ACL-Änderungen benötigen eigene Rechte.
- Capability-Übertragung ist eine ausdrückliche Delegation.
- Bestehende Capabilities werden nur über einen gesonderten Widerruf entzogen.
- Ungültige ACLs werden nicht teilweise angewendet.

---

## 23. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_INVALID_ACL` | ACL-Struktur ist ungültig |
| `STATUS_INVALID_PRINCIPAL` | Principal ist unbekannt oder unzulässig |
| `STATUS_ACL_TOO_LARGE` | ACL überschreitet den Grenzwert |
| `STATUS_NOT_SUPPORTED` | Dateisystem unterstützt die ACL nicht |
| `STATUS_READ_ONLY` | ACL kann nicht verändert werden |
| `STATUS_POLICY_DENIED` | höhere Sicherheitsrichtlinie verweigert die Änderung |
| `STATUS_INHERITANCE_CONFLICT` | Vererbungsregeln sind widersprüchlich |
| `STATUS_REVOKE_REQUIRED` | vorhandene Capabilities erfordern gesonderten Widerruf |

---

## 24. Alternative Ansätze

### 24.1 Nur klassische Unix-Modusbits

**Vorteile:**

- einfache und bekannte Semantik
- geringer Speicherbedarf

**Nachteile:**

- unzureichend für Anwendungen, Dienste und mehrere Gruppen
- schwache Delegationsmöglichkeiten

**Entscheidung:** Nur als Kompatibilitätsansicht unterstützt.

### 24.2 ACL-Prüfung bei jeder einzelnen I/O-Operation

**Vorteile:**

- ACL-Änderungen wirken sofort

**Nachteile:**

- hoher Laufzeitaufwand
- widerspricht stabilen Capability-Rechten
- erschwert Delegation und asynchrone I/O

**Entscheidung:** ACLs werden primär beim Öffnen und bei Namensoperationen geprüft.

### 24.3 Ausschließlich Capabilities ohne persistente ACLs

**Vorteile:**

- klares Laufzeitmodell
- geringe ACL-Komplexität

**Nachteile:**

- keine persistente benutzerbezogene Zugriffsregel
- schwierige Rechtevergabe nach Neustart

**Entscheidung:** ACLs steuern die initiale Capability-Ausgabe; Capabilities autorisieren den laufenden Zugriff.

---

## 25. Konsequenzen

### Positive Konsequenzen

- feingranulare Dateiberechtigungen
- Unterstützung für Benutzer, Gruppen, Dienste und Anwendungen
- persistente Regeln zur Capability-Ausgabe
- kontrollierte Vererbung
- getrennte Rechte für Traversierung und Auflistung
- Kompatibilität mit einfachen POSIX-Rechten
- Auditierbarkeit kritischer Zugriffe

### Negative Konsequenzen

- komplexere Rechteauswertung
- Vererbung kann schwer nachvollziehbare Ergebnisse erzeugen
- bestehende Handles müssen gesondert widerrufen werden
- nicht alle Dateisysteme können ACLs vollständig speichern

### Risiken

- falsch geordnete Allow- und Deny-Einträge
- veraltete ACL-Caches
- unbeabsichtigte Handle-Delegation
- übermäßig große ACLs
- Rechteverlust bei Export in einfachere Dateisysteme
- Audit-Ereignisüberflutung

---

## 26. Implementierungsrichtlinien

- ACLs verwenden stabile Security IDs statt Namen.
- Alle ACLs werden vor Anwendung vollständig validiert.
- Nicht kanonische ACLs werden normalisiert oder abgelehnt.
- Das VFS verwendet Default Deny.
- Mount- und Systemrichtlinien haben Vorrang vor ACL-Allow-Einträgen.
- ACL-Änderungen und Capability-Widerruf bleiben getrennte Operationen.
- ACL-Caches berücksichtigen Identitäts-, Gruppen- und Richtlinienversionen.
- Vererbte Einträge werden eindeutig als solche markiert.
- Komplexe ACLs dürfen bei POSIX-Konvertierung nicht stillschweigend verloren gehen.

---

## 27. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Allow- und Deny-Einträge
- kanonische Auswertungsreihenfolge
- Default Deny
- Benutzer-, Gruppen-, Dienst- und Anwendungs-Principals
- Datei- und Verzeichnisrechte
- Vererbung und geschützte ACLs
- Eigentümerwechsel
- ACL-Änderung bei bestehenden Handles
- Capability-Übertragung
- Mount- und Systemrichtlinien
- ACL-Cacheinvalidierung
- POSIX-Modusbit-Abbildung
- Dateisysteme ohne ACL-Unterstützung
- Audit-Flags
- ungültige und übergroße ACLs

---

## 28. Verbindliche Invarianten

1. ACLs steuern die Erzeugung und Rechteausstattung neuer VFS-Capabilities.
2. Capabilities bleiben der unmittelbare Zugriffsnachweis.
3. Principals werden durch stabile Security IDs identifiziert.
4. Nicht ausdrücklich gewährte Rechte bleiben verweigert.
5. ACL-Allow-Einträge können Mount- oder Systemrichtlinien nicht überschreiben.
6. Vererbte und explizite Einträge bleiben unterscheidbar.
7. ACL-Änderungen widerrufen bestehende Handles nicht automatisch.
8. Capability-Übertragung ist eine ausdrückliche Delegation.
9. Ungültige ACLs werden vollständig abgelehnt und nie teilweise angewendet.
10. Dateiinhalte werden durch ACL-Audit nicht protokolliert.

---

## 29. Zusammenfassung

NovaOS verwendet versionierte Access Control Lists für persistente und feingranulare Dateiberechtigungen.

ACLs werden bei Pfadauflösung, Objekterzeugung und Öffnung ausgewertet und bestimmen, welche Rechte eine neue Capability erhalten darf. Der anschließende Zugriff erfolgt weiterhin über diese Capability.

Stabile Security IDs, Default Deny, kontrollierte Vererbung und der Vorrang von Mount- und Systemrichtlinien verbinden persistente Dateiberechtigungen mit dem nativen Capability-Modell von NovaOS.