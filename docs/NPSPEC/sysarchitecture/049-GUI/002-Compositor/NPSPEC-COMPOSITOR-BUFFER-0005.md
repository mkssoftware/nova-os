# NPSPEC-COMPOSITOR-BUFFER-0005 – Nova GPU Buffer Allocation

**Status:** Angenommen  
**Bereich:** Compositor / GPU-Speicherverwaltung  
**System:** NovaOS

## 1. Zweck

Nova GPU Buffer Allocation definiert die effiziente und sichere Reservierung, Verwaltung und Freigabe von Grafikbuffern im GPU-Speicher.

Ziel ist eine performante Speicherverwaltung mit geringem Allokationsaufwand, kontrolliertem Ressourcenverbrauch und optimaler Nutzung der verfügbaren Grafikhardware.

## 2. Architektur

- Integration in Nova Graphics Buffer Management.
- Hardwareunabhängige Allokationsschnittstelle.
- Gerätespezifische GPU-Memory-Backends.
- Trennung zwischen logischen Buffern und physischer Speicherplatzierung.
- Unterstützung dedizierter und gemeinsam genutzter Grafikspeicherbereiche.
- Capability-basierte Ressourcenverwaltung.
- Integration in Nova Buffer Synchronization.
- Unterstützung von Buffer-Pools und Speicherwiederverwendung.
- Zusammenarbeit mit dem NovaOS-Speichermanagement.

## 3. Kernfunktionen

- Allokation und Freigabe von GPU-Buffern.
- Auswahl geeigneter Speicherbereiche.
- Verwaltung von Speichergröße, Alignment und Layout.
- Unterstützung linearer und GPU-optimierter Speicherlayouts.
- Verwaltung unterschiedlicher Pixelformate.
- Unterstützung von Rendering-, Texture- und Scanout-Buffern.
- Wiederverwendung vorhandener Speicherressourcen.
- Verwaltung von GPU-Speicherbudgets.
- Erkennung und Behandlung von Speicherknappheit.
- Unterstützung von Speicherresidenz und Migration.
- Überwachung von Fragmentierung und Speicherverbrauch.

## 4. Normative Anforderungen

- BUFFER-001: GPU-Speicherallokationen MÜSSEN über eine definierte Schnittstelle erfolgen.
- BUFFER-002: Speicheranforderungen MÜSSEN vor der Allokation validiert werden.
- BUFFER-003: Größe, Alignment, Format und Nutzungszweck MÜSSEN berücksichtigt werden.
- BUFFER-004: Die Speicherplatzierung MUSS durch das jeweilige GPU-Backend bestimmt werden können.
- BUFFER-005: Dedizierter und gemeinsam genutzter Grafikspeicher MÜSSEN unterstützt werden, sofern die Hardware dies bereitstellt.
- BUFFER-006: Speicherallokationen MÜSSEN gegen unberechtigte Zugriffe isoliert werden.
- BUFFER-007: Nicht initialisierte Speicherinhalte DÜRFEN nicht an andere Clients gelangen.
- BUFFER-008: GPU-Speicherbudgets MÜSSEN pro Client begrenzbar sein.
- BUFFER-009: Speicherknappheit MUSS kontrolliert behandelt werden.
- BUFFER-010: Geeignete Speicherressourcen SOLLEN wiederverwendet werden.
- BUFFER-011: Buffer-Pools SOLLEN zur Reduzierung häufiger Allokationen eingesetzt werden.
- BUFFER-012: GPU-Buffer DÜRFEN während aktiver Nutzung nicht freigegeben oder inkompatibel verschoben werden.
- BUFFER-013: Speicherfreigaben MÜSSEN mit Nova Buffer Synchronization abgestimmt werden.
- BUFFER-014: GPU-spezifische Speicherlayouts MÜSSEN bei Import und Export berücksichtigt werden.
- BUFFER-015: Speicherresidenz und Migration MÜSSEN bei entsprechender Hardwareunterstützung verwaltet werden können.
- BUFFER-016: Fehlgeschlagene Allokationen DÜRFEN den Compositor nicht destabilisieren.
- BUFFER-017: Verlorene GPU-Speicherressourcen MÜSSEN nach einem GPU-Reset erkannt und invalidiert werden.
- BUFFER-018: Der tatsächliche GPU-Speicherverbrauch MUSS überwacht werden können.

## 5. Speicherklassen

### Device Local Memory
GPU-optimierter Speicher für Rendering, Texturen und Bildkomposition.

### Host Visible Memory
CPU-zugänglicher Grafikspeicher für Datenaustausch und Upload-Operationen.

### Shared GPU Memory
Gemeinsam nutzbarer Speicher für integrierte Grafikeinheiten oder kompatible Geräte.

### Scanout Memory
Speicherressourcen, die für die direkte Bildausgabe geeignet sind.

### Transient Memory
Kurzlebige Speicherbereiche für temporäre Rendering-Operationen.

Die Speicherklassen beschreiben Eigenschaften und Verwendungszwecke. Sie MÜSSEN nicht unterschiedlichen physischen Speicherbereichen entsprechen.

## 6. Allokationsstrategien

### Dedicated Allocation
Eigene Speicherallokation für große oder besonders anspruchsvolle Ressourcen.

### Suballocation
Aufteilung größerer Speicherblöcke zur Reduzierung des Allokationsaufwands.

### Buffer Pooling
Wiederverwendung kompatibler Grafikbuffer.

### Transient Allocation
Zeitlich begrenzte Nutzung von Speicher für temporäre Rendering-Ressourcen.

### Memory Budgeting
Kontrollierte Verteilung verfügbarer GPU-Speicherressourcen.

### Memory Eviction
Freigabe oder Auslagerung geeigneter Ressourcen bei Speicherknappheit, sofern deren Nutzung und Wiederherstellbarkeit dies erlauben.

## 7. Allokationsablauf

1. **Request:** Speicheranforderung empfangen.
2. **Validate:** Größe, Format, Alignment und Berechtigungen prüfen.
3. **Select:** Geeignete Speicherklasse bestimmen.
4. **Reserve:** Speicherbudget prüfen und reservieren.
5. **Allocate:** Speicherblock zuweisen oder vorhandenen Pool verwenden.
6. **Initialize:** Buffer und Metadaten initialisieren.
7. **Use:** Ressource für Grafikoperationen bereitstellen.
8. **Synchronize:** Laufende Zugriffe vor der Freigabe abschließen.
9. **Recycle:** Geeignete Ressourcen wiederverwenden.
10. **Release:** Nicht mehr benötigten Speicher freigeben.

## 8. Performance und Zuverlässigkeit

- Minimierung häufiger GPU-Speicherallokationen.
- Reduzierung von Speicherfragmentierung.
- Wiederverwendung kompatibler Buffer.
- Bevorzugung geeigneter GPU-lokaler Speicherbereiche.
- Vermeidung unnötiger Speichertransfers.
- Asynchrone Ressourcenfreigabe.
- Dynamische Anpassung an verfügbare Speicherbudgets.
- Kontrollierte Reduzierung temporärer Speicheranforderungen.
- Schutz vor Speicherlecks und Use-after-Free.
- Wiederherstellung nach GPU-Speicherverlust.
- Erfassung von Speicherverbrauch, Fragmentierung und Allokationszeiten.

## 9. Ergebnis

NovaOS erhält eine flexible und hardwareunabhängige Architektur zur Verwaltung von GPU-Speicherressourcen.

Grafikbuffer können effizient allokiert, wiederverwendet und freigegeben werden.

Durch Speicherbudgets, Buffer-Pools und gerätespezifische Optimierungen werden Allokationsaufwand, Fragmentierung und Speicherverbrauch reduziert, während die Stabilität des Compositors erhalten bleibt.