```
 ┏━╸┏━┓┏━╸┏━┓┏━┓┏━┓┏━┓
 ┃  ┣━┓┗━┓┃┃┃┏━┛┣━┛┣━┛
 ┗━╸┗━┛┗━┛┗━┛┗━╸╹  ╹
=======================
SixPhpive02 Goes Native
```
# C6502PP
### The most unnecessary port of the world's least sensible 6502 emulator!

## What
A C++ implementation of [SixPhpive02](https://github.com/0xABADCAFE/sixphphive02)
- Compile Time Abstraction, uses templates and concepts in place of polymorphism and interfaces.
- Supports original switch/case or computed goto jump table.
- Achieves ~100x the performance of the Elephpant powered original on the same hardware.

## Why
Mainly a nerdsnipe, but also an excuse to play with a hypergolic mix of C++20 concepts and low level dirty GCC-isms.

## Things
The key concept of separating the CPU from the outside world that it talks to was kept. Whereas the PHP version used interfaces _IDevice_ _IByteAccessible_ and corresponding extensions and implementations, C6502PP uses _concepts_.

```C++
    // A Device must provide fluent softRest() and hardReset() pair
    template<typename T>
    concept Device = requires(T t) {
        { t.softReset() } noexcept -> std::same_as<T&>;
        { t.hardReset() } noexcept -> std::same_as<T&>;
    };

    // A ByteAccessible must provide readByte() and writeByte() operations
    template<typename T>
    concept ByteAccessible = requires(T t, Address address, Byte value) {
        { t.readByte(address) } noexcept -> std::convertible_to<Byte>;
        { t.writeByte(address, value) } noexcept -> std::convertible_to<void>;
    };

    // A BusDevice is a union of Device and ByteAccessible
    template<typename T>
    concept BusDevice = Device<T> && ByteAccessible<T>;

    // A Processor is a Device that has a dependency on a BusDevice.
    // This is how we compile-time model the original in which a Processor is given an outside world dependency.
    template<typename D, typename B>
    concept Processor = Device<D> && BusDevice<B> && std::constructible_from<D, B&>;
```
Connecting up the CPU (Processor Concept) and a SimpleMemory (just a 64K byte array) is handled by putting both of these into a container template that joins them. For example:

```C++

// Structure implements everything stipulated by the BusDevice concept.
struct SimpleMemory {
    Byte bytes[MEM_SIZE] = { 0 };

    // Expeted methods of BusDevice, readByte(), etc.
};

// Template implements everything stipulated by the Processor concept.
template <BusDevice Bus>
struct CPU {
    Bus &oOutside;

    // Expected constructor behaviour.
    CPU(Bus& oBus): oOutside(oBus) { reset(); }

    // Expected methods Processor, softReset(), etc.
};

template <BusDevice MemoryBus>
struct System {
    CPU<MemoryBus> oCPU; // Satisfies Processor concept
    MemoryBus oBus;      // Satisfies BusDevice concept

    System() : oCPU(oBus) {
         // Injection dependency handled on system construction
    }

    // Methods for the system as a whole...
```

This configuration is sometimes called the _Motherboard Pattern_ in emulation circles. In our case, it means that when the compiler synthesises the CPU code and it sees lots of calls to oOutside.readByte() and friends, those calls are mapped directly to the SimpleMemory dependency and the compiler can simply optimse them away completely allowing the generated code to direclty access the array.

Contrast this to the standard OOP methodology in which the BusDevice would be some abstract class with concrete realisations. In that model, it is likely that readByte() etc. are virtual functions and as good as impossible to inline within the CPU code. A typical virtual function has a double indirection (virtual table access followed by getting the pointer in the table to the implementation code of the method).

Here we are taking a bet that we don't want a runtime configured System but even if there were several to choose from, we can stamp them out at compile time and simply choose the one to use at runtime, all without ever taking the runtime virtual call hit.
