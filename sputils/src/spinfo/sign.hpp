#pragma once

// Grammar specification of signatures
// ----------------------------------------------------
// signature    ::= <empty>                                     // empty signature
//                | '[' IDENTIFIER ']'                          // type parameter
//                | module ('.' (class | method))*              // general signature
//                ;
//
// module       ::= (IDENTIFIER ('::' IDENTIFIER)*)?;           // module part of signature
// class        ::= IDENTIFIER typeparams?;                     // class part of signature
// method       ::= IDENTIFIER typeparams? '(' params? ')';     // method part of signature
//
// typeparams   ::= '[' IDENTIFIER (',' IDENTIFIER)* ']';       // typeparams required by class or method
// params       ::= param (',' param)*;                         // param list
// param        ::= '[' IDENTIFIER ']'                          // type parameter as a param
//                | module ('.' class)+ ('(' params? ')')?      // general signature allowed by param
//                ;

#include "../spimp/common.hpp"

class SignElement;
class SignParam;

/// Represents a signature
class Sign final {
    friend class SignParser;

  public:
    /// Describes the kind of the signature
    enum class Kind {
        /// Signature is empty
        EMPTY,
        /// Signature refers to a module
        MODULE,
        /// Signature refers to a class
        CLASS,
        /// Signature refers to a method
        METHOD,
        /// Signature refers to a type param
        TYPE_PARAM
    };

  private:
    /// The signature elements
    vector<SignElement> elements;

  public:
    /**
     * Creates a signature object
     * @param text the text of the signature
     */
    Sign(const string &text);

    /**
     * Creates a signature object
     * @param elements the elements of the signature
     */
    explicit Sign(const vector<SignElement> &elements);

    Sign();
    Sign(const Sign &other);
    Sign(Sign &&other) noexcept;
    Sign &operator=(const Sign &other);
    Sign &operator=(Sign &&other) noexcept;
    ~Sign();

    const vector<SignElement> &get_elements() const {
        return elements;
    }

    vector<SignElement> &get_elements() {
        return elements;
    }

    /**
     * @return true if the signature is empty, false otherwise
     */
    bool empty() const {
        return get_kind() == Kind::EMPTY;
    }

    /**
     * @return the name of the signature
     */
    string get_name() const;

    /**
     * @return the kind of the signature
     */
    Kind get_kind() const;

    /**
     * @return the type params of the signature if exists, otherwise returns an empty array
     */
    const vector<string> &get_type_params() const;

    /**
     * @return the params of the signature if exists, otherwise returns an empty array
     */
    const vector<SignParam> &get_params() const;

    /**
     * @return the signature of the parent element
     */
    Sign get_parent() const;

    /**
     * @return the signature of the parent module
     */
    Sign get_parent_module() const;

    /**
     * @return the signature of the parent class if exists, otherwise returns an empty sign
     */
    Sign get_parent_class() const;

    /**
     * Appends a copy of this signature and another signature
     * @param sign the other signature to be appended
     * @return the appended signature
     */
    Sign operator|(const Sign &sign) const;

    /**
     * Appends a copy of this signature and a string
     * @param str the string to be appended
     * @return the appended signature
     */
    Sign operator|(const string &str) const;

    /**
     * Appends a copy of this signature and a SignElement
     * @param element the element to be appended
     * @return the appended signature
     */
    Sign operator|(const SignElement &element) const;

    /**
     * Appends this signature with another signature
     * @param sign the signature to be appended
     * @return this signature after append
     */
    Sign &operator|=(const Sign &sign);

    /**
     * Appends this signature and a string
     * @param str the string to be appended
     * @return this signature after append
     */
    Sign &operator|=(const string &str);

    /**
     * Appends this signature and a SignElement
     * @param element the element to be appended
     * @return this signature after append
     */
    Sign &operator|=(const SignElement &element);

    /**
     * @return the string representation of the sign
     */
    string to_string() const;

    static const Sign EMPTY;
};

inline bool operator==(const Sign &sign1, const Sign &sign2) {
    return sign1.to_string() == sign2.to_string();
}

inline bool operator==(const string &sign1, const Sign &sign2) {
    return sign1 == sign2.to_string();
}

inline bool operator==(const Sign &sign1, const string &sign2) {
    return sign1.to_string() == sign2;
}

template<>
struct std::hash<Sign> {
    size_t operator()(const Sign &sign) const noexcept {
        return std::hash<string>()(sign.to_string());
    }
};

class SignParam final {
    friend class SignParser;

  public:
    enum class Kind {
        /// Parameter refers to a class
        CLASS,
        /// Parameter refers to a type param
        TYPE_PARAM,
        /// Parameter refers to a callback
        CALLBACK,
    };

  private:
    Kind kind;
    Sign name;
    vector<SignParam> params;

  public:
    SignParam(Kind kind, const Sign &name, const vector<SignParam> &params = {}) : kind(kind), name(name), params(params) {}

    SignParam(const SignParam &other) = default;
    SignParam(SignParam &&other) noexcept = default;
    SignParam &operator=(const SignParam &other) = default;
    SignParam &operator=(SignParam &&other) noexcept = default;
    ~SignParam() = default;

    Kind get_kind() const {
        return kind;
    }

    const Sign &get_name() const {
        return name;
    }

    const vector<SignParam> &get_params() const {
        return params;
    }

    string to_string() const;
};

class SignElement final {
    friend class SignParser;

  private:
    string name;
    Sign::Kind kind;
    vector<string> type_params;
    vector<SignParam> params;

  public:
    SignElement(const string &name, Sign::Kind kind, const vector<string> &type_params = {}, const vector<SignParam> &params = {})
        : name(name), kind(kind), type_params(type_params), params(params) {}

    SignElement(const SignElement &other) = default;
    SignElement(SignElement &&other) noexcept = default;
    SignElement &operator=(const SignElement &other) = default;
    SignElement &operator=(SignElement &&other) noexcept = default;
    ~SignElement() = default;

    const string &get_name() const {
        return name;
    }

    Sign::Kind get_kind() const {
        return kind;
    }

    const vector<SignParam> &get_params() const {
        return params;
    }

    const vector<string> &get_type_params() const {
        return type_params;
    }

    string to_string() const;
};
