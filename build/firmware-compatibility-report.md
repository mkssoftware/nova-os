# NovaOS BIOS/UEFI Compatibility Report

| Capability | BIOS | UEFI | Identical contract | Evidence |
|---|---:|---:|---:|---|
| Firmware entry | True | True | True | SeaBIOS / EDK2 OVMF |
| Graphics framebuffer | True | True | True | VBE / GOP |
| Graphics fallback | True | True | True | VBE mode ranking / software renderer |
| Boot manager UI | True | True | False | separate BIOS and UEFI UI pipelines |
| Resources and integrity | True | True | False | UEFI resource manager has no BIOS-C parity |
| Input | True | True | False | BIOS keyboard versus UEFI keyboard/pointer |
| Recovery | False | True | False | independent recovery implementations |
| Kernel handoff | True | False | False | UEFI productive kernel handoff not implemented |
| Secure Boot | False | False | False | optional and not implemented |
| ExitBootServices | True | False | False | not applicable to BIOS; UEFI handoff missing |

Automated: true
Deterministic: true
Virtual platforms: SeaBIOS, EDK2/OVMF
Physical hardware validated: false
Full BIOS/UEFI behavioral parity: false
