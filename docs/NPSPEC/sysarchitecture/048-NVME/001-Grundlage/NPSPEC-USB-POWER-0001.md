# NPSPEC-USB-POWER-0001 – USB Power Management

## Status

Angenommen

## Kategorie

USB / Power / Energieverwaltung

## Zweck

Diese Spezifikation definiert Energiezustände und Strombudgetierung für USB-Geräte und Hubs.

## Energiezustände

NovaOS MUSS folgende Zustände modellieren:

- Powered,
- Default,
- Addressed,
- Configured,
- Suspended,
- Resuming,
- Off.

## Strombudget

Hubs und Ports MÜSSEN ein Strombudget führen. Geräte, deren Configuration mehr Strom anfordert als verfügbar, DÜRFEN nicht aktiviert werden.

## Suspend und Resume

USB Core MUSS Suspend/Resume kontrolliert an Controller und Class Driver weiterreichen.

## Sicherheit

Geräte DÜRFEN nicht durch fehlerhafte Descriptoren unbegrenzte Energieanforderungen erzwingen.

## Akzeptanzkriterien

- Suspend stoppt periodische Transfers.
- Resume reaktiviert konfigurierte Endpoints.
- Overbudget führt zu kontrollierter Ablehnung.

