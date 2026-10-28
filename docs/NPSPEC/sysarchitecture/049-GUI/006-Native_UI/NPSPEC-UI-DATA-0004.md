# NPSPEC-UI-DATA-0004 – Nova Commands and Actions

**Status:** Angenommen  
**Bereich:** UI / Data / Commands and Actions  
**System:** NovaOS

## 1. Zweck

Nova Commands and Actions definiert ein einheitliches System zur Ausführung von Benutzeraktionen und Anwendungsbefehlen.

Ziel ist die Trennung zwischen Benutzerinteraktion, Befehlsdefinition und tatsächlicher Ausführung.

Befehle können unabhängig vom auslösenden UI-Element verwendet und über NovaLang, NovaStudio oder Logic Graph implementiert werden.

Das System arbeitet ereignisgesteuert, typsicher und unabhängig vom Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Command Framework | Einheitliches Befehlsmodell |
| Command Registry | Registrierung und Verwaltung von Befehlen |
| Command Binding Engine | Verbindung zwischen UI-Elementen und Befehlen |
| Command Dispatcher | Weiterleitung von Ausführungsanforderungen |
| Command State Manager | Verwaltung der Befehlsverfügbarkeit |
| Action Executor | Ausführung konkreter Aktionen |
| Command Context Manager | Bereitstellung des Ausführungskontexts |

Die Ausführung erfolgt innerhalb der bestehenden NovaOS-Sicherheits- und Threading-Architektur.

## 3. Commands und Actions

NovaOS unterscheidet zwischen Commands und Actions.

| Element | Bedeutung |
|---|---|
| Command | Beschreibt eine ausführbare Benutzerabsicht |
| Action | Implementiert eine konkrete Operation |
| Command Binding | Verbindet ein UI-Element mit einem Command |
| Command Context | Enthält die für die Ausführung benötigten Informationen |

Ein Command kann durch unterschiedliche UI-Elemente ausgelöst werden.

Eine Action kann von mehreren Commands verwendet werden.

Commands DÜRFEN aus mehreren Actions zusammengesetzt werden.

## 4. Command-Modell

Ein Command besitzt:

- Eindeutige Identität.
- Definierten Namen.
- Optionalen Parameter.
- Ausführungsfunktion.
- Verfügbarkeitsprüfung.
- Ausführungsstatus.
- Optionalen Rückgabewert.
- Optionalen Abbruchmechanismus.

Commands MÜSSEN unabhängig von ihrer visuellen Darstellung definiert werden können.

Die Verfügbarkeit eines Commands MUSS vor der Ausführung geprüft werden.

## 5. Deklarative Command Bindings

UI-Elemente können Commands über Nova Data Binding verwenden.

Beispiel:

```xml
<Button
    Content="Speichern"
    Command="{Binding SaveCommand}" />
```

Commands können Parameter erhalten:

```xml
<Button
    Content="Löschen"
    Command="{Binding DeleteCommand}"
    CommandParameter="{Binding SelectedItem}" />
```

Die UI muss keine Kenntnis über die konkrete Implementierung des Commands besitzen.

## 6. NovaLang-Integration

Commands können direkt in NovaLang implementiert werden.

Konzeptionelles Beispiel:

```vb
Public Class DocumentViewModel

    Public Command SaveCommand As New Command(
        AddressOf SaveDocument,
        AddressOf CanSaveDocument
    )

    Private Sub SaveDocument()
        Document.Save()
    End Sub

    Private Function CanSaveDocument() As Boolean
        Return Document.IsModified
    End Function

End Class
```

Die konkrete Sprachsyntax wird durch die NovaLang-Spezifikation festgelegt.

NovaLang MUSS Commands typsicher definieren und ausführen können.

## 7. Command-Verfügbarkeit

Commands können abhängig vom aktuellen Anwendungszustand verfügbar oder gesperrt sein.

Beispiele:

- Speichern nur bei geänderten Daten.
- Kopieren nur bei vorhandener Auswahl.
- Rückgängig nur bei verfügbaren Änderungen.
- Drucken nur bei verfügbarem Druckdienst.

Dabei gilt:

- Commands stellen ihre Verfügbarkeit über eine einheitliche Schnittstelle bereit.
- Änderungen der Verfügbarkeit werden ereignisgesteuert gemeldet.
- Gebundene UI-Elemente können ihren Zustand automatisch aktualisieren.
- Gesperrte Commands DÜRFEN nicht durch die UI-Bindung ausgeführt werden.

Die Verfügbarkeitsprüfung ersetzt keine Sicherheitsprüfung.

## 8. Asynchrone Commands

Commands können asynchrone Operationen ausführen.

Dabei gilt:

- Die UI Event Loop DARF nicht blockiert werden.
- Commands können Fortschritt und Ausführungsstatus bereitstellen.
- Laufende Operationen MÜSSEN kontrolliert abgebrochen werden können, sofern die Operation einen Abbruch unterstützt.
- Fehler MÜSSEN über definierte Schnittstellen gemeldet werden.
- Mehrfachausführungen MÜSSEN kontrolliert werden.

Ein Command kann während seiner Ausführung weitere Aufrufe verhindern oder ausdrücklich parallele Ausführungen erlauben.

## 9. Command-Zustände

Ein Command kann folgende Zustände besitzen:

| Zustand | Bedeutung |
|---|---|
| Ready | Ausführungsbereit |
| Disabled | Aktuell nicht ausführbar |
| Running | Wird ausgeführt |
| Completed | Erfolgreich abgeschlossen |
| Failed | Fehlerhaft abgeschlossen |
| Cancelled | Kontrolliert abgebrochen |

Zustandsänderungen werden über Nova Observable Properties bereitgestellt.

UI-Elemente können diese Zustände über Nova Visual State Styling darstellen.

## 10. Command Routing

Commands können innerhalb definierter UI-Kontexte weitergeleitet werden.

Mögliche Ziele:

- Aktives Control.
- Aktives Fenster.
- Aktuelle Anwendung.
- Registrierter Command Handler.

Dabei gilt:

- Die Zielermittlung MUSS deterministisch erfolgen.
- Der Ausführungskontext MUSS eindeutig bestimmt werden.
- Nicht behandelte Commands MÜSSEN kontrolliert zurückgewiesen werden.
- Command Routing DARF keine Sicherheitsgrenzen umgehen.

Die Weiterleitung orientiert sich an der logischen UI-Hierarchie und den registrierten Command Handlern.

## 11. Zusammengesetzte Actions

Actions können zu komplexeren Abläufen kombiniert werden.

Unterstützte Strukturen können sein:

- Sequenzielle Ausführung.
- Bedingte Ausführung.
- Parallele Ausführung.
- Wiederholungen.
- Fehlerbehandlung.
- Rücknahme unterstützter Operationen.

Zusammengesetzte Actions MÜSSEN die Regeln der NovaOS Structured Concurrency einhalten.

Eine Action DARF nur die Capabilities verwenden, für die ihr Ausführungskontext autorisiert ist.

## 12. Undo und Redo

Commands können optional rückgängig gemacht oder wiederholt werden.

Dabei gilt:

- Rücknehmbare Commands MÜSSEN ihre erforderlichen Zustandsinformationen bereitstellen.
- Undo und Redo werden über definierte Schnittstellen ausgeführt.
- Nicht rücknehmbare Commands MÜSSEN entsprechend gekennzeichnet werden.
- Die Rücknahme DARF keine Sicherheits- oder Konsistenzregeln verletzen.

Die Verwaltung der Undo-/Redo-Historie erfolgt durch das zuständige Transaktions- beziehungsweise History-System.

## 13. Tastatur und alternative Eingaben

Commands können durch unterschiedliche Eingabemethoden ausgelöst werden.

Beispiele:

- Mausklick.
- Tastaturkürzel.
- Touch.
- Menüeintrag.
- Ribbon-Control.
- Kontextmenü.
- Barrierefreie Eingabemethoden.

Alle Eingabemethoden SOLLEN denselben Command verwenden können.

Die konkrete Eingabemethode DARF die funktionale Implementierung des Commands nicht verändern.

## 14. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Visuelle Erstellung von Commands.
- Definition von Parametern.
- Verbindung mit UI-Elementen.
- Bearbeitung von Verfügbarkeitsbedingungen.
- Darstellung von Ausführungszuständen.
- Erstellung zusammengesetzter Actions.
- Verbindung mit Logic Graph.
- Debugging und Fehlerdiagnose.

Commands und Actions MÜSSEN über gemeinsame Metadaten in NovaLang und Logic Graph verfügbar sein.

Eine Änderung der Implementierungsdarstellung DARF die funktionale Bedeutung eines Commands nicht verändern.

## 15. Sicherheit

Nova Commands and Actions unterliegt Nova.Trust.

Dabei gilt:

- Commands DÜRFEN keine Berechtigungen selbstständig erweitern.
- Actions MÜSSEN innerhalb ihres autorisierten Capability-Kontexts ausgeführt werden.
- Command-Parameter MÜSSEN validiert werden.
- Sicherheitsrelevante Operationen MÜSSEN ihre Berechtigungen unmittelbar vor der Ausführung prüfen.
- Command Routing DARF keine Capability-Grenzen umgehen.
- Fehlerhafte Actions DÜRFEN andere Sicherheitskontexte nicht beeinträchtigen.

Die Ausführung eines Commands stellt keine automatische Autorisierung der zugrunde liegenden Operation dar.

## 16. Performance

Nova Commands and Actions arbeitet ereignisgesteuert.

Dabei gilt:

- Command-Verfügbarkeit wird bei relevanten Zustandsänderungen aktualisiert.
- Unveränderte Zustände verursachen keine unnötigen UI-Aktualisierungen.
- Command Bindings SOLLEN wiederverwendet werden.
- Asynchrone Operationen DÜRFEN die UI nicht blockieren.
- Nicht benötigte Command Bindings werden kontrolliert freigegeben.

Eine permanente Überprüfung aller Commands ist nicht erforderlich.

## 17. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein einheitliches Command- und Action-System bereitstellen.
- **UI-002:** Commands MÜSSEN unabhängig von UI-Elementen definiert werden können.
- **UI-003:** UI-Elemente MÜSSEN deklarativ an Commands gebunden werden können.
- **UI-004:** Commands MÜSSEN typsichere Parameter unterstützen.
- **UI-005:** Command-Verfügbarkeit MUSS dynamisch ermittelt werden können.
- **UI-006:** Änderungen der Verfügbarkeit MÜSSEN ereignisgesteuert gemeldet werden.
- **UI-007:** Commands MÜSSEN synchron und asynchron ausgeführt werden können.
- **UI-008:** Asynchrone Commands DÜRFEN die UI Event Loop nicht blockieren.
- **UI-009:** Command-Ausführungszustände MÜSSEN beobachtbar sein.
- **UI-010:** Command Routing MUSS deterministisch erfolgen.
- **UI-011:** Mehrfachausführungen MÜSSEN kontrolliert werden können.
- **UI-012:** Commands MÜSSEN über unterschiedliche Eingabemethoden ausgelöst werden können.
- **UI-013:** Zusammengesetzte Actions MÜSSEN unterstützt werden können.
- **UI-014:** Rücknehmbare Commands MÜSSEN Undo und Redo unterstützen können.
- **UI-015:** Commands MÜSSEN mit Nova Data Binding zusammenarbeiten.
- **UI-016:** Commands MÜSSEN mit Nova Observable Properties zusammenarbeiten.
- **UI-017:** NovaLang MUSS Commands und Actions definieren können.
- **UI-018:** NovaStudio MUSS Commands visuell bearbeiten können.
- **UI-019:** Logic Graph MUSS Commands und Actions verwenden können.
- **UI-020:** Commands MÜSSEN unabhängig vom Renderbackend funktionieren.
- **UI-021:** Commands und Actions MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-022:** Nova Commands and Actions MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 18. Integration

- **Nova UI Runtime:** Verwaltung und Ausführung von Commands.
- **Nova Declarative UI Model:** Deklarative Command Bindings.
- **Nova Data Binding:** Verbindung zwischen Commands und UI.
- **Nova Observable Properties:** Verfügbarkeit und Ausführungszustände.
- **Nova Control Framework:** Command-fähige Controls.
- **Nova Control States:** Darstellung funktionaler Zustände.
- **Nova Visual State Styling:** Visuelle Command-Zustände.
- **Nova UI Event System:** Ereignisgesteuerte Auslösung.
- **Nova UI Threading Model:** Synchronisation und asynchrone Ausführung.
- **Nova UI Component Lifecycle:** Verwaltung von Command Bindings.
- **NovaLang:** Programmatische Command-Implementierung.
- **NovaStudio:** Visueller Command- und Action-Editor.
- **Nova Logic Graph:** Grafische Implementierung von Actions.
- **Nova Capability System:** Autorisierte Operationen.
- **Nova.Trust:** Berechtigungen und Isolation.

## 19. Ergebnis

NovaOS erhält ein einheitliches, typsicheres und ereignisgesteuertes System zur Definition und Ausführung von Benutzeraktionen.

Commands können unabhängig von ihrer visuellen Darstellung verwendet werden und verbinden klassische Programmierung mit dem Nova Logic Graph.

Durch dynamische Verfügbarkeitsprüfung, asynchrone Ausführung und Capability-basierte Sicherheit bleiben Benutzeraktionen konsistent, flexibel und kontrollierbar.

**Grundsatz:** Ein Command beschreibt, was der Benutzer erreichen möchte. Eine Action bestimmt, wie diese Absicht umgesetzt wird.