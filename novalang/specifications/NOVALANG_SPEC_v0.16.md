# NovaLang 0.16 – NovaShell: echte `ls`/`exit`-Befehle

Baut auf v0.1-v0.15 auf. Reine Anwendungs-Erweiterung in
`NovaShell.nova` - KEINE neue Compiler-Grammatik.

## 1. `DoListRoot()` - echter `ls`-Befehl

`VFS_OP_LOOKUP("/")` liefert ein Handle auf die NovaFS-Wurzel, danach
wiederholtes `VFS_OP_READ_DIRECTORY` (Index 0,1,2,... bis zum ersten
Fehlschlag - `NOT_FOUND`, da der Index die Anzahl der Eintraege
ueberschreitet) liest jeden Verzeichniseintrag in einen
`vfsapi_dir_entry_t`-Puffer. Der Name liegt dort bei Byte-Offset 20
(siehe `vfsapi.h`) - da `Console.WriteBuffer` nur ab Offset 0 eines
BENANNTEN Puffers ausgeben kann, wird der Name per `PeekByte`/
`PokeByte` (beide mit Laufzeit-Offset, v0.15) in einen eigenen
Puffer kopiert, dann ausgegeben.

**Wird deterministisch direkt in `Main()` aufgerufen** (nicht nur
hinter Live-Eingabe versteckt) - eine echte VFS-Verzeichnisauflistung
braucht keinen Tastendruck, um boot-verifizierbar zu sein.

## 2. `CommandIsExit()`/`CommandIsLs()` - zwei weitere Dispatch-Bausteine

Byte-fuer-Byte-Vergleich gegen `"EXIT"` (4 Byte) bzw. `"LS"` (2 Byte),
nach demselben Muster wie `CommandIsHelp()` (v0.13). `EXIT` erkennt den
Befehl, terminiert aber NOCH NICHT echt (siehe Abschnitt 4) - `LS`
ruft bei Erkennung `DoListRoot()` auf.

## 3. Boot-verifiziert

Serial-Output bestaetigt eine ECHTE, vollstaendige NovaFS-Wurzel-
Auflistung (14 Eintraege: die 12 von `kmain.c` erzeugten Testdateien +
`"System"` (neu, v0.14) + `"Benutzer"` (bereits im Basis-Image
vorhanden)). Voller Boot bis `NOVA_KERNEL_READY`, 0
`"FEHLGESCHLAGEN"`-Zeilen im gesamten Boot-Log.

## 4. Explizit weiterhin NICHT Teil dieser Version

Echte Prozess-Terminierung fuer `exit` (kein sicherer allgemeiner
Terminierungsmechanismus in NovaOS, siehe `NOVALANG_SPEC_v0.1.md`
Abschnitt 3), Unterverzeichnisse auflisten (nur die Wurzel), `cd`/
`cat`/`cp`/`mv`/`rm`/`mkdir`/`run` und die restlichen ~23
Pflichtbefehle, Argumentparsing (Befehl + Parameter), echte
dauerhaft laufende Live-Polling-REPL-Schleife.
