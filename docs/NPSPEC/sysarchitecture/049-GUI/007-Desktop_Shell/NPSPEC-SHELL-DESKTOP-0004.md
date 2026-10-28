# NPSPEC-SHELL-DESKTOP-0004 – Nova Desktop Context Menus

**Status:** Angenommen  
**Bereich:** Shell / Desktop / Context Menus  
**System:** NovaOS

## 1. Zweck

Nova Desktop Context Menus definiert die Bereitstellung und Verwaltung kontextabhängiger Menüs innerhalb der NovaOS-Desktopumgebung.

Kontextmenüs stellen die für ein ausgewähltes Objekt oder einen Arbeitsbereich verfügbaren Aktionen bereit.

Ziel ist eine einheitliche, schnelle und erweiterbare Bedienung, bei der Funktionen über das NovaOS-Capability-System bereitgestellt werden.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Context Menu Manager | Verwaltung und Darstellung von Kontextmenüs |
| Context Resolver | Ermittlung des aktuellen Kontextes |
| Action Provider | Bereitstellung verfügbarer Aktionen |
| Menu Composer | Zusammenstellung und Gruppierung der Menüeinträge |
| Menu Extension Registry | Registrierung zusätzlicher Menüfunktionen |
| Menu State Manager | Verwaltung von Auswahl und Menüzuständen |

Die Darstellung erfolgt über das native Nova UI Framework.

## 3. Kontextmodell

Der Context Resolver bestimmt den Kontext anhand von:

- Ausgewähltem Objekt oder Objektgruppe.
- Objekttyp und semantischem Datentyp.
- Aktuellem Arbeitsbereich.
- Verfügbaren Capabilities.
- Benutzerberechtigungen.
- Aktuellem System- und Objektzustand.

Dabei gilt:

- Kontextinformationen MÜSSEN eindeutig und strukturiert bereitgestellt werden.
- Mehrfachauswahl MUSS unterstützt werden.
- Kontextmenüs DÜRFEN nicht ausschließlich anhand von Dateiendungen zusammengestellt werden.
- Änderungen des Kontextes MÜSSEN berücksichtigt werden.

## 4. Menütypen

NovaOS unterscheidet insbesondere:

| Menütyp | Verwendung |
|---|---|
| Desktop Context Menu | Aktionen für den freien Desktopbereich |
| Object Context Menu | Aktionen für Dateien, Ordner und andere Objekte |
| Selection Context Menu | Aktionen für mehrere ausgewählte Objekte |
| Capability Context Menu | Kontextabhängige Capability-Funktionen |
| Extension Context Menu | Funktionen registrierter Shell-Erweiterungen |

Alle Menütypen verwenden dasselbe grundlegende Menüsystem.

## 5. Menüstruktur

Kontextmenüs unterstützen:

- Menüeinträge.
- Untermenüs.
- Trennlinien.
- Symbole.
- Tastenkombinationen.
- Aktivierte und deaktivierte Einträge.
- Umschaltbare Optionen.
- Kontextabhängige Sichtbarkeit.

Dabei gilt:

- Menüeinträge MÜSSEN eindeutige Aktionsreferenzen besitzen.
- Die Menüstruktur MUSS deklarativ beschreibbar sein.
- Funktionen SOLLEN logisch gruppiert werden.
- Häufig verwendete Aktionen SOLLEN leicht erreichbar sein.
- Erweiterungen DÜRFEN die grundlegende Bedienbarkeit nicht beeinträchtigen.

## 6. Capability-Integration

Kontextmenüs verwenden das NovaOS-Capability-System zur Ermittlung und Ausführung von Aktionen.

Dabei gilt:

- Capabilities MÜSSEN ihre unterstützten Objekttypen und Aktionen deklarieren können.
- Der Menu Composer MUSS passende Aktionen anhand des aktuellen Kontextes auswählen.
- Nicht verfügbare Aktionen SOLLEN ausgeblendet oder deaktiviert werden.
- Berechtigungen MÜSSEN bei der Ausführung erneut geprüft werden.
- Kontextmenüs DÜRFEN Capability-Implementierungen nicht selbst enthalten müssen.

Die Darstellung einer Aktion im Kontextmenü verleiht keine Berechtigung zu ihrer Ausführung.

## 7. Erweiterbarkeit

Anwendungen, Solutions und Capabilities können zusätzliche Menüaktionen registrieren.

Dabei gilt:

- Erweiterungen MÜSSEN das Nova Shell Extension Model verwenden.
- Menübeiträge MÜSSEN deklarativ registrierbar sein.
- Erweiterungen DÜRFEN keine internen Menüstrukturen direkt verändern.
- Konflikte zwischen Menübeiträgen MÜSSEN deterministisch aufgelöst werden.
- Fehlerhafte Erweiterungen MÜSSEN isoliert werden können.
- Der Benutzer SOLL zusätzliche Menüeinträge deaktivieren können.

## 8. Benutzerinteraktion

Kontextmenüs können über folgende Eingaben geöffnet werden:

- Rechte Maustaste.
- Kontextmenütaste.
- Definierte Tastenkombination.
- Touch-Geste.
- Alternative Eingabemethoden.

Dabei gilt:

- Menüs MÜSSEN vollständig über die Tastatur bedienbar sein.
- Die Menüposition MUSS innerhalb des sichtbaren Arbeitsbereichs bleiben.
- Untermenüs MÜSSEN abhängig vom verfügbaren Bildschirmplatz ausgerichtet werden.
- Fokus und Auswahl MÜSSEN eindeutig erkennbar sein.
- Das Schließen eines Menüs DARF keine Aktion unbeabsichtigt ausführen.

## 9. Dynamische Menüinhalte

Menüinhalte können sich abhängig vom aktuellen Kontext verändern.

Dabei gilt:

- Kontextabhängige Aktionen MÜSSEN bei Bedarf ermittelt werden.
- Langsame Action Provider DÜRFEN die UI Event Loop nicht blockieren.
- Asynchrone Ergebnisse MÜSSEN dem ursprünglichen Kontext zugeordnet bleiben.
- Veraltete Ergebnisse DÜRFEN nicht auf einen inzwischen geänderten Kontext angewendet werden.
- Häufig verwendete Menüdefinitionen SOLLEN zwischengespeichert werden.

## 10. Darstellung und Performance

Die Kontextmenüs verwenden das Nova UI Styling System.

Dabei gilt:

- Menüs MÜSSEN das aktive Theme berücksichtigen.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Animationen SOLLEN dezent und optional sein.
- Änderungen MÜSSEN über Damage Tracking verarbeitet werden.
- Unveränderte Menübereiche DÜRFEN nicht permanent neu gerendert werden.
- Die Menüöffnung MUSS auch bei vielen registrierten Erweiterungen reaktionsfähig bleiben.

## 11. Sicherheit

Nova Desktop Context Menus unterliegt Nova.Trust.

Dabei gilt:

- Aktionen MÜSSEN innerhalb ihrer autorisierten Capabilities ausgeführt werden.
- Erweiterungen DÜRFEN keine zusätzlichen Berechtigungen durch Menüregistrierung erhalten.
- Sicherheitskritische Aktionen MÜSSEN geeignete Bestätigungen oder Authentifizierungen verwenden.
- Menübeiträge aus nicht vertrauenswürdigen Quellen MÜSSEN kontrolliert behandelt werden.
- Fehlerhafte Action Provider DÜRFEN die Desktop Shell nicht zum Absturz bringen.

## 12. Normative Anforderungen

- **DESKTOP-001:** NovaOS MUSS ein einheitliches Desktop Context Menu System bereitstellen.
- **DESKTOP-002:** Kontextmenüs MÜSSEN anhand strukturierter Kontextinformationen zusammengestellt werden.
- **DESKTOP-003:** Einzel- und Mehrfachauswahl MÜSSEN unterstützt werden.
- **DESKTOP-004:** Menüeinträge MÜSSEN deklarativ definierbar sein.
- **DESKTOP-005:** Aktionen MÜSSEN über das Capability- und Command-System ausführbar sein.
- **DESKTOP-006:** Erweiterungen MÜSSEN über definierte Schnittstellen integrierbar sein.
- **DESKTOP-007:** Konflikte zwischen Menübeiträgen MÜSSEN deterministisch behandelt werden.
- **DESKTOP-008:** Kontextmenüs MÜSSEN vollständig per Tastatur bedienbar sein.
- **DESKTOP-009:** Langsame Action Provider DÜRFEN die UI Event Loop nicht blockieren.
- **DESKTOP-010:** Die Darstellung MUSS das native Nova UI Framework verwenden.
- **DESKTOP-011:** Kontextmenüs MÜSSEN Damage Tracking unterstützen.
- **DESKTOP-012:** Berechtigungen MÜSSEN vor der Aktionsausführung geprüft werden.
- **DESKTOP-013:** Fehlerhafte Erweiterungen DÜRFEN die Shell nicht beeinträchtigen.
- **DESKTOP-014:** Nova Desktop Context Menus MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Desktop Environment:** Desktopkontext und Arbeitsbereich.
- **Nova Desktop Icons:** Objektbezogene Kontextmenüs.
- **Nova Shell Extension Model:** Erweiterbare Menüaktionen.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova UI Event System:** Eingabeverarbeitung.
- **Nova Commands and Actions:** Aktionsausführung.
- **Nova Capability System:** Kontextabhängige Funktionen.
- **Nova Data Transfer:** Objekt- und Auswahlkontexte.
- **Nova VFS:** Datei- und Ordneraktionen.
- **Nova.Trust:** Berechtigungen und Sicherheit.

## 14. Ergebnis

NovaOS erhält ein einheitliches, erweiterbares und Capability-basiertes Kontextmenüsystem.

Verfügbare Aktionen werden anhand des aktuellen Kontextes ermittelt und über definierte Schnittstellen bereitgestellt.

Die Desktop Shell bleibt für Darstellung und Benutzerinteraktion verantwortlich, während die eigentlichen Funktionen durch Capabilities ausgeführt werden.

**Grundsatz:** Ein Kontextmenü zeigt dem Benutzer die Funktionen, die an dieser Stelle sinnvoll und verfügbar sind – nicht sämtliche Funktionen, die das System besitzt.