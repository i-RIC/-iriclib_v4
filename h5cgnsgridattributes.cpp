#include "error_macros.h"
#include "h5cgnsgridattributes.h"
#include "h5groupcloser.h"
#include "iriclib_errorcodes.h"

#include "internal/iric_logger.h"

#include "private/h5cgnsgridattributes_impl.h"

#include <sstream>

#define CHECK_NAME_EXISTS \
	if (impl->m_names.find(name) == impl->m_names.end()) {\
		std::ostringstream ss;\
		ss << "In H5CgnsGridAttributes::" << __func__ << "(), data with name " << name << " does not exist";\
		_iric_logger_error(ss.str());\
		return IRIC_DATA_NOT_FOUND;\
	}

using namespace iRICLib;

namespace {

std::string dimensionDataName(const std::string& dimName)
{
	std::ostringstream ss;
	ss << "Dimension_" << dimName;

	return ss.str();
}

std::string dimensionValueName(int dimId)
{
	if (dimId == 1) {
		return "Value";
	}

	std::ostringstream ss;
	ss << "Value" << (dimId - 1);

	return ss.str();
}

template <typename V>
int readDataArray(hid_t groupId, const std::string& name, const std::string& valName, std::vector<V>* values)
{
	hid_t gId;
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::openGroup");
	int ier = H5Util::openGroup(groupId, name, H5Util::userDefinedDataLabel(), &gId);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::openGroup", ier);
	RETURN_IF_ERR;

	H5GroupCloser closer(gId);

	std::unordered_set<std::string> tmp_names;

	H5Util::getGroupNames(gId, &tmp_names);
	if (tmp_names.find(valName) == tmp_names.end()) {
		return  IRIC_DATA_NOT_FOUND;
	}

	_IRIC_LOGGER_TRACE_CALL_START("H5Util::readDataArrayValue");
	ier = H5Util::readDataArrayValue(gId, valName, values);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::readDataArrayValue", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

template <typename V>
int createDataArray(hid_t groupId, const std::string& name, const std::string& valName, const std::vector<V>& values)
{
	hid_t gId;
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::openOrCreateUserDefinedDataGroup");
	int ier = H5Util::createUserDefinedDataGroup(groupId, name, &gId);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::openOrCreateUserDefinedDataGroup", ier);
	RETURN_IF_ERR;

	H5GroupCloser closer(gId);

	_IRIC_LOGGER_TRACE_CALL_START("H5Util::createDataArray");
	ier = H5Util::createDataArray(gId, valName, values);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::createDataArray", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

template <typename V>
int updateDataArray(hid_t groupId, const std::string& name, const std::string& valName, const std::vector<V>& values)
{
	hid_t gId;
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::openOrCreateUserDefinedDataGroup");
	int ier = H5Util::openGroup(groupId, name, H5Util::userDefinedDataLabel(), &gId);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::openOrCreateUserDefinedDataGroup", ier);
	RETURN_IF_ERR;

	H5GroupCloser closer(gId);

	_IRIC_LOGGER_TRACE_CALL_START("H5Util::createDataArray");
	ier = H5Util::createDataArray(gId, valName, values);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::createDataArray", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

template <typename V>
int updateOrCreateDataArray(hid_t groupId, const std::string& name, const std::string& valName, const std::vector<V>& values, std::unordered_set<std::string>* names)
{
	if (names->find(name) == names->end()) {
		int ier = createDataArray(groupId, name, valName, values);
		RETURN_IF_ERR;
		names->insert(name);
	} else {
		int ier = updateDataArray(groupId, name, valName, values);
		RETURN_IF_ERR;
	}
	return IRIC_NO_ERROR;
}

} // namespace

H5CgnsGridAttributes::H5CgnsGridAttributes(hid_t groupId, H5CgnsZone* zone) :
	impl {new Impl {}}
{
	impl->m_groupId = groupId;
	impl->m_zone = zone;

	H5Util::getGroupNames(impl->m_groupId, &(impl->m_names));
}

H5CgnsGridAttributes::~H5CgnsGridAttributes()
{
	delete impl;
}

int H5CgnsGridAttributes::getValueNames(std::vector<std::string>* names) const
{
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::getGroupNames");
	int ier = H5Util::getGroupNames(impl->m_groupId, names);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::getGroupNames", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::getValueNames(std::unordered_set<std::string>* names) const
{
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::getGroupNames");
	int ier = H5Util::getGroupNames(impl->m_groupId, names);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::getGroupNames", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::getValueType(const std::string& name, H5Util::DataArrayValueType *type) const
{
	CHECK_NAME_EXISTS;

	hid_t gId;
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::openGroup");
	int ier = H5Util::openGroup(impl->m_groupId, name, H5Util::userDefinedDataLabel(), &gId);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::openGroup", ier);
	RETURN_IF_ERR;

	H5GroupCloser closer(gId);

	_IRIC_LOGGER_TRACE_CALL_START("H5Util::readDataArrayValueType");
	ier = H5Util::readDataArrayValueType(gId, "Value", type);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::readDataArrayValueType", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::readValue(const std::string& name, std::vector<int>* values) const
{
	CHECK_NAME_EXISTS;

	return readDataArray(impl->m_groupId, name, "Value", values);
}

int H5CgnsGridAttributes::readValue(const std::string& name, std::vector<double>* values) const
{
	CHECK_NAME_EXISTS;

	return readDataArray(impl->m_groupId, name, "Value", values);
}

int H5CgnsGridAttributes::readFunctionalDimensionSize(const std::string& name, const std::string& dimname, int* size) const
{
	CHECK_NAME_EXISTS;

	hid_t gId;
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::openGroup");
	int ier = H5Util::openGroup(impl->m_groupId, name, H5Util::userDefinedDataLabel(), &gId);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::openGroup", ier);
	RETURN_IF_ERR;

	H5GroupCloser closer(gId);

	std::unordered_set<std::string> tmp_names;

	H5Util::getGroupNames(gId, &tmp_names);
	auto dimDataName = dimensionDataName(dimname);
	if (tmp_names.find(dimDataName) == tmp_names.end()) {
		// data does not exist;
		*size = 0;
		return  IRIC_DATA_NOT_FOUND;
	}

	_IRIC_LOGGER_TRACE_CALL_START("H5Util::readDataArrayLength");
	ier = H5Util::readDataArrayLength(gId, dimDataName, size);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5Util::readDataArrayLength", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::readFunctionalDimension(const std::string& name, const std::string& dimname, std::vector<int>* values) const
{
	CHECK_NAME_EXISTS;

	return readDataArray(impl->m_groupId, name, dimensionDataName(dimname), values);
}

int H5CgnsGridAttributes::readFunctionalDimension(const std::string& name, const std::string& dimname, std::vector<double>* values) const
{
	CHECK_NAME_EXISTS;

	return readDataArray(impl->m_groupId, name, dimensionDataName(dimname), values);
}

int H5CgnsGridAttributes::readFunctional(const std::string& name, int dimid, std::vector<int>* values) const
{
	CHECK_NAME_EXISTS;

	return readDataArray(impl->m_groupId, name, dimensionValueName(dimid), values);
}

int H5CgnsGridAttributes::readFunctional(const std::string& name, int dimid, std::vector<double>* values) const
{
	CHECK_NAME_EXISTS;

	return readDataArray(impl->m_groupId, name, dimensionValueName(dimid), values);
}

int H5CgnsGridAttributes::writeValue(const std::string& name, const std::vector<int>& values) const
{
	int ier = updateOrCreateDataArray(impl->m_groupId, name, "Value", values, &(impl->m_names));
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::writeValue(const std::string& name, const std::vector<double>& values) const
{
	int ier = updateOrCreateDataArray(impl->m_groupId, name, "Value", values, &(impl->m_names));
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::writeFunctionalDimension(const std::string& name, const std::string& dimname, const std::vector<int>& values) const
{
	if (values.size() == 0) {return IRIC_NO_ERROR;}

	int ier = updateOrCreateDataArray(impl->m_groupId, name, dimensionDataName(dimname), values, &(impl->m_names));
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::writeFunctionalDimension(const std::string& name, const std::string& dimname, const std::vector<double>& values) const
{
	if (values.size() == 0) {return IRIC_NO_ERROR;}

	int ier = updateOrCreateDataArray(impl->m_groupId, name, dimensionDataName(dimname), values, &(impl->m_names));
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::writeFunctional(const std::string& name, int dimid, const std::vector<int>& values) const
{
	int ier = updateOrCreateDataArray(impl->m_groupId, name, dimensionValueName(dimid), values, &(impl->m_names));
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::writeFunctional(const std::string& name, int dimid, const std::vector<double>& values) const
{
	int ier = updateOrCreateDataArray(impl->m_groupId, name, dimensionValueName(dimid), values, &(impl->m_names));
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

int H5CgnsGridAttributes::copyTo(H5CgnsGridAttributes* target)
{
	int ier;
	_IRIC_LOGGER_TRACE_CALL_START("H5Util::copyGroupRecursively");
	ier = H5Util::copyGroupRecursively(impl->m_groupId, target->impl->m_groupId);
	_IRIC_LOGGER_TRACE_CALL_END_WITHVAL("H5CgnsZoneBc::Impl::loadBcs", ier);
	RETURN_IF_ERR;

	return IRIC_NO_ERROR;
}

H5CgnsZone* H5CgnsGridAttributes::zone() const
{
	return impl->m_zone;
}
