# NPSPEC-BOOTMOTION-0013
# Reduced Motion Mode

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0013 |
| Titel | Reduced Motion Mode |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0012, NPSPEC-ACCESSIBILITY-0001 |
| Zugehörige ADRs | ADR-BOOTACCESS-0002, ADR-BOOTACCESS-0003, ADR-BOOTMOTION-0004, ADR-BOOTMOTION-0008, ADR-BOOTDESIGN-0004 |

---

# 1. Ziel

Diese Spezifikation definiert den **Reduced Motion Mode** der NovaOS Boot UI.

Der Modus reduziert oder ersetzt Animationen, welche für Benutzer mit Bewegungs- oder Gleichgewichtsempfindlichkeit belastend sein können.

Der Reduced Motion Mode verfolgt dabei folgende Ziele:

- Reduzierung großflächiger Bewegungen
- Vermeidung von Zoom-Animationen
- Vermeidung starker Skalierungen
- Vermeidung von Parallax-Effekten
- Erhalt der räumlichen Orientierung
- Beibehaltung aller Informationen
- vollständige Bedienbarkeit

Animationen werden nicht vollständig entfernt, sondern – sofern sie Informationen vermitteln – durch ruhigere Alternativen wie Fade- oder Farbübergänge ersetzt. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Animationen der Bootoberfläche.

Sie umfasst:

- Dialoge
- Menüs
- Navigation
- Fokus
- Controls
- Progress Animationen
- Materialanimationen
- Overlayanimationen
- Glass Material
- Acrylic Material

Nicht Bestandteil sind:

- Hardwareinitialisierung
- Kernelstart
- Bootlogo

---

# 3. Architektur

```text
Accessibility Settings

↓

Reduced Motion Manager

↓

Motion Policy

↓

Animation Scheduler

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Der Reduced Motion Mode verändert ausschließlich die Darstellung.

Nicht verändert werden:

- Navigation
- Reihenfolge
- Bedienlogik
- Fokus
- Funktionalität

Jede Aktion muss weiterhin eindeutig erkennbar bleiben.

---

# 5. Aktivierung

Der Modus kann aktiviert werden durch:

- Systemeinstellung
- Bootkonfiguration
- Accessibility-Menü
- API

Der Zustand gilt global für die gesamte Boot UI.

---

# 6. Motion Policy

Der Motion Manager entscheidet für jede Animation:

```text
Animation

↓

Erlaubt

oder

Ersetzt

oder

Deaktiviert
```

---

# 7. Erlaubte Animationen

Auch im Reduced Motion Mode bleiben erhalten:

- Fade
- Farbwechsel
- Transparenz
- Highlight
- Fokusindikatoren

Diese Animationen vermitteln wichtige Zustandsinformationen.

---

# 8. Ersetzte Animationen

Folgende Animationen werden ersetzt:

| Standard | Reduced Motion |
|---|---|
| Slide | Fade |
| Zoom | Fade |
| Spring | Ease Out |
| Bounce | Fade |
| Elastic | Ease Out |
| Scale | Opacity |

---

# 9. Deaktivierte Animationen

Nicht zulässig sind:

- Parallax
- starke Zoombewegungen
- mehrfache Rotation
- kontinuierliche Schwingungen
- dekorative Bewegungen
- räumliche Tiefeneffekte

Diese Bewegungen gelten als mögliche Auslöser für Bewegungsunwohlsein. :contentReference[oaicite:1]{index=1}

---

# 10. Dialoge

Beim Öffnen eines Dialogs:

Standard:

```text
Fade

+

Scale

+

Blur
```

Reduced Motion:

```text
Fade
```

---

# 11. Navigation

Standard:

```text
Slide
```

Reduced Motion:

```text
Cross Fade
```

Die Navigationshierarchie bleibt unverändert erkennbar.

---

# 12. Fokus

Focus Motion verwendet:

- Fade
- Highlight
- Focus Ring

Nicht verwendet werden:

- Spring
- Scale

---

# 13. Progress Animationen

Bestimmte Progressanzeigen bleiben unverändert.

Indeterminierte Animationen:

- langsamer
- ruhiger
- geringere Bewegungsamplitude

---

# 14. Materialanimationen

Materialeffekte werden reduziert.

Betroffen sind:

- Blur
- Glow
- Reflection
- Shadow

Transparenz bleibt erhalten.

---

# 15. Glass Material

Glass bleibt verfügbar.

Reduziert werden:

- Blur Radius
- Lichtreflexe
- Materialbewegungen

---

# 16. Animationstempo

Empfohlene Anpassungen:

| Animation | Standard | Reduced |
|---|---:|---:|
| Fade | 180 ms | 150 ms |
| Highlight | 120 ms | 100 ms |
| Fokus | 120 ms | 100 ms |

Kurze, ruhige Animationen werden bevorzugt.

---

# 17. Qualitätsprofile

Reduced Motion ist unabhängig von den Rendering Quality Profiles.

Beide Systeme können gleichzeitig aktiv sein.

---

# 18. Unterbrechbare Animationen

Alle verbleibenden Animationen bleiben vollständig:

- unterbrechbar
- pausierbar
- umkehrbar

---

# 19. Eingaben

Benutzereingaben werden niemals verzögert.

Animationen dürfen die Reaktionszeit nicht beeinflussen.

---

# 20. API

Der aktuelle Status ist jederzeit abfragbar.

Änderungen wirken sofort auf neu gestartete Animationen.

Bereits laufende Animationen werden kontrolliert auf die neue Bewegungsrichtlinie umgestellt.

---

# 21. Speicherverwaltung

Der Reduced Motion Mode verwendet:

- keine zusätzlichen Animationen
- keine zusätzlichen Objektpools
- keine zusätzlichen Heap-Allokationen

Alle Umschaltungen erfolgen über feste Konfigurationsstrukturen.

---

# 22. Performance

Der Reduced Motion Mode darf:

- CPU-Auslastung senken
- GPU-Auslastung senken
- Speicherverbrauch reduzieren

Er darf niemals zu höherem Ressourcenverbrauch führen.

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Motion Policy
- fehlende Ersatzanimation
- beschädigte Accessibility-Konfiguration
- ungültiger Animationsmodus

Im Fehlerfall wird automatisch der Standardmodus verwendet.

---

# 24. Sicherheit

Das System verhindert:

- widersprüchliche Motion Policies
- doppelte Animationen
- rekursive Umschaltungen
- Nullpointer
- ungültige Zustandswechsel

---

# 25. API

```c
nova_result_t
nova_motion_set_reduced(
    bool enabled);

bool
nova_motion_is_reduced(void);

void
nova_motion_apply_policy(
    nova_animation_t* animation);

void
nova_motion_reload_policy(void);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- Reduced Motion Status
- ersetzte Animationen
- deaktivierte Animationen
- aktive Policies
- Performancegewinn
- Schedulerlaufzeit
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- Aktivierung
- Deaktivierung
- Dialoganimationen
- Navigation
- Fokus
- Progress Animationen
- Materialanimationen
- Qualitätsprofile
- Laufende Animationen
- Recovery Mode

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle problematischen Bewegungen reduziert oder ersetzt werden.
- Fade-Animationen erhalten bleiben.
- Navigation weiterhin eindeutig nachvollziehbar bleibt.
- sämtliche Accessibility-Funktionen vollständig nutzbar bleiben.
- der Modus ohne Neustart aktiviert und deaktiviert werden kann.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTACCESS-0002 – Hochkontrastmodus
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen
- ADR-BOOTMOTION-0004 – Easing- und Spring-Funktionen
- ADR-BOOTMOTION-0008 – Unterstützung von „Reduced Motion“
- ADR-BOOTDESIGN-0004 – Kontrastabhängige Oberflächen

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0005 – Easing Functions
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0010 – Focus and Selection Motion
- NPSPEC-BOOTMOTION-0011 – Progress Animations
- NPSPEC-BOOTMOTION-0012 – Interruptible Animations
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 30. Zusammenfassung

Der **Reduced Motion Mode** definiert eine barrierefreie Bewegungsrichtlinie für die NovaOS Bootoberfläche. Bewegungsintensive Animationen werden durch ruhigere Alternativen ersetzt oder deaktiviert, ohne die Bedienbarkeit oder Informationsvermittlung einzuschränken. Durch globale Motion Policies, deterministische Umschaltung und vollständige Integration in das Motion System entsteht eine konsistente, performante und zugängliche Benutzeroberfläche auf BIOS- und UEFI-Systemen. Empfehlungen aktueller Accessibility-Richtlinien sehen ebenfalls vor, großflächige Bewegungen durch sanfte Überblendungen oder statische Zustandswechsel zu ersetzen und nur essenzielle Animationen beizubehalten. :contentReference[oaicite:2]{index=2}