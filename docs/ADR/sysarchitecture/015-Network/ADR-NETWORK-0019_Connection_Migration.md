# ADR-NETWORK-0019 – Connection Migration

## Status

Angenommen

## Kategorie

Network / Connection Migration / Mobility / NetworkFlow / Multipath

## Kontext

NovaOS trennt Network Identity, Address und Route voneinander und verwendet `NetworkFlow` als stabile logische Kommunikationsabstraktion.

Während eines laufenden Flows können sich jedoch die zugrunde liegenden Netzwerkbedingungen ändern. Ein Gerät kann beispielsweise zwischen WLAN und Mobilfunk wechseln, eine IP-Adresse verlieren, einen besseren Netzwerkpfad erhalten oder innerhalb eines Overlay-Netzwerks seinen Standort verändern.

Solche Änderungen sollen nicht zwangsläufig zum Verlust der logischen Verbindung führen.

---

## Entscheidung

NovaOS unterstützt **Connection Migration als native Eigenschaft geeigneter `NetworkFlow`- und Transportprovider**.

```text
NetworkFlow
     ↓
Active Connection
     ↓
Path / Address Change
     ↓
Migration
     ↓
New Path / Address
     ↓
Same Logical NetworkFlow
```

Die logische Identität eines `NetworkFlow` bleibt dabei von der konkreten Adresse, Route und Netzwerkschnittstelle getrennt.

Connection Migration ist eine Fähigkeit des jeweiligen Providers und darf nicht für Transportmechanismen vorgetäuscht werden, die sie nicht zuverlässig unterstützen.

---

## Grundprinzip

```text
Connection identity
    ≠
Network path
```

Ein Pfadwechsel soll eine bestehende logische Kommunikation erhalten können, ohne deren Security-, Authority- oder Contract-Eigenschaften neu zu definieren.

---

## Stabile Flow Identity

Ein migrierbarer `NetworkFlow` besitzt eine stabile logische Identität.

```text
NetworkFlow
   ├── Old Path
   └── New Path
```

Änderungen an:

- IP-Adresse,
- Interface,
- Route,
- Provider,
- physischem Netzwerk

dürfen deshalb nicht automatisch einen neuen logischen Flow erzeugen.

---

## Providerfähigkeit

Transportprovider deklarieren explizit, ob und unter welchen Bedingungen sie Connection Migration unterstützen.

```text
NetworkFlow
     ↓
Transport Provider
     ↓
Migration Capability
```

QUIC oder zukünftige Transportprovider können native Migration unterstützen.

Bei Transporten ohne entsprechende Semantik kann NovaOS lediglich einen neuen Flow aufbauen oder eine höherliegende Wiederaufnahme verwenden.

Die Unterschiede dürfen gegenüber höheren Schichten nicht als falsche Garantie verborgen werden.

---

## Migration Trigger

Eine Migration kann durch Änderungen der Netzwerkumgebung oder durch Policy ausgelöst werden.

Dazu gehören insbesondere:

- Verlust eines Netzwerkpfads,
- Interface-Wechsel,
- Adressänderung,
- Mobility,
- Policy-Based Routing,
- bessere verfügbare Route,
- Overlay-/VPN-Änderungen.

Adaptive Optimierung darf Migration als Soft-Policy verwenden, sofern keine Hard Constraints verletzt werden.

---

## Hard Constraints

Vor einer Migration muss der neue Pfad alle relevanten Hard Constraints erfüllen.

```text
Migration Candidate
       ↓
Security
Data Sovereignty
Authority
Transport Requirements
Network Policy
       ↓
Eligible Path
```

Ein technisch erreichbarer Pfad ist nicht automatisch ein zulässiger Migrationspfad.

Insbesondere darf eine Verbindung nicht aus Performancegründen auf einen Pfad migriert werden, der Security- oder Data-Sovereignty-Anforderungen verletzt.

---

## Security Continuity

Connection Migration darf die bestehende Sicherheitsidentität einer Verbindung nicht allein aufgrund einer neuen Netzwerkadresse verändern.

```text
Authenticated Peer
       ↓
Path Migration
       ↓
Same Authenticated Peer
```

Der neue Pfad muss jedoch entsprechend den Anforderungen des verwendeten Transport- und Security-Providers validiert werden.

Adressänderung ist weder neue Identität noch ausreichender Identitätsnachweis.

---

## Multipath Integration

Connection Migration und Multipath Networking verwenden dieselbe Path-Abstraktion.

```text
NetworkFlow
   ├── Path A
   └── Path B
```

Multipath kann mehrere Pfade gleichzeitig verwenden.

Connection Migration kann dagegen einen Flow von einem bisherigen auf einen anderen Pfad verlagern.

Beide Mechanismen dürfen miteinander kombiniert werden.

---

## Overlay und VPN

Migration muss mit Overlay- und VPN-Netzwerken koordinierbar sein.

Dabei können sich unabhängig voneinander ändern:

- Overlay-Pfad,
- Underlay-Pfad,
- physisches Interface,
- externe Adresse.

Ein verpflichtendes Overlay oder VPN darf durch Connection Migration nicht umgangen werden.

---

## QoS und Resource Economy

Ein neuer Pfad muss weiterhin die geltenden Ressourcen- und QoS-Anforderungen berücksichtigen.

Migration darf verwendet werden, um beispielsweise:

- Latenz zu reduzieren,
- Überlastung zu vermeiden,
- Energieverbrauch zu optimieren,
- Connectivity zu erhalten.

Hard Resource Limits und verbindliche Policies bleiben dabei bestehen.

---

## Graceful Migration

Wenn möglich, soll eine Migration kontrolliert erfolgen, bevor der bisherige Pfad aufgegeben wird.

```text
Old Path
    ↓
Validate New Path
    ↓
Migrate
    ↓
Retire Old Path
```

Dadurch können unnötige Unterbrechungen reduziert werden.

Ist der alte Pfad bereits ausgefallen, darf ein geeigneter Provider eine direkte Recovery-Migration versuchen.

Die konkreten Verfahren werden in NPSPECs definiert.

---

## Fallback

Nicht jeder Flow ist migrierbar.

```text
Path Change
    ↓
Migration supported?
 ├── Yes → Migrate
 └── No  → Reconnect / Fail
```

Wenn Migration nicht möglich ist, gelten die normalen Recovery-, Retry- oder Fehlerregeln des jeweiligen Contracts.

Connection Migration darf keine unterbrechungsfreie Kommunikation garantieren, wenn Transport oder Gegenstelle dies nicht unterstützen.

---

## Deterministic Mode

Im Deterministic Mode müssen automatische Migrationen kontrollierbar sein.

Der Contract kann beispielsweise festlegen:

- erlaubte Pfade,
- erlaubte Provider,
- Migration nur bei Ausfall,
- keine optimierungsgetriebene Migration.

Damit bleibt reproduzierbares Netzwerkverhalten möglich.

---

## Introspection

Migrationen müssen kontrolliert introspektierbar sein.

NovaOS soll insbesondere nachvollziehen können:

```text
NetworkFlow
     ↓
Old Path
     ↓
Migration Reason
     ↓
New Path
     ↓
Migration Result
```

Dies ermöglicht Diagnose von Mobility-, Connectivity- und Performanceproblemen.

---

## Normative Anforderungen

1. NovaOS MUSS Connection Migration für geeignete Transportprovider unterstützen können.
2. Die logische `NetworkFlow`-Identität MUSS von Adresse, Route und Interface getrennt bleiben.
3. Transportprovider MÜSSEN ihre Migration Capability explizit deklarieren.
4. NovaOS DARF Migration NICHT für Provider garantieren, die diese Semantik nicht unterstützen.
5. Neue Migrationspfade MÜSSEN alle geltenden Hard Constraints erfüllen.
6. Connection Migration DARF Security-, Capability- oder Data-Sovereignty-Policies NICHT umgehen.
7. Eine Adressänderung DARF NICHT automatisch als Änderung der Peer Identity behandelt werden.
8. Der neue Pfad MUSS gemäß Transport- und Security-Contract validiert werden.
9. Connection Migration MUSS mit Multipath Networking koordinierbar sein.
10. Connection Migration MUSS mit Overlay- und VPN-Netzwerken kombinierbar sein.
11. QoS- und Resource-Budget-Anforderungen MÜSSEN auch nach einer Migration gelten.
12. Adaptive Migration DARF nur innerhalb zulässiger Soft Policies erfolgen.
13. NovaOS MUSS mit nicht migrierbaren Flows korrekt umgehen können.
14. Deterministic Mode MUSS automatische Migrationen einschränken oder kontrollieren können.
15. Migrationen und ihre Ursachen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- laufende Verbindungen können Netzwerkwechsel überstehen,
- Mobility wird Bestandteil der nativen Netzwerkarchitektur,
- IP-Adresse und Interface werden nicht mit Connection Identity verwechselt,
- Multipath, VPN und Policy-Based Routing können mit Migration kombiniert werden,
- adaptive Pfadoptimierung wird ohne Änderung der logischen Flow Identity möglich.

### Negative Konsequenzen

- nicht alle Transportprotokolle unterstützen echte Connection Migration,
- Security- und Path-Validation werden komplexer,
- Migration kann kurzfristig zusätzliche Netzwerk- und Ressourcenlast erzeugen.

---

## Verworfene Alternativen

### Jede Adressänderung beendet die Verbindung

Verworfen.

Dies koppelt die logische Kommunikation unnötig an einen konkreten Netzwerkpfad und erschwert Mobility.

### Connection Migration ausschließlich in Anwendungen

Verworfen.

Dadurch würden Anwendungen Transport-, Routing-, Mobility- und Security-Mechanismen selbst implementieren müssen.

### Migration ohne erneute Pfadvalidierung

Verworfen.

Ein neuer Netzwerkpfad muss weiterhin Security-, Policy- und Data-Sovereignty-Anforderungen erfüllen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0004_TCP_UDP_QUIC_als_Transportprovider`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0014_Native_Overlay_VPN_Architektur`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`
- `ADR-NETWORK-0016_Adaptive_Congestion_Control`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0015_Graceful_Degradation`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-CONNECTION-MIGRATION-0001`
- `NPSPEC-NETWORK-MIGRATION-PATH-0001`
- `NPSPEC-NETWORK-MIGRATION-SECURITY-0001`
- `NPSPEC-NETWORK-MIGRATION-MULTIPATH-0001`
- `NPSPEC-NETWORK-MIGRATION-OVERLAY-0001`
- `NPSPEC-NETWORK-MIGRATION-RECOVERY-0001`
- `NPSPEC-NETWORK-CONNECTION-MIGRATION-TEST-0001`

---

## Ergebnis

NovaOS entkoppelt die Lebensdauer einer logischen Netzwerkverbindung von ihrem aktuellen Netzwerkpfad:

```text
Stable NetworkFlow
       ↓
Current Path
       ↓
Network Change
       ↓
Policy + Validation
       ↓
Connection Migration
       ↓
New Path
```

Damit können geeignete Verbindungen Adress-, Interface- und Pfadwechsel überstehen, während Identity, Security, Data Sovereignty und die ursprünglichen Flow Contracts erhalten bleiben.