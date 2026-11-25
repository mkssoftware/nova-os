# NovaLang 0.14 – NovaFS: echtes B+-Baum-Leaf-Splitting + NovaShell am Masterauftragspfad

Baut auf v0.1-v0.13 auf. Kernel-Erweiterung (nicht NovaLang selbst),
siehe `feedback_novafs_root_directory_full.md`/`project_novashell.md`
Punkt 7. Schliesst den in v0.13 gefundenen, konkreten Blocker: NovaFS
konnte systemweit nur 13 Verzeichniseintraege insgesamt verwalten (ein
einziges, nie gesplittetes B+-Baum-Blatt), was eine echte Installation
am Masterauftrags-Zielpfad `/System/Apps/Shell/NovaShell.nef`
verhinderte.

## 1. `novafs_tree_insert()` - echtes B+-Baum-Splitting

`novafs_tree_insert_simple()` (unveraendert, weiterhin vom eigenen
Selbsttest genutzt, der explizit das ALTE Nicht-Split-Verhalten bei
einem vollen Blatt prueft) implementierte bewusst nur den Fall "Blatt
hat noch Platz". Die neue `novafs_tree_insert()` (`novafs.c`/`novafs.h`)
ist der fuer produktiven Code zu verwendende Einfuege-Einstiegspunkt:

- **Leaf-Split:** ist das per `novafs_cursor_seek()` positionierte
  Blatt voll, werden die vorhandenen Items + das einzufuegende Item
  (max_items+1 insgesamt) in einen Scratch-Puffer kopiert, zur Haelfte
  im bestehenden Block belassen, zur Haelfte in einen NEU allozierten
  Geschwisterblock (`novafs_alloc_block()`, bereits etablierter, unter
  Transaktionen sicherer Mechanismus) geschrieben.
- **Inner-Node-Split (Propagation nach oben):** ist der Elternknoten
  beim Einfuegen des neuen Separators auch voll, wird ER nach demselben
  Muster gesplittet - eine Schleife traegt das beliebig weit nach oben
  (bis `NFS_MAX_HEIGHT=4`).
- **Wurzelwachstum:** erreicht die Propagation die Wurzel, baut
  `novafs_tree_grow_root()` einen NEUEN Wurzelknoten mit genau 2 Kindern
  (die alte, gesplittete Wurzel + den neuen Geschwisterblock) und haengt
  ihn per `sb_root_offset` im Superblock ein - die Baumhoehe waechst um 1.
- `NFS_N_PARENT` wird bei einem Split NUR symbolisch uebernommen (nie
  rueckwirkend korrigiert) - bewusst, weil dieses Feld laut
  `novafs_cursor_seek()` NUR fuer den jeweils AKTUELLEN Wurzelknoten
  gelesen/validiert wird, nie fuer einen anderen Knoten. Ein nach einem
  Wurzelwachstum "veralteter" Parent-Zeiger auf einem Nicht-Wurzel-
  Knoten ist deshalb folgenlos.

Produktive Aufrufer umgestellt: `novafs_create()` (Objekt- UND
Verzeichnis-Baum), `novafs_rename()`, Extent-Wachstum beim Datei-
Schreiben. Die 5 Selbsttest-Aufrufe (Szenario 1-3, explizit das alte
"kein Split"-Verhalten pruefend) bleiben bei `_simple()`.

## 2. Boot-verifizierter Beweis (Szenario 4, neu im B+-Baum-Selbsttest)

Der Objekt-Baum wird nach den bestehenden Szenarien absichtlich exakt
bis `max_items` (26) gefuellt (Szenario 2, unveraendert). Szenario 4
fuegt per `novafs_tree_insert()` ein 27. Item ein (Schluessel 999, den
Szenario 2 bereits erfolglos mit `_simple()` versucht hatte) - muss
jetzt erfolgreich sein, UND alle 27 Items muessen danach in sortierter
Reihenfolge per Cursor auffindbar sein, UEBER die neue Blattgrenze UND
die neu gewachsene Wurzel hinweg. Boot-verifiziert: `"NovaFS B+-Tree
Kern Selftest ok"`.

**Bug im Testwerkzeug, nicht in der echten Logik:** der erste Lauf
scheiterte - nicht am Splitting selbst (das arbeitete von Anfang an
korrekt, per granularer Diagnose bestaetigt: korrekter neuer Block,
korrekte Links/Rechts-Aufteilung, korrektes Wurzelwachstum), sondern am
B+-Baum-Selbsttests EIGENEM synthetischem Fake-Blockgeraet
(`novafs_tree_test_write()`), das nur zur Setup-Zeit VORREGISTRIERTE
Bloecke beschreiben konnte - ein per echtem `novafs_alloc_block()`
waehrend des Splits neu allozierter Block war darin unbekannt. Fix:
`novafs_tree_test_write()` registriert einen unbekannten Block jetzt
LAZY nach (wie ein echter Datentraeger auch), `NOVAFS_TREE_TEST_MAX_
BLOCKS` von 8 auf 16 angehoben.

## 3. NovaShell.nef an den echten Masterauftragspfad migriert

`/System`, `/System/Apps`, `/System/Apps/Shell` (3 neue Verzeichnisse,
jedes davon vorher am bereits vollen Wurzel-Blatt gescheitert) werden
jetzt erfolgreich angelegt, gefolgt von `greeting.txt` und
`NovaShell.nef` selbst - alle als eigene Objekte unter
`/System/Apps/Shell`. Die v0.13-Interim-Massnahmen sind nicht mehr
noetig:
- "Hello NovaLang" (v0.1-Test) wieder dauerhaft installiert+gespawnt
  (Quelltext war ohnehin nie geloescht).
- `NovaShell.nova`s VFS-Begruessungslektuere liest jetzt ECHT
  `/System/Apps/Shell/greeting.txt` (vorher: ehrlicher Fallback
  `"greeting lookup failed"`, da die Datei fehlte) - Serial-Output
  bestaetigt die tatsaechlich ausgegebene Begruessung: `"Welcome to
  NovaOS - NovaShell (work in progress)"`.

**Boot-verifiziert:** alle 5 Installationsschritte (`/System`,
`/System/Apps`, `/System/Apps/Shell`, `greeting.txt`, `NovaShell.nef`)
+ Spawn melden `"ok"`, `NOVA_KERNEL_READY` erreicht, NULL
`"FEHLGESCHLAGEN"`-Zeilen im gesamten Boot-Log (vollstaendiger
Regressionstest aller v0.1-v0.13-Testprogramme bestaetigt).

## 4. Explizit weiterhin NICHT Teil dieser Version

Placeholder-Ersetzung in `greeting.txt` (`${OS_VERSION}` etc.), Befehls-
Dispatch fuer die restlichen ~28 Pflichtbefehle, eine ECHTE dauerhaft
laufende Live-Polling-REPL-Schleife, echte Boot-Verifikation mit einem
tatsaechlichen Tastendruck, Blatt-ZUSAMMENLEGUNG bei Loeschungen
(Splitting ist einseitig - ein Baum waechst, schrumpft aber nicht
automatisch zurueck), `NFS_N_PARENT`-Nachpflege nach einem Split.
