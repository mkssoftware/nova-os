# ADR-SECURITY-0017 – Break-Glass Access mit Audit

## Status

Angenommen

## Kategorie

Security / Emergency Access / Authorization / Audit

## Kontext

NovaOS besitzt bewusst kein universelles Root-Privileg. Administrative Operationen benötigen konkrete Capabilities und unterliegen weiterhin MAC-, ABAC-, Trust- und Sovereignty-Regeln.

Für außergewöhnliche Situationen kann jedoch ein kontrollierter Notfallzugriff erforderlich sein, beispielsweise bei:

- beschädigten Security Policies,
- ausgefallenen Identity- oder Authorization-Diensten,
- Recovery kritischer Systemzustände,
- Verlust regulärer administrativer Zugänge,
- schwerwiegenden Sicherheitsvorfällen.

Dafür benötigt NovaOS einen expliziten Break-Glass-Mechanismus.

---

## Entscheidung

NovaOS unterstützt **Break-Glass Access als kontrollierten, zeitlich begrenzten und vollständig auditierbaren Notfallzugriff**.

Break-Glass erzeugt keinen universellen Root-Modus.

Stattdessen werden ausschließlich die für den Notfall erforderlichen erhöhten Capabilities bereitgestellt.

```text
Emergency Condition
       ↓
Break-Glass Authorization
       ↓
Temporary Capability Set
       ↓
Emergency Operation
       ↓
Audit + Expiration
```

---

## Grundprinzip

```text
Emergency access is exceptional authority,
not universal authority.
```

---

## Explizite Aktivierung

Break-Glass muss bewusst aktiviert werden.

Eine normale Anwendung oder ein Systemdienst darf nicht automatisch in einen Break-Glass-Kontext wechseln.

Die Aktivierung kann abhängig von der Security Policy zusätzliche Anforderungen besitzen, beispielsweise:

- starke Authentifizierung,
- physische Anwesenheit,
- Recovery-Kontext,
- mehrere autorisierende Personen,
- Hardware- oder Recovery-Schlüssel.

Die konkreten Verfahren werden in NPSPECs definiert.

---

## Begrenzte Authority

Break-Glass stellt nur die für den jeweiligen Notfall vorgesehenen Capabilities bereit.

Beispiel:

```text
Repair Security Policy
        ≠
Unlimited System Authority
```

Auch Break-Glass folgt soweit möglich dem Least-Privilege-Prinzip.

---

## Zeitliche Begrenzung

Break-Glass-Authority muss zeitlich oder kontextuell begrenzt sein.

```text
Activate
   ↓
Emergency Session
   ↓
Temporary Authority
   ↓
Expire / Revoke
```

Capability Leases und Epochs können hierfür verwendet werden.

Dauerhafte Break-Glass-Authority ist nicht Teil des nativen Sicherheitsmodells.

---

## Sicherheitsgrenzen

Break-Glass darf nicht automatisch sämtliche Hard Constraints deaktivieren.

Insbesondere bleiben unabhängig kontrollierbar:

- Data Sovereignty,
- Information Flow,
- Code Integrity,
- Trust Anchors,
- Boot Security,
- Ressourcen- und Hardwaregrenzen.

Welche Regeln in einem konkreten Recovery-Szenario verändert werden dürfen, muss explizit autorisiert sein.

---

## Audit

Jede Break-Glass-Nutzung muss besonders gekennzeichnet und auditierbar sein.

Mindestens nachvollziehbar sein müssen:

```text
Who
When
Why
Which Authority
Which Operations
Which Resources
Result
```

Break-Glass darf den eigenen Audit-Trail nicht stillschweigend entfernen oder verändern können.

---

## NovaDOS und Recovery

NovaDOS und andere Recovery-Umgebungen können Break-Glass-Funktionen bereitstellen.

```text
Recovery Environment
       ↓
Break-Glass Authorization
       ↓
Restricted Recovery Capabilities
```

Der Recovery-Kontext bedeutet jedoch nicht automatisch unbegrenzte Authority.

---

## Fail-Safe Verhalten

Kann eine erforderliche Break-Glass-Autorisierung nicht zuverlässig validiert werden, muss der Zugriff abgelehnt werden.

```text
Unknown Authorization
        ↓
Deny
```

Ein Notfall allein darf keine implizite Umgehung der Sicherheitsarchitektur erzeugen.

---

## Normative Anforderungen

1. NovaOS MUSS kontrollierten Break-Glass Access unterstützen können.
2. Break-Glass DARF KEIN universelles Root-Privileg erzeugen.
3. Break-Glass MUSS explizit aktiviert und autorisiert werden.
4. Break-Glass MUSS konkrete, begrenzte Capabilities bereitstellen.
5. Break-Glass-Authority MUSS zeitlich oder kontextuell begrenzbar sein.
6. Break-Glass MUSS mit Capability Revocation, Leases und Epochs integrierbar sein.
7. Least Privilege SOLL auch für Break-Glass gelten.
8. Hard Constraints DÜRFEN nur durch ausdrücklich dafür vorgesehene Emergency Authority verändert werden.
9. Break-Glass-Nutzung MUSS besonders gekennzeichnet und auditierbar sein.
10. Audit-Einträge MÜSSEN gegen unautorisierte Veränderung geschützt sein.
11. NovaDOS und Recovery MÜSSEN Break-Glass ohne universelle Authority unterstützen können.
12. Multi-Party Authorization MUSS für besonders kritische Break-Glass-Operationen möglich sein.
13. Nach Ende des Emergency Context MUSS die temporäre Authority ungültig werden.
14. Nicht zuverlässig validierbare Break-Glass-Autorisierung MUSS fail-safe abgelehnt werden.

---

## Konsequenzen

### Positive Konsequenzen

- kontrollierte Wiederherstellung bei schweren Systemproblemen,
- kein universelles dauerhaftes Notfallprivileg,
- nachvollziehbare Nutzung außergewöhnlicher Authority,
- Integration mit dem bestehenden Capability-Modell,
- Unterstützung sicherheitskritischer Recovery-Szenarien.

### Negative Konsequenzen

- zusätzliche Recovery- und Authorization-Komplexität,
- Break-Glass-Credentials selbst werden zu besonders schützenswerten Assets,
- zu restriktive Policies können Notfallmaßnahmen erschweren.

---

## Verworfene Alternativen

### Universelles Emergency-Root

Verworfen.

Dies würde eine globale Sicherheitsumgehung schaffen.

### Break-Glass ohne Audit

Verworfen.

Außergewöhnliche Authority muss besonders nachvollziehbar sein.

### Automatischer Break-Glass-Modus bei Systemfehlern

Verworfen.

Ein Fehler darf keine automatische Privilegienerweiterung verursachen.

---

## Abhängigkeiten

- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-BOOT-0011_NovaDOS_als_Offline_Recovery_Umgebung`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0002_Kein_universelles_Root_Privileg`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0010_Capability_Revocation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-BREAK-GLASS-0001`
- `NPSPEC-SECURITY-BREAK-GLASS-AUTHORIZATION-0001`
- `NPSPEC-SECURITY-BREAK-GLASS-AUDIT-0001`
- `NPSPEC-SECURITY-BREAK-GLASS-RECOVERY-0001`
- `NPSPEC-SECURITY-BREAK-GLASS-TEST-0001`

---

## Ergebnis

NovaOS stellt für außergewöhnliche Notfälle einen kontrollierten Emergency Access bereit:

```text
Emergency
    ↓
Explicit Authorization
    ↓
Limited Temporary Capabilities
    ↓
Emergency Operation
    ↓
Audit
    ↓
Revocation / Expiration
```

Break-Glass ermöglicht notwendige Notfallmaßnahmen, ohne das grundlegende NovaOS-Prinzip eines Systems ohne universelles Root-Privileg aufzugeben.