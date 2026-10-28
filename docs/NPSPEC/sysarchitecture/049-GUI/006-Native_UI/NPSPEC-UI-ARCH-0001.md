# NPSPEC-UI-ARCH-0001 – Nova UI Architecture

**Status:** Angenommen  
**Bereich:** UI / Architecture  
**System:** NovaOS

## 1. Zweck

Nova UI definiert die native Benutzeroberflächenarchitektur von NovaOS.

Ziel ist ein modernes, deklaratives, performantes und vollständig anpassbares UI-System, das unabhängig von HTML, CSS, JavaScript und externen UI-Frameworks funktioniert.

Die Architektur orientiert sich an den bewährten Konzepten von WPF, wird jedoch vollständig für NovaOS entwickelt.

Nova UI bildet die gemeinsame Grundlage für Systemoberflächen, Nova Apps, Capabilities und Solutions.

## 2. Grundprinzipien

Nova UI basiert auf folgenden Architekturprinzipien:

- **Deklarativ:** Benutzeroberflächen beschreiben ihren gewünschten Zustand statt einzelner Zeichenoperationen.
- **Reaktiv:** Änderungen an Daten und Eigenschaften aktualisieren ausschließlich betroffene UI-Bereiche.
- **Komponentenbasiert:** Oberflächen bestehen aus wiederverwendbaren Steuerelementen.
- **Rendererunabhängig:** Darstellung und UI-Logik sind voneinander getrennt.
- **Datengebunden:** UI-Eigenschaften können direkt an Datenmodelle gebunden werden.
- **Stilbasiert:** Aussehen und Verhalten werden soweit sinnvoll getrennt.
- **Ereignisgesteuert:** Benutzerinteraktionen werden über definierte Ereignisse verarbeitet.
- **Deterministisch:** Identische Zustände und Eingaben erzeugen reproduzierbare Ergebnisse.
- **Ressourcenschonend:** Unveränderte Bereiche werden nicht unnötig neu berechnet oder gezeichnet.
- **Offline-First:** Sämtliche grundlegenden Funktionen arbeiten ohne Netzwerk oder KI.

## 3. Architektur

Nova UI besteht aus folgenden Hauptkomponenten:

| Komponente | Aufgabe |
|---|---|
| UI Runtime | Verwaltung des UI-Lebenszyklus |
| UI Object Model | Einheitliches Objektmodell für UI-Elemente |
| Declarative UI Engine | Verarbeitung deklarativer UI-Definitionen |
| Layout Engine | Berechnung von Positionen und Größen |
| Property System | Verwaltung von Eigenschaften und Abhängigkeiten |
| Data Binding Engine | Verbindung zwischen Daten und Oberfläche |
| Style and Template Engine | Gestaltung und Wiederverwendung |
| Control Framework | Standard- und benutzerdefinierte Steuerelemente |
| UI Event System | Verarbeitung von UI-Ereignissen |
| Animation Engine | Zeitbasierte Änderungen von UI-Eigenschaften |
| UI Renderer Interface | Abstraktion des Renderbackends |
| Accessibility Interface | Barrierefreie Bereitstellung der UI-Struktur |

Die Komponenten MÜSSEN über definierte Schnittstellen zusammenarbeiten.

## 4. UI-Objektmodell

Jedes UI-Element basiert auf einem gemeinsamen Objektmodell.

Ein UI-Element besitzt mindestens:

- Eindeutige Identität innerhalb seines UI-Kontexts.
- Typ.
- Eigenschaften.
- Optional übergeordnetes Element.
- Optional untergeordnete Elemente.
- Sichtbarkeits- und Aktivierungszustand.
- Layoutinformationen.
- Ereignisbindungen.

Nova UI unterscheidet zwischen:

- **Logical Tree:** Logische Struktur und Beziehungen der UI-Elemente.
- **Visual Tree:** Tatsächlich darzustellende Elemente.
- **Render Tree:** Für die Darstellung optimierte Renderinformationen.

Diese Strukturen DÜRFEN voneinander abweichen.

Änderungen MÜSSEN kontrolliert zwischen den betroffenen Strukturen synchronisiert werden.

## 5. Deklarative Benutzeroberflächen

Benutzeroberflächen werden über eine native deklarative Beschreibung definiert.

Die Beschreibung unterstützt:

- Elemente und Hierarchien.
- Eigenschaften.
- Layouts.
- Styles.
- Templates.
- Ressourcen.
- Datenbindungen.
- Ereignisbindungen.
- Animationen.

Die UI-Definition beschreibt die Oberfläche, während die Anwendungslogik deren Verhalten bestimmt.

NovaLang MUSS die Erstellung und Manipulation von UI-Elementen unterstützen.

NovaStudio MUSS deklarative UI-Definitionen visuell bearbeiten können.

## 6. Property System

Nova UI verwendet ein einheitliches Eigenschaftssystem.

Eigenschaften unterstützen bei Bedarf:

- Standardwerte.
- Typprüfung.
- Validierung.
- Änderungsbenachrichtigungen.
- Datenbindung.
- Vererbung.
- Styles.
- Animationen.

Änderungen an Eigenschaften MÜSSEN ausschließlich die tatsächlich betroffenen Verarbeitungsschritte auslösen.

Beispielsweise darf eine reine Farbänderung keine vollständige Neuberechnung des Layouts erzwingen.

## 7. Layout Engine

Die Layout Engine berechnet Position und Größe der UI-Elemente.

Sie unterstützt mindestens:

- Absolute Positionierung.
- Horizontale und vertikale Anordnung.
- Rasterlayouts.
- Flexible Größenverteilung.
- Mindest- und Maximalgrößen.
- Abstände und Ausrichtung.
- Verschachtelte Layouts.
- DPI- und skalierungsabhängige Darstellung.

Die Layoutberechnung erfolgt bedarfsorientiert.

Unveränderte Layoutbereiche MÜSSEN wiederverwendet werden können.

## 8. Data Binding

Nova UI unterstützt deklarative Datenbindungen zwischen UI-Eigenschaften und Datenmodellen.

Unterstützte Bindungsarten:

- One-Time.
- One-Way.
- Two-Way.
- One-Way-to-Source.

Datenänderungen MÜSSEN gezielte Aktualisierungen auslösen können.

Zyklische Bindungen und fehlerhafte Datenquellen MÜSSEN kontrolliert behandelt werden.

Datenbindungen DÜRFEN keine Sicherheits- oder Capability-Grenzen umgehen.

## 9. Styles, Templates und Controls

Nova UI trennt die Funktion eines Steuerelements von seiner visuellen Darstellung.

### Styles

Definieren wiederverwendbare Eigenschaften und Zustandsdarstellungen.

### Templates

Definieren die visuelle Struktur eines Steuerelements.

### Controls

Stellen wiederverwendbare UI-Funktionen bereit.

Nova UI unterstützt:

- Standard Controls.
- Custom Controls.
- Zusammengesetzte Controls.
- Benutzerdefinierte Styles und Templates.

Das Erscheinungsbild eines Steuerelements MUSS geändert werden können, ohne dessen grundlegende Funktion neu implementieren zu müssen.

## 10. Ereignisverarbeitung

Nova UI verwendet ein eigenes UI-Ereignismodell auf Grundlage der Nova Input Architecture.

Unterstützte Ereignisse umfassen:

- Pointer- und Tastaturereignisse.
- Fokusereignisse.
- Gesten.
- Steuerelementereignisse.
- Eigenschaftsänderungen.
- Lebenszyklusereignisse.

UI-Ereignisse können direkt oder entlang definierter Elementhierarchien verarbeitet werden.

NovaLang unterstützt deklarative und dynamische Ereignisbindungen.

Beispiel:

    Private Sub Button1_Click() As Handle Button_1.Click
        ' Ereignisbehandlung
    End Sub

Dynamische Bindungen erfolgen beispielsweise über:

    addHandle Button_1.Click To buttonClick()

Die Ereignisverarbeitung MUSS unabhängig vom Renderbackend funktionieren.

## 11. Rendering

Nova UI erzeugt eine rendererunabhängige Darstellung der Benutzeroberfläche.

Die eigentliche Ausgabe erfolgt über das Nova UI Renderer Interface.

Anfangs wird die vorhandene NovaOS-Grafikbibliothek verwendet.

Später können spezialisierte oder hardwarebeschleunigte Renderer eingesetzt werden.

Dabei gilt:

- UI-Logik und Rendering bleiben getrennt.
- Unveränderte Elemente werden nicht unnötig neu verarbeitet.
- Änderungen erzeugen gezielte Invalidierungen.
- Damage Tracking wird unterstützt.
- Rendering erfolgt unabhängig von der Eingabeverarbeitung.
- Renderer können ausgetauscht werden, ohne die UI-Definition verändern zu müssen.

Die Übergabe der gerenderten Oberflächen an den Compositor erfolgt über die zuständige Grafik- und Displayarchitektur.

## 12. Animationen

Nova UI unterstützt deklarative Animationen von UI-Eigenschaften.

Animationen MÜSSEN:

- Zeitbasiert statt framebasiert arbeiten.
- Unterbrechbar sein.
- Ressourcenlimits berücksichtigen.
- Bei Bedarf deaktivierbar sein.
- Nur betroffene Darstellungsbereiche aktualisieren.

Die Animation Engine DARF keine permanente Neuzeichnung unveränderter Oberflächen verursachen.

## 13. Generative und dynamische UI

Nova UI unterstützt dynamisch erzeugte Benutzeroberflächen.

Dabei gilt:

- Jede Oberfläche basiert auf einer gültigen deklarativen UI-Struktur.
- Dynamische Änderungen werden über definierte Transaktionen angewendet.
- UI-Zustände können gespeichert und wiederhergestellt werden.
- Generierte Oberflächen unterliegen denselben Sicherheits- und Validierungsregeln wie statische Oberflächen.
- Die Darstellung MUSS ohne KI reproduzierbar bleiben.

KI DARF UI-Definitionen erzeugen oder verändern, ist jedoch keine notwendige Laufzeitkomponente.

## 14. Sicherheit und Isolation

Nova UI unterliegt Nova.Trust und dem Capability-System.

- UI-Elemente DÜRFEN keine unautorisierten Systemfunktionen ausführen.
- Datenbindungen MÜSSEN bestehende Zugriffsrechte beachten.
- Ereignisbindungen DÜRFEN keine Capability-Grenzen umgehen.
- Benutzerdefinierte Controls MÜSSEN innerhalb ihres autorisierten Kontexts arbeiten.
- Fehlerhafte UI-Komponenten DÜRFEN andere Prozesse nicht beeinträchtigen.
- Geschützte Eingabekontexte MÜSSEN unterstützt werden.

Die Darstellung einer Funktion begründet keine Berechtigung zu deren Ausführung.

## 15. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives, deklaratives UI-System bereitstellen.
- **UI-002:** Nova UI DARF HTML, CSS oder JavaScript nicht als technische Grundlage voraussetzen.
- **UI-003:** UI-Logik, Layout und Rendering MÜSSEN voneinander getrennt sein.
- **UI-004:** Ein einheitliches UI-Objektmodell MUSS bereitgestellt werden.
- **UI-005:** Logische und visuelle Elementstrukturen MÜSSEN unterstützt werden.
- **UI-006:** Ein einheitliches Property System MUSS vorhanden sein.
- **UI-007:** Deklarative Datenbindungen MÜSSEN unterstützt werden.
- **UI-008:** Styles, Templates und Custom Controls MÜSSEN unterstützt werden.
- **UI-009:** UI-Ereignisse MÜSSEN über definierte Schnittstellen verarbeitet werden.
- **UI-010:** Die Layout Engine MUSS bedarfsorientierte Neuberechnungen unterstützen.
- **UI-011:** Das Rendering MUSS über eine austauschbare Backend-Schnittstelle erfolgen.
- **UI-012:** Änderungen MÜSSEN gezielte UI-Invalidierungen auslösen können.
- **UI-013:** Nova UI MUSS mit dem Damage Tracking des Compositors zusammenarbeiten.
- **UI-014:** Animationen MÜSSEN zeitbasiert und unterbrechbar sein.
- **UI-015:** Dynamisch erzeugte Oberflächen MÜSSEN validierbar und reproduzierbar sein.
- **UI-016:** UI-Zustände MÜSSEN gespeichert und wiederhergestellt werden können.
- **UI-017:** Nova UI MUSS die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-018:** Nova UI MUSS Barrierefreiheit und Tastaturnavigation unterstützen.
- **UI-019:** Nova UI MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova Framework:** UI-Bibliothek und öffentliche Programmierschnittstellen.
- **NovaLang:** Native UI-Programmierung und Ereignisbindungen.
- **NovaStudio:** Visueller Designer und deklarative UI-Bearbeitung.
- **Nova Input Architecture:** Eingabeereignisse.
- **Nova Focus Manager:** Fokusverwaltung.
- **Nova Graphics Architecture:** Grafische Darstellung.
- **Nova Render Engine:** Rendering.
- **Nova Compositor:** Zusammensetzung der Oberflächen.
- **Nova Window Manager:** Fensterverwaltung.
- **Nova Capability System:** Funktions- und Berechtigungsmodell.
- **Nova Solutions:** Deklarative Benutzeroberflächen und Layouts.
- **Nova.Trust:** Sicherheit und Isolation.

## 17. Ergebnis

NovaOS erhält eine eigenständige, native und deklarative UI-Architektur.

Durch die Trennung von Darstellung, Verhalten, Layout und Rendering können Benutzeroberflächen effizient entwickelt, gestaltet, angepasst und auf unterschiedlichen Grafikbackends ausgeführt werden.

Die Architektur unterstützt klassische Anwendungen ebenso wie visuell programmierte Nova Apps, Capabilities und dynamische Solutions.

**Grundsatz:** Nova UI beschreibt, was dargestellt werden soll. Das System entscheidet effizient, wie und wann die Darstellung aktualisiert wird.