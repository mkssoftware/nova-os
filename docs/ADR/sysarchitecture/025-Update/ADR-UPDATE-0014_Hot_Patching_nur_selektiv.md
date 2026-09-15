# ADR-UPDATE-0014 – Hot Patching nur selektiv

## Status

Angenommen

## Kategorie

Update / Hot Patching / Security

## Kontext

Hot Patching ermöglicht Änderungen an laufendem Code ohne Neustart einer Komponente oder des Systems.

Dies kann besonders bei kritischen Sicherheitskorrekturen oder hochverfügbaren Systemen sinnvoll sein.

Hot Patching erhöht jedoch die Komplexität und kann den tatsächlich laufenden Zustand vom ursprünglich verifizierten Artefakt abweichen lassen.

## Entscheidung

NovaOS unterstützt **Hot Patching nur selektiv**.

Hot Patching ist kein allgemeiner Update-Mechanismus, sondern eine kontrollierte Ausnahme für ausdrücklich dafür geeignete Komponenten und Patches.

```text
Normal Update
    ↓
Preferred

Hot Patch
    ↓
Exceptional Path
```

## Zulässigkeit

Ein Hot Patch darf nur angewendet werden, wenn:

```text
Component supports Hot Patching
Patch is explicitly authorized
Target Version is exact
Compatibility is verified
Rollback is defined
```

Unbekannte oder nicht eindeutig passende Zustände dürfen nicht gepatcht werden.

## Exakte Zielbindung

Jeder Patch muss an einen eindeutig bestimmten Ausgangszustand gebunden sein.

```text
Target Content ID H1
        +
Patch P
        ↓
Patched State H2
```

Ein Patch für H1 darf nicht auf eine lediglich ähnliche Version angewendet werden.

## Patch-Verifikation

Vor Anwendung müssen mindestens geprüft werden:

```text
Target Identity
Patch Integrity
Signature
Provenance
Authorization
Compatibility
```

Hot Patching darf keine Trust- oder Supply-Chain-Prüfung umgehen.

## Anwendung

Der Patch wird nur an definierten sicheren Patch Points aktiviert.

```text
Running Component
       ↓
Safe Patch Point
       ↓
Apply Patch
       ↓
Verify
       ↓
Continue
```

Laufender Code darf nicht in einen undefinierten Zwischenzustand geraten.

## Rollback

Hot Patches müssen grundsätzlich eine definierte Rückkehrstrategie besitzen.

```text
Patched State
     ↓
Failure
     ↓
Rollback
     ↓
Previous State
```

Ist ein sicherer Rollback nicht möglich, muss dies vor Anwendung explizit bekannt sein und entsprechend behandelt werden.

## Persistenter Zustand

Ein Hot Patch darf nicht zum dauerhaft unbekannten Systemzustand werden.

Der nächste reguläre Softwarezustand muss die Korrektur entweder enthalten oder den Hot Patch kontrolliert ablösen.

```text
Base Version
    +
Hot Patch
    ↓
Temporary State
    ↓
Regular Updated Version
```

## Kritische Komponenten

Hot Patching kann insbesondere für ausgewählte:

```text
Kernel Components
Security Services
Critical Drivers
Core Services
```

verwendet werden.

Die Kritikalität einer Komponente allein rechtfertigt jedoch keinen Hot Patch.

## Normative Anforderungen

1. Hot Patching DARF nur für ausdrücklich unterstützte Komponenten verwendet werden.
2. Jeder Hot Patch MUSS an einen exakten Ausgangszustand gebunden sein.
3. Patch, Zielzustand und Autorisierung MÜSSEN vor Anwendung verifiziert werden.
4. Hot Patches MÜSSEN über definierte sichere Patch Points aktiviert werden.
5. Ein Hot Patch DARF keinen undefinierten Laufzeitzustand erzeugen.
6. Eine Rollback- oder Recovery-Strategie MUSS definiert sein.
7. Hot Patching DARF Signatur-, Provenance-, Trust- oder Security-Prüfungen nicht umgehen.
8. Der gepatchte Zustand MUSS systemweit als solcher erkennbar sein.
9. Hot Patches SOLLEN durch ein reguläres Update konsolidiert werden.
10. Hot Patching DARF reguläre transaktionale Updates nicht als Standardmechanismus ersetzen.

## Konsequenzen

### Positive Konsequenzen

- kritische Korrekturen können ohne vollständigen Neustart aktiviert werden,
- Ausfallzeiten können reduziert werden,
- sicherheitskritische Fehler können schneller behoben werden.

### Negative Konsequenzen

- Laufzeitzustände werden komplexer,
- Patch-Kompatibilität muss exakt geprüft werden,
- Testing und Recovery werden aufwendiger.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0010_Package_Signing_Provenance_und_SBOM`
- `ADR-UPDATE-0013_Live_Service_Driver_und_Component_Updates`
- `ADR-ARCH-0012_Hot_Replacement`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-HOT-PATCH-0001`
- `NPSPEC-UPDATE-HOT-PATCH-VERIFICATION-0001`
- `NPSPEC-UPDATE-HOT-PATCH-ROLLBACK-0001`

## Ergebnis

NovaOS behandelt Hot Patching als kontrollierte Ausnahme:

```text
Exact Known State
      ↓
Authorized Patch
      ↓
Verification
      ↓
Safe Patch Point
      ↓
Apply
      ↓
Verify
```

Der zentrale Grundsatz lautet:

```text
Hot Patching nur dort,
wo ein sicherer Live-Eingriff
explizit nachgewiesen ist.
```