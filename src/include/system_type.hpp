#ifndef C6502PP_SYSTEM_TYPE_HPP
#   define C6502PP_SYSTEM_TYPE_HPP
#   include "system.hpp"

namespace C6502PP {

#ifdef STATIC_SYSTEM
using SystemType = CompileTimeSystem<MOS6502, Bus::SimpleMemory>;
#else
using SystemType = RuntimeSystem<MOS6502, Bus::AbstractMemory>;
#endif

}

#endif
