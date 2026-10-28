# NPSPEC-UI-DESIGNER-0002 – Nova UI Serialization

**Status:** Angenommen  
**Bereich:** UI / Designer / Serialization  
**System:** NovaOS

## 1. Zweck

Nova UI Serialization definiert die verlustfreie Speicherung, Wiederherstellung und Synchronisation deklarativer Benutzeroberflächen.

Ziel ist ein einheitliches Serialisierungssystem für NovaStudio, den visuellen UI-Designer und die Nova UI Runtime.

Das native Dateiformat ist `.nui`. Es beschreibt die Struktur, Eigenschaften, Ressourcen und Bindungen einer Benutzeroberfläche, unabhängig vom verwendeten Renderer.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| UI Serializer | Umwandlung des UI-Modells in `.nui` |
| UI Deserializer | Einlesen und Rekonstruktion des UI-Modells |
| UI Document Model | Gemeinsame strukturierte Dokumentrepräsentation |
| UI Schema Validator | Prüfung von Syntax, Typen und Struktur |
| UI Type Resolver | Auflösung registrierter Controls und Eigenschaften |
| UI Change Tracker | Erfassung von Dokumentänderungen |
| UI Version Manager | Verwaltung der Formatkompatibilität |

Die Serialisierung MUSS unabhängig von der Nova UI Runtime ausführbar sein.

## 3. Dateiformat

NovaOS verwendet ein natives, deklaratives XML-basiertes Format mit der Dateiendung `.nui`.

Beispiel:

```xml
<Window
    Name="MainWindow"
    Title="Nova Anwendung"
    Width="800"
    Height="600">

    <Grid>
        <Button
            Name="Button_1"
            Content="Speichern"
            Width="120"
            Height="40" />
    </Grid>

</Window>
```

Dabei gilt:

- Das Format MUSS menschenlesbar sein.
- Die Dokumentstruktur MUSS eindeutig definiert sein.
- Eigenschaften MÜSSEN typsicher interpretierbar sein.
- Verschachtelte Controls MÜSSEN unterstützt werden.
- Das Format DARF keine Abhängigkeit von HTML, CSS oder JavaScript besitzen.

## 4. Serialisierbare Inhalte

Folgende Bestandteile MÜSSEN unterstützt werden:

- UI-Elemente und ihre Hierarchie.
- Layoutdefinitionen.
- Eigenschaften und lokale Werte.
- Styles und Control Templates.
- Data Templates.
- Resource Dictionaries und Ressourcenreferenzen.
- Data Bindings.
- Commands und Event Bindings.
- Animationen und Transitions.
- Accessibility-Eigenschaften.

Laufzeitinterne Zustände wie native Handles, Threadreferenzen und temporäre Renderressourcen DÜRFEN nicht Bestandteil der deklarativen Serialisierung sein.

## 5. Typen und Eigenschaften

Die Serialisierung verwendet das Nova UI Property System und die registrierten Typdefinitionen.

Dabei gilt:

- Eigenschaftswerte MÜSSEN anhand ihrer Typdefinition verarbeitet werden.
- Standardwerte SOLLEN nicht unnötig gespeichert werden.
- Explizit gesetzte Werte MÜSSEN von impliziten Standardwerten unterscheidbar bleiben.
- Unbekannte Eigenschaften MÜSSEN kontrolliert behandelt werden.
- Ungültige Typkonvertierungen MÜSSEN nachvollziehbare Diagnoseinformationen erzeugen.

## 6. Bindings und Ressourcen

Bindings und Ressourcenreferenzen werden als deklarative Definitionen gespeichert.

Dabei gilt:

- Data Bindings MÜSSEN als Bindung erhalten bleiben.
- Dynamische Ressourcen DÜRFEN nicht durch ihre aktuellen Laufzeitwerte ersetzt werden.
- Command Bindings MÜSSEN ihre Zielreferenzen behalten.
- Ereignisbindungen MÜSSEN mit NovaLang und Logic Graph kompatibel sein.
- Die Serialisierung DARF keine Bindings ausführen.

## 7. Verlustfreie Bearbeitung

NovaStudio MUSS `.nui`-Dokumente ohne unbeabsichtigten Informationsverlust bearbeiten können.

Dabei gilt:

- Nicht veränderte Dokumentbereiche SOLLEN erhalten bleiben.
- Kommentare SOLLEN erhalten bleiben.
- Unbekannte, syntaktisch gültige Erweiterungen MÜSSEN bei unveränderter Bearbeitung erhalten bleiben.
- Änderungen SOLLEN möglichst kleine Quelltextdifferenzen erzeugen.
- Elementreihenfolgen DÜRFEN nicht ohne funktionale Notwendigkeit verändert werden.
- Die Speicherung MUSS deterministisch erfolgen.

## 8. Designer-Synchronisation

Der visuelle Designer und der Quelltexteditor verwenden dasselbe UI Document Model.

Dabei gilt:

- Änderungen MÜSSEN bidirektional synchronisiert werden.
- Die Synchronisation SOLL inkrementell erfolgen.
- Undo und Redo MÜSSEN beide Bearbeitungsformen berücksichtigen.
- Ungültiger Quelltext DARF das letzte gültige Dokumentmodell nicht zerstören.
- Konflikte MÜSSEN nachvollziehbar dargestellt werden.
- Der Designer DARF keine eigenständige, inkompatible Dokumentrepräsentation verwenden.

## 9. Versionierung und Erweiterbarkeit

Das `.nui`-Format MUSS kontrolliert weiterentwickelt werden können.

Dabei gilt:

- Dokumente MÜSSEN eine eindeutig bestimmbare Formatversion besitzen.
- Neue Controls und Eigenschaften MÜSSEN registrierbar sein.
- Unbekannte Erweiterungen DÜRFEN nicht automatisch verworfen werden.
- Inkompatible Dokumentversionen MÜSSEN erkannt werden.
- Migrationen MÜSSEN explizit und nachvollziehbar erfolgen.
- Eine Migration DARF das Originaldokument nicht ungesichert überschreiben.

## 10. Validierung und Fehlerbehandlung

Der Deserializer MUSS Dokumente vor der Übernahme in die UI Runtime validieren.

Geprüft werden insbesondere:

- Syntax und Dokumentstruktur.
- Bekannte Elementtypen.
- Eigenschaftstypen.
- Ressourcenreferenzen.
- Bindings.
- Namenskonflikte.
- Nicht zulässige Referenzen.

Fehler MÜSSEN mit möglichst genauer Position und Ursache gemeldet werden.

NovaStudio SOLL auch teilweise fehlerhafte Dokumente zur Korrektur öffnen können.

Die produktive UI Runtime DARF ungültige Dokumentbereiche nicht ungeprüft ausführen.

## 11. Sicherheit

Nova UI Serialization unterliegt Nova.Trust.

Dabei gilt:

- Das Einlesen eines Dokuments DARF keine beliebige Codeausführung auslösen.
- Controls und Erweiterungen MÜSSEN über autorisierte Typregistrierungen aufgelöst werden.
- Deklarative Inhalte DÜRFEN keine zusätzlichen Capabilities verleihen.
- Externe Ressourcenreferenzen MÜSSEN den geltenden Berechtigungen unterliegen.
- Parser MÜSSEN gegen übermäßige Verschachtelung, zyklische Referenzen und unkontrollierten Ressourcenverbrauch geschützt sein.

## 12. Performance

Die Serialisierung MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Unveränderte Dokumentbereiche SOLLEN nicht unnötig neu verarbeitet werden.
- Inkrementelles Parsing SOLL unterstützt werden.
- Große Dokumente DÜRFEN die NovaStudio-Oberfläche nicht dauerhaft blockieren.
- Die Deserialisierung MUSS von der eigentlichen Darstellung getrennt bleiben.
- Änderungen am Dokument DÜRFEN keine vollständige Neuerstellung der UI erzwingen, sofern eine sichere inkrementelle Aktualisierung möglich ist.

## 13. Normative Anforderungen

- **UI-001:** NovaOS MUSS `.nui` als natives deklaratives UI-Format unterstützen.
- **UI-002:** Serialisierung und Deserialisierung MÜSSEN auf einem gemeinsamen Dokumentmodell beruhen.
- **UI-003:** Die Serialisierung MUSS unabhängig vom Renderbackend funktionieren.
- **UI-004:** UI-Strukturen, Eigenschaften, Ressourcen und Bindings MÜSSEN serialisierbar sein.
- **UI-005:** Laufzeitinterne Ressourcen DÜRFEN nicht als deklarative UI-Daten gespeichert werden.
- **UI-006:** Eigenschaftswerte MÜSSEN typsicher verarbeitet werden.
- **UI-007:** Bindings MÜSSEN als deklarative Definitionen erhalten bleiben.
- **UI-008:** Die Serialisierung MUSS deterministische Ergebnisse erzeugen.
- **UI-009:** Gültige unbekannte Erweiterungen MÜSSEN bei unveränderter Bearbeitung erhalten bleiben.
- **UI-010:** Designer und Quelltexteditor MÜSSEN bidirektional synchronisiert werden.
- **UI-011:** Ungültiger Quelltext DARF gültige Dokumentzustände nicht zerstören.
- **UI-012:** Dokumentversionen MÜSSEN erkannt und geprüft werden.
- **UI-013:** Fehler MÜSSEN nachvollziehbare Diagnoseinformationen erzeugen.
- **UI-014:** Die Deserialisierung DARF keine unautorisierte Codeausführung ermöglichen.
- **UI-015:** Große Dokumente SOLLEN inkrementell verarbeitet werden.
- **UI-016:** Nova UI Serialization MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Declarative UI Model:** Definition des UI-Dokumentformats.
- **Nova UI Property System:** Typisierte Eigenschaften.
- **Nova UI Runtime:** Instanziierung gültiger UI-Dokumente.
- **Nova Control Framework:** Registrierung von Controls.
- **Nova Layout Engine:** Layoutdefinitionen.
- **Nova Styling System:** Styles und Templates.
- **Nova Resource Dictionaries:** Ressourcenverwaltung.
- **Nova Data Binding:** Deklarative Bindungen.
- **Nova UI Animation Engine:** Animationsdefinitionen.
- **Nova Visual UI Designer:** Visuelle Dokumentbearbeitung.
- **NovaStudio:** Quelltexteditor, Validierung und Synchronisation.
- **NovaLang / Logic Graph:** Ereignis- und Command-Verknüpfungen.
- **Nova.Trust:** Sichere Dokumentverarbeitung.

## 15. Ergebnis

NovaOS erhält ein einheitliches, deterministisches und erweiterbares Serialisierungssystem für native Benutzeroberflächen.

`.nui`-Dokumente können visuell oder textuell bearbeitet, validiert, gespeichert und durch die Nova UI Runtime geladen werden.

Die Serialisierung bewahrt deklarative Informationen und ermöglicht eine verlustfreie Zusammenarbeit zwischen Designer und Quelltexteditor.

**Grundsatz:** Eine Benutzeroberfläche besitzt genau eine maßgebliche deklarative Beschreibung – unabhängig davon, ob sie grafisch gestaltet oder als Quelltext bearbeitet wird.