# NPSPEC-BOOTDIALOG-0005
# Warning Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0005 |
| Titel | Warning Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0004, NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0005, ADR-BOOTSAFE-0002, ADR-BOOTACCESS-0001, ADR-BOOTMOTION-0008, ADR-BOOTINPUT-0006 |

---

# 1. Ziel

Diese Spezifikation definiert den **Warning Dialog** der NovaOS Boot UI.

Der Warning Dialog informiert den Benutzer über potenzielle Risiken oder ungewöhnliche Systemzustände, bevor diese zu Fehlern oder Datenverlust führen können.

Das System unterstützt:

- Systemwarnungen
- Konfigurationswarnungen
- Sicherheitswarnungen
- Integritätswarnungen
- Speicherwarnungen
- Bootwarnungen
- Hardwarewarnungen
- Recovery-Warnungen

Der Warning Dialog dient der frühzeitigen Information und unterstützt den Benutzer bei sicheren Entscheidungen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Warnmeldungen der Bootoberfläche.

Sie umfasst:

- Boot Manager
- Recovery
- Diagnose
- Systemkonfiguration
- Backup
- Wiederherstellung
- Hardwareprüfung

Nicht Bestandteil sind:

- Fehlermeldungen
- Informationsdialoge
- Progressdialoge

---

# 3. Architektur

```text
Application

↓

Warning Request

↓

Dialog Manager

↓

Warning Dialog

↓

User Decision

↓

Application
```

---

# 4. Grundprinzip

Ein Warning Dialog weist auf einen Zustand hin, der:

- Aufmerksamkeit erfordert
- Risiken beinhaltet
- den Bootvorgang beeinflussen kann
- überprüft werden sollte

Der Dialog informiert den Benutzer, bevor eine mögliche Fehlersituation eintritt.

---

# 5. Aufgaben

Der Warning Dialog übernimmt:

- Warnhinweise anzeigen
- Benutzer informieren
- Risiken erläutern
- Handlungsoptionen bereitstellen
- Navigation blockieren (bei modalen Warnungen)
- Benutzerentscheidung erfassen
- Ergebnis zurückgeben
- Warnstatus protokollieren

---

# 6. Dialogaufbau

Der Dialog besteht aus:

```text
Warnsymbol

↓

Titel

↓

Warnbeschreibung

↓

Optionale Detailinformationen

↓

Empfohlene Maßnahme

↓

Schaltflächen
```

---

# 7. Warnstufen

Unterstützt werden:

| Stufe | Beschreibung |
|---|---|
| Low | Hinweis |
| Medium | erhöhte Aufmerksamkeit |
| High | hohes Risiko |
| Critical | unmittelbare Gefahr |

Die Warnstufe beeinflusst Darstellung und Standardverhalten.

---

# 8. Schaltflächen

Standardmäßig werden unterstützt:

- OK
- Cancel
- Continue
- Retry
- Ignore
- Open Details

Die tatsächlich angezeigten Schaltflächen richten sich nach der Warnart.

---

# 9. Standardverhalten

Je nach Warnstufe:

**Low**

- Benutzer kann fortfahren.

**Medium**

- Benutzerentscheidung erforderlich.

**High**

- sichere Standardoption voreingestellt.

**Critical**

- zusätzliche Bestätigung möglich.

---

# 10. Warnsymbol

Jede Warnung besitzt ein eindeutiges Symbol.

Beispiele:

- allgemeine Warnung
- Datenträger
- Sicherheit
- Netzwerk
- Speicher
- Temperatur

Das Symbol unterstützt die schnelle Erkennung der Warnursache.

---

# 11. Zusatzinformationen

Optional können angezeigt werden:

- Fehlercode
- Laufwerk
- Dateisystem
- Gerätebezeichnung
- Uhrzeit
- Diagnoseinformationen

Technische Details können ein- oder ausgeblendet werden.

---

# 12. Navigation

Ist der Dialog modal:

- wird die Hintergrundnavigation blockiert.

Ist der Dialog nichtmodal:

- bleibt die Navigation verfügbar.

---

# 13. Benutzerentscheidung

Mögliche Ergebnisse:

```text
OK

Continue

Cancel

Retry

Ignore
```

Genau ein Ergebnis wird zurückgegeben.

---

# 14. Fokus

Beim Öffnen:

- erhält die empfohlene sichere Aktion den Fokus.

Der Fokus bleibt vollständig innerhalb des Dialogs.

---

# 15. Darstellung

Der Warning Dialog verwendet:

- Glass Dialog
- Warnsymbol
- Modal Layer (optional)
- Backdrop (optional)

Die Darstellung orientiert sich an der Warnstufe.

---

# 16. Motion

Standardanimation:

```text
Fade

+

Scale

+

Backdrop Fade
```

Bei Reduced Motion:

```text
Fade
```

---

# 17. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen
- vollständige Tastaturbedienung

Warnstufe und Titel müssen eindeutig ausgegeben werden.

---

# 18. Speicherverwaltung

Warning Dialoge verwenden:

- statische Dialogobjekte
- feste Layoutstrukturen
- feste Symboltabellen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Erzeugung
- O(1)-Schließen
- deterministische Laufzeit
- keine Heap-Allokationen

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Warnstufe
- fehlendes Warnsymbol
- beschädigte Dialogdaten
- ungültige Standardaktion
- Nullpointer

Im Fehlerfall wird eine generische Warnmeldung angezeigt.

---

# 21. Sicherheit

Das System verhindert:

- fehlende Warnhinweise
- automatische Fortsetzung kritischer Aktionen
- Fokusverlust
- Navigation außerhalb modaler Warnungen
- inkonsistente Warnzustände

Warnungen mit hoher oder kritischer Einstufung dürfen nicht automatisch bestätigt werden.

---

# 22. API

```c
typedef enum
{
    NOVA_WARNING_LOW,
    NOVA_WARNING_MEDIUM,
    NOVA_WARNING_HIGH,
    NOVA_WARNING_CRITICAL
}
nova_warning_level_t;

nova_result_t
nova_warning_dialog_show(
    nova_warning_level_t level,
    const char* title,
    const char* message);

nova_dialog_result_t
nova_warning_dialog_result(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Warnstufe
- Warncode
- Benutzerentscheidung
- Öffnungszeit
- Schließzeit
- Fokuswechsel
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Low Warning
- Medium Warning
- High Warning
- Critical Warning
- modale Warnung
- nichtmodale Warnung
- Accessibility
- Reduced Motion
- Recovery
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Warnstufen korrekt dargestellt werden.
- sichere Standardaktionen automatisch fokussiert werden.
- modale Warnungen die Hintergrundnavigation blockieren.
- Warnsymbole eindeutig der Warnart entsprechen.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0005 – Einheitliche Warnungsdialoge
- ADR-BOOTSAFE-0002 – Sichere Warn- und Bestätigungsdialoge
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- ADR-BOOTINPUT-0006 – Fokusverwaltung für Dialoge

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTDIALOG-0004 – Confirmation Dialog
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion

---

# 27. Zusammenfassung

Der **Warning Dialog** dient der frühzeitigen Anzeige potenzieller Risiken innerhalb der NovaOS Bootoberfläche. Durch klar definierte Warnstufen, standardisierte Warnsymbole, sichere Standardaktionen und vollständige Integration in Navigation, Motion und Accessibility entsteht ein konsistentes Warnsystem für BIOS- und UEFI-basierte Bootumgebungen. Die Architektur gewährleistet, dass sicherheitsrelevante Hinweise eindeutig kommuniziert und Benutzerentscheidungen kontrolliert verarbeitet werden.