# ADR-KERNEL-0014: System Call ABI

* **ADR-ID:** ADR-KERNEL-0014
* **Titel:** System Call ABI
* **Status:** Akzeptiert
* **Version:** 1.0.0
* **Datum:** 2026-07-29
* **Autor:** NovaOS Architekturgruppe
* **Kategorie:** Kernel-Architektur
* **Gültigkeitsbereich:** Kernel, HAL, Loader, Runtime, Standardbibliotheken, Compiler
* **Ersetzt:** –
* **Ersetzt durch:** –
* **Abhängigkeiten:** ADR-KERNEL-0012, ADR-KERNEL-0013, NPSPEC-ABI-0001

---

# 1. Zusammenfassung

NovaOS definiert eine **plattformübergreifende System Call ABI (Application Binary Interface)** als einzige offizielle Schnittstelle zwischen Benutzermodus und Kernelmodus.

Die ABI beschreibt die binären Konventionen für Systemaufrufe, darunter Registerbelegung, Parameterübergabe, Rückgabewerte, Fehlerbehandlung, CPU-Zustand und Versionskompatibilität. Die konkrete Implementierung der CPU-Instruktionen (`syscall`, `sysenter`, `svc`, `ecall` usw.) wird ausschließlich durch die Hardware Abstraction Layer (HAL) gekapselt, sodass die logische ABI für Anwendungen auf allen unterstützten Architekturen identisch bleibt. Eine stabile Systemaufruf-ABI ist eine zentrale Voraussetzung für langfristige Binärkompatibilität.

---

# 2. Kontext

Benutzerprogramme dürfen keine Kernelstrukturen direkt verwenden.

Alle privilegierten Operationen erfolgen über:

* Systemaufrufe
* Capability-Prüfungen
* Handle-System
* Kernelobjekte

Hierfür wird eine stabile ABI benötigt.

---

# 3. Problemstellung

Es musste entschieden werden,

* wie Anwendungen Kernelaufrufe durchführen,
* wie Parameter übergeben werden,
* wie Ergebnisse zurückgegeben werden,
* wie Mehrarchitektur-Unterstützung erreicht wird,
* und wie zukünftige Erweiterungen möglich bleiben.

---

# 4. Entscheidung

NovaOS definiert eine einheitliche System Call ABI.

Dabei gilt:

* alle Kernelaufrufe erfolgen ausschließlich über diese ABI,
* die ABI ist architekturunabhängig spezifiziert,
* architekturspezifische Unterschiede werden vollständig durch die HAL gekapselt,
* Binärkompatibilität besitzt höchste Priorität.

---

# 5. Motivation

Diese Architektur bietet:

* stabile Binärschnittstellen
* Mehrarchitektur-Unterstützung
* einfache Portierung
* langfristige Kompatibilität
* geringe Kopplung zwischen Anwendungen und Kernel
* klar definierte Kernelgrenzen

---

# 6. Designprinzipien

Die System Call ABI basiert auf:

* Stabilität
* Rückwärtskompatibilität
* Architekturunabhängigkeit
* Erweiterbarkeit
* deterministischem Verhalten
* Performance

---

# 7. Architekturübersicht

```text
            Benutzerprogramm

                    │

          Runtime / Standardbibliothek

                    │

             NovaOS System Call ABI

                    │

     HAL (architekturspezifischer Einstieg)

                    │

               Kernel Dispatcher

                    │

              Kernel Subsysteme
```

---

# 8. ABI-Verantwortung

Die ABI definiert:

* Aufrufkonvention
* Registerbelegung
* Parameterübergabe
* Rückgabewerte
* Fehlercodes
* CPU-Zustand
* Versionsregeln

Nicht Bestandteil sind die eigentlichen Kernelimplementierungen.

---

# 9. Architekturunabhängigkeit

Die logische ABI bleibt identisch für:

* x86
* x86_64
* ARM64
* RISC-V

Lediglich der CPU-Einstiegspunkt unterscheidet sich je Architektur.

---

# 10. Einstieg in den Kernel

Die HAL verwendet je nach Prozessorarchitektur die geeignete CPU-Instruktion.

Beispiele:

* `SYSCALL`
* `SYSENTER`
* `SVC`
* `ECALL`

Diese Unterschiede sind für Anwendungen vollständig transparent.

---

# 11. System Call Nummern

Jeder Systemaufruf besitzt:

* eine eindeutige System Call ID
* einen stabilen Namen
* eine feste Parameterdefinition

Einmal veröffentlichte IDs dürfen nicht wiederverwendet werden.

---

# 12. Parameterübergabe

Die ABI definiert eindeutig:

* Reihenfolge
* Datentypen
* Alignment
* Registerbelegung
* Speicherübergaben

Dadurch können Programme jeder Programmiersprache dieselbe Kernel-ABI verwenden.

---

# 13. Rückgabewerte

Ein Systemaufruf liefert:

* Erfolgswert
* Fehlercode
* optionale Ausgabedaten

Mehrdeutige Rückgabewerte sind unzulässig.

---

# 14. Fehlerbehandlung

Fehler werden standardisiert zurückgegeben.

Beispiele:

* Invalid Handle
* Invalid Capability
* Permission Denied
* Invalid Parameter
* Object Not Found
* Timeout
* Out of Memory

---

# 15. CPU-Zustand

Nach Rückkehr aus einem Systemaufruf gilt:

* definierte Registerinhalte
* definierte Flags
* definierter Stackzustand
* definierte Alignment-Regeln

Nicht definierte Seiteneffekte sind unzulässig.

---

# 16. Thread-Kontext

Ein Systemaufruf darf:

* blockieren,
* unterbrochen werden,
* den Scheduler aktivieren,
* einen Threadwechsel auslösen.

Diese Vorgänge bleiben vollständig transparent.

---

# 17. Capability-Prüfung

Vor jeder privilegierten Operation erfolgt:

1. Handle-Auflösung
2. Capability-Prüfung
3. Parameterprüfung
4. Objekttypprüfung
5. Kerneloperation

---

# 18. Speicherübergabe

Benutzerspeicher wird niemals direkt verwendet.

Der Kernel validiert:

* Adressen
* Speicherbereiche
* Zugriffsrechte
* Ausrichtung

Ungültige Speicherbereiche führen zur sofortigen Fehlerbehandlung.

---

# 19. Pointer-Sicherheit

Alle Zeiger aus dem Benutzermodus gelten zunächst als **nicht vertrauenswürdig**.

Vor der Verwendung müssen sie vollständig geprüft werden.

---

# 20. ABI-Versionierung

Die ABI besitzt eine eigene Versionsnummer.

Neue Funktionen werden ausschließlich ergänzt.

Bereits veröffentlichte Systemaufrufe bleiben dauerhaft kompatibel. Eine stabile ABI verhindert, dass bestehende Anwendungen bei Kernelaktualisierungen neu kompiliert werden müssen.

---

# 21. Erweiterbarkeit

Neue Systemaufrufe erhalten:

* neue IDs
* eigene Dokumentation
* vollständige Abwärtskompatibilität

Bestehende Aufrufe werden nicht verändert.

---

# 22. Sprachunabhängigkeit

Die ABI ist unabhängig von:

* C
* Rust
* NovaLang
* C++
* Zig
* Python
* anderen Programmiersprachen

Alle Sprachen können dieselbe ABI direkt oder über Laufzeitbibliotheken verwenden.

---

# 23. Performance

Die ABI soll:

* minimale Registerkopien
* geringe Wechselkosten
* schnelle Dispatcher
* kurze Kernelpfade

ermöglichen.

---

# 24. Sicherheit

Die System Call ABI schützt vor:

* ungültigen Handles
* manipulierten Parametern
* Rechteeskalation
* Speicherverletzungen
* Kernelzugriffen außerhalb definierter Schnittstellen

---

# 25. Debug-Unterstützung

Optional unterstützt NovaOS:

* System-Call-Tracing
* Performance-Zähler
* Argumentprotokolle
* Fehlerstatistiken
* ABI-Kompatibilitätsprüfungen

---

# 26. Mehrarchitektur-Unterstützung

Für jede unterstützte Architektur existiert:

* eigener Einstiegscode
* eigene Registerbelegung
* eigene CPU-Instruktionen

Die logische ABI bleibt dabei unverändert.

---

# 27. Auswirkungen

Positive Auswirkungen:

* stabile Binärschnittstelle
* einfache Compilerintegration
* hohe Portabilität
* klare Kernelgrenze
* langfristige Kompatibilität
* einfache Sprachunterstützung

Negative Auswirkungen:

* ABI muss dauerhaft gepflegt werden
* Änderungen sind nur eingeschränkt möglich
* höhere Anforderungen an Dokumentation und Tests

---

# 28. Verworfene Alternativen

## Architekturabhängige ABI

Vorteile:

* maximale Hardwareoptimierung

Nachteile:

* schwierige Portierung
* unterschiedliche Compiler
* hoher Wartungsaufwand

---

## Direkte Kernelbibliotheken

Vorteile:

* einfache Implementierung

Nachteile:

* keine stabile Binärschnittstelle
* starke Kopplung zwischen Anwendungen und Kernel

---

## ABI ohne Versionsverwaltung

Vorteile:

* geringerer Verwaltungsaufwand

Nachteile:

* häufige Inkompatibilitäten
* erschwerte Langzeitunterstützung

---

# 29. Implementierungsrichtlinien

Alle Kernelkomponenten müssen:

* ausschließlich die definierte System Call ABI verwenden,
* Parameter vollständig validieren,
* Fehler standardisiert zurückgeben,
* ABI-Kompatibilität wahren,
* keine architekturspezifischen Besonderheiten außerhalb der HAL einführen.

---

# 30. Teststrategie

Die System Call ABI wird geprüft durch:

* Unit-Tests
* ABI-Kompatibilitätstests
* Sprachübergreifende Tests
* Performancebenchmarks
* Sicherheitstests
* Lasttests
* Langzeittests

---

# 31. Konsequenzen

Die System Call ABI bildet die verbindliche Kommunikationsschicht zwischen Anwendungen und Kernel.

Alle zukünftigen Systemaufrufe, Laufzeitbibliotheken, Compiler und Programmiersprachen müssen diese ABI verwenden. Änderungen an bestehenden ABI-Regeln sind grundsätzlich unzulässig; Erweiterungen erfolgen ausschließlich durch neue Systemaufrufe oder optionale Funktionen. Dadurch bleibt die Binärkompatibilität über mehrere Kernelgenerationen hinweg erhalten.

---

# 32. Zusammenfassung

NovaOS definiert eine **plattformübergreifende System Call ABI** als stabile und langfristig kompatible Binärschnittstelle zwischen Benutzer- und Kernelmodus. Die ABI legt Aufrufkonventionen, Parameterübergabe, Rückgabewerte, Fehlerbehandlung und Versionsregeln fest, während architekturspezifische Unterschiede vollständig durch die HAL abstrahiert werden. Dadurch entsteht eine sichere, leistungsfähige und zukunftssichere Grundlage für sämtliche Systemaufrufe innerhalb von NovaOS.
