#pragma once

#include <libmpq/libmpq/mpq.h>

struct SMPQFileLocation
{
	SMPQFileLocation() :
		archive(nullptr),
		fileNumber(0),
		exists(false)
	{}

	SMPQFileLocation(mpq_archive* _archive, uint32 _fileNumber) :
		archive(_archive),
		fileNumber(_fileNumber),
		exists(true)
	{}

	bool exists;
	mpq_archive* archive;
	uint32 fileNumber;
};

class ZN_API CMPQFilesStorage 
	: public IznFilesStorage
{
public:
	CMPQFilesStorage(std::string _path);
	virtual ~CMPQFilesStorage();

	// IFilesStorage
	std::shared_ptr<IFile>  OpenFile(std::string FileName, EFileAccessType FileAccessType = EFileAccessType::Read) override;
	bool                    SaveFile(std::shared_ptr<IFile> File) override;
	size_t                  GetFileSize(std::string FileName) const override;
	bool                    IsFileExists(std::string FileName) const override;

	// CMPQFilesStorage
	void AddArchive(std::string _filename);
	SMPQFileLocation GetFileLocation(const std::string& _filename) const ;

private:
	const std::string           m_Path;

	std::vector<mpq_archive_s*> m_OpenArchives;
	mutable std::mutex m_Lock;
};