
# NPSPEC-STUDIO-GRAPH-PROPERTIES-0001 – NovaLang Studio Graph Properties

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Eigenschaften

## Zweck

Definiert die Anzeige und Bearbeitung von Eigenschaften ausgewählter Logic-Graph-Elemente.

Ziel ist eine zentrale, übersichtliche und typsichere Konfiguration von Knoten, Ports, Verbindungen und Graphen ohne unnötige Dialogwechsel.

## Architektur

Das Properties-System besteht aus:

- **Property Inspector:** Darstellung der Eigenschaften.
- **Selection Bridge:** Synchronisation mit der Canvas-Auswahl.
- **Property Schema Provider:** Bereitstellung typisierter Eigenschaftsdefinitionen.
- **Property Editor Factory:** Auswahl geeigneter Eingabeelemente.
- **Property Validator:** Prüfung eingegebener Werte.
- **Change Controller:** Kontrollierte Übernahme von Änderungen.
- **Undo Integration:** Rücknahme von Eigenschaftsänderungen.

Der Property Inspector verwendet die bestehenden Graph- und NovaLang-Typdefinitionen.

## Unterstützte Eigenschaften

Bearbeitbar sind:

- **Graph:** Name, Beschreibung, Version und Konfiguration.
- **Node:** Name, Parameter und Ausführungsoptionen.
- **Port:** Typisierte Standardwerte und zulässige Einstellungen.
- **Connection:** Verbindungseigenschaften und Routingoptionen.
- **Subgraph:** Parameter und Schnittstellen.
- **Capability Node:** Deklarierte Capability-Konfiguration.
- **Custom Script Node:** Script-Referenz und Schnittstellenparameter.

Nicht veränderbare Eigenschaften werden ausschließlich lesend dargestellt.

## Bedienkonzept

Der Inspector erscheint als integrierter Seitenbereich des Graph Editors.

Unterstützt werden:

- Automatische Aktualisierung bei Auswahlwechsel
- Gruppierung nach Eigenschaftskategorien
- Direkte Inline-Bearbeitung
- Suche und Filter
- Ein- und Ausklappen von Bereichen
- Mehrfachauswahl mit gemeinsamen Eigenschaften
- Anzeige von Standardwerten
- Zurücksetzen auf zulässige Standardwerte

Häufig verwendete Eigenschaften müssen ohne zusätzliche Dialoge erreichbar sein.

## Typisierte Eingabeelemente

Der Property Editor verwendet passende Steuerelemente:

- Textfelder für Zeichenketten
- Zahlenfelder für numerische Werte
- Schalter für Boolean-Werte
- Auswahllisten für Enumerationen
- Typauswahl für NovaLang-Datentypen
- Ressourcen- und Referenzauswahl
- Strukturierte Editoren für komplexe Werte

Eingaben müssen anhand des deklarierten Eigenschaftstyps geprüft werden.

## Änderungsverarbeitung

Eigenschaftsänderungen werden zunächst validiert und anschließend atomar in das Graphmodell übernommen.

Änderungen an Ports, Typen oder Schnittstellen lösen die Prüfung betroffener Verbindungen aus.

Ungültige Werte dürfen nicht als gültige Konfiguration übernommen werden.

## Capability-Integration

Capability-Eigenschaften werden anhand der registrierten Capability-Verträge angezeigt.

Der Inspector darf Capability-Anforderungen konfigurieren, jedoch keine Berechtigungen selbst erteilen.

Sicherheitsrelevante Änderungen müssen durch die zentrale Solution- und Berechtigungsprüfung erfasst werden.

## Speicherung

Persistente Eigenschaften werden über die Graph-Serialisierung gespeichert.

Reine Darstellungseinstellungen bleiben von der Ausführungssemantik getrennt.

Geänderte Eigenschaften müssen mit Graph-Versionierung und Undo/Redo kompatibel sein.

## Normative Anforderungen

1. Der Graph Editor MUSS einen integrierten Property Inspector bereitstellen.
2. Eigenschaften MÜSSEN abhängig von der aktuellen Auswahl angezeigt werden.
3. Eigenschaftstypen MÜSSEN aus den registrierten Verträgen abgeleitet werden.
4. Bearbeitbare und schreibgeschützte Eigenschaften MÜSSEN unterscheidbar sein.
5. Eingaben MÜSSEN vor ihrer Übernahme validiert werden.
6. Änderungen MÜSSEN atomar in das Graphmodell übernommen werden.
7. Schnittstellenänderungen MÜSSEN eine erneute Graphvalidierung auslösen.
8. Undo und Redo MÜSSEN unterstützt werden.
9. Mehrfachauswahl SOLL gemeinsame Eigenschaften bearbeiten können.
10. Capability-Konfiguration DARF keine Berechtigungen automatisch erteilen.
11. Editor-Eigenschaften DÜRFEN die Ausführungssemantik nicht unbeabsichtigt verändern.
12. Alle grundlegenden Inspector-Funktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält einen zentralen, typsicheren und unmittelbar bedienbaren Property Inspector zur effizienten Konfiguration sämtlicher Logic-Graph-Elemente.
