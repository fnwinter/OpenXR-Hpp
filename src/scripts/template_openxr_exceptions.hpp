
namespace OPENXR_HPP_NAMESPACE {

OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool failed(Result v) { return static_cast<int>(v) < 0; }
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool succeeded(Result v) { return static_cast<int>(v) >= 0; }
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool unqualifiedSuccess(Result v) { return v == Result::Success; }

/*% macro resultOperators(op) %*/
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator/*{- op -}*/(Result lhs, int rhs) { return get(lhs) /*{- op -}*/ rhs; }
OPENXR_HPP_CONSTEXPR OPENXR_HPP_INLINE bool operator/*{- op -}*/(int lhs, Result rhs) { return lhs /*{- op -}*/ get(rhs); }
/*% endmacro %*/

/*{resultOperators('<')}*/
/*{resultOperators('>')}*/
/*{resultOperators('<=')}*/
/*{resultOperators('>=')}*/
/*{resultOperators('==')}*/
/*{resultOperators('!=')}*/

}  // namespace OPENXR_HPP_NAMESPACE

#if !defined(OPENXR_HPP_NO_EXCEPTIONS)

namespace OPENXR_HPP_NAMESPACE {

namespace impl {
#if defined(_MSC_VER) && (_MSC_VER == 1800)
#define noexcept _NOEXCEPT
#endif

class ErrorCategoryImpl : public std::error_category {
   public:
    virtual const char* name() const noexcept override { return OPENXR_HPP_NAMESPACE_STRING "::Result"; }
    virtual std::string message(int ev) const override { return to_string(static_cast<Result>(ev)); }
};

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef noexcept
#endif
}  // namespace impl

OPENXR_HPP_INLINE const std::error_category& errorCategory() {
    static impl::ErrorCategoryImpl instance;
    return instance;
}

OPENXR_HPP_INLINE std::error_code make_error_code(Result e) { return std::error_code(static_cast<int>(e), errorCategory()); }

OPENXR_HPP_INLINE std::error_condition make_error_condition(Result e) {
    return std::error_condition(static_cast<int>(e), errorCategory());
}

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#define noexcept _NOEXCEPT
#endif

class Error {
   public:
    virtual ~Error() = default;

    virtual const char* what() const noexcept = 0;
};

class LogicError : public Error, public std::logic_error {
   public:
    explicit LogicError(const std::string& what) : Error(), std::logic_error(what) {}
    explicit LogicError(char const* what) : Error(), std::logic_error(what) {}
    virtual ~LogicError() = default;

    virtual const char* what() const noexcept { return std::logic_error::what(); }
};

class SystemError : public Error, public std::system_error {
   public:
    SystemError(std::error_code ec) : Error(), std::system_error(ec) {}
    SystemError(std::error_code ec, std::string const& what) : Error(), std::system_error(ec, what) {}
    SystemError(std::error_code ec, char const* what) : Error(), std::system_error(ec, what) {}
    SystemError(int ev, std::error_category const& ecat) : Error(), std::system_error(ev, ecat) {}
    SystemError(int ev, std::error_category const& ecat, std::string const& what) : Error(), std::system_error(ev, ecat, what) {}
    SystemError(int ev, std::error_category const& ecat, char const* what) : Error(), std::system_error(ev, ecat, what) {}
    virtual ~SystemError() = default;

    virtual const char* what() const noexcept { return std::system_error::what(); }
};

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef noexcept
#endif

//# for val in result_enum.values
//# if "XR_ERROR" in val.name
/*{ protect_begin(val, enum) }*/
//#     set valname = create_enum_value(val.name, 'XrResult')
//#    set classname = create_enum_exception(val.name)
class /*{classname}*/ : public SystemError {
   public:
    /*{classname}*/ (std::string const& message)
        : SystemError(make_error_code(Result::/*{valname}*/), message){}

          /*{classname}*/ (char const* message)
        : SystemError(make_error_code(Result::/*{valname}*/), message) {}
};
/*{ protect_end(val, enum) }*/
//# endif
//# endfor

OPENXR_HPP_INLINE void throwResultException(Result result, char const* message) {
    switch (result) {
        //# for val in result_enum.values
        /*{ protect_begin(val, enum) }*/
        //#     set valname = create_enum_value(val.name, 'XrResult')
        case Result::/*{valname}*/:
            //# if "XR_ERROR" in val.name
            //#     set classname = create_enum_exception(val.name)
            throw /*{classname}*/ (message);
            //# else

            throw SystemError(make_error_code(result));
            //# endif
            /*{ protect_end(val, enum) }*/

            //# endfor

        default:
            throw SystemError(make_error_code(result));
    }
}
}  // namespace OPENXR_HPP_NAMESPACE

namespace std {
template <>
struct is_error_code_enum<OPENXR_HPP_NAMESPACE::Result> : public true_type {};
}  // namespace std
#endif  // !defined(OPENXR_HPP_NO_EXCEPTIONS)

namespace OPENXR_HPP_NAMESPACE {

template <typename T>
struct ResultValue {
    ResultValue(Result r, T& v) : result(r), value(v) {}

    ResultValue(Result r, T&& v) : result(r), value(std::move(v)) {}

    Result result;
    T value;

    operator std::tuple<Result&, T&>() { return std::tuple<Result&, T&>(result, value); }
};

template <typename T>
struct ResultValueType {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    typedef ResultValue<T> type;
#else
    typedef T type;
#endif
};

template <>
struct ResultValueType<void> {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    typedef Result type;
#else
    typedef void type;
#endif
};

template <typename T>
OPENXR_HPP_INLINE void ignore(T const&) {}

OPENXR_HPP_INLINE ResultValueType<void>::type createResultValue(Result result, char const* message) {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    ignore(message);
    OPENXR_HPP_ASSERT(result == Result::Success);
    return result;
#else
    if (failed(result)) {
        throwResultException(result, message);
    }
#endif
}
template <typename T>
OPENXR_HPP_INLINE typename ResultValueType<T>::type createResultValue(Result result, T& data, char const* message) {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    ignore(message);
    OPENXR_HPP_ASSERT(result == Result::Success);
    return ResultValue<T>(result, std::move(data));
#else
    if (failed(result)) {
        throwResultException(result, message);
    }
    return std::move(data);
#endif
}

OPENXR_HPP_INLINE Result createResultValue(Result result, char const* message, std::initializer_list<Result> successCodes) {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    ignore(message);
    OPENXR_HPP_ASSERT(std::find(successCodes.begin(), successCodes.end(), result) != successCodes.end());
#else
    if (std::find(successCodes.begin(), successCodes.end(), result) == successCodes.end()) {
        throwResultException(result, message);
    }
#endif
    return result;
}

template <typename T>
OPENXR_HPP_INLINE ResultValue<T> createResultValue(Result result, T& data, char const* message,
                                                   std::initializer_list<Result> successCodes) {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    ignore(message);
    OPENXR_HPP_ASSERT(std::find(successCodes.begin(), successCodes.end(), result) != successCodes.end());
#else
    if (std::find(successCodes.begin(), successCodes.end(), result) == successCodes.end()) {
        throwResultException(result, message);
    }
#endif
    return ResultValue<T>(result, data);
}

#ifndef OPENXR_HPP_NO_SMART_HANDLE
template <typename T, typename D>
OPENXR_HPP_INLINE typename ResultValueType<UniqueHandle<T, D>>::type createResultValue(
    Result result, T& data, char const* message, typename UniqueHandleTraits<T, D>::deleter const& deleter) {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    ignore(message);
    OPENXR_HPP_ASSERT(result == Result::Success);
    return ResultValue<UniqueHandle<T, D>>(result, UniqueHandle<T, D>(std::move(data), deleter));
#else
    if (failed(result)) {
        throwResultException(result, message);
    }
    return UniqueHandle<T, D>(data, deleter);
#endif
}
template <typename T, typename D>
OPENXR_HPP_INLINE ResultValue<UniqueHandle<T, D>> createResultValue(Result result, T& data, char const* message,
                                                                    typename UniqueHandleTraits<T, D>::deleter const& deleter,
                                                                    std::initializer_list<Result> successCodes) {
#ifdef OPENXR_HPP_NO_EXCEPTIONS
    ignore(message);
    OPENXR_HPP_ASSERT(std::find(successCodes.begin(), successCodes.end(), result) != successCodes.end());
    return ResultValue<UniqueHandle<T, D>>(result, UniqueHandle<T, D>(std::move(data), deleter));
#else
    if (std::find(successCodes.begin(), successCodes.end(), result) == successCodes.end()) {
        throwResultException(result, message);
    }
    return ResultValue<UniqueHandle<T, D>>(result, UniqueHandle<T, D>{data, deleter});
#endif
}
#endif
}  // namespace OPENXR_HPP_NAMESPACE
