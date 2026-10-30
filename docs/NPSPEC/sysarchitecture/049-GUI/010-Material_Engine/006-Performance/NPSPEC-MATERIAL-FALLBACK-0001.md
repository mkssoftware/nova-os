# NPSPEC-MATERIAL-FALLBACK-0001 – Nova Material Rendering Fallback

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Rendering Fallback definiert die kontrollierte Ersatzdarstellung von Materialien, wenn bestimmte Rendering-Funktionen nicht verfügbar sind oder nicht zuverlässig ausgeführt werden können.

Ziel ist die Aufrechterhaltung einer stabilen, lesbaren und vollständig bedienbaren NovaOS-Oberfläche unabhängig von GPU-Fähigkeiten, Ressourcenverfügbarkeit oder Materialkomplexität.

## 2. Architektur

Material Rendering Fallback verwendet die bestehende Nova Material Rendering Pipeline und Nova Graphics Engine.

Die Zuständigkeiten sind:

- **Material System:** Bereitstellung von Materialdefinitionen und Ersatzmaterialien.
- **Material Renderer:** Auswahl und Verarbeitung der verfügbaren Materialimplementierung.
- **Material Fallback:** Festlegung der Ersatzstrategien.
- **Adaptive Material Quality:** Qualitätsreduzierung bei Ressourcenknappheit.
- **Graphics Engine:** Bereitstellung verfügbarer GPU- und CPU-Rendering-Funktionen.
- **Compositor:** Komposition der resultierenden Materialoberflächen.

Es wird keine eigenständige Fallback-Rendering-Pipeline implementiert.

## 3. Fallback-Grundprinzip

Jedes Material muss eine definierte Ersatzdarstellung besitzen.

Verbindliche Anforderungen:

- Fehlende Rendering-Funktionen dürfen die grundlegende Materialdarstellung nicht verhindern.
- Fallbacks müssen automatisch aktiviert werden können.
- Die funktionale Bedeutung von UI-Elementen muss erhalten bleiben.
- Materialdefinitionen dürfen durch einen Fallback nicht verändert werden.
- Die Auswahl muss deterministisch und nachvollziehbar erfolgen.
- Ein Fallback darf keine zusätzlichen Sicherheitsberechtigungen benötigen.

## 4. Fallback-Stufen

Die Material Engine unterstützt folgende Ersatzstufen:

1. **Native:** Vollständige Materialimplementierung.
2. **Simplified:** Vereinfachte Implementierung desselben Materials.
3. **Alternative:** Kompatibler Ersatzmaterialtyp.
4. **Solid:** Einfache deckende Materialdarstellung.

Die Auswahl erfolgt anhand der verfügbaren Rendering-Funktionen und der Materialanforderungen.

Eine nicht verfügbare Stufe wird übersprungen.

## 5. Material-Fallbacks

Für die Standardmaterialien gelten folgende bevorzugte Ersatzstrategien:

| Material | Vereinfachung | Ersatzmaterial |
|---|---|---|
| Acrylic | Reduzierte Hintergrundunschärfe | Solid |
| Liquid Glass | Ohne komplexe Lichtbrechung und Verformung | Acrylic oder Solid |
| Frosted Glass | Vereinfachte Unschärfe | Solid |
| Crystal | Reduzierte Reflexionen und Beleuchtung | Acrylic oder Solid |
| Solid | Vereinfachte Farb- und Alpha-Verarbeitung | Deckende Systemfarbe |

Die konkrete Ersatzdarstellung muss ausreichend Kontrast und Lesbarkeit gewährleisten.

## 6. Effekt-Fallbacks

Einzelne Materialeffekte müssen unabhängig voneinander ersetzt oder deaktiviert werden können.

Anforderungen:

- Nicht unterstützte Effekte dürfen übersprungen werden.
- Vereinfachte Effektimplementierungen werden bevorzugt.
- Der Ausfall eines dekorativen Effekts darf nicht automatisch das gesamte Material deaktivieren.
- Effektabhängigkeiten müssen bei der Ersatzwahl berücksichtigt werden.
- Ungültige Effektkombinationen müssen kontrolliert aufgelöst werden.
- Die grundlegende Materialdarstellung muss erhalten bleiben.

## 7. GPU- und CPU-Fallback

Material Rendering Fallback muss den Wechsel zwischen GPU- und CPU-Verarbeitung unterstützen.

Verbindliche Anforderungen:

- Die GPU-Verarbeitung wird bevorzugt, sofern verfügbar.
- Grundlegende Materialien müssen CPU-basiert darstellbar sein.
- Nicht unterstützte GPU-Funktionen müssen durch geeignete Alternativen ersetzt werden.
- Bei GPU-Ausfall müssen betroffene Ressourcen kontrolliert invalidiert werden.
- Der Wechsel darf keine ungültigen Rendering-Ressourcen weiterverwenden.
- Die Wiederherstellung der GPU-Verarbeitung muss ohne Neuerstellung der UI-Elemente möglich sein.

## 8. Ressourcenabhängiger Fallback

Ein Fallback kann bei Ressourcenknappheit aktiviert werden.

Berücksichtigt werden:

- GPU-Speicher
- Arbeitsspeicher
- Rendering-Latenz
- Verfügbare Hardwarefunktionen
- Energiezustand
- Systemweite Qualitätsgrenzen

Verbindliche Anforderungen:

- Ressourcenbudgets müssen eingehalten werden.
- Kritische Ressourcenknappheit muss eine unmittelbare Vereinfachung ermöglichen.
- Unnötige Wechsel zwischen Fallback-Stufen sind zu vermeiden.
- Die Entscheidung muss mit Nova Adaptive Material Quality abgestimmt werden.

## 9. Backdrop-Fallback

Hintergrundabhängige Materialien müssen ohne Backdrop Sampling darstellbar bleiben.

Anforderungen:

- Fehlende Hintergrunddaten dürfen keine Rendering-Fehler verursachen.
- Geschützte Hintergrundinhalte dürfen nicht durch Ersatzverfahren offengelegt werden.
- Bei nicht verfügbarer Hintergrundabtastung werden definierte Ersatzfarben oder deckende Materialien verwendet.
- Hintergrundabhängige Effekte dürfen kontrolliert deaktiviert werden.
- Ein Backdrop-Fallback darf keine permanenten Neuberechnungen auslösen.

## 10. Styles und Themes

Fallbacks müssen mit Nova Material Styles, Templates und Themes kompatibel sein.

Verbindliche Anforderungen:

- Materialreferenzen bleiben während eines Fallbacks gültig.
- Theme-Farben und semantische Materialressourcen müssen berücksichtigt werden.
- Individuelle Materialeigenschaften bleiben erhalten, soweit sie vom Ersatzmaterial unterstützt werden.
- Nicht unterstützte Eigenschaften werden kontrolliert ignoriert.
- Theme-Wechsel müssen auch im Fallback-Modus funktionieren.
- Nach Wiederherstellung der ursprünglichen Implementierung müssen die gültigen Materialeigenschaften erneut angewendet werden.

## 11. Damage Tracking und Caching

Material Rendering Fallback muss mit Nova Material Damage Tracking und Nova Material Caching zusammenarbeiten.

Anforderungen:

- Fallback-Wechsel invalidieren ausschließlich betroffene Materialbereiche.
- Inkompatible Cache-Einträge müssen verworfen werden.
- Weiterhin gültige Ressourcen dürfen wiederverwendet werden.
- Unveränderte Materialbereiche dürfen nicht unnötig neu berechnet werden.
- Ein Fallback darf keine dauerhafte vollständige Bildschirmaktualisierung auslösen.

## 12. Wiederherstellung

Die Material Engine muss die Rückkehr zu einer höherwertigen Implementierung unterstützen.

Verbindliche Anforderungen:

- Die Verfügbarkeit benötigter Rendering-Funktionen muss erneut geprüft werden können.
- Eine Wiederherstellung darf erst bei ausreichenden Ressourcen erfolgen.
- Häufige Wechsel müssen durch geeignete Stabilisierung verhindert werden.
- Materialzustände und UI-Bindungen müssen erhalten bleiben.
- Fehlgeschlagene Wiederherstellungen dürfen die funktionierende Ersatzdarstellung nicht beeinträchtigen.
- Automatische Wiederherstellung muss systemweite Benutzer- und Qualitätsvorgaben respektieren.

## 13. Barrierefreiheit

Barrierefreiheit besitzt Vorrang vor dekorativen Materialeffekten.

Anforderungen:

- Fallback-Materialien müssen ausreichende Lesbarkeit gewährleisten.
- Kontrastvorgaben müssen berücksichtigt werden.
- Fokus- und Interaktionszustände müssen sichtbar bleiben.
- Reduzierte Transparenz und Bewegung müssen unterstützt werden.
- Eine vollständig deckende Darstellung muss verfügbar sein.
- Die Bedienbarkeit darf durch einen Fallback nicht eingeschränkt werden.

## 14. Diagnose und Fehlerbehandlung

Fallback-Ereignisse müssen über die bestehenden NovaOS-Diagnosesysteme nachvollziehbar sein.

Mindestens erfassbar sind:

- Betroffener Materialtyp
- Ursache des Fallbacks
- Gewählte Ersatzimplementierung
- Zeitpunkt beziehungsweise Ereignisfolge des Wechsels
- Wiederherstellungsstatus

Wiederkehrende Fehler sollen zusammengefasst werden.

Benutzerbenachrichtigungen erfolgen nur, wenn eine relevante Einschränkung oder erforderliche Benutzerentscheidung vorliegt.

## 15. Determinismus und Sicherheit

Verbindliche Anforderungen:

- Fallback-Regeln müssen eindeutig definiert sein.
- Ersatzimplementierungen müssen validiert werden.
- Zyklische Fallback-Abhängigkeiten sind unzulässig.
- Fallbacks dürfen keine Sicherheitsgrenzen der Graphics Engine oder des Compositors umgehen.
- Fehlerhafte Materialimplementierungen müssen isoliert behandelt werden.
- Ein deterministischer Betriebsmodus muss reproduzierbare Fallback-Entscheidungen ermöglichen.
- Ist keine reguläre Materialimplementierung verfügbar, muss eine sichere deckende Systemdarstellung verwendet werden.

## 16. Ergebnis

Nova Material Rendering Fallback gewährleistet eine zuverlässige Materialdarstellung bei fehlenden Hardwarefunktionen, Ressourcenknappheit oder Rendering-Fehlern.

Durch abgestufte Ersatzimplementierungen, CPU-Unterstützung und kontrollierte Wiederherstellung bleibt die NovaOS-Oberfläche funktionsfähig und bedienbar.

Die Material Engine kann damit komplexe visuelle Effekte anbieten, ohne die grundlegende Systemdarstellung von deren Verfügbarkeit abhängig zu machen.