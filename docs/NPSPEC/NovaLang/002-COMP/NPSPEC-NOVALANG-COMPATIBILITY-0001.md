
# NPSPEC-NOVALANG-COMPATIBILITY-0001 – NovaLang Compatibility

## Status

Angenommen

## Kategorie

NovaLang / Kompatibilität

## Zweck

Definiert die Kompatibilität von NovaLang zwischen Sprachversionen, Laufzeitumgebungen, Prozessorarchitekturen und NovaOS-Versionen.

Ziel sind langfristig funktionsfähige Programme, stabile Schnittstellen und kontrollierte Migrationen.

## Kompatibilitätsebenen

| Ebene | Beschreibung |
|---|---|
| Source | Kompatibilität des Quellcodes |
| Language | Syntax und Sprachsemantik |
| Binary | Binärformate und ABI |
| Runtime | Laufzeitfunktionen und Objektmodell |
| Library | Standardbibliothek und öffentliche APIs |
| Platform | NovaOS-Versionen und Zielarchitekturen |
| Capability | Versionierte Systemfähigkeiten und Verträge |

## Sprachkompatibilität

NovaLang orientiert sich syntaktisch an VB.NET, besitzt jedoch eine eigenständige Sprachdefinition.

- Vertraute VB.NET-Syntax soll möglichst erhalten bleiben.
- NovaLang garantiert keine vollständige VB.NET- oder .NET-Kompatibilität.
- Abweichungen müssen dokumentiert werden.
- Bestehende Sprachversionen dürfen nicht stillschweigend umgedeutet werden.
- Inkompatible Änderungen erfordern eine neue Major-Version.

## Binär- und Runtime-Kompatibilität

- AOT-Module müssen ihre Zielarchitektur und ABI-Version deklarieren.
- Nova Bytecode und Nova IR besitzen eigene Formatversionen.
- Die Runtime muss benötigte Funktionen und Versionen prüfen.
- Inkompatible Module dürfen nicht unkontrolliert geladen werden.
- Unterschiedliche Bibliotheksversionen dürfen parallel existieren, sofern ihre Isolation und Verträge dies erlauben.

## Plattformkompatibilität

NovaLang unterstützt mehrere Zielarchitekturen über getrennte Compiler-Backends.

Plattformabhängige Funktionen müssen über definierte Schnittstellen bereitgestellt werden.

Programme dürfen fehlende Plattformfunktionen erkennen und kontrolliert darauf reagieren.

## Capability-Kompatibilität

Capabilities besitzen stabile Identitäten nach dem Schema:

`domain.authority.namespace.name`

- Capability-Verträge müssen versioniert sein.
- Kompatible Erweiterungen dürfen bestehende Aufrufe nicht verändern.
- Inkompatible Schnittstellenänderungen benötigen eine neue Vertragsversion.
- Fehlende Capabilities müssen eindeutig diagnostiziert werden.
- Kompatibilitätsmechanismen dürfen keine Berechtigungen erweitern.

## Rückwärtskompatibilität

NovaLang soll ältere Programme möglichst ohne Quellcodeänderungen ausführen können.

Dazu können verwendet werden:

- Unterstützung älterer Sprachversionen
- Versionierte Runtime-Schnittstellen
- Kompatibilitätsadapter
- Parallel installierbare Bibliotheken
- Kontrollierte Migrationswerkzeuge

Kompatibilitätsadapter dürfen Sicherheits- und Speicherregeln nicht umgehen.

## NovaLang Studio

NovaLang Studio unterstützt:

- Prüfung von Sprach- und Runtime-Versionen
- Erkennung inkompatibler APIs
- Hinweise auf veraltete Funktionen
- Automatisierte Migrationen mit Änderungsprüfung
- Kompatibilitätsdiagnosen für Solutions und Logic Graph

## Normative Anforderungen

1. NovaLang MUSS Sprach-, Binär-, Runtime- und API-Kompatibilität getrennt behandeln.
2. Versionen und benötigte Laufzeitmerkmale MÜSSEN eindeutig deklariert werden.
3. Inkompatible Änderungen MÜSSEN erkannt und diagnostiziert werden.
4. Bestehende Sprachsemantik DARF innerhalb einer kompatiblen Version nicht verändert werden.
5. Capability-Verträge MÜSSEN versioniert und sicher aufgelöst werden.
6. Kompatibilitätsmechanismen DÜRFEN Sicherheits- und Berechtigungsgrenzen nicht umgehen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN demselben Sprachkompatibilitätsmodell folgen.
8. NovaLang Studio SOLL Migrationen und Kompatibilitätsprüfungen unterstützen.

## Ergebnis

NovaLang erhält ein langfristig stabiles Kompatibilitätsmodell für Quellcode, Binärdateien, Laufzeitumgebungen und NovaOS-Capabilities, ohne die Weiterentwicklung der Sprache oder die Systemsicherheit einzuschränken.
