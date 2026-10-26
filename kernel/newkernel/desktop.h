/* NovaOS C-Kernel - Nova Desktop Shell ("Desktop First"). Zeigt so frueh
   wie moeglich einen echten, komponierten Desktop (Hintergrund + Taskleiste
   + Beispielfenster mit Schatten + Cursor) ueber display.c/compositor.c,
   bevor nicht-kritische Subsysteme im Hintergrund weiterladen. Ersetzt den
   fruehen Flaechen-Fill-Platzhalter durch echte Compositor-Komposition
   (siehe NPSPEC-DISPLAY-ARCH-0007 "Desktop Ready" / "Background
   Initialization"). Der volle Window Manager (Fensterverwaltung,
   mehrere echte Clients) ist weiterhin NICHT portiert. */
#ifndef NOVA_DESKTOP_H
#define NOVA_DESKTOP_H

/* 0 = Desktop komponiert und angezeigt, -1 = kein Framebuffer verfuegbar. */
int desktop_show(void);

#endif
