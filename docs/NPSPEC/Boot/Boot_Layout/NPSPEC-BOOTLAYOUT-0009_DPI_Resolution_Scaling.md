# NPSPEC-BOOTLAYOUT-0009
# DPI and Resolution Scaling

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0009 |
| Titel | DPI and Resolution Scaling |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0008, NPSPEC-BOOTUI-0006 |
| Zugehörige ADRs | ADR-BOOTDISPLAY-0002, ADR-BOOTLAYOUT-0011, ADR-BOOTDESIGN-0002, ADR-BOOTACCESS-0003, ADR-BOOTPERF-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **DPI and Resolution Scaling System** der NovaOS Boot UI.

Das System stellt sicher, dass sämtliche Benutzeroberflächenelemente unabhängig von Bildschirmauflösung und Pixeldichte in identischen physischen Proportionen dargestellt werden.

Es unterstützt:

- automatische DPI-Erkennung
- feste Skalierungsstufen
- benutzerdefinierte Skalierung
- BIOS- und UEFI-Grafikmodi
- virtuelle Maschinen
- hochauflösende Displays
- zukünftige Displaytechnologien

Die Skalierung erfolgt zentral und einheitlich für die gesamte Bootoberfläche.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen UI-Elemente.

Sie umfasst:

- Layout
- Schriftgrößen
- Icons
- Buttons
- Dialoge
- Listen
- Abstände
- Fokusrahmen
- Touchflächen

Nicht Bestandteil sind:

- Rendering-Pipeline
- Animationen
- Eingabeverarbeitung

---

# 3. Architektur

```text
Display Detection

↓

Resolution Manager

↓

DPI Calculator

↓

Scaling Engine

↓

Layout Engine

↓

Rendering
```

---

# 4. Grundprinzip

Alle Layoutberechnungen erfolgen zunächst in **logischen Einheiten (Device Independent Units)**.

Erst im letzten Schritt werden diese in physische Pixel umgerechnet.

Direkte Verwendung von Pixelwerten innerhalb der Layoutlogik ist unzulässig.

---

# 5. Aufgaben

Das Skalierungssystem übernimmt:

- DPI-Berechnung
- Skalierungsfaktor
- Umrechnung logischer Größen
- Schriftgrößenanpassung
- Iconskalierung
- Mindestgrößen
- Touchflächen
- Layoutanpassung

---

# 6. Logische Einheiten

Die NovaOS Boot UI verwendet als Basiseinheit:

```text
1 DLU (Device Layout Unit)
```

Alle Größenangaben innerhalb der Layout Engine erfolgen in DLU.

Pixelwerte entstehen ausschließlich während der Renderphase.

---

# 7. Skalierungsfaktor

Der globale Skalierungsfaktor berechnet sich aus:

```text
Logical Size

×

Scale Factor

↓

Physical Pixels
```

Der Faktor gilt für sämtliche UI-Elemente.

---

# 8. Unterstützte Skalierungsstufen

| Skalierung | Faktor |
|---|---:|
| 100 % | 1.00 |
| 125 % | 1.25 |
| 150 % | 1.50 |
| 175 % | 1.75 |
| 200 % | 2.00 |
| 250 % | 2.50 |
| 300 % | 3.00 |

Weitere Faktoren sind zulässig.

---

# 9. DPI-Erkennung

Die DPI werden bevorzugt aus den Firmwareinformationen ermittelt.

Sind keine zuverlässigen Informationen verfügbar, erfolgt die Berechnung anhand:

- Bildschirmauflösung
- Standardreferenz
- Firmwaredaten

Kann keine verlässliche DPI bestimmt werden, wird standardmäßig 96 DPI angenommen.

---

# 10. Schriftgrößen

Alle Schriftgrößen werden skaliert.

Mindestgrößen:

| Element | Mindestgröße |
|---|---:|
| Standardtext | 12 DLU |
| Dialogtitel | 18 DLU |
| Statuszeile | 11 DLU |
| Warntext | 14 DLU |

Die Umrechnung in Pixel erfolgt erst nach Anwendung des Skalierungsfaktors.

---

# 11. Icons

Icons werden ausschließlich aus Vektordaten oder hochauflösenden Rasterquellen skaliert.

Unterstützte Standardgrößen:

- 16 DLU
- 20 DLU
- 24 DLU
- 32 DLU
- 48 DLU
- 64 DLU

---

# 12. Buttons

Buttons besitzen:

- Mindestbreite
- Mindesthöhe
- skalierbare Innenabstände
- skalierbare Icons

Die physische Bedienfläche darf niemals kleiner als die definierte Mindestgröße werden.

---

# 13. Abstände

Folgende Größen werden skaliert:

- Padding
- Margin
- Spacing
- Fokusrahmen
- Border

Das Verhältnis zwischen allen Abständen bleibt erhalten.

---

# 14. Rundung

Nach der Umrechnung auf Pixel werden Werte deterministisch gerundet.

Die Rundungsregeln müssen auf allen Plattformen identisch sein.

Unterschiedliche Rundungsalgorithmen zwischen BIOS und UEFI sind unzulässig.

---

# 15. Mehrere Displays

Der Bootmanager verwendet genau einen aktiven Ausgabemonitor.

Ein Wechsel des Ausgabegerätes führt zu einer vollständigen Neuberechnung:

- DPI
- Skalierung
- Layout
- Safe Area

---

# 16. Accessibility

Accessibility kann zusätzlich vergrößern:

- Schriftgrößen
- Touchflächen
- Dialoggrößen
- Fokusrahmen

Die Accessibility-Skalierung wird nach dem globalen DPI-Faktor angewendet.

---

# 17. Responsive Layout

Responsive Layout und DPI-Skalierung arbeiten unabhängig voneinander.

Responsive Regeln bestimmen die Struktur.

Die DPI-Skalierung bestimmt ausschließlich die physische Größe.

---

# 18. Layout Invalidierung

Eine vollständige Neuberechnung erfolgt bei:

- Auflösungswechsel
- DPI-Wechsel
- Skalierungsänderung
- Displaywechsel
- Accessibility-Änderung

---

# 19. Speicherverwaltung

Das Skalierungssystem verwendet:

- globale Skalierungsparameter
- keine zusätzlichen Layoutkopien
- keine mehrfachen Renderbäume

Heap-Allokationen während der Skalierungsberechnung sind verboten.

---

# 20. Performance

Anforderungen:

- O(n)-Berechnung
- deterministische Ergebnisse
- konstante Skalierungsfaktoren pro Frame
- keine Heap-Allokationen

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige DPI
- Skalierungsfaktor ≤ 0
- beschädigte Displayinformationen
- ungültige Auflösung
- numerische Überläufe

Im Fehlerfall wird automatisch auf 100 % Skalierung mit 96 DPI zurückgegriffen.

---

# 22. Sicherheit

Das System verhindert:

- negative Größen
- unlesbare Schriftgrößen
- abgeschnittene Buttons
- inkonsistente Rundungen
- Nullpointer

---

# 23. API

```c
typedef struct
{
    uint32_t dpi_x;
    uint32_t dpi_y;

    float scale_factor;

} nova_scaling_info_t;

nova_result_t
nova_scaling_initialize(void);

const nova_scaling_info_t*
nova_scaling_get(void);

nova_result_t
nova_scaling_update(
    const nova_display_info_t* display);

int32_t
nova_scale_to_pixels(
    int32_t logical_value);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Bildschirmauflösung
- DPI
- Skalierungsfaktor
- Layoutklasse
- Safe Area
- Rundungsfehler
- Invalidierungen
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- 96 DPI
- 120 DPI
- 144 DPI
- 200 % Skalierung
- 4K
- 8K
- BIOS
- UEFI
- Accessibility
- Displaywechsel

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Größen in logischen Einheiten berechnet werden.
- Pixelwerte ausschließlich während der Renderphase entstehen.
- alle unterstützten Skalierungsstufen korrekt funktionieren.
- Schriften, Icons und Controls proportional skaliert werden.
- Accessibility-Skalierungen korrekt angewendet werden.
- keine Heap-Allokationen während der Skalierungsberechnung stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTDISPLAY-0002 – Einheitliche Auflösungsverwaltung
- ADR-BOOTLAYOUT-0011 – Logische Layouteinheiten
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen
- ADR-BOOTPERF-0001 – Festes Speicherbudget

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0007 – Responsive Boot Layout
- NPSPEC-BOOTLAYOUT-0008 – Safe Display Areas
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **DPI and Resolution Scaling System** definiert die einheitliche Skalierung sämtlicher Benutzeroberflächenelemente der NovaOS Boot UI. Alle Layoutberechnungen erfolgen zunächst in geräteunabhängigen logischen Einheiten und werden erst unmittelbar vor dem Rendering in Pixel umgerechnet. Durch zentrale DPI-Erkennung, deterministische Rundung, adaptive Skalierungsstufen und vollständige Integration in das Responsive- und Accessibility-System entstehen konsistente und hardwareunabhängige Oberflächen auf BIOS- und UEFI-Plattformen.