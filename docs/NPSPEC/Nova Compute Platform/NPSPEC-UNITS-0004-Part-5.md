# NPSPEC-UNITS-0004
# Measurement Uncertainty and Error Propagation

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0004 |
| Teil | 5 von 6 |
| Kapitel | 49–60 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 49. Runtime Objects

Measurement-Objekte sind First-Class-Objekte der Nova Runtime.

Eigenschaften:

- UUID
- Reference Counting
- Reflection
- Shared Memory
- IPC-fähig
- Versioniert
- Thread-sicher

Measurement-Objekte dürfen direkt zwischen Prozessen übergeben werden, ohne semantische Informationen zu verlieren.

---

# 50. Serialisierung

Measurements müssen vollständig serialisierbar sein.

Zu serialisieren sind mindestens:

- Messwert
- Einheit
- Dimension
- Standardunsicherheit
- Verteilung
- Konfidenzniveau
- Zeitstempel
- Sensorreferenz
- Kalibrierungsreferenz
- Traceability-Referenz
- Flags
- Version

Unterstützte Formate:

- Nova Binary Format
- JSON
- CBOR
- MessagePack

---

# 51. IPC

Measurements werden über Nova IPC transportiert.

Eigenschaften:

- Zero-Copy sofern möglich
- Handle-basierter Datenaustausch
- Shared-Memory-Unterstützung
- Integritätsprüfung
- Versionsprüfung

Der Empfänger muss dieselbe Semantik wie der Sender erhalten.

---

# 52. Reflection

Measurement-Objekte unterstützen vollständige Laufzeit-Reflection.

Abfragbar sind u. a.:

- Datentyp
- Einheit
- Dimension
- Unsicherheitsmodell
- Verteilung
- Sensor
- Kalibrierung
- Metadaten

Reflection ist sprachunabhängig verfügbar.

---

# 53. Compute Runtime

Alle Compute-Dienste arbeiten direkt mit Measurement-Objekten.

Beispiele:

- Matrix Engine
- Tensor Engine
- FFT Engine
- Physics Engine
- AI Runtime
- Robotics Runtime

Unsicherheiten müssen während der Berechnung erhalten oder korrekt fortgepflanzt werden.

---

# 54. GPU-Integration

Measurement-Daten können auf GPU-Backends übertragen werden.

Anforderungen:

- Metadaten bleiben erhalten
- Dimensionen bleiben erhalten
- Unsicherheitsmodell bleibt erhalten
- Ergebnis muss reproduzierbar sein

Optimierte GPU-Repräsentationen dürfen intern verwendet werden.

---

# 55. Tensor-Integration

Measurement-Tensoren bestehen aus Measurement-Objekten gleicher Dimension.

Beispiel:

```text
Temperature Tensor
```

Alle Elemente besitzen:

- identische Dimension
- kompatible Einheit
- individuelles Unsicherheitsmodell

Gemischte Einheiten müssen vor der Verarbeitung normalisiert werden.

---

# 56. Performance

Zielsetzungen:

| Operation | Ziel |
|---|---:|
| Measurement Copy | O(1) Handle |
| Reflection | O(1) |
| Serialization | O(n) |
| Propagation | O(n) |
| Validation | O(n) |

Konstante Metadaten dürfen zwischengespeichert werden.

---

# 57. Thread Safety

Measurement-Objekte unterstützen parallele Lesezugriffe.

Schreibzugriffe erfolgen:

- atomar
- transaktional
- versionsgesichert

Unveränderliche Measurement-Objekte sollen bevorzugt werden.

---

# 58. Sicherheit

Measurements dürfen keine unautorisierten Änderungen zulassen.

Zu schützen sind:

- Kalibrierungsinformationen
- Traceability
- Signaturen
- Unsicherheitsbudget
- Sensorreferenzen

Manipulationen müssen erkennbar sein.

---

# 59. Diagnose

Die Runtime stellt Diagnosefunktionen bereit.

Beispiele:

```c
nova_measurement_verify();
nova_measurement_dump();
nova_measurement_validate();
```

Diagnosen umfassen:

- Dimensionsfehler
- fehlende Kalibrierung
- ungültige Unsicherheit
- ungültige Verteilungen
- beschädigte Metadaten

---

# 60. Logging

Die Measurement Runtime kann sicherheits- und qualitätsrelevante Ereignisse protokollieren.

Typische Ereignisse:

- Erstellung
- Änderung
- Kalibrierung
- Fehlerfortpflanzung
- GPU-Ausführung
- IPC-Übertragung
- Validierungsfehler

Das Logging ist konfigurierbar und DSGVO-konform implementierbar.

---

# Anhang A – Öffentliche API

```c
nova_result_t nova_measurement_serialize(
    const nova_measurement_t* measurement,
    nova_stream_t* stream);

nova_result_t nova_measurement_deserialize(
    nova_stream_t* stream,
    nova_measurement_t* measurement);

nova_result_t nova_measurement_reflect(
    const nova_measurement_t* measurement,
    nova_reflection_object_t* out_reflection);
```

---

# Ausblick auf Part 6

Kapitel 61–72:

- Testframework
- Referenzdatensätze
- Compliance
- Zertifizierung
- Qualitätsrichtlinien
- Best Practices
- Wartung
- Roadmap
- Anhänge
- Schlussbestimmungen
- Offene Punkte
- Folgespezifikationen
