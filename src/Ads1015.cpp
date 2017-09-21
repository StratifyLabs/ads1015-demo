
#include <sapi/sys.hpp>
#include "ads1015.h"
#include "Ads1015.hpp"

Ads1015::Ads1015() : I2C(1) {
	m_slave_addr = 0x48; //use this as the default
}



int Ads1015::init(){
	Trace t;

	if( I2C::init(I2C::FLAG_SET_MASTER, 100000) < 0 ){
		t.assign("I2C init");
		t.trace_critical();
		return -1;
	}

	prepare(slave_addr());

	if( read(ADS1015_CONFIG_ADDRESS, &m_config, 2) != 2 ){
		t.assign("Read Config");
		t.trace_critical();
		return -1;
	}

	m_config = byte_swap(m_config);


	return 0;
}

int Ads1015::read(int channel){
	Trace t;
	u16 config;
	u16 data;
	prepare(slave_addr());

	//channel is config
	config = m_config;
	config &= ~ADS1015_CONFIG_CHANNEL_MASK;
	config |= ADS1015_CONFIG_START_CONVERSION;
	config |= (((channel & 0x03) | 0x04) << ADS1015_CONFIG_CHANNEL_SHIFT);

	config = byte_swap(config);
	if( write(ADS1015_CONFIG_ADDRESS, &config, 2) != 2 ){
		t.assign("set config");
		t.trace_critical();
		return -1;
	}

	//wait for the conversion to complete
	Timer::wait_msec(5);

	if( read(ADS1015_CONVERSION_ADDRESS, &data, 2) != 2 ){
		t.assign("read data");
		t.trace_critical();
		return -1;
	}

	data = byte_swap(data);
	return data;

}

int Ads1015::read_mv(int channel){
	int data = read(channel);
	if( data < 0 ){
		return -1;
	}

	return (data * 2048 + 0x7ff0/2) / 0x7ff0;


}


u16 Ads1015::byte_swap(u16 byte){
	u8 swap[2];
	swap[0] = byte >> 8;
	swap[1] = byte & 0xff;
	return swap[0] | swap[1] << 8;
}

