# ADR-SECURITY-0014 – Secure und Measured Boot

## Status

Angenommen

## Kategorie

Security / Boot / Trust / Integrity

## Kontext

Die Sicherheitsgarantien von NovaOS sind nur belastbar, wenn bereits der Startpfad des Systems überprüfbar ist.

Manipulierte Firmware-Komponenten, Bootloader, Kernel oder frühe Systemkomponenten könnten ansonsten spätere Sicherheitsmechanismen umgehen.

NovaOS benötigt deshalb sowohl eine kontrollierte Boot-Verifikation als auch einen nachvollziehbaren Nachweis darüber, welche Komponenten tatsächlich gestartet wurden.

---

## Entscheidung

NovaOS unterstützt **Secure Boot und Measured Boot als komplementäre Mechanismen**.

```text
Secure Boot
    → Darf diese Komponente starten?

Measured Boot
    → Welche Komponente wurde tatsächlich gestartet?
```

Beide Mechanismen werden in die allgemeine Nova.Trust- und Boot-Architektur integriert.

---

## Grundprinzip

```text
Verify before execution.
Measure before trust depends on state.
```

Secure Boot schützt die Ausführungskette.

Measured Boot dokumentiert kryptographisch den tatsächlichen Bootzustand.

---

## Secure Boot

Jede sicherheitsrelevante Bootstufe muss die nächste Stufe gemäß ihrer Trust Policy überprüfen können.

```text
Firmware
   ↓ verify
Bootloader
   ↓ verify
Kernel
   ↓ verify
Early System Components
```

Eine erfolgreiche kryptographische Prüfung bedeutet dabei nicht automatisch universelles Vertrauen oder Authority.

---

## Measured Boot

Boot-Komponenten und relevante Konfigurationen können vor oder während ihrer Aktivierung gemessen werden.

```text
Boot Component
      ↓
Cryptographic Measurement
      ↓
Measurement Log / TPM
```

Dadurch entsteht ein überprüfbarer Nachweis des tatsächlichen Bootzustands.

Measured Boot verhindert die Ausführung einer Komponente nicht automatisch.

---

## Secure Boot und Measured Boot

Beide Mechanismen erfüllen unterschiedliche Aufgaben.

```text
              Boot Component
              /            \
             ↓              ↓
      Verification      Measurement
             ↓              ↓
       Execution?       Boot Evidence
```

NovaOS darf beide unabhängig oder gemeinsam verwenden, wobei sicherheitskritische Systemkonfigurationen beide Mechanismen verlangen können.

---

## TPM Integration

Wenn ein TPM oder vergleichbare vertrauenswürdige Hardware verfügbar ist, kann NovaOS Boot Measurements dort verankern.

Dies ermöglicht unter anderem:

- geschützte Measurement Registers,
- Attestation,
- Bindung von Secrets an Systemzustände,
- überprüfbare Boothistorie.

NovaOS darf jedoch nicht grundsätzlich unbootbar werden, nur weil eine bestimmte TPM-Implementierung fehlt, sofern die aktive Security Policy dies nicht ausdrücklich verlangt.

---

## Boot Policy

Die Boot Policy bestimmt, welche Komponenten ausgeführt werden dürfen.

Sie kann Anforderungen definieren für:

- Signaturen,
- Code Integrity,
- Signer Trust,
- Versionen,
- Revocation,
- Boot-Konfiguration,
- Recovery-Komponenten.

Die Policy muss zwischen kryptographischer Gültigkeit und tatsächlichem Trust unterscheiden.

---

## Recovery

Recovery und NovaDOS sind Bestandteil des kontrollierten Bootmodells.

Ein Recovery-Pfad darf Secure Boot nicht implizit umgehen.

```text
Normal Boot
Recovery Boot
NovaDOS
```

können unterschiedliche Policies besitzen, müssen aber jeweils explizit definierten Trust-Regeln folgen.

---

## Updates und Rollback

Bootloader-, Kernel- und Systemupdates müssen mit Secure und Measured Boot kompatibel bleiben.

A/B-Boot und Rollback dürfen nur auf Zustände wechseln, die gemäß der aktuellen Boot Policy zulässig sind.

Ein kryptographisch korrektes, aber sicherheitskritisch widerrufenes Image darf nicht allein wegen seiner gültigen Signatur gestartet werden.

---

## Entwicklungsmodus

NovaOS darf einen expliziten Development Boot Mode unterstützen.

Dieser kann alternative oder selbst signierte Komponenten erlauben.

Ein solcher Modus muss:

- bewusst aktiviert,
- sichtbar,
- policygesteuert,
- auditierbar

sein.

Er darf die Produktions-Bootpolicy nicht unbemerkt abschalten.

---

## Architekturunabhängigkeit

Secure und Measured Boot dürfen nicht ausschließlich an UEFI oder eine einzelne Hardwareplattform gebunden sein.

```text
UEFI
BIOS
Future Firmware
        ↓
NovaOS Boot Trust Architecture
```

Plattformspezifische Mechanismen werden über die Boot- und Platform-Abstraktionen integriert.

---

## Audit und Introspection

NovaOS muss den Boot-Trust-Zustand nachvollziehbar machen können.

Dazu gehören insbesondere:

```text
Boot Components
Verification State
Measurement State
Signer Identity
Trust State
Boot Policy
Recovery State
```

---

## Normative Anforderungen

1. NovaOS MUSS Secure Boot unterstützen.
2. NovaOS MUSS Measured Boot unterstützen.
3. Secure Boot und Measured Boot MÜSSEN getrennte Sicherheitsfunktionen bleiben.
4. Sicherheitskritische Boot-Komponenten MÜSSEN vor ihrer Ausführung gemäß Boot Policy überprüfbar sein.
5. Relevante Boot-Komponenten MÜSSEN kryptographisch messbar sein.
6. NovaOS MUSS TPM-gestützte Measurements unterstützen können.
7. Das Fehlen eines TPM DARF den Bootvorgang nur verhindern, wenn die aktive Security Policy dies verlangt.
8. Eine gültige Signatur DARF widerrufenen oder anderweitig nicht vertrauenswürdigen Code NICHT automatisch autorisieren.
9. Recovery und NovaDOS MÜSSEN in die Boot-Trust-Architektur integriert sein.
10. Rollback DARF aktuelle Security- und Revocation-Policies NICHT umgehen.
11. Ein Development Boot Mode MUSS explizit und auditierbar sein.
12. Secure und Measured Boot DÜRFEN NICHT ausschließlich von UEFI abhängig sein.
13. Boot-Trust- und Measurement-Zustände MÜSSEN kontrolliert introspektierbar und auditierbar sein.
14. Nicht zuverlässig validierbarer sicherheitskritischer Boot-Code MUSS fail-safe behandelt werden.

---

## Konsequenzen

### Positive Konsequenzen

- überprüfbare Bootkette,
- Schutz vor manipulierten Boot-Komponenten,
- kryptographisch nachvollziehbarer Systemstart,
- Grundlage für Attestation und zustandsgebundene Secrets,
- sichere Integration von Recovery und Rollback.

### Negative Konsequenzen

- Schlüssel-, Policy- und Revocation-Verwaltung werden komplexer,
- plattformübergreifende Unterstützung benötigt unterschiedliche Mechanismen,
- fehlerhafte Boot-Policies können legitime Systeme am Start hindern.

---

## Verworfene Alternativen

### Nur Secure Boot

Verworfen.

Verifikation allein liefert keinen vollständigen Nachweis darüber, welcher Systemzustand tatsächlich gestartet wurde.

### Nur Measured Boot

Verworfen.

Messung allein verhindert nicht die Ausführung unerlaubter Komponenten.

### Ausschließlich UEFI Secure Boot

Verworfen.

Die NovaOS-Architektur soll unabhängig vom konkreten Firmwaremodell bleiben.

### Recovery ohne Boot-Verifikation

Verworfen.

Ein ungeschützter Recovery-Pfad würde die Trusted Boot Chain umgehen.

---

## Abhängigkeiten

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Unterstützung`
- `ADR-BOOT-0005_Trusted_Boot_Chain`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot`
- `ADR-BOOT-0007_TPM_Integration`
- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-BOOT-0009_A_B_Boot_und_Rollback`
- `ADR-SECURITY-0013_Code_Integrity_und_Code_Signing`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-SECURE-BOOT-0001`
- `NPSPEC-SECURITY-MEASURED-BOOT-0001`
- `NPSPEC-SECURITY-BOOT-POLICY-0001`
- `NPSPEC-SECURITY-BOOT-MEASUREMENT-0001`
- `NPSPEC-SECURITY-BOOT-TRUST-0001`
- `NPSPEC-SECURITY-BOOT-TEST-0001`

---

## Ergebnis

NovaOS kombiniert präventive Boot-Verifikation mit kryptographischer Zustandsmessung:

```text
Boot Component
      ↓
Verify
      ↓
Measure
      ↓
Execute
      ↓
Trusted Boot Evidence
```

Secure Boot kontrolliert, **was gestartet werden darf**.

Measured Boot dokumentiert, **was tatsächlich gestartet wurde**.

Gemeinsam bilden beide Mechanismen die Grundlage für einen überprüfbaren Systemstart und die weitere Trust-Architektur von NovaOS.