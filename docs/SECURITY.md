# gOS Security Considerations

## Kernel Hardening Checklist

- [ ] Stack canaries disabled for now (freestanding), re-enable with custom implementation
- [ ] No stack execution: NX bit set in page tables
- [ ] ASLR not yet implemented (future work)
- [ ] User/supervisor page flags used in VMM
- [ ] TSS IST stacks for critical exceptions
- [ ] `cli` in panic handler to prevent recursive interrupts

## Privilege Levels

| Ring | Usage | Status |
|------|-------|--------|
| Ring 0 | Kernel | Active |
| Ring 3 | User processes | Not yet entered |

## Security Context

The `security/security.c` stub tracks:
- UID/GID
- Effective UID/GID
- Capability bitmap
- Privileged flag

## Known Risks

1. **No KASLR**: Kernel is at a fixed virtual address
2. **No SMAP/SMEP**: Not enforced in this version
3. **No stack canaries**: Custom implementation needed for freestanding
4. **No ASLR**: User processes would have predictable addresses
5. **Open I/O ports**: User mode would need I/O bitmap in TSS

## Future Work

- Implement KASLR (randomize kernel base at boot)
- Enable SMAP/SMEP if CPU supports them
- Custom stack canary per process
- I/O permission bitmap in TSS for user mode
- Secure boot / signed kernel images
- Capability-based security model

## Reference

- https://wiki.osdev.org/Supervisor_Mode_Access_Prevention
- https://wiki.osdev.org/Supervisor_Mode_Execution_Prevention
- https://wiki.osdev.org/Security
