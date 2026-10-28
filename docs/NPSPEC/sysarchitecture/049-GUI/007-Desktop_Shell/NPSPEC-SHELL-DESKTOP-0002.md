# NPSPEC-SHELL-DESKTOP-0002 – Nova Desktop Background

**Status:** Angenommen  
**Bereich:** Shell / Desktop / Background  
**System:** NovaOS

## 1. Zweck

Nova Desktop Background definiert die Verwaltung und Darstellung des Desktophintergrunds von NovaOS.

Ziel ist eine moderne, individuell anpassbare und ressourcenschonende Hintergrunddarstellung, die sich harmonisch in die Desktopumgebung integriert.

Der Hintergrund MUSS unabhängig von Desktop-Elementen, Taskleiste und Anwendungsfenstern verwaltet werden.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Background Manager | Verwaltung der Hintergrundkonfiguration |
| Background Provider | Bereitstellung von Hintergrundinhalten |
| Background Renderer | Aufbereitung der Hintergrunddarstellung |
| Background Scheduler | Steuerung zeitabhängiger Änderungen |
| Background State Manager | Speicherung und Wiederherstellung |

Die eigentliche Bildkomposition erfolgt über den Nova Compositor.

Der Background Renderer DARF keine eigene Displayausgabe implementieren.

## 3. Hintergrundtypen

NovaOS unterstützt folgende Hintergrundtypen:

- Einfarbige Hintergründe.
- Farbverläufe.
- Statische Bilder.
- Bilderserien und Diashows.
- Animierte Hintergründe.
- Dynamisch generierte Hintergründe.

Weitere Hintergrundtypen MÜSSEN über registrierte Background Provider integrierbar sein.

Alle Hintergrundtypen MÜSSEN ohne Netzwerkverbindung verwendbar sein können.

## 4. Bilddarstellung

Für statische Bilder werden folgende Darstellungsmodi unterstützt:

| Modus | Verhalten |
|---|---|
| Fill | Bildschirm vollständig ausfüllen, gegebenenfalls beschneiden |
| Fit | Vollständiges Bild unter Beibehaltung des Seitenverhältnisses anzeigen |
| Stretch | Bild auf Bildschirmgröße strecken |
| Center | Bild zentriert darstellen |
| Tile | Bild wiederholt darstellen |
| Span | Bild über mehrere Monitore verteilen |

Dabei gilt:

- Seitenverhältnisse MÜSSEN bei `Fill` und `Fit` erhalten bleiben.
- Unterschiedliche Auflösungen und DPI-Skalierungen MÜSSEN unterstützt werden.
- Die Bildverarbeitung SOLL vorhandene GPU-Funktionen nutzen können.
- Farbprofile und Ausgabefarbräume MÜSSEN über die zuständigen Grafiksubsysteme berücksichtigt werden.

## 5. Mehrmonitorbetrieb

Nova Desktop Background MUSS mehrere Monitore unterstützen.

Dabei gilt:

- Jeder Monitor MUSS einen eigenen Hintergrund besitzen können.
- Ein gemeinsames Hintergrundbild über mehrere Monitore MUSS möglich sein.
- Unterschiedliche Auflösungen und Skalierungen MÜSSEN berücksichtigt werden.
- Änderungen der Monitorkonfiguration MÜSSEN erkannt werden.
- Gespeicherte Hintergrundeinstellungen SOLLEN nach erneutem Anschließen eines Monitors wiederhergestellt werden.

## 6. Dynamische Hintergründe

Dynamische Hintergründe können auf definierte Ereignisse oder Bedingungen reagieren.

Beispiele:

- Tageszeit.
- Benutzerdefinierte Zeitpläne.
- Theme-Wechsel.
- Systemzustände.
- Benutzerinteraktionen.

Dabei gilt:

- Dynamische Hintergründe MÜSSEN ereignisgesteuert arbeiten können.
- Änderungen SOLLEN nur bei tatsächlichem Bedarf erfolgen.
- Permanente Aktualisierungen ohne sichtbare Änderungen DÜRFEN nicht stattfinden.
- Der Benutzer MUSS dynamische Hintergründe deaktivieren können.
- Ein statischer Ersatzhintergrund MUSS verfügbar sein.

## 7. Animationen

Animierte Hintergründe verwenden die Nova Animation Engine beziehungsweise geeignete Grafik- und Medienkomponenten.

Dabei gilt:

- Animationen MÜSSEN pausierbar sein.
- Bei verdecktem Desktop SOLLEN Animationen ausgesetzt werden.
- Reduced Motion MUSS berücksichtigt werden.
- Bildwiederholraten SOLLEN an den tatsächlichen Darstellungsbedarf angepasst werden.
- Animationen DÜRFEN die Bedienbarkeit des Desktops nicht beeinträchtigen.

## 8. Ressourcenverwaltung

Die Hintergrunddarstellung MUSS ressourcenschonend erfolgen.

Dabei gilt:

- Dekodierte Bilder SOLLEN zwischengespeichert werden.
- Hintergrundressourcen SOLLEN nur bei Bedarf geladen werden.
- Nicht verwendete Ressourcen MÜSSEN freigegeben werden können.
- Große Bilder SOLLEN passend zur Ausgabeauflösung verarbeitet werden.
- Bei Ressourcenknappheit MUSS auf eine einfachere Darstellung zurückgegriffen werden können.

Ein statischer Hintergrund DARF keine kontinuierliche CPU- oder GPU-Aktivität verursachen.

## 9. Rendering und Damage Tracking

Die Hintergrunddarstellung MUSS mit dem Nova Compositor zusammenarbeiten.

Dabei gilt:

- Unveränderte Hintergründe DÜRFEN nicht permanent neu gerendert werden.
- Änderungen MÜSSEN gezielt invalidiert werden.
- Bei statischen Hintergründen SOLLEN vorbereitete Bildressourcen wiederverwendet werden.
- Animierte Hintergründe MÜSSEN ausschließlich tatsächlich veränderte Bereiche aktualisieren können.
- Das Verschieben von Fenstern DARF keine unnötige Neuberechnung des Hintergrundinhalts auslösen.

Die Zusammensetzung sichtbarer Flächen bleibt Aufgabe des Compositors.

## 10. Konfiguration und Benutzerkontrolle

Der Benutzer MUSS Hintergründe über die NovaOS-Systemeinstellungen verwalten können.

Unterstützt werden:

- Auswahl des Hintergrundtyps.
- Auswahl von Bildern und Bilderserien.
- Darstellungsmodus.
- Monitorzuordnung.
- Wechselintervalle.
- Animationseinstellungen.
- Aktivierung und Deaktivierung dynamischer Funktionen.

Dabei gilt:

- Änderungen SOLLEN unmittelbar sichtbar sein.
- Einstellungen MÜSSEN gespeichert werden.
- Der vorherige Hintergrund SOLL wiederherstellbar sein.
- Automatische Änderungen MÜSSEN vom Benutzer kontrollierbar bleiben.

## 11. Fehlerbehandlung

Fehlerhafte Hintergrundinhalte DÜRFEN die Desktop Shell nicht beeinträchtigen.

Dabei gilt:

- Nicht lesbare Dateien MÜSSEN erkannt werden.
- Fehlerhafte Bilddaten DÜRFEN keinen Shell-Absturz verursachen.
- Fehlgeschlagene Background Provider MÜSSEN isoliert werden können.
- Bei Fehlern MUSS ein gültiger Ersatzhintergrund verwendet werden.
- Fehler MÜSSEN über Nova Diagnostics nachvollziehbar sein.

## 12. Sicherheit

Nova Desktop Background unterliegt Nova.Trust.

Dabei gilt:

- Background Provider MÜSSEN autorisiert sein.
- Hintergrunddateien MÜSSEN über die vorgesehenen VFS-Schnittstellen geladen werden.
- Externe Inhalte DÜRFEN nicht ohne entsprechende Berechtigung abgerufen werden.
- Animierte oder dynamische Hintergründe DÜRFEN keine zusätzlichen Capabilities erhalten.
- Hintergrundinhalte DÜRFEN keine unautorisierten Systemoperationen ausführen.

## 13. Normative Anforderungen

- **DESKTOP-001:** NovaOS MUSS ein eigenständiges Desktop Background System bereitstellen.
- **DESKTOP-002:** Statische Bilder, Farben und Farbverläufe MÜSSEN unterstützt werden.
- **DESKTOP-003:** Animierte und dynamische Hintergründe MÜSSEN optional integrierbar sein.
- **DESKTOP-004:** Unterschiedliche Bilddarstellungsmodi MÜSSEN unterstützt werden.
- **DESKTOP-005:** Mehrmonitorbetrieb MUSS unterstützt werden.
- **DESKTOP-006:** Hintergrundeinstellungen MÜSSEN benutzerbezogen gespeichert werden.
- **DESKTOP-007:** Die Darstellung MUSS über die zuständigen Grafiksubsysteme erfolgen.
- **DESKTOP-008:** Statische Hintergründe DÜRFEN keine permanente Neuberechnung verursachen.
- **DESKTOP-009:** Dynamische Hintergründe MÜSSEN bedarfsgesteuert arbeiten.
- **DESKTOP-010:** Reduced Motion MUSS berücksichtigt werden.
- **DESKTOP-011:** Hintergrundressourcen MÜSSEN kontrolliert verwaltet werden.
- **DESKTOP-012:** Fehlerhafte Hintergründe DÜRFEN die Desktop Shell nicht zum Absturz bringen.
- **DESKTOP-013:** Ein gültiger Ersatzhintergrund MUSS verfügbar sein.
- **DESKTOP-014:** Background Provider MÜSSEN Nova.Trust berücksichtigen.
- **DESKTOP-015:** Nova Desktop Background MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Desktop Environment:** Verwaltung des Desktoparbeitsbereichs.
- **Nova Desktop Shell Architecture:** Einbindung in die Shell.
- **Nova Shell Lifecycle:** Initialisierung und Beendigung.
- **Nova Shell Recovery:** Fehlerbehandlung und Wiederherstellung.
- **Nova UI Framework:** Konfigurationsoberflächen.
- **Nova Compositor:** Bildkomposition und Damage Tracking.
- **Nova Render Engine:** Bildverarbeitung und Rendering.
- **Nova Display Server:** Monitorauflösungen und Ausgabekonfiguration.
- **Nova Color Management:** Farbprofile und Farbräume.
- **Nova Animation Engine:** Animierte Hintergründe.
- **Nova VFS:** Zugriff auf Hintergrunddateien.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 15. Ergebnis

NovaOS erhält ein flexibles und ressourcenschonendes System zur Darstellung und Verwaltung von Desktophintergründen.

Statische, animierte und dynamische Hintergründe können individuell konfiguriert und auf mehreren Monitoren dargestellt werden.

Durch bedarfsgesteuerte Aktualisierung, Damage Tracking und kontrollierte Ressourcenverwaltung bleibt die Hintergrunddarstellung effizient.

**Grundsatz:** Ein Desktophintergrund soll die Arbeitsumgebung bereichern, ohne unnötig Aufmerksamkeit oder Systemressourcen zu beanspruchen.