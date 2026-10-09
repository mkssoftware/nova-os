
# NPSPEC-STUDIO-PREVIEW-0001 – NovaLang Studio Preview

## Status

Angenommen

## Kategorie

NovaLang Studio / Vorschau / Entwicklungswerkzeuge

## Zweck

Definiert die integrierte Echtzeitvorschau für NovaOS-Benutzeroberflächen und Solutions.

Ziel ist die unmittelbare Darstellung und Prüfung von Änderungen, ohne die vollständige Anwendung neu starten zu müssen.

## Architektur

Die Preview Engine besteht aus:

- **Preview Manager:** Verwaltung der Vorschau-Sitzungen.
- **Preview Renderer:** Darstellung über den NovaOS-UI-Renderer.
- **State Manager:** Verwaltung des Vorschauzustands.
- **Hot Reload Engine:** Übernahme unterstützter Änderungen.
- **Interaction Simulator:** Simulation von Benutzerinteraktionen.
- **Runtime Bridge:** Verbindung zu Logic Graph und NovaLang Runtime.

Die Vorschau verwendet dieselben UI-Verträge und Rendering-Regeln wie die produktive Ausführung.

## Funktionen

- Live Preview von `.nui`-Oberflächen
- Vorschau vollständiger Solutions
- Hot Reload bei Änderungen
- Simulation von Benutzerinteraktionen
- Darstellung verschiedener Fenstergrößen und Skalierungen
- Vorschau von Themes und Zuständen
- Anzeige von Bindungs- und Laufzeitfehlern
- Verwendung definierter Testdaten

## Hot Reload

Unterstützte Änderungen werden inkrementell übernommen.

Der bestehende UI-Zustand soll nach Möglichkeit erhalten bleiben.

Nicht kompatible Änderungen müssen einen kontrollierten Neustart der Vorschau auslösen oder ausdrücklich zur Bestätigung angeboten werden.

Fehlerhafte Änderungen dürfen die letzte gültige Vorschau nicht unkontrolliert beschädigen.

## Solution-Integration

Die Vorschau kann Logic-Graph-Knoten und Custom Scripts ausführen.

Externe Capabilities werden standardmäßig durch kontrollierte Testimplementierungen ersetzt oder deaktiviert.

Echte Systemzugriffe benötigen eine ausdrückliche Autorisierung und bleiben an die verifizierte Solution-Identität gebunden.

## Normative Anforderungen

1. NovaLang Studio MUSS eine integrierte Preview Engine bereitstellen.
2. `.nui`-Oberflächen und Solutions MÜSSEN darstellbar sein.
3. Die Vorschau MUSS denselben UI-Renderer und dieselbe deklarative Semantik wie NovaOS verwenden.
4. Unterstützte Änderungen MÜSSEN ohne vollständigen Neustart übernommen werden können.
5. UI-Zustände SOLLEN bei Hot Reload erhalten bleiben.
6. Nicht kompatible Änderungen MÜSSEN kontrolliert behandelt werden.
7. Benutzerinteraktionen und Datenbindungen MÜSSEN testbar sein.
8. Vorschau-Sitzungen MÜSSEN isoliert und kontrolliert beendbar sein.
9. Externe Capability-Zugriffe DÜRFEN nicht ohne Autorisierung erfolgen.
10. Fehler MÜSSEN an das zentrale Diagnosesystem weitergeleitet werden.
11. Die Preview Engine MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine schnelle, interaktive und sichere Echtzeitvorschau für Benutzeroberflächen und Solutions mit Hot Reload und direkter Integration in UI Designer und Logic Graph.
