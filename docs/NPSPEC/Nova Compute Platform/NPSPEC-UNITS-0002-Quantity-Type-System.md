# NPSPEC-UNITS-0002
# Quantity Type System

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0002 |
| Titel | Quantity Type System |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Abhängigkeiten | NPSPEC-COMPUTE-0001, NPSPEC-UNITS-0001, NPSPEC-OBJECT-0001 |
| Zielplattformen | x86_64, AArch64, RISC-V |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Diese Spezifikation definiert das native Quantity Type System von Nova OS.

Das Quantity Type System beschreibt physikalische Größen, Dimensionen, Einheiten, Skalare, Messwerte, Unsicherheiten, Umrechnungen, mathematische Operationen, Speicherrepräsentationen, Laufzeitprüfung, Compilerprüfung, Serialisierung und ABI-Kompatibilität.

Es bildet die verbindliche Grundlage für alle physikalischen, technischen, wissenschaftlichen und numerischen Berechnungen innerhalb der Nova Compute Platform.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- native Nova-Systembibliotheken,
- Nova Compute Services,
- Anwendungen,
- Treiber,
- wissenschaftliche Module,
- Robotik,
- KI-Systeme,
- Signalverarbeitung,
- Simulationen,
- Messwerterfassung,
- industrielle Steuerungen,
- Entwicklerwerkzeuge.

Das Quantity Type System darf nicht durch modulspezifische oder anwendungsspezifische Einheitensysteme ersetzt werden.

Anwendungen dürfen zusätzliche Einheiten und Größen definieren, müssen jedoch die in diesem Dokument beschriebenen Basistypen und Registrierungsmechanismen verwenden.

---

# 3. Leitprinzipien

## 3.1 Jede physikalische Größe besitzt eine Dimension

Eine physikalische Größe ist niemals nur ein primitiver Zahlenwert.

```c
nova_quantity_t distance;
```

Eine Quantity enthält mindestens:

- numerischen Wert,
- Dimensionsinformation,
- Einheit,
- Skalierung,
- Typinformation.

## 3.2 Dimension und Einheit sind getrennte Konzepte

Die Dimension beschreibt die physikalische Art einer Größe. Die Einheit beschreibt ihre konkrete Darstellung.

```text
Dimension:
Länge

Einheiten:
Meter
Kilometer
Millimeter
Zoll
Fuß
```

`Meter` und `Kilometer` besitzen dieselbe Dimension, aber unterschiedliche Skalierungsfaktoren.

## 3.3 SI ist die kanonische Referenz

Alle physikalischen Größen besitzen eine kanonische SI-Repräsentation.

```text
1 Kilometer = 1000 Meter
```

Intern darf Nova eine optimierte Darstellung verwenden. Die semantische Referenz bleibt die zugehörige SI-Einheit.

## 3.4 Dimensionsfehler müssen verhindert werden

Ungültige Operationen müssen möglichst früh erkannt werden.

```c
nova_length_t length;
nova_mass_t mass;

nova_add(length, mass);
```

Ergebnis:

```text
NOVA_ERROR_DIMENSION_MISMATCH
```

Bei statisch bekannten Typen soll der Fehler bereits durch Compiler oder Typsystem erkannt werden.

## 3.5 Präzision muss explizit sein

Unterstützte Repräsentationen umfassen unter anderem:

```text
float32
float64
float128
decimal64
decimal128
int64
fixed-point
rational
```

Stille Präzisionsverluste sind nicht zulässig.

## 3.6 Einheitenumrechnungen müssen nachvollziehbar sein

Die Runtime unterscheidet mindestens:

- lineare Skalierung,
- affine Umrechnung,
- logarithmische Umrechnung,
- kontextabhängige Umrechnung,
- nicht unterstützte Umrechnung.

---

# 4. Begrifflichkeiten

## 4.1 Quantity

Eine Quantity ist ein numerischer Wert mit physikalischer Bedeutung.

```text
Quantity = Wert + Dimension + Einheit + Metadaten
```

Beispiele:

```text
5 Meter
12 Kilogramm
230 Volt
20 Grad Celsius
48 Kilohertz
```

## 4.2 Dimension

Eine Dimension beschreibt die physikalische Kategorie einer Größe, beispielsweise Länge, Masse, Zeit, elektrische Stromstärke, Temperatur, Stoffmenge oder Lichtstärke.

## 4.3 Unit

Eine Unit beschreibt die konkrete Maßeinheit einer Quantity, beispielsweise Meter, Kilometer, Newton, Pascal, Volt oder Celsius.

## 4.4 Dimensionless Quantity

Eine dimensionslose Größe besitzt einen neutralen Dimensionsvektor.

Beispiele:

- Reibungskoeffizient,
- Brechungsindex,
- Wirkungsgrad,
- Reynolds-Zahl,
- Wahrscheinlichkeit.

Dimensionslos bedeutet nicht bedeutungslos. Winkel werden in Nova als semantisch eigener Quantity-Typ behandelt.

## 4.5 Measurement

Ein Measurement ist eine Quantity mit Messmetadaten. Es kann Unsicherheit, Messzeit, Quelle, Sensor-ID, Kalibrierungsstatus, Vertrauensintervall und Qualitätskennzeichen enthalten.

---

# 5. SI-Basisdimensionen

Nova verwendet sieben SI-Basisdimensionen.

| Index | Dimension | Symbol |
|---:|---|---|
| 0 | Länge | L |
| 1 | Masse | M |
| 2 | Zeit | T |
| 3 | elektrische Stromstärke | I |
| 4 | thermodynamische Temperatur | Θ |
| 5 | Stoffmenge | N |
| 6 | Lichtstärke | J |

Der kanonische Dimensionsvektor lautet:

```text
[L, M, T, I, Θ, N, J]
```

---

# 6. Dimensionsvektor

Jede Dimension wird als Exponentenvektor dargestellt.

```text
Länge:            [1, 0,  0, 0, 0, 0, 0]
Geschwindigkeit:  [1, 0, -1, 0, 0, 0, 0]
Beschleunigung:   [1, 0, -2, 0, 0, 0, 0]
Kraft:            [1, 1, -2, 0, 0, 0, 0]
Energie:          [2, 1, -2, 0, 0, 0, 0]
```

## 6.1 Ganzzahlige Exponenten

```c
typedef struct nova_dimension_vector
{
    int8_t length;
    int8_t mass;
    int8_t time;
    int8_t current;
    int8_t temperature;
    int8_t amount;
    int8_t luminous_intensity;
} nova_dimension_vector_t;
```

## 6.2 Rationale Exponenten

Spätere Versionen dürfen rationale Exponenten unterstützen.

```c
typedef struct nova_rational_exponent
{
    int16_t numerator;
    uint16_t denominator;
} nova_rational_exponent_t;
```

## 6.3 Dimensionsvergleich

```c
bool nova_dimension_equal(
    const nova_dimension_vector_t* a,
    const nova_dimension_vector_t* b
);
```

Zwei Quantities sind dimensionskompatibel, wenn alle Exponenten identisch sind.

---

# 7. Kanonische Quantity-Struktur

```c
typedef struct nova_quantity
{
    nova_object_header_t object;

    nova_numeric_type_t numeric_type;
    nova_dimension_id_t dimension_id;
    nova_unit_id_t unit_id;

    nova_quantity_flags_t flags;

    union
    {
        int64_t integer_value;
        uint64_t unsigned_value;
        float float32_value;
        double float64_value;
        nova_float128_t float128_value;
        nova_decimal128_t decimal128_value;
    } value;
} nova_quantity_t;
```

Jede Quantity muss einen gültigen numerischen Typ, eine gültige Dimension, eine gültige Einheit, eine definierte Speicherrepräsentation, dokumentierte Rundungsregeln und eine eindeutige Serialisierung besitzen.

Für häufig verwendete Größen soll Nova statische Spezialtypen anbieten:

```c
typedef nova_quantity_t nova_length_t;
typedef nova_quantity_t nova_mass_t;
typedef nova_quantity_t nova_time_t;
typedef nova_quantity_t nova_velocity_t;
typedef nova_quantity_t nova_acceleration_t;
typedef nova_quantity_t nova_force_t;
typedef nova_quantity_t nova_energy_t;
typedef nova_quantity_t nova_power_t;
typedef nova_quantity_t nova_pressure_t;
typedef nova_quantity_t nova_temperature_t;
typedef nova_quantity_t nova_frequency_t;
```

Langfristig dürfen Compiler oder Sprache stärkere, voneinander getrennte Typen erzeugen.

---

# 8. Statisch typisierte Quantity-Typen

Statische Quantity-Typen sollen Dimensionsfehler zur Compile-Zeit verhindern.

```cpp
nova::Length distance = 12.0_m;
nova::Time duration = 4.0_s;

nova::Velocity velocity = distance / duration;
```

Ungültig:

```cpp
nova::Length length = 10.0_m;
nova::Mass mass = 4.0_kg;

auto invalid = length + mass;
```

Konzeptionelles Typschema:

```cpp
template<
    typename NumericType,
    int L,
    int M,
    int T,
    int I,
    int Theta,
    int N,
    int J
>
class Quantity;
```

Beispiele:

```cpp
using Length =
    Quantity<double, 1, 0, 0, 0, 0, 0, 0>;

using Velocity =
    Quantity<double, 1, 0, -1, 0, 0, 0, 0>;

using Force =
    Quantity<double, 1, 1, -2, 0, 0, 0, 0>;
```

---

# 9. Numerische Repräsentation

Mindestens folgende Typen müssen vorgesehen werden:

```text
NOVA_NUM_INT8
NOVA_NUM_INT16
NOVA_NUM_INT32
NOVA_NUM_INT64
NOVA_NUM_UINT8
NOVA_NUM_UINT16
NOVA_NUM_UINT32
NOVA_NUM_UINT64
NOVA_NUM_FLOAT16
NOVA_NUM_FLOAT32
NOVA_NUM_FLOAT64
NOVA_NUM_FLOAT128
NOVA_NUM_DECIMAL64
NOVA_NUM_DECIMAL128
NOVA_NUM_FIXED
NOVA_NUM_RATIONAL
NOVA_NUM_COMPLEX64
NOVA_NUM_COMPLEX128
```

Der Standardtyp für wissenschaftliche Berechnungen ist `float64`.

Für Embedded-, Echtzeit- und Finanzanwendungen muss Fixed-Point-Unterstützung vorgesehen werden:

```c
typedef struct nova_fixed_value
{
    int64_t raw;
    uint8_t fractional_bits;
} nova_fixed_value_t;
```

Dezimaltypen sind insbesondere für Finanzberechnungen, juristisch relevante Werte und exakte Dezimaldarstellungen vorgesehen.

---

# 10. Einheitendefinition

```c
typedef struct nova_unit_descriptor
{
    nova_unit_id_t id;

    const char* name;
    const char* symbol;

    nova_dimension_id_t dimension_id;

    long double scale;
    long double offset;

    nova_unit_flags_t flags;
} nova_unit_descriptor_t;
```

## 10.1 Lineare Einheiten

```text
SI-Wert = Wert × Skalierungsfaktor
```

Beispiel Kilometer:

```text
scale = 1000
offset = 0
```

## 10.2 Affine Einheiten

```text
SI-Wert = Wert × Skalierungsfaktor + Offset
```

Beispiel Celsius:

```text
Kelvin = Celsius + 273.15
```

## 10.3 Logarithmische Einheiten

Logarithmische Einheiten wie Dezibel, dBm, pH oder Magnitude benötigen eine eigene Konvertierungsklasse.

## 10.4 Kontextabhängige Einheiten

Währungen, Monate, Kalenderjahre und historische Maße dürfen nur über explizite Konvertierungsdienste umgerechnet werden.

---

# 11. Standard-Quantity-Typen

## 11.1 Geometrie

```text
nova_length_t
nova_area_t
nova_volume_t
nova_angle_t
nova_solid_angle_t
nova_curvature_t
nova_radius_t
```

## 11.2 Mechanik

```text
nova_mass_t
nova_time_t
nova_velocity_t
nova_acceleration_t
nova_force_t
nova_momentum_t
nova_energy_t
nova_work_t
nova_power_t
nova_pressure_t
nova_torque_t
nova_angular_velocity_t
nova_angular_acceleration_t
nova_moment_of_inertia_t
nova_density_t
nova_dynamic_viscosity_t
nova_kinematic_viscosity_t
```

## 11.3 Elektrizität und Magnetismus

```text
nova_electric_current_t
nova_electric_charge_t
nova_voltage_t
nova_resistance_t
nova_conductance_t
nova_capacitance_t
nova_inductance_t
nova_electric_field_t
nova_magnetic_flux_t
nova_magnetic_flux_density_t
nova_electric_power_t
```

## 11.4 Thermodynamik

```text
nova_temperature_t
nova_temperature_delta_t
nova_heat_t
nova_heat_capacity_t
nova_specific_heat_capacity_t
nova_entropy_t
nova_thermal_conductivity_t
nova_heat_flux_t
```

## 11.5 Stoffmenge und Chemie

```text
nova_amount_of_substance_t
nova_molar_mass_t
nova_molar_concentration_t
nova_mass_concentration_t
nova_molality_t
```

## 11.6 Optik

```text
nova_luminous_intensity_t
nova_luminous_flux_t
nova_illuminance_t
nova_luminance_t
nova_radiant_flux_t
nova_radiance_t
```

## 11.7 Signalverarbeitung

```text
nova_frequency_t
nova_angular_frequency_t
nova_sample_rate_t
nova_phase_t
nova_amplitude_t
nova_gain_t
nova_sound_pressure_t
nova_sound_pressure_level_t
```

## 11.8 Datenverarbeitung

```text
nova_bit_t
nova_byte_t
nova_data_rate_t
nova_storage_size_t
nova_symbol_rate_t
```

---

# 12. Temperaturmodell

Absolute Temperatur und Temperaturdifferenz sind unterschiedliche Typen.

```c
nova_temperature_t absolute;
nova_temperature_delta_t difference;
```

Zulässig:

```text
Temperatur - Temperatur = Temperaturdifferenz
Temperatur + Temperaturdifferenz = Temperatur
```

Nicht ohne explizite Semantik zulässig:

```text
Temperatur + Temperatur
```

Celsius und Fahrenheit sind affine Darstellungen absoluter Temperatur:

```c
nova_temperature_t room =
    nova_temperature_from_celsius(20.0);

nova_temperature_t value =
    nova_temperature_from_fahrenheit(68.0);
```

---

# 13. Winkelmodell

Winkel werden als semantisch eigener Typ behandelt.

```c
nova_angle_t angle;
```

Unterstützte Einheiten:

- Radiant,
- Grad,
- Bogenminute,
- Bogensekunde,
- Umdrehung.

Trigonometrische Funktionen akzeptieren ausschließlich Winkeltypen:

```c
nova_scalar_t result =
    nova_sin(angle);
```

---

# 14. Dimensionslose Größen

Nova soll wichtige dimensionslose Größen als eigene semantische Typen anbieten:

```text
nova_ratio_t
nova_percentage_t
nova_efficiency_t
nova_refractive_index_t
nova_friction_coefficient_t
nova_reynolds_number_t
nova_mach_number_t
nova_probability_t
```

Diese Typen besitzen denselben neutralen Dimensionsvektor, sind aber nicht automatisch semantisch austauschbar.

---

# 15. Konstruktion von Quantities

Generische Konstruktion:

```c
nova_result_t nova_quantity_create(
    nova_numeric_value_t value,
    nova_unit_id_t unit,
    nova_quantity_t* out_quantity
);
```

Spezifische Konstruktoren:

```c
nova_length_t nova_meters(double value);
nova_length_t nova_kilometers(double value);
nova_mass_t nova_kilograms(double value);
nova_time_t nova_seconds(double value);
nova_force_t nova_newtons(double value);
nova_pressure_t nova_pascals(double value);
```

Sprachen mit Literalunterstützung dürfen folgende Schreibweisen anbieten:

```cpp
10.0_m
5.0_kg
250.0_ms
9.81_mps2
230.0_V
50.0_Hz
```

---

# 16. Mathematische Operationen

Addition und Subtraktion sind nur bei kompatiblen Dimensionen erlaubt.

```c
nova_result_t nova_quantity_add(
    const nova_quantity_t* a,
    const nova_quantity_t* b,
    nova_quantity_t* result
);

nova_result_t nova_quantity_subtract(
    const nova_quantity_t* a,
    const nova_quantity_t* b,
    nova_quantity_t* result
);
```

Bei Multiplikation werden Dimensionsexponenten addiert:

```text
Kraft × Weg = Energie
```

Bei Division werden Dimensionsexponenten subtrahiert:

```text
Weg / Zeit = Geschwindigkeit
```

Potenzen und Wurzeln sind nur zulässig, wenn die resultierenden Exponenten darstellbar sind.

```text
Länge² = Fläche
Länge³ = Volumen
√Fläche = Länge
```

Vergleiche sind nur bei kompatiblen Dimensionen erlaubt.

---

# 17. Einheitenumrechnung

```c
nova_result_t nova_quantity_convert(
    const nova_quantity_t* source,
    nova_unit_id_t target_unit,
    nova_quantity_t* result
);
```

Beispiel:

```c
nova_length_t distance =
    nova_kilometers(5.0);

nova_length_t meters;

nova_quantity_convert(
    &distance,
    NOVA_UNIT_METER,
    &meters
);
```

Ergebnis:

```text
5000 m
```

Ungültige Umrechnungen wie `Kilogramm → Meter` müssen mit `NOVA_ERROR_INCOMPATIBLE_UNIT` abbrechen.

Unterstützte Rundungsmodi:

```text
NOVA_ROUND_NEAREST
NOVA_ROUND_TOWARD_ZERO
NOVA_ROUND_UP
NOVA_ROUND_DOWN
NOVA_ROUND_EXACT
```

---

# 18. Unsicherheitsmodell

```c
typedef struct nova_measurement
{
    nova_quantity_t value;
    nova_quantity_t uncertainty;

    nova_uncertainty_type_t uncertainty_type;
    nova_confidence_t confidence;

    nova_timestamp_t timestamp;
    nova_source_id_t source;
} nova_measurement_t;
```

Unsicherheitsarten:

```text
NOVA_UNCERTAINTY_ABSOLUTE
NOVA_UNCERTAINTY_RELATIVE
NOVA_UNCERTAINTY_STANDARD_DEVIATION
NOVA_UNCERTAINTY_CONFIDENCE_INTERVAL
NOVA_UNCERTAINTY_DISTRIBUTION
```

Für unabhängige Unsicherheiten bei Addition und Subtraktion:

```text
u(z) = √(u(x)² + u(y)²)
```

Für Multiplikation und Division werden relative Unsicherheiten kombiniert:

```text
u_r(z) = √(u_r(x)² + u_r(y)²)
```

Korrelierte Messgrößen benötigen eine Kovarianzbeschreibung.

---

# 19. Konstanten

```c
typedef struct nova_physical_constant
{
    nova_constant_id_t id;

    const char* name;
    const char* symbol;

    nova_quantity_t value;
    nova_quantity_t uncertainty;

    const char* source;
    const char* reference_version;
} nova_physical_constant_t;
```

Beispiele:

```text
NOVA_CONSTANT_SPEED_OF_LIGHT
NOVA_CONSTANT_PLANCK
NOVA_CONSTANT_REDUCED_PLANCK
NOVA_CONSTANT_GRAVITATIONAL
NOVA_CONSTANT_BOLTZMANN
NOVA_CONSTANT_AVOGADRO
NOVA_CONSTANT_ELEMENTARY_CHARGE
NOVA_CONSTANT_GAS_CONSTANT
```

Naturkonstanten müssen versionsgebunden sein, beispielsweise an `CODATA-2022`.

---

# 20. Tensor- und Vektorintegration

Tensoren dürfen Quantities enthalten:

```c
nova_tensor_t temperature_field;
```

Ein Quantity-Tensor besitzt einen gemeinsamen Datentyp, eine gemeinsame Dimension, eine gemeinsame Einheit und optional elementweise Unsicherheiten.

Gemischte Einheiten müssen vor der Verarbeitung normalisiert werden.

```text
Nicht zulässig:
[1 m, 2 km, 3 cm]

Zulässig:
[1, 2000, 0.03] m
```

Ableitungen und Integrationen führen Dimensionen korrekt weiter:

```text
dx/dt = Geschwindigkeit
∫ Geschwindigkeit dt = Länge
```

---

# 21. Compute-Service-Integration

Der Nova Compute Service muss Quantity-Metadaten über Prozessgrenzen hinweg erhalten.

Zu übertragen sind mindestens:

- Objekt-Handle,
- numerischer Typ,
- Dimension,
- Einheit,
- Shape,
- Genauigkeit,
- Unsicherheit.

CPU-, GPU-, NPU- und FPGA-Backends dürfen Metadaten intern reduzieren, müssen sie am API-Rand jedoch vollständig wiederherstellen.

Ein Compute-Backend darf niemals stillschweigend Einheiteninformationen verlieren.

---

# 22. ABI

Der öffentliche Quantity ABI muss langfristig stabil bleiben.

Direkter Zugriff auf interne Strukturen ist außerhalb der Core-API nicht erlaubt.

Für prozessübergreifende Kommunikation wird ein Handle-Modell bevorzugt:

```c
typedef uint64_t nova_quantity_handle_t;
```

```c
nova_result_t nova_quantity_create_handle(
    const nova_quantity_t* quantity,
    nova_quantity_handle_t* out_handle
);

nova_result_t nova_quantity_read_handle(
    nova_quantity_handle_t handle,
    nova_quantity_t* out_quantity
);

nova_result_t nova_quantity_release_handle(
    nova_quantity_handle_t handle
);
```

---

# 23. Serialisierung

Serialisierte Quantities müssen enthalten:

- Schema-Version,
- numerischen Typ,
- Wert,
- Dimensions-ID,
- Einheiten-ID,
- Unsicherheit,
- Metadaten.

Binärformat:

```text
NQTY
Version
Flags
Numeric Type
Dimension ID
Unit ID
Payload Length
Payload
Optional Metadata
Checksum
```

Textformat:

```json
{
  "type": "nova.quantity",
  "version": 1,
  "value": 12.5,
  "unit": "m",
  "dimension": "length"
}
```

Externe Austauschformate dürfen JSON, CBOR oder MessagePack verwenden. Die interne ABI darf davon nicht abhängig sein.

---

# 24. Reflection

```c
nova_dimension_id_t nova_quantity_get_dimension(
    const nova_quantity_t* quantity
);

nova_unit_id_t nova_quantity_get_unit(
    const nova_quantity_t* quantity
);

nova_numeric_type_t nova_quantity_get_numeric_type(
    const nova_quantity_t* quantity
);
```

---

# 25. Formatierung

Standarddarstellung:

```text
12.5 m
```

Nova darf automatisch passende SI-Präfixe wählen:

```text
0.000001 m → 1 µm
```

Die Darstellung darf lokalisiert werden:

```text
Deutsch: 12,5 m
Englisch: 12.5 m
```

Die interne Repräsentation bleibt unverändert.

---

# 26. Parsing

```c
nova_result_t nova_quantity_parse(
    const char* input,
    nova_locale_id_t locale,
    nova_quantity_t* result
);
```

Unterstützte Eingaben:

```text
12 m
12.5 kg
9,81 m/s²
230 V
20 °C
48 kHz
```

Mehrdeutige Eingaben müssen abgelehnt oder durch Kontext aufgelöst werden. Das System darf keine unsichere Interpretation stillschweigend auswählen.

---

# 27. Fehlerbehandlung

Fehlercodes:

```text
NOVA_ERROR_INVALID_QUANTITY
NOVA_ERROR_INVALID_UNIT
NOVA_ERROR_INVALID_DIMENSION
NOVA_ERROR_DIMENSION_MISMATCH
NOVA_ERROR_INCOMPATIBLE_UNIT
NOVA_ERROR_NUMERIC_OVERFLOW
NOVA_ERROR_NUMERIC_UNDERFLOW
NOVA_ERROR_PRECISION_LOSS
NOVA_ERROR_DIVISION_BY_ZERO
NOVA_ERROR_INVALID_OPERATION
NOVA_ERROR_UNSUPPORTED_CONVERSION
NOVA_ERROR_UNCERTAINTY_MISMATCH
NOVA_ERROR_SERIALIZATION
```

Fehlerobjekt:

```c
typedef struct nova_quantity_error
{
    nova_error_code_t code;

    nova_dimension_id_t expected_dimension;
    nova_dimension_id_t actual_dimension;

    nova_unit_id_t source_unit;
    nova_unit_id_t target_unit;

    const char* message;
} nova_quantity_error_t;
```

---

# 28. Einheitenregistrierung

```c
nova_result_t nova_unit_register(
    const nova_unit_descriptor_t* descriptor,
    nova_unit_id_t* out_id
);
```

Eine neue Einheit muss definieren:

- eindeutigen Namen,
- eindeutiges Symbol,
- Dimension,
- Umrechnungsregel,
- Skalierung,
- Offset,
- Version,
- Eigentümer-Namespace.

Beispiele:

```text
org.nova.si.meter
org.nova.imperial.inch
com.example.robot.encoder_tick
```

Benutzerdefinierte Einheiten dürfen keine Systemeinheiten überschreiben.

---

# 29. Dimensionsregistrierung

Neue semantische Dimensionen dürfen registriert werden, sofern sie nicht vollständig durch bestehende SI-Dimensionen beschrieben werden können.

Beispiele:

- Währung,
- Information,
- Ereignisanzahl,
- benutzerdefinierte Skalen.

Physikalische Dimensionen sollen bevorzugt aus SI-Basisdimensionen abgeleitet werden.

---

# 30. Compiler-Unterstützung

Der Nova-Compiler soll Dimensionsinformationen im Typsystem abbilden können.

```cpp
auto distance = 100.0_m;
auto time = 5.0_s;
auto speed = distance / time;
```

Abgeleiteter Typ:

```text
speed: Velocity
```

Compilerfehler müssen verständlich sein:

```text
Cannot add Quantity<Length> to Quantity<Mass>.

Left operand:
Length [L¹]

Right operand:
Mass [M¹]
```

Konstante Einheitenumrechnungen sollen bereits zur Compile-Zeit ausgeführt werden.

---

# 31. Sprachbindungen

Offizielle Bindings sollen mindestens vorgesehen werden für:

- C,
- C++,
- Rust,
- Nova Language,
- Python,
- JavaScript,
- C#,
- Swift.

Alle Sprachbindungen müssen dieselben Dimensionsregeln einhalten.

---

# 32. C-API-Beispiele

Geschwindigkeit:

```c
nova_length_t distance =
    nova_meters(120.0);

nova_time_t duration =
    nova_seconds(8.0);

nova_velocity_t velocity;

nova_result_t result =
    nova_quantity_divide(
        &distance,
        &duration,
        &velocity
    );
```

Kraft:

```c
nova_mass_t mass =
    nova_kilograms(12.0);

nova_acceleration_t acceleration =
    nova_meters_per_second_squared(9.81);

nova_force_t force;

nova_physics_force(
    &mass,
    &acceleration,
    &force
);
```

Temperatur:

```c
nova_temperature_t room =
    nova_temperature_from_celsius(21.5);

double kelvin;

nova_temperature_to_kelvin(
    &room,
    &kelvin
);
```

---

# 33. C++-API-Beispiele

```cpp
using namespace nova::units;

Length distance = 120.0_m;
Time duration = 8.0_s;

Velocity speed = distance / duration;

Force force =
    12.0_kg * 9.81_mps2;
```

---

# 34. Messwertbeispiel

```cpp
Measurement<Temperature> roomTemperature
{
    .value = 21.3_degC,
    .uncertainty = 0.2_K,
    .confidence = 0.95
};
```

---

# 35. Thread-Sicherheit

Unveränderliche Quantity-Objekte sind thread-sicher.

Veränderliche Messwert- oder Tensorobjekte müssen Synchronisationsregeln definieren.

Globale Einheiten- und Dimensionsregister müssen nebenläufigen Lesezugriff unterstützen. Registrierungsvorgänge müssen atomar sein.

---

# 36. Sicherheit

Quantity-Daten dürfen keinen direkten Hardwarezugriff ermöglichen.

Einheiten- und Dimensionsdefinitionen müssen validiert werden.

Nicht vertrauenswürdige Anwendungen dürfen:

- keine Systemeinheiten überschreiben,
- keine Systemdimensionen verändern,
- keine ABI-IDs reservierter Namespaces verwenden,
- keine ungültigen Umrechnungsfunktionen registrieren.

---

# 37. Performance

Für statisch bekannte Quantities soll die Dimensionsprüfung zur Compile-Zeit erfolgen.

Dynamische Quantities müssen optimierte Dimension- und Unit-IDs verwenden.

Häufige Umrechnungsfaktoren dürfen gecacht werden.

Homogene Quantity-Arrays müssen SIMD-fähig sein.

---

# 38. Tests

Jeder Quantity-Typ muss mindestens folgende Tests besitzen:

- Konstruktion,
- Dimensionsvergleich,
- Addition,
- Subtraktion,
- Multiplikation,
- Division,
- Potenz,
- Umrechnung,
- Serialisierung,
- Parsing,
- Formatierung,
- Rundung,
- Überlauf,
- Unsicherheitsfortpflanzung.

---

# 39. Referenztests

```text
1 km = 1000 m
1 h = 3600 s
0 °C = 273.15 K
1 N = 1 kg·m·s⁻²
1 J = 1 kg·m²·s⁻²
1 Pa = 1 kg·m⁻¹·s⁻²
1 W = 1 kg·m²·s⁻³
```

---

# 40. ABI-Versionierung

Der Quantity ABI besitzt eine eigene Versionsnummer:

```text
NOVA_QUANTITY_ABI_VERSION
```

Erweiterungen müssen abwärtskompatibel sein. Nicht kompatible Änderungen erfordern eine neue Major-Version.

---

# 41. Nicht-Ziele

Diese Spezifikation definiert nicht:

- konkrete Formelbibliotheken,
- vollständige Physikmodelle,
- FFT-Implementierungen,
- Tensoralgorithmen,
- numerische Löser,
- UI-Darstellungen wissenschaftlicher Daten.

Diese Themen werden in separaten NPSPEC-Dokumenten beschrieben.

---

# 42. Offene Punkte

Folgende Punkte müssen in späteren Revisionen konkretisiert werden:

- rationale Dimensionsexponenten,
- exakte Rationalarithmetik,
- symbolische Einheitenrechnung,
- automatische Einheitensimplifizierung,
- benutzerdefinierte Dimensionssemantik,
- komplexe Quantities,
- probabilistische Unsicherheitsmodelle,
- Interoperabilität mit externen Wissenschaftsformaten,
- Compiler-ABI für statische Quantity-Typen.

---

# 43. Geplante Folgespezifikationen

```text
NPSPEC-UNITS-0003
Unit Registry and Conversion Engine

NPSPEC-UNITS-0004
Measurement Uncertainty and Error Propagation

NPSPEC-UNITS-0005
Physical Constants Registry

NPSPEC-UNITS-0006
Quantity Serialization Format

NPSPEC-UNITS-0007
Compiler-Level Dimension Checking

NPSPEC-UNITS-0008
Quantity Tensor Integration
```

---

# 44. Zusammenfassung

Das Nova Quantity Type System stellt sicher, dass physikalische und technische Werte innerhalb von Nova OS niemals als bedeutungslose Zahlen behandelt werden.

Jede Quantity besitzt:

- einen Wert,
- einen numerischen Typ,
- eine Dimension,
- eine Einheit,
- definierte Umrechnungsregeln,
- optionale Unsicherheiten,
- eine stabile ABI-Repräsentation.

Dadurch können Dimensionsfehler früh erkannt, Einheiten automatisch umgerechnet und wissenschaftliche Berechnungen konsistent über alle Nova-Systemdienste hinweg ausgeführt werden.

Das Quantity Type System bildet die Grundlage für Physik, Signalverarbeitung, Robotik, Simulation, KI, Messwerterfassung und sämtliche weiteren wissenschaftlichen Compute-Dienste von Nova OS.
