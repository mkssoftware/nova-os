# RFC-BOOT-0007: Nova Boot Security Chain (NBSC) (Teil 1)

* **Dokumenttyp:** Request for Comments (RFC)
* **Dokument-ID:** RFC-BOOT-0007
* **Titel:** Nova Boot Security Chain (NBSC)
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Protocol / Sicherheit
* **Gültigkeitsbereich:** Firmware, Stage-1, Stage-2, Boot Module Manager, Kernel Loader, Recovery, Kernel
* **Abhängigkeiten:** RFC-BOOT-0001, RFC-BOOT-0002, RFC-BOOT-0003, RFC-BOOT-0005, RFC-BOOT-0006, ADR-BOOT-1012, ADR-BOOT-1020
* **Letzte Aktualisierung:** 2026-07-29

---

# Inhaltsverzeichnis

1. Einleitung
2. Motivation
3. Ziele
4. Nicht-Ziele
5. Architektur
6. Designprinzipien
7. Vertrauensmodell
8. Signaturmodell
9. Schlüsselverwaltung
10. Integritätsmodell
11. Sicherheitsrichtlinien
12. Fehlerbehandlung
13. Qualitätsanforderungen

---

# 1. Einleitung

Dieses Dokument definiert die **Nova Boot Security Chain (NBSC)**.

Die NBSC beschreibt die vollständige Vertrauenskette des NovaOS-Bootvorgangs und legt fest, wie Bootkomponenten kryptografisch überprüft, validiert und sicher aneinander übergeben werden.

---

# 2. Motivation

Ein modernes Betriebssystem muss sicherstellen, dass ausschließlich authentische und unveränderte Bootkomponenten ausgeführt werden.

Manipulationen während des Bootvorgangs können das gesamte System kompromittieren. Deshalb wird jeder Übergabeschritt kryptografisch abgesichert.

---

# 3. Ziele

Die Nova Boot Security Chain verfolgt folgende Ziele:

* Aufbau einer durchgängigen Chain of Trust
* Integritätsprüfung aller Bootkomponenten
* Authentizitätsprüfung durch digitale Signaturen
* Plattformunabhängigkeit
* Erweiterbarkeit
* Unterstützung optionaler Hardware-Root-of-Trust-Komponenten

---

# 4. Nicht-Ziele

Die NBSC definiert nicht:

* Benutzeranmeldung
* Dateisystemberechtigungen
* Anwendungssicherheit
* Netzwerksicherheit
* Laufzeitsicherheit des Kernels

Sie beschreibt ausschließlich die Sicherheit des Bootprozesses.

---

# 5. Architekturübersicht

```text id="9n1rwb"
Firmware

↓

Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Kernel Loader

↓

Kernel

↓

Userspace
```

Jede Stufe überprüft die nachfolgende Stufe, bevor diese ausgeführt wird.

---

# 6. Grundprinzipien

Die NBSC basiert auf folgenden Prinzipien:

* Keine implizite Vertrauensannahme
* Jede Komponente validiert ihren Nachfolger
* Kryptografische Integrität
* Versionierte Sicherheitsrichtlinien
* Deterministische Prüfungen
* Erweiterbare Vertrauensanker

---

# 7. Chain of Trust

Die Vertrauenskette beginnt bei der Firmware und endet erst mit dem gestarteten Kernel.

Eine Komponente darf ausschließlich gestartet werden, wenn ihre Integrität erfolgreich geprüft wurde.

---

# 8. Vertrauensanker

Die NBSC unterstützt mehrere Vertrauensanker.

Beispiele:

* Firmware
* TPM (optional)
* Hardware Security Module (optional)
* Software-Zertifikatsspeicher

Die Architektur ist nicht von einem einzelnen Vertrauensanker abhängig.

---

# 9. Signaturpflicht

Folgende Komponenten müssen digital signiert sein:

* Stage-2
* Bootmodule
* Kernelimage
* Recovery-Module
* Sicherheitsmodule

Optionale Komponenten können ebenfalls signiert werden.

---

# 10. Signaturalgorithmen

Empfohlene Verfahren:

* Ed25519
* ECDSA P-256
* RSA-3072

Weitere Verfahren können in zukünftigen Versionen ergänzt werden.

---

# 11. Hashalgorithmen

Unterstützt werden mindestens:

* SHA-256
* SHA-384
* SHA-512

Unsichere oder veraltete Algorithmen dürfen nicht verwendet werden.

---

# 12. Integritätsprüfung

Vor jeder Übergabe werden mindestens geprüft:

* Dateigröße
* Header
* Versionsinformationen
* Prüfsumme
* Hash
* Digitale Signatur

---

# 13. Schlüsselverwaltung

Die NBSC trennt strikt zwischen:

* öffentlichen Schlüsseln
* privaten Schlüsseln
* Vertrauensankern
* Zertifikatsinformationen

Private Schlüssel dürfen niemals Bestandteil des BootFS sein.

---

# 14. Zertifikate

Optional können Zertifikate verwendet werden.

Sie dienen ausschließlich der Authentifizierung von Signaturen und ersetzen nicht die eigentliche Integritätsprüfung.

---

# 15. Vertrauensspeicher

Der Vertrauensspeicher enthält ausschließlich öffentliche Informationen.

Beispiele:

* öffentliche Schlüssel
* Zertifikatsketten
* Richtlinienversionen

---

# 16. Mehrere Schlüssel

Die NBSC unterstützt mehrere vertrauenswürdige Signaturschlüssel.

Dadurch können Schlüsselwechsel durchgeführt werden, ohne bestehende Installationen unbrauchbar zu machen.

---

# 17. Schlüsselrotation

Ein Schlüssel kann ersetzt werden, wenn:

* der neue Schlüssel bereits vertrauenswürdig ist,
* die Richtlinienversion kompatibel ist,
* und die Integrität der Vertrauenskette erhalten bleibt.

---

# 18. Schlüsselwiderruf

Widerrufene Schlüssel dürfen keine neuen Komponenten mehr validieren.

Bereits installierte Komponenten müssen entsprechend den Sicherheitsrichtlinien behandelt werden.

---

# 19. Vertrauensrichtlinien

Die Sicherheitsrichtlinien definieren:

* zulässige Algorithmen
* minimale Schlüssellängen
* erlaubte Signaturversionen
* unterstützte Richtlinienversionen

---

# 20. Sicherheitsstufen

Die NBSC kann unterschiedliche Sicherheitsstufen definieren.

Beispiele:

* Standard
* Erweitert
* Hochsicher
* Entwicklungsmodus

Der Entwicklungsmodus ist ausschließlich für Testsysteme vorgesehen.

---

# 21. Entwicklungsmodus

Im Entwicklungsmodus können zusätzliche Diagnoseinformationen aktiviert werden.

Die Integritätsprüfung bleibt jedoch verpflichtend.

---

# 22. Produktionsmodus

Im Produktionsmodus sind ausschließlich signierte Komponenten zulässig.

Unsignierte oder manipulierte Komponenten dürfen nicht ausgeführt werden.

---

# 23. Integritätsverletzungen

Wird eine Integritätsverletzung erkannt, muss der Bootvorgang kontrolliert unterbrochen werden.

Mögliche Reaktionen:

* Fehlermeldung
* Recovery
* Rollback
* Neustart

---

# 24. Recovery-Integration

Die Recovery-Umgebung ist Bestandteil der Vertrauenskette.

Auch Recovery-Module müssen dieselben Sicherheitsanforderungen erfüllen wie alle übrigen Bootkomponenten.

---

# 25. Bootkonfiguration

Die Bootkonfiguration darf sicherheitskritische Prüfungen nicht deaktivieren oder umgehen.

---

# 26. Nova Boot ABI

Die Übergabe der Bootinformationen erfolgt ausschließlich über das Nova Boot ABI.

Die Integrität dieser Daten muss vor der Übergabe sichergestellt werden.

---

# 27. Qualitätsanforderungen

Die Nova Boot Security Chain muss:

* deterministisch,
* vollständig nachvollziehbar,
* kryptografisch abgesichert,
* versioniert,
* auditierbar,
* und langfristig erweiterbar

sein.

---

# 28. Zusammenfassung Teil 1

Die **Nova Boot Security Chain (NBSC)** definiert die grundlegenden Sicherheitsmechanismen der NovaOS-Bootarchitektur. Sie etabliert eine durchgängige Vertrauenskette von der Firmware bis zum Kernel und schreibt die kryptografische Prüfung aller sicherheitsrelevanten Bootkomponenten vor. Durch versionierte Richtlinien, mehrere Vertrauensanker und ein flexibles Schlüsselmanagement schafft sie die Grundlage für einen langfristig sicheren und erweiterbaren Bootprozess.



# RFC-BOOT-0007: Nova Boot Security Chain (NBSC) (Teil 2)

---

# 29. Vertrauensübergabe

Jede Bootkomponente übergibt die Kontrolle ausschließlich an eine erfolgreich validierte Nachfolgekomponente.

Vor jeder Übergabe müssen sämtliche Integritäts- und Authentizitätsprüfungen abgeschlossen sein.

---

# 30. Sicherheitszustände

Während des Bootvorgangs kann sich das System in folgenden Sicherheitszuständen befinden:

```text id="g1s9wd"
UNVERIFIED

↓

VALIDATING

↓

TRUSTED

↓

RUNNING
```

---

# 31. Fehlerzustände

Mögliche Sicherheitsfehler:

* INVALID_SIGNATURE
* INVALID_HASH
* INVALID_CERTIFICATE
* REVOKED_KEY
* UNSUPPORTED_POLICY
* TRUST_FAILURE

---

# 32. Vertrauensabbruch

Kann eine Komponente nicht erfolgreich validiert werden, endet die Vertrauenskette.

Eine Ausführung der betroffenen Komponente ist unzulässig.

---

# 33. Sicherheitsrichtlinien

Alle Prüfungen erfolgen nach einer versionierten Sicherheitsrichtlinie.

Diese definiert unter anderem:

* erlaubte Algorithmen,
* Mindestschlüssellängen,
* akzeptierte Zertifikate,
* unterstützte Richtlinienversionen.

---

# 34. Richtlinienversion

Jede Sicherheitsrichtlinie besitzt:

* Major-Version
* Minor-Version

Neue Minor-Versionen dürfen ausschließlich kompatible Erweiterungen enthalten.

---

# 35. Auditierbarkeit

Alle sicherheitsrelevanten Entscheidungen sollen nachvollziehbar protokolliert werden.

Beispiele:

* erfolgreiche Signaturprüfung
* fehlgeschlagene Integritätsprüfung
* verwendeter Vertrauensanker
* Richtlinienversion

---

# 36. Sicherheitsprotokoll

Das Sicherheitsprotokoll kann enthalten:

* Zeitstempel (falls verfügbar)
* geprüfte Komponente
* Hashalgorithmus
* Signaturalgorithmus
* Prüfergebnis

---

# 37. Sicherheitsereignisse

Beispiele:

* Schlüsselwiderruf erkannt
* beschädigte Signatur
* ungültiger Header
* Richtlinienverletzung
* Integritätsfehler

---

# 38. Recovery-Auslösung

Die Recovery-Umgebung kann automatisch gestartet werden bei:

* fehlgeschlagener Kernelvalidierung,
* beschädigten Bootmodulen,
* ungültiger Sicherheitsrichtlinie,
* nicht vertrauenswürdigen Komponenten.

---

# 39. Rollback

Ist eine frühere signierte Version verfügbar, kann automatisch ein Rollback vorgeschlagen oder durchgeführt werden.

Dabei gelten dieselben Sicherheitsanforderungen wie für aktuelle Komponenten.

---

# 40. Vertrauensspeicher-Aktualisierung

Neue öffentliche Schlüssel oder Zertifikate dürfen ausschließlich über signierte Aktualisierungspakete eingespielt werden.

---

# 41. Schlüsselmigration

Die NBSC unterstützt einen kontrollierten Wechsel auf neue Signaturschlüssel.

Während einer Übergangsphase können mehrere vertrauenswürdige Schlüssel parallel akzeptiert werden.

---

# 42. TPM-Integration

Ein **Trusted Platform Module (TPM)** kann als zusätzlicher Vertrauensanker genutzt werden.

Die Verwendung eines TPM ist optional und darf nicht Voraussetzung für den Betrieb von NovaOS sein.

---

# 43. Hardware Security Module

Alternativ kann ein Hardware Security Module (HSM) verwendet werden.

Die Schnittstelle muss austauschbar sein, sodass unterschiedliche Implementierungen unterstützt werden.

---

# 44. Softwarebasierter Vertrauensanker

Ist keine spezielle Hardware vorhanden, verwendet NovaOS einen softwarebasierten Vertrauensspeicher.

Dadurch bleibt die Bootarchitektur auf älteren Systemen vollständig funktionsfähig.

---

# 45. Mehrere Sicherheitsprofile

Ein System kann unterschiedliche Sicherheitsprofile bereitstellen.

Beispiele:

* Standard
* Unternehmensrichtlinie
* Entwicklungsprofil
* Testprofil

Der Wechsel zwischen Profilen darf nur autorisiert erfolgen.

---

# 46. Sicherheitsworkflow

```text id="v5nhu8"
Komponente laden

↓

Header prüfen

↓

Hash berechnen

↓

Signatur prüfen

↓

Richtlinie prüfen

↓

Vertrauen herstellen

↓

Ausführen
```

---

# 47. Vertrauenszustandsautomat

```text id="p9rmk2"
UNVERIFIED

↓

VALIDATING

↓

TRUSTED

↓

EXECUTED

↓

FINISHED
```

Fehler führen unmittelbar in einen Abbruchzustand.

---

# 48. Fehlerbehandlung

Bei sicherheitsrelevanten Fehlern darf keine automatische Fortsetzung erfolgen.

Mögliche Reaktionen:

* Boot abbrechen
* Recovery starten
* Rollback durchführen
* Benutzer informieren

---

# 49. Benutzerinformation

Sicherheitsmeldungen müssen verständlich formuliert sein.

Anzuzeigen sind mindestens:

* betroffene Komponente,
* Fehlerklasse,
* empfohlene Maßnahme.

Interne kryptografische Details können optional eingeblendet werden.

---

# 50. Schutz vor Downgrade

Die Sicherheitsrichtlinien können das Starten älterer, bekanntermaßen unsicherer Komponenten verhindern.

Ausnahmen sind ausschließlich über definierte Recovery- oder Entwicklungsrichtlinien zulässig.

---

# 51. Messbare Integrität

Optional können Integritätsmessungen protokolliert werden.

Diese Messwerte dürfen später für:

* Sicherheitsanalysen,
* Systemprüfungen,
* oder Attestierungsverfahren

verwendet werden.

---

# 52. Kryptografische Agilität

Die NBSC ist so aufgebaut, dass kryptografische Verfahren künftig ersetzt oder ergänzt werden können.

Neue Algorithmen werden über versionierte Sicherheitsrichtlinien eingeführt.

---

# 53. Erweiterbarkeit

Neue Sicherheitsmechanismen werden ausschließlich über definierte Erweiterungspunkte ergänzt.

Vorhandene Schnittstellen bleiben kompatibel.

---

# 54. Mehrarchitektur-Unterstützung

Die Sicherheitskette arbeitet unabhängig von der Zielarchitektur.

Sie gilt gleichermaßen für:

* x86
* x86_64
* ARM64
* RISC-V
* zukünftige Plattformen

---

# 55. Logging

Das Bootprotokoll enthält mindestens:

* validierte Komponenten
* verwendete Richtlinie
* verwendeten Vertrauensanker
* Prüfergebnisse
* Recovery-Auslösungen

---

# 56. Teststrategie

Die Sicherheitskette wird geprüft durch:

* Unit-Tests
* Integrationstests
* Fuzzing
* Penetrationstests
* Regressionstests
* Schlüsselrotations-Tests

---

# 57. Referenzarchitektur

```text id="rm0ct1"
Trust Anchor

↓

Security Policy

↓

Stage-2

↓

Boot Module Manager

↓

Kernel Loader

↓

Kernel
```

---

# 58. Referenzablauf

```text id="k7lvqa"
Komponente öffnen

↓

Header validieren

↓

Hash berechnen

↓

Signatur prüfen

↓

Richtlinie anwenden

↓

Vertrauen herstellen

↓

Komponente starten
```

---

# 59. Konformitätsanforderungen

Eine Implementierung erfüllt dieses RFC, wenn:

* jede Bootkomponente vor der Ausführung validiert wird,
* digitale Signaturen verpflichtend geprüft werden,
* ausschließlich vertrauenswürdige Schlüssel akzeptiert werden,
* Widerrufsrichtlinien berücksichtigt werden,
* Sicherheitsverletzungen einen kontrollierten Abbruch auslösen,
* und die Vertrauenskette bis zur Kernelübergabe vollständig erhalten bleibt.

---

# 60. Referenzimplementierung

Die Referenzimplementierung dient ausschließlich als Nachweis dieser Spezifikation.

Alternative Implementierungen sind zulässig, sofern sämtliche Anforderungen dieses RFC erfüllen.

---

# 61. Zukunftserweiterungen

Die Nova Boot Security Chain ist auf langfristige Weiterentwicklung ausgelegt.

Mögliche Erweiterungen:

* Post-Quanten-Kryptografie
* mehrere parallele Vertrauensanker
* Remote-Attestierung
* Hardware-Root-of-Trust-Erweiterungen
* signierte Sicherheitsrichtlinien
* Integritätsmessungen über den gesamten Bootprozess

---

# 62. Abwärtskompatibilität

Neue Minor-Versionen dürfen ausschließlich kompatible Erweiterungen enthalten.

Inkompatible Änderungen an Sicherheitsrichtlinien oder Vertrauensmodellen erfordern eine neue Major-Version.

---

# 63. Zusammenfassung

Die **Nova Boot Security Chain (NBSC)** definiert die vollständige Vertrauenskette des NovaOS-Bootprozesses. Von der Firmware über Stage-1, Stage-2, den Boot Module Manager und den Kernel Loader bis hin zum Kernel wird jede Komponente vor ihrer Ausführung anhand standardisierter Integritäts- und Authentizitätsprüfungen validiert.

Durch versionierte Sicherheitsrichtlinien, kryptografische Agilität, kontrollierte Schlüsselrotation, optionale Hardware-Vertrauensanker und ein konsequentes Audit- und Protokollierungskonzept schafft die NBSC eine robuste, nachvollziehbare und langfristig erweiterbare Sicherheitsarchitektur. Sie bildet damit das Fundament für einen manipulationssicheren Startvorgang innerhalb der NovaOS-Bootplattform.
