# NPSPEC-BOOTMANAGER-UI-0017
# Default Boot Entry Selection

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0017 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-RADIOBUTTON-0001, NPSPEC-BOOTCONTROL-STATUSBADGE-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTCONFIG-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Benutzeroberfläche **Default Boot Entry Selection**.

Sie ermöglicht dem Benutzer, den standardmäßigen Boot-Eintrag festzulegen, der nach Ablauf des Automatic Boot Countdowns automatisch gestartet wird.

Die Änderung erfolgt über eine sichere und nachvollziehbare Oberfläche und wird dauerhaft in der Bootkonfiguration gespeichert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- lokale Betriebssysteme
- Recovery-Einträge
- Snapshot-Boots
- Netzwerk-Boot-Einträge
- zukünftige Bootmodule

---

# 3. Ziele

Die Oberfläche muss:

- den aktuellen Standard eindeutig anzeigen
- einen neuen Standard auswählen lassen
- Änderungen sicher speichern
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- vollständig in die NovaOS Design Language integriert sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

Zurück

Standard-Boot auswählen

---------------------------------------------------------------

○ NovaOS Stable

● NovaOS Testing

○ Recovery

○ Netzwerk-Boot

○ Snapshot

---------------------------------------------------------------

Beschreibung

---------------------------------------------------------------

Aktueller Standard:
NovaOS Testing

---------------------------------------------------------------

[ Übernehmen ]

[ Abbrechen ]

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Die Oberfläche besteht aus:

- Header
- Bootliste
- Detailbereich
- Statusbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Titel
- Kurzbeschreibung

---

# 7. Bootliste

Für jeden Eintrag werden dargestellt:

- Betriebssystemname
- Icon
- Version
- Datenträger
- Status Badge
- Standardkennzeichnung

Der Standard-Boot-Eintrag wird zusätzlich hervorgehoben.

---

# 8. Auswahlmodell

Es darf immer nur **ein** Standard-Boot-Eintrag existieren.

Die Auswahl erfolgt über Radio Buttons oder ein gleichwertiges exklusives Auswahlmodell.

---

# 9. Detailbereich

Beim Markieren eines Eintrags werden angezeigt:

- vollständiger Name
- Version
- Kernel
- Datenträger
- Partition
- Bootmodus
- letzter erfolgreicher Start

---

# 10. Statusbereich

Der Statusbereich zeigt:

- aktueller Standard
- geänderte Auswahl
- Countdowndauer
- Speicherstatus

---

# 11. Aktionen

Standardaktionen:

- Übernehmen
- Abbrechen

Optional:

- Booteintrag testen
- Countdown konfigurieren
- Erweiterte Optionen

---

# 12. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Navigation erfolgt gemäß Boot Input Architecture.

---

# 13. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- weich animiert werden
- automatisch auf dem aktuellen Standard beginnen

---

# 14. Animationen

Unterstützt werden:

- Fokusanimationen
- Listenanimationen
- Statusanimationen
- Dialoganimationen

Alle Animationen verwenden Motion Tokens.

---

# 15. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 16. Accessibility

Die Oberfläche unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Der aktuelle Standard muss zusätzlich textlich gekennzeichnet werden.

---

# 17. Fehlerfälle

Zu behandeln sind:

- kein Standard-Boot vorhanden
- ausgewählter Eintrag ungültig
- Eintrag beschädigt
- Speichern fehlgeschlagen
- Bootkonfiguration nicht verfügbar

Fehler werden als Dialog dargestellt.

---

# 18. Sicherheit

Vor dem Speichern werden geprüft:

- Eintrag vorhanden
- Eintrag bootfähig
- Integritätsstatus
- Konfigurationsschreibrechte

Ein beschädigter Boot-Eintrag darf nicht als Standard gespeichert werden.

---

# 19. Speicherverwaltung

Die Oberfläche verwendet:

- List Pool
- Dialog Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 20. Performance

Anforderungen:

- Initialisierung < 30 ms
- O(1)-Boot Entry Lookup
- O(1)-Standard Lookup
- O(1)-Theme Lookup
- deterministische Aktualisierung

---

# 21. API

```c
typedef struct
{
    uint32_t default_entry;
    bool modified;
}
nova_default_boot_selection_t;

nova_result_t
nova_default_boot_selection_open(void);

nova_result_t
nova_default_boot_selection_set(
    uint32_t boot_entry);

nova_result_t
nova_default_boot_selection_save(void);

const nova_default_boot_selection_t*
nova_default_boot_selection_status(void);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- bisheriger Standard
- neuer Standard
- Benutzeraktion
- Speichervorgang
- Theme
- Ladezeit

---

# 23. Testfälle

Zu testen sind:

- Standard ändern
- Standard unverändert
- ungültiger Boot-Eintrag
- beschädigter Eintrag
- Themewechsel
- Accessibility
- Tastatursteuerung
- Speichern
- Abbrechen
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- immer genau ein Standard-Boot-Eintrag definiert ist.
- Änderungen zuverlässig gespeichert werden.
- beschädigte Boot-Einträge nicht als Standard gewählt werden können.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- sämtliche Informationen korrekt dargestellt werden.
- die Oberfläche vollständig per Tastatur bedienbar ist.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 25. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0017 – Default Boot Entry Selection
- ADR-BOOTCONFIG-0001 – Boot Configuration
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-RADIOBUTTON-0001 – Radio Button
- NPSPEC-BOOTCONTROL-STATUSBADGE-0001 – Status Badge
- NPSPEC-BOOTCONTROL-DIALOG-0001 – Dialog
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTCONFIG-0001 – Boot Configuration

---

# 26. Zusammenfassung

Die **Default Boot Entry Selection** stellt die zentrale Benutzeroberfläche zur Verwaltung des Standard-Boot-Eintrags im NovaOS Boot Manager bereit. Sie ermöglicht eine sichere Auswahl des automatisch zu startenden Betriebssystems, validiert den ausgewählten Eintrag vor dem Speichern und integriert sich vollständig in die NovaOS Design Language. Durch eindeutige Statusanzeigen, exklusive Auswahlmechanismen und vollständige Unterstützung für verschiedene Eingabemethoden entsteht eine konsistente und zuverlässige Konfigurationsoberfläche für den automatischen Systemstart.