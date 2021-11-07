#pragma once

#include <vector>
#include "FileData.h"

enum {
    SORTBYNAME = 0, SORTBYNAMEINV,
    SORTBYRATING, SORTBYRATINGINV,
    SORTBYTIMESPLAYED, SORTBYTIMESPLAYEDINV,
    SORTBYLASTPLAYED, SORTBYLASTPLAYEDINV,
    SORTBYGENRE, SORTBYGENREINV,
    SORTBYRELEASEDATE, SORTBYRELEASEDATEINV,
    SORTBYRELEASEYEAR, SORTBYRELEASEYEARINV
};

namespace FileSorts
{
	bool compareFileName(const FileData* file1, const FileData* file2);
	bool compareRating(const FileData* file1, const FileData* file2);
	bool compareTimesPlayed(const FileData* file1, const FileData* fil2);
	bool compareLastPlayed(const FileData* file1, const FileData* file2);
    bool compareGenre(const FileData* file1, const FileData* file2);
    bool compareReleaseDate(const FileData* file1, const FileData* file2);
    bool compareReleaseYear(const FileData* file1, const FileData* file2);

	bool compareFileNameInv(const FileData* file1, const FileData* file2);
	bool compareRatingInv(const FileData* file1, const FileData* file2);
	bool compareTimesPlayedInv(const FileData* file1, const FileData* fil2);
	bool compareLastPlayedInv(const FileData* file1, const FileData* file2);
    bool compareGenreInv(const FileData* file1, const FileData* file2);
    bool compareReleaseDateInv(const FileData* file1, const FileData* file2);
    bool compareReleaseYearInv(const FileData* file1, const FileData* file2);

    extern const std::vector<FileData::SortType> SortTypes;
};
