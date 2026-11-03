/* NovaOS C-Kernel - Boot-Splash (Logo + Lauflicht-Ladebalken).

   Nutzer-Anstoss (2026-11-03): der allererste sichtbare Frame aus
   desktop.c's desktop_show() (nur Volltonfarbe + Cursor, siehe dort) blieb
   bisher den GESAMTEN Hintergrund-Ladevorgang in kmain.c ueber unveraendert
   auf dem Bildschirm stehen - der echte, fertig eingerichtete Desktop
   (Wallpaper/Taskbar/Icons, siehe kmain.c) wird erst sehr viel spaeter vom
   Compositor-Thread gezeichnet, der aber bis scheduler_enable() (ganz am
   Ende des Ring-3-Rundwegs) inaktiv bleibt. Dieses Modul ersetzt diesen
   langen, optisch toten Abschnitt durch einen animierten Splash-Screen
   (zentriertes Logo + 8-Punkte-Lauflicht), der komplett synchron aus
   kmain.c heraus angesteuert wird:

     1. bootsplash_show() NACH desktop_show() - zeichnet den ersten Frame
        direkt in den echten Framebuffer (framebuffer.c), WAEHREND der
        Compositor noch inaktiv ist - kein Platz fuer ein Kollisions-
        Problem mit echter Nebenlaeufigkeit.
     2. bootsplash_tick() aus kmain.c's report()-Helfer heraus bei JEDEM
        Subsystem-Selbsttest aufgerufen - report() wird in der gesamten
        Hintergrund-Phase hunderte Male aufgerufen, das genuegt fuer eine
        fluessig wirkende, NIEMALS stehenbleibende Animation, ohne einen
        echten Timer-Interrupt-Hook zu brauchen (waere vor Scheduler-
        Aktivierung ohnehin nicht sauber moeglich).
     3. bootsplash_hide() + ein expliziter compositor_composite()-Aufruf
        (kmain.c, unmittelbar vor ring3_enter()) ersetzen den Splash final
        durch den inzwischen fertig eingerichteten echten Desktop.

   Logo-Pixel kommen aus bootsplash_logo.h (von scripts/
   build-bootsplash-logo.ps1 aus assets/logoNew.png generiert, bereits auf
   BOOTSPLASH_BG_RGB alpha-vorgeblendet) - Kompilierzeit-Einbettung wie bei
   icons_blob.h, NICHT Laufzeit-Laden wie wallpaper.c (NovaFS ist zu diesem
   fruehen Zeitpunkt noch nicht gemountet). */
#ifndef NOVA_BOOTSPLASH_H
#define NOVA_BOOTSPLASH_H

/* 0 = Erfolg (Framebuffer vorhanden, erster Frame gezeichnet, Animation
   aktiv), -1 = kein Framebuffer verfuegbar. */
int bootsplash_show(void);

/* Rueckt die Lauflicht-Animation ggf. einen Schritt weiter und zeichnet
   dabei NUR den kleinen Ladebalken-Bereich neu (billig genug fuer einen
   Aufruf pro Subsystem-Selbsttest). No-op, falls bootsplash_show() nicht
   erfolgreich war oder bootsplash_hide() bereits aufgerufen wurde. */
void bootsplash_tick(void);

/* Beendet die Splash-Phase (weitere bootsplash_tick()-Aufrufe werden
   No-op). Zeichnet NICHT selbst den echten Desktop - der Aufrufer MUSS
   direkt danach compositor_composite() aufrufen, um den Framebuffer-
   Inhalt durch den echten, bereits vollstaendig eingerichteten Desktop zu
   ersetzen (siehe Scope-Kommentar oben). */
void bootsplash_hide(void);

/* 1 = bootsplash_show() lief erfolgreich und bootsplash_hide() wurde noch
   nicht aufgerufen. */
int bootsplash_active(void);

int bootsplash_self_test(void); /* 0 = ok */

#endif
