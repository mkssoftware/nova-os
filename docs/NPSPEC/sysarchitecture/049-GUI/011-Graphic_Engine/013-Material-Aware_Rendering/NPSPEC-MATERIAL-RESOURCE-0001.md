# NPSPEC-MATERIAL-RESOURCE-0001 – Nova Material Resource Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine / Nova GPU Resource Management

## 1. Zweck

Nova Material Resource Management definiert die Verwaltung, Bereitstellung und Wiederverwendung sämtlicher Ressourcen, die zur Darstellung grafischer Materialien benötigt werden.

Ziel ist die effiziente Nutzung von GPU-Speicher, Texturen, Buffern und temporären Render Targets, ohne unnötige Speicherallokationen oder Datenkopien zu verursachen.

Die Ressourcenverwaltung MUSS mit Nova GPU Resource Management zusammenarbeiten und dessen Speicher-, Synchronisations- und Sicherheitsmechanismen verwenden.

## 2. Architekturprinzipien

- **Resource-Aware:** Materialoperationen berücksichtigen verfügbare Ressourcen.
- **On-Demand:** Ressourcen werden möglichst bedarfsgesteuert bereitgestellt.
- **Reuse-Oriented:** Kompatible Ressourcen werden wiederverwendet.
- **Lifetime-Aware:** Ressourcen bleiben nur so lange erhalten wie erforderlich.
- **Zero-Copy:** Unnötige Datenkopien werden vermieden.
- **Budget-Aware:** Materialressourcen unterliegen definierten Speicherbudgets.
- **Dependency-Driven:** Ressourcenlebenszeiten berücksichtigen Materialabhängigkeiten.
- **Synchronization-Safe:** Ressourcen werden erst nach Abschluss aller Zugriffe freigegeben.

## 3. Architektur

Nova Material Resource Management besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Material Resource Manager | Verwaltung materialbezogener Ressourcen |
| Resource Descriptor | Deklarative Ressourcenbeschreibung |
| Resource Allocator | Anforderung von GPU-Speicher |
| Resource Pool Controller | Wiederverwendung kompatibler Ressourcen |
| Resource Lifetime Tracker | Überwachung der Ressourcenlebenszeit |
| Resource Budget Controller | Kontrolle des Speicherverbrauchs |
| Resource Release Manager | Sichere Rückgabe nicht benötigter Ressourcen |

Die physische Speicherverwaltung verbleibt bei Nova GPU Resource Management.

## 4. Resource Model

Jede Materialressource besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Resource ID | Eindeutige Ressourcenidentifikation |
| Resource Type | Art der Ressource |
| Resource Descriptor | Format, Größe und Verwendung |
| Material Reference | Zugehörige Materialinstanz oder gemeinsame Nutzung |
| Resource Version | Aktuelle Ressourcenversion |
| Lifetime | Gültigkeits- und Nutzungszeitraum |
| Access State | Aktuelle Zugriffsbedingungen |
| Residency State | Verfügbarkeit im GPU-Speicher |
| Resource State | Lebenszykluszustand |

Logische Ressourcenidentität und physische Speicherzuordnung MÜSSEN getrennt bleiben.

## 5. Unterstützte Ressourcen

NovaOS MUSS folgende Ressourcenarten unterstützen können:

| Ressource | Verwendung |
|---|---|
| Material Textures | Material- und Effekttexturen |
| Backdrop Textures | Hintergrundabtastung |
| Intermediate Textures | Zwischenergebnisse von Materialeffekten |
| Render Targets | Ausgabe einzelner Render-Passes |
| Uniform Buffers | Materialparameter |
| Storage Buffers | Erweiterte Shader-Daten |
| Samplers | Texturfilterung und Adressierung |
| Shared Resources | Gemeinsam verwendete Materialdaten |

Weitere Ressourcenarten DÜRFEN abhängig von den GPU-Fähigkeiten ergänzt werden.

## 6. Resource Allocation

Materialressourcen MÜSSEN über Nova GPU Resource Management angefordert werden.

Dabei gilt:

- Größe, Format, Alignment und Verwendungszweck MÜSSEN angegeben werden.
- Geeignete Speicherklassen MÜSSEN berücksichtigt werden.
- Speicherbudgets DÜRFEN nicht unkontrolliert überschritten werden.
- Wiederverwendbare Ressourcen SOLLEN bevorzugt werden.
- Temporäre Ressourcen SOLLEN möglichst spät angefordert werden.

Eine Materialinstanz DARF keine unkontrollierten direkten GPU-Speicherallokationen durchführen.

## 7. Resource Pooling

NovaOS SOLL häufig benötigte Materialressourcen über Resource Pools wiederverwenden.

Dabei gilt:

- Ressourcen müssen hinsichtlich Format, Größe und Verwendung kompatibel sein.
- Wiederverwendung erfolgt erst nach Abschluss aller vorherigen Zugriffe.
- Ressourceninhalte gelten nach einer Pool-Rückgabe grundsätzlich nicht als gültig.
- Ressourcen MÜSSEN vor einer Verwendung mit erforderlichen Inhalten initialisiert werden.
- Nicht benötigte Pool-Ressourcen SOLLEN bei Speicherbelastung freigegeben werden.

Resource Pooling ersetzt nicht Nova Material Effect Caching.

## 8. Resource Lifetime Management

NovaOS MUSS die Lebenszeit materialbezogener Ressourcen verfolgen.

Der logische Ablauf lautet:

`Requested → Allocated → Ready → In Use → Pending Release → Released`

Dabei gilt:

- Ressourcen bleiben während aktiver GPU-Operationen gültig.
- Mehrere Verbraucher MÜSSEN berücksichtigt werden.
- Gemeinsame Ressourcen dürfen erst nach Abschluss sämtlicher Zugriffe freigegeben werden.
- GPU Fences und Timeline Synchronization MÜSSEN berücksichtigt werden.
- Veraltete Ressourcen DÜRFEN nicht vorzeitig wiederverwendet werden.

## 9. Transient Resources

NovaOS MUSS temporäre Materialressourcen unterstützen.

Dazu gehören insbesondere:

- Blur-Zwischenergebnisse.
- Refraction-Render-Targets.
- Reflection-Zwischenergebnisse.
- Temporäre Backdrop-Texturen.
- Zwischenstufen komplexer Liquid-Glass-Pipelines.

Temporäre Ressourcen SOLLEN über Nova Render Graph Resource Management verwaltet werden.

GPU Memory Aliasing DARF verwendet werden, wenn Ressourcenlebenszeiten und Synchronisationsbedingungen dies sicher erlauben.

## 10. Shared Material Resources

NovaOS SOLL kompatible Materialressourcen gemeinsam verwenden können.

Dabei gilt:

- Gemeinsame Backdrop-Ressourcen SOLLEN mehrfach nutzbar sein.
- Unveränderliche Materialdaten SOLLEN geteilt werden können.
- Prozessübergreifende Freigaben benötigen entsprechende Capabilities.
- Schreibzugriffe MÜSSEN kontrolliert werden.
- Geschützte Inhalte DÜRFEN nicht unautorisiert zugänglich werden.

Zero-Copy Sharing SOLL verwendet werden, sofern Hardware und Sicherheitsanforderungen dies ermöglichen.

## 11. Resource Budgeting

Nova Material Resource Management MUSS Speicherbudgets berücksichtigen.

Dabei SOLLEN folgende Kategorien getrennt erfasst werden:

- Permanente Materialressourcen.
- Temporäre Render Targets.
- Material Cache.
- Resource Pools.
- Gemeinsam verwendete Ressourcen.

Die Budgetierung MUSS mit Nova GPU Memory Budgeting zusammenarbeiten.

Gemeinsam verwendete Ressourcen DÜRFEN in der physischen Gesamtspeicherbilanz nicht mehrfach gezählt werden.

## 12. Memory Pressure Management

NovaOS MUSS auf GPU-Speicherbelastung reagieren können.

Die bevorzugte Reihenfolge lautet:

1. Nicht benötigte temporäre Ressourcen freigeben.
2. Unbenutzte Pool-Ressourcen reduzieren.
3. Entbehrliche Material-Cache-Einträge entfernen.
4. Material-Zwischenauflösungen kontrolliert reduzieren.
5. Vereinfachte Material-Pipelines verwenden.
6. Geeignete Material-Fallbacks aktivieren.

Aktive Ressourcen DÜRFEN nicht zur kurzfristigen Speichergewinnung unsicher freigegeben werden.

## 13. Incremental Rendering Integration

Nova Material Resource Management MUSS inkrementelles Material-Rendering unterstützen.

Dabei gilt:

- Gültige Ressourcen und Zwischenergebnisse bleiben erhalten.
- Änderungen einzelner Regionen SOLLEN keine vollständige Ressourcen-Neuallokation erfordern.
- Teilaktualisierungen MÜSSEN unterstützt werden, sofern die Ressource dies erlaubt.
- Ressourcenwechsel MÜSSEN betroffene Cache-Ergebnisse invalidieren.
- Unveränderte Materialressourcen SOLLEN wiederverwendet werden.

## 14. Render Graph Integration

Nova Material Resource Management MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Ressourcenanforderungen werden deklarativ bereitgestellt.
- Ressourcenlebenszeiten werden anhand der geplanten Render-Passes ermittelt.
- Temporäre Ressourcen können zwischen kompatiblen Verarbeitungsschritten wiederverwendet werden.
- GPU Resource Barriers MÜSSEN berücksichtigt werden.
- Ressourcenfreigaben erfolgen erst nach sicherem Abschluss der GPU-Verwendung.

Die Material Engine DARF keine Ressourcenfreigabe erzwingen, die aktive Render-Graph-Abhängigkeiten verletzt.

## 15. Liquid Glass Integration

NovaOS MUSS die Ressourcenanforderungen komplexer Liquid-Glass-Effekte unterstützen.

Dazu gehören:

- Backdrop Textures.
- Blur Render Targets.
- Refraction Textures.
- Reflection Resources.
- Lighting Buffers.
- Material Intermediate Results.

Dabei gilt:

- Gemeinsame Ressourcen SOLLEN wiederverwendet werden.
- Temporäre Ressourcen SOLLEN möglichst effizient gepoolt werden.
- Unveränderte Effektergebnisse SOLLEN erhalten bleiben.
- Speicherintensive Effekte MÜSSEN adaptive Qualitätsstufen unterstützen.
- Bei Ressourcenmangel MUSS ein geeigneter Material-Fallback möglich sein.

## 16. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Fehlgeschlagene Speicherallokation.
- Ungültige Ressourcenhandles.
- Inkompatible Ressourcenformate.
- Nicht verfügbare GPU-Ressourcen.
- Überschrittene Speicherbudgets.
- Ungültige Ressourcenlebenszeiten.
- GPU-Geräteverlust.

Bei Ressourcenfehlern MUSS eine kontrollierte Wiederherstellung oder ein geeigneter Material-Fallback möglich sein.

Ressourcen DÜRFEN keine Daten anderer Prozesse offenlegen.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Verwaltung materialbezogener Ressourcen bereitstellen.
2. Die physische GPU-Speicherverwaltung MUSS über Nova GPU Resource Management erfolgen.
3. Ressourcen MÜSSEN eindeutig identifizierbar und versionierbar sein.
4. Ressourcenlebenszeiten MÜSSEN verfolgt werden.
5. Temporäre Materialressourcen MÜSSEN unterstützt werden.
6. Resource Pooling SOLL unterstützt werden.
7. Zero-Copy Sharing SOLL bevorzugt werden, sofern sicher und möglich.
8. GPU Memory Budgeting MUSS berücksichtigt werden.
9. GPU Memory Pressure MUSS kontrolliert behandelt werden.
10. Nova Render Graph MUSS integriert werden.
11. Nova GPU Synchronization MUSS berücksichtigt werden.
12. Aktive Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
13. Inkrementelles Material-Rendering MUSS unterstützt werden.
14. Nova Material Effect Caching MUSS integrierbar sein.
15. Liquid-Glass-Ressourcen MÜSSEN effizient verwaltbar sein.
16. Adaptive Qualitätsstufen und Material-Fallbacks MÜSSEN unterstützt werden.
17. Prozessübergreifende Ressourcenfreigaben MÜSSEN capability-basiert abgesichert sein.
18. Im deterministischen Modus MÜSSEN logische Ressourcenentscheidungen bei identischen Eingaben und Systemzuständen reproduzierbar sein.

## 18. Ergebnis

Nova Material Resource Management stellt eine zentrale Infrastruktur zur effizienten Verwaltung materialbezogener GPU-Ressourcen bereit.

Durch bedarfsgesteuerte Allokation, Resource Pooling, kontrollierte Ressourcenlebenszeiten, Zero-Copy und integrierte Speicherbudgetierung werden unnötige Speicheroperationen reduziert.

Damit ermöglicht NovaOS eine ressourceneffiziente Darstellung komplexer Materialien, insbesondere Liquid Glass, bei gleichzeitig kontrolliertem GPU-Speicherverbrauch.