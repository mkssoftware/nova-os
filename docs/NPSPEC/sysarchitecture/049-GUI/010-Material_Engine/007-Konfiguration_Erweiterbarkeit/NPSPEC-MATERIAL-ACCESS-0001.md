# NPSPEC-MATERIAL-ACCESS-0001 – Nova Material Accessibility

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Accessibility definiert die Anforderungen an barrierefreie Materialdarstellungen innerhalb von NovaOS.

Ziel ist eine dauerhaft lesbare, verständliche und bedienbare Benutzeroberfläche unabhängig von Materialtyp, Transparenz, Beleuchtung oder Animation.

Visuelle Materialeffekte dürfen die Zugänglichkeit und Funktionalität der Benutzeroberfläche nicht beeinträchtigen.

## 2. Architektur

Material Accessibility integriert die Nova Material Engine in das bestehende Nova UI Accessibility System.

Die Zuständigkeiten sind:

- **Nova UI Accessibility:** Verwaltung der Barrierefreiheitseinstellungen.
- **Material Accessibility:** Umsetzung materialbezogener Anpassungen.
- **Material System:** Bereitstellung der Materialeigenschaften.
- **Nova Theme System:** Bereitstellung barrierefreier Darstellungsvorgaben.
- **Material Renderer:** Verarbeitung der angepassten Materialparameter.
- **Compositor:** Abschließende Komposition der Materialoberflächen.

Es wird kein eigenständiges Accessibility-System implementiert.

## 3. Grundprinzipien

Die Materialdarstellung folgt diesen Grundsätzen:

- Lesbarkeit besitzt Vorrang vor dekorativen Effekten.
- Funktionale Informationen müssen unabhängig vom Material erkennbar bleiben.
- Transparenz und Animationen müssen reduzierbar sein.
- Benutzerdefinierte Einstellungen müssen berücksichtigt werden.
- Barrierefreiheit darf keine bestimmte GPU-Funktion voraussetzen.
- Anpassungen müssen zur Laufzeit möglich sein.

## 4. Kontrast

Materialien müssen ausreichende Kontraste zwischen Vordergrund- und Hintergrundinhalten ermöglichen.

Verbindliche Anforderungen:

- Materialfarben müssen mit den Kontrastvorgaben des Nova UI-Systems kompatibel sein.
- Hintergrundabhängige Materialien müssen wechselnde Hintergrundhelligkeiten berücksichtigen.
- Bei unzureichendem Kontrast müssen geeignete Materialanpassungen möglich sein.
- Dekorative Reflexionen und Glanzlichter dürfen die Lesbarkeit nicht beeinträchtigen.
- Text- und Symbolkontraste müssen anhand des tatsächlich komponierten Erscheinungsbildes bewertet werden.
- Die geltenden Kontrastanforderungen des Nova UI Accessibility Systems müssen eingehalten werden.

## 5. Transparenz

NovaOS muss eine systemweite Reduzierung transparenter Materialeffekte unterstützen.

Anforderungen:

- Transparenzintensität muss begrenzbar sein.
- Hintergrundabhängige Materialien müssen durch deckendere Varianten ersetzt werden können.
- Bei deaktivierter Transparenz dürfen keine unnötigen Hintergrundabtastungen erfolgen.
- Material Styles und Themes müssen die Einstellung berücksichtigen.
- Die funktionale Darstellung muss erhalten bleiben.

## 6. Unschärfe und Hintergrundeffekte

Hintergrundunschärfe und ähnliche Effekte müssen anpassbar sein.

Verbindliche Anforderungen:

- Unschärfeeffekte müssen reduzierbar oder deaktivierbar sein.
- Hintergrundeffekte dürfen keine wichtigen Informationen verdecken.
- Starke Hintergrundkontraste müssen durch geeignete Materialeigenschaften ausgeglichen werden können.
- Bei deaktivierten Hintergrundeffekten muss eine definierte Ersatzdarstellung verwendet werden.
- Nicht benötigtes Backdrop Sampling muss entfallen.

## 7. Animation und Bewegung

Materialanimationen müssen die systemweiten Bewegungseinstellungen berücksichtigen.

Anforderungen:

- Dekorative Animationen müssen reduzierbar oder deaktivierbar sein.
- Flüssige Verformungen und bewegte Reflexionen müssen abgeschaltet werden können.
- Notwendige Zustandsänderungen müssen weiterhin erkennbar bleiben.
- Bei reduzierter Bewegung sollen unmittelbare Zustandswechsel oder dezente Übergänge verwendet werden.
- Animationen dürfen keine dauerhaft unnötigen Rendering-Aktualisierungen verursachen.

Die Animationssteuerung erfolgt über das bestehende NovaOS Animation System.

## 8. Beleuchtung und Reflexionen

Dynamische Beleuchtung und Reflexionen dürfen die Wahrnehmbarkeit von UI-Inhalten nicht beeinträchtigen.

Verbindliche Anforderungen:

- Reflexionsintensität muss begrenzbar sein.
- Glanzlichter dürfen wichtige Bedienelemente nicht überstrahlen.
- Dynamische Beleuchtung muss deaktivierbar sein.
- Materialeffekte dürfen keine unkontrollierten starken Helligkeitswechsel erzeugen.
- Bei reduzierter visueller Komplexität müssen vereinfachte Beleuchtungsmodelle verfügbar sein.

## 9. Farben und Farbwahrnehmung

Materialien müssen unterschiedliche Anforderungen an die Farbwahrnehmung unterstützen.

Anforderungen:

- Funktionale Zustände dürfen nicht ausschließlich durch Farbe vermittelt werden.
- Materialfarben müssen mit systemweiten Farbanpassungen kompatibel sein.
- Theme- und Akzentfarben müssen barrierefrei angepasst werden können.
- Materialeffekte dürfen notwendige Statusinformationen nicht verfälschen.
- Farbtransformationen müssen das NovaOS Color Management berücksichtigen.

## 10. Interaktionszustände

Materialien müssen funktionale UI-Zustände eindeutig darstellen.

Mindestens berücksichtigt werden:

- Normal
- Hovered
- Pressed
- Focused
- Selected
- Disabled
- Active
- Inactive

Verbindliche Anforderungen:

- Fokuszustände müssen eindeutig erkennbar bleiben.
- Deaktivierte Elemente müssen unterscheidbar sein.
- Ausgewählte Elemente müssen unabhängig von dekorativen Effekten erkennbar bleiben.
- Materialtransparenz darf Interaktionszustände nicht unkenntlich machen.
- Materialeffekte dürfen keine Änderungen an der tatsächlichen Bedienlogik verursachen.

## 11. Systemweite Einstellungen

Material Accessibility verwendet die Einstellungen des Nova UI Accessibility Systems.

Mindestens unterstützt werden:

- Hoher Kontrast
- Reduzierte Transparenz
- Reduzierte Bewegung
- Reduzierte visuelle Effekte
- Benutzerdefinierte Farbanpassungen

Verbindliche Anforderungen:

- Änderungen müssen zur Laufzeit wirksam werden.
- Bestehende Materialinstanzen müssen automatisch aktualisiert werden.
- Benutzerdefinierte Einstellungen besitzen Vorrang vor dekorativen Theme-Vorgaben.
- Anwendungen dürfen verpflichtende Barrierefreiheitseinstellungen nicht umgehen.
- Die ursprünglichen Materialdefinitionen bleiben unverändert.

## 12. Styles, Templates und Themes

Material Accessibility muss mit Nova Material Styles, Templates und Themes kompatibel sein.

Anforderungen:

- Styles können barrierefreie Materialvarianten bereitstellen.
- Templates müssen alternative Materialdarstellungen unterstützen.
- Themes können eigene Accessibility-Ressourcen definieren.
- Systemweite Barrierefreiheitsvorgaben besitzen Vorrang vor dekorativen Einstellungen.
- Fehlende Accessibility-Varianten müssen durch definierte Ersatzmaterialien ausgeglichen werden.
- Individuelle Materialeigenschaften dürfen verbindliche Barrierefreiheitsvorgaben nicht überschreiben.

## 13. Adaptive Qualität und Fallback

Barrierefreiheit muss unabhängig von der verfügbaren Rendering-Qualität gewährleistet bleiben.

Verbindliche Anforderungen:

- Sämtliche Qualitätsstufen müssen funktionale UI-Zustände erhalten.
- Qualitätsreduzierungen dürfen notwendige Kontraste nicht verschlechtern.
- Nicht verfügbare Materialeffekte müssen kontrolliert ersetzt werden.
- Eine barrierefreie Solid-Darstellung muss verfügbar sein.
- GPU- und CPU-Rendering müssen dieselben grundlegenden Accessibility-Vorgaben berücksichtigen.
- Sicherheits- und Barrierefreiheitsanforderungen besitzen Vorrang vor der bevorzugten Materialqualität.

## 14. Damage Tracking und Performance

Accessibility-Anpassungen müssen ressourcenschonend verarbeitet werden.

Anforderungen:

- Änderungen invalidieren ausschließlich betroffene Materialbereiche.
- Unveränderte Materialergebnisse dürfen wiederverwendet werden.
- Deaktivierte Effekte dürfen keine unnötigen Rendering-Operationen auslösen.
- Änderungen von Accessibility-Einstellungen müssen abhängige Cache-Einträge gezielt invalidieren.
- Eine vollständige Bildschirmaktualisierung darf nur erfolgen, wenn sie tatsächlich erforderlich ist.
- Accessibility-Prüfungen dürfen die funktionale Eingabeverarbeitung nicht unnötig blockieren.

## 15. Designer-Integration

Der Nova UI Designer muss die Überprüfung barrierefreier Materialdarstellungen unterstützen.

Mindestens vorgesehen sind:

- Vorschau mit hohem Kontrast
- Vorschau mit reduzierter Transparenz
- Vorschau mit reduzierter Bewegung
- Prüfung von Vordergrund- und Hintergrundkontrasten
- Anzeige problematischer Materialeigenschaften
- Vorschau der Material-Fallbacks

Die Prüfungen müssen das tatsächlich resultierende Materialverhalten berücksichtigen.

## 16. Determinismus und Sicherheit

Verbindliche Anforderungen:

- Accessibility-Regeln müssen eindeutig und deterministisch aufgelöst werden.
- Systemweite Einschränkungen dürfen durch benutzerdefinierte Materialien nicht umgangen werden.
- Ungültige Accessibility-Konfigurationen müssen kontrolliert behandelt werden.
- Materialanpassungen dürfen keine unberechtigten Hintergrundzugriffe ermöglichen.
- Fehlerhafte Materialeffekte dürfen die barrierefreie Ersatzdarstellung nicht verhindern.
- Bei nicht erfüllbaren Accessibility-Anforderungen muss ein definiertes sicheres Ersatzmaterial verwendet werden.

## 17. Ergebnis

Nova Material Accessibility gewährleistet, dass sämtliche Materialien der Nova Material Engine mit den Barrierefreiheitsanforderungen von NovaOS kompatibel bleiben.

Durch anpassbare Transparenz, Kontrast, Bewegung, Beleuchtung und Materialeffekte können komplexe Oberflächen individuell vereinfacht werden.

Die Integration in das bestehende Nova UI Accessibility System stellt sicher, dass visuelle Innovationen die Lesbarkeit, Bedienbarkeit und Kontrolle des Benutzers nicht einschränken.