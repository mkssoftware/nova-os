# NPSPEC-SHELL-SYSTEM-0004 – Nova System Dialog Integration

**Status:** Angenommen  
**Bereich:** Shell / System / Dialog Integration  
**System:** NovaOS

## 1. Zweck

Nova System Dialog Integration definiert die einheitliche Einbindung und Darstellung von Systemdialogen innerhalb der NovaOS Desktop Shell.

Systemdialoge ermöglichen standardisierte Benutzerinteraktionen für Dateioperationen, Berechtigungen, Einstellungen, Bestätigungen und andere Systemfunktionen.

Ziel ist eine konsistente, sichere und barrierefreie Dialogdarstellung, unabhängig davon, welche Anwendung, Solution oder Capability den Dialog anfordert.

**Grundprinzip:** Die zuständige Systemkomponente definiert den Dialoginhalt und seine Aktionen. Die Shell übernimmt die Integration in die Desktopumgebung.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| System Dialog Manager | Koordination von Dialoganforderungen |
| Dialog Registry | Registrierung verfügbarer Dialogtypen |
| Dialog Resolver | Auflösung des zuständigen Dialoganbieters |
| Dialog Presentation Controller | Darstellung und Fensterintegration |
| Dialog Interaction Controller | Verarbeitung von Benutzerinteraktionen |
| Dialog Lifecycle Manager | Verwaltung des Dialoglebenszyklus |
| Dialog Result Dispatcher | Rückgabe strukturierter Ergebnisse |

Dabei gilt:

- Systemdialoge MÜSSEN über definierte Schnittstellen angefordert werden.
- Die Darstellung MUSS das native Nova UI Framework verwenden können.
- Dialoglogik und Darstellung MÜSSEN voneinander getrennt bleiben.
- Sicherheitskritische Dialoge MÜSSEN unabhängig von der gewöhnlichen Desktop Shell betrieben werden können.
- Anwendungen DÜRFEN keine privilegierten Systemdialoge imitieren.

## 3. Unterstützte Dialogtypen

NovaOS unterstützt standardisierte Dialoge für:

- Datei öffnen und speichern.
- Ordner auswählen.
- Datei ersetzen und Konflikte auflösen.
- Eigenschaften.
- Berechtigungen und Capabilities.
- Authentifizierung.
- Warnungen, Fehler und Informationen.
- Fortschrittsanzeigen.
- Drucken.
- Freigaben.
- Geräteauswahl.
- Netzwerkverbindungen.
- Anmeldedaten.
- Installation und Updates.
- Farb- und Schriftauswahl.
- Datum und Uhrzeit.
- Absturzberichte.
- Recovery und Systemdiagnose.

Weitere Dialogtypen MÜSSEN über die Dialog Registry registrierbar sein.

## 4. Dialogmodell

Eine Dialoganforderung enthält mindestens:

- Eindeutige Anforderungs-ID.
- Dialogtyp.
- Identität des Aufrufers.
- Sicherheitskontext.
- Dialogparameter.
- Optionales Besitzerfenster.
- Erwarteten Ergebnistyp.

Optional können enthalten sein:

- Titel und Beschreibung.
- Vordefinierte Auswahlwerte.
- Validierungsregeln.
- Verfügbare Aktionen.
- Abbruchbedingungen.
- Darstellungsoptionen.

Dabei gilt:

- Dialogparameter MÜSSEN strukturiert und validierbar sein.
- Ergebnisse MÜSSEN typisiert zurückgegeben werden.
- Dialoganforderungen DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Nicht unterstützte Parameter MÜSSEN kontrolliert behandelt werden.

## 5. Dialoglebenszyklus

Ein Systemdialog durchläuft folgende Zustände:

1. **Requested:** Dialog wurde angefordert.
2. **Resolving:** Zuständiger Dialoganbieter wird ermittelt.
3. **Preparing:** Dialog wird vorbereitet.
4. **Visible:** Dialog ist sichtbar.
5. **Processing:** Benutzeraktion wird verarbeitet.
6. **Completed:** Dialog wurde erfolgreich abgeschlossen.
7. **Cancelled:** Dialog wurde abgebrochen.
8. **Failed:** Dialog konnte nicht abgeschlossen werden.

Dabei gilt:

- Zustandsübergänge MÜSSEN eindeutig definiert sein.
- Dialoganforderungen MÜSSEN asynchron verarbeitet werden können.
- Ein Dialog MUSS ein eindeutiges Abschlussresultat liefern.
- Mehrfache Ergebnisübermittlungen MÜSSEN verhindert werden.
- Fehlerhafte Dialoge DÜRFEN die aufrufende Anwendung nicht dauerhaft blockieren.

## 6. Fensterintegration und Modalität

NovaOS unterstützt:

- Modale Dialoge.
- Nicht modale Dialoge.
- Fenstergebundene Dialoge.
- Systemweite Dialoge.
- Geschützte Sicherheitsdialoge.

Dabei gilt:

- Fenstergebundene Dialoge MÜSSEN ihrem Besitzerfenster zugeordnet werden.
- Modale Dialoge DÜRFEN nur den vorgesehenen Interaktionsbereich blockieren.
- Fokus und Z-Reihenfolge MÜSSEN durch den Nova Window Manager verwaltet werden.
- Dialoge MÜSSEN innerhalb der verfügbaren Bildschirmfläche positioniert werden.
- Sicherheitsdialoge MÜSSEN über einen vertrauenswürdigen Darstellungs- und Eingabepfad verfügen.
- Ein Dialog DARF nicht unbeabsichtigt hinter seinem Besitzerfenster verschwinden.

## 7. Benutzerinteraktion

Systemdialoge MÜSSEN einheitliche Bedienkonzepte verwenden.

Dabei gilt:

- Bestätigen und Abbrechen MÜSSEN konsistent funktionieren.
- Standardaktionen MÜSSEN eindeutig erkennbar sein.
- Destruktive Aktionen MÜSSEN entsprechend gekennzeichnet werden.
- Eingaben MÜSSEN vor ihrer Verarbeitung validiert werden.
- Fehler MÜSSEN verständlich und möglichst unmittelbar am betroffenen Bedienelement angezeigt werden.
- Dialoge MÜSSEN vollständig per Tastatur bedienbar sein.
- Der Benutzer MUSS eine Aktion abbrechen können, sofern der zugrunde liegende Vorgang dies zulässt.

## 8. Capability-Integration

Systemdialoge MÜSSEN das Capability-basierte Architekturmodell von NovaOS unterstützen.

Dabei gilt:

- Capabilities MÜSSEN standardisierte Dialoge anfordern können.
- Dialoganbieter MÜSSEN über definierte Schnittstellen austauschbar sein.
- Anwendungen und Solutions MÜSSEN dieselben Systemdialoge verwenden können.
- Die Dialogdarstellung DARF keine fachliche Capability-Implementierung enthalten.
- Dialogaktionen MÜSSEN an die zuständigen Systemdienste oder Capabilities delegiert werden.
- Erweiterungen DÜRFEN keine geschützten Dialogtypen ohne entsprechende Autorisierung ersetzen.

## 9. Darstellung und Anpassung

Die Darstellung erfolgt über das Nova UI Framework.

Dabei gilt:

- Dialoge MÜSSEN das aktive Theme berücksichtigen.
- Styles und Templates MÜSSEN unterstützt werden.
- Unterschiedliche Bildschirmgrößen und DPI-Skalierungen MÜSSEN berücksichtigt werden.
- Dialoge SOLLEN ihre Größe an Inhalt und verfügbare Bildschirmfläche anpassen.
- Animationen SOLLEN dezent und optional sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Unveränderte Dialogbereiche DÜRFEN nicht permanent neu gerendert werden.
- Damage Tracking MUSS unterstützt werden.

## 10. Dialogergebnisse und Abbruch

Der Dialog Result Dispatcher übermittelt Ergebnisse an den Aufrufer.

Dabei gilt:

- Ergebnisse MÜSSEN eindeutig einer Dialoganforderung zugeordnet werden.
- Erfolgreiche, abgebrochene und fehlgeschlagene Vorgänge MÜSSEN unterscheidbar sein.
- Ergebnisse MÜSSEN dem definierten Ergebnisschema entsprechen.
- Das Schließen eines Dialogfensters DARF nicht automatisch als erfolgreiche Bestätigung gelten.
- Beim Beenden des Aufrufers MÜSSEN zugehörige Dialoge kontrolliert beendet oder gemäß ihrer Richtlinie weitergeführt werden.
- Bereits ausgeführte Systemaktionen DÜRFEN durch einen Dialogabbruch nicht unbeabsichtigt wiederholt werden.

## 11. Performance und Fehlerbehandlung

Nova System Dialog Integration MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Häufig verwendete Dialogressourcen SOLLEN zwischengespeichert werden.
- Dialoganforderungen DÜRFEN die UI Event Loop nicht blockieren.
- Lang laufende Operationen MÜSSEN asynchron ausführbar sein.
- Nicht erreichbare Dialoganbieter MÜSSEN kontrolliert behandelt werden.
- Fehlerhafte Erweiterungen DÜRFEN die Desktop Shell nicht beeinträchtigen.
- Für wesentliche System- und Sicherheitsdialoge MÜSSEN geeignete Rückfallmechanismen verfügbar sein.
- Ein Shell-Neustart DARF laufende privilegierte Systemoperationen nicht unkontrolliert verändern.

## 12. Sicherheit und Datenschutz

Nova System Dialog Integration unterliegt Nova.Trust.

Dabei gilt:

- Dialoganforderungen MÜSSEN dem tatsächlichen Aufrufer zugeordnet werden.
- Privilegierte Dialogtypen MÜSSEN ausschließlich von autorisierten Komponenten bereitgestellt werden.
- Berechtigungs- und Authentifizierungsdialoge MÜSSEN gegen Manipulation und Imitation geschützt sein.
- Sensible Eingaben MÜSSEN über geschützte Schnittstellen verarbeitet werden.
- Anwendungen DÜRFEN keine Zugangsdaten aus fremden Dialogen auslesen.
- Dialogergebnisse DÜRFEN nur an autorisierte Empfänger übermittelt werden.
- Sicherheitsentscheidungen MÜSSEN durch die zuständigen Sicherheitsdienste getroffen werden.
- Das Subsystem MUSS vollständig offline und ohne KI funktionieren.

## 13. Normative Anforderungen

- **SYSTEM-DIALOG-001:** NovaOS MUSS eine zentrale Systemdialog-Integration bereitstellen.
- **SYSTEM-DIALOG-002:** Dialogtypen MÜSSEN strukturiert registrierbar sein.
- **SYSTEM-DIALOG-003:** Dialoganforderungen MÜSSEN eindeutige Identitäten besitzen.
- **SYSTEM-DIALOG-004:** Dialoglogik und Darstellung MÜSSEN getrennt sein.
- **SYSTEM-DIALOG-005:** Dialoge MÜSSEN asynchron angefordert werden können.
- **SYSTEM-DIALOG-006:** Modale und nicht modale Dialoge MÜSSEN unterstützt werden.
- **SYSTEM-DIALOG-007:** Fensterzuordnung und Fokus MÜSSEN über den Window Manager erfolgen.
- **SYSTEM-DIALOG-008:** Dialogergebnisse MÜSSEN strukturiert und typisiert sein.
- **SYSTEM-DIALOG-009:** Sicherheitskritische Dialoge MÜSSEN über geschützte Systemkomponenten bereitgestellt werden.
- **SYSTEM-DIALOG-010:** Anwendungen, Solutions und Capabilities MÜSSEN standardisierte Dialoge verwenden können.
- **SYSTEM-DIALOG-011:** Dialoge MÜSSEN das native Nova UI Framework verwenden können.
- **SYSTEM-DIALOG-012:** Fehlerhafte Dialoganbieter DÜRFEN die Desktop Shell nicht beeinträchtigen.
- **SYSTEM-DIALOG-013:** Die Darstellung MUSS Damage Tracking unterstützen.
- **SYSTEM-DIALOG-014:** Dialoge MÜSSEN barrierefrei bedienbar sein.
- **SYSTEM-DIALOG-015:** Nova System Dialog Integration MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Desktop Shell Architecture:** Einbindung in die Desktopumgebung.
- **Nova Window Manager:** Fensterzuordnung, Modalität und Fokus.
- **Nova UI Framework:** Darstellung und Benutzerinteraktion.
- **Nova UI Data Binding:** Dialogdaten und Validierung.
- **Nova UI Animation Engine:** Übergänge und Animationen.
- **Nova Compositor:** Darstellung und Damage Tracking.
- **Nova Capability System:** Dialoganforderungen und Aktionen.
- **Nova Application Management:** Zuordnung aufrufender Anwendungen.
- **Nova Solutions:** Verwendung standardisierter Dialoge.
- **Nova VFS:** Datei- und Ordnerdialoge.
- **Nova Authentication:** Geschützte Authentifizierungsdialoge.
- **Nova.Trust:** Identität, Berechtigungen und Sicherheit.
- **Nova Shell Recovery:** Wiederherstellung und Rückfallmechanismen.

## 15. Ergebnis

NovaOS erhält eine zentrale, sichere und erweiterbare Infrastruktur zur Integration standardisierter Systemdialoge.

Anwendungen, Solutions und Capabilities können einheitliche Dialoge anfordern, ohne deren Darstellung oder systemnahe Funktionalität selbst implementieren zu müssen.

Durch die Trennung von Dialoglogik, Darstellung und Sicherheitsentscheidungen bleiben Systemdialoge konsistent, austauschbar und zuverlässig.

**Grundsatz:** NovaOS stellt einheitliche Dialoge bereit, damit der Benutzer vertraute Bedienabläufe vorfindet – unabhängig davon, welche Anwendung oder Capability eine Entscheidung benötigt.