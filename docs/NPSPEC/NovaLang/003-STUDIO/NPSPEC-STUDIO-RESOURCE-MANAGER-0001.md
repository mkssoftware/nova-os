
# NPSPEC-STUDIO-RESOURCE-MANAGER-0001 – NovaLang Studio Resource Manager

## Status

Angenommen

## Kategorie

NovaLang Studio / Ressourcenverwaltung

## Zweck

Definiert die zentrale Verwaltung von Ressourcen innerhalb von NovaLang-Projekten und NovaOS-Solutions.

Ziel ist die einfache Organisation, Bearbeitung und Einbindung von Dateien und Medien, ohne Ressourcen unnötig zu duplizieren.

## Architektur

Der Resource Manager besteht aus:

- **Resource Explorer:** Übersicht und Organisation von Ressourcen.
- **Resource Registry:** Verwaltung von Identitäten und Metadaten.
- **Resource Resolver:** Auflösung von Ressourcenreferenzen.
- **Resource Preview:** Vorschau unterstützter Dateitypen.
- **Resource Validator:** Prüfung von Integrität und Kompatibilität.
- **Build Integration:** Bereitstellung benötigter Ressourcen für Build-Artefakte.

## Unterstützte Ressourcen

- Bilder, Icons und SVG-Dateien
- Schriftarten
- Audio- und Videodateien
- UI-Styles und Themes
- Lokalisierungsdateien
- Konfigurationsdateien
- Binäre und benutzerdefinierte Ressourcen

Ressourcentypen müssen erweiterbar sein.

## Ressourcenmodell

Jede Ressource besitzt:

- Eindeutige Identität innerhalb ihres Gültigkeitsbereichs
- Typ und Speicherort
- Optionale Metadaten
- Referenzinformationen
- Integritätsstatus

Ressourcen können projektübergreifend referenziert werden, sofern entsprechende Zugriffsrechte bestehen.

Eine Referenz darf nicht automatisch eine physische Kopie erzeugen.

## Studio-Integration

Der Resource Manager unterstützt:

- Importieren und Entfernen von Ressourcen
- Umbenennen und Verschieben
- Suche und Filterung
- Vorschau
- Erkennung fehlender Referenzen
- Verwendung im UI Designer und Logic Graph
- Aktualisierung betroffener Referenzen

Änderungen müssen mit dem Workspace- und Projektmodell synchronisiert werden.

## Build-Integration

Der Build Manager ermittelt die tatsächlich benötigten Ressourcen.

Nicht verwendete Ressourcen sollen nicht unnötig in Build-Artefakte übernommen werden.

Ressourcen müssen bei der Erstellung von Solutions korrekt aufgelöst und auf Integrität geprüft werden.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Resource Manager bereitstellen.
2. Ressourcen MÜSSEN eindeutig identifizierbar und referenzierbar sein.
3. Import, Organisation, Suche und Vorschau MÜSSEN unterstützt werden.
4. Ressourcenreferenzen MÜSSEN projekt- und solutionübergreifend auflösbar sein, soweit autorisiert.
5. Umbenennen und Verschieben MÜSSEN betroffene Referenzen berücksichtigen.
6. Fehlende oder inkompatible Ressourcen MÜSSEN diagnostiziert werden.
7. UI Designer, Logic Graph und Build Manager MÜSSEN dieselbe Ressourcenverwaltung verwenden.
8. Unnötige Ressourcenduplikate SOLLEN vermieden werden.
9. Ressourcenoperationen MÜSSEN die NovaOS-Capability- und Zugriffsregeln einhalten.
10. Der Resource Manager MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine einheitliche, effiziente Ressourcenverwaltung für Programme und Solutions mit konsistenten Referenzen, direkter Vorschau und nahtloser Build-Integration.
