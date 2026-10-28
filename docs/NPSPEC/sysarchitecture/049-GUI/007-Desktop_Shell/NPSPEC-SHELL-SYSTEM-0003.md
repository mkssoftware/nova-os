# NPSPEC-SHELL-SYSTEM-0003 – Nova Lock Screen

**Status:** Angenommen  
**Bereich:** Shell / System / Lock Screen  
**System:** NovaOS

## 1. Zweck

Nova Lock Screen definiert den Sperrbildschirm von NovaOS.

Er schützt eine aktive Benutzersitzung vor unautorisiertem Zugriff, ohne laufende Anwendungen oder Hintergrundprozesse unnötig zu unterbrechen.

Der Sperrbildschirm MUSS Sicherheit, schnelle Wiederaufnahme und eine ruhige, vertraute Benutzeroberfläche miteinander verbinden.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Lock Screen Manager | Verwaltung des Sperrbildschirm-Lebenszyklus |
| Lock Screen View | Darstellung der Sperroberfläche |
| Lock State Controller | Verwaltung des Sperrzustands |
| Authentication Controller | Anbindung an die Benutzerauthentifizierung |
| Lock Screen Policy Manager | Auswertung von Sperr- und Sicherheitsrichtlinien |
| Lock Screen State Manager | Speicherung benutzerbezogener Einstellungen |

Dabei gilt:

- Der Sperrbildschirm MUSS über einen geschützten Systemkontext betrieben werden.
- Die Desktop Shell DARF die Sperre nicht eigenständig aufheben.
- Authentifizierung MUSS durch die zuständigen Sicherheitsdienste erfolgen.
- Ein Absturz der Desktop Shell DARF eine gesperrte Sitzung nicht freigeben.
- Die grafische Darstellung MUSS vom sicherheitsrelevanten Sperrzustand getrennt sein.

## 3. Sperrzustände

NovaOS unterstützt folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Unlocked | Benutzersitzung ist freigegeben |
| Locking | Sperrvorgang wird durchgeführt |
| Locked | Sitzung ist geschützt |
| Authenticating | Authentifizierung läuft |
| Unlocking | Sitzung wird freigegeben |

Dabei gilt:

- Zustandsübergänge MÜSSEN eindeutig definiert sein.
- Eine Sitzung DARF erst nach erfolgreicher Authentifizierung freigegeben werden.
- Fehlgeschlagene Authentifizierungen MÜSSEN den gesperrten Zustand erhalten.
- Sicherheitsrelevante Zustandsänderungen MÜSSEN nachvollziehbar protokolliert werden.

## 4. Aktivierung

Der Sperrbildschirm MUSS über folgende Ereignisse aktiviert werden können:

- Manuelle Sperrung.
- Konfigurierbare Tastenkombination.
- Benutzerdefinierte Inaktivitätszeit.
- Sicherheitsrichtlinien.
- Wechsel des Benutzerkontos.
- Optionale Systemereignisse.

Dabei gilt:

- Automatische Sperrzeiten MÜSSEN konfigurierbar sein, soweit Sicherheitsrichtlinien dies erlauben.
- Die Sperrung MUSS unabhängig von der Desktop Shell ausgelöst werden können.
- Der Benutzer MUSS seine Sitzung jederzeit manuell sperren können.
- Eine Sperrung DARF laufende Anwendungen nicht automatisch beenden.

## 5. Darstellung

Der Sperrbildschirm verwendet das native Nova UI Framework.

Unterstützte Elemente:

- Hintergrundbild.
- Datum und Uhrzeit.
- Benutzername und Benutzerbild.
- Authentifizierungsbereich.
- Netzwerkstatus.
- Energie- und Akkustatus.
- Optionale Benachrichtigungsindikatoren.
- Barrierefreiheitsfunktionen.

Dabei gilt:

- Die Oberfläche MUSS das aktive Theme berücksichtigen.
- Hintergrundbilder MÜSSEN individuell konfigurierbar sein.
- Animationen SOLLEN dezent und optional sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Die Darstellung MUSS Damage Tracking berücksichtigen.
- Nicht verfügbare Systeminformationen DÜRFEN die Sperrfunktion nicht beeinträchtigen.

## 6. Authentifizierung

Nova Lock Screen MUSS die NovaOS-Authentifizierungsdienste verwenden.

Unterstützte Verfahren können sein:

- Passwort.
- PIN.
- Hardwarebasierte Authentifizierung.
- Biometrische Verfahren, sofern unterstützt.
- Weitere autorisierte Authentifizierungsmethoden.

Dabei gilt:

- Der Sperrbildschirm DARF Zugangsdaten nicht selbst dauerhaft speichern.
- Authentifizierungsdaten MÜSSEN über geschützte Schnittstellen verarbeitet werden.
- Fehlversuche MÜSSEN gemäß den geltenden Sicherheitsrichtlinien behandelt werden.
- Alternative Authentifizierungsverfahren DÜRFEN die Sicherheitsanforderungen nicht umgehen.
- Nach erfolgreicher Authentifizierung MUSS ausschließlich die autorisierte Sitzung freigegeben werden.

## 7. Sitzungsisolation

Während der Sperrung MUSS die Benutzersitzung geschützt bleiben.

Dabei gilt:

- Anwendungen DÜRFEN im Hintergrund weiterlaufen, sofern keine Sicherheits- oder Energierichtlinie dies untersagt.
- Benutzeroberflächen der gesperrten Sitzung DÜRFEN keine Eingaben erhalten.
- Vertrauliche Fensterinhalte DÜRFEN nicht sichtbar sein.
- Zwischenablage und Drag & Drop MÜSSEN gegenüber der Sperroberfläche isoliert bleiben.
- Andere Benutzer DÜRFEN die gesperrte Sitzung nicht übernehmen.
- Die Freigabe MUSS den vorherigen Sitzungszustand erhalten.

## 8. Benachrichtigungen und Systemstatus

Der Sperrbildschirm DARF ausgewählte Systeminformationen anzeigen.

Dabei gilt:

- Benachrichtigungsinhalte MÜSSEN entsprechend den Datenschutzrichtlinien gefiltert werden.
- Sensible Inhalte MÜSSEN standardmäßig geschützt sein.
- Der Benutzer MUSS die Sichtbarkeit regulärer Benachrichtigungen konfigurieren können.
- Sicherheitskritische Statusinformationen DÜRFEN ohne Offenlegung vertraulicher Daten angezeigt werden.
- Benachrichtigungsaktionen, die eine entsperrte Sitzung erfordern, MÜSSEN eine Authentifizierung auslösen.

## 9. Mehrmonitorbetrieb

Nova Lock Screen MUSS mehrere Bildschirme unterstützen.

Dabei gilt:

- Alle Ausgabegeräte der gesperrten Sitzung MÜSSEN vor unautorisiertem Zugriff geschützt sein.
- Der Authentifizierungsbereich SOLL auf einem geeigneten Bildschirm erscheinen.
- Andere Bildschirme DÜRFEN eine reduzierte Sperrdarstellung verwenden.
- Änderungen der Monitorkonfiguration DÜRFEN die Sitzungssperre nicht umgehen.
- Unterschiedliche Auflösungen und DPI-Skalierungen MÜSSEN berücksichtigt werden.

## 10. Energieverwaltung

Der Sperrbildschirm MUSS mit dem Nova Power Management zusammenarbeiten.

Dabei gilt:

- Bildschirmabschaltung und Energiesparmodi MÜSSEN unterstützt werden.
- Die Sitzung MUSS nach dem Aufwachen weiterhin gesperrt bleiben, sofern die geltende Richtlinie dies verlangt.
- Der Sperrbildschirm DARF Energiesparzustände nicht unnötig verhindern.
- Hintergrundanimationen MÜSSEN bei ausgeschaltetem Bildschirm pausieren.
- Der Energiezustand MUSS bei Bedarf angezeigt werden können.

## 11. Wiederherstellung und Ausfallsicherheit

Die Sperrfunktion MUSS auch bei Fehlern der Desktop Shell zuverlässig bleiben.

Dabei gilt:

- Ein Shell-Neustart DARF die Sitzung nicht entsperren.
- Ein Absturz der Sperrbildschirmdarstellung MUSS eine geschützte Wiederherstellung auslösen.
- Bei fehlgeschlagener Wiederherstellung MUSS die Sitzung gesperrt bleiben.
- Sicherheitsrelevante Zustände DÜRFEN nicht allein aus gespeicherten UI-Daten rekonstruiert werden.
- Eine minimale, funktionsfähige Authentifizierungsoberfläche MUSS als Rückfalllösung verfügbar sein.

## 12. Sicherheit und Barrierefreiheit

Nova Lock Screen unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Die Sperroberfläche MUSS gegenüber gewöhnlichen Anwendungen isoliert sein.
- Nicht autorisierte Prozesse DÜRFEN die Sperroberfläche nicht überlagern oder Eingaben abfangen.
- Sicherheitsrelevante Eingaben MÜSSEN über vertrauenswürdige Eingabepfade verarbeitet werden.
- Anwendungen DÜRFEN den Sperrzustand nicht eigenmächtig verändern.
- Screenreader und alternative Eingabemethoden MÜSSEN unterstützt werden.
- Barrierefreiheitsfunktionen DÜRFEN die Authentifizierung nicht umgehen.
- Nova Lock Screen MUSS vollständig offline und ohne KI funktionieren.

## 13. Normative Anforderungen

- **SYSTEM-LOCK-001:** NovaOS MUSS einen geschützten Sperrbildschirm bereitstellen.
- **SYSTEM-LOCK-002:** Der Sperrzustand MUSS unabhängig von der Desktop Shell verwaltet werden.
- **SYSTEM-LOCK-003:** Die Freigabe MUSS eine erfolgreiche Authentifizierung voraussetzen.
- **SYSTEM-LOCK-004:** Laufende Anwendungen DÜRFEN durch die Sperrung nicht automatisch beendet werden.
- **SYSTEM-LOCK-005:** Vertrauliche Sitzungsinhalte MÜSSEN geschützt bleiben.
- **SYSTEM-LOCK-006:** Die Sperroberfläche MUSS das native Nova UI Framework verwenden können.
- **SYSTEM-LOCK-007:** Mehrmonitorbetrieb MUSS unterstützt werden.
- **SYSTEM-LOCK-008:** Automatische und manuelle Sperrung MÜSSEN möglich sein.
- **SYSTEM-LOCK-009:** Benachrichtigungen MÜSSEN datenschutzkonform dargestellt werden.
- **SYSTEM-LOCK-010:** Ein Shell-Absturz DARF die Sitzung nicht freigeben.
- **SYSTEM-LOCK-011:** Eine geschützte Rückfalloberfläche MUSS verfügbar sein.
- **SYSTEM-LOCK-012:** Der Sperrbildschirm MUSS mit der Energieverwaltung zusammenarbeiten.
- **SYSTEM-LOCK-013:** Nova.Trust und Accessibility MÜSSEN berücksichtigt werden.
- **SYSTEM-LOCK-014:** Nova Lock Screen MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Session Management:** Sitzungszustand und Benutzerwechsel.
- **Nova Authentication:** Authentifizierung und Sitzungsfreigabe.
- **Nova.Trust:** Identität, Berechtigungen und Sicherheitsrichtlinien.
- **Nova Desktop Shell:** Aktivierung und Benutzeroberfläche.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova Window Manager:** Sitzungsfenster und Eingabefokus.
- **Nova Display Server:** Mehrmonitorbetrieb.
- **Nova Input System:** Geschützte Eingabeverarbeitung.
- **Nova Notification Center:** Datenschutzkonforme Benachrichtigungen.
- **Nova System Status Indicators:** Systemstatus.
- **Nova Power Management:** Bildschirmabschaltung und Energiesparzustände.
- **Nova Shell Recovery:** Wiederherstellung der Sperroberfläche.

## 15. Ergebnis

NovaOS erhält einen sicheren, zuverlässigen und individuell anpassbaren Sperrbildschirm.

Die aktive Benutzersitzung bleibt geschützt, während Anwendungen und Hintergrunddienste entsprechend ihren Richtlinien weiterarbeiten können.

Durch die Trennung von Sicherheitszustand und grafischer Darstellung bleibt die Sperrfunktion auch bei Fehlern der Desktop Shell erhalten.

**Grundsatz:** NovaOS schützt die Privatsphäre des Benutzers, ohne seine Arbeit unnötig zu unterbrechen. Sicherheit bleibt jederzeit gewährleistet – auch dann, wenn die Benutzeroberfläche ausfällt.