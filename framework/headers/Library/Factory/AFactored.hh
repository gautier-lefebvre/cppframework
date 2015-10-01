#ifndef		__LIBRARY_FACTORY_AFACTORED_HH__
#define		__LIBRARY_FACTORY_AFACTORED_HH__

#include	<chrono>

namespace	Factory {
	class	AFactored {
	private:
		bool	_valid;
		std::chrono::steady_clock::time_point _lastOutOfPoolTimePoint;

	public:
		AFactored();
		virtual ~AFactored();

	public:
		virtual void reinit() = 0;

	public:
		void	isValid(bool);
		bool	isValid() const;

		void	updateLastOutOfPoolTimePoint();
		const std::chrono::steady_clock::time_point& lastOutOfPoolTimePoint() const;
	};
}

#endif		/* __LIBRARY_FACTORY_AFACTORED_HH__ */