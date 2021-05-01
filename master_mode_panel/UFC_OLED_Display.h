#include "Wire.h"
#include <SSD1311.h>

extern char current_module[];

SSD1311 OLED;

void OLED_20x2_setup()
{
    Wire.begin();
    OLED.powerMode(SSD1311_LCD_OFF);
    OLED.selectRomRam(SSD1311_ROM_C, SSD1311_CGRAM_1); // Change of font needed to access degree symbol at char(223).
    OLED.clear();
    // Enable second line of OLED.
    OLED.sendCommand(0x2A); // **** Set "RE"=1
    OLED.sendCommand(0x08); // **** Set 5-dot, 3 or 4 line(0x09), 1 or 2 line(0x08)
    OLED.sendCommand(0x28); // **** Set "IS"=0 , "RE" =0 //28

    OLED.powerMode(SSD1311_LCD_ON);
    // String latlon = "N41o43E54o12'1";
    // latlon[3] = char(223);
    // OLED.sendString(latlon.c_str(), 3, 0);
    // OLED.sendString("Base.ino", 5, 1);
}

char *hornet_symbols(char *newValue)
{
    int i = 0;
    while (newValue[i] != '\0')
    {
        switch (newValue[i])
        {
        case '@':
            newValue[i] = char(223); // degree symbol
            break;
        case '.':
            newValue[i] = char(161); // lower circle symbol
            break;
        // The below handles the case for "ERROR" shows up as "pww0w"
        case 'p':
            newValue[i] = 'E'; // lower circle symbol
            break;
        case 'w':
            newValue[i] = 'R'; // lower circle symbol
            break;
        }
        i++;
    }
    return newValue;
}

char *harrier_symbols(char *newValue)
{
    int i = 0;
    while (newValue[i] != '\0')
    {
        switch (newValue[i])
        {
        case '@':
            newValue[i] = char(223); // degree symbol
            break;
        }
        i++;
    }
    return newValue;
}

String blackshark_symbols(char *input)
{
    int iInput = 0;
    int iOutput = 0;
    String output = "        ";
    while (input[iInput] != '\0' && iInput < 6)
    {
        if (iInput == 3 || iInput == 5)
        {
            output[iOutput++] = char(39); // apostrophe ' symbol
        }
        output[iOutput++] = input[iInput++];
    }
    return output;
}

// F/A-18
void onUfcScratchpadNumberDisplayChange(char *newValue)
{
    if (!strcmp(current_module, "FA-18C_hornet"))
    {
        OLED.sendString(hornet_symbols(newValue), 6, 1);
    }
}
DcsBios::StringBuffer<8> ufcScratchpadNumberDisplayBuffer(0x7446, onUfcScratchpadNumberDisplayChange);

void onUfcScratchpadString1DisplayChange(char *newValue)
{
    if (!strcmp(current_module, "FA-18C_hornet"))
    {
        OLED.sendString(hornet_symbols(newValue), 2, 1);
    }
}
DcsBios::StringBuffer<2> ufcScratchpadString1DisplayBuffer(0x744e, onUfcScratchpadString1DisplayChange);

void onUfcScratchpadString2DisplayChange(char *newValue)
{
    if (!strcmp(current_module, "FA-18C_hornet"))
    {
        OLED.sendString(hornet_symbols(newValue), 5, 1);
    }
}
DcsBios::StringBuffer<2> ufcScratchpadString2DisplayBuffer(0x7450, onUfcScratchpadString2DisplayChange);

// AV8BNA
void onUfcScratchpadChange(char *newValue)
{
    if (!strcmp(current_module, "AV8BNA"))
    {
        OLED.sendString(harrier_symbols(newValue), 2, 1);
    }
}
DcsBios::StringBuffer<12> ufcScratchpadBuffer(0x7986, onUfcScratchpadChange);

// A-10C
void onCmscTxtJmrChange(char *newValue)
{
    if (!strcmp(current_module, "A-10C") || !strcmp(current_module, "A-10C_2"))
    {
        OLED.sendString(newValue, 0, 0);
    }
}
DcsBios::StringBuffer<8> cmscTxtJmrBuffer(0x1096, onCmscTxtJmrChange);
void onCmscTxtMwsChange(char *newValue)
{
    if (!strcmp(current_module, "A-10C") || !strcmp(current_module, "A-10C_2"))
    {
        OLED.sendString(newValue, 0, 1);
    }
}
DcsBios::StringBuffer<8> cmscTxtMwsBuffer(0x12b0, onCmscTxtMwsChange);
void onCmscTxtChaffFlareChange(char *newValue)
{
    if (!strcmp(current_module, "A-10C") || !strcmp(current_module, "A-10C_2"))
    {
        OLED.sendString(newValue, 8, 0);
    }
}
DcsBios::StringBuffer<8> cmscTxtChaffFlareBuffer(0x108e, onCmscTxtChaffFlareChange);
void onCmscLaunchChange(unsigned int newValue)
{
    if (!strcmp(current_module, "A-10C") || !strcmp(current_module, "A-10C_2"))
    {
        if (newValue)
        {
            OLED.sendString("MISSILE", 8, 1);
        }
        else
        {

            OLED.sendString("       ", 8, 1);
        }
    }
}
DcsBios::IntegerBuffer cmscLaunchBuffer(0x1012, 0x0100, 8, onCmscLaunchChange);

// KA-50
void onPviLine1SignChange(char *newValue)
{
    if (!strcmp(current_module, "Ka-50"))
    {
        OLED.sendString(newValue, 0, 0);
    }
}
DcsBios::StringBuffer<1> pviLine1SignBuffer(0x1920, onPviLine1SignChange);
void onPviLine1TextChange(char *newValue)
{
    if (!strcmp(current_module, "Ka-50"))
    {
        String lineText = newValue;
        OLED.sendString(lineText.substring(0, 3).c_str(), 1, 0);
        OLED.sendString(lineText.substring(3, 5).c_str(), 5, 0);
        OLED.sendString(lineText.substring(5, 6).c_str(), 8, 0);
    }
}
DcsBios::StringBuffer<6> pviLine1TextBuffer(0x1924, onPviLine1TextChange);
void onPviLine1PointChange(char *newValue)
{
    if (!strcmp(current_module, "Ka-50"))
    {
        OLED.sendString(newValue, 11, 0);
        OLED.sendString("WP", 13, 0);
    }
}
DcsBios::StringBuffer<1> pviLine1PointBuffer(0x1930, onPviLine1PointChange);

void onPviLine1Apostrophe1Change(char *newValue) { OLED.sendString(newValue, 4, 0); }
DcsBios::StringBuffer<1> pviLine1Apostrophe1Buffer(0x1934, onPviLine1Apostrophe1Change);
void onPviLine1Apostrophe2Change(char *newValue) { OLED.sendString(newValue, 7, 0); }
DcsBios::StringBuffer<1> pviLine1Apostrophe2Buffer(0x1936, onPviLine1Apostrophe2Change);

void onPviLine2SignChange(char *newValue)
{
    if (!strcmp(current_module, "Ka-50"))
    {
        OLED.sendString(newValue, 0, 1);
    }
}
DcsBios::StringBuffer<1> pviLine2SignBuffer(0x1922, onPviLine2SignChange);
void onPviLine2TextChange(char *newValue)
{
    if (!strcmp(current_module, "Ka-50"))
    {
        String lineText = newValue;
        OLED.sendString(lineText.substring(0, 3).c_str(), 1, 1);
        OLED.sendString(lineText.substring(3, 5).c_str(), 5, 1);
        OLED.sendString(lineText.substring(5, 6).c_str(), 8, 1);
    }
}
DcsBios::StringBuffer<6> pviLine2TextBuffer(0x192a, onPviLine2TextChange);
void onPviLine2PointChange(char *newValue)
{
    if (!strcmp(current_module, "Ka-50"))
    {
        OLED.sendString(newValue, 11, 1);
        OLED.sendString("POI", 13, 1);
    }
}
DcsBios::StringBuffer<1> pviLine2PointBuffer(0x1932, onPviLine2PointChange);
void onPviLine2Apostrophe1Change(char *newValue) { OLED.sendString(newValue, 4, 1); }
DcsBios::StringBuffer<1> pviLine2Apostrophe1Buffer(0x1938, onPviLine2Apostrophe1Change);
void onPviLine2Apostrophe2Change(char *newValue) { OLED.sendString(newValue, 7, 1); }
DcsBios::StringBuffer<1> pviLine2Apostrophe2Buffer(0x193a, onPviLine2Apostrophe2Change);