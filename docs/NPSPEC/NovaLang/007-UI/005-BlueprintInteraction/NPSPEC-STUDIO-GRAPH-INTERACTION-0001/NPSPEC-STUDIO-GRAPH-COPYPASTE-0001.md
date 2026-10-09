# NPSPEC-STUDIO-GRAPH-COPYPASTE-0001

**Status:** Angenommen

## Zweck

Kopieren und Einfügen für den NovaLang Studio Logic Graph festlegen.

## Festlegungen

- Kopieren serialisiert ausgewählte Knoten, interne Leitungen und nötige Layoutinformationen in ein versioniertes Austauschformat.
- Einfügen erzeugt neue Knoteninstanz-IDs und prüft NovaLang-Typen, vorhandene Capabilities und Berechtigungen erneut.
- Externe Verbindungen werden nicht stillschweigend wiederhergestellt; sensible Tokens und Zugangsdaten werden nie mitkopiert.

## Ergebnis

Eine einheitliche, nachvollziehbare und für NovaLang sowie das Capability-Modell sichere kopieren und einfügen im Logic Graph.
