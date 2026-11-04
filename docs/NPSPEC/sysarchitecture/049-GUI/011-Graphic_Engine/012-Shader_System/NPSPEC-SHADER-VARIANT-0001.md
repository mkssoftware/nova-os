# NPSPEC-SHADER-VARIANT-0001 – Nova Shader Variant Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Shader System

## 1. Zweck

Nova Shader Variant Management definiert die Erstellung, Auswahl, Verwaltung und Wiederverwendung unterschiedlicher Shader-Varianten innerhalb von NovaOS.

Ziel ist die effiziente Anpassung von Shadern an GPU-Fähigkeiten, Materialeigenschaften und Rendering-Anforderungen, ohne unnötige Kompilierungen oder eine unkontrollierte Anzahl von Varianten zu verursachen.

## 2. Architekturprinzipien

- **Feature-Based:** Varianten werden anhand tatsächlich benötigter Funktionen erzeugt.
- **Capability-Aware:** GPU-Fähigkeiten bestimmen zulässige Varianten.
- **Minimal Variants:** Unnötige Variantenkombinationen werden vermieden.
- **Cache-First:** Vorhandene gültige Varianten werden wiederverwendet.
- **Lazy Compilation:** Varianten werden bei Bedarf erstellt.
- **Precompilation:** Häufig benötigte Varianten können vorab kompiliert werden.
- **Deterministic:** Gleiche Anforderungen erzeugen dieselbe logische Variantenauswahl.
- **Fallback-Safe:** Nicht verfügbare Varianten besitzen definierte Ersatzstrategien.

## 3. Variant Architecture

Nova Shader Variant Management besteht aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Variant Manager | Zentrale Variantenverwaltung |
| Feature Resolver | Ermittlung benötigter Shader-Funktionen |
| Variant Descriptor | Deklarative Variantenbeschreibung |
| Variant Selector | Auswahl kompatibler Varianten |
| Variant Compiler | Anforderung der Shader-Kompilierung |
| Variant Cache | Wiederverwendung vorhandener Varianten |
| Variant Policy | Begrenzung und Priorisierung |

Die eigentliche Kompilierung MUSS über Nova Shader Compilation erfolgen.

## 4. Shader Variant Model

Jede Shader-Variante besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Variant ID | Eindeutige Variantenidentifikation |
| Base Shader ID | Zugehöriger Basisshader |
| Shader Version | Version des Basisshaders |
| Feature Set | Aktivierte Shader-Funktionen |
| Specialization Parameters | Kompilierungsrelevante Parameter |
| Resource Layout | Benötigte Ressourcenbindungen |
| Backend Target | Ziel-GPU und Backend |
| Variant State | Aktueller Zustand |

Die Variantenidentität MUSS aus allen kompilierungsrelevanten Eigenschaften eindeutig ableitbar sein.

## 5. Variant Parameters

NovaOS MUSS zwischen statischen und dynamischen Shader-Parametern unterscheiden.

**Statische Parameter** können eine eigene Shader-Variante erfordern:

- Shader-Funktionen und Algorithmen.
- Hardwareabhängige Implementierungen.
- Ressourcenlayouts.
- Kompilierungszeitkonstanten.
- Unterschiedliche Shader-Stufen.

**Dynamische Parameter** SOLLEN ohne neue Shader-Variante geändert werden:

- Farben.
- Transparenz.
- Blur-Radius, sofern dynamisch unterstützt.
- Materialintensität.
- Transformationswerte.
- Animationsparameter.

Dynamische Parameter DÜRFEN nicht unnötig zur Erzeugung zusätzlicher Varianten führen.

## 6. Variant Selection

Die Variantenauswahl erfolgt anhand folgender Kriterien:

1. Benötigte Shader-Funktionen.
2. Unterstützte GPU-Fähigkeiten.
3. Kompatibilität des Ressourcenlayouts.
4. Rendering- und Materialanforderungen.
5. Verfügbarkeit bereits kompilierter Varianten.
6. Definierte Qualitäts- und Fallback-Regeln.

Die Auswahl MUSS die funktionale Korrektheit gewährleisten.

Eine vorhandene, aber inkompatible Variante DARF nicht verwendet werden.

## 7. Variant Explosion Prevention

NovaOS MUSS eine unkontrollierte Vermehrung von Shader-Varianten verhindern.

Dazu SOLLEN folgende Verfahren verwendet werden:

- Zusammenfassung kompatibler Funktionen.
- Dynamische Parameter statt statischer Varianten.
- Gemeinsame Shader-Bausteine.
- Begrenzung zulässiger Feature-Kombinationen.
- Bedarfsgesteuerte Kompilierung.
- Priorisierte Precompilation.
- Wiederverwendung kompatibler Pipeline-Konfigurationen.

Variantenbudgets MÜSSEN pro Shader oder Shader-Familie definierbar sein.

## 8. Variant Lifecycle

NovaOS definiert folgende Zustände:

`Defined → Requested → Compiling → Ready → Retired → Released`

Zusätzliche Zustände:

- **Cached:** Gültiges Kompilierungsergebnis vorhanden.
- **Failed:** Kompilierung fehlgeschlagen.
- **Unsupported:** GPU unterstützt die erforderlichen Funktionen nicht.
- **Obsolete:** Variante gehört zu einer veralteten Shader-Version.

Aktive Varianten DÜRFEN nicht vor Abschluss ihrer GPU-Verwendung freigegeben werden.

## 9. Compilation und Caching

Nova Shader Variant Management MUSS mit folgenden Komponenten zusammenarbeiten:

- Nova Shader Compilation.
- Nova Shader Caching.
- Nova Shader Precompilation.
- Nova Shader Pipeline.

Dabei gilt:

- Identische Variantenanforderungen SOLLEN zusammengeführt werden.
- Gültige Cache-Einträge MÜSSEN bevorzugt verwendet werden.
- Neue Varianten werden nur bei Bedarf kompiliert.
- Häufig benötigte Varianten können vorab vorbereitet werden.
- Änderungen am Basisshader MÜSSEN betroffene Varianten invalidieren.

## 10. Material Engine Integration

Nova Shader Variant Management MUSS die Nova Material Engine unterstützen.

Insbesondere bei:

- Acrylic.
- Liquid Glass.
- Frosted Glass.
- Crystal.
- Solid.
- Blur.
- Refraction.
- Reflection.
- Lighting.

Materialvarianten SOLLEN gemeinsame Shader-Funktionen wiederverwenden.

Unterschiedliche Materialparameter SOLLEN möglichst über dynamische Ressourcenbindungen verarbeitet werden.

Die Auswahl einer Materialvariante MUSS die tatsächlichen GPU-Fähigkeiten berücksichtigen.

## 11. Adaptive Variant Selection

NovaOS DARF zwischen funktional geeigneten Shader-Varianten adaptiv auswählen.

Berücksichtigt werden können:

- GPU-Leistung.
- Speicherverbrauch.
- Rendering-Kosten.
- Energiebedarf.
- Aktuelle Systemlast.
- Gewähltes Qualitätsprofil.

Adaptive Entscheidungen DÜRFEN keine Sicherheitsanforderungen oder Ressourcenabhängigkeiten verletzen.

Qualitätsreduzierungen MÜSSEN den festgelegten Material- und Rendering-Fallbacks entsprechen.

## 12. Versioning und Hot Replacement

NovaOS MUSS Shader-Varianten versionieren.

Dabei gilt:

- Änderungen am Basisshader erzeugen neue Variantenrevisionen.
- Veröffentlichte Varianten bleiben unveränderlich.
- Neue Varianten können parallel zu bestehenden vorbereitet werden.
- Der Wechsel erfolgt erst nach erfolgreicher Validierung.
- Alte Varianten bleiben bis zum Abschluss aktiver GPU-Zugriffe gültig.

Fehlgeschlagene neue Varianten DÜRFEN bestehende funktionierende Varianten nicht beschädigen.

## 13. Sicherheit und Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Feature-Kombinationen.
- Nicht unterstützte GPU-Funktionen.
- Inkompatible Ressourcenlayouts.
- Fehlgeschlagene Shader-Kompilierung.
- Überschrittene Variantenbudgets.
- Ungültige Cache-Einträge.
- GPU-Geräteverlust.

Shader-Varianten MÜSSEN dieselben Validierungs- und Sicherheitsanforderungen wie ihre Basisshader erfüllen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale Shader-Variantenverwaltung bereitstellen.
2. Varianten MÜSSEN eindeutig identifizierbar sein.
3. Statische und dynamische Shader-Parameter MÜSSEN unterschieden werden.
4. Varianten MÜSSEN anhand tatsächlicher GPU-Fähigkeiten ausgewählt werden.
5. Unnötige Variantenkombinationen MÜSSEN vermieden werden.
6. Variantenbudgets MÜSSEN definierbar sein.
7. Shader Compilation MUSS integriert werden.
8. Shader Caching MUSS unterstützt werden.
9. Shader Precompilation MUSS integrierbar sein.
10. Shader Pipeline MUSS Varianten verwenden können.
11. Die Nova Material Engine MUSS unterstützt werden.
12. Aktive Varianten DÜRFEN nicht vorzeitig freigegeben werden.
13. Shader-Versionierung und kontrolliertes Hot Replacement MÜSSEN unterstützt werden.
14. Nicht unterstützte Varianten MÜSSEN einen definierten Fallback besitzen.
15. Im deterministischen Modus MUSS die Variantenauswahl bei identischen Eingaben und Hardwarefähigkeiten reproduzierbar sein.

## 15. Ergebnis

Nova Shader Variant Management ermöglicht die kontrollierte Verwaltung unterschiedlicher Shader-Ausführungsvarianten.

Durch funktionsbasierte Auswahl, dynamische Parameter, Variantenbegrenzung, Caching und bedarfsgesteuerte Kompilierung werden Speicherverbrauch und Kompilierungsaufwand reduziert.

Damit kann NovaOS Shader effizient an unterschiedliche GPU-Architekturen, Materialeffekte und Leistungsanforderungen anpassen, ohne eine unkontrollierte Variantenvielfalt zu erzeugen.