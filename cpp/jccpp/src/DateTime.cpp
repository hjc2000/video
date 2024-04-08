#include"jccpp/DateTime.h"

using namespace std;
using namespace std::chrono;

int64_t DateTime::GetRealTimeInMilliseconds()
{

	duration since_epoch = system_clock::now().time_since_epoch();
	int64_t millis = duration_cast<milliseconds>(since_epoch).count();
	return millis;
}

int64_t DateTime::GetSteadyTimeInMilliseconds()
{
	duration now = steady_clock::now().time_since_epoch();
	int64_t now_in_milliseconds = duration_cast<milliseconds>(now).count();
	return now_in_milliseconds;
}
