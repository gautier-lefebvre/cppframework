#ifndef		__LIBRARY_FACTORY_AFACTORED_HH__
#define		__LIBRARY_FACTORY_AFACTORED_HH__

namespace	Factory {
	class	AFactored {
	private:
		bool	_valid;

	public:
		AFactored();
		virtual ~AFactored();

	public:
		virtual void reinit() = 0;

	public:
		bool	isValid() const;
		void	isValid(bool);
	};
}

#endif		/* __LIBRARY_FACTORY_AFACTORED_HH__ */