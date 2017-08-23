#include "cases.h"

#include <iostream>

int main(int argc, char* argv[])
{
	case_InitSuccess();
	case_InitFail();

	case_InitReadSuccess();
	case_InitReadFail();

	case_gotoRawDataTop();

	case_InitOptionCheck();

	case_InitCC();

	case_CheckLock();
	case_CheckCancel();

	case_CalcCondRead();
	case_CalcCondWrite();

	case_BcRead();
	case_BcWrite();

	case_Complex();

	case_GridRead();
	case_GridReadFunc();
	case_GridWrite();

	case_SolStartEnd();

	case_SolWriteStd();
	case_SolWriteDivide();

	case_addGridAndResult();

	case_read_adf();
	case_read_hdf5();

	case_read_adf_no_results();
	case_read_hdf5_no_results();

	return 0;
}
