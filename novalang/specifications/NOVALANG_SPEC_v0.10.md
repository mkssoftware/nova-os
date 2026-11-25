# NovaLang 0.10 – Erweiterung: Tastatur-/Maus-Input-Polling

Baut auf v0.1-v0.9 auf. Reine Anwendungs-/Beweis-Erweiterung - KEINE
neue Compiler-Grammatik und KEINE Kernel-Aenderung noetig. Schliesst den
in `project_novashell.md` Phase-A-Fund benannten Baustein "Tastatur-
Eingabe" fuer NovaShells geplante REPL-Eingabezeile an, indem bewiesen
wird, dass der bereits bestehende generische `SysCall(...)`-Mechanismus
(siehe `NOVALANG_SPEC_v0.4.md`) den Input-Polling-Syscall direkt
aufrufen kann - genau wie `NOVALANG_SPEC_v0.5.md`s VFS-Test bereits fuer
`VFS_OP_LOOKUP` bewiesen hat.

## 1. Kein neuer Syscall, keine neue Grammatik

`SYSCALL_SERVICE_INPUT` (4) / `INPUT_OP_POLL` (1) existierten bereits im
Kernel (siehe `syscall.h`/`syscall.c syscall_dispatch_input()`) -
dieselbe EDX-Pointer-auf-Struct-Konvention wie bei den VFS-Syscalls.
`syscall_input_poll_args_t` ist 40 Byte gross (10 x `uint32_t`:
`has_event`, `type`, `timestamp_ticks`, `key_vkey`, `modifiers`,
`mouse_dx`, `mouse_dy`, `mouse_buttons`, `cursor_x`, `cursor_y`).
`hello_syscall.nova` (v0.4-Test) wurde ERWEITERT (nicht als eigene
Datei, siehe Abschnitt 3): `Dim pollbuf As Buffer(40)` + `SysCall(4, 1,
AddressOf(pollbuf), 0)`, in einer `While`-Schleife dreimal aufgerufen,
danach `PeekInt(pollbuf, 0)` fuer `has_event`.

## 2. Wichtige Einschraenkung dieser Version: keine echte Tastendruck-Verifikation

Diese QEMU-Testumgebung kann laut einem frueheren, bereits
dokumentierten Befund (siehe `project_keyboard_shortcuts.md`) KEINE
echten Tastendruecke per QMP injizieren. Dieser Test beweist deshalb
NUR den Polling-SYSCALL-MECHANISMUS selbst:
- wiederholter Aufruf in einer Schleife ohne Crash (REPL-taugliches
  Muster - eine Eingabezeile braucht genau so eine Schleife),
- korrekter `SYSCALL_STATUS_OK`-Rueckgabewert bei JEDEM Aufruf,
- korrektes Auslesen von `has_event` per `PeekInt` (erwartungsgemaess 0
  in einem Boot ohne echte Tastatur-/Maus-Interaktion - `has_event==0`
  ist laut Syscall-Doku explizit KEIN Fehler, siehe `syscall.h`).

**NICHT bewiesen (bewusst, ehrlich benannt):** eine echte VKEY->ASCII-
Uebersetzung (der Syscall liefert nur Scancodes, keine Zeichen) - das
ist ein separater, noch offener Schritt, sobald eine echte Tasten-
Interaktion in dieser Umgebung verifizierbar ist oder ein anderer
Verifikationsweg (analog zum synthetischen `input_self_test()` aus
`project_keyboard_shortcuts.md`) gefunden wird.

## 3. Keine neue NovaFS-Datei

Siehe `feedback_novafs_leaf_capacity.md` - der Test wurde an
`hello_syscall.nova`/`hello_syscall.nef` angehaengt statt eine neue
Datei anzulegen.

## 4. Boot-verifiziert

Serial-Output bestaetigt (neben den unveraenderten v0.4-Zeilen):
`"input poll ok"` (3x), `"input poll loop finished"`,
`"input queue empty as expected (headless boot)"`. Voller Boot bis
`NOVA_KERNEL_READY`, alle bisherigen NEF-/NovaLang-Testprogramme
weiterhin korrekt.

## 5. Explizit weiterhin NICHT Teil dieser Version

VKEY->ASCII-Uebersetzung, eine echte Eingabezeile/REPL-Schleife mit
Zeichen-Puffer und Backspace/Enter-Behandlung, Maus-Events, echte
Tastendruck-Verifikation in dieser Testumgebung.
