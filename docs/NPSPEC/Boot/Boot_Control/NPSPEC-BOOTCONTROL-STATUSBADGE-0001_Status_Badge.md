# NPSPEC-BOOTCONTROL-STATUSBADGE-0001
# Status Badge

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-STATUSBADGE-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-ICON-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Status Badge Control** der NovaOS Boot UI.

Ein Status Badge dient zur kompakten Darstellung des aktuellen Zustands eines Objekts oder einer Funktion. Es ergänzt bestehende Controls wie Listen, Tiles oder Dialoge um leicht erkennbare Statusinformationen.

Typische Einsatzbereiche:

- Bootstatus
- Sicherheitsstatus
- Recovery-Zustand
- Gerätezustand
- Update-Status
- Warnhinweise
- Fehlermeldungen
- Versionskennzeichnung

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Status Badges.

Sie umfasst:

- Text-Badges
- Icon-Badges
- Kombinierte Badges
- Farbige Statusanzeigen

Nicht Bestandteil sind:

- Tooltips
- Benachrichtigungen
- Dialoge

---

# 3. Architektur

```text
Boot Control Framework

↓

Status Badge

├── Background

├── Icon (optional)

├── Text (optional)

└── Renderer
```

---

# 4. Grundprinzip

Ein Status Badge beschreibt genau einen Status.

Es ergänzt ein anderes Control und besitzt standardmäßig keine eigenständige Benutzerinteraktion.

Ein Badge darf niemals allein sicherheitskritische Informationen vermitteln.

---

# 5. Aufgaben

Das Status Badge übernimmt:

- Statusdarstellung
- Themeintegration
- Accessibility
- automatische Aktualisierung
- optionale Animationen

---

# 6. Aufbau

```text
Status Badge

├── Background

├── Icon

├── Text

└── Border (optional)
```

Text oder Icon können einzeln oder gemeinsam verwendet werden.

---

# 7. Eigenschaften

Jedes Badge besitzt:

- ID
- Status
- Sichtbarkeit
- Style
- Template

Optional:

- Text
- Symbol
- Tooltip

---

# 8. Badge-Typen

Unterstützt werden:

- Information
- Success
- Warning
- Error
- Critical
- Neutral
- Custom

Der Typ beeinflusst ausschließlich die Darstellung.

---

# 9. Zustände

```text
Visible

Hidden

Disabled
```

Ein Badge besitzt standardmäßig keine Hover-, Fokus- oder Pressed-Zustände.

---

# 10. Statuswerte

Beispiele:

```text
OK

Warnung

Fehler

Aktiv

Inaktiv

Neu

Beta
```

Statuswerte werden vollständig durch das zugehörige Control bestimmt.

---

# 11. Darstellung

Das Badge verwendet:

- Glass Material
- abgerundete Form
- Themefarben
- optionales Symbol
- optionalen Text

Die Darstellung erfolgt ausschließlich über Styles und Templates.

---

# 12. Größen

Empfohlene Größen:

| Typ | Höhe |
|------|------|
| Small | 20 DLU |
| Medium | 24 DLU |
| Large | 32 DLU |

Die Breite richtet sich automatisch nach dem Inhalt.

---

# 13. Animationen

Optional unterstützt:

- Fade
- Color Transition
- Scale
- Status Transition

Animationen dürfen ausschließlich Zustandsänderungen visualisieren.

---

# 14. Aktualisierung

Ein Badge aktualisiert sich automatisch, wenn sich der Status des zugehörigen Objekts ändert.

Der Statuswechsel erfolgt atomar.

---

# 15. Accessibility

Jedes Badge besitzt:

- Name
- Rolle
- Beschreibung
- Status

Screenreader geben den vollständigen Status aus.

Dekorative Badges werden ignoriert.

---

# 16. Themeintegration

Das Theme definiert:

- Hintergrund
- Rahmen
- Farben
- Transparenz
- Schriftarten
- Schatten

Es existieren keine fest codierten Farben.

---

# 17. Speicherverwaltung

Badges verwenden:

- gemeinsame Templates
- gemeinsame Styles
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Statuswechsel
- O(1)-Darstellung
- O(1)-Themewechsel
- deterministische Laufzeit

Nur geänderte Badges werden neu gerendert.

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Status
- beschädigtes Template
- fehlender Style
- Nullpointer

Kann ein Badge nicht dargestellt werden, wird der Status ausschließlich textuell ausgegeben.

---

# 20. Sicherheit

Das System verhindert:

- ungültige Statuskombinationen
- beschädigte Statusreferenzen
- rekursive Statusaktualisierungen
- fehlerhafte Themezuweisungen

Kritische Warnungen müssen zusätzlich textlich dargestellt werden.

---

# 21. API

```c
typedef struct nova_status_badge nova_status_badge_t;

typedef enum
{
    NOVA_BADGE_INFORMATION,
    NOVA_BADGE_SUCCESS,
    NOVA_BADGE_WARNING,
    NOVA_BADGE_ERROR,
    NOVA_BADGE_CRITICAL,
    NOVA_BADGE_NEUTRAL,
    NOVA_BADGE_CUSTOM
}
nova_badge_type_t;

nova_result_t
nova_status_badge_create(
    nova_badge_type_t type,
    nova_status_badge_t** badge);

nova_result_t
nova_status_badge_set_text(
    nova_status_badge_t* badge,
    const char* text);

nova_result_t
nova_status_badge_set_icon(
    nova_status_badge_t* badge,
    nova_icon_t icon);

nova_result_t
nova_status_badge_set_visible(
    nova_status_badge_t* badge,
    bool visible);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Badge-ID
- Badge-Typ
- Statuswechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 23. Testfälle

Zu testen sind:

- Information
- Success
- Warning
- Error
- Critical
- Themewechsel
- Accessibility
- Statuswechsel
- Animationen
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Badge-Typen korrekt dargestellt werden.
- Statusänderungen deterministisch verarbeitet werden.
- Theme und Templates vollständig verwendet werden.
- kritische Status zusätzlich textlich verfügbar sind.
- Accessibility vollständig integriert ist.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTCONTROL-STATUSBADGE-0001 – Status Badge
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-ICON-0001 – Icon
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Status Badge** definiert das standardisierte Steuerelement zur kompakten Darstellung von Statusinformationen innerhalb der NovaOS-Bootoberfläche. Es unterstützt Text-, Symbol- und kombinierte Darstellungen, automatische Statusaktualisierungen, Theme-Integration, Animationen sowie vollständige Accessibility. Durch deterministische Statusverwaltung, zentrale Style-Definitionen und statische Speicherverwaltung entsteht ein konsistentes, leichtgewichtiges und wiederverwendbares UI-Element für BIOS- und UEFI-basierte Bootumgebungen.