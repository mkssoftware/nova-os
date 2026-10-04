# NPSPEC-BOOT-VIEW-SWITCHING-001 – NovaOS Boot View Switching

## Status

Angenommen

## Kategorie

Boot / Benutzeroberfläche

## Zweck

Diese Spezifikation definiert den Wechsel zwischen den Bootansichten von NovaOS.

Der Benutzer kann während des laufenden Bootvorgangs zwischen Boot Splash und Boot Console wechseln, ohne den Bootprozess zu unterbrechen, neu zu starten oder dessen Zustand zu verändern.

## Grundprinzip

Boot Splash und Boot Console sind zwei Darstellungen desselben laufenden Bootvorgangs.

Der Wechsel betrifft ausschließlich die sichtbare Ansicht.

Folgende Zustände laufen unabhängig von der aktuell sichtbaren Ansicht weiter:

- Bootfortschritt
- aktuelle Bootphase
- Initialisierung
- Logerfassung
- Warnungen
- Fehler
- Systemstatus

Die Bootansichten dürfen keine voneinander getrennten Bootzustände besitzen.

## Bootansichten

Der Mechanismus verwaltet mindestens:

- `BootSplash`
- `BootConsole`

Beim normalen Systemstart ist `BootSplash` die aktive Standardansicht.

## Eingabe

Im Boot Splash gilt:

`F3` → Boot Console öffnen

In der Boot Console gilt:

`ESC` → Boot Splash öffnen

Andere Eingaben dürfen den Ansichtswechsel nicht unbeabsichtigt auslösen.

## Ansichtswechsel

Ein Wechsel erfolgt nach folgendem Prinzip:

```text
Boot Splash
    |
   F3
    |
    v
Boot Console
    |
   ESC
    |
    v
Boot Splash
```

Der Wechsel darf ausschließlich die aktive Darstellung verändern.

Der Bootprozess selbst läuft währenddessen weiter.

## Zustandsübergänge

Der Ansichtsstatus kann mindestens folgende Zustände besitzen:

```text
BOOT_VIEW_SPLASH
BOOT_VIEW_CONSOLE
```

Zulässige Übergänge:

```text
BOOT_VIEW_SPLASH  -> BOOT_VIEW_CONSOLE
BOOT_VIEW_CONSOLE -> BOOT_VIEW_SPLASH
```

Ein Ansichtswechsel darf keinen Wechsel einer Bootphase erzwingen.

## Synchronisierung

Beide Bootansichten verwenden dieselbe zentrale Bootzustandsquelle.

Synchron bleiben insbesondere:

- Gesamtfortschritt
- aktuelle Bootphase
- Bootzeit
- Komponentenstatus
- Warnungen
- Fehler
- Logeinträge

Die sichtbare Ansicht liest den aktuellen Zustand und stellt ihn entsprechend ihrer eigenen Darstellung dar.

## Logerfassung

Die Erfassung von Bootmeldungen läuft unabhängig von der sichtbaren Ansicht.

Ist der Boot Splash aktiv, werden Logmeldungen weiterhin gesammelt.

Wird anschließend die Boot Console geöffnet, müssen bereits erzeugte Meldungen verfügbar sein.

Das Schließen der Boot Console darf keine Logdaten löschen.

## Fortschritt

Der Wechsel zwischen den Ansichten darf den dargestellten Bootfortschritt nicht zurücksetzen.

Beispiel:

```text
Boot Splash:   42 %
       |
      F3
       v
Boot Console:  42 %
       |
      ESC
       v
Boot Splash:   aktueller Fortschritt
```

Der Fortschritt wird ausschließlich vom gemeinsamen Bootsystem bereitgestellt.

## Verhalten bei Fehlern

Warnungen und Fehler verändern den View-Switching-Mechanismus grundsätzlich nicht.

Tritt während des Boot Splash ein Fehler auf, wird dieser im gemeinsamen Bootlog erfasst.

Der Benutzer kann mit `F3` zur Boot Console wechseln und den Fehler dort einsehen.

Bei einem fatalen Bootfehler muss die Boot Console weiterhin als Diagnoseansicht verfügbar bleiben.

Ein separater Fehlerbildschirm wird nicht erzeugt.

## Verhalten bei abgeschlossenem Boot

Wird der Bootvorgang erfolgreich abgeschlossen, endet der Boot-View-Modus.

Dies gilt unabhängig davon, ob aktuell:

- der Boot Splash oder
- die Boot Console

sichtbar ist.

Anschließend übernimmt die reguläre NovaOS-Benutzeroberfläche.

## Performance

Der Ansichtswechsel muss unmittelbar erfolgen und darf den Bootprozess nicht wesentlich beeinflussen.

Ein vollständiges Neuinitialisieren des Bootsystems oder der Bootzustände beim Ansichtswechsel ist nicht zulässig.

Grafische Ressourcen der jeweils anderen Ansicht dürfen verwaltet oder neu gezeichnet werden, solange der gemeinsame Bootzustand erhalten bleibt.

## Normative Anforderungen

1. Der Boot Splash MUSS die Standardansicht des normalen Bootvorgangs sein.
2. `F3` MUSS vom Boot Splash zur Boot Console wechseln.
3. `ESC` MUSS von der Boot Console zum Boot Splash wechseln.
4. Der Ansichtswechsel DARF den Bootvorgang NICHT unterbrechen.
5. Der Ansichtswechsel DARF den Bootvorgang NICHT neu starten.
6. Boot Splash und Boot Console MÜSSEN denselben zentralen Bootzustand verwenden.
7. Der Bootfortschritt DARF durch einen Ansichtswechsel NICHT zurückgesetzt werden.
8. Logmeldungen MÜSSEN unabhängig von der sichtbaren Ansicht weiter erfasst werden.
9. Bereits erzeugte Logmeldungen DÜRFEN beim Ansichtswechsel NICHT verloren gehen.
10. Warnungen und Fehler MÜSSEN auch dann erfasst werden, wenn der Boot Splash sichtbar ist.
11. Der Wechsel der Ansicht DARF keine Bootphase erneut ausführen.
12. Ein fataler Bootfehler MUSS weiterhin den Zugriff auf die Boot Console ermöglichen.
13. Nach erfolgreichem Bootabschluss MUSS der Boot-View-Modus unabhängig von der aktuell sichtbaren Ansicht beendet werden.
14. Der Ansichtswechsel DARF den Systemstart nicht wesentlich verzögern.

## Abhängigkeiten

- `NPSPEC-BOOT-SPLASH-001`
- `NPSPEC-BOOT-CONSOLE-001`

## Ergebnis

NovaOS besitzt einen einheitlichen Mechanismus zum Umschalten zwischen Boot Splash und Boot Console.

`F3` öffnet während des Boot Splash die Boot Console und `ESC` führt zurück zum Boot Splash. Beide Ansichten verwenden denselben laufenden Bootzustand, während Initialisierung, Fortschritt und Logerfassung ohne Unterbrechung weiterlaufen.