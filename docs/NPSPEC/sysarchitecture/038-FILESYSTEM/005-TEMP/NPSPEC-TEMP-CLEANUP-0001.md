# NPSPEC-TEMP-CLEANUP-0001 – Nova Temporary Resource Cleanup

## Status

Angenommen

## Kategorie

Temporary Resources / Cleanup

## Zweck

NovaOS definiert die kontrollierte Bereinigung nicht mehr benötigter temporärer Ressourcen.

Cleanup verhindert dauerhaft verwaiste Temp-Ressourcen und gibt belegte Systemressourcen zuverlässig zurück.

## Grundprinzipien

```text
Cleanup ≠ Blind Delete
Released ≠ Destroyed
Expired → Reclaimable
Recovery Candidate ≠ Reclaimable
Cleanup ≠ Authority
```

## Cleanup-Modell

Eine Ressource kann abhängig von Zustand und Policy zur Bereinigung freigegeben werden:

```text
Active
  ↓
Released / Expired / Owner Terminated
  ↓
Reclaimable
  ↓
Cleanup
  ↓
Destroyed
```

Vor der endgültigen Bereinigung müssen bestehende Recovery-, Sicherheits- und Lebenszyklusregeln berücksichtigt werden.

## Cleanup-Auslöser

Cleanup kann ausgelöst werden durch:

```text
Explicit Release
Process Exit
Session End
Boot Completion
TTL Expiration
Owner Removal
Resource Pressure
Recovery Completion
Scheduled Cleanup
```

## Cleanup Manager

NovaOS stellt einen zentral koordinierten Cleanup-Mechanismus bereit.

```text
Temp Registry
     ↓
Cleanup Evaluation
     ↓
Policy Check
     ↓
Recovery Check
     ↓
Reclaim
     ↓
Verify
```

Subsysteme können eigene Cleanup-Handler bereitstellen, bleiben jedoch an das gemeinsame Temp-Modell gebunden.

## Verwaiste Ressourcen

Nach Abstürzen oder unerwarteten Abbrüchen können Ressourcen ohne aktiven Owner verbleiben.

NovaOS muss solche Ressourcen erkennen können.

```text
Owner Missing
     ↓
Validate Resource
     ↓
Recovery Candidate?
   ↙             ↘
 Yes             No
  ↓               ↓
Preserve       Reclaim
```

Eine Ressource darf nicht entfernt werden, solange sie noch für eine zulässige Recovery benötigt wird.

## Resource Pressure

Unter Ressourcendruck kann Cleanup priorisiert werden.

Bevorzugte Kandidaten sind:

```text
Expired
Released
Orphaned
Invalid Cache
Low Priority Temp
```

Aktive Ressourcen dürfen nicht allein wegen ihrer temporären Klassifikation unkontrolliert zerstört werden.

## Sichere Bereinigung

Sensible temporäre Daten können eine sichere Cleanup-Policy verlangen.

Diese kann zusätzliche Maßnahmen zur Löschung oder Unzugänglichmachung der Daten erfordern.

## Normative Anforderungen

1. NovaOS MUSS reclaimable Temp-Ressourcen automatisch bereinigen können.
2. Cleanup MUSS den aktuellen Ressourcenstatus prüfen.
3. Aktive Ressourcen DÜRFEN nicht unkontrolliert entfernt werden.
4. Recovery-Kandidaten MÜSSEN vor Cleanup erkannt werden.
5. Verwaiste Temp-Ressourcen MÜSSEN erkennbar sein.
6. TTL-abgelaufene Ressourcen SOLLEN bevorzugt reclaimable werden.
7. Cleanup MUSS unter Ressourcendruck priorisierbar sein.
8. Cleanup DARF keine zusätzliche Authority erzeugen.
9. Sensible Ressourcen MÜSSEN gemäß ihrer Security Policy bereinigt werden.
10. Nach erfolgreichem Cleanup DÜRFEN alte Referenzen nicht auf neue Ressourcen umgebunden werden.
11. Cleanup-Fehler MÜSSEN erkennbar sein.
12. Cleanup MUSS mit den definierten Temp-Scopes zusammenarbeiten.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-TEMP-PROCESS-0001`
- `NPSPEC-TEMP-SESSION-0001`
- `NPSPEC-TEMP-BOOT-0001`
- `NPSPEC-TEMP-TTL-0001`
- `NPSPEC-TEMP-CACHE-0001`
- `NPSPEC-TEMP-RECOVERY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS kann temporäre Ressourcen systemweit kontrolliert zurückgewinnen. Abgelaufene, freigegebene und verwaiste Ressourcen werden erkannt und bereinigt, während aktive oder für Recovery benötigte Ressourcen geschützt bleiben.