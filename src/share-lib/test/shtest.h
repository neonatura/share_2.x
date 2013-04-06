
#ifndef __TEST__SHTEST_H__
#define __TEST__SHTEST_H__

#include "CuTest.h"

#define _TEST(_name) void TEST_##_name(CuTest *ct) 
#define _TRUE(_proc) CuAssertTrue(ct, (_proc))
#define _TRUEPTR(_proc) CuAssertPtrNotNull(ct, (_proc))

#endif /* ndef __TEST__SHTEST_H__ */


