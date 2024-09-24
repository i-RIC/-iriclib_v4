#include "error_macros.h"
#include "h5cgnsbase.h"
#include "h5cgnsfile.h"
#include "h5cgnsgridattributes.h"
#include "h5cgnsgridcoordinates.h"
#include "h5cgnszone.h"
#include "iriclib_errorcodes.h"
#include "iriclib.h"
#include "vectorutil.h"

#include "internal/iric_h5cgnsfiles.h"
#include "internal/iric_logger.h"
#include "internal/iric_util.h"
#include "internal/iric_outputerror.h"

#include <sstream>

using namespace iRICLib;

namespace {

int dimOr1(int dim) {
	if (dim == 0) {return 1;}

	return dim;
}

} // namespace

int cg_iRIC_Read_Grid2d_Str_Size_WithGridId(int fid, int gid, int* isize, int* jsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* z;
	int ier = _iric_get_zone(fid, gid, &z, __func__);
	RETURN_IF_ERR;

	if (z->type() == H5CgnsZone::Type::Unstructured) {
		std::ostringstream ss;
		ss << "In cg_iRIC_Read_Grid2d_Str_Size_WithGridId(), grid for fid " << fid << ", gid " << gid << " is an unstructured grid";
		_iric_logger_error(ss.str());
		return IRIC_INVALID_GRIDTYPE;
	}
	if (z->base()->dimension() != 2) {
		std::ostringstream ss;
		ss << "In cg_iRIC_Read_Grid2d_Str_Size_WithGridId(), grid for fid " << fid << ", gid " << gid << " has dimension " << z->base()->dimension();
		_iric_logger_error(ss.str());
		return IRIC_INVALID_DIMENSION;
	}
	const auto& s =  z->size();
	*isize = s.at(0);
	*jsize = s.at(1);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid2d_Coords_WithGridId(int fid, int gid, double* x_arr, double* y_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* z;
	int ier = _iric_get_zone(fid, gid, &z, __func__);
	RETURN_IF_ERR;

	if (z->base()->dimension() != 2) {
		std::ostringstream ss;
		ss << "In cg_iRIC_GetGridCoord2d_WithGridId(), grid for fid " << fid << ", gid " << gid << " has dimension " << z->base()->dimension();
		_iric_logger_error(ss.str());
		return IRIC_INVALID_DIMENSION;
	}
	std::vector<double> xbuffer, ybuffer;

	ier = z->gridCoordinates()->readCoordinatesX(&xbuffer);
	ier = z->gridCoordinates()->readCoordinatesY(&ybuffer);

	_vectorToPointerT(xbuffer, x_arr);
	_vectorToPointerT(ybuffer, y_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid3d_Str_Size_WithGridId(int fid, int gid, int* isize, int* jsize, int* ksize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* z;
	int ier = _iric_get_zone(fid, gid, &z, __func__);
	RETURN_IF_ERR;

	if (z->type() == H5CgnsZone::Type::Unstructured) {
		std::ostringstream ss;
		ss << "In cg_iRIC_Read_Grid3d_Str_Size_WithGridId(), grid for fid " << fid << ", gid " << gid << " is an unstructured grid";
		_iric_logger_error(ss.str());
		return IRIC_INVALID_GRIDTYPE;
	}
	if (z->base()->dimension() != 3) {
		std::ostringstream ss;
		ss << "In cg_iRIC_Read_Grid3d_Str_Size_WithGridId(), grid for fid " << fid << ", gid " << gid << " has dimension " << z->base()->dimension();
		_iric_logger_error(ss.str());
		return IRIC_INVALID_DIMENSION;
	}
	const auto& s =  z->size();
	*isize = s.at(0);
	*jsize = s.at(1);
	*ksize = s.at(2);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid3d_Coords_WithGridId(int fid, int gid, double* x_arr, double* y_arr, double* z_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	if (zone->base()->dimension() != 3) {
		std::ostringstream ss;
		ss << "In cg_iRIC_Read_Grid3d_Coords_WithGridId(), grid for fid " << fid << ", gid " << gid << " has dimension " << zone->base()->dimension();
		_iric_logger_error(ss.str());
		return IRIC_INVALID_DIMENSION;
	}
	std::vector<double> xbuffer, ybuffer, zbuffer;

	ier = zone->gridCoordinates()->readCoordinatesX(&xbuffer);
	RETURN_IF_ERR;
	ier = zone->gridCoordinates()->readCoordinatesY(&ybuffer);
	RETURN_IF_ERR;
	ier = zone->gridCoordinates()->readCoordinatesZ(&zbuffer);
	RETURN_IF_ERR;

	_vectorToPointerT(xbuffer, x_arr);
	_vectorToPointerT(ybuffer, y_arr);
	_vectorToPointerT(zbuffer, z_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_CellType_WithGridId(int fid, int gid, int* type)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	H5CgnsZone::CellType ct;
	ier = zone->readUnstructuredGridCellType(&ct);
	RETURN_IF_ERR;

	*type = static_cast<int> (ct);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_TriangleElementsSize_WithGridId(int fid, int gid, int* size)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	ier = zone->readTriangleElementsSize(size);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_TriangleElementsSize2_WithGridId(int fid, int gid, int* size)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	ier = zone->readTriangleElementsValueCount(size);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_TriangleElements_WithGridId(int fid, int gid, int* id_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> indices;
	ier = zone->readTriangleElements(&indices);
	RETURN_IF_ERR;

	_vectorToPointerT(indices, id_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_LineElementsSize_WithGridId(int fid, int gid, int* size)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	ier = zone->readLineElementsSize(size);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_LineElementsSize2_WithGridId(int fid, int gid, int* size)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	ier = zone->readLineElementsValueCount(size);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_LineElements_WithGridId(int fid, int gid, int* id_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> indices;
	ier = zone->readLineElements(&indices);
	RETURN_IF_ERR;

	_vectorToPointerT(indices, id_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_NodeCount_WithGridId(int fid, int gid, int* count)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	*count = zone->nodeCount();

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_CellCount_WithGridId(int fid, int gid, int* count)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	*count = zone->cellCount();

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_IFaceCount_WithGridId(int fid, int gid, int* count)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	*count = zone->iFaceCount();

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_JFaceCount_WithGridId(int fid, int gid, int* count)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	*count = zone->jFaceCount();

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_KFaceCount_WithGridId(int fid, int gid, int* count)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	*count = zone->kFaceCount();

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Real_Node_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	zone->gridAttributes()->readValue(name, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Integer_Node_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	zone->gridAttributes()->readValue(name, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_StringMaxLen_Node_WithGridId(int fid, int gid, const char* name, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readDimensions(name, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_String_Node_WithGridId(int fid, int gid, const char* name, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readValue(name, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Real_Cell_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readValue(name, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Integer_Cell_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readValue(name, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_StringMaxLen_Cell_WithGridId(int fid, int gid, const char* name, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readDimensions(name, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_String_Cell_WithGridId(int fid, int gid, const char* name, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readValue(name, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Real_IFace_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readValue(name, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Integer_IFace_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readValue(name, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_StringMaxLen_IFace_WithGridId(int fid, int gid, const char* name, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readDimensions(name, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_String_IFace_WithGridId(int fid, int gid, const char* name, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readValue(name, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Real_JFace_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readValue(name, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Integer_JFace_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readValue(name, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_StringMaxLen_JFace_WithGridId(int fid, int gid, const char* name, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readDimensions(name, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_String_JFace_WithGridId(int fid, int gid, const char* name, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readValue(name, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_FunctionalDimensionSize_WithGridId(int fid, int gid, const char* name, const char* dimname, int* count)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	ier = zone->gridAttributes()->readFunctionalDimensionSize(name, dimname, count);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_FunctionalDimension_Integer_WithGridId(int fid, int gid, const char* name, const char* dimname, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readFunctionalDimension(name, dimname, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_FunctionalDimension_Real_WithGridId(int fid, int gid, const char* name, const char* dimname, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readFunctionalDimension(name, dimname, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_FunctionalTimeSize_WithGridId(int fid, int gid, const char* name, int* count)
{
	return cg_iRIC_Read_Grid_FunctionalDimensionSize_WithGridId(fid, gid, name, "Time", count);
}

int cg_iRIC_Read_Grid_FunctionalTime_WithGridId(int fid, int gid, const char* name, double* time_arr)
{
	return cg_iRIC_Read_Grid_FunctionalDimension_Real_WithGridId(fid, gid, name, "Time", time_arr);
}

int cg_iRIC_Read_Grid_Functional_Integer_Node_WithGridId(int fid, int gid, const char* name, int dimid, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Real_Node_WithGridId(int fid, int gid, const char* name, int dimid, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_StringMaxLen_Node_WithGridId(int fid, int gid, const char* name, int dimid, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readFunctionalDimensions(name, dimid, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_String_Node_WithGridId(int fid, int gid, const char* name, int dimid, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readFunctional(name, dimid, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Integer_Cell_WithGridId(int fid, int gid, const char* name, int dimid, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Real_Cell_WithGridId(int fid, int gid, const char* name, int dimid, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_StringMaxLen_Cell_WithGridId(int fid, int gid, const char* name, int dimid, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readFunctionalDimensions(name, dimid, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_String_Cell_WithGridId(int fid, int gid, const char* name, int dimid, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readFunctional(name, dimid, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Integer_IFace_WithGridId(int fid, int gid, const char* name, int dimid, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Real_IFace_WithGridId(int fid, int gid, const char* name, int dimid, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_StringMaxLen_IFace_WithGridId(int fid, int gid, const char* name, int dimid, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readFunctionalDimensions(name, dimid, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_String_IFace_WithGridId(int fid, int gid, const char* name, int dimid, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readFunctional(name, dimid, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Integer_JFace_WithGridId(int fid, int gid, const char* name, int dimid, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_Real_JFace_WithGridId(int fid, int gid, const char* name, int dimid, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer;
	ier = zone->gridAttributes()->readFunctional(name, dimid, &buffer);
	RETURN_IF_ERR;

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_StringMaxLen_JFace_WithGridId(int fid, int gid, const char* name, int dimid, int* smsize)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	ier = zone->gridAttributes()->readFunctionalDimensions(name, dimid, &dims);
	RETURN_IF_ERR;

	*smsize = static_cast<int> (dims.at(1));

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Read_Grid_Functional_String_JFace_WithGridId(int fid, int gid, const char* name, int dimid, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<char> buffer;
	zone->gridAttributes()->readFunctional(name, dimid, &buffer);

	_vectorToPointerT(buffer, v_arr);

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid1d_Coords_WithGridId(int fid, int isize, double* x_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(1);
	std::vector<int> size;
	size.push_back(isize);
	size.push_back(dimOr1(isize - 1));
	auto zone = base->createDefaultZone(H5CgnsZone::Type::Structured, size);

	std::vector<double> coords(isize);

	_pointerToVectorT(x_arr, &coords);

	ier = zone->gridCoordinates()->writeCoordinatesX(coords);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_Grid2d_Coords_WithGridId(int fid, int isize, int jsize, double* x_arr, double* y_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(2);

	std::vector<int> size;
	size.push_back(isize);
	size.push_back(jsize);
	size.push_back(dimOr1(isize - 1));
	size.push_back(dimOr1(jsize - 1));
	auto zone = base->createDefaultZone(H5CgnsZone::Type::Structured, size);

	auto totalSize = isize * jsize;

	std::vector<double> xVec(totalSize), yVec(totalSize);

	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_Grid3d_Coords_WithGridId(int fid, int isize, int jsize, int ksize, double* x_arr, double* y_arr, double* z_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(3);

	std::vector<int> size;
	size.push_back(isize);
	size.push_back(jsize);
	size.push_back(ksize);
	size.push_back(dimOr1(isize - 1));
	size.push_back(dimOr1(jsize - 1));
	size.push_back(dimOr1(ksize - 1));
	auto zone = base->createDefaultZone(H5CgnsZone::Type::Structured, size);

	auto totalSize = isize * jsize * ksize;

	std::vector<double> xVec(totalSize), yVec(totalSize), zVec(totalSize);
	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);
	_pointerToVectorT(z_arr, &zVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesZ(zVec);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_NamedGrid1d_Coords_WithGridId(int fid, const char* name, int isize, double* x_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(1);
	std::vector<int> size;
	size.push_back(isize);
	size.push_back(dimOr1(isize - 1));
	auto zone = base->createZone(name, H5CgnsZone::Type::Structured, size);

	std::vector<double> coords(isize);

	_pointerToVectorT(x_arr, &coords);

	ier = zone->gridCoordinates()->writeCoordinatesX(coords);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_NamedGrid2d_Coords_WithGridId(int fid, const char* name, int isize, int jsize, double* x_arr, double* y_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(2);

	std::vector<int> size;
	size.push_back(isize);
	size.push_back(jsize);
	size.push_back(dimOr1(isize - 1));
	size.push_back(dimOr1(jsize - 1));
	auto zone = base->createZone(name,  H5CgnsZone::Type::Structured, size);

	auto totalSize = isize * jsize;

	std::vector<double> xVec(totalSize), yVec(totalSize);

	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_NamedGrid3d_Coords_WithGridId(int fid, const char* name, int isize, int jsize, int ksize, double* x_arr, double* y_arr, double* z_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(3);

	std::vector<int> size;
	size.push_back(isize);
	size.push_back(jsize);
	size.push_back(ksize);
	size.push_back(dimOr1(isize - 1));
	size.push_back(dimOr1(jsize - 1));
	size.push_back(dimOr1(ksize - 1));
	auto zone = base->createZone(name, H5CgnsZone::Type::Structured, size);

	auto totalSize = isize * jsize * ksize;

	std::vector<double> xVec(totalSize), yVec(totalSize), zVec(totalSize);
	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);
	_pointerToVectorT(z_arr, &zVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesZ(zVec);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_Grid2d_Unst_Triangles_WithGridId(int fid, int psize, double* x_arr, double* y_arr, int csize, int* idx_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(2);
	auto name = base->nextDefaultName();

	ier = cg_iRIC_Write_NamedGrid2d_Unst_Triangles_WithGridId(fid, name.c_str(), psize, x_arr, y_arr, csize, idx_arr, gid);

	_IRIC_LOGGER_TRACE_LEAVE();
	return ier;
}

int cg_iRIC_Write_Grid2d_Unst_Lines_WithGridId(int fid, int psize, double* x_arr, double* y_arr, int csize, int* idx_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(2);
	auto name = base->nextDefaultName();

	ier = cg_iRIC_Write_NamedGrid2d_Unst_Lines_WithGridId(fid, name.c_str(), psize, x_arr, y_arr, csize, idx_arr, gid);

	_IRIC_LOGGER_TRACE_LEAVE();
	return ier;
}

int cg_iRIC_Write_Grid3d_Unst_Lines_WithGridId(int fid, int psize, double* x_arr, double* y_arr, double* z_arr, int csize, int* idx_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(3);
	auto name = base->nextDefaultName();

	ier = cg_iRIC_Write_NamedGrid3d_Unst_Lines_WithGridId(fid, name.c_str(), psize, x_arr, y_arr, z_arr, csize, idx_arr, gid);

	_IRIC_LOGGER_TRACE_LEAVE();
	return ier;
}

int cg_iRIC_Write_NamedGrid2d_Unst_Triangles_WithGridId(int fid, const char* name, int psize, double* x_arr, double* y_arr, int csize, int* idx_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(2);

	std::vector<int> size;
	size.push_back(psize);
	size.push_back(csize);
	auto zone = base->createZone(name, H5CgnsZone::Type::Unstructured, size);

	std::vector<double> xVec(psize), yVec(psize);

	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	std::vector<int> indices(csize * 3);

	_pointerToVectorT(idx_arr, &indices);

	ier = zone->writeTriangleElements(indices);

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_NamedGrid2d_Unst_Lines_WithGridId(int fid, const char* name, int psize, double* x_arr, double* y_arr, int csize, int* idx_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(2);

	std::vector<int> size;
	size.push_back(psize);
	size.push_back(csize);
	auto zone = base->createZone(name, H5CgnsZone::Type::Unstructured, size);

	std::vector<double> xVec(psize), yVec(psize);

	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	std::vector<int> indices(csize * 2);

	_pointerToVectorT(idx_arr, &indices);

	ier = zone->writeLineElements(indices);

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_NamedGrid3d_Unst_Lines_WithGridId(int fid, const char* name, int psize, double* x_arr, double* y_arr, double* z_arr, int csize, int* idx_arr, int* gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsFile* file = nullptr;
	int ier = _iric_h5cgnsfiles_get(fid, &file);
	RETURN_IF_ERR;

	auto base = file->base(3);

	std::vector<int> size;
	size.push_back(psize);
	size.push_back(csize);
	auto zone = base->createZone(name, H5CgnsZone::Type::Unstructured, size);

	std::vector<double> xVec(psize), yVec(psize), zVec(psize);

	_pointerToVectorT(x_arr, &xVec);
	_pointerToVectorT(y_arr, &yVec);
	_pointerToVectorT(z_arr, &zVec);

	ier = zone->gridCoordinates()->writeCoordinatesX(xVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesY(yVec);
	RETURN_IF_ERR;

	ier = zone->gridCoordinates()->writeCoordinatesZ(zVec);
	RETURN_IF_ERR;

	std::vector<int> indices(csize * 2);

	_pointerToVectorT(idx_arr, &indices);

	ier = zone->writeLineElements(indices);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return file->getGridId(zone, gid);
}

int cg_iRIC_Write_Grid_Real_Node_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer(zone->nodeCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Integer_Node_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer(zone->nodeCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_String_Node_WithGridId(int fid, int gid, const char* name, int maxlen, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	dims.push_back(zone->nodeCount());
	dims.push_back(maxlen);
	size_t size = zone->nodeCount() * maxlen;

	std::vector<char> buffer(size);
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer, dims);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Real_Cell_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer(zone->cellCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Integer_Cell_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer(zone->cellCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_String_Cell_WithGridId(int fid, int gid, const char* name, int maxlen, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	dims.push_back(zone->cellCount());
	dims.push_back(maxlen);
	size_t size = zone->cellCount() * maxlen;

	std::vector<char> buffer(size);
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer, dims);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Real_IFace_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer(zone->iFaceCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Integer_IFace_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer(zone->iFaceCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_String_IFace_WithGridId(int fid, int gid, const char* name, int maxlen, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	dims.push_back(zone->iFaceCount());
	dims.push_back(maxlen);
	size_t size = zone->iFaceCount() * maxlen;

	std::vector<char> buffer(size);
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer, dims);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Real_JFace_WithGridId(int fid, int gid, const char* name, double* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<double> buffer(zone->jFaceCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_Integer_JFace_WithGridId(int fid, int gid, const char* name, int* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<int> buffer(zone->jFaceCount());
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Write_Grid_String_JFace_WithGridId(int fid, int gid, const char* name, int maxlen, char* v_arr)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid, gid, &zone, __func__);
	RETURN_IF_ERR;

	std::vector<size_t> dims;
	dims.push_back(zone->jFaceCount());
	dims.push_back(maxlen);
	size_t size = zone->jFaceCount() * maxlen;

	std::vector<char> buffer(size);
	_pointerToVectorT(v_arr, &buffer);

	ier = zone->gridAttributes()->writeValue(name, buffer, dims);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}

int cg_iRIC_Copy_Grid_WithGridId(int fid_from, int fid_to, int gid)
{
	_IRIC_LOGGER_TRACE_ENTER();

	H5CgnsZone* zone;
	int ier = _iric_get_zone(fid_from, gid, &zone, __func__);
	RETURN_IF_ERR;

	H5CgnsFile* toFile;
	ier = _iric_h5cgnsfiles_get(fid_to, &toFile);

	H5CgnsBase* toBase = toFile->base(zone->base()->dimension());
	ier = zone->copyGridTo(toBase);
	RETURN_IF_ERR;

	_IRIC_LOGGER_TRACE_LEAVE();
	return IRIC_NO_ERROR;
}
