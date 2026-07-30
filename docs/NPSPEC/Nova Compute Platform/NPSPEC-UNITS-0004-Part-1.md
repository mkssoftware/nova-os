# NPSPEC-UNITS-0004
# Measurement Uncertainty and Error Propagation

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0004 |
| Titel | Measurement Uncertainty and Error Propagation |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Teil | 1 von 6 |
| Kapitel | 1–12 |
| Abhängigkeiten | NPSPEC-COMPUTE-0001, NPSPEC-UNITS-0001, NPSPEC-UNITS-0002, NPSPEC-UNITS-0003, NPSPEC-OBJECT-0001 |
| Zielplattformen | x86_64, AArch64, RISC-V |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Diese Spezifikation definiert das Messwert-, Unsicherheits- und Fehlerfortpflanzungsmodell von Nova OS.

Sie legt fest, wie Nova:

- Messwerte repräsentiert,
- Unsicherheiten speichert,
- Messfehler klassifiziert,
- Verteilungen beschreibt,
- Kovarianzen und Korrelationen behandelt,
- Unsicherheiten mathematisch fortpflanzt,
- Kalibrierungen referenziert,
- Messketten dokumentiert,
- Messwerte serialisiert,
- Messwerte über IPC austauscht,
- Messdaten in Tensoren und Compute-Diensten verarbeitet.

Die Spezifikation ist die verbindliche Grundlage für alle Nova-Komponenten, die reale Messdaten verarbeiten.

---

# 2. Zielsetzung

Das Measurement Framework verfolgt folgende Ziele:

1. Messwerte dürfen nicht als bedeutungslose Skalare behandelt werden.
2. Unsicherheiten müssen Bestandteil des Datentyps sein.
3. Messwert und Unsicherheit müssen dieselbe Dimension besitzen.
4. Fehlerfortpflanzung muss automatisch und nachvollziehbar erfolgen.
5. Messketten müssen rückverfolgbar sein.
6. Kalibrierungen müssen versioniert und zeitlich gültig sein.
7. Messdaten müssen über Prozessgrenzen verlustfrei übertragen werden können.
8. Unsicherheitsmodelle müssen skalare, vektorielle und tensorielle Daten unterstützen.
9. Deterministische und Monte-Carlo-basierte Verfahren müssen vorgesehen werden.
10. Compute-Backends dürfen Unsicherheitsmetadaten nicht verlieren.

---

# 3. Geltungsbereich

Diese Spezifikation gilt für:

- physikalische Messungen,
- Sensoren,
- Robotik,
- industrielle Automatisierung,
- Laborsoftware,
- wissenschaftliche Anwendungen,
- Signalverarbeitung,
- Navigation,
- Bild- und Audiosensorik,
- medizinische Messgeräte,
- Simulationen mit Messdaten,
- KI-Systeme mit Sensorschnittstellen,
- Datenlogger,
- Kalibrierungssoftware,
- Qualitätskontrolle.

Nicht Gegenstand dieser Spezifikation sind:

- konkrete Sensortreiber,
- physikalische Formelbibliotheken,
- vollständige Statistikbibliotheken,
- hardwareabhängige Kalibrierungsverfahren,
- regulatorische Zertifizierungsprozesse einzelner Branchen.

---

# 4. Leitprinzipien

## 4.1 Messwert statt Rohzahl

Ein Sensorwert wird als `nova_measurement_t` und nicht als nackter primitiver Datentyp behandelt.

Nicht ausreichend:

```c
double temperature = 21.4;
```

Vorgesehen:

```c
nova_measurement_t temperature;
```

## 4.2 Unsicherheit ist Teil der Semantik

Ein Messwert ohne Unsicherheitsinformation ist unvollständig, sofern die Quelle keine exakte Größe liefert.

## 4.3 Einheit und Dimension bleiben erhalten

Wert, Unsicherheit und Verteilung müssen mit dem Nova Quantity Type System kompatibel sein.

## 4.4 Fehler und Unsicherheit sind zu unterscheiden

Ein Fehler ist die Abweichung zwischen Messwert und Referenzwert.

Eine Unsicherheit beschreibt den Bereich plausibler Werte unter einem definierten Modell.

## 4.5 Rückführbarkeit ist explizit

Kalibrierungen, Referenzen und Messketten müssen durch stabile IDs beschrieben werden.

## 4.6 Metadaten dürfen nicht still verloren gehen

Bei Berechnung, Serialisierung, IPC und Backend-Ausführung müssen relevante Messmetadaten erhalten bleiben.

---

# 5. Terminologie

## 5.1 Messgröße

Die physikalische oder technische Größe, die bestimmt werden soll.

Beispiele:

- Temperatur,
- Länge,
- Spannung,
- Druck,
- Geschwindigkeit.

## 5.2 Messwert

Der numerisch bestimmte Wert einer Messgröße.

## 5.3 Wahrer Wert

Der idealisierte exakte Wert einer Messgröße.

Der wahre Wert ist in realen Messsystemen im Allgemeinen nicht vollständig bekannt.

## 5.4 Referenzwert

Ein bekannter oder vereinbarter Wert, gegen den Messungen verglichen werden.

## 5.5 Fehler

```text
Fehler = Messwert - Referenzwert
```

## 5.6 Unsicherheit

Ein nichtnegativer Parameter, der die Streuung der Werte beschreibt, die der Messgröße vernünftigerweise zugeordnet werden können.

## 5.7 Standardunsicherheit

Eine Unsicherheit, die als Standardabweichung ausgedrückt wird.

## 5.8 Erweiterte Unsicherheit

Eine kombinierte Standardunsicherheit multipliziert mit einem Überdeckungsfaktor.

## 5.9 Bias

Eine systematische Verschiebung des Erwartungswertes gegenüber einem Referenzwert.

## 5.10 Drift

Eine zeitabhängige systematische Veränderung einer Messeigenschaft.

---

# 6. Measurement Object Model

Jeder Messwert ist ein Nova-Objekt.

```text
Nova Object
    ↓
Measurement Object
    ├── Quantity Value
    ├── Standard Uncertainty
    ├── Distribution
    ├── Confidence
    ├── Timestamp
    ├── Sensor Reference
    ├── Calibration Reference
    ├── Traceability Reference
    └── Metadata
```

Grundstruktur:

```c
typedef struct nova_measurement
{
    nova_object_header_t object;

    nova_quantity_t value;
    nova_quantity_t standard_uncertainty;

    nova_distribution_descriptor_t distribution;

    nova_sensor_handle_t sensor;
    nova_calibration_handle_t calibration;
    nova_traceability_handle_t traceability;

    nova_timestamp_t timestamp;
    nova_confidence_level_t confidence;

    nova_measurement_flags_t flags;
} nova_measurement_t;
```

---

# 7. Messwert und Unsicherheit

## 7.1 Dimensionsgleichheit

Messwert und absolute Unsicherheit müssen dieselbe Dimension besitzen.

Beispiel:

```text
Messwert:
12.5 m

Standardunsicherheit:
0.02 m
```

Ungültig:

```text
Messwert:
12.5 m

Unsicherheit:
0.02 s
```

## 7.2 Relative Unsicherheit

Relative Unsicherheit ist dimensionslos.

```text
u_rel = u_abs / |x|
```

## 7.3 Nullwerte

Bei einem Messwert von null darf eine relative Unsicherheit nicht ohne Sonderbehandlung berechnet werden.

Die Runtime muss in diesem Fall entweder:

- absolute Unsicherheit verwenden,
- eine definierte Grenzwertregel anwenden,
- oder einen Fehler zurückgeben.

---

# 8. Measurement Descriptor

```c
typedef struct nova_measurement_descriptor
{
    nova_measurement_uuid_t uuid;

    nova_quantity_t value;
    nova_quantity_t standard_uncertainty;

    nova_distribution_id_t distribution_id;

    nova_sensor_uuid_t sensor_uuid;
    nova_calibration_uuid_t calibration_uuid;
    nova_traceability_uuid_t traceability_uuid;

    nova_timestamp_t timestamp;

    nova_measurement_flags_t flags;
    nova_version_t version;
} nova_measurement_descriptor_t;
```

Pflichtfelder:

- Messwert,
- Dimension,
- Einheit,
- Standardunsicherheit oder Unsicherheitsstatus,
- Zeitstempel oder expliziter `NO_TIMESTAMP`-Status,
- Herkunft oder expliziter `UNKNOWN_SOURCE`-Status.

---

# 9. Measurement Flags

```c
typedef enum nova_measurement_flags
{
    NOVA_MEASUREMENT_FLAG_NONE              = 0,
    NOVA_MEASUREMENT_FLAG_EXACT             = 1u << 0,
    NOVA_MEASUREMENT_FLAG_ESTIMATED         = 1u << 1,
    NOVA_MEASUREMENT_FLAG_CALIBRATED        = 1u << 2,
    NOVA_MEASUREMENT_FLAG_UNCALIBRATED      = 1u << 3,
    NOVA_MEASUREMENT_FLAG_TRACEABLE         = 1u << 4,
    NOVA_MEASUREMENT_FLAG_SIMULATED         = 1u << 5,
    NOVA_MEASUREMENT_FLAG_DERIVED           = 1u << 6,
    NOVA_MEASUREMENT_FLAG_FILTERED          = 1u << 7,
    NOVA_MEASUREMENT_FLAG_FUSED             = 1u << 8,
    NOVA_MEASUREMENT_FLAG_OUTLIER           = 1u << 9,
    NOVA_MEASUREMENT_FLAG_INVALID           = 1u << 10,
    NOVA_MEASUREMENT_FLAG_STALE             = 1u << 11
} nova_measurement_flags_t;
```

---

# 10. Messwertarten

Nova unterscheidet mindestens folgende Messwertarten:

## 10.1 Direkte Messung

Der Wert wird unmittelbar durch einen Sensor bestimmt.

## 10.2 Indirekte Messung

Der Wert wird aus mehreren Eingangsmessungen berechnet.

## 10.3 Abgeleiteter Messwert

Der Wert entsteht durch mathematische Verarbeitung eines oder mehrerer Messwerte.

## 10.4 Simulierter Messwert

Der Wert stammt aus einer Simulation und nicht aus realer Sensorik.

## 10.5 Referenzmesswert

Der Wert dient als Referenz für Vergleich, Kalibrierung oder Validierung.

## 10.6 Fusionsmesswert

Der Wert entsteht durch die Kombination mehrerer Sensorquellen.

---

# 11. Fehlerklassifikation

Fehler werden mindestens in folgende Klassen eingeteilt:

```text
Messfehler
├── Zufälliger Fehler
├── Systematischer Fehler
├── Grober Fehler
├── Quantisierungsfehler
├── Rundungsfehler
├── Kalibrierungsfehler
├── Driftfehler
├── Modellfehler
└── Übertragungsfehler
```

Jeder Fehler darf klassifiziert, protokolliert und mit einer Ursache verknüpft werden.

---

# 12. Zufällige Fehler

Zufällige Fehler ändern sich bei wiederholten Messungen unvorhersehbar.

Typische Ursachen:

- thermisches Rauschen,
- Quantisierungsrauschen,
- mechanische Schwingungen,
- elektromagnetische Störungen,
- begrenzte Sensorauflösung.

Zufällige Fehler werden durch statistische Modelle beschrieben.

Mindestens zu unterstützen sind:

- Normalverteilung,
- Gleichverteilung,
- Dreiecksverteilung,
- Student-t-Verteilung,
- benutzerdefinierte Verteilung.

Ein zufälliger Fehler darf nicht automatisch als systematischer Bias interpretiert werden.

---

# Anhang A – Vorläufige API aus Part 1

```c
nova_result_t nova_measurement_create(
    const nova_quantity_t* value,
    const nova_quantity_t* standard_uncertainty,
    nova_measurement_t* out_measurement
);

nova_result_t nova_measurement_validate(
    const nova_measurement_t* measurement
);

nova_result_t nova_measurement_get_relative_uncertainty(
    const nova_measurement_t* measurement,
    nova_ratio_t* out_relative_uncertainty
);

nova_result_t nova_measurement_set_sensor(
    nova_measurement_t* measurement,
    nova_sensor_handle_t sensor
);

nova_result_t nova_measurement_set_calibration(
    nova_measurement_t* measurement,
    nova_calibration_handle_t calibration
);
```

---

# Anhang B – Anforderungen aus Part 1

Die Implementierung muss mindestens unterstützen:

- Measurement als Nova-Objekt,
- Messwert mit Quantity-Typ,
- absolute Standardunsicherheit,
- relative Unsicherheit,
- Dimensionsprüfung,
- Zeitstempel,
- Sensorreferenz,
- Kalibrierungsreferenz,
- Traceability-Referenz,
- Fehlerklassifikation,
- Verteilungsreferenz,
- Measurement Flags.

---

# Ausblick auf Part 2

Part 2 umfasst die Kapitel 13–24:

```text
13. Systematische Fehler
14. Grobe Fehler
15. Bias
16. Drift
17. Wiederholbarkeit
18. Reproduzierbarkeit
19. Stabilität
20. Auflösung
21. Genauigkeit
22. Präzision
23. Kalibrierung
24. Rückführbarkeit
```
