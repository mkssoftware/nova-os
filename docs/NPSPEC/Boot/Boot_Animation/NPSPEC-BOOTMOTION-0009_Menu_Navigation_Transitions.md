# NPSPEC-BOOTMOTION-0009
# Menu Navigation Transitions

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0009 |
| Titel | Menu Navigation Transitions |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0008, NPSPEC-BOOTNAV-0001 bis NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTMOTION-0006, ADR-BOOTNAV-0001, ADR-BOOTNAV-0002, ADR-BOOTNAV-0003, ADR-BOOTNAV-0004 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Navigationstransitionen** zwischen Menüs der NovaOS Boot UI.

Sie beschreibt sämtliche Bewegungsabläufe beim Wechsel zwischen:

- Hauptmenü
- Untermenüs
- Recovery-Menü
- Einstellungen
- Diagnose
- Bootoptionen
- Bestätigungsdialogen

Das Ziel besteht darin, die Navigationshierarchie jederzeit eindeutig erkennbar zu machen und dem Benutzer eine klare räumliche Orientierung zu geben.

Alle Navigationen werden durch das zentrale Transition System ausgeführt. Moderne UI-Richtlinien empfehlen konsistente Übergänge zwischen Navigationszuständen, um Orientierung und Verständlichkeit zu verbessern. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Menüwechsel.

Sie umfasst:

- Hauptmenü
- Untermenüs
- Recovery
- Einstellungen
- Diagnose
- Passwortdialoge
- Bootaktionen

Nicht Bestandteil sind:

- Dialoganimationen
- Splash Screen
- Bootlogo

---

# 3. Architektur

```text
Benutzereingabe

↓

Navigation Controller

↓

Transition Manager

↓

Animation Scheduler

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Ein Menüwechsel besteht niemals aus einem abrupten Austausch des Inhalts.

Jede Navigation besitzt:

- Exit Motion
- Enter Motion

Beide Bewegungen laufen synchronisiert.

---

# 5. Navigationsarten

Unterstützt werden:

- Forward Navigation
- Back Navigation
- Root Navigation
- Modal Navigation
- Replace Navigation
- Recovery Navigation

---

# 6. Navigation Forward

Beim Wechsel in ein Untermenü:

```text
Menü A

↓

Slide nach links

↓

Menü B

↓

Slide von rechts
```

Dadurch wird die Hierarchie eindeutig dargestellt.

---

# 7. Navigation Back

Beim Zurückkehren:

```text
Menü B

↓

Slide nach rechts

↓

Menü A

↓

Slide von links
```

Vorwärts- und Rückwärtsnavigation sind spiegelbildlich aufgebaut.

---

# 8. Root Navigation

Ein Wechsel direkt zum Hauptmenü verwendet:

- Cross Fade
- leichter Scale-Effekt

Seitliche Bewegungen werden vermieden.

---

# 9. Replace Navigation

Bei einem vollständigen Austausch eines Menüs:

- Fade Out
- Cross Fade
- Fade In

Der Benutzer erkennt einen Zustandswechsel ohne Richtungsinformation.

---

# 10. Recovery Navigation

Recovery-Menüs verwenden:

- langsames Fade
- dezente Skalierung

Seitliche Bewegungen sind nicht zulässig.

---

# 11. Übergangsablauf

```text
Navigation anfordern

↓

Eingabe sperren

↓

Exit Motion

↓

Menü wechseln

↓

Enter Motion

↓

Fokus setzen

↓

Eingabe freigeben
```

---

# 12. Standardanimationen

Unterstützt werden:

- Slide
- Fade
- Scale
- Cross Fade
- Material Transition

Alle anderen Animationen sind optional.

---

# 13. Richtungen

Erlaubte Richtungen:

- Links
- Rechts
- Oben
- Unten

Standard:

- Vorwärts → Links
- Zurück → Rechts

---

# 14. Bewegungswerte

Empfohlene Werte:

| Eigenschaft | Start | Ende |
|---|---:|---:|
| X-Offset | ±32 DLU | 0 DLU |
| Deckkraft | 0 % | 100 % |
| Skalierung | 98 % | 100 % |

---

# 15. Materialanimation

Während eines Menüwechsels dürfen animiert werden:

- Blur
- Tint
- Transparenz
- Shadow
- Glow

Alle Materialparameter laufen synchron.

---

# 16. Hintergrund

Der Hintergrund bleibt während der Navigation stabil.

Nur aktive Menüflächen werden animiert.

Dadurch bleibt die räumliche Orientierung erhalten.

---

# 17. Fokus

Nach Abschluss der Enter Motion wird automatisch:

- das erste fokussierbare Element,
- oder das zuletzt aktive Element

aktiviert.

---

# 18. Unterbrechung

Eine laufende Navigation darf:

- abgebrochen
- ersetzt
- umgekehrt

werden.

Der Scheduler übernimmt den aktuellen Animationszustand.

---

# 19. Mehrere Navigationen

Während einer laufenden Navigation wird keine weitere Navigation gestartet.

Neue Anforderungen werden in eine Warteschlange aufgenommen.

---

# 20. Qualitätsprofile

Je nach Qualitätsprofil dürfen reduziert werden:

- Blur
- Shadow
- Glow
- Materialanimation

Slide und Fade bleiben erhalten.

---

# 21. Reduced Motion

Bei aktiviertem Reduced Motion:

- Slide wird deaktiviert.
- Fade bleibt erhalten.
- Materialanimationen werden reduziert.

Seitliche Bewegungen entfallen vollständig. :contentReference[oaicite:1]{index=1}

---

# 22. Speicherverwaltung

Alle Navigationstransitionen verwenden:

- feste Transitionobjekte
- festen Animationspool

Heap-Allokationen während der Laufzeit sind verboten.

---

# 23. Performance

Anforderungen:

- deterministische Laufzeit
- synchronisierte Animationen
- O(n)-Scheduler
- keine Heap-Allokationen

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiges Zielmenü
- fehlende Transition
- ungültiger Navigationszustand
- beschädigte Menüstruktur
- doppelte Navigation

Im Fehlerfall erfolgt sofortiger Zustandswechsel ohne Animation.

---

# 25. Sicherheit

Das System verhindert:

- doppelte Navigationen
- rekursive Menüwechsel
- Endlosschleifen
- Nullpointer
- ungültige Fokuszustände

---

# 26. API

```c
nova_result_t
nova_navigation_transition_begin(
    nova_menu_t* source,
    nova_menu_t* target);

void
nova_navigation_transition_update(
    uint64_t current_time);

bool
nova_navigation_transition_running(void);

void
nova_navigation_transition_cancel(void);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- Anzahl Navigationen
- aktive Transitionen
- durchschnittliche Dauer
- Richtungswechsel
- Materialanimationen
- Fokuswechsel
- Schedulerlaufzeit
- Fehleranzahl

---

# 28. Testfälle

Zu testen sind:

- Vorwärtsnavigation
- Rückwärtsnavigation
- Root Navigation
- Replace Navigation
- Recovery Navigation
- Reduced Motion
- Qualitätswechsel
- Navigationsabbruch
- Fokuswechsel
- Recovery Mode

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Menüwechsel über das Transition System erfolgen.
- Vorwärts- und Rückwärtsnavigation spiegelbildlich animiert werden.
- Materialanimationen synchron ausgeführt werden.
- Focus Management korrekt funktioniert.
- Reduced Motion vollständig unterstützt wird.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Navigationsverhalten zeigen.

---

# 30. Referenzen

## ADRs

- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTNAV-0001 – Zustandsbasierte Navigation
- ADR-BOOTNAV-0002 – Hierarchischer Navigations-Stack
- ADR-BOOTNAV-0003 – Einheitliches Zurück-Verhalten
- ADR-BOOTNAV-0004 – Trennung von Navigation und Bootaktionen

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTNAV-0001 – Navigation State Model
- NPSPEC-BOOTNAV-0002 – Navigation Stack

---

# 31. Zusammenfassung

Die **Menu Navigation Transitions** definieren sämtliche Übergänge zwischen Menüs der NovaOS Bootoberfläche. Durch konsistente Slide-, Fade- und Materialanimationen wird die Navigationshierarchie klar visualisiert und die Orientierung des Benutzers verbessert. Ein zentrales Transition System, deterministische Abläufe, feste Ressourcenverwaltung sowie Unterstützung für Reduced Motion gewährleisten eine flüssige und einheitliche Navigation auf BIOS- und UEFI-Systemen. :contentReference[oaicite:2]{index=2}
