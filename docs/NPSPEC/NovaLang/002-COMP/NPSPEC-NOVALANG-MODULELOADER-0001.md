
# NPSPEC-NOVALANG-MODULELOADER-0001 – NovaLang Module Loader

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Modulverwaltung

## Zweck

Definiert das sichere Laden, Verknüpfen und Verwalten von NovaLang-Modulen zur Laufzeit.

Ziel sind kurze Ladezeiten, geringer Speicherverbrauch, kontrollierte Abhängigkeiten und die Integration in das NovaOS-Capability-System.

## Architektur

Der Module Loader ist Bestandteil der NovaLang Runtime.

| Komponente | Aufgabe |
|---|---|
| Module Resolver | Auflösung von Modulidentitäten |
| Dependency Resolver | Prüfung und Auflösung von Abhängigkeiten |
| Format Loader | Verarbeitung nativer Module und Bytecode |
| Symbol Resolver | Verknüpfung öffentlicher Symbole |
| Version Validator | Prüfung von ABI und Kompatibilität |
| Security Validator | Prüfung von Integrität und Berechtigungen |
| Lifecycle Manager | Initialisierung und Freigabe |

## Unterstützte Module

- Native AOT-Bibliotheken
- Nova-Bytecode-Module
- NovaLang-Standardbibliotheken
- Solution-Komponenten
- Autorisierte native Fremdbibliotheken über FFI

Ein `Imports` stellt lediglich Namen bereit und löst keine automatische Modulausführung aus.

## Ladevorgang

1. Modulidentität und Version auflösen.
2. Integrität, Herkunft und Ladeberechtigung prüfen.
3. Abhängigkeiten und Versionskonflikte prüfen.
4. Binärformat beziehungsweise Bytecode validieren.
5. Modul in den vorgesehenen Schutzkontext laden.
6. Symbole und Runtime-Schnittstellen verknüpfen.
7. Modul kontrolliert initialisieren.
8. Modul für autorisierte Aufrufe bereitstellen.

Fehler müssen den Ladevorgang kontrolliert abbrechen.

## Modulidentität und Versionierung

Jedes Modul besitzt:

- Eindeutige Modulidentität
- Modulversion
- ABI- beziehungsweise Bytecode-Version
- Deklarierte Abhängigkeiten
- Öffentliche Schnittstellen
- Integritätsinformationen

Mehrere Versionen dürfen parallel geladen werden, sofern ihre Namensräume und Laufzeitverträge konfliktfrei getrennt sind.

## Ladeverhalten

- Module sollen bei Bedarf geladen werden (Lazy Loading).
- Bereits geladene kompatible Module dürfen wiederverwendet werden.
- Zirkuläre Abhängigkeiten müssen erkannt und nach definierten Regeln behandelt werden.
- Initialisierung erfolgt in einer deterministisch festgelegten Reihenfolge.
- Fehlgeschlagene Initialisierung darf keinen teilweise freigegebenen Modulzustand hinterlassen.
- Entladen ist nur zulässig, wenn keine aktiven Aufrufe oder gültigen Abhängigkeiten verletzt werden.

## Sicherheit und Isolation

- Module dürfen nur innerhalb autorisierter Ausführungskontexte geladen werden.
- Bytecode muss vor der Ausführung verifiziert werden.
- Native Module benötigen geeignete Vertrauens- und Isolationsprüfungen.
- Laden und Verknüpfen dürfen keine zusätzlichen Capability-Berechtigungen erzeugen.
- Capability-Handles dürfen ausschließlich über autorisierte Mechanismen übergeben werden.
- Logic-Graph-Custom-Scripts dürfen keine eigenständigen nativen Module nachladen.

## NovaOS-Integration

Der Module Loader berücksichtigt das private `SYS`-Overlay einer Anwendung.

Private Abhängigkeiten dürfen logisch im System-Namespace erscheinen, bleiben jedoch physisch dem jeweiligen Programm zugeordnet.

Ein privates Overlay darf das globale `/System` nicht verändern oder Berechtigungsgrenzen umgehen.

## Normative Anforderungen

1. NovaLang MUSS einen nativen Module Loader bereitstellen.
2. Modulidentität, Version und Abhängigkeiten MÜSSEN vor dem Laden geprüft werden.
3. Bytecode-Module MÜSSEN vor der Ausführung verifiziert werden.
4. Native Module MÜSSEN die NovaOS-Isolationsregeln einhalten.
5. Modulinitialisierung und -freigabe MÜSSEN kontrolliert erfolgen.
6. Versionskonflikte und Abhängigkeitszyklen MÜSSEN erkannt werden.
7. Lazy Loading und Modulwiederverwendung SOLLEN unterstützt werden.
8. Module DÜRFEN keine Capability-Berechtigungen durch das Laden erhalten.
9. Private `SYS`-Overlays MÜSSEN vom globalen System-Namespace isoliert bleiben.
10. Der Module Loader DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält einen modularen, versionierten und sicherheitsbewussten Module Loader für native Bibliotheken, Bytecode und Solution-Komponenten mit Lazy Loading, kontrollierter Initialisierung und Unterstützung privater NovaOS-System-Overlays.
