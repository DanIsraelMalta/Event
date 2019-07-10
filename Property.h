/**
* A property encapsulates a value, and inform a subscriber when that value changes.
*
* Dan Israel Malta
**/
#pragma once
#include "Signal.h"
#include <iostream>
#include <type_traits>

// forward deceleration of Property
template<typename T> class Property;

/**
* type trait to test if an object is a Property
**/
template<typename T> struct IsProperty              { static constexpr bool value{ false }; };
template<typename T> struct IsProperty<Property<T>> { static constexpr bool value{ true };  };

/**
* \brief property
* 
* @param {T, in} type of underlying value
**/
template<typename T> class Property {
    // properties
    private:
        Signal<T> m_on_change;              // a signal which is invoked *when*   internal value is changed
        Signal<T> m_prior_to_change;        // a signal which is invoked *before* internal value has changed

        const Property<T>* m_connection;    // a property to which current property is connected
        std::size_t m_connection_id;        //connection ID (index)
        T m_value;                          // held value

    // constructor
    public:

        // default constructor
        constexpr Property() : m_value{}, m_connection(nullptr), m_connection_id(-1) {}

        // value constructors
        explicit constexpr Property(const T& xi_val) : m_value(xi_val),            m_connection(nullptr), m_connection_id(-1) {}
        explicit constexpr Property(T&& xi_val)      : m_value(std::move(xi_val)), m_connection(nullptr), m_connection_id(-1) {}

        // move/copy constructors
        Property(const Property<T>& xi_other) : m_value(xi_other.m_value),            m_connection(nullptr), m_connection_id(-1) {}
        Property(Property<T>&& xi_other)      : m_value(std::move(xi_other.m_value)), m_connection(nullptr), m_connection_id(-1) {}

        // move/copy assignments
        Property& operator=(const Property& xi_other) {
            set(xi_other.m_value);

            return *this;
        }
        Property& operator=(Property&& xi_other) noexcept {
            set(std::move(xi_other.m_value));
            xi_other.m_value = T{};

            return *this;
        }

        // assigns a new value to Property
        template<typename U, typename std::enable_if<!IsProperty<U>::value && std::is_convertible<U, T>::value>::type* = nullptr> 
        Property& operator=(const U& xi_value) {
            set(static_cast<T>(xi_value));
            return *this;
        }

    //  relational operator overloading
    public:

        // compares the values of two Properties
        constexpr bool operator==(const Property<T>& xi_rhs) const {
            return Property<T>::get() == xi_rhs.get();
        }

        constexpr bool operator!=(const Property<T>& xi_rhs) const {
            return Property<T>::get() != xi_rhs.get();
        }

        // compares the values of the Property to another value
        constexpr bool operator==(const T& xi_rhs) const { return (Property<T>::get() == xi_rhs); }
        constexpr bool operator!=(const T& xi_rhs) const { return (Property<T>::get() != xi_rhs); }

    // operator overloading
    public:

        // '()' operator overloading (returns Property internal value)
        const T& operator()() const { return Property<T>::get(); }

    // methods
    public:

        /**
        * \brief return the 'prior_to_change' signal
        * 
        * @param {Signal<T>, out} return m_prior_to_change
        **/
        const Signal<T>& prior_to_change() const { return m_prior_to_change; }

        /**
        * \brief return the 'on_change' signal
        *
        * @param {Signal<T>, out} return m_on_change
        **/
        const Signal<T>& on_change() const { return m_on_change; }

        /**
        * \brief set property value.
        *        'm_prior_to_change' will be invoked prior to value change.
        *        'm_on_change' will be invoked after value change.
        * 
        * @param {T , in} new value
        **/
        virtual void set(const T& value) {
            if (value == m_value) return;

            m_prior_to_change.emit(m_value);
            m_value = value;
            m_on_change.emit(m_value);
        }

        /**
        * \brief set property value, without invoking signals
        * 
        * @param {T , in} new value
        **/
        void set_with_no_emit(const T& value) {
            m_value = value;
        }

        /**
        * \brief invoke 'm_prior_to_change' && 'm_on_change' without changing property value
        **/
        void touch() {
            m_prior_to_change.emit(m_value);
            m_on_change.emit(m_value);
        }

        /**
        * \brief return internal value
        * 
        * @param {T, out} internal value
        **/
        const T& get() const { return m_value; }

        /**
        * \brief connect two properties to each other,
        *        i.e. - if source property value has changed
        *       current property value will change accordingly.
        * 
        * @param {Property, in} binded property
        * @param {, out} 
        **/
        void connect_from(const Property<T>& source) noexcept {
            disconnect();
            m_connection = &source;
            m_connection_id = source.on_change().connect([this](T const& value) {
                set(value);
                return true;
            });
            set(source.get());
        }

        /**
        * \brief disconnect from any binded property
        **/
        void disconnect() noexcept {
            if (!m_connection) return;

            m_connection->on_change().disconnect(m_connection_id);
            m_connection_id = -1;
            m_connection = nullptr;
        }

        /**
        * \brief silent property notifications
        **/
        void disconnect_auditors() noexcept {
            m_on_change.disconnect_all();
            m_prior_to_change.disconnect_all();
        }   
};

template<typename T> std::istream& operator >> (std::istream& xi_stream, Property<T>& xi_value) {
    T tmp;
    xi_stream >> tmp;
    xi_value.set(tmp);
    return xi_stream;
}

template<typename T> std::ostream& operator << (std::ostream& xo_stream, const Property<T>& xi_value) {
    xo_stream << xi_value.get();
    return xo_stream;
}
