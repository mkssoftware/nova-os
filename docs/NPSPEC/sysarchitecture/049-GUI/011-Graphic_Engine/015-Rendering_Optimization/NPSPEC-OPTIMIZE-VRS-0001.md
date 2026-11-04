# NPSPEC-OPTIMIZE-VRS-0001 – Nova Variable Rate Shading

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Optimization

## 1. Zweck

Nova Variable Rate Shading (VRS) definiert die adaptive Steuerung der Fragment- bzw. Pixel-Shader-Ausführungsrate innerhalb der NovaOS-Grafikarchitektur.

Ziel ist die Reduzierung unnötiger GPU-Shader-Berechnungen, indem visuell weniger empfindliche Bildbereiche mit einer geringeren Shading-Rate verarbeitet werden.

Die Optimierung MUSS die Lesbarkeit, Bedienbarkeit und definierte Darstellungsqualität der Benutzeroberfläche erhalten.

## 2. Architekturprinzipien

- **Region-Based:** Unterschiedliche Bildbereiche können unterschiedliche Shading-Raten verwenden.
- **Content-Aware:** Die Auswahl berücksichtigt den dargestellten Inhalt.
- **Material-Aware:** Materialeigenschaften beeinflussen die zulässige Shading-Rate.
- **Adaptive:** Die Shading-Rate kann an GPU-Auslastung und Qualitätsrichtlinien angepasst werden.
- **Hardware-Aware:** Verfügbare VRS-Funktionen werden berücksichtigt.
- **Quality-Preserving:** Kritische UI-Elemente behalten ihre erforderliche Darstellungsqualität.
- **Incremental:** Unveränderte VRS-Konfigurationen werden wiederverwendet.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Shading-Entscheidungen.

## 3. Architektur

Nova Variable Rate Shading erweitert die bestehende Render- und GPU-Architektur.

| Komponente | Verantwortung |
|---|---|
| VRS Controller | Koordination der Shading-Rate-Auswahl |
| VRS Capability Resolver | Ermittlung unterstützter Hardwarefunktionen |
| VRS Region Analyzer | Bewertung geeigneter Bildregionen |
| VRS Rate Selector | Auswahl der Shading-Rate |
| VRS State Manager | Verwaltung versionierter VRS-Zustände |
| VRS Performance Evaluator | Bewertung der tatsächlichen Einsparungen |

Die physische Umsetzung erfolgt über das jeweilige GPU-Backend.

Eine eigenständige Rendering-Pipeline DARF nicht entstehen.

## 4. Shading Rate Model

NovaOS MUSS Shading-Raten unabhängig von einer bestimmten GPU-API beschreiben können.

Beispielhafte Raten:

| Shading-Rate | Bedeutung |
|---|---|
| 1×1 | Eine Fragment-Shader-Auswertung pro Pixel |
| 1×2 | Eine Auswertung für einen Bereich von zwei Pixeln |
| 2×1 | Eine Auswertung für einen Bereich von zwei Pixeln |
| 2×2 | Eine Auswertung für einen Bereich von vier Pixeln |
| 2×4 | Eine Auswertung für einen Bereich von acht Pixeln |
| 4×2 | Eine Auswertung für einen Bereich von acht Pixeln |
| 4×4 | Eine Auswertung für einen Bereich von sechzehn Pixeln |

Die tatsächlich verfügbaren Raten hängen von der GPU ab.

NovaOS MUSS ausschließlich unterstützte Shading-Raten verwenden.

Die Raten beschreiben die Fragment-Shading-Frequenz und nicht automatisch die Auflösung des Render Targets.

## 5. VRS Capability Detection

NovaOS MUSS verfügbare VRS-Funktionen zur Laufzeit ermitteln können.

Dabei sind insbesondere zu berücksichtigen:

- Unterstützung für Variable Rate Shading.
- Verfügbare Shading-Raten.
- Per-Draw-Shading-Rate.
- Per-Primitive-Shading-Rate.
- Screen-Space-Shading-Rate-Images.
- Unterstützte Render-Target-Formate.
- Einschränkungen bei Multisampling.
- Hardwareabhängige Kombinationsregeln.

Nicht unterstützte Funktionen DÜRFEN nicht vorausgesetzt werden.

## 6. Content-Aware Shading

NovaOS SOLL Shading-Raten anhand des dargestellten Inhalts auswählen.

| Inhalt | Bevorzugte Strategie |
|---|---|
| Text | Volle Shading-Qualität |
| Kleine UI-Symbole | Volle Shading-Qualität |
| Fensterrahmen | Hohe Qualität |
| Interaktive Controls | Hohe Qualität |
| Feine Linien | Volle Shading-Qualität |
| Große homogene Flächen | Reduzierte Shading-Rate möglich |
| Weiche Hintergrundeffekte | Reduzierte Shading-Rate möglich |
| Dekorative Effekte | Adaptive Shading-Rate |
| Unscharfe Hintergrundbereiche | Reduzierte Shading-Rate möglich |

Eine reduzierte Shading-Rate DARF nur verwendet werden, wenn die definierte Darstellungsqualität erhalten bleibt.

## 7. Material-Aware VRS

Nova Variable Rate Shading MUSS mit Nova Material Engine zusammenarbeiten.

Dabei gilt:

- Materialien können bevorzugte Shading-Raten deklarieren.
- Effektstufen können unterschiedliche Qualitätsanforderungen besitzen.
- Blur-Berechnungen können reduzierte Shading-Raten verwenden, sofern das Ergebnis korrekt bleibt.
- Refraction benötigt eine ausreichende Genauigkeit für sichtbare Verzerrungen.
- Reflection kann abhängig von Detailgrad und Materialeigenschaften angepasst werden.
- Feine Highlights und scharfe Materialgrenzen SOLLEN mit hoher Qualität verarbeitet werden.

Die VRS-Auswahl MUSS mit Nova Adaptive Material Quality kompatibel sein.

## 8. Region-Based VRS

NovaOS SOLL unterschiedliche Shading-Raten innerhalb eines Render Targets unterstützen.

Dabei gilt:

- Regionen können unterschiedliche Qualitätsanforderungen besitzen.
- Text- und Detailregionen SOLLEN bevorzugt mit voller Shading-Rate verarbeitet werden.
- Große gleichmäßige Bereiche können eine reduzierte Shading-Rate erhalten.
- Regionengrenzen MÜSSEN korrekt berücksichtigt werden.
- Die tatsächliche VRS-Tile-Größe der Hardware MUSS beachtet werden.

Bei fehlender Unterstützung für regionenbasierte VRS-Images MUSS eine alternative Strategie verwendet werden.

## 9. Adaptive VRS Selection

NovaOS SOLL die Shading-Rate anhand folgender Faktoren anpassen:

- GPU-Auslastung.
- Fragment-Shader-Kosten.
- Frame-Latenz.
- Energieverbrauch.
- Materialkomplexität.
- Sichtbare Detaildichte.
- Benutzerdefinierte Qualitätsrichtlinien.

Die Auswahl MUSS mit Nova Adaptive Rendering Strategy und Nova Adaptive Rendering Quality zusammenarbeiten.

Hysterese SOLL häufige Qualitätswechsel verhindern.

## 10. Incremental VRS Updates

NovaOS MUSS VRS-Konfigurationen inkrementell aktualisieren können.

Dabei gilt:

- Unveränderte VRS-Regionen SOLLEN erhalten bleiben.
- Änderungen an Material oder Geometrie MÜSSEN berücksichtigt werden.
- Änderungen der Qualitätsrichtlinien MÜSSEN relevante VRS-Zustände invalidieren.
- Gültige VRS-Images SOLLEN wiederverwendet werden.
- Änderungen der Shading-Rate MÜSSEN bei der Gültigkeit betroffener Render-Ergebnisse berücksichtigt werden.

Eine VRS-Änderung DARF keine inkonsistenten Cache-Ergebnisse erzeugen.

## 11. Render Graph Integration

Nova Variable Rate Shading MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Render-Passes können VRS-Anforderungen deklarieren.
- VRS-Konfigurationen MÜSSEN vor der GPU-Ausführung feststehen.
- Shading-Rate-Images MÜSSEN als GPU-Ressourcen verwaltet werden.
- Ressourcenabhängigkeiten und Barriers MÜSSEN berücksichtigt werden.
- Nicht unterstützte VRS-Konfigurationen MÜSSEN auf gültige Alternativen abgebildet werden.

VRS DARF keine zusätzlichen Render-Passes erzwingen, deren Kosten den erwarteten Nutzen dauerhaft übersteigen.

## 12. GPU Resource Management

NovaOS MUSS VRS-Ressourcen über Nova GPU Resource Management verwalten.

Dabei gilt:

- VRS-Images MÜSSEN korrekt allokiert werden.
- Ressourcenformate MÜSSEN hardwarekompatibel sein.
- Unveränderte VRS-Images SOLLEN wiederverwendet werden.
- GPU-Synchronisation MUSS eingehalten werden.
- Nicht benötigte Ressourcen DÜRFEN freigegeben werden.

## 13. Performance Evaluation

NovaOS SOLL die tatsächliche Wirksamkeit von VRS erfassen.

Relevante Kennzahlen sind:

- Reduzierte Fragment-Shader-Ausführungen.
- GPU-Ausführungszeit.
- Frame-Latenz.
- Kosten der VRS-Konfiguration.
- Speicherverbrauch der VRS-Ressourcen.
- Auswirkungen auf die Darstellungsqualität.

VRS SOLL deaktiviert werden können, wenn keine ausreichenden Leistungsvorteile entstehen.

## 14. Fallback

NovaOS MUSS ohne Hardware-VRS vollständig funktionsfähig bleiben.

Die Fallback-Reihenfolge lautet:

1. Hardware-VRS mit geeigneter regionaler Steuerung.
2. Vereinfachte hardwareunterstützte VRS-Konfiguration.
3. Normales Rendering mit voller Shading-Rate.

Alternative Optimierungen wie reduzierte Effektauflösung oder inkrementelles Rendering bleiben unabhängig von VRS verfügbar.

Eine Softwareemulation von Hardware-VRS ist nicht erforderlich.

## 15. Sicherheit und Konsistenz

NovaOS MUSS folgende Anforderungen erfüllen:

- VRS-Zustände MÜSSEN versioniert sein.
- Nicht unterstützte Shading-Raten DÜRFEN nicht verwendet werden.
- Ressourcenlebenszeiten MÜSSEN eingehalten werden.
- GPU-Synchronisation MUSS korrekt erfolgen.
- Geschützte Oberflächeninhalte MÜSSEN isoliert bleiben.
- Kritische UI-Elemente MÜSSEN lesbar und bedienbar bleiben.
- Bei fehlerhaften VRS-Konfigurationen MUSS normales Rendering möglich sein.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Variable Rate Shading als optionale GPU-Optimierung unterstützen können.
2. Hardwarefähigkeiten MÜSSEN zur Laufzeit erkannt werden.
3. Nicht unterstützte Shading-Raten DÜRFEN nicht verwendet werden.
4. Per-Draw-VRS SOLL unterstützt werden, sofern verfügbar.
5. Region-Based VRS SOLL unterstützt werden, sofern verfügbar.
6. Content-Aware Shading MUSS möglich sein.
7. Material-Aware Shading MUSS unterstützt werden.
8. Text und kritische UI-Details MÜSSEN ihre erforderliche Darstellungsqualität behalten.
9. Adaptive Shading-Raten SOLLEN unterstützt werden.
10. VRS-Konfigurationen MÜSSEN versioniert werden.
11. Inkrementelle VRS-Aktualisierung SOLL unterstützt werden.
12. Nova Render Graph MUSS integriert werden.
13. Nova GPU Resource Management MUSS berücksichtigt werden.
14. Nova Adaptive Material Quality MUSS integriert werden.
15. VRS-Entscheidungen SOLLEN kostenbasiert erfolgen.
16. GPU-Synchronisation und Ressourcenlebenszeiten MÜSSEN eingehalten werden.
17. VRS MUSS deaktivierbar sein.
18. Ohne Hardware-VRS MUSS normales Rendering möglich bleiben.
19. Die funktionale Rendering-Korrektheit MUSS erhalten bleiben.
20. Im deterministischen Modus MÜSSEN VRS-Entscheidungen bei identischen Eingaben und Systemzuständen reproduzierbar sein.

## 17. Ergebnis

Nova Variable Rate Shading ermöglicht die gezielte Reduzierung von Fragment-Shader-Berechnungen in visuell weniger empfindlichen Bildbereichen.

Durch hardwareabhängige Shading-Raten, inhaltsbasierte Qualitätsentscheidungen und Integration in die bestehende Render-Architektur können GPU-Ressourcen effizienter genutzt werden.

Damit kann NovaOS komplexe grafische Oberflächen und Materialeffekte mit geringerem Rechenaufwand darstellen, während Text, Bedienelemente und wichtige visuelle Details ihre erforderliche Qualität behalten.