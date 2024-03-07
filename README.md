# On-Demand Paging with Apache NuttX RTOS on Ox64 BL808 SBC

We're testing On-Demand Paging with Apache NuttX RTOS on Ox64 BL808 SBC: https://github.com/apache/nuttx/pull/11824

On QEMU 32-bit RISC-V: It runs like this: https://gist.github.com/lupyuen/6075f31b575b54108e60b028083c16f7

```text
+ qemu-system-riscv32 -semihosting -M virt,aclint=on -cpu rv32 -smp 8 -bios none -kernel nuttx -nographic
ABCnx_start: Entry
uart_register: Registering /dev/console
uart_register: Registering /dev/ttyS0
work_start_lowpri: Starting low-priority kernel worker thread(s)
nxtask_activate: lpwork pid=1,TCB=0x80407ab0
nxtask_activate: AppBringUp pid=2,TCB=0x80407e70
nx_start_application: Starting init task: /system/bin/init
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 0000000f, EPC: 80010698, MTVAL: c0001000
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 0000000f, EPC: 80010698, MTVAL: c0002000
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 0000000f, EPC: 80010698, MTVAL: c0003000
...
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 0000000f, EPC: 80012fe4, MTVAL: c0900c04
nxtask_activate: /system/bin/init pid=3,TCB=0x80408458
nxtask_exit: AppBringUp pid=2,TCB=0x80407e70

NuttShell (NSH) NuttX-12.4.0-RC0
nsh> nx_start: CPU0: Beginning Idle Loop
```

We see different results on Ox64 Device and Ox64 Emulator. Let's investigate...

# Test on Ox64 Device

Based on this NuttX Config: https://github.com/lupyuen2/wip-pinephone-nuttx/blob/on-demand-paging3/boards/risc-v/bl808/ox64/configs/nsh_paging/defconfig#L73

Ox64 SBC fails to start NSH: https://gist.github.com/lupyuen/224cac41efa1db0bebda1414de49eed1

```text
ABCnx_start: Entry
uart_register: Registering /dev/console
work_start_lowpri: Starting low-priority kernel worker thread(s)
nxtask_activate: lpwork pid=1,TCB=0x504092f0
nxtask_activate: AppBringUp pid=2,TCB=0x50409900
nx_start_application: Starting init task: /system/bin/init
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 000000000000000f, EPC: 000000005020a0b0, MTVAL: 0000000080001000
riscv_fillpage: ARCH_TEXT_SIZE=0x80000
riscv_fillpage: ARCH_TEXT_VEND=0x80080000
riscv_fillpage: vaddr=0x80001000
riscv_fillpage: FIX_ARCH_TEXT_VEND=0x80080000
riscv_fillpage: vaddr >= CONFIG_ARCH_TEXT_VBASE && vaddr <= ARCH_TEXT_VEND
riscv_fillpage: !paddr1
riscv_fillpage: mmu_ln_setentry1
riscv_fillpage: riscv_pgwipe1
riscv_fillpage: riscv_pgvaddr
riscv_fillpage: mm_pgalloc
riscv_fillpage: riscv_pgwipe2
riscv_fillpage: mmu_ln_setentry2
riscv_fillpage: return
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 000000000000000f, EPC: 000000005020a0b0, MTVAL: 0000000080001000
riscv_fillpage: ARCH_TEXT_SIZE=0x80000
riscv_fillpage: ARCH_TEXT_VEND=0x80080000
riscv_fillpage: vaddr=0x80001000
riscv_fillpage: FIX_ARCH_TEXT_VEND=0x80080000
riscv_fillpage: vaddr >= CONFIG_ARCH_TEXT_VBASE && vaddr <= ARCH_TEXT_VEND
riscv_fillpage: riscv_pgvaddr
riscv_fillpage: mm_pgalloc
riscv_fillpage: riscv_pgwipe2
riscv_fillpage: mmu_ln_setentry2
riscv_fillpage: return
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 000000000000000f, EPC: 000000005020a0b0, MTVAL: 0000000080001000
```

riscv_fillpage is here: https://github.com/lupyuen2/wip-pinephone-nuttx/blob/on-demand-paging3/arch/risc-v/src/common/riscv_exception.c#L97-L239

TODO: It loops forever at the same MCAUSE, EPC, MTVAL. Why?

# Test on Ox64 Emulator

Based on this NuttX Config: https://github.com/lupyuen2/wip-pinephone-nuttx/blob/on-demand-paging3/boards/risc-v/bl808/ox64/configs/nsh_paging/defconfig#L73

Ox64 Emulator also fails to start NSH: https://gist.github.com/lupyuen/a9821b6867e98fb67c379f1fd842819a

```text
ABCnx_start: Entry
uart_register: Registering /dev/console
work_start_lowpri: Starting low-priority kernel worker thread(s)
nxtask_activate: lpwork pid=1,TCB=0x504092f0
nxtask_activate: AppBringUp pid=2,TCB=0x50409900
nx_start_application: Starting init task: /system/bin/init
raise_exception2: cause=15, tval=0x80001000, pc=0x5020a0b0
pc =000000005020a0b0 ra =0000000050211570 sp =000000005040c510 gp =0000000000000000
tp =0000000000000000 t0 =0000000000000003 t1 =0000000000000007 t2 =0000000000000020
s0 =000000000000aa00 s1 =0000000000000000 a0 =0000000080000020 a1 =0000000050ade000
a2 =000000000000aa00 a3 =000000000000000d a4 =0000000080001000 a5 =0000000000000fe1
a6 =00000000000006f0 a7 =0000000080000020 s2 =000000000000ab7c s3 =0000000000000055
s4 =000000005040ab20 s5 =000000005040a900 s6 =0000000080000020 s7 =000000005040a918
s8 =000000005040c6d8 s9 =0000000000000000 s10=00000000000006f0 s11=0000000000000000
t3 =000000005040c50c t4 =000000005040c500 t5 =00000000000000ff t6 =000000000000000f
priv=S mstatus=0000000a000400a2 cycles=97521621
 mideleg=0000000000000222 mie=0000000000000220 mip=0000000000000080
riscv_fillpage: EXCEPTION: Store/AMO page fault. MCAUSE: 000000000000000f, EPC: 000000005020a0b0, MTVAL: 0000000080001000
riscv_fillpage: ARCH_TEXT_SIZE=0x80000
riscv_fillpage: ARCH_TEXT_VEND=0x80080000
riscv_fillpage: vaddr=0x80001000
riscv_fillpage: FIX_ARCH_TEXT_VEND=0x80080000
riscv_fillpage: vaddr >= CONFIG_ARCH_TEXT_VBASE && vaddr <= ARCH_TEXT_VEND
riscv_fillpage: !paddr1
riscv_fillpage: mmu_ln_setentry1
raise_exception2: cause=13, tval=0x30002084, pc=0x50200b48
pc =0000000050200b48 ra =0000000050200d04 sp =0000000050400830 gp =0000000000000000
tp =0000000000000000 t0 =000000000000002e t1 =000000000000006a t2 =00000000000001ff
s0 =0000000000000072 s1 =0000000200042120 a0 =0000000050400110 a1 =0000000000000072
a2 =000000000000000e a3 =0000000000000053 a4 =0000000030002000 a5 =000000000000000a
a6 =000000000000003f a7 =0000000000000014 s2 =0000000050400270 s3 =000000005021ad8e
s4 =000000005021ad80 s5 =0000000000000030 s6 =000000005021ab52 s7 =0000000000000a00
s8 =0000000050400988 s9 =0000000000000000 s10=0000000000000000 s11=0000000000000023
t3 =000000000000006c t4 =0000000000000068 t5 =0000000000000009 t6 =000000000000002a
priv=S mstatus=0000000a000401a0 cycles=113000467
 mideleg=0000000000000222 mie=0000000000000220 mip=00000000000000a0
raise_exception2: cause=13, tval=0x30002084, pc=0x50200b48
```

riscv_fillpage is here: https://github.com/lupyuen2/wip-pinephone-nuttx/blob/on-demand-paging3/arch/risc-v/src/common/riscv_exception.c#L97-L239

Our NuttX UART Driver (0x50200b48) tries to read the UART Register (0x30002084) but fails (Load Page Fault, cause=13).

Let's track down why mmu_ln_setentry caused the UART I/O to fail...

```text
riscv_fillpage: mmu_ln_setentry1: ptlevel=0x2, ptprev=0x50600000, paddr=0x5060c000, vaddr=0x80001000, MMU_UPGT_FLAGS=0
raise_exception2: cause=13, tval=0x30002084, pc=0x50200b48
```

# TODO

TODO

<p align="center">
<img src="https://raw.githubusercontent.com/apache/nuttx/master/Documentation/_static/NuttX320.png" width="175">
</p>

![POSIX Badge](https://img.shields.io/badge/POSIX-Compliant-brightgreen?style=flat&label=POSIX)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue
)](https://nuttx.apache.org/docs/latest/introduction/licensing.html)
![Issues Tracking Badge](https://img.shields.io/badge/issue_track-github-blue?style=flat&label=Issue%20Tracking)
[![Contributors](https://img.shields.io/github/contributors/apache/nuttx
)](https://github.com/apache/nuttx/graphs/contributors)
[![GitHub Build Badge](https://github.com/apache/nuttx/workflows/Build/badge.svg)](https://github.com/apache/nuttx/actions/workflows/build.yml)
[![Documentation Badge](https://github.com/apache/nuttx/workflows/Build%20Documentation/badge.svg)](https://nuttx.apache.org/docs/latest/index.html)

Apache NuttX is a real-time operating system (RTOS) with an emphasis on
standards compliance and small footprint. Scalable from 8-bit to 64-bit
microcontroller environments, the primary governing standards in NuttX are POSIX
and ANSI standards. Additional standard APIs from Unix and other common RTOSs
(such as VxWorks) are adopted for functionality not available under these
standards, or for functionality that is not appropriate for deeply-embedded
environments (such as fork()).

For brevity, many parts of the documentation will refer to Apache NuttX as simply NuttX.

## Getting Started
First time on NuttX? Read the [Getting Started](https://nuttx.apache.org/docs/latest/quickstart/index.html) guide!
If you don't have a board available, NuttX has its own simulator that you can run on terminal.

## Documentation
You can find the current NuttX documentation on the [Documentation Page](https://nuttx.apache.org/docs/latest/).

Alternatively, you can build the documentation yourself by following the Documentation Build [Instructions](https://nuttx.apache.org/docs/latest/contributing/documentation.html).

The old NuttX documentation is still available in the [Apache wiki](https://cwiki.apache.org/NUTTX/NuttX).

## Supported Boards
NuttX supports a wide variety of platforms. See the full list on the [Supported Platforms](https://nuttx.apache.org/docs/latest/platforms/index.html) page.

## Contributing
If you wish to contribute to the NuttX project, read the [Contributing](https://nuttx.apache.org/docs/latest/contributing/index.html) guidelines for information on Git usage, coding standard, workflow and the NuttX principles.

## License
The code in this repository is under either the Apache 2 license, or a license compatible with the Apache 2 license. See the [License Page](https://nuttx.apache.org/docs/latest/introduction/licensing.html) for more information.
