# NPSPEC-UI-CONTROL-0003 – Nova Custom Controls

**Status:** Angenommen  
**Bereich:** UI / Controls / Custom Controls  
**System:** NovaOS

## 1. Zweck

Nova Custom Controls definiert die Erstellung, Erweiterung und Integration benutzerdefinierter UI-Steuerelemente innerhalb von NovaOS.

Ziel ist ein einheitliches Erweiterungsmodell, mit dem Entwickler eigene Controls erstellen können, ohne grundlegende Funktionen wie Layout, Rendering, Eingabeverarbeitung oder Datenbindung neu implementieren zu müssen.

Custom Controls basieren auf dem Nova Control Framework und können in NovaLang, NovaStudio und Logic Graph verwendet werden.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Custom Control Base | Grundlage benutzerdefinierter Controls |
| Control Type Registry | Registrierung und Auflösung von Control-Typen |
| Control Metadata Provider | Bereitstellung von Eigenschaften, Ereignissen und Designerinformationen |
| Control Template Resolver | Zuordnung visueller Templates |
| Control Behavior Manager | Verwaltung benutzerdefinierter Verhaltensweisen |
| Control Resource Manager | Verwaltung benötigter Ressourcen |

Custom Controls verwenden die bestehenden Nova UI Subsysteme.

Eine eigene Layout- oder Rendering-Infrastruktur ist nicht erforderlich.

## 3. Control-Arten

NovaOS unterstützt drei grundlegende Erweiterungsarten.

| Typ | Beschreibung |
|---|---|
| Extended Control | Erweiterung eines vorhandenen Controls |
| Composite Control | Zusammensetzung mehrerer vorhandener UI-Elemente |
| Custom Control | Eigenständiges Control mit eigenen Eigenschaften und eigenem Verhalten |

### Extended Control

Ein vorhandenes Control wird durch zusätzliche Eigenschaften, Ereignisse oder Funktionen erweitert.

### Composite Control

Mehrere vorhandene Controls werden zu einer wiederverwendbaren Komponente zusammengefasst.

### Custom Control

Ein eigenständiger Control-Typ definiert sein Verhalten und seine öffentliche Schnittstelle.

Die visuelle Struktur kann über Templates oder eigene Renderfunktionen bereitgestellt werden.

## 4. Control-Definition

Ein Custom Control kann folgende Bestandteile besitzen:

- Eindeutigen Typnamen.
- Basisklasse.
- Eigenschaften.
- Ereignisse.
- Methoden.
- Zustände.
- Standard-Style.
- Control Template.
- Datenbindungen.
- Designer-Metadaten.
- Ressourcen.

Die öffentliche Schnittstelle MUSS eindeutig definiert sein.

Interne Implementierungsdetails SOLLEN von der öffentlichen Control-Schnittstelle getrennt bleiben.

## 5. NovaLang-Implementierung

Custom Controls können in NovaLang implementiert werden.

Beispiel:

```vb
Public Class StatusIndicator
    Inherits Control

    Public Property StatusText As String
    Public Property IsActive As Boolean

    Public Event StatusChanged()

    Public Sub SetStatus(active As Boolean)
        If IsActive <> active Then
            IsActive = active
            RaiseEvent StatusChanged()
        End If
    End Sub

End Class
```

Die tatsächliche Registrierung von Eigenschaften und Ereignissen erfolgt über die Mechanismen des Nova Control Frameworks.

NovaLang MUSS die erforderlichen Typinformationen für Runtime und Designer bereitstellen können.

## 6. Deklarative Verwendung

Registrierte Custom Controls können im Nova Declarative UI Model verwendet werden.

Beispiel:

```xml
<Window xmlns:custom="Nova.Controls.Custom">

    <StackPanel Spacing="12">

        <custom:StatusIndicator
            Name="SystemStatus"
            StatusText="System bereit"
            IsActive="True" />

    </StackPanel>

</Window>
```

Die deklarative Verwendung MUSS denselben Regeln wie bei Standard Controls folgen.

Unbekannte oder nicht verfügbare Control-Typen MÜSSEN kontrolliert behandelt werden.

## 7. Eigenschaften und Datenbindung

Custom Controls verwenden das Nova UI Property System.

Eigenschaften können unterstützen:

- Standardwerte.
- Typprüfung.
- Validierung.
- Änderungsbenachrichtigungen.
- Datenbindung.
- Layoutinvalidierung.
- Renderinvalidierung.
- Designer-Metadaten.

Eigenschaften MÜSSEN ihre Auswirkungen auf Layout und Darstellung korrekt deklarieren.

Datenbindungen DÜRFEN keine Sicherheits- oder Capability-Grenzen umgehen.

## 8. Ereignisse und Verhalten

Custom Controls können eigene Ereignisse und Verhaltensweisen definieren.

Dabei gilt:

- Ereignisse werden über das Nova UI Event System verarbeitet.
- Ereignishandler können in NovaLang implementiert werden.
- Ereignisse können mit Logic Graph verbunden werden.
- Dynamische Ereignisbindungen werden unterstützt.
- Ereignisbindungen werden beim Entfernen eines Controls kontrolliert freigegeben.

Custom Controls DÜRFEN wiederverwendbare Behaviors bereitstellen.

## 9. Styles und Templates

Custom Controls unterstützen das Nova UI Style and Template System.

Dabei gilt:

- Ein Control kann ein Standard-Template besitzen.
- Templates können ausgetauscht werden.
- Styles können Eigenschaften und visuelle Zustände definieren.
- Funktionales Verhalten bleibt unabhängig vom Template.
- Templatewechsel SOLLEN die bestehende Control-Instanz erhalten.

Ein Custom Control DARF eigene visuelle Elemente verwenden, ohne die öffentliche Control-Schnittstelle zu verändern.

## 10. Layout und Rendering

Custom Controls verwenden die Nova Layout Engine.

Dabei gilt:

- Controls nehmen am Measure- und Arrange-Verfahren teil.
- Eigene Größenanforderungen können definiert werden.
- Layoutcontainer können innerhalb eines Controls verwendet werden.
- Eigene Renderoperationen erfolgen ausschließlich über die Nova UI Renderer Abstraction.
- Änderungen lösen gezielte Layout- oder Renderinvalidierungen aus.

Direkte Zugriffe auf Framebuffer, GPU oder Compositor DÜRFEN nicht vorausgesetzt werden.

Unveränderte Controls DÜRFEN keine unnötigen Zeichenoperationen verursachen.

## 11. Lebenszyklus

Custom Controls verwenden den Nova UI Component Lifecycle.

Dabei werden insbesondere folgende Zustände berücksichtigt:

- Created.
- Initializing.
- Initialized.
- Attached.
- Active.
- Suspended.
- Detached.
- Disposing.
- Disposed.

Custom Controls MÜSSEN ihre Ressourcen entsprechend dem Lebenszyklus verwalten.

Asynchrone Aufgaben, Ereignisbindungen und temporäre Ressourcen MÜSSEN bei Bedarf beendet oder freigegeben werden.

Ein erneutes Anhängen eines Controls MUSS möglich sein, sofern dessen Lebenszyklus dies erlaubt.

## 12. NovaStudio-Integration

Custom Controls können im NovaStudio UI Designer verwendet werden.

Dazu stellt das Framework Metadaten bereit für:

- Toolbox-Einträge.
- Eigenschaften.
- Ereignisse.
- Methoden.
- Standardwerte.
- Kategorien.
- Beschreibungen.
- Designer-Vorschau.

Custom Controls SOLLEN ohne zusätzliche Designerimplementierung grundlegende Bearbeitungsfunktionen unterstützen.

Erweiterte Designerfunktionen DÜRFEN optional bereitgestellt werden.

Die Designer-Vorschau MUSS innerhalb eines kontrollierten Ausführungskontexts erfolgen.

## 13. Logic-Graph-Integration

Öffentliche Eigenschaften, Methoden und Ereignisse können im Nova Logic Graph verfügbar gemacht werden.

Dabei gilt:

- Eigenschaften können als lesbare oder schreibbare Nodes bereitgestellt werden.
- Methoden können als Funktions-Nodes verwendet werden.
- Ereignisse können als Event-Nodes dienen.
- Datentypen müssen mit dem NovaLang-Typsystem übereinstimmen.
- Berechtigungen werden nicht durch die grafische Darstellung eines Nodes erweitert.

Die Code- und Graph-Darstellung SOLLEN dieselbe öffentliche Control-Schnittstelle verwenden.

## 14. Registrierung und Wiederverwendung

Custom Controls werden über die Control Type Registry verfügbar gemacht.

Eine Registrierung enthält mindestens:

- Eindeutige Typidentität.
- Typname.
- Version.
- Öffentliche Schnittstelle.
- Erforderliche Abhängigkeiten.
- Standardressourcen.

Controls können innerhalb von Anwendungen oder wiederverwendbaren Bibliotheken bereitgestellt werden.

Die Registrierung MUSS Typkonflikte und inkompatible Versionen kontrolliert behandeln.

Die Verwendung eines Controls DARF keine globale Installation voraussetzen, sofern es innerhalb des Anwendungskontexts bereitgestellt wird.

## 15. Sicherheit und Isolation

Custom Controls unterliegen Nova.Trust.

Dabei gilt:

- Controls erhalten keine impliziten Systemberechtigungen.
- Geschützte Funktionen werden ausschließlich über autorisierte Capabilities aufgerufen.
- Benutzerdefinierter Code wird entsprechend seinem Vertrauens- und Ausführungskontext behandelt.
- Control-Metadaten und Ressourcen MÜSSEN validiert werden.
- Nicht vertrauenswürdige Controls DÜRFEN keine unkontrollierte Codeausführung im privilegierten UI-Kontext erhalten.
- Fehler einzelner Controls DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

Die Integration eines Controls in NovaStudio DARF dessen Berechtigungen nicht erweitern.

## 16. Performance und Ressourcen

Custom Controls MÜSSEN die Ressourcenregeln der Nova UI Runtime einhalten.

Dabei gilt:

- Ressourcen werden bedarfsgerecht erstellt.
- Gemeinsam nutzbare Styles und Templates SOLLEN wiederverwendet werden.
- Änderungen erzeugen gezielte Invalidierungen.
- Unveränderte Controls verursachen keine unnötigen Renderoperationen.
- Hintergrundaufgaben werden über das Nova UI Threading Model koordiniert.
- Ressourcen werden beim Entfernen oder Entsorgen kontrolliert freigegeben.

Custom Controls DÜRFEN keine permanente Aktualisierungsschleife benötigen.

## 17. Normative Anforderungen

- **UI-001:** NovaOS MUSS benutzerdefinierte Controls unterstützen.
- **UI-002:** Custom Controls MÜSSEN auf dem Nova Control Framework basieren.
- **UI-003:** Erweiterte, zusammengesetzte und eigenständige Controls MÜSSEN möglich sein.
- **UI-004:** Custom Controls MÜSSEN eigene Eigenschaften, Methoden und Ereignisse definieren können.
- **UI-005:** Custom Controls MÜSSEN das Nova UI Property System verwenden.
- **UI-006:** Datenbindungen MÜSSEN unterstützt werden.
- **UI-007:** Styles und Control Templates MÜSSEN unterstützt werden.
- **UI-008:** Custom Controls MÜSSEN deklarativ verwendbar sein.
- **UI-009:** Custom Controls MÜSSEN aus NovaLang verwendbar sein.
- **UI-010:** Öffentliche Control-Funktionen MÜSSEN für Logic Graph bereitgestellt werden können.
- **UI-011:** Custom Controls MÜSSEN mit der Nova Layout Engine zusammenarbeiten.
- **UI-012:** Eigene Renderoperationen MÜSSEN über die Renderer Abstraction erfolgen.
- **UI-013:** Custom Controls MÜSSEN den Nova UI Component Lifecycle einhalten.
- **UI-014:** Control-Typen MÜSSEN registriert und eindeutig aufgelöst werden können.
- **UI-015:** Custom Controls MÜSSEN grundlegende Designer-Metadaten bereitstellen können.
- **UI-016:** Änderungen MÜSSEN gezielte Layout- und Renderinvalidierungen auslösen.
- **UI-017:** Custom Controls MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-018:** Fehlerhafte Controls MÜSSEN kontrolliert behandelt werden.
- **UI-019:** Custom Controls MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-020:** Nova Custom Controls MÜSSEN vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 18. Integration

- **Nova Control Framework:** Gemeinsame Control-Architektur.
- **Nova Standard Controls:** Wiederverwendbare Basiskomponenten.
- **Nova UI Runtime:** Verwaltung der Control-Instanzen.
- **Nova UI Property System:** Eigenschaften und Änderungsbenachrichtigungen.
- **Nova UI Event System:** Ereignisverarbeitung.
- **Nova UI Data Binding:** Verbindung mit Datenquellen.
- **Nova Layout Engine:** Größen- und Positionsberechnung.
- **Nova UI Style Engine:** Erscheinungsbild.
- **Nova UI Template Engine:** Visuelle Struktur.
- **Nova UI Renderer Abstraction:** Grafische Darstellung.
- **Nova UI Component Lifecycle:** Lebenszyklus und Ressourcenverwaltung.
- **Nova UI Threading Model:** Ausführung und Synchronisation.
- **Nova UI Accessibility:** Barrierefreiheit.
- **NovaLang:** Implementierung und Verwendung.
- **NovaStudio:** Visueller Designer.
- **Nova Logic Graph:** Visuelle Programmierung.
- **Nova Framework:** Wiederverwendbare Control-Bibliotheken.
- **Nova.Trust:** Berechtigungen und Isolation.

## 19. Ergebnis

NovaOS erhält ein einheitliches Erweiterungsmodell für benutzerdefinierte UI-Steuerelemente.

Entwickler können vorhandene Controls erweitern, mehrere Komponenten kombinieren oder vollständig eigene Steuerelemente implementieren.

Alle Custom Controls integrieren sich in die bestehenden Systeme für Eigenschaften, Ereignisse, Datenbindung, Layout, Rendering und visuelle Programmierung.

**Grundsatz:** Ein Nova Custom Control erweitert die Möglichkeiten der Benutzeroberfläche, ohne deren grundlegende Architektur neu implementieren zu müssen.