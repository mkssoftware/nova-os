
# NPSPEC-STUDIO-PACKAGING-0001 – NovaLang Studio Packaging

## Status

Angenommen

## Kategorie

NovaLang Studio / Build / Paketierung

## Zweck

Definiert die Erstellung installierbarer und portabler Pakete für NovaLang-Programme und NovaOS-Solutions.

Ziel ist die zuverlässige Bereitstellung vollständiger Anwendungen mit überprüfbarer Integrität, klaren Abhängigkeiten und minimalem Ressourcenverbrauch.

## Architektur

Das Packaging-System besteht aus:

- **Package Manager:** Steuerung der Paketerstellung.
- **Package Builder:** Zusammenstellung der Artefakte.
- **Dependency Resolver:** Ermittlung benötigter Abhängigkeiten.
- **Manifest Generator:** Erstellung der Paketmetadaten.
- **Package Validator:** Prüfung von Struktur und Integrität.
- **Signing Integration:** Anbindung autorisierter Signaturverfahren.

Die Paketierung verwendet die Artefakte des Build Managers.

## Pakettypen

Unterstützt werden:

- Klassische NovaLang-Programme
- NovaOS-Solutions
- Bibliotheken und Module
- Studio-Erweiterungen

Pakete können abhängig vom Zielsystem installierbar oder portabel bereitgestellt werden.

## Paketinhalt

Ein Paket enthält die benötigten Bestandteile:

- Ausführbare Artefakte
- Ressourcen
- Abhängigkeiten
- Versionsinformationen
- Paketmanifest
- Integritätsinformationen
- Deklarierte Capability-Anforderungen

Nicht benötigte Entwicklungsdateien sollen ausgeschlossen werden.

## Solution-Packaging

NovaOS-Solutions enthalten:

- `solution.xml` mit dauerhafter GUID
- Logic Graph
- Custom-NovaLang-Skripte beziehungsweise kompilierte Artefakte
- Deklarative `.nui`-Oberflächen
- Benötigte Ressourcen
- Capability-Verträge und Anforderungen

Die Solution-Identität muss erhalten bleiben.

Eine Paketänderung darf keine bestehende Berechtigung ungeprüft übernehmen.

## Abhängigkeiten

Abhängigkeiten können paketintern bereitgestellt oder über versionierte Systemkomponenten aufgelöst werden.

Private Systemabhängigkeiten dürfen über den vorgesehenen `SYS`-Overlay-Mechanismus eingebunden werden.

Globale Systemänderungen benötigen gesonderte Berechtigungen.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Package Manager bereitstellen.
2. Programme, Solutions, Bibliotheken und Erweiterungen MÜSSEN paketierbar sein.
3. Pakete MÜSSEN ein versioniertes Manifest besitzen.
4. Abhängigkeiten MÜSSEN automatisch ermittelt und validiert werden.
5. Paketstruktur und Integrität MÜSSEN überprüfbar sein.
6. Reproduzierbare Paketierung MUSS bei deterministischen Build-Eingaben unterstützt werden.
7. Die dauerhafte Solution-GUID MUSS erhalten bleiben.
8. Capability-Anforderungen MÜSSEN deklarativ enthalten sein.
9. Paketierung DARF keine Berechtigungen erteilen oder Sicherheitsgrenzen umgehen.
10. Signaturen MÜSSEN über autorisierte Verfahren erstellt und geprüft werden können.
11. Die Paketierung MUSS auch ohne grafische Studio-Oberfläche funktionieren.
12. Das Packaging-System MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine einheitliche Paketierungsinfrastruktur zur sicheren, reproduzierbaren und portablen Bereitstellung von NovaLang-Programmen und NovaOS-Solutions.
