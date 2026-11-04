/* NovaOS C-Kernel - Nova Taskbar Liquid Glass Integration. Verdrahtet die
   Ring-3-Taskbar (ushell/shell_main.c, laeuft als echter Userspace-Prozess
   nur ueber int-0x80-Syscalls, siehe syscall.c) MIT der gesamten seit
   Phase 1 aufgebauten Graphics-Engine-Kette - der erste Anschluss dieser
   zwoelf Phasen an ein tatsaechlich sichtbares Feature.

   ARCHITEKTUR-PROBLEM UND LOESUNG: Ring 3 hat KEINEN Zugriff auf
   Kernel-Header/-Funktionen (nur Syscalls) und kann daher keines der
   Graphics-Engine-Module selbst aufrufen. shell_main.c zeichnet seine
   Taskbar deshalb unveraendert als flache Farbflaeche in seinen eigenen,
   per GUI_OP_ATTACH_BUFFER angehefteten Puffer. Dieses Modul haengt sich
   auf der KERNEL-Seite an genau zwei bestehende Syscall-Stellen
   (syscall.c's GUI_OP_CREATE_SURFACE/GUI_OP_COMMIT-Dispatch):

   1. `taskbarglass_on_surface_created()` erkennt per Geometrie-Heuristik
      (x=0, h=TASKBARGLASS_HEIGHT, y=Bildschirmhoehe-TASKBARGLASS_HEIGHT -
      EXAKT die Geometrie, die shell_main.c beim Erzeugen anfordert; alle
      per Syscall erzeugten Surfaces teilen sich denselben owner_pid, siehe
      SYSCALL_SINGLE_CLIENT_PID in syscall.h, daher reicht der Besitzer
      allein nicht zur Unterscheidung), OB eine neu erzeugte Surface die
      Taskbar ist, und legt bei Erkennung den gesamten Graphics-Engine-
      Zustand dafuer an (Scene Node, Materialinstanz, Backdrop-Abhaengigkeit,
      Layer, Frame-Scheduler-Ausgabe).
   2. `taskbarglass_on_commit()` laeuft bei JEDEM GUI_OP_COMMIT auf die
      erkannte Taskbar-Surface, VOR dem eigentlichen surface_commit(), und
      ersetzt per Chroma-Key-Substitution alle Pixel, die ring3
      unveraendert in der bekannten flachen Taskbar-Hintergrundfarbe
      belassen hat, durch einen echten, per Nova Liquid Glass berechneten
      Hintergrund - Start-Button und Uhrtext (andere Farben) bleiben
      UNVERAENDERT scharf. shell_main.c selbst wird dabei NICHT veraendert.

   ECHTE INTEGRATION ALLER ZWOELF PHASEN (nicht nur Aufruf-Kosmetik):
   - **Scene Graph (Phase 2)**: ein echter Node fuer die Taskbar + ein
     Backdrop-Node, in EINER gemeinsamen Szene.
   - **Materender (Phase 9)**: `materender_bind()`/`materender_evaluate()`
     entscheiden ECHT, ob der (im Vergleich zur Chroma-Key-Substitution
     selbst teurere) `matfx_apply()`-Glas-Hintergrund-Blend neu berechnet
     werden muss, oder ob der zuletzt berechnete Puffer weiterhin gueltig
     ist (Material-Generation UND Damage unveraendert).
   - **Liquid Glass (Phase 10)**: `liquidglass_set_backdrop()` haengt die
     Taskbar strukturell vom Backdrop-Node ab (DEPENDENCY-0001/BACKDROP-
     0001) - in diesem Kernel aendert sich der synthetische Backdrop nie
     (keine Bildtapete/keine Fenster unter der Taskbar), daher bleibt diese
     Abhaengigkeit nach dem ersten Aufbau praktisch immer erfuellt, ist
     aber strukturell echt vorhanden.
   - **Damage Tracking (Phase 5)**: jeder Commit meldet echten Damage fuer
     den Taskbar-Node (ring3 hat frischen Inhalt gezeichnet).
   - **Compositor/Layer (Phase 6)**: ein echter Layer mit `needs_backdrop=1`
     (Liquid Glass -> `LAYER_EFFECT_DEPENDENT`, nie okkludiert, Phase-6-
     Regel).
   - **Frame Scheduling (Phase 8)**: `fsched_frame_request()` gated die
     (relativ teure) Glas-Substitution ueber ein Mindestintervall -
     verhindert, dass sehr dichte Commits die Substitution haeufiger als
     noetig ausloesen; bei Drosselung bleibt der zuletzt gesetzte Inhalt
     bestehen, der naechste zugelassene Commit holt die Substitution nach.
   - **Rendering Optimization (Phase 11)**: `gfxoptimize_should_render()`
     prueft echten Occlusion-/Damage-/Cache-Zustand, bevor ueberhaupt
     Rechenzeit in die Substitution investiert wird.
   - **Graphics Performance (Phase 12)**: `gfxperf_begin()/_end()` misst
     die tatsaechliche Dauer jeder Glas-Substitution (Subsystem-Tag
     COMPOSITOR).
   - **Render Engine (Phase 3)**: wird TRANSITIV durch `materender_evaluate()`
     benutzt (dessen interner `render_execute_retained()`-Aufruf).

   BEWUSST NICHT TEIL DIESER INTEGRATION: Render Graph (Phase 4, fuer
   Mehr-Pass-Planung gedacht - die Taskbar ist ein einzelner Blend-Pass)
   und Display Protocol (Phase 7, ein alternatives Surface-Commit-Protokoll
   fuer kuenftige Ring-3-Clients - diese Chrome-Oberflaeche nutzt bereits
   das etablierte, funktionierende window.c/surface.c, eine Umstellung
   darauf waere eine eigene, groessere Migration ohne Mehrwert fuer dieses
   Feature). */
#ifndef NOVA_TASKBARGLASS_H
#define NOVA_TASKBARGLASS_H

#include <stdint.h>

/* MUSS mit shell_main.c's TASKBAR_HEIGHT/Fuellfarbe uebereinstimmen (siehe
   dessen draw_taskbar(): `render2d_fill_rect(surf, 0,0,width,TASKBAR_HEIGHT,
   0x10141Cu)`). Separat gepflegt, da ring3/shell_main.c komplett unabhaengig
   vom Kernel-Baum kompiliert (siehe dessen Dateikopf) - keine gemeinsame
   Header-Datei moeglich. */
#define TASKBARGLASS_HEIGHT    28u
#define TASKBARGLASS_BG_RGB    0x10141Cu
#define TASKBARGLASS_MAX_WIDTH 1280u

void taskbarglass_initialize(void);
int taskbarglass_self_test(void); /* 0 = ok */

/* Von syscall.c's GUI_OP_CREATE_SURFACE-Handler nach erfolgreichem
   window_create() aufzurufen. Erkennt per Geometrie, ob handle die
   Taskbar ist (siehe Dateikopf) - KEIN Fehler, wenn nicht (jede andere
   Surface laeuft unveraendert weiter). 1 = als Taskbar erkannt und
   Graphics-Engine-Zustand dafuer angelegt, 0 = keine Taskbar. */
int taskbarglass_on_surface_created(uint32_t handle, uint32_t x, uint32_t y,
                                     uint32_t w, uint32_t h);

/* Von syscall.c's GUI_OP_COMMIT-Handler VOR dem eigentlichen
   surface_commit() aufzurufen. No-Op (gibt 0 zurueck), wenn handle nicht
   die zuvor erkannte Taskbar ist. 1 = Pruefung/Substitution durchgefuehrt
   (unabhaengig davon, ob die Substitution durch Culling/Drosselung
   tatsaechlich uebersprungen wurde - das ist kein Fehler). */
int taskbarglass_on_commit(uint32_t handle);

#endif
