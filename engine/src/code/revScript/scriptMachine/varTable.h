////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on June 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine's variables table

#ifndef _REV_SCRIPT_SCRIPTMACHINE_VARTABLE_H_
#define _REV_SCRIPT_SCRIPTMACHINE_VARTABLE_H_

#include <vector.h>
#include <revCore/variant/variant.h>

namespace rev { namespace script
{
	class CVarTable
	{
	public:
		CVarTable() : mFirstFreeSlot(0) {}	// Constructor

		void		lockVar		(unsigned _idx);	// Gets ownership of the var stored at _idx;
		void		releaseVar	(unsigned _idx);	// Relinquishes ownership of the var stored at _idx.
		unsigned	newVar		();					// Requests a new var. It gives you the ownership of the requested var
		void		getRawValue	(unsigned _idx, CVariant& _dst) const;	// Get the immediate value of the variable.
									// If the variable stores a list of indices to other variables, this call won't perform
									// the substitution, it will return the raw list of indices
		void		setRawValue	(unsigned _idx, const CVariant& _v); // Set the raw value of a variable. It clears any
									// Previous value stored in it.
		void		getLiteral	(unsigned _idx, CVariant& _dst) const;
									// Get the value of the requested variable to _dst. If the variable is a vector, this
									// method will automatically perform recursive substitutions to gather all literals inside.
		void		setLiteral	(unsigned _idx, const CVariant& _v); // Set a literal value to a variable.
									// If the literal is a vector, each element of it will be stored in an independent variable
	private:
		void		clearSlot	(unsigned _idx); // Clears the content of the slot and releases any dependencies
		unsigned	getFirstEmptySlot(); // Return the position of the first empty variable slot in the table (creates one if
									// necessary).
		unsigned	nextFreeSlot(unsigned _idx); // Returns the first free slot after (not including) given index

	private:
		typedef rtl::pair<unsigned, CVariant>	slotT;	// pair<references, value>
		typedef rtl::vector<slotT>				tableT;	// table of slots

		// Data store
		tableT	mTable;	// Table of variables. Contains variable values and references
		// Allocation cache
		unsigned mFirstFreeSlot; // Index pointing to the first empty slot in the table
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_SCRIPTMACHINE_VARTABLE_H_