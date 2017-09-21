
#include <cstdio>
#include <sos/dev/ffifo.h>
#include <sapi/sys.hpp>
#include <sapi/var.hpp>
#include <sapi/hal.hpp>

#include "TempSensor.hpp"
#include "Ads1015.hpp"

volatile bool m_is_quit;

void signal_quit(int a){
	m_is_quit = true;
}

static int monitor_temperature(const char * device, int channel, int period);

int main(int argc, char * argv[]){
	SignalHandler handler(signal_quit);
	Signal event(Signal::QUIT);
	Trace t;

	Cli cli(argc, argv);
	int channel = -1;
	int period = 1000;
	String device;

	//check for -v and --version options
	cli.handle_version();

	if( cli.is_option("-o") ){
		device = cli.get_option_argument("-o");
	}

	if( cli.is_option("--channel") ){
		channel = cli.get_option_value("--channel");
	}

	if( cli.is_option("-p") ){
		period = cli.get_option_value("-p");
		if( period < 50 ){
			period = 50;
		}

		if( period > 5000 ){
			period = 5000;
		}
	}


	t.assign("Start");
	t.trace_message();

	m_is_quit = false;
	event.set_handler(handler);
	if( monitor_temperature(device.c_str(), channel, period) < 0 ){
		t.assign("Monitor error");
		t.trace_fatal();
		exit(1);
	}

	t.assign("Done");
	t.trace_message();
	return 0;
}

int monitor_temperature(const char * device, int channel, int period){
	Dev output;
	Ads1015 adc;
	float temp;
	u16 data;
	Trace t;
	int count;

	if( adc.init() < 0 ){
		t.assign("ADC Init Failed");
		t.trace_fatal();
		exit(0);
	}

	if( output.open(device, Dev::RDWR) < 0 ){
		t.assign("open failed");
		t.trace_critical();
		return -1;
	}

	count = 0;
	do {
		ChartString point;
		point.start_points();

		data = adc.read_mv(0);
		temp = TempSensor::calc_temperature(data);

		Timer::wait_msec(period);
		point.append_pointf(count, temp);
		point.end_points();

		output.write(channel | 0x01, point.c_str(), point.len());
		count++;
	} while( m_is_quit == false);

	output.close();

	return 0;



}
