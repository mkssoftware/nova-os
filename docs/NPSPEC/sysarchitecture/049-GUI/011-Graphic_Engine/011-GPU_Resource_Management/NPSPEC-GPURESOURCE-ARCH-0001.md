# NPSPEC-GPURESOURCE-ARCH-0001 – Nova GPU Resource Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Resource Architecture definiert die zentrale Architektur zur Verwaltung sämtlicher GPU-Ressourcen innerhalb von NovaOS.

Ziel ist eine effiziente, sichere und hardwareunabhängige Ressourcenverwaltung mit minimalem Speicherverbrauch, kontrollierten Ressourcenlebenszyklen und möglichst geringer CPU-GPU-Kommunikation.

Die Architektur MUSS sowohl GPUs mit dediziertem Videospeicher als auch integrierte GPUs mit gemeinsam genutztem Systemspeicher unterstützen.

## 2. Architekturprinzipien

- **Resource-Oriented:** GPU-Ressourcen werden als eigenständige verwaltete Objekte behandelt.
- **Backend-Independent:** Ressourcen besitzen eine hardwareunabhängige Beschreibung.
- **Lifetime-Aware:** Ressourcen werden erst nach Abschluss aller relevanten Zugriffe freigegeben.
- **Budget-Aware:** Speicherverbrauch wird anhand verfügbarer Budgets kontrolliert.
- **Residency-Aware:** Die Verfügbarkeit von Ressourcen im GPU-adressierbaren Speicher wird berücksichtigt.
- **Zero-Copy:** Unnötige Speichertransfers werden vermieden.
- **Cache-Aware:** Wiederverwendbare Ressourcen bleiben bei ausreichendem Speicher verfügbar.
- **Capability-Based:** Ressourcen werden durch NovaOS-Capabilities geschützt.

## 3. Architekturmodell

Nova GPU Resource Management besteht logisch aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Resource Manager | Zentrale Verwaltung von GPU-Ressourcen |
| Memory Manager | Verwaltung verfügbarer GPU-Speicherbereiche |
| Allocation Manager | Speicherzuweisung und Freigabe |
| Residency Manager | Verwaltung der Ressourcenverfügbarkeit |
| Budget Manager | Überwachung und Begrenzung des Speicherverbrauchs |
| Resource Pool Manager | Wiederverwendung geeigneter Ressourcen |
| Resource Cache | Zwischenspeicherung wiederverwendbarer Ressourcen |
| Eviction Manager | Kontrollierte Verdrängung entbehrlicher Ressourcen |
| Resource Tracker | Überwachung von Zuständen, Referenzen und Lebenszyklen |

Die Komponenten MÜSSEN über definierte Schnittstellen zusammenarbeiten.

## 4. GPU Resource Model

Jede GPU-Ressource besitzt eine logische Beschreibung.

| Eigenschaft | Beschreibung |
|---|---|
| Resource ID | Eindeutige Ressourcenidentifikation |
| Resource Type | Art der GPU-Ressource |
| Owner | Zugehöriger Prozess oder Systemdienst |
| Size | Benötigte Speichergröße |
| Format | Daten- oder Pixelformat |
| Usage | Zulässige Verwendungsarten |
| Memory Class | Erforderliche Speicherklasse |
| Residency State | Aktuelle Speicherverfügbarkeit |
| Resource State | Aktueller logischer Zustand |
| Lifetime | Gültigkeitsdauer |
| Backend Handle | Hardwareabhängige Referenz |

Logische Ressourcenidentität und physische Speicherzuweisung MÜSSEN getrennt verwaltet werden.

## 5. Resource Types

NovaOS MUSS mindestens folgende Ressourcentypen unterstützen:

- GPU Buffers
- Textures
- Render Targets
- Depth/Stencil Buffers
- Storage Images
- Staging Buffers
- Shared Surface Buffers
- Shader- und Pipeline-Ressourcen
- Temporäre Render-Graph-Ressourcen

Weitere Ressourcentypen DÜRFEN durch GPU-Backends ergänzt werden.

## 6. Memory Management

Nova GPU Resource Architecture MUSS unterschiedliche Speicherklassen unterstützen.

Dazu gehören:

- Device-Local Memory
- Host-Visible Memory
- Host-Coherent Memory
- Host-Cached Memory
- Shared System Memory

Speichereigenschaften MÜSSEN anhand tatsächlicher Hardwarefähigkeiten ermittelt werden.

NovaOS SOLL die für den jeweiligen Verwendungszweck effizienteste Speicherklasse auswählen.

Eine bestimmte Speicherklasse DARF nicht auf jeder GPU vorausgesetzt werden.

## 7. Resource Lifecycle

GPU-Ressourcen besitzen einen kontrollierten Lebenszyklus:

`Created → Allocated → Active → Retired → Released`

Dabei gilt:

1. Ressourcen werden vor ihrer Verwendung validiert.
2. Speicher wird entsprechend den Ressourcenanforderungen zugewiesen.
3. Aktive GPU-Zugriffe werden nachverfolgt.
4. Nicht mehr benötigte Ressourcen werden zur Freigabe vorgemerkt.
5. Die tatsächliche Freigabe erfolgt erst nach Abschluss aller relevanten Zugriffe.

Ein logischer Destroy-Aufruf DARF keine noch verwendete GPU-Ressource vorzeitig freigeben.

## 8. Resource Residency

NovaOS MUSS die Verfügbarkeit von GPU-Ressourcen im erforderlichen Speicherbereich berücksichtigen.

Dabei gilt:

- Benötigte Ressourcen MÜSSEN vor ihrer Verwendung verfügbar sein.
- Residency-Änderungen werden kontrolliert durchgeführt.
- Nicht benötigte Ressourcen DÜRFEN verdrängt werden.
- Erforderliche Wiederherstellungen MÜSSEN vor abhängigen Zugriffen abgeschlossen sein.
- Nicht jede GPU unterstützt hardwareseitige Residency-Verwaltung.

Die konkrete Umsetzung erfolgt durch das jeweilige GPU-Backend.

## 9. Memory Budgeting

NovaOS MUSS GPU-Speicherbudgets verwalten können.

Dabei werden berücksichtigt:

- Gesamter verfügbarer GPU-Speicher.
- Aktuell belegter Speicher.
- Reservierter Speicher.
- Prozessbezogene Speicherbudgets.
- Systemkritische Ressourcen.
- Temporäre Rendering-Ressourcen.

Speicherbudgets SOLLEN dynamisch an verfügbare Hardware- und Systemressourcen angepasst werden.

Anwendungen DÜRFEN keine unbegrenzte GPU-Speicherbelegung voraussetzen.

## 10. Resource Pooling und Caching

NovaOS SOLL häufig benötigte GPU-Ressourcen wiederverwenden.

Dazu gehören:

- Temporäre Render Targets.
- Staging Buffers.
- Command-bezogene Speicherbereiche.
- Wiederverwendbare Texturen.
- Render-Graph-Zwischenbuffer.

Dabei gilt:

- Ressourcen müssen vor Wiederverwendung kompatibel sein.
- Noch aktive GPU-Zugriffe MÜSSEN berücksichtigt werden.
- Cache- und Pool-Ressourcen unterliegen Speicherbudgets.
- Nicht benötigte Ressourcen DÜRFEN verdrängt werden.

Resource Pooling und Resource Caching MÜSSEN logisch unterscheidbar bleiben.

## 11. Resource Aliasing

NovaOS SOLL die gemeinsame Nutzung physischer Speicherbereiche durch Ressourcen mit nicht überlappenden Lebensdauern unterstützen.

Dabei gilt:

- Ressourcenlebensdauern MÜSSEN analysiert werden.
- Überlappende aktive Zugriffe MÜSSEN verhindert werden.
- Erforderliche Aliasing Barriers werden berücksichtigt.
- Speicherzuweisungen bleiben eindeutig nachvollziehbar.
- Resource Aliasing DARF keine Datenisolation verletzen.

Die Planung temporärer Render-Graph-Ressourcen erfolgt in Zusammenarbeit mit Nova Transient Resource Aliasing.

## 12. Synchronization Integration

Nova GPU Resource Architecture MUSS mit Nova GPU Synchronization zusammenarbeiten.

Dabei gilt:

- GPU-Zugriffe werden über Resource Hazard Tracking berücksichtigt.
- Ressourcenfreigaben beachten GPU Fences und Timeline-Werte.
- Queue Ownership Transfers werden unterstützt.
- CPU-GPU-Speichersichtbarkeit wird gewährleistet.
- Shared Surfaces verwenden kontrollierte Acquire- und Release-Synchronisation.

GPU-Ressourcen DÜRFEN nicht allein aufgrund abgeschlossener CPU-Operationen als frei betrachtet werden.

## 13. Zero-Copy und Resource Sharing

NovaOS SOLL GPU-Ressourcen möglichst ohne unnötige Speichertransfers gemeinsam verwenden.

Voraussetzungen:

- Kompatible Speicherformate.
- Geeignete Hardwareunterstützung.
- Gültige Zugriffsberechtigungen.
- Korrekte Synchronisation.
- Kontrollierte Ressourcenlebensdauer.

Zero-Copy DARF nicht erzwungen werden, wenn eine Kopie für Kompatibilität, Sicherheit oder Leistung erforderlich ist.

## 14. Memory Pressure Management

NovaOS MUSS auf GPU-Speichermangel kontrolliert reagieren.

Mögliche Maßnahmen:

1. Freigabe ungenutzter Cache-Ressourcen.
2. Reduzierung temporärer Ressourcenbestände.
3. Verdrängung wiederherstellbarer Ressourcen.
4. Anpassung der Rendering-Qualität.
5. Verwendung alternativer Speicherklassen.
6. Kontrollierte Ablehnung neuer Ressourcenanforderungen.

Systemkritische Ressourcen SOLLEN bevorzugt geschützt werden.

## 15. Sicherheit und Isolation

GPU-Ressourcen MÜSSEN capability-basiert geschützt werden.

Dabei gilt:

- Jede Ressource besitzt einen definierten Eigentümer.
- Zugriffsrechte MÜSSEN validiert werden.
- Prozessübergreifende Freigaben erfordern entsprechende Capabilities.
- Ressourcenhandles DÜRFEN keine unautorisierten Speicherzugriffe ermöglichen.
- Wiederverwendete Speicherbereiche MÜSSEN gegen unzulässige Datenoffenlegung geschützt werden.

Die konkrete Speicherisolation MUSS durch geeignete Treiber- und Hardwaremechanismen umgesetzt werden.

## 16. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Fehlgeschlagene Speicherzuweisungen.
- Überschrittene Speicherbudgets.
- Ungültige Ressourcenhandles.
- Nicht unterstützte Ressourcenformate.
- Fehlgeschlagene Residency-Operationen.
- Synchronisationsfehler.
- GPU-Timeouts.
- Geräteverlust.

Dabei gilt:

- Fehlerhafte Ressourcen DÜRFEN nicht weiterverwendet werden.
- Noch aktive GPU-Zugriffe MÜSSEN berücksichtigt werden.
- Ressourcen MÜSSEN kontrolliert invalidiert oder wiederhergestellt werden können.
- Speicherfehler DÜRFEN keine unkontrollierte Beschädigung anderer Ressourcen verursachen.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale GPU-Ressourcenarchitektur bereitstellen.
2. Logische Ressourcen und physische Speicherzuweisungen MÜSSEN getrennt verwaltet werden.
3. GPU Buffers und Textures MÜSSEN unterstützt werden.
4. Unterschiedliche GPU-Speicherklassen MÜSSEN berücksichtigt werden.
5. Ressourcenlebenszyklen MÜSSEN kontrolliert verwaltet werden.
6. Ressourcen DÜRFEN nicht während aktiver GPU-Zugriffe freigegeben werden.
7. GPU-Speicherbudgets MÜSSEN unterstützt werden.
8. Resource Residency MUSS berücksichtigt werden.
9. Resource Pooling SOLL unterstützt werden.
10. Resource Caching SOLL unterstützt werden.
11. Resource Aliasing SOLL für geeignete Ressourcen verfügbar sein.
12. GPU Synchronization MUSS integriert werden.
13. Zero-Copy SOLL bevorzugt werden, sofern sicher und effizient.
14. GPU Memory Pressure MUSS kontrolliert behandelt werden.
15. Ressourcen MÜSSEN capability-basiert geschützt werden.
16. Prozessübergreifende Ressourcenfreigaben MÜSSEN autorisiert sein.
17. Unterschiedliche GPU-Architekturen MÜSSEN unterstützt werden.
18. Bei fehlender Hardwareunterstützung MÜSSEN sichere Fallback-Strategien verfügbar sein.
19. Im deterministischen Modus MÜSSEN logische Ressourcenentscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 18. Ergebnis

Nova GPU Resource Architecture bildet die zentrale Grundlage für die Verwaltung sämtlicher GPU-Ressourcen innerhalb von NovaOS.

Durch hardwareunabhängige Ressourcenmodelle, kontrollierte Speicherzuweisungen, Resource Pooling, Caching, Residency Management und GPU-Synchronisation werden Speicherverbrauch und Verwaltungsaufwand reduziert.

Damit erhält NovaOS eine skalierbare, sichere und ressourceneffiziente GPU-Architektur für Rendering, Compute, Compositor und Display Server.