/**
* Signal-Slot object
*
* A signal may call multiple slots (i.e. - functions) with the same signature.
* When a signal 'emuit()' method is invoed, a function which was connected to that signal shall be called.
* Any argument passed to 'emit()' will be passed to the given function.
*
* Dan Israel Malta
**/
#pragma once

#include <functional>
#include <map>

/**
* \brief Signal
* 
* @param {Args..., in} signature
**/
template<typename... Args> class Signal {
    // properties
    private:
        mutable std::map<std::size_t, std::function<void(Args...)>> m_slots;    // signal-slot dictionary
        mutable std::size_t m_id;                                               // slot index

    // constructors
    public:

        // default constructor
        constexpr Signal() : m_id(0) {}

        // copy/move constructor
        Signal(const Signal&)     = delete;
        Signal(Signal&&) noexcept = delete;

        // copy/move assignment
        Signal& operator=(const Signal&)     = delete;
        Signal& operator=(Signal&&) noexcept = delete;

    // methods
    public:

        /**
        * \brief connect an object member function (slot) to signal
        * 
        * @param {T,      in}  object to be connected to singal
        * @param {void,   in}  object's (connecteds to signal) member function
        * @param {size_t, out} slot id
        **/
        template<typename T> std::size_t connect_member(T *inst, void (T::*func)(Args...)) {
            return connect([=](Args... args) {
                (inst->*func)(args...);
            });
        }

        /**
        * \brief connect an object member constant function (slot) to signal
        *
        * @param {T,      in}  object to be connected to singal
        * @param {void,   in}  object's (connecteds to signal) member function
        * @param {size_t, out} slot id
        **/
        template<typename T> std::size_t connect_member(T *inst, void (T::*func)(Args...) const ) {
            return connect([=](Args... args) {
                (inst->*func)(args...);
            });
        }

        /**
        * \brief connect std::function (slot) to signal.
        * 
        * @param {std::function, in}  std::function to be connected to singal
        * @param {size_t,        out} slot id
        **/
        std::size_t connect(const std::function<void(Args...)>& slot) const {
            m_slots.insert(std::make_pair(++m_id, slot));
            return m_id;
        }

        /**
        * \brief disconnects an existent slot
        *
        * @param {size_t, in} slot id
        **/
        void disconnect(std::size_t id) const {
            m_slots.erase(id);
        }

        /**
        * \brief disconnects all existent slots
        **/
        void disconnect_all() const {
            m_slots.clear();
        }

        /**
        * \brief invoke all connected slots
        **/
        void emit(Args... p) {
            for (auto it : m_slots) {
                it.second(p...);
            }
        }
};
