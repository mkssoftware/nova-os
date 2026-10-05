# NPSPEC-CAPABILITY-PARAMETER-0001 – Nova Capability Parameter

## Status

Angenommen

## Kategorie

Capability / Parameter

## Zweck

NovaOS definiert Parameter als typisierte Konfigurationswerte einer Capability-Operation.

Parameter steuern die konkrete Ausführung einer Operation, ohne selbst Eingabedaten, Authority oder Provider-spezifische Konfiguration darzustellen.

## Grundprinzipien

```text
Parameter ≠ Input Data
Parameter ≠ Authority
Parameter ≠ Capability
Parameter ≠ Provider Configuration
Parameter Value ≠ Permission
Default Value ≠ Mandatory Value
```

## Parameter-Modell

Ein Parameter wird mindestens beschrieben durch:

```text
CapabilityParameter
├── Name
├── SemanticTypeID
├── Required
├── Constraints
└── DefaultValue
```

Optional:

```text
Unit
AllowedValues
Minimum
Maximum
Precision
Description
```

## Abgrenzung zu Input

Inputs transportieren die zu verarbeitenden Daten.

Parameter konfigurieren, wie die Operation diese Daten verarbeitet.

Beispiel:

```text
de.nova.image.filter.gaussian

Input:
image → Image

Parameter:
radius   → Number
strength → Number

Output:
image → Image
```

## Typisierung

Parameter müssen über definierte Typen beschreibbar sein:

```text
Boolean
Integer
Number
String
Enum
Duration
Size
SemanticType
```

Spezialisierte Typen können über die Type Registry registriert werden.

## Constraints

Parameter dürfen Wertebereiche und weitere Bedingungen definieren:

```text
radius
├── Type: Number
├── Minimum: 0
├── Maximum: 100
└── Default: 5
```

Ein Wert außerhalb des gültigen Bereichs muss vor der Ausführung abgelehnt werden.

## Standardwerte

Optionale Parameter dürfen einen Standardwert besitzen:

```text
Parameter omitted
      ↓
DefaultValue
      ↓
Validated Parameter
```

Standardwerte sind Bestandteil des Interface-Vertrags und dürfen nicht stillschweigend providerabhängig variieren.

## Benannte Parameter

Parameter werden über stabile Namen adressiert:

```text
radius = 5
strength = 0.8
```

Die Reihenfolge darf keine semantische Bedeutung besitzen, sofern das Interface dies nicht ausdrücklich definiert.

## Providerunabhängigkeit

Provider derselben Capability müssen die definierten Parameter semantisch gleich interpretieren.

Provider-spezifische Optimierungen dürfen das sichtbare Parameterverhalten nicht verändern.

## Validierung

Vor der Ausführung erfolgt:

```text
Parameter
   ↓
Type Validation
   ↓
Constraint Validation
   ↓
Default Resolution
   ↓
Execution Contract
   ↓
Capability Execution
```

Ungültige oder unbekannte Parameter müssen kontrolliert behandelt werden.

## Sicherheit

Parameter dürfen keine Authority erzeugen oder erweitern.

Ein Parameter wie:

```text
device = "camera0"
```

ersetzt keine Capability für den Zugriff auf dieses Gerät.

Objekt-, Geräte- oder Ressourcenreferenzen müssen weiterhin über autorisierte Referenzen oder Handles erfolgen.

## Versionierung

Neue optionale Parameter dürfen bei kompatiblen Interface-Versionen ergänzt werden.

Entfernung, Umdeutung oder inkompatible Änderung bestehender Parameter erfordert eine entsprechende Interface-Versionierung.

## Normative Anforderungen

1. Capability-Parameter MÜSSEN durch das Capability Interface deklarierbar sein.
2. Parameter MÜSSEN stabile Namen und definierte Typen besitzen.
3. Parameter und Input-Daten MÜSSEN getrennte Konzepte bleiben.
4. Parameter DÜRFEN Constraints besitzen.
5. Optionale Parameter DÜRFEN definierte Standardwerte besitzen.
6. Standardwerte MÜSSEN providerunabhängig definiert sein.
7. Parameterwerte MÜSSEN vor der Ausführung validiert werden.
8. Provider MÜSSEN dieselben Parameter semantisch kompatibel interpretieren.
9. Parameter DÜRFEN keine Authority erzeugen oder erweitern.
10. Ressourcenreferenzen in Parametern DÜRFEN Capability-Prüfungen nicht umgehen.
11. Inkompatible Parameteränderungen MÜSSEN über Interface-Versionierung behandelt werden.
12. Parameter, Typen, Constraints und Standardwerte MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-INPUT-0001`
- `NPSPEC-CAPABILITY-OUTPUT-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaOS besitzt ein einheitliches typisiertes Parametermodell für Capability-Operationen. Parameter können Ausführungen präzise konfigurieren, während Eingabedaten, Provider-Implementierung und tatsächliche Authority konsequent davon getrennt bleiben.