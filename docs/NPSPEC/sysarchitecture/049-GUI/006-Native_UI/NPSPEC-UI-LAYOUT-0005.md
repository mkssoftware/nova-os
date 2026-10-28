# NPSPEC-UI-LAYOUT-0005 – Nova Adaptive Layout

**Status:** Angenommen  
**Bereich:** UI / Layout / Adaptive  
**System:** NovaOS

## 1. Zweck

Nova Adaptive Layout definiert die automatische Anpassung nativer Benutzeroberflächen an unterschiedliche Bildschirmgrößen, Auflösungen, Skalierungen, Eingabemethoden und Nutzungskontexte.

Ziel ist eine flexible und benutzerfreundliche Oberfläche, die sich an die verfügbaren Bedingungen anpasst, ohne dass Anwendungen unterschiedliche UI-Implementierungen benötigen.

Nova Adaptive Layout erweitert die Nova Layout Engine um regelbasierte Layoutanpassungen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Adaptive Layout Manager | Koordination der Layoutanpassungen |
| Adaptive Context Provider | Bereitstellung relevanter Umgebungsinformationen |
| Adaptive Rule Engine | Auswertung deklarativer Anpassungsregeln |
| Layout State Resolver | Auswahl des passenden Layoutzustands |
| Adaptive Transition Controller | Kontrollierter Wechsel zwischen Layoutzuständen |

Die Architektur verwendet die vorhandenen Layoutcontainer und erzeugt keine eigene Rendering-Pipeline.

## 3. Adaptive Context

Der Adaptive Context beschreibt die aktuellen Bedingungen einer Benutzeroberfläche.

Er umfasst bei Bedarf:

- Verfügbare Breite und Höhe.
- Bildschirmauflösung.
- DPI-Skalierung.
- Bildschirmausrichtung.
- Eingabemethoden.
- Fenstergröße.
- Barrierefreiheitseinstellungen.
- Benutzerdefinierte Layoutpräferenzen.

Die Anpassung MUSS auf definierten und überprüfbaren Kontextinformationen basieren.

Eine KI ist für die Auswertung nicht erforderlich.

## 4. Adaptive Regeln

Layoutanpassungen werden deklarativ über Bedingungen und zugehörige Eigenschaften beschrieben.

Unterstützte Anpassungen:

- Änderung der Anordnungsrichtung.
- Änderung von Abständen.
- Anpassung von Elementgrößen.
- Ein- und Ausblenden optionaler Elemente.
- Wechsel von Layoutcontainern.
- Änderung der Spalten- und Zeilenanzahl.
- Anpassung der Navigation.
- Auswahl alternativer Control Templates.

Regeln MÜSSEN deterministisch ausgewertet werden.

Bei mehreren zutreffenden Regeln MUSS eine eindeutig definierte Prioritäts- und Konfliktauflösung gelten.

## 5. Breakpoints

Nova Adaptive Layout unterstützt definierbare Breakpoints.

Breakpoints beschreiben Bedingungen, bei denen ein anderer Layoutzustand verwendet wird.

Beispiel:

```xml
<AdaptiveLayout>
    <AdaptiveLayout.States>
        <AdaptiveState Name="Compact"
                       MaxWidth="600" />

        <AdaptiveState Name="Medium"
                       MinWidth="601"
                       MaxWidth="1024" />

        <AdaptiveState Name="Expanded"
                       MinWidth="1025" />
    </AdaptiveLayout.States>
</AdaptiveLayout>
```

Die angegebenen Werte verwenden logische UI-Einheiten.

Breakpoints DÜRFEN durch Anwendungen und Benutzer angepasst werden.

NovaOS DARF keine festen Bildschirmgrößen als allgemeingültige Gerätekategorien voraussetzen.

## 6. Layoutzustände

Ein Adaptive Layout kann mehrere benannte Layoutzustände besitzen.

Beispiele:

- Compact.
- Medium.
- Expanded.
- Custom.

Jeder Zustand beschreibt ausschließlich die vom Standardlayout abweichenden Eigenschaften.

Gemeinsame UI-Elemente und Datenbindungen SOLLEN erhalten bleiben.

Ein Zustandswechsel DARF keine vollständige Neuerstellung der Benutzeroberfläche erzwingen, sofern die vorhandene Struktur weiterverwendet werden kann.

## 7. Anpassung an Eingabemethoden

Nova Adaptive Layout berücksichtigt unterschiedliche Eingabemethoden.

Dazu gehören:

- Maus.
- Tastatur.
- Touch.
- Stift.
- Kombinierte Eingabegeräte.

Beispielsweise können bei Touchbedienung größere Interaktionsflächen verwendet werden.

Die Anpassung DARF andere verfügbare Eingabemethoden nicht unnötig einschränken.

Ein Wechsel der Eingabemethode MUSS nicht automatisch einen vollständigen Layoutwechsel auslösen.

## 8. Benutzerpräferenzen

Nova Adaptive Layout berücksichtigt benutzerdefinierte Einstellungen.

Dazu gehören:

- Bevorzugte Elementgrößen.
- Abstände.
- Navigationsdarstellung.
- Schriftgrößen.
- Barrierefreiheit.
- Individuelle Layoutanordnungen.

Benutzerdefinierte Anpassungen SOLLEN gegenüber automatischen Empfehlungen Vorrang besitzen.

Gespeicherte Layoutpräferenzen MÜSSEN wiederhergestellt werden können.

Automatische Anpassungen DÜRFEN ausdrücklich festgelegte Benutzerentscheidungen nicht unbemerkt überschreiben.

## 9. Dynamische Anpassung

Layoutanpassungen können während der Laufzeit erfolgen.

Auslöser sind beispielsweise:

- Größenänderungen eines Fensters.
- Wechsel der Bildschirmauflösung.
- Änderung der DPI-Skalierung.
- Wechsel zwischen Monitoren.
- Änderung der Eingabemethode.
- Änderung von Benutzerpräferenzen.

Änderungen MÜSSEN über die Nova Layout Engine verarbeitet werden.

Mehrere zusammengehörige Anpassungen SOLLEN transaktional angewendet werden.

## 10. Zustandswechsel und Animationen

Wechsel zwischen Layoutzuständen können animiert werden.

Dabei gilt:

- Animationen sind optional.
- Die Bedienbarkeit MUSS während des Übergangs erhalten bleiben.
- Animationen MÜSSEN unterbrechbar sein.
- Barrierefreiheitseinstellungen MÜSSEN berücksichtigt werden.
- Änderungen DÜRFEN keine unnötige permanente Neuzeichnung verursachen.

Ohne Animation erfolgt der Zustandswechsel unmittelbar.

## 11. Stabilität und Hysterese

Nova Adaptive Layout MUSS häufige unnötige Zustandswechsel vermeiden können.

Dazu können verwendet werden:

- Hysterese an Breakpoint-Grenzen.
- Zusammenfassung schneller Kontextänderungen.
- Verzögerte Auswertung nicht kritischer Anpassungen.
- Wiederverwendung bereits berechneter Layoutzustände.

Die Oberfläche DARF bei geringfügigen Größenänderungen nicht unkontrolliert zwischen Zuständen wechseln.

## 12. Performance

Nova Adaptive Layout verwendet die Invalidierungs- und Cachingmechanismen der Nova Layout Engine.

Dabei gilt:

- Nur betroffene Eigenschaften werden aktualisiert.
- Unveränderte UI-Komponenten bleiben erhalten.
- Gültige Layoutberechnungen werden wiederverwendet.
- Mehrere Änderungen werden nach Möglichkeit zusammengefasst.
- Nicht benötigte Layoutvarianten verursachen keine permanente Rechenlast.

Die Anpassung erfolgt ereignisgesteuert.

## 13. Sicherheit

Adaptive Layouts unterliegen Nova.Trust.

- Layoutregeln DÜRFEN keine zusätzlichen Capabilities verleihen.
- Dynamische Änderungen MÜSSEN validiert werden.
- Benutzerpräferenzen DÜRFEN keine Sicherheitsrichtlinien umgehen.
- Geschützte Bedienelemente DÜRFEN durch automatische Anpassungen nicht unzulässig verändert werden.
- Fehlerhafte Layoutregeln DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Die Anpassung der Darstellung verändert keine bestehenden Zugriffsrechte.

## 14. Normative Anforderungen

- **UI-001:** NovaOS MUSS adaptive Layoutanpassungen unterstützen.
- **UI-002:** Adaptive Layouts MÜSSEN auf der Nova Layout Engine basieren.
- **UI-003:** Anpassungsregeln MÜSSEN deklarativ definiert werden können.
- **UI-004:** Regeln MÜSSEN deterministisch ausgewertet werden.
- **UI-005:** Definierbare Breakpoints MÜSSEN unterstützt werden.
- **UI-006:** Mehrere Layoutzustände MÜSSEN unterstützt werden.
- **UI-007:** Dynamische Zustandswechsel MÜSSEN möglich sein.
- **UI-008:** Unterschiedliche Eingabemethoden MÜSSEN berücksichtigt werden können.
- **UI-009:** Benutzerdefinierte Layoutpräferenzen MÜSSEN unterstützt werden.
- **UI-010:** Automatische Anpassungen DÜRFEN ausdrückliche Benutzerentscheidungen nicht unbemerkt überschreiben.
- **UI-011:** Unveränderte UI-Komponenten SOLLEN bei Zustandswechseln erhalten bleiben.
- **UI-012:** Layoutänderungen MÜSSEN gezielte Invalidierungen auslösen.
- **UI-013:** Häufige unnötige Zustandswechsel MÜSSEN vermieden werden können.
- **UI-014:** Adaptive Layouts MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-015:** Fehlerhafte Anpassungsregeln MÜSSEN kontrolliert behandelt werden.
- **UI-016:** Adaptive Layouts MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-017:** Nova Adaptive Layout MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Layout Engine:** Layoutberechnung und Invalidierung.
- **Nova Grid Layout:** Adaptive Rasteranordnung.
- **Nova Stack Layout:** Wechsel der Anordnungsrichtung.
- **Nova Dock Layout:** Anpassung angedockter Bereiche.
- **Nova Declarative UI Model:** Definition adaptiver Regeln.
- **Nova UI Runtime:** Verwaltung der Layoutzustände.
- **Nova UI Property System:** Dynamische Eigenschaftsänderungen.
- **Nova UI Component Lifecycle:** Erhaltung und Wiederverwendung von Komponenten.
- **Nova UI Renderer Abstraction:** Darstellung der angepassten Oberfläche.
- **Nova Input Architecture:** Erkennung verfügbarer Eingabemethoden.
- **Nova Display Server:** Bildschirm- und Skalierungsinformationen.
- **Nova UI Accessibility:** Barrierefreiheitseinstellungen.
- **Nova.Trust:** Sicherheit und Isolation.

## 16. Ergebnis

NovaOS erhält ein natives, deklaratives und regelbasiertes Adaptive Layout System.

Benutzeroberflächen können sich automatisch an unterschiedliche Bildschirmgrößen, Fenstergrößen, Eingabemethoden und Benutzerpräferenzen anpassen.

Die Anpassungen erfolgen deterministisch und ressourcenschonend, ohne die bestehende UI-Struktur unnötig neu aufzubauen.

**Grundsatz:** Nova Adaptive Layout passt die Benutzeroberfläche an die tatsächlichen Bedingungen und Bedürfnisse des Benutzers an – nicht den Benutzer an ein starres Layout.