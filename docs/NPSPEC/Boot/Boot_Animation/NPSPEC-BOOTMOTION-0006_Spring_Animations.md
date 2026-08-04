# NPSPEC-BOOTMOTION-0006
# Spring Animations

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0006 |
| Titel | Spring Animations |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0005 |
| Zugehörige ADRs | ADR-BOOTMOTION-0004, ADR-BOOTMOTION-0005, ADR-BOOTMOTION-0006, ADR-BOOTMOTION-0008 |

---

# 1. Ziel

Diese Spezifikation definiert das **Spring Animation System** der NovaOS Boot UI.

Spring Animations simulieren eine physikalische Federbewegung und ermöglichen natürliche Übergänge mit sanftem Überschwingen und automatischer Beruhigung.

Das System wird eingesetzt für:

- Dialoge
- Panels
- Menüs
- Hovereffekte
- Fokusanimationen
- Buttons
- Glass Material
- Acrylic Material
- Cursorbewegungen
- Snap-Effekte

Im Gegensatz zu klassischen Easing-Funktionen basiert Spring Animation auf einem physikalischen Feder-Dämpfer-Modell mit Parametern wie Steifigkeit und Dämpfung. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle physikbasierten Animationen.

Sie umfasst:

- Position
- Skalierung
- Rotation
- Transparenz
- Blur
- Shadow
- Glow
- Materialparameter

Nicht Bestandteil sind:

- Timeline
- Scheduler
- Keyframe Animationen

---

# 3. Architektur

```text
Timeline

↓

Animation Scheduler

↓

Spring Solver

↓

Property Animator

↓

Compositor
```

---

# 4. Grundprinzip

Eine Spring Animation bewegt einen Wert nicht entlang einer festen Kurve.

Stattdessen nähert sich der Wert seiner Zielposition durch ein simuliertes Feder-Dämpfer-System.

---

# 5. Eigenschaften

Jede Spring Animation besitzt:

- Startwert
- Zielwert
- Geschwindigkeit
- Masse
- Federsteifigkeit
- Dämpfung
- Ruhetoleranz

---

# 6. Springbeschreibung

```c
typedef struct
{
    float mass;

    float stiffness;

    float damping;

    float velocity;

    float tolerance;

} nova_spring_t;
```

---

# 7. Physikalisches Modell

Eine Spring Animation besteht aus:

- Federkraft
- Dämpfung
- Geschwindigkeit
- Zielposition

Die Berechnung erfolgt kontinuierlich bis zum Erreichen des Gleichgewichts.

---

# 8. Parameter

Pflichtparameter:

| Parameter | Beschreibung |
|---|---|
| Mass | virtuelle Masse |
| Stiffness | Federsteifigkeit |
| Damping | Dämpfung |
| Velocity | Anfangsgeschwindigkeit |
| Target | Zielwert |

---

# 9. Standardwerte

Empfohlene Standardwerte:

| Parameter | Wert |
|---|---:|
| Mass | 1.0 |
| Stiffness | 350 |
| Damping | 30 |
| Velocity | 0 |
| Tolerance | 0.001 |

Diese Werte erzeugen eine ruhige Standardbewegung.

---

# 10. Dämpfung

Unterstützte Modi:

| Modus | Beschreibung |
|---|---|
| Undamped | dauerhafte Schwingung |
| Underdamped | leichtes Überschwingen |
| Critical | schnellste Beruhigung |
| Overdamped | kein Überschwingen |

Standard ist **Critical Damping**, da hier das Ziel ohne sichtbares Nachschwingen schnell erreicht wird. :contentReference[oaicite:1]{index=1}

---

# 11. Steifigkeit

Die Federsteifigkeit bestimmt:

- Beschleunigung
- Reaktionsgeschwindigkeit
- Bewegungscharakter

Größere Werte erzeugen schnellere Bewegungen.

---

# 12. Masse

Die virtuelle Masse beeinflusst:

- Trägheit
- Beschleunigung
- Bewegungsgefühl

Eine größere Masse erzeugt weichere Bewegungen.

---

# 13. Anfangsgeschwindigkeit

Spring Animationen dürfen eine Anfangsgeschwindigkeit besitzen.

Sie wird insbesondere verwendet für:

- Drag & Drop
- Cursorbewegungen
- Scrollbewegungen
- Snap-Effekte

---

# 14. Überschwingen

Überschwingen ist zulässig für:

- Buttons
- Menüs
- Panels
- Cursor

Nicht zulässig für:

- Sicherheitsdialoge
- Passwortdialoge
- Kritische Warnungen

---

# 15. Gleichgewicht

Eine Animation endet wenn:

- Zielposition erreicht
- Geschwindigkeit unter Toleranz
- Positionsabweichung unter Toleranz

---

# 16. Springgruppen

Mehrere Spring Animationen dürfen synchron laufen.

Jede Animation besitzt jedoch ihre eigenen Federparameter.

---

# 17. Materialanimationen

Spring Animationen dürfen verwendet werden für:

- Blur
- Glow
- Shadow
- Transparenz
- Glass Strength

---

# 18. Qualitätsprofile

Je nach Qualitätsprofil dürfen:

- Iterationen reduziert
- Spring Animationen vereinfacht
- Spring → Ease Out ersetzt

werden.

---

# 19. Reduced Motion

Bei aktiviertem Reduced Motion:

- Spring Animationen werden deaktiviert.
- Stattdessen werden Ease-Out-Animationen verwendet.
- Überschwingen ist nicht zulässig.

---

# 20. Berechnung

Die Berechnung erfolgt mit einer festen Integrationsschrittweite.

Variable Zeitschritte sind unzulässig.

Dadurch bleibt das Bewegungsverhalten auf sämtlichen Plattformen identisch.

---

# 21. Speicherverwaltung

Alle Spring-Instanzen stammen aus einem festen Objektpool.

Heap-Allokationen während der Animation sind verboten.

---

# 22. Performance

Anforderungen:

- deterministische Laufzeit
- begrenzte Iterationen
- SIMD-Unterstützung zulässig
- keine rekursiven Solver

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- negative Masse
- negative Dämpfung
- ungültige Federsteifigkeit
- numerische Instabilität
- NaN-Werte

Bei Fehlern wird automatisch auf **Ease Out** zurückgegriffen.

---

# 24. Sicherheit

Das System verhindert:

- unendliche Schwingungen
- instabile Federparameter
- Nullpointer
- numerische Überläufe
- Endlossimulationen

---

# 25. API

```c
nova_result_t
nova_spring_initialize(void);

nova_result_t
nova_spring_create(
    nova_spring_t* spring);

void
nova_spring_update(
    nova_spring_t* spring,
    float delta_time);

bool
nova_spring_is_at_rest(
    const nova_spring_t* spring);

void
nova_spring_destroy(
    nova_spring_t* spring);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- aktive Spring Animationen
- Federsteifigkeit
- Dämpfung
- Anfangsgeschwindigkeit
- Iterationen
- Berechnungszeit
- Gleichgewicht erreicht
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- Standard Spring
- Critical Damping
- Underdamped
- Overdamped
- Drag Animation
- Materialanimation
- Reduced Motion
- Qualitätswechsel
- Recovery
- numerische Grenzwerte

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Spring Animationen deterministisch berechnet werden.
- Critical Damping korrekt unterstützt wird.
- Anfangsgeschwindigkeiten verarbeitet werden.
- Reduced Motion Spring Animationen ersetzt.
- instabile Parameter erkannt werden.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Bewegungsabläufe erzeugen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTMOTION-0004 – Easing- und Spring-Funktionen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTMOTION-0008 – Unterstützung von „Reduced Motion“

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0004 – Animatable Properties
- NPSPEC-BOOTMOTION-0005 – Easing Functions

---

# 30. Zusammenfassung

Die **Spring Animations** erweitern das Motion System der NovaOS Boot UI um physikalisch basierte Bewegungsabläufe. Durch Federsteifigkeit, Dämpfung, Masse und Anfangsgeschwindigkeit entstehen natürliche Übergänge mit kontrolliertem Überschwingen und sanfter Beruhigung. Feste Integrationsschritte, deterministische Berechnung sowie adaptive Qualitätsprofile gewährleisten ein identisches Verhalten auf BIOS- und UEFI-Systemen bei gleichzeitig hoher Performance und Stabilität.