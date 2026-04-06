
Wayback Machine
106 captures
30 Dec 2018 - 21 Feb 2026
Aug 	SEP 	Nov
	17 	
2020 	2021 	2022
About this capture

Semiconductor & Computer Engineering

     WikiChip 
     Architectures 
     Chips 

Skylake (server) - Microarchitectures - Intel
< intel‎ | microarchitectures

Edit Values
Skylake (server) µarch
General Info
Arch Type	CPU
Designer	Intel
Manufacturer	Intel
Introduction	May 4, 2017
Process	14 nm
Core Configs	4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28
Pipeline
Type	Superscalar
OoOE	Yes
Speculative	Yes
Reg Renaming	Yes
Stages	14-19
Instructions
ISA	x86-64
Extensions	MOVBE, MMX, SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2, POPCNT, AVX, AVX2, AES, PCLMUL, FSGSBASE, RDRND, FMA3, F16C, BMI, BMI2, VT-x, VT-d, TXT, TSX, RDSEED, ADCX, PREFETCHW, CLFLUSHOPT, XSAVE, SGX, MPX, AVX-512
Cache
L1I Cache	32 KiB/core
8-way set associative
L1D Cache	32 KiB/core
8-way set associative
L2 Cache	1 MiB/core
16-way set associative
L3 Cache	1.375 MiB/core
11-way set associative
Cores
Core Names	Skylake X,
Skylake W,
Skylake SP
Succession
Broadwell
Cascade Lake
Contemporary
Skylake (client)

Skylake (SKL) Server Configuration is Intel's successor to Broadwell, an enhanced 14nm+ process microarchitecture for enthusiasts and servers. Skylake succeeded Broadwell. Skylake is the "Architecture" phase as part of Intel's PAO model. The microarchitecture was developed by Intel's R&D center in Haifa, Israel.

For desktop enthusiasts, Skylake is branded Core i7, and Core i9 processors (under the Core X series). For scalable server class processors, Intel branded it as Xeon Bronze, Xeon Silver, Xeon Gold, and Xeon Platinum.

There are a fair number of major differences in the Skylake server configuration vs the client configuration.
Contents

    1 Codenames
    2 Brands
    3 Release Dates
    4 Process Technology
    5 Compatibility
    6 Compiler support
        6.1 CPUID
    7 Architecture
        7.1 Key changes from Broadwell
            7.1.1 CPU changes
            7.1.2 New instructions
        7.2 Block Diagram
            7.2.1 Entire SoC Overview
                7.2.1.1 LCC SoC
                7.2.1.2 HCC SoC
                7.2.1.3 XCC SoC
                7.2.1.4 Individual Core
        7.3 Memory Hierarchy
    8 Overview
    9 Core
        9.1 Overview
            9.1.1 Configurability
        9.2 Pipeline
            9.2.1 Front-end
            9.2.2 Execution engine
                9.2.2.1 Scheduler & 512-SIMD addition
                    9.2.2.1.1 Scheduler Ports & Execution Units
            9.2.3 Memory subsystem
    10 New Technologies
        10.1 Memory Protection Extension (MPX)
        10.2 Key Protection Technology (KPT)
        10.3 Memory Protection Keys for Userspace (PKU)
        10.4 Mode-Based Execute (MBE) Control
    11 Mesh Architecture
        11.1 Organization
        11.2 Cache Coherency
            11.2.1 Sub-NUMA Clustering
    12 Scalability
    13 Integrated Omni-Path
    14 Sockets/Platform
        14.1 Packages
    15 Floorplan
        15.1 Physical Layout
            15.1.1 North Cap
            15.1.2 DDR PHYs
            15.1.3 Layout
            15.1.4 Evolution
    16 Die
        16.1 North Cap
        16.2 Memory PHYs
        16.3 Core Tile
        16.4 Low Core Count (LCC)
        16.5 High Core Count (HCC)
        16.6 Extreme Core Count (XCC)
    17 All Skylake Chips
    18 References
    19 Documents

Codenames

    See also: Client Skylake's Codenames

Core 	Abbrev 	Platform 	Target
Skylake SP 	SKL-SP 	Purley 	Server Scalable Processors
Skylake X 	SKL-X 	Basin Falls 	High-end desktops & enthusiasts market
Skylake W 	SKL-W 	Basin Falls 	Enterprise/Business workstations
Skylake DE 	SKL-DE 		Dense server/edge computing
Brands

    See also: Client Skylake's Brands

New Xeon branding

Intel introduced a number of new server chip families with the introduction of Skylake SP as well as a new enthusiasts family with the introduction of Skylake X.
Logo 	Family 	General Description 	Differentiating Features
Cores 	HT 	AVX 	AVX2 	AVX-512 	TBT 	ECC
core i7 logo (2015).png 	Core i7 	Enthusiasts/High Performance (X) 	6 - 8 	✔ 	✔ 	✔ 	✔ 	✔ 	✘
core i9x logo.png 	Core i9 	Enthusiasts/High Performance 	10 - 18 	✔ 	✔ 	✔ 	✔ 	✔ 	✘
Logo 	Family 	General Description 	Differentiating Features
Cores 	HT 	TBT 	AVX-512 	AVX-512 Units 	UPI links 	Scalability
xeon logo (2015).png 	Xeon D 	Dense servers / edge computing 	4-18 	✔ 	✔ 	✔ 	1 	✘
xeon logo (2015).png 	Xeon W 	Business workstations 	4-18 	✔ 	✔ 	✔ 	2 	✘
xeon bronze (2017).png 	Xeon Bronze 	Entry-level performance /
Cost-sensitive 	6 - 8 	✘ 	✘ 	✔ 	1 	2 	Up to 2
xeon silver (2017).png 	Xeon Silver 	Mid-range performance /
Efficient lower power 	4 - 12 	✔ 	✔ 	✔ 	1 	2 	Up to 2
xeon gold (2017).png 	Xeon Gold 5000 	High performance 	4 - 14 	✔ 	✔ 	✔ 	1 	2 	Up to 4
Xeon Gold 6000 	Higher performance 	6 - 22 	✔ 	✔ 	✔ 	2 	3 	Up to 4
xeon platinum (2017).png 	Xeon Platinum 	Highest performance / flexibility 	4 - 28 	✔ 	✔ 	✔ 	2 	3 	Up to 8
Release Dates

Skylake-based Core X was introduced in May 2017 while Skylake SP was introduced in July 2017.
Process Technology

    Main article: 14 nm lithography process

Unlike mainstream Skylake models, all Skylake server configuration models are fabricated on Intel's enhanced 14+ nm process which is used by Kaby Lake.
Compatibility
Vendor 	OS 	Version 	Notes
Microsoft 	Windows 	Windows Server 2008 	Support
Windows Server 2008 R2
Windows Server 2012
Windows Server 2012 R2
Windows Server 2016
Linux 	Linux 	Kernel 3.19 	Initial Support (MPX support)
Apple 	macOS 	10.12.3 	iMac Pro
Compiler support
Compiler 	Arch-Specific 	Arch-Favorable
ICC 	-march=skylake-avx512 	-mtune=skylake-avx512
GCC 	-march=skylake-avx512 	-mtune=skylake-avx512
LLVM 	-march=skylake-avx512 	-mtune=skylake-avx512
Visual Studio 	/arch:AVX2 	/tune:skylake
CPUID
Core 	Extended
Family 	Family 	Extended
Model 	Model
X, SP, DE, W 	0 	0x6 	0x5 	0x5
Family 6 Model 85
Architecture

Skylake server configuration introduces a number of significant changes from both Intel's previous microarchitecture, Broadwell, as well as the Skylake (client) architecture. Unlike client models, Skylake servers and HEDT models will still incorporate the fully integrated voltage regulator (FIVR) on-die. Those chips also have an entirely new multi-core system architecture that brought a new mesh interconnect network (from ring topology).
Key changes from Broadwell
skylake sp buffer windows.png

    Improved "14 nm+" process (see Kaby Lake § Process Technology)
    Omni-Path Architecture (OPA)
    Mesh architecture (from ring)
        Sub-NUMA Clustering (SNC) support (replaces the Cluster-on-Die (COD) implementation)
    Chipset
        Wellsburg → Lewisburg
        Bus/Interface to Chipset
            DMI 3.0 (from 2.0)
                Increase in transfer rate from 5.0 GT/s to 8.0 GT/s (~3.93GB/s up from 2GB/s) per lane
                Limits motherboard trace design to 7 inches max from (down from 8) from the CPU to chipset
        DMI upgraded to Gen3
    Core
        All the changes from Skylake Client (For full list, see Skylake (Client) § Key changes from Broadwell)
        Front End
            LSD is disabled (Likely due to a bug; see § Front-end for details)
        Back-end
            Port 4 now performs 512b stores (from 256b)
            Port 0 & Port 1 can now be fused to perform AVX-512
            Port 5 now can do full 512b operations (not on all models)
        Memory Subsystem
            Larger store buffer (56 entries, up from 42)
            Page split load penalty reduced 20-fold
            Larger Write-back buffer
            Store is now 64B/cycle (from 32B/cycle)
            Load is now 2x64B/cycle (from 2x32B/cycle)
            New Features
                Adaptive Double Device Data Correction (ADDDC)

    Memory
        L2$
            Increased to 1 MiB/core (from 256 KiB/core)
            Latency increased from 12 to 14
        L3$
            Reduced to 1.375 MiB/core (from 2.5 MiB/core)
            Now non-inclusive (was inclusive)
        DRAM
            hex-channel DDR4-2666 (from quad-channel)

    TLBs
        ITLB
            4 KiB page translations was changed from 4-way to 8-way associative
        STLB
            4 KiB + 2 MiB page translations was changed from 6-way to 12-way associative
        DMI/PEG are now on a discrete clock domain with BCLK sitting on its own domain with full-range granularity (1 MHz intervals)
    Testability
        New support for Direct Connect Interface (DCI), a new debugging transport protocol designed to allow debugging of closed cases (e.g. laptops, embedded) by accessing things such as JTAG through any USB 3 port.

CPU changes

See Skylake (Client) § CPU changes
New instructions

    See also: Client Skylake's New instructions

Skylake server introduced a number of new instructions:

    MPX - Memory Protection Extensions
    XSAVEC - Save processor extended states with compaction to memory
    XSAVES - Save processor supervisor-mode extended states to memory.
    CLFLUSHOPT - Flush & Invalidates memory operand and its associated cache line (All L1/L2/L3 etc..)
    AVX-512, specifically:
        AVX512F - AVX-512 Foundation
        AVX512CD - AVX-512 Conflict Detection
        AVX512BW - AVX-512 Byte and Word
        AVX512DQ - AVX-512 Doubleword and Quadword
        AVX512VL - AVX-512 Vector Length
    PKU - Memory Protection Keys for Userspace
    PCOMMIT - PCOMMIT instruction
    CLWB - Force cache line write-back without flush

Block Diagram
Entire SoC Overview
LCC SoC

    skylake sp lcc block diagram.svg

HCC SoC

    skylake sp hcc block diagram.svg

XCC SoC

    skylake sp xcc block diagram.svg

Individual Core

    skylake server block diagram.svg

Memory Hierarchy
skylake x memory changes.png

Some major organizational changes were done to the cache hierarchy in Skylake server configuration vs Broadwell/Haswell. The memory hierarchy for Skylake's server and HEDT processors has been rebalanced. Note that the L3 is now non-inclusive and some of the SRAM from the L3 cache was moved into the private L2 cache.

    Cache
        L0 µOP cache:
            1,536 µOPs/core, 8-way set associative
                32 sets, 6-µOP line size
                statically divided between threads, inclusive with L1I
        L1I Cache:
            32 KiB/core, 8-way set associative
                64 sets, 64 B line size
                competitively shared by the threads/core
        L1D Cache:
            32 KiB/core, 8-way set associative
            64 sets, 64 B line size
            competitively shared by threads/core
            4 cycles for fastest load-to-use (simple pointer accesses)
                5 cycles for complex addresses
            128 B/cycle load bandwidth
            64 B/cycle store bandwidth
            Write-back policy
        L2 Cache:
            1 MiB/core, 16-way set associative
            64 B line size
            Inclusive
            64 B/cycle bandwidth to L1$
            Write-back policy
            14 cycles latency
        L3 Cache:
            1.375 MiB/core, 11-way set associative, shared across all cores
                Note that a few models have non-default cache sizes due to disabled cores
            2,048 sets, 64 B line size
            Non-inclusive victim cache
            Write-back policy
            50-70 cycles latency
        Snoop Filter (SF):
            2,048 sets, 12-way set associative
    DRAM
        6 channels of DDR4, up to 2666 MT/s
            RDIMM and LRDIMM
            bandwidth of 21.33 GB/s
            aggregated bandwidth of 128 GB/s

Skylake TLB consists of dedicated L1 TLB for instruction cache (ITLB) and another one for data cache (DTLB). Additionally there is a unified L2 TLB (STLB).

    TLBs:
        ITLB
            4 KiB page translations:
                128 entries; 8-way set associative
                dynamic partitioning
            2 MiB / 4 MiB page translations:
                8 entries per thread; fully associative
                Duplicated for each thread
        DTLB
            4 KiB page translations:
                64 entries; 4-way set associative
                fixed partition
            2 MiB / 4 MiB page translations:
                32 entries; 4-way set associative
                fixed partition
            1G page translations:
                4 entries; 4-way set associative
                fixed partition
        STLB
            4 KiB + 2 MiB page translations:
                1536 entries; 12-way set associative. (Note: STLB is incorrectly reported as "6-way" by CPUID leaf 2 (EAX=02H). Skylake erratum SKL148 recommends software to simply ignore that value.)
                fixed partition
            1 GiB page translations:
                16 entries; 4-way set associative
                fixed partition

Overview
skylake server overview.svg

The Skylake server architecture marks a significant departure from the previous decade of multi-core system architecture at Intel. Since Westmere Intel has been using a ring bus interconnect to interlink multiple cores together. As Intel continued to add more I/O, increase the memory bandwidth, and added more cores which increased the data traffic flow, that architecture started to show its weakness. With the introduction of the Skylake server architecture, the interconnect was entirely re-architected to a 2-dimensional mesh interconnect.

A superset model is shown on the right. Skylake-based servers are the first mainstream servers to make use of Intel's new mesh interconnect architecture, an architecture that was previously explored, experimented with, and enhanced with Intel's Phi many-core processors. In this configuration, the cores, caches, and the memory controllers are organized in rows and columns - each with dedicated connections going through each of the rows and columns allowing for a shortest path between any tile, reducing latency, and improving the bandwidth. Those processors are offered from 4 cores up to 28 cores with 8 to 56 threads. In addition to the system-level architectural changes, with Skylake, Intel now has a separate core architecture for those chips which incorporate a plethora of new technologies and features including support for the new AVX-512 instruction set extension.

All models incorporate 6 channels of DDR4 supporting up to 12 DIMMS for a total of 768 GiB (with extended models support 1.5 TiB). For I/O all models incorporate 48x (3x16) lanes of PCIe 3.0. There is an additional x4 lanes PCIe 3.0 reserved exclusively for DMI for the the Lewisburg (LBG) chipset. For a selected number of models, specifically those with F suffix, they have an Omni-Path Host Fabric Interface (HFI) on-package (see Integrated Omni-Path).

Skylake processors are designed for scalability, supporting 2-way, 4-way, and 8-way multiprocessing through Intel's new Ultra Path Interconnect (UPI) interconnect links, with two to three links being offered (see § Scalability). High-end models have node controller support allowing for even higher way configuration (e.g., 32-way multiprocessing).
Core
Overview

Skylake shares most of the development vectors with its predecessor while introducing a one of new constraint. The overall goals were:

    Performance improvements - the traditional way of milking more performance by increasing the instructions per cycle as well as clock frequency.
    Power efficiency - reduction of power for all functional blocks
    Security enhancements - new security features are implemented in hardware in the core
    Configurability 

Configurability
skylake master core configs.svg

Intel has been experiencing a growing divergence in functionality over the last number of iterations of their microarchitecture between their mainstream consumer products and their high-end HPC/server models. Traditionally, Intel has been using the same exact core design for everything from their lowest end value models (e.g. Celeron) all the way up to the highest-performance enterprise models (e.g. Xeon E7). While the two have fundamentally different chip architectures, they use the same exact CPU core architecture as the building block.

This design philosophy has changed with Skylake. In order to better accommodate the different functionalities of each segment without sacrificing features or making unnecessary compromises, Intel went with a configurable core. The Skylake core is a single development project, making up a master superset core. The project results in two derivatives: one for servers (the substance of this article) and one for clients. All mainstream models (from Celeron/Pentium all the way up to Core i7/Xeon E3) use the client core configuration. Server models (e.g. Xeon Gold/Xeon Platinum) are using the new server configuration instead.

The server core is considerably larger than the client one, featuring Advanced Vector Extensions 512 (AVX-512). Skylake servers support what was formerly called AVX3.2 (AVX512F + AVX512CD + AVX512BW + AVX512DQ + AVX512VL). The server core also incorporates a number of new technologies not found in the client configuration. In addition to the execution units that were added, the cache hierarchy has changed for the server core as well, incorporating a large L2 and a portion of the LLC as well as the caching and home agent and the snoop filter that needs to accommodate the new cache changes.

Below is a visual that helps show how the server core was evolved from the client core.

    skylake sp mesh core tile zoom with client shown.png

Pipeline

The Skylake core focuses on extracting performance and reducing power through a number of key ways. Intel builds Skylake on previous microarchitectures, descendants of Sandy Bridge. For the core to increase the overall performance, Intel focused on extracting additional parallelism.
Front-end

For the most part, with the exception of the LSD, the front-end of the Skylake server core is identical to the client configuration. For in-depth detail of the Skylake front-end see Skylake (client) § Front-end.

The only major difference in the front-end from the client core configuration is the LSD. The Loop Stream Detector (LSD) has been disabled. While the exact reason is not known, it might be related to a severe issue that was experienced by the OCaml Development Team. The issue was patched via microcode on the client platform, however this change might indicate it was possibly disabled on there as well. The exact implications of this are unknown.
Execution engine

The Skylake server configuration core back-end is identical to the client configuration up to the scheduler. For in-depth detail of the Skylake back-end up to that point, see Skylake (client) § Execution engine.
Scheduler & 512-SIMD addition
skylake scheduler server.svg

The scheduler itself was increased by 50%; with up to 97 entries (from 64 in Broadwell) being competitively shared between the two threads. Skylake continues with a unified design; this is in contrast to designs such as AMD's Zen which uses a split design each one holding different types of µOPs. Scheduler includes the two register files for integers and vectors. It's in those register files that output operand data is store. In Skylake, the integer register file was also slightly increased from 160 entries to 180.
skylake sp added cach and vpu.png

This is the first implementation to incorporate AVX-512, a 512-bit SIMD x86 instruction set extension. AVX-512 operations can take place on every port. For 512-bit wide FMA SIMD operations, Intel introduced two different mechanisms ways:

In the simple implementation, the variants used in the entry-level and mid-range Xeon servers, AVX-512 fuses Port 0 and Port 1 to form a 512-bit FMA unit. Since those two ports are 256-wide, an AVX-512 option that is dispatched by the scheduler to port 0 will execute on both ports. Note that unrelated operations can still execute in parallel. For example, an AVX-512 operation and an Int ALU operation may execute in parallel - the AVX-512 is dispatched on port 0 and use the AVX unit on port 1 as well and the Int ALU operation will execute independently in parallel on port 1.

In the high-end and highest performance Xeons, Intel added a second dedicated 512-bit wide AVX-512 FMA unit in addition to the fused Port0-1 operations described above. The dedicated unit is situated on Port 5.

Physically, Intel added 768 KiB L2 cache and the second AVX-512 VPU externally to the core.
Scheduler Ports & Execution Units
Scheduler Ports Designation
Port 0	Integer/Vector Arithmetic, Multiplication, Logic, Shift, and String ops	512-bit Vect ALU/Shift/Mul/FMA
FP Add, Multiply, FMA
Integer/FP Division and Square Root
AES Encryption
Branch2
Port 1	Integer/Vector Arithmetic, Multiplication, Logic, Shift, and Bit Scanning
FP Add, Multiply, FMA
Port 5	Integer/Vector Arithmetic, Logic	512-bit Vect ALU/Shift/Mul/FMA
Vector Permute
x87 FP Add, Composite Int, CLMUL
Port 6	Integer Arithmetic, Logic, Shift
Branch
Port 2	Load, AGU
Port 3	Load, AGU
Port 4	Store, AGU
Port 7	AGU
Execution Units
Execution Unit 	# of Units 	Instructions
ALU 	4 	add, and, cmp, or, test, xor, movzx, movsx, mov, (v)movdqu, (v)movdqa, (v)movap*, (v)movup*
DIV 	1 	divp*, divs*, vdiv*, sqrt*, vsqrt*, rcp*, vrcp*, rsqrt*, idiv
Shift 	2 	sal, shl, rol, adc, sarx, adcx, adox, etc...
Shuffle 	1 	(v)shufp*, vperm*, (v)pack*, (v)unpck*, (v)punpck*, (v)pshuf*, (v)pslldq, (v)alignr, (v)pmovzx*, vbroadcast*, (v)pslldq, (v)psrldq, (v)pblendw
Slow Int 	1 	mul, imul, bsr, rcl, shld, mulx, pdep, etc...
Bit Manipulation 	2 	andn, bextr, blsi, blsmsk, bzhi, etc
FP Mov 	1 	(v)movsd/ss, (v)movd gpr
SIMD Misc 	1 	STTNI, (v)pclmulqdq, (v)psadw, vector shift count in xmm
Vec ALU 	3 	(v)pand, (v)por, (v)pxor, (v)movq, (v)movq, (v)movap*, (v)movup*, (v)andp*, (v)orp*, (v)paddb/w/d/q, (v)blendv*, (v)blendp*, (v)pblendd
Vec Shift 	2 	(v)psllv*, (v)psrlv*, vector shift count in imm8
Vec Add 	2 	(v)addp*, (v)cmpp*, (v)max*, (v)min*, (v)padds*, (v)paddus*, (v)psign, (v)pabs, (v)pavgb, (v)pcmpeq*, (v)pmax, (v)cvtps2dq, (v)cvtdq2ps, (v)cvtsd2si, (v)cvtss2si
Vec Mul 	2 	(v)mul*, (v)pmul*, (v)pmadd*
This table was taken verbatim from the Intel manual. Execution unit mapping to MMX instructions are not included.
Memory subsystem
skylake-sp memory.svg

Skylake's memory subsystem is in charge of the loads and store requests and ordering. Since Haswell, it's possible to sustain two memory reads (on ports 2 and 3) and one memory write (on port 4) each cycle. Each memory operation can be of any register size up to 512 bits. Skylake memory subsystem has been improved. The store buffer has been increased by 42 entries from Broadwell to 56 for a total of 128 simultaneous memory operations in-flight or roughly 60% of all µOPs. Special care was taken to reduce the penalty for page-split loads; previously scenarios involving page-split loads were thought to be rarer than they actually are. This was addressed in Skylake with page-split loads are now made equal to other splits loads. Expect page split load penalty down to 5 cycles from 100 cycles in Broadwell. The average latency to forward a load to store has also been improved and stores that miss in the L1$ generate L2$ requests to the next level cache much earlier in Skylake than before.

The L2 to L1 bandwidth in Skylake is the same as Haswell at 64 bytes per cycle in either direction. Note that one operation can be done each cycle; i.e., the L1 can either receive data from the L1 or send data to the Load/Store buffers each cycle, but not both. Latency from L2$ to L3$ has also been increased from 4 cycles/line to 2 cycles/line.

The medium level cache (MLC) and last level cache (LLC) was rebalanced. Traditionally, Intel had a 256 KiB L2 cache which was duplicated along with the L1s over in the LLC which was 2.5 MiB. That is, prior to Skylake, the 256 KiB L2 cache actually took up 512 KiB of space for a total of 2.25 mebibytes effective cache per core. In Skylake Intel doubled the L2 and quadrupled the effective capacity to 1 MiB while decreasing the LLC to 1.375 MiB. The LLC is also now made non-inclusive, i.e., the L2 may or may not be in the L3 (no guarantee is made); what stored where will depend on the particular access pattern of the executing application, the size of code and data accessed, and the inter-core sharing behavior. Having an inclusive L3 makes cache coherence considerably easier to implement. Snooping only requires checking the L3 cache tags to know if the data is on board and in which core. It also makes passing data around a bit more efficient. It's currently unknown what mechanism is being used to reduce snooping. In the past, Intel has discussed a couple of additional options they were researching such as NCID (non-inclusive cache, inclusive directory architecture). It's possible that a NCID is being used in Skylake or a related derivative. These changes also mean that software optimized for data placing in the various caches needs to be revised for the new changes, particularly in situations where data is not shared, the overall capacity can be treated as L2+L3 for a total of 2.375 MiB.
New Technologies
Memory Protection Extension (MPX)

    Main article: Intel's Memory Protection Extension

Memory Protection Extension (MPX) is a new x86 extension that offers a hardware-level bound checking implementation. This extension allows an application to define memory boundaries for allocated memory areas. The processors can then check all proceeding memory accesses against those boundaries to ensure accesses are not out of bound. A program accessing a boundary-marked buffer out of buffer will generate an exception.
Key Protection Technology (KPT)

Key Protection Technology (KPT) is designed to help secure sensitive private keys in hardware at runtime. KPT augments QuickAssist Technology (QAT) hardware crypto accelerators with run-time storage of private keys using Intel's existing Platform Trust Technology (PTT), thereby allowing high throughput hardware security acceleration. The QAT accelerators are all integrated onto Intel's new Lewisburg chipset along with the Converged Security Manageability Engine (CSME) which implements Intel's PTT. The CSME is linked through a private hardware link that is invisible to x86 software and simple hardware probes.
Memory Protection Keys for Userspace (PKU)

Memory Protection Keys for Userspace (PKU also PKEYs) is an extension that provides a mechanism for enforcing page-based protections - all without requiring modification of the page tables when an application changes protection domains. PKU introduces 16 keys by re-purposing the 4 ignored bits from the page table entry.
Mode-Based Execute (MBE) Control

Mode-Based Execute (MBE) is an enhancement to the Extended Page Tables (EPT) that provides finer level of control of execute permissions. With MBE the previous Execute Enable (X) bit is turned into Execute Userspace page (XU) and Execute Supervisor page (XS). The processor selects the mode based on the guest page permission. With proper software support, hypervisors can take advantage of this as well to ensure integrity of kernel-level code.
Mesh Architecture

    Main article: Intel's Mesh Interconnect Architecture

skylake sp xcc die config.png

On the previous number of generations, Intel has been adding cores onto the die and connecting them via a ring architecture. This was sufficient until recently. With each generation, the added cores increased the access latency while lowering the available bandwidth per core. Intel mitigated this problem by splitting up the die into two halves each on its own ring. This reduced hopping distance and added additional bandwidth but it did not solve the growing fundamental inefficiencies of the ring architecture.

This was completely addressed with the new mesh architecture that is implemented in the Skylake server processors. The mesh consists of a 2-dimensional array of half rings going in the vertical and horizontal directions which allow communication to take the shortest path to the correct node. The new mesh architecture implements a modular design for the routing resources in order to remove the various bottlenecks. That is, the mesh architecture now integrates the caching agent, the home agent, and the IO subsystem on the mesh interconnect distributed across all the cores. Each core now has its own associated LLC slice as well as the snooping filter and the Caching and Home Agent (CHA). Additional nodes such as the two memory controllers, the Ultra Path Interconnect (UPI) nodes and PCIe are not independent node on the mesh as well and they now behave identically to any other node/core in the network. This means that in addition to the performance increase expected from core-to-core and core-to-memory latency, there should be a substantial increase in I/O performance. The CHA which is found on each of the LLC slices now maps addresses being accessed to the specific LLC bank, memory controller, or I/O subsystem. This provides the necessary information required for the routing to take place.
Organization
skylake (server) half rings.png

Each die has a grid of converged mesh stops (CMS). For example, for the XCC die, there are 36 CMSs. As the name implies, the CMS is a block that effectively interfaces between all the various subsystems and the mesh interconnect. The locations of the CMSes for the large core count is shown on the diagram below. It should be pointed that although the CMS appears to be inside the core tiles, most of the mesh is likely routed above the cores in a similar fashion to how Intel has done it with the ring interconnect which was wired above the caches in order reduce the die area.


    skylake server cms units.svg


Each core tile interfaces with the mesh via its associated converged mesh stop (CMS). The CMSs at the very top are for the UPI links and PCIe links to interface with the mesh we annotated on the previous page. Additionally, the two integrated memory controllers have their own CMS they use to interface with the mesh as well.

Every stop at each tile is directly connected to its immediate four neighbors – north, south, east, and west.


        skylake sp cms links.svg


Every vertical column of CMSs form a bi-directional half ring. Similarly, every horizontal row forms a bi-directional half ring.


        skylake sp mesh half rings.png


Cache Coherency

Given the new mesh architecture, new tradeoffs were involved. The new UPI inter-socket links are a valuable resource that could bottlenecked when flooded with unnecessary cross-socket snoop requests. There's also considerably higher memory bandwidth with Skylake which can impact performance. As a compromise, the previous four snoop modes (no-snoop, early snoop, home snoop, and directory) have been reduced to just directory-base coherency. This also alleviates the implementation complexity (which is already complex enough in itself).
snc clusters.png

It should be pointed out that the directory-base coherency optimizations that were done in previous generations have been furthered improved with Skylake - particularly OSB, HitME cache, IO directory cache. Skylake maintained support for Opportunistic Snoop Broadcast (OSB) which allows the network to opportunistically make use of the UPI links when idle or lightly loaded thereby avoiding an expensive memory directory lookup. With the mesh network and distributed CHAs, HitME is now distributed and scales with the CHAs, enhancing the speeding up of cache-to-cache transfers (Those are your migratory cache lines that frequently get transferred between nodes). Specifically for I/O operations, the I/O directory cache (IODC), which was introduced with Haswell, improves stream throughput by eliminating directory reads for InvItoE from snoop caching agent. Previously this was implemented as a 64-entry directory cache to complement the directory in memory. In Skylake, with a distributed CHA at each node, the IODC is implemented as an eight-entry directory cache per CHA.
Sub-NUMA Clustering

In previous generations Intel had a feature called cluster-on-die (COD) which was introduced with Haswell. With Skylake, there's a similar feature called sub-NUMA cluster (SNC). With a memory controller physically located on each side of the die, SNC allows for the creation of two localized domains with each memory controller belonging to each domain. The processor can then map the addresses from the controller to the distributed home agents and LLC in its domain. This allows executing code to experience lower LLC and memory latency within its domain compared to accesses outside of the domain.

It should be pointed out that in contrast to COD, SNC has a unique location for every address in the LLC and is never duplicated across LLC banks (previously, COD cache lines could have copies). Additionally, on multiprocessor systems, addresses mapped to memory on remote sockets are still uniformly distributed across all LLC banks irrespective of the localized SNC domain.
Scalability

    See also: QuickPath Interconnect and Ultra Path Interconnect

In the last couple of generations, Intel has been utilizing QuickPath Interconnect (QPI) which served as a high-speed point-to-point interconnect. QPI has been replaced by the Ultra Path Interconnect (UPI) which is higher-efficiency coherent interconnect for scalable systems, allowing multiple processors to share a single shared address space. Depending on the exact model, each processor can have either two or three UPI links connecting to the other processors.

UPI links eliminate some of the scalability limitations that surfaced in QPI over the past few microarchitecture iterations. They use directory-based home snoop coherency protocol and operate at up either 10.4 GT/s or 9.6 GT/s. This is quite a bit different from previous generations. In addition to the various improvements done to the protocol layer, Skylake SP now implements a distributed CHA that is situated along with the LLC bank on each core. It's in charge of tracking the various requests from the core as well as responding to snoop requests from both local and remote agents. The ease of distributing the home agent is a result of Intel getting rid of the requirement on preallocation of resources at the home agent. This also means that future architectures should be able to scale up well.

Depending on the exact model, Skylake processors can scale from 2-way all the way up to 8-way multiprocessing. Note that the high-end models that support 8-way multiprocessing also only come with three UPI links for this purpose while the lower end processors can have either two or three UPI links. Below are the typical configurations for those processors.


2-way SMP; 2 UPI links

skylake sp 2-way 2 upi.svg
2-way SMP; 3 UPI links

skylake sp 2-way 3 upi.svg


4-way SMP; 2 UPI links

skylake sp 4-way 2 upi.svg
4-way SMP; 3 UPI links

skylake sp 4-way 3 upi.svg


8-way SMP; 3 UPI links

skylake sp 8-way 3 upi.svg
Integrated Omni-Path

    See also: Intel's Omni-Path

IFT (Internal Faceplate Transition) Carrier

A number of Skylake SP models (specifically those with the "F" suffix) incorporate the Omni-Path Host Fabric Interface (HFI) on-package. This was previously done with the Knights Landing ("F" suffixed) models. This, in addition to improving cost and power efficiencies, also eliminates the dependency on the x16 PCIe lanes on the motherboard. With the HFI on package, the chip can be plugged in directly to the IFT (Internal Faceplate Transition) carrier via a separate IFP (Internal Faceplate-to-Processor) 1-port cable (effectively a Twinax cable).


        skylake sp with hfi to carrier.png


Regardless of the model, the integrated fabric die has a TDP of 8 Watts (note that this value is already included in the model's TDP value).
Sockets/Platform

Both Skylake X and PS are a two-chip solution linked together via Intel's standard DMI 3.0 bus interface which utilizes 4 PCIe 3.0 lanes (having a transfer rate of 8 GT/s per lane). Skylake SP has additional SMP capabilities which utilizes either 2 or 3 (depending on the model) Ultra Path Interconnect (UPI) links.
	Core 	Socket 	Permanent 	Platform 	Chipset 	Chipset Bus 	SMP Interconnect
skylake x (back).png 	Skylake X 	LGA-2066 	No 	2-chip 	Lewisburg 	DMI 3.0 	✘
  	Skylake SP 	LGA-3647 	2-chip + 2-8-way SMP 	UPI
Packages
Core 	Die Type 	Package 	Dimensions
Skylake SP 	LCC 	FCLGA-3647 	76.16 mm x 56.6 mm
HCC
XCC
Skylake X 	LCC 	FCLGA-2066 	58.5 mm x 51 mm
HCC
Floorplan
skylake sp major blocks.svg

All Skylake server dies consist of three major blocks:

    DDR PHYs
    North Cap
    Mesh Tiles

Those blocks are found on all die configuration and form the base for Intel's highly configurable floorplan. Depending on the market segment and model specification targets, Intel can add and remove rows of tiles.
XCC Die
skylake (server) die major blocks (xcc).png
HCC Die
skylake (server) die major blocks (hcc).png
Physical Layout
North Cap

The North Cap at the very top of the die contains all the I/O agents and PHYs as well as serial IP ports, and the fuse unit. For the most part this configuration largely the same for all the dies. For the smaller dies, the extras are removed (e.g., the in-package PCIe link is not needed).

At the very top of the North Cap are the various I/O connectivity. There are a total of 128 high-speed I/O lanes – 3×16 (48) PCIe lanes operating at 8 GT/s, x4 DMI lanes for hooking up the Lewisburg chipset, 16 on-package PCIe lanes (operating at 2.5/5/8 GT/s), and 3×20 (60) Ultra-Path Interconnect (UPI) lanes operating at 10.4 GT/s for the multiprocessing support.

At the south-west corner of the North Cap is the clock generator unit (CGU) and the Global Power Management Unit (Global PMU). The CGU contains an all-digital (AD) filter phase-locked loops (PLL) and an all-digital uncore PLL. The filter ADPLL is dedicated to the generation of all on-die reference clock used for all the core PLLs and one uncore PLL. The power management unit also has its own dedicated all-digital PLL.

At the bottom part of the North Cap are the Mesh stops for the various I/O to interface with the Mesh.
DDR PHYs

There are the two DDR4 PHYs which are identical for all the dies (albeit in the low-end models, the extra channel is simply disabled). There are two independent and identical physical sections of 3 DDR4 channels each which reside on the east and west edges of the die. Each channel is 72-bit (64 bit and an 8-bit ECC), supporting 2-DIMM per channel with a data rate of up to 2666 MT/s for a bandwidth of 21.33 GB/s and an aggregated bandwidth of 128 GB/s. RDIMM and LRDIMM are supported.

The location of the PHYs was carefully chosen in order to ease the package design, specifically, they were chosen in order to maintain escape routing and pin-out order matching between the CPU and the DIMM slots to shorten package and PCB routing length in order to improve signal integrity.
Layout

    skylake (server) die area layout.svg

Evolution

The original Skylake large die started out as a 5 by 5 core tile (25 tiles, 25 cores) as shown by the image from Intel on the left side. The memory controllers were next to the PHYs on the east and west side. An additional row was inserted to get to a 5 by 6 grid. Two core tiles one from each of the sides was then replaced by the new memory controller module which can interface with the mesh just like any other core tile. The final die is shown in the image below as well on the right side.

    skylaake server layout evoluation.png

Die

    See also: Client Skylake's Die

Skylake SP chips and wafer.

Skylake Server class models and high-end desktop (HEDT) consist of 3 different dies:

    12 tiles (3x4), 10-core, Low Core Count (LCC)
    20 tiles (5x4), 18-core, High Core Count (HCC)
    30 tiles (5x6), 28-core, Extreme Core Count (XCC)

North Cap

HCC:

    skylake (server) northcap (hcc).png

    skylake (server) northcap (hcc) (annotated).png

XCC:

    skylake (server) northcap (xcc).png

    skylake (server) northcap (xcc) (annotated).png


Memory PHYs

Data bytes are located on the north and south sub-sections of the channel layout. Command, Control, Clock signals, and process, supply voltage, and temperature (PVT) compensation circuitry are located in the middle section of the channels.

    skylake sp memory phys (annotated).png

Core Tile

    ~4.8375 x 3.7163
    ~ 17.978 mm² die area

    skylake sp core.png

    skylake sp mesh core tile zoom.png

Low Core Count (LCC)

    14 nm process
    12 metal layers
    ~22.26 mm x ~14.62 mm
    ~325.44 mm² die size
    10 cores
    12 tiles (3x4)


    (NOT official die shot, artist's rendering based on the larger die)
    skylake lcc die shot.jpg

High Core Count (HCC)

Die shot of the octadeca core HEDT Skylake X processors.

    14 nm process
    13 metal layers
    ~485 mm² die size (estimated)
    18 cores
    20 tiles (5x4)

    skylake (octadeca core).png


    skylake (octadeca core) (annotated).png

Extreme Core Count (XCC)

    14 nm process
    13 metal layers
    ~694 mm² die size (estimated)
    28 cores
    30 tiles (5x6)


    skylake-sp xcc die shot.png


    skylake-sp xcc die shot (annotated).png

All Skylake Chips
 	List of Skylake Processors
 	Main processor	Frequency/Turbo	Mem	Major Feature Diff
Model	Launched	Price	Family	Core Name	Cores	Threads	L2$	L3$	TDP	Frequency	Max Turbo	Max Mem	Turbo	SMT
 	Uniprocessors
i7-7800X	26 June 2017	$ 389.00	Core i7	Skylake X	6	12	6 MiB	8.25 MiB	140 W	3.5 GHz	4 GHz	128 GiB	✔	✔
i7-7820X	26 June 2017	$ 599.00	Core i7	Skylake X	8	16	8 MiB	11 MiB	140 W	3.6 GHz	4.3 GHz	128 GiB	✔	✔
i7-9800X	November 2018	$ 589.00	Core i7	Skylake X Refresh	8	16	8 MiB	16.5 MiB	165 W	3.8 GHz	4.4 GHz	128 GiB	✔	✔
i9-7900X	26 June 2017	$ 999.00	Core i9	Skylake X	10	20	10 MiB	13.75 MiB	140 W	3.3 GHz	4.3 GHz	128 GiB	✔	✔
i9-7920X	28 August 2017	$ 1,199.00	Core i9	Skylake X	12	24	12 MiB	16.5 MiB	140 W	2.9 GHz	4.3 GHz	128 GiB	✔	✔
i9-7940X	25 September 2017	$ 1,399.00	Core i9	Skylake X	14	28	14 MiB	19.25 MiB	165 W	3.1 GHz	4.3 GHz	128 GiB	✔	✔
i9-7960X	25 September 2017	$ 1,699.00	Core i9	Skylake X	16	32	16 MiB	22 MiB	165 W	2.8 GHz	4.2 GHz	128 GiB	✔	✔
i9-7980XE	25 September 2017	$ 1,999.00	Core i9	Skylake X	18	36	18 MiB	24.75 MiB	165 W	2.6 GHz	4.2 GHz	128 GiB	✔	✔
i9-9820X	November 2018	$ 898.00	Core i9	Skylake X Refresh	10	20	10 MiB	16.5 MiB	165 W	3.3 GHz	4.1 GHz	128 GiB	✔	✔
i9-9900X	November 2018	$ 989.00	Core i9	Skylake X Refresh	10	20	10 MiB	19.25 MiB	165 W	3.5 GHz	4.4 GHz	128 GiB	✔	✔
i9-9920X	November 2018	$ 1,189.00	Core i9	Skylake X Refresh	12	24	12 MiB	19.25 MiB	165 W	3.5 GHz	4.4 GHz	128 GiB	✔	✔
i9-9940X	November 2018	$ 1,387.00	Core i9	Skylake X Refresh	14	28	14 MiB	19.25 MiB	165 W	3.3 GHz	4.4 GHz	128 GiB	✔	✔
i9-9960X	November 2018	$ 1,684.00	Core i9	Skylake X Refresh	16	32	16 MiB	22 MiB	165 W	3.1 GHz	4.4 GHz	128 GiB	✔	✔
i9-9980XE	November 2018	$ 1,979.00	Core i9	Skylake X Refresh	18	36	18 MiB	24.75 MiB	165 W	3 GHz	4.4 GHz	128 GiB	✔	✔
i9-9990XE	3 January 2019		Core i9	Skylake X Refresh	14	28	14 MiB	19.25 MiB	255 W	4 GHz	5 GHz	128 GiB	✔	✔
D-2123IT	7 February 2018	$ 213.00	Xeon D	Skylake DE	4	8	4 MiB	8.25 MiB	60 W	2.2 GHz	3 GHz	512 GiB	✔	✔
D-2141I	7 February 2018	$ 555.00	Xeon D	Skylake DE	8	16	8 MiB	11 MiB	65 W	2.2 GHz	3 GHz	512 GiB	✔	✔
D-2142IT	7 February 2018	$ 438.00	Xeon D	Skylake DE	8	16	8 MiB	11 MiB	65 W	1.9 GHz	3 GHz	512 GiB	✔	✔
D-2143IT	7 February 2018	$ 566.00	Xeon D	Skylake DE	8	16	8 MiB	11 MiB	65 W	2.2 GHz	3 GHz	512 GiB	✔	✔
D-2145NT	7 February 2018	$ 502.00	Xeon D	Skylake DE	8	16	8 MiB	11 MiB	65 W	1.9 GHz	3 GHz	512 GiB	✔	✔
D-2146NT	7 February 2018	$ 641.00	Xeon D	Skylake DE	8	16	8 MiB	11 MiB	80 W	2.3 GHz	3 GHz	512 GiB	✔	✔
D-2161I	7 February 2018	$ 962.00	Xeon D	Skylake DE	12	24	12 MiB	16.5 MiB	90 W	2.2 GHz	3 GHz	512 GiB	✔	✔
D-2163IT	7 February 2018	$ 930.00	Xeon D	Skylake DE	12	24	12 MiB	16.5 MiB	75 W	2.1 GHz	3 GHz	512 GiB	✔	✔
D-2166NT	7 February 2018	$ 1,005.00	Xeon D	Skylake DE	12	24	12 MiB	16.5 MiB	85 W	2 GHz	3 GHz	512 GiB	✔	✔
D-2173IT	7 February 2018	$ 1,229.00	Xeon D	Skylake DE	14	28	14 MiB	19.25 MiB	70 W	1.7 GHz	3 GHz	512 GiB	✔	✔
D-2177NT	7 February 2018	$ 1,443.00	Xeon D	Skylake DE	14	28	14 MiB	19.25 MiB	105 W	1.9 GHz	3 GHz	512 GiB	✔	✔
D-2183IT	7 February 2018	$ 1,764.00	Xeon D	Skylake DE	16	32	16 MiB	22 MiB	100 W	2.2 GHz	3 GHz	512 GiB	✔	✔
D-2187NT	7 February 2018	$ 1,989.00	Xeon D	Skylake DE	16	32	16 MiB	22 MiB	110 W	2 GHz	3 GHz	512 GiB	✔	✔
D-2191	7 February 2018	$ 2,406.00	Xeon D	Skylake DE	18	36	18 MiB	24.75 MiB	86 W	1.6 GHz	3 GHz	512 GiB	✔	✔
W-2102	29 August 2017	$ 202.00	Xeon W	Skylake W	4	4	4 MiB	8.25 MiB	120 W	2.9 GHz		512 GiB	✘	✘
W-2104	29 August 2017	$ 255.00	Xeon W	Skylake W	4	4	4 MiB	8.25 MiB	120 W	3.2 GHz		512 GiB	✘	✘
W-2123	29 August 2017	$ 294.00	Xeon W	Skylake W	4	8	4 MiB	8.25 MiB	120 W	3.6 GHz	3.9 GHz	512 GiB	✔	✔
W-2125	29 August 2017	$ 444.00	Xeon W	Skylake W	4	8	4 MiB	8.25 MiB	120 W	4 GHz	4.5 GHz	512 GiB	✔	✔
W-2133	29 August 2017	$ 617.00	Xeon W	Skylake W	6	12	6 MiB	8.25 MiB	140 W	3.6 GHz	3.9 GHz	512 GiB	✔	✔
W-2135	29 August 2017	$ 835.00	Xeon W	Skylake W	6	12	6 MiB	8.25 MiB	140 W	3.7 GHz	4.5 GHz	512 GiB	✔	✔
W-2140B	21 December 2017		Xeon W	Skylake W	8	16	8 MiB	11 MiB		3.2 GHz	4.2 GHz	512 GiB	✔	✔
W-2145	29 August 2017	$ 1,113.00	Xeon W	Skylake W	8	16	8 MiB	11 MiB	140 W	3.7 GHz	4.5 GHz	512 GiB	✔	✔
W-2150B	21 December 2017		Xeon W	Skylake W	10	20	10 MiB	13.75 MiB		3 GHz	4.5 GHz	512 GiB	✔	✔
W-2155	29 August 2017	$ 1,440.00	Xeon W	Skylake W	10	20	10 MiB	13.75 MiB	140 W	3.3 GHz	4.5 GHz	512 GiB	✔	✔
W-2170B	21 December 2017		Xeon W	Skylake W	14	28	14 MiB	19.25 MiB		2.5 GHz	4.3 GHz	512 GiB	✔	✔
W-2175	29 August 2017	$ 1,947.00	Xeon W	Skylake W	14	28	14 MiB	19.25 MiB	140 W	2.5 GHz	4.3 GHz	512 GiB	✔	✔
W-2191B	21 December 2017		Xeon W	Skylake W	18	36	18 MiB	24.75 MiB		2.3 GHz	4.3 GHz	512 GiB	✔	✔
W-2195	29 August 2017	$ 2,553.00	Xeon W	Skylake W	18	36	18 MiB	24.75 MiB	140 W	2.3 GHz	4.3 GHz	512 GiB	✔	✔
W-3175X	30 January 2019	$ 2,999.00	Xeon W	Skylake SP	28	56	28 MiB	38.5 MiB	255 W	3.1 GHz	4.3 GHz	512 GiB	✔	✔
 	Multiprocessors (2-way)
3104	11 July 2017	$ 213.00	Xeon Bronze	Skylake SP	6	6	6 MiB	8.25 MiB	85 W	1.7 GHz		768 GiB	✘	✘
3106	11 July 2017	$ 306.00	Xeon Bronze	Skylake SP	8	8	8 MiB	11 MiB	85 W	1.7 GHz		768 GiB	✘	✘
6138P	16 May 2018	$ 4,937.00	Xeon Gold	Skylake SP	20	40	20 MiB	27.5 MiB	195 W	2 GHz	3.7 GHz	768 GiB	✔	✔
4108	11 July 2017	$ 417.00	Xeon Silver	Skylake SP	8	16	8 MiB	11 MiB	85 W	1.8 GHz	3 GHz	768 GiB	✔	✔
4109T	11 July 2017	$ 501.00	Xeon Silver	Skylake SP	8	16	8 MiB	11 MiB	70 W	2 GHz	3 GHz	768 GiB	✔	✔
4110	11 July 2017	$ 501.00	Xeon Silver	Skylake SP	8	16	8 MiB	11 MiB	85 W	2.1 GHz	3 GHz	768 GiB	✔	✔
4112	11 July 2017	$ 473.00	Xeon Silver	Skylake SP	4	8	4 MiB	8.25 MiB	85 W	2.6 GHz	3 GHz	768 GiB	✔	✔
4114	11 July 2017	$ 694.00	Xeon Silver	Skylake SP	10	20	10 MiB	13.75 MiB	85 W	2.2 GHz	3 GHz	768 GiB	✔	✔
4114T	11 July 2017		Xeon Silver	Skylake SP	10	20	10 MiB	13.75 MiB	85 W	2.2 GHz	3 GHz	768 GiB	✔	✔
4116	11 July 2017	$ 1,002.00	Xeon Silver	Skylake SP	12	24	12 MiB	16.5 MiB	85 W	2.1 GHz	3 GHz	768 GiB	✔	✔
4116T	11 July 2017		Xeon Silver	Skylake SP	12	24	12 MiB	16.5 MiB	85 W	2.1 GHz	3 GHz	768 GiB	✔	✔
 	Multiprocessors (4-way)
5115	11 July 2017	$ 1,221.00	Xeon Gold	Skylake SP	10	20	10 MiB	13.75 MiB	85 W	2.4 GHz	3.2 GHz	768 GiB	✔	✔
5117	11 July 2017		Xeon Gold	Skylake SP	14	28	14 MiB	19.25 MiB	105 W	2 GHz	2.8 GHz	768 GiB	✔	✔
5117F	11 July 2017		Xeon Gold	Skylake SP	14	28	14 MiB	19.25 MiB	113 W	2 GHz	2.8 GHz	768 GiB	✔	✔
5118	11 July 2017	$ 1,273.00	Xeon Gold	Skylake SP	12	24	12 MiB	16.5 MiB	105 W	2.3 GHz	3.2 GHz	768 GiB	✔	✔
5119T	11 July 2017		Xeon Gold	Skylake SP	14	28	14 MiB	19.25 MiB	85 W	1.9 GHz	3.2 GHz	768 GiB	✔	✔
5120	11 July 2017	$ 1,555.00	Xeon Gold	Skylake SP	14	28	14 MiB	19.25 MiB	105 W	2.2 GHz	3.2 GHz	768 GiB	✔	✔
5120T	11 July 2017	$ 1,727.00	Xeon Gold	Skylake SP	14	28	14 MiB	19.25 MiB	105 W	2.2 GHz	3.2 GHz	768 GiB	✔	✔
5122	11 July 2017	$ 1,221.00	Xeon Gold	Skylake SP	4	8	4 MiB	16.5 MiB	105 W	3.6 GHz	3.7 GHz	768 GiB	✔	✔
6126	11 July 2017	$ 1,776.00	Xeon Gold	Skylake SP	12	24	12 MiB	19.25 MiB	125 W	2.6 GHz	3.7 GHz	768 GiB	✔	✔
6126F	11 July 2017	$ 1,931.00	Xeon Gold	Skylake SP	12	24	12 MiB	19.25 MiB	135 W	2.6 GHz	3.7 GHz	768 GiB	✔	✔
6126T	11 July 2017	$ 1,865.00	Xeon Gold	Skylake SP	12	24	12 MiB	19.25 MiB	125 W	2.6 GHz	3.7 GHz	768 GiB	✔	✔
6128	11 July 2017	$ 1,697.00	Xeon Gold	Skylake SP	6	12	6 MiB	19.25 MiB	115 W	3.4 GHz	3.7 GHz	768 GiB	✔	✔
6130	11 July 2017	$ 1,894.00	Xeon Gold	Skylake SP	16	32	16 MiB	22 MiB	125 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
6130F	11 July 2017	$ 2,049.00	Xeon Gold	Skylake SP	16	32	16 MiB	22 MiB	135 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
6130T	11 July 2017	$ 1,988.00	Xeon Gold	Skylake SP	16	32	16 MiB	22 MiB	125 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
6132	11 July 2017	$ 2,111.00	Xeon Gold	Skylake SP	14	28	14 MiB	19.25 MiB	140 W	2.6 GHz	3.7 GHz	768 GiB	✔	✔
6134	11 July 2017	$ 2,214.00	Xeon Gold	Skylake SP	8	16	8 MiB	24.75 MiB	130 W	3.2 GHz	3.7 GHz	768 GiB	✔	✔
6134M	11 July 2017	$ 5,217.00	Xeon Gold	Skylake SP	8	16	8 MiB	24.75 MiB	130 W	3.2 GHz	3.7 GHz	1,536 GiB	✔	✔
6136	11 July 2017	$ 2,460.00	Xeon Gold	Skylake SP	12	24	12 MiB	24.75 MiB	150 W	3 GHz	3.7 GHz	768 GiB	✔	✔
6138	11 July 2017	$ 2,612.00	Xeon Gold	Skylake SP	20	40	20 MiB	27.5 MiB	125 W	2 GHz	3.7 GHz	768 GiB	✔	✔
6138F	11 July 2017	$ 2,767.00	Xeon Gold	Skylake SP	20	40	20 MiB	27.5 MiB	135 W	2 GHz	3.7 GHz	768 GiB	✔	✔
6138T	11 July 2017	$ 2,742.00	Xeon Gold	Skylake SP	20	40	20 MiB	27.5 MiB	125 W	2 GHz	3.7 GHz	768 GiB	✔	✔
6140	11 July 2017	$ 2,445.00	Xeon Gold	Skylake SP	18	36	18 MiB	24.75 MiB	140 W	2.3 GHz	3.7 GHz	768 GiB	✔	✔
6140M	11 July 2017	$ 5,448.00	Xeon Gold	Skylake SP	18	36	18 MiB	24.75 MiB	140 W	2.3 GHz	3.7 GHz	1,536 GiB	✔	✔
6142	11 July 2017	$ 2,946.00	Xeon Gold	Skylake SP	16	32	16 MiB	22 MiB	150 W	2.6 GHz	3.7 GHz	768 GiB	✔	✔
6142F	11 July 2017	$ 3,101.00	Xeon Gold	Skylake SP	16	32	16 MiB	22 MiB	160 W	2.6 GHz	3.7 GHz	768 GiB	✔	✔
6142M	11 July 2017	$ 5,949.00	Xeon Gold	Skylake SP	16	32	16 MiB	22 MiB	150 W	2.6 GHz	3.7 GHz	1,536 GiB	✔	✔
6144	11 July 2017	$ 2,925.00	Xeon Gold	Skylake SP	8	16	8 MiB	24.75 MiB	150 W	3.5 GHz	4.2 GHz	768 GiB	✔	✔
6146	11 July 2017	$ 3,286.00	Xeon Gold	Skylake SP	12	24	12 MiB	24.75 MiB	165 W	3.2 GHz	4.2 GHz	768 GiB	✔	✔
6148	11 July 2017	$ 3,072.00	Xeon Gold	Skylake SP	20	40	20 MiB	27.5 MiB	150 W	2.4 GHz	3.7 GHz	768 GiB	✔	✔
6148F	11 July 2017	$ 3,227.00	Xeon Gold	Skylake SP	20	40	20 MiB	27.5 MiB	160 W	2.4 GHz	3.7 GHz	768 GiB	✔	✔
6150	11 July 2017	$ 3,358.00	Xeon Gold	Skylake SP	18	36	18 MiB	24.75 MiB	165 W	2.7 GHz	3.7 GHz	768 GiB	✔	✔
6152	11 July 2017	$ 3,655.00	Xeon Gold	Skylake SP	22	44	22 MiB	30.25 MiB	140 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
6154	11 July 2017	$ 3,543.00	Xeon Gold	Skylake SP	18	36	18 MiB	24.75 MiB	200 W	3 GHz	3.7 GHz	768 GiB	✔	✔
6161			Xeon Gold	Skylake SP	22	44	22 MiB	30.25 MiB	165 W	2.2 GHz	3 GHz		✘	✔
 	Multiprocessors (8-way)
8124	2017		Xeon Platinum	Skylake SP	18	36	18 MiB	24.75 MiB	240 W	3 GHz	3.5 GHz	768 GiB	✔	✔
8124M	2017		Xeon Platinum	Skylake SP	18	36	18 MiB	24.75 MiB	240 W	3 GHz	3.5 GHz	1,536 GiB	✔	✔
8153	11 July 2017	$ 3,115.00	Xeon Platinum	Skylake SP	16	32	16 MiB	22 MiB	125 W	2 GHz	2.8 GHz	768 GiB	✔	✔
8156	11 July 2017	$ 7,007.00	Xeon Platinum	Skylake SP	4	8	4 MiB	16.5 MiB	105 W	3.6 GHz	3.7 GHz	768 GiB	✔	✔
8158	11 July 2017	$ 7,007.00	Xeon Platinum	Skylake SP	12	24	12 MiB	24.75 MiB	150 W	3 GHz	3.7 GHz	768 GiB	✔	✔
8160	11 July 2017	$ 4,702.00	Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB	150 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
8160F	11 July 2017	$ 4,856.00	Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB	160 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
8160M	11 July 2017	$ 7,704.00	Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB	150 W	2.1 GHz	3.7 GHz	1,536 GiB	✔	✔
8160T	11 July 2017	$ 4,936.00	Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB	150 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
8163	2017		Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB		2.5 GHz		768 GiB	✔	✔
8164	11 July 2017	$ 6,114.00	Xeon Platinum	Skylake SP	26	52	26 MiB	35.75 MiB	150 W	2 GHz	3.7 GHz	768 GiB	✔	✔
8168	11 July 2017	$ 5,890.00	Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB	205 W	2.7 GHz	3.7 GHz	768 GiB	✔	✔
8170	11 July 2017	$ 7,405.00	Xeon Platinum	Skylake SP	26	52	26 MiB	35.75 MiB	165 W	2.1 GHz	3.7 GHz	768 GiB	✔	✔
8170F			Xeon Platinum	Skylake SP	26	52	26 MiB	35.75 MiB	165 W	2.1 GHz	2.8 GHz	768 GiB	✔	✔
8170M	11 July 2017	$ 10,409.00	Xeon Platinum	Skylake SP	26	52	26 MiB	35.75 MiB	165 W	2.1 GHz	3.7 GHz	1,536 GiB	✔	✔
8173M			Xeon Platinum	Skylake SP	28	56	28 MiB	38.5 MiB		2.1 GHz	3.8 GHz	1,536 GiB	✔	✔
8175	2017		Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB		2.5 GHz	3,100 GHz	768 GiB	✔	✔
8175M	2017		Xeon Platinum	Skylake SP	24	48	24 MiB	33 MiB		2.5 GHz	3.5 GHz	1,536 GiB	✔	✔
8176	11 July 2017	$ 8,719.00	Xeon Platinum	Skylake SP	28	56	28 MiB	38.5 MiB	165 W	2.1 GHz	3.8 GHz	768 GiB	✔	✔
8176F	11 July 2017		Xeon Platinum	Skylake SP	28	56	28 MiB	38.5 MiB	173 W	2.1 GHz	3.8 GHz	768 GiB	✔	✔
8176M	11 July 2017	$ 11,722.00	Xeon Platinum	Skylake SP	28	56	28 MiB	38.5 MiB	165 W	2.1 GHz	3.8 GHz	1,536 GiB	✔	✔
8180	11 July 2017	$ 10,009.00	Xeon Platinum	Skylake SP	28	56	28 MiB	38.5 MiB	205 W	2.5 GHz	3.8 GHz	768 GiB	✔	✔
8180M	11 July 2017	$ 13,011.00	Xeon Platinum	Skylake SP	28	56	28 MiB	38.5 MiB	205 W	2.5 GHz	3.8 GHz	1,536 GiB	✔	✔
Count: 113
References

    Intel Unveils Powerful Intel Xeon Scalable Processors, Live Event, July 11, 2017
    Intel Xeon Scalable Process Architecture Deep Dive, Akhilesh Kumar & Malay Trivedi, Skylake-SP CPU & Lewisburg PCH Architects, June 12th, 2017.
    IEEE Hot Chips (HC28) 2017.
    IEEE ISSCC 2018

Documents

    New Intel Core X-Series Processor Family
    Intel Xeon (Skylake SP) Processors Product Brief
    Intel Xeon (Skylake SP) Processors Product Overview
    Xeon (Skylake W) Workstations Overview
    Optimal HPC solutions with Intel Scalable Xeons

Categories:

    cpu microarchitectures by intelmicroarchitectures by intelall microarchitectures

Facts about "Skylake (server) - Microarchitectures - Intel"
RDF feed
codename	Skylake (server) +
core count	4 +, 6 +, 8 +, 10 +, 12 +, 14 +, 16 +, 18 +, 20 +, 22 +, 24 +, 26 + and 28 +
designer	Intel +
first launched	May 4, 2017 +
full page name	intel/microarchitectures/skylake (server) +
instance of	microarchitecture +
instruction set architecture	x86-64 +
manufacturer	Intel +
microarchitecture type	CPU +
name	Skylake (server) +
pipeline stages (max)	19 +
pipeline stages (min)	14 +
process	14 nm (0.014 μm, 1.4e-5 mm) +
This page was last modified on 23 March 2020, at 23:05.

    Privacy policyAbout WikiChipDisclaimers 

@burnsville:~/hwsec-course/lab-cacheattacks/Part2-DeadDrop$ lscpu
Architecture:             x86_64
  CPU op-mode(s):         32-bit, 64-bit
  Address sizes:          46 bits physical, 48 bits virtual
  Byte Order:             Little Endian
CPU(s):                   32
  On-line CPU(s) list:    0-31
Vendor ID:                GenuineIntel
  Model name:             Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz
    CPU family:           6
    Model:                85
    Thread(s) per core:   2
    Core(s) per socket:   8
    Socket(s):            2
    Stepping:             4
    CPU max MHz:          3000.0000
    CPU min MHz:          800.0000
    BogoMIPS:             4200.00
    Flags:                fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx f
                          xsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts
                          rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx s
                          mx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadli
                          ne_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb cat_l3 cdp_l3 pti
                           intel_ppin ssbd mba ibrs ibpb stibp tpr_shadow flexpriority ept vpid ept_ad fsgsbase tsc_adju
                          st bmi1 hle avx2 smep bmi2 erms invpcid rtm cqm mpx rdt_a avx512f avx512dq rdseed adx smap clf
                          lushopt clwb intel_pt avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves cqm_llc cqm_oc
                          cup_llc cqm_mbm_total cqm_mbm_local dtherm ida arat pln pts vnmi pku ospke md_clear flush_l1d
                          arch_capabilities ibpb_exit_to_user
Virtualization features:
  Virtualization:         VT-x
Caches (sum of all):
  L1d:                    512 KiB (16 instances)
  L1i:                    512 KiB (16 instances)
  L2:                     16 MiB (16 instances)
  L3:                     22 MiB (2 instances)
NUMA:
  NUMA node(s):           2
  NUMA node0 CPU(s):      0-7,16-23
  NUMA node1 CPU(s):      8-15,24-31
Vulnerabilities:
  Gather data sampling:   Mitigation; Microcode
  Itlb multihit:          KVM: Mitigation: VMX disabled
  L1tf:                   Mitigation; PTE Inversion; VMX conditional cache flushes, SMT vulnerable
  Mds:                    Mitigation; Clear CPU buffers; SMT vulnerable
  Meltdown:               Mitigation; PTI
  Mmio stale data:        Mitigation; Clear CPU buffers; SMT vulnerable
  Reg file data sampling: Not affected
  Retbleed:               Mitigation; IBRS
  Spec rstack overflow:   Not affected
  Spec store bypass:      Mitigation; Speculative Store Bypass disabled via prctl
  Spectre v1:             Mitigation; usercopy/swapgs barriers and __user pointer sanitization
  Spectre v2:             Mitigation; IBRS; IBPB conditional; STIBP conditional; RSB filling; PBRSB-eIBRS Not affected;
                          BHI Not affected
  Srbds:                  Not affected
  Tsx async abort:        Mitigation; Clear CPU buffers; SMT vulnerable
  Vmscape:                Mitigation; IBPB before exit to userspace
benluis@burnsville:~/hwsec-course/lab-cacheattacks/Part2-DeadDrop$