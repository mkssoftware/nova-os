# ADR-CRYPTO-0006 – Secure Erase als Key Destruction wo möglich

## Status

Angenommen

## Kategorie

Cryptography / Storage / Key Management / Data Lifecycle

## Kontext

Das sichere physische Überschreiben gespeicherter Daten ist auf modernen Speichersystemen nicht immer zuverlässig möglich.

Flash Translation Layers, Wear Leveling, Copy-on-Write, Snapshots, Replikation, Caches und verteilte Storage-Systeme können dazu führen, dass ältere physische Kopien nicht direkt erreichbar oder gezielt überschreibbar sind.

Für kryptographisch geschützte Daten kann NovaOS stattdessen die Unzugänglichkeit der Daten durch kontrollierte Vernichtung des erforderlichen Schlüsselmaterials erreichen.

---

## Entscheidung

NovaOS verwendet **Cryptographic Erasure durch Key Destruction als bevorzugten Secure-Erase-Mechanismus, wo die kryptographische Architektur dies zuverlässig ermöglicht**.

```text
Encrypted Data
      +
Required Key
      ↓
Readable Data
```

Wird der notwendige Schlüssel irreversibel zerstört:

```text
Encrypted Data
      +
No Recoverable Key
      ↓
Cryptographically Inaccessible
```

Physisches Überschreiben bleibt als ergänzender Mechanismus erhalten, wenn Hardware, Storage-Technologie oder Security Policy dies erfordern.

---

## Grundprinzip

```text
Destroy the ability to decrypt,
not necessarily every physical copy.
```

Cryptographic Erasure ist nur dann gültig, wenn keine autorisierte oder unkontrollierte wiederherstellbare Kopie des erforderlichen Schlüsselmaterials verbleibt.

---

## Key Scope

Die hierarchische Key Architecture muss Löschung auf unterschiedlichen Granularitäten ermöglichen.

```text
Object Key destroyed
    → Object inaccessible

Domain Key destroyed
    → Domain potentially inaccessible

Volume Key destroyed
    → Volume potentially inaccessible
```

NovaOS soll deshalb möglichst die kleinste geeignete Key Domain zerstören.

---

## Per-Object Secure Erase

Per-Object Encryption ermöglicht besonders präzise kryptographische Löschung.

```text
Object
   ↓
Object Key
   ↓ destroy
Object cryptographically erased
```

Andere Objekte derselben Storage Domain bleiben davon unberührt.

---

## Key Wrapping

Bei gewrappten Schlüsseln kann Secure Erase durch Vernichtung des relevanten DEK oder der letzten notwendigen Wrapping-Beziehung erfolgen.

Dabei muss sichergestellt sein, dass keine andere gültige Key Generation, Recovery-Kopie oder Wrapping-Beziehung weiterhin Zugriff ermöglicht.

---

## Snapshots, Backups und Replikate

Key Destruction muss den gesamten relevanten kryptographischen Schutzbereich berücksichtigen.

```text
Primary Storage
Snapshots
Replicas
Backups
Caches
        ↓
Shared Required Key
        ↓ destroy
Cryptographic Erasure
```

Existieren unabhängige entschlüsselbare Kopien oder Schlüssel, ist die Löschung nicht vollständig.

NovaOS darf in diesem Fall keinen erfolgreichen vollständigen Secure Erase behaupten.

---

## Hardwaregeschützte Schlüssel

Nicht exportierbare Schlüssel in TPMs, Secure Elements oder HSMs können durch Backend-spezifische Key-Destruction-Mechanismen gelöscht werden.

NovaOS verwendet hierfür die abstrakte Hardware-Key-Backend-Architektur.

Das Backend muss ausreichende Garantien für die jeweilige Secure-Erase-Policy bereitstellen.

---

## Physisches Löschen

Cryptographic Erasure ersetzt physisches Löschen nicht in allen Fällen.

Zusätzliche Löschmechanismen können erforderlich sein bei:

- unverschlüsselten Daten,
- kompromittiertem Schlüsselmaterial,
- exportierten Schlüsseln,
- regulatorischen Anforderungen,
- geeigneter Hardware mit verifizierbarem Secure-Erase-Support.

```text
Secure Erase Policy
      ↓
Cryptographic Erasure
      +
Optional Physical Erasure
```

---

## Transaktionale Löschung

Secure Erase muss als kontrollierte Systemoperation behandelt werden.

Der Löschvorgang darf nicht als erfolgreich gelten, bevor der erforderliche Key-Destruction-Zustand erreicht wurde.

Irreversible Key Destruction muss explizit von normalem Datei-Löschen, Papierkorb oder reversiblem Storage-Rollback unterschieden werden.

---

## Recovery

Recovery-Fähigkeit und Secure Erase stehen bewusst in Konflikt.

```text
Recoverable
    ≠
Cryptographically Erased
```

Existiert ein gültiger Recovery Key, muss dieser entweder ebenfalls zerstört oder ausdrücklich außerhalb des Secure-Erase-Scopes liegen.

Die resultierende Löschgarantie muss entsprechend angegeben werden.

---

## Audit und Introspection

NovaOS muss den Secure-Erase-Zustand nachvollziehbar darstellen können.

Beispielsweise:

```text
Erase Target
Key Scope
Key Generation
Recovery State
Replication State
Destruction Method
Result
```

Audit-Daten dürfen selbst kein zerstörtes Schlüsselmaterial enthalten.

---

## Normative Anforderungen

1. NovaOS MUSS Cryptographic Erasure durch Key Destruction unterstützen.
2. Key Destruction SOLL bevorzugt werden, wenn sie eine belastbare Löschgarantie bietet.
3. Secure Erase MUSS den tatsächlichen Key Scope berücksichtigen.
4. Per-Object Encryption MUSS objektbezogene Cryptographic Erasure ermöglichen können.
5. Key Wrapping und Key Generations MÜSSEN bei der Löschentscheidung berücksichtigt werden.
6. Verbleibende entschlüsselbare Backups, Snapshots, Replikate oder Recovery Keys MÜSSEN berücksichtigt werden.
7. NovaOS DARF vollständigen Secure Erase NICHT bestätigen, solange relevante Entschlüsselungsmöglichkeiten bestehen.
8. Hardwaregeschützte Schlüssel MÜSSEN über geeignete Backend-Mechanismen zerstörbar sein können.
9. Physisches Löschen MUSS als ergänzender Mechanismus unterstützt werden können.
10. Unverschlüsselte Daten DÜRFEN NICHT allein durch Key Destruction als gelöscht gelten.
11. Secure Erase MUSS von normalem logischem Löschen unterschieden werden.
12. Irreversible Key Destruction MUSS explizit autorisiert werden.
13. Recovery Policies MÜSSEN bei Secure Erase berücksichtigt werden.
14. Secure-Erase-Ergebnisse MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- schnelle Löschung großer verschlüsselter Datenbestände,
- funktioniert auch bei Copy-on-Write, Wear Leveling und Replikation,
- präzise Löschung einzelner Objekte durch Per-Object Keys,
- gute Integration mit der hierarchischen Key Architecture,
- weniger Abhängigkeit von physischen Storage-Eigenschaften.

### Negative Konsequenzen

- Löschgarantie hängt von sicherem Key Management ab,
- Recovery Keys und unabhängige Kopien erschweren vollständige Löschung,
- kompromittiertes oder zuvor exportiertes Schlüsselmaterial kann nicht durch spätere Key Destruction zurückgerufen werden.

---

## Verworfene Alternativen

### Secure Erase ausschließlich durch Überschreiben

Verworfen.

Auf modernen Storage-Systemen kann nicht garantiert werden, dass sämtliche physischen Kopien überschrieben werden.

### Key Destruction immer als ausreichend betrachten

Verworfen.

Unverschlüsselte oder unabhängig entschlüsselbare Kopien würden weiterhin existieren.

### Normales Datei-Löschen als Secure Erase

Verworfen.

Das Entfernen einer logischen Referenz stellt keine belastbare Löschgarantie dar.

---

## Abhängigkeiten

- `ADR-CRYPTO-0002_Verschlüsselung_at_Rest_und_in_Transit`
- `ADR-CRYPTO-0003_Per_Object_Encryption`
- `ADR-CRYPTO-0004_Hierarchische_Key_Architecture`
- `ADR-CRYPTO-0005_Key_Wrapping_und_Key_Rotation`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-SECURE-ERASE-0001`
- `NPSPEC-CRYPTO-KEY-DESTRUCTION-0001`
- `NPSPEC-CRYPTO-OBJECT-ERASURE-0001`
- `NPSPEC-CRYPTO-HARDWARE-KEY-DESTRUCTION-0001`
- `NPSPEC-CRYPTO-SECURE-ERASE-VERIFICATION-0001`
- `NPSPEC-CRYPTO-SECURE-ERASE-TEST-0001`

---

## Ergebnis

NovaOS behandelt sichere Löschung bei verschlüsselten Daten primär als kontrollierte Vernichtung der kryptographischen Zugriffsmöglichkeit:

```text
Encrypted Data
      ↓
Required Key
      ↓
Irreversible Key Destruction
      ↓
Data Cryptographically Inaccessible
```

Damit kann NovaOS auch auf modernen Storage-Systemen mit Snapshots, Copy-on-Write, Wear Leveling und Replikation belastbare Löschmechanismen bereitstellen, sofern sämtliche relevanten Entschlüsselungsmöglichkeiten zuverlässig beseitigt wurden.