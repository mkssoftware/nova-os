# ADR-IDENTITY-0007 – Federated Identity

## Status

Angenommen

## Kategorie

Identity Architecture / Federation

## Kontext

NovaOS muss Identitäten aus externen Identity-Systemen integrieren können, ohne seine eigene Identity-Architektur von diesen Systemen abhängig zu machen.

Dazu gehören beispielsweise:

- Unternehmensidentitäten,
- Cloud-Identitäten,
- Web-Identity-Provider,
- andere NovaOS-Systeme,
- organisationsübergreifende Identitäten.

Externe Identitäten dürfen dabei weder automatisch lokale Authority noch vollständiges Vertrauen erhalten.

## Entscheidung

NovaOS unterstützt **Federated Identity** als kontrollierte Verbindung zwischen einer NovaOS-Identity und einer extern bestätigten Identity.

```text
External Identity Provider
          ↓
Federation / Verification
          ↓
Federated Identity Binding
          ↓
NovaOS Identity
```

Die externe Identity bleibt von der lokalen NovaOS-Identity getrennt.

## Grundprinzip

```text
Federated Identity = Verified external identity relationship

Federated Identity ≠ Local Identity
Federated Identity ≠ Authority
Federated Identity ≠ Trust
```

Federation erweitert das Identity-Modell, ersetzt es aber nicht.

## Federated Identity Binding

Eine NovaOS-Identity kann mit einer oder mehreren externen Identities verbunden werden.

```text
NovaOS Identity
 ├── Local Credentials
 ├── Federated Identity A
 └── Federated Identity B
```

Das Binding muss den externen Identity Provider und die dortige Identity eindeutig referenzieren.

## Federation Provider

Unterschiedliche Federation-Technologien werden über austauschbare Provider integriert.

```text
Identity System
      ↓
Federation Provider
      ↓
NovaOS Identity Model
```

Das native NovaOS-Identity-Modell bleibt dadurch unabhängig von konkreten Protokollen oder externen Plattformen.

## Authentication und Authority

Ein Federation Provider kann bestätigen, dass eine externe Identity erfolgreich authentisiert wurde.

Die daraus resultierende lokale Authority wird jedoch separat bestimmt.

```text
External Authentication
        ↓
Verified Identity
        ↓
NovaOS Policy
        ↓
Capabilities / Authority
```

Externe Rollen, Gruppen oder Claims dürfen nicht ungeprüft zu NovaOS-Authority werden.

## Lifecycle

Federated Identity Bindings müssen erstellt, aktualisiert, getrennt und widerrufen werden können.

Der Verlust oder die Entfernung eines Federation Bindings darf eine unabhängige lokale Identity nicht automatisch zerstören.

## Offline-Verhalten

Federated Identity darf das First-Class-Modell lokaler Identitäten nicht beeinträchtigen.

Operationen, die eine aktuelle externe Verifikation benötigen, können Netzwerkzugriff erfordern.

Bereits vorhandene lokale Identity-Funktionen bleiben davon unabhängig, sofern ihre Policy keine erneute Federation-Verifikation verlangt.

## Privacy und Data Sovereignty

Bei Federation dürfen nur notwendige Identity-Daten mit externen Providern ausgetauscht werden.

Federation unterliegt:

- Data Minimization,
- Selective Disclosure,
- Data Sovereignty,
- Information Flow Control.

## Normative Anforderungen

1. NovaOS MUSS Federated Identity unterstützen können.
2. Federated Identity MUSS vom nativen NovaOS-Identity-Modell getrennt bleiben.
3. Eine NovaOS-Identity MUSS mehrere Federation Bindings besitzen können.
4. Federation MUSS über austauschbare Provider integrierbar sein.
5. Externe Authentication DARF keine automatische NovaOS-Authority erzeugen.
6. Externe Rollen, Gruppen und Claims MÜSSEN durch NovaOS-Policies ausgewertet werden.
7. Federation Bindings MÜSSEN widerrufbar sein.
8. Das Entfernen eines Bindings DARF eine unabhängige lokale Identity nicht automatisch zerstören.
9. Federation DARF lokale First-Class Identity nicht voraussetzen oder ersetzen.
10. Identity-Daten MÜSSEN nach Data-Minimization-Prinzipien ausgetauscht werden.
11. Data-Sovereignty- und IFC-Regeln MÜSSEN auch für Federation gelten.
12. Federation Bindings und relevante Verifikationen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Integration externer Identity-Systeme,
- keine Abhängigkeit des Kernmodells von einzelnen Identity Providern,
- lokale und federierte Identities können kombiniert werden,
- externe Authentication bleibt von NovaOS-Authority getrennt.

### Negative Konsequenzen

- Federation Provider benötigen Trust- und Lifecycle-Management,
- externe Systeme können temporär nicht erreichbar sein.

## Verworfene Alternativen

### Externe Identity direkt als NovaOS-Identity verwenden

Verworfen, da dadurch das native Identity-Modell von externen Systemen abhängig würde.

### Externe Claims direkt als Authority übernehmen

Verworfen, da Authentication und Authorization getrennt bleiben müssen.

### Nur einen Federation Provider unterstützen

Verworfen, da NovaOS protokoll- und providerunabhängig bleiben soll.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-IDENTITY-0006_Lokale_Identität_als_First_Class_Modell`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0005_Selective_Disclosure`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-FEDERATION-0001`
- `NPSPEC-IDENTITY-FEDERATION-PROVIDER-0001`
- `NPSPEC-IDENTITY-FEDERATION-BINDING-0001`
- `NPSPEC-IDENTITY-FEDERATION-CLAIMS-0001`
- `NPSPEC-IDENTITY-FEDERATION-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-FEDERATION-TEST-0001`

## Ergebnis

NovaOS integriert externe Identität über kontrollierte Federation Bindings:

```text
External Identity
       ↓
Federation Provider
       ↓
Verified Binding
       ↓
NovaOS Identity
       ↓
Policy
       ↓
Capabilities
```

Damit können externe Identity-Systeme verwendet werden, ohne die Unabhängigkeit des nativen NovaOS-Identity- und Security-Modells aufzugeben.