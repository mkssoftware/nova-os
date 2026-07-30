# ADR-PROC-0010 – Prozessgruppen

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0010 |
| Titel | Prozessgruppen |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Sessions / Signalisierung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0009 |
| Nachfolgende Spezifikation | NPSPEC-PROCESSGROUP-0001 |

---

## 1. Kontext

NovaOS benötigt neben hierarchischen Job Objects auch eine leichtgewichtige Möglichkeit, logisch zusammenarbeitende Prozesse zu gruppieren.

Typische Anwendungsfälle sind:

- eine Shell-Pipeline,
- mehrere Prozesse eines Konsolenbefehls,
- Vordergrund- und Hintergrundaufträge eines Terminals,
- gemeinsame Signalisierung,
- Debugging mehrerer zusammengehöriger Prozesse,
- Kompatibilität mit POSIX- und Unix-Anwendungen,
- kontrollierte Terminalzuordnung,
- gemeinsame Warte- und Statusabfragen.

Beispiel einer Shell-Pipeline:

```text
producer | filter | consumer
```

Diese Pipeline besteht aus mehreren Prozessen, soll aber von der Shell als eine zusammengehörige Ausführungseinheit behandelt werden.

Die Shell muss beispielsweise:

- die gesamte Pipeline anhalten,
- die gesamte Pipeline fortsetzen,
- ein Abbruchereignis an alle Prozesse senden,
- auf die Beendigung aller Prozesse warten,
- die Pipeline als Vordergrundgruppe eines Terminals festlegen

können.

Job Objects aus ADR-PROC-0009 sind dafür allein nicht geeignet. Sie dienen primär der hierarchischen Ressourcenverwaltung, Isolation, Abrechnung und Lebenszyklussteuerung.

Prozessgruppen sind dagegen eine flache, leichtgewichtige Gruppierung für Signalisierung, Terminalsteuerung und Kompatibilität.

---

## 2. Problemstellung

Ohne native Prozessgruppen müsste eine Shell oder Anwendung alle beteiligten Prozess-Handles selbst verwalten.

Dabei entstehen mehrere Probleme:

- Prozesse können zwischen Abfrage und Signalisierung beendet werden.
- Prozess-IDs können wiederverwendet werden.
- neue Pipelineprozesse könnten übersehen werden.
- gemeinsame Stop-, Continue- und Terminate-Ereignisse wären nicht atomar adressierbar.
- Vordergrund- und Hintergrundgruppen eines Terminals wären schwer abzubilden.
- POSIX-Kompatibilität müsste vollständig in Userspace emuliert werden.
- Debugger und Supervisoren hätten keine stabile Gruppenidentität.

Prozessgruppen müssen dennoch klar von Job Objects getrennt bleiben, damit keine zwei konkurrierenden Ressourcenhierarchien entstehen.

---

## 3. Entscheidungsziele

Prozessgruppen sollen:

1. mehrere Prozesse leichtgewichtig zusammenfassen,
2. atomare Gruppensignalisierung ermöglichen,
3. Shell-Pipelines und Job Control unterstützen,
4. Vordergrund- und Hintergrundgruppen eines Terminals abbilden,
5. POSIX-Kompatibilität erleichtern,
6. stabile Kernelobjekte statt reiner Prozess-ID-Listen verwenden,
7. Race Conditions bei Gruppenoperationen vermeiden,
8. Sessions und kontrollierende Terminals unterstützen,
9. klar von Job Objects abgegrenzt bleiben,
10. sichere Handle- und Capability-Prüfungen verwenden,
11. Prozessbeendigung und Gruppenauflösung korrekt behandeln,
12. lokale Diagnose und Telemetrie bereitstellen.

---

## 4. Begriffsdefinitionen

### 4.1 Prozessgruppe

Eine Prozessgruppe ist ein flaches Kernelobjekt, das mehrere Prozesse für gemeinsame Signalisierung, Terminalsteuerung, Warten und Statusabfragen zusammenfasst.

---

### 4.2 Process Group ID

Die Process Group ID, kurz PGID, ist eine stabile Kennung einer Prozessgruppe innerhalb einer laufenden NovaOS-Instanz und des zugehörigen PID-Namespace.

---

### 4.3 Gruppenleiter

Der Gruppenleiter ist der Prozess, dessen Prozess-ID bei kompatiblen Schnittstellen der ursprünglichen PGID entsprechen kann.

Der Gruppenleiter besitzt keine automatische Verwaltungsautorität über alle Gruppenmitglieder.

---

### 4.4 Session

Eine Session ist ein Kernelobjekt, das eine oder mehrere Prozessgruppen für Terminal- und Anmeldesemantik zusammenfasst.

---

### 4.5 Session Leader

Der Session Leader ist der Prozess, der eine neue Session erzeugt hat.

---

### 4.6 Vordergrund-Prozessgruppe

Die Vordergrund-Prozessgruppe ist die Prozessgruppe einer Session, die momentan direkte Terminaleingaben und bestimmte terminalgenerierte Ereignisse empfangen darf.

---

### 4.7 Hintergrund-Prozessgruppe

Eine Hintergrund-Prozessgruppe gehört zur gleichen Session, ist aber nicht die aktuelle Vordergrundgruppe des kontrollierenden Terminals.

---

### 4.8 Orphaned Process Group

Eine verwaiste Prozessgruppe ist eine Prozessgruppe, deren relevante Elternprozesse außerhalb der Gruppe nicht mehr in derselben Session existieren.

Die genaue Semantik wird in der POSIX-Kompatibilitätsspezifikation definiert.

---

## 5. Abgrenzung zu Job Objects

Prozessgruppen und Job Objects erfüllen unterschiedliche Aufgaben.

| Eigenschaft | Prozessgruppe | Job Object |
|---|---|---|
| Hauptzweck | Signalisierung und Terminalsteuerung | Ressourcenverwaltung und Isolation |
| Struktur | flach | hierarchisch |
| Ressourcenlimits | nein | ja |
| CPU- und Speicherabrechnung | nur Statistik | verbindliche Limits und Abrechnung |
| Shell Job Control | ja | nicht primär |
| Terminal-Vordergrundgruppe | ja | nein |
| rekursive Child Jobs | nein | ja |
| Kill-on-Close | standardmäßig nein | optional ja |
| Prozessmitgliedschaft | maximal eine direkte Gruppe | genau ein direkter Job |
| POSIX-Kompatibilität | zentral | ergänzend |

Ein Prozess kann gleichzeitig:

- einem Job Object,
- einer Prozessgruppe,
- einer Session,
- einer Sicherheitsdomäne

angehören.

Diese Mitgliedschaften sind voneinander unabhängig, soweit keine übergeordnete Sicherheitsrichtlinie sie einschränkt.

---

## 6. Betrachtete Alternativen

### 6.1 Keine Kernel-Prozessgruppen

Shells und Anwendungen verwalten Prozesslisten ausschließlich im Userspace.

#### Vorteile

- keine zusätzliche Kernelstruktur,
- flexible Userspace-Implementierung.

#### Nachteile

- Race Conditions,
- Prozess-ID-Wiederverwendung,
- keine atomare Gruppensignalisierung,
- schwierige Terminalintegration,
- schlechte POSIX-Kompatibilität.

Diese Alternative wird verworfen.

---

### 6.2 Verwendung von Job Objects für alle Gruppenaufgaben

Shell-Pipelines werden jeweils als Job Objects dargestellt.

#### Vorteile

- Wiederverwendung vorhandener Gruppenmechanismen,
- gemeinsame Beendigung bereits vorhanden,
- Ressourcenverwaltung wäre automatisch möglich.

#### Nachteile

- Job Objects sind für einfache Pipelines zu schwergewichtig,
- hierarchische Ressourcenregeln sind nicht immer gewünscht,
- Session- und Terminalsemantik würden Job Objects überladen,
- POSIX-Prozessgruppen und NovaOS-Jobs hätten unterschiedliche Erwartungen.

Diese Alternative wird verworfen.

---

### 6.3 Prozessgruppen nur als numerische PGID

Die Gruppe existiert nur als gleiche Nummer in den Prozessobjekten.

#### Vorteile

- geringer Speicherbedarf,
- einfache POSIX-ähnliche Darstellung.

#### Nachteile

- kein stabiles Kernelobjekt,
- schwierige Referenzzählung,
- Race Conditions bei Auflösung,
- keine Handle-basierte Sicherheit,
- begrenzte Diagnosefähigkeit.

Diese Alternative wird verworfen.

---

### 6.4 Prozessgruppen als eigenständige Kernelobjekte

Jede Prozessgruppe wird als referenzgezähltes Kernelobjekt mit Mitgliederliste, Sessionreferenz und Ereigniszustand dargestellt.

#### Vorteile

- stabile Identität,
- atomare Gruppenoperationen,
- sichere Handles,
- saubere Session- und Terminalintegration,
- gute Diagnosefähigkeit,
- POSIX-Kompatibilität möglich.

#### Nachteile

- zusätzliche Kernelobjekte,
- Mitgliedschafts- und Lebenszykluslogik erforderlich,
- Synchronisation mit Prozessbeendigung notwendig.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS führt Prozessgruppen als eigenständige, flache und referenzgezählte Kernelobjekte ein.

Jeder Userspace-Prozess gehört zu jedem Zeitpunkt höchstens einer Prozessgruppe an.

Eine Prozessgruppe:

- besitzt eine unveränderliche PGID,
- gehört genau einer Session an,
- enthält null oder mehr Prozesse,
- kann eine Vordergrundgruppe eines Terminals sein,
- unterstützt atomare Gruppenoperationen,
- besitzt keine eigenen Ressourcenlimits,
- bildet keine Hierarchie,
- ersetzt kein Job Object.

Die Mitgliedschaft wird im Prozessobjekt gespeichert.

```c
process->process_group
```

---

## 8. Systemprozesse ohne normale Prozessgruppe

Frühe Kernel- und Systemprozesse können während des Bootstraps vorübergehend keiner normalen Userspace-Prozessgruppe angehören.

Bevor ein regulärer Userspace-Prozess vollständig gestartet wird, muss jedoch eine gültige Prozessgruppen- und Sessionzuordnung feststehen.

Mögliche Ausnahmen sind:

- interne Kernelprozesse,
- frühe Bootstrap-Prozesse,
- spezielle Recovery-Kontexte.

Diese Ausnahmen dürfen nicht für normale Anwendungen verwendet werden.

---

## 9. Flache Struktur

Prozessgruppen sind nicht hierarchisch.

Eine Prozessgruppe kann:

- keine Child-Prozessgruppen besitzen,
- keiner Parent-Prozessgruppe untergeordnet sein,
- keine Richtlinien an andere Prozessgruppen vererben.

Hierarchie wird ausschließlich über:

- Sessions für Terminalorganisation,
- Job Objects für Ressourcenverwaltung,
- Eltern-Kind-Prozessbeziehungen für Abstammung

abgebildet.

---

## 10. Gruppenidentität

Jede Prozessgruppe besitzt:

- eine unveränderliche Kernelobjekt-ID,
- eine PGID im zugehörigen PID-Namespace,
- eine Referenz auf ihre Session,
- eine Mitgliederliste,
- einen Zustand,
- eine Generation.

PGID und Kernelobjekt-ID sind getrennte Konzepte.

Eine PGID kann nach endgültiger Freigabe der alten Gruppe später wiederverwendet werden. Ein bestehendes Handle verweist jedoch immer auf das ursprüngliche Kernelobjekt und niemals auf eine spätere Gruppe mit derselben numerischen PGID.

---

## 11. Gruppenleiter

Beim Erzeugen einer Prozessgruppe kann ein Prozess zum Gruppenleiter werden.

Für POSIX-Kompatibilität gilt typischerweise:

```text
PGID = PID des Gruppenleiters
```

NovaOS trennt jedoch:

- numerische Gruppenidentität,
- Kernelobjekt,
- Verwaltungsrechte.

Der Gruppenleiter:

- darf beendet werden,
- muss nicht der älteste Prozess bleiben,
- besitzt keine automatische Kontrolle über alle Mitglieder,
- wird nach seiner Beendigung nicht automatisch ersetzt.

Die Prozessgruppe kann nach dem Ende ihres ursprünglichen Gruppenleiters weiterbestehen, solange Mitglieder oder Kernelreferenzen vorhanden sind.

---

## 12. Sessionzugehörigkeit

Jede Prozessgruppe gehört genau einer Session an.

Alle Mitgliedsprozesse müssen derselben Session wie die Prozessgruppe angehören.

Ein Prozess darf nicht in eine Prozessgruppe einer anderen Session verschoben werden.

```text
process.session == process_group.session
```

Ein Sessionwechsel eines Prozesses erfordert deshalb die Erzeugung oder Auswahl einer passenden Prozessgruppe innerhalb der neuen Session.

---

## 13. Erzeugung einer Prozessgruppe

Eine neue Prozessgruppe wird durch einen berechtigten Prozess oder Systemdienst erzeugt.

Der Ablauf lautet:

1. aufrufenden Prozess validieren,
2. Ziel-Session bestimmen,
3. PGID reservieren,
4. Prozessgruppenobjekt erzeugen,
5. Gruppenleiter festlegen,
6. Prozess atomar zuordnen,
7. Gruppenobjekt veröffentlichen,
8. Ereignis erzeugen.

Die Gruppe darf nicht öffentlich sichtbar werden, bevor ihr grundlegender Zustand vollständig initialisiert ist.

---

## 14. Prozesszuordnung bei Erzeugung

Die Prozesserzeugungs-API muss eine atomare Zuordnung zu einer Prozessgruppe unterstützen.

Dadurch kann eine Shell eine Pipeline ohne Zwischenzustand aufbauen.

```text
Prozessobjekt erzeugen
        ↓
Job Object zuordnen
        ↓
Session zuordnen
        ↓
Prozessgruppe zuordnen
        ↓
Sicherheitsrichtlinien anwenden
        ↓
ersten Thread ausführbar machen
```

Ein Prozess darf nicht kurzzeitig in einer falschen Vordergrund- oder Hintergrundgruppe laufen.

---

## 15. Vererbung

Ein neuer Prozess erbt standardmäßig:

- die Session,
- die Prozessgruppe,
- das Job Object

seines Elternprozesses.

Eine Shell oder ein berechtigter Starter kann bei der Prozesserzeugung eine andere Prozessgruppe derselben Session angeben.

Ein Kindprozess darf sich nicht eigenständig einer Prozessgruppe einer fremden Session anschließen.

---

## 16. Wechsel der Prozessgruppe

Ein Prozess kann innerhalb seiner Session einer anderen Prozessgruppe zugeordnet werden, wenn die Sicherheits- und Kompatibilitätsregeln dies erlauben.

Der Wechsel erfolgt atomar:

1. Quellgruppe sperren,
2. Zielgruppe sperren,
3. Sessiongleichheit prüfen,
4. Berechtigung prüfen,
5. Prozess aus Quellgruppe entfernen,
6. Prozess in Zielgruppe einfügen,
7. Prozessreferenz aktualisieren,
8. Gruppenereignisse erzeugen.

Für mehrere Gruppensperren gilt eine stabile Sperrreihenfolge anhand der Kernelobjekt-ID.

---

## 17. Einschränkungen beim Gruppenwechsel

Ein Gruppenwechsel kann abgelehnt werden, wenn:

- Ziel- und Quellgruppe unterschiedlichen Sessions angehören,
- die erforderliche Capability fehlt,
- der Prozess bereits beendet wird,
- eine Terminaloperation gleichzeitig läuft,
- eine Debug- oder Sicherheitsrichtlinie den Wechsel verbietet,
- die Zielgruppe bereits aufgelöst wird,
- POSIX-Kompatibilitätsregeln widersprechen.

Ein Prozess darf eine Gruppenoperation nicht verwenden, um Job-, Ressourcen- oder Sicherheitsgrenzen zu umgehen.

---

## 18. Prozessgruppen und Job Objects

Mitglieder derselben Prozessgruppe können unterschiedlichen Job Objects angehören, sofern eine Systemrichtlinie dies erlaubt.

Dies ist für allgemeine Prozessgruppen flexibel, kann aber die gemeinsame Lebenszyklussteuerung erschweren.

Eine Shell-Pipeline soll normalerweise innerhalb desselben Job Objects oder eines gemeinsamen Child Jobs laufen.

Eine strengere Systemrichtlinie kann verlangen:

```text
alle Mitglieder einer Prozessgruppe
    müssen im selben Job oder in derselben Job-Unterhierarchie liegen
```

Prozessgruppensignale überschreiben keine Job- oder Capability-Grenzen.

---

## 19. Gruppensignalisierung

Ein berechtigter Aufrufer kann ein Ereignis oder Signal an alle aktuellen Mitglieder einer Prozessgruppe senden.

Konzeptionell:

```c
nova_status_t nova_process_group_signal(
    nova_handle_t process_group,
    const nova_process_signal_t* signal
);
```

Der Kernel erstellt für die Operation eine konsistente Sicht auf die Mitglieder.

Prozesse, die nach dem festgelegten linearen Operationspunkt beitreten, müssen das Signal nicht rückwirkend erhalten.

Prozesse, die davor Mitglieder waren, werden entsprechend den Berechtigungsregeln berücksichtigt.

---

## 20. Berechtigungsprüfung bei Gruppensignalen

Eine Gruppensignalisierung bedeutet nicht automatisch, dass der Aufrufer jeden Mitgliedsprozess kontrollieren darf.

NovaOS unterstützt zwei Modelle:

### 20.1 Vollständig autorisierte Gruppenoperation

Der Aufrufer besitzt eine Capability für die gesamte Prozessgruppe.

Das Signal kann an alle Mitglieder zugestellt werden.

### 20.2 Mitgliedsweise Autorisierung

Der Kernel prüft die Berechtigung für jeden Mitgliedsprozess einzeln.

Nicht autorisierte Mitglieder werden übersprungen oder führen abhängig vom API-Modus zu einem Fehler.

Das Ergebnis muss angeben, ob:

- alle Prozesse erreicht wurden,
- einige Prozesse übersprungen wurden,
- kein Prozess erreicht wurde.

---

## 21. Gruppenoperationen

Prozessgruppen unterstützen mindestens folgende Operationen:

- Signal oder Ereignis senden,
- Stop anfordern,
- Continue anfordern,
- Terminate anfordern,
- auf Zustandsänderung warten,
- Mitglieder auflisten,
- Vordergrundstatus abfragen,
- aggregierte Statusinformationen abfragen.

Ressourcenlimits und rekursive Child-Job-Steuerung bleiben Aufgaben der Job Objects.

---

## 22. Stop einer Prozessgruppe

Eine Stop-Operation fordert alle betroffenen Mitgliedsprozesse auf, ihre ausführbaren Threads anzuhalten.

Der Prozesszustand wird entsprechend aktualisiert.

Threads, die bereits blockiert sind, bleiben in ihrem ursprünglichen Wartezustand, werden aber zusätzlich als gruppengestoppt markiert.

Nach Aufhebung des Stop-Zustands:

- zuvor lauffähige Threads werden wieder lauffähig,
- zuvor blockierte Threads bleiben blockiert,
- beendete Threads bleiben beendet.

---

## 23. Continue einer Prozessgruppe

Eine Continue-Operation entfernt den gruppenbezogenen Stop-Zustand.

Sie hebt keine anderen Stop- oder Suspend-Gründe auf.

Ein Thread kann gleichzeitig angehalten sein durch:

- Prozessgruppen-Stop,
- Debugger,
- Job-Pause,
- administratives Suspend,
- System-Recovery.

Der Thread wird erst wieder ausführbar, wenn kein aktiver Stop-Grund mehr besteht.

---

## 24. Terminate einer Prozessgruppe

Eine Terminate-Operation sendet eine Beendigungsanforderung an alle berechtigten aktuellen Mitglieder.

Sie ist keine automatische rekursive Jobbeendigung.

Prozesse, die nach Beginn der Operation in die Gruppe eintreten, werden standardmäßig nicht rückwirkend erfasst.

Für eine garantierte Lebenszyklusbeendigung einschließlich später erzeugter Kindprozesse muss ein Job Object verwendet werden.

---

## 25. Warten auf eine Prozessgruppe

Ein Prozess oder Supervisor kann auf definierte Gruppenzustände warten.

Mögliche Wartebedingungen:

```text
ANY_MEMBER_EXITED
ALL_MEMBERS_EXITED
GROUP_EMPTY
GROUP_STOPPED
GROUP_CONTINUED
FOREGROUND_CHANGED
GROUP_DESTROYED
```

Das Warten erfolgt über Kernel-Warteobjekte und benötigt keine aktive Polling-Schleife.

Eine leere Gruppe und eine zerstörte Gruppe sind unterschiedliche Zustände.

---

## 26. Leere Prozessgruppe

Eine Prozessgruppe gilt als leer, wenn sie keine Mitgliedsprozesse mehr enthält.

Eine leere Gruppe kann kurzfristig weiterbestehen, wenn:

- Handles auf sie verweisen,
- sie als Vordergrundgruppe eines Terminals eingetragen ist,
- eine Gruppenoperation noch aktiv ist,
- Diagnose- oder Kompatibilitätsreferenzen existieren.

Neue Prozesse dürfen einer leeren, aber noch aktiven Gruppe nur beitreten, wenn die Gruppenpolicy dies erlaubt.

POSIX-kompatible Gruppen können nach dem Austritt des letzten Mitglieds für neue Mitgliedschaften geschlossen werden.

---

## 27. Auflösung einer Prozessgruppe

Eine Prozessgruppe wird endgültig aufgelöst, wenn:

- sie keine Mitglieder besitzt,
- sie nicht mehr als Terminal-Vordergrundgruppe referenziert wird,
- keine externen Handles oder Kernelreferenzen bestehen,
- keine Gruppenoperation aktiv ist,
- ihr Zustand eine Freigabe erlaubt.

Die PGID darf erst nach der endgültigen Auflösung zur Wiederverwendung freigegeben werden.

---

## 28. Gruppenbeendigung beim Exit des Leiters

Die Beendigung des Gruppenleiters beendet nicht automatisch die gesamte Prozessgruppe.

Andere Mitglieder bleiben in der Gruppe.

Die PGID bleibt gültig, solange die Gruppe existiert.

Ein neuer Gruppenleiter wird nicht automatisch bestimmt.

Diagnosewerkzeuge können den Zustand kennzeichnen als:

```text
leader_alive = false
```

---

## 29. Sessions

Eine Session fasst eine oder mehrere Prozessgruppen zusammen.

Typische Struktur:

```text
Session
├── Vordergrund-Prozessgruppe
├── Hintergrund-Prozessgruppe A
└── Hintergrund-Prozessgruppe B
```

Eine Session besitzt:

- eine Session-ID,
- optional einen Session Leader,
- optional ein kontrollierendes Terminal,
- eine Menge von Prozessgruppen,
- eine Vordergrundgruppenreferenz,
- Sicherheits- und Kompatibilitätsmetadaten.

---

## 30. Erzeugung einer Session

Ein berechtigter Prozess kann eine neue Session erzeugen.

Dabei wird typischerweise:

1. eine neue Session-ID reserviert,
2. ein Sessionobjekt erzeugt,
3. eine neue Prozessgruppe erzeugt,
4. der aufrufende Prozess zum Session Leader,
5. der aufrufende Prozess zum Gruppenleiter,
6. die alte Prozessgruppenzuordnung entfernt,
7. der Prozess der neuen Gruppe zugeordnet.

Die Operation wird atomar ausgeführt.

---

## 31. Session Leader

Der Session Leader ist der Prozess, der die Session erzeugt hat.

Der Session Leader:

- besitzt nicht automatisch alle Verwaltungsrechte,
- kann ein kontrollierendes Terminal erwerben,
- kann beendet werden,
- wird nach seinem Ende nicht automatisch ersetzt.

Eine Session kann weiterbestehen, solange Prozessgruppen oder Kernelreferenzen vorhanden sind.

---

## 32. Kontrollierendes Terminal

Eine Session kann höchstens ein kontrollierendes Terminal besitzen.

Ein Terminal kann höchstens einer kontrollierenden Session zugeordnet sein, sofern die Terminalpolicy nichts anderes festlegt.

Die Zuordnung wird als Kernelreferenz verwaltet.

Ein Prozess darf ein Terminal nicht allein durch Kenntnis einer numerischen ID übernehmen.

Die Übernahme benötigt:

- ein Terminalhandle,
- eine passende Session,
- erforderliche Capabilities,
- Einhaltung der Terminalpolicy.

---

## 33. Vordergrund-Prozessgruppe

Ein kontrollierendes Terminal besitzt höchstens eine Vordergrund-Prozessgruppe.

Diese Gruppe muss:

- zur kontrollierenden Session gehören,
- aktiv oder kontrolliert leer sein,
- über ein gültiges Gruppenobjekt referenziert werden.

Die Vordergrundgruppe darf Terminaleingaben empfangen.

Terminalgenerierte Ereignisse werden an diese Gruppe zugestellt.

---

## 34. Hintergrund-Prozessgruppen

Alle anderen Prozessgruppen derselben Session gelten relativ zum Terminal als Hintergrundgruppen.

Hintergrundgruppen können abhängig von der Terminalpolicy eingeschränkt werden bei:

- Terminaleingaben,
- Terminalkonfigurationsänderungen,
- bestimmten Terminalausgaben.

Ein unzulässiger Hintergrundzugriff kann:

- abgelehnt werden,
- ein Gruppen-Stop-Ereignis auslösen,
- ein Kompatibilitätssignal erzeugen,
- als Fehler an den aufrufenden Prozess zurückgegeben werden.

---

## 35. Terminalgenerierte Ereignisse

Ein Terminal kann Ereignisse an seine Vordergrund-Prozessgruppe senden.

Beispiele:

- Interrupt,
- Quit,
- Suspend,
- Terminalgrößenänderung,
- Verbindungsabbruch.

Die konkrete Tastenzuordnung wird durch die Terminalkonfiguration bestimmt.

Das Terminal adressiert das Prozessgruppenobjekt über eine stabile Kernelreferenz und nicht ausschließlich über eine numerische PGID.

---

## 36. Wechsel der Vordergrundgruppe

Ein berechtigter Prozess kann die Vordergrund-Prozessgruppe eines Terminals ändern.

Der Kernel prüft:

- Terminalhandle,
- Sessiongleichheit,
- Prozessgruppenstatus,
- Capabilities,
- Sicherheitsrichtlinien.

Die Änderung wird atomar veröffentlicht.

```text
alte Vordergrundgruppe
        ↓
Terminalreferenz wechseln
        ↓
neue Vordergrundgruppe
```

Wartende Terminaloperationen müssen die neue Generation beachten.

---

## 37. Terminalverlust

Wird ein kontrollierendes Terminal getrennt oder zerstört, erzeugt NovaOS definierte Session- und Prozessgruppenereignisse.

Mögliche Reaktionen:

- Hangup-Ereignis an die Vordergrundgruppe,
- Benachrichtigung des Session Leaders,
- Aufhebung der Terminalreferenz,
- Stop oder Beendigung bestimmter Gruppen nach Policy.

Der Terminalverlust beendet nicht automatisch das zugehörige Job Object.

---

## 38. Verwaiste Prozessgruppen

NovaOS erkennt verwaiste Prozessgruppen für POSIX-kompatibles Job Control.

Eine Gruppe kann als verwaist gelten, wenn kein Mitglied einen relevanten Elternprozess außerhalb der Gruppe, aber innerhalb derselben Session besitzt.

Wird eine gestoppte Prozessgruppe verwaist, kann die Kompatibilitätsschicht definierte Ereignisse erzeugen, damit Prozesse nicht dauerhaft unbeaufsichtigt angehalten bleiben.

Die genaue POSIX-Signalabbildung wird in der Kompatibilitätsspezifikation festgelegt.

---

## 39. PID-Namespaces

PGIDs gelten innerhalb eines PID-Namespace.

Dasselbe Prozessgruppenobjekt kann je nach Namespace unterschiedlich sichtbar oder gar nicht sichtbar sein.

Eine gruppenbezogene Operation über eine numerische PGID wird immer relativ zum aufrufenden PID-Namespace aufgelöst.

Handles bleiben davon unabhängig und verweisen direkt auf das Kernelobjekt.

Namespacegrenzen dürfen keine unberechtigten Gruppenoperationen ermöglichen.

---

## 40. Handles statt nur numerischer IDs

NovaOS verwendet für native APIs bevorzugt Handles.

Numerische PGIDs dienen hauptsächlich:

- Kompatibilität,
- Diagnose,
- Shellanzeige,
- Namensraumauflösung.

Ein Handle schützt vor:

- ID-Wiederverwendung,
- Race Conditions,
- Zugriff auf eine spätere Gruppe mit gleicher PGID,
- fehlender Rechtebindung.

Native Verwaltungsoperationen sollen daher ein Prozessgruppenhandle verwenden.

---

## 41. Sicherheitsmodell

Prozessgruppen sind Capability-geschützte Kernelobjekte.

Mögliche Capabilities sind:

```text
CAP_PROCESS_GROUP_CREATE
CAP_PROCESS_GROUP_JOIN
CAP_PROCESS_GROUP_MOVE
CAP_PROCESS_GROUP_SIGNAL
CAP_PROCESS_GROUP_STOP
CAP_PROCESS_GROUP_CONTINUE
CAP_PROCESS_GROUP_TERMINATE
CAP_PROCESS_GROUP_QUERY
CAP_SESSION_CREATE
CAP_SESSION_CONTROL
CAP_TERMINAL_SET_FOREGROUND
CAP_PROCESS_GROUP_ADMIN
```

Capabilities können eingeschränkt werden auf:

- eine bestimmte Prozessgruppe,
- eine bestimmte Session,
- bestimmte Signal- oder Ereignistypen,
- bestimmte Gruppenoperationen,
- eigene Nachkommen,
- dieselbe Sicherheitsdomäne.

---

## 42. Gruppenhandles

Ein Prozessgruppenhandle enthält explizite Rechte.

Mögliche Rechte sind:

```text
PROCESS_GROUP_QUERY
PROCESS_GROUP_ENUMERATE
PROCESS_GROUP_SIGNAL
PROCESS_GROUP_STOP
PROCESS_GROUP_CONTINUE
PROCESS_GROUP_TERMINATE
PROCESS_GROUP_ADD_MEMBER
PROCESS_GROUP_REMOVE_MEMBER
PROCESS_GROUP_WAIT
PROCESS_GROUP_SET_FOREGROUND
```

Ein Beobachtungshandle gewährt keine Verwaltungsrechte.

Das Schließen des letzten Handles beendet die Mitgliedsprozesse nicht automatisch.

---

## 43. API-Grundmodell

Die native API soll mindestens folgende Operationen unterstützen:

```c
nova_status_t nova_process_group_create(
    const nova_process_group_create_info_t* create_info,
    nova_handle_t* process_group
);

nova_status_t nova_process_group_open(
    nova_process_group_id_t process_group_id,
    uint32_t requested_rights,
    nova_handle_t* process_group
);

nova_status_t nova_process_group_assign(
    nova_handle_t process_group,
    nova_handle_t process
);

nova_status_t nova_process_group_signal(
    nova_handle_t process_group,
    const nova_process_signal_t* signal,
    nova_process_group_signal_result_t* result
);

nova_status_t nova_process_group_stop(
    nova_handle_t process_group
);

nova_status_t nova_process_group_continue(
    nova_handle_t process_group
);

nova_status_t nova_process_group_terminate(
    nova_handle_t process_group,
    nova_status_t exit_status
);

nova_status_t nova_process_group_query(
    nova_handle_t process_group,
    nova_process_group_info_t* info
);

nova_status_t nova_process_group_wait(
    nova_handle_t process_group,
    nova_process_group_wait_condition_t condition,
    uint64_t timeout_ns
);
```

Die endgültige ABI wird in einer separaten Spezifikation festgelegt.

---

## 44. Erzeugungsinformationen

Eine mögliche Erzeugungsstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    nova_handle_t session;
    nova_handle_t initial_process;

    nova_process_group_id_t requested_id;

    uint32_t flags;
    uint32_t reserved;
} nova_process_group_create_info_t;
```

Mögliche Flags sind:

```c
typedef enum
{
    NOVA_PROCESS_GROUP_FLAG_NONE           = 0,
    NOVA_PROCESS_GROUP_FLAG_POSIX_COMPAT   = 1u << 0,
    NOVA_PROCESS_GROUP_FLAG_ALLOW_REJOIN   = 1u << 1,
    NOVA_PROCESS_GROUP_FLAG_CLOSE_ON_EMPTY = 1u << 2
} nova_process_group_flags_t;
```

---

## 45. Informationsstruktur

Eine mögliche Informationsstruktur lautet:

```c
typedef struct
{
    nova_process_group_id_t process_group_id;
    nova_session_id_t session_id;
    nova_process_id_t leader_process_id;

    uint64_t direct_process_count;
    uint64_t stopped_process_count;
    uint64_t running_process_count;

    bool leader_alive;
    bool is_foreground;
    bool is_orphaned;
    bool is_empty;

    uint32_t state;
    uint32_t flags;

    uint64_t generation;
} nova_process_group_info_t;
```

---

## 46. Session-API

Die Session- und Terminalsteuerung soll mindestens folgende Operationen unterstützen:

```c
nova_status_t nova_session_create(
    const nova_session_create_info_t* create_info,
    nova_handle_t* session
);

nova_status_t nova_session_set_controlling_terminal(
    nova_handle_t session,
    nova_handle_t terminal
);

nova_status_t nova_terminal_set_foreground_group(
    nova_handle_t terminal,
    nova_handle_t process_group
);

nova_status_t nova_terminal_get_foreground_group(
    nova_handle_t terminal,
    nova_handle_t* process_group
);
```

Session- und Prozessgruppen-APIs werden getrennt versioniert.

---

## 47. Kernelinternes Datenmodell

Ein Prozessgruppenobjekt enthält konzeptionell:

```c
typedef struct nova_process_group
{
    nova_kernel_object_t object;

    nova_process_group_id_t id;
    nova_process_group_state_t state;

    struct nova_session* session;
    struct nova_process* original_leader;

    nova_list_t members;

    nova_lock_t member_lock;

    uint64_t generation;
    uint32_t flags;

    bool leader_alive;
    bool orphaned;
    bool accepting_members;
} nova_process_group_t;
```

Die konkrete Speicheranordnung bleibt der Implementierung überlassen.

---

## 48. Session-Datenmodell

Ein Sessionobjekt enthält konzeptionell:

```c
typedef struct nova_session
{
    nova_kernel_object_t object;

    nova_session_id_t id;

    struct nova_process* leader;
    struct nova_terminal* controlling_terminal;
    struct nova_process_group* foreground_group;

    nova_list_t process_groups;

    nova_lock_t session_lock;

    uint64_t generation;
    uint32_t flags;
} nova_session_t;
```

Referenzen auf Terminal und Vordergrundgruppe müssen gegen parallele Auflösung geschützt sein.

---

## 49. Synchronisation

Mitgliedschaftsänderungen werden durch die Prozessgruppensperre geschützt.

Operationen über Prozessgruppe und Session müssen einer festen Sperrreihenfolge folgen.

Eine mögliche Reihenfolge lautet:

```text
Session
    ↓
Prozessgruppe nach Objekt-ID
    ↓
Prozess
```

Gruppensignalisierung darf keine Gruppen- oder Session-Sperre während einer potenziell blockierenden Signalzustellung halten.

Stattdessen wird eine stabile, referenzgezählte Mitgliedersicht erzeugt.

---

## 50. Generationenmodell

Jede relevante Änderung erhöht die Prozessgruppengeneration.

Dazu gehören:

- Prozessbeitritt,
- Prozessaustritt,
- Zustandsänderung,
- Wechsel der Vordergrundrolle,
- Änderung des Orphaned-Zustands,
- Schließen der Gruppe für neue Mitglieder.

Sessions und Terminals besitzen zusätzlich eigene Generationen.

Generationen ermöglichen die Erkennung veralteter Abfragen und Terminaloperationen.

---

## 51. Telemetrie und Diagnose

Für Prozessgruppen werden mindestens folgende Informationen erfasst:

- PGID,
- Session-ID,
- ursprünglicher Gruppenleiter,
- Gruppenleiterstatus,
- Mitgliederanzahl,
- Mitgliedsänderungen,
- Vordergrundstatus,
- Orphaned-Status,
- Stop- und Continue-Ereignisse,
- Gruppensignale,
- fehlgeschlagene Zustellungen,
- Erzeugungs- und Auflösungszeitpunkt,
- Prozessgruppengeneration.

Diagnosewerkzeuge sollen Prozessgruppen, Sessions, Jobs und Prozessbäume getrennt darstellen können.

---

## 52. Kompatibilität

NovaOS bildet POSIX-Prozessgruppen und Sessions auf die nativen Kernelobjekte ab.

Kompatible Funktionen können auf folgende native Operationen abgebildet werden:

```text
setpgid()
getpgid()
getpgrp()
setsid()
getsid()
tcsetpgrp()
tcgetpgrp()
kill(-pgid, signal)
```

Die native NovaOS-API bleibt handle- und capabilitybasiert.

Numerische POSIX-Schnittstellen werden durch die Kompatibilitätsschicht validiert und in sichere Kerneloperationen übersetzt.

---

## 53. Verhalten bei Prozessbeendigung

Bei der Beendigung eines Prozesses muss der Kernel:

1. den Prozess als beendet markieren,
2. ihn aus seiner Prozessgruppe entfernen,
3. den Leiterstatus aktualisieren,
4. den Orphaned-Zustand neu berechnen,
5. gegebenenfalls ein Empty-Ereignis erzeugen,
6. Session- und Terminalreferenzen überprüfen,
7. wartende Beobachter benachrichtigen.

Die Gruppenentfernung erfolgt vor der endgültigen Freigabe des Prozessobjekts.

---

## 54. Verhalten bei Sessionbeendigung

Eine Session wird nicht allein durch die Beendigung des Session Leaders automatisch zerstört.

Sie bleibt bestehen, solange:

- Prozessgruppen vorhanden sind,
- Mitgliedsprozesse existieren,
- ein Terminal oder Handle sie referenziert.

Beim endgültigen Auflösen einer Session werden:

- die Terminalzuordnung aufgehoben,
- Vordergrundreferenzen entfernt,
- verbleibende leere Gruppen aufgelöst,
- Diagnoseereignisse erzeugt.

---

## 55. Fehlerbehandlung

Eine Prozessgruppenoperation wird abgelehnt, wenn:

- das Handle ungültig ist,
- die PGID nicht existiert,
- die erforderliche Capability fehlt,
- Prozess und Gruppe unterschiedlichen Sessions angehören,
- der Prozess bereits beendet wird,
- die Gruppe keine neuen Mitglieder akzeptiert,
- eine Terminalpolicy widerspricht,
- die Vordergrundgruppe nicht zur Terminalsession gehört,
- ein Namespacezugriff unzulässig ist,
- die Strukturversion nicht unterstützt wird.

Mögliche Statuswerte sind:

```text
NOVA_STATUS_OK
NOVA_STATUS_INVALID_ARGUMENT
NOVA_STATUS_INVALID_HANDLE
NOVA_STATUS_PROCESS_GROUP_NOT_FOUND
NOVA_STATUS_SESSION_MISMATCH
NOVA_STATUS_PERMISSION_DENIED
NOVA_STATUS_PROCESS_TERMINATING
NOVA_STATUS_PROCESS_GROUP_CLOSED
NOVA_STATUS_TERMINAL_MISMATCH
NOVA_STATUS_PARTIAL_DELIVERY
NOVA_STATUS_GROUP_EMPTY
NOVA_STATUS_TIMEOUT
NOVA_STATUS_NOT_SUPPORTED
```

Eine ungültige Userspace-Anforderung darf keinen Kernel-Panic auslösen.

---

## 56. Konsequenzen

### 56.1 Positive Konsequenzen

- native Unterstützung für Shell-Pipelines,
- atomare Gruppensignalisierung,
- saubere Terminal- und Vordergrundsteuerung,
- gute POSIX-Kompatibilität,
- stabile Gruppenidentität über Handles,
- Schutz vor Prozess-ID-Wiederverwendung,
- getrennte Zuständigkeit von Prozessgruppen und Job Objects,
- einfacheres Debugging und Warten,
- korrekte Session- und Orphaned-Semantik.

### 56.2 Negative Konsequenzen

- zusätzliche Kernelobjekte und Mitgliederlisten,
- komplexe Session- und Terminalinteraktionen,
- Gruppensignalisierung benötigt stabile Mitgliedersichten,
- POSIX-Kompatibilitätsregeln erhöhen den Testaufwand,
- Prozessgruppen- und Jobmitgliedschaft können unterschiedlich sein.

### 56.3 Risiken

- Race Conditions bei Prozessbeitritt und Gruppensignalisierung,
- veraltete Terminal-Vordergrundreferenzen,
- fehlerhafte PGID-Wiederverwendung,
- Deadlocks zwischen Session-, Gruppen- und Prozesssperren,
- unberechtigte Signale an Gruppenmitglieder,
- dauerhaft gestoppte verwaiste Gruppen,
- Verwechslung von Prozessgruppe und Job Object.

Diese Risiken werden begrenzt durch:

- eigenständige Kernelobjekte,
- Handles und Capabilities,
- Referenzzählung,
- feste Sperrreihenfolge,
- Generationen,
- atomare Mitgliedschaftsänderungen,
- klare Abgrenzung zu Job Objects.

---

## 57. Auswirkungen auf andere Subsysteme

### 57.1 Prozessverwaltung

Jeder reguläre Userspace-Prozess benötigt eine Prozessgruppen- und Sessionreferenz.

### 57.2 Job Objects

Jobmitgliedschaft und Prozessgruppenmitgliedschaft bleiben getrennte Eigenschaften.

### 57.3 Signal- und Ereignissystem

Das Signal- und Ereignissystem muss atomare Gruppenadressierung unterstützen.

### 57.4 Terminalsystem

Terminals benötigen stabile Referenzen auf Session und Vordergrund-Prozessgruppe.

### 57.5 Shell

Shells können Pipelines als Prozessgruppen erzeugen und kontrollieren.

### 57.6 PID-Namespaces

PGIDs werden innerhalb des jeweiligen PID-Namespace aufgelöst.

### 57.7 Capability-System

Gruppen-, Session- und Terminaloperationen werden durch feingranulare Capabilities geschützt.

### 57.8 Debugger

Debugger können Prozessgruppen als gemeinsame Debugziele verwenden, benötigen aber weiterhin individuelle Prozessrechte.

### 57.9 Telemetrie

Mitgliedschafts-, Signal- und Vordergrundänderungen müssen diagnostizierbar sein.

---

## 58. Implementierungsrichtlinien

1. Prozessgruppen werden als eigenständige Kernelobjekte implementiert.
2. Jeder reguläre Userspace-Prozess gehört höchstens einer Prozessgruppe an.
3. Jede Prozessgruppe gehört genau einer Session an.
4. Prozessgruppen bleiben flach und erhalten keine Parent-Child-Hierarchie.
5. Prozesszuordnung kann atomar während der Prozesserzeugung erfolgen.
6. Native APIs verwenden bevorzugt Handles statt numerischer PGIDs.
7. Gruppensignalisierung erstellt eine stabile, referenzgezählte Mitgliedersicht.
8. Die Beendigung des Gruppenleiters beendet die Gruppe nicht automatisch.
9. Prozessgruppen besitzen keine Ressourcenlimits.
10. Ressourcenverwaltung bleibt Aufgabe der Job Objects.
11. Terminalreferenzen verwenden Kernelobjekte statt ausschließlich numerischer IDs.
12. Die erste Implementierung unterstützt Erzeugung, Beitritt, Austritt, Signalisierung und Warten.
13. Sessions und Terminal-Job-Control werden als nachfolgende Ausbaustufe ergänzt.
14. POSIX-Kompatibilität wird auf die nativen Objekte abgebildet.
15. Prozessgruppenänderungen erhöhen eine Generation.

---

## 59. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- Erzeugung einer Prozessgruppe,
- Erzeugung eines Gruppenleiters,
- atomare Zuordnung beim Prozessstart,
- Vererbung durch Kindprozesse,
- Beitritt zu einer Gruppe derselben Session,
- verweigerter Beitritt zu einer fremden Session,
- Wechsel zwischen Prozessgruppen,
- paralleler Beitritt und Austritt,
- Beendigung des Gruppenleiters,
- Weiterbestehen der Gruppe ohne Leiter,
- Gruppensignalisierung,
- teilweise autorisierte Signalisierung,
- Prozessbeitritt während einer Signaloperation,
- Prozessbeendigung während einer Signaloperation,
- Stop einer Prozessgruppe,
- Continue einer Prozessgruppe,
- mehrere gleichzeitige Stop-Gründe,
- Terminate einer Prozessgruppe,
- Warten auf ein Mitglied,
- Warten auf eine leere Gruppe,
- Auflösung einer Gruppe,
- sichere PGID-Wiederverwendung,
- Sessionerzeugung,
- Session-Leader-Beendigung,
- Zuordnung eines kontrollierenden Terminals,
- Wechsel der Vordergrundgruppe,
- Hintergrundzugriff auf Terminaleingaben,
- Terminalverlust,
- verwaiste Prozessgruppe,
- POSIX-Kompatibilitätsaufrufe,
- PID-Namespace-Grenzen,
- Handle- und Capability-Prüfungen,
- Sperrreihenfolge unter hoher Parallelität,
- Prozessgruppe mit Mitgliedern aus mehreren Jobs,
- Telemetrie und Generationen.

---

## 60. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- die vollständige Signal-Semantik,
- genaue POSIX-Signalnummern,
- die komplette Terminal-API,
- konkrete Shell-Benutzeroberflächen,
- Job-Object-Ressourcenlimits,
- Container-Namespaces,
- Debuggergruppen,
- verteilte Prozessgruppen über mehrere Rechner,
- die endgültige öffentliche ABI,
- sämtliche Sonderfälle der POSIX-Job-Control-Spezifikation.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 61. Verbindliche Invarianten

1. Prozessgruppen sind eigenständige, referenzgezählte Kernelobjekte.
2. Prozessgruppen sind flach und bilden keine Hierarchie.
3. Jeder reguläre Userspace-Prozess gehört höchstens einer Prozessgruppe an.
4. Jede Prozessgruppe gehört genau einer Session an.
5. Alle Mitglieder einer Prozessgruppe gehören derselben Session an.
6. Prozessgruppen besitzen keine eigenen Ressourcenlimits.
7. Job Objects bleiben für Ressourcenverwaltung und hierarchische Lebenszyklussteuerung verantwortlich.
8. Die Beendigung des Gruppenleiters beendet die Prozessgruppe nicht automatisch.
9. Native Verwaltungsoperationen verwenden bevorzugt Handles.
10. Ein bestehendes Handle darf durch PGID-Wiederverwendung niemals auf eine andere Gruppe zeigen.
11. Prozesszuordnung und Gruppenwechsel erfolgen atomar.
12. Ein Prozess darf Prozessgruppen nicht verwenden, um Job- oder Sicherheitsgrenzen zu umgehen.
13. Gruppensignalisierung muss einen eindeutigen linearen Operationspunkt besitzen.
14. Nicht autorisierte Gruppenmitglieder dürfen keine unberechtigten Signale erhalten.
15. Stop- und Continue-Zustände dürfen andere Suspend-Gründe nicht überschreiben.
16. Die Terminal-Vordergrundgruppe muss zur kontrollierenden Session gehören.
17. Prozessbeendigung entfernt den Prozess vor seiner endgültigen Freigabe aus der Gruppe.
18. PGIDs werden innerhalb eines PID-Namespace aufgelöst.
19. Jede relevante Mitgliedschafts- oder Zustandsänderung erhöht die Prozessgruppengeneration.
20. KI- oder lernbasierte Komponenten dürfen Prozessgruppen-, Session-, Terminal-, Capability- oder Sicherheitsgrenzen nicht umgehen.

---

## 62. Referenzen

- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- ADR-PROC-0003 – Scheduler Classes
- ADR-PROC-0004 – Prioritäten
- ADR-PROC-0005 – Echtzeitprozesse
- ADR-PROC-0006 – Idle Thread
- ADR-PROC-0007 – CPU Affinity
- ADR-PROC-0008 – Context Switching
- ADR-PROC-0009 – Job Objects
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- NPSPEC-PROCESSGROUP-0001 – Process Group Specification
- NPSPEC-SESSION-0001 – Session Specification
- NPSPEC-JOB-0001 – Job Object Specification
- NPSPEC-PROCESS-0001 – Process Specification
- NPSPEC-THREAD-0001 – Thread Specification
- NPSPEC-TERMINAL-0001 – Terminal Specification

---

## 63. Zusammenfassung der Entscheidung

NovaOS führt Prozessgruppen als eigenständige, flache und referenzgezählte Kernelobjekte ein.

Prozessgruppen fassen Prozesse für gemeinsame Signalisierung, Stop-, Continue- und Terminate-Operationen sowie für Shell- und Terminal-Job-Control zusammen. Sie besitzen keine Ressourcenlimits und bilden keine Hierarchie.

Jeder reguläre Userspace-Prozess gehört höchstens einer Prozessgruppe an. Jede Prozessgruppe gehört genau einer Session, und alle ihre Mitgliedsprozesse müssen derselben Session angehören.

Prozessgruppen bleiben klar von Job Objects getrennt:

- Prozessgruppen steuern Signalisierung und Terminalverhalten.
- Job Objects steuern Ressourcen, Isolation und hierarchische Lebenszyklen.

Native NovaOS-Schnittstellen verwenden bevorzugt Handles und Capabilities. Numerische PGIDs werden hauptsächlich für POSIX-Kompatibilität, Diagnose und Namespaceauflösung verwendet.

Die Beendigung des Gruppenleiters beendet die Gruppe nicht automatisch. Prozessgruppen können weiterbestehen, solange Mitglieder oder gültige Kernelreferenzen vorhanden sind. Sessions und kontrollierende Terminals verwenden stabile Kernelreferenzen auf ihre jeweilige Vordergrund-Prozessgruppe.