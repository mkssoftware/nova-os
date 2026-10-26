# NPSPEC-COMPOSITOR-SURFACE-0006 – Nova Surface Sharing

**Status:** Angenommen  
**Bereich:** Compositor / Surface-Freigabe  
**System:** NovaOS

## 1. Zweck

Nova Surface Sharing definiert die sichere und effiziente gemeinsame Nutzung grafischer Oberflächen zwischen Anwendungen, Systemdiensten und dem Nova Compositor.

Ziel ist die kontrollierte Bereitstellung von Surface-Inhalten ohne unnötige Speicherkopien und ohne Verletzung der Prozess- und Sicherheitsisolation.

## 2. Architektur

- Integration in Nova Surface Management und Nova Display Protocol.
- Capability-basierte Freigabe von Surfaces und Grafikbuffern.
- Trennung zwischen Surface-Eigentum, Zugriffsrechten und Buffer-Nutzung.
- Unterstützung von Zero-Copy-Sharing.
- Prozessübergreifende Ressourcenverwaltung über sichere Handles.
- Synchronisation über Nova Surface Synchronization.
- Unterstützung hardwarebeschleunigter und softwarebasierter Buffer.
- Keine direkte Freigabe interner Compositor-Speicherstrukturen.

## 3. Kernfunktionen

- Freigabe grafischer Oberflächen zwischen berechtigten Clients.
- Erstellung eingeschränkter Surface-Referenzen.
- Vergabe und Entzug von Zugriffsrechten.
- Gemeinsame Nutzung von Grafikbuffern.
- Unterstützung lesender und kontrollierter schreibender Zugriffe.
- Synchronisation gemeinsamer Buffer.
- Verwaltung von Referenzen und Ressourcenlebensdauer.
- Unterstützung eingebetteter und gespiegelter Oberflächen.
- Bereitstellung von Surface-Inhalten für autorisierte Bildschirmaufnahmen.
- Erkennung und Bereinigung ungültiger Freigaben.

## 4. Normative Anforderungen

- SURFACE-001: Jede Surface MUSS einen eindeutig definierten Eigentümer besitzen.
- SURFACE-002: Surface-Freigaben MÜSSEN durch Capabilities autorisiert werden.
- SURFACE-003: Zugriffsrechte MÜSSEN unabhängig vom Surface-Eigentum verwaltet werden.
- SURFACE-004: Freigaben MÜSSEN auf notwendige Berechtigungen beschränkt werden können.
- SURFACE-005: Lesende Freigaben DÜRFEN keine impliziten Schreibrechte gewähren.
- SURFACE-006: Schreibende Zugriffe MÜSSEN explizit autorisiert und synchronisiert werden.
- SURFACE-007: Clients DÜRFEN keine fremden Surface-Inhalte ohne entsprechende Berechtigung auslesen.
- SURFACE-008: Freigegebene Ressourcen MÜSSEN über sichere, widerrufbare Referenzen erreichbar sein.
- SURFACE-009: Der Entzug einer Freigabe MUSS zukünftige Zugriffe verhindern.
- SURFACE-010: Bereits laufende Zugriffe MÜSSEN beim Widerruf kontrolliert abgeschlossen oder abgebrochen werden.
- SURFACE-011: Grafikbuffer DÜRFEN während aktiver Nutzung nicht vorzeitig freigegeben werden.
- SURFACE-012: Gemeinsame Buffer-Zugriffe MÜSSEN mit Nova Surface Synchronization abgestimmt werden.
- SURFACE-013: Zero-Copy-Sharing SOLL bevorzugt werden, sofern Hardware und Sicherheitsanforderungen dies erlauben.
- SURFACE-014: Geschützte Systemoberflächen DÜRFEN nicht ohne ausdrückliche Berechtigung freigegeben werden.
- SURFACE-015: Bildschirmaufnahmen MÜSSEN bestehende Datenschutz- und Capability-Regeln berücksichtigen.
- SURFACE-016: Surface-Freigaben MÜSSEN pro Client begrenzbar sein.
- SURFACE-017: Beim Beenden eines Clients MÜSSEN dessen Freigaben kontrolliert bereinigt werden.
- SURFACE-018: Fehlerhafte Freigaben DÜRFEN den Compositor oder andere Clients nicht destabilisieren.

## 5. Freigabemodell

### Owner
Besitzt die Surface und kontrolliert ihre freigabefähigen Eigenschaften innerhalb der geltenden Systemrichtlinien.

### Shared Reference
Stellt eine eingeschränkte Referenz auf eine freigegebene Surface bereit.

### Read Access
Erlaubt autorisierten Zugriff auf freigegebene Bildinhalte.

### Write Access
Erlaubt ausdrücklich genehmigte Änderungen an dafür vorgesehenen Ressourcen.

### Presentation Access
Erlaubt die Einbindung einer Surface in einen autorisierten Darstellungskontext.

### Capture Access
Erlaubt den Zugriff auf Bildinhalte für Bildschirmaufnahmen oder Übertragungen.

Freigaberechte MÜSSEN unabhängig voneinander vergeben und entzogen werden können.

## 6. Freigabeablauf

1. **Request:** Client fordert eine Surface-Freigabe an.
2. **Authorize:** Berechtigungen und Sicherheitsrichtlinien prüfen.
3. **Create:** Eingeschränkte Freigabereferenz erstellen.
4. **Transfer:** Referenz über einen autorisierten IPC-Kanal übertragen.
5. **Acquire:** Empfänger übernimmt die freigegebene Ressource.
6. **Synchronize:** Zugriffe und Buffer-Nutzung koordinieren.
7. **Use:** Surface entsprechend den gewährten Rechten verwenden.
8. **Revoke:** Freigabe bei Bedarf widerrufen.
9. **Release:** Referenzen und nicht mehr benötigte Ressourcen freigeben.

## 7. Sicherheits- und Ressourcenverwaltung

- Keine Weitergabe roher Speicheradressen zwischen Prozessen.
- Prüfung sämtlicher Freigabe- und Zugriffsoperationen.
- Schutz vor unberechtigter Rechteweitergabe.
- Widerrufbarkeit delegierter Capabilities.
- Referenzzählung oder gleichwertige sichere Lebensdauerverwaltung.
- Isolation fehlerhafter oder kompromittierter Clients.
- Schutz vertraulicher und sicherheitskritischer Oberflächen.
- Kontrollierte Behandlung von Client-Abstürzen.
- Protokollierung sicherheitsrelevanter Freigabeoperationen.

## 8. Performance und Zuverlässigkeit

- Zero-Copy-Buffer-Sharing bei geeigneter Hardware.
- Vermeidung unnötiger CPU-GPU-Speichertransfers.
- Asynchrone Freigabe- und Synchronisationsoperationen.
- Wiederverwendung bestehender Buffer-Handles.
- Begrenzung gleichzeitig freigegebener Ressourcen.
- Vermeidung blockierender Zugriffe zwischen unabhängigen Clients.
- Kontrollierter Fallback auf sichere Kopierverfahren.
- Automatische Bereinigung verwaister Freigaben.
- Wiederherstellung gültiger Freigabebeziehungen nach Compositor-Fehlern, sofern Berechtigungen weiterhin bestehen.

## 9. Ergebnis

NovaOS erhält eine sichere und ressourcenschonende Architektur zur gemeinsamen Nutzung grafischer Oberflächen.

Anwendungen und Systemdienste können Surface-Inhalte kontrolliert austauschen, einbetten oder darstellen, ohne ihre Prozessisolation aufzugeben.

Capability-basierte Berechtigungen, synchronisierte Buffer-Zugriffe und Zero-Copy-Verfahren bilden die Grundlage für effiziente grafische Zusammenarbeit und moderne Desktopfunktionen.