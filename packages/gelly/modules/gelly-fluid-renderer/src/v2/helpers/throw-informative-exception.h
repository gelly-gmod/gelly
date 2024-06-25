#ifndef THROW_INFORMATIVE_EXCEPTION_H
#define THROW_INFORMATIVE_EXCEPTION_H

#include <winerror.h>

#include <exception>

#ifndef GELLY_FUNCTION_NAME
#ifdef _MSC_VER
#define GELLY_FUNCTION_NAME __FUNCTION__
#else
#define GELLY_FUNCTION_NAME __func__
#endif
#endif

#define GELLY_RENDERER_THROW(classname, message) \
	throw classname(GELLY_FUNCTION_NAME ": " message)

#define GELLY_RENDERER_THROW_ON_FAIL(result, classname, message) \
	if (FAILED(result)) {                                        \
		GELLY_RENDERER_THROW(classname, message);                \
	}

#endif	// THROW_INFORMATIVE_EXCEPTION_H
