# NPSPEC-SHELL-DESKTOP-0001 – Nova Desktop Environment

**Status:** Angenommen  
**Bereich:** Shell / Desktop  
**System:** NovaOS

## 1. Zweck

Nova Desktop Environment definiert den grafischen Arbeitsbereich der NovaOS Desktop Shell.

Der Desktop stellt die zentrale Benutzeroberfläche für den Zugriff auf Dateien, Anwendungen, Solutions, Capabilities und Systemfunktionen bereit.

Ziel ist eine vertraute, moderne und ressourcenschonende Arbeitsumgebung, die sich individuell anpassen lässt, ohne den Benutzer unnötig abzulenken.

## 2. Architektur

Die Desktopumgebung besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Desktop Manager | Verwaltung des Arbeitsbereichs |
| Desktop Surface | Darstellung des Desktops |
| Desktop Item Manager | Verwaltung von Desktop-Elementen |
| Desktop Layout Manager | Positionierung und Anordnung |
| Desktop Background Manager | Hintergrundbilder und Darstellung |
| Desktop Interaction Manager | Auswahl, Drag & Drop und Kontextaktionen |
| Desktop State Manager | Speicherung und Wiederherstellung |

Der Desktop MUSS das native Nova UI Framework verwenden.

Fensterverwaltung und Zusammensetzung der Oberflächen bleiben Aufgaben des Window Managers beziehungsweise Compositors.

## 3. Desktop-Oberfläche

Der Desktop bildet den Hintergrund des grafischen Arbeitsbereichs.

Unterstützt werden:

- Statische Hintergrundbilder.
- Einfarbige und verlaufende Hintergründe.
- Optionale animierte Hintergründe.
- Desktop-Elemente und Verknüpfungen.
- Kontextmenüs.
- Mehrere Monitore.
- Benutzerdefinierte Layouts.

Dabei gilt:

- Die Desktop-Oberfläche MUSS unabhängig von Anwendungsfenstern verwaltet werden.
- Hintergrundbilder MÜSSEN unterschiedliche Auflösungen und Skalierungen unterstützen.
- Animationen DÜRFEN nur bei Bedarf ausgeführt werden.
- Der Desktop MUSS ohne aktive Animationen vollständig funktionsfähig sein.

## 4. Desktop-Elemente

Desktop-Elemente repräsentieren Dateien, Ordner, Anwendungen, Solutions, Capabilities oder Verknüpfungen.

Dabei gilt:

- Elemente MÜSSEN eine eindeutige Identität besitzen.
- Darstellung und zugrunde liegende Ressource MÜSSEN getrennt bleiben.
- Elemente MÜSSEN über ihre registrierten Aktionen geöffnet werden können.
- Symbole, Beschriftungen und Zustände MÜSSEN dynamisch aktualisierbar sein.
- Das Entfernen einer Verknüpfung DARF nicht automatisch die referenzierte Ressource löschen.

## 5. Desktop-Layout

Der Desktop Layout Manager verwaltet die Anordnung der Elemente.

Unterstützt werden:

- Freie Positionierung.
- Rasterausrichtung.
- Automatische Anordnung.
- Sortierung nach Eigenschaften.
- Benutzerdefinierte Gruppierung.
- Monitorabhängige Layouts.

Dabei gilt:

- Positionen MÜSSEN benutzerbezogen gespeichert werden.
- Änderungen der Bildschirmauflösung DÜRFEN Elemente nicht dauerhaft unerreichbar machen.
- Layouts MÜSSEN wiederherstellbar sein.
- Automatische Anordnungen DÜRFEN manuelle Layouts nicht ohne Benutzerentscheidung überschreiben.

## 6. Benutzerinteraktion

Die Desktopumgebung unterstützt:

- Einzel- und Mehrfachauswahl.
- Doppelklick und Tastaturaktivierung.
- Kontextmenüs.
- Drag & Drop.
- Umbenennen.
- Kopieren, Verschieben und Löschen.
- Tastaturnavigation.

Dabei gilt:

- Eingaben MÜSSEN über das Nova Input System verarbeitet werden.
- Dateioperationen MÜSSEN über die zuständigen Storage- und VFS-Schnittstellen erfolgen.
- Aktionen MÜSSEN die geltenden Capabilities und Berechtigungen berücksichtigen.
- Drag & Drop MUSS das Nova Data Transfer System verwenden.

## 7. Kontextabhängige Funktionen

Der Desktop integriert das NovaOS-Capability-System.

Dabei gilt:

- Verfügbare Aktionen MÜSSEN anhand des ausgewählten Objekts bestimmt werden können.
- Kontextmenüs DÜRFEN durch autorisierte Shell-Erweiterungen ergänzt werden.
- Nicht verfügbare Funktionen SOLLEN ausgeblendet oder deaktiviert werden.
- Der Desktop DARF Capabilities nicht selbst implementieren müssen.
- Kontextabhängige Vorschläge SOLLEN dezent und abschaltbar sein.

## 8. Mehrere Monitore

Nova Desktop Environment MUSS Mehrmonitorbetrieb unterstützen.

Dabei gilt:

- Jeder Monitor MUSS einen eigenen Desktopbereich besitzen können.
- Hintergrundbilder und Layouts MÜSSEN individuell konfigurierbar sein.
- Änderungen der Monitorkonfiguration MÜSSEN erkannt werden.
- Elemente eines entfernten Monitors MÜSSEN erreichbar bleiben.
- Nach Wiederanschluss SOLL die vorherige Anordnung wiederhergestellt werden.

## 9. Anpassbarkeit

Der Benutzer kann die Desktopumgebung individuell gestalten.

Dazu gehören:

- Hintergrund und Farben.
- Symbolgröße und Abstände.
- Anordnung und Gruppierung.
- Sichtbarkeit von Desktop-Elementen.
- Desktop-Layouts.
- Kontextabhängige Darstellungsregeln.

Dabei gilt:

- Einstellungen MÜSSEN dauerhaft gespeichert werden können.
- Layouts SOLLEN exportierbar und übertragbar sein.
- Änderungen SOLLEN rückgängig gemacht werden können.
- Automatische Anpassungen MÜSSEN durch den Benutzer kontrollierbar bleiben.

## 10. Rendering und Performance

Der Desktop MUSS bedarfsgesteuert dargestellt werden.

Dabei gilt:

- Änderungen MÜSSEN über Damage Tracking erfasst werden.
- Nur betroffene Bereiche DÜRFEN zur Neudarstellung invalidiert werden.
- Unveränderte Desktopbereiche DÜRFEN keine permanente Neuberechnung verursachen.
- Symbole und Hintergrundressourcen SOLLEN zwischengespeichert werden.
- Animationen MÜSSEN bei verdecktem oder inaktivem Desktop reduziert beziehungsweise ausgesetzt werden.
- Hintergrundaktivitäten DÜRFEN die Reaktionsfähigkeit des Desktops nicht dauerhaft beeinträchtigen.

## 11. Zustand und Wiederherstellung

Der Desktop State Manager verwaltet den wiederherstellbaren Benutzerzustand.

Dabei gilt:

- Layoutänderungen SOLLEN inkrementell gespeichert werden.
- Zustandsdaten MÜSSEN konsistent und validierbar sein.
- Ein Shell-Neustart DARF die gespeicherte Desktopanordnung nicht verlieren.
- Beschädigte Konfigurationen MÜSSEN durch gültige Standardwerte ersetzt werden können.
- Die Wiederherstellung MUSS mit Nova Shell Recovery zusammenarbeiten.

## 12. Sicherheit und Barrierefreiheit

Die Desktopumgebung unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Desktop-Aktionen MÜSSEN die Berechtigungen des Benutzers beachten.
- Erweiterungen DÜRFEN keine unautorisierten Systemoperationen ausführen.
- Desktop-Elemente MÜSSEN für Screenreader zugänglich sein.
- Die Bedienung MUSS vollständig über die Tastatur möglich sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Der Desktop MUSS vollständig offline funktionieren.

## 13. Normative Anforderungen

- **DESKTOP-001:** NovaOS MUSS einen nativen grafischen Desktop bereitstellen.
- **DESKTOP-002:** Der Desktop MUSS das Nova UI Framework verwenden.
- **DESKTOP-003:** Desktop-Elemente MÜSSEN unabhängig von ihren zugrunde liegenden Ressourcen verwaltet werden.
- **DESKTOP-004:** Freie und automatische Elementanordnung MÜSSEN unterstützt werden.
- **DESKTOP-005:** Desktop-Layouts MÜSSEN gespeichert und wiederhergestellt werden können.
- **DESKTOP-006:** Mehrmonitorbetrieb MUSS unterstützt werden.
- **DESKTOP-007:** Kontextabhängige Funktionen MÜSSEN über das Capability-System integrierbar sein.
- **DESKTOP-008:** Drag & Drop MUSS über das Nova Data Transfer System erfolgen.
- **DESKTOP-009:** Der Desktop MUSS Damage Tracking und bedarfsgesteuertes Rendering verwenden.
- **DESKTOP-010:** Ein Shell-Neustart DARF den gespeicherten Desktopzustand nicht verlieren.
- **DESKTOP-011:** Der Benutzer MUSS die Desktopdarstellung individuell anpassen können.
- **DESKTOP-012:** Desktop-Aktionen MÜSSEN Nova.Trust berücksichtigen.
- **DESKTOP-013:** Die grundlegende Desktopbedienung MUSS barrierefrei sein.
- **DESKTOP-014:** Nova Desktop Environment MUSS ohne Cloud- oder KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Desktop Shell Architecture:** Übergeordnete Desktopumgebung.
- **Nova Shell Lifecycle:** Initialisierung und Beendigung.
- **Nova Shell Recovery:** Wiederherstellung nach Fehlern.
- **Nova Shell Extension Model:** Erweiterbare Desktopfunktionen.
- **Nova UI Framework:** Native Darstellung und Interaktion.
- **Nova Window Manager:** Verwaltung der Anwendungsfenster.
- **Nova Compositor:** Zusammensetzung der Oberflächen.
- **Nova Display Server:** Bildschirmkonfiguration.
- **Nova Input System:** Maus-, Tastatur- und alternative Eingaben.
- **Nova VFS:** Datei- und Ordneroperationen.
- **Nova Data Transfer:** Drag & Drop und Datenaustausch.
- **Nova Capability System:** Kontextabhängige Funktionen.
- **Nova.Trust:** Berechtigungen und Sicherheit.

## 15. Ergebnis

NovaOS erhält eine native, modulare und individuell anpassbare Desktopumgebung.

Der Desktop unterstützt klassische Bedienkonzepte und integriert gleichzeitig das Capability-basierte Systemmodell von NovaOS.

Durch bedarfsgesteuertes Rendering, gespeicherte Layouts und unabhängige Shell-Komponenten bleibt die Arbeitsumgebung schnell, zuverlässig und wiederherstellbar.

**Grundsatz:** Der Desktop ist der persönliche Arbeitsbereich des Benutzers. Er passt sich dessen Arbeitsweise an, ohne ihm eine bestimmte Bedienung aufzuzwingen.