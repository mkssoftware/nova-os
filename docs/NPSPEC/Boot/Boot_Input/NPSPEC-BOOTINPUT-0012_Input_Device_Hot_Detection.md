# NPSPEC-BOOTINPUT-0012
# Input Device Hot Detection

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0012 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTINPUT-0003, NPSPEC-BOOTINPUT-0004, NPSPEC-BOOTINPUT-0005, NPSPEC-BOOTINPUT-0006, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Input Device Hot Detection System** der NovaOS Boot UI.

Das System erkennt Eingabegeräte, die während der Laufzeit des Boot Managers verbunden oder entfernt werden, und integriert sie automatisch in das zentrale Eingabesystem. Dadurch kann die Bootoberfläche ohne Neustart auf neue Tastaturen, Mäuse, Touch-Geräte oder Controller reagieren.

Die Erkennung erfolgt vollständig hardwareunabhängig über eine zentrale Geräteverwaltung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- USB-Keyboards
- USB-Mäuse
- USB-Controller
- USB-Touchscreens
- Bluetooth-Eingabegeräte (UEFI, optional)
- zukünftige HID-Geräte

---

# 3. Ziele

Das Hot-Detection-System muss:

- Geräte automatisch erkennen
- Geräte automatisch entfernen
- hardwareunabhängig arbeiten
- deterministisch sein
- bestehende Eingaben nicht unterbrechen
- Accessibility unterstützen

---

# 4. Architektur

```text
Firmware

↓

Input HAL

↓

Device Detector

↓

Device Registry

↓

Input Manager

↓

Unified Input Events
```

---

# 5. Komponenten

Das System besteht aus:

- Device Detector
- Device Registry
- Device Validator
- Device Mapper
- Event Generator
- Device Monitor

---

# 6. Grundprinzip

Jedes Eingabegerät besitzt einen Lebenszyklus:

```text
Gerät erkannt

↓

Validierung

↓

Registrierung

↓

Aktiv

↓

Entfernt

↓

Freigabe
```

Geräte dürfen während der Laufzeit beliebig hinzugefügt oder entfernt werden.

---

# 7. Unterstützte Gerätetypen

Das System unterstützt:

### Tastatur

- USB HID
- PS/2
- Bluetooth HID (optional)

### Maus

- USB HID
- PS/2

### Touch

- HID Touch
- Multitouch

### Controller

- HID
- XInput-kompatibel

---

# 8. Geräteidentität

Jedes Gerät besitzt:

- Device ID
- Device Type
- Device Instance
- Herstellerkennung
- Produktkennung
- Firmwareinformationen (optional)

Die Device ID bleibt während der gesamten Laufzeit eindeutig.

---

# 9. Gerätezustände

Unterstützt werden:

```text
Unknown

Detected

Initializing

Ready

Disconnected

Error
```

Nur Geräte im Zustand **Ready** erzeugen Eingabeereignisse.

---

# 10. Gerätevalidierung

Vor der Aktivierung werden geprüft:

- Geräteklasse
- Protokoll
- unterstützte Funktionen
- Firmwarekompatibilität
- interne Datenstrukturen

Nicht unterstützte Geräte werden ignoriert.

---

# 11. Geräteereignisse

Folgende Ereignisse werden erzeugt:

```text
Device Added

Device Removed

Device Changed

Device Error

Device Ready
```

Alle Ereignisse werden über das Unified Input Event Model verarbeitet.

---

# 12. Gerätewechsel

Wird ein neues Eingabegerät erkannt:

```text
Gerät

↓

Device Added

↓

Registry

↓

Input Manager

↓

Verfügbar
```

Die aktive Benutzeroberfläche bleibt unverändert.

---

# 13. Geräteentfernung

Beim Entfernen eines Geräts:

- aktive Pointer werden freigegeben
- Pointer Capture wird beendet
- Fokus bleibt erhalten
- Wiederholungsereignisse werden gestoppt

Die Oberfläche bleibt weiterhin bedienbar.

---

# 14. Gerätepriorität

Mehrere Geräte desselben Typs werden gleichzeitig unterstützt.

Beispiele:

- zwei Tastaturen
- mehrere Mäuse
- mehrere Controller

Alle Geräte erzeugen gleichwertige Eingabeereignisse.

---

# 15. Accessibility

Das System unterstützt:

- alternative Eingabegeräte
- Spezialtastaturen
- Assistive Controller
- vereinfachte Geräteerkennung

Accessibility-Geräte werden wie Standardgeräte behandelt.

---

# 16. Sicherheit

Das System verhindert:

- doppelte Geräte-IDs
- fehlerhafte Geräteinitialisierung
- beschädigte Geräteinformationen
- rekursive Geräteereignisse
- ungültige Gerätetypen

Jedes Gerät wird vor der Registrierung vollständig validiert.

---

# 17. Speicherverwaltung

Das System verwendet:

- statische Gerätetabellen
- feste Registry-Einträge
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Geräteregistrierung
- O(1)-Lookup
- O(1)-Entfernung
- deterministische Laufzeit

Die Erkennung neuer Geräte darf die Eingabelatenz nicht beeinflussen.

---

# 19. API

```c
typedef enum
{
    NOVA_DEVICE_UNKNOWN,
    NOVA_DEVICE_KEYBOARD,
    NOVA_DEVICE_MOUSE,
    NOVA_DEVICE_TOUCH,
    NOVA_DEVICE_CONTROLLER
}
nova_input_device_type_t;

typedef struct
{
    uint32_t device_id;
    nova_input_device_type_t type;
    bool connected;
}
nova_input_device_t;

nova_result_t
nova_input_device_initialize(void);

nova_result_t
nova_input_device_scan(void);

uint32_t
nova_input_device_count(void);

const nova_input_device_t*
nova_input_device_get(
    uint32_t device_id);

bool
nova_input_device_connected(
    uint32_t device_id);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Geräte-ID
- Gerätetyp
- Verbindungsstatus
- Initialisierungszeit
- Gerätewechsel
- Fehler
- Registrierungsdauer

---

# 21. Testfälle

Zu testen sind:

- Tastatur anschließen
- Maus anschließen
- Controller anschließen
- Touch-Gerät anschließen
- Geräte entfernen
- Mehrere Geräte
- Gerätefehler
- Accessibility-Geräte
- Wiederholtes An- und Abstecken
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- unterstützte Geräte automatisch erkannt werden.
- Geräte ohne Neustart verwendet werden können.
- Geräteentfernungen korrekt verarbeitet werden.
- Pointer Capture und Wiederholungsereignisse sauber beendet werden.
- mehrere Geräte gleichzeitig unterstützt werden.
- Accessibility-Geräte vollständig integriert sind.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTINPUT-0012 – Input Device Hot Detection
- ADR-BOOTINPUT-0001 – Einheitliche Eingabearchitektur
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0001 – Performance Budget

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTINPUT-0003 – Keyboard Input
- NPSPEC-BOOTINPUT-0004 – Mouse Input
- NPSPEC-BOOTINPUT-0005 – Touch Input
- NPSPEC-BOOTINPUT-0006 – Pointer Model
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 24. Zusammenfassung

Das **Input Device Hot Detection System** definiert die automatische Erkennung und Verwaltung von Eingabegeräten innerhalb der NovaOS-Bootoberfläche. Es integriert neu angeschlossene Geräte ohne Neustart, entfernt getrennte Geräte kontrolliert aus dem Eingabesystem und erzeugt standardisierte Geräteereignisse über das Unified Input Event Model. Durch zentrale Geräteverwaltung, deterministische Zustandsübergänge, Unterstützung mehrerer gleichzeitiger Geräte und statische Speicherverwaltung entsteht eine robuste und erweiterbare Eingabeinfrastruktur für BIOS- und UEFI-basierte Bootumgebungen.