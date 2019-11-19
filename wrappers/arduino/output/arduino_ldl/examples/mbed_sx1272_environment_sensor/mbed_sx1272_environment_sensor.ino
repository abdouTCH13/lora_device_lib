/* Copyright (c) 2019 Cameron Harper
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * */

#include <arduino_ldl.h>
#include "src/Grove_Temperature_And_Humidity_Sensor/DHT.h"

static void get_identity(struct arduino_ldl_id *id)
{       
    static const struct arduino_ldl_id _id = {
        .joinEUI = {0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U},
        .devEUI = {0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x00U,0x01U},
        .appKey = {0x2bU,0x7eU,0x15U,0x16U,0x28U,0xaeU,0xd2U,0xa6U,0xabU,0xf7U,0x15U,0x88U,0x09U,0xcfU,0x4fU,0x3cU},
        .nwkKey = {0x2bU,0x7eU,0x15U,0x16U,0x28U,0xaeU,0xd2U,0xa6U,0xabU,0xf7U,0x15U,0x88U,0x09U,0xcfU,0x4fU,0x3cU}
    };

    memcpy(id, &_id, sizeof(*id));
}

LDL::MAC& get_ldl()
{
    static LDL::SX1272 radio(
        LDL_RADIO_PA_RFO,  /* specify power amplifier */
        A0,                 /* radio reset pin */
        10,                 /* radio select pin */
        2,                  /* radio dio0 pin */
        3                   /* radio dio1 pin */
    );
    
    static LDL::MAC mac(
        radio,              /* mac needs a radio */
        LDL_EU_863_870,         /* specify region */
        get_identity        /* specify name of function that returns euis and keys */               
    );
    
    return mac;
}

DHT dht(
    6,                  /* pin */
    DHT11               /* sensor type */
);

static void on_rx(uint16_t counter, uint8_t port, const uint8_t *data, uint8_t size)
{
    // do something with this information
    (void)counter;
    (void)port;
    (void)data;
    (void)size;
}

void setup() 
{
    Serial.begin(115200U);       
    dht.begin();            
 
    LDL::MAC& ldl = get_ldl();

    ldl.onRX(on_rx);
    ldl.onEvent(ldl.eventDebug);
}

void loop() 
{  
    LDL::MAC& ldl = get_ldl();
    
    if(ldl.ready()){
    
        if(ldl.joined()){
                
            float buf[] = {
                dht.readTemperature(),
                dht.readHumidity()
            };
            
            ldl.unconfirmedData(1U, buf, sizeof(buf));
        }
        else{
        
            
            ldl.otaa();
        }
    }
    
    ldl.process();            
}
