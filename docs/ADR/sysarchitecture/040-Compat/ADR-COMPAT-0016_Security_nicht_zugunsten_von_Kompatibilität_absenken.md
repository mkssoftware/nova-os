# ADR-COMPAT-0016 – Security nicht zugunsten von Kompatibilität absenken

## Status

Angenommen

## Kategorie

Compatibility / Security / Isolation

## Kontext

Legacy-Software kann Sicherheitsannahmen besitzen, die nicht mit dem Sicherheitsmodell von NovaOS vereinbar sind.

Beispiele:

```text
Globale Schreibrechte
Unbeschränkter Prozesszugriff
Unsichere Speicheroperationen
Direkter Hardwarezugriff
Veraltete Authentifizierung
Nicht isolierte IPC
Ausführbarer beschreibbarer Speicher
```

Hohe Softwarekompatibilität darf nicht dazu führen, dass NovaOS seine grundlegenden Sicherheitsgarantien systemweit abschwächt.

## Entscheidung

NovaOS behandelt **Security als harte Grenze für Compatibility**.

```text
Compatibility Request
        ↓
Security Validation
        ↓
Allowed → Execute
Denied  → Isolate / Emulate / Reject
```

Kann Legacy-Semantik nur durch Verletzung einer harten Sicherheitsregel bereitgestellt werden, wird diese Semantik nicht direkt unterstützt.

## Keine globale Absenkung

Compatibility-Komponenten dürfen keine globalen Sicherheitsmechanismen deaktivieren.

Insbesondere dürfen nicht zugunsten einzelner Anwendungen abgeschwächt werden:

```text
Capability Enforcement
Memory Protection
Process Isolation
Code Integrity
W^X
Device Isolation
Information Flow Policies
Data Sovereignty
Trust Requirements
```

## Kompatibilität durch Isolation

Benötigt Legacy-Software unsichere historische Eigenschaften, werden diese innerhalb einer begrenzten Compatibility Domain nachgebildet.

```text
Legacy Application
        ↓
Compatibility Domain
        ↓
Security Boundary
        ↓
NovaOS
```

Die Legacy-Anwendung darf innerhalb ihrer virtuellen Umgebung eine ältere Semantik sehen, ohne dass diese außerhalb der Domain gilt.

## Fallback

Kann eine Anforderung nicht sicher übersetzt werden, gilt die Execution Compatibility Ladder.

```text
Translation
    ↓
Isolation
    ↓
Virtual Machine
    ↓
Emulation
    ↓
Reject
```

Eine stärkere Isolation ist einer Absenkung der Systemsicherheit vorzuziehen.

## Keine impliziten Ausnahmen

Kompatibilitätsprobleme erzeugen keine automatischen Security Exceptions.

Auch bekannte oder häufig verwendete Legacy-Software erhält keine Sonderrechte allein aufgrund ihrer Verbreitung oder Kompatibilitätsanforderungen.

Explizit genehmigte Ausnahmen müssen weiterhin innerhalb der von NovaOS erlaubten Sicherheitsgrenzen liegen.

## Unbekannter Sicherheitszustand

Kann NovaOS nicht bestimmen, ob eine Legacy-Operation sicher ausgeführt werden kann, gilt:

```text
Unknown ≠ Safe
```

Die Operation muss isoliert, eingeschränkt oder abgelehnt werden.

## Normative Anforderungen

1. Compatibility DARF harte NovaOS-Sicherheitsregeln nicht überschreiben.
2. Compatibility-Komponenten DÜRFEN globale Sicherheitsmechanismen nicht deaktivieren.
3. Legacy-Semantik SOLL durch Übersetzung oder Isolation bereitgestellt werden.
4. Unsichere Anforderungen SOLLEN auf eine stärkere Compatibility-Stufe verschoben werden.
5. Legacy-Software DARF keine impliziten Sonderrechte erhalten.
6. Compatibility Capabilities MÜSSEN dem Least-Privilege-Prinzip folgen.
7. Sicherheitsgrenzen MÜSSEN auch innerhalb von Compatibility Domains durchsetzbar bleiben.
8. Ein unbekannter Sicherheitszustand DARF nicht als sicher behandelt werden.
9. Ist keine sichere Ausführungsform möglich, MUSS die Ausführung abgelehnt werden.
10. Security Constraints MÜSSEN gegenüber Compatibility-Optimierungen Vorrang haben.

## Konsequenzen

### Positiv

- Legacy-Kompatibilität schwächt NovaOS nicht systemweit,
- unsichere Software kann kontrolliert isoliert werden,
- Compatibility bleibt mit dem Capability-Sicherheitsmodell vereinbar,
- neue Sicherheitsmechanismen können eingeführt werden, ohne Legacy-Software zum Architekturzwang zu machen.

### Negativ

- bestimmte Legacy-Anwendungen funktionieren möglicherweise nur eingeschränkt,
- stärkere Isolation kann zusätzlichen Ressourcenbedarf verursachen,
- einzelne Anwendungen können vollständig inkompatibel bleiben.

## Abhängigkeiten

- `ADR-COMPAT-0007_Execution_Compatibility_Ladder`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-COMPAT-0009_Legacy_Driver_in_Isolation_oder_VM`
- `ADR-COMPAT-0012_JIT_nur_mit_Generated_Code_Capability`
- `ADR-SECURITY-0001`
- `ADR-CAPABILITY-0001`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-AUTONOMY-0008_Autonomy_unter_harten_Safety_Security_User_Constraints`

## Ergebnis

Für NovaOS gilt:

```text
Security > Compatibility

Compatible if safe
Isolate if necessary
Reject if unsafe
```

Kompatibilität wird innerhalb der Sicherheitsarchitektur realisiert. Die Sicherheitsarchitektur wird nicht an Legacy-Software angepasst.