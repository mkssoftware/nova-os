# NPSPEC-BOOTINPUT-0011
# Input Repeat

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0011 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTINPUT-0003, NPSPEC-BOOTINPUT-0004, NPSPEC-BOOTINPUT-0005, NPSPEC-BOOTINPUT-0008, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Input Repeat System** der NovaOS-Bootoberfläche.

Das Input Repeat System steuert die automatische Wiederholung kontinuierlicher Benutzereingaben, solange eine Eingabe aktiv gehalten wird. Es stellt sicher, dass sämtliche unterstützten Eingabegeräte identische Wiederholungsmechanismen verwenden und die Wiederholungsrate unabhängig von Hardware und Firmware deterministisch bleibt.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Tastatur
- Controller
- D-Pad
- Analogstick (digitale Navigation)
- optionale Pointer-Eingaben
- Accessibility-Eingaben

---

# 3. Ziele

Das Input Repeat System muss:

- hardwareunabhängig arbeiten
- deterministische Wiederholungen erzeugen
- zentral konfigurierbar sein
- Accessibility unterstützen
- mehrere gleichzeitige Wiederholungen verwalten
- unabhängig von BIOS und UEFI funktionieren

---

# 4. Architektur

```text
Input Device

↓

Input HAL

↓

Input Event

↓

Repeat Manager

↓

Repeat Timer

↓

Unified Input Event

↓

Dispatcher
```

---

# 5. Komponenten

Das Repeat-System besteht aus:

- Repeat Manager
- Repeat Timer
- Active Repeat Table
- Input Validator
- Event Generator
- Accessibility Adapter

---

# 6. Grundprinzip

Eine Eingabe kann genau einen Repeat-Vorgang erzeugen.

Der Ablauf:

```text
Key Down

↓

Initial Delay

↓

Repeat

↓

Repeat

↓

Repeat

↓

Key Up

↓

Stop
```

---

# 7. Unterstützte Eingaben

Repeat wird unterstützt für:

- Zeichen
- Navigation
- Listen
- Slider
- Scrollbars
- Controller Navigation

Nicht unterstützt:

- Einmalaktionen
- Bestätigungsdialoge
- Kritische Aktionen

---

# 8. Repeat-Zustände

```text
Idle

Waiting

Repeating

Paused

Stopped
```

Nur aktive Eingaben dürfen sich im Zustand **Repeating** befinden.

---

# 9. Initial Delay

Vor der ersten Wiederholung wird eine Wartezeit eingehalten.

Standard:

```text
Initial Delay

≈ 400–600 ms
```

Die konkrete Zeit wird zentral konfiguriert.

---

# 10. Wiederholungsrate

Nach Ablauf des Initial Delay erfolgt die Wiederholung mit einer festen Frequenz.

Beispiel:

```text
20 Wiederholungen/Sekunde
```

Die Frequenz ist systemweit einheitlich.

---

# 11. Gleichzeitige Wiederholungen

Mehrere Eingaben dürfen gleichzeitig aktiv sein.

Beispiel:

```text
Controller

↓

Navigation
```

```text
Keyboard

↓

Texteingabe
```

Jede Eingabe besitzt ihren eigenen Repeat-Kontext.

---

# 12. Navigation

Bei Fokusnavigation werden ausschließlich gültige Navigationsereignisse wiederholt.

Die Navigation endet automatisch:

- am Listenende
- bei deaktivierten Controls
- bei Fokusverlust

---

# 13. Accessibility

Unterstützt werden:

- Slow Keys
- Repeat Keys
- Sticky Keys
- konfigurierbare Wiederholungsrate

Accessibility kann:

- Repeat deaktivieren
- Delay verlängern
- Frequenz reduzieren

---

# 14. Fokusintegration

Repeat-Ereignisse werden ausschließlich an das aktuell gültige Ziel-Control gesendet.

Bei Fokuswechsel:

```text
Repeat Stop

↓

Focus Change

↓

Neuer Repeat
```

---

# 15. Sicherheit

Das System verhindert:

- doppelte Repeat-Kontexte
- Repeat ohne aktive Eingabe
- Endlosschleifen
- Wiederholung kritischer Aktionen
- beschädigte Repeat-Zustände

Alle Repeat-Kontexte werden validiert.

---

# 16. Speicherverwaltung

Das Repeat-System verwendet:

- feste Repeat-Einträge
- statische Timer
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 17. Performance

Anforderungen:

- O(1)-Repeat Start
- O(1)-Repeat Stop
- O(1)-Timer Update
- deterministische Laufzeit

Die Anzahl aktiver Wiederholungen darf die Eingabelatenz nicht beeinflussen.

---

# 18. API

```c
typedef struct
{
    uint32_t input_id;
    uint64_t start_time;
    uint64_t next_repeat;
    bool active;
}
nova_repeat_context_t;

nova_result_t
nova_repeat_initialize(void);

nova_result_t
nova_repeat_start(
    uint32_t input_id);

nova_result_t
nova_repeat_stop(
    uint32_t input_id);

bool
nova_repeat_active(
    uint32_t input_id);

nova_result_t
nova_repeat_update(void);
```

---

# 19. Diagnosedaten

Zu erfassen sind:

- Eingabe-ID
- Wiederholungsanzahl
- Initial Delay
- Wiederholungsrate
- Wiederholungsdauer
- Fehler
- Latenz

---

# 20. Testfälle

Zu testen sind:

- Tastatur
- Controller
- Navigation
- Slider
- Scrollbar
- Fokuswechsel
- Accessibility
- Mehrere Wiederholungen
- Initial Delay
- Fehlerfälle

---

# 21. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche unterstützten Eingaben deterministisch wiederholt werden.
- Initial Delay und Wiederholungsrate zentral konfigurierbar sind.
- Fokuswechsel aktive Wiederholungen korrekt beenden.
- Accessibility vollständig integriert ist.
- kritische Aktionen nicht automatisch wiederholt werden.
- mehrere Repeat-Kontexte gleichzeitig unterstützt werden.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 22. Referenzen

## ADRs

- ADR-BOOTINPUT-0011 – Input Repeat
- ADR-BOOTINPUT-0003 – Keyboard Input
- ADR-BOOTINPUT-0008 – Focus Management
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTINPUT-0003 – Keyboard Input
- NPSPEC-BOOTINPUT-0004 – Mouse Input
- NPSPEC-BOOTINPUT-0005 – Touch Input
- NPSPEC-BOOTINPUT-0008 – Focus Management
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 23. Zusammenfassung

Das **Input Repeat System** definiert die standardisierte Wiederholung kontinuierlicher Benutzereingaben innerhalb der NovaOS-Bootoberfläche. Es unterstützt Tastatur-, Controller- und Navigationsereignisse, verwaltet Initial Delay und Wiederholungsrate zentral und gewährleistet eine deterministische Verarbeitung unabhängig von BIOS oder UEFI. Durch feste Repeat-Kontexte, zentrale Timerverwaltung und vollständige Accessibility-Integration entsteht ein konsistentes und performantes Wiederholungssystem für sämtliche unterstützten Eingabegeräte.