# Nova Studio – Benutzerhandbuch

Diese Anleitung beschreibt, was tatsächlich funktioniert. Siehe
`IMPLEMENTATION_STATUS.md` für eine vollständige Liste dessen, was
(noch) NICHT funktioniert - nichts davon wird hier beschrieben, als
würde es gehen.

## Start

`nova-studio.exe` ausführen (siehe `BUILD_WINDOWS.md`). Die IDE öffnet
mit eigener Titelleiste, Ribbon, Explorer links, Editor in der Mitte,
Ausgabe-/Probleme-Panel unten.

## Ein neues Projekt anlegen

Ribbon → Start → Projekt → **Neues Projekt** (kein Tastenkürzel
vorhanden, nur über den Ribbon-Button erreichbar). Name und
Speicherort wählen, "Erstellen". Nova Studio legt an:

```
<Projektname>/
  project.xml
  src/
    main.nova   (Hallo-Welt-Vorlage)
```

und öffnet `main.nova` automatisch zum Bearbeiten.

## Ein vorhandenes Projekt öffnen

Ribbon → Start → Projekt → **Projekt öffnen** (oder `Strg+Shift+O`).
Ordner mit einer `project.xml`-Datei wählen.

## Code bearbeiten

Normaler Text-Editor mit NovaLang-Syntax-Hervorhebung, mehreren
Dokument-Tabs, Suchen/Ersetzen (`Strg+F`/`Strg+H`), Gehe-zu-Zeile
(`Strg+G`), Zoom (`Strg++`/`Strg+-`), Dunkel-/Hell-Design (Ribbon →
Ansicht → Design).

**Wichtig:** jede Registerkarte merkt sich ihren eigenen, noch nicht
gespeicherten Inhalt beim Wechseln zwischen Tabs (siehe
`IMPLEMENTATION_STATUS.md` zum behobenen Datenverlust-Fehler) - trotzdem
gilt: regelmäßig speichern (`Strg+S`) bleibt sinnvoll, bevor die IDE
geschlossen wird.

## Erstellen (Build)

Ribbon → Erstellen → **Projektmappe erstellen** (`F7`). Bei offenem
Projekt wird immer die Einstiegsdatei des Projekts übersetzt (erste
Datei in `project.xml`s `<Sources>`), unabhängig davon, welche
Registerkarte gerade angezeigt wird. Ohne offenes Projekt wird die
aktuell aktive Registerkarte übersetzt (muss zuvor gespeichert sein).

Ablauf im Hintergrund: `novac.exe` kompiliert, `nef.exe link` verlinkt,
`nef.exe validate` prüft das Ergebnis - alle drei Schritte erscheinen
mit ihrer vollständigen Ausgabe im Ausgabe-Panel. Bei einem Fehler
wechselt die IDE automatisch zum **Probleme**-Tab; ein Doppelklick auf
einen Fehler öffnet die betroffene Datei an der gemeldeten Zeile.

**Neu erstellen** löscht vorher alle Build-Artefakte
(`.code.bin`/`.rodata.bin`/`.data.bin`/`.nef`) und baut komplett neu.
**Bereinigen** löscht nur diese Artefakte, ohne neu zu bauen.

## Ausführen

Ribbon → Erstellen → **Ausführen** (`F5`). Meldet den Pfad der gebauten
`.nef`-Datei und weist ausdrücklich darauf hin, dass das tatsächliche
Starten unter NovaOS/QEMU in dieser Version noch nicht automatisiert
ist (siehe `IMPLEMENTATION_STATUS.md`) - die Datei muss derzeit manuell
in ein NovaFS-Testimage übertragen und per QEMU gebootet werden.

## Eine NEF-Datei untersuchen

Eine `.nef`-Datei im Explorer doppelklicken (oder über "Datei öffnen"
auswählen) zeigt NICHT ihren Binärinhalt im Editor, sondern ruft den
**NEF Inspector** auf: `nef.exe validate` + `nef.exe inspect` im
Ausgabe-Panel, mit Header-Feldern, Sections, Flags und Zielarchitektur.

## Tastenkürzel (Auswahl)

| Kürzel | Aktion |
|---|---|
| `Strg+N` | Neue Datei |
| `Strg+O` | Datei öffnen |
| `Strg+Shift+O` | Projekt öffnen |
| `Strg+S` | Speichern |
| `Strg+Shift+S` | Speichern unter |
| `Strg+W` | Registerkarte schließen |
| `F7` | Erstellen |
| `Strg+Shift+B` | Neu erstellen |
| `F5` | Ausführen |
| `Strg+F` / `Strg+H` | Suchen / Ersetzen (im aktuellen Dokument) |
| `Strg+Shift+F` | Im Projekt suchen (alle .nova/.nlf/.nui/.xml/.md/.txt-Dateien im Projektordner) |
| `Strg+G` | Gehe zu Zeile |
| `Strg+E` | Explorer ein-/ausblenden |

## Was (noch) NICHT geht

Kurzfassung (Details: `IMPLEMENTATION_STATUS.md`): keine
Autovervollständigung/IntelliSense, kein Debugger, kein automatischer
QEMU-Start, keine Mehrdatei-Projekte mit internen Modulreferenzen, kein
visueller `.nui`-Designer, kein Installer, keine Git-Integration
(Button zeigt das ehrlich an), kein Terminal-Tab.
