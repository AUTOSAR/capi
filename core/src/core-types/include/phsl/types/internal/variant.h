// Modified by iSOFT Infrastructure Software Co., Ltd. on 2022-06-24
//
// Copyright 2018 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Implementation details of phsl/types/variant.h, pulled into a
// separate file to avoid cluttering the top of the API header with
// implementation details.

#ifndef PBSL_TYPES_INTERNAL_VARIANT_H_
#define PBSL_TYPES_INTERNAL_VARIANT_H_

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "phsl/base/config.h"
#include "phsl/base/internal/identity.h"
#include "phsl/base/internal/inline_variable.h"
#include "phsl/base/internal/invoke.h"
#include "phsl/base/macros.h"
#include "phsl/base/optimization.h"
#include "phsl/meta/type_traits.h"
#include "phsl/types/bad_variant_access.h"
#include "phsl/utility/utility.h"

#if !defined(PBSL_USES_STD_VARIANT)

namespace phsl {
PBSL_NAMESPACE_BEGIN

template < class... Types >
class variant;

PBSL_INTERNAL_INLINE_CONSTEXPR(size_t, variant_npos, static_cast< size_t >(-1));

template < class T >
struct variant_size;

template < std::size_t I, class T >  // NOLINT : keep template parameter naming style
struct variant_alternative;

namespace variant_internal {

// NOTE: See specializations below for details.
template < std::size_t I, class T >  // NOLINT : keep template parameter naming style
struct VariantAlternativeSfinae
{
};

// Requires: I < variant_size_v<T>.
//
// Value: The Ith type of Types...
template < std::size_t I,
           class T0,
           class... Tn >  // NOLINT : keep template parameter naming style
struct VariantAlternativeSfinae< I, variant< T0, Tn... > > : VariantAlternativeSfinae< I - 1, variant< Tn... > >
{
};

// Value: T0
template < class T0, class... Ts >
struct VariantAlternativeSfinae< 0, variant< T0, Ts... > >
{
    using type = T0;  // NOLINT
};

template < std::size_t I, class T >  // NOLINT : keep template parameter naming style
using VariantAlternativeSfinaeT = typename VariantAlternativeSfinae< I, T >::type;

// NOTE: Requires T to be a reference type.
template < class T, class U >
struct GiveQualsTo;

template < class T, class U >
struct GiveQualsTo< T &, U >
{
    using type = U &;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< T &&, U >
{
    using type = U &&;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< const T &, U >
{
    using type = const U &;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< const T &&, U >
{
    using type = const U &&;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< volatile T &, U >
{
    using type = volatile U &;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< volatile T &&, U >
{
    using type = volatile U &&;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< volatile const T &, U >
{
    using type = volatile const U &;  // NOLINT
};

template < class T, class U >
struct GiveQualsTo< volatile const T &&, U >
{
    using type = volatile const U &&;  // NOLINT
};

template < class T, class U >
using GiveQualsToT = typename GiveQualsTo< T, U >::type;

// Convenience alias, since size_t integral_constant is used a lot in this file.
template < std::size_t I >  // NOLINT : keep template parameter naming style
using SizeT = std::integral_constant< std::size_t, I >;

using NPos = SizeT< variant_npos >;

template < class Variant, class T, class = void >
struct IndexOfConstructedType
{
};

template < std::size_t I, class Variant >  // NOLINT : keep template parameter naming style
struct VariantAccessResultImpl;

template < std::size_t I,
           template < class... >
           class Variantemplate,
           class... T >  // NOLINT : keep template parameter naming style
struct VariantAccessResultImpl< I, Variantemplate< T... > & >
{
    using type = typename phsl::variant_alternative< I, variant< T... > >::type &;  // NOLINT
};

template < std::size_t I,
           template < class... >
           class Variantemplate,
           class... T >  // NOLINT : keep template parameter naming style
struct VariantAccessResultImpl< I, const Variantemplate< T... > & >
{
    using type = const typename phsl::variant_alternative< I, variant< T... > >::type &;  // NOLINT
};

template < std::size_t I,
           template < class... >
           class Variantemplate,
           class... T >  // NOLINT : keep template parameter naming style
struct VariantAccessResultImpl< I, Variantemplate< T... > && >
{
    using type = typename phsl::variant_alternative< I, variant< T... > >::type &&;  // NOLINT
};

template < std::size_t I,
           template < class... >
           class Variantemplate,
           class... T >  // NOLINT : keep template parameter naming style
struct VariantAccessResultImpl< I, const Variantemplate< T... > && >
{
    using type = const typename phsl::variant_alternative< I, variant< T... > >::type &&;  // NOLINT
};

template < std::size_t I, class Variant >  // NOLINT : keep template parameter naming style
using VariantAccessResult = typename VariantAccessResultImpl< I, Variant && >::type;

// NOTE: This is used instead of std::array to reduce instantiation overhead.
template < class T, std::size_t Size >  // NOLINT : keep template parameter naming style
struct SimpleArray
{
    static_assert(Size != 0, "");
    T value[Size];
};

template < class T >
struct AccessedType
{
    using type = T;  // NOLINT
};

template < class T >
using AccessedTypeT = typename AccessedType< T >::type;

template < class T, std::size_t Size >  // NOLINT : keep template parameter naming style
struct AccessedType< SimpleArray< T, Size > >
{
    using type = AccessedTypeT< T >;  // NOLINT
};

template < class T >
constexpr T AccessSimpleArray(const T &value)
{
    return value;
}

template < class T,
           std::size_t Size,
           class... SizeT >  // NOLINT : keep template parameter naming style
constexpr AccessedTypeT< T > AccessSimpleArray(const SimpleArray< T, Size > &table,
                                               std::size_t head_index,  // NOLINT
                                               SizeT... tail_indices)   // NOLINT
{
    return AccessSimpleArray(table.value[head_index], tail_indices...);
}

// Note: Intentionally is an alias.
template < class T >
using AlwaysZero = SizeT< 0 >;

template < class Op, class... Vs >
struct VisitIndicesResultImpl
{
    using type = phsl::result_of_t< Op(AlwaysZero< Vs >...) >;  // NOLINT
};

template < class Op, class... Vs >
using VisitIndicesResultT = typename VisitIndicesResultImpl< Op, Vs... >::type;

template < class ReturnType, class FunctionObject, class EndIndices, class BoundIndices >
struct MakeVisitationMatrix;

template < class ReturnType,
           class FunctionObject,
           std::size_t... Indices >                                // NOLINT : keep template parameter naming style
constexpr ReturnType call_with_indices(FunctionObject &&function)  // NOLINT
{
    static_assert(std::is_same< ReturnType, decltype(std::declval< FunctionObject >()(SizeT< Indices >()...)) >::value,
                  "Not all visitation overloads have the same return type.");
    return std::forward< FunctionObject >(function)(SizeT< Indices >()...);
}

template < class ReturnType,
           class FunctionObject,
           std::size_t... BoundIndices >  // NOLINT : keep template parameter naming style
struct MakeVisitationMatrix< ReturnType, FunctionObject, index_sequence<>, index_sequence< BoundIndices... > >
{
    using ResultType = ReturnType (*)(FunctionObject &&);
    static constexpr ResultType Run()
    {
        return &call_with_indices< ReturnType, FunctionObject, (BoundIndices - 1)... >;
    }
};

template < typename Is, std::size_t J >  // NOLINT : keep template parameter naming style
struct AppendToIndexSequence;

template < typename Is, std::size_t J >  // NOLINT : keep template parameter naming style
using AppendToIndexSequenceT = typename AppendToIndexSequence< Is, J >::type;

template < std::size_t... Is, std::size_t J >  // NOLINT : keep template parameter naming style
struct AppendToIndexSequence< index_sequence< Is... >, J >
{
    using type = index_sequence< Is..., J >;  // NOLINT : keep template parameter naming style
};

template < class ReturnType, class FunctionObject, class EndIndices, class CurrIndices, class BoundIndices >
struct MakeVisitationMatrixImpl;

template < class ReturnType,
           class FunctionObject,
           class EndIndices,
           std::size_t... CurrIndices,  // NOLINT : keep template parameter naming style
           class BoundIndices >
struct MakeVisitationMatrixImpl< ReturnType,
                                 FunctionObject,
                                 EndIndices,
                                 index_sequence< CurrIndices... >,
                                 BoundIndices >
{
    using ResultType = SimpleArray<
        typename MakeVisitationMatrix< ReturnType, FunctionObject, EndIndices, index_sequence<> >::ResultType,
        sizeof...(CurrIndices) >;

    static constexpr ResultType Run()
    {
        return {{MakeVisitationMatrix< ReturnType, FunctionObject, EndIndices,
                                       AppendToIndexSequenceT< BoundIndices, CurrIndices > >::Run()...}};
    }
};

template < class ReturnType,
           class FunctionObject,
           std::size_t HeadEndIndex,       // NOLINT : keep template parameter naming style
           std::size_t... TailEndIndices,  // NOLINT : keep template parameter naming style
           std::size_t... BoundIndices >   // NOLINT : keep template parameter naming style
struct MakeVisitationMatrix< ReturnType,
                             FunctionObject,
                             index_sequence< HeadEndIndex, TailEndIndices... >,
                             index_sequence< BoundIndices... > >
    : MakeVisitationMatrixImpl< ReturnType,
                                FunctionObject,
                                index_sequence< TailEndIndices... >,
                                phsl::make_index_sequence< HeadEndIndex >,
                                index_sequence< BoundIndices... > >
{
};

struct UnreachableSwitchCase
{
    template < class Op >
    [[noreturn]] static VisitIndicesResultT< Op, std::size_t > Run(Op && /*ignored*/)
    {
        PBSL_UNREACHABLE();
    }
};

template < class Op, std::size_t I >  // NOLINT : keep template parameter naming style
struct ReachableSwitchCase
{
    static VisitIndicesResultT< Op, std::size_t > Run(Op &&op)
    {
        return phsl::base_internal::invoke(std::forward< Op >(op), SizeT< I >());
    }
};

// The number 33 is just a guess at a reasonable maximum to our switch. It is
// not based on any analysis. The reason it is a power of 2 plus 1 instead of a
// power of 2 is because the number was picked to correspond to a power of 2
// amount of "normal" alternatives, plus one for the possibility of the user
// providing "monostate" in addition to the more natural alternatives.
PBSL_INTERNAL_INLINE_CONSTEXPR(std::size_t, MaxUnrolledVisitCases, 33);

// Note: The default-definition is for unreachable cases.
template < bool IsReachable >  // NOLINT : keep template parameter naming style
struct PickCaseImpl
{
    template < class Op, std::size_t I >  // NOLINT : keep template parameter naming style
    using Apply = UnreachableSwitchCase;
};

template <>
struct PickCaseImpl< /*IsReachable =*/true >
{
    template < class Op, std::size_t I >  // NOLINT : keep template parameter naming style
    using Apply = ReachableSwitchCase< Op, I >;
};

// Note: This form of dance with template aliases is to make sure that we
//       instantiate a number of templates proportional to the number of variant
//       alternatives rather than a number of templates proportional to our
//       maximum unrolled amount of visitation cases (aliases are effectively
//       "free" whereas other template instantiations are costly).
template < class Op,
           std::size_t I,
           std::size_t EndIndex >  // NOLINT : keep template parameter naming style
using PickCase = typename PickCaseImpl< (I < EndIndex) >::template Apply< Op, I >;

template < class ReturnType >
[[noreturn]] ReturnType TypedThrowBadVariantAccess()
{
    phsl::variant_internal::ThrowBadVariantAccess();
}

// Given N variant sizes, determine the number of cases there would need to be
// in a single switch-statement that would cover every possibility in the
// corresponding N-ary visit operation.
template < std::size_t... NumAlternatives >  // NOLINT : keep template parameter naming style
struct NumCasesOfSwitch;

template < std::size_t HeadNumAlternatives,
           std::size_t... TailNumAlternatives >  // NOLINT : keep template parameter naming style
struct NumCasesOfSwitch< HeadNumAlternatives, TailNumAlternatives... >
{
    static constexpr std::size_t value  // NOLINT
        = (HeadNumAlternatives + 1) * NumCasesOfSwitch< TailNumAlternatives... >::value;
};

template <>
struct NumCasesOfSwitch<>
{
    static constexpr std::size_t value = 1;  // NOLINT
};

// A switch statement optimizes better than the table of function pointers.
template < std::size_t EndIndex >  // NOLINT : keep template parameter naming style
struct VisitIndicesSwitch
{
    static_assert(EndIndex <= MaxUnrolledVisitCases, "Maximum unrolled switch size exceeded.");

    template < class Op >
    static VisitIndicesResultT< Op, std::size_t > Run(Op &&op, std::size_t i)
    {
        constexpr std::size_t kSizeT_5{5U};
        constexpr std::size_t kSizeT_6{6U};
        constexpr std::size_t kSizeT_7{7U};
        constexpr std::size_t kSizeT_8{8U};
        constexpr std::size_t kSizeT_9{9U};
        constexpr std::size_t kSizeT_10{10U};
        constexpr std::size_t kSizeT_11{11U};
        constexpr std::size_t kSizeT_12{12U};
        constexpr std::size_t kSizeT_13{13U};
        constexpr std::size_t kSizeT_14{14U};
        constexpr std::size_t kSizeT_15{15U};
        constexpr std::size_t kSizeT_16{16U};
        constexpr std::size_t kSizeT_17{17U};
        constexpr std::size_t kSizeT_18{18U};
        constexpr std::size_t kSizeT_19{19U};
        constexpr std::size_t kSizeT_20{20U};
        constexpr std::size_t kSizeT_21{21U};
        constexpr std::size_t kSizeT_22{22U};
        constexpr std::size_t kSizeT_23{23U};
        constexpr std::size_t kSizeT_24{24U};
        constexpr std::size_t kSizeT_25{25U};
        constexpr std::size_t kSizeT_26{26U};
        constexpr std::size_t kSizeT_27{27U};
        constexpr std::size_t kSizeT_28{28U};
        constexpr std::size_t kSizeT_29{29U};
        constexpr std::size_t kSizeT_30{30U};
        constexpr std::size_t kSizeT_31{31U};
        constexpr std::size_t kSizeT_32{32U};

        switch (i) {
            case 0:
                return PickCase< Op, 0, EndIndex >::Run(std::forward< Op >(op));
            case 1:
                return PickCase< Op, 1, EndIndex >::Run(std::forward< Op >(op));
            case 2:
                return PickCase< Op, 2, EndIndex >::Run(std::forward< Op >(op));
            case 3:
                return PickCase< Op, 3, EndIndex >::Run(std::forward< Op >(op));
            case 4:
                return PickCase< Op, 4, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_5:
                return PickCase< Op, kSizeT_5, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_6:
                return PickCase< Op, kSizeT_6, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_7:
                return PickCase< Op, kSizeT_7, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_8:
                return PickCase< Op, kSizeT_8, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_9:
                return PickCase< Op, kSizeT_9, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_10:
                return PickCase< Op, kSizeT_10, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_11:
                return PickCase< Op, kSizeT_11, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_12:
                return PickCase< Op, kSizeT_12, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_13:
                return PickCase< Op, kSizeT_13, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_14:
                return PickCase< Op, kSizeT_14, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_15:
                return PickCase< Op, kSizeT_15, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_16:
                return PickCase< Op, kSizeT_16, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_17:
                return PickCase< Op, kSizeT_17, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_18:
                return PickCase< Op, kSizeT_18, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_19:
                return PickCase< Op, kSizeT_19, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_20:
                return PickCase< Op, kSizeT_20, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_21:
                return PickCase< Op, kSizeT_21, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_22:
                return PickCase< Op, kSizeT_22, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_23:
                return PickCase< Op, kSizeT_23, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_24:
                return PickCase< Op, kSizeT_24, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_25:
                return PickCase< Op, kSizeT_25, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_26:
                return PickCase< Op, kSizeT_26, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_27:
                return PickCase< Op, kSizeT_27, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_28:
                return PickCase< Op, kSizeT_28, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_29:
                return PickCase< Op, kSizeT_29, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_30:
                return PickCase< Op, kSizeT_30, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_31:
                return PickCase< Op, kSizeT_31, EndIndex >::Run(std::forward< Op >(op));
            case kSizeT_32:
                return PickCase< Op, kSizeT_32, EndIndex >::Run(std::forward< Op >(op));
            default:
                PBSL_ASSERT(i == variant_npos);
                return phsl::base_internal::invoke(std::forward< Op >(op), NPos());
        }
    }
};

template < std::size_t... EndIndices >  // NOLINT : keep template parameter naming style
struct VisitIndicesFallback
{
    template < class Op, class... SizeT >
    static VisitIndicesResultT< Op, SizeT... > Run(Op &&op, SizeT... indices)
    {
        return AccessSimpleArray(MakeVisitationMatrix< VisitIndicesResultT< Op, SizeT... >, Op,
                                                       index_sequence< (EndIndices + 1)... >, index_sequence<> >::Run(),
                                 (indices + 1)...)(std::forward< Op >(op));
    }
};

// Take an N-dimensional series of indices and convert them into a single index
// without loss of information. The purpose of this is to be able to convert an
// N-ary visit operation into a single switch statement.
template < std::size_t... >
struct FlattenIndices;

template < std::size_t HeadSize,
           std::size_t... TailSize >  // NOLINT : keep template parameter naming style
struct FlattenIndices< HeadSize, TailSize... >
{
    template < class... SizeType >
    static constexpr std::size_t Run(std::size_t head, SizeType... tail)
    {
        return head + HeadSize * FlattenIndices< TailSize... >::Run(tail...);
    }
};

template <>
struct FlattenIndices<>
{
    static constexpr std::size_t Run() { return 0; }
};

// Take a single "flattened" index (flattened by FlattenIndices) and determine
// the value of the index of one of the logically represented dimensions.
template < std::size_t I,             // NOLINT
           std::size_t IndexToGet,    // NOLINT
           std::size_t HeadSize,      // NOLINT
           std::size_t... TailSize >  // NOLINT : keep template parameter naming style
struct UnflattenIndex
{
    static constexpr std::size_t value = UnflattenIndex< I / HeadSize, IndexToGet - 1,
                                                         TailSize... >::value;  // NOLINT
};

template < std::size_t I,
           std::size_t HeadSize,
           std::size_t... TailSize >  // NOLINT : keep template parameter naming style
struct UnflattenIndex< I, 0, HeadSize, TailSize... >
{
    static constexpr std::size_t value = (I % HeadSize);  // NOLINT
};

// The backend for converting an N-ary visit operation into a unary visit.
template < class IndexSequence,
           std::size_t... EndIndices >  // NOLINT : keep template parameter naming style
struct VisitIndicesVariadicImpl;

template < std::size_t... N,
           std::size_t... EndIndices >  // NOLINT : keep template parameter naming style
struct VisitIndicesVariadicImpl< phsl::index_sequence< N... >, EndIndices... >
{
    // A type that can take an N-ary function object and converts it to a unary
    // function object that takes a single, flattened index, and "unflattens" it
    // into its individual dimensions when forwarding to the wrapped object.
    template < class Op >
    struct FlattenedOp
    {
        template < std::size_t I >  // NOLINT : keep template parameter naming style
        VisitIndicesResultT< Op, decltype(EndIndices)... > operator()(SizeT< I > /*index*/) &&
        {
            return base_internal::invoke(
                std::forward< Op >(op),
                SizeT< UnflattenIndex< I, N, (EndIndices + 1)... >::value - std::size_t{1} >()...);
        }

        Op &&op;
    };

    template < class Op, class... SizeType >
    static VisitIndicesResultT< Op, decltype(EndIndices)... > Run(Op &&op, SizeType... i)
    {
        return VisitIndicesSwitch< NumCasesOfSwitch< EndIndices... >::value >::Run(
            FlattenedOp< Op >{std::forward< Op >(op)},
            FlattenIndices< (EndIndices + std::size_t{1})... >::Run((i + std::size_t{1})...));
    }
};

template < std::size_t... EndIndices >  // NOLINT : keep template parameter naming style
struct VisitIndicesVariadic
    : VisitIndicesVariadicImpl< phsl::make_index_sequence< sizeof...(EndIndices) >, EndIndices... >
{
};

// This implementation will flatten N-ary visit operations into a single switch
// statement when the number of cases would be less than our maximum specified
// switch-statement size.
// TODO:(calabrese)
//   Based on benchmarks, determine whether the function table approach actually
//   does optimize better than a chain of switch statements and possibly update
//   the implementation accordingly. Also consider increasing the maximum switch
//   size.
template < std::size_t... EndIndices >  // NOLINT : keep template parameter naming style
struct VisitIndices
    : phsl::conditional_t< (NumCasesOfSwitch< EndIndices... >::value <= MaxUnrolledVisitCases),
                           VisitIndicesVariadic< EndIndices... >,
                           VisitIndicesFallback< EndIndices... > >
{
};

template < std::size_t EndIndex >  // NOLINT : keep template parameter naming style
struct VisitIndices< EndIndex >
    : phsl::conditional_t< (EndIndex <= MaxUnrolledVisitCases),
                           VisitIndicesSwitch< EndIndex >,
                           VisitIndicesFallback< EndIndex > >
{
};

    // Suppress bogus warning on MSVC: MSVC complains that the `reinterpret_cast`
    // below is returning the address of a temporary or local object.
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable : 4172)
    #endif  // _MSC_VER

// TODO:(calabrese) std::launder
// TODO:(calabrese) constexpr
// NOTE: DO NOT REMOVE the `inline` keyword as it is necessary to work around a
// MSVC bug. See https://github.com/abseil/abseil-cpp/issues/129 for details.
template < class Self, std::size_t I >  // NOLINT : keep template parameter naming style
inline VariantAccessResult< I, Self > AccessUnion(Self &&self, SizeT< I > /*i*/)
{
    return reinterpret_cast< VariantAccessResult< I, Self > >(self);
}

    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif  // _MSC_VER

template < class T >
void DeducedDestroy(T &self)
{
    self.~T();
}

// NOTE: This type exists as a single entity for variant and its bases to
// befriend. It contains helper functionality that manipulates the state of the
// variant, such as the implementation of things like assignment and emplace
// operations.
struct VariantCoreAccess
{
    template < class VariantType >
    static typename VariantType::Variant &Derived(VariantType &self)
    {
        return static_cast< typename VariantType::Variant & >(self);
    }

    template < class VariantType >
    static const typename VariantType::Variant &Derived(const VariantType &self)
    {
        return static_cast< const typename VariantType::Variant & >(self);
    }

    template < class VariantType >
    static void Destroy(VariantType &self)
    {
        Derived(self).destroy();
        self.index_ = phsl::variant_npos;
    }

    template < class Variant >
    static void SetIndex(Variant &self, std::size_t i)
    {
        self.index_ = i;
    }

    template < class Variant >
    static void InitFrom(Variant &self, Variant &&other)
    {
        VisitIndices< phsl::variant_size< Variant >::value >::Run(
            InitFromVisitor< Variant, Variant && >{&self, std::forward< Variant >(other)}, other.index());
        self.index_ = other.index();
    }

    // Access a variant alternative, assuming the index is correct.
    template < std::size_t I, class Variant >  // NOLINT : keep template parameter naming style
    static VariantAccessResult< I, Variant > Access(Variant &&self)
    {
        // This cast instead of invocation of AccessUnion with an rvalue is a
        // workaround for msvc. Without this there is a runtime failure when dealing
        // with rvalues.
        // TODO:(calabrese) Reduce test case and find a simpler workaround.
        return static_cast< VariantAccessResult< I, Variant > >(
            variant_internal::AccessUnion(self.state_, SizeT< I >()));
    }

    // Access a variant alternative, throwing if the index is incorrect.
    template < std::size_t I, class Variant >  // NOLINT : keep template parameter naming style
    static VariantAccessResult< I, Variant > CheckedAccess(Variant &&self)
    {
        if (PBSL_PREDICT_FALSE(self.index_ != I)) {
            TypedThrowBadVariantAccess< VariantAccessResult< I, Variant > >();
        }

        return Access< I >(std::forward< Variant >(self));
    }

    // The implementation of the move-assignment operation for a variant.
    template < class VType >
    struct MoveAssignVisitor
    {
        using DerivedType = typename VType::Variant;
        template < std::size_t NewIndex >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< NewIndex > /*new_i*/) const
        {
            if (left->index_ == NewIndex) {
                Access< NewIndex >(*left) = std::move(Access< NewIndex >(*right));
            } else {
                Derived(*left).template emplace< NewIndex >(std::move(Access< NewIndex >(*right)));
            }
        }

        void operator()(SizeT< phsl::variant_npos > /*new_i*/) const { Destroy(*left); }

        VType *left;
        VType *right;
    };

    template < class VType >
    static MoveAssignVisitor< VType > MakeMoveAssignVisitor(VType *left, VType *other)
    {
        return {left, other};
    }

    // The implementation of the assignment operation for a variant.
    template < class VType >
    struct CopyAssignVisitor
    {
        using DerivedType = typename VType::Variant;
        template < std::size_t NewIndex >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< NewIndex > /*new_i*/) const
        {
            using New = typename phsl::variant_alternative< NewIndex, DerivedType >::type;

            if (left->index_ == NewIndex) {
                Access< NewIndex >(*left) = Access< NewIndex >(*right);
            } else if (std::is_nothrow_copy_constructible< New >::value
                       || !std::is_nothrow_move_constructible< New >::value) {
                Derived(*left).template emplace< NewIndex >(Access< NewIndex >(*right));
            } else {
                Derived(*left) = DerivedType(Derived(*right));
            }
        }

        void operator()(SizeT< phsl::variant_npos > /*new_i*/) const { Destroy(*left); }

        VType *left;
        const VType *right;
    };

    template < class VType >
    static CopyAssignVisitor< VType > MakeCopyAssignVisitor(VType *left, const VType &other)
    {
        return {left, &other};
    }

    // The implementation of conversion-assignment operations for variant.
    template < class Left, class QualifiedNew >
    struct ConversionAssignVisitor
    {
        using NewIndex = variant_internal::IndexOfConstructedType< Left, QualifiedNew >;

        void operator()(SizeT< NewIndex::value > /*old_i*/
        ) const
        {
            Access< NewIndex::value >(*left) = std::forward< QualifiedNew >(other);
        }

        template < std::size_t OldIndex >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< OldIndex >  /*old_i*/
        ) const
        {
            using New = typename phsl::variant_alternative< NewIndex::value, Left >::type;
            if (std::is_nothrow_constructible< New, QualifiedNew >::value
                || !std::is_nothrow_move_constructible< New >::value) {
                left->template emplace< NewIndex::value >(std::forward< QualifiedNew >(other));
            } else {
                // the standard says "equivalent to
                // operator=(variant(std::forward<T>(t)))", but we use `emplace` here
                // because the variant's move assignment operator could be deleted.
                left->template emplace< NewIndex::value >(New(std::forward< QualifiedNew >(other)));
            }
        }

        Left *left;
        QualifiedNew &&other;
    };

    template < class Left, class QualifiedNew >
    static ConversionAssignVisitor< Left, QualifiedNew > MakeConversionAssignVisitor(Left *left, QualifiedNew &&qual)
    {
        return {left, std::forward< QualifiedNew >(qual)};
    }

    // Backend for operations for `emplace()` which destructs `*self` then
    // construct a new alternative with `Args...`.
    template < std::size_t NewIndex,
               class Self,
               class... Args >  // NOLINT : keep template parameter naming style
    static typename phsl::variant_alternative< NewIndex, Self >::type &Replace(Self *self, Args &&...args)
    {
        Destroy(*self);
        using New         = typename phsl::variant_alternative< NewIndex, Self >::type;
        New *const result = ::new (static_cast< void * >(&self->state_)) New(std::forward< Args >(args)...);
        self->index_      = NewIndex;
        return *result;
    }

    template < class LeftVariant, class QualifiedRightVariant >
    struct InitFromVisitor
    {
        template < std::size_t NewIndex >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< NewIndex > /*new_i*/) const
        {
            using Alternative = typename variant_alternative< NewIndex, LeftVariant >::type;
            ::new (static_cast< void * >(&left->state_))
                Alternative(Access< NewIndex >(std::forward< QualifiedRightVariant >(right)));
        }

        void operator()(SizeT< phsl::variant_npos > /*new_i*/) const
        {
            // This space intentionally left blank.
        }
        LeftVariant *left;
        QualifiedRightVariant &&right;
    };
};

template < class Expected, class... T >
struct IndexOfImpl;

template < class Expected >
struct IndexOfImpl< Expected >
{
    using IndexFromEnd        = SizeT< 0 >;
    using MatchedIndexFromEnd = IndexFromEnd;
    using MultipleMatches     = std::false_type;
};

template < class Expected, class Head, class... Tail >
struct IndexOfImpl< Expected, Head, Tail... > : IndexOfImpl< Expected, Tail... >
{
    using IndexFromEnd = SizeT< IndexOfImpl< Expected, Tail... >::IndexFromEnd::value + 1 >;
};

template < class Expected, class... Tail >
struct IndexOfImpl< Expected, Expected, Tail... > : IndexOfImpl< Expected, Tail... >
{
    using IndexFromEnd        = SizeT< IndexOfImpl< Expected, Tail... >::IndexFromEnd::value + 1 >;
    using MatchedIndexFromEnd = IndexFromEnd;
    using MultipleMatches
        = std::integral_constant< bool, IndexOfImpl< Expected, Tail... >::MatchedIndexFromEnd::value != 0 >;
};

template < class Expected, class... Types >
struct IndexOfMeta
{
    using Results = IndexOfImpl< Expected, Types... >;
    static_assert(!Results::MultipleMatches::value,
                  "Attempted to access a variant by specifying a type that "
                  "matches more than one alternative.");
    static_assert(Results::MatchedIndexFromEnd::value != 0,
                  "Attempted to access a variant by specifying a type that does "
                  "not match any alternative.");
    using type = SizeT< sizeof...(Types) - Results::MatchedIndexFromEnd::value >;  // NOLINT
};

template < class Expected, class... Types >
using IndexOf = typename IndexOfMeta< Expected, Types... >::type;

template < class Variant,
           class T,
           std::size_t CurrIndex >  // NOLINT : keep template parameter naming style
struct UnambiguousIndexOfImpl;

// Terminating case encountered once we've checked all of the alternatives
template < class T, std::size_t CurrIndex >  // NOLINT : keep template parameter naming style
struct UnambiguousIndexOfImpl< variant<>, T, CurrIndex > : SizeT< CurrIndex >
{
};

// Case where T is not Head
template < class Head,
           class... Tail,
           class T,
           std::size_t CurrIndex >  // NOLINT : keep template parameter naming style
struct UnambiguousIndexOfImpl< variant< Head, Tail... >, T, CurrIndex >
    : UnambiguousIndexOfImpl< variant< Tail... >, T, CurrIndex + 1 >::type
{
};

// Case where T is Head
template < class Head,
           class... Tail,
           std::size_t CurrIndex >  // NOLINT : keep template parameter naming style
struct UnambiguousIndexOfImpl< variant< Head, Tail... >, Head, CurrIndex >
    : SizeT< UnambiguousIndexOfImpl< variant< Tail... >, Head, 0 >::value == sizeof...(Tail)
                 ? CurrIndex
                 : CurrIndex + sizeof...(Tail) + 1 >
{
};

template < class Variant, class T >
struct UnambiguousIndexOf;

struct NoMatch
{
    struct type  // NOLINT : keep template parameter naming style
    {
    };
};

template < class... Alts, class T >
struct UnambiguousIndexOf< variant< Alts... >, T >
    : std::conditional< UnambiguousIndexOfImpl< variant< Alts... >, T, 0 >::value != sizeof...(Alts),
                        UnambiguousIndexOfImpl< variant< Alts... >, T, 0 >,
                        NoMatch >::type::type
{
};

template < class T, std::size_t /*Dummy*/ >
using UnambiguousTypeOfImpl = T;

template < class Variant, class T >
using UnambiguousTypeOfT = UnambiguousTypeOfImpl< T, UnambiguousIndexOf< Variant, T >::value >;

template < class H, class... T >
class VariantStateBase;

// This is an implementation of the "imaginary function" that is described in
// [variant.ctor]
// It is used in order to determine which alternative to construct during
// initialization from some type T.
template < class Variant, std::size_t I = 0 >  // NOLINT : keep template parameter naming style
struct ImaginaryFun;

template < std::size_t I >  // NOLINT : keep template parameter naming style
struct ImaginaryFun< variant<>, I >
{
    static void Run() = delete;
};

template < class H, class... T, std::size_t I >  // NOLINT : keep template parameter naming style
struct ImaginaryFun< variant< H, T... >, I > : ImaginaryFun< variant< T... >, I + 1 >
{
    using ImaginaryFun< variant< T... >, I + 1 >::Run;

    // NOTE: const& and && are used instead of by-value due to lack of guaranteed
    // move elision of C++17. This may have other minor differences, but tests
    // pass.
    static SizeT< I > Run(const H &, SizeT< I >);
    static SizeT< I > Run(H &&, SizeT< I >);
};

// The following metafunctions are used in constructor and assignment
// constraints.
template < class Self, class T >
struct IsNeitherSelfNorInPlace : std::true_type
{
};

template < class Self >
struct IsNeitherSelfNorInPlace< Self, Self > : std::false_type
{
};

template < class Self, class T >
struct IsNeitherSelfNorInPlace< Self, in_place_type_t< T > > : std::false_type
{
};

template < class Self, std::size_t I >  // NOLINT : keep template parameter naming style
struct IsNeitherSelfNorInPlace< Self, in_place_index_t< I > > : std::false_type
{
};

template < class Variant, class T >
struct IndexOfConstructedType< Variant, T, void_t< decltype(ImaginaryFun< Variant >::Run(std::declval< T >(), {})) > >
    : decltype(ImaginaryFun< Variant >::Run(std::declval< T >(), {}))
{
};

template < std::size_t... Is >  // NOLINT : keep template parameter naming style
struct ContainsVariantNPos
    : phsl::negation< std::is_same< std::integer_sequence< bool, 0 <= Is... >,
                                    std::integer_sequence< bool, Is != phsl::variant_npos... > > >
{
};

template < class Op, class... QualifiedVariants >
using RawVisitResult = phsl::result_of_t< Op(VariantAccessResult< 0, QualifiedVariants >...) >;

// NOTE: The spec requires that all return-paths yield the same type and is not
// SFINAE-friendly, so we can deduce the return type by examining the first
// result. If it's not callable, then we get an error, but are compliant and
// fast to compile.
// TODO:(calabrese) Possibly rewrite in a way that yields better
// compile errors at the cost of longer compile-times.
template < class Op, class... QualifiedVariants >
struct VisitResultImpl
{
    using type = phsl::result_of_t< Op(VariantAccessResult< 0, QualifiedVariants >...) >;  // NOLINT
};

// Done in two steps intentionally so that we don't cause substitution to fail.
template < class Op, class... QualifiedVariants >
using VisitResult = typename VisitResultImpl< Op, QualifiedVariants... >::type;

template < class Op, class... QualifiedVariants >
struct PerformVisitation
{
    using ReturnType = VisitResult< Op, QualifiedVariants... >;

    template < std::size_t... Is >  // NOLINT : keep template parameter naming style
    constexpr ReturnType operator()(SizeT< Is >... indices) const
    {
        return Run(typename ContainsVariantNPos< Is... >::type{}, phsl::index_sequence_for< QualifiedVariants... >(),
                   indices...);
    }

    template < std::size_t... TupIs,
               std::size_t... Is >  // NOLINT : keep template parameter naming style
    constexpr ReturnType Run(std::false_type /*has_valueless*/, index_sequence< TupIs... >, SizeT< Is >...) const
    {
        static_assert(
            std::is_same< ReturnType, phsl::result_of_t< Op(VariantAccessResult< Is, QualifiedVariants >...) > >::value,
            "All visitation overloads must have the same return type.");
        return phsl::base_internal::invoke(
            std::forward< Op >(op),
            VariantCoreAccess::Access< Is >(std::forward< QualifiedVariants >(std::get< TupIs >(variant_tup)))...);
    }

    template < std::size_t... TupIs,
               std::size_t... Is >  // NOLINT : keep template parameter naming style
    [[noreturn]] ReturnType Run(std::true_type /*has_valueless*/, index_sequence< TupIs... >, SizeT< Is >...) const
    {
        phsl::variant_internal::ThrowBadVariantAccess();
    }

    // TODO:(calabrese) Avoid using a tuple, which causes lots of
    // instantiations Attempts using lambda variadic captures fail on current GCC.
    std::tuple< QualifiedVariants &&... > variant_tup;  // NOLINT : keep template parameter naming style
    Op &&op;
};

template < class... T >
union Union;

// We want to allow for variant<> to be trivial. For that, we need the default
// constructor to be trivial, which means we can't define it ourselves.
// Instead, we use a non-default constructor that takes NoopConstructorTag
// that doesn't affect the triviality of the types.
struct NoopConstructorTag
{
};

template < std::size_t I >  // NOLINT : keep template parameter naming style
struct EmplaceTag
{
};

template <>
union Union<>
{
    constexpr explicit Union(NoopConstructorTag) noexcept {}
};

    // Suppress bogus warning on MSVC: MSVC complains that Union<T...> has a defined
    // deleted destructor from the `std::is_destructible` check below.
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable : 4624)
    #endif  // _MSC_VER

template < class Head, class... Tail >
union Union< Head, Tail... >
{
    using TailUnion = Union< Tail... >;

    explicit constexpr Union(NoopConstructorTag /*tag*/) noexcept : tail(NoopConstructorTag()) {}

    template < class... P >
    explicit constexpr Union(EmplaceTag< 0 >, P &&...args) : head(std::forward< P >(args)...)
    {
    }

    template < std::size_t I, class... P >  // NOLINT : keep template parameter naming style
    explicit constexpr Union(EmplaceTag< I >, P &&...args) : tail(EmplaceTag< I - 1 >{}, std::forward< P >(args)...)
    {
    }

    Head head;
    TailUnion tail;
};

    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif  // _MSC_VER

// TODO:(calabrese) Just contain a Union in this union (certain configs
// fail).
template < class... T >
union DestructibleUnionImpl;

template <>
union DestructibleUnionImpl<>
{
    constexpr explicit DestructibleUnionImpl(NoopConstructorTag) noexcept {}
};

template < class Head, class... Tail >  // NOLINT : keep template parameter naming style
union DestructibleUnionImpl< Head, Tail... >
{
    using TailUnion = DestructibleUnionImpl< Tail... >;

    explicit constexpr DestructibleUnionImpl(NoopConstructorTag /*tag*/) noexcept : tail(NoopConstructorTag()) {}

    template < class... P >
    explicit constexpr DestructibleUnionImpl(EmplaceTag< 0 >, P &&...args) : head(std::forward< P >(args)...)
    {
    }

    template < std::size_t I, class... P >  // NOLINT : keep template parameter naming style
    explicit constexpr DestructibleUnionImpl(EmplaceTag< I >, P &&...args)
        : tail(EmplaceTag< I - 1 >{}, std::forward< P >(args)...)
    {
    }
    ~DestructibleUnionImpl() {}  // NOLINT : Using = default will cause compilation error

    Head head;
    TailUnion tail;

public:
    /// @brief Copy constructor
    /// @param[in] object DestructibleUnionImpl&
    DestructibleUnionImpl(DestructibleUnionImpl const &object) = default;
    /// @brief Copy the assignment function
    /// @param[in] object DestructibleUnionImpl&
    /// @return DestructibleUnionImpl&
    DestructibleUnionImpl &operator=(DestructibleUnionImpl const &object) = default;
    /// @briefMove constructor
    /// @param[in] object DestructibleUnionImpl&& object
    DestructibleUnionImpl(DestructibleUnionImpl &&object) noexcept = default;
    /// @brief Move operation
    /// @param[in] object DestructibleUnionImpl&& object
    /// @return DestructibleUnionImpl&
    DestructibleUnionImpl &operator=(DestructibleUnionImpl &&object) noexcept = default;
};

// This union type is destructible even if one or more T are not trivially
// destructible. In the case that all T are trivially destructible, then so is
// this resultant type.
template < class... T >
using DestructibleUnion
    = phsl::conditional_t< std::is_destructible< Union< T... > >::value, Union< T... >, DestructibleUnionImpl< T... > >;

// Deepest base, containing the actual union and the discriminator
template < class H, class... T >
class VariantStateBase
{
protected:
    using Variant = variant< H, T... >;

    template < class LazyH          = H,
               class ConstructibleH = phsl::enable_if_t< std::is_default_constructible< LazyH >::value, LazyH > >
    constexpr VariantStateBase() noexcept(std::is_nothrow_default_constructible< ConstructibleH >::value)
        : state_(EmplaceTag< 0 >()), index_(0)
    {
    }

    template < std::size_t I, class... P >  // NOLINT : keep template parameter naming style
    explicit constexpr VariantStateBase(EmplaceTag< I > tag, P &&...args)
        : state_(tag, std::forward< P >(args)...), index_(I)
    {
    }

    explicit constexpr VariantStateBase(NoopConstructorTag) : state_(NoopConstructorTag()), index_(variant_npos) {}

    void destroy()  // NOLINT
    {
        // Does nothing (shadowed in child if non-trivial)
    }

    DestructibleUnion< H, T... > state_;  // NOLINT
    std::size_t index_;                   // NOLINT
};

using phsl::internal::type_identity;

// OverloadSet::Overload() is a unary function which is overloaded to
// take any of the element types of the variant, by reference-to-const.
// The return type of the overload on T is type_identity<T>, so that you
// can statically determine which overload was called.
//
// Overload() is not defined, so it can only be called in unevaluated
// contexts.
template < typename... Ts >
struct OverloadSet;

template < typename T, typename... Ts >
struct OverloadSet< T, Ts... > : OverloadSet< Ts... >
{
    using Base = OverloadSet< Ts... >;
    static type_identity< T > Overload(const T &);
    using Base::Overload;
};

template <>
struct OverloadSet<>
{
    // For any case not handled above.
    static void Overload(...);
};

template < class T >
using LessThanResult = decltype(std::declval< T >() < std::declval< T >());

template < class T >
using GreaterThanResult = decltype(std::declval< T >() > std::declval< T >());

template < class T >
using LessThanOrEqualResult = decltype(std::declval< T >() <= std::declval< T >());

template < class T >
using GreaterThanOrEqualResult = decltype(std::declval< T >() >= std::declval< T >());

template < class T >
using EqualResult = decltype(std::declval< T >() == std::declval< T >());

template < class T >
using NotEqualResult = decltype(std::declval< T >() != std::declval< T >());

using type_traits_internal::is_detected_convertible;

template < class... T >
using RequireAllHaveEqualT
    = phsl::enable_if_t< phsl::conjunction< is_detected_convertible< bool, EqualResult, T >... >::value, bool >;

template < class... T >
using RequireAllHaveNotEqualT
    = phsl::enable_if_t< phsl::conjunction< is_detected_convertible< bool, NotEqualResult, T >... >::value, bool >;

template < class... T >
using RequireAllHaveLessThanT
    = phsl::enable_if_t< phsl::conjunction< is_detected_convertible< bool, LessThanResult, T >... >::value, bool >;

template < class... T >
using RequireAllHaveLessThanOrEqualT
    = phsl::enable_if_t< phsl::conjunction< is_detected_convertible< bool, LessThanOrEqualResult, T >... >::value,
                         bool >;

template < class... T >
using RequireAllHaveGreaterThanOrEqualT
    = phsl::enable_if_t< phsl::conjunction< is_detected_convertible< bool, GreaterThanOrEqualResult, T >... >::value,
                         bool >;

template < class... T >
using RequireAllHaveGreaterThanT
    = phsl::enable_if_t< phsl::conjunction< is_detected_convertible< bool, GreaterThanResult, T >... >::value, bool >;

// Helper template containing implementations details of variant that can't go
// in the private section. For convenience, this takes the variant type as a
// single template parameter.
template < typename T >
struct VariantHelper;

template < typename... Ts >
struct VariantHelper< variant< Ts... > >
{
    // Type metafunction which returns the element type selected if
    // OverloadSet::Overload() is well-formed when called with argument type U.
    template < typename U >
    using BestMatch = decltype(variant_internal::OverloadSet< Ts... >::Overload(std::declval< U >()));

    // Type metafunction which returns true if OverloadSet::Overload() is
    // well-formed when called with argument type U.
    // CanAccept can't be just an alias because there is a MSVC bug on parameter
    // pack expansion involving decltype.
    template < typename U >
    struct CanAccept : std::integral_constant< bool, !std::is_void< BestMatch< U > >::value >
    {
    };

    // Type metafunction which returns true if Other is an instantiation of
    // variant, and variants's converting constructor from Other will be
    // well-formed. We will use this to remove constructors that would be
    // ill-formed from the overload set.
    template < typename Other >
    struct CanConvertFrom;

    template < typename... Us >
    struct CanConvertFrom< variant< Us... > > : public phsl::conjunction< CanAccept< Us >... >
    {
    };
};

// A type with nontrivial copy ctor and trivial move ctor.
struct TrivialMoveOnly  // NOLINT
{
    TrivialMoveOnly()                        = default;
    TrivialMoveOnly(TrivialMoveOnly &&other) = default;
};

// Trait class to detect whether a type is trivially move constructible.
// A union's defaulted copy/move constructor is deleted if any variant member's
// copy/move constructor is nontrivial.
template < typename T >
struct IsTriviallyMoveConstructible : std::is_move_constructible< Union< T, TrivialMoveOnly > >
{
};

// To guarantee triviality of all special-member functions that can be trivial,
// we use a chain of conditional bases for each one.
// The order of inheritance of bases from child to base are logically:
//
// variant
// VariantCopyAssignBase
// VariantMoveAssignBase
// VariantCopyBase
// VariantMoveBase
// VariantStateBaseDestructor
// VariantStateBase
//
// Note that there is a separate branch at each base that is dependent on
// whether or not that corresponding special-member-function can be trivial in
// the resultant variant type.

template < class... T >
class VariantStateBaseDestructorNontrivial;

template < class... T >
class VariantMoveBaseNontrivial;

template < class... T >
class VariantCopyBaseNontrivial;

template < class... T >
class VariantMoveAssignBaseNontrivial;

template < class... T >
class VariantCopyAssignBaseNontrivial;

// Base that is dependent on whether or not the destructor can be trivial.
template < class... T >
using VariantStateBaseDestructor = phsl::conditional_t< std::is_destructible< Union< T... > >::value,
                                                        VariantStateBase< T... >,
                                                        VariantStateBaseDestructorNontrivial< T... > >;

// Base that is dependent on whether or not the move-constructor can be
// implicitly generated by the compiler (trivial or deleted).
// Previously we were using `std::is_move_constructible<Union<T...>>` to check
// whether all Ts have trivial move constructor, but it ran into a GCC bug:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=84866
// So we have to use a different approach (i.e. `HasTrivialMoveConstructor`) to
// work around the bug.
template < class... T >
using VariantMoveBase
    = phsl::conditional_t< phsl::disjunction< phsl::negation< phsl::conjunction< std::is_move_constructible< T >... > >,
                                              phsl::conjunction< IsTriviallyMoveConstructible< T >... > >::value,
                           VariantStateBaseDestructor< T... >,
                           VariantMoveBaseNontrivial< T... > >;

// Base that is dependent on whether or not the copy-constructor can be trivial.
template < class... T >
using VariantCopyBase
    = phsl::conditional_t< phsl::disjunction< phsl::negation< phsl::conjunction< std::is_copy_constructible< T >... > >,
                                              std::is_copy_constructible< Union< T... > > >::value,
                           VariantMoveBase< T... >,
                           VariantCopyBaseNontrivial< T... > >;

// Base that is dependent on whether or not the move-assign can be trivial.
template < class... T >
using VariantMoveAssignBase = phsl::conditional_t<
    phsl::disjunction< phsl::conjunction< phsl::is_move_assignable< Union< T... > >,
                                          std::is_move_constructible< Union< T... > >,
                                          std::is_destructible< Union< T... > > >,
                       phsl::negation< phsl::conjunction< std::is_move_constructible< T >...,
                                                          // Note: We're not qualifying this with
                                                          // phsl:: because it doesn't compile
                                                          // under MSVC.
                                                          is_move_assignable< T >... > > >::value,
    VariantCopyBase< T... >,
    VariantMoveAssignBaseNontrivial< T... > >;

// Base that is dependent on whether or not the copy-assign can be trivial.
template < class... T >
using VariantCopyAssignBase = phsl::conditional_t<
    phsl::disjunction< phsl::conjunction< phsl::is_copy_assignable< Union< T... > >,
                                          std::is_copy_constructible< Union< T... > >,
                                          std::is_destructible< Union< T... > > >,
                       phsl::negation< phsl::conjunction< std::is_copy_constructible< T >...,
                                                          // Note: We're not qualifying this with
                                                          // phsl:: because it doesn't compile
                                                          // under MSVC.
                                                          is_copy_assignable< T >... > > >::value,
    VariantMoveAssignBase< T... >,
    VariantCopyAssignBaseNontrivial< T... > >;

template < class... T >
using VariantBase = VariantCopyAssignBase< T... >;

template < class... T >
class VariantStateBaseDestructorNontrivial : protected VariantStateBase< T... >
{
private:
    using Base = VariantStateBase< T... >;

protected:
    using Base::Base;

    VariantStateBaseDestructorNontrivial()                                                 = default;
    VariantStateBaseDestructorNontrivial(VariantStateBaseDestructorNontrivial &&) noexcept = default;
    VariantStateBaseDestructorNontrivial(const VariantStateBaseDestructorNontrivial &)     = default;
    VariantStateBaseDestructorNontrivial &operator=(VariantStateBaseDestructorNontrivial &&) noexcept = default;
    VariantStateBaseDestructorNontrivial &operator=(const VariantStateBaseDestructorNontrivial &) = default;

    struct Destroyer
    {
        template < std::size_t I >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< I > i) const
        {
            using Alternative = typename phsl::variant_alternative< I, variant< T... > >::type;
            variant_internal::AccessUnion(self->state_, i).~Alternative();
        }

        void operator()(SizeT< phsl::variant_npos > /*i*/) const
        {
            // This space intentionally left blank
        }

        VariantStateBaseDestructorNontrivial *self;
    };

    void destroy()  // NOLINT
    {
        VisitIndices< sizeof...(T) >::Run(Destroyer{this}, index_);
    }

    ~VariantStateBaseDestructorNontrivial() { destroy(); }

protected:
    using Base::index_;
    using Base::state_;
};

template < class... T >
class VariantMoveBaseNontrivial : protected VariantStateBaseDestructor< T... >
{
private:
    using Base = VariantStateBaseDestructor< T... >;

protected:
    using Base::Base;

    struct Construct
    {
        template < std::size_t I >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< I > i) const
        {
            using Alternative = typename phsl::variant_alternative< I, variant< T... > >::type;
            ::new (static_cast< void * >(&self->state_))
                Alternative(variant_internal::AccessUnion(std::move(other->state_), i));
        }

        void operator()(SizeT< phsl::variant_npos > /*i*/) const {}

        VariantMoveBaseNontrivial *self;
        VariantMoveBaseNontrivial *other;
    };

    VariantMoveBaseNontrivial() = default;
    VariantMoveBaseNontrivial(VariantMoveBaseNontrivial &&other) noexcept(
        phsl::conjunction< std::is_nothrow_move_constructible< T >... >::value)
        : Base(NoopConstructorTag())
    {
        VisitIndices< sizeof...(T) >::Run(Construct{this, &other}, other.index_);
        index_ = other.index_;
    }

    VariantMoveBaseNontrivial(VariantMoveBaseNontrivial const &) = default;

    VariantMoveBaseNontrivial &operator=(VariantMoveBaseNontrivial &&) noexcept = default;
    VariantMoveBaseNontrivial &operator=(VariantMoveBaseNontrivial const &) = default;

public:
    ~VariantMoveBaseNontrivial() = default;

protected:
    using Base::index_;
    using Base::state_;
};

template < class... T >
class VariantCopyBaseNontrivial : protected VariantMoveBase< T... >
{
private:
    using Base = VariantMoveBase< T... >;

protected:
    using Base::Base;

    VariantCopyBaseNontrivial()                                      = default;
    VariantCopyBaseNontrivial(VariantCopyBaseNontrivial &&) noexcept = default;

    struct Construct
    {
        template < std::size_t I >  // NOLINT : keep template parameter naming style
        void operator()(SizeT< I > i) const
        {
            using Alternative = typename phsl::variant_alternative< I, variant< T... > >::type;
            ::new (static_cast< void * >(&self->state_)) Alternative(variant_internal::AccessUnion(other->state_, i));
        }

        void operator()(SizeT< phsl::variant_npos > /*i*/) const {}

        VariantCopyBaseNontrivial *self;
        const VariantCopyBaseNontrivial *other;
    };

    VariantCopyBaseNontrivial(VariantCopyBaseNontrivial const &other) : Base(NoopConstructorTag())
    {
        VisitIndices< sizeof...(T) >::Run(Construct{this, &other}, other.index_);
        index_ = other.index_;
    }

    VariantCopyBaseNontrivial &operator=(VariantCopyBaseNontrivial &&) noexcept = default;
    VariantCopyBaseNontrivial &operator=(VariantCopyBaseNontrivial const &) = default;

public:
    ~VariantCopyBaseNontrivial() = default;

protected:
    using Base::index_;
    using Base::state_;
};

template < class... T >
class VariantMoveAssignBaseNontrivial : protected VariantCopyBase< T... >
{
    friend struct VariantCoreAccess;

private:
    using Base = VariantCopyBase< T... >;

protected:
    using Base::Base;

    VariantMoveAssignBaseNontrivial()                                            = default;
    VariantMoveAssignBaseNontrivial(VariantMoveAssignBaseNontrivial &&) noexcept = default;
    VariantMoveAssignBaseNontrivial(const VariantMoveAssignBaseNontrivial &)     = default;
    VariantMoveAssignBaseNontrivial &operator=(VariantMoveAssignBaseNontrivial const &) = default;

    VariantMoveAssignBaseNontrivial &operator=(VariantMoveAssignBaseNontrivial &&other) noexcept(
        phsl::conjunction< std::is_nothrow_move_constructible< T >..., std::is_nothrow_move_assignable< T >... >::value)
    {
        VisitIndices< sizeof...(T) >::Run(VariantCoreAccess::MakeMoveAssignVisitor(this, &other), other.index_);
        return *this;
    }

public:
    ~VariantMoveAssignBaseNontrivial() = default;

protected:
    using Base::index_;
    using Base::state_;
};

template < class... T >
class VariantCopyAssignBaseNontrivial : protected VariantMoveAssignBase< T... >
{
    friend struct VariantCoreAccess;

private:
    using Base = VariantMoveAssignBase< T... >;

protected:
    using Base::Base;

    VariantCopyAssignBaseNontrivial()                                            = default;
    VariantCopyAssignBaseNontrivial(VariantCopyAssignBaseNontrivial &&) noexcept = default;
    VariantCopyAssignBaseNontrivial(const VariantCopyAssignBaseNontrivial &)     = default;
    VariantCopyAssignBaseNontrivial &operator=(VariantCopyAssignBaseNontrivial &&) noexcept = default;
    VariantCopyAssignBaseNontrivial &operator=(const VariantCopyAssignBaseNontrivial &other)
    {
        if (&other == this) {
            return *this;
        }
        VisitIndices< sizeof...(T) >::Run(VariantCoreAccess::MakeCopyAssignVisitor(this, other), other.index_);
        return *this;
    }

public:
    ~VariantCopyAssignBaseNontrivial() = default;

protected:
    using Base::index_;
    using Base::state_;
};

////////////////////////////////////////
// Visitors for Comparison Operations //
////////////////////////////////////////

template < class... Types >
struct EqualsOp
{
    const variant< Types... > *v;
    const variant< Types... > *w;

    constexpr bool operator()(SizeT< phsl::variant_npos > /*v_i*/) const { return true; }

    template < std::size_t I >  // NOLINT : keep template parameter naming style
    constexpr bool operator()(SizeT< I > /*v_i*/) const
    {
        return VariantCoreAccess::Access< I >(*v) == VariantCoreAccess::Access< I >(*w);
    }
};

template < class... Types >
struct NotEqualsOp
{
    const variant< Types... > *v;
    const variant< Types... > *w;

    constexpr bool operator()(SizeT< phsl::variant_npos > /*v_i*/) const { return false; }

    template < std::size_t I >  // NOLINT : keep template parameter naming style
    constexpr bool operator()(SizeT< I > /*v_i*/) const
    {
        return VariantCoreAccess::Access< I >(*v) != VariantCoreAccess::Access< I >(*w);
    }
};

template < class... Types >
struct LessThanOp
{
    const variant< Types... > *v;
    const variant< Types... > *w;

    constexpr bool operator()(SizeT< phsl::variant_npos > /*v_i*/) const { return false; }

    template < std::size_t I >  // NOLINT : keep template parameter naming style
    constexpr bool operator()(SizeT< I > /*v_i*/) const
    {
        return VariantCoreAccess::Access< I >(*v) < VariantCoreAccess::Access< I >(*w);
    }
};

template < class... Types >
struct GreaterThanOp
{
    const variant< Types... > *v;
    const variant< Types... > *w;

    constexpr bool operator()(SizeT< phsl::variant_npos > /*v_i*/) const { return false; }

    template < std::size_t I >  // NOLINT : keep template parameter naming style
    constexpr bool operator()(SizeT< I > /*v_i*/) const
    {
        return VariantCoreAccess::Access< I >(*v) > VariantCoreAccess::Access< I >(*w);
    }
};

template < class... Types >
struct LessThanOrEqualsOp
{
    const variant< Types... > *v;
    const variant< Types... > *w;

    constexpr bool operator()(SizeT< phsl::variant_npos > /*v_i*/) const { return true; }

    template < std::size_t I >  // NOLINT : keep template parameter naming style
    constexpr bool operator()(SizeT< I > /*v_i*/) const
    {
        return VariantCoreAccess::Access< I >(*v) <= VariantCoreAccess::Access< I >(*w);
    }
};

template < class... Types >
struct GreaterThanOrEqualsOp
{
    const variant< Types... > *v;
    const variant< Types... > *w;

    constexpr bool operator()(SizeT< phsl::variant_npos > /*v_i*/) const { return true; }

    template < std::size_t I >  // NOLINT : keep template parameter naming style
    constexpr bool operator()(SizeT< I > /*v_i*/) const
    {
        return VariantCoreAccess::Access< I >(*v) >= VariantCoreAccess::Access< I >(*w);
    }
};

// Precondition: v.index() == w.index();
template < class... Types >
struct SwapSameIndex
{
    variant< Types... > *v;
    variant< Types... > *w;
    template < std::size_t I >  // NOLINT : keep template parameter naming style
    void operator()(SizeT< I >) const
    {
        type_traits_internal::Swap(VariantCoreAccess::Access< I >(*v), VariantCoreAccess::Access< I >(*w));
    }

    void operator()(SizeT< variant_npos >) const {}
};

// TODO:(calabrese) do this from a different namespace for proper adl
// usage
template < class... Types >
struct Swap
{
    variant< Types... > *v;
    variant< Types... > *w;

    void generic_swap() const  // NOLINT
    {
        variant< Types... > tmp(std::move(*w));
        VariantCoreAccess::Destroy(*w);
        VariantCoreAccess::InitFrom(*w, std::move(*v));
        VariantCoreAccess::Destroy(*v);
        VariantCoreAccess::InitFrom(*v, std::move(tmp));
    }

    void operator()(SizeT< phsl::variant_npos > /*w_i*/) const
    {
        if (!v->valueless_by_exception()) {
            generic_swap();
        }
    }

    template < std::size_t Wi >  // NOLINT : keep template parameter naming style
    void operator()(SizeT< Wi > /*w_i*/)
    {
        if (v->index() == Wi) {
            VisitIndices< sizeof...(Types) >::Run(SwapSameIndex< Types... >{v, w}, Wi);
        } else {
            generic_swap();
        }
    }
};

template < typename Variant, typename = void, typename... Ts >
struct VariantHashBase  // NOLINT
{
    VariantHashBase()                        = delete;
    VariantHashBase(const VariantHashBase &) = delete;
    VariantHashBase(VariantHashBase &&)      = delete;
    VariantHashBase &operator=(const VariantHashBase &) = delete;
    VariantHashBase &operator=(VariantHashBase &&) = delete;
};

struct VariantHashVisitor
{
    template < typename T >
    size_t operator()(const T &t)
    {
        return std::hash< T >{}(t);
    }
};

template < typename Variant, typename... Ts >
struct VariantHashBase< Variant,
                        phsl::enable_if_t< phsl::conjunction< type_traits_internal::IsHashable< Ts >... >::value >,
                        Ts... >
{
    using argument_type = Variant;  // NOLINT
    using result_type   = size_t;   // NOLINT
    size_t operator()(const Variant &var) const
    {
        type_traits_internal::AssertHashEnabled< Ts... >();
        if (var.valueless_by_exception()) {
            constexpr std::int64_t kNUM_239799884{239799884};
            return kNUM_239799884;
        }
        size_t result = VisitIndices< variant_size< Variant >::value >::Run(
            PerformVisitation< VariantHashVisitor, const Variant & >{std::forward_as_tuple(var), VariantHashVisitor{}},
            var.index());
        // Combine the index and the hash result in order to distinguish
        // std::variant<int, int> holding the same value as different alternative.
        return result ^ var.index();
    }
};

}  // namespace variant_internal
PBSL_NAMESPACE_END
}  // namespace phsl

#endif  // !defined(PBSL_USES_STD_VARIANT)
#endif  // PBSL_TYPES_INTERNAL_VARIANT_H_
