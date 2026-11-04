# NPSPEC-SHADER-CACHE-0001 – Nova Shader Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Shader System

## 1. Zweck

Nova Shader Caching definiert die Zwischenspeicherung und Wiederverwendung von Shader-Zwischendarstellungen, kompilierten Shadern und GPU-Pipelines.

Ziel ist die Vermeidung unnötiger Kompilierungen, die Reduzierung von Ladezeiten und die Minimierung shaderbedingter Unterbrechungen während des Renderings.

Das Caching MUSS unabhängig von einer konkreten Grafik-API funktionieren und mit Nova Shader Compilation sowie Nova Shader Pipeline zusammenarbeiten.

## 2. Architekturprinzipien

- **Cache-First:** Gültige Kompilierungsergebnisse werden bevorzugt wiederverwendet.
- **Content-Addressed:** Shader-Inhalte bilden die Grundlage der Cache-Identifikation.
- **Versioned:** Compiler-, Shader- und Backend-Versionen werden berücksichtigt.
- **Multi-Level:** Unterschiedliche Kompilierungsstufen besitzen getrennte Cache-Ebenen.
- **Persistent:** Geeignete Cache-Einträge können dauerhaft gespeichert werden.
- **Asynchronous:** Cache-Zugriffe und Speicheroperationen erfolgen möglichst ohne Blockierung.
- **Budget-Aware:** Speicherverbrauch wird kontrolliert.
- **Deterministic:** Cache-Treffer verändern nicht die definierten Shader-Semantiken.

## 3. Cache Architecture

NovaOS unterscheidet folgende Cache-Ebenen:

| Cache | Verantwortung |
|---|---|
| Source Cache | Wiederverwendung aufbereiteter Shader-Quellen |
| IR Cache | Speicherung validierter Shader-Zwischendarstellungen |
| Compiled Shader Cache | Speicherung backendabhängiger Shader-Ergebnisse |
| Pipeline Cache | Wiederverwendung ausführbarer Pipeline-Konfigurationen |
| Persistent Cache | Dauerhafte Speicherung geeigneter Cache-Artefakte |

Die Cache-Ebenen MÜSSEN logisch getrennt bleiben.

Nova Shader Pipeline bleibt für die Verwaltung ausführbarer Pipeline-Objekte verantwortlich.

## 4. Cache Entry Model

Jeder Cache-Eintrag besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Cache ID | Eindeutige Identifikation |
| Cache Type | Cache-Ebene |
| Content Hash | Hash der relevanten Eingaben |
| Shader Version | Version des Shader-Inhalts |
| Compiler Version | Verwendete Compiler-Version |
| Backend Target | Ziel-Backend und GPU-Kompatibilität |
| Compile Options | Kompilierungsparameter |
| Dependency Signature | Relevante Abhängigkeiten |
| Cache State | Gültigkeitszustand |
| Cache Size | Speicherverbrauch |

Cache-Schlüssel MÜSSEN alle Eigenschaften berücksichtigen, die das Kompilierungsergebnis beeinflussen.

## 5. Cache Lifecycle

NovaOS definiert folgende Zustände:

`Missing → Creating → Valid → Invalid → Evictable → Removed`

Dabei gilt:

1. Ein Shader-Ergebnis wird angefordert.
2. Der zugehörige Cache-Schlüssel wird berechnet.
3. Ein vorhandener Cache-Eintrag wird gesucht.
4. Treffer werden auf Gültigkeit und Kompatibilität geprüft.
5. Gültige Ergebnisse werden wiederverwendet.
6. Fehlende Ergebnisse werden kompiliert und anschließend gespeichert.

Unvollständige oder fehlgeschlagene Kompilierungen DÜRFEN nicht als gültige Cache-Einträge veröffentlicht werden.

## 6. Cache Validation

Vor jeder Wiederverwendung MUSS NovaOS die Cache-Gültigkeit prüfen.

Zu berücksichtigen sind:

- Shader-Inhalt und Entry Point.
- Shader Stage.
- IR- und Compiler-Version.
- Compile Options.
- Spezialisierungsparameter.
- Ressourcen- und Pipeline-Layouts.
- GPU-Backend und relevante Hardwarefähigkeiten.
- Treiberkompatibilität.

Backendabhängige Binärdaten DÜRFEN nur unter nachgewiesener Kompatibilität wiederverwendet werden.

## 7. Cache Invalidation

NovaOS MUSS gezielte Cache-Invalidierung unterstützen.

Auslöser sind insbesondere:

- Änderungen am Shader-Code.
- Änderungen an der Shader IR.
- Compiler-Updates.
- Änderungen relevanter Compile Options.
- Inkompatible Pipeline-Layouts.
- Änderungen des GPU-Backends.
- Inkompatible Treiber- oder Hardwareänderungen.

Nur betroffene Cache-Einträge SOLLEN invalidiert werden.

Bereits aktive GPU-Pipelines MÜSSEN bis zum sicheren Abschluss ihrer Verwendung gültig bleiben.

## 8. Persistent Shader Cache

NovaOS SOLL einen persistenten Shader-Cache unterstützen.

Dabei gilt:

- Cache-Artefakte können lokal gespeichert werden.
- Cache-Dateien MÜSSEN versioniert und auf Integrität geprüft werden.
- Schreibvorgänge MÜSSEN atomar veröffentlicht werden.
- Beschädigte Einträge MÜSSEN verworfen werden.
- Der Cache MUSS ohne Netzwerkverbindung funktionieren.
- Cache-Daten DÜRFEN keine zwingende Voraussetzung für die Shader-Ausführung sein.

Der persistente Cache MUSS jederzeit sicher neu aufgebaut werden können.

## 9. Asynchronous Cache Operations

Cache-Operationen SOLLEN asynchron erfolgen.

Dabei gilt:

- Lese- und Schreiboperationen SOLLEN zeitkritische Renderpfade nicht blockieren.
- Gleichzeitige identische Kompilierungsanfragen SOLLEN zusammengeführt werden.
- Cache-Einträge DÜRFEN erst nach vollständiger Erstellung veröffentlicht werden.
- Hintergrundoperationen MÜSSEN hinsichtlich CPU-, Speicher- und I/O-Verbrauch begrenzbar sein.

## 10. Cache Replacement und Memory Budget

Nova Shader Caching MUSS definierte Speicherbudgets berücksichtigen.

Die Verdrängung SOLL anhand folgender Kriterien erfolgen:

- Letzte Verwendung.
- Nutzungshäufigkeit.
- Größe des Cache-Eintrags.
- Wiederherstellungskosten.
- Priorität systemkritischer Shader.

Nicht mehr benötigte Cache-Artefakte DÜRFEN entfernt werden.

Aktive GPU-Objekte DÜRFEN durch Cache-Bereinigung nicht vorzeitig zerstört werden.

## 11. Shader Compilation und Pipeline Integration

Nova Shader Caching MUSS mit folgenden Komponenten zusammenarbeiten:

- Nova Shader Compilation.
- Nova Shader Pipeline.
- Nova Shader Precompilation.
- Nova Shader Variant Management.
- Nova GPU Resource Management.

Dabei gilt:

- Der Compiler prüft geeignete Cache-Ebenen vor einer erneuten Übersetzung.
- Pipeline-Erstellung kann kompatible Cache-Artefakte verwenden.
- Shader-Varianten erhalten eindeutig unterscheidbare Cache-Schlüssel.
- Precompilation kann den Cache vor der ersten Verwendung befüllen.
- Laufzeitänderungen an Shadern invalidieren nur betroffene Ergebnisse.

## 12. Sicherheit und Isolation

Nova Shader Caching MUSS die NovaOS-Sicherheitsarchitektur berücksichtigen.

Dabei gilt:

- Cache-Artefakte gelten nicht automatisch als vertrauenswürdig.
- Persistente Cache-Daten MÜSSEN auf Integrität und Kompatibilität geprüft werden.
- Nicht vertrauenswürdige Shader DÜRFEN Sicherheitsvalidierungen nicht durch Cache-Treffer umgehen.
- Prozessübergreifende Cache-Nutzung MUSS Zugriffsrechte respektieren.
- Cache-Schlüssel DÜRFEN keine Berechtigungsprüfung ersetzen.

## 13. Fehlerbehandlung und Fallback

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Beschädigte Cache-Einträge.
- Inkompatible Shader-Binärdaten.
- Fehlgeschlagene Cache-Schreibvorgänge.
- Nicht verfügbare Cache-Speicherbereiche.
- Überschrittene Speicherbudgets.
- GPU-Geräteverlust.

Bei Cache-Fehlern MUSS die Shader-Kompilierung ohne Cache-Wiederverwendung möglich bleiben.

Ein Cache-Fehler DARF keine gültige bereits aktive Pipeline beschädigen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Shader-Cache-Verwaltung bereitstellen.
2. IR-, Compiler- und Pipeline-Caches MÜSSEN logisch getrennt sein.
3. Cache-Schlüssel MÜSSEN alle ergebnisrelevanten Eingaben berücksichtigen.
4. Cache-Einträge MÜSSEN vor Wiederverwendung validiert werden.
5. Gezielte Cache-Invalidierung MUSS unterstützt werden.
6. Persistentes Shader-Caching SOLL verfügbar sein.
7. Cache-Zugriffe SOLLEN asynchron erfolgen.
8. Identische parallele Kompilierungsanfragen SOLLEN zusammengeführt werden.
9. Cache-Speicherbudgets MÜSSEN eingehalten werden.
10. Aktive GPU-Pipelines DÜRFEN nicht vorzeitig freigegeben werden.
11. Nova Shader Compilation MUSS integriert werden.
12. Nova Shader Pipeline MUSS Cache-Artefakte verwenden können.
13. Shader Precompilation und Variant Management MÜSSEN integrierbar sein.
14. Cache-Treffer DÜRFEN Sicherheitsvalidierungen nicht umgehen.
15. Bei Cache-Fehlern MUSS ein sicherer Fallback verfügbar sein.
16. Im deterministischen Modus MÜSSEN Cache-Treffer und Cache-Fehler bei identischen Shader-Eingaben dieselben definierten Shader-Semantiken gewährleisten.

## 15. Ergebnis

Nova Shader Caching stellt eine mehrstufige Cache-Infrastruktur für Shader-Zwischendarstellungen, kompilierte GPU-Programme und Pipeline-Artefakte bereit.

Durch inhaltsbasierte Identifikation, gezielte Invalidierung, persistente Speicherung und asynchrone Verarbeitung werden wiederholte Kompilierungen und unnötige Laufzeitunterbrechungen reduziert.

Damit verbessert NovaOS die Startzeiten, die Reaktionsfähigkeit und die Rendering-Leistung, insbesondere bei komplexen Materialeffekten wie Liquid Glass.