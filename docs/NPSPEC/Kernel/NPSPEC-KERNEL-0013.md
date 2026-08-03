# NPSPEC-KERNEL-0013 – Handle Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0013 |
| Titel | Handle Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Handle- und Ressourcenverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0011, NPSPEC-KERNEL-0012 |
| Zugehörige ADRs | ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-IPC-0005, ADR-VFS-0007 |

---

## 1. Zweck

Diese Spezifikation definiert den Handle Manager des NovaOS-Kernels.

Der Handle Manager verwaltet:

- Prozess- und Kernel-Handle-Tabellen,
- Erzeugung und Schließung von Handles,
- Generationsschutz,
- objektspezifische Rechte,
- Duplizierung und Übertragung,
- Vererbung,
- Handle-Limits,
- parallele Zugriffe,
- Diagnose und Audit.

## 2. Grundprinzip

Ein Handle ist eine pro Sicherheitsdomain gültige Referenz auf ein Kernelobjekt.

```c
typedef uint64_t np_handle_t;
```

Ein Handle:

- enthält keinen direkt verwendbaren Kernelzeiger,
- ist nur innerhalb seiner Handle-Tabelle gültig,
- besitzt eine definierte Rechtemenge,
- hält normalerweise eine starke Objektreferenz,
- kann durch Schließen ungültig gemacht werden,
- ist durch eine Generationskennung gegen veraltete Verwendung geschützt.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Object Manager | Kernelobjekte und deren Lebenszyklus |
| Handle Manager | Handle-Tabellen und Zugriffsreferenzen |
| Capability-System | Bedeutung und Delegation von Rechten |
| Process Manager | Zuordnung der Handle-Tabelle zum Prozess |
| System Call Interface | sichere Annahme von Userspace-Handles |
| IPC-System | kontrollierte Handle-Übertragung |

## 4. Handle-Sichtbarkeit

Ein Handlewert ist ausschließlich in seiner Quelltabelle gültig.

Dasselbe Objekt kann in verschiedenen Prozessen über unterschiedliche Handles mit unterschiedlichen Rechten erreichbar sein.

```text
Prozess A: Handle 0x1042 -> Event [WAIT | SIGNAL]
Prozess B: Handle 0x2081 -> Event [WAIT]
```

Ein numerisch gleicher Handlewert in zwei Prozessen muss nicht auf dasselbe Objekt verweisen.

## 5. Handle-Aufbau

Ein Handle wird logisch aus folgenden Teilen gebildet:

```text
[ optionale Tags | Generation | Tabellenindex ]
```

Die konkrete Bitaufteilung ist ABI- und architekturabhängig.

Der Aufbau muss mindestens ermöglichen:

- effiziente Tabellenauflösung,
- Erkennung veralteter Handles,
- Reservierung besonderer Werte,
- Erweiterung ohne Offenlegung von Kerneladressen.

## 6. Reservierte Handlewerte

Mindestens folgende Werte sind reserviert:

```c
#define NP_INVALID_HANDLE ((np_handle_t)0)
```

Zusätzliche Werte können für Pseudohandles reserviert werden.

Reguläre Handle-Erzeugung darf niemals einen reservierten Wert zurückgeben.

## 7. Handle-Tabelle

Jeder Userspace-Prozess besitzt genau eine primäre Handle-Tabelle.

```c
typedef struct np_handle_table {
    np_handle_segment_t** segments;

    uint32_t segment_count;
    uint32_t segment_capacity;

    uint64_t active_handles;
    uint64_t maximum_handles;

    uint32_t allocation_hint;
    uint32_t flags;

    np_rwlock_t lock;
} np_handle_table_t;
```

Die interne Implementierung kann segmentiert, mehrstufig oder anderweitig erweiterbar sein.

## 8. Kernel-Handle-Tabelle

Der Kernel besitzt eine oder mehrere interne Handle-Tabellen.

Kernelhandles:

- sind nicht direkt im Userspace gültig,
- werden nicht über öffentliche APIs offengelegt,
- unterliegen weiterhin Typ- und Rechteprüfungen,
- können auf bestimmte Kernel-Subsysteme begrenzt sein.

Ein Kernelhandle darf nicht durch bloßes Kopieren seines Wertes in ein Userspace-Handle umgewandelt werden.

## 9. Handle-Eintrag

```c
typedef struct np_handle_entry {
    np_kernel_object_t* object;
    np_capability_rights_t rights;

    uint32_t flags;
    uint32_t generation;

    uint32_t object_type;
    uint32_t state;
} np_handle_entry_t;
```

Der Eintrag liegt ausschließlich im geschützten Kernelspeicher.

## 10. Eintragszustände

```c
typedef enum np_handle_entry_state {
    NP_HANDLE_ENTRY_FREE,
    NP_HANDLE_ENTRY_RESERVED,
    NP_HANDLE_ENTRY_ACTIVE,
    NP_HANDLE_ENTRY_CLOSING,
    NP_HANDLE_ENTRY_TRANSFER_PENDING
} np_handle_entry_state_t;
```

| Zustand | Bedeutung |
|---|---|
| `FREE` | Eintrag ist unbenutzt |
| `RESERVED` | Eintrag wird initialisiert |
| `ACTIVE` | Handle ist verwendbar |
| `CLOSING` | Schließung wurde eingeleitet |
| `TRANSFER_PENDING` | Eintrag ist Teil einer atomaren Übertragung |

Nur Einträge im Zustand `ACTIVE` dürfen regulär aufgelöst werden.

## 11. Handle-Flags

```c
typedef enum np_handle_flags {
    NP_HANDLE_INHERITABLE    = 1U << 0,
    NP_HANDLE_TRANSFERABLE   = 1U << 1,
    NP_HANDLE_CLOSE_ON_EXEC  = 1U << 2,
    NP_HANDLE_AUDITED        = 1U << 3,
    NP_HANDLE_PROTECTED      = 1U << 4,
    NP_HANDLE_KERNEL_ONLY    = 1U << 5,
    NP_HANDLE_NO_DUPLICATE   = 1U << 6
} np_handle_flags_t;
```

Unbekannte oder widersprüchliche Flags müssen abgelehnt werden.

## 12. Handle-Erzeugung

```c
np_status_t np_handle_create(
    np_handle_table_t* table,
    np_kernel_object_t* object,
    np_capability_rights_t rights,
    uint32_t flags,
    np_handle_t* handle
);
```

Der Ablauf ist:

1. Objekt und Rechte prüfen,
2. freien Eintrag reservieren,
3. starke Objektreferenz übernehmen,
4. Rechte und Flags setzen,
5. Generationswert festlegen,
6. Eintrag atomar aktivieren,
7. Handlewert erzeugen,
8. Ergebnis an den Aufrufer zurückgeben.

Ein unvollständig initialisiertes Handle darf niemals sichtbar werden.

## 13. Rechteprüfung bei Erzeugung

Die angeforderten Rechte müssen:

- vom Objekttyp unterstützt werden,
- durch den Erzeuger delegierbar sein,
- mit dem Sicherheitsprofil übereinstimmen,
- innerhalb der Job- und Domain-Grenzen liegen.

Unbekannte oder nicht delegierbare Rechte werden nicht stillschweigend entfernt, sondern als Fehler abgelehnt.

## 14. Handle-Auflösung

```c
np_status_t np_handle_resolve(
    np_handle_table_t* table,
    np_handle_t handle,
    np_object_type_id_t expected_type,
    np_capability_rights_t required_rights,
    np_kernel_object_t** object
);
```

Die Auflösung prüft:

- reservierte Handlewerte,
- Tabellenindex,
- Generationskennung,
- Eintragszustand,
- erwarteten Objekttyp,
- erforderliche Rechte,
- Objektzustand,
- Sicherheitsdomain.

Bei Erfolg erhält der Aufrufer eine temporäre starke Objektreferenz.

## 15. Typprüfung

System-Call-Handler müssen den erwarteten Objekttyp angeben.

Ein Datei-Handle darf beispielsweise nicht als Prozess- oder Event-Handle verwendet werden.

Unterstützt eine API mehrere Typen, muss sie eine explizite Liste zulässiger Typen verwenden.

Reine Typinterpretation anhand des Userspace-Eingabewerts ist unzulässig.

## 16. Generationsschutz

Jeder Handle-Eintrag besitzt einen Generationswert.

Beim Schließen wird die Generation erhöht, bevor der Eintrag erneut verwendet werden darf.

Dadurch wird verhindert, dass ein altes Handle unbeabsichtigt ein neues Objekt am selben Tabellenindex referenziert.

```text
Index 42, Generation 7 -> geschlossen
Index 42, Generation 8 -> neues Objekt
altes Handle mit Generation 7 -> ungültig
```

## 17. Generationsüberlauf

Ein Generationszähler kann theoretisch überlaufen.

Die Implementierung muss daher:

- ausreichend große Generationsfelder verwenden,
- schnelle Wiederverwendung desselben Index vermeiden,
- bei kritischem Überlauf einen Eintrag dauerhaft sperren oder die Tabelle erneuern,
- niemals stillschweigend ein möglicherweise wieder gültiges altes Handle erzeugen.

## 18. Temporäre Referenzen

Nach erfolgreicher Handle-Auflösung wird die Tabellensperre freigegeben, während die temporäre Objektreferenz bestehen bleibt.

Dadurch kann ein paralleles Schließen des Handles nicht zur Freigabe des Objekts während einer laufenden Operation führen.

Nach Abschluss der Operation muss die temporäre Referenz freigegeben werden.

## 19. Handle-Schließung

```c
np_status_t np_handle_close(
    np_handle_table_t* table,
    np_handle_t handle
);
```

Der Ablauf ist:

1. Handle dekodieren und validieren,
2. Eintrag atomar auf `CLOSING` setzen,
3. Eintrag für neue Auflösungen sperren,
4. Generation erhöhen,
5. typspezifische Close-Operation ausführen,
6. Handle- und Referenzzähler reduzieren,
7. Objektreferenz freigeben,
8. Eintrag wieder verfügbar machen.

## 20. Geschützte Handles

Handles mit `NP_HANDLE_PROTECTED` dürfen nur geschlossen werden durch:

- den Besitzer mit spezieller Berechtigung,
- den Process Manager während der Prozessbeendigung,
- einen autorisierten Administrator,
- einen definierten Recovery-Pfad.

Geschützte Handles sind für kritische Laufzeit- oder Serviceobjekte vorgesehen und müssen sparsam eingesetzt werden.

## 21. Doppelte Schließung

Das wiederholte Schließen desselben Handlewerts liefert einen definierten Fehler:

```text
NP_ERR_INVALID_HANDLE
```

oder:

```text
NP_ERR_STALE_HANDLE
```

Eine doppelte Schließung darf keine zweite Objektreferenz freigeben.

## 22. Handle-Duplizierung

```c
np_status_t np_handle_duplicate(
    np_handle_table_t* source_table,
    np_handle_t source_handle,
    np_handle_table_t* target_table,
    np_capability_rights_t requested_rights,
    uint32_t flags,
    np_handle_t* target_handle
);
```

Die Duplizierung erzeugt einen neuen Handle-Eintrag und eine neue starke Referenz auf dasselbe Objekt.

## 23. Rechte bei Duplizierung

Die Rechte des Zielhandles müssen eine Teilmenge sein von:

- den Rechten des Quellhandles,
- den delegierbaren Rechten des Aufrufers,
- den im Zielprozess zulässigen Rechten,
- den vom Objekttyp unterstützten Rechten.

Es gilt:

```text
target_rights ⊆ source_rights
```

Eine Rechteerhöhung benötigt eine unabhängige autorisierte Capability und darf nicht allein durch Duplizierung erfolgen.

## 24. Duplizierung innerhalb eines Prozesses

Ein Prozess darf eigene Handles duplizieren, wenn:

- das Quellhandle `DUPLICATE` erlaubt,
- `NP_HANDLE_NO_DUPLICATE` nicht gesetzt ist,
- die neuen Rechte nicht größer sind,
- kein Handle-Limit überschritten wird.

Die neue Handle-Generation und der Tabellenindex sind unabhängig vom Quellhandle.

## 25. Prozessübergreifende Duplizierung

Eine Duplizierung in einen fremden Prozess erfordert:

- Zugriff auf den Zielprozess,
- `DUPLICATE`- oder `TRANSFER`-Recht,
- Zustimmung der Capability-Richtlinie,
- kompatiblen Ziel-Namespace,
- Einhaltung der Zielprozesslimits.

Das numerische Quellhandle wird nicht in den Zielprozess kopiert. Der Zielprozess erhält einen neuen lokalen Handlewert.

## 26. Handle-Übertragung über IPC

IPC-Nachrichten können Handles transportieren.

Der Sender beschreibt:

- Quellhandle,
- zu delegierende Rechte,
- Übertragungsmodus,
- gewünschte Zielflags.

Der Kernel validiert und erzeugt das Zielhandle atomar mit der Nachrichtenzustellung.

Userspace kann keine Kernelobjektzeiger oder selbst erzeugte Handle-Einträge übertragen.

## 27. Kopieren und Verschieben

```c
typedef enum np_handle_transfer_mode {
    NP_HANDLE_TRANSFER_COPY,
    NP_HANDLE_TRANSFER_MOVE
} np_handle_transfer_mode_t;
```

| Modus | Verhalten |
|---|---|
| `COPY` | Quellhandle bleibt gültig |
| `MOVE` | Quellhandle wird nach erfolgreicher Zielerzeugung geschlossen |

Bei fehlgeschlagener Übertragung bleibt das Quellhandle unverändert gültig.

## 28. Atomare Übertragung

Eine `MOVE`-Übertragung muss atomar erscheinen.

Zulässig sind nur folgende Ergebnisse:

- Zielhandle wurde erstellt und Quellhandle geschlossen,
- Zielhandle wurde nicht erstellt und Quellhandle bleibt gültig.

Ein Zwischenzustand mit verlorenem Handle ist unzulässig.

## 29. Handle-Vererbung

Beim Erzeugen eines Kindprozesses werden Handles standardmäßig nicht vererbt.

Vererbung erfolgt nur für ausdrücklich ausgewählte Handles, wenn:

- `NP_HANDLE_INHERITABLE` gesetzt ist,
- der Objekttyp Vererbung erlaubt,
- der Erzeuger delegieren darf,
- das Sicherheitsprofil des Kindes die Rechte zulässt.

Die Rechte können während der Vererbung reduziert werden.

## 30. Close-on-Exec

Handles mit `NP_HANDLE_CLOSE_ON_EXEC` werden beim Austausch des Prozess-Images geschlossen.

Dies verhindert, dass interne Handles unbeabsichtigt an ein neues Programm-Image weitergegeben werden.

Die Schließung erfolgt vor dem Start des neuen Images und muss mit dem Image-Austausch transaktional abgestimmt sein.

## 31. Pseudohandles

Pseudohandles referenzieren Objekte des aktuellen Ausführungskontexts.

Beispiele:

```c
#define NP_CURRENT_PROCESS   ((np_handle_t)-1)
#define NP_CURRENT_THREAD    ((np_handle_t)-2)
#define NP_CURRENT_JOB       ((np_handle_t)-3)
#define NP_CURRENT_NAMESPACE ((np_handle_t)-4)
```

Pseudohandles:

- sind keine Einträge in der Handle-Tabelle,
- werden beim Systemaufruf dynamisch aufgelöst,
- können nicht direkt geschlossen werden,
- können nicht unverändert übertragen werden.

## 32. Materialisierung von Pseudohandles

Ein Pseudohandle kann über eine explizite Operation in ein reguläres Handle umgewandelt werden.

```c
np_status_t np_handle_materialize(
    np_handle_t pseudo_handle,
    np_capability_rights_t requested_rights,
    np_handle_t* regular_handle
);
```

Die vergebenen Rechte richten sich nach dem aktuellen Sicherheitskontext und dürfen nicht automatisch administrativ sein.

## 33. Handle-Abfragen

```c
np_status_t np_handle_query(
    np_handle_t handle,
    np_handle_information_t* information
);
```

Die Abfrage kann enthalten:

- Objekttyp,
- aktuelle Handle-Rechte,
- Handle-Flags,
- Objektzustand,
- Vererbbarkeit,
- Übertragbarkeit.

Kerneladressen und interne Tabellenpositionen werden nicht ausgegeben.

## 34. Handle-Flags ändern

```c
np_status_t np_handle_set_flags(
    np_handle_t handle,
    uint32_t flags,
    uint32_t mask
);
```

Nicht alle Flags dürfen nachträglich verändert werden.

Insbesondere `KERNEL_ONLY`, `PROTECTED` und Sicherheitsattribute erfordern besondere Berechtigungen oder sind unveränderlich.

## 35. Rechte reduzieren

Ein Prozess kann die Rechte eines eigenen Handles dauerhaft reduzieren.

```c
np_status_t np_handle_restrict(
    np_handle_t handle,
    np_capability_rights_t reduced_rights
);
```

Es gilt:

```text
reduced_rights ⊆ current_rights
```

Entfernte Rechte können über dasselbe Handle nicht wiederhergestellt werden.

## 36. Handle-Tabellenwachstum

Handle-Tabellen wachsen bedarfsgesteuert.

Das Wachstum muss:

- definierte Prozesslimits beachten,
- Größenüberläufe verhindern,
- bestehende Handlewerte gültig halten,
- ohne Offenlegung teilweise initialisierter Segmente erfolgen.

Ein Tabellenwachstum darf im `NO_WAIT`-Kontext fehlschlagen.

## 37. Handle-Limits

Limits können gelten pro:

- Prozess,
- Job Object,
- Capability-Domain,
- Objekttyp,
- System.

```c
typedef struct np_handle_limits {
    uint64_t maximum_handles;
    uint64_t maximum_inheritable;
    uint64_t maximum_transfers_per_message;
} np_handle_limits_t;
```

Ein Prozess darf seine Limits reduzieren, aber nicht ohne Berechtigung erhöhen.

## 38. Prozessbeendigung

Beim Prozessende wird die Handle-Tabelle geschlossen.

Der Ablauf ist:

1. neue Handle-Erzeugungen verhindern,
2. neue Übertragungen abbrechen,
3. aktive Einträge sammeln,
4. Handles in definierter Reihenfolge schließen,
5. typspezifische Close-Operationen ausführen,
6. Objektreferenzen freigeben,
7. Tabellenspeicher freigeben.

Das Ende eines Prozesses darf keine Handles in anderen Prozessen schließen.

## 39. Reihenfolge beim Schließen

Handles werden nicht allein nach ihrem numerischen Wert geschlossen.

Abhängige Ressourcen können in Gruppen behandelt werden, beispielsweise:

1. ausstehende asynchrone Operationen,
2. IPC- und Netzwerkendpunkte,
3. Dateien und VFS-Objekte,
4. Geräte- und DMA-Objekte,
5. Jobs und Prozessgruppen,
6. verbleibende Objekte.

Objekttypen müssen auch bei abweichender Reihenfolge sicher geschlossen werden können.

## 40. Parallelität

Der Handle Manager muss folgende parallele Operationen sicher unterstützen:

- Auflösung und Schließung,
- Duplizierung und Schließung,
- Übertragung und Prozessbeendigung,
- Tabellenwachstum und Lookup,
- Rechtebeschränkung und Systemaufruf,
- Image-Austausch und Vererbung.

Temporäre Objektreferenzen verhindern Use-after-free nach dem Verlassen der Tabellensperre.

## 41. Synchronisation

Geeignete Mechanismen sind:

- segmentlokale Sperren,
- Lese-/Schreibsperren,
- atomare Eintragszustände,
- Generationszähler,
- verzögerte Freigabe alter Tabellensegmente.

Eine einzige globale Sperre für alle Handle-Tabellen ist unzulässig.

Sperren dürfen nicht über beliebige blockierende Objektoperationen gehalten werden.

## 42. Sperrreihenfolge

Bei Operationen mit zwei Handle-Tabellen werden Sperren in deterministischer Reihenfolge erworben.

Die Reihenfolge kann beispielsweise anhand einer eindeutigen Tabellenkennung erfolgen.

Alternativ wird ein transaktionales Reservierungsverfahren verwendet.

Dies verhindert Deadlocks bei gleichzeitigen gegenläufigen Übertragungen.

## 43. Handle-Caching

Kernelkomponenten dürfen aufgelöste Objekte nicht dauerhaft allein über einen gecachten Handle-Eintrag verwenden.

Für längere Nutzung ist eine eigene starke Objektreferenz erforderlich.

CPU-lokale Lookup-Caches sind zulässig, wenn:

- Generationen validiert werden,
- Schließungen sie sicher invalidieren,
- Rechteänderungen berücksichtigt werden,
- keine Kernelzeiger an Userspace gelangen.

## 44. System-Call-Integration

Jeder System-Call-Handler gibt den erwarteten Objekttyp und die erforderlichen Rechte an.

Beispiel:

```c
np_status_t np_sys_event_signal(
    np_syscall_context_t* context,
    np_handle_t event_handle
)
{
    np_kernel_object_t* object;

    np_status_t status = np_handle_resolve(
        context->handle_table,
        event_handle,
        NP_OBJECT_EVENT,
        NP_RIGHT_SIGNAL,
        &object
    );

    if (status != NP_OK) {
        return status;
    }

    status = np_event_signal((np_event_t*)object);
    np_object_release(object);

    return status;
}
```

## 45. IPC-Integration

Bei einer Handle-Übertragung über IPC muss der Kernel:

- alle Quellhandles vorprüfen,
- Zielkapazität reservieren,
- Rechte reduzieren,
- Zielhandles vorbereiten,
- Nachricht und Handles gemeinsam veröffentlichen,
- bei Fehlern alle Reservierungen zurückrollen.

Eine teilweise Übertragung ist nur zulässig, wenn sie ausdrücklich durch die IPC-Operation angefordert wurde.

## 46. Audit

Folgende Operationen können Auditereignisse erzeugen:

- Übertragung zwischen Sicherheitsdomains,
- Duplizierung administrativer Rechte,
- Öffnen geschützter Objekte,
- Änderung von Handle-Schutzflags,
- fehlgeschlagene Rechteerhöhungen,
- wiederholte Verwendung veralteter Handles.

Normale Handle-Lookups sollen nicht einzeln auditiert werden, sofern keine besondere Sicherheitsrichtlinie besteht.

## 47. Diagnose

Der Handle Manager stellt autorisierten Diagnosediensten bereit:

- Anzahl aktiver Handles,
- Tabellenauslastung,
- Handleanzahl nach Objekttyp,
- Rechteverteilung,
- Vererbungs- und Übertragungsflags,
- fehlgeschlagene Lookups,
- veraltete Handlezugriffe,
- Duplizierungen und Übertragungen,
- Schließfehler.

Unprivilegierte Prozesse dürfen nur ihre eigene Handle-Tabelle abfragen.

## 48. Statistik

```c
typedef struct np_handle_statistics {
    uint64_t active_handles;
    uint64_t peak_handles;

    uint64_t created_handles;
    uint64_t closed_handles;
    uint64_t duplicated_handles;
    uint64_t transferred_handles;

    uint64_t invalid_lookups;
    uint64_t stale_lookups;
    uint64_t access_denials;
} np_handle_statistics_t;
```

Statistiken sollen pro Prozess, Job und System verfügbar sein.

## 49. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_INVALID_HANDLE` | Handle existiert nicht |
| `NP_ERR_STALE_HANDLE` | Generation ist veraltet |
| `NP_ERR_HANDLE_TYPE` | Objekttyp stimmt nicht überein |
| `NP_ERR_HANDLE_CLOSING` | Handle wird bereits geschlossen |
| `NP_ERR_HANDLE_LIMIT` | Handle-Limit erreicht |
| `NP_ERR_ACCESS_DENIED` | erforderliches Recht fehlt |
| `NP_ERR_NOT_TRANSFERABLE` | Handle darf nicht übertragen werden |
| `NP_ERR_NOT_INHERITABLE` | Handle darf nicht vererbt werden |
| `NP_ERR_RIGHTS_ESCALATION` | angeforderte Rechte sind zu umfangreich |
| `NP_ERR_PROCESS_EXITED` | Zielprozess wurde beendet |
| `NP_ERR_TRANSACTION_ABORTED` | atomare Übertragung wurde zurückgerollt |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 50. Sicherheitsanforderungen

Der Handle Manager muss sicherstellen, dass:

- Handlewerte keine Kerneladressen enthalten,
- Handles nur in ihrer zugehörigen Tabelle gültig sind,
- Typ und Rechte bei jeder Auflösung geprüft werden,
- veraltete Handles durch Generationen erkannt werden,
- doppelte Schließungen keine Referenzzähler beschädigen,
- Rechte bei Duplizierung nicht unautorisiert erweitert werden,
- Handle-Übertragungen atomar und capability-geprüft erfolgen,
- nicht vererbbare Handles nicht an Kindprozesse gelangen,
- Pseudohandles nicht unverändert übertragen werden,
- Prozessbeendigung alle eigenen Handle-Referenzen freigibt.

## 51. Datenschutz

Handle-Tabellen können Informationen über geöffnete Dateien, Geräte, Dienste und Kommunikationspartner enthalten.

Daher gelten:

- Prozesse sehen standardmäßig nur ihre eigene Tabelle,
- globale Handle-Abfragen benötigen besondere Capabilities,
- Objektinhalte werden nicht über die Handle-Diagnose ausgegeben,
- Diagnose und Audit bleiben standardmäßig lokal,
- sensible Namen und Beziehungen können ausgeblendet werden.

## 52. Fehlerbehandlung

Ungültige oder veraltete Handles liefern einen definierten Fehler und dürfen den Kernelzustand nicht verändern.

Folgende Situationen gelten als schwere Kernelkonsistenzfehler:

- Handle-Eintrag verweist auf freigegebenen Objektspeicher,
- Generation stimmt mit einem unzulässig wiederverwendeten Eintrag überein,
- doppelte Freigabe einer Objektreferenz,
- beschädigte Handle-Tabelle,
- Rechteausweitung ohne Autorisierung,
- verlorenes Quell- und Zielhandle nach fehlgeschlagener Übertragung,
- Userspace erhält einen direkt nutzbaren Kernelzeiger.

## 53. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Erzeugung eines Handles,
2. Auflösung mit korrektem Typ und Recht,
3. Ablehnung eines ungültigen Handles,
4. Ablehnung eines falschen Objekttyps,
5. Ablehnung fehlender Rechte,
6. reguläre Schließung,
7. doppelte Schließung,
8. Generationswechsel nach Wiederverwendung,
9. Ablehnung eines veralteten Handles,
10. temporäre Referenz während paralleler Schließung,
11. Duplizierung innerhalb eines Prozesses,
12. Duplizierung mit reduzierten Rechten,
13. Ablehnung einer Rechteerhöhung,
14. Duplizierung in einen anderen Prozess,
15. Handle-Copy über IPC,
16. Handle-Move über IPC,
17. Rollback einer fehlgeschlagenen Übertragung,
18. Übertragung mehrerer Handles,
19. Vererbung an einen Kindprozess,
20. Schutz nicht vererbbarer Handles,
21. Close-on-Exec,
22. geschütztes Handle,
23. Materialisierung eines Pseudohandles,
24. Ablehnung des Schließens eines Pseudohandles,
25. Handle-Tabellenwachstum,
26. Einhaltung des Handle-Limits,
27. parallele Lookups auf mehreren CPUs,
28. parallele gegenläufige Übertragungen,
29. Prozessbeendigung mit offenen Handles,
30. Rechtebeschränkung eines bestehenden Handles,
31. Änderung zulässiger Handle-Flags,
32. CPU-lokales Lookup-Caching,
33. korrekte Audit- und Diagnosedaten,
34. vollständige Freigabe aller Objektreferenzen.

## 54. Verbindliche Invarianten

1. Jeder reguläre Handlewert ist genau einer Handle-Tabelle zugeordnet.
2. Ein gültiges Handle verweist auf genau ein Kernelobjekt.
3. Handlewerte enthalten keine direkt dereferenzierbaren Kerneladressen.
4. Jede Auflösung prüft Index, Generation, Zustand, Typ und Rechte.
5. Ein geschlossenes Handle kann nicht erneut verwendet werden.
6. Ein alter Handlewert darf nach Wiederverwendung seines Tabellenplatzes nicht wieder gültig werden.
7. Zielrechte einer normalen Duplizierung sind niemals größer als Quellrechte.
8. Jede aktive Handle-Referenz hält das Zielobjekt am Leben.
9. Eine Handle-Schließung gibt die zugehörige Objektreferenz höchstens einmal frei.
10. Eine fehlgeschlagene Move-Übertragung lässt das Quellhandle gültig.
11. Pseudohandles sind keine regulären Handle-Tabelleneinträge.
12. Beim Prozessende werden alle Handles des Prozesses kontrolliert geschlossen.

## 55. Referenzablauf einer Handle-Auflösung

```text
Systemaufruf erhält Handle
  -> Pseudohandle oder reguläres Handle unterscheiden
  -> Tabellenindex und Generation dekodieren
  -> Handle-Eintrag unter Synchronisation laden
  -> Zustand ACTIVE prüfen
  -> Generation vergleichen
  -> Objekttyp prüfen
  -> erforderliche Rechte prüfen
  -> starke temporäre Objektreferenz übernehmen
  -> Tabellensperre freigeben
  -> Objektoperation ausführen
  -> temporäre Referenz freigeben
```

## 56. Referenzablauf einer Handle-Übertragung

```text
Sender übergibt Handleliste
  -> Quellhandles vollständig validieren
  -> Transferrechte und Zielfähigkeit prüfen
  -> Zieltabellenplätze reservieren
  -> Rechte für Zielhandles reduzieren
  -> starke Objektreferenzen übernehmen
  -> Nachricht und Zielhandles atomar veröffentlichen
  -> bei MOVE Quellhandles schließen
  -> bei Fehler alle Zielreservierungen zurückrollen
  -> Ergebnis an Sender zurückgeben
```

## 57. Zusammenfassung

Der NovaOS Handle Manager stellt generationsgeschützte und capability-basierte Referenzen auf Kernelobjekte bereit.

Jeder Prozess besitzt eine eigene Handle-Tabelle. Typ, Generation und Rechte werden bei jeder Verwendung geprüft, während temporäre Objektreferenzen parallele Schließungen sicher überstehen.

Duplizierung, Vererbung und IPC-Übertragung können Rechte ausschließlich innerhalb autorisierter Grenzen delegieren. Atomare Transferregeln verhindern, dass Handles bei fehlgeschlagenen Operationen verloren gehen oder unkontrolliert zwischen Sicherheitsdomains wechseln.