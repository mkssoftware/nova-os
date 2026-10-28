# NPSPEC-SHELL-SYSTEM-0002 – Nova System Status Indicators

**Status:** Angenommen  
**Bereich:** Shell / System / Status Indicators  
**System:** NovaOS

## 1. Zweck

Nova System Status Indicators definiert die einheitliche Bereitstellung und Darstellung von Systemzuständen innerhalb der NovaOS Desktop Shell.

Statusindikatoren informieren den Benutzer über relevante Systemzustände, ohne unnötige Aufmerksamkeit zu beanspruchen.

Das Subsystem MUSS unabhängig von der konkreten Darstellung funktionieren und Statusinformationen für Taskleiste, System Tray, Quick Settings und weitere Oberflächen bereitstellen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Status Indicator Manager | Zentrale Verwaltung der Statusindikatoren |
| Status Indicator Registry | Registrierung verfügbarer Indikatoren |
| Status Provider Interface | Einheitliche Schnittstelle zu Systemdiensten |
| Status State Manager | Verwaltung aktueller Zustände |
| Status Event Dispatcher | Verteilung von Statusänderungen |
| Status Presentation Adapter | Bereitstellung darstellungsunabhängiger Statusinformationen |

Dabei gilt:

- Die Statusverwaltung MUSS unabhängig von der Desktop Shell arbeiten können.
- Systemzustände MÜSSEN von den zuständigen Systemdiensten stammen.
- Das Subsystem DARF keine eigene Hardware- oder Geräteverwaltung implementieren.
- Mehrere Oberflächen MÜSSEN dieselben Statusinformationen verwenden können.

## 3. Unterstützte Statusindikatoren

NovaOS unterstützt unter anderem:

- Netzwerkverbindung und Signalqualität.
- WLAN und Bluetooth.
- Lautstärke und Audiogeräte.
- Akku, Ladezustand und Stromversorgung.
- Energiesparmodus.
- Datum und Uhrzeit.
- Sicherheits- und Systemzustand.
- Angeschlossene Geräte.
- Hintergrundaktivitäten.
- Do Not Disturb.
- Updates und Wartungsvorgänge.

Dabei gilt:

- Indikatoren MÜSSEN dynamisch registrierbar sein.
- Nicht verfügbare Funktionen MÜSSEN entsprechend gekennzeichnet werden.
- Neue Statusindikatoren MÜSSEN über Capabilities integrierbar sein.
- Nicht benötigte Indikatoren SOLLEN ausgeblendet werden können.

## 4. Statusmodell

Ein Statusindikator besitzt mindestens:

- Eindeutige ID.
- Absenderidentität.
- Anzeigename.
- Statuskategorie.
- Aktuellen Zustand.
- Symbolreferenz.
- Zeitstempel der letzten Aktualisierung.

Optional können enthalten sein:

- Sekundärer Status.
- Numerischer Wert.
- Einheit.
- Beschreibung.
- Verfügbare Aktionen.
- Priorität.
- Fortschrittsinformationen.

Statusinformationen MÜSSEN strukturiert und unabhängig von ihrer visuellen Darstellung bereitgestellt werden.

## 5. Statuszustände

Folgende grundlegende Zustände werden unterstützt:

| Zustand | Bedeutung |
|---|---|
| Normal | Regulärer Betriebszustand |
| Active | Funktion ist aktiv |
| Busy | Vorgang wird ausgeführt |
| Warning | Aufmerksamkeit empfohlen |
| Error | Fehlerzustand |
| Disabled | Funktion deaktiviert |
| Unavailable | Funktion nicht verfügbar |
| Unknown | Zustand nicht zuverlässig bestimmbar |

Dabei gilt:

- Statuszustände MÜSSEN eindeutig definiert sein.
- Fachspezifische Zustände DÜRFEN das Grundmodell erweitern.
- Unbekannte Zustände DÜRFEN nicht als fehlerfreier Normalzustand dargestellt werden.
- Statusänderungen MÜSSEN nachvollziehbar sein.

## 6. Statusaktualisierung

Statusinformationen MÜSSEN ereignisgesteuert aktualisiert werden.

Dabei gilt:

- Systemdienste MÜSSEN Zustandsänderungen über definierte Schnittstellen melden können.
- Unveränderte Zustände DÜRFEN keine unnötigen UI-Aktualisierungen auslösen.
- Häufige Änderungen SOLLEN zusammengefasst werden.
- Veraltete Zustände MÜSSEN erkannt und entsprechend gekennzeichnet werden können.
- Regelmäßige Abfragen DÜRFEN nur verwendet werden, wenn keine geeignete Ereignisquelle verfügbar ist.
- Die Statusverwaltung DARF die UI Event Loop nicht blockieren.

## 7. Darstellung

Statusindikatoren können in unterschiedlichen Oberflächen dargestellt werden:

- System Tray.
- Taskleiste.
- Quick Settings.
- Notification Center.
- Systemeinstellungen.
- Benutzerdefinierte Solutions.

Dabei gilt:

- Die Darstellung MUSS das native Nova UI Framework verwenden.
- Symbole MÜSSEN das aktive Theme berücksichtigen.
- Zustände DÜRFEN nicht ausschließlich durch Farben vermittelt werden.
- Tooltips und ergänzende Informationen SOLLEN verfügbar sein.
- Selten benötigte Indikatoren SOLLEN dezent dargestellt werden.
- Die Darstellung MUSS Damage Tracking unterstützen.

## 8. Benutzerinteraktion

Statusindikatoren können optionale Aktionen bereitstellen.

Beispiele:

- Netzwerkstatus öffnen.
- Lautstärke ändern.
- Audiogerät auswählen.
- Energiemodus wechseln.
- Geräteinformationen anzeigen.
- Systemeinstellungen öffnen.

Dabei gilt:

- Aktionen MÜSSEN über autorisierte Commands oder Capabilities erfolgen.
- Statusindikatoren DÜRFEN keine eigene Systemsteuerungslogik implementieren.
- Nicht verfügbare Aktionen MÜSSEN deaktiviert werden.
- Sicherheitskritische Aktionen MÜSSEN erforderliche Berechtigungsprüfungen durchlaufen.
- Alle interaktiven Indikatoren MÜSSEN per Tastatur bedienbar sein.

## 9. Priorisierung und Aufmerksamkeit

NovaOS MUSS zwischen informativen und aufmerksamkeitsrelevanten Zuständen unterscheiden.

Dabei gilt:

- Normale Zustände SOLLEN ohne Animationen dargestellt werden.
- Warnungen MÜSSEN eindeutig erkennbar sein.
- Kritische Ereignisse MÜSSEN an das Nova Notification System übergeben werden können.
- Statusindikatoren DÜRFEN nicht eigenständig wiederholte Benachrichtigungen erzeugen.
- Blinkende oder dauerhaft animierte Indikatoren SOLLEN vermieden werden.
- Der Benutzer MUSS die Sichtbarkeit regulärer Indikatoren konfigurieren können.

## 10. Mehrmonitorbetrieb

Statusindikatoren MÜSSEN mehrere Bildschirme unterstützen.

Dabei gilt:

- Ein Systemzustand MUSS unabhängig von der Anzahl seiner Darstellungen verwaltet werden.
- Indikatoren DÜRFEN auf mehreren Taskleisten angezeigt werden.
- Mehrere Darstellungen MÜSSEN denselben aktuellen Zustand verwenden.
- Monitorspezifische Informationen MÜSSEN dem jeweiligen Ausgabegerät zugeordnet werden.
- Änderungen der Monitorkonfiguration MÜSSEN dynamisch berücksichtigt werden.

## 11. Zustand und Wiederherstellung

Der Status State Manager verwaltet aktuelle Statusinformationen und benutzerbezogene Einstellungen.

Dabei gilt:

- Statusinformationen MÜSSEN nach einem Shell-Neustart erneut bereitgestellt werden können.
- Tatsächliche Systemzustände MÜSSEN nach der Wiederherstellung mit den zuständigen Diensten abgeglichen werden.
- Gespeicherte Zustände DÜRFEN nicht ungeprüft als aktuell gelten.
- Benutzerdefinierte Sichtbarkeits- und Anordnungseinstellungen MÜSSEN erhalten bleiben.
- Fehlerhafte Statusanbieter DÜRFEN die Desktop Shell nicht beeinträchtigen.

## 12. Performance und Ressourcenverwaltung

Nova System Status Indicators MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Statusinformationen SOLLEN zwischen mehreren Oberflächen gemeinsam genutzt werden.
- Mehrfache identische Statusabfragen SOLLEN vermieden werden.
- Symbole und Metadaten SOLLEN zwischengespeichert werden.
- Nicht sichtbare Indikatoren DÜRFEN keine unnötigen Renderingoperationen verursachen.
- Häufige Statusänderungen MÜSSEN kontrolliert verarbeitet werden.
- Fehlerhafte oder nicht erreichbare Statusanbieter MÜSSEN isoliert behandelt werden können.

## 13. Sicherheit und Barrierefreiheit

Nova System Status Indicators unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Statusanbieter MÜSSEN eindeutig identifizierbar sein.
- Anwendungen DÜRFEN keine vertrauenswürdigen Systemindikatoren imitieren.
- Sicherheitsrelevante Statusinformationen MÜSSEN aus autorisierten Quellen stammen.
- Sensible Informationen DÜRFEN nicht ohne Berechtigung offengelegt werden.
- Statusaktionen MÜSSEN vor ihrer Ausführung autorisiert werden.
- Screenreader MÜSSEN Status und Zustandsänderungen erfassen können.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.
- Das Subsystem MUSS vollständig offline und ohne KI funktionieren.

## 14. Normative Anforderungen

- **SYSTEM-STATUS-001:** NovaOS MUSS eine zentrale Verwaltung von Systemstatusindikatoren bereitstellen.
- **SYSTEM-STATUS-002:** Statusanbieter MÜSSEN dynamisch registrierbar sein.
- **SYSTEM-STATUS-003:** Statusinformationen MÜSSEN von autorisierten Systemdiensten stammen.
- **SYSTEM-STATUS-004:** Statusindikatoren MÜSSEN eindeutige Identitäten besitzen.
- **SYSTEM-STATUS-005:** Statusänderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- **SYSTEM-STATUS-006:** Mehrere Oberflächen MÜSSEN dieselben Statusinformationen verwenden können.
- **SYSTEM-STATUS-007:** Nicht verfügbare und unbekannte Zustände MÜSSEN unterscheidbar sein.
- **SYSTEM-STATUS-008:** Statusaktionen MÜSSEN an zuständige Commands oder Capabilities delegiert werden.
- **SYSTEM-STATUS-009:** Statusindikatoren DÜRFEN keine eigene Hardwareverwaltung implementieren.
- **SYSTEM-STATUS-010:** Die Darstellung MUSS Damage Tracking unterstützen.
- **SYSTEM-STATUS-011:** Mehrmonitorbetrieb MUSS unterstützt werden.
- **SYSTEM-STATUS-012:** Statusinformationen MÜSSEN nach einem Shell-Neustart aktualisiert werden können.
- **SYSTEM-STATUS-013:** Fehlerhafte Statusanbieter DÜRFEN die Desktop Shell nicht beeinträchtigen.
- **SYSTEM-STATUS-014:** Sicherheitsrelevante Indikatoren MÜSSEN Nova.Trust berücksichtigen.
- **SYSTEM-STATUS-015:** Die Darstellung MUSS barrierefrei sein.
- **SYSTEM-STATUS-016:** Nova System Status Indicators MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Desktop Shell Architecture:** Einbindung in die Desktopumgebung.
- **Nova System Tray:** Darstellung kompakter Statusindikatoren.
- **Nova Quick Settings:** Statusanzeige und Schnellaktionen.
- **Nova Taskbar Architecture:** Integration in die Taskleiste.
- **Nova Notification Center:** Meldung relevanter Zustandsänderungen.
- **Nova UI Framework:** Darstellung und Benutzerinteraktion.
- **Nova Compositor:** Oberflächenkomposition und Damage Tracking.
- **Nova Display Server:** Anzeige- und Monitorstatus.
- **Nova Network Services:** Netzwerkstatus.
- **Nova Audio Services:** Audio- und Lautstärkestatus.
- **Nova Power Management:** Energie- und Akkustatus.
- **Nova Device Management:** Gerätestatus.
- **Nova Capability System:** Erweiterbare Statusanbieter und Aktionen.
- **Nova.Trust:** Identität, Berechtigungen und Sicherheit.

## 16. Ergebnis

NovaOS erhält eine zentrale, erweiterbare und ressourcenschonende Infrastruktur zur Bereitstellung und Darstellung von Systemzuständen.

Systemdienste und Capabilities können ihre Zustände einheitlich veröffentlichen. Taskleiste, System Tray, Quick Settings und weitere Oberflächen verwenden diese Informationen, ohne eigene Statusverwaltungen implementieren zu müssen.

**Grundsatz:** NovaOS zeigt dem Benutzer jederzeit den relevanten Systemzustand – zuverlässig, verständlich und so unauffällig wie möglich.