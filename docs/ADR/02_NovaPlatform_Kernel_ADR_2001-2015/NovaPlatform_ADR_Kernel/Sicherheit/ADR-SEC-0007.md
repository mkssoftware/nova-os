# ADR-SEC-0007 – Capability-basierte Zugriffskontrolle

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0007 |
| Titel | Capability-basierte Zugriffskontrolle |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Kernelsicherheit und Ressourcenautorisierung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-SEC-0006, ADR-IPC-0001, ADR-VFS-0009 |
| Nachfolgende Spezifikation | NPSPEC-SEC-CAPABILITY-ACCESS-CONTROL |

---

## 1. Kontext

NovaOS benötigt ein einheitliches Sicherheitsmodell für den Zugriff auf Kernelobjekte und Systemressourcen.

Zu diesen Ressourcen gehören:

- Prozesse und Threads
- Speicherobjekte
- IPC-Endpunkte
- Dateien und Verzeichnisse
- Geräte
- Interrupts und DMA-Kontexte
- Sockets
- Namespaces
- Debug-Sessions
- Diagnose- und Sicherheitsdienste

Klassische globale Kennungen oder ausschließlich benutzerbasierte Rechte reichen für eine feingranulare Isolation nicht aus.

---

## 2. Problemstellung

Wenn die Kenntnis einer Prozess-ID, eines Pfades oder eines Objektnamens für den Zugriff ausreichen würde, entstünden weitreichende und schwer kontrollierbare Berechtigungen.

NovaOS muss sicherstellen, dass:

- jeder Zugriff ausdrücklich autorisiert wird,
- Rechte auf einzelne Objekte begrenzt werden,
- Prozesse nur notwendige Fähigkeiten erhalten,
- Delegation kontrolliert möglich ist,
- Rechte nicht unbemerkt erweitert werden,
- widerrufene und veraltete Handles zuverlässig erkannt werden,
- Ressourcenkennungen keine Berechtigung darstellen.

---

## 3. Entscheidung

NovaOS verwendet Capabilities als primären Laufzeitnachweis für den Zugriff auf Kernelobjekte.

Eine Capability verbindet:

- eine geschützte Referenz auf ein Kernelobjekt,
- eine Menge ausdrücklich gewährter Rechte,
- Übertragungs- und Vererbungsregeln,
- eine Objekt- und Handle-Generation,
- optionale Widerrufs- und Ablaufregeln.

Der Kernel prüft bei jeder geschützten Operation die verwendete Capability und das für die Operation notwendige Recht.

---

## 4. Grundprinzip

```text
Prozess
   |
   v
Capability Handle
   |
   v
Capability Space
   |
   v
Objektreferenz + Rechte
   |
   v
Kernelobjekt
```

Der im Userspace sichtbare Handlewert enthält keine direkt nutzbare Kerneladresse.

Ein Handle ist nur innerhalb des zugehörigen Capability Space gültig.

---

## 5. Capability-Eintrag

```c
typedef struct capability_entry {
    uint64_t object_id;
    uint64_t object_generation;
    uint64_t rights;
    uint32_t flags;
    uint32_t handle_generation;
} capability_entry_t;
```

Der tatsächliche Kernel-Eintrag enthält zusätzlich:

- sichere Objektreferenz,
- Eigentümer-Capability-Space,
- Widerrufszustand,
- Delegationsinformationen,
- Auditklassifikation,
- Referenzanzahl.

Diese internen Daten werden nicht direkt an den Userspace ausgegeben.

---

## 6. Capability Space

Jeder Prozess besitzt einen eigenen Capability Space.

Dieser verwaltet:

- gültige Handles,
- zugehörige Kernelobjekte,
- gewährte Rechte,
- Handle-Generationen,
- Referenzen,
- Übertragungsflags,
- Widerrufszustände.

Gleiche numerische Handlewerte in verschiedenen Prozessen können auf unterschiedliche Objekte verweisen.

Ein Prozess darf den Capability Space eines anderen Prozesses nicht ohne besondere Debug- oder Verwaltungscapability untersuchen.

---

## 7. Handle-Sicherheit

Ein Handle besteht logisch aus:

```text
Tabellenindex + Handle-Generation
```

Die Generation verhindert, dass ein alter Handle nach Freigabe und Wiederverwendung des Tabelleneintrags auf ein neues Objekt zugreift.

Bei jeder Auflösung prüft der Kernel:

1. gültiger Prozesskontext,
2. Index innerhalb der Tabelle,
3. übereinstimmende Handle-Generation,
4. nicht widerrufener Eintrag,
5. gültige Objektgeneration,
6. erforderliche Rechte,
7. gültiger Objektzustand.

---

## 8. Rechte

Jeder Kernelobjekttyp definiert eigene Rechte.

Gemeinsame Rechte können sein:

| Recht | Bedeutung |
|---|---|
| `RIGHT_INSPECT` | freigegebene Metadaten lesen |
| `RIGHT_READ` | Daten lesen |
| `RIGHT_WRITE` | Daten verändern |
| `RIGHT_CONTROL` | Objektzustand kontrollieren |
| `RIGHT_WAIT` | auf Objektzustand warten |
| `RIGHT_SIGNAL` | Objekt signalisieren |
| `RIGHT_DUPLICATE` | Capability duplizieren |
| `RIGHT_TRANSFER` | Capability übertragen |
| `RIGHT_ADMIN` | privilegierte Verwaltung |

Objektspezifische Rechte werden getrennt definiert, beispielsweise `PROCESS_RIGHT_TERMINATE` oder `SOCKET_RIGHT_CONNECT`.

---

## 9. Least Authority

NovaOS folgt dem Prinzip der geringstmöglichen Berechtigung.

Ein Prozess erhält nur die Capabilities, die er für seine Aufgabe benötigt.

Beispiele:

- Ein Logger erhält Schreibzugriff auf einen Logkanal, aber keinen Zugriff auf fremde Prozesse.
- Ein Grafikdienst erhält Zugriff auf Display- und Grafikobjekte, aber nicht auf Blockgeräte.
- Ein Treiber erhält nur die MMIO-, Interrupt- und DMA-Ressourcen seines Geräts.
- Eine Anwendung erhält nur ihre freigegebenen Dateien und Netzwerkrechte.
- Ein Updater erhält nur Zugriff auf vorbereitete Systemgenerationen.

Breite globale Administratorhandles sollen vermieden werden.

---

## 10. Capability-Erzeugung

Capabilities entstehen nur durch kontrollierte Kerneloperationen:

- Erzeugung eines neuen Kernelobjekts
- Öffnen eines autorisierten Named Object
- ACL- und Richtlinienprüfung
- Prozessstart
- Capability-Duplikation
- IPC-Übertragung
- administrative Delegation
- Erzeugung eines eingeschränkten Child-Kontexts

Der Userspace kann keine gültige Capability allein durch das Erzeugen eines Handlewerts herstellen.

---

## 11. Duplizierung

Eine Capability kann mit denselben oder reduzierten Rechten dupliziert werden:

```c
status_t capability_duplicate(
    cap_handle_t source_handle,
    uint64_t requested_rights,
    uint32_t flags,
    cap_handle_t *new_handle
);
```

Voraussetzungen:

- Quell-Capability besitzt `RIGHT_DUPLICATE`,
- angeforderte Rechte sind Teil der Quellrechte,
- Objekt und Sicherheitsrichtlinie erlauben die Duplizierung,
- Capability ist nicht widerrufen.

Eine Duplizierung darf Rechte niemals erweitern.

---

## 12. Übertragung

Capabilities können über kontrollierte IPC-Mechanismen an andere Prozesse übertragen werden.

Unterstützte Modi:

| Modus | Bedeutung |
|---|---|
| Copy | Absender und Empfänger besitzen anschließend eine Capability |
| Move | Capability wird vom Absender zum Empfänger verschoben |
| Reduce | Empfänger erhält eine Capability mit reduzierten Rechten |
| Delegate | Capability wird mit zusätzlichen Delegationsgrenzen übertragen |

Nachricht und Capability-Anhänge werden atomar übertragen.

Schlägt die Capability-Übertragung fehl, wird auch die zugehörige IPC-Nachricht nicht teilweise zugestellt.

---

## 13. Delegation

Eine delegierte Capability kann zusätzliche Grenzen besitzen:

- Zielprozess oder Sicherheitsdomäne
- maximale Rechte
- Übertragungsverbot
- Duplizierungsverbot
- Ablaufzeit
- maximale Delegationstiefe
- Bindung an Session oder Job
- Widerrufsbeziehung zum Aussteller

Eine delegierte Capability darf nicht aus ihrer vorgesehenen Sicherheitsdomäne heraus übertragen werden, wenn dies durch ihre Richtlinie verboten ist.

---

## 14. Vererbung

Bei der Prozesserzeugung werden Capabilities nicht automatisch vollständig vererbt.

Der Elternprozess oder Process Manager erstellt eine explizite Übergabeliste.

Dabei gelten:

- Close-on-Execute als sicherer Standard für sensible Handles,
- ausdrückliche Allowlist für Sandbox-Prozesse,
- Reduktion der übertragenen Rechte,
- Prüfung von Namespace-Grenzen,
- Prüfung der Job- und Sicherheitsrichtlinie.

Unbeabsichtigte Capability-Vererbung gilt als Sicherheitsfehler.

---

## 15. Attenuation

Attenuation bezeichnet die Erzeugung einer Capability mit schwächeren Rechten oder zusätzlichen Grenzen.

Beispiel:

```text
Datei-Capability:
READ | WRITE | TRANSFER

reduzierte Capability:
READ
```

Weitere mögliche Einschränkungen:

- nur bestimmter Dateibereich
- nur Append
- nur bestimmte RPC-Methoden
- nur bestimmte Netzwerkziele
- nur lesendes Shared-Memory-Mapping
- begrenzte Lebenszeit

Attenuation ist nicht umkehrbar, ohne erneut eine stärkere autorisierte Capability zu erhalten.

---

## 16. Widerruf

NovaOS unterstützt den Widerruf von Capabilities für sicherheitskritische Ressourcen.

Mögliche Modelle:

- einzelner Handle-Widerruf
- Widerruf aller Ableitungen
- Widerruf einer Delegationsgruppe
- Objektweiter Widerruf
- Widerruf durch Namespace- oder Sessionende
- zeitbasierter Ablauf

```c
status_t capability_revoke(
    cap_handle_t authority_handle,
    uint32_t revoke_mode
);
```

Der Widerruf benötigt eine dafür autorisierte Capability.

---

## 17. Widerrufssemantik

Nach einem Widerruf gilt:

- neue Operationen werden abgelehnt,
- blockierte Operationen werden aufgeweckt,
- noch nicht gestartete asynchrone Operationen werden abgebrochen,
- laufende Operationen erreichen einen definierten sicheren Punkt,
- temporäre Kernelreferenzen werden kontrolliert freigegeben,
- der Handle liefert `STATUS_REVOKED`.

Bereits dauerhaft abgeschlossene Operationen werden durch Widerruf nicht rückgängig gemacht.

---

## 18. Objektlebenszeit

Capability-Lebenszeit und Objektlebenszeit sind getrennt.

Ein Kernelobjekt bleibt bestehen, solange mindestens eine gültige Referenz vorhanden ist durch:

- Capability
- aktives Mapping
- laufende Kerneloperation
- interne Subsystemreferenz
- registrierte asynchrone Operation

Das Schließen der letzten externen Capability zerstört ein Objekt erst, wenn alle internen Referenzen freigegeben wurden.

Ein entferntes Gerät oder ungemountetes Dateisystem kann weiterhin ein Objekt besitzen, dessen Operationen nur noch Fehler liefern.

---

## 19. Named Objects

Named Objects ermöglichen die Auffindbarkeit eines Objekts, erteilen aber keinen Zugriff.

Der Ablauf ist:

1. Namespace-Capability prüfen.
2. Namen auflösen.
3. Namensrichtlinie prüfen.
4. Objektstatus prüfen.
5. angeforderte Rechte begrenzen.
6. neue Capability erzeugen.

Ein Name, Pfad oder eine Objekt-ID ist niemals ein Ersatz für eine Capability.

---

## 20. ACL-Integration

Persistente ACLs bestimmen bei VFS- und vergleichbaren Namensoperationen, welche Capability-Rechte neu ausgegeben werden dürfen.

Nach erfolgreichem Öffnen ist die Capability der unmittelbare Zugriffsnachweis.

Eine spätere ACL-Änderung widerruft bestehende Capabilities nicht automatisch.

Für dringende Sicherheitsfälle muss ein expliziter Capability-Widerruf ausgeführt werden.

---

## 21. Benutzer- und Sicherheitskontexte

Capabilities werden zusätzlich durch übergeordnete Sicherheitsrichtlinien begrenzt.

Mögliche Kontexte:

- Benutzer
- Gruppe
- Anwendung
- Systemdienst
- Job Object
- Session
- Sandbox
- Netzwerk-Namespace
- Recovery
- Secure-Debugging-Modus

Eine Capability kann keine Operation erlauben, die durch eine nicht delegierbare höhere Systemrichtlinie ausdrücklich verboten ist.

---

## 22. Privilegierte Systemdienste

NovaOS vermeidet einen einzelnen allmächtigen Systemprozess.

Stattdessen erhalten privilegierte Dienste getrennte Capability-Sets.

Beispiele:

- Device Manager
- Mount Manager
- Network Manager
- Power Manager
- Auditdienst
- Debugdienst
- Update Manager
- Recovery Manager

Ein kompromittierter Dienst soll nur die Ressourcen gefährden können, für die er tatsächlich Capabilities besitzt.

---

## 23. Treiber

Treiber erhalten ausschließlich Capabilities für:

- zugewiesenes Gerät
- MMIO-Bereiche
- I/O-Ports
- Interrupts
- DMA-Kontext
- notwendige IPC-Endpunkte
- erlaubte Diagnosekanäle

Treiber dürfen keine Hardware durch globales Scannen oder physische Adresskenntnis übernehmen.

Beim Treiberabsturz oder Geräteentfernen werden zugehörige Ressourcen-Capabilities widerrufen.

---

## 24. Debugging

Debug-Capabilities sind besonders sicherheitskritisch.

Ein Debugger benötigt getrennte Rechte für:

- Attach
- Prozesskontrolle
- Speicherlesen
- Speicherschreiben
- Registerzugriff
- Breakpoints
- geschützte Prozesse
- Kernel-Debugging

Eine gewöhnliche Prozess-Capability gewährt keinen Debugzugriff.

Kernel-Debugging bleibt zusätzlich durch Secure Debugging geschützt.

---

## 25. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Handle oder Generation ist ungültig |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_REVOKED` | Capability wurde widerrufen |
| `STATUS_OBJECT_CLOSED` | Objekt ist geschlossen |
| `STATUS_OBJECT_REMOVED` | zugrunde liegende Ressource wurde entfernt |
| `STATUS_RIGHTS_NOT_REDUCIBLE` | angeforderte Rechte sind ungültig |
| `STATUS_TRANSFER_DENIED` | Capability darf nicht übertragen werden |
| `STATUS_DELEGATION_LIMIT` | Delegationsgrenze wurde erreicht |
| `STATUS_EXPIRED` | Capability ist abgelaufen |
| `STATUS_LIMIT_EXCEEDED` | Capability-Limit wurde überschritten |
| `STATUS_POLICY_DENIED` | höhere Sicherheitsrichtlinie verbietet die Operation |

---

## 26. Ressourcenbegrenzung

Capability Spaces sind begrenzt.

Limits gelten für:

- Handles pro Prozess
- delegierte Capabilities
- Widerrufsgruppen
- Transferanhänge pro IPC-Nachricht
- Capability-Duplikationen
- Ablauf- und Timerobjekte
- administrative Capabilities
- Gesamtreferenzen auf ein Objekt

Ein Prozess darf den Kernel nicht durch massenhaft erzeugte oder zyklisch übertragene Capabilities überlasten.

---

## 27. Sicherheit

Für das Capability-System gelten folgende Regeln:

- Handlewerte enthalten keine Kernelzeiger.
- Jede geschützte Operation prüft Objekttyp und Rechte.
- Rechte können nur reduziert, niemals implizit erweitert werden.
- Handles verwenden Generationen gegen Wiederverwendung.
- Capability Spaces sind pro Prozess isoliert.
- Übertragung und Vererbung erfolgen ausdrücklich.
- Widerruf wird atomar sichtbar.
- Capability-Metadaten sind für den Userspace nicht direkt veränderbar.
- Objekt-IDs und Namen erteilen keine Rechte.
- höhere nicht delegierbare Systemrichtlinien bleiben wirksam.

---

## 28. Audit

Auditiert werden insbesondere:

- Ausgabe administrativer Capabilities
- Delegation über Sicherheitsdomänen
- Widerruf sicherheitskritischer Rechte
- fehlgeschlagene Rechteerweiterungen
- Capability-Übertragung an geschützte Prozesse
- Debug- und Kernel-Capabilities
- Änderungen globaler Sicherheitsrichtlinien
- ungewöhnlich hohe Capability-Erzeugungsraten

Gewöhnliche Datei- oder IPC-Handleoperationen werden nicht einzeln auditiert.

Objektinhalte und geheime Nutzdaten werden nicht in Auditdatensätze übernommen.

---

## 29. Diagnose

Autorisierte Diagnosewerkzeuge dürfen anzeigen:

- Capability-Anzahl
- Objekttyp
- gewährte Rechte
- Übertragungsflags
- Widerrufszustand
- Ablaufzeit
- Delegationsbeziehung
- Objekt- und Handle-Generation

Kernelzeiger und interne Tabellenadressen bleiben verborgen.

Unprivilegierte Prozesse dürfen nur ihren eigenen Capability Space in begrenzter Form untersuchen.

---

## 30. Alternative Ansätze

### 30.1 Globale Objekt-Handles

**Vorteile:**

- einfache systemweite Referenzierung

**Nachteile:**

- Kenntnis eines Handles könnte Zugriff ermöglichen
- keine prozesslokale Isolation

**Entscheidung:** Abgelehnt.

### 30.2 Ausschließlich benutzer- und gruppenbasierte Rechte

**Vorteile:**

- bekanntes Sicherheitsmodell

**Nachteile:**

- zu grob für Dienste, Geräte und Sandboxen
- schwierige kontrollierte Delegation

**Entscheidung:** Benutzerrechte und ACLs ergänzen die Capability-Ausgabe, ersetzen sie aber nicht.

### 30.3 Ein globaler Administratorprozess

**Vorteile:**

- einfache zentrale Verwaltung

**Nachteile:**

- vollständige Systemkompromittierung bei einem Dienstfehler
- widerspricht Least Authority

**Entscheidung:** Privilegien werden auf getrennte Systemdienste verteilt.

---

## 31. Konsequenzen

### Positive Konsequenzen

- feingranulare Ressourcenautorisierung
- klare Trennung von Identität und Berechtigung
- kontrollierte Delegation
- sichere Rechteabschwächung
- Unterstützung für Sandboxen und isolierte Treiber
- Widerruf sicherheitskritischer Zugriffe
- reduzierte Auswirkungen kompromittierter Dienste

### Negative Konsequenzen

- höhere Komplexität als globale Handles
- Capability-Lebenszeiten müssen sorgfältig verwaltet werden
- Widerruf laufender Operationen benötigt definierte sichere Punkte
- Kompatibilitätsschichten müssen klassische Rechte auf Capabilities abbilden

### Risiken

- unbeabsichtigte Capability-Vererbung
- zu weitreichende Capability-Sets
- fehlende Widerrufsbeziehungen
- Handle-Reuse-Fehler
- Capability-Leaks
- Umgehung höherer Richtlinien durch fehlerhafte Subsysteme

---

## 32. Implementierungsrichtlinien

- Alle öffentlichen Kernelobjektzugriffe verwenden Capabilities.
- Capability Spaces bleiben pro Prozess getrennt.
- Handlewerte enthalten Index und Generation.
- Rechte werden bei jeder geschützten Operation geprüft.
- Duplikation und Übertragung können Rechte nur reduzieren.
- Prozessstarts verwenden explizite Capability-Allowlists.
- Sicherheitskritische Delegationen erhalten Widerrufsbeziehungen.
- Objekt- und Handle-Lebenszeiten werden getrennt verwaltet.
- IDs, Namen und Pfade sind niemals Zugriffsnachweise.
- Systemdienste erhalten minimale, auf ihre Aufgabe begrenzte Capability-Sets.
- Fehlerpfade dürfen keine teilweise übertragenen Capabilities hinterlassen.

---

## 33. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Capability-Erzeugung und -Schließung
- prozesslokale Handle-Isolation
- Handle- und Objektgenerationen
- Rechteprüfungen
- Duplizierung mit reduzierten Rechten
- Copy-, Move- und Reduce-Übertragung
- Vererbung und Close-on-Execute
- Delegationsgrenzen
- Ablaufzeiten
- einzelner und rekursiver Widerruf
- laufende Operationen während Widerruf
- Objektzerstörung
- Capability-Space-Limits
- Treiber- und Debug-Capabilities
- Fuzzing von Handlewerten und Transferdeskriptoren

---

## 34. Verbindliche Invarianten

1. Capabilities sind der primäre Laufzeitnachweis für Kernelobjektzugriffe.
2. Jeder Prozess besitzt einen getrennten Capability Space.
3. Handlewerte und Objekt-IDs stellen keine Zugriffsberechtigung dar.
4. Handle- und Objektgenerationen verhindern veraltete Zugriffe.
5. Capability-Rechte können niemals implizit erweitert werden.
6. Duplikation und Übertragung dürfen Rechte nur erhalten oder reduzieren.
7. Vererbung erfolgt ausschließlich über explizite Regeln.
8. Widerruf verhindert alle neuen Operationen über die betroffene Capability.
9. ACLs und Namensrichtlinien steuern die Capability-Ausgabe, ersetzen sie aber nicht.
10. Eine Capability kann eine höhere nicht delegierbare Systemrichtlinie nicht umgehen.
11. Userspace kann Capability-Metadaten nicht direkt verändern.
12. Kernelzeiger werden niemals als öffentliche Handles verwendet.

---

## 35. Zusammenfassung

NovaOS verwendet capability-basierte Zugriffskontrolle als einheitliches Laufzeit-Sicherheitsmodell für Kernelobjekte und Systemressourcen.

Jeder Prozess besitzt einen isolierten Capability Space. Capabilities verbinden eine sichere Objektreferenz mit ausdrücklich gewährten Rechten sowie Regeln für Duplizierung, Übertragung, Vererbung und Widerruf.

ACLs, Identitäten und Systemrichtlinien bestimmen, welche Capabilities ausgegeben werden dürfen. Der tatsächliche Zugriff erfolgt anschließend ausschließlich über diese begrenzten Capabilities. Dadurch werden Least Authority, sichere Delegation und die Isolation von Anwendungen, Diensten und Treibern konsequent im Kernel durchgesetzt.