# NovaOS Desktop – Implementierungsplan

## Ausgangslage

NovaOS besitzt bereits eine native C17-UI-Runtime mit Retained Scene Graph,
DLU-Layout, Damage Tracking, Accessibility Tree, Input Routing, Window- und
Surface-Verträgen, Frame Scheduling sowie einer ersten Kernel-/Ring-3-Anbindung.
Die neue Oberfläche wird auf diesen Verträgen aufgebaut; es wird kein separates
Web-Framework eingeführt.

Die fünf Designreferenzen werden zu einem gemeinsamen System zusammengeführt.
Die ersten vier Bilder bestimmen Desktop, Fenster, Taskleiste und Anwendungen.
Aus der fünften Referenz wird ausschließlich der dreispaltige Aufbau des
Startmenüs übernommen.

## Umsetzungsreihenfolge

1. **Designsystem und Shell-Verträge**
   Aurora-/Acrylic-Farben, DLU-Radien, Abstände, Motion-Zeiten, responsive
   Shell-Geometrie und ein heapfreier Fensterzustand.
2. **Produktiver Desktop-Renderer**
   Branding, globale Befehlsleiste, Systemstatus, Acrylic-Taskleiste,
   Nova-Orb, aktive App-Indikatoren und Wallpaper-/Aurora-Hintergrund.
3. **Startmenü**
   Linke Navigation, App-Raster und Vorschläge sowie rechte Profil-, Zeit-,
   Nova-AI-, System- und Recent-Spalte. Öffnen/Schließen, Escape, Outside-Click,
   Fokus und 190-ms-Transition.
4. **Window Manager**
   Aktivieren, Verschieben, Minimieren, Maximieren, Wiederherstellen und
   Schließen mit Damage-basierten Neuzeichnungen.
5. **Nova Explorer**
   Sidebar, Breadcrumb, Suche, Toolbar, Ordnerkarten, Dateiliste und Statusbar.
6. **Nova Sheet**
   gemeinsames Ribbon, fokussierbare Budgettabelle, Tabs, Statusbar und
   einklappbares Fähigkeiten-Panel.
7. **Fähigkeiten Studio**
   Katalog, verschiebbare Nodes, Ports und Verbindungen, Pan/Zoom/Fit sowie
   Inspector und Testbereich.
8. **Feinschliff und Validierung**
   Accessibility, Tastaturbedienung, mehrere Auflösungen, visuelle QEMU-
   Screenshots, Performance- und Damage-Prüfung.

## Bereits umgesetzt

- zentrale NovaOS-Aurora-/Acrylic-Tokens ohne komponenteneigene Farben
- einheitliche Radien für Controls, Cards, Fenster, Taskleiste und Startmenü
- responsive Shell-Geometrie ab 1280×720 sowie kompakter Fallback
- Taskleisten-Pins für Explorer, Browser, Mail, Code, Terminal, Nova Sheet und
  Fähigkeiten Studio
- Startmenü-Zustand mit Animation, Toggle, Escape und Outside-Click
- globale Command Palette mit `Ctrl+K`
- Fensterzustände für Explorer, Sheet und Fähigkeiten Studio einschließlich
  Aktivieren, Verschieben, Minimieren, Maximieren, Wiederherstellen und Schließen
- automatisierter nativer Runtime-Test ohne Heap- oder Betriebssystemabhängigkeit
- produktive Darstellung von Branding, globaler Befehlsleiste, Systemstatus,
  NovaWindow-Explorer, Nova-Orb, Aurora-Taskleiste und dreispaltigem Startmenü
  durch den Kernel-Software-Renderer
- vollständiger Ring-3-Eingabepfad für Fokus, Startmenü und geordnetes
  Herunterfahren; reine semantische Fokuswechsel vermeiden unnötige Vollframes

Als nächste sichtbare Ausbaustufe werden Explorer-Auswahl und
Fensterinteraktionen an Maus- und Tastaturereignisse gebunden. Danach folgen
die produktiven Szenen für Nova Sheet und Fähigkeiten Studio.
