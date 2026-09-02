# ADR-SECURITY-0019 – Secret Use und Secret Export getrennt

## Status

Angenommen

## Kategorie

Security / Secrets / Capabilities / Key Management

## Kontext

Anwendungen und Systemdienste müssen Geheimnisse wie kryptographische Schlüssel, Tokens oder Credentials verwenden können.

Für viele Operationen ist es jedoch nicht erforderlich, das eigentliche Secret offenzulegen.

Beispielsweise benötigt eine Anwendung für eine Signatur lediglich:

```text
Sign(Data, Key)
```

und nicht:

```text
Export(Key)
```

Würde die Nutzung eines Secrets automatisch dessen Export erlauben, könnten kompromittierte Komponenten sensible Schlüssel oder Credentials dauerhaft extrahieren.

---

## Entscheidung

NovaOS trennt **Secret Use und Secret Export als unterschiedliche Authorities**.

```text
Use Secret
    ≠
Export Secret
```

Eine Capability zur Verwendung eines Secrets berechtigt nicht automatisch dazu, dessen Rohwert auszulesen oder zu exportieren.

---

## Grundprinzip

```text
Use authority does not imply disclosure authority.
```

---

## Secret Use

Eine Use-Capability erlaubt definierte Operationen mit einem Secret.

Beispiele:

```text
Sign
Decrypt
Authenticate
Derive Key
Generate MAC
```

Das Secret verbleibt dabei möglichst innerhalb seiner geschützten Secret Domain.

```text
Application
    ↓
Use Capability
    ↓
Secret Service
    ↓
Cryptographic Operation
    ↓
Result
```

---

## Secret Export

Der Export eines Secrets ist eine eigenständige privilegierte Operation.

```text
Secret
   ↓
Export Capability
   ↓
Authorized Destination
```

Export kann vollständig verboten oder zusätzlich durch:

- MAC,
- ABAC,
- Trust,
- Data Sovereignty,
- Information Flow

beschränkt werden.

---

## Nicht exportierbare Secrets

NovaOS muss Secrets unterstützen, die grundsätzlich als nicht exportierbar definiert sind.

Beispiele sind Schlüssel in:

```text
TPM
Secure Element
Hardware Security Module
Protected Key Store
```

Solche Secrets können verwendet werden, ohne dass ihre Rohdaten jemals der aufrufenden Anwendung zugänglich werden.

---

## Capability-Modell

Secret Authority muss operationsbezogen vergeben werden können.

Beispiel:

```text
Key Capability
 ├── Sign
 └── Verify
```

bedeutet nicht automatisch:

```text
Decrypt
Derive
Export
Delete
```

Damit folgt Secret-Zugriff dem Least-Privilege-Prinzip.

---

## Delegation

Eine delegierte Secret-Capability darf nur die ausdrücklich delegierte Authority übertragen.

```text
Original:
Sign + Export

        ↓ Attenuation

Delegated:
Sign
```

Die delegierte Komponente erhält dadurch keinen Zugriff auf das Secret selbst.

---

## Information Flow

Secret Export wird als besonders sensitiver Information Flow behandelt.

Eine Export-Capability darf bestehende:

- Information-Flow-Regeln,
- Data-Sovereignty-Regeln,
- Trust Requirements,
- MAC-Policies

nicht umgehen.

---

## Secret Services

NovaOS soll Secret-Verarbeitung bevorzugt über isolierte Dienste oder Hardware durchführen können.

```text
Caller
   ↓
Typed Operation
   ↓
Secret Service / Secure Hardware
   ↓
Secret
```

Dadurch muss Secret-Material nicht unnötig zwischen Execution Domains kopiert werden.

---

## Audit und Introspection

NovaOS muss zwischen Secret Use und Secret Export unterscheiden können.

Audit-Ereignisse können beispielsweise erfassen:

```text
Secret Identity
Operation
Caller
Capability
Export / Non-Export
Result
```

Das Secret selbst darf nicht Bestandteil des Audit-Trails sein.

---

## Normative Anforderungen

1. NovaOS MUSS Secret Use und Secret Export als getrennte Authorities behandeln.
2. Eine Use-Capability DARF NICHT automatisch Secret Export erlauben.
3. Secret-Authority MUSS operationsbezogen beschränkbar sein.
4. NovaOS MUSS nicht exportierbare Secrets unterstützen können.
5. Secret-Operationen MÜSSEN ohne Offenlegung des Roh-Secrets ausführbar sein können.
6. Secret Export MUSS eine explizite Capability erfordern.
7. Export MUSS zusätzlich durch Security-, Trust-, IFC- und Sovereignty-Policies beschränkbar sein.
8. Capability Delegation DARF keine nicht vorhandene Export-Authority erzeugen.
9. Attenuation MUSS Secret-Authority weiter einschränken können.
10. Hardwaregeschützte Secrets MÜSSEN ohne Export verwendbar sein können.
11. Secret-Material SOLL Execution-Domain-Grenzen nur verlassen, wenn dies ausdrücklich erforderlich und autorisiert ist.
12. Secret Use und Secret Export MÜSSEN getrennt auditierbar sein.
13. Audit und Introspection DÜRFEN Secret-Material NICHT offenlegen.
14. Unklare oder fehlende Export-Authority MUSS fail-closed behandelt werden.

---

## Konsequenzen

### Positive Konsequenzen

- geringeres Risiko der Secret-Extraktion,
- Unterstützung nicht exportierbarer Hardware-Schlüssel,
- konsequentes Least Privilege,
- sichere Delegation kryptographischer Operationen,
- Secrets müssen Anwendungen häufig überhaupt nicht offengelegt werden.

### Negative Konsequenzen

- Anwendungen müssen Secret-Operationen über definierte Interfaces ausführen,
- Legacy-Software kann direkten Schlüsselzugriff erwarten,
- Secret Services werden zu sicherheitskritischen Komponenten.

---

## Verworfene Alternativen

### Secret-Zugriff impliziert vollständigen Export

Verworfen.

Dies würde unnötig weitreichende Authority erzeugen.

### Secrets immer als normale Dateien behandeln

Verworfen.

Dateizugriff bildet die unterschiedlichen Secret-Operationen nicht ausreichend ab.

### Export grundsätzlich verbieten

Verworfen.

Bestimmte legitime Backup-, Migration- oder Interoperabilitätsszenarien benötigen kontrollierten Export.

---

## Abhängigkeiten

- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0009_Capability_Delegation`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-SECRET-0001`
- `NPSPEC-SECURITY-SECRET-USE-0001`
- `NPSPEC-SECURITY-SECRET-EXPORT-0001`
- `NPSPEC-SECURITY-NON-EXPORTABLE-SECRET-0001`
- `NPSPEC-SECURITY-SECRET-TEST-0001`

---

## Ergebnis

NovaOS behandelt die Verwendung und Offenlegung eines Secrets als unterschiedliche Sicherheitsoperationen:

```text
Secret
 ├── Use Capability
 │      ↓
 │   Operation
 │
 └── Export Capability
        ↓
     Disclosure
```

Eine Komponente kann dadurch mit einem Secret arbeiten, ohne das Secret selbst besitzen oder auslesen zu können.

Dies reduziert die notwendige Authority und begrenzt die Auswirkungen kompromittierter Anwendungen und Dienste.