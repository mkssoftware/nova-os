# NPSPEC-BOOTDESIGN-0011
# Interaction States

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0011 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTINPUT-0008 |

---

# 1. Ziel

Diese Spezifikation definiert das **Interaction State System** der NovaOS-Bootoberfläche.

Interaction States beschreiben sämtliche visuellen und logischen Zustände, die UI-Komponenten während ihrer Lebensdauer annehmen können. Ziel ist ein konsistentes Verhalten aller Steuerelemente unabhängig vom jeweiligen Theme oder der zugrunde liegenden Implementierung.

Alle Zustände werden semantisch definiert und über Design Tokens, Motion Tokens und Theme Tokens dargestellt.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Buttons
- Listen
- Menüs
- Dialoge
- Textfelder
- Schalter
- Slider
- Kontrollkästchen
- sämtliche interaktiven UI-Komponenten

---

# 3. Ziele

Das Interaction-State-System muss:

- konsistente Zustände definieren
- deterministisch arbeiten
- Themewechsel unterstützen
- Accessibility unterstützen
- Animationen integrieren
- leicht erweiterbar sein

---

# 4. Architektur

```text
Benutzereingabe

↓

Input System

↓

State Manager

↓

Design Tokens

↓

Motion Engine

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- State Manager
- Interaction Registry
- Theme Integration
- Motion Integration
- Validation Engine
- Accessibility Manager

---

# 6. Grundprinzip

Jede interaktive Komponente besitzt einen definierten Zustand.

```text
Pointer

↓

Interaction State

↓

Theme

↓

Renderer
```

Mehrere Zustände können gleichzeitig aktiv sein, sofern dies ausdrücklich erlaubt ist.

---

# 7. Standardzustände

Pflichtzustände:

```text
Normal

Hover

Focused

Pressed

Selected

Checked

Expanded

Collapsed

Disabled

ReadOnly

Loading

Error
```

Diese Zustände gelten systemweit.

---

# 8. Zustandsübergänge

Zustände dürfen ausschließlich über definierte Übergänge wechseln.

Beispiel:

```text
Normal

↓

Hover

↓

Pressed

↓

Selected

↓

Normal
```

Ungültige Übergänge sind unzulässig.

---

# 9. Visuelle Darstellung

Jeder Zustand kann definieren:

- Farbe
- Hintergrund
- Rahmen
- Transparenz
- Schatten
- Radius
- Cursor
- Animation

Die Darstellung erfolgt ausschließlich über semantische Tokens.

---

# 10. Fokus

Der Fokuszustand besitzt höchste Priorität.

Er muss:

- eindeutig sichtbar sein
- unabhängig von Farben erkennbar bleiben
- Themefähig sein

Die Darstellung erfolgt über Focus Tokens.

---

# 11. Hover

Hover ist ausschließlich für Zeigegeräte verfügbar.

Touchgeräte erzeugen keinen dauerhaften Hover-Zustand.

---

# 12. Disabled

Deaktivierte Komponenten:

- reagieren nicht auf Eingaben
- erhalten reduzierte visuelle Hervorhebung
- bleiben lesbar

Disabled darf niemals mit ReadOnly verwechselt werden.

---

# 13. ReadOnly

ReadOnly-Komponenten:

- können Fokus erhalten
- erlauben Textauswahl (falls zutreffend)
- verhindern Änderungen

Die Darstellung unterscheidet sich von Disabled.

---

# 14. Loading

Während des Ladezustands:

- Eingaben können blockiert werden
- Aktivitätsindikatoren angezeigt werden
- Animationen verwendet werden

Nach Abschluss erfolgt automatisch der Zustandswechsel.

---

# 15. Error

Fehlerzustände müssen:

- eindeutig sichtbar sein
- lesbar bleiben
- nicht ausschließlich über Farbe dargestellt werden

Zusätzliche Symbole oder Texte werden empfohlen.

---

# 16. Accessibility

Accessibility unterstützt:

- stärkere Fokusdarstellung
- reduzierte Animationen
- höhere Kontraste
- größere Interaktionsflächen

Alle Zustände bleiben semantisch identisch.

---

# 17. Themeintegration

Themes dürfen verändern:

- Farben
- Schatten
- Animationen
- Transparenzen
- Rahmen

Die Zustandsnamen bleiben unverändert.

---

# 18. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Zustandsdefinitionen
- fehlende Tokens
- ungültige Übergänge
- doppelte Zustandsdefinitionen

Ungültige Themes werden abgelehnt.

---

# 19. Sicherheit

Das System verhindert:

- ungültige Zustandswechsel
- rekursive Zustände
- fehlende Zustandsdefinitionen
- inkonsistente Darstellungen

Alle Zustandsdefinitionen werden validiert.

---

# 20. Speicherverwaltung

Das Interaction-State-System verwendet:

- statische Zustandsdefinitionen
- Theme-Puffer
- State-Pools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 21. Performance

Anforderungen:

- O(1)-State Lookup
- O(1)-Theme Lookup
- O(1)-Token Lookup
- deterministische Laufzeit

Zustandsinformationen werden beim Theme-Laden vorbereitet.

---

# 22. API

```c
typedef enum
{
    NOVA_STATE_NORMAL,
    NOVA_STATE_HOVER,
    NOVA_STATE_FOCUSED,
    NOVA_STATE_PRESSED,
    NOVA_STATE_SELECTED,
    NOVA_STATE_DISABLED,
    NOVA_STATE_READONLY,
    NOVA_STATE_LOADING,
    NOVA_STATE_ERROR
}
nova_interaction_state_t;

const nova_state_style_t*
nova_state_get(
    nova_interaction_state_t state);

bool
nova_state_transition_allowed(
    nova_interaction_state_t current,
    nova_interaction_state_t next);

nova_result_t
nova_state_reload(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- aktive Zustände
- Zustandswechsel
- ungültige Übergänge
- Validierungsfehler
- Ladezeit

---

# 24. Testfälle

Zu testen sind:

- Hover
- Fokus
- Pressed
- Disabled
- ReadOnly
- Loading
- Error
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Komponenten ausschließlich definierte Interaction States verwenden.
- Zustandswechsel deterministisch erfolgen.
- Themewechsel ohne Änderungen an Komponenten möglich sind.
- Accessibility alle Zustände unterstützt.
- ungültige Zustandswechsel erkannt werden.
- ReadOnly und Disabled klar unterschieden werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Interaktionsverhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDESIGN-0011 – Interaction States
- ADR-BOOTCONTROL-0002 – Control State Model
- ADR-BOOTMOTION-0001 – Motion Engine
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTINPUT-0008 – Focus Management

---

# 27. Zusammenfassung

Das **Interaction State System** definiert die standardisierten Zustände sämtlicher interaktiver Komponenten der NovaOS-Bootoberfläche. Es beschreibt Zustandsmodelle, erlaubte Übergänge und deren visuelle Darstellung vollständig über semantische Design-, Theme- und Motion-Tokens. Durch zentrale Zustandsverwaltung, Themeintegration, Accessibility-Unterstützung und deterministische Verarbeitung gewährleistet das System ein konsistentes, vorhersehbares und langfristig wartbares Interaktionsverhalten auf BIOS- und UEFI-basierten Bootoberflächen.