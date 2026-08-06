# NPSPEC-BOOTMANAGER-UI-0006
# Snapshot Selection

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0006 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Recovery / Self-Healing |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0004, NPSPEC-BOOTMANAGER-UI-0005, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TILE-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTSNAPSHOT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den Bildschirm **Snapshot Selection** des NovaOS Boot Managers.

Die Snapshot Selection ermöglicht dem Benutzer die Auswahl eines verfügbaren System-Snapshots zur Wiederherstellung, Analyse oder zum temporären Start. Die Oberfläche dient als zentrale Verwaltung aller vom System erkannten Snapshots.

Die Snapshot-Auswahl ist Bestandteil der Recovery- und Self-Healing-Umgebung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- lokale Snapshots
- automatische Snapshots
- manuelle Snapshots
- Recovery-Snapshots
- schreibgeschützte Snapshots
- zukünftige Snapshot-Typen

---

# 3. Ziele

Die Snapshot Selection muss:

- sämtliche Snapshots übersichtlich darstellen
- schnelle Wiederherstellung ermöglichen
- Metadaten vollständig anzeigen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

Zurück

Snapshot Selection

---------------------------------------------------------------

Filter

Suche

---------------------------------------------------------------

Snapshot Tiles

Snapshot Tiles

Snapshot Tiles

---------------------------------------------------------------

Snapshot Details

---------------------------------------------------------------

[ Wiederherstellen ]

[ Nur starten ]

[ Details ]

[ Löschen ]

[ Zurück ]

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- Filterleiste
- Suchfeld
- Snapshotliste
- Detailbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Snapshot-Icon
- Titel
- Anzahl verfügbarer Snapshots

---

# 7. Snapshotliste

Jeder Snapshot wird als Snapshot Tile dargestellt.

Ein Tile enthält:

- Snapshotname
- Erstellungsdatum
- Uhrzeit
- Betriebssystem
- Größe
- Status Badge
- Typ
- Integritätsstatus

---

# 8. Snapshottypen

Unterstützt werden:

- automatisch
- manuell
- Recovery
- vor Update
- vor Installation
- Sicherungspunkt
- Nur Lesen

Neue Typen können ergänzt werden.

---

# 9. Detailbereich

Beim Markieren eines Snapshots werden angezeigt:

- vollständiger Name
- Beschreibung
- Erstellungszeitpunkt
- Betriebssystem
- Kernelversion
- Dateisystem
- Snapshotgröße
- Integritätsstatus
- Prüfsumme
- Ersteller

---

# 10. Filter

Unterstützt werden:

- Typ
- Datum
- Integritätsstatus
- Betriebssystem
- Favoriten (optional)

Filter können kombiniert werden.

---

# 11. Suche

Die Suche unterstützt:

- Snapshotname
- Beschreibung
- Version
- Datum

Die Suche erfolgt in Echtzeit.

---

# 12. Aktionen

Standardaktionen:

- Wiederherstellen
- Nur starten
- Details anzeigen
- Löschen
- Zurück

Optional:

- Umbenennen
- Exportieren
- Prüfsumme anzeigen

---

# 13. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch
- Gamepad (optional)

Die Navigation erfolgt gemäß Boot Input Architecture.

---

# 14. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- weich animiert werden
- Suchfeld und Liste eindeutig unterscheiden

---

# 15. Animationen

Unterstützt werden:

- Listenanimationen
- Fokusanimationen
- Suchanimationen
- Tileanimationen
- Dialoganimationen

Alle Animationen verwenden Motion Tokens.

---

# 16. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen aus Design Tokens.

---

# 17. Accessibility

Die Snapshot Selection unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Funktionen müssen ohne Maus erreichbar sein.

---

# 18. Fehlerfälle

Zu behandeln sind:

- Snapshot beschädigt
- Snapshot unvollständig
- Integritätsprüfung fehlgeschlagen
- Snapshot nicht kompatibel
- Snapshot nicht vorhanden

Fehler werden über Dialoge dargestellt.

---

# 19. Sicherheit

Vor einer Wiederherstellung werden geprüft:

- Integrität
- Kompatibilität
- Signatur
- Dateisystem
- ausreichender Speicherplatz

Das Löschen eines Snapshots verlangt immer eine Bestätigung.

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Snapshot Pool
- List Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Öffnungszeit < 100 ms
- O(1)-Snapshot Lookup
- O(1)-Filter Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Metadaten werden vor dem Anzeigen geladen.

---

# 22. API

```c
typedef struct
{
    uint64_t snapshot_id;
    uint64_t timestamp;
    bool valid;
    bool bootable;
}
nova_snapshot_descriptor_t;

nova_result_t
nova_snapshot_selection_open(void);

nova_result_t
nova_snapshot_restore(
    uint64_t snapshot_id);

nova_result_t
nova_snapshot_boot(
    uint64_t snapshot_id);

nova_result_t
nova_snapshot_selection_close(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Snapshotanzahl
- Filter
- Suchbegriff
- ausgewählter Snapshot
- Integritätsstatus
- Ladezeit
- Validierungsfehler

---

# 24. Testfälle

Zu testen sind:

- automatische Snapshots
- manuelle Snapshots
- beschädigte Snapshots
- Wiederherstellung
- Nur starten
- Suche
- Filter
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Snapshots vollständig dargestellt werden.
- Filter und Suche korrekt funktionieren.
- Wiederherstellung und temporärer Start unterstützt werden.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- beschädigte Snapshots eindeutig gekennzeichnet werden.
- sämtliche Metadaten korrekt angezeigt werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0006 – Snapshot Selection
- ADR-BOOTSNAPSHOT-0001 – Snapshot Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0004 – Recovery Menu
- NPSPEC-BOOTMANAGER-UI-0005 – Self-Healing Menu
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TILE-0001 – Boot Option Tile
- NPSPEC-BOOTDIALOG-0001 – Dialog
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTSNAPSHOT-0001 – Snapshot Framework

---

# 27. Zusammenfassung

Die **Snapshot Selection** bildet die zentrale Verwaltungsoberfläche für alle System-Snapshots innerhalb der NovaOS Recovery- und Self-Healing-Umgebung. Sie kombiniert eine leistungsfähige Such- und Filterfunktion mit detaillierten Metadaten und sicheren Wiederherstellungs- bzw. Startoptionen. Durch vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente, performante und benutzerfreundliche Oberfläche zur Verwaltung und Wiederherstellung von Systemzuständen.