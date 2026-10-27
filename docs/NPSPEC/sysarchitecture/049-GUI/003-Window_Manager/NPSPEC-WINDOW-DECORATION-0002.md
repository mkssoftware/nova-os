# NPSPEC-WINDOW-DECORATION-0002 – Nova Title Bars

**Status:** Angenommen  
**Bereich:** Window Manager / Fenstertitelleisten  
**System:** NovaOS

## 1. Zweck

Nova Title Bars definiert Aufbau, Darstellung und Verhalten der Titelleisten innerhalb der NovaOS-Fensterverwaltung.

Ziel ist eine einheitliche, intuitive und anpassbare Titelleiste, die grundlegende Fensteroperationen bereitstellt und sich harmonisch in das native Nova UI-System integriert.

## 2. Architektur

- Verwaltung der Titelleisten über Nova Window Decorations.
- Darstellung durch Nova UI und den Compositor.
- Fensteroperationen werden ausschließlich durch den Window Manager ausgeführt.
- Trennung zwischen visueller Gestaltung und funktionalem Verhalten.
- Unterstützung systemseitiger und anwendungsseitiger Titelleisten.
- Deklarative Gestaltung über Styles und Templates.
- DPI-unabhängige Darstellung.
- Integration in Focus Management, Window State Model und Input System.

## 3. Aufbau

Eine Standardtitelleiste besteht aus folgenden Bereichen:

| Bereich | Funktion |
|---|---|
| Icon Area | Anwendungssymbol und Systemmenü |
| Title Area | Fenstertitel und optionaler Status |
| Custom Content | Optionale anwendungsspezifische Elemente |
| Window Controls | Minimieren, Maximieren und Schließen |
| Drag Region | Bereich zum Verschieben des Fensters |

Die Anordnung MUSS sich an die verfügbare Fensterbreite anpassen.

Anwendungen DÜRFEN zusätzliche Inhalte wie Suchfelder, Registerkarten oder Werkzeugleisten integrieren.

## 4. Titelleistenvarianten

NovaOS unterstützt folgende Varianten:

| Variante | Beschreibung |
|---|---|
| Standard | Klassische Titelleiste mit Fensterbedienelementen |
| Compact | Platzsparende Titelleiste |
| Extended | Erweiterte Titelleiste mit zusätzlichen Inhalten |
| Integrated | Titelleiste in die Anwendungsoberfläche integriert |
| Hidden | Ausgeblendete Titelleiste, beispielsweise im Vollbildmodus |

Die verwendete Variante richtet sich nach Fensterart, Benutzerpräferenzen und Anwendungsanforderungen.

## 5. Normative Anforderungen

- WINDOW-001: Reguläre Fenster MÜSSEN standardmäßig eine systemseitige Titelleiste erhalten.
- WINDOW-002: Titelleisten MÜSSEN den aktuellen Fensterzustand korrekt widerspiegeln.
- WINDOW-003: Fensteroperationen MÜSSEN durch den Window Manager autorisiert werden.
- WINDOW-004: Titelleisten MÜSSEN DPI-unabhängig skalierbar sein.
- WINDOW-005: Die Standardtitelleiste MUSS Maus-, Touch- und Tastaturbedienung unterstützen.
- WINDOW-006: Anwendungen DÜRFEN eigene Titelleisten über definierte Nova UI-Schnittstellen bereitstellen.
- WINDOW-007: Interaktive Inhalte und verschiebbare Bereiche MÜSSEN eindeutig voneinander abgegrenzt werden.
- WINDOW-008: Nicht verfügbare Fensteroperationen MÜSSEN entsprechend dargestellt oder ausgeblendet werden.
- WINDOW-009: Titelleisten DÜRFEN geschützte Systemoberflächen nicht unberechtigt imitieren.
- WINDOW-010: Änderungen von Fenstertitel, Symbol oder Status MÜSSEN ohne Neuerstellung des Fensters möglich sein.
- WINDOW-011: Titelleisten MÜSSEN mit den geltenden Fokus- und Modalitätsregeln kompatibel sein.
- WINDOW-012: Fensterbedienelemente MÜSSEN unabhängig von optionalen visuellen Effekten bedienbar bleiben.
- WINDOW-013: Bei fehlerhaften benutzerdefinierten Titelleisten MUSS eine sichere Bedienmöglichkeit erhalten bleiben.
- WINDOW-014: Die Darstellung MUSS ohne KI-Abhängigkeit deterministisch funktionieren.

## 6. Fensterinteraktionen

Die Standardtitelleiste unterstützt folgende Aktionen:

| Interaktion | Aktion |
|---|---|
| Einfacher Klick | Fenster aktivieren |
| Ziehen der Drag Region | Fenster verschieben |
| Doppelklick auf Drag Region | Maximieren oder wiederherstellen |
| Rechtsklick auf Drag Region | Systemmenü öffnen |
| Klick auf Minimieren | Fenster minimieren |
| Klick auf Maximieren | Fenster maximieren oder wiederherstellen |
| Klick auf Schließen | Schließanforderung auslösen |

Die Aktionen MÜSSEN die individuellen Fenstereigenschaften berücksichtigen.

Benutzerdefinierte Eingabebelegungen DÜRFEN unterstützt werden.

## 7. Integration anwendungsspezifischer Inhalte

Anwendungen können zusätzliche Elemente in die Titelleiste integrieren:

- Registerkarten.
- Suchfelder.
- Navigationsschaltflächen.
- Statusanzeigen.
- Kontextabhängige Werkzeugleisten.
- Anwendungsbezogene Menüs.

Diese Elemente werden über Nova UI bereitgestellt.

Der Window Manager MUSS weiterhin eindeutig erkennen können, welche Bereiche für Fensteroperationen vorgesehen sind.

Anwendungsinhalte DÜRFEN die vorgeschriebenen Systemfunktionen nicht unzugänglich machen.

## 8. Visuelle Gestaltung

Titelleisten unterstützen:

- Helle und dunkle Themes.
- Anpassbare Farben und Transparenz.
- Optionale Acrylic- und Blur-Effekte.
- Unterschiedliche Höhen und Abstände.
- Zustandsabhängige Hervorhebungen.
- Dezente Darstellung inaktiver Fenster.
- Barrierefreie Kontrast- und Skalierungsoptionen.

Die Titelleiste SOLL möglichst wenig visuellen Raum beanspruchen, ohne die Bedienbarkeit einzuschränken.

## 9. Zustandsabhängiges Verhalten

### Aktives Fenster

- Deutlich erkennbare aktive Titelleiste.
- Vollständig verfügbare Fensterbedienelemente.
- Optionale visuelle Hervorhebung.

### Inaktives Fenster

- Reduzierte visuelle Betonung.
- Fensterbedienelemente bleiben entsprechend den geltenden Regeln verfügbar.

### Maximiertes Fenster

- Titelleiste wird an den verfügbaren Arbeitsbereich angepasst.
- Maximierungsschaltfläche wechselt zur Wiederherstellungsfunktion.

### Vollbildfenster

- Titelleiste wird standardmäßig ausgeblendet.
- Systemseitige Fenstersteuerung bleibt über definierte Eingabemethoden erreichbar.

### Modales Fenster

- Darstellung entsprechend dem Dialogtyp.
- Modalitäts- und Fokusregeln bleiben unverändert.

## 10. Fehlerbehandlung und Performance

- Ungültige Titelleistentemplates werden durch eine Standardimplementierung ersetzt.
- Nicht verfügbare Effekte werden automatisch deaktiviert.
- Änderungen von Titeln oder Statusinformationen aktualisieren nur betroffene Darstellungsbereiche.
- Titelleisten dürfen keine unnötigen vollständigen Fenster-Neuzeichnungen verursachen.
- Nicht reagierende Anwendungen dürfen systemseitige Fensterbedienelemente nicht blockieren.
- Ein Compositor-Neustart verändert die logischen Titelleisten- und Fensterzustände nicht.

## 11. Ergebnis

NovaOS erhält ein flexibles und einheitliches Titelleistensystem.

Anwendungen können klassische, kompakte oder vollständig integrierte Titelleisten verwenden, während grundlegende Fensteroperationen zuverlässig verfügbar bleiben.

Die Integration in Nova UI ermöglicht individuelle Designs, ohne die Zuständigkeiten des Window Managers aufzuheben.

**Grundsatz:** Die Titelleiste soll sich der Anwendung anpassen, ohne die vertraute Bedienung des Fensters zu verändern.