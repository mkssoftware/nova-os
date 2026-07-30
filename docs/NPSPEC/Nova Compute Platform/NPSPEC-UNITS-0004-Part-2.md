# NPSPEC-UNITS-0004
# Measurement Uncertainty and Error Propagation

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0004 |
| Teil | 2 von 6 |
| Kapitel | 13–24 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 13. Systematische Fehler

Systematische Fehler verschieben Messwerte reproduzierbar in eine Richtung.

Eigenschaften:

- reproduzierbar
- kalibrierbar
- modellierbar
- nicht durch Mittelwertbildung eliminierbar

Typische Ursachen:

- Sensoroffset
- falsche Skalierung
- Temperaturabhängigkeit
- Alterung
- fehlerhafte Referenznormale

---

# 14. Grobe Fehler

Grobe Fehler entstehen durch Bedienungs-, Hardware- oder Softwarefehler.

Beispiele:

- falscher Sensor
- Überlauf
- vertauschte Einheiten
- defekte Kalibrierung
- Kommunikationsfehler

Grobe Fehler dürfen nicht in die statistische Unsicherheitsrechnung übernommen werden, sondern müssen erkannt und behandelt werden.

---

# 15. Bias

Der Bias beschreibt die systematische Abweichung zwischen Erwartungswert und Referenzwert.

```text
Bias = Mittelwert(Messung) − Referenzwert
```

Nova speichert Bias separat von der Standardunsicherheit.

---

# 16. Drift

Drift beschreibt eine zeitabhängige Änderung des Messverhaltens.

Unterstützte Driftmodelle:

- linear
- exponentiell
- temperaturabhängig
- benutzerdefiniert

Jede Driftfunktion besitzt Gültigkeitsbereich und Versionskennung.

---

# 17. Wiederholbarkeit

Wiederholbarkeit beschreibt die Übereinstimmung mehrerer Messungen unter identischen Bedingungen.

Zu dokumentieren sind:

- Messdauer
- Umgebungsbedingungen
- Sensor
- Bediener
- Konfiguration

---

# 18. Reproduzierbarkeit

Reproduzierbarkeit beschreibt die Übereinstimmung von Messungen unter veränderten Bedingungen.

Mögliche Unterschiede:

- anderer Sensor
- anderer Bediener
- anderer Standort
- anderer Zeitpunkt

---

# 19. Stabilität

Die Stabilität eines Messsystems beschreibt die zeitliche Konstanz seiner Eigenschaften.

Zu überwachen sind:

- Offset
- Rauschen
- Drift
- Kalibrierstatus
- Alterung

---

# 20. Auflösung

Die Auflösung ist die kleinste unterscheidbare Änderung einer Messgröße.

Sie ist von der Unsicherheit zu unterscheiden.

Beispiel:

```text
Auflösung: 0,01 °C
Unsicherheit: ±0,08 °C
```

---

# 21. Genauigkeit

Genauigkeit beschreibt die Nähe eines Messwertes zum Referenzwert.

Nova behandelt Genauigkeit als Qualitätsmerkmal, nicht als mathematische Größe.

---

# 22. Präzision

Präzision beschreibt die Streuung wiederholter Messungen.

Hohe Präzision bedeutet nicht automatisch hohe Genauigkeit.

---

# 23. Kalibrierung

Kalibrierungen werden als eigenständige Objekte geführt.

Eine Kalibrierung enthält mindestens:

- Kalibrier-ID
- Verfahren
- Referenz
- Gültigkeitszeitraum
- Unsicherheitsbudget
- Verantwortliche Stelle

Messwerte referenzieren ihre Kalibrierung über einen Handle.

---

# 24. Rückführbarkeit

Jeder Messwert kann auf nationale oder internationale Normale rückführbar sein.

Die Rückführungskette umfasst beispielsweise:

```text
Messwert
↓
Sensor
↓
Kalibrierung
↓
Referenzgerät
↓
Normallabor
↓
SI-Definition
```

Die Runtime muss diese Kette als Metadaten speichern und serialisieren können.

---

# Anhang A – API

```c
nova_result_t nova_measurement_set_bias(
    nova_measurement_t* measurement,
    const nova_quantity_t* bias
);

nova_result_t nova_measurement_set_calibration(
    nova_measurement_t* measurement,
    nova_calibration_handle_t calibration
);

nova_result_t nova_measurement_get_traceability(
    const nova_measurement_t* measurement,
    nova_traceability_handle_t* out_traceability
);
```

---

# Ausblick auf Part 3

Kapitel 25–36:

- Standardunsicherheit
- Kombinierte Unsicherheit
- Erweiterte Unsicherheit
- Überdeckungsfaktor
- Konfidenzintervalle
- Kovarianz
- Korrelation
- Wahrscheinlichkeitsverteilungen
- Normalverteilung
- Student-t-Verteilung
- Unsicherheitsbudget
- Unsicherheitsfortpflanzung
