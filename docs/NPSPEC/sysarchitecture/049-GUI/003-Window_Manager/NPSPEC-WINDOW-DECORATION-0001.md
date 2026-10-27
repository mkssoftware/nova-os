# NPSPEC-WINDOW-DECORATION-0001 – Nova Window Decorations

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterdekorationen  
**System:** NovaOS

## 1. Zweck

Nova Window Decorations definiert die Darstellung und Verwaltung der äußeren Bedienelemente von Fenstern.

Ziel ist eine einheitliche, moderne und anpassbare Fensterdarstellung, die sich vertraut bedienen lässt und gleichzeitig die Designphilosophie von NovaOS unterstützt.

## 2. Architektur

- Zentrale Verwaltung der Fensterdekorationen durch den Nova Window Manager.
- Darstellung über das native Nova UI-System und den Compositor.
- Trennung zwischen Fensterinhalt, Fensterrahmen und Fensterzustand.
- Unterstützung systemseitiger und anwendungsseitiger Dekorationen.
- Einheitliche Eingabeverarbeitung für Fensteroperationen.
- Integration in Nova Window State Model und Window Management.
- Unterstützung von Themes, Styles und Templates.
- DPI-unabhängige Darstellung.
- Anpassbare Transparenz- und Unschärfeeffekte.

## 3. Dekorationsmodell

NovaOS unterscheidet folgende Dekorationsarten:

| Typ | Beschreibung |
|---|---|
| System Decoration | Vom System bereitgestellter Fensterrahmen |
| Client Decoration | Von der Anwendung gestalteter Fensterrahmen |
| Custom Decoration | Individuelle Dekoration auf Basis des Nova UI-Systems |
| Borderless | Fenster ohne sichtbaren Standardrahmen |

Systemdekorationen bilden den Standard.

Anwendungsseitige Dekorationen DÜRFEN die Sicherheits- und Bedienregeln des Window Managers nicht umgehen.

## 4. Dekorationselemente

Eine Standarddekoration kann folgende Elemente enthalten:

| Element | Funktion |
|---|---|
| Title Bar | Titelbereich des Fensters |
| Window Icon | Anwendungssymbol |
| Window Title | Fenstertitel |
| Minimize Button | Fenster minimieren |
| Maximize Button | Fenster maximieren oder wiederherstellen |
| Close Button | Fenster schließen |
| Window Border | Sichtbare Fensterbegrenzung |
| Resize Handles | Interaktive Größenänderung |
| System Menu | Fensterbezogene Systemfunktionen |

Die Verfügbarkeit einzelner Bedienelemente richtet sich nach den Eigenschaften und Berechtigungen des Fensters.

## 5. Normative Anforderungen

- WINDOW-001: NovaOS MUSS ein einheitliches System für Fensterdekorationen bereitstellen.
- WINDOW-002: Reguläre Fenster MÜSSEN standardmäßig eine Systemdekoration erhalten.
- WINDOW-003: Dekorationen MÜSSEN unabhängig vom Fensterinhalt verwaltet werden können.
- WINDOW-004: Fensterbedienelemente MÜSSEN die geltenden Fensterzustände und Berechtigungen berücksichtigen.
- WINDOW-005: Dekorationen MÜSSEN DPI-unabhängig skalierbar sein.
- WINDOW-006: Fensteroperationen MÜSSEN durch den Window Manager autorisiert und ausgeführt werden.
- WINDOW-007: Anwendungen DÜRFEN eigene Dekorationen verwenden, sofern die geltenden Systemrichtlinien dies erlauben.
- WINDOW-008: Client Decorations DÜRFEN keine privilegierten Systemoberflächen imitieren.
- WINDOW-009: Dekorationen MÜSSEN mit Maus, Touch und Tastatur bedienbar sein.
- WINDOW-010: Die Bedienbarkeit MUSS auch ohne Transparenz-, Blur- oder Animationseffekte gewährleistet sein.
- WINDOW-011: Dekorationen MÜSSEN unterschiedliche Fensterzustände korrekt darstellen.
- WINDOW-012: Geschützte Systemfenster MÜSSEN ihre vorgeschriebenen Dekorationseigenschaften beibehalten.
- WINDOW-013: Themes und Styles DÜRFEN die Funktionalität der Fensterbedienelemente nicht verändern.
- WINDOW-014: Fensterdekorationen MÜSSEN bei Änderungen von Theme, DPI und Fensterzustand aktualisiert werden.
- WINDOW-015: Die Darstellung DARF die Eingabeverarbeitung und Fensterverwaltung nicht blockieren.
- WINDOW-016: Fensterdekorationen MÜSSEN ohne KI-Abhängigkeit deterministisch funktionieren.

## 6. Gestaltung

NovaOS verwendet eine ruhige und moderne Gestaltung mit folgenden Eigenschaften:

- Klare Fensterkonturen.
- Dezente Hervorhebung aktiver Fenster.
- Zurückhaltende Darstellung inaktiver Fenster.
- Optionale Glas-, Acrylic- und Blur-Effekte.
- Konsistente Symbole und Bedienelemente.
- Anpassbare Farben, Abstände und Rahmenstärken.
- Unterstützung heller und dunkler Designs.
- Barrierearme Kontrast- und Bedienoptionen.

Selten benötigte Dekorationselemente DÜRFEN visuell zurückgenommen werden, solange ihre Erreichbarkeit und Erkennbarkeit gewährleistet bleiben.

## 7. Fensterzustände

### Normal

- Vollständige Standarddekoration.
- Verschiebbare Titelleiste.
- Größenänderung entsprechend den Fenstereigenschaften.

### Maximized

- Dekoration an die Bildschirmgeometrie angepasst.
- Wiederherstellungsschaltfläche statt Maximierungsschaltfläche.
- Keine unnötigen äußeren Rahmenabstände.

### Fullscreen

- Reguläre Dekoration standardmäßig ausgeblendet.
- Systemfunktionen bleiben über definierte Eingabemethoden erreichbar.

### Inactive

- Reduzierte visuelle Hervorhebung.
- Keine Veränderung der funktionalen Fensterrechte.

### Modal

- Dekoration entsprechend dem Dialogtyp.
- Fokus- und Modalitätsregeln bleiben verbindlich.

## 8. Eingabeverarbeitung

Dekorationen unterstützen folgende Standardinteraktionen:

- Titelleiste ziehen: Fenster verschieben.
- Titelleiste doppelklicken: Maximieren oder wiederherstellen.
- Fensterkante ziehen: Größe ändern.
- Minimieren: Fenster minimieren.
- Maximieren: Fenster maximieren oder wiederherstellen.
- Schließen: Reguläre Schließanforderung auslösen.
- Systemmenü öffnen: Verfügbare Fensteroperationen anzeigen.

Die Zuordnung von Eingabebereichen MUSS eindeutig sein.

Clientseitige Dekorationen MÜSSEN interaktive Fensterbereiche über definierte Schnittstellen registrieren können.

## 9. Integration in Nova UI

Fensterdekorationen werden über deklarative Styles und Templates beschrieben.

Das System unterstützt:

- Austauschbare Dekorationstemplates.
- Benutzerdefinierte Titelleisten.
- Anpassbare Bedienelemente.
- Zustandsabhängige Darstellung.
- Datenbindung an Fensterinformationen.
- Integration zusätzlicher Anwendungsfunktionen.

Der Window Manager bleibt unabhängig von der konkreten visuellen Implementierung.

## 10. Fehlerbehandlung und Performance

- Bei fehlerhaften Dekorationstemplates wird eine sichere Standarddekoration verwendet.
- Nicht verfügbare Effekte werden automatisch deaktiviert.
- Dekorationen dürfen keine ungültigen Fensterzustände erzeugen.
- Größen- und Positionsänderungen bleiben unabhängig vom Rendering funktionsfähig.
- Wiederverwendbare Dekorationsressourcen SOLLEN gemeinsam genutzt werden.
- Änderungen sollen ausschließlich betroffene Darstellungsbereiche aktualisieren.
- Ein Compositor-Neustart verändert die logischen Fensterzustände nicht.

## 11. Ergebnis

NovaOS erhält ein einheitliches, anpassbares und sicheres System für Fensterdekorationen.

Fenster können klassisch, modern oder individuell gestaltet werden, ohne ihre grundlegende Bedienbarkeit zu verlieren.

Durch die Integration in Nova UI und die Trennung zwischen Darstellung und Fensterverwaltung bleiben Dekorationen flexibel, effizient und zuverlässig.

**Grundsatz:** Fensterdekorationen unterstützen die Bedienung, ohne unnötig Aufmerksamkeit vom eigentlichen Fensterinhalt abzulenken.