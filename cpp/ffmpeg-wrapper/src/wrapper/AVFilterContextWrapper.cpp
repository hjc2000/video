#include"AVFilterContextWrapper.h"

using namespace video;
using namespace std;

AVFilterContextWrapper::AVFilterContextWrapper()
{

}

AVFilterContextWrapper::AVFilterContextWrapper(AVFilterContext *filter_ctx)
{
	_wrapped_obj = filter_ctx;
}

AVFilterContextWrapper::AVFilterContextWrapper(AVFilterContextWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
}

AVFilterContextWrapper &AVFilterContextWrapper::operator=(AVFilterContextWrapper const &other)
{
	_wrapped_obj = other._wrapped_obj;
	return *this;
}

void AVFilterContextWrapper::link(AVFilterContextWrapper &next_filter)
{
	if (avfilter_link(_wrapped_obj, 0, next_filter, 0))
	{
		throw jc::Exception();
	}
}

AVFilterContextWrapper &AVFilterContextWrapper::operator<<(AVFilterContextWrapper &next_filter)
{
	link(next_filter);
	return next_filter;
}
