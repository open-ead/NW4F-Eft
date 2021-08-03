from elfUtils import extract as f_elf_extract, openFile as f_elf_open
from struct import unpack as f_unpack


def main():
    elfObj = f_elf_open('Turbo_kiosk.elf')

    tableBin = f_elf_extract(elfObj, 0x101539f0, 0x1016b9f0 - 0x101539f0)
    numBytes = 32*32*32 * 3
    assert len(tableBin) == numBytes

    table = f_unpack('>%uB' % numBytes, tableBin)
    table_str = '\n'.join([('    ' + ', '.join("%3u" % value for value in table[i : i + 3]) + ',') for i in range(0, numBytes, 3)])
    print(table_str)


if __name__ == '__main__':
    main()
