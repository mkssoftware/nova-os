# NPSPEC-UI-DESIGNER-0004 – Nova Design-Time Properties

**Status:** Angenommen  
**Bereich:** UI / Designer / Properties  
**System:** NovaOS

## 1. Zweck

Nova Design-Time Properties definiert die Eigenschaften und Metadaten, die NovaStudio zur visuellen Bearbeitung nativer NovaOS-Benutzeroberflächen benötigt.

Ziel ist die einheitliche Unterstützung von Standard Controls, Custom Controls, Layouts, Styles und Bindings im visuellen Designer.

Design-Time-Eigenschaften erweitern die Entwicklungsumgebung, ohne das produktive Laufzeitverhalten der Benutzeroberfläche unnötig zu beeinflussen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Design-Time Property Manager | Verwaltung der Designereigenschaften |
| Property Metadata Provider | Bereitstellung von Eigenschaftsmetadaten |
| Property Inspector Adapter | Anbindung an den Eigenschafteneditor |
| Property Editor Registry | Registrierung geeigneter Eigenschaftseditoren |
| Design-Time Value Provider | Bereitstellung von Vorschauwerten |
| Property Validation Service | Prüfung bearbeiteter Werte |

Die Architektur verwendet das bestehende Nova UI Property System.

## 3. Eigenschaftsmetadaten

Controls und ihre Eigenschaften können zusätzliche Design-Time-Metadaten bereitstellen.

Dazu gehören:

- Anzeigename und Beschreibung.
- Kategorie.
- Datentyp.
- Standardwert.
- Zulässige Werte und Wertebereiche.
- Schreib- und Lesbarkeit.
- Sichtbarkeit im Designer.
- Bevorzugter Eigenschaftseditor.
- Unterstützung von Bindings und Ressourcen.
- Auswirkungen auf Layout und Rendering.

Metadaten MÜSSEN unabhängig von der konkreten Darstellung des Property Inspectors definiert werden können.

## 4. Eigenschaftskategorien

NovaStudio SOLL Eigenschaften nach ihrer Funktion gruppieren.

Vorgesehene Kategorien:

- Allgemein.
- Layout.
- Darstellung.
- Text.
- Verhalten.
- Daten und Bindings.
- Events und Commands.
- Animationen.
- Accessibility.
- Erweitert.

Custom Controls DÜRFEN zusätzliche Kategorien definieren.

## 5. Property Inspector

Der Property Inspector verwendet die registrierten Metadaten zur Darstellung und Bearbeitung von Eigenschaften.

Dabei gilt:

- Eigenschaften MÜSSEN entsprechend ihrem Datentyp bearbeitet werden.
- Schreibgeschützte Eigenschaften MÜSSEN erkennbar sein.
- Geerbte und lokal definierte Werte MÜSSEN unterscheidbar bleiben.
- Bindings und Ressourcenreferenzen DÜRFEN nicht unbeabsichtigt durch feste Werte ersetzt werden.
- Änderungen MÜSSEN Undo und Redo unterstützen.
- Ungültige Eingaben MÜSSEN nachvollziehbar gekennzeichnet werden.

## 6. Eigenschaftseditoren

NovaStudio unterstützt spezialisierte Editoren für unterschiedliche Eigenschaftstypen.

Beispiele:

| Eigenschaftstyp | Editor |
|---|---|
| Boolean | Checkbox |
| Enum | Auswahlliste |
| Number | Numerisches Eingabefeld |
| Color | Farbauswahl |
| Font | Schrifteditor |
| Thickness | Abstandseditor |
| Resource | Ressourcenauswahl |
| Binding | Binding-Editor |
| Command | Command-Auswahl |
| Template | Template-Editor |

Custom Controls MÜSSEN eigene Property Editors registrieren können.

## 7. Design-Time-spezifische Eigenschaften

NovaOS unterscheidet zwischen produktiven UI-Eigenschaften und ausschließlich für den Designer bestimmten Eigenschaften.

Design-Time-Eigenschaften können beispielsweise definieren:

- Vorschautexte.
- Beispielbilder.
- Simulierte Daten.
- Design-Time DataContext.
- Vorschauzustände.
- Design-Time-Sichtbarkeit.
- Hilfslinien und Layoutmarkierungen.

Konzeptionelles Beispiel:

```xml
<TextBlock
    Text="{Binding UserName}"
    d:Text="Max Mustermann" />
```

Der Präfix `d:` kennzeichnet ausschließlich für die Entwicklungsumgebung bestimmte Eigenschaften.

Dabei gilt:

- Design-Time-Eigenschaften DÜRFEN das produktive Verhalten nicht verändern.
- Die Nova UI Runtime MUSS Design-Time-Metadaten ignorieren können.
- Die Live Preview DARF Design-Time-Werte verwenden.
- Design-Time-Werte DÜRFEN produktive Bindings nicht dauerhaft überschreiben.

## 8. Custom Controls

Custom Controls können eigene Design-Time-Metadaten bereitstellen.

Dabei gilt:

- Eigenschaftskategorien MÜSSEN definierbar sein.
- Benutzerdefinierte Editoren MÜSSEN registrierbar sein.
- Standardwerte und Validierungsregeln MÜSSEN übernommen werden können.
- Nicht verfügbare Editoren MÜSSEN durch geeignete Standardeditoren ersetzt werden können.
- Fehlende Metadaten DÜRFEN die Verwendung eines Controls nicht grundsätzlich verhindern.

## 9. Validierung

Die Validierung erfolgt über das Nova UI Property System und ergänzende Design-Time-Regeln.

Dabei gilt:

- Datentypen MÜSSEN geprüft werden.
- Wertebereiche und Einschränkungen MÜSSEN berücksichtigt werden.
- Fehler MÜSSEN unmittelbar im Property Inspector sichtbar sein.
- Ungültige Änderungen DÜRFEN gültige Dokumentzustände nicht zerstören.
- Design-Time-Validierung DARF produktive Sicherheitsprüfungen nicht ersetzen.

## 10. Synchronisation und Serialisierung

Design-Time Properties integrieren sich in Nova UI Serialization.

Dabei gilt:

- Produktive Eigenschaftswerte werden regulär in `.nui` gespeichert.
- Design-Time-Metadaten MÜSSEN eindeutig gekennzeichnet werden.
- Änderungen MÜSSEN zwischen Designer und Quelltexteditor synchronisiert werden.
- Unbekannte gültige Design-Time-Eigenschaften SOLLEN beim Speichern erhalten bleiben.
- Die Serialisierung MUSS deterministisch erfolgen.

## 11. Live Preview

Die Live Preview verwendet Design-Time Properties zur Darstellung von Entwicklungszuständen.

Dabei gilt:

- Änderungen MÜSSEN unmittelbar übernommen werden können.
- Design-Time-Daten MÜSSEN von produktiven Daten getrennt bleiben.
- Unterschiedliche Control States SOLLEN simulierbar sein.
- Unveränderte UI-Bereiche SOLLEN nicht erneut berechnet werden.
- Design-Time-Werte DÜRFEN keine unautorisierten Anwendungsaktionen auslösen.

## 12. Sicherheit

Nova Design-Time Properties unterliegt Nova.Trust.

Dabei gilt:

- Property Editors MÜSSEN innerhalb ihres autorisierten Sicherheitskontexts arbeiten.
- Design-Time-Metadaten DÜRFEN keine zusätzlichen Capabilities verleihen.
- Benutzerdefinierte Editoren DÜRFEN keine unautorisierten Systemzugriffe ausführen.
- Vorschauwerte DÜRFEN keine vertraulichen produktiven Daten voraussetzen.
- Die grundlegende Funktion MUSS vollständig offline verfügbar sein.

## 13. Normative Anforderungen

- **UI-001:** NovaStudio MUSS Design-Time Properties unterstützen.
- **UI-002:** Design-Time-Metadaten MÜSSEN auf dem Nova UI Property System aufbauen.
- **UI-003:** Eigenschaften MÜSSEN typisiert und kategorisiert werden können.
- **UI-004:** Der Property Inspector MUSS geeignete Eigenschaftseditoren verwenden.
- **UI-005:** Custom Controls MÜSSEN eigene Metadaten bereitstellen können.
- **UI-006:** Benutzerdefinierte Property Editors MÜSSEN registrierbar sein.
- **UI-007:** Design-Time-Eigenschaften MÜSSEN eindeutig von produktiven Eigenschaften unterscheidbar sein.
- **UI-008:** Design-Time-Werte DÜRFEN produktive Bindings nicht verändern.
- **UI-009:** Eigenschaftsänderungen MÜSSEN validiert werden.
- **UI-010:** Undo und Redo MÜSSEN unterstützt werden.
- **UI-011:** Änderungen MÜSSEN mit `.nui` synchronisiert werden.
- **UI-012:** Die Live Preview MUSS Design-Time-Werte verwenden können.
- **UI-013:** Ungültige Änderungen DÜRFEN gültige Dokumentzustände nicht zerstören.
- **UI-014:** Design-Time-Komponenten MÜSSEN Nova.Trust berücksichtigen.
- **UI-015:** Die Architektur MUSS unabhängig vom Renderbackend funktionieren.
- **UI-016:** Nova Design-Time Properties MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova UI Property System:** Eigenschaftsdefinitionen und Metadaten.
- **Nova Visual UI Designer:** Visuelle Eigenschaftsbearbeitung.
- **Nova UI Serialization:** Speicherung in `.nui`.
- **Nova Live Preview:** Darstellung von Vorschauwerten.
- **Nova Control Framework:** Eigenschaften nativer Controls.
- **Nova Custom Controls:** Benutzerdefinierte Design-Time-Metadaten.
- **Nova Data Binding:** Bindungsdefinitionen.
- **Nova Styling System:** Styles und Ressourcen.
- **Nova UI Accessibility:** Barrierefreiheitseigenschaften.
- **NovaStudio:** Property Inspector und Editoren.
- **Nova.Trust:** Sicherheit und Isolation.

## 15. Ergebnis

NovaStudio erhält ein einheitliches System zur Darstellung, Bearbeitung und Validierung von UI-Eigenschaften während der Entwicklung.

Standard Controls und Custom Controls können ihre Eigenschaften einschließlich geeigneter Editoren und Vorschauwerte bereitstellen.

Produktive Eigenschaften und reine Design-Time-Informationen bleiben eindeutig getrennt.

**Grundsatz:** Der Designer darf zusätzliche Informationen über eine Benutzeroberfläche besitzen, ohne deren produktives Verhalten zu verändern.