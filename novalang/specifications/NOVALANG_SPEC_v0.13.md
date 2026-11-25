# NovaLang 0.13 – NovaShell.nef installiert (INTERIM-Pfad, echter Blocker dokumentiert)

Baut auf v0.1-v0.12 auf. Erster echter Baustein von NovaShell 1.0
selbst (nicht mehr nur ein Compiler-Testprogramm), siehe
`NovaShell.nova`. KEINE neue Compiler-Grammatik - reine Anwendung der
bis v0.12 geschaffenen Sprachmittel.

## 1. Was NovaShell.nova in dieser Version beweist

- **Echte VFS-Begruessungsdatei-Lektuere:** `SysCall(3, 1, ...)`
  (VFS_OP_LOOKUP) + `SysCall(3, 2, ...)` (VFS_OP_READ) gegen eine
  konfigurierbare Datei, mit `Console.WriteBuffer` fuer die Ausgabe.
  Faellt in dieser Version ehrlich auf `"greeting lookup failed"`
  zurueck, da `greeting.txt` wegen des unten beschriebenen NovaFS-
  Blockers (noch) nicht angelegt werden konnte.
- **`CommandIsHelp()`:** erster echter Befehls-Dispatch-Baustein -
  Byte-fuer-Byte-Vergleich der Eingabezeile gegen das Literal `"HELP"`
  (ps2.c liefert Buchstaben immer grossgeschrieben, siehe
  `NOVALANG_SPEC_v0.11.md`). Boot-verifiziert mit zwei deterministischen
  Faellen (Zeile = "HELP" -> Treffer, Zeile leer -> kein Treffer).
- **Echte Live-Polling-Eingabeschleife:** `SysCall(4, 1, ...)` JEDE
  Iteration neu aufgerufen (nicht wie in v0.12 eine feste synthetische
  Folge), wiederverwendet `ProcessOneVkeyLine()` (v0.12s `ProcessOneVkey`-
  Muster). Auf 10 Iterationen begrenzt (echte Tastendruecke koennen in
  dieser Testumgebung nicht injiziert werden, siehe
  `NOVALANG_SPEC_v0.10.md`/`v0.11.md` - eine unbegrenzte Schleife wuerde
  den Boot fuer immer blockieren).

## 2. Echter, per Diagnose-Build praezise geklaerter NovaFS-Blocker

Der urspruengliche Plan war die Installation am Masterauftrags-Pfad
`/System/Apps/Shell/NovaShell.nef` (3 Verzeichnisse + 2 Dateien = 5 neue
NovaFS-Objekte). Das scheiterte boot-verifiziert. Durch direkte
Instrumentierung von `novafs_tree_insert_simple()` (temporaer, siehe
`feedback_novafs_root_directory_full.md`) wurde zweifelsfrei geklaert:

**Der GESAMTE NovaFS-Verzeichnisbaum (ALLE Verzeichnisse systemweit,
nicht pro Elternverzeichnis) teilt sich aktuell EIN EINZIGES Blatt mit
`max_items=13`.** Eine fruehere Annahme dieser Session ("Verschachtelung
verteilt neue Eintraege auf frische Blaetter pro Elternverzeichnis")
war FALSCH - bestaetigt durch den Diagnose-Log: das Anlegen von
`/System/Apps` (Elternverzeichnis = die NEU erzeugte `/System`-ID, kein
Bezug zur Wurzel) griff auf DASSELBE Blatt zu, das bereits durch die
Wurzel-Eintraege gefuellt war (`count` sprang von 12 auf 13 beim
Anlegen von `/System`, danach WIEDER 13 beim - gescheiterten - Versuch,
`/Apps` darunter anzulegen).

Ohne echtes B+-Baum-Leaf-Splitting (von `novafs_tree_insert_simple()`
bewusst nicht implementiert, siehe Code-Kommentar in `novafs.c`) ist
die GESAMTE NovaFS-Installation seit dieser Session bei maximal 13
Verzeichniseintraegen insgesamt gedeckelt.

## 3. Fix fuer DIESE Version: ein Slot freigemacht, flach installiert

1. `"Hello NovaLang"` (v0.1-NovaLang-Test, die einzige Faehigkeit war
   `Console.WriteLine`, die jedes andere NovaLang-Testprogramm ebenfalls
   zeigt) wurde aus der dauerhaften NovaFS-Installation in `kmain.c`
   entfernt (Quelltext `novalang/examples/hello_nova.nova` bleibt
   vollstaendig erhalten, `novac` kompiliert ihn weiterhin jederzeit als
   eigenstaendigen Compiler-Regressionstest) - macht GENAU EINEN Slot
   frei.
2. `NovaShell.nef` wird mit diesem einen Slot FLACH unter
   `/NovaShell.nef` installiert (NICHT am Masterauftragspfad) -
   `greeting.txt` entfaellt (kein zweiter Slot mehr frei).

**Boot-verifiziert:** `"NovaShell.nef anlegen (INTERIM, flach) ok"`,
`"spawnen ok"`, danach echter Programmoutput: `"NovaShell 1.0 (work in
progress) starting"`, `"greeting lookup failed"` (ehrlich, da die Datei
fehlt), `"command match HELP ok"`, `"command match empty correctly
rejected"`, `"no real keypress captured..."`. Voller Boot bis
`NOVA_KERNEL_READY`, alle bisherigen Testprogramme (ausser dem bewusst
entfernten "Hello NovaLang") weiterhin korrekt.

## 4. Offene Entscheidung (Nutzer, siehe project_novashell.md Punkt 7)

Die Migration zum echten Masterauftragspfad braucht EINE von zwei
echten Loesungen:
- (a) echtes B+-Baum-Leaf-Splitting fuer `novafs_tree_directory`
  implementieren - der permanente Fix, aber eine eigenstaendige,
  nicht-triviale Dateisystem-Aufgabe.
- (b) 3 WEITERE bestehende Testdateien opfern/zusammenlegen, um die
  verbleibenden 3 Slots (fuer `Apps`, `Shell`, `NovaShell.nef`
  getrennt von `greeting.txt`) freizumachen - schneller, aber verliert
  fast die Haelfte der NovaLang-Compiler-Regressionstests als
  dauerhafte Boot-Installationen (Quelltexte bleiben in jedem Fall
  erhalten).

## 5. Explizit weiterhin NICHT Teil dieser Version

Masterauftragspfad `/System/Apps/Shell/NovaShell.nef`, Begruessungs-
Datei + Platzhalter-Ersetzung, weitere Befehle ausser `HELP`, echte
Boot-Verifikation mit tatsaechlichem Tastendruck, echtes B+-Baum-
Splitting in NovaFS.
