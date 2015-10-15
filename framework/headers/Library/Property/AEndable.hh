#ifndef		__LIBRARY_PROPERTY_AENDABLE_HH__
#define		__LIBRARY_PROPERTY_AENDABLE_HH__

/**
 *	\class AEndable Library/Property/AEndable.hh
 *	\brief Abstract class from which all classes than can be stopped by SIGINT inherit.
 */
class AEndable {
protected:
	bool	_end; /*!< set to true when the child class must end. */

public:
	/**
	 *	\brief Constructor of AEndable.
	 *
	 *	Sets the end state to false.
	 */
	AEndable();

	/**
	 *	\brief Destructor of AEndable.
	 */
	virtual ~AEndable();

public:
	/**
	 *	\brief Tells the child class that it must stop.
	 */
	virtual void	end() = 0;

public:
	/**
	 *	\brief Checks if the end method has been called.
	 *	\return true if the end method has been called.
	 */
	bool	mustEnd() const;

	/**
	 *	\brief Changes the end states of the object.
	 *	\param state the new state.
	 */
	void	mustEnd(bool state);
};

#endif		/* __LIBRARY_PROPERTY_AENDABLE_HH__ */
