# ADR-TRUST-0006 – Software und Supply Chain Provenance

## Status

Angenommen

## Kategorie

Trust Architecture / Software Trust / Supply Chain

## Kontext

Software in NovaOS entsteht nicht nur aus dem finalen ausführbaren Artefakt. Quellcode, Abhängigkeiten, Build-Systeme, Compiler, Pakete und Transformationen bilden eine Supply Chain.

Eine gültige Signatur des Endprodukts reicht deshalb nicht aus, um dessen Herkunft und Entstehung nachvollziehen zu können.

Nova.Trust benötigt eine durchgängige Software-Provenance.

## Entscheidung

NovaOS behandelt **Software und Supply Chain Provenance als First-Class Trust Evidence**.

```text
Source
  ↓
Dependencies
  ↓
Build
  ↓
Artifact
  ↓
Package
  ↓
Installation
  ↓
Execution
```

Relevante Beziehungen dieser Kette können durch Nova.Trust erfasst, verifiziert und für Trust-Entscheidungen verwendet werden.

## Grundprinzip

```text
Trust the provenance,
not only the final artifact.
```

Die Herkunft eines Softwareartefakts muss getrennt von seiner Signatur betrachtet werden.

## Provenance Chain

Software-Provenance kann insbesondere Beziehungen zwischen folgenden Entitäten beschreiben:

- Source Repository,
- Source Revision,
- Dependencies,
- Build Environment,
- Build Tools,
- Compiler und Toolchain,
- Build Identity,
- erzeugten Artefakten,
- Package und Distribution,
- Installation.

Die konkreten Provenance-Datenformate werden in NPSPECs definiert.

## Abhängigkeiten

Softwareabhängigkeiten werden Teil der Provenance-Beziehung.

```text
Application
   ├── Library A
   ├── Library B
   └── Capability Provider C
```

Dadurch kann Nova.Trust nicht nur das Hauptartefakt, sondern auch relevante Bestandteile seiner Supply Chain berücksichtigen.

## Build Provenance

Ein Build kann mit nachvollziehbarer Evidence an seine Eingaben und Build-Umgebung gebunden werden.

```text
Sources + Dependencies + Toolchain
                ↓
              Build
                ↓
             Artifact
```

Reproduzierbare oder anderweitig verifizierbare Builds können zusätzliche Trust Evidence liefern, sind jedoch nicht zwingend die einzige zulässige Build-Strategie.

## Provenance und Signatur

Signatur und Provenance bleiben getrennte Trust-Eigenschaften.

```text
Signature  → Who signed this artifact?
Provenance → How did this artifact come into existence?
```

Eine gültige Signatur darf fehlende oder widersprüchliche Provenance nicht automatisch ersetzen.

## Updates und Supply Chain

Softwareupdates erzeugen neue Provenance-Zustände.

Die stabile Application- oder Service-Identity kann erhalten bleiben, während Version, Artefakt, Build und Abhängigkeiten wechseln.

Nova.Trust kann dadurch neue Versionen unabhängig bewerten.

## Legacy Software

Software ohne vollständige Provenance darf weiterhin über definierte Compatibility Policies verwendbar sein.

Fehlende Provenance muss jedoch als fehlende Trust Evidence erkennbar bleiben.

```text
Unknown Provenance ≠ Trusted Provenance
```

## Normative Anforderungen

1. Nova.Trust MUSS Software-Provenance als native Trust Evidence unterstützen.
2. Provenance MUSS vom Signaturstatus getrennt bewertet werden.
3. Softwareartefakte MÜSSEN mit relevanten Herkunftsinformationen verknüpfbar sein.
4. Abhängigkeiten MÜSSEN Bestandteil der Supply-Chain-Provenance sein können.
5. Build-Prozesse MÜSSEN Provenance Evidence erzeugen können.
6. Toolchain- und Build-Identity MÜSSEN in Trust-Entscheidungen einbeziehbar sein.
7. Updates MÜSSEN neue Provenance-Zustände erzeugen können, ohne automatisch die stabile Application Identity zu verändern.
8. Widersprüchliche Provenance MUSS als Trust-relevanter Zustand behandelbar sein.
9. Fehlende Provenance DARF nicht als verifizierte Provenance interpretiert werden.
10. Legacy Software MUSS explizit als Software mit eingeschränkter Provenance behandelbar sein.
11. Provenance DARF keine Capability Authority erzeugen.
12. Relevante Supply-Chain-Beziehungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Softwareherkunft wird über das Endartefakt hinaus nachvollziehbar,
- Supply-Chain-Angriffe können besser erkannt und eingegrenzt werden,
- Build-Systeme und Abhängigkeiten werden Teil der Trust-Bewertung,
- Softwareupdates können unabhängig neu bewertet werden,
- Provenance ergänzt Code Signing statt es zu ersetzen.

### Negative Konsequenzen

- Build- und Distributionssysteme müssen Provenance-Daten bereitstellen,
- Legacy Software besitzt häufig nur unvollständige Provenance.

## Verworfene Alternativen

### Nur finale Software-Signatur prüfen

Verworfen, da die vorherige Supply Chain dadurch unsichtbar bleibt.

### Herkunft ausschließlich über Paketquelle bestimmen

Verworfen, da Repository oder Store nur einen Teil der Softwareherkunft darstellen.

### Software ohne vollständige Provenance grundsätzlich verbieten

Verworfen, da dies Legacy- und Compatibility-Szenarien unnötig ausschließen würde.

## Abhängigkeiten

- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0005_Trust_Graph_statt_globalem_Trust_Score`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-IDENTITY-0005_Service_und_Application_Identity`

## Zugehörige NPSPECs

- `NPSPEC-TRUST-SOFTWARE-PROVENANCE-0001`
- `NPSPEC-TRUST-SUPPLY-CHAIN-0001`
- `NPSPEC-TRUST-BUILD-PROVENANCE-0001`
- `NPSPEC-TRUST-DEPENDENCY-PROVENANCE-0001`
- `NPSPEC-TRUST-SOFTWARE-PROVENANCE-TEST-0001`

## Ergebnis

Nova.Trust bewertet Software nicht ausschließlich anhand ihres finalen Artefakts:

```text
Source
  ↓
Dependencies
  ↓
Build + Toolchain
  ↓
Artifact
  ↓
Distribution
  ↓
Installation
  ↓
Nova.Trust
```

Damit wird die gesamte relevante Software Supply Chain zu nachvollziehbarer Trust Evidence, während Signatur, Provenance, Trust und Authority weiterhin klar getrennte Konzepte bleiben.