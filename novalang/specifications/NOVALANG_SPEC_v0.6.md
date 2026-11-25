# NovaLang 0.6 – Erweiterung: While-Schleife

Baut auf v0.1-v0.5 auf. Erster echter Kontrollfluss-Wiederholungsmechanismus
- direkt motiviert durch NovaShells REPL-Hauptschleife (siehe
`project_novashell.md`). Testprogramm (`novalang/examples/hello_while.nova`),
boot-verifiziert mit korrektem Ergebnis: Schleife 1 zaehlt `i` von 0 bis 5
hoch (genau 5 "loop iteration"-Zeilen), Schleife 2 prueft, dass eine von
Anfang an falsche Bedingung NULL Durchlaeufe ergibt (kein Do-While-
Verhalten) - "this must never print" erscheint tatsaechlich nie.

## 1. Neue Grammatik

```
Statement  := ... | WhileStmt
WhileStmt  := "While" Cond NEWLINE Statement* "End" "While" NEWLINE
Cond       := Ident CmpOp (IntLiteral | Ident)
```

`Cond` ist jetzt ein eigener, von `If` UND `While` gemeinsam genutzter
Grammatik-Baustein (`parse_cond()` im Parser) - identische Einschraenkung
wie bisher: die linke Seite MUSS eine bereits deklarierte Integer-Variable
sein, kein beliebiger Ausdruck. **Kein `Exit While`** in dieser Version -
eine Schleife laeuft immer bis die Bedingung falsch wird.

## 2. Semantik

`While Cond ... End While` prueft die Bedingung VOR jedem Durchlauf
(inklusive dem ersten) - eine von Anfang an falsche Bedingung fuehrt zu
null Durchlaeufen, genau wie in den meisten Sprachen (kein Do-While).

## 3. Codegenerierung

Teilt sich die Bedingungsauswertung mit `If` (`emit_cond_inverse_jump()`
im Codegenerator, `mov eax,[lhs]` / `cmp eax,rhs` / invertierter `jcc`):

```
loop_start:
    mov eax, [lhs]
    cmp eax, <rhs>
    jcc_invers  loop_exit      ; Backpatch, Ziel erst nach dem Koerper bekannt
    <Schleifenkoerper>
    jmp         loop_start     ; Ziel schon bekannt, kein Backpatching noetig
loop_exit:
```

Neuer Hilfsbaustein `emit_jmp_to()` fuer Sprunge zu einem BEREITS
bekannten (typischerweise rueckwaerts liegenden) Ziel - ersetzt den
bisherigen Ad-hoc-Inline-Code der `Return`-YIELD-Endlosschleife (siehe
v0.1), die jetzt denselben Baustein verwendet. Gleiche `rel8`-
Bereichsgrenze wie bei `If` (siehe `NOVALANG_SPEC_v0.3.md` Abschnitt 3) -
ein zu grosser Schleifenkoerper fuehrt zu einem ehrlichen Compilerfehler,
nicht zu stillschweigend falschem Code.

## 4. Keine neuen Kernel-Aenderungen

Diese Version brauchte KEINE Kernel-/NEF-Aenderung - reine Codegen-
Erweiterung auf Basis bereits vorhandener Bausteine.

## 5. Explizit weiterhin NICHT Teil dieser Version

Siehe v0.1-v0.5 - zusaetzlich: `Exit While`/`Continue`, `For`-Schleifen,
beliebige Ausdruecke in der Bedingung (nur Variable-vs-Literal/Variable),
verschachtelte Schleifen wurden NICHT explizit getestet (sollten durch
die rekursive `parse_stmt_list()`/`emit_stmt_list()`-Struktur funktionieren,
aber ohne eigenen Testnachweis in dieser Version).
