
# NPSPEC-STUDIO-PROJECT-0001 – NovaLang Studio Project Management

## Status

Angenommen

## Kategorie

NovaLang Studio / Projektverwaltung

## Zweck

Definiert das Projektmodell und die Projektverwaltung von NovaLang Studio.

Ziel ist eine einheitliche Verwaltung von Quellcode, Abhängigkeiten, Ressourcen, Build-Konfigurationen und Entwicklungsartefakten.

Projekte bilden eigenständige Entwicklungseinheiten und können innerhalb eines Workspace gemeinsam verwaltet werden.

## Projektmodell

| Element | Beschreibung |
|---|---|
| Project | Eigenständige Entwicklungseinheit |
| Project Manifest | Identität und Konfiguration |
| Sources | NovaLang-Quelldateien |
| Resources | Bilder, Daten und weitere Ressourcen |
| Dependencies | Referenzierte Bibliotheken und Module |
| Build Configuration | Zielplattform und Compileroptionen |
| Artifacts | Erzeugte Build-Ergebnisse |

Ein Projekt kann unabhängig von einer Solution existieren.

## Projekttypen

NovaLang Studio unterstützt mindestens:

- NovaLang Application
- NovaLang Library
- NovaLang Module
- NovaLang Console Application
- NovaOS System Component
- NovaOS Driver
- NovaOS Solution Component

Weitere Projekttypen können über versionierte Erweiterungen ergänzt werden.

Eine NovaOS-Solution bleibt eine eigenständige Zusammenstellung aus Capabilities, Logic Graph und UI und darf nicht mit einem klassischen Programmprojekt gleichgesetzt werden.

## Projektstruktur

Beispiel:

```text
MeinProjekt/
├── project.xml
├── Sources/
│   ├── main.nova
│   └── helper.nova
├── Resources/
├── Tests/
└── Build/
```

Die physische Ordnerstruktur ist frei konfigurierbar.

Projektdateien können auch außerhalb des Projektordners referenziert werden, sofern entsprechende Zugriffsrechte bestehen.

## Projektmanifest

`project.xml` beschreibt die versionierte Projektkonfiguration.

Mindestens enthalten sind:

- Eindeutige Projekt-ID
- Projektname
- Manifestversion
- Projekttyp
- NovaLang-Sprachversion
- Zielarchitektur und Zielplattform
- Build-Konfigurationen
- Quelldateien und Ressourcen
- Projekt- und Bibliotheksabhängigkeiten

Die Projekt-ID dient der Entwicklungsorganisation und stellt keine Sicherheitsidentität dar.

Die GUID einer Solution wird weiterhin über deren `solution.xml` verwaltet.

## Project Manager

Der Project Manager übernimmt:

- Erstellen, Öffnen und Schließen von Projekten
- Verwaltung von Quelldateien und Ressourcen
- Hinzufügen und Entfernen von Abhängigkeiten
- Verwaltung von Build-Konfigurationen
- Prüfung von Projektverweisen
- Integration in Workspace und Language Service

Projektoperationen müssen über definierte Schnittstellen verfügbar sein.

## Abhängigkeitsverwaltung

Projekte können andere Projekte, Module und Bibliotheken referenzieren.

- Abhängigkeiten müssen eindeutig identifizierbar sein.
- Versionen und Kompatibilitätsanforderungen müssen überprüft werden.
- Zirkuläre Build-Abhängigkeiten müssen erkannt werden.
- Abhängigkeiten dürfen keine impliziten Capability-Berechtigungen erzeugen.
- Private Systemabhängigkeiten können über den NovaOS-`SYS`-Overlay-Mechanismus bereitgestellt werden.

Die tatsächliche Auflösung erfolgt durch die zuständigen Build- und Modulverwaltungskomponenten.

## Build-Konfigurationen

Ein Projekt kann mehrere Build-Konfigurationen besitzen.

Beispiele:

- Debug
- Release
- Test
- Deterministic

Konfigurierbar sind unter anderem:

- Zielarchitektur
- AOT- oder Bytecode-Ausgabe
- Optimierungsstufe
- Debug-Symbole
- Diagnostik- und Profilingoptionen
- Ressourcenanforderungen

Build-Konfigurationen dürfen die definierten Sicherheitsregeln nicht außer Kraft setzen.

## Projektvorlagen

NovaLang Studio stellt Projektvorlagen bereit.

Vorlagen können enthalten:

- Projektmanifest
- Grundlegende Quelldateien
- Beispielkonfigurationen
- Teststruktur
- Build-Einstellungen

Vorlagen müssen versioniert und erweiterbar sein.

## Integration mit Solutions

Ein Projekt kann Quellcode oder Komponenten für eine Solution bereitstellen.

Dabei gilt:

- Die Solution verwaltet ihre eigene Identität und Capability-Verträge.
- Projektabhängigkeiten erzeugen keine Berechtigungen.
- Custom Scripts verwenden exakt NovaLang.
- Logic Graph und UI Designer greifen auf dieselben Sprach- und Typinformationen zu.
- Die Einbindung eines Projekts darf die Sicherheitsgrenzen einer Solution nicht verändern.

## Performance und Zuverlässigkeit

- Projektmetadaten müssen schnell geladen werden können.
- Große Projekte sollen inkrementell analysiert werden.
- Unveränderte Build-Ergebnisse sollen wiederverwendet werden.
- Änderungen am Manifest müssen validiert werden.
- Fehlgeschlagene Projektoperationen dürfen keine beschädigten Konfigurationen hinterlassen.

## Normative Anforderungen

1. NovaLang Studio MUSS eigenständige Projekte unabhängig von Solutions unterstützen.
2. Jedes Projekt MUSS ein versioniertes Projektmanifest besitzen.
3. Projektidentitäten MÜSSEN eindeutig und von Solution-Sicherheitsidentitäten getrennt sein.
4. Quelldateien, Ressourcen und Abhängigkeiten MÜSSEN zentral verwaltbar sein.
5. Mehrere Build-Konfigurationen MÜSSEN unterstützt werden.
6. Projektabhängigkeiten MÜSSEN aufgelöst und auf Kompatibilität geprüft werden.
7. Zirkuläre Build-Abhängigkeiten MÜSSEN erkannt werden.
8. Projekte MÜSSEN in Workspaces integriert werden können.
9. Projektkonfigurationen DÜRFEN keine Capability-Berechtigungen erteilen.
10. Projektoperationen MÜSSEN konsistent und gegen Datenverlust abgesichert sein.
11. Projektvorlagen SOLLEN erweiterbar und versioniert sein.
12. Die Projektverwaltung MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine einheitliche, modulare Projektverwaltung für Anwendungen, Bibliotheken, Module und NovaOS-Komponenten mit versionierten Manifesten, kontrollierten Abhängigkeiten und integrierter Build-Verwaltung.
