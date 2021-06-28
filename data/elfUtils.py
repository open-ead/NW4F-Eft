#elf.py from https://github.com/aboood40091/CafeLoader-Project-Compiler/blob/master/elf.py
from elf import ELF


def extract(elfObj, address, size):
    outBuffer = bytearray(size)

    for entry in elfObj.secHeadEnts:
        if entry.vAddr <= address:
            if address + size <= entry.vAddr + entry.size_:
                start = address - entry.vAddr
                end   = address - entry.vAddr + size
                outBuffer = entry.data[start:end]
                break

            else:
                start = address - entry.vAddr
                end   = entry.size_
                dataSize = end - start 
                outBuffer[:dataSize] = entry.data[start:end]

        # The above mirrors the below

        else:
            if address + size <= entry.vAddr + entry.size_:
                dataStart = entry.vAddr - address
                dataEnd   = size
                size = dataEnd - dataStart 
                outBuffer[dataStart:dataEnd] = entry.data[:size]

            else:
                dataStart = entry.vAddr - address
                dataEnd   = entry.vAddr - address + entry.size_
                outBuffer[dataStart:dataEnd] = entry.data

    return outBuffer


def openFile(path):
    return ELF(path)
