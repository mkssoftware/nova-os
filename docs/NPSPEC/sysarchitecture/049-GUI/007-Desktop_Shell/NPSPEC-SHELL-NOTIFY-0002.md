# NPSPEC-SHELL-NOTIFY-0002 – Nova Toast Notifications

**Status:** Angenommen  
**Bereich:** Shell / Notifications / Toast  
**System:** NovaOS

## 1. Zweck

Nova Toast Notifications definiert die kurzzeitige Einblendung von Benachrichtigungen auf dem NovaOS-Desktop.

Toast Notifications informieren über relevante Ereignisse, ohne den Arbeitsfluss unnötig zu unterbrechen.

Die Darstellung MUSS dezent, ressourcenschonend und vollständig durch den Benutzer kontrollierbar sein.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Toast Manager | Verwaltung aktiver Toasts |
| Toast Queue | Warteschlange eingehender Benachrichtigungen |
| Toast Policy Controller | Prüfung von Priorität und Darstellungsregeln |
| Toast Layout Manager | Positionierung und Anordnung |
| Toast View | Native Darstellung |
| Toast Animation Controller | Ein- und Ausblendungen |
| Toast Interaction Controller | Benutzerinteraktionen |

Toast Notifications sind eine Darstellungskomponente des Nova Notification Systems.

Sie DÜRFEN keine eigene unabhängige Benachrichtigungsverwaltung implementieren.

## 3. Toast-Modell

Ein Toast referenziert eine Benachrichtigung des Notification Managers.

Darstellbare Inhalte sind:

- Symbol des Absenders.
- Titel.
- Nachrichtentext.
- Zeitstempel.
- Optionale Fortschrittsanzeige.
- Optionale Aktionsschaltflächen.
- Schließen-Schaltfläche.

Dabei gilt:

- Jeder Toast MUSS einer registrierten Benachrichtigung zugeordnet sein.
- Inhalte MÜSSEN strukturiert übergeben werden.
- Lange Texte MÜSSEN begrenzt oder gekürzt werden können.
- Erweiterte Informationen SOLLEN im Notification Center verfügbar sein.
- Toasts DÜRFEN keine beliebigen ausführbaren UI-Inhalte enthalten.

## 4. Darstellung und Positionierung

Toasts werden als temporäre Oberflächenelemente über dem Desktop dargestellt.

Dabei gilt:

- Die Darstellung MUSS das native Nova UI Framework verwenden.
- Die Position MUSS konfigurierbar sein.
- Mehrere Toasts MÜSSEN geordnet dargestellt werden.
- Toasts DÜRFEN sich nicht gegenseitig überdecken.
- Bildschirmgrenzen und DPI-Skalierung MÜSSEN berücksichtigt werden.
- Vollbildanwendungen und aktive Arbeitsbereiche MÜSSEN berücksichtigt werden.
- Der Compositor übernimmt die visuelle Zusammensetzung.

Standardmäßig SOLLEN Toasts am Bildschirmrand erscheinen und keine zentrale Arbeitsfläche verdecken.

## 5. Lebenszyklus

Ein Toast durchläuft folgende Zustände:

1. **Queued:** Wartet auf Darstellung.
2. **Appearing:** Wird eingeblendet.
3. **Visible:** Ist sichtbar.
4. **Interacting:** Benutzer interagiert mit dem Toast.
5. **Dismissing:** Wird ausgeblendet.
6. **Closed:** Darstellung wurde beendet.

Dabei gilt:

- Zustandsübergänge MÜSSEN eindeutig definiert sein.
- Toasts MÜSSEN automatisch ausgeblendet werden können.
- Die Anzeigedauer MUSS konfigurierbar sein.
- Aktive Benutzerinteraktionen SOLLEN das automatische Ausblenden pausieren.
- Das Schließen eines Toasts DARF die zugehörige Benachrichtigung nicht automatisch aus dem Notification Center löschen.
- Ein Toast DARF keine Anwendung blockieren.

## 6. Priorisierung und Warteschlange

Der Toast Policy Controller entscheidet anhand der Notification Policies über die Darstellung.

Dabei gilt:

- Nicht jede Benachrichtigung MUSS einen Toast erzeugen.
- Niedrige Prioritäten SOLLEN standardmäßig ohne Toast zugestellt werden können.
- Die Anzahl gleichzeitig sichtbarer Toasts MUSS begrenzt sein.
- Weitere Toasts MÜSSEN kontrolliert in einer Warteschlange verarbeitet werden.
- Wiederholte Meldungen SOLLEN zusammengefasst werden.
- Kritische Benachrichtigungen MÜSSEN entsprechend ihrer Systemrichtlinien behandelt werden.
- Anwendungen DÜRFEN die Darstellungsregeln nicht eigenmächtig umgehen.

## 7. Benutzerinteraktion

Toasts unterstützen:

- Anklicken zum Öffnen.
- Ausführen angebotener Aktionen.
- Manuelles Schließen.
- Tastaturnavigation.
- Fokussteuerung.
- Optionales Aufrufen weiterer Informationen.

Dabei gilt:

- Aktionen MÜSSEN über den Notification Action Controller ausgeführt werden.
- Erforderliche Berechtigungen MÜSSEN geprüft werden.
- Toasts DÜRFEN den Tastaturfokus nicht ungefragt übernehmen.
- Eine Interaktion MUSS auch ohne Maus möglich sein.
- Kritische Bestätigungsdialoge DÜRFEN nicht ausschließlich als automatisch verschwindende Toasts umgesetzt werden.

## 8. Animationen und visuelle Effekte

Nova Toast Notifications unterstützt dezente Animationen.

Dazu gehören:

- Einblenden.
- Ausblenden.
- Verschieben.
- Größenanpassung.
- Aktualisierung bestehender Inhalte.

Dabei gilt:

- Animationen MÜSSEN über die Nova UI Animation Engine und den Compositor erfolgen.
- Animationen SOLLEN zeitlich kurz und zurückhaltend sein.
- Reduced Motion MUSS berücksichtigt werden.
- Bei deaktivierten Animationen MUSS die Funktion vollständig erhalten bleiben.
- Unveränderte Bildschirmbereiche DÜRFEN nicht neu gerendert werden.
- Damage Tracking MUSS verwendet werden.

## 9. Mehrmonitorbetrieb

Toasts MÜSSEN mehrere Bildschirme unterstützen.

Dabei gilt:

- Der Zielmonitor MUSS anhand definierter Regeln bestimmt werden.
- Standardmäßig SOLL der aktive oder konfigurierte Benachrichtigungsmonitor verwendet werden.
- Die Positionierung MUSS die jeweilige Arbeitsfläche berücksichtigen.
- Ein Toast DARF nicht unbeabsichtigt mehrfach auf verschiedenen Monitoren erscheinen.
- Änderungen der Monitorkonfiguration MÜSSEN während des Betriebs berücksichtigt werden.

## 10. Ruhemodus und Benutzerkontrolle

Die Darstellung unterliegt den Einstellungen des Nova Notification Centers.

Dabei gilt:

- Der Ruhemodus MUSS berücksichtigt werden.
- Anwendungsspezifische Toast-Einstellungen MÜSSEN unterstützt werden.
- Der Benutzer MUSS Toasts vollständig deaktivieren können, soweit keine zwingenden Sicherheitsanforderungen entgegenstehen.
- Unterdrückte Toasts SOLLEN als Benachrichtigungen im Notification Center erhalten bleiben.
- Automatische Anpassungen MÜSSEN nachvollziehbar und abschaltbar sein.

## 11. Performance und Fehlerbehandlung

Toast Notifications MÜSSEN ereignisgesteuert arbeiten.

Dabei gilt:

- Die Darstellung DARF die UI Event Loop nicht blockieren.
- Symbole und Layoutressourcen SOLLEN wiederverwendet werden.
- Unsichtbare Toasts DÜRFEN keine Renderingaktivität verursachen.
- Häufige Aktualisierungen SOLLEN zusammengefasst werden.
- Fehlerhafte Toast-Inhalte DÜRFEN die Desktop Shell nicht beeinträchtigen.
- Bei Ressourcenmangel MÜSSEN Animationen und visuelle Effekte reduziert werden können.
- Ein Neustart der Shell DARF gespeicherte Benachrichtigungen nicht verlieren.

## 12. Sicherheit und Barrierefreiheit

Nova Toast Notifications unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Absenderidentitäten MÜSSEN überprüfbar sein.
- Anwendungen DÜRFEN keine Systembenachrichtigungen imitieren.
- Sensible Inhalte MÜSSEN entsprechend den Datenschutzrichtlinien ausgeblendet werden können.
- Toast-Aktionen MÜSSEN autorisiert werden.
- Screenreader MÜSSEN relevante Inhalte erfassen können.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Automatisches Ausblenden MUSS die Anforderungen an barrierefreie Bedienung berücksichtigen.

## 13. Normative Anforderungen

- **NOTIFY-TOAST-001:** NovaOS MUSS temporäre Desktop-Benachrichtigungen unterstützen.
- **NOTIFY-TOAST-002:** Toasts MÜSSEN über den zentralen Notification Manager verwaltet werden.
- **NOTIFY-TOAST-003:** Die Darstellung MUSS das native Nova UI Framework verwenden.
- **NOTIFY-TOAST-004:** Position und Anzeigedauer MÜSSEN konfigurierbar sein.
- **NOTIFY-TOAST-005:** Die Anzahl gleichzeitig sichtbarer Toasts MUSS begrenzt werden.
- **NOTIFY-TOAST-006:** Toasts MÜSSEN automatisch und manuell geschlossen werden können.
- **NOTIFY-TOAST-007:** Das Schließen eines Toasts DARF den Benachrichtigungsverlauf nicht automatisch löschen.
- **NOTIFY-TOAST-008:** Ruhemodus und Prioritätsregeln MÜSSEN berücksichtigt werden.
- **NOTIFY-TOAST-009:** Toasts DÜRFEN den Tastaturfokus nicht ungefragt übernehmen.
- **NOTIFY-TOAST-010:** Animationen MÜSSEN deaktivierbar sein.
- **NOTIFY-TOAST-011:** Damage Tracking MUSS unterstützt werden.
- **NOTIFY-TOAST-012:** Mehrmonitorbetrieb MUSS unterstützt werden.
- **NOTIFY-TOAST-013:** Toast-Aktionen MÜSSEN autorisiert werden.
- **NOTIFY-TOAST-014:** Die Darstellung MUSS barrierefrei sein.
- **NOTIFY-TOAST-015:** Nova Toast Notifications MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Notification Center:** Benachrichtigungsverwaltung und Verlauf.
- **Nova Notification Policy Manager:** Prioritäten und Unterbrechungsregeln.
- **Nova Notification Action Controller:** Ausführung von Aktionen.
- **Nova Desktop Shell:** Integration in die Desktopumgebung.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova UI Animation Engine:** Animationen und Übergänge.
- **Nova Compositor:** Oberflächenkomposition und Damage Tracking.
- **Nova Window Manager:** Arbeitsflächen und Monitorzuordnung.
- **Nova Display Server:** Bildschirmkonfiguration.
- **Nova Accessibility:** Barrierefreie Bedienung.
- **Nova.Trust:** Sicherheit und Datenschutz.

## 15. Ergebnis

NovaOS erhält ein einheitliches System für kurzzeitige Desktop-Benachrichtigungen.

Toasts informieren über relevante Ereignisse, ermöglichen schnelle Aktionen und verschwinden anschließend wieder, ohne den Arbeitsfluss dauerhaft zu unterbrechen.

Die eigentliche Benachrichtigung bleibt unabhängig von ihrer temporären Darstellung verwaltet.

**Grundsatz:** Ein Toast macht auf etwas aufmerksam, ohne Aufmerksamkeit einzufordern, die das Ereignis nicht verdient.