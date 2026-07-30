# RFC-BOOT-0003: Nova Boot Module Format (NBMF) (Teil 1)

* **Dokumenttyp:** Request for Comments (RFC)
* **Dokument-ID:** RFC-BOOT-0003
* **Titel:** Nova Boot Module Format (NBMF)
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Protocol / Modulformat
* **Gültigkeitsbereich:** Stage-2, Boot Module Manager, Recovery, Kernel Loader und zukünftige Bootmodule
* **Abhängigkeiten:** RFC-BOOT-0001, RFC-BOOT-0002, ADR-BOOT-1018, ADR-BOOT-1019
* **Letzte Aktualisierung:** 2026-07-29

---

# Inhaltsverzeichnis

1. Einleitung
2. Motivation
3. Ziele
4. Nicht-Ziele
5. Begriffe
6. Architektur
7. Modulaufbau
8. Modulheader
9. Versionsmodell
10. Modularten
11. Abhängigkeiten
12. Speicherlayout
13. Einstiegspunkte
14. Lebenszyklus
15. Validierung

---

# 1. Einleitung

Dieses Dokument definiert das **Nova Boot Module Format (NBMF)**.

Das NBMF beschreibt das standardisierte Dateiformat sämtlicher ausführbarer Bootmodule innerhalb der NovaOS-Bootumgebung.

Alle Module, die vom Boot Module Manager geladen werden, müssen diesem Format entsprechen.

---

# 2. Motivation

Die Bootumgebung besteht aus mehreren unabhängigen Komponenten.

Beispiele:

* Recovery
* Kernel Loader
* Diagnose
* Speichertest
* Firmware-Tools
* zukünftige Erweiterungen

Damit diese Komponenten einheitlich geladen werden können, wird ein gemeinsames Modulformat definiert.

---

# 3. Ziele

Das Modulformat verfolgt folgende Ziele:

* einheitliches Ladeverfahren
* eindeutige Identifikation
* Versionsverwaltung
* kryptografische Signierung
* Plattformunabhängigkeit
* Erweiterbarkeit
* einfache Validierung

---

# 4. Nicht-Ziele

Das NBMF definiert nicht:

* BootFS
* Boot-ABI
* Kerneldateiformat
* Userspace-Programme
* ELF-Ausführung im Kernel

Es beschreibt ausschließlich Bootmodule.

---

# 5. Grundprinzipien

Das Nova Boot Module Format basiert auf folgenden Prinzipien:

* Selbstbeschreibend
* Versioniert
* Signierbar
* Erweiterbar
* Validierbar
* Deterministisch

---

# 6. Architekturübersicht

```text id="6f7wqa"
BootFS

↓

Boot Module Manager

↓

Nova Boot Module

↓

Start
```

Der Boot Module Manager verarbeitet ausschließlich gültige Module.

---

# 7. Moduldefinition

Ein Bootmodul ist eine eigenständige ausführbare Komponente.

Es besitzt:

* eindeutige Identität
* Metadaten
* Code
* Daten
* Signatur

---

# 8. Dateiendung

Empfohlene Dateiendung:

```text id="q8fg1n"
.nbm
```

Alternative Endungen dürfen implementierungsabhängig verwendet werden.

---

# 9. Dateiaufbau

Jedes Modul besitzt folgenden logischen Aufbau.

```text id="0hyh76"
Module Header

↓

Module Metadata

↓

Executable Image

↓

Optional Resources

↓

Signature Block
```

---

# 10. Modulheader

Der Header beschreibt sämtliche Eigenschaften des Moduls.

Er enthält keine Programmlogik.

---

# 11. Headerfelder

Ein Header enthält mindestens:

* Magic Number
* Formatversion
* Modul-ID
* Modulklasse
* Architektur
* Dateigröße
* Einstiegspunkt

---

# 12. Beispielheader

```c id="2o0vgb"
typedef struct
{
    uint32_t magic;
    uint16_t major;
    uint16_t minor;
    uint32_t module_id;
    uint32_t module_type;
    uint64_t image_size;
    uint64_t entry_point;
} nbmf_header_t;
```

---

# 13. Magic Number

Jedes Modul beginnt mit einer eindeutigen Magic Number.

Dadurch erkennt der Boot Module Manager ungültige Dateien sofort.

---

# 14. Formatversion

Das Modulformat besitzt:

* Major-Version
* Minor-Version

Neue inkompatible Änderungen erfordern eine neue Major-Version.

---

# 15. Modul-ID

Jedes Modul besitzt eine dauerhaft eindeutige Kennung.

Beispiele:

* BOOT_KERNEL_LOADER
* BOOT_RECOVERY
* BOOT_MEMTEST
* BOOT_DIAGNOSE
* BOOT_UPDATE

---

# 16. Modulname

Optional kann ein menschenlesbarer Modulname gespeichert werden.

Dieser dient ausschließlich Diagnose- und Verwaltungszwecken.

---

# 17. Modulbeschreibung

Optional kann eine kurze Beschreibung enthalten sein.

Beispiel:

> "NovaOS Recovery Environment"

---

# 18. Modulklasse

Das Modulformat unterscheidet verschiedene Klassen.

Beispiele:

* Recovery
* Loader
* Diagnose
* Utility
* Service
* Test
* Experimental

---

# 19. Architekturkennung

Ein Modul beschreibt eindeutig seine Zielarchitektur.

Beispiele:

* x86
* x86_64
* ARM64
* RISC-V

---

# 20. Einstiegspunkt

Der Einstiegspunkt beschreibt die erste auszuführende Funktion des Moduls.

---

# 21. Imagegröße

Die Größe beschreibt ausschließlich den ausführbaren Teil des Moduls.

Signaturen und Metadaten werden getrennt betrachtet.

---

# 22. Modulstatus

Während des Bootvorgangs kann ein Modul folgende Zustände besitzen:

* gefunden
* geladen
* geprüft
* gestartet
* beendet
* fehlerhaft

---

# 23. Modulmetadaten

Zusätzliche Metadaten können enthalten:

* Hersteller
* Buildnummer
* Builddatum
* Lizenz
* Beschreibung

---

# 24. Buildinformationen

Optional:

* Compiler
* Optimierungsstufe
* Buildsystem
* Git-Revision

Diese Informationen dienen ausschließlich Diagnosezwecken.

---

# 25. Modulabhängigkeiten

Ein Modul kann andere Module voraussetzen.

Beispiele:

```text id="7i6p2t"
Recovery

↓

Framebuffer

↓

Input

↓

BootFS
```

---

# 26. Pflichtabhängigkeiten

Pflichtabhängigkeiten müssen erfüllt sein.

Andernfalls darf das Modul nicht gestartet werden.

---

# 27. Optionale Abhängigkeiten

Optionale Module erweitern den Funktionsumfang.

Fehlen sie, bleibt das Modul dennoch lauffähig.

---

# 28. Modulpriorität

Jedes Modul kann eine Priorität besitzen.

Diese beeinflusst ausschließlich die Startreihenfolge.

---

# 29. Modulkonflikte

Ein Modul kann definieren, mit welchen anderen Modulen es inkompatibel ist.

Der Boot Module Manager verhindert gleichzeitige Aktivierung.

---

# 30. Modulressourcen

Ein Modul kann optionale Ressourcen enthalten.

Beispiele:

* Icons
* Bitmap-Fonts
* Übersetzungen
* Konfigurationsdateien
* Themes

---

# 31. Ressourcenbereiche

Ressourcen werden logisch vom ausführbaren Code getrennt.

Dadurch bleibt das Modulformat übersichtlich.

---

# 32. Speicherlayout

Nach dem Laden besitzt ein Modul folgenden Aufbau.

```text id="zjlwmq"
Header

↓

Code

↓

Read-Only Data

↓

Writable Data

↓

Resources
```

---

# 33. Ladeadresse

Die endgültige Ladeadresse bestimmt ausschließlich der Boot Module Manager.

Module dürfen keine festen Adressen voraussetzen.

---

# 34. Relokation

Das Modulformat unterstützt positionsunabhängigen Code (PIC).

Dadurch können Module flexibel im Speicher platziert werden.

---

# 35. Qualitätsanforderungen

Ein gültiges Bootmodul muss:

* einen gültigen Header besitzen,
* versioniert sein,
* eindeutig identifizierbar sein,
* validierbar sein,
* eine definierte Architektur besitzen,
* einen Einstiegspunkt definieren
* und deterministisch geladen werden können.

---

# 36. Zusammenfassung Teil 1

Das Nova Boot Module Format definiert einen standardisierten Aufbau für sämtliche Bootmodule innerhalb der NovaOS-Bootumgebung. Jedes Modul besitzt einen selbstbeschreibenden Header, eindeutige Identifikationsmerkmale, Versionsinformationen sowie klar definierte Metadaten und Ressourcenbereiche. Durch diese Struktur können Bootmodule unabhängig entwickelt, versioniert und vom Boot Module Manager einheitlich verarbeitet werden.




# RFC-BOOT-0003: Nova Boot Module Format (NBMF) (Teil 2)

---

# 37. Signaturblock

Jedes ausführbare Bootmodul muss einen Signaturblock besitzen.

Der Signaturblock befindet sich logisch am Ende des Moduls und enthält ausschließlich Informationen zur Integritäts- und Authentizitätsprüfung.

---

# 38. Bestandteile des Signaturblocks

Der Signaturblock enthält mindestens:

* Signaturalgorithmus
* Hashalgorithmus
* Signatur
* Schlüsselkennung
* Signaturversion

Optional können zusätzliche Metadaten enthalten sein.

---

# 39. Unterstützte Hashalgorithmen

Empfohlen werden:

* SHA-256
* SHA-384
* SHA-512

Weitere Algorithmen können in zukünftigen Versionen ergänzt werden.

---

# 40. Signaturalgorithmen

Das Modulformat schreibt keinen einzelnen Algorithmus vor.

Empfohlene Verfahren:

* Ed25519
* ECDSA P-256
* RSA-3072

Die unterstützten Verfahren werden durch den Boot Module Manager definiert.

---

# 41. Signaturprüfung

Vor der Ausführung eines Moduls erfolgt zwingend:

1. Header prüfen
2. Formatversion prüfen
3. Dateigröße prüfen
4. Hash berechnen
5. Signatur verifizieren
6. Abhängigkeiten prüfen
7. Modul freigeben

---

# 42. Vertrauenskette

Die Modulprüfung ist Bestandteil der NovaOS-Chain-of-Trust.

```text id="2syg79"
Stage-1

↓

Stage-2

↓

Boot Module Manager

↓

Bootmodul

↓

Kernel Loader
```

Ein Modul ohne gültige Signatur darf nicht ausgeführt werden.

---

# 43. Ladealgorithmus

Der Boot Module Manager lädt Module in einer fest definierten Reihenfolge.

```text id="m0tr0r"
Datei öffnen

↓

Header lesen

↓

Version prüfen

↓

Signatur prüfen

↓

Speicher reservieren

↓

Code laden

↓

Relokation

↓

Abhängigkeiten prüfen

↓

Start
```

---

# 44. Speicherreservierung

Vor dem Laden reserviert der Boot Module Manager ausreichend Speicher für:

* Code
* Daten
* Ressourcen
* Relokationstabellen

---

# 45. Initialisierung

Vor dem Einstiegspunkt werden folgende Schritte durchgeführt:

* Speicher aufbauen
* Relokation anwenden
* Datenbereiche initialisieren
* Ressourcen registrieren

---

# 46. Einstiegspunkt

Nach erfolgreicher Initialisierung wird ausschließlich der definierte Einstiegspunkt aufgerufen.

Andere Funktionen dürfen nicht direkt gestartet werden.

---

# 47. Rückgabewerte

Der Einstiegspunkt liefert einen standardisierten Rückgabecode.

Beispiele:

* SUCCESS
* FAILURE
* RETRY
* REBOOT_REQUIRED
* RECOVERY_REQUIRED

---

# 48. Modulzustände

Während seines Lebenszyklus kann sich ein Modul in folgenden Zuständen befinden:

```text id="b4tb1y"
FOUND

↓

VALIDATED

↓

LOADED

↓

INITIALIZED

↓

RUNNING

↓

FINISHED
```

---

# 49. Fehlerzustände

Zusätzliche Zustände:

* INVALID_SIGNATURE
* INVALID_VERSION
* INVALID_FORMAT
* DEPENDENCY_ERROR
* INTERNAL_ERROR

---

# 50. Modulbeendigung

Nach erfolgreicher Ausführung gibt das Modul die Kontrolle an den Boot Module Manager zurück.

Alternativ kann es die Kontrolle an ein anderes Modul (z. B. den Kernel Loader) übergeben, sofern dies Bestandteil des Protokolls ist.

---

# 51. Ressourcenverwaltung

Ressourcen werden ausschließlich lesend verwendet.

Eine Veränderung eingebetteter Ressourcen ist unzulässig.

---

# 52. Schreibbarer Speicher

Schreibbare Daten werden in separaten Speicherbereichen angelegt.

Dadurch bleibt der Codebereich unverändert.

---

# 53. Mehrfachstart

Ein Modul kann angeben, ob Mehrfachstarts zulässig sind.

Mögliche Werte:

* erlaubt
* nicht erlaubt
* nur nach Neustart

---

# 54. Modulparameter

Module können standardisierte Parameter empfangen.

Beispiele:

* Debugmodus
* Sprachkennung
* Recovery-Modus
* Bootoptionen

---

# 55. Modulkommunikation

Bootmodule kommunizieren nicht direkt miteinander.

Die Kommunikation erfolgt ausschließlich über:

* den Boot Module Manager,
* das Nova Boot ABI,
* oder definierte öffentliche Schnittstellen.

---

# 56. Fehlercodes

Standardisierte Fehlercodes:

* MODULE_NOT_FOUND
* INVALID_HEADER
* INVALID_SIGNATURE
* INVALID_VERSION
* OUT_OF_MEMORY
* DEPENDENCY_MISSING
* ENTRYPOINT_FAILED
* RESOURCE_ERROR

---

# 57. Protokollierung

Der Boot Module Manager protokolliert:

* Ladezeit
* Signaturstatus
* Fehler
* Modulversion
* Rückgabecode

Diese Informationen können vom Recovery-Modul ausgewertet werden.

---

# 58. Recovery-Integration

Schlägt das Laden eines Moduls fehl, kann abhängig vom Fehler:

* ein alternativer Start erfolgen,
* das Recovery-Modul geladen werden,
* oder der Bootvorgang sicher beendet werden.

---

# 59. Kompatibilität

Innerhalb derselben Major-Version müssen Module weiterhin lauffähig bleiben, sofern ausschließlich kompatible Erweiterungen vorgenommen wurden.

---

# 60. Erweiterungsmechanismus

Neue Headerfelder werden ausschließlich kompatibel ergänzt.

Bereits definierte Felder dürfen:

* ihre Bedeutung,
* Größe
* oder Position

nicht verändern.

---

# 61. Erweiterungsbereiche

Das Modulformat erlaubt optionale Erweiterungsbereiche.

Beispiele:

* Debuginformationen
* Performance-Metadaten
* Herstellerdaten
* experimentelle Funktionen

Unbekannte Erweiterungen müssen ignoriert werden können.

---

# 62. Debugmodule

Debugmodule besitzen dieselbe Struktur wie produktive Module.

Zusätzliche Diagnoseinformationen dürfen enthalten sein, beeinflussen jedoch nicht das eigentliche Ladeverfahren.

---

# 63. Teststrategie

Das Modulformat wird geprüft durch:

* Unit-Tests
* Integrations-Tests
* Signaturtests
* ABI-Tests
* Fuzzing
* Regressionstests

---

# 64. Referenzlayout

```text id="zj4l4k"
Header

↓

Metadata

↓

Executable Image

↓

Resources

↓

Extensions

↓

Signature Block
```

---

# 65. Referenzladeablauf

```text id="z6jgh9"
Header lesen

↓

Header validieren

↓

Version prüfen

↓

Hash berechnen

↓

Signatur prüfen

↓

Abhängigkeiten prüfen

↓

Speicher reservieren

↓

Image laden

↓

Relokation

↓

Initialisierung

↓

Start
```

---

# 66. Konformitätsanforderungen

Eine Implementierung erfüllt dieses RFC, wenn:

* jedes Modul einen gültigen Header besitzt,
* eine eindeutige Modul-ID vorhanden ist,
* Versionen korrekt angegeben werden,
* Signaturen erfolgreich geprüft werden,
* Abhängigkeiten validiert werden,
* der Einstiegspunkt eindeutig definiert ist,
* Ressourcen logisch getrennt gespeichert werden,
* und ausschließlich definierte Modulzustände verwendet werden.

---

# 67. Referenzimplementierung

Die Referenzimplementierung dient ausschließlich als Nachweis der Spezifikation.

Andere Implementierungen sind zulässig, sofern sämtliche Anforderungen dieses RFC erfüllen.

---

# 68. Zukunftserweiterungen

Das Modulformat ist auf langfristige Erweiterbarkeit ausgelegt.

Mögliche zukünftige Erweiterungen:

* komprimierte Module
* verschlüsselte Module
* Delta-Updates
* Modulcontainer
* Remote-Bootmodule
* optionale Debugsymbole
* mehrstufige Signaturen
* Hardware-spezifische Erweiterungen

---

# 69. Abwärtskompatibilität

Neue Minor-Versionen dürfen ausschließlich kompatible Erweiterungen enthalten.

Inkompatible Änderungen erfordern eine neue Major-Version.

---

# 70. Zusammenfassung

Das **Nova Boot Module Format (NBMF)** definiert ein standardisiertes, versioniertes und kryptografisch absicherbares Dateiformat für sämtliche Bootmodule innerhalb der NovaOS-Bootumgebung. Jedes Modul besteht aus einem selbstbeschreibenden Header, ausführbarem Code, optionalen Ressourcen, Erweiterungsbereichen und einem Signaturblock.

Durch standardisierte Modulzustände, definierte Ladealgorithmen, klare Abhängigkeitsregeln und eine konsequente Integritätsprüfung entsteht ein robustes und langfristig erweiterbares Modulformat. Der Boot Module Manager kann dadurch alle Bootmodule unabhängig von ihrer Funktion einheitlich validieren, laden und ausführen, wodurch die Modularität und Wartbarkeit der gesamten NovaOS-Bootarchitektur sichergestellt werden.
