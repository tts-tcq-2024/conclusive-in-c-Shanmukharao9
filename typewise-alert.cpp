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
TemperatureLimit coolTypeTable[3];

// Function to initialize temperature limits
void initTemperatureLimits() {
    coolTypeTable[PASSIVE_COOLING].lowerLimit = 0;
    coolTypeTable[PASSIVE_COOLING].upperLimit = 35;

    coolTypeTable[HI_ACTIVE_COOLING].lowerLimit = 0;
    coolTypeTable[HI_ACTIVE_COOLING].upperLimit = 45;

    coolTypeTable[MED_ACTIVE_COOLING].lowerLimit = 0;
    coolTypeTable[MED_ACTIVE_COOLING].upperLimit = 40;
}

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

// Function declarations for alert actions
void sendToController(BreachType breachType);
void sendToEmail(BreachType breachType);

// Array of alert functions indexed by alert target type
AlertFunction alertFunctions[2];

// Function to initialize alert functions
void initAlertFunctions() {
    alertFunctions[TO_CONTROLLER] = sendToController;
    alertFunctions[TO_EMAIL] = sendToEmail;
}

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
EmailMessage emailMessages[3];

// Function to initialize email messages
void initEmailMessages() {
    emailMessages[TOO_LOW].message = "Hi, the temperature is too low";
    emailMessages[TOO_HIGH].message = "Hi, the temperature is too high";
    emailMessages[NORMAL].message = "";
}

// Function to send alert via email
void sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";
    if (emailMessages[breachType].message != NULL && emailMessages[breachType].message[0] != '\0') {
        printf("To: %s\n", recipient);
        printf("%s\n", emailMessages[breachType].message);
    }
}

// Main function or initialization function
int main() {
    // Initialize all necessary data
    initTemperatureLimits();
    initAlertFunctions();
    initEmailMessages();

    // Example usage
    BatteryCharacter batteryChar;
    batteryChar.coolingType = HI_ACTIVE_COOLING;
    checkAndAlert(TO_EMAIL, batteryChar, 50.0);

    return 0;
}
