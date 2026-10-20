/* NovaOS C-Kernel - minimaler Desktop-Platzhalter ("Desktop First").
   Zeigt so frueh wie moeglich eine Desktop-Flaeche, bevor nicht-kritische
   Subsysteme im Hintergrund weiterladen. Der echte Compositor/Window-Manager
   (siehe boot/bootloader/bootmenu/) wird spaeter als Hintergrund-Subsystem
   portiert und ersetzt diesen Platzhalter. */
#ifndef NOVA_DESKTOP_H
#define NOVA_DESKTOP_H

/* 0 = Desktop gezeichnet, -1 = kein Framebuffer verfuegbar. */
int desktop_show(void);

#endif
