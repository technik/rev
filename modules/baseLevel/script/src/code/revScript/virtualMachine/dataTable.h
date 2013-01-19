//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 19th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's data table

#ifndef _REV_SCRIPT_VIRTUALMACHINE_DATATABLE_H_
#define _REV_SCRIPT_VIRTUALMACHINE_DATATABLE_H_

#include <vector>
#include "../variant/variant.h"

namespace rev { namespace script {

	class DataTable
	{
	public:
		unsigned	addData		(const Variant& _x);					// Notice adding data makes you the owner of it
		void		releaseData	(unsigned _index);						// Relinquish ownership of the variable
		void		consultData (unsigned _index, Variant& _dst) const;	// Same as getData but without requesting ownership of the data
		void		getData		(unsigned _index, Variant& _dst);		// Read data at _index position, store it into _dst and request ownership of the variable
		void		setData		(unsigned _index, const Variant& _src);

	private:
		unsigned	firstEmptyIndex();
	private:
		std::vector<std::pair<Variant, unsigned> >	mTable;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_VIRTUALMACHINE_DATATABLE_H_