#ifndef __DCSBIOS_MODULE_LEDS_H
#define __DCSBIOS_MODULE_LEDS_H

#include <DcsBios.h>

/***
 * ModuleLED
 * 
 * This is a modification of the standard DcsBios::LED that allows the functionality to only be enabled when the specified module is in use.
 * 
***/

namespace DcsBios
{
    class ModuleLED : public LED
    {
    private:
        char *moduleName;
        bool is_active = false;
        unsigned char pin;

    public:
        ModuleLED(char *moduleName, unsigned int address, unsigned int mask, char pin) : moduleName(moduleName), pin(pin), LED(address, mask, pin)
        {
        }

        virtual void loop()
        {
            if (is_active)
            {
                LED::loop();
            }
        }

        void set_active_according_to_module(char *newValue)
        {
            digitalWrite(pin, LOW); // Turn off by default until turned on again by module.
            is_active = !strcmp(moduleName, newValue);
        }
    };
}

#endif
