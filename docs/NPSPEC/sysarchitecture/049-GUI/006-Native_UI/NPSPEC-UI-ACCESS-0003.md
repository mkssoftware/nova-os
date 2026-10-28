# NPSPEC-UI-ACCESS-0003 – Nova Screen Reader Interface

**Status:** Angenommen  
**Bereich:** UI / Accessibility / Screen Reader  
**System:** NovaOS

## 1. Zweck

Nova Screen Reader Interface definiert die native Schnittstelle zwischen der NovaOS-Benutzeroberfläche und Screenreadern.

Ziel ist die zuverlässige, strukturierte und ereignisgesteuerte Bereitstellung von UI-Informationen für deren sprachliche oder taktile Ausgabe.

Die Schnittstelle ist unabhängig vom verwendeten Screenreader, Sprachsynthesesystem und Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Screen Reader Interface | Einheitliche Schnittstelle für Screenreader |
| Accessibility Tree Adapter | Zugriff auf semantische UI-Informationen |
| Screen Reader Event Dispatcher | Übermittlung relevanter UI-Ereignisse |
| Navigation Interface | Semantische Navigation innerhalb der UI |
| Text Access Interface | Zugriff auf Text und Textpositionen |
| Action Interface | Ausführung autorisierter Bedienaktionen |
| Announcement Manager | Verwaltung von Statusmeldungen |

Das Interface verwendet den Nova Accessibility Tree und die Accessibility Provider der Controls.

## 3. Semantische Informationen

Screenreader können folgende Informationen abfragen:

- Elementname und Beschreibung.
- Semantische Rolle.
- Aktueller Zustand.
- Wert und Wertebereich.
- Fokus- und Auswahlstatus.
- Verfügbare Aktionen.
- Hierarchische Beziehungen.
- Bedienungshinweise.

Die Informationen MÜSSEN dem tatsächlichen UI-Zustand entsprechen.

Screenreader DÜRFEN nicht auf die Analyse gerenderter Pixel angewiesen sein.

## 4. Navigation

Das Interface unterstützt die strukturierte Navigation innerhalb des Accessibility Tree.

Dazu gehören:

- Navigation zwischen Eltern- und Kindelementen.
- Navigation zwischen Geschwisterelementen.
- Navigation nach semantischen Rollen.
- Navigation zwischen fokussierbaren Elementen.
- Navigation innerhalb von Listen, Tabellen und Baumstrukturen.
- Ermittlung des aktuell fokussierten Elements.

Die semantische Navigation MUSS unabhängig von der visuellen Position funktionieren.

## 5. Textzugriff

Textbasierte Controls stellen standardisierte Textinformationen bereit.

Unterstützt werden:

- Lesen vollständiger Texte.
- Lesen definierter Textbereiche.
- Zeichen-, Wort- und Zeilennavigation.
- Ermittlung der Cursorposition.
- Ermittlung markierter Textbereiche.
- Benachrichtigung über Textänderungen.

Dabei gilt:

- Textpositionen MÜSSEN eindeutig definiert sein.
- Unicode-Text MUSS korrekt verarbeitet werden.
- Geschützte Eingabefelder DÜRFEN keine vertraulichen Inhalte offenlegen.
- Schreiboperationen MÜSSEN über autorisierte Control-Aktionen erfolgen.

## 6. Ereignisgesteuerte Benachrichtigungen

Screenreader erhalten relevante Änderungen über Accessibility Events.

Beispiele:

- Fokuswechsel.
- Auswahländerungen.
- Wertänderungen.
- Textänderungen.
- Zustandsänderungen.
- Öffnen und Schließen von Dialogen.
- Neue Status- und Fehlermeldungen.

Dabei gilt:

- Ereignisse MÜSSEN in nachvollziehbarer Reihenfolge übermittelt werden.
- Unveränderte Zustände DÜRFEN keine unnötigen Meldungen erzeugen.
- Häufige Änderungen SOLLEN zusammengefasst werden.
- Wichtige Meldungen DÜRFEN nicht durch unwichtige Ereignisse verdrängt werden.

## 7. Statusmeldungen und Prioritäten

NovaOS unterstützt die Ausgabe von Statusmeldungen ohne Fokuswechsel.

Dafür werden folgende Prioritäten definiert:

| Priorität | Verhalten |
|---|---|
| Polite | Ausgabe bei geeigneter Gelegenheit |
| Assertive | Bevorzugte zeitnahe Ausgabe |
| Critical | Sicherheits- oder funktionskritische Meldung |

Die Priorität beschreibt die Dringlichkeit einer Meldung, nicht die Berechtigung des auslösenden Elements.

Wiederholte identische Meldungen SOLLEN zusammengefasst werden.

## 8. Bedienaktionen

Screenreader können unterstützte UI-Aktionen anfordern.

Beispiele:

- Button auslösen.
- Auswahl verändern.
- Wert setzen.
- Element erweitern oder reduzieren.
- Scrollen.
- Fokus anfordern.

Dabei gilt:

- Aktionen MÜSSEN über die regulären Accessibility Provider erfolgen.
- Die bestehende Command- und Event-Infrastruktur wird verwendet.
- Aktionen MÜSSEN die jeweiligen Berechtigungen berücksichtigen.
- Das Interface DARF keine privilegierten Eingaben oder Commands vortäuschen.

## 9. Sprachausgabe und Braille

Das Screen Reader Interface stellt semantische Informationen bereit, übernimmt jedoch nicht selbst die Sprachsynthese.

Die Ausgabe kann durch externe oder native Komponenten erfolgen:

- Text-to-Speech.
- Braillezeilen.
- Andere taktile Ausgabegeräte.

Dabei gilt:

- Ausgabeformate DÜRFEN nicht fest an einen Sprachsyntheseanbieter gebunden sein.
- Sprachinformationen und Textstruktur SOLLEN bereitgestellt werden.
- Das Interface MUSS ohne Cloudverbindung funktionieren.
- Konkrete Ausgabegeräte werden über eigenständige Systemkomponenten angebunden.

## 10. Virtualisierte Inhalte

Screenreader MÜSSEN virtualisierte Listen, Tabellen und andere große Datenstrukturen bedienen können.

Dabei gilt:

- Nicht sichtbare Elemente MÜSSEN nicht vollständig als UI-Objekte existieren.
- Relevante Inhalte MÜSSEN über Accessibility Provider erreichbar bleiben.
- Navigation DARF nicht unnötig sämtliche Datensätze laden.
- Virtuelle Elemente MÜSSEN stabil identifizierbar sein.
- Änderungen der Datenstruktur MÜSSEN kontrolliert gemeldet werden.

## 11. Threading und Performance

Das Interface arbeitet ereignisgesteuert und asynchron, soweit dies erforderlich ist.

Dabei gilt:

- Screenreader-Abfragen DÜRFEN die UI Event Loop nicht dauerhaft blockieren.
- Abfragen MÜSSEN konsistente Accessibility-Daten liefern.
- Änderungsbenachrichtigungen SOLLEN inkrementell erfolgen.
- Nicht mehr benötigte Abonnements MÜSSEN freigegeben werden.
- Langsame Screenreader DÜRFEN die Darstellung der Benutzeroberfläche nicht blockieren.

## 12. Sicherheit und Datenschutz

Nova Screen Reader Interface unterliegt Nova.Trust.

Dabei gilt:

- Screenreader MÜSSEN für den Zugriff auf geschützte UI-Informationen autorisiert sein.
- Passwortfelder DÜRFEN keine Klartextwerte bereitstellen.
- Anwendungs- und Benutzergrenzen MÜSSEN eingehalten werden.
- Bedienaktionen MÜSSEN reguläre Berechtigungsprüfungen durchlaufen.
- Accessibility-Daten DÜRFEN nicht unautorisiert an andere Prozesse weitergegeben werden.
- Die Schnittstelle MUSS vollständig lokal betrieben werden können.

## 13. NovaStudio

NovaStudio unterstützt:

- Prüfung der Screenreader-Kompatibilität.
- Simulation semantischer Navigation.
- Anzeige zugänglicher Namen und Rollen.
- Analyse von Fokus- und Statusmeldungen.
- Prüfung der Textzugänglichkeit.
- Erkennung fehlender Accessibility Provider.

## 14. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine native Screenreader-Schnittstelle bereitstellen.
- **UI-002:** Die Schnittstelle MUSS den Nova Accessibility Tree verwenden.
- **UI-003:** Semantische Rollen, Namen und Zustände MÜSSEN abfragbar sein.
- **UI-004:** Strukturierte Navigation MUSS unterstützt werden.
- **UI-005:** Textzugriff und Textnavigation MÜSSEN unterstützt werden.
- **UI-006:** Geschützte Eingaben DÜRFEN nicht offengelegt werden.
- **UI-007:** Relevante UI-Änderungen MÜSSEN ereignisgesteuert gemeldet werden.
- **UI-008:** Statusmeldungen MÜSSEN ohne Fokuswechsel möglich sein.
- **UI-009:** Unterschiedliche Meldungsprioritäten MÜSSEN unterstützt werden.
- **UI-010:** Screenreader MÜSSEN autorisierte Bedienaktionen auslösen können.
- **UI-011:** Virtualisierte Controls MÜSSEN zugänglich bleiben.
- **UI-012:** Screenreader-Abfragen DÜRFEN die UI Event Loop nicht dauerhaft blockieren.
- **UI-013:** Die Schnittstelle MUSS unabhängig von Sprachsynthese und Renderbackend funktionieren.
- **UI-014:** NovaStudio MUSS die Screenreader-Kompatibilität prüfen können.
- **UI-015:** Sämtliche Zugriffe MÜSSEN Nova.Trust berücksichtigen.
- **UI-016:** Nova Screen Reader Interface MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Accessibility Architecture:** Übergeordnete Barrierefreiheitsarchitektur.
- **Nova Accessibility Tree:** Semantische UI-Hierarchie.
- **Nova Accessibility Provider:** Eigenschaften und Bedienaktionen.
- **Nova UI Event System:** Ereignisbenachrichtigungen.
- **Nova UI Threading Model:** Synchronisierte Abfragen.
- **Nova Control Framework:** Zugänglichkeit nativer Controls.
- **Nova Virtualized Controls:** Virtuelle Inhalte.
- **Nova Commands and Actions:** Autorisierte Bedienaktionen.
- **Nova Input System:** Fokus und alternative Eingaben.
- **Nova Audio System:** Optionale Sprachausgabe.
- **NovaStudio:** Prüfung und Diagnose.
- **Nova.Trust:** Zugriffsschutz und Isolation.

## 16. Ergebnis

NovaOS erhält eine einheitliche, native und rendererunabhängige Schnittstelle für Screenreader.

Unterstützende Technologien können die Benutzeroberfläche semantisch erfassen, Texte lesen, Änderungen erkennen und autorisierte Bedienaktionen ausführen.

Die Schnittstelle ermöglicht sowohl sprachliche als auch taktile Ausgabe, ohne eine bestimmte Screenreader-Implementierung vorauszusetzen.

**Grundsatz:** Ein Screenreader muss die Bedeutung und Bedienbarkeit einer Benutzeroberfläche erkennen können, ohne ihre grafische Darstellung interpretieren zu müssen.