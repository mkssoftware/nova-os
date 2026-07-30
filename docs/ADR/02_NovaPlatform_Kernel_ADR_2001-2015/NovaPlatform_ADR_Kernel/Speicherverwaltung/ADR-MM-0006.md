# ADR-MM-0006: Copy-on-Write (CoW)

* **ADR-ID:** ADR-MM-0006
* **Titel:** Copy-on-Write (CoW)
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Memory Management
* **Gültigkeitsbereich:** Virtual Memory Manager (VMM), Process Manager, Shared Memory, Loader, Page Fault Handler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-MM-0001, ADR-MM-0004, ADR-MM-0005, ADR-KERNEL-0009

---

# 1. Zusammenfassung

NovaOS implementiert **Copy-on-Write (CoW)** als Standardmechanismus zur effizienten gemeinsamen Nutzung physischer Speicherseiten.

Mehrere Prozesse können zunächst dieselben physischen Seiten gemeinsam verwenden. Erst wenn einer der Prozesse auf eine als Copy-on-Write markierte Seite schreibt, wird automatisch eine private Kopie dieser Seite erzeugt. Dadurch werden unnötige Speicherkopien vermieden und sowohl Speicherverbrauch als auch CPU-Zeit reduziert. Copy-on-Write verzögert das Kopieren von Speicher bis zum ersten Schreibzugriff und ist eine grundlegende Optimierung moderner Betriebssysteme.

---

# 2. Kontext

Viele Speicherbereiche sind zunächst identisch.

Beispiele:

* fork()
* Shared Memory
* File Mapping
* Prozessklone
* Bibliotheken
* Speicher-Snapshots

Eine sofortige Kopie sämtlicher Seiten wäre ineffizient.

---

# 3. Problemstellung

Es musste entschieden werden,

* wann physische Seiten kopiert werden,
* wie gemeinsame Seiten verwaltet werden,
* wie Schreibzugriffe erkannt werden,
* wie Page Faults verarbeitet werden,
* und wie Speicher effizient genutzt wird.

---

# 4. Entscheidung

NovaOS verwendet Copy-on-Write als Standardmechanismus für gemeinsam genutzte beschreibbare Seiten.

Dabei gilt:

* Seiten werden zunächst gemeinsam genutzt.
* Alle CoW-Seiten sind zunächst schreibgeschützt.
* Erst ein Schreibzugriff löst eine Kopie aus.
* Lesender Zugriff erzeugt niemals Kopien.

---

# 5. Motivation

Diese Architektur bietet:

* geringeren Speicherverbrauch
* schnelleres Prozessklonen
* weniger Speicherkopien
* höhere Performance
* bessere Skalierbarkeit
* effiziente Shared-Memory-Unterstützung

---

# 6. Designprinzipien

Copy-on-Write basiert auf:

* Lazy Allocation
* Zero-Copy
* Referenzzählung
* Seitenschutz
* deterministischen Page Faults
* Modularität

---

# 7. Architekturübersicht

```text
             Gemeinsame Seite
                  │
            Referenzzähler = 2
                  │
        +---------+---------+
        |                   |
        ▼                   ▼
    Prozess A           Prozess B
    Read Only           Read Only
        │
        ▼
  Schreibzugriff
        │
        ▼
 Page Fault Handler
        │
        ▼
 Neue physische Seite
        │
        ▼
 Prozess A erhält eigene Kopie
```

---

# 8. CoW-Seiten

Eine Copy-on-Write-Seite besitzt:

* physische Seite
* Referenzzähler
* CoW-Flag
* Read-Only-Attribut
* Eigentümerinformationen

---

# 9. Referenzzählung

Jede gemeinsam genutzte Seite besitzt einen atomaren Referenzzähler.

Beim:

* Mapping → erhöhen
* Unmapping → verringern
* Freigeben → Seite entfernen, wenn Referenzzähler Null erreicht

---

# 10. Gemeinsame Nutzung

Solange keine Schreiboperation erfolgt,

* teilen sich alle Prozesse dieselbe physische Seite,
* entstehen keine zusätzlichen Kopien.

Dies spart Speicher und reduziert den Initialisierungsaufwand.

---

# 11. Schreibzugriff

Versucht ein Prozess auf eine CoW-Seite zu schreiben,

tritt ein Page Fault auf.

Der Page Fault Handler übernimmt anschließend:

* neue Seite reservieren
* Daten kopieren
* Seitentabelle aktualisieren
* Referenzzähler anpassen
* Schreibschutz entfernen

Ein Schreibzugriff auf eine gemeinsam genutzte CoW-Seite löst einen Seitenfehler aus; anschließend wird eine private Kopie erzeugt und die Zuordnung aktualisiert.

---

# 12. Lesender Zugriff

Lesende Zugriffe verursachen:

* keine Kopie
* keinen zusätzlichen Speicher
* keine Änderung der Referenzzählung

---

# 13. Page Fault Integration

Der Page Fault Handler erkennt:

* CoW-Seiten
* echte Schutzverletzungen
* ungültige Zugriffe
* Kernelzugriffe

Nur gültige CoW-Fehler führen zur Seitenduplizierung.

---

# 14. Virtuelle Regionen

Copy-on-Write ist vollständig in die Virtual Memory Regions integriert.

Eine Region kann:

* vollständig CoW sein,
* teilweise CoW sein,
* oder CoW deaktivieren.

---

# 15. Shared Memory

Shared Memory kann optional Copy-on-Write verwenden.

Dadurch entstehen:

* private Kopien
* getrennte Änderungen
* unveränderte Originalseiten

---

# 16. Prozessklonen

Beim Klonen eines Prozesses:

* werden keine Daten kopiert,
* alle beschreibbaren Seiten erhalten CoW,
* Referenzzähler werden erhöht,
* Seitentabellen angepasst.

Das schnelle Erzeugen von Kindprozessen nach `fork()` ist einer der klassischen Einsatzzwecke von Copy-on-Write.

---

# 17. Dateiabbildungen

Private File Mappings unterstützen CoW.

Dadurch bleiben:

* Originaldateien unverändert,
* Änderungen lokal,
* Speicher effizient genutzt.

---

# 18. Speicherverwaltung

Der Virtual Memory Manager übernimmt:

* Seitenduplizierung
* Referenzzählung
* Rechteverwaltung
* Seitentabellenaktualisierung

Der Page Allocator liefert neue physische Seiten.

---

# 19. Mehrkernbetrieb

Copy-on-Write unterstützt:

* SMP
* CPU Hotplug
* atomare Referenzzählung
* TLB-Synchronisation

Alle Operationen sind thread-sicher.

---

# 20. TLB-Aktualisierung

Nach einer erfolgreichen Seitenduplizierung erfolgt:

* Aktualisierung der Seitentabelle
* lokaler TLB-Flush
* gegebenenfalls Interprozessor-Interrupt zur TLB-Synchronisation

---

# 21. Sicherheit

Copy-on-Write schützt vor:

* ungewollter Speicherfreigabe
* Datenkorruption
* inkonsistenten Referenzzählern
* Schreibzugriffen auf gemeinsame Seiten
* Race Conditions

---

# 22. Performance

Die Architektur soll ermöglichen:

* minimale Speicherkopien
* schnelle Prozessklone
* geringe Speicherbelegung
* kurze Page-Fault-Latenzen
* hohe Mehrkernskalierung

---

# 23. Debug-Unterstützung

Optional unterstützt NovaOS:

* CoW-Statistiken
* Referenzzähleranalyse
* Page-Fault-Tracing
* Speicherduplikationsanalyse
* Mapping-Historie

---

# 24. Erweiterbarkeit

Die Architektur unterstützt zukünftige Erweiterungen:

* Huge Page CoW
* Hardware Memory Tagging
* verschlüsselte CoW-Seiten
* Snapshot-Unterstützung
* Dateisystem-CoW-Integration
* VM-Snapshots

---

# 25. Auswirkungen

Positive Auswirkungen:

* erhebliche Speichereinsparung
* schnelle Prozesserzeugung
* weniger Speicherkopien
* hohe Performance
* effiziente Shared-Memory-Nutzung
* moderne Speicherverwaltung

Negative Auswirkungen:

* komplexerer Page-Fault-Handler
* zusätzlicher Verwaltungsaufwand
* Schreibzugriffe verursachen zusätzliche Latenz

---

# 26. Verworfene Alternativen

## Sofortiges Kopieren

Vorteile:

* einfache Implementierung

Nachteile:

* hoher Speicherverbrauch
* langsame Prozessklone
* unnötige Kopieroperationen

---

## Ausschließlich Shared Memory

Vorteile:

* einfache Verwaltung

Nachteile:

* keine privaten Änderungen
* geringe Flexibilität

---

## Vollständig private Seiten

Vorteile:

* einfache Rechteverwaltung

Nachteile:

* extrem hoher Speicherbedarf
* schlechte Skalierbarkeit

---

# 27. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* Copy-on-Write ausschließlich über den Virtual Memory Manager verwalten,
* Referenzzähler atomar aktualisieren,
* Seitenduplizierungen ausschließlich im Page-Fault-Handler durchführen,
* keine direkten Kopien gemeinsam genutzter Seiten erzeugen,
* TLB-Aktualisierungen korrekt durchführen.

---

# 28. Teststrategie

Copy-on-Write wird geprüft durch:

* Unit-Tests
* Prozessklon-Tests
* Shared-Memory-Tests
* Page-Fault-Tests
* Mehrkerntests
* Referenzzählungstests
* Langzeittests
* Performancebenchmarks

---

# 29. Konsequenzen

Copy-on-Write wird zum Standardmechanismus für gemeinsam genutzte beschreibbare Speicherseiten innerhalb von NovaOS.

Alle zukünftigen Komponenten – insbesondere Prozessverwaltung, Shared Memory, File Mapping und Virtual Memory Manager – nutzen Copy-on-Write, um Speicherverbrauch und Kopieraufwand zu minimieren. Dadurch entsteht eine moderne, skalierbare und speichereffiziente Architektur, die sowohl Desktop- als auch Server- und Virtualisierungsszenarien optimal unterstützt.

---

# 30. Zusammenfassung

NovaOS implementiert **Copy-on-Write (CoW)** als grundlegenden Optimierungsmechanismus der virtuellen Speicherverwaltung. Gemeinsam genutzte Seiten werden erst bei einem tatsächlichen Schreibzugriff dupliziert, wodurch Speicherverbrauch und Kopieraufwand erheblich reduziert werden. Durch die enge Integration mit Virtual Memory Regions, Page Fault Handler, Shared Memory und dem Virtual Memory Manager entsteht eine leistungsfähige, sichere und skalierbare Architektur für moderne Betriebssysteme.
