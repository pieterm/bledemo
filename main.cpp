/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"


#define LOG(...)    { pc.printf(__VA_ARGS__); }

#define LED_GREEN   p21
#define LED_RED     p22
#define LED_BLUE    p23
#define BUTTON_PIN  p17
#define BATTERY_PIN p1

#define UART_TX     p9
#define UART_RX     p11

Serial pc(UART_TX, UART_RX);

DigitalOut ledBlue(LED_BLUE, 1);
DigitalOut ledGreen(LED_GREEN, 1);
DigitalOut ledRed(LED_RED, 1);
 
const static char     DEVICE_NAME[]        = "<CHANGE ME!>";
 
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    LOG("Disconnected, restart advertising\n");
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising(); // restart advertising
}

void connectionCallback(const Gap::ConnectionCallbackParams_t *params)
{
    LOG("Connected\n");
}

void periodicCallback(void)
{
    ledBlue = !ledBlue; /* Do blinky on LED1 while we're waiting for BLE events */
}
 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE &ble = params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) {
        return;
    }

    /* setup service(s) */
    
    /* set callback functions */
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gap().onConnection(connectionCallback);
    
    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */

    ble.gap().startAdvertising();
}
 
int main(void)
{
    pc.baud(115200);
    
    Ticker ticker;
    ticker.attach(periodicCallback, 1); // blink LED every second
 
    LOG("Initialising the nRF51822\n");
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(bleInitComplete);
 
    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }
 
    LOG("Initialized, running now\n");
    // infinite loop
    while (1) {
        ble.waitForEvent(); // low power wait for event
    }
}

