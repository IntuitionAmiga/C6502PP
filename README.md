```
            =========================
              ┏━╸┏━┓┏━╸┏━┓┏━┓┏━┓┏━┓
              ┃  ┣━┓┗━┓┃┃┃┏━┛┣━┛┣━┛
              ┗━╸┗━┛┗━┛┗━┛┗━╸╹  ╹
            =========================
            SixPhphive02 Goes Native!
```
# C6502PP
### The most unnecessary port of the world's least sensible 6502 emulator!

## What
A C++ implementation of [SixPhphive02](https://github.com/0xABADCAFE/sixphphive02)
- Compile Time Abstraction, uses templates and concepts in place of polymorphism and interfaces.
- Supports original switch/case or computed goto jump table.
- Achieves ~147x the performance of the Elephpant powered original on the same hardware.

## Why
Mainly a nerdsnipe, but also an excuse to play with a hypergolic mix of C++20 concepts and low level dirty GCC-isms.

## A Journey

### Origin

It was lockdown and SixPhphive02 was originally written for amusement and as an experiment in how you might go about such a thing in a language like PHP. The end result wasn't too bad and didn't stray too far away from good practise:

- There were interfaces that were incrementally extended and merged, e.g. _IDevice_ that defined reset behaviours, _IByteAccessible_ that defined read/write behaviours, _IBusDevice_ that merged them and a _IProcessor_ interface that extended _IDevice_ adding more specific behaviours for a CPU.
- There were concrete implementations, e.g. a 6502 implementation of _IProcessor_, RAM and ROM implementations of _IBusDevice_ and even a PageMapped device that managed a collection of other _IBusDevice_ instances that lived at different address.
- The CPU class accepted an _IBusDevice_ implementation as a constructor dependency to keep things neatly decoupled.
- The CPU class implemented a basic interpreter that in a loop pulled the next instuction byte from the bus and a _switch/case_ to handle the instruction.

To test the performance, two benchmarks were ran.

- The simplest possible instruction, NOP, was repeatedly executed in blocks of 32768 to get a baseline for the fastest throughtput.
- The KlausD diagnostic ROM (used to validate the emulation was correct) was ran to completion and timed.

Knowing the total instruction counts for each, on a 2018 i7-7500U @ 3.5GHz running PHP 8.1 (at the time) without JIT:

- NOP peaked at 4.31 MIPS
- Klaus D diagnostic achieved 3.07 MIPS

If I am honest, this was actually better than I expected.

### Today

It was Easter and I wanted to mess around with something new but I didn't want the cognitive overhead of thinking what, so I thought I'd port SixPhphive02 to C++. 

### Attempt 1

The first port was a like-for-like reimplementation. I didn't port everything, just the CPU, a basic _AbstractMemory_ with a concrete implementation for a basic array of 65536 bytes. This was given as a dependency on construction, just like the original PHP version. Everything else was basically identical:

- The CPU class ran a loop that fetched the next instruction byte from the _AbstractMemory_ dependency
- A _switch/case_ was used to handle the instruction.

The same basic benchmarks were ran on the same hardware under the same conditions:

- NOP peaked at 433.54 MIPS
- Klaus D diagnostic achieved 185.16 MIPS

This was already a **100x** speedup for the simplest operation and a **60x** speed up more generally.

### Attempt 2

What I wanted to do was to try _compile time_ abstraction. The next iteration changed things:

- The CPU became a template class that depended on another template for the memory.
- The idea was that the required memory access methods should be inlineable and should be optimised away to direct array accesses.
- Notions of how the data should be aligned to the machine cache width were included in the templating.

The same benchmarks were repeated:

- NOP peaked at 523.64 MIPS
- Klaus D diagnostic achieved 294.75 MIPS

This was definitely a result supporting the compile time over runtime abstraction argument.

### Attempt 3

I wanted to validate my assumptions about what was going on in the code and inspected the assembly:

- The original version relied on `call` for the virtual functions and it was clear this was all stripped away for direct array access in the new version.
- However, I did notice that the reference for the Memory instance was being reloaded in every handler.

That surprised me because being such a hot reference you'd expect it to get put into a register. So I thought I might try and _pin_ it. To do that, I created a local reference with the same name as the member and marked it as `__restrict__` which is a promise to the compiler that it won't change over the lifetime of the scope it's defined in. This should make it easy for the compiler to keep it in a register and avoid having to constantly reload it.

The same benchmarks were repeated:

- NOP peaked at 569.16 MIPS
- Klaus D diagnostic achieved 289.46 MIPS

The dip in the diagnostic performance was unexpected but completely repeatable. The assumption was that allocating the reference into a register reduced the number of registers available elsewhere potentially slowingdown some of the other operations.

- Checking the generated assembler showed that the reference was indeed persisted in a register but it was not completely obvious which other instruction handlers had been impacted negatively.

### Attempt 4

Looking at the assembly language reminded me that _switch/case_ constructs are sometimes not as fast as people like to think. Since I was compiling for 64-bit, the compiler was generating a jump table with 32-bit displacements from the program counter. 256 entries, 4 bytes each (1KiB) and all funneling though a central dispatch location. So I decided to change that to use _computed goto_.

- This is a GCC-ism that allows the address of a label to be taken and used as an indirect _goto_ destination.
- The overall size of the executable was already around 32 KiB so this got me thinking that I could construct an array of 16-bit offsets using a bit of label arithmetic and this table would be half the size of the typical switch/case.
- Finally, the computed goto could be added at the end of each instruction handler to automatically determine where to go next, without branching backwards and forwards from the single dispatch location:
    - This approach is commonly known as _threaded dispatch_
    - Note, not _threaded_ as in concurrent, but to run a thread through something.

Doing this without radically having to rewrite everything was solved using a set of regular preprocessor macros that either generate either the regular switch/case logic or the new computed goto.

The same benchmarks were repeated:

- NOP peaked at 761.93 MIPS
- Klaus D achived 452.20 MIPS

That's now **176.8x** faster than the original PHP in the simplest case and **147.3x** faster more generally.


### Tidying Up

Understandably the code was a bit of a mess by now so in order to bring some sanity to all the templating, I decided to try C++20 _concepts_ to bring it together by reintroducing the basic ideas that _interfaces_ were used to solve in the first versions:


```C++

    /**
     * Device
     *
     * Has soft and hard reset behaviours
     */
    template<typename T>
    concept Device = requires(T t) {
        { t.softReset() } noexcept -> std::same_as<T&>; // Fluent
        { t.hardReset() } noexcept -> std::same_as<T&>; // Fluent
    };

    /**
     * ByteAccessible
     *
     * Defines basic 8-bit 16-bit addressable IO
     */
    template<typename T>
    concept ByteAccessible = requires(T t, Address address, Byte value) {
        { t.readByte(address) } noexcept -> std::convertible_to<Byte>;
        { t.writeByte(address, value) } noexcept -> std::convertible_to<void>;
    };

    /**
     * BusDevice
     *
     * Something that is both a Device and ByteAccessible
     */
    template<typename T>
    concept BusDevice = Device<T> && ByteAccessible<T>;

    /**
     * Processor
     *
     * Defines Device with constructor dependency on BusDevice.
     */
    template<typename D, typename B>
    concept Processor = Device<D> && BusDevice<B> && std::constructible_from<D, B&> && requires(D d, Address address) {
        { d.setProgramCounter(address) } noexcept -> std::same_as<D&>; // Fluent
        { d.getProgramCounter() } noexcept -> std::same_as<Address>;
        { d.run() } noexcept -> std::same_as<D&>; // Fluent
    };


    /**
     * CompileTimeSystem<Processor, BusDevice>
     *
     * Wraps an instance of a given Processor concept and BusDevice concept such that code generator is
     * able to inine away the read/write calls to the bus implementation.
     */
    template <
        template <typename> typename CPUType, // This is not the most intuitive syntax, is it?
        BusDevice MemoryBus
    >
    struct CompileTimeSystem {
    
        using CPU = CPUType<MemoryBus>;
    
        CPU oCPU;
        MemoryBus oBus;
    
        CompileTimeSystem() : oCPU(oBus) {
            // Ensure the CPU is constructed with the Bus
        }
    
        CompileTimeSystem& run() {
            oCPU.run();
            return *this;
        }
    
        CompileTimeSystem& runFrom(Address iStart) {
            oCPU.setProgramCounter(iStart).run();
            return *this;
        }
    
        CompileTimeSystem& softReset() noexcept {
            oCPU.softReset();
            oBus.softReset();
            return *this;
        }
    
        CompileTimeSystem& hardReset() noexcept {
            oCPU.hardReset();
            oBus.hardReset();
            return *this;
        }
    };
```

The _CompileTimeSystem_ configuration is sometimes referred the _Motherboard Pattern_ in emulation circles. In our case, it means that when the compiler synthesises the CPU code and it sees lots of calls to oOutside.readByte() and friends, those calls are mapped directly to the SimpleMemory dependency and the compiler can simply optimse them away completely allowing the generated code to direclty access the array.

Contrast this to the standard OOP methodology in which the BusDevice would be some abstract class with concrete realisations. In that model, it is likely that readByte() etc. are virtual functions and as good as impossible to inline within the CPU code. A typical virtual function has a double indirection (virtual table access followed by getting the pointer in the table to the implementation code of the method).

Here we are taking a bet that we don't want a runtime configured System but even if there were several to choose from, we can stamp them out at compile time and simply choose the one to use at runtime, all without ever taking the runtime virtual call hit.

## Tweaks
Since an approximate hundredfold increase in performance going from idiomatic OOP PHP to templated C++ wasn't enough, some additional changes were made to further improve performance:

- The basic infinite loop around a switch/case model can be swapped for a jump table. This uses computed goto, which allows for _threaded dispatch_. This allows each opcode handler to calculate the next one to call and branch directly to it.
- The table is reduced to 16-bit wide entries which dramatically improves L1 cache hit rate.
- Even when templated and inlined, access to the Bus from the CPU code still must go via the reference. By assigning this to a local reference variable and using the `__restrict__` qualifier we give the compiler the information it needs to put that reference into a register for the durarion of the interpeter job. This reduces both the number of native operations 
and the number of memory accesses per emulated instruction.
- Structures are aligned to the target architecture cache line width, or 64 if that value cannot be reliably determined at compile time.
