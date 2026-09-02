# ADR-SECURITY-0018 – Fail Closed für Sicherheitsentscheidungen

## Status

Angenommen

## Kategorie

Security / Authorization / Fail-Safe Defaults

## Kontext

Sicherheitsentscheidungen können aufgrund fehlender Informationen, nicht erreichbarer Policy-Dienste, ungültiger Capabilities, unbekannter Trust-Zustände oder interner Fehler nicht eindeutig getroffen werden.

Ein solches Problem darf nicht dazu führen, dass eine geschützte Operation automatisch erlaubt wird.

NovaOS benötigt deshalb eine einheitliche Fail-Closed-Semantik für sicherheitsrelevante Entscheidungen.

---

## Entscheidung

NovaOS verwendet **Fail Closed als Standardverhalten für Sicherheitsentscheidungen**.

```text
Security Decision
      ↓
 ┌────┼─────┐
 ↓    ↓     ↓
Allow Deny Unknown/Error
 ↓    ↓     ↓
Allow Deny  Deny
```

Eine Operation wird nur ausgeführt, wenn die erforderliche Autorisierung positiv festgestellt wurde.

---

## Grundprinzip

```text
Unknown ≠ Allowed
```

und:

```text
No verified authorization
        =
No operation
```

---

## Geltungsbereich

Fail Closed gilt insbesondere für:

- Capability Validation,
- Capability Delegation und Revocation,
- ABAC- und MAC-Entscheidungen,
- Sandbox-Grenzen,
- Code Integrity und Trust,
- Information Flow Control,
- Data Sovereignty,
- Break-Glass Authorization,
- sicherheitskritische Geräte- und Systemzugriffe.

---

## Fehlerzustände

Fail Closed greift unter anderem bei:

```text
Missing Authority
Invalid Authority
Expired Authority
Revoked Authority
Unknown Authority
Policy Evaluation Failure
Missing Security Attribute
Trust Validation Failure
Integrity Validation Failure
```

Die konkrete Fehlerklassifikation wird in NPSPECs definiert.

---

## Keine stille Degradation

NovaOS darf eine Sicherheitsanforderung nicht stillschweigend abschwächen, nur um eine Operation fortsetzen zu können.

Beispiel:

```text
Required Secure Policy unavailable
        ↓
NOT: weaker policy
        ↓
Deny / Wait / Revalidate
```

Graceful Degradation darf nur innerhalb weiterhin erfüllter Hard Constraints stattfinden.

---

## Verfügbarkeit und Sicherheit

Fail Closed kann dazu führen, dass eine Operation bei Ausfall einer Sicherheitskomponente nicht verfügbar ist.

Für harte Sicherheitsentscheidungen gilt:

```text
Security
   >
Availability
```

Wo hohe Verfügbarkeit erforderlich ist, muss die Sicherheitsarchitektur selbst redundant oder lokal auswertbar gestaltet werden, anstatt bei Ausfall die Sicherheitsprüfung zu umgehen.

---

## Bestehende Operationen

Fail Closed bedeutet nicht automatisch, dass jede bereits laufende Operation sofort beendet werden muss.

Das Verhalten hängt vom jeweiligen Contract und Sicherheitszustand ab.

NovaOS muss zwischen:

```text
New Authorization
Existing Authorized Operation
Revoked Authority
Unknown Security State
```

unterscheiden können.

Die konkreten Lifecycle-Regeln werden in NPSPECs definiert.

---

## Recovery und Break-Glass

Recovery- oder Break-Glass-Pfade sind keine Ausnahme vom Fail-Closed-Prinzip.

Sie besitzen eigene explizite Autorisierungsregeln.

```text
Normal Authorization failed
        ↓
Break-Glass Authorization
        ↓
Explicitly verified?
   ├── Yes → Emergency Authority
   └── No  → Deny
```

---

## Adaptive Systeme

Adaptive oder KI-basierte Systeme dürfen einen unbekannten Sicherheitszustand nicht als wahrscheinlich sicher interpretieren.

```text
Prediction
    ≠
Authorization
```

Lern- und Vorhersagemodelle dürfen Fail Closed nicht umgehen.

---

## Audit und Introspection

Fail-Closed-Entscheidungen müssen nachvollziehbar sein.

NovaOS soll unterscheiden können zwischen:

```text
Explicit Deny
Missing Authority
Unknown State
Validation Failure
Policy Failure
Revocation
```

Damit bleiben Sicherheitsfehler diagnostizierbar, ohne sie permissiv behandeln zu müssen.

---

## Normative Anforderungen

1. NovaOS MUSS für Sicherheitsentscheidungen standardmäßig Fail Closed verwenden.
2. Eine geschützte Operation DARF nur bei positiv festgestellter Autorisierung ausgeführt werden.
3. `Unknown` DARF NICHT als `Allowed` interpretiert werden.
4. Fehlende, ungültige, abgelaufene oder widerrufene Authority MUSS zur Ablehnung führen.
5. Fehler bei erforderlicher Policy-Auswertung MÜSSEN fail-safe behandelt werden.
6. Fehlende sicherheitskritische Attribute DÜRFEN NICHT permissiv interpretiert werden.
7. Security-, Trust-, Integrity-, Sovereignty- und Information-Flow-Prüfungen DÜRFEN bei Fehlern NICHT stillschweigend übersprungen werden.
8. Graceful Degradation DARF Hard Security Constraints NICHT abschwächen.
9. Verfügbarkeitsanforderungen DÜRFEN keine implizite Security-Umgehung erzeugen.
10. Break-Glass MUSS weiterhin explizit autorisiert werden.
11. Adaptive Systeme DÜRFEN Fail-Closed-Entscheidungen NICHT überschreiben.
12. Fail-Closed-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Sicherheitsfehler führen nicht zu unbeabsichtigter Authority,
- einheitliches Verhalten bei unbekannten Zuständen,
- keine stillen Security-Downgrades,
- robuste Grundlage für Capabilities, MAC, ABAC und Data Sovereignty.

### Negative Konsequenzen

- Fehler in Sicherheitsdiensten können Operationen blockieren,
- hohe Verfügbarkeit erfordert robuste oder redundante Policy-Infrastruktur,
- fehlerhafte Policies können legitime Zugriffe verhindern.

---

## Verworfene Alternativen

### Fail Open

Verworfen.

Ein Fehler würde sonst möglicherweise zusätzliche Authority erzeugen.

### Fail Open für Verfügbarkeit

Verworfen.

Verfügbarkeit darf harte Sicherheitsgrenzen nicht automatisch außer Kraft setzen.

### KI-basierte Entscheidung bei unbekanntem Zustand

Verworfen.

Eine Vorhersage ist kein belastbarer Autorisierungsnachweis.

---

## Abhängigkeiten

- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`
- `ADR-SECURITY-0010_Capability_Revocation`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-SECURITY-0017_Break_Glass_Access_mit_Audit`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-FAIL-CLOSED-0001`
- `NPSPEC-SECURITY-AUTHORIZATION-FAILURE-0001`
- `NPSPEC-SECURITY-SECURITY-STATE-0001`
- `NPSPEC-SECURITY-FAIL-CLOSED-TEST-0001`

---

## Ergebnis

NovaOS erlaubt sicherheitskritische Operationen nur bei eindeutig bestätigter Autorisierung.

```text
Verified Allow → Execute

Deny
Unknown
Error
Invalid
Unavailable
    → Do Not Execute
```

Damit gilt systemweit:

```text
Unknown ≠ Allowed
```

Fehler oder Unsicherheit in der Sicherheitsarchitektur können dadurch niemals automatisch zusätzliche Authority erzeugen.