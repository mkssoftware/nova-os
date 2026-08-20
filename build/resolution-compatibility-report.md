# NovaOS Resolution Compatibility Report

Deterministischer Lauf: 65 simulierte Kombinationen (13 Aufloesungen x 5 Skalierungsstufen).
Reale EDK2/QEMU-GOP-Framebuffer: 3. Die Pruefsummen dienen der Regressionserkennung; eine freigegebene Pixel-Baseline und Toleranz sind noch nicht normativ definiert.

| Aufloesung | Seitenverhaeltnis | Ergebnis | Bytes | SHA-256 | Artefakt |
|---|---|---|---:|---|---|
| 800x600 | 4:3 | PASS | 1440015 | `8A27DF94EFA863A1515A316CA4D3851E5EA2D52BE2E6166B9688A743FED2CAAF` | build/uefi-800x600.ppm |
| 1280x720 | 16:9 | PASS | 2764816 | `8A16A25618DB4D0035D728F874909BD8E0ADE964D21CE31CEC1ABCE6EA11679C` | build/uefi-1280x720.ppm |
| 1920x1080 | 16:9 | PASS | 6220817 | `C8162C6BB6150B0DA2A049381C0ADCC09360CF6D4048E3A13B28D454FE1109F3` | build/uefi-1920x1080.ppm |

## Verbleibende Grenzen

- GOP-Modi fuer alle zehn Pflichtaufloesungen sowie 5:4, 16:10 und Ultra-Wide stehen in der verwendeten QEMU-Firmware nicht vollstaendig zur Verfuegung.
- Installer und Self-Healing besitzen noch keine produktive GUI.
- SVG bleibt wegen des unvollstaendigen normativen Vektorvertrags blockiert.
- Referenzbilder und zulaessige Pixel-Differenztoleranzen sind in NPSPEC-BOOTTEST-0006 nicht beziffert.
