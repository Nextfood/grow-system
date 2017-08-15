#ifndef _SetupAds1018_H_
#define _SetupAds1018_H_

class SetupAds1018 {
public:

    enum datarateEnum {
        DATARATE_128_SPS = 0,
        DATARATE_250_SPS = 1,
        DATARATE_490_SPS = 2,
        DATARATE_920_SPS = 3,
        DATARATE_1600_SPS = 4,
        DATARATE_2400_SPS = 5,
        DATARATE_3300_SPS = 6
    };

    enum muxEnum {
        MUX_DIFF_0_1 = 0,
        MUX_DIFF_0_3 = 1,
        MUX_DIFF_1_3 = 2,
        MUX_DIFF_2_3 = 3,
        MUX_SE_0 = 4,
        MUX_SE_1 = 5,
        MUX_SE_2 = 6,
        MUX_SE_3 = 7
    };

    enum pgaEnum {
        PGA_FSR_6_144V = 0,
        PGA_FSR_4_096V = 1,
        PGA_FSR_2_048V = 2,
        PGA_FSR_1_024V = 3,
        PGA_FSR_0_512V = 4,
        PGA_FSR_0_256V = 5
    };

    struct ntc {
        pgaEnum pga;
        datarateEnum datarate;
        muxEnum mux;
        float refRes;
        float res0;
        float b25_50;
        float t0;
    };

    char* id;
    int pin_cs = 10;
    int pin_sclk = 13;
    int pin_mosi = 11;
    int pin_miso = 12;

    ntc voltage_ref = {PGA_FSR_6_144V, DATARATE_128_SPS, MUX_SE_0, 66000.0, 10000.0, 3435.0, 25.0};
    ntc thermistor = {PGA_FSR_2_048V, DATARATE_128_SPS, MUX_DIFF_2_3, 66000.0, 10000.0, 3435.0, 25.0};
};


#endif
