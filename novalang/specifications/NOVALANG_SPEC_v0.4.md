# NovaLang 0.4 – Erweiterung: genereller Syscall-Zugriff

Baut auf v0.1-v0.3 auf. Groesster Einzelschritt Richtung NovaShell-
Faehigkeit (siehe `project_novashell.md`): bisher konnte NovaLang-Code
NUR `Console.WriteLine` (fest verdrahtet auf `SYSCALL_CORE_DEBUG_WRITE`)
aufrufen. Jetzt kann NovaLang-Code JEDEN NovaOS-Syscall direkt aufrufen.

## 1. Neue Grammatik

```
AssignStmt := Ident "=" (IntExpr | SysCallExpr) NEWLINE
SysCallExpr := "SysCall" "(" SysArg "," SysArg "," SysArg "," SysArg ")"
SysArg      := IntLiteral | Ident
```

`SysArg` ist bewusst NUR Literal/Variable (kein verschachtelter Ausdruck) -
siehe Abschnitt 3 fuer die Begruendung.

Testprogramm (`novalang/examples/hello_syscall.nova`), boot-verifiziert
mit korrektem Ergebnis ("generic syscall returned OK"):

```novalang
Dim result As Integer = 999
result = SysCall(1, 13, 0, 0)   ' SYSCALL_SERVICE_CORE, SYSCALL_CORE_YIELD
If result = 0 Then
    Console.WriteLine("generic syscall returned OK")
Else
    Console.WriteLine("generic syscall returned unexpected status")
End If
```

## 2. Semantik

`result = SysCall(service, operation, arg1, arg2)` ruft den Syscall exakt
nach der in `syscall.c syscall_dispatch()` ermittelten (NICHT erfundenen)
Konvention auf: `EAX`=service, `EBX`=operation, `ECX`=ABI-Version (IMMER
fest `SYSCALL_ABI_VERSION`=1, kein NovaLang-Operand), `EDX`=arg1,
`ESI`=arg2, `INT 0x80`. Das Ergebnis (`EAX` nach der Instruktion, i.d.R.
ein `SYSCALL_STATUS_*`-Code) wird in die Ziel-Integer-Variable
geschrieben.

**Was `SysCall` NICHT kann (v0.4):** `arg1`/`arg2` sind nur `EDX`/`ESI` -
viele Syscalls (VFS/Input) erwarten dort einen POINTER auf eine Argument-
Struct im validierten Fenster `[0x400000,0x404000)`, keinen rohen
Skalarwert. Ohne echte Laufzeit-Strukturen/Speicheradressen-Operanden
(noch nicht Teil von NovaLang) kann `SysCall` bisher nur Operationen
nutzen, die ausschliesslich skalare Argumente brauchen (z.B.
`SYSCALL_CORE_YIELD`, `SYSCALL_CORE_TOGGLE_*`). VFS-/Input-Aufrufe
brauchen zusaetzlich einen Weg, eine Struct im DATA-Bereich anzulegen UND
ihre Adresse als `SysCall`-Argument zu uebergeben - das ist der naechste
Schritt (siehe `project_novashell.md` Prioritaet 3 "echte Laufzeit-
Strings"/Struct-Support).

## 3. Codegenerierung

Jedes der vier Argumente wird DIREKT in sein Zielregister geladen
(`mov <reg>, imm32` oder `mov <reg>, [addr]` je nach Operandentyp) - KEIN
Umweg ueber ein gemeinsames Register. Neuer generischer Hilfsbaustein
`emit_mov_reg_from_mem32()` (ModRM mit beliebigem reg-Feld, nicht nur
EAX wie die bisherigen `emit_eax_memop32()`-Aufrufe) macht das moeglich.

`SysArg` ist bewusst NICHT Teil einer `+`/`-`-Kette (anders als normale
`IntExpr`-Terme): eine Kombination wie `x = 5 + SysCall(...)` wuerde
einen Registerkonflikt erzeugen (der Syscall selbst clobbert EAX/EBX/
ECX/EDX/ESI - jedes dieser Register wird fuer die Akkumulation in einer
`+`/`-`-Kette gebraucht). Diese Einschraenkung ist dokumentiert, keine
versteckte Luecke.

## 4. Kernel-Erweiterung: keine

Diese Version brauchte KEINE Kernel-Aenderung - `SysCall` nutzt exakt den
bereits bestehenden `int 0x80`-Mechanismus, keinen neuen Syscall.

## 5. Explizit weiterhin NICHT Teil dieser Version

Siehe v0.1-v0.3 - zusaetzlich: Pointer-/Struct-Argumente fuer `SysCall`
(noetig fuer VFS/Input), ein neuer Prozess-Spawn-Syscall (noetig fuer
NovaShells `run`-Befehl), `For`/`While`, Funktionsparameter, echte
Laufzeit-Strings.
