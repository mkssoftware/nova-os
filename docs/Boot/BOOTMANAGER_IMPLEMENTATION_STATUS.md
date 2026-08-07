# NovaOS Bootmanager – Implementierungsstatus und Blocker

Stand: 2026-08-07

Dieses Dokument ergänzt die automatisch erzeugte Abdeckungsmatrix in
`build/bootmanager-npspec-coverage.md`. Es unterscheidet zwischen einer
fehlenden GUI-Implementierung und Funktionen, deren spezifizierte Oberfläche
vorhanden ist, deren Backend aber außerhalb des aktuellen BIOS-Bootmanagers
liegt.

## Nachgewiesene GUI-Pfade

- Hauptmenü, Fünf-Sekunden-Countdown und Standardauswahl
- Tastatur- und PS/2-Mausnavigation
- Dark Theme und High Contrast
- Kontextuelle F1-Hilfe
- Details und erweiterte Startoptionen
- Ausschalten-/Neustartdialog
- Recovery, Self-Healing-Status und Snapshot-Leerzustand
- ausführbarer AA55/55AA-Bootmanager-Speicherschnelltest
- read-only Datenträgerdiagnose
- Verschlüsselungs-, Netzwerk- und Firmwarestatus
- verständliche Bootloader- und Kernel-Fehlerdarstellung

Die zugehörigen QEMU-Nachweise sind die Make-Ziele `test`, `test-mouse`,
`test-theme`, `test-ui-flows`, `test-recovery`, `test-platform` und
`test-uefi`. Der UEFI-Test weist EFI-Einstieg, GOP-Initialisierung,
GUI-Zeichnung und die automatische Standardauswahl nach fünf Sekunden nach.

## Externe Funktionsblocker

| Bereich | Blockierende Abhängigkeit | Sicheres aktuelles Verhalten |
|---|---|---|
| Self-Healing und Dateisystemreparatur | NovaFS-Treiber, Transaktionsjournal und signierter Recovery-Snapshot fehlen | Schreibende Aktionen bleiben gesperrt; erkannte Bootartefakte werden nur angezeigt |
| Snapshot-Auswahl | Kein validierter NovaFS-Snapshot-Katalog | Erklärter Leerzustand, keine erfundenen Wiederherstellungspunkte |
| Vollständiger RAM-Test | Kein außerhalb des Bootmanagers reservierter Testbereich und kein vollständiger destruktiver Testalgorithmus | Sicherer lokaler AA55/55AA-Schreib-/Lesetest mit Wiederherstellung des Originalwerts |
| GPT/MBR, SMART und Partitionsreparatur | Storage-, ATA/AHCI- und Partitionsmodule fehlen | Ausschließlich read-only BIOS-Bootlaufwerksstatus |
| Volume-Entsperrung | Crypto-, TPM-, Smartcard- und Keyslot-Backend fehlen; kein verschlüsseltes Startvolume angefordert | Authentifizierung bleibt deaktiviert und es werden keine Geheimnisse abgefragt |
| Netzwerk-Boot | PXE-UNDI-/NIC-, DHCP-, TFTP- und HTTP-Backend fehlen | No-Adapter/No-Server-Status; kein unsicherer Netzwerkzugriff |
| Firmware-Setup | Legacy BIOS besitzt keinen standardisierten Setup-Reboot; der neue UEFI-Anwendungspfad besitzt noch keinen Runtime-Services-Setupwechsel | Hersteller-Setup-Taste wird erklärt; kein vorgetäuschter Firmwareübergang |
| Secure Boot | Legacy-BIOS-Pfad besitzt keine UEFI Secure-Boot-Datenbanken | Status wird als nicht verfügbar angezeigt |

## Blocker durch unvollständige Spezifikationen

`NPSPEC-BOOTRESOURCE-0002` beschreibt die Bestandteile eines Boot Asset
Package, legt aber kein interoperables binäres Wire-Format fest. Es fehlen
mindestens der konkrete Magic-Wert, die feste Headergröße, Feldoffsets,
Endianness, Manifest-/Index-Offsets und das Verfahren der Gesamtprüfsumme.
Die gezeigte C-Struktur ist ausschließlich ein Laufzeit-Handle und enthält
nicht die zuvor geforderten Headerfelder.

`NPSPEC-BOOTRESOURCE-0003` definiert einen Laufzeit-Descriptor, jedoch keine
normative On-Disk-Packung des Index, keinen Index-Header, keine konkrete
Resource-ID-/Hash-Ableitung und kein Kollisionsverfahren. Ein jetzt erfundenes
Format wäre nicht nachweisbar kompatibel und würde die NPSPEC umgehen.

Damit ist die Laufzeit-Auslagerung der statischen Font-, Logo- und Icondaten
in ein BAP blockiert, bis diese Felder spezifiziert sind. Ein proprietäres
Zwischenformat wird ausdrücklich nicht als BAP ausgegeben.

## Motion- und Compositor-Laufzeit

Der UEFI-Pfad verwendet nun eine feste, heapfreie Motion- und
Compositor-Laufzeit. Implementiert sind globale Millisekunden-Timeline,
deterministischer O(n)-Scheduler, Property-Interpolation, Easing,
Spring-Fallback, unterbrechbare Transitionen, Dialog-/Navigations-/Fokus- und
Progress-Motion, Reduced Motion und automatische Budget-Degradation.

Der Compositor besitzt feste Surface- und Layer-Pools, stabile Z-Sortierung,
Alpha-Compositing, Overlay-/Modal-Semantik, modale Eingabesperre,
Damage-Regionen, deterministisches Acrylic-Noise sowie Glass-, Acrylic-,
Vollton- und Safe-Fallbacks. `boot-ui-runtime-check` prüft diese Invarianten
außerhalb der Firmware; `test-uefi` weist die Integration in QEMU nach.

## Ergänzte Resource-, Unicode- und Icon-Spezifikationen

Die nachträglich befüllten NPSPECs `NPSPEC-BOOTRESOURCE-0001`,
`NPSPEC-BOOTTEXT-0003` und `NPSPEC-BOOTDESIGN-0009` sind neu abgeglichen.
Der UEFI-Pfad besitzt nun eine zentrale feste Resource Registry mit stabilen
URI-IDs, CRC32-Validierung vor jeder Nutzung, Lazy Load, Cache-Hits,
Referenzzählung, Release, Fallbackfeldern und Diagnosedaten.

Der UTF-8-Decoder validiert ein- bis vierbyte Sequenzen, weist Overlong-
Kodierungen, Surrogate und ungültige Codepoints sicher ab und verwendet
U+FFFD/Missing-Glyph-Fallback. Die vorhandenen Segoe-UI-Glyphen und alle sechs
Menüicons werden aus denselben 2-Bit-AA-Quelldaten wie im BIOS-Pfad generiert,
zentral registriert und ausschließlich über semantische Icon-Tokens genutzt.
Internationale Glyphatlanten und ein allgemeiner SVG-Renderer fehlen noch.

## Gemeinsame Eingabelaufzeit

`input.c` stellt nun eine feste O(1)-Ringqueue mit monotonen Sequenzen und
einheitlichen Keyboard-, Pointer-, Touch-, Controller-, Navigations- und
Systemevents bereit. Implementiert und hostseitig getestet sind
Geräte-Hot-Detection, sichere Eventmarkierung, Pointer Capture, Rückkehr zum
Hit-Testing, explizite Fokusnachbarn, Shortcut-Konfliktprüfung und Commands
sowie konfiguriertes Input Repeat. Die UEFI-Tastatur speist dieselbe Queue.

`test-uefi-input` wartet in QEMU auf die vollständig initialisierte
Motion-/Input-Runtime, bricht den Countdown per Tastendruck ab, navigiert über
zwei Queue-Events zu „Einstellungen“ und weist die ausgeführte Aktion mit
`UEFI:SETTINGS` nach. Der UEFI-Simple-Pointer-Adapter ist ebenfalls angebunden:
relative Bewegung wird in den zentralen Cursor überführt, Hit-Testing setzt
Fokus und Auswahl, die linke Taste löst genau eine Control-Aktion aus und jede
Pointer-Aktivität beendet den Countdown. QEMU weist die Protokollerkennung mit
`UEFI:POINTER-READY` nach. Die synthetische HMP-Mausinjektion erreicht das
Simple-Pointer-Protokoll im headless QEMU-Pfad derzeit nicht; ein echter
Klick-End-to-End-Test bleibt deshalb offen. Touch fehlt weiterhin.

## Diagnose und Performance

Die gemeinsame Runtime besitzt einen strukturierten, heapfreien
256-Ereignis-Ringpuffer mit Schweregraden und deterministischem Überschreiben.
Rendering-, Input-, Motion-, Resource-, Icon- und Compositorzähler werden in
einem zentralen Snapshot zusammengeführt. Frame-, Layout-, Animation-,
Render- und Compositorzeiten, Budgetverletzungen sowie adaptive
Qualitätswechsel sind über feste Statusstrukturen abrufbar.

Das Low-End-Profil reserviert maximal 32 MiB; die drei maximalen
1280×1024-Surfacebereiche bleiben darunter. Safe Quality schaltet Motion auf
Reduced Mode und Materialien auf den opaken Fallback, ohne Layout oder
Bedienlogik zu verändern. `UEFI:DIAGNOSTICS-READY` belegt die Initialisierung
im QEMU-Pfad. Persistenter Export und eine echte GPU-/High-End-Erkennung sind
noch nicht vorhanden.

## UEFI-Unteransichten und Sitzungszustand

Der UEFI-Bootmanager besitzt nun eine feste, heapfreie View-Zustandsmaschine
für Hauptauswahl, Einstellungen, Boot-Diagnose, Recovery, Energieoptionen und
kontextbezogene Hilfe. Alle Ansichten verwenden dieselben Controls, Icons,
UTF-8-Texte, Fokusregeln, Pointer-Hit-Tests und Compositor-Layer. Escape führt
deterministisch zur Hauptauswahl zurück; F1 öffnet die lokale Hilfe.

Der Countdown wird in der Statuszeile tatsächlich von fünf bis eins
aktualisiert. Jede Tastatur- oder Pointer-Aktivität entfernt ihn vollständig.
Nicht implementierte oder ohne geprüftes Backend unsichere Aktionen zeigen
ihren verständlichen Hinweis an derselben Stelle, statt still zu scheitern.
`test-uefi-input` durchläuft in QEMU Installation-Nicht-verfügbar,
Einstellungen, Diagnose, Recovery, Energieansicht, Rücknavigation und F1-Hilfe.

Das NovaOS-Logo wird aus exakt derselben geprüften 2-Bit-AA-Quelle wie im
BIOS-Pfad in einen C-Header überführt, als Image-Ressource registriert und auf
jeder UEFI-Ansicht in Nova-Blau gezeichnet. Im Bootmanager wird die Bildmarke
horizontal gespiegelt, sodass der Tierkopf spezifikationsgemäß nach rechts
schaut; die Fehlerdarstellung bleibt davon unabhängig.

Der Energiepfad nutzt UEFI `ResetSystem` für Herunterfahren und Kaltstart.
Keine Energieaktion läuft beim ersten Enter: Zunächst erscheint eine explizite
Bestätigung, erst ein zweites Enter auf derselben Auswahl darf sie ausführen.
Recovery-, Firmware- und Netzwerk-Neustart bleiben ohne verifiziertes Ziel
sicher gesperrt und werden verständlich als nicht verfügbar erklärt.

## Theme- und Accessibility-Runtime

Der UEFI-Pfad besitzt drei vollständige, statische Theme-Tabellen für Dark,
Light und High Contrast. Hintergrund, Flächen, Primär-/Sekundärtext, Akzent,
Fokus, Auswahl, Rahmen, Disabled- und Statusfarben sowie Radius-, Abstands-,
Transparenz- und Accessibilitywerte werden ausschließlich über semantische
O(1)-Tokens aufgelöst. Die Tabellen werden vor Aktivierung auf Vollständigkeit
und Mindesthelligkeitskontrast geprüft und als Theme-Ressourcen registriert.

„Darstellung“ in den Einstellungen wechselt unmittelbar und ohne neue
Controls zwischen allen drei Themes. High Contrast erzwingt opake Materialien,
Reduced Motion, einen dauerhaft sichtbaren gelben Fokus und einen weißen
Kartenrahmen. Der separate Accessibility-Eintrag schaltet Reduced Motion auch
in Dark und Light. `test-uefi-themes` aktiviert alle drei Varianten in QEMU,
prüft die Debugmarker und erzeugt visuelle Light-/High-Contrast-Referenzframes.

## Responsive DLU-Layoutlaufzeit

Der UEFI-Bootmanager verwendet jetzt eine zentrale, heapfreie Layoutlaufzeit
anstelle fester Pixelkoordinaten. DLU werden mit definierter Ganzzahlrundung
in Pixel umgerechnet; Safe Area, Compact-/Standard-/Wide-Breakpoints,
Min-/Preferred-/Max-Constraints sowie Measure-, Arrange- und Finalize-Zähler
sind implementiert. Panel, Kopfbereich, Branding, Liste, Eintragshöhen,
Abstände, Icons, Statuszeile und skalierter Text beziehen ihre Bounds aus
demselben berechneten Layout.

Hosttests prüfen die in den Layout-NPSPECs genannten Auflösungen von
640×480 bis 3840×2160. `test-uefi-resolutions` schaltet QEMUs GOP tatsächlich
auf 800×600, 1280×720 und 1920×1080, wartet auf einen vollständig
komponierten Countdown-Frame und erzeugt visuelle Nachweise. Der aktuelle
Compositor besitzt bewusst nur Speicher für maximal 1920×1080; ein echter
4K-Firmwareframe benötigt deshalb noch einen gekachelten oder direkten
Framebuffer-Backendpfad. Physische Displaymaße liefert GOP nicht, daher wird
die DPI-Diagnose deterministisch mit 96 DPI als unbekannter Firmware-Fallback
geführt.

## Modale Dialoge und Navigation

Der UEFI-Pfad besitzt jetzt einen festen Navigation-Stack mit Push, Pop,
Replace und Recovery-Transitionstypen. Jeder Eintrag bewahrt Seite, Auswahl,
Fokus-ID, Scrollposition und Kontext. Escape schließt zuerst den obersten
Dialog; andernfalls wird der vorherige Navigationseintrag in O(1)
wiederhergestellt. Ein Back auf der Root-Seite erzeugt keinen ungültigen Pop.

Die neue Dialoglaufzeit verwendet einen festen Pool und einen separaten Stack.
Sie bildet den Lifecycle von Created bis Destroyed ab, erlaubt höchstens einen
modalen Dialog, fängt Tastatur und Pointer, zeichnet einen Alpha-Backdrop und
zentriert die abgerundete Dialogkarte in der Safe Area. Information, Warning,
Error, Confirmation, Progress, Credential und Recovery sind typisiert;
Dialogtexte werden UTF-8-sicher auf mehrere Zeilen umgebrochen. Ergebnisse
werden genau einmal als OK, Cancel, Yes, No, Retry, Ignore oder Abort geliefert.

Installation-, Recovery-, Firmware- und nicht verfügbare Neustartpfade zeigen
dadurch echte modale Hinweise statt bloßer Statuszeilen. Herunterfahren und
Neustarten verwenden einen Bestätigungsdialog; die zerstörerische Schaltfläche
muss nach ihrer Auswahl ein zweites Mal bestätigt werden. `test-uefi-dialog`
prüft Darstellung und Escape-Abbruch, `test-uefi-input` vier Stack-
Wiederherstellungen und `test-uefi-power` den vollständigen bestätigten
Shutdown bis `ResetSystem`.

Noch offen sind das Credential-Authentifizierungsbackend, ein allgemeiner
Page/View-Baum und die zeitliche Kopplung der Dialog-Enter-/Exit-Motion.

Der Progress-Teil ist inzwischen sichtbar angebunden: Die read-only
Boot-Diagnose durchläuft Initialisierung, Firmware-, Grafik- und Eingabephase,
zeigt bestimmten und unbestimmten Fortschritt, ganzzahlige Prozentwerte,
Statuswechsel sowie einen expliziten Abschlusszustand. Ungültige Werte werden
auf 0 bis 1000 begrenzt. `test-uefi-progress` weist den vollständigen Frame,
100 Prozent und das anschließende Schließen in QEMU nach.

Auch das sichere Credential-Control ist als inaktives Grundmodul vorhanden:
ein statischer 128-Byte-Puffer nimmt validierte Unicode-Codepoints als UTF-8
auf, zeigt ausschließlich Maskierungspunkte, löscht jeweils einen vollständigen
Codepoint und wird beim Schließen über volatile Schreibzugriffe vollständig
genullt. Es wird absichtlich noch von keinem normalen Menüpfad geöffnet, weil
kein verschlüsseltes Volume und kein Crypto-/TPM-Authentifizierungsdienst
erkannt werden. Damit fragt NovaOS keine Geheimnisse ohne legitimen Verbraucher
ab. Cursor/Selection, Fehlversuchspolitik und das Authentifizierungsbackend
bleiben offen.

## Sichtbare Navigationstransitionen

Jeder UEFI-Seitenwechsel durchläuft nun Exit, atomare Stackänderung und
Enter. Vorwärtsnavigation verwendet einen 32-DLU-Slide nach links mit Fade,
Back ist exakt nach rechts gespiegelt. Replace verwendet Cross-Fade ohne
Richtungsinformation; Recovery einen bewusst ruhigeren 240-ms-Fade. Der
Hintergrund und die Seitenkarte bleiben stabil, während der aktive
Menüinhalt über Source-/Destination-Clipping des Compositors bewegt wird.

Während der Transition ist die Eingabe gesperrt und die neue Auswahl bleibt
bis zum abgeschlossenen Enter ohne Fokusmarkierung. Reduced Motion entfernt
den DLU-Versatz vollständig und behält nur den Fade. Wegen des reinen
Software-Compositors werden deterministische Start-/Mitte-/Ende-Keyframes
verwendet; damit bleibt die Transition auch bei Full HD innerhalb eines
vertretbaren Bootzeitbudgets. Hosttests prüfen alle Transitiontypen,
Spiegelung, Cancel und Fokusfreigabe. `test-uefi-input` weist die tatsächlich
transformierten Push-/Pop-/Recovery-Frames nach, `test-uefi-themes` den
Reduced-Motion-Fallback. Die BIOS-Anbindung bleibt offen.

## Page- und View-Modell

Die UEFI-Oberfläche basiert jetzt auf festen Pools für 16 Pages und 64 Views.
Jede der sechs Bootmanager-Ansichten besitzt genau eine Page mit eindeutigem
Root sowie Header-, Content- und Footer-View. Views tragen Besitzer, Parent,
Kinder- und Geschwisterbeziehungen, Layoutbox, Sichtbarkeit, Fokusreihenfolge,
Accessibility-Rolle und -Namen. Der Aufbau wird iterativ auf fehlende Roots,
doppelte IDs, fremde Besitzer, Zyklen und ungültige Lebenszyklen geprüft.

Pages durchlaufen Created, Loaded, Active, Suspended und Closed; Views
Created, Initialized, Visible, Active, Inactive und Destroyed. Es kann genau
eine Page aktiv sein. Ereignisse werden ohne Rekursion von Root zu Child-Views
weitergereicht. Modale Dialoge erhalten jeweils eine eigene temporäre Page;
beim Schließen werden alle zugehörigen Views zerstört und die suspendierte
Menü-Page samt Fokus wieder aktiviert. Layout-Safe-Area und Content-Panel
werden bei jedem Frame in die aktiven View-Layoutboxen geschrieben.

Der Navigation-Stack entspricht nun der spezifizierten Standardgröße von 64
Einträgen und unterstützt Push, Pop, Replace sowie Reset auf Root in O(1).
Hosttests decken Baumvalidierung, Lifecycle, Eventdurchlauf, Modal-Page,
Fokusrestore und Reset ab. QEMU weist `PAGES-READY`, Page-Aktivierungen,
Dialog-Page-Aktivierung und die Wiederherstellung nach Escape nach. Die
gemeinsame BIOS-Anbindung bleibt offen.

## Typisierte Standard-Controls

Die gemeinsame Control-Runtime unterscheidet nun neben dem Lebenszyklus auch
Visible, Enabled, Focused, Hovered, Pressed, Selected, Checked,
Indeterminate, Busy, Error, Read-only, Locked und Expanded. Wertebereiche,
Schrittweiten und Änderungen werden begrenzt und diagnostisch erfasst;
gesperrte, beschäftigte oder nur lesbare Controls lösen keine Aktion aus.

Checkbox, Switch, Slider, Scrollbar, Progress und Separator besitzen eigene
Renderer statt der bisherigen generischen Rechteckdarstellung. Die
UEFI-Einstellungsseite verwendet davon sichtbar einen Theme-Status, einen
echten Reduced-Motion-Switch sowie Tastatur- und Sprach-Badges. Der Switch
folgt unmittelbar dem Laufzeitzustand und allen drei Themes. Hosttests decken
Toggle, Sperre, Wertebereich, Schrittweite, Clamp und Diagnostik ab;
`test-uefi-themes` weist den Zustandswechsel und die Darstellung in QEMU nach.

Das Context Menu ist inzwischen ebenfalls produktiv angebunden. F2 oder ein
Rechtsklick öffnet genau ein Menü am aktuellen Ziel-Control; die Position wird
in die Safe Area geklemmt. Pfeiltasten, Enter, Escape, Pointer-Hover,
Pointer-Aktivierung und ein Klick außerhalb sind definiert. „Details“ und
„Erweiterte Optionen“ führen in sichere, modale Informationspfade. Der neue
QEMU-Test `test-uefi-context` prüft den vollständigen F2-Pfad, den sichtbaren
Frame, die Fokusnavigation und die ausgeführte Aktion.

Noch offen sind insbesondere produktive TextField-/Slider-Flows,
Pointer-Drag, Context-Untermenüs, anklickbare Breadcrumb-Eltern sowie die
gemeinsame BIOS-Anbindung. Diese NPSPECs bleiben daher korrekt als teilweise
integriert markiert.

TextField und Password Field besitzen nun zusätzlich einen festen, validierten
UTF-8-Puffer mit Maximalgrenze, Caret und Selektion. Einfügen, Backspace,
Delete und Cursorbewegung achten auf Codepoint-Grenzen. Standard-, Zahlen-,
Hex-, Passwort-, Dateiname- und Suchmodus filtern vor der Übernahme;
Read-only, Locked und Busy verhindern Änderungen. Der Renderer zeichnet
Placeholder, Auswahl, Caret, Fokus und Fehlerzustand. Passwortzeichen werden
niemals als Klartext gezeichnet und der gesamte Puffer wird über volatile
Schreibzugriffe gelöscht. Hosttests prüfen mehrbyte UTF-8, Filter,
Selektion, ungültige Sequenzen, Maskierung und Löschung.

Ein einzelner zentraler Tooltip erscheint nach 750 ms ruhendem Fokus und wird
bei jeder Eingabe, bei Dialogen oder beim Context Menu sofort ausgeblendet.
Er fängt weder Fokus noch Eingaben und bleibt innerhalb der Safe Area. Auf
Unterseiten ersetzt ein semantischer Breadcrumb den einfachen Titel durch
„Start“, ein gezeichnetes Chevron-Icon und den aktuellen Seitennamen. Damit
ist kein nicht vorhandenes Fontzeichen nötig. `test-uefi-tooltip-breadcrumb`
prüft beide Zustände und erzeugt getrennte visuelle Referenzframes.

## Architekturarbeit mit noch offener Integration

- BIOS Stage 2 verwendet wegen des nahezu ausgeschöpften Real-Mode-Abbilds
  weiterhin seinen direkten Renderer. Die gemeinsame C-Runtime ist dort noch
  nicht angebunden; deshalb sind die Motion-/Compositor-NPSPECs trotz
  vollständigem UEFI-Grundpfad nur teilweise integriert.
- Der aktuelle Glass-Pfad besitzt Tint, Transparenz und sicheren Vollton-
  Fallback, aber noch keinen separaten, gekachelten Backdrop-Blur-Cache.
- Font-, Logo- und Icondaten sind noch statisch in Stage 2 eingebettet. Der
  BAP-Ressourcenlader ist erforderlich, bevor weitere größere GUI-Module
  integriert werden: Von 32768 Stage-2-Bytes sind aktuell nur rund 790 Bytes
  Padding frei und der Real-Mode-Adressraum endet bei 0x10000.
- Der UEFI-Bootmanager startet als relocatable PE32+-Anwendung, initialisiert
  GOP, zeichnet die Nova-Menüoberfläche, verwendet Text-/Icon-Ressourcen,
  Tastatur und Simple Pointer und führt den Fünf-Sekunden-Standardstart aus.
  Funktionsgleichheit mit allen BIOS-Unteransichten, Kernel-Lader und
  NBHP/BIB-Übergabe fehlt noch.

Diese Punkte sind keine Freigabe für vereinfachte oder erfundene Backends.
Die Oberfläche muss bis zur Bereitstellung der jeweiligen Abhängigkeit einen
eindeutigen, sicheren und verständlichen Nicht-verfügbar-Zustand anzeigen.
