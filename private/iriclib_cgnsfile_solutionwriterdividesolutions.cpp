#include "iriclib_cgnsfile_solutionwriterdividesolutions.h"
#include "iriclib_cgnsfile_solutionwriterstandard.h"
#include "iriclib_cgnsfile_impl.h"
#include "../error_macros.h"

#include <cgnslib.h>

#include <assert.h>
#include <string>
#include <sstream>

using namespace iRICLib;

namespace {

const static std::string GRIDCOORD_NAME = "GridCoordinatesForSolution1";
static const std::string ZINAME = "ZoneIterativeData";

int copyBase(int fid_in, int bid_in, int fid_out, int* bid_out)
{
	char baseName[32];
	int celldim, physdim;

	int ier = cg_base_read(fid_in, bid_in, &(baseName[0]), &celldim, &physdim);
	RETURN_IF_ERR;

	return cg_base_write(fid_out, baseName, celldim, physdim, bid_out);
}

int copyZone(int fid_in, int bid_in, int zid_in, int fid_out, int bid_out, int* zid_out)
{
	char zoneName[32];
	cgsize_t zoneSize[9];
	ZoneType_t zoneType;

	int ier = cg_zone_read(fid_in, bid_in, zid_in, &(zoneName[0]), &(zoneSize[0]));
	RETURN_IF_ERR;
	ier = cg_zone_type(fid_in, bid_in, zid_in, &zoneType);
	RETURN_IF_ERR;

	return cg_zone_write(fid_out, bid_out, zoneName, zoneSize, zoneType, zid_out);
}

int linkGrid(const char* filename_src, int fid_src, int bid_src, int zid_src, int gid_src, int fid_tgt, int bid_tgt, int zid_tgt, char* linkname = nullptr)
{
	char name[32];
	int celldim, physdim;
	cgsize_t zoneSize[9];

	std::string path;

	int ier = cg_base_read(fid_src, bid_src, &(name[0]), &celldim, &physdim);
	RETURN_IF_ERR;
	path.append(name);

	ier = cg_zone_read(fid_src, bid_src, zid_src, &(name[0]), &(zoneSize[0]));
	RETURN_IF_ERR;
	path.append("/");
	path.append(name);

	cg_grid_read(fid_src, bid_src, zid_src, gid_src, &(name[0]));
	path.append("/");
	path.append(name);

	if (linkname == nullptr) {
		linkname = &(name[0]);
	}

	ier = cg_goto(fid_tgt, bid_tgt, "Zone_t", zid_tgt, NULL);
	RETURN_IF_ERR;

	return cg_link_write(linkname, filename_src, path.c_str());
}

int linkSolution(const char* filename_src, int fid_src, int bid_src, int zid_src, int sid_src, int fid_tgt, int bid_tgt, int zid_tgt, char* linkname = nullptr)
{
	char name[32];
	int celldim, physdim;
	cgsize_t zoneSize[9];
	GridLocation_t location;

	std::string path;

	int ier = cg_base_read(fid_src, bid_src, &(name[0]), &celldim, &physdim);
	RETURN_IF_ERR;
	path.append(name);

	ier = cg_zone_read(fid_src, bid_src, zid_src, &(name[0]), &(zoneSize[0]));
	RETURN_IF_ERR;
	path.append("/");
	path.append(name);

	ier = cg_sol_info(fid_src, bid_src, zid_src, sid_src, &(name[0]), &location);
	RETURN_IF_ERR;
	path.append("/");
	path.append(name);

	if (linkname == nullptr) {
		linkname = &(name[0]);
	}

	ier = cg_goto(fid_tgt, bid_tgt, "Zone_t", zid_tgt, NULL);
	RETURN_IF_ERR;

	return cg_link_write(linkname, filename_src, path.c_str());
}

std::string solutionFileName(const std::string& origName, int solId)
{
	std::ostringstream oss;

	std::string::size_type pos = origName.find(".");
	oss << origName.substr(0, pos);
	oss << "_Solution" << solId << ".cgn";
	return oss.str();
}

} // namespace

CgnsFile::SolutionWriterDivideSolutions::SolutionWriterDivideSolutions(CgnsFile::Impl* impl) :
	CgnsFile::SolutionWriter(impl),
	m_fileId {0},
	m_baseId {0},
	m_zoneId {0}
{}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Time(double time)
{
	int ier = closeFileIfOpen();
	RETURN_IF_ERR;

	Impl* i = impl();
	ier = CgnsFile::SolutionWriterStandard::stdSolWriteTime(time, i);
	RETURN_IF_ERR;

	m_fileName = solutionFileName(i->m_fileName, i->m_solId);

	ier = setupSolutionFile(m_fileName, i, &m_fileId, &m_baseId, &m_zoneId);
	RETURN_IF_ERR;

	ier = cg_biter_write(m_fileId, m_baseId, CgnsFile::Impl::BINAME.c_str(), 1);
	RETURN_IF_ERR;

	ier = cg_goto(m_fileId, m_baseId, "BaseIterativeData_t", 1, NULL);
	RETURN_IF_ERR;

	cgsize_t dimVec = 1;
	ier = cg_array_write("TimeValues", RealDouble, 1, &dimVec, &time);
	RETURN_IF_ERR;

	ier = cg_ziter_write(m_fileId, m_baseId, m_zoneId, ZINAME.c_str());
	RETURN_IF_ERR;

	std::vector<std::string> sols;
	std::vector<std::string> cellsols;
	std::vector<std::string> ifacesols;
	std::vector<std::string> jfacesols;
	ier = Impl::addSolutionNode(m_fileId, m_baseId, m_zoneId, 1, &sols, &cellsols, &ifacesols, &jfacesols);
	RETURN_IF_ERR;

	// Vertex

	char solname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getSolName(i->m_solId, solname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 1, i->m_fileId, i->m_baseId, i->m_zoneId, solname);
	RETURN_IF_ERR;

	i->m_solPointers.push_back(solname);
	ier = CgnsFile::Impl::writeFlowSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solPointers);
	RETURN_IF_ERR;

	// CellCenter

	char cellsolname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getCellSolName(i->m_solId, cellsolname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 2, i->m_fileId, i->m_baseId, i->m_zoneId, cellsolname);
	RETURN_IF_ERR;

	i->m_cellSolPointers.push_back(cellsolname);
	ier = CgnsFile::Impl::writeFlowCellSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_cellSolPointers);
	RETURN_IF_ERR;

	// IFaceCenter

	char ifacesolname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getIFaceSolName(i->m_solId, ifacesolname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 3, i->m_fileId, i->m_baseId, i->m_zoneId, ifacesolname);
	RETURN_IF_ERR;

	i->m_ifaceSolPointers.push_back(ifacesolname);
	ier = CgnsFile::Impl::writeFlowIFaceSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_ifaceSolPointers);
	RETURN_IF_ERR;

	// JFaceCenter

	char jfacesolname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getJFaceSolName(i->m_solId, jfacesolname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 4, i->m_fileId, i->m_baseId, i->m_zoneId, jfacesolname);
	RETURN_IF_ERR;

	i->m_jfaceSolPointers.push_back(jfacesolname);
	ier = CgnsFile::Impl::writeFlowJFaceSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_jfaceSolPointers);
	RETURN_IF_ERR;

	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Iteration(int index)
{
	int ier = closeFileIfOpen();
	RETURN_IF_ERR;

	Impl* i = impl();
	CgnsFile::SolutionWriterStandard::stdSolWriteIteration(index, i);
	m_fileName = solutionFileName(i->m_fileName, i->m_solId);

	ier = setupSolutionFile(m_fileName, i, &m_fileId, &m_baseId, &m_zoneId);
	RETURN_IF_ERR;

	ier = cg_biter_write(m_fileId, m_baseId, CgnsFile::Impl::BINAME.c_str(), 1);
	cg_goto(m_fileId, m_baseId, "BaseIterativeData_t", 1, NULL);
	cgsize_t dimVec = 1;
	ier = cg_array_write("IterationValues", Integer, 1, &dimVec, &index);
	RETURN_IF_ERR;

	ier = cg_ziter_write(m_fileId, m_baseId, m_zoneId, ZINAME.c_str());
	RETURN_IF_ERR;

	std::vector<std::string> sols;
	std::vector<std::string> cellsols;
	std::vector<std::string> ifacesols;
	std::vector<std::string> jfacesols;
	ier = Impl::addSolutionNode(m_fileId, m_baseId, m_zoneId, 1, &sols, &cellsols, &ifacesols, &jfacesols);
	RETURN_IF_ERR;

	// Vertex

	char solname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getSolName(i->m_solId, solname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 1, i->m_fileId, i->m_baseId, i->m_zoneId, solname);
	RETURN_IF_ERR;

	i->m_solPointers.push_back(solname);
	ier = CgnsFile::Impl::writeFlowSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solPointers);
	RETURN_IF_ERR;

	// CellCenter

	char cellsolname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getCellSolName(i->m_solId, cellsolname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 2, i->m_fileId, i->m_baseId, i->m_zoneId, cellsolname);
	RETURN_IF_ERR;

	i->m_cellSolPointers.push_back(cellsolname);
	ier = CgnsFile::Impl::writeFlowCellSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_cellSolPointers);
	RETURN_IF_ERR;

	// IFaceCenter

	char ifacesolname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getIFaceSolName(i->m_solId, ifacesolname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 3, i->m_fileId, i->m_baseId, i->m_zoneId, ifacesolname);
	RETURN_IF_ERR;

	i->m_ifaceSolPointers.push_back(ifacesolname);
	ier = CgnsFile::Impl::writeFlowIFaceSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_ifaceSolPointers);
	RETURN_IF_ERR;


	// JFaceCenter

	char jfacesolname[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getJFaceSolName(i->m_solId, jfacesolname);

	ier = linkSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 4, i->m_fileId, i->m_baseId, i->m_zoneId, jfacesolname);
	RETURN_IF_ERR;

	i->m_jfaceSolPointers.push_back(jfacesolname);
	ier = CgnsFile::Impl::writeFlowJFaceSolutionPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_jfaceSolPointers);
	RETURN_IF_ERR;

	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_GridCoord2d(double *x, double *y)
{
	std::vector<std::string> coords;
	int ier = Impl::addSolutionGridCoordNode(m_fileId, m_baseId, m_zoneId, 1, &coords);
	RETURN_IF_ERR;

	Impl* i = impl();
	ier = CgnsFile::SolutionWriterStandard::stdSolWriteGridCoord2d(x, y, m_fileId, m_baseId, m_zoneId, 2, i);
	RETURN_IF_ERR;

	char gridName[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getSolGridCoordName(i->m_solId, gridName);
	ier = linkGrid(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 2, i->m_fileId, i->m_baseId, i->m_zoneId, gridName);
	RETURN_IF_ERR;

	i->m_solGridCoordPointers.push_back(gridName);
	return i->writeGridCoordinatesPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solGridCoordPointers);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_GridCoord3d(double *x, double *y, double *z)
{
	std::vector<std::string> coords;
	int ier = Impl::addSolutionGridCoordNode(m_fileId, m_baseId, m_zoneId, 1, &coords);
	RETURN_IF_ERR;

	Impl* i = impl();
	ier = CgnsFile::SolutionWriterStandard::stdSolWriteGridCoord3d(x, y, z, m_fileId, m_baseId, m_zoneId, 2, i);
	RETURN_IF_ERR;

	char gridName[CgnsFile::Impl::NAME_MAXLENGTH];
	CgnsFile::Impl::getSolGridCoordName(i->m_solId, gridName);
	ier = linkGrid(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 2, i->m_fileId, i->m_baseId, i->m_zoneId, gridName);
	RETURN_IF_ERR;

	i->m_solGridCoordPointers.push_back(gridName);
	return i->writeGridCoordinatesPointers(i->m_fileId, i->m_baseId, i->m_zoneId, i->m_solGridCoordPointers);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Integer(const char *name, int* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::VERTEX_SOLUTION_ID, Integer, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Cell_Integer(const char *name, int* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::CELL_SOLUTION_ID, Integer, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_IFace_Integer(const char *name, int* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::IFACE_2D_SOLUTION_ID, Integer, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_JFace_Integer(const char *name, int* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::JFACE_2D_SOLUTION_ID, Integer, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Real(const char *name, double* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::VERTEX_SOLUTION_ID, RealDouble, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_Cell_Real(const char *name, double* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::CELL_SOLUTION_ID, RealDouble, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_IFace_Real(const char *name, double* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::IFACE_2D_SOLUTION_ID, RealDouble, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Write_JFace_Real(const char *name, double* data)
{
	int F;
	return cg_field_write(m_fileId, m_baseId, m_zoneId, CgnsFile::Impl::JFACE_2D_SOLUTION_ID, RealDouble, name, data, &F);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Particle_Write_Pos2d(cgsize_t count, double* x, double* y)
{
	SolutionWriterStandard::stdSolParticleWritePos2d(count, x, y, m_fileId, m_baseId, m_zoneId, 1);
	Impl* i = impl();
	char linkname[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(i->m_solId, linkname);
	return linkParticleSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 1, i->m_fileId, i->m_baseId, i->m_zoneId, linkname);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Particle_Write_Pos3d(cgsize_t count, double* x, double* y, double* z)
{
	SolutionWriterStandard::stdSolParticleWritePos3d(count, x, y, z, m_fileId, m_baseId, m_zoneId, 1);
	Impl* i = impl();
	char linkname[Impl::NAME_MAXLENGTH];
	Impl::getParticleSolName(i->m_solId, linkname);
	return linkParticleSolution(m_fileName.c_str(), m_fileId, m_baseId, m_zoneId, 1, i->m_fileId, i->m_baseId, i->m_zoneId, linkname);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Particle_Write_Real(const char* name, double* value)
{
	return SolutionWriterStandard::stdSolParticleWriteReal(name, value, m_fileId, m_baseId, m_zoneId, 1);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_Particle_Write_Integer(const char* name, int* value)
{
	return SolutionWriterStandard::stdSolParticleWriteInteger(name, value, m_fileId, m_baseId, m_zoneId, 1);
}

int CgnsFile::SolutionWriterDivideSolutions::Sol_PolyData_Write_GroupEnd()
{
	return stdSolPolyDataGroupEnd(m_fileId, m_baseId, m_zoneId, 1);
}

int CgnsFile::SolutionWriterDivideSolutions::Flush()
{
	return closeFileIfOpen();
}

int CgnsFile::SolutionWriterDivideSolutions::closeFileIfOpen()
{
	if (m_fileId == 0) {return 0;}

	int ier = cg_close(m_fileId);
	RETURN_IF_ERR;
	m_fileId = 0;

	return 0;
}

int CgnsFile::SolutionWriterDivideSolutions::setupSolutionFile(const std::string& solFileName, CgnsFile::Impl* i, int* fileId, int* baseId, int* zoneId)
{
	int fileType;
	int ier = cg_get_file_type(i->m_fileId, &fileType);
	RETURN_IF_ERR;
	cg_set_file_type(fileType);

	ier = cg_open(solFileName.c_str(), CG_MODE_WRITE, fileId);
	RETURN_IF_ERR;

	ier = copyBase(i->m_fileId, i->m_baseId, *fileId, baseId);
	RETURN_IF_ERR;

	ier = copyZone(i->m_fileId, i->m_baseId, i->m_zoneId, *fileId, *baseId, zoneId);
	RETURN_IF_ERR;

	ier = linkGrid(i->m_fileName.c_str(), i->m_fileId, i->m_baseId, i->m_zoneId, 1, *fileId, *baseId, *zoneId, nullptr);
	RETURN_IF_ERR;

	cg_close(*fileId);

	cg_set_file_type(CG_FILE_NONE);

	return cg_open(solFileName.c_str(), CG_MODE_MODIFY, fileId);
}

int CgnsFile::SolutionWriterDivideSolutions::linkParticleSolution(const char* filename_src, int fid_src, int bid_src, int zid_src, int sid_src, int fid_tgt, int bid_tgt, int zid_tgt, const char* linkname)
{
	char name[32];
	int celldim, physdim;
	cgsize_t zoneSize[9];

	std::string path;

	int ier = cg_base_read(fid_src, bid_src, &(name[0]), &celldim, &physdim);
	RETURN_IF_ERR;
	path.append(name);

	ier = cg_zone_read(fid_src, bid_src, zid_src, &(name[0]), &(zoneSize[0]));
	RETURN_IF_ERR;
	path.append("/");
	path.append(name);

	CgnsFile::Impl::getParticleSolName(sid_src, name);
	path.append("/");
	path.append(name);

	if (linkname == nullptr) {
		linkname = &(name[0]);
	}

	ier = cg_goto(fid_tgt, bid_tgt, "Zone_t", zid_tgt, NULL);
	RETURN_IF_ERR;

	return cg_link_write(linkname, filename_src, path.c_str());
}
