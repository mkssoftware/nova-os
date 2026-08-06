# NPSPEC-BOOTDESIGN-0010
# Motion Tokens

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDESIGN-0010 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Design System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTDESIGN-0002, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTMOTION-0002, NPSPEC-BOOTTHEME-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Motion Tokens** der NovaOS-Bootoberfläche.

Motion Tokens stellen sämtliche Bewegungsparameter der Benutzeroberfläche als semantische Design Tokens bereit. Dazu gehören Animationsdauern, Easing-Kurven, Übergänge und Bewegungsprofile. Dadurch wird sichergestellt, dass alle Animationen konsistent, austauschbar und themefähig bleiben.

UI-Komponenten enthalten keinerlei fest kodierte Animationsparameter.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Seitenübergänge
- Dialoganimationen
- Fokusanimationen
- Hovereffekte
- Statusanimationen
- Ladeanimationen
- Glasmaterial-Animationen
- Layoutübergänge
- sämtliche UI-Komponenten

---

# 3. Ziele

Das Motion-Token-System muss:

- konsistente Animationen gewährleisten
- semantische Bewegungsprofile verwenden
- Themewechsel unterstützen
- deterministisch arbeiten
- Accessibility berücksichtigen
- leicht erweiterbar sein

---

# 4. Architektur

```text
Theme

↓

Motion Tokens

↓

Motion Engine

↓

Animation Player

↓

UI-Komponenten
```

---

# 5. Komponenten

Das System besteht aus:

- Motion Registry
- Motion Token Resolver
- Theme Integration
- Motion Cache
- Validation Engine
- Accessibility Manager

---

# 6. Grundprinzip

Alle Animationen verwenden ausschließlich Motion Tokens.

```text
Button Hover

↓

Motion.Fast

↓

Theme

↓

Motion Engine
```

Feste Animationsdauern oder Easing-Kurven innerhalb von Komponenten sind nicht zulässig.

---

# 7. Motion Token Kategorien

Pflichtkategorien:

- Duration
- Easing
- Transition
- Emphasis
- Feedback
- Navigation
- Loading
- Focus

Neue Kategorien können ergänzt werden.

---

# 8. Duration Tokens

Pflichttokens:

```text
Motion.Instant

Motion.UltraFast

Motion.Fast

Motion.Normal

Motion.Slow

Motion.VerySlow
```

Das Theme definiert die tatsächlichen Zeitwerte.

---

# 9. Easing Tokens

Unterstützt werden:

```text
Motion.Linear

Motion.EaseIn

Motion.EaseOut

Motion.EaseInOut

Motion.Decelerate

Motion.Accelerate

Motion.Spring

Motion.Overshoot
```

Die mathematischen Parameter werden vom Theme bereitgestellt.

---

# 10. Transition Tokens

Beispiele:

```text
Motion.Page

Motion.Dialog

Motion.Menu

Motion.Tooltip

Motion.Card

Motion.Focus

Motion.Selection
```

Diese Tokens definieren standardisierte Übergänge.

---

# 11. Feedback Tokens

Interaktive Rückmeldungen:

- Hover
- Pressed
- Focus
- Selected
- Activated
- Completed
- Error

Jeder Zustand besitzt ein eigenes Motion Token.

---

# 12. Bewegungsprinzipien

Animationen dienen ausschließlich:

- Orientierung
- Zustandswechsel
- Fokusführung
- Benutzerfeedback

Animationen dürfen niemals dekorativen Selbstzweck erfüllen.

---

# 13. Synchronisation

Mehrere Animationen können synchronisiert werden.

Dabei werden:

- gemeinsame Startzeit
- gemeinsame Dauer
- gemeinsame Easing-Kurve

über Motion Tokens definiert.

---

# 14. Accessibility

Accessibility kann:

- Animationsdauer verkürzen
- Animationen deaktivieren
- vereinfachte Bewegungen verwenden

Die semantischen Motion Tokens bleiben unverändert.

---

# 15. Themeintegration

Themes dürfen verändern:

- Animationsdauer
- Easing-Kurven
- Bewegungsprofile
- Prioritäten

Die Namen der Motion Tokens bleiben unverändert.

---

# 16. Validierung

Vor der Aktivierung eines Themes werden geprüft:

- vollständige Motion Tokens
- ungültige Zeitwerte
- fehlende Easing-Kurven
- doppelte Tokens

Ungültige Themes werden abgelehnt.

---

# 17. Sicherheit

Das System verhindert:

- negative Animationszeiten
- ungültige Easing-Parameter
- rekursive Motion Tokens
- doppelte Tokendefinitionen

Alle Motion Tokens werden validiert.

---

# 18. Speicherverwaltung

Das Motion-System verwendet:

- statische Tokentabellen
- Motion Cache
- Theme-Puffer

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Motion Lookup
- O(1)-Theme Lookup
- O(1)-Token Lookup
- deterministische Laufzeit

Alle Motion Tokens werden beim Theme-Laden vorbereitet.

---

# 20. API

```c
typedef enum
{
    NOVA_MOTION_INSTANT,
    NOVA_MOTION_ULTRA_FAST,
    NOVA_MOTION_FAST,
    NOVA_MOTION_NORMAL,
    NOVA_MOTION_SLOW,
    NOVA_MOTION_VERY_SLOW
}
nova_motion_token_t;

const nova_motion_style_t*
nova_motion_get(
    nova_motion_token_t token);

bool
nova_motion_exists(
    nova_motion_token_t token);

nova_result_t
nova_motion_reload(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Theme-ID
- Anzahl Motion Tokens
- Animationsprofile
- Easing-Kurven
- Validierungsfehler
- Ladezeit

---

# 22. Testfälle

Zu testen sind:

- Hoveranimationen
- Dialoganimationen
- Seitenwechsel
- Fokusanimationen
- Themewechsel
- Accessibility
- deaktivierte Animationen
- ungültige Motion Tokens
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen ausschließlich Motion Tokens verwenden.
- Themewechsel ohne Änderungen an UI-Komponenten möglich sind.
- alle Bewegungsprofile semantisch definiert sind.
- Accessibility Bewegungen reduzieren oder deaktivieren kann.
- ungültige Motion Tokens erkannt werden.
- keine festen Animationsparameter in Komponenten gespeichert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Bewegungsverhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTDESIGN-0010 – Motion Tokens
- ADR-BOOTDESIGN-0002 – Design Tokens
- ADR-BOOTMOTION-0001 – Motion Engine
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTDESIGN-0002 – Boot Design Tokens
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTTHEME-0001 – Theme System

---

# 25. Zusammenfassung

Die **Motion Tokens** definieren die semantische Grundlage sämtlicher Bewegungsabläufe der NovaOS-Bootoberfläche. Sie abstrahieren Animationsdauern, Easing-Kurven, Übergänge und Bewegungsprofile in wiederverwendbare Design Tokens und trennen dadurch Animationslogik von UI-Komponenten. Durch Themeintegration, Accessibility-Unterstützung, deterministische Verarbeitung und statische Speicherverwaltung gewährleisten Motion Tokens eine konsistente, hochwertige und langfristig wartbare Bewegungsarchitektur für BIOS- und UEFI-basierte Bootoberflächen.