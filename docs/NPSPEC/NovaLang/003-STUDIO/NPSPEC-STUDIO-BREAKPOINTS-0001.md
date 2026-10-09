
# NPSPEC-STUDIO-BREAKPOINTS-0001 – NovaLang Studio Breakpoints

## Status

Angenommen

## Kategorie

NovaLang Studio / Debugging / Breakpoints

## Zweck

Definiert die Verwaltung und Ausführung von Breakpoints innerhalb von NovaLang Studio.

Ziel ist das gezielte Anhalten und Untersuchen von NovaLang-Programmen, Tasks, Solutions und Logic-Graph-Ausführungen.

Breakpoints müssen zuverlässig, ressourcenschonend und unabhängig vom verwendeten Ausführungsmodus funktionieren.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Breakpoint Manager | Zentrale Verwaltung aller Breakpoints |
| Breakpoint Resolver | Zuordnung zu ausführbaren Positionen |
| Condition Evaluator | Prüfung bedingter Haltepunkte |
| Hit Counter | Verwaltung von Trefferzählern |
| Execution Controller | Kontrolliertes Anhalten der Ausführung |
| Breakpoint Store | Persistenz der Breakpoint-Konfiguration |
| Breakpoint Renderer | Darstellung im Editor und Logic Graph |

Die Implementierung verwendet das gemeinsame Debug-Protokoll aus `NPSPEC-NOVALANG-DEBUGGING-0001`.

## Breakpoint-Typen

| Typ | Beschreibung |
|---|---|
| Line Breakpoint | Hält an einer ausführbaren Quellcodeposition |
| Function Breakpoint | Hält beim Eintritt in eine Funktion |
| Conditional Breakpoint | Hält bei erfüllter Bedingung |
| Hit Count Breakpoint | Hält nach einer definierten Trefferanzahl |
| Exception Breakpoint | Hält bei bestimmten Exceptions |
| Data Breakpoint | Überwacht Änderungen an unterstützten Daten |
| Graph Breakpoint | Hält an einem Logic-Graph-Knoten |
| Logpoint | Protokolliert Informationen ohne reguläres Anhalten |

Nicht jeder Breakpoint-Typ muss auf jeder Zielplattform technisch verfügbar sein. Einschränkungen müssen angezeigt werden.

## Breakpoint-Modell

Jeder Breakpoint besitzt:

- Eindeutige Breakpoint-ID
- Breakpoint-Typ
- Aktivierungszustand
- Zielreferenz
- Optionale Bedingung
- Optionale Trefferanzahl
- Optionale Task- oder Thread-Einschränkung
- Zugehörige Debug-Konfiguration

Breakpoints besitzen zusätzlich einen Auflösungszustand:

- Pending
- Verified
- Disabled
- Unresolved
- Error

Ein angelegter Breakpoint gilt erst nach Bestätigung durch das Debug-Ziel als tatsächlich wirksam.

## Line Breakpoints

Line Breakpoints werden direkt im Code Editor gesetzt.

Unterstützt werden:

- Setzen per Klick auf den Editorrand
- Aktivieren und Deaktivieren
- Entfernen
- Bearbeiten von Bedingungen
- Anzeige des Auflösungszustands
- Navigation zur Quellcodeposition

Bei Quellcodeänderungen muss die Position soweit möglich aktualisiert werden.

Kann ein Breakpoint nicht eindeutig einer ausführbaren Instruktion zugeordnet werden, muss dies angezeigt werden.

## Conditional Breakpoints

Ein Conditional Breakpoint hält nur an, wenn seine Bedingung erfüllt ist.

Beispiel:

```vb
ergebnis > 100
person.Alter >= 18
index = 50
```

Bedingungen werden im aktuellen Ausführungskontext ausgewertet.

Standardmäßig sind ausschließlich seiteneffektfreie Ausdrücke zulässig.

Fehler bei der Auswertung müssen diagnostiziert werden und dürfen nicht unbemerkt als erfüllte Bedingung gelten.

## Hit Count Breakpoints

Hit Count Breakpoints unterstützen:

- Anhalten beim N-ten Treffer
- Anhalten ab dem N-ten Treffer
- Anhalten bei jedem N-ten Treffer

Trefferzähler müssen einer konkreten Debug-Sitzung zugeordnet werden.

Das Verhalten bei Neustart und erneutem Aktivieren muss eindeutig definiert sein.

## Exception Breakpoints

Exception Breakpoints ermöglichen das Anhalten:

- Beim Auslösen einer Exception
- Bei unbehandelten Exceptions
- Bei ausgewählten Exception-Typen

Der Debugger muss die jeweilige Exception und ihren Ausführungskontext anzeigen können.

## Data Breakpoints

Data Breakpoints überwachen unterstützte Speicher- oder Objektzustände.

Dabei gilt:

- Überwachte Daten müssen eindeutig identifizierbar sein.
- Die Lebensdauer der überwachten Ressource muss berücksichtigt werden.
- Hardware- und Runtime-Einschränkungen müssen angezeigt werden.
- Nicht unterstützte Datenüberwachung darf nicht als aktiv dargestellt werden.

Data Breakpoints dürfen keine Speicherisolation umgehen.

## Logic-Graph-Breakpoints

Im Logic Graph können Breakpoints auf Knoten gesetzt werden.

Unterstützt werden:

- Anhalten vor der Knotenausführung
- Anhalten nach der Knotenausführung
- Bedingtes Anhalten anhand verfügbarer Daten
- Anzeige von Eingangs- und Ausgangswerten
- Unterscheidung paralleler Ausführungspfade

Graph-Breakpoints verwenden stabile Knotenidentitäten.

Ein Graph-Breakpoint darf keinen Capability-Aufruf unkontrolliert wiederholen.

## Logpoints

Logpoints erzeugen Diagnoseausgaben, ohne die Ausführung regulär anzuhalten.

Beispiel:

```text
Verarbeitung erreicht: index={index}
```

Ausdrücke müssen denselben Sicherheitsregeln wie Watch-Ausdrücke unterliegen.

Logpoints benötigen begrenzbare Ausgabepuffer und dürfen keine geschützten Daten unautorisiert offenlegen.

## Ausführungssteuerung

Beim Erreichen eines Breakpoints kann der Debugger:

- Den betroffenen Task anhalten
- Einen definierten Ausführungskontext anhalten
- Den gesamten Zielprozess anhalten

Der gewählte Anhaltemodus muss erkennbar sein.

Andere Tasks dürfen nicht unbeabsichtigt blockiert werden, sofern keine vollständige Prozesspause angefordert wurde.

## AOT, JIT und Interpreter

| Modus | Breakpoint-Umsetzung |
|---|---|
| Interpreter | Prüfung an virtuellen Ausführungspunkten |
| JIT | Zuordnung zu kompilierten Codepositionen |
| AOT | Zuordnung über native Debug-Informationen |

JIT-Optimierungen und Codeersetzungen müssen bestehende Breakpoints soweit möglich erhalten.

Nicht verfügbare Quellcodepositionen müssen eindeutig gekennzeichnet werden.

## Persistenz

Breakpoint-Konfigurationen können workspacebezogen gespeichert werden.

Gespeichert werden:

- Breakpoint-ID und Typ
- Dokument- oder Knotenreferenz
- Position beziehungsweise Symbolidentität
- Aktivierungszustand
- Bedingungen
- Trefferregeln

Laufzeitadressen dürfen nicht als dauerhaft stabile Quellcodereferenzen vorausgesetzt werden.

## Benutzeroberfläche

NovaLang Studio stellt Breakpoints bereit über:

- Editorrand
- Logic-Graph-Knoten
- Breakpoints Panel
- Kontextmenü
- Tastenkombinationen
- Debug-Werkzeugleiste

Die Darstellung unterscheidet aktivierte, deaktivierte, nicht aufgelöste und fehlerhafte Breakpoints.

Häufig verwendete Aktionen müssen unmittelbar erreichbar sein.

## Performance

- Deaktivierte Breakpoints dürfen keine unnötigen Laufzeitprüfungen verursachen.
- Bedingungen werden nur an relevanten Haltepunkten ausgewertet.
- Breakpoint-Daten müssen effizient indiziert werden.
- Trefferzähler und Log-Ausgaben müssen begrenzbar sein.
- Änderungen an Breakpoints sollen ohne vollständigen Neustart möglich sein.
- Der Debugger muss den zusätzlichen Ausführungsaufwand nachvollziehbar machen können.

## Sicherheit

- Breakpoints benötigen eine autorisierte Debug-Sitzung.
- Bedingungen dürfen standardmäßig keine Seiteneffekte verursachen.
- Geschützte Speicherbereiche dürfen nicht unautorisiert untersucht werden.
- Capability- und Sandbox-Grenzen bleiben wirksam.
- Breakpoints dürfen keine zusätzlichen Berechtigungen erzeugen.
- Externe Debug-Ziele unterliegen den NovaOS-Zugriffsregeln.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Breakpoint Manager bereitstellen.
2. Line-, Function-, Conditional-, Hit-Count- und Exception-Breakpoints MÜSSEN unterstützt werden.
3. Logic-Graph-Breakpoints MÜSSEN verfügbar sein.
4. Breakpoints MÜSSEN eindeutige Identitäten und Auflösungszustände besitzen.
5. Nicht auflösbare Breakpoints MÜSSEN entsprechend gekennzeichnet werden.
6. Bedingungen MÜSSEN standardmäßig seiteneffektfrei ausgewertet werden.
7. Breakpoints MÜSSEN während einer Debug-Sitzung verwaltbar sein.
8. AOT, JIT und Interpreter MÜSSEN dasselbe grundlegende Breakpoint-Modell verwenden.
9. Task- und prozessbezogenes Anhalten MÜSSEN eindeutig unterschieden werden.
10. Breakpoint-Konfigurationen MÜSSEN workspacebezogen gespeichert werden können.
11. Data Breakpoints MÜSSEN ihre technischen Einschränkungen offenlegen.
12. Logpoints DÜRFEN keine geschützten Informationen unautorisiert ausgeben.
13. Breakpoints DÜRFEN keine Capability-, Speicher- oder Sandbox-Grenzen umgehen.
14. Ressourcenverbrauch und Diagnoseausgaben MÜSSEN begrenzbar sein.
15. Die Breakpoint-Verwaltung MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine zentrale, sichere und leistungsfähige Breakpoint-Verwaltung für NovaLang-Code, Tasks und Logic Graph.

Haltepunkte können gezielt gesetzt, kombiniert, überwacht und dauerhaft verwaltet werden, während Debugger und Runtime ein gemeinsames Ausführungsmodell verwenden.
