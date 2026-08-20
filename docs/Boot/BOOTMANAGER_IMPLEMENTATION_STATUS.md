# NovaOS Bootmanager – Implementierungsstatus und Blocker

Stand: 2026-08-13

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

## Zentrales Boot-UI-Diagnoseframework

NPSPEC-BOOTDIAG-0001 besitzt nun eine zentrale, heapfreie Diagnose-API. Elf
Standardbereiche und weitere fest begrenzte Module lassen sich registrieren.
Ereignisse tragen Sequenz, Zeit, sechs Schweregrade, Kategorie, Modul, Quelle,
Beschreibung, Metadaten sowie optional Fehlerklasse, Bootphase, Ursache und
Empfehlung. Der chronologische Ringpuffer ist bis 256 Einträge konfigurierbar;
Abfragen filtern Zeitraum, Modul, Schweregrad, Kategorie, Bootphase und
Fehlerklasse.

Performance-, Speicher- und Ressourcen-Snapshots liefern unter anderem FPS,
Frame-/Render-/Layout-/Inputzeit, CPU-/GPU-Telemetrie, Speicher, Peak,
Fragmentierung, Cachebelegung, Evictions und Budgetverletzungen. Als sensibel
markierte oder durch die Schutzregel erkannte Passwörter, private Schlüssel,
Secrets und Token werden vor der Speicherung redigiert. NDF/1, JSON, Plain
Text und ein versioniertes Binärformat werden ausschließlich nach expliziter
Benutzerfreigabe in einen vom Aufrufer bereitgestellten Puffer exportiert.
Live-Diagnose ist standardmäßig aus und ebenfalls autorisierungspflichtig.

Die Diagnoseansicht aktualisiert die Datensicht über Übersicht und erzeugt
über Ereignisse ausdrücklich einen lokalen NDF-Bericht. Hosttests decken
Ringüberlauf, Module, strukturierten Fehlerkontext, sämtliche Filter,
Redaktion, vier Exportformate, Autorisierung, Live-Gate und Reset ab. Das reale
GPT/FAT32-Abbild meldet unter OVMF/QEMU
`UEFI:DIAGNOSTICS-FRAMEWORK-READY` vor dem ersten vollständigen Menüframe.

Offen bleiben ein persistentes Exportziel mit Dateiauswahl, echte Firmware-
Sensoren für CPU-/GPU-Auslastung, Datenträger-/Netzwerkdiagnose, Expertenmodus,
der vollständige Tab-/Detailaufbau aus BOOTMANAGER-UI-0014, BIOS-Parität und
eine belastbare Overheadmessung auf physischer Hardware.

## Rendering Statistics

NPSPEC-BOOTDIAG-0002 verwendet einen vorallokierten Ring für 600 Frames. Jeder
Eintrag enthält Frame-ID, Start, Ende, Dauer, Draw Calls, Dirty Regions und
deren Fläche sowie die Layeranzahl. Die API liefert den aktuellen Frame,
rollierende Werte für eine und zehn Sekunden und die gesamte Sitzung mit FPS,
Minimum, Maximum und Mittelwert. Compositor, Renderqueue, Dirty-/Layer-Manager,
Framebuffer, Effects, Blur, Image Renderer, Motion, Resources und Memory
speisen ihre vorhandenen Zähler in einen gemeinsamen Snapshot ein.

Die Diagnoseübersicht zeigt nach ausdrücklichem Aktualisieren FPS, Framezeit
und Draw Calls. Renderingdaten können auf Benutzeranforderung heapfrei als
NDF, JSON, CSV oder versioniertes Binärformat in einen bereitgestellten Puffer
exportiert werden. Hosttests prüfen zwei unterschiedliche Frameklassen,
Sitzungsaggregation, Software-Renderer, Speicherwerte und alle vier Formate.
Das GPT/FAT32-Image meldet in OVMF/QEMU
`UEFI:RENDERING-STATISTICS-READY`.

Noch nicht verfügbar sind echte GPU-Zeit, Transferzeit und GPU-Speicher, weil
QEMU und der aktuelle Bootmanager kein Hardware-GPU-Backend besitzen. Ebenso
fehlen SVG-Zähler wegen des dokumentierten unvollständigen SVG-Vertrags,
Font-/Glyph-Detailcounter, per-Layer/-Fenster-Zeitmessung, physische Langzeit-
und Overheadtests sowie dieselbe Pipeline im BIOS-Pfad.

## Input Event Tracing und sichtbare GUI-Reaktionszeit

NPSPEC-BOOTDIAG-0003 besitzt nun einen vollständig vorallokierten, passiven
256er Trace-Ring für Tastatur, Repeat, Pointer, Wheel, Touch, Controller,
Fokuswechsel und Shortcuts. Er speichert Geräte-, Control-, Fenster- und
Dialogkontext sowie Capture-, Dispatch-, Handler- und Abschlusszeiten,
Queuezustand und Renderauslösung. Filter, Fehlerzähler und nutzerautorisierter
NDF-/JSON-/CSV-/Binärexport sind integriert. Tracing ist standardmäßig aus;
Secure-Eingaben unterdrücken Unicode vor der Speicherung. Die Ereignisaktion
aktiviert es ausdrücklich und exportiert/deaktiviert es beim nächsten Aufruf.

Nach dem gemeldeten sichtbaren Stocken wurde außerdem der reale Softwarepfad
optimiert: Das 5-ms-Polling ersetzt die frühere 20-ms-Schleife, Low-End-
Animationsframes warten nur noch 1 statt 30 ms, vollständig transparente
Pixel werden im Compositor übersprungen, opaque Pixel direkt übernommen und
BGRA-GOP-Zeilen ohne allgemeine Pixelkonvertierung kopiert. Die statische
Grundfläche wird pro View/Theme/Auflösung/Skalierung gecacht. Normale Auswahl-
und Pointerframes rasterisieren nur Interaktionspanel, Status sowie alte und
neue Cursorfläche; der Compositor erhält und präsentiert mehrere getrennte
Dirty Regions. Dialoge, Tooltips und Übergänge behalten sicherheitshalber den
Vollbildpfad.

Der UEFI-Hotpath bündelt jetzt außerdem die häufigen relativen
Simple-Pointer-Pakete zu höchstens einem sichtbaren Bewegungsframe je 15 ms.
Die Eingabeabfrage selbst bleibt bei 5 ms. Tastenflanken, Loslassen und
Mausrad werden sofort verarbeitet, während reine Bewegungen akkumuliert
werden. Dadurch lösen QEMU-Mauspakete nicht mehr jeweils ein vollständiges
Software-Compositing aus; der Marker `POINTER-FRAME-COALESCED` macht den
produktiven Pfad diagnostizierbar.

Ein Startframefehler in derselben Cachepipeline ist behoben: Zuvor wurde die
statische Basisebene im ersten Entrance-Frame mit Deckkraft null präsentiert
und ihre Dirty Regions dabei verbraucht. Dadurch konnten Nova-Dark-Hintergrund,
linkes Branding und oberer Nova-Blau-Balken dauerhaft schwarz beziehungsweise
unsichtbar bleiben. Die Basisebene wird jetzt sofort deckend präsentiert; nur
der interaktive Inhalt verwendet den Entrance-Fade. Der reale 1280x720-QEMU-
Framebuffer enthält danach 633.722 Pixel in `#101113` und 15.943 Pixel in
`#267CC1`, bei keinem vollständig schwarzen Pixel.

## Boot UI Test Architecture

NPSPEC-BOOTTEST-0001 besitzt jetzt eine zentrale, heapfreie Testarchitektur
mit einem vorreservierten 64er Registry-/Result-Pool. Testfaelle tragen eine
stabile ID, Suite, versionierte Testdaten, Name, Beschreibung, Erwartungswert,
Modul, Bootphase, Prioritaet, Hardware- und Qualitaetsprofil sowie Testebene
und Testart. Der Runner filtert Suites, plant sie in stabiler
Registrierungsreihenfolge, erfasst Pending/Running/Passed/Failed/Skipped,
Fehlercode, Ist-/Sollwert, Laufzeit, Wiederholungen, Gesamt-/Durchschnittszeit
und prueft ueber einen Zustandsfingerabdruck die Isolation jedes Falls.

Der Hosttest belegt Registrierung, Duplicate-Abweisung, zwei PASS, einen
SKIPPED, deterministische 25-us-Messwerte, autorisiertes Reporting sowie die
Erkennung eines Erwartungsfehlers. Im GPT/FAT32-UEFI-Boot sammelt derselbe
Runner die bereits separat validierten Control-, Navigation-, Dialog- und
Rendering-Suites und meldet `BOOT-TEST-ARCHITECTURE-READY` vor dem Start der
interaktiven Runtime.

Noch offen sind produktive Installer-/Self-Healing-/Touch-/ARM64-Suites,
echte Stackinformationen, persistente CI-Artefakte, Nightly-/PR-/Release-
Workflowdefinitionen, ein vollstaendiger Coverage-Analyzer auf API- und
Zustandsebene, Stresslaeufe auf physischer Hardware sowie die gemeinsame
Anbindung der Legacy-BIOS-Tests an denselben C-Runner.

## Rendering Reference Tests

NPSPEC-BOOTTEST-0002 besitzt jetzt einen heapfreien, rendererunabhaengigen
Referenzbild-Comparator mit einem festen 32er Ergebnispool. Versionierte und
schreibgeschuetzte Metadaten enthalten Referenz-ID, Version, Aufloesung, DPI,
Theme, Hardwareprofil und Erstellungszeit. Der Vergleich wertet RGB und Alpha
mit der normativen Kanalabweichung von maximal eins aus, erlaubt eine
Positionsabweichung von einem Pixel, unterstuetzt Masken und erzeugt fuer jede
nicht tolerierte Abweichung einen magentafarbenen Differenzpixel. Ergebnis und
autorisierter Bericht enthalten Referenz-/Test-ID, abweichende Pixel, groesste
Kanalabweichung, erste Fehlerposition und Aehnlichkeit.

Der Hosttest prueft exakte, Farb-, Positions-, Fehler- und Maskenframes sowie
das Export-Gate. Dieselbe API laeuft vor der UEFI-Runtime mit zwei PASS- und
einem absichtlich erkannten FAIL-Fall; das reale GPT/FAT32-QEMU-Image meldet
`RENDER-REFERENCE-TESTS-READY`. Bestehende QEMU-Skripte erzeugen reale Frames
fuer Dark/Light/High Contrast, Dialog, Tooltip, Recovery und 800x600,
1280x720 sowie 1920x1080.

Noch fehlt ein freigegebener, versionierter und schreibgeschuetzter
Vollbild-Baselinebestand, der diese QEMU-Frames automatisch in allen drei
Themes, den fuenf Pflichtaufloesungen und allen fuenf Hardwareprofilen mit dem
Comparator verbindet. Ebenso fehlen persistente Heatmapdateien und
Bereichsbeschreibungen, produktives SVG-/GPU-Rendering, von QEMU-OVMF
angebotene 1024x768-/4K-GOP-Modi, physische Hardware und BIOS-Paritaet. Diese
Punkte werden deshalb weiterhin nicht als bestanden ausgewiesen.

## Dialog Tests

NPSPEC-BOOTTEST-0005 besitzt nun eine isolierte, heapfreie 10er Testsuite samt
Ergebnis-, Summary- und autorisierter Report-API. Information, Warning, Error,
Confirmation, Progress, Credential, Recovery, Network, Firmware und Custom
werden über den produktiven Dialogpfad geöffnet. Die Suite prüft Modal-Falle,
deterministischen Fokusumlauf, Tastatur-/Escape-Abschluss, Progresszustand,
UTF-8-Credentials und sichere Löschung. Dialogpool, IDs und Diagnosezustand
werden nach jedem Fall exakt restauriert. Hosttest und GPT/FAT32-UEFI-Start
prüfen zehn erfolgreiche Fälle und den Marker `DIALOG-TESTS-READY`.

Touch/Double-Tap/Long-Press, Screenreader, echte Zeitmessung, Referenzbilder für
alle zehn Typen, sämtliche Theme-/Scale-Kombinationen und BIOS-Parität bleiben
offen. Netzwerk- und Firmwaretypen besitzen den gemeinsamen produktiven
Dialogvertrag, aber noch keine eigenen fachlichen Backends.

## Navigation Tests

NPSPEC-BOOTTEST-0004 besitzt nun eine isolierte, heapfreie 10er Routenregistry
mit Ergebnis-, Summary- und autorisierter Report-API. Bootmanager, Recovery,
Diagnose, Dialog, Liste, Menü, Tastatur und Pointer laufen über den produktiven
Navigation Stack. Jede Route prüft Startzustand, Push, Transitionabschluss,
Zielseite, Selection/Fokus/Scroll/Kontext, Back-Restore, Root-Grenze,
Sackgasse und Schleifenindikator. Stack, Visualzustand, Motionzeiger und
Diagnosewerte werden nach jedem Lauf exakt restauriert. Installer und Touch
werden mangels produktivem Backend reproduzierbar als `SKIPPED` statt als
Scheinerfolg ausgewiesen. Host und GPT/FAT32-UEFI validieren acht PASS, zwei
SKIPPED und `NAVIGATION-TESTS-READY`; der bestehende interaktive QEMU-Test
belegt zusätzlich reale Unterseiten, Dialoge, Hilfe, Rücknavigation sowie
Slide-/Fade-Transitions.

Offen bleiben produktive Installer-/Touchrouten, Screenreader, Double Click,
Gesten/Long Press, echte Laufzeitmessung, vollständige Screenshots pro Route,
Self-Healing/Snapshotfachlogik, physische Eingabegeräte und BIOS-Parität.

## Routed Control Events

NPSPEC-BOOTCONTROL-0001 besitzt jetzt ein gemeinsames heapfreies
Ereignissystem fuer alle 18 definierten Control-Ereignisse von Create bis
ThemeChanged. Jeder der 128 Control-Slots besitzt genau einen Handler plus
Kontext. Dispatch beginnt am Target und leitet ein unbehandeltes Ereignis in
stabiler Reihenfolge iterativ ueber Parent, Dialogcontainer und Anwendung nach
oben. Die Route wird vor Ausfuehrung validiert; ungueltige Handles, zerstoerte
Controls, Zyklen und Tiefenueberlauf werden ohne Rekursion abgewiesen.

Focus erzeugt Blur/Focus, Invoke erzeugt Click. Der produktive UEFI-Pfad
erzeugt ausserdem MouseEnter, MouseLeave, MouseMove, MouseDown, MouseUp und
KeyDown; CharacterInput wird erst nach akzeptierter Texteingabe erzeugt.
Treffer auf dekorativen Icons werden auf das vollstaendige semantische
ListItem beziehungsweise Tile normalisiert. Die Bootmanager-Liste empfaengt
dadurch Maus- und Tastaturereignisse als gebubbelte Item-Ereignisse.
Lifecyclewechsel erzeugen StateChanged, Bounds LayoutChanged, Text-/Range-/
Slider-/Toggle-Aenderungen ValueChanged, Texteingabe CharacterInput und die
zentrale Theme-Neubindung ThemeChanged. Damit muessen spezialisierte Controls
diese Ereignisse nicht mehr manuell oder uneinheitlich ausloesen.
Beim Destroy wird ein letztes Ereignis zugestellt, der Handler entfernt und
das Control jetzt korrekt aus der Siblingkette des Parents ausgehaengt. Die
Diagnose zaehlt Dispatches, behandelte und gebubbelte Ereignisse sowie Fehler.
Host und GPT/FAT32-UEFI pruefen fuer sieben reale Eingabeereignistypen die
Target-Parent-Reihenfolge sowie Stop bei handled, Exactly-once-Invoke,
Zyklenschutz und sauberen Child/Parent-Abbau. QEMU meldet
`CONTROL-INPUT-EVENTS-READY` und weist fuer gesendete Tasten produktive
`CONTROL-KEY-EVENT-BUBBLED`-Ereignisse nach.

Die gemeinsame Input-Laufzeit erkennt DoubleClick nur nach einem vollstaendigen
ersten Klick auf demselben Control, innerhalb der konfigurierten 200..1000 ms
und hoechstens 4 DLU Bewegung. Nach einem erkannten Paar wird die folgende
Freigabe unterdrueckt, damit ein TripleClick keine ueberlappenden Paare bildet.
Das Ereignis wird geroutet, diagnostisch gezaehlt und bei autorisiertem Trace
mit Geraet, Pointer, Ziel, Position und Zeit gespeichert. Host und QEMU-UEFI
pruefen positive sowie Zeit-, Ziel- und Distanz-Negativfaelle.

Noch fehlen KeyUp aus UEFI SimpleTextInput, produktive
Touch-/Controller-Erzeuger, ein separates Preview/Capture-Modell, asynchrone
Quellen, BIOS-Anbindung und physische Eingabegeraetetests. Die NPSPEC fordert
fuer den normativen Fluss nur Target-zu-Parent-Bubbling; Capture ist daher eine
Erweiterung und kein vorgetaeschtes Akzeptanzkriterium.

## Secure Password Field

NPSPEC-BOOTCONTROL-PASSWORD-0001 besitzt jetzt eine spezialisierte
Password-Control-API auf dem festen Control-Pool. Generisches SetText wird fuer
Passwortfelder abgewiesen; Eingabe erfolgt codepointweise ohne Heap, wird
unmittelbar maskiert und beim Clear volatil ueber den gesamten 96-Byte-Puffer
geloescht. Unterstuetzt sind UTF-8-sichere Caret-/Delete-/Backspace-Grenzen,
Mindestlaenge, Upper-/Lowercase-/Ziffernregeln, Error-Zustand, private
Accessibility, Diagnose ohne Inhalt sowie ein explizit opt-in und auf maximal
zehn Sekunden begrenztes Reveal mit automatischem Timeout. Ungueltige Zeichen,
UTF-8 oder Overflow loeschen den sicheren Puffer sofort.

Der Credential-Dialog zeichnet nun das produktive Password Control statt einer
separaten Punktprimitive. Der Dialogpuffer bleibt die einzige Klartextquelle;
das Control erhaelt ausschliesslich einen synthetischen Maskenzaehler. Der
UEFI-Eingabepfad verarbeitet Zeichen und Backspace direkt im aktiven
Credential-Dialog. Ein automatisierter QEMU-Test navigiert zur Diagnose,
oeffnet den F12-Sicherheitsdialog, sendet fuenf Zeichen, wartet auf fuenf reale
Inputmarker und prueft einen stabilen Screenshot mit fuenf Maskierungspunkten.

Touch-/Controller-Tastatur, Clipboard- und Drag-and-Drop-Backends existieren
weiterhin nicht und koennen daher nur durch Abwesenheit abgesichert werden.
Ein externes Authentifizierungs-, TPM-/Datentraegerverschluesselungsbackend,
geschuetzte physische Speicherseiten, Screenreader-Hardwaretest, BIOS-Paritaet
und ein produktiver Reveal-Button bleiben offen.

## Control Interaction Tests

NPSPEC-BOOTTEST-0003 besitzt nun eine isolierte, heapfreie 12er Ergebnis- und
Report-API. Button, Toggle Button, Checkbox, List, TextField, Slider,
ProgressBar und Scrollbar laufen direkt über die produktiven Control-APIs;
Dialog und Navigation binden die spezialisierten Testsuites ein. Geprüft werden
Lifecycle, Bounds, Accessibility, Focus, Hover/Pressed, Dirty-/Visualzustand,
Ereignisreihenfolge, Exactly-once-Aktion, UTF-8-Text, Auswahl, Range/Step und
Scrollkopplung. Radio Button und ComboBox werden mangels produktiver Typen
ehrlich als `SKIPPED` geführt. Host und GPT/FAT32-UEFI validieren zehn PASS,
zwei SKIPPED und `CONTROL-INTERACTION-TESTS-READY`.

Der Tooltip-QEMU-Test deckte außerdem eine reale Overlay-Regression auf: Bei
einem komplexen transparenten Frame wurde der Overlaypuffer zwar geleert, die
darunterliegende gecachte Basisebene jedoch nicht erneut zusammengesetzt.
Dadurch konnten alte Selektionsmarker sichtbar bleiben. Tooltip, Kontextmenü,
Dialog und der jeweils erste Folgeframe erzwingen jetzt die Rekonstruktion der
Basisebene. Der neue QEMU-Tooltipframe zeigt ausschließlich den aktuellen
Selektionsmarker.

Radio Button, ComboBox, Tree View, Touch, Screenreader, vollständige
Modifier-/Repeatmatrix, Pixelreferenzen aller Zustände und BIOS-Parität fehlen.

Hosttests und der vollständige interaktive UEFI-Navigationstest bestehen. Das
GPT/FAT32-QEMU-Abbild weist `BASE-SURFACE-CACHE-HIT` und
`PARTIAL-INTERACTION-FRAME` nach. Ein objektiver 30-/60-FPS-Nachweis auf
physischer Hardware, GPU-Beschleunigung, weitere engere Item-Damage-Rechtecke
und BIOS-Parität bleiben offen.

## Semantische Interaction States

Die UEFI-Control-Laufzeit besitzt nun die zwölf in
`NPSPEC-BOOTDESIGN-0011` geforderten Zustände als zentrale, heapfreie
Registry mit fester Übergangsmatrix. Hover wird nur von Pointereingaben
angenommen; Fokus überlagert Hover und Auswahl; Disabled, ReadOnly und
Loading besitzen getrennte Eingabesperren. Error wird neben der Fehlerfarbe
mit einem sichtbaren Fehlersymbol und Statusstring dargestellt. Gemeinsame
Controls beziehen Hintergrund, Vordergrund, Rahmen und Radius aus dem
aktiven Zustandsstil. Dark, Light und High Contrast werden über denselben
Resolver bedient.

`boot-ui-runtime-check` prüft alle zwölf Zustände, erlaubte und verbotene
Übergänge sowie Eingabesperren. Der reale EDK2/QEMU-Pfad meldet
`UEFI:INTERACTION-STATES-READY`; das separat gebaute GPT/FAT32-Abbild
`build/nova-uefi-interaction-current.img` erreicht zusätzlich den vollständig
gezeichneten Countdownframe.

## Validiertes Dark Theme

Dark ist als versionierte Standardressource mit Descriptor,
`nova_theme_default`, `nova_theme_is_dark` und atomarer Default-Aktivierung
angebunden. Die Aktivierungsprüfung erfasst alle Farbrollen, Alpha- und
DLU-Invarianten sowie Mindestkontraste für Texte, Fokus und Akzent. Alle
nachgelagerten Designregistries lösen ihr Dark-Profil ohne Heap-Allokationen
und in O(1) auf. Das Image `build/nova-uefi-dark-current.img` wurde mit EDK2
gestartet und meldet `UEFI:DARK-THEME-READY` vor dem ersten vollständigen
Countdownframe. Externe Themepakete bleiben bis zur normativen Definition der
BAP-/Index-Wire-Formate blockiert.

## Validiertes Light Theme

Light besitzt nun denselben versionierten Ressourcen-, Descriptor-,
Kontrast- und atomaren Aktivierungspfad wie Dark, ist aber ausdrücklich nicht
als Default markiert. Der QEMU-Ablauftest synchronisiert Tasteneingaben an den
fertigen Navigationsframe, wechselt Dark → Light → High Contrast → Dark und
erzeugt referenzierbare 1024×768-Framebuffer. Das eigenständige Abbild
`build/nova-uefi-light-current.img` meldet `UEFI:LIGHT-THEME-READY` und
erreicht den vollständig gezeichneten Countdownframe.

## Validiertes High Contrast Theme

High Contrast besitzt einen eigenen Accessibility-Descriptor und erzwingt
vor der Aktivierung opake Materialien, Reduced Motion, mindestens 2 DLU
Fokusgeometrie, 7:1-Kontrast für Text/Fokus/Ebenen und 4,5:1 für alle
Statusrollen. Der QEMU-Thementest erzeugt einen echten High-Contrast-
Framebuffer und prüft Themewechsel sowie Reduced Motion. Das Image
`build/nova-uefi-high-contrast-current.img` meldet
`UEFI:HIGH-CONTRAST-THEME-READY`. Screenreader-/Sprachausgabe bleibt als
fehlendes externes Accessibility-Backend ausdrücklich offen.

## Visual-Continuity-Vertrag

Der Bootbereich stellt nun einen versionierten, heapfreien Übergabekontext
mit aktiver Theme-ID und Registry-Signatur bereit. Ein nachfolgendes Modul
muss dieselben Typography-, Spacing-, Radius-, Icon-, Material-, Motion- und
Interaction-Registries sowie ausschließlich semantische Tokens deklarieren;
Abweichungen werden vor der Nutzung abgewiesen. EDK2/QEMU meldet
`UEFI:VISUAL-CONTINUITY-READY` aus
`build/nova-uefi-continuity-current.img`.

Die vollständige Akzeptanz von `NPSPEC-BOOTDESIGN-0015` bleibt extern
blockiert: Login, Desktop und Nova Shell existieren im aktuellen Projekt noch
nicht als ausführbare Verbraucher dieses Vertrags. Deshalb sind die
geforderten Übergänge Boot → Login → Desktop und ein systemweiter
Pixel-/Registryvergleich derzeit nicht beweisbar.

## Fehlende normative Accessibility-Abhängigkeiten

`NPSPEC-BOOTDESIGN-0014` und `0015` verweisen auf
`NPSPEC-BOOTACCESS-0001` und `NPSPEC-BOOTTHEME-0001`. Beide Dokumente sind
im aktuellen NPSPEC-Bestand nicht vorhanden. Vorhandene High-Contrast-,
Tastatur-, Reduced-Motion- und semantische Control-Funktionen bleiben
implementiert und getestet; ein normativer Screenreader-/Accessibility-
Frameworkvertrag oder ein darüber hinausgehendes Theme-Wire-API wird jedoch
nicht erfunden.

## Unterbrechbare Animationen

Animationen erhalten Zwischenwert, Restdauer und Geschwindigkeit nun über
Interrupt, Resume, Redirect und Reverse. Die Übergänge laufen aus dem
aktuellen Framezustand weiter; Reverse verwendet den tatsächlichen Ursprung.
Hosttests prüfen alle Operationen und Diagnosen. Das eigenständige Image
`build/nova-uefi-motion-current.img` führt beim UEFI-Start einen Selbsttest
aus und meldet `UEFI:INTERRUPTIBLE-MOTION-READY`.

## Reduced-Motion-Policy

Reduced Motion klassifiziert nun jede animierbare Property zentral. Fade,
Tint und Fokusrahmen bleiben kurz erhalten, räumliche Bewegungen werden
ersetzt und Blur, Glow sowie Shadow deaktiviert. Ein Wechsel wirkt im selben
Frame auch auf laufende Animationen. Dialoge verwenden weder Scale noch
bewegte Materialeffekte, Navigation bleibt als Cross-Fade verständlich. Der
interaktive QEMU-Test committed die Einstellung und weist
`UEFI:NAV-REDUCED-FADE` nach; das Image
`build/nova-uefi-reduced-motion-current.img` meldet zusätzlich
`UEFI:REDUCED-MOTION-POLICY-READY`.

## Motion Performance Budget

Das Motion-System überwacht Frame- und Schedulergrenze, festen Poolverbrauch
und typisierte Objektlimits. Wiederholte Verletzungen deaktivieren in fester
Reihenfolge Glow, Shadow, Blur, Spring, Material und zuletzt dekorative
Animationen; danach ist Safe Mode aktiv. Essenzielle Status- und
Fokusübergänge bleiben erhalten. Hosttests prüfen alle sechs Stufen. Das
Image `build/nova-uefi-motion-budget-current.img` simuliert denselben Ablauf
im UEFI-Pfad und meldet `UEFI:MOTION-BUDGET-READY`.

## Boot-UI- und Frame-Performance

Die Diagnose-API führt nun FPS, Frameklasse A–E, alle acht Pipelinezeiten,
Min/Max/Mittelwert, Draw Calls, Dirty Regions, Speicher sowie Soft-, Hard-
und Phasenverletzungen. Startupgrenzen und ein vollständiger Klasse-A- sowie
Klasse-E-Frame werden im Host und im UEFI-Selbsttest geprüft. Das Image
`build/nova-uefi-frame-budget-current.img` meldet
`UEFI:FRAME-BUDGET-READY`.

Noch nicht als Erfüllungsnachweis gewertet werden reale Zeitziele: Der
aktuelle Firmwarepfad besitzt keine phasenweise Hochpräzisionsmessung und der
vollständig softwaregerenderte QEMU-Debugaufbau ist deutlich langsamer als
100 ms bis zum ersten Frame. Reale 30/60-FPS- sowie Startup-Grenzen benötigen
optimierten Releasecode und Tests auf repräsentativer physischer Hardware.

## Deterministisches Speicherbudget

Der zentrale Memory Manager besitzt nun konfigurierbare Minimal-, Standard-
und Comfort-Profile mit 32, 64 und 128 MiB. Rendering, Ressourcen-, Glyph-,
SVG- und Themecache, Animationen, Controls, Layout, Diagnose und Reserve haben
jeweils feste Budgets. Die Diagnose liefert Gesamt-, Frei-, Peak-, Pool- und
Cacheverbrauch, Fragmentierung, Evictions, Budgetverletzungen und
Speicherdruckstufe. Der vorhandene feste Arena-Allocator sperrt reguläre
Session-Allokationen nach Eintritt in die UI-Laufzeit; nur der vorallokierte
Framepool bleibt pro Frame nutzbar. Cache-Evictions aus dem Resource Manager
werden zentral bilanziert, und sensible Puffer können über eine gegen
Wegoptimieren geschützte Zeroisierung bereinigt werden.

Hosttests decken alle drei Profile, die exakte Bereichssumme, Überlauf,
fünfstufigen Speicherdruck, Laufzeitsperre, Frame-Reset und sichere Löschung
ab. Das aktuelle GPT/FAT32-Abbild meldet unter EDK2/QEMU
`UEFI:MEMORY-BUDGET-READY`.

Noch nicht vollständig angebunden sind die separaten Verbrauchszähler aller
spezialisierten Glyph-, SVG-, Theme-, Layout- und Draw-Command-Pools sowie die
automatische Ausführung jeder konkreten Degradationsaktion. Crypto-, TPM- und
Recovery-Schlüsselverbraucher existieren noch nicht; ihre sichere Löschung
kann daher nur auf API-Ebene, nicht im End-to-End-Fluss belegt werden.

## Startup-Time Budget

Die Diagnose verwaltet nun feste Zeitbudgets für Stage 1/2, Hardware, Grafik,
Ressourcen, Theme, UI, Bootmanager, Kernelladen und Kernelübergabe. Jeder
Phaseneintrag enthält Start, Ende, Dauer, Sollwert, Abschlussstatus und
Verletzungen. Zusätzlich werden Gesamtzeit, Standard-/Recovery-/Diagnoseziel,
Hardwareklasse, Performanceklasse A bis E, Optimierungshinweise und die Zeit
vom Abschluss der UI-Initialisierung bis zum ersten vollständigen Frame
geführt. Eine Überschreitung erzeugt ein Diagnoseereignis, unterbricht den
Bootvorgang aber nicht und setzt keine Sicherheitsprüfung außer Kraft.

Der UEFI-Pfad misst Entry, Grafik, Ressourcen, Theme, UI, Bootmanager und den
ersten Frame mit einem monotonen TSC-Zeitgeber. Hosttests prüfen sämtliche
exakten Phasengrenzen, die 50-ms-Erstframegrenze, Überschreitungsdiagnose und
alle Klassen A bis E. Das GPT/FAT32-Abbild bootet unter EDK2/QEMU bis zur
vollständigen Oberfläche und meldet `UEFI:STARTUP-BUDGET-READY`.

Stage 1 liegt vor dem Eintritt in `BOOTX64.EFI`; Kernelladen und Handoff werden
erst bei einem tatsächlich gestarteten Systemeintrag abgeschlossen. Außerdem
ist der 1-GHz-TSC-Fallback für virtuelle Maschinen nur eine Schätzung. Die
normativen Echtzeitziele benötigen deshalb weiterhin einen kalibrierten
Hardwaretimer sowie Kalt-, Warm-, Recovery-, Netzwerk-, Installer- und
Slow-Disk-Messungen auf repräsentativer physischer Hardware.

## Adaptive Quality Management

Der Quality Manager verarbeitet nun CPU-Score, GPU-Verfügbarkeit und -Last,
Software-Rendering, freien Speicher, Framezeit, Cachefüllung und Auflösung.
Auto, Ultra, High, Balanced, Low und Safe definieren Blur, Schatten, Glow,
Transparenz, Antialiasing, Sampling, Rasterstufen für SVG/Glyphen/Icons,
Animationsschritt, Cachebudget und Compositor-Fallback. Drei dauerhaft
langsame Frames lösen eine Degradation aus; eine Erholung erfolgt erst nach
120 stabil schnellen Frames und jeweils nur um eine Stufe. Dadurch führen
kurze Lastspitzen nicht zu sichtbarem Profilflattern.

Profilwechsel arbeiten ohne Heapallokation, invalidieren betroffene Caches,
erzwingen einen vollständigen Folgeframe und erhalten Theme, Farben, Layout,
Controls, Navigation, Eingabe und Sicherheitsstatus. Unter
„Darstellung → Grafikqualität …“ sind alle sechs Modi per Tastatur und Maus
auswählbar und werden atomar in der Bootkonfiguration gespeichert. Hosttests
decken alle Profile, Autoauswahl, Dauerlast, Erholung, Speichermangel,
GPU-Ausfall, Software-Rendering, feste Benutzerwahl und ungültige Telemetrie
ab. Der UEFI-Selbsttest meldet `UEFI:ADAPTIVE-QUALITY-READY`; der interaktive
QEMU-Test öffnet das Untermenü, wählt Safe und weist
`UEFI:QUALITY-SELECTION` sowie `UEFI:CONFIGURATION-COMMIT` nach.

Im aktuellen Firmwarepfad existieren noch keine echten CPU- oder
GPU-Auslastungssensoren. SVG-Rendering ist weiterhin normativ blockiert;
SVG- und Glyph-Rasterstufen bleiben daher zunächst durchgängige Policywerte,
nicht vollständig angebundene Cache-Implementierungen. Physische
Hardwareprofile, Installer/Self-Healing und BIOS-Parität fehlen ebenfalls.

## Low-End Hardware Profile

Das eigenständige LEHP erkennt einen Kern, höchstens 1 GHz, höchstens 32 MiB
für die Boot-UI, fehlende GPU sowie Recovery/Safe Mode. Es kann unabhängig
davon manuell aktiviert und deaktiviert werden. Aktivierung koordiniert das
AQM-Low-Profil, den Software-Renderer, Dirty Regions, Double Buffering,
ereignisgetriebenen Leerlauf, reduzierte pflichterhaltende Animationen,
vereinfachten Blur und maximal eine Schattenebene. Recovery und Safe Mode
verwenden innerhalb desselben Profils die Safe-Qualität.

Das Speicherprofil umfasst exakt 32 MiB. Davon entfallen 6 MiB auf Ressourcen,
je 2 MiB auf Glyphen, SVG und Controls sowie je 512 KiB auf Theme und
Animationen; die übrigen festen Managerbudgets und die Reserve werden
deterministisch berechnet. Nach dem Aufbau der Oberfläche wird der reguläre
Runtime-Heap gesperrt, während die vorallokierte Frame-Arena weiterarbeitet.
Die Diagnose meldet Profilstatus, Aktivierungsart, Softwarepfad, Ziel- und
Ist-FPS, Framezeit, Speicher, Cache sowie den Erhalt von Layout, Funktion und
Sicherheit.

Hosttests prüfen automatische und manuelle Aktivierung, moderne Hardware ohne
Autoaktivierung, sämtliche exakten Teilbudgets, Recovery→Safe und die
Runtime-Sperre. Das reale GPT/FAT32-Abbild aktiviert das Profil im
GPU-unabhängigen Softwarepfad und meldet `UEFI:LOW-END-PROFILE-READY`, bevor
die vollständige Oberfläche und der Countdown erscheinen.

Nicht als erfüllt gewertet wird das reale 30-FPS-/80-ms-/150-ms-Ziel: Der
instrumentierte QEMU-Debugbuild rendert vollständig in Software und ist dafür
nicht repräsentativ. Ein optimierter Releasebuild auf physischer
1-GHz-/512-MiB-Hardware, HDD/USB-I/O-Tests, Touch, externe SVG-/Glyph-Caches,
Embedded-Systeme und BIOS-End-to-End-Parität bleiben offen.

## High-Quality Hardware Profile

Das HQHP besitzt eine eigene Hardware- und Policy-API. Automatische
Aktivierung verlangt mindestens vier Kerne, 8 GiB System-RAM, 256 MiB freien
Boot-UI-Speicher, einen schnellen Datenträger und vor allem ein tatsächlich
registriertes GPU-Backend. Ohne dieses Backend wird Hardwarebeschleunigung
nicht behauptet. Bei Aktivierung koordiniert das Profil Ultra-Qualität,
hochwertiges Antialiasing und Sampling, vollständige Motion Tokens,
erweiterten Blur, mehrstufige Schatten, Transparenz, Layer-/Resource-Caching,
Dirty Regions und Double Buffering. Die barrierefreie Einstellung
„Reduzierte Bewegung“ behält dabei immer Vorrang.

Das feste Budget beträgt 256 MiB: Ressourcen erhalten 64 MiB, Glyphen und SVG
je 16 MiB, Theme 4 MiB, Animationen und Controls je 16 MiB. Rendering, Layout,
Diagnose und Reserve vervollständigen das Budget deterministisch. Die
Runtime-Heapsperre bleibt auch bei Profilwechseln erhalten. Diagnosewerte
umfassen GPU-Backend, Beschleunigungsstatus und -Last, FPS, Framezeit,
Speicher, Cache, Materialmerkmale und Fallbackstatus.

Hosttests verwenden einen explizit simulierten GPU-Backendvertrag und prüfen
Erkennung, Ultra, sämtliche Budgets, 60-FPS-Diagnose, Reduced Motion und einen
GPU-Ausfall mit vollständigem Software-Fallback. QEMU stellt in der gewählten
Konfiguration kein NovaOS-GPU-Backend bereit; das reale GPT/FAT32-Abbild weist
daher korrekt `UEFI:HIGH-QUALITY-FALLBACK-READY` nach und zeichnet die
Oberfläche weiterhin vollständig per Software.

Nicht implementiert und deshalb nicht als erfüllt markiert sind echte
GPU-Beschleunigung für Compositing, Blur, Alpha, Skalierung und Vektoren sowie
ein physischer 60-FPS-/4K-/Ultra-Wide-/NVMe-Nachweis. Subpixeltext, externe
SVG-/Glyph-Caches, Touch, Präzisions-Touchpad, Stift und BIOS-Parität bleiben
ebenfalls offen.

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

`NPSPEC-BOOTRENDER-0012_Alpha_Compositing.md` enthält nicht die angekündigte
Alpha-Compositing-Spezifikation, sondern ist inhaltlich eine vollständige
Kopie von `NPSPEC-BOOTRENDER-0011_2D_Transformations.md`. Auch Dokument-ID und
Titel im Inhalt lauten `NPSPEC-BOOTRENDER-0011` und `2D Transformations`; es
gibt darin keinen Alpha-Compositing-Vertrag. Ohne Festlegung von Farbmodell,
Straight oder Premultiplied Alpha, Operatoren, Rundungsreihenfolge, API und
Akzeptanztests wäre eine behauptete 0012-Konformität erfunden. Das bereits
vorhandene Compositor-Blending bleibt nutzbar, gilt aber nicht als Nachweis
dieser fehlenden Spezifikation.

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
URI-IDs. Ein 128er Objektpool und ein 256er Open-Addressing-Hashindex liefern
deterministischen O(1)-Lookup ohne Heap. Das Ressourcenmodell speichert Name,
ID, Typ, Version, Größe, CRC32, Herkunft, Priorität, Zustand, Datenadresse,
Referenzen, Fallback und bis zu acht Abhängigkeiten. CRC32 wird vor der
Registrierung und erneut vor jeder Nutzung geprüft.

Der Loader unterstützt Lazy Loading, synchrones Preload und die Reihenfolge
Critical, High, Normal, Low. Bereits geladene Ressourcen werden aus dem Cache
referenziert. Abhängigkeiten werden transitiv geladen und beim letzten Release
transitiv freigegeben; ein fester Visiting-Bitset erkennt Zyklen. Das feste
Cachebudget unterstützt LRU, LFU und permanente Einträge. Minimum, Maximum,
Reserve und kritischer Bereich sind systemweit validiert; bei Druck werden
zunächst LRU- und danach LFU-Kandidaten nach Priorität, Nutzungszeit und
Zugriffshäufigkeit deterministisch entfernt. Aktive Referenzen sowie
Permanent-Einträge bleiben geschützt. Unload, Manager-Shutdown, explizite
Bereinigung, ungültige Zustände und das
optionale, noch nicht implementierte Background Loading besitzen eindeutige
Ergebnisse. Fehlende oder nach Registrierung beschädigte Ressourcen wechseln
nur auf einen erneut verifizierten Default-Fallback.

Font, Icons, Themes, Designmanifest und NovaOS-Logo werden zentral registriert
und vor dem ersten UI-Frame bis Priorität Normal preloaded. Hosttests decken
Lazy/Preload, Cache, Referenzen, transitive Abhängigkeiten, Zyklen, CRC-Fallback,
kleines Budget mit LRU, LFU-Zugriffshäufigkeit, permanente Ressourcen,
Grenzwerte, Bereinigung, Busy-Unload und Shutdown ab. Standardschrift, alle
15 Systemicons und das Logo sind permanent; das aktive Theme bleibt über
seine Lebensdauer referenziert. OVMF/QEMU meldet RESOURCE-LOADER-READY,
RESOURCE-CACHE-READY und RESOURCE-PRELOAD-READY. Externe BAP-Dateien,
Packageheader, Indexdeskriptoren, Offsets und Signaturen bleiben blockiert,
solange NPSPEC-BOOTRESOURCE-0002/0003 kein normatives Wire-Format definieren.

## Resource Versioning

NPSPEC-BOOTRESOURCE-0013 ist im UEFI-Ressourcenpfad als eigene heapfreie
Multi-Version-Registry umgesetzt. 32 feste Einträge und ein 64-Slot-
Open-Addressing-Index trennen logische Ressourcen-IDs von ihren physischen
Backing-Ressourcen. Damit können mehrere Versionen parallel vorhanden sein,
ohne den Duplikat- oder Integritätsschutz des Resource Managers aufzuweichen.

Jeder Eintrag führt `Major.Minor.Patch.Build`, Mindest-/Maximalversion und bis
zu acht versionierte Mindestabhängigkeiten. Unterschiedliche Major-Versionen
sind inkompatibel, Minor und Patch werden abwärtskompatibel geprüft. Die
Buildnummer beeinflusst entsprechend der Spezifikation nicht die
Kompatibilität, dient aber als reproduzierbarer Tie-Breaker. Die Auswahl nimmt
deterministisch die höchste kompatible Version. Abhängigkeiten werden transitiv
mit Zyklenschutz geprüft; Auswahl und Rollback stellen bei jedem Konflikt den
vorherigen aktiven Zustand atomar wieder her.

Der Hosttest deckt Major, Minor, Patch, Build, Bereiche, parallele Versionen,
Duplikate, Mindestabhängigkeiten, Konflikte und Rollback ab. Das aktuelle
GPT/FAT32-Image führt unter OVMF/QEMU eine versionierte Theme-/Font-Auswahl und
einen Rollback aus und meldet `UEFI:RESOURCE-VERSIONING-READY`.

Offen bleiben externe Paketmanifeste, die Prüfung manipulierter
Manifestversionsfelder und paketübergreifend vorbereitete Tabellen, weil
BOOTRESOURCE-0002/0003 weiterhin kein normatives BAP-/Index-Wire-Format
festlegen. Die getrennte BIOS-Runtime verwendet die Registry ebenfalls noch
nicht.

## Asset-Build-Pipeline

Der technisch unabhängig vom noch undefinierten Paketformat umsetzbare Teil
von NPSPEC-BOOTRESOURCE-0014 ist als automatisches Buildzeit-Gate vorhanden.
`validate-boot-assets.ps1` scannt Ressourcen rekursiv in kanonischer
Pfadreihenfolge und erkennt PNG, SVG, TTF, OTF, Theme, Sprache, Animation,
Cursor und Binärdaten. Unbekannte oder leere Dateien brechen den Build ab.
PNG-Signatur, IHDR und Dimensionen, sicheres SVG-XML ohne DTD, striktes UTF-8
sowie die definierten Formatheader werden vor jeder weiteren Verwendung
geprüft.

Für jede gültige Quelle entstehen deterministisch eine `boot://`-URI, dieselbe
FNV-1a-64-ID wie in der Laufzeit, vollständige Major.Minor.Patch.Build-
Metadaten, Typ, Größe und SHA-256. Der stabile Report enthält weder Zeitstempel
noch Zufallswerte. Debug, Development und Release sind explizite Profile;
doppelte IDs und beschädigte Quellen sind harte Fehler. `make
asset-pipeline-check` erzeugt aus denselben Quellen zweimal byteidentische
Reports und prüft zusätzlich, dass ein beschädigtes PNG sicher abgewiesen wird.

Eine vollständige BAP-Datei darf noch nicht erzeugt werden: BOOTRESOURCE-0002
und -0003 definieren weder Headerpackung, Offsets, Endianness noch das
Prüfsummenverfahren für Manifest und Index. BOOTSECURITY-0001 liefert zudem
keinen verwendbaren Schlüssel-/Signaturcontainer für Ed25519 oder ECDSA P-256.
Damit bleiben Manifest-/Index-/Paketbytes, Paketkompression und Signierung
ausdrücklich blockiert. Die Pipeline läuft nur beim Build und benötigt deshalb
keinen QEMU-Laufzeitmarker; das resultierende UEFI-Image wird weiterhin separat
unter OVMF/QEMU geprüft.

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

## Semantische Typography-, Spacing- und Radius-Tokens

BOOTDESIGN-0004 bis -0006 besitzen nun eine gemeinsame, heapfreie und pro
Theme vorbereitete Tokenebene. Die Typography-Registry enthält alle elf
Pflichtrollen von Display bis Monospace. Jede Rolle führt Font-ID, Größe,
minimale, normale und maximale Zeilenhöhe, echten Schriftschnitt sowie
semantisches und numerisches Letter Spacing. Die Font-IDs zeigen auf die
zentrale Font-Resource-/Fallbackregistry; ungültige Zeilenhöhen oder fehlende
Fonts verhindern die Designaktivierung.

Spacing stellt alle acht DLU-Rollen XXS bis XXXL bereit und validiert ihre
monotone Hierarchie. Radius enthält None, Tiny, Small, Medium, Large, XLarge,
Pill und Circle. Dark und Light teilen das reguläre Profil, High Contrast
vergrößert Typografie und Abstände und vereinfacht Radien. Ein Themewechsel
tauscht die vorbereiteten Tabellen O(1) aus, ohne Controls neu zu erzeugen.
Zentrale Control-, Listen-, Badge- und Panelstyles verwenden Padding und
Radien bereits produktiv über die semantischen Resolver.

Der Hosttest prüft sämtliche 27 Rollen, Zeilenhöheninvarianten, Reihenfolge und
Theme-/Accessibilitywechsel. Das GPT/FAT32-Image validiert die Tabellen vor dem
ersten Frame und meldet unter OVMF/QEMU
`UEFI:SEMANTIC-DESIGN-TOKENS-READY`. Noch zu migrieren sind lokale DLU-/Scale-
Werte einiger spezialisierter Renderer; vollständige Unicodefonts, ein eigenes
Touchprofil und identische Nutzung in der BIOS-Runtime fehlen ebenfalls.

## Elevation, Glass und Motion Tokens

BOOTDESIGN-0007, -0008 und -0010 sind als weitere statische Themeprofile in
die Designruntime integriert. Sechs Elevationsebenen besitzen jeweils ein
Shadow Token mit bis zu zwei Lagen aus DLU-X/Y, Blur, Deckkraft und Farbe.
Dark und Light verwenden abgestufte weiche Schatten; High Contrast deaktiviert
Tiefeneffekte, ohne die semantischen Ebenen zu entfernen. Der Dialogrenderer
bezieht seine beiden Schattenlagen produktiv ausschließlich aus Level4.

Die Materialregistry enthält None, Surface, Glass, Glass.Light, Glass.Medium,
Glass.Strong, Overlay und Dialog. Jede Rolle führt Oberflächen-/Rahmenfarbe,
DLU-Blur, Alpha, Reflexion, Helligkeit, Elevation und Opaque-Fallback. Dialog
und Overlay beziehen Farbe, Rand, Blur und Transparenz aus diesen Tokens und
verwenden den bestehenden cachefähigen Software-Blur-/Compositorpfad. High
Contrast ersetzt sämtliche Glasrollen deterministisch durch opake Flächen.

Motion stellt sechs Dauerklassen, acht Easingrollen und sieben Übergangsprofile
für Page, Dialog, Menu, Tooltip, Card, Focus und Selection bereit. Dialog
Enter/Exit, Seitennavigation, Fokusring und Fortschrittsänderung verwenden die
Resolver statt lokaler Dauern und Easings. Reduced Motion und High Contrast
liefern einen linearen 1-ms-Endzustand bei unveränderten Rollennamen.

Hosttests prüfen Vollständigkeit, Parameter und Accessibility-Fallbacks. Das
reale GPT/FAT32-Image validiert die Tabellen vor dem ersten Frame und meldet
unter OVMF/QEMU `UEFI:EFFECT-MOTION-TOKENS-READY`. Noch offen sind separate
Reflection-/Brightness-Passes, die Migration einzelner älterer
Spezialanimationen und die identische Tokenverwendung im BIOS-Pfad.

## Theme- und Accessibility-Runtime

Der UEFI-Pfad besitzt drei vollständige, statische Theme-Tabellen für Dark,
Light und High Contrast. Hintergrund, Flächen, Primär-/Sekundärtext, Akzent,
Fokus, Auswahl, Rahmen, Disabled- und Statusfarben sowie Radius-, Abstands-,
Transparenz- und Accessibilitywerte werden ausschließlich über semantische
O(1)-Tokens aufgelöst. Die Tabellen werden vor Aktivierung auf Vollständigkeit
und Mindesthelligkeitskontrast geprüft und als Theme-Ressourcen registriert.
Die heapfreie Registry führt ID, Ressourcen-ID, Name, Autor, Theme- und
Ressourcenversion, 22 semantische Tokens, Ladezustand und Referenzen. Ein
fester Open-Addressing-Index ermöglicht die geforderte O(1)-Auflösung.

Ein Wechsel lädt und validiert zunächst die vollständige Zielressource. Erst
danach wird der aktive Eintrag atomar umgeschaltet; die bisherige Ressource
wird anschließend freigegeben. Dadurch bleibt genau ein Theme aktiv und ein
ungültiges Ziel verändert den sichtbaren Zustand nicht. Wiederholte Auswahl
nutzt den Resource-Cache ohne Referenzleck. Farben, Materialfallback und
Reduced Motion einschließlich der Animationsregistry werden ohne Neustart
gemeinsam aktualisiert.

„Darstellung“ in den Einstellungen wechselt unmittelbar und ohne neue
Controls zwischen allen drei Themes. High Contrast erzwingt opake Materialien,
Reduced Motion, einen dauerhaft sichtbaren gelben Fokus und einen weißen
Kartenrahmen. Der separate Accessibility-Eintrag schaltet Reduced Motion auch
in Dark und Light. `test-uefi-themes` aktiviert alle drei Varianten in QEMU,
prüft die Debugmarker und erzeugt visuelle Light-/High-Contrast-Referenzframes.
Der Hosttest prüft zusätzlich Metadaten, Hash-Lookup, Cache, Referenzschutz und
Rollback. Das aktuelle GPT/FAT32-Image durchläuft den produktiven Pfad unter
OVMF/QEMU und meldet `UEFI:THEME-RESOURCE-READY`.

Offen bleiben externe Themepakete und darin deklarierte Font-, Icon-,
Animations- und Layoutabhängigkeiten, weil BOOTRESOURCE-0002/0003 weiterhin
kein normatives BAP-/Index-Wire-Format festlegen. Ebenso fehlt die gemeinsame
Theme-Registry im platzbeschränkten BIOS-Pfad.

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
ist jetzt zentral implementiert. Die feste Konfiguration trägt Version 1.2.0,
Theme, Qualitätsprofil, Skalierung, Motion, Tooltips, Tooltip-Verzögerung,
eine validierte Doppelklickzeit von 200 bis 1000 ms, eine lineare
Mausgeschwindigkeit von 25 bis 400 Prozent sowie Recovery-, Watchdog-,
Eingabe- und Debugwerte. Sichere Defaults funktionieren
ohne externe Datei; jeder Wert besitzt einen geprüften Bereich und die
Runtime-Struktur eine deterministische interne Integritätsprüfsumme.

Änderungen laufen ausschließlich über Begin, Set und Commit. Erst eine
vollständig gültige Staging-Konfiguration ersetzt atomar den aktuellen Stand;
andernfalls bleibt dieser unverändert und die Transaktion wird als Rollback
gezählt. Explizites Rollback, Default-Reset, Generation, Änderungsmeldungen und
temporäre Overrides sind heapfrei implementiert. Overrides verändern die
Basiswerte nicht und können gemeinsam entfernt werden.

Themeauswahl, Reduced Motion, Tooltip-Schalter, Tooltip-Verzögerung,
Doppelklickerkennung und die produktive UEFI-Mausbewegung sind an diesen
Manager angebunden. Bei 100 Prozent bleibt die bisherige Firmware-
Normalisierung `/8` exakt erhalten; 25 und 400 Prozent bilden die validierten
Grenzen, und sehr kleine Rohbewegungen bleiben mindestens ein Pixel. Eine
nichtlineare Beschleunigungskurve ist nicht normativ festgelegt und wird daher
nicht erfunden. Die bestehenden QEMU-Tests für Settings und Themes
weisen jeden produktiven Commit mit `UEFI:CONFIGURATION-COMMIT` nach und zeigen
den finalen Zustand in `build/uefi-settings-controls.ppm`. Hosttests prüfen
zusätzlich verschachtelte Transaktionen, ungültige Werte, Rollback, Reset,
Override-Isolation, eine beschädigte Prüfsumme und die Pointerwerte 25, 100 und
400 Prozent. Der GPT/FAT32-QEMU-Test fordert `POINTER-SPEED-READY`.

## Productive Pointer Capture

NPSPEC-BOOTINPUT-0007 verwendet jetzt eine direkt per Pointer-ID adressierte
feste Acht-Slot-Tabelle. Begin, Owner-Abfrage, Status, Routing, Release und
Cancel sind damit O(1). Doppel-Capture, ungültige IDs sowie Null- oder
zerstörte Controls werden abgewiesen und diagnostisch gezählt. Capture hat
Vorrang vor Hit Testing; Geräteverlust und Seitenwechsel brechen aktive
Captures ab.

Der Tooltip-Delay-Slider und die Diagnose-Scrollbar übernehmen Pointer 0 beim
Drag-Beginn. Dadurch erhalten sie Bewegungen auch außerhalb ihrer Bounds. Bei
Pointer-Up erfolgt `Released`, bei Geräte- oder Szenenverlust `Cancelled`.
Hosttests prüfen Exklusivität, Owner, Out-of-bounds-Routing, Release,
Invalid-ID und Hot-Removal-Cancel. Das reale GPT/FAT32-QEMU-Image muss
`POINTER-CAPTURE-READY` melden.

Jeder Slot führt zusätzlich Pointer-ID, letzten Owner, Begin, Ende, Dauer,
Release-, Cancel- und Fehlerzähler. Bei einem Abbruch wird vor dem Entfernen
des Owners ein `CaptureCancel`-Ereignis vom Target zum Parent geroutet. Der
Hosttest misst 25 ms für eine normale Freigabe und 40 ms für Geräteverlust;
QEMU validiert ein 45-ms-Cancel und fordert
`POINTER-CAPTURE-CANCEL-EVENT-READY`.

Dialogöffnung und Recovery-Eintritt rufen den zentralen Cancel-Pfad jetzt vor
dem Kontextwechsel auf. Host und QEMU prüfen dafür unabhängige Zeitwerte und
die geroutete Ereignisreihenfolge; die QEMU-Gates heißen
`DIALOG-CAPTURE-CANCEL-READY` und `RECOVERY-CAPTURE-CANCEL-READY`.

Noch offen sind Touch/Pen, BIOS-Parität,
automatisierte HMP-Mausinjektion und physische Hardware. `Pending` bleibt für
zukünftige asynchrone Quellen reserviert; die aktuelle Bootlaufzeit entscheidet
Capture-Anforderungen synchron.

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

NPSPEC-BOOTTEST-0006 wird durch eine vorallokierte 80er Testregistry ergänzt.
Der Hostlauf prüft 65 isolierte Kombinationen: zehn Pflichtauflösungen sowie
5:4, 21:9 und 32:9 jeweils bei 100, 125, 150, 175 und 200 Prozent. Validiert
werden Auflösungs- und Seitenverhältnisklasse, Safe Area, logische DLU-Fläche,
Layout, Renderfähigkeit, erreichbare Controls, Dialog/Text und die vollständige
Wiederherstellung der aktiven Konfiguration. EDK2/QEMU erzeugt zusätzlich
echte P6-Framebuffer in 800x600, 1280x720 und 1920x1080. Der automatisierte
Bericht prüft deren Abmessungen und protokolliert SHA-256-Werte in
`build/resolution-compatibility-report.md`.

Nicht als vollständig erfüllt gelten reale GOP-Aufnahmen aller Pflicht- und
Ultra-Wide-/4K-Modi, ein Difference Analyzer mit normativ bezifferter
Pixelbaseline und Toleranz, vollständige per-Control-Clipping-/Textüberlauf-
Introspektion, sämtliche Themes je Matrixfall, Installer/Self-Healing,
physische Hardware sowie BIOS-Parität.

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

## Buffering and Frame Presentation

NPSPEC-BOOTRENDER-0008 besitzt nun einen eigenen, plattformneutralen Present
Scheduler zwischen Compositor und Framebuffer Backend. Der produktive UEFI-Pfad
verwendet Double Buffering: Die vollständige Szene entsteht ausschließlich im
unsichtbaren Compositor-Backbuffer und gelangt erst in der Presentphase in den
firmwareabhängigen Frontbuffer. Ein globaler Lock sowie explizite Zustände
Free, Rendering, Ready, Presenting und Displayed verhindern paralleles Rendern,
Freigeben oder Präsentieren.

Der Scheduler verwaltet bis zu 64 validierte Dirty Regions, vereinigt
Überlappungen und wählt bei der ersten Darstellung, erzwungener Invalidierung,
mehr als 60 Prozent geänderter Fläche oder einer Backendgrenze automatisch ein
Full Present. Andernfalls wird ausschließlich die vereinigte Teilmenge
kopiert. Optionales Triple Buffering ist implementiert, wird aber nur
aktiviert, wenn der Aufrufer bereits beim Start einen ausreichend großen
Pending Buffer und das zugehörige Speicherbudget bereitstellt; im normalen
Double-Buffer-Pfad belegt er deshalb keinen zusätzlichen Vollbildspeicher.

Diagnosewerte erfassen Frames, Bufferzustände, Full/Partial/Forced Presents,
Dirty-Merges, Lockkonflikte, kopierte Pixel, Double-/Triple-Frames, Frame-,
Render- und Presentdauer sowie VSync- und Pageflip-Strategie. Scheitert ein
Present, versucht der Scheduler genau einmal ein Full Present. Ein zweiter
Fehler löst den Lock, verwirft den Frame kontrolliert und meldet dem zentralen
Recovery Manager den Renderingfehler für Safe Mode.

Hosttests belegen Double und Triple Buffer, illegale Zustandswechsel, Present
Lock, Full/Partial Present, Dirty Merge, Zeitmessung und die vollständige
Recovery-Eskalation. Der echte OVMF/QEMU-Pfad meldet
UEFI:PRESENT-SCHEDULER-READY und präsentiert im Countdown-Start 15 vollständige
Runtimeframes.

Offen bleiben die Anbindung an den separaten BIOS-LFB-Renderer, echtes
firmware- oder hardwaregestütztes VSync und Page Flipping, ein produktiv
aktiviertes Triple-Buffer-Profil, hochauflösende Hardwarezeitmessung sowie
Tearing- und Bare-Metal-Nachweise. Daher bleibt die Spezifikation teilweise
integriert.

## Dirty Region Rendering

NPSPEC-BOOTRENDER-0009 besitzt jetzt einen zentralen, heapfreien Dirty Manager.
Kapazität bis 64 Regionen und Flächenschwellwert sind beim Start
konfigurierbar; der produktive Compositor verwendet 64 Regionen und schaltet
ab 50 Prozent der Bildschirmfläche auf Full Damage. Leere oder vollständig
außerhalb liegende Rechtecke werden verworfen, teilweise sichtbare Rechtecke
overflow-fest geclippt und identische Einträge dedupliziert. Überlappungen
werden transitiv vereinigt, sodass auch eine neue Region, die zwei vorhandene
Gruppen verbindet, genau eine Union erzeugt.

Kapazitätsüberlauf, Initialisierung, Auflösungs- und Themewechsel, Recovery,
Speicherfehler und explizite Invalidierung besitzen getrennte Full-Damage-
Gründe. Für Effekte kann eine Region um Blur-Radius, Shadow-Extent und
Alpha-Nachbarschaft erweitert werden. Der Manager bietet außerdem eine
Layer-Schnittprüfung. Diagnosewerte enthalten Additions, Ablehnungen,
Duplikate, Merges, Clips, Erweiterungen, Full-/Partial-Entscheidungen,
Flächenmittelgrundlage und maximale Einzelfläche.

Der Compositor führt seine bisherigen lokalen Present-Regionen nicht mehr
parallel: Er speist den zentralen Manager und reicht dessen unverändertes
Damage Set an den Present Scheduler weiter. Hosttests decken einzelne,
mehrere, überlappende, identische und geclippte Regionen sowie Threshold,
Kapazität, Blur/Shadow/Alpha-Erweiterung und Integer-Overflow ab. OVMF/QEMU
meldet UEFI:DIRTY-MANAGER-READY und präsentiert das aktuelle GPT/FAT32-Image
weiterhin erfolgreich.

Offen bleibt eine feinere produktive Invalidierung: Die aktuelle UI zeichnet
viele vollständige Layer pro Frame neu, statt bei Controls, Animationen und
Mausbewegungen konsequent nur alte und neue Bounds einzureichen. Ebenso fehlen
die identische BIOS-Anbindung und reale Mikrosekunden-/Bare-Metal-Messungen.
Die Spezifikation bleibt deshalb teilweise integriert.

## Clipping and Masks

NPSPEC-BOOTRENDER-0010 besitzt nun einen zentralen, heapfreien Clip- und
Mask-Manager. Jeder Frame beginnt mit genau einem Rootclip über die gesamte
logische Ausgabe. Bis zu 32 verschachtelte Pushes erzeugen aus Eltern- und
Kindrechteck unveränderliche effektive Scopes; leere Schnitte bleiben als
gültige, vollständig unsichtbare Scopes erhalten. Inside, Partial und Outside
werden vor der Rasterisierung unterschieden. Root-Pop, Stacküberlauf,
ungültige Rechtecke und veraltete Scope-IDs werden kontrolliert abgewiesen.

Die feste Registry verwaltet 64 generationensichere Masken der Typen Rectangle,
1-Bit-Bitmap, 8-Bit-Alpha, Vector Callback und Rounded Rectangle. Bis zu 32
aktive Masken werden in Reihenfolge durch Alpha-Multiplikation kombiniert.
Bitmap- und Alphamasken prüfen Pointer, Stride und Gesamtgröße,
Rounded-Masken ihren Radius, Vektormasken rekursive Callback-Aufrufe.
Aktive Masken können nicht zerstört oder erneut rekursiv gepusht werden. Ein
fester 256er Coverage-Cache vermeidet wiederholte statische Maskenberechnung.

Render Commands speichern den wirksamen Clip- und Maskscope bereits beim
Submit und bleiben dadurch trotz späterem Pop unveränderlich. Clear, Rechteck,
Linie und Kreis führen zuerst den Bounding-Box-Test aus, verwerfen vollständig
unsichtbare Befehle und schreiben keinen Pixel außerhalb von Clip, Surface
oder Maske. Teiltransparente Masken skalieren die Quellalpha; das eigentliche
Blending bleibt korrekt in der nachfolgenden Compositorphase.

Hosttests belegen Root-, Dialog-, Scroll- und verschachtelte Clips,
vollständiges und partielles Clipping, alle fünf Masktypen, Maskkombination,
Cache, Rekursionsschutz, Lifecycle und einen tatsächlich nur mit Alpha 128
geschriebenen Rasterpixel. Das aktuelle GPT/FAT32-Image meldet unter OVMF/QEMU
UEFI:CLIP-MASK-READY und präsentiert weiterhin alle Countdownframes.

Offen bleibt die vollständige Migration der älteren direkten Text-, Icon- und
Control-Rasterizer auf Render Commands. Dadurch verwenden produktive Dialog-
und Scrollinhalte den neuen Scopevertrag noch nicht lückenlos. Ellipse,
Polygon und Bézier sind ebenfalls noch keine Render-Command-Primitive;
BIOS-Parität und SIMD-Optimierung fehlen. Die beiden contentReference-
Platzhalter in den Abschnitten 25 und 33 der Draft-NPSPEC sind redaktionell
ungültig, blockieren aber keine der technisch auslegbaren Anforderungen.

## 2D Transformations

NPSPEC-BOOTRENDER-0011 besitzt nun eine gemeinsame affine 3×2-API. Die
öffentliche Schnittstelle verwendet wie spezifiziert Floatwerte; vor dem
Render-Hotpath wird jede Matrix einmal validiert und deterministisch in 16.16
überführt. Identity, Translation, Rotation im Bogenmaß, uniforme und
nichtuniforme Skalierung, negative Spiegelung, X-/Y-Shear, Pivot,
Matrixmultiplikation, Inversion, Punktabbildung und Four-Corner-Bounding-Box
sind implementiert. Operationen werden per Pre-Multiplikation in der
vorgeschriebenen Reihenfolge Scale, Rotation, Translation aufgebaut.

Die freestanding Runtime benötigt keine libm-Abhängigkeit. Sinus und Cosinus
verwenden Winkelreduktion und eine feste Polynomauswertung; 0°, 90°, 180° und
270° werden auf exakte Kardinalwerte eingerastet. NaN, Infinity, übergroße
Komponenten, singuläre Fixed-Matrizen, Rechenüberläufe sowie Stack-Over- und
Underflow werden abgewiesen. Ein fester 32er Transformationsstack berechnet
Parent × Local zu World.

Scene Graph und Layer Manager verwenden nicht mehr zwei eigene ungeprüfte
Multiplikationen. Beide teilen die zentrale 16.16-Implementierung; insbesondere
ist eine gültige 90°-Matrix mit m11 und m22 gleich null jetzt zulässig.
World Bounds entstehen aus allen vier transformierten Ecken und stimmen damit
auch bei Rotation, Scale, Mirror und Shear. Layouttranslation wird vor der
Elternmultiplikation korrekt Bestandteil der lokalen Matrix.

Jeder Render Command speichert beim Submit seine Worldmatrix. Bounding Box und
Clipping werden erst nach der Transformation berechnet. Linien transformieren
beide Endpunkte; Rechtecke und Kreise werden über eine geprüfte inverse Matrix
in den lokalen Raum zurückprojiziert. Der Hosttest weist unter anderem nach,
dass ein lokales 1×1-Rechteck nach Scale 2 und Translation 2 exakt die
Bildschirmpixel zwei und drei schreibt. Weitere Tests decken S→R→T, Pivot,
Mirror, Shear, Inverse Roundtrip, Float→Fixed, Stackschutz und rotierte
Scene-Graph-Bounds ab. OVMF/QEMU meldet UEFI:TRANSFORM2D-READY.

Offen bleiben die Migration der älteren direkten Text-, Icon-, Image- und
Control-Rasterizer auf Commands, die automatische Einspeisung alter und neuer
Bounds in den produktiven Dirty Manager, sichtbare Rotation-/Scale-Animationen
im normalen Bootflow, BIOS-Parität, SIMD und echte Mikrosekundenmessungen.
Der contentReference-Platzhalter in Abschnitt 1 der Draft-NPSPEC ist
redaktionell ungültig, aber kein technischer Implementierungsblocker.

## Rounded Geometry

`NPSPEC-BOOTRENDER-0014` besitzt nun ein gemeinsames, heapfreies mathematisches
Rounded-Geometry-Modul. Es unterstützt uniforme und vier individuelle
Eckradien, begrenzt sie automatisch auf `min(Breite, Höhe) / 2`, skaliert
Geometrie und Radien deterministisch in 16.16-DLU und interpoliert Position,
Größe und Radien für Animationen. Das Hit-Testing prüft die tatsächliche
Kontur statt nur der Bounding Box.

Der analytische Kreisgleichungstest rastert ohne Polygon- oder Bitmapmaske.
Die Qualitätsstufen bieten abgeschaltetes AA sowie deterministisches 2×2- und
4×4-Kanten-AA. Innen-, zentrierte und Außenrahmen werden als Differenz zweier
gerundeter Konturen erzeugt. Alle Schreibzugriffe werden an Surfacegrenzen
geclippt; Diagnosen erfassen Erzeugung, Rendering, Hit Tests, Ablehnungen,
Radien und AA-Status.

Controls und Bootmanager-Panels verwenden produktiv diesen Renderer. Damit
sind insbesondere Buttons, Karten, Dialogflächen, Schalter, Eingabefelder und
der obere Balken nicht mehr von zwei separaten ganzzahligen Scanline-Helfern
abhängig. Zusätzlich speichert ein eigener Rounded-Rectangle-Render-Command
Einzelradien und Worldtransform unveränderlich; die inverse Rasterisierung
unterstützt Translation, Rotation, Scale und Mirror und läuft durch die
zentralen Clip- und Maskscopes.

Hosttests decken Radius null, Maximalradius, Einzelradien, AA-Kantencoverage,
DLU-Skalierung, Parameteranimation, alle Rahmenmodi, formgenaues Hit Testing
und transformiertes/geclipptes Command-Rendering ab. Das neu erzeugte echte
GPT/FAT32-Image bootet unter OVMF/QEMU, meldet
`UEFI:ROUNDED-GEOMETRY-READY` und präsentiert alle Countdownframes.

Nicht als erfüllt gelten Gradient-, Blur- und Schattenkonturintegration sowie
identische BIOS-Pixelresultate. Die von 0014 vorausgesetzte
`NPSPEC-BOOTRENDER-0013` (Gradients) existiert nicht als Datei und 0012 enthält
statt Alpha Compositing eine Kopie von 0011. Diese fehlenden Vorverträge werden
nicht durch proprietäre Annahmen ersetzt.

## Shadows and Glows

NPSPEC-BOOTRENDER-0015 besitzt nun ein gemeinsames heapfreies Effektsystem
mit einer festen Liste von bis zu acht geordneten Effekten je Renderobjekt.
Unterstützt werden Drop, Inner, Ambient und Contact Shadow sowie Outer, Inner,
Focus und Accent Glow. Der validierte Deskriptor enthält RGBA-Farbe, Radius,
Deckkraft und unabhängige positive oder negative X-/Y-Offsets. Clear sowie
Interpolation von Radius, Farbe, Deckkraft und Offset sind vorhanden.

Die Effektkontur wird direkt aus dem Rounded-Geometry-Objekt einschließlich
seiner vier individuellen Radien abgeleitet. Shadow- und Glowphase sind
getrennt, sodass die spezifizierte Reihenfolge Shadow, Fill, Border, Glow
aufgebaut werden kann. Low, Standard und High begrenzen den Arbeitsradius
deterministisch. Der Hotpath verwendet keine Heap-Allokation und verwirft
Pixel vor der Berechnung anhand von Surfacegrenzen und aktuellem Clip. Danach
wird die kombinierte Alpha-Maskcoverage angewandt und die vollständige
Effektregion an den Dirty Manager gemeldet.

Dialoge nutzen produktiv einen Ambient Shadow und einen nach unten versetzten
Drop Shadow entlang ihrer tatsächlichen Rundung. High Contrast reduziert die
Effektqualität und zeichnet den Schatten nicht, damit die opake, klar
umrandete Accessibility-Darstellung erhalten bleibt. Diagnosen erfassen
Objekte, gesetzte/gelöschte Effekte, Shadow-/Glowanzahl, geschriebene Pixel,
Maximalradius, Cache-Misses, Qualität und Effekte pro Frame.

Hosttests decken alle acht Effekttypen, Listenreihenfolge und -kapazität,
negative Radien und andere ungültige Werte, Animation, Qualitätswechsel sowie
einen echten Alpha-Maskscope in Kombination mit hartem Clip ab. Das reale
GPT/FAT32-Image bootet unter OVMF/QEMU und meldet
UEFI:SHADOW-GLOW-READY zusammen mit allen Countdownframes.

Offen bleiben ein echter separabler Blur-Kernel, Effektcache und
Renderzeitmessung, die produktive Focus-/Hover-Glow-Motion für alle Controls,
die vollständige Anbindung jedes Renderobjekttyps sowie identische
BIOS-Pixelresultate. Außerdem fehlen weiterhin die normativen Vorverträge aus
0012 (falsch befüllt) und 0013 (keine Datei); deshalb wird 0015 nicht als
vollständig integriert ausgewiesen.

## Background Blur

NPSPEC-BOOTRENDER-0016 besitzt nun eine heapfreie Background-Blur-Engine.
Sie verlangt eine bereits fertig gerenderte Hintergrund-Surface und eine
getrennte Ziel-Surface; In-place-Aufrufe werden kontrolliert abgewiesen. Ein
Ring aus höchstens 65 horizontal gefilterten Zeilen und ein begrenzter
512×512-Ergebnis-Cache vermeiden zusätzliche Vollbildsurfaces. Unterstützt werden
Box, Gaussian und Dual-Pass Gaussian sowie Low, Standard und High. Radius,
Region, Surface, Stride, Kernel und Qualität werden vor jedem Zugriff geprüft.

Der Renderer kopiert ausschließlich die angeforderte Hintergrundregion,
berechnet horizontale und vertikale Pässe getrennt und schreibt erst danach.
Rounded Geometry mit vier Einzelradien und 4×4-AA sowie kombinierte
Rechteck-/Rounded-/Alpha-Maskscopes greifen nach dem Kernel. Die Zielregion
wird dem Damage-System gemeldet. Der Cache speichert für Regionen bis 512×512
Pixel das bereits fertig maskierte Ergebnis; Treffer sind reine Kopien ohne
erneute Kernel- oder Maskenauswertung. Größere Regionen werden korrekt
gerendert, aber nicht gecacht. Nur überlappende Invalidierungen verwerfen ihn.

Das angegebene Speicherbudget wird geprüft. Reicht es nicht für Snapshot und
Zeilenring, bleibt der Pfad funktionsfähig, degradiert aber automatisch auf
Low mit begrenztem Radius. Rekursive Renderaufrufe, negative oder unendliche
Radien, ungültige Regionen, doppelte Initialisierungspfade und Zugriffe nach
Shutdown werden kontrolliert behandelt. Diagnosen erfassen Regionen,
Kopier-/Blurpixel, Cache Hits/Misses, Invalidierungen, Kernel, Radius,
Qualität, Speicher und Degradation.

Dialog-Glassflächen sind im stabilen Dialog-Lifecycle an diesen Pfad
angebunden; Enter-/Exit-Motion deaktiviert teure Effekte explizit und High
Contrast bleibt opak. Host-Pixeltests belegen Impulsverteilung,
Dual-Pass-Ausgabe, gerundete AA-Kante, Cache Hit/Miss, fremde und
überlappende Dirty Regions, Speichermangel sowie einen echten Alpha-Maskscope.
Eine erste Vollbildsnapshot-Variante vergrößerte das PE-Image auf rund
40,8 MiB und wurde von OVMF nicht mehr geladen. Nach der speichergerechten
Umstellung beträgt SizeOfImage rund 33,7 MiB. Der neu gebaute echte
GPT/FAT32-OVMF-Pfad startet wieder, meldet BACKGROUND-BLUR-READY und
präsentiert alle elf Countdownframes.

Der interaktive QEMU-Dialoglauf besitzt wegen des ersten softwareberechneten
Firmwareframes nun eine Frist von 60 statt 15 Sekunden. Er belegt zwei echte
BACKGROUND-BLUR-FRAME-Ausgaben, zunächst einen Cache-Miss und anschließend
einen Cache-Hit, einen stabilen Screenshot sowie Resume, Escape, Exit-Motion,
Cancel-Ergebnis und Seitenwiederherstellung. Teure Effekte sind während
Enter-/Exit-Motion explizit deaktiviert und nur im stabilen Lifecycle aktiv.

Offen bleiben SIMD, Tile-Parallelisierung, Hardwarezeitmessung, automatische
Layer-Dirty-Generationen, BIOS-Pixelparität und der fehlende normative
Alpha-Vertrag aus 0012.

## Image Rendering

NPSPEC-BOOTRENDER-0017 besitzt nun einen gemeinsamen Image Loader, Decoder,
Cache und Renderer. Ressourcen werden vor dem ersten Rendern vollständig in
internes premultiplied RGBA8888 konvertiert. Unterstützt sind RAW RGBA8888,
BGRA8888, RGB888 und RGB565, das interne unkomprimierte BMP24/32 sowie PNG mit
8-Bit Truecolor, RGBA oder Indexed Color einschließlich PLTE und tRNS. Der
PNG-Pfad validiert Signatur, Chunkgrenzen und CRC, Zlib-Header und Adler-32,
dekodiert Stored-, Fixed- und Dynamic-Deflate-Blöcke und rekonstruiert alle
fünf PNG-Filter. APNG und interlaced PNG werden entsprechend der NPSPEC
kontrolliert abgewiesen.

Ein fester 512-KiB-Pixelpool und ein 240-KiB-Dekodierbereich begrenzen den
Speicher ohne Allokation im Render-Hotpath. Ressourcen-IDs bilden den Cache;
wiederholtes Laden erhöht nur die Referenzzahl. Breite, Höhe, Stride,
Dateigröße, Speicherprodukte, Chunkanzahl, Format und sämtliche Decodergrenzen
werden vor Zugriffen geprüft. Der Renderer unterstützt Nearest und Bilinear,
Fit, Fill, Stretch und Originalgröße, Translation, Rotation, Skalierung,
Spiegelung, Clip-/Maskscopes, Opacity, Tint, beliebige Ziel-Surfaces und
Damage-Weitergabe. Ein schneller Identitäts-/Skalierungspfad vermeidet die
allgemeine inverse Transformation und Divisionen pro Pixel. Das sichtbare
Nova-Logo wird produktiv über diese API gezeichnet.

Hosttests belegen RAW-Konvertierung und Premultiplikation, Cache und
Referenzen, Nearest/Bilinear, Spiegelung, affine Transformation, Clipping,
BMP24 sowie RGBA-, RGB- und indizierte PNGs. Außerdem werden Stored und
komprimiertes Deflate, CRC-Schäden und nicht unterstütztes Interlacing
geprüft. Das reale GPT/FAT32-Image bootet unter OVMF/QEMU mit
IMAGE-RENDERER-READY; Menüframes sowie der vollständige Dialoglauf mit Blur,
Escape und Seitenwiederherstellung bestehen. Die QEMU-Fristen betragen wegen
der langsamen TCG-Softwaredarstellung 90 Sekunden für den initialen
Animations-/Countdownnachweis und 60 Sekunden für den Dialog-Exit.

Offen bleiben das Laden aus einem externen Boot Asset Package, weil dessen
normative BAP- und Index-Wire-Formate in BOOTRESOURCE-0002/0003 fehlen,
identische BIOS-Pixelresultate, SIMD, belastbare Decode-/Renderzeitmessungen,
größere Speichermangel-/Fuzzfälle und der normative Alpha-Compositing-Vertrag
aus der falsch befüllten NPSPEC-BOOTRENDER-0012.

## Rendering Quality Profiles

NPSPEC-BOOTRENDER-0018 besitzt nun einen zentralen heapfreien Quality Manager
mit den fünf normativen Profilen Ultra, High, Standard, Performance und Safe.
Jedes Profil besitzt eine unveränderliche, validierte Parametertabelle für
Blur-Stufe und Maximalradius, Shadow-/Glow-Qualität und Effektdichte,
Anti-Aliasing, Nearest/Bilinear, Animation, Transparenz, Dithering,
Speicherbudget und Compositor-Fallback. Standard ist der deterministische
Initialzustand. Explizite Konfiguration hat Vorrang; Auto berücksichtigt
Speicher, Firmwareeinschränkung und Software-Rendering.

Ein Profilwechsel setzt Blur, Effekte und deren maximale Anzahl, Rounded-AA,
Image-Sampling, Motion und Compositor gemeinsam. Der Blur-Cache wird geleert
und der Dirty Manager erzwingt ein vollständiges Re-Rendering. Safe schaltet
Blur, Schatten, Glows, Transparenz und Animation aus und verwendet den
minimalen Compositorpfad. Dialoge, Controls und Branding lesen die aktiven
Parameter produktiv, statt eigene Qualitätswerte fest einzubauen.

Die adaptive Steuerung degradiert bei Speichermangel sofort. Eine einzelne
langsame Darstellung löst dagegen keinen Wechsel aus; erst drei aufeinander
folgende Überschreitungen von 33,334 ms reduzieren in Auto die Stufe. Ein
Grafikfehler kann auf Performance, ein kritischer Fehler auf Safe eskalieren.
Alle Entscheidungen erfolgen bei Diagnose-/Fehlerereignissen außerhalb des
eigentlichen Pixel-Hotpaths. Diagnosen erfassen aktives und gewünschtes
Profil, Grund, Wechsel, Anwendungen, automatische Anpassungen, Full Redraws,
Cacheinvalidierungen, deaktivierte Effekte, Budgetverletzungen, Speicher und
Grafikdegradationen.

Hosttests belegen alle fünf Profile, Invalidwerte, Ultra-Parameter, Safe,
Hardware-Autoauswahl, dreistufige Frame-Hysterese, Speichermangel,
Cacheinvalidierung und Full Redraw. Das reale GPT/FAT32-Image meldet unter
OVMF/QEMU RENDER-QUALITY-READY und besteht den vollständigen Menüpfad sowie
Dialog, Blur-Cache, Escape und Seitenwiederherstellung.

Offen bleiben ein persistentes Konfigurations-Backend, echte GPU-/Firmware-
Leistungsinventarisierung, produktives Dithering und reduzierte
Renderauflösung, die dynamische Neudimensionierung bereits reservierter Pools,
automatische Hochstufung nach dauerhafter Erholung, identisches BIOS-Verhalten
und Bare-Metal-Performancewerte.

## Software Rendering Fallback

NPSPEC-BOOTRENDER-0019 besitzt nun eine gemeinsame Software-Renderer-
Lifecycle-API. Sie umfasst Initialisierung, Shutdown, Framebeginn,
Scene-Graph-Rendering, Present, Frameabschluss, Speicherbericht, Reset,
Verfügbarkeit und Textfallback-Anforderung. Der produktive UEFI-Bootmanager
durchläuft ausschließlich die CPU-Pipeline aus Render Commands, Scene Graph,
Software-Rasterizer, Compositor, Present Scheduler und Framebuffer Backend;
Firmware- oder GPU-Zeichenbeschleunigung wird nicht verwendet. Der portable
Scalarpfad ist die Referenz und benötigt weder SIMD noch mehrere Kerne.

Die Zustandsmaschine unterscheidet Offline, Active, Resetting, Safe und Text.
Der erste Render- oder Presentfehler verwirft einen eventuell offenen
Command-/Transform-/Clip-/Maskframe, leert den Blur-Cache und erzwingt ein
Full Repaint. Ein unmittelbar folgender Fehler aktiviert das vollständige
Safe-Qualitätsprofil. Ein dritter Folgefehler fordert den funktionalen
UEFI-Textmodus an; der Bootprozess wird dadurch nicht abgebrochen. Ein
erfolgreicher Frame setzt die Fehlerfolge zurück. Speichermangel unter
16 MiB deaktiviert teure Effekte über Safe, leert temporäre Caches und
erzwingt Full Damage.

Die Initialisierung validiert Auflösung, Framebuffer und Speicher. Fehlender
Framebuffer fordert direkt Text an. Doppelter Shutdown, Nutzung im falschen
Zustand, Null-/inaktive Szenen sowie Render-/Presentfehler sind typisiert.
Diagnosen erfassen aktiven Renderer und Scalar-/SIMD-Stufe, Frames, Presents,
Pixel, Draw Calls, Dirty Regions, Cache Hits/Misses, Speicher, Fehler, Resets,
Full Repaints, Safe-Eintritte und Textanforderungen.

Hosttests belegen erfolgreiche Scene-Graph-Traversierung, Frameabschluss,
Speichermangel, die vollständige Folge Reset → Full Repaint → Safe → Text,
fehlenden Framebuffer und doppelten Shutdown. Das reale GPT/FAT32-Image meldet
unter OVMF/QEMU SOFTWARE-RENDERER-READY und besteht Menü und Dialog. Ein eigener
interaktiver QEMU-Test öffnet die Diagnoseseite, injiziert mit F11 einen
Renderfehler und belegt SOFTWARE-RENDERER-RESET sowie
SOFTWARE-RENDERER-RECOVERED mit anschließend weiter gezeichneter GUI.

Der BIOS-Pfad rendert weiterhin vollständig per CPU direkt in den validierten
VBE-LFB und besitzt einen getesteten Textfallback, ist aber noch nicht an
dieselbe C-Pipeline gebunden. Offen bleiben daher identische BIOS-/UEFI-
Pixelresultate, SIMD-Laufzeitauswahl, vollständige Neuinitialisierung aller
persistent reservierten Surface-Pools und Bare-Metal-Fehlerinjektion.
Gradienten bleiben wegen der fehlenden NPSPEC-BOOTRENDER-0013 offen; der
normative Alpha-Vertrag in 0012 ist weiterhin falsch befüllt.

## Resource Compression

NPSPEC-BOOTRESOURCE-0005 ist jetzt in den zentralen Resource Loader integriert.
`compression.c` implementiert die verpflichtenden Verfahren NONE und LZ4 als
deterministischen O(n)-Raw-Blockdecoder. Token, erweiterte Literal- und
Matchlängen, Backreference-Offsets, Überlappungen, Quell- und Zielgrenzen sowie
Pointer- und Größenüberläufe werden vor jedem Zugriff geprüft. Das optionale
Zstd wird typisiert als nicht unterstützt abgewiesen.

Komprimierte Ressourcen verbleiben bis zum ersten Zugriff gepackt. Der Loader
prüft zunächst die CRC32 der gepackten Daten, dekomprimiert in einen statischen,
heapfreien 512-KiB-Blockpool und prüft danach Originalgröße und Original-CRC32.
Die dekodierte Kopie wird in denselben referenzgezählten Cache übernommen und
höchstens einmal erzeugt. Unter Pooldruck werden unreferenzierte, niedrig
priorisierte LRU-Ressourcen freigegeben. Beschädigte Daten aktivieren die
konfigurierte Fallback-Ressource; ungültige oder übergroße Ausgaben werden ohne
Speicherüberschreibung abgewiesen.

Hosttests decken NONE, LZ4-Literale, überlappende Matches, abgeschnittene und
beschädigte Streams, Zielgrößenfehler, Zstd-Abweisung, Lazy Loading,
Cache-Wiederverwendung, Fallback und Poolgrenze ab. Das reale GPT/FAT32-Abbild
dekomprimiert beim Start unter OVMF/QEMU eine gepackte Ressource, validiert sie
und meldet `UEFI:RESOURCE-LZ4-DECODE-READY`.

Offen bleiben Zstd, belastbare Dekompressionszeitmessung, Ressourcen oberhalb
des statischen 512-KiB-Pools und die gemeinsame BIOS-C-Pipeline. Eine echte
Anbindung komprimierter Ressourcen aus Boot Asset Packages ist weiterhin durch
die nicht normativ definierten BAP-/Index-Wire-Formate in
NPSPEC-BOOTRESOURCE-0002 und -0003 blockiert.

## Resource Integrity Verification

NPSPEC-BOOTRESOURCE-0006 besitzt nun einen zentralen, heapfreien
Integritätsmanager. Er unterstützt CRC32 und vollständiges SHA-256 mit
statischem Hashkontext und konstantzeitlichem Digestvergleich. Der
Vertrauenszustand unterscheidet Unknown, Valid, Signed, Invalid und Corrupted;
die Richtlinien Permissive, Standard und Strict werden zentral und
deterministisch ausgewertet. Strict akzeptiert nur einen Signaturstatus, den
ein vorgelagerter kryptografischer Prüfer nachweisbar als Verified geliefert
hat. Ein bloß vorhandenes Signaturfeld wird nicht als Authentizitätsbeweis
behandelt.

Der Resource Loader prüft ID-/Typkontext, Version, Datenzeiger, Größe,
Pointerüberlauf und Signaturstatus. Gepackte Daten werden vor der
Dekomprimierung gegen CRC32 und optional SHA-256 geprüft; danach folgen
Originalgröße, Original-CRC32 und optional Original-SHA-256. Der ermittelte
Trust-Status liegt O(1) direkt am Ressourcenobjekt. Nur eine weiterhin
validierte Cachekopie überspringt erneutes Hashing; nach Unload oder Eviction
wird bei der nächsten Nutzung erneut geprüft. Beschädigte Ressourcen werden
verworfen und können über den bestehenden Loader-Fallback ersetzt werden.

Hosttests belegen den bekannten FIPS-SHA-256-Vektor `abc`, CRC32/SHA-256,
Permissive/Standard/Strict, ungültige und vorgelagert verifizierte
Signaturzustände sowie den Cache-Skip. Bestehende Loader-Tests decken außerdem
Korruption, Abhängigkeiten, Zyklen und Fallback ab. Das reale GPT/FAT32-Abbild
prüft unter OVMF/QEMU feste SHA-256-Werte derselben Ressource vor und nach LZ4
und meldet `UEFI:RESOURCE-SHA256-VERIFIED`.

Nicht vorgetäuscht werden Ed25519 oder ECDSA P-256: Für Schlüssel, Signaturen,
Trust Anchors und Richtlinien fehlt eine NPSPEC-BOOTSECURITY-0001 im aktuellen
Dokumentbestand. Auch Paket-/Indexheader, Manifest und ihre signierten
Bytebereiche sind in BOOTRESOURCE-0002/0003 nicht normativ als Wire-Format
definiert. Diese Authentizitätsanteile bleiben daher eindeutig blockiert.
Zusätzlich offen sind optionales SHA-512, Validierungszeitmessung und dieselbe
Integritätspipeline im BIOS-Pfad.

## PNG Decoder

NPSPEC-BOOTRESOURCE-0007 ist nun als plattformneutraler, heapfreier Decoder in
den Image Renderer und Resource Loader integriert. Die öffentliche API umfasst
Initialisierung, reine Validierung und Dekodierung. Ein statischer 240-KiB-
Arbeitsbereich nimmt den Inflate-/Scanline-Datenstrom auf; fertige Bilder
werden ausschließlich aus dem bestehenden 512-KiB-RGBA-Pixelpool erzeugt.

Unterstützt sind die verpflichtenden Farbtypen Grayscale, RGB, RGBA und
Indexed mit den normativen Bit-Tiefen 1, 2, 4 und 8, soweit der jeweilige
PNG-Farbtyp sie erlaubt. Zusätzlich wird Grayscale+Alpha mit 8 Bit verarbeitet.
Packed Samples werden bitgenau extrahiert und auf RGBA32 skaliert. PLTE und
tRNS funktionieren für Indexed, Grayscale und RGB; echte Alpha-Kanäle werden
premultipliziert. Mehrere zusammenhängende IDAT-Chunks, Stored-, Fixed- und
Dynamic-Deflate, Adler32 sowie None, Sub, Up, Average und Paeth sind vollständig
im statischen Decoderpfad enthalten.

Der Parser prüft PNG-Signatur, IHDR als ersten Chunk, Chunknamen und Reserved
Bit, jede Chunklänge und Addition, CRC32 jedes Chunks, PLTE-/tRNS-/IDAT-
Reihenfolge, unbekannte Critical Chunks, IEND ohne Nutzdaten und das exakte
Dateiende. Dimensionen, Scanlinegröße, Pixelbudget und Paletteindizes werden
vor einer Image-Allokation validiert. Diagnosen erfassen Bildgröße, Farbtyp,
Bit-Tiefe, Chunks, IDATs, Filterzeilen, CRC- und Formatfehler.

Hosttests belegen RGB, RGBA, Indexed 8/2 Bit, Grayscale 1/8 Bit,
Grayscale+Alpha, alle Transparenzpfade, mehrere IDATs, alle fünf Filter,
Deflatevarianten, CRC-Schäden und die öffentliche Validate-API. Das reale
GPT/FAT32-Abbild lädt unter OVMF/QEMU eine eingebettete 1-Bit-Grayscale-PNG mit
tRNS durch den produktiven Resource Loader und meldet
`UEFI:PNG-DECODER-READY`.

Offen bleiben die optionale 16-Bit-Unterstützung, Adam7-Interlacing, mehr als
16 IDAT-Chunks, belastbare Decodezeitmessung und dieselbe C-Pipeline im
BIOS-Pfad. Externe PNGs aus BAP-Dateien bleiben zusätzlich vom fehlenden
Wire-Format in BOOTRESOURCE-0002/0003 abhängig.

## SVG Icon Renderer – normativ blockiert

NPSPEC-BOOTRESOURCE-0008 kann derzeit nicht spezifikationsgetreu implementiert
werden. Sie fordert ausdrücklich NPSPEC-BOOTRENDER-0012 und -0013 als
Transformations-, Alpha- und Vektorgeometriegrundlage. Eine Datei 0013 ist im
Dokumentbestand nicht vorhanden. Die Datei
`NPSPEC-BOOTRENDER-0012_Alpha_Compositing.md` trägt intern die Dokument-ID 0011
und ist inhaltlich eine Kopie der 2D-Transformationsspezifikation; ein
Alpha-Compositing- oder Vector-Geometry-Vertrag fehlt damit ebenfalls.

Auch BOOTRESOURCE-0008 selbst nennt zwar SVG-Elemente und Attribute, definiert
aber nicht die unterstützten Path-Kommandos, Fill-/Winding-Regel, Stroke Caps
und Joins, Kurven-/Arc-Flattening, XML-Namespace-/Entityregeln oder die Syntax
für Themefarbreferenzen. Ohne diese Regeln wären Rasterresultate und damit die
geforderte BIOS-/UEFI-Identität nicht objektiv testbar. Ein beliebiger
Teilparser würde den fehlenden Vertrag nur verdecken.

Der Bootmanager verwendet deshalb weiterhin die bereits validierte,
plattformneutrale 2-Bit-Antialias-Iconquelle mit DLU-Skalierung und Theme-Tint.
Der SVG-Renderer bleibt im Audit ausdrücklich **Blockiert**, bis 0013 ergänzt,
0012 korrigiert und die oben genannten SVG-Unterregeln festgelegt sind.

## Font Resources

Der technisch eindeutig definierte Teil von NPSPEC-BOOTRESOURCE-0009 ist nun
implementiert. `font_resources.c` stellt eine statische Registry für 16 Fonts
mit einem 32-Slot-Open-Addressing-Index bereit. Jeder Eintrag enthält Font- und
Ressourcen-ID, Font-/Ressourcenversion, Name, Familie, Stil, Gewicht,
Priorität, sortierte Unicodebereiche, Fallback-ID, Zustand und Referenzen.
Fontdaten werden ausschließlich über den zentralen Resource Loader geladen;
direkter Dateizugriff der Textengine findet nicht statt.

Der Resolver verarbeitet deterministisch Primärfont, font-eigenen Fallback,
Theme-Fallback, Systemfont und schließlich Missing Glyph. Rekursive Ketten
werden erkannt. Primary, Heading, Monospace, Symbol und Fallback sind als
Theme-Rollen gespeichert; ein Themewechsel ändert nur IDs und lädt die UI
nicht neu. Die produktive 2-Bit-AA-Systemschrift ist als UI/SemiBold mit ihrer
tatsächlichen ASCII-/Umlautabdeckung registriert, und die Textengine konsultiert
die Registry vor jeder Glyphenauswahl.

Hosttests belegen O(1)-Lookup, Metadaten, Load/Release, Cache, Duplicate-Schutz,
Unicodeabdeckung, Missing Glyph, eine zweite Symbolschrift, Fallback und
Theme-Rollen. Das reale GPT/FAT32-Abbild lädt und löst die Systemschrift unter
OVMF/QEMU produktiv auf und meldet `UEFI:FONT-RESOURCE-REGISTRY-READY`.

Das verpflichtende Nova Font Package kann noch nicht geparst werden: Keine
vorhandene Spezifikation definiert NFP-Magic, Headergröße, Endianness,
Tabellenoffsets, Unicode-Mapping, Glyph-/Metrik-/Kerningrecords oder signierte
Bytebereiche. Externe NFP/BAP-Fonts bleiben deshalb blockiert. Ebenfalls offen
sind vollständige Unicode-/Sprach-/Scriptfonts, optionale TTF/OTF-Unterstützung,
Ladezeitmessung und dieselbe Registry im BIOS-Pfad.

## Animation Resources

NPSPEC-BOOTRESOURCE-0010 besitzt nun eine zentrale, heapfreie Registry für 32
Animationen mit einem 64-Slot-O(1)-ID-Index und bis zu 32 statisch kopierten
Keyframes je Ressource. Deskriptor und Validator prüfen ID, Name, Font-unabhängige
Ressourcen-/Formatversion, Dauer, Wiederholungsmodus, Trigger, Kategorie,
Priorität sowie Property, Easing und strikt ansteigende Zeitpunkte. Jede
verwendete Property muss einen Keyframe bei 0 und am Timelineende besitzen.

Der Timeline-Resolver wertet kombinierte Properties in O(n) aus und verwendet
dieselben deterministischen Fixed-Point-Easings wie die Motion Engine. Once,
feste Wiederholungszahl und Infinite werden verarbeitet. Themes können acht
Animationsslots austauschen und die Geschwindigkeit zwischen 25 und 400
Prozent setzen. Reduced Motion liefert unmittelbar den sicheren Endzustand und
wird mit der Motion Engine synchronisiert. Trigger werden nach Kategorie und
Priorität aufgelöst; Loader, Referenzen und Cache laufen ausschließlich über
den Resource Manager.

Hosttests belegen kombinierte Opacity-/Scale-Keyframes, Zwischenwerte,
Wiederholung, Theme-Speed, Trigger, Themebindung, Cache, Duplicate-Schutz,
ungültige Timelines und Reduced Motion. Das reale GPT/FAT32-Abbild registriert,
lädt, bindet und sampelt unter OVMF/QEMU eine Page-Enter-Ressource und meldet
`UEFI:ANIMATION-RESOURCE-READY`.

Noch nicht sämtliche bestehenden spezialisierten Dialog-, Fokus-, Navigation-
und Fortschrittshelfer beziehen ihre Defaultwerte ausschließlich aus dieser
Registry; diese Migration bleibt offen. Ebenfalls fehlen ein normatives
externes Animations-Wire-Format, eine eindeutige Darstellung von Color-
Keyframes und Triggerpayloads, Lade-/Auswertungszeitmessung sowie dieselbe
Registry im BIOS-Pfad.

## Animation Diagnostics und Frame-Spitzen

NPSPEC-BOOTDIAG-0004 ist als passive, heapfreie Messschicht in die Motion Engine und den realen Framepfad integriert. Ein vorallokierter 256er Ring zeichnet Start, Pause, Fortsetzung, Wiederholung, Abschluss, Abbruch und Fehler samt Motion Token, Property, Easing, Soll-/Ist-Dauer, Latenz, Gruppe, Priorität, Qualitäts- und Hardwareprofil auf.

Die Frameanalyse erfasst Budgetüberschreitungen, ausgelassene oder doppelte Frames und Jitter sowie Render-, Compositor-, Layout- und CPU-Anteile. Der lokale Diagnoseexport enthält nun zusätzlich den Animationsbericht; NDF, JSON, CSV und Binärformat sind nur nach ausdrücklicher Nutzeraktion verfügbar. Hosttests decken Lifecycle, Filter, Frameabweichungen, Exportautorisierung und Reset ab. Das aktuelle GPT/FAT32-Image weist die Integration in EDK2/QEMU mit `UEFI:ANIMATION-DIAGNOSTICS-READY` nach.

Noch offen sind eine grafische Timeline-/Graph-Detailansicht, echte GPU-Zeit, allgemeine Window-/Dialog-ID-Zuordnung, persistente Exportziele, physische Langzeit-/Overheadmessungen und BIOS-Parität.

## Resource Loading Diagnostics

NPSPEC-BOOTDIAG-0005 ist in den Resource Manager eingebunden. Ein vorallokierter 256er Ring protokolliert Anfrage, Ladestart, Integritätsprüfung, Dekomprimierung, Cache Hit/Miss, Abschluss, Freigabe und Fehler. Jeder Eintrag enthält Ressourcen-ID, Typ, Version, Zustand, Ergebnis, Kompressions- und Truststatus, Größen-, Speicher- und Zeitfelder. Filter stehen für Typ, Ergebnis, Integrität, Zeit, Bootphase, Modul, Cache Hits und Fehler bereit.

Der explizite Diagnoseexport erzeugt zusätzlich einen Ressourcenbericht als NDF; die API unterstützt autorisiert auch JSON, CSV und Binärformat. Hosttests verwenden den echten LZ4-, Integritäts-, Cache-, Fehler- und Fallbackpfad. Das aktuelle GPT/FAT32-Image bestätigt die produktive Integration in EDK2/QEMU mit `UEFI:RESOURCE-LOADING-DIAGNOSTICS-READY`.

Offen bleiben Firmware-Hochpräzisionsmessungen für jede einzelne Loaderphase, allgemeine Parent-/Bootphasen-/Modulzuordnung, SVG-Diagnose bis zum normativen SVG-Vertrag, persistente Exportziele, physische Overheadmessungen und BIOS-Parität.

## Performance Regression Tests

NPSPEC-BOOTTEST-0008 besitzt nun einen heapfreien Baseline Manager für Bootzeit, FPS, Framezeit, Speicher, Ressourcenladen, Animation, Eingabelatenz und Cache-Hit-Rate. Jede Baseline führt Build-ID, Version, Hardwareprofil, Firmwaretyp, Richtung und Schwelle. Abweichungen werden deterministisch in Basispunkten berechnet und nach den normativen 5-Prozent- beziehungsweise FPS-3-Prozent-Grenzen als None, Minor, Major oder Critical klassifiziert.

Der Runner kann die vorhandenen Startup-, Rendering-, Memory-, Resource-, Animation- und Inputdiagnosen automatisch übernehmen. Ein autorisierter CI-lesbarer Bericht enthält Benchmark, Baseline, Messwert, Abweichung und Klasse. Hosttests prüfen die exakten Grenzwerte und sämtliche Regressionsklassen; EDK2/QEMU bestätigt die Integration mit `UEFI:PERFORMANCE-REGRESSION-READY`.

Offen bleiben persistente Vergleichsdaten für den letzten erfolgreichen Build und Releases, Diagrammdateien, echte Nightly-/Pull-Request-/Release-Workflows, Installer-End-to-End-Benchmarks, eine physische Hardwarematrix und BIOS-Parität.

## Resource Corruption Tests

NPSPEC-BOOTTEST-0009 besitzt eine isolierte, heapfreie Korruptionssuite mit statischem 64-KiB-Manipulationspuffer und festem Ergebnisarray. Sie erzeugt reproduzierbar Bitfehler, abgeschnittene und leere Daten sowie ungültige Header, Signaturen, Prüfsummen, Versionen und Metadaten. Jede manipulierte Kopie durchläuft den produktiven Integritäts- und Registrierungsweg; Produktivbytes werden nicht verändert.

Die Ergebnisse enthalten Ressource, Fallback, Korruptionsart, Fehlercode, Speicherzustand, Erkennung, Bootfortsetzung und Poolkonsistenz. Hosttests prüfen alle acht Arten, einen CI-lesbaren Bericht sowie die Unverändertheit von Registry und Cache. Bereits vorhandene Tests ergänzen PNG-CRC/Header, SHA-256, LZ4-Fehler/Fallback, Fonts, Themes, Animationen und Konfiguration. Das GPT/FAT32-Image führt acht isolierte Proben gegen die Logoressource aus und meldet `UEFI:RESOURCE-CORRUPTION-TESTS-READY`.

SVG und Asset Packages bleiben mangels normativer Formate blockiert. Außerdem fehlen echte kryptografische Signaturprüfung, Random-Fuzzing, persistente Reports, vollständige Recovery-UI-Matrix, Installer und BIOS-Parität.

## Fallback Mode Tests

NPSPEC-BOOTTEST-0010 besitzt eine gemeinsame heapfreie Test-API für Software-/Textausgabe, Standardtheme, Systemschrift, Platzhaltergrafik, Standardanimation, Recovery, Safe Mode, Minimalmodus und Low-End-Profil. Die Suite ruft die produktiven Subsystem-APIs auf und erfasst Aktivierung, Erfolg, automatische Umschaltung, Bedienbarkeit, Integritäts- und Sicherheitserhalt, Recovery-Verfügbarkeit sowie Speicher vor und nach dem Wechsel.

Hosttests aktivieren alle neun Ketten und erzeugen einen autorisierten CI-Bericht. Das reale GPT/FAT32-Image führt dieselbe Suite aus, stellt die temporär veränderten Recovery-, Compositor- und Motionzustände anschließend wieder her und meldet `UEFI:FALLBACK-MODE-TESTS-READY`.

Offen bleiben echte GPU-/GOP-/VESA-Fehlerinjektion auf physischer Hardware beziehungsweise im BIOS-Pfad, SVG-Fallback bis zum normativen Vektorvertrag, Touch, Installer, Self-Healing, hochpräzise Umschaltzeiten und eine echte Secure-Boot-Zustandsabfrage.

## Input Repeat

NPSPEC-BOOTINPUT-0011 besitzt nun eine zentrale, heapfreie Tabelle aus acht
parallelen Repeat-Kontexten. Jeder Kontext speichert die vollständige
Unified-Eventvorlage, Input- und Device-ID, gebundenes Fokusziel, Start,
nächsten Termin, Stop, Dauer, Anzahl, Fehler und einen der Zustände `Idle`,
`Waiting`, `Repeating`, `Paused` oder `Stopped`.

Keyboard-, Character-, Navigations-, Controller- und optionale Pointer-Wheel-
Ereignisse sind zulässig; kritische Einmalaktionen werden abgewiesen. Initial
Delay von 100 bis 5000 ms und Intervall von 20 bis 2000 ms sind zentral
konfigurierbar. Accessibility kann Repeat vollständig deaktivieren, das Delay
verlängern und die Frequenz reduzieren. Fokus- und Geräteverlust stoppen aktive
Kontexte. Hosttests prüfen Timing, mehrere Kontexte und Eventtypen, Pause/Resume,
kritische Aktionen, Fokus, Geräteverlust und Accessibility. Das GPT/FAT32-Abbild
meldet unter QEMU `UEFI:INPUT-REPEAT-READY`.

Offen bleiben die produktive KeyUp-Anbindung unter UEFI SimpleTextInput,
Controller-/Analog-HAL, automatisches Ende an echten Listengrenzen,
Konfigurationspersistenz, BIOS-Parität und physische Tests.

## Keyboard Shortcuts

NPSPEC-BOOTINPUT-0010 besitzt nun eine zentrale, heapfreie Hash-Registry mit 64
Slots für maximal 32 Shortcuts. Ein Eintrag enthält Taste, Modifier, Command,
Global-/Kontext-/Dialog-/Developer-Scope, Kontext-ID und optional eine
Fokusbindung. Die Auflösung erfolgt deterministisch in der Reihenfolge Dialog,
autorisierter Developer, Kontext und Global und liefert ausschließlich eine
Command-ID an den registrierten Dispatcher.

Gleichrangige Duplikate werden bei der Registrierung als Konflikt abgewiesen.
Unregister verwendet Tombstones, Entwickler-Shortcuts erfordern eine explizite
Autorisierung und rekursiver Command-Dispatch wird blockiert. Diagnosen erfassen
Registrierung, Entfernen, Ausführung, Konflikte, Fehlschläge, unautorisierte und
rekursive Aufrufe sowie die letzte Kombination, Scope, Command, Kontext und den
Fokus. Hosttests prüfen alle Scopes, Priorität, Konflikt, Autorisierung, Dispatch,
Rekursion und Entfernen. Das GPT/FAT32-Abbild meldet unter QEMU
`UEFI:KEYBOARD-SHORTCUTS-READY`.

Offen bleiben echte Mehrtasten-Sequenzen jenseits von Modifier-Chords, eine
zentral konfigurierbare Standardbelegung, Sticky-/Slow-Keys-Provider, die
produktive Command-Anbindung sämtlicher Views, BIOS-Parität und physische Tests.

## Directional Focus Navigation

NPSPEC-BOOTINPUT-0009 verwendet nun einen statischen, direkt adressierten
Navigationsgraphen mit sechs Zielslots je Control. Explizite Nachbarn besitzen
Vorrang. Für Up, Down, Left und Right werden geometrische Kandidaten aus den
logischen Control-Bounds, dem Abstand auf der Hauptachse und der Querabweichung
beim Aufbau des Scope-Graphen vorbestimmt. Forward und Backward verwenden die
Scope-Reihenfolge. Lookup und Navigation laufen dadurch in O(1); nur der seltene
Graphaufbau arbeitet quadratisch über maximal 32 Scope-Controls.

Randverhalten kann auf Stop oder Wrap gesetzt werden; benutzerdefinierte
Weiterleitung entsteht über explizite Kanten. Vor jedem Wechsel werden Ziel,
Fokusfähigkeit und aktiver Scope erneut validiert, rekursive Navigation wird
gesperrt. Diagnosen enthalten Quelle, Ziel, Richtung, Scope, explizite und
geometrische Treffer, Wraps, Fehler und Fehlschläge. Hosttests prüfen expliziten
Vorrang, horizontale und vertikale Geometrie, Wrap und Scope-Isolation. Das
GPT/FAT32-Abbild meldet unter QEMU `UEFI:DIRECTIONAL-FOCUS-READY`.

Offen bleiben ein eigenes Navigationsprioritätsfeld am Control,
Analogstick-Deadzone und -Repeat, ein echter Screenreader, BIOS-Parität und
physische Controller-Tests.

## Focus Management

NPSPEC-BOOTINPUT-0008 besitzt nun einen heapfreien Stack aus acht Focus Scopes
mit bis zu 32 vorregistrierten Controls je Scope. Jeder Scope definiert einen
Default-Fokus oder verwendet deterministisch sein erstes Element. `Enter` und
`Leave` isolieren verschachtelte Dialog-/Overlaybereiche und stellen den zuvor
aktiven Fokus atomar wieder her. Vorwärts- und Rückwärtsnavigation arbeiten über
den aktuellen Index in O(1) und schließen zyklisch.

Nur lebende, sichtbare, aktivierte und nicht dekorative Controls innerhalb des
aktiven Scope können Fokus erhalten. Focus/Blur-Ereignisse, Themezustand,
Accessibility-Trace und Announcement-Zähler sowie Fokus-, Scope-, Navigations-,
Restore- und Fehlerdiagnosen sind angebunden. Hosttests prüfen Default, Reihenfolge,
Wrap, modale Isolation, Fremdfokus, Restore und ungültige Controls. Das reale
GPT/FAT32-Abbild meldet unter QEMU `UEFI:FOCUS-SCOPE-MANAGER-READY`.

Offen bleiben ein einheitliches Control-Backing für alle produktiven
Dialogschaltflächen, Layout-/Dokument-Fallbackreihenfolge, ein echter
Screenreader-Provider, eine Firmware-Latenzzeitquelle, BIOS-Parität und physische
Accessibility-Tests.

## Input Device Hot Detection

NPSPEC-BOOTINPUT-0012 besitzt nun eine feste, heapfreie 16er Registry mit direkt
adressiertem O(1)-Lookup. Der definierte Lebenszyklus `Detected`, `Initializing`,
`Ready`, `Disconnected` und `Error` wird durch `Device Added`, `Changed`, `Ready`,
`Removed` und `Error` als Unified-Systemereignisse sichtbar. Nur `Ready`-Geräte
dürfen normale Eingaben liefern. Entfernung beendet Pointer Capture und Repeat,
erhält jedoch den UI-Fokus; Wiederanmeldung ist ohne Neustart möglich.

Hosttests prüfen Ereignisreihenfolge, mehrere Geräte, Zustands- und Count-Abfragen,
Entfernung, Fokus, gesperrte Eingaben, Typfehler und ID-Slotkollision. Das reale
GPT/FAT32-Abbild prüft denselben Lebenszyklus unter EDK2/QEMU und meldet
`UEFI:INPUT-HOT-DETECTION-READY`.

Die UEFI-Schicht registriert zusätzlich einen hardwareunabhängigen Scanner beim
Input Manager. Er fragt `EFI_SIMPLE_POINTER_PROTOCOL` alle 250 ms erneut ab,
synchronisiert Hinzufügen und Entfernen und verwirft bei Geräteverlust noch
ausstehende Pointerbewegungen und Tastenstände. Unified-Geräteereignisse werden
beim Lesen der Tastatur kontrolliert übersprungen und können daher keine
Tastaturdaten verfälschen. Ein isolierter Firmware-Mock prüft unveränderte Scans,
Entfernen und erneutes Hinzufügen; QEMU meldet
`UEFI:INPUT-DEVICE-MONITOR-READY`.

Offen bleiben echte Firmwareprovider für Hersteller-, Produkt- und
Firmwareinformationen sowie Bluetooth, Touch, Controller, BIOS-Parität und
physische Hotplugtests.

## BIOS-/UEFI-Kompatibilitätsmatrix

NPSPEC-BOOTTEST-0007 besitzt nun einen gemeinsamen automatisierten Runner. `make test-firmware-compatibility` baut und startet den Legacy-BIOS-Pfad unter SeaBIOS sowie den UEFI-Pfad unter EDK2/OVMF. Anschließend entsteht [firmware-compatibility-report.md](../../build/firmware-compatibility-report.md) mit getrennten Ergebnissen und einem expliziten Paritätsfeld für Firmwareentry, Grafik, Fallback, Bootmanager, Ressourcen, Eingabe, Recovery, Kernelhandoff, Secure Boot und ExitBootServices.

Nachgewiesen sind VBE und GOP, beide Bootmanagerstarts, BIOS-NBHP/BIB samt Kernelstart sowie die vollständige UEFI-GUI mit Input, Ressourcen, Diagnose und Fallbacks. Der Bericht behauptet bewusst keine vollständige Parität: BIOS und UEFI verwenden noch getrennte UI-, Resource-, Input- und Recoverypipelines. Der produktive UEFI-Kernelhandoff einschließlich Memory Map und ExitBootServices fehlt weiterhin.

Weitere klare Restlücken sind CSM, UEFI Class 2/3, Secure Boot, TPM, Touch, Installer, VirtualBox/VMware/Hyper-V, physische Referenzhardware und pixelidentische Ausgabe.
## Alignment und Spacing (NPSPEC-BOOTLAYOUT-0002)

Die Layoutlaufzeit unterstützt jetzt horizontale und vertikale Ausrichtung,
Stretch, Baseline-Ausrichtung, getrennte Margins und Paddings sowie feste und
gleichmäßige Verteilung. Alle Maße werden als DLU entgegengenommen und mit der
zentralen Ganzzahlskalierung berechnet; bei aktivierter Barrierefreiheit gilt
eine Mindestskalierung von 125 %. Die Implementierung benötigt keinen Heap und
verteilt mehrere Elemente in O(n).

Der Hosttest deckt Center, Stretch, Baseline, Accessibility, Space-Between,
Space-Evenly und ungültige Insets ab. Der UEFI-Selbsttest prüft die identische
Laufzeit im GPT/FAT32-Abbild und meldet `UEFI:ALIGNMENT-SPACING-READY`.
Noch offen sind die vollständige Migration aller produktiven Controls, echte
Font-Baselines, verschachtelte Container, BIOS-Parität und physische Tests.
## Stack Layout (NPSPEC-BOOTLAYOUT-0003)

Die Layoutlaufzeit besitzt nun feste, heapfreie Stackcontainer für vertikale
und horizontale Anordnung. Bis zu 16 Kinder werden in stabiler Reihenfolge mit
DLU-Padding und -Spacing, Alignment, Stretch, Constraints und allen definierten
Verteilungsmodi berechnet. Unsichtbare Kinder belegen keinen Platz.
Verschachtelung ist bis zu einer festen Tiefe möglich; doppelte IDs, mehrfach
eingehängte Container und Zyklen werden vor dem Layout abgewiesen.

Hosttests prüfen Stretch, Sichtbarkeit, Accessibility, Verschachtelung sowie
Duplikat- und Zyklusschutz. Der UEFI-Selbsttest meldet im GPT/FAT32-Abbild
`UEFI:STACK-LAYOUT-READY`. Offen bleiben die vollständige Migration der
produktiven Seiten, BIOS-Parität, die Recovery-/Auflösungsmatrix und reale
Zeitmessung.
## Grid Layout (NPSPEC-BOOTLAYOUT-0004)

Die Layoutlaufzeit unterstützt jetzt heapfreie Grids mit Auto-, Fixed-, Star-
und Percentage-Tracks. Je Grid stehen feste Listen für acht Zeilen, acht
Spalten und 32 Kinder zur Verfügung. RowSpan, ColumnSpan, DLU-Abstände,
Margin, Padding, Constraints, Zellalignment, Stretch und optional reservierte
unsichtbare Zellen werden deterministisch berechnet. Duplikate, überlappende
Zellen, ungültige Indizes, Mehrfacheinhängung und Zyklen werden abgewiesen.

Hosttests decken alle Tracktypen, Spans, Accessibility und Überlappungsschutz
ab. Der Firmware-Selbsttest meldet im GPT/FAT32-Abbild
`UEFI:GRID-LAYOUT-READY`. Offen bleiben die Migration produktiver Dialoge und
Formulare, explizite Render-Z-Sortierung, BIOS-Parität, eine vollständige
Auflösungsmatrix und reale Zeitmessung.
## Overlay Layout (NPSPEC-BOOTLAYOUT-0005)

Die Layoutlaufzeit besitzt nun eine feste, heapfreie Overlayliste mit sieben
Layerprioritäten und zusätzlichem Z-Index. Kinder unterstützen DLU-Padding,
Margin und absolute Offsets, Alignment, Stretch, optionales Clipping,
Visible/Hidden/Collapsed, Accessibility-Mindestflächen und Materialmetadaten
für Glass, Acrylic und Solid. Modale Ebenen sperren niedrigere Eingabeziele;
der Hit-Test läuft von der obersten sichtbaren interaktiven Ebene abwärts.

Duplikate, gleich priorisierte modale Ebenen, Mehrfacheinhängung, Zyklen und
Überläufe werden abgewiesen. Hosttests prüfen Layer-/Z-Reihenfolge, Clipping,
Materialien, Modalität, Hit-Testing und Sichtbarkeitswechsel. Der UEFI-Test
meldet `UEFI:OVERLAY-LAYOUT-READY`. Offen bleiben die vollständige Migration
der produktiven Dialog-/Tooltip-Pfade, BIOS-Parität, physische Auflösungstests
und reale Zeitmessung.
## Constraint-Based Sizing (NPSPEC-BOOTLAYOUT-0006)

Die bisherige Min/Preferred/Max-Hilfsfunktion wurde um eine vollständige
DLU-Constraint-Engine erweitert. Unterstützt werden Auto, Fixed, Content,
Percentage, Stretch und Remaining Space sowie Margin, Padding, Min/Max,
Preferred und feste Seitenverhältnisse. Konflikte werden in der definierten
Priorität Min, Max, Aspect, Preferred, Stretch und Percentage aufgelöst.
Beschädigte Regeln liefern einen sicheren Standardbereich und einen Fehlerstatus.

Accessibility erzwingt für interaktive Elemente mindestens 44 DLU und eine
Skalierung von 125 %. Stack, Grid und Overlay können adaptive Constraints im
Measure-Pass direkt verwenden. Hosttests prüfen alle zentralen Größenarten,
Prioritäten, 16:9, Accessibility, Fehlerfallback und Containerintegration. Das
UEFI-Abbild meldet `UEFI:CONSTRAINT-SIZING-READY`. Offen sind verbleibende
direkte Layouttokens der Hauptseite, proportionale Mehrfach-Stretchgewichte,
vollständige Controlmigration, BIOS-Parität sowie Recovery-/DPI-Hardwaretests.
## Responsive Boot Layout (NPSPEC-BOOTLAYOUT-0007)

Die UI verwendet nun eine zentrale responsive Policy auf Basis der logischen
DLU-Breite. Die Breakpoints liegen exakt bei Compact unter 800 DLU, Standard
von 800 bis 1279 DLU und Wide ab 1280 DLU. Dieselbe Policy entscheidet über
Stacked/Side-by-Side-Reflow, adaptive Abstände, begrenzte Control- und
Dialogbreiten und erkennt Ultrawide. Auflösung, DPI, Skalierung,
Accessibility, Breakpointwechsel und Generation werden diagnostiziert.

Dialoge werden zentriert und nötigenfalls vollständig in die Safe Area
verkleinert. Der produktive Hauptlayoutpfad bezieht Layoutklasse, Panelbreite,
Innen- und Zeilenabstände aus der Policy. Hosttests prüfen 640×480, die exakten
800-/1280-DLU-Grenzen, 4K, Accessibility, 21:9 und übergroße Dialoge; zusätzlich
bleibt die 65-Fall-Auflösungs-/Skalierungsmatrix aktiv. QEMU meldet auf der
realen GOP-Fläche `UEFI:RESPONSIVE-LAYOUT-READY`. Offen sind die Migration
einzelner Recovery-/Passwort-Bounds-Helfer, physische Rotation/Displaywechsel
und BIOS-Pixelparität.
## Safe Display Areas (NPSPEC-BOOTLAYOUT-0008)

Der Resolution Manager verwaltet jetzt genau eine aktive, heapfreie Safe Area.
Standardmäßig werden an jeder Kante 5 % reserviert. UEFI darf diese Reserve
explizit deaktivieren; BIOS-Overscan erzwingt mindestens 7 %, Remote-Ausgabe
mindestens 6 %. Asymmetrische Firmware-Insetdaten werden je Kante mit dem
größeren Sicherheitswert kombiniert. Accessibility erweitert die nutzbare
Fläche kontrolliert, ohne gemeldete Hardwareinsets zu unterschreiten.

Die Policy erzeugt Pixel- und DLU-Koordinaten in O(1), besitzt ein Contains-Gate
für interaktive Bounds und fällt bei beschädigten Daten auf die vollständige
Bildschirmfläche zurück. Responsive Policy, Hauptlayout und Dialog-Fit verwenden
ausschließlich diese Safe Area. Hosttests prüfen GOP, BIOS 800×600, Overscan,
asymmetrische Insets, Accessibility und Fehlerfallback; QEMU meldet auf der
realen GOP-Fläche `UEFI:SAFE-DISPLAY-AREA-READY`. Offen sind automatische
Hardwareprofilerkennung, die Prüfung jedes produktiven Control-Bounds,
physischer Overscan und BIOS-Pixelparität.
## DPI and Resolution Scaling (NPSPEC-BOOTLAYOUT-0009)

Der Resolution Manager stellt nun eine zentrale Scaling-Information mit
DPI X/Y, globalem Faktor, Accessibility-Faktor, effektivem Faktor und
Generation bereit. Verlässliche Firmware-DPI werden bevorzugt; fehlende oder
unglaubwürdige Werte führen explizit zu 96 DPI. Unterstützt werden 100, 125,
150, 175, 200, 250 und 300 % sowie freie Zwischenwerte. Accessibility wird
nach dem globalen Faktor mit 125 % angewandt und bei 300 % begrenzt.

Die Umrechnung nutzt auf allen Pfaden dieselbe symmetrische Ganzzahlrundung,
saturiert bei `int32`-Überlauf und kann Intervalle über gemeinsam gerundete
Kanten ohne Lücken zwischen Nachbarelementen snappen. Alignment, Constraints,
Stack, Grid, Overlay und das produktive Layout nutzen den effektiven Faktor;
Responsive-Breakpoints bleiben unabhängig davon logisch. Hosttests prüfen DPI,
alle Stufen, 137,5 %, Accessibility, Rundung, Snapping und Überlauf. QEMU meldet
`UEFI:DPI-RESOLUTION-SCALING-READY`. Offen sind physische GOP-Displaymaße,
die Vereinheitlichung einzelner Text-/Icon-Scale-Einstiege, 8K-QEMU,
Display-Hotplug und BIOS-Pixelparität.

## Text-Aware Layout (NPSPEC-BOOTLAYOUT-0010)

Die Textmessung liefert nun intrinsische Mindest- und Maximalbreite,
Zeilenhöhe, Ascender, Descender, Baseline und Wortabstand. Word Wrap,
Character Wrap, No Wrap, Mehrzeilen-Autohöhe und eine maximale Zeilenzahl
werden in einem deterministischen Durchlauf ausgewertet. DPI,
Accessibility-Skalierung und Zeichenabstand fließen vor dem Measure Pass ein.
Ungültiges UTF-8 sowie fehlende Glyphen aktivieren den Ersatzfont und werden
diagnostiziert.

Ein fester Cache mit 24 Einträgen speichert Textmetriken ohne Heap und prüft
kurze Texte über einen exakten UTF-8-Schlüssel. `nova_layout_apply_text`
kombiniert die Metriken mit Iconbreite, Iconabstand und Padding. Stack-Kinder
können diese Messung vor Constraints und Arrange anfordern und führen die
berechnete Baseline mit. Der produktive Tooltip berechnet Breite und
Mindesthöhe ebenfalls aus der neuen API. Hosttests prüfen lokalisierte lange
Texte, Wrap, Cachetreffer, Baseline, Accessibility, No Wrap, beschädigtes
UTF-8 sowie die Stackintegration. Der QEMU-Test meldet
`UEFI:TEXT-AWARE-TOOLTIP-READY`.

Der Renderer besitzt getrennte Pfade für keine Kürzung sowie End-, Middle- und
Start-Ellipsis. Die Breite der drei Punkte wird vor der Auswahl von Präfix und
Suffix reserviert. Auch Grid-Auto-Tracks und Overlay-Kinder verwenden jetzt
die intrinsischen Text-, Icon- und Paddinggrößen vor ihrem Arrange-Pass.

Noch offen sind RTL-Shaping, die vollständige Migration aller Controls sowie
identische Pixelresultate im Legacy-BIOS-Pfad.

## Boot Dialog Architecture (NPSPEC-BOOTDIALOG-0001)

Der feste Viererpool besitzt nun getrennte O(1)-Operationen für Create, Show
und Close. Jeder Dialog erhält eine stabile Root-View-ID, einen eigenen
Navigationskontext, gespeicherten Vorfokus, Öffnungs- und Schließzeit sowie
eine Lifecycle-Bitmaske. Zustandswechsel akzeptieren ausschließlich die Folge
Created, Initialized, Opening, Visible, Active, Closing und Destroyed.

Mehrere nichtmodale Dialoge können auf dem separaten Stack liegen und geben
nach dem Schließen den darunterliegenden Dialog wieder frei. Solange ein
modaler Dialog aktiv ist, kann kein nichtmodaler Dialog darüber geöffnet
werden; dadurch bleiben Modalität und Fokuspriorität eindeutig. Ergebnisse
werden genau einmal typisiert gespeichert, sichere Eingaben vor Destroyed
gelöscht und Pointer-Captures vor einem Kontextwechsel abgebrochen.

Hosttests prüfen den vollständigen Lifecycle, Root- und Navigationsidentität,
zwei nichtmodale Ebenen, Timing, Ergebnis und Modalpriorität. Die isolierte
Zehn-Typen-Suite validiert dieselbe Architektur im UEFI-Build. QEMU meldet
`UEFI:DIALOG-ARCHITECTURE-READY` und öffnet sowie schließt weiterhin den realen
Dialogframe. Offen bleiben produktive nichtmodale Hinweise,
Touch-/Controller-/Screenreaderprovider und Legacy-BIOS-Parität.

## Glass Dialog (NPSPEC-BOOTDIALOG-0003)

Standarddialoge verwenden die zentralen Glass-Material-, Radius-, Border- und
Shadowtokens. Zusätzlich zur gerundeten Außenkontur besitzt die Dialogfläche
nun eine dezente obere Lichtkante. Der Softwareblur arbeitet ausschließlich
in der gerundeten Region hinter dem Dialog und verwendet feste Ring- und
512×512-Cachepuffer.

Der Blur-Cacheschlüssel enthält jetzt die Generation der statischen
Basisebene. Eine neue Seite oder ein neu aufgebautes Theme kann deshalb keine
geometrisch passende, aber inhaltlich veraltete Blurfläche übernehmen. Der
Hosttest prüft neben echten Blurpixeln und Rundungscoverage auch Cachehit,
Generationswechsel, Dirty-Invalidierung, Speicherdegradation und Alpha-Maske.

Der produktive Pfad unterscheidet diagnostisch zwischen echtem Glass-Blur,
transparentem No-Blur-Fallback und vollständig opakem Fallback. QEMU besitzt
kein GPU-Backend und aktiviert das Low-End-Profil; der reale Bildtest weist
deshalb korrekt `UEFI:GLASS-DIALOG-TRANSPARENT-FALLBACK` nach. Der Test-Gate
akzeptiert alle drei normativen Materialpfade. Offen bleiben ein
beschleunigter QEMU-Blurframe, physische GPU-/High-Contrast-Referenzbilder und
Legacy-BIOS-Parität.

## Modal Dialog (NPSPEC-BOOTDIALOG-0002)

Modalität ist jetzt ein expliziter Managerzustand statt nur ein Darstellungsflag.
Ein O(1)-Gate akzeptiert Eingaben ausschließlich für die ID des aktiven
Modaldialogs. NavigateTo, Back und Root prüfen zusätzlich eine zentrale
Navigationssperre. Der Fokus bleibt durch zyklisches Vor-/Zurückschalten in
der Buttonliste; Wraps werden diagnostiziert.

Das Backdrop wird beim Aktivieren atomar gesetzt und beim Schließen entfernt.
Die GUI zeichnet es ausschließlich für modale Dialoge und übergibt den
Modalstatus an den tatsächlichen Compositor-Layer. Abbrechbare Dialoge
behandeln Back als Cancel, ohne die Seite zu verlassen. Nicht abbrechbare
Sicherheits- und Fortschrittsdialoge blockieren Back sowie Seitennavigation.

Hosttests prüfen Input-ID, Navigationsblockade, Backdrop-Lifecycle, Fokusfalle
und beide Backvarianten. Der UEFI-Selbsttest meldet
`UEFI:MODAL-DIALOG-READY`; der reale QEMU-Lauf belegt Enter, Modalframe,
Backdrop, Escape-Close und Wiederherstellung der Seite. Offen bleiben
Touch-/Controller-/Screenreaderprovider und Legacy-BIOS-Parität.

## Confirmation Dialog (NPSPEC-BOOTDIALOG-0004)

Bestätigungen besitzen nun einen eigenen statischen Vertrag für Information,
Warning, Critical und Security. Der Dialog wird mitsamt seiner festen
Buttonliste atomar validiert; ungültige Stufen, leere Inhalte, fehlende
Standardaktionen und ungültige Ergebnisse werden sicher abgewiesen. Bei
Information liegt der Fokus auf der harmlosen Bestätigung, bei allen
zustandsändernden Stufen stets auf Abbrechen. Critical und Security verlangen
nach der bewussten Fokusbewegung zusätzlich eine zweite Aktivierung.

Alle acht definierten Ergebnisse einschließlich Continue sind typisiert. Eine
Entscheidung kann nur einmal geschlossen und abgefragt werden. Stufe, Titel,
Entscheidung, Fehler, Fokuswechsel sowie Öffnungs- und Schließzeit werden ohne
Heap erfasst. Semantische Information-, Warning-, Error- und Lock-Icons sowie
Warnfarben werden mit Glass, Modal Backdrop, Safe Area, High Contrast und
Reduced Motion kombiniert. Herunterfahren, Neustart und Firmware-Setup nutzen
den neuen Pfad produktiv.

Hosttests prüfen alle vier Stufen, fehlerhafte Requests, sicheren Fokus,
Modalität, Doppelaktivierung und Diagnosen. Der gezielte QEMU-Test navigiert
bis zum realen Herunterfahren-Dialog und belegt `CONFIRMATION-SAFE-DEFAULT`,
`MODAL-BACKDROP-FRAME`, einen stabilen Confirmation-Frame sowie Escape als
Cancel; das Referenzbild liegt in `build/uefi-confirmation.png`. Offen bleiben
die optionale Verkettung mit Passwort-/Bestätigungstextdialogen,
Screenreader-/Touch-/Controllerprovider und Legacy-BIOS-Parität.

## Warning Dialog (NPSPEC-BOOTDIALOG-0005)

Warnungen besitzen nun einen spezialisierten statischen Vertrag für Low,
Medium, High und Critical. Sechs feste Ursachenklassen wählen semantisch
zwischen allgemeiner Warnung, Datenträger, Sicherheit, Netzwerk, Speicher und
Temperatur. Nachricht, empfohlene Maßnahme und optionaler Warncode werden
innerhalb der Safe Area dargestellt. Low fokussiert das harmlose OK;
Medium, High und Critical fokussieren Abbrechen. Critical verwendet zusätzlich
das Zweifach-Arming vor einer Fortsetzung.

Der Dialog kann modal oder nichtmodal geöffnet werden. Modale Warnungen nutzen
Backdrop und vollständige Eingabe-/Navigationssperre, nichtmodale Warnungen
lassen den Seitenkontext aktiv. Ungültige Stufen, Symbole, Nulltexte oder
beschädigte Requests werden nicht still verworfen: stattdessen erscheint eine
generische sichere Warnung mit Diagnoseempfehlung. Stufe, Symbol, Code,
Entscheidung, Zeit, Fokus und Fehlerzähler bleiben heapfrei erfasst.

Recovery-, Firmware- und nicht verfügbare Neustartziele nutzen den Pfad
produktiv. Hosttests prüfen alle Stufen, beide Modalitätsarten, sicheren Fokus,
Fehlerfallback und Exactly-once-Ergebnisse. Der QEMU-Test navigiert zum realen
Recovery-Hinweis und belegt `WARNING-DIALOG-OPEN`, den stabilen Warning-Frame,
Modal Backdrop und Escape als Cancel. Das Referenzbild liegt unter
`build/uefi-warning.png`. Offen bleiben ein aufklappbarer technischer
Detailbereich, reale Hardwarewarnprovider, Screenreader-/Touch-/Controller und
Legacy-BIOS-Parität.

## UEFI-Pointerbewegung

Der Firmwareadapter bindet nicht länger nur das erste von `LocateProtocol`
gelieferte Zeigegerät. Er enumeriert bis zu acht relative Simple-Pointer- und
acht Absolute-Pointer-Handles und pollt alle aktiven Geräte. Dadurch kann ein
vorhandener, aber inaktiver PS/2-Handle eine tatsächlich verwendete USB-Maus
oder ein virtuelles Tablet nicht mehr verdecken. Hotplug ersetzt die feste
Handlemenge atomar und setzt Geräte nur bei einer wirklichen Änderung zurück.

Absolute Firmwarekoordinaten werden anhand der Min-/Max-Werte des UEFI-Modus
direkt auf die aktuelle GOP-Auflösung normalisiert. Relative Geräte durchlaufen
weiterhin den konfigurierbaren linearen Geschwindigkeitspfad. Dessen bisherige
Fehlskalierung wurde korrigiert: 100 Prozent entsprechen jetzt exakt 1:1 statt
12,5 Prozent; 25 und 400 Prozent entsprechen 0,25x und 4x. Kleine Deltas bleiben
mindestens einen Pixel sichtbar. Beide Protokolle verwenden anschließend
dasselbe Hit Testing, Hover-, Klick-, Capture- und Damage-System.

Der isolierte Firmwaretest deckt relative und absolute Protokolle,
Bildschirmnormalisierung, beide Buttons, Entfernung und Wiederanmeldung ab.
Der Boot-UI-Test prüft die neue Prozentsemantik; das aktualisierte GPT/FAT32-
Image durchläuft den vollständigen OVMF/QEMU-Smoke-Test. Eine automatisierte
headless QEMU-Mausbewegung bleibt durch die Eingabekonsole dieser QEMU/OVMF-
Kombination nicht zuverlässig reproduzierbar und wird deshalb nicht als
erfolgreicher Hardwaretest ausgegeben.
