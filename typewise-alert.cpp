#include "typewise-alert.h"
#include <stdio.h>

checkAndAlert(TO_EMAIL, batteryChar, 45.0, "user@example.com");

// Define constants
#define ALERT_FUNCTION_COUNT 2
#define CONTROLLER_HEADER 0xfeed

// Function pointer typedef for alert functions
typedef void (*AlertFunction)(BreachType, const char*);

// Data Structure for temperature limits
typedef struct {
    int lowerLimit;
    int upperLimit;
} TemperatureLimit;

// Define the temperature limits for each cooling type
TemperatureLimit limitsTable[] = {
    [PASSIVE_COOLING] = {0, 35},
    [HI_ACTIVE_COOLING] = {0, 45},
    [MED_ACTIVE_COOLING] = {0, 40}
};

// Infer the breach type based on the temperature and limits
BreachType inferBreach(double value, double lowerLimit, double upperLimit) {
    if (value < lowerLimit) {
        return TOO_LOW;
    }
    if (value > upperLimit) {
        return TOO_HIGH;
    }
    return NORMAL;
}

// Classify the temperature breach based on the cooling type
BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
    TemperatureLimit limits = limitsTable[coolingType];
    return inferBreach(temperatureInC, limits.lowerLimit, limits.upperLimit);
}

// Function to send alert to the controller
void sendToController(BreachType breachType, const char* unused) {
    (void)unused; // Unused parameter
    printf("%x : %x\n", CONTROLLER_HEADER, breachType);
}

// Data Structure for email messages
typedef struct {
    const char* message;
} EmailMessage;

// Define the email messages for each breach type
EmailMessage emailMessages[] = {
    [TOO_LOW] = {"Hi, the temperature is too low"},
    [TOO_HIGH] = {"Hi, the temperature is too high"},
    [NORMAL] = {""}
};

// Function to send alert via email to a specified recipient
void sendToEmail(BreachType breachType, const char* recipient) {
    if (emailMessages[breachType].message != NULL && emailMessages[breachType].message[0] != '\0') {
        printf("To: %s\n", recipient);
        printf("%s\n", emailMessages[breachType].message);
    }
}

// Array of alert functions indexed by alert target type
AlertFunction alertFunctions[ALERT_FUNCTION_COUNT] = {
    [TO_CONTROLLER] = sendToController,
    [TO_EMAIL] = sendToEmail
};

// Check and alert based on the temperature, alert target, and recipient email
void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC, const char* recipient) {
    if (alertTarget < 0 || alertTarget >= ALERT_FUNCTION_COUNT) {
        printf("Invalid alert target!\n");
        return;
    }

    BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);
    alertFunctions[alertTarget](breachType, recipient);
}
