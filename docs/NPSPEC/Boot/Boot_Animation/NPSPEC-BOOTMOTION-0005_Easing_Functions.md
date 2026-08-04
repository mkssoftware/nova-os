# NPSPEC-BOOTMOTION-0005
# Easing Functions

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0005 |
| Titel | Easing Functions |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0004 |
| Zugehörige ADRs | ADR-BOOTMOTION-0004, ADR-BOOTMOTION-0005, ADR-BOOTMOTION-0006, ADR-BOOTDESIGN-0001 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Easing Functions** der NovaOS Boot UI.

Easing Functions bestimmen die Geschwindigkeit einer Animation über deren gesamte Laufzeit und sorgen für natürliche Bewegungsabläufe.

Sie bilden die Grundlage für:

- Dialoganimationen
- Menüanimationen
- Buttonanimationen
- Materialanimationen
- Glass-Animationen
- Hovereffekte
- Fokusanimationen
- Übergänge

Alle Animationen des Bootmanagers verwenden ausschließlich die hier definierten Easing Functions.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche zeitbasierten Animationen.

Sie umfasst:

- Controls
- Layer
- Panels
- Dialoge
- Menüs
- Glass Material
- Acrylic Material
- Cursor
- Overlays

Nicht Bestandteil sind:

- Audio
- Physiksimulationen
- Partikelsysteme

---

# 3. Architektur

```text
Timeline

↓

Animation Scheduler

↓

Easing Function

↓

Interpolator

↓

Property Animator

↓

Compositor
```

---

# 4. Grundprinzip

Eine Easing Function erhält:

```text
Progress

0.0

↓

1.0
```

und liefert:

```text
Interpolated Progress

0.0

↓

1.0
```

Sie verändert ausschließlich den zeitlichen Verlauf einer Animation.

---

# 5. Anforderungen

Alle Easing Functions müssen:

- deterministisch
- stetig
- numerisch stabil
- reproduzierbar

sein.

---

# 6. Standard Easing

Die Standardfunktion lautet:

```text
Ease Out Cubic
```

Sie wird verwendet, wenn keine andere Funktion angegeben wurde.

---

# 7. Unterstützte Funktionen

Pflicht:

- Linear
- Ease In
- Ease Out
- Ease In Out
- Cubic
- Quartic
- Quintic
- Sine
- Circular
- Exponential
- Back
- Bounce
- Elastic
- Spring
- Step

Weitere Funktionen können ergänzt werden.

---

# 8. Linear

Eigenschaften:

- konstante Geschwindigkeit
- keine Beschleunigung
- keine Verzögerung

Einsatz:

- Fortschrittsanzeigen
- Timer
- Diagnosen

---

# 9. Ease In

Eigenschaften:

- langsamer Start
- zunehmende Geschwindigkeit

Verwendung:

- Einblenden
- Dialogöffnung
- Zoom

---

# 10. Ease Out

Eigenschaften:

- schneller Start
- sanftes Ende

Verwendung:

- Dialogschluss
- Hover
- Fokus

---

# 11. Ease In Out

Eigenschaften:

- langsamer Start
- gleichmäßige Mitte
- langsames Ende

Standard für größere UI-Animationen.

---

# 12. Cubic

Eigenschaften:

- natürliche Beschleunigung
- ruhiger Bewegungsablauf

Verwendung:

- Panels
- Menüs
- Dialoge

---

# 13. Quartic und Quintic

Eigenschaften:

- stärkere Beschleunigung
- weichere Endphase

Verwendung:

- große Übergänge
- Vollbildanimationen

---

# 14. Sine

Eigenschaften:

- harmonischer Verlauf
- gleichmäßige Bewegung

Verwendung:

- Materialanimationen
- Farbverläufe
- Transparenz

---

# 15. Circular

Eigenschaften:

- kreisförmige Beschleunigung
- sanfte Übergänge

Geeignet für:

- Zoom
- Skalierung

---

# 16. Exponential

Eigenschaften:

- sehr starke Beschleunigung

Nur für kurze Animationen geeignet.

---

# 17. Back

Eigenschaften:

- leichtes Überschwingen
- anschließende Rückbewegung

Geeignet für:

- Buttons
- Dialoge
- Panels

---

# 18. Bounce

Eigenschaften:

- simuliert Abprallen

Verwendung ausschließlich für dekorative Animationen.

Nicht zulässig für sicherheitskritische Dialoge.

---

# 19. Elastic

Eigenschaften:

- federartige Bewegung
- mehrere Schwingungen

Nur für optionale UI-Elemente zulässig.

---

# 20. Spring

Spring basiert auf einem physikalischen Feder-Dämpfer-Modell.

Parameter:

- Masse
- Federkonstante
- Dämpfung

Die Animation endet automatisch nach Erreichen eines stabilen Zustands.

---

# 21. Step

Step besitzt keine Interpolation.

Der Übergang erfolgt sprunghaft.

Geeignet für:

- Zustandswechsel
- Sichtbarkeit
- Z-Index

---

# 22. Materialanimationen

Empfohlene Easing Functions:

| Property | Funktion |
|---|---|
| Blur | Ease Out |
| Tint | Sine |
| Transparenz | Ease Out |
| Glow | Cubic |
| Shadow | Ease Out |

---

# 23. Qualitätsprofile

Bei reduzierter Qualität dürfen ersetzt werden:

- Elastic → Ease Out
- Bounce → Ease Out
- Spring → Cubic

Dadurch sinkt der Rechenaufwand.

---

# 24. Reduced Motion

Bei aktiviertem Reduced Motion:

- Bounce deaktivieren
- Elastic deaktivieren
- Spring deaktivieren

Erlaubt bleiben:

- Linear
- Ease Out
- Sine

---

# 25. Speicherverwaltung

Alle Easing Functions sind statisch implementiert.

Dynamische Erzeugung neuer Funktionen ist nicht zulässig.

---

# 26. Performance

Anforderungen:

- O(1)-Berechnung
- keine Iterationen
- keine Heap-Allokationen
- numerisch stabil

Spring bildet die einzige Ausnahme und verwendet eine begrenzte iterative Berechnung.

---

# 27. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Funktion
- ungültige Parameter
- numerischer Überlauf
- NaN-Ergebnis
- unendlicher Wert

Im Fehlerfall wird automatisch auf **Linear** zurückgegriffen.

---

# 28. Sicherheit

Das System verhindert:

- rekursive Easing-Aufrufe
- ungültige Rückgabewerte
- instabile Berechnungen
- Division durch Null
- Parameter außerhalb definierter Bereiche

---

# 29. API

```c
typedef float
(*nova_easing_function_t)(
    float progress);

float
nova_ease_linear(
    float t);

float
nova_ease_out(
    float t);

float
nova_ease_in_out(
    float t);

float
nova_ease_spring(
    float t,
    const nova_spring_parameters_t* params);
```

---

# 30. Diagnosedaten

Zu erfassen sind:

- verwendete Easing Functions
- Springparameter
- Berechnungszeit
- Anzahl Animationen
- Fehleranzahl
- Ersatzfunktionen
- Qualitätsprofil

---

# 31. Testfälle

Zu testen sind:

- Linear
- Ease In
- Ease Out
- Cubic
- Sine
- Spring
- Bounce
- Elastic
- Reduced Motion
- Qualitätswechsel

---

# 32. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Easing Functions deterministisch arbeiten.
- Standardanimationen Ease Out Cubic verwenden.
- Materialanimationen empfohlene Funktionen nutzen.
- Reduced Motion korrekt unterstützt wird.
- ungültige Funktionen automatisch ersetzt werden.
- keine Heap-Allokationen während der Berechnung stattfinden.
- BIOS- und UEFI-Systeme identische Bewegungsabläufe erzeugen.

---

# 33. Referenzen

## ADRs

- ADR-BOOTMOTION-0004 – Easing- und Spring-Funktionen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTDESIGN-0001 – Ableitung vom Nova Design Language

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0004 – Animatable Properties

---

# 34. Zusammenfassung

Die **Easing Functions** definieren den zeitlichen Verlauf sämtlicher Animationen der NovaOS Bootoberfläche. Durch einen standardisierten Satz deterministischer Interpolationsfunktionen – von Linear über Ease-Out bis hin zu Spring – entstehen natürliche und konsistente Bewegungen. Adaptive Qualitätsprofile und die Unterstützung von „Reduced Motion“ gewährleisten dabei sowohl eine hochwertige Benutzererfahrung als auch eine zuverlässige Ausführung auf BIOS- und UEFI-Systemen.