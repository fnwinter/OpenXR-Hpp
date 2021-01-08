//## Copyright (c) 2017-2020 The Khronos Group Inc.
//## Copyright (c) 2019-2020 Collabora, Ltd.
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
 * @brief Contains a dynamically-loading dispatcher class: loads entry points with xrGetInstanceProcAddr, optionally lazily.
 * @ingroup dispatch
 */

//# from 'macros.hpp' import forwardCommandArgs, make_pfn_type, make_pfn_getter_name

#include "openxr_dispatch_traits.hpp"

#include <openxr/openxr.h>

#ifdef OPENXR_HPP_DOXYGEN
#include <openxr/openxr_platform.h>
#endif

//# include('define_assert.hpp') without context
//# include('define_inline_constexpr.hpp') without context
//# include('define_namespace.hpp') without context

namespace OPENXR_HPP_NAMESPACE {

/*% macro make_pfn_name(cur_cmd) -%*/ /*{cur_cmd.name | replace("xr", "pfn")}*/ /*%- endmacro %*/

/*!
 * @brief Dispatch class for OpenXR that looks up all functions using a provided or statically-available xrGetInstanceProcAddr
 * function and the optional Instance.
 *
 * This is safer to use, especially in large/multi-module applications, than DispatchLoaderStatic, and is thus recommended.
 *
 * By default, it is lazy-populating: only populating a function pointer when it is attempted to be called (if this object is not
 * const). You can early-populate it using the createFullyPopulated() factory method, providing an Instance and optionally a
 * xrGetInstanceProcAddr function pointer.
 *
 * This class stores all function pointers as type-erased PFN_xrVoidFunction, casting at time of call. This allows the same memory
 * representation to be used across translation units that may not share the same platform defines. Only the member function
 * trampolines containing the casts are conditional on platform defines.
 *
 * @ingroup dispatch
 */
class DispatchLoaderDynamic {
   public:
    /*!
     * @name Constuctor/Factory functions
     * @{
     */
    /*!
     * @brief Create a lazy-populating dispatch table.
     */
    explicit DispatchLoaderDynamic(XrInstance instance, PFN_xrGetInstanceProcAddr getInstanceProcAddr)
        : m_instance(instance), pfnGetInstanceProcAddr(reinterpret_cast<PFN_xrVoidFunction>(getInstanceProcAddr)) {
    }

#ifndef XR_NO_PROTOTYPES
    /*!
     * @brief Create a lazy-populating dispatch table using the static xrGetInstanceProcAddr.
     */
    explicit DispatchLoaderDynamic(XrInstance instance = XR_NULL_HANDLE)
        : DispatchLoaderDynamic(instance, &::xrGetInstanceProcAddr) {}
#endif // !XR_NO_PROTOTYPES

    /*!
     * @brief Create a fully-populated dispatch table given a non-null XrInstance and a getInstanceProcAddr.
     */
    static DispatchLoaderDynamic createFullyPopulated(XrInstance instance, PFN_xrGetInstanceProcAddr getInstanceProcAddr)
    {
        OPENXR_HPP_ASSERT(instance != XR_NULL_HANDLE);
        DispatchLoaderDynamic dispatch{instance, getInstanceProcAddr};
        dispatch.populateFully();
        return dispatch;
    }
    //! @}

    /*!
     * @brief Fully populate a dispatch table given a non-null XrInstance and a getInstanceProcAddr.
     */
    void populateFully()
    {
        OPENXR_HPP_ASSERT(m_instance != XR_NULL_HANDLE);
        OPENXR_HPP_ASSERT(pfnGetInstanceProcAddr != nullptr);
        //# for cur_cmd in sorted_cmds
        populate_(/*{cur_cmd.name | quote_string}*/, /*{make_pfn_name(cur_cmd)}*/);
        //# endfor
    }
    /*!
     * @brief Fully populate a dispatch table given a non-null XrInstance and a getInstanceProcAddr.
     */
    void populateFully(XrInstance instance, PFN_xrGetInstanceProcAddr getInstanceProcAddr)
    {
        m_instance = instance;
        pfnGetInstanceProcAddr = reinterpret_cast<PFN_xrVoidFunction>(getInstanceProcAddr);
        populateFully();
    }

    /*!
     * @name Entry points
     * @brief These populate the function pointer (if required and non-const), then cast it and call it.
     *
     * We store everything as PFN_xrVoidFunction to allow us to have the same representation all over,
     * despite containing function pointers that might not be accessible without some platform defines.
     * Thus, only our accessor methods differ between different includes of this header, not our data members.
     *
     * @{
     */

    //# for cur_cmd in sorted_cmds
    /*{ protect_begin(cur_cmd) }*/
    //! @brief Call /*{cur_cmd.name}*/, populating function pointer if required.
    OPENXR_HPP_INLINE /*{cur_cmd.cdecl | collapse_whitespace | replace(";", "")}*/ {
        //## Populate
        XrResult result = populate_(/*{cur_cmd.name | quote_string}*/, /*{make_pfn_name(cur_cmd)}*/);
        if (XR_FAILED(result)) {
            return result;
        }
        //## Cast and call
        return (reinterpret_cast</*{ make_pfn_type(cur_cmd) }*/>(/*{make_pfn_name(cur_cmd)}*/))(
            /*{ forwardCommandArgs(cur_cmd) }*/);
    }

    //! @brief Call /*{cur_cmd.name}*/ (const overload - does not populate function pointer)
    OPENXR_HPP_INLINE /*{cur_cmd.cdecl | collapse_whitespace | replace(";", "")}*/ const {
        //## Cast and call
        return (reinterpret_cast</*{ make_pfn_type(cur_cmd) }*/>(/*{make_pfn_name(cur_cmd)}*/))(
            /*{ forwardCommandArgs(cur_cmd) }*/);
    }

    //#     filter block_doxygen_comment
    //! @brief Return the function pointer for /*{cur_cmd.name}*/, populating function pointer if required.
    //#     endfilter
    OPENXR_HPP_INLINE /*{ make_pfn_type(cur_cmd) }*/ /*{ make_pfn_getter_name(cur_cmd) }*/() {
        //## Populate
        XrResult result = populate_(/*{cur_cmd.name | quote_string}*/, /*{make_pfn_name(cur_cmd)}*/);
        if (XR_FAILED(result)) {
            return nullptr;
        }
        //## Cast and return
        return (reinterpret_cast</*{ make_pfn_type(cur_cmd) }*/>(/*{make_pfn_name(cur_cmd)}*/));
    }

    //#     filter block_doxygen_comment
    //! @brief Return the function pointer for /*{cur_cmd.name}*/ (const overload - does not populate function pointer)
    //#     endfilter
    OPENXR_HPP_INLINE /*{ make_pfn_type(cur_cmd) }*/ /*{ make_pfn_getter_name(cur_cmd) }*/()  const {
        //## Cast and return
        return (reinterpret_cast</*{ make_pfn_type(cur_cmd) }*/>(/*{make_pfn_name(cur_cmd)}*/));
    }
    /*{ protect_end(cur_cmd) }*/

    //# endfor

    //! @}
   private:
    //! @brief Internal utility function to populate a function pointer if it is nullptr.
    OPENXR_HPP_INLINE XrResult populate_(const char *function_name, PFN_xrVoidFunction &pfn) {
        if (pfn == nullptr) {
            return reinterpret_cast<PFN_xrGetInstanceProcAddr>(pfnGetInstanceProcAddr)(m_instance, function_name, &pfn);
        }
        return XR_SUCCESS;
    }
    XrInstance m_instance;
    //# for cur_cmd in sorted_cmds
    PFN_xrVoidFunction /*{ make_pfn_name(cur_cmd) }*/ {};
    //# endfor
};

}  // namespace OPENXR_HPP_NAMESPACE

#ifndef OPENXR_HPP_DOXYGEN
OPENXR_HPP_CLASS_IS_DISPATCH(OPENXR_HPP_NAMESPACE::DispatchLoaderDynamic)
#endif  // !OPENXR_HPP_DOXYGEN

//# include('file_footer.hpp')
