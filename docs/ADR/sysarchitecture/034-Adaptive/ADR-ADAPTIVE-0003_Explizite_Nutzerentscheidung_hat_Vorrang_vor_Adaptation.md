# ADR-ADAPTIVE-0003 – Explizite Nutzerentscheidung hat Vorrang vor Adaptation

## Status

Angenommen

## Kategorie

Adaptive Systems / User Authority

## Kontext

NovaOS verwendet adaptive Modelle, um Nutzerverhalten vorherzusagen und Abläufe zu optimieren.

Dabei können gelernte Präferenzen mit einer expliziten Nutzerentscheidung in Konflikt geraten.

Beispiel:

```text
Adaptive Model:
    Preferred Provider = GPU

User:
    Use CPU
```

Das System benötigt eine eindeutige Regel, welche Entscheidung Vorrang besitzt.

## Entscheidung

Eine **explizite Nutzerentscheidung besitzt Vorrang vor adaptiven Entscheidungen und gelernten Präferenzen**.

```text
Explicit User Decision
        ↓
Hard System Constraints
        ↓
Execution
```

Adaptive Modelle dürfen eine explizite Nutzerentscheidung weder überschreiben noch automatisch rückgängig machen.

## Priorität

Für nutzerbezogene Entscheidungen gilt:

```text
Hard System Constraints
        ↓
Explicit User Decision
        ↓
Explicit User Preference
        ↓
Inferred Preference
        ↓
Adaptive Prediction
```

Hard Constraints wie Security, Safety oder Data Sovereignty bleiben weiterhin verbindlich.

## Explizite Nutzerentscheidung

Als explizite Entscheidung gelten beispielsweise:

```text
Manuelle Auswahl
Bestätigte Auswahl
Explizites Override
Explizite Einstellung
Explizite Ablehnung
Explizite Freigabe
```

Eine bloß vorhergesagte oder indirekt abgeleitete Präferenz gilt nicht als explizite Entscheidung.

## Einmalige Entscheidungen

Eine einmalige Nutzerentscheidung gilt zunächst für den konkreten Kontext.

```text
Prediction: Provider A

User selects Provider B

        ↓

Current Execution:
Provider B
```

Sie muss nicht automatisch als dauerhafte Präferenz gespeichert werden.

## Dauerhafte Präferenzen

Der Nutzer kann Entscheidungen ausdrücklich als dauerhaft festlegen.

```text
Always use X
Never use Y
Prefer Z
```

Solche Einstellungen besitzen Vorrang vor adaptiv gelernten Präferenzen.

Adaptive Modelle dürfen sie nicht selbstständig verändern.

## Ablehnung von Vorschlägen

Lehnt der Nutzer eine adaptive Empfehlung ab:

```text
Prediction
    ↓
Suggestion
    ↓
User Rejects
```

wird die Ablehnung respektiert.

Sie kann gleichzeitig als Lernsignal dienen:

```text
User Rejection
      ↓
Deviation
      ↓
Model Correction
```

Das Lernen verändert zukünftige Vorhersagen, nicht die bereits getroffene Nutzerentscheidung.

## Keine adaptive Gegensteuerung

NovaOS darf wiederholte Nutzerentscheidungen nicht durch verstärkte Gegensteuerung beantworten.

Nicht zulässig:

```text
User chooses B
      ↓
Model prefers A
      ↓
System repeatedly forces A
```

Stattdessen:

```text
User chooses B
      ↓
Deviation detected
      ↓
Model adapts toward observed behavior
```

## Konflikt mit Hard Constraints

Nutzerentscheidungen können keine verbindlichen Hard Constraints außer Kraft setzen.

Beispiel:

```text
User:
    Execute remotely

Data Sovereignty:
    LocalOnly
```

Ergebnis:

```text
Request rejected
```

NovaOS muss den Konflikt erklären, statt entweder die Nutzerentscheidung oder den Hard Constraint stillschweigend zu verändern.

## Automatische Optimierung

Innerhalb des durch Nutzerentscheidung und Hard Constraints verbleibenden Lösungsraums darf NovaOS weiterhin optimieren.

```text
Hard Constraints
      ↓
Explicit User Decision
      ↓
Remaining Solution Space
      ↓
Adaptive Optimization
```

Der Nutzer muss dafür nicht jede technische Detailentscheidung selbst treffen.

## Entscheidungshierarchie

Eine explizite Entscheidung kann unterschiedliche Reichweiten besitzen:

```text
This Action
This Session
This Workspace
This Application
Persistent
```

Die Reichweite muss eindeutig bestimmt sein.

Eine lokale Entscheidung darf nicht unbeabsichtigt zu einer globalen Präferenz werden.

## Änderung durch den Nutzer

Der Nutzer darf eine frühere explizite Entscheidung ändern.

```text
Previous User Decision
        ↓
New User Decision
        ↓
New Decision Takes Precedence
```

Adaptive Modelle dürfen daraus lernen, aber die Änderung nicht blockieren.

## Decision Tracing

Bei Konflikten zwischen Prediction und Nutzerentscheidung soll nachvollziehbar sein:

```text
Prediction
User Decision
Deviation
Applied Decision
Model Correction
```

Dabei bleibt die tatsächliche Nutzerentscheidung eindeutig von der Vorhersage getrennt.

## Privacy

Explizite Nutzerentscheidungen dürfen nur soweit für Adaptation gespeichert werden, wie dies erforderlich und zulässig ist.

Eine Entscheidung muss nicht automatisch eine unbegrenzt gespeicherte Verhaltenshistorie erzeugen.

## Normative Anforderungen

1. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Vorhersagen besitzen.
2. Adaptive Modelle DÜRFEN explizite Nutzerentscheidungen nicht überschreiben.
3. Hard Constraints MÜSSEN weiterhin Vorrang vor Nutzerentscheidungen besitzen.
4. Einmalige Entscheidungen DÜRFEN nicht automatisch als dauerhafte Präferenz interpretiert werden.
5. Dauerhafte explizite Präferenzen DÜRFEN nicht durch adaptives Lernen verändert werden.
6. Ablehnungen adaptiver Vorschläge MÜSSEN respektiert werden.
7. Nutzerentscheidungen DÜRFEN als Lernsignal verwendet werden.
8. Lernen DARF eine bereits getroffene Nutzerentscheidung nicht rückwirkend verändern.
9. Wiederholte Abweichungen SOLLEN primär das adaptive Modell korrigieren.
10. Adaptive Systeme DÜRFEN den Nutzer nicht durch wiederholte Gegensteuerung zur vorhergesagten Entscheidung drängen.
11. Die Reichweite einer expliziten Entscheidung MUSS eindeutig bestimmbar sein.
12. Nutzer MÜSSEN frühere explizite Entscheidungen ändern können.
13. Innerhalb verbleibender Freiheitsgrade DARF NovaOS weiterhin adaptiv optimieren.
14. Konflikte zwischen Nutzerentscheidung und Hard Constraints MÜSSEN erklärbar sein.
15. Nutzerentscheidung, Prediction und Model Correction SOLLEN getrennt introspektierbar sein.
16. Speicherung und Verarbeitung von Nutzerentscheidungen MUSS Privacy-, Security- und Data-Sovereignty-Regeln einhalten.

## Konsequenzen

### Positive Konsequenzen

- Nutzer behalten die Kontrolle über explizite Entscheidungen,
- adaptive Systeme bleiben unterstützend statt bestimmend,
- wiederholte Fehlvorhersagen führen zur Anpassung des Systems,
- dauerhafte Einstellungen bleiben stabil,
- automatische Optimierung bleibt innerhalb freier Entscheidungsräume möglich.

### Negative Konsequenzen

- Nutzerentscheidungen können adaptive Optimierungen bewusst verhindern,
- Reichweite und Lebensdauer expliziter Entscheidungen müssen verwaltet werden,
- Konflikte mit Hard Constraints benötigen verständliche Rückmeldungen.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0002_Prediction_User_Decision_Deviation_Model_Correction`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-USER-AUTHORITY-0001`
- `NPSPEC-ADAPTIVE-EXPLICIT-DECISION-0001`
- `NPSPEC-ADAPTIVE-DECISION-SCOPE-0001`
- `NPSPEC-ADAPTIVE-DECISION-OVERRIDE-0001`

## Ergebnis

NovaOS ordnet Adaptation der expliziten Nutzerentscheidung unter:

```text
Hard Constraints
       ↓
Explicit User Decision
       ↓
Remaining Freedom
       ↓
Adaptive Optimization
```

Der zentrale Grundsatz lautet:

```text
NovaOS darf vorhersagen,
vorschlagen und optimieren.

Sobald der Nutzer
eine explizite Entscheidung trifft,
hat diese Entscheidung Vorrang.

Das System passt sich
an den Nutzer an –
nicht der Nutzer
an das adaptive Modell.
```