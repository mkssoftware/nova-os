# NPSPEC-OPTIMIZE-BATCHING-0001 – Nova Draw Call Batching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Optimization

## 1. Zweck

Nova Draw Call Batching definiert die Zusammenfassung kompatibler Zeichenoperationen zu möglichst wenigen GPU Draw Calls.

Ziel ist die Reduzierung des CPU-Aufwands für die GPU-Befehlsvorbereitung, der Treiber- und Submission-Kosten sowie unnötiger Pipeline- und Ressourcenwechsel.

Die Zusammenfassung MUSS die korrekte Darstellung, Zeichenreihenfolge und Ressourcenabhängigkeiten erhalten.

## 2. Architekturprinzipien

- **State-Aware:** Zeichenoperationen werden anhand kompatibler GPU-Zustände gruppiert.
- **Pipeline-Aware:** Gemeinsame Shader- und Pipeline-Konfigurationen werden bevorzugt.
- **Order-Preserving:** Sichtbare Zeichenreihenfolgen bleiben erhalten.
- **Material-Aware:** Kompatible Materialoperationen können zusammengefasst werden.
- **Region-Aware:** Clipping und Damage Regions werden berücksichtigt.
- **Incremental:** Unveränderte Batches können wiederverwendet werden.
- **Cost-Based:** Batching erfolgt nur bei erwartetem Leistungsgewinn.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Batch-Ergebnisse.

## 3. Architektur

Nova Draw Call Batching verwendet Nova GPU Command Architecture und Nova Render Graph.

| Komponente | Verantwortung |
|---|---|
| Batch Collector | Sammlung kompatibler Zeichenoperationen |
| Batch Compatibility Resolver | Prüfung gemeinsamer GPU-Zustände |
| Batch Builder | Erstellung zusammengefasster Zeichenbefehle |
| Batch State Cache | Wiederverwendung gültiger Batch-Konfigurationen |
| Batch Cost Evaluator | Bewertung des Batching-Nutzens |
| Batch Submission Adapter | Übergabe an GPU Command Buffers |

Eine eigenständige GPU-Command- oder Ressourcenverwaltung DARF nicht entstehen.

## 4. Batch Model

Ein Draw Batch MUSS mindestens folgende Informationen besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Batch ID | Eindeutige Identifikation |
| Pipeline Reference | Verwendete Grafikpipeline |
| Material Reference | Zugehörige Materialkonfiguration |
| Resource Bindings | Benötigte GPU-Ressourcen |
| Render Target | Zieloberfläche |
| Draw Operations | Enthaltene Zeichenoperationen |
| Clip State | Verwendete Clipping-Konfiguration |
| Draw Order | Verbindliche Zeichenreihenfolge |
| Batch Version | Version des Batch-Zustands |

Batch-Daten MÜSSEN während ihrer GPU-Ausführung unveränderlich bleiben.

## 5. Batch Compatibility

Zeichenoperationen DÜRFEN zusammengefasst werden, wenn ihre Ausführung innerhalb eines gemeinsamen Batches korrekt möglich ist.

Dabei MÜSSEN insbesondere berücksichtigt werden:

- Grafikpipeline und Shader.
- Render Target und Attachment-Konfiguration.
- Material- und Blend-Zustände.
- Texturen und Resource Bindings.
- Vertex- und Indexformate.
- Clipping und Scissor Regions.
- Depth- und Stencil-Zustände.
- Zeichenreihenfolge.
- Ressourcen- und Synchronisationsabhängigkeiten.

Unterschiedliche Parameter DÜRFEN über Instanzdaten oder geeignete GPU-Buffer bereitgestellt werden.

## 6. Batching Strategies

NovaOS SOLL folgende Verfahren unterstützen:

| Strategie | Beschreibung |
|---|---|
| State Batching | Gruppierung identischer GPU-Zustände |
| Material Batching | Zusammenfassung kompatibler Materialoperationen |
| Texture Batching | Gemeinsame Verarbeitung kompatibler Texturzugriffe |
| Geometry Batching | Zusammenfassung kleiner Geometrieoperationen |
| Instanced Rendering | Gemeinsame Darstellung wiederholter Geometrien |
| Multi-Draw | Mehrere Zeichenoperationen innerhalb eines GPU-Befehls |
| Indirect Drawing | GPU-gesteuerte Zeichenparameter, sofern unterstützt |

Die Auswahl MUSS von den verfügbaren Hardwarefunktionen und erwarteten Kosten abhängen.

## 7. Draw Order Preservation

NovaOS MUSS die korrekte Zeichenreihenfolge gewährleisten.

Dabei gilt:

- Transparente Zeichenoperationen DÜRFEN nicht beliebig umgeordnet werden.
- Blend-Abhängigkeiten MÜSSEN erhalten bleiben.
- Opake Operationen DÜRFEN umgeordnet werden, wenn das Ergebnis unverändert bleibt.
- Material- und Backdrop-Abhängigkeiten MÜSSEN berücksichtigt werden.
- Render-Pass-Grenzen DÜRFEN nicht ohne gültige Abhängigkeitsanalyse überschritten werden.

Eine Reduzierung der Draw Calls DARF keine visuellen Fehler verursachen.

## 8. Incremental Batching

NovaOS MUSS die inkrementelle Aktualisierung von Draw Batches unterstützen.

Dabei gilt:

- Unveränderte Batch-Konfigurationen SOLLEN wiederverwendet werden.
- Änderungen einzelner Zeichenoperationen SOLLEN nur betroffene Batches aktualisieren.
- Gültige GPU Command Buffers können wiederverwendet werden.
- Änderungen an Pipeline- oder Resource Bindings MÜSSEN berücksichtigt werden.
- Veraltete Batch-Versionen DÜRFEN nicht erneut verwendet werden.

Batch-Wiederverwendung MUSS mit Nova Render Command Reuse zusammenarbeiten.

## 9. Damage-Aware Batching

Nova Draw Call Batching MUSS Nova Damage Tracking berücksichtigen.

Dabei gilt:

- Zeichenoperationen außerhalb relevanter Damage Regions SOLLEN entfallen.
- Kompatible Operationen innerhalb betroffener Regionen SOLLEN zusammengefasst werden.
- Clipping- und Scissor-Informationen MÜSSEN erhalten bleiben.
- Batch-Grenzen DÜRFEN an Damage Regions angepasst werden.
- Eine Zusammenfassung DARF keine unnötige vollständige Neuberechnung erzwingen.

## 10. GPU Resource Integration

NovaOS MUSS Batching mit Nova GPU Resource Management integrieren.

Dabei gilt:

- Vertex- und Indexdaten SOLLEN effizient gespeichert werden.
- Instanzdaten SOLLEN gemeinsam nutzbare Buffer verwenden.
- Resource Bindings SOLLEN unnötige Zustandswechsel vermeiden.
- Temporäre Batch-Buffer SOLLEN wiederverwendet werden.
- Ressourcenlebenszeiten und GPU-Synchronisation MÜSSEN eingehalten werden.

Zero-Copy SOLL verwendet werden, sofern technisch möglich und sicher.

## 11. Cost-Based Batching

NovaOS SOLL den Nutzen von Batching anhand folgender Faktoren bewerten:

- Anzahl eingesparter Draw Calls.
- CPU-Aufwand für Batch-Erstellung.
- GPU-Ausführungszeit.
- Anzahl notwendiger Pipeline-Wechsel.
- Speicherverbrauch zusätzlicher Batch-Daten.
- Datenübertragungsaufwand.
- Kosten inkrementeller Aktualisierungen.

Batching DARF übersprungen werden, wenn die Zusammenfassung höhere Kosten verursacht als die getrennte Ausführung.

## 12. Render Graph Integration

Nova Draw Call Batching MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Batching erfolgt innerhalb gültiger Render-Pass- und Abhängigkeitsgrenzen.
- Kompatible Zeichenoperationen SOLLEN gemeinsam vorbereitet werden.
- GPU Resource Barriers MÜSSEN erhalten bleiben.
- Parallel Command Recording DARF unterstützt werden.
- Die endgültige GPU-Ausführung erfolgt über Nova GPU Command Submission.

Draw Call Batching und Render Pass Merging bleiben getrennte Optimierungsverfahren.

## 13. Sicherheit und Fallback

NovaOS MUSS folgende Situationen sicher behandeln:

- Inkompatible GPU-Zustände.
- Ungültige Resource Bindings.
- Veraltete Batch-Konfigurationen.
- Nicht unterstützte Instancing- oder Multi-Draw-Funktionen.
- Ressourcenmangel.
- Fehlgeschlagene Batch-Erstellung.

Bei fehlender Kompatibilität MÜSSEN Zeichenoperationen getrennt ausgeführt werden können.

Batching DARF keine Ressourcen- oder Prozessisolationsgrenzen verletzen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Draw Call Batching unterstützen.
2. Kompatible Zeichenoperationen SOLLEN zusammengefasst werden.
3. GPU-Pipeline- und Ressourcenkompatibilität MUSS geprüft werden.
4. Die korrekte Zeichenreihenfolge MUSS erhalten bleiben.
5. Transparenz- und Blend-Abhängigkeiten MÜSSEN berücksichtigt werden.
6. State Batching MUSS unterstützt werden.
7. Material Batching SOLL unterstützt werden.
8. Instanced Rendering SOLL unterstützt werden, sofern verfügbar.
9. Multi-Draw SOLL unterstützt werden, sofern verfügbar.
10. Inkrementelle Batch-Aktualisierung MUSS möglich sein.
11. Gültige Batch-Konfigurationen SOLLEN wiederverwendet werden.
12. Nova Damage Tracking MUSS berücksichtigt werden.
13. Nova Render Command Reuse MUSS integrierbar sein.
14. Nova Render Graph MUSS integriert werden.
15. Nova GPU Resource Management MUSS berücksichtigt werden.
16. Batching-Entscheidungen SOLLEN kostenbasiert erfolgen.
17. GPU-Synchronisation und Ressourcenlebenszeiten MÜSSEN eingehalten werden.
18. Bei inkompatiblen Operationen MUSS getrenntes Rendering möglich bleiben.
19. Im deterministischen Modus MÜSSEN Batch-Entscheidungen bei identischen Eingaben reproduzierbar sein.

## 15. Ergebnis

Nova Draw Call Batching reduziert den CPU- und GPU-Verwaltungsaufwand durch die Zusammenfassung kompatibler Zeichenoperationen.

Durch State Batching, Material Batching, Instanced Rendering und inkrementelle Batch-Wiederverwendung werden unnötige Draw Calls und Zustandswechsel vermieden.

Damit kann NovaOS auch komplexe grafische Oberflächen mit zahlreichen UI-Elementen effizient darstellen, ohne für jedes einzelne Element einen separaten GPU Draw Call ausführen zu müssen.