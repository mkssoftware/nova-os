# NPSPEC-USB-ENUMERATION-0001 – USB Enumeration

## Status

Angenommen

## Kategorie

USB / Enumeration / Geräteerkennung

## Zweck

Diese Spezifikation definiert den Ablauf, mit dem NovaOS USB-Geräte erkennt, adressiert, beschreibt und an passende Treiber bindet.

## Ablauf

```text
Port Connect
  -> Debounce
  -> Reset Port
  -> Read Device Descriptor
  -> Assign Address
  -> Read Configurations
  -> Select Configuration
  -> Create Interfaces
  -> Bind Class Driver
```

## Anforderungen

- Enumeration MUSS pro Port serialisiert werden.
- Geräteadressen MÜSSEN eindeutig pro Bus vergeben werden.
- Descriptoren MÜSSEN längen- und typvalidiert werden.
- Fehlerhafte Geräte DÜRFEN andere Ports nicht blockieren.

## Timeouts

Jeder Enumeration-Schritt MUSS einen Timeout besitzen. Ein Timeout setzt nur das betroffene Gerät oder den betroffenen Port zurück.

## Akzeptanzkriterien

- Neu verbundene Geräte erhalten eine Adresse.
- Mehrere Interfaces eines Geräts können unterschiedliche Class Driver erhalten.
- Fehlerhafte Descriptoren führen zu einem `EnumerationFailed`-Zustand.

