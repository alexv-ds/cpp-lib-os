// -*- C++ -*-
//
// Copyright 2024 Dmitry Igrishin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DMITIGR_OS_SMBIOS_HPP
#define DMITIGR_OS_SMBIOS_HPP

#include "../base/assert.hpp"
#include "../base/stream.hpp"
#include "../base/traits.hpp"
#include "../rnd/uuid.hpp"
#include "error.hpp"
#ifdef _WIN32
#include "../winbase/exceptions.hpp"
#endif

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <optional>
#ifdef __linux__
#include <fstream>
#endif

namespace dmitigr::os::firmware {

class Smbios_table final {
public:
  using Byte = std::uint8_t;
  using Word = std::uint16_t;
  using Dword = std::uint32_t;
  using Qword = std::uint64_t;

  struct Version final {
#ifdef _WIN32
    Byte used_20_calling_method{};
#endif
    Byte major_version{};
    Byte minor_version{};
    Byte dmi_revision{};

#ifdef _WIN32
    Dword length{};
#endif

    [[nodiscard]] bool is_version_ge(const Byte major, const Byte minor) const noexcept
    {
      return major_version >= major && minor_version >= minor;
    }
  };
  static_assert(std::is_standard_layout_v<Version>);

  struct Structure {
    Byte type{};
    Byte length{};
    Word handle{};
  };
  static_assert(std::is_standard_layout_v<Structure>);

  struct Bios_info final : Structure {
    std::optional<std::string> vendor;
    std::optional<std::string> version;
    std::optional<std::string> release_date;
    Byte rom_size{};
  };

  struct Sys_info final : Structure {
    std::optional<std::string> manufacturer;
    std::optional<std::string> product;
    std::optional<std::string> version;
    std::optional<std::string> serial_number;
    rnd::Uuid uuid{};
  };

  struct Baseboard_info final : Structure {
    std::optional<std::string> manufacturer;
    std::optional<std::string> product;
    std::optional<std::string> version;
    std::optional<std::string> serial_number;
  };

  struct Processor_info final : Structure {
    enum class Type : Byte {
      Unspecified = 0x00,
      Other [[maybe_unused]] = 0x01,
      Unknown [[maybe_unused]] = 0x02,
      CentralProcessor [[maybe_unused]] = 0x03,
      MathProcessor [[maybe_unused]] = 0x04,
      DspProcessor [[maybe_unused]] = 0x05,
      VideoProcessor [[maybe_unused]] = 0x06
    };

    enum class Upgrade : Byte {
      Unspecified = 0x00,
      Other [[maybe_unused]] = 0x01,
      Unknown [[maybe_unused]] = 0x02,
      DaughterBoard [[maybe_unused]] = 0x03,
      ZIFSocket [[maybe_unused]] = 0x04,
      ReplaceablePiggyBack [[maybe_unused]] = 0x05,
      None [[maybe_unused]] = 0x06,
      LIFSocket [[maybe_unused]] = 0x07,
      Slot1 [[maybe_unused]] = 0x08,
      Slot2 [[maybe_unused]] = 0x09,
      Pin370Socket [[maybe_unused]] = 0x0A,
      SlotA [[maybe_unused]] = 0x0B,
      SlotM [[maybe_unused]] = 0x0C,
      Socket423 [[maybe_unused]] = 0x0D,
      SocketA [[maybe_unused]] = 0x0E,
      Socket478 [[maybe_unused]] = 0x0F,
      Socket754 [[maybe_unused]] = 0x10,
      Socket940 [[maybe_unused]] = 0x11,
      Socket939 [[maybe_unused]] = 0x12,
      SocketmPGA604 [[maybe_unused]] = 0x13,
      SocketLGA771 [[maybe_unused]] = 0x14,
      SocketLGA775 [[maybe_unused]] = 0x15,
      SocketS1 [[maybe_unused]] = 0x16,
      SocketAM2 [[maybe_unused]] = 0x17,
      SocketF [[maybe_unused]] = 0x18,
      SocketLGA1366 [[maybe_unused]] = 0x19,
      SocketG34 [[maybe_unused]] = 0x1A,
      SocketAM3 [[maybe_unused]] = 0x1B,
      SocketC32 [[maybe_unused]] = 0x1C,
      SocketLGA1156 [[maybe_unused]] = 0x1D,
      SocketLGA1567 [[maybe_unused]] = 0x1E,
      SocketPGA988A [[maybe_unused]] = 0x1F,
      SocketBGA1288 [[maybe_unused]] = 0x20,
      SocketrPGA988B [[maybe_unused]] = 0x21,
      SocketBGA1023 [[maybe_unused]] = 0x22,
      SocketBGA1224 [[maybe_unused]] = 0x23,
      SocketLGA1155 [[maybe_unused]] = 0x24,
      SocketLGA1356 [[maybe_unused]] = 0x25,
      SocketLGA2011 [[maybe_unused]] = 0x26,
      SocketFS1 [[maybe_unused]] = 0x27,
      SocketFS2 [[maybe_unused]] = 0x28,
      SocketFM1 [[maybe_unused]] = 0x29,
      SocketFM2 [[maybe_unused]] = 0x2A,
      SocketLGA2011_3 [[maybe_unused]] = 0x2B,
      SocketLGA1356_3 [[maybe_unused]] = 0x2C,
      SocketLGA1150 [[maybe_unused]] = 0x2D,
      SocketBGA1168 [[maybe_unused]] = 0x2E,
      SocketBGA1234 [[maybe_unused]] = 0x2F,
      SocketBGA1364 [[maybe_unused]] = 0x30,
      SocketAM4 [[maybe_unused]] = 0x31,
      SocketLGA1151 [[maybe_unused]] = 0x32,
      SocketBGA1356 [[maybe_unused]] = 0x33,
      SocketBGA1440 [[maybe_unused]] = 0x34,
      SocketBGA1515 [[maybe_unused]] = 0x35,
      SocketLGA3647_1 [[maybe_unused]] = 0x36,
      SocketSP3 [[maybe_unused]] = 0x37,
      SocketSP3r2 [[maybe_unused]] = 0x38,
      SocketLGA2066 [[maybe_unused]] = 0x39,
      SocketBGA1392 [[maybe_unused]] = 0x3A,
      SocketBGA1510 [[maybe_unused]] = 0x3B,
      SocketBGA1528 [[maybe_unused]] = 0x3C,
      SocketLGA4189 [[maybe_unused]] = 0x3D,
      SocketLGA1200 [[maybe_unused]] = 0x3E,
      SocketLGA4677 [[maybe_unused]] = 0x3F,
      SocketLGA1700 [[maybe_unused]] = 0x40,
      SocketBGA1744 [[maybe_unused]] = 0x41,
      SocketBGA1781 [[maybe_unused]] = 0x42,
      SocketBGA1211 [[maybe_unused]] = 0x43,
      SocketBGA2422 [[maybe_unused]] = 0x44,
      SocketLGA1211 [[maybe_unused]] = 0x45,
      SocketLGA2422 [[maybe_unused]] = 0x46,
      SocketLGA5773 [[maybe_unused]] = 0x47,
      SocketBGA5773 [[maybe_unused]] = 0x48
    };

    enum class Family : Word {
      Unspecified = 0x00,
      // indicator for Processor_info::processor_family
      // just use Processor_info::processor_family_2 field
      // for SMBIOS 2.6+
      UseNewFiled [[maybe_unused]] = 0xFE,

      Other [[maybe_unused]] = 0x01,
      Unknown [[maybe_unused]] = 0x02,
      _8086 [[maybe_unused]] = 0x03,
      _80286 [[maybe_unused]] = 0x04,
      Intel386processor [[maybe_unused]] = 0x05,
      Intel486processor [[maybe_unused]] = 0x06,
      _8087 [[maybe_unused]] = 0x07,
      _80287 [[maybe_unused]] = 0x08,
      _80387 [[maybe_unused]] = 0x09,
      _80487 [[maybe_unused]] = 0x0A,
      IntelPentiumprocessor [[maybe_unused]] = 0x0B,
      PentiumProprocessor [[maybe_unused]] = 0x0C,
      PentiumIIprocessor [[maybe_unused]] = 0x0D,
      PentiumprocessorwithMMXtechnology [[maybe_unused]] = 0x0E,
      IntelCeleronprocessor [[maybe_unused]] = 0x0F,
      PentiumIIXeonprocessor [[maybe_unused]] = 0x10,
      PentiumIIIprocessor [[maybe_unused]] = 0x11,
      M1Family [[maybe_unused]] = 0x12,
      M2Family [[maybe_unused]] = 0x13,
      IntelCeleronMprocessor [[maybe_unused]] = 0x14,
      IntelPentium4HTprocessor [[maybe_unused]] = 0x15,
      AMDDuronProcessorFamily [[maybe_unused]] = 0x18,
      K5Family [[maybe_unused]] = 0x19,
      K6Family [[maybe_unused]] = 0x1A,
      K6_2 [[maybe_unused]] = 0x1B,
      K6_3 [[maybe_unused]] = 0x1C,
      AMDAthlonProcessorFamily [[maybe_unused]] = 0x1D,
      AMD29000Family [[maybe_unused]] = 0x1E,
      K6_2plus [[maybe_unused]] = 0x1F,
      PowerPCFamily [[maybe_unused]] = 0x20,
      PowerPC601 [[maybe_unused]] = 0x21,
      PowerPC603 [[maybe_unused]] = 0x22,
      PowerPC603plus [[maybe_unused]] = 0x23,
      PowerPC604 [[maybe_unused]] = 0x24,
      PowerPC620 [[maybe_unused]] = 0x25,
      PowerPCx704 [[maybe_unused]] = 0x26,
      PowerPC750 [[maybe_unused]] = 0x27,
      IntelCoreDuoprocessor [[maybe_unused]] = 0x28,
      IntelCoreDuomobileprocessor [[maybe_unused]] = 0x29,
      IntelCoreSolomobileprocessor [[maybe_unused]] = 0x2A,
      IntelAtomprocessor [[maybe_unused]] = 0x2B,
      IntelCoreMprocessor [[maybe_unused]] = 0x2C,
      Intel_Corem3processor [[maybe_unused]] = 0x2D,
      Intel_Corem5processor [[maybe_unused]] = 0x2E,
      Intel_Corem7processor [[maybe_unused]] = 0x2F,
      AlphaFamily [[maybe_unused]] = 0x30,
      Alpha21064 [[maybe_unused]] = 0x31,
      Alpha21066 [[maybe_unused]] = 0x32,
      Alpha21164 [[maybe_unused]] = 0x33,
      Alpha21164PC [[maybe_unused]] = 0x34,
      Alpha21164a [[maybe_unused]] = 0x35,
      Alpha21264 [[maybe_unused]] = 0x36,
      Alpha21364 [[maybe_unused]] = 0x37,
      AMDTurionIIUltraDual_CoreMobileMProcessorFamily [[maybe_unused]] = 0x38,
      AMDTurionIIDual_CoreMobileMProcessorFamily [[maybe_unused]] = 0x39,
      AMDAthlonIIDual_CoreMProcessorFamily [[maybe_unused]] = 0x3A,
      AMDOpteron6100SeriesProcessor [[maybe_unused]] = 0x3B,
      AMDOpteron4100SeriesProcessor [[maybe_unused]] = 0x3C,
      AMDOpteron6200SeriesProcessor [[maybe_unused]] = 0x3D,
      AMDOpteron4200SeriesProcessor [[maybe_unused]] = 0x3E,
      AMDFXSeriesProcessor [[maybe_unused]] = 0x3F,
      MIPSFamily [[maybe_unused]] = 0x40,
      MIPSR4000 [[maybe_unused]] = 0x41,
      MIPSR4200 [[maybe_unused]] = 0x42,
      MIPSR4400 [[maybe_unused]] = 0x43,
      MIPSR4600 [[maybe_unused]] = 0x44,
      MIPSR10000 [[maybe_unused]] = 0x45,
      AMDC_SeriesProcessor [[maybe_unused]] = 0x46,
      AMDE_SeriesProcessor [[maybe_unused]] = 0x47,
      AMDA_SeriesProcessor [[maybe_unused]] = 0x48,
      AMDG_SeriesProcessor [[maybe_unused]] = 0x49,
      AMDZ_SeriesProcessor [[maybe_unused]] = 0x4A,
      AMDR_SeriesProcessor [[maybe_unused]] = 0x4B,
      AMDOpteron4300SeriesProcessor [[maybe_unused]] = 0x4C,
      AMDOpteron6300SeriesProcessor [[maybe_unused]] = 0x4D,
      AMDOpteron3300SeriesProcessor [[maybe_unused]] = 0x4E,
      AMDFireProSeriesProcessor [[maybe_unused]] = 0x4F,
      SPARCFamily [[maybe_unused]] = 0x50,
      SuperSPARC [[maybe_unused]] = 0x51,
      microSPARCII [[maybe_unused]] = 0x52,
      microSPARCIIep [[maybe_unused]] = 0x53,
      UltraSPARC [[maybe_unused]] = 0x54,
      UltraSPARCII [[maybe_unused]] = 0x55,
      UltraSPARCIii [[maybe_unused]] = 0x56,
      UltraSPARCIII [[maybe_unused]] = 0x57,
      UltraSPARCIIIi [[maybe_unused]] = 0x58,
      _68040Family [[maybe_unused]] = 0x60,
      _68xxx [[maybe_unused]] = 0x61,
      _68000 [[maybe_unused]] = 0x62,
      _68010 [[maybe_unused]] = 0x63,
      _68020 [[maybe_unused]] = 0x64,
      _68030 [[maybe_unused]] = 0x65,
      AMDAthlonX4Quad_CoreProcessorFamily [[maybe_unused]] = 0x66,
      AMDOpteronX1000SeriesProcessor [[maybe_unused]] = 0x67,
      AMDOpteronX2000SeriesAPU [[maybe_unused]] = 0x68,
      AMDOpteronA_SeriesProcessor [[maybe_unused]] = 0x69,
      AMDOpteronX3000SeriesAPU [[maybe_unused]] = 0x6A,
      AMDZenProcessorFamily [[maybe_unused]] = 0x6B,
      HobbitFamily [[maybe_unused]] = 0x70,
      CrusoeTM5000Family [[maybe_unused]] = 0x78,
      CrusoeTM3000Family [[maybe_unused]] = 0x79,
      EfficeonTM8000Family [[maybe_unused]] = 0x7A,
      Weitek [[maybe_unused]] = 0x80,
      Itaniumprocessor [[maybe_unused]] = 0x82,
      AMDAthlon64ProcessorFamily [[maybe_unused]] = 0x83,
      AMDOpteronProcessorFamily [[maybe_unused]] = 0x84,
      AMDSempronProcessorFamily [[maybe_unused]] = 0x85,
      AMDTurion64MobileTechnology [[maybe_unused]] = 0x86,
      Dual_CoreAMDOpteronProcessorFamily [[maybe_unused]] = 0x87,
      AMDAthlon64X2Dual_CoreProcessorFamily [[maybe_unused]] = 0x88,
      AMDTurion64X2MobileTechnology [[maybe_unused]] = 0x89,
      Quad_CoreAMDOpteronProcessorFamily [[maybe_unused]] = 0x8A,
      Third_GenerationAMDOpteronProcessorFamily [[maybe_unused]] = 0x8B,
      AMDPhenomFXQuad_CoreProcessorFamily [[maybe_unused]] = 0x8C,
      AMDPhenomX4Quad_CoreProcessorFamily [[maybe_unused]] = 0x8D,
      AMDPhenomX2Dual_CoreProcessorFamily [[maybe_unused]] = 0x8E,
      AMDAthlonX2Dual_CoreProcessorFamily [[maybe_unused]] = 0x8F,
      PA_RISCFamily [[maybe_unused]] = 0x90,
      PA_RISC8500 [[maybe_unused]] = 0x91,
      PA_RISC8000 [[maybe_unused]] = 0x92,
      PA_RISC7300LC [[maybe_unused]] = 0x93,
      PA_RISC7200 [[maybe_unused]] = 0x94,
      PA_RISC7100LC [[maybe_unused]] = 0x95,
      PA_RISC7100 [[maybe_unused]] = 0x96,
      V30Family [[maybe_unused]] = 0xA0,
      Quad_CoreIntelXeonprocessor3200Series [[maybe_unused]] = 0xA1,
      Dual_CoreIntelXeonprocessor3000Series [[maybe_unused]] = 0xA2,
      Quad_CoreIntelXeonprocessor5300Series [[maybe_unused]] = 0xA3,
      Dual_CoreIntelXeonprocessor5100Series [[maybe_unused]] = 0xA4,
      Dual_CoreIntelXeonprocessor5000Series [[maybe_unused]] = 0xA5,
      Dual_CoreIntelXeonprocessorLV [[maybe_unused]] = 0xA6,
      Dual_CoreIntelXeonprocessorULV [[maybe_unused]] = 0xA7,
      Dual_CoreIntelXeonprocessor7100Series [[maybe_unused]] = 0xA8,
      Quad_CoreIntelXeonprocessor5400Series [[maybe_unused]] = 0xA9,
      Quad_CoreIntelXeonprocessor [[maybe_unused]] = 0xAA,
      Dual_CoreIntelXeonprocessor5200Series [[maybe_unused]] = 0xAB,
      Dual_CoreIntelXeonprocessor7200Series [[maybe_unused]] = 0xAC,
      Quad_CoreIntelXeonprocessor7300Series [[maybe_unused]] = 0xAD,
      Quad_CoreIntelXeonprocessor7400Series [[maybe_unused]] = 0xAE,
      Multi_CoreIntelXeonprocessor7400Series [[maybe_unused]] = 0xAF,
      PentiumIIIXeonprocessor [[maybe_unused]] = 0xB0,
      PentiumIIIProcessorwithIntelSpeedStepTechnology [[maybe_unused]] = 0xB1,
      Pentium4Processor [[maybe_unused]] = 0xB2,
      IntelXeonprocessor [[maybe_unused]] = 0xB3,
      AS400Family [[maybe_unused]] = 0xB4,
      IntelXeonprocessorMP [[maybe_unused]] = 0xB5,
      AMDAthlonXPProcessorFamily [[maybe_unused]] = 0xB6,
      AMDAthlonMPProcessorFamily [[maybe_unused]] = 0xB7,
      IntelItanium2processor [[maybe_unused]] = 0xB8,
      IntelPentiumMprocessor [[maybe_unused]] = 0xB9,
      IntelCeleronDprocessor [[maybe_unused]] = 0xBA,
      IntelPentiumDprocessor [[maybe_unused]] = 0xBB,
      IntelPentiumProcessorExtremeEdition [[maybe_unused]] = 0xBC,
      IntelCoreSoloProcessor [[maybe_unused]] = 0xBD,
      Reserved [[maybe_unused]] = 0xBE,
      IntelCore2DuoProcessor [[maybe_unused]] = 0xBF,
      IntelCore2Soloprocessor [[maybe_unused]] = 0xC0,
      IntelCore2Extremeprocessor [[maybe_unused]] = 0xC1,
      IntelCore2Quadprocessor [[maybe_unused]] = 0xC2,
      IntelCore2Extrememobileprocessor [[maybe_unused]] = 0xC3,
      IntelCore2Duomobileprocessor [[maybe_unused]] = 0xC4,
      IntelCore2Solomobileprocessor [[maybe_unused]] = 0xC5,
      IntelCorei7processor [[maybe_unused]] = 0xC6,
      Dual_CoreIntelCeleronprocessor [[maybe_unused]] = 0xC7,
      IBM390Family [[maybe_unused]] = 0xC8,
      G4 [[maybe_unused]] = 0xC9,
      G5 [[maybe_unused]] = 0xCA,
      ESA_390G6 [[maybe_unused]] = 0xCB,
      z_Architecturebase [[maybe_unused]] = 0xCC,
      IntelCorei5processor [[maybe_unused]] = 0xCD,
      IntelCorei3processor [[maybe_unused]] = 0xCE,
      IntelCorei9processor [[maybe_unused]] = 0xCF,
      VIAC7_MProcessorFamily [[maybe_unused]] = 0xD2,
      VIAC7_DProcessorFamily [[maybe_unused]] = 0xD3,
      VIAC7ProcessorFamily [[maybe_unused]] = 0xD4,
      VIAEdenProcessorFamily [[maybe_unused]] = 0xD5,
      Multi_CoreIntelXeonprocessor [[maybe_unused]] = 0xD6,
      Dual_CoreIntelXeonprocessor3xxxSeries [[maybe_unused]] = 0xD7,
      Quad_CoreIntelXeonprocessor3xxxSeries [[maybe_unused]] = 0xD8,
      VIANanoProcessorFamily [[maybe_unused]] = 0xD9,
      Dual_CoreIntelXeonprocessor5xxxSeries [[maybe_unused]] = 0xDA,
      Quad_CoreIntelXeonprocessor5xxxSeries [[maybe_unused]] = 0xDB,
      Dual_CoreIntelXeonprocessor7xxxSeries [[maybe_unused]] = 0xDD,
      Quad_CoreIntelXeonprocessor7xxxSeries [[maybe_unused]] = 0xDE,
      Multi_CoreIntelXeonprocessor7xxxSeries [[maybe_unused]] = 0xDF,
      Multi_CoreIntelXeonprocessor3400Series [[maybe_unused]] = 0xE0,
      AMDOpteron3000SeriesProcessor [[maybe_unused]] = 0xE4,
      AMDSempronIIProcessor [[maybe_unused]] = 0xE5,
      EmbeddedAMDOpteronQuad_CoreProcessorFamily [[maybe_unused]] = 0xE6,
      AMDPhenomTriple_CoreProcessorFamily [[maybe_unused]] = 0xE7,
      AMDTurionUltraDual_CoreMobileProcessorFamily [[maybe_unused]] = 0xE8,
      AMDTurionDual_CoreMobileProcessorFamily [[maybe_unused]] = 0xE9,
      AMDAthlonDual_CoreProcessorFamily [[maybe_unused]] = 0xEA,
      AMDSempronSIProcessorFamily [[maybe_unused]] = 0xEB,
      AMDPhenomIIProcessorFamily [[maybe_unused]] = 0xEC,
      AMDAthlonIIProcessorFamily [[maybe_unused]] = 0xED,
      Six_CoreAMDOpteronProcessorFamily [[maybe_unused]] = 0xEE,
      AMDSempronMProcessorFamily [[maybe_unused]] = 0xEF,
      i860 [[maybe_unused]] = 0xFA,
      i960 [[maybe_unused]] = 0xFB,
      ARMv7 [[maybe_unused]] = 0x100,
      ARMv8 [[maybe_unused]] = 0x101,
      ARMv9 [[maybe_unused]] = 0x102,
      ReservedforfutureusebyARM [[maybe_unused]] = 0x103,
      SH_3 [[maybe_unused]] = 0x104,
      SH_4 [[maybe_unused]] = 0x105,
      ARM [[maybe_unused]] = 0x118,
      StrongARM [[maybe_unused]] = 0x119,
      _6x86 [[maybe_unused]] = 0x12C,
      MediaGX [[maybe_unused]] = 0x12D,
      MII [[maybe_unused]] = 0x12E,
      WinChip [[maybe_unused]] = 0x140,
      DSP [[maybe_unused]] = 0x15E,
      VideoProcessor [[maybe_unused]] = 0x1F4,
      RISC_VRV32 [[maybe_unused]] = 0x200,
      RISC_VRV64 [[maybe_unused]] = 0x201,
      RISC_VRV128 [[maybe_unused]] = 0x202,
      LoongArch [[maybe_unused]] = 0x258,
      Loongson1ProcessorFamily [[maybe_unused]] = 0x259,
      Loongson2ProcessorFamily [[maybe_unused]] = 0x25A,
      Loongson3ProcessorFamily [[maybe_unused]] = 0x25B,
      Loongson2KProcessorFamily [[maybe_unused]] = 0x25C,
      Loongson3AProcessorFamily [[maybe_unused]] = 0x25D,
      Loongson3BProcessorFamily [[maybe_unused]] = 0x25E,
      Loongson3CProcessorFamily [[maybe_unused]] = 0x25F,
      Loongson3DProcessorFamily [[maybe_unused]] = 0x260,
      Loongson3EProcessorFamily [[maybe_unused]] = 0x261,
      Dual_CoreLoongson2KProcessor2xxxSeries [[maybe_unused]] = 0x262,
      Quad_CoreLoongson3AProcessor5xxxSeries [[maybe_unused]] = 0x26C,
      Multi_CoreLoongson3AProcessor5xxxSeries [[maybe_unused]] = 0x26D,
      Quad_CoreLoongson3BProcessor5xxxSeries [[maybe_unused]] = 0x26E,
      Multi_CoreLoongson3BProcessor5xxxSeries [[maybe_unused]] = 0x26F,
      Multi_CoreLoongson3CProcessor5xxxSeries [[maybe_unused]] = 0x270,
      Multi_CoreLoongson3DProcessor5xxxSeries [[maybe_unused]] = 0x271,
    };

    // 2.0+
    std::optional<std::string> socket;
    Type type{Type::Unspecified};
    Family family{Family::Unspecified};
    std::optional<std::string> manufacturer;
    Qword id{};
    std::optional<std::string> version;
    Byte voltage{};
    Word external_clock{};
    Word max_speed{};
    Word current_speed{};
    Byte status{};
    Upgrade upgrade{Upgrade::Unspecified};

    // 2.1+
    Word l1_cache_handle{};
    Word l2_cache_handle{};
    Word l3_cache_handle{};

    // 2.3+
    std::optional<std::string> serial_number;
    std::optional<std::string> asset_tag;
    std::optional<std::string> part_number;

    // 2.5+
    Byte core_count{};
    Byte core_enabled{};
    Byte thread_count{};
    Word characteristics{};

    // 2.6+
    Family family_2{Family::Unspecified};

    // 3.0+
    Word core_count_2{};
    Word core_enabled_2{};
    Word thread_count_2{};

    // 3.6+
    Word thread_enabled{};
  };

  static Smbios_table from_system()
  {
    Smbios_table result;
    result.data_ = load_raw_data();
    return result;
  }

  [[nodiscard]] const std::vector<Byte>& raw() const noexcept
  {
    return data_;
  }

  [[nodiscard]] Bios_info bios_info() const
  {
    const auto* const s = structure(0);
    auto result = make_structure<Bios_info>(*s);
    result.vendor = field<std::optional<std::string>>(s, 0x4);
    result.version = field<std::optional<std::string>>(s, 0x5);
    result.release_date = field<std::optional<std::string>>(s, 0x8);
    result.rom_size = field<Byte>(s, 0x9);
    return result;
  }

  [[nodiscard]] Sys_info sys_info() const
  {
    const auto* const s = structure(1);
    auto result = make_structure<Sys_info>(*s);
    result.manufacturer = field<std::optional<std::string>>(s, 0x4);
    result.product = field<std::optional<std::string>>(s, 0x5);
    result.version = field<std::optional<std::string>>(s, 0x6);
    result.serial_number = field<std::optional<std::string>>(s, 0x7);
    result.uuid = field<std::array<Byte, 16>>(s, 0x8);
    return result;
  }

  [[nodiscard]] std::optional<Baseboard_info> baseboard_info() const
  {
    const auto* const s = structure(2, true);
    if (!s)
      return std::nullopt;
    auto result = make_structure<Baseboard_info>(*s);
    result.manufacturer = field<std::optional<std::string>>(s, 0x4);
    result.product = field<std::optional<std::string>>(s, 0x5);
    result.version = field<std::optional<std::string>>(s, 0x6);
    result.serial_number = field<std::optional<std::string>>(s, 0x7);
    return result;
  }

  [[nodiscard]] std::vector<Processor_info> processors_info() const
  {
    const auto version = Smbios_table::version();

    std::vector<Processor_info> result;
    for (auto* s = first_structure(); s; s = next_structure(s)) {
      if (s->type != 0x04)
        continue;

      result.emplace_back(make_structure<Processor_info>(*s));
      auto& info = result.back();

      if (version.is_version_ge(2,0)) {
        info.socket = std::move(field<decltype(info.socket)>(s, 0x04));
        info.type = static_cast<decltype(info.type)>(
          field<std::underlying_type_t<decltype(info.type)>>(s, 0x05)
        );
        info.family = static_cast<decltype(info.family)>(field<Byte>(s, 0x06));
        info.manufacturer = std::move(field<decltype(info.manufacturer)>(s, 0x07));
        info.id = field<decltype(info.id)>(s, 0x08);
        info.version = std::move(field<decltype(info.version)>(s, 0x10));
        info.voltage = field<decltype(info.voltage)>(s, 0x11);
        info.external_clock = field<decltype(info.external_clock)>(s, 0x12);
        info.max_speed = field<decltype(info.max_speed)>(s, 0x14);
        info.current_speed = field<decltype(info.current_speed)>(s, 0x16);
        info.status = field<decltype(info.status)>(s, 0x18);
        info.upgrade = static_cast<decltype(info.upgrade)>(
          field<std::underlying_type_t<decltype(info.upgrade)>>(s, 0x19)
        );
      }

      if (version.is_version_ge(2,1)) {
        info.l1_cache_handle = field<decltype(info.l1_cache_handle)>(s, 0x1A);
        info.l2_cache_handle = field<decltype(info.l2_cache_handle)>(s, 0x1C);
        info.l3_cache_handle = field<decltype(info.l3_cache_handle)>(s, 0x1E);
      }

      if (version.is_version_ge(2,3)) {
        info.serial_number = std::move(field<decltype(info.serial_number)>(s, 0x20));
        info.asset_tag = std::move(field<decltype(info.asset_tag)>(s, 0x21));
        info.part_number = std::move(field<decltype(info.part_number)>(s, 0x22));
      }

      if (version.is_version_ge(2,5)) {
        info.core_count = field<decltype(info.core_count)>(s, 0x23);
        info.core_enabled = field<decltype(info.core_enabled)>(s, 0x24);
        info.thread_count = field<decltype(info.thread_count)>(s, 0x25);
        info.characteristics = field<decltype(info.characteristics)>(s, 0x26);
      }

      if (version.is_version_ge(2,6)) {
        info.family_2 = static_cast<decltype(info.family_2)>(
          field<std::underlying_type_t<decltype(info.family_2)>>(s, 0x28)
        );
      }

      if (version.is_version_ge(3,0)) {
        info.core_count_2 = field<decltype(info.core_count_2)>(s, 0x2A);
        info.core_enabled_2 = field<decltype(info.core_enabled_2)>(s, 0x2C);
        info.thread_count_2 = field<decltype(info.thread_count_2)>(s, 0x2E);
      }

      if (version.is_version_ge(3,6)) {
        info.thread_enabled = field<decltype(info.thread_enabled)>(s, 0x30);
      }
    }
    return result;
  }

private:
  std::vector<Byte> data_;

  Smbios_table() = default;

  template<class S>
  static S make_structure(const Structure& s)
  {
    static_assert(std::is_base_of_v<Structure, S>);
    S result;
    result.Structure::type = s.type;
    result.length = s.length;
    result.handle = s.handle;
    return result;
  }

  [[nodiscard]] const Structure* structure(const Byte type,
    const bool no_throw_if_not_found = false) const
  {
    for (auto* s = first_structure(); s; s = next_structure(s)) {
      if (s->type == type)
        return s;
    }
    if (no_throw_if_not_found)
      return nullptr;
    else
      throw std::runtime_error{"no BIOS information structure of type "
        +std::to_string(type)+" found in SMBIOS"};
  }

  static const char* unformed_section(const Structure* const s) noexcept
  {
    DMITIGR_ASSERT(s);
    return reinterpret_cast<const char*>(s) + s->length;
  }

  [[nodiscard]] const Structure* first_structure() const noexcept
  {
    return reinterpret_cast<const Structure*>(data_.data());
  }

  const Structure* next_structure(const Structure* const s) const noexcept
  {
    DMITIGR_ASSERT(s);
    bool is_prev_char_zero{};
    const auto length = static_cast<std::ptrdiff_t>(data_.size());
    const auto* const fst = first_structure();
    for (const char* ptr{unformed_section(s)};
         ptr + 1 - reinterpret_cast<const char*>(fst) < length; ++ptr) {
      if (*ptr == 0) {
        if (is_prev_char_zero)
          return reinterpret_cast<const Structure*>(ptr + 1);
        else
          is_prev_char_zero = true;
      } else
        is_prev_char_zero = false;
    }
    return nullptr;
  }

  template<typename T>
  static T field(const Structure* const s, const std::ptrdiff_t offset)
  {
    DMITIGR_ASSERT(offset >= 0x0);
    using Dt = std::decay_t<T>;
    const Byte* const ptr = reinterpret_cast<const Byte*>(s) + offset;
    if constexpr (std::is_same_v<Dt, std::optional<std::string>>) {
      const int idx = *ptr;
      if (!idx)
        return std::nullopt;
      const char* str = unformed_section(s);
      for (int i{1}; i < idx; ++i) {
        std::string_view view{str};
        str = view.data() + view.size() + 1;
        DMITIGR_ASSERT(*str != 0);
      }
      return std::string{str};
    } else if constexpr (Is_std_array<Dt>::value) {
      using V = typename Dt::value_type;
      if constexpr (std::is_same_v<V, Byte>) {
        Dt result;
        std::memcpy(result.data(), ptr, result.size());
        return result;
      } else
        static_assert(false_value<V>, "unsupported type");
    } else if constexpr (
      std::is_same_v<Dt, Byte>  || std::is_same_v<Dt, Word> ||
      std::is_same_v<Dt, Dword> || std::is_same_v<Dt, Qword>) {
      return *reinterpret_cast<const Dt*>(ptr);
    } else
      static_assert(false_value<T>, "unsupported type");
  }

  static std::vector<Byte> read_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      throw std::runtime_error(
        std::format("{} - is not exists", path.string()));
    }
    if (!std::filesystem::is_regular_file(path)) {
      throw std::runtime_error(
        std::format("{} - is not regular file", path.string()));
    }
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path, std::ios::in | std::ios::binary | std::ios::ate);
    std::vector<Byte> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(
      reinterpret_cast<char*>(buffer.data()),
      static_cast<std::streamsize>(buffer.size())
    );
    return buffer;
  }

  static Version version() {
    // Header is immutable, we can cache it for performance
    // purposes
    static std::optional<Version> version = std::nullopt;
    if (version) {
      return version.value();
    }

#ifdef _WIN32
    // TODO: todo
#elif __linux__
    using namespace std::literals;
    const std::vector<Byte> header_data
      = read_file("/sys/firmware/dmi/tables/smbios_entry_point");

    // https://github.com/mirror/dmidecode/blob/484f8935b0fc768841f43fa388b191196b5e12fd/dmidecode.c#L6068
    if (header_data.size() < 24)
      throw std::runtime_error(std::format(
        "Invalid smbios type - 'header_data.size() < 24'"));

    const std::string_view head_marker(
      reinterpret_cast<const char*>(header_data.data()),
      reinterpret_cast<const char*>(header_data.data() + 5));

    if (head_marker != "_SM3_"sv)
      throw std::runtime_error(
        "Invalid smbios type - header mark is not '_SM3_'");

    // https://github.com/mirror/dmidecode/blob/484f8935b0fc768841f43fa388b191196b5e12fd/dmidecode.c#L5721C6-L5721C26
    if (header_data[0x06] > 0x20)
      throw std::runtime_error(std::format(
        "Entry point length too large ({} bytes, expected {})",
        static_cast<unsigned int>(header_data[0x06]), 0x18U));

    version = Version{
      .major_version = header_data[0x07],
      .minor_version = header_data[0x08],
      .dmi_revision = header_data[0x09],
    };
#else
    #error Unsupported OS family
#endif
    return version.value();
  }

public:
  static std::vector<Byte> load_raw_data() {
    std::vector<Byte> data;
#ifdef _WIN32
    // TODO: todo
#elif __linux__
    data = read_file("/sys/firmware/dmi/tables/DMI");
#else
    #error Unsupported OS family
#endif
    return data;
  }
};

} // namespace dmitigr::os::firmware

#endif  // DMITIGR_OS_SMBIOS_HPP
