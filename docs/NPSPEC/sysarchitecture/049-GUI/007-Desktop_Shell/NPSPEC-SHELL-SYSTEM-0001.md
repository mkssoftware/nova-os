# NPSPEC-SHELL-SYSTEM-0001 – Nova Quick Settings

**Status:** Angenommen  
**Bereich:** Shell / System / Quick Settings  
**System:** NovaOS

## 1. Zweck

Nova Quick Settings definiert den zentralen Schnellzugriff auf häufig verwendete Systemeinstellungen und Funktionen innerhalb der NovaOS Desktop Shell.

Ziel ist es, wichtige Einstellungen unmittelbar ändern zu können, ohne die vollständige Systemsteuerung öffnen zu müssen.

Quick Settings MUSS eine vertraute, übersichtliche und individuell anpassbare Bedienoberfläche bereitstellen.

**Grundprinzip:** Häufig benötigte Funktionen sind sofort erreichbar, selten verwendete Optionen bleiben im Hintergrund.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Quick Settings Manager | Verwaltung und Koordination |
| Quick Settings Registry | Registrierung verfügbarer Funktionen |
| Quick Settings View | Darstellung und Benutzerinteraktion |
| Quick Settings Layout Manager | Anordnung und Gruppierung |
| Quick Settings Action Controller | Verarbeitung von Benutzeraktionen |
| Quick Settings State Manager | Speicherung der Benutzereinstellungen |

Quick Settings ist Bestandteil der Desktop Shell und verwendet das native Nova UI Framework.

Die eigentliche Verwaltung von Systemeinstellungen MUSS durch die zuständigen Systemdienste erfolgen.

## 3. Unterstützte Funktionen

Quick Settings unterstützt unter anderem:

- Netzwerk und WLAN.
- Bluetooth.
- Lautstärke und Audiogeräte.
- Bildschirmhelligkeit.
- Energiesparmodus.
- Do Not Disturb.
- Anzeige- und Projektionsmodi.
- Nachtmodus und Farbtemperatur.
- Barrierefreiheit.
- Weitere registrierte Systemfunktionen.

Dabei gilt:

- Funktionen MÜSSEN dynamisch registrierbar sein.
- Nicht unterstützte Funktionen MÜSSEN ausgeblendet oder deaktiviert werden.
- Verfügbare Funktionen MÜSSEN anhand der tatsächlichen Systemfähigkeiten ermittelt werden.
- Neue Capabilities MÜSSEN über definierte Schnittstellen integrierbar sein.
- Quick Settings DARF keine Hardwaresteuerung selbst implementieren.

## 4. Quick-Setting-Modell

Jede Schnellfunktion besitzt mindestens:

- Eindeutige ID.
- Anzeigename.
- Symbol.
- Funktionsreferenz.
- Aktuellen Zustand.
- Unterstützte Aktionen.
- Verfügbarkeitsstatus.

Optional können enthalten sein:

- Beschreibung.
- Sekundärer Status.
- Wertebereich.
- Untermenü.
- Erweiterte Einstellungen.
- Benutzerdefinierte Darstellung.

Dabei gilt:

- Zustände MÜSSEN von den zuständigen Systemdiensten bereitgestellt werden.
- Änderungen MÜSSEN ereignisgesteuert übertragen werden.
- Die Darstellung MUSS unabhängig von der Implementierung der Funktion bleiben.

## 5. Bedienelemente

Quick Settings unterstützt unterschiedliche Interaktionstypen.

| Element | Verwendung |
|---|---|
| Toggle | Ein- und Ausschalten |
| Slider | Stufenlose Werteinstellung |
| Selector | Auswahl zwischen Optionen |
| Action Button | Ausführen einer Aktion |
| Expandable Control | Erweiterte Schnelloptionen |
| Status Indicator | Anzeige eines Systemzustands |

Dabei gilt:

- Bedienelemente MÜSSEN über das Nova UI Framework bereitgestellt werden.
- Zustandsänderungen MÜSSEN über autorisierte Commands oder Capabilities erfolgen.
- Die Oberfläche MUSS laufende und fehlgeschlagene Änderungen darstellen können.
- Ein angezeigter Zustand DARF erst als bestätigt gelten, wenn der zuständige Systemdienst die Änderung bestätigt hat.

## 6. Benutzerinteraktion

Quick Settings MUSS über verschiedene Einstiegspunkte erreichbar sein.

Dazu gehören:

- System Tray.
- Taskleiste.
- Tastenkombinationen.
- Autorisierte Shell-Aktionen.

Dabei gilt:

- Das Panel MUSS schnell geöffnet und geschlossen werden können.
- Einstellungen SOLLEN mit möglichst wenigen Interaktionen erreichbar sein.
- Häufig verwendete Funktionen SOLLEN unmittelbar sichtbar sein.
- Erweiterte Optionen DÜRFEN in Unteransichten dargestellt werden.
- Ein Wechsel zu den vollständigen Systemeinstellungen MUSS möglich sein.
- Das Panel DARF den Tastaturfokus nicht ungefragt von sicherheitskritischen Dialogen übernehmen.

## 7. Layout und Anpassung

Der Benutzer MUSS Quick Settings individuell anpassen können.

Unterstützt werden:

- Hinzufügen und Entfernen von Schnellfunktionen.
- Freie Anordnung.
- Benutzerdefinierte Gruppierung.
- Unterschiedliche Elementgrößen.
- Ein- und Ausblenden einzelner Funktionen.

Dabei gilt:

- Benutzerdefinierte Anordnungen MÜSSEN gespeichert werden.
- NovaOS DARF die Anordnung nicht eigenmächtig dauerhaft verändern.
- Systemseitige Empfehlungen MÜSSEN optional und abschaltbar sein.
- Das Layout MUSS unterschiedliche Bildschirmgrößen und DPI-Skalierungen berücksichtigen.
- Eine funktionsfähige Standardanordnung MUSS verfügbar sein.

## 8. Zustandsverwaltung

Quick Settings MUSS den tatsächlichen Systemzustand widerspiegeln.

Dabei gilt:

- Statusinformationen MÜSSEN über definierte Systemdienst-Schnittstellen bezogen werden.
- Änderungen außerhalb von Quick Settings MÜSSEN automatisch übernommen werden.
- Laufende Änderungen MÜSSEN erkennbar sein.
- Fehlgeschlagene Änderungen MÜSSEN verständlich gemeldet werden.
- Nicht erreichbare Systemdienste DÜRFEN das Panel nicht blockieren.
- Ungültige oder veraltete Zustände MÜSSEN entsprechend gekennzeichnet werden.

## 9. Capability-Integration

Quick Settings MUSS das Capability-basierte Architekturmodell von NovaOS unterstützen.

Dabei gilt:

- Capabilities MÜSSEN Schnellfunktionen registrieren können.
- Registrierte Funktionen MÜSSEN ihre unterstützten Aktionen und Zustände deklarieren.
- Die Shell MUSS diese Funktionen ohne eigene Implementierungslogik darstellen können.
- Capability-Aktionen MÜSSEN über die zuständige Runtime ausgeführt werden.
- Nicht vertrauenswürdige Erweiterungen MÜSSEN isoliert werden können.
- Das Entfernen einer Capability MUSS ihre Schnellfunktion automatisch deaktivieren oder entfernen.

## 10. Performance und Darstellung

Quick Settings MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Das Panel SOLL bei Bedarf initialisiert werden.
- Häufig verwendete Ressourcen SOLLEN zwischengespeichert werden.
- Statusänderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Unveränderte Bereiche DÜRFEN nicht permanent neu gerendert werden.
- Damage Tracking MUSS unterstützt werden.
- Animationen MÜSSEN über die Nova UI Animation Engine und den Compositor erfolgen.
- Reduced Motion MUSS berücksichtigt werden.
- Fehlerhafte Erweiterungen DÜRFEN die Desktop Shell nicht beeinträchtigen.

## 11. Mehrmonitorbetrieb

Quick Settings MUSS mehrere Bildschirme berücksichtigen.

Dabei gilt:

- Das Panel SOLL auf dem Monitor des auslösenden Bedienelements erscheinen.
- Die Positionierung MUSS Bildschirmgrenzen und verfügbare Arbeitsflächen berücksichtigen.
- Unterschiedliche DPI-Skalierungen MÜSSEN unterstützt werden.
- Systemweite Einstellungen MÜSSEN unabhängig vom verwendeten Monitor konsistent bleiben.
- Monitorspezifische Einstellungen MÜSSEN dem richtigen Ausgabegerät zugeordnet werden.

## 12. Zustand und Wiederherstellung

Der Quick Settings State Manager speichert benutzerbezogen:

- Sichtbare Schnellfunktionen.
- Reihenfolge und Gruppierung.
- Layoutoptionen.
- Benutzerdefinierte Anpassungen.

Dabei gilt:

- Einstellungen MÜSSEN einen Shell-Neustart überstehen.
- Die Wiederherstellung DARF keine Systemaktionen unbeabsichtigt erneut ausführen.
- Beschädigte Konfigurationen DÜRFEN den Start der Desktop Shell nicht verhindern.
- Eine Standardkonfiguration MUSS wiederherstellbar sein.
- Benutzerdefinierte Layouts SOLLEN exportierbar sein.

## 13. Sicherheit und Barrierefreiheit

Nova Quick Settings unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Aktionen MÜSSEN vor ihrer Ausführung autorisiert werden.
- Schnellfunktionen DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Sicherheitskritische Einstellungen MÜSSEN erforderliche Bestätigungen berücksichtigen.
- Anwendungen DÜRFEN keine fremden Systemeinstellungen ohne Berechtigung verändern.
- Alle Bedienelemente MÜSSEN per Tastatur erreichbar sein.
- Screenreader, High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Das Subsystem MUSS vollständig offline und ohne KI funktionieren.

## 14. Normative Anforderungen

- **SYSTEM-QUICK-001:** NovaOS MUSS ein zentrales Quick-Settings-Panel bereitstellen.
- **SYSTEM-QUICK-002:** Die Darstellung MUSS das native Nova UI Framework verwenden.
- **SYSTEM-QUICK-003:** Schnellfunktionen MÜSSEN dynamisch registrierbar sein.
- **SYSTEM-QUICK-004:** Systemzustände MÜSSEN von den zuständigen Diensten bereitgestellt werden.
- **SYSTEM-QUICK-005:** Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- **SYSTEM-QUICK-006:** Toggle-, Slider-, Auswahl- und Aktionsbedienelemente MÜSSEN unterstützt werden.
- **SYSTEM-QUICK-007:** Der Benutzer MUSS Schnellfunktionen individuell anordnen können.
- **SYSTEM-QUICK-008:** Benutzerdefinierte Anordnungen MÜSSEN gespeichert werden.
- **SYSTEM-QUICK-009:** Quick Settings DARF keine eigene Hardwareverwaltung implementieren.
- **SYSTEM-QUICK-010:** Aktionen MÜSSEN über autorisierte Commands oder Capabilities erfolgen.
- **SYSTEM-QUICK-011:** Nicht verfügbare Funktionen MÜSSEN entsprechend gekennzeichnet werden.
- **SYSTEM-QUICK-012:** Die Oberfläche MUSS Damage Tracking unterstützen.
- **SYSTEM-QUICK-013:** Mehrmonitorbetrieb und DPI-Skalierung MÜSSEN berücksichtigt werden.
- **SYSTEM-QUICK-014:** Fehlerhafte Erweiterungen DÜRFEN die Shell nicht beeinträchtigen.
- **SYSTEM-QUICK-015:** Quick Settings MUSS barrierefrei bedienbar sein.
- **SYSTEM-QUICK-016:** Nova Quick Settings MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Desktop Shell Architecture:** Einbindung in die Desktopumgebung.
- **Nova System Tray:** Schnellzugriff und Statusanzeige.
- **Nova Taskbar Architecture:** Öffnen des Quick-Settings-Panels.
- **Nova Notification Center:** Integration von Do Not Disturb.
- **Nova UI Framework:** Bedienelemente und Darstellung.
- **Nova UI Animation Engine:** Animationen und Übergänge.
- **Nova Compositor:** Oberflächenkomposition und Damage Tracking.
- **Nova Display Server:** Anzeige- und Monitoreinstellungen.
- **Nova Audio Services:** Lautstärke und Audiogeräte.
- **Nova Network Services:** Netzwerk- und Funkverbindungen.
- **Nova Power Management:** Energie- und Leistungsprofile.
- **Nova Capability System:** Erweiterbare Schnellfunktionen.
- **Nova System Settings:** Vollständige Systemeinstellungen.
- **Nova.Trust:** Berechtigungen und Sicherheitsrichtlinien.

## 16. Ergebnis

NovaOS erhält ein einheitliches, erweiterbares und individuell anpassbares Quick-Settings-System.

Häufig verwendete Einstellungen können unmittelbar geändert werden, während die eigentliche Systemsteuerung weiterhin durch die zuständigen Dienste erfolgt.

Durch die Capability-Integration können neue Funktionen hinzugefügt werden, ohne die Desktop Shell verändern zu müssen.

**Grundsatz:** Quick Settings stellt genau die Funktionen bereit, die der Benutzer gerade benötigt – schnell erreichbar, verständlich und ohne unnötige Komplexität.