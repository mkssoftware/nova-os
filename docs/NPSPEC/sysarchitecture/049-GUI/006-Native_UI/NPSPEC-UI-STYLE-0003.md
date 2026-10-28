# NPSPEC-UI-STYLE-0003 – Nova Resource Dictionaries

**Status:** Angenommen  
**Bereich:** UI / Styling / Resources  
**System:** NovaOS

## 1. Zweck

Nova Resource Dictionaries definiert die zentrale Verwaltung, Wiederverwendung und Auflösung von UI-Ressourcen innerhalb von NovaOS.

Ziel ist die Trennung gemeinsam genutzter Gestaltungs- und Konfigurationswerte von einzelnen UI-Elementen.

Resource Dictionaries ermöglichen konsistente Designs, dynamische Ressourcenänderungen und eine speichereffiziente Nutzung gemeinsamer Objekte.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Resource Dictionary | Speicherung benannter Ressourcen |
| Resource Manager | Verwaltung und Lebenszyklus |
| Resource Resolver | Auflösung von Ressourcenreferenzen |
| Resource Scope Manager | Verwaltung der Gültigkeitsbereiche |
| Resource Merge Manager | Zusammenführung mehrerer Dictionaries |
| Resource Dependency Tracker | Erfassung dynamischer Ressourcenabhängigkeiten |
| Resource Cache | Wiederverwendung aufgelöster Ressourcen |

Resource Dictionaries verwenden das Nova UI Property System und das Nova Declarative UI Model.

## 3. Ressourcenmodell

Eine Ressource besteht mindestens aus:

- Eindeutigem Schlüssel innerhalb ihres Gültigkeitsbereichs.
- Definiertem Datentyp.
- Ressourcenwert oder Ressourcendefinition.

Unterstützte Ressourcentypen umfassen:

- Farben und Brushes.
- Schriftarten und Schriftgrößen.
- Abstände und geometrische Werte.
- Styles.
- Control Templates.
- Animationen.
- Icons und Bilder.
- Weitere registrierte UI-Ressourcentypen.

Ressourcen MÜSSEN typsicher aufgelöst werden.

Benutzerdefinierte Ressourcentypen DÜRFEN registriert werden.

## 4. Deklarative Definition

Resource Dictionaries werden über das Nova Declarative UI Model beschrieben.

Beispiel:

```xml
<ResourceDictionary>

    <Color Key="AccentColor"
           Value="#408CFF" />

    <SolidColorBrush Key="AccentBrush"
                     Color="{Resource AccentColor}" />

    <Thickness Key="DefaultPadding"
               Value="12,8" />

    <Style Key="PrimaryButton"
           TargetType="Button">

        <Setter Property="Background"
                Value="{Resource AccentBrush}" />

        <Setter Property="Padding"
                Value="{Resource DefaultPadding}" />

    </Style>

</ResourceDictionary>
```

Ressourcen können innerhalb eines Dictionaries auf andere Ressourcen verweisen.

Zyklische Referenzen MÜSSEN erkannt und kontrolliert behandelt werden.

## 5. Ressourcenreferenzen

NovaOS unterstützt zwei grundlegende Referenzarten.

### Statische Ressourcen

Werden bei der Initialisierung beziehungsweise beim erstmaligen Auflösen gebunden.

```xml
<Button Background="{StaticResource AccentBrush}" />
```

### Dynamische Ressourcen

Werden bei Änderungen der zugrunde liegenden Ressource aktualisiert.

```xml
<Button Background="{DynamicResource AccentBrush}" />
```

Dabei gilt:

- Statische Ressourcen benötigen keine dauerhafte Änderungsüberwachung.
- Dynamische Ressourcen MÜSSEN relevante Änderungen erkennen.
- Änderungen MÜSSEN gezielt an abhängige Eigenschaften weitergegeben werden.
- Ungültige Referenzen MÜSSEN kontrolliert behandelt werden.

## 6. Gültigkeitsbereiche

Resource Dictionaries können auf verschiedenen Ebenen definiert werden:

1. UI-Element.
2. Übergeordneter UI-Container.
3. Fenster oder UI-Komponente.
4. Anwendung.
5. Aktives Theme.
6. NovaOS-Systemressourcen.

Die Ressourcenauflösung beginnt im lokalen Kontext und durchsucht anschließend die übergeordneten Gültigkeitsbereiche.

Die Suchreihenfolge MUSS eindeutig und deterministisch sein.

Lokale Ressourcen besitzen Vorrang vor gleichnamigen Ressourcen übergeordneter Bereiche.

## 7. Zusammenführung von Dictionaries

Mehrere Resource Dictionaries können zusammengeführt werden.

Beispiel:

```xml
<ResourceDictionary>

    <ResourceDictionary.MergedDictionaries>

        <ResourceDictionary Source="Colors.nui" />

        <ResourceDictionary Source="Typography.nui" />

        <ResourceDictionary Source="Controls.nui" />

    </ResourceDictionary.MergedDictionaries>

</ResourceDictionary>
```

Dabei gilt:

- Dictionaries MÜSSEN modular kombinierbar sein.
- Relative Ressourcenpfade werden anhand eines definierten Basiskontexts aufgelöst.
- Konflikte zwischen gleichnamigen Ressourcen MÜSSEN deterministisch behandelt werden.
- Zyklische Dictionary-Abhängigkeiten MÜSSEN erkannt werden.
- Fehlende Dictionaries MÜSSEN kontrolliert behandelt werden.

Die Konfliktauflösung MUSS dokumentiert und unabhängig von zufälligen Ladezeitpunkten sein.

## 8. Ressourcenpriorität

Bei mehrfach definierten Ressourcenschlüsseln gilt:

1. Lokale Ressourcen besitzen Vorrang vor übergeordneten Ressourcen.
2. Direkt definierte Ressourcen besitzen Vorrang vor zusammengeführten Dictionaries derselben Ebene.
3. Innerhalb zusammengeführter Dictionaries entscheidet eine definierte Reihenfolge.
4. Nicht gefundene Ressourcen werden über festgelegte Fallback-Regeln behandelt.

Eine Ressource DARF nicht aufgrund einer zufälligen Registrierungsreihenfolge ausgewählt werden.

## 9. Dynamische Ressourcenänderungen

Resource Dictionaries können während der Laufzeit aktualisiert werden.

Beispiele:

- Änderung der Akzentfarbe.
- Wechsel des Themes.
- Anpassung von Schriftgrößen.
- Aktualisierung von Icons.
- Änderung benutzerdefinierter Designwerte.

Dabei gilt:

- Änderungen werden über den Resource Dependency Tracker erfasst.
- Nur tatsächlich abhängige Eigenschaften werden aktualisiert.
- Mehrere Änderungen SOLLEN konsistent zusammengefasst werden.
- Ungültige Aktualisierungen DÜRFEN bestehende gültige Ressourcen nicht beschädigen.

Ressourcenänderungen MÜSSEN ohne vollständige Neuerstellung der Benutzeroberfläche möglich sein.

## 10. Ressourcenlebenszyklus

Ressourcen unterliegen der Verwaltung durch die Nova UI Runtime.

Dabei gilt:

- Ressourcen werden bedarfsgerecht geladen.
- Gemeinsam verwendbare Ressourcen SOLLEN geteilt werden.
- Nicht mehr benötigte Ressourcen werden freigegeben.
- Dynamische Referenzen werden beim Entfernen ihrer Besitzer gelöst.
- Ressourcenänderungen berücksichtigen den UI Component Lifecycle.

Die Lebensdauer einer Ressource MUSS unabhängig von einzelnen visuellen Control-Instanzen verwaltet werden können.

## 11. Unveränderlichkeit und gemeinsame Nutzung

Ressourcen können unveränderlich oder veränderbar sein.

Dabei gilt:

- Unveränderliche Ressourcen SOLLEN gemeinsam genutzt werden.
- Veränderbare Ressourcen benötigen eine definierte Besitz- und Änderungssemantik.
- Änderungen an gemeinsam genutzten Ressourcen MÜSSEN kontrolliert weitergegeben werden.
- Unbeabsichtigte Änderungen durch einzelne Controls MÜSSEN verhindert werden.

Das Resource System DARF keine unkontrollierte gemeinsame Veränderbarkeit voraussetzen.

## 12. Theme-Integration

Resource Dictionaries bilden eine Grundlage des Nova Theme Management.

Dabei gilt:

- Themes können eigene Dictionaries bereitstellen.
- Theme-Varianten können Ressourcen überschreiben.
- Dynamische Ressourcen ermöglichen Themewechsel zur Laufzeit.
- Fehlende Theme-Ressourcen werden über definierte Fallbacks aufgelöst.
- Benutzerdefinierte Ressourcen können innerhalb autorisierter Bereiche Vorrang erhalten.

Ein Themewechsel MUSS bestehende funktionale Control-Instanzen erhalten.

## 13. Performance und Caching

Nova Resource Dictionaries arbeitet bedarfsorientiert.

Dabei gilt:

- Ressourcenauflösungen SOLLEN zwischengespeichert werden.
- Cacheeinträge MÜSSEN ihren Gültigkeitsbereich berücksichtigen.
- Dynamische Änderungen invalidieren ausschließlich betroffene Einträge.
- Ressourcenabhängigkeiten SOLLEN effizient verwaltet werden.
- Nicht verwendete Ressourcen DÜRFEN keine permanenten Aktualisierungen verursachen.

Das System MUSS ohne kontinuierliche Ressourcenprüfung funktionieren.

## 14. NovaStudio-Integration

NovaStudio unterstützt die Erstellung und Bearbeitung von Resource Dictionaries.

Dazu gehören:

- Ressourcenübersicht.
- Erstellung und Bearbeitung von Ressourcen.
- Verwaltung von Ressourcenschlüsseln.
- Typprüfung.
- Anzeige von Ressourcenreferenzen.
- Verwaltung zusammengeführter Dictionaries.
- Erkennung von Konflikten und zyklischen Abhängigkeiten.
- Vorschau dynamischer Ressourcenänderungen.

Designer und Runtime MÜSSEN dieselben Ressourcenformate verwenden können.

## 15. Sicherheit

Nova Resource Dictionaries unterliegt Nova.Trust.

Dabei gilt:

- Ressourcenquellen MÜSSEN validiert werden.
- Ressourcenreferenzen MÜSSEN bestehende Zugriffsrechte einhalten.
- Dictionaries DÜRFEN keine unkontrollierte Codeausführung ermöglichen.
- Ressourcen DÜRFEN keine zusätzlichen Capabilities verleihen.
- Fehlerhafte Dictionaries DÜRFEN andere UI-Instanzen nicht beeinträchtigen.
- Externe Ressourcen DÜRFEN nur über autorisierte Zugriffswege geladen werden.

## 16. Normative Anforderungen

- **UI-001:** NovaOS MUSS Resource Dictionaries unterstützen.
- **UI-002:** Ressourcen MÜSSEN über eindeutige Schlüssel innerhalb ihres Gültigkeitsbereichs identifizierbar sein.
- **UI-003:** Ressourcen MÜSSEN typsicher aufgelöst werden.
- **UI-004:** Statische und dynamische Ressourcenreferenzen MÜSSEN unterstützt werden.
- **UI-005:** Ressourcen MÜSSEN in hierarchischen Gültigkeitsbereichen verwaltet werden können.
- **UI-006:** Mehrere Dictionaries MÜSSEN zusammengeführt werden können.
- **UI-007:** Ressourcenkonflikte MÜSSEN deterministisch aufgelöst werden.
- **UI-008:** Zyklische Ressourcen- und Dictionary-Abhängigkeiten MÜSSEN erkannt werden.
- **UI-009:** Dynamische Ressourcenänderungen MÜSSEN gezielt weitergegeben werden.
- **UI-010:** Ressourcenänderungen MÜSSEN ohne vollständige Neuerstellung der UI möglich sein.
- **UI-011:** Gemeinsam nutzbare Ressourcen SOLLEN wiederverwendet werden.
- **UI-012:** Ressourcen MÜSSEN kontrolliert freigegeben werden.
- **UI-013:** Resource Dictionaries MÜSSEN mit dem Nova Styling System zusammenarbeiten.
- **UI-014:** Resource Dictionaries MÜSSEN mit dem Nova Theme Management zusammenarbeiten.
- **UI-015:** Ressourcenänderungen MÜSSEN gezielte Layout- und Renderinvalidierungen auslösen.
- **UI-016:** Resource Dictionaries MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-017:** NovaStudio MUSS Resource Dictionaries bearbeiten und validieren können.
- **UI-018:** Fehlerhafte Ressourcen MÜSSEN kontrolliert behandelt werden.
- **UI-019:** Resource Dictionaries MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-020:** Nova Resource Dictionaries MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 17. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Runtime:** Ressourcenverwaltung und Lebenszyklus.
- **Nova Declarative UI Model:** Deklarative Ressourcendefinitionen.
- **Nova UI Property System:** Ressourcenbindung an Eigenschaften.
- **Nova Styling System:** Styles und Gestaltungseigenschaften.
- **Nova Theme Management:** Themes und Designvarianten.
- **Nova Control Templates:** Wiederverwendbare visuelle Strukturen.
- **Nova Control Framework:** Ressourcenverwendung in Controls.
- **Nova UI Data Binding:** Dynamische Eigenschaftsaktualisierung.
- **Nova Layout Engine:** Geometrische Auswirkungen.
- **Nova UI Renderer Abstraction:** Grafische Ressourcen.
- **NovaStudio:** Ressourceneditor und Validierung.
- **Nova.Trust:** Sicherheit und Isolation.

## 18. Ergebnis

NovaOS erhält ein natives, hierarchisches und erweiterbares Resource-Dictionary-System.

UI-Ressourcen können zentral definiert, gemeinsam verwendet und dynamisch aktualisiert werden.

Durch deterministische Ressourcenauflösung, gezielte Änderungsverfolgung und kontrollierte Ressourcenverwaltung bleiben Benutzeroberflächen konsistent, flexibel und ressourcenschonend.

**Grundsatz:** Eine Ressource wird einmal definiert, kann mehrfach verwendet werden und wird nur dort aktualisiert, wo ihre Änderung tatsächlich relevant ist.