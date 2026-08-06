# NPSPEC-BOOTMANAGER-UI-0002
# Boot Entry Details Dialog

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0002 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001, NPSPEC-BOOTCONTROL-CARD-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTCONTROL-BUTTON-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Boot Entry Details Dialog** der NovaOS-Bootoberfläche.

Der Dialog dient der detaillierten Darstellung eines einzelnen Boot-Eintrags. Er ermöglicht dem Benutzer, technische Informationen einzusehen, den Zustand des Eintrags zu überprüfen und – sofern zulässig – erweiterte Aktionen für den ausgewählten Boot-Eintrag auszuführen.

Der Dialog ist ausschließlich informativ, sofern keine expliziten Bearbeitungsfunktionen aktiviert wurden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- lokale Betriebssysteme
- Recovery-Einträge
- Installationsmedien
- Netzwerk-Boot-Einträge
- Diagnosemodule
- zukünftige Bootquellen

---

# 3. Ziele

Der Dialog muss:

- vollständige Informationen bereitstellen
- übersichtlich strukturiert sein
- konsistent zur NovaOS Design Language sein
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- auf allen unterstützten Auflösungen funktionieren

---

# 4. Dialogaufbau

```text
+--------------------------------------------------+

Icon

Boot Entry Name

Kurze Beschreibung

----------------------------------------------------

Eigenschaften

• Betriebssystem

• Version

• Kernel

• Architektur

• Bootmodus

• Datenträger

• Partition

• Dateisystem

• UUID

• Standardstart

• Letzter erfolgreicher Start

----------------------------------------------------

Status

----------------------------------------------------

Aktionen

[ Starten ]

[ Als Standard ]

[ Erweiterte Optionen ]

[ Schließen ]

+--------------------------------------------------+
```

---

# 5. Komponenten

Der Dialog besteht aus:

- Dialogtitel
- Icon
- Eigenschaftenliste
- Statusbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Titelbereich

Der Kopfbereich enthält:

- Betriebssystemicon
- Anzeigename
- kurze Beschreibung
- optional Version

Der Titelbereich bleibt stets sichtbar.

---

# 7. Eigenschaften

Mindestens folgende Eigenschaften werden angezeigt:

- Anzeigename
- Betriebssystem
- Version
- Kernel
- Architektur
- Firmwaremodus
- Bootloader
- Datenträger
- Partition
- Dateisystem
- UUID
- Installationspfad
- Standardstatus

Weitere Eigenschaften dürfen ergänzt werden.

---

# 8. Statusbereich

Der Statusbereich informiert über:

- Bootfähig
- Standardstart
- Recovery verfügbar
- Fehler erkannt
- Integrität geprüft
- Signaturstatus
- Letzter erfolgreicher Start

Statusinformationen werden über Status Badges dargestellt.

---

# 9. Aktionen

Standardaktionen:

- Starten
- Als Standard festlegen
- Erweiterte Optionen
- Schließen

Optional:

- Reparieren
- Recovery starten
- Integrität prüfen

---

# 10. Layout

Der Dialog verwendet:

- Glass Card
- vertikale Informationsgruppen
- konsistente Abstände
- responsive Breiten

Das Layout wird über die Boot Layout Engine definiert.

---

# 11. Navigation

Unterstützt werden:

- Maus
- Tastatur
- Touch

Die Tab-Reihenfolge folgt der visuellen Struktur.

---

# 12. Fokus

Der Fokus muss:

- eindeutig sichtbar sein
- weich animiert werden
- niemals verloren gehen

Beim Öffnen erhält die primäre Aktion den Fokus.

---

# 13. Animation

Der Dialog unterstützt:

- Fade-In
- Scale-In
- Fokusanimationen
- Buttonanimationen
- Fade-Out

Alle Bewegungen verwenden Motion Tokens.

---

# 14. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen aus Design Tokens.

---

# 15. Accessibility

Der Dialog unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Inhalte müssen ohne Maus erreichbar sein.

---

# 16. Fehlerfälle

Der Dialog muss folgende Situationen behandeln:

- beschädigter Boot-Eintrag
- fehlende Partition
- ungültige UUID
- beschädigte Konfiguration
- fehlende Signatur

Fehler werden deutlich hervorgehoben.

---

# 17. Sicherheit

Der Dialog verhindert:

- Bearbeitung schreibgeschützter Einträge
- ungültige Standardzuweisungen
- Start nicht bootfähiger Einträge
- Ausführung unzulässiger Aktionen

Vor jeder Aktion erfolgt eine Validierung.

---

# 18. Speicherverwaltung

Der Dialog verwendet:

- Dialog Pool
- Property Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 19. Performance

Anforderungen:

- Öffnungszeit < 50 ms
- O(1)-Property Lookup
- O(1)-Status Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Alle Daten werden vor dem Öffnen geladen.

---

# 20. API

```c
typedef struct
{
    uint32_t boot_entry_id;
    bool bootable;
    bool is_default;
}
nova_boot_entry_details_context_t;

nova_result_t
nova_boot_entry_details_open(
    uint32_t boot_entry_id);

nova_result_t
nova_boot_entry_details_close(void);

const nova_boot_entry_details_context_t*
nova_boot_entry_details_current(void);

nova_result_t
nova_boot_entry_details_refresh(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Boot Entry ID
- Öffnungszeit
- Theme
- Anzahl Eigenschaften
- Statusinformationen
- Validierungsfehler
- Benutzeraktionen

---

# 22. Testfälle

Zu testen sind:

- gültiger Boot-Eintrag
- beschädigter Eintrag
- Recovery-Eintrag
- Netzwerk-Boot
- Themewechsel
- Tastatursteuerung
- Touchsteuerung
- Accessibility
- verschiedene Auflösungen
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Eigenschaften des Boot-Eintrags korrekt dargestellt werden.
- Statusinformationen vollständig angezeigt werden.
- alle unterstützten Aktionen korrekt funktionieren.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- der Dialog vollständig per Tastatur bedienbar ist.
- beschädigte Einträge eindeutig gekennzeichnet werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- alle Animationen ausschließlich Motion Tokens verwenden.

---

# 24. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0002 – Boot Entry Details Dialog
- ADR-BOOTDIALOG-0001 – Dialog Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-CARD-0001 – Glass Card
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTCONTROL-BUTTON-0001 – Button
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine

---

# 25. Zusammenfassung

Der **Boot Entry Details Dialog** stellt sämtliche relevanten Informationen eines Boot-Eintrags in einer klar strukturierten, modernen und konsistenten Oberfläche bereit. Er kombiniert technische Eigenschaften, Statusinformationen und zulässige Aktionen in einem dialogbasierten Aufbau und integriert sich vollständig in die NovaOS Design Language, das Theme-System und das Motion Framework. Dadurch ermöglicht er eine sichere und komfortable Verwaltung einzelner Boot-Einträge innerhalb des NovaOS Boot Managers.