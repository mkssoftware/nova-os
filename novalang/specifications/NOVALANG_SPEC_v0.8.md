# NovaLang 0.8 – Erweiterung: Prozess-Spawn-Syscall

Baut auf v0.1-v0.7 auf. Kernel-Erweiterung (nicht nur Compiler-Arbeit wie
v0.4-v0.7): schliesst die in `project_novashell.md` Phase-A-Analyse
gefundene Luecke "kein Ring-3-aufrufbarer Prozess-Spawn-Syscall" - die
Voraussetzung fuer NovaShells `run`-Befehl.

## 1. Kernel-Erweiterung: SYSCALL_CORE_SPAWN

`nef_load_and_spawn()` (siehe `nef_loader.h`) existierte bisher NUR als
kernelinterne C-Funktion, von `kmain.c` fuer alle bisherigen Testprogramme
mit `loader_pid=1u` (Kernel) aufgerufen. Neuer Syscall (`syscall.h`/
`syscall.c`, Operation 18 unter `SYSCALL_SERVICE_CORE`):

```c
typedef struct {
    uint32_t path_addr;
    uint32_t path_len;
    uint32_t result_pid; /* out */
} syscall_spawn_args_t;
```

`EDX` = Pointer auf diese Struct (im validierten Fenster, wie bei VFS-
Syscalls). **`loader_pid` wird IMMER auf den AUFRUFENDEN Prozess selbst
gesetzt (`caller_pid`), NIE vom Aufrufer uebergeben** - dasselbe
Confinement-Prinzip wie bei NEF-Bibliotheks-Importen: ein Prozess kann
dem neu gestarteten Programm nie mehr Capabilities verschaffen als er
selbst hat (`nef_load_and_spawn()` prueft das bereits intern per
`security_check()`), und braucht selbst `SECURITY_CAP_FS_READ`, um die
Zieldatei ueberhaupt zu oeffnen - deshalb KEINE zusaetzliche, eigene
Capability-Pruefung fuer SPAWN selbst noetig. Bei Erfolg: `EAX`=0,
`result_pid` = neue PID. Bei Fehler: `EAX` = der ROHE `NEF_LOAD_ERR_*`-
Code (wie bei den VFS-Syscalls, keine Beschoenigung auf einen generischen
Status).

**Bekannte, bewusst akzeptierte Einschraenkung:** die `nef_process_t`
(Scheduler-Slot/Seiten/Adressraum-Tracking) des neu gespawnten Prozesses
wird NICHT dauerhaft gespeichert - sie existiert nur als Stack-lokale
Variable waehrend des Syscall-Aufrufs. Der Prozess LAEUFT danach normal
weiter (echter Scheduler-Slot, echter Adressraum), kann aber spaeter NIE
per `nef_unload()` wieder abgebaut werden. Das ist konsistent mit der
BEREITS bestehenden Grenze dieses Kernels (jeder bisherige Testprozess
laeuft ebenfalls fuer immer) - ein echter Prozess-Lebenszyklus (Tracking-
Tabelle PID->`nef_process_t`, Terminierung, Reaping) ist eine groessere,
separate Aufgabe (siehe `project_novashell.md`s "sicherer Terminierungs-
Syscall").

## 2. Keine neue Grammatik

Keine neuen Schluesselwoerter/Builtins - das Testprogramm nutzt
ausschliesslich bereits vorhandene v0.4-v0.7-Primitiven (`SysCall`,
`Buffer`, `AddressOf`, `PokeInt`, `PokeString`, `PeekInt`), um die
`syscall_spawn_args_t`-Struct manuell zu befuellen - derselbe Beweis wie
bei v0.5s VFS-Test, dass der generische Syscall-Mechanismus bereits
ausreicht, OHNE dass der Compiler fuer jede neue Kernel-Faehigkeit einen
eigenen Sonderfall braucht.

## 3. Wichtiger Fund: NovaFS-Kapazitaetsgrenze (kein Compiler-/Syscall-Bug)

Ein urspruenglicher Plan, das Spawn-Testprogramm als EIGENE, neue
NovaFS-Datei (`hello_spawn.nef`) anzulegen, scheiterte boot-verifiziert
an `novafs_create()` (`-1`, "Blatt voll" laut `novafs_tree_insert_simple()`-
Kommentar in `novafs.c`) - NovaFS' B-Baum implementiert noch KEIN
Blatt-Splitting, und dieses Boot hatte bereits 8 vorherige NEUE NovaLang-
Testdateien unter derselben Wurzel angelegt (plus die 4-5 ursprünglichen
NEF-Testdateien). Das ist eine ECHTE, vorbestehende Dateisystem-Grenze,
KEIN Bug in NovaLang oder dem neuen Syscall - siehe
`feedback_novafs_leaf_capacity.md` fuer die volle Diagnose.

**Fix (fuer DIESE Version):** der Spawn-Test wurde stattdessen an
`hello_bytes.nova` (ein bereits bestehendes, bereits angelegtes
Testprogramm) angehaengt, statt eine neue Datei zu erzeugen - keine neue
NovaFS-Datei, kein Konflikt mit der Blatt-Kapazitaet. **Wichtige Folge
fuer kuenftige Versionen:** weitere NEUE, EIGENSTAENDIGE NovaLang-
Testprogramme werden FRUEHER ODER SPAETER erneut an dieser Grenze
scheitern, solange NovaFS kein Blatt-Splitting hat. Kuenftige Test-
programme sollten entweder (a) an ein bestehendes Programm angehaengt
werden (wie hier), oder (b) NovaFS' Splitting muesste echt implementiert
werden (separate, grössere Aufgabe, nicht Teil des NovaLang-Auftrags).

## 4. Explizit weiterhin NICHT Teil dieser Version

Siehe v0.1-v0.7 - zusaetzlich: `nef_unload()`/echte Prozessterminierung
fuer gespawnte Prozesse, Warten auf Prozessende/Exit-Code-Abruf (fuer
NovaShells "kontrolliertes Warten"), Argument-Uebergabe an das gespawnte
Programm, NovaFS-Blatt-Splitting.
