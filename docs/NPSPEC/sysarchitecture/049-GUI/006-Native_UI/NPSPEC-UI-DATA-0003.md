# NPSPEC-UI-DATA-0003 – Nova Data Templates

**Status:** Angenommen  
**Bereich:** UI / Data / Templates  
**System:** NovaOS

## 1. Zweck

Nova Data Templates definiert die deklarative Darstellung von Datenobjekten innerhalb nativer UI-Elemente.

Ziel ist die Trennung zwischen Datenstruktur und visueller Präsentation, sodass unterschiedliche Datentypen automatisch mit passenden UI-Strukturen dargestellt werden können.

Data Templates verwenden das Nova Data Binding und arbeiten unabhängig von Control-Implementierung und Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Data Template Registry | Registrierung verfügbarer Data Templates |
| Data Template Resolver | Auswahl eines passenden Templates |
| Data Template Factory | Erstellung visueller Template-Instanzen |
| Template Context Manager | Zuordnung von Datenobjekten und DataContext |
| Template Selector | Auswahl anhand definierter Bedingungen |
| Template Instance Manager | Verwaltung und Freigabe erzeugter UI-Strukturen |

Nova Data Templates verwendet das Nova Declarative UI Model und die Nova UI Runtime.

## 3. Data-Template-Modell

Ein Data Template beschreibt, wie ein Datenobjekt visuell dargestellt wird.

Ein Template kann enthalten:

- Ziel-Datentyp.
- Eindeutigen Ressourcenschlüssel.
- Deklarative UI-Struktur.
- Datenbindungen.
- Styles.
- Ressourcenreferenzen.
- Visuelle Zustände.
- Optionale Auswahlbedingungen.

Das Datenobjekt bleibt unabhängig von seiner visuellen Darstellung.

Ein Data Template DARF keine eigenständige Anwendungslogik voraussetzen.

## 4. Deklarative Definition

Data Templates werden über das Nova Declarative UI Model definiert.

Beispiel:

```xml
<DataTemplate DataType="Person">

    <StackPanel Orientation="Horizontal"
                Spacing="8">

        <Image
            Source="{Binding ProfileImage}"
            Width="40"
            Height="40" />

        <StackPanel>

            <TextBlock
                Text="{Binding FullName}"
                FontWeight="Bold" />

            <TextBlock
                Text="{Binding Description}" />

        </StackPanel>

    </StackPanel>

</DataTemplate>
```

Das Template beschreibt ausschließlich die visuelle Struktur eines Datenobjekts.

Die Werte werden über Nova Data Binding bereitgestellt.

## 5. Verwendung in Controls

Data Templates können innerhalb datenbasierter Controls verwendet werden.

Beispiel:

```xml
<ListView ItemsSource="{Binding Persons}">

    <ListView.ItemTemplate>

        <DataTemplate DataType="Person">

            <StackPanel Orientation="Horizontal"
                        Spacing="8">

                <TextBlock Text="{Binding FullName}" />

                <TextBlock Text="{Binding Age}" />

            </StackPanel>

        </DataTemplate>

    </ListView.ItemTemplate>

</ListView>
```

Für jedes realisierte Datenelement wird eine passende visuelle Template-Instanz bereitgestellt.

Die vollständige Datenmenge MUSS dafür nicht gleichzeitig als UI-Struktur existieren.

## 6. Automatische Template-Auswahl

NovaOS unterstützt die automatische Auswahl anhand des Datentyps.

Dabei gilt:

- Templates können einem Datentyp zugeordnet werden.
- Die Auswahl berücksichtigt die Typkompatibilität.
- Spezifischere Typdefinitionen SOLLEN Vorrang vor allgemeinen Definitionen besitzen.
- Mehrdeutige Zuordnungen MÜSSEN deterministisch aufgelöst werden.
- Fehlende Templates MÜSSEN kontrolliert behandelt werden.

Explizit zugewiesene Templates besitzen Vorrang vor automatisch ausgewählten Templates.

## 7. Template Selector

Ein Template Selector ermöglicht die Auswahl anhand definierter Bedingungen.

Beispiele:

- Datentyp.
- Datenstatus.
- Elementkategorie.
- Benutzerpräferenz.
- Darstellungsmodus.

Dabei gilt:

- Auswahlregeln MÜSSEN deterministisch sein.
- Änderungen relevanter Daten können eine erneute Auswahl auslösen.
- Die Auswahl DARF keine unkontrollierte Codeausführung ermöglichen.
- Ein Fallback-Template SOLL verfügbar sein.

## 8. DataContext und Datenbindung

Die visuelle Template-Instanz erhält das zugehörige Datenobjekt als DataContext.

Dabei gilt:

- Bindungen werden relativ zum Datenobjekt aufgelöst.
- Observable Properties werden unterstützt.
- Änderungen des Datenobjekts aktualisieren betroffene UI-Eigenschaften.
- Beim Austausch des Datenobjekts werden Bindungen neu zugeordnet.
- Nicht mehr benötigte Beobachtungsregistrierungen werden freigegeben.

Die Datenquelle MUSS keine Kenntnis über das verwendete Data Template besitzen.

## 9. Data Templates und Control Templates

NovaOS unterscheidet zwei Template-Arten.

| Template | Aufgabe |
|---|---|
| Control Template | Definiert die visuelle Struktur eines Controls |
| Data Template | Definiert die visuelle Darstellung eines Datenobjekts |

Ein Data Template kann Controls verwenden.

Diese Controls besitzen wiederum eigene Control Templates.

Beide Template-Arten MÜSSEN unabhängig voneinander austauschbar sein.

## 10. Virtualisierung und Recycling

Nova Data Templates arbeitet mit Nova Virtualized Controls zusammen.

Dabei gilt:

- Template-Instanzen werden nur bei Bedarf erstellt.
- Nicht mehr benötigte Instanzen können wiederverwendet werden.
- Datenbindungen werden beim Recycling neu zugeordnet.
- Elementbezogene Zustände MÜSSEN korrekt zurückgesetzt werden.
- Unterschiedliche Template-Typen MÜSSEN beim Recycling berücksichtigt werden.

Eine Änderung des DataContext DARF keine unbeabsichtigte Übernahme vorheriger Datenzustände verursachen.

## 11. Dynamische Templatewechsel

Data Templates können während der Laufzeit ausgetauscht werden.

Dabei gilt:

- Änderungen des Datentyps oder relevanter Auswahlbedingungen können einen Templatewechsel auslösen.
- Die Datenobjekte bleiben erhalten.
- Bestehende Datenbindungen werden kontrolliert gelöst.
- Neue Template-Instanzen werden bedarfsgerecht erstellt.
- Layout und Rendering werden gezielt invalidiert.

Ein Templatewechsel DARF keine Änderung der zugrunde liegenden Daten erzwingen.

## 12. Styles und Themes

Data Templates können Styles und Ressourcen des aktiven Themes verwenden.

Dabei gilt:

- Themewechsel aktualisieren betroffene Darstellungseigenschaften.
- Funktionale Datenzustände bleiben erhalten.
- Dynamische Ressourcen werden unterstützt.
- Benutzerdefinierte Styles können angewendet werden.

Data Templates MÜSSEN mit Nova Resource Dictionaries zusammenarbeiten.

## 13. Lebenszyklus und Ressourcen

Template-Instanzen unterliegen dem Nova UI Component Lifecycle.

Dabei gilt:

- Instanzen werden bei Bedarf erstellt.
- Datenbindungen werden bei der Initialisierung aktiviert.
- Nicht benötigte Instanzen können recycelt oder entsorgt werden.
- Ereignisbindungen und Ressourcen werden kontrolliert freigegeben.
- Entfernte Template-Instanzen DÜRFEN Datenobjekte nicht unbeabsichtigt im Speicher halten.

## 14. Performance

Nova Data Templates arbeitet bedarfsorientiert.

Dabei gilt:

- Template-Definitionen SOLLEN gemeinsam verwendet werden.
- Template-Auflösungen SOLLEN zwischengespeichert werden.
- Instanzen werden ausschließlich bei Bedarf erstellt.
- Unveränderte Daten verursachen keine unnötigen UI-Aktualisierungen.
- Datenänderungen lösen gezielte Layout- oder Renderinvalidierungen aus.

Eine permanente Neuerstellung der Template-Struktur ist nicht erforderlich.

## 15. NovaStudio-Integration

NovaStudio unterstützt:

- Visuelle Erstellung von Data Templates.
- Auswahl des Ziel-Datentyps.
- Bearbeitung der UI-Struktur.
- Erstellung von Datenbindungen.
- Definition von Template-Auswahlregeln.
- Vorschau mit Beispieldaten.
- Prüfung von Typkompatibilität.
- Diagnose fehlerhafter Bindungen.

Designer und Runtime MÜSSEN dieselben Template-Definitionen verwenden können.

## 16. Sicherheit

Nova Data Templates unterliegt Nova.Trust.

Dabei gilt:

- Datenbindungen DÜRFEN ausschließlich autorisierte Daten verwenden.
- Templates DÜRFEN keine zusätzlichen Capabilities verleihen.
- Template-Auswahlregeln MÜSSEN innerhalb ihres autorisierten Kontexts ausgeführt werden.
- Deklarative Template-Definitionen MÜSSEN validiert werden.
- Fehlerhafte Templates DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

## 17. Normative Anforderungen

- **UI-001:** NovaOS MUSS deklarative Data Templates unterstützen.
- **UI-002:** Datenstruktur und visuelle Darstellung MÜSSEN getrennt bleiben.
- **UI-003:** Data Templates MÜSSEN Datentypen zugeordnet werden können.
- **UI-004:** Automatische Template-Auswahl MUSS unterstützt werden.
- **UI-005:** Explizite Template-Zuweisungen MÜSSEN Vorrang besitzen.
- **UI-006:** Template Selector MÜSSEN unterstützt werden können.
- **UI-007:** Template-Auswahl MUSS deterministisch erfolgen.
- **UI-008:** Template-Instanzen MÜSSEN das zugehörige Datenobjekt als DataContext verwenden können.
- **UI-009:** Data Templates MÜSSEN Nova Data Binding unterstützen.
- **UI-010:** Observable Properties MÜSSEN berücksichtigt werden.
- **UI-011:** Data Templates MÜSSEN mit virtualisierten Controls zusammenarbeiten.
- **UI-012:** Template-Instanzen MÜSSEN kontrolliert recycelt oder freigegeben werden können.
- **UI-013:** Dynamische Templatewechsel MÜSSEN unterstützt werden.
- **UI-014:** Datenobjekte MÜSSEN beim Templatewechsel erhalten bleiben.
- **UI-015:** Änderungen MÜSSEN gezielte Layout- und Renderinvalidierungen auslösen.
- **UI-016:** Data Templates MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-017:** NovaStudio MUSS Data Templates visuell bearbeiten können.
- **UI-018:** Fehlerhafte Templates MÜSSEN kontrolliert behandelt werden.
- **UI-019:** Data Templates MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-020:** Nova Data Templates MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 18. Integration

- **Nova Declarative UI Model:** Deklarative Template-Definitionen.
- **Nova UI Runtime:** Verwaltung der Template-Instanzen.
- **Nova Data Binding:** Verbindung zwischen Daten und UI.
- **Nova Observable Properties:** Änderungsbenachrichtigungen.
- **Nova Control Framework:** Darstellung innerhalb nativer Controls.
- **Nova Control Templates:** Visuelle Struktur verwendeter Controls.
- **Nova Virtualized Controls:** Bedarfsgerechte Instanziierung.
- **Nova Styling System:** Gestaltung.
- **Nova Theme Management:** Themeabhängige Darstellung.
- **Nova Resource Dictionaries:** Wiederverwendbare Ressourcen.
- **Nova Layout Engine:** Größen- und Positionsberechnung.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Component Lifecycle:** Lebenszyklus und Ressourcenverwaltung.
- **NovaStudio:** Visueller Data-Template-Designer.
- **Nova.Trust:** Sicherheit und Isolation.

## 19. Ergebnis

NovaOS erhält ein natives, deklaratives und wiederverwendbares Data-Template-System.

Datenobjekte können automatisch mit geeigneten UI-Strukturen dargestellt werden, ohne ihre Datenmodelle an bestimmte Controls oder Designs zu binden.

Durch Datenbindung, Virtualisierung und gezielte Aktualisierungen bleibt die Darstellung flexibel und ressourcenschonend.

**Grundsatz:** Das Datenobjekt beschreibt, welche Informationen vorhanden sind. Das Data Template bestimmt, wie diese Informationen dargestellt werden.