#include "../boot/include/nova_boot_protocol.h"

/* Die eigentlichen Layoutprüfungen stehen als _Static_assert im öffentlichen
 * ABI-Header. Diese Übersetzungseinheit macht sie zu einem Build-Gate. */
int nova_boot_protocol_layout_is_valid(void) {
    return NOVA_BIB_VERSION_MAJOR == 1u && NOVA_BIB_VERSION_MINOR == 2u;
}
