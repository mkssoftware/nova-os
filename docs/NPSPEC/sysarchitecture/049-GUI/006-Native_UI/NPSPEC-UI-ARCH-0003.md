# NPSPEC-UI-ARCH-0003 – Nova Declarative UI Model

**Status:** Angenommen  
**Bereich:** UI / Architecture / Declarative Model  
**System:** NovaOS

## 1. Zweck

Nova Declarative UI Model definiert die deklarative Beschreibung nativer Benutzeroberflächen innerhalb von NovaOS.

Ziel ist die Trennung zwischen Oberflächenstruktur, Darstellung, Daten und Anwendungslogik.

Eine UI-Definition beschreibt, welche Elemente existieren, welche Eigenschaften sie besitzen und wie sie miteinander verbunden sind. Die Nova UI Runtime übernimmt deren Verarbeitung.

Das Modell bildet die Grundlage für Nova UI, NovaLang, NovaStudio, Nova Apps und Solutions.

## 2. Architektur

Das deklarative UI-Modell umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| UI Schema | Definition zulässiger Elemente und Eigenschaften |
| UI Definition Parser | Einlesen deklarativer UI-Beschreibungen |
| UI Definition Validator | Prüfung von Struktur, Typen und Referenzen |
| UI Object Factory | Erzeugung nativer UI-Objekte |
| UI Resource Resolver | Auflösung von Styles, Templates und Ressourcen |
| UI Binding Resolver | Auflösung deklarativer Datenbindungen |
| UI Definition Serializer | Speicherung und Wiederherstellung von UI-Definitionen |

Die Verarbeitung erfolgt unabhängig vom verwendeten Grafikbackend.

## 3. Deklaratives Modell

Eine UI-Definition beschreibt mindestens:

- UI-Elemente und ihre Hierarchie.
- Eigenschaften und Standardwerte.
- Layoutregeln.
- Styles und Templates.
- Ressourcenreferenzen.
- Datenbindungen.
- Ereignisbindungen.
- Zustände und Animationen.

Die Beschreibung DARF keine unmittelbaren Zeichenbefehle voraussetzen.

Die tatsächliche Darstellung wird durch Nova UI und den aktiven Renderer erzeugt.

## 4. UI-Definitionsformat

NovaOS verwendet ein natives, strukturiertes und versionsfähiges UI-Definitionsformat.

Für die textuelle Darstellung wird eine XML-basierte Syntax vorgesehen.

Beispiel:

```xml
<Window Title="Nova App" Width="800" Height="500">
    <StackPanel Spacing="12">
        <TextBlock Text="Willkommen bei NovaOS" />

        <Button Name="Button_1"
                Content="Klicken"
                Width="120" />
    </StackPanel>
</Window>
```

Die XML-Darstellung dient ausschließlich der deklarativen Beschreibung.

Nova UI verwendet weder HTML noch CSS als Grundlage.

Die Runtime DARF validierte UI-Definitionen in einer optimierten binären Repräsentation verarbeiten.

Textuelle und binäre Repräsentationen MÜSSEN dasselbe logische UI-Modell beschreiben.

## 5. Elemente und Hierarchien

UI-Elemente werden hierarchisch organisiert.

Dabei gilt:

- Jedes Element besitzt einen definierten Typ.
- Elemente können Eigenschaften und untergeordnete Elemente besitzen.
- Eltern-Kind-Beziehungen unterliegen dem jeweiligen UI-Schema.
- Elemente können über eindeutige Namen referenziert werden.
- Die logische Struktur muss nicht der späteren visuellen Struktur entsprechen.

Ungültige Hierarchien MÜSSEN bei der Validierung erkannt werden.

## 6. Eigenschaften

Eigenschaften werden deklarativ definiert.

Unterstützt werden:

- Primitive Werte.
- Aufzählungen.
- Strukturierte Werte.
- Ressourcenreferenzen.
- Datenbindungen.
- Zusammengesetzte Eigenschaften.

Die Verarbeitung erfolgt über das Nova UI Property System.

Eigenschaftswerte MÜSSEN anhand ihrer definierten Typen validiert werden.

Änderungen an Eigenschaften DÜRFEN nur die tatsächlich betroffenen UI-Bereiche invalidieren.

## 7. Datenbindungen

Deklarative UI-Definitionen können Eigenschaften mit Datenquellen verbinden.

Beispiel:

```xml
<TextBlock Text="{Binding Benutzername}" />
```

Die Bindungsdefinition beschreibt die Beziehung zwischen Eigenschaft und Datenquelle.

Die tatsächliche Verarbeitung übernimmt die Nova UI Data Binding Engine.

Datenbindungen MÜSSEN typprüfbar und validierbar sein.

Ungültige Bindungen DÜRFEN nicht zum Absturz der gesamten UI-Instanz führen.

## 8. Styles und Templates

Das deklarative Modell unterstützt wiederverwendbare Gestaltungsdefinitionen.

### Styles

Definieren gemeinsame Eigenschaften und visuelle Zustände.

### Templates

Definieren die visuelle Struktur eines Steuerelements.

### Ressourcen

Stellen gemeinsam verwendbare Werte, Bilder, Farben, Schriftdefinitionen und andere UI-Ressourcen bereit.

Styles und Templates MÜSSEN unabhängig von der eigentlichen Anwendungslogik austauschbar sein.

## 9. Ereignisbindungen

UI-Definitionen können Ereignisse mit NovaLang-Funktionen oder Logic-Graph-Ereignisknoten verbinden.

Die Bindung MUSS eindeutig auflösbar sein.

NovaLang unterstützt beispielsweise:

```vb
Private Sub Button1_Click() As Handle Button_1.Click
    ' Ereignisbehandlung
End Sub
```

Die deklarative UI-Definition beschreibt die Elemente und verfügbaren Ereignisse.

Die eigentliche Ereignislogik bleibt Bestandteil des Programms beziehungsweise Logic Graphs.

## 10. Dynamische UI-Definitionen

NovaOS unterstützt die dynamische Erstellung und Veränderung deklarativer Oberflächen.

Dabei gilt:

- Änderungen werden vor ihrer Anwendung validiert.
- Zusammengehörige Änderungen SOLLEN transaktional erfolgen.
- Unveränderte UI-Elemente SOLLEN erhalten bleiben.
- Bestehende Zustände SOLLEN bei kompatiblen Änderungen übernommen werden.
- Fehlerhafte Änderungen DÜRFEN die zuletzt gültige UI-Struktur nicht beschädigen.

Dynamische und generativ erzeugte Oberflächen verwenden dasselbe UI-Schema.

Eine KI ist zur Verarbeitung nicht erforderlich.

## 11. Versionierung und Kompatibilität

Jede UI-Definition besitzt eine Schema-Version.

NovaOS MUSS:

- Die verwendete Schema-Version erkennen.
- Unterstützte Definitionen laden können.
- Nicht unterstützte Eigenschaften kontrolliert behandeln.
- Inkompatible Definitionen eindeutig zurückweisen.
- Definierte Migrationsmechanismen unterstützen können.

Erweiterungen des UI-Schemas DÜRFEN bestehende gültige Definitionen nicht unbeabsichtigt verändern.

## 12. Sicherheit

Deklarative UI-Definitionen unterliegen Nova.Trust.

- UI-Definitionen MÜSSEN vor ihrer Instanziierung validiert werden.
- Ressourcenreferenzen MÜSSEN autorisiert sein.
- Datenbindungen DÜRFEN keine Zugriffsrechte umgehen.
- Ereignisbindungen DÜRFEN keine zusätzlichen Capabilities verleihen.
- Unbekannte Elemente DÜRFEN nicht automatisch ausführbaren Code laden.
- Dynamische UI-Definitionen MÜSSEN denselben Sicherheitsregeln wie statische Definitionen entsprechen.

Die UI-Beschreibung selbst ist keine Berechtigung zur Ausführung von Systemfunktionen.

## 13. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives deklaratives UI-Modell bereitstellen.
- **UI-002:** Das Modell MUSS unabhängig von HTML, CSS und JavaScript funktionieren.
- **UI-003:** UI-Definitionen MÜSSEN strukturiert und versionsfähig sein.
- **UI-004:** Elemente, Eigenschaften und Hierarchien MÜSSEN deklarativ beschreibbar sein.
- **UI-005:** Layouts, Styles, Templates und Ressourcen MÜSSEN unterstützt werden.
- **UI-006:** Datenbindungen und Ereignisbindungen MÜSSEN deklarativ referenzierbar sein.
- **UI-007:** UI-Definitionen MÜSSEN vor der Instanziierung validiert werden.
- **UI-008:** Textuelle und optimierte binäre Repräsentationen MÜSSEN dasselbe logische Modell abbilden.
- **UI-009:** Dynamische Änderungen MÜSSEN kontrolliert angewendet werden.
- **UI-010:** Unveränderte UI-Elemente SOLLEN bei Aktualisierungen wiederverwendet werden.
- **UI-011:** UI-Definitionen MÜSSEN serialisierbar und wiederherstellbar sein.
- **UI-012:** Das deklarative Modell MUSS unabhängig vom Renderbackend funktionieren.
- **UI-013:** UI-Definitionen DÜRFEN keine Sicherheits- oder Capability-Grenzen umgehen.
- **UI-014:** Identische Definitionen, Ressourcen und Ausgangszustände MÜSSEN reproduzierbare UI-Strukturen erzeugen.
- **UI-015:** Nova Declarative UI Model MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Instanziierung und Lebenszyklus.
- **Nova UI Property System:** Eigenschaften.
- **Nova UI Layout Engine:** Layoutregeln.
- **Nova UI Data Binding:** Datenbindungen.
- **Nova UI Style and Template Engine:** Gestaltung.
- **Nova UI Control Framework:** Steuerelemente.
- **Nova UI Renderer Interface:** Grafische Darstellung.
- **NovaLang:** Programmatische UI-Logik.
- **NovaStudio:** Visueller Designer und Quelltextbearbeitung.
- **Nova Logic Graph:** Visuelle Ereignis- und Programmlogik.
- **Nova Solutions:** Deklarative Oberflächen und Layouts.
- **Nova.Trust:** Validierung und Berechtigungen.

## 15. Ergebnis

NovaOS erhält ein einheitliches, deklaratives und versionsfähiges Modell zur Beschreibung nativer Benutzeroberflächen.

UI-Definitionen können sowohl durch NovaLang als auch durch den visuellen Designer von NovaStudio erstellt, bearbeitet und ausgeführt werden.

Die Trennung von UI-Beschreibung, Anwendungslogik und Rendering ermöglicht eine flexible, performante und langfristig erweiterbare Benutzeroberflächenarchitektur.

**Grundsatz:** Die UI-Definition beschreibt die Oberfläche und ihre Beziehungen. Nova UI übernimmt deren effiziente und sichere Umsetzung.