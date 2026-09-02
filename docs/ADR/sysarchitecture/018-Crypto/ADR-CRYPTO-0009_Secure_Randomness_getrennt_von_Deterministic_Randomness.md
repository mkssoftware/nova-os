# ADR-CRYPTO-0009 – Secure Randomness getrennt von Deterministic Randomness

## Status

Angenommen

## Kategorie

Cryptography / Randomness / Determinism / Security

## Kontext

NovaOS benötigt Zufallswerte für sehr unterschiedliche Zwecke.

Sicherheitskritische Operationen benötigen kryptographisch sichere, nicht vorhersagbare Zufallswerte, beispielsweise für:

- Schlüsselgenerierung,
- Nonces,
- Tokens,
- Session Secrets,
- Challenges,
- Capability Secrets.

Andere Bereiche benötigen dagegen bewusst reproduzierbare Zufallsfolgen, beispielsweise für:

- Tests,
- Simulationen,
- reproduzierbare Builds,
- Deterministic Mode,
- Debugging,
- wissenschaftliche Berechnungen.

Beide Anforderungen dürfen nicht über dieselbe implizite Randomness-Abstraktion vermischt werden.

---

## Entscheidung

NovaOS trennt **Secure Randomness und Deterministic Randomness als unterschiedliche semantische Systemdienste**.

```text
Randomness
 ├── Secure Randomness
 │      → unpredictable
 │
 └── Deterministic Randomness
        → reproducible
```

Eine API muss eindeutig ausdrücken, welche Art von Randomness benötigt wird.

---

## Grundprinzip

```text
Unpredictable randomness
        ≠
Reproducible randomness
```

Deterministic Randomness darf niemals implizit als kryptographisch sichere Randomness verwendet werden.

---

## Secure Randomness

Secure Randomness wird für sicherheitskritische Zufallswerte verwendet.

```text
Hardware Entropy
      +
System Entropy
      ↓
Secure Random Generator
      ↓
Cryptographic Randomness
```

Der Generator muss kryptographisch geeignete Eigenschaften besitzen und über die Crypto-Architektur bereitgestellt werden.

Die konkrete Konstruktion wird in NPSPECs definiert.

---

## Entropy Architecture

NovaOS sammelt geeignete Entropie aus verfügbaren und vertrauenswürdigen Quellen.

Mögliche Quellen sind:

```text
Hardware RNG
Platform Security Hardware
Interrupt Timing
Device Events
Platform-specific Entropy Sources
```

Keine einzelne Hardwarequelle muss grundsätzlich alleiniger Vertrauensanker sein.

Entropy Sources müssen entsprechend ihrer Qualität und Trust-Eigenschaften behandelt werden können.

---

## Deterministic Randomness

Deterministic Randomness erzeugt reproduzierbare Folgen aus einem expliziten Seed.

```text
Seed
  ↓
Deterministic Generator
  ↓
R1 → R2 → R3 → ...
```

Identischer Algorithmus, Seed und Kontext sollen im definierten Deterministic Mode reproduzierbare Ergebnisse ermöglichen.

---

## Explizite Typtrennung

Secure und Deterministic Randomness sollen über unterschiedliche APIs beziehungsweise Semantic Types repräsentiert werden.

Beispiel:

```text
SecureRandom
DeterministicRandom
```

Dadurch soll eine versehentliche Verwendung eines deterministischen Generators für kryptographische Operationen bereits durch API- und Typsystem verhindert oder deutlich erschwert werden.

---

## Deterministic Mode

Der systemweite Deterministic Mode darf Secure Randomness nicht automatisch deterministisch machen.

```text
Deterministic Mode
      ↓
Deterministic Workloads
      ↓
DeterministicRandom
```

Sicherheitskritische Schlüssel, Tokens oder Secrets bleiben weiterhin von Secure Randomness abhängig.

Falls vollständige Reproduzierbarkeit mit echter Secure Randomness unvereinbar ist, muss dieser Konflikt explizit behandelt werden.

---

## Seed Management

Seeds für Deterministic Randomness sind Teil des jeweiligen Execution Context.

Sie müssen explizit:

- gesetzt,
- abgeleitet,
- weitergegeben oder
- aufgezeichnet

werden können, wenn Reproduzierbarkeit erforderlich ist.

Ein Seed darf nicht automatisch als kryptographisches Secret betrachtet werden.

---

## Secure Randomness und Schlüsselgenerierung

Kryptographische Schlüssel und andere sicherheitskritische Secrets müssen Secure Randomness verwenden, sofern das jeweilige kryptographische Verfahren Randomness benötigt.

```text
Key Generation
      ↓
SecureRandom
```

Ein `DeterministicRandom` darf hierfür nur verwendet werden, wenn ein ausdrücklich definiertes kryptographisches Verfahren deterministische Generierung vorsieht und dessen Security Contract dies erlaubt.

---

## Fehlerverhalten

Steht keine ausreichend sichere Randomness zur Verfügung, darf NovaOS keine schwachen oder vorhersehbaren Ersatzwerte erzeugen.

```text
Secure Randomness Required
          ↓
Insufficient Entropy / Generator Failure
          ↓
Fail / Wait
```

Ein automatischer Fallback auf Deterministic Randomness ist verboten.

---

## Normative Anforderungen

1. NovaOS MUSS Secure Randomness und Deterministic Randomness als getrennte Abstraktionen behandeln.
2. Secure Randomness MUSS für kryptographisch sicherheitskritische Zufallswerte verfügbar sein.
3. Deterministic Randomness MUSS explizit seedbar und reproduzierbar sein können.
4. Beide Randomness-Arten SOLLEN durch unterschiedliche APIs oder Semantic Types getrennt werden.
5. Deterministic Randomness DARF NICHT implizit als Secure Randomness verwendet werden.
6. Der Deterministic Mode DARF Secure Randomness NICHT automatisch deterministisch machen.
7. Secure Randomness MUSS geeignete Entropy Sources verwenden.
8. NovaOS DARF NICHT grundsätzlich von einer einzelnen Hardware-Entropy-Quelle abhängig sein.
9. Entropy Sources MÜSSEN hinsichtlich Qualität und Trust bewertbar sein.
10. Kryptographische Schlüsselgenerierung MUSS geeignete Secure Randomness verwenden, sofern der Crypto Contract nichts anderes definiert.
11. Seeds für Deterministic Randomness MÜSSEN explizit kontrollierbar sein.
12. Fehlende Secure Randomness DARF NICHT durch schwache oder deterministische Ersatzwerte kompensiert werden.
13. Fehler der Secure-Randomness-Infrastruktur MÜSSEN fail-closed behandelt werden.
14. Randomness-Provider und relevante Gesundheitszustände MÜSSEN kontrolliert introspektierbar sein, ohne geheime Zustände offenzulegen.

---

## Konsequenzen

### Positive Konsequenzen

- klare Trennung zwischen Sicherheit und Reproduzierbarkeit,
- geringeres Risiko versehentlich vorhersehbarer kryptographischer Werte,
- saubere Integration des Deterministic Mode,
- reproduzierbare Tests und Simulationen,
- austauschbare Entropy- und Randomness-Provider.

### Negative Konsequenzen

- zwei getrennte Randomness-Abstraktionen müssen gepflegt werden,
- vollständige Systemreproduzierbarkeit kann bei sicherheitskritischer Randomness bewusst begrenzt sein,
- Entropy Management wird zu einer sicherheitskritischen Systemfunktion.

---

## Verworfene Alternativen

### Eine universelle Random-API

Verworfen.

Sie würde sicherheitskritische und reproduzierbare Randomness semantisch vermischen.

### Deterministischer System-RNG im Deterministic Mode

Verworfen.

Dadurch könnten kryptographische Schlüssel, Tokens oder Secrets vorhersehbar werden.

### Hardware-RNG als einzige Entropiequelle

Verworfen.

NovaOS soll nicht von der Sicherheit und Verfügbarkeit einer einzelnen Entropiequelle abhängig sein.

### Fallback auf PRNG bei Entropiefehlern

Verworfen.

Ein Sicherheitsfehler darf nicht durch vorhersehbare Randomness verborgen werden.

---

## Abhängigkeiten

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CRYPTO-0001_Crypto_Agility`
- `ADR-CRYPTO-0004_Hierarchische_Key_Architecture`
- `ADR-CRYPTO-0007_Authenticated_Encryption_für_schützenswerte_Objekte`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-SECURITY-0020_Hardware_Backend_für_schützenswerte_Schlüssel`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-SECURE-RANDOM-0001`
- `NPSPEC-CRYPTO-DETERMINISTIC-RANDOM-0001`
- `NPSPEC-CRYPTO-ENTROPY-POOL-0001`
- `NPSPEC-CRYPTO-ENTROPY-SOURCE-0001`
- `NPSPEC-CRYPTO-RANDOM-HEALTH-0001`
- `NPSPEC-CRYPTO-DETERMINISTIC-SEED-0001`
- `NPSPEC-CRYPTO-RANDOM-TEST-0001`

---

## Ergebnis

NovaOS trennt kryptographische Unvorhersagbarkeit konsequent von reproduzierbarer Pseudozufälligkeit:

```text
                 Randomness
                 /        \
                ↓          ↓
        SecureRandom   DeterministicRandom
             ↓               ↓
      Unpredictability   Reproducibility
             ↓               ↓
        Security       Tests / Simulation /
                       Deterministic Mode
```

Damit kann NovaOS gleichzeitig sichere kryptographische Zufallswerte und reproduzierbare Ausführungen bereitstellen, ohne beide Sicherheitsmodelle miteinander zu vermischen.