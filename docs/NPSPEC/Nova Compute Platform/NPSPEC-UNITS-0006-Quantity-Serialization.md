# NPSPEC-UNITS-0006
# Quantity Serialization

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0006 |
| Titel | Quantity Serialization |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Abhängigkeiten | NPSPEC-UNITS-0001 bis 0005 |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Diese Spezifikation definiert die standardisierte Serialisierung von `nova_quantity_t`, `nova_measurement_t` und verwandten Datentypen.

Ziele:

- verlustfreie Speicherung
- plattformunabhängiger Datenaustausch
- stabile ABI
- langfristige Archivierung
- IPC- und Netzwerkübertragung
- Versionierung
- Vorwärts- und Rückwärtskompatibilität

---

# 2. Grundprinzipien

Jede serialisierte Quantity muss mindestens enthalten:

- Schema-Version
- Quantity-Typ
- Numerischer Typ
- Wert
- Dimension
- Einheit
- Flags

Measurements enthalten zusätzlich:

- Unsicherheit
- Verteilung
- Konfidenz
- Zeitstempel
- Sensorreferenz
- Kalibrierung
- Traceability

---

# 3. Serialisierungsformate

Nova definiert vier Referenzformate:

1. Nova Binary Format (NQTY)
2. JSON
3. CBOR
4. MessagePack

Das Binärformat dient als Referenz für IPC und Persistenz.

---

# 4. Nova Binary Format

Header:

```text
Magic      NQTY
Version
Flags
Payload Length
Checksum
```

Payload:

```text
Quantity Descriptor
Value
Dimension ID
Unit ID
Metadata
```

---

# 5. JSON

Beispiel:

```json
{
  "schema": 1,
  "type": "quantity",
  "value": 9.81,
  "unit": "m/s²",
  "dimension": "acceleration"
}
```

---

# 6. CBOR & MessagePack

Diese Formate verwenden dieselbe logische Struktur wie JSON, jedoch kompakt und binär kodiert.

---

# 7. Versionsverwaltung

Jeder Datensatz besitzt:

- Schema-Version
- Dokumentversion
- optionale ABI-Version

Neue Felder dürfen ergänzt werden, bestehende Pflichtfelder dürfen nicht entfernt werden.

---

# 8. Validierung

Vor dem Deserialisieren sind mindestens zu prüfen:

- Magic
- Version
- Prüfsumme
- Dimensions-ID
- Unit-ID
- Datentyp
- Payload-Länge

Ungültige Daten müssen verworfen werden.

---

# 9. API

```c
nova_result_t nova_quantity_serialize(
    const nova_quantity_t* quantity,
    nova_stream_t* stream);

nova_result_t nova_quantity_deserialize(
    nova_stream_t* stream,
    nova_quantity_t* quantity);

nova_result_t nova_measurement_serialize(
    const nova_measurement_t* measurement,
    nova_stream_t* stream);

nova_result_t nova_measurement_deserialize(
    nova_stream_t* stream,
    nova_measurement_t* measurement);
```

---

# 10. Performance

Ziele:

| Operation | Ziel |
|---|---:|
| Serialize | O(n) |
| Deserialize | O(n) |
| Handle Serialization | O(1) |

Zero-Copy ist für Shared-Memory-IPC vorzusehen.

---

# 11. Sicherheit

Serialisierte Daten dürfen optional signiert werden.

Unterstützt werden:

- Prüfsummen
- Digitale Signaturen
- Integritätsprüfung
- Versionsprüfung

---

# 12. Reflection

Serialisierte Objekte müssen Reflection unterstützen.

Abfragbar:

- Datentyp
- Einheit
- Dimension
- Schema-Version
- Metadaten

---

# 13. Tests

Pflichttests:

- Roundtrip
- Versionskompatibilität
- Binärformat
- JSON
- CBOR
- MessagePack
- Beschädigte Daten
- Performance

---

# 14. Roadmap

Geplante Erweiterungen:

- Streaming-Serialisierung
- Delta-Encoding
- Kompression
- GPU-kompatible Formate
- Netzwerkoptimierung

---

# 15. Zusammenfassung

Quantity Serialization definiert den offiziellen Austauschstandard für Quantities und Measurements innerhalb der Nova Compute Platform. Dadurch bleiben Einheit, Dimension, Unsicherheit und Metadaten über Speicherung, IPC und Netzwerke hinweg vollständig erhalten.
