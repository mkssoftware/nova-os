/* NovaOS C-Kernel - Nova Desktop Shell ("Desktop First"). Zeigt so frueh
   wie moeglich einen echten, komponierten Desktop (Hintergrund + Cursor)
   ueber display.c/compositor.c, bevor nicht-kritische Subsysteme im
   Hintergrund weiterladen. Ersetzt den fruehen Flaechen-Fill-Platzhalter
   durch echte Compositor-Komposition (siehe NPSPEC-DISPLAY-ARCH-0007
   "Desktop Ready" / "Background Initialization").

   Taskbar/Desktop-Icons/Startmenue/Notifications zeichnet seit deren
   Portierung NICHT mehr dieses Modul, sondern desktop_shell.c (echte
   window.c/surface.c-Fenster, laeuft erst nach dem Compositor-Thread-
   Start in kmain.c) - vorher ueberlagerten sich die alte, hier gezeichnete
   Rechteck-Taskbar und die neue echte Taskbar sichtbar am unteren
   Bildschirmrand. Dieses Modul bleibt bewusst auf den allerersten,
   minimalen sichtbaren Frame reduziert. */
#ifndef NOVA_DESKTOP_H
#define NOVA_DESKTOP_H

/* 0 = Desktop komponiert und angezeigt, -1 = kein Framebuffer verfuegbar. */
int desktop_show(void);

#endif
