# NovaOS Kernel – Gap-Analyse (Phase 0, laufend)

Dieses Dokument verfolgt den Auftrag "NovaOS Kernel Completion &
Stabilization". Wichtiger Hinweis zum Umfang: `kernel/newkernel/` enthält
aktuell **über 300 `.c`/`.h`-Module** (alle werden tatsächlich kompiliert und
gelinkt, siehe `build/*.o`). Ein vollständiges, zeilengenaues Audit aller
Module an einem Stück ist nicht seriös leistbar und würde gegen Regel 18
("keine erfundenen Prozentwerte/Behauptungen ohne Nachweis") verstoßen.

Diese Datei wird daher **inkrementell** gepflegt: jeder Eintrag basiert auf
tatsächlich gelesenem Code (Dateipfad + Zeilen zitiert), nicht auf
Vermutung. Noch nicht untersuchte Module stehen explizit unter
"Noch nicht auditiert" (Abschnitt 5) statt stillschweigend als in Ordnung
zu gelten.

## 1. Bereits durch frühere Sessions etabliert (siehe Projekt-Memory)

- **NovaFS/VFS** (`novafs.c`, 3299 Zeilen, `vfsapi.c`, 578 Zeilen):
  `COMPLETE_VERIFIED` für Objekt-/Verzeichnis-/Extent-B+-Bäume inkl. echtem
  Leaf-/Inner-/Wurzel-Splitting (`novafs_tree_insert()`, boot-verifiziert
  2026-11-23, siehe `feedback_novafs_root_directory_full.md`). VFS-Schicht
  erzwingt Capability- und Systembereichs-Policy-Prüfung bei jedem
  Schreibzugriff neu (`vfsapi.h:7-14`).
- **NEF 1.0** (`nef.c`, `nef_loader.c`, 972 Zeilen): Phasen 1-5
  boot-verifiziert (Format, Toolchain, Loader, Signaturen, Capabilities,
  dynamisches Linken). Phasen 6-7+9 offen.
- **PMM**: dynamisch (16→64 MiB + Contig-Arena), `NOVA_KERNEL_READY` wird
  erreicht.

## 2. Diese Session neu verifiziert

### 2.1 Prozessisolation / Virtual Memory Manager — **IMPLEMENTED_UNVERIFIED (Korrektur)**

Erster Verdacht (aus `vfsapi.h:25-27`, das von "keiner echten Pro-Prozess-
Seitentabellen-Isolation" spricht) hat sich bei direkter Prüfung von
`paging.c`/`ring3.c` **nicht bestätigt** — Korrektur, um keine falsche
Behauptung stehen zu lassen (Regel 18):

- `paging_create_address_space()` (`paging.c:71`) legt pro Prozess ein
  echtes, eigenständiges Page Directory an; `ring3.c:240-269` nutzt das
  explizit beim Erzeugen eines neuen Ring-3-Prozesses ("ab hier KEIN
  geteiltes Kernel-Page-Directory mehr fuer Code/Stack") und mapped
  Code-, Stack- und GUI-Puffer-Seiten einzeln in dieses Directory.
- `paging_switch_address_space()` (`paging.c:126`) wechselt beim
  Kontextwechsel tatsächlich das CR3 (`ring3.c:325`,
  `paging_switch_address_space(process_get_cr3(ring3_pid))`).
- `paging.c:318-351` hat einen eigenen Selbsttest, der zwei unabhängige
  Adressräume anlegt, zwischen ihnen wechselt und wieder zurück zum
  Kernel-Adressraum schaltet.

Die `vfsapi.h`-Aussage bezieht sich also vermutlich enger gefasst auf die
Konvention gleicher virtueller Adressen (z. B. das validierte
Syscall-Pointer-Fenster `0x400000-0x404000`, `RING3_CODE_VADDR`,
`RING3_GUI_BUFFER_VADDR`) über alle Prozesse hinweg, nicht auf einen
tatsächlich geteilten physischen Adressraum. Als `IMPLEMENTED_UNVERIFIED`
eingestuft, nicht `COMPLETE_VERIFIED`: es fehlt noch ein gezielter
Boot-Test, der zwei Ring-3-Prozesse gegenseitig auf denselben virtuellen
Adressen unterschiedliche physische Inhalte halten lässt und einen
Schreibzugriff eines Prozesses auf eine Seite ohne `PAGING_PAGE_USER`/
fremden Adressraum nachweislich abfängt (Page-Fault statt Kernel-Crash).
Das ist Phase-1-Abnahmekriterium und noch offen.

**Nächster Schritt:** gezielten Isolations-Selbsttest ergänzen (zwei
Prozesse, je eigene Seite an identischer virtueller Adresse, Nachweis der
Trennung) statt nur auf den bestehenden Zwei-Adressraum-Selbsttest in
`paging.c` zu vertrauen, der keine Prozess-Workload simuliert.

### 2.2 Sicherheits-/Capability-Architektur — **PARTIAL / DEFECTIVE (parallele Systeme)**

Es existieren mindestens **drei unabhängige, nicht synchronisierte**
Autorisierungs-Mechanismen, die diese Session direkt gelesen hat — plus
laut `security.h:2-6`s eigenem Kommentar zwei weitere (`program.c`s
Capability-Registry, `verify.c`s `cap_query`-Stub), die noch nicht
gegengeprüft wurden ("vier unterschiedliche Capability-Konzepte, die
schon im alten ASM nebeneinander existierten" — im alten Code also
offenbar sogar schon bekanntes technisches Altlast-Problem):

1. `security.c` (64 Zeilen) — flache Tabelle `{pid, caps}`
   (`SECURITY_CAPACITY` Slots), grobe Bitmaske pro Prozess, kein
   Objektbezug, keine Delegation.
2. `cap_core.c` (165 Zeilen) — echte Objekt-Capability-Engine: pro
   Capability `{subject, rights, delegable, domain, generation, state}`,
   mit `cap_core_derive()` (Rechte-Abschwächung bei Delegation),
   `cap_core_revoke()`, eigenem Selbsttest. Architektonisch der
   richtige Ansatz, aber:
3. `cap.c` (100 Zeilen) — Bootstrap-Policy-IDs, laut Code-Kommentar in
   `syscall.c:832-836` nur für `nef_load_and_spawn()`/Shutdown verdrahtet.

**Befund `syscall.c`** (1460 Zeilen, zentraler Syscall-Dispatch): nur
**2 Stellen im gesamten Dispatcher** rufen überhaupt eine Capability-
Prüfung auf (`cap_check()` für `SYSCALL_CORE_SHUTDOWN`,
`security_check()` indirekt über `nef_load_and_spawn()` für Spawn). Die
übrigen Syscall-Gruppen (VFS außerhalb von `vfsapi.c`'s eigener Prüfung,
IPC, THREAD, DEVICE, TIME, SYSTEM) werden **nicht** zentral auf
Capabilities geprüft. Das widerspricht Phase 3/7 des Auftrags
("Capability-basierte Berechtigungen" für jede Syscall-Gruppe).

**Korrektur (2026-11-24):** `cap.c` ist **kein** drittes, unabhängiges
System — eigener Kopfkommentar (`cap.c:1-9`) sagt explizit "DUENNER
WRAPPER UEBER cap_core.c (023-CAPABILITY-Konsolidierung)": eine frühere
Session hat `cap.c`/`cap_core.c` bereits zusammengeführt (`cap_create()`
ruft `cap_core_create()` + publiziert ein evbus-Event, usw.). Es bleiben
also real nur **zwei** Systeme: `security.c` (flach, PID-basiert, für
VFS/NEF-Capabilities load-bearing im Einsatz) und `cap.c`/`cap_core.c`
(Objekt-Capability-Engine, aber nur für den einzigen SHUTDOWN-Syscall
verdrahtet). `program.c`/`verify.c` weiterhin nicht gegengeprüft.

**Umgesetzt (2026-11-24):** Statt der ursprünglich geplanten größeren
Migration (die beiden Systeme zusammenführen) wurde die eigentliche
Lücke direkt geschlossen, die der Befund oben beschreibt — die meisten
Syscall-Operationen waren überhaupt nicht capability-geprüft. Geprüft,
welche der **tatsächlich implementierten** Syscall-Dienste
(`SYSCALL_SERVICE_GUI/VFS/INPUT/CORE` — IPC/THREAD/DEVICE/TIME/SYSTEM aus
dem Master-Auftrag existieren in diesem Kernel noch gar nicht als
Ring-3-erreichbare Syscalls, daher aktuell nichts zu gaten) tatsächlich
ungeprüften, global wirksamen Zustand mutieren: `SYSCALL_CORE_
TOGGLE_START_MENU/_EXPLORER/_SETTINGS/_AUTOHIDE` — vor dem Fix konnte
**jeder** Ring-3-Prozess das Startmenü/den Explorer/die Einstellungen/die
Taskbar-Auto-Hide-Einstellung für alle anderen Prozesse umschalten (GUI-
Operationen haben dagegen bereits eine Handle-Besitzprüfung,
`syscall_gui_handle_owned_by()`, VFS bereits `security_check()` in
`vfsapi.c` — beide waren also keine echte Lücke). Fix: zentrale
`security_check(caller_pid, SECURITY_CAP_DISPLAY_SYSTEM_UI)`-Prüfung in
`syscall.c`s CORE-Dispatch vor jeder der vier TOGGLE-Operationen
(`SYSCALL_STATUS_CAP` bei Ablehnung, Code existierte dafür bereits,
war aber bisher nur für SHUTDOWN genutzt). `SECURITY_CAP_DISPLAY_SYSTEM_UI`
(`security.h`, Bit 14) war bereits definiert, aber bis dahin komplett
ungenutzt — jetzt den fünf echten System-UI-Prozessen (Taskbar,
Startmenü, Explorer, Einstellungen, Desktop-Icons) bei ihrem Spawn in
`ushell_process.c` gewährt (alle fünf rufen laut Grep mindestens eine der
vier TOGGLE-Operationen auf). Boot-verifiziert: `System Call Interface
Selbsttest OK (7/7)`, alle fünf `USHELL spawn ...`-Capability-Grants
erfolgreich, `NOVA_KERNEL_READY` erreicht, 0 FEHLGESCHLAGEN.

**Weiterhin offen:** die zwei Systeme (`security.c`/`cap_core.c`) sind
noch nicht zusammengeführt — das bleibt ein größerer, riskanterer Schritt
für eine spätere Session. `GET_START_MENU_VISIBLE`/`_GET_EXPLORER_
VISIBLE`/`_GET_SETTINGS_VISIBLE`/`_GET_AUTOHIDE_ENABLED` (reine Lesezugriffe)
bewusst NICHT gegated — Sichtbarkeitsabfragen sind nicht sicherheitskritisch
und jeder Prozess darf sie lesen.

### 2.3 Prozess-Lebenszyklus (`process.c`, 259 Zeilen) — **TEILWEISE BEHOBEN (2026-11-24)**

**Update:** `process_reap()` ruft jetzt tatsächlich `object_release(rec->handle)`
und das neu ergänzte `security_forget(rec->pid)` (`security.c`/`security.h`)
auf, statt nur den Record zu nullen. `security_forget()` gibt — anders als
das vorhandene `security_revoke()`, das nur die Caps-Bitmaske löscht — den
`security_table`-Slot wirklich frei (`pid=0`, `--security_count`), damit
`SECURITY_CAPACITY` nicht durch längst beendete Prozesse dauerhaft
verbraucht bleibt. Boot-verifiziert (isolierte QEMU-Instanz, 2026-11-24):
`Process Manager Selftest ok`, `Security Capability Manager Selftest ok`,
`NOVA_KERNEL_READY` erreicht, 0 `FEHLGESCHLAGEN`-Zeilen, NovaShell-`ls`
weiterhin alle 14 Wurzel-Einträge korrekt. Nach Boot-Bestätigung nach
`build/nova-uefi.img` und `qemu-test/nova-uefi.img` promotet.

**Weiterhin offen (nicht in diesem Schritt behoben):** `vas_destroy()`
(`vas.h:95`) wurde bewusst NICHT aufgerufen — Gegenprüfung ergab, dass
Ring-3-Prozesse (`ring3.c`) ihren Adressraum nie über `vas_create()`
anlegen, sondern direkt über `paging_create_address_space()`
(`paging.c:71`). `vas.c` ist für den echten Prozess-Lebenszyklus komplett
unbenutzt (siehe `vas.h:7-19`, eigener Scope-Hinweis: reine NPSPEC-
Buchhaltungsebene ohne Hardware-Anbindung, siehe
[[feedback_vas_not_used_by_ring3]]). Der tatsächliche Leak liegt also
nicht bei `vas.c`, sondern beim **Page Directory und den darin gemappten
Seiten** (Code-, Stack-, GUI-Puffer-Seite aus `ring3.c:235-269`):
`paging.h` bietet aktuell **keine Gegenfunktion** zu
`paging_create_address_space()`.

**Versuch und Rückbau (2026-11-24, wichtige Lehre):** ein
`paging_destroy_address_space()` wurde implementiert (Page-Table-Walk je
Directory-Eintrag, unterscheidet per Byte-Vergleich gegen den Kernel-
Basis-Adressraum zwischen geerbten/geteilten Einträgen — NICHT freigeben —
und vom jeweiligen Prozess selbst hinzugefügten Einträgen — freigeben),
inklusive eigenem Selbsttest (Frame-Count-Bilanz vor/nach Destroy) in
`paging_self_test()`. Der Selbsttest selbst bestand (Frame-Bilanz exakt
ausgeglichen), aber der volle Boot-Test schlug **viele Selbsttests später**
mit einem Page Fault fehl (`CR2` weit außerhalb des per `-m 256M`
konfigurierten RAM — klassisches Symptom für eine Speicherkorruption, die
erst beim späteren Wiederverwenden eines fälschlich freigegebenen oder
doppelt freigegebenen Rahmens sichtbar wird). Per Bisektion bestätigt:
die Ursache liegt eindeutig in `paging_destroy_address_space()`/seinem
Aufruf aus `process_reap()`, nicht im unten beschriebenen
`frame_alloc_contiguous()`-Fix. Root Cause trotz mehrfacher Code-Durchsicht
nicht mit vertretbarem Aufwand sicher gefunden — ein Verdachtspunkt:
`pmm_free_page()` (`pmm.c:184-198`) hat **keinerlei Doppel-Freigabe-Schutz**
(pusht `page` blind auf den Freispeicher-Stack, ohne zu prüfen, ob die
Seite dort schon liegt) — sollte der Destroy-Pfad irgendeinen Rahmen
zweimal oder einen vom Kernel noch tatsächlich genutzten Rahmen
freigeben, würde das nicht sofort auffallen, sondern erst Boot-Schritte
später, wenn der doppelt vergebene Rahmen von zwei verschiedenen,
voneinander unwissenden Verbrauchern gleichzeitig benutzt wird. **Komplett
zurückgenommen** (Funktion aus `paging.c`/`paging.h` entfernt, Aufruf aus
`process_reap()` entfernt, Selbsttest-Erweiterung entfernt) — Boot nach
Rückbau erneut verifiziert: 0 FEHLGESCHLAGEN, `NOVA_KERNEL_READY` erreicht.
Der Leak (Page Directory + Code-/Stack-/GUI-Puffer-Seiten pro terminiertem
Ring-3-Prozess) bleibt damit ein **offener Punkt** und braucht beim
nächsten Versuch entweder (a) `pmm_free_page()` zuerst um eine
Doppel-Freigabe-Prüfung zu ergänzen (würde diese Fehlerklasse beim nächsten
Versuch sofort statt Minuten später sichtbar machen) oder (b) einen
gezielten Debug-Build mit CR2/Vektor-Ausgabe im Exception-Pfad (siehe
`interrupt.c:99-104`, aktuell ohne Diagnoseausgabe) **dauerhaft** ergänzt,
bevor der nächste Versuch gestartet wird.

**Separat und unabhängig gefunden, behoben und boot-verifiziert:**
`frame_alloc_contiguous()` (`framealloc.c`) rief `pmm_alloc_page()` in
einer Schleife auf und prüfte NUR, ob die zufällig zurückgegebenen Seiten
absteigend zusammenhingen — das funktioniert ausschließlich, solange seit
Boot noch nichts in anderer Reihenfolge freigegeben wurde. Der oben
beschriebene (zurückgenommene) `paging_destroy_address_space()`-Testlauf
hat genau das zum ersten Mal verletzt und `framealloc_self_test()` dadurch
zum Fehlschlagen gebracht — ein echter, eigenständiger Fund: JEDE
künftige Freigabe außerhalb strikter Spiegelbild-Reihenfolge würde dieselbe
Funktion brechen. Fix: delegiert jetzt an das bereits vorhandene, robuste
`pmm_alloc_contiguous()` (`pmm.c:268-310`, durchsucht den tatsächlichen
Freispeicher nach einem zusammenhängenden Lauf, mit dedizierter
Fallback-Arena) statt die fragile eigene Suche zu duplizieren. Boot-
verifiziert: `Frame Allocator Selftest ok`.

**PMM-Hardening (2026-11-24, direkte Folge des Rückbaus oben, FERTIG):**
`pmm_free_page()` (`pmm.c`) hatte **keinerlei Doppel-Freigabe-Schutz** und
**keine Obergrenzenprüfung** (nur `page % 4096 == 0` und
`page >= pmm_reserved_end`) — ein Aufrufer-Bug hätte jede beliebige Adresse
(auch eine MMIO-/Framebuffer-Adresse weit oberhalb des PMM-verwalteten
Bereichs) blind auf den Freispeicher-Stack gepusht, von wo sie später
`pmm_alloc_page()` einem völlig unbeteiligten Aufrufer als vermeintlich
normalen Datenrahmen weitergereicht hätte — ein plausibler Mechanismus
für genau die Art von später, scheinbar zusammenhangsloser Korruption, die
oben beobachtet wurde. Fix: (1) `page >= PMM_BOOTSTRAP_IDENTITY_LIMIT`
wird jetzt abgelehnt, (2) eine neue Bitmap (`pmm_free_bitmap`, 1 Bit pro
Rahmen = 2 KiB) erkennt Doppel-Freigaben in O(1). **Wichtige Lehre:** ein
erster Versuch mit einem linearen Scan über `pmm_frames` (statt der
Bitmap) bestand zwar seinen eigenen PMM-Selbsttest, brachte aber durch die
dadurch verlängerte Boot-Laufzeit drei zeitkritische, tick-basierte
Selbsttests (Task Deadline Manager, Thread Manager, Progressive Readiness
Thread Start) zum Fehlschlagen — ein Performance-Regressions-Bug, der
durch reinen Logik-Review am Scan selbst nicht auffiel, erst durch den
vollen Boot-Test. Mit der O(1)-Bitmap-Lösung boot-verifiziert: 0
FEHLGESCHLAGEN, `NOVA_KERNEL_READY` erreicht, alle zuvor betroffenen
Selbsttests wieder `ok`. Promotet nach `build/nova-uefi.img` und
`qemu-test/nova-uefi.img`.

Ursprünglicher Befund (vor der Korrektur):

`process_terminate()` (`process.c:163-174`) setzt nur `state` und
`exit_status`. `process_reap()` (`process.c:184-192`) nullt danach den
gesamten Record. **Keine** der beiden Funktionen gibt tatsächlich etwas
frei:

- Der beim `process_create()` erzeugte Objekt-Handle (`object_create()`,
  `process.c:100`) wird nie mit `object_release()` wieder freigegeben
  (vgl. `object.h:21`).
- Keine Freigabe der `security_table`-Capability-Einträge
  (`security_revoke()` existiert, `security.c:33-41`, wird aber nirgends
  bei Prozessende aufgerufen) — `security_table` wächst monoton, auch
  wenn Prozesse längst beendet und eingesammelt wurden.
- Kein Aufruf von `vas_destroy()` (`vas.h:95`) für den Adressraum des
  Prozesses.
- Kein Aufruf von `ipc_endpoint_close()` (`ipc.h:95`) für offene
  IPC-Endpunkte des Prozesses.

Das sind genau die in Phase 2 des Auftrags geforderten Cleanup-Schritte
("Bei Prozessende müssen sämtliche eigenen Ressourcen freigegeben
werden"). Die Bausteine dafür **existieren bereits** im Kernel — sie
werden nur nicht aus `process_terminate()`/`process_reap()` heraus
aufgerufen. Das ist der am einfachsten zu schließende Gap dieser Analyse
(kein neues Subsystem nötig, nur Verdrahtung vorhandener Funktionen) und
wird als erste konkrete Maßnahme in dieser Session angegangen.

### 2.4 `object.c`/`handle.c`-Paar — **faktisch toter Code in Produktion**

`handle.c:27` hat ein einziges globales Feld `handle_owner_pid`, das
außerhalb des eigenen Selbsttests (`handle.c:155`,`193`) **nirgends** im
gesamten Kernel gesetzt wird (bestätigt per Grep über `kernel/newkernel`).
Da `handle_create()`/`handle_resolve()`/`handle_close()` alle als erste
Prüfung `pid != handle_owner_pid` verlangen und `handle_owner_pid` in
Produktion dauerhaft `0` bleibt (und `pid == 0` explizit abgelehnt wird),
ist dieses Modul in der laufenden Kernel-Instanz **unbenutzbar** — bereits
in `vfsapi.h:15-24` vom Projekt selbst so dokumentiert. Stattdessen baut
jedes Subsystem, das syscall-exponierte Handles braucht, seine **eigene,
flache Tabelle** (`vfsapi.c`, `surface.c`/`window.c` für GUI, vermutlich
weitere). Das ist funktional nicht falsch, aber Code-Duplikation und
Inkonsistenz-Risiko: jede dieser Tabellen reimplementiert Generation-
Counter, Rechte-Masken und Owner-Prüfung unabhängig und potenziell mit
abweichenden Fehlerfällen.

**Empfehlung:** entweder `handle.c` einen echten Pro-PID-Mechanismus geben
(z. B. `handle_owner_pid` durch ein Array/eine Lookup-Funktion ersetzen,
die den aktuell aktiven Prozesskontext abfragt) und alle Ad-hoc-Tabellen
darauf migrieren, oder `handle.c`/`object.c` als das was sie sind
kennzeichnen (interne Kernel-Objektverwaltung, kein Userspace-Handle-
Mechanismus) und aus den Phase-2/3-Abnahmekriterien für "Handles" streichen.

### 2.5 Exception-/Panic-Handling (`exception.c`, `panic.c`, `interrupt.c`) — **TEILWEISE BEHOBEN (2026-11-24)**

**Befund:** `exception.c` (176 Zeilen) ist eine vollständige, eigenständig
getestete Klassifizierungs-/Record-Bau-Schicht für CPU-Exceptions
(`exc_vector_to_code()`, `exc_code_to_category()`, `exc_build_record()`,
Fixup-Tabellen-Suche) — `Exception Manager Selftest OK (14/14)`.
`panic.c` (179 Zeilen) ist ein vollständiger, funktionierender Panic-
Handler mit strukturiertem Crash-Report (Fehlercode/Subsystem/EIP/CR2/
Boot-Phase/CPU-ID/Security-State) und checksummed Crash-Dump
(`kernel_panic()`, `crash_dump_capture_minimal()`/`_validate()`).

**Trotzdem war keines von beiden an den echten CPU-Exception-Pfad
angeschlossen.** `interrupt.c`s `interrupt_dispatch_c()` behandelte JEDE
CPU-Exception (Vektor 0-31 — Division durch Null, GPF, Page Fault, ...),
egal ob von Kernel- oder Ring-3-Code ausgelöst, mit einer einzigen
generischen Serial-Zeile und einem bedingungslosen `hlt`-Loop — ohne EIP,
ohne CR2, ohne Fehlerklasse, ohne jede Unterscheidung. `exception.h:4-12`
dokumentiert selbst bereits bewusst, warum: der volle
`exception_manager_dispatch()` (inkl. Beendigung NUR des fehlerhaften
Ring-3-Prozesses + Scheduler-Interaktion) würde echten, hier noch nicht
vorhandenen Taskwechsel-aus-dem-Fault-Handler voraussetzen — **das bleibt
bewusst unverändert** (siehe nächster Absatz), da ein Versuch ohne
Verständnis von `scheduler.c`s Kontextwechsel-Mechanismus das gleiche
Risiko hätte wie der zurückgenommene `paging_destroy_address_space()`-
Versuch (Abschnitt 2.3).

**Umgesetzt (sicher, da reine Diagnose-Verbesserung ohne Verhaltensänderung
im Erfolgsfall):** `interrupt.c`s Exception-Zweig ruft jetzt tatsächlich
`exc_build_record()` (liefert u. a. Fehlerklasse UND Ring0/Ring3-Ursprung
aus dem Trap-Frame) und danach `kernel_panic()` auf, statt der bisherigen
nackten Textzeile. Das System hält weiterhin an (keine Verhaltensänderung
für den Erfolgspfad — kein einziger der ~250 Boot-Selbsttests löst
überhaupt eine CPU-Exception aus, boot-verifiziert: 0 FEHLGESCHLAGEN,
`NOVA_KERNEL_READY` erreicht), aber ein ECHTER Absturz liefert jetzt einen
vollständigen, strukturierten Diagnose-Report (EIP/CR2/Fehlerklasse/
Boot-Phase/CPU/Security-State + Crash-Dump) statt nichts.

**Weiterhin offen (größerer, risikoreicherer Folgeschritt):** eine
Ring-3-Exception sollte idealerweise NUR den fehlerhaften Prozess beenden
(Phase 2/7 des Master-Auftrags: "Kontrollierter Prozessabbruch bei
Schutzverletzung"), statt die gesamte Maschine anzuhalten. Das erfordert
zuerst ein Verständnis von `scheduler.c`s Taskwechsel-Mechanismus
(aus einem Interrupt-Handler heraus NICHT zur unterbrochenen Adresse
zurückkehren, sondern zu einem anderen Thread) — bewusst nicht in diesem
Schritt versucht.

### 2.6 Scheduler/SMP (`scheduler.c`, 812 Zeilen, `smp.c`, 234 Zeilen) — **PARTIAL (reines Audit, keine Änderung)**

**Scheduler:** Der tatsächlich aktive Dispatch-Pfad
(`scheduler_on_tick()`) ist ein echter, funktionierender, präemptiver
Round-Robin-Taskwechsel mit Pro-Slot-CR3/ESP0 (boot-verifizierter
`scheduler_self_test()` beweist echten Adressraumwechsel). Das ist die
Basis, auf der die gesamte bisherige Ring-3-Desktop-Funktionalität
(Taskbar/Explorer/Startmenü/Einstellungen/Icons/NovaShell) tatsächlich
läuft — funktioniert nachweislich.

**Aber:** `scheduler.c` enthält daneben VIER vollständig implementierte
und jeweils EIGENSTÄNDIG self-getestete Scheduling-Policy-Engines:
Work-Stealing-Queues (`worksteal_*`), ein EDF/FIFO-Echtzeit-Scheduler
(`rt_*`), ein CFS-Analogon mit `vruntime` (`cfs_*`), ein QoS-Budget-
Scheduler (`qos_*`). **Keine davon hat irgendeinen Einfluss auf
`scheduler_on_tick()`** — der echte Dispatcher wählt den nächsten Slot
rein per fortlaufendem Index (`slot + 1`), unabhängig von Priorität,
Deadline, Fairness oder QoS-Budget. `scheduler_admit_task()` (einziger
externer Aufrufer: `excon.c:55`) registriert einen Task zwar bei
`rt_task_register()`/`cfs_task_enqueue()`, aber nichts liest diese
Tabellen je wieder aus, um tatsächlich zu entscheiden, WER als Nächstes
laufen darf. Das bedeutet: NovaOS hat aktuell **keine wirksame
Prioritäts-/Deadline-/Fairness-Planung** trotz vollständig vorhandener
Datenstrukturen dafür — exakt das von Phase 2 des Master-Auftrags
verlangte "Praeemptives Scheduling" mit "Prioritaetsbehandlung" /
"Vermeidung von Starvation" ist nur die Hälfte: Präemption ja, Priorität/
Fairness nein.

**SMP:** `smp_initialize()` bootet Application Processors tatsächlich
echt (INIT-SIPI-SIPI-Sequenz, LAPIC Virtual-Wire, Timeout-Soft-Fail je
AP) — boot-verifizierbar per Heartbeat-Zähler (`smp_self_test()` beweist
echten Fortschritt, nicht nur "online gemeldet"). **Aber** `ap_entry_c()`
sagt es im eigenen Kommentar bereits explizit: jeder AP läuft danach
dauerhaft in einer reinen `pause`-Schleife, **mit deaktivierten
Interrupts (IF=0) und OHNE jemals dem Scheduler beizutreten**. Das
deckt sich mit der bereits früher dokumentierten Beobachtung
([[project_taskbar_redraw_freeze_bug]]: "APs joinen den Scheduler NIE").
Trotz `-smp 4` läuft also **jeglicher** echte Kernel-/Ring-3-Code
ausschließlich auf der BSP (CPU 0) — kein echtes Multi-Core-Scheduling,
keine Lastverteilung, keine Parallelität außer dem reinen AP-Boot-Nachweis
selbst. Widerspricht Phase 2 ("SMP-Lastverteilung", "Per-CPU-Runqueues").

**Nächster Schritt (nicht in dieser Session, hohe Komplexität/Risiko):**
APs müssten eigene IDT/TSS-Zustände, einen eigenen Timer-Interrupt-Pfad
und Zugriff auf `scheduler_on_tick()` (oder eine SMP-sichere Variante
davon mit Per-CPU-Runqueues statt des aktuellen rein globalen Zustands —
`scheduler_contexts[]`/`thread_running_on[]` sind nicht als thread-sicher
für mehrere gleichzeitig tickende CPUs dokumentiert) erhalten. Das ist ein
eigenständiges, großes Vorhaben, kein Nebenbei-Fix.

### 2.7 IPC-Endpoint-Modell (`ipc.c`/`ipc.h`) — **BEHOBEN (2026-11-24, boot-verifiziert)**

**Befund:** Das in einer früheren Session eingeführte NPSPEC-IPC-0001-
Phase-1-Endpoint-Modell (echte Owner-Identität, explizite Sender-
Allowlist statt bloßer ID-Kenntnis, Backpressure-Reject statt stillem
Verwerfen, Introspection ohne Payload-Leck) ist vollständig implementiert
und per eigenem Selbsttest (`ipc_endpoint_manager_self_test()`)
nachweislich korrekt. `channel.c`/`rpc.c`/`pubsub.c`/`eventbus.c` (die
laut `ipc.h`-Kommentar als eigene Folgephasen AUF diesem Grundmodell
aufbauen) existieren bereits real und laufen boot-verifiziert
(`IPC Channel/PubSub/Event Bus Manager Selftest ok`). Shared-Memory-
Buffer-Referenzen und echtes Zero-Copy (ebenfalls in den NPSPEC-Folge-
spezifikationen erwähnt) existieren dagegen NICHT (`MISSING`, keine
Dateien gefunden).

**Gefundene Lücke:** `ipc.h` dokumentierte selbst explizit einen echten
Leak: "Automatisches Schließen beim Prozess-Ende (`process_terminate()`)
ist [...] NICHT verdrahtet [...] ein künftiger Reaper-Mechanismus kann
`ipc_endpoint_close()` für jeden vom terminierten Prozess gehaltenen
Endpoint aufrufen." Ein Prozess, der einen Endpoint öffnete und dann
terminiert/reaped wurde, hielt dessen Tabellenslot für immer belegt
(8 Slots gesamt, `IPC_ENDPOINT_CAPACITY`) — ein echter, wenn auch
kleiner, Ressourcen-Leak, exakt im selben Muster wie der bereits in
Abschnitt 2.3 behobene `object_release()`/`security_forget()`-Leak bei
`process_reap()`.

**Fix (sicher, additiv, gleiches Risikoprofil wie Abschnitt 2.3 Punkt 5):**
neue Funktion `ipc_endpoint_release_owner(uint32_t pid)` (`ipc.c`/`.h`)
schließt jeden noch offenen Endpoint, dessen `owner_pid == pid` ist
(`pid==0`-Guard analog zu `security_forget()`). Aufgerufen aus
`process_reap()` (`process.c`) direkt neben den beiden bestehenden
Aufrufen. Neuer Testfall in `process_manager_self_test()`: legt vor dem
Terminieren einen Endpoint im Namen des Testprozesses an, beweist nach
dem Reap, dass `ipc_endpoint_info()` ihn jetzt als `CLOSED` meldet
(vorher wäre er `OPEN` geblieben). Boot-verifiziert in isolierter
QEMU-Instanz: `Process Manager Selftest ok`, `IPC Endpoint Manager
Selftest ok`, 0 FEHLGESCHLAGEN, `NOVA_KERNEL_READY` erreicht, danach
`build/nova-uefi.img` + `qemu-test/nova-uefi.img` promotet.

**Weiterhin offen:** Capability-Enforcement der Endpoint-Operationen
selbst nutzt bewusst NICHT das globale `security.c`/`cap_core.c`-System,
sondern ein eigenes Owner+Allowlist-Modell (das ist laut NPSPEC-IPC-0001
so gewollt, keine Lücke). Shared-Memory/Zero-Copy bleiben `MISSING`.

### 2.8 Treiber-Stack: PCI/AHCI/NVMe/USB (`pci.c`, `ahci.c`, `nvme.c`, `usb.c`) — **GEMISCHT**

**`pci.c` (25 Zeilen) — COMPLETE_VERIFIED (kein Minimal-Stub, sondern
bewusst kleine Primitive):** liefert nur `pci_config_read32`/`write32`
(CONFIG_ADDRESS/CONFIG_DATA, Mechanism #1). Die eigentliche Bus-
Enumeration (Multifunction-/Bounds-Logik, Klassenfilterung NVMe/xHCI/
AHCI, BAR-Auslesen inkl. AHCI-Sonderfall BAR5/Offset 0x24 statt BAR0)
lebt in `driver.c:driver_pci_scan()`/`disc_find_matched()` - eine saubere
Schichtung, keine Lücke.

**`ahci.c` (418 Zeilen) — COMPLETE_VERIFIED:** echter Controller-Reset
(BIOS/OS-Handoff via BOHC, AE/HR-Sequenz), Port-Probing mit Link-Timeout,
COMMAND-LIST/FIS-Receive-Allokation, IDENTIFY DEVICE mit LBA48-/
Sektorgrößen-Prüfung, echte Register-H2D-FIS-Kommandos (READ/WRITE_DMA_
EXT, FLUSH_CACHE_EXT) inkl. Fehlerpfad (Port-Stop+SERR/IS-Clear+Neustart
bei Task-Error). `ahci_self_test()` beweist echten I/O UND die Ablehnung
eines Zugriffs jenseits der Kapazität - kein Fake-Erfolg.

**`nvme.c` (417 Zeilen) — COMPLETE_VERIFIED, mit einer dokumentierten
Grenze:** echtes CC/CSTS-Handshake, Admin-Queue-Setup, Doorbell-Stride
aus CAP korrekt extrahiert, Identify Controller/Namespace, I/O-Queue-
Erstellung, Read/Write/Flush über echte Submission/Completion-Queues.
**Gefundene, aber aktuell folgenlose Einschränkung:** `nvme_blk_read()`/
`_write()` nutzen NUR PRP1 (eine einzige physische Seite) - kein PRP2/
keine PRP-Liste. Ein Transfer, der über eine 4-KiB-Seite hinausreicht,
würde beschädigte Daten liefern. Aktuell **nicht erreichbar**: jeder
tatsächliche Aufrufer (`novafs.c` via `NOVAFS_SECTORS_PER_BLOCK=8` × 512
Byte = exakt 4096 Byte, `gpt.c`/`ahci.c`-Selbsttest mit 1 Sektor) bleibt
innerhalb einer Seite. Für künftige Arbeit festgehalten: sollte je eine
Blockgröße >4 KiB oder ein Multi-Block-Transfer in einem Aufruf
eingeführt werden, MUSS vorher PRP2/PRP-Liste ergänzt werden.

**`usb.c` (535 Zeilen) — PARTIAL/DEFECTIVE, aber bereits ehrlich selbst
dokumentiert (`usb.h:5-13`):** xHCI-Controller-Bring-up (Reset, DCBAA,
Command-Ring+Link-TRB, Event-Ring+ERST, Runtime-Interrupter, Start) UND
Root-Hub-Port-Scan (PP/CSC/Reset/PED/Speed via echte MMIO-Zugriffe) sind
real. **Aber:** `usb_control_read_descriptor()` - echte EP0-Control-
Transfers über xHCI-Transfer-Ringe - ist ein PERMANENTER Stub (gibt
immer -1 zurück), identisch zum alten ASM-Vorgänger. Dadurch schlägt
JEDE Geräte-Enumeration bereits am ersten Schritt
(`usb_core_enumerate_device()` Zeile 309) fehl - `usb_set_address()`/
`usb_set_configuration()` (No-Op-Stubs) und die komplette HID-/Mass-
Storage-Klassentreiber-Logik (`usb_bind_class_driver()`,
`usb_msc_init()`) sind strukturell fertig, aber **praktisch
unerreichbarer Code**.

**Schwererer Einzelbefund (über die bereits dokumentierte Lücke hinaus):**
`usb_blk_read()`/`usb_blk_write()` (Zeile 214-250) bauen zwar ein
korrektes BOT-CBW-Paket, senden es aber NIE über einen echten Bulk-
Transfer - Kommentar "Polling-Stub, Erfolg simulieren" - und geben
**immer `return 0` (Erfolg) zurück, ohne den übergebenen Buffer jemals
zu befüllen oder zu lesen**. Das ist exakt der vom Master-Auftrag
verbotene Fake-Erfolgscode. Aktuell folgenlos, weil `usb_msc_init()` -
und damit jede Block-Device-Registrierung über USB - durch die
Enumerationslücke oben nie erreicht wird; würde aber bei Behebung der
Enumeration SOFORT zu stillschweigend korrumpierten Lese-/
Schreibzugriffen führen, wenn man sie nicht gleichzeitig mitbehebt.

**UPDATE (2026-11-24, auf Nutzerwunsch umgesetzt): echte EP0-Control-
Transfers implementiert, boot-verifiziert.** `usb.c` hat jetzt eine
vollstaendige xHCI-Command-Ring-/Event-Ring-Engine (`xhci_cmd_ring_
submit()`, `xhci_poll_event()`, `xhci_wait_event()`), echte `Enable Slot`-
und `Address Device`-Kommandos (`xhci_enable_slot()`, `xhci_address_
device()`), einen echten EP0-Transfer-Ring pro Geraet (`xhci_ep0_ring_
submit()`) und einen vollstaendigen Standard-Control-Transfer (`xhci_
control_transfer()`: Setup-/Data-/Status-Stage-TRBs nach xHCI-Spec
4.11.2.2). `usb_control_read_descriptor()`/`usb_set_configuration()`
nutzen das jetzt echt; `usb_core_enumerate_device()` folgt dem korrekten
xHCI-Ablauf (Enable Slot -> Device-/Input-Context+EP0-Ring -> Address
Device -> Standard-Deskriptoren -> Konfiguration), nicht mehr der alten,
fuer xHCI eigentlich falschen "Adresse selbst waehlen"-Reihenfolge des
ASM-Originals (bei xHCI vergibt der Controller selbst die Adresse ueber
das Address-Device-Kommando, Software waehlt sie NICHT).

**Beim Boot-Test gefundener UND behobener echter Bug:** `xhci_wait_event()`
sprang bei einem nicht-passenden Event per `continue` direkt zur
naechsten Iteration und UEBERSPRANG dabei die Timeout-Pruefung - bei
einem Strom fremder Events (z.B. Port-Status-Change) konnte das
Warten UNBEGRENZT andauern. Im isolierten Boot-Test mit echtem
emuliertem xHCI-Controller + USB-Tastatur (`-device qemu-xhci -device
usb-kbd`) reproduziert (echter Haenger, kein Boot-Fortschritt mehr nach
"Thread 2 lief"), durch Hinzufuegen der Timeout-Pruefung AUSSERHALB des
`continue`-Zweigs behoben.

**Zweiter, bereits VOR dieser Session bestehender Bug gefunden (beim
Debuggen des obigen Haengers aufgedeckt, nicht durch die EP0-Aenderung
verursacht):** `driver.c`s PCI-BAR-Extraktion (`driver_pci_scan()`,
verwendet von AHCI/NVMe/USB fuer BAR0 bzw. BAR5) ignorierte 64-Bit-BARs
komplett - las nur das untere Dword und maskierte es, ohne das High-
Dword auch nur zu pruefen. Unter diesem q35/OVMF-Testaufbau platziert
die Firmware den 64-Bit-faehigen xHCI-BAR standardmaessig OBERHALB
4 GiB (beobachtet: High-Dword=0xC0, d.h. physische Adresse
0xC0_00004000) - die alte Maskierung lieferte dadurch eine falsche,
abgeschnittene 32-Bit-Adresse (0x00004000, zufaellig ein gueltig
aussehender aber komplett falscher Low-Memory-Wert). MMIO-Zugriffe auf
diese falsche Adresse lasen/schrieben unvorhersehbaren Speicher statt
echter Controller-Register - eine plausible Ursache fuer unvorhersehbares
Verhalten bei JEDER zukuenftigen Arbeit mit einem 64-Bit-BAR-Geraet,
nicht nur USB. Fix: Typ-Bits[2:1] der BAR pruefen, bei 64-Bit-BAR das
High-Dword lesen; ist es ungleich 0 (Adresse >4 GiB, fuer diesen 32-Bit-
Kernel strukturell nicht abbildbar), `bar0` bewusst auf 0 setzen, damit
die bereits vorhandene "bar0==0 -> kein Controller"-Pruefung in
`xhci_init_controller()`/`ahci_initialize()`/`nvme_probe_controller()`
den Fall korrekt als "nicht erreichbar" behandelt statt mit einer
falschen Adresse weiterzuarbeiten.

**Verifikation:** isolierter QEMU-Boot (mit UND ohne `-device qemu-xhci
-device usb-kbd`) zeigt 0 FEHLGESCHLAGEN, `NOVA_KERNEL_READY` erreicht,
boot-verifiziert und promotet (`build/nova-uefi.img`, `qemu-test/nova-
uefi.img`). **Wichtige Einschraenkung, ehrlich dokumentiert:** eine
Ende-zu-Ende-Verifikation mit einem WIRKLICH erkannten, enumerierten
USB-Geraet (z.B. "USB HID Tastatur erkannt" im Log) konnte in DIESEM
QEMU/OVMF-Testaufbau NICHT erreicht werden, weil die Firmware den xHCI-
Controller-BAR oberhalb 4 GiB platziert (siehe oben) - fuer den 32-Bit-
Kernel architektonisch unerreichbar, unabhaengig von der EP0-Logik
selbst. Der `-global q35-pcihost.pci-hole64-size=0`-Versuch, QEMU zu
einer Platzierung unterhalb 4 GiB zu zwingen, aenderte daran NICHTS
(OVMFs eigene Ressourcenzuteilung entscheidet das, nicht die QEMU-
Hostbridge-Fenstergroesse). Die EP0-Control-Transfer-Logik selbst wurde
daher nur STRUKTURELL (TRB-Layout exakt nach xHCI-Spec 4.11.2.2/6.4.3/
6.4.6 geprueft, saubere Kompilierung ohne Warnungen) und durch den oben
gefundenen echten Timeout-Bug TEILWEISE dynamisch verifiziert, nicht
durch eine vollstaendige erfolgreiche Geraete-Enumeration. Einstufung:
**IMPLEMENTED_UNVERIFIED** (nicht COMPLETE_VERIFIED) bis ein Testaufbau
mit einem unterhalb 4 GiB erreichbaren xHCI-Controller (z.B. anderes
Chipset/BAR-Allokationsverhalten, oder echte Hardware) eine volle
Enumeration bestaetigt.

**Weiterhin fehlend (unveraendert, siehe usb.h):** echte Bulk-Transfers
(Mass-Storage BOT) - `usb_blk_read()`/`_write()` gaben vorher FAKE-
ERFOLG zurueck (verbotener Dummy-Code), geben jetzt ehrlich -1 zurueck.
Interrupt-Transfers fuer HID-Eingaben ebenfalls nicht implementiert.

## 3. Boot-Status (Baseline, vor jeder Änderung dieser Session)

Letzter bekannter Boot-Log (siehe vorherige Session, `project_yield_syscall.md`
u.a.): voller Boot bis `NOVA_KERNEL_READY`, 0 `"FEHLGESCHLAGEN"`-Zeilen.
Diese Baseline muss nach jeder Änderung dieser Session per isolierter
QEMU-Instanz erneut bestätigt werden (Methodik siehe `CLAUDE.md`-Memory
dieses Projekts: kompilieren → linken → objcopy → `build-elf32.ps1` →
`build-nki.ps1` → `build-uefi-image.ps1 -NovaFsImage` → QEMU seriell).

## 4. Priorisierte Arbeitsreihenfolge (Vorschlag)

1. **Prozess-Cleanup verdrahten** (Abschnitt 2.3) — klein, nutzt
   vorhandene Primitive, sofort boot-testbar. *Wird jetzt umgesetzt.*
2. **Capability-Enforcement vereinheitlichen** (Abschnitt 2.2) — größerer
   Schnitt, sollte erst nach Klärung von Abschnitt 2.1 (echte
   Adressraumtrennung) erfolgen, da capability-Durchsetzung ohne
   Speicherisolation nur begrenzten Wert hat.
3. **Paging/VAS-Audit** (Abschnitt 2.1) — klären ob Pro-Prozess-Page-
   Directories real sind oder nur das `cr3`-Feld existiert ohne Wirkung.
4. **Restliche ~290 Module** (Treiber: AHCI/NVMe/USB/PCI; Grafik-Stack:
   display/compositor/render*/mat*; die "a*"-Familie aus adaptiven/
   autonomen Subsystemen `acache/aconstraint/adaptfb/.../apreload`; die
   "r*"/"tx*"-Familie aus Resource-Economy/Transaktionssystemen;
   Scheduler/SMP/IPC im Detail) — schrittweise in weiteren Sessions,
   priorisiert nach dem, was der Master-Auftrag als boot-kritisch nennt
   (Speicher → Prozess/Thread → Syscall/ABI → IPC/Sync → Treiber → VFS →
   Sicherheit → Rest).

## 5. Noch nicht auditiert (explizit, keine Bewertung impliziert)

Alle übrigen Dateien unter `kernel/newkernel/` außer den in Abschnitt 1/2
genannten — insbesondere: `scheduler.c`, `smp.c`, `ipc.c`, `syscall.c`
(nur die Capability-Frage wurde geprüft, nicht Argumentvalidierung/
Fehlercode-Konsistenz im Detail), `ahci.c`, `nvme.c`, `usb.c`, `pci.c`,
`gpt.c`, `block.c`, gesamter Grafik-/Compositor-/Material-Stack,
`interrupt.c`, `exception.c`, `panic.c`, `irqmgr.c`, `thermal*.c`,
`power*.c`, `acpi`-Bezug (kein `acpi.c` gefunden — zu prüfen, ob ACPI
überhaupt existiert oder nur über `apower.c`/`power.c`/`powermgr.c`
abgedeckt wird), die komplette "adaptive/autonome" Modulfamilie
(`a*.c`: ca. 25 Dateien) und die Resource-/Transaktions-Familie
(`r*.c`/`tx*.c`: ca. 40 Dateien).
