# NPSPEC-GPURESOURCE-MEMORY-0001 – Nova GPU Memory Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Memory Management definiert die Verwaltung des GPU-adressierbaren Speichers innerhalb von NovaOS.

Ziel ist eine effiziente Speichernutzung mit minimaler Fragmentierung, kontrollierten Speicherzugriffen und möglichst geringen CPU-GPU-Datenübertragungen.

Die Architektur MUSS dedizierte GPUs, integrierte GPUs und unterschiedliche Speicherarchitekturen unterstützen.

## 2. Architekturprinzipien

- **Hardware-Independent:** Einheitliches Speichermodell für unterschiedliche GPU-Architekturen.
- **Memory-Efficient:** Speicher wird bedarfsgerecht verwendet.
- **Allocation-Aware:** Speicherzuweisungen berücksichtigen Größe, Alignment und Verwendungszweck.
- **Residency-Aware:** Ressourcen müssen vor ihrer Verwendung verfügbar sein.
- **Budget-Aware:** Speicherverbrauch unterliegt definierten Budgets.
- **Zero-Copy:** Unnötige Speichertransfers werden vermieden.
- **Synchronization-Safe:** Speicher bleibt während aktiver GPU-Zugriffe gültig.
- **Capability-Based:** Speicherzugriffe werden kontrolliert autorisiert.

## 3. GPU Memory Model

NovaOS unterscheidet logisch folgende Speicherklassen:

| Speicherklasse | Beschreibung |
|---|---|
| Device Local | Für GPU-Zugriffe optimierter Speicher |
| Host Visible | Direkt durch die CPU adressierbarer GPU-Speicher |
| Host Coherent | Speicher mit geeigneter CPU-GPU-Kohärenz |
| Host Cached | CPU-seitig cachebarer Speicher |
| Shared Memory | Gemeinsam verwendeter Systemspeicher |
| Staging Memory | Speicher für kontrollierte Datentransfers |

Speicherklassen DÜRFEN sich überschneiden.

Ihre tatsächlichen Eigenschaften MÜSSEN vom GPU-Backend ermittelt werden.

## 4. Memory Allocation

Speicherzuweisungen MÜSSEN mindestens folgende Anforderungen berücksichtigen:

- Speichergröße
- Alignment
- Speicherklasse
- Ressourcenverwendung
- Hardwarebeschränkungen
- Speicherbudget
- Erforderliche Zugriffsrechte

NovaOS SOLL kleine Speicheranforderungen durch geeignete Suballocation-Verfahren verwalten.

Große oder spezialisierte Ressourcen DÜRFEN eigene Speicherzuweisungen erhalten.

Die konkreten Allokationsstrategien werden durch Nova GPU Memory Allocation definiert.

## 5. GPU Address Space

NovaOS MUSS GPU-adressierbare Speicherbereiche kontrolliert verwalten.

Dabei gilt:

1. GPU-Adressen werden durch das GPU-Backend bereitgestellt.
2. Anwendungen erhalten keine unautorisierten physischen Speicheradressen.
3. Speicherbereiche werden eindeutig ihren Ressourcen zugeordnet.
4. GPU-seitige Adressübersetzungen werden entsprechend den Hardwarefähigkeiten eingerichtet.
5. Ungültige Speicherzugriffe MÜSSEN soweit technisch möglich verhindert oder erkannt werden.

Eine GPU-MMU oder IOMMU SOLL zur Speicherisolation verwendet werden, sofern verfügbar.

## 6. Memory Mapping

NovaOS MUSS kontrollierte CPU-Zugriffe auf geeignete GPU-Speicherbereiche ermöglichen.

Dabei gilt:

- Nur CPU-sichtbarer Speicher DARF direkt gemappt werden.
- Mapping-Rechte MÜSSEN validiert werden.
- Speicher- und Cache-Kohärenz MÜSSEN berücksichtigt werden.
- Gleichzeitige inkompatible CPU- und GPU-Zugriffe MÜSSEN verhindert werden.
- Unmapping DARF aktive GPU-Zugriffe nicht unzulässig beeinträchtigen.

Nicht kohärenter Speicher benötigt geeignete Flush- und Invalidate-Operationen.

## 7. Memory Residency

Nova GPU Memory Management MUSS mit Nova GPU Resource Residency zusammenarbeiten.

Dabei gilt:

- Benötigte Ressourcen müssen vor ihrer GPU-Verwendung verfügbar sein.
- Residency-Änderungen werden kontrolliert durchgeführt.
- Verdrängte Ressourcen werden bei Bedarf wiederhergestellt.
- Ressourcen mit aktiven GPU-Zugriffen DÜRFEN nicht unzulässig verdrängt werden.
- Hardware ohne dynamische Residency-Verwaltung MUSS unterstützt werden.

## 8. Memory Fragmentation

NovaOS SOLL Speicherfragmentierung minimieren.

Geeignete Verfahren sind:

- Suballocation
- Memory Pools
- Freispeicherzusammenführung
- Größenklassen
- Kontrollierte Speicherkompaktierung

Eine Verschiebung aktiver GPU-Ressourcen DARF nur erfolgen, wenn ihre Adressierung, Synchronisation und Lebensdauer korrekt behandelt werden.

Nicht verschiebbare Ressourcen MÜSSEN berücksichtigt werden.

## 9. Memory Sharing und Zero-Copy

NovaOS SOLL gemeinsame GPU-Speichernutzung ohne unnötige Kopien ermöglichen.

Voraussetzungen:

- Kompatible Speicherformate und Layouts.
- Geeignete Hardwareunterstützung.
- Autorisierte Ressourcenfreigabe.
- Korrekte CPU-GPU- und GPU-GPU-Synchronisation.
- Kontrollierte Speicherlebensdauer.

Zero-Copy DARF nicht erzwungen werden, wenn dadurch Sicherheit oder Leistung beeinträchtigt werden.

## 10. Memory Budgeting

Nova GPU Memory Management MUSS mit Nova GPU Memory Budgeting zusammenarbeiten.

Berücksichtigt werden:

- Verfügbarer GPU-Speicher.
- Aktuelle Speicherbelegung.
- Reservierte Speicherbereiche.
- Prozessbezogene Budgets.
- Systemkritische Ressourcen.
- Temporäre GPU-Ressourcen.

Neue Speicheranforderungen MÜSSEN anhand verfügbarer Ressourcen bewertet werden.

## 11. Memory Pressure

Bei GPU-Speichermangel MUSS NovaOS kontrolliert reagieren.

Mögliche Maßnahmen:

1. Freigabe ungenutzter Speicherbereiche.
2. Reduzierung von Resource Pools.
3. Verdrängung wiederherstellbarer Ressourcen.
4. Wiederverwendung temporärer Speicherbereiche.
5. Verwendung geeigneter alternativer Speicherklassen.
6. Kontrollierte Ablehnung neuer Speicheranforderungen.

Speichermangel DARF keine unkontrollierte Beschädigung bestehender GPU-Ressourcen verursachen.

## 12. Synchronization Integration

Nova GPU Memory Management MUSS mit Nova GPU Synchronization zusammenarbeiten.

Dabei gilt:

- Aktive GPU-Zugriffe werden nachverfolgt.
- Speicherfreigaben berücksichtigen Fences und Timeline-Werte.
- CPU-GPU-Speichersichtbarkeit wird gewährleistet.
- Queueübergreifende Zugriffe werden korrekt synchronisiert.
- Resource Aliasing berücksichtigt erforderliche Speicherbarrieren.

Speicher DARF erst wiederverwendet werden, wenn alle relevanten Zugriffe abgeschlossen sind.

## 13. Sicherheit und Isolation

GPU-Speicher MUSS entsprechend den NovaOS-Sicherheitsrichtlinien geschützt werden.

Dabei gilt:

- Speicherzuweisungen besitzen definierte Eigentümer.
- Zugriffe erfolgen capability-basiert.
- Prozessübergreifende Freigaben erfordern ausdrückliche Berechtigungen.
- Freigegebene Speicherbereiche MÜSSEN vor unautorisiertem Zugriff geschützt werden.
- Wiederverwendeter Speicher DARF keine fremden Daten offenlegen.

Hardwarebedingte Einschränkungen der GPU-Speicherisolation MÜSSEN durch geeignete Treiber- und Systemrichtlinien berücksichtigt werden.

## 14. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Fehlgeschlagene Speicherzuweisungen.
- Ungültige Speicher-Mappings.
- Überschrittene Speicherbudgets.
- Nicht unterstützte Speicherklassen.
- GPU-Speicherzugriffsfehler.
- Synchronisationsfehler.
- Geräteverlust.

Dabei gilt:

- Fehlerhafte Speicherbereiche DÜRFEN nicht weiterverwendet werden.
- Ressourcen MÜSSEN kontrolliert invalidiert oder wiederhergestellt werden können.
- Noch aktive GPU-Zugriffe MÜSSEN bei der Fehlerbehandlung berücksichtigt werden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale GPU-Speicherverwaltung bereitstellen.
2. Dedizierte und integrierte GPU-Speicherarchitekturen MÜSSEN unterstützt werden.
3. Speicherklassen MÜSSEN anhand tatsächlicher Hardwarefähigkeiten ermittelt werden.
4. Speicherzuweisungen MÜSSEN Größe, Alignment und Verwendung berücksichtigen.
5. GPU-Adressräume MÜSSEN kontrolliert verwaltet werden.
6. CPU-Mapping MUSS für geeignete Speicherbereiche unterstützt werden.
7. Cache-Kohärenz MUSS berücksichtigt werden.
8. Speicherfragmentierung SOLL minimiert werden.
9. Resource Residency MUSS integriert werden.
10. GPU-Speicherbudgets MÜSSEN berücksichtigt werden.
11. Memory Pressure MUSS kontrolliert behandelt werden.
12. Zero-Copy SOLL bevorzugt werden, sofern sicher und effizient.
13. Speicherfreigaben MÜSSEN GPU-Synchronisationsbedingungen berücksichtigen.
14. Resource Aliasing SOLL unterstützt werden.
15. Speicherzugriffe MÜSSEN capability-basiert geschützt werden.
16. Wiederverwendeter Speicher DARF keine fremden Daten offenlegen.
17. Bei fehlender Hardwareunterstützung MÜSSEN sichere Fallback-Strategien verfügbar sein.
18. Im deterministischen Modus MÜSSEN logische Speicherverwaltungsentscheidungen bei identischen Eingaben und Speicherbedingungen reproduzierbar sein.

## 16. Ergebnis

Nova GPU Memory Management stellt die zentrale Verwaltung des GPU-adressierbaren Speichers innerhalb von NovaOS bereit.

Durch hardwareabhängige Speicherklassenauswahl, effiziente Speicherzuweisung, kontrolliertes Mapping, Residency Management und Synchronisation wird der verfügbare GPU-Speicher möglichst effizient genutzt.

Damit erhält NovaOS eine sichere und skalierbare Speichergrundlage für Rendering, Compute, Compositor und Display Server.