/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once

#include <fstream>
#include <ostream>
#include <cstdint>
#include <cstddef>

#include "../e2e_packhelper.h"

namespace E2E
{

	class DictEntryRawData
	{
	public:
		enum class EntryType { Dir, Data};
		PACKSTRUCT(
		struct DataRawHeader
		{
			uint8_t  mdbdataStr[0x08]; // MDbData + \0
			uint32_t zero1;
			uint32_t checksumDatafield;
		});

		PACKSTRUCT(
		struct Raw
		{
			uint32_t indexAddress;
			uint32_t dataAddress ;
			uint32_t dataLengt   ;

			uint32_t zero        ; // TODO: unknown
			int32_t  patientID   ; // dir + edb
			int32_t  studyID     ; // .edb
			int32_t  seriesID    ; // .sdb
			int32_t  imageID     ;
			int16_t  subID       ;

			uint16_t unknown     ; // not included in checksum, unknown why
			uint32_t type        ;
			uint32_t checksum    ; // sum of block + 0x789ABCDF
		});


		DictEntryRawData(std::ifstream& stream, std::size_t linkAddr, EntryType t);
		DictEntryRawData(const DictEntryRawData& other) = default;

		void print(std::ostream& stream) const;

		bool validChecksum()                                     const;
		bool validIndexEntry()                                   const;
		bool isValid()                                           const { return validIndexEntry() && validChecksum(); }

		std::size_t getFoundAddr()                               const { return foundAddr; }
		uint32_t getCalculatedChecksum()                         const { return calculatedChecksum; }

		const Raw& getRaw()                                      const { return rawdata; }
		const DataRawHeader& getDataRawHeader()                  const { return dataRawHeader; }

		std::size_t getEntryId()                                 const { return id; }
		static void resetEntryCounter()                                { idCounter = 0; }

		bool compare(const DictEntryRawData& other) const;

		constexpr static const std::size_t dirEntryHeaderSize  = sizeof(Raw);
		constexpr static const std::size_t dataEntryHeaderSize = sizeof(DataRawHeader) + sizeof(Raw);

		EntryType getType() const                                      { return type; }

	private:
		Raw           rawdata           ;
		DataRawHeader dataRawHeader     ;
		EntryType     type              ;
		std::size_t   foundAddr         ;
		uint32_t      calculatedChecksum;

		std::size_t id;
		static std::size_t idCounter;

		void calcChecksumDir();
		void calcChecksumData();

		void copyFrom(const DictEntryRawData& other);
	};

	inline std::ostream& operator<<(std::ostream& stream, const DictEntryRawData& ob) { ob.print(stream); return stream; }


}
