import struct

with open('build/kernel.elf', 'rb') as f:
    data = bytearray(f.read())

shoff = struct.unpack_from('<Q', data, 0x28)[0]
shentsize = struct.unpack_from('<H', data, 0x3A)[0]
shnum = struct.unpack_from('<H', data, 0x3C)[0]
shstrndx = struct.unpack_from('<H', data, 0x3E)[0]

strtab_off = struct.unpack_from('<Q', data, shoff + shstrndx * shentsize + 0x18)[0]

for i in range(shnum):
    off = shoff + i * shentsize
    name_idx = struct.unpack_from('<I', data, off)[0]
    sec_type = struct.unpack_from('<I', data, off + 4)[0]
    name = data[strtab_off + name_idx : strtab_off + name_idx + 20]
    name = name.split(b'\x00')[0].decode('ascii', errors='ignore')
    if name == '.note.Xen':
        print(f'Found .note.Xen at section {i}, type={sec_type}')
        if sec_type == 1:
            struct.pack_into('<I', data, off + 4, 7)
            print('Changed type to 7 (SHT_NOTE)')
        break

with open('build/kernel.elf', 'wb') as f:
    f.write(data)
print('Done')
