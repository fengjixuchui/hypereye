#pragma once

#include <cstdint>

struct segment_t
{
    /// @brief
    ///
    uint64_t selector;

    /// @brief
    ///
    uint64_t limit;

    /// @brief
    ///
    uint64_t rights;

    /// @brief
    ///
    uint64_t base;
};

struct es_t   : public segment_t {};
struct cs_t   : public segment_t {};
struct ss_t   : public segment_t {};
struct ds_t   : public segment_t {};
struct fs_t   : public segment_t {};
struct gs_t   : public segment_t {};
struct tr_t   : public segment_t {};
struct ldtr_t : public segment_t {};

struct selector_t
{
    union
    {
        uint16_t flags;

        struct
        {
            /// @brief Requested Privilege Level.
            ///
            uint16_t rpl    : 2;
            /// @brief Table Indicator.
            ///
            uint16_t ti     : 1;
            /// @brief Index.
            ///
            uint16_t index  : 13;
        };
    };
};
static_assert(sizeof(selector_t) == sizeof(uint16_t), "segment selector size mismatch");

struct descriptor_t
{
    union
    {
        uint64_t flags;

        struct
        {
            /// @brief Segment Limit.
            ///
            uint64_t limit_low  : 16;
            /// @brief Segment base address.
            ///
            uint64_t base_low   : 16;
            /// @brief Segment base address.
            ///
            uint64_t base_mid   : 8;
            /// @brief Segment type.
            ///
            uint64_t type       : 4;
            /// @brief Descriptor type.
            ///
            uint64_t system     : 1;
            /// @brief Descriptor privilege level.
            ///
            uint64_t dpl        : 2;
            /// @brief Segment present.
            ///
            uint64_t present    : 1;
            /// @brief Segment Limit.
            ///
            uint64_t limit_high : 4;
            /// @brief Available for use by system software.
            ///
            uint64_t avl        : 1;
            /// @brief 64-bit code segment (IA-32e mode only).
            ///
            uint64_t l          : 1;
            /// @brief Default operation size (0 = 16-bit segment; 1 = 32-bit segment).
            ///
            uint64_t db         : 1;
            /// @brief Granularity.
            ///
            uint64_t granularity: 1;
            /// @brief Segment base address.
            ///
            uint64_t base_high  : 8;
        };
    };

    /// @brief Segment base address.
    ///
    uint64_t base_upper : 32;

    inline uint64_t base() const
    {
        return base_low | base_mid << 16 | base_high << 24 | base_upper << 32;
    }

    inline uint64_t limit() const
    {
        return limit_low | limit_high << 8;
    }
};
static_assert(sizeof(descriptor_t) ==  16, "segment descriptor size mismatch");

/// @brief Descriptor table register.
///
#pragma pack(push, 1)
struct gdtr_t
{
    /// @brief
    ///
    uint16_t limit;
    /// @brief
    ///
    uint64_t base;
};
#pragma pack(pop)
static_assert(sizeof(gdtr_t) == 10, "Gdtr size mismatch");

/// @brief Same for interrupt descriptor table register.
///
struct idtr_t : public gdtr_t {};
static_assert(sizeof(idtr_t) == 10, "Idtr size mismatch");
