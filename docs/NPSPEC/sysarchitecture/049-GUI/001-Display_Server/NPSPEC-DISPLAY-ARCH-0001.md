# NPSPEC-DISPLAY-ARCH-0001 – Nova Display Architecture

## Status

Angenommen

## Kategorie

NovaOS / Display / Architektur

## Zweck

Definiert die grundlegende Architektur des NovaOS-Displaysystems.

Ziel ist eine performante, modulare und hardwareunabhängige Darstellung grafischer Oberflächen mit Unterstützung für mehrere Bildschirme, Hardwarebeschleunigung und unterschiedliche Grafikgeräte.

Das Displaysystem bildet die Grundlage für Compositor, Window Manager, Nova UI und sämtliche grafischen Anwendungen.

## Architektur

Das Displaysystem besteht aus:

- **Display Manager:** Verwaltung und Konfiguration angeschlossener Bildschirme.
- **Display HAL:** Hardwareunabhängige Schnittstelle zu Grafikgeräten.
- **Display Driver Interface:** Anbindung von GPU- und Displaytreibern.
- **Display Output Manager:** Verwaltung physischer und virtueller Bildschirmausgänge.
- **Display Mode Manager:** Auflösungen, Bildwiederholraten und Farbtiefen.
- **Framebuffer Manager:** Verwaltung der Ausgabepuffer.
- **Display Synchronization:** Synchronisation der Bildausgabe.
- **Display Event Manager:** Verarbeitung von Anzeigeereignissen.

Die Komponenten verwenden definierte Schnittstellen und können unabhängig weiterentwickelt werden.

## Architekturmodell

Grafische Anwendungen greifen nicht unmittelbar auf die Displayhardware zu.

Der reguläre Darstellungsweg lautet:

Nova UI / Anwendungen
    ↓
Window Manager
    ↓
Compositor
    ↓
Graphics Renderer
    ↓
Display Subsystem
    ↓
Display HAL / Grafiktreiber
    ↓
GPU / Displayhardware

Der Window Manager verwaltet Fensterzustände und Anordnung.

Der Compositor setzt die sichtbaren Oberflächen zusammen.

Der Graphics Renderer erzeugt die benötigten Bilddaten.

Das Display Subsystem übernimmt deren kontrollierte Ausgabe.

## Display-Abstraktion

Jeder Bildschirm wird als eigenständiges Display-Objekt verwaltet.

Ein Display besitzt:

- Eindeutige Display-ID
- Zugeordneten Grafikausgang
- Verfügbare Anzeigemodi
- Aktive Auflösung
- Bildwiederholrate
- Farbtiefe und Farbformat
- Skalierungsfaktor
- Ausrichtung
- Betriebszustand

Physische und virtuelle Displays verwenden eine gemeinsame Schnittstelle.

## Mehrbildschirmbetrieb

Unterstützt werden:

- Einzelbildschirmbetrieb
- Erweiterter Desktop
- Bildschirmspiegelung
- Unterschiedliche Auflösungen
- Unterschiedliche Bildwiederholraten
- Individuelle Skalierung
- Dynamisches Anschließen und Entfernen

Änderungen der Displaykonfiguration dürfen laufende Anwendungen nicht unnötig beeinträchtigen.

## Framebuffer und Bildausgabe

Das Display Subsystem verwaltet die für die Ausgabe benötigten Framebuffer.

Unterstützt werden:

- Single Buffering
- Double Buffering
- Triple Buffering
- Page Flipping
- Synchronisierte Bildübergabe
- Partielle Aktualisierungen, sofern unterstützt

Die Auswahl erfolgt abhängig von Hardwarefähigkeiten und Leistungsanforderungen.

## Hardwarebeschleunigung

Die Architektur unterstützt GPU-beschleunigte Bildausgabe.

Hardwareabhängige Funktionen werden über die Display HAL und Grafiktreiber bereitgestellt.

Ein softwarebasierter Fallback muss für grundlegende Bildausgaben verfügbar sein.

## Synchronisation

Die Bildausgabe berücksichtigt:

- Vertical Synchronization
- Frame Presentation
- Bildwiederholraten
- Vermeidung von Screen Tearing
- Minimierung unnötiger Latenzen

Variable Refresh Rate kann bei geeigneter Hardware unterstützt werden.

## Boot und Initialisierung

Das Displaysystem unterstützt eine mehrstufige Initialisierung:

1. **Early Display:** Einfache Framebuffer-Ausgabe während des Bootvorgangs.
2. **Display Initialization:** Erkennung und Initialisierung der Grafikgeräte.
3. **Native Display:** Aktivierung der regulären Displaytreiber.
4. **Desktop Ready:** Bereitstellung der Bildausgabe für den Compositor.

Der Wechsel zwischen den Phasen soll ohne unnötiges Flackern erfolgen.

Die grafische Oberfläche darf bereits starten, während nicht benötigte Displayfunktionen im Hintergrund initialisiert werden.

## Fehlerbehandlung

Bei Fehlern einzelner Displays oder Treiber muss das System kontrolliert reagieren.

Mögliche Maßnahmen:

- Wiederherstellung der Displaykonfiguration
- Neuinitialisierung eines Grafikausgangs
- Wechsel auf einen sicheren Anzeigemodus
- Softwarebasierte Bildausgabe
- Kontrollierter Neustart betroffener Displaykomponenten

Ein Displayfehler darf nicht zwangsläufig einen vollständigen Systemabsturz verursachen.

## Sicherheit und Isolation

Anwendungen dürfen Displayhardware nicht ohne entsprechende Berechtigung direkt kontrollieren.

Framebuffer und Grafikspeicher müssen gegen unberechtigte Zugriffe geschützt werden.

Die Displayverwaltung muss mit dem NovaOS-Capability- und Ressourcenmodell kompatibel sein.

## Normative Anforderungen

1. NovaOS MUSS ein eigenständiges Display Subsystem besitzen.
2. Das Displaysystem MUSS hardwareunabhängige Schnittstellen bereitstellen.
3. Displaytreiber MÜSSEN über definierte Treiberschnittstellen angebunden werden.
4. Physische und virtuelle Displays MÜSSEN einheitlich verwaltbar sein.
5. Mehrbildschirmbetrieb MUSS unterstützt werden.
6. Auflösung, Bildwiederholrate und Skalierung MÜSSEN konfigurierbar sein.
7. Framebuffer-Verwaltung und synchronisierte Bildausgabe MÜSSEN unterstützt werden.
8. GPU-Beschleunigung SOLL verwendet werden, sofern verfügbar.
9. Eine grundlegende Software- oder Framebuffer-Ausgabe MUSS als Fallback verfügbar sein.
10. Displayänderungen MÜSSEN als Ereignisse bereitgestellt werden.
11. Das Displaysystem MUSS einen frühen Boot-Framebuffer unterstützen.
12. Nicht benötigte Displaykomponenten SOLLEN verzögert initialisiert werden können.
13. Fehler einzelner Displaykomponenten MÜSSEN möglichst isoliert behandelt werden.
14. Anwendungen DÜRFEN keine unautorisierten direkten Hardwarezugriffe durchführen.
15. Das Displaysystem MUSS unabhängig von Window Manager, Compositor und Nova UI initialisierbar sein.
16. Die grundlegende Bildausgabe MUSS ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine modulare, hardwareunabhängige und ausfallsichere Displayarchitektur als technische Grundlage für Grafiktreiber, Compositor, Window Manager und Nova UI.

Die Architektur ermöglicht schnelle Desktop-Initialisierung, moderne Mehrbildschirmkonfigurationen und eine schrittweise Erweiterung der Grafikfunktionen.