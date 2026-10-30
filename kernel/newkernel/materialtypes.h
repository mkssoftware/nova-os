/* NovaOS C-Kernel - Nova Material Engine: Standard-Materialtypen. Port aus
   docs/NPSPEC/sysarchitecture/049-GUI/010-Material_Engine/002-Materialtypen/
   NPSPEC-MATERIAL-{ACRYLIC,LIQUID,FROSTED,CRYSTAL,SOLID}-0001.md.

   Registriert die fuenf eingebauten Materialien (feste IDs MATERIAL_ID_*
   aus material.h) mit den in den jeweiligen Specs (Abschnitt 2/3)
   genannten Default-Werten. Muss nach material_initialize() und vor jeder
   anderen Materialverwendung aufgerufen werden. */
#ifndef NOVA_MATERIALTYPES_H
#define NOVA_MATERIALTYPES_H

void materialtypes_initialize(void);
int materialtypes_self_test(void); /* 0 = ok */

#endif
