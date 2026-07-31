# ADR-SEC-0010 – Secure Debugging

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0010 |
| Titel | Secure Debugging |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Sicherheit / Kernel / Debugging / Diagnose |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0004, ADR-SEC-0005, ADR-SEC-0006, ADR-SEC-0008, ADR-SEC-0009 |
| Nachfolgende Spezifikation | NPSPEC-SECUREDEBUG-0001 |

---

## 1. Kontext

NovaOS benötigt leistungsfähige Debugfunktionen für:

- Kernelentwicklung,
- Treiberentwicklung,
- Systemdienstdiagnose,
- Userspace-Anwendungen,
- Crash-Analyse,
- Hardwarebring-up,
- Performanceanalyse,
- Recovery und Support.

Ein Debugger kann jedoch besonders weitreichende Zugriffe erhalten.

Abhängig von seinem Ziel kann er:

- Prozesse anhalten,
- Register lesen und verändern,
- Speicher lesen und schreiben,
- Breakpoints setzen,
- Kontrollfluss verändern,
- Kernelcode patchen,
- Sicherheitsentscheidungen beobachten,
- Seitentabellen untersuchen,
- Geheimnisse und Benutzerdaten auslesen.

Ein unkontrollierter Debugzugang kann nahezu sämtliche Sicherheitsmechanismen umgehen.

Dies betrifft insbesondere:

- Secure Kernel Startup,
- Kernel ASLR,
- Stack Protector,
- NX und W^X,
- SMEP und SMAP,
- Kernel Isolation,
- Capabilities,
- Audit,
- Verschlüsselung,
- Prozess- und Benutzerdatenschutz.

Debugging darf deshalb nicht als gewöhnliche Komfortfunktion behandelt werden. Es benötigt ein eigenes Sicherheits-, Autorisierungs-, Sitzungs- und Auditmodell.

---

## 2. Problemstellung

NovaOS muss unterschiedliche Debuganforderungen unterstützen:

- Ein Entwickler möchte seinen eigenen Userspace-Prozess debuggen.
- Ein Administrator möchte einen Systemdienst untersuchen.
- Ein Kernelentwickler benötigt Kerneldebugging.
- Ein Supporttechniker benötigt begrenzten Diagnosezugriff.
- Ein Recovery-System muss Crash-Dumps analysieren.
- Ein eingebettetes Gerät benötigt eventuell einen Hardware-Debugport.
- Ein Produktionssystem darf nicht unbemerkt in einen unsicheren Debugzustand wechseln.

Diese Fälle besitzen sehr unterschiedliche Risiken.

Ein allgemeines Administratorrecht reicht nicht aus, um sämtliche Debugfunktionen sicher abzubilden. Insbesondere Kerneldebugging bedeutet faktisch Kontrolle über die höchste Softwareprivilegienstufe.

---

## 3. Entscheidungsziele

Secure Debugging soll:

1. Debugzugriffe standardmäßig verweigern,
2. Userspace-, Systemdienst- und Kerneldebugging trennen,
3. ziel- und operationsbezogene Capabilities verwenden,
4. Kerneldebugging in Produktionssystemen besonders schützen,
5. lokale physische Autorisierung unterstützen,
6. Remote-Debugging authentisieren und verschlüsseln,
7. Debugsitzungen zeitlich begrenzen,
8. sämtliche privilegierten Debugvorgänge auditieren,
9. Kerneladressen und Geheimnisse vor unberechtigter Einsicht schützen,
10. W^X, NX und Kernel Isolation nicht dauerhaft deaktivieren,
11. Hardware-Debugports kontrollieren,
12. den Debugzustand im Trust State sichtbar machen.

---

## 4. Begriffsdefinitionen

### 4.1 Debug Target

Ein Debug Target ist das Objekt, das untersucht wird.

Mögliche Ziele:

- Thread,
- Prozess,
- Job Object,
- Systemdienst,
- Kernelmodul,
- gesamter Kernel,
- virtuelle Maschine,
- Gerät,
- Crash Dump.

---

### 4.2 Debugger

Der Debugger ist die autorisierte Komponente, die eine Debugsitzung steuert.

---

### 4.3 Debug Session

Eine Debug Session ist ein zeitlich begrenztes, referenzgezähltes Kernelobjekt, das Debugger, Ziel, Rechte, Policy und Auditkontext verbindet.

---

### 4.4 Live Debugging

Live Debugging untersucht oder verändert ein laufendes Ziel.

---

### 4.5 Post-Mortem Debugging

Post-Mortem Debugging analysiert einen Crash Dump oder zuvor gespeicherten Zustand, ohne den ursprünglichen laufenden Kernel zu verändern.

---

### 4.6 Kernel Debugging

Kernel Debugging ermöglicht Zugriff auf privilegierte Kernelzustände.

Es besitzt eine wesentlich höhere Sicherheitswirkung als normales Prozessdebugging.

---

### 4.7 Debug Unlock

Debug Unlock ist die kontrollierte Aktivierung einer normalerweise gesperrten Debugfunktion.

---

### 4.8 Debug Authorization Token

Ein Debug Authorization Token ist ein signiertes, begrenztes Autorisierungsobjekt für eine konkrete Debugaktion oder -sitzung.

---

### 4.9 Physical Presence

Physical Presence ist eine lokale, nicht ausschließlich remote auslösbare Bestätigung am Gerät.

Beispiele:

- Bootmenübestätigung,
- Hardwaretaste,
- lokaler Recovery-Code,
- Firmwaredialog,
- physischer Service-Schalter.

---

## 5. Bedrohungsmodell

Secure Debugging schützt insbesondere gegen:

- unautorisierte Prozessinspektion,
- unbemerkten Kernel-Speicherzugriff,
- Remote-Debugzugriff ohne Authentisierung,
- dauerhafte Debug-Hintertüren,
- Missbrauch von Breakpoints zur Codeänderung,
- Offenlegung von Schlüsseln und Tokens,
- Umgehung von KASLR, NX oder Kernel Isolation,
- Debugzugriff nach Ablauf der Autorisierung,
- Verwendung eines Debugtokens auf einem anderen Gerät,
- unkontrollierte Hardware-Debugports.

Secure Debugging schützt nicht vollständig gegen:

- kompromittierte Debugsignaturschlüssel,
- physische invasive Hardwareangriffe,
- kompromittierte Firmware unterhalb des Kernels,
- einen bereits vollständig kompromittierten Kernel,
- absichtlich unsicher gebaute Development-Images,
- externe Hardwareprobes mit direktem Buszugriff.

---

## 6. Betrachtete Alternativen

### 6.1 Debugging immer aktiviert

Alle Systeme besitzen jederzeit aktive Debugschnittstellen.

#### Vorteile

- einfache Entwicklung,
- schnelle Fehleranalyse,
- keine gesonderte Aktivierung.

#### Nachteile

- erhebliche Angriffsoberfläche,
- mögliche Remote- oder lokale Privilegieneskalation,
- Kernel- und Benutzergeheimnisse können ausgelesen werden,
- ungeeignet für Produktionssysteme.

Diese Alternative wird verworfen.

---

### 6.2 Debugging nur in speziellen Builds

Produktions-Builds enthalten keinerlei Debugfunktionen.

#### Vorteile

- kleine Angriffsoberfläche,
- klare Trennung,
- keine versehentliche Produktionsaktivierung.

#### Nachteile

- eingeschränkte Produktionsdiagnose,
- erschwerte Analyse seltener Hardwarefehler,
- zusätzlicher Build- und Freigabeaufwand,
- keine kontrollierte Supportdiagnose.

Diese Alternative wird als alleinige Lösung verworfen.

---

### 6.3 Allgemeines Administratorrecht

Administratoren dürfen sämtliche Prozesse und den Kernel debuggen.

#### Vorteile

- einfaches Berechtigungsmodell,
- bekannte klassische Semantik.

#### Nachteile

- zu große Rechtebündelung,
- Administrator wird automatisch zum Kernelcontroller,
- keine Ziel- oder Zeitbegrenzung,
- schwache Nachvollziehbarkeit.

Diese Alternative wird verworfen.

---

### 6.4 Capability-basierte Debugsitzungen

Jede Debugsitzung erhält ausdrücklich begrenzte Rechte, ein Ziel, eine Laufzeit und eine Sicherheitsrichtlinie.

#### Vorteile

- feingranulare Autorisierung,
- Trennung von Userspace- und Kerneldebugging,
- gute Auditierbarkeit,
- zeitliche Begrenzung,
- Remote-Debugging kontrollierbar,
- Produktionsdiagnose ohne permanent offenen Zugang.

#### Nachteile

- höhere Implementierungskomplexität,
- Schlüssel- und Tokenverwaltung,
- Debugwerkzeuge müssen das Sitzungsmodell unterstützen,
- Kerneldebugging bleibt grundsätzlich risikoreich.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS verwendet capability-basierte, explizit autorisierte und zeitlich begrenzte Debugsitzungen.

Debugging ist standardmäßig deaktiviert, bis:

1. ein zulässiges Debug Target bestimmt wurde,
2. der Debugger authentisiert wurde,
3. eine passende Capability vorliegt,
4. die aktive Sicherheitsrichtlinie zustimmt,
5. gegebenenfalls Physical Presence bestätigt wurde,
6. eine Debug Session erzeugt wurde.

Kerneldebugging besitzt eine eigene, strengere Autorisierung und kann den Trust State des laufenden Systems herabstufen.

---

## 8. Debugklassen

NovaOS unterscheidet mindestens folgende Debugklassen:

| Debugklasse | Ziel | Sicherheitsniveau |
|---|---|---|
| `SELF_DEBUG` | eigener Prozess | normal |
| `PROCESS_DEBUG` | fremder Prozess | erhöht |
| `JOB_DEBUG` | mehrere Prozesse eines Jobs | erhöht |
| `SERVICE_DEBUG` | privilegierter Systemdienst | hoch |
| `MODULE_DEBUG` | Kernelmodul | sehr hoch |
| `KERNEL_READONLY_DEBUG` | Kernelzustand nur lesend | sehr hoch |
| `KERNEL_LIVE_DEBUG` | laufender Kernel mit Schreibrechten | maximal kritisch |
| `POST_MORTEM_DEBUG` | geschützter Crash Dump | abhängig vom Dump |
| `HARDWARE_DEBUG` | JTAG, SWD oder Plattformport | maximal kritisch |

---

## 9. Self Debugging

Ein Prozess darf sich grundsätzlich selbst debuggen, wenn seine Prozesspolicy dies erlaubt.

Mögliche Funktionen:

- eigene Threads anhalten,
- eigene Register lesen,
- eigene Breakpoints setzen,
- eigenen Speicher untersuchen,
- eigene Debugsymbole verwenden.

Self Debugging berechtigt nicht zum:

- Zugriff auf fremde Prozesse,
- Lesen von Kerneladressen,
- Umgehen von Sandboxgrenzen,
- Erhöhen eigener Capabilities,
- Verändern von Kernelcode.

---

## 10. Fremdprozess-Debugging

Das Debuggen eines fremden Prozesses benötigt:

- Handle auf das Ziel,
- Debug-Capability für dieses Ziel,
- kompatible Sicherheitsdomäne oder explizite Delegation,
- Zustimmung der Job- und Prozesspolicy,
- erfolgreiche Sitzungserzeugung.

Ein gemeinsamer Benutzeraccount allein ist keine ausreichende Autorisierung für beliebiges Prozessdebugging.

---

## 11. Job-Debugging

Ein Debugger kann eine Debug Session für ein Job Object erhalten.

Diese kann Prozesse erfassen, die:

- bereits Mitglied sind,
- während der Sitzung neu erzeugt werden,
- ausdrücklich in der Sessionpolicy enthalten sind.

Die Rechte bleiben durch:

- Jobgrenzen,
- Sicherheitsdomänen,
- Prozessausnahmen,
- Capability-Beschränkungen

begrenzt.

Kernel- oder Systemprozesse werden nicht allein durch ihre Jobzugehörigkeit debugfähig.

---

## 12. Systemdienst-Debugging

Privilegierte Systemdienste können Zugriff auf:

- Authentisierungsdaten,
- Capability-Entscheidungen,
- Verschlüsselungsschlüssel,
- Benutzerinhalte,
- Geräteverwaltung

besitzen.

Ihr Debugging benötigt daher eine erhöhte Capability und kann zusätzliche Schutzmaßnahmen verlangen:

- Physical Presence,
- Development- oder Maintenance-Modus,
- zeitlich begrenztes Token,
- Redaction sensibler Speicherbereiche,
- Supervisorbenachrichtigung.

---

## 13. Kernel-Read-only-Debugging

Readging-only-Kerneldebugging erlaubt beispielsweise:

- Register lesen,
- Kernelobjektzustände abfragen,
- Threads auflisten,
- Schedulerzustände untersuchen,
- geschützte Stacktraces erzeugen,
- Symbolinformationen verwenden.

Es erlaubt nicht:

- Kernelmemory schreiben,
- Register verändern,
- Kontrollfluss verändern,
- Seitentabellen bearbeiten,
- Module laden,
- Sicherheitsrichtlinien ändern.

Auch read-only Kerneldebugging kann Geheimnisse offenlegen und benötigt deshalb eine besondere Capability.

---

## 14. Kernel-Live-Debugging

Kernel-Live-Debugging kann:

- CPUs anhalten,
- Kernelregister verändern,
- Kernelmemory schreiben,
- Breakpoints setzen,
- Kontrollfluss umleiten,
- Codepatches erzeugen.

Diese Rechte entsprechen faktisch vollständiger Kernelkontrolle.

Deshalb gilt im Produktionsmodus standardmäßig:

```text
Kernel Live Debugging = deaktiviert
```

Eine Aktivierung benötigt eine ausdrücklich autorisierte Debugpolicy und in der Regel Physical Presence.

---

## 15. Post-Mortem Debugging

Post-Mortem Debugging wird gegenüber Live-Kerneldebugging bevorzugt.

Vorteile:

- laufender Kernel wird nicht verändert,
- keine zusätzliche aktive Kernelangriffsfläche,
- reproduzierbare Offlineanalyse,
- kontrollierte Redaction,
- besser begrenzbare Datenfreigabe.

Der Zugriff bleibt dennoch capabilitygeschützt, da Crash-Dumps sensible Daten enthalten können.

---

## 16. Debug Session als Kernelobjekt

Jede Debug Session wird als Kernelobjekt dargestellt.

Sie enthält mindestens:

```text
session_id
debugger_subject
target_object
debug_class
granted_rights
creation_time
expiration_time
authorization_id
policy_generation
audit_context
connection_state
session_state
```

Die Session wird über Handles und Capabilities verwaltet.

---

## 17. Sitzungszustände

Eine Debug Session durchläuft folgende Zustände:

```text
CREATED
   ↓
AUTHORIZING
   ↓
ACTIVE
   ↕
PAUSED
   ↓
REVOKING
   ↓
REVOKED
   ↓
CLOSED
```

| Zustand | Bedeutung |
|---|---|
| `CREATED` | Sessionobjekt wurde erzeugt |
| `AUTHORIZING` | Authentisierung und Policyprüfung laufen |
| `ACTIVE` | Debugoperationen sind erlaubt |
| `PAUSED` | Session bleibt bestehen, Operationen sind angehalten |
| `REVOKING` | Rechte werden zurückgenommen |
| `REVOKED` | Debugzugriff ist nicht mehr erlaubt |
| `CLOSED` | Sessionobjekt wird freigegeben |

---

## 18. Debugrechte

Debugrechte werden getrennt vergeben.

Mögliche Rechte:

```text
DEBUG_QUERY
DEBUG_ENUMERATE_THREADS
DEBUG_READ_REGISTERS
DEBUG_WRITE_REGISTERS
DEBUG_READ_MEMORY
DEBUG_WRITE_MEMORY
DEBUG_SET_BREAKPOINT
DEBUG_SET_WATCHPOINT
DEBUG_SINGLE_STEP
DEBUG_SUSPEND
DEBUG_RESUME
DEBUG_INSPECT_MAPPINGS
DEBUG_INSPECT_CAPABILITIES
DEBUG_READ_SYMBOLS
DEBUG_PATCH_CODE
DEBUG_CONTROL_KERNEL
```

Ein lesender Debugzugriff erhält keine impliziten Schreibrechte.

---

## 19. Least Privilege

Jede Debug Session erhält nur die für ihren Zweck notwendigen Rechte.

Beispiel:

```text
Supportdiagnose:
    DEBUG_QUERY
    DEBUG_READ_REGISTERS
    DEBUG_READ_SYMBOLS
    begrenzter DEBUG_READ_MEMORY

nicht erlaubt:
    DEBUG_WRITE_MEMORY
    DEBUG_PATCH_CODE
    DEBUG_CONTROL_KERNEL
```

Eine spätere Erweiterung der Rechte benötigt eine neue Autorisierungsentscheidung und ein Audit Event.

---

## 20. Autorisierungsquellen

Eine Debugautorisierung kann stammen aus:

- Prozess-Selbstautorisierung,
- Capability-Delegation,
- lokaler Administratorentscheidung,
- signiertem Debug Authorization Token,
- Development Policy,
- Recovery Policy,
- Physical Presence,
- Gerätebesitzer-Policy.

Mehrere Bedingungen können gleichzeitig erforderlich sein.

---

## 21. Debug Authorization Token

Ein Debug Authorization Token enthält mindestens:

```text
token_version
token_id
issuer
target_device
target_object
debug_class
allowed_rights
not_before
expires_at
maximum_session_duration
allowed_transport
nonce
policy_generation
signature
```

Das Token ist:

- signiert,
- zeitlich begrenzt,
- zielgebunden,
- nicht frei übertragbar,
- gegen Wiederverwendung geschützt.

---

## 22. Gerätebindung

Ein Debugtoken kann an ein Gerät gebunden werden über:

- Geräteidentität,
- TPM- oder Trust-Backend-Schlüssel,
- Installations-ID,
- signierten Hardware-Fingerprint.

Ein Token für Gerät A darf nicht auf Gerät B verwendet werden.

Fehlt ein Hardwareanker, wird die schwächere Gerätebindung im Trust State dokumentiert.

---

## 23. Zielbindung

Ein Debugtoken kann begrenzt werden auf:

- einen Prozess,
- ein Job Object,
- einen Dienst,
- ein Kernelmodul,
- eine Kernel-Build-ID,
- eine Systemgeneration,
- einen Crash Dump.

Ein Token für einen Userspace-Prozess darf nicht zum Kerneldebugging erweitert werden.

---

## 24. Zeitliche Begrenzung

Jede privilegierte Debug Session besitzt:

- frühesten Aktivierungszeitpunkt,
- Ablaufzeitpunkt,
- maximale Aktivdauer,
- optional maximale Leerlaufzeit.

Nach Ablauf:

1. Session wird auf `REVOKING` gesetzt,
2. neue Operationen werden abgelehnt,
3. Breakpoints und temporäre Zustände werden entfernt,
4. Ziel wird kontrolliert fortgesetzt oder gestoppt,
5. Session wird geschlossen,
6. Ablauf wird auditiert.

---

## 25. Physical Presence

Physical Presence wird insbesondere verlangt für:

- Kernel-Live-Debugging,
- Hardware-Debugport-Aktivierung,
- Änderung der Kernel-Debugpolicy,
- Entschlüsselung vollständiger Speicherdumps,
- Debugging besonders geschützter Systemdienste.

Die Bestätigung muss lokal und eindeutig sein.

Eine Remote-Verbindung darf Physical Presence nicht allein simulieren.

---

## 26. Debugmodi

NovaOS definiert folgende Systemmodi:

| Modus | Debugverhalten |
|---|---|
| `PRODUCTION` | Self Debugging und streng autorisierte Prozessdiagnose |
| `MAINTENANCE` | erweiterte Dienstdiagnose, zeitlich begrenzt |
| `DEVELOPMENT` | Kernel- und Modulentwicklung möglich |
| `RECOVERY` | Post-Mortem- und Reparaturdiagnose |
| `LOCKDOWN` | nur minimale lokale Diagnose |
| `FORENSIC` | read-only, umfangreiche nachvollziehbare Analyse |

Der aktive Modus wird im Trust State und Audit sichtbar.

---

## 27. Produktionsmodus

Im Produktionsmodus gelten mindestens:

- Kernel-Live-Debugging standardmäßig deaktiviert,
- Hardware-Debugports gesperrt,
- Remote-Debugserver nicht automatisch gestartet,
- vollständige Kerneladressen verborgen,
- Prozessdebugging capabilitygeschützt,
- vollständige Speicherdumps verschlüsselt,
- alle privilegierten Debugaktionen auditiert.

---

## 28. Development Mode

Der Development Mode kann erlauben:

- Kernel-Live-Debugging,
- zusätzliche Symbole,
- ausführlichere Stacktraces,
- Codepatches,
- Test-Breakpoints,
- reproduzierbare KASLR-Konfigurationen,
- Debug-Hardwareports.

Der Modus muss:

- ausdrücklich aktiviert werden,
- sichtbar gekennzeichnet sein,
- vom Secure Startup bestätigt werden,
- einen eigenen Schlüssel oder eine Development Policy verwenden.

---

## 29. Kein automatischer Debug-Fallback

Ein Boot-, Kernel- oder Sicherheitsfehler darf nicht automatisch einen offenen Debugserver starten.

Zulässige Reaktionen sind:

- verifiziertes Recovery,
- lokaler Panic-Bildschirm,
- geschützter Crash Dump,
- Physical-Presence-Dialog,
- autorisierter Maintenance-Modus.

Dadurch wird verhindert, dass ein Angreifer absichtlich einen Fehler erzeugt, um Debugzugriff zu erhalten.

---

## 30. Secure-Startup-Integration

Secure Kernel Startup prüft:

- ob Debugging erlaubt ist,
- welche Debugschlüssel gültig sind,
- welcher Systemmodus aktiv ist,
- ob Kernel-Live-Debugging zulässig ist,
- ob Hardware-Debugports gesperrt bleiben müssen.

Debugrelevante Bootparameter werden signiert und policygeprüft.

Beispiele:

```text
debug.kernel=off
debug.remote=off
debug.hardware=locked
debug.mode=production
```

---

## 31. Trust State

Mögliche Debug-Trust-Flags sind:

```text
DEBUG_DISABLED
PROCESS_DEBUG_ALLOWED
KERNEL_READONLY_DEBUG_ALLOWED
KERNEL_LIVE_DEBUG_ALLOWED
REMOTE_DEBUG_ENABLED
HARDWARE_DEBUG_ENABLED
PHYSICAL_PRESENCE_CONFIRMED
DEBUG_AUTH_TOKEN_ACTIVE
DEVELOPMENT_MODE_ACTIVE
DEBUG_TRUST_DEGRADED
```

Der Trust State zeigt den tatsächlich aktiven Zustand und nicht nur die konfigurierte Absicht.

---

## 32. Auswirkungen auf versiegelte Geheimnisse

Kernel-Live-Debugging kann die Vertraulichkeit versiegelter Geheimnisse aufheben.

NovaOS kann deshalb bei aktiviertem Kerneldebugging:

- bestimmte TPM-Secrets nicht freigeben,
- Benutzer-Schlüsselbunde sperren,
- Hochsicherheitsdomänen nicht starten,
- Netzwerkidentitäten deaktivieren,
- den Zugriff auf besonders sensible Dateien verweigern.

Diese Einschränkungen werden vor Aktivierung der Debug Session angezeigt.

---

## 33. Kerneldebugging und Systemvertrauen

Ein System mit aktiver Kernel-Schreibdebugsitzung gilt nicht mehr als normal vertrauenswürdig.

Mögliche Statusänderung:

```text
FIRMWARE_VERIFIED
        ↓
DEBUG_ACTIVE
        ↓
POLICY_DEGRADED
```

Der ursprüngliche Secure-Boot-Nachweis bleibt historisch gültig, beweist aber nicht mehr die Unverändertheit des laufenden Kernels nach Debugänderungen.

---

## 34. Lokales Debugging

Lokales Debugging verwendet bevorzugt:

- direkt gestartete Debuganwendung,
- lokales geschütztes Terminal,
- Recovery-Umgebung,
- physische Wartungsschnittstelle.

Auch lokales Debugging benötigt:

- Authentisierung,
- Capability-Prüfung,
- Zielprüfung,
- Auditierung.

Lokaler Zugriff bedeutet nicht automatisch uneingeschränkte Berechtigung.

---

## 35. Remote-Debugging

Remote-Debugging ist standardmäßig deaktiviert.

Wenn aktiviert, benötigt es:

- explizite lokale Policy,
- authentisierten Debugger,
- verschlüsselte Verbindung,
- gegenseitige Authentisierung,
- Replay-Schutz,
- Sitzungsablauf,
- Rate Limits,
- Auditierung,
- begrenzte Netzwerkbindung.

Der Debugserver lauscht nicht standardmäßig auf allen Netzwerkschnittstellen.

---

## 36. Remote-Transport

Ein Remote-Debugtransport muss:

- Vertraulichkeit,
- Integrität,
- Authentizität,
- Replay-Schutz,
- Sitzungsbindung

bereitstellen.

Klartextprotokolle sind für privilegiertes Debugging nicht zulässig.

Der Transport ist vom eigentlichen Debugprotokoll getrennt, damit unterschiedliche sichere Transportbackends möglich bleiben.

---

## 37. Netzwerkbindung

Ein Remote-Debugdienst kann gebunden werden an:

- Loopback,
- dedizierte Managementschnittstelle,
- bestimmtes VLAN,
- VPN,
- physisch getrenntes Wartungsnetz.

Eine Bindung an öffentliche oder unkontrollierte Netzwerke ist im Produktionsmodus standardmäßig verboten.

---

## 38. Verbindungsbestätigung

Bei privilegiertem Remote-Debugging zeigt NovaOS lokal mindestens:

- Debuggeridentität,
- angefordertes Ziel,
- Rechte,
- Sitzungsdauer,
- Netzwerkquelle,
- Trust-Auswirkung.

Die lokale Bestätigung kann bei vorab signierten Wartungspolicies entfallen, wenn dies ausdrücklich erlaubt wurde.

---

## 39. Rate Limiting

Authentisierungs- und Sitzungsversuche werden begrenzt.

Erfasst werden:

- fehlgeschlagene Authentisierungen,
- ungültige Tokens,
- abgelaufene Tokens,
- Replay-Versuche,
- Verbindungsfluten,
- unerlaubte Debugoperationen.

Wiederholte Verstöße können:

- Verbindungen sperren,
- Maintenance-Modus beenden,
- eine lokale Sicherheitswarnung erzeugen.

---

## 40. Debugprotokoll

Das Debugprotokoll ist versioniert.

Jede Nachricht enthält mindestens:

```text
protocol_version
session_id
sequence
operation
target_id
payload_length
integrity_context
```

Unbekannte kritische Operationen werden abgelehnt.

Alle Längen, Offsets und Objektkennungen werden vollständig validiert.

---

## 41. Parserisolation

Der Remote-Debugparser läuft nach Möglichkeit außerhalb des Kernelkerns in einem isolierten Systemdienst.

Der Kernel erhält nur validierte, capabilitygebundene Debugoperationen.

Vorteile:

- kleinere Kernelangriffsfläche,
- Dienst kann neu gestartet werden,
- komplexe Transportprotokolle bleiben im Userspace,
- Parserfehler gefährden nicht unmittelbar den gesamten Kernel.

---

## 42. Debug Broker

NovaOS verwendet einen privilegierten Debug Broker.

Der Debug Broker:

- authentisiert Debugger,
- validiert Tokens,
- erzeugt Debug Sessions,
- verwaltet Transporte,
- setzt Timeouts durch,
- übersetzt Debugoperationen,
- erzeugt Audit Events.

Der Broker erhält nicht automatisch vollständige Kernel-Schreibrechte. Diese werden nur für eine konkrete Session delegiert.

---

## 43. Debug Broker Isolation

Der Debug Broker läuft:

- in einem eigenen Job Object,
- in einer eigenen Sicherheitsdomäne,
- mit festen Ressourcenlimits,
- unter einem Supervisor,
- ohne unnötige Geräte- oder Dateisystemrechte.

Ein Absturz des Brokers beendet aktive Remote-Sessions und führt nicht automatisch zu offenen Kerneldebugrechten.

---

## 44. Prozessstopp

Ein Debugger kann ein Ziel nur mit entsprechender Capability anhalten.

Beim Stoppen eines Prozesses werden:

- Threads kontrolliert an sicheren Punkten angehalten,
- Debugzustände gespeichert,
- Job- und Supervisorrichtlinien berücksichtigt,
- Echtzeit- und Safety-Auswirkungen geprüft.

Ein sicherheitskritischer Prozess darf nicht ohne passende Safety-Policy unbegrenzt angehalten werden.

---

## 45. Kernelstopp

Kernel-Live-Debugging kann eine oder mehrere CPUs stoppen.

Mögliche Modi:

| Modus | Verhalten |
|---|---|
| `STOP_ONE_CPU` | nur Ziel-CPU wird angehalten |
| `STOP_TARGET_CPUS` | definierte CPU-Menge wird angehalten |
| `STOP_THE_WORLD` | alle normalen CPUs werden angehalten |
| `NON_STOP` | andere Threads laufen weiter |

`STOP_THE_WORLD` benötigt besondere Autorisierung und feste Watchdogregeln.

---

## 46. Debugger und Watchdog

Ein Debuggerstopp darf nicht unbeabsichtigt:

- Hardware-Watchdogs auslösen,
- Safety-Controller blockieren,
- Recovery verhindern,
- Geräte in gefährlichem Zustand belassen.

Die Debug Session definiert:

- Watchdogpolicy,
- maximale Stopdauer,
- Safe-State-Anforderung,
- automatische Fortsetzung oder Abbruch.

Watchdogs werden nicht unbegrenzt deaktiviert.

---

## 47. Echtzeitprozesse

Das Debuggen eines Echtzeitprozesses kann seine Garantien ungültig machen.

Vor Aktivierung wird deshalb festgelegt:

- ob Deadlines ausgesetzt werden,
- ob ein Safe State erforderlich ist,
- ob die Echtzeitreservierung widerrufen wird,
- ob das System in einen Maintenance-Modus wechselt.

Eine aktive Debug Session wird in der Echtzeittelemetrie sichtbar.

---

## 48. Robotik und Safety

Bei Robotik- oder Maschinensteuerung darf ein Debugger keine aktive Steuerung unkontrolliert anhalten.

Erforderlich können sein:

- physischer Wartungsmodus,
- abgeschaltete Aktoren,
- unabhängiger Safety-Controller,
- reduzierte Geschwindigkeit,
- definierter Safe State.

Kerneldebugging an einem aktiv steuernden Produktivsystem ist standardmäßig nicht zulässig.

---

## 49. Speicherlesen

Debug-Speicherzugriffe werden gegen Ziel, Bereich und Rechte geprüft.

Der Debugger darf nur Bereiche lesen, die durch seine Session abgedeckt sind.

Mögliche Einschränkungen:

- keine Schlüsselbereiche,
- keine fremden Sicherheitsdomänen,
- keine Kernel-Seitentabellen,
- nur definierte Module,
- nur Snapshots,
- begrenzte Datenmenge.

Ein allgemeines `read physical memory`-Recht ist eine eigene hochkritische Capability.

---

## 50. Speicherschreiben

Schreibzugriff ist wesentlich kritischer als Lesezugriff.

Vor einem Schreibzugriff prüft NovaOS:

- Zielbereich,
- aktiven Threadzustand,
- Seitenrechte,
- W^X,
- Kernelisolationspolicy,
- Sessionrechte,
- Debugmodus,
- Auditfähigkeit.

Schreibzugriff auf Kerneltext verwendet ausschließlich den kontrollierten Codepatch-Pfad.

---

## 51. Keine direkten physischen Schreibzugriffe

Ein Debugger erhält standardmäßig keinen direkten beliebigen physischen Speicherzugriff.

Physische Schreibrechte könnten umgehen:

- virtuelle Speicherrechte,
- W^X,
- Kernel Isolation,
- Seitentabellenschutz,
- Prozessisolation,
- Capability-Prüfungen.

Ein solcher Zugriff ist nur in einer besonders autorisierten Hardware- oder Recovery-Debugsession zulässig.

---

## 52. Registerzugriff

Register werden nach Typ getrennt:

- allgemeine Register,
- Userspace-Kontrollregister,
- FPU-/SIMD-Zustand,
- Debugregister,
- privilegierte Kontrollregister,
- Seitentabellenregister,
- Sicherheitsregister.

Eine Prozess-Debugsession darf keine privilegierten Kernelkontrollregister verändern.

Kernelkontrollregister benötigen `DEBUG_CONTROL_KERNEL`.

---

## 53. Breakpoints

NovaOS unterstützt:

- Hardware-Breakpoints,
- Software-Breakpoints,
- Watchpoints,
- bedingte Breakpoints.

Hardware-Breakpoints werden bevorzugt, wenn sie ausreichen.

Software-Breakpoints verändern Code und müssen deshalb W^X-konform gesetzt werden.

---

## 54. W^X-konforme Software-Breakpoints

Der Ablauf für einen Kernel-Software-Breakpoint lautet:

```text
Zielausführung stoppen
        ↓
Codebereich gegen Sessionrechte prüfen
        ↓
Ausführungsabbildung kontrolliert deaktivieren
        ↓
temporäre nicht ausführbare Schreibansicht erzeugen
        ↓
Breakpoint-Instruktion schreiben
        ↓
Schreibansicht entfernen
        ↓
RX-Rechte wiederherstellen
        ↓
Caches und TLB synchronisieren
        ↓
Ausführung fortsetzen
```

Eine dauerhafte RWX-Seite ist nicht zulässig.

---

## 55. Breakpoint-Inventar

Jede Debug Session verwaltet ein Inventar ihrer:

- Breakpoints,
- Watchpoints,
- Codepatches,
- angehaltenen Threads,
- veränderten Register,
- temporären Mappings.

Beim Sessionende müssen diese Zustände kontrolliert entfernt oder nach ausdrücklicher Policy dauerhaft übernommen werden.

---

## 56. Sessionende

Beim normalen Sessionende:

1. neue Debugoperationen sperren,
2. ausstehende Operationen abbrechen,
3. Breakpoints entfernen,
4. temporäre Codepatches zurücknehmen,
5. Debugregister bereinigen,
6. Ziele entsprechend der Policy fortsetzen,
7. Transport schließen,
8. Sessionabschluss auditieren,
9. Handles und Capabilities widerrufen.

---

## 57. Unerwarteter Verbindungsabbruch

Bei einem Remote-Verbindungsabbruch gilt eine vorab definierte Policy.

Mögliche Reaktionen:

| Policy | Verhalten |
|---|---|
| `RESUME` | Ziel kontrolliert fortsetzen |
| `REMAIN_PAUSED` | Ziel bleibt begrenzt angehalten |
| `TERMINATE_TARGET` | Zielprozess beenden |
| `REVOKE_AND_RECOVER` | Session widerrufen und Supervisor aktivieren |
| `SAFE_STATE` | Plattform-Safe-State auslösen |

Kernel- und Safety-Debugging verwenden keine unbegrenzte `REMAIN_PAUSED`-Policy ohne Watchdog.

---

## 58. Sessionwiderruf

Eine Debug Session kann widerrufen werden durch:

- Ablaufzeit,
- Administrator,
- Tokenwiderruf,
- Policyänderung,
- Trust-State-Änderung,
- Verbindungsfehler,
- Sicherheitsverletzung,
- Recovery- oder Panic-Vorgang.

Der Widerruf wirkt auf alle abgeleiteten Handles und Debugrechte.

---

## 59. Debugsymbole

Debugsymbole können Kernel- und Programmstrukturen offenlegen.

NovaOS trennt:

- öffentliche Symbole,
- lokale Entwicklungssymbole,
- geschützte Produktionssymbole,
- vollständige Typinformationen.

Produktionssysteme enthalten nicht zwingend vollständige Debugsymbole.

Symbolserver oder Symboldateien benötigen:

- Build-ID-Zuordnung,
- Integritätsschutz,
- Zugriffskontrolle,
- keine automatische externe Übertragung.

---

## 60. KASLR und Symbole

Zur Auflösung randomisierter Adressen benötigt der Debugger:

- Build-ID,
- Modul-ID,
- autorisierte Layoutinformationen,
- gegebenenfalls KASLR-Slide.

Der Slide wird nur an Sessions mit:

```text
DEBUG_READ_KERNEL_LAYOUT
```

freigegeben.

Eine normale Prozess-Debugsession erhält keine Kernel-KASLR-Informationen.

---

## 61. Stacktraces

Stacktraces werden abhängig von der Debugklasse redigiert.

Mögliche Darstellungen:

- nur symbolische Funktionsnamen,
- relative Offsets,
- maskierte Adressen,
- vollständige Adressen für autorisierte Kerneldiagnose.

Stackinhalte können Geheimnisse enthalten und werden nicht automatisch vollständig übertragen.

---

## 62. Geheimnisschutz

Debugzugriff auf bekannte Geheimnisbereiche wird standardmäßig blockiert oder redigiert.

Dazu gehören:

- private Schlüssel,
- Authentisierungstoken,
- Stack-Canaries,
- KASLR-Seeds,
- Dump-Verschlüsselungsschlüssel,
- TPM-Secrets,
- Benutzerpasswörter,
- biometrische Rohdaten.

Kernel-Live-Debugging mit vollständigem Speicherzugriff kann diese Trennung technisch möglicherweise umgehen. Deshalb wird ein solcher Modus als vollständige Vertrauensherabstufung behandelt.

---

## 63. Debug und Audit

Folgende Vorgänge werden mindestens auditiert:

- Debug Session angefordert,
- Debugger authentisiert,
- Autorisierung erteilt oder verweigert,
- Physical Presence bestätigt,
- Debug Session gestartet,
- Rechte erweitert,
- Ziel angehalten oder fortgesetzt,
- Kernelmemory gelesen oder geschrieben,
- Codepatch gesetzt oder entfernt,
- Debugtoken widerrufen,
- Verbindung abgebrochen,
- Debug Session beendet,
- Hardware-Debugport aktiviert,
- Crash Dump entschlüsselt oder exportiert.

---

## 64. Audit-Datenminimierung

Audit Records enthalten:

- Debuggeridentität,
- Ziel-ID,
- Rechte,
- Zeit,
- Ergebnis,
- betroffenen Bereichstyp,
- Datenmenge,
- Policygeneration.

Sie enthalten nicht automatisch:

- gelesene Speicherinhalte,
- Schlüssel,
- vollständige Registerinhalte,
- vollständige Kerneladressen,
- Debugger-Nutzlasten.

---

## 65. Debug-Session-Protokoll

Zusätzlich zum allgemeinen Audit kann eine privilegierte Debug Session ein eigenes Aktivitätsprotokoll führen.

Dieses enthält:

```text
operation_sequence
operation_type
target
range
result
timestamp
session_id
```

Der Inhalt wird entsprechend der Datenschutz- und Aufbewahrungspolicy gespeichert.

---

## 66. Kernel-Panic-Integration

Bei einem Kernel-Panic werden aktive Debug Sessions widerrufen.

Im autorisierten Development Mode kann die Panic Policy anschließend einen lokalen Debugger aktivieren.

Im Produktionsmodus gilt:

- kein automatisch offener Remote-Debugserver,
- geschützter Crash Dump,
- kontrollierter Reboot oder Recovery,
- Debuganalyse erst nach erneuter Autorisierung.

---

## 67. Recovery-Debugging

Das verifizierte Recovery-System kann Post-Mortem-Debugging bereitstellen.

Es kann:

- Crash-Dumps auflisten,
- Integrität prüfen,
- Dumps entschlüsseln, wenn autorisiert,
- Symbole zuordnen,
- Diagnoseexporte erzeugen.

Recovery-Debugging darf die Secure-Startup- und Rollback-Policy nicht umgehen.

---

## 68. Hardware-Debugports

Hardware-Debugschnittstellen wie:

- JTAG,
- SWD,
- Debug UART,
- DCI,
- Hersteller-Debuginterfaces

sind in Produktionssystemen standardmäßig gesperrt.

Soweit Hardware dies unterstützt, werden sie:

- durch Firmware deaktiviert,
- über Sicherungs- oder Lockbits gesperrt,
- durch Physical Presence kontrolliert,
- nur in Development- oder Maintenance-Modi aktiviert.

---

## 69. Hardware-Debug-Lifecycle

Der Hardware-Debugstatus besitzt Zustände:

```text
LOCKED
   ↓
AUTHORIZED
   ↓
ENABLED
   ↓
ACTIVE
   ↓
REVOKED
   ↓
LOCKED
```

Manche Hardware erlaubt nach einem permanenten Lock keine erneute Aktivierung. Diese Eigenschaft muss bei Geräteentwicklung und Fertigung ausdrücklich berücksichtigt werden.

---

## 70. Produktionsgeräte

Für veröffentlichte Produktionsgeräte gilt:

- Hardware-Debugports standardmäßig gesperrt,
- keine bekannten Standardpasswörter,
- keine universellen Unlock-Tokens,
- gerätespezifische Autorisierung,
- zeitlich begrenzter Maintenance-Zugriff,
- sichtbare Debugstatusanzeige, soweit möglich.

Ein globaler Hersteller-Masterkey ist zu vermeiden.

---

## 71. Debug-UART

Eine serielle Kernelkonsole kann sensible Informationen offenlegen.

Im Produktionsmodus ist sie:

- deaktiviert,
- nur ausgabebeschränkt,
- physisch geschützt oder
- capability- und policygesteuert.

Sie darf keine vollständigen Kerneladressen, Schlüssel oder Benutzerdaten standardmäßig ausgeben.

---

## 72. Anti-Replay

Debugtokens und Remotesitzungen verwenden:

- Nonces,
- Sequenznummern,
- Ablaufzeiten,
- einmalige Token-IDs,
- Bindung an Boot-ID oder Gerätegeneration.

Ein aufgezeichnetes Debug-Unlock darf nicht später erneut verwendet werden.

---

## 73. Widerrufslisten

Kompromittierte Debugschlüssel und Tokenaussteller können widerrufen werden.

Widerrufsinformationen werden durch Secure Startup und den Debug Broker geprüft.

Ein widerrufener Schlüssel darf keine neue Debug Session autorisieren.

Bereits aktive Sessions können abhängig von der Policy sofort beendet werden.

---

## 74. Schlüsseltrennung

Debugschlüssel werden getrennt von:

- Release-Signing-Keys,
- Recovery-Keys,
- Audit-Keys,
- Benutzer-Keys,
- Festplattenverschlüsselungs-Keys

verwaltet.

Mögliche Rollen:

```text
DEBUG_PROCESS_AUTH
DEBUG_SERVICE_AUTH
DEBUG_KERNEL_READONLY_AUTH
DEBUG_KERNEL_LIVE_AUTH
DEBUG_HARDWARE_UNLOCK_AUTH
DEBUG_DUMP_DECRYPT_AUTH
```

---

## 75. Datenschutz

Debugging kann personenbezogene und vertrauliche Daten offenlegen.

Daher gelten:

- Zweckbindung,
- minimale Rechte,
- zeitliche Begrenzung,
- kontrollierter Export,
- lokale Verarbeitung als Standard,
- Auditierung,
- Aufbewahrungsfristen,
- Redaction.

Eine Debug Session ist keine allgemeine Erlaubnis zur Einsicht in sämtliche Benutzerinhalte.

---

## 76. Remote-Export

Debugdaten werden nicht automatisch an externe Supportdienste übertragen.

Ein Export benötigt:

- ausdrückliche Autorisierung,
- Zieldefinition,
- Verschlüsselung,
- Redaction,
- Größenbegrenzung,
- Audit Record.

Der Benutzer oder Geräteadministrator muss erkennen können, dass ein Export stattfindet.

---

## 77. Debugger-Plugins

Debugger-Plugins und Erweiterungen laufen nicht automatisch mit vollständigen Debugrechten.

Sie werden:

- signiert oder geprüft,
- in getrennten Prozessen ausgeführt,
- capabilitybegrenzt,
- durch IPC mit dem Debug Broker verbunden,
- separat auditiert.

Ein UI-Plugin darf nicht allein durch Installation Kernelmemory-Zugriff erhalten.

---

## 78. Skripte und Automatisierung

Debugskripte können weitreichende Operationen automatisieren.

Sie werden deshalb wie Debuggercode behandelt.

Eine Session kann einschränken:

- erlaubte Befehle,
- maximale Laufzeit,
- Zielbereiche,
- Schleifen und Wiederholungen,
- Schreiboperationen,
- Export.

Unsignierte Skripte erhalten keine automatische Kerneldebugberechtigung.

---

## 79. API-Grundmodell

Eine mögliche native API lautet:

```c
nova_status_t nova_debug_session_create(
    const nova_debug_session_create_info_t* create_info,
    nova_handle_t* debug_session
);

nova_status_t nova_debug_session_authorize(
    nova_handle_t debug_session,
    const nova_debug_authorization_t* authorization
);

nova_status_t nova_debug_session_revoke(
    nova_handle_t debug_session
);

nova_status_t nova_debug_read_memory(
    nova_handle_t debug_session,
    uintptr_t target_address,
    void* destination,
    size_t size
);

nova_status_t nova_debug_write_memory(
    nova_handle_t debug_session,
    uintptr_t target_address,
    const void* source,
    size_t size
);

nova_status_t nova_debug_get_registers(
    nova_handle_t debug_session,
    nova_thread_id_t thread_id,
    nova_register_set_t* registers
);

nova_status_t nova_debug_set_breakpoint(
    nova_handle_t debug_session,
    const nova_breakpoint_info_t* breakpoint,
    nova_handle_t* breakpoint_handle
);
```

---

## 80. Sessionerzeugung

Eine mögliche Erzeugungsstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    nova_handle_t target;
    nova_debug_class_t debug_class;

    uint64_t requested_rights;
    uint64_t maximum_duration_ns;

    nova_handle_t authorization_token;
    nova_handle_t transport;

    uint32_t flags;
    uint32_t reserved;
} nova_debug_session_create_info_t;
```

Eine angeforderte Berechtigung ist noch keine erteilte Berechtigung.

---

## 81. Sitzungsinformationen

Eine mögliche Statusstruktur lautet:

```c
typedef struct
{
    nova_debug_session_id_t session_id;
    nova_debug_class_t debug_class;
    nova_debug_session_state_t state;

    uint64_t granted_rights;
    uint64_t creation_time_ns;
    uint64_t expiration_time_ns;
    uint64_t last_activity_time_ns;

    nova_object_id_t debugger_subject;
    nova_object_id_t target_object;

    uint64_t policy_generation;
    uint32_t transport_type;
    uint32_t flags;
} nova_debug_session_info_t;
```

---

## 82. Capability-Modell

Mögliche Debug-Capabilities sind:

```text
CAP_DEBUG_SELF
CAP_DEBUG_PROCESS
CAP_DEBUG_JOB
CAP_DEBUG_SERVICE
CAP_DEBUG_READ_MEMORY
CAP_DEBUG_WRITE_MEMORY
CAP_DEBUG_READ_REGISTERS
CAP_DEBUG_WRITE_REGISTERS
CAP_DEBUG_BREAKPOINT
CAP_DEBUG_KERNEL_READ
CAP_DEBUG_KERNEL_WRITE
CAP_DEBUG_KERNEL_LAYOUT
CAP_DEBUG_KERNEL_CONTROL
CAP_DEBUG_HARDWARE_UNLOCK
CAP_DEBUG_REMOTE
CAP_DEBUG_DUMP_DECRYPT
CAP_DEBUG_ADMIN
```

Capabilities können eingeschränkt werden auf:

- ein bestimmtes Ziel,
- einen Adressbereich,
- einen Registertyp,
- bestimmte Operationen,
- eine Debugklasse,
- eine maximale Dauer,
- einen Transport,
- eine Systemgeneration.

---

## 83. Fehlercodes

Mögliche Status- und Fehlercodes sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_DEBUG_DISABLED
NOVA_STATUS_DEBUG_TARGET_INVALID
NOVA_STATUS_DEBUG_CLASS_DENIED
NOVA_STATUS_DEBUG_AUTH_REQUIRED
NOVA_STATUS_DEBUG_AUTH_INVALID
NOVA_STATUS_DEBUG_TOKEN_EXPIRED
NOVA_STATUS_DEBUG_TOKEN_REVOKED
NOVA_STATUS_DEBUG_PHYSICAL_PRESENCE_REQUIRED
NOVA_STATUS_DEBUG_RIGHT_DENIED
NOVA_STATUS_DEBUG_SESSION_EXPIRED
NOVA_STATUS_DEBUG_SESSION_REVOKED
NOVA_STATUS_DEBUG_TARGET_BUSY
NOVA_STATUS_DEBUG_MEMORY_REDACTED
NOVA_STATUS_DEBUG_WX_VIOLATION
NOVA_STATUS_DEBUG_TRANSPORT_UNTRUSTED
NOVA_STATUS_DEBUG_POLICY_CONFLICT
NOVA_STATUS_DEBUG_TRUST_DEGRADED
```

Eine ungültige Debuganforderung darf keinen Kernel-Panic auslösen.

---

## 84. Verhalten bei Sicherheitsverletzung

Erkennt NovaOS innerhalb einer Debug Session:

- Tokenmanipulation,
- Replay,
- Rechteüberschreitung,
- unerlaubten Speicherbereich,
- Protokollmanipulation,
- Transportintegritätsfehler,
- wiederholte Authentisierungsfehler,

wird die Session:

1. pausiert oder widerrufen,
2. von ihren Debughandles getrennt,
3. aus dem Ziel entfernt,
4. auditiert,
5. gegebenenfalls durch eine lokale Sicherheitswarnung gemeldet.

Bei möglicher Kernelmanipulation kann zusätzlich die Kernel Panic Policy ausgelöst werden.

---

## 85. Telemetrie und Diagnose

NovaOS erfasst mindestens:

- aktive Debug Sessions,
- Debugklasse,
- Zieltyp,
- erteilte Rechte,
- lokale oder Remote-Verbindung,
- Physical-Presence-Status,
- Sitzungsdauer,
- fehlgeschlagene Authentisierungen,
- widerrufene Tokens,
- gesetzte Breakpoints,
- Kernelcodepatches,
- Debug-Trust-State,
- Hardware-Debugport-Status.

Sensible Speicherinhalte werden nicht in normale Telemetrie aufgenommen.

---

## 86. Performance

Ist keine Debug Session aktiv, soll Secure Debugging nur geringe Laufzeitkosten verursachen.

Mögliche Kosten entstehen durch:

- Debug-Hooks,
- Breakpointprüfung,
- Audit,
- Symbolverwaltung,
- Sessionzustände.

Produktions-Builds können ungenutzte erweiterte Debugpfade deaktivieren oder modular laden, solange die sichere Autorisierungsarchitektur erhalten bleibt.

---

## 87. Konsequenzen

### 87.1 Positive Konsequenzen

- Debugging bleibt möglich, ohne permanent offene Hintertür,
- klare Trennung von Prozess- und Kerneldebugging,
- ziel-, operations- und zeitbezogene Capabilities,
- sichere Remote-Verbindungen,
- vollständige Auditierbarkeit privilegierter Sitzungen,
- Hardware-Debugports werden kontrolliert,
- Post-Mortem-Debugging wird bevorzugt,
- Trust State zeigt aktive Sicherheitsminderung,
- W^X und Kernel Isolation bleiben grundsätzlich erhalten.

### 87.2 Negative Konsequenzen

- höhere Komplexität für Debugwerkzeuge,
- Token- und Schlüsselverwaltung erforderlich,
- Produktionsdiagnose benötigt zusätzliche Autorisierungsschritte,
- Kernel-Live-Debugging kann Sicherheitsgarantien reduzieren,
- Remote-Debugging erhöht die Angriffsoberfläche,
- Redaction kann die Analyse erschweren.

### 87.3 Risiken

- kompromittierter Debugschlüssel,
- nicht widerrufene Debug Session,
- unvollständig entfernte Breakpoints,
- offener Hardware-Debugport,
- Geheimnisoffenlegung über Speicherzugriff,
- unsicherer Remote-Parser,
- Watchdogprobleme bei gestopptem Kernel,
- dauerhafte W^X-Ausnahme,
- Missbrauch des Development Mode.

Diese Risiken werden begrenzt durch:

- Capability-Sessions,
- Physical Presence,
- signierte und zeitlich begrenzte Tokens,
- Debug Broker Isolation,
- gegenseitig authentisierte Transporte,
- Sitzungsinventar,
- automatischen Widerruf,
- Audit,
- Secure-Startup-Integration.

---

## 88. Auswirkungen auf andere Subsysteme

### 88.1 Secure Kernel Startup

Debugmodus, Schlüssel und Bootparameter werden vor Kernelstart geprüft.

### 88.2 Kernel ASLR

Kernel-Layoutinformationen werden nur an entsprechend autorisierte Sessions ausgegeben.

### 88.3 NX und W^X

Software-Breakpoints und Codepatches verwenden kontrollierte nicht-RWX-Übergänge.

### 88.4 SMEP/SMAP

Debugger verwenden kontrollierte Speicherzugriffs- und Uaccess-Schnittstellen.

### 88.5 Kernel Isolation

Debugzugriffe auf Seitentabellen und Kernelmappings benötigen eigene Capabilities.

### 88.6 Audit

Alle privilegierten Debugsitzungen und kritischen Operationen werden auditiert.

### 88.7 Kernel Panic Policy

Panic-Debugging ist nur im autorisierten Development- oder Maintenance-Modus möglich.

### 88.8 Prozessverwaltung

Prozess- und Threadstopp müssen mit Lebenszyklus, Job Objects und Supervisoren koordiniert werden.

### 88.9 Echtzeitscheduler

Debugging kann Echtzeitgarantien widerrufen und Safe-State-Policies auslösen.

### 88.10 Capability-System

Debugrechte werden als feingranulare, zielgebundene Capabilities modelliert.

### 88.11 Netzwerk

Remote-Debugging verwendet ausschließlich authentisierte und verschlüsselte Transportkanäle.

### 88.12 Recovery

Post-Mortem-Diagnose wird bevorzugt in einer verifizierten Recovery-Umgebung ausgeführt.

---

## 89. Implementierungsrichtlinien

1. Debugging ist standardmäßig deaktiviert.
2. Jede Debugoperation benötigt eine aktive Debug Session.
3. Debug Sessions sind ziel-, rechte- und zeitgebunden.
4. Self Debugging erhält keine impliziten Kernelrechte.
5. Kernel-Live-Debugging ist im Produktionsmodus standardmäßig verboten.
6. Kerneldebugging benötigt eine besondere Capability und normalerweise Physical Presence.
7. Remote-Debugging ist standardmäßig deaktiviert.
8. Remote-Transporte müssen verschlüsselt, authentisiert und replaygeschützt sein.
9. Der Remote-Protokollparser läuft nach Möglichkeit in einem isolierten Debug Broker.
10. Debugtokens sind signiert, zeitlich begrenzt und gerätegebunden.
11. Debugschlüssel werden von Release-, Recovery- und Audit-Schlüsseln getrennt.
12. Jede privilegierte Debugoperation wird auditiert.
13. Debug-Speicherzugriffe werden gegen Zielbereiche und Redaction-Policies geprüft.
14. Software-Breakpoints und Codepatches halten W^X ein.
15. Eine Debug Session verwaltet alle temporären Breakpoints, Registeränderungen und Mappings.
16. Beim Sessionende werden sämtliche temporären Debugzustände entfernt.
17. Kernel-Live-Debugging stuft den aktiven Trust State herab.
18. Versiegelte Hochsicherheitsgeheimnisse können bei Kerneldebugging gesperrt bleiben.
19. Hardware-Debugports sind auf Produktionsgeräten standardmäßig gesperrt.
20. Ein Fehler startet niemals automatisch einen offenen Debugserver.
21. Post-Mortem-Debugging wird gegenüber Live-Kerneldebugging bevorzugt.
22. Debugdaten werden nicht automatisch extern übertragen.

---

## 90. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Self-Debugging,
- Fremdprozess-Debugging,
- verweigerter Zugriff ohne Capability,
- Job-Debugging,
- privilegierter Systemdienst,
- read-only Kerneldebugging,
- Kernel-Live-Debugging im Development Mode,
- verweigertes Kerneldebugging im Produktionsmodus,
- Physical Presence,
- gültiges Debugtoken,
- abgelaufenes Debugtoken,
- widerrufenes Debugtoken,
- Token für falsches Gerät,
- Token für falsches Ziel,
- Token-Replay,
- Sitzungsablauf,
- Leerlauf-Timeout,
- Rechteerweiterung,
- Sessionwiderruf,
- Verbindungsabbruch,
- automatische Breakpointbereinigung,
- Registerlesen,
- verweigerter Kontrollregisterzugriff,
- Speicherlesen mit Redaction,
- Speicherschreiben,
- physischer Speicherzugriff,
- Hardware-Breakpoint,
- W^X-konformer Software-Breakpoint,
- Kernelcodepatch,
- TLB- und Cache-Synchronisation,
- KASLR-Symbolauflösung,
- verweigerter Kernel-Slide-Zugriff,
- lokales Debugging,
- Remote-Debugging,
- gegenseitige Authentisierung,
- Transportmanipulation,
- Rate Limiting,
- Debug-Broker-Absturz,
- Debugger-Plugin-Isolation,
- Debugskript-Begrenzung,
- Echtzeitprozess-Debugging,
- Robotik-Safe-State,
- Watchdog bei Kernelstopp,
- Kernel-Panic im Development Mode,
- Post-Mortem-Debugging,
- Crash-Dump-Entschlüsselung,
- Hardware-Debugport-Lock,
- Hardware-Debug-Unlock,
- Suspend und Resume,
- Audit aller privilegierten Operationen,
- Datenschutz und Export,
- Fuzzing des Debugprotokolls,
- Fuzzing des Debug-Brokers.

---

## 91. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- das vollständige Debugprotokoll,
- konkrete TLS- oder Transportalgorithmen,
- die Benutzeroberfläche des Debuggers,
- konkrete JTAG-, SWD- oder DCI-Hardwareimplementierungen,
- vollständige Symbolserver-Infrastruktur,
- automatische KI-Fehleranalyse,
- Hersteller-Supportprozesse,
- konkrete Fertigungs-Lockbits,
- vollständige virtuelle Maschinen-Debugarchitektur,
- die endgültige öffentliche Debug-ABI.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 92. Verbindliche Invarianten

1. Debugging ist ohne aktive Autorisierung standardmäßig deaktiviert.
2. Jede Debugoperation gehört zu einer aktiven Debug Session.
3. Jede Debug Session ist an ein konkretes Ziel gebunden.
4. Debugrechte werden getrennt und nach Least Privilege vergeben.
5. Leserechte gewähren keine impliziten Schreibrechte.
6. Prozessdebugging gewährt keine Kerneldebugrechte.
7. Kernel-Live-Debugging ist im Produktionsmodus standardmäßig deaktiviert.
8. Kerneldebugging benötigt eine besondere Capability und Policy.
9. Privilegiertes Remote-Debugging verwendet niemals Klartexttransport.
10. Remote-Debugging ist standardmäßig deaktiviert.
11. Debugtokens sind signiert, zeitlich begrenzt und replaygeschützt.
12. Ein Debugtoken darf nicht auf ein anderes Gerät oder Ziel übertragen werden.
13. Physical Presence kann nicht ausschließlich durch eine Remote-Anforderung ersetzt werden.
14. Jede privilegierte Debug Session wird auditiert.
15. Codepatches und Software-Breakpoints dürfen keine dauerhaften RWX-Mappings erzeugen.
16. Beim Sessionende werden temporäre Breakpoints, Mappings und Rechte entfernt.
17. Eine abgelaufene oder widerrufene Session darf keine weiteren Debugoperationen ausführen.
18. Kernel-Live-Debugging muss im Trust State sichtbar sein.
19. Ein System mit aktivem Kernel-Schreibdebugging darf nicht als unverändert produktionsvertrauenswürdig gelten.
20. Hardware-Debugports sind auf Produktionsgeräten standardmäßig gesperrt.
21. Ein Boot- oder Kernel-Fehler darf nicht automatisch einen offenen Debugserver starten.
22. Crash-Dumps und Debugdaten werden nicht automatisch extern übertragen.
23. Geheimnisse werden in begrenzten Debug Sessions standardmäßig redigiert.
24. Eine Capability ersetzt keine Debugpolicy, Zielprüfung oder Sitzungsautorisierung.
25. Kerneldebugging darf NX, W^X, SMEP, SMAP oder Kernel Isolation nicht dauerhaft und unbemerkt deaktivieren.
26. Recovery-Debugging darf Secure Startup und Rollback Protection nicht umgehen.
27. Debugger-Plugins und Skripte erhalten keine impliziten Rechte des Debug Brokers.
28. KI- oder lernbasierte Komponenten dürfen Debug-, Token-, Transport-, Audit-, Datenschutz-, Capability- oder Trust-State-Regeln nicht umgehen.

---

## 93. Referenzen

- ADR-SEC-0001 – Secure Kernel Startup
- ADR-SEC-0002 – Kernel ASLR
- ADR-SEC-0003 – Stack Protector
- ADR-SEC-0004 – NX Memory
- ADR-SEC-0005 – SMEP/SMAP
- ADR-SEC-0006 – Kernel Isolation
- ADR-SEC-0008 – Audit
- ADR-SEC-0009 – Kernel Panic Policy
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0008 – Context Switching
- ADR-PROC-0009 – Job Objects
- NPSPEC-SECUREDEBUG-0001 – Secure Debugging Specification
- NPSPEC-AUDIT-0001 – Audit Specification
- NPSPEC-KERNELPANIC-0001 – Kernel Panic Specification
- NPSPEC-CRASHDUMP-0001 – Kernel Crash Dump Specification
- NPSPEC-CAPABILITY-0001 – Capability Specification
- NPSPEC-RECOVERY-0001 – Recovery Specification

---

## 94. Zusammenfassung der Entscheidung

NovaOS verwendet capability-basierte, zielgebundene und zeitlich begrenzte Debug Sessions.

Debugging ist standardmäßig deaktiviert. Jede Sitzung benötigt:

- ein konkretes Debug Target,
- definierte Einzelrechte,
- eine gültige Capability,
- erfolgreiche Authentisierung,
- Zustimmung der aktiven Debugpolicy,
- gegebenenfalls Physical Presence.

Userspace-, Systemdienst-, Kernel-, Hardware- und Post-Mortem-Debugging werden getrennt behandelt. Kernel-Live-Debugging ist im Produktionsmodus standardmäßig deaktiviert und gilt bei Aktivierung als Herabstufung des laufenden Systemvertrauens.

Remote-Debugging ist ebenfalls standardmäßig deaktiviert. Wird es aktiviert, verwendet es gegenseitige Authentisierung, Verschlüsselung, Replay-Schutz, Rate Limits und zeitlich begrenzte Sitzungen. Der komplexe Remote-Protokollparser läuft nach Möglichkeit in einem isolierten Debug Broker statt direkt im Kernel.

Software-Breakpoints und Kernelcodepatches müssen NX und W^X einhalten. Eine Debug Session verwaltet sämtliche temporären Breakpoints, Registeränderungen und Mappings und entfernt sie bei Ablauf oder Widerruf.

Hardware-Debugports wie JTAG oder SWD sind auf Produktionsgeräten standardmäßig gesperrt. Eine Aktivierung benötigt eine gerätegebundene Autorisierung und in der Regel Physical Presence.

Alle privilegierten Debugvorgänge werden lokal auditiert. Debugdaten und Crash-Dumps werden nicht automatisch an externe Dienste übertragen. Post-Mortem-Debugging in einer verifizierten Recovery-Umgebung wird gegenüber Live-Kerneldebugging bevorzugt.