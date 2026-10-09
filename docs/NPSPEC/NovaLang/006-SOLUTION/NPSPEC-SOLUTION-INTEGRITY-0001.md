
# NPSPEC-SOLUTION-INTEGRITY-0001 – NovaOS Solution Integrity

## Status

Angenommen

## Kategorie

NovaOS / Solution / Integrität und Sicherheit

## Zweck

Definiert die Integritätsprüfung von NovaOS-Solutions und ihrer Bestandteile.

Ziel ist, unautorisierte Änderungen zuverlässig zu erkennen und sicherzustellen, dass Berechtigungen ausschließlich für eine verifizierte Solution gelten.

## Architektur

Das Integrity-System besteht aus:

- **Integrity Manager:** Koordination der Integritätsprüfungen.
- **Manifest Validator:** Prüfung der `solution.xml`.
- **Content Verifier:** Kryptografische Prüfung der Solution-Dateien.
- **Signature Verifier:** Überprüfung digitaler Signaturen.
- **Identity Binder:** Verknüpfung mit der Solution-Identität.
- **Change Detector:** Erkennung sicherheitsrelevanter Änderungen.

## Integritätsmodell

Jede Solution besitzt:

- Dauerhafte Solution-GUID
- Version
- Definierten Dateibestand
- Kryptografische Inhaltsprüfsummen
- Deklarierte Capability-Anforderungen
- Optional eine digitale Signatur

Die `solution.xml` beschreibt den prüfbaren Dateibestand. Ein kanonisches Manifest verhindert Mehrdeutigkeiten bei der Hashberechnung.

## Prüfverfahren

Vor der vertrauenswürdigen Ausführung werden geprüft:

1. Gültigkeit und Struktur der `solution.xml`
2. Übereinstimmung der Solution-GUID
3. Vollständigkeit und Integrität aller relevanten Dateien
4. Integrität von Logic Graphs, `.nlf`-Scripts und `.nui`-Oberflächen
5. Gültigkeit vorhandener Signaturen
6. Übereinstimmung mit registrierten Berechtigungen

Die Manifest-Integrität selbst muss durch eine vertrauenswürdige Signatur oder einen geschützten Referenzwert abgesichert sein.

## Änderungen und Berechtigungen

Änderungen an Scripts, Graphen, Capability-Anforderungen oder ausführbaren Komponenten verändern den verifizierten Integritätszustand.

Bereits erteilte Berechtigungen dürfen nicht allein aufgrund einer unveränderten GUID übernommen werden.

Eine erneute Autorisierung ist erforderlich, wenn die verifizierte Identität oder der Sicherheitsvertrag nicht mehr von bestehenden Berechtigungen abgedeckt wird.

Reine Editor-Layoutänderungen dürfen ohne erneute Berechtigungsabfrage behandelt werden, sofern sie nachweislich keine sicherheitsrelevanten Inhalte verändern.

## Signaturen und Vertrauen

Digitale Signaturen ermöglichen die Prüfung von Herkunft und Unverändertheit.

Eine gültige Signatur bedeutet nicht automatisch, dass sämtliche angeforderten Capabilities erlaubt sind.

Nicht signierte Solutions dürfen gemäß NovaOS-Sicherheitsrichtlinien ausgeführt werden, müssen jedoch denselben Integritäts- und Berechtigungsprüfungen unterliegen.

## Laufzeitsicherheit

Nach der Prüfung muss verhindert werden, dass ausführbare Inhalte unbemerkt ausgetauscht werden.

Die Runtime verwendet verifizierte Artefakte oder prüft deren Integrität vor dem Laden erneut.

Bei einer Integritätsverletzung wird die betroffene Ausführung kontrolliert verweigert oder beendet.

## Normative Anforderungen

1. Jede Solution MUSS eine dauerhafte GUID besitzen.
2. Alle sicherheitsrelevanten Solution-Dateien MÜSSEN kryptografisch prüfbar sein.
3. Das Integritätsmanifest MUSS gegen unbemerkte Manipulation geschützt sein.
4. Die Integritätsprüfung MUSS vor der vertrauenswürdigen Ausführung erfolgen.
5. Signaturen MÜSSEN gegen vertrauenswürdige Schlüssel geprüft werden.
6. Eine GUID allein DARF nicht als Sicherheitsnachweis gelten.
7. Berechtigungen MÜSSEN an eine verifizierte Solution-Identität und deren Sicherheitsvertrag gebunden sein.
8. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Berechtigungsprüfung auslösen.
9. Eine gültige Signatur DARF keine Capability-Berechtigungen automatisch erteilen.
10. Nicht verifizierte Inhalte DÜRFEN keine bestehenden Berechtigungen übernehmen.
11. Integritätsverletzungen MÜSSEN protokolliert und kontrolliert behandelt werden.
12. Die Integritätsprüfung MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine kryptografisch abgesicherte Solution-Integrität, die Manipulationen erkennt, Berechtigungen an verifizierte Inhalte bindet und unautorisierte Änderungen an ausführbaren Komponenten verhindert.
