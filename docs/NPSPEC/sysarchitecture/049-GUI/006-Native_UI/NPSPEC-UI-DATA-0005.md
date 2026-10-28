# NPSPEC-UI-DATA-0005 – Nova Data Validation

**Status:** Angenommen  
**Bereich:** UI / Data / Validation  
**System:** NovaOS

## 1. Zweck

Nova Data Validation definiert die einheitliche Prüfung von Benutzereingaben und gebundenen Daten innerhalb der nativen NovaOS-Benutzeroberfläche.

Ziel ist die frühzeitige Erkennung ungültiger Werte, die konsistente Weitergabe von Validierungsergebnissen und eine verständliche Rückmeldung an den Benutzer.

Die Validierung bleibt unabhängig von der visuellen Darstellung, dem Renderbackend und der konkreten Datenquelle.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Validation Engine | Koordination der Datenvalidierung |
| Validation Rule Registry | Verwaltung von Validierungsregeln |
| Validation Context | Bereitstellung des Prüfungskontexts |
| Validation Result | Einheitliches Validierungsergebnis |
| Validation State Manager | Verwaltung aktueller Validierungszustände |
| Validation Dependency Tracker | Erkennung abhängiger Validierungen |
| Validation Notification Manager | Weitergabe von Validierungsergebnissen |

Nova Data Validation integriert sich in Nova Data Binding und Nova Observable Properties.

## 3. Validierungsmodell

Validierungsregeln können auf unterschiedlichen Ebenen definiert werden:

- Einzelne Eigenschaften.
- Komplexe Datenobjekte.
- Zusammenhängende Eigenschaftsgruppen.
- Formulare.
- Benutzerdefinierte Datenmodelle.

Eine Validierungsregel besitzt:

- Eindeutige Identität.
- Definierten Gültigkeitsbereich.
- Prüfbedingung.
- Optionalen Fehlercode.
- Optionalen Meldungstext.
- Schweregrad.
- Optionalen Abhängigkeiten.

Validierungsregeln MÜSSEN unabhängig von UI-Controls ausführbar sein.

## 4. Validierungsarten

NovaOS unterstützt folgende Validierungsarten:

| Art | Beschreibung |
|---|---|
| Required | Prüfung auf erforderliche Werte |
| Type | Prüfung des Datentyps |
| Range | Prüfung zulässiger Wertebereiche |
| Length | Prüfung der Zeichen- oder Elementanzahl |
| Format | Prüfung definierter Datenformate |
| Comparison | Vergleich mehrerer Werte |
| Custom | Benutzerdefinierte Validierungslogik |
| Object | Prüfung zusammenhängender Objekteigenschaften |

Validierungsregeln DÜRFEN kombiniert werden.

Die Reihenfolge ihrer Ausführung MUSS deterministisch sein.

## 5. Deklarative Validierung

Validierungsregeln können im Nova Declarative UI Model definiert werden.

Konzeptionelles Beispiel:

```xml
<TextBox Text="{Binding UserName, Mode=TwoWay}">

    <TextBox.ValidationRules>

        <RequiredRule
            Message="Bitte einen Namen eingeben." />

        <LengthRule
            Minimum="3"
            Maximum="50"
            Message="Der Name muss 3 bis 50 Zeichen enthalten." />

    </TextBox.ValidationRules>

</TextBox>
```

Die konkrete Syntax wird durch das deklarative UI-Schema festgelegt.

Validierungsregeln MÜSSEN auch unabhängig von einer deklarativen UI-Definition verwendet werden können.

## 6. NovaLang-Integration

NovaLang unterstützt die Definition benutzerdefinierter Validierungsregeln.

Konzeptionelles Beispiel:

```vb
Public Class AgeValidator
    Inherits ValidationRule

    Public Overrides Function Validate(
        value As Integer
    ) As ValidationResult

        If value < 0 OrElse value > 120 Then
            Return ValidationResult.Error(
                "Das Alter muss zwischen 0 und 120 liegen."
            )
        End If

        Return ValidationResult.Success()

    End Function

End Class
```

Validierungsregeln MÜSSEN typsicher implementiert werden können.

Die konkrete Sprach- und API-Syntax wird durch NovaLang und das Nova Framework festgelegt.

## 7. Validierungszeitpunkte

Validierungen können zu unterschiedlichen Zeitpunkten ausgeführt werden.

| Zeitpunkt | Verhalten |
|---|---|
| OnChange | Prüfung bei Wertänderung |
| OnLostFocus | Prüfung beim Fokusverlust |
| OnCommit | Prüfung vor Übernahme eines Wertes |
| OnSubmit | Prüfung beim Absenden eines Formulars |
| Explicit | Prüfung durch ausdrücklichen Aufruf |

Der Validierungszeitpunkt MUSS konfigurierbar sein.

Eine Validierung DARF nicht unnötig bei jedem Renderdurchlauf ausgeführt werden.

## 8. Validierungsergebnisse

Ein Validierungsergebnis enthält mindestens:

- Gültigkeitsstatus.
- Optionalen Fehlercode.
- Optionalen Meldungstext.
- Schweregrad.
- Bezug zur betroffenen Eigenschaft oder zum Objekt.

Unterstützte Schweregrade:

- Information.
- Warning.
- Error.

Ein Warnhinweis MUSS von einem Fehler unterschieden werden können.

Ob ein Ergebnis die Datenübernahme verhindert, wird durch die jeweilige Validierungsrichtlinie bestimmt.

## 9. Validierungszustände

Datenobjekte und Controls können folgende Validierungszustände besitzen:

| Zustand | Bedeutung |
|---|---|
| Unvalidated | Noch nicht geprüft |
| Validating | Prüfung läuft |
| Valid | Prüfung erfolgreich |
| Warning | Prüfung mit Warnhinweisen |
| Invalid | Ungültige Daten |

Validierungszustände werden über Nova Observable Properties bereitgestellt.

Die visuelle Darstellung erfolgt über Nova Control States und Nova Visual State Styling.

## 10. Datenbindung und Werteübernahme

Nova Data Validation arbeitet mit Nova Data Binding zusammen.

Dabei gilt:

- Eingabewerte können vor der Übernahme geprüft werden.
- Konvertierungsfehler werden als Validierungsfehler bereitgestellt.
- Ungültige Werte DÜRFEN nicht entgegen der definierten Validierungsrichtlinie übernommen werden.
- TwoWay-Bindungen MÜSSEN Validierungsergebnisse berücksichtigen.
- Änderungen der Datenquelle können eine erneute Prüfung auslösen.

Ein ungültiger Eingabewert MUSS nicht automatisch aus dem Eingabefeld entfernt werden.

Der Benutzer SOLL fehlerhafte Eingaben korrigieren können, ohne sie vollständig erneut eingeben zu müssen.

## 11. Abhängige Validierung

Validierungsregeln können mehrere Eigenschaften berücksichtigen.

Beispiele:

- Passwort und Passwortbestätigung.
- Beginn und Ende eines Zeitraums.
- Mindestwert und Höchstwert.
- Abhängige Pflichtfelder.

Dabei gilt:

- Abhängigkeiten MÜSSEN definiert werden können.
- Änderungen relevanter Eigenschaften invalidieren betroffene Validierungsergebnisse.
- Unabhängige Regeln werden nicht unnötig erneut ausgeführt.
- Zyklische Abhängigkeiten MÜSSEN erkannt und kontrolliert behandelt werden.

## 12. Asynchrone Validierung

NovaOS unterstützt asynchrone Validierungsregeln.

Beispiele:

- Prüfung größerer Datenmengen.
- Zugriff auf lokale Datenbanken.
- Prüfung komplexer Dokumentstrukturen.
- Autorisierte externe Prüfungen.

Dabei gilt:

- Asynchrone Validierungen DÜRFEN die UI Event Loop nicht blockieren.
- Laufende Prüfungen MÜSSEN einen definierten Status besitzen.
- Veraltete Ergebnisse DÜRFEN neuere Eingabewerte nicht überschreiben.
- Nicht mehr benötigte Prüfungen MÜSSEN abgebrochen oder entkoppelt werden können.
- Fehler während der Prüfung MÜSSEN kontrolliert behandelt werden.

Ein technischer Prüfungsfehler DARF nicht automatisch als fachlich gültiges Ergebnis interpretiert werden.

## 13. Fehlerdarstellung

Nova Data Validation definiert keine feste visuelle Fehlerdarstellung.

Controls können Validierungsinformationen beispielsweise darstellen durch:

- Rahmenfarben.
- Fehlersymbole.
- Hinweistexte.
- Tooltips.
- Zusammenfassungen.
- Statusanzeigen.

Dabei gilt:

- Die Darstellung erfolgt über Styles und Control Templates.
- Fehlermeldungen MÜSSEN verständlich bereitgestellt werden können.
- Barrierefreiheitseinstellungen MÜSSEN berücksichtigt werden.
- Fehler DÜRFEN nicht ausschließlich durch Farben vermittelt werden.

Die Validierungslogik bleibt unabhängig von ihrer Darstellung.

## 14. Formularvalidierung

Mehrere Validierungsregeln können zu einem gemeinsamen Validierungskontext zusammengefasst werden.

Dabei gilt:

- Formulare können ihren Gesamtvalidierungsstatus bereitstellen.
- Fehler einzelner Eigenschaften können gesammelt werden.
- Die Ausführung eines Submit-Commands kann vom Validierungsergebnis abhängen.
- Die erste ungültige Eingabe kann gezielt fokussiert werden.
- Bereits gültige Werte SOLLEN nicht unnötig erneut geprüft werden.

Die Entscheidung über die tatsächliche Ausführung einer Operation verbleibt beim zuständigen Command beziehungsweise der Anwendungslogik.

## 15. Performance

Nova Data Validation arbeitet ereignisgesteuert und bedarfsorientiert.

Dabei gilt:

- Validierungen werden ausschließlich bei relevanten Änderungen ausgelöst.
- Unveränderte Ergebnisse SOLLEN wiederverwendet werden.
- Mehrere Prüfanforderungen SOLLEN zusammengefasst werden können.
- Aufwendige Prüfungen SOLLEN asynchron ausgeführt werden.
- Nicht mehr benötigte Validierungsressourcen werden freigegeben.

Eine permanente Überprüfung sämtlicher Daten ist nicht erforderlich.

## 16. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Visuelle Erstellung von Validierungsregeln.
- Zuordnung von Regeln zu Eigenschaften.
- Konfiguration von Wertebereichen.
- Bearbeitung von Fehlermeldungen.
- Definition abhängiger Validierungen.
- Vorschau von Validierungszuständen.
- Diagnose fehlerhafter Regeln.
- Erstellung benutzerdefinierter Validierungen im Logic Graph.

Validierungsregeln MÜSSEN über gemeinsame Metadaten in NovaLang und Logic Graph verfügbar sein.

## 17. Sicherheit

Nova Data Validation unterliegt Nova.Trust.

Dabei gilt:

- Validierungsregeln DÜRFEN nur auf autorisierte Daten zugreifen.
- Benutzerdefinierte Prüfungen MÜSSEN innerhalb ihres Capability-Kontexts ausgeführt werden.
- Fehlermeldungen DÜRFEN keine geschützten Informationen offenlegen.
- Validierungsregeln DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Fehlerhafte Regeln DÜRFEN andere Sicherheitskontexte nicht beeinträchtigen.

UI-Validierung ersetzt keine erforderliche Prüfung an der tatsächlichen Ausführungs- oder Vertrauensgrenze.

## 18. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein einheitliches Data-Validation-System bereitstellen.
- **UI-002:** Validierungsregeln MÜSSEN unabhängig von UI-Controls definiert werden können.
- **UI-003:** Validierungen MÜSSEN deklarativ und programmatisch definiert werden können.
- **UI-004:** Standardisierte und benutzerdefinierte Validierungsregeln MÜSSEN unterstützt werden.
- **UI-005:** Validierungsregeln MÜSSEN typsicher verarbeitet werden.
- **UI-006:** Validierungszeitpunkte MÜSSEN konfigurierbar sein.
- **UI-007:** Validierungsergebnisse MÜSSEN standardisiert bereitgestellt werden.
- **UI-008:** Unterschiedliche Schweregrade MÜSSEN unterstützt werden.
- **UI-009:** Validierungszustände MÜSSEN beobachtbar sein.
- **UI-010:** Nova Data Binding MUSS Validierungsergebnisse berücksichtigen können.
- **UI-011:** Abhängige Validierungen MÜSSEN unterstützt werden.
- **UI-012:** Asynchrone Validierungen MÜSSEN möglich sein.
- **UI-013:** Veraltete Validierungsergebnisse DÜRFEN aktuelle Ergebnisse nicht überschreiben.
- **UI-014:** Validierungsfehler MÜSSEN unabhängig von ihrer visuellen Darstellung verwaltet werden.
- **UI-015:** Formularvalidierungen MÜSSEN unterstützt werden.
- **UI-016:** Validierungen MÜSSEN ereignisgesteuert ausgeführt werden können.
- **UI-017:** NovaStudio MUSS Validierungsregeln bearbeiten und prüfen können.
- **UI-018:** Logic Graph MUSS Validierungsregeln verwenden können.
- **UI-019:** Nova Data Validation MUSS unabhängig vom Renderbackend funktionieren.
- **UI-020:** Validierungen MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-021:** Nova Data Validation MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 19. Integration

- **Nova Data Binding:** Validierung gebundener Werte.
- **Nova Observable Properties:** Änderungsbenachrichtigungen und Validierungszustände.
- **Nova Commands and Actions:** Prüfung vor der Ausführung von Aktionen.
- **Nova UI Property System:** Validierbare Eigenschaften.
- **Nova Control Framework:** Eingabe- und Formularcontrols.
- **Nova Control States:** Funktionale Validierungszustände.
- **Nova Visual State Styling:** Darstellung von Fehlern und Warnungen.
- **Nova UI Event System:** Auslösung von Validierungen.
- **Nova UI Threading Model:** Asynchrone Prüfungen.
- **Nova UI Accessibility:** Zugängliche Fehlerrückmeldungen.
- **NovaLang:** Benutzerdefinierte Validierungslogik.
- **NovaStudio:** Visueller Validierungseditor.
- **Nova Logic Graph:** Grafische Validierungsregeln.
- **Nova.Trust:** Berechtigungen und Isolation.

## 20. Ergebnis

NovaOS erhält ein einheitliches, typsicheres und ereignisgesteuertes System zur Validierung von Daten.

Benutzereingaben und Datenmodelle können anhand wiederverwendbarer Regeln geprüft werden, während Fehlerzustände automatisch an die Benutzeroberfläche weitergegeben werden.

Durch abhängige und asynchrone Validierungen, gezielte Aktualisierungen und eine klare Trennung von Logik und Darstellung bleibt das System flexibel und ressourcenschonend.

**Grundsatz:** Nova Data Validation prüft Daten dort, wo ihre Gültigkeit entscheidend ist, und informiert den Benutzer dort, wo er Fehler sinnvoll korrigieren kann.