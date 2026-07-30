# NPSPEC-UNITS-0001
# Units, Dimensions and Quantities

| Dokument | NPSPEC-UNITS-0001 |
|-----------|-------------------|
| Titel | Units, Dimensions and Quantities |
| Version | 1.0.0 |
| Status | Angenommen |
| Autor | Nova OS Projekt |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Abhängigkeiten | NPSPEC-COMPUTE-0001 |
| Letzte Änderung | 2026-07-13 |

---

# 1. Ziel

Das Nova Units System (NUS) definiert sämtliche physikalischen Größen, Einheiten, Dimensionen und Konstanten innerhalb von Nova OS.

Es stellt sicher, dass:

- mathematische Berechnungen korrekt sind,
- Dimensionsfehler erkannt werden,
- Einheiten automatisch umgerechnet werden,
- sämtliche Compute-Module dieselben Definitionen verwenden.

Das Unit-System ist Bestandteil der Nova Compute Platform und dient als gemeinsame Grundlage für alle wissenschaftlichen Berechnungen. Die Spezifikation orientiert sich am Internationalen Einheitensystem (SI) und den Regeln der Dimensionsanalyse. :contentReference[oaicite:0]{index=0}

---

# 2. Vision

In Nova OS existiert keine physikalische Größe ohne Typ.

Ein Programmierer schreibt niemals:

```cpp
double velocity;
```

sondern

```cpp
nova_velocity velocity;
```

Dadurch werden ganze Klassen mathematischer Fehler verhindert.

---

# 3. Grundprinzipien

## 3.1 Jede Größe besitzt einen Typ

Beispiele

```text
Mass
Length
Time
Velocity
Acceleration
Pressure
Force
Voltage
Current
Power
Energy
Temperature
Density
Frequency
```

---

## 3.2 Dimensionen sind Bestandteil des Typs

Nova kennt die physikalische Dimension jeder Größe.

Beispiel

```
Force

↓

kg · m / s²
```

---

## 3.3 Typprüfung

Ungültige Berechnungen sind verboten.

Nicht erlaubt:

```cpp
Length + Mass
```

Nicht erlaubt:

```cpp
Temperature * Time
```

Nicht erlaubt:

```cpp
Current + Velocity
```

---

## 3.4 Automatische Einheitenumrechnung

Alle Einheiten werden intern automatisch umgerechnet.

Beispiel

```cpp
1 km

↓

1000 m
```

ohne Informationsverlust.

---

## 3.5 SI als Referenz

Intern verwendet Nova ausschließlich SI-Basiseinheiten.

Andere Einheiten werden beim Einlesen konvertiert und bei Bedarf wieder ausgegeben. Dieses Konzept entspricht dem etablierten Vorgehen der SI-Dimensionsanalyse. :contentReference[oaicite:1]{index=1}

---

# 4. Basiseinheiten

Nova verwendet die sieben SI-Basisgrößen.

| Größe | Einheit | Symbol |
|--------|----------|--------|
| Länge | Meter | m |
| Masse | Kilogramm | kg |
| Zeit | Sekunde | s |
| Strom | Ampere | A |
| Temperatur | Kelvin | K |
| Stoffmenge | Mol | mol |
| Lichtstärke | Candela | cd |

---

# 5. Dimensionsmodell

Intern besitzt jede Größe einen Dimensionsvektor.

```text
(L,M,T,I,Θ,N,J)
```

Beispiel

```
Velocity

(1,0,-1,0,0,0,0)
```

```
Acceleration

(1,0,-2,0,0,0,0)
```

```
Force

(1,1,-2,0,0,0,0)
```

Dimensionen werden mathematisch über Exponenten der SI-Basisgrößen beschrieben. :contentReference[oaicite:2]{index=2}

---

# 6. Physikalische Größen

Nova definiert unter anderem:

## Mechanik

- Length
- Area
- Volume
- Velocity
- Acceleration
- Force
- Torque
- Momentum
- Energy
- Power
- Pressure
- Density

---

## Elektrik

- Voltage
- Current
- Resistance
- Conductance
- Charge
- Capacitance
- Inductance
- ElectricField
- MagneticField

---

## Thermodynamik

- Temperature
- Heat
- HeatFlux
- HeatCapacity
- Entropy
- ThermalConductivity

---

## Optik

- LuminousFlux
- Illuminance
- Luminance

---

## Chemie

- MolarMass
- Concentration
- AmountOfSubstance

---

## Signalverarbeitung

- Frequency
- AngularFrequency
- SampleRate
- Phase
- Amplitude

---

# 7. Einheiten

Beispiele

```
Meter
Millimeter
Centimeter
Kilometer
```

```
Second
Millisecond
Microsecond
Nanosecond
```

```
Newton
Pascal
Joule
Watt
Volt
Ampere
Tesla
Henry
Farad
Ohm
```

---

# 8. Winkel

Nova behandelt Winkel als eigene Größe.

```cpp
Angle
```

Unterstützt

- Radiant
- Grad
- Bogenminute
- Bogensekunde

---

# 9. Temperaturen

Temperaturen benötigen Sonderbehandlung.

Unterstützt

- Kelvin
- Celsius
- Fahrenheit

Intern erfolgt die Verarbeitung in Kelvin.

---

# 10. Konstanten

Nova enthält sämtliche wichtigen Naturkonstanten.

Beispiele

```
π
e
c
G
h
ħ
k_B
R
N_A
μ₀
ε₀
σ
```

Jede Konstante besitzt

- Wert
- Einheit
- Quelle
- Genauigkeit
- Versionsnummer

---

# 11. Unsicherheiten

Optional können Messunsicherheiten gespeichert werden.

```cpp
nova_length

Wert

↓

12.3 m

Unsicherheit

↓

±0.02 m
```

Damit unterstützt Nova wissenschaftliche Messketten.

---

# 12. Umrechnungen

Nova erlaubt automatische Konvertierungen.

Beispiele

```cpp
km

↓

m
```

```cpp
mph

↓

m/s
```

```cpp
bar

↓

Pa
```

```cpp
°C

↓

K
```

Ungültige Umrechnungen werden abgelehnt.

---

# 13. Dimensionsprüfung

Vor jeder Berechnung prüft Nova:

```text
Dimension A

+

Dimension B
```

Nur identische Dimensionen dürfen addiert oder subtrahiert werden. Dies entspricht den Grundregeln der Dimensionsanalyse. :contentReference[oaicite:3]{index=3}

---

# 14. API-Beispiel

```cpp
nova_length distance = 120.0_m;

nova_time duration = 8.0_s;

nova_velocity speed =
    distance / duration;
```

Ergebnis

```
15 m/s
```

---

# 15. Fehlerbehandlung

Nova liefert verständliche Fehler.

Beispiel

```
DimensionMismatch

Length

+

Temperature
```

oder

```
InvalidConversion

Kilogram

↓

Meter
```

---

# 16. Compute-Integration

Alle Compute-Bibliotheken verwenden ausschließlich das Nova Units System.

```
Nova Physics

↓

Nova Units

↓

Nova Compute Runtime
```

Es existiert keine zweite Definition physikalischer Größen.

---

# 17. Erweiterbarkeit

Neue Einheiten können registriert werden.

Beispiele

- Astronomie
- Luftfahrt
- Medizin
- Robotik
- Elektromobilität

Die zugrunde liegenden Dimensionen bleiben unverändert.

---

# 18. Entwicklerprinzipien

Jede neue physikalische Größe muss definieren:

- Name
- Symbol
- Dimension
- SI-Basiseinheit
- unterstützte Einheiten
- Umrechnungsregeln
- Genauigkeit
- Dokumentation
- Testfälle

---

# 19. Langfristige Ziele

Das Nova Units System soll die zentrale Referenz für sämtliche Berechnungen in Nova OS werden.

Ziele:

- vollständige Typensicherheit
- automatische Dimensionsprüfung
- konsistente Einheiten
- reproduzierbare Ergebnisse
- einfache Erweiterbarkeit
- wissenschaftliche Genauigkeit
- langfristige ABI-Stabilität

---

# 20. Zusammenfassung

Das Nova Units System bildet das Fundament aller mathematischen, physikalischen und technischen Berechnungen in Nova OS.

Jede physikalische Größe besitzt einen eindeutigen Typ, eine definierte Dimension und eine konsistente Einheitendarstellung.

Dadurch können Dimensionsfehler früh erkannt, automatische Umrechnungen durchgeführt und sämtliche Compute-Dienste auf einer gemeinsamen, wissenschaftlich fundierten Grundlage arbeiten.