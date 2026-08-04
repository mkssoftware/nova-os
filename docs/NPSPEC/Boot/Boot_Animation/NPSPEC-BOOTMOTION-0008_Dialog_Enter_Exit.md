# NPSPEC-BOOTMOTION-0008
# Dialog Enter and Exit Motion

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0008 |
| Titel | Dialog Enter and Exit Motion |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0007, NPSPEC-BOOTCOMPOSITOR-0004 |
| Zugehörige ADRs | ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0005, ADR-BOOTMOTION-0006, ADR-BOOTDIALOG-0001, ADR-BOOTDIALOG-0002 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Enter- und Exit-Animationen** modaler Dialoge der NovaOS Boot UI.

Sie beschreibt:

- Öffnen eines Dialogs
- Schließen eines Dialogs
- Hintergrundanimation
- Materialanimation
- Fokuswechsel
- Animationen verschachtelter Dialoge
- Synchronisation aller beteiligten Ebenen

Das Ziel besteht darin, den Benutzer jederzeit eindeutig erkennen zu lassen:

- welcher Dialog aktiv ist,
- woher er erscheint,
- wohin er verschwindet,
- welcher Bereich momentan bedienbar ist.

Gezielte Enter- und Exit-Transitionen helfen dabei, Zustandswechsel verständlich und räumlich nachvollziehbar darzustellen. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- modale Dialoge
- Warnungen
- Bestätigungsdialoge
- Recovery-Dialoge
- Passwortdialoge
- Einstellungsdialoge
- Unterdialoge

Nicht Bestandteil sind:

- Seitenwechsel
- Menünavigation
- Bootlogo

---

# 3. Architektur

```text
UI Event

↓

Dialog Manager

↓

Transition System

↓

Animation Scheduler

↓

Dialog Layer

↓

Overlay Layer

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Dialoge erscheinen niemals abrupt.

Jeder Dialog besitzt:

- Enter Motion
- Exit Motion

Beide Bewegungen müssen zueinander spiegelbildlich sein.

---

# 5. Ablauf Enter Motion

```text
Dialog erzeugen

↓

Overlay Fade In

↓

Background Blur

↓

Dialog Scale

↓

Dialog Fade

↓

Fokus setzen

↓

Interaktion freigeben
```

---

# 6. Ablauf Exit Motion

```text
Interaktion sperren

↓

Dialog Fade Out

↓

Scale Out

↓

Overlay Fade Out

↓

Blur entfernen

↓

Dialog zerstören

↓

Fokus zurückgeben
```

---

# 7. Enter Animation

Standardwerte:

| Eigenschaft | Start | Ende |
|---|---:|---:|
| Deckkraft | 0 % | 100 % |
| Skalierung | 95 % | 100 % |
| Blur | 0 % | 100 % |

---

# 8. Exit Animation

Standardwerte:

| Eigenschaft | Start | Ende |
|---|---:|---:|
| Deckkraft | 100 % | 0 % |
| Skalierung | 100 % | 95 % |
| Blur | 100 % | 0 % |

---

# 9. Overlay

Während Enter Motion:

- Overlay einblenden
- Hintergrund abdunkeln
- Eingaben blockieren

Während Exit Motion erfolgt der umgekehrte Ablauf.

---

# 10. Background Blur

Glass-Dialoge animieren zusätzlich:

- Blur Radius
- Tint
- Transparenz

Die Blur-Animation beginnt vor der Dialoganimation.

---

# 11. Materialanimation

Animiert werden:

- Blur
- Tint
- Shadow
- Glow
- Border
- Highlight

Alle Materialparameter laufen synchron.

---

# 12. Fokus

Der Tastaturfokus wird erst gesetzt nachdem:

- Enter Motion abgeschlossen ist.

Beim Schließen wird der Fokus an das zuvor aktive Element zurückgegeben.

---

# 13. Eingaben

Während Enter und Exit Motion:

- Maus
- Tastatur
- Touch

dürfen ausschließlich den aktiven Dialog beeinflussen.

---

# 14. Verschachtelte Dialoge

Unterdialoge erscheinen:

- oberhalb des Elterndialogs
- mit eigener Enter Motion

Der Elterndialog bleibt sichtbar.

---

# 15. Mehrere Dialoge

Dialoge werden niemals gleichzeitig geöffnet.

Neue Dialoge warten bis:

- Enter Motion abgeschlossen ist.

---

# 16. Animationen

Standardanimationen:

| Animation | Dauer |
|---|---:|
| Overlay Fade | 120 ms |
| Dialog Fade | 180 ms |
| Scale | 180 ms |
| Blur | 220 ms |

Alle Werte stammen aus den Motion Tokens.

---

# 17. Easing

Standard:

- Ease Out Cubic

Materialanimationen:

- Sine

Scale:

- Spring (Critical)

---

# 18. Unterbrechung

Eine laufende Dialoganimation darf:

- gestoppt
- umgekehrt
- ersetzt

werden.

Der Scheduler übernimmt den aktuellen Animationszustand.

---

# 19. Navigation

Beim Dialogwechsel:

```text
Dialog A

↓

Exit Motion

↓

Dialog B

↓

Enter Motion
```

Beide Dialoge dürfen sich nicht überlagern.

---

# 20. Reduced Motion

Bei aktiviertem Reduced Motion:

- Scale deaktivieren
- Spring deaktivieren
- Fade beibehalten
- Blur sofort setzen

Größere Bewegungen entfallen. :contentReference[oaicite:1]{index=1}

---

# 21. Qualitätsprofile

Je nach Profil dürfen reduziert werden:

- Blur
- Glow
- Shadow
- Spring

Fade bleibt immer erhalten.

---

# 22. Speicherverwaltung

Alle Dialoganimationen verwenden:

- festen Animationspool
- feste Materialobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 23. Performance

Anforderungen:

- deterministische Laufzeit
- synchronisierte Materialanimation
- keine rekursiven Dialoganimationen
- keine Heap-Allokationen

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- doppelte Dialogöffnung
- ungültiger Fokus
- beschädigte Animation
- ungültige Materialparameter
- fehlender Dialog

Im Fehlerfall erfolgt der Zustandswechsel sofort ohne Animation.

---

# 25. Sicherheit

Das System verhindert:

- doppelte Dialoge
- Endlosschleifen
- rekursive Öffnungen
- ungültige Fokuszustände
- Nullpointer

---

# 26. API

```c
nova_result_t
nova_dialog_enter(
    nova_dialog_t* dialog);

nova_result_t
nova_dialog_exit(
    nova_dialog_t* dialog);

bool
nova_dialog_is_animating(
    const nova_dialog_t* dialog);

void
nova_dialog_cancel_animation(
    nova_dialog_t* dialog);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- Enter Motion Dauer
- Exit Motion Dauer
- aktive Dialoganimationen
- Materialanimationen
- Blurzeit
- Schedulerzeit
- Fokuswechsel
- Fehleranzahl

---

# 28. Testfälle

Zu testen sind:

- Dialog öffnen
- Dialog schließen
- Recovery Dialog
- Passwortdialog
- verschachtelte Dialoge
- Reduced Motion
- Qualitätswechsel
- Animation abbrechen
- Fokuswechsel
- Recovery Mode

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Dialoge Enter- und Exit-Motion verwenden.
- Overlay und Blur synchron animiert werden.
- Fokus erst nach Abschluss der Enter Motion gesetzt wird.
- Exit Motion den Fokus korrekt zurückgibt.
- Reduced Motion vollständig unterstützt wird.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Dialoganimationen erzeugen.

---

# 30. Referenzen

## ADRs

- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTDIALOG-0001 – Modale Overlay-Dialoge für Untermenüs
- ADR-BOOTDIALOG-0002 – Dialoge als eigene Compositor-Ebene

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0005 – Easing Functions
- NPSPEC-BOOTMOTION-0006 – Spring Animations
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTCOMPOSITOR-0004 – Modal Layer
- NPSPEC-BOOTCOMPOSITOR-0005 – Backdrop Processing
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material

---

# 31. Zusammenfassung

Das **Dialog Enter and Exit Motion System** definiert sämtliche Bewegungsabläufe beim Öffnen und Schließen modaler Dialoge der NovaOS Bootoberfläche. Durch synchronisierte Fade-, Scale-, Blur- und Materialanimationen sowie eine kontrollierte Fokusverwaltung entstehen flüssige, verständliche und konsistente Zustandswechsel. Die deterministische Ausführung, adaptive Qualitätsprofile und vollständige Unterstützung von Reduced Motion gewährleisten identisches Verhalten auf BIOS- und UEFI-Systemen. :contentReference[oaicite:2]{index=2}