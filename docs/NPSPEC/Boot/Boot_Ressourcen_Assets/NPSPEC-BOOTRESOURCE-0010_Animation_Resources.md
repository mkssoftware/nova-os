# NPSPEC-BOOTRESOURCE-0010
# Animation Resources

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0010 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTMOTION-0002, NPSPEC-BOOTMOTION-0003, NPSPEC-BOOTTHEME-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Verwaltung von **Animation Resources** innerhalb der NovaOS-Bootoberfläche.

Animation Resources beschreiben sämtliche animierten UI-Elemente in einer datengetriebenen Form. Sie enthalten ausschließlich Animationsdefinitionen und keine Programmlogik. Die Ausführung erfolgt durch die Boot Motion Engine.

Dadurch können Animationen zentral verwaltet, versioniert, wiederverwendet und durch Themes ausgetauscht werden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Seitenübergänge
- Dialoganimationen
- Fokusanimationen
- Hoveranimationen
- Bootlogo-Animationen
- Ladeanimationen
- Statusanimationen
- Themeanimationen

---

# 3. Ziele

Das System muss:

- Animationen zentral verwalten
- deklarative Animationen unterstützen
- deterministisch arbeiten
- Themeintegration ermöglichen
- Wiederverwendung fördern
- zukünftige Animationstypen unterstützen

---

# 4. Architektur

```text
Boot Asset Package

↓

Animation Resource

↓

Resource Manager

↓

Motion Engine

↓

Animation Player

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Animation Loader
- Animation Registry
- Timeline Resolver
- Motion Resource Manager
- Animation Cache
- Validator

---

# 6. Grundprinzip

Animationen werden nicht programmiert, sondern beschrieben.

```text
Animation Resource

↓

Motion Engine

↓

Timeline

↓

Renderer
```

Die Motion Engine interpretiert die Ressource zur Laufzeit.

---

# 7. Animation Resource

Eine Animation enthält mindestens:

- Animation-ID
- Name
- Version
- Dauer
- Wiederholungsverhalten
- Easing
- Keyframes

Optional:

- Trigger
- Priorität
- Kategorie

---

# 8. Animationstypen

Unterstützt werden:

- Fade
- Scale
- Translation
- Rotation
- Opacity
- Blur
- Glow
- Color Transition

Mehrere Animationen können kombiniert werden.

---

# 9. Timeline

Jede Animation besitzt eine Timeline.

Diese enthält:

- Startzeit
- Endzeit
- Keyframes
- Interpolation

Alle Zeitangaben erfolgen in Millisekunden.

---

# 10. Keyframes

Ein Keyframe beschreibt:

- Zeitpunkt
- Eigenschaft
- Zielwert
- Interpolation

Zwischenwerte werden von der Motion Engine berechnet.

---

# 11. Wiederholung

Unterstützt werden:

- einmalig
- fest definierte Wiederholungsanzahl
- Endlosschleife

Endlosschleifen dürfen nur für ausdrücklich dafür vorgesehene Animationen verwendet werden (z. B. Aktivitätsindikatoren).

---

# 12. Trigger

Animationen können ausgelöst werden durch:

- Fokus
- Klick
- Seitenwechsel
- Dialog öffnen
- Dialog schließen
- Systemereignis

Die Triggerdefinition erfolgt deklarativ.

---

# 13. Themeintegration

Themes können:

- Animationen ersetzen
- Geschwindigkeiten anpassen
- Easing-Kurven ändern
- Animationen deaktivieren

Die Programmlogik bleibt unverändert.

---

# 14. Cache

Bereits geladene Animationen werden registriert.

Mehrfaches Laden derselben Animation ist nicht zulässig.

---

# 15. Sicherheit

Das System verhindert:

- rekursive Animationen
- ungültige Timelines
- beschädigte Ressourcen
- doppelte Animation-IDs
- ungültige Keyframes

Alle Animationen werden vor der Registrierung validiert.

---

# 16. Speicherverwaltung

Das System verwendet:

- statische Timeline-Puffer
- Keyframe-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 17. Performance

Anforderungen:

- O(1)-Animations-Lookup
- O(1)-Timeline-Lookup
- O(n)-Keyframe-Auswertung
- deterministische Laufzeit

Bereits registrierte Animationen werden ausschließlich wiederverwendet.

---

# 18. API

```c
typedef struct
{
    uint32_t animation_id;
    uint32_t duration_ms;
    uint16_t keyframe_count;
}
nova_animation_resource_t;

nova_result_t
nova_animation_resource_initialize(void);

const nova_animation_resource_t*
nova_animation_resource_load(
    uint32_t resource_id);

const nova_animation_resource_t*
nova_animation_resource_find(
    uint32_t animation_id);

bool
nova_animation_resource_validate(
    const nova_animation_resource_t* animation);
```

---

# 19. Diagnosedaten

Zu erfassen sind:

- Animation-ID
- Dauer
- Anzahl Keyframes
- Wiederholungsmodus
- Trigger
- Ladezeit
- Speicherverbrauch
- Fehler

---

# 20. Testfälle

Zu testen sind:

- Fade
- Scale
- Rotation
- Translation
- kombinierte Animationen
- Trigger
- Themewechsel
- beschädigte Ressourcen
- doppelte IDs
- Fehlerfälle

---

# 21. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen ausschließlich über Animation Resources bereitgestellt werden.
- Timelines korrekt verarbeitet werden.
- Keyframes deterministisch ausgewertet werden.
- Themewechsel Animationen austauschen können.
- doppelte Animationen nicht mehrfach geladen werden.
- beschädigte Ressourcen erkannt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 22. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0010 – Animation Resources
- ADR-BOOTMOTION-0001 – Motion Engine
- ADR-BOOTMOTION-0002 – Animation Timeline
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTMOTION-0003 – Animation Player
- NPSPEC-BOOTTHEME-0001 – Theme System

---

# 23. Zusammenfassung

Die **Animation Resources** definieren die standardisierte Verwaltung deklarativer Animationen innerhalb der NovaOS-Bootoberfläche. Sie beschreiben Timelines, Keyframes, Trigger und Interpolationen unabhängig von der Programmlogik und werden durch die Motion Engine ausgeführt. Durch zentrale Registrierung, Themeintegration, Wiederverwendung und statische Speicherverwaltung ermöglichen Animation Resources eine konsistente, erweiterbare und performante Animationsinfrastruktur für BIOS- und UEFI-basierte Bootumgebungen.