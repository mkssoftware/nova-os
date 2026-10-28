# NPSPEC-INPUT-ARCH-0003 – Nova Input Event Pipeline

**Status:** Angenommen  
**Bereich:** Input / Architecture / Event Processing  
**System:** NovaOS

## 1. Zweck

Nova Input Event Pipeline definiert die Verarbeitung und Weiterleitung von Eingabeereignissen innerhalb von NovaOS.

Ziel ist eine einheitliche, latenzarme, sichere und deterministische Ereignisverarbeitung für physische und virtuelle Eingabegeräte.

Die Pipeline trennt Hardwareerfassung, Normalisierung, Zustandsverwaltung, Eingabeinterpretation und Ereigniszustellung.

## 2. Architektur

Die Input Event Pipeline besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Event Collector | Übernahme von Eingabeereignissen aus Gerätetreibern |
| Event Normalizer | Umwandlung in ein einheitliches Ereignisformat |
| Event Queue Manager | Verwaltung und zeitliche Ordnung der Ereignisse |
| Input State Updater | Aktualisierung der Eingabezustände |
| Input Event Mapper | Zuordnung physischer zu logischen Eingaben |
| Input Event Router | Bestimmung des zulässigen Ereignisempfängers |
| Event Dispatcher | Zustellung an Systemkomponenten und Anwendungen |

Die Pipeline verarbeitet Eingaben unabhängig von Nova UI und der verwendeten Grafikarchitektur.

## 3. Ereignisverarbeitung

Ein Eingabeereignis durchläuft folgende Phasen:

1. **Capture:** Der Gerätetreiber erfasst die Eingabe.
2. **Normalize:** Das Ereignis wird in das Nova Input Event Format überführt.
3. **Queue:** Das Ereignis wird in die zuständige Warteschlange eingereiht.
4. **State Update:** Der aktuelle Gerätezustand wird aktualisiert.
5. **Mapping:** Die Eingabe wird entsprechend der aktiven Konfiguration interpretiert.
6. **Routing:** Sitzung, Fokus und Berechtigungen bestimmen den Empfänger.
7. **Dispatch:** Das Ereignis wird an den berechtigten Empfänger zugestellt.

Die Verarbeitung MUSS die Reihenfolge zusammengehöriger Ereignisse erhalten.

Die Pipeline DARF Verarbeitungsschritte zusammenfassen, sofern deren beobachtbares Verhalten erhalten bleibt.

## 4. Ereignismodell

Jedes Ereignis besitzt mindestens:

- Ereignistyp.
- Gerätekennung.
- Monotonen Zeitstempel.
- Sequenznummer.
- Ereignisdaten.
- Kennzeichnung der Eingabequelle.

Zusätzliche Informationen können enthalten:

- Modifier-Zustände.
- Geräte-Capabilities.
- Sitzungskennung.
- Koordinaten.
- Vertrauens- und Herkunftsinformationen.

Ereignisse MÜSSEN eindeutig einer registrierten Eingabequelle zugeordnet werden können.

Nicht vertrauenswürdige Quellen DÜRFEN ihre Herkunft nicht selbst als vertrauenswürdig deklarieren.

## 5. Ereigniswarteschlangen

Der Event Queue Manager verwendet begrenzte Warteschlangen.

Dabei gilt:

- Ereignisse eines Geräts MÜSSEN in ihrer logischen Reihenfolge verarbeitet werden.
- Ereignisse verschiedener Geräte DÜRFEN parallel erfasst werden.
- Die Verarbeitung MUSS auch unter hoher Eingabelast funktionsfähig bleiben.
- Warteschlangenüberläufe MÜSSEN erkannt werden.
- Kritische Zustandsänderungen DÜRFEN nicht unkontrolliert verloren gehen.

Bei Überlastung DÜRFEN aufeinanderfolgende Bewegungsereignisse zusammengefasst werden.

Tastendrücke, Tastenfreigaben und andere relevante Zustandsübergänge MÜSSEN erhalten bleiben oder durch eine definierte Zustandssynchronisation wiederhergestellt werden.

Eine globale Totalordnung aller Eingabeereignisse ist nicht erforderlich.

## 6. Input State Synchronization

Der Input State Updater verwaltet den aktuellen Zustand jedes Eingabegeräts.

Dazu gehören:

- Gedrückte Tasten und Schalter.
- Modifier-Zustände.
- Zeigerpositionen.
- Achsenwerte.
- Aktive Touchkontakte.

Zustandsänderungen MÜSSEN in derselben logischen Reihenfolge erfolgen wie die zugehörigen Ereignisse.

Bei Geräteverlust, Warteschlangenüberlauf oder fehlerhaften Ereignisfolgen MUSS eine kontrollierte Zustandssynchronisation erfolgen.

Dauerhaft hängen gebliebene Tasten- oder Touchzustände MÜSSEN vermieden werden.

## 7. Event Mapping und Routing

Die Input Mapping Engine interpretiert normalisierte Ereignisse anhand der aktiven Eingabekonfiguration.

Die anschließende Weiterleitung berücksichtigt:

- Aktive Benutzersitzung.
- Eingabefokus.
- Fenster- und Oberflächenhierarchie.
- Pointer Capture.
- Modale Eingabekontexte.
- Systemreservierte Eingaben.
- Nova.Trust-Berechtigungen.

Die Pipeline MUSS physische Tasteneingaben von erzeugten Texteingaben unterscheiden.

Die Ermittlung des Eingabeempfängers erfolgt anhand eines konsistenten Fokus- und Sitzungszustands.

Anwendungen DÜRFEN keine Ereignisse anderer Anwendungen ohne entsprechende Berechtigung erhalten.

## 8. Latenz und Performance

Die Pipeline ist auf geringe Eingabelatenz ausgelegt.

Festlegungen:

- Interrupt-Kontexte führen ausschließlich zeitkritische Erfassungsarbeiten aus.
- Aufwendige Verarbeitung erfolgt außerhalb des Interrupt-Kontexts.
- Ereignisse werden ohne unnötige Wartezyklen weitergeleitet.
- Bewegungsereignisse DÜRFEN kontrolliert zusammengefasst werden.
- Interaktive Eingaben SOLLEN vor nicht zeitkritischen Hintergrundarbeiten verarbeitet werden.
- Die Pipeline DARF nicht auf die Fertigstellung eines Rendering-Frames warten.

Nova Frame Pacing berücksichtigt den jeweils verfügbaren Eingabezustand bei der Planung der Darstellung.

Die Eingabeverarbeitung bleibt unabhängig von der Bildwiederholrate.

## 9. Sicherheit und Isolation

Die Input Event Pipeline unterliegt Nova.Trust.

Dabei gilt:

- Ereignisse MÜSSEN einer überprüfbaren Eingabequelle zugeordnet sein.
- Virtuelle Eingaben MÜSSEN von physischen Eingaben unterscheidbar bleiben.
- Eingabeinjektion erfordert eine entsprechende Capability.
- Systemreservierte Eingabesequenzen werden vor der regulären Anwendungszustellung behandelt.
- Globale Eingabeüberwachung erfordert eine ausdrückliche Berechtigung.
- Eingabeereignisse DÜRFEN keine unkontrollierten Speicherreferenzen enthalten.

Sicherheitsrelevante Ereignisse MÜSSEN an vertrauenswürdige Systemkomponenten weitergeleitet werden.

## 10. Fehlerbehandlung

- Ungültige Ereignisse werden kontrolliert zurückgewiesen.
- Unbekannte Gerätekennungen lösen keine reguläre Ereigniszustellung aus.
- Warteschlangenüberläufe werden diagnostiziert und durch definierte Wiederherstellungsverfahren behandelt.
- Entfernte Geräte lösen die Bereinigung ihrer aktiven Zustände aus.
- Nicht erreichbare Empfänger DÜRFEN die gesamte Pipeline nicht blockieren.
- Fehler einzelner Anwendungen DÜRFEN keine Eingaben anderer Anwendungen beeinträchtigen.
- Fehlerhafte Zeitstempel und Sequenznummern MÜSSEN erkannt werden.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Input Event Pipeline bereitstellen.
- **INPUT-002:** Eingabeereignisse MÜSSEN vor ihrer Weiterverarbeitung normalisiert werden.
- **INPUT-003:** Jedes Ereignis MUSS eine Gerätekennung und einen monotonen Zeitstempel besitzen.
- **INPUT-004:** Die logische Ereignisreihenfolge pro Gerät MUSS erhalten bleiben.
- **INPUT-005:** Ereigniswarteschlangen MÜSSEN definierte Kapazitätsgrenzen besitzen.
- **INPUT-006:** Warteschlangenüberläufe MÜSSEN erkannt und kontrolliert behandelt werden.
- **INPUT-007:** Zustandsänderungen DÜRFEN nicht unkontrolliert verloren gehen.
- **INPUT-008:** Eingabezustände MÜSSEN mit der Ereignisverarbeitung konsistent bleiben.
- **INPUT-009:** Ereignisempfänger MÜSSEN anhand von Sitzung, Fokus und Berechtigungen bestimmt werden.
- **INPUT-010:** Physische Tasteneingaben und Texteingaben MÜSSEN getrennt behandelt werden.
- **INPUT-011:** Virtuelle Eingaben MÜSSEN eindeutig identifizierbar sein.
- **INPUT-012:** Eingabeinjektion MUSS durch Nova.Trust kontrolliert werden.
- **INPUT-013:** Die Eingabeverarbeitung DARF nicht von der Rendering-Frequenz abhängig sein.
- **INPUT-014:** Die Pipeline MUSS unabhängig von Nova UI und Compositor funktionieren.
- **INPUT-015:** Ereignisverarbeitung MUSS bei identischer Ereignisfolge und identischem Systemzustand reproduzierbare Ergebnisse liefern.
- **INPUT-016:** Die Pipeline MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Input Device Abstraction:** Einheitliche Eingabequellen und Geräteereignisse.
- **Nova Input Device Manager:** Registrierung und Lebenszyklus der Eingabegeräte.
- **Nova Input Mapping Engine:** Interpretation und Zuordnung von Eingaben.
- **Nova Input Focus Manager:** Bestimmung des aktiven Eingabeempfängers.
- **Nova Window Manager:** Fensterhierarchie und Eingabefokus.
- **Nova UI:** Verarbeitung zugestellter Eingabeereignisse.
- **Nova Time:** Monotone Zeitstempel.
- **Nova Frame Pacing:** Zeitnahe Berücksichtigung von Eingaben bei der Darstellung.
- **Nova.Trust:** Berechtigungen und Isolation.

## 13. Ergebnis

NovaOS erhält eine einheitliche und deterministische Verarbeitungskette für sämtliche Eingabeereignisse.

Die Architektur gewährleistet eine klare Trennung zwischen Geräteerfassung, Ereignisverarbeitung, Zustandsverwaltung und sicherer Zustellung.

Durch begrenzte Warteschlangen, kontrollierte Ereigniszusammenfassung und unabhängige Verarbeitung bleibt das Eingabesystem auch bei hoher Last reaktionsfähig.

**Grundsatz:** Jede Eingabe wird so früh wie möglich erfasst, zuverlässig verarbeitet und ausschließlich an den dafür vorgesehenen Empfänger weitergeleitet.