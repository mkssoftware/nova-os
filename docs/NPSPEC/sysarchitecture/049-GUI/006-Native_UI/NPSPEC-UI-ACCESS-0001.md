# NPSPEC-UI-ACCESS-0001 – Nova Accessibility Architecture

**Status:** Angenommen  
**Bereich:** UI / Accessibility  
**System:** NovaOS

## 1. Zweck

Nova Accessibility Architecture definiert die grundlegende Architektur für eine barrierefreie Bedienung von NovaOS.

Ziel ist eine Benutzeroberfläche, die unabhängig von individuellen körperlichen, sensorischen oder kognitiven Fähigkeiten vollständig bedienbar bleibt.

Barrierefreiheit ist ein integraler Bestandteil des nativen Nova UI Frameworks und keine nachträglich ergänzte Funktion.

## 2. Architektur

Das Accessibility-System umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Accessibility Manager | Zentrale Verwaltung der Barrierefreiheit |
| Accessibility Tree | Semantische Darstellung der UI-Hierarchie |
| Accessibility Provider | Bereitstellung von Informationen und Aktionen |
| Accessibility Event Manager | Benachrichtigung über relevante Änderungen |
| Accessibility Settings | Verwaltung individueller Einstellungen |
| Assistive Technology Interface | Schnittstelle für unterstützende Technologien |
| Accessibility Policy Manager | Durchsetzung systemweiter Einstellungen |

Die Architektur MUSS unabhängig vom verwendeten UI-Renderer funktionieren.

## 3. Accessibility Tree

NovaOS stellt eine semantische Hierarchie der Benutzeroberfläche bereit.

Jedes relevante UI-Element kann folgende Informationen besitzen:

- Rolle und Elementtyp.
- Zugänglichen Namen.
- Beschreibung.
- Aktuellen Zustand.
- Verfügbare Aktionen.
- Beziehungen zu anderen Elementen.
- Position und sichtbaren Bereich.
- Fokus- und Auswahlstatus.

Dabei gilt:

- Der Accessibility Tree MUSS aus dem Nova UI Object Model abgeleitet werden.
- Rein dekorative Elemente DÜRFEN ausgeblendet werden.
- Informationen MÜSSEN mit dem tatsächlichen UI-Zustand übereinstimmen.
- Virtualisierte Controls MÜSSEN ihre Inhalte zugänglich machen, ohne sämtliche Elemente dauerhaft erzeugen zu müssen.

## 4. Accessibility Properties

Das Nova UI Property System stellt standardisierte Accessibility-Eigenschaften bereit.

Beispiele:

| Eigenschaft | Bedeutung |
|---|---|
| AccessibleName | Zugänglicher Elementname |
| AccessibleDescription | Ergänzende Beschreibung |
| AccessibleRole | Semantische Rolle |
| AccessibleHelpText | Bedienungshinweise |
| AccessibleValue | Aktueller Wert |
| AccessibleState | Aktueller Zustand |
| AccessibleLabelledBy | Verknüpfte Beschriftung |
| AccessibleHidden | Ausschluss rein dekorativer Elemente |

Eigenschaften MÜSSEN deklarativ und programmatisch gesetzt werden können.

Fehlende Informationen SOLLEN aus geeigneten Control-Eigenschaften abgeleitet werden.

## 5. Accessibility Provider

Controls stellen ihre Funktionen über standardisierte Accessibility Provider bereit.

Unterstützte Funktionsmuster umfassen:

- Invoke für ausführbare Aktionen.
- Value für lesbare und veränderbare Werte.
- Range für Wertebereiche.
- Selection für auswählbare Elemente.
- ExpandCollapse für aufklappbare Inhalte.
- Scroll für scrollbare Bereiche.
- Text für Textinhalte und Textnavigation.

Custom Controls MÜSSEN passende Accessibility Provider implementieren können.

Die Ausführung von Aktionen erfolgt über die reguläre UI- und Command-Infrastruktur.

## 6. Tastaturbedienung und Fokus

NovaOS MUSS vollständig ohne Maus bedienbar sein.

Dabei gilt:

- Interaktive Controls MÜSSEN über die Tastatur erreichbar sein.
- Die Fokusreihenfolge MUSS logisch und nachvollziehbar sein.
- Der aktuelle Fokus MUSS erkennbar bleiben.
- Tastaturkürzel und alternative Bedienwege SOLLEN unterstützt werden.
- Fokusänderungen MÜSSEN über Accessibility Events gemeldet werden.
- Modale Dialoge MÜSSEN eine kontrollierte Fokusführung besitzen.

Die Fokusverwaltung erfolgt in Zusammenarbeit mit dem Nova UI Event System.

## 7. Visuelle Barrierefreiheit

NovaOS unterstützt systemweite Anpassungen der Darstellung.

Dazu gehören:

- Hoher Kontrast.
- Skalierbare Schriftgrößen.
- Anpassbare UI-Skalierung.
- Gut erkennbare Fokusindikatoren.
- Reduzierte Transparenz.
- Reduzierte Animationen.
- Anpassbare Farben und Kontraste.

Dabei gilt:

- Informationen DÜRFEN nicht ausschließlich durch Farben vermittelt werden.
- Texte und Bedienelemente MÜSSEN auch bei vergrößerter Darstellung nutzbar bleiben.
- Layouts MÜSSEN auf veränderte Skalierungen reagieren.
- Systemweite Barrierefreiheitseinstellungen besitzen Vorrang vor rein dekorativen Styles.

## 8. Unterstützende Technologien

NovaOS stellt eine einheitliche Schnittstelle für unterstützende Technologien bereit.

Dazu gehören insbesondere:

- Screenreader.
- Bildschirmlupen.
- Bildschirmtastaturen.
- Alternative Eingabegeräte.
- Sprachsteuerung.
- Schalterbasierte Bedienhilfen.

Die Schnittstelle MUSS semantische UI-Informationen und autorisierte Bedienaktionen bereitstellen.

Unterstützende Technologien DÜRFEN nicht vom verwendeten Renderbackend abhängig sein.

Die Accessibility-Architektur definiert die Schnittstellen; konkrete Hilfsprogramme werden eigenständig implementiert.

## 9. Accessibility Events

Relevante Änderungen werden ereignisgesteuert gemeldet.

Beispiele:

- Fokuswechsel.
- Wertänderungen.
- Auswahländerungen.
- Zustandsänderungen.
- Änderungen der UI-Struktur.
- Neue Statusmeldungen.
- Änderungen zugänglicher Beschriftungen.

Dabei gilt:

- Ereignisse MÜSSEN den tatsächlichen UI-Zustand widerspiegeln.
- Unveränderte Zustände DÜRFEN keine unnötigen Meldungen erzeugen.
- Häufige Änderungen SOLLEN kontrolliert zusammengefasst werden.
- Zeitkritische Informationen MÜSSEN zuverlässig übermittelt werden.

## 10. Deklarative UI und Custom Controls

Accessibility-Informationen können direkt im Nova Declarative UI Model definiert werden.

Konzeptionelles Beispiel:

```xml
<Button
    Content="Speichern"
    AccessibleName="Dokument speichern"
    AccessibleDescription="Speichert das aktuelle Dokument" />
```

Dabei gilt:

- Standard Controls MÜSSEN geeignete Accessibility-Informationen bereitstellen.
- Control Templates DÜRFEN die semantische Bedeutung eines Controls nicht unbeabsichtigt verändern.
- Custom Controls MÜSSEN ihre Rolle, Zustände und Aktionen definieren können.
- Data Templates MÜSSEN zugängliche Datenrepräsentationen ermöglichen.

## 11. Einstellungen und Benutzerkontrolle

Accessibility-Einstellungen werden zentral durch NovaOS verwaltet.

Dabei gilt:

- Einstellungen MÜSSEN dauerhaft gespeichert werden können.
- Änderungen SOLLEN ohne Neustart wirksam werden.
- Anwendungen MÜSSEN die wirksamen Systemeinstellungen berücksichtigen.
- Benutzer können Einstellungen jederzeit ändern oder zurücksetzen.
- NovaOS DARF unterstützende Anpassungen vorschlagen, jedoch nicht ungefragt dauerhaft aktivieren.

Die Benutzerkontrolle besitzt Vorrang vor automatischen Anpassungen.

## 12. Performance und Ressourcen

Die Accessibility-Architektur arbeitet bedarfsgesteuert.

Dabei gilt:

- Der Accessibility Tree SOLL inkrementell aktualisiert werden.
- Unveränderte UI-Elemente SOLLEN ihre semantischen Informationen behalten.
- Accessibility-Abfragen DÜRFEN keine vollständige Neuberechnung der Benutzeroberfläche erzwingen.
- Virtualisierte Inhalte MÜSSEN ressourcenschonend zugänglich bleiben.
- Unterstützende Technologien DÜRFEN die UI Event Loop nicht dauerhaft blockieren.

## 13. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Bearbeitung von Accessibility-Eigenschaften.
- Prüfung fehlender Beschriftungen.
- Analyse der Fokusreihenfolge.
- Vorschau verschiedener Kontrast- und Skalierungseinstellungen.
- Simulation reduzierter Animationen.
- Darstellung des Accessibility Tree.
- Prüfung von Custom Controls.
- Diagnose von Accessibility Events.

Logic Graph MUSS Accessibility-Aktionen über dieselben autorisierten Commands und UI-Schnittstellen verarbeiten können.

## 14. Sicherheit und Datenschutz

Nova Accessibility Architecture unterliegt Nova.Trust.

Dabei gilt:

- Unterstützende Technologien MÜSSEN innerhalb eines autorisierten Sicherheitskontexts arbeiten.
- Geschützte Inhalte DÜRFEN nicht unautorisiert offengelegt werden.
- Accessibility-Aktionen DÜRFEN keine Berechtigungsprüfungen umgehen.
- Passwort- und andere geschützte Eingabefelder MÜSSEN ihre Vertraulichkeit bewahren.
- Accessibility Provider DÜRFEN keine zusätzlichen Capabilities verleihen.
- Die grundlegende Barrierefreiheit MUSS vollständig offline funktionieren.

## 15. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine zentrale Accessibility-Architektur bereitstellen.
- **UI-002:** Relevante UI-Elemente MÜSSEN semantisch beschrieben werden können.
- **UI-003:** Ein Accessibility Tree MUSS bereitgestellt werden.
- **UI-004:** Standard Controls MÜSSEN geeignete Accessibility Provider besitzen.
- **UI-005:** Custom Controls MÜSSEN Accessibility Provider implementieren können.
- **UI-006:** Die Benutzeroberfläche MUSS vollständig per Tastatur bedienbar sein.
- **UI-007:** Fokus und Fokusreihenfolge MÜSSEN nachvollziehbar sein.
- **UI-008:** Accessibility Events MÜSSEN relevante Zustandsänderungen melden.
- **UI-009:** Screenreader und andere unterstützende Technologien MÜSSEN über standardisierte Schnittstellen angebunden werden können.
- **UI-010:** Hoher Kontrast und skalierbare Darstellung MÜSSEN unterstützt werden.
- **UI-011:** Reduzierte Animationen MÜSSEN berücksichtigt werden.
- **UI-012:** Informationen DÜRFEN nicht ausschließlich über Farben vermittelt werden.
- **UI-013:** Virtualisierte Controls MÜSSEN zugänglich bleiben.
- **UI-014:** Accessibility-Einstellungen MÜSSEN zentral verwaltet werden.
- **UI-015:** Änderungen der Einstellungen SOLLEN ohne Neustart wirksam werden.
- **UI-016:** Accessibility-Informationen MÜSSEN unabhängig vom Renderbackend verfügbar sein.
- **UI-017:** NovaStudio MUSS Accessibility-Eigenschaften prüfen und bearbeiten können.
- **UI-018:** Accessibility-Aktionen MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-019:** Die Accessibility-Architektur MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Verwaltung der UI-Elemente.
- **Nova UI Property System:** Accessibility-Eigenschaften.
- **Nova Declarative UI Model:** Deklarative Accessibility-Definitionen.
- **Nova Control Framework:** Semantik und Bedienbarkeit der Controls.
- **Nova Control Templates:** Erhaltung der Control-Semantik.
- **Nova Virtualized Controls:** Zugänglichkeit virtualisierter Inhalte.
- **Nova UI Event System:** Accessibility Events.
- **Nova Routed Events:** Ereignisweiterleitung.
- **Nova Commands and Actions:** Autorisierte Accessibility-Aktionen.
- **Nova Layout Engine:** Skalierung und adaptive Darstellung.
- **Nova Theme Management:** Kontrast- und Darstellungseinstellungen.
- **Nova Reduced Motion Support:** Reduzierte Animationen.
- **Nova Input System:** Alternative Eingabemethoden.
- **Nova UI Renderer Abstraction:** Rendererunabhängige Darstellung.
- **NovaStudio:** Accessibility-Designer und Diagnose.
- **Nova Logic Graph:** Barrierefreie Interaktionslogik.
- **Nova.Trust:** Sicherheit und Datenschutz.

## 17. Ergebnis

NovaOS erhält eine native, systemweite und rendererunabhängige Accessibility-Architektur.

UI-Elemente stellen ihre Bedeutung, Zustände und Aktionen über ein einheitliches semantisches Modell bereit. Unterstützende Technologien können diese Informationen verwenden, ohne die interne Implementierung einzelner Controls kennen zu müssen.

Durch zentrale Einstellungen, vollständige Tastaturbedienbarkeit und die Integration in das Nova UI Framework bleibt die Benutzeroberfläche individuell anpassbar und vollständig nutzbar.

**Grundsatz:** Barrierefreiheit ist keine zusätzliche Betriebsart von NovaOS, sondern eine grundlegende Eigenschaft seiner Benutzeroberfläche.