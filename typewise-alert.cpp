#include "typewise-alert.h"
#include <stdio.h>

// Function pointer typedef for alert functions
typedef void (*AlertFunction)(BreachType);

// Data Structure for temperature limits
typedef struct {
    int lowerLimit;
    int upperLimit;
} TemperatureLimit;

// Define the temperature limits for each cooling type
TemperatureLimit coolTypeTable[] = {
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
    TemperatureLimit limits = coolTypeTable[coolingType];
    return inferBreach(temperatureInC, limits.lowerLimit, limits.upperLimit);
}

// Array of alert functions indexed by alert target type
AlertFunction alertFunctions[] = {
    [TO_CONTROLLER] = sendToController,
    [TO_EMAIL] = sendToEmail
};

// Check and alert based on the temperature and alert target
void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC) {
    if (alertTarget < 0 || alertTarget >= sizeof(alertFunctions) / sizeof(alertFunctions[0])) {
        printf("Invalid alert target!\n");
        return;
    }

    BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);
    alertFunctions[alertTarget](breachType);
}

// Function to send alert to the controller
void sendToController(BreachType breachType) {
    const unsigned short header = 0xfeed;
    printf("%x : %x\n", header, breachType);
}

// Data Structure for sending email message
typedef struct {
    const char* message;
} EmailMessage;

// Define the email messages for each breach type
EmailMessage emailMessages[] = {
    [TOO_LOW] = {"Hi, the temperature is too low"},
    [TOO_HIGH] = {"Hi, the temperature is too high"},
    [NORMAL] = {""}
};

// Function to send alert via email
void sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";
    if (emailMessages[breachType].message != NULL && emailMessages[breachType].message[0] != '\0') {
        printf("To: %s\n", recipient);
        printf("%s\n", emailMessages[breachType].message);
    }
}
