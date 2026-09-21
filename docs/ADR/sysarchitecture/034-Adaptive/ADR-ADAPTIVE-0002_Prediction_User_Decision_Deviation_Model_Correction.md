# ADR-ADAPTIVE-0002 – Prediction → User Decision → Deviation → Model Correction

## Status

Angenommen

## Kategorie

Adaptive Systems / User Adaptation

## Kontext

NovaOS soll Nutzer bei wiederkehrenden Entscheidungen unterstützen und zukünftige Aktionen vorhersagen können.

Beispiele:

```text
App / Capability Selection
Datei- oder Objektzugriff
Preloading
UI-Vorschläge
Workflow Prediction
Provider Preference
Automatisierungsvorschläge
```

Eine Vorhersage darf jedoch nicht mit der tatsächlichen Absicht des Nutzers gleichgesetzt werden.

Die tatsächliche Nutzerentscheidung ist das maßgebliche Feedbacksignal.

## Entscheidung

NovaOS verwendet für nutzerbezogene Adaptivität den Zyklus:

```text
Prediction
    ↓
User Decision
    ↓
Deviation
    ↓
Model Correction
```

Die Abweichung zwischen Vorhersage und tatsächlicher Nutzerentscheidung wird als Prediction Error behandelt.

## Grundmodell

Beispiel:

```text
Prediction:
    User will open Object A

Actual Decision:
    User opens Object B

Deviation:
    Prediction incorrect

Model Correction:
    Adjust future prediction
```

Die Nutzerentscheidung überschreibt dabei nicht rückwirkend die Vorhersage, sondern dient als Beobachtung für zukünftiges Lernen.

## Nutzerentscheidung als Ground Truth

Für nutzerbezogene Vorhersagen gilt grundsätzlich:

```text
Prediction ≠ Intent
```

Erst eine tatsächliche Nutzeraktion liefert belastbare Information über die konkrete Entscheidung.

NovaOS darf deshalb eine Vorhersage nicht als bereits erteilten Nutzerauftrag behandeln.

## Arten von Deviation

Abweichungen können unterschiedlich stark sein.

Beispiele:

```text
Exact Match
Partial Match
Alternative Selected
Prediction Ignored
Prediction Rejected
Opposite Decision
```

Die konkrete Bewertung hängt von der jeweiligen Capability und Semantik ab.

## Wiederholte Abweichungen

Ein einzelner Fehler soll nicht zwangsläufig zu einer starken Modelländerung führen.

```text
Prediction A
    ↓
User chooses B
    ↓
Small Correction
```

Wiederholt sich die Abweichung:

```text
Prediction A
    ↓
User chooses B
    ↓
Prediction A
    ↓
User chooses B
    ↓
Repeated Deviation
    ↓
Stronger Model Correction
```

NovaOS soll dadurch erkennen können, dass ein bisheriges Muster nicht mehr zur tatsächlichen Nutzung passt.

## Positive Bestätigung

Auch korrekte Vorhersagen sind Lernsignale.

```text
Prediction A
    ↓
User chooses A
    ↓
Prediction Confirmed
```

Wiederholte Bestätigung kann die Confidence des Modells erhöhen.

## Kontextabhängigkeit

Nutzerentscheidungen dürfen nicht ohne relevanten Kontext verallgemeinert werden.

Beispiele:

```text
Time
Current Task
Active Workspace
Input Type
Device
Available Capabilities
Previous Action
System State
```

Eine Entscheidung in einem bestimmten Kontext bedeutet nicht automatisch dieselbe Präferenz in jedem anderen Kontext.

## Confidence

Vorhersagen müssen eine Unsicherheit besitzen können.

```text
Prediction
    +
Confidence
```

Beispiel:

```text
Object A → 0.72
Object B → 0.21
Other    → 0.07
```

Confidence darf nicht als Wahrscheinlichkeit im mathematisch strengen Sinn interpretiert werden, sofern das verwendete Modell dies nicht garantiert.

## Vorschlag statt Zwang

Nutzerbezogene Prediction darf primär zur Unterstützung verwendet werden.

Beispiele:

```text
Preload likely object
Prioritize likely capability
Suggest next action
Prepare likely workflow
```

Eine Vorhersage darf keine explizite Nutzerentscheidung ersetzen, wenn diese erforderlich ist.

## Preloading

Prediction kann verwendet werden, um wahrscheinlich benötigte Ressourcen vorzubereiten.

```text
Prediction
    ↓
Low-Cost Preload
    ↓
User Decision
   ├── Match    → Use Prepared State
   └── Deviation → Discard / Reuse
```

Fehlvorhersagen müssen ohne funktionale Nachteile behandelbar sein.

## Kein Training gegen den Nutzer

NovaOS darf wiederholte Abweichungen nicht als Verhalten interpretieren, das der Nutzer „korrigieren“ müsse.

Stattdessen gilt:

```text
System Prediction
      ↓
User disagrees repeatedly
      ↓
System adapts
```

Nicht:

```text
System Prediction
      ↓
User disagrees
      ↓
System pushes prediction harder
```

## Explizite Entscheidungen

Explizite Nutzerentscheidungen besitzen höhere Aussagekraft als indirekt abgeleitete Signale.

Grundsätzlich:

```text
Explicit User Decision
        >
Inferred Preference
        >
Historical Prediction
```

Eine explizite dauerhafte Einstellung darf nicht durch statistisches Lernen überschrieben werden.

## Hard Constraints

Nutzeradaptivität arbeitet ausschließlich innerhalb bestehender Hard Constraints.

```text
Security
Safety
Privacy
Data Sovereignty
Authorization
Hard Realtime
Correctness
       ↓
Allowed Actions
       ↓
User Prediction
```

Eine vorhergesagte Nutzerpräferenz kann keinen Hard Constraint aufheben.

## Privacy

Nutzerbezogene Lernmodelle sollen bevorzugt lokal arbeiten.

Es gilt:

```text
Data Minimization
Local Processing Preferred
Purpose Limitation
Retention Control
No Unnecessary Raw History
```

Wo möglich, sollen abstrahierte Lernsignale statt vollständiger Nutzerhistorien gespeichert werden.

## Decision Tracing

Relevante adaptive Entscheidungen sollen nachvollziehbar sein.

```text
Prediction
    ↓
Decision ID
    ↓
User Decision
    ↓
Deviation
    ↓
Model Correction
```

Dabei müssen Privacy-Regeln eingehalten werden.

## Deterministic Mode

Nutzeradaptive Modelle dürfen deterministische Ausführungen nicht unkontrolliert beeinflussen.

Der Modellzustand muss bei Bedarf:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

## Normative Anforderungen

1. NovaOS MUSS tatsächliche Nutzerentscheidungen von Vorhersagen unterscheiden.
2. Nutzerbezogene Adaptivität SOLL dem Modell `Prediction → User Decision → Deviation → Model Correction` folgen.
3. Abweichungen MÜSSEN als Lernsignal verwendbar sein.
4. Korrekte Vorhersagen SOLLEN als positive Bestätigung verwendbar sein.
5. Wiederholte Abweichungen SOLLEN stärkere Modellkorrekturen auslösen können als einzelne Abweichungen.
6. Nutzerentscheidungen MÜSSEN im relevanten Kontext bewertet werden.
7. Vorhersagen SOLLEN eine Confidence oder Unsicherheit darstellen können.
8. Prediction DARF nicht automatisch als Nutzerauftrag interpretiert werden.
9. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor abgeleiteten Präferenzen besitzen.
10. Explizite dauerhafte Einstellungen DÜRFEN nicht durch adaptives Lernen überschrieben werden.
11. Fehlvorhersagen bei Preloading DÜRFEN keine funktionalen Nachteile verursachen.
12. Wiederholte Abweichungen MÜSSEN primär zur Anpassung des Systemmodells führen.
13. Nutzeradaptivität DARF Hard Constraints nicht abschwächen.
14. Nutzerbezogene Lernmodelle SOLLEN bevorzugt lokal verarbeitet werden.
15. Nutzeradaptivität MUSS Privacy-, Security- und Data-Sovereignty-Regeln einhalten.
16. Relevante Modellzustände MÜSSEN für Deterministic Mode fixierbar, versionierbar oder deaktivierbar sein.
17. Relevante Prediction Errors und Modellkorrekturen SOLLEN introspektierbar sein.

## Konsequenzen

### Positive Konsequenzen

- NovaOS passt sich tatsächlichem Nutzerverhalten an,
- wiederholte Fehlvorhersagen korrigieren das Systemmodell,
- Preloading und Vorschläge können kontinuierlich besser werden,
- explizite Nutzerentscheidungen bleiben maßgeblich,
- das System versucht sich an den Nutzer anzupassen statt umgekehrt.

### Negative Konsequenzen

- Nutzerentscheidungen müssen korrekt mit Vorhersagen korreliert werden,
- Kontextänderungen können historische Muster entwerten,
- Privacy und Datenminimierung begrenzen bewusst die verfügbare Lernhistorie.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0020_KI_nicht_Kernel_Voraussetzung`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0002_Retention_und_Expiration`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-USER-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-USER-DECISION-0001`
- `NPSPEC-ADAPTIVE-USER-DEVIATION-0001`
- `NPSPEC-ADAPTIVE-USER-MODEL-CORRECTION-0001`
- `NPSPEC-ADAPTIVE-USER-CONFIDENCE-0001`

## Ergebnis

NovaOS richtet nutzerbezogene Adaptivität an der tatsächlichen Entscheidung des Nutzers aus:

```text
Prediction
    ↓
User Decision
    ↓
Compare
    ↓
Deviation
    ↓
Model Correction
    ↓
Better Prediction
```

Der zentrale Grundsatz lautet:

```text
NovaOS sagt voraus,
was der Nutzer wahrscheinlich möchte.

Der Nutzer entscheidet,
was er tatsächlich möchte.

Weichen beide voneinander ab,
lernt NovaOS daraus.

Nicht der Nutzer passt sich
der Vorhersage an –
die Vorhersage passt sich
dem Nutzer an.
```