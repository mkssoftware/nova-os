# ADR-VFS-0008 – Symbolische Links

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0008 |
| Titel | Symbolische Links |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Virtual File System und Pfadauflösung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0002, ADR-VFS-0003, ADR-VFS-0007, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-VFS-SYMBOLIC-LINKS |

---

## 1. Kontext

Symbolische Links ermöglichen es, einen Dateisystemeintrag auf einen anderen Pfad verweisen zu lassen.

Sie werden unter anderem benötigt für:

- alternative Verzeichnisstrukturen
- Kompatibilitätsumgebungen
- Versionsumschaltungen
- Benutzerverknüpfungen
- Paketverwaltung
- Weiterleitung auf gemeinsam genutzte Daten
- atomare Aktivierung vorbereiteter Systempfade

Im Gegensatz zu Hard Links speichern symbolische Links keine direkte Objektidentität, sondern einen Pfadtext.

---

## 2. Problemstellung

Symbolische Links beeinflussen die Pfadauflösung und können Sicherheitsgrenzen umgehen, wenn sie unkontrolliert verfolgt werden.

Mögliche Risiken sind:

- Endlosschleifen
- Verlassen eines Namespace-Roots
- Time-of-check-to-time-of-use-Rennen
- Umleitung auf unerwartete Mounts
- Zugriff auf privilegierte Dateien
- Austausch eines Links während einer Sicherheitsprüfung
- unterschiedliche Semantik zwischen Dateisystemen

NovaOS benötigt deshalb verbindliche, VFS-weit einheitliche Regeln.

---

## 3. Entscheidung

NovaOS unterstützt symbolische Links als nativen VFS-Objekttyp.

Die Auflösung erfolgt ausschließlich durch das VFS und unterliegt:

- dem aktiven Mount Namespace,
- dem Capability-Root der Operation,
- einer festen Auflösungstiefe,
- Mount- und Zugriffsrechten,
- expliziten Follow- oder No-Follow-Regeln,
- atomarer Verbindung zwischen Auflösung und Öffnung.

Ein symbolischer Link erteilt keine zusätzlichen Zugriffsrechte.

---

## 4. Linkinhalt

Ein symbolischer Link enthält einen UTF-8-kodierten Pfadtext.

Beispiele:

```text
../shared/config
/system/current/bin/tool
documents/latest
```

Der Linkinhalt ist kein VFS-Handle und keine direkte VNode-Referenz.

Das Ziel darf beim Erstellen des Links nicht existieren. Dadurch sind bewusst auch „dangling symlinks“ möglich.

---

## 5. Relative und absolute Links

### 5.1 Relativer Link

Ein relativer Link wird ausgehend vom Verzeichnis aufgelöst, in dem der Link gespeichert ist.

Beispiel:

```text
Link: /apps/editor/config
Ziel: ../shared/editor-config
```

### 5.2 Absoluter Link

Ein absoluter Link beginnt mit `/` und wird am Root des aktuellen VFS Namespace beziehungsweise Capability-Roots aufgelöst.

Er bezieht sich niemals auf einen globalen Host-Root außerhalb der aktuellen Sicherheitsgrenze.

---

## 6. Namespace-Grenzen

Symbolische Links dürfen den aktiven Namespace-Root nicht verlassen.

Das gilt auch für Links mit:

- absoluten Pfaden,
- mehreren `..`-Komponenten,
- verschachtelten Links,
- Mountübergängen,
- Zielpfaden in übergeordneten Namespaces.

Erfolgt eine Operation relativ zu einer eingeschränkten Directory-Capability, kann diese zusätzlich als Capability-Root verwendet werden.

Ein Link kann dann nur Ziele innerhalb dieses Roots erreichen.

---

## 7. Auflösungsalgorithmus

Das VFS löst einen Pfad komponentenweise auf:

1. nächste Pfadkomponente lesen,
2. Verzeichnisrechte prüfen,
3. VNode ermitteln,
4. symbolischen Link erkennen,
5. Linkinhalt validieren,
6. Zielpfad in die verbleibende Auflösung einsetzen,
7. Linktiefe erhöhen,
8. Namespace- und Root-Grenzen erneut prüfen,
9. Auflösung fortsetzen,
10. abschließende Objektoperation atomar ausführen.

Jede durch einen Link eingefügte Komponente unterliegt denselben Prüfungen wie der ursprüngliche Pfad.

---

## 8. Auflösungstiefe

NovaOS begrenzt die Anzahl verfolgter symbolischer Links.

```text
VFS_SYMLINK_MAX_DEPTH = 40
```

Der Grenzwert gilt für die gesamte einzelne Pfadauflösung, nicht nur für direkt aufeinanderfolgende Links.

Wird der Grenzwert überschritten, liefert das VFS:

```text
STATUS_SYMLINK_LOOP
```

Eine separate vollständige Schleifensuche ist nicht zwingend erforderlich, solange die Tiefe sicher begrenzt bleibt.

---

## 9. Erstellen eines Links

```c
status_t vfs_symlink_at(
    cap_handle_t directory_handle,
    const char *link_name,
    const char *target_path,
    uint32_t flags
);
```

Der Aufrufer benötigt:

- Traversierungsrecht für das Verzeichnis,
- `VFS_RIGHT_CREATE_CHILD`,
- einen zulässigen Linknamen,
- ausreichende Ressourcen.

Für das Ziel selbst ist beim Erstellen keine Capability erforderlich, da es nicht existieren muss.

Der Link darf jedoch keine unzulässigen Zeichen oder überlangen Pfade enthalten.

---

## 10. Linkinhalt lesen

Der gespeicherte Linkinhalt kann ohne Zielauflösung gelesen werden:

```c
status_t vfs_readlink_at(
    cap_handle_t directory_handle,
    const char *path,
    char *buffer,
    uint32_t capacity,
    uint32_t *required_size
);
```

`readlink` liefert exakt den gespeicherten Pfadtext.

Der Kernel fügt nicht automatisch ein Nullzeichen hinzu, sofern die API-Spezifikation dies nicht ausdrücklich anfordert.

---

## 11. Follow- und No-Follow-Regeln

VFS-Operationen müssen eindeutig festlegen, ob der letzte Link verfolgt wird.

Mögliche Flags:

```c
#define VFS_RESOLVE_FOLLOW_FINAL      (1u << 0)
#define VFS_RESOLVE_NO_FOLLOW_FINAL   (1u << 1)
#define VFS_RESOLVE_NO_SYMLINKS       (1u << 2)
#define VFS_RESOLVE_BENEATH_ROOT      (1u << 3)
#define VFS_RESOLVE_NO_MOUNT_ESCAPE   (1u << 4)
```

Zwischenkomponenten werden normalerweise verfolgt, sofern `VFS_RESOLVE_NO_SYMLINKS` nicht gesetzt ist.

Sicherheitskritische Verwaltungsoperationen sollen No-Follow- oder Handle-basierte Zugriffe bevorzugen.

---

## 12. Öffnen eines Links

Wird ein Pfad mit `VFS_RESOLVE_NO_FOLLOW_FINAL` geöffnet, kann das VFS ein Handle auf den Link selbst zurückgeben, sofern die Operation dies unterstützt.

Dieses Handle erlaubt beispielsweise:

- Linkinhalt lesen,
- Metadaten untersuchen,
- Link löschen,
- Link umbenennen.

Es erteilt keinen Zugriff auf das Zielobjekt.

---

## 13. Berechtigungsprüfung

Die Berechtigungen des Links ersetzen nicht die Berechtigungen des Zielpfads.

Bei der Zielauflösung prüft das VFS:

- Traversierungsrechte aller Zielverzeichnisse,
- Mount-Flags,
- Namespace-Grenzen,
- Rechte am abschließenden Ziel,
- angeforderte File-Handle-Rechte.

Die Kenntnis oder Lesbarkeit eines Linkinhalts berechtigt nicht zum Öffnen des Ziels.

---

## 14. TOCTOU-Schutz

Folgender Ablauf ist sicherheitskritisch und zu vermeiden:

```text
Pfad prüfen
Link separat untersuchen
Pfad später erneut öffnen
```

Zwischen Prüfung und Öffnen könnte ein Angreifer den Link austauschen.

NovaOS verwendet daher:

- atomare `open_at`-Operationen,
- Directory-Capabilities,
- No-Follow-Flags,
- VNode- und Generationsprüfungen,
- Handle-basierte Folgeoperationen.

Eine erfolgreiche Prüfung eines Pfadtexts ist keine dauerhafte Autorisierung für spätere Operationen.

---

## 15. Mount-Übergänge

Ein symbolischer Link darf auf ein Ziel in einem anderen sichtbaren Mount verweisen.

Dies ist nur zulässig, wenn:

- der Ziel-Mount im aktuellen Namespace sichtbar ist,
- keine `NO_MOUNT_ESCAPE`-Regel verletzt wird,
- die Mount-Flags die Operation erlauben,
- der Capability-Root nicht verlassen wird.

Ein Link kann keinen verborgenen Mount sichtbar machen.

---

## 16. Löschen und Umbenennen

Das Löschen eines symbolischen Links entfernt standardmäßig nur den Link selbst, nicht sein Ziel.

Auch Umbenennen und Verschieben wirken auf den Linkeintrag, sofern die Operation den abschließenden Link nicht ausdrücklich verfolgt.

Bei Verwaltungsoperationen muss die API deshalb eine eindeutige Final-Component-Semantik besitzen.

---

## 17. Beschädigte und ungültige Links

Ein symbolischer Link kann ungültig sein, wenn:

- das Ziel nicht existiert,
- der gespeicherte Pfad ungültig ist,
- die maximale Linktiefe überschritten wird,
- das Ziel außerhalb des erlaubten Roots liegt,
- ein benötigter Mount nicht sichtbar ist,
- Rechte auf dem Zielpfad fehlen.

Ein nicht vorhandenes Ziel macht den Link nicht selbst zu einem beschädigten Dateisystemobjekt.

---

## 18. Dateisystemunterstützung

Dateisysteme können symbolische Links unterschiedlich speichern:

- eigener On-Disk-Objekttyp,
- Inline-Linkinhalt,
- regulärer Datenblock mit Linkkennzeichnung,
- Emulation durch eine Kompatibilitätsschicht.

Das VFS stellt unabhängig von der Speicherung dieselbe native Semantik bereit.

Dateisysteme ohne sichere Linkunterstützung dürfen die Funktion mit `STATUS_NOT_SUPPORTED` ablehnen.

---

## 19. Cache-Integration

VNodes und Verzeichniseinträge symbolischer Links dürfen gecacht werden.

Der Cache muss invalidiert werden bei:

- Linklöschung,
- Umbenennung,
- Änderung des Linkinhalts,
- Mountwechsel,
- Dateisystemgenerationen,
- extern erkannten Änderungen.

Bereits aufgelöste Ziele dürfen nicht unbegrenzt gecacht werden, wenn sich Namespace, Mountstruktur oder Berechtigungen geändert haben.

---

## 20. Sicherheit

Für symbolische Links gelten folgende Regeln:

- Links erteilen keine Rechte auf ihr Ziel.
- Absolute Links bleiben innerhalb des aktuellen Namespace-Roots.
- `..` darf Root- und Capability-Grenzen nicht überschreiten.
- Linktiefe ist fest begrenzt.
- Sicherheitskritische APIs unterstützen No-Follow.
- Mount- und Zugriffsrechte werden nach jeder Linkauflösung erneut geprüft.
- Linkinhalte gelten als nicht vertrauenswürdige Pfaddaten.
- Symbolische Links dürfen keine Kernelobjekt-IDs oder Capabilities kodieren.

---

## 21. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_NOT_FOUND` | Link oder Ziel existiert nicht |
| `STATUS_SYMLINK_LOOP` | maximale Auflösungstiefe überschritten |
| `STATUS_ACCESS_DENIED` | Ziel oder Pfad ist nicht zugänglich |
| `STATUS_ROOT_ESCAPE` | Auflösung würde den erlaubten Root verlassen |
| `STATUS_INVALID_PATH` | Linkinhalt oder Zielpfad ist ungültig |
| `STATUS_NAME_TOO_LONG` | Linkname oder Zielpfad ist zu lang |
| `STATUS_NOT_SYMLINK` | Objekt ist kein symbolischer Link |
| `STATUS_SYMLINK_DISALLOWED` | Operation erlaubt keine Linkauflösung |
| `STATUS_MOUNT_BOUNDARY` | verbotener Mountübergang |
| `STATUS_NOT_SUPPORTED` | Dateisystem unterstützt keine Links |

---

## 22. Audit und Diagnose

Normale Linkauflösungen werden nicht auditiert.

Audit-Ereignisse können erzeugt werden bei:

- wiederholten Root-Escape-Versuchen,
- Symlink-Angriffen auf geschützte Systempfade,
- Umleitung privilegierter Operationen,
- Veränderungen sicherheitskritischer Systemlinks,
- Linkauflösung in Recovery- oder Update-Namespaces.

Diagnosewerkzeuge dürfen Linkinhalt, VNode-Typ und Auflösungsfehler anzeigen, wenn der Aufrufer die erforderlichen Rechte besitzt.

---

## 23. Alternative Ansätze

### 23.1 Keine symbolischen Links

**Vorteile:**

- einfachere und sicherere Pfadauflösung

**Nachteile:**

- schlechtere Kompatibilität
- weniger flexible Verzeichnisstrukturen
- erschwerte Versionsumschaltungen

**Entscheidung:** Abgelehnt.

### 23.2 Links als direkte VNode-Referenzen

**Vorteile:**

- kein ungültiges Ziel durch Umbenennung
- schnelle Auflösung

**Nachteile:**

- entspricht eher Hard Links
- ungeeignet über Dateisystem- und Mountgrenzen

**Entscheidung:** Abgelehnt.

### 23.3 Vollständiges Verbot von Links in privilegierten Operationen

**Vorteile:**

- geringere Angriffsfläche

**Nachteile:**

- unnötig unflexibel

**Entscheidung:** Privilegierte Operationen verwenden explizite No-Follow- und Root-Beschränkungen statt eines globalen Verbots.

---

## 24. Konsequenzen

### Positive Konsequenzen

- flexible Dateisystemstrukturen,
- Unterstützung von Kompatibilitätsumgebungen,
- kontrollierte Verweise über Mountgrenzen,
- sichere Namespace-Integration,
- eindeutige Follow- und No-Follow-Semantik,
- Schutz vor Linkschleifen und Root Escape.

### Negative Konsequenzen

- komplexere Pfadauflösung,
- zusätzliche TOCTOU-Risiken,
- Linkziele können ungültig werden,
- Dateisysteme benötigen eine kompatible Speicherung.

### Risiken

- Umleitung privilegierter Dateioperationen,
- Linktausch zwischen Prüfung und Verwendung,
- Schleifen und überlange Auflösung,
- unerwartete Mountübergänge,
- fehlerhafte Cacheinvalidierung.

---

## 25. Implementierungsrichtlinien

- Die Linkauflösung erfolgt ausschließlich im VFS.
- Absolute Links starten am aktuellen Namespace- oder Capability-Root.
- Jede eingefügte Pfadkomponente wird vollständig neu geprüft.
- Die Auflösungstiefe wird auf 40 Links begrenzt.
- Native Sicherheits-APIs verwenden Directory-Capabilities und No-Follow-Flags.
- Linkinhalt lesen und Linkziel öffnen bleiben getrennte Operationen.
- Löschen wirkt standardmäßig auf den Link und nicht auf sein Ziel.
- Linkauflösung und abschließendes Öffnen müssen atomar verbunden sein.
- Caches beachten Namespace-, Mount- und Dateisystemgenerationen.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- relative und absolute Links,
- nicht vorhandene Ziele,
- verschachtelte Links,
- direkte und indirekte Schleifen,
- maximale Auflösungstiefe,
- `..` und Root-Grenzen,
- Mountübergänge,
- No-Follow-Flags,
- Linkerstellung und `readlink`,
- Löschen und Umbenennen,
- parallelen Linktausch,
- Namespace-Wechsel,
- Cacheinvalidierung,
- beschädigte Linkinhalte,
- TOCTOU-Angriffsversuche.

---

## 27. Verbindliche Invarianten

1. Symbolische Links speichern Pfade und keine direkte Objektidentität.
2. Ein Link erteilt keine Zugriffsrechte auf sein Ziel.
3. Absolute Links beginnen am Root der aktuellen Sicherheitsansicht.
4. Links dürfen den Namespace- oder Capability-Root nicht verlassen.
5. Jede Zielkomponente wird erneut validiert und autorisiert.
6. Die Anzahl verfolgter Links ist fest begrenzt.
7. Native APIs besitzen eine eindeutige Follow- oder No-Follow-Semantik.
8. Das Löschen eines Links löscht nicht dessen Ziel.
9. Pfadprüfung und Objektöffnung müssen gegen TOCTOU-Rennen geschützt sein.
10. Symbolische Links können keine verborgenen Mounts oder Capabilities sichtbar machen.

---

## 28. Zusammenfassung

NovaOS unterstützt symbolische Links als native VFS-Objekte mit UTF-8-kodierten Pfadzielen.

Die Auflösung bleibt vollständig innerhalb des aktuellen Mount Namespace und eines optional eingeschränkten Capability-Roots. Feste Tiefenlimits, No-Follow-Optionen und atomare Handle-basierte Dateioperationen schützen vor Schleifen, Root Escape und TOCTOU-Angriffen.

Symbolische Links verbessern Flexibilität und Kompatibilität, gewähren selbst aber niemals Zugriff auf ihr Ziel.