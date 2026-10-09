
# NPSPEC-LOGIC-ERRORHANDLING-0001 – NovaOS Logic Graph Error Handling

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Fehlerbehandlung

## Zweck

Definiert die Erkennung, Weiterleitung und Behandlung von Fehlern während der Logic-Graph-Ausführung.

Ziel ist eine kontrollierte Fehlerbehandlung ohne unkontrollierte Abstürze, Datenverluste oder Verletzungen von Sicherheitsgrenzen.

## Architektur

Das Error-Handling-System besteht aus:

- **Error Manager:** Zentrale Koordination der Fehlerbehandlung.
- **Error Registry:** Verwaltung typisierter Fehler.
- **Error Router:** Weiterleitung an zuständige Fehlerpfade.
- **Exception Bridge:** Integration der NovaLang-Fehlerbehandlung.
- **Recovery Controller:** Steuerung zulässiger Wiederherstellungsmaßnahmen.
- **Diagnostic Reporter:** Protokollierung und Analyse.

## Fehlerarten

Unterstützt werden:

- **Validation Error:** Ungültige Graphstruktur oder Typen.
- **Execution Error:** Fehler bei der Knotenausführung.
- **Script Error:** NovaLang-Laufzeitfehler.
- **Capability Error:** Verweigerte oder nicht verfügbare Systemfähigkeit.
- **Resource Error:** Ressourcenerschöpfung oder Zeitüberschreitung.
- **State Error:** Ungültige Zustandsoperation.
- **Cancellation:** Kontrollierter Abbruch.

Jeder Fehler besitzt einen Typ, eine Quellenreferenz und optionale Diagnoseinformationen.

## Fehlerweiterleitung

Knoten können definierte Fehlerausgänge besitzen.

Nicht behandelte Fehler werden an den übergeordneten Subgraph oder Ausführungskontext weitergeleitet.

Erreicht ein Fehler die oberste Ebene, entscheidet die Graph Runtime über den kontrollierten Abbruch der betroffenen Ausführung.

## NovaLang-Integration

Custom Scripts verwenden die reguläre NovaLang-Semantik für `Try`, `Catch` und `Finally`.

Nicht behandelte Exceptions werden über die Exception Bridge in typisierte Graphfehler überführt.

Fehlerbehandlung darf keine Sicherheits- oder Ressourcenbeschränkungen umgehen.

## Wiederherstellung

Unterstützt werden:

- Kontrollierter Wiederholungsversuch
- Definierter Fallback-Pfad
- Fehlerisolierung betroffener Teilgraphen
- Wiederherstellung gültiger Zustände
- Kontrollierter Abbruch

Wiederholungsversuche müssen begrenzt sein.

Operationen mit Seiteneffekten dürfen nur wiederholt werden, wenn dies gemäß ihrem Vertrag sicher ist.

## Sicherheit und Diagnose

Fehlerhafte Knoten dürfen andere Solutions nicht kompromittieren.

Sicherheitsverletzungen und verweigerte Berechtigungen dürfen nicht durch alternative Fehlerpfade umgangen werden.

Diagnosen müssen den verursachenden Knoten, die Execution-ID und die Fehlerursache nachvollziehbar zuordnen.

## Normative Anforderungen

1. Die Graph Runtime MUSS typisierte Fehler unterstützen.
2. Fehler MÜSSEN eindeutig ihrer Quelle zugeordnet werden können.
3. Knoten MÜSSEN definierte Fehlerausgänge bereitstellen können.
4. Nicht behandelte Fehler MÜSSEN an übergeordnete Ausführungskontexte weitergeleitet werden.
5. NovaLang-Exceptions MÜSSEN in die Graph-Fehlerbehandlung integriert sein.
6. `Finally`- und Ressourcenfreigaberegeln MÜSSEN eingehalten werden.
7. Wiederholungsversuche MÜSSEN begrenzbar sein.
8. Fehlerhafte Ausführungen MÜSSEN kontrolliert beendet werden können.
9. Wiederherstellungsmaßnahmen DÜRFEN keine Capability-Grenzen umgehen.
10. Fehler und Wiederherstellungsversuche MÜSSEN diagnostizierbar sein.
11. Fehler einer Solution DÜRFEN andere Solutions nicht kompromittieren.
12. Das Error-Handling-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine einheitliche, sichere und nachvollziehbare Fehlerbehandlung für Logic Graphs mit kontrollierter Weiterleitung, Wiederherstellung und Integration in die NovaLang Runtime.
