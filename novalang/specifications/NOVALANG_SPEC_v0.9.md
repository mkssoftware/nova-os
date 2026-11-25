# NovaLang 0.9 – Erweiterung: Mehrere Functions + Parameter

Baut auf v0.1-v0.8 auf. Reine Compiler-Erweiterung (keine Kernel-
Aenderung, anders als v0.8) - schliesst die in `project_novashell.md`
Phase-A-Analyse benannte Luecke "nur EINE Function pro Module, keine
Parameter", die fuer eine wartbare Befehls-Dispatch-Struktur in
NovaShell (statt einer einzigen riesigen Main-Funktion) gebraucht wird.

## 1. Grammatik

```
FunctionDecl := "Function" Ident "(" ParamList? ")" "As" "Int32" NEWLINE
                 Statement*
                 "End" "Function" NEWLINE
ParamList    := Ident "As" "Integer" ("," Ident "As" "Integer")*

CallAssignStmt := Ident "=" Ident "(" (SysArg ("," SysArg)*)? ")" NEWLINE
```

Ein Module darf jetzt mehrere `Function`-Deklarationen enthalten
(v0.9-Limit: 16, siehe `ast.h NcModule::functions[16]`). **Main muss
IMMER die ERSTE Function im Module sein** - haelt `entry_offset=0` beim
`nef link` trivial (siehe Abschnitt 3). Jede weitere Function darf einen
beliebigen Namen tragen (ausser "Main", das ist reserviert), hat 0-8
Integer-Parameter (v0.9-Limit, siehe `NcFunction::param_var_index[8]`)
und gibt per `Return IntExpr` einen echten `Int32`-Wert zurueck.

**Return-Semantik ist bewusst GESPALTEN** (siehe `parser.c`
`parse_return()`/`NcParser::in_main`):
- In Main: UNVERAENDERT gegenueber v0.1 - `Return` IntLiteral (kein
  Ausdruck), kodiert NICHTS, geht in die dauerhafte
  `SYSCALL_CORE_YIELD`-Schleife (kein Prozess-Exitcode-Mechanismus in
  NovaOS, siehe `NOVALANG_SPEC_v0.1.md` Abschnitt 3).
- In jeder anderen Function: `Return` IntExpr (voller Ausdruck wie bei
  `If`/`While`-Bedingungen), echter Rueckgabewert in EAX, echtes `ret`.

Ein Funktionsaufruf ist nur als `Ident "=" Ident "(" ... ")"` erlaubt
(Zuweisung des Rueckgabewerts) - kein bare Aufruf als eigenstaendiges
Statement in dieser Version. Argumente nutzen dieselbe eingeschraenkte
`SysArg`-Grammatik wie `SysCall(...)` (Literal/Variable/`AddressOf`/
`PeekInt`/`PeekByte`, kein verschachtelter Ausdruck).

## 2. Parameter = Integer-Variablen mit Namensmangling

Ein Parameter ist INTERN eine ganz normale Integer-Variable (derselbe
Mechanismus wie `Dim x As Integer`, siehe `NOVALANG_SPEC_v0.3.md`), nur
mit einem gemangelten Namen (`"<FuncName>.<ParamName>"`), VOR dem Parsen
des Funktionskoerpers in `mod->int_var_names`/`int_var_initial`
registriert (siehe `parser.c parse_function()`). Dadurch braucht KEINE
der bestehenden Stellen, die eine Variable referenzieren (Zuweisung,
`If`/`While`-Bedingung, `SysCall`-/Call-Argument), eine Sonderbehandlung
fuer Parameter - sie sehen einfach wie jede andere Integer-Variable aus.

`lookup_var()` (ersetzt `find_int_var()` an JEDER Stelle, die eine
Variable IM FUNKTIONSKOERPER aufloest) versucht zuerst den gemangelten
Namen innerhalb der aktuell geparsten Function, dann den Klarnamen
(Modul-globale `Dim`-Variablen) - `Dim`-Deklarationen selbst bleiben
bewusst UNGEMANGELT.

**Bewusste, dokumentierte Grenze:** KEINE echte Stackframe, KEINE
Rekursions-/Wiedereintritts-Sicherheit. Ein rekursiver oder
wiedereintretender Aufruf (z.B. eine Function, die sich selbst oder
sich gegenseitig rekursiv aufruft) wuerde sich seine eigenen
Parameter-Slots ueberschreiben, da sie feste Adressen in der
DATA-Section sind (wie jede andere Integer-Variable), keine
Stack-Allokation. Fuer NovaShells geplante Befehls-Dispatch-Struktur
(nicht-rekursive Hilfsfunktionen) ausreichend.

## 3. Codegen: mehrere Functions, echtes `call rel32`/`ret`

Main (immer `functions[0]`) wird WEITERHIN als ERSTES emittiert, bei
Code-Offset 0 - der bestehende `entry_offset=0`-Vertrag beim `nef link`
bleibt unveraendert. Jede weitere Function bekommt ihren Code-Offset
ERST unmittelbar vor ihrer eigenen Emission (siehe `codegen_x86.c
nc_codegen_x86()`).

Ein Aufruf (`STMT_ASSIGN_CALL`) bindet zuerst jedes Argument in den
Parameter-Slot der Zielfunktion (Wert nach EAX laden, dann `mov
[param_addr], eax` - dieselbe Lade-Logik wie bei `PokeInt`/`SysCall`-
Argumenten), dann `call rel32` (Opcode `0xE8`), dann wird der
Rueckgabewert aus EAX in die Zielvariable gespeichert.

**Vorwaertsreferenzen (der Normalfall, da Main fast immer Helfer-
Functions aufruft, die TEXTUELL SPAETER im Modul stehen):**
- **Parser-Ebene:** `find_function()` sieht nur bereits VOLLSTAENDIG
  geparste Functions - das reicht fuer die Aufloesung eines Aufrufziels
  NICHT aus (Main wird als allererste Function geparst, bevor irgendeine
  Helfer-Function ueberhaupt existiert). Ein eigener, unabhaengiger
  Vorab-Scan (`nc_parse_module()`, eigener `NcLexer` ueber denselben
  Quelltext) sammelt deshalb VOR dem eigentlichen Parsen die Namen ALLER
  Functions im Modul (`NcParser::all_func_names`) - `find_function_any()`
  sucht darin und liefert den Index, den die Function SPAETER beim
  tatsaechlichen Parsen bekommen wird (Scan- und Parse-Reihenfolge sind
  identisch zur Quelltextreihenfolge).
- **Codegen-Ebene:** ein `call` auf eine noch nicht emittierte Function
  wird als Patch vermerkt (`NcCodegenCtx::patch_at`/`patch_func`, rel32
  statt rel8 wie beim bestehenden If/While-Backpatching, da Functions
  weiter als 127 Byte voneinander entfernt liegen koennen) und NACH der
  Emission ALLER Functions aufgeloest.

## 4. Boot-verifiziert

`hello_while.nova` (v0.6-While-Test) wurde um den v0.9-Test ERWEITERT
(siehe Abschnitt 5 - NICHT als eigene Datei), mit `Add(x, y)` (einfache
Parameter + Rueckgabewert, von Main per Vorwaertsreferenz aufgerufen -
uebt das Patch-System) und `DoubleSum(x, y)` (ruft INTERN selbst
`Add()` auf - ein echter Function-zu-Function-Aufruf, nicht nur
Main-zu-Function). Serial-Output bestaetigt: `"add ok"`,
`"nested call ok"` (neben den unveraenderten v0.6-Zeilen). Voller Boot
bis `NOVA_KERNEL_READY` erreicht.

## 5. Wichtig: KEINE neue NovaFS-Datei angelegt

Siehe `feedback_novafs_leaf_capacity.md` - mit 5 urspruenglichen
NEF-Testdateien + 7 bereits bestehenden NovaLang-Testdateien war die
NovaFS-Wurzel bereits nahe an der bekannten Blatt-Kapazitaetsgrenze
(kein Blatt-Splitting in `novafs_tree_insert_simple()`). Der v0.9-Test
wurde deshalb an `hello_while.nova`/`hello_while.nef` angehaengt statt
eine 8. neue NovaLang-Datei anzulegen.

## 6. Explizit weiterhin NICHT Teil dieser Version

Keine Rekursion/Wiedereintritts-Sicherheit (siehe Abschnitt 2), kein
bare Funktionsaufruf als eigenstaendiges Statement (nur als
Zuweisung), keine Function-lokalen `Dim`-Namensraeume (nur Parameter
sind gemangelt, `Dim` bleibt global pro Modul - ein `Dim` gleichen
Namens in zwei verschiedenen Functions wuerde kollidieren), kein
Ueberladen (zwei Functions mit demselben Namen bleiben ein Fehler),
String-/Buffer-Parameter (nur `Integer`).
