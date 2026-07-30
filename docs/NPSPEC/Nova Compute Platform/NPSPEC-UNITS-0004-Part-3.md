# NPSPEC-UNITS-0004
# Measurement Uncertainty and Error Propagation

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0004 |
| Teil | 3 von 6 |
| Kapitel | 25–36 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 25. Standardunsicherheit

Die Standardunsicherheit ist die Unsicherheit eines Messwertes, ausgedrückt als Standardabweichung.

```text
u(x)
```

Sie ist die Basiseinheit aller weiteren Unsicherheitsberechnungen.

Arten:

- Typ-A (statistisch bestimmt)
- Typ-B (aus Spezifikationen, Kalibrierung oder Erfahrung)

---

# 26. Kombinierte Unsicherheit

Werden mehrere unabhängige Unsicherheiten kombiniert, ergibt sich die kombinierte Standardunsicherheit.

Für unabhängige Größen:

```text
u_c = √(u₁² + u₂² + … + uₙ²)
```

Für korrelierte Größen ist zusätzlich die Kovarianzmatrix zu berücksichtigen.

---

# 27. Erweiterte Unsicherheit

Die erweiterte Unsicherheit ergibt sich aus:

```text
U = k · u_c
```

Dabei ist:

- U = erweiterte Unsicherheit
- u_c = kombinierte Standardunsicherheit
- k = Überdeckungsfaktor

Standardmäßig verwendet Nova bei 95 % Konfidenz den Faktor k = 2, sofern kein anderer Wert spezifiziert ist.

---

# 28. Überdeckungsfaktor

Der Überdeckungsfaktor beschreibt den Zusammenhang zwischen Standard- und erweiterter Unsicherheit.

Typische Werte:

| Konfidenz | k |
|---:|---:|
| 68,3 % | 1 |
| 95 % | 2 |
| 99,7 % | 3 |

Anwendungen dürfen eigene Faktoren definieren, sofern diese dokumentiert werden.

---

# 29. Konfidenzintervalle

Ein Konfidenzintervall beschreibt den Bereich, in dem der wahre Wert mit einer definierten Wahrscheinlichkeit erwartet wird.

Beispiel:

```text
20,00 °C ±0,10 °C
95 %
```

Konfidenz und Unsicherheit müssen immer gemeinsam angegeben werden.

---

# 30. Kovarianz

Kovarianzen beschreiben gemeinsame Abhängigkeiten zwischen Messgrößen.

Nova speichert Kovarianzen explizit.

```c
typedef struct nova_covariance
{
    nova_quantity_t value;
    nova_measurement_uuid_t first;
    nova_measurement_uuid_t second;
} nova_covariance_t;
```

---

# 31. Korrelation

Korrelationskoeffizienten liegen im Bereich:

```text
-1 ≤ r ≤ +1
```

Sonderfälle:

- +1 vollständig korreliert
- 0 unkorreliert
- -1 vollständig negativ korreliert

Korrelationsinformationen müssen bei der Unsicherheitsfortpflanzung berücksichtigt werden.

---

# 32. Wahrscheinlichkeitsverteilungen

Nova unterstützt mindestens:

- Normalverteilung
- Gleichverteilung
- Dreiecksverteilung
- Student-t-Verteilung
- Rechteckverteilung
- benutzerdefinierte Verteilungen

Jede Verteilung besitzt eine eindeutige Kennung sowie dokumentierte Parameter.

---

# 33. Normalverteilung

Die Normalverteilung ist das Standardmodell für viele physikalische Messgrößen.

Eigenschaften:

- Mittelwert
- Standardabweichung
- Symmetrie

Sie dient als Standardannahme, sofern kein anderes Modell angegeben ist.

---

# 34. Student-t-Verteilung

Die Student-t-Verteilung wird bei kleinen Stichproben verwendet.

Sie berücksichtigt:

- Freiheitsgrade
- Stichprobengröße
- erhöhte Unsicherheit

Nova muss den Freiheitsgrad als Metadatum speichern können.

---

# 35. Unsicherheitsbudget

Ein Unsicherheitsbudget beschreibt alle relevanten Beiträge zu einer Messunsicherheit.

Beispiel:

```text
Sensorrauschen
Kalibrierung
Auflösung
Temperatur
Drift
Rundung
```

Jeder Beitrag enthält:

- Wert
- Einheit
- Verteilung
- Typ
- Quelle

---

# 36. Fehlerfortpflanzung

Nova implementiert die allgemeine Fehlerfortpflanzung nach wissenschaftlichen Standards.

Mindestens zu unterstützen sind:

- Addition
- Subtraktion
- Multiplikation
- Division
- Potenzen
- Wurzeln

Zusätzlich sollen bereitgestellt werden:

- numerische Fehlerfortpflanzung
- Monte-Carlo-Propagation
- Sensitivitätsanalyse

Die Runtime entscheidet anhand der verfügbaren Metadaten, welches Verfahren angewendet werden kann.

---

# Anhang A – API

```c
nova_result_t nova_measurement_combine_uncertainty(
    const nova_measurement_t* input,
    size_t count,
    nova_measurement_t* result
);

nova_result_t nova_measurement_propagate(
    const nova_expression_t* expression,
    const nova_measurement_t* operands,
    size_t count,
    nova_measurement_t* result
);

nova_result_t nova_measurement_set_covariance(
    nova_measurement_t* first,
    nova_measurement_t* second,
    const nova_covariance_t* covariance
);
```

---

# Ausblick auf Part 4

Kapitel 37–48:

- Messketten
- Sensorfusion
- Sensor-Metadaten
- Zeitstempel
- Abtastung
- Filterung
- SI-Rückführbarkeit
- Kalibrierzertifikate
- Validierung
- Qualitätsbewertung
- Messdatenlebenszyklus
- Auditierbarkeit
