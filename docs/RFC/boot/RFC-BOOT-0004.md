# RFC-BOOT-0004: Nova Boot Configuration Format (NBCF) (Teil 1)

* **Dokumenttyp:** Request for Comments (RFC)
* **Dokument-ID:** RFC-BOOT-0004
* **Titel:** Nova Boot Configuration Format (NBCF)
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Protocol / Konfiguration
* **Gültigkeitsbereich:** Stage-2, Boot Module Manager, Kernel Loader, Recovery, Bootmenü
* **Abhängigkeiten:** RFC-BOOT-0001, RFC-BOOT-0002, RFC-BOOT-0003, ADR-BOOT-1012, ADR-BOOT-1020
* **Letzte Aktualisierung:** 2026-07-29

---

# Inhaltsverzeichnis

1. Einleitung
2. Motivation
3. Ziele
4. Nicht-Ziele
5. Architektur
6. Designprinzipien
7. Konfigurationsmodell
8. Dateiformat
9. Versionsverwaltung
10. Bootprofile
11. Parameter
12. Validierung
13. Sicherheit
14. Fehlerbehandlung

---

# 1. Einleitung

Dieses Dokument definiert das **Nova Boot Configuration Format (NBCF)**.

Das NBCF beschreibt das standardisierte Konfigurationsformat der NovaOS-Bootumgebung.

Es legt fest, wie Bootprofile, Bootparameter und Konfigurationen gespeichert, gelesen und validiert werden.

---

# 2. Motivation

Eine moderne Bootumgebung benötigt konfigurierbare Eigenschaften.

Beispiele:

* Standardbetriebssystem
* Bootmenü
* Zeitüberschreitung
* Recoveryoptionen
* Debugmodus
* Grafikmodus
* Kernelparameter
* Sicherheitsoptionen

Diese Einstellungen dürfen nicht hart im Bootloader implementiert sein.

---

# 3. Ziele

Das NBCF verfolgt folgende Ziele:

* standardisierte Konfiguration
* einfache Erweiterbarkeit
* Menschenlesbarkeit
* Versionierung
* Validierung
* Mehrprofil-Unterstützung
* Plattformunabhängigkeit

---

# 4. Nicht-Ziele

Das NBCF definiert nicht:

* BootFS
* Kernelparameterformat
* NovaFS
* Benutzerkonfigurationen
* Desktopeinstellungen

Es beschreibt ausschließlich die Bootkonfiguration.

---

# 5. Architekturübersicht

```text id="6g1s9t"
BootFS

↓

boot.cfg

↓

Stage-2

↓

Boot Module Manager

↓

Kernel Loader
```

---

# 6. Grundprinzipien

Das NBCF basiert auf folgenden Prinzipien:

* Klar lesbar
* Deterministisch
* Versioniert
* Erweiterbar
* Robust
* Fehlertolerant

---

# 7. Konfigurationsmodell

Die Bootkonfiguration besteht aus:

* globalen Einstellungen
* Bootprofilen
* Modulkonfigurationen
* Sicherheitsoptionen

---

# 8. Dateiname

Standarddatei:

```text id="f9f6vw"
boot.cfg
```

Alternative Namen können implementierungsabhängig unterstützt werden.

---

# 9. Speicherort

Die Konfigurationsdatei befindet sich im BootFS.

Empfohlener Pfad:

```text id="ld9knl"
/boot/boot.cfg
```

---

# 10. Dateikodierung

Die Konfiguration wird in UTF-8 gespeichert.

Eine Byte Order Mark (BOM) wird nicht verwendet.

---

# 11. Zeilenenden

Empfohlen wird:

LF

Andere Zeilenenden dürfen toleriert werden.

---

# 12. Dateiformat

Das NBCF verwendet ein textbasiertes Schlüssel-Wert-Format.

Beispiel:

```text id="18d6lc"
timeout = 5

default = nova
```

Die genaue Grammatik wird in einem späteren Abschnitt formal definiert.

---

# 13. Kommentare

Kommentare beginnen mit:

```text id="g6g8yh"
#
```

Beispiel:

```text id="ccj3yl"
# Nova Boot Configuration
timeout = 5
```

Kommentare beeinflussen die Verarbeitung nicht.

---

# 14. Leerzeichen

Zusätzliche Leerzeichen dürfen ignoriert werden.

Die Bedeutung einer Konfiguration darf sich dadurch nicht ändern.

---

# 15. Versionierung

Jede Konfigurationsdatei enthält eine Formatversion.

Beispiel:

```text id="i8yn7o"
format = 1.0
```

---

# 16. Globale Einstellungen

Globale Einstellungen gelten für sämtliche Bootprofile.

Beispiele:

* Timeout
* Sprache
* Standardprofil
* Menümodus
* Theme

---

# 17. Timeout

Der Timeout beschreibt die Wartezeit vor dem automatischen Start.

Beispiel:

```text id="cl4cxw"
timeout = 5
```

---

# 18. Standardsystem

Das Standardprofil bestimmt den automatischen Bootvorgang.

Beispiel:

```text id="vjlwmg"
default = nova
```

---

# 19. Sprache

Die Bootumgebung kann eine Standardsprache definieren.

Beispiel:

```text id="e8lyb9"
language = de-DE
```

---

# 20. Theme

Optional kann ein Boot-Theme gewählt werden.

Beispiel:

```text id="7t2rmp"
theme = default
```

---

# 21. Bootprofile

Ein Bootprofil beschreibt einen vollständigen Starteintrag.

Jedes Profil besitzt einen eindeutigen Namen.

---

# 22. Beispielprofil

```text id="72em6w"
profile nova
{
}
```

---

# 23. Profilname

Profilnamen müssen eindeutig sein.

Groß- und Kleinschreibung wird unterschieden.

---

# 24. Profilparameter

Ein Profil kann enthalten:

* Kernel
* Bootmodule
* Kernelparameter
* Recoveryoptionen
* Grafikmodus

---

# 25. Kerneldatei

Beispiel:

```text id="cr8mm3"
kernel = kernel.bin
```

---

# 26. Bootmodule

Mehrere Module können definiert werden.

Beispiel:

```text id="trpqzc"
module = recovery.nbm

module = memtest.nbm
```

---

# 27. Kernelparameter

Kernelparameter werden explizit angegeben.

Beispiel:

```text id="b1gc0q"
cmdline = quiet splash
```

---

# 28. Grafikmodus

Ein Profil kann einen gewünschten Grafikmodus definieren.

Beispiel:

```text id="wqqx2h"
video = auto
```

---

# 29. Recoveryoption

Beispiel:

```text id="v4q2tx"
recovery = enabled
```

---

# 30. Debugmodus

Beispiel:

```text id="wjl1xg"
debug = false
```

---

# 31. Signaturprüfung

Die Signaturprüfung kann nicht deaktiviert werden.

Konfigurationen dürfen diese Sicherheitsfunktion nicht umgehen.

---

# 32. Sicherheitsoptionen

Konfigurierbar sind beispielsweise:

* Verbose Logging
* Recovery automatisch
* Diagnose erlauben

Nicht konfigurierbar sind sicherheitskritische Kernfunktionen.

---

# 33. Validierung

Vor der Nutzung überprüft Stage-2 mindestens:

* Syntax
* Formatversion
* Pflichtfelder
* Profilnamen
* Wertebereiche

---

# 34. Fehlerbehandlung

Ungültige Konfigurationen führen nicht zu undefiniertem Verhalten.

Mögliche Reaktionen:

* Standardwerte verwenden
* Benutzer informieren
* Recovery starten

---

# 35. Qualitätsanforderungen

Das Nova Boot Configuration Format muss:

* leicht lesbar,
* versioniert,
* eindeutig,
* validierbar,
* deterministisch,
* und langfristig erweiterbar

sein.

---

# 36. Zusammenfassung Teil 1

Das **Nova Boot Configuration Format (NBCF)** definiert ein standardisiertes, menschenlesbares und versioniertes Konfigurationsformat für die NovaOS-Bootumgebung. Es trennt Bootlogik und Konfiguration konsequent voneinander und ermöglicht die Verwaltung globaler Einstellungen sowie mehrerer Bootprofile über eine klar strukturierte Textdatei.




# RFC-BOOT-0004: Nova Boot Configuration Format (NBCF) (Teil 2)

---

# 37. Formale Grammatik

Die NBCF-Datei folgt einer eindeutig definierten Grammatik.

Ein Parser muss dieselbe Datei auf jeder Plattform identisch interpretieren.

Vereinfachte Struktur:

```text
configuration

↓

global settings

↓

profiles

↓

optional extensions
```

---

# 38. Datentypen

Folgende Datentypen sind definiert:

* Boolean
* Integer
* String
* Identifier
* Liste
* Pfad

Andere Datentypen dürfen ausschließlich über zukünftige Erweiterungen eingeführt werden.

---

# 39. Boolean

Erlaubte Werte:

```text
true
false
```

Andere Schreibweisen sind unzulässig.

---

# 40. Integer

Ganzzahlen werden dezimal gespeichert.

Beispiel:

```text
timeout = 10
```

Negative Werte sind nur zulässig, wenn das jeweilige Feld diese ausdrücklich erlaubt.

---

# 41. Strings

Strings werden in Anführungszeichen gespeichert.

Beispiel:

```text
title = "NovaOS"
```

UTF-8 ist verpflichtend.

---

# 42. Identifier

Identifier bestehen ausschließlich aus:

* Buchstaben
* Ziffern
* Unterstrich
* Bindestrich

Leerzeichen sind nicht zulässig.

---

# 43. Listen

Mehrere Werte können als Liste angegeben werden.

Beispiel:

```text
modules =
[
    "recovery.nbm",
    "diagnose.nbm",
    "kernel_loader.nbm"
]
```

Die Reihenfolge der Elemente bleibt erhalten.

---

# 44. Pfadangaben

Pfade werden grundsätzlich relativ zum BootFS interpretiert.

Beispiel:

```text
kernel = "/kernel/kernel.bin"
```

Absolute Host-Dateisystempfade sind nicht zulässig.

---

# 45. Profilblöcke

Jedes Profil beginnt mit einer eindeutigen Deklaration.

Beispiel:

```text
profile "NovaOS"
{
    ...
}
```

---

# 46. Pflichtfelder eines Profils

Mindestens erforderlich:

* Name
* Kernel
* Version

Weitere Felder können optional sein.

---

# 47. Optionale Profilfelder

Beispiele:

* Beschreibung
* Symbol
* Theme
* Sprache
* Debugmodus
* Recoverymodus

---

# 48. Modulkonfiguration

Module können innerhalb eines Profils konfiguriert werden.

Beispiel:

```text
module "recovery"
{
    enabled = true
}
```

---

# 49. Modulparameter

Jedes Modul darf eigene Parameter definieren.

Diese werden ausschließlich vom jeweiligen Modul ausgewertet.

---

# 50. Profilvererbung

Profile können Eigenschaften anderer Profile übernehmen.

Beispiel:

```text
profile "Debug"

inherits "NovaOS"
{
    debug = true
}
```

Mehrfachvererbung ist nicht zulässig.

---

# 51. Überschreiben von Eigenschaften

Ein erbendes Profil darf geerbte Werte ersetzen.

Nicht definierte Eigenschaften werden übernommen.

---

# 52. Erweiterungsbereiche

Das NBCF erlaubt definierte Erweiterungsbereiche.

Beispiel:

```text
extension "vendor"
{
}
```

Unbekannte Erweiterungen müssen ignoriert werden.

---

# 53. Parseranforderungen

Ein NBCF-Parser muss:

* deterministisch arbeiten,
* fehlerhafte Eingaben erkennen,
* gültige Standardwerte anwenden,
* unbekannte Erweiterungen überspringen.

---

# 54. Syntaxfehler

Beispiele:

* fehlende Klammern
* doppelte Profilnamen
* ungültige Datentypen
* unbekannte Pflichtfelder
* beschädigte Zeichenkodierung

---

# 55. Semantische Fehler

Beispiele:

* Kerneldatei existiert nicht
* Modul fehlt
* ungültige Version
* mehrfach definiertes Standardprofil

---

# 56. Standardwerte

Fehlende optionale Werte werden durch definierte Standardwerte ersetzt.

Beispiel:

```text
debug = false
```

---

# 57. Ladealgorithmus

```text
boot.cfg lesen

↓

UTF-8 prüfen

↓

Syntax prüfen

↓

Version prüfen

↓

Globale Einstellungen laden

↓

Profile laden

↓

Module validieren

↓

Standardprofil bestimmen
```

---

# 58. Profilauswahl

Das aktive Profil wird bestimmt durch:

1. Benutzerauswahl
2. Recovery-Regeln
3. Standardprofil

---

# 59. Fehlerstrategie

Bei Konfigurationsfehlern gilt folgende Priorität:

1. Standardwert verwenden
2. Benutzer informieren
3. Recovery anbieten
4. Boot abbrechen

---

# 60. Sicherheitsregeln

Konfigurationsdateien dürfen nicht:

* Signaturen deaktivieren
* Sicherheitsprüfungen umgehen
* Trust-Informationen verändern
* Boot-ABI manipulieren

Diese Eigenschaften sind ausschließlich durch den Bootloader festgelegt.

---

# 61. Integritätsprüfung

Optional kann die Konfigurationsdatei zusätzlich signiert werden.

Vor der Verarbeitung erfolgt:

* Hashprüfung
* Signaturprüfung
* Versionsprüfung

---

# 62. Protokollierung

Der Parser kann folgende Ereignisse protokollieren:

* Syntaxfehler
* Warnungen
* verwendetes Profil
* ersetzte Standardwerte
* unbekannte Erweiterungen

---

# 63. Recovery

Ist die Konfiguration unlesbar oder ungültig, darf automatisch ein sicheres Standardprofil oder das Recovery-Modul gestartet werden.

---

# 64. Internationalisierung

Alle Texte innerhalb der Konfiguration werden als UTF-8 gespeichert.

Sprachabhängige Inhalte können über separate Sprachdateien ergänzt werden.

---

# 65. Referenzstruktur

```text
boot.cfg

↓

Globale Einstellungen

↓

Bootprofile

↓

Modulkonfiguration

↓

Erweiterungen
```

---

# 66. Referenzbeispiel

```text
format = 1.0

timeout = 5

default = "NovaOS"

profile "NovaOS"
{
    kernel = "/kernel/kernel.bin"

    cmdline = "quiet splash"

    module = "kernel_loader.nbm"

    module = "recovery.nbm"
}
```

---

# 67. Teststrategie

Das NBCF wird getestet durch:

* Parsertests
* Syntaxtests
* Regressionstests
* Fuzzing
* Kompatibilitätstests
* Negativtests

---

# 68. Konformitätsanforderungen

Eine Implementierung erfüllt dieses RFC, wenn:

* die NBCF-Datei gemäß dieser Spezifikation verarbeitet wird,
* UTF-8 unterstützt wird,
* globale Einstellungen und Profile korrekt ausgewertet werden,
* Standardwerte definiert sind,
* unbekannte Erweiterungen ignoriert werden,
* sicherheitsrelevante Einstellungen nicht über die Konfiguration außer Kraft gesetzt werden,
* und Konfigurationsfehler kontrolliert behandelt werden.

---

# 69. Referenzimplementierung

Die Referenzimplementierung dient als Nachweis der Spezifikation.

Andere Implementierungen sind zulässig, sofern sie alle Anforderungen dieses RFC erfüllen.

---

# 70. Zukunftserweiterungen

Das Nova Boot Configuration Format ist ausdrücklich auf zukünftige Erweiterungen ausgelegt.

Mögliche Erweiterungen:

* grafische Bootprofile
* Mehrsprachigkeit
* Hardwareprofile
* signierte Profilpakete
* modulabhängige Konfigurationsdateien
* bedingte Profile
* Konfigurationsvorlagen
* automatische Profilmigration

---

# 71. Abwärtskompatibilität

Neue Minor-Versionen dürfen ausschließlich kompatible Erweiterungen enthalten.

Änderungen an Pflichtfeldern oder der Grammatik erfordern eine neue Major-Version.

---

# 72. Referenzarchitektur

```text
BootFS

↓

boot.cfg

↓

NBCF Parser

↓

Konfigurationsmodell

↓

Boot Module Manager

↓

Kernel Loader
```

---

# 73. Zusammenfassung

Das **Nova Boot Configuration Format (NBCF)** definiert ein standardisiertes, textbasiertes und versioniertes Konfigurationsformat für die gesamte NovaOS-Bootumgebung. Es beschreibt globale Einstellungen, Bootprofile, Modulkonfigurationen und optionale Erweiterungen in einer klar strukturierten und deterministisch interpretierbaren Form.

Durch eine formale Grammatik, standardisierte Datentypen, definierte Parserregeln und eine strikte Trennung zwischen Konfiguration und sicherheitskritischer Bootlogik ermöglicht das NBCF eine robuste, leicht wartbare und langfristig erweiterbare Verwaltung des Bootverhaltens von NovaOS. Es bildet damit die zentrale Grundlage für alle benutzer- und systemseitigen Einstellungen innerhalb des Nova Boot Protocols.
