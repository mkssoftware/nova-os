# NPSPEC-BOOTCONTROL-SPINNER-0001
# Activity Indicator

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-SPINNER-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTMOTION-0011, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Activity Indicator (Spinner)** der NovaOS Boot UI.

Der Activity Indicator signalisiert, dass eine Operation aktiv ausgeführt wird, deren Fortschritt jedoch nicht numerisch bestimmt werden kann.

Er dient ausschließlich zur Visualisierung laufender Aktivitäten.

Typische Einsatzbereiche:

- Initialisierung
- Hardwareerkennung
- Geräteprüfung
- Netzwerkverbindung
- Modulinitialisierung
- Recovery
- Datenträgersuche
- Hintergrundoperationen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Activity Indicators.

Sie umfasst:

- Spinner
- Progress Ring
- Dot Spinner
- Circular Loader

Nicht Bestandteil sind:

- determinate Progress Bars
- Slider
- Scrollbars

---

# 3. Architektur

```text
Boot Control Framework

↓

Activity Indicator

↓

Motion System

↓

Renderer
```

---

# 4. Grundprinzip

Ein Activity Indicator besitzt keinen numerischen Fortschritt.

Er zeigt ausschließlich an, dass eine Operation aktiv ausgeführt wird.

Der Spinner darf niemals einen konkreten Fortschrittswert suggerieren.

---

# 5. Aufgaben

Der Activity Indicator übernimmt:

- Aktivitätsanzeige
- Animation
- Themeintegration
- Accessibility
- Statusdarstellung

---

# 6. Aufbau

```text
Activity Indicator

├── Track (optional)

├── Spinner

├── Status Label (optional)

└── Animation Layer
```

---

# 7. Eigenschaften

Jeder Activity Indicator besitzt:

- ID
- Stil
- Größe
- Sichtbarkeit
- Enabled
- Style
- Template

Optional:

- Beschriftung
- Statusmeldung
- Timeout

---

# 8. Typen

Unterstützt werden:

- Circular Spinner
- Progress Ring
- Dot Spinner
- Arc Spinner

Die Auswahl erfolgt über das Template.

---

# 9. Zustände

```text
Idle

Running

Paused

Completed

Cancelled

Hidden
```

Optional:

```text
Error
```

---

# 10. Animation

Die Animation läuft kontinuierlich.

Eigenschaften:

- gleichmäßige Bewegung
- konstante Geschwindigkeit
- keine Sprünge
- endlose Wiederholung

Die Animation endet erst nach Abschluss der Operation.

---

# 11. Darstellung

Der Activity Indicator verwendet:

- Glass Material
- Themefarben
- Transparenz
- Glow
- Schatten

Die Darstellung wird vollständig über Theme und Template definiert.

---

# 12. Animationstypen

Unterstützt werden:

- Rotation
- Pulsieren
- Punktanimation
- Segmentrotation
- Ringanimation

Alle Animationen werden durch das Motion-System gesteuert.

---

# 13. Statusanzeige

Optional kann ein Label dargestellt werden.

Beispiele:

```text
Starte System ...

Initialisiere Geräte ...

Lade Module ...

Verbinde Netzwerk ...
```

Die Statusanzeige ist unabhängig von der Animation.

---

# 14. Themeintegration

Das Theme definiert:

- Farben
- Transparenz
- Schatten
- Material
- Animationsparameter

Alle visuellen Eigenschaften stammen aus dem Theme.

---

# 15. Accessibility

Jeder Activity Indicator besitzt:

- Name
- Rolle
- Beschreibung
- Status

Screenreader geben beispielsweise aus:

```text
Operation wird ausgeführt
```

Der Spinner selbst wird nicht beschrieben.

---

# 16. Ressourcenverwaltung

Activity Indicators verwenden:

- gemeinsame Templates
- gemeinsame Styles
- zentrale Animationsdefinitionen

Animationen werden mehrfach genutzt.

---

# 17. Speicherverwaltung

Das Control verwendet:

- Objektpools
- statische Templates
- gemeinsame Ressourcen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Animationsschritt
- O(1)-Statuswechsel
- deterministische Laufzeit

Animationen dürfen keine unnötigen Neuberechnungen erzeugen.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Animationsstil
- beschädigtes Template
- fehlender Style
- Nullpointer

Kann keine Animation dargestellt werden, wird eine statische Aktivitätsanzeige verwendet.

---

# 20. Sicherheit

Das System verhindert:

- mehrere Animationen pro Control
- inkonsistente Zustände
- rekursive Animationen
- beschädigte Motion-Daten

Nach Abschluss wird die Animation sofort beendet.

---

# 21. API

```c
typedef struct nova_activity_indicator nova_activity_indicator_t;

typedef enum
{
    NOVA_ACTIVITY_SPINNER,
    NOVA_ACTIVITY_RING,
    NOVA_ACTIVITY_DOTS,
    NOVA_ACTIVITY_ARC
}
nova_activity_style_t;

nova_result_t
nova_activity_create(
    nova_activity_indicator_t** indicator);

nova_result_t
nova_activity_start(
    nova_activity_indicator_t* indicator);

nova_result_t
nova_activity_stop(
    nova_activity_indicator_t* indicator);

nova_result_t
nova_activity_set_style(
    nova_activity_indicator_t* indicator,
    nova_activity_style_t style);

bool
nova_activity_running(
    const nova_activity_indicator_t* indicator);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Indicator-ID
- Animationsstil
- Laufzeit
- Statuswechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Spinner
- Ring
- Dots
- Arc
- Start
- Stop
- Themewechsel
- Accessibility
- Animation
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationstypen korrekt dargestellt werden.
- keine Fortschrittswerte suggeriert werden.
- Animationen flüssig und deterministisch laufen.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-SPINNER-0001 – Activity Indicator
- ADR-BOOTMOTION-0011 – Progress Animations
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTMOTION-0011 – Progress Animations
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Der **Activity Indicator** definiert das standardisierte Steuerelement zur Darstellung laufender, nicht quantifizierbarer Operationen innerhalb der NovaOS-Bootoberfläche. Er unterstützt verschiedene Animationsformen wie Spinner, Ring oder Punktanimationen, besitzt keine numerische Fortschrittsanzeige und integriert sich vollständig in das Theme-, Motion- und Accessibility-Framework. Durch deterministische Animationen, zentrale Ressourcenverwaltung und statische Speicherstrukturen entsteht ein performantes und konsistentes UI-Control für BIOS- und UEFI-basierte Bootumgebungen.