#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>

#define HIGH 1
#define LOW 0

static uint8_t eulaData[4];
static PrintConsole *console;
static int x, y;

static int modIndex;

// Function prototypes
static void initStatics(void);
static uint16_t getEulaVersion(void);
static void setEulaVersion(uint16_t version);
static void refreshEulaVal(void);

int main(int argc, char **argv)
{
    // Initialize services
    aptInit();
    gfxInitDefault();
    hidInit();
    cfguInit();
    
    initStatics();
    
    refreshEulaVal();
    uint16_t origVal = getEulaVersion();
    
    //Flushes the buffer
    gfxFlushBuffers();
    gfxSwapBuffers();
    
    while(aptMainLoop()) //Always have the main code loop in here
    {
        hidScanInput(); //Checks which keys are pressed

        u32 button = hidKeysDown(); // Checks which keys are up and which ones are down
        if (button & KEY_LEFT)
        {
            modIndex = HIGH;
            refreshEulaVal();
        }
        if (button & KEY_RIGHT)
        {
            modIndex = LOW;
            refreshEulaVal();
        }
        if (button & KEY_UP)
        {
            eulaData[modIndex] += 1;
            refreshEulaVal();
        }
        if (button & KEY_DOWN)
        {
            eulaData[modIndex] -= 1;
            refreshEulaVal();
        }
        if (button & KEY_X)
        {
            setEulaVersion(0xFFFF);
            refreshEulaVal();
        }
        if (button & KEY_Y)
        {
            setEulaVersion(0x0000);
            refreshEulaVal();
        }
        if (button & KEY_START)
        {
            CFG_UpdateConfigNANDSavegame();
            break;
        }
        if (button & KEY_SELECT)
        {
            setEulaVersion(origVal);
            CFG_UpdateConfigNANDSavegame();
            break;
        }
    }

    // Exit services
    aptExit();
    gfxExit();
    hidExit();
    cfguExit();
    return 0;
}

static void initStatics(void)
{
    // Initialize static pointers/values
    console = consoleInit(GFX_TOP, NULL);
    printf("\nEULA Setter:\n");
    printf("Press UP to increase selected byte\n");
    printf("Press DOWN to decrease selected byte\n");
    printf("Press LEFT to modify high byte\n");
    printf("Press RIGHT to modify low byte\n");
    printf("Press X for 0xFFFF\n");
    printf("Press Y for 0x0000\n");
    printf("Press START to save and exit\n");
    printf("Press SELECT to exit without saving\n\n");
    printf("Current accepted EULA version: ");
    
    x = console->cursorX;
    y = console->cursorY;
    
    modIndex = LOW;
    
    CFGU_GetConfigInfoBlk2(4, 0xD0000, eulaData);
}

static uint16_t getEulaVersion(void)
{
    return (uint16_t)((eulaData[HIGH] << 8) | (eulaData[LOW]));
}

static void setEulaVersion(uint16_t version)
{
    *((uint16_t *)eulaData) = version;
    CFG_SetConfigInfoBlk8(4, 0xD0000, eulaData);
}

static void refreshEulaVal(void)
{
    console->cursorX = x;
    console->cursorY = y;
    setEulaVersion(getEulaVersion());
    if (modIndex == LOW)
    {
        printf(" %02X .[%02X]\n\n", eulaData[HIGH], eulaData[LOW]);
    }
    else
    {
        printf("[%02X]. %02X \n\n", eulaData[HIGH], eulaData[LOW]);
    }
}