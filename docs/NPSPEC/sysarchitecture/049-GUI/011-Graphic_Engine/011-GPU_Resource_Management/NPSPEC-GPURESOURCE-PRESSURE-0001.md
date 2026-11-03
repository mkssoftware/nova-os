# NPSPEC-GPURESOURCE-PRESSURE-0001 – Nova GPU Memory Pressure Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Memory Pressure Management definiert die Erkennung und Behandlung von GPU-Speicherknappheit.

Ziel ist die Aufrechterhaltung einer stabilen und reaktionsfähigen Grafikverarbeitung, auch wenn der verfügbare GPU-Speicher stark ausgelastet ist.

NovaOS MUSS Speicherknappheit möglichst frühzeitig erkennen und kontrolliert darauf reagieren, bevor kritische Speicheranforderungen fehlschlagen.

## 2. Architekturprinzipien

- **Event-Driven:** Speicherbelastung wird ereignisgesteuert verarbeitet.
- **Budget-Aware:** Entscheidungen basieren auf verfügbaren Speicherbudgets.
- **Priority-Based:** Systemkritische Ressourcen werden bevorzugt geschützt.
- **Adaptive:** Maßnahmen richten sich nach dem tatsächlichen Speicherdruck.
- **Progressive:** Speicherentlastung erfolgt abgestuft.
- **Non-Blocking:** Unnötige CPU- und GPU-Blockierungen werden vermieden.
- **Recovery-Oriented:** Nach Entlastung wird der Normalbetrieb kontrolliert wiederhergestellt.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-Architekturen.

## 3. Memory Pressure Model

NovaOS definiert folgende logische Belastungszustände:

| Zustand | Bedeutung |
|---|---|
| Normal | Ausreichend GPU-Speicher verfügbar |
| Elevated | Erhöhte Speicherbelastung |
| High | Deutliche Speicherknappheit |
| Critical | Kritischer Speichermangel |
| Exhausted | Neue Speicheranforderungen können nicht zuverlässig erfüllt werden |

Die Einstufung MUSS verfügbare Budgets, Speicherbelegung und aktuelle Allokationsanforderungen berücksichtigen.

Feste Schwellenwerte DÜRFEN nicht unabhängig von der tatsächlichen Hardwarekapazität verwendet werden.

## 4. Pressure Detection

NovaOS MUSS GPU-Speicherbelastung anhand geeigneter Informationen erkennen.

Dazu gehören:

- Verfügbares GPU-Speicherbudget.
- Aktuelle Speicherbelegung.
- Reservierte Speicherbereiche.
- Residenter Speicher.
- Cache- und Pool-Verbrauch.
- Fehlgeschlagene Allokationen.
- Fragmentierung und Allokationsbeschränkungen.
- Speicherbedarf ausstehender GPU-Aufträge.

Die Erkennung SOLL ereignisgesteuert erfolgen.

Eine ergänzende periodische Überprüfung DARF verwendet werden, wenn das GPU-Backend keine ausreichenden Ereignisse bereitstellt.

## 5. Pressure State Management

Der Memory Pressure Manager MUSS Zustandsänderungen kontrolliert verwalten.

Dabei gilt:

1. Speicherbelastung wird ermittelt.
2. Der aktuelle Pressure-Zustand wird bewertet.
3. Erforderliche Entlastungsmaßnahmen werden bestimmt.
4. Betroffene Subsysteme werden informiert.
5. Die Wirkung der Maßnahmen wird überprüft.
6. Nach ausreichender Entlastung erfolgt die Rückkehr in einen niedrigeren Belastungszustand.

Hysterese SOLL häufige Zustandswechsel verhindern.

## 6. Progressive Memory Reclamation

NovaOS MUSS abgestufte Speicherentlastung unterstützen.

| Belastung | Bevorzugte Maßnahmen |
|---|---|
| Elevated | Ungenutzte Cache-Einträge reduzieren |
| High | Resource Pools verkleinern und wiederherstellbare Ressourcen verdrängen |
| Critical | Temporäre Ressourcen reduzieren und Rendering-Qualität anpassen |
| Exhausted | Nicht kritische Allokationen ablehnen und geschützte Reserven erhalten |

Die Maßnahmen MÜSSEN anhand ihrer tatsächlichen Speicherersparnis und Auswirkungen bewertet werden.

Aktive GPU-Ressourcen DÜRFEN nicht unsicher freigegeben werden.

## 7. Resource Reclamation

NovaOS SOLL Speicher zunächst aus entbehrlichen Ressourcen zurückgewinnen.

Bevorzugte Kandidaten sind:

- Ungenutzte Resource-Cache-Einträge.
- Freie Speicherblöcke aus Resource Pools.
- Nicht benötigte temporäre Render Targets.
- Wiederherstellbare Texturen.
- Nicht mehr benötigte Staging Buffers.

Ressourcen mit aktiven GPU-Zugriffen MÜSSEN bis zu ihrer sicheren Freigabe geschützt bleiben.

Die physische Speicherfreigabe MUSS durch den zuständigen Resource Manager bestätigt werden.

## 8. Adaptive Rendering Integration

Nova GPU Memory Pressure Management MUSS mit Nova Adaptive Rendering Strategy zusammenarbeiten.

Bei hoher Speicherbelastung SOLLEN geeignete Anpassungen möglich sein:

- Reduzierung temporärer Render-Target-Größen.
- Verringerung der Cache-Kapazitäten.
- Verwendung speichersparender Effektvarianten.
- Reduzierung optionaler Materialeffekte.
- Begrenzung zusätzlicher Zwischenbuffer.

Die visuelle Qualität DARF angepasst werden, sofern die Darstellung korrekt und benutzbar bleibt.

Systemkritische Oberflächenelemente MÜSSEN weiterhin zuverlässig dargestellt werden.

## 9. Resource Residency und Eviction

Nova GPU Memory Pressure Management MUSS mit Nova GPU Resource Residency und Nova GPU Resource Eviction zusammenarbeiten.

Dabei gilt:

- Eviction-Kandidaten werden anhand ihrer Wiederherstellbarkeit bewertet.
- Aktive GPU-Zugriffe werden berücksichtigt.
- Wiederherstellungskosten fließen in Entscheidungen ein.
- Erforderliche Synchronisation MUSS eingehalten werden.
- Ressourcen ohne sichere Wiederherstellung DÜRFEN nicht unkontrolliert verdrängt werden.

## 10. Priority und Fairness

NovaOS MUSS systemkritische GPU-Ressourcen bevorzugt schützen.

Dazu gehören insbesondere:

- Display Server.
- Compositor.
- Aktive Systemoberfläche.
- Notwendige Präsentationsbuffer.
- Ressourcen zur Fehlerbehandlung und Wiederherstellung.

Gewöhnliche Anwendungen MÜSSEN innerhalb ihrer Speicherbudgets verwaltet werden.

Eine einzelne Anwendung DARF nicht dauerhaft die gesamte verfügbare GPU-Speicherkapazität beanspruchen und dadurch die Systemdarstellung gefährden.

## 11. Multi-GPU und Shared Memory

NovaOS MUSS Speicherbelastung je GPU und Speichersegment getrennt verwalten.

Dabei gilt:

- Unterschiedliche GPUs besitzen unabhängige Pressure-Zustände.
- Gemeinsam genutzter Systemspeicher wird mit dem allgemeinen Memory Manager abgestimmt.
- Speicherentlastung auf einer GPU DARF keine unkontrollierte Belastung anderer Speicherbereiche verursachen.
- Ressourcenmigration DARF nur bei geeigneter Hardwareunterstützung und korrekter Synchronisation erfolgen.

## 12. Recovery Management

Nach einer Speicherentlastung MUSS NovaOS kontrolliert zum Normalbetrieb zurückkehren können.

Dabei gilt:

- Reduzierte Caches DÜRFEN bedarfsgerecht wieder aufgebaut werden.
- Adaptive Qualitätsreduzierungen SOLLEN schrittweise zurückgenommen werden.
- Speicherbudgets werden erneut bewertet.
- Unnötige Ressourcen-Neuallokationen SOLLEN vermieden werden.
- Hysterese verhindert wiederholte Wechsel zwischen Entlastung und Wiederaufbau.

## 13. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Situationen kontrolliert behandeln:

- Kritischer GPU-Speichermangel.
- Wiederholt fehlgeschlagene Allokationen.
- Nicht freigebbare Ressourcen.
- Fehlgeschlagene Eviction-Operationen.
- Unvollständige Budgetinformationen.
- GPU-Timeouts.
- Geräteverlust.

Dabei gilt:

- Speicherentlastung DARF keine fremden Ressourcen unautorisiert verändern.
- Freigaben MÜSSEN bestehende Synchronisationsbedingungen beachten.
- Fehlerhafte Anwendungen DÜRFEN keine unbegrenzten Ressourcen beanspruchen.
- Bei Geräteverlust MUSS eine kontrollierte Invalidierung beziehungsweise Wiederherstellung erfolgen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS GPU Memory Pressure Management bereitstellen.
2. Speicherbelastung MUSS anhand tatsächlicher Ressourcenbedingungen bewertet werden.
3. Mehrere Pressure-Zustände MÜSSEN unterstützt werden.
4. Zustandsänderungen SOLLEN ereignisgesteuert verarbeitet werden.
5. Speicherentlastung MUSS abgestuft erfolgen.
6. Ungenutzte Cache- und Pool-Ressourcen SOLLEN bevorzugt freigegeben werden.
7. Resource Residency MUSS integriert werden.
8. Resource Eviction MUSS integriert werden.
9. GPU Memory Budgeting MUSS berücksichtigt werden.
10. Aktive GPU-Ressourcen DÜRFEN nicht unsicher freigegeben werden.
11. Systemkritische Ressourcen MÜSSEN bevorzugt geschützt werden.
12. Adaptive Rendering SOLL zur Speicherentlastung beitragen können.
13. Unterschiedliche GPUs und Speichersegmente MÜSSEN getrennt berücksichtigt werden.
14. Wiederherstellungsmaßnahmen MÜSSEN nach ausreichender Entlastung möglich sein.
15. Hysterese SOLL unnötige Zustandswechsel verhindern.
16. Speicherentlastung MUSS capability-basierte Zugriffsrechte respektieren.
17. Kritischer Speichermangel MUSS kontrolliert behandelt werden.
18. Im deterministischen Modus MÜSSEN Entlastungsentscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 15. Ergebnis

Nova GPU Memory Pressure Management gewährleistet die kontrollierte Behandlung von GPU-Speicherknappheit.

Durch frühzeitige Erkennung, abgestufte Speicherentlastung, priorisierte Ressourcenverwaltung und adaptive Rendering-Strategien bleibt NovaOS auch bei hoher GPU-Speicherbelastung möglichst stabil und reaktionsfähig.

Damit wird verhindert, dass unnötiger Speicherverbrauch oder einzelne Anwendungen die Zuverlässigkeit der gesamten Grafikverarbeitung gefährden.