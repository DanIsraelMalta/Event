#include "Signal.h"
#include "Property.h"
#include <iostream>
#include <sstream>

struct Button {
    Signal<> on_click;
};

struct Message {
    void display() const {
        std::cout << "Hello World!" << std::endl;
    }
};

class Person {
    private:
        std::string m_name;

    public:
        Person(const std::string& xi_name) : m_name(xi_name) {}

        Signal<const std::string&> say;

        void listen(const std::string &message) {
            std::cout << m_name << " received: " << message << std::endl;
        }
};

void test_signal_1() {
    Button  button;
    Message message;

    button.on_click.connect_member(&message, &Message::display);
    button.on_click.emit();
}
void test_signal_2() {
    Person alice("Alice"), bob("Bob");

    alice.say.connect_member(&bob, &Person::listen);
    bob.say.connect_member(&alice, &Person::listen);

    alice.say.emit("Have a nice day!");
    bob.say.emit("Thank you!");
}

void test_property_1() {
    Property<int> Integer;

    Integer.on_change().connect([](int val) {
        std::cout << "Value changed to: " << val << std::endl;
    });

    std::cout << "Value: " << Integer << std::endl;

    std::stringstream sstr("42");
    sstr >> Integer;
}

void test_property_2() {
    Property<float> InputValue;
    Property<float> OutputValue;
    Property<bool>  CriticalSituation;

    OutputValue.connect_from(InputValue);

    OutputValue.on_change().connect([&](float val) {
        std::cout << "Output: " << val << std::endl;
        if (val > 0.5f) CriticalSituation = true;
        else            CriticalSituation = false;
    });

    CriticalSituation.on_change().connect([](bool val) {
        if (val) std::cout << "Danger danger!" << std::endl;
    });

    InputValue = 0.2;
    InputValue = 0.4;
    InputValue = 0.6;
}

int main() {
    test_signal_1();
    test_signal_2();
    test_property_1();
    test_property_2();

    return 1;
}
