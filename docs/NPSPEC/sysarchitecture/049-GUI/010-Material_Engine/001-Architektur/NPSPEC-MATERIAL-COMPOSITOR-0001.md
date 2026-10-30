# NPSPEC-MATERIAL-COMPOSITOR-0001 – Nova Material Compositor Integration

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Die Nova Material Compositor Integration definiert die Zusammenarbeit zwischen der Nova Material Engine und dem Nova Compositor.

Sie ermöglicht die korrekte Darstellung transparenter, lichtbrechender und hintergrundabhängiger Materialien innerhalb der bestehenden Compositor-Pipeline.

Ziel ist eine effiziente Materialdarstellung unter Berücksichtigung von Ebenenreihenfolge, Damage Tracking und Ressourcenverbrauch.

## 2. Architektur

Die Material Engine ist über eine definierte Schnittstelle mit dem Nova Compositor verbunden.

Die Zuständigkeiten sind klar getrennt:

- **Material Engine:** Definiert Materialien, Eigenschaften und benötigte Effekte.
- **Material Renderer:** Berechnet die visuellen Materialeffekte.
- **Compositor:** Verwaltet Oberflächen, Ebenenreihenfolge, Hintergrunddaten und die endgültige Bildkomposition.
- **Graphics Engine:** Stellt die erforderlichen Rendering-Funktionen bereit.

Die Material Engine darf die Ebenenverwaltung des Compositors nicht umgehen.

## 3. Materialoberflächen

Materialien werden auf Oberflächen oder definierte Teilbereiche angewendet.

Verbindliche Anforderungen:

- Eine Oberfläche kann mehrere Materialbereiche besitzen.
- Materialbereiche unterstützen Clipping und Maskierung.
- Materialien müssen mit transparenten und undurchsichtigen Oberflächen kompatibel sein.
- Materialeffekte dürfen ausschließlich innerhalb ihrer definierten Ausgabebereiche sichtbar werden.
- Materialänderungen dürfen keine Neuerstellung der zugehörigen Oberfläche erfordern.

Materialbereiche werden innerhalb der bestehenden Surface-Struktur des Compositors verwaltet.

## 4. Hintergrundzugriff

Hintergrundabhängige Materialien benötigen Zugriff auf Bildinhalte hinter ihrer Oberfläche.

Anforderungen:

- Der Compositor stellt die benötigten Hintergrunddaten bereit.
- Die Hintergrundabtastung berücksichtigt die korrekte Ebenenreihenfolge.
- Es dürfen ausschließlich bereits komponierte, darunterliegende Inhalte verwendet werden.
- Selbstreferenzierende und zyklische Hintergrundzugriffe sind ausgeschlossen.
- Der Hintergrundzugriff erfolgt über kontrollierte Compositor-Schnittstellen.
- Geschützte Oberflächen und Inhalte müssen bestehende Sicherheitsrichtlinien einhalten.

Die Material Engine erhält keinen uneingeschränkten Zugriff auf fremde Surface-Buffer.

## 5. Ebenenkomposition

Materialien werden entsprechend der bestehenden Z-Order verarbeitet.

Dabei gelten folgende Anforderungen:

- Undurchsichtige Materialien können ohne Hintergrundabtastung dargestellt werden.
- Transparente Materialien berücksichtigen darunterliegende Ebenen.
- Lichtbrechende Materialien verwenden den Hintergrundzustand vor ihrer eigenen Komposition.
- Überlagernde Materialien müssen korrekt miteinander kombiniert werden.
- Änderungen der Ebenenreihenfolge müssen betroffene Materialbereiche invalidieren.

Die Material Engine darf keine eigenständige globale Z-Order verwalten.

## 6. Damage Tracking

Die Materialintegration muss das bestehende Damage Tracking des Nova Compositors verwenden.

Verbindliche Anforderungen:

- Änderungen an Materialeigenschaften erzeugen gezielte Damage-Regionen.
- Hintergrundänderungen invalidieren davon abhängige Materialbereiche.
- Effektbedingte Abtastbereiche werden bei der Damage-Berechnung berücksichtigt.
- Änderungen verdeckter Bereiche dürfen keine unnötigen Rendering-Operationen auslösen.
- Unveränderte Materialbereiche dürfen zwischengespeicherte Ergebnisse wiederverwenden.
- Die Invalidierung muss auch bei verschachtelten transparenten Materialien korrekt erfolgen.

Eine vollständige Neuberechnung des Bildschirms erfolgt nur, wenn dies technisch erforderlich oder effizienter ist.

## 7. Rendering und Synchronisation

Die Materialverarbeitung wird in den bestehenden Compositor-Rendering-Ablauf integriert.

Anforderungen:

- Materialeffekte werden vor der endgültigen Komposition ihres Ausgabebereichs berechnet.
- Hintergrunddaten müssen vor ihrer Verwendung vollständig verfügbar sein.
- GPU-Operationen müssen korrekt synchronisiert werden.
- Unnötige Kopien zwischen Surface-Buffern sind zu vermeiden.
- Materialergebnisse dürfen keine unvollständigen Frames erzeugen.
- Die Materialverarbeitung muss mit der bestehenden Frame-Synchronisation kompatibel sein.

Die Integration darf keine zusätzliche permanente Rendering-Schleife erzeugen.

## 8. Ressourcenverwaltung

Der Compositor und die Material Engine verwenden die vorhandene Grafikressourcenverwaltung.

Anforderungen:

- Temporäre Rendering-Buffer werden nach Möglichkeit wiederverwendet.
- Gemeinsame Hintergrunddaten dürfen von mehreren Materialeffekten genutzt werden.
- Nicht mehr benötigte Materialressourcen werden freigegeben.
- Ressourcen müssen bei Änderungen der Bildschirmauflösung oder des Rendering-Backends aktualisiert werden können.
- GPU-Speicherverbrauch muss begrenzbar sein.

Die Material Engine darf keine unkontrollierten Ressourcenallokationen auslösen.

## 9. Performance und Fallback

Die Integration muss auch auf leistungsschwächerer Hardware funktionsfähig bleiben.

Verbindliche Anforderungen:

- GPU-beschleunigte Materialeffekte werden bevorzugt.
- Die Effektqualität kann dynamisch reduziert werden.
- Nicht unterstützte Effekte werden durch definierte Alternativen ersetzt.
- Bei fehlender Hintergrundabtastung wird ein geeignetes Fallback-Material verwendet.
- Unsichtbare Materialbereiche werden nicht gerendert.
- Materialeffekte dürfen die grundlegende Bedienbarkeit des Desktops nicht beeinträchtigen.

Die Auswahl eines Fallbacks erfolgt über die Material Engine und wird vom Compositor berücksichtigt.

## 10. Sicherheit und Isolation

Materialeffekte dürfen die bestehenden Sicherheitsgrenzen des Compositors nicht verletzen.

Anforderungen:

- Anwendungen erhalten keinen direkten Zugriff auf fremde Surface-Buffer.
- Hintergrundabtastungen erfolgen ausschließlich über autorisierte Compositor-Operationen.
- Geschützte Inhalte dürfen nicht durch Materialeffekte offengelegt werden.
- Materialparameter und Effektbereiche müssen validiert werden.
- Fehlerhafte Materialoperationen dürfen den Compositor nicht destabilisieren.

## 11. Ergebnis

Die Nova Material Compositor Integration verbindet die Nova Material Engine mit der bestehenden Oberflächen- und Ebenenverwaltung von NovaOS.

Sie ermöglicht Acrylic, Liquid Glass, Frosted Glass, Crystal und weitere Materialien mit korrekter Hintergrundverarbeitung, Lichtbrechung und Transparenz.

Durch die Wiederverwendung vorhandener Compositor-Funktionen, Damage Tracking und GPU-Ressourcen bleibt die Materialdarstellung effizient, sicher und vollständig in die NovaOS-Grafikarchitektur integriert.