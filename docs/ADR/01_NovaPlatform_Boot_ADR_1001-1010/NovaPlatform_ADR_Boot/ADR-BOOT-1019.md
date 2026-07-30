# ADR-BOOT-1019: Signierte Boot-Komponenten (Teil 1)

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1019
* **Titel:** Signierte Boot-Komponenten
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture / Security
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** NovaOS Bootloader, BootFS, Bootmodule, Kernel Loader und Kernel

---

# Referenzen

* ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
* ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
* ADR-BOOT-1013 – Stage-1 bleibt minimal
* ADR-BOOT-1018 – Recovery als Bootloader-Modul
* ADR-0019 – Capability-basierter Systemschutz
* ADR-6032 – Trust-Backend-Abstraktion
* NPSPEC-BOOTFS-0001

---

# 1. Kontext

Der Bootprozess besitzt die höchsten Privilegien eines Betriebssystems.

Bereits kleinste Manipulationen an:

* Stage-2
* Bootmodulen
* Kernel Loader
* Kernel
* Recovery

können die vollständige Kontrolle über das System ermöglichen.

Aus diesem Grund muss bereits während des Bootvorgangs die Integrität sämtlicher Bootkomponenten überprüft werden.

Eine kryptografisch überprüfte Vertrauenskette ("Chain of Trust") ist ein bewährtes Konzept, bei dem jede Bootstufe die nächste vor ihrer Ausführung verifiziert.

---

# 2. Problemstellung

Es muss entschieden werden, ob Bootkomponenten ohne Integritätsprüfung geladen werden dürfen oder ob jede Komponente vor ihrer Ausführung kryptografisch überprüft wird.

---

# 3. Entscheidung

Alle ausführbaren Bootkomponenten werden digital signiert.

Vor ihrer Ausführung erfolgt eine kryptografische Signaturprüfung.

Nur erfolgreich verifizierte Komponenten dürfen ausgeführt werden.

---

# 4. Ziele

Diese Entscheidung verfolgt folgende Ziele:

* Integrität
* Authentizität
* Manipulationsschutz
* Vertrauenswürdiger Bootprozess
* Erweiterbarkeit
* Plattformunabhängigkeit

---

# 5. Grundprinzip

Jede Bootstufe überprüft die nächste Bootstufe.

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Bootmodule

↓

Kernel Loader

↓

Kernel
```

Dadurch entsteht eine vollständige Vertrauenskette.

---

# 6. Vertrauensmodell

NovaOS implementiert eine eigene kryptografische Vertrauenskette.

Sie ist unabhängig von:

* BIOS
* UEFI Secure Boot
* TPM

Diese Technologien dürfen zusätzlich genutzt werden, sind jedoch keine Voraussetzung.

---

# 7. Signaturpflicht

Folgende Komponenten müssen signiert werden:

* Stage-2
* Kernel Loader
* Recovery
* Diagnosemodule
* Bootmodule
* Kernel
* zukünftige Booterweiterungen

---

# 8. Ausnahmen

Nicht signaturpflichtig sind beispielsweise:

* Konfigurationsdateien
* Protokolldateien
* Benutzerdaten

Soweit sie keinen ausführbaren Code enthalten.

---

# 9. Stage-1

Stage-1 besitzt selbst keine Signaturprüfung für sich.

Da Stage-1 unmittelbar durch Firmware geladen wird, bildet sie den ersten Bestandteil der NovaOS-Vertrauenskette.

---

# 10. Stage-1 Aufgabe

Stage-1 überprüft ausschließlich Stage-2.

Weitere Komponenten werden nicht direkt geprüft.

Dadurch bleibt Stage-1 klein und übersichtlich.

---

# 11. Stage-2

Stage-2 überprüft anschließend:

* Bootmodule
* Recovery
* Kernel Loader

---

# 12. Kernel Loader

Der Kernel Loader überprüft:

* Kernel
* optionale Kernelmodule
* Bootparameter
* weitere sicherheitsrelevante Artefakte

---

# 13. Chain of Trust

Die Vertrauenskette lautet:

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Recovery / Kernel Loader

↓

Kernel
```

Jede Ebene überprüft die unmittelbar nachfolgende Ebene.

---

# 14. Signaturalgorithmus

NovaOS verwendet moderne kryptografische Signaturverfahren.

Der konkrete Algorithmus wird in einer separaten Spezifikation definiert.

Die Architektur muss algorithmusunabhängig bleiben.

---

# 15. Hashfunktion

Vor der Signaturprüfung wird ein kryptografischer Hash berechnet.

Die Architektur darf verschiedene Hashverfahren unterstützen.

---

# 16. Schlüsseltrennung

Private Schlüssel verlassen niemals die Build-Infrastruktur.

Im Bootprozess werden ausschließlich öffentliche Schlüssel verwendet.

---

# 17. Öffentliche Schlüssel

Öffentliche Schlüssel können gespeichert werden in:

* Stage-1
* Stage-2
* Trust Store
* Firmware
* TPM (optional)

---

# 18. Mehrere Schlüssel

NovaOS unterstützt mehrere vertrauenswürdige Signaturschlüssel.

Dadurch können beispielsweise unterstützt werden:

* Entwicklungszertifikate
* Testzertifikate
* Releasezertifikate
* Unternehmenszertifikate

---

# 19. Schlüsselrotation

Schlüssel müssen austauschbar sein.

Ein kompromittierter Schlüssel darf ersetzt werden, ohne die gesamte Bootarchitektur zu ändern.

---

# 20. Vertrauensspeicher

Die Liste vertrauenswürdiger öffentlicher Schlüssel wird zentral verwaltet.

---

# 21. Signaturformat

Jede Bootkomponente besitzt:

* Binärdaten
* Metadaten
* Signatur

Die konkrete Containerstruktur wird separat spezifiziert.

---

# 22. Integritätsprüfung

Vor jeder Ausführung werden geprüft:

* Dateigröße
* Header
* Hash
* Signatur
* Versionsinformationen

---

# 23. Fehlerbehandlung

Schlägt die Signaturprüfung fehl, darf die Komponente nicht ausgeführt werden.

Stattdessen wird eine definierte Fehlerbehandlung gestartet.

---

# 24. Recovery

Falls verfügbar, kann automatisch das Recovery-Modul gestartet werden.

Dadurch kann eine beschädigte Bootkomponente ersetzt werden.

---

# 25. Benutzerinformation

Manipulationen dürfen nicht stillschweigend ignoriert werden.

Der Benutzer erhält eine eindeutige Fehlermeldung.

---

# 26. Signierte Module

Alle Bootmodule unterliegen denselben Sicherheitsregeln.

Es existieren keine Ausnahmen für einzelne Modultypen.

---

# 27. BootFS

BootFS dient als Speicherort signierter Bootkomponenten.

Es stellt selbst keine Vertrauensentscheidung dar.

Die Integritätsprüfung erfolgt unabhängig vom Dateisystem.

---

# 28. Aktualisierung

Neue Versionen von Bootkomponenten müssen ebenfalls signiert sein.

Unsigned Updates werden grundsätzlich abgelehnt.

---

# 29. Entwicklungsmodus

Ein optionaler Entwicklermodus darf nicht signierte Komponenten zulassen.

Dieser Modus muss jedoch:

* eindeutig erkennbar,
* bewusst aktiviert
* und dokumentiert sein.

---

# 30. Produktionsmodus

Im Produktionsmodus dürfen ausschließlich gültig signierte Komponenten gestartet werden.

---

# 31. Integritätsverletzung

Eine ungültige Signatur gilt als Sicherheitsverletzung.

Sie darf niemals ignoriert werden.

---

# 32. Auditierbarkeit

Alle Signaturprüfungen sollen protokolliert werden.

---

# 33. Wiederholbarkeit

Die Prüfung derselben Binärdatei muss immer zum gleichen Ergebnis führen.

---

# 34. Determinismus

Die Signaturprüfung besitzt keine zufälligen Komponenten.

---

# 35. Plattformunabhängigkeit

Die Architektur funktioniert unabhängig von:

* BIOS
* UEFI
* x86
* ARM64
* RISC-V

---

# 36. Sicherheitsprinzip

Vertrauen entsteht ausschließlich durch kryptografische Verifikation.

Nicht durch Dateinamen, Speicherorte oder Konfigurationen.

---

# 37. Vertrauensanker

Stage-1 bildet den ersten softwareseitigen Vertrauensanker innerhalb der NovaOS-Bootkette.

---

# 38. Erweiterbarkeit

Neue Bootmodule übernehmen automatisch dieselben Signaturregeln.

Es sind keine Sondermechanismen erforderlich.

---

# 39. Wartbarkeit

Die Signaturprüfung wird als eigenständige Bibliothek implementiert.

Dadurch vermeiden alle Bootkomponenten doppelte Implementierungen.

---

# 40. Zusammenfassung Teil 1

NovaOS schützt den gesamten Bootprozess durch eine eigene kryptografische Vertrauenskette. Jede Bootstufe überprüft die Signatur der unmittelbar nachfolgenden Komponente, bevor diese ausgeführt wird. Dadurch können manipulierte oder nicht autorisierte Bootkomponenten erkannt und blockiert werden, unabhängig davon, ob UEFI Secure Boot oder ein TPM vorhanden sind. Dieses Prinzip entspricht dem bewährten Konzept einer kryptografischen Chain of Trust, wird jedoch als eigenständige, firmwareunabhängige NovaOS-Architektur umgesetzt.



# ADR-BOOT-1019: Signierte Boot-Komponenten (Teil 2)

---

# 41. Signaturbibliothek

Die kryptografische Signaturprüfung wird durch eine gemeinsame Bibliothek bereitgestellt.

Eigenschaften:

* zentrale Implementierung
* wiederverwendbar
* deterministisch
* unabhängig vom Dateisystem
* unabhängig von der Firmware

Dadurch verwenden alle Bootkomponenten denselben Prüfmechanismus.

---

# 42. Architektur

Die Signaturprüfung wird als eigenständige Schicht eingeführt.

```text
Stage-2

↓

Verification Library

↓

Hash Engine

↓

Trust Store

↓

Bootkomponente
```

---

# 43. Verifikationsablauf

Vor jeder Ausführung wird folgender Ablauf durchgeführt:

```text
Komponente laden

↓

Header prüfen

↓

Hash berechnen

↓

Signatur prüfen

↓

Vertrauenskette prüfen

↓

Version prüfen

↓

Ausführen
```

---

# 44. Headerprüfung

Vor der kryptografischen Prüfung wird überprüft:

* Magic Number
* Dateigröße
* Versionsformat
* Architektur
* Komponententyp

Ungültige Header werden sofort verworfen.

---

# 45. Hashberechnung

Die Hashberechnung erfolgt über den vollständigen signierten Datenbereich.

Nicht signierte Metadaten dürfen den Hashwert nicht beeinflussen.

---

# 46. Signaturprüfung

Die Signatur wird ausschließlich mit einem vertrauenswürdigen öffentlichen Schlüssel geprüft.

Private Schlüssel befinden sich niemals auf dem Zielsystem.

---

# 47. Vertrauenskette

Die Vertrauenskette umfasst:

```text
Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Recovery

↓

Kernel Loader

↓

Kernel
```

Jede Ebene validiert ausschließlich ihre direkte Nachfolgekomponente.

---

# 48. Schlüsselverwaltung

Der Trust Store unterstützt:

* mehrere Schlüssel
* Ablaufdaten
* Schlüsselkennungen
* Widerrufsstatus
* zukünftige Erweiterungen

---

# 49. Schlüsselkennungen

Jeder öffentliche Schlüssel besitzt eine eindeutige Kennung.

Dies erleichtert:

* Rotation
* Protokollierung
* Fehlersuche
* Auditierung

---

# 50. Schlüsselwiderruf

Ein kompromittierter Schlüssel muss widerrufen werden können.

Widerrufene Schlüssel dürfen keine Bootkomponenten mehr validieren.

---

# 51. Mehrere Signaturen

Eine Bootkomponente darf mehrere gültige Signaturen besitzen.

Beispiele:

* Hersteller
* Unternehmenszertifikat
* Testzertifikat

---

# 52. Algorithmusunabhängigkeit

Die Architektur schreibt keinen konkreten Signaturalgorithmus dauerhaft fest.

Neue kryptografische Verfahren können später ergänzt werden, ohne das Modulformat grundlegend zu ändern.

---

# 53. Versionsprüfung

Neben der Signatur kann zusätzlich die Versionsnummer geprüft werden.

Dadurch lassen sich unerwünschte Downgrades erkennen.

---

# 54. Anti-Rollback

Optional kann ein Anti-Rollback-Mechanismus aktiviert werden.

Dieser verhindert das Starten älterer, aber gültig signierter Versionen, sofern dies sicherheitspolitisch gewünscht ist.

---

# 55. Recovery-Integration

Schlägt eine Signaturprüfung fehl, kann automatisch:

* Recovery gestartet,
* ein gültiger Snapshot angeboten,
* oder eine Reparatur eingeleitet werden.

---

# 56. Bootmenü

Das Bootmenü zeigt den Signaturstatus jeder Bootkomponente an.

Beispiel:

```text
Kernel          ✓ Signiert

Recovery        ✓ Signiert

Diagnose        ✓ Signiert

Testmodul       ⚠ Entwicklermodus
```

---

# 57. Diagnose

Diagnosefunktionen können anzeigen:

* verwendeter Schlüssel
* Hashwert
* Signaturstatus
* Komponentenversion
* Prüfergebnis

---

# 58. Logging

Folgende Ereignisse werden protokolliert:

* erfolgreiche Prüfung
* fehlgeschlagene Prüfung
* unbekannter Schlüssel
* abgelaufener Schlüssel
* widerrufener Schlüssel

---

# 59. Benutzerinformation

Bei einer fehlgeschlagenen Prüfung erhält der Benutzer eine eindeutige Meldung.

Beispielsweise:

* Signatur ungültig
* Komponente manipuliert
* Schlüssel unbekannt
* Version nicht zulässig

---

# 60. Entwicklerbetrieb

Der Entwicklermodus muss deutlich sichtbar sein.

Während dieses Modus dürfen Sicherheitsmechanismen nicht stillschweigend deaktiviert werden.

---

# 61. Produktionsbetrieb

Im Produktionsmodus gilt:

Nicht signierte Komponenten werden niemals gestartet.

---

# 62. Bootmodule

Alle Bootmodule unterliegen denselben Sicherheitsregeln.

Dies umfasst unter anderem:

* Recovery
* Diagnose
* Speichertest
* Kernel Loader
* zukünftige Erweiterungen

---

# 63. Kernel

Der Kernel wird wie jedes andere Bootmodul behandelt.

Er besitzt keine Sonderrechte hinsichtlich der Signaturprüfung.

---

# 64. Erweiterungsmodule

Neue Bootmodule müssen dieselbe Verifikationsschnittstelle verwenden.

Dadurch bleibt das Sicherheitsmodell konsistent.

---

# 65. Build-System

Das Build-System erzeugt:

* Binärdatei
* Hash
* Signatur
* Metadaten

als zusammengehöriges Artefakt.

---

# 66. Continuous Integration

Vor jeder Veröffentlichung müssen automatisch geprüft werden:

* Integrität
* Signatur
* Kompatibilität
* Reproduzierbarkeit

---

# 67. Reproduzierbare Builds

Nach Möglichkeit sollen reproduzierbare Builds unterstützt werden.

Dadurch kann überprüft werden, dass veröffentlichte Binärdateien tatsächlich aus dem dokumentierten Quellcode erzeugt wurden.

---

# 68. Performance

Die Signaturprüfung erfolgt ausschließlich beim Laden einer Komponente.

Während der Laufzeit entstehen dadurch keine zusätzlichen Kosten.

---

# 69. Speicherbedarf

Die Signaturbibliothek soll möglichst kompakt bleiben.

Nicht benötigte kryptografische Verfahren werden nicht eingebunden.

---

# 70. Fehlerisolation

Ein Fehler innerhalb der Signaturprüfung darf nicht zu undefiniertem Verhalten führen.

Die betroffene Komponente wird stattdessen verworfen.

---

# 71. Testbarkeit

Die Verifikationsbibliothek muss unabhängig testbar sein.

Hierzu gehören:

* Unit-Tests
* Integrationstests
* Negativtests
* Fuzzing
* Regressionstests

---

# 72. Sicherheitsanforderungen

Die Implementierung muss:

* deterministisch
* nachvollziehbar
* auditierbar
* fehlertolerant
* modular

sein.

---

# 73. Alternativen

## Keine Signaturprüfung

**Abgelehnt**

Nachteile:

* Manipulationen bleiben unentdeckt
* keine Vertrauenskette
* geringes Sicherheitsniveau

---

## Ausschließlich UEFI Secure Boot

**Abgelehnt**

Gründe:

* BIOS-Systeme werden nicht unterstützt.
* NovaOS soll firmwareunabhängig arbeiten.
* Die Sicherheitsarchitektur soll auf allen unterstützten Plattformen konsistent sein.

---

## TPM als Pflicht

**Abgelehnt**

Gründe:

* ältere Systeme würden ausgeschlossen,
* TPM ist nicht überall verfügbar,
* NovaOS soll auch ohne TPM sicher starten können.

---

## Eigene Signaturarchitektur

**Angenommen**

Sie erfüllt die Anforderungen an Plattformunabhängigkeit, Modularität und langfristige Erweiterbarkeit.

---

# 74. Implementierungsstrategie

## Phase 1

* Trust Store
* Hashbibliothek
* Signaturprüfung

## Phase 2

* Stage-2-Verifikation
* Recovery-Verifikation
* Kernel-Verifikation

## Phase 3

* Schlüsselrotation
* mehrere Zertifikate
* Audit-Logging

## Phase 4

* Widerrufslisten
* Anti-Rollback
* Richtlinienverwaltung

## Phase 5

* optionale Integration mit TPM
* optionale Integration mit UEFI Secure Boot
* Remote-Attestation (optional)

---

# 75. Beispielarchitektur

```text
Firmware

↓

Stage-1

↓

Stage-2

↓

Verification Library

↓

Boot Module Manager

↓

Kernel Loader

↓

Kernel
```

---

# 76. Dokumentation

Die Signaturarchitektur muss dokumentieren:

* Modulformat
* Hashverfahren
* Signaturformat
* Trust Store
* Schlüsselverwaltung
* Fehlercodes
* Prüfabläufe

---

# 77. Zukunftssicherheit

Neue kryptografische Verfahren können ergänzt werden, ohne bestehende Bootmodule grundlegend zu verändern.

Ebenso können zusätzliche Vertrauensquellen integriert werden.

---

# 78. Plattformunabhängigkeit

Die Architektur darf nicht von folgenden Technologien abhängen:

* BIOS
* UEFI Secure Boot
* TPM
* Secure Enclave
* Hardware Security Module

Diese können ergänzend genutzt werden, sind jedoch optional.

---

# 79. Konformitätskriterien

Eine Implementierung erfüllt dieses ADR, wenn:

* alle ausführbaren Bootkomponenten digital signiert sind,
* jede Bootstufe ausschließlich ihre direkte Nachfolgekomponente verifiziert,
* ungültig signierte Komponenten nicht gestartet werden,
* die Signaturprüfung über eine gemeinsame Bibliothek erfolgt,
* mehrere öffentliche Schlüssel unterstützt werden,
* Schlüsselrotation möglich ist,
* Recovery bei fehlgeschlagener Verifikation gestartet werden kann,
* und die Sicherheitsarchitektur unabhängig von BIOS, UEFI Secure Boot und TPM funktioniert.

---

# 80. Entscheidungsergebnis

NovaOS implementiert eine eigene kryptografische Vertrauenskette für den gesamten Bootprozess. Alle ausführbaren Bootkomponenten werden digital signiert und vor ihrer Ausführung verifiziert. Die Architektur bleibt dabei bewusst unabhängig von firmware- oder hardwaregebundenen Sicherheitsmechanismen und kann optional mit diesen zusammenarbeiten.

---

# 81. Zusammenfassung

Die Sicherheit des Bootprozesses basiert auf einer durchgängigen Vertrauenskette, bei der jede Bootstufe die Integrität der unmittelbar nachfolgenden Komponente überprüft. Dadurch werden manipulierte oder nicht autorisierte Bootmodule zuverlässig erkannt und ihre Ausführung verhindert.

Die Signaturarchitektur ist modular aufgebaut, unterstützt mehrere öffentliche Schlüssel, Schlüsselrotation sowie optionale Erweiterungen wie Anti-Rollback oder TPM-Integration. Gleichzeitig bleibt NovaOS unabhängig von UEFI Secure Boot und kann dieselben Sicherheitsprinzipien auf BIOS-, UEFI-, ARM- und RISC-V-Systemen anwenden.
