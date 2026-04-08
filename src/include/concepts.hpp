#ifndef C6502PP_CONCEPTS_HPP
#   define C6502PP_CONCEPTS_HPP
#   include <cstdint>
#   include <cstddef>
#   include <concepts>

namespace C6502PP {

#   ifdef __cpp_lib_hardware_interference_size
    using std::hardware_destructive_interference_size;
    inline constexpr std::size_t NativeCacheLine = hardware_destructive_interference_size;
#   else
    // 64 is the 'Golden Constant' for x86_64 and most ARMv8/v9.
    inline constexpr std::size_t NativeCacheLine = 64;
#   endif

    using Byte    = uint8_t;
    using Word    = uint16_t;
    using Address = uint16_t;

    /**
     * Device
     *
     * Has soft and hard reset behaviours
     */
    template<typename T>
    concept Device = requires(T t) {
        { t.softReset() } noexcept -> std::same_as<T&>;
        { t.hardReset() } noexcept -> std::same_as<T&>;
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
     * Defines Device with constructor dependency on BusDevice
     */
    template<typename D, typename B>
    concept Processor = Device<D> && BusDevice<B> && std::constructible_from<D, B&> && requires(D d, Address address) {
        { d.setProgramCounter(address) } noexcept -> std::same_as<D&>;
        { d.getProgramCounter() } noexcept -> std::same_as<Address>;
    };

}

#endif
