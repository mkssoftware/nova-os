# ADR-POWER-0005 – Thermal Management

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-POWER-0005 |
| Titel | Thermal Management |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Energieverwaltung und Hardwareschutz |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-POWER-0001, ADR-POWER-0002, ADR-POWER-0004, ADR-DEV-0001, ADR-DEV-0002, ADR-DEV-0005 |
| Nachfolgende Spezifikation | NPSPEC-POWER-THERMAL-MANAGEMENT |

---

## 1. Kontext

Prozessoren, Grafikadapter, Speichergeräte, Akkus und andere Komponenten erzeugen während des Betriebs Wärme.

Zu hohe Temperaturen können verursachen:

- instabiles Systemverhalten
- Datenfehler
- verkürzte Hardwarelebensdauer
- Leistungseinbrüche
- Akkuschäden
- dauerhafte Geräteschäden
- Brand- oder Sicherheitsrisiken

NovaOS benötigt deshalb ein zentrales Thermal Management, das Temperaturen überwacht und rechtzeitig Gegenmaßnahmen einleitet.

---

## 2. Problemstellung

Temperatursensoren und Kühlmechanismen unterscheiden sich erheblich zwischen Plattformen.

NovaOS muss:

- verschiedene Sensorquellen vereinheitlichen
- fehlerhafte Messwerte erkennen
- thermische Zonen modellieren
- Lüfter und Leistung kontrollieren
- passive und aktive Kühlung koordinieren
- kritische Grenzwerte erzwingen
- unnötiges Drosseln vermeiden
- auch bei Ausfall des Userspace reagieren können

Der Schutz der Hardware muss Vorrang vor Leistungs- und Benutzerpräferenzen besitzen.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen Thermal Manager als sicherheitskritische Komponente des Power Managers.

Der Thermal Manager:

- sammelt validierte Sensordaten,
- verwaltet Thermal Zones,
- bewertet Trip Points,
- steuert Cooling Devices,
- koordiniert CPU-, GPU- und Device-Leistung,
- löst bei kritischen Temperaturen einen sicheren Shutdown aus.

Die unmittelbare kritische Schutzlogik bleibt kernelnah. Komplexe Profile, Benutzeroberflächen und Langzeitdiagnose können durch einen Userspace-Dienst bereitgestellt werden.

---

## 4. Architektur

```text
Temperatursensoren
        |
        v
   Thermal Zones
        |
        v
  Thermal Governor
        |
        +--> Lüfter
        +--> CPU/GPU-Drosselung
        +--> Device Power
        +--> Lastbegrenzung
        +--> Shutdown
```

Plattformspezifische Sensoren und Kühlgeräte werden über versionierte Treiber abstrahiert.

---

## 5. Thermal Zones

Eine Thermal Zone repräsentiert einen thermisch zusammenhängenden Bereich.

Beispiele:

- CPU Package
- einzelne CPU-Kerne
- GPU
- Mainboard
- Akku
- NVMe-Gerät
- Gehäuse
- Netzteil
- Umgebungssensor

Eine Zone kann mehrere Sensoren und mehrere Cooling Devices besitzen.

```c
typedef struct thermal_zone_info {
    uint64_t zone_id;
    uint32_t state;
    int32_t temperature_millicelsius;
    int32_t trend_millicelsius_per_second;
    uint32_t sensor_count;
    uint32_t cooling_device_count;
} thermal_zone_info_t;
```

---

## 6. Temperatureinheit

NovaOS verwendet intern Milligrad Celsius:

```text
1 °C = 1000 m°C
```

Beispiel:

```text
72,5 °C = 72500 m°C
```

Treiber müssen ihre nativen Sensordaten in diese Einheit umrechnen.

Ungültige, nicht verfügbare oder außerhalb des Sensors liegende Werte werden ausdrücklich als Fehlerzustand gemeldet und nicht als reale Temperatur interpretiert.

---

## 7. Sensorquellen

Temperaturdaten können stammen aus:

- CPU-internen Sensoren
- ACPI
- Embedded Controller
- Device Tree
- GPU- und Gerätetreibern
- NVMe- und SMART-Daten
- Akkumanagement
- Plattformcontroller
- Hypervisor-Schnittstellen
- externen Sensorsystemen

Jede Quelle meldet:

- Messwert
- Genauigkeit oder Toleranz
- Aktualisierungsrate
- gültigen Messbereich
- Fehlerstatus
- zugehörige Thermal Zone

Firmwarewerte gelten als nicht vertrauenswürdig und werden validiert.

---

## 8. Sensorvalidierung

Der Thermal Manager prüft Messwerte auf:

- gültigen Wertebereich
- unrealistische Sprünge
- feststehende Werte
- fehlende Aktualisierungen
- widersprüchliche Sensoren
- ungültige Einheiten
- Überläufe
- bekannte Firmwarefehler

Bei mehreren Sensoren kann die Zone konservativ den höchsten plausiblen Wert verwenden.

Ein ausgefallener Sensor darf nicht automatisch als niedrige Temperatur behandelt werden.

---

## 9. Trip Points

Eine Thermal Zone besitzt mehrere Temperaturschwellen.

| Trip Point | Bedeutung |
|---|---|
| `PASSIVE` | Leistungsaufnahme reduzieren |
| `ACTIVE` | aktive Kühlung erhöhen |
| `HOT` | starke Drosselung und Funktionsbegrenzung |
| `CRITICAL` | sicherer Shutdown erforderlich |
| `EMERGENCY` | sofortige Notabschaltung |

Trip Points enthalten:

- Aktivierungstemperatur
- Hysterese
- Priorität
- zugehörige Cooling Devices
- maximale Reaktionszeit
- Herkunft und Vertrauensstufe

Kritische Hardwaregrenzen können durch Benutzerprofile nicht angehoben werden.

---

## 10. Hysterese

Hysterese verhindert ein schnelles Wechseln zwischen Zuständen.

Beispiel:

```text
ACTIVE bei 80 °C einschalten
unter 75 °C wieder reduzieren
```

Jeder Trip Point besitzt eine definierte Rückkehrschwelle.

Zusätzlich kann der Governor eine Mindestverweildauer für einen Cooling State verwenden.

Sicherheitskritische Maßnahmen dürfen durch Hysterese nicht gefährlich verzögert werden.

---

## 11. Cooling Devices

Ein Cooling Device ist ein steuerbarer Mechanismus zur Temperaturreduktion.

Beispiele:

- Lüfter
- CPU-Frequenzbegrenzung
- CPU-Core-Parking
- GPU-Leistungslimit
- Device Sleep
- Displayhelligkeit
- Ladeleistungsbegrenzung
- Netzwerk- oder Speicherleistungsbegrenzung
- Prozesslastbegrenzung

Jedes Cooling Device meldet eine geordnete Menge von Cooling States.

Ein höherer Zustand bedeutet eine stärkere Kühlwirkung oder Leistungsbegrenzung.

---

## 12. Aktive Kühlung

Aktive Kühlung verwendet physische Kühlkomponenten, insbesondere Lüfter.

Der Thermal Manager kann:

- Lüfterstufen setzen
- PWM-Werte steuern
- mehrere Lüfter koordinieren
- Mindestdrehzahlen erzwingen
- Lüfterfehler erkennen
- akustische Profile berücksichtigen

Leiser Betrieb ist ein Optimierungsziel, aber kein Grund, thermische Sicherheitsgrenzen zu verletzen.

Bei Lüfterausfall werden passive Maßnahmen früher und stärker eingesetzt.

---

## 13. Passive Kühlung

Passive Kühlung reduziert die Wärmeproduktion.

Maßnahmen können sein:

1. Hintergrundprozesse drosseln,
2. CPU- und GPU-Frequenz reduzieren,
3. Turbo- und Boost-Modi begrenzen,
4. tiefe Device-Sleep-Zustände bevorzugen,
5. nicht benötigte CPU-Kerne deaktivieren,
6. Ladeleistung reduzieren,
7. nicht kritische Geräte abschalten.

Echtzeit- und QoS-Anforderungen werden berücksichtigt, können kritischen Thermalschutz aber nicht außer Kraft setzen.

---

## 14. Thermal Governor

Der Thermal Governor entscheidet anhand von:

- aktueller Temperatur
- Temperaturtrend
- Trip Points
- Sensorvertrauen
- verfügbarer Kühlleistung
- Energieprofil
- bisheriger Reaktion
- Hardwareabhängigkeiten

Er reagiert nicht nur auf die aktuelle Temperatur, sondern auch auf schnelle Temperaturanstiege.

Die Regelung soll Überschwingen und unnötiges Takten der Cooling States vermeiden.

---

## 15. Reaktionsstufen

NovaOS verwendet grundsätzlich folgende Eskalation:

```text
Normalbetrieb
     |
     v
aktive Kühlung erhöhen
     |
     v
Hintergrundlast begrenzen
     |
     v
CPU/GPU und Geräte drosseln
     |
     v
nicht kritische Funktionen deaktivieren
     |
     v
sicherer Shutdown
     |
     v
Notabschaltung
```

Bei sehr schnellem Temperaturanstieg dürfen Stufen übersprungen werden.

---

## 16. CPU- und Scheduler-Integration

Der Thermal Manager kann dem Scheduler Leistungsgrenzen mitteilen.

Mögliche Maßnahmen:

- Hintergrundklassen begrenzen
- neue Last auf kühlere CPUs verteilen
- Turbo verhindern
- CPU-Kerne parken
- CPU-Frequenz reduzieren
- Echtzeitverletzungen diagnostisch melden

Der Scheduler darf thermisch gesperrte CPUs nicht für zusätzliche Last bevorzugen.

Eine Lastverschiebung darf nicht zu einer Überhitzung anderer Zonen führen.

---

## 17. Geräteintegration

Geräte melden:

- eigene Temperaturwerte
- sichere Betriebsgrenzen
- mögliche Cooling States
- erforderliche Reaktionszeiten
- Fehlerzustände
- Abhängigkeiten

Der Device Manager koordiniert notwendige Zustandsänderungen.

Ein Gerät mit eigener Hardwareabschaltung bleibt zusätzlich durch seine Firmware geschützt. NovaOS ersetzt keine zwingenden Hardware-Schutzschaltungen.

---

## 18. Akkus und Laden

Akkus erhalten eigene Thermal Zones.

Bei zu hoher oder zu niedriger Temperatur kann NovaOS:

- Ladeleistung reduzieren
- Laden vorübergehend stoppen
- Entladeleistung begrenzen
- Schnellladen deaktivieren
- Benutzer warnen
- System geordnet herunterfahren

Akku-Grenzwerte haben Vorrang vor Benutzer- und Leistungsprofilen.

Unplausible Akkusensordaten führen zu einem konservativen Ladezustand.

---

## 19. Kritischer Shutdown

Wird ein `CRITICAL` Trip Point erreicht, startet NovaOS einen beschleunigten sicheren Shutdown.

Dabei werden nur notwendige Schritte ausgeführt:

1. neue Benutzeroperationen stoppen,
2. kritische Dateisystemdaten synchronisieren,
3. Geräte in sicheren Zustand versetzen,
4. lokale Diagnose sichern,
5. Plattform ausschalten.

Wird der `EMERGENCY` Trip Point erreicht oder steigt die Temperatur weiter gefährlich an, darf NovaOS ohne vollständigen Shutdownpfad sofort abschalten.

Hardwareschutz hat in diesem Fall Vorrang vor möglichem Datenverlust.

---

## 20. Sensor- und Governor-Ausfall

Bei Ausfall kritischer Sensoren verwendet NovaOS eine Fail-safe-Richtlinie.

Mögliche Reaktionen:

- konservative Leistungsbegrenzung
- Lüfter auf sichere hohe Stufe setzen
- Boost deaktivieren
- nicht notwendige Geräte abschalten
- Warnung und Diagnose
- Shutdown, wenn keine sichere Überwachung mehr möglich ist

Der Ausfall des Userspace-Thermaldienstes darf die kernelnahen Schutzmaßnahmen nicht deaktivieren.

---

## 21. Energie- und Gerätezustände

Thermal Management arbeitet mit den übrigen Power-Subsystemen zusammen:

- CPU Idle
- Device Sleep
- Suspend
- Hibernate
- CPU-Leistungszustände
- Energieprofile

Suspend oder Hibernate darf nicht begonnen werden, wenn ein thermischer Zustand einen schnelleren Shutdown erfordert.

Ein schlafendes Gerät darf nach Wake-up nicht unmittelbar in einen unzulässigen Leistungszustand zurückkehren.

---

## 22. Benutzerprofile

NovaOS kann Profile anbieten:

| Profil | Verhalten |
|---|---|
| Quiet | frühere Leistungsbegrenzung, niedrigere Lüfterdrehzahl soweit sicher |
| Balanced | ausgewogene Regelung |
| Performance | höhere Leistung bis zu sicheren Grenzen |
| Cooling Priority | stärkere aktive Kühlung |
| Battery Care | konservative Akku- und Ladegrenzen |

Benutzerprofile dürfen:

- kritische Trip Points nicht anheben,
- Notabschaltung nicht deaktivieren,
- Sensorfehler nicht ignorieren,
- Hardwaregrenzen nicht überschreiten.

---

## 23. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `THERMAL_RIGHT_READ` | freigegebene Temperaturen lesen |
| `THERMAL_RIGHT_PROFILE` | Benutzerprofil auswählen |
| `THERMAL_RIGHT_COOLING` | Cooling Device kontrolliert ansteuern |
| `THERMAL_RIGHT_TRIP_CONFIG` | nicht kritische Trip Points konfigurieren |
| `THERMAL_RIGHT_DIAGNOSE` | detaillierte Sensor- und Fehlerdaten lesen |
| `THERMAL_RIGHT_ADMIN` | Thermal-Subsystem verwalten |

Kritische und Emergency-Grenzen dürfen nur durch vertrauenswürdige Plattformdaten oder signierte Systemrichtlinien festgelegt werden.

---

## 24. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Sensoren pro Zone
- Thermal Zones
- Cooling Devices
- Abtastraten
- Diagnoseereignisse
- Benutzer- und Treiberanforderungen
- Regelaktualisierungen
- Fehlerwiederholungen

Sehr hohe Abtastraten werden nur verwendet, wenn Hardware und thermische Dynamik dies rechtfertigen.

Ein Sensor darf das System nicht durch eine unkontrollierte Ereignisrate überlasten.

---

## 25. Sicherheit und Datenschutz

Für Thermal Management gelten folgende Regeln:

- kritischer Hardwareschutz kann nicht durch Anwendungen deaktiviert werden,
- Sensor- und Firmwarewerte werden validiert,
- Cooling Devices werden nur über Capabilities gesteuert,
- Temperaturwerte werden nicht als alleinige Authentizitätsquelle verwendet,
- Prozessbezogene Thermaldaten werden unprivilegierten Aufrufern nicht detailliert offengelegt,
- Thermal-Telemetrie bleibt standardmäßig lokal,
- Notabschaltung hat Vorrang vor gewöhnlichen Richtlinien,
- Debug- und Performanceprofile dürfen kritische Grenzwerte nicht umgehen.

---

## 26. Audit und Diagnose

NovaOS erfasst lokal:

- Temperaturverlauf in begrenzter Auflösung
- erreichte Trip Points
- Cooling-State-Änderungen
- Drosselungsdauer
- Lüfterfehler
- Sensorfehler
- thermische Shutdowns
- Profiländerungen
- betroffene Hardwarezonen

Langzeitverläufe werden begrenzt und aggregiert.

Anwendungsaktivitäten werden nicht unnötig mit Temperaturdaten verknüpft.

---

## 27. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_SENSOR_UNAVAILABLE` | Sensor liefert keine Daten |
| `STATUS_SENSOR_INVALID` | Messwert ist unplausibel |
| `STATUS_THERMAL_LIMIT` | thermische Grenze wurde erreicht |
| `STATUS_COOLING_FAILED` | Cooling Device reagiert nicht |
| `STATUS_PROFILE_DENIED` | Profil verletzt Sicherheitsgrenzen |
| `STATUS_TRIP_POINT_LOCKED` | kritischer Grenzwert ist geschützt |
| `STATUS_THERMAL_SHUTDOWN` | Shutdown wurde thermisch ausgelöst |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_LIMIT_EXCEEDED` | Thermal-Ressourcenlimit überschritten |
| `STATUS_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

---

## 28. Alternative Ansätze

### 28.1 Thermische Regelung ausschließlich durch Firmware

**Vorteile:**

- funktioniert unabhängig vom Betriebssystem
- geringer NovaOS-Aufwand

**Nachteile:**

- keine Koordination mit Scheduler und Geräten
- oft grobe oder undurchsichtige Regelung

**Entscheidung:** Firmware-Hardwareschutz bleibt aktiv, wird aber durch NovaOS-Regelung ergänzt.

### 28.2 Thermal Management vollständig im Userspace

**Vorteile:**

- einfach aktualisierbar
- flexible Profile

**Nachteile:**

- Dienstausfall könnte Schutz verzögern
- zu hohe Latenz für kritische Situationen

**Entscheidung:** Profile im Userspace, kritischer Schutz kernelnah.

### 28.3 Nur feste Temperaturgrenzen

**Vorteile:**

- einfache Implementierung

**Nachteile:**

- keine Trendbewertung
- unnötiges Schwanken zwischen Zuständen
- schlechte Anpassung an verschiedene Hardware

**Entscheidung:** NovaOS verwendet Trip Points, Hysterese und Trendbewertung.

---

## 29. Konsequenzen

### Positive Konsequenzen

- zentraler Schutz vor Überhitzung
- koordinierte Lüfter- und Leistungssteuerung
- Integration mit Scheduler und Device Manager
- Unterstützung verschiedener Plattformen und Sensoren
- Fail-safe-Verhalten bei Sensorausfall
- anpassbare, aber sicher begrenzte Benutzerprofile
- lokale Diagnose thermischer Probleme

### Negative Konsequenzen

- plattformspezifische Sensor- und Lüftertreiber erforderlich
- Temperaturwerte können ungenau oder verzögert sein
- Drosselung kann Leistung und Echtzeitverhalten beeinflussen
- komplexe Regelung mehrerer thermisch gekoppelter Zonen

### Risiken

- fehlerhafte Sensorwerte
- zu spät einsetzende Drosselung
- instabile Lüfterregelung
- Überhitzung einer benachbarten Zone
- Datenverlust bei Notabschaltung
- falsche Firmwaregrenzwerte

---

## 30. Implementierungsrichtlinien

- Thermal Management bleibt immer aktiv.
- Kritische Schutzlogik wird kernelnah ausgeführt.
- Sensorwerte werden vor Verwendung validiert.
- Fehlende Sensordaten werden nicht als niedrige Temperatur behandelt.
- Trip Points verwenden Hysterese.
- Temperaturtrend wird zusätzlich zum aktuellen Wert berücksichtigt.
- Aktive Kühlung wird möglichst vor starker Leistungsdrosselung genutzt.
- Kritische Grenzwerte können durch Benutzerprofile nicht angehoben werden.
- Bei fehlender sicherer Überwachung wird konservativ gedrosselt oder heruntergefahren.
- Thermal-Telemetrie bleibt lokal und datensparsam.

---

## 31. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- mehrere Thermal Zones
- mehrere Sensoren pro Zone
- PASSIVE-, ACTIVE-, HOT-, CRITICAL- und EMERGENCY-Trip-Points
- Hysterese
- schnelle Temperaturanstiege
- Lüftersteuerung und Lüfterausfall
- CPU- und GPU-Drosselung
- Geräte- und Akkutemperaturen
- Sensorausfall und unplausible Werte
- Userspace-Dienstausfall
- Profile und geschützte Grenzwerte
- Suspend und Resume
- kritischen Shutdown
- Notabschaltung
- Ereignis- und Ressourcenlimits

---

## 32. Verbindliche Invarianten

1. Thermal Management ist während des Systembetriebs grundsätzlich aktiv.
2. Kritische Schutzmaßnahmen funktionieren ohne Userspace-Dienst.
3. Fehlende oder ungültige Sensordaten werden nicht als sichere niedrige Temperatur behandelt.
4. Kritische und Emergency-Grenzen können nicht durch Anwendungen angehoben werden.
5. Cooling Devices werden ausschließlich kontrolliert angesteuert.
6. Trip Points verwenden definierte Hysterese.
7. Thermischer Schutz hat Vorrang vor Leistung, QoS und Echtzeitpräferenzen.
8. Bei unkontrollierbarer Temperatur darf NovaOS das System notabschalten.
9. Firmware-Schutzmechanismen bleiben zusätzlich aktiv.
10. Thermal-Telemetrie bleibt standardmäßig lokal und enthält keine unnötigen Anwendungsdaten.

---

## 33. Zusammenfassung

NovaOS implementiert einen zentralen Thermal Manager zur Überwachung und Regelung thermischer Hardwarezustände.

Temperatursensoren werden in Thermal Zones zusammengeführt. Trip Points, Hysterese und Trendbewertung steuern Lüfter, CPU- und GPU-Leistung, Device Sleep sowie weitere Cooling Devices.

Kritische Schutzlogik bleibt kernelnah und funktioniert auch bei Ausfall des Userspace-Dienstes. Benutzerprofile dürfen Geräuschentwicklung und Leistung beeinflussen, aber niemals kritische Hardwaregrenzen oder die Notabschaltung außer Kraft setzen.