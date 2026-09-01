# ADR-NETWORK-0006 – Identity, Address und Route strikt trennen

## Status

Angenommen

## Kategorie

Network / Identity / Addressing / Routing / Location Transparency

## Kontext

Klassische Netzwerkarchitekturen koppeln die Identität eines Kommunikationspartners häufig stark an seine aktuelle Netzwerkadresse.

Eine IP-Adresse beschreibt jedoch primär einen erreichbaren Netzwerkort. Sie ist keine dauerhafte Identität eines Geräts, Dienstes oder Kommunikationspartners.

Ein System kann mehrere Interfaces und Adressen besitzen, Adressen können sich ändern und derselbe Service kann über verschiedene Netzwerkpfade erreichbar sein.

NovaOS benötigt deshalb eine klare Trennung zwischen **wer**, **wo** und **wie erreichbar**.

---

## Entscheidung

NovaOS trennt **Identity, Address und Route als eigenständige Netzwerkbegriffe**.

```text
Identity
   ↓
Address Resolution
   ↓
Address
   ↓
Route Resolution
   ↓
Route
```

Dabei gilt:

```text
Identity
→ Wer ist der Kommunikationspartner?

Address
→ Wo ist er aktuell adressierbar?

Route
→ Über welchen Pfad wird er erreicht?
```

Keine dieser Ebenen darf grundsätzlich mit einer anderen gleichgesetzt werden.

---

## Grundprinzip

```text
Identity != Address != Route
```

---

## Network Identity

Eine Network Identity repräsentiert einen logischen Kommunikationspartner.

Dies kann beispielsweise sein:

- ein Service,
- ein Gerät,
- eine Execution Domain,
- ein Remote-System,
- ein autorisierter Provider.

Die Identity soll unabhängig von der aktuell verwendeten IP-Adresse oder Netzwerkverbindung bestehen können.

Sie kann mit Nova.Trust verbunden werden, ist aber nicht automatisch selbst eine Zugriffsberechtigung.

---

## Network Address

Eine Network Address beschreibt einen konkreten adressierbaren Netzwerkort.

Beispiele sind:

```text
IPv6 Address
IPv4 Address
```

Eine Identity kann gleichzeitig mehrere Adressen besitzen:

```text
Identity
 ├── IPv6 Address A
 ├── IPv6 Address B
 └── IPv4 Address C
```

Adressen können hinzugefügt, entfernt oder geändert werden, ohne dadurch zwingend die logische Identity zu verändern.

---

## Route

Eine Route beschreibt den aktuell geeigneten Netzwerkpfad zu einer Adresse beziehungsweise einem Ziel.

```text
Address
   ↓
Route
   ├── Interface
   ├── Gateway / Next Hop
   └── Path Properties
```

Eine Adresse kann über mehrere mögliche Routen erreichbar sein.

Route Selection ist daher eine eigenständige Policy- und Optimierungsentscheidung.

---

## NetworkFlow Integration

`NetworkFlow` verwendet primär eine logische Zielidentität beziehungsweise einen Service Contract.

```text
NetworkFlow
     ↓
Target Identity
     ↓
Address Resolution
     ↓
Eligible Addresses
     ↓
Route Resolution
     ↓
Network Path
```

Dadurch muss die logische Flow Identity nicht verloren gehen, wenn sich Adresse oder Netzwerkpfad ändern.

---

## Multi-Homing und Mobility

Die Trennung unterstützt Systeme mit mehreren Netzwerkverbindungen.

Beispielsweise kann dieselbe Identity gleichzeitig über:

- Ethernet,
- WLAN,
- Mobilfunk,
- weitere Netzwerkprovider

erreichbar sein.

```text
Identity
   ├── Address A → Ethernet
   ├── Address B → WLAN
   └── Address C → Mobile
```

Ein Wechsel des Netzwerkpfads muss nicht automatisch einen Wechsel der logischen Kommunikationsidentität bedeuten.

Ob ein bestehender Flow migriert werden kann, hängt vom verwendeten Transportprovider und dessen Contract ab.

---

## Resolution

Identity Resolution und Route Resolution sind getrennte Vorgänge.

```text
Identity Resolution
Identity → Addresses

Route Resolution
Address → Paths
```

Dadurch können beide Ebenen unabhängig optimiert, gecacht und aktualisiert werden.

Die konkrete Namens-, Discovery- und Routingmechanik wird in NPSPECs beziehungsweise weiteren ADRs festgelegt.

---

## Security und Trust

Identity darf nicht aus einer IP-Adresse allein abgeleitet werden.

```text
IP Address
    ≠
Trusted Identity
```

Authentizität und Trust müssen über geeignete Security- und Nova.Trust-Mechanismen bestimmt werden.

Ebenso erzeugt die Kenntnis einer Identity oder Address keine Communication Authority.

---

## Data Sovereignty und Policy

Route Selection kann durch Hard Constraints eingeschränkt werden.

Beispielsweise können bestimmte:

- Netzwerkprovider,
- Interfaces,
- Regionen,
- Trust Domains,
- Netzwerkpfade

für bestimmte Daten unzulässig sein.

```text
Eligible Routes
      ↓
Hard Constraints
      ↓
Permitted Routes
      ↓
Optimization
```

Eine technisch erreichbare Route ist damit nicht automatisch eine zulässige Route.

---

## Introspection

NovaOS muss die Beziehung zwischen Identity, Address und Route kontrolliert sichtbar machen können.

```text
NetworkFlow
    ↓
Identity
    ↓
Address
    ↓
Route
    ↓
Interface
```

Damit können Routing-, Mobility- und Connectivity-Entscheidungen nachvollzogen werden.

---

## Normative Anforderungen

1. NovaOS MUSS Network Identity, Network Address und Route als getrennte Konzepte behandeln.
2. Network Identity DARF NICHT ausschließlich durch eine IP-Adresse definiert werden.
3. Eine Identity MUSS mehrere gleichzeitig gültige Addresses besitzen können.
4. Addresses MÜSSEN sich ändern können, ohne zwingend die logische Identity zu verändern.
5. Eine Address MUSS über mehrere mögliche Routes erreichbar sein können.
6. Route Selection MUSS unabhängig von Identity Resolution erfolgen können.
7. `NetworkFlow` SOLL eine logische Zielidentität unabhängig vom aktuellen Netzwerkpfad erhalten können.
8. IPv6- und IPv4-Adressen MÜSSEN in dasselbe allgemeine Address-Modell integrierbar sein.
9. Multi-Homing MUSS ohne Vervielfachung der logischen Identity darstellbar sein.
10. Identity DARF NICHT allein aus einer Netzwerkadresse als vertrauenswürdig abgeleitet werden.
11. Identity, Address und Route DÜRFEN jeweils keine implizite Communication Authority erzeugen.
12. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN die zulässigen Addresses und Routes begrenzen können.
13. Route Selection MUSS Hard Constraints vor Optimierungszielen berücksichtigen.
14. Identity-, Address- und Route-Beziehungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- stabile Identitäten trotz wechselnder IP-Adressen,
- bessere Unterstützung für Multi-Homing und Mobility,
- sauberere Location Transparency,
- flexible Route Selection,
- klare Trennung zwischen Erreichbarkeit und Vertrauen,
- bessere Grundlage für zukünftige Netzwerkprotokolle.

### Negative Konsequenzen

- zusätzliche Resolution-Ebenen werden benötigt,
- Identity-, Address- und Route-Lifecycles müssen koordiniert werden,
- Compatibility APIs müssen klassische adressbasierte Modelle auf die native Architektur abbilden.

---

## Verworfene Alternativen

### IP-Adresse als Netzwerkidentität

Verworfen.

Adressen können wechseln, mehrfach vorhanden sein und beweisen keine logische oder vertrauenswürdige Identität.

### Identity direkt an ein Interface binden

Verworfen.

Dies würde Multi-Homing, Mobility und alternative Netzwerkpfade unnötig einschränken.

### Address und Route als ein gemeinsames Objekt behandeln

Verworfen.

Dieselbe Adresse kann abhängig von Topologie und Policy über unterschiedliche Pfade erreichbar sein.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0001_IPv6_als_First_Class_Protokoll`
- `ADR-NETWORK-0002_IPv4_als_vollwertige_Kompatibilität`
- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-IDENTITY-0001`
- `NPSPEC-NETWORK-ADDRESS-0001`
- `NPSPEC-NETWORK-IDENTITY-RESOLUTION-0001`
- `NPSPEC-NETWORK-ROUTE-0001`
- `NPSPEC-NETWORK-ROUTE-SELECTION-0001`
- `NPSPEC-NETWORK-MULTIHOMING-0001`
- `NPSPEC-NETWORK-IDENTITY-ROUTE-TEST-0001`

---

## Ergebnis

NovaOS trennt die drei grundlegenden Fragen der Netzwerkkommunikation:

```text
Identity
   ↓
Who?

Address
   ↓
Where?

Route
   ↓
How?
```

Dadurch können Identitäten stabil bleiben, während sich Adressen und Netzwerkpfade unabhängig verändern und anhand von Security-, Sovereignty-, Locality- und Performance-Anforderungen ausgewählt werden.