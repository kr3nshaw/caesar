cmake_minimum_required(VERSION 2.8)

project(sf2cute CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VERSION "0.1.0")

include_directories(include)

if(MSVC)
    option(STATIC_CRT "Use static CRT libraries" OFF)

    # Rewrite command line flags to use /MT if necessary
    if(STATIC_CRT)
        foreach(flag_var
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif()
endif()


#============================================================================
# sf2cute
#============================================================================

set(SF2CUTE_PUBLIC_HDRS
    include/sf2cute.hpp
    include/sf2cute/modulator_item.hpp
    include/sf2cute/preset.hpp
    include/sf2cute/preset_zone.hpp
    include/sf2cute/sample.hpp
    include/sf2cute/types.hpp
    include/sf2cute/version.hpp
    include/sf2cute/zone.hpp
    include/sf2cute/file.hpp
    include/sf2cute/generator_item.hpp
    include/sf2cute/instrument.hpp
    include/sf2cute/instrument_zone.hpp
    include/sf2cute/modulator.hpp
    include/sf2cute/modulator_key.hpp
)
set(SF2CUTE_PRIVATE_HDRS
    src/sf2cute/byteio.hpp
    src/sf2cute/file_writer.hpp
    src/sf2cute/riff.hpp
    src/sf2cute/riff_ibag_chunk.hpp
    src/sf2cute/riff_igen_chunk.hpp
    src/sf2cute/riff_imod_chunk.hpp
    src/sf2cute/riff_inst_chunk.hpp
    src/sf2cute/riff_pbag_chunk.hpp
    src/sf2cute/riff_pgen_chunk.hpp
    src/sf2cute/riff_phdr_chunk.hpp
    src/sf2cute/riff_pmod_chunk.hpp
    src/sf2cute/riff_shdr_chunk.hpp
    src/sf2cute/riff_smpl_chunk.hpp
)
set(SF2CUTE_SRCS
    src/sf2cute/file.cpp
    src/sf2cute/file_writer.cpp
    src/sf2cute/generator_item.cpp
    src/sf2cute/instrument.cpp
    src/sf2cute/instrument_zone.cpp
    src/sf2cute/modulator.cpp
    src/sf2cute/modulator_key.cpp
    src/sf2cute/modulator_item.cpp
    src/sf2cute/preset.cpp
    src/sf2cute/preset_zone.cpp
    src/sf2cute/riff.cpp
    src/sf2cute/riff_ibag_chunk.cpp
    src/sf2cute/riff_igen_chunk.cpp
    src/sf2cute/riff_imod_chunk.cpp
    src/sf2cute/riff_inst_chunk.cpp
    src/sf2cute/riff_pbag_chunk.cpp
    src/sf2cute/riff_pgen_chunk.cpp
    src/sf2cute/riff_phdr_chunk.cpp
    src/sf2cute/riff_pmod_chunk.cpp
    src/sf2cute/riff_shdr_chunk.cpp
    src/sf2cute/riff_smpl_chunk.cpp
    src/sf2cute/sample.cpp
    src/sf2cute/zone.cpp
)

add_library(sf2cute STATIC ${SF2CUTE_SRCS} ${SF2CUTE_PUBLIC_HDRS} ${SF2CUTE_PRIVATE_HDRS})

#============================================================================
# Example binaries
#============================================================================

add_executable(write_sf2 examples/write_sf2.cpp)
target_link_libraries(write_sf2 sf2cute)
