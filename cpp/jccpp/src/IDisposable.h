#pragma once
#include<atomic>

class IDisposable
{
public:
	virtual ~IDisposable() {}
	virtual void Dispose() = 0;
};