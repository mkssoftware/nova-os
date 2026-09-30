# NPSPEC-CAPSULE-0005 – Capsule Security & Identity

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Sicherheits- und Identitätsanforderungen für `Nova.TaskCapsule`.

Ziel ist, dass eine TaskCapsule eindeutig identifizierbar, auf Integrität und Herkunft prüfbar und sicher zwischen Speicherorten oder Systemen übertragen werden kann, ohne bestehende Berechtigungsgrenzen zu umgehen.

## Grundprinzip

```text
TaskCapsule
    ↓
Identity
    +
Integrity
    +
Authenticity
    +
Policy Context
    ↓
Verification
    ↓
Trusted Resume
```

Eine TaskCapsule transportiert Zustand und Kontext, aber keine automatisch gültigen Berechtigungen.

## Capsule Identity

Jede TaskCapsule muss eine eindeutige Identität besitzen.

Beispiel:

```text
capsule:
    01K...
```

Die Identität bezeichnet die logische TaskCapsule und ist unabhängig von:

```text
filename
storage_location
device
process
```

## Instance Identity

Von einer Capsule dürfen Kopien oder abgeleitete Instanzen entstehen.

Dabei muss zwischen:

```text
logical_capsule_id
instance_id
```

unterschieden werden können.

Beispiel:

```text
logical_capsule:
    capsule:42

instance:
    capsule-instance:91
```

Dadurch bleiben Herkunft und Ableitungen nachvollziehbar.

## Owner Identity

Eine Capsule darf einen logischen Besitzer oder Ursprungskontext referenzieren.

Beispiel:

```text
owner {
    identity:
        user:1234
}
```

Diese Information dient der Zuordnung.

Sie ersetzt keine aktuelle Authentifizierung.

## Origin

Die Herkunft einer Capsule muss beschreibbar sein.

Beispiel:

```text
origin {
    system:
        device:workstation-01

    created_by:
        Nova.TaskCapsule

    created_at:
        ...
}
```

Herkunftsinformationen dürfen durch kryptografische Nachweise abgesichert werden.

## Integrität

NovaOS muss erkennen können, ob eine Capsule nach ihrer Erstellung verändert oder beschädigt wurde.

Mindestens prüfbar sein müssen:

```text
manifest
task_state
embedded_resources
dependency_information
security_metadata
```

Mögliche Mechanismen:

```text
hashes
Merkle structures
digital signatures
authenticated metadata
```

## Signaturen

Eine Capsule darf digital signiert werden.

Beispiel:

```text
signature {
    signer:
        system_identity

    algorithm:
        ...

    value:
        ...
}
```

Eine gültige Signatur bestätigt die Integrität und die kryptografisch nachweisbare Herkunft des signierten Inhalts.

Sie bedeutet nicht automatisch, dass der Inhalt vertrauenswürdig oder zur Ausführung autorisiert ist.

## Trust

Trust muss getrennt von Identität behandelt werden.

Beispiel:

```text
identity:
    known

signature:
    valid

trust:
    untrusted
```

Eine korrekt signierte Capsule darf weiterhin als nicht vertrauenswürdig behandelt werden.

## Security Context

Die Capsule darf sicherheitsrelevanten Kontext transportieren.

Beispiele:

```text
information_labels
ownership
origin
required_permissions
trust_requirements
policy_references
```

Dieser Kontext beschreibt Anforderungen.

Er gewährt selbst keine Rechte.

## Keine Berechtigungsübertragung

Eine gespeicherte Berechtigung darf nicht automatisch auf ein anderes System oder eine neue Sitzung übertragen werden.

Beispiel:

```text
Original System:
    camera access allowed
```

Nach Migration:

```text
Target System:
    camera access must be revalidated
```

Dasselbe gilt unter anderem für:

```text
filesystem permissions
network access
device access
credentials
external services
```

## Credentials

Persistente Zugangsdaten sollen nicht direkt in einer TaskCapsule gespeichert werden.

Stattdessen sollen sichere Referenzen verwendet werden.

Beispiel:

```text
credential_reference:
    credential:vault:42
```

Beim Resume wird die Berechtigung erneut geprüft.

Falls Secrets eingebettet werden müssen, benötigen sie einen gesondert geschützten Speicherbereich.

## Verschlüsselung

Eine TaskCapsule oder einzelne Bestandteile dürfen verschlüsselt werden.

Beispiel:

```text
encryption {
    scope:
        resources

    key_reference:
        ...
}
```

Verschlüsselung darf auf unterschiedlichen Ebenen erfolgen:

```text
WHOLE_CAPSULE
METADATA
RESOURCE
STATE
```

Die Schlüssel selbst sollen nicht ungeschützt gemeinsam mit den verschlüsselten Daten gespeichert werden.

## Information Flow

Persistente Information-Flow-Labels müssen beim Packaging erhalten bleiben.

Beispiel:

```text
resource {
    label:
        confidential
}
```

Beim Resume dürfen die Labels nicht ohne autorisierte Declassification entfernt werden.

## Manipulationserkennung

Veränderungen müssen mindestens folgende Zustände erzeugen können:

```text
VALID
MODIFIED
CORRUPTED
UNVERIFIABLE
```

Eine nicht verifizierbare Capsule darf nicht automatisch wie eine vollständig verifizierte Capsule behandelt werden.

## Replay

NovaOS muss verhindern können, dass sicherheitskritische einmalige Aktionen durch das erneute Laden einer Capsule unbeabsichtigt wiederholt werden.

Beispiel:

```text
Payment completed
    ↓
Capsule restored
    ↓
Payment must NOT automatically execute again
```

Hierfür können verwendet werden:

```text
Causality
external_effect_state
transaction_identity
idempotency_token
```

## Forks

Wird eine Capsule dupliziert und anschließend unabhängig fortgesetzt, entstehen logische Branches.

Beispiel:

```text
Capsule A
    ├── Branch B
    └── Branch C
```

Diese müssen voneinander unterscheidbar sein.

Eine Branch-Identität darf die ursprüngliche Abstammung referenzieren.

## Import fremder Capsules

Beim Import einer unbekannten Capsule muss NovaOS mindestens prüfen:

```text
format
integrity
signature
origin
trust
dependencies
permissions
information_flow
```

Bis zum Abschluss dieser Prüfung darf keine privilegierte Ausführung erfolgen.

## Isolation

Nicht vertrauenswürdige Capsules müssen mit eingeschränkten Rechten geöffnet oder analysiert werden können.

Beispiel:

```text
Untrusted Capsule
    ↓
restricted inspection environment
    ↓
verification
    ↓
optional execution
```

Das reine Anzeigen von Metadaten darf nicht automatisch die enthaltene Aufgabe ausführen.

## Security Validation Result

Die Sicherheitsprüfung kann mindestens folgende Zustände liefern:

```text
TRUSTED
VALID_UNTRUSTED
AUTHORIZATION_REQUIRED
MODIFIED
CORRUPTED
BLOCKED
```

### `TRUSTED`

Integrität, Identität und benötigter Trust-Kontext sind gültig.

### `VALID_UNTRUSTED`

Die Capsule ist technisch unverändert, besitzt jedoch keinen ausreichenden Trust.

### `AUTHORIZATION_REQUIRED`

Vor Resume ist eine neue Berechtigung erforderlich.

### `MODIFIED`

Der Inhalt unterscheidet sich vom erwarteten signierten oder gehashten Zustand.

### `CORRUPTED`

Die Capsule ist beschädigt.

### `BLOCKED`

Security- oder Policy-Regeln verhindern die Nutzung.

## Beispiel

```text
TaskCapsule {
    identity {
        capsule_id:
            capsule:42

        instance_id:
            capsule-instance:91
    }

    origin {
        system:
            device:workstation-01
    }

    security {
        trust_requirement:
            verified

        information_labels {
            project.confidential
        }
    }

    integrity {
        manifest_hash:
            ...

        state_hash:
            ...
    }

    signature {
        signer:
            device:workstation-01

        value:
            ...
    }
}
```

Beim Resume:

```text
Integrity:
    VALID

Signature:
    VALID

Trust:
    ACCEPTED

Permissions:
    REVALIDATION_REQUIRED
```

Erst nach erfolgreicher Berechtigungsprüfung darf die Aufgabe fortgesetzt werden.

## Normative Anforderungen

1. Jede TaskCapsule MUSS eine eindeutige logische Identität besitzen.
2. Capsule-Identität und konkrete Capsule-Instanz MÜSSEN unterscheidbar sein können.
3. Manifest, Task State und relevante Ressourcen MÜSSEN auf Integrität prüfbar sein.
4. Signaturgültigkeit DARF nicht automatisch mit Trust oder Autorisierung gleichgesetzt werden.
5. TaskCapsules DÜRFEN keine früheren Berechtigungen automatisch auf neue Sicherheitskontexte übertragen.
6. Persistente Secrets SOLLEN über geschützte Referenzen statt als Klartext gespeichert werden.
7. Information-Flow- und Sicherheitsmetadaten MÜSSEN beim Transfer erhalten bleiben.
8. Nicht vertrauenswürdige Capsules MÜSSEN ohne privilegierte automatische Ausführung inspizierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Capsule Identity
- Herkunft
- Integritätsprüfung
- Signaturen
- Trust
- Credential-Behandlung
- Security Context
- Replay- und Fork-Grundlagen

Nicht Bestandteil sind:

- konkrete kryptografische Algorithmen
- allgemeines Benutzer- und Identitätsmanagement
- Resource Packaging
- Capability Isolation
- Migration und Resume
- Versionskompatibilität

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-CAPSULE-0002 – Task State Capture`
- `NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`
- `NPSPEC-CAPSULE-0006 – Capsule Migration & Resume`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`
- `NPSPEC-CAUSAL-0002 – Causal Event Records`