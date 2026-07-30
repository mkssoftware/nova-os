# NPSPEC-UNITS-0004
# Measurement Uncertainty and Error Propagation

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0004 |
| Teil | 4 von 6 |
| Kapitel | 37–48 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 37. Messketten

Eine Messkette beschreibt den vollständigen Weg vom physikalischen Ereignis bis zum verarbeiteten Messwert.

```text
Messgröße
→ Sensor
→ Signalaufbereitung
→ A/D-Wandler
→ Treiber
→ Measurement Object
→ Compute Runtime
```

Jede Station kann eigene Unsicherheiten hinzufügen und muss dokumentiert werden.

---

# 38. Sensorfusion

Messwerte mehrerer Sensoren dürfen zu einem neuen Measurement kombiniert werden.

Anforderungen:

- gemeinsame Referenzkoordinaten
- bekannte Zeitbasis
- dokumentierte Fusionsmethode
- Unsicherheitsfortpflanzung

Typische Verfahren:

- gewichteter Mittelwert
- Kalman-Filter
- Partikelfilter
- Bayes-Fusion

---

# 39. Sensor-Metadaten

Jeder Sensor besitzt Metadaten:

- Hersteller
- Modell
- Seriennummer
- Firmware
- Messbereich
- Auflösung
- Genauigkeit
- Kalibrierdatum
- Umgebungsbedingungen

Messwerte referenzieren den Sensor über einen stabilen Handle.

---

# 40. Zeitstempel

Jeder Measurement-Eintrag kann einen hochauflösenden Zeitstempel besitzen.

Eigenschaften:

- UTC-basierte Referenz
- Monotonic-Clock-Unterstützung
- Nanosekundenauflösung
- Synchronisationsstatus

---

# 41. Abtastung

Die Runtime unterstützt verschiedene Abtastmodelle:

- periodisch
- ereignisgesteuert
- kontinuierlich
- Burst-Modus

Abtastrate und Nyquist-relevante Parameter sind Metadaten des Messstroms.

---

# 42. Filterung

Filter verändern den Messwert und müssen nachvollziehbar dokumentiert werden.

Unterstützte Klassen:

- Tiefpass
- Hochpass
- Bandpass
- Medianfilter
- Gleitender Mittelwert
- Benutzerdefinierte Filter

Gefilterte Messwerte erhalten das Flag `FILTERED`.

---

# 43. SI-Rückführbarkeit

Die Rückführbarkeit beschreibt die Verbindung zu anerkannten Referenzstandards.

Eine vollständige Kette umfasst:

```text
Measurement
→ Calibration
→ Reference Instrument
→ Accredited Laboratory
→ National Standard
→ SI Definition
```

---

# 44. Kalibrierzertifikate

Kalibrierungen können auf digitale Zertifikate verweisen.

Pflichtinformationen:

- Zertifikats-ID
- Aussteller
- Gültigkeitszeitraum
- Referenznorm
- Unsicherheitsbudget
- Digitale Signatur (optional)

---

# 45. Validierung

Vor der Verwendung muss ein Measurement validiert werden.

Geprüft werden:

- Dimension
- Einheit
- Unsicherheit
- Zeitstempel
- Sensorreferenz
- Kalibrierstatus
- Wertebereich

Ungültige Messwerte dürfen nicht stillschweigend verarbeitet werden.

---

# 46. Qualitätsbewertung

Jeder Messwert kann einen Qualitätsindikator besitzen.

Beispiele:

```text
Excellent
Good
Acceptable
Poor
Invalid
```

Zusätzlich kann ein numerischer Qualitätswert gespeichert werden.

---

# 47. Messdatenlebenszyklus

Lebenszyklus:

```text
Erfassung
→ Validierung
→ Kalibrierung
→ Verarbeitung
→ Speicherung
→ Analyse
→ Archivierung
→ Löschung
```

Alle Änderungen müssen nachvollziehbar sein.

---

# 48. Auditierbarkeit

Alle relevanten Änderungen an Measurement-Objekten können protokolliert werden.

Zu erfassen sind:

- Erstellung
- Änderung
- Kalibrierung
- Fusion
- Filterung
- Export
- Löschung

Auditinformationen müssen manipulationssicher speicherbar sein.

---

# Anhang A – API

```c
nova_result_t nova_measurement_validate(
    const nova_measurement_t* measurement);

nova_result_t nova_measurement_attach_sensor(
    nova_measurement_t* measurement,
    nova_sensor_handle_t sensor);

nova_result_t nova_measurement_attach_certificate(
    nova_measurement_t* measurement,
    nova_certificate_handle_t certificate);
```

---

# Ausblick auf Part 5

Kapitel 49–60:

- Runtime Objects
- Serialization
- IPC
- Reflection
- Compute Runtime
- GPU Integration
- Tensor Integration
- Performance
- Thread Safety
- Security
- Diagnostics
- Logging
