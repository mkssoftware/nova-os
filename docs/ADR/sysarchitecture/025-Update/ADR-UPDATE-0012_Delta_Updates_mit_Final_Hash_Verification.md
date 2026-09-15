# ADR-UPDATE-0012 – Delta Updates mit Final Hash Verification

## Status

Angenommen

## Kategorie

Update / Distribution / Integrity

## Kontext

Content-Addressed Packages können bei großen Systemkomponenten erhebliche Downloadmengen verursachen, obwohl sich zwischen zwei Versionen nur kleine Teile geändert haben.

NovaOS soll deshalb Updates effizient als Delta übertragen können, ohne die Integritätsgarantien der vollständigen Pakete zu verlieren.

## Entscheidung

NovaOS unterstützt **Delta Updates** zwischen bekannten Paketversionen.

```text
Old Package
    +
Delta
    ↓
New Package
```

Nach Anwendung des Deltas MUSS jedoch immer die Content ID des vollständig rekonstruierten Zielartefakts überprüft werden.

```text
Reconstructed Package
        ↓
Final Hash
        ↓
Expected Content ID
```

Nur bei Übereinstimmung darf das Artefakt verwendet werden.

## Delta

Ein Delta beschreibt die Transformation zwischen zwei eindeutig identifizierten Artefakten:

```text
Source Content ID
        ↓
      Delta
        ↓
Target Content ID
```

Ein Delta darf nur auf die dafür vorgesehene Source Content ID angewendet werden.

## Final Hash Verification

Die Integrität des Deltas allein reicht nicht aus.

Entscheidend ist die Integrität des resultierenden vollständigen Artefakts.

```text
Apply Delta
    ↓
Calculate Final Hash
    ↓
Hash == Target Content ID?
 ┌──────┴──────┐
 ↓             ↓
Yes            No
 ↓             ↓
Accept        Reject
```

Damit besitzt ein per Delta erzeugtes Paket dieselbe Content-Integrity-Garantie wie ein vollständig heruntergeladenes Paket.

## Fallback

Kann ein Delta nicht angewendet oder erfolgreich verifiziert werden, muss NovaOS auf das vollständige Paket zurückfallen können.

```text
Delta Failed
     ↓
Full Package
     ↓
Hash Verification
```

Ein fehlgeschlagenes Delta darf keinen teilweise aktualisierten Zustand aktivieren.

## Auswahl

Delta Updates sind eine Transportoptimierung.

NovaOS kann abhängig von:

```text
Delta Size
Full Package Size
Available Source Version
Network Cost
CPU Cost
Storage
```

entscheiden, ob Delta oder vollständiges Paket verwendet wird.

Die resultierende Target Content ID bleibt in beiden Fällen identisch.

## Update Pipeline

Delta-Rekonstruktion erfolgt vor der Aktivierung:

```text
Acquire Delta
     ↓
Verify Source
     ↓
Apply Delta
     ↓
Final Hash Verification
     ↓
Prepare
     ↓
Atomic Update
```

Die Transactional Update Pipeline behandelt danach das rekonstruierte Artefakt wie jedes andere verifizierte Content-Addressed Package.

## Security

Ein Delta erhält keine eigene Vertrauensstellung gegenüber dem Zielartefakt.

```text
Valid Delta
    ≠
Valid Target Package
```

Signatur, Provenance und Trust werden auf Grundlage des endgültigen Zielartefakts beziehungsweise seiner zugehörigen Evidenz geprüft.

## Normative Anforderungen

1. NovaOS MUSS Delta Updates unterstützen können.
2. Ein Delta MUSS eindeutig Source und Target Content ID zugeordnet sein.
3. Vor Anwendung MUSS die Source Content ID überprüft werden.
4. Nach Rekonstruktion MUSS der vollständige Target Hash berechnet werden.
5. Der Final Hash MUSS der erwarteten Target Content ID entsprechen.
6. Bei Hash-Mismatch MUSS das rekonstruierte Artefakt verworfen werden.
7. Ein fehlgeschlagenes Delta DARF keinen teilweise aktualisierten Zustand aktivieren.
8. NovaOS MUSS auf den Download des vollständigen Pakets zurückfallen können.
9. Delta Updates DÜRFEN Package-Signing-, Provenance- oder Trust-Prüfungen nicht ersetzen.
10. Delta Updates MÜSSEN vor dem Commit der Transactional Update Pipeline abgeschlossen und verifiziert sein.

## Konsequenzen

### Positive Konsequenzen

- deutlich kleinere Update-Downloads,
- geringere Netzwerkbelastung,
- Content-Addressing bleibt vollständig erhalten,
- Delta und Full Package führen zum selben verifizierten Zielartefakt.

### Negative Konsequenzen

- Delta-Erzeugung und -Anwendung benötigen zusätzliche Logik,
- Rekonstruktion verursacht zusätzlichen CPU- und Storage-Aufwand,
- mehrere Ausgangsversionen können unterschiedliche Deltas benötigen.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0010_Package_Signing_Provenance_und_SBOM`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-DELTA-0001`
- `NPSPEC-UPDATE-DELTA-FORMAT-0001`
- `NPSPEC-UPDATE-DELTA-VERIFICATION-0001`
- `NPSPEC-UPDATE-DELTA-FALLBACK-0001`

## Ergebnis

NovaOS darf Updates effizient als Delta übertragen:

```text
Known Source
     +
Delta
     ↓
Reconstruct
     ↓
Final Hash Verification
     ↓
Exact Target Content ID
```

Der zentrale Grundsatz lautet:

```text
Das Delta optimiert die Übertragung.

Vertraut wird ausschließlich
dem vollständig rekonstruierten
und verifizierten Zielartefakt.
```