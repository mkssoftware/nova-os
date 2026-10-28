# NPSPEC-INPUT-ARCH-0004 – Nova Input Security

**Status:** Angenommen  
**Bereich:** Input / Architecture / Security  
**System:** NovaOS

## 1. Zweck

Nova Input Security definiert die Sicherheitsarchitektur für die Erfassung, Verarbeitung und Weiterleitung von Eingaben innerhalb von NovaOS.

Ziel ist der Schutz vor unberechtigtem Zugriff auf Eingabedaten, Eingabemanipulation, Keylogging und der Umgehung von Sicherheitsmechanismen.

Das Subsystem integriert sich in Nova.Trust und gewährleistet die Isolation von Anwendungen, Benutzersitzungen und Eingabequellen.

## 2. Architektur

Nova Input Security umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Input Security Manager | Zentrale Durchsetzung der Sicherheitsrichtlinien |
| Input Source Validator | Überprüfung von Eingabequellen |
| Input Access Controller | Kontrolle des Zugriffs auf Eingabedaten |
| Input Injection Controller | Absicherung künstlich erzeugter Eingaben |
| Secure Input Handler | Verarbeitung sicherheitskritischer Eingaben |
| Input Isolation Manager | Trennung von Anwendungen und Sitzungen |

Sicherheitsentscheidungen werden über Nova.Trust getroffen.

Die Input Event Pipeline MUSS die Sicherheitsrichtlinien vor der Zustellung von Ereignissen durchsetzen.

## 3. Vertrauensmodell

Jede Eingabequelle besitzt eine überprüfbare Herkunft.

Unterschieden werden:

- Physische Eingabegeräte.
- Virtuelle Eingabegeräte.
- Systeminterne Eingabequellen.
- Autorisierte Automatisierung.
- Entfernte Eingabequellen.

Die Herkunft eines Ereignisses ist unabhängig von dessen Berechtigungen.

Ein physisches Gerät gilt nicht automatisch als vertrauenswürdig.

Virtuelle Eingaben DÜRFEN nicht als physische Eingaben ausgegeben werden.

Die Vertrauensbewertung erfolgt anhand der Geräteidentität, Treiberherkunft, Sitzung und geltenden Sicherheitsrichtlinien.

## 4. Eingabeisolation

Eingaben werden grundsätzlich nur dem berechtigten Empfänger zugestellt.

Dabei gelten folgende Regeln:

- Anwendungen erhalten Eingaben ausschließlich innerhalb ihres zulässigen Eingabekontexts.
- Benutzersitzungen sind voneinander isoliert.
- Hintergrundanwendungen erhalten keine fremden Tastatureingaben.
- Globale Eingabeüberwachung erfordert eine ausdrückliche Capability.
- Eingabe-Capture ist an den genehmigten Kontext gebunden.
- Fokuswechsel MÜSSEN bei der Ereigniszustellung berücksichtigt werden.

Ein Fenster DARF den Eingabefokus nicht eigenmächtig übernehmen, wenn dadurch Sicherheitsrichtlinien verletzt werden.

## 5. Sichere Eingaben

NovaOS unterstützt einen geschützten Eingabemodus für sicherheitskritische Interaktionen.

Dazu gehören insbesondere:

- Benutzeranmeldung.
- Passworteingaben.
- Authentifizierungsdialoge.
- Berechtigungsbestätigungen.
- Sicherheitsrelevante Systemaktionen.

Während einer geschützten Eingabe:

- Werden Ereignisse ausschließlich an den autorisierten Empfänger zugestellt.
- Sind globale Eingabeüberwachung und unberechtigtes Capture ausgeschlossen.
- Sind nicht autorisierte Eingabeinjektionen gesperrt.
- Werden sensible Eingabeinhalte nicht in allgemeinen Diagnoseprotokollen gespeichert.
- MUSS der geschützte Eingabekontext durch vertrauenswürdige Systemkomponenten verwaltet werden.

Der geschützte Eingabemodus DARF nicht allein durch eine gewöhnliche Anwendung aktiviert oder vorgetäuscht werden.

## 6. Eingabeinjektion

NovaOS erlaubt kontrollierte Eingabeinjektion für berechtigte Funktionen.

Beispiele:

- Barrierefreiheit.
- Automatisierte Tests.
- Benutzerautorisierte Automatisierung.
- Virtuelle Eingabegeräte.

Für Eingabeinjektionen gilt:

- Die ausführende Komponente MUSS eine entsprechende Capability besitzen.
- Ziel und Umfang der Berechtigung MÜSSEN begrenzt sein.
- Injizierte Ereignisse MÜSSEN als künstlich erzeugt identifizierbar bleiben.
- Sicherheitskritische Eingabesequenzen DÜRFEN nicht durch unberechtigte Injektionen ausgelöst werden.
- Berechtigungen MÜSSEN widerrufbar sein.

Eine allgemeine Eingabeinjektionsberechtigung DARF keine implizite Berechtigung zur Steuerung geschützter Eingabekontexte enthalten.

## 7. Systemreservierte Eingaben

NovaOS unterstützt reservierte Eingabesequenzen für sicherheitskritische Systemfunktionen.

Diese werden vor der regulären Anwendungszustellung verarbeitet.

Anwendungen DÜRFEN solche Sequenzen nicht:

- Abfangen.
- Unterdrücken.
- Umdefinieren.
- Simulieren, um Sicherheitsprüfungen zu umgehen.

Die Erkennung erfolgt durch eine vertrauenswürdige Komponente der Input Event Pipeline.

Sicherheitskritische Aktionen MÜSSEN unabhängig vom aktuellen Anwendungsfokus erreichbar bleiben.

## 8. Geräte- und Treibersicherheit

Eingabetreiber und Gerätebeschreibungen werden als potenziell fehlerhafte Eingabequellen behandelt.

Festlegungen:

- Gerätedaten MÜSSEN validiert werden.
- Treiberzugriffe unterliegen den zugewiesenen Berechtigungen.
- Fehlerhafte Ereignisse DÜRFEN keine Speicherverletzungen verursachen.
- Geräte DÜRFEN keine beliebigen Systemereignisse erzeugen.
- Unbekannte oder nicht autorisierte Gerätefunktionen werden eingeschränkt.
- Treiber MÜSSEN entsprechend den Nova.Trust-Richtlinien geladen werden.

Die Geräteherkunft allein stellt keinen Nachweis der Vertrauenswürdigkeit dar.

## 9. Berechtigungen

Nova Input Security definiert mindestens folgende Capability-Kategorien:

| Capability | Berechtigung |
|---|---|
| Input.Receive | Empfang regulär zugestellter Eingaben |
| Input.DeviceAccess | Kontrollierter Zugriff auf ein bestimmtes Eingabegerät |
| Input.GlobalMonitor | Überwachung freigegebener globaler Eingabeereignisse |
| Input.Inject | Erzeugung künstlicher Eingaben innerhalb eines genehmigten Kontexts |
| Input.Capture | Exklusives Eingabe-Capture innerhalb eines zulässigen Kontexts |
| Input.Secure | Zugriff auf geschützte Eingabefunktionen für vertrauenswürdige Systemkomponenten |

Berechtigungen können auf Geräte, Sitzungen, Anwendungen und konkrete Eingabekontexte begrenzt werden.

Die Vergabe erfolgt über Nova.Trust.

## 10. Datenschutz und Diagnose

NovaOS verarbeitet Eingabedaten grundsätzlich lokal.

Dabei gilt:

- Tastatureingaben werden nicht standardmäßig protokolliert.
- Sensible Eingabeinhalte DÜRFEN nicht in allgemeinen Diagnoseaufzeichnungen erscheinen.
- Diagnosefunktionen verwenden bevorzugt aggregierte Ereignisstatistiken.
- Sicherheitsrelevante Zugriffe SOLLEN nachvollziehbar protokolliert werden.
- Protokolle DÜRFEN keine Passwörter oder vergleichbaren Geheimnisse enthalten.
- Netzwerkübertragungen von Eingabedaten erfolgen ausschließlich über ausdrücklich autorisierte Funktionen.

Die Sicherheitsüberwachung MUSS ohne Cloud- oder KI-Abhängigkeit funktionieren.

## 11. Fehlerbehandlung

Bei Sicherheitsverletzungen oder ungültigen Eingaben gilt:

- Nicht autorisierte Ereignisse werden zurückgewiesen.
- Fehlgeschlagene Berechtigungsprüfungen führen nicht zur Ereigniszustellung.
- Ungültige Geräteinformationen werden kontrolliert behandelt.
- Widerrufene Berechtigungen verhindern weitere geschützte Zugriffe.
- Fehler in Anwendungen DÜRFEN die Eingabeisolation nicht aufheben.
- Bei unbekanntem Sicherheitszustand wird der Zugriff verweigert.

Die Wiederherstellung eines sicheren Eingabezustands hat Vorrang vor der Fortsetzung einer fehlerhaften Eingabeverarbeitung.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Sicherheitskontrolle für Eingabeereignisse bereitstellen.
- **INPUT-002:** Eingabeereignisse MÜSSEN einer überprüfbaren Quelle zugeordnet werden können.
- **INPUT-003:** Anwendungen DÜRFEN keine fremden Eingaben ohne entsprechende Berechtigung empfangen.
- **INPUT-004:** Benutzersitzungen MÜSSEN hinsichtlich ihrer Eingaben isoliert sein.
- **INPUT-005:** Globale Eingabeüberwachung MUSS durch Nova.Trust kontrolliert werden.
- **INPUT-006:** Eingabeinjektion MUSS eine ausdrücklich gewährte Capability voraussetzen.
- **INPUT-007:** Virtuelle und injizierte Eingaben MÜSSEN als solche identifizierbar sein.
- **INPUT-008:** Sicherheitskritische Eingabesequenzen MÜSSEN vor der regulären Anwendungszustellung behandelt werden.
- **INPUT-009:** Geschützte Eingabekontexte MÜSSEN gegen unberechtigte Überwachung und Injektion abgesichert sein.
- **INPUT-010:** Eingabeberechtigungen MÜSSEN widerrufbar sein.
- **INPUT-011:** Geräte- und Treiberdaten MÜSSEN vor ihrer Verwendung validiert werden.
- **INPUT-012:** Sensible Eingabeinhalte DÜRFEN nicht in allgemeinen Diagnoseprotokollen gespeichert werden.
- **INPUT-013:** Sicherheitsprüfungen DÜRFEN nicht durch Fokuswechsel oder virtuelle Geräte umgangen werden.
- **INPUT-014:** Bei fehlender Berechtigung MUSS der Zugriff verweigert werden.
- **INPUT-015:** Die Sicherheitsarchitektur MUSS unabhängig von Nova UI funktionieren.
- **INPUT-016:** Nova Input Security MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova.Trust:** Identität, Berechtigungen, Capabilities und Audit.
- **Nova Input Architecture:** Zentrale Sicherheitsrichtlinien.
- **Nova Input Device Abstraction:** Geräteidentität und Eingabequellen.
- **Nova Input Event Pipeline:** Sicherheitsprüfung und Ereigniszustellung.
- **Nova Input Focus Manager:** Geschützte Fokus- und Eingabekontexte.
- **Nova Window Manager:** Fensterzuordnung und Sitzungsisolation.
- **Nova UI:** Sichere Eingabefelder und Authentifizierungsdialoge.
- **Nova System Diagnostics:** Sicherheitsereignisse und Fehlerdiagnose.

## 14. Ergebnis

NovaOS erhält eine zentrale Sicherheitsarchitektur für sämtliche Eingabequellen und Eingabeereignisse.

Die Trennung von Geräteherkunft, Berechtigungen, Eingabefokus und geschützten Eingabekontexten verhindert unberechtigte Eingabeüberwachung und Manipulation.

Gleichzeitig bleiben autorisierte Automatisierung, Barrierefreiheit und virtuelle Eingabegeräte möglich.

**Grundsatz:** Eingaben gehören dem Benutzer und dürfen ausschließlich von den Komponenten verarbeitet werden, denen er beziehungsweise das System ausdrücklich vertraut.