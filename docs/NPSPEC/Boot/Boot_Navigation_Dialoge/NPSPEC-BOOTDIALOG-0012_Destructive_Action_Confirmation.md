# NPSPEC-BOOTDIALOG-0012
# Destructive Action Confirmation

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0012 |
| Titel | Destructive Action Confirmation |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0011, NPSPEC-BOOTSAFE-0001 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0012, ADR-BOOTSAFE-0002, ADR-BOOTSEC-0002, ADR-BOOTACCESS-0001, ADR-BOOTINPUT-0006 |

---

# 1. Ziel

Diese Spezifikation definiert die **Destructive Action Confirmation** der NovaOS Boot UI.

Sie beschreibt die Bestätigung irreversibler oder potenziell datenzerstörender Aktionen während des Bootvorgangs.

Das System schützt den Benutzer vor versehentlichen Änderungen oder Datenverlust.

Es unterstützt:

- Datenträgerformatierung
- Partitionslöschung
- Bootloader-Neuinstallation
- Wiederherstellung auf Werkseinstellungen
- Löschen von Backups
- Schlüsselvernichtung
- Sicherheitsfunktionen
- zukünftige destruktive Systemaktionen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche irreversiblen Systemaktionen.

Sie umfasst:

- Boot Manager
- Recovery
- Datenträgerverwaltung
- Backup
- Wiederherstellung
- Verschlüsselung
- Wartungsfunktionen

Nicht Bestandteil sind:

- normale Bestätigungen
- Informationsdialoge
- Warnmeldungen

---

# 3. Architektur

```text
Application

↓

Risk Evaluation

↓

Destructive Confirmation

↓

User Validation

↓

Application
```

---

# 4. Grundprinzip

Keine destruktive Aktion darf ohne ausdrückliche Benutzerbestätigung ausgeführt werden.

Bei kritischen Aktionen genügt eine einfache Schaltflächenbestätigung nicht.

Je nach Risiko können zusätzliche Bestätigungsschritte erforderlich sein.

---

# 5. Aufgaben

Das System übernimmt:

- Risikoeinstufung
- Sicherheitsbestätigung
- Benutzerwarnung
- Ergebnisverwaltung
- Fokussteuerung
- Schutz vor Fehlbedienung
- Protokollierung
- sichere Freigabe

---

# 6. Risikoklassen

Unterstützt werden:

| Klasse | Beschreibung |
|---|---|
| Medium | teilweise rückgängig machbar |
| High | erheblicher Datenverlust möglich |
| Critical | dauerhaft irreversibel |
| System | sicherheitskritische Systemänderung |

Die Risikoklasse bestimmt den erforderlichen Bestätigungsumfang.

---

# 7. Dialogaufbau

Der Dialog besteht aus:

```text
Warnsymbol

↓

Titel

↓

Beschreibung

↓

Folgen der Aktion

↓

Betroffene Objekte

↓

Bestätigung

↓

Schaltflächen
```

---

# 8. Sicherheitsinformationen

Der Dialog beschreibt eindeutig:

- welche Daten betroffen sind
- welche Folgen entstehen
- ob die Aktion rückgängig gemacht werden kann
- welche Alternativen existieren

Fachbegriffe sollen vermieden oder erläutert werden.

---

# 9. Bestätigungsstufen

Je nach Risiko:

**Medium**

- einfache Bestätigung

**High**

- zusätzliche Rückfrage

**Critical**

- Eingabe eines Bestätigungstextes oder Kennworts

**System**

- Administratorbestätigung oder Sicherheitsnachweis

---

# 10. Standardaktion

Die sichere Aktion erhält immer den Initialfokus.

Beispiele:

- Cancel
- Abbrechen
- Zurück

Eine destruktive Aktion darf niemals standardmäßig ausgewählt sein.

---

# 11. Schaltflächen

Standardmäßig werden unterstützt:

- Cancel
- Continue
- Delete
- Format
- Restore
- Shutdown

Die Beschriftung soll die konkrete Aktion eindeutig benennen.

---

# 12. Bestätigungstext

Optional kann verlangt werden:

```text
FORMATIEREN

oder

DELETE
```

Die Aktion wird erst nach korrekter Eingabe freigegeben.

---

# 13. Navigation

Während der Dialog aktiv ist:

- Hintergrundnavigation blockiert
- Fokus eingeschlossen
- keine Seitennavigation

Back entspricht standardmäßig **Cancel**.

---

# 14. Darstellung

Der Dialog verwendet:

- Glass Dialog
- Modal Layer
- Backdrop
- Warnsymbol
- hervorgehobene Warninformationen

Die Gestaltung richtet sich nach der Risikoklasse.

---

# 15. Motion

Standardanimation:

```text
Fade

+

Scale
```

Bei Reduced Motion:

```text
Fade
```

Animationen dürfen den Benutzer nicht von Warnhinweisen ablenken.

---

# 16. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen
- vollständige Tastaturbedienung

Warnhinweise und Folgen der Aktion müssen vollständig ausgegeben werden.

---

# 17. Speicherverwaltung

Das System verwendet:

- statische Dialogobjekte
- feste Layoutstrukturen
- feste Eingabepuffer

Sensible Eingaben werden nach Abschluss sicher gelöscht.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Dialogerzeugung
- O(1)-Bestätigungsprüfung
- deterministische Laufzeit
- keine Heap-Allokationen

---

# 19. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Risikoklasse
- ungültiger Bestätigungstext
- fehlende sichere Standardaktion
- beschädigte Dialogdaten
- Nullpointer

Im Fehlerfall wird die Aktion automatisch abgebrochen.

---

# 20. Sicherheit

Das System verhindert:

- unbeabsichtigte Datenlöschung
- automatische Bestätigungen
- Standardfokus auf destruktiven Aktionen
- unvollständige Warnhinweise
- Umgehung der Sicherheitsbestätigung

Irreversible Aktionen dürfen ausschließlich nach erfolgreicher Validierung ausgeführt werden.

---

# 21. API

```c
typedef enum
{
    NOVA_RISK_MEDIUM,
    NOVA_RISK_HIGH,
    NOVA_RISK_CRITICAL,
    NOVA_RISK_SYSTEM
}
nova_risk_level_t;

nova_result_t
nova_destructive_dialog_show(
    nova_risk_level_t level,
    const char* title,
    const char* message);

nova_dialog_result_t
nova_destructive_dialog_result(void);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Risikoklasse
- Aktion
- Benutzerentscheidung
- Bestätigungsstufe
- Öffnungszeit
- Schließzeit
- Speicherverbrauch
- Fehleranzahl

Sensible Eingaben dürfen nicht protokolliert werden.

---

# 23. Testfälle

Zu testen sind:

- Medium Risk
- High Risk
- Critical Risk
- System Risk
- Bestätigungstext
- Passwortbestätigung
- Back Navigation
- Accessibility
- Reduced Motion
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede destruktive Aktion eine Sicherheitsbestätigung erfordert.
- die sichere Aktion standardmäßig fokussiert wird.
- zusätzliche Bestätigungsschritte abhängig von der Risikoklasse unterstützt werden.
- sensible Eingaben nach Abschluss sicher gelöscht werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTDIALOG-0012 – Bestätigung destruktiver Aktionen
- ADR-BOOTSAFE-0002 – Sichere Bestätigungsabläufe
- ADR-BOOTSEC-0002 – Schutz kritischer Systemfunktionen
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTINPUT-0006 – Fokusverwaltung

## NPSPECs

- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTDIALOG-0004 – Confirmation Dialog
- NPSPEC-BOOTDIALOG-0011 – Dialog Result and Cancellation
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Die **Destructive Action Confirmation** definiert den standardisierten Sicherheitsdialog für sämtliche irreversiblen oder sicherheitskritischen Aktionen innerhalb der NovaOS Bootoberfläche. Durch risikobasierte Bestätigungsstufen, eindeutige Warnhinweise, sichere Standardaktionen und optionale Mehrfachbestätigungen schützt das System zuverlässig vor unbeabsichtigten Änderungen und Datenverlust. Die vollständige Integration in Navigation, Accessibility und das Dialogsystem gewährleistet ein konsistentes Verhalten auf BIOS- und UEFI-Plattformen.