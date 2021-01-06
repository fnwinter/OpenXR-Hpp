//## Copyright (c) 2017-2019 The Khronos Group Inc.
//## Copyright (c) 2019 Collabora, Ltd.
//##
//## Licensed under the Apache License, Version 2.0 (the "License");
//## you may not use this file except in compliance with the License.
//## You may obtain a copy of the License at
//##
//##     http://www.apache.org/licenses/LICENSE-2.0
//##
//## Unless required by applicable law or agreed to in writing, software
//## distributed under the License is distributed on an "AS IS" BASIS,
//## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//## See the License for the specific language governing permissions and
//## limitations under the License.
//##
//## ---- Exceptions to the Apache 2.0 License: ----
//##
//## As an exception, if you use this Software to generate code and portions of
//## this Software are embedded into the generated code as a result, you may
//## redistribute such product without providing attribution as would otherwise
//## be required by Sections 4(a), 4(b) and 4(d) of the License.
//##
//## In addition, if you combine or link code generated by this Software with
//## software that is licensed under the GPLv2 or the LGPL v2.0 or 2.1
//## ("`Combined Software`") and if a court of competent jurisdiction determines
//## that the patent provision (Section 3), the indemnity provision (Section 9)
//## or other Section of the License conflicts with the conditions of the
//## applicable GPL or LGPL license, you may retroactively and prospectively
//## choose to deem waived or otherwise exclude such Section(s) of the License,
//## but only in their entirety and only with respect to the Combined Software.

//# include('file_header.hpp')

/**
 * @file
 * @brief Defines wrappers for OpenXR handle types, with function calls transformed into methods.
 *
 * Also includes free functions for those few that do not take a handle as their first argument.
 *
 * Does not include the implementations of the functions: for that, see @ref openxr_method_impls.hpp
 */

//# from 'method_decl_macros.hpp' import method_prototypes with context

#include "openxr_atoms.hpp"
#include "openxr_enums.hpp"
#include "openxr_exceptions.hpp"
#include "openxr_flags.hpp"
#include "openxr_handles_forward.hpp"
#include "openxr_structs_forward.hpp"
#include "openxr_time.hpp"

#include <openxr/openxr.h>

#ifdef OPENXR_HPP_DOXYGEN
#include <openxr/openxr_platform.h>
#endif

#ifndef OPENXR_HPP_DISABLE_ENHANCED_MODE
#include <vector>
#endif  // !OPENXR_HPP_DISABLE_ENHANCED_MODE

//# include('define_inline_constexpr.hpp') without context
//# include('define_conversion.hpp') without context
//# include('define_namespace.hpp') without context

#if !defined(OPENXR_HPP_TYPESAFE_EXPLICIT)
#if defined(OPENXR_HPP_TYPESAFE_CONVERSION)
#define OPENXR_HPP_TYPESAFE_EXPLICIT
#else
#define OPENXR_HPP_TYPESAFE_EXPLICIT explicit
#endif
#endif  // !OPENXR_HPP_TYPESAFE_EXPLICIT

#ifdef OPENXR_HPP_DOXYGEN
#define OPENXR_HPP_NO_DEFAULT_DISPATCH
#define OPENXR_HPP_NO_SMART_HANDLE
#undef OPENXR_HPP_NO_SMART_HANDLE
#define OPENXR_HPP_DEFAULT_CORE_DISPATCHER DispatchLoaderStatic()
#undef OPENXR_HPP_DEFAULT_CORE_DISPATCHER
#define OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER
#undef OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER
#endif

/*!
 * @def OPENXR_HPP_TYPESAFE_CONVERSION
 * @brief Whether to force conversion/implicit constructors.
 *
 * Enabled by default on 64-bit platforms.
 * 32-bit OpenXR is not typesafe for handles, so this is disabled on such platforms by default.
 * To enable this feature on 32-bit platforms please define `OPENXR_HPP_TYPESAFE_CONVERSION`
 *
 * @ingroup config
 */
/*!
 * @defgroup config_dispatch Default dispatch configuration
 * @ingroup config
 */
/*!
 * @def OPENXR_HPP_NO_DEFAULT_DISPATCH
 * @brief Define to disable default dispatch arguments.
 * @see OPENXR_HPP_DEFAULT_CORE_DISPATCHER, OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE
 * @ingroup config_dispatch
 */
/*!
 * @def OPENXR_HPP_DEFAULT_CORE_DISPATCHER
 * @brief Define to the expression you'd like to use as the default dispatcher for core API functions.
 *
 * Defaults to `DispatchLoaderStatic()` unless `OPENXR_HPP_NO_DEFAULT_DISPATCH` is defined.
 *
 * If both this and `OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE` are defined (by you, or by default if `OPENXR_HPP_NO_DEFAULT_DISPATCH` is not defined),
 * `OPENXR_HPP_DEFAULT_CORE_DISPATCH_ARG` will be defined to `= OPENXR_HPP_DEFAULT_CORE_DISPATCHER`.
 *
 * @see DispatchLoaderStatic, OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE, OPENXR_HPP_NO_DEFAULT_DISPATCH
 * @ingroup config_dispatch
 */
/*!
 * @def OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE
 * @brief Define to the type of the expression you'd like to use as the default dispatcher for core API functions.
 *
 * This will be used as the default type parameter in functions where `OPENXR_HPP_DEFAULT_CORE_DISPATCHER` will be the default argument.
 *
 * Defaults to `DispatchLoaderStatic` unless `OPENXR_HPP_NO_DEFAULT_DISPATCH` is defined.
 *
 * If both this and `OPENXR_HPP_DEFAULT_CORE_DISPATCHER` are defined (by you, or by default if `OPENXR_HPP_NO_DEFAULT_DISPATCH` is not defined),
 * `OPENXR_HPP_DEFAULT_CORE_DISPATCH_TYPE_ARG` will be defined to `= OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE`.
 *
 * @see DispatchLoaderStatic, OPENXR_HPP_DEFAULT_CORE_DISPATCHER, OPENXR_HPP_NO_DEFAULT_DISPATCH
 * @ingroup config_dispatch
 */
/*!
 * @def OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER
 * @brief Define to the expression you'd like to use as the default dispatcher for extension API functions.
 *
 * This has no default value: by default, you need to provide a dispatcher explicitly for extension functions.
 * If you define this and `OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER_TYPE`, however,
 * `OPENXR_HPP_DEFAULT_EXT_DISPATCH_ARG` will be set to `=` and your definition.
 *
 * A globally-accessible instance of xr::DispatchLoaderDynamic would be suitable.
 *
 * @ingroup config_dispatch
 */

/*!
 * @def OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER_TYPE
 * @brief Define to the type expression you'd like to use as the type of the default dispatcher for extension API functions.
 *
 * This has no default value: by default, you need to provide a dispatcher explicitly for extension functions.
 * If you define this and `OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER`, however,
 * `OPENXR_HPP_DEFAULT_EXT_DISPATCH_TYPE_ARG` will be set to `=` and your definition.
 *
 * xr::DispatchLoaderDynamic would be suitable.
 *
 * @ingroup config_dispatch
 */
/*!
 * @def OPENXR_HPP_DISABLE_ENHANCED_MODE
 * @brief Define in order to disable the more complete C++ projections of OpenXR methods, leaving only the most C-like prototypes behind.
 *
 * This will disable returning of output parameters (directly and through ReturnType pairs),
 * wrapping of two-call-idiom methods, functions creating `Unique` handles with ownership, etc.
 * It slightly reduces the number of files included.
 *
 * @ingroup config
 */


/*!
 * @def OPENXR_HPP_NO_SMART_HANDLE
 * @brief Define in order to disable the UniqueHandle-creating C++ method projections.
 *
 * Enhanced mode creation calls by default include a projection that returns a smart handle representing unique ownership.
 * Defining this macro will hide those methods.
 *
 * @see OPENXR_HPP_DISABLE_ENHANCED_MODE
 *
 * @ingroup config
 */

#ifndef OPENXR_HPP_NO_DEFAULT_DISPATCH

#if !defined(XR_NO_PROTOTYPES) && !defined(OPENXR_HPP_DEFAULT_CORE_DISPATCHER) && !defined(OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE)
#define OPENXR_HPP_DEFAULT_CORE_DISPATCHER DispatchLoaderStatic()
#define OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE DispatchLoaderStatic
#include "openxr_static_dispatch.hpp"
#endif  // !defined(XR_NO_PROTOTYPES) && !defined(OPENXR_HPP_DEFAULT_CORE_DISPATCHER)

#endif  // !OPENXR_HPP_NO_DEFAULT_DISPATCH

#if defined(OPENXR_HPP_DEFAULT_CORE_DISPATCHER) && defined(OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE)
#define OPENXR_HPP_DEFAULT_CORE_DISPATCH_ARG = OPENXR_HPP_DEFAULT_CORE_DISPATCHER
#define OPENXR_HPP_DEFAULT_CORE_DISPATCH_TYPE_ARG = OPENXR_HPP_DEFAULT_CORE_DISPATCHER_TYPE
#endif

#if defined(OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER) && defined(OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER_TYPE)
#define OPENXR_HPP_DEFAULT_EXT_DISPATCH_ARG = OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER
#define OPENXR_HPP_DEFAULT_EXT_DISPATCH_TYPE_ARG = OPENXR_HPP_DEFAULT_EXTENSION_DISPATCHER_TYPE
#endif

#ifndef OPENXR_HPP_DEFAULT_CORE_DISPATCH_ARG
#define OPENXR_HPP_DEFAULT_CORE_DISPATCH_ARG
#endif
#ifndef OPENXR_HPP_DEFAULT_CORE_DISPATCH_TYPE_ARG
#define OPENXR_HPP_DEFAULT_CORE_DISPATCH_TYPE_ARG
#endif
#ifndef OPENXR_HPP_DEFAULT_EXT_DISPATCH_ARG
#define OPENXR_HPP_DEFAULT_EXT_DISPATCH_ARG
#endif
#ifndef OPENXR_HPP_DEFAULT_EXT_DISPATCH_TYPE_ARG
#define OPENXR_HPP_DEFAULT_EXT_DISPATCH_TYPE_ARG
#endif


//# include('enhanced_mode_results.hpp') without context

namespace OPENXR_HPP_NAMESPACE {

#ifndef OPENXR_HPP_DISABLE_ENHANCED_MODE
// The generalization of std::string with user-specifiable allocator types.
template <typename Allocator = std::allocator<char>>
using string_with_allocator = std::basic_string<char, std::char_traits<char>, Allocator>;
#endif  // !OPENXR_HPP_DISABLE_ENHANCED_MODE

//# for handle in gen.api_handles
//#     set shortname = project_type_name(handle.name)

/*{ protect_begin(handle) }*/
//#     include "handle.hpp"
/*{ protect_end(handle) }*/

//# endfor

//## Generate free-function prototypes
/*!
 * @defgroup api_free_functions OpenXR API free functions
 *
 * Equivalent to the method wrappers in the handle classes,
 * but for the few functions that don't take (or don't require)
 * a handle as their first argument.
 * @{
 */
// Declarations - implementations are out of line.

//# for cur_cmd in sorted_cmds if not cur_cmd.handle
/*{ method_prototypes(cur_cmd, none) }*/
//# endfor

//! @}

}  // namespace OPENXR_HPP_NAMESPACE

//# include('file_footer.hpp')
