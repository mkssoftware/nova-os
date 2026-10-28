# NPSPEC-SHELL-START-0001 – Nova Start Menu

**Status:** Angenommen  
**Bereich:** Shell / Start Menu  
**System:** NovaOS

## 1. Zweck

Nova Start Menu definiert das zentrale Startmenü der NovaOS Desktop Shell.

Es ermöglicht den schnellen Zugriff auf Anwendungen, Solutions, Capabilities, Dateien, Einstellungen und Systemfunktionen.

Das Startmenü MUSS sowohl eine klassische, vertraute Bedienung als auch die erweiterten Möglichkeiten des Capability-basierten NovaOS unterstützen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Start Menu Manager | Verwaltung und Lebenszyklus |
| Start Menu View | Darstellung und Benutzerinteraktion |
| Start Item Registry | Registrierung verfügbarer Einträge |
| Start Menu Layout Manager | Anordnung und Gruppierung |
| Start Menu Search Controller | Integration der Suche |
| Start Menu Action Controller | Ausführung ausgewählter Aktionen |
| Start Menu State Manager | Speicherung der Benutzereinstellungen |

Das Startmenü ist Bestandteil der Desktop Shell und verwendet das native Nova UI Framework.

## 3. Grundstruktur

Das Startmenü unterstützt folgende Bereiche:

- Angeheftete Anwendungen und Solutions.
- Alle verfügbaren Anwendungen.
- Capabilities und Systemfunktionen.
- Suchfeld.
- Zuletzt verwendete Elemente.
- Benutzerprofil.
- Energie- und Sitzungsfunktionen.
- Benutzerdefinierte Gruppen.

Dabei gilt:

- Die Bereiche MÜSSEN unabhängig voneinander aktualisierbar sein.
- Nicht benötigte Bereiche MÜSSEN ausblendbar sein.
- Die Anordnung SOLL individuell konfigurierbar sein.
- Das Startmenü MUSS auch ohne Suchfunktion vollständig bedienbar bleiben.

## 4. Startmenü-Einträge

Ein Startmenü-Eintrag besitzt mindestens:

- Eindeutige Identität.
- Anzeigename.
- Symbol.
- Elementtyp.
- Referenz auf die zugehörige Ressource oder Capability.
- Verfügbare Aktionen.

Unterstützte Elementtypen:

- Anwendungen.
- Solutions.
- Capabilities.
- Dateien und Ordner.
- Systemeinstellungen.
- Systemfunktionen.
- Benutzerdefinierte Verknüpfungen.

Einträge MÜSSEN unabhängig von der eigentlichen Implementierung ihrer Funktionen verwaltet werden.

## 5. Anwendungs- und Capability-Integration

Das Startmenü MUSS Anwendungen, Solutions und Capabilities über definierte Registrierungen erkennen.

Dabei gilt:

- Installierte Anwendungen MÜSSEN automatisch registrierbar sein.
- Solutions MÜSSEN als eigenständige Einträge erscheinen können.
- Capabilities MÜSSEN direkt oder über geeignete Funktionsgruppen erreichbar sein.
- Nicht verfügbare Funktionen SOLLEN ausgeblendet oder deaktiviert werden.
- Änderungen an Registrierungen MÜSSEN ereignisgesteuert übernommen werden.
- Das Startmenü DARF keine Capability-Implementierungen selbst enthalten.

## 6. Suche

Die Suche ermöglicht das Auffinden registrierter Anwendungen, Solutions, Capabilities und weiterer freigegebener Inhalte.

Dabei gilt:

- Die Suche MUSS über eine definierte Nova Search-Schnittstelle erfolgen.
- Ergebnisse MÜSSEN nach Typ und Relevanz unterscheidbar sein.
- Suchvorgänge DÜRFEN die UI Event Loop nicht blockieren.
- Suchergebnisse MÜSSEN die Berechtigungen des Benutzers berücksichtigen.
- Eine lokale Suche MUSS ohne Netzwerkverbindung funktionieren.
- KI-basierte Suchfunktionen DÜRFEN ausschließlich optionale Erweiterungen sein.

## 7. Angeheftete und zuletzt verwendete Elemente

Der Benutzer MUSS häufig verwendete Elemente anheften können.

Dabei gilt:

- Angeheftete Elemente MÜSSEN frei angeordnet werden können.
- Benutzerdefinierte Gruppen MÜSSEN unterstützt werden.
- Anheftungen MÜSSEN dauerhaft gespeichert werden.
- Zuletzt verwendete Elemente MÜSSEN deaktivierbar sein.
- Der Benutzer MUSS den Verlauf einsehen und löschen können.
- Automatische Vorschläge DÜRFEN angeheftete Elemente nicht eigenmächtig verändern.

## 8. Benutzerinteraktion

Das Startmenü unterstützt:

- Öffnen über die Start-Schaltfläche.
- Öffnen über die Tastatur.
- Maus-, Touch- und alternative Eingabemethoden.
- Tastaturnavigation.
- Kontextmenüs.
- Drag & Drop.
- Direkte Ausführung von Aktionen.

Dabei gilt:

- Aktionen MÜSSEN über das Nova Command- und Capability-System ausgeführt werden können.
- Anwendungsstarts MÜSSEN über das zuständige Anwendungsmanagement erfolgen.
- Drag & Drop MUSS Nova Data Transfer verwenden.
- Das Startmenü MUSS ohne Maus vollständig bedienbar sein.
- Das Öffnen und Schließen DARF laufende Anwendungen nicht beeinflussen.

## 9. Darstellung und Anpassung

Das Startmenü verwendet das native Nova UI Framework.

Unterstützt werden:

- Unterschiedliche Menügrößen.
- Benutzerdefinierte Anordnung.
- Listen- und Rasterdarstellung.
- Themes und Styles.
- Transparenz- und Acrylic-Effekte.
- Adaptive Layouts.

Dabei gilt:

- Das Startmenü MUSS unterschiedliche Bildschirmauflösungen und DPI-Skalierungen berücksichtigen.
- Die Darstellung MUSS auf kleinen und großen Bildschirmen funktionieren.
- Animationen SOLLEN dezent und optional sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Visuelle Effekte DÜRFEN die Bedienbarkeit nicht beeinträchtigen.

## 10. System- und Sitzungsfunktionen

Das Startmenü stellt zentrale Systemfunktionen bereit.

Dazu gehören:

- Einstellungen.
- Sperren.
- Abmelden.
- Neustarten.
- Herunterfahren.
- Energiesparfunktionen, sofern verfügbar.

Dabei gilt:

- Systemaktionen MÜSSEN an die zuständigen Systemdienste delegiert werden.
- Nicht unterstützte Funktionen DÜRFEN nicht als ausführbar dargestellt werden.
- Kritische Aktionen MÜSSEN erforderliche Bestätigungen und Berechtigungsprüfungen berücksichtigen.
- Das Startmenü DARF Energie- oder Sitzungsverwaltung nicht selbst implementieren.

## 11. Performance und Lebenszyklus

Das Startmenü MUSS schnell und ressourcenschonend arbeiten.

Dabei gilt:

- Die grundlegende Startmenüfunktion MUSS während des frühen Desktopstarts verfügbar sein.
- Optionale Inhalte SOLLEN nach Bedarf geladen werden.
- Häufig verwendete Symbole und Metadaten SOLLEN zwischengespeichert werden.
- Lange Listen SOLLEN virtualisiert werden.
- Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Unveränderte Bereiche DÜRFEN nicht permanent neu gerendert werden.
- Damage Tracking MUSS unterstützt werden.
- Fehlerhafte Erweiterungen DÜRFEN das Startmenü nicht blockieren.

## 12. Zustand und Wiederherstellung

Der Start Menu State Manager speichert benutzerbezogen:

- Angeheftete Elemente.
- Gruppen und Reihenfolge.
- Layout und Darstellungsoptionen.
- Sichtbarkeit einzelner Bereiche.
- Such- und Verlaufseinstellungen.

Dabei gilt:

- Einstellungen MÜSSEN nach einem Shell-Neustart wiederherstellbar sein.
- Beschädigte Konfigurationen DÜRFEN den Start nicht verhindern.
- Eine funktionsfähige Standardkonfiguration MUSS verfügbar sein.
- Benutzerdefinierte Anpassungen SOLLEN exportierbar und wiederherstellbar sein.

## 13. Sicherheit und Barrierefreiheit

Nova Start Menu unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Startmenü-Einträge DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Aktionen MÜSSEN vor ihrer Ausführung autorisiert werden.
- Erweiterungen MÜSSEN über das Nova Shell Extension Model eingebunden werden.
- Nicht vertrauenswürdige Erweiterungen MÜSSEN isoliert werden können.
- Suchergebnisse DÜRFEN keine unautorisierten Informationen offenlegen.
- Alle wesentlichen Bedienelemente MÜSSEN für Screenreader zugänglich sein.
- Das Startmenü MUSS vollständig offline und ohne KI funktionieren.

## 14. Normative Anforderungen

- **START-001:** NovaOS MUSS ein natives Startmenü bereitstellen.
- **START-002:** Anwendungen, Solutions und Capabilities MÜSSEN integriert werden können.
- **START-003:** Startmenü-Einträge MÜSSEN eindeutige Identitäten besitzen.
- **START-004:** Einträge MÜSSEN dynamisch registriert und entfernt werden können.
- **START-005:** Angeheftete Elemente und benutzerdefinierte Gruppen MÜSSEN unterstützt werden.
- **START-006:** Eine lokale Suchfunktion MUSS integrierbar sein.
- **START-007:** Das Startmenü MUSS auch ohne Suche bedienbar sein.
- **START-008:** Aktionen MÜSSEN an die zuständigen Systemkomponenten delegiert werden.
- **START-009:** System- und Sitzungsfunktionen MÜSSEN erreichbar sein.
- **START-010:** Die Darstellung MUSS das Nova UI Framework verwenden.
- **START-011:** Das Startmenü MUSS vollständig per Tastatur bedienbar sein.
- **START-012:** Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- **START-013:** Damage Tracking MUSS unterstützt werden.
- **START-014:** Benutzereinstellungen MÜSSEN gespeichert und wiederhergestellt werden können.
- **START-015:** Fehlerhafte Erweiterungen DÜRFEN die Desktop Shell nicht beeinträchtigen.
- **START-016:** Nova.Trust und Accessibility MÜSSEN berücksichtigt werden.
- **START-017:** Nova Start Menu MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Desktop Shell Architecture:** Übergeordnete Shell-Verwaltung.
- **Nova Shell Lifecycle:** Initialisierung und Wiederherstellung.
- **Nova Taskbar Architecture:** Start-Schaltfläche und Menüaktivierung.
- **Nova Application Management:** Anwendungsregistrierung und Startvorgänge.
- **Nova Capability System:** Funktionsbereitstellung.
- **Nova Solutions:** Integration benutzerdefinierter Arbeitsumgebungen.
- **Nova Search:** Suche nach Anwendungen, Funktionen und Inhalten.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova Shell Extension Model:** Erweiterbare Startmenü-Einträge.
- **Nova Data Transfer:** Drag & Drop.
- **Nova System Services:** Energie- und Sitzungsfunktionen.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 16. Ergebnis

NovaOS erhält ein einheitliches, erweiterbares und individuell anpassbares Startmenü.

Es verbindet den klassischen Anwendungsstart mit dem Capability-basierten Bedienkonzept von NovaOS und ermöglicht einen schnellen Zugriff auf Programme, Funktionen und persönliche Arbeitsumgebungen.

Durch bedarfsgesteuertes Laden, ereignisgesteuerte Aktualisierung und eine native Darstellung bleibt das Startmenü schnell und ressourcenschonend.

**Grundsatz:** Das Startmenü ist der vertraute Einstiegspunkt in NovaOS. Es macht Funktionen leicht auffindbar, ohne dem Benutzer eine bestimmte Arbeitsweise aufzuzwingen.