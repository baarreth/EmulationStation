#include "FileSorts.h"

namespace FileSorts
{
	const FileData::SortType typesArr[] = {
		FileData::SortType(&compareFileName, true, "filename, ascending"),
		FileData::SortType(&compareFileName, false, "filename, descending"),

		FileData::SortType(&compareRating, true, "rating, ascending"),
		FileData::SortType(&compareRating, false, "rating, descending"),

		FileData::SortType(&compareTimesPlayed, true, "times played, ascending"),
		FileData::SortType(&compareTimesPlayed, false, "times played, descending"),

		FileData::SortType(&compareLastPlayed, true, "last played, ascending"),
		FileData::SortType(&compareLastPlayed, false, "last played, descending"),

        FileData::SortType(&compareGenre, true, "genre, ascending"),
        FileData::SortType(&compareGenre, false, "genre, descending"),

        FileData::SortType(&compareReleaseDate, true, "release date, ascending"),
        FileData::SortType(&compareReleaseDate, false, "release date, descending")
	};

	const std::vector<FileData::SortType> SortTypes(typesArr, typesArr + sizeof(typesArr)/sizeof(typesArr[0]));

	//returns if file1 should come before file2
	bool compareFileName(const FileData* file1, const FileData* file2)
	{
        const std::string & name1 = file1->getName();
        const std::string & name2 = file2->getName();
        unsigned int count = name1.length() > name2.length() ? name2.length() : name1.length();
        auto n1 = name1.begin();
        auto n2 = name2.begin();
        for(unsigned int i = 0; i < count; i++, n1++, n2++)
        {
            if(tolower(*n1) != tolower(*n2)) return tolower(*n1) < tolower(*n2);
        }
        return name1.length() < name2.length();
	}

	bool compareRating(const FileData* file1, const FileData* file2)
	{
		//only games have rating metadata
		if(file1->metadata.getType() == GAME_METADATA && file2->metadata.getType() == GAME_METADATA)
		{
			return file1->metadata.getFloat("rating") < file2->metadata.getFloat("rating");
		}
		return false;
	}

	bool compareTimesPlayed(const FileData* file1, const FileData* file2)
	{
		//only games have playcount metadata
		if(file1->metadata.getType() == GAME_METADATA && file2->metadata.getType() == GAME_METADATA)
		{
			return (file1)->metadata.getInt("playcount") < (file2)->metadata.getInt("playcount");
		}
		return false;
	}

	bool compareLastPlayed(const FileData* file1, const FileData* file2)
	{
		//only games have lastplayed metadata
		if(file1->metadata.getType() == GAME_METADATA && file2->metadata.getType() == GAME_METADATA)
		{
			return (file1)->metadata.getTime("lastplayed") < (file2)->metadata.getTime("lastplayed");
		}
		return false;
	}

    inline bool compareStrings(const std::string & s1, const std::string & s2, unsigned int count = 0) {
        if(!count) count = s1.length() > s2.length() ? s2.length() : s1.length();
        auto n1 = s1.begin(), n2 = s2.begin();
        for(unsigned int i = 0; i < count; i++, n1++, n2++) if(*n1 != *n2) return *n1 < *n2;
        return s1.length() < s2.length();
    }

    bool compareGenre(const FileData* file1, const FileData* file2)
    {
        if(file1->metadata.getType() == GAME_METADATA && file2->metadata.getType() == GAME_METADATA)
        {
            return compareStrings((file1)->metadata.get("genre"), (file2)->metadata.get("genre"));
        }
        return false;
    }

    bool compareReleaseDate(const FileData* file1, const FileData* file2)
    {
        if(file1->metadata.getType() == GAME_METADATA && file2->metadata.getType() == GAME_METADATA)
        {
            return compareStrings((file1)->metadata.get("releasedate"), (file2)->metadata.get("releasedate"), 6);
        }
        return false;
    }
};
