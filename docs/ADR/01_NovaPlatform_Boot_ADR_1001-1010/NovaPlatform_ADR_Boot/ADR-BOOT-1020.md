# ADR-BOOT-1020: Bootloader und Kernel teilen keine internen Strukturen ohne versioniertes ABI (Teil 1)

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1020
* **Titel:** Bootloader und Kernel teilen keine internen Strukturen ohne versioniertes ABI
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture / ABI
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** Stage-2, Boot Module Manager, Kernel Loader, Kernel und alle zukünftigen Bootmodule

---

# Referenzen

* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* ADR-BOOT-1013 – Stage-1 bleibt minimal
* ADR-BOOT-1019 – Signierte Boot-Komponenten
* NPSPEC-BOOTHANDOFF-0001
* NPSPEC-KERNELENTRY-0001

---

# 1. Kontext

Der Bootloader übergibt beim Start Informationen an den Kernel.

Diese Informationen umfassen beispielsweise:

* Speicherlayout
* Framebuffer
* Bootparameter
* CPU-Informationen
* ACPI
* EFI-Informationen
* BootFS
* Modulinformationen

Damit Bootloader und Kernel zusammenarbeiten können, benötigen sie eine gemeinsame Datenschnittstelle.

Bewährte Bootprotokolle verwenden hierfür klar definierte, versionierte Strukturen, damit Bootloader und Kernel unabhängig weiterentwickelt werden können.

---

# 2. Problemstellung

Ohne definierte Schnittstelle könnten Bootloader und Kernel dieselben internen C-Strukturen verwenden.

Dies führt zu Problemen:

* Änderungen an einer Struktur beeinflussen beide Komponenten.
* Neue Kernel können mit älteren Bootloadern inkompatibel werden.
* Alte Kernel können neue Bootloader nicht mehr verstehen.
* Wartbarkeit und Testbarkeit verschlechtern sich erheblich.

---

# 3. Entscheidung

Bootloader und Kernel teilen **niemals interne Datenstrukturen**.

Der Datenaustausch erfolgt ausschließlich über ein **versioniertes Boot-ABI**.

Dieses ABI ist die einzige offizielle Kommunikationsschnittstelle zwischen beiden Komponenten.

---

# 4. Ziele

Diese Entscheidung verfolgt folgende Ziele:

* klare Architekturgrenzen
* lose Kopplung
* Versionskompatibilität
* Erweiterbarkeit
* Testbarkeit
* langfristige Wartbarkeit

---

# 5. Grundprinzip

Die Kommunikation erfolgt ausschließlich über definierte ABI-Strukturen.

```text
Bootloader

↓

Nova Boot ABI

↓

Kernel
```

Interne Datenstrukturen bleiben privat.

---

# 6. Definition des Boot-ABI

Das Boot-ABI beschreibt ausschließlich:

* Datenformate
* Feldgrößen
* Datentypen
* Ausrichtung
* Versionsinformationen
* Gültigkeitsregeln

Nicht Bestandteil des ABI sind interne Implementierungsdetails.

---

# 7. Private Strukturen

Interne Bootloaderstrukturen bleiben vollständig gekapselt.

Beispiele:

* Speichermanager
* Modulverwaltung
* Dateisystemobjekte
* Parser
* Cache

Diese dürfen niemals direkt an den Kernel übergeben werden.

---

# 8. Private Kernelstrukturen

Ebenso bleiben Kernelstrukturen privat.

Beispiele:

* Scheduler
* Speicherverwaltung
* Objektverwaltung
* Prozesslisten
* Gerätelisten

Der Bootloader kennt diese Strukturen nicht.

---

# 9. Keine gemeinsamen Header

Bootloader und Kernel dürfen keine gemeinsamen internen Headerdateien verwenden.

Erlaubt sind ausschließlich Header, welche das öffentliche Boot-ABI definieren.

---

# 10. Öffentliche ABI-Dateien

Beispiel:

```text
include/

    nova_boot_abi.h
```

Diese Datei beschreibt ausschließlich das öffentliche Übergabeformat.

---

# 11. Versionierung

Jede ABI-Version besitzt mindestens:

* Major-Version
* Minor-Version

Optional:

* Patch-Version

Dieses Vorgehen entspricht bewährten Prinzipien versionierter ABI- und Firmware-Schnittstellen.

---

# 12. Major-Version

Eine neue Major-Version bedeutet:

* inkompatible Änderungen
* Strukturänderungen
* Feldentfernungen
* neue Pflichtfelder

---

# 13. Minor-Version

Eine Minor-Version bedeutet:

* abwärtskompatible Erweiterungen
* optionale Felder
* zusätzliche Informationen

---

# 14. Patch-Version

Patch-Versionen ändern ausschließlich:

* Dokumentation
* Fehlerkorrekturen
* Implementierungsdetails

Das ABI bleibt unverändert.

---

# 15. ABI-Header

Jede Übergabestruktur beginnt mit einem gemeinsamen Header.

Beispiel:

```c
typedef struct
{
    uint32_t magic;
    uint16_t major;
    uint16_t minor;
    uint32_t size;
} nova_boot_header_t;
```

---

# 16. Magic Number

Jede Struktur besitzt eine eindeutige Magic Number.

Dadurch erkennt der Kernel beschädigte oder falsche Übergabestrukturen sofort.

---

# 17. Größenfeld

Jede Struktur enthält ihre eigene Größe.

Dadurch können spätere Erweiterungen erkannt werden.

---

# 18. Versionsprüfung

Vor dem Lesen einer Struktur überprüft der Kernel:

* Magic Number
* Version
* Größe

Erst danach erfolgt die Verarbeitung.

---

# 19. Erweiterbarkeit

Neue Felder werden ausschließlich am Ende einer Struktur ergänzt.

Bereits vorhandene Felder dürfen ihre Bedeutung nicht ändern.

Dieses Prinzip ähnelt etablierten ABI-Richtlinien, bei denen bestehende Schnittstellen erhalten und nur kompatibel erweitert werden.

---

# 20. Keine impliziten Annahmen

Der Kernel darf niemals annehmen:

* feste Strukturgröße
* feste Feldanzahl
* unveränderte Reihenfolge außerhalb der ABI-Spezifikation

Er verarbeitet ausschließlich das dokumentierte ABI.

---

# 21. Optionalfelder

Neue optionale Felder werden nur ausgewertet, wenn:

* die Struktur groß genug ist,
* und die ABI-Version dies erlaubt.

---

# 22. Rückwärtskompatibilität

Ein neuer Kernel soll ältere ABI-Versionen unterstützen können, sofern dies innerhalb derselben Major-Version vorgesehen ist.

---

# 23. Vorwärtskompatibilität

Ein älterer Kernel darf unbekannte optionale Felder ignorieren.

Dadurch bleibt die Kommunikation innerhalb einer Major-Version stabil.

---

# 24. Boot-Handoff

Das Boot-Handoff besteht ausschließlich aus ABI-Strukturen.

Keine internen Zeiger oder Implementierungsobjekte dürfen Bestandteil der Übergabe sein.

---

# 25. Zeiger

Rohe interne Zeiger auf Bootloaderobjekte dürfen niemals an den Kernel übergeben werden.

Übergeben werden ausschließlich:

* Werte
* Handles
* Adressen gemäß ABI
* standardisierte Beschreibungsstrukturen

---

# 26. Handles

Falls Handles verwendet werden, müssen diese vollständig durch das ABI definiert sein.

Ihre interne Bedeutung bleibt ausschließlich dem Bootloader vorbehalten.

---

# 27. Architekturunabhängigkeit

Das Boot-ABI darf keine Annahmen über interne Compilerlayouts treffen.

Ausrichtung und Datentypen müssen eindeutig spezifiziert werden.

---

# 28. Compilerunabhängigkeit

Das ABI muss unabhängig funktionieren von:

* GCC
* Clang
* zukünftigen Compilern

---

# 29. Sprachunabhängigkeit

Das Boot-ABI darf nicht an C gebunden sein.

Es muss ebenso aus:

* Rust
* C++
* Zig
* Assembly

verwendbar sein.

---

# 30. Dokumentationspflicht

Jede ABI-Version wird vollständig dokumentiert.

Die Dokumentation umfasst:

* Felddefinitionen
* Datentypen
* Versionen
* Erweiterungsregeln
* Kompatibilitätsregeln

---

# 31. Validierung

Vor der Nutzung einer ABI-Struktur erfolgt immer eine vollständige Plausibilitätsprüfung.

---

# 32. Fehlerbehandlung

Ungültige ABI-Versionen führen niemals zu undefiniertem Verhalten.

Stattdessen erfolgt:

* Fehlermeldung
* Recovery
* Abbruch des Bootvorgangs

gemäß den Sicherheitsrichtlinien.

---

# 33. Testbarkeit

Das Boot-ABI kann unabhängig vom Bootloader getestet werden.

Ebenso kann der Kernel mit simulierten ABI-Strukturen getestet werden.

---

# 34. Modularität

Das ABI bildet eine eigenständige Architekturschicht.

Es gehört weder vollständig zum Bootloader noch vollständig zum Kernel.

---

# 35. Qualitätsanforderungen

Das Boot-ABI muss:

* stabil
* eindeutig
* dokumentiert
* versioniert
* erweiterbar
* validierbar

sein.

---

# 36. Zusammenfassung Teil 1

NovaOS trennt Bootloader und Kernel konsequent durch ein versioniertes Boot-ABI. Beide Komponenten besitzen ihre eigenen internen Datenstrukturen und tauschen ausschließlich standardisierte, dokumentierte und versionierte Übergabestrukturen aus. Dadurch können Bootloader und Kernel unabhängig weiterentwickelt werden, ohne sich gegenseitig durch interne Implementierungsänderungen zu beeinflussen. Dieses Vorgehen verbessert Wartbarkeit, Kompatibilität und langfristige Evolvierbarkeit der Bootarchitektur.



# ADR-BOOT-1020: Bootloader und Kernel teilen keine internen Strukturen ohne versioniertes ABI (Teil 2)

---

# 37. ABI-Bibliothek

Das Boot-ABI wird als eigenständige Bibliothek definiert.

Diese Bibliothek enthält ausschließlich:

* öffentliche Datentypen
* Konstanten
* Versionsinformationen
* Validierungsregeln

Sie enthält **keine** Implementierungslogik.

---

# 38. Schichtenmodell

Die Architektur folgt einer klaren Schichtung.

```text
Stage-2

↓

Nova Boot ABI

↓

Kernel Loader

↓

Kernel
```

Das ABI bildet die einzige offizielle Kommunikationsschicht.

---

# 39. Dateneigentum

Jede Struktur besitzt einen eindeutigen Eigentümer.

Beispiele:

* Bootloader besitzt Bootloaderstrukturen.
* Kernel besitzt Kernelstrukturen.
* Das Boot-ABI besitzt ausschließlich Übergabestrukturen.

---

# 40. Lebensdauer

ABI-Strukturen existieren ausschließlich für die Dauer der Übergabe.

Nach erfolgreicher Initialisierung übernimmt der Kernel die benötigten Informationen in eigene interne Datenstrukturen.

---

# 41. Datenkopie

Der Kernel kopiert sämtliche benötigten Informationen aus dem Boot-ABI.

Danach wird nicht mehr auf die ursprünglichen ABI-Strukturen zugegriffen.

Dadurch werden versteckte Abhängigkeiten vermieden.

---

# 42. Speicherverwaltung

Der Kernel darf den vom Bootloader verwendeten Speicher freigeben, sobald sämtliche ABI-Daten übernommen wurden.

---

# 43. Zeiger innerhalb des ABI

Zeiger innerhalb der ABI-Strukturen sind nur zulässig, wenn ihre Bedeutung vollständig dokumentiert ist.

Beispiele:

* Framebufferadresse
* Speicherkarten
* ACPI-Tabellen
* Device Tree

Alle Adressen beziehen sich ausschließlich auf definierte Speicherbereiche.

---

# 44. Keine internen Objektzeiger

Folgende Übergaben sind unzulässig:

* Bootloaderobjekte
* Listenköpfe
* C++-Objekte
* virtuelle Tabellen
* Referenzzähler
* interne Handles ohne Spezifikation

---

# 45. Serialisierung

Alle ABI-Strukturen müssen vollständig serialisierbar sein.

Sie dürfen keine compilerabhängigen Konstrukte enthalten.

---

# 46. Endianness

Das ABI definiert eindeutig die Byte-Reihenfolge.

Alle unterstützten Architekturen interpretieren die Daten identisch.

---

# 47. Datentypen

Das ABI verwendet ausschließlich fest definierte Datentypen.

Beispiele:

```text
uint8_t

uint16_t

uint32_t

uint64_t
```

Compilerabhängige Typen wie `int`, `long` oder `size_t` werden vermieden.

---

# 48. Alignment

Für jede Struktur ist die Speicheranordnung eindeutig definiert.

Nicht dokumentiertes Padding darf nicht Bestandteil des ABI sein.

---

# 49. Erweiterungsblöcke

Neue Funktionen werden über Erweiterungsblöcke eingeführt.

```text
Boot Header

↓

Standardfelder

↓

Extension Block

↓

Extension Block

↓

Extension Block
```

Dadurch bleiben ältere Kernel kompatibel. Dieses Erweiterungsprinzip ähnelt etablierten, versionierten Bootprotokollen, bei denen neue Felder ergänzt werden, ohne bestehende Strukturen inkompatibel zu verändern.

---

# 50. Unbekannte Erweiterungen

Unbekannte Erweiterungsblöcke werden übersprungen.

Der Kernel verarbeitet ausschließlich unterstützte Erweiterungen.

---

# 51. Feature Flags

Neue ABI-Funktionen werden zusätzlich über Feature Flags beschrieben.

Dadurch kann der Kernel gezielt erkennen, welche Funktionen verfügbar sind.

---

# 52. Kompatibilitätsmatrix

Für jede ABI-Version wird dokumentiert:

* unterstützte Kernelversionen
* unterstützte Bootloaderversionen
* bekannte Einschränkungen
* entfernte Funktionen

---

# 53. Versionsverhandlung

Bootloader und Kernel vergleichen zunächst ihre unterstützten ABI-Versionen.

Erst danach beginnt die eigentliche Initialisierung.

---

# 54. Inkompatible Versionen

Ist keine gemeinsame Major-Version vorhanden, wird der Bootvorgang kontrolliert beendet.

Der Benutzer erhält eine eindeutige Diagnose.

---

# 55. Fehlercodes

Das ABI definiert standardisierte Fehlercodes.

Beispiele:

* ungültige Magic Number
* inkompatible Version
* beschädigte Struktur
* ungültige Größe
* fehlende Pflichtdaten

---

# 56. Logging

Alle ABI-Prüfungen können protokolliert werden.

Dies erleichtert:

* Fehlersuche
* Regressionstests
* Support
* Debugging

---

# 57. Debugmodus

Ein Debugmodus darf zusätzliche ABI-Informationen ausgeben.

Beispielsweise:

* Strukturgrößen
* Versionsnummern
* Feature Flags
* Erweiterungsblöcke

---

# 58. Testbarkeit

Das ABI wird unabhängig getestet.

Hierzu gehören:

* Unit-Tests
* Kompatibilitätstests
* Negativtests
* Fuzzing
* Versionsmigrationen

---

# 59. Regressionstests

Vor jeder Veröffentlichung müssen geprüft werden:

* alte Bootloader mit neuen Kerneln
* neue Bootloader mit älteren Kerneln
* identische ABI-Versionen
* inkompatible Versionen

---

# 60. Dokumentation

Jede ABI-Version erhält eine vollständige Spezifikation.

Diese dokumentiert:

* alle Felder
* Datentypen
* Größen
* Erweiterungsregeln
* Beispiele
* Fehlerfälle

---

# 61. Build-System

Das Build-System stellt sicher, dass Bootloader und Kernel dieselbe ABI-Spezifikation verwenden.

---

# 62. Continuous Integration

Die CI überprüft automatisch:

* ABI-Kompatibilität
* Größenänderungen
* Feldänderungen
* Dokumentationskonsistenz

---

# 63. ABI-Stabilität

Innerhalb einer Major-Version bleibt das ABI stabil.

Inkompatible Änderungen sind ausschließlich in einer neuen Major-Version zulässig.

Dieses Vorgehen entspricht etablierten Richtlinien für stabile ABI-Schnittstellen.

---

# 64. Modularität

Das ABI darf unabhängig vom Bootloader weiterentwickelt werden.

Ebenso darf sich der Kernel intern verändern, ohne das ABI anzupassen.

---

# 65. Sicherheit

Alle ABI-Daten werden vor ihrer Verwendung validiert.

Es wird niemals davon ausgegangen, dass Bootdaten automatisch vertrauenswürdig sind.

---

# 66. Recovery

Bei einer inkompatiblen ABI-Version kann automatisch das Recovery-Modul gestartet werden.

Dadurch lassen sich Bootloader oder Kernel aktualisieren.

---

# 67. Zukunftssicherheit

Das ABI muss zukünftige Erweiterungen ermöglichen.

Beispiele:

* neue Speicherinformationen
* neue Grafikdaten
* Virtualisierung
* Sicherheitsinformationen
* neue Architekturen

---

# 68. Plattformunabhängigkeit

Das ABI wird unabhängig entwickelt von:

* BIOS
* UEFI
* x86
* ARM64
* RISC-V

---

# 69. Sprachunabhängigkeit

Die ABI-Spezifikation bleibt unabhängig von der Programmiersprache.

Sie kann von:

* C
* Rust
* Zig
* C++
* Assembly

gleichermaßen implementiert werden.

---

# 70. Qualitätsanforderungen

Das Boot-ABI muss:

* eindeutig
* dokumentiert
* validierbar
* versioniert
* deterministisch
* langfristig stabil

sein.

---

# 71. Alternativen

## Gemeinsame interne Header

**Abgelehnt**

Nachteile:

* starke Kopplung
* geringe Wartbarkeit
* Versionsprobleme

---

## Gemeinsame interne C-Strukturen

**Abgelehnt**

Nachteile:

* Compilerabhängigkeit
* ABI-Brüche
* versteckte Abhängigkeiten

---

## Nicht versioniertes ABI

**Abgelehnt**

Nachteile:

* keine kontrollierte Evolution
* schwierige Migration
* hohe Fehleranfälligkeit

---

## Versioniertes Boot-ABI

**Angenommen**

Es ermöglicht eine langfristig stabile und klar dokumentierte Schnittstelle zwischen Bootloader und Kernel.

---

# 72. Implementierungsstrategie

## Phase 1

* ABI-Header
* Versionsinformationen
* Magic Number

## Phase 2

* Speicherinformationen
* Framebuffer
* Bootparameter

## Phase 3

* Erweiterungsblöcke
* Feature Flags
* Validierung

## Phase 4

* Kompatibilitätstests
* CI-Prüfungen
* Dokumentation

## Phase 5

* zukünftige ABI-Erweiterungen
* neue Architekturen
* zusätzliche Bootmodule

---

# 73. Beispielarchitektur

```text
Stage-2

↓

Nova Boot ABI

↓

Kernel Loader

↓

Kernel

↓

Interne Kernelstrukturen
```

---

# 74. Konformitätskriterien

Eine Implementierung erfüllt dieses ADR, wenn:

* Bootloader und Kernel keine gemeinsamen internen Datenstrukturen verwenden,
* sämtliche Übergaben ausschließlich über das Nova Boot-ABI erfolgen,
* jede ABI-Struktur versioniert ist,
* Magic Number und Größenfeld validiert werden,
* Erweiterungen ausschließlich kompatibel erfolgen,
* interne Zeiger nicht Bestandteil des ABI sind,
* und Kernel wie Bootloader unabhängig voneinander weiterentwickelt werden können.

---

# 75. Entscheidungsergebnis

NovaOS führt ein strikt versioniertes Boot-ABI als einzige offizielle Kommunikationsschnittstelle zwischen Bootloader und Kernel ein. Beide Komponenten besitzen vollständig getrennte interne Datenstrukturen und kommunizieren ausschließlich über klar definierte, dokumentierte und validierte Übergabestrukturen.

---

# 76. Zusammenfassung

Bootloader und Kernel werden architektonisch konsequent voneinander entkoppelt. Interne Implementierungsdetails bleiben auf beiden Seiten privat und werden niemals direkt ausgetauscht. Stattdessen erfolgt jede Kommunikation über ein stabiles, versioniertes Boot-ABI mit klar definierten Datentypen, Versionsregeln und Erweiterungsmechanismen.

Diese Architektur ermöglicht die unabhängige Weiterentwicklung beider Komponenten, verbessert die Wartbarkeit, vereinfacht Tests und verhindert ABI-Brüche durch interne Änderungen. Gleichzeitig schafft sie eine belastbare Grundlage für zukünftige Erweiterungen der NovaOS-Bootarchitektur.
