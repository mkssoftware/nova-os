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
`test-theme`, `test-ui-flows`, `test-recovery`, `test-platform`,
`test-uefi`, `test-uefi-settings-controls`, `test-uefi-firmware`,
`test-uefi-scrollview` und `test-uefi-image`.
Der UEFI-Test weist EFI-Einstieg, GOP-Initialisierung,
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
| Firmware-Setup | Legacy BIOS besitzt keinen standardisierten Setup-Reboot; UEFI-Unterstützung hängt von `OsIndicationsSupported.BootToFwUi` ab | UEFI bietet den bestätigten Setup-Neustart nur bei gemeldeter Unterstützung an; sonst bleibt die Aktion deaktiviert und erklärt |
| Secure Boot | Legacy-BIOS-Pfad besitzt keine UEFI Secure-Boot-Datenbanken | Status wird als nicht verfügbar angezeigt |

## UEFI-Firmware-Einstellungen

Die eigene Firmware-Seite liest Firmwarehersteller und Revision aus der
System Table sowie `SecureBoot`, `SetupMode` und `OsIndicationsSupported` über
typisierte Runtime Services. Fehlende Variablen werden ausdrücklich als nicht
verfügbar dargestellt. Die Seite nimmt keine Firmwareparameteränderungen vor.
Nur wenn `BootToFwUi` unterstützt wird, erscheint der Setup-Neustart aktiv;
nach dem zweistufigen Bestätigungsdialog setzt NovaOS ausschließlich das
standardisierte `OsIndications`-Bit und löst einen kalten Reset aus.
`test-uefi-firmware` weist mit OVMF Statusseite, Hersteller, Capability,
Bestätigung, gesetzte Anforderung und Resetpfad nach.

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
U+FFFD/Missing-Glyph-Fallback. Die vorhandenen Segoe-UI-Glyphen und die sechs
gemeinsamen BIOS-/UEFI-Menüicons werden aus denselben 2-Bit-AA-Quelldaten
generiert. Der UEFI-Pfad ergänzt Back, Forward, Restart, Warning, Error,
Information, Success, Lock und Search als statisch erzeugte, geglättete
Ressourcen. Damit besitzen alle 15 semantischen Icon-Tokens eine eigene zentral
registrierte Ressource; Unterseiten verwenden eigene Tokenzuordnungen statt
des früheren Home-Fallbacks. Internationale Glyphatlanten, ein allgemeiner
SVG-Renderer und die BIOS-Parität der neun erweiterten Symbole fehlen noch.

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

Enter und Exit sind inzwischen zeitlich an die UEFI-Dialoginstanz gekoppelt:
der Standardpfad blendet die Karte in 180 ms ein und skaliert sie von 95 auf
100 Prozent; Reduced Motion verwendet einen 150-ms-Fade ohne Skalierung. Der
finale Frame wird vor der Eingabefreigabe vollstÃ¤ndig gezeichnet, Escape spielt
die Exit-Motion vor dem Abbruch ab und der QEMU-Test wartet auf einen explizit
stabilen Frame. Der modale Page/View-Baum suspendiert und restauriert dabei die
vorherige Seite samt Fokus. Noch offen sind das Credential-
Authentifizierungsbackend, nichtmodale Dialoge, Touch und BIOS-ParitÃ¤t.

Der Progress-Teil ist inzwischen sichtbar angebunden: Die read-only
Boot-Diagnose durchläuft Initialisierung, Firmware-, Grafik- und Eingabephase,
zeigt bestimmten und unbestimmten Fortschritt, ganzzahlige Prozentwerte,
Statuswechsel sowie einen expliziten Abschlusszustand. Ungültige Werte werden
auf 0 bis 1000 begrenzt. `test-uefi-progress` weist den vollständigen Frame,
100 Prozent und das anschließende Schließen in QEMU nach.

Die unbestimmte Initialisierungsphase verwendet zusätzlich einen echten
heapfreien Activity Indicator statt einer scheinbar numerischen Teilfüllung.
Die Control-Laufzeit unterstützt Spinner-, Ring-, Dots- und Arc-Templates,
validierte Start-/Stop-Zustände und eine begrenzte Phase von 0 bis 1000.
Hosttests prüfen alle vier Stile und ungültige Werte; der QEMU-Test hält den
ersten Diagnoseframe gezielt an und erzeugt `build/uefi-activity.ppm` als
visuellen Nachweis. Eine kontinuierliche Kopplung an die Motion-Timeline und
die gemeinsame BIOS-Anbindung fehlen noch.

Die Dialogaktionen sind nun echte Standard-Button-Controls statt manuell
gezeichneter Flächen. Standard, Primary, Secondary, Destructive, Icon, Text
und Toggle sind validierte statische Typen; Text und optionales Icon werden im
Control gerendert. Pressed, Toggle, Busy, Fokus, Theme und exakt eine Aktion je
vollständiger Aktivierung werden im Hosttest geprüft. Die produktiven
Dialogpfade verwenden Primary, Secondary und Destructive und bleiben durch die
vorhandenen QEMU-Dialog- und Power-Tests abgedeckt.

Unter Einstellungen ist die Darstellungsauswahl jetzt ein echter Menu Button.
Er ist fest an genau ein statisches Menü gebunden, zeigt einen eindeutigen
Pfeil und verwaltet Collapsed/Expanded ohne Heap-Allokation. Das Overlay wird
unterhalb des Buttons linksbündig und innerhalb der Safe Area platziert. Maus,
Enter, Space, Pfeile, Home, End, Escape und Links sind angebunden; nach Auswahl
oder Abbruch kehrt der Fokus zum Menu Button zurück. Dark, Light und Hoher
Kontrast werden erst durch die Menü-Auswahl aktiviert. `test-uefi-themes`
belegt Öffnung, drei Aktionen und stabile Framebuffer-Ausgaben; der offene
Zustand wird als `build/uefi-menu-button.ppm` festgehalten.

Label, Icon, Image und Separator sind jetzt eigenstÃ¤ndige typisierte Controls.
Labels besitzen neun Darstellungsrollen, kombinierte horizontale und vertikale
Ausrichtung, Wrap/Ellipsis, dynamischen UTF-8-Text sowie DLU-/DPI-Skalierung.
Die NovaOS-Wortmarke und die dynamische Statuszeile verwenden sie produktiv.
Alle sechs sichtbaren Eintrags- und Tile-Symbole werden über validierte
Icon-Controls aus der zentralen Tokenbibliothek gerendert. Das NovaOS-Logo ist
ein Image-Control mit Original, Stretch, Fit, Fill und Center, proportionalem
Clipping, Alpha-Coverage und Theme-Tint. Ein nicht interaktiver horizontaler
Separator gliedert die allgemeinen und Firmware-Einstellungen; vertikale und
beschriftete Varianten sind ebenfalls implementiert und hostgetestet.
`test-uefi-settings-controls` belegt alle vier produktiven Renderer nach dem
Compositor-Commit; `build/uefi-basic-controls.png` wurde visuell geprüft.
Weitere Bildformate bleiben bis zur normativen Definition von BAP und
Resource-Index bewusst blockiert.

List und ListItem besitzen nun eine feste indexierbare Collection mit bis zu
64 EintrÃ¤gen je Liste innerhalb des zentralen 128-Control-Pools. EinfÃ¼gen und
Entfernen bewahren die Reihenfolge; None-, Single- und Multi-Auswahl sowie die
direkte Auswahl laufen ohne Heap-Allokation, die direkte Indexauswahl in O(1).
Ein explizites sichtbares Fenster virtualisiert die DiagnoseeintrÃ¤ge zusammen
mit der vorhandenen ScrollView. ListItems besitzen typisierte ZustÃ¤nde,
Subtitle und Status sowie den bestehenden Theme-, Icon-, Label-, Fokus- und
Accessibility-Pfad. Pfeile, Home/End, Page Up/Down, Enter, Space, Maus und
KontextmenÃ¼ arbeiten auf denselben produktiven Controls. Der neue
`test-uefi-list-controls` prÃ¼ft die zusÃ¤tzlichen Navigationstasten und die
Space-Aktivierung bis zum stabilen Dialogframe; der Frame liegt als
`build/uefi-list-controls.png` vor.

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
echten Reduced-Motion-Switch, eine unabhängige Tooltip-Checkbox und einen
diskreten Tooltip-Verzögerungs-Slider. Checkbox und Slider verändern das
laufende Tooltip-System unmittelbar; der Slider bleibt zwischen 250 und
1500 ms, rastet in 250-ms-Schritten und unterstützt Links/Rechts/Home/End,
Enter/Space sowie Pointer-Drag. Alle Controls folgen dem Laufzeitzustand und
allen drei Themes. Hosttests decken Toggle, Sperre, Wertebereich, Schrittweite,
Clamp und Diagnostik ab; `test-uefi-settings-controls` prüft Checkbox,
Slidernavigation, Grenzwerte und den stabilen visuellen Frame in QEMU.

Das Context Menu ist inzwischen ebenfalls produktiv angebunden. F2 oder ein
Rechtsklick öffnet genau ein Menü am aktuellen Ziel-Control; die Position wird
in die Safe Area geklemmt. Pfeiltasten, Enter, Escape, Pointer-Hover,
Pointer-Aktivierung und ein Klick außerhalb sind definiert. „Details“ und
„Erweiterte Optionen“ führen in sichere, modale Informationspfade. Der neue
QEMU-Test `test-uefi-context` prüft den vollständigen F2-Pfad, den sichtbaren
Frame, die Fokusnavigation und die ausgeführte Aktion.

ScrollView und Scrollbar sind nicht mehr bloße Typdeklarationen. Die gemeinsame
Control-Laufzeit verwaltet Viewport und Contentgröße, X/Y-Position, Clamp und
Scroll-Into-View in O(1). Horizontale und vertikale Scrollbars dürfen nur fest
an eine ScrollView gebunden werden; ihr Thumb erhält eine proportionale Größe
mit Mindestmaß. Die Diagnoseansicht zeigt produktiv vier von sechs vollständigen
Zeilen, scrollt den Fokus automatisch ein und unterstützt Pfeile, Page Up/Down,
Home/End, Mausrad, Track-Sprung und Pointer-Drag. `test-uefi-scrollview` weist
Page Down, End/Home und den komponierten Frame `build/uefi-scrollview.ppm` nach.

Auch die große Inhaltsfläche ist nun ein echtes Card-Container-Control statt
eines direkt im UI-Code gezeichneten Panels. Unterstützt werden Standard,
Information, Warning, Error, Success, Interactive und Custom mit validiertem
Typwechsel, statischer Child-Gruppierung, Rekursionsschutz, Theme-Rahmen,
Radien und semantischen Akzenten. Glass- und Acrylic-Tints werden nicht mehr im
Compositor fest codiert, sondern aus den aktiven Theme-Tokens gesetzt. Die
produktive Seitenkarte bleibt innerhalb des 32-MiB-Low-End-Budgets auf der
stabilen Basissurface; eine separate Materialebene benötigt künftig gekachelte
Surfaces und wird ausdrücklich nicht durch eine dritte Full-HD-Surface erkauft.
Hosttests prüfen alle sieben Typen, die QEMU-Theme-Suite Dark, Light und den
opaken High-Contrast-Fallback.

Die Recovery-Seite verwendet nun produktive Boot Option Tiles statt einer
zweiten gewöhnlichen Liste. Sechs Kacheln bilden Reparatur, Integritätsprüfung,
Snapshot, Speicherdiagnose, Datenträgerdiagnose und Zurück jeweils mit zentralem
Icon, Titel, zweizeiliger Beschreibung und verständlichem Status ab. Standard,
Primary, Recovery, Maintenance, Diagnostic, Destructive und Custom sind als
validierte statische Templates vorhanden. Das responsive 2x3-Grid besitzt
semantische Typmarkierungen sowie einen davon klar getrennten vollständigen
Fokusrahmen. Maus, Enter, Space und zweidimensionale Pfeilnavigation arbeiten
auf denselben Tile-Aktionen; unsichere Recovery-Aktionen bleiben hinter dem
bestehenden modalen Hinweis gesperrt. `test-uefi-recovery-tiles` prüft beide
Gridachsen, Space-Aktivierung und `build/uefi-recovery-tiles.ppm` in QEMU.

Der klickbare Breadcrumb-Elternknoten „Start“ ist jetzt zugleich ein echter
Icon Button mit zentralem Home-Token. Das spezialisierte Control validiert
Icon-ID, nichtleeren Tooltip und Aktion und besitzt einen verpflichtenden
Accessibility-Namen. Icon und Text übernehmen Themefarben; Hover und Fokus
verwenden die Card-/Focus-Tokens. Pointer sowie Tab mit Enter oder Space lösen
weiterhin ausschließlich den zentralen Root-Reset aus. Der vorhandene
Breadcrumb-QEMU-Test prüft Fokusframe, Aktivierung und `UEFI:ICONBUTTON-FRAME-READY`.
Eine bevorzugte allgemeine SVG-Quelle, Touch/Controller, Icon-Button-Motion und
die gemeinsame BIOS-Anbindung fehlen noch.

Der aktuelle UEFI-Entwicklungsstand wird zusätzlich als eigenständig bootbares
64-MiB-GPT-Abbild mit FAT32-EFI-Systempartition und
`EFI/BOOT/BOOTX64.EFI` erzeugt. `test-uefi-image` startet genau
`build/nova-uefi.img` anstelle des QEMU-Verzeichnisbackends und wartet auf den
vollständig gezeichneten Countdown-Frame. Damit ist das ausgegebene Image ein
reproduzierbarer Nachweis des aktuellen UEFI-GUI-Stands.

Der Breadcrumb zeigt auf jeder Unterseite den Pfad aus Start, Chevron und
aktueller Seite. Der Elternknoten Start besitzt eigene themebasierte Hover- und
Fokuszustände, präzises Pointer-Hit-Testing sowie Tab/Enter/Space-Bedienung.
Seine Aktivierung wird an den zentralen Navigation-Controller gemeldet, der
den Stack validiert in O(1) auf Root zurücksetzt und Hauptauswahl, Fokus und
Statuszeile wiederherstellt. Der aktuelle Seiteneintrag bleibt passiv.

Noch offen sind insbesondere Context-Untermenüs, tiefere Breadcrumb-Pfade,
Pointer-Textselektion sowie die
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

Das TextField ist nun außerdem produktiv in die UEFI-Hilfeseite eingebunden.
F1 öffnet einen lokalen Index mit acht verständlichen Hilfethemen. Das
Suchfeld filtert Titel und Schlüsselwörter unmittelbar; Backspace, Delete,
Links/Rechts, Home/End, Enter, Escape und Pointer-Fokus sind angebunden.
Leere Treffer werden ausgeblendet und von Fokus- sowie Mausnavigation
übersprungen. Ein Treffer öffnet einen kontextbezogenen Detaildialog, ohne
interne oder vertrauliche Diagnosedaten zu zeigen. `test-uefi-help-search`
prüft Bearbeitung, Filterung, stabilen Frame und das Öffnen eines Treffers.

Ein einzelner zentraler Tooltip erscheint nach der konfigurierten Verzögerung
von 250 bis 1500 ms bei ruhendem Fokus und wird
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

## Zentrale Styles, Templates und StatusBadges

Die UEFI-Control-Laufzeit besitzt jetzt zentrale, statische Style- und
Template-Registries. Styles werden per ID in O(1) gebunden, lösen ihre
Vererbung bereits beim Definieren auf und beziehen Hintergrund, Text, Fokus,
Rahmen sowie Information-, Erfolg-, Warn- und Fehlerfarben aus dem aktiven
Theme. Opaque, Glass, Acrylic und Transparent werden als validierte
Materialtypen geführt. Ein Themewechsel bindet alle registrierten Controls
ohne Heap-Allokation neu; Dark, Light und High Contrast sind in QEMU geprüft.

Für jeden Control-Typ wird bei der Initialisierung ein statisches
Standardtemplate mit benannten Parts angelegt. Alternative Templates dürfen
nur an typkompatible Controls gebunden werden und müssen alle Pflicht-Parts
enthalten; Selbstvererbung, fehlende Eltern und inkompatible Bindungen werden
abgewiesen. Die aktuelle Stufe beschreibt und validiert die Struktur zentral,
der Renderer verarbeitet jedoch noch nicht einen vollständig materialisierten
Visual Tree. Diese Grenze sowie die fehlende BIOS-Parität bleiben im Audit
ausdrücklich sichtbar.

Die sechs Recovery-Kacheln zeigen ihren Zustand nun über echte semantische
StatusBadge-Controls. Information, Erfolg, Warnung und Neutral verwenden die
Themefarben, ein optionales Icon und einen lesbaren Textfallback. Die Badges
sind absichtlich nicht fokussierbar und lösen keine Aktion aus. Hosttests
prüfen alle sieben Typen und Fehlerfälle; `test-uefi-recovery-tiles` bestätigt
den produktiven Frame in QEMU.

## Boot-UI Error Recovery

`NPSPEC-BOOTUI-0008` besitzt nun einen zentralen heapfreien Recovery Manager.
Er klassifiziert UI-Fehler als Information, Warnung, wiederherstellbar,
kritisch oder fatal und führt sie deterministisch durch Logging, Retry, lokalen
Fallback, Reinitialisierung, Subsystem-Isolation, Safe Mode oder Textmodus.
Eine feste 32er Historie hält Code, Subsystem, Schweregrad, Maßnahme, Zeitpunkt
und Ergebnis; sechs unabhängige Watchdogs überwachen Rendering, Layout, Input,
Motion, Navigation und Ressourcen ohne Endlosschleifen.

Im Safe Mode werden Motion, Glass, Blur, Schatten und Vector-Filter zentral
deaktiviert und das bestehende sichere Qualitätsprofil aktiviert. Die
Diagnose-Seite stellt mit F10 einen reproduzierbaren Selbsttest bereit und
zeigt anschließend verständlich „UI-Fehler behoben - sicherer
Darstellungsmodus aktiv“. F9 simuliert dort einen fatalen GUI-Ausfall: Der
Bootmanager wechselt in einen funktionalen UEFI-Textfallback und setzt den
Standardstart spätestens nach fünf Sekunden fort. Der QEMU-Test
`test-uefi-ui-recovery` belegt Managerinitialisierung, Safe-Mode-Frame,
Textfallback und `UEFI:TEXT-CONTINUE`.

Noch nicht vollständig angebunden sind Fehlerinjektionen an jedem einzelnen
Renderer-, Layout- und Ressourcenaufruf sowie die identische BIOS-Runtime.
Auch `NPSPEC-BOOTUI-0009` bleibt teilweise: Sichere Runtime-Defaults und
validierte Einstellungen existieren, das geforderte externe Binärformat nennt
aber weder Magic, Header/Feldoffsets und Endianness noch Section-Codes,
Prüfsummenalgorithmus oder Migrationsabbildungen. Bis diese Wire-Definition
vorliegt, wird bewusst kein vermeintlich kompatibles Dateiformat erfunden.

## Boot-UI Memory Model

`NPSPEC-BOOTUI-0007` besitzt nun einen zentralen statischen Memory Manager für
Permanent-, Runtime-, Scene-, Animation-, Frame-, Render-, Cache- und
Diagnoseobjekte. Die normativen Einzelbudgets ergeben exakt die geforderte
64-MiB-Obergrenze; tatsächlich reserviert werden nur kleinere getrennte
Arenen. Allokationen sind deterministische Bump-Operationen und mindestens 16,
für Renderdaten 64 Byte ausgerichtet. Eine feste 256er Metadatentabelle führt
ID, Größe, Owner, Pool, Referenzzähler, Alignment und Zustand.

Pointervalidierung, ungültige Freigaben, Double Free, Alignmentfehler,
Metadaten- und Poolüberlauf werden gezählt. Ein Poolüberlauf schreibt niemals
über seine Arena hinaus, sondern fordert beim Recovery Manager den Safe Mode
an. Der Frame-Pool benötigt keine Einzelfreigaben und wird nach jedem Bild
atomar geleert. Der produktive UEFI-Renderer reserviert seine Frame-Metadaten
über diesen Pfad und validiert sie vor dem Reset.

Auf der Diagnose-Seite führt F8 einen sichtbaren Alignment- und Frame-Reset-
Selbsttest aus. `test-uefi-ui-recovery` wartet auf den vollständig gezeichneten
Statusframe und erzeugt `build/uefi-memory-self-test.ppm`. Die Hosttests decken
zusätzlich Referenzzählung, statische Permanentobjekte, Double Free und einen
erzwungenen Cache-Overflow mit Recovery ab. Noch offen sind die Migration aller
älteren statischen Subsystemspeicher, die Reservierung der Arenen über echte
UEFI-Seiten und die gemeinsame BIOS-Anbindung.

## Boot-UI Runtime Configuration

Der technisch eindeutig definierte Runtime-Anteil von `NPSPEC-BOOTUI-0009`
ist jetzt zentral implementiert. Die feste Konfiguration trägt Version 1.0.0,
Theme, Qualitätsprofil, Skalierung, Motion, Tooltips, Tooltip-Verzögerung,
Recovery-, Watchdog-, Eingabe- und Debugwerte. Sichere Defaults funktionieren
ohne externe Datei; jeder Wert besitzt einen geprüften Bereich und die
Runtime-Struktur eine deterministische interne Integritätsprüfsumme.

Änderungen laufen ausschließlich über Begin, Set und Commit. Erst eine
vollständig gültige Staging-Konfiguration ersetzt atomar den aktuellen Stand;
andernfalls bleibt dieser unverändert und die Transaktion wird als Rollback
gezählt. Explizites Rollback, Default-Reset, Generation, Änderungsmeldungen und
temporäre Overrides sind heapfrei implementiert. Overrides verändern die
Basiswerte nicht und können gemeinsam entfernt werden.

Themeauswahl, Reduced Motion, Tooltip-Schalter und Tooltip-Verzögerung sind an
diesen Manager angebunden. Die bestehenden QEMU-Tests für Settings und Themes
weisen jeden produktiven Commit mit `UEFI:CONFIGURATION-COMMIT` nach und zeigen
den finalen Zustand in `build/uefi-settings-controls.ppm`. Hosttests prüfen
zusätzlich verschachtelte Transaktionen, ungültige Werte, Rollback, Reset,
Override-Isolation und eine beschädigte Prüfsumme.

Nicht umgesetzt ist weiterhin die externe binäre Persistenz. Die Spezifikation
nennt keine Magic/Formatkennung, Headergröße, Feldoffsets, Endianness,
Section-Codes, externen Prüfsummenalgorithmus oder konkrete Migrationstabellen.
Die interne Runtime-Prüfsumme wird ausdrücklich nicht als erfundenes
Dateiformat ausgegeben; dieser Teil bleibt bis zu einer normativen Wire-
Definition blockiert dokumentiert.

## Boot-UI Runtime und Lifecycle

`NPSPEC-BOOTUI-0002` und `NPSPEC-BOOTUI-0003` besitzen jetzt einen zentralen,
heapfreien Runtime-Core. Die Initialisierung erzwingt die spezifizierte
Abhängigkeitsfolge von Memory und Platform bis Input und Renderer. Ungültige
Subsystemreihenfolgen und Lifecycle-Transitionen werden abgewiesen und in den
Diagnosedaten gezählt. Eine feste 32er Scheduler-Queue verarbeitet fällige
Aufgaben deterministisch von Critical bis Idle und verhindert rekursiven
Wiedereintritt.

Der produktive UEFI-Zeichenpfad ist in den Runtime-Frame eingebunden. Jeder
vollständige Frame durchläuft Input, Event Queue, Control Update, Application,
Motion, Layout, Dirty Detection, Render Queue, Rendering, Compositor, Present
und Diagnostics in genau dieser Reihenfolge. Eine ausgelassene oder doppelte
Phase verwirft den Frame sicher. Initialisierungsfehler von GOP oder GUI führen
den Runtime-Zustand vor dem vorhandenen Textfallback in Recovery.

Der Lifecycle umfasst Created, Initializing, Loading Resources, Building Scene,
Layout, Ready, Running, Suspended, Recovery, Shutdown und Destroyed. Suspend und
Recovery sperren Eingaben. Der normale Standardstart durchläuft Shutdown und
Destroyed, bevor die Startaktion fortgesetzt wird. Auf der Diagnose-Seite löst
F6 einen sichtbaren Suspend-/Resume-Selbsttest aus. Der QEMU-Test
`test-uefi-ui-recovery` wartet auf den vollständig präsentierten Statusframe und
erzeugt `build/uefi-runtime-lifecycle.ppm`; der Hosttest prüft zusätzlich
Initialisierungsreihenfolge, Framefehler, Schedulerpriorität, Recovery sowie
Shutdown/Destroy.

Offen bleiben die gemeinsame BIOS-Anbindung, echte Firmware-Suspend-/Resume-
Ereignisse, Touch-/Controllerprovider und hardwarebasierte Performancezeiten.

## Boot-UI State Model

`NPSPEC-BOOTUI-0004` besitzt nun einen gemeinsamen, statischen Zustandskern mit
256 Objektplätzen. Er kennt die spezifizierten Domänen Runtime, Scene, Dialog,
Overlay, Control, Focus, Pointer, Keyboard, Touch, Navigation, Page, Animation,
Render, Resource, Theme und Error. Jede Domäne verwendet eine eigene feste
Übergangsmatrix. Objekt-ID, Owner, Parent, aktueller und vorheriger Zustand,
Zeitstempel, Transitionen und Fehler werden ohne Heap geführt.

Jede Transition validiert Zustand und Elternhierarchie vor der atomaren
Änderung. Anschließend entstehen geordnet OnExit-, OnTransition- und OnEnter-
Ereignisse im 256er Ringlog; optionale Callbacks erhalten dieselbe Reihenfolge.
Checkpoint, Rollback und Reset sind explizit. Ein versionierter interner
Snapshot mit Magic, Größe und FNV-Prüfsumme kann einen Zustand sicher sichern
und wiederherstellen; manipulierte Snapshots werden verworfen. Elternobjekte
können nicht vor aktiven Kindern entfernt werden.

Die Runtime verwendet diesen Kern als autoritative Transitionsebene. Ihre
Framepipeline setzt zusätzlich die synchronisierten Phasen Input, Update,
Animation, Layout, Render, Present und Idle. Der F6-Diagnosetest erzeugt einen
Scene-Control-Baum, prüft Rollback und Snapshot-Restore, baut Kind vor Eltern
ab und zeichnet erst danach den sichtbaren Erfolgsstatus. Hosttests prüfen
zusätzlich verbotene Sprünge, die Control-Folge Focused, Pressed, Released,
Focused, Callbackreihenfolge und Snapshotkorruption. QEMU bestätigt
`UEFI:STATE-MODEL-SELF-TEST-FRAME`.

Die bereits existierenden spezialisierten Dialog-, Page-, Navigation-, Motion-,
Resource- und Input-Automaten sind funktional, aber noch nicht vollständig als
Objekte dieses gemeinsamen Kerns registriert. Diese Migration sowie Touch,
Controller und BIOS-Parität bleiben im Audit ausdrücklich teilweise offen.

## BIOS/UEFI Graphics Abstraction

Der UEFI-Pfad von `NPSPEC-BOOTUI-0005` verwendet jetzt einen gemeinsamen,
heapfreien `GraphicsContext`. Er enthält Auflösung, bytegenauen Pitch,
Farbtiefe, PixelFormat, DPI/Skalierung, Front- und Backbuffer, Capabilities,
Framebufferadresse und -größe sowie Firmware- und Hardwarebeschreibung. Alle
Zeiger, Dimensionen, Pitchwerte und Buffergrenzen werden vor der Aktivierung
validiert. Layout, Controls und Compositor greifen nicht mehr auf GOP-Typen
oder firmwareeigene Pixeloperationen zu.

Der Compositor zeichnet ausschließlich in seinen Offscreen-Buffer. Erst nach
der Runtime-Phase `Present` übergibt er das vollständige Bild an die GAL; ein
Present während Input, Update, Animation, Layout oder Render wird verworfen.
Der sichtbare Framebuffer wird zeilenweise mit dem gelieferten Pitch statt mit
einer angenommenen Breite adressiert. Der Pfad allokiert während Present nicht
und zählt Präsentationen, Bytes, direkte und konvertierte Pixel sowie Fehler.

Das interne 32-Bit-Farbmodell wird in RGBA8888, BGRA8888, RGB888, BGR888,
RGB565 oder beliebige Firmware-Bitmasken umgesetzt. Das GOP-Backend übernimmt
RGB-, BGR- und Bitmask-Modi und weist BltOnly sicher ab. Der Standardstart
beendet nach Runtime und State Model auch den GraphicsContext explizit.
Hosttests prüfen Kanalreihenfolge, RGB565/Bitmask, einen Pitch größer als die
Bildbreite, zu kleine Buffer und die Present-Phasensperre. QEMU bestätigt den
echten GPT/FAT32-Image-Boot, Recovery sowie 800x600, 1280x720 und 1920x1080;
`build/uefi-gal-1280x720.png` dokumentiert den sichtbaren Frame.

Noch nicht vereinheitlicht ist der platzkritische BIOS/VBE-Stage-2-Renderer.
Ebenso fehlen die Auswahl unter mehreren GOP-Handles, echtes Triple Buffering,
QHD/4K oberhalb der derzeitigen statischen 1920x1080-Compositorfläche,
physische DPI-Metadaten und gemessene Firmware-Zeitbudgets. Diese Punkte sind
im Audit als teilweise offen ausgewiesen.

## Resolution and Scaling System

`NPSPEC-BOOTUI-0006` besitzt jetzt einen zentralen, gecachten DLU-Scaling-
Kontext. Er speichert physische Auflösung, logischen Viewport, DPI und deren
Verlässlichkeit, automatischen und erzwungenen Skalierungswert, vierseitige
Safe Area, Auflösungsklasse und Seitenverhältnis. Die Spezifikationsreferenz
1920x1080 entspricht nun exakt 100 Prozent; 3840x2160 ergibt ohne verlässliche
DPI 200 Prozent und damit wieder 1920x1080 logische DLU.

Die Runtime unterscheidet XS, S, M, L, XL, XXL und Ultra sowie 4:3, 5:4, 16:9,
16:10, 21:9, 32:9 und Custom. Bei verlässlicher Firmware-DPI wird DPI/96 auf
den nächsten 25-Prozent-Schritt quantisiert. Ohne verlässliche DPI verwendet
sie die konservative Relation zur Full-HD-Referenz. Automatik ist der sichere
Default; die Konfiguration akzeptiert zusätzlich erzwungene 100 bis 300
Prozent. Ungültige Auflösung oder Skalierung wird abgewiesen, ungültige DPI
fällt auf 96 DPI mit Auflösungsheuristik zurück.

DLU-zu-Pixel und Pixel-zu-DLU runden deterministisch auf ganze Pixel. Layout,
Text, Icons, Abstände, Radien, Dialoge und Motion-Distanzen verwenden denselben
Wert. Die Safe Area beträgt pro Seite drei Prozent mit einer Mindestgrenze für
kleine Modi. Eine Neuberechnung erfolgt nur bei Start, Auflösungs-, DPI- oder
Scalewechsel; wiederholte Frames lesen den vorberechneten Kontext.

Hosttests prüfen 640x480 bis 3840x2160, QHD, UHD, 21:9, 144 DPI, 175-Prozent-
Override, Rückkehr zur Automatik, ungültige Werte und Pixelalignment. QEMU
belegt GOP, Scaling, Layout und GAL-Present bei 800x600, 1280x720 und der
korrigierten Full-HD-Referenz. `build/uefi-scaling-fullhd.png` ist der sichtbare
Nachweis.

Noch offen sind ein sichtbares Scale-Control in den Einstellungen, echte
physische DPI aus Hardwaredaten, Multi-/Dual-Screen, die gemeinsame BIOS-
Anbindung und QEMU-QHD/UHD. Letztere sind derzeit durch die statische
1920x1080-Compositorfläche begrenzt und deshalb nur im Layout-Hosttest belegt.

## NovaOS Design Compatibility

`NPSPEC-BOOTUI-0010` besitzt nun ein versioniertes Designmanifest 1.0.0. Es
bindet Theme-, Token-, Typografie-, Icon-, Font- und Motion-Version an dieselbe
Major-Version und schützt das Manifest mit einer FNV-Prüfsumme. Beim Start
werden Dark, Light und High Contrast, die gemeinsame Schrift, das NovaOS-Logo
und sämtliche semantischen Icons vor ihrer Verwendung geprüft. Eine
inkompatible Major-Version oder beschädigte Ressource verhindert die
Aktivierung.

Gemeinsame DLU-Token definieren Heading, Body, Caption, Zeilenhöhen,
Laufweiten, Gewichte, Buttonhöhe und -radius, Dialogradius, Kontrollabstände,
Motion-Dauern und -Kurven sowie Glass-, Kanten- und Schattenwerte. OEM-Daten
sind auf validierte Hintergrund-, Logo- und Animationsressourcen sowie eine
opake Akzentfarbe begrenzt; Komponenten, Navigation, Layout und Basistokens
bleiben unveränderbar. Der Hosttest prüft Versionen, Ressourcen, Tokenwerte und
OEM-Grenzen. Der QEMU-Theme-Test hat alle drei Themes sichtbar durchlaufen und
protokolliert `UEFI:DESIGN-COMPATIBILITY-READY`.

Die vollständige Akzeptanz kann noch nicht behauptet werden: Im Repository
existiert kein NovaOS-Desktop-Renderer als visueller Vergleichspartner. Ebenso
fehlen ein allgemeiner SVG-Primärpfad, ein externes gemeinsames Designpaket und
vollständige BIOS-Parität. Das Laden eines solchen Pakets bleibt zusätzlich an
den nicht normativ definierten BAP-/Index-Wire-Formaten blockiert.

## Bootmanager UI Architecture

`NPSPEC-BOOTUI-0001` wird jetzt durch ein zentrales ABI-1.0-Manifest technisch
abgebildet. Es registriert Platform, Resources, Graphics, Renderer, Scene,
Layout, Motion, Controls, Navigation, Dialoge, Boot Applications und
Diagnostics als getrennte Schichten. Jede Schicht besitzt eine feste ID,
Version, Plattformneutralitätsangabe und explizite Abhängigkeitsmaske. Eine
Schicht kann nur nach allen Vorgängern bereit werden; doppelte Registrierung,
ABI-Konflikte und verfrühte Anwendungsschichten werden verworfen.

Der produktive UEFI-Start validiert das vollständige Manifest erst nachdem die
konkreten Module initialisiert wurden und protokolliert anschließend
`UEFI:UI-ARCHITECTURE-READY`. Der Hosttest erzwingt zusätzlich einen falschen
Application-vor-Navigation-Versuch. Zusammen mit Runtime, State Model, GAL,
Compositor, Layout, Motion, Controls, Navigation, Dialogen und Recovery ist die
modulare UEFI-Pipeline damit explizit und prüfbar.

Teilweise offen bleiben die gemeinsame BIOS-Anbindung, das vollständige
Retained-Window-System, Touch, SVG/Blur, reale Firmware-Performancewerte und
Tests auf VMware, Hyper-V, VirtualBox und Bare Metal. Diese Punkte werden nicht
durch einen QEMU-Erfolg als erledigt ausgegeben.

## Retained Scene Graph und Render Commands

Der erste Block aus `NPSPEC-BOOTRENDER-0001` bis `0003` ist nun als eigene,
plattformneutrale Laufzeit vorhanden. Der Scene Graph verwendet einen festen
Pool mit 256 Nodes und genau einem unsichtbaren Root. Nodes besitzen stabile
IDs, Parent/Child/Sibling-Beziehungen, Typ, Visibility/Hidden/Collapsed,
Enabled, Bounds, lokale und akkumulierte 16.16-Matrizen, lokale und vererbte
Opacity, Layer, Z-Index, Ressourcenhandle sowie getrennte Layout-, Render-,
State-, Animation- und Transform-Dirtybits.

Attach, Detach und Destroy validieren sämtliche Handles, verhindern Zyklen und
Mehrfacheltern und erzwingen Kind-vor-Eltern-Abbau. Die lineare Traversierung
ist in Einfügereihenfolge deterministisch und akkumuliert Transformation,
Opacity und Sichtbarkeit. Jedes erzeugte Control wird automatisch als Node
registriert. Elternbeziehungen werden gespiegelt; Bounds, Sichtbarkeit,
Aktivierung und Dirtyzustand werden vor jedem produktiven UEFI-Compositing
synchronisiert und der Baum anschließend traversiert.

Der neue heapfreie Render Command Buffer besitzt 1024 Plätze pro Frame.
Commands sind nach Submit unveränderliche Kopien und enthalten ID, Typ, Layer,
Z-Index, Erzeugungsreihenfolge, Clip-ID, Opacity, Ressourcenhandle, Ziel-Surface
und Geometrie. Die Queue validiert vor der Ausführung, sortiert stabil nach
Layer, Z und Creation Order, bildet angrenzende Rechteck-Batches, verwaltet
verschachtelte Clips und rastert Clear, Rechteck, Bresenham-Linie und gefüllten
Kreis. Der produktive Bootframe erzeugt seinen Hintergrund bereits über diese
Queue; Compositor und GAL bleiben davon getrennte nachfolgende Phasen.

Hosttests prüfen Zyklenschutz, Transform-/Opacity-Vererbung, Traversal,
Lebensdauer, ungültige Commands, Sortierung, Batchbildung, Clipping und Pixel-
Resultate. Das reale GPT/FAT32-Image unter OVMF protokolliert
`UEFI:SCENE-GRAPH-READY` und `UEFI:RENDER-COMMANDS-READY` und präsentiert elf
vollständige Frames.

Noch offen sind die vollständige Migration aller Text-, Icon- und Control-
Primitive auf Commands, Command-Erzeugung ausschließlich aus dem Scene-
Traversal, weitere Primitive und State-Tabellen, PNG/SVG sowie die identische
BIOS-Anbindung. Deshalb bleiben alle drei NPSPECs ehrlich als teilweise
integriert gekennzeichnet.

## Layer and Surface Management

`NPSPEC-BOOTRENDER-0004` besitzt jetzt getrennte zentrale Manager für Surface-
und Layer-Lebensdauer. Der Surface Manager stellt 256 generationensichere
Handles bereit. Größen- und Pitchberechnung sind overflow-fest, Pitch wird auf
64 Byte ausgerichtet und Breite, Höhe, Einzelgröße sowie Poolbudget werden vor
jeder Reservierung geprüft. Feste Segmente trennen Persistent, Scene, Effect,
Frame und Emergency; Recycling verwendet nur format-, pool- und
kapazitätskompatible Slots und erhöht dabei die Handle-Generation.

Referenzzählung erkennt Überlauf und Double Release. Exklusive Locks verhindern
Freigabe oder konkurrierenden Zugriff; Unlock übernimmt eine konkrete Damage-
Region oder Full Damage. Der Framepool wird atomar zurückgesetzt. Eine kleine,
bereits beim Start reservierte Emergency-Surface bleibt normalen Pfaden
entzogen. Die beiden großen vorhandenen Compositorflächen werden ohne Kopie als
validierte externe Backbuffer-/Layer-Surfaces importiert.

Der Layer Manager verwaltet bis zu 512 Layer bei höchstens 32 Ebenen Tiefe. Er
kennt Root, Background, Content, Control, Popup, Dialog, Overlay, Notification,
Diagnostic, Cursor, Emergency und Custom sowie alle fünf Surface-Policies.
Generation Handles, Eltern-/Kindlisten und Zyklenschutz sichern die
Lebensdauer. AUTO isoliert Gruppen mit Opacity, Transform, Blur, Maske oder
Cache; REQUIRED, CACHED und TRANSIENT isolieren zwingend. Traversal akkumuliert
Worldtransform, Opacity und Clip und sortiert stabil nach Standardklasse,
Z-Index, Creation Order und ID. Damage propagiert bis zum Root. Mutationen
während Traversal, Compositing und Present werden abgewiesen.

Der produktive UEFI-Pfad bildet Background, Content, Controls, Dialog,
Overlay, Cursor und Emergency als verwaltete Layer ab, synchronisiert Bounds,
Opacity und Dialogsichtbarkeit in der Updatephase und sperrt die Struktur für
Compositing und Present. Hosttests decken Erzeugung, Recycling, stale Handles,
Budgets, Lockkonflikte, Frame-Reset, Emergency, Hierarchie, Zyklus, Isolation,
Sortierung, Damage, Surface-Bindung und Phasensperre ab. QEMU bestätigt
`UEFI:SURFACE-MANAGER-READY` und `UEFI:LAYER-MANAGER-READY`.

Weiterhin offen bleiben Mutation Queue, Maskensurfaces, Cache-Eviction,
Triple-Buffering, partielle Present-Regionen, die vollständige automatische
Qualitätsdegradation und BIOS-Parität. Der Audit kennzeichnet die Spezifikation
daher weiterhin als teilweise integriert.

## Framebuffer Backend

`NPSPEC-BOOTRENDER-0005` besitzt nun ein separates Framebuffer Backend unter
dem Compositor. Es erkennt VBE, GOP und Test als Backendtypen und hält einen
erweiterbaren virtuellen Typ frei. Die einmalige Initialisierung übernimmt den
bereits firmwarevalidierten Graphics Context und prüft Adresse, Auflösung,
bytegenauen Pitch, BPP, Pixelformat, Mappinggröße und Capabilities erneut. Der
Compositor ruft keine GOP- oder rohe GAL-Present-Funktion mehr auf.

Die gemeinsame API bietet bounds-geprüftes Lesen und Schreiben einzelner
Pixel, horizontale und vertikale Linien, Rechtecke sowie Blockkopien aus dem
kanonischen 32-Bit-Farbmodell. Jede Adresse wird aus `y * Pitch + x * BPP`
berechnet und nochmals gegen die Framebuffergröße geprüft. RGBA8888,
BGRA8888, RGB888, BGR888, RGB565 und validierte Bitmasken werden über die
gemeinsame Konvertierung verarbeitet.

Ein Frame beginnt atomar, nimmt maximal 32 Dirty Regions auf, vereinigt
überlappende Bereiche und darf nur in der Runtime-Presentphase abgeschlossen
werden. Die GAL validiert zunächst sämtliche Regionen und schreibt erst danach
die betroffenen Pixel; ein ungültiger Bereich erzeugt deshalb keinen
Teilzustand. Ohne Damage wird sicher ein Full Present verwendet. Diagnosewerte
umfassen Backend, Mappingdaten, Frames, Full/Partial Presents, geschriebene
Bytes, Pixel-/Linien-/Rechteck-/Blockzugriffe, Bounds-, Mapping-, Format- und
Presentfehler.

Hosttests verwenden einen Pitch von zehn Pixeln bei nur acht sichtbaren
Pixeln. Sie prüfen sämtliche Primitive, einen Schreibversuch außerhalb der
Bounds sowie ein Partial Present und bestätigen, dass ein Pixel außerhalb der
Dirty Region seinen Sentinelwert behält. Der produktive GOP-Pfad initialisiert
den Vertrag vor der UI, der Compositor übergibt seine Damage-Menge an ihn und
QEMU bestätigt `UEFI:FRAMEBUFFER-BACKEND-READY` sowie elf vollständige
Presentphasen.

Offen bleiben die Anbindung dieses C-Vertrags an den platzoptimierten
BIOS-Assembler, ein explizit umschaltbarer Single-Buffer-Betrieb,
plattformabhängige Cache-Flush-/MMIO-Barrieren, ein echtes Virtual-/Remote-
Backend und Bare-Metal-Nachweise. Deshalb bleibt der Auditstatus teilweise.

## UEFI GOP Backend

`NPSPEC-BOOTRENDER-0006` ist jetzt in eine firmwareunabhängige GOP-Policy und
einen dünnen UEFI-Adapter getrennt. Der Adapter verwendet ausschließlich
`EFI_GRAPHICS_OUTPUT_PROTOCOL`, inventarisiert über `QueryMode` bis zu 64 Modi,
kopiert die untrusted Firmwaredaten in feste Kandidatenstrukturen und gibt die
Firmwarepuffer sofort wieder frei. Im Render- oder Present-Hotpath findet kein
GOP-Aufruf und keine Allokation statt.

Die Policy prüft pro Kandidat Auflösung, PixelsPerScanLine, Overflow,
Pixelformat und bei BitMask vier disjunkte Farbmasken. BLT-only wird
kontrolliert abgelehnt. Ohne Vorgabe bleibt der gültige aktuelle Modus aktiv;
eine gewünschte Auflösung muss exakt vorhanden sein. Ist der aktuelle Modus
ungültig, wird deterministisch der größte gültige Modus und bei Gleichstand
die kleinste Mode-ID gewählt. Nach einem `SetMode` wird nicht der alte
QueryMode-Snapshot verwendet, sondern der aktive GOP-Modus erneut vollständig
gelesen und validiert.

Der resultierende Descriptor enthält Framebufferadresse und -größe, Auflösung,
Pitch, Bytes pro Pixel, internes Format, Farbmasks, Mode-ID und Modeanzahl.
FrameBufferBase muss ungleich null sein; `Pitch * Height` wird overflow-fest
gegen FrameBufferSize geprüft. Erst danach werden GAL und gemeinsames
Framebuffer Backend aktiviert. Fehler führen über den bereits vorhandenen
Runtime-Recoverypfad in den verständlichen Textfallback und blockieren den
Bootvorgang nicht.

Hosttests prüfen Current-, Preferred- und deterministische Fallback-Auswahl,
eine nicht vorhandene Wunschauflösung, BLT-only, Descriptor und Shutdown. QEMU
belegt den normalen GPT/FAT32-Start sowie separate echte GOP-Modi 800×600,
1280×720 und 1920×1080. Alle vier Starts melden
`UEFI:GOP-MODES-VALIDATED`; das Standardimage meldet zusätzlich
`UEFI:GOP-DESCRIPTOR-READY` und präsentiert elf Frames.

Noch offen sind die Übergabe dieses Descriptors über ein UEFI-NBHP/BIB an den
Kernel, automatisierte GOP-not-found- und Safe-Mode-Injektion, ein echter
BitMask-GOP-QEMU-Modus sowie Nachweise auf mehreren realen Firmware- und
Bare-Metal-Implementierungen. Daher bleibt auch diese NPSPEC teilweise.

## BIOS VBE Backend

NPSPEC-BOOTRENDER-0007 verwendet im BIOS-Pfad nicht länger einen
festverdrahteten Modus. Stage 2 fordert VBE-2.0-Controllerdaten an, prüft
VESA-Signatur und Version und liest höchstens 256 Einträge aus der
Firmware-Modusliste. Die Auswahl folgt exakt der vorgegebenen Reihenfolge
1920×1080, 1600×900, 1366×768, 1280×720, 1024×768 und 800×600; erst danach
darf ein anderer kompatibler Modus verwendet werden.

Jede ModeInfo wird auf Supported-, Graphics- und LFB-Attribute, 32 Bit Direct
Color, PhysBasePtr, Auflösung, bytegenauen Mindest-Pitch, overflow-festes
Pitch × Height, das Framebuffer-Adressende und BGRX8888-Farbmasks geprüft.
Der ausgewählte Descriptor wird direkt vor und nach SetMode erneut validiert.
Erst dann werden Framebufferflag, Adresse, Pitch, Breite, Höhe, BPP und
Pixelformat in den BIB-Graphics-TLV übernommen. Jeder Fehler löscht Flag und
vollständige Payload, schaltet Modus 3 ein und setzt den Bootvorgang fort.

make test-platform belegt mit QEMU std VGA BIOS:VBE-BACKEND-READY, den
gewählten Modusrang, die interaktive GUI und NOVA_KERNEL_READY.
make test-bios-vbe-fallback startet dasselbe Image mit -vga none und belegt
BIOS:VBE-TEXT-FALLBACK sowie einen weiterhin erfolgreichen Kernelstart.

Teilweise offen bleibt die Bindung des gemeinsamen C-Framebuffer-Backends mit
Full/Partial Present an den platzbegrenzten BIOS-Assembler. Der gegenwärtige
BIB-v1.2-Graphics-TLV besitzt außerdem keine normativen Felder für
Framebuffergröße oder einzelne Farbmasks; die Masken werden daher vor der
Übergabe streng validiert und durch BGRX8888 beschrieben, aber nicht einzeln
transportiert. RGB888/RGB565 werden nicht aktiviert, weil dieselbe NPSPEC VBE
2.0, LFB und 32 Bit als Voraussetzung festlegt. Firmware-Injektion für einzelne
ungültige ModeInfo-Felder und reale Bare-Metal-Nachweise fehlen ebenfalls.
