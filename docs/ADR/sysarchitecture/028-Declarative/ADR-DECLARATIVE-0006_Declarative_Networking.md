# ADR-DECLARATIVE-0006 – Declarative Networking

## Status

Angenommen

## Kategorie

Declarative Architecture / Networking

## Kontext

Netzwerkkonfiguration wird traditionell häufig durch imperative Einzeloperationen vorgenommen:

```text
Create Interface
Assign Address
Add Route
Configure DNS
Open Port
```

NovaOS soll stattdessen beschreiben, **welcher Netzwerkzustand gelten soll**, ohne die dafür notwendigen Einzelschritte vorzugeben.

## Entscheidung

NovaOS unterstützt **Declarative Networking**.

Netzwerkzustände werden als Desired Network State beschrieben.

```text
Desired Network State
        ↓
Network Reconciliation
        ↓
Actual Network State
        ↓
Verification
```

## Desired Network State

Deklarative Netzwerkdefinitionen können beispielsweise enthalten:

```text
Connectivity
Addresses
Routes
DNS
Firewall Policy
Required Services
Network Capabilities
Locality
```

Die Definition beschreibt das Ziel und nicht die konkrete Befehlsfolge.

## Actual Network State

NovaOS beobachtet den tatsächlich vorhandenen Netzwerkzustand.

```text
Desired:
    Connectivity = Online

Actual:
    Connectivity = Offline
```

Abweichungen werden dem Reconciliation-System bereitgestellt.

## Reconciliation

```text
Observe Network
      ↓
Compare
      ↓
Plan
      ↓
Execute
      ↓
Verify
```

Der Reconciler bestimmt die notwendigen Änderungen anhand von Contracts, Policies und verfügbaren Netzwerkressourcen.

## Netzwerkressourcen

Netzwerkkomponenten werden als deklarative Ressourcen behandelt.

Beispiele:

```text
Interface
Address
Route
Endpoint
Network Service
Firewall Policy
```

Die konkrete Hardware oder Implementierung muss nicht Bestandteil des Desired State sein, sofern dies nicht ausdrücklich erforderlich ist.

## Capabilities

Netzwerkanforderungen sollen bevorzugt über Capabilities beschrieben werden.

```text
Requires:
    Network.IPv6
    Network.DNS
    Network.HTTP
```

NovaOS kann geeignete Provider anhand der verfügbaren Ressourcen bestimmen.

## Security

Declarative Networking muss Declarative Security berücksichtigen.

```text
Network Desired State
        +
Security Desired State
        ↓
Valid Network Plan
```

Eine Netzwerkanforderung darf keine geltende Security- oder Data-Sovereignty-Policy umgehen.

## Dynamische Netzwerke

Der Actual State kann sich durch externe Ereignisse ändern:

```text
Link Lost
Wi-Fi Changed
Address Changed
Route Lost
```

NovaOS kann darauf durch erneute Reconciliation reagieren.

## Verification

Eine Netzwerkänderung gilt erst nach Überprüfung des tatsächlichen Zustands als erfolgreich.

```text
Apply
  ↓
Observe
  ↓
Verify
```

Ein erfolgreicher Konfigurationsaufruf allein reicht nicht aus.

## Normative Anforderungen

1. NovaOS MUSS Netzwerkzustände deklarativ beschreiben können.
2. Desired Network State und Actual Network State MÜSSEN getrennt behandelt werden.
3. Netzwerkressourcen MÜSSEN durch State Reconciliation verwaltbar sein.
4. Netzwerkabhängigkeiten SOLLEN über Capabilities beschreibbar sein.
5. Security- und Data-Sovereignty-Policies MÜSSEN bei der Netzwerkplanung berücksichtigt werden.
6. Änderungen des Actual Network State MÜSSEN eine erneute Reconciliation auslösen können.
7. Netzwerkänderungen MÜSSEN nach ihrer Ausführung verifizierbar sein.
8. Ein Desired Network State DARF erst nach Verification als erreicht gelten.
9. Die deklarative Netzwerkdefinition SOLL von konkreter Hardware unabhängig bleiben.

## Konsequenzen

### Positive Konsequenzen

- Netzwerke werden über Zielzustände statt Befehlsfolgen konfiguriert,
- Hardware und Netzwerkprovider bleiben austauschbar,
- Netzwerkänderungen können automatisch ausgeglichen werden,
- Security und Networking können gemeinsam geplant werden.

### Negative Konsequenzen

- Netzwerkzustände müssen zuverlässig beobachtet werden,
- dynamische Netzwerkänderungen können häufige Reconciliation auslösen.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-DECLARATIVE-0003_Declarative_Resources`
- `ADR-DECLARATIVE-0005_Declarative_Security`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-ARCH-0007_Location_Transparency`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-NETWORK-0001`
- `NPSPEC-DECLARATIVE-NETWORK-STATE-0001`
- `NPSPEC-DECLARATIVE-NETWORK-RECONCILIATION-0001`

## Ergebnis

NovaOS beschreibt Netzwerke über ihren gewünschten Zustand:

```text
Desired Network State
        ↓
Observe
        ↓
Reconcile
        ↓
Apply
        ↓
Verify
        ↓
Actual Network State
```

Der zentrale Grundsatz lautet:

```text
Nicht:
Konfiguriere dieses Netzwerk so.

Sondern:
Dieser Netzwerkzustand
soll gelten.
``` 