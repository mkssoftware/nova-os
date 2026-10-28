# NPSPEC-INPUT-FOCUS-0002 – Nova Keyboard Focus Navigation

**Status:** Angenommen  
**Bereich:** Input / Focus / Keyboard Navigation  
**System:** NovaOS

## 1. Zweck

Nova Keyboard Focus Navigation definiert die tastaturbasierte Navigation zwischen fokussierbaren Elementen innerhalb von NovaOS.

Ziel ist eine konsistente, barrierefreie und deterministische Bedienung von Benutzeroberflächen ohne Maus, Touchscreen oder Stift.

Die Navigation funktioniert unabhängig vom verwendeten UI-Renderer und unterstützt sowohl klassische als auch dynamisch erzeugte Benutzeroberflächen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Focus Navigation Controller | Steuerung der Tastaturnavigation |
| Focus Navigation Tree | Logische Struktur fokussierbarer Elemente |
| Focus Traversal Engine | Ermittlung des nächsten Fokusziels |
| Focus Scope Manager | Verwaltung abgegrenzter Navigationsbereiche |
| Focus Navigation Policy | Regeln für Reihenfolge und Navigation |

Die systemweite Fokusverwaltung erfolgt durch Nova Focus Manager.

Nova Keyboard Focus Navigation verwaltet die Navigation innerhalb eines autorisierten Fokuskontexts.

## 3. Navigationsmodell

NovaOS unterstützt folgende Navigationsarten:

| Navigation | Beschreibung |
|---|---|
| Sequential | Navigation zum nächsten oder vorherigen Element |
| Directional | Räumliche Navigation anhand einer Richtung |
| Hierarchical | Navigation innerhalb hierarchischer Strukturen |
| Scoped | Navigation innerhalb eines definierten Fokusbereichs |
| Explicit | Direkte Navigation zu einem bestimmten Element |

Standardmäßig gelten:

- `Tab`: Nächstes fokussierbares Element.
- `Shift + Tab`: Vorheriges fokussierbares Element.
- `Pfeiltasten`: Richtungsabhängige Navigation, sofern vom Steuerelement oder Fokusbereich vorgesehen.
- `Enter`: Aktivierung oder Bestätigung entsprechend dem Element.
- `Space`: Aktivierung entsprechend dem Element.
- `Escape`: Abbruch oder Verlassen eines Kontexts, sofern vorgesehen.

Die konkrete Tastenbelegung MUSS durch die zuständigen Eingaberegeln anpassbar bleiben.

## 4. Focus Navigation Tree

Der Focus Navigation Tree bildet die logische Navigationsstruktur einer Benutzeroberfläche ab.

Jedes fokussierbare Element besitzt mindestens:

- Eindeutige Elementkennung innerhalb seines Fokuskontexts.
- Fokusfähigkeit.
- Aktivierungszustand.
- Sichtbarkeitszustand.
- Navigationsreihenfolge.
- Zugehörigen Focus Scope.

Optionale Eigenschaften:

- Räumliche Position.
- Explizites nächstes Fokusziel.
- Navigationsgruppe.
- Standardfokus.
- Verhalten bei Bereichsgrenzen.

Der Navigation Tree MUSS nicht mit dem visuellen Rendering Tree identisch sein.

Dynamische Änderungen an der Oberfläche MÜSSEN berücksichtigt werden.

## 5. Fokusreihenfolge

Die Navigation erfolgt anhand einer definierten Reihenfolge.

Dabei gilt:

- Explizite Navigationsregeln besitzen Vorrang.
- Ohne explizite Regeln wird die logische Elementreihenfolge verwendet.
- Nicht fokussierbare Elemente werden übersprungen.
- Deaktivierte oder nicht verfügbare Elemente erhalten keinen regulären Fokus.
- Unsichtbare Elemente werden nicht fokussiert.
- Die Reihenfolge MUSS unabhängig vom Rendering reproduzierbar sein.

Nova UI DARF die Navigationsreihenfolge deklarativ festlegen.

## 6. Focus Scopes

Focus Scopes begrenzen die Navigation auf definierte Bereiche.

Beispiele:

- Fenster.
- Dialoge.
- Menüs.
- Symbolleisten.
- Formulare.
- Zusammengesetzte Steuerelemente.

Ein Focus Scope kann:

- Einen Standardfokus definieren.
- Einen vorherigen Fokus speichern.
- Die Navigation innerhalb seines Bereichs begrenzen.
- Den Fokus an einen übergeordneten Bereich zurückgeben.

Modale Dialoge MÜSSEN verhindern können, dass der Fokus durch reguläre Navigation ihren zulässigen Bereich verlässt.

## 7. Räumliche Navigation

NovaOS unterstützt richtungsabhängige Fokuswechsel.

Die Zielermittlung berücksichtigt:

- Gewünschte Navigationsrichtung.
- Geometrische Position.
- Abstand zwischen Elementen.
- Sichtbarkeit.
- Fokusfähigkeit.
- Definierte Navigationsprioritäten.

Bei mehreren möglichen Zielen MUSS eine deterministische Auswahl erfolgen.

Explizite Navigationsbeziehungen besitzen Vorrang vor automatischer geometrischer Zielermittlung.

## 8. Fokuswechsel und Wiederherstellung

Ein Navigationsvorgang erfolgt in folgender Reihenfolge:

1. Navigationseingabe empfangen.
2. Aktiven Focus Scope ermitteln.
3. Navigationsrichtung beziehungsweise Aktion bestimmen.
4. Gültiges Fokusziel ermitteln.
5. Fokuswechsel beim zuständigen Focus Manager anfordern.
6. Fokuszustand aktualisieren.
7. Fokusänderungsereignisse zustellen.

Wird ein fokussiertes Element entfernt oder deaktiviert, MUSS ein gültiges Ersatzfokusziel ermittelt oder der Fokus kontrolliert freigegeben werden.

Beim erneuten Betreten eines Focus Scope SOLL der vorherige Fokus wiederhergestellt werden, sofern das Ziel weiterhin gültig ist.

## 9. Integration mit Nova UI

Nova UI stellt die benötigten Informationen über fokussierbare Steuerelemente bereit.

Dazu gehören:

- Fokusfähigkeit.
- Navigationsreihenfolge.
- Focus Scopes.
- Explizite Navigationsbeziehungen.
- Standardfokus.
- Sichtbarkeit und Aktivierungszustand.

Die Tastaturnavigation MUSS auch bei dynamisch erzeugten UI-Layouts funktionieren.

Eine KI-generierte Benutzeroberfläche unterliegt denselben deterministischen Navigationsregeln wie eine statisch definierte Oberfläche.

Die Navigation DARF keine KI zur Laufzeit voraussetzen.

## 10. Barrierefreiheit

Nova Keyboard Focus Navigation ist eine grundlegende Komponente der Barrierefreiheit.

Festlegungen:

- Sämtliche grundlegenden Systemfunktionen MÜSSEN ohne Zeigegerät erreichbar sein.
- Der aktuelle Tastaturfokus MUSS visuell erkennbar dargestellt werden können.
- Die Fokusreihenfolge SOLL der logischen Bedienreihenfolge entsprechen.
- Fokuswechsel MÜSSEN für unterstützende Technologien verfügbar sein.
- Tastaturnavigation DARF nicht ausschließlich von visuellen Positionen abhängen.
- Benutzerdefinierte Navigationseinstellungen MÜSSEN berücksichtigt werden.

Die visuelle Fokusdarstellung erfolgt durch Nova UI beziehungsweise den zuständigen Renderer.

## 11. Sicherheit und Fehlerbehandlung

Die Navigation unterliegt Nova Input Security und Nova.Trust.

- Navigation DARF keine Sitzungs- oder Berechtigungsgrenzen umgehen.
- Geschützte Eingabekontexte besitzen Vorrang.
- Ungültige Fokusziele werden übersprungen.
- Zirkuläre Navigationsbeziehungen MÜSSEN kontrolliert behandelt werden.
- Fehlerhafte UI-Strukturen DÜRFEN die zentrale Eingabeverarbeitung nicht blockieren.
- Anwendungen DÜRFEN keinen Fokus außerhalb ihres autorisierten Bereichs erzwingen.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine einheitliche tastaturbasierte Fokusnavigation bereitstellen.
- **INPUT-002:** Vorwärts- und Rückwärtsnavigation über `Tab` und `Shift + Tab` MÜSSEN unterstützt werden.
- **INPUT-003:** Räumliche Navigation MUSS unterstützt werden können.
- **INPUT-004:** Die Navigation MUSS auf einer logischen Fokusstruktur basieren.
- **INPUT-005:** Nicht fokussierbare, deaktivierte und unsichtbare Elemente MÜSSEN übersprungen werden.
- **INPUT-006:** Explizite Navigationsreihenfolgen MÜSSEN unterstützt werden.
- **INPUT-007:** Focus Scopes MÜSSEN unterstützt werden.
- **INPUT-008:** Modale Fokusbereiche MÜSSEN ihre Navigationsgrenzen durchsetzen können.
- **INPUT-009:** Dynamische Änderungen der UI-Struktur MÜSSEN berücksichtigt werden.
- **INPUT-010:** Bei ungültigen Fokuszielen MUSS eine kontrollierte Wiederherstellung erfolgen.
- **INPUT-011:** Fokuswechsel MÜSSEN über Nova Focus Manager koordiniert werden.
- **INPUT-012:** Navigationsergebnisse MÜSSEN bei identischem Zustand und identischer Eingabe reproduzierbar sein.
- **INPUT-013:** Der aktuelle Tastaturfokus MUSS für die visuelle Darstellung und unterstützende Technologien verfügbar sein.
- **INPUT-014:** Die Navigation MUSS unabhängig vom verwendeten Grafikbackend funktionieren.
- **INPUT-015:** Sicherheits- und Sitzungsgrenzen MÜSSEN eingehalten werden.
- **INPUT-016:** Nova Keyboard Focus Navigation MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Focus Manager:** Zentrale Fokusverwaltung.
- **Nova Keyboard Events:** Tastaturereignisse.
- **Nova Event Routing:** Zustellung fokusabhängiger Eingaben.
- **Nova Event Dispatcher:** Fokus- und Navigationseignisse.
- **Nova Global Shortcuts:** Abgrenzung systemweiter Tastenkombinationen.
- **Nova Window Manager:** Fenster- und Dialogkontexte.
- **Nova UI:** Fokusstruktur und Steuerelementnavigation.
- **Nova Accessibility:** Barrierefreie Bedienung.
- **Nova Input Security / Nova.Trust:** Berechtigungen und Isolation.

## 14. Ergebnis

NovaOS erhält eine einheitliche und deterministische Tastaturnavigation für sämtliche nativen Benutzeroberflächen.

Durch logische Fokusstrukturen, definierte Navigationsreihenfolgen und Focus Scopes bleiben klassische, dynamische und generativ erzeugte Oberflächen vollständig per Tastatur bedienbar.

**Grundsatz:** Jede grundlegende Funktion von NovaOS muss auch ohne Maus erreichbar und bedienbar sein.