<br/>
<br/>
<h1 align="center">The Kraken Operating System</h1>
<br/>
<p align="center">
    <img src="docs/kraken.png"/>
</p>
<h2 align="center">
    <a href="README.md">Read Me</a> --
    <a href="LICENSE.md">License</a>  
</h2>

# Let's get *Kraken*!

**Kraken** is a small multi-core operating system built upon a monolithc kernel written in C targeting x86-64. Kraken is built from scratch and is in its early stages of development.
To see what features are currently implemented and what features are planned for the future, refer to the Roadmap section. 

There are two main goals of this project:
1. **Completeness** - I would like to have Kraken support features typically found in relatively modern operating systems. This includes implementing a networking stack, desktop environment, and SMP among other things. In addition, I would like Kraken to run on real hardware and eventually support Aarch64, so that I'm forced to rid the OS of architecture-dependent code.
2. **Learning** - Of course, I learned quite a bit from my OS course in university and we built our own minimal kernels. However, it lacked many features, I had not completely understood many concepts, and I was generally left unsatisfied (... and confused). This project is my attempt to rectify the misunderstandings I had while also learning more about OS development.

# Roadmap
The roadmap is subject to change as I progress further and learn more about the requirements to complete a checkpoint or milestone. The checked boxes are items that are completed and present on the master branch. 

- [ ] **Milestone 1: Humble Beginnings**
    - [x] Setup segmentation via GDT
    - [x] Bitmap-based physical memory manager (PMM)
    - [x] Basic Interrupts
      - [x] Setup IDT
      - [x] Initialize PIC
    - [x] Basic virtual memory capabilities via paging
    - [x] Create a kernel heap
    - [x] Parse ACPI tables
      - [x] MADT
      - [ ] HPET
    - [x] Enable I/O APIC and Local APICs
    - [ ] CGA text mode support
    - [ ] Basic PS2 Keyboard and mouse support
    - [ ] Support for timers
      - [ ] HPET
      - [ ] LAPIC timer (and synchronization)
      - [ ] RTC
      - [ ] PIT
- [ ] **Milestone 2: Into the Userspace**
    - [x] Kernel data structures
      - [x] Vector
      - [x] Queue 
      - [x] Hashmap
    - [ ] Virtual memory manager
      - [ ] Refcounts for paging structures
      - [ ] Virtual memory range allocator
    - [ ] ATA support
    - [ ] System Calls
    - [ ] ELF parsing
    - [ ] Support for Processes
    - [ ] Creating a basic shell
    - [ ] Multiprocessor support
      - [ ] Synchronization constructs
      - [ ] Multiprocessor Scheduling
- [ ] **Milestone 3: Devices Galore**
  - [ ] ACPICA support and device detection
  - [ ] Creating a device tree
  - [ ] PCI support
  - [ ] USB support
  - [ ] SATA support
  - [ ] Framebuffer support
- [ ] **Milestone 4: Above and Beyond**
  - [ ] Libc and POSIX compliance
  - [ ] Virtual Filesystem
  - [ ] Porting Software
    - [ ] Doom
    - [ ] Bim
    - [ ] More incoming (?) ...
  - [ ] Networking stack

# Build
Coming soon ...

# License
<p align="center">
    <img align="right" src="docs/mit.png"/>
</p>

The Kraken operating system and its components are licensed under the MIT License.

Refer to [LICENSE.md](LICENSE.md) or visit this [website](https://opensource.org/licenses/MIT) for the full text of this license.