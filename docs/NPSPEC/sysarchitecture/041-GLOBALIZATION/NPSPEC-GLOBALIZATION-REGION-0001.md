# NPSPEC-GLOBALIZATION-REGION-0001 – Nova Region Model

## Status

Angenommen

## Kategorie

Globalization / Region

## Zweck

NovaOS definiert ein einheitliches Regionsmodell für geografisch und regional abhängige Systemeinstellungen.

Die Region beschreibt regionale Konventionen und Standards, bleibt jedoch von Sprache, Locale, Zeitzone, physischem Standort und internen Systemidentitäten getrennt.

## Grundprinzipien

```text
Region ≠ Language
Region ≠ Locale
Region ≠ Time Zone
Region ≠ Physical Location
Region ≠ Citizenship
Region ≠ Authority
```

## Modell

```text
RegionContext
├── RegionID
├── MeasurementSystem
├── CurrencyPreference
├── PaperFormat
├── WeekConvention
└── RegionalStandards
```

`RegionID` verwendet eine standardisierte, maschinenlesbare Regionskennung.

Beispiele:

```text
DE
AT
CH
US
GB
JP
```

## Regionskontexte

NovaOS unterstützt unterschiedliche Regionskontexte:

```text
System Region
User Region
Session Region
Program Region
Solution Region
```

Ein spezifischerer Kontext darf einen allgemeineren Kontext überschreiben.

```text
System
  ↓
User
  ↓
Session
  ↓
Program / Solution
```

## Regionale Eigenschaften

Eine Region darf Standardwerte für folgende Bereiche bereitstellen:

```text
Measurement System
Currency Preference
Paper Format
First Day of Week
Weekend Convention
Regional Standards
```

Diese Werte sind Vorgaben und dürfen durch explizite Nutzereinstellungen überschrieben werden, sofern keine technischen oder gesetzlichen Einschränkungen bestehen.

## Sprache und Locale

Region, Sprache und Locale bleiben getrennte Konzepte.

```text
Language: de
Region: CH
Locale: de-CH
```

Ein Nutzer darf beispielsweise Deutsch als Sprache verwenden und gleichzeitig regionale Einstellungen eines anderen Landes wählen.

## Zeitzone

Die Region bestimmt nicht automatisch die Zeitzone.

```text
Region: DE
Time Zone: Europe/Berlin

Region: DE
Time Zone: America/New_York
```

Beide Einstellungen müssen unabhängig konfigurierbar bleiben.

## Physischer Standort

NovaOS darf aus der eingestellten Region keinen tatsächlichen geografischen Standort des Nutzers ableiten.

Ebenso darf eine Standortbestimmung die konfigurierte Region nicht ohne ausdrückliche Entscheidung des Nutzers verändern.

## Regionale Standards

Hardware, Funk, Medien oder andere Komponenten dürfen regionsabhängige regulatorische Anforderungen besitzen.

Solche zwingenden Einschränkungen müssen von reinen Darstellungs- und Nutzerpräferenzen getrennt bleiben.

```text
User Region Preference
        ≠
Regulatory Constraint
```

Eine frei gewählte Region darf keine Sicherheits- oder regulatorischen Beschränkungen umgehen.

## Laufzeitwechsel

Die Region soll während einer laufenden Sitzung geändert werden können.

Davon abhängige Komponenten sollen ihre Darstellung und Standardwerte aktualisieren können, ohne interne Identitäten oder gespeicherte kanonische Daten zu verändern.

## Normative Anforderungen

1. NovaOS MUSS Region unabhängig von Sprache und Locale modellieren.
2. Regionskennungen MÜSSEN standardisiert und maschinenlesbar sein.
3. Region und Zeitzone MÜSSEN unabhängig konfigurierbar sein.
4. Region und physischer Standort MÜSSEN getrennt bleiben.
5. Region DARF regionale Standardwerte bereitstellen.
6. Nutzerpräferenzen SOLLEN regionale Standardwerte überschreiben können.
7. Regionale Darstellungsregeln DÜRFEN kanonisch gespeicherte Werte nicht verändern.
8. Ein Regionswechsel DARF keine internen Systemidentitäten verändern.
9. Eine konfigurierte Region DARF nicht als Nachweis des tatsächlichen Standorts verwendet werden.
10. Regulatorische Einschränkungen DÜRFEN nicht allein durch Änderung der Region umgangen werden.
11. Regionswechsel SOLLEN zur Laufzeit möglich sein.
12. Effektive Region und angewendete regionale Standards MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-SYSTEM-LOCALE-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS behandelt die Region als eigenständigen Globalization-Kontext. Regionale Standards und Vorgaben können unabhängig von Sprache, Locale, Zeitzone und physischem Standort gewählt werden, ohne stabile Systemidentitäten oder kanonische Daten zu verändern.