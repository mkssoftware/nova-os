# NPSPEC-GPURESOURCE-BUFFER-0001 – Nova GPU Buffer Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Buffer Management definiert die Erstellung, Verwaltung, Verwendung und Freigabe linearer GPU-Speicherressourcen innerhalb von NovaOS.

Ziel ist eine effiziente und sichere Buffer-Verwaltung für Rendering, Compute, Datentransfers und die GPU-Command-Verarbeitung.

Die Architektur MUSS dedizierte und integrierte GPUs unterstützen und unabhängig von einer konkreten Grafik-API funktionieren.

## 2. Architekturprinzipien

- **Resource-Oriented:** GPU-Buffer werden als eigenständige Ressourcen verwaltet.
- **Usage-Aware:** Speicherzuweisungen berücksichtigen die vorgesehene Verwendung.
- **Range-Aware:** Zugriffe können auf einzelne Buffer-Bereiche begrenzt werden.
- **Lifetime-Aware:** Buffer bleiben während aktiver GPU-Zugriffe gültig.
- **Zero-Copy:** Unnötige Speicherübertragungen werden vermieden.
- **Pooling:** Geeignete Buffer können wiederverwendet werden.
- **Synchronization-Safe:** CPU- und GPU-Zugriffe werden korrekt synchronisiert.
- **Backend-Independent:** Einheitliches logisches Buffer-Modell.

## 3. GPU Buffer Model

Jeder GPU-Buffer besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Buffer ID | Eindeutige Ressourcenidentifikation |
| Size | Logische Buffer-Größe |
| Usage Flags | Zulässige Verwendungsarten |
| Memory Class | Verwendete Speicherklasse |
| Allocation ID | Zugehörige Speicherzuweisung |
| Offset | Position innerhalb der Speicherzuweisung |
| Alignment | Erforderliche Speicherausrichtung |
| Resource State | Aktueller logischer Zustand |
| Owner | Eigentümer der Ressource |
| Lifetime | Gültigkeitsdauer |

Logische Buffer-Identität und physische Speicherzuweisung MÜSSEN getrennt verwaltet werden.

## 4. Buffer Types

NovaOS SOLL folgende Buffer-Verwendungen unterstützen:

| Buffer-Typ | Verwendung |
|---|---|
| Vertex Buffer | Geometriedaten |
| Index Buffer | Vertex-Indizes |
| Uniform Buffer | Shader-Konstanten |
| Storage Buffer | Allgemeine GPU-Daten |
| Indirect Buffer | Indirekte GPU-Kommandos |
| Transfer Buffer | GPU-Datenübertragungen |
| Staging Buffer | CPU-GPU-Datenaustausch |
| Readback Buffer | GPU-CPU-Datenübertragung |

Ein Buffer DARF mehrere Verwendungsarten besitzen, sofern das GPU-Backend diese Kombination unterstützt.

## 5. Buffer Allocation

Nova GPU Buffer Management MUSS mit Nova GPU Memory Allocation zusammenarbeiten.

Dabei gilt:

1. Buffer-Größe und Verwendungszweck werden ermittelt.
2. Hardwareabhängige Speicheranforderungen werden geprüft.
3. Eine geeignete Speicherklasse wird ausgewählt.
4. Speicher wird zugewiesen.
5. Der Buffer wird entsprechend den Backend-Anforderungen gebunden.

Dedicated Allocation und Suballocation MÜSSEN entsprechend den Hardwarefähigkeiten unterstützt werden können.

## 6. Buffer Views und Ranges

NovaOS MUSS Zugriffe auf definierte Buffer-Bereiche unterstützen.

Ein Buffer Range besteht mindestens aus:

- Buffer ID
- Offset
- Length
- Access Type

Dabei gilt:

- Buffer-Bereiche MÜSSEN innerhalb der gültigen Buffer-Grenzen liegen.
- Alignment-Anforderungen MÜSSEN eingehalten werden.
- Überlappende Zugriffe MÜSSEN bei der Synchronisation berücksichtigt werden.
- Unabhängige Buffer-Bereiche SOLLEN parallel verwendet werden können.

## 7. CPU Memory Mapping

NovaOS MUSS CPU-Mapping für geeignete GPU-Buffer unterstützen.

Dabei gilt:

- Nur CPU-sichtbarer Speicher DARF direkt gemappt werden.
- Mapping-Rechte MÜSSEN validiert werden.
- Nicht kohärenter Speicher benötigt geeignete Flush- und Invalidate-Operationen.
- Gleichzeitige inkompatible CPU- und GPU-Zugriffe MÜSSEN verhindert werden.
- Unmapping DARF aktive GPU-Zugriffe nicht unzulässig beeinträchtigen.

Die erforderliche Speichersichtbarkeit wird durch Nova CPU-GPU Synchronization gewährleistet.

## 8. Buffer Transfers

NovaOS MUSS kontrollierte Buffer-Datenübertragungen unterstützen.

Dazu gehören:

- CPU-to-GPU Upload.
- GPU-to-CPU Readback.
- GPU-to-GPU Copy.
- Buffer-to-Texture Copy.
- Texture-to-Buffer Copy.
- Partielle Buffer-Aktualisierung.

Transfers SOLLEN asynchron erfolgen.

Dabei gilt:

- Quell- und Zielbereiche MÜSSEN gültig sein.
- Überlappende Kopierbereiche MÜSSEN entsprechend den Backend-Regeln behandelt werden.
- Erforderliche Synchronisationsbedingungen MÜSSEN eingehalten werden.
- Nicht abgeschlossene Transfers DÜRFEN nicht als erfolgreich abgeschlossen gemeldet werden.

## 9. Buffer State Tracking

NovaOS MUSS relevante Buffer-Zugriffe verfolgen.

Dazu gehören:

- Read
- Write
- Read-Write
- Transfer Read
- Transfer Write
- Shader Access
- Indirect Command Read

Die Zugriffsverwaltung MUSS mit Nova GPU Resource Hazard Tracking zusammenarbeiten.

RAW-, WAR- und WAW-Abhängigkeiten MÜSSEN berücksichtigt werden.

## 10. Buffer Lifecycle

NovaOS definiert folgenden Lebenszyklus:

`Created → Allocated → Active → Retired → Released`

Dabei gilt:

- Buffer werden vor ihrer Verwendung validiert.
- Aktive GPU-Zugriffe werden nachverfolgt.
- Logisch freigegebene Buffer bleiben bis zum Abschluss aller relevanten Zugriffe geschützt.
- Physischer Speicher wird erst nach sicherer Freigabe wiederverwendet.

GPU Fences und Timeline Synchronization MÜSSEN integrierbar sein.

## 11. Buffer Pooling

Nova GPU Buffer Management SOLL mit Nova GPU Resource Pooling zusammenarbeiten.

Dabei gilt:

- Kompatible Buffer können wiederverwendet werden.
- Häufig benötigte Buffer-Größen können in Pools verwaltet werden.
- Temporäre Buffer können über Ring- oder Linear-Allocator bereitgestellt werden.
- Noch aktive Buffer DÜRFEN nicht erneut vergeben werden.
- Pool-Kapazitäten unterliegen GPU-Speicherbudgets.

Wiederverwendete Buffer-Inhalte gelten grundsätzlich als undefiniert, sofern ihre Gültigkeit nicht ausdrücklich nachgewiesen wurde.

## 12. Buffer Aliasing

NovaOS SOLL Buffer Aliasing für geeignete Ressourcen unterstützen.

Dabei gilt:

- Mehrere logische Buffer können denselben physischen Speicherbereich verwenden.
- Ihre aktiven Verwendungen dürfen sich nicht unzulässig überschneiden.
- Erforderliche Aliasing Barriers MÜSSEN berücksichtigt werden.
- Speicherkompatibilität und Alignment MÜSSEN gewährleistet sein.

Die Planung erfolgt durch Nova GPU Memory Aliasing.

## 13. Buffer Residency

Nova GPU Buffer Management MUSS mit Nova GPU Resource Residency zusammenarbeiten.

Dabei gilt:

- Benötigte Buffer MÜSSEN vor ihrer GPU-Verwendung verfügbar sein.
- Aktive Buffer DÜRFEN nicht unsicher verdrängt werden.
- Wiederherstellbare Buffer können bei Speicherknappheit freigegeben werden.
- Erforderliche Wiederherstellungen MÜSSEN vor abhängigen Zugriffen abgeschlossen sein.

## 14. Zero-Copy und Buffer Sharing

NovaOS SOLL Buffer zwischen autorisierten Komponenten ohne unnötige Kopien gemeinsam verwenden.

Dabei gilt:

- Sharing erfolgt über kontrollierte Ressourcenhandles.
- Zugriffsrechte MÜSSEN geprüft werden.
- CPU-GPU- und GPU-GPU-Synchronisation MÜSSEN gewährleistet sein.
- Buffer-Lebensdauern MÜSSEN alle Consumer berücksichtigen.
- Nicht unterstützte Sharing-Verfahren benötigen einen sicheren Fallback.

Zero-Copy DARF nicht zulasten der Speicherisolation erfolgen.

## 15. Performance und Optimierung

NovaOS SOLL den Verwaltungsaufwand für GPU-Buffer minimieren.

Dazu gehören:

- Suballocation kleiner Buffer.
- Wiederverwendung temporärer Speicherbereiche.
- Asynchrone Datentransfers.
- Zusammenfassung kompatibler Transfers.
- Partielle Buffer-Aktualisierung.
- Vermeidung unnötiger CPU-GPU-Synchronisation.
- Reduzierung häufiger Backend-Allokationen.

Optimierungen DÜRFEN erforderliche Ressourcenabhängigkeiten nicht verletzen.

## 16. Sicherheit und Isolation

GPU-Buffer MÜSSEN capability-basiert geschützt werden.

Dabei gilt:

- Jeder Buffer besitzt einen definierten Eigentümer.
- Lese-, Schreib- und Sharing-Rechte MÜSSEN unterscheidbar sein.
- Ungültige Buffer Handles MÜSSEN zurückgewiesen werden.
- Buffer-Zugriffe MÜSSEN auf gültige Speicherbereiche begrenzt werden.
- Wiederverwendeter Speicher DARF keine fremden Daten offenlegen.
- Prozessübergreifende Freigaben benötigen entsprechende Berechtigungen.

## 17. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Buffer-Größen.
- Fehlgeschlagene Speicherzuweisungen.
- Ungültige Buffer-Bereiche.
- Nicht unterstützte Usage Flags.
- Fehlerhafte Speicher-Mappings.
- Fehlgeschlagene Datentransfers.
- Synchronisationsfehler.
- GPU-Geräteverlust.

Fehlerhafte Buffer DÜRFEN nicht als gültige Ressourcen bereitgestellt werden.

Bei Geräteverlust MÜSSEN betroffene Buffer invalidiert oder kontrolliert wiederhergestellt werden.

## 18. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale GPU-Buffer-Verwaltung bereitstellen.
2. Unterschiedliche Buffer-Verwendungsarten MÜSSEN unterstützt werden.
3. Logische Buffer und physische Speicherzuweisungen MÜSSEN getrennt verwaltet werden.
4. GPU Memory Allocation MUSS integriert werden.
5. Buffer-Ranges MÜSSEN unterstützt werden.
6. CPU-Mapping MUSS für geeignete Speicherbereiche möglich sein.
7. Cache-Kohärenz MUSS berücksichtigt werden.
8. Asynchrone Buffer-Transfers SOLLEN bevorzugt werden.
9. Partielle Buffer-Aktualisierungen MÜSSEN möglich sein.
10. GPU Resource Hazard Tracking MUSS integriert werden.
11. Aktive Buffer DÜRFEN nicht vorzeitig freigegeben werden.
12. GPU Resource Pooling SOLL unterstützt werden.
13. GPU Memory Aliasing SOLL unterstützt werden.
14. Resource Residency MUSS berücksichtigt werden.
15. Zero-Copy SOLL bevorzugt werden, sofern sicher und effizient.
16. Buffer-Zugriffe MÜSSEN capability-basiert geschützt werden.
17. Nicht unterstützte Hardwarefunktionen MÜSSEN durch geeignete Fallbacks behandelt werden.
18. Im deterministischen Modus MÜSSEN logische Buffer-Verwaltungsentscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 19. Ergebnis

Nova GPU Buffer Management stellt die zentrale Verwaltung linearer GPU-Speicherressourcen innerhalb von NovaOS bereit.

Durch effiziente Speicherzuweisung, Buffer-Ranges, asynchrone Transfers, Pooling, Aliasing und kontrollierte Synchronisation werden Speicherverbrauch und Verwaltungsaufwand reduziert.

Damit erhält NovaOS eine leistungsfähige Buffer-Verwaltung für Rendering, Compute, Compositor und die gesamte GPU-Command-Verarbeitung.