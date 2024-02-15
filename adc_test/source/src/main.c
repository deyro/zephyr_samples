/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/adc.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ADC_DEVICE_NAME         DT_LABEL(DT_ALIAS(adcctrl))
#define ADC_RESOLUTION			12
#define ADC_GAIN				ADC_GAIN_1
#define ADC_REFERENCE			ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME_DEFAULT
//#define ADC_1ST_CHANNEL_ID		8

#define BUFFER_SIZE  6
static int16_t m_sample_buffer[BUFFER_SIZE];
#define BAD_ANALOG_READ -123

//#if !defined(INVALID_ADC_VALUE)
//#define INVALID_ADC_VALUE SHRT_MIN
//#endif

static bool _IsInitialized = false;
static uint8_t _LastChannel = 250;

// the channel configuration with channel not yet filled in
static struct adc_channel_cfg m_1st_channel_cfg = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = 0, // gets set during init
	.differential	  = 0,
#if CONFIG_ADC_CONFIGURABLE_INPUTS
	.input_positive   = 0, // gets set during init
#endif
};

// return device* for the adc
static const struct device* getAdcDevice(void) {
	return device_get_binding(ADC_DEVICE_NAME);
}

// initialize the adc channel
static const struct device* init_adc(const struct device *adc_dev, int channel)
{
	int ret;
//	const struct device *adc_dev = getAdcDevice();
	if(_LastChannel != channel)
	{
		_IsInitialized = false;
		_LastChannel = channel;
	}

	if ( adc_dev != NULL && !_IsInitialized)
	{
		// strangely channel_id gets the channel id and input_positive gets id+1
		m_1st_channel_cfg.channel_id = channel;
#if CONFIG_ADC_CONFIGURABLE_INPUTS
        m_1st_channel_cfg.input_positive = channel+1,
#endif
		ret = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
		if(ret != 0)
		{
			printf("Setting up of the first channel failed with code %d", ret);
			adc_dev = NULL;
		}
		else
		{
			_IsInitialized = true;	// we don't have any other analog users
		}
	}

	memset(m_sample_buffer, 0, sizeof(m_sample_buffer));
	return adc_dev;
}

// ------------------------------------------------
// read one channel of adc
// ------------------------------------------------
static int16_t read_adc_channel(const struct device *adc_dev, int channel) {
	const struct adc_sequence sequence = {
		.options     = NULL,				// extra samples and callback
		.channels    = BIT(channel),		// bit mask of channels to read
		.buffer      = m_sample_buffer,		// where to put samples read
		.buffer_size = sizeof(m_sample_buffer),
		.resolution  = ADC_RESOLUTION,		// desired resolution
		.oversampling = 0,					// don't oversample
		.calibrate = 0						// don't calibrate
	};

	int ret;
	int16_t sample_value = BAD_ANALOG_READ;
//	const struct device *adc_dev = init_adc(channel);
	init_adc(adc_dev, channel);
	if (adc_dev) {
		ret = adc_read(adc_dev, &sequence);
		if (ret == 0) {
			sample_value = m_sample_buffer[0];
		}
	}

	return sample_value;
}

// ------------------------------------------------
// high level read adc channel and convert to float voltage
// ------------------------------------------------
static float convert_to_voltage(const struct device *adc_dev, int16_t sv) {

	// Convert the result to voltage
	// Result = [V(p) - V(n)] * GAIN/REFERENCE / 2^(RESOLUTION)

	int multip = 256;
	// find 2**adc_resolution
	switch (ADC_RESOLUTION) {
	default:
	case 8:
		multip = 256;
		break;
	case 10:
		multip = 1024;
		break;
	case 12:
		multip = 4096;
		break;
	case 14:
		multip = 16384;
		break;
	}

	// the 3.3 relates to the voltage divider being used in my circuit
	float fout = (sv * 3.3 / multip);
	return fout;
}

void main(void) {
	printk("Starting adc read\n");

	int chan_vbat = 8;	// PC2
	int chan_vbus = 7;	// PC1
	int chan_vfan = 6;	// PC0
	int16_t sv = BAD_ANALOG_READ;

	const struct device *adc_dev = getAdcDevice();

	float volts = -1;

	while (1) {
		sv = read_adc_channel(adc_dev, chan_vbat);
		if (sv == BAD_ANALOG_READ)
			printf("bad analog read chan_vbat\n");
//		printf("sample_value = %d\n", sv);

		volts = convert_to_voltage(adc_dev, sv);
		printf("VBAT (PC2) = %f\n", volts);

		sv = read_adc_channel(adc_dev, chan_vbus);
		if (sv == BAD_ANALOG_READ)
			printf("bad analog read chan_vbus\n");
//		printf("sample_value = %d\n", sv);

		volts = convert_to_voltage(adc_dev, sv);
		printf("VBUS (PC1) = %f\n", volts);

		sv = read_adc_channel(adc_dev, chan_vfan);
		if (sv == BAD_ANALOG_READ)
			printf("bad analog read chan_vfan\n");
//		printf("sample_value = %d\n", sv);

		volts = convert_to_voltage(adc_dev, sv);
		printf("VFAN (PC0) = %f\n", volts);

		printf("\n");

		k_sleep(K_MSEC(1500));
	}
}
