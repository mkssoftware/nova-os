# NPSPEC-BOOTCOMPOSITOR-0008
# Transparency Fallback

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0008 |
| Titel | Transparency Fallback |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0005 bis NPSPEC-BOOTCOMPOSITOR-0007, NPSPEC-BOOTRENDER-0018 |
| Zugehörige ADRs | ADR-BOOTDESIGN-0003, ADR-BOOTGFX-0010, ADR-BOOTGFX-0011, ADR-BOOTPERF-0004, ADR-BOOTSAFE-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Transparency Fallback System** der NovaOS Boot UI.

Das System stellt sicher, dass sämtliche Oberflächen auch dann korrekt dargestellt werden, wenn Transparenz-, Blur- oder Materialeffekte nicht verfügbar oder deaktiviert sind.

Das Fallback-System garantiert:

- vollständige Bedienbarkeit
- identisches Layout
- konsistente Farben
- ausreichenden Kontrast
- deterministische Darstellung
- stabile Performance

Die Benutzeroberfläche darf niemals unlesbar oder unbenutzbar werden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Materialsysteme der Bootoberfläche.

Sie umfasst:

- Glass Material
- Acrylic Material
- Overlay Layer
- Dialoge
- Panels
- Menüs
- Popups
- Benachrichtigungen

Nicht Bestandteil sind:

- Icons
- Schriftarten
- Bilder

---

# 3. Architektur

```text
Material

↓

Capability Check

↓

Fallback Decision

↓

Fallback Material

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Jede Oberfläche besitzt:

- Primärdarstellung
- Fallbackdarstellung

Kann die Primärdarstellung nicht verwendet werden, erfolgt automatisch der Wechsel auf das Fallback.

Die Umschaltung erfolgt vollständig transparent für den Benutzer. Moderne Oberflächensysteme verwenden vergleichbare Strategien und ersetzen Transparenzmaterialien bei ungeeigneten Bedingungen durch opake Darstellungen. :contentReference[oaicite:0]{index=0}

---

# 5. Auslöser

Ein Fallback wird aktiviert bei:

- Safe Mode
- deaktiviertem Blur
- deaktivierter Transparenz
- Speichermangel
- Renderfehler
- Qualitätsprofil "Safe"
- Recovery Mode

---

# 6. Fallback-Stufen

Das System unterstützt:

| Stufe | Beschreibung |
|---|---|
| Level 0 | Vollständiges Material |
| Level 1 | Blur reduziert |
| Level 2 | Blur deaktiviert |
| Level 3 | Transparenz reduziert |
| Level 4 | Volltonfläche |
| Level 5 | Safe UI |

---

# 7. Materialabbildung

| Material | Fallback |
|---|---|
| Glass | Halbtransparente Fläche |
| Acrylic | Getönte Fläche |
| Blur | Volltonfarbe |
| Glow | deaktiviert |
| Shadow | vereinfacht |

---

# 8. Layout

Ein Fallback darf niemals:

- Größen ändern
- Positionen ändern
- Abstände verändern
- Z-Reihenfolgen verändern

Nur die Darstellung darf angepasst werden.

---

# 9. Farben

Fallbackfarben stammen ausschließlich aus den Nova Design Tokens.

Direkte Farbwerte sind unzulässig.

---

# 10. Transparenz

Die Transparenz wird schrittweise reduziert.

```text
100 %

↓

80 %

↓

60 %

↓

40 %

↓

0 %
```

Der Übergang erfolgt abhängig vom Qualitätsprofil.

---

# 11. Blur

Blur wird in mehreren Stufen reduziert.

```text
16 px

↓

12 px

↓

8 px

↓

4 px

↓

0 px
```

---

# 12. Hintergrund

Ist Blur deaktiviert, wird stattdessen eine statische Hintergrundfarbe verwendet.

Diese muss ausreichend Kontrast zum Inhalt bieten.

---

# 13. Rahmen

Auch im Fallback bleiben:

- Rahmen
- Rundungen
- Konturen

vollständig erhalten.

Dadurch bleibt die Materialstruktur erkennbar.

---

# 14. Schatten

Fallback-Regeln:

- Ambient Shadow → deaktivieren
- Drop Shadow → vereinfachen
- Inner Shadow → deaktivieren

---

# 15. Glow

Glow wird als erstes deaktiviert.

Die Bedienbarkeit darf dadurch nicht beeinträchtigt werden.

---

# 16. Gradienten

Gradienten dürfen reduziert werden auf:

- einfacher Verlauf
- Volltonfläche

---

# 17. Noise

Materialrauschen wird vollständig entfernt.

---

# 18. Kontrast

Der Kontrast wird automatisch erhöht.

Text muss jederzeit eindeutig lesbar bleiben.

---

# 19. Safe Mode

Im Safe Mode bestehen sämtliche Materialien aus:

- Volltonfarbe
- Rahmen
- Text

Alle Effekte werden deaktiviert.

---

# 20. Qualitätsprofile

Die Rendering Quality Profiles bestimmen die aktive Fallback-Stufe.

Je niedriger das Profil, desto stärker wird die Materialvereinfachung.

---

# 21. Automatische Umschaltung

Ein Wechsel erfolgt automatisch bei:

- Renderfehler
- Speichermangel
- manueller Benutzerwahl
- Recovery
- Diagnosemodus

Ein Neustart des Bootmanagers ist nicht erforderlich.

---

# 22. Speicherverwaltung

Fallback-Materialien benötigen:

- keine Blur Surface
- keinen Noise Cache
- keine zusätzlichen Effektflächen

Dadurch reduziert sich der Speicherbedarf erheblich.

---

# 23. Performance

Fallback-Materialien reduzieren:

- Blur
- Glow
- Schatten
- Materialberechnung
- Backdrop Processing

Dadurch sinkt die Renderzeit.

---

# 24. Fehlerbehandlung

Kann weder Material noch Fallback erzeugt werden:

```text
Glass

↓

Fallback

↓

Vollton

↓

Safe UI
```

Der Bootmanager bleibt jederzeit bedienbar.

---

# 25. Sicherheit

Das System verhindert:

- rekursive Fallbacks
- ungültige Materialzustände
- doppelte Umschaltungen
- Nullpointer
- ungültige Qualitätsstufen

---

# 26. API

```c
nova_result_t
nova_material_apply_fallback(
    nova_material_t* material);

bool
nova_material_requires_fallback(
    const nova_material_t* material);

void
nova_material_set_fallback_level(
    uint32_t level);

uint32_t
nova_material_get_fallback_level(void);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- aktiver Fallback-Level
- Blur deaktiviert
- Transparenz deaktiviert
- Materialtyp
- Speicherersparnis
- Renderzeit
- Umschaltungen
- Fehlerursache

---

# 28. Testfälle

Zu testen sind:

- Blur deaktiviert
- Transparenz deaktiviert
- Safe Mode
- Recovery
- Qualitätswechsel
- Speichermangel
- Renderfehler
- Materialwechsel
- Themewechsel
- Vollständiger Rückfall

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Materialien ein definiertes Fallback besitzen.
- Layout und Bedienbarkeit unverändert bleiben.
- automatische Umschaltungen korrekt funktionieren.
- Safe Mode vollständig ohne Transparenzeffekte arbeitet.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 30. Referenzen

## ADRs

- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTPERF-0004 – Automatische Effektdegradation
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0005 – Backdrop Processing
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTCOMPOSITOR-0007 – Acrylic Material
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTRENDER-0019 – Software Rendering Fallback

---

# 31. Zusammenfassung

Das **Transparency Fallback System** gewährleistet, dass die NovaOS-Bootoberfläche auch ohne Transparenz-, Blur- oder Materialeffekte vollständig funktionsfähig bleibt. Durch mehrstufige Materialvereinfachung, automatische Umschaltung und die konsequente Trennung von Layout und Darstellung bleibt die Benutzeroberfläche unter allen Hardware- und Fehlerbedingungen konsistent, performant und jederzeit bedienbar. Moderne UI-Systeme verfolgen ein vergleichbares Prinzip, indem Transparenzmaterialien unter ungeeigneten Bedingungen automatisch durch opake Oberflächen ersetzt werden. :contentReference[oaicite:1]{index=1}