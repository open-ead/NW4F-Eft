from elfUtils import extract as f_elf_extract, openFile as f_elf_open
from struct import unpack as f_unpack


elemcount_Vec3 = 3
sizeof_float = 4
sizeof_pointer = 4

debug_print_str = bytearray()

def debug_print(string):
    assert isinstance(string, str)
    global debug_print_str
    debug_print_str += string.encode('utf-8')
    debug_print_str += b'\n'


def sphereCommon(elfObj, name, numAddresses, addresses_map, innerTablePrefix, start):
    assert len(addresses_map) == numAddresses

    addressesByteSize = numAddresses * sizeof_pointer
    addressesBin = f_elf_extract(elfObj, start, addressesByteSize)
    assert len(addressesBin) == addressesByteSize

    addresses = f_unpack('>%uI' % numAddresses, addressesBin)
    assert len(addresses) == numAddresses

    for address, numVec3 in zip(addresses, addresses_map):
        numFloats = numVec3 * elemcount_Vec3

        byteSize = numFloats * sizeof_float
        tableBin = f_elf_extract(elfObj, address, byteSize)
        assert len(tableBin) == byteSize

        table = f_unpack('>%uf' % numFloats, tableBin)
        assert len(table) == numFloats

        debug_print("static const math::VEC3 %s%u[%u] = {" % (innerTablePrefix, numVec3, numVec3))

        # I saw that some values make more sense with precision of 6 decimal places, whereas it doesn't affect others
        table_str = '\n'.join([('    ' + 'f, '.join("%9s" % ("%.6f" % value) for value in table[i : i + elemcount_Vec3]) + 'f,') for i in range(0, numFloats, elemcount_Vec3)])
        debug_print(table_str)

        debug_print("};\n")

    # I thought they would use "const math::VEC3* const", but this table ends up in the .data section, unlike the other tables which are in the .rodata section
    debug_print("static const math::VEC3* %s[%u] = {" % (name, numAddresses))
    for numVec3 in addresses_map:
        debug_print("    %s%u," % (innerTablePrefix, numVec3))
    debug_print("};\n")


def sphere(elfObj):
    addresses_map = [
        2,
        3,
        4,
        6,
        8,
        12,
        20,
        32,
    ]

    sphereCommon(elfObj, 'gSameDivideSphereTbl',    8, addresses_map, "gSameDivideSphere",    0x10153848)


def sphere64(elfObj):
    addresses_map = range(2, 65)
    sphereCommon(elfObj, 'gSameDivideSphere64Tbl', 63, addresses_map, "gSameDivideSphere64_", 0x10153868)


def main():
    elfObj = f_elf_open('Turbo_kiosk.elf')
    sphere(elfObj)
    sphere64(elfObj)

    with open("eft_EmitterVolume.hpp", "wb+") as out:
        out.write(debug_print_str)


if __name__ == '__main__':
    main()
