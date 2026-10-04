# NPSPEC-BOOT-CONSOLE-001 – NovaOS Boot Console

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die NovaOS Boot Console als zentrale technische Status-, Diagnose- und Fehleransicht während des Systemstarts.

Die Boot Console ersetzt alle bisherigen separaten Boot-Fehlerbildschirme. Sämtliche während des Bootvorgangs auftretenden Informationen, Warnungen und Fehler werden innerhalb dieser Konsole dargestellt.

## Grundprinzip

Boot Splash und Boot Console sind zwei Ansichten desselben laufenden Bootvorgangs.

Die Boot Console besitzt keine eigene Bootlogik und startet keinen separaten Diagnosemodus.

Sie erhält ihren Zustand direkt vom NovaOS-Bootsystem.

Ein Wechsel zwischen Boot Splash und Boot Console darf den Bootvorgang weder unterbrechen noch verändern.

## Darstellung

Die freigegebene NovaOS-BootConsole-Darstellung ist die visuelle Sollvorgabe.

Die Boot Console wird über demselben Weltraum-/Erde-Hintergrund wie der Boot Splash dargestellt.

Die Konsole besteht aus einem großen halbtransparenten glasartigen Panel mit:

- dunkler transparenter Grundfläche,
- blauem Fluent-/Acrylic-Erscheinungsbild,
- weichen Schatten,
- dezentem blauem Glow,
- abgerundeten Ecken,
- klarer Trennung zwischen Hintergrund und Konsoleninhalt.

Das Konsolenfenster besitzt zu allen Bildschirmrändern einen Abstand von maximal etwa 3 %.

## Kopfbereich

Im oberen Bereich werden mindestens dargestellt:

- `NovaOS Boot Console`
- `System Initialization and Diagnostics`
- aktuelle Build-Information
- aktuelle Bootzeit beziehungsweise Laufzeit

Die Informationen müssen klar vom eigentlichen Bootlog getrennt sein.

## Bootlog

Der zentrale Bereich enthält den fortlaufenden technischen Bootlog.

Ein Logeintrag kann enthalten:

- Zeitstempel,
- Log-Level,
- Komponente,
- Status,
- Meldung.

Neue Meldungen werden während des laufenden Bootvorgangs kontinuierlich ergänzt.

Der sichtbare Bereich folgt standardmäßig den neuesten Einträgen.

Bereits erzeugte Meldungen bleiben während des Bootvorgangs erhalten.

## Log-Level

Die Boot Console verwendet mindestens folgende sichtbaren Log-Level:

- `INFO`
- `WARN`
- `ERROR`

Die Level werden sowohl textuell als auch farblich unterschieden.

### INFO

`INFO` verwendet eine blaue beziehungsweise cyanfarbene NovaOS-Akzentfarbe.

Es kennzeichnet normale Status- und Initialisierungsmeldungen.

### WARN

`WARN` verwendet Gelb beziehungsweise Amber.

Es kennzeichnet Probleme oder ungewöhnliche Zustände, bei denen der Bootvorgang grundsätzlich fortgesetzt werden kann.

### ERROR

`ERROR` verwendet Rot.

Es kennzeichnet fehlgeschlagene Operationen oder Fehlerzustände.

Die Farbe allein darf nicht die einzige Möglichkeit zur Unterscheidung der Log-Level sein. Das jeweilige Level muss zusätzlich als Text sichtbar sein.

## Fehlerdarstellung

Alle bisherigen separaten NovaOS-Boot-Fehlerscreens werden entfernt.

Bootfehler werden ausschließlich innerhalb der Boot Console dargestellt.

Dies gilt insbesondere für:

- Kernel-Initialisierungsfehler,
- Treiberfehler,
- Gerätefehler,
- Dateisystemfehler,
- Dienstfehler,
- Konfigurationsfehler,
- Sicherheitsfehler,
- sonstige Fehler während des Bootvorgangs.

Ein Fehler darf den Benutzer nicht automatisch auf einen separaten Fehlerbildschirm umleiten.

Kann NovaOS trotz eines Fehlers weiter booten, bleibt die Boot Console als normale Diagnoseansicht verfügbar.

Kann der Bootvorgang nicht fortgesetzt werden, bleibt die Boot Console sichtbar und zeigt den finalen Fehlerzustand.

## Fortschrittsanzeige

Im unteren Bereich der Boot Console wird der aktuelle Gesamtfortschritt des Bootvorgangs dargestellt.

Die Fortschrittsanzeige besteht aus:

- horizontaler Fortschrittsleiste,
- NovaOS-blauem Fortschrittssegment,
- Prozentwert.

Der angezeigte Fortschritt muss mit dem Fortschritt des Boot Splash synchron sein.

Die Boot Console berechnet keinen eigenen Bootfortschritt.

## Bootphasen

Zusätzlich zum Gesamtfortschritt können die wesentlichen Bootbereiche dargestellt werden.

Die vorgesehene Darstellung umfasst:

- Kernel
- Drivers
- Services
- Desktop

Jeder Bereich zeigt seinen aktuellen Zustand.

Mögliche Zustände sind beispielsweise:

- ausstehend,
- wird initialisiert,
- erfolgreich,
- Warnung,
- Fehler.

Die Boot Console stellt nur vom Bootsystem bereitgestellte Zustände dar.

## Eingabe

Während die Boot Console aktiv ist, gilt:

`ESC` → zurück zum NovaOS Boot Splash

Der Wechsel erfolgt über den gemeinsamen Boot-View-Mechanismus.

Der laufende Bootvorgang wird dabei nicht angehalten oder neu gestartet.

Nach einem erneuten Wechsel zur Boot Console werden der aktuelle Bootzustand und die vorhandenen Logeinträge weiter angezeigt.

## Synchronisation

Boot Splash und Boot Console verwenden denselben Bootzustand.

Zwischen beiden Ansichten müssen mindestens synchron bleiben:

- Gesamtfortschritt,
- aktuelle Bootphase,
- Systemzustände,
- Warnungen,
- Fehler,
- Logdaten.

Die Ansichten dürfen keine voneinander unabhängigen Bootzustände verwalten.

## Verhalten bei abgeschlossenem Boot

Wird der Systemstart erfolgreich abgeschlossen, übergibt das Bootsystem die Kontrolle an die reguläre NovaOS-Benutzeroberfläche.

Ist zu diesem Zeitpunkt die Boot Console geöffnet, darf dies den Übergang zum Desktop nicht verhindern.

## Verhalten bei fatalem Fehler

Kann der Bootvorgang aufgrund eines fatalen Fehlers nicht fortgesetzt werden:

- bleibt die Boot Console sichtbar,
- wird der betreffende Fehler als `ERROR` dargestellt,
- bleibt der bereits erzeugte Bootlog verfügbar,
- darf kein separater Fehlerbildschirm erscheinen.

Weitere Recovery- oder Neustartaktionen werden durch dafür vorgesehene Komponenten definiert.

## Performance

Die Boot Console muss mit den während des Bootvorgangs verfügbaren Grafikfunktionen funktionieren.

Sie darf keine Abhängigkeit von der später gestarteten NovaOS-Desktopumgebung besitzen.

Das Anzeigen und Aktualisieren des Logs darf den eigentlichen Bootvorgang nicht wesentlich verzögern.

Die Logerzeugung darf nicht davon abhängig sein, ob die Boot Console gerade sichtbar ist.

## Normative Anforderungen

1. Die Boot Console MUSS über `F3` vom Boot Splash aus erreichbar sein.
2. `ESC` MUSS aus der Boot Console zum Boot Splash zurückführen.
3. Ein Ansichtswechsel DARF den Bootvorgang NICHT unterbrechen oder neu starten.
4. Boot Splash und Boot Console MÜSSEN denselben Bootzustand verwenden.
5. Die Boot Console MUSS laufende Bootmeldungen darstellen können.
6. `INFO`, `WARN` und `ERROR` MÜSSEN eindeutig unterscheidbar sein.
7. `INFO` MUSS blau beziehungsweise cyan dargestellt werden.
8. `WARN` MUSS gelb beziehungsweise amber dargestellt werden.
9. `ERROR` MUSS rot dargestellt werden.
10. Das Log-Level MUSS zusätzlich zur Farbe textuell dargestellt werden.
11. Alle bisherigen separaten Boot-Fehlerbildschirme MÜSSEN durch die Boot Console ersetzt werden.
12. Bootfehler MÜSSEN innerhalb der Boot Console dargestellt werden.
13. Die Boot Console MUSS auch bei einem fatal abgebrochenen Bootvorgang sichtbar bleiben können.
14. Die Fortschrittsanzeige MUSS mit dem tatsächlichen Bootfortschritt synchronisiert sein.
15. Die Boot Console DARF keinen eigenen künstlichen Bootfortschritt erzeugen.
16. Logmeldungen MÜSSEN unabhängig von der aktuell sichtbaren Bootansicht erfasst werden.
17. Die Boot Console DARF keine Abhängigkeit von der NovaOS-Desktopumgebung besitzen.
18. Die Darstellung DARF den eigentlichen Bootvorgang nicht wesentlich verzögern.

## Abgrenzung

Diese Spezifikation definiert die Darstellung und das Verhalten der NovaOS Boot Console.

Nicht Bestandteil dieser Spezifikation sind:

- die eigentliche Bootlogik,
- Ermittlung des Bootfortschritts,
- persistente Speicherung von Bootlogs,
- Recovery-Aktionen,
- Neustartlogik,
- Boot Splash,
- allgemeine Logik zum Wechsel zwischen Bootansichten.

## Zugehörige NPSPECs

- `NPSPEC-BOOT-SPLASH-001`
- `NPSPEC-BOOT-VIEW-SWITCHING-001`