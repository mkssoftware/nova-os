# ADR-IPC-0005 – Named Objects

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-IPC-0005 |
| Titel | Named Objects |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Interprozesskommunikation und Kernelobjekte |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-IPC-0001, ADR-IPC-0002, ADR-IPC-0003, ADR-KERNEL-0102, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-IPC-NAMED-OBJECTS |

---

## 1. Kontext

NovaOS verwendet Capabilities für den Zugriff auf Kernelobjekte. Eine Capability kann jedoch nur verwendet werden, wenn ein Prozess sie bereits besitzt oder von einem anderen Prozess übertragen bekommt.

Für bestimmte systemweite oder sitzungsbezogene Dienste wird zusätzlich ein kontrollierter Namensraum benötigt.

Typische benannte Objekte sind:

- Message-Channel-Endpunkte
- Shared-Memory-Objekte
- Events
- Semaphore
- Service-Endpunkte
- ausgewählte Geräte- und Systemdienste

Named Objects ermöglichen das Auffinden eines Objekts über einen stabilen Namen. Der Name selbst erteilt jedoch noch keinen Zugriff.

---

## 2. Problemstellung

Ein globaler, unkontrollierter Objektnamensraum würde mehrere Risiken erzeugen:

- Namenskonflikte
- Service-Spoofing
- unerlaubtes Auffinden fremder Objekte
- Informationslecks
- fehlende Trennung zwischen Benutzern und Sitzungen
- unbegrenztes Erzeugen benannter Objekte
- schwer definierbare Lebenszeiten

NovaOS benötigt deshalb einen hierarchischen und capability-geschützten Namensraum.

---

## 3. Entscheidung

NovaOS führt kernelverwaltete Object Namespaces ein.

Ein Named Object besteht aus:

- einem Namen,
- einem Eintrag in einem Namespace,
- einer Referenz auf ein Kernelobjekt,
- einer Zugriffsrichtlinie,
- optionalen Lebenszeit- und Sichtbarkeitsregeln.

Das Auflösen eines Namens liefert keine rohe Objekt-ID, sondern eine neue Capability mit den erlaubten Rechten.

---

## 4. Grundprinzip

Der Zugriff erfolgt in zwei getrennten Schritten:

1. Ein Prozess besitzt eine Capability für einen Object Namespace.
2. Der Prozess löst innerhalb dieses Namespace einen Namen auf.

```text
Namespace-Capability
        |
        v
   Name auflösen
        |
        v
Berechtigungen prüfen
        |
        v
Objekt-Capability erzeugen
```

Ohne eine gültige Namespace-Capability kann ein Prozess weder Namen registrieren noch auflösen.

---

## 5. Namespace-Hierarchie

NovaOS verwendet getrennte Namensräume für unterschiedliche Gültigkeitsbereiche:

| Namespace | Gültigkeitsbereich |
|---|---|
| System | Gesamtes laufendes System |
| Session | Eine Benutzer- oder Anmeldesitzung |
| User | Ein bestimmter Benutzerkontext |
| Job | Prozesse innerhalb eines Job Objects |
| Process | Nur der aktuelle Prozess |
| Private | Nur explizit autorisierte Prozesse |

Beispielhafte Namen:

```text
/system/services/display
/system/services/audio
/session/services/clipboard
/user/events/settings-changed
/job/shared/cache
/process/local/worker-ready
```

Ein Prozess sieht ausschließlich Namensräume, für die er eine Capability besitzt.

---

## 6. Namensregeln

Objektnamen müssen folgende Anforderungen erfüllen:

- UTF-8-Kodierung
- Normalisierung in eine definierte Unicode-Normalform
- Groß- und Kleinschreibung werden unterschieden
- `/` trennt Namenskomponenten
- `.` und `..` sind nicht zulässig
- leere Namenskomponenten sind nicht zulässig
- Namen müssen eine feste Maximallänge einhalten
- eingebettete Nullzeichen sind verboten

Empfohlene Grenzwerte:

| Eigenschaft | Grenzwert |
|---|---:|
| Maximale Gesamtlänge | 1024 Byte |
| Maximale Komponentenlänge | 255 Byte |
| Maximale Hierarchietiefe | 32 Ebenen |

---

## 7. Unterstützte Objekttypen

Nicht jedes Kernelobjekt darf benannt werden.

Die erste Implementierung unterstützt:

- Message-Channel-Endpunkte
- Shared-Memory-Objekte
- Events
- Semaphore
- Service-Registrierungen
- ausgewählte Geräteendpunkte

Nicht benennbar sind standardmäßig:

- Prozesse
- Threads
- rohe Speicherbereiche
- Seitentabellen
- Interruptobjekte
- interne Kernelstrukturen
- temporäre Sicherheitsobjekte

Weitere Objekttypen müssen ausdrücklich für die Registrierung freigegeben werden.

---

## 8. Registrierung

Ein Objekt wird über eine gültige Namespace-Capability registriert:

```c
status_t object_name_register(
    cap_handle_t namespace_handle,
    const char *name,
    cap_handle_t object_handle,
    uint32_t flags
);
```

Mögliche Flags:

```c
#define NAME_FLAG_EXCLUSIVE    (1u << 0)
#define NAME_FLAG_REPLACEABLE  (1u << 1)
#define NAME_FLAG_EPHEMERAL    (1u << 2)
#define NAME_FLAG_PRIVATE      (1u << 3)
```

`NAME_FLAG_EXCLUSIVE` verhindert das Überschreiben eines bereits vorhandenen Namens.

Eine Ersetzung ist nur zulässig, wenn der vorhandene Eintrag ausdrücklich ersetzbar ist und der Aufrufer die erforderlichen Verwaltungsrechte besitzt.

---

## 9. Auflösung

Ein Prozess kann ein benanntes Objekt öffnen:

```c
status_t object_name_open(
    cap_handle_t namespace_handle,
    const char *name,
    uint32_t requested_rights,
    cap_handle_t *object_handle
);
```

Der Kernel prüft:

1. die Namespace-Capability,
2. die syntaktische Gültigkeit des Namens,
3. die Existenz des Eintrags,
4. die Sicherheitsrichtlinie des Eintrags,
5. die angeforderten Objektrechte.

Die zurückgegebene Capability enthält höchstens die Rechte, die der Namenseintrag für den Aufrufer erlaubt.

---

## 10. Entfernung und Ersetzung

Ein Namenseintrag kann entfernt werden:

```c
status_t object_name_unregister(
    cap_handle_t namespace_handle,
    const char *name
);
```

Das Entfernen des Namens zerstört nicht automatisch das referenzierte Kernelobjekt.

Bereits ausgegebene Capabilities bleiben gültig, sofern das Objekt oder die Capabilities nicht ausdrücklich widerrufen werden.

Für einen kontrollierten Serviceaustausch kann eine atomare Ersetzungsoperation vorgesehen werden:

```c
status_t object_name_replace(
    cap_handle_t namespace_handle,
    const char *name,
    cap_handle_t new_object_handle
);
```

Neue Auflösungen erhalten anschließend das neue Objekt. Bereits bestehende Capabilities bleiben unverändert.

---

## 11. Zugriffsrechte

Namespace-Capabilities besitzen mindestens folgende Rechte:

| Recht | Bedeutung |
|---|---|
| `NAMESPACE_RIGHT_LOOKUP` | Namen auflösen |
| `NAMESPACE_RIGHT_REGISTER` | Neue Namen registrieren |
| `NAMESPACE_RIGHT_UNREGISTER` | Eigene Namen entfernen |
| `NAMESPACE_RIGHT_REPLACE` | Vorhandene Einträge ersetzen |
| `NAMESPACE_RIGHT_ENUMERATE` | Sichtbare Namen auflisten |
| `NAMESPACE_RIGHT_CREATE_CHILD` | Untergeordneten Namespace erzeugen |
| `NAMESPACE_RIGHT_ADMIN` | Namespace verwalten |
| `NAMESPACE_RIGHT_TRANSFER` | Namespace-Capability weitergeben |

Das Recht zum Auflösen eines Namens beinhaltet nicht automatisch das Recht, den gesamten Namespace aufzulisten.

---

## 12. Sicherheitsrichtlinie

Jeder Namenseintrag besitzt eine Richtlinie, die festlegt:

- welche Sicherheitskontexte das Objekt öffnen dürfen,
- welche maximalen Rechte ausgegeben werden,
- ob der Eintrag aufgelistet werden darf,
- ob der Eintrag ersetzt werden kann,
- ob eine besondere Capability erforderlich ist.

Ein Name ist daher nur ein Bezeichner und niemals ein Sicherheitsnachweis.

Die Kenntnis von `/system/services/security` berechtigt einen Prozess nicht zum Öffnen dieses Dienstes.

---

## 13. Schutz vor Service-Spoofing

Systemnamen dürfen nur durch vertrauenswürdige Komponenten registriert werden.

Für geschützte Dienste gelten folgende Regeln:

- Reservierte Präfixe benötigen administrative Rechte.
- Kritische Systemnamen werden bereits während des Systemstarts registriert.
- Nicht privilegierte Prozesse dürfen keine ähnlich aussehenden Systemnamen anlegen.
- Namen werden vor Vergleichen einheitlich normalisiert.
- Homoglyphen und unzulässige Steuerzeichen werden in geschützten Namespaces abgelehnt.
- Ersetzungen sicherheitskritischer Dienste werden auditiert.

Systemkomponenten dürfen einem Dienst nicht ausschließlich aufgrund seines Namens vertrauen. Die zurückgegebene Capability und die Objektidentität müssen ebenfalls validiert werden.

---

## 14. Lebenszeit

Named Objects unterstützen unterschiedliche Lebenszeitmodelle:

| Modell | Verhalten |
|---|---|
| Persistent bis Unregister | Name bleibt bis zur ausdrücklichen Entfernung bestehen |
| Ephemeral | Name wird entfernt, wenn der registrierende Besitzer endet |
| Namespacegebunden | Name endet mit dem zugehörigen Namespace |
| Serviceverwaltet | Lebenszeit wird durch einen Systemdienst kontrolliert |

Das Entfernen eines Namens und das Zerstören des Objekts sind getrennte Vorgänge.

Ein verwaister Eintrag, dessen Zielobjekt nicht mehr existiert, muss automatisch entfernt oder als ungültig markiert werden.

---

## 15. Session- und Benutzerisolation

Benutzersitzungen erhalten getrennte Session-Namespaces.

Dadurch können mehrere Sitzungen gleichzeitig Namen wie folgenden verwenden:

```text
/session/services/clipboard
```

Die Namen bezeichnen in jeder Sitzung unterschiedliche Objekte.

Prozesse erhalten beim Start nur die Namespace-Capabilities, die ihrem Sicherheits-, Benutzer-, Session- und Job-Kontext entsprechen.

Ein Wechsel in einen anderen Namespace ist ohne ausdrückliche Capability nicht möglich.

---

## 16. Auflistung

Wenn ein Prozess `NAMESPACE_RIGHT_ENUMERATE` besitzt, darf er sichtbare Einträge abfragen:

```c
status_t object_name_enumerate(
    cap_handle_t namespace_handle,
    uint64_t cursor,
    object_name_entry_t *entries,
    uint32_t entry_capacity,
    uint32_t *entry_count,
    uint64_t *next_cursor
);
```

Die Auflistung darf nur Metadaten offenlegen, die laut Richtlinie sichtbar sind.

Objektadressen, Kernelzeiger und nicht autorisierte Eigentümerinformationen dürfen nicht ausgegeben werden.

---

## 17. Parallelität

Registrierung, Auflösung, Entfernung und Ersetzung können gleichzeitig stattfinden.

Der Kernel garantiert:

- atomare Registrierung eines einzelnen Namens,
- atomare Entfernung,
- atomare Ersetzung der Objektreferenz,
- keine teilweise sichtbaren Namenseinträge,
- sichere Referenzierung während einer Auflösung.

Eine erfolgreiche Auflösung liefert entweder das alte oder das neue Objekt, niemals einen ungültigen Zwischenzustand.

---

## 18. Ressourcenbegrenzung

Named Objects werden begrenzt:

- pro Prozess
- pro Benutzer
- pro Session
- pro Job Object
- pro Namespace
- systemweit

Limits berücksichtigen:

- Anzahl registrierter Namen
- Gesamtlänge der Namen
- Anzahl der Namespaces
- Anzahl der Unterebenen
- Anzahl gleichzeitiger Auflösungen

Nicht privilegierte Prozesse dürfen keine unbegrenzten Einträge im System-Namespace erzeugen.

---

## 19. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Ungültige Capability |
| `STATUS_INVALID_NAME` | Name ist syntaktisch ungültig |
| `STATUS_NAME_TOO_LONG` | Name überschreitet den Grenzwert |
| `STATUS_NAME_NOT_FOUND` | Kein sichtbarer Eintrag vorhanden |
| `STATUS_NAME_EXISTS` | Name ist bereits registriert |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_OBJECT_TYPE_MISMATCH` | Objekttyp darf nicht registriert werden |
| `STATUS_OBJECT_CLOSED` | Zielobjekt existiert nicht mehr |
| `STATUS_LIMIT_EXCEEDED` | Ressourcenlimit wurde überschritten |
| `STATUS_NOT_REPLACEABLE` | Eintrag darf nicht ersetzt werden |

Aus Sicherheitsgründen darf `STATUS_NAME_NOT_FOUND` auch zurückgegeben werden, wenn ein Eintrag existiert, für den der Aufrufer keinerlei Sichtbarkeitsrecht besitzt.

---

## 20. Audit und Diagnose

Audit-Ereignisse werden insbesondere erzeugt bei:

- Registrierung geschützter Systemnamen
- Ersetzung sicherheitskritischer Dienste
- wiederholten unerlaubten Auflösungsversuchen
- Namenskonflikten in geschützten Namespaces
- administrativer Entfernung fremder Einträge
- Überschreitung von Ressourcenlimits

Normale Auflösungen gewöhnlicher Anwendungsobjekte werden nicht standardmäßig protokolliert.

Diagnosewerkzeuge dürfen nur Namespaces und Einträge anzeigen, für die eine entsprechende Inspektionsberechtigung besteht.

---

## 21. Alternative Ansätze

### 21.1 Ein globaler Namespace

**Vorteile:**

- einfache Implementierung
- alle Dienste über denselben Pfad erreichbar

**Nachteile:**

- schwache Isolation
- Namenskonflikte
- erhöhtes Spoofing-Risiko

**Entscheidung:** Abgelehnt.

### 21.2 Namen direkt als Zugriffsberechtigung

**Vorteile:**

- einfache APIs

**Nachteile:**

- Name wird zum unsicheren Geheimnis
- keine feingranulare Rechteverwaltung
- schwer widerrufbar

**Entscheidung:** Abgelehnt. Namen dienen nur zur Auflösung in Capabilities.

### 21.3 Ausschließlich direkte Capability-Übergabe

**Vorteile:**

- kein globaler Namensraum erforderlich
- starke Kontrolle über Verbindungen

**Nachteile:**

- initiale Dienstauffindung bleibt ungelöst
- zentrale Systemdienste sind schwer erreichbar

**Entscheidung:** Direkte Übergabe bleibt bevorzugt, Named Objects ergänzen sie für die initiale Auffindung.

---

## 22. Konsequenzen

### Positive Konsequenzen

- kontrollierte Auffindbarkeit von Systemdiensten
- Trennung zwischen Namen und Zugriffsrechten
- Isolation zwischen Benutzern, Sessions und Jobs
- Schutz vor globalen Namenskonflikten
- Integration in das Capability-Modell
- atomarer Austausch von Service-Endpunkten

### Negative Konsequenzen

- zusätzlicher Kernelaufwand für Namensräume
- Lebenszeit und Ersetzung müssen sorgfältig verwaltet werden
- Unicode-Normalisierung erhöht die Implementierungskomplexität
- Anwendungen benötigen weiterhin Capabilities für den tatsächlichen Zugriff

### Risiken

- Service-Spoofing durch ähnlich aussehende Namen
- verwaiste Namenseinträge
- Ressourcenerschöpfung
- versehentlich zu weitreichende Namespace-Capabilities
- Informationslecks durch ungeschützte Auflistung

---

## 23. Implementierungsrichtlinien

- Namen müssen vor Speicherung und Vergleich normalisiert werden.
- Namensauflösung und Capability-Erzeugung müssen gemeinsam autorisiert werden.
- Namen dürfen niemals Kernelzeiger oder Objektadressen enthalten.
- Die Enumeration benötigt ein getrenntes Zugriffsrecht.
- Systempräfixe müssen gegen nicht autorisierte Registrierung geschützt sein.
- Ephemerale Einträge müssen bei Prozessende zuverlässig entfernt werden.
- Ersetzungen müssen atomar erfolgen.
- Namensfehler dürfen keine geschützten Einträge offenlegen.
- Namensauflösung darf symbolische Schleifen nicht ermöglichen.

---

## 24. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Registrierung und Auflösung
- Entfernung und atomare Ersetzung
- getrennte System-, Session-, User- und Job-Namespaces
- Zugriffsrechte
- Unicode-Normalisierung
- ungültige und überlange Namen
- doppelte Registrierung
- ephemerale Einträge
- Prozessbeendigung
- geschützte Systempräfixe
- Enumeration mit eingeschränkter Sichtbarkeit
- Ressourcenlimits
- parallele Auflösung und Ersetzung
- Service-Spoofing-Versuche

---

## 25. Verbindliche Invarianten

1. Ein Name ist kein Zugriffsrecht.
2. Jede Namensauflösung benötigt eine gültige Namespace-Capability.
3. Die Auflösung liefert ausschließlich eine capability-geschützte Objektreferenz.
4. Ausgegebene Rechte dürfen die Richtlinie des Namenseintrags nicht überschreiten.
5. System-, Session-, User-, Job- und private Namespaces bleiben voneinander isoliert.
6. Namensregistrierung, Entfernung und Ersetzung erfolgen atomar.
7. Das Entfernen eines Namens widerruft bestehende Objekt-Capabilities nicht automatisch.
8. Nicht sichtbare Einträge dürfen durch Fehlercodes nicht offengelegt werden.
9. Reservierte Systemnamen dürfen nur autorisierte Komponenten registrieren.
10. Named Objects ersetzen nicht die direkte Capability-Übergabe.

---

## 26. Zusammenfassung

NovaOS verwendet hierarchische, capability-geschützte Object Namespaces zur kontrollierten Auffindung von Kernelobjekten und Systemdiensten.

Namen dienen ausschließlich als Bezeichner. Der tatsächliche Zugriff erfolgt immer über eine vom Kernel erzeugte und eingeschränkte Capability.

Getrennte Namespaces für System, Benutzer, Sessions, Jobs und Prozesse verhindern Namenskonflikte und reduzieren das Risiko von Service-Spoofing und unerlaubtem Informationszugriff.