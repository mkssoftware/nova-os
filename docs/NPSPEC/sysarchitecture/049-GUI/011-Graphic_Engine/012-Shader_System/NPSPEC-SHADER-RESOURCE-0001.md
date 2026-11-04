# NPSPEC-SHADER-RESOURCE-0001 – Nova Shader Resource Binding

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Shader System

## 1. Zweck

Nova Shader Resource Binding definiert die sichere und effiziente Bereitstellung von GPU-Ressourcen für Shader innerhalb von NovaOS.

Ziel ist ein hardwareunabhängiges Bindungssystem für Texturen, Buffer, Sampler und Shader-Parameter, das unnötige Bindungswechsel reduziert und die korrekte Synchronisation gewährleistet.

## 2. Architekturprinzipien

- **Declarative:** Ressourcenbindungen werden durch definierte Layouts beschrieben.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-APIs.
- **Type-Safe:** Ressourcentypen und Zugriffsarten werden validiert.
- **Capability-Based:** Shader erhalten ausschließlich autorisierte Ressourcen.
- **Versioned:** Bindungszustände sind versioniert und nachvollziehbar.
- **Reuse-Oriented:** Kompatible Bindungskonfigurationen werden wiederverwendet.
- **Synchronization-Safe:** Ressourcenabhängigkeiten werden berücksichtigt.
- **Zero-Copy:** Ressourcen werden möglichst ohne Datenkopien gebunden.

## 3. Resource Binding Architecture

Nova Shader Resource Binding besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Binding Manager | Zentrale Verwaltung der Ressourcenbindungen |
| Binding Layout | Deklarative Beschreibung erwarteter Ressourcen |
| Binding Validator | Prüfung von Typen, Rechten und Kompatibilität |
| Binding Set | Konkrete Zuordnung von Ressourcen |
| Binding Cache | Wiederverwendung kompatibler Bindungskonfigurationen |
| Backend Adapter | Umsetzung in GPU-spezifische Bindungsmechanismen |

Die logische Bindungsbeschreibung MUSS von ihrer hardwareabhängigen Implementierung getrennt bleiben.

## 4. Resource Binding Model

Jede Ressourcenbindung besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Binding ID | Eindeutige Bindungsidentifikation |
| Layout ID | Zugehöriges Binding Layout |
| Binding Index | Position innerhalb des Layouts |
| Resource Type | Erwarteter Ressourcentyp |
| Resource Handle | Referenz auf die GPU-Ressource |
| Access Mode | Read, Write oder Read-Write |
| Shader Visibility | Zulässige Shader-Stufen |
| Resource Range | Verwendeter Ressourcenbereich |
| Binding Version | Version der Bindung |

Bindungsidentität und physische GPU-Speicheradresse MÜSSEN getrennt bleiben.

## 5. Unterstützte Ressourcen

NovaOS MUSS folgende grundlegende Ressourcenbindungen unterstützen:

| Ressource | Verwendung |
|---|---|
| Uniform Buffer | Shader-Konstanten und Parameter |
| Storage Buffer | Allgemeine GPU-Daten |
| Sampled Texture | Lesender Texturzugriff |
| Storage Texture | Shaderbasierter Lese- und Schreibzugriff |
| Sampler | Texturfilterung und Adressierung |
| Push Constants | Kleine unmittelbar gebundene Parameterbereiche |

Optionale Ressourcenarten DÜRFEN abhängig von den GPU-Fähigkeiten ergänzt werden.

## 6. Binding Layout

Ein Binding Layout definiert die erwartete Ressourcenstruktur eines Shaders.

Es MUSS mindestens beschreiben:

- Ressourcenposition und Typ.
- Zulässige Zugriffsart.
- Sichtbarkeit für Shader-Stufen.
- Erforderliche Buffer- und Texture-Eigenschaften.
- Array-Größe, sofern zutreffend.
- Dynamische Offsets, sofern unterstützt.

Binding Layouts MÜSSEN mit Nova Shader Pipeline kompatibel sein.

Kompatible Layouts SOLLEN wiederverwendet werden.

## 7. Binding Sets

NovaOS MUSS konkrete Ressourcenbindungen zu Binding Sets zusammenfassen können.

Dabei gilt:

- Binding Sets referenzieren ein gültiges Binding Layout.
- Ressourcen MÜSSEN vor der Verwendung validiert werden.
- Veröffentlichte Binding Sets MÜSSEN einen konsistenten Zustand besitzen.
- Änderungen an aktiven Bindungen DÜRFEN laufende GPU-Kommandos nicht beeinflussen.
- Neue Bindungsversionen SOLLEN ohne vollständige Neuerstellung unveränderter Ressourcen möglich sein.

Backend-spezifische Descriptor Sets, Descriptor Tables oder vergleichbare Mechanismen werden intern abgebildet.

## 8. Resource Validation

Vor der GPU-Ausführung MUSS NovaOS prüfen:

1. Ressourcenhandle und Ressourcengeneration sind gültig.
2. Ressourcentyp und Binding Layout stimmen überein.
3. Angeforderte Zugriffsrechte sind zulässig.
4. Buffer-Ranges und Texture-Subresources sind gültig.
5. Alignment und Hardwarelimits werden eingehalten.
6. Ressourcen sind verfügbar beziehungsweise resident.
7. Erforderliche Synchronisationsbedingungen sind erfüllt.

Ungültige Bindungen DÜRFEN nicht zur GPU-Ausführung gelangen.

## 9. Dynamic Resource Binding

NovaOS SOLL dynamische Ressourcenbindungen unterstützen.

Dazu gehören:

- Dynamische Uniform-Buffer-Offsets.
- Dynamische Storage-Buffer-Offsets.
- Austausch von Texture Views.
- Aktualisierung von Materialparametern.
- Wechsel kompatibler Binding Sets.

Dynamische Änderungen SOLLEN keine Shader-Neukompilierung erfordern.

Eine neue Pipeline-Version ist nur erforderlich, wenn sich kompilierungsrelevante Eigenschaften ändern.

## 10. Binding Cache und Optimierung

NovaOS SOLL kompatible Bindungskonfigurationen wiederverwenden.

Dabei gilt:

- Identische Binding Layouts können gemeinsam verwendet werden.
- Unveränderte Binding Sets SOLLEN wiederverwendet werden.
- Redundante Bindungswechsel SOLLEN vermieden werden.
- Cache-Schlüssel MÜSSEN Ressourcenidentität, Generation, Layout und relevante Bindungsparameter berücksichtigen.
- Ungültige oder veraltete Bindungen MÜSSEN invalidiert werden.

Bindungsoptimierungen DÜRFEN die definierte GPU-Ausführungsreihenfolge nicht verletzen.

## 11. Synchronization und Hazard Tracking

Nova Shader Resource Binding MUSS mit folgenden Subsystemen zusammenarbeiten:

- Nova GPU Resource Hazard Tracking.
- Nova GPU Resource Barriers.
- Nova GPU Synchronization.
- Nova GPU Resource Residency.

Dabei gilt:

- Shader-Zugriffsarten MÜSSEN für die Abhängigkeitsanalyse verfügbar sein.
- RAW-, WAR- und WAW-Konflikte MÜSSEN berücksichtigt werden.
- Ressourcenlayouts und Queue Ownership MÜSSEN korrekt behandelt werden.
- Ressourcen DÜRFEN nicht während aktiver GPU-Zugriffe unsicher ersetzt oder freigegeben werden.

Die Bindung einer Ressource ersetzt keine erforderliche Synchronisation.

## 12. Render Graph Integration

Nova Shader Resource Binding MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Ressourcenbindungen werden den jeweiligen Render Passes zugeordnet.
- Zugriffsarten und betroffene Ressourcenbereiche werden bereitgestellt.
- Der Render Graph kann daraus Abhängigkeiten und Barriers ableiten.
- Ressourcenbindungen MÜSSEN mit dem ausführbaren Render-Graph-Plan kompatibel sein.

## 13. Material Engine Integration

Nova Shader Resource Binding MUSS die Nova Material Engine unterstützen.

Insbesondere für:

- Materialfarben und Transparenz.
- Blur-Parameter.
- Refraction- und Reflection-Parameter.
- Backdrop Textures.
- Lighting-Daten.
- Animationsparameter.

Materialparameter SOLLEN über dynamische Bindungen aktualisiert werden.

Änderungen einzelner Materialparameter DÜRFEN nicht grundsätzlich eine vollständige Shader- oder Pipeline-Neukompilierung auslösen.

## 14. Sicherheit und Isolation

Ressourcenbindungen MÜSSEN die NovaOS-Capability-Architektur respektieren.

Dabei gilt:

- Shader erhalten ausschließlich autorisierte Ressourcenhandles.
- Lese- und Schreibrechte MÜSSEN getrennt geprüft werden.
- Prozessübergreifende Ressourcenfreigaben benötigen entsprechende Berechtigungen.
- Ungültige oder widerrufene Handles MÜSSEN zurückgewiesen werden.
- Physische GPU-Adressen DÜRFEN nicht unkontrolliert offengelegt werden.
- Wiederverwendete Bindungsobjekte DÜRFEN keine fremden Ressourcen zugänglich machen.

Die GPU-Treiberarchitektur MUSS die erforderliche Speicherisolation gewährleisten.

## 15. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Ressourcenhandles.
- Inkompatible Binding Layouts.
- Nicht unterstützte Ressourcentypen.
- Ungültige Buffer-Offsets.
- Nicht verfügbare Texture Views.
- Überschrittene Hardwarelimits.
- Fehlende Ressourcenrechte.
- Fehlgeschlagene Backend-Bindungen.
- GPU-Geräteverlust.

Fehlerhafte Binding Sets DÜRFEN nicht als gültig veröffentlicht werden.

Bei einem Fehler MUSS die betroffene GPU-Operation kontrolliert abgebrochen oder über einen geeigneten Fallback behandelt werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Shader Resource Binding Verwaltung bereitstellen.
2. Logische Bindungen und Backend-Implementierungen MÜSSEN getrennt sein.
3. Binding Layouts MÜSSEN unterstützt werden.
4. Binding Sets MÜSSEN unterstützt werden.
5. Ressourcentypen und Zugriffsarten MÜSSEN validiert werden.
6. Buffer-Ranges und Texture-Subresources MÜSSEN berücksichtigt werden.
7. Dynamische Ressourcenbindungen SOLLEN unterstützt werden.
8. Kompatible Bindungskonfigurationen SOLLEN wiederverwendet werden.
9. GPU Resource Management MUSS integriert werden.
10. GPU Resource Hazard Tracking MUSS berücksichtigt werden.
11. Render Graph MUSS Ressourcenbindungen auswerten können.
12. Nova Shader Pipeline MUSS Binding Layouts verwenden können.
13. Nova Material Engine MUSS dynamische Materialparameter binden können.
14. Aktive Ressourcenbindungen DÜRFEN nicht unsicher verändert oder freigegeben werden.
15. Ressourcenrechte MÜSSEN capability-basiert geprüft werden.
16. Hardwarelimits MÜSSEN berücksichtigt werden.
17. Geeignete Fallbacks MÜSSEN verfügbar sein.
18. Im deterministischen Modus MÜSSEN logische Bindungsentscheidungen bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova Shader Resource Binding stellt eine einheitliche und sichere Infrastruktur zur Bereitstellung von GPU-Ressourcen für Shader bereit.

Durch deklarative Binding Layouts, wiederverwendbare Binding Sets, dynamische Parameter und integrierte Synchronisation werden Ressourcenwechsel und Verwaltungsaufwand reduziert.

Damit erhält NovaOS eine effiziente Grundlage für Shader-Ausführung, Render Graph, Compute und die Nova Material Engine mit dynamischen Liquid-Glass-Effekten.