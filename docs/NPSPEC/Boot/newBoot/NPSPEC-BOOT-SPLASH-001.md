# NPSPEC-BOOT-SPLASH-001 – NovaOS Boot Splash

## Status

Angenommen

## Zweck

Diese Spezifikation definiert den normalen grafischen Bootbildschirm von NovaOS.

Der Boot Splash stellt den laufenden Systemstart visuell dar und bildet die standardmäßige Benutzeransicht während des Bootvorgangs.

## Grundprinzip

Der Boot Splash ist ausschließlich eine Darstellung des laufenden Bootprozesses.

Er enthält keine eigene Bootlogik und berechnet den Bootfortschritt nicht selbst.

Der tatsächliche Zustand des Bootvorgangs wird vom NovaOS-Bootsystem bereitgestellt.

Der Boot Splash bleibt aktiv, bis entweder:

- der Systemstart abgeschlossen ist,
- zur Boot Console gewechselt wird.

## Visuelles Design

Die freigegebene NovaOS-BootSplash-Darstellung ist die visuelle Sollvorgabe.

Der Bildschirm besteht aus:

- dunkelblauem bis schwarzem Weltraumhintergrund,
- Sternen und sichtbaren kosmischen Strukturen,
- der Erdkrümmung im unteren Bildbereich,
- Europa bei Nacht mit sichtbaren Stadtlichtern,
- einem Sonnenaufgang am rechten Horizont,
- dem zentralen Nova-Stern,
- einem dezenten kreisförmigen Halo um den Nova-Stern,
- dem Schriftzug `NovaOS`,
- einer schmalen Fortschrittsanzeige.

`Nova` wird hell bzw. weiß dargestellt.

`OS` wird im NovaOS-Blau dargestellt.

Nova-Stern, Schriftzug und Fortschrittsanzeige befinden sich auf einer gemeinsamen vertikalen Mittelachse.

## Fortschrittsanzeige

Unterhalb des NovaOS-Schriftzuges befindet sich eine schmale horizontale Fortschrittsanzeige.

Sie besteht aus:

- einer dezenten Grundfläche,
- einem blau leuchtenden Fortschrittssegment,
- einem weichen Glow-Effekt.

Die Anzeige repräsentiert den vom Bootsystem bereitgestellten Gesamtfortschritt.

Der Boot Splash darf keinen eigenen künstlichen Fortschritt erzeugen.

Ein bereits dargestellter Fortschritt darf während eines normalen Bootvorgangs nicht sichtbar zurückspringen.

## Bootzustand

Der Boot Splash wird während des normalen NovaOS-Systemstarts angezeigt.

Der eigentliche Bootprozess läuft unabhängig von der Darstellung weiter.

Ein Wechsel der Bootansicht darf:

- keinen Bootvorgang neu starten,
- keine Bootphase wiederholen,
- keinen Systemzustand verlieren,
- den Bootvorgang nicht anhalten.

Nach erfolgreichem Abschluss des Bootvorgangs wird der Boot Splash beendet und die reguläre NovaOS-Benutzeroberfläche übernommen.

## Eingabe

Während der Boot Splash aktiv ist, besitzt folgende Taste eine definierte Funktion:

`F3` → Wechsel zur NovaOS Boot Console

Die Verarbeitung des Ansichtswechsels wird durch `NPSPEC-BOOT-VIEW-SWITCHING-001` definiert.

Der Bootprozess läuft während des Wechsels unverändert weiter.

## Meldungen

Der normale Boot Splash zeigt keine technischen Bootmeldungen an.

Nicht dargestellt werden insbesondere:

- Kernelmeldungen,
- Treibermeldungen,
- Diagnoseinformationen,
- Informationsmeldungen,
- Warnungen,
- Fehlermeldungen,
- interne Bootphasen,
- Debug-Ausgaben.

Diese Informationen gehören zur NovaOS Boot Console.

Separate klassische Fehlerbildschirme sind nicht Bestandteil des Boot Splash.

## Skalierung

Der Boot Splash muss sich an unterschiedliche Bildschirmauflösungen und Seitenverhältnisse anpassen können.

Die Gestaltung darf dabei nicht verzerrt werden.

Die relative Position von:

- Nova-Stern,
- NovaOS-Schriftzug,
- Fortschrittsanzeige,
- Erde,
- Horizont

muss erhalten bleiben.

Bei abweichenden Seitenverhältnissen darf der Hintergrund proportional beschnitten werden.

Sichtbare leere Bildschirmränder sind zu vermeiden.

## Performance

Der Boot Splash muss mit den während der frühen Bootphase verfügbaren Grafikfunktionen darstellbar sein.

Er darf keine Abhängigkeit von der später gestarteten NovaOS-Desktopumgebung besitzen.

Visuelle Effekte dürfen den Systemstart nicht wesentlich verzögern.

Falls bestimmte grafische Effekte technisch noch nicht verfügbar sind, muss eine vereinfachte Darstellung möglich sein, ohne den Bootprozess zu beeinflussen.

## Beispiel

```text
Systemstart
    |
    v
Bootsystem initialisieren
    |
    v
NovaOS Boot Splash anzeigen
    |
    +------ F3 ------> Boot Console
    |                     |
    |                  ESC / Wechsel
    |                     |
    <---------------------+
    |
    v
Bootfortschritt aktualisieren
    |
    v
Systemstart abgeschlossen
    |
    v
NovaOS Benutzeroberfläche
```

## Normative Anforderungen

1. Der Boot Splash MUSS während des normalen NovaOS-Systemstarts als Standardansicht verwendet werden.
2. Die Darstellung MUSS dem festgelegten NovaOS-BootSplash-Design entsprechen.
3. Nova-Stern, Schriftzug und Fortschrittsanzeige MÜSSEN zentral ausgerichtet sein.
4. Die Fortschrittsanzeige MUSS den vom Bootsystem bereitgestellten Bootfortschritt darstellen.
5. Der Boot Splash DARF keinen eigenen Bootfortschritt simulieren.
6. Technische Boot-, Diagnose-, Warn- oder Fehlermeldungen DÜRFEN NICHT direkt auf dem Boot Splash dargestellt werden.
7. `F3` MUSS den Wechsel zur NovaOS Boot Console ermöglichen.
8. Ein Ansichtswechsel DARF den laufenden Bootprozess NICHT unterbrechen oder neu starten.
9. Der Boot Splash MUSS unabhängig von der NovaOS-Desktopumgebung funktionieren.
10. Die Darstellung MUSS ohne sichtbare Verzerrung an unterstützte Bildschirmauflösungen angepasst werden können.
11. Der Boot Splash DARF den eigentlichen Bootvorgang nicht wesentlich verzögern.
12. Nach erfolgreichem Abschluss des Bootvorgangs MUSS die Kontrolle an die reguläre NovaOS-Benutzeroberfläche übergeben werden.

## Abgrenzung

Diese Spezifikation definiert ausschließlich den normalen NovaOS Boot Splash.

Nicht Bestandteil dieser Spezifikation sind:

- Darstellung und Aufbau der Boot Console,
- Format und Speicherung von Bootlogs,
- Farben und Darstellung einzelner Log-Level,
- Navigation innerhalb der Boot Console,
- allgemeine Logik zum Wechsel zwischen Bootansichten,
- eigentliche Boot- und Initialisierungslogik,
- Berechnung des Bootfortschritts.

Diese Funktionen werden durch separate NPSPECs definiert.

## Zugehörige NPSPECs

- `NPSPEC-BOOT-CONSOLE-001`
- `NPSPEC-BOOT-VIEW-SWITCHING-001`