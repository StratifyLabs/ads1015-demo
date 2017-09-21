
#ifndef ADS1015_HPP_
#define ADS1015_HPP_

#include <sapi/hal.hpp>

class Ads1015 : public I2C {
public:
	Ads1015();

	int init();
	int read(int channel);

	int read_mv(int channel);

	using I2C::read;



private:
	u8 slave_addr() const { return m_slave_addr; }
	u8 m_slave_addr;
	u16 m_config;

	u16 byte_swap(u16 byte);

};

#endif /* ADS1015_HPP_ */
